#include "vpfImplementationHeader2Math11_EllipticCurves.h"

unsigned int EllipticCurveWeierstrassNormalForm::HashFunction(const EllipticCurveWeierstrassNormalForm& input)
{ return
  input.linearCoefficient.HashFunction() * SomeRandomPrimes[0] +
  input.constantTerm.HashFunction() * SomeRandomPrimes[1] ;
}

bool EllipticCurveWeierstrassNormalForm::operator==(const EllipticCurveWeierstrassNormalForm& other) const
{ return this->linearCoefficient == other.linearCoefficient &&
  this->constantTerm == other.constantTerm;
}
