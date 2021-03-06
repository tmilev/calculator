// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader11_HeaderEllipticCurves
#define vpfHeader11_HeaderEllipticCurves
#include "math_general.h"

class EllipticCurveWeierstrassNormalForm {
public:
  LargeInteger linearCoefficient;
  LargeInteger constantTerm;
  std::string name;
  LargeIntegerUnsigned order;
  LargeIntegerUnsigned modulo;
  static bool getOrderNISTCurve(
    const std::string& curveName, LargeIntegerUnsigned& output, std::stringstream *commentsOnFailure
  );
  static unsigned int hashFunction(const EllipticCurveWeierstrassNormalForm& input);
  bool operator==(const EllipticCurveWeierstrassNormalForm& other) const;
};

template <typename Coefficient>
class ElementEllipticCurve {
public:
  EllipticCurveWeierstrassNormalForm owner;
  Coefficient xCoordinate;
  Coefficient yCoordinate;
  bool flagInfinity;
  static unsigned int hashFunction(const ElementEllipticCurve<Coefficient>& input);
  void invert();
  bool makeGeneratorNISTCurve(const std::string& input, std::stringstream* commentsOnFailure);
  void makeGeneratorSecp256k1();
  void makeOne(const EllipticCurveWeierstrassNormalForm& inputCurve);
  bool operator==(const ElementEllipticCurve& other) const;
  bool operator*=(const ElementEllipticCurve& other);
  std::string toString(FormatExpressions* format = nullptr) const;
  ElementEllipticCurve() {
    this->flagInfinity = true;
  }
};
#endif
