
#include "pc.h"

#include <mpc.h>

#include "real.h"

#include "system.h"

#define UNARY_FUNCTION_BODY(y, x, func)                                       \
  do                                                                          \
    {                                                                         \
      mp_prec_t pr, pi;                                                       \
                                                                              \
      mpc_get_prec2 (&pr, &pi, x);                                            \
      mpc_set_prec (y, max (pr, pi));                                         \
                                                                              \
      mpc_##func (y, x, mpfr_get_default_rounding_mode ());                   \
    }                                                                         \
  while (0)

#define BINARY_FUNCTION_BODY(y, a, b, func)                                   \
  do                                                                          \
    {                                                                         \
      mp_prec_t apr, api, bpr, bpi;                                           \
                                                                              \
      mpc_get_prec2 (&apr, &api, a);                                          \
      mpc_get_prec2 (&bpr, &bpi, b);                                          \
                                                                              \
      mp_prec_t pr = max (apr, bpr), pi = max (api, bpi);                     \
                                                                              \
      mpc_set_prec (y, max (pr, pi));                                         \
                                                                              \
      mpc_##func (y, a, b, mpfr_get_default_rounding_mode ());                \
    }                                                                         \
  while (0)

void
map1 (mpc_ptr x, void (*f) (mpfr_ptr))
{
  f (x->re);
  f (x->im);
}
void
map2 (mpc_ptr y, mpc_srcptr x, void (*f) (mpfr_ptr, mpfr_srcptr))
{
  f (y->re, x->re);
  f (y->im, x->im);
}
void
map3 (mpc_ptr y, mpc_srcptr a, mpc_srcptr b, void (*f) (mpfr_ptr, mpfr_srcptr, mpfr_srcptr))
{
  f (y->re, a->re, b->re);
  f (y->im, a->im, b->im);
}

char *
complex_read (mpc_ptr x, const char *buf)
{
  char *s = real_read (x->re, buf);

  while (isspace (*s))
    s++;

  if (*s != '+' && *s != '-')
    {
      error (0, 0, "failed to read im part");
      return s;
    }

  bool sign = (*s == '-');

  if (*s == 'i')
    {
      s++;
      if (!isspace (*s))
        {
          error (0, 0, "failed to read im part");
          return s;
        }
      while (isspace (*s))
        s++;
      if (!isdigit (*s))
        {
          error (0, 0, "failed to read im part");
          return s;
        }
      s = real_read (x->im, s);
    }
  else if (isdigit (*s))
    {
      s = real_read (x->im, s);
      while (isspace (*s))
        s++;
      if (*s != 'i')
        {
          error (0, 0, "failed to read im part");
          return s;
        }
    }
  MPFR_SIGN (x->im) = sign;
  return s;
}
void
complex_write (char *buf, mpc_srcptr x)
{
  real_write (buf, x->re);

  if (mpfr_zero_p (x->im))
    return;

  *buf++ = ' ';
  *buf++ = (mpfr_sgn (x->im) > 0 ? '+' : '-');
  *buf++ = ' ';

  real_write (buf, x->im);
  *buf = 'i';
}
static size_t
complex_len (mpc_srcptr x)
{
  return real_len (x->re) + real_len (x->im) + 6;
}

void
complex_copy (mpc_ptr y, mpc_srcptr x)
{
  map2 (y, x, real_copy);
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
  BINARY_FUNCTION_BODY (y, a, b, pow);
}
void
complex_root (mpc_ptr y, mpc_srcptr b, mpc_srcptr a)
{
  /* TODO */
}

/* sqrt(z) = sqrt((abs(z) + re(z)) / 2) + i * sqrt((abs(z) - re(z)) / 2) * sign(im(z)) */
void
complex_sqrt (mpc_ptr y, mpc_srcptr x)
{
  if (mpfr_zero_p (x->im))
    {
      if (mpfr_sgn (x->re) < 0)
        {
          real_copy (y->re, x->re);
          real_neg (y->re);
          real_sqrt (y->im, y->re);
          mpfr_set_ui (y->re, 0, mpfr_get_default_rounding_mode ());
        }
      else
        {
          real_sqrt (y->re, x->re);
          mpfr_set_ui (y->im, 0, mpfr_get_default_rounding_mode ());
        }
    }
  else
    {
      if (mpfr_zero_p (x->re))
        {
          /* sqrt(bi) = sqrt(b / 2) + i * sqrt(b / 2) */
          mpfr_div_ui (y->im, x->im, 2, mpfr_get_default_rounding_mode ());
          real_abs (y->im);
          real_sqrt (y->re, y->im);
          real_copy (y->im, y->re);
        }
      else
        {
          complex_copy (y, x);
          complex_abs (y);
          mpfr_t t;
          mpfr_init2 (t, mpfr_get_default_prec ());

          real_sub (t, y->re, x->re);
          mpfr_div_ui (t, t, 2, mpfr_get_default_rounding_mode ());
          real_sqrt (y->im, t);
          real_add (t, y->re, x->re);
          mpfr_div_ui (t, t, 2, mpfr_get_default_rounding_mode ());
          real_sqrt (y->re, t);

          mpfr_clear (t);
        }
      MPFR_SIGN (y->im) = mpfr_sgn (x->im);
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
  if (!mpfr_zero_p (b->im) || !mpfr_zero_p (b->im))
    {
      cerror (shift_operand_is_not_integer);
      return;
    }

  map3 (y, a, b, real_rsh);
}
void
complex_lsh (mpc_ptr y, mpc_srcptr a, mpc_srcptr b)
{
  if (!mpfr_integer_p (b->re) || !mpfr_zero_p (b->im))
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
  /* TODO */
}
void
complex_sec (mpc_ptr y, mpc_srcptr x)
{
  /* TODO */
}
void
complex_csc (mpc_ptr y, mpc_srcptr x)
{
  /* TODO */
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
  int result = mpfr_equal_p (a->re, b->re);
  if (result == 0)
    result = mpfr_equal_p (a->im, b->im);

  mpc_set_si (y, result, mpfr_get_default_rounding_mode ());
}
void complex_not_equal (mpc_ptr y, mpc_srcptr a, mpc_srcptr b)
{
  int result = mpfr_equal_p (a->re, b->re);
  if (result != 0)
    result = mpfr_equal_p (a->im, b->im);

  mpc_set_si (y, !result, mpfr_get_default_rounding_mode ());
}
void complex_less (mpc_ptr y, mpc_srcptr a, mpc_srcptr b)
{
  int result = mpfr_less_p (a->re, b->re);
  if (result == 0)
    result = mpfr_less_p (a->im, b->im);

  mpc_set_si (y, result, mpfr_get_default_rounding_mode ());
}
void complex_less_equal (mpc_ptr y, mpc_srcptr a, mpc_srcptr b)
{
  int result = mpfr_lessequal_p (a->re, b->re);
  if (result == 0)
    result = mpfr_lessequal_p (a->im, b->im);

  mpc_set_si (y, result, mpfr_get_default_rounding_mode ());
}
void complex_greater (mpc_ptr y, mpc_srcptr a, mpc_srcptr b)
{
  int result = mpfr_greater_p (a->re, b->re);
  if (result == 0)
    result = mpfr_greater_p (a->im, b->im);

  mpc_set_si (y, result, mpfr_get_default_rounding_mode ());
}
void complex_greater_equal (mpc_ptr y, mpc_srcptr a, mpc_srcptr b)
{
  int result = mpfr_greaterequal_p (a->re, b->re);
  if (result == 0)
    result = mpfr_greaterequal_p (a->im, b->im);

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
