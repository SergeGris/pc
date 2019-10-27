
#include "pc.h"

#include <mpc.h>

#include "real.h"
#include "complex.h"
#include "util.h"

#include "system.h"

#define UNARY_FUNCTION_BODY(y, x, func)                                 \
  do                                                                    \
    mpc_##func (y, x, mpfr_get_default_rounding_mode ());               \
  while (0)

#define BINARY_FUNCTION_BODY(y, a, b, func)                             \
  do                                                                    \
    mpc_##func (y, a, b, mpfr_get_default_rounding_mode ());            \
  while (0)

void
map1 (mpc_ptr x, void (*f) (mpfr_ptr))
{
  f (mpc_realref (x));
  f (mpc_imagref (x));
}
void
map2 (mpc_ptr y, mpc_srcptr x, void (*f) (mpfr_ptr, mpfr_srcptr))
{
  f (mpc_realref (y), mpc_realref (x));
  f (mpc_imagref (y), mpc_imagref (x));
}
void
map3 (mpc_ptr y, mpc_srcptr a, mpc_srcptr b, void (*f) (mpfr_ptr, mpfr_srcptr, mpfr_srcptr))
{
  f (mpc_realref (y), mpc_realref (a), mpc_realref (b));
  f (mpc_imagref (y), mpc_imagref (a), mpc_imagref (b));
}

void
complex_init (mpc_ptr x)
{
  map1 (x, real_init);
}
void
complex_clear (mpc_ptr x)
{
  map1 (x, real_clear);
}
void
complex_set_si (mpc_ptr x, signed long int n)
{
  real_set_si (mpc_realref (x), n);
  real_set_zero (mpc_imagref (x));
}
void
complex_set_ui (mpc_ptr x, unsigned long int n)
{
  real_set_ui (mpc_realref (x), n);
  real_set_zero (mpc_imagref (x));
}
void
complex_init_set_si (mpc_ptr x, signed long int n)
{
  real_init_set_si (mpc_realref (x), n);
  real_init_set_si (mpc_imagref (x), 0);
}
void
complex_init_set_ui (mpc_ptr x, unsigned long int n)
{
  real_init_set_ui (mpc_realref (x), n);
  real_init_set_ui (mpc_imagref (x), 0);
}
void
complex_set_zero (mpc_ptr x)
{
  map1 (x, real_set_zero);
}

char *
complex_read (mpc_ptr x, const char *buf)
{
  char *s = real_read (mpc_realref (x), buf);

  skip_whitespace (&s);

  if (*s != '\0' && *s != '+' && *s != '-' && *s != 'i')
    {
      error (0, 0, "failed to read im part");
      return s;
    }
  bool read_real = *s == 'i';

  if (read_real)
    mpfr_swap (mpc_realref (x), mpc_imagref (x));

  skip_whitespace (&s);

  bool sign = (*s == '-');

  if (!read_real && *s == 'i')
    {
      s++;
      skip_whitespace (&s);
      if (!isdigit (*s))
        goto error;
      s = real_read (mpc_imagref (x), s);
    }
  else if (isdigit (*s))
    {
      s = real_read (mpc_imagref (x), s);
      skip_whitespace (&s);
      if (*s != 'i')
        goto error;
    }
  MPFR_SIGN (mpc_imagref (x)) = sign;
  return s;
error:
  error (0, 0, "failed to read imagine part");
  return NULL;
}
void
complex_write (char *buf, mpc_srcptr x)
{
  real_write (buf, mpc_realref (x));

  if (MPFR_IS_ZERO (mpc_imagref (x)))
    return;
  buf = strchr (buf, '\0');

  *buf++ = ' ';
  *buf++ = (MPFR_IS_POS (mpc_imagref (x)) ? '+' : '-');
  *buf++ = ' ';

  real_write (buf, mpc_imagref (x));
  buf = strchr (buf, '\0');
  *buf = 'i';
}
mp_prec_t
complex_len (mpc_srcptr x)
{
  return real_len (mpc_realref (x)) + real_len (mpc_imagref (x)) + 6;
}

void
complex_copy (mpc_ptr y, mpc_srcptr x)
{
  map2 (y, x, real_copy);
}

void
complex_imag_suffix (mpc_ptr y, mpc_srcptr x)
{
  real_copy (mpc_realref (y), mpc_imagref (x));
  real_copy (mpc_imagref (y), mpc_realref (x));
  real_neg (mpc_realref (y));
}

void
complex_neg (mpc_ptr x)
{
  map1 (x, real_neg);
}
void
complex_abs (mpc_ptr x)
{
  map1 (x, real_abs);
}
void
complex_sign (mpc_ptr x)
{
  mpc_t c;
  mpfr_t f;
  mp_prec_t pr, pi;

  mpc_get_prec2 (&pr, &pi, x);
  mpc_init2 (c, max (pr, pi));
  mpfr_init2 (f, max (pr, pi));

  mpc_abs (f, x, mpfr_get_default_rounding_mode ());
  mpc_set_fr (c, f, mpfr_get_default_rounding_mode ());
  mpc_div (x, c, x, mpfr_get_default_rounding_mode ());

  mpfr_clear (f);
  mpc_clear (c);
}

void
complex_round (mpc_ptr x)
{
  map1 (x, real_round);
}
void
complex_ceil (mpc_ptr x)
{
  map1 (x, real_ceil);
}
void
complex_floor (mpc_ptr x)
{
  map1 (x, real_floor);
}
void
complex_trunc (mpc_ptr x)
{
  map1 (x, real_trunc);
}
void
complex_frac (mpc_ptr x)
{
  map1 (x, real_frac);
}

int
complex_cmp (mpc_srcptr a, mpc_srcptr b)
{
  return mpc_cmp (a, b);
}
int
complex_cmpabs (mpc_srcptr a, mpc_srcptr b)
{
  return mpc_cmp_abs (a, b);
}

void
complex_add (mpc_ptr y, mpc_srcptr a, mpc_srcptr b)
{
  BINARY_FUNCTION_BODY (y, a, b, add);
}
void
complex_sub (mpc_ptr y, mpc_srcptr a, mpc_srcptr b)
{
  BINARY_FUNCTION_BODY (y, a, b, sub);
}
void
complex_mul (mpc_ptr y, mpc_srcptr a, mpc_srcptr b)
{
  BINARY_FUNCTION_BODY (y, a, b, mul);
}
void
complex_div (mpc_ptr y, mpc_srcptr a, mpc_srcptr b)
{
  BINARY_FUNCTION_BODY (y, a, b, div);
}
void
complex_pow (mpc_ptr y, mpc_srcptr a, mpc_srcptr b)
{
  if (MPFR_IS_ZERO (mpc_realref (a)) && MPFR_IS_ZERO (mpc_imagref (a)))
    {
      if (!MPFR_IS_ZERO (mpc_imagref (b)))
        cerror (complex_power_of_zero);
      else if (MPFR_IS_ZERO (mpc_realref (b)))
        cerror (zero_power_of_zero);
      else if (mpfr_cmp_ui (mpc_realref (b), 0) < 0)
        cerror (negative_power_of_zero);
      else
        complex_set_zero (y);
      return;
    }

  BINARY_FUNCTION_BODY (y, a, b, pow);
}
void
complex_root (mpc_ptr y, mpc_srcptr b, mpc_srcptr a)
{
  if (MPFR_IS_ZERO (mpc_realref (a)) && MPFR_IS_ZERO (mpc_imagref (a)))
    {
      if (!MPFR_IS_ZERO (mpc_imagref (b)))
        cerror (complex_root_of_zero);
      else if (MPFR_IS_ZERO (mpc_realref (b)))
        cerror (zero_root_of_zero);
      else
        complex_set_zero (y);
      return;
    }

  mpc_t t;
  complex_init (t);

  complex_ln (y, a);
  complex_div (t, y, b);
  complex_exp (y, t);

  complex_clear (t);
}

/* sqrt(z) = sqrt((abs(z) + re(z)) / 2) + i * sqrt((abs(z) - re(z)) / 2) * sign(im(z)) */
void
complex_sqrt (mpc_ptr y, mpc_srcptr x)
{
  if (MPFR_IS_ZERO (mpc_imagref (x)))
    {
      if (MPFR_IS_NEG (mpc_realref (x)))
        {
          real_copy (mpc_realref (y), mpc_realref (x));
          real_neg (mpc_realref (y));
          real_sqrt (mpc_imagref (y), mpc_realref (y));
          real_set_zero (mpc_realref (y));
        }
      else
        {
          real_sqrt (mpc_realref (y), mpc_realref (x));
          real_set_zero (mpc_imagref (y));
        }
    }
  else
    {
      if (MPFR_IS_ZERO (mpc_realref (x)))
        {
          /* sqrt(bi) = sqrt(b / 2) + i * sqrt(b / 2) */
          real_div_ui (mpc_imagref (y), mpc_imagref (x), 2);
          real_abs (mpc_imagref (y));
          real_sqrt (mpc_realref (y), mpc_imagref (y));
          real_copy (mpc_imagref (y), mpc_realref (y));
        }
      else
        {
          complex_copy (y, x);
          complex_abs (y);
          mpfr_t t;
          real_init (t);

          real_sub (t, mpc_realref (y), mpc_realref (x));
          real_div_ui (t, t, 2);
          real_sqrt (mpc_imagref (y), t);
          real_add (t, mpc_realref (y), mpc_realref (x));
          real_div_ui (t, t, 2);
          real_sqrt (mpc_realref (y), t);

          real_clear (t);
        }
      MPFR_SIGN (mpc_imagref (y)) = mpfr_sgn (mpc_imagref (x));
    }
}
void
complex_cbrt (mpc_ptr y, mpc_srcptr x)
{
  /* TODO */
}

void
complex_rsh (mpc_ptr y, mpc_srcptr a, mpc_srcptr b)
{
  if (!MPFR_IS_ZERO (mpc_imagref (b)) || !MPFR_IS_ZERO (mpc_imagref (b)))
    {
      cerror (shift_operand_is_not_integer);
      return;
    }

  map3 (y, a, b, real_rsh);
}
void
complex_lsh (mpc_ptr y, mpc_srcptr a, mpc_srcptr b)
{
  if (!mpfr_integer_p (mpc_realref (b)) || !MPFR_IS_ZERO (mpc_imagref (b)))
    {
      cerror (shift_operand_is_not_integer);
      return;
    }

  map3 (y, a, b, real_lsh);
}

void
complex_exp (mpc_ptr y, mpc_srcptr x)
{
  map2 (y, x, real_exp);
}
void
complex_ln (mpc_ptr y, mpc_srcptr x)
{
  map2 (y, x, real_ln);
}

/* Trigonometric functions.  */
void
complex_sin (mpc_ptr y, mpc_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, sin);
}
void
complex_cos (mpc_ptr y, mpc_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, cos);
}
void
complex_tan (mpc_ptr y, mpc_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, tan);
}
void
complex_cot (mpc_ptr y, mpc_srcptr x)
{
  complex_tan (y, x);
  mpc_t one;
  complex_init_set_ui (one, 1);
  complex_div (y, one, y);
  complex_clear (one);
}
void
complex_sec (mpc_ptr y, mpc_srcptr x)
{

}
void
complex_csc (mpc_ptr y, mpc_srcptr x)
{
  /*TODO*/
}

/* Inverse trigonometric functions.  */
void
complex_asin (mpc_ptr y, mpc_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, asin);
}
void
complex_acos (mpc_ptr y, mpc_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, acos);
}
void
complex_atan (mpc_ptr y, mpc_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, atan);
}
void
complex_atan2 (mpc_ptr y, mpc_srcptr a, mpc_srcptr b)
{
  /* TODO */
}
void
complex_acot (mpc_ptr y, mpc_srcptr x)
{
  /* TODO */
}

// Hyperbolic functions
void
complex_sinh (mpc_ptr y, mpc_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, sinh);
}
void
complex_cosh (mpc_ptr y, mpc_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, cosh);
}
void
complex_tanh (mpc_ptr y, mpc_srcptr x)
{
  UNARY_FUNCTION_BODY (y, x, tanh);
}
void
complex_coth (mpc_ptr y, mpc_srcptr x)
{ /* TODO */
}

/* Factorials.  */
void
complex_fact (mpc_ptr y, mpc_srcptr x)
{
}
void
complex_ffact (mpc_ptr y, mpc_srcptr x)
{
}
void
complex_subfact (mpc_ptr y, mpc_srcptr x)
{
}
void
complex_primorial (mpc_ptr y, mpc_srcptr x)
{
}

void
complex_gcd (mpc_ptr y, mpc_srcptr a, mpc_srcptr b)
{
}
void
complex_lcm (mpc_ptr y, mpc_srcptr a, mpc_srcptr b)
{
}

void complex_equal (mpc_ptr y, mpc_srcptr a, mpc_srcptr b)
{
  int result = mpfr_equal_p (mpc_realref (a), mpc_realref (b));
  if (result == 0)
    result = mpfr_equal_p (mpc_imagref (a), mpc_imagref (b));

  mpc_set_si (y, result, mpfr_get_default_rounding_mode ());
}
void complex_not_equal (mpc_ptr y, mpc_srcptr a, mpc_srcptr b)
{
  int result = mpfr_equal_p (mpc_realref (a), mpc_realref (b));
  if (result != 0)
    result = mpfr_equal_p (mpc_imagref (a), mpc_imagref (b));

  mpc_set_si (y, !result, mpfr_get_default_rounding_mode ());
}
void complex_less (mpc_ptr y, mpc_srcptr a, mpc_srcptr b)
{
  int result = mpfr_less_p (mpc_realref (a), mpc_realref (b));
  if (result == 0)
    result = mpfr_less_p (mpc_imagref (a), mpc_imagref (b));

  mpc_set_si (y, result, mpfr_get_default_rounding_mode ());
}
void complex_less_equal (mpc_ptr y, mpc_srcptr a, mpc_srcptr b)
{
  int result = mpfr_lessequal_p (mpc_realref (a), mpc_realref (b));
  if (result == 0)
    result = mpfr_lessequal_p (mpc_imagref (a), mpc_imagref (b));

  mpc_set_si (y, result, mpfr_get_default_rounding_mode ());
}
void complex_greater (mpc_ptr y, mpc_srcptr a, mpc_srcptr b)
{
  int result = mpfr_greater_p (mpc_realref (a), mpc_realref (b));
  if (result == 0)
    result = mpfr_greater_p (mpc_imagref (a), mpc_imagref (b));

  mpc_set_si (y, result, mpfr_get_default_rounding_mode ());
}
void complex_greater_equal (mpc_ptr y, mpc_srcptr a, mpc_srcptr b)
{
  int result = mpfr_greaterequal_p (mpc_realref (a), mpc_realref (b));
  if (result == 0)
    result = mpfr_greaterequal_p (mpc_imagref (a), mpc_imagref (b));

  mpc_set_si (y, result, mpfr_get_default_rounding_mode ());
}

void
complex_fibonacci (mpc_ptr y, mpc_srcptr x)
{

}
void
complex_tribonacci (mpc_ptr y, mpc_srcptr x)
{
}
