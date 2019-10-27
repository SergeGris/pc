/*  This file is part of pc.

    Copyright (C) 1991-1994, 1997, 2006, 2008, 2012-2017 Free Software Foundation, Inc.
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
    <http://www.gnu.org/licenses>.  */

%{

#include "pc.h"

#include <stdio.h>
#include <string.h>

#include "util.h"
#include "system.h"

/* current function number. */
long cur_func = -1;

/* Expression encoded information -- See comment at expression rules. */
#define EX_ASSGN 0
#define EX_REG   1
#define EX_COMP  2
#define EX_PAREN 4
#define EX_VOID  8
#define EX_EMPTY 16

%}

%start program

%union
{
  char *s_value;
  char c_value;
  long i_value;
  arg_list *a_value;
}

%token <i_value> ENDOFLINE AND OR NOT
%token <s_value> STRING NAME NUMBER
/*     '-', '+' are tokens themselves   */
/*     '=', '+=',  '-=', '*=', '/=', '%=', '^=' */
%token <c_value> ASSIGN_OP
/*     '==', '<=', '>=', '!=', '<', '>'   */
%token <s_value> REL_OP
/*     '++', '--'         */
%token <c_value> INCR_DECR
/*     'define', 'break', 'quit', 'length'  */
%token <i_value> Define    Break    Quit    Length
/*     'return', 'for', 'if', 'while', 'sqrt', 'else'   */
%token <i_value> Return    For    If    While    Sqrt   Else
/*     'scale', 'ibase', 'obase', 'auto', 'read', 'random'  */
%token <i_value> Scale Ibase Obase Auto    Read    Random
/*     'warranty', 'halt', 'last', 'continue', 'print', 'limits'   */
%token <i_value> Warranty  Halt  Last  Continue  Print  Limits
/*     'history', 'void' */
%token <i_value> UNARY_PLUS UNARY_MINUS HistoryVar Void

/* Types of all other things. */
%type <i_value> expression return_expression named_expression opt_expression
%type <c_value> '+' '-' '*' '/' '%'
%type <a_value> opt_parameter_list opt_auto_define_list define_list
%type <a_value> opt_argument_list argument_list
%type <i_value> program input_item semicolon_list statement_list
%type <i_value> statement function statement_or_error required_eol
%type <i_value> opt_void

/* precedence */
%left OR
%left AND
%nonassoc NOT
%left REL_OP
%right ASSIGN_OP
%left '+' '-'
%left '*' '/' '%'
%nonassoc UNARY_PLUS
%nonassoc UNARY_MINUS
%nonassoc INCR_DECR

%%
program: /* empty */
{ $$ = 0; welcome (); }

%%
