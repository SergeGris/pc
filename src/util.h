
#ifndef _UTIL_H
#define _UTIL_H 1

#include <stdbool.h>
#include <stddef.h>
#include <mpfr.h>

extern void round_result (char *buf, mp_prec_t digits, bool fixed);
extern void pretty_result (char *buf);
extern mp_prec_t digits2bits (mp_prec_t digits);
extern mp_prec_t bits2digits (mp_prec_t bits);
extern void skip_whitespace (char **s);
extern const char *skip_spaces (const char *s);
extern void strip_extra_spaces (char **s);
extern bool is_latin_alpha (unsigned char c);

extern int yylex (void);
extern void yyerror (const char *str, ...);

extern void new_yy_file (FILE *fp);

extern void checkferror_input (FILE *fp);
extern void checkferror_output (FILE *fp);

extern void welcome (void);
extern void warranty (void);

extern size_t strlcpy (char *__restrict dst, const char *__restrict src, size_t n)
  __THROW __nonnull ((1, 2));

/* Argument list.  Recorded in the function so arguments can
   be checked at call time.  */
typedef struct arg_list
{
  int av_name;
  bool arg_is_var; /* Variable parameters.  */
  struct arg_list *next;
} arg_list;

/* A list of files to process. */
typedef struct file_node
{
  char *name;
  struct file_node *next;
} file_node;

/* The list of file names to process. */
extern file_node *file_names;

/* The name of the current file being processed.  */
extern char *file_name;

/* Input Line numbers and other error information. */
extern int line_no;

/* Access to the yy input file.  Defined in scan.c. */
extern FILE *yyin;

#endif /* _UTIL_H */
