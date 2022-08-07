#include "math_extra_elliptic_curves_implementation.h"
#include "math_extra_algebraic_numbers.h"

unsigned int EllipticCurveWeierstrassNormalForm::hashFunction(
  const EllipticCurveWeierstrassNormalForm& input
) {
  return
  input.linearCoefficient.hashFunction() * HashConstants::constant0 +
  input.constantTerm.hashFunction() * HashConstants::constant1;
}

bool EllipticCurveWeierstrassNormalForm::getOrderNISTCurve(
  const std::string& curveName,
  LargeIntegerUnsigned& output,
  std::stringstream* commentsOnFailure
) {
  if (curveName == "secp256k1") {
    output.assignString(
      "115792089237316195423570985008687907852"
      "837564279074904382605163141518161494337"
    );
    return true;
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure
    << "I have not yet been taught the order of your desired curve: "
    << curveName
    << ". ";
  }
  return false;
}

bool EllipticCurveWeierstrassNormalForm::operator==(
  const EllipticCurveWeierstrassNormalForm& other
) const {
  return
  this->linearCoefficient == other.linearCoefficient &&
  this->constantTerm == other.constantTerm;
}

template < >
void ElementEllipticCurve<ElementZmodP>::makeGeneratorSecp256k1() {
  LargeIntegerUnsigned modulus;
  modulus.assignString(
    "115792089237316195423570985008687907853"
    "269984665640564039457584007908834671663"
  );
  this->xCoordinate.modulus = modulus;
  this->yCoordinate.modulus = modulus;
  this->xCoordinate.value.assignString(
    "550662630222773436695787188951685343262"
    "50603453777594175500187360389116729240"
  );
  this->yCoordinate.value.assignString(
    "326705100207588169780830851305070431844"
    "71273380659243275938904335757337482424"
  );
  this->flagInfinity = false;
  this->owner.constantTerm = 7;
  this->owner.linearCoefficient = 0;
  this->owner.name = "secp256k1";
  this->owner.order.assignString(
    "115792089237316195423570985008687907852"
    "837564279074904382605163141518161494337"
  );
}

template < >
bool ElementEllipticCurve<ElementZmodP>::makeGeneratorNISTCurve(
  const std::string& input, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("ElementEllipticCurve::makeGeneratorNISTCurve");
  if (input == "secp256k1") {
    this->makeGeneratorSecp256k1();
    return true;
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure
    << "I do not recognize the requested curve name. "
    << "The curves I have implemented are: "
    << "secp256k1";
  }
  return false;
}
