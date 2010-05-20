
#include "polyhedra.h"

class WeylAlgebra
{
public:

};

class ElementWeylAlgebra
{
private:
  PolynomialRationalCoeff StandardOrder;
public:
  int NumVariables;
  void MultiplyOnTheLeft(ElementWeylAlgebra& standsOnTheLeft);
  void MultiplyTwoMonomials( Monomial<Rational>& left, Monomial<Rational>& right, PolynomialRationalCoeff& OrderedOutput);
  ElementWeylAlgebra()
  { this->NumVariables=0; };
};

void ElementWeylAlgebra::MultiplyTwoMonomials( Monomial<Rational>& left, Monomial<Rational>& right, PolynomialRationalCoeff& OrderedOutput)
{ Monomial<Rational> buffer;
  buffer.init(left.NumVariables);
  assert(left.NumVariables==right.NumVariables);
  OrderedOutput.Nullify(left.NumVariables);
  for (int i=0; i<left.NumVariables; i++)
  {
  }
}

void ElementWeylAlgebra::MultiplyOnTheLeft(ElementWeylAlgebra& standsOnTheLeft)
{ for (int i=0; i<this->StandardOrder.size; i++)
  {
  }
}

void main_test_function()
{
}
