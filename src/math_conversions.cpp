#include "math_conversions.h"

bool ConverterOfRationalToZModP::convert(
  const Rational& input, ElementZmodP& output
) {
  output.modulus = this->modulus;
  return output.assignRational(input);
}

ConverterOfRationalToZModP::ConverterOfRationalToZModP(
  const LargeIntegerUnsigned& inputModulus
) {
  this->modulus = inputModulus;
}
