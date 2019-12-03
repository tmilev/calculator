// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader11_HeaderEllipticCurves
#define vpfHeader11_HeaderEllipticCurves
#include "math_general.h"

static ProjectInformationInstance ProjectInfovpfHeader_HeaderEllipticCurves(__FILE__, "Header, elliptic curves. ");

class EllipticCurveWeierstrassNormalForm {
public:
  LargeInteger linearCoefficient;
  LargeInteger constantTerm;
  std::string name;
  LargeIntegerUnsigned order;
  LargeIntegerUnsigned modulo;
  void MakeSecp256k1();
  static bool GetOrderNISTCurve(
    const std::string& curveName, LargeIntegerUnsigned& output, std::stringstream *commentsOnFailure
  );
  static unsigned int HashFunction(const EllipticCurveWeierstrassNormalForm& input);
  bool operator==(const EllipticCurveWeierstrassNormalForm& other) const;
};

template <typename coefficient>
class ElementEllipticCurve {
public:
  EllipticCurveWeierstrassNormalForm owner;
  coefficient xCoordinate;
  coefficient yCoordinate;
  bool flagInfinity;
  static unsigned int HashFunction(const ElementEllipticCurve<coefficient>& input);
  void Invert();
  bool MakeGeneratorNISTCurve(const std::string& input, std::stringstream* commentsOnFailure);
  void MakeGeneratorSecp256k1();
  void MakeOne(const EllipticCurveWeierstrassNormalForm& inputCurve);
  bool operator==(const ElementEllipticCurve& other) const;
  bool operator*=(const ElementEllipticCurve& other);
  std::string ToString(FormatExpressions* theFormat = nullptr) const;
  ElementEllipticCurve() {
    this->flagInfinity = true;
  }
};
#endif
