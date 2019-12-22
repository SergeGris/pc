
#ifndef _REALIS_H
#define _REALIS_H 1

#include "pc.h"

#include "system.h"

#define MPFR_PREC(x) ((x)->_mpfr_prec)
#define MPFR_EXP(x)  ((x)->_mpfr_exp)
#define MPFR_MANT(x) ((x)->_mpfr_d)

#define MPFR_SIGN_POS (+1)
#define MPFR_SIGN_NEG (-1)

#define MPFR_IS_ZERO(x)     (MPFR_EXP (x) == __MPFR_EXP_ZERO)
#define MPFR_NOT_ZERO(x)    (MPFR_EXP (x) != __MPFR_EXP_ZERO)
#define MPFR_IS_POS(x)      (!MPFR_IS_ZERO (x) && MPFR_SIGN (x) > 0)
#define MPFR_IS_NEG(x)      (!MPFR_IS_ZERO (x) && MPFR_SIGN (x) < 0)
#define MPFR_SET_POS(x)     (MPFR_SIGN (x) = MPFR_SIGN_POS)
#define MPFR_SET_NEG(x)     (MPFR_SIGN (x) = MPFR_SIGN_NEG)
#define MPFR_CHANGE_SIGN(x) (MPFR_SIGN (x) = -MPFR_SIGN (x))

/* Fast replacement for mpfr_set_zero (x, +1) */
#define MPFR_SET_ZERO(x) do { MPFR_EXP (x) = __MPFR_EXP_ZERO; MPFR_SET_POS (x); } while (0)

extern void realis_init (mpfr_ptr x);
extern void realis_inits (mpfr_ptr x, ...);
extern void realis_clear (mpfr_ptr x);
extern void realis_clears (mpfr_ptr x, ...);
extern void realis_set_si (mpfr_ptr x, signed long int n);
extern void realis_set_ui (mpfr_ptr x, unsigned long int n);
extern signed long int realis_get_si (mpfr_srcptr x);
extern unsigned long int realis_get_ui (mpfr_srcptr x);
extern void realis_init_set_si (mpfr_ptr x, signed long int n);
extern void realis_init_set_ui (mpfr_ptr x, unsigned long int n);
extern void realis_set_zero (mpfr_ptr x);
extern void realis_set_inf (mpfr_ptr x, int sign);
extern char *realis_read (mpfr_ptr y, const char *buf);
extern void realis_read_postfix (mpfr_ptr y, const char *s);
extern char *realis_write (char *buf, mpfr_srcptr x);
extern mpfr_prec_t realis_len (mpfr_srcptr x);
extern void realis_copy (mpfr_ptr y, mpfr_srcptr x);

extern void realis_neg (mpfr_ptr y, mpfr_srcptr x);
extern void realis_abs (mpfr_ptr y, mpfr_srcptr x);
extern void realis_sign (mpfr_ptr y, mpfr_srcptr x);

extern void realis_round (mpfr_ptr y, mpfr_srcptr x);
extern void realis_ceil (mpfr_ptr y, mpfr_srcptr x);
extern void realis_floor (mpfr_ptr y, mpfr_srcptr x);
extern void realis_trunc (mpfr_ptr y, mpfr_srcptr x);
extern void realis_frac (mpfr_ptr y, mpfr_srcptr x);

extern int realis_cmp (mpfr_srcptr a, mpfr_srcptr b);
extern int realis_cmpabs (mpfr_srcptr a, mpfr_srcptr b);

extern void realis_add (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
extern void realis_add_ui (mpfr_ptr y, mpfr_srcptr a, unsigned long int n);
extern void realis_sub (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
extern void realis_sub_ui (mpfr_ptr y, mpfr_srcptr a, unsigned long int n);
extern void realis_ui_sub (mpfr_ptr y, unsigned long int n, mpfr_srcptr a);
extern void realis_mul (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
extern void realis_mul_ui (mpfr_ptr y, mpfr_srcptr x, unsigned long int n);
extern void realis_div (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
extern void realis_div_ui (mpfr_ptr y, mpfr_srcptr x, unsigned long int n);
extern void realis_pow (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
extern void realis_pow_si (mpfr_ptr y, mpfr_srcptr a, signed long int b);
extern void realis_pow_ui (mpfr_ptr y, mpfr_srcptr a, unsigned long int b);
extern void realis_root (mpfr_ptr y, mpfr_srcptr b, mpfr_srcptr a);

extern void realis_inv (mpfr_ptr y, mpfr_srcptr x);

extern void realis_sqr (mpfr_ptr y, mpfr_srcptr x);
extern void realis_sqrt (mpfr_ptr y, mpfr_srcptr x);
extern void realis_cbrt (mpfr_ptr y, mpfr_srcptr x);

extern void realis_rsh (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
extern void realis_lsh (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);

extern void realis_exp (mpfr_ptr y, mpfr_srcptr x);
extern void realis_ln (mpfr_ptr y, mpfr_srcptr x);

extern void realis_agm (mpfr_ptr z, mpfr_srcptr x0, mpfr_srcptr y0);

/* Trigonometric functions.  */
extern void realis_sin (mpfr_ptr y, mpfr_srcptr x);
extern void realis_cos (mpfr_ptr y, mpfr_srcptr x);
extern void realis_tan (mpfr_ptr y, mpfr_srcptr x);
extern void realis_cot (mpfr_ptr y, mpfr_srcptr x);
extern void realis_sec (mpfr_ptr y, mpfr_srcptr x);
extern void realis_csc (mpfr_ptr y, mpfr_srcptr x);

/* Inverse trigonometric functions.  */
extern void realis_asin (mpfr_ptr y, mpfr_srcptr x);
extern void realis_acos (mpfr_ptr y, mpfr_srcptr x);
extern void realis_atan (mpfr_ptr y, mpfr_srcptr x);
extern void realis_atan2 (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
extern void realis_acot (mpfr_ptr y, mpfr_srcptr x);

/* Hyperbolic functions.  */
extern void realis_sinh (mpfr_ptr y, mpfr_srcptr x);
extern void realis_cosh (mpfr_ptr y, mpfr_srcptr x);
extern void realis_tanh (mpfr_ptr y, mpfr_srcptr x);
extern void realis_coth (mpfr_ptr y, mpfr_srcptr x);

/* Factorials.  */
extern void realis_fact (mpfr_ptr y, mpfr_srcptr x);
extern void realis_ffact (mpfr_ptr y, mpfr_srcptr x);
extern void realis_subfact (mpfr_ptr y, mpfr_srcptr x);
extern void realis_primorial (mpfr_ptr y, mpfr_srcptr x);
extern void realis_superfactorial (mpfr_ptr y, mpfr_srcptr x);
extern void realis_hyperfactorial (mpfr_ptr y, mpfr_srcptr x);

extern void realis_gcd (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
extern void realis_lcm (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);

extern void realis_mod (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
extern void realis_rem (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);

extern void realis_fibonacci (mpfr_ptr y, mpfr_srcptr x);
extern void realis_tribonacci (mpfr_ptr y, mpfr_srcptr x);

extern void realis_beta (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
extern void realis_gamma (mpfr_ptr y, mpfr_srcptr x);
extern void realis_zeta (mpfr_ptr y, mpfr_srcptr x);

extern void realis_equal (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
extern void realis_not_equal (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
extern void realis_less (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
extern void realis_less_equal (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
extern void realis_greater (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
extern void realis_greater_equal (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);

extern void realis_and (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
extern void realis_or (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
extern void realis_xor (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);
extern void realis_not (mpfr_ptr y, mpfr_srcptr x);

extern void angle_result (mpfr_ptr x);

extern void realis_pi (mpfr_ptr x);
extern void realis_tau (mpfr_ptr x);
extern void realis_e (mpfr_ptr x);
extern void realis_y (mpfr_ptr x);
extern void realis_catalan (mpfr_ptr x);

typedef void (*realis_const_function_t) (mpfr_ptr x);
typedef void (*realis_unary_function_t) (mpfr_ptr y, mpfr_srcptr x);
typedef void (*realis_binary_function_t) (mpfr_ptr y, mpfr_srcptr a, mpfr_srcptr b);

#endif /* _REALIS_H */
