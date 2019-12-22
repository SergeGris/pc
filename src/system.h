/*  system-dependent definitions for BrainFuck Compiler
    Copyright (C) 2019 Sergey Sushilin
    This file is part of the BrainFuck Compiler

    BrainFuck Compiler is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation, version 3

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/gpl-3.0-standalone.html>.  */

/* Include this file _after_ system headers if possible.  */

#ifndef _SYSTEM_H
#define _SYSTEM_H 1

#include <alloca.h>

#include <sys/cdefs.h>

#include <error.h>

#include <errno.h>

/* Some systems do not define this; POSIX mentions it but says it is
   obsolete.  gnulib defines it, but only on native Windows systems,
   and there only because MSVC 10 does.  */
#if !defined(ENODATA)
# define ENODATA (-1)
#endif

#include <stdbool.h>

/*** From GCC source code ***/
#include <stdio.h>

/* NOTE: pc is multi-threaded program, but threads used only in thread_loop,
   no reason to use the locking function in another functions in pc.

   HAVE_DECL_PUTC_UNLOCKED actually indicates whether or not the stdio
   code is multi-thread safe by default.  If it is set to 0, then do
   not worry about using the _unlocked functions.

   fputs_unlocked, fwrite_unlocked, and fprintf_unlocked are
   extensions and need to be prototyped by hand (since we do not
   define _GNU_SOURCE).  */
#if defined(HAVE_DECL_PUTC_UNLOCKED) && HAVE_DECL_PUTC_UNLOCKED
# if defined(HAVE_PUTC_UNLOCKED)
#  undef putc
#  define putc(c, stream) putc_unlocked (c, stream)
# endif
# if defined(HAVE_PUTCHAR_UNLOCKED)
#  undef putchar
#  define putchar(c) putchar_unlocked (c)
# endif
# if defined(HAVE_GETC_UNLOCKED)
#  undef getc
#  define getc(stream) getc_unlocked (stream)
# endif
# if defined(HAVE_GETCHAR_UNLOCKED)
#  undef getchar
#  define getchar() getchar_unlocked ()
# endif
# if defined(HAVE_FPUTC_UNLOCKED)
#  undef fputc
#  define fputc(c, stream) fputc_unlocked (c, stream)
# endif

# if defined(HAVE_FEOF_UNLOCKED)
#  undef feof
#  define feof(stream) feof_unlocked (stream)
#  if defined(HAVE_DECL_FEOF_UNLOCKED) && !HAVE_DECL_FEOF_UNLOCKED
extern int feof_unlocked (FILE *);
#  endif
# endif
# if defined(HAVE_FILENO_UNLOCKED)
#  undef fileno
#  define fileno(stream) fileno_unlocked (stream)
#  if defined (HAVE_DECL_FILENO_UNLOCKED) && !HAVE_DECL_FILENO_UNLOCKED
extern int fileno_unlocked (FILE *);
#  endif
# endif
# if defined(HAVE_FFLUSH_UNLOCKED)
#  undef fflush
#  define fflush(stream) fflush_unlocked (stream)
#  if defined (HAVE_DECL_FFLUSH_UNLOCKED) && !HAVE_DECL_FFLUSH_UNLOCKED
extern int fflush_unlocked (FILE *);
#  endif
# endif
# if defined(HAVE_FGETC_UNLOCKED)
#  undef fgetc
#  define fgetc(stream) fgetc_unlocked (stream)
#  if defined(HAVE_DECL_FGETC_UNLOCKED) && !HAVE_DECL_FGETC_UNLOCKED
extern int fgetc_unlocked (FILE *);
#  endif
# endif
# if defined(HAVE_FGETS_UNLOCKED)
#  undef fgets
#  define fgets(s, n, stream) fgets_unlocked (s, n, stream)
#  if defined(HAVE_DECL_FGETS_UNLOCKED) && !HAVE_DECL_FGETS_UNLOCKED
extern char *fgets_unlocked (char *, int, FILE *);
#  endif
# endif
# if defined(HAVE_PUTS_UNLOCKED)
#  undef puts
#  define puts(string) puts_unlocked (string)
#  if defined(HAVE_DECL_PUTS_UNLOCKED) && !HAVE_DECL_PUTS_UNLOCKED
extern int puts_unlocked (const char *);
#  endif
# endif
# if defined(HAVE_FPUTS_UNLOCKED)
#  undef fputs
#  define fputs(string, stream) fputs_unlocked (string, stream)
#  if defined(HAVE_DECL_FPUTS_UNLOCKED) && !HAVE_DECL_FPUTS_UNLOCKED
extern int fputs_unlocked (const char *, FILE *);
#  endif
# endif
# if defined(HAVE_FERROR_UNLOCKED)
#  undef ferror
#  define ferror(stream) ferror_unlocked (stream)
#  if defined(HAVE_DECL_FERROR_UNLOCKED) && !HAVE_DECL_FERROR_UNLOCKED
extern int ferror_unlocked (FILE *);
#  endif
# endif
# if defined(HAVE_FREAD_UNLOCKED)
#  undef fread
#  define fread(ptr, size, n, stream) fread_unlocked (ptr, size, n, stream)
#  if defined(HAVE_DECL_FREAD_UNLOCKED) && !HAVE_DECL_FREAD_UNLOCKED
extern size_t fread_unlocked (void *, size_t, size_t, FILE *);
#  endif
# endif
# if defined(HAVE_FWRITE_UNLOCKED)
#  undef fwrite
#  define fwrite(ptr, size, n, stream) fwrite_unlocked (ptr, size, n, stream)
#  if defined(HAVE_DECL_FWRITE_UNLOCKED) && !HAVE_DECL_FWRITE_UNLOCKED
extern size_t fwrite_unlocked (const void *, size_t, size_t, FILE *);
#  endif
# endif

# if defined(HAVE_PRINTF_UNLOCKED)
#  undef printf
/* We cannot use a function-like macro here because we do not know if
   we have varargs macros.  */
#  define printf(fmt, ...) printf_unlocked (fmt, __VA_ARGS__)
#  if defined(HAVE_DECL_PRINTF_UNLOCKED) && !HAVE_DECL_PRINTF_UNLOCKED
extern int printf_unlocked (const char *, ...);
#  endif
# endif
# if defined(HAVE_FPRINTF_UNLOCKED)
#  undef fprintf
#  define fprintf(stream, fmt, ...) fprintf_unlocked (stream, fmt, __VA_ARGS__)
#  if defined(HAVE_DECL_FPRINTF_UNLOCKED) && !HAVE_DECL_FPRINTF_UNLOCKED
extern int fprintf_unlocked (FILE *, const char *, ...);
#  endif
# endif

#endif

/*** ***/

#include <stdlib.h>
#define EXIT_TROUBLE 2

#include "version.h"

/* Redirection and wildcarding when done by the utility itself.
   Generally a noop, but used in particular for OS/2.  */
#if !defined(initialize_main)
# if !defined(__OS2__)
#  define initialize_main(ac, av) /* empty */
# else
#  define initialize_main(ac, av) \
    do { _wildcard (ac, av); _response (ac, av); } while (0)
# endif
#endif

#include "stat-macros.h"

#include "gettext.h"

#if ENABLE_NLS
/* On some systems, things go awry when <libintl.h> comes after <locale.h>.  */
# include <libintl.h>
# include <locale.h>
# define _(String) gettext (String)
# if defined(gettext_noop)
#  define N_(String) gettext_noop (String)
# else
#  define N_(String) (String)
# endif
# define S_(Msgid1, Msgid2, n) ngettext (Msgid1, Msgid2, n)
#else /* !ENABLE_NLS */
/* Include <locale.h> first to avoid conflicts with these macros.  */
# include <locale.h>
# undef gettext
# undef ngettext
# undef textdomain
# undef bindtextdomain

# define gettext(Msgid) (Msgid)
# define ngettext(Msgid1, Msgid2, n) (n == 1 ? Msgid1 : Msgid2)
# define textdomain(Domainname) do { } while (0)
# define bindtextdomain(Domainname, Dirname) do { } while (0)

# define  _(String) (String)
# define N_(String) (String)
# define S_(Msgid1, Msgid2, n) (n == 1 ? Msgid1 : Msgid2)
#endif /* ENABLE_NLS */

#include <limits.h>
#include <inttypes.h>

/* Return a value that pluralizes the same way that N does, in all
   languages we know of.  */
static inline unsigned long int _GL_ATTRIBUTE_CONST
select_plural (uintmax_t n)
{
  /* Reduce by a power of ten, but keep it away from zero.
     The gettext manual says 1000000 should be safe.  */
  enum { PLURAL_REDUCER = 1000000 };
  return (n <= ULONG_MAX ? n : n % PLURAL_REDUCER + PLURAL_REDUCER);
}

#include "verify.h"

#define HELP_OPTION_DESCRIPTION \
  _("      --help           display this help and exit\n")
#define VERSION_OPTION_DESCRIPTION \
  _("      --version        output version information and exit\n")

/* Check for errors on write.  */
#include "closeout.h"

#include "propername.h"
/* Define away proper_name (leaving proper_name_utf8, which affects far
   fewer programs), since it is not worth the cost of adding ~17KB to
   the x86_64 text size of every single program.  This avoids a 40%
   (almost ~2MB) increase in the on-disk space utilization for the set
   of the 100 binaries.  */
#define proper_name(x) (x)

#include "progname.h"


#undef max
#undef min
#undef MAX
#undef MIN

/* Taken from Linux's kernel.h */
/* Indirect macros required for expanded argument pasting, eg. __LINE__.  */
#define ___PASTE(a, b) a##b
#define __PASTE(a, b) ___PASTE (a, b)

/* Not-quite-unique ID.  */
#if !defined(__UNIQUE_ID)
# define __UNIQUE_ID(prefix) __PASTE (__PASTE (__UNIQUE_ID_, prefix), __LINE__)
#endif

/**
 * min()/max()/clamp() macros must accomplish three things:
 *
 * - avoid multiple evaluations of the arguments (so side-effects like
 *   "x++" happen only once) when non-constant.
 * - perform strict type-checking (to generate warnings instead of
 *   nasty runtime surprises).  See the "unnecessary" pointer comparison
 *   in __typecheck().
 * - retain result as a constant expressions when called with only
 *   constant expressions (to avoid tripping VLA warnings in stack
 *   allocation usage).
 */
#define __typecheck(x, y) \
  (!!(sizeof ((typeof (x) *) 1 == (typeof (y) *) 1)))

/*
 * This returns a constant expression while determining if an argument is
 * a constant expression, most importantly without evaluating the argument.
 * Glory to Martin Uecker <Martin.Uecker@med.uni-goettingen.de>
 */
#define __is_constexpr(x) \
  (sizeof (int) == sizeof (*(8 ? ((void *) ((long int) (x) * 0L)) : (int *) 8)))

#define __no_side_effects(x, y) \
  (__is_constexpr (x) && __is_constexpr (y))

#define __safe_cmp(x, y) \
  (__typecheck (x, y) && __no_side_effects (x, y))

#define __cmp(x, y, op) ((x) op (y) ? (x) : (y))

#define __cmp_once(x, y, unique_x, unique_y, op) \
  ({ \
    typeof (x) __unique_x__ = (x); \
    typeof (y) __unique_y__ = (y); \
    __cmp (__unique_x__, __unique_y__, op); \
  })

#define __careful_cmp(x, y, op) \
  __builtin_choose_expr (__safe_cmp (x, y), \
  __cmp (x, y, op), \
  __cmp_once (x, y, __UNIQUE_ID (__x), __UNIQUE_ID (__y), op))

/**
 * min - return minimum of two values of the same or compatible types
 * @x: first value
 * @y: second value
 */
#define min(x, y) __careful_cmp (x, y, <)

/**
 * max - return maximum of two values of the same or compatible types
 * @x: first value
 * @y: second value
 */
#define max(x, y) __careful_cmp (x, y, >)


/* Use this to suppress gcc's '...may be used before initialized' warnings.  */
#if defined(lint)
# define IF_LINT(Code) Code
#else
# define IF_LINT(Code) /* empty */
#endif /* lint */

#if !defined(FALLTHROUGH)
# if defined(__GNUC__) && __GNUC__ >= 7
#  define FALLTHROUGH __attribute__ ((__fallthrough__))
# else
#  define FALLTHROUGH ((void) 0)
# endif
#endif /* FALLTHROUGH */


#define __same_type(a, b) __builtin_types_compatible_p (typeof (a), typeof (b))

/*
 * Force a compilation error if condition is false, but also produce a
 * result (of value 0 and type size_t), so the expression can be used
 * e.g. in a structure initializer (or where-ever else comma expressions
 * are not permitted).
 */
#define BUILD_BUG_IF_FALSE(e) (sizeof (struct { int: (-!(e)); }))

/* &a[0] degrades to a pointer: a different type from an array.  */
#define __must_be_array(a) BUILD_BUG_IF_FALSE (!__same_type ((a), &(a)[0]))

/*
 * countof - get the number of elements in array @arr
 * @__arr: array to be sized
 */
#define countof(arr) (sizeof (arr) / sizeof ((arr)[0]) + __must_be_array (arr))
#define endof(arr) ((arr) + countof (arr))


#define aminmax(x, l, h) \
  do \
    { \
      typeof (l) __L__ = (l); \
      typeof (h) __H__ = (h); \
      if ((x) > (__H__)) \
        (x) = (__H__); \
      if ((x) < (__L__)) \
        (x) = (__L__); \
    } \
  while (0)


#define   likely(condition) __builtin_expect ((condition) != 0, true)
#define unlikely(condition) __builtin_expect ((condition) != 0, false)


static inline char * _GL_ATTRIBUTE_CONST
bad_cast (const char *s)
{
  return (char *) s;
}

extern void usage (int status) __attribute__ ((__noreturn__));

/* How coreutils quotes filenames, to minimize use of outer quotes,
   but also provide better support for copy and paste when used.  */
#include "quotearg.h"

/* Use these to shell quote only when necessary,
   when the quoted item is already delimited with colons.  */
#define quotef(arg) \
  quotearg_n_style_colon (0, shell_escape_quoting_style, arg)
#define quotef_n(n, arg) \
  quotearg_n_style_colon (n, shell_escape_quoting_style, arg)

/* Use these when there are spaces around the file name,
   in the error message.  */
#define quoteaf(arg) \
  quotearg_style (shell_escape_always_quoting_style, arg)
#define quoteaf_n(n, arg) \
  quotearg_n_style (n, shell_escape_always_quoting_style, arg)

#endif /* _SYSTEM_H */
