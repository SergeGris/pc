
#ifndef _PC_H
#define _PC_H 1

#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include <gmp.h>
#include <mpfr.h>

#include <ctype.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <thread.h>


#define MPFR_SIGN_POS (1)
#define MPFR_SIGN_NEG (-1)

#define MPFR_IS_NEG(x)      (MPFR_SIGN (x) < 0)
#define MPFR_IS_POS(x)      (MPFR_SIGN (x) > 0)
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
  MODE_FIX = 'f',
  MODE_SCI = 'e'
};

extern bool
  enable_postfix, use_separator1, use_separator2;

extern unsigned int
  angle_mode, number_format, fix_digits, base, separator_freq1, separator_freq2;

extern char
  separator1, separator2;

extern void cerror (unsigned int id);
extern atomic_uint cerrno;

enum
{
#define SET(n, s) n
# include "errors.def"
#undef SET
};

#endif /* _PC_H */
