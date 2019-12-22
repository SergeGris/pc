
#ifndef _COMPLEX_H
#define _COMPLEX_H 1

#include <mpc.h>

#include <stdbool.h>

#include "pc.h"
#include "realis.h"

#include "system.h"

void complex_init (mpc_ptr x);
void complex_clear (mpc_ptr x);
void complex_set_si (mpc_ptr x, signed long int n);
void complex_set_ui (mpc_ptr x, unsigned long int n);
void complex_init_set_si (mpc_ptr x, signed long int n);
void complex_init_set_ui (mpc_ptr x, unsigned long int n);
char *complex_read (mpc_ptr y, const char *buf);
char *complex_write (char *buf, mpc_srcptr x);
mpfr_prec_t complex_len (mpc_srcptr x);
void complex_copy (mpc_ptr y, mpc_srcptr x);
void complex_imag_suffix (mpc_ptr y, mpc_srcptr x);
void complex_neg (mpc_ptr x);
void complex_abs (mpc_ptr x);
void complex_sign (mpc_ptr x);

void complex_round (mpc_ptr x);
void complex_ceil (mpc_ptr x);
void complex_floor (mpc_ptr x);
void complex_trunc (mpc_ptr x);
void complex_frac (mpc_ptr x);

int complex_cmp (mpc_srcptr a, mpc_srcptr b);
int complex_cmpabs (mpc_srcptr a, mpc_srcptr b);

void complex_add (mpc_ptr y, mpc_srcptr a, mpc_srcptr b);
void complex_sub (mpc_ptr y, mpc_srcptr a, mpc_srcptr b);
void complex_mul (mpc_ptr y, mpc_srcptr a, mpc_srcptr b);
void complex_div (mpc_ptr y, mpc_srcptr a, mpc_srcptr b);
void complex_pow (mpc_ptr y, mpc_srcptr a, mpc_srcptr b);
void complex_root (mpc_ptr y, mpc_srcptr b, mpc_srcptr a);

void complex_sqrt (mpc_ptr y, mpc_srcptr x);
void complex_cbrt (mpc_ptr y, mpc_srcptr x);

void complex_rsh (mpc_ptr y, mpc_srcptr a, mpc_srcptr b);
void complex_lsh (mpc_ptr y, mpc_srcptr a, mpc_srcptr b);

void complex_exp (mpc_ptr y, mpc_srcptr x);
void complex_ln (mpc_ptr y, mpc_srcptr x);

void complex_agm (mpc_ptr z, mpc_srcptr x0, mpc_srcptr y0);

/* Trigonometric functions.  */
void complex_sin (mpc_ptr y, mpc_srcptr x);
void complex_cos (mpc_ptr y, mpc_srcptr x);
void complex_tan (mpc_ptr y, mpc_srcptr x);
void complex_cot (mpc_ptr y, mpc_srcptr x);
void complex_sec (mpc_ptr y, mpc_srcptr x);
void complex_csc (mpc_ptr y, mpc_srcptr x);

/* Inverse trigonometric functions.  */
void complex_asin (mpc_ptr y, mpc_srcptr x);
void complex_acos (mpc_ptr y, mpc_srcptr x);
void complex_atan (mpc_ptr y, mpc_srcptr x);
void complex_atan2 (mpc_ptr y, mpc_srcptr a, mpc_srcptr b);
void complex_acot (mpc_ptr y, mpc_srcptr x);

/* Hyperbolic functions.  */
void complex_sinh (mpc_ptr y, mpc_srcptr x);
void complex_cosh (mpc_ptr y, mpc_srcptr x);
void complex_tanh (mpc_ptr y, mpc_srcptr x);
void complex_coth (mpc_ptr y, mpc_srcptr x);

/* Factorials.  */
void complex_fact (mpc_ptr y, mpc_srcptr x);
void complex_ffact (mpc_ptr y, mpc_srcptr x);
void complex_subfact (mpc_ptr y, mpc_srcptr x);
void complex_primorial (mpc_ptr y, mpc_srcptr x);

void complex_gcd (mpc_ptr y, mpc_srcptr a, mpc_srcptr b);
void complex_lcm (mpc_ptr y, mpc_srcptr a, mpc_srcptr b);

void complex_fibonacci (mpc_ptr y, mpc_srcptr x);
void complex_tribonacci (mpc_ptr y, mpc_srcptr x);

void complex_equal (mpc_ptr y, mpc_srcptr a, mpc_srcptr b);
void complex_not_equal (mpc_ptr y, mpc_srcptr a, mpc_srcptr b);
void complex_less (mpc_ptr y, mpc_srcptr a, mpc_srcptr b);
void complex_less_equal (mpc_ptr y, mpc_srcptr a, mpc_srcptr b);
void complex_greater (mpc_ptr y, mpc_srcptr a, mpc_srcptr b);
void complex_greater_equal (mpc_ptr y, mpc_srcptr a, mpc_srcptr b);

extern mpc_t c_one;

#endif /* _COMPLEX_H */
