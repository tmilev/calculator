//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".

#include "math_1_general.h"
#include <math.h>
ProjectInformationInstance projectInfoInstanceFloatingPoint(__FILE__, "Built in c++ floating point routines wrapper.");

double FloatingPoint::sqrt(double argument) {
  return ::sqrt(argument);
}

double FloatingPoint::sin(double argument) {
  return ::sin(argument);
}

std::string FloatingPoint::DoubleToString(double input) {
  std::stringstream out;
  out.precision(8);
  out << std::fixed << input;
  std::string result = out.str();
  bool hasDot = false;
  for (int i = result.size() - 1; i >= 0; i --) {
    if (result[i] == '.') {
      hasDot = true;
      break;
    }
  }
  if (!hasDot) {
    return result;
  }
  int firstNonZeroIndex = 0;
  for (firstNonZeroIndex = result.size() - 1; firstNonZeroIndex >= 0; firstNonZeroIndex --) {
    if (result[firstNonZeroIndex] == '.') {
      result.resize(firstNonZeroIndex);
      return result;
    }
    if (result[firstNonZeroIndex] != '0') {
      break;
    }
  }
  result.resize(firstNonZeroIndex + 1);
  return result;
}

double FloatingPoint::cos(double argument) {
  return std::cos(argument);
}

double FloatingPoint::abs(double argument) {
  return argument >= 0 ? argument : - argument;
}

double FloatingPoint::log(double argument) {
  return std::log(argument);
}

double FloatingPoint::arctan(double argument) {
  return std::atan(argument);
}

double FloatingPoint::arccos(double argument) {
  return std::acos(argument);
}

double FloatingPoint::arcsin(double argument) {
  return std::asin(argument);
}

double FloatingPoint::floor(double argument) {
  return std::floor(argument);
}

double FloatingPoint::power(double base, double exponent) {
  return std::pow(base, exponent);
}
