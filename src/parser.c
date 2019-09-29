
#include "pc.h"

#include "complex.h"
#include "real.h"

#include "stack.h"

#include "system.h"

typedef struct
{
  char *name;
  mpc_t oldx, newx;
  bool modif;
} variable_t;
typedef struct
{
  char *name;
  char *body;
  struct stack *args;
} function_t;
typedef struct
{
  const char *name;
  int flags;
  void *rfunc;
  void *cfunc;
} op_t;
typedef struct
{
  const op_t *op;
  const char *input_pointer;
} op_ip_t;
typedef struct
{
  const char *name;
  u32 name_len;
  u32 index;
} label_t;

mpc_t ans, old_ans, ret_val;
tid_t thread;
const char *error_position;
unsigned int goto_pos;
unsigned int command_number;
unsigned int in_parenthesis;
stack_t number_stack;
stack_t op_stack;
stack_t variable_stack;
stack_t label_stack;
stack_t goto_positions;
stack_t function_stack;
const op_t **function_tab_sorted;

enum Type
{
  VARIABLE,
  CONSTANT,
  LEFT,    /* Parenthesis, commands.  */
  RIGHT,   /* Parenthesis, comma, semicolon, etc.  */
  KEYWORD, /* return, goto, etc.  */
  PREFIX,
  POSTFIX,
  UNARY,
  BINARY,
  TERNARY,

  VA_ARGS,
  VA_ARGS_MAX = VA_ARGS + 42,
};

#define set_op(s, t, n) {s, t, (void *) real_##n, (void *) complex_##n}

const op_t function_tab[] = {
  /* Prefix operators.  */
  set_op ("+", PREFIX, add),
  set_op ("-", PREFIX, sub),
  set_op ("~", PREFIX, not),

  /* Binary operators.  */
  set_op ("+", BINARY, add),
  set_op ("-", BINARY, sub),
  set_op ("*", BINARY, mul),
  set_op ("/", BINARY, div),
  set_op ("%", BINARY, rem),

  set_op ("==", BINARY, equal),
  set_op ("!=", BINARY, not_equal),
  set_op ("<",  BINARY, less),
  set_op (">",  BINARY, greater),
  set_op ("<=", BINARY, less_equal),
  set_op (">=", BINARY, greater_equal),

  set_op ("&",  BINARY, and),
  set_op ("|",  BINARY, or),
  set_op ("^",  BINARY, xor),
  set_op ("<<", BINARY, lsh),
  set_op (">>", BINARY, rsh),

  /* Binary functions.  */

  /*
  //postfix operators
  {"!!", 2, F FFACTX, 0, 0},
  {"!", 2, F FACTORIALX, 0, 0},
  {"[", 2, 0, 0, F INDEXM},
  {degSymbol, 2, F DEGX, 0, 0},

  //constants
  {"pi", 1, F PIX, 0, 0},
  {"ans", 1, 0, 0, F ANS},
  {"rand", 1, F RANDX, 0, 0},

  //prefix operators
  {"abs", 9, F ABSX, F ABSC, F ABSM},
  {"sign", 9, F SIGNX, F SIGNC, 0},
  {"round", 9, F ROUNDX, F ROUNDC, F ROUNDM},
  {"trunc", 9, F TRUNCX, F TRUNCC, F TRUNCM},
  {"int", 9, F INTX, F INTC, F INTM}, {"floor", 9, F INTX, F INTC, F INTM},
  {"ceil", 9, F CEILX, F CEILC, F CEILM},
  {"frac", 9, F FRACX, F FRACC, F FRACM},
  {"real", 9, 0, F REALC, F REALM},
  {"imag", 9, 0, F IMAGC, F IMAGM},
  {"conjg", 9, 0, F CONJGC, F CONJGM},

  {"exp", 8, F EXPX, F EXPC, 0},
  {"ln", 8, 0, F LNC, 0},
  {"log", 8, 0, F LOG10C, 0},
  {"sqrt", 8, 0, F SQRTC, 0},
  {"ffact", 8, F FFACTX, 0, 0},
  {"fact", 8, F FACTORIALX, 0, 0},
  {"not", 8, F NOTX, F NOTC, F NOTM},
  {"divisor", 8, F DIVISORX, 0, 0},
  {"prime", 8, F PRIMEX, 0, 0},
  {"isprime", 8, F ISPRIMEX, 0, 0},
  {"fibon", 8, F FIBONACCIX, 0, 0},
  {"random", 8, F RANDOMX, 0, 0},
  {"arg", 8, 0, F ARGC, 0},

  {"sin", 8, F SINX, F SINC, 0},
  {"cos", 8, F COSX, F COSC, 0},
  {"tan", 8, F TANX, F TANC, 0}, {"tg", 8, F TANX, F TANC, 0},
  {"cot", 8, F COTGX, F COTGC, 0}, {"cotg", 8, F COTGX, F COTGC, 0},
  {"sec", 8, F SECX, F SECC, 0},
  {"csc", 8, F CSCX, F CSCC, 0}, {"cosec", 8, F CSCX, F CSCC, 0},
  {"asin", 8, 0, F ASINC, 0}, {"arcsin", 8, 0, F ASINC, 0},
  {"acos", 8, 0, F ACOSC, 0}, {"arccos", 8, 0, F ACOSC, 0},
  {"atan", 8, F ATANX, F ATANC, 0}, {"atg", 8, F ATANX, F ATANC, 0},
  {"arctan", 8, F ATANX, F ATANC, 0}, {"arctg", 8, F ATANX, F ATANC, 0},
  {"acot", 8, F ACOTGX, F ACOTGC, 0}, {"acotg", 8, F ACOTGX, F ACOTGC, 0},
  {"arccot", 8, F ACOTGX, F ACOTGC, 0}, {"arccotg", 8, F ACOTGX, F ACOTGC, 0},
  {"asec", 8, 0, F ASECC, 0},
  {"acsc", 8, 0, F ACSCC, 0},

  {"sinh", 8, F SINHX, F SINHC, 0},
  {"cosh", 8, F COSHX, F COSHC, 0},
  {"tanh", 8, F TANHX, F TANHC, 0}, {"tgh", 8, F TANHX, F TANHC, 0},
  {"coth", 8, F COTGHX, F COTGHC, 0}, {"cotgh", 8, F COTGHX, F COTGHC, 0},
  {"sech", 8, 0, F SECHC, 0},
  {"csch", 8, 0, F CSCHC, 0},
  {"asinh", 8, 0, F ASINHC, 0}, {"argsinh", 8, 0, F ASINHC, 0},
  {"acosh", 8, 0, F ACOSHC, 0}, {"argcosh", 8, 0, F ACOSHC, 0},
  {"atanh", 8, 0, F ATANHC, 0}, {"atgh", 8, 0, F ATANHC, 0}, {"argtanh", 8, 0,
  F ATANHC, 0}, {"argtgh", 8, 0, F ATANHC, 0},
  {"acoth", 8, 0, F ACOTGHC, 0}, {"acotgh", 8, 0, F ACOTGHC, 0}, {"argcoth",
  8, 0, F ACOTGHC, 0}, {"argcotgh", 8, 0, F ACOTGHC, 0},
  {"asech", 8, 0, F ASECHC, 0},
  {"acsch", 8, 0, F ACSCHC, 0},

  {"radtodeg", 8, F RADTODEGX, 0, 0},
  {"degtorad", 8, F DEGTORADX, 0, 0},
  {"radtograd", 8, F RADTOGRADX, 0, 0},
  {"gradtorad", 8, F GRADTORADX, 0, 0},
  {"degtograd", 8, F DEGTOGRADX, 0, 0},
  {"gradtodeg", 8, F GRADTODEGX, 0, 0},
  {"deg", 8, F DEGX, 0, 0},
  {"rad", 8, F RADX, 0, 0},
  {"grad", 8, F GRADX, 0, 0},
  {"todeg", 8, F TODEGX, 0, 0},
  {"torad", 8, F TORADX, 0, 0},
  {"tograd", 8, F TOGRADX, 0, 0},
  {"dms", 8, F DMSTODECX, 0, 0},
  {"todms", 8, F DECTODMSX, 0, 0},
  {"ftoc", 8, F FTOCX, 0, 0},
  {"ctof", 8, F CTOFX, 0, 0},
  */
};

/*
 * Functions with real operand, but imaginary result:
 * conjg, polar, ^, #, sqrt, ln, log, asin, acos, acosh, atanh, acoth
 */

//---------------------------------------------------------------------------

void
variable_stack_destructor (void *p)
{
  struct variable_t *v = p;
  mpc_clear (v->oldx);
  mpc_clear (v->newx);
  free (v->name);
}
void
function_stack_destructor (void *p)
{
  struct function_t *f = p;
  free (f->name);
  free (f->body);
  stack_clear (f->args);
}

bool
skip_comment (const char **str)
{
  const char *s = *str;
  if (*s != '/' || *(s + 1) != '*')
    return false;
  const char *s0 = s;
  s += 2;
  while (*s != '*' || *(s + 1) != '/')
    {
      if (*s == '\0')
        {
          error_position = *str = s0;
          cerror (end_of_comment_is_missing);
          return false;
        }
      s++;
    }
  s++;
  /* Position is at the ending '/' */
  *str = s;
  return true;
}

void
skip_spaces (const char **s)
{
  while (isspace (**s) || skip_comment (s))
    (*s)++;
}

// Return 0 if there is no string at s
// Return 1 if string does not have double quotes
// Return (count of double quotes + 1)
size_t
skip_string (const char **str)
{
  const char *s = *str;
  size_t n = 0;
  if (*s == '\"')
    {
      while (true)
        {
          n++;
          for (s++; *s != '\"'; s++)
            {
              if (*s == '\0')
                {
                  cerror (missing_quote_at_end_of_printed_text);
                  *str = s;
                  return 0;
                }
            }
          if (*(s + 1) != '\"')
            break;
          // Double quotes
          s++;
        }
      // Position is at the ending "
    }
  *str = s;
  return n;
}

// Convert double qoutes to single quote
// Result is not nul terminated!
void
copy_string (char *dest, const char *src)
{
  while (true)
    {
      char c = *src++;
      if (c == '\"')
        {
          if (*src != '\"')
            break;
          src++;
        }
      *dest++ = c;
    }
}

//---------------------------------------------------------------------------

void
do_op (void)
{
  int i;
  const op_t *o;
  struct stack *t;
  void *fr, *fc;
  mpc_t a1, a2, y;

  if (cerrno != 0 || stack_is_empty (op_stack))
    return;

  mpc_init (y);
  t = stack_pop (op_stack);
  o = t->op;
  error_position = t->input_pointer;
  fr = o->rfunc;
  fc = o->cfunc;
  i = o->type;
  if (i == CONSTANT)
    {
      /* Const function.  */
      if (fr != NULL)
        ((real_func1_t)fr) (y->re);
      else
        ((complex_func1_t)fc) (y);
      stack_push (number_stack, y);
    }
  else
    {
      if (number_stack->n_used > 0 && (fr != NULL || fc != NULL))
        {
          deref (number_stack);
          a1 = stack_pop (number_stack);
          if (i == UNARY || i == POSTFIX)
            {
              // Unary operator
              if (!mpfr_is_zero (a1->im) || fr == NULL)
                {
                  if (fc == NULL)
                    cerror (function_does_not_support_complex);
                  else
                    ((real_func2_t) fc) (y, a1);
                }
              else
                ((real_func2_t) fr) (y->r, a1->r);
              mpc_clear (a1);
              numStack[numStack.len - 1] = y;
            }
          else
            {
              if (i == 9 || i >= 400)
                {
                  // Unary operator
                  if (isImag (a1) || fr == NULL)
                    {
                      if (fc == NULL)
                        cerror (err::Function_Do_Not_Support_Complex);
                      else
                        (reinterpret_cast<TunaryC0> (fc)) (a1);
                    }
                  else
                    (reinterpret_cast<Tunary0> (fr)) (a1.r);
                }
              else
                {
                  // Binary operator
                  if (number_stack.n_used < 2)
                    return;
                  numStack--;
                  if (fm != ASSIGNM)
                    {
                      deref (numStack[numStack.len - 1]);
                    }

                  a2 = numStack[numStack.len - 1];
                  y = ALLOCC (precision);

                  if (isMatrix (a1) || isMatrix (a2)
                      || fc == NULL && fr == NULL)
                    {
                      if (fm == NULL)
                        {
                          cerror (err::Function_Do_Not_Support_Matrices);
                        }
                      else
                        {
                          (reinterpret_cast<TbinaryC> (fm)) (y, a2, a1);
                        }
                    }
                  else
                    {
                      if (isImag (a1) || isImag (a2) || fr == NULL)
                        {
                          if (fc == NULL)
                            {
                              cerror (err::Function_Do_Not_Support_Complex);
                            }
                          else
                            {
                              (reinterpret_cast<TbinaryC> (fc)) (y, a2, a1);
                            }
                        }
                      else
                        {
                          (reinterpret_cast<Tbinary> (fr)) (y.r, a2.r, a1.r);
                        }
                    }
                  FREEM (a2);
                  FREEM (a1);
                  numStack[numStack.len - 1] = y;
                }
            }
        }
    }
}

//---------------------------------------------------------------------------

static bool
is_var_letter (char c)
{
  return is_latin_alpha (c) || c == '_' || signed(c) < 0
         || c >= '0' && c <= '9';
}

//---------------------------------------------------------------------------

i32
find_label (const char *s, int len)
{
  label_t *a;

  for (intmax_t i = labels.len - 1; i >= 0; i--)
    {
      a = &labels[i];
      if (len == a->name_len && !strncmp (s, a->name, a->name_len))
        return a->ind;
      if (i == 0)
        break;
    }

  return -1;
}

//---------------------------------------------------------------------------

static const op_t **op_last_compared;

struct search_info_t
{
  char c1;
  const char *s;
  int t;
};

int
cmp_search_op (search_info_t *si, const op_t **po)
{
  const char *d, *a;
  char c;
  int i;

  /* Compare the first character.  */
  d = (*po)->name;
  i = (unsigned char)si->c1 - *(const unsigned char *)d;
  if (i == 0)
    {
      /* Compare other characters.  */
      for (a = si->s + 1, d++;; d++, a++)
        {
          c = (char)tolower (*a);
          i = (unsigned char)c - *(const unsigned char *)d;
          if (i != 0 || c == '\0')
            break;
        }
      if ((c < 'a' || c > 'z' || *(d - 1) < 'a' || *(d - 1) > 'z')
          && *d == '\0' && si->t == (*op)->type)
        /* Match found.  */
        return 0;
    }

  op_last_compared = po;
  return i != 0 ? i : si->t - (*po)->type;
}

int
cmp_search_op_s (const char *s, int type, const operator_t **po)
{
  search_info_t si;
  si.s = s;
  si.c1 = *s;
  si.t = type;
  return cmp_search_operator (&si, po);
}

Token
token (const char *s, const char **e, bool is_for, bool is_postfix)
{
  char c;
  const char *a;
  mpc_t x;
  const op_t *o, **po, **po1;
  stack_t *t;
  variable_t *v;
  int i;
  size_t variable_name_lenght;
  search_info_t search_info;

  if (cerrno != 0)
    return -3;

  skip_spaces (s);
  error_position = s;
  c = *s;
  if (c == '\0' || c == ';' || c == ']' || (c == ',' && in_parenthesis == 0))
    return T_END;
  /* Read a number.  */
  if (c >= '0' && c <= '9' || c == '.')
    {
      mpc_init (x);
      s = real_read (x.r, s);
      if (*s >= '0' && *s <= '9')
        {
          error_position = s;
          cerror (digit_is_outside_selected_base);
        }
      stack_push (number_stack, x);
      *e = s;
      return T_REAL;
    }
  if (c == '(')
    in_parenthesis++;
  if (c == ')')
    in_parenthesis--;

  /* Store the first character to c1 for faster comparisons.  */
  search_info.c1 = (char)tolower (*s);
  if (!is_latin_alpha (*s) || is_latin_alpha (*(s + 1)))
    {
      /* Function name cannot be a single letter
         Find a function or an operator.  */
      search_info.s = s;
      po = (const Top **)bsearch (&si, function_tab_sorted, countof (function_tab_sorted),
                                  sizeof (Top *), (int (*) (const void *, const void *)) cmp_search_op);
      if (po == NULL)
        {
          po = op_last_compared;
          if (is_latin_alpha (search_info.c1))
            {
              /* Compare prefixes.  */
              if (po > function_tab_sorted + 1
                  && !cmp_search_operator_s ((*(po - 1))->name, po - 2))
                po--;
              while (po > function_tab_sorted
                     && !cmp_search_operator_s ((*po)->name, po - 1))
                po--;
            }
          else
            {
              /* Compare all operators which start with the same character.  */
              while (po > function_tab_sorted && *(po - 1)->name[0] == si.c1
                     && cmp_search_operator (&si, po))
                po--;
            }
          if (cmpSearchOp (&si, po))
            {
              po = 0;
            }
        }
      if (po != NULL)
        {
          /* One function can be prefix of another function,
             Find the longest function that match input string.  */
          for (po1 = po + 1; po1 < funcTabSorted + countof (funcTab); po1++)
            {
              i = cmpSearchOp (&si, po1);
              if (i < 0)
                {
                  break;
                }
              if (i == 0)
                {
                  po = po1;
                }
            }
          // Add operation to the stack
          t = opStack++;
          t->input_pointer = s;
          t->op = o = *po;
          // Skip this token
          *e = s + strlen (o->name);
          return o->type;
        }
    }
  // Operator i
  if (is_postfix)
    {
      if (si.c1 == 'i')
        {
          if (!isLetter (*(s + 1)))
            {
              t = opStack++;
              t->input_pointer = s;
              t->op = &opImag;
              s++;
              return opImag.type;
            }
        }
    }
  /*
  // Variable
  variable_name_lenght = 0;
  for (a = s; isVarLetter(*a); a++) {
    variable_name_lenght++;
  }
  if (varNameLen != 0) {
    v = findVar(s, 0);
    skipSpaces(a);
    if (v == NULL && (*a == '=' && *(a + 1) != '=' || isFor)) {
      // Create a new variable
      v = vars++;
      v->name = new char[(varNameLen + 1) * sizeof(char)];
      v->name[varNameLen] = 0;
      memcpy(v->name, s, varNameLen);
      v->newx.r = v->newx.i = 0;
      v->oldx = ALLOCC(1);
      v->modif = false;
    }
    if (v != NULL) {
      s = a;
      *numStack++ = x = ALLOCC(precision); // Deref needs this precision
      x.r[0] = reinterpret_cast<Tint>(v - vars);
      x.r[-3] = -1;
      return 0;
    }
  }
  */

  // Identifier not found
  c = *s;

  if (isLetter (c))
    cerror (unknown_function_or_variable);
  else
    cerror (unknown_operator);

  return -2;
}

void
init_all_stacks (void)
{
  stack_init (number_stack, sizeof (mpc_t), mpc_clear);
  stack_init (operation_stack, sizeof (operation_t), NULL);
  stack_init (variable_stack, sizeof (variable_t), variable_stack_destructor);
  stack_init (label_stack, sizeof (label_t), NULL);
  stack_init (goto_stack, sizeof (goto_t), NULL);
  stack_init (function_stack, sizeof (function_t), function_stack_destructor);
}

void
clear_all_stacks (void)
{
  stack_clear (function_stack);
  stack_clear (goto_stack);
  stack_clear (label_stack);
  stack_clear (variable_stack);
  stack_clear (operation_stack);
  stack_clear (number_stack);
}
