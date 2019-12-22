
#ifndef _UTIL_H
#define _UTIL_H 1

#include <stdbool.h>
#include <stddef.h>
#include <mpfr.h>

extern void round_result (char *buf, mp_prec_t digits, bool fixed);
extern void pretty_result (char *buf);
extern mp_prec_t digits2bits (mp_prec_t digits);
extern mp_prec_t bits2digits (mp_prec_t bits);
extern bool is_prime (unsigned long int n) __THROW _GL_ATTRIBUTE_CONST;
extern unsigned int fast_isqrt (unsigned long int n) __THROW _GL_ATTRIBUTE_CONST;
extern void skip_whitespace (unsigned char **s);

extern int yylex (void);
extern void yyerror (const char *str, ...);
extern unsigned int yyerrno;
extern char *yytext;

extern void welcome (void);
extern void version (void);
extern void warranty (void);

extern size_t strlcpy (char *__restrict dst, const char *__restrict src, size_t n)
  __THROW __nonnull ((1, 2));

/* Access to the yy input file.  Defined in scan.c.  */
extern FILE *yyin;

static bool is_latin_alpha_table[UCHAR_MAX + 1] =
{
  ['A'...'Z'] = true,
  ['a'...'z'] = true,
};
static inline bool
is_latin_alpha (unsigned char c)
{
  return is_latin_alpha_table[c];
}

#endif /* _UTIL_H */
