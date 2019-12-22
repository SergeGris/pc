
#include "pc.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>

#include "realis.h"
#include "util.h"

#include "system.h"

#include "die.h"
#include "intprops.h"
#include "xalloc.h"

/*
 * Some info about ronna, quecca and bundecca:
 *
 * en: https://www.sciencemag.org/news/2019/02/you-know-kilo-mega-and-giga-metric-system-ready-ronna-and-quecca
 * ru: https://habr.com/ru/post/440364/
 */
static struct postfix_t
{
  char c;
  int e;
} postfix_tab[] = {
  { 'b', -11 }, // Bundecto
  { 'q', -10 }, // Quecto
  { 'r', -9 },  // Ronto
  { 'y', -8 },  // Yocto
  { 'z', -7 },  // Zepto
  { 'a', -6 },  // Atto
  { 'f', -5 },  // Femto
  { 'p', -4 },  // Piko
  { 'n', -3 },  // Nano
  { 'u', -2 },  // Mikro
  { 'm', -1 },  // Mili

  { 'k', +1 },  // Kilo
  { 'M', +2 },  // Mega
  { 'G', +3 },  // Giga
  { 'T', +4 },  // Tera
  { 'P', +5 },  // Peta
  { 'X', +6 },  // Exa
  { 'Z', +7 },  // Zeta
  { 'Y', +8 },  // Yotta
  { 'R', +9 },  // Ronna
  { 'Q', +10 }, // Quecca
  { 'B', +11 }  // Bundecca
};

#define MPFR_CHECK_ZERO(y)           \
  if (MPFR_EXP (y) < -MPFR_PREC (y)) \
    MPFR_SET_ZERO (y);
#define CONST_FUNCTION_BODY(x, func) \
  mpfr_##func ((x), mpfr_get_default_rounding_mode ());
#define UNARY_FUNCTION_BODY(y, x, func)                                  \
  do                                                                     \
    {                                                                    \
      mpfr_set ((y), (x), mpfr_get_default_rounding_mode ());            \
      mpfr_set_prec ((y), max (MPFR_EXP (x), mpfr_get_default_prec ())); \
      mpfr_##func ((y), (x), mpfr_get_default_rounding_mode ());         \
      MPFR_CHECK_ZERO (y);                                               \
    }                                                                    \
  while (0)
#define UNARY_FUNCTION_BODY_I(y, n, func)                         \
  do                                                              \
    {                                                             \
      mpfr_##func ((y), (n), mpfr_get_default_rounding_mode ());  \
      MPFR_CHECK_ZERO (y);                                        \
    }                                                             \
  while (0)
#define BINARY_FUNCTION_BODY(y, a, b, func)                             \
  do                                                                    \
    {                                                                   \
      /*mpfr_set_prec (y, max (MPFR_EXP (a), MPFR_EXP (b)));*/          \
      mpfr_##func ((y), (a), (b), mpfr_get_default_rounding_mode ());   \
      MPFR_CHECK_ZERO (y);                                              \
    }                                                                   \
  while (0)

#define UNARY_FUNCTION_BODY_ZI(y, n, func)                              \
  do                                                                    \
    {                                                                   \
      mpz_t yz;                                                         \
      mpz_init2 (yz, MPFR_PREC (y));                                    \
      mpz_##func (yz, n);                                               \
      mpz2mpfr (y, yz);                                                 \
      mpz_clear (yz);                                                   \
    }                                                                   \
  while (0)
#define UNARY_FUNCTION_BODY_Z(y, x, func)                               \
  do                                                                    \
    {                                                                   \
      mpz_t yz, xz;                                                     \
      mpz_init2 (yz, MPFR_PREC (y));                                    \
      mpz_init2 (xz, MPFR_PREC (x));                                    \
                                                                        \
      mpfr2mpz (xz, x);                                                 \
      mpz_##func (yz, xz);                                              \
                                                                        \
      mpz_clear (xz);                                                   \
      mpz2mpfr (y, yz);                                                 \
      mpz_clear (yz);                                                   \
    }                                                                   \
  while (0)
#define BINARY_FUNCTION_BODY_Z(y, a, b, func)                           \
  do                                                                    \
    {                                                                   \
      mpz_t yz, az, bz;                                                 \
      mpz_init2 (yz, MPFR_PREC (y));                                    \
      mpz_init2 (az, MPFR_PREC (a));                                    \
      mpz_init2 (bz, MPFR_PREC (b));                                    \
                                                                        \
      mpfr2mpz (az, a);                                                 \
      mpfr2mpz (bz, b);                                                 \
      mpz_##func (yz, az, bz);                                          \
                                                                        \
      mpz_clear (bz);                                                   \
      mpz_clear (az);                                                   \
      mpz2mpfr (y, yz);                                                 \
      mpz_clear (yz);                                                   \
    }                                                                   \
  while (0)

/* x *= base^e */
static void
eex (mpfr_ptr x, long int e, int base)
{
  mpfr_t t, u;
  realis_init_set_si (t, base);
  realis_init (u);

  BINARY_FUNCTION_BODY (u, t, e, pow_si);
  realis_mul (x, x, u);

  realis_clear (u);
  realis_clear (t);
}

void
realis_init (mpfr_ptr x)
{
  mpfr_init (x);
  MPFR_SET_ZERO (x);
}
void
realis_init2 (mpfr_ptr x, mp_prec_t p)
{
  mpfr_init2 (x, p);
  MPFR_SET_ZERO (x);
}
void
realis_inits (mpfr_ptr x, ...)
{
  va_list arg_ptr;
  va_start (arg_ptr, x);
  while (x != NULL)
    {
      realis_init (x);
      x = va_arg (arg_ptr, mpfr_ptr);
    }
  va_end (arg_ptr);
}
void
realis_clear (mpfr_ptr x)
{
  mpfr_clear (x);
}
void
realis_clears (mpfr_ptr x, ...)
{
  va_list arg_ptr;
  va_start (arg_ptr, x);
  while (x != NULL)
    {
      realis_clear (x);
      x = va_arg (arg_ptr, mpfr_ptr);
    }
  va_end (arg_ptr);
}
void
realis_set_si (mpfr_ptr x, signed long int n)
{
  UNARY_FUNCTION_BODY_I (x, n, set_si);
}
void
realis_set_ui (mpfr_ptr x, unsigned long int n)
{
  UNARY_FUNCTION_BODY_I (x, n, set_ui);
}
void
realis_set_z (mpfr_ptr y, mpz_srcptr z)
{
  UNARY_FUNCTION_BODY_I (y, z, set_z);
}
signed long int
realis_get_si (mpfr_srcptr x)
{
  return CONST_FUNCTION_BODY (x, get_si);
}
unsigned long int
realis_get_ui (mpfr_srcptr x)
{
  return CONST_FUNCTION_BODY (x, get_ui);
}
void
realis_init_set (mpfr_ptr x, mpfr_srcptr n)
{
  UNARY_FUNCTION_BODY (x, n, init_set);
}
void
realis_init_set_si (mpfr_ptr x, signed long int n)
{
  UNARY_FUNCTION_BODY_I (x, n, init_set_si);
}
void
realis_init_set_ui (mpfr_ptr x, unsigned long int n)
{
  UNARY_FUNCTION_BODY_I (x, n, init_set_ui);
}
void
realis_set_zero (mpfr_ptr x)
{
  MPFR_SET_ZERO (x);
}
void
realis_set_inf (mpfr_ptr x, int sign)
{
  mpfr_set_inf (x, sign);
}

void
realis_read_postfix (mpfr_ptr y, const char *s)
{
  if (is_latin_alpha (*s) && !is_latin_alpha (*(s + 1)))
    {
      if (*s == '+' || *s == '-')
        s++;

      if (base <= 10 ? toupper (*s) == 'E' : toupper (*s) == 'P')
        {
          /* Exponent.  */
          s++;
          eex (y, strtol (s, NULL, base), base);
        }
      else if (enable_postfix)
        {
          /* Engineer symbols.  */
          for (size_t i = 0; i <= countof (postfix_tab); i++)
            {
              if (postfix_tab[i].c == *s)
                {
                  s++;
                  eex (y, postfix_tab[i].e, 1000);
                  break;
                }
            }
        }
    }
}
char *
realis_read (mpfr_ptr y, const char *buf)
{
  char *s = NULL;
  mpfr_strtofr (y, buf, &s, base, mpfr_get_default_rounding_mode ());
  return s;
}

/* Output in stdout for a while.  */
char *
realis_write (char *buf, mpfr_srcptr x)
{
  if (mpfr_nan_p (x))
    {
      strcpy (buf, "@NaN@");
      return buf;
    }
  if (mpfr_inf_p (x))
    {
      strcpy (buf, MPFR_IS_POS (x) ? "@Inf@" : "-@Inf@");
      return buf;
    }
  if (MPFR_IS_ZERO (x))
    {
      *buf++ = '0';
      if (number_format == MODE_SCI)
        strcpy (buf, " E+0");
      else
        *buf = '\0';
      return buf;
    }

  mpfr_prec_t prec = bits2digits (mpfr_get_prec (x)) - 1;

#if 0
  mpfr_exp_t e;
  char *s = mpfr_get_str (NULL, &e, base, prec, x, mpfr_get_default_rounding_mode ());
  strcpy (buf, s);
  puts (buf);
  mpfr_free_str (s);

  round_result (buf, prec, number_format == MODE_FIX);
  puts (buf);
  pretty_result (buf);
#endif
#if 1
  /* TODO: use base not equal 10.  */
  char *format = xmalloc (INT_STRLEN_BOUND (mpfr_exp_t) + 5 + 1);

  mpfr_sprintf (format, "%%.%PuR*%c", prec > 0 ? prec : 1, number_format);

  if (mpfr_sprintf (buf, format, mpfr_get_default_rounding_mode (), x) < 0)
    die (EXIT_FAILURE, errno, _("failed to write buffer in function %s"), __func__);

  free (format);

  round_result (buf, prec, number_format == MODE_FIX);
  pretty_result (buf);

  return buf;
#endif
#if 0
  mpfr_exp_t e;
  char *s = mpfr_get_str (NULL, &e, base, prec, x, mpfr_get_default_rounding_mode ());

  char *s0 = s;
  /* for op=3.1416 we have s = "31416" and e = 1 */

  /* outputs possible sign and significand */
  if (*s == '-')
    *buf++ = *s++;
  *buf++ = *s++; /* leading digit */
  *buf++ = '.';
  *buf++ = *s++; /* trailing significand */
  mpfr_free_str (s0);

  e--; /* Due to the leading digit.  */

  round_result (buf, prec, number_format == MODE_FIX && e == 0);

  /* Outputs exponent.  */
  if (e != 0)
    {
      assert (e >= LONG_MIN);
      assert (e <= LONG_MAX);

      int r = sprintf (buf, (base <= 10 ? "e%zi" : "@%zi"), (ssize_t) e);
      if (unlikely (r < 0))
        error (EXIT_FAILURE, errno, _("failed to write buffer in function %s"), __func__);
    }

  pretty_result (buf);
#endif

  /* realis_clear (x); */
}
mpfr_prec_t
realis_len (mpfr_srcptr x)
{
  return bits2digits (mpfr_get_prec (x))
    + 1 /* possible sign */
    + 1 /* possible dot */
    + 1 /* possible sign before exponent */
    + 1 /* possible exponent char */
    + INT_STRLEN_BOUND (mp_exp_t) /* possible exponent number */;
}

void
realis_copy (mpfr_ptr y, mpfr_srcptr x)
{
  mpfr_prec_t xp = mpfr_get_prec (x);

  if (mpfr_get_prec (y) != xp)
    mpfr_set_prec (y, xp);

  UNARY_FUNCTION_BODY (y, x, set);
}

void
realis_neg (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, neg);
}
void
realis_abs (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, abs);
}
void
realis_sign (mpfr_ptr y, mpfr_srcptr x)
{
  realis_set_si (y, mpfr_sgn (x));
}

void
realis_round (mpfr_ptr y, mpfr_srcptr x)
{
  mpfr_round (y, x);
}
void
realis_ceil (mpfr_ptr y, mpfr_srcptr x)
{
  mpfr_ceil (y, x);
}
void
realis_floor (mpfr_ptr y, mpfr_srcptr x)
{
  mpfr_floor (y, x);
}
void
realis_trunc (mpfr_ptr y, mpfr_srcptr x)
{
  mpfr_trunc (y, x);
}
void
realis_frac (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, frac);
}

int
realis_cmp (mpfr_srcptr a, mpfr_srcptr b)
{
  return mpfr_cmp (a, b);
}
int
realis_cmpabs (mpfr_srcptr a, mpfr_srcptr b)
{
  return mpfr_cmpabs (a, b);
}

void
realis_add (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  BINARY_FUNCTION_BODY (y, a, b, add);
}
void
realis_add_ui (mpfr_ptr y, mpfr_srcptr a, unsigned long int n)
{
  BINARY_FUNCTION_BODY (y, a, n, add_ui);
}
void
realis_sub (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  BINARY_FUNCTION_BODY (y, a, b, sub);
}
void
realis_sub_ui (mpfr_ptr y, mpfr_srcptr a, unsigned long int n)
{
  BINARY_FUNCTION_BODY (y, a, n, sub_ui);
}
void
realis_ui_sub (mpfr_ptr y, unsigned long int n, mpfr_srcptr a)
{
  BINARY_FUNCTION_BODY (y, n, a, ui_sub);
}
void
realis_mul (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  BINARY_FUNCTION_BODY (y, a, b, mul);
}
void
realis_mul_ui (mpfr_ptr y, mpfr_srcptr x, unsigned long int n)
{
  BINARY_FUNCTION_BODY (y, x, n, mul_ui);
}
void
realis_div (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  if (MPFR_IS_ZERO (b))
    {
      cerror (division_by_zero);
      return;
    }

  BINARY_FUNCTION_BODY (y, a, b, div);
}
void
realis_div_ui (mpfr_ptr y, mpfr_srcptr x, unsigned long int n)
{
  if (n == 0)
    {
      cerror (division_by_zero);
      return;
    }

  BINARY_FUNCTION_BODY (y, x, n, div_ui);
}
void
realis_pow (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  if (MPFR_IS_ZERO (a))
    {
      if (MPFR_IS_ZERO (b))
        {
          cerror (zero_power_of_zero);
          return;
        }
      if (MPFR_IS_NEG (b))
        {
          cerror (negative_power_of_zero);
          return;
        }
      realis_set_zero (y);
    }
  if (MPFR_IS_NEG (a))
    {
      if (!mpfr_integer_p (b))
        {
          cerror(not_integer_power_of_negative_number);
          return;
        }
    }

  BINARY_FUNCTION_BODY (y, a, b, pow);
}
void
realis_pow_si (mpfr_ptr y, mpfr_srcptr a, signed long int b)
{
  if (MPFR_IS_ZERO (a))
    {
      if (b == 0)
        {
          cerror (zero_power_of_zero);
          return;
        }
      if (b < 0)
        {
          cerror (negative_power_of_zero);
          return;
        }
      realis_set_zero (y);
    }

  BINARY_FUNCTION_BODY (y, a, b, pow_si);
}
void
realis_pow_ui (mpfr_ptr y, mpfr_srcptr a, unsigned long int b)
{
  if (MPFR_IS_ZERO (a))
    {
      if (b == 0)
        {
          cerror (zero_power_of_zero);
          return;
        }
      realis_set_zero (y);
    }

  BINARY_FUNCTION_BODY (y, a, b, pow_ui);
}
void
realis_root (mpfr_ptr y, mpfr_srcptr b, mpfr_srcptr a)
{
  bool sign = MPFR_IS_NEG (a);
  bool b_is_odd = false;
  if (MPFR_IS_ZERO (a))
    {
      if (MPFR_IS_NEG (b))
        cerror (negative_root_of_zero);
      else if (MPFR_IS_ZERO (b))
        cerror (zero_root_of_zero);
      else
        realis_set_zero (y);
      return;
    }
  if (sign)
    {
      if (!mpfr_integer_p (b))
        {
          cerror (not_integer_root_of_negative_number);
          return;
        }
      else if (mpfr_fits_ulong_p (b, mpfr_get_default_rounding_mode ()))
        {
          if (realis_get_ui (b) % 2 == 0)
            {
              cerror (even_root_of_negative_number);
              return;
            }
          else
            b_is_odd = true;
        }
      else
        {
          mpfr_t t;
          realis_init (t);
          realis_div_ui (t, b, 2);
          if (!mpfr_integer_p (t))
            {
              cerror (even_root_of_negative_number);
              return;
            }
          else
            b_is_odd = true;
          realis_clear (t);
        }
    }
  if (mpfr_cmp_si (b, +2) == 0)
    {
      realis_sqrt (y, a);
      return;
    }
  if (mpfr_cmp_si (b, -2) == 0)
    {
      realis_sqrt (y, a);
      MPFR_CHANGE_SIGN (y);
      return;
    }
  if (mpfr_cmp_si (b, +1) == 0)
    {
      realis_copy (y, a);
      return;
    }
  if (mpfr_cmp_si (b, -1) == 0)
    {
      realis_inv (y, a);
      return;
    }

  /* TODO!!! */
  mpfr_t t, u;
  realis_init (t);
  realis_init (u);
  realis_copy (t, a);
  if (MPFR_IS_NEG (t))
    MPFR_CHANGE_SIGN (t);

  realis_ln (u, t);
  realis_div (t, u, b);
  realis_exp (y, t);

  if (MPFR_IS_NEG (t) && b_is_odd)
    MPFR_CHANGE_SIGN (y);
}

void
realis_sqr (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, sqr);
}
void
realis_sqrt (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, sqrt);
}
void
realis_cbrt (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, cbrt);
}

void
realis_rsh (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  if (!mpfr_integer_p (b))
    {
      cerror (shift_operand_is_not_integer);
      return;
    }

  mpfr_t t;
  realis_init_set_ui (t, 2);
  realis_pow (t, t, b);
  realis_div (y, a, t);
  realis_clear (t);
}
void
realis_lsh (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  if (!mpfr_integer_p (b))
    {
      cerror (shift_operand_is_not_integer);
      return;
    }

  mpfr_t t;
  realis_init_set_ui (t, 2);
  realis_pow (t, t, b);
  realis_mul (y, a, t);
  realis_clear (t);
}

void
realis_exp (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, exp);
}
void
realis_ln (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, log);
}

void
realis_agm (mpfr_ptr z, mpfr_srcptr x0, mpfr_srcptr y0)
{
  BINARY_FUNCTION_BODY (z, x0, y0, agm);
}

void
realis_inv (mpfr_ptr y, mpfr_srcptr x)
{
  realis_div (y, one, x);
}

/* Trigonometric functions.  */
void
realis_sin (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, sin);
}
void
realis_cos (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, cos);
}
void
realis_tan (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, tan);
}
void
realis_cot (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, cot);
}
void
realis_sec (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, sec);
}
void
realis_csc (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, csc);
}

/* Inverse trigonometric functions.  */
void
realis_asin (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, asin);
}
void
realis_acos (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, acos);
}
void
realis_atan (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, atan);
}
void
realis_atan2 (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  BINARY_FUNCTION_BODY (y, a, b, atan2);
}
void
realis_acot (mpfr_ptr y, mpfr_srcptr x)
{
  realis_inv (y, x);
  realis_atan (y, y);
}

/* Hyperbolic functions.  */
void
realis_sinh (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, sinh);
}
void
realis_cosh (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, cosh);
}
void
realis_tanh (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, tanh);
}
void
realis_coth (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, coth);
}

/* Factorials.  */
void
realis_fact (mpfr_ptr y, mpfr_srcptr x)
{
  if (MPFR_IS_NEG (x))
    {
      cerror (function_does_not_support_negative);
      return;
    }
  if (!mpfr_integer_p (x))
    {
      cerror (function_does_not_support_fractional);
      return;
    }
  if (!mpfr_fits_ulong_p (x, mpfr_get_default_rounding_mode ()))
    {
      cerror (overflow);
      return;
    }

  UNARY_FUNCTION_BODY_I (y, realis_get_ui (x), fac_ui);
}
void
realis_ffact (mpfr_ptr y, mpfr_srcptr x)
{
  if (MPFR_IS_NEG (x))
    {
      cerror (function_does_not_support_negative);
      return;
    }
  if (!mpfr_integer_p (x))
    {
      cerror (function_does_not_support_fractional);
      return;
    }
  if (!mpfr_fits_ulong_p (x, mpfr_get_default_rounding_mode ()))
    {
      cerror (overflow);
      return;
    }

  unsigned long int n = realis_get_ui (x);
  if (n <= 3)
    {
      realis_set_ui (y, n);
      return;
    }

  mpfr_t t, u;
  realis_inits (t, u, (mpfr_ptr) 0);
  if (n % 2 == 0)
    {
      n /= 2;
      realis_set_ui (t, n);
      realis_lsh (u, one, t);
      UNARY_FUNCTION_BODY_I (t, n, fac_ui);
      realis_mul (y, u, t);
    }
  else
    {
      mpfr_t v, r;
      realis_inits (v, r, (mpfr_ptr) 0);

      n = (n - 1) / 2;
      realis_set_ui (t, n);
      realis_lsh (u, one, t);
      UNARY_FUNCTION_BODY_I (t, n, fac_ui);
      realis_mul (r, t, u);
      UNARY_FUNCTION_BODY_I (t, (n + 1) * 2, fac_ui);
      realis_div (y, t, r);

      realis_clears (r, v, (mpfr_ptr) 0);
    }
  realis_clears (u, t, (mpfr_ptr) 0);
}
void
realis_subfact (mpfr_ptr y, mpfr_srcptr x)
{
  mpfr_t t, e;
  mpfr_inits (t, e, (mpfr_ptr) 0);

  realis_fact (t, x);
  realis_e (e);
  realis_div (y, t, e);
  realis_round (y, y);

  mpfr_clears (e, t, (mpfr_ptr) 0);
}
void
realis_primorial (mpfr_ptr y, mpfr_srcptr x)
{
  if (MPFR_IS_NEG (x))
    {
      cerror (function_does_not_support_negative);
      return;
    }
  if (!mpfr_integer_p (x))
    {
      cerror (function_does_not_support_fractional);
      return;
    }
  if (!mpfr_fits_ulong_p (x, mpfr_get_default_rounding_mode ()))
    {
      cerror (overflow);
      return;
    }

  UNARY_FUNCTION_BODY_ZI (y, realis_get_ui (x), primorial_ui);
}
void
realis_superfactorial (mpfr_ptr y, mpfr_srcptr x)
{
  if (MPFR_IS_NEG (x))
    {
      cerror (function_does_not_support_negative);
      return;
    }
  if (!mpfr_integer_p (x))
    {
      cerror (function_does_not_support_fractional);
      return;
    }
  if (!mpfr_fits_ulong_p (x, mpfr_get_default_rounding_mode ()))
    {
      cerror (overflow);
      return;
    }

  unsigned long int n = realis_get_ui (x);
  realis_set_ui (y, 1);
  mpfr_t t;
  for (unsigned long int k = 1; k <= n; k++)
    {
      realis_init_set_ui (t, k);
      realis_pow_ui (t, t, n - k + 1);
      realis_mul (y, y, t);
    }
}
void
realis_hyperfactorial (mpfr_ptr y, mpfr_srcptr x)
{
  if (MPFR_IS_NEG (x))
    {
      cerror (function_does_not_support_negative);
      return;
    }
  if (!mpfr_integer_p (x))
    {
      cerror (function_does_not_support_fractional);
      return;
    }
  if (!mpfr_fits_ulong_p (x, mpfr_get_default_rounding_mode ()))
    {
      cerror (overflow);
      return;
    }

  unsigned long int n = realis_get_ui (x);
  realis_set_ui (y, 1);
  mpfr_t t;
  for (unsigned long int i = 2; i <= n; i++)
    {
      realis_init_set_ui (t, i);
      realis_superfactorial (t, t);
      realis_mul (y, y, t);
    }
}

void
realis_gcd (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  BINARY_FUNCTION_BODY_Z (y, a, b, gcd);
}
void
realis_lcm (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  BINARY_FUNCTION_BODY_Z (y, a, b, lcm);
}

/*
  !Modulus and Remainder are different functions!

  i.e.

  13 rem (-5) = 3
  (-13) rem 5 = -3

  13 mod (-5) = -2
  (-13) mod 5 = 2

  They give different result if signs is not same.
*/
static inline void
modrem_impl (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b, bool is_mod)
{
  if (MPFR_IS_ZERO (b))
    {
      cerror (division_by_zero);
      return;
    }
  if (!mpfr_integer_p (a) || !mpfr_integer_p (b))
    {
      cerror (modulo_division_by_non_integer_number);
      return;
    }
  if (mpfr_equal_p (a, b))
    {
      realis_set_zero (y);
      return;
    }

  mpfr_sign_t sign = MPFR_SIGN (b);

  mpfr_t t;
  realis_init (t);
  realis_div (t, a, b);
  realis_floor (t, t);
  realis_mul (t, t, b);
  realis_sub (y, a, t);
  realis_clear (t);

  if (is_mod && MPFR_SIGN (a) != MPFR_SIGN (b))
    {
      if (MPFR_SIGN (a) == MPFR_SIGN_POS)
        realis_add_ui (y, y, 1);
      else /* MPFR_SIGN (a) == MPFR_SIGN_NEG */
        realis_sub_ui (y, y, 1);
    }

  MPFR_SIGN (y) = sign;
}

void
realis_mod (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  modrem_impl (y, a, b, true);
}
void
realis_rem (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  modrem_impl (y, a, b, false);
}

void
realis_fibonacci (mpfr_ptr y, mpfr_srcptr x)
{
  if (MPFR_IS_NEG (x))
    {
      cerror (function_does_not_support_negative);
      return;
    }
  if (!mpfr_integer_p (x))
    {
      cerror (function_does_not_support_fractional);
      return;
    }
  if (!mpfr_fits_ulong_p (x, mpfr_get_default_rounding_mode ()))
    {
      cerror (overflow);
      return;
    }

  UNARY_FUNCTION_BODY_ZI (y, realis_get_ui (x), fib_ui);
}

// tribonacci(x)
// a1 = (19 + 3 * sqrt(33))^(1 / 3);
// a2 = (19 - 3 * sqrt(33))^(1 / 3);
// b  = (586 + 102 * sqrt(33))^(1 / 3);
// round(3 * b * ((1 / 3 * (a1 + a2 + 1))^x / (b^2 - 2 * b + 4)))
void
realis_tribonacci (mpfr_ptr y, mpfr_srcptr x)
{
  mpfr_t sqrt297;
  mpfr_t a1, a2, b, t, u;
  realis_inits (sqrt297, a1, a2, b, t, u, (mpfr_ptr) 0);

  realis_set_ui (t, 297); /* 3 * sqrt(33) == sqrt(297) */
  realis_sqrt (t, t);
  realis_mul_ui (sqrt297, t, 3);

  // a1 = (19 + 3 * sqrt(33))^(1 / 3)
  realis_add_ui (a1, sqrt297, 19);
  realis_cbrt (a1, a1);

  // a2 = (19 - 3 * sqrt(33))^(1 / 3)
  realis_ui_sub (a2, 19, sqrt297);
  realis_cbrt (a2, a2);

  // b = (586 + 102 * sqrt(33))^(1 / 3)
  realis_mul_ui (b, sqrt297, 33); /* 102 * sqrt(33) == 33 * sqrt(297) */ realis_clear (sqrt297);
  realis_add_ui (b, b, 586);
  realis_cbrt (b, b);

  // round(3 * b * ((1 / 3 * (a1 + a2 + 1))^x / (b^2 - 2 * b + 4)))

  // u = (1 / 3 * (a1 + a2 + 1))^x
  realis_add (t, a1, a2); realis_clears (a1, a2, (mpfr_ptr) 0);
  realis_add_ui (t, t, 1);
  realis_div_ui (t, t, 3);
  realis_pow (u, t, x);

  // u /= (b^2 - 2 * b + 4)
  realis_sqr (t, b);
  realis_mul_ui (y, b, 2);
  realis_sub (t, t, y);
  realis_add_ui (t, t, 4);
  realis_div (u, u, t);

  // y = 3 * b * u
  realis_mul_ui (t, b, 3);
  realis_mul (y, t, u);

  // round(y)
  realis_round (y, y);

  realis_clears (b, t, u, (mpfr_ptr) 0);
}

void
realis_beta (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  BINARY_FUNCTION_BODY (y, a, b, beta);
}
void
realis_gamma (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, gamma);
}
void
realis_digamma (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, digamma);
}
void
realis_zeta (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, zeta);
}

#if 0
void
realis_equal (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  realis_set_ui (y, mpfr_equal_p (a, b) ? 1 : 0);
}
void
realis_not_equal (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  realis_set_ui (y, !mpfr_equal_p (a, b) ? 1 : 0);
}
void
realis_less (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  realis_set_ui (y, mpfr_less_p (a, b) ? 1 : 0);
}
void
realis_less_equal (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  realis_set_ui (y, mpfr_lessequal_p (a, b) ? 1 : 0);
}
void
realis_greater (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  realis_set_ui (y, mpfr_greater_p (a, b) ? 1 : 0);
}
void
realis_greater_equal (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  realis_set_ui (y, mpfr_greaterequal_p (a, b) ? 1 : 0);
}
#endif

void
realis_and (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  BINARY_FUNCTION_BODY_Z (y, a, b, and);
}
void
realis_or (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  //  BINARY_FUNCTION_BODY_Z (y, a, b, eor);
}
void
realis_xor (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  BINARY_FUNCTION_BODY_Z (y, a, b, xor);
}
void
realis_not (mpfr_ptr y, mpfr_srcptr x)
{
  if (!mpfr_integer_p (x))
    {
      cerror (function_does_not_support_fractional);
      return;
    }

  if (MPFR_IS_POS (x) || MPFR_IS_ZERO (x))
    realis_add (y, x, one);
  else
    realis_sub (y, x, one);

  MPFR_CHANGE_SIGN (y);
}

void
angle_result1 (mpfr_ptr x)
{
  if (angle_mode == ANGLE_RAD)
    return;
  error (0, 0, "implement me");
}

void
realis_pi (mpfr_ptr x)
{
  CONST_FUNCTION_BODY (x, const_pi);
}
void
realis_tau (mpfr_ptr x)
{
  realis_pi (x);
  realis_mul_ui (x, x, 2);
}
void
realis_e (mpfr_ptr x)
{
  realis_exp (x, one);
}
void
realis_y (mpfr_ptr x)
{
  CONST_FUNCTION_BODY (x, const_euler);
}
void
realis_catalan (mpfr_ptr x)
{
  CONST_FUNCTION_BODY (x, const_catalan);
}
