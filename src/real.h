
#include <mpfr.h>

#include <stdbool.h>

#include "pc.h"

#include "system.h"

void real_init (mpfr_ptr x);
void real_inits (mpfr_ptr x, ...);
void real_clear (mpfr_ptr x);
void real_clears (mpfr_ptr x, ...);
void real_set_si (mpfr_ptr x, signed long int n);
void real_set_ui (mpfr_ptr x, unsigned long int n);
void real_init_set_si (mpfr_ptr x, signed long int n);
void real_init_set_ui (mpfr_ptr x, unsigned long int n);
void real_set_zero (mpfr_ptr x);
char *real_read (mpfr_ptr y, const char *buf);
void real_write (char *buf, mpfr_srcptr x);
mp_prec_t real_len (mpfr_srcptr x);
void real_copy (mpfr_ptr y, mpfr_srcptr x);
void real_neg (mpfr_ptr x);
void real_abs (mpfr_ptr x);
void real_sign (mpfr_ptr x);

void real_round (mpfr_ptr x);
void real_ceil (mpfr_ptr x);
void real_floor (mpfr_ptr x);
void real_trunc (mpfr_ptr x);
void real_frac (mpfr_ptr x);

int real_cmp (mpfr_srcptr a, mpfr_srcptr b);
int real_cmpabs (mpfr_srcptr a, mpfr_srcptr b);

void real_add (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
void real_sub (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
void real_mul (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
void real_mul_ui (mpfr_ptr y, mpfr_srcptr x, unsigned long int n);
void real_div (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
void real_div_ui (mpfr_ptr y, mpfr_srcptr x, unsigned long int n);
void real_pow (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
void real_root (mpfr_ptr y, mpfr_srcptr b, mpfr_srcptr a);

void real_inv (mpfr_ptr y, mpfr_srcptr x);

void real_sqrt (mpfr_ptr y, mpfr_srcptr x);
void real_cbrt (mpfr_ptr y, mpfr_srcptr x);

void real_rsh (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
void real_lsh (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);

void real_exp (mpfr_ptr y, mpfr_srcptr x);
void real_ln (mpfr_ptr y, mpfr_srcptr x);

void real_agm (mpfr_ptr z, mpfr_srcptr x0, mpfr_srcptr y0);

/* Trigonometric functions.  */
void real_sin (mpfr_ptr y, mpfr_srcptr x);
void real_cos (mpfr_ptr y, mpfr_srcptr x);
void real_tan (mpfr_ptr y, mpfr_srcptr x);
void real_cot (mpfr_ptr y, mpfr_srcptr x);
void real_sec (mpfr_ptr y, mpfr_srcptr x);
void real_csc (mpfr_ptr y, mpfr_srcptr x);

/* Inverse trigonometric functions.  */
void real_asin (mpfr_ptr y, mpfr_srcptr x);
void real_acos (mpfr_ptr y, mpfr_srcptr x);
void real_atan (mpfr_ptr y, mpfr_srcptr x);
void real_atan2 (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
void real_acot (mpfr_ptr y, mpfr_srcptr x);

/* Hyperbolic functions.  */
void real_sinh (mpfr_ptr y, mpfr_srcptr x);
void real_cosh (mpfr_ptr y, mpfr_srcptr x);
void real_tanh (mpfr_ptr y, mpfr_srcptr x);
void real_coth (mpfr_ptr y, mpfr_srcptr x);

/* Factorials.  */
void real_fact (mpfr_ptr y, mpfr_srcptr x);
void real_ffact (mpfr_ptr y, mpfr_srcptr x);
void real_subfact (mpfr_ptr y, mpfr_srcptr x);
void real_primorial (mpfr_ptr y, mpfr_srcptr x);

void real_gcd (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
void real_lcm (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);

void real_fibonacci (mpfr_ptr y, mpfr_srcptr x);
void real_tribonacci (mpfr_ptr y, mpfr_srcptr x);

void real_equal (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
void real_not_equal (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
void real_less (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
void real_less_equal (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
void real_greater (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
void real_greater_equal (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);

void real_not (mpfr_ptr y, mpfr_srcptr x);

void angle_result (mpfr_ptr x);

void const_pi (mpfr_ptr x);
void const_e (mpfr_ptr x);
void const_y (mpfr_ptr x);
void const_catalan (mpfr_ptr x);

typedef void (*real_const_function_t) (mpfr_ptr x);
typedef void (*real_unary_function_t) (mpfr_ptr y, mpfr_srcptr x);
typedef void (*real_binary_function_t) (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
