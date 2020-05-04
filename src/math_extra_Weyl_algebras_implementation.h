// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfImplementationHeaderWeylAlgebras_already_included
#define vpfImplementationHeaderWeylAlgebras_already_included

#include "math_general.h"
#include "math_extra_algebraic_numbers.h"

template <class Coefficient>
bool ElementWeylAlgebra<Coefficient>::isPolynomial(Polynomial<Coefficient>* whichPoly) const {
  if (whichPoly != 0) {
    whichPoly->makeZero();
  }
  for (int i = 0; i < this->size(); i ++) {
    if (!(*this)[i].differentialPart.isConstant()) {
      return false;
    }
    if (whichPoly != 0) {
      whichPoly->addMonomial((*this)[i].polynomialPart, this->coefficients[i]);
    }
  }
  return true;
}

template <class Coefficient>
bool ElementWeylAlgebra<Coefficient>::hasNonSmallPositiveIntegerDerivation() const {
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].hasNonSmallPositiveIntegerDerivation()) {
      return true;
    }
  }
  return false;
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::multiplyTwoMonomials(
  const MonomialWeylAlgebra& left, const MonomialWeylAlgebra& right, ElementWeylAlgebra& output
) const {
  SelectionWithDifferentMaxMultiplicities tempSel;
  int theDimensioN = MathRoutines::maximum(left.minimalNumberOfVariables(), right.minimalNumberOfVariables());
  tempSel.Multiplicities.initializeFillInObject(theDimensioN, 0);
  tempSel.MaxMultiplicities.setSize(theDimensioN);
  int theExpectedSize = 1;
  for (int i = 0; i < theDimensioN; i ++) {
    int powerDiffOp = 0;
    if (!left.differentialPart(i).isSmallInteger(&powerDiffOp)) {
      global.fatal << "This is a programming error. Requested operations with elements of Weyl algebra "
      << "that have monomials of exponent "
      << left.differentialPart(i).toString()
      << " which I cannot handle. If this is bad user input, "
      << "it should have been caught at an earlier level. " << global.fatal;
    }
    tempSel.MaxMultiplicities[i] = powerDiffOp;
    theExpectedSize *= powerDiffOp;
  }
  tempSel.elements.initializeFillInObject(theDimensioN, 0);
  MonomialWeylAlgebra buffer;
  buffer.makeOne();
  output.makeZero();
  int numCycles = tempSel.TotalNumSubsetsMustBeSmalInt();
  Rational coeffBuff;
  output.setExpectedSize(theExpectedSize);
  for (int i = 0; i < numCycles; i ++) {
    coeffBuff = 1;
    for (int k = 0; k<theDimensioN; k ++) {
      int multDrop = tempSel.Multiplicities[k];
      Rational theDOPower = left.differentialPart(k);
      Rational thePolPower = right.polynomialPart(k);
      coeffBuff *= Rational::nChooseK(theDOPower, multDrop) * Rational::nChooseK(thePolPower, multDrop) * Rational::factorial(multDrop);
      buffer.polynomialPart.setVariable(k, left.polynomialPart(k) + right.polynomialPart(k) - multDrop);
      buffer.differentialPart.setVariable(k, left.differentialPart(k) + right.differentialPart(k) - multDrop);
    }
    output.addMonomial(buffer, coeffBuff);
    tempSel.incrementReturnFalseIfPastLast();
  }
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::LieBracketOnTheLeftMakeReport(const ElementWeylAlgebra& standsOnTheLeft) {
  this->lieBracketOnTheLeft(standsOnTheLeft);
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::LieBracketOnTheRightMakeReport(const ElementWeylAlgebra& standsOnTheRight) {
  this->lieBracketOnTheRight(standsOnTheRight);
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::lieBracket(
  const ElementWeylAlgebra& left, const ElementWeylAlgebra& right, ElementWeylAlgebra& output
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
void ElementWeylAlgebra<Coefficient>::lieBracketOnTheLeft(const ElementWeylAlgebra& standsOnTheLeft) {
  ElementWeylAlgebra tempEl1, tempEl2;
  tempEl1 = *this;
  tempEl1.multiplyOnTheLeft(standsOnTheLeft);
  tempEl2 = standsOnTheLeft;
  tempEl2.multiplyOnTheLeft(*this);
  *this = tempEl1;
  *this -= tempEl2;
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::lieBracketOnTheRight(const ElementWeylAlgebra& standsOnTheRight) {
  ElementWeylAlgebra tempEl1, tempEl2;
  tempEl1 = standsOnTheRight;
  tempEl1.multiplyOnTheLeft(*this);
  tempEl2 = *this;
  tempEl2.multiplyOnTheLeft(standsOnTheRight);
  *this = tempEl1;
  *this -= tempEl2;
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::multiplyOnTheLeft(const  ElementWeylAlgebra& standsOnTheLeft) {
  ElementWeylAlgebra buffer;
  ElementWeylAlgebra Accum;
  Accum.makeZero();
  Coefficient currentCF;
  for (int j = 0; j < standsOnTheLeft.size(); j ++) {
    for (int i = 0; i < this->size(); i ++) {
      this->multiplyTwoMonomials(standsOnTheLeft[j], (*this)[i], buffer);
      currentCF = standsOnTheLeft.coefficients[j];
      currentCF *= this->coefficients[i];
      buffer *= currentCF;
      Accum += buffer;
    }
  }
  *this = Accum;
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::operator*=(const ElementWeylAlgebra& standsOnTheRight) {
  ElementWeylAlgebra buffer;
  ElementWeylAlgebra Accum;
  Accum.makeZero();
  Coefficient currentCF;
  for (int j = 0; j < standsOnTheRight.size(); j ++) {
    for (int i = 0; i < this->size(); i ++) {
      this->multiplyTwoMonomials((*this)[i], standsOnTheRight[j], buffer);
      currentCF = this->coefficients[i];
      currentCF *= standsOnTheRight.coefficients[j];
      buffer *= currentCF;
      Accum += buffer;
    }
  }
  *this = Accum;
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::raiseToPower(int thePower) {
  ElementWeylAlgebra WeylOne;
  WeylOne.makeOne();
  MathRoutines::raiseToPower(*this, thePower, WeylOne);
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::MakeGEpsPlusEpsInTypeD(int i, int j, int NumVars) {
  this->makeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.makeOne();
  tempMon.polynomialPart.setVariable(i, 1);
  tempMon.differentialPart.setVariable(j + NumVars, 1);
  this->addMonomial(tempMon, 1);
  tempMon.makeOne();
  tempMon.polynomialPart.setVariable(j, 1);
  tempMon.differentialPart.setVariable(i + NumVars, 1);
  this->addMonomial(tempMon, 1);
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::MakeGEpsMinusEpsInTypeD(int i, int j, int NumVars) {
  this->makeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.makeOne();
  tempMon.polynomialPart.setVariable(i, 1);
  tempMon.differentialPart.setVariable(j, 1);
  this->addMonomial(tempMon, 1);
  tempMon.makeOne();
  tempMon.polynomialPart.setVariable(j + NumVars, 1);
  tempMon.differentialPart.setVariable(i + NumVars, 1);
  this->addMonomial(tempMon, 1);
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::MakeGMinusEpsMinusEpsInTypeD(int i, int j, int NumVars) {
  this->makeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.makeOne();
  tempMon.polynomialPart.setVariable(i + NumVars, 1);
  tempMon.differentialPart.setVariable(j, 1);
  this->addMonomial(tempMon, 1);
  tempMon.makeOne();
  tempMon.polynomialPart.setVariable(j + NumVars, 1);
  tempMon.differentialPart.setVariable(i, 1);
  this->addMonomial(tempMon, 1);
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::Makedidj(int i, int j, int NumVars) {
  this->makeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.makeOne();
  tempMon.differentialPart.setVariable(i, 1);
  tempMon.differentialPart.setVariable(j, 1);
  this->addMonomial(tempMon, 1);
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::Makexixj(int i, int j, int NumVars) {
  this->makeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.makeOne();
  tempMon.polynomialPart.setVariable(i, 1);
  tempMon.polynomialPart.setVariable(j, 1);
  this->addMonomial(tempMon, 1);
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::Makexi(int i, int NumVars) {
  (void) NumVars;
  this->makeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.makeOne();
  tempMon.polynomialPart.setVariable(i, 1);
  this->addMonomial(tempMon, 1);
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::Makedi(int i, int NumVars) {
  (void) NumVars;
  this->makeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.makeOne();
  tempMon.differentialPart.setVariable(i, 1);
  this->addMonomial(tempMon, 1);
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::Makexidj(int i, int j, int NumVars) {
  (void) NumVars;
  this->makeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.makeOne();
  tempMon.polynomialPart.setVariable(i, 1);
  tempMon.differentialPart.setVariable(j, 1);
  this->addMonomial(tempMon, 1);
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::GetStandardOrderDiffOperatorCorrespondingToNraisedTo(
  const Rational& inputRationalPower,
  int indexVar,
  ElementWeylAlgebra& outputDO,
  Polynomial<Rational>& outputDenominator
) {
  outputDenominator.makeOne();
  MonomialWeylAlgebra tempMon;
  outputDO.makeZero();
  int inputPower = 0;
  if (!inputRationalPower.isSmallInteger(&inputPower)) {
    global.fatal << "This is a programming error: "
    << "I can give you a differential operator only from integer exponent. " << global.fatal;
  }
  if (inputPower >= 0) {
    tempMon.polynomialPart.makeEi(indexVar, inputPower);
  } else {
    tempMon.differentialPart.makeEi(indexVar, - inputPower);
  }
  outputDO.addMonomial(tempMon, 1);
  inputPower *= - 1;
  Polynomial<Rational> newMult;
  newMult.makeDegreeOne(0, indexVar, 1);
  for (int i = 0; i < inputPower; i ++) {
    outputDenominator *= newMult;
    newMult -= 1;
  }
}

template <class Coefficient>
bool ElementWeylAlgebra<Coefficient>::substitution(
  const PolynomialSubstitution<Rational>& SubPolyPart, const PolynomialSubstitution<Rational>& SubDiffPArt
) {
  MacroRegisterFunctionWithName("ElementWeylAlgebra::substitution");
  Polynomial<Rational> DOpart, polyPart;
  MonomialWeylAlgebra theMon;
  ElementWeylAlgebra output;
  output.makeZero();
  Coefficient theNewCoeff;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialWeylAlgebra& currentMon = (*this)[i];
    if (!currentMon.polynomialPart.substitution(SubPolyPart, polyPart)) {
      return false;
    }
    if (!currentMon.differentialPart.substitution(SubDiffPArt, DOpart)) {
      return false;
    }
    for (int j = 0; j < polyPart.size(); j ++) {
      for (int k = 0; k < DOpart.size(); k ++) {
        theMon.polynomialPart = polyPart[j];
        theMon.differentialPart = DOpart[k];
        theNewCoeff = this->coefficients[i];
        theNewCoeff *= polyPart.coefficients[j];
        theNewCoeff *= DOpart.coefficients[k];
        output.addMonomial(theMon, theNewCoeff);
      }
    }
  }
  *this = output;
  return true;
}

template <class Coefficient>
void ElementWeylAlgebra<Coefficient>::FourierTransform(ElementWeylAlgebra<Coefficient>& output) const {
  MacroRegisterFunctionWithName("ElementWeylAlgebra::FourierTransformMe");
  if (&output == this) {
    ElementWeylAlgebra thisCopy = *this;
    thisCopy.FourierTransform(output);
    return;
  }
  LargeInteger totalDeg;
  Coefficient theCoeff;
  output.makeZero();
  MonomialWeylAlgebra theMon;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialWeylAlgebra& currentMon = (*this)[i];
    if (!(currentMon.polynomialPart.totalDegree() + currentMon.differentialPart.totalDegree()).isInteger(&totalDeg)) {
      global.fatal << "This is a programming error: calling Fourier transoform "
      << "on differential operator with non-integral exponents. " << global.fatal;
    }
    theMon.differentialPart = currentMon.polynomialPart;
    theMon.polynomialPart = currentMon.differentialPart;
    theCoeff = this->coefficients[i];
    if (totalDeg.IsEven()) {
      theCoeff *= - 1;
    }
    output.addMonomial(theMon, theCoeff);
  }
}

template <class Coefficient>
bool ElementWeylAlgebra<Coefficient>::ActOnPolynomial(Polynomial<Rational>& thePoly) const {
  Polynomial<Rational> result;
  result.makeZero();
  MonomialP resultMon;
  Rational coeff;
  for (int i = 0; i < this->size(); i ++) {
    for (int j = 0; j < thePoly.size(); j ++) {
      const MonomialP& currentPolMon = thePoly[j];
      const MonomialWeylAlgebra& currentOpMon = (*this)[i];
      resultMon = currentPolMon;
      coeff = thePoly.coefficients[j];
      coeff *= this->coefficients[i];
      for (int k = 0; k < currentOpMon.minimalNumberOfVariables(); k ++) {
        int numDiff = 0;
        if (!currentOpMon.differentialPart(k).isSmallInteger(&numDiff)) {
          return false;
        }
        for (; numDiff > 0; numDiff --) {
          coeff *= resultMon[k];
          if (coeff.isEqualToZero()) {
            break;
          }
          resultMon.multiplyByVariable(k, -1);
        }
        if (coeff.isEqualToZero()) {
          break;
        }
      }
      resultMon *= currentOpMon.polynomialPart;
      result.addMonomial(resultMon, coeff);
    }
  }
  thePoly = result;
  return true;
}

#endif
