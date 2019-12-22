/* A Bison parser, made by GNU Bison 3.4.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_SRC_PARSE_H_INCLUDED
# define YY_YY_SRC_PARSE_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ENDOFLINE = 258,
    AND = 259,
    OR = 260,
    UNARY_MINUS = 261,
    FFACT_OP = 262,
    STRING = 263,
    NAME = 264,
    Z_NUMBER = 265,
    NUMBER = 266,
    POSTFIX = 267,
    OP_AND_ASSIGN = 268,
    COMPARE_OP = 269,
    SPACESHIP_OP = 270,
    INCR_DECR = 271,
    Fun = 272,
    Var = 273,
    Quit = 274,
    Length = 275,
    Return = 276,
    For = 277,
    While = 278,
    If = 279,
    Else = 280,
    Break = 281,
    Continue = 282,
    Prec = 283,
    Base = 284,
    Rnd = 285,
    Warranty = 286,
    Print = 287,
    Neg = 288,
    Abs = 289,
    Sign = 290,
    Round = 291,
    Ceil = 292,
    Floor = 293,
    Trunc = 294,
    Frac = 295,
    Cmp = 296,
    Cmpabs = 297,
    Pow = 298,
    Root = 299,
    Sqrt = 300,
    Cbrt = 301,
    Lsh = 302,
    Rsh = 303,
    Exp = 304,
    Ln = 305,
    Agm = 306,
    Sin = 307,
    Cos = 308,
    Tan = 309,
    Cot = 310,
    Sec = 311,
    Csc = 312,
    Arcsin = 313,
    Arccos = 314,
    Arctan = 315,
    Arctan2 = 316,
    Arccot = 317,
    Sinh = 318,
    Cosh = 319,
    Tanh = 320,
    Coth = 321,
    Sf = 322,
    Hf = 323,
    Gcd = 324,
    Lcm = 325,
    Mod = 326,
    Fibonacci = 327,
    Tribonacci = 328,
    Beta = 329,
    Gamma = 330,
    Zeta = 331,
    Pi = 332,
    Tau = 333,
    E = 334,
    Y = 335,
    Catalan = 336,
    Inf = 337
  };
#endif
/* Tokens.  */
#define ENDOFLINE 258
#define AND 259
#define OR 260
#define UNARY_MINUS 261
#define FFACT_OP 262
#define STRING 263
#define NAME 264
#define Z_NUMBER 265
#define NUMBER 266
#define POSTFIX 267
#define OP_AND_ASSIGN 268
#define COMPARE_OP 269
#define SPACESHIP_OP 270
#define INCR_DECR 271
#define Fun 272
#define Var 273
#define Quit 274
#define Length 275
#define Return 276
#define For 277
#define While 278
#define If 279
#define Else 280
#define Break 281
#define Continue 282
#define Prec 283
#define Base 284
#define Rnd 285
#define Warranty 286
#define Print 287
#define Neg 288
#define Abs 289
#define Sign 290
#define Round 291
#define Ceil 292
#define Floor 293
#define Trunc 294
#define Frac 295
#define Cmp 296
#define Cmpabs 297
#define Pow 298
#define Root 299
#define Sqrt 300
#define Cbrt 301
#define Lsh 302
#define Rsh 303
#define Exp 304
#define Ln 305
#define Agm 306
#define Sin 307
#define Cos 308
#define Tan 309
#define Cot 310
#define Sec 311
#define Csc 312
#define Arcsin 313
#define Arccos 314
#define Arctan 315
#define Arctan2 316
#define Arccot 317
#define Sinh 318
#define Cosh 319
#define Tanh 320
#define Coth 321
#define Sf 322
#define Hf 323
#define Gcd 324
#define Lcm 325
#define Mod 326
#define Fibonacci 327
#define Tribonacci 328
#define Beta 329
#define Gamma 330
#define Zeta 331
#define Pi 332
#define Tau 333
#define E 334
#define Y 335
#define Catalan 336
#define Inf 337

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 113 "src/parse.y"

  char *s_value;
  long int i_value;
  unsigned char c_value;

#line 227 "src/parse.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SRC_PARSE_H_INCLUDED  */
