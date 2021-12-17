#include "math_extra_elliptic_curves_implementation.h"
#include "math_extra_algebraic_numbers.h"

unsigned int EllipticCurveWeierstrassNormalForm::hashFunction(const EllipticCurveWeierstrassNormalForm& input) {
  return
  input.linearCoefficient.hashFunction() * HashConstants::constant0 +
  input.constantTerm.hashFunction() * HashConstants::constant1;
}

bool EllipticCurveWeierstrassNormalForm::getOrderNISTCurve(
  const std::string& curveName, LargeIntegerUnsigned &output, std::stringstream* commentsOnFailure
) {
  if (curveName == "secp256k1") {
    output.assignString("115792089237316195423570985008687907852837564279074904382605163141518161494337");
    return true;
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "I have not yet been taught the order of your desired curve: " << curveName << ". ";
  }
  return false;
}

bool EllipticCurveWeierstrassNormalForm::operator==(const EllipticCurveWeierstrassNormalForm& other) const {
  return this->linearCoefficient == other.linearCoefficient &&
  this->constantTerm == other.constantTerm;
}

template < >
void ElementEllipticCurve<ElementZmodP>::makeGeneratorSecp256k1() {
  LargeIntegerUnsigned theModulo;
  theModulo.assignString  ("115792089237316195423570985008687907853269984665640564039457584007908834671663");
  this->xCoordinate.modulus = theModulo;
  this->yCoordinate.modulus = theModulo;
  this->xCoordinate.value.assignString("55066263022277343669578718895168534326250603453777594175500187360389116729240");
  this->yCoordinate.value.assignString("32670510020758816978083085130507043184471273380659243275938904335757337482424");
  this->flagInfinity = false;
  this->owner.constantTerm = 7;
  this->owner.linearCoefficient = 0;
  this->owner.name = "secp256k1";
  this->owner.order.assignString("115792089237316195423570985008687907852837564279074904382605163141518161494337");
}

template < >
bool ElementEllipticCurve<ElementZmodP>::makeGeneratorNISTCurve(
  const std::string& input, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("ElementEllipticCurve::makeGeneratorNISTCurve");
  if (input == "secp256k1") {
    this->makeGeneratorSecp256k1();
    return true;
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure
    << "I do not recognize the requested curve name. The curves I have implemented are: "
    << "secp256k1";
  }
  return false;
}
