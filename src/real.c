
#include "pc.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>

#include "real.h"
#include "util.h"

#include "system.h"

#include "die.h"
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

#define CONST_FUNCTION_BODY(x, func) \
  mpfr_##func (x, mpfr_get_default_rounding_mode ())
#define UNARY_FUNCTION_BODY(y, x, func) \
  mpfr_##func (y, x, mpfr_get_default_rounding_mode ())
#define BINARY_FUNCTION_BODY(y, a, b, func) \
  mpfr_##func (y, a, b, mpfr_get_default_rounding_mode ());

/* x *= base^e */
static void
eex (mpfr_ptr x, long int e, int base)
{
  mpfr_t t, u;
  real_init_set_si (t, base);
  real_init (u);

  mpfr_pow_si (u, t, e, mpfr_get_default_rounding_mode ());
  mpfr_mul (x, x, u, mpfr_get_default_rounding_mode ());

  real_clear (u);
  real_clear (t);
}

void
real_init (mpfr_ptr x)
{
  mpfr_init (x);
  mpfr_set_zero (x, MPFR_SIGN_POS);
}
void
real_inits (mpfr_ptr x, ...)
{
  va_list arg_ptr;
  va_start (arg_ptr, x);
  while (x != NULL)
    {
      real_init (x);
      x = va_arg (arg_ptr, mpfr_ptr);
    }
  va_end (arg_ptr);
}
void
real_clear (mpfr_ptr x)
{
  mpfr_clear (x);
#if _DEBUG
  /* Keep it here to know we clear x.  */
  memcpy (x, "DEADBEEF", sizeof ("DEADBEEF"));
#endif
}
void
real_clears (mpfr_ptr x, ...)
{
  va_list arg_ptr;
  va_start (arg_ptr, x);
  while (x != NULL)
    {
      real_clear (x);
      x = va_arg (arg_ptr, mpfr_ptr);
    }
  va_end (arg_ptr);
}
void
real_set_si (mpfr_ptr x, signed long int n)
{
  mpfr_set_si (x, n, mpfr_get_default_rounding_mode ());
}
void
real_set_ui (mpfr_ptr x, unsigned long int n)
{
  mpfr_set_ui (x, n, mpfr_get_default_rounding_mode ());
}
void
real_init_set_si (mpfr_ptr x, signed long int n)
{
  mpfr_init_set_si (x, n, mpfr_get_default_rounding_mode ());
}
void
real_init_set_ui (mpfr_ptr x, unsigned long int n)
{
  mpfr_init_set_ui (x, n, mpfr_get_default_rounding_mode ());
}
void
real_set_zero (mpfr_ptr x)
{
  mpfr_set_zero (x, MPFR_SIGN_POS);
}

char *
real_read (mpfr_ptr y, const char *buf)
{
  char *s = NULL;
  mpfr_strtofr (y, buf, &s, base, mpfr_get_default_rounding_mode ());

  skip_whitespace (&s);

  if (is_latin_alpha (*s) && !is_latin_alpha (*(s + 1)))
    {
      if (*s == (base <= 10 ? 'E' : 'P'))
        {
          /* Exponent.  */
          s++;
          eex (y, strtol (s, &s, base), base);
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

  return s;
}

/* Output in stdout for a while.  */
void
real_write (char *buf, mpfr_srcptr x)
{
  if (mpfr_nan_p (x))
    {
      strcpy (buf, "@NaN@");
      return;
    }
  if (mpfr_inf_p (x))
    {
      strcpy (buf, MPFR_IS_POS (x) ? "@Inf@" : "-@Inf@");
      return;
    }
  if (mpfr_zero_p (x))
    {
      if (MPFR_IS_NEG (x))
        *buf++ = '-';
      *buf++ = '0';
      if (number_format == MODE_SCI)
        strcpy (buf, " E+0");
      else
        *buf = '\0';
      return;
    }

  mp_prec_t prec = bits2digits (mpfr_get_default_prec () - 1);

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

  char *format = xmalloc (19 + 5 + 1); /* Max len of 64 signed number is 19.  */

  mpfr_sprintf (format, "%%.%PuR*%c", prec > 0 ? prec : 0, (char) number_format);

  if (mpfr_sprintf (buf, format, mpfr_get_default_rounding_mode (), x) < 0)
    die (EXIT_FAILURE, errno, _("failed to write buffer in function %s"), __func__);

  free (format);

  round_result (buf, prec, number_format == MODE_FIX);
  pretty_result (buf);

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

      int r = sprintf (buf, (base <= 10 ? "e%li" : "@%li"), (long int) e);
      if (unlikely (r < 0))
        error (EXIT_FAILURE, errno, _("failed to write buffer in function %s"), __func__);
    }

  pretty_result (buf);
#endif
}
mp_prec_t
real_len (mpfr_srcptr x)
{
#if _DEBUG && 0
  mp_prec_t len = bits2digits (mpfr_get_default_prec () + 32);
  char *s = xmalloc (len + 100);
  real_write (s, x);
  printf ("len == %zi\nstrlen == %zu\n", len, strlen (s));
  free (s);
  return len;
#else
  return bits2digits (mpfr_get_default_prec ())
    + 1 /* possible sign */
    + 1 /* possible dot */
    + 1 /* possible sign before exponent */
    + 1 /* possible exponent char */
    + 19 /* possible exponent number */;
#endif
}

void
real_copy (mpfr_ptr y, mpfr_srcptr x)
{
  mp_prec_t xp = mpfr_get_prec (x);

  if (mpfr_get_prec (y) != xp)
    mpfr_set_prec (y, xp);

  mpfr_set (y, x, mpfr_get_default_rounding_mode ());
}

void
real_neg (mpfr_ptr x)
{
  UNARY_FUNCTION_BODY (x, x, neg);
}
void
real_abs (mpfr_ptr x)
{
  UNARY_FUNCTION_BODY (x, x, abs);
}
void
real_sign (mpfr_ptr x)
{
  real_set_ui (x, mpfr_sgn (x));
}

void
real_round (mpfr_ptr x)
{
  mpfr_round (x, x);
}
void
real_ceil (mpfr_ptr x)
{
  mpfr_ceil (x, x);
}
void
real_floor (mpfr_ptr x)
{
  mpfr_floor (x, x);
}
void
real_trunc (mpfr_ptr x)
{
  mpfr_trunc (x, x);
}
void
real_frac (mpfr_ptr x)
{
  mpfr_frac (x, x, mpfr_get_default_rounding_mode ());
}

int
real_cmp (mpfr_srcptr a, mpfr_srcptr b)
{
  return mpfr_cmp (a, b);
}
int
real_cmpabs (mpfr_srcptr a, mpfr_srcptr b)
{
  return mpfr_cmpabs (a, b);
}

void
real_add (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  BINARY_FUNCTION_BODY (y, a, b, add);
}
void
real_sub (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  BINARY_FUNCTION_BODY (y, a, b, sub);
}
void
real_mul (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  BINARY_FUNCTION_BODY (y, a, b, mul);
}
void
real_mul_ui (mpfr_ptr y, mpfr_srcptr x, unsigned long int n)
{
  BINARY_FUNCTION_BODY (y, x, n, mul_ui);
}
void
real_div (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  if (mpfr_zero_p (b))
    {
      cerror (division_by_zero);
      return;
    }

  BINARY_FUNCTION_BODY (y, a, b, div);
}
void
real_div_ui (mpfr_ptr y, mpfr_srcptr x, unsigned long int n)
{
  if (n == 0)
    {
      cerror (division_by_zero);
      return;
    }

  BINARY_FUNCTION_BODY (y, x, n, div_ui);
}
void
real_pow (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  if (mpfr_zero_p (a))
    {
      if (mpfr_zero_p (b))
        {
          cerror (zero_power_of_zero);
          return;
        }
      if (MPFR_IS_NEG (b))
        {
          cerror (negative_power_of_zero);
          return;
        }
      real_set_zero (y);
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
real_root (mpfr_ptr y, mpfr_srcptr b, mpfr_srcptr a)
{
  bool sign = MPFR_IS_NEG (a);
  bool b_is_odd = false;
  if (mpfr_zero_p (a))
    {
      if (MPFR_IS_NEG (b))
        cerror (negative_root_of_zero);
      else if (mpfr_zero_p (b))
        cerror (zero_root_of_zero);
      else
        real_set_zero (y);
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
          if (mpfr_get_ui (b, mpfr_get_default_rounding_mode ()) % 2 == 0)
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
          real_init (t);
          real_div_ui (t, b, 2);
          if (!mpfr_integer_p (t))
            {
              cerror (even_root_of_negative_number);
              return;
            }
          else
            b_is_odd = true;
          real_clear (t);
        }
    }
  if (mpfr_cmp_si (b, +2) == 0)
    {
      real_sqrt (y, a);
      return;
    }
  if (mpfr_cmp_si (b, -2) == 0)
    {
      real_sqrt (y, a);
      MPFR_CHANGE_SIGN (y);
      return;
    }
  if (mpfr_cmp_si (b, +1) == 0)
    {
      real_copy (y, a);
      return;
    }
  if (mpfr_cmp_si (b, -1) == 0)
    {
      real_inv (y, a);
      return;
    }

  /* TODO!!! */
  mpfr_t t, u;
  real_init (t);
  real_init (u);
  real_copy (t, a);
  if (MPFR_IS_NEG (t))
    MPFR_CHANGE_SIGN (t);

  real_ln (u, t);
  real_div (t, u, b);
  real_exp (y, t);

  if (MPFR_IS_NEG (t) && b_is_odd)
    MPFR_CHANGE_SIGN (y);
}

void
real_sqrt (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, sqrt);
}
void
real_cbrt (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, cbrt);
}

void
real_rsh (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  if (!mpfr_integer_p (b))
    {
      cerror (shift_operand_is_not_integer);
      return;
    }

  mpfr_t t;
  real_init_set_ui (t, 2);
  real_pow (t, t, b);
  real_div (y, a, t);
  real_clear (t);
}
void
real_lsh (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  if (!mpfr_integer_p (b))
    {
      cerror (shift_operand_is_not_integer);
      return;
    }

  mpfr_t t;
  real_init_set_ui (t, 2);
  real_pow (t, t, b);
  real_mul (y, a, t);
  real_clear (t);
}

void
real_exp (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, exp);
}
void
real_ln (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, log);
}

void
real_agm (mpfr_ptr z, mpfr_srcptr x0, mpfr_srcptr y0)
{
  BINARY_FUNCTION_BODY (z, x0, y0, agm);
}

void
real_inv (mpfr_ptr y, mpfr_srcptr x)
{
  mpfr_t one;
  real_init_set_ui (one, 1);
  real_div (y, one, x);
  real_clear (one);
}

/* Trigonometric functions.  */
void
real_sin (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, sin);
}
void
real_cos (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, cos);
}
void
real_tan (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, tan);
}
void
real_cot (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, cot);
}
void
real_sec (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, sec);
}
void
real_csc (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, csc);
}

/* Inverse trigonometric functions.  */
void
real_asin (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, asin);
}
void
real_acos (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, acos);
}
void
real_atan (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, atan);
}
void
real_atan2 (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  BINARY_FUNCTION_BODY (y, a, b, atan2);
}
void
real_acot (mpfr_ptr y, mpfr_srcptr x)
{
  real_inv (y, x);
  real_atan (y, y);
}

/* Hyperbolic functions.  */
void
real_sinh (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, sinh);
}
void
real_cosh (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, cosh);
}
void
real_tanh (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, tanh);
}
void
real_coth (mpfr_ptr y, mpfr_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, coth);
}

/* Factorials.  */
void
real_fact (mpfr_ptr y, mpfr_srcptr x)
{
  if (!mpfr_fits_ulong_p (x, mpfr_get_default_rounding_mode ()))
    {
      cerror (overflow);
      return;
    }

  unsigned long int n = mpfr_get_ui (x, mpfr_get_default_rounding_mode ());
  if (n <= 1)
    {
      real_set_zero (y);
      return;
    }

  mpfr_fac_ui (y, mpfr_get_ui (x, mpfr_get_default_rounding_mode ()),
               mpfr_get_default_rounding_mode ());
}
void
real_ffact (mpfr_ptr y, mpfr_srcptr x)
{
}
void
real_subfact (mpfr_ptr y, mpfr_srcptr x)
{
  if (!mpfr_integer_p (x))
    {
      cerror(function_does_not_support_fractional);
      return;
    }
  if (mpfr_sgn (x) < 0)
    {
      cerror(function_does_not_support_negative);
      return;
    }

  mpfr_t t, u, one;
  mpfr_inits (t, u, (mpfr_ptr) 0);

  real_init_set_ui (one, 1);
  real_fact (t, x);
  real_exp (u, one);
  real_div (y, t, u);
  real_round (y);
  real_clear (one);

  mpfr_clears (u, t, (mpfr_ptr) 0);
}
void
real_primorial (mpfr_ptr y, mpfr_srcptr x)
{
}

void
real_gcd (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  if (!mpfr_integer_p (a) || !mpfr_integer_p (b))
    {
      cerror (gcd_of_real_numbers);
      return;
    }

  mpfr_t t;
  real_init_set_ui (t, 1);
  if (mpfr_cmpabs (a, t) == 0 || mpfr_cmpabs (b, t) == 0)
    {
      mpfr_set_ui (y, 1, mpfr_get_default_rounding_mode ());
      return;
    }



  MPFR_SIGN (y) = MPFR_SIGN_POS;
}
void
real_lcm (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
}

void
real_mod (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  if (mpfr_zero_p (b))
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
      real_set_zero (y);
      return;
    }

  mpfr_sign_t sign = MPFR_SIGN (b);

  mpfr_t t;
  real_init (t);
  real_div (t, a, b);
  real_floor (t);
  real_mul (t, t, b);
  real_sub (y, a, t);
  real_clear (t);

  MPFR_SIGN (y) = sign;
}

void
real_fibonacci (mpfr_ptr y, mpfr_srcptr x)
{
  mpfr_t t, u, v, s, one;

  /* Если точность х * 2, то работает нормально.  */
  mpfr_inits2 (bits2digits (mpfr_get_default_prec ()) * 2, t, u, v, s, one, (mpfr_ptr) 0);

  real_set_ui (t, 5);
  real_set_ui (one, 1);
  real_sqrt (s, t);
  real_add (t, s, one);
  real_div_ui (t, t, 2);
  real_pow (u, t, x);
  real_sub (t, one, s);
  real_div_ui (t, t, 2);
  real_pow (v, t, x);
  real_sub (t, u, v);
  real_div (y, t, s);
  real_round (y);

  mpfr_clears (one, s, v, u, t, (mpfr_ptr) 0);
}

// tribonacci(x)
// a1 = (19 + 3 * sqrt(33))^(1 / 3);
// a2 = (19 - 3 * sqrt(33))^(1 / 3);
// b  = (586 + 102 * sqrt(33))^(1 / 3);
// round(3 * b * ((1 / 3 * (a1 + a2 + 1))^x / (b^2 - 2 * b + 4)))
void
real_tribonacci (mpfr_ptr y, mpfr_srcptr x)
{
  mpfr_t c2r33, c1, c2, c3, c4, c1d3, c19, c102, c586;
  mpfr_t a1, a2, b, t, u, r;
  real_inits (c2r33, c1, c2, c3, c4, c1d3, c19, c102, c586, a1, a2, b, t, u, r, (mpfr_ptr) 0);

  real_set_ui (t, 33);
  real_sqrt (c2r33, t);
  real_set_ui (c1, 1);
  real_set_ui (c2, 2);
  real_set_ui (c3, 3);
  real_set_ui (c4, 4);
  real_set_ui (c1d3, 1);
  real_div_ui (c1d3, c1d3, 3);
  real_set_ui (c19,  19);
  real_set_ui (c102, 102);
  real_set_ui (c586, 586);

  // a1 = (19 + 3 * sqrt(33))^(1 / 3)
  real_mul (t, c3, c2r33);
  real_add (u, c19, t);
  real_pow (a1, u, c1d3);

  // a2 = (19 - 3 * sqrt(33))^(1 / 3)
  real_sub (u, c19, t);
  real_pow (a2, u, c1d3);

  // b = (586 + 102 * sqrt(33))^(1 / 3)
  real_mul (t, c102, c2r33);
  real_add (u, c586, t);
  real_pow (b, u, c1d3);

  // round(3 * b * ((1 / 3 * (a1 + a2 + 1))^x / (b^2 - 2 * b + 4)))

  // (1 / 3 * (a1 + a2 + 1))^x
  real_add (t, a1, a2);
  real_add (u, t, c1);
  real_mul (t, c1d3, u);
  real_pow (u, t, x);

  // u / (b^2 - 2 * b + 4)
  real_mul (t, b, b);
  real_mul (r, c2, b);
  real_sub (a1, t, r);
  real_add (t, a1, c4);
  real_div (r, u, t);

  // 3 * b * r
  real_mul (t, c3, b);
  real_mul (y, t, r);

  // round(y)
  real_round (y);

  real_clears (c2r33, c1, c2, c3, c4, c1d3, c19, c102, c586, a1, a2, b, t, u, r, (mpfr_ptr) 0);
}

void
real_equal (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  real_set_ui (y, mpfr_equal_p (a, b) ? 1 : 0);
}
void
real_not_equal (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  real_set_ui (y, !mpfr_equal_p (a, b) ? 1 : 0);
}
void
real_less (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  real_set_ui (y, mpfr_less_p (a, b) ? 1 : 0);
}
void
real_less_equal (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  real_set_ui (y, mpfr_lessequal_p (a, b) ? 1 : 0);
}
void
real_greater (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  real_set_ui (y, mpfr_greater_p (a, b) ? 1 : 0);
}
void
real_greater_equal (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b)
{
  real_set_ui (y, mpfr_greaterequal_p (a, b) ? 1 : 0);
}

void
real_not (mpfr_ptr y, mpfr_srcptr x)
{
  if (!mpfr_integer_p (x))
    {
      cerror (function_does_not_support_fractional);
      return;
    }

  mpfr_t one;
  real_init_set_ui (one, 1);

  if (MPFR_IS_POS (x) || mpfr_zero_p (x))
    real_add (y, x, one);
  else
    real_sub (y, x, one);

  MPFR_CHANGE_SIGN (y);

  real_clear (one);
}

void
angle_result (mpfr_ptr x)
{
  if (angle_mode == ANGLE_RAD)
    return;
  error (0, 0, "implement me");
}

void
const_pi (mpfr_ptr x)
{
  CONST_FUNCTION_BODY (x, const_pi);
}
void
const_e (mpfr_ptr x)
{
  mpfr_t one;
  real_init_set_ui (one, 1);
  real_exp (x, one);
  real_clear (one);
}
void
const_y (mpfr_ptr x)
{
  CONST_FUNCTION_BODY (x, const_euler);
}
void
const_catalan (mpfr_ptr x)
{
  CONST_FUNCTION_BODY (x, const_catalan);
}
