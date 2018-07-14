//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader11_ImplementationHeaderEllipticCurves
#define vpfHeader11_ImplementationHeaderEllipticCurves
#include "vpfHeader2Math11_EllipticCurves.h"

static ProjectInformationInstance ProjectInfovpfImplementationHeader_HeaderEllipticCurves(__FILE__, "Implementation header, elliptic curves. ");

template <typename coefficient>
unsigned int ElementEllipticCurve<coefficient>::HashFunction(const ElementEllipticCurve<coefficient>& input)
{ if (input.flagInfinity)
    return 0;
  return input.xCoordinate.HashFunction() * SomeRandomPrimes[0] +
  input.yCoordinate.HashFunction() * SomeRandomPrimes[1] +
  input.owner.HashFunction(input.owner) * SomeRandomPrimes[2];
}

template <typename coefficient>
bool ElementEllipticCurve<coefficient>::operator==(const ElementEllipticCurve& other) const
{ if (!(this->owner == other.owner))
    return false;
  return this->xCoordinate == other.xCoordinate &&
  this->yCoordinate == other.yCoordinate &&
  this->flagInfinity == other.flagInfinity;
}

template <typename coefficient>
std::string ElementEllipticCurve<coefficient>::ToString(FormatExpressions* theFormat) const
{ std::stringstream out;
  Polynomial<Rational> leftHandSide, rightHandSide;
  leftHandSide.MakeMonomiaL(1, 2, 1);
  rightHandSide.MakeMonomiaL(0, 3, 1);
  rightHandSide.AddMonomial(MonomialP(0, 1), this->owner.linearCoefficient);
  rightHandSide += (Rational) this->owner.constantTerm;
  out << "ElementEllipticCurveNormalForm{}(" << leftHandSide.ToString(theFormat)
  << " = " << rightHandSide.ToString(theFormat) << ", "
  << MonomialP(0, 1).ToString(theFormat) << " = " << this->xCoordinate.ToString()
  << ", " << MonomialP(1, 1).ToString(theFormat) << " = " << this->yCoordinate.ToString()
  << ") ";
  return out.str();
}

#endif
