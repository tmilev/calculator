// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".

#include "math_general.h"
#include <math.h>

double FloatingPoint::Sqrt(double argument) {
  return ::sqrt(argument);
}

double FloatingPoint::Sin(double argument) {
  return ::sin(argument);
}

std::string FloatingPoint::DoubleToString(double input) {
  std::stringstream out;
  out.precision(8);
  out << std::fixed << input;
  std::string result = out.str();
  bool hasDot = false;
  for (int i = static_cast<signed>(result.size()) - 1; i >= 0; i --) {
    if (result[static_cast<unsigned>(i)] == '.') {
      hasDot = true;
      break;
    }
  }
  if (!hasDot) {
    return result;
  }
  int firstNonZeroIndex = 0;
  for (firstNonZeroIndex = static_cast<signed>(result.size()) - 1; firstNonZeroIndex >= 0; firstNonZeroIndex --) {
    if (result[static_cast<unsigned>(firstNonZeroIndex)] == '.') {
      result.resize(static_cast<unsigned>(firstNonZeroIndex));
      return result;
    }
    if (result[static_cast<unsigned>(firstNonZeroIndex)] != '0') {
      break;
    }
  }
  result.resize(static_cast<unsigned>(firstNonZeroIndex) + 1);
  return result;
}

double FloatingPoint::Cos(double argument) {
  return cos(argument);
}

double FloatingPoint::Abs(double argument) {
  return argument >= 0 ? argument : - argument;
}

double FloatingPoint::Log(double argument) {
  return log(argument);
}

double FloatingPoint::Arctan(double argument) {
  return atan(argument);
}

double FloatingPoint::Arccos(double argument) {
  return acos(argument);
}

double FloatingPoint::Arcsin(double argument) {
  return asin(argument);
}

double FloatingPoint::Floor(double argument) {
  return floor(argument);
}

double FloatingPoint::Power(double base, double exponent) {
  return pow(base, exponent);
}
