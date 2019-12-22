/*  This file is part of pc.

    Copyright (C) 2019 Sergey Sushilin

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License , or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING.  If not, see
    <http://www.gnu.org/licenses/gpl-3.0-standalone.html>.  */

%{
#include "pc.h"

#include <error.h>
#include <unistd.h>

#include "realis.h"
#include "util.h"

#include "quote.h"
#include "xalloc.h"

static variable_t *
get_variable (const char *name)
{
  if (!is_empty (variables))
    for (size_t i = 0; i < variables->n_used; i++)
      {
        variable_t *var = ((variable_t *) stack_at (variables, i));
        if (var != NULL && strcmp (var->name, name) == 0)
          return var;
      }

  return NULL;
}

void
pc_print (mpfr_ptr x)
{
  char *s = xmalloc (realis_len (x));
  realis_write (s, x);
  puts (s);
  free (s);
}

#define INIT(t)                             \
  if (__get_cerrno () == 0)                 \
    {                                       \
      mpfr_ptr t = xmalloc (sizeof (*t));   \
      realis_init (t);                      \

#define FINI(t) \
      stack_push (number_stack, t); \
      t = NULL; \
    }

#define POP1(t) \
  mpfr_ptr t = (mpfr_ptr) stack_pop (number_stack)
#define POP2(t1, t2) \
  mpfr_ptr t2 = (mpfr_ptr) stack_pop (number_stack); \
  mpfr_ptr t1 = (mpfr_ptr) stack_pop (number_stack)
#define PEEK() \
  (mpfr_ptr) stack_peek (number_stack)
#define CLEAR1(t) \
  realis_clear (t); \
  free (t)
#define CLEAR2(t1, t2) \
  realis_clear (t1); \
  realis_clear(t2);  \
  free (t1); \
  free (t2)

#define CONST_FUNCTION(fn)                         \
  do                                               \
    {                                              \
      INIT (t);                                    \
      realis_##fn (t);                             \
      FINI (t);                                    \
    }                                              \
  while (0)
#define UNARY_FUNCTION(fn)                      \
  do                                            \
    {                                           \
      INIT (t);                                 \
      POP1 (t1);                                \
      realis_##fn (t, t1);                      \
      CLEAR1 (t1);                              \
      FINI (t);                                 \
    }                                           \
  while (0)
#define BINARY_FUNCTION(fn)                     \
  do                                            \
    {                                           \
      INIT (t);                                 \
      POP2 (t1, t2);                            \
      realis_##fn (t, t1, t2);                  \
      CLEAR2 (t1, t2);                          \
      FINI (t);                                 \
    }                                           \
  while (0)

%}

%union
{
  char *s_value;
  long int i_value;
  unsigned char c_value;
}

%defines

%token <i_value> ENDOFLINE AND OR UNARY_MINUS FFACT_OP
%token <s_value> STRING NAME Z_NUMBER NUMBER POSTFIX
%token <c_value> OP_AND_ASSIGN /* '+=', '-=', '*=', '/=', '%=' */
%token <s_value> COMPARE_OP /* '==', '<=', '>=', '!=', '<', '>' */
%token <i_value> SPACESHIP_OP /* <=> */
%token <c_value> INCR_DECR
%token <i_value> Fun Var Quit Length
%token <i_value> Return For While If Else Break Continue
%token <i_value> Prec Base Rnd
%token <i_value> Warranty Print

%token <i_value> Neg Abs Sign
%token <i_value> Round Ceil Floor Trunc Frac
%token <i_value> Cmp Cmpabs
%token <i_value> Pow Root
%token <i_value> Sqrt Cbrt
%token <i_value> Lsh Rsh
%token <i_value> Exp Ln
%token <i_value> Agm
%token <i_value> Sin Cos Tan Cot Sec Csc
%token <i_value> Arcsin Arccos Arctan Arctan2 Arccot
%token <i_value> Sinh Cosh Tanh Coth
%token <i_value> Sf Hf
%token <i_value> Gcd Lcm
%token <i_value> Mod
%token <i_value> Fibonacci Tribonacci
%token <i_value> Beta Gamma Zeta
%token <i_value> Pi Tau E Y Catalan
%token <i_value> Inf

%type <i_value> stmt
%type <i_value> expr
%type <c_value> '+' '-' '*' '/' '%' '=' '!' '#' '|' '&' '^'
%type <i_value> program

/* Precedence.  */
%left COMPARE_OP SPACESHIP_OP
%right OP_AND_ASSIGN '='
%left OR AND '|' '&' '^'
%left '+' '-'
%left '*' '/' '%' Lsh Rsh
%nonassoc Mod
%left UNARY_MINUS
%left '!' FFACT_OP '#'
%nonassoc INCR_DECR

%start program

%%
program:
{ $$ = 0; }
| program input_line
;
input_line:
ENDOFLINE
| expr ENDOFLINE
{
  if (__get_cerrno() == 0)
    {
      POP1 (t);
      pc_print (t);
      CLEAR1 (t);
    }
  else
    __set_cerrno (0);
}
| stmt ENDOFLINE
| error ENDOFLINE { yyerrok; }
;
expr:
NUMBER { INIT (t); realis_read (t, $1); FINI (t); free ($1); }
| Inf { INIT (t); realis_set_inf (t, MPFR_SIGN_POS); FINI (t); }
| NAME
{
  INIT (t);
  variable_t *v = get_variable ($1);
  if (v == NULL)
    cerrorf (unknown_variable, "%s", quote ($1));
  else
    realis_copy (t, v->val);
  free ($1);
  FINI (t);
}
| NAME '(' expr ')' { INIT (t); POP1 (t1); cerrorf (unknown_function, "%s", quote ($1)); CLEAR1 (t1); FINI (t); }
| NAME '(' expr ',' expr ')' { INIT (t); POP2 (t1, t2); cerrorf (unknown_function, "%s", quote ($1)); CLEAR2 (t1, t2); FINI (t); }
| expr '+' expr { BINARY_FUNCTION (add); }
| expr '-' expr { BINARY_FUNCTION (sub); }
| expr '*' expr { BINARY_FUNCTION (mul); }
| expr '/' expr { BINARY_FUNCTION (div); }
| expr '%' expr { BINARY_FUNCTION (rem); }
| '+' expr { $$ = 0; }
| '-' expr %prec UNARY_MINUS { UNARY_FUNCTION (neg); }
| '(' expr ')' { $$ = 0; }
| Abs  '(' expr ')' { UNARY_FUNCTION (abs); }
| '|' expr '|' { UNARY_FUNCTION (abs); }
| Sign '(' expr ')' { UNARY_FUNCTION (sign); }
| Round '(' expr ')' { UNARY_FUNCTION (round); }
| Ceil  '(' expr ')' { UNARY_FUNCTION (ceil);  }
| Floor '(' expr ')' { UNARY_FUNCTION (floor); }
| Trunc '(' expr ')' { UNARY_FUNCTION (trunc); }
| Frac  '(' expr ')' { UNARY_FUNCTION (frac);  }
| Pow  '(' expr ',' expr ')' { BINARY_FUNCTION (pow); }
| Root '(' expr ',' expr ')' { BINARY_FUNCTION (root); }
| Sqrt '(' expr ')' { UNARY_FUNCTION (sqrt); }
| Cbrt '(' expr ')' { UNARY_FUNCTION (cbrt); }
| expr Lsh expr { BINARY_FUNCTION (lsh); }
| expr Rsh expr { BINARY_FUNCTION (rsh); }
| Exp '(' expr ')' { UNARY_FUNCTION (exp); }
| Ln '(' expr ')'  { UNARY_FUNCTION (ln); }
| Agm '(' expr ',' expr ')' { BINARY_FUNCTION (agm); }
| Sin '(' expr ')' { UNARY_FUNCTION (sin); }
| Cos '(' expr ')' { UNARY_FUNCTION (cos); }
| Tan '(' expr ')' { UNARY_FUNCTION (tan); }
| Cot '(' expr ')' { UNARY_FUNCTION (cot); }
| Sec '(' expr ')' { UNARY_FUNCTION (sec); }
| Csc '(' expr ')' { UNARY_FUNCTION (csc); }
| Arcsin  '(' expr ')' { UNARY_FUNCTION (asin); }
| Arccos  '(' expr ')' { UNARY_FUNCTION (acos); }
| Arctan  '(' expr ')' { UNARY_FUNCTION (atan); }
| Arctan2 '(' expr ',' expr ')' { BINARY_FUNCTION (atan2); }
| Arccot  '(' expr ')' { UNARY_FUNCTION (acot); }
| Sinh '(' expr ')' { UNARY_FUNCTION (sinh); }
| Cosh '(' expr ')' { UNARY_FUNCTION (cosh); }
| Tanh '(' expr ')' { UNARY_FUNCTION (tanh); }
| Coth '(' expr ')' { UNARY_FUNCTION (coth); }
| expr '!' { UNARY_FUNCTION (fact); }
| expr FFACT_OP { UNARY_FUNCTION (ffact); }
| '!' expr { UNARY_FUNCTION (subfact); }
| expr '#' { UNARY_FUNCTION (primorial); }
| Sf '(' expr ')' { UNARY_FUNCTION (superfactorial); }
| Hf '(' expr ')' { UNARY_FUNCTION (hyperfactorial); }
| Gcd '(' expr ',' expr ')' { BINARY_FUNCTION (gcd); }
| Lcm '(' expr ',' expr ')' { BINARY_FUNCTION (lcm); }
| expr Mod expr { BINARY_FUNCTION (mod); }
| Fibonacci '(' expr ')' { UNARY_FUNCTION (fibonacci); }
| Tribonacci '(' expr ')'{ UNARY_FUNCTION (tribonacci); }
| Beta '(' expr ',' expr ')' { BINARY_FUNCTION (beta); }
| Gamma '(' expr ')' { UNARY_FUNCTION (gamma); }
| Zeta '(' expr ')' { UNARY_FUNCTION (zeta); }
| Pi  { CONST_FUNCTION (pi); }
| Tau { CONST_FUNCTION (tau); }
| E   { CONST_FUNCTION (e); }
| Y   { CONST_FUNCTION (y); }
| Catalan { CONST_FUNCTION (catalan); }
| expr COMPARE_OP expr
{
  INIT (t);
  POP2 (t1, t2);
  int diff = realis_cmp (t1, t2);

  if ($2[0] == '=') /* "==" */
    realis_set_ui (t, diff == 0 ? 1 : 0);
  else
    {
      if ($2[0] == '!') /* "!=" */
        realis_set_ui (t, diff != 0 ? 1 : 0);
      else if ($2[0] == '<')
        {
          if ($2[1] == '=')
            realis_set_ui (t, diff <= 0 ? 1 : 0);
          else /* $2[1] == '\0' */
            realis_set_ui (t, diff < 0 ? 1 : 0);
        }
      else /* $2[0] == '>' */
        {
          if ($2[1] == '=')
            realis_set_ui (t, diff >= 0 ? 1 : 0);
          else /* $2[1] == '\0' */
            realis_set_ui (t, diff > 0 ? 1 : 0);
        }
    }

  CLEAR2 (t1, t2);
  FINI (t);
  free ($2);
}
| expr SPACESHIP_OP expr
{
  INIT (t);
  POP2 (t1, t2);
  realis_init_set_si (t, realis_cmp (t1, t2));
  CLEAR2 (t1, t2);
  FINI (t);
}
| expr '|' expr { INIT (t); POP2 (t1, t2); realis_or (t, t1, t2); CLEAR2 (t1, t2); FINI (t); }
| expr '&' expr { INIT (t); POP2 (t1, t2); realis_and (t, t1, t2); CLEAR2 (t1, t2); FINI (t); }
| expr '^' expr { INIT (t); POP2 (t1, t2); realis_xor (t, t1, t2); CLEAR2 (t1, t2); FINI (t); }
| expr OR expr  { INIT (t); POP2 (t1, t2); realis_init_set_ui (t, MPFR_NOT_ZERO (t1) || MPFR_NOT_ZERO (t2)); CLEAR2 (t1, t2); FINI (t); }
| expr AND expr { INIT (t); POP2 (t1, t2); realis_init_set_ui (t, MPFR_NOT_ZERO (t1) && MPFR_NOT_ZERO (t2)); CLEAR2 (t1, t2); FINI (t); }
| INCR_DECR NAME
{
  INIT (t);
  variable_t *v = get_variable ($2);
  if (v == NULL)
    cerrorf (unknown_variable, "%s", quote ($2));
  mpfr_ptr x = v->val;
  if (!mpfr_integer_p (x))
    cerror (function_does_not_support_fractional);
  else
    {
      if ($1 == '+')
        realis_add_ui (t, x, 1);
      else
        realis_sub_ui (t, x, 1);
    }
  realis_copy (x, t);
  FINI (t);

  free ($2);
}
| NAME '=' expr
{
  INIT (t);
  POP1 (t1);
  variable_t *v = get_variable ($1);
  if (v == NULL)
    cerrorf (unknown_variable, "%s", quote ($1));
  else
    {
      realis_copy (v->val, t1);
      realis_copy (t, t1);
    }
  free ($1);
  CLEAR1 (t1);
  FINI (t);
}
;
stmt:
Warranty { warranty (); }
| Prec '=' NUMBER { mpfr_set_default_prec (digits2bits (strtoul ($3, NULL, base) + 1)); free ($3); }
| Base '=' NUMBER { base = strtoul ($3, NULL, base); free ($3); }
| Rnd '=' NAME { mpfr_set_default_rounding_mode (string2rounding ($3)); free ($3); }
| Print '(' STRING ')' { size_t len = strlen ($3); $3[len - 1] = '\n'; write (fileno (stdout), $3 + 1, len - 1); free ($3); }
| Quit { exit (0); }
| Quit '(' ')' { exit (0); }
| Quit '(' NUMBER ')' { int exit_code = strtol ($3, NULL, base); free ($3); exit (exit_code); }
| Var NAME '=' expr
{
  POP1 (t1);
  variable_t *v = get_variable ($2);
  if (v != NULL)
    cerrorf (redeclaration_of_variable, "%s", quote ($2));
  else
    {
      variable_t *var = xmalloc (sizeof (*var));
      var->name = xstrdup ($2);
      realis_init (var->val);
      realis_copy (var->val, t1);
      stack_push (variables, var);
    }
  free ($2);
  CLEAR1 (t1);
}
| NAME OP_AND_ASSIGN expr
{
  POP1 (t1);
  variable_t *v = get_variable ($1);
  if (v == NULL)
    cerrorf (unknown_variable, "%s", quote ($1));
  else
    {
      mpfr_ptr t2 = v->val;
      switch ($2)
        {
        case '+':
          realis_add (t2, t2, t1);
          break;
        case '-':
          realis_sub (t2, t2, t1);
          break;
        case '*':
          realis_mul (t2, t2, t1);
          break;
        case '/':
          realis_div (t2, t2, t1);
          break;
        case '%':
          realis_rem (t2, t2, t1);
          break;
        case '<':
          realis_lsh (t2, t2, t1);
          break;
        case '>':
          realis_rsh (t2, t2, t1);
          break;
        }
    }
  free ($1);
  CLEAR1 (t1);
}
;
%%
