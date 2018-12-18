#include "vpfImplementationHeader2Math11_EllipticCurves.h"
#include "vpfHeader2Math2_AlgebraicNumbers.h"

unsigned int EllipticCurveWeierstrassNormalForm::HashFunction(const EllipticCurveWeierstrassNormalForm& input)
{ return
  input.linearCoefficient.HashFunction() * SomeRandomPrimes[0] +
  input.constantTerm.HashFunction() * SomeRandomPrimes[1] ;
}

bool EllipticCurveWeierstrassNormalForm::GetOrderNISTCurve(const std::string& curveName, LargeIntUnsigned &output, std::stringstream* commentsOnFailure)
{ if (curveName == "secp256k1")
  { output.AssignString("115792089237316195423570985008687907852837564279074904382605163141518161494337");
    return true;
  }
  if (commentsOnFailure != 0)
    *commentsOnFailure << "I have not yet been taught the order of your desired curve: " << curveName << ". ";
  return false;
}

bool EllipticCurveWeierstrassNormalForm::operator==(const EllipticCurveWeierstrassNormalForm& other) const
{ return this->linearCoefficient == other.linearCoefficient &&
  this->constantTerm == other.constantTerm;
}

template < >
void ElementEllipticCurve<ElementZmodP>::MakeGeneratorSecp256k1()
{
  LargeIntUnsigned theModulo;
  theModulo.AssignString  ("115792089237316195423570985008687907853269984665640564039457584007908834671663");
  this->xCoordinate.theModulo = theModulo;
  this->yCoordinate.theModulo = theModulo;
  this->xCoordinate.theValue.AssignString("55066263022277343669578718895168534326250603453777594175500187360389116729240");
  this->yCoordinate.theValue.AssignString("32670510020758816978083085130507043184471273380659243275938904335757337482424");
  this->flagInfinity = false;
  this->owner.constantTerm = 7;
  this->owner.linearCoefficient = 0;
  this->owner.name = "secp256k1";
  this->owner.order.AssignString("115792089237316195423570985008687907852837564279074904382605163141518161494337");
}

template < >
bool ElementEllipticCurve<ElementZmodP>::MakeGeneratorNISTCurve
(const std::string& input, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("ElementEllipticCurve::MakeGeneratorNISTCurve");
  if (input == "secp256k1")
  { this->MakeGeneratorSecp256k1();
    return true;
  }
  if (commentsOnFailure != 0)
    *commentsOnFailure
    << "I do not recognize the requested curve name. The curves I have implemented are: "
    << "secp256k1";
  return false;
}
