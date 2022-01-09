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
public:
  List<int> coefficients;
  IntegerModulusSmall* modulus;
  PolynomialUnivariateModular();
  PolynomialUnivariateModular(IntegerModulusSmall* modulus);
};

class PolynomialModuloPolynomialModuloInteger {
public:
  PolynomialUnivariateModular modulus;

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
  void operator-=(const PolynomialModuloPolynomial<Coefficient>& other);
  std::string toString(FormatExpressions* format = nullptr) const;
  PolynomialModuloPolynomial<Coefficient> one();
  static unsigned int hashFunction(const PolynomialModuloPolynomial<Coefficient>& input);
  unsigned int hashFunction() const;
  bool operator==(const PolynomialModuloPolynomial<Coefficient>& other)const;
  bool isEqualToZero() const;
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
#endif // header_math_extra_polynomial_factorization_ALREADY_INCLUDED
