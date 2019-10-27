
#ifndef _PC_H
#define _PC_H 1

#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include <gmp.h>
#include <mpfr.h>

#include <readline/readline.h>
#include <readline/history.h>

#include <ctype.h>
#if __STDC_VERSION__ >= 201112L
# include <stdatomic.h>
#endif
#include <stdbool.h>
#include <thread.h>

#define MPFR_SIGN_POS (+1)
#define MPFR_SIGN_NEG (-1)

#define MPFR_IS_ZERO(x)     (mpfr_zero_p (x))
#define MPFR_IS_NEG(x)      (!MPFR_IS_ZERO (x) && MPFR_SIGN (x) < 0)
#define MPFR_IS_POS(x)      (!MPFR_IS_ZERO (x) && MPFR_SIGN (x) > 0)
#define MPFR_SET_POS(x)     (MPFR_SIGN (x) = MPFR_SIGN_POS)
#define MPFR_SET_NEG(x)     (MPFR_SIGN (x) = MPFR_SIGN_NEG)
#define MPFR_CHANGE_SIGN(x) (MPFR_SIGN (x) = -MPFR_SIGN (x))

enum
{
  ANGLE_DEG,
  ANGLE_RAD,
  ANGLE_GRAD
};
enum
{
  MODE_NORM = 'g',
  MODE_FIX  = 'f',
  MODE_SCI  = 'e'
};

extern bool
  enable_postfix, use_separator1, use_separator2;

extern unsigned int
  angle_mode, number_format, fix_digits, base, separator_freq1, separator_freq2;

extern char
  separator1, separator2;

extern void cerror (unsigned int id);
#if __STDC_VERSION__ >= 201112L
extern atomic_uint cerrno, yyerrno;
#else
extern unsigned int cerrno, yyerrno;
#endif

enum
{
  pc_success,
#define SET(n, s) n
# include "errors.def"
#undef SET
};

/* Macro Definitions */
#define HISTORY_SIZE(n) stifle_history (n)
#define UNLIMIT_HISTORY unstifle_history ()

#endif /* _PC_H */
