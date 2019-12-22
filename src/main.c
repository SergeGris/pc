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
 * along with this program.  If not, see <https://www.gnu.org/licenses/gpl-3.0-standalone.html>.
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

#include "realis.h"
#include "complex.h"
#include "util.h"
#include "parse.h"

#include "die.h"
#include "long-options.h"
#include "quote.h"
#include "xalloc.h"
#include "xdectoint.h"
#include "yesno.h"

#include "system.h"

/* For long options that have no equivalent short option, use a
   non-character as a pseudo short option, starting with CHAR_MAX + 1.  */
enum
{
  EXPRESSION_OPTION = CHAR_MAX + 1,
  HELP_OPTION,
  VERSION_OPTION,
  WARRANTY_OPTION
};

static const struct option long_options[] =
{
  {"precision", required_argument, NULL, 'p'},
  {"rounding", required_argument, NULL, 'r'},
  {"angle", required_argument, NULL, 'a'},
  {"mode", required_argument, NULL, 'm'},
  {"help", no_argument, NULL, HELP_OPTION},
  {"version", no_argument, NULL, VERSION_OPTION},
  {"warranty", no_argument, NULL, WARRANTY_OPTION},
  {NULL, 0, NULL, '\0'}
};
const char *const short_options = "a:m:p:r:i:o:";

void
usage (int status)
{
  puts ("pc -- is the best calculator 4 you");

  exit (status);
}

void
interrupt_handler (int signo
#if HAVE_SIGACTION && HAVE_STRUCT_SIGACTION_SA_SIGACTION
                   , siginfo_t *siginfo
                   , void *context
#endif
                   )
{
  int saved_errno = errno;
  if (signo == SIGINT)
    {
      char msg[] = "\nuse 'quit' to exit\n";
      write (STDERR_FILENO, msg, sizeof (msg) - 1);
#if !HAVE_SIGACTION || !HAVE_STRUCT_SIGACTION_SA_SIGACTION
      if (signal (SIGINT, interrupt_handler) == SIG_ERR)
        error (0, errno, "signal()");
#endif
    }
  else
    assert (false);
  errno = saved_errno;
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
  if (fd >= 0 && close (fd) != 0)
    error (0, errno, "%s", quotef (filename));

  return -1;
}

/* -1 -- undefined; 0 -- false; 1 -- true.  */
static int interactive = -1;

static void
parse_options (int argc, char **argv)
{
  int optc;

  while ((optc = getopt_long (argc, argv, short_options, long_options, NULL)) > 0)
    switch (optc)
      {
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
        if ((yyin = rl_instream = fopen (optarg, "r")) == NULL)
          die (EXIT_FAILURE, errno, "cannot open file %s", quoteaf (optarg));
        break;
      case HELP_OPTION:
        usage (EXIT_SUCCESS);
      case VERSION_OPTION:
        version ();
        exit (EXIT_SUCCESS);
      case WARRANTY_OPTION:
        warranty ();
        exit (EXIT_SUCCESS);
      default:
        usage (EXIT_TROUBLE);
      }
}

static void
variables_destructor (void *p)
{
  variable_t *v = p;
  free (v->name);
  realis_clear (v->val);
  free (p);
}
static void
number_stack_destructor (void *p)
{
  mpfr_ptr t = p;
  realis_clear (t);
  free (t);
}

int
main (int argc, char **argv)
{
#if HAVE_SIGACTION && HAVE_STRUCT_SIGACTION_SA_SIGACTION
  struct sigaction act;
#endif

  /* Setting values of global variables.  */
  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  if (setvbuf (stdout, NULL, _IONBF, 0) != 0)
    error (0, errno, _("could not set buffering of stdout to mode _IONBF"));

  /* Interactive? */
  if (interactive < 0 && isatty (STDIN_FILENO) && isatty (STDOUT_FILENO))
    interactive = 1;

  /* Readline support.  Set both application name and input file.  */
  rl_readline_name = PROGRAM_NAME;
  rl_instream = stdin;

  using_history ();

  mpfr_set_default_prec (digits2bits (100 + 1));
  mpfr_set_default_rounding_mode (MPFR_RNDNA);

  parse_options (argc, argv);

  if (interactive)
    {
      puts ("This is free software with ABSOLUTELY NO WARRANTY.");
      printf ("For more details type %s.\n", quote ("warranty"));

#if HAVE_SIGACTION && HAVE_STRUCT_SIGACTION_SA_SIGACTION
      memset (&act, '\0', sizeof (act));
      sigemptyset (&act.sa_mask);
      act.sa_sigaction = interrupt_handler;
      act.sa_flags = SA_SIGINFO;
      if (sigaction (SIGINT, &act, NULL) < 0)
        error (0, errno, "sigaction()");
#else
      if (signal (SIGINT, interrupt_handler) == SIG_ERR)
        error (0, errno, "signal()");
#endif
    }

  rl_bind_key ('\t', rl_insert);

  /* mpfr_t half = {{1, MPFR_SIGN_POS, 1, (mp_limb_t [1]) {((size_t) (~(mp_limb_t)0) / 4)}}}; */
  mpfr_init2 (one, 1);
  realis_set_ui (one, 1);

  stack_init (variables, variables_destructor);
  stack_init (number_stack, number_stack_destructor);

  int status = yyparse ();

  stack_clear (number_stack);
  stack_clear (variables);

  realis_clear (one);

  return status;
}
