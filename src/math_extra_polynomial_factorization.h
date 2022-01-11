// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_math_extra_polynomial_factorization_ALREADY_INCLUDED
#define header_math_extra_polynomial_factorization_ALREADY_INCLUDED
#include "math_general.h"

class IntegerModulusSmall {
public:
  int modulus;
  List<int> inverses;
};

// Dense univariate polynomial mod p for a small integer p.
// Intended for very fast computations.
class PolynomialUnivariateModular {
  friend std::ostream& operator<<(
    std::ostream& output,
    const PolynomialUnivariateModular& input
  ) {
    output << input.toString();
    return output;
  }
private:
  // Makes sure the coefficients array holds
  // at least the given number of elements.
  // Beefs up newly created coefficients with zeroes.
  void ensureCoefficientLength(int desiredLength);
  void trimTrailingZeroes();
public:
  List<int> coefficients;
  IntegerModulusSmall* modulus;
  PolynomialUnivariateModular();
  PolynomialUnivariateModular(IntegerModulusSmall* modulus);
  void derivative(PolynomialUnivariateModular& output) const;
  void operator=(const Polynomial<ElementZmodP>& other);
  void operator=(const ElementZmodP& other);
  void operator-=(const PolynomialUnivariateModular& other);
  void operator*=(const PolynomialUnivariateModular& other);
  void operator*=(int other);
  std::string toString(FormatExpressions* format = nullptr) const;
  static bool greatestCommonDivisor(
    const PolynomialUnivariateModular& left,
    const PolynomialUnivariateModular& right,
    PolynomialUnivariateModular& output,
    const ElementZmodP& unused,
    std::stringstream* commentsOnFailure
  );
  static bool greatestCommonDivisor(
    const PolynomialUnivariateModular& left,
    const PolynomialUnivariateModular& right,
    PolynomialUnivariateModular& output,
    std::stringstream* commentsOnFailure
  );
  int getLeadingCoefficient() const;
  void divideBy(
    const PolynomialUnivariateModular& divisor,
    PolynomialUnivariateModular& outputQuotient,
    PolynomialUnivariateModular& outputRemainder
  ) const;
  void addAnotherTimesConstant(
    const PolynomialUnivariateModular& other, int coefficient
  );
  void addAnotherTimesTerm(
    const PolynomialUnivariateModular& other,
    int coefficient,
    int termPower
  );
  void addTerm(
    int coefficient,
    int termPower
  );
  void toPolynomialNonDense(Polynomial<ElementZmodP>& output) const;
  bool isConstant() const;
  bool checkInitialization() const;
  int totalDegreeInt() const;
  ElementZmodP evaluate(
    const ElementZmodP& input,
    const ElementZmodP& unused
  ) const;
  inline int getModulus() const {
    return this->modulus->modulus;
  }
  inline int reduceIntModP(int input) {
    input %= this->modulus->modulus;
    if (input < 0) {
      input += this->modulus->modulus;
    }
    return input;
  }
  void makeZero();
  bool isEqualToZero() const;
  // Multiplies the polynomial by a constant so the leading coefficient is one.
  void rescaleSoLeadingCoefficientIsOne();
  class Test {
  public:
    static bool all();
    static bool greatestCommonDivisor();
    static bool division();
    static bool testOneGreatestCommonDivisor(
      int modulus,
      const std::string& left,
      const std::string& right,
      const std::string& expected
    );
    static bool testOneDivision(
      int modulus,
      const std::string& dividend,
      const std::string& divisor,
      const std::string& expectedQuotient,
      const std::string& expectedRemainder
    );
  };
};

class PolynomialUnivariateModularAsModulus {
public:
  void operator=(const PolynomialUnivariateModular& inputModulus);
};

class PolynomialModuloPolynomialModuloInteger {
  friend std::ostream& operator<<(
    std::ostream& output,
    const PolynomialModuloPolynomialModuloInteger& input
  ) {
    output << input.toString();
    return output;
  }
public:
  PolynomialUnivariateModularAsModulus* modulus;
  PolynomialUnivariateModular value;

  void makeFromModulusAndValue(
    PolynomialUnivariateModularAsModulus* inputModulus,
    const Polynomial<ElementZmodP>& inputValue
  );

  void operator-=(const PolynomialModuloPolynomialModuloInteger& other);
  void operator+=(const ElementZmodP& other);
  void operator*=(const PolynomialModuloPolynomialModuloInteger& other);

  bool isEqualToZero() const;
  const PolynomialUnivariateModular& getValue() const {
    return this->value;
  }
  std::string toString(FormatExpressions* format = nullptr) const;
  void reduce();
};

template <class Coefficient>
class PolynomialModuloPolynomial {
  friend std::ostream& operator<<(std::ostream& output, const PolynomialModuloPolynomial& unused) {
    (void) unused;
    output << "A polynomial modulo polynomial.";
    return output;
  }
public:
  Polynomial<Coefficient> modulus;
  Polynomial<Coefficient> value;
  void reduce();
  void operator*=(const PolynomialModuloPolynomial<Coefficient>& other);
  void operator+=(const PolynomialModuloPolynomial<Coefficient>& other);
  void operator+=(const Coefficient& other);
  void operator-=(const PolynomialModuloPolynomial<Coefficient>& other);
  std::string toString(FormatExpressions* format = nullptr) const;
  PolynomialModuloPolynomial<Coefficient> one();
  static unsigned int hashFunction(const PolynomialModuloPolynomial<Coefficient>& input);
  unsigned int hashFunction() const;
  bool operator==(const PolynomialModuloPolynomial<Coefficient>& other)const;
  bool isEqualToZero() const;
  void makeFromModulusAndValue(
    const Polynomial<Coefficient>* inputModulus,
    const Polynomial<Coefficient>& inputValue
  );
  const Polynomial<Coefficient>& getValue() {
    return this->value;
  }
};

class PolynomialFactorizationKronecker {
  public:
  static const int maximumDegreeDefault = 20;
  PolynomialFactorizationUnivariate<Rational>* output;
  Polynomial<Rational> current;
  bool oneFactor(
    std::stringstream* comments,
    std::stringstream* commentsOnFailure
  );
  PolynomialFactorizationKronecker() {
    this->output = nullptr;
  }
  static bool solvePolynomial(
    const Polynomial<Rational>& input,
    List<AlgebraicNumber>& output,
    AlgebraicClosureRationals& closure,
    std::stringstream* commentsOnFailure
  );
  static std::string name() {
    return "Kronecker";
  }
};

template<class Coefficient>
void PolynomialModuloPolynomial<Coefficient>::operator*=(
  const PolynomialModuloPolynomial& other
) {
  if (other.modulus != this->modulus) {
    global.fatal << "Not allowed to multiply quotient-ring "
    << "elements of different rings. [This modulus, other modulus]: "
    << this->modulus << ", " << other.modulus << global.fatal;
  }
  this->value *= other.value;
  this->reduce();
}

template<class Coefficient>
void PolynomialModuloPolynomial<Coefficient>::operator+=(
  const PolynomialModuloPolynomial& other
) {
  if (other.modulus != this->modulus) {
    global.fatal << "Not allowed to add quotient-ring "
    << "elements of different rings. [This modulus, other modulus]: "
    << this->modulus << ", " << other.modulus << global.fatal;
  }
  this->value += other.value;
  this->reduce();
}

template<class Coefficient>
void PolynomialModuloPolynomial<Coefficient>::operator+=(
  const Coefficient& other
) {
  this->value += other;
  this->reduce();
}

template<class Coefficient>
void PolynomialModuloPolynomial<Coefficient>::operator-=(
  const PolynomialModuloPolynomial& other
) {
  if (other.modulus != this->modulus) {
    global.fatal << "Not allowed to subtract quotient-ring "
    << "elements of different rings. [This modulus, other modulus]: "
    << this->modulus << ", " << other.modulus << global.fatal;
  }
  this->value -= other.value;
  this->reduce();
}

template<class Coefficient>
unsigned int PolynomialModuloPolynomial<Coefficient>::hashFunction() const {
  return
  this->value.hashFunction() * HashConstants::constant0 +
  this->modulus.hashFunction() * HashConstants::constant1;
}

template<class Coefficient>
bool PolynomialModuloPolynomial<Coefficient>::operator==(
  const PolynomialModuloPolynomial<Coefficient>& other
) const {
  return this->modulus == other.modulus && this->value == other.value;
}

template<class Coefficient>
unsigned int PolynomialModuloPolynomial<Coefficient>::hashFunction(
  const PolynomialModuloPolynomial<Coefficient>& input
) {
  return input.hashFunction();
}

template<class Coefficient>
std::string PolynomialModuloPolynomial<Coefficient>::toString(
  FormatExpressions* format
) const {
  std::stringstream out;
  out << this->value.toString(format) << "(mod (" << this->modulus.toString(format) << "))";
  return out.str();
}

template<class Coefficient>
PolynomialModuloPolynomial<Coefficient> PolynomialModuloPolynomial<Coefficient>::one() {
  PolynomialModuloPolynomial<Coefficient> result;
  result.modulus = this->modulus;
  result.value.makeConstant(this->modulus.coefficients[0].one());
  return result;
}

template<class Coefficient>
bool PolynomialModuloPolynomial<Coefficient>::isEqualToZero() const {
  return this->value.isEqualToZero();
}

template<class Coefficient>
void PolynomialModuloPolynomial<Coefficient>::reduce() {
  Polynomial<Coefficient> unusedQuotient;
  this->value.divideBy(
    this->modulus,
    unusedQuotient,
    this->value,
    &MonomialPolynomial::orderDefault()
  );
}

template <class Coefficient>
void PolynomialModuloPolynomial<Coefficient>::makeFromModulusAndValue(
  const Polynomial<Coefficient>* inputModulus,
  const Polynomial<Coefficient>& inputValue
) {
  if (inputModulus == nullptr) {
    global.fatal << "Null modulus not allowed. " << global.fatal;
  }
  this->modulus = *inputModulus;
  this->value = inputValue;
}
#endif // header_math_extra_polynomial_factorization_ALREADY_INCLUDED
