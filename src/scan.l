/*  This file is part of pc.

    Copyright (C) 2019 Sergey Sushilin

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING.  If not, see
    <http://www.gnu.org/licenses/gpl-3.0-standalone.html>.  */

/* scan.l: the (f)lex description file for the scanner.  */

%{

#include "pc.h"
#include <errno.h>
#include <unistd.h>
#include "die.h"
#include "xalloc.h"
#include "util.h"
#include "parse.h"
#include "system.h"

/* Using flex, we can ask for a smaller input buffer.  With lex, this
   does nothing! */
#ifdef SMALL_BUF
# undef YY_READ_BUF_SIZE
# define YY_READ_BUF_SIZE 512
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Support for the readline and history libraries.  This allows
   nicer input on the interactive part of input.  */

/* Have input call the following function.  */
#undef  YY_INPUT
#define YY_INPUT(buf, result, max_size) \
  rl_input ((char *) buf, &result, max_size)

/* Variables to help interface readline with pc.  */
static char *rl_line  = NULL;
static char *rl_start = NULL;
static int   rl_len   = 0;

/* Definitions for readline access.  */
extern FILE *rl_instream;

/* rl_input puts upto MAX characters into BUF with the number put in
   BUF placed in *RESULT.  If the yy input file is the same as
   rl_instream (stdin), use readline.  Otherwise, just read it.  */
static void
rl_input (char *buf, int *result, int max)
{
  if (yyin != rl_instream)
    {
      while ((*result = read (fileno (yyin), buf, max)) < 0)
        if (errno != EINTR)
          {
            yyerror ("read() in flex scanner failed");
            exit (1);
          }
      return;
    }

  /* Do we need a new string? */
  if (rl_len == 0)
    {
      if (rl_start)
        free (rl_start);
      rl_start = readline (NULL);
      if (rl_start == NULL)
        {
          *result = '\0';
          rl_len = 0;
          return;
        }
      rl_line = rl_start;
      size_t len = strlen (rl_line);
      if (len >= INT_MAX)
        /* We must check the overflow because flex using int
           to save line length.  */
        die (EXIT_FAILURE, 0, "input line is too long");
      rl_len = (int) len + 1;
      if (rl_len != 1)
        add_history (rl_line);
      rl_line[rl_len - 1] = '\n';
      fflush (stdout);
      if (ferror (stdout) != 0)
        die (EXIT_FAILURE, errno, "stdout");
    }

  if (rl_len <= max)
    {
      strlcpy (buf, rl_line, rl_len);
      *result = rl_len;
      rl_len = 0;
    }
  else
    {
      strlcpy (buf, rl_line, max);
      *result = max;
      rl_line += max;
      rl_len -= max;
    }
}

%}

DIGIT [0-9A-Z]
LETTER [a-z]
%s slcomment
%option noyywrap

%%
"//" { BEGIN(slcomment); }
<slcomment>[^\n]* { BEGIN(INITIAL); }
<slcomment>"\n" { BEGIN(INITIAL); return(ENDOFLINE); }
fun       return(Fun);
var       return(Var);
break     return(Break);
quit|exit return(Quit);
length    return(Length);
return    return(Return);
for       return(For);
while     return(While);
if        return(If);
else      return(Else);
prec      return(Prec);
base      return(Base);
rnd       return(Rnd);

neg  return(Neg);
abs  return(Abs);
sign return(Sign);

round return(Round);
ceil  return(Ceil);
floor return(Floor);
trunc return(Trunc);
frac  return(Frac);

cmp    return(Cmp);
cmpabs return(Cmpabs);

pow  return(Pow);
root return(Root);

sqrt return(Sqrt);
cbrt return(Cbrt);

"<<" return(Lsh);
">>" return(Rsh);

exp return(Exp);
ln  return(Ln);

agm return(Agm);

sin return(Sin);
cos return(Cos);
tan return(Tan);
cot return(Cot);
sec return(Sec);
csc return(Csc);

arcsin|asin   return(Arcsin);
arccos|acos   return(Arccos);
arctan|atan   return(Arctan);
arctan2|atan2 return(Arctan2);
arccot|acot   return(Arccot);

sinh return(Sinh);
cosh return(Cosh);
tanh return(Tanh);
coth return(Coth);

sf return(Sf);
hf return(Hf);

gcd return(Gcd);
lcm return(Lcm);

mod return(Mod);

fibonacci  return(Fibonacci);
tribonacci return(Tribonacci);

beta  return(Beta);
gamma return(Gamma);
zeta  return(Zeta);

pi        return(Pi);
tau       return(Tau);
e         return(E);
y         return(Y);
catalan   return(Catalan);

@inf@|@Inf@|inf return(Inf);

warranty return(Warranty);
continue return(Continue);
print    return(Print);
"+"|"-"|"*"|"/"|"%"|";"|"("|")"|"{"|"}"|"["|"]"|","|"|"|"="|"!"|"#"|"|"|"&"|"^" { yylval.c_value = yytext[0]; return(yytext[0]); }
"&&"     return(AND);
"||"     return(OR);
"!!"     return(FFACT_OP);
"+="|"-="|"*="|"/="|"%="|"<<="|">>=" { yylval.c_value = yytext[0]; return(OP_AND_ASSIGN); }
"=="|"<="|">="|"!="|"<"|">" { yylval.s_value = xstrdup(yytext); return(COMPARE_OP); }
"<=>" { return(SPACESHIP_OP); }
"++"|"--" { yylval.c_value = yytext[0]; return(INCR_DECR); }
"\n" { return(ENDOFLINE); }
\\\n { /* Ignore a "quoted" newline.  */ }
[ \t]+ { /* Ignore spaces and tabs.  */ }
"/*" {
  int c;

  while (true)
    {
      while (((c = input()) != '*') && (c != EOF))
        ;
      if (c == '*')
        {
          while ((c = input()) == '*')
            /* Eat it.  */;
          if (c == '/')
            break; /* At end of comment.  */
        }
      if (c == EOF)
        {
          error (0, 0, "EOF encountered in a comment.");
          if (ferror (stderr) != 0)
            die (EXIT_FAILURE, errno, "stderr");
          break;
        }
    }
}
[a-zA-Z][a-zA-Z0-9_]* { yylval.s_value = xstrdup(yytext); return(NAME); }
\"[^\"]*\" {
  const char *look;
  size_t count = 0;
  yylval.s_value = xstrdup (yytext);
  for (look = yytext; *look != '\0'; look++)
    if (*look == '"')
      count++;
  if (count != 2)
    yyerror ("NUL character in string.");
  return(STRING);
}
{DIGIT}({DIGIT}|\\\n)*("."({DIGIT}|\\\n)*)?|"."(\\\n)*{DIGIT}({DIGIT}|\\\n)* {
  char *src, *dst;
  size_t len;
  /* Remove a trailing decimal point.  */
  len = strlen (yytext);
  if (yytext[len - 1] == '.')
    yytext[len - 1] = '\0';
  /* Remove leading zeros.  */
  src = yytext;
  dst = yytext;
  while (*src == '0')
    src++;
  if (unlikely (*src == '\0'))
    src--;
  /* Copy strings removing the newlines.  */
  while (*src != '\0')
    {
      if (unlikely (*src == '\\'))
        src += 2;
      if (unlikely (*src == ','))
        src++;
      else
        *dst++ = *src++;
    }
  *dst = '\0';
  yylval.s_value = xstrdup (yytext);
  return(NUMBER);
}

%%
