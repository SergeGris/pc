/*  This file is part of GNU bc.

    Copyright (C) 1991-1994, 1997, 2006, 2008, 2012-2017 Free Software Foundation, Inc.

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
    <http://www.gnu.org/licenses>.

    You may contact the author by:
       e-mail:  philnelson@acm.org
      us-mail:  Philip A. Nelson
                Computer Science Department, 9062
                Western Washington University
                Bellingham, WA 98226-9062

*************************************************************************/

/* scan.l: the (f)lex description file for the scanner. */

%{

#include "pc.h"
#include <errno.h>
#include <unistd.h>
#include "die.h"
#include "xalloc.h"
#include "util.h"
#include "parse.h"
#include "system.h"

file_node *file_names = NULL;
char *file_name = "standard input";
int line_no = 0;

/* Using flex, we can ask for a smaller input buffer.  With lex, this
   does nothing! */
#ifdef SMALL_BUF
# undef YY_READ_BUF_SIZE
# define YY_READ_BUF_SIZE 512
#endif

/* Force . as last for now. */
#define DOT_IS_LAST

/* We want to define our own yywrap. */
#undef yywrap
int yywrap (void);

/* Support for the readline and history libraries.  This allows
   nicer input on the interactive part of input. */

/* Have input call the following function. */
#undef  YY_INPUT
#define YY_INPUT(buf, result, max_size) \
  rl_input ((char *) buf, &result, max_size)

/* Variables to help interface readline with pc. */
static char *rl_line  = (char *) 0;
static char *rl_start = (char *) 0;
static int   rl_len   = 0;

/* Definitions for readline access. */
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
        free(rl_start);
      rl_start = readline (NULL);
      if (rl_start == NULL)
        {
          /* end of file */
          *result = '\0';
          rl_len = 0;
          return;
        }
      rl_line = rl_start;
      rl_len = (int) strlen (rl_line) + 1; /* Oh, flex, come on! */
      if (rl_len != 1)
        add_history (rl_line);
      rl_line[rl_len - 1] = '\n';
      fflush (stdout);
      checkferror_output(stdout);
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
%%
"#" { BEGIN(slcomment); }
<slcomment>[^\n]* { BEGIN(INITIAL); }
<slcomment>"\n" { line_no++; BEGIN(INITIAL); return(ENDOFLINE); }
define  { return(Define); }
break   { return(Break); }
quit    { return(Quit); }
length  { return(Length); }
return  { return(Return); }
for     { return(For); }
if      { return(If); }
while   { return(While); }
sqrt    { return(Sqrt); }
scale   { return(Scale); }
ibase   { return(Ibase); }
obase   { return(Obase); }
auto    { return(Auto); }
else   { return(Else); }
read    { return(Read); }
random  { return(Random); }
halt    { return(Halt); }
last    { return(Last); }
void    { return(Void); }
history { return(HistoryVar); }

warranty return(Warranty);
continue return(Continue);
print  return(Print);
limits return(Limits);
"." {
#ifdef DOT_IS_LAST
       return(Last);
#else
       yyerror ("illegal character: %s",yytext);
#endif
    }
"+"|"-"|";"|"("|")"|"{"|"}"|"["|"]"|","|"^" { yylval.c_value = yytext[0]; return ((int) yytext[0]); }
&& { return(AND); }
\|\| { return(OR); }
"!" { return(NOT); }
"*"|"/"|"%"|"&" { yylval.c_value = yytext[0]; return((int) yytext[0]); }
"="|\+=|-=|\*=|\/=|%=|\^=  { yylval.c_value = yytext[0]; return(ASSIGN_OP); }
=\+|=-|=\*|=\/|=%|=\^  {
       yylval.c_value = '=';
       yyless (1);
       return(ASSIGN_OP);
           }
==|\<=|\>=|\!=|"<"|">" { yylval.s_value = xstrdup(yytext); return(REL_OP); }
\+\+|-- { yylval.c_value = yytext[0]; return(INCR_DECR); }
"\n" { line_no++; return(ENDOFLINE); }
\\\n { line_no++; /* ignore a "quoted" newline */ }
[ \t]+ { /* ignore spaces and tabs */ }
"/*" {
  int c;

  while (1)
    {
      while (((c = input()) != '*') && (c != EOF))
        {
          /* eat it */
          if (c == '\n')
            line_no++;
        }
      if (c == '*')
        {
          while ((c = input()) == '*')
            /* eat it */;
          if (c == '/')
            break; /* at end of comment */
          if (c == '\n')
            line_no++;
        }
      if (c == EOF)
        {
          fprintf (stderr, "EOF encountered in a comment.\n");
          checkferror_output (stderr);
          break;
        }
    }
      }
[a-z][a-z0-9_]* { yylval.s_value = xstrdup(yytext); return(NAME); }
\"[^\"]*\" {
        const char *look;
        int count = 0;
        yylval.s_value = xstrdup(yytext);
        for (look = yytext; *look != 0; look++)
          {
            if (*look == '\n')
              line_no++;
            if (*look == '"')
              count++;
          }
        if (count != 2)
          yyerror ("NUL character in string.");
        return(STRING);
      }
{DIGIT}({DIGIT}|\\\n)*("."({DIGIT}|\\\n)*)?|"."(\\\n)*{DIGIT}({DIGIT}|\\\n)* {
        char *src, *dst;
        size_t len;
        /* remove a trailing decimal point. */
        len = strlen(yytext);
        if (yytext[len-1] == '.')
          yytext[len-1] = 0;
        /* remove leading zeros. */
        src = yytext;
        dst = yytext;
        while (*src == '0')
          src++;
        if (*src == '\0')
          src--;
        /* Copy strings removing the newlines. */
        while (*src != '\0')
          {
            if (*src == '\\')
              {
                src += 2;
                line_no++;
              }
            if (*src == ',')
              src++;
            else
              *dst++ = *src++;
          }
        *dst = 0;
        yylval.s_value = xstrdup(yytext);
        return(NUMBER);
      }
. {
   if (yytext[0] < ' ')
     yyerror ("illegal character: ^%c",yytext[0] + '@');
   else if (yytext[0] > '~')
     yyerror ("illegal character: \\%03o", (int) yytext[0]);
   else
     yyerror ("illegal character: %s", yytext);
}
%%

/* This is the function that opens all the files.
   It returns true if the file was opened, otherwise
   it returns false.  */
int
open_new_file (void)
{
  FILE *new_file;
  file_node *temp;

  /* Set the line number. */
  line_no = 1;

  /* Check to see if we are done. */
  if (yyin == stdin)
    return false;

  /* One of the argv values. */
  if (file_names != NULL)
    {
      new_file = fopen (file_names->name, "r");
      if (new_file != NULL)
        {
          new_yy_file (new_file);
          temp = file_names;
          file_name  = temp->name;
          file_names = temp->next;
          free (temp);
          return true;
        }
      die (1, 0, "File %s is unavailable.\n", file_names->name);
    }

  /* If we fall through to here, we should return stdin.  */
  new_yy_file (stdin);
  return true;
}

/* Set yyin to the new file. */
void
new_yy_file (FILE *file)
{
  if (unlikely (yyin != stdin))
    if (fclose (yyin) != 0)
      error (0, errno, "(standard input)");

  yyin = file;
}

/* This is the way to get multiple files input into lex.  */
int
yywrap (void)
{
  if (!open_new_file ())
    return 1;        /* EOF on standard in.  */
  return 0;          /* We have more input.  */
  yyunput (0, NULL); /* Make sure the compiler think yyunput is used.  */
}
