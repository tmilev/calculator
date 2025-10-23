#ifndef header_math_modular_arithmetic_ALREADY_INCLUDED
#define header_math_modular_arithmetic_ALREADY_INCLUDED

#include "math_large_integers.h"

class ElementZmodP {
public:
  friend std::ostream& operator<<(
    std::ostream& output, const ElementZmodP& input
  ) {
    output << input.toString();
    return output;
  }
  static const int maximumModulusForUserFacingPolynomialDivision;
  LargeIntegerUnsigned modulus;
  LargeIntegerUnsigned value;
  bool flagDeallocated;
  friend ElementZmodP operator*(int left, const ElementZmodP& right);
  unsigned int hashFunction() const;
  static unsigned int hashFunction(const ElementZmodP& input) {
    return input.hashFunction();
  }
  ElementZmodP() {
    this->flagDeallocated = false;
  }
  ElementZmodP(const ElementZmodP& other) {
    this->flagDeallocated = false;
    *this = other;
  }
  ~ElementZmodP() {
    this->flagDeallocated = true;
  }
  void checkIamInitialized() const;
  bool invert();
  bool isEqualToZero() const {
    return this->value.isEqualToZero();
  }
  bool isEqualToOne() const {
    return this->value.isEqualToOne();
  }
  // Returns the number by which the vector was multiplied.
  static ElementZmodP scaleNormalizeIndex(
    List<ElementZmodP>& toBeScaled, int indexNonZeroElement
  );
  // Required by RationalFunction when scaling simultaneously
  // numerator and denominator.
  // Returns a copy of the number.
  ElementZmodP getNumerator() const;
  // Required by RationalFunction.
  // Returns one.
  ElementZmodP getDenominator() const;
  void operator=(const ElementZmodP& other);
  void operator=(const LargeIntegerUnsigned& other);
  void operator=(const LargeInteger& other);
  void makeZero();
  void makeOne(const LargeIntegerUnsigned& newModulo);
  void makeMinusOne(const LargeIntegerUnsigned& newModulo);
  void makeFrom(int inputModulus, int inputValue);
  void makeFrom(const LargeIntegerUnsigned& inputModulus, int inputValue);
  void checkEqualModuli(const ElementZmodP& other);
  void negate();
  bool operator!=(const ElementZmodP& other) const;
  bool operator==(int other) const;
  bool operator==(const ElementZmodP& other) const;
  void operator*=(const ElementZmodP& other);
  void operator*=(int other);
  void operator*=(const LargeInteger& other);
  bool operator*=(const Rational& other);
  void operator+=(const ElementZmodP& other);
  void operator-=(const ElementZmodP& other);
  void operator-=(const LargeIntegerUnsigned& other);
  bool operator+=(const Rational& other);
  void operator=(const int other);
  ElementZmodP operator+(const Rational& other) const;
  ElementZmodP operator-(const ElementZmodP& other) const;
  ElementZmodP operator/(const Rational& other) const;
  ElementZmodP operator/(const ElementZmodP& other) const;
  ElementZmodP operator*(const Rational& other) const;
  ElementZmodP operator*(const ElementZmodP& other) const;
  std::string toString(const FormatExpressions* format = nullptr) const;
  std::string toMathML(
    const FormatExpressions* format = nullptr,
    MathExpressionFormattingProperties* outputProperties = nullptr
  ) const;
  std::string toStringModP() const;
  std::string toMathMLModP() const;
  static std::string toStringModP(const LargeIntegerUnsigned& modulus);
  bool assignRational(const Rational& other);
  void operator=(const Rational& other);
  bool operator/=(const ElementZmodP& den);
  bool operator/=(const LargeInteger& den);
  bool needsParenthesisForMultiplication(
    const FormatExpressions* format = nullptr
  ) const;
  static void convertPolynomialRationalToModular(
    const Polynomial<Rational>& input,
    Polynomial<ElementZmodP>& output,
    const LargeIntegerUnsigned& newModulus
  );
  static void convertPolynomialsRationalToModular(
    const List<Polynomial<Rational> >& input,
    List<Polynomial<ElementZmodP> >& output,
    const LargeIntegerUnsigned& newModulus
  );
  static void convertPolynomialModularToPolynomialIntegral(
    const Polynomial<ElementZmodP>& input,
    Polynomial<Rational>& output,
    bool useNegatives
  );
  // Convert modular polynomial to another modular polynomial by copying all
  // values using the new modulus.
  static void convertLiftPolynomialModular(
    const Polynomial<ElementZmodP>& input,
    Polynomial<ElementZmodP>& output,
    const LargeIntegerUnsigned& newModulus
  );
  // Format a polynomial with modular coefficients.
  // The modulo information will not be repeated for all coefficients, but will
  // be put at the end of the expression.
  std::string toStringPolynomial(
    const Polynomial<ElementZmodP>& input, FormatExpressions* format
  ) const;
  std::string toStringPolynomialCalculator(
    const Polynomial<ElementZmodP>& input, FormatExpressions* format
  ) const;
  std::string toMathMLPolynomialCalculator(
    const Polynomial<ElementZmodP>& input, FormatExpressions* format
  ) const;
  ElementZmodP zero() const;
  ElementZmodP one() const;
  static ElementZmodP zeroStatic();
  static ElementZmodP oneStatic();
  bool operator>(const ElementZmodP& other) const;
  class Test {
  public:
    static bool all();
    static bool basicOperations();
    static bool scale();
  };
};

#endif // header_math_modular_arithmetic_ALREADY_INCLUDED
