
#ifndef _PC_H
#define _PC_H 1

#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include <gmp.h>
#include <mpfr.h>
#include <mpc.h>

#include <readline/readline.h>
#include <readline/history.h>

#include <ctype.h>
#if __STDC_VERSION__ >= 201112L
# include <stdatomic.h>
#endif
#include <stdbool.h>
#include <noreturn.h>
#define noreturn _GL_NORETURN_FUNC

#include "stack.h"

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

mpfr_rnd_t string2rounding (const char *s);

extern void cerror (unsigned int id);
extern void cerrorf (unsigned int id, const char *fmt, ...);
#if __STDC_VERSION__ >= 201112L
extern atomic_uint cerrno;
# define __set_cerrno(val) (cerrno = (val))
# define __get_cerrno()    (cerrno)
#else
extern unsigned int cerrno;
# define __set_cerrno(val) (__atomic_store_n (&cerrno, (val), __ATOMIC_SEQ_CST))
# define __get_cerrno()    (__atomic_load_n  (&cerrno,        __ATOMIC_SEQ_CST))
#endif

static inline void
mpz2mpfr (mpfr_ptr fr, mpz_srcptr z)
{
  mpfr_set_z (fr, z, mpfr_get_default_rounding_mode ());
}
static inline void
mpfr2mpz (mpz_ptr z, mpfr_srcptr fr)
{
  mpfr_get_z (z, fr, mpfr_get_default_rounding_mode ());
}

mpfr_t one; /* = {{1, MPFR_SIGN_POS, 1, (mp_limb_t [1]) {(~((mp_limb_t) 0) - 1) / 2}}}; */

typedef enum { Z, Q, R, C, H } tag_t;

typedef struct number
{
  tag_t tag;
  union
  {
    mpz_ptr z;
    mpq_ptr q;
    mpfr_ptr r;
  };
} number_t[1], *number_ptr;

typedef struct
{
  char *name;
  mpfr_t val;
} variable_t;

struct stack variables[1];

struct stack number_stack[1];

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

#define PROGRAM_NAME "pc"
#define AUTHORS \
  proper_name ("Sergey Sushilin")

#endif /* _PC_H */
