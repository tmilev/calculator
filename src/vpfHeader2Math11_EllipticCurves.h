//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader11_HeaderEllipticCurves
#define vpfHeader11_HeaderEllipticCurves
#include "vpfHeader2Math0_General.h"

static ProjectInformationInstance ProjectInfovpfHeader_HeaderEllipticCurves(__FILE__, "Header, elliptic curves. ");

class EllipticCurveWeierstrassNormalForm
{
public:
  LargeInt linearCoefficient;
  LargeInt constantTerm;
  void MakeSecp256k1();
  static unsigned int HashFunction(const EllipticCurveWeierstrassNormalForm& input);
  bool operator==(const EllipticCurveWeierstrassNormalForm& other) const;
};

template <typename coefficient>
class ElementEllipticCurve
{
public:
  EllipticCurveWeierstrassNormalForm owner;
  coefficient xCoordinate;
  coefficient yCoordinate;
  bool flagInfinity;
  static unsigned int HashFunction(const ElementEllipticCurve<coefficient>& input);
  void Invert();
  void MakeOne(const EllipticCurveWeierstrassNormalForm& inputCurve);
  bool operator==(const ElementEllipticCurve& other) const;
  bool operator*=(const ElementEllipticCurve& other);
  std::string ToString(FormatExpressions* theFormat = 0) const;
  ElementEllipticCurve();
};
#endif
