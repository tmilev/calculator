
#ifndef header_math_extra_weyl_algebras_implementation_ALREADY_INCLUDED
#define header_math_extra_weyl_algebras_implementation_ALREADY_INCLUDED

#include "math_general.h"
#include "math_extra_algebraic_numbers.h"

template <class Coefficient>
bool ElementWeylAlgebra<Coefficient>::isPolynomial(
  Polynomial<Coefficient>* whichPolynomial
) const {
  if (whichPolynomial != 0) {
    whichPolynomial->makeZero();
  }
  for (int i = 0; i < this->size(); i ++) {
    if (!(*this)[i].differentialPart.isConstant()) {
      return false;
    }
    if (whichPolynomial != 0) {
      whichPolynomial->addMonomial((*this)[i].polynomialPart, this->
        coefficients[i]
      );
    }
  }
  return true;
}

template <class Coefficient>
bool ElementWeylAlgebra<Coefficient>::hasNonSmallPositiveIntegerDerivation()
const {
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].hasNonSmallPositiveIntegerDerivation()) {
      return true;
    }
  }
  return false;
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::multiplyTwoMonomials(
  const MonomialWeylAlgebra& left,
  const MonomialWeylAlgebra& right,
  ElementWeylAlgebra& output
) const {
  SelectionWithDifferentMaxMultiplicities tempSel;
  int dimension =
  MathRoutines::maximum(
    left.minimalNumberOfVariables(), right.minimalNumberOfVariables()
  );
  tempSel.multiplicities.initializeFillInObject(dimension, 0);
  tempSel.capacities.setSize(dimension);
  int expectedSize = 1;
  for (int i = 0; i < dimension; i ++) {
    int powerDiffOp = 0;
    if (!left.differentialPart(i).isSmallInteger(&powerDiffOp)) {
      global.fatal
      << "Requested operations with elements of Weyl algebra "
      << "that have monomials of exponent "
      << left.differentialPart(i).toString()
      << " which I cannot handle. If this is bad user input, "
      << "it should have been caught at an earlier level. "
      << global.fatal;
    }
    tempSel.capacities[i] = powerDiffOp;
    expectedSize *= powerDiffOp;
  }
  tempSel.elements.initializeFillInObject(dimension, 0);
  MonomialWeylAlgebra buffer;
  buffer.makeOne();
  output.makeZero();
  int numCycles = tempSel.totalNumberSubsetsSmallInt();
  Rational coefficientBuffer;
  output.setExpectedSize(expectedSize);
  for (int i = 0; i < numCycles; i ++) {
    coefficientBuffer = 1;
    for (int k = 0; k < dimension; k ++) {
      int multiplicityDrop = tempSel.multiplicities[k];
      Rational differentialOperatorPower = left.differentialPart(k);
      Rational polynomialPower = right.polynomialPart(k);
      coefficientBuffer *=
      Rational::nChooseK(differentialOperatorPower, multiplicityDrop) *
      Rational::nChooseK(polynomialPower, multiplicityDrop) *
      Rational::factorial(multiplicityDrop);
      buffer.polynomialPart.setVariable(
        k,
        left.polynomialPart(k) + right.polynomialPart(k) - multiplicityDrop
      );
      buffer.differentialPart.setVariable(
        k,
        left.differentialPart(k) + right.differentialPart(k) - multiplicityDrop
      );
    }
    output.addMonomial(buffer, coefficientBuffer);
    tempSel.incrementReturnFalseIfPastLast();
  }
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::lieBracketOnTheLeftMakeReport(
  const ElementWeylAlgebra& standsOnTheLeft
) {
  this->lieBracketOnTheLeft(standsOnTheLeft);
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::lieBracketOnTheRightMakeReport(
  const ElementWeylAlgebra& standsOnTheRight
) {
  this->lieBracketOnTheRight(standsOnTheRight);
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::lieBracket(
  const ElementWeylAlgebra& left,
  const ElementWeylAlgebra& right,
  ElementWeylAlgebra& output
) {
  if (&output == &right || &output == &left) {
    ElementWeylAlgebra leftCopy = left;
    ElementWeylAlgebra rightCopy = right;
    ElementWeylAlgebra::lieBracket(leftCopy, rightCopy, output);
    return;
  }
  output = right;
  output.multiplyOnTheLeft(left);
  ElementWeylAlgebra subtracand;
  subtracand = left;
  subtracand.multiplyOnTheLeft(right);
  output -= subtracand;
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::assignPolynomial(
  const Polynomial<Rational>& input
) {
  this->makeZero();
  MonomialWeylAlgebra monomial;
  for (int i = 0; i < input.size(); i ++) {
    monomial.polynomialPart = input[i];
    this->addMonomial(monomial, input.coefficients[i]);
  }
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::lieBracketOnTheLeft(
  const ElementWeylAlgebra& standsOnTheLeft
) {
  ElementWeylAlgebra rightSummand;
  ElementWeylAlgebra leftSummand;
  rightSummand = *this;
  rightSummand.multiplyOnTheLeft(standsOnTheLeft);
  leftSummand = standsOnTheLeft;
  leftSummand.multiplyOnTheLeft(*this);
  *this = rightSummand;
  *this -= leftSummand;
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::lieBracketOnTheRight(
  const ElementWeylAlgebra& standsOnTheRight
) {
  ElementWeylAlgebra leftSummand;
  ElementWeylAlgebra rightSummand;
  leftSummand = standsOnTheRight;
  leftSummand.multiplyOnTheLeft(*this);
  rightSummand = *this;
  rightSummand.multiplyOnTheLeft(standsOnTheRight);
  *this = leftSummand;
  *this -= rightSummand;
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::multiplyOnTheLeft(
  const ElementWeylAlgebra& standsOnTheLeft
) {
  ElementWeylAlgebra summand;
  ElementWeylAlgebra accumulator;
  accumulator.makeZero();
  Coefficient currentCoefficient;
  for (int j = 0; j < standsOnTheLeft.size(); j ++) {
    for (int i = 0; i < this->size(); i ++) {
      this->multiplyTwoMonomials(
        standsOnTheLeft[j], (*this)[i], summand
      );
      currentCoefficient = standsOnTheLeft.coefficients[j];
      currentCoefficient *= this->coefficients[i];
      summand *= currentCoefficient;
      accumulator += summand;
    }
  }
  *this = accumulator;
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::operator*=(
  const ElementWeylAlgebra& standsOnTheRight
) {
  ElementWeylAlgebra summand;
  ElementWeylAlgebra accumulator;
  accumulator.makeZero();
  Coefficient currentCoefficient;
  for (int j = 0; j < standsOnTheRight.size(); j ++) {
    for (int i = 0; i < this->size(); i ++) {
      this->multiplyTwoMonomials((*this)[i], standsOnTheRight[j], summand
      );
      currentCoefficient = this->coefficients[i];
      currentCoefficient *= standsOnTheRight.coefficients[j];
      summand *= currentCoefficient;
      accumulator += summand;
    }
  }
  *this = accumulator;
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::raiseToPower(int power) {
  ElementWeylAlgebra weylOne;
  weylOne.makeOne();
  MathRoutines::raiseToPower(*this, power, weylOne);
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::makeGEpsPlusEpsInTypeD(
  int i, int j, int numberOfVariables
) {
  this->makeZero();
  MonomialWeylAlgebra monomialWeylAlgebra;
  monomialWeylAlgebra.makeOne();
  monomialWeylAlgebra.polynomialPart.setVariable(i, 1);
  monomialWeylAlgebra.differentialPart.setVariable(j + numberOfVariables, 1);
  this->addMonomial(monomialWeylAlgebra, 1);
  monomialWeylAlgebra.makeOne();
  monomialWeylAlgebra.polynomialPart.setVariable(j, 1);
  monomialWeylAlgebra.differentialPart.setVariable(i + numberOfVariables, 1);
  this->addMonomial(monomialWeylAlgebra, 1);
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::makeGEpsMinusEpsInTypeD(
  int i, int j, int numberOfVariables
) {
  this->makeZero();
  MonomialWeylAlgebra monomialWeylAlgebra;
  monomialWeylAlgebra.makeOne();
  monomialWeylAlgebra.polynomialPart.setVariable(i, 1);
  monomialWeylAlgebra.differentialPart.setVariable(j, 1);
  this->addMonomial(monomialWeylAlgebra, 1);
  monomialWeylAlgebra.makeOne();
  monomialWeylAlgebra.polynomialPart.setVariable(j + numberOfVariables, 1);
  monomialWeylAlgebra.differentialPart.setVariable(i + numberOfVariables, 1);
  this->addMonomial(monomialWeylAlgebra, 1);
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::makeGMinusEpsMinusEpsInTypeD(
  int i, int j, int numberOfVariables
) {
  this->makeZero();
  MonomialWeylAlgebra monomialWeylAlgebra;
  monomialWeylAlgebra.makeOne();
  monomialWeylAlgebra.polynomialPart.setVariable(i + numberOfVariables, 1);
  monomialWeylAlgebra.differentialPart.setVariable(j, 1);
  this->addMonomial(monomialWeylAlgebra, 1);
  monomialWeylAlgebra.makeOne();
  monomialWeylAlgebra.polynomialPart.setVariable(j + numberOfVariables, 1);
  monomialWeylAlgebra.differentialPart.setVariable(i, 1);
  this->addMonomial(monomialWeylAlgebra, 1);
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::makedidj(int i, int j) {
  this->makeZero();
  MonomialWeylAlgebra monomialWeylAlgebra;
  monomialWeylAlgebra.makeOne();
  monomialWeylAlgebra.differentialPart.setVariable(i, 1);
  monomialWeylAlgebra.differentialPart.setVariable(j, 1);
  this->addMonomial(monomialWeylAlgebra, 1);
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::makexixj(int i, int j) {
  this->makeZero();
  MonomialWeylAlgebra monomialWeylAlgebra;
  monomialWeylAlgebra.makeOne();
  monomialWeylAlgebra.polynomialPart.setVariable(i, 1);
  monomialWeylAlgebra.polynomialPart.setVariable(j, 1);
  this->addMonomial(monomialWeylAlgebra, 1);
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::makexi(int i) {
  this->makeZero();
  MonomialWeylAlgebra monomialWeylAlgebra;
  monomialWeylAlgebra.makeOne();
  monomialWeylAlgebra.polynomialPart.setVariable(i, 1);
  this->addMonomial(monomialWeylAlgebra, 1);
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::makedi(int i) {
  this->makeZero();
  MonomialWeylAlgebra monomialWeylAlgebra;
  monomialWeylAlgebra.makeOne();
  monomialWeylAlgebra.differentialPart.setVariable(i, 1);
  this->addMonomial(monomialWeylAlgebra, 1);
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::makexidj(int i, int j) {
  this->makeZero();
  MonomialWeylAlgebra monomialWeylAlgebra;
  monomialWeylAlgebra.makeOne();
  monomialWeylAlgebra.polynomialPart.setVariable(i, 1);
  monomialWeylAlgebra.differentialPart.setVariable(j, 1);
  this->addMonomial(monomialWeylAlgebra, 1);
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::
getStandardOrderDifferentialOperatorCorrespondingToNRaisedTo(
  const Rational& inputRationalPower,
  int indexVariable,
  ElementWeylAlgebra& outputDifferentialOperator,
  Polynomial<Rational>& outputDenominator
) {
  outputDenominator.makeOne();
  MonomialWeylAlgebra monomialWeylAlgebra;
  outputDifferentialOperator.makeZero();
  int inputPower = 0;
  if (!inputRationalPower.isSmallInteger(&inputPower)) {
    global.fatal
    << "Differential operator requires integer exponent. "
    << global.fatal;
  }
  if (inputPower >= 0) {
    monomialWeylAlgebra.polynomialPart.makeEi(indexVariable, inputPower);
  } else {
    monomialWeylAlgebra.differentialPart.makeEi(indexVariable, - inputPower);
  }
  outputDifferentialOperator.addMonomial(monomialWeylAlgebra, 1);
  inputPower *= - 1;
  Polynomial<Rational> newMultiplicity;
  newMultiplicity.makeDegreeOne(0, indexVariable, 1);
  for (int i = 0; i < inputPower; i ++) {
    outputDenominator *= newMultiplicity;
    newMultiplicity -= 1;
  }
}

template <class Coefficient>
bool ElementWeylAlgebra<Coefficient>::substitute(
  const PolynomialSubstitution<Rational>& substitutionPolynomialPart,
  const PolynomialSubstitution<Rational>& substitutionDifferentialPart
) {
  STACK_TRACE("ElementWeylAlgebra::substitute");
  Polynomial<Rational> differentialOperatorPart, polyPart;
  MonomialWeylAlgebra monomial;
  ElementWeylAlgebra output;
  output.makeZero();
  Coefficient newCoefficient;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialWeylAlgebra& currentMon = (*this)[i];
    if (
      !currentMon.polynomialPart.substitute(
        substitutionPolynomialPart, polyPart, Rational::one()
      )
    ) {
      return false;
    }
    if (
      !currentMon.differentialPart.substitute(
        substitutionDifferentialPart,
        differentialOperatorPart,
        Rational::one()
      )
    ) {
      return false;
    }
    for (int j = 0; j < polyPart.size(); j ++) {
      for (int k = 0; k < differentialOperatorPart.size(); k ++) {
        monomial.polynomialPart = polyPart[j];
        monomial.differentialPart = differentialOperatorPart[k];
        newCoefficient = this->coefficients[i];
        newCoefficient *= polyPart.coefficients[j];
        newCoefficient *= differentialOperatorPart.coefficients[k];
        output.addMonomial(monomial, newCoefficient);
      }
    }
  }
  *this = output;
  return true;
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::fourierTransform(
  ElementWeylAlgebra<Coefficient>& output
) const {
  STACK_TRACE("ElementWeylAlgebra::fourierTransform");
  if (&output == this) {
    ElementWeylAlgebra thisCopy = *this;
    thisCopy.fourierTransform(output);
    return;
  }
  LargeInteger totalDegree;
  Coefficient coefficient;
  output.makeZero();
  MonomialWeylAlgebra monomial;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialWeylAlgebra& currentMon = (*this)[i];
    if (
      !(
        currentMon.polynomialPart.totalDegree() +
        currentMon.differentialPart.totalDegree()
      ).isInteger(&totalDegree)
    ) {
      global.fatal
      << "Calling Fourier transoform "
      << "on differential operator with "
      << "non-integral exponents is not allowed. "
      << global.fatal;
    }
    monomial.differentialPart = currentMon.polynomialPart;
    monomial.polynomialPart = currentMon.differentialPart;
    coefficient = this->coefficients[i];
    if (totalDegree.isEven()) {
      coefficient *= - 1;
    }
    output.addMonomial(monomial, coefficient);
  }
}

template <class Coefficient>
bool ElementWeylAlgebra<Coefficient>::actOnPolynomial(
  Polynomial<Rational>& actedUpon
) const {
  Polynomial<Rational> result;
  result.makeZero();
  MonomialPolynomial resultMonomial;
  Rational currentCoefficient;
  for (int i = 0; i < this->size(); i ++) {
    for (int j = 0; j < actedUpon.size(); j ++) {
      const MonomialPolynomial& currentMonomial = actedUpon[j];
      const MonomialWeylAlgebra& currentOpMon = (*this)[i];
      resultMonomial = currentMonomial;
      currentCoefficient = actedUpon.coefficients[j];
      currentCoefficient *= this->coefficients[i];
      for (int k = 0; k < currentOpMon.minimalNumberOfVariables(); k ++) {
        int numDiff = 0;
        if (
          !currentOpMon.differentialPart(k).isSmallInteger(&numDiff)
        ) {
          return false;
        }
        for (; numDiff > 0; numDiff --) {
          currentCoefficient *= resultMonomial[k];
          if (currentCoefficient.isEqualToZero()) {
            break;
          }
          resultMonomial.multiplyByVariable(k, - 1);
        }
        if (currentCoefficient.isEqualToZero()) {
          break;
        }
      }
      resultMonomial *= currentOpMon.polynomialPart;
      result.addMonomial(resultMonomial, currentCoefficient);
    }
  }
  actedUpon = result;
  return true;
}

#endif // header_math_extra_weyl_algebras_implementation_ALREADY_INCLUDED
