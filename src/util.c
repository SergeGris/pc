
#include "pc.h"

#include "util.h"

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <math.h>

#include "xalloc.h"

#include "system.h"

static const char digit_tab[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void
round_result (char *buf, mp_prec_t digits, bool fixed)
{
  char *s, *k, *dot;
  mp_prec_t n;
  size_t i;

  if (*buf == '+' || *buf == '-')
    buf++;
  s = buf;
  dot = 0;
  /* Find the first not zero digit.  */
  for (;; s++)
    {
      if (*s == '0')
        continue;
      if (*s == '.')
        {
          dot = s;
          continue;
        }
      break;
    }
  if (dot != NULL && fixed)
    {
      n = fix_digits + 1 - (ptrdiff_t) (s - dot);
      if (n <= 0)
        n = 1;
      if (digits > n)
        digits = n;
    }
  /* Find the last digit.  */
  for (;; s++)
    {
      if (*s == '\0' || *s == ' ')
        {
          k = s;
          if (dot == NULL)
            return;
          goto ltrunc;
        }
      if (*s == '.')
        {
          dot = s;
          if (fixed && digits > fix_digits)
            digits = fix_digits;
          continue;
        }
      if (digits-- <= 0 && dot != NULL)
        break;
    }
  i = *s - '0';
  if (i > 9)
    i = *s - 'A' + 10;
  if (i > 35)
    i = *s - 'a' + 10;
  k = s;
  if (i >= ((base + 1) >> 1))
    {
      /* Increment.  */
      while (true)
        {
          if (s == buf)
            {
              /* Insert 1 at the beginning.  */
              i = strlen (buf);
              memmove (buf + 1, buf, i + 1);
              *s = '1';
              k++;
              break;
            }
          s--;
          if (*s == '.')
            s--;
          if (base > 10 ? (*s != 'A' + base - 11 && *s != 'a' + base - 11)
                        : (*s != '0' + base - 1))
            {
              if (*s == '9')
                *s = digit_tab[10];
              else
                (*s)++;
              break;
            }
          *s = '0';
        }
    }
ltrunc:
  /* Truncate trailing zeros.  */
  k--;
  while (k > buf && *k == '0')
    k--;
  /* Delete trailing dot.  */
  if (*k != '.')
    k++;
  /* Move exponent.  */
  for (s = k; *s != '\0' && *s != ' '; s++)
    ;
  strcpy (k, s);
}

void
pretty_result (char *buf)
{
  char *s, *d, *b1, *b2, *e1, *e2;
  ptrdiff_t n1, n2, a, f1, f2;
  size_t i;

  if (!use_separator1 && !use_separator2)
    return;

  aminmax (separator_freq1, 1, 10000);
  aminmax (separator_freq2, 1, 10000);

  f1 = separator_freq1;
  f2 = separator_freq2;

  if (base == 2 || base == 16)
    {
      for (i = 1; i < f1; i <<= 1)
        ;
      f1 = i;
      for (i = 1; i < f2; i <<= 1)
        ;
      f2 = i;
    }

  b1 = buf;
  if (*b1 == '+' || *b1 == '-')
    b1++;
  for (s = b1; *s != '\0' && *s != ' ' && *s != '.'; s++)
    ;
  e1 = s;
  if (*s == '.')
    s++;
  b2 = s;
  for (; *s && *s != ' '; s++)
    ;
  e2 = s;
  n1 = (ptrdiff_t) (e1 - b1);
  n2 = (ptrdiff_t) (e2 - b2);
  a = 0;
  if (use_separator1)
    a += (n1 - 1) / f1;
  if (use_separator2)
    a += (n2 - 1) / f2;
  i = strlen (s) + 1;
  d = s + a;
  memmove (d, s, i);
  i = 0;
  if (use_separator2)
    {
      i = n2 % f2;
      if (i == 0)
        i = f2;
      i++;
    }
  while (s > b2)
    {
      if (--i == 0)
        {
          *--d = (char) separator2;
          i = f2;
        }
      *--d = *--s;
    }
  if (b2 != e1)
    *--d = *--s;
  i = use_separator1 ? f1 + 1 : 0;
  for (; s > b1;)
    {
      if (--i == 0)
        {
          *--d = (char) separator1;
          i = f1;
        }
      *--d = *--s;
    }
}

/*
 * Bits - decimal digits relation
 *   bits   = ceil (digits * log[2](10))
 *   digits = floor (bits * log[10](2))
 */
mp_prec_t
digits2bits (mp_prec_t digits)
{
  static_assert (sizeof (mp_prec_t) >= sizeof (double),
                 "sizeof (mp_prec_t) < sizeof (double)");
  static_assert ((mp_prec_t)(double) LLONG_MAX == (mp_prec_t) LLONG_MAX,
                 "(mp_prec_t)(double) LLONG_MAX != (mp_prec_t) LLONG_MAX");

  return (mp_prec_t) ceil ((double) digits * 3.321928094887362347870);
}
mp_prec_t
bits2digits (mp_prec_t bits)
{
  static_assert (sizeof (mp_prec_t) >= sizeof (double),
                 "sizeof (mp_prec_t) < sizeof (double)");
#if _DEBUG
  mp_prec_t MPPREC_MAX = LONG_MAX;
  double DBL_MAX = LONG_MAX;
  assert (MPPREC_MAX == (mp_prec_t) DBL_MAX);
#endif

  return (mp_prec_t) floor ((double) bits * 0.301029995663981195214);
}

void
skip_whitespace (char **s)
{
  while (isspace ((unsigned char) **s))
    (*s)++;
}

bool
is_latin_alpha (int c)
{
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

void
strip_spaces (char **s)
{
  unsigned char *result = *s;
  size_t i = 0, j = 0;

  while (isspace (result[i]))
    i++;

  for (; i < strlen (result); i++)
    if (!isspace (result[i]))
      result[j++] = result[i];
    else if (!isspace (result[j - 1]))
      result[j++] = ' ';

  if (result[j - 1] == ' ')
    result[--j] = '\0';
  else
    result[j] = '\0';

  *s = xrealloc (result, j);
}

static const char *pc_errors[] =
{
  [0] = "cerrno == 0",
#define SET(i, s) [i] = s
# include "errors.def"
#undef SET
};

void
cerror (unsigned int id)
{
  assert (id < countof (pc_errors));

  if (cerrno != 0 || id == 0)
   return;

  cerrno = id;

  error (0, 0, "%u: %s", id, pc_errors[id]);
}

#undef __OUTPUT_ERROR_TEXT__
