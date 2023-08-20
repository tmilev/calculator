#ifndef header_math_extra_elliptic_curves_ALREADY_INCLUDED
#define header_math_extra_elliptic_curves_ALREADY_INCLUDED

#include "math_general.h"

class EllipticCurveWeierstrassNormalForm {
public:
  LargeInteger linearCoefficient;
  LargeInteger constantTerm;
  std::string name;
  LargeIntegerUnsigned order;
  LargeIntegerUnsigned modulo;
  static bool getOrderNISTCurve(
    const std::string& curveName,
    LargeIntegerUnsigned& output,
    std::stringstream* commentsOnFailure
  );
  static unsigned int hashFunction(
    const EllipticCurveWeierstrassNormalForm& input
  );
  bool operator==(const EllipticCurveWeierstrassNormalForm& other) const;
};

template <typename Coefficient>
class ElementEllipticCurve {
  friend std::ostream& operator<<(
    std::ostream& output, const ElementEllipticCurve& unused
  ) {
    (void) unused;
    output << "An element of an elliptic curve.";
    return output;
  }
public:
  EllipticCurveWeierstrassNormalForm owner;
  Coefficient xCoordinate;
  Coefficient yCoordinate;
  bool flagInfinity;
  static unsigned int hashFunction(
    const ElementEllipticCurve<Coefficient>& input
  );
  void invert();
  bool makeGeneratorNISTCurve(
    const std::string& input, std::stringstream* commentsOnFailure
  );
  void makeGeneratorSecp256k1();
  void makeOne(const EllipticCurveWeierstrassNormalForm& inputCurve);
  bool operator==(const ElementEllipticCurve& other) const;
  bool operator*=(const ElementEllipticCurve& other);
  std::string toString(FormatExpressions* format = nullptr) const;
  ElementEllipticCurve() {
    this->flagInfinity = true;
  }
};

#endif // header_math_extra_elliptic_curves_ALREADY_INCLUDED
