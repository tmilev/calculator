// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeaderRationalFunction_already_included
#define vpfHeaderRationalFunction_already_included

#include "math_general.h"

template <class Coefficient>
class RationalFunction {
private:
  void addSameTypes(const RationalFunction<Rational>& other);
  void addHonestRationalFunction(const RationalFunction<Rational>& other);
  void reduceRationalFunctionToPolynomial();
  void reducePolynomialToRational();
  bool convertToType(int theType);
  static bool greatestCommonDivisorQuick(
    const Polynomial<Coefficient>& left,
    const Polynomial<Coefficient>& right,
    Polynomial<Coefficient>& output
  );
public:
  friend std::ostream& operator<<(std::ostream& output, const RationalFunction<Coefficient>& theRF) {
    output << theRF.toString();
    return output;
  }
  MemorySaving<Polynomial<Coefficient> > numerator;
  MemorySaving<Polynomial<Coefficient> > denominator;
  Coefficient constantValue;
  int expressionType;
  enum typeExpression {
    typeConstant = 0,
    typePolynomial = 1,
    typeRationalFunction = 2,
    typeError = 3
  };
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  bool needsParenthesisForMultiplication(FormatExpressions* unused = nullptr) const;
  bool findOneVariableRationalRoots(List<Rational>& output);
  static RationalFunction one();
  static RationalFunction zero();
  static RationalFunction zeroStatic();
  int minimalNumberOfVariables() const;
  bool substitution(
    const PolynomialSubstitution<Rational>& substitution,
    const Coefficient& one
  );
  bool substitution(
    const PolynomialSubstitution<Rational>& substitution,
    const Coefficient& one,
    std::stringstream* commentsOnFailure
  );
  RationalFunction(const RationalFunction<Rational>& other);
  RationalFunction();
  RationalFunction(int other);
  RationalFunction(const Rational& other);
  Rational rationalValue() const;
  void raiseToPower(int thePower);
  void reduceMemory();
  void operator=(const Polynomial<Coefficient>& other);
  unsigned int hashFunction() const;
  static unsigned int hashFunction(const RationalFunction<Coefficient>& input);
  void operator=(int other);
  void operator=(const Rational& other);
  void operator=(const RationalFunction<Rational>& other);
  bool checkConsistency() const;
  void setNumberOfVariables(int goalNumVars);
  void setNumberOfVariablesSubstituteDeletedByOne(int newNumVars);
  void makeOneLetterMonomial(int theIndex, const Rational& theCoeff, int ExpectedNumVars = 0);
  void getNumerator(Polynomial<Coefficient>& output) const;
  void getDenominator(Polynomial<Coefficient>& output) const;
  bool isNegative();
  void clearDenominators(RationalFunction<Coefficient>& outputWasMultipliedBy);
  void operator+=(const Polynomial<Coefficient>& other);
  void operator+=(const RationalFunction<Coefficient>& other);
  bool operator==(int other) const;
  bool operator!=(int other);
  bool operator==(const RationalFunction<Coefficient>& other) const;
  void simplify();
  // Scales the numerator and denominator simultaneously
  // so all coefficients are integers,
  // the leading monomial of the denominator has positive coefficient,
  // and either the denominator or the numerator
  // has coefficients that are relatively prime.
  void simplifyLeadingCoefficientOnly();
  // Scales the numerator and denominator so that both
  // have positive leading coefficients, all coefficients are integers,
  // and the denominator and numerator have relatively prime coefficients.
  // Returns the number by which the element was multiplied.
  Rational scaleToIntegral();
  void operator+=(int theConstant);
  void operator*=(const RationalFunction<Coefficient>& other);
  void operator*=(const Polynomial<Coefficient>& other);
  void operator*=(const MonomialP& other);
  void operator*=(const Coefficient& other);
  void operator*=(int other);
  bool operator<(const RationalFunction<Rational>& other) const;
  bool operator<=(const RationalFunction<Rational>& other) const;
  bool operator>(const RationalFunction<Rational>& other) const;
  void multiplyByConstant(const Rational& theConst);
  void invert();
  void makeOne();
  void makeZero();
  void makeMonomial(
    int letterIndex,
    const Rational& power,
    const Rational& coefficient = 1
  );
  void makeConstant(const Rational& theCoeff) {
    this->makeZero();
    this->constantValue = theCoeff;
  }
  bool isConstant(Rational* whichConstant = nullptr) const;
  bool isInteger() const;
  bool isSmallInteger(int* whichInteger = nullptr) const;
  bool isEqualToZero() const {
    return this->expressionType == this->typeConstant && this->constantValue.isEqualToZero();
  }
  bool isEqualToOne() const {
    return this->expressionType == this->typeConstant && this->constantValue.isEqualToOne();
  }
  bool isEqualTo(const RationalFunction<Coefficient>& other) const;
  static bool getRelations(
    const List<Polynomial<Rational> >& inputElements,
    List<Polynomial<Rational> >& outputGeneratorLabels,
    List<Polynomial<Rational> >& outputRelations,
    std::stringstream& comments
  );
  static RationalFunction scaleNormalizeIndex(
    List<RationalFunction<Rational> >& input, int indexNonZeroElement
  );
  static void greatestCommonDivisor(
    const Polynomial<Coefficient>& left,
    const Polynomial<Coefficient>& right,
    Polynomial<Coefficient>& output
  );
  static void leastCommonMultiple(
    const Polynomial<Coefficient>& left,
    const Polynomial<Coefficient>& right,
    Polynomial<Coefficient>& output
  );
  void operator-=(int other) {
    *this -= static_cast<Rational>(other);
  }
  void operator-=(const Coefficient& other);
  void operator-=(const RationalFunction<Coefficient>& other);
  void operator/=(int other);
  void operator/=(const Polynomial<Coefficient>& other);
  void operator/=(const RationalFunction<Coefficient>& other);
  void negate();
  class Test {
  public:
    static bool all();
    static bool scaleNormalizeIndex();
    static RationalFunction<Rational> fromString(const std::string& input);
    static bool fromStringTest();
  };
};

template<>
bool RationalFunction<Rational>::Test::fromStringTest();
template<>
bool RationalFunction<Rational>::Test::scaleNormalizeIndex();

#endif
