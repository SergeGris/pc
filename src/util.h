
#ifndef _UTIL_H
#define _UTIL_H 1

#include <mpfr.h>
#include <stdbool.h>

extern void round_result (char *buf, mp_prec_t digits, bool fixed);
extern void pretty_result (char *buf);
extern mp_prec_t digits2bits (mp_prec_t digits);
extern mp_prec_t bits2digits (mp_prec_t bits);
extern void skip_whitespace (char **s);
extern void strip_spaces (char **s);
extern bool is_latin_alpha (int c);

#endif /* _UTIL_H */
