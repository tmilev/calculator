#include "math_basics.h"
#include <math.h>

double FloatingPoint::sqrtFloating(double argument) {
  return ::sqrt(argument);
}

double FloatingPoint::sinFloating(double argument) {
  return ::sin(argument);
}

std::string FloatingPoint::doubleToString(double input) {
  return FloatingPoint::doubleToString(input, 8);
}

std::string FloatingPoint::doubleToString(double input, int precision) {
  std::stringstream out;
  out.precision(precision);
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
  for (
    firstNonZeroIndex = static_cast<signed>(result.size()) - 1;
    firstNonZeroIndex >= 0; firstNonZeroIndex --
  ) {
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

double FloatingPoint::cosFloating(double argument) {
  return cos(argument);
}

double FloatingPoint::absFloating(double argument) {
  return argument >= 0 ? argument : - argument;
}

bool FloatingPoint::isNaN(const double& argument) {
#ifdef MACRO_use_wasm
  return isnan(argument);
#else
  return std::isnan(argument);
#endif
}

double FloatingPoint::logFloating(double argument) {
  return log(argument);
}

double FloatingPoint::arctan(double argument) {
  return atan(argument);
}

double FloatingPoint::arctan2(double y, double x) {
  return atan2(y, x);
}

double FloatingPoint::arccos(double argument) {
  return acos(argument);
}

double FloatingPoint::arcsin(double argument) {
  return asin(argument);
}

double FloatingPoint::floorFloating(double argument) {
  return floor(argument);
}

double FloatingPoint::round(double argument) {
  return std::round(argument);
}

double FloatingPoint::power(double base, double exponent) {
  return pow(base, exponent);
}

template < >
unsigned int HashFunctions::hashFunction(const double& input) {
  if (FloatingPoint::isNaN(input)) {
    return 5;
  }
  return static_cast<unsigned>(input* 10000);
}

unsigned int MathRoutines::hashDouble(const double& input) {
  return HashFunctions::hashFunction(input);
}
