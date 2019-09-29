
/*
 * Precise Calculator
 *
 * Copyright (C) 2019 Сергей Сушилин
 * Copyright (C) 2004-2018 Petr Laštovička
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <config.h>

#include <cctype>
#include <cstring>
#include <sys/types.h>

#include <stdexcept>
#include <thread>

#include "system.hh"

#include "range.hh"
#include "matrix.hh"

#include "preccalc.hh"

const intmax_t MAX_OUTPUT_SIZE = 10000000000;

struct op_t
{
  const char *name;
  uint type;
  void *func;  // Function for Real numbers
  void *ffunc; // Function for Fractional numbers
  void *cfunc; // Function for Complex numbers
  void *mfunc; // Function for Matrices
};
struct stack_t
{
  const op_t *op;
  const char *input_ptr;
};
struct label_t
{
  const char *name;
  int name_len;
  int ind;
};

// TODO
#define msg(...)

bool write_log = false;
Matrix ans, old_ans, return_value;
const char *error_position;
int goto_position;
uint command_number;
uint in_parenthesis;
Darray<Matrix> number_stack;
Darray<stack_t> op_stack;
Darray<var_t> vars;
Darray<label_t> labels;
Darray<const char *> goto_positions;
Darray<func_t> functions;
const op_t **function_tab_sorted;

void
SETBASEX (Matrix *y, const Matrix b, const Matrix x)
{
  COPYM (y, x);
  basein = static_cast<uint> (b.A[0].r);
}

void
ANS (Matrix *y)
{
  COPYM (y, ans);
}

void
RETM (Matrix *y)
{
  if (is_nan (return_value.A[0].r))
    {
      COPYM (&return_value, *y);
      number_stack--;
      cerrno = err::overflow;
    }
}

Matrix
deref1 (Matrix &x)
{
  if (is_variable (x))
    {
      var_t *v = to_variable (x);
      return v->modif ? v->newx : v->oldx;
    }
  Complex zero = {0, 0};
  return *(to_matrix (zero));
}

bool
deref (Matrix &y, Matrix &x)
{
  if ((is_variable (x) || is_range (x)))
    {
      var_t *v = to_variable (x);
      y = v->modif ? v->newx : v->oldx;
      return true;
    }
  else
    {
      y = x;
      return false;
    }
}

void
deref (Matrix &x)
{
  Matrix v;
  if (deref (v, x))
    {
      if (is_range (x))
        {
          Matrix y;
          Tint ind = (Tint) (x.A[0].r);
          INDEXM (y, v, &ind);
          FREEM (x);
          x = y;
        }
      else
        COPYM (x, v);
    }
}

void
ASSIGNM (Matrix y, const Matrix a, const Matrix x)
{
  if (!is_variable (a) && !is_range (a))
    cerror (err::there_is_numeric_expression_at_left_side_of_assignment);
  else
    {
      COPYM (y, x);
      var_t *v = to_variable (a);
      if (is_range (a))
        {
          if (!v->modif)
            COPYM (v->newx, v->oldx);

          assign_range (v->newx, x, const_cast<Numx *> (&a.A[0].r));
        }
      else
        assign (v->newx, x);
      v->modif = true;
    }
}

void
INCDEC (Matrix &y, const Matrix &a, bool isINC)
{
  if (!is_variable (a) && !is_range (a))
    {
      if (isINC)
        cerror (err::increment_of_expression);
      else
        cerror (err::decrement_of_expression);
    }
  else
    {
      var_t *v = to_variable (a);
      Matrix &c = v->modif ? v->newx : v->oldx;
      if (is_range (a))
        {
          if (!v->modif)
            COPYM (v->newx, v->oldx);
          incdec_range (y, v->newx, isINC, &a.A[0].r);
          v->modif = true;
        }
      else
        {
          if (is_matrix (c))
            {
              if (isINC)
                cerror (err::increment_of_matrix);
              else
                cerror (err::decrement_of_matrix);
            }
          else
            {
              if (isINC)
                y.A[0].r = c.A[0].r + 1;
              else
                y.A[0].r = c.A[0].r - 1;

              y.A[0].i = c.A[0].i;
              assign (v->newx, y);
              v->modif = true;
            }
        }
    }
}

void
INCC (Matrix y, const Matrix a)
{
  INCDEC (y, a, true);
}
void
DECC (Matrix y, const Matrix a)
{
  INCDEC (y, a, false);
}

void
SWAPM (Matrix y, Matrix a, Matrix b)
{
  if (!is_variable (a) || !is_variable (b))
    {
      cerror (err::parameters_must_be_variables);
      return;
    }
  var_t *va = to_variable (a);
  var_t *vb = to_variable (b);
  if (va != vb)
    {
      if (va->modif && vb->modif)
        swap (va->newx, vb->newx);
      else
        {
          Matrix wa, wb;
          deref (wa, a);
          deref (wb, b);
          assign (va->newx, wb);
          assign (vb->newx, wa);
          va->modif = vb->modif = true;
        }
    }
  SETM (y, 0);
}

void
GOTOX (Numx y)
{
  if (!is_int (y) || y < 0 || y >= UINT_MAX)
    errGoto ();
  goto_position = (uint) (y);
}

void
GOTORELX (Numx y)
{
  if (!is_int (y) || y >= UINT_MAX)
    errGoto ();

  goto_position = command_number + (int) (y);

  if (gotoPos < 0)
    errGoto();
}

//---------------------------------------------------------------------------

enum : uint
{
  CMDVAR     = 0,
  CMDCONST   = 1,
  CMDPOSTFIX = 2,

  CMDBASE    = 3,
  CMDPLUS    = 144,
  CMDMINUS   = 143,
  CMDASSIGN  = 397,
  CMDRIGHT   = 455,
  CMDLEFT    = 401,
  CMDGOTO    = 402,
  CMDEND     = 460,
  CMDVARARG  = 500,
  CMDFOR     = 550
};

#define F (void *)

const op_t op_neg    = {0, 9, F NEGX, F NEGC, F NEGM};
const op_t op_imag   = {0, 2, 0, F ISUFFIXC, 0};
const op_t op_transp = {0, 2, 0, 0, F TRANSP2M};

/*
 * 0 = number or variable
 * 1 = constant function
 * 2 = postfix operator
 * 3 = base
 * 4..7 = binary operator
 * 8 = prefix operator
 * 9 = prefix operator
 * 10..399 = binary operator
 * 400..449 = left parenthesis, commands
 * 450..499 = right parenthesis, comma, semicolon, nul
 * 500..549 = function with variable number of arguments
 * 503 = ternary function
 * 550..599 = for, foreach
 */
const op_t function_tab[] =
{
  {"(", CMDLEFT, F 0, F 0, F 0},
  {")", CMDRIGHT, F 0, F 0, F 0},
  {",", 398, F 0, F 0, F CONCATM},
  {"\\", 399, F 0, F 0, F CONCATROWM},

  {"return", 400, F 0, F 0, F RETM},
  {"goto", CMDGOTO, F GOTOX, F 0, F 0},
  {"gotor", 400, F GOTORELX, F 0, F 0},

  {"base", CMDBASE, F 0, F 0, F SETBASEX},
  {"bin", CMDBASE, F 0, F 0, F SETBASEX},
  {"tri", CMDBASE, F 0, F 0, F SETBASEX},
  {"oct", CMDBASE, F 0, F 0, F SETBASEX},
  {"dec", CMDBASE, F 0, F 0, F SETBASEX},
  {"hex", CMDBASE, F 0, F 0, F SETBASEX},

  // Statistical functions
  {"min", 8, F 0, F 0, F MINM},
  {"max", 8, F 0, F 0, F MAXM},
  {"med", 8, F 0, F 0, F MEDIANM},
  {"mode", 8, F 0, F 0, F MODEM},
  {"sort", 9, F 0, F 0, F SORTM},
  {"sortd", 9, F 0, F 0, F SORTDM},
  {"reverse", 9, F 0, F 0, F REVERSEM},
  {"sum", 8, F 0, F 0, F SUM1M},
  {"sumx", 8, F 0, F 0, F SUMXM},
  {"sumy", 8, F 0, F 0, F SUMYM},
  {"sumq", 8, F 0, F 0, F SUM2M},
  {"sumxq", 8, F 0, F 0, F SUMX2M},
  {"sumyq", 8, F 0, F 0, F SUMY2M},
  {"sumxy", 8, F 0, F 0, F SUMXYM},
  {"geom", 8, F 0, F 0, F GEOMM},
  {"harmon", 8, F 0, F 0, F HARMONM},
  {"product", 8, F 0, F 0, F PRODUCTM},
  {"count", 9, F 0, F 0, F COUNTM},
  {"ave", 8, F 0, F 0, F AVE1M}, {"mean", 8, F 0, F 0, F AVE1M},
  {"avex", 8, F 0, F 0, F AVEXM}, {"meanx", 8, F 0, F 0, F AVEXM},
  {"avey", 8, F 0, F 0, F AVEYM}, {"meany", 8, F 0, F 0, F AVEYM},
  {"aveq", 8, F 0, F 0, F AVE2M}, {"meanq", 8, F 0, F 0, F AVE2M},
  {"avexq", 8, F 0, F 0, F AVEX2M}, {"meanxq", 8, F 0, F 0, F AVEX2M},
  {"aveyq", 8, F 0, F 0, F AVEY2M}, {"meanyq", 8, F 0, F 0, F AVEY2M},

  {"vara", 8, F 0, F 0, F VAR0M},
  {"varxa", 8, F 0, F 0, F VARX0M},
  {"varya", 8, F 0, F 0, F VARY0M},
  {"var", 8, F 0, F 0, F VAR1M},
  {"varx", 8, F 0, F 0, F VARX1M},
  {"vary", 8, F 0, F 0, F VARY1M},

  {"stdeva", 8, F 0, F 0, F STDEV0M},
  {"stdevxa", 8, F 0, F 0, F STDEVX0M},
  {"stdevya", 8, F 0, F 0, F STDEVY0M},
  {"stdev", 8, F 0, F 0, F STDEV1M},
  {"stdevx", 8, F 0, F 0, F STDEVX1M},
  {"stdevy", 8, F 0, F 0, F STDEVY1M},

  /* TODO
  {"lra", 8, F 0, F 0, F LRAM},
  {"lrb", 8, F 0, F 0, F LRBM},
  {"lrr", 8, F 0, F 0, F LRRM},
  {"lrx", CMDVARARG + 2, F 0, F 0, F LRXM},
  {"lry", CMDVARARG + 2, F 0, F 0, F LRYM},
  */

  {"gcd", 8, F 0, F 0, F GCDM},
  {"lcm", 8, F 0, F 0, F LCMM},

  {"if", CMDVARARG + 3, F IFX, F 0, F 0},

  {"integral", CMDFOR + 5, F 0, F 0, F INTEGRALM},
  {"for", CMDFOR + 4, F 0, F 0, F 0}, {"foreach", CMDFOR + 3, F 0, F 0, F 0},
  {"sumfor", CMDFOR + 4, F 0, F 0, F PLUSM}, {"sumforeach", CMDFOR + 3, F 0, F 0, F PLUSM},
  {"productfor", CMDFOR + 4, F 0, F ONEC, F MULTM}, {"productforeach", CMDFOR + 3, F 0, F ONEC, F MULTM},
  {"listfor", CMDFOR + 4, F 0, F 0, F CONCATM}, {"listforeach", CMDFOR + 3, F 0, F 0, F CONCATM},
  {"rowsfor", CMDFOR + 4, F 0, F 0, F CONCATROWM}, {"rowsforeach", CMDFOR + 3, F 0, F 0, F CONCATROWM},
  {"minfor", CMDFOR + 4, F 0, F 0, F MIN3M}, {"minforeach", CMDFOR + 3, F 0, F 0, F MIN3M},
  {"maxfor", CMDFOR + 4, F 0, F 0, F MAX3M}, {"maxforeach", CMDFOR + 3, F 0, F 0, F MAX3M},

  {"hypot", CMDVARARG + 2, F HYPOTX, F 0, F 0},
  {"polar", CMDVARARG + 2, F 0, F POLARC, F 0},
  {"complex", CMDVARARG + 2, F 0, F COMPLEXC, F 0},
  {"logn", CMDVARARG + 2, F 0, F LOGNC, F 0},

  {"++", 2, F 0, F INCC, F 0},
  {"--", 2, F 0, F DECC, F 0},

  // Binary operators
  {"or", 199, F ORX, F ORC, F ORM}, {"|", 199, F ORX, F ORC, F ORM},
  {"nor", 199, F NORX, F 0, F 0},
  {"bitnor", 199, F NORBX, F NORBC, F NORBM},
  {"xor", 198, F XORX, F XORC, F XORM},
  {"and", 197, F ANDX, F ANDC, F ANDM}, {"&", 197, F ANDX, F ANDC, F ANDM},
  {"nand", 197, F NANDX, F 0, F 0},
  {"bitnand", 197, F NANDBX, F NANDBC, F NANDBM},
  {"imp", 200, F IMPX, F 0, F 0}, {"->", 200, F IMPX, F 0, F 0},
  {"bitimp", 200, F IMPBX, F IMPBC, F IMPBM},
  {"eqv", 201, F EQVX, F 0, F 0},
  {"biteqv", 201, F EQVBX, F EQVBC, F EQVBM},

  {"==", 171, F EQUALX, F EQUALC, F EQUALM},
  {"!=", 171, F NOTEQUALX, F NOTEQUALC, F NOTEQUALM},
  {">=", 170, F GREATEREQX, F 0, F 0},
  {">", 170, F GREATERX, F 0, F 0},
  {"<=", 170, F LESSEQX, F 0, F 0},
  {"<", 170, F LESSX, F 0, F 0},

  {"lsh", 155, F LSHX, F LSHC, F LSHM}, {"<<", 155, F LSHX, F LSHC, F LSHM},
  {"rsh", 155, F RSHX, F RSHC, F RSHM}, {">>", 155, F RSHX, F RSHC, F RSHM},
  {"rshi", 155, F RSHIX, F RSHIC, F RSHIM},

  {"=", CMDASSIGN, F 0, F 0, F ASSIGNM},
  {"+", CMDPLUS, F PLUSX, F PLUSC, F PLUSM},
  {"-", CMDMINUS, F MINUSX, F MINUSC, F MINUSM},
  {"*", 121, F MULTX, F MULTC, F MULTM},
  {"/", 121, F DIVX, F DIVC, F DIVM},
  {"idiv", 121, F IDIVX, F 0, F 0},
  {"mod", 121, F MODX, F 0, F 0},
  {"rem", 121, F REMX, F 0, F 0}, {"%", 121, F REMX, F 0, F 0},
  {"^", 5, F 0, F POWC, F POWM}, {"pow", 5, F 0, F POWC, F POWM},
  {"#", 4, F 0, F ROOTC, F 0}, {"root", 4, F 0, F ROOTC, F 0},

  {"combin", 111, F COMBINX, F 0, F 0}, {"ncr", 111, F COMBINX, F 0, F 0},
  {"permut", 111, F PERMUTX, F 0, F 0}, {"npr", 111, F PERMUTX, F 0, F 0},
  {"agm", 121, F AGMX, F 0, F 0},

  // Postfix operators
  {"!", 2, F FACTORIALX, F 0, F 0}, {"fact", 2, F FACTORIALX, F 0, F 0},
  {"!!", 2, F FFACTX, F 0, F 0}, {"ffact", 2, F FFACTX, F 0, F 0},
  {"[", 2, F 0, F 0, F INDEXM},
  {"°", 2, F DEGX, F 0, F 0},

  // Constants
  {"pi", CMDCONST, F PIX, F 0, F 0},
  {"tau", CMDCONST, F TAUX, F 0, F 0},
  {"phi", CMDCONST, F PHIX, F 0, F 0},
  {"ans", CMDCONST, F 0, F 0, F ANS},
  {"rand", CMDCONST, F RANDX, F 0, F 0},

  // Prefix operators
  {"abs", 9, F ABSX, F ABSC, F ABSM},
  {"neg", 9, F NEGX, F NEGC, F NEGM},
  {"sign", 9, F SIGNX, F SIGNC, F 0},

  // TODO: does it work?
  {"!", 8, F SUBFACTX, F 0, F 0},

  {"round", 9, F ROUNDX, F ROUNDC, F ROUNDM},
  {"floor", 9, F FLOORX, F FLOORC, F FLOORM},
  {"ceil", 9, F CEILX, F CEILC, F CEILM},
  {"trunc", 9, F TRUNCX, F TRUNCC, F TRUNCM},
  {"frac", 9, F FRACX, F FRACC, F FRACM},

  {"real", 9, F 0, F REALC, F REALM}, {"re", 9, F 0, F REALC, F REALM},
  {"imag", 9, F 0, F IMAGC, F IMAGM}, {"im", 9, F 0, F IMAGC, F IMAGM},
  {"conjg", 9, F 0, F CONJGC, F CONJGM},

  {"exp", 8, F EXPX, F EXPC, F 0},
  {"ln", 8, F 0, F LNC, F 0},

  {"lb", 8, F 0, F LOG2C, F 0},
  {"lt", 8, F 0, F LOG3C, F 0},
  {"log", 8, F 0, F LOG10C, F 0}, // In preccalc 'log' always means log10, NEVER 'ln'

  {"sqrt", 8, F 0, F SQRTC, F 0},
  {"subfact", 8, F SUBFACTX, F 0, F 0},
  {"not", 8, F NOTX, F NOTC, F NOTM}, {"~", 8, F NOTX, F NOTC, F NOTM},
  // TODO: {"divisor", 8, F DIVISORX, F 0, F 0},
  {"fibon", 8, F FIBONACCIX, F FIBONACCIC, F 0},
  {"tribon", 8, F TRIBONACCIX, F 0, F 0},
  {"random", 8, F RANDOMX, F 0, F 0},
  {"arg", 8, F 0, F ARGC, F 0},

  // Trigonometric functions
  {"sin", 8, F SINX, F SINC, F 0},
  {"cos", 8, F COSX, F COSC, F 0},

  {"tan", 8, F TANX, F TANC, F 0}, {"tg", 8, F TANX, F TANC, F 0},
  {"cot", 8, F COTX, F COTC, F 0}, {"ctg", 8, F COTX, F COTC, F 0},

  {"sec", 8, F SECX, F SECC, F 0},
  {"cosec", 8, F CSCX, F CSCC, F 0}, {"csc", 8, F CSCX, F CSCC, F 0},

  // Inverse trigonometric functions
  {"asin", 8, F 0, F ASINC, 0}, {"arcsin", 8, F 0, F ASINC, F 0},

  {"acos", 8, F 0, F ACOSC, 0}, {"arccos", 8, F 0, F ACOSC, F 0},

  {"atan", 8, F ATANX, F ATANC, F 0}, {"atg", 8, F ATANX, F ATANC, F 0},
  {"arctan", 8, F ATANX, F ATANC, F 0}, {"arctg", 8, F ATANX, F ATANC, F 0},

  {"acot", 8, F ACOTX, F ACOTC, F 0}, {"actg", 8, F ACOTX, F ACOTC, F 0},
  {"arccot", 8, F ACOTX, F ACOTC, F 0}, {"arcctg", 8, F ACOTX, F ACOTC, F 0},

  {"asec", 8, F 0, F ASECC, F 0}, {"arcsec", 8, 0, F ASECC, F 0},

  {"acosec", 8, F 0, F ACSCC, F 0}, {"acsc", 8, 0, F ACSCC, F 0},
  {"arccosec", 8, F 0, F ACSCC, F 0}, {"arccsc", 8, 0, F ACSCC, F 0},

  // Hyperbolic functions
  {"sinh", 8, F SINHX, F SINHC, F 0},
  {"cosh", 8, F COSHX, F COSHC, F 0},

  {"tanh", 8, F TANHX, F TANHC, F 0}, {"tgh", 8, F TANHX, F TANHC, F 0},
  {"coth", 8, F COTHX, F COTHC, F 0}, {"ctgh", 8, F COTHX, F COTHC, F 0},

  {"sech", 8, F 0, F SECHC, F 0},
  {"cosech", 8, F 0, F CSCHC, F 0}, {"csch", 8, 0, F CSCHC, F 0},

  // Inverse hyperbolic functions
  {"asinh", 8, F 0, F ASINHC, F 0}, {"argsinh", 8, F 0, F ASINHC, F 0},

  {"acosh", 8, F 0, F ACOSHC, F 0}, {"argcosh", 8, F 0, F ACOSHC, F 0},

  {"atanh", 8, F 0, F ATANHC, F 0}, {"atgh", 8, F 0, F ATANHC, F 0},
  {"argtanh", 8, F 0, F ATANHC, F 0}, {"argtgh", 8, F 0, F ATANHC, F 0},

  {"acoth", 8, F 0, F ACOTHC, F 0}, {"actgh", 8, F 0, F ACOTHC, F 0},
  {"argcoth", 8, F 0, F ACOTHC, F 0}, {"argctgh", 8, F 0, F ACOTHC, F 0},

  {"asech", 8, F 0, F ASECHC, F 0}, {"argsech", 8, F 0, F ASECHC, F 0},

  {"acsch", 8, F 0, F ACSCHC, F 0}, {"acosech", 8, F 0, F ACSCHC, F 0},
  {"argcsch", 8, F 0, F ACSCHC, F 0}, {"argcosech", 8, F 0, F ACSCHC, F 0},

  {"radtodeg", 8, F RADTODEGX, F 0, F 0},
  {"degtorad", 8, F DEGTORADX, F 0, F 0},
  {"radtograd", 8, F RADTOGRADX, F 0, F 0},
  {"gradtorad", 8, F GRADTORADX, F 0, F 0},
  {"degtograd", 8, F DEGTOGRADX, F 0, F 0},
  {"gradtodeg", 8, F GRADTODEGX, F 0, F 0},

  {"deg", 8, F DEGX, F 0, F 0},
  {"rad", 8, F RADX, F 0, F 0},
  {"grad", 8, F GRADX, F 0, F 0},

  {"todeg", 8, F TODEGX, F 0, F 0},
  {"torad", 8, F TORADX, F 0, F 0},
  {"tograd", 8, F TOGRADX, F 0, F 0},

  {"dms", 8, F DMSTODECX, F 0, F 0},
  {"todms", 8, F DECTODMSX, F 0, F 0},
  {"ftoc", 8, F FTOCX, F 0, F 0},
  {"ctof", 8, F CTOFX, F 0, F 0},

  // Matrix operators
  {"transp", 9, F 0, F 0, F TRANSPM},
  {"invert", 8, F 0, F 0, F INVERTM},
  {"elim", 9, F 0, F 0, F ELIMM},
  {"solve", 9, F 0, F 0, F EQUSOLVEM},
  {"angle", CMDVARARG + 2, F 0, F 0, F ANGLEM},
  {"vert", 120, F 0, F 0, F VERTM},
  {"width", 9, F 0, F 0, F WIDTHM},
  {"height", 9, F 0, F 0, F HEIGHTM},
  {"det", 8, F 0, F 0, F DETM},
  {"rank", 8, F 0, F 0, F RANKM},
  {"polynom", CMDVARARG + 2, F 0, F 0, F POLYNOMM},
  {"matrix", CMDVARARG + 2, F 0, F 0, F MATRIXM},
  {"swap", CMDVARARG + 2, F 0, F 0, F SWAPM}
};

#undef F

/*
 * Functions with real operand, but imaginary result:
 * conjg, polar, ^, #, sqrt, ln, log, asin, acos, acosh, atanh, acoth
 */

//---------------------------------------------------------------------------

#define __OUTPUT_ERROR_TEXT__(txt) \
  do \
    error (0, 0, "%u: %s\n", (uint) cerrno, txt); \
  while (0)

void
cerror (err id)
{
  preccalc_assert (id > err::errM && id < err::errN, "Invalid error number");

  if (cerrno != 0)
    return;

  cerrno = id;

  switch (id)
    {
    case err::function_does_not_support_negative:
      __OUTPUT_ERROR_TEXT__ (_("The function does not support negative numbers"));
      break;
    case err::function_does_not_support_fractional:
      __OUTPUT_ERROR_TEXT__ (_("The function does not support fractional numbers"));
      break;
    case err::function_does_not_support_complex:
      __OUTPUT_ERROR_TEXT__ (_("The function does not support complex numbers"));
      break;
    case err::function_does_not_support_matrices:
      __OUTPUT_ERROR_TEXT__ (_("The function does not support matrices"));
      break;
    case err::division_by_zero:
      __OUTPUT_ERROR_TEXT__ (_("Division by zero"));
      break;
    case err::zero_power_of_zero:
      __OUTPUT_ERROR_TEXT__ (_("Zero power of zero"));
      break;
    case err::negative_power_of_zero:
      __OUTPUT_ERROR_TEXT__ (_("Negative power of zero"));
      break;
    case err::complex_power_of_zero:
      __OUTPUT_ERROR_TEXT__ (_("Complex power of zero"));
      break;
    case err::not_integer_power_of_negative_number:
      __OUTPUT_ERROR_TEXT__ (_("Not integer power of negative number"));
      break;
    case err::zero_root_of_zero:
      __OUTPUT_ERROR_TEXT__ (_("Zero root of zero"));
      break;
    case err::complex_root_of_zero:
      __OUTPUT_ERROR_TEXT__ (_("Complex root of zero"));
      break;
    case err::negative_root_of_zero:
      __OUTPUT_ERROR_TEXT__ (_("Negative root of zero"));
      break;
    case err::even_root_of_negative_number:
      __OUTPUT_ERROR_TEXT__ (_("Even root of negative number"));
      break;
    case err::not_integer_root_of_negative_number:
      __OUTPUT_ERROR_TEXT__ (_("Not integer root of negative number"));
      break;
    case err::logarithm_of_zero:
      __OUTPUT_ERROR_TEXT__ (_("Logarithm of zero"));
      break;
    case err::logarithm_of_number_to_base_zero:
      __OUTPUT_ERROR_TEXT__ (_("Logarithm of number to base zero"));
      break;
    case err::logarithm_of_not_positive_number:
      __OUTPUT_ERROR_TEXT__ (_("Logarithm of not positive number"));
      break;
    case err::overflow:
      __OUTPUT_ERROR_TEXT__ (_("Overflow, number is too big"));
      break;
    case err::sign_must_be_equal_zero_or_one:
      __OUTPUT_ERROR_TEXT__ (_("Sign must be equal zero or one"));
      break;
    case err::shift_operand_is_not_integer:
      __OUTPUT_ERROR_TEXT__ (_("The shift operand is not integer"));
      break;
    case err::operand_of_combination_is_not_integer:
      __OUTPUT_ERROR_TEXT__ (_("Operand of combination is not integer"));
      break;
    case err::operand_of_permutation_is_not_integer:
      __OUTPUT_ERROR_TEXT__ (_("Operand of permutation is not integer"));
      break;
    case err::operand_of_factorial_is_not_integer:
      __OUTPUT_ERROR_TEXT__ (_("Operand of factorial is not integer"));
      break;
    case err::gcd_of_non_integer:
      __OUTPUT_ERROR_TEXT__ (_("The greatest common divisor (gcd) of non-integers"));
      break;
    case err::lcm_of_non_integer:
      __OUTPUT_ERROR_TEXT__ (_("The least common multiple (lcm) of non-integers"));
      break;
    case err::gcd_of_real_numbers:
      __OUTPUT_ERROR_TEXT__ (_("The greatest common divisor (gcd) of real numbers"));
      break;
    case err::lcm_of_real_numbers:
      __OUTPUT_ERROR_TEXT__ (_("The least common multiple (lcm) of real numbers"));
      break;
    case err::gcd_of_complex_numbers:
      __OUTPUT_ERROR_TEXT__ (_("The greatest common divisor (gcd) of complex numbers"));
      break;
    case err::lcm_of_complex_numbers:
      __OUTPUT_ERROR_TEXT__ (_("The least common multiple (lcm) of complex numbers"));
      break;
    case err::divisor_of_real_number:
      __OUTPUT_ERROR_TEXT__ (_("Divisor of real number"));
      break;
    case err::computation_is_too_difficult:
      __OUTPUT_ERROR_TEXT__ (_("Computation is too difficult"));
      break;
    case err::angle_of_point_0_0:
      __OUTPUT_ERROR_TEXT__ (_("Angle of point [0, 0]"));
      break;
    case err::invalid_number_after_goto:
      __OUTPUT_ERROR_TEXT__ (_("Invalid number after goto"));
      break;
    case err::infinite_result:
      __OUTPUT_ERROR_TEXT__ (_("Infinite result"));
      break;
    case err::operand_of_arcsin_is_not_in_minus_one_one:
      __OUTPUT_ERROR_TEXT__ (_("Operand of arcsin is not in <-1, +1>"));
      break;
    case err::operand_of_arccos_is_not_in_minus_one_one:
      __OUTPUT_ERROR_TEXT__ (_("Operand of arccos is not in <-1, +1>"));
      break;
    case err::arctanh_from_one:
      __OUTPUT_ERROR_TEXT__ (_("arctanh from 1"));
      break;
    case err::arctanh_from_minus_one:
      __OUTPUT_ERROR_TEXT__ (_("arctanh from -1"));
      break;
    case err::arccoth_from_one:
      __OUTPUT_ERROR_TEXT__ (_("arccoth from 1"));
      break;
    case err::arccoth_from_minus_one:
      __OUTPUT_ERROR_TEXT__ (_("arccoth from -1"));
      break;
    case err::arctan_from_imag_one:
      __OUTPUT_ERROR_TEXT__ (_("arctan from 1i"));
      break;
    case err::arctan_from_imag_minus_one:
      __OUTPUT_ERROR_TEXT__ (_("arctan from -1i"));
      break;
    case err::arccot_from_imag_one:
      __OUTPUT_ERROR_TEXT__ (_("arccot from 1i"));
      break;
    case err::arccot_from_imag_minus_one:
      __OUTPUT_ERROR_TEXT__ (_("arccot from -1i"));
      break;
    case err::matrices_have_incorrect_size:
      __OUTPUT_ERROR_TEXT__ (_("Matrices have incorrect size"));
      break;
    case err::increment_of_matrix:
      __OUTPUT_ERROR_TEXT__ (_("Increment of a matrix"));
      break;
    case err::decrement_of_matrix:
      __OUTPUT_ERROR_TEXT__ (_("Decrement of a matrix"));
      break;
    case err::matrices_do_not_have_same_size:
      __OUTPUT_ERROR_TEXT__ (_("Matrices do not have the same size"));
      break;
    case err::matrices_do_not_have_same_number_of_columns:
      __OUTPUT_ERROR_TEXT__ (_("Number of columns of the first matrix is not equal to number of columns of the second matrix"));
      break;
    case err::vectors_have_different_lengths:
      __OUTPUT_ERROR_TEXT__ (_("Vectors have different lengths"));
      break;
    case err::not_integer_power_of_matrix:
      __OUTPUT_ERROR_TEXT__ (_("Not integer power of matrix"));
      break;
    case err::matrix_is_not_square:
      __OUTPUT_ERROR_TEXT__ (_("Matrix is not square"));
      break;
    case err::arguments_have_to_be_vectors_of_length_three:
      __OUTPUT_ERROR_TEXT__ (_("Arguments have to be vectors of length 3"));
      break;
    case err::matrix_is_not_regular:
      __OUTPUT_ERROR_TEXT__ (_("Matrix is not regular"));
      break;
    case err::index_is_out_of_range:
      __OUTPUT_ERROR_TEXT__ (_("Index is out of range"));
      break;
    case err::index_is_not_integer:
      __OUTPUT_ERROR_TEXT__ (_("Index is not integer"));
      break;
    case err::index_is_less_than_zero:
      __OUTPUT_ERROR_TEXT__ (_("Index is less than zero"));
      break;
    case err::matrix_does_not_have_two_columns:
      __OUTPUT_ERROR_TEXT__ (_("Matrix does not have 2 columns"));
      break;
    case err::matrix_must_have_more_than_one_column:
      __OUTPUT_ERROR_TEXT__ (_("Matrix must have more than one column"));
      break;
    case err::equations_are_not_solvable:
      __OUTPUT_ERROR_TEXT__ (_("Equations are not solvable"));
      break;
    case err::parameter_is_not_vector:
      __OUTPUT_ERROR_TEXT__ (_("Parameter is not a vector"));
      break;
    case err::first_parameter_has_to_be_variable:
      __OUTPUT_ERROR_TEXT__ (_("The first parameter has to be a variable"));
      break;
    case err::fourth_argument_has_to_be_integer:
      __OUTPUT_ERROR_TEXT__ (_("The fourth argument has to be integer"));
      break;
    case err::function_matrix_must_have_parameters_number_of_rows_number_of_columns:
      __OUTPUT_ERROR_TEXT__ (_("Function matrix must have parameters number of rows, number of columns"));
      break;
    case err::trigonometric_function_operand_is_too_big:
      __OUTPUT_ERROR_TEXT__ (_("Trigonometric function operand is too big"));
      break;
    case err::parameters_must_be_variables:
      __OUTPUT_ERROR_TEXT__ (_("Parameters must be variables"));
      break;
    case err::result_is_too_long:
      __OUTPUT_ERROR_TEXT__ (_("Result is too long"));
      break;
    case err::first_operand_of_mod_is_longer_than_precision:
      __OUTPUT_ERROR_TEXT__ (_("The first operand of modulus (mod) function is longer than precision"));
      break;
    case err::first_operand_of_rem_is_longer_than_precision:
      __OUTPUT_ERROR_TEXT__ (_("The first operand of remainder (rem) function is longer than precision"));
      break;
    case err::mod_of_real_numbers:
      __OUTPUT_ERROR_TEXT__ (_("The modulus (mod) function of real numbers"));
      break;
    case err::rem_of_real_numbers:
      __OUTPUT_ERROR_TEXT__ (_("The remainder (rem) function of real numbers"));
      break;
    case err::there_is_numeric_expression_at_left_side_of_assignment:
      __OUTPUT_ERROR_TEXT__ (_("There is a numeric expression at the left side of an assignment"));
      break;
    case err::increment_of_expression:
      __OUTPUT_ERROR_TEXT__ (_("Increment of an expression"));
      break;
    case err::decrement_of_expression:
      __OUTPUT_ERROR_TEXT__ (_("Decrement of an expression"));
      break;
    case err::right_bracket_expected:
      __OUTPUT_ERROR_TEXT__ (_("] expected"));
      break;
    case err::digit_is_outside_selected_base:
      __OUTPUT_ERROR_TEXT__ (_("The digit is outside the selected base"));
      break;
    case err::unknown_function_or_variable:
      __OUTPUT_ERROR_TEXT__ (_("Unknown function or variable"));
      break;
    case err::unknown_operator:
      __OUTPUT_ERROR_TEXT__ (_("Unknown operator"));
      break;
    case err::left_parenthesis_expected:
      __OUTPUT_ERROR_TEXT__ (_("Left parenthesis expected"));
      break;
    case err::wrong_number_of_arguments:
      __OUTPUT_ERROR_TEXT__ (_("Wrong number of arguments"));
      break;
    case err::number_variable_or_prefix_function_expected:
      __OUTPUT_ERROR_TEXT__ (_("Number, variable or prefix function expected"));
      break;
    case err::binary_operator_expected:
      __OUTPUT_ERROR_TEXT__ (_("Binary operator expected"));
      break;
    case err::missing_quote_at_end_of_printed_text:
      __OUTPUT_ERROR_TEXT__ (_("Missing quote at the end of a printed text"));
      break;
    case err::fatal_error:
      __OUTPUT_ERROR_TEXT__ (_("Fatal error"));
      break;
    case err::unmatched_comma:
      __OUTPUT_ERROR_TEXT__ (_("Unmatched comma"));
      break;
    case err::unmatched_right_bracket:
      __OUTPUT_ERROR_TEXT__ (_("Unmatched right bracket"));
      break;
    case err::unmatched_parenthesis:
      __OUTPUT_ERROR_TEXT__ (_("Unmatched parenthesis"));
      break;
    case err::end_of_comment_is_missing:
      __OUTPUT_ERROR_TEXT__ (_("End of comment is missing"));
      break;
    default:
      msg (_("Undefined error message %i"), static_cast<uint> (id));
      std::terminate ();
    }
}

#undef __OUTPUT_ERROR_TEXT__

void
overflow ()
{
  cerror (err::overflow);
}
void
errGoto ()
{
  cerror (err::invalid_number_after_goto);
}
void
indexOutOfRange ()
{
  cerror (err::index_is_out_of_range);
}
void
differentSize ()
{
  cerror (err::matrices_do_not_have_same_size);
}

//---------------------------------------------------------------------------

void
cleanup ()
{
  for (size_t i = 0; i < number_stack.len; i++)
    FREEM (number_stack[i]);

  number_stack.len = 0;
  op_stack.len  = 0;
}

void
var_t::destroy ()
{
  FREEM (oldx);
  FREEM (newx);
  delete[] name;
}

void
func_t::destroy ()
{
  delete[] name;
  delete[] body;
  deleteDarray (args);
}

bool
skip_comment (const char *&s)
{
  if (*s != '/' || *(s + 1) != '*')
    return false;
  const char *s0 = s;
  s += 2;
  while (*s != '*' || *(s + 1) != '/')
    {
      if (*s == '\0')
        {
          error_position = s = s0;
          cerror (err::end_of_comment_is_missing);
          return false;
        }
      ++s;
    }
  ++s;
  // Position is at the ending '/'
  return true;
}

__const bool
is_space (const char c)
{
  if (c == ' '
   || c == '\t'
   || c == '\r'
   || c == '\n'
   || c == '\f'
   || c == '\v')
    return true;
  else
    return false;
}

__pure void
skipSpaces (const char *&s)
{
  while (is_space (*s) || skip_comment (s))
    ++s;
}

// Return 0 if there is no string at s
// Return 1 if string does not have double quotes
// Return (count of double quotes + 1)
size_t
skip_string (const char *&s)
{
  size_t n = 0;
  if (*s == '\"')
    {
      while (true)
        {
          ++n;
          for (++s; *s != '\"'; ++s)
            {
              if (*s == '\0')
                {
                  cerror (err::missing_quote_at_end_of_printed_text);
                  return false;
                }
            }
          if (*(s + 1) != '\"')
            break;
          // Double quotes
          ++s;
        }
      // Position is at the ending "
    }
  return n;
}

// Convert double qoutes to single quote
// Result is not nul terminated!
__const void
copy_string (char *dest, const char *src)
{
  while (true)
    {
      char c = *src++;
      if (c == '\"')
        {
          if (*src != '\"')
            break;
          ++src;
        }
      *dest++ = c;
    }
}

//---------------------------------------------------------------------------

void
do_op ()
{
  uint i;
  const op_t *o;
  stack_t *t;
  void *fr, *fc, *fm;
  Matrix a1, a2, y;

  if (cerrno != 0 || op_stack.len == 0)
    return;

  t = op_stack--;
  o = t->op;
  error_position = t->input_ptr;
  fr = o->func;
  fc = o->cfunc;
  fm = o->mfunc;
  i = o->type;
  if (i == CMDCONST)
    {
      // Const function
      if (fm != nullptr)
        (reinterpret_cast<unaryM0_t> (fm)) (y);
      else
        {
          y.tag = Tag::COMPLEX;
          y.len = y.cols = y.rows = 1;
          if (fr != nullptr)
            (reinterpret_cast<unary0_t> (fr)) (&y.A[0].r);
          else
            (reinterpret_cast<unaryC0_t> (fc)) (&y.A[0]);
        }
      *number_stack++ = y;
    }
  else
    {
      if (number_stack.len > 0 && (fr != nullptr || fc != nullptr || fm != nullptr))
        {
          if (fc != DECC && fc != INCC)
            deref (number_stack[number_stack.len - 1]);
          a1 = number_stack[number_stack.len - 1];
          if (i == 8 || i == 2)
            {
              // Unary operator
              if (is_matrix (a1) || fc == nullptr && fr == nullptr)
                {
                  if (fm == nullptr)
                    cerror (err::function_do_not_support_matrices);
                  else
                    (reinterpret_cast<unaryM2_t> (fm)) (y, a1);
                }
              else
                {
                  if (is_imag (a1.A[0]) || fr == nullptr)
                    {
                      if (fc == nullptr)
                        cerror (err::function_do_not_support_complex);
                      else
                        {
                          matrix2complex (y);
                          (reinterpret_cast<unaryC2_t> (fc)) (&y.A[0], a1.A[0]);
                        }
                    }
                  else
                    {
                      matrix2complex (y);
                      (reinterpret_cast<unary2_t> (fr)) (&y.A[0].r, a1.A[0].r);
                    }
                }
              FREEM (a1);
              number_stack[number_stack.len - 1] = y;
            }
          else
            {
              if (i == 9 || i >= 400)
                {
                  // Unary operator
                  if (is_matrix (a1) || fc == nullptr && fr == nullptr)
                    {
                      if (fm == nullptr)
                        cerror (err::function_do_not_support_matrices);
                      else
                        (reinterpret_cast<unaryM0_t> (fm)) (a1);
                    }
                  else
                    {
                      if (is_imag (a1.A[0]) || fr == nullptr)
                        {
                          if (fc == nullptr)
                            cerror (err::function_do_not_support_complex);
                          else
                            {
                              (reinterpret_cast<unaryC0_t> (fc)) (&a1.A[0]);
                              matrix2complex (a1);
                            }
                        }
                      else
                        {
                          (reinterpret_cast<unary0_t> (fr)) (&a1.A[0].r);
                          matrix2complex (a1);
                        }
                    }
                }
              else
                {
                  // Binary operator
                  if (number_stack.len < 2)
                    return;
                  number_stack--;
                  if (fm != ASSIGNM)
                    deref (number_stack[number_stack.len - 1]);

                  a2 = number_stack[number_stack.len - 1];

                  if (is_matrix (a1)
                   || is_matrix (a2)
                   || fc == nullptr && fr == nullptr)
                    {
                      if (fm == nullptr)
                        cerror (err::function_do_not_support_matrices);
                      else
                        (reinterpret_cast<binaryM_t> (fm)) (y, a2, a1);
                    }
                  else
                    {
                      if (is_imag (a1.A[0]) || is_imag (a2.A[0]) || fr == nullptr)
                        {
                          if (fc == nullptr)
                            cerror (err::function_do_not_support_complex);
                          else
                            {
                              (reinterpret_cast<binaryC_t> (fc)) (&y.A[0], a2.A[0], a1.A[0]);
                              matrix2complex (y);
                              matrix2complex (a1);
                              matrix2complex (a2);
                            }
                        }
                      else
                        {
                          (reinterpret_cast<binary_t> (fr)) (&y.A[0].r, a2.A[0].r, a1.A[0].r);
                          matrix2complex (y);
                          matrix2complex (a1);
                          matrix2complex (a2);
                        }
                    }
                  FREEM (a2);
                  FREEM (a1);
                  number_stack[number_stack.len - 1] = y;
                }
            }
        }
    }
}

//---------------------------------------------------------------------------

bool
is_var_letter (char c)
{
  return is_letter (c) || signed (c) < 0 || c >= '0' && c <= '9';
}

bool
is_letter (char c)
{
  return c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c == '_';
}

var_t *
find_var (const char *s, const char **e)
{
  var_t *v;
  const char *d, *a;
  char c;

  for (intmax_t i = vars.len - 1;; --i)
    {
      v = &vars[i];
      for (d = v->name, a = s;; ++d, ++a)
        {
          c = static_cast<char> (tolower (*a));
          if (c != tolower (*d) || !c)
            break;
        }
      if (*d == '\0' && !is_var_letter (c))
        {
          if (e != nullptr)
            *e = a;
          return v;
        }
      if (i == 0)
        break;
    }
  return 0;
}

//---------------------------------------------------------------------------

int
find_label (const char *s, int len)
{
  label_t *a;

  for (intmax_t i = labels.len - 1;; --i)
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

intmax_t
get_index ()
{
  intmax_t result = 0;
  if (number_stack.len != 0)
    {
      Matrix &x = *number_stack--;
      deref (x);
      if (is_matrix (x))
        cerror (err::function_do_not_support_matrices);
      else if (is_imag (x.A[0]))
        cerror (err::function_do_not_support_complex);
      else
        {
          if (!is_int (x.A[0].r))
            cerror (err::index_is_not_integer);
          else
            {
              result = (intmax_t) (x.A[0].r);
              if (result < 0)
                cerror (err::index_is_less_than_zero);
            }
        }
      FREEM (x);
    }
  return result;
}

void
array_index (const char *&s)
{
  Tint D[2][2];
  Matrix y, x1;

  op_stack--;
  D[0][0] = D[1][0] = -1;
  for (intmax_t i = 0;; ++i)
    {
      skip_spaces (s);
      if (*s != ']')
        {
          parse (s, &s);
          if (cerrno != 0)
            return;
          D[i][0] = D[i][1] = get_index ();
          if (*s == ',')
            {
              parse (s + 1, &s);
              if (cerrno != 0)
                return;
              D[i][1] = get_index ();
            }
          if (*s != ']')
            {
              cerror (err::right_bracket_expected);
              return;
            }
        }
      ++s;
      skip_spaces (s);
      if (i != 0 || *s != '[')
        break;
      ++s;
    }

  if (number_stack.len == 0)
    return;
  Matrix &x = number_stack[number_stack.len - 1];
  if (is_variable (x))
    {
      // Create range
      x.A[0].r.tag = Tag::RANGE;
      Numx xD[4] = &x.A[0].r;
      xD[0] = D[0][0];
      xD[1] = D[0][1];
      xD[2] = D[1][0];
      xD[3] = D[1][1];
    }
  else
    {
      deref (x1, x);
      INDEXM (y, x1, &D[0][0]);
      FREEM (x);
      x = y;
    }
}

//---------------------------------------------------------------------------

static const op_t **opLastCompared;

struct search_info_t
{
  char c1;
  const char *s;
};

int
compare_search_op (search_info_t *si, const op_t **po)
{
  const char *d, *a;
  char c;
  ptrdiff_t i;

  // Compare the first character
  d = (*po)->name;
  i = static_cast<uchar> (si->c1) - *(reinterpret_cast<const uchar *> (d));
  if (i == 0)
    {
      // Compare other characters
      for (a = si->s + 1, d++;; d++, a++)
        {
          c = static_cast<char> (tolower (*a));
          i = static_cast<uchar> (c) - *(reinterpret_cast<const uchar *> (d));
          if (i != 0 || c == '\0')
            break;
        }
      if ((c < 'a' || c > 'z' || d[-1] < 'a' || d[-1] > 'z') && *d == '\0')
        // Match found
        return 0;
    }
  op_last_compared = po;
  return i;
}

int
compare_search_ops (const char *s, const op_t **po)
{
  search_info_t si;
  si.s = s;
  si.c1 = *s;
  return compare_search_op (&si, po);
}

int
token (const char *&s, bool is_for = false, bool is_postfix = false)
{
  char c;
  const char *a;
  Matrix x;
  const op_t *o, **po, **po1;
  stack_t *t;
  var_t *v;
  int i;
  size_t variable_name_len;
  search_info_t si;

  if (cerrno != 0)
    return -3;

  skip_spaces (s);
  error_position = s;
  c = *s;
  if (c == '\0' || c == ';' || c == ']' || (c == ',' && in_parenthesis == 0))
    return CMDEND;
  // Read a number
  if (c >= '0' && c <= '9' || c == '.')
    {
      s = READX (&x.A[0].r, s);
      if (*s >= '0' && *s <= '9')
        {
          error_position = s;
          cerror (err::digit_is_outside_selected_base);
        }
      *number_stack++ = x;
      return 0;
    }
  if (c == '(')
    ++in_parenthesis;
  if (c == ')')
    --in_parenthesis;

  // Store the first character to c1 for faster comparisons
  si.c1 = static_cast<char> (tolower (*s));
  if (!is_letter (*s) || is_letter (s[1]))
    {
      // Function name cannot be a single letter
      // Find a function or an operator
      si.s = s;
      po = reinterpret_cast<const op_t **> (bsearch (&si, function_tab_sorted, countof (function_tab), sizeof (op_t *), (int (*) (const void *, const void *)) compare_search_op));
      if (po == nullptr)
        {
          po = op_last_compared;
          if (is_letter (si.c1))
            {
              // Compare prefixes
              if (po > function_tab_sorted + 1 && !compare_search_ops (po[-1]->name, po - 2))
                --po;
              while (po > function_tab_sorted && !compare_search_ops ((*po)->name, po - 1))
                --po;
            }
          else
            {
              // Compare all operators which start with the same character
              while (po > function_tab_sorted && po[-1]->name[0] == si.c1 && compare_search_op (&si, po))
                --po;
            }
          if (compare_search_op (&si, po))
            po = nullptr;
        }
      if (po != nullptr)
        {
          // One function can be prefix of another function,
          // Find the longest function that match input string
          for (po1 = po + 1; po1 < function_tab_sorted + countof (function_tab); ++po1)
            {
              i = compare_search_op (&si, po1);
              if (i < 0)
                break;
              if (i == 0)
                po = po1;
            }
          // Add operation to the stack
          t = op_stack++;
          t->input_ptr = s;
          t->op = o = *po;
          // Skip this token
          s += strlen (o->name);
          return o->type;
        }
    }
  // Operators i, j, '
  if (is_postfix)
    {
      if (si.c1 == 'i' || si.c1 == 'j')
        {
          imag_char = si.c1;
          if (!is_letter (s[1]))
            {
              t = op_stack++;
              t->input_ptr = s;
              t->op = &opImag;
              ++s;
              return op_imag.type;
            }
        }
      if (si.c1 == '\'')
        {
          t = op_stack++;
          t->input_ptr = s;
          t->op = &opTransp;
          ++s;
          return op_transp.type;
        }
    }
  // Variable
  varNameLen = 0;
  for (a = s; is_var_letter (*a); a++)
    varNameLen++;
  if (variable_name_len != 0)
    {
      v = find_var (s, 0);
      skip_spaces (a);
      if (v == nullptr && (*a == '=' && *(a + 1) != '=' || is_for))
        {
          // Create a new variable
          v = vars++;
          v->name = new char[variable_name_len + 1];
          v->name[variable_name_len] = 0;
          std::memcpy (v->name, s, variable_name_len);
          v->newx.A[0].r = v->newx.A[0].i = 0;
          v->modif = false;
        }
      if (v != nullptr)
        {
          s = a;
          *number_stack++ = x;
          x.A[0].r = reinterpret_cast<ssize_t> (v - vars);
          x.A[0].r.tag = Tag::VARIABLE;
          return 0;
        }
    }
  // Identifier not found
  c = *s;
  if (is_letter (c))
    cerror (err::unknown_function_or_variable);
  else
    cerror (err::unknown_operator);
  return -2;
}

//---------------------------------------------------------------------------

void
skip_arg (const char *s, const char **e)
{
  size_t d, b;
  char c;

  for (d = b = 0;; ++s)
    {
      skip_comment (s);
      skip_string (s);
      c = *s;
      if (c == '(')
        ++d;
      if (c == ')')
        {
          if (d == 0)
            break;
          --d;
        }
      if (c == '[') ++b;
      if (c == ']') --b;

      if (c == 0 || c == ';' || (c == ',' && d == 0 && b == 0))
        break;
    }
  *e = s;
}

//---------------------------------------------------------------------------

int
args (const char *input, const char **end)
{
  size_t i, n;
  ssize_t j, len = 0;
  const char *s, *e, *formula = nullptr;
  Matrix y, t, u, *stack_end;
  Complex *A = nullptr;
  const op_t *o;
  void *fr, *fc, *fm;
  stack_t stk;
  bool imag, matrix, is_if, is_each, if_cond = false;

  stk = *op_stack--;
  o = stk.op;
  is_if = o->func == IFX;
  s = input;
  skip_spaces (s);
  if (*s != '(')
    {
      error_position = s;
      cerror (err::left_parenthesis_expected);
      return -2;
    }
  ++s;

  for (i = 1;; ++i)
    {
      if (o->type == CMDFOR + i)
        {
          formula = s;
          skip_arg (s, &e);
        }
      else
        {
          if (o->type >= CMDFOR && i == 1)
            {
              j = token (s, true);
              e = s;
              if (j != 0 || !is_variable (number_stack[number_stack.len - 1]))
                cerror (err::first_parameter_has_to_be_variable);
            }
          else
            parse (s, &e);
        }
      if (cerrno != 0)
        return -1;
      if (*e != ',')
        break;
      s = e + 1;
      if (is_if)
        {
          if (i == 1)
            {
              deref (number_stack[number_stack.len - 1]);
              y = number_stack--;
              if_cond = !is_zero (*y);
              FREEM (y);
            }
          if (i == 1 + (if_cond ? 1 : 0))
            {
              skip_arg (s, &e);
              ++i;
              if (*e != ',')
                break;
              s = e + 1;
            }
        }
    }
  if (*e == ')')
    ++e;
  if (end != nullptr)
    *end = e;
  n = o->type - CMDVARARG;
  if (n > 50)
    n -= (CMDFOR - CMDVARARG);
  if (n != 0 && i != n)
    {
      error_position = input;
      cerror (err::wrong_number_of_arguments);
      return -3;
    }
  if (is_if)
    return 0;

  imag = matrix = false;
  stack_end = &number_stack[number_stack.len];
  j = -1;
  if (formula != nullptr)
    i -= 2;
  for (; static_cast<size_t> (-j) <= i; --j)
    {
      Matrix &v = stack_end[j];
      if (o->mfunc != SWAPM)
        deref (v);
      if (is_matrix (v))
        matrix = true;
      if (is_imag (v.A[0]))
        imag = true;
    }
  if (formula != nullptr)
    ++i;
  error_position = stk.input_ptr;
  fm = o->mfunc;
  fc = o->cfunc;
  if (o->type >= CMDFOR)
    {
      if (fm == INTEGRALM)
        {
          (reinterpret_cast<void (*) (Matrix, Matrix, Matrix, Matrix, Matrix, const char *)> (o->mfunc)) (y, stack_end[-3], stack_end[-2], stack_end[-1], stack_end[-4], formula);
        }
      else
        {
          // For cycle
          is_each = o->type != CMDFOR + 4;
          if (is_each)
            {
              // Get matrix length and pointer to items
              if (is_matrix (stack_end[-1]))
                {
                  Matrix m = stack_end[-1];
                  len = m.len;
                  A = m.A;
                }
              else
                {
                  len = 1;
                  A = to_complex (stack_end[-1]);
                }
            }
          else
            {
              // First value and last value must be real or complex
              if (is_matrix (stack_end[-2]) || is_matrix (stack_end[-1]))
                {
                  cerror (err::function_do_not_support_matrices);
                  goto lend;
                }
              // Assign the first value to variable
              ASSIGNM (stack_end[-2], stack_end[-3], stack_end[-2]);
            }
          if (fc != nullptr)
            {
              (reinterpret_cast<unaryC0_t> (fc)) (&y.A[0]);
              matrix2complex (y);
            }
          for (j = 0;; ++j)
            {
              if (is_each)
                {
                  if (j >= len)
                    break;
                  // Assign matrix item to variable
                  ASSIGNM (A[j], stack_end[-2].A[0], A[j]);
                }
              else
                {
                  // Is variable greater then last value
                  if (cerrno != 0 || CMPC (to_variable (stack_end[-3])->newx.A[0], stack_end[-1].A[0]) > 0)
                    break;
                }
              // Evaluate expression
              parse (formula, &e);
              if (cerrno != 0)
                break;
              deref (*stack_end);
              // Add item to result
              if (fm != nullptr)
                {
                  if (j != 0)
                    {
                      (reinterpret_cast<binaryM_t> (fm)) (u, y, *stack_end);
                      swap (y, u);
                    }
                  else
                    swap (y, *stack_end);
                }
              FREEM (*number_stack--);
              if (!isEach)
                // Increment variable
                INCC (t, stack_end[-3]);
            }
          FREEM (u);
          FREEM (t);
        }
    }
  else
    {
      // Call the function
      fr = o->func;
      if (fr == nullptr)
        {
          imag = true;
          if (fc == nullptr)
            matrix = true;
        }
      if (matrix)
        {
          fc = fm;
          imag = true;
        }
      if (matrix && fm == nullptr)
        cerror (err::function_do_not_support_matrices);
      else
        {
          if (imag && fc == nullptr)
            cerror (err::function_do_not_support_complex);
          else
            {
              if (n == 2)
                {
                  if (imag)
                    (reinterpret_cast<binaryC_t> (fc)) (&y.A[0], stack_end[-2].A[0], stack_end[-1].A[0]);
                  else
                    (reinterpret_cast<binary_t> (fr)) (&y.A[0].r, stack_end[-2].A[0].r, stack_end[-1].A[0].r);
                  matrix2complex (y);
                }
              else
                {
                  if (n != 0)
                    {
                      if (imag)
                        (reinterpret_cast<arrayargC_t> (fc)) (&y.A[0], stack_end[-i].A);
                      else
                        (reinterpret_cast<arrayarg_t> (fr)) (&y.A[0].r, stack_end[-i].A);
                    }
                  else
                    {
                      if (imag)
                        (reinterpret_cast<varargC_t> (fc)) (&y.A[0], i, stack_end[-i].A);
                      else
                        (reinterpret_cast<vararg_t> (fr)) (&y.A[0].r, i, stack_end[-i].A);
                    }
                }
            }
        }
    }

lend:
  // Free arguments
  while (i-- != 0)
    FREEM (number_stack--);
  // Store the result
  *number_stack++ = y;
  return 0;
}

//---------------------------------------------------------------------------

void
parse (const char *input, const char **e)
{
  uint t;
  int u, in_par;
  Tlen stackBeg = op_stack.len;
  stack_t stk;
  const char *s = input, *b;
  Matrix a;

  in_par = in_parenthesis;
  in_parenthesis = 0;
  for (;;)
    {
      // Prefix operators
      do
        {
          t = token (s);
          if (t == CMDPLUS)
            {
              t = 8;
              op_stack--;
            }
          if (t == CMDMINUS)
            {
              op_stack[op_stack.len - 1].op = &opNeg;
              t = opNeg.type;
            }
          if (t >= CMDVARARG && t < CMDVARARG + 100)
            {
              args (s, &s);
              t = 0;
            }
          if (t == CMDBASE)
            {
              // Save current base
              *number_stack++ = a;
              a.A[0].r = baseIn;
              // Set new base
              switch (errPos[2])
                {
                case 'x':
                case 'X':
                  baseIn = 16;
                  break;
                case 'c':
                case 'C':
                  baseIn = 10;
                  break;
                case 't':
                case 'T':
                  baseIn = 8;
                  break;
                case 'i':
                case 'I':
                  baseIn = 3;
                  break;
                case 'n':
                case 'N':
                  baseIn = 2;
                  break;
                default:
                  baseIn = 0;
                  while (*s >= '0' && *s <= '9')
                    {
                      baseIn = baseIn * 10 + (*s - '0');
                      s++;
                    }
                  break;
                }
              if (baseIn == 0)
                errPos = s;
              else
                {
                  skipSpaces (s);
                  if (*s == ';' || *s == '\0')
                    {
                      t = 0;
                      op_stack--;
                    }
                  else
                    t = 8;
                }
            }
          if (t == CMDGOTO)
            {
              skipSpaces (s);
              for (b = s; isVarLetter (*b); b++);
              u = findLabel (s, static_cast<int> (b - s));
              if (u >= 0)
                {
                  *number_stack++ = a;
                  SETM (a, u);
                  s = b;
                  t = 0;
                }
            }
        }
      while (t >= 8 && t <= 9 || t >= 400 && t < 450);

      // Constant function
      if (t == CMDCONST)
        doOp ();
      if (t > 1)
        {
          if (op_stack.len > 0
           && op_stack[op_stack.len - 1].op->mfunc == RETM
           && (t == CMDEND || t == CMDRIGHT))
            cerrno = 1102;
          else
            cerror (err::number_variable_or_prefix_function_expected);
        }

    l2:
      // Postfix operators
      t = token (s, false, true);
      if (t == CMDPOSTFIX)
        {
          if (op_stack.len != 0
           && op_stack[op_stack.len - 1].op->mfunc == INDEXM)
            arrayIndex (s);
          else
            doOp ();
          goto l2;
        }
      // Binary operators
      if (t < 3 || t >= 8 && t <= 9 || t >= CMDVARARG || t >= 400 && t < 450)
        cerror (err::binary_operator_expected);
      if (cerrno != 0)
        break;
      stk.op = 0;
      stk.input_ptr = 0;
      if (t != CMDEND)
        stk = *op_stack--;
      while (op_stack.len > stackBeg && cerrno == 0)
        {
          u = op_stack[op_stack.len - 1].op->type;
          if (t < u || t == CMDASSIGN && u == CMDASSIGN)
            break;
          doOp ();
          if (u == CMDLEFT && t == CMDRIGHT)
            {
              // Prefix operators before parenthesis
              if (op_stack.len > stackBeg && cerrno == 0)
                {
                  u = op_stack[op_stack.len - 1].op->type;
                  if (u >= 8 && u <= 9)
                    doOp ();
                }
              goto l2;
            }
        }
      if (stk.op == nullptr)
        {
          if (e != nullptr)
            *e = s;
          break;
        }
      if (t == CMDRIGHT)
        {
          if (e != nullptr)
            *e = stk.input_ptr;
          break;
        }
      *op_stack++ = stk;
    }
  in_parenthesis = in_par;
}

//---------------------------------------------------------------------------
void
wrAns ()
{
  if (cerrno != 0)
    return;
  char *buf = AWRITEM (ans, digits, matrixFormat);
  printf ("%s", buf);   // TODO: SetWindowText(hOut, buf);
  delete[] buf;
}
//---------------------------------------------------------------------------

void
initLabels (const char *s)
{
  char c;
  bool quot = false;
  int n = 0;
  const char *t;
  label_t *a;

  gotoPositions.reset ();
  *gotoPositions++ = s;
  labels.reset ();

  for (;;)
    {
      skipSpaces (s);
      // Label
      for (t = s; isVarLetter (*t); ++t)
  ;
      if (*t == ':' && t > s)
        {
          a = labels++;
          a->ind = n;
          a->name = s;
          a->name_len = static_cast<int> (t - s);
        }
      // Find semi-colon
      for (;;)
        {
          c = *s;
          if (c == '\0')
            {
              *gotoPositions++ = s;
              return;
            }
          s++;
          if (c == ';' && !quot)
            {
              *gotoPositions++ = s;
              n++;
              if (*s == 0)
                return;
              break;
            }
          if (c == '"')
            quot = !quot;
        }
    }
}

//---------------------------------------------------------------------------

int
thread_loop (char *param)
{
  time_t time = GetTickCount ();
  char *output, *a, t[16];
  const char *e, *input, *s;
  bool b, isPrint;
  var_t *v;
  Matrix y;
  Tlen i, n;
  uint baseOld;
  Darray<char> buf;

  //  SetDlgItemText (hWin, IDC_TIME, "");
  //  SetWindowText (hOut, "");
  output = 0;
  cleanup ();
  initLabels (param);

  baseOld = baseIn;

  for (;;)
    {
      baseIn = baseOld;
      input = param;
      buf.setLen (1);
      buf[0] = '\0';
      for (i = vars.len - 1;; --i)
        {
          v = &vars[i];
          v->modif = false;
          if (i == 0)
            break;
        }
      if (!isNaN (oldAns.A[0].r))
        assign (ans, oldAns);
      else
        assign (oldAns, ans);

      for (command_number = 0;; command_number++)
        {
          // Command cycle
          // Label
          skipSpaces (input);
          for (s = input; isVarLetter (*s); s++);
          if (*s == ':')
            {
              input = s + 1;
              skipSpaces (input);
            }
          // Print command
          isPrint = (strnicmp (input, "print", 5) == 0);
          if (isPrint)
            {
              input += 5;
              skipSpaces (input);
            }

        lcmd:
          if (*input == '"' && isPrint)
            {
              e = input;
              size_t doubleQuotes = skipString (e);
              ++input;
              n = static_cast<int> (e - input) - doubleQuotes - 1;
              a = (buf += n) - 1;
              if (buf.len > MAX_OUTPUT_SIZE)
                {
                  buf -= n;
                  cerror (err::result_is_too_long);
                }
              else
                {
                  if (doubleQuotes > 1)
                    copyString (a, input);
                  else
                    std::memcpy (a, input, n);
                  a[n] = '\0';
                }
              if (*e != '\0')
                ++e;
              skipSpaces (e);
            }
          else if (*input == '\0' || *input == ';')
            {
              // Empty command
              e = input;
            }
          else
            {
              gotoPos = -1;
              retValue.A[0].r = retValue.A[0].i = 0;
              in_parenthesis = 0;
              parse (input, &e);
              if (number_stack.len != 1)
                cerror (err::fatal_error);
              if (cerrno == 0)
                {
                  if (*e == ',' && !isPrint)
                    cerror (err::unmatched_comma);
                  if (*e == ']')
                    cerror (err::unmatched_right_bracket);
                  if (*e == ')')
                    cerror (err::unmatched_parenthesis);
                }
              if (gotoPos >= 0)
                {
                  cleanup ();
                  if (gotoPos >= gotoPositions.len)
                    errGoto ();
                  else
                    {
                      input = gotoPositions[gotoPos];
                      command_number = gotoPos - 1;
                      continue;
                    }
                }
              if (cerrno == 1102)
                {
                  cleanup ();
                  y = retValue;
                  e = strchr (input, 0);
                }
              else
                {
                  if (cerrno != 0)
                    {
                      // TODO
                      // PostMessage (hWin, WM_INPUTCUR, 0, static_cast<int> (errPos - param));
                      break;
                    }
                  y = *number_stack--;
                }
              deref (y);
              if (cerrno != 0)
                {
                  FREEM (y);
                  break;
                }
              if (*e != ';' || isPrint)
                {
                  // Convert the result to a string
                  n = buf.len;
                  a = (buf += LENM (y, digits)) - 1;
                  if (buf.len > MAX_OUTPUT_SIZE)
                    {
                      buf.len = n;
                      cerror (err::result_is_too_long);
                    }
                  else
                    {
                      WRITEM (a, y, digits, matrixFormat);
                      buf.setLen (n + strlen (a));
                    }
                }
              // Set Ans
              FREEM (ans);
              ans = y;
            }
          if (cerrno != 0)
            break;
          input = e + 1;
          if (isPrint)
            {
              if (*e == ',')
                {
                  skipSpaces (input);
                  if (*input != ';' && *input != '\0')
                    {
                      // Space
                      a = (buf++);
                      a[-1] = ' ';
                      *a = '\0';
                      goto lcmd;
                    }
                }
              else
                {
                  // New line
                  a = (buf += 2);
                  a[-1] = '\n';
                  *a = '\0';
                }
            }
          if (*e == '\0')
            break;
        }

    lout:
      if (cerrno != 0)
        break;
      // Compare with the previous result
      b = output != nullptr && strcmp (output, buf) == 0;
      delete[] output;
      output = buf.array;
      buf.array = new char[buf.capacity];
      if (b)
        break;
      // TODO  SetWindowText (hOut, output);
    }
  if (cerrno == 0 || cerrno == 1100)
    {
      // Display calculation time
      sprintf (t, "%zu ms", GetTickCount () - time);
      //TODO SetDlgItemText (hWin, IDC_TIME, t);
      // Save to log file
      if (writeLog && *fnLog != '\0' && output != nullptr)
        {
          FILE *f = fopen (fnLog, "w");
          fseek (f, 0L, SEEK_END);
          saveResult (f, "%1\n = \n%2\n--------------------\n",
                      /* format string is in saveResult, too */ param, output, true);
          fclose (f);
        }
    }
  else
    {
      for (i = vars.len - 1;; --i)
        {
          vars[i].modif = false;
          if (i == 0)
            break;
        }
    }

  delete[] output;
  delete[] param;
  return 0;
}

//---------------------------------------------------------------
int
stop ()
{
  //TODO
  /*  static int lock = 0;

  if (thread)
    {
      if (lock)
        {
          return 1;
        }
      ++lock;
      // Abort previous calculation
      cerrno.store (1100);
      while (MsgWaitForMultipleObjects (1, &thread, FALSE, INFINITE, QS_ALLINPUT) == WAIT_OBJECT_0 + 1)
        {
          MSG mesg;
          while (PeekMessage (&mesg, NULL, 0, 0, PM_REMOVE))
            {
              if (mesg.message == WM_CLOSE)
                {
                  PostMessage (hWin, WM_CLOSE, 0, 0);
                  return 2;
                }
              processMessage (mesg);
            }
        }
      CloseHandle (thread);
      thread = 0;
      --lock;
      }*/
  return 0;
}

void
calc (char *input)
{
  if (stop ())
    return;
  // Get the current base and precision
  //TODO base = GetDlgItemInt (hWin, IDC_BASE, 0, FALSE);
  if (base < 2 || base > 36)
    {
      base = base < 2 ? 2 : 36;
      //TODO SetDlgItemInt (hWin, IDC_BASE, base, FALSE);
    }
  baseIn = base;
  //TODO digits = GetDlgItemInt (hWin, IDC_PRECISION, 0, FALSE);
  //TODO fixDigits = GetDlgItemInt (hWin, IDC_FIXDIGITS, 0, FALSE);
  cerrno = 0;
  // Start a working thread
  std::thread thread (thread_loop, input);
}

__deprecated
char *
getIn (void)
{
  return "pi * 2 / tau";
}

void
calc ()
{
  // Copy content of the input edit box
  char *input = getIn ();
  // Add to history
  for_list (str_item_t, history)
    {
      if (!strcmp (input, item->s))
        {
          delete item;
          break;
        }
    }
  history.append (new str_item_t (input));
  current_history = 0;
  amin (max_history, 2u);
  while (history_length > max_history)
    delete history.remove ();
  // Calculate
  calc (input);
}

void
next_ans ()
{
  if (stop ())
    return;
  COPYM (old_ans, ans);
  for (size_t i = vars.len - 1;; --i)
    {
      var_t *v = &vars[i];
      if (v->modif)
        {
          swap (v->newx, v->oldx);
          v->modif = false;
        }
      if (i == 0)
        break;
    }
}

static int
compare_op (const void *a, const void *b)
{
  return stricmp ((*reinterpret_cast<op_t **> (const_cast<void *> (a)))->name,
                  (*reinterpret_cast<op_t **> (const_cast<void *> (b)))->name);
}

void
init_function_tab ()
{
  function_tab_sorted = new const op_t *[countof (function_tab)];

  for (size_t i = 0; i < countof (function_tab); ++i)
    function_tab_sorted[i] = &function_tab[i];

  qsort (function_tab_sorted, countof (function_tab), sizeof (op_t *), compare_op);
}

//---------------------------------------------------------------
