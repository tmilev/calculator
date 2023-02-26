
#ifndef header_math_basics_ALREADY_INCLUDED
#define header_math_basics_ALREADY_INCLUDED

#include "general_logging_global_variables.h"

class MathRoutines {
public:
  template <class Coefficient>
  static bool invertXModN(
    const Coefficient& x, const Coefficient& n, Coefficient& output
  ) {
    Coefficient quotient;
    Coefficient remainder;
    Coefficient dividend;
    Coefficient divisor;
    Coefficient dividendN;
    Coefficient dividendX;
    Coefficient divisorN;
    Coefficient divisorX;
    Coefficient swapX;
    Coefficient swapN;
    if (x < 0 || n < 0) {
      fatalCrash("Negative inputs to invertXModN");
    }
    // At any given moment, dividend = dividendN * N + dividendX * X.
    dividend = n;
    dividendN = 1;
    dividendX = 0;
    // At any given moment, divisor = divisorN * N + divisorX * X.
    divisor = x;
    divisorN = 0;
    divisorX = 1;
    while (divisor > 0) {
      quotient = dividend / divisor;
      remainder = dividend % divisor;
      dividend = divisor;
      divisor = remainder;
      swapX = dividendX;
      swapN = dividendN;
      dividendX = divisorX;
      dividendN = divisorN;
      divisorX = swapX - quotient * divisorX;
      divisorN = swapN - quotient * divisorN;
    }
    if (dividend != 1) {
      // d and p were not relatively prime.
      return false;
    }
    output = dividendX % n;
    if (output < 0) {
      output += n;
    }
    return true;
  }
  static int leastCommonMultiple(int a, int b);
  template <typename Integral>
  static Integral greatestCommonDivisor(Integral left, Integral right) {
    Integral temp;
    while (!(right == 0)) {
      temp = left % right;
      left = right;
      right = temp;
    }
    return left;
  }
  template <typename Integral>
  static Integral leastCommonMultiple(Integral a, Integral b) {
    Integral result;
    result = a;
    result /= MathRoutines::greatestCommonDivisor(a, b);
    result *= b;
    return result;
  }
  static int twoToTheNth(int n);
  static bool isLatinLetter(char input);
  static bool isDigit(const std::string& input, int* whichDigit = nullptr);
  static bool isDigit(char input, int* whichDigit = nullptr);
  static bool hasDecimalDigitsOnly(const std::string& input);
  static bool isHexDigit(char digitCandidate);
  template <class Type>
  static bool generateVectorSpaceClosedWithRespectToLieBracket(
    List<Type>& inputOutputElements, int upperDimensionBound
  ) {
    return
    MathRoutines::generateVectorSpaceClosedWithRespectToOperation(
      inputOutputElements, upperDimensionBound, Type::lieBracket
    );
  }
  static Vector<double> getVectorDouble(Vector<Rational>& input);
  template <class Type>
  static bool generateVectorSpaceClosedWithRespectToOperation(
    List<Type>& inputOutputElts,
    int upperDimensionBound,
    void(*binaryOperation)(
      const Type& left, const Type& right, Type& output
    )
  );
  static char convertHumanReadableHexToCharValue(char input);
  static void nChooseK(int n, int k, LargeInteger& result);
  static LargeInteger nChooseK(int n, int k);
  static int factorial(int n);
  static inline double E() {
    return 2.718281828459;
  }
  static inline double pi() {
    return 3.141592653589793238462643383279;
  }
  // The MS compiler refuses to compile the following, hence the above line.
  // static const double Pi = (double)3.141592653589793238462643383279;
  static int kToTheNth(int k, int n);
  static void kToTheNth(int k, int n, LargeInteger& output);
  inline static int parity(int n);
  static int binomialCoefficientMultivariate(int n, List<int>& choices);
  static bool isPrimeSimple(int input);
  template <class Coefficient, typename IntegerType>
  static void raiseToPower(
    Coefficient& element,
    const IntegerType& power,
    const Coefficient& ringUnit
  );
  template <typename Coefficient>
  inline static Coefficient maximum(
    const Coefficient& a, const Coefficient& b
  ) {
    if (a > b) {
      return a;
    } else {
      return b;
    }
  }
  template <typename T>
  inline static void swap(T& a, T& b) {
    if (&a == &b) {
      return;
    }
    T temp;
    temp = a;
    a = b;
    b = temp;
  }
  template <class Element>
  inline static Element minimum(const Element& a, const Element& b) {
    if (a > b) {
      return b;
    } else {
      return a;
    }
  }
  template <class Element>
  static std::string toStringBrackets(const Element& input) {
    if (!input.needsParenthesisForMultiplication()) {
      return input.toString();
    }
    std::string result;
    result.append("(");
    result.append(input.toString());
    result.append(")");
    return result;
  }
  template <class Element>
  static std::string toStringBrackets(
    const Element& input, FormatExpressions* format
  ) {
    if (!input.needsParenthesisForMultiplication()) {
      return input.toString(format);
    }
    std::string result;
    result.append("(");
    result.append(input.toString(format));
    result.append(")");
    return result;
  }
  static double reducePrecision(double x);
  static unsigned int hashDouble(const double& input);
  static unsigned int hashVectorDoubles(const Vector<double>& input);
  static unsigned int hashListDoubles(const List<double>& input);
  static unsigned int hashListInts(const List<int>& input);
  static unsigned int hashListStrings(const List<std::string>& input);
  template <class Element>
  static void lieBracket(
    const Element& standsOnTheLeft,
    const Element& standsOnTheRight,
    Element& output
  );
  template <typename number>
  static number complexConjugate(number x) {
    return x.getComplexConjugate();
  }
  static int complexConjugate(int x) {
    return x;
  }
  static double complexConjugate(double x) {
    return x;
  }
  static bool isInteger(Rational x);
  static bool parseListIntegers(
    const std::string& input,
    List<int>& result,
    std::stringstream* commentsOnFailure
  );
  static void parseListIntegersNoFailure(
    const std::string& input, List<int>& result
  );
};

// We are wrapping the math.h c++ functions for portability reasons
// (if for some reason we want to change from math.h to a better floating point
// library, we only need to change the implementation of the FloatingPoint
// class.
// Due to conflicts with namespaces, we use function names different from the
// built-in ones.
class FloatingPoint {
public:
  static double absFloating(double argument);
  static double sinFloating(double argument);
  static double cosFloating(double argument);
  static double arccos(double argument);
  static double arctan(double argument);
  static double arcsin(double argument);
  static double sqrtFloating(double argument);
  static double power(double base, double exponent);
  static double logFloating(double argument);
  static bool isNaN(const double& argument);
  static double floorFloating(double argument);
  static std::string doubleToString(double input);
  static std::string doubleToString(double input, int precision);
};

#endif // header_math_basics_ALREADY_INCLUDED
