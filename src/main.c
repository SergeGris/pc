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
#include "util.h"

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
const char *const short_options = "a:e:m:p:r:o:";

void
usage (int status)
{
  puts ("pc -- is the best calculator 4 you");

  exit (status);
}

enum
{
 TOK_ADD,
 TOK_SUB,
 TOK_MUL,
 TOK_DIV,
 TOK_SIN,
 TOK_COS,
 TOK_PI,
 TOK_E,
 TOK_Y,
 TOK_CATALAN
};

typedef struct builtin
{
  const char *name;
  void *func;
  unsigned long int n_args;
  unsigned long int tok;
} builtin_t;
builtin_t builtins[] =
{
  {"+", real_add, 2, TOK_ADD},
  {"-", real_sub, 2, TOK_SUB},
  {"*", real_mul, 2, TOK_MUL},
  {"/", real_div, 2, TOK_DIV},
  {"sin", real_sin, 1, TOK_SIN},
  {"cos", real_cos, 1, TOK_COS},
  {"pi", const_pi, 0, TOK_PI},
  {"e", const_e, 0, TOK_E},
  {"y", const_y, 0, TOK_Y},
  {"catalan", const_catalan, 0, TOK_CATALAN}
};

void
main_loop (void)
{
  rl_bind_key ('\t', rl_insert);

  while (true)
    {
      char *response = readline (NULL);

      /* If the line has any text in it, save it on the history.  */
      if (response != NULL && *response != '\0')
        add_history (response);

      //      printf ("%zu\n%zu\n", malloc_usable_size (response), strlen (response));
      //      strip_spaces (&response);
      //      printf ("%zu\n%zu\n", malloc_usable_size (response), strlen (response));

      char delims[] = " \t\v\n\r\f";
      char *r = strtok (response, delims);
      for (size_t i = 0; i < countof (builtins); i++)
        {
          builtin_t cur = builtins[i];
          if (strcmp (cur.name, r) == 0)
            {
              mpfr_t a[2];
              real_inits (a[0], a[1], (mpfr_ptr) 0);

              for (unsigned long int j = 0; j < cur.n_args; j++)
                real_read (a[j], strtok (NULL, delims) ? : "0");
              switch (cur.n_args)
                {
                case (unsigned long int) -1:
                  cur.func ();
                  break;
                case 0:
                  ((real_const_function_t) cur.func) (a[0]);
                  break;
                case 1:
                  ((real_unary_function_t) cur.func) (a[0], a[0]);
                  break;
                case 2:
                  ((real_binary_function_t) cur.func) (a[0], a[0], a[1]);
                  break;
                default:
                  die (EXIT_FAILURE, 0, "unknow count of arguments");
                }

              char *s = xmalloc (real_len (a[0]));
              real_write (s, a[0]);
              puts (s);
              free (s);

              real_clears (a[1], a[0], (mpfr_ptr) 0);
              break;
            }
        }

      //      puts (response);
      //      system (response);

      /* TODO: */
      /* parse (response);
         calc (); */

      free (response);
    }
}

mpfr_rnd_t
string2rounding (const char *s)
{
  if (strncasecmp (s, "RND", 3) == 0)
    {
      s += 3;
      size_t len = strlen (s);
      if (len == 1)
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
      if (len == 2 && toupper (*s) == 'N' && toupper (*(s + 1)) == 'A')
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
  if (fd >= 0)
    if (close (fd) != 0)
      error (0, errno, "%s", quotef (filename));

  return -1;
}

/* -1 -- undefined; 0 -- false; 1 -- true.  */
static int interactive = -1;

static void
parse_options (int argc, char **argv)
{
  mpfr_set_default_prec (digits2bits (100 + 1));
  mpfr_set_default_rounding_mode (MPFR_RNDN);
  int optc;
  while ((optc = getopt_long (argc, argv, short_options, long_options, NULL)) != -1)
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
        /* TODO */
        break;
      case HELP_OPTION:
        usage (EXIT_SUCCESS);
      case VERSION_OPTION:
        printf ("%s %s\n", PROGRAM_NAME, VERSION);
        printf ("Copyright © %i Sergey Sushilin.\n", COPYRIGHT_YEAR);
        fputs (_("\
\n\
License GNU General Public License version 3 <https://gnu.org/licenses/gpl-3.0.htmpl>.\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n\
\n\
"), stdout);
        const char *const authors[] = { AUTHORS, NULL };
        size_t n_authors = countof (authors) - 1;
        static_assert (countof (authors) >= 2 /* NULL we count too.  */, "n_authors < 1");

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

  if (interactive)
    {
      /* Readline support.  Set both application name and input file.  */
      rl_readline_name = PROGRAM_NAME;
      rl_instream = stdin;
      using_history ();

      puts ("This is free software with ABSOLUTELY NO WARRANTY.");
      printf ("For more details type %s.\n", quote ("warranty"));
    }

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

  //mpfr_out_str (stdout, 10, 0, x, mpfr_get_default_rounding_mode ());
  //  complex_write (s = malloc (complex_len (n)), n);
  //  puts (s);
  //  mpfr_out_str (stdout, 10, 0, x, mpfr_get_default_rounding_mode ());
  //  putchar ('\n');

  //  mpc_set_inf (x, 0);
  //  complex_write (s = malloc (complex_len (n)), n);

  //  complex_read (x, s);
  //  puts (s);
  if (interactive)
    main_loop ();

  return 0;
}
