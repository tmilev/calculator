//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader11_HeaderEllipticCurves
#define vpfHeader11_HeaderEllipticCurves
#include "vpfHeader2Math0_General.h"

static ProjectInformationInstance ProjectInfovpfHeader_HeaderEllipticCurves(__FILE__, "Header, elliptic curves. ");

template <typename coefficient>
class EllipticCurveWeierstrassNormalForm
{

public:
  coefficient linearCoefficient;
  coefficient constantTerm;
  List<coefficient> generatorsX;
  List<coefficient> generatorsY;
  void MakeSecp256k1();
  static unsigned int HashFunction(const EllipticCurveWeierstrassNormalForm<coefficient>& input)
  { return
    input.linearCoefficient.HashFunction() * SomeRandomPrimes[0] +
    input.constantTerm.HashFunction() * SomeRandomPrimes[1] +
    input.generatorsX.HashFunction() * SomeRandomPrimes[2] +
    input.generatorsY.HashFunction() * SomeRandomPrimes[3];
  }
  bool operator==(const EllipticCurveWeierstrassNormalForm& other)
  { return this->linearCoefficient == other.linearCoefficient &&
    this->constantTerm == other.constantTerm &&
    this->generatorsX == other.generatorsX &&
    this->generatorsY == other.generatorsY;
  }
};

template <typename coefficient>
class ElementEllipticCurve
{
public:
  EllipticCurveWeierstrassNormalForm<coefficient>* owner;
  coefficient xCoordinate;
  coefficient yCoordinate;
  bool isInfinity;
  static unsigned int HashFunction(const ElementEllipticCurve<coefficient>& input)
  { if (input.isInfinity)
      return 0;
    return input.xCoordinate.HashFunction() * SomeRandomPrimes[0] +
    input.yCoordinate.HashFunction() * SomeRandomPrimes[1];
  }

};
#endif
