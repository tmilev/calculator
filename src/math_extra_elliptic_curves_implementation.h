// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader11_ImplementationHeaderEllipticCurves
#define vpfHeader11_ImplementationHeaderEllipticCurves
#include "math_extra_elliptic_curves.h"
#include "math_extra_algebraic_numbers.h"

template <typename coefficient>
unsigned int ElementEllipticCurve<coefficient>::HashFunction(const ElementEllipticCurve<coefficient>& input) {
  if (input.flagInfinity) {
    return 0;
  }
  return input.xCoordinate.HashFunction() * SomeRandomPrimes[0] +
  input.yCoordinate.HashFunction() * SomeRandomPrimes[1] +
  input.owner.HashFunction(input.owner) * SomeRandomPrimes[2];
}

template <typename coefficient>
bool ElementEllipticCurve<coefficient>::operator==(const ElementEllipticCurve& other) const {
  if (!(this->owner == other.owner)) {
    return false;
  }
  if (this->flagInfinity == true && other.flagInfinity == true) {
    return true;
  }
  return this->xCoordinate == other.xCoordinate &&
  this->yCoordinate == other.yCoordinate &&
  this->flagInfinity == other.flagInfinity;
}

template <typename coefficient>
void ElementEllipticCurve<coefficient>::Invert() {
  if (this->flagInfinity) {
    return;
  }
  this->yCoordinate *= - 1;
}

template <typename coefficient>
void ElementEllipticCurve<coefficient>::MakeOne(const EllipticCurveWeierstrassNormalForm& inputCurve) {
  this->owner = inputCurve;
  this->flagInfinity = true;
}

template <typename coefficient>
bool ElementEllipticCurve<coefficient>::operator*=(const ElementEllipticCurve& other) {
  if (!(this->owner == other.owner)) {
    return false;
  }
  if (this->flagInfinity) {
    *this = other;
    return true;
  }
  if (other.flagInfinity) {
    return true;
  }
  // We use the formulas:
  // s = slope will be determined later
  // (the slope of secant line (two points) or tangent line (double point).
  // x_C = s^2 - x_A - x_B
  // y_C = -s(x_C - x_A)-y_A
  coefficient slope;
  if (this->xCoordinate == other.xCoordinate) {
    if (this->yCoordinate == other.yCoordinate * (- 1)) {
      this->flagInfinity = true;
      this->xCoordinate = 0;
      this->yCoordinate = 0;
      return true;
    }
    //at this point of code, the two points must be equal.
    //We have that:
    //slope = (3x^2 - linearCoefficient)/(2y)
    slope = (3 * this->xCoordinate * this->xCoordinate + this->owner.linearCoefficient) / (2 * this->yCoordinate);
  } else {
    slope = (other.yCoordinate - this->yCoordinate) / (other.xCoordinate - this->xCoordinate);
  }
  coefficient originalX = this->xCoordinate;
  this->xCoordinate = slope * slope - this->xCoordinate - other.xCoordinate;
  this->yCoordinate = slope * (this->xCoordinate - originalX) * (- 1) - this->yCoordinate;
  this->flagInfinity = false;
  return true;
}

template <typename coefficient>
std::string ElementEllipticCurve<coefficient>::toString(FormatExpressions* theFormat) const {
  std::stringstream out;
  Polynomial<Rational> leftHandSide, rightHandSide;
  leftHandSide.MakeMonomiaL(1, 2, 1);
  rightHandSide.MakeMonomiaL(0, 3, 1);
  rightHandSide.AddMonomial(MonomialP(0, 1), this->owner.linearCoefficient);
  rightHandSide += Rational(this->owner.constantTerm);
  out << "ElementEllipticCurveNormalForm{}(" << leftHandSide.toString(theFormat)
  << " = " << rightHandSide.toString(theFormat) << ", "
  << MonomialP(0, 1).toString(theFormat) << " = " << this->xCoordinate.toString()
  << ", " << MonomialP(1, 1).toString(theFormat) << " = " << this->yCoordinate.toString()
  << ") ";
  if (this->flagInfinity) {
    out << " Infinity. ";
  }
  return out.str();
}
#endif
