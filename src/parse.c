/* A Bison parser, made by GNU Bison 3.4.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.4.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 19 "src/parse.y"

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


#line 163 "src/parse.c"

# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
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

#line 376 "src/parse.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SRC_PARSE_H_INCLUDED  */



#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   3111

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  97
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  5
/* YYNRULES -- Number of rules.  */
#define YYNRULES  90
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  266

#define YYUNDEFTOK  2
#define YYMAXUTOK   337

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  ((unsigned) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    89,     2,    90,     2,    87,    92,     2,
      94,    95,    85,    83,    96,    84,     2,    86,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    88,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    93,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    91,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   171,   171,   172,   175,   176,   187,   188,   191,   192,
     193,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   297,   305,   306,   307,   308,   309,   310,
     331,   349,   350,   351,   352,   353,   354,   355,   356,   357,
     374
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ENDOFLINE", "AND", "OR", "UNARY_MINUS",
  "FFACT_OP", "STRING", "NAME", "Z_NUMBER", "NUMBER", "POSTFIX",
  "OP_AND_ASSIGN", "COMPARE_OP", "SPACESHIP_OP", "INCR_DECR", "Fun", "Var",
  "Quit", "Length", "Return", "For", "While", "If", "Else", "Break",
  "Continue", "Prec", "Base", "Rnd", "Warranty", "Print", "Neg", "Abs",
  "Sign", "Round", "Ceil", "Floor", "Trunc", "Frac", "Cmp", "Cmpabs",
  "Pow", "Root", "Sqrt", "Cbrt", "Lsh", "Rsh", "Exp", "Ln", "Agm", "Sin",
  "Cos", "Tan", "Cot", "Sec", "Csc", "Arcsin", "Arccos", "Arctan",
  "Arctan2", "Arccot", "Sinh", "Cosh", "Tanh", "Coth", "Sf", "Hf", "Gcd",
  "Lcm", "Mod", "Fibonacci", "Tribonacci", "Beta", "Gamma", "Zeta", "Pi",
  "Tau", "E", "Y", "Catalan", "Inf", "'+'", "'-'", "'*'", "'/'", "'%'",
  "'='", "'!'", "'#'", "'|'", "'&'", "'^'", "'('", "')'", "','", "$accept",
  "program", "input_line", "expr", "stmt", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,    43,    45,    42,    47,    37,    61,    33,
      35,   124,    38,    94,    40,    41,    44
};
# endif

#define YYPACT_NINF -90

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-90)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  (!!((Yytable_value) == (-1)))

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -90,   220,   -90,     0,   -90,    53,   -90,    -7,    -5,   -89,
     -82,   -80,   -79,   -90,   -84,   -83,   -81,   -78,   -77,   -76,
     -75,   -74,   -73,   -72,   -71,   -70,   -69,   -68,   -67,   -66,
     -65,   -64,   -62,   -61,   -60,   -59,   -58,   -57,   -55,   -54,
     -53,   -52,   -51,   -50,   -49,   -48,   -47,   -46,   -45,   -44,
     -43,   -42,   -41,   -90,   -90,   -90,   -90,   -90,   -90,  2941,
    2941,  2941,  2941,  2941,   -90,  2833,     9,   -90,  2941,  2941,
    2941,   -90,   -33,    20,     3,     4,    48,    55,  2941,  2941,
    2941,  2941,  2941,  2941,  2941,  2941,  2941,  2941,  2941,  2941,
    2941,  2941,  2941,  2941,  2941,  2941,  2941,  2941,  2941,  2941,
    2941,  2941,  2941,  2941,  2941,  2941,  2941,  2941,  2941,  2941,
    2941,  2941,  2941,  2941,  2941,  2941,   -87,   616,    31,   -90,
    2850,   652,   -90,  2941,  2941,   -90,  2941,  2941,  2941,  2941,
    2941,  2941,  2941,  2941,  2941,  2941,   -90,   -90,  2941,  2941,
    2941,   -90,  2880,    69,   301,  2941,   -39,   -90,   -90,   -90,
     -90,   -31,   669,   761,   778,   870,   887,   979,   996,   318,
     408,  1088,  1105,  1197,  1214,   425,  1306,  1323,  1415,  1432,
    1524,  1541,  1633,  1650,  1742,   455,  1759,  1851,  1868,  1960,
    1977,  2069,  2086,   515,   545,  2178,  2195,   562,  2287,  2304,
    3017,   -90,   643,   643,    69,    69,    47,    47,   112,   616,
     616,    47,    47,    47,   643,   643,   643,   -90,  2941,  2880,
     -90,   -90,   -90,   -90,   -90,   -90,   -90,   -90,   -90,  2941,
    2941,   -90,   -90,   -90,   -90,  2941,   -90,   -90,   -90,   -90,
     -90,   -90,   -90,   -90,   -90,  2941,   -90,   -90,   -90,   -90,
     -90,   -90,   -90,  2941,  2941,   -90,   -90,  2941,   -90,   -90,
    2396,  2413,  2505,  2522,  2614,  2631,  2723,  2740,   -90,   -90,
     -90,   -90,   -90,   -90,   -90,   -90
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     4,    10,     8,     0,     0,    86,
       0,     0,     0,    81,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    67,    68,    69,    70,    71,     9,     0,
       0,     0,     0,     0,     3,     0,     0,     7,     0,     0,
       0,    79,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    10,    18,    19,    55,
       0,     0,     5,     0,     0,    54,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    53,    56,     0,     0,
       0,     6,    90,    80,     0,     0,     0,    87,    82,    83,
      84,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      22,    20,    78,    77,    72,    73,    33,    34,    61,    13,
      14,    15,    16,    17,    74,    75,    76,    11,     0,    89,
      88,    85,    21,    23,    24,    25,    26,    27,    28,     0,
       0,    31,    32,    35,    36,     0,    38,    39,    40,    41,
      42,    43,    44,    45,    46,     0,    48,    49,    50,    51,
      52,    57,    58,     0,     0,    62,    63,     0,    65,    66,
       0,     0,     0,     0,     0,     0,     0,     0,    12,    29,
      30,    37,    47,    59,    60,    64
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -90,   -90,   -90,    -1,   -90
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    64,   204,    66
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      65,    69,    71,    67,    72,    73,    74,    70,    75,    76,
      77,    78,   141,    79,   148,   149,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,   146,    95,    96,    97,    98,    99,   100,   125,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   125,   145,   210,   150,   117,   118,
     119,   120,   121,   151,   211,     0,    68,   142,   143,   144,
       0,     0,     0,   123,   124,     0,   125,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   147,   128,   129,   130,   125,
     136,   137,   192,   193,     0,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,     0,   136,   137,   205,   206,
     130,    69,     0,     0,   209,     0,     0,    70,     0,     0,
       0,     0,   131,   132,   133,   134,   135,     0,   136,   137,
     138,   139,   140,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    -1,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   136,   137,     0,     0,     0,     0,   250,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   251,   252,
       2,     3,     0,     4,   253,     0,     0,     0,     0,     5,
       0,     6,     0,     0,   254,     0,     7,     0,     8,     9,
       0,     0,   255,   256,     0,     0,   257,     0,    10,    11,
      12,    13,    14,     0,    15,    16,    17,    18,    19,    20,
      21,     0,     0,    22,    23,    24,    25,     0,     0,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,     0,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,   123,   124,     0,   125,    61,
       0,    62,     0,     0,    63,   126,   127,     0,     0,     0,
       0,     0,   123,   124,     0,   125,     0,     0,     0,     0,
       0,     0,   126,   127,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   128,   129,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   128,   129,     0,     0,     0,
       0,     0,   130,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   131,   132,   133,   134,   135,   130,
     136,   137,   138,   139,   140,     0,   207,   208,     0,     0,
       0,   131,   132,   133,   134,   135,     0,   136,   137,   138,
     139,   140,   123,   124,   219,   125,     0,     0,     0,     0,
       0,     0,   126,   127,     0,     0,     0,     0,     0,   123,
     124,     0,   125,     0,     0,     0,     0,     0,     0,   126,
     127,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   128,   129,     0,     0,   123,
     124,     0,   125,     0,     0,     0,     0,     0,     0,   126,
     127,     0,   128,   129,     0,     0,     0,     0,     0,   130,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   131,   132,   133,   134,   135,   130,   136,   137,   138,
     139,   140,   128,   129,   220,     0,     0,     0,   131,   132,
     133,   134,   135,     0,   136,   137,   138,   139,   140,   123,
     124,   225,   125,     0,     0,     0,   130,     0,     0,   126,
     127,     0,     0,     0,     0,     0,     0,     0,   131,   132,
     133,   134,   135,     0,   136,   137,   138,   139,   140,   123,
     124,   235,   125,     0,     0,     0,     0,     0,     0,   126,
     127,     0,   128,   129,     0,     0,   123,   124,     0,   125,
       0,     0,     0,     0,     0,     0,   126,   127,     0,     0,
       0,     0,     0,     0,     0,     0,   130,     0,     0,     0,
       0,     0,   128,   129,     0,     0,     0,     0,   131,   132,
     133,   134,   135,     0,   136,   137,   138,   139,   140,   128,
     129,   243,     0,     0,     0,     0,   130,     0,     0,     0,
       0,     0,     0,   125,     0,     0,     0,     0,   131,   132,
     133,   134,   135,   130,   136,   137,   138,   139,   140,     0,
       0,   244,     0,     0,     0,   131,   132,   133,   134,   135,
     125,   136,   137,   138,   139,   140,   123,   124,   247,   125,
       0,     0,     0,   128,   129,     0,   126,   127,     0,     0,
       0,     0,     0,   123,   124,     0,   125,     0,     0,     0,
       0,     0,     0,   126,   127,     0,     0,   130,     0,     0,
     128,   129,     0,     0,     0,     0,     0,     0,     0,   128,
     129,   133,   134,   135,     0,   136,   137,     0,     0,     0,
       0,     0,     0,     0,   130,     0,   128,   129,     0,     0,
       0,     0,     0,   130,     0,     0,   131,   132,   133,   134,
     135,     0,   136,   137,     0,   131,   132,   133,   134,   135,
     130,   136,   137,   138,   139,   140,     0,   191,     0,     0,
       0,     0,   131,   132,   133,   134,   135,     0,   136,   137,
     138,   139,   140,     0,   212,   123,   124,     0,   125,     0,
       0,     0,     0,     0,     0,   126,   127,     0,     0,     0,
       0,     0,   123,   124,     0,   125,     0,     0,     0,     0,
       0,     0,   126,   127,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   128,   129,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   128,   129,     0,     0,     0,
       0,     0,   130,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   131,   132,   133,   134,   135,   130,
     136,   137,   138,   139,   140,     0,   213,     0,     0,     0,
       0,   131,   132,   133,   134,   135,     0,   136,   137,   138,
     139,   140,     0,   214,   123,   124,     0,   125,     0,     0,
       0,     0,     0,     0,   126,   127,     0,     0,     0,     0,
       0,   123,   124,     0,   125,     0,     0,     0,     0,     0,
       0,   126,   127,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   128,   129,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   128,   129,     0,     0,     0,     0,
       0,   130,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   131,   132,   133,   134,   135,   130,   136,
     137,   138,   139,   140,     0,   215,     0,     0,     0,     0,
     131,   132,   133,   134,   135,     0,   136,   137,   138,   139,
     140,     0,   216,   123,   124,     0,   125,     0,     0,     0,
       0,     0,     0,   126,   127,     0,     0,     0,     0,     0,
     123,   124,     0,   125,     0,     0,     0,     0,     0,     0,
     126,   127,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   128,   129,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   128,   129,     0,     0,     0,     0,     0,
     130,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   131,   132,   133,   134,   135,   130,   136,   137,
     138,   139,   140,     0,   217,     0,     0,     0,     0,   131,
     132,   133,   134,   135,     0,   136,   137,   138,   139,   140,
       0,   218,   123,   124,     0,   125,     0,     0,     0,     0,
       0,     0,   126,   127,     0,     0,     0,     0,     0,   123,
     124,     0,   125,     0,     0,     0,     0,     0,     0,   126,
     127,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   128,   129,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   128,   129,     0,     0,     0,     0,     0,   130,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   131,   132,   133,   134,   135,   130,   136,   137,   138,
     139,   140,     0,   221,     0,     0,     0,     0,   131,   132,
     133,   134,   135,     0,   136,   137,   138,   139,   140,     0,
     222,   123,   124,     0,   125,     0,     0,     0,     0,     0,
       0,   126,   127,     0,     0,     0,     0,     0,   123,   124,
       0,   125,     0,     0,     0,     0,     0,     0,   126,   127,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   128,   129,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   128,   129,     0,     0,     0,     0,     0,   130,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     131,   132,   133,   134,   135,   130,   136,   137,   138,   139,
     140,     0,   223,     0,     0,     0,     0,   131,   132,   133,
     134,   135,     0,   136,   137,   138,   139,   140,     0,   224,
     123,   124,     0,   125,     0,     0,     0,     0,     0,     0,
     126,   127,     0,     0,     0,     0,     0,   123,   124,     0,
     125,     0,     0,     0,     0,     0,     0,   126,   127,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   128,   129,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     128,   129,     0,     0,     0,     0,     0,   130,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   131,
     132,   133,   134,   135,   130,   136,   137,   138,   139,   140,
       0,   226,     0,     0,     0,     0,   131,   132,   133,   134,
     135,     0,   136,   137,   138,   139,   140,     0,   227,   123,
     124,     0,   125,     0,     0,     0,     0,     0,     0,   126,
     127,     0,     0,     0,     0,     0,   123,   124,     0,   125,
       0,     0,     0,     0,     0,     0,   126,   127,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   128,   129,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   128,
     129,     0,     0,     0,     0,     0,   130,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   131,   132,
     133,   134,   135,   130,   136,   137,   138,   139,   140,     0,
     228,     0,     0,     0,     0,   131,   132,   133,   134,   135,
       0,   136,   137,   138,   139,   140,     0,   229,   123,   124,
       0,   125,     0,     0,     0,     0,     0,     0,   126,   127,
       0,     0,     0,     0,     0,   123,   124,     0,   125,     0,
       0,     0,     0,     0,     0,   126,   127,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   128,   129,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   128,   129,
       0,     0,     0,     0,     0,   130,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   131,   132,   133,
     134,   135,   130,   136,   137,   138,   139,   140,     0,   230,
       0,     0,     0,     0,   131,   132,   133,   134,   135,     0,
     136,   137,   138,   139,   140,     0,   231,   123,   124,     0,
     125,     0,     0,     0,     0,     0,     0,   126,   127,     0,
       0,     0,     0,     0,   123,   124,     0,   125,     0,     0,
       0,     0,     0,     0,   126,   127,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     128,   129,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   128,   129,     0,
       0,     0,     0,     0,   130,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   131,   132,   133,   134,
     135,   130,   136,   137,   138,   139,   140,     0,   232,     0,
       0,     0,     0,   131,   132,   133,   134,   135,     0,   136,
     137,   138,   139,   140,     0,   233,   123,   124,     0,   125,
       0,     0,     0,     0,     0,     0,   126,   127,     0,     0,
       0,     0,     0,   123,   124,     0,   125,     0,     0,     0,
       0,     0,     0,   126,   127,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   128,
     129,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   128,   129,     0,     0,
       0,     0,     0,   130,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   131,   132,   133,   134,   135,
     130,   136,   137,   138,   139,   140,     0,   234,     0,     0,
       0,     0,   131,   132,   133,   134,   135,     0,   136,   137,
     138,   139,   140,     0,   236,   123,   124,     0,   125,     0,
       0,     0,     0,     0,     0,   126,   127,     0,     0,     0,
       0,     0,   123,   124,     0,   125,     0,     0,     0,     0,
       0,     0,   126,   127,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   128,   129,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   128,   129,     0,     0,     0,
       0,     0,   130,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   131,   132,   133,   134,   135,   130,
     136,   137,   138,   139,   140,     0,   237,     0,     0,     0,
       0,   131,   132,   133,   134,   135,     0,   136,   137,   138,
     139,   140,     0,   238,   123,   124,     0,   125,     0,     0,
       0,     0,     0,     0,   126,   127,     0,     0,     0,     0,
       0,   123,   124,     0,   125,     0,     0,     0,     0,     0,
       0,   126,   127,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   128,   129,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   128,   129,     0,     0,     0,     0,
       0,   130,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   131,   132,   133,   134,   135,   130,   136,
     137,   138,   139,   140,     0,   239,     0,     0,     0,     0,
     131,   132,   133,   134,   135,     0,   136,   137,   138,   139,
     140,     0,   240,   123,   124,     0,   125,     0,     0,     0,
       0,     0,     0,   126,   127,     0,     0,     0,     0,     0,
     123,   124,     0,   125,     0,     0,     0,     0,     0,     0,
     126,   127,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   128,   129,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   128,   129,     0,     0,     0,     0,     0,
     130,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   131,   132,   133,   134,   135,   130,   136,   137,
     138,   139,   140,     0,   241,     0,     0,     0,     0,   131,
     132,   133,   134,   135,     0,   136,   137,   138,   139,   140,
       0,   242,   123,   124,     0,   125,     0,     0,     0,     0,
       0,     0,   126,   127,     0,     0,     0,     0,     0,   123,
     124,     0,   125,     0,     0,     0,     0,     0,     0,   126,
     127,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   128,   129,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   128,   129,     0,     0,     0,     0,     0,   130,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   131,   132,   133,   134,   135,   130,   136,   137,   138,
     139,   140,     0,   245,     0,     0,     0,     0,   131,   132,
     133,   134,   135,     0,   136,   137,   138,   139,   140,     0,
     246,   123,   124,     0,   125,     0,     0,     0,     0,     0,
       0,   126,   127,     0,     0,     0,     0,     0,   123,   124,
       0,   125,     0,     0,     0,     0,     0,     0,   126,   127,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   128,   129,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   128,   129,     0,     0,     0,     0,     0,   130,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     131,   132,   133,   134,   135,   130,   136,   137,   138,   139,
     140,     0,   248,     0,     0,     0,     0,   131,   132,   133,
     134,   135,     0,   136,   137,   138,   139,   140,     0,   249,
     123,   124,     0,   125,     0,     0,     0,     0,     0,     0,
     126,   127,     0,     0,     0,     0,     0,   123,   124,     0,
     125,     0,     0,     0,     0,     0,     0,   126,   127,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   128,   129,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     128,   129,     0,     0,     0,     0,     0,   130,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   131,
     132,   133,   134,   135,   130,   136,   137,   138,   139,   140,
       0,   258,     0,     0,     0,     0,   131,   132,   133,   134,
     135,     0,   136,   137,   138,   139,   140,     0,   259,   123,
     124,     0,   125,     0,     0,     0,     0,     0,     0,   126,
     127,     0,     0,     0,     0,     0,   123,   124,     0,   125,
       0,     0,     0,     0,     0,     0,   126,   127,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   128,   129,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   128,
     129,     0,     0,     0,     0,     0,   130,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   131,   132,
     133,   134,   135,   130,   136,   137,   138,   139,   140,     0,
     260,     0,     0,     0,     0,   131,   132,   133,   134,   135,
       0,   136,   137,   138,   139,   140,     0,   261,   123,   124,
       0,   125,     0,     0,     0,     0,     0,     0,   126,   127,
       0,     0,     0,     0,     0,   123,   124,     0,   125,     0,
       0,     0,     0,     0,     0,   126,   127,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   128,   129,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   128,   129,
       0,     0,     0,     0,     0,   130,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   131,   132,   133,
     134,   135,   130,   136,   137,   138,   139,   140,     0,   262,
       0,     0,     0,     0,   131,   132,   133,   134,   135,     0,
     136,   137,   138,   139,   140,     0,   263,   123,   124,     0,
     125,     0,     0,     0,     0,     0,     0,   126,   127,     0,
       0,     0,     0,     0,   123,   124,     0,   125,     0,     0,
       0,     0,     0,     0,   126,   127,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     128,   129,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   128,   129,     0,
       0,     0,     0,     0,   130,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   131,   132,   133,   134,
     135,   130,   136,   137,   138,   139,   140,     0,   264,     0,
       0,     0,     0,   131,   132,   133,   134,   135,     0,   136,
     137,   138,   139,   140,     0,   265,   122,   123,   124,     0,
     125,     0,     0,     0,     0,     0,     0,   126,   127,     0,
       0,     0,     0,     0,   123,   124,     0,   125,     0,     0,
       0,     0,     0,     0,   126,   127,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     128,   129,     0,     0,   123,   124,     0,   125,     0,     0,
       0,     0,     0,     0,   126,   127,     0,   128,   129,     0,
       0,     0,     0,     0,   130,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   131,   132,   133,   134,
     135,   130,   136,   137,   138,   139,   140,   128,   129,     0,
       0,     0,     0,   131,   132,   133,   134,   135,     0,   136,
     137,   190,   139,   140,     0,     0,     0,     0,     0,     0,
     116,   130,     6,     0,     0,     0,     0,     7,     0,     0,
       0,     0,     0,   131,   132,   133,   134,   135,     0,   136,
     137,   138,   139,   140,     0,    15,    16,    17,    18,    19,
      20,    21,     0,     0,    22,    23,    24,    25,     0,     0,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,     0,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,   116,     0,     6,     0,
      61,     0,    62,     7,     0,    63,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    15,    16,    17,    18,    19,    20,    21,     0,     0,
      22,    23,    24,    25,     0,     0,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,     0,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,     0,     0,     0,     0,    61,     0,     0,     0,
       0,    63
};

static const yytype_int16 yycheck[] =
{
       1,    88,     9,     3,     9,    94,    88,    94,    88,    88,
      94,    94,     3,    94,    11,    11,    94,    94,    94,    94,
      94,    94,    94,    94,    94,    94,    94,    94,    94,    94,
      94,    11,    94,    94,    94,    94,    94,    94,     7,    94,
      94,    94,    94,    94,    94,    94,    94,    94,    94,    94,
      94,    94,    94,    94,     7,    88,    95,     9,    59,    60,
      61,    62,    63,     8,    95,    -1,    13,    68,    69,    70,
      -1,    -1,    -1,     4,     5,    -1,     7,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,    95,    47,    48,    71,     7,
      89,    90,   123,   124,    -1,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,    -1,    89,    90,   139,   140,
      71,    88,    -1,    -1,   145,    -1,    -1,    94,    -1,    -1,
      -1,    -1,    83,    84,    85,    86,    87,    -1,    89,    90,
      91,    92,    93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    71,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    89,    90,    -1,    -1,    -1,    -1,   208,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   219,   220,
       0,     1,    -1,     3,   225,    -1,    -1,    -1,    -1,     9,
      -1,    11,    -1,    -1,   235,    -1,    16,    -1,    18,    19,
      -1,    -1,   243,   244,    -1,    -1,   247,    -1,    28,    29,
      30,    31,    32,    -1,    34,    35,    36,    37,    38,    39,
      40,    -1,    -1,    43,    44,    45,    46,    -1,    -1,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    -1,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,     4,     5,    -1,     7,    89,
      -1,    91,    -1,    -1,    94,    14,    15,    -1,    -1,    -1,
      -1,    -1,     4,     5,    -1,     7,    -1,    -1,    -1,    -1,
      -1,    -1,    14,    15,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,    -1,
      -1,    -1,    71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    83,    84,    85,    86,    87,    71,
      89,    90,    91,    92,    93,    -1,    95,    96,    -1,    -1,
      -1,    83,    84,    85,    86,    87,    -1,    89,    90,    91,
      92,    93,     4,     5,    96,     7,    -1,    -1,    -1,    -1,
      -1,    -1,    14,    15,    -1,    -1,    -1,    -1,    -1,     4,
       5,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,    14,
      15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,     4,
       5,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,    14,
      15,    -1,    47,    48,    -1,    -1,    -1,    -1,    -1,    71,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    83,    84,    85,    86,    87,    71,    89,    90,    91,
      92,    93,    47,    48,    96,    -1,    -1,    -1,    83,    84,
      85,    86,    87,    -1,    89,    90,    91,    92,    93,     4,
       5,    96,     7,    -1,    -1,    -1,    71,    -1,    -1,    14,
      15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    84,
      85,    86,    87,    -1,    89,    90,    91,    92,    93,     4,
       5,    96,     7,    -1,    -1,    -1,    -1,    -1,    -1,    14,
      15,    -1,    47,    48,    -1,    -1,     4,     5,    -1,     7,
      -1,    -1,    -1,    -1,    -1,    -1,    14,    15,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    71,    -1,    -1,    -1,
      -1,    -1,    47,    48,    -1,    -1,    -1,    -1,    83,    84,
      85,    86,    87,    -1,    89,    90,    91,    92,    93,    47,
      48,    96,    -1,    -1,    -1,    -1,    71,    -1,    -1,    -1,
      -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,    83,    84,
      85,    86,    87,    71,    89,    90,    91,    92,    93,    -1,
      -1,    96,    -1,    -1,    -1,    83,    84,    85,    86,    87,
       7,    89,    90,    91,    92,    93,     4,     5,    96,     7,
      -1,    -1,    -1,    47,    48,    -1,    14,    15,    -1,    -1,
      -1,    -1,    -1,     4,     5,    -1,     7,    -1,    -1,    -1,
      -1,    -1,    -1,    14,    15,    -1,    -1,    71,    -1,    -1,
      47,    48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,
      48,    85,    86,    87,    -1,    89,    90,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    71,    -1,    47,    48,    -1,    -1,
      -1,    -1,    -1,    71,    -1,    -1,    83,    84,    85,    86,
      87,    -1,    89,    90,    -1,    83,    84,    85,    86,    87,
      71,    89,    90,    91,    92,    93,    -1,    95,    -1,    -1,
      -1,    -1,    83,    84,    85,    86,    87,    -1,    89,    90,
      91,    92,    93,    -1,    95,     4,     5,    -1,     7,    -1,
      -1,    -1,    -1,    -1,    -1,    14,    15,    -1,    -1,    -1,
      -1,    -1,     4,     5,    -1,     7,    -1,    -1,    -1,    -1,
      -1,    -1,    14,    15,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,    -1,
      -1,    -1,    71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    83,    84,    85,    86,    87,    71,
      89,    90,    91,    92,    93,    -1,    95,    -1,    -1,    -1,
      -1,    83,    84,    85,    86,    87,    -1,    89,    90,    91,
      92,    93,    -1,    95,     4,     5,    -1,     7,    -1,    -1,
      -1,    -1,    -1,    -1,    14,    15,    -1,    -1,    -1,    -1,
      -1,     4,     5,    -1,     7,    -1,    -1,    -1,    -1,    -1,
      -1,    14,    15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    47,    48,    -1,    -1,    -1,    -1,
      -1,    71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    83,    84,    85,    86,    87,    71,    89,
      90,    91,    92,    93,    -1,    95,    -1,    -1,    -1,    -1,
      83,    84,    85,    86,    87,    -1,    89,    90,    91,    92,
      93,    -1,    95,     4,     5,    -1,     7,    -1,    -1,    -1,
      -1,    -1,    -1,    14,    15,    -1,    -1,    -1,    -1,    -1,
       4,     5,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,
      14,    15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    47,    48,    -1,    -1,    -1,    -1,    -1,
      71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    83,    84,    85,    86,    87,    71,    89,    90,
      91,    92,    93,    -1,    95,    -1,    -1,    -1,    -1,    83,
      84,    85,    86,    87,    -1,    89,    90,    91,    92,    93,
      -1,    95,     4,     5,    -1,     7,    -1,    -1,    -1,    -1,
      -1,    -1,    14,    15,    -1,    -1,    -1,    -1,    -1,     4,
       5,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,    14,
      15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    47,    48,    -1,    -1,    -1,    -1,    -1,    71,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    83,    84,    85,    86,    87,    71,    89,    90,    91,
      92,    93,    -1,    95,    -1,    -1,    -1,    -1,    83,    84,
      85,    86,    87,    -1,    89,    90,    91,    92,    93,    -1,
      95,     4,     5,    -1,     7,    -1,    -1,    -1,    -1,    -1,
      -1,    14,    15,    -1,    -1,    -1,    -1,    -1,     4,     5,
      -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,    14,    15,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    47,    48,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    47,    48,    -1,    -1,    -1,    -1,    -1,    71,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      83,    84,    85,    86,    87,    71,    89,    90,    91,    92,
      93,    -1,    95,    -1,    -1,    -1,    -1,    83,    84,    85,
      86,    87,    -1,    89,    90,    91,    92,    93,    -1,    95,
       4,     5,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,
      14,    15,    -1,    -1,    -1,    -1,    -1,     4,     5,    -1,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    14,    15,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    47,    48,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      47,    48,    -1,    -1,    -1,    -1,    -1,    71,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,
      84,    85,    86,    87,    71,    89,    90,    91,    92,    93,
      -1,    95,    -1,    -1,    -1,    -1,    83,    84,    85,    86,
      87,    -1,    89,    90,    91,    92,    93,    -1,    95,     4,
       5,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,    14,
      15,    -1,    -1,    -1,    -1,    -1,     4,     5,    -1,     7,
      -1,    -1,    -1,    -1,    -1,    -1,    14,    15,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    47,    48,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,
      48,    -1,    -1,    -1,    -1,    -1,    71,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    84,
      85,    86,    87,    71,    89,    90,    91,    92,    93,    -1,
      95,    -1,    -1,    -1,    -1,    83,    84,    85,    86,    87,
      -1,    89,    90,    91,    92,    93,    -1,    95,     4,     5,
      -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,    14,    15,
      -1,    -1,    -1,    -1,    -1,     4,     5,    -1,     7,    -1,
      -1,    -1,    -1,    -1,    -1,    14,    15,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    47,    48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,
      -1,    -1,    -1,    -1,    -1,    71,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    84,    85,
      86,    87,    71,    89,    90,    91,    92,    93,    -1,    95,
      -1,    -1,    -1,    -1,    83,    84,    85,    86,    87,    -1,
      89,    90,    91,    92,    93,    -1,    95,     4,     5,    -1,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    14,    15,    -1,
      -1,    -1,    -1,    -1,     4,     5,    -1,     7,    -1,    -1,
      -1,    -1,    -1,    -1,    14,    15,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      47,    48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,
      -1,    -1,    -1,    -1,    71,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    83,    84,    85,    86,
      87,    71,    89,    90,    91,    92,    93,    -1,    95,    -1,
      -1,    -1,    -1,    83,    84,    85,    86,    87,    -1,    89,
      90,    91,    92,    93,    -1,    95,     4,     5,    -1,     7,
      -1,    -1,    -1,    -1,    -1,    -1,    14,    15,    -1,    -1,
      -1,    -1,    -1,     4,     5,    -1,     7,    -1,    -1,    -1,
      -1,    -1,    -1,    14,    15,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,
      48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,
      -1,    -1,    -1,    71,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    83,    84,    85,    86,    87,
      71,    89,    90,    91,    92,    93,    -1,    95,    -1,    -1,
      -1,    -1,    83,    84,    85,    86,    87,    -1,    89,    90,
      91,    92,    93,    -1,    95,     4,     5,    -1,     7,    -1,
      -1,    -1,    -1,    -1,    -1,    14,    15,    -1,    -1,    -1,
      -1,    -1,     4,     5,    -1,     7,    -1,    -1,    -1,    -1,
      -1,    -1,    14,    15,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,    -1,
      -1,    -1,    71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    83,    84,    85,    86,    87,    71,
      89,    90,    91,    92,    93,    -1,    95,    -1,    -1,    -1,
      -1,    83,    84,    85,    86,    87,    -1,    89,    90,    91,
      92,    93,    -1,    95,     4,     5,    -1,     7,    -1,    -1,
      -1,    -1,    -1,    -1,    14,    15,    -1,    -1,    -1,    -1,
      -1,     4,     5,    -1,     7,    -1,    -1,    -1,    -1,    -1,
      -1,    14,    15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    47,    48,    -1,    -1,    -1,    -1,
      -1,    71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    83,    84,    85,    86,    87,    71,    89,
      90,    91,    92,    93,    -1,    95,    -1,    -1,    -1,    -1,
      83,    84,    85,    86,    87,    -1,    89,    90,    91,    92,
      93,    -1,    95,     4,     5,    -1,     7,    -1,    -1,    -1,
      -1,    -1,    -1,    14,    15,    -1,    -1,    -1,    -1,    -1,
       4,     5,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,
      14,    15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    47,    48,    -1,    -1,    -1,    -1,    -1,
      71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    83,    84,    85,    86,    87,    71,    89,    90,
      91,    92,    93,    -1,    95,    -1,    -1,    -1,    -1,    83,
      84,    85,    86,    87,    -1,    89,    90,    91,    92,    93,
      -1,    95,     4,     5,    -1,     7,    -1,    -1,    -1,    -1,
      -1,    -1,    14,    15,    -1,    -1,    -1,    -1,    -1,     4,
       5,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,    14,
      15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    47,    48,    -1,    -1,    -1,    -1,    -1,    71,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    83,    84,    85,    86,    87,    71,    89,    90,    91,
      92,    93,    -1,    95,    -1,    -1,    -1,    -1,    83,    84,
      85,    86,    87,    -1,    89,    90,    91,    92,    93,    -1,
      95,     4,     5,    -1,     7,    -1,    -1,    -1,    -1,    -1,
      -1,    14,    15,    -1,    -1,    -1,    -1,    -1,     4,     5,
      -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,    14,    15,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    47,    48,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    47,    48,    -1,    -1,    -1,    -1,    -1,    71,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      83,    84,    85,    86,    87,    71,    89,    90,    91,    92,
      93,    -1,    95,    -1,    -1,    -1,    -1,    83,    84,    85,
      86,    87,    -1,    89,    90,    91,    92,    93,    -1,    95,
       4,     5,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,
      14,    15,    -1,    -1,    -1,    -1,    -1,     4,     5,    -1,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    14,    15,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    47,    48,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      47,    48,    -1,    -1,    -1,    -1,    -1,    71,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,
      84,    85,    86,    87,    71,    89,    90,    91,    92,    93,
      -1,    95,    -1,    -1,    -1,    -1,    83,    84,    85,    86,
      87,    -1,    89,    90,    91,    92,    93,    -1,    95,     4,
       5,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,    14,
      15,    -1,    -1,    -1,    -1,    -1,     4,     5,    -1,     7,
      -1,    -1,    -1,    -1,    -1,    -1,    14,    15,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    47,    48,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,
      48,    -1,    -1,    -1,    -1,    -1,    71,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    84,
      85,    86,    87,    71,    89,    90,    91,    92,    93,    -1,
      95,    -1,    -1,    -1,    -1,    83,    84,    85,    86,    87,
      -1,    89,    90,    91,    92,    93,    -1,    95,     4,     5,
      -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,    14,    15,
      -1,    -1,    -1,    -1,    -1,     4,     5,    -1,     7,    -1,
      -1,    -1,    -1,    -1,    -1,    14,    15,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    47,    48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,
      -1,    -1,    -1,    -1,    -1,    71,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    84,    85,
      86,    87,    71,    89,    90,    91,    92,    93,    -1,    95,
      -1,    -1,    -1,    -1,    83,    84,    85,    86,    87,    -1,
      89,    90,    91,    92,    93,    -1,    95,     4,     5,    -1,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    14,    15,    -1,
      -1,    -1,    -1,    -1,     4,     5,    -1,     7,    -1,    -1,
      -1,    -1,    -1,    -1,    14,    15,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      47,    48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,
      -1,    -1,    -1,    -1,    71,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    83,    84,    85,    86,
      87,    71,    89,    90,    91,    92,    93,    -1,    95,    -1,
      -1,    -1,    -1,    83,    84,    85,    86,    87,    -1,    89,
      90,    91,    92,    93,    -1,    95,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    14,    15,    -1,
      -1,    -1,    -1,    -1,     4,     5,    -1,     7,    -1,    -1,
      -1,    -1,    -1,    -1,    14,    15,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      47,    48,    -1,    -1,     4,     5,    -1,     7,    -1,    -1,
      -1,    -1,    -1,    -1,    14,    15,    -1,    47,    48,    -1,
      -1,    -1,    -1,    -1,    71,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    83,    84,    85,    86,
      87,    71,    89,    90,    91,    92,    93,    47,    48,    -1,
      -1,    -1,    -1,    83,    84,    85,    86,    87,    -1,    89,
      90,    91,    92,    93,    -1,    -1,    -1,    -1,    -1,    -1,
       9,    71,    11,    -1,    -1,    -1,    -1,    16,    -1,    -1,
      -1,    -1,    -1,    83,    84,    85,    86,    87,    -1,    89,
      90,    91,    92,    93,    -1,    34,    35,    36,    37,    38,
      39,    40,    -1,    -1,    43,    44,    45,    46,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    -1,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,     9,    -1,    11,    -1,
      89,    -1,    91,    16,    -1,    94,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    34,    35,    36,    37,    38,    39,    40,    -1,    -1,
      43,    44,    45,    46,    -1,    -1,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    -1,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    -1,    -1,    -1,    -1,    89,    -1,    -1,    -1,
      -1,    94
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    98,     0,     1,     3,     9,    11,    16,    18,    19,
      28,    29,    30,    31,    32,    34,    35,    36,    37,    38,
      39,    40,    43,    44,    45,    46,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    89,    91,    94,    99,   100,   101,     3,    13,    88,
      94,     9,     9,    94,    88,    88,    88,    94,    94,    94,
      94,    94,    94,    94,    94,    94,    94,    94,    94,    94,
      94,    94,    94,    94,    94,    94,    94,    94,    94,    94,
      94,    94,    94,    94,    94,    94,    94,    94,    94,    94,
      94,    94,    94,    94,    94,    94,     9,   100,   100,   100,
     100,   100,     3,     4,     5,     7,    14,    15,    47,    48,
      71,    83,    84,    85,    86,    87,    89,    90,    91,    92,
      93,     3,   100,   100,   100,    88,    11,    95,    11,    11,
       9,     8,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
      91,    95,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,    95,    96,   100,
      95,    95,    95,    95,    95,    95,    95,    95,    95,    96,
      96,    95,    95,    95,    95,    96,    95,    95,    95,    95,
      95,    95,    95,    95,    95,    96,    95,    95,    95,    95,
      95,    95,    95,    96,    96,    95,    95,    96,    95,    95,
     100,   100,   100,   100,   100,   100,   100,   100,    95,    95,
      95,    95,    95,    95,    95,    95
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    97,    98,    98,    99,    99,    99,    99,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   101,   101,   101,   101,   101,   101,   101,   101,   101,
     101
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     2,     2,     2,     1,     1,
       1,     4,     6,     3,     3,     3,     3,     3,     2,     2,
       3,     4,     3,     4,     4,     4,     4,     4,     4,     6,
       6,     4,     4,     3,     3,     4,     4,     6,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     6,     4,     4,
       4,     4,     4,     2,     2,     2,     2,     4,     4,     6,
       6,     3,     4,     4,     6,     4,     4,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     3,     2,
       3,     1,     3,     3,     3,     4,     1,     3,     4,     4,
       3
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return (YYSIZE_T) (yystpcpy (yyres, yystr) - yyres);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yynewstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  *yyssp = (yytype_int16) yystate;

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = (YYSIZE_T) (yyssp - yyss + 1);

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2:
#line 171 "src/parse.y"
    { (yyval.i_value) = 0; }
#line 2214 "src/parse.c"
    break;

  case 5:
#line 177 "src/parse.y"
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
#line 2229 "src/parse.c"
    break;

  case 7:
#line 188 "src/parse.y"
    { yyerrok; }
#line 2235 "src/parse.c"
    break;

  case 8:
#line 191 "src/parse.y"
    { INIT (t); realis_read (t, (yyvsp[0].s_value)); FINI (t); free ((yyvsp[0].s_value)); }
#line 2241 "src/parse.c"
    break;

  case 9:
#line 192 "src/parse.y"
    { INIT (t); realis_set_inf (t, MPFR_SIGN_POS); FINI (t); }
#line 2247 "src/parse.c"
    break;

  case 10:
#line 194 "src/parse.y"
    {
  INIT (t);
  variable_t *v = get_variable ((yyvsp[0].s_value));
  if (v == NULL)
    cerrorf (unknown_variable, "%s", quote ((yyvsp[0].s_value)));
  else
    realis_copy (t, v->val);
  free ((yyvsp[0].s_value));
  FINI (t);
}
#line 2262 "src/parse.c"
    break;

  case 11:
#line 204 "src/parse.y"
    { INIT (t); POP1 (t1); cerrorf (unknown_function, "%s", quote ((yyvsp[-3].s_value))); CLEAR1 (t1); FINI (t); }
#line 2268 "src/parse.c"
    break;

  case 12:
#line 205 "src/parse.y"
    { INIT (t); POP2 (t1, t2); cerrorf (unknown_function, "%s", quote ((yyvsp[-5].s_value))); CLEAR2 (t1, t2); FINI (t); }
#line 2274 "src/parse.c"
    break;

  case 13:
#line 206 "src/parse.y"
    { BINARY_FUNCTION (add); }
#line 2280 "src/parse.c"
    break;

  case 14:
#line 207 "src/parse.y"
    { BINARY_FUNCTION (sub); }
#line 2286 "src/parse.c"
    break;

  case 15:
#line 208 "src/parse.y"
    { BINARY_FUNCTION (mul); }
#line 2292 "src/parse.c"
    break;

  case 16:
#line 209 "src/parse.y"
    { BINARY_FUNCTION (div); }
#line 2298 "src/parse.c"
    break;

  case 17:
#line 210 "src/parse.y"
    { BINARY_FUNCTION (rem); }
#line 2304 "src/parse.c"
    break;

  case 18:
#line 211 "src/parse.y"
    { (yyval.i_value) = 0; }
#line 2310 "src/parse.c"
    break;

  case 19:
#line 212 "src/parse.y"
    { UNARY_FUNCTION (neg); }
#line 2316 "src/parse.c"
    break;

  case 20:
#line 213 "src/parse.y"
    { (yyval.i_value) = 0; }
#line 2322 "src/parse.c"
    break;

  case 21:
#line 214 "src/parse.y"
    { UNARY_FUNCTION (abs); }
#line 2328 "src/parse.c"
    break;

  case 22:
#line 215 "src/parse.y"
    { UNARY_FUNCTION (abs); }
#line 2334 "src/parse.c"
    break;

  case 23:
#line 216 "src/parse.y"
    { UNARY_FUNCTION (sign); }
#line 2340 "src/parse.c"
    break;

  case 24:
#line 217 "src/parse.y"
    { UNARY_FUNCTION (round); }
#line 2346 "src/parse.c"
    break;

  case 25:
#line 218 "src/parse.y"
    { UNARY_FUNCTION (ceil);  }
#line 2352 "src/parse.c"
    break;

  case 26:
#line 219 "src/parse.y"
    { UNARY_FUNCTION (floor); }
#line 2358 "src/parse.c"
    break;

  case 27:
#line 220 "src/parse.y"
    { UNARY_FUNCTION (trunc); }
#line 2364 "src/parse.c"
    break;

  case 28:
#line 221 "src/parse.y"
    { UNARY_FUNCTION (frac);  }
#line 2370 "src/parse.c"
    break;

  case 29:
#line 222 "src/parse.y"
    { BINARY_FUNCTION (pow); }
#line 2376 "src/parse.c"
    break;

  case 30:
#line 223 "src/parse.y"
    { BINARY_FUNCTION (root); }
#line 2382 "src/parse.c"
    break;

  case 31:
#line 224 "src/parse.y"
    { UNARY_FUNCTION (sqrt); }
#line 2388 "src/parse.c"
    break;

  case 32:
#line 225 "src/parse.y"
    { UNARY_FUNCTION (cbrt); }
#line 2394 "src/parse.c"
    break;

  case 33:
#line 226 "src/parse.y"
    { BINARY_FUNCTION (lsh); }
#line 2400 "src/parse.c"
    break;

  case 34:
#line 227 "src/parse.y"
    { BINARY_FUNCTION (rsh); }
#line 2406 "src/parse.c"
    break;

  case 35:
#line 228 "src/parse.y"
    { UNARY_FUNCTION (exp); }
#line 2412 "src/parse.c"
    break;

  case 36:
#line 229 "src/parse.y"
    { UNARY_FUNCTION (ln); }
#line 2418 "src/parse.c"
    break;

  case 37:
#line 230 "src/parse.y"
    { BINARY_FUNCTION (agm); }
#line 2424 "src/parse.c"
    break;

  case 38:
#line 231 "src/parse.y"
    { UNARY_FUNCTION (sin); }
#line 2430 "src/parse.c"
    break;

  case 39:
#line 232 "src/parse.y"
    { UNARY_FUNCTION (cos); }
#line 2436 "src/parse.c"
    break;

  case 40:
#line 233 "src/parse.y"
    { UNARY_FUNCTION (tan); }
#line 2442 "src/parse.c"
    break;

  case 41:
#line 234 "src/parse.y"
    { UNARY_FUNCTION (cot); }
#line 2448 "src/parse.c"
    break;

  case 42:
#line 235 "src/parse.y"
    { UNARY_FUNCTION (sec); }
#line 2454 "src/parse.c"
    break;

  case 43:
#line 236 "src/parse.y"
    { UNARY_FUNCTION (csc); }
#line 2460 "src/parse.c"
    break;

  case 44:
#line 237 "src/parse.y"
    { UNARY_FUNCTION (asin); }
#line 2466 "src/parse.c"
    break;

  case 45:
#line 238 "src/parse.y"
    { UNARY_FUNCTION (acos); }
#line 2472 "src/parse.c"
    break;

  case 46:
#line 239 "src/parse.y"
    { UNARY_FUNCTION (atan); }
#line 2478 "src/parse.c"
    break;

  case 47:
#line 240 "src/parse.y"
    { BINARY_FUNCTION (atan2); }
#line 2484 "src/parse.c"
    break;

  case 48:
#line 241 "src/parse.y"
    { UNARY_FUNCTION (acot); }
#line 2490 "src/parse.c"
    break;

  case 49:
#line 242 "src/parse.y"
    { UNARY_FUNCTION (sinh); }
#line 2496 "src/parse.c"
    break;

  case 50:
#line 243 "src/parse.y"
    { UNARY_FUNCTION (cosh); }
#line 2502 "src/parse.c"
    break;

  case 51:
#line 244 "src/parse.y"
    { UNARY_FUNCTION (tanh); }
#line 2508 "src/parse.c"
    break;

  case 52:
#line 245 "src/parse.y"
    { UNARY_FUNCTION (coth); }
#line 2514 "src/parse.c"
    break;

  case 53:
#line 246 "src/parse.y"
    { UNARY_FUNCTION (fact); }
#line 2520 "src/parse.c"
    break;

  case 54:
#line 247 "src/parse.y"
    { UNARY_FUNCTION (ffact); }
#line 2526 "src/parse.c"
    break;

  case 55:
#line 248 "src/parse.y"
    { UNARY_FUNCTION (subfact); }
#line 2532 "src/parse.c"
    break;

  case 56:
#line 249 "src/parse.y"
    { UNARY_FUNCTION (primorial); }
#line 2538 "src/parse.c"
    break;

  case 57:
#line 250 "src/parse.y"
    { UNARY_FUNCTION (superfactorial); }
#line 2544 "src/parse.c"
    break;

  case 58:
#line 251 "src/parse.y"
    { UNARY_FUNCTION (hyperfactorial); }
#line 2550 "src/parse.c"
    break;

  case 59:
#line 252 "src/parse.y"
    { BINARY_FUNCTION (gcd); }
#line 2556 "src/parse.c"
    break;

  case 60:
#line 253 "src/parse.y"
    { BINARY_FUNCTION (lcm); }
#line 2562 "src/parse.c"
    break;

  case 61:
#line 254 "src/parse.y"
    { BINARY_FUNCTION (mod); }
#line 2568 "src/parse.c"
    break;

  case 62:
#line 255 "src/parse.y"
    { UNARY_FUNCTION (fibonacci); }
#line 2574 "src/parse.c"
    break;

  case 63:
#line 256 "src/parse.y"
    { UNARY_FUNCTION (tribonacci); }
#line 2580 "src/parse.c"
    break;

  case 64:
#line 257 "src/parse.y"
    { BINARY_FUNCTION (beta); }
#line 2586 "src/parse.c"
    break;

  case 65:
#line 258 "src/parse.y"
    { UNARY_FUNCTION (gamma); }
#line 2592 "src/parse.c"
    break;

  case 66:
#line 259 "src/parse.y"
    { UNARY_FUNCTION (zeta); }
#line 2598 "src/parse.c"
    break;

  case 67:
#line 260 "src/parse.y"
    { CONST_FUNCTION (pi); }
#line 2604 "src/parse.c"
    break;

  case 68:
#line 261 "src/parse.y"
    { CONST_FUNCTION (tau); }
#line 2610 "src/parse.c"
    break;

  case 69:
#line 262 "src/parse.y"
    { CONST_FUNCTION (e); }
#line 2616 "src/parse.c"
    break;

  case 70:
#line 263 "src/parse.y"
    { CONST_FUNCTION (y); }
#line 2622 "src/parse.c"
    break;

  case 71:
#line 264 "src/parse.y"
    { CONST_FUNCTION (catalan); }
#line 2628 "src/parse.c"
    break;

  case 72:
#line 266 "src/parse.y"
    {
  INIT (t);
  POP2 (t1, t2);
  int diff = realis_cmp (t1, t2);

  if ((yyvsp[-1].s_value)[0] == '=') /* "==" */
    realis_set_ui (t, diff == 0 ? 1 : 0);
  else
    {
      if ((yyvsp[-1].s_value)[0] == '!') /* "!=" */
        realis_set_ui (t, diff != 0 ? 1 : 0);
      else if ((yyvsp[-1].s_value)[0] == '<')
        {
          if ((yyvsp[-1].s_value)[1] == '=')
            realis_set_ui (t, diff <= 0 ? 1 : 0);
          else /* $2[1] == '\0' */
            realis_set_ui (t, diff < 0 ? 1 : 0);
        }
      else /* $2[0] == '>' */
        {
          if ((yyvsp[-1].s_value)[1] == '=')
            realis_set_ui (t, diff >= 0 ? 1 : 0);
          else /* $2[1] == '\0' */
            realis_set_ui (t, diff > 0 ? 1 : 0);
        }
    }

  CLEAR2 (t1, t2);
  FINI (t);
  free ((yyvsp[-1].s_value));
}
#line 2664 "src/parse.c"
    break;

  case 73:
#line 298 "src/parse.y"
    {
  INIT (t);
  POP2 (t1, t2);
  realis_init_set_si (t, realis_cmp (t1, t2));
  CLEAR2 (t1, t2);
  FINI (t);
}
#line 2676 "src/parse.c"
    break;

  case 74:
#line 305 "src/parse.y"
    { INIT (t); POP2 (t1, t2); realis_or (t, t1, t2); CLEAR2 (t1, t2); FINI (t); }
#line 2682 "src/parse.c"
    break;

  case 75:
#line 306 "src/parse.y"
    { INIT (t); POP2 (t1, t2); realis_and (t, t1, t2); CLEAR2 (t1, t2); FINI (t); }
#line 2688 "src/parse.c"
    break;

  case 76:
#line 307 "src/parse.y"
    { INIT (t); POP2 (t1, t2); realis_xor (t, t1, t2); CLEAR2 (t1, t2); FINI (t); }
#line 2694 "src/parse.c"
    break;

  case 77:
#line 308 "src/parse.y"
    { INIT (t); POP2 (t1, t2); realis_init_set_ui (t, MPFR_NOT_ZERO (t1) || MPFR_NOT_ZERO (t2)); CLEAR2 (t1, t2); FINI (t); }
#line 2700 "src/parse.c"
    break;

  case 78:
#line 309 "src/parse.y"
    { INIT (t); POP2 (t1, t2); realis_init_set_ui (t, MPFR_NOT_ZERO (t1) && MPFR_NOT_ZERO (t2)); CLEAR2 (t1, t2); FINI (t); }
#line 2706 "src/parse.c"
    break;

  case 79:
#line 311 "src/parse.y"
    {
  INIT (t);
  variable_t *v = get_variable ((yyvsp[0].s_value));
  if (v == NULL)
    cerrorf (unknown_variable, "%s", quote ((yyvsp[0].s_value)));
  mpfr_ptr x = v->val;
  if (!mpfr_integer_p (x))
    cerror (function_does_not_support_fractional);
  else
    {
      if ((yyvsp[-1].c_value) == '+')
        realis_add_ui (t, x, 1);
      else
        realis_sub_ui (t, x, 1);
    }
  realis_copy (x, t);
  FINI (t);

  free ((yyvsp[0].s_value));
}
#line 2731 "src/parse.c"
    break;

  case 80:
#line 332 "src/parse.y"
    {
  INIT (t);
  POP1 (t1);
  variable_t *v = get_variable ((yyvsp[-2].s_value));
  if (v == NULL)
    cerrorf (unknown_variable, "%s", quote ((yyvsp[-2].s_value)));
  else
    {
      realis_copy (v->val, t1);
      realis_copy (t, t1);
    }
  free ((yyvsp[-2].s_value));
  CLEAR1 (t1);
  FINI (t);
}
#line 2751 "src/parse.c"
    break;

  case 81:
#line 349 "src/parse.y"
    { warranty (); }
#line 2757 "src/parse.c"
    break;

  case 82:
#line 350 "src/parse.y"
    { mpfr_set_default_prec (digits2bits (strtoul ((yyvsp[0].s_value), NULL, base) + 1)); free ((yyvsp[0].s_value)); }
#line 2763 "src/parse.c"
    break;

  case 83:
#line 351 "src/parse.y"
    { base = strtoul ((yyvsp[0].s_value), NULL, base); free ((yyvsp[0].s_value)); }
#line 2769 "src/parse.c"
    break;

  case 84:
#line 352 "src/parse.y"
    { mpfr_set_default_rounding_mode (string2rounding ((yyvsp[0].s_value))); free ((yyvsp[0].s_value)); }
#line 2775 "src/parse.c"
    break;

  case 85:
#line 353 "src/parse.y"
    { size_t len = strlen ((yyvsp[-1].s_value)); (yyvsp[-1].s_value)[len - 1] = '\n'; write (fileno (stdout), (yyvsp[-1].s_value) + 1, len - 1); free ((yyvsp[-1].s_value)); }
#line 2781 "src/parse.c"
    break;

  case 86:
#line 354 "src/parse.y"
    { exit (0); }
#line 2787 "src/parse.c"
    break;

  case 87:
#line 355 "src/parse.y"
    { exit (0); }
#line 2793 "src/parse.c"
    break;

  case 88:
#line 356 "src/parse.y"
    { int exit_code = strtol ((yyvsp[-1].s_value), NULL, base); free ((yyvsp[-1].s_value)); exit (exit_code); }
#line 2799 "src/parse.c"
    break;

  case 89:
#line 358 "src/parse.y"
    {
  POP1 (t1);
  variable_t *v = get_variable ((yyvsp[-2].s_value));
  if (v != NULL)
    cerrorf (redeclaration_of_variable, "%s", quote ((yyvsp[-2].s_value)));
  else
    {
      variable_t *var = xmalloc (sizeof (*var));
      var->name = xstrdup ((yyvsp[-2].s_value));
      realis_init (var->val);
      realis_copy (var->val, t1);
      stack_push (variables, var);
    }
  free ((yyvsp[-2].s_value));
  CLEAR1 (t1);
}
#line 2820 "src/parse.c"
    break;

  case 90:
#line 375 "src/parse.y"
    {
  POP1 (t1);
  variable_t *v = get_variable ((yyvsp[-2].s_value));
  if (v == NULL)
    cerrorf (unknown_variable, "%s", quote ((yyvsp[-2].s_value)));
  else
    {
      mpfr_ptr t2 = v->val;
      switch ((yyvsp[-1].c_value))
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
  free ((yyvsp[-2].s_value));
  CLEAR1 (t1);
}
#line 2861 "src/parse.c"
    break;


#line 2865 "src/parse.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 412 "src/parse.y"

