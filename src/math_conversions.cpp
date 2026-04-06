#include "math_conversions.h"
#include "math_extra_algebraic_numbers.h"

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

bool ConversionFunctions::convertMatrixAlgebraicToMatrixRational(
  Matrix<AlgebraicNumber>& input,
  Matrix<Rational>& output,
  std::stringstream* commentsOnFailure
) {
  Rational converter;
  output.resize(input.numberOfRows, input.numberOfColumns, false, nullptr);
  for (int i = 0; i < input.numberOfRows; i ++) {
    for (int j = 0; j < input.numberOfColumns; j ++) {
      if (!input(i, j).isRational(&converter)) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << "Failed to convert "
          << input(i, j).toString()
          << " to rational number. ";
        }
        return false;
      }
      output(i, j) = converter;
    }
  }
  return true;
}
