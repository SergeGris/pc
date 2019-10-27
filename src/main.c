/*
 * pc
 *
 * Copyright (C) 2019 Sergey Sushilin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "pc.h"

#include <assert.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <malloc.h>
#include <unistd.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "real.h"
#include "complex.h"
#include "stack.h"
#include "util.h"
#include "parse.h"

#include "die.h"
#include "long-options.h"
#include "quote.h"
#include "xalloc.h"
#include "xdectoint.h"

#include "system.h"

#define PROGRAM_NAME "pc"

#define AUTHORS \
  proper_name ("Sergey Sushilin")

/* For long options that have no equivalent short option, use a
   non-character as a pseudo short option, starting with CHAR_MAX + 1.  */
enum
{
  EXPRESSION_OPTION = CHAR_MAX + 1,
  HELP_OPTION,
  VERSION_OPTION
};

static const struct option long_options[] =
{
  {"expression", required_argument, NULL, 'e'},
  {"precision", required_argument, NULL, 'p'},
  {"rounding", required_argument, NULL, 'r'},
  {"angle", required_argument, NULL, 'a'},
  {"mode", required_argument, NULL, 'm'},
  {"help", no_argument, NULL, HELP_OPTION},
  {"version", no_argument, NULL, VERSION_OPTION},
  {NULL, 0, NULL, '\0'}
};
const char *const short_options = "a:e:m:p:r:i:o:";

void
usage (int status)
{
  puts ("pc -- is the best calculator 4 you");

  exit (status);
}

void
pc_help (void)
{
  puts ("TODO");
}
void
pc_warranty (void)
{
  fputs ("\
\n\
    Copyright (C) 2019 Sergey Sushilin\n\
\n\
    This program is free software: you can redistribute it and/or modify\n\
    it under the terms of the GNU General Public License as published by\n\
    the Free Software Foundation, version 3.\n\
\n\
    This program is distributed in the hope that it will be useful,\n\
    but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
    GNU General Public License for more details.\n\
\n\
    You should have received a copy of the GNU General Public License\n\
    along with this program.  If not, see <https://www.gnu.org/licenses/gpl-3.0.html>.\n\
\n\
", stdout);
}

void
pc_prec (mpfr_srcptr new_prec)
{
  mp_prec_t p;
  if (mpfr_fits_slong_p (new_prec, mpfr_get_default_rounding_mode ())
      && (p = real_get_si (new_prec)) > 0)
    mpfr_set_default_prec (digits2bits (p + 1));
  else
    error (0, 0, _("invalid precision"));
}

void
pc_base (mpfr_ptr new_base)
{
  unsigned int b;
  if (mpfr_fits_slong_p (new_base, mpfr_get_default_rounding_mode ())
      && (b = real_get_si (new_base)) > 0 && b < 62)
    base = b;
  else
    error (0, 0, _("invalid base"));
}

_Noreturn void
pc_exit (void)
{
  exit (0);
}

void
pc_print (mpfr_srcptr x)
{
  char *s = xmalloc (real_len (x));
  real_write (s, x);
  puts (s);
  free (s);
}

mpfr_t ans;
void
pc_ans (mpfr_ptr x)
{
  real_copy (x, ans);
}

typedef struct
{
  char *name;
  mpfr_t val;
} variable_t;

struct stack variables[1];
size_t
get_variable_id (const char *name)
{
  if (!is_empty (variables))
    for (size_t i = 0; i < variables->n_used; i++)
      {
        variable_t *var = ((variable_t *) stack_at (variables, i));
        if (var != NULL && strcmp (var->name, name) == 0)
          return i;
      }
  return -1;
}
void
variables_destructor (void *p)
{
  variable_t *v = p;
  free (v->name);
  real_clear (v->val);
}

typedef struct
{
  const char *name;
  void *func;
  size_t n_args; /* Number of mpfr_*ptr arguments.  */
  bool print_result;
} function_t;
function_t functions[] =
{
  {"help", pc_help, 0, false},
  {"warranty", pc_warranty, 0, false},
  {"prec", pc_prec, 1, false},
  {"exit", pc_exit, 0, false},

  {"set", NULL, 2, false},
  {"=", NULL, 2, false},

  {"print", pc_print, 1, false},
  {"ans", pc_ans, 1, true},

  {"+", real_add, 3, true},
  {"-", real_sub, 3, true},
  {"*", real_mul, 3, true},
  {"/", real_div, 3, true},
  {"pow", real_pow, 3, true},
  {"root", real_root, 3, true},
  {"exp", real_exp, 2, true},
  {"round", real_round, 1, true},
  {"ceil", real_ceil, 1, true},
  {"floor", real_floor, 1, true},
  {"trunc", real_trunc, 1, true},
  {"frac", real_frac, 1, true},
  {"sin", real_sin, 2, true},
  {"cos", real_cos, 2, true},
  {"tan", real_tan, 2, true},
  {"cot", real_cot, 2, true},

  {"fact", real_fact, 2, true},
  {"ffact", real_ffact, 2, true},
  {"subfact", real_subfact, 2, true},

  {"pi", const_pi, 1, true},
  {"tau", const_tau, 1, true},
  {"e", const_e, 1, true},
  {"y", const_y, 1, true},
  {"catalan", const_catalan, 1, true},
  {"", NULL, 0, false}
};
const char first_letters_of_functions[] = "hwpes=pa+-*/prercftfsctcffspteyc";

size_t
get_function_id (const char *function_name)
{
  const char *s = first_letters_of_functions;
  char *e = strchr (s, *function_name);
  if (e != NULL && *function_name != '\0')
    do
      if (strcmp (functions[e - s].name + 1, function_name + 1) == 0)
        return e - s;
    while ((e = strchr (e + 1, *function_name)) != NULL);

  return -1;
}

void
interrupt_handler (int sig)
{
  (void) sig;

  int saved_errno = errno;
  char msg[] = "\nuse 'exit' to quit\n";
  write (STDOUT_FILENO, msg, sizeof (msg) - 1);
  if (signal (SIGINT, interrupt_handler) == SIG_ERR)
    error (0, errno, "SIGINT");
  errno = saved_errno;
}

void
main_loop (void)
{
#if _DEBUG
  assert (sizeof (first_letters_of_functions) == countof (functions));
  for (size_t i = 0; first_letters_of_functions[i] != '\0'; i++)
    assert (first_letters_of_functions[i] == functions[i].name[0]);
#endif

  if (signal (SIGINT, interrupt_handler) == SIG_ERR)
    error (0, errno, "SIGINT");

  rl_bind_key ('\t', rl_insert);

  stack_init (variables, variables_destructor);

  mpfr_t a[3];
  real_inits (ans, a[0], a[1], a[2], (mpfr_ptr) 0);
  mp_prec_t prec = mpfr_get_default_prec ();

  yyparse ();

  while (true)
    {
      char *response = readline (NULL);

      /* If the line has any text in it, save it on the history.  */
      if (response != NULL)
        {
          if (*response == '\0')
            goto lcontinue;
          add_history (response);
        }
      else
        break;

      if (prec != mpfr_get_default_prec ())
        {
          prec = mpfr_get_default_prec ();
          mpfr_set_prec (a[0], prec);
          mpfr_set_prec (a[1], prec);
          mpfr_set_prec (a[2], prec);
          mpfr_set_prec (ans, prec);
        }

      strip_extra_spaces (&response);

      if (strcmp (response, "exit") == 0)
        pc_exit ();

      if (*response != '(')
        {
          error (0, 0, "expected '('");
          goto lcontinue;
        }
      char *r = strtok (response + 1, " ");

      if (r == NULL)
        goto lcontinue;

      if (r[strlen (r) - 1] == ')')
        r[strlen (r) - 1] = '\0';
      size_t fid = get_function_id (r);
      if (fid == (size_t) -1)
        {
          free (response);
          cerror (unknown_function);
          break;
        }
      function_t cur = functions[fid];

      for (size_t j = 0; j < cur.n_args; j++)
        {
          r = strtok (NULL, " ");
          if (r != NULL)
            {
              if (strcmp (functions[fid].name, "set") != 0
                       && strcmp (functions[fid].name, "=") != 0)
                {
                  if (is_latin_alpha (*r))
                    {
                      size_t f;
                      if ((f = get_function_id (r)) != (size_t) -1 && functions[f].n_args == 1)
                        ((real_const_function_t) functions[f].func) (a[j]);
                      if ((f = get_variable_id (r)) != (size_t) -1)
                        real_copy (a[j], ((variable_t *) stack_at (variables, f))->val);
                    }
                  else
                    {
                      char *s = real_read (a[j], r);
                      if (j == cur.n_args - 2 && *skip_spaces (s) != ')' && *((r = strtok (NULL, " ")) != NULL ? r : "") != ')')
                        {
                          error (0, 0, "expected ')'");
                          goto lcontinue;
                        }
                    }
                }
              else
                {
                  size_t f = get_function_id (r);
                  size_t v = get_variable_id (r);
                  if ((f != (size_t) -1 && functions[f].n_args == 1) || v != (size_t) -1)
                    {
                      error (0, 0, "conflicting name of variable");
                      goto lcontinue;
                    }
                  variable_t var;
                  var.name = xstrdup (r);
                  r = strtok (NULL, " ");
                  if (r == NULL)
                    {
                      error (0, 0, "expected number");
                      goto lcontinue;
                    }
                  real_init (var.val);
                  r = real_read (var.val, r);
                  stack_push (variables, &var);
                  //                  printf ("%zu\n%s\n", (v = get_variable_id (var.name)), ((variable_t *) stack_at (variables, v))->name);
                }
            }
          else
            real_set_zero (a[j]);
        }
      if (cur.func != NULL)
        switch (cur.n_args)
          {
          case 0:
            ((void (*) (void)) cur.func) ();
            break;
          case 1:
            ((real_const_function_t) cur.func) (a[0]);
            break;
          case 2:
            ((real_unary_function_t) cur.func) (a[0], a[0]);
            break;
          case 3:
            ((real_binary_function_t) cur.func) (a[0], a[0], a[1]);
            break;
          default:
            die (EXIT_FAILURE, 0, "unknow count of arguments");
          }

      if (cur.print_result)
        {
#if __STDC_VERSION__ >= 201112L
          if (cerrno == 0)
#else
          if (__atomic_load_n (&cerrno, __ATOMIC_SEQ_CST) == 0)
#endif
            {
              real_copy (ans, a[0]);
              char *s = xmalloc (real_len (a[0]));
              real_write (s, a[0]);
              puts (s);
              free (s);
            }
        }
      else
        {
#if __STDC_VERSION__ >= 201112L
          cerrno = 0;
#else
          __atomic_store_n (&cerrno, 0, __ATOMIC_SEQ_CST);
#endif
        }

      //      puts (response);
      //      system (response);

      /* TODO: */
      /* parse (response);
         calc (); */

    lcontinue:
      real_set_zero (a[0]);
      real_set_zero (a[1]);
      real_set_zero (a[2]);

      free (response);
    }

  real_clears (a[2], a[1], a[0], ans, (mpfr_ptr) 0);
}

mpfr_rnd_t
string2rounding (const char *s)
{
  if (toupper (*(s + 0)) == 'R'
      && toupper (*(s + 1)) == 'N'
      && toupper (*(s + 2)) == 'D'
      && *(s + 3) != '\0')
    {
      s += 3;
      if (*(s + 1) == '\0')
        {
          switch (toupper (*s))
            {
            case 'N':
              /* Round to nearest, with ties to even.  */
              return MPFR_RNDN;
            case 'Z':
              /* Round toward zero.  */
              return MPFR_RNDZ;
            case 'U':
              /* Round toward +Inf.  */
              return MPFR_RNDU;
            case 'D':
              /* Round toward -Inf.  */
              return MPFR_RNDD;
            case 'A':
              /* Round away from zero.  */
              return MPFR_RNDA;
            case 'F':
              /* Faithful rounding.  */
              return MPFR_RNDF;
            default:
              break;
            }
        }
      else if (toupper (*(s + 0)) == 'N'
               && toupper (*(s + 1)) == 'A'
               && *(s + 2) == '\0')
        /* Round to nearest, with ties away from zero.  */
        return MPFR_RNDNA;
    }

  die (EXIT_FAILURE, 0, "unknown mpfr rounding mode");
}

/* Redirect a standard I/O file descriptor to a file.  */
int
redirect (const char *filename, int flags, mode_t mode, int destfd)
{
  int fd;
  if ((fd = open (filename, flags, mode)) >= 0
      && dup2 (fd, destfd) >= 0
      && close (fd) == 0 ? true : (fd = -1, false))
    return fd;

  error (0, errno, "%s", quotef (filename));
  if (fd < 0)
    error (0, errno, "%s", quotef (filename));

  return -1;
}

/* -1 -- undefined; 0 -- false; 1 -- true.  */
static int interactive = -1;

static void
parse_options (int argc, char **argv)
{
  /* Readline support.  Set both application name and input file.  */
  rl_readline_name = PROGRAM_NAME;
  rl_instream = stdin;

  using_history ();

  mpfr_set_default_prec (digits2bits (42 + 1));
  mpfr_set_default_rounding_mode (MPFR_RNDNA);

  int optc;
  while ((optc = getopt_long (argc, argv, short_options, long_options, NULL)) > 0)
    switch (optc)
      {
      case 'e':
        /* TODO: */
        /* parse (optarg);
           calc (); */
        break;
      case 'p':
        mpfr_set_default_prec (digits2bits (xdectoumax (optarg, 0, MPFR_PREC_MAX - 1, "", _("invalid precision"), 0) + 1));
        break;
      case 'r':
        mpfr_set_default_rounding_mode (string2rounding (optarg));
        break;
      case 'a':
        if (strcasecmp (optarg, "rad") == 0)
          angle_mode = ANGLE_RAD;
        else if (strcasecmp (optarg, "deg") == 0)
          angle_mode = ANGLE_DEG;
        else if (strcasecmp (optarg, "grad") == 0)
          angle_mode = ANGLE_GRAD;
        else
          die (EXIT_FAILURE, 0, "unknown angle mode %s", quoteaf (optarg));
        break;
      case 'm':
        if (strcasecmp (optarg, "norm") == 0)
          number_format = MODE_NORM;
        else if (strcasecmp (optarg, "fix") == 0)
          number_format = MODE_FIX;
        else if (strcasecmp (optarg, "sci") == 0)
          number_format = MODE_SCI;
        else
          die (EXIT_FAILURE, 0, "unknown number format %s", quoteaf (optarg));
        break;
      case 'o':
        if (redirect (optarg, O_WRONLY|O_CREAT, 0644, STDOUT_FILENO) < 0)
          die (EXIT_FAILURE, 0, "failed to redirect a standard output descriptor to a file %s", quoteaf (optarg));
        interactive = 0;
        break;
      case 'i':
        yyin = rl_instream = fopen (optarg, "r");
        if (rl_instream == NULL)
          die (EXIT_FAILURE, errno, "cannot open file %s", quoteaf (optarg));
        break;
      case HELP_OPTION:
        usage (EXIT_SUCCESS);
      case VERSION_OPTION:
        printf ("%s %s\n", PROGRAM_NAME, VERSION);
        printf ("Copyright © %s Sergey Sushilin.\n", COPYRIGHT_YEAR);
        fputs (_("\
\n\
License GNU General Public License version 3 <https://gnu.org/licenses/gpl-3.0.html>.\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n\
\n\
"), stdout);
        const char *const authors[] = { AUTHORS, NULL };
        size_t n_authors = countof (authors) - 1;
        verify (countof (authors) >= 1 + 1 /* NULL we count too.  */);

        if (n_authors == 1)
          printf (_("Written by %s.\n"), authors[0]);
        else
          {
            fputs (_("Written by "), stdout);
            for (size_t i = 0; i < n_authors - 1; i++)
              {
                fputs (authors[i], stdout);
                if (i != n_authors - 2)
                  putchar (',');
                putchar (i % 3 == 0 ? '\n' : ' ');
              }
            printf (_("and %s.\n"), authors[n_authors - 1]);
          }
        exit (EXIT_SUCCESS);
      default:
        usage (EXIT_TROUBLE);
      }
}

int
main (int argc, char **argv)
{
  /* Setting values of global variables.  */
  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  if (setvbuf (stdout, NULL, _IONBF, 0) != 0)
    die (EXIT_FAILURE, errno, _("could not set buffering of stdout to mode _IONBF"));

  /* Interactive? */
  if (interactive < 0 && isatty (STDIN_FILENO) && isatty (STDOUT_FILENO))
    interactive = 1;

  parse_options (argc, argv);

  //  if (interactive)
    {

      //      puts ("This is free software with ABSOLUTELY NO WARRANTY.");
      //      printf ("For more details type %s.\n", quote ("warranty"));
    }
#if 0
  mpfr_t n;
  real_init (n);
  real_set_si (n, -1);
  real_div_ui (n, n, 12);

  char *s;
  real_write (s = xmalloc (real_len (n)), n);
  puts (cerrno == 0 ? s : "");
  free (s);

  real_write (s = xmalloc (real_len (n)), n);
  if (cerrno == 0)
    puts (s);
  free (s);
#endif

#if _DEBUG
  verify (sizeof (first_letters_of_functions) == countof (functions));
  for (size_t i = 0; first_letters_of_functions[i] != '\0'; i++)
    assert (first_letters_of_functions[i] == functions[i].name[0]);
#endif

  if (signal (SIGINT, interrupt_handler) == SIG_ERR)
    error (0, errno, "SIGINT");

  rl_bind_key ('\t', rl_insert);

  return yyparse ();

  //mpfr_out_str (stdout, 10, 0, x, mpfr_get_default_rounding_mode ());
  //  complex_write (s = malloc (complex_len (n)), n);
  //  puts (s);
  //  mpfr_out_str (stdout, 10, 0, x, mpfr_get_default_rounding_mode ());
  //  putchar ('\n');

  //  mpc_set_inf (x, 0);
  //  complex_write (s = malloc (complex_len (n)), n);

  //  complex_read (x, s);
  //  puts (s);
  main_loop ();

  return 0;
}
