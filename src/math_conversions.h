#ifndef header_math_conversions_ALREADY_INCLUDED
#define header_math_conversions_ALREADY_INCLUDED

#include "math_general.h"
#include "math_modular_arithmetic.h"

class ConverterOfRationalToZModP: public CoefficientConverter<
  Rational, ElementZmodP
> {
public:
  ConverterOfRationalToZModP(const LargeIntegerUnsigned& inputModulus = 0);
  LargeIntegerUnsigned modulus;
  bool convert(const Rational& input, ElementZmodP& output);
};

#endif // header_math_conversions_ALREADY_INCLUDED
