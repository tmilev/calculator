// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfImplementationHeaderWeylAlgebras_already_included
#define vpfImplementationHeaderWeylAlgebras_already_included

#include "math_general.h"
#include "math_extra_algebraic_numbers.h"

template <class coefficient>
bool ElementWeylAlgebra<coefficient>::IsPolynomial(Polynomial<coefficient>* whichPoly) const {
  if (whichPoly != 0) {
    whichPoly->makeZero();
  }
  for (int i = 0; i < this->size(); i ++) {
    if (!(*this)[i].differentialPart.IsConstant()) {
      return false;
    }
    if (whichPoly != 0) {
      whichPoly->AddMonomial((*this)[i].polynomialPart, this->coefficients[i]);
    }
  }
  return true;
}

template <class coefficient>
bool ElementWeylAlgebra<coefficient>::HasNonSmallPositiveIntegerDerivation() const {
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].HasNonSmallPositiveIntegerDerivation()) {
      return true;
    }
  }
  return false;
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::MultiplyTwoMonomials(
  const MonomialWeylAlgebra& left, const MonomialWeylAlgebra& right, ElementWeylAlgebra& output
) const {
  SelectionWithDifferentMaxMultiplicities tempSel;
  int theDimensioN = MathRoutines::Maximum(left.minimalNumberOfVariables(), right.minimalNumberOfVariables());
  tempSel.Multiplicities.initializeFillInObject(theDimensioN, 0);
  tempSel.MaxMultiplicities.setSize(theDimensioN);
  int theExpectedSize = 1;
  for (int i = 0; i < theDimensioN; i ++) {
    int powerDiffOp = 0;
    if (!left.differentialPart(i).IsSmallInteger(&powerDiffOp)) {
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
  buffer.makeOne(theDimensioN);
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
      coeffBuff *= Rational::NChooseK(theDOPower, multDrop) * Rational::NChooseK(thePolPower, multDrop) * Rational::Factorial(multDrop);
      buffer.polynomialPart.setVariable(k, left.polynomialPart(k) + right.polynomialPart(k) - multDrop);
      buffer.differentialPart.setVariable(k, left.differentialPart(k) + right.differentialPart(k) - multDrop);
    }
    output.AddMonomial(buffer, coeffBuff);
    tempSel.IncrementReturnFalseIfPastLast();
  }
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::LieBracketOnTheLeftMakeReport(const ElementWeylAlgebra& standsOnTheLeft) {
  this->LieBracketOnTheLeft(standsOnTheLeft, global);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::LieBracketOnTheRightMakeReport(const ElementWeylAlgebra& standsOnTheRight) {
  this->LieBracketOnTheRight(standsOnTheRight, global);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::LieBracket(
  const ElementWeylAlgebra& left, const ElementWeylAlgebra& right, ElementWeylAlgebra& output
) {
  if (&output == &right || &output == &left) {
    ElementWeylAlgebra leftCopy = left;
    ElementWeylAlgebra rightCopy = right;
    ElementWeylAlgebra::LieBracket(leftCopy, rightCopy, output);
    return;
  }
  output = right;
  output.MultiplyOnTheLeft(left);
  ElementWeylAlgebra subtracand;
  subtracand = left;
  subtracand.MultiplyOnTheLeft(right);
  output -= subtracand;
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::LieBracketOnTheLeft(const ElementWeylAlgebra& standsOnTheLeft) {
  ElementWeylAlgebra tempEl1, tempEl2;
  tempEl1 = *this;
  tempEl1.MultiplyOnTheLeft(standsOnTheLeft);
  //tempEl1.ComputeDebugString(false);
  tempEl2 = standsOnTheLeft;
  tempEl2.MultiplyOnTheLeft(*this);
  //tempEl2.ComputeDebugString(false);
  *this = tempEl1;
  *this -= tempEl2;
  //this->ComputeDebugString(false);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::LieBracketOnTheRight(const ElementWeylAlgebra& standsOnTheRight) {
  ElementWeylAlgebra tempEl1, tempEl2;
  tempEl1 = standsOnTheRight;
  tempEl1.MultiplyOnTheLeft(*this);
  tempEl2 = *this;
  tempEl2.MultiplyOnTheLeft(standsOnTheRight, global);
  *this = tempEl1;
  *this -= tempEl2;
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::MultiplyOnTheLeft(const  ElementWeylAlgebra& standsOnTheLeft) {
  ElementWeylAlgebra buffer;
  ElementWeylAlgebra Accum;
  Accum.makeZero();
  coefficient currentCF;
  for (int j = 0; j < standsOnTheLeft.size(); j ++) {
    for (int i = 0; i < this->size(); i ++) {
      this->MultiplyTwoMonomials(standsOnTheLeft[j], (*this)[i], buffer);
      currentCF = standsOnTheLeft.coefficients[j];
      currentCF *= this->coefficients[i];
      buffer *= currentCF;
      Accum += buffer;
    }
  }
  *this = Accum;
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::operator*=(const ElementWeylAlgebra& standsOnTheRight) {
  ElementWeylAlgebra buffer;
  ElementWeylAlgebra Accum;
  Accum.makeZero();
  coefficient currentCF;
  for (int j = 0; j < standsOnTheRight.size(); j ++) {
    for (int i = 0; i < this->size(); i ++) {
      this->MultiplyTwoMonomials((*this)[i], standsOnTheRight[j], buffer);
      currentCF = this->coefficients[i];
      currentCF *= standsOnTheRight.coefficients[j];
      buffer *= currentCF;
      Accum += buffer;
    }
  }
  *this = Accum;
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::raiseToPower(int thePower) {
  ElementWeylAlgebra WeylOne;
  WeylOne.makeOne(this->minimalNumberOfVariables());
  MathRoutines::raiseToPower(*this, thePower, WeylOne);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::MakeGEpsPlusEpsInTypeD(int i, int j, int NumVars) {
  this->makeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.makeOne(NumVars);
  tempMon.polynomialPart[i] = 1;
  tempMon.differentialPart[j + NumVars] = 1;
  this->AddMonomial(tempMon, 1);
  tempMon.makeOne(NumVars);
  tempMon.polynomialPart[j] = 1;
  tempMon.differentialPart[i + NumVars] = 1;
  this->AddMonomial(tempMon, 1);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::MakeGEpsMinusEpsInTypeD(int i, int j, int NumVars) {
  this->makeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.makeOne(NumVars);
  tempMon.polynomialPart[i] = 1;
  tempMon.differentialPart[j] = 1;
  this->AddMonomial(tempMon, 1);
  tempMon.makeOne(NumVars);
  tempMon.polynomialPart[j + NumVars] = 1;
  tempMon.differentialPart[i + NumVars] = 1;
  this->AddMonomial(tempMon, 1);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::MakeGMinusEpsMinusEpsInTypeD(int i, int j, int NumVars) {
  this->makeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.makeOne(NumVars);
  tempMon.polynomialPart[i + NumVars] = 1;
  tempMon.differentialPart[j] = 1;
  this->AddMonomial(tempMon, 1);
  tempMon.makeOne(NumVars);
  tempMon.polynomialPart[j + NumVars] = 1;
  tempMon.differentialPart[i] = 1;
  this->AddMonomial(tempMon, 1);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::Makedidj(int i, int j, int NumVars) {
  this->makeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.makeOne(NumVars);
  tempMon.differentialPart[i] = 1;
  tempMon.differentialPart[j] = 1;
  this->AddMonomial(tempMon, 1);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::Makexixj(int i, int j, int NumVars) {
  this->makeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.makeOne(NumVars);
  tempMon.polynomialPart[i] = 1;
  tempMon.polynomialPart[j] = 1;
  this->AddMonomial(tempMon, 1);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::Makexi(int i, int NumVars) {
  this->makeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.makeOne(NumVars);
  tempMon.polynomialPart.setVariable(i, 1);
  this->AddMonomial(tempMon, 1);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::Makedi(int i, int NumVars) {
  this->makeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.makeOne(NumVars);
  tempMon.differentialPart.setVariable(i, 1);
  this->AddMonomial(tempMon, 1);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::Makexidj(int i, int j, int NumVars) {
  this->makeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.makeOne(NumVars);
  tempMon.polynomialPart.setVariable(i, 1);
  tempMon.differentialPart.setVariable(j, 1);
  this->AddMonomial(tempMon, 1);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::GetStandardOrderDiffOperatorCorrespondingToNraisedTo(
  const Rational& inputRationalPower, int indexVar, ElementWeylAlgebra& outputDO, Polynomial<Rational>& outputDenominator
) {
  outputDenominator.makeOne();
  MonomialWeylAlgebra tempMon;
  outputDO.makeZero();
  int inputPower = 0;
  if (!inputRationalPower.IsSmallInteger(&inputPower)) {
    global.fatal << "This is a programming error: "
    << " I can give you a differential operator only from integer exponent. " << global.fatal;
  }
  if (inputPower >= 0) {
    tempMon.polynomialPart.MakeEi(indexVar, inputPower);
  } else {
    tempMon.differentialPart.MakeEi(indexVar, - inputPower);
  }
  outputDO.AddMonomial(tempMon, 1);
  inputPower *= - 1;
  Polynomial<Rational> newMult;
  newMult.makeDegreeOne(0, indexVar, 1);
  for (int i = 0; i < inputPower; i ++) {
    outputDenominator *= newMult;
    newMult -= 1;
  }
  //output/= den;
}

template <class coefficient>
bool ElementWeylAlgebra<coefficient>::Substitution(
  const PolynomialSubstitution<Rational>& SubPolyPart, const PolynomialSubstitution<Rational>& SubDiffPArt
) {
  MacroRegisterFunctionWithName("ElementWeylAlgebra::Substitution");
  Polynomial<Rational> DOpart, polyPart;
  MonomialWeylAlgebra theMon;
  ElementWeylAlgebra output;
  output.makeZero();
  coefficient theNewCoeff;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialWeylAlgebra& currentMon = (*this)[i];
    if (!currentMon.polynomialPart.Substitution(SubPolyPart, polyPart)) {
      return false;
    }
    if (!currentMon.differentialPart.Substitution(SubDiffPArt, DOpart)) {
      return false;
    }
    for (int j = 0; j < polyPart.size(); j ++) {
      for (int k = 0; k < DOpart.size(); k ++) {
        theMon.polynomialPart = polyPart[j];
        theMon.differentialPart = DOpart[k];
        theNewCoeff = this->coefficients[i];
        theNewCoeff *= polyPart.coefficients[j];
        theNewCoeff *= DOpart.coefficients[k];
        output.AddMonomial(theMon, theNewCoeff);
      }
    }
  }
  *this = output;
  return true;
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::FourierTransform(ElementWeylAlgebra<coefficient>& output) const {
  MacroRegisterFunctionWithName("ElementWeylAlgebra::FourierTransformMe");
  if (&output == this) {
    ElementWeylAlgebra thisCopy = *this;
    thisCopy.FourierTransform(output);
    return;
  }
  LargeInteger totalDeg;
  coefficient theCoeff;
  output.makeZero();
  MonomialWeylAlgebra theMon;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialWeylAlgebra& currentMon = (*this)[i];
    if (!(currentMon.polynomialPart.TotalDegree() + currentMon.differentialPart.TotalDegree()).IsInteger(&totalDeg)) {
      global.fatal << "This is a programming error: calling Fourier transoform "
      << "on differential operator with non-integral exponents. " << global.fatal;
    }
    theMon.differentialPart = currentMon.polynomialPart;
    theMon.polynomialPart = currentMon.differentialPart;
    theCoeff = this->coefficients[i];
    if (totalDeg.IsEven()) {
      theCoeff *= - 1;
    }
    output.AddMonomial(theMon, theCoeff);
  }
}

template <class coefficient>
bool ElementWeylAlgebra<coefficient>::ActOnPolynomial(Polynomial<Rational>& thePoly) const {
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
        if (!currentOpMon.differentialPart(k).IsSmallInteger(&numDiff)) {
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
      result.AddMonomial(resultMon, coeff);
    }
  }
  thePoly = result;
  return true;
}

#endif
