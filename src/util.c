
#include "pc.h"

#include "util.h"

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <math.h>
#include <unistd.h>

#include "die.h"
#include "intprops.h"
#include "verify.h"
#include "quote.h"
#include "xalloc.h"

#include "system.h"

static const char digit_tab[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void
round_result (char *buf, mpfr_prec_t digits, bool fixed)
{
  char *s, *k, *dot;
  mpfr_prec_t n;
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

  aminmax (separator_freq1, 1u, 10000u);
  aminmax (separator_freq2, 1u, 10000u);

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
  for (; *s != '\0' && *s != ' '; s++)
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
  while (s > b1)
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
mpfr_prec_t
digits2bits (mpfr_prec_t digits)
{
  static_assert (sizeof (mpfr_prec_t) >= sizeof (double),
                 "sizeof (mpfr_prec_t) < sizeof (double)");

  return (mpfr_prec_t) ceil ((double) digits * 3.321928094887362347870);
}
mpfr_prec_t
bits2digits (mpfr_prec_t bits)
{
  static_assert (sizeof (mpfr_prec_t) >= sizeof (double),
                 "sizeof (mpfr_prec_t) < sizeof (double)");

  return (mpfr_prec_t) floor ((double) bits * 0.301029995663981195214);
}

/* Find last (most-significant) set bit.  */
static unsigned long int
fls (unsigned long int x)
{
  return (sizeof (x) * CHAR_BIT) - 1 - __builtin_clzl (x);
}

#if ULONG_MAX == UINT64_MAX
unsigned int
fast_isqrt (unsigned long int n)
{
  unsigned long int b, m, y;
  y = 0;
  m = 1UL << (fls (n) & ~1UL);

  while (m != 0)
    {
      b = y + m;
      y >>= 1;
      if (n >= b)
        {
          n -= b;
          y += m;
        }
      m >>= 2;
    }

  return y;
}
#elif ULONG_MAX == UINT32_MAX
unsigned int
fast_isqrt (unsigned long int n)
{
  unsigned int x = (n / 0x3F + 0x3F) >> 1;
  x = (n / x + x) >> 1;
  x = (n / x + x) >> 1;
  return x;
}
#endif

bool
is_prime (unsigned long int n)
{
  for (unsigned long int i = 2; i <= fast_isqrt (n); i++)
    if (n % i == 0)
      return false;
  return true;
}

void
skip_whitespace (unsigned char **s)
{
  while (isspace (**s))
    (*s)++;
}

void
strip_extra_spaces (char **s)
{
  if (s == NULL || *s == NULL)
    return;

  unsigned char *result = *s;
  size_t i = 0;

  while (isspace (result[i]))
    i++;
  if (result[i] == '\0')
    {
      result[0] = '\0';
      *s = xrealloc (result, 1);
      return;
    }

  size_t j;
  for (j = 0; i < strlen (result); i++)
    {
      if (!isspace (result[i]))
        result[j++] = result[i];
      else if (result[j - 1] != ' ' && result[j + 1] != '\0')
        result[j++] = ' ';
    }
  result[j] = '\0';

  *s = xrealloc (result, j);
}

static const char *pc_errors[] =
{
  [0] = "this message should not have been printed (cerrno == 0)",
#define SET(i, s) [i] = s
# include "errors.def"
#undef SET
};

void
cerror (unsigned int id)
{
  assert (id < countof (pc_errors));

  if (__get_cerrno () != 0 || id == 0)
    return;
  __set_cerrno (id);

  error (0, 0, "%u: %s", id, pc_errors[id]);
}

void
cerrorf (unsigned int id, const char *fmt, ...)
{
  assert (id < countof (pc_errors));
  assert (fmt != NULL);

  if (__get_cerrno () != 0 || id == 0)
    return;
  __set_cerrno (id);

  const char *cerr = pc_errors[id];
  va_list arg_ptr;
  va_start (arg_ptr, fmt);

  char *p = malloc (strlen (fmt) + INT_STRLEN_BOUND (unsigned int) + strlen (cerr) + sizeof (": : : \n"));
  sprintf (p, "%s: %u: %s: %s\n", program_name, id, cerr, fmt);
  vfprintf (stderr, p, arg_ptr);
  free (p);

  va_end (arg_ptr);
}

mpfr_rnd_t
string2rounding (const char *s)
{
  if (*s != '\0')
    {
      if (*(s + 1) == '\0')
        {
          switch (toupper (*s))
            {
            case 'N':
              /* Round to nearest, with ties to even.  */
              return MPFR_RNDN;
            case 'Z':
              /* Round toward zero.  */
              return MPFR_RNDZ;
            case 'U':
              /* Round toward +Inf.  */
              return MPFR_RNDU;
            case 'D':
              /* Round toward -Inf.  */
              return MPFR_RNDD;
            case 'A':
              /* Round away from zero.  */
              return MPFR_RNDA;
            case 'F':
              /* Faithful rounding.  */
              return MPFR_RNDF;
            default:
              break;
            }
        }
      else if (strcasecmp (s, "NA") == 0)
        /* Round to nearest, with ties away from zero.  */
        return MPFR_RNDNA;
    }

  yyerror ("unknown mpfr rounding mode %s", quote (s));
  return mpfr_get_default_rounding_mode ();
}

/*
 * Safe strncpy, the result is always a valid
 * NUL-terminated string that fits in the buffer
 * (unless, of course, the buffer size is zero).
 */
size_t
strlcpy (char *dst, const char *src, size_t n)
{
  if (n != 0 && *src != '\0')
    {
      size_t len = strnlen (src, n);
      memcpy (dst, src, len);
      dst[len] = '\0';
      return len;
    }
  return 0;
}

/* The standard yyerror routine.  Built with variable number of argumnets.  */
void
yyerror (const char *str, ...)
{
  va_list args;

  va_start (args, str);
  vfprintf (stderr, str, args);
  va_end (args);

  fputc ('\n', stderr);
  if (ferror (stderr) != 0)
    die (EXIT_FAILURE, errno, "stderr");
}

void
welcome (void)
{
  puts ("This is free software with ABSOLUTELY NO WARRANTY.");
  printf ("For more details type %s.\n", quote ("warranty"));
}

void
version (void)
{
  printf ("%s %s\n", PROGRAM_NAME, VERSION);
  fputs (_("\
Copyright © "COPYRIGHT_YEAR" Sergey Sushilin.\n\
\n\
License GNU General Public License version 3 <https://gnu.org/licenses/gpl-3.0-standalone.html>.\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n\
\n\
"), stdout);
  const char *const authors[] = { AUTHORS, (const char *) 0 };
  size_t n_authors = countof (authors) - 1;
  verify (countof (authors) >= 1 + 1 /* NULL we count too.  */);

  if (n_authors == 1)
    printf (_("Written by %s.\n"), authors[0]);
  else
    {
      fputs (_("Written by "), stdout);
      for (size_t i = 0; i < n_authors - 1; i++)
        printf ("%s,%c", authors[i], i % 3 == 0 ? '\n' : ' ');
      printf (_("and %s.\n"), authors[n_authors - 1]);
    }
}

void
warranty (void)
{
  fputs ("\
\n\
Copyright © "COPYRIGHT_YEAR" Sergey Sushilin\n\
\n\
This program is free software: you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation, version 3.\n\
\n\
This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with this program.  If not, see <https://www.gnu.org/licenses/gpl-3.0-standalone.html>.\n\
\n\
", stdout);
}
