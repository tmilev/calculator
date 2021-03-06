// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfImplementationHeaderPolynomialComputationsBasic_already_included
#define vpfImplementationHeaderPolynomialComputationsBasic_already_included

#include "math_general.h"

template <class Coefficient>
bool MonomialPolynomial::substitution(
  const List<Polynomial<Coefficient> >& theSubstitution,
  Polynomial<Coefficient>& output,
  const Coefficient& one
) const {
  MacroRegisterFunctionWithName("MonomialPolynomial::substitution");
  output.makeConstant(one);
  if (this->isConstant()) {
    return true;
  }
  Polynomial<Coefficient> tempPoly;
  for (int i = 0; i < this->monomialBody.size; i ++) {
    if (this->monomialBody[i] == 0) {
      continue;
    }
    if (i >= theSubstitution.size) {
      global.fatal << "Attempt to carry out a substitution in the monomial "
      << this->toString()
      << " which does have non-zero exponent of variable x_{"
      << i + 1 << "}; however, the input substitution has "
      << theSubstitution.size
      << " variable images. More precisely, the input substitution is:  "
      << theSubstitution.toString() << ". " << global.fatal;
    }
    int theExponent = 0;
    if (!this->monomialBody[i].isSmallInteger(&theExponent) || this->monomialBody[i] < 0) {
      if (theSubstitution[i].isMonomialCoefficientOne()) {
        MonomialPolynomial tempMon = theSubstitution[i][0];
        tempMon.raiseToPower(this->monomialBody[i]);
        output *= tempMon;
        continue;
      }
      global.comments << "This may or may not be a programming error. "
      << "I cannot carry out a substitution in a monomial that has exponent "
      << "which is not a small integer: it is " << this->monomialBody[i]
      << " instead. " << GlobalVariables::Crasher::getStackTraceEtcErrorMessageHTML();
      return false;
    }
    tempPoly = theSubstitution[i];
    tempPoly.raiseToPower(theExponent, one);
    output *= tempPoly;
  }
  return true;
}

template<class Coefficient>
bool Polynomial<Coefficient>::isOneVariableNonConstantPolynomial(int* whichVariable) const {
  int tempInt;
  if (whichVariable == nullptr) {
    whichVariable = &tempInt;
  }
  if (!this->isOneVariablePolynomial(whichVariable)) {
    return false;
  }
  return *whichVariable != - 1;
}

template<class Coefficient>
Polynomial<Coefficient> Polynomial<Coefficient>::one() const {
  Polynomial<Coefficient> result;
  result.makeOne();
  return result;
}

template<class Coefficient>
Polynomial<Coefficient> Polynomial<Coefficient>::zero() {
  Polynomial<Coefficient> result;
  result.makeZero();
  return result;
}

template<class Coefficient>
Rational Polynomial<Coefficient>::rationalValue() {
  Rational result;
  this->getConstantTerm(result, 0);
  return result;
}

template<class Coefficient>
bool Polynomial<Coefficient>::isOneVariablePolynomial(int* whichVariable) const {
  int tempInt;
  if (whichVariable == nullptr) {
    whichVariable = &tempInt;
  }
  *whichVariable = - 1;
  for (int i = 0; i < this->size(); i ++) {
    for (int j = 0; j < (*this)[i].minimalNumberOfVariables(); j ++) {
      if ((*this)[i](j) != 0) {
        if (*whichVariable == - 1) {
          *whichVariable = j;
        } else if (*whichVariable != j) {
          return false;
        }
      }
    }
  }
  return true;
}

template <class Coefficient>
void Polynomial<Coefficient>::makeDeterminantFromSquareMatrix(
  const Matrix<Polynomial<Coefficient> >& theMat
) {
  if (theMat.numberOfColumns != theMat.numberOfRows) {
    global.fatal << "Cannot compute determinant: matrix has "
    << theMat.numberOfRows << " rows and " << theMat.numberOfColumns
    << " columns. " << global.fatal;
  }
  Permutation thePerm;
  thePerm.initPermutation(theMat.numberOfRows);
  int numCycles = thePerm.getNumberOfPermutations();
  List<int> permutationIndices;
  thePerm.getPermutationLthElementIsTheImageofLthIndex(permutationIndices);
  Polynomial<Coefficient> result, theMonomial;
  result.makeZero();
  result.setExpectedSize(numCycles);
  for (int i = 0; i < numCycles; i ++, thePerm.incrementAndGetPermutation(permutationIndices)) {
    theMonomial.makeOne();
    for (int j = 0; j < permutationIndices.size; j ++) {
      theMonomial *= theMat(j, permutationIndices[j]);
    }
    // The following can be made much faster, but no need right now as it won't be a bottleneck.
    int sign = 1;
    for (int j = 0; j < permutationIndices.size; j ++) {
      for (int k = j + 1; k < permutationIndices.size; k ++) {
        if (permutationIndices[k] < permutationIndices[j]) {
          sign *= - 1;
        }
      }
    }
    theMonomial *= sign;
    result += theMonomial;
  }
  *this = result;
}

template <class Coefficient>
int Polynomial<Coefficient>::totalDegreeInt() const {
  int result = - 1;
  if (!this->totalDegree().isSmallInteger(&result)) {
    global.fatal
    << "Requested total degree of a "
    << "polynomial in int formal, but the "
    << "degree of the polynomial is not a small integer. "
    << global.fatal;
  }
  return result;
}

template <class Coefficient>
Rational Polynomial<Coefficient>::totalDegree() const {
  Rational result = 0;
  for (int i = 0; i < this->size(); i ++) {
    result = MathRoutines::maximum((*this)[i].totalDegree(), result);
  }
  return result;
}

template <class Coefficient>
bool Polynomial<Coefficient>::substitution(
  const List<Polynomial<Coefficient> >& substitution,
  const Coefficient& one
) {
  MacroRegisterFunctionWithName("Polynomial::substitution");
  Polynomial<Coefficient> sum, monomialContribution;
  for (int i = 0; i < this->size(); i ++) {
    if (!(*this)[i].substitution(substitution, monomialContribution, one)) {
      return false;
    }
    monomialContribution *= this->coefficients[i];
    sum += monomialContribution;
  }
  *this = sum;
  return true;
}

template <class Coefficient>
void Polynomial<Coefficient>::makeOne() {
  this->makeConstant(1);
}

template <class Coefficient>
void Polynomial<Coefficient>::makeDegreeOne(
  int numberOfVariables,
  int nonZeroIndex,
  const Coefficient& coefficient
) {
  this->makeZero();
  MonomialPolynomial tempM;
  tempM.makeEi(nonZeroIndex, 1, numberOfVariables);
  this->addMonomial(tempM, coefficient);
}

template <class Coefficient>
void Polynomial<Coefficient>::makeDegreeOne(
  int numberOfVariables,
  int nonZeroIndex1,
  int nonZeroIndex2,
  const Coefficient& coefficient1,
  const Coefficient& coefficient2
) {
  (void) numberOfVariables;
  this->makeZero();
  MonomialPolynomial tempM;
  tempM.makeEi(nonZeroIndex1);
  this->addMonomial(tempM, coefficient1);
  tempM.makeEi(nonZeroIndex2);
  this->addMonomial(tempM, coefficient2);
}

template <class Coefficient>
void Polynomial<Coefficient>::makeDegreeOne(
  int numberOfVariables,
  int nonZeroIndex,
  const Coefficient& coefficient1,
  const Coefficient& constantTerm
) {
  this->makeDegreeOne(numberOfVariables, nonZeroIndex, coefficient1);
  *this += constantTerm;
}

template <class Coefficient>
Coefficient Polynomial<Coefficient>::evaluate(const Vector<Coefficient>& input, const Coefficient& zero) {
  MacroRegisterFunctionWithName("Polynomial::evaluate");
  Coefficient output = zero;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialPolynomial& currentMon = (*this)[i];
    Coefficient accum = this->coefficients[i];
    Coefficient tempElt;
    for (int j = 0; j < currentMon.minimalNumberOfVariables(); j ++) {
      int numCycles = 0;
      if (!(*this)[i](j).isSmallInteger(&numCycles)) {
        global.fatal
        << "Attempting to evaluate a polynomial whose "
        << i + 1 << "^{th} variable is raised to the power "
        << (*this)[i](j).toString()
        << ". Raising variables to power is allowed "
        << "only if the power is a small integer. "
        << "If the user has requested such an operation, "
        << "it *must* be intercepted at an earlier level "
        << "(and the user must be informed)." << global.fatal;
      }
      bool isPositive = numCycles > 0;
      if (numCycles < 0) {
        numCycles = - numCycles;
      }
      tempElt = input[j];
      MathRoutines::raiseToPower(tempElt, numCycles, zero.one());
      if (!isPositive) {
        tempElt.invert();
      }
      accum *= tempElt;
    }
    output += accum;
  }
  return output;
}

template <class Coefficient>
void Polynomial<Coefficient>::setNumberOfVariablesSubstituteDeletedByOne(int newNumVars) {
  MacroRegisterFunctionWithName("Polynomial_CoefficientType::setNumberOfVariablesSubstituteDeletedByOne");
  if (newNumVars >= this->minimalNumberOfVariables()) {
    return;
  }
  if (newNumVars < 0) {
    global.fatal
    << "Requesting negative number of variables (more precisely, "
    << newNumVars << ") is not allowed. " << global.fatal;
  }
  Polynomial<Coefficient> accumulator;
  accumulator.makeZero();
  accumulator.setExpectedSize(this->size());
  MonomialPolynomial monomial;
  for (int i = 0; i < this->size(); i ++) {
    monomial.makeOne();
    for (int j = 0; j < newNumVars; j ++) {
      monomial.setVariable(j, (*this)[i](j));
    }
    accumulator.addMonomial(monomial, this->coefficients[i]);
  }
  this->operator=(accumulator);
}

template <class Coefficient>
bool Polynomial<Coefficient>::hasSmallIntegralPositivePowers(
  int* whichTotalDegree
) const {
  int maximum = 0;
  int current = 0;
  for (int i = 0; i < this->size(); i ++) {
    if (!this->monomials[i].hasSmallIntegralPositivePowers(&current)) {
      return false;
    }
    maximum = MathRoutines::maximum(maximum, current);
  }
  if (whichTotalDegree != nullptr) {
    *whichTotalDegree = maximum;
  }
  return true;
}

template <class Coefficient>
void Polynomial<Coefficient>::shiftVariableIndicesToTheRight(int VarIndexShift) {
  if (VarIndexShift < 0) {
    global.fatal
    << "Requesting negative variable shift (more precisely, "
    << VarIndexShift << ") not allowed. " << global.fatal;
  }
  if (VarIndexShift == 0) {
    return;
  }
  int oldNumVars = this->minimalNumberOfVariables();
  Polynomial<Coefficient> Accum;
  Accum.makeZero();
  Accum.setExpectedSize(this->size());
  MonomialPolynomial tempM;
  for (int i = 0; i < this->size(); i ++) {
    tempM.makeOne();
    for (int j = 0; j < oldNumVars; j ++) {
      tempM.setVariable(j + VarIndexShift, (*this)[i](j));
    }
    Accum.addMonomial(tempM, this->coefficients[i]);
  }
  *this = Accum;
}

template <class Coefficient>
bool Polynomial<Coefficient>::isEqualToOne() const {
  Coefficient tempC;
  if (this->isConstant(&tempC)) {
    return tempC.isEqualToOne();
  }
  return false;
}

template <class Coefficient>
bool Polynomial<Coefficient>::isMonomialCoefficientOne() const {
  if (this->size() != 1) {
    return false;
  }
  return this->coefficients[0].isEqualToOne();
}

template <class Coefficient>
bool Polynomial<Coefficient>::isOneLetterFirstDegree(int* whichLetter) const {
  if (this->size() != 1) {
    return false;
  }
  return (*this)[0].isOneLetterFirstDegree(whichLetter);
}

template <class Coefficient>
bool Polynomial<Coefficient>::isConstant(Coefficient* whichConstant) const {
  if (this->size() > 1) {
    return false;
  }
  if (this->size() == 0) {
    if (whichConstant != nullptr) {
      *whichConstant = 0;
    }
    return true;
  }
  if (whichConstant != nullptr) {
    *whichConstant = this->coefficients[0];
  }
  const MonomialPolynomial& theMon = (*this)[0];
  return theMon.isConstant();
}

template <class Coefficient>
bool Polynomial<Coefficient>::isNegative() const {
  Coefficient tempC;
  if (!this->isConstant(&tempC)) {
    return false;
  }
  return tempC.isNegative();
}

template <class Coefficient>
bool Polynomial<Coefficient>::isLinearNoConstantTerm() {
  for (int i = 0; i < this->size; i ++) {
    if (!this->objects[i].isLinearNoConstantTerm()) {
      return false;
    }
  }
  return true;
}

template <class Coefficient>
void SylvesterMatrix<Coefficient>::fillSylvesterMatrix(
  const Polynomial<Coefficient>& polynomial,
  int columnOffset,
  Matrix<Coefficient>& output
) {
  int totalPower = polynomial.totalDegreeInt();
  int numberOfColumns = output.numberOfRows - totalPower;
  for (int i = 0; i < polynomial.size(); i ++) {
    int power = polynomial.monomials[i].totalDegreeInt();
    const Coefficient& coefficient = polynomial.coefficients[i];
    for (int j = 0; j < numberOfColumns; j ++) {
      int row = totalPower - power + j;
      int column = j + columnOffset;
      output(row, column) = coefficient;
    }
  }
}

template <class Coefficient>
bool SylvesterMatrix<Coefficient>::sylvesterMatrixProduct(
  const List<Polynomial<Coefficient> >& polynomials,
  Matrix<Coefficient>& output,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("SylvesterMatrix::sylvesterMatrixProduct");
  LargeInteger totalPower = 0;
  for (int i = 0; i < polynomials.size; i ++) {
    Polynomial<Coefficient>& current = polynomials[i];
    if (current.minimalNumberOfVariables() > 1) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "I know how to compute Sylvester "
        << "matrix only for univariate polynomials. Polynomial number "
        << i + 1 << " has " << current.minimalNumberOfVariables()
        << " variables.";
      }
      return false;
    }
    if (current.isEqualToZero()) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "I don't have a Sylvester matrix "
        << "definition when one of the inputs is the zero polynomial. ";
      }
      return false;
    }
    int currentPower = 0;
    if (!current.hasSmallIntegralPositivePowers(&currentPower)) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Polynomial powers are not small, positive or integral. ";
      }
      return false;
    }
    totalPower += currentPower;
  }
  int maximumDimension = 1000;
  if (totalPower == 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "The calculator does not allow empty (0x0) "
      << "Sylvester matrix.";
    }
    return false;
  }
  if (totalPower > maximumDimension) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "The sylvester matrix is too large: "
      << totalPower << " by " << totalPower
      << ", the maximum allowed is: " << maximumDimension << " by "
      << maximumDimension << ". ";
    }
    return false;
  }
  int dimension = totalPower.getIntValueTruncated();
  List<Polynomial<Coefficient> > products;
  for (int i = 0; i < polynomials.size; i ++) {
    Polynomial<Coefficient> product;
    product.makeConstant(polynomials[0].coefficients[0].one());
    for (int j = 0; j < polynomials.size; j ++) {
      if (i == j) {
        continue;
      }
      product *= polynomials[j];
    }
    products.addOnTop(product);
  }
  return SylvesterMatrix<Coefficient>::sylvesterMatrixMulti(products, dimension, output);
}

template <class Coefficient>
bool SylvesterMatrix<Coefficient>::sylvesterMatrix(
  const Polynomial<Coefficient>& left,
  const Polynomial<Coefficient>& right,
  Matrix<Coefficient>& output,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Polynomial::sylvesterMatrix");
  List<Polynomial<Coefficient> > polynomials;
  polynomials.addOnTop(left);
  polynomials.addOnTop(right);
  return SylvesterMatrix::sylvesterMatrixProduct(polynomials, output, commentsOnFailure);
}


template <class Coefficient>
bool SylvesterMatrix<Coefficient>::sylvesterMatrixMulti(
  const List<Polynomial<Coefficient> >& polynomials,
  int dimension,
  Matrix<Coefficient>& output
) {
  MacroRegisterFunctionWithName("SylvesterMatrix::sylvesterMatrixMulti");
  output.initialize(dimension, dimension);
  output.makeZero(polynomials[0].coefficients[0].zero());
  int columnOffset = 0;
  for (int i = 0; i < polynomials.size; i ++) {
    SylvesterMatrix<Coefficient>::fillSylvesterMatrix(polynomials[i], columnOffset, output);
    columnOffset += output.numberOfRows - polynomials[i].totalDegreeInt();
  }
  return true;
}

template <class Coefficient>
bool Polynomial<Coefficient>::isLinear() {
  for (int i = 0; i < this->size(); i ++) {
    if (!(*this)[i].isLinear()) {
      return false;
    }
  }
  return true;
}

template <class Coefficient>
bool Polynomial<Coefficient>::isLinearGetRootConstantTermLastCoordinate(Vector<Coefficient>& outputRoot) {
  outputRoot.makeZero(this->minimalNumberOfVariables() + 1);
  int index;
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].isConstant()) {
      *outputRoot.lastObject() = this->coefficients[i];
    } else {
      if ((*this)[i].isOneLetterFirstDegree(&index)) {
        outputRoot[index] = this->coefficients[i];
      } else {
        return false;
      }
    }
  }
  return true;
}

template <class Coefficient>
void Polynomial<Coefficient>::raiseToPower(
  int d, const Coefficient& one
) {
  if (d == 1) {
    return;
  }
  if (d < 0) {
    global.fatal << "Attempt to raise the polynomial "
    << this->toString() << " to the negative power "
    << d << ". " << global.fatal;
  }
  Polynomial<Coefficient> onePolynomial;
  onePolynomial.makeConstant(one);
  MathRoutines::raiseToPower(*this, d, onePolynomial);
}

template <class Coefficient>
bool Polynomial<Coefficient>::getRootFromLinearPolynomialConstantTermLastVariable(Vector<Coefficient>& outputRoot) {
  return this->isLinearGetRootConstantTermLastCoordinate(outputRoot);
}

template <class Coefficient>
Matrix<Coefficient> Polynomial<Coefficient>::evaluateUnivariatePolynomial(
  const Matrix<Coefficient>& input
) {
  // for univariate polynomials only
  MacroRegisterFunctionWithName("Polynomial::evaluateUnivariatePolynomial");
  Matrix<Coefficient> output, tempElt, idMat;
  idMat.makeIdentityMatrix(input.numberOfColumns);
  output.makeZeroMatrix(input.numberOfColumns);
  for (int i = 0; i < this->size; i ++) {
    const MonomialPolynomial& currentMon = (*this)[i];
    int numCycles = 0;
    if (!currentMon(0).isSmallInteger(&numCycles) ) {
      global.fatal
      << "Attempting to evaluate a polynomial whose "
      << i + 1 << "^{th} variable is raised to the power "
      << currentMon(0).toString()
      << ". Raising variables to power is allowed "
      << "only if the power is a small integer. "
      << "If the user has requested such an operation, "
      << "it *must* be intercepted at an earlier level "
      << "(and the user must be informed)."
      << global.fatal;
    }
    bool isPositive = (numCycles > 0);
    if (numCycles < 0) {
      numCycles = - numCycles;
    }
    tempElt = input;
    MathRoutines::raiseToPower(tempElt, numCycles, idMat);
    if (!isPositive) {
      tempElt.invert();
    }
    tempElt *= this->coefficients[i];
    output += tempElt;
  }
  return output;
}

template <class Coefficient>
int Polynomial<Coefficient>::getHighestIndexSuchThatHigherIndexVariablesDontParticipate() {
  int result = - 1;
  for (int i = 0; i < this->size; i ++) {
    result = MathRoutines::maximum(
      result,
      this->objects[i].getHighestIndexSuchThatHigherIndexVariablesDontParticipate()
    );
  }
  return result;
}

template <class Coefficient>
void Polynomial<Coefficient>::scaleToPositiveMonomialExponents(MonomialPolynomial& outputScale) {
  int numVars = this->minimalNumberOfVariables();
  outputScale.makeOne();
  for (int i = 0; i < numVars; i ++) {
    for (int j = 0; j < this->size(); j ++) {
      const MonomialPolynomial& currentMonomial = (*this)[j];
      Rational currentScale = MathRoutines::minimum(outputScale(i), currentMonomial(i));
      outputScale.setVariable(i, currentScale);
    }
  }
  outputScale.invert();
  this->multiplyBy(outputScale);
}

template <class Coefficient>
bool Polynomial<Coefficient>::operator<=(const Coefficient& other) const {
  Coefficient constME;
  if (!this->isConstant(&constME)) {
    global.fatal << "This may or may not be a programming error: "
    << "attempting to compare a non-constant polynomial to "
    << "a constant. I cannot judge at the moment whether "
    << "allowing that is a good decision. "
    << "In any case, crashing to let you know. "
    << global.fatal;
    return false;
  }
  return constME <= other;
}

template <class Coefficient>
bool Polynomial<Coefficient>::operator<(const Coefficient& other) const {
  Coefficient constME;
  if (!this->isConstant(&constME)) {
    global.fatal << "This may or may not be a programming error: "
    << "attempting to compare a non-constant polynomial to "
    << "a constant. I cannot judge at the moment whether allowing "
    << "that is a good decision. In any case, crashing to let you know. "
    << global.fatal;
    return false;
  }
  return constME<other;
}

template <class Coefficient>
bool Polynomial<Coefficient>::operator>(const Polynomial<Coefficient>& other) const {
  if (other.size() == 0) {
    if (this->size() == 0) {
      return false;
    } else {
      return true;
    }
  }
  if (this->size() == 0) {
    return false;
  }
  if (this->totalDegree() > other.totalDegree()) {
    return true;
  }
  if (this->totalDegree() < other.totalDegree()) {
    return false;
  }
  MonomialPolynomial thisMaximalMonomial, otherMaximalMonomial;
  List<MonomialPolynomial>::Comparator& monomialOrder = MonomialPolynomial::orderDefault();
  int thisMaxMonIndex = this->getIndexLeadingMonomial(&thisMaximalMonomial, nullptr, &monomialOrder);
  int otherMaxMonIndex = other.getIndexLeadingMonomial(&otherMaximalMonomial, nullptr, &monomialOrder);
  if (thisMaximalMonomial > otherMaximalMonomial) {
    return true;
  }
  if (otherMaximalMonomial > thisMaximalMonomial) {
    return false;
  }
  if (this->coefficients[thisMaxMonIndex] > other.coefficients[otherMaxMonIndex]) {
    return true;
  }
  if (other.coefficients[otherMaxMonIndex] > this->coefficients[thisMaxMonIndex]) {
    return false;
  }
  return false;
}

template <class Coefficient>
bool Polynomial<Coefficient>::isGEQCompareByTopMonomialTotalDegThenLexicographic(
  const Polynomial<Coefficient>& left, const Polynomial<Coefficient>& right
) {
  if (left.isEqualToZero()) {
    return right.isEqualToZero();
  }
  if (right.isEqualToZero()) {
    return true;
  }
  return left[left.GetIndexMaxMonomialTotalDegThenLexicographic()].MonomialPolynomial::IsGEQTotalDegThenLexicographic(
    right[right.GetIndexMaxMonomialTotalDegThenLexicographic()]
  );
}

template <class Coefficient>
bool Polynomial<Coefficient>::isGEQCompareByTopMonomialLexicographicLastVarStrongest(
  const Polynomial<Coefficient>& left, const Polynomial<Coefficient>& right
) {
  if (left.isEqualToZero()) {
    return right.isEqualToZero();
  }
  if (right.isEqualToZero()) {
    return true;
  }
  int indexMaxLeft = left.GetIndexMaxMonomialLexicographicLastVariableStrongest();
  int indexMaxRight = right.GetIndexMaxMonomialLexicographicLastVariableStrongest();
  return left[indexMaxLeft].MonomialPolynomial::IsGEQLexicographicLastVariableStrongest(right[indexMaxRight]);
}

template <class Coefficient>
bool Polynomial<Coefficient>::isEqualTo(const Polynomial<Coefficient>& p) const {
  return *this == p;
}

template <class Coefficient>
Polynomial<Coefficient> Polynomial<Coefficient>::operator+(const Polynomial<Coefficient>& other) const {
  Polynomial<Coefficient> copy = *this;
  copy += other;
  return copy;
}

template <class Coefficient>
Polynomial<Coefficient> Polynomial<Coefficient>::operator-(const Polynomial<Coefficient>& other) const {
  Polynomial<Coefficient> copy = *this;
  copy -= other;
  return copy;
}

template <class Coefficient>
void Polynomial<Coefficient>::operator-=(int x) {
  MonomialPolynomial tempMon;
  tempMon.makeOne();
  this->subtractMonomial(tempMon, x);
}

template <class Coefficient>
void Polynomial<Coefficient>::operator-=(const Coefficient& other) {
  MonomialPolynomial tempMon;
  tempMon.makeOne();
  this->subtractMonomial(tempMon, other);
}

template <class Coefficient>
void Polynomial<Coefficient>::operator-=(const Polynomial<Coefficient>& other) {
  this->::LinearCombination<MonomialPolynomial, Coefficient>::operator-=(other);
}

template <class Coefficient>
void Polynomial<Coefficient>::operator*=(const MonomialPolynomial& other) {
  this->multiplyBy(other, *this);
}

template <class Coefficient>
void Polynomial<Coefficient>::operator*=(const Polynomial<Coefficient>& other) {
  this->::ElementMonomialAlgebra<MonomialPolynomial, Coefficient>::operator*=(other);
}

template <class Coefficient>
Polynomial<Coefficient> Polynomial<Coefficient>::operator%(const Polynomial<Coefficient>& other) {
  Polynomial<Coefficient> temp;
  Polynomial<Coefficient> result;
  this->divideBy(other, temp, result, &MonomialPolynomial::orderDefault());
  return result;
}

template <class Coefficient>
void Polynomial<Coefficient>::operator/=(const Polynomial<Coefficient>& other) {
  Polynomial<Coefficient> tempMe = *this;
  Polynomial<Coefficient> tempRemainder;
  tempMe.divideBy(other, *this, tempRemainder, &MonomialPolynomial::orderDefault());
}

template <class Coefficient>
void Polynomial<Coefficient>::operator/=(int other) {
  this->::LinearCombination<MonomialPolynomial, Coefficient>::operator/=(other);
}

template <class Coefficient>
void Polynomial<Coefficient>::operator/=(const Coefficient& other) {
  this->::LinearCombination<MonomialPolynomial, Coefficient>::operator/=(other);
}

template <class Coefficient>
template <class otherType>
void Polynomial<Coefficient>::operator*=(const otherType& other) {
  this->::LinearCombination<MonomialPolynomial, Coefficient>::operator*= (other);
}

template <class Coefficient>
void Polynomial<Coefficient>::operator=(const Polynomial<Coefficient>& other) {
  this->::LinearCombination<MonomialPolynomial, Coefficient>::operator=(other);
}

template <class Coefficient>
template<class otherType>
void Polynomial<Coefficient>::operator=(const Polynomial<otherType>& other) {
  this->::LinearCombination<MonomialPolynomial, Coefficient>::operator=(other);
}

template <class Coefficient>
void Polynomial<Coefficient>::operator=(const Coefficient& other) {
  this->makeConstant(other);
}

template <class Coefficient>
void Polynomial<Coefficient>::operator=(int other) {
  Coefficient tempCF;
  tempCF = other;
  this->makeConstant(tempCF);
}

template <class Coefficient>
template <class otherType>
void Polynomial<Coefficient>::assignOtherType(const Polynomial<otherType>& other) {
  this->NumVars = other.NumVars;
  this->::LinearCombination<MonomialPolynomial, Coefficient>::assignOtherType(other);
}

template <class Coefficient>
bool Polynomial<Coefficient>::operator<=(const Polynomial<Coefficient>& other) const {
  return !(*this > other);
}

template <class Coefficient>
bool Polynomial<Coefficient>::isProportionalTo(
  const Polynomial<Coefficient>& other,
  Coefficient& outputTimesMeEqualsOther,
  const Coefficient& ringUnit
) const {
  if (this->size() != other.size()) {
    return false;
  }
  if (other.size() == 0) {
    outputTimesMeEqualsOther = ringUnit;
    return true;
  }
  const MonomialPolynomial& firstMon = (*this)[0];
  int indexInOther = other.monomials.getIndex(firstMon);
  if (indexInOther == - 1) {
    return false;
  }
  outputTimesMeEqualsOther = other.coefficients[indexInOther];
  outputTimesMeEqualsOther /= this->coefficients[0];
  Polynomial<Coefficient> tempP;
  tempP = *this;
  tempP *= outputTimesMeEqualsOther;
  tempP -= other;
  return tempP.isEqualToZero();
}

template <class Coefficient>
void Polynomial<Coefficient>::divideBy(
  const Polynomial<Coefficient>& inputDivisor,
  Polynomial<Coefficient>& outputQuotient,
  Polynomial<Coefficient>& outputRemainder,
  typename List<MonomialPolynomial>::Comparator* monomialOrder
) const {
  MacroRegisterFunctionWithName("Polynomial::divideBy");
  if (monomialOrder == nullptr) {
    monomialOrder = &MonomialPolynomial::orderDefault();
  }
  if (&outputQuotient == &outputRemainder) {
    global.fatal << "Input and output addresses not allowed to coincide. " << global.fatal;
  }
  if (inputDivisor.isEqualToZero()) {
    global.fatal << "Division by zero polynomial. " << global.fatal;
  }
  if (
    &outputRemainder == this ||
    &outputQuotient == this ||
    &outputRemainder == &inputDivisor ||
    &outputQuotient == &inputDivisor
  ) {
    Polynomial<Coefficient> newQuotient, newRemainder;
    this->divideBy(inputDivisor, newQuotient, newRemainder, monomialOrder);
    outputQuotient = newQuotient;
    outputRemainder = newRemainder;
    return;
  }
  outputRemainder = *this;
  MonomialPolynomial scaleRemainder, scaleInput;
  Polynomial<Coefficient> divisorShiftedExponents = inputDivisor;
  outputRemainder.scaleToPositiveMonomialExponents(scaleRemainder);
  divisorShiftedExponents.scaleToPositiveMonomialExponents(scaleInput);
  MonomialPolynomial remainderLeadingMonomial;
  Coefficient remainderLeadingCoefficient;
  int remainderLeadingIndex = outputRemainder.getIndexLeadingMonomial(
    &remainderLeadingMonomial, &remainderLeadingCoefficient, monomialOrder
  );
  MonomialPolynomial leadingMonomialShiftedDivisor;
  Coefficient leadingCoefficientShiftedDivisor;
  divisorShiftedExponents.getIndexLeadingMonomial(
    &leadingMonomialShiftedDivisor,
    &leadingCoefficientShiftedDivisor,
    monomialOrder
  );
  outputQuotient.makeZero();
  if (remainderLeadingIndex == - 1) {
    return;
  }
  outputQuotient.setExpectedSize(this->size());
  MonomialPolynomial quotientMonomial;
  Polynomial<Coefficient> subtracand;
  subtracand.setExpectedSize(this->size());
  while (monomialOrder->greaterThanOrEqualTo(remainderLeadingMonomial, leadingMonomialShiftedDivisor)) {
    quotientMonomial = remainderLeadingMonomial;
    quotientMonomial /= leadingMonomialShiftedDivisor;
    if (!quotientMonomial.hasPositiveOrZeroExponents()) {
      break;
    }
    Coefficient quotientCoefficient = remainderLeadingCoefficient;
    quotientCoefficient /= leadingCoefficientShiftedDivisor;
    outputQuotient.addMonomial(quotientMonomial, quotientCoefficient);
    subtracand = divisorShiftedExponents;
    subtracand.multiplyBy(quotientMonomial, quotientCoefficient);
    outputRemainder -= subtracand;
    int remainderIndex = outputRemainder.getIndexLeadingMonomial(
      &remainderLeadingMonomial,
      &remainderLeadingCoefficient,
      monomialOrder
    );
    if (remainderIndex == - 1) {
      break;
    }
  }
  scaleInput.invert();
  outputQuotient.multiplyBy(scaleInput);
  outputQuotient.multiplyBy(scaleRemainder);
  outputRemainder.multiplyBy(scaleRemainder);
}

template <class Coefficient>
void Polynomial<Coefficient>::assignCharacteristicPolynomial(const Matrix<Coefficient>& input) {
  MacroRegisterFunctionWithName("Polynomial::assignCharacteristicPolynomial");
  if (input.numberOfColumns != input.numberOfRows) {
    global.fatal << "Request the "
    << "minimimal polynomial of a non-square matrix. "
    << global.fatal;
  }
  int n = input.numberOfColumns;
  if (n == 0) {
    global.fatal
    << "At present, the characteristic polynomial of "
    << "a 0x0 matrix is not defined. "
    << "Crashing to let you know. If you think this "
    << "should be changed, document why and "
    << "modify the present assertion. " << global.fatal;
  }
  this->makeConstant(1);
  Matrix<Coefficient> acc = input;
  Coefficient currenCF;
  for (int i = 1; i < n; i ++) {
    currenCF = - acc.getTrace() / i;
    this->addMonomial(MonomialPolynomial(0, i), currenCF);
    for (int j = 0; j < n; j ++) {
      acc(j, j) += currenCF;
    }
    acc.multiplyOnTheLeft(input);
  }
  this->addMonomial(MonomialPolynomial(0, n), - acc.getTrace() / n);
}

template <class Coefficient>
void Polynomial<Coefficient>::assignMinimalPolynomial(const Matrix<Coefficient>& input) {
  MacroRegisterFunctionWithName("Polynomial::assignMinimalPolynomial");
  if (input.numberOfColumns != input.numberOfRows) {
    global.fatal << "Request the "
    << "minimimal polynomial of a non-square matrix. "
    << global.fatal;
  }
  int theDim = input.numberOfColumns;
  this->makeOne();
  Vectors<Coefficient> theBasis;
  Vector<Coefficient> theVectorPowers;
  Vector<Coefficient> firstDependentPower;
  Polynomial<Coefficient> currentFactor;
  MonomialPolynomial tempM;
  for (int col = 0; col < theDim; col ++) {
    theVectorPowers.makeEi(theDim,col);
    theBasis.setSize(0);
    theBasis.addOnTop(theVectorPowers);
    for (int i = 0; i < theDim; i ++) {
      input.actOnVectorColumn(*theBasis.lastObject(), theVectorPowers, Coefficient::zero());
      if (theBasis.linearSpanContainsVector(theVectorPowers)) {
        break;
      }
      theBasis.addOnTop(theVectorPowers);
    }
    theVectorPowers.getCoordinatesInBasis(theBasis, firstDependentPower);
    currentFactor.setExpectedSize(theBasis.size + 1);
    currentFactor.makeZero();
    for (int i = 0; i < theBasis.size; i ++) {
      tempM.makeEi(0, i, 1);
      currentFactor.addMonomial(tempM, - firstDependentPower[i]);
    }
    tempM.makeEi(0, theBasis.size, 1);
    currentFactor.addMonomial(tempM, 1);
    *this = MathRoutines::leastCommonMultiple(*this, currentFactor);
  }
  this->scaleNormalizeLeadingMonomial(&MonomialPolynomial::orderDefault());
}

template <class Coefficient>
int Polynomial<Coefficient>::getMaximumPowerOfVariableIndex(int VariableIndex) {
  int result = 0;
  for (int i = 0; i < this->size(); i ++) {
    result = MathRoutines::maximum(result, (*this)[i](VariableIndex).numeratorShort);
    if (!(*this)[i](VariableIndex).isSmallInteger()) {
      global.fatal << "Function getMaximumPowerOfVariableIndex "
      << "called on a polynomial whose monomials "
      << "have degrees that are not small integers. "
      << "This needs to be fixed! " << global.fatal;
    }
  }
  return result;
}

template <class Coefficient>
void Polynomial<Coefficient>::interpolate(
  const Vector<Coefficient>& thePoints, const Vector<Coefficient>& valuesAtThePoints
) {
  Polynomial<Coefficient> lagrangeInterpolator, accumulator;
  this->makeZero();
  for (int i = 0; i < thePoints.size; i ++) {
    lagrangeInterpolator.makeConstant(1);
    for (int j = 0; j < thePoints.size; j ++) {
      if (i == j) {
        continue;
      }
      accumulator.makeDegreeOne(1, 0, 1, - thePoints[j]);
      accumulator /= thePoints[i] - thePoints[j];
      lagrangeInterpolator *= accumulator;
    }
    lagrangeInterpolator *= valuesAtThePoints[i];
    *this += lagrangeInterpolator;
  }
}

template <class Coefficient>
Coefficient Polynomial<Coefficient>::getDiscriminant() {
  MacroRegisterFunctionWithName("Polynomial::getDiscriminant");
  if (this->minimalNumberOfVariables() > 1) {
    global.fatal
    << "I do not have a definition of discriminant "
    << "for more than one variable. "
    << global.fatal;
  }
  if (this->totalDegree() != 2) {
    global.fatal
    << "Discriminant not implemented for polynomial "
    << "of degree not equal to 1."
    << global.fatal;
  }
  Coefficient a = this->getCoefficientOf(MonomialPolynomial(0, 2));
  Coefficient b = this->getCoefficientOf(MonomialPolynomial(0, 1));
  Coefficient c = this->getCoefficientOf(MonomialPolynomial(0, 0));
  return b * b - a * c * 4;
}

template <class Coefficient>
void Polynomial<Coefficient>::getConstantTerm(Coefficient& output, const Coefficient& ringZero) const {
  MonomialPolynomial tempM;
  tempM.makeOne();
  int i = this->monomials.getIndex(tempM);
  if (i == - 1) {
    output = ringZero;
  } else {
    output = this->coefficients[i];
  }
}

template <class Coefficient>
void Polynomial<Coefficient>::makeMonomial(
  int letterIndex,
  const Rational& power,
  const Coefficient& inputCoefficient
) {
  if (letterIndex < 0) {
    global.fatal << "Negative variable index: "
    << letterIndex << " is not allowed. "
    << global.fatal;
  }
  this->makeZero();
  MonomialPolynomial variable;
  variable.makeOne();
  variable.setVariable(letterIndex, power);
  this->addMonomial(variable, inputCoefficient);
}

template <class Coefficient>
void Polynomial<Coefficient>::getCoefficientInFrontOfLinearTermVariableIndex(
  int index, Coefficient& output
) {
  MonomialPolynomial tempM;
  tempM.makeEi(index);
  int i = this->monomials.getIndex(tempM);
  if (i == - 1) {
    output = 0;
  } else {
    output = this->coefficients[i];
  }
}

template<class Coefficient>
bool Polynomial<Coefficient>::isSquareFree(
  const Coefficient& one, std::stringstream* comments
) const {
  MacroRegisterFunctionWithName("Polynomial::isSquareFree");
  Vector<Polynomial<Coefficient> > differentials;
  if (this->isConstant()) {
    return false;
  }
  if (!this->differential(differentials, comments)) {
    return false;
  }
  Polynomial<Coefficient> divisor = *this;
  for (int i = 0; i < differentials.size; i ++) {
    if (differentials[i].isEqualToZero()) {
      continue;
    }
    Polynomial::greatestCommonDivisor(divisor, differentials[i], divisor, one, comments);
  }
  return divisor.totalDegree() == 0;
}

template<class Coefficient>
bool Polynomial<Coefficient>::differential(
  Vector<Polynomial<Coefficient> >& output, std::stringstream* comments
) const {
  MacroRegisterFunctionWithName("Polynomial::differential");
  int numberOfVariables = this->minimalNumberOfVariables();
  output.setSize(numberOfVariables);
  for (int i = 0; i < this->size(); i ++) {
    const MonomialPolynomial& currentMonomial = this->monomials[i];
    const Coefficient& currentCoefficient = this->coefficients[i];
    int currentNumberOfVariables = currentMonomial.minimalNumberOfVariables();
    for (int j = 0; j < currentNumberOfVariables; j ++) {
      Coefficient newCoefficient = currentCoefficient;
      Rational power = currentMonomial(j);
      LargeInteger powerInteger;
      if (!power.isInteger(&powerInteger)) {
        if (comments != nullptr) {
          *comments << "Monomial has non-integer power " << power << ". ";
        }
        return false;
      }
      newCoefficient *= power;
      MonomialPolynomial newMonomial = currentMonomial;
      power -= 1;
      newMonomial.setVariable(j, power);
      output[j].addMonomial(newMonomial, newCoefficient);
    }
  }
  return true;
}

template<class Coefficient>
bool Polynomial<Coefficient>::differential(
  Polynomial<Coefficient>& output,
  std::stringstream* comments
) const {
  MacroRegisterFunctionWithName("Polynomial::differential");
  if (&output == this) {
    Polynomial<Coefficient> thisCopy = *this;
    return thisCopy.differential(output, comments);
  }
  Vector<Polynomial<Coefficient> > differentials;
  if (!this->differential(differentials, comments)) {
    return false;
  }
  output.makeZero();
  Polynomial<Coefficient> nextSummand;
  for (int i = 0; i < differentials.size; i ++) {
    nextSummand = differentials[i];
    MonomialPolynomial differentialVariable(differentials.size + i, 1);
    nextSummand *= differentialVariable;
    output += nextSummand;
  }
  return true;
}

template <class Coefficient>
bool PolynomialOrder<Coefficient>::compareLeftGreaterThanRight(
  const Polynomial<Coefficient>& left, const Polynomial<Coefficient>& right
) const {
  MacroRegisterFunctionWithName("PolynomialOrder::compareLeftGreaterThanRight");
  List<MonomialPolynomial> sortedLeft = left.monomials;
  List<MonomialPolynomial> sortedRight = right.monomials;
  sortedLeft.quickSortAscending(&this->monomialOrder);
  sortedRight.quickSortAscending(&this->monomialOrder);
  int leftIndex = sortedLeft.size - 1;
  int rightIndex = sortedRight.size - 1;
  for (; leftIndex >= 0 && rightIndex >= 0; leftIndex --, rightIndex--) {
    MonomialPolynomial& leftMonomial = sortedLeft[leftIndex];
    MonomialPolynomial& rightMonomial = sortedRight[rightIndex];
    if (leftMonomial > rightMonomial) {
      return true;
    }
    if (rightMonomial > leftMonomial) {
      return false;
    }
    Coefficient leftCoefficient = left.getCoefficientOf(leftMonomial);
    Coefficient rightCoefficient = right.getCoefficientOf(rightMonomial);
    if (leftCoefficient > rightCoefficient) {
      return true;
    }
    if (rightCoefficient > leftCoefficient) {
      return false;
    }
  }
  if (left.size() > right.size()) {
    return true;
  }
  if (right.size() > left.size()) {
    return false;
  }
  return false;
}

template<class Coefficient>
std::string PolynomialDivisionReport<Coefficient>::getPolynomialStringSpacedMonomialsLaTeX(
  const Polynomial<Coefficient>& thePoly,
  std::string* highlightColor,
  List<MonomialPolynomial>* theHighLightedMons,
  int* firstNonZeroIndex
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::getPolynomialStringSpacedMonomialsLaTeX");
  std::stringstream out;
  bool found = false;
  int countMons = 0;
  if (firstNonZeroIndex != nullptr) {
    *firstNonZeroIndex = - 1;
  }
  for (int i = 0; i < this->allMonomials.size; i ++) {
    int theIndex = thePoly.monomials.getIndex(this->allMonomials[i]);
    if (theIndex == - 1) {
      if (i != this->allMonomials.size - 1) {
        out << "&";
      }
      continue;
    }
    if (firstNonZeroIndex != nullptr) {
      if (*firstNonZeroIndex == - 1) {
        *firstNonZeroIndex = i;
      }
    }
    countMons ++;
    bool useHighlightStyle = false;
    if (highlightColor != nullptr) {
      if (theHighLightedMons != nullptr) {
        if (theHighLightedMons->contains(this->allMonomials[i])) {
          useHighlightStyle = true;
        }
      }
    }
    out << "$";
    if (useHighlightStyle) {
      out << "\\color{" << *highlightColor << "}{";
    }
    out << Polynomial<Coefficient>::getBlendCoefficientAndMonomial(
      thePoly[theIndex],
      thePoly.coefficients[theIndex],
      found,
      &this->owner->format
    );
    found = true;
    if (useHighlightStyle) {
      out << "}\\color{black}";
    }
    out << "$ ";
    if (i != this->allMonomials.size - 1) {
      out << "& ";
    }
  }
  if (countMons != thePoly.size()) {
    out << " Programming ERROR!";
  }
  return out.str();
}

template <class Coefficient>
std::string PolynomialDivisionReport<Coefficient>::getDivisionStringLaTeX() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::getDivisionStringLaTeX");
  this->checkInitialization();
  std::stringstream out;
  List<Polynomial<Coefficient> >& theRemainders = this->intermediateRemainders;
  List<Polynomial<Coefficient> >& theSubtracands = this->intermediateSubtractands;
  this->owner->format.monomialOrder = this->owner->polynomialOrder.monomialOrder;
  std::string highlightedColor = "red";
  this->allMonomials.addOnTopNoRepetition(this->startingPolynomial.monomials);
  for (int i = 0; i < theRemainders.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(theRemainders[i].monomials);
  }
  for (int i = 0; i < theSubtracands.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(theSubtracands[i].monomials);
  }
  //List<std::string> basisColorStyles;
  //basisColorStyles.setSize(this->theBasis.size);
  this->allMonomials.quickSortDescending(&this->owner->polynomialOrder.monomialOrder);
  this->owner->format.flagUseLatex = true;
  out << this->owner->toStringLetterOrder(true);
  out << theRemainders.size << " division steps total.";
  out << "\\renewcommand{\\arraystretch}{1.2}";
  out << "\\begin{longtable}{|c";
  for (int i = 0; i < this->allMonomials.size; i ++) {
    out << "c";
  }
  out << "|} \\hline";
  out << "&" <<  "\\multicolumn{" << this->allMonomials.size
  << "}{|c|}{\\textbf{Remainder}}" << "\\\\";
  out << "\\multicolumn{1}{|c|}{} & ";
  Polynomial<Coefficient>& remainder = this->owner->remainderDivision;
  out << this->getPolynomialStringSpacedMonomialsLaTeX(
    remainder, &highlightedColor, &remainder.monomials
  ) << "\\\\\\hline";
  out << "\\textbf{Divisor(s)} &" << "\\multicolumn{"
  << this->allMonomials.size << "}{|c|}{\\textbf{Quotient(s)}}"
  << "\\\\";
  for (int i = 0; i < this->owner->basis.size; i ++) {
    out << "$";
    out << this->owner->basis[i].element.toString(&this->owner->format);
    out << "$";
    out << "& \\multicolumn{" << this->allMonomials.size << "}{|l|}{";
    out << "$" << this->owner->quotients[i].toString(&this->owner->format) << "$" << "}\\\\\\hline\\hline";
  }
  for (int i = 0; i < theRemainders.size; i ++) {
    if (i < theRemainders.size - 1) {
      out << "$\\underline{~}$";
    }
    out << "&"
    << this->getPolynomialStringSpacedMonomialsLaTeX(
      theRemainders[i], &highlightedColor, &this->intermediateHighlightedMons[i]
    ) << "\\\\\n";
    if (i < theSubtracands.size) {
      out << "&";
      out << this->getPolynomialStringSpacedMonomialsLaTeX(
        theSubtracands[i], &highlightedColor
      ) << "\\\\\\cline{2-" << this->allMonomials.size + 1 << "}";
    }
  }
  out << "\\hline";
  out << "\\end{longtable}";
  return out.str();
}

template<class Coefficient>
std::string PolynomialDivisionReport<Coefficient>::getPolynomialStringSpacedMonomialsHtml(
  const Polynomial<Coefficient>& thePoly,
  const std::string& extraStyle,
  List<MonomialPolynomial>* theHighLightedMons
) {
  std::stringstream out;
  bool found = false;
  int countMons = 0;
  for (int i = 0; i < this->allMonomials.size; i ++) {
    int theIndex = thePoly.monomials.getIndex(this->allMonomials[i]);
    if (theIndex == - 1) {
      out << "<td" << extraStyle << ">" << "</td>";
      continue;
    }
    countMons ++;
    bool useHighlightStyle = false;
    if (theHighLightedMons != nullptr) {
      if (theHighLightedMons->contains(this->allMonomials[i])) {
        useHighlightStyle = true;
      }
    }
    out << "<td" << extraStyle << ">";
    if (useHighlightStyle) {
      out << "<span style ='color:red'>";
    }
    if (this->owner->format.flagUseLatex) {
      std::string monomialWithCoefficient = Polynomial<Coefficient>::getBlendCoefficientAndMonomial(
        thePoly[theIndex],
        thePoly.coefficients[theIndex],
        found,
        &this->owner->format
      );

      out << HtmlRoutines::getMathNoDisplay(monomialWithCoefficient);
    } else {
      out << Polynomial<Coefficient>::getBlendCoefficientAndMonomial(
        thePoly[theIndex],
        thePoly.coefficients[theIndex],
        found,
        &this->owner->format
      );
    }
    found = true;
    if (useHighlightStyle) {
      out << "</span>";
    }
    out << "</td>";
  }
  if (countMons != thePoly.size()) {
    out << "<td><b>Programming ERROR!</b></td>";
  }
  return out.str();
}

template <class Coefficient>
bool PolynomialDivisionReport<Coefficient>::checkInitialization() {
  if (this->owner == nullptr) {
    global.fatal << "PolynomialDivisionReport has zero owner." << global.fatal;
  }
  return false;
}

template <class Coefficient>
std::string PolynomialDivisionReport<Coefficient>::getDivisionStringHtml() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::getDivisionStringHtml");
  this->checkInitialization();
  std::stringstream out;
  List<Polynomial<Coefficient> >& theRemainders = this->intermediateRemainders;
  List<Polynomial<Coefficient> >& theSubtracands = this->intermediateSubtractands;
  this->owner->format.monomialOrder = this->owner->polynomialOrder.monomialOrder;
  std::string underlineStyle = " style ='white-space: nowrap; border-bottom:1px solid black;'";
  this->allMonomials.clear();
  this->allMonomials.addOnTopNoRepetition(this->startingPolynomial.monomials);
  for (int i = 0; i < theRemainders.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(theRemainders[i].monomials);
    if (i < theSubtracands.size) {
      this->allMonomials.addOnTopNoRepetition(theSubtracands[i].monomials);
    }
  }
  //List<std::string> basisColorStyles;
  //basisColorStyles.setSize(this->theBasis.size);
  this->allMonomials.quickSortDescending(&this->owner->polynomialOrder.monomialOrder);
  out << this->owner->toStringLetterOrder(false);
  out << "<br>";
  out << theRemainders.size << " division steps total.<br>";
  out << "<table style ='white-space: nowrap; border:1px solid black;'>";
  out << "<tr><td " << underlineStyle << "><b>Remainder:</b></td>";
  out << this->getPolynomialStringSpacedMonomialsHtml(
    this->owner->remainderDivision, underlineStyle, &this->owner->remainderDivision.monomials
  )
  << "</td></tr>";
  out << "<tr><td style ='border-right:1px solid black;'><b>Divisor(s)</b></td><td colspan ='"
  << this->allMonomials.size + 1 << "'><b>Quotient(s) </b></td>"
  << "</tr>";
  FormatExpressions& format = this->owner->format;
  for (int i = 0; i < this->owner->basis.size; i ++) {
    out << "<tr>";
    out << "<td style ='border-right:1px solid black; border-bottom: 1px solid gray;'>";
    if (this->owner->format.flagUseLatex) {
      out << HtmlRoutines::getMathNoDisplay(this->owner->basis[i].element.toString(&format), - 1);
    } else {
      out << this->owner->basis[i].element.toString(&format);
    }
    out << "</td>";
    out << "<td style ='border-bottom:1px solid gray;' colspan ='"
    << this->allMonomials.size + 1 << "'>";
    out << HtmlRoutines::getMathNoDisplay(this->owner->quotients[i].toString(&format));
    out << "</td></tr>";
  }
  out << "</tr>";
  if (theRemainders.size != this->intermediateHighlightedMons.size) {
    global.fatal << "Should have as many remainders: " << theRemainders.size
    << " as intermediate highlighted mons: "
    << this->intermediateHighlightedMons.size << global.fatal;
  }
  if (theRemainders.size != theSubtracands.size + 1) {
    global.fatal << "Remainders should equal subtracands plus one. " << global.fatal;
  }
  for (int i = 0; i < theRemainders.size; i ++) {
    out << "<tr><td></td>"
    << this->getPolynomialStringSpacedMonomialsHtml(
      theRemainders[i], "", &this->intermediateHighlightedMons[i]
    ) << "</tr>";
    if (i < theSubtracands.size) {
      out << "<tr><td>-</td></tr>";
      out << "<tr><td></td>" << this->getPolynomialStringSpacedMonomialsHtml(theSubtracands[i], underlineStyle)
      << "</tr>";
    }
  }
  out << "</table>";
  return out.str();
}

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
  this->value.hashFunction() * someRandomPrimes[0] +
  this->modulus.hashFunction() * someRandomPrimes[1];
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
std::string PolynomialModuloPolynomial<Coefficient>::toString(FormatExpressions* format
) {
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

#endif
