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
#include <unistd.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "real.h"
#include "complex.h"
#include "util.h"

#include "die.h"
#include "long-options.h"
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

void
main_loop (void)
{
  rl_bind_key ('\t', rl_insert);

  while (true)
    {
      char *response = readline ("");

      /* If the line has any text in it, save it on the history.  */
      if (response != NULL && *response != '\0' && *response != ' ')
        add_history (response);

      /* TODO: */
      /* parse (response); */

      free (response);
    }
}

mpfr_rnd_t
string2rounding (const char *s)
{
  if (strlen (s) != 4 || strncasecmp (s, "RND", 3) != 0)
    goto ldie;

  switch (toupper (*(s + 3)))
    {
    case 'N':
      /* Round to nearest, with ties to even.  */
      return MPFR_RNDN;
      break;
    case 'Z':
      /* Round toward zero.  */
      return MPFR_RNDZ;
      break;
    case 'U':
      /* Round toward +Inf.  */
      return MPFR_RNDU;
      break;
    case 'D':
      /* Round toward -Inf.  */
      return MPFR_RNDD;
      break;
    case 'A':
      /* Round away from zero.  */
      return MPFR_RNDA;
      break;
    case 'F':
      /* Faithful rounding.  */
      return MPFR_RNDF;
      break;
    default:
      goto ldie;
    }

ldie:
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

static void
parse_options (int argc, char **argv)
{
  mpfr_set_default_prec (digits2bits (100));
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
        mpfr_set_default_prec (digits2bits (xdectoumax (optarg, 0, MPFR_PREC_MAX, "", _("invalid precision"), 0)));
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
          die (EXIT_FAILURE, 0, "unknown angle mode %s", quoteaf (optarg));
        break;
      case 'o':
        if (redirect (optarg, O_WRONLY|O_CREAT, 0644, STDOUT_FILENO) < 0)
          die (EXIT_FAILURE, 0, "failed to redirect a standard output descriptor to a file %s", quoteaf (optarg));
        break;
      case HELP_OPTION:
        usage (EXIT_SUCCESS);
      case VERSION_OPTION:
        printf ("Copyright © %i Sergey Sushilin\n", COPYRIGHT_YEAR);
        fputs (_("\
\n\
License GNU General Public License version 3 <https://gnu.org/licenses/gpl-3.0.htmpl>.\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n\
\n\
"), stdout);
        const char *const authors[] = { AUTHORS, NULL };
        size_t n_authors = countof (authors) - 1;
        static_assert (countof (authors) - 1 >= 1, "n_authors < 1");

        if (n_authors == 1)
          printf (_("Written by %s.\n"), authors[0]);
        else
          {
            printf (_("Written by "));
            for (size_t i = 0; i < n_authors - 1; i++)
              {
                printf ("%s,", authors[i]);
                putchar (i % 3 == 0 ? '\n' : ' ');
              }
            printf (_("and %s.\n"), authors[n_authors - 1]);
          }
        /* version_etc (stdout, PROGRAM_NAME, PACKAGE_NAME, Version, AUTHORS, (const char *) NULL); */
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

  /* Line buffer stdout to ensure lines are written atomically and immediately
     so that processes running in parallel do not intersperse their output.  */
  if (setvbuf (stdout, NULL, _IOLBF, 0) != 0)
    die (EXIT_FAILURE, errno, _("could not set buffering of stdout to mode _IOLBF"));

  parse_options (argc, argv);

  bool interactive = false;

  /* Interactive? */
  if (isatty (STDIN_FILENO) && isatty (STDOUT_FILENO))
    interactive = true;

  mpfr_t x, n;
  mpfr_inits (x, n, NULL);
  mpfr_set_ui (n, 12, mpfr_get_default_rounding_mode ());
  real_pow (x, n, n);

  char *s;
  //mpfr_out_str (stdout, 10, 0, x, mpfr_get_default_rounding_mode ());
  real_write (s = malloc (real_len (x)), x);
  puts (s);
  mpfr_out_str (stdout, 10, mpfr_get_default_prec (), x, mpfr_get_default_rounding_mode ());
  putchar ('\n');

  mpfr_set_inf (x, 0);
  real_write (s = malloc (real_len (x)), x);
  puts (s);

  if (interactive)
    main_loop ();

  return 0;
}
