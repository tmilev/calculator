// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "math_extra_Weyl_algebras_implementation.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_extra_modules_semisimple_Lie_algebras_implementation.h"
#include "math_extra_semisimple_Lie_algebras_implementation.h"
#include "math_extra_finite_groups_implementation.h"
#include "math_extra_symmetric_groups_and_generalizations.h"
#include "calculator_html_functions.h"
#include "crypto.h"
#include "math_extra_elliptic_curves_implementation.h"
#include <cmath>

Expression operator*(const Expression& left, const Expression& right) {
  MacroRegisterFunctionWithName("operator*(Expression, Expression)");
  left.checkInitialization();
  Expression result;
  result.MakeXOX(*left.owner, left.owner->opTimes(), left, right);
  return result;
}

Expression operator/(const Expression& left, const Expression& right) {
  left.checkInitialization();
  Expression result;
  result.MakeXOX(*left.owner, left.owner->opDivide(), left, right);
  return result;
}

Expression operator+(const Expression& left, const Expression& right) {
  left.checkInitialization();
  Expression result;
  result.MakeXOX(*left.owner, left.owner->opPlus(), left, right);
  return result;
}

Expression operator-(const Expression& left, const Expression& right) {
  left.checkInitialization();
  Expression result;
  result.MakeXOX(*left.owner, left.owner->opMinus(), left, right);
  return result;
}

//If you get a specialization after instantiation error:
//the following template specialization funcitons must appear
//here and nowhere else (discovered through extremely painful experimentation).

template < >
List<Expression>::Comparator*
FormatExpressions::GetMonOrder<Expression>() {
  return nullptr;
}

//Expression::getTypeOperation specializations follow
template < >
int Expression::getTypeOperation<ElementEllipticCurve<Rational> >() const {
  this->checkInitialization();
  return this->owner->opEllipticCurveElementsRational();
}

template < >
int Expression::getTypeOperation<ElementEllipticCurve<ElementZmodP> >() const {
  this->checkInitialization();
  return this->owner->opEllipticCurveElementsZmodP();
}

template < >
int Expression::getTypeOperation<RationalFunction>() const {
  this->checkInitialization();
  return this->owner->opRationalFunction();
}

template < >
int Expression::getTypeOperation<InputBox>() const {
  this->checkInitialization();
  return this->owner->opUserInputTextBox();
}

template < >
int Expression::getTypeOperation<Rational>() const {
  this->checkInitialization();
  return this->owner->opRational();
}

template < >
int Expression::getTypeOperation<ElementHyperoctahedralGroupR2>() const {
  this->checkInitialization();
  return this->owner->opElementHyperOctahedral();
}

template < >
int Expression::getTypeOperation<GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> >() const {
  this->checkInitialization();
  return this->owner->opHyperoctahedralGroupRep();
}

template < >
int Expression::getTypeOperation<std::string>() const {
  this->checkInitialization();
  return this->owner->opString();
}

template < >
int Expression::getTypeOperation<AlgebraicNumber>() const {
  this->checkInitialization();
  return this->owner->opAlgNumber();
}

template < >
int Expression::getTypeOperation<ElementZmodP>() const {
  this->checkInitialization();
  return this->owner->opEltZmodP();
}

template < >
int Expression::getTypeOperation<ElementUniversalEnveloping<RationalFunction> >() const {
  this->checkInitialization();
  return this->owner->opElementUEoverRF();
}

template < >
int Expression::getTypeOperation<Weight<Rational> >() const {
  this->checkInitialization();
  return this->owner->opWeightLieAlg();
}

template < >
int Expression::getTypeOperation<Weight<Polynomial<Rational> > >() const {
  this->checkInitialization();
  return this->owner->opWeightLieAlgPoly();
}

template < >
int Expression::getTypeOperation<MatrixTensor<Rational> >() const {
  this->checkInitialization();
  return this->owner->opMatTensorRat();
}

template < >
int Expression::getTypeOperation<int>() const {
  this->checkInitialization();
  return this->owner->opRational();
}

template < >
int Expression::getTypeOperation<LargeIntegerUnsigned>() const {
  this->checkInitialization();
  return this->owner->opRational();
}

template < >
int Expression::getTypeOperation<double>() const {
  this->checkInitialization();
  return this->owner->opDouble();
}

template < >
int Expression::getTypeOperation<Polynomial<Rational> >() const {
  this->checkInitialization();
  return this->owner->opPolynomialRational();
}

template < >
int Expression::getTypeOperation<Polynomial<AlgebraicNumber> >() const {
  this->checkInitialization();
  return this->owner->opPolynomialAlgebraicNumbers();
}

template < >
int Expression::getTypeOperation<Polynomial<ElementZmodP> >() const {
  this->checkInitialization();
  return this->owner->opPolynomialModuloInteger();
}

template < >
int Expression::getTypeOperation<PolynomialModuloPolynomial<ElementZmodP> >() const {
  this->checkInitialization();
  return this->owner->opPolynomialModuloPolynomialModuloInteger();
}

template < >
int Expression::getTypeOperation<ElementWeylAlgebra<Rational> >() const {
  this->checkInitialization();
  return this->owner->opElementWeylAlgebra();
}

template < >
int Expression::getTypeOperation<SemisimpleLieAlgebra*>() const {
  this->checkInitialization();
  return this->owner->opSemisimpleLieAlgebrA();
}

template < >
int Expression::getTypeOperation<LittelmannPath>() const {
  this->checkInitialization();
  return this->owner->opLittelmannPath();
}

template < >
int Expression::getTypeOperation<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >() const {
  this->checkInitialization();
  return this->owner->opLRO();
}

template < >
int Expression::getTypeOperation<ElementTensorsGeneralizedVermas<RationalFunction> >() const {
  this->checkInitialization();
  return this->owner->opElementTensorGVM();
}

template < >
int Expression::getTypeOperation<CharacterSemisimpleLieAlgebraModule<Rational> >() const {
  this->checkInitialization();
  return this->owner->opCharSSAlgMod();
}

template < >
int Expression::getTypeOperation<Plot>() const {
  this->checkInitialization();
  return this->owner->opCalculusPlot();
}

template < >
int Expression::getTypeOperation<PlotObject>() const {
  this->checkInitialization();
  return this->owner->opCalculusPlot();
}

template < >
int Expression::getTypeOperation<SemisimpleSubalgebras>() const {
  this->checkInitialization();
  return this->owner->opSemisimpleSubalgebras();
}

template < >
int Expression::getTypeOperation<WeylGroupData>() const {
  this->checkInitialization();
  return this->owner->opWeylGroup();
}

template < >
int Expression::getTypeOperation<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >() const {
  this->checkInitialization();
  return this->owner->opWeylGroupRep();
}

template < >
int Expression::getTypeOperation<ElementWeylGroup>() const {
  this->checkInitialization();
  return this->owner->opWeylGroupElement();
}

template < >
int Expression::getTypeOperation<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >() const {
  this->checkInitialization();
  return this->owner->opWeylGroupVirtualRep();
}

// Expression::getTypeOperation specializations end.
// Expression::convertsInternally specializations follow.
template < >
bool Expression::convertsInternally(
  WithContext<ElementSemisimpleLieAlgebra<AlgebraicNumber> >* whichElement
) const {
  MacroRegisterFunctionWithName("Expression::convertsInternally");
  if (this->owner == nullptr) {
    return false;
  }
  ElementUniversalEnveloping<RationalFunction> element;
  if (!this->isOfType<ElementUniversalEnveloping<RationalFunction> >(&element)) {
    return false;
  }
  ElementSemisimpleLieAlgebra<Rational> elementSemisimple;
  if (!element.GetLieAlgebraElementIfPossible(elementSemisimple)) {
    return false;
  }
  if (whichElement != nullptr) {
    this->getContext(whichElement->context);
    whichElement->content = elementSemisimple;
  }
  return true;
}

//Expression::ConvertsToType specializations end.
//Expression::addObjectReturnIndex specializations follow

template < >
int Expression::addObjectReturnIndex(const
ElementEllipticCurve<Rational>
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.EllipticCurveElementsRational
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
ElementEllipticCurve<ElementZmodP>
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.EllipticCurveElementsZmodP
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
SemisimpleSubalgebras
& inputValue) const {
  this->checkInitialization();
  inputValue.checkInitialization();
  if (!this->owner->theObjectContainer.theSSSubalgebraS.contains(inputValue.owner->theWeyl.theDynkinType)) {
    global.fatal << "Semisimple subalgebras must be allocated directly in the object container. " << global.fatal;
  }
  return this->owner->theObjectContainer.theSSSubalgebraS.getIndex(inputValue.owner->theWeyl.theDynkinType);
}

template < >
int Expression::addObjectReturnIndex(const
ElementHyperoctahedralGroupR2
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.theElementsHyperOctGroup
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
InputBox
& inputValue) const {
  this->checkInitialization();
  this->owner->theObjectContainer.theUserInputTextBoxesWithValues
  .setKeyValue(inputValue.name, inputValue);
  return this->owner->theObjectContainer.theUserInputTextBoxesWithValues
  .getIndex(inputValue.name);
}

template < >
int Expression::addObjectReturnIndex(const
std::string
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.theStrings
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational>
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.theHyperoctahedralReps
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
Rational
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.theRationals
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
double
& inputValue) const {
  this->checkInitialization();
  if (std::isnan(inputValue)) {
    return 0;
  }
  return this->owner->theObjectContainer.theDoubles
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
RationalFunction
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.theRFs
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
MatrixTensor<Rational>
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.theMatTensorRats
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
ElementUniversalEnveloping<RationalFunction>
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.theUEs
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
Weight<Rational>
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.theWeights
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
Weight<Polynomial<Rational> >
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.theWeightsPoly
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
int
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.theRationals
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
LargeIntegerUnsigned
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.theRationals
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
Polynomial<Rational>
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.polynomialsRational
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
Polynomial<ElementZmodP>
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.polynomialsModular
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
PolynomialModuloPolynomial<ElementZmodP>
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.polynomialQuotientsModular
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
Polynomial<AlgebraicNumber>
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.polynomialsAlgebraic
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
ElementWeylAlgebra<Rational>
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.theWeylAlgebraElements
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(
SemisimpleLieAlgebra* const
& inputValue) const {
  this->checkInitialization();
  if (!this->owner->theObjectContainer.semisimpleLieAlgebras.contains(inputValue->theWeyl.theDynkinType)) {
    global.fatal << "Semisimple Lie algebra must be allocated directly in the object container. " << global.fatal;
  }
  int index = this->owner->theObjectContainer.semisimpleLieAlgebras.getIndex(inputValue->theWeyl.theDynkinType);
  return index;
}

template < >
int Expression::addObjectReturnIndex(const
AlgebraicNumber
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.theAlgebraicNumbers
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
ElementZmodP
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.theEltsModP
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
MonomialTensor<int, MathRoutines::IntUnsignIdentity>
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.theLittelmannOperators
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
ElementTensorsGeneralizedVermas<RationalFunction>
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.theTensorElts
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
CharacterSemisimpleLieAlgebraModule<Rational>
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.theCharsSSLieAlgFD
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
LittelmannPath
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.theLSpaths
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
Plot
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.thePlots
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
PlotObject
& inputValue) const {
  this->checkInitialization();
  Plot plotContainer;
  plotContainer += inputValue;
  return this->owner->theObjectContainer.thePlots
  .addNoRepetitionOrReturnIndexFirst(plotContainer);
}

template < >
int Expression::addObjectReturnIndex(const
WeylGroupData
& inputValue) const {
  this->checkInitialization();
  this->owner->theObjectContainer.GetLieAlgebraCreateIfNotPresent(inputValue.theDynkinType);
  return this->owner->theObjectContainer.semisimpleLieAlgebras.getIndex(inputValue.theDynkinType);
}

template < >
int Expression::addObjectReturnIndex(const
GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.theWeylGroupReps
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.theWeylGroupVirtualReps
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
ElementWeylGroup
& inputValue) const {
  this->checkInitialization();
  return this->owner->theObjectContainer.theWeylGroupElements
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

//Expression::addObjectReturnIndex specializations end

//start Expression::getValueNonConst specializations.
template < >
Rational& Expression::getValueNonConst() const {
  if (!this->isOfType<Rational>()) {
    global.fatal << "This is a programming error: expression not of required type Rational. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theRationals.getElement(this->GetLastChild().theData);
}

template < >
ElementEllipticCurve<Rational>& Expression::getValueNonConst() const {
  if (!this->isOfType<ElementEllipticCurve<Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type Rational. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.EllipticCurveElementsRational.getElement(this->GetLastChild().theData);
}

template < >
Polynomial<ElementZmodP>& Expression::getValueNonConst() const {
  if (!this->isOfType<Polynomial<ElementZmodP> >()) {
    global.fatal << "This is a programming error: expression not of required type Polynomial mod integer. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.polynomialsModular.getElement(this->GetLastChild().theData);
}

template < >
PolynomialModuloPolynomial<ElementZmodP>& Expression::getValueNonConst() const {
  if (!this->isOfType<PolynomialModuloPolynomial<ElementZmodP> >()) {
    global.fatal
    << "This is a programming error: expression not of "
    << "required type Polynomial mod Polynomial mod integer. "
    << "The expression equals "
    << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.polynomialQuotientsModular.getElement(this->GetLastChild().theData);
}

template < >
ElementEllipticCurve<ElementZmodP>& Expression::getValueNonConst() const {
  if (!this->isOfType<ElementEllipticCurve<ElementZmodP> >()) {
    global.fatal << "This is a programming error: expression not of required type Rational. The expression equals "
    << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.EllipticCurveElementsZmodP.getElement(this->GetLastChild().theData);
}

template < >
ElementHyperoctahedralGroupR2& Expression::getValueNonConst() const {
  if (!this->isOfType<ElementHyperoctahedralGroupR2>()) {
    global.fatal << "This is a programming error: expression not of required type Rational. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theElementsHyperOctGroup.getElement(this->GetLastChild().theData);
}

template < >
InputBox& Expression::getValueNonConst() const {
  if (!this->isOfType<InputBox>()) {
    global.fatal << "This is a programming error: expression not of required type Rational. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theUserInputTextBoxesWithValues.theValues[this->GetLastChild().theData];
}

template < >
GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> & Expression::getValueNonConst() const {
  if (!this->isOfType<GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type Rational. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theHyperoctahedralReps[this->GetLastChild().theData];
}

template < >
ElementZmodP& Expression::getValueNonConst() const {
  if (!this->isOfType<ElementZmodP>())
    global.fatal << "This is a programming error: expression not of required type ElementZmodP. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  return this->owner->theObjectContainer.theEltsModP.getElement(this->GetLastChild().theData);
}

template < >
AlgebraicNumber& Expression::getValueNonConst() const {
  if (!this->isOfType<AlgebraicNumber>()) {
    global.fatal << "This is a programming error: expression not of required type AlgebraicNumber. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theAlgebraicNumbers.getElement(this->GetLastChild().theData);
}

template < >
double& Expression::getValueNonConst() const {
  if (!this->isOfType<double>()) {
    global.fatal << "This is a programming error: expression not of required type double. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theDoubles.getElement(this->GetLastChild().theData);
}

template < >
std::string& Expression::getValueNonConst() const {
  if (!this->isOfType<std::string>()) {
    global.fatal << "This is a programming error: expression not of required type std::string. "
    << "The expression equals " << this->toString() << ". Comments so far: "
    << this->owner->comments.str() << global.fatal;
  }
  return this->owner->theObjectContainer.theStrings.getElement(this->GetLastChild().theData);
}

template < >
RationalFunction& Expression::getValueNonConst() const {
  if (!this->isOfType<RationalFunction>()) {
    global.fatal << "This is a programming error: expression not of required type RationalFunctionOld. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theRFs.getElement(this->GetLastChild().theData);
}

template < >
ElementUniversalEnveloping<RationalFunction>& Expression::getValueNonConst() const {
  if (!this->isOfType<ElementUniversalEnveloping<RationalFunction> >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "ElementUniversalEnveloping_RationalFunctionOld. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theUEs.getElement(this->GetLastChild().theData);
}

template < >
Polynomial<Rational>& Expression::getValueNonConst() const {
  if (!this->isOfType<Polynomial<Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type Polynomial_Rational. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.polynomialsRational.getElement(this->GetLastChild().theData);
}

template < >
Polynomial<AlgebraicNumber>& Expression::getValueNonConst() const {
  if (!this->isOfType<Polynomial<AlgebraicNumber> >()) {
    global.fatal << "This is a programming error: expression not of required type Polynomial_AlgebraicNumber. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.polynomialsAlgebraic.getElement(this->GetLastChild().theData);
}

template < >
ElementWeylAlgebra<Rational>& Expression::getValueNonConst() const {
  if (!this->isOfType<ElementWeylAlgebra<Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "ElementWeylAlgebra_Rational. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theWeylAlgebraElements.getElement(this->GetLastChild().theData);
}

template < >
LittelmannPath& Expression::getValueNonConst() const {
  if (!this->isOfType<LittelmannPath>()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "LittelmannPath. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theLSpaths.getElement(this->GetLastChild().theData);
}

template < >
Weight<Polynomial<Rational> >& Expression::getValueNonConst() const {
  if (!this->isOfType<Weight<Polynomial<Rational> > >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "LittelmannPath. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theWeightsPoly.getElement(this->GetLastChild().theData);
}

template < >
MonomialTensor<int, MathRoutines::IntUnsignIdentity>& Expression::getValueNonConst() const {
  if (!this->isOfType<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "MonomialTensor. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theLittelmannOperators.getElement(this->GetLastChild().theData);
}

template < >
ElementTensorsGeneralizedVermas<RationalFunction>& Expression::getValueNonConst() const {
  if (!this->isOfType<ElementTensorsGeneralizedVermas<RationalFunction> >()) {
    global.fatal << "This is a programming error: expression not of "
    << "required type ElementTGVM_RationalFunctionOld. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theTensorElts.getElement(this->GetLastChild().theData);
}

template < >
CharacterSemisimpleLieAlgebraModule<Rational>& Expression::getValueNonConst() const {
  if (!this->isOfType<CharacterSemisimpleLieAlgebraModule<Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "charSSAlgMod_Rational. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theCharsSSLieAlgFD.getElement(this->GetLastChild().theData);
}

template < >
SemisimpleLieAlgebra*& Expression::getValueNonConst() const {
  if (!this->isOfType<SemisimpleLieAlgebra*>()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "SemisimpleLieAlgebra. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.semisimpleLieAlgebraPointers[this->GetLastChild().theData];
}

template < >
MatrixTensor<Rational>& Expression::getValueNonConst() const {
  if (!this->isOfType<MatrixTensor<Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "MatrixTensorRational. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theMatTensorRats.getElement(this->GetLastChild().theData);
}

template < >
SemisimpleSubalgebras& Expression::getValueNonConst() const {
  if (!this->isOfType<SemisimpleSubalgebras>()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "SemisimpleSubalgebras. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theSSSubalgebraS.theValues[this->GetLastChild().theData];
}

template < >
Plot& Expression::getValueNonConst() const {
  if (!this->isOfType<Plot>()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "Plot. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.thePlots[this->GetLastChild().theData];
}

template < >
WeylGroupData& Expression::getValueNonConst() const {
  if (!this->isOfType<WeylGroupData>()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "WeylGroupData. The expression equals "
    << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.semisimpleLieAlgebras.theValues[
    this->GetLastChild().theData
  ].theWeyl;
}

template < >
GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& Expression::getValueNonConst() const {
  if (!this->isOfType<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "WeylGroupRepresentation_Rational. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theWeylGroupReps.getElement(this->GetLastChild().theData);
}

template < >
ElementWeylGroup& Expression::getValueNonConst() const {
  if (!this->isOfType<ElementWeylGroup>()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "ElementWeylGroup. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theWeylGroupElements.getElement(this->GetLastChild().theData);
}

template < >
VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& Expression::getValueNonConst() const {
  if (!this->isOfType<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "WeylGroupVirtualRepresentation. The expression equals "
    << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theWeylGroupVirtualReps.getElement(this->GetLastChild().theData);
}

bool Expression::isMatrix(int* outputNumRows, int* outputNumCols) const {
  MacroRegisterFunctionWithName("Expression::isMatrix");
  if (this->owner == nullptr) {
    return false;
  }
  if (this->size() < 1) {
    return false;
  }
  bool isGood = (*this)[0].startsWith(this->owner->opMatriX());
  if (!isGood) {
    return false;
  }
  if (outputNumRows != nullptr) {
    *outputNumRows = this->size() - 1;
  }
  if (outputNumCols != nullptr) {
    if (this->size() <= 1) {
      *outputNumCols = 0;
    } else {
      *outputNumCols = (*this)[1].size() - 1;
    }
  }
  return true;
}

//end Expression::getValueNonConst specializations.
//start Expression::convertInternally specializations.
template< >
bool Expression::convertInternally<Polynomial<AlgebraicNumber> >(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_Polynomial_AlgebraicNumber");
  this->checkInitialization();
  if (this->isOfType<Rational>()) {
    Polynomial<AlgebraicNumber> resultP;
    resultP.makeConstant(this->getValue<Rational>());
    return output.assignValueWithContext(resultP, this->getContext(), *this->owner);
  }
  if (this->isOfType<AlgebraicNumber>()) {
    Polynomial<AlgebraicNumber> resultP;
    resultP.makeConstant(this->getValue<AlgebraicNumber>());
    return output.assignValueWithContext(resultP, this->getContext(), *this->owner);
  }
  if (this->isOfType<Polynomial<Rational> >()) {
    Polynomial<AlgebraicNumber> resultP;
    resultP = this->getValue<Polynomial<Rational> >();
    return output.assignValueWithContext(resultP, this->getContext(), *this->owner);
  }
  if (this->isOfType<Polynomial<AlgebraicNumber> >()) {
    output = *this;
    return true;
  }
  return false;
}

template< >
bool Expression::convertInternally<Polynomial<Rational> >(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_Polynomial_Rational");
  this->checkInitialization();
  if (this->isOfType<Rational>()) {
    Polynomial<Rational> resultP;
    resultP.makeConstant(this->getValue<Rational>());
    return output.assignValueWithContext(resultP, this->getContext(), *this->owner);
  }
  if (this->isOfType<Polynomial<Rational> >()) {
    output = *this;
    return true;
  }
  return false;
}

template< >
bool Expression::convertInternally<Polynomial<ElementZmodP> >(Expression& output) const {
  MacroRegisterFunctionWithName("Expression::convertInternally");
  this->checkInitialization();
  if (this->isOfType<Polynomial<ElementZmodP> >()) {
    output = *this;
    return true;
  }
  return false;
}

template< >
bool Expression::convertInternally<PolynomialModuloPolynomial<ElementZmodP> >(Expression& output) const {
  MacroRegisterFunctionWithName("Expression::convertInternally");
  this->checkInitialization();
  if (this->isOfType<PolynomialModuloPolynomial<ElementZmodP> >()) {
    output = *this;
    return true;
  }
  return false;
}

template< >
bool Expression::convertInternally<RationalFunction>(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_RationalFunctionOld");
  this->checkInitialization();
  if (this->isOfType<Rational>()) {
    RationalFunction resultRF;
    resultRF.makeConstant(this->getValue<Rational>());
    return output.assignValueWithContext(
      resultRF, this->getContext(), *this->owner
    );
  }
  if (this->isOfType<Polynomial<Rational> >()) {
    RationalFunction resultRF;
    resultRF = this->getValue<Polynomial<Rational> >();
    return output.assignValueWithContext(
      resultRF, this->getContext(), *this->owner
    );
  }
  if (this->isOfType<RationalFunction>()) {
    output = *this;
    return true;
  }
  (*this->owner)
  << "<hr>ConvertToType_RationalFunctionOld: Failed to convert "
  << this->toString() << " to rational function. ";
  return false;
}

template< >
bool Expression::convertInternally<ElementWeylAlgebra<Rational> >(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_ElementWeylAlgebra");
  this->checkInitialization();
  if (this->isOfType<Rational>()) {
    ElementWeylAlgebra<Rational> resultEWA;
    resultEWA.makeConstant(this->getValue<Rational>());
    return output.assignValueWithContext(resultEWA, this->getContext(), *this->owner);
  }
  if (this->isOfType<Polynomial<Rational> >()) {
    ElementWeylAlgebra<Rational> resultEWA;
    resultEWA.AssignPolynomial(this->getValue<Polynomial<Rational> >());
    return output.assignValueWithContext(resultEWA, this->getContext(), *this->owner);
  }
  if (this->isOfType<ElementWeylAlgebra<Rational> >()) {
    output = *this;
    return true;
  }
  return false;
}

template< >
bool Expression::convertInternally<Weight<Polynomial<Rational> > >(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_Weight_Polynomial");
  this->checkInitialization();
  if (this->isOfType<Weight<Polynomial<Rational> > >()) {
    output = *this;
    return true;
  }
  return false;
}

template< >
bool Expression::convertInternally<ElementUniversalEnveloping<RationalFunction> >(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_RationalFunctionOld");
  this->checkInitialization();
  if (!this->isBuiltInType())
    return false;
  if (this->isOfType<ElementUniversalEnveloping<RationalFunction> >()) {
    output = *this;
    return true;
  }
  SemisimpleLieAlgebra* theOwner = this->getAmbientSemisimpleLieAlgebraNonConstUseWithCaution();
  if (theOwner == nullptr) {
    this->owner->comments << "<hr>Failed to convert " << this->toString() << " (Lispified: " << this->toStringFull()
    << ") to element of universal enveloping -  failed to extract ambient Lie algebra. ";
    return false;
  }
  ElementUniversalEnveloping<RationalFunction> outputUE;
  if (this->isOfType<Rational>()) {
    outputUE.makeConstant(this->getValue<Rational>(), *theOwner);
    return output.assignValueWithContext(outputUE, this->getContext(), *this->owner);
  } else if (this->isOfType<Polynomial<Rational> >()) {
    RationalFunction tempRF;
    tempRF = this->getValue<Polynomial<Rational> >();
    outputUE.makeConstant(tempRF, *theOwner);
    return output.assignValueWithContext(outputUE, this->getContext(), *this->owner);
  } else if (this->isOfType<RationalFunction>()) {
    outputUE.makeConstant(this->getValue<RationalFunction>(), *theOwner);
    return output.assignValueWithContext(outputUE, this->getContext(), *this->owner);
  }
  return *this->owner << "<hr>Failed to convert " << this->toString()
  << " to element of universal enveloping -  I don't know how. ";
}

template< >
bool Expression::convertInternally<ElementTensorsGeneralizedVermas<RationalFunction> >(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_RationalFunctionOld");
  this->checkInitialization();
  if (this->isOfType<ElementTensorsGeneralizedVermas<RationalFunction> >()) {
    output = *this;
    return true;
  }
  return false;
}

template< >
bool Expression::convertInternally<Rational>(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_Rational");
  this->checkInitialization();
  if (this->isOfType<Rational>()) {
    output = *this;
    return true;
  }
  return false;
}

template< >
bool Expression::convertInternally<AlgebraicNumber>(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_AlgebraicNumber");
  this->checkInitialization();
  if (this->isOfType<AlgebraicNumber>()) {
    output = *this;
    return true;
  }
  Rational theRat;
  if (!this->isOfType<Rational>(&theRat)) {
    return false;
  }
  AlgebraicNumber outRat = theRat;
  return output.assignValue(outRat, *this->owner);
}

template< >
bool Expression::convertInternally<double>(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_double");
  this->checkInitialization();
  if (this->isOfType<double>()) {
    output = *this;
    return true;
  }
  double outputDouble = 0;
  if (this->evaluatesToDouble(&outputDouble)) {
    return output.assignValue(outputDouble, *this->owner);
  }
  return false;
}
//end Expression::convertToType specializations.

bool Expression::CheckConsistencyRecursively() const {
  MacroRegisterFunctionWithName("Expression::CheckConsistencyRecursively");
  this->checkConsistency();
  for (int i = 0; i < this->children.size; i ++) {
    (*this)[i].checkConsistency();
  }
  return true;
}

bool Expression::checkConsistency() const {
  MacroRegisterFunctionWithName("Expression::checkConsistency");
  // warning: do not use toString method from here: toString itself calls CheckConosistency,
  // so that causes an "infinite" recursion call cycle,
  // i.e., stack overflow.
  if (this->flagDeallocated) {
    global.fatal << "Use after free of Expression. " << global.fatal;
  }
  this->checkInitialization();
  if (this->children.size == 0) {
    return false;
  }
  if (this->isBuiltInType()) {
    if (this->children.size != 3) {
      global.fatal << "This is a programming error. At the moment of writing, an expression of built-in type must have 3 "
      << "children: type, context, and index in Calculator. The expression is " << this->toStringFull()
      << global.fatal;
    }
    const Expression& mustBeTheContext = (*this)[1];
    if (!mustBeTheContext.startsWith(this->owner->opContext())) {
      global.fatal << "This is a programming error. At the moment of writing, "
      << "the second child of a built-in type must be a context. It is instead "
      << mustBeTheContext.toStringFull() << global.fatal;
    }
    for (int i = 1; i < mustBeTheContext.children.size; i ++) {
      bool isGood = false;
      const Expression& currentE = mustBeTheContext[i];
      if (currentE.startsWith(this->owner->opPolynomialVariables())) {
        isGood = true;
      }
      if (currentE.startsWith(this->owner->opSemisimpleLieAlgebrA())) {
        isGood = true;
      }
      if (currentE.startsWith(this->owner->opWeylAlgebraVariables())) {
        isGood = true;
      }
      if (!isGood) {
        global.fatal << "This is a programming error. The context "
        << mustBeTheContext.toStringFull() << " has an entry which I do not recognize, namely, "
        << currentE.toStringFull() << ". " << global.fatal;
      }
    }
  }
  return true;
}

const Expression& Expression::operator[](int n) const {
  this->checkInitialization();
  int childIndex = this->children[n];
  if (childIndex < 0) {
    global.fatal << "<hr>This is a programming error: the child of position "
    << n << " out of " << this->children.size - 1
    << " is not contained in the expression container. " << global.fatal;
  }
  return this->owner->theExpressionContainer[childIndex];
}

Expression Expression::zero() {
  if (this->owner == nullptr) {
    return Expression::zeroStatic();
  }
  Expression result;
  return result.assignValue(0, *this->owner);
}

Expression Expression::zeroStatic() {
  Expression result;
  return result;
}

bool Expression::addChildRationalOnTop(const Rational& inputRat) {
  this->checkInitialization();
  Expression ratE;
  ratE.assignValue(inputRat, *this->owner);
  return this->addChildOnTop(ratE);
}

bool Expression::addChildOnTop(const Expression& inputChild) {
  this->checkInitialization();
  this->children.addOnTop(
    this->owner->theExpressionContainer.addNoRepetitionOrReturnIndexFirst(inputChild)
  );
  return true;
}

bool Expression::setChildAtomValue(int childIndex, int TheAtomValue) {
  this->checkInitialization();
  Expression tempE;
  tempE.makeAtom(TheAtomValue, *this->owner);
  this->children.setObjectAtIndex(
    childIndex,
    this->owner->theExpressionContainer.addNoRepetitionOrReturnIndexFirst(tempE)
  );
  return true;
}

bool Expression::setChildAtomValue(int childIndex, const std::string& theAtom) {
  this->checkInitialization();
  Expression tempE;
  tempE.makeAtom(theAtom, *this->owner);
  this->children.setObjectAtIndex(
    childIndex, this->owner->theExpressionContainer.addNoRepetitionOrReturnIndexFirst(tempE)
  );
  return true;
}

bool Expression::setChild(int childIndexInMe, const Expression& inputChild) {
  this->checkInitialization();
  int theIndexOfTheExpression = this->owner->theExpressionContainer.addNoRepetitionOrReturnIndexFirst(inputChild);
  this->children.setObjectAtIndex(childIndexInMe, theIndexOfTheExpression);
  return true;
}

bool Expression::setChild(int childIndexInMe, int childIndexInBoss) {
  this->checkInitialization();
  this->children.setObjectAtIndex(childIndexInMe, childIndexInBoss);
  if (this->children[childIndexInMe] != childIndexInBoss) {
    global.fatal << "This shouldn't happen." << global.fatal;
  }
  return true;
}

bool Expression::addChildAtomOnTop(int theOp) {
  this->checkInitialization();
  Expression tempE;
  tempE.makeAtom(theOp, *this->owner);
  return this->addChildOnTop(tempE);
}

bool Expression::addChildAtomOnTop(const std::string& theOperationString) {
  this->checkInitialization();
  return this->addChildAtomOnTop(
    this->owner->AddOperationNoRepetitionOrReturnIndexFirst(theOperationString)
  );
}

bool Expression::startsWithBuiltInAtom() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->isAtom() || this->children.size < 1) {
    return false;
  }
  return (*this)[0].isBuiltInAtom();
}

bool Expression::startsWithArithmeticOperation() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->isAtom() || this->children.size < 1) {
    return false;
  }
  return (*this)[0].isArithmeticOperation();
}

bool Expression::startsWithFunctionWithComplexRange() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->isAtom() || this->children.size < 1) {
    return false;
  }
  return (*this)[0].isKnownFunctionWithComplexRange();
}

bool Expression::startsWithGivenOperation(const std::string& theOperation, int desiredChildren) const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->size() == 0) {
    return false;
  }
  int theOpIndex = this->owner->operations.getIndex(theOperation);
  if (theOpIndex == - 1) {
    return false;
  }
  if (desiredChildren >= 0) {
    if (desiredChildren != this->size()) {
      return false;
    }
  }
  return (*this)[0].isOperationGiven(theOpIndex);
}

bool Expression::startsWith(int theOp, int N) const {
  if (N != - 1) {
    if (this->children.size != N) {
      return false;
    }
  } else {
    if (this->children.size == 0) {
      return false;
    }
  }
  if (!(*this)[0].isAtom()) {
    return false;
  }
  if (theOp == - 1) {
    return true;
  }
  return (*this)[0].theData == theOp;
}

bool Expression::GetExpressionLeafs(HashedList<Expression>& outputAccumulateLeafs) const {
  MacroRegisterFunctionWithName("Expression::GetExpressionLeafs");
  if (this->owner == nullptr) {
    return true;
  }
  RecursionDepthCounter(&this->owner->RecursionDeptH);
  if (this->owner->RecursionDepthExceededHandleRoughly("In Expression::GetExpressionLeafs:")) {
    return false;
  }
  if (this->isBuiltInType() || this->isAtom()) {
    outputAccumulateLeafs.addOnTopNoRepetition(*this);
    return true;
  }
  for (int i = 0; i < this->size(); i ++) {
    if (!((*this)[i].GetExpressionLeafs(outputAccumulateLeafs))) {
      return false;
    }
  }
  return true;
}

bool Expression::getBoundVariables(HashedList<Expression>& outputAccumulateBoundVariables) const {
  MacroRegisterFunctionWithName("Expression::getBoundVariables");
  if (this->owner == nullptr) {
    return true;
  }
  RecursionDepthCounter(&this->owner->RecursionDeptH);
  if (this->owner->RecursionDepthExceededHandleRoughly("In Expression::GetFreeVariables:")) {
    return false;
  }
  if (this->isBuiltInType()) { //<- this may need to be rewritten as some built in types will store free variables in their context.
    return true;
  }
  if (this->startsWith(this->owner->opBind(), 2)) {
    outputAccumulateBoundVariables.addOnTopNoRepetition((*this)[1]);
    return true;
  }
  for (int i = 0; i < this->size(); i ++) {
    if (!((*this)[i].getBoundVariables(outputAccumulateBoundVariables))) {
      return false;
    }
  }
  return true;
}

bool Expression::AllowedAsFreeVariableAtom(const std::string &input) const {
  return
    input != "="              &&
    input != ">"              &&
    input != "\""             &&
    input != "=="             &&
    input != "<"              &&
    input != "Sequence"       &&
    input != "Matrix"         &&
    input != "\\circ"         &&
    input != "\\emptyset"     &&
    input != "\\in"           &&
    input != "\\cup"          &&
    input != "\\infty"        &&
    input != "IntervalClosed" &&
    input != "IntervalOpen"   &&
    input != "[)"             &&
    input != "(]"             &&
    input != "and"            &&
    input != "or"             &&
    input != "not"
  ;
}

bool Expression::GetFreeVariables(HashedList<Expression>& outputAccumulateFreeVariables, bool excludeNamedConstants) const {
  MacroRegisterFunctionWithName("Expression::GetFreeVariables");
  if (this->owner == nullptr) {
    return true;
  }
  RecursionDepthCounter(&this->owner->RecursionDeptH);
  if (this->owner->RecursionDepthExceededHandleRoughly("In Expression::GetFreeVariables:")) {
    return false;
  }
  if (this->isBuiltInType()) { //<- this may need to be rewritten as some built in types will store free variables in their context.
    return true;
  }
  if (this->isOfType<InputBox>()) {
    return true;
  }
  std::string atomName;
  if (this->isOperation(&atomName)) {
    bool doAddExpression = !this->isKnownFunctionWithComplexRange();
    if (doAddExpression) {
      doAddExpression = this->AllowedAsFreeVariableAtom(atomName);
    }
    if (doAddExpression && excludeNamedConstants) {
      if (this->owner->knownDoubleConstants.contains(*this)) {
        doAddExpression = false;
      }
    }
    if (doAddExpression) {
      outputAccumulateFreeVariables.addOnTopNoRepetition(*this);
    }
    return true;
  }
  for (int i = 0; i < this->size(); i ++) {
    if (!((*this)[i].GetFreeVariables(outputAccumulateFreeVariables, excludeNamedConstants))) {
      return false;
    }
  }
  return true;
}

bool Expression::isIntegralFdx(
  Expression* differentialVariable, Expression* functionToIntegrate, Expression* integrationSet
) const {
  MacroRegisterFunctionWithName("Expression::isIntegralFdx");
  if (this->owner == nullptr) {
    return false;
  }
  if (!this->startsWith(this->owner->opIntegral(), 3)) {
    return false;
  }
  if (!(*this)[2].isDifferentialOneFormOneVariable(differentialVariable, functionToIntegrate)) {
    return false;
  }
  if (integrationSet != nullptr) {
    *integrationSet = (*this)[1];
  }
  return true;
}

bool Expression::isIndefiniteIntegralFdx(
  Expression* differentialVariable, Expression* functionToIntegrate, Expression* integrationSet
) const {
  MacroRegisterFunctionWithName("Expression::isIndefiniteIntegralFdx");
  this->checkConsistency();
  Expression tempE;
  if (integrationSet == nullptr) {
    integrationSet = &tempE;
  }
  if (!this->isIntegralFdx(differentialVariable, functionToIntegrate, integrationSet)) {
    return false;
  }
  return integrationSet->isOperationGiven(this->owner->opIndefiniteIndicator());
}

bool Expression::isDefiniteIntegralOverIntervalFdx(
  Expression* differentialVariable, Expression* functionToIntegrate, Expression* integrationSet
) const {
  MacroRegisterFunctionWithName("Expression::isDefiniteIntegralOverIntervalFdx");
  this->checkConsistency();
  Expression tempE;
  if (integrationSet == nullptr) {
    integrationSet = &tempE;
  }
  if (!this->isIntegralFdx(differentialVariable, functionToIntegrate, integrationSet)) {
    return false;
  }
  return integrationSet->startsWith(this->owner->opLimitBoundary(), 3);
}

bool Expression::isDifferentialOneFormOneVariable(
  Expression* outputDifferentialOfWhat, Expression* outputCoeffInFrontOfDifferential
) const {
  MacroRegisterFunctionWithName("Expression::isDifferentialOneFormOneVariable");
  if (this->owner == nullptr) {
    return false;
  }
  if (!this->startsWith(this->owner->opDifferential()) || this->size() != 3) {
    return false;
  }
  if (outputDifferentialOfWhat != nullptr) {
    *outputDifferentialOfWhat = (*this)[1];
  }
  if (outputCoeffInFrontOfDifferential != nullptr) {
    *outputCoeffInFrontOfDifferential = (*this)[2];
  }
  return true;
}

bool Expression::ContainsAsSubExpressionNoBuiltInTypes(const Expression& input) const {
  if (this->owner == nullptr) {
    return false;
  }
  RecursionDepthCounter theCounter(&this->owner->RecursionDeptH);
  if (this->owner->RecursionDepthExceededHandleRoughly("In Expression::ContainsAsSubExpression: ")) {
    return false;
  }
  if (*this == input) {
    return true;
  }
  if (!this->IsSuitableForRecursion()) {
    return false;
  }
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].ContainsAsSubExpressionNoBuiltInTypes(input)) {
      return true;
    }
  }
  return false;
}

bool Expression::ContainsAsSubExpressionNoBuiltInTypes(int inputAtom) const {
  if (this->owner == nullptr) {
    return false;
  }
  Expression theE;
  theE.makeAtom(inputAtom, *this->owner);
  return this->ContainsAsSubExpressionNoBuiltInTypes(theE);
}

bool Expression::isContext() const {
  if (this->owner == nullptr) {
    return false;
  }
  return this->startsWith(this->owner->opContext());
}

bool Expression::isError(std::string* outputErrorMessage) const {
  if (this->owner == nullptr) {
    return false;
  }
  if (!this->startsWith(this->owner->opError(), 2)) {
    return false;
  }
  return (*this)[1].isOfType<std::string>(outputErrorMessage);
}

bool Expression::isSequenceDoubleButNotTripleNested() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (!this->isSequenceNElements()) {
    return false;
  }
  for (int i = 1; i < this->size(); i ++) {
    if (!((*this)[i].isSequenceNElements())) {
      return false;
    }
  }
  for (int i = 0; i < this->size(); i ++) {
    for (int j = 0; j < (*this)[i].size(); j ++) {
      if (!(*this)[i][j].isSequenceNElements()) {
        return true;
      }
    }
  }
  return false;
}

bool Expression::isSequenceNElements(int N) const {
  if (this->owner == nullptr) {
    return false;
  }
  return this->startsWith(this->owner->opSequence(), N + 1);
}

bool Expression::isIntervalRealLine() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->startsWith(this->owner->opIntervalOpen())) {
    return true;
  }
  if (this->startsWith(this->owner->opIntervalClosed(), 3)) {
    return true;
  }
  if (this->startsWith(this->owner->opIntervalLeftClosed(), 3)) {
    return true;
  }
  if (this->startsWith(this->owner->opIntervalRightClosed(), 3)) {
    return true;
  }
  return false;
}

bool Expression::isEqualToOne() const {
  int theInt;
  if (this->isSmallInteger(&theInt)) {
    return theInt == 1;
  }
  return false;
}

bool Expression::IsEqualToTwo() const {
  int theInt;
  if (this->isSmallInteger(&theInt)) {
    return theInt == 2;
  }
  return false;
}

bool Expression::IsEqualToHalf() const {
  Rational theRat;
  if (this->isRational(&theRat)) {
    Rational half(1, 2);
    return theRat == half;
  }
  return false;
}

bool Expression::IsKnownToBeNonNegative() const {
  MacroRegisterFunctionWithName("Expression::IsKnownToBeNonNegative");
  double theDouble = 0;
  if (this->evaluatesToDouble(&theDouble)) {
    return theDouble >= 0;
  }
  if (this->owner == nullptr) {
    return false;
  }
  Expression testInequality, testResult;
  Calculator& theCommands = *(this->owner);
  testInequality.MakeXOX(theCommands, theCommands.opGreaterThan(), *this, theCommands.EZero());
  if (this->owner->EvaluateExpression(theCommands, testInequality, testResult)) {
    if (testResult.isEqualToOne()) {
      return true;
    }
  }
  testInequality.MakeXOX(theCommands, theCommands.opGreaterThanOrEqualTo(), *this, theCommands.EZero());
  if (theCommands.EvaluateExpression(theCommands, testInequality, testResult)) {
    if (testResult.isEqualToOne()) {
      return true;
    }
  }
  return false;
}

bool Expression::IsNegativeConstant() const {
  if (this->owner == nullptr) {
    return false;
  }
  Rational theRat;
  if (this->isOfType<Rational>(&theRat)) {
    return theRat.isNegative();
  }
  if (!this->isConstantNumber()) {
    return false;
  }
  double tempD = - 1;
  if (!this->evaluatesToDouble(&tempD)) {
    return false;
  }
  return tempD < 0;
}

bool Expression::IsEqualToMOne() const {
  MacroRegisterFunctionWithName("Expression::IsEqualToMOne");
  if (!this->isConstantNumber()) {
    return false;
  }
  if (this->isOfType<Rational>()) {
    return this->getValue<Rational>() == - 1;
  }
  return false;
}

bool Expression::isEqualToZero() const {
  if (this->isOfType<Rational>()) {
    return this->getValue<Rational>().isEqualToZero();
  }
  if (this->isOfType<double>()) {
    return this->getValue<double>() == 0.0;
  }
  if (this->isOfType<AlgebraicNumber>()) {
    return this->getValue<AlgebraicNumber>().isEqualToZero();
  }
  if (this->isOfType<Polynomial<Rational> >()) {
    return this->getValue<Polynomial<Rational> >().isEqualToZero();
  }
  if (this->isOfType<RationalFunction>()) {
    return this->getValue<RationalFunction>().isEqualToZero();
  }
  return false;
}

bool Expression::makeError(const std::string& theError, Calculator& owner) {
  this->reset(owner, 2);
  this->checkConsistency();
  this->addChildAtomOnTop(owner.opError());
  this->checkConsistency();
  return this->addChildValueOnTop(theError);
}

bool Expression::isSmallInteger(int* whichInteger) const {
  Rational theRat;
  if (this->isOfType<Rational>(&theRat)) {
    return theRat.isSmallInteger(whichInteger);
  }
  AlgebraicNumber theAlgNum;
  if (this->isOfType<AlgebraicNumber>(&theAlgNum)) {
    return theAlgNum.isSmallInteger(whichInteger);
  }
  return false;
}

bool Expression::isRational(Rational* whichRational) const {
  if (this->isOfType<Rational>(whichRational)) {
    return true;
  }
  AlgebraicNumber theAlgNum;
  if (this->isOfType<AlgebraicNumber>(&theAlgNum)) {
    return theAlgNum.isRational(whichRational);
  }
  return false;
}

bool Expression::divisionByMeShouldBeWrittenInExponentForm() const {
  if (this->owner == nullptr) {
    return false;
  }
  RecursionDepthCounter theCounter(&this->owner->RecursionDeptH);
  if (this->owner->RecursionDepthExceededHandleRoughly()) {
    return false;
  }
  if (this->startsWith(this->owner->opTimes())) {
    for (int i = 1; i < this->children.size; i ++) {
      if (!(*this)[i].divisionByMeShouldBeWrittenInExponentForm()) {
        return false;
      }
    }
  }
  if (this->startsWith(this->owner->opThePower(), 3)) {
    return (*this)[1].divisionByMeShouldBeWrittenInExponentForm();
  }
  if (this->startsWith(this->owner->opPlus())) {
    return false;
  }
  return true;
}

bool Expression::isPositiveNumber() const {
  MacroRegisterFunctionWithName("Expression::isPositiveNumber");
  Rational theRat;
  if (this->isOfType<Rational>(&theRat)) {
    return theRat.isPositive();
  }
  double theDouble = - 1;
  if (!this->evaluatesToDouble(&theDouble)) {
    return false;
  }
  if (std::isnan(theDouble)) {
    return false;
  }
  return theDouble > 0;
}

bool Expression::isConstantNumber() const {
  MacroRegisterFunctionWithName("Expression::isConstantNumber");
  if (this->owner == nullptr) {
    return false;
  }
  RecursionDepthCounter thecounter(&this->owner->RecursionDeptH);
  if (this->owner->RecursionDepthExceededHandleRoughly("In Expression::isConstantNumber: ")) {
    return false;
  }
  if (this->isOfType<Rational>() || this->isOfType<AlgebraicNumber>() || this->isOfType<double>()) {
    return true;
  }
  if (this->isOperationGiven(this->owner->opPi())) {
    return true;
  }
  if (this->isOperationGiven(this->owner->opE())) {
    return true;
  }
  if (this->startsWithFunctionWithComplexRange()) {
    for (int i = 1; i < this->size(); i ++) {
      if (!(*this)[i].isConstantNumber()) {
        return false;
      }
    }
    return true;
  }
  return false;
}

bool Expression::IsAlgebraicRadical() const {
  MacroRegisterFunctionWithName("Expression::IsAlgebraicRadical");
  if (this->owner == nullptr) {
    return false;
  }
  RecursionDepthCounter thecounter(&this->owner->RecursionDeptH);
  if (this->owner->RecursionDepthExceededHandleRoughly("In Expression::IsAlgebraicRadical: ")) {
    return false;
  }
  if (this->isOfType<Rational>() || this->isOfType<AlgebraicNumber>()) {
    return true;
  }
  if (
    this->startsWith(this->owner->opPlus()) ||
    this->startsWith(this->owner->opMinus())||
    this->startsWith(this->owner->opTimes()) ||
    this->startsWith(this->owner->opDivide())
  ) {
    for (int i = 1; i < this->size(); i ++) {
      if (!(*this)[i].IsAlgebraicRadical()) {
        return false;
      }
    }
    return true;
  }
  if (this->startsWith(this->owner->opThePower(), 3)) {
    if (!(*this)[2].isRational()) {
      return false;
    }
    return (*this)[1].IsAlgebraicRadical();
  }
  if (this->startsWith(this->owner->opSqrt(), 3)) {
    if (!(*this)[1].isRational()) {
      return false;
    }
    return (*this)[2].IsAlgebraicRadical();
  }
  return false;
}

bool Expression::isInteger(LargeInteger* whichInteger) const {
  Rational theRat;
  if (!this->isOfType<Rational>(&theRat)) {
    return false;
  }
  return theRat.isInteger(whichInteger);
}

bool Expression::isIntegerNonNegative(LargeIntegerUnsigned* whichInteger) const {
  Rational theRat;
  if (!this->isOfType<Rational>(&theRat)) {
    return false;
  }
  if (theRat.IsNonPositive()) {
    return false;
  }
  if (whichInteger == nullptr) {
    return theRat.isInteger();
  }
  LargeInteger container;
  if (!theRat.isInteger(&container)) {
    return false;
  }
  *whichInteger = container.value;
  return true;
}

bool Expression::isIntegerFittingInInt(int* whichInteger) const {
  Rational theRat;
  if (!this->isOfType<Rational>(&theRat)) {
    return false;
  }
  return theRat.isIntegerFittingInInt(whichInteger);
}

bool Expression::makeAtom(const std::string& atomName, Calculator& newBoss) {
  this->reset(newBoss);
  this->theData = newBoss.AddOperationNoRepetitionOrReturnIndexFirst(atomName);
  return true;
}

void Expression::getBaseExponentForm(Expression& outputBase, Expression& outputExponent) const {
  this->checkInitialization();
  if (this->startsWith(this->owner->opThePower(), 3)) {
    outputBase = (*this)[1];
    outputExponent = (*this)[2];
    return;
  }
  outputBase = *this;
  outputExponent.assignValue(1, *this->owner);
}

bool Expression::getContext(ExpressionContext& output) const {
  output = this->getContext();
  return true;
}

ExpressionContext Expression::getContext() const {
  this->checkInitialization();
  ExpressionContext result(*this->owner);
  if (this->isBuiltInType()) {
    result.fromExpression((*this)[1]);
    return result;
  }
  if (this->isMatrix()) {
    if ((*this)[0].startsWith(this->owner->opMatriX())) {
      if ((*this)[0].size() > 2) {
        result.fromExpression((*this)[0][2]);
        return result;
      }
    }
  }
  global.fatal
  << "This is a programming error: getContext called on an Expression "
  << "that is not a built-in data type. "
  << "I can't display the expression as this may cause ``infinite'' "
  << "recursion if the error is caused by the toString method. "
  << "Here is however the lisp form "
  << this->toStringFull() << " of the expression. "
  << "Here's stack trace. " << global.fatal;
  return ExpressionContext(*this->owner);
}

int Expression::getNumberOfColumns() const {
  MacroRegisterFunctionWithName("Expression::getNumberOfColumns");
  if (!this->isSequenceNElements()) {
    return - 1;
  }
  int theMax = 1;
  for (int i = 1; i < this->size(); i ++) {
    if ((*this)[i].isSequenceNElements()) {
      theMax = MathRoutines::Maximum((*this)[i].size() - 1, theMax);
    }
  }
  return theMax;
}

void Expression::getMultiplicandsRecursive(List<Expression>& outputAppendList, int depth) const {
  MacroRegisterFunctionWithName("Expression::getMultiplicandsRecursive");
  this->checkInitialization();
  if (depth == 0) {
    outputAppendList.setSize(0);
  }
  if (!this->startsWith(this->owner->opTimes())) {
    outputAppendList.addOnTop(*this);
    return;
  }
  for (int i = 1; i < this->size(); i ++) {
    (*this)[i].getMultiplicandsDivisorsRecursive(outputAppendList, depth + 1);
  }
}

void Expression::getMultiplicandsDivisorsRecursive(List<Expression>& outputAppendList, int depth) const {
  MacroRegisterFunctionWithName("Expression::getMultiplicandsDivisorsRecursive");
  this->checkInitialization();
  if (depth == 0) {
    outputAppendList.setSize(0);
  }
  if (!this->startsWith(this->owner->opTimes()) && !this->startsWith(this->owner->opDivide())) {
    outputAppendList.addOnTop(*this);
    return;
  }
  for (int i = 1; i < this->size(); i ++) {
    (*this)[i].getMultiplicandsDivisorsRecursive(outputAppendList, depth + 1);
  }
}

void Expression::getCoefficientMultiplicandForm(Expression& outputCoeff, Expression& outputNoCoeff) const {
  MacroRegisterFunctionWithName("Expression::getCoefficientMultiplicandForm");
  this->checkInitialization();
  if (this->startsWith(this->owner->opTimes(), 3)) {
    if ((*this)[1].isConstantNumber()) {
      outputNoCoeff = (*this)[2];
      outputCoeff = (*this)[1];
      return;
    }
  }
  outputCoeff.assignValue(1, *this->owner);
  outputNoCoeff = *this;
}

void Expression::getCoefficientMultiplicandForm(Rational& outputCoeff, Expression& outputNoCoeff) const {
  this->checkInitialization();
  if (this->startsWith(this->owner->opTimes(), 3)) {
    if ((*this)[1].isOfType(&outputCoeff)) {
      outputNoCoeff = (*this)[2];
      return;
    }
  }
  outputCoeff = 1;
  outputNoCoeff = *this;
  return;
}

int Expression::getExpressionTreeSize() const {
  if (this->owner == nullptr) {
    return 1;
  }
  this->checkInitialization();
  RecursionDepthCounter theCounter(&this->owner->RecursionDeptH);
  if (this->owner->RecursionDepthExceededHandleRoughly("While computing Expression::getExpressionTreeSize: ")) {
    return 1;
  }
  if (this->isAtom()) {
    return 1;
  }
  int result = 0;
  for (int i = 0; i < this->children.size; i ++) {
    result += (*this)[i].getExpressionTreeSize();
  }
  return result;
}

bool Expression::operator>(const Expression& other) const {
  MacroRegisterFunctionWithName("Expression::operatorGreaterThan");
  double left = 0, right = 0;
  bool leftEvalsToDouble = this->evaluatesToDouble(&left);
  bool rightEvalsToDouble = other.evaluatesToDouble(&right);
  if (leftEvalsToDouble && rightEvalsToDouble) {
    if (std::isnan(left) && std::isnan(right)) {
      return false; //WARNING nan == nan evaluates to false!
    }
    // left != right generates a bizarre warning:
    // "comparing floating point with == or != is unsafe".
    if ((left - right) != 0.0) {
      if (std::isnan(left)) {
        return true;
      }
      if (std::isnan(right)) {
        return false;
      }
      return left > right;
    }
    if ((left - right) == 0.0) {
      if ((*this) == other) {
        return false;
      }
    }
  }
  if (leftEvalsToDouble && !rightEvalsToDouble) {
    return false;
  }
  if (!leftEvalsToDouble && rightEvalsToDouble) {
    return true;
  }
  Rational leftCoeff, rightCoeff;
  Expression leftMon, rightMon;
  this->getCoefficientMultiplicandForm(leftCoeff, leftMon);
  other.getCoefficientMultiplicandForm(rightCoeff, rightMon);
  if (leftMon == rightMon) {
    bool result = leftCoeff > rightCoeff;
    return result;
  }
  bool result = leftMon.GreaterThanNoCoeff(rightMon);
  return result;
}

bool Expression::GreaterThanNoCoeff(const Expression& other) const {
  MacroRegisterFunctionWithName("Expression::GreaterThanNoCoeff");
  if (this->isOfType<Rational>() && other.isOfType<Rational>()) {
    return this->getValue<Rational>() > other.getValue<Rational>();
  }
  if (this->isBuiltInType() && !other.isBuiltInType()) {
    return false;
  }
  if (!this->isBuiltInType() && other.isBuiltInType()) {
    return true;
  }
  int thisExpressionTreeSize = this->getExpressionTreeSize();
  int otherExpressionTreeSize = other.getExpressionTreeSize();
  if (thisExpressionTreeSize > otherExpressionTreeSize) {
    return true;
  }
  if (otherExpressionTreeSize > thisExpressionTreeSize) {
    return false;
  }
  if (this->children.size == 0 && other.children.size == 0) {
    std::string leftS, rightS;
    if (this->isOperation(&leftS)) {
      if (other.isOperation(&rightS)) {
        return leftS > rightS;
      }
    }
    return this->theData > other.theData;
  }
  if (this->children.size > other.children.size) {
    return true;
  }
  if (other.children.size > this->children.size) {
    return false;
  }
  for (int i = 0; i < this->children.size; i ++) {
    if ((*this)[i] > other[i]) {
      return true;
    } else if (other[i] > (*this)[i]) {
      return false;
    }
  }
  return false;
}

template<>
bool Expression::toStringBuiltIn<std::string>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  bool useQuotes = theFormat == nullptr ? false : theFormat->flagUseQuotes;
  bool isFinal = theFormat == nullptr ? true : theFormat->flagExpressionIsFinal;
  if (!useQuotes) {
    if (isFinal) {
      out
      << StringRoutines::ConvertStringToCalculatorDisplay(input.getValue<std::string>());
    } else {
      out << HtmlRoutines::convertStringToHtmlString(input.getValue<std::string>(), false);
    }
  } else {
    out << "\"" << StringRoutines::ConvertStringToJavascriptString(input.getValue<std::string>()) << "\"";
  }
  return true;
}

template<>
bool Expression::toStringBuiltIn<Rational>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  if (input.hasNonEmptyContext()) {
    out << "Rational{}(" << input[1].toString() << ", ";
  }
  if (!input.owner->flagUseFracInRationalLaTeX) {
    out << input.getValue<Rational>().toString();
  } else {
    out << input.getValue<Rational>().ToStringFrac();
  }
  if (input.hasNonEmptyContext()) {
    out << ")";
  }
  return true;
}

template<>
bool Expression::toStringBuiltIn<ElementEllipticCurve<Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  FormatExpressions format;
  input.getContext().getFormat(format);
  format.flagUseFrac = true;
  out << input.getValue<ElementEllipticCurve<Rational> >().toString(&format);
  return true;
}

template<>
bool Expression::toStringBuiltIn<ElementEllipticCurve<ElementZmodP> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  FormatExpressions format;
  input.getContext().getFormat(format);
  format.flagUseFrac = true;
  out << input.getValue<ElementEllipticCurve<ElementZmodP> >().toString(&format);
  return true;
}

template<>
bool Expression::toStringBuiltIn<ElementZmodP>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  out << input.getValue<ElementZmodP>().toString();
  return true;
}

template<>
bool Expression::toStringBuiltIn<InputBox>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  out << input.getValue<InputBox>().getUserInputBox();
  return true;
}

template<>
bool Expression::toStringBuiltIn<GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  out << input.getValue<GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> >().toString();
  return true;
}

template<>
bool Expression::toStringBuiltIn<ElementHyperoctahedralGroupR2>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  out << input.getValue<ElementHyperoctahedralGroupR2>().toString(theFormat);
  return true;
}

template<>
bool Expression::toStringBuiltIn<Polynomial<Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  bool showContext = input.owner == nullptr ? false : input.owner->flagDisplayContext;
  FormatExpressions format;
  input.getContext().getFormat(format);
  format.flagUseFrac = true;
  if (!input.owner->flagHidePolynomialBuiltInTypeIndicator) {
    out << "Polynomial{}(";
  }
  out << input.getValue<Polynomial<Rational> >().toString(&format);
  if (!input.owner->flagHidePolynomialBuiltInTypeIndicator) {
    out << ")";
  }
  if (showContext) {
    out << "[" << input.getContext().toString() << "]";
  }
  return true;
}

template<>
bool Expression::toStringBuiltIn<Polynomial<ElementZmodP> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  bool showContext = input.owner == nullptr ? false : input.owner->flagDisplayContext;
  FormatExpressions format;
  format.flagSuppressModP = !input.owner->flagHidePolynomialBuiltInTypeIndicator;
  const Polynomial<ElementZmodP>& polynomial = input.getValue<Polynomial<ElementZmodP> >();
  input.getContext().getFormat(format);
  format.flagUseFrac = true;
  if (!input.owner->flagHidePolynomialBuiltInTypeIndicator) {
    out << "PolynomialModP{}(";
  }
  out << polynomial.toString(&format);
  if (!input.owner->flagHidePolynomialBuiltInTypeIndicator) {
    if (!polynomial.isEqualToZero()) {
      out << "," << polynomial.coefficients[0].theModulus;
    }
    out << ")";
  }
  if (showContext) {
    out << "[" << input.getContext().toString() << "]";
  }
  return true;
}

template<>
bool Expression::toStringBuiltIn<Polynomial<AlgebraicNumber> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  FormatExpressions format;
  input.getContext().getFormat(format);
  bool showContext = input.owner == nullptr ? false : input.owner->flagDisplayContext;
  format.flagUseFrac = true;
  out << "PolynomialAlgebraicNumbers{}(";
  std::string currentString = input.getValue<Polynomial<AlgebraicNumber> >().toString(&format);
  if (currentString.size() > 0) {
    if (currentString[0] == '-') {
      currentString = currentString.substr(1);
      out << "-";
    }
  }
  if (input.owner->flagUseNumberColors) {
    out << "\\color{red}{";
  }
  out << currentString;
  if (input.owner->flagUseNumberColors) {
    out << "}";
  }
  out << ")";
  if (showContext) {
    out << "[" << input.getContext().toString() << "]";
  }
  return true;
}

template<>
bool Expression::toStringBuiltIn<PolynomialModuloPolynomial<ElementZmodP> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  FormatExpressions format;
  input.getContext().getFormat(format);
  format.flagUseFrac = true;
  const PolynomialModuloPolynomial<ElementZmodP>& element = input.getValue<PolynomialModuloPolynomial<ElementZmodP> >();
  LargeIntegerUnsigned modulus;
  if (!element.value.isEqualToZero()) {
    modulus = element.value.coefficients[0].theModulus;
  }
  format.flagSuppressModP = true;
  out
  << "PolynomialModP{}(" << element.value.toString(&format)
  << ", "
  << modulus
  << ") mod "
  << "PolynomialModP{}(" << element.modulus.toString(&format)
  << ", "
  << modulus
  << ") ";
  return true;
}

template<>
bool Expression::toStringBuiltIn<RationalFunction>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  bool showContext = input.owner == nullptr ? false : input.owner->flagDisplayContext;
  (void) theFormat;
  FormatExpressions format;
  input.getContext().getFormat(format);
  format.flagUseFrac = true;
  out << "MakeRationalFunction{}("
  << input.getValue<RationalFunction>().toString(&format) << ")";
  if (showContext) {
    out << "[" << input.getContext().toString() << "]";
  }
  return true;
}

template<>
bool Expression::toStringBuiltIn<Weight<Polynomial<Rational> > >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  FormatExpressions format;
  input.getContext().getFormat(format);
  format.flagFormatWeightAsVectorSpaceIndex = false;
  out << input.getValue<Weight<Polynomial<Rational> > >().toString(&format);
  return true;
}

template<>
bool Expression::toStringBuiltIn<SemisimpleLieAlgebra*>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  out << "SSLieAlg{}("
  << input.getValue<SemisimpleLieAlgebra*>()->ToStringLieAlgebraName()
  << ")";
  return true;
}

template<>
bool Expression::toStringBuiltIn<ElementUniversalEnveloping<RationalFunction> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  FormatExpressions format;
  input.getContext().getFormat(format);
  out << "UEE{}(" //<< input.getContext().toString() << ", "
  << input.getValue<ElementUniversalEnveloping<RationalFunction> >().toString(&format)
  << ")";
  return true;
}

template<>
bool Expression::toStringBuiltIn<MatrixTensor<Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  FormatExpressions format;
  input.getContext().getFormat(format);
  format.flagUseLatex = true;
  format.flagUseHTML = false;
  if (input.getValue<MatrixTensor<Rational> >().GetMinNumColsNumRows() < 20) {
    out << "MatrixRationalsTensorForm{}("
    << input.getValue<MatrixTensor<Rational> > ().toStringMatrixForm(&format)
    << ")";
  } else {
    out << input.getValue<MatrixTensor<Rational> >().toString();
  }
  return true;
}

template<>
bool Expression::toStringBuiltIn<ElementTensorsGeneralizedVermas<RationalFunction> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  FormatExpressions format;
  input.getContext().getFormat(format);
  out << "ETGVM{}(";
  out << input.getValue<ElementTensorsGeneralizedVermas<RationalFunction> >().toString(&format);
  out << ")";
  return true;
}

template<>
bool Expression::toStringBuiltIn<Plot>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  bool isFinal = theFormat == nullptr ? true : theFormat->flagExpressionIsFinal;
  if (isFinal) {
    Plot& thePlot = input.getValueNonConst<Plot>();
    thePlot.flagIncludeExtraHtmlDescriptions = (theFormat == nullptr) ? true : theFormat->flagIncludeExtraHtmlDescriptionsInPlots;
    thePlot.flagPlotShowJavascriptOnly = input.owner->flagPlotShowJavascriptOnly;
    out << thePlot.getPlotHtml(*input.owner);
    if (input.owner->flagWriteLatexPlots) {
      out << input.owner->WriteDefaultLatexFileReturnHtmlLink(thePlot.GetPlotStringAddLatexCommands(false), nullptr, true);
      out << "<br><b>LaTeX code used to generate the output. </b><br>"
      << thePlot.GetPlotStringAddLatexCommands(true);
    }
  } else {
    out << "(plot not shown)";
  }
  return true;
}

template<>
bool Expression::toStringBuiltIn<WeylGroupData>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  FormatExpressions format;
  WeylGroupData& theGroup = input.getValueNonConst<WeylGroupData>();
  format.flagUseLatex = true;
  format.flagUseHTML = false;
  format.flagUseReflectionNotation = true;
  out << theGroup.toString(&format);
  return true;
}

template<>
bool Expression::toStringBuiltIn<ElementWeylGroup >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  FormatExpressions format;
  const ElementWeylGroup& theElt = input.getValue<ElementWeylGroup>();
  format.flagUseLatex = true;
  format.flagUseHTML = false;
  format.flagUseReflectionNotation = true;
  out << theElt.toString(&format);
  return true;
}

template<>
bool Expression::toStringBuiltIn<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  FormatExpressions format;
  const GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& theElt =
  input.getValue<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >();
  format.flagUseLatex = true;
  format.flagUseHTML = false;
  format.flagUseReflectionNotation = true;
  out << theElt.toString(&format);
  return true;
}

template<>
bool Expression::toStringBuiltIn<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  FormatExpressions format;
  input.getContext().getFormat(format);
  const VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& theElt =
  input.getValue<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >();
  format.flagUseLatex = true;
  format.flagUseHTML = false;
  format.flagUseReflectionNotation = true;
  out << theElt.toString(&format);
  return true;
}

template<>
bool Expression::toStringBuiltIn<CharacterSemisimpleLieAlgebraModule<Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  CharacterSemisimpleLieAlgebraModule<Rational> theElt = input.getValue<CharacterSemisimpleLieAlgebraModule<Rational> >();
  out << theElt.toString();
  return true;
}

template<>
bool Expression::toStringBuiltIn<SemisimpleSubalgebras>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  FormatExpressions format;
  SemisimpleSubalgebras& theSubalgebras = input.getValueNonConst<SemisimpleSubalgebras>();
  format.flagUseLatex = true;
  format.flagUseHTML = true;
  format.flagCandidateSubalgebraShortReportOnly = false;
  format.flagIncludeMutableInformation = false;
  format.flagUseMathSpanPureVsMouseHover = false;
  out << theSubalgebras.toString(&format);
  return true;
}

template<>
bool Expression::toStringBuiltIn<double>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  std::string currentString = FloatingPoint::DoubleToString(input.getValue<double>());
  if (currentString.size() > 0) {
    if (currentString[0] == '-') {
      currentString = currentString.substr(1);
      out << "-";
    }
  }
  if (input.owner->flagUseNumberColors) {
    out << "\\color{blue}{";
  }
  out << currentString;
  if (input.owner->flagUseNumberColors) {
    out << "}";
  }
  return true;
}

template<>
bool Expression::toStringBuiltIn<AlgebraicNumber>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  FormatExpressions format;
  if (input.owner->flagUseFracInRationalLaTeX) {
    format.flagUseFrac = true;
  }
  std::string currentString = input.getValue<AlgebraicNumber>().toString(&format);
  if (currentString.size() > 0) {
    if (currentString[0] == '-') {
      currentString = currentString.substr(1);
      out << "-";
    }
  }
  if (input.owner->flagUseNumberColors) {
    out << "\\color{red}{";
  }
  out << currentString;
  if (input.owner->flagUseNumberColors) {
    out << "}";
  }
  return true;
}

template<>
bool Expression::toStringBuiltIn<LittelmannPath>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  out << input.getValue<LittelmannPath>().toString();
  return true;
}

template<>
bool Expression::toStringBuiltIn<ElementWeylAlgebra<Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  FormatExpressions format;
  bool showContext = input.owner == nullptr ? false : input.owner->flagDisplayContext;
  input.getContext().getFormat(format);
  format.flagUseHTML = false;
  format.flagUseLatex = true;
  out << "ElementWeylAlgebra{}(";
  out << input.getValue<ElementWeylAlgebra<Rational> >().toString(&format);
  out << ")";
  if (showContext) {
    out << "[" << input.getContext().toString() << "]";
  }
  return true;
}

bool Expression::toStringData(std::stringstream& out, FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("Expression::toStringData");
  if (this->owner == nullptr) {
    out << "(non-initialized)";
    return true;
  }
  if (this->isAtom()) {
    if (this->isOperationGiven(this->owner->opDifferential())) {
      out << "{\\text{d}}";
    } else if (this->isOperationGiven(this->owner->opPhantom())) {
      out << "\\phantom{}";
    } else if (this->owner->flagUseLnInsteadOfLog && this->isOperationGiven(this->owner->opLog())) {
      out << "\\ln";
    } else if (this->theData < this->owner->GetOperations().size && this->theData >= 0) {
      out << this->owner->GetOperations()[this->theData];
    } else {
      out << "[unknown atom of value " << this->theData << "]";
    }
    return true;
  }
  if (this->isMatrixOfType<RationalFunction>()) {
    FormatExpressions format;
    this->getContext().getFormat(format);
    format.flagUseHTML = false;
    format.flagUseLatex = true;
    Matrix<RationalFunction> theMat;
    this->owner->functionGetMatrix(*this, theMat);
    out << theMat.toString(&format);
    return true;
  }
  int theType = - 1;
  if (!this->isBuiltInType(&theType)) {
    return false;
  }
  Calculator& commands = *this->owner;
  // The following handlers are initialized in Calculator::initializeToStringHandlers.
  if (!commands.toStringDataHandlers.contains(theType)) {
    return false;
  }
  // handler must hold the function pointer:
  // Expression::toStringBuiltIn<builtInType>,
  // where builtInType is one of the types registered in
  // Calculator::initializeToStringHandlers.
  Expression::ToStringHandler handler = commands.toStringDataHandlers.getValueNoFail(theType);
  return handler(*this, out, theFormat);
}

std::string Expression::toStringSemiFull() const {
  std::stringstream out;
  if (this->toStringData(out)) {
    out << " ";
  } else {
    if (this->children.size > 0) {
      out << "(";
      for (int i = 0; i < this->children.size; i ++) {
        out << (*this)[i].toStringSemiFull();
        if (i != this->children.size - 1) {
          out << ", ";
        }
      }
      out << ")";
    }
  }
  return out.str();
}

std::string Expression::toStringFull() const {
  std::stringstream out;
  if (this->isAtom()) {
    out << this->theData << " ";
  }
  if (this->children.size > 0) {
    out << "(";
    for (int i = 0; i < this->children.size; i ++) {
      out << (*this)[i].toStringFull();
      if (i != this->children.size - 1) {
        out << ", ";
      }
    }
    out << ")";
  }
  return out.str();
}

bool Expression::needsParenthesisForBaseOfExponent() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->startsWith(this->owner->opBind())) {
    return (*this)[1].isList();
  }
  if (this->startsWith(this->owner->opPlus()) || this->startsWith(this->owner->opMinus())) {
    return true;
  }
  if (this->isOfType<Rational>()) {
    return this->getValue<Rational>() < 0 || !this->getValue<Rational>().isInteger();
  }
  if (this->isOfType<double>()) {
    return this->getValue<double>() < 0;
  }
  if (this->isOfType<AlgebraicNumber>()) {
    LargeInteger tempI;
    if (!this->getValue<AlgebraicNumber>().isInteger(&tempI)) {
      return true;
    }
    return tempI > 0;
  }
  if (this->isAtom() || this->children.size == 0) {
    return false;
  }
  return true;
}

bool Expression::needsParenthesisForMultiplicationWhenSittingOnTheRightMost(const Expression* leftNeighbor) const {
  if (this->owner == nullptr) {
    return false;
  }
  if (leftNeighbor != nullptr) {
    if (leftNeighbor->startsWith(this->owner->opDivide(), 3)) {
      if ((*leftNeighbor)[1] == "Differential") {
        return true;
      }
    }
  }
  if (
    this->startsWith(this->owner->opPlus()) ||
    this->startsWith(this->owner->opMinus()) ||
    this->startsWith(this->owner->opDefine())
  ) {
    return true;
  }
  if (this->isOfType<Rational>()) {
    return this->getValue<Rational>().needsParenthesisForMultiplicationWhenSittingOnTheRightMost();
  }
  if (this->isOfType<AlgebraicNumber>()) {
    return this->getValue<AlgebraicNumber>().needsParenthesisForMultiplicationWhenSittingOnTheRightMost();
  }
  if (this->isAtom() || this->size() == 0) {
    return false;
  }
  if (this->startsWith(this->owner->opUnderscore())) {
    return false;
  }
  if (this->startsWith(this->owner->opTimes(), 3)) {
    if ((*this)[1].isOfType<Rational>()) {
      return (*this)[1].getValue<Rational>().needsParenthesisForMultiplicationWhenSittingOnTheRightMost();
    }
    return false;
  }
  if (this->size() > 1) {
    const Expression& firstE = (*this)[0];
    if (firstE.isOperationGiven(this->owner->opTimes())) {
      return
      (*this)[1].needsParenthesisForMultiplicationWhenSittingOnTheRightMost() ||
      (*this)[1].needsParenthesisForMultiplication();
    }
    if (firstE.isOperationGiven(this->owner->opThePower())) {
      return false;
    }
    if (this->startsWith(this->owner->opBind())) {
      return (*this)[1].needsParenthesisForMultiplication();
    }
    return false;
  }
  return false;
}

bool Expression::needsParenthesisForAddition() const {
  MacroRegisterFunctionWithName("Expression::needsParenthesisForAddition");
  if (this->owner == nullptr) {
    return false;
  }
  if (this->startsWith(this->owner->opDefine())) {
    return true;
  }
  return false;
}

bool Expression::needsParenthesisForMultiplication(FormatExpressions* theFormat) const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->isOfType<std::string>()) {
    return false;
  }
  if (this->startsWith(this->owner->opAbsoluteValue())) {
    return false;
  }
  if (this->isMatrix()) {
    return false;
  }
  if (
    this->startsWith(this->owner->opPlus()) ||
    this->startsWith(this->owner->opMinus()) ||
    this->startsWith(this->owner->opDefine()) ||
    this->startsWith(this->owner->opLimit())
  ) {
    return true;
  }
  if (this->startsWith(this->owner->opDivide(), 3)) {
    if ((*this)[1] == "Differential") {
      return false;
    }
  }
  if (this->isSequenceNElements()) {
    if (this->size() > 2) {
      return false;
    }
  }
  if (this->isOfType<Rational>()) {
    return this->getValue<Rational>().needsParenthesisForMultiplication(theFormat);
  }
  if (this->isOfType<AlgebraicNumber>()) {
    return this->getValue<AlgebraicNumber>().needsParenthesisForMultiplication(theFormat);
  }
  if (this->isAtom() || this->children.size == 0) {
    return false;
  }
  if (this->startsWith(this->owner->opUnderscore())) {
    return false;
  }
  if (this->size() > 1) {
    const Expression& firstE = (*this)[0];
    if (firstE.isOperationGiven(this->owner->opTimes())) {
      return false;
    }
    if (firstE.isOperationGiven(this->owner->opThePower())) {
      const Expression& base = (*this)[1];
      if (base.children.size > 0) {
        if (base[0].isAtomWhoseExponentsAreInterpretedAsFunction()) {
          return true;
        }
      }
      return false;
    }
    if (this->startsWith(this->owner->opBind())) {
      return (*this)[1].needsParenthesisForMultiplication();
    }
    return true;
  }
  return false;
}

bool Calculator::innerFlattenCommandEnclosuresOneLayeR(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  if (input.size() != 2) {
    return false;
  }
  return Calculator::functionFlattenCommandEnclosuresOneLayer(
    theCommands, input[1], output
  );
}

bool Calculator::functionFlattenCommandEnclosuresOneLayer(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerFlattenCommandEnclosuresOneLayer");
  if (input.startsWith(theCommands.opCommandEnclosure())) {
    if (input.size() <= 1) {
      return false;
    }
    Expression result(theCommands);
    result.addChildAtomOnTop(theCommands.opCommandEnclosureStart());
    if (input.size() == 2) {
      result.addChildOnTop(input[1]);
      result.addChildAtomOnTop(theCommands.opCommandEnclosureFinish());
      output = result;
      return true;
    }
    for (int i = 1; i < input.size(); i ++) {
      result.addChildOnTop(input[i]);
    }
    result.addChildAtomOnTop(theCommands.opCommandEnclosureFinish());
    output = result;
    return true;
  }
  if (!input.startsWith(theCommands.opEndStatement())) {
    return false;
  }
  Expression result(theCommands);
  for (int i = 0; i < input.size(); i ++) {
    if (input[i].startsWith(theCommands.opCommandEnclosure())) {
      bool processed = false;
      result.addChildAtomOnTop(theCommands.opCommandEnclosureStart());
      if (input[i].size() == 2) {
        if (input[i][1].startsWith(theCommands.opEndStatement())) {
          for (int j = 1; j < input[i][1].size(); j ++) {
            result.addChildOnTop(input[i][1][j]);
          }
          processed = true;
        }
      }
      if (!processed) {
        for (int j = 1; j < input[i].size(); j ++) {
          result.addChildOnTop(input[i][j]);
        }
      }
      result.addChildAtomOnTop(theCommands.opCommandEnclosureFinish());
    } else if (input[i].startsWithGivenOperation("MatchesPattern")) {
      result.addChildAtomOnTop(theCommands.opCommandEnclosureStart());
      result.addChildOnTop(input[i]);
      result.addChildAtomOnTop(theCommands.opCommandEnclosureFinish());
    } else {
      result.addChildOnTop(input[i]);
    }
  }
  output = result;
  return true;
}

std::string Expression::toStringAllSlidersInExpression() const {
  HashedList<std::string, MathRoutines::HashString> boxNames;
  if (!this->HasInputBoxVariables(&boxNames)) {
    return "";
  }
  if (
    this->owner->theObjectContainer.userInputBoxSliderDisplayed.size <
    this->owner->theObjectContainer.theUserInputTextBoxesWithValues.size()
  ) {
    this->owner->theObjectContainer.resetSliders();
  }
  MapReferences<std::string, InputBox, MathRoutines::HashString>&
  theSliders = this->owner->theObjectContainer.theUserInputTextBoxesWithValues;
  std::stringstream out;
  for (int i = 0; i < boxNames.size; i ++) {
    if (!theSliders.contains(boxNames[i])) {
      out << boxNames[i] << " not found. ";
      continue;
    }
    int theIndex = theSliders.getIndex(boxNames[i]);
    if (this->owner->theObjectContainer.userInputBoxSliderDisplayed[theIndex]) {
      continue;
    }
    this->owner->theObjectContainer.userInputBoxSliderDisplayed[theIndex] = true;
    InputBox& theBox = theSliders.theValues[theIndex];
    std::string theSliderName = theBox.GetSliderName();
    out << "<input name =\""
    << theSliderName
    << "\" type =\"range\"";
    double theReader = 0;
    out << std::fixed;
    out.precision(4);
    if (theBox.min.evaluatesToDouble(&theReader)) {
      out << "min =\"" << theReader << "\" ";
    }
    if (theBox.max.evaluatesToDouble(&theReader)) {
      out << "max =\"" << theReader << "\" ";
    }
    if (theBox.step.evaluatesToDouble(&theReader)) {
      out << "step =\"" << theReader << "\" ";
    }
    if (theBox.value.evaluatesToDouble(&theReader)) {
      out << "value =\"" << theReader << "\" ";
    } else {
      out << "value =\"1\" ";
    }
    out << "oninput =\"window.calculator.calculator.updateSliderToInputBox('"
    << boxNames[i]
    << "','"
    << theSliderName
    << "');\"></input>";
  }
  return out.str();
}

JSData Expression::ToJSData(FormatExpressions* theFormat, const Expression& startingExpression) const {
  MacroRegisterFunctionWithName("Expression::ToJSData");
  JSData result, input, output;
  input.theType = JSData::token::tokenArray;
  output.theType = JSData::token::tokenArray;
  if (this->isListStartingWithAtom(this->owner->opEndStatement())) {
    for (int i = 1; i < this->size(); i ++) {
      const Expression currentE = (*this)[i];
      if (!this->owner->flagHideLHS) {
        if (i < startingExpression.size()) {
          input[i - 1] = startingExpression[i].toString(theFormat);
        } else {
          input[i - 1] = "No matching starting expression - possible use of the Melt keyword.";
        }
      } else {
        input[i - 1] = "...";
      }
      std::stringstream out;
      if (currentE.isOfType<std::string>()) {
        out << currentE.getValue<std::string>();
      } else if (
        currentE.hasType<Plot> () ||
        currentE.isOfType<SemisimpleSubalgebras>() ||
        currentE.isOfType<WeylGroupData>() ||
        currentE.isOfType<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >()
      ) {
        out << currentE.toString(theFormat);
      } else {
        out << HtmlRoutines::GetMathSpanPure(currentE.toString(theFormat), 1700);
      }
      out << currentE.toStringAllSlidersInExpression();
      output[i - 1] = out.str();
    }
  } else {
    input[0] = HtmlRoutines::GetMathSpanPure(startingExpression.toString(theFormat), 1700);
    if (
      this->isOfType<std::string>() || this->isOfType<Plot>() ||
      this->isOfType<SemisimpleSubalgebras>() || this->isOfType<WeylGroupData>()
    ) {
      output[0] = this->toString(theFormat);
    } else {
      output[0] = HtmlRoutines::GetMathSpanPure(this->toString(theFormat), 1700);
    }
  }
  result["input"] = input;
  result["output"] = output;
  return result;
}

bool Expression::toStringTimes(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opTimes(), 3)) {
    return false;
  }
  std::string secondE = input[2].toString(theFormat);
  if (input[1].isOperationGiven(input.owner->opSqrt())) {
    // A malformed expression such as: "\sqrt 3" will be parsed as "sqrt * 3"
    // and later corrected to "\sqrt{3}".
    out << "\\sqrt{" << secondE << "}";
    return true;
  }
  std::string firstE = input[1].toString(theFormat);
  bool firstNeedsBrackets = input[1].needsParenthesisForMultiplication();
  bool secondNeedsBrackets = input[2].needsParenthesisForMultiplicationWhenSittingOnTheRightMost(&(input[1]));
  if (secondE.size() > 0) {
    if (secondE[0] == '-') {
      secondNeedsBrackets = true;
    }
  }
  bool mustHaveTimes = false;
  bool collapseUnits = true;
  if (theFormat != nullptr) {
    collapseUnits = !theFormat->flagDontCollalpseProductsByUnits;
  }
  if (collapseUnits) {
    if (firstE == "-1" || firstE == "- 1") {
      firstE = "-";
      firstNeedsBrackets = false;
    }
    if (firstE == "1") {
      firstE = "";
    }
  }
  if (firstNeedsBrackets) {
    out << "\\left(" << firstE << "\\right)";
  } else {
    out << firstE;
  }
  bool mustHaveSpace = true;
  if (!firstNeedsBrackets && !secondNeedsBrackets && firstE != "" && firstE != "-") {
    if (MathRoutines::isADigit(secondE[0])) {
      mustHaveTimes = true;
    }
    if (MathRoutines::isADigit(firstE[firstE.size() - 1]) ) {
      if (StringRoutines::StringBeginsWith(secondE, "\\frac")) {
        mustHaveTimes = true;
      }
    }
  }
  if (firstNeedsBrackets || mustHaveTimes || firstE == "" || firstE == "-") {
    mustHaveSpace = false;
  }
  if (mustHaveTimes) {
    out << "\\cdot ";
  } else if (mustHaveSpace) {
    out << " ";
  }
  if (secondNeedsBrackets) {
    out << "\\left(" << secondE << "\\right)";
  } else {
    out << secondE;
  }
  return true;
}

void Expression::toStringOpMultiplicative(
  std::stringstream& out,
  const std::string& operation,
  FormatExpressions* theFormat
) const {
  std::string secondE = (*this)[2].toString(theFormat);
  std::string firstE = (*this)[1].toString(theFormat);
  bool firstNeedsBrackets = (*this)[1].needsParenthesisForMultiplication();
  bool secondNeedsBrackets = (*this)[2].needsParenthesisForMultiplication();
  if (firstE == "- 1" || firstE == "-1") {
    firstE = "-";
    firstNeedsBrackets = false;
  }
  if (firstE == "1") {
    firstE = "";
  }
  if (firstNeedsBrackets) {
    out << "\\left(" << firstE << "\\right)";
  } else {
    out << firstE;
  }
  out << operation << " ";
  if (secondNeedsBrackets) {
    out << "\\left(" << secondE << "\\right)";
  } else {
    out << secondE;
  }
}

std::string Expression::toStringTreeHtml(int depth) const {
  std::stringstream out;
  if (depth > 100) {
    out << "<br>...";
    return out.str();
  }
  std::stringstream data;
  if (this->toStringData(data)) {
    out << "<br>";
    for (int i = 0; i < depth; i ++) {
      out << "_";
    }
    if (data.str() == "" && this->isOfType<std::string>()) {
      out << "[empty string]";
    }
    out << data.str();
    return out.str();
  }
  for (int i = 0; i < this->size(); i ++) {
    out << (*this)[i].toStringTreeHtml(depth + 1);
  }
  return out.str();
}

bool Expression::toStringDivide(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  Calculator& commands = *input.owner;
  if (!input.startsWith(commands.opDivide(), 3)) {
    return false;
  }
  bool doUseFrac = commands.flagUseFracInRationalLaTeX;
  if (
    doUseFrac &&
    (input[1].startsWith(commands.opTimes()) || input[1].startsWith(commands.opDivide()))
  ) {
    List<Expression> multiplicandsLeft;
    input.getMultiplicandsDivisorsRecursive(multiplicandsLeft, 0);
    for (int i = 0; i < multiplicandsLeft.size; i ++) {
      if (
        multiplicandsLeft[i].startsWith(commands.opIntegral()) ||
        multiplicandsLeft[i].isOperationGiven(commands.opIntegral())
      ) {
        doUseFrac = false;
        break;
      }
    }
  }
  if (!doUseFrac) {
    std::string firstE = input[1].toString(theFormat);
    std::string secondE = input[2].toString(theFormat);
    bool firstNeedsBrackets =
      !(input[1].isListStartingWithAtom(commands.opTimes()) ||
      input[1].isListStartingWithAtom(commands.opDivide()));
    bool secondNeedsBrackets = true;
    if (input[2].isOfType<Rational>()) {
      if (input[2].getValue<Rational>().isInteger()) {
        secondNeedsBrackets = false;
      }
    }
    if (firstNeedsBrackets) {
      out << "(" << firstE << ")";
    } else {
      out << firstE;
    }
    out << "/";
    if (secondNeedsBrackets) {
      out << "(" << secondE << ")";
    } else {
      out << secondE;
    }
  } else {
    StateMaintainer<bool> maintain;
    if (theFormat != nullptr) {
      maintain.initialize(theFormat->flagExpressionNewLineAllowed);
      theFormat->flagExpressionNewLineAllowed = false;
    }
    std::string firstE = input[1].toString(theFormat);
    std::string secondE = input[2].toString(theFormat);
    out << "\\frac{" << firstE << "}{" << secondE << "}";
  }
  return true;
}

bool Expression::toStringPower(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  Calculator& commands = *input.owner;
  if (!input.startsWith(commands.opThePower(), 3)) {
    return false;
  }
  bool involvesExponentsInterpretedAsFunctions = false;
  const Expression& firstE = input[1];
  const Expression& secondE = input[2];
  if (firstE.startsWith(- 1, 2)) {
    bool shouldProceed = firstE[0].isAtomWhoseExponentsAreInterpretedAsFunction() &&
    !secondE.IsEqualToMOne() && secondE.isRational();
    if (
      shouldProceed && firstE[0].isOperationGiven(commands.opLog()) &&
      commands.flagUseLnAbsInsteadOfLogForIntegrationNotation
    ) {
      shouldProceed = false;
    }
    if (shouldProceed) {
      involvesExponentsInterpretedAsFunctions = true;
      Expression newFunE;
      newFunE.MakeXOX(*input.owner, commands.opThePower(), firstE[0], input[2]);
      newFunE.checkConsistency();
      out << "{" << newFunE.toString(theFormat) << "}{}";
      if (
        firstE[1].needsParenthesisForMultiplicationWhenSittingOnTheRightMost() ||
        firstE[1].startsWith(commands.opTimes())
      ) {
        out << "\\left(" << firstE[1].toString(theFormat) << "\\right)";
      } else {
        out << firstE[1].toString(theFormat);
      }
    }
  }
  if (!involvesExponentsInterpretedAsFunctions) {
    bool isSqrt = false;
    if (input[2].isOfType<Rational>()) {
      if (input[2].getValue<Rational>().isEqualTo(Rational(1, 2))) {
        isSqrt = true;
      }
    }
    if (isSqrt) {
      out << "\\sqrt{" << input[1].toString(theFormat) << "}";
    } else {
      std::string secondEstr = input[2].toString(theFormat);
      std::string firstEstr = input[1].toString(theFormat);
      if (input[1].needsParenthesisForBaseOfExponent()) {
        bool useBigParenthesis = true;
        if (input[1].startsWith(commands.opDivide())) {
          useBigParenthesis = true;
        }
        if (useBigParenthesis) {
          out << "\\left(";
        } else {
          out << "(";
        }
        out << firstEstr;
        if (useBigParenthesis) {
          out << "\\right)";
        } else {
          out << ")";
        }
      } else {
        out << firstEstr;
      }
      out << "^{" << secondEstr << "}";
    }
  }
  return true;
}

bool Expression::toStringGeneral(
  std::stringstream& out, FormatExpressions* theFormat
) const {
  if (this->size() < 2) {
    return false;
  }
  out << (*this)[0].toString(theFormat);
  bool needParenthesis = true;
  if (this->size() == 2) {
    if ((*this)[0].isAtomWhoseExponentsAreInterpretedAsFunction()) {
      needParenthesis = !(*this)[1].isAtom();
    }
    if ((*this)[0].isPowerOfAtomWhoseExponentsAreInterpretedAsFunction()) {
      needParenthesis = !(*this)[1].isAtom();
    }
  }
  out << "{}";
  if (needParenthesis) {
    out << "\\left(";
  }
  for (int i = 1; i < this->children.size; i ++) {
    out << (*this)[i].toString(theFormat);
    if (i != this->children.size - 1) {
      out << ", ";
    }
  }
  if (needParenthesis) {
    out << "\\right)";
  }
  return true;
}

bool Expression::toStringEndStatement(
  std::stringstream& out,
  Expression* startingExpression,
  JSData* outputJS,
  FormatExpressions* theFormat
) const {
  if (!this->isListStartingWithAtom(this->owner->opEndStatement())) {
    return false;
  }
  MemorySaving<FormatExpressions> temporaryFormat;
  if (theFormat == nullptr) {
    theFormat = &temporaryFormat.getElement();
    theFormat->flagExpressionIsFinal = true;
  }
  bool isFinal = theFormat->flagExpressionIsFinal;
  bool createTable = (startingExpression != nullptr);
  bool createSingleTable = false;
  if (createTable == false && theFormat != nullptr && !global.flagRunningConsoleRegular) {
    createSingleTable = theFormat->flagMakingExpressionTableWithLatex;
    theFormat->flagMakingExpressionTableWithLatex = false;
  }
  if (!createSingleTable && !createTable && this->size() > 2) {
    out << "(";
  }
  if (createSingleTable) {
    out << "<table class =\"tableCalculatorOutput\">";
  }
  std::string currentInput, currentOutput;
  if (outputJS != nullptr) {
    (*outputJS)["input"].theType = JSData::token::tokenArray;
    (*outputJS)["output"].theType = JSData::token::tokenArray;
  }
  for (int i = 1; i < this->size(); i ++) {
    const Expression currentE = (*this)[i];
    if (createTable) {
      out << "<tr><td class =\"cellCalculatorInput\">";
      if (!this->owner->flagHideLHS) {
        if (i < (*startingExpression).size()) {
          theFormat->flagDontCollalpseProductsByUnits = true;
          currentInput = HtmlRoutines::GetMathSpanPure((*startingExpression)[i].toString(theFormat));
        } else {
          currentInput = "No matching starting expression - possible use of the Melt keyword.";
        }
      } else {
        currentInput = "...";
      }
      out << currentInput;
      if (outputJS != nullptr) {
        (*outputJS)["input"][i - 1] = currentInput;
      }
      if (i != this->size() - 1) {
        out << ";";
      }
      out << "</td><td class =\"cellCalculatorResult\">";
      if ((*this)[i].isOfType<std::string>() && isFinal) {
        currentOutput = StringRoutines::ConvertStringToCalculatorDisplay(currentE.getValue<std::string>());
      } else if ((
          currentE.hasType<Plot> () ||
          currentE.isOfType<SemisimpleSubalgebras>() ||
          currentE.isOfType<WeylGroupData>() ||
          currentE.isOfType<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >()
        ) && isFinal
      ) {
        theFormat->flagDontCollalpseProductsByUnits = false;
        currentOutput = currentE.toString(theFormat);
      } else {
        theFormat->flagDontCollalpseProductsByUnits = false;
        currentOutput = HtmlRoutines::GetMathSpanPure(currentE.toString(theFormat), 1700);
      }
      currentOutput += currentE.toStringAllSlidersInExpression();
      if (outputJS != nullptr) {
        (*outputJS)["output"][i - 1] = currentOutput;
      }
      out << currentOutput;
      out << "</td></tr>";
    } else {
      bool addLatexDelimiter = true;
      std::stringstream outWithDelimiter;
      if (createSingleTable) {
        out << "<tr><td>\n";
        addLatexDelimiter = !currentE.hasType<Plot>();
        if (addLatexDelimiter) {
          outWithDelimiter << "\\(";
        }
      }
      outWithDelimiter << currentE.toString(theFormat);
      if (createSingleTable && addLatexDelimiter) {
        outWithDelimiter << "\\)";
      }
      outWithDelimiter << currentE.toStringAllSlidersInExpression();
      out << outWithDelimiter.str();
      if (outputJS != nullptr) {
        (*outputJS)["output"][i] = outWithDelimiter.str();
      }
      if (createSingleTable) {
        out << "</td><tr>\n";
      }
      if (i != this->size() - 1 && !createSingleTable) {
        out << ";";
      }
    }
    if (theFormat != nullptr) {
      theFormat->flagExpressionIsFinal = isFinal;
    }
  }
  if (createSingleTable) {
    out << "</table>";
  }
  if (!createSingleTable && !createTable && this->size() > 2) {
    out << ")";
  }
  return true;
}

bool Expression::toStringPlus(const Expression& input, std::stringstream& out, FormatExpressions* theFormat) {
  if (!input.startsWith(input.owner->opPlus())) {
    return false;
  }
  if (input.children.size < 3) {
    global.fatal << "Plus operation takes at least 2 arguments, whereas this expression has "
    << input.children.size - 1 << " arguments. " << global.fatal;
  }
  const Expression& left = input[1];
  const Expression& right = input[2];
  std::string leftString;
  if (left.needsParenthesisForAddition()) {
    leftString = "\\left(" + left.toString(theFormat) + "\\right)";
  } else {
    leftString = left.toString(theFormat);
  }
  out << leftString;
  bool allowNewLine = false;
  if (theFormat != nullptr) {
    allowNewLine = theFormat->flagExpressionNewLineAllowed;
  }
  bool useFrac = input.owner->flagUseFracInRationalLaTeX;
  if (allowNewLine && !useFrac && leftString.size() > static_cast<unsigned>(FormatExpressions::ExpressionLineBreak)) {
    out << "\\\\\n";
  }
  std::string rightString = right.toString(theFormat);
  if (right.needsParenthesisForAddition()) {
    rightString = "\\left(" + right.toString(theFormat) + "\\right)";
  }
  if (rightString.size() > 0) {
    if (rightString[0] != '-') {
      out << "+";
    }
  }
  out << rightString;
  return true;
}

bool Expression::toStringDirectSum(const Expression& input, std::stringstream& out, FormatExpressions* theFormat) {
  if (!input.isListStartingWithAtom(input.owner->opDirectSum())) {
    return false;
  }
  if (input.children.size < 3) {
    global.fatal << "Direct sum operation takes at least 2 arguments, whereas this expression has "
    << input.children.size << " arguments. " << global.fatal;
  }
  const Expression& left = input[1];
  const Expression& right = input[2];
  std::string leftString;
  if (left.needsParenthesisForAddition()) {
    leftString = "\\left(" + left.toString(theFormat) + "\\right)";
  } else {
    leftString = left.toString(theFormat);
  }
  out << leftString;
  bool allowNewLine = false;
  if (theFormat != nullptr) {
    allowNewLine = theFormat->flagExpressionNewLineAllowed;
  }
  bool useFrac = input.owner->flagUseFracInRationalLaTeX;
  if (allowNewLine && !useFrac && leftString.size() > static_cast<unsigned>(FormatExpressions::ExpressionLineBreak)) {
    out << "\\\\\n";
  }
  std::string rightString = right.needsParenthesisForAddition() ?
  ("\\left(" + right.toString(theFormat) + "\\right)") : right.toString(theFormat);
  if (rightString.size() > 0) {
    if (rightString[0] != '-') {
      out << "\\oplus ";
    }
  }
  out << rightString;
  return true;
}

bool Expression::toStringSequence(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.isListStartingWithAtom(input.owner->opSequence())) {
    return false;
  }
  if (input.size() == 2) {
    out << "{Sequence{}";
  }
  out << "\\left(";
  int charCounter = 0;
  bool allowNewLine = false;
  if (theFormat != nullptr) {
    allowNewLine = theFormat->flagExpressionNewLineAllowed;
  }
  for (int i = 1; i < input.size(); i ++) {
    std::string currentChildString = input[i].toString(theFormat);
    out << currentChildString;
    charCounter += currentChildString.size();
    if (i != input.children.size - 1) {
      out << ", ";
      if (theFormat != nullptr) {
        if (allowNewLine && charCounter > 50) {
          if (theFormat->flagUseLatex) {
            out << "\\\\\n";
          }
          if (theFormat->flagUseHTML) {
            out << "\n<br>\n";
          }
        }
      }
    }
    charCounter %= 50;
  }
  out << "\\right)";
  if (input.size() == 2) {
    out << "}";
  }
  return true;
}

bool Expression::toStringMatrix(const Expression& input, std::stringstream& out, FormatExpressions* theFormat) {
  if (!input.isMatrix()) {
    return false;
  }
  if (theFormat->flagUseLatex && !theFormat->flagUsePmatrix) {
    out << "\\left(";
  }
  if (!theFormat->flagUsePmatrix) {
    int numCols = input.getNumberOfColumns();
    out << "\\begin{array}{";
    for (int i = 0; i < numCols; i ++) {
      out << "c";
    }
    out << "} ";
  } else {
    out << "\\begin{pmatrix}";
  }
  for (int i = 1; i < input.size(); i ++) {
    for (int j = 1; j < input[i].size(); j ++) {
      out << input[i][j].toString(theFormat);
      if (j != input[i].size() - 1) {
        out << " & ";
      }
    }
    if (i != input.size() - 1) {
      out << "\\\\ \n";
    }
  }
  if (theFormat->flagUsePmatrix) {
    out << " \\end{pmatrix}";
  } else {
    out << " \\end{array}";
  }
  if (theFormat->flagUseLatex && !theFormat->flagUsePmatrix) {
    out << "\\right)";
  }
  return true;
}

bool Expression::toStringDefine(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opDefine(), 3)) {
    return false;
  }
  std::string firstE  = input[1].toString(theFormat);
  std::string secondE = input[2].toString(theFormat);
  if (
    input[1].isListStartingWithAtom(input.owner->opDefine()) ||
    input[1].isListStartingWithAtom(input.owner->opGreaterThan()) ||
    input[1].isListStartingWithAtom(input.owner->opGreaterThanOrEqualTo()) ||
    input[1].isListStartingWithAtom(input.owner->opLessThan()) ||
    input[1].isListStartingWithAtom(input.owner->opLessThanOrEqualTo())
  ) {
    out << "\\left(" << firstE << "\\right)";
  } else {
    out << firstE;
  }
  out << "=";
  if (input[2].isListStartingWithAtom(input.owner->opDefine())) {
    out << "\\left(" << secondE << "\\right)";
  } else {
    out << secondE;
  }
  return true;
}

bool Expression::toStringLnAbsoluteInsteadOfLogarithm(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (
    !input.owner->flagUseLnAbsInsteadOfLogForIntegrationNotation ||
    !input.startsWith(input.owner->opLog(), 2)
  ) {
    return false;
  }
  std::string theArg = input[1].toString(theFormat);
  if (!StringRoutines::StringBeginsWith(theArg, "\\left|")) {
    out << "\\ln \\left|" << theArg << "\\right|";
  } else {
    out << "\\ln " << theArg;
  }
  return true;
}

bool Expression::toStringDifferential(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opDifferential())) {
    return false;
  }
  if (input.size() == 2) {
    return Expression::toStringDifferential2(input, out, theFormat);
  }
  return Expression::toStringDifferential3(input, out, theFormat);
}

bool Expression::toStringDifferential3(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opDifferential(), 3)) {
    return false;
  }
  bool needsParen = input[2].needsParenthesisForMultiplication() ||
  input[2].needsParenthesisForMultiplicationWhenSittingOnTheRightMost();
  if (input[2].startsWith(input.owner->opDivide())) {
    needsParen = false;
  }
  bool rightNeedsParen = (!input[1].isAtom()) && (!input[1].isBuiltInType());
  std::string theCoeff = input[2].toString(theFormat);
  if (theCoeff == "1") {
    needsParen = false;
    theCoeff = "";
  }
  if (needsParen) {
    out << "\\left(";
  }
  out << theCoeff;
  if (needsParen) {
    out << "\\right)";
  }
  out << " {\\text{d}} ";
  if (rightNeedsParen) {
    out << "\\left(";
  }
  out << input[1].toString(theFormat);
  if (rightNeedsParen) {
    out << "\\right)";
  }
  return true;
}

bool Expression::toStringDifferentiate(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opDifferentiate(), 3)) {
    return false;
  }
  out << "\\frac{\\text{d}} ";
  if (input[2].needsParenthesisForMultiplication()) {
    out << "\\left(" << input[2].toString(theFormat)
    << "\\right)";
  } else {
    out << input[2].toString(theFormat);
  }
  out << "}{{\\text{d}} "
  << input[1].toString(theFormat) << "}";
  return true;
}

bool Expression::toStringDifferential2(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opDifferential(), 2)) {
    return false;
  }
  bool needsParen = (!input[1].isAtom()) && (!input[1].isBuiltInType());
  out << "{\\text{d}}{}";
  if (needsParen) {
    out << "\\left(";
  }
  out << input[1].toString(theFormat);
  if (needsParen) {
    out << "\\right)";
  }
  return true;
}

bool Expression::toStringFactorial(const Expression& input, std::stringstream& out, FormatExpressions* theFormat) {
  if (!input.startsWith(input.owner->opFactorial(), 2)) {
    return false;
  }
  if (input[1].needsParenthesisForBaseOfExponent()) {
    out << "\\left(" << input[1].toString(theFormat) << "\\right) !";
  } else {
    out << input[1].toString(theFormat) << "!";
  }
  return true;
}

bool Expression::toStringSqrt(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opSqrt())) {
    return false;
  }
  if (input.size() == 2) {
    return Expression::toStringSqrt2(input, out, theFormat);
  }
  return Expression::toStringSqrt3(input, out, theFormat);
}

bool Expression::toStringSqrt2(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opSqrt(), 2)) {
    return false;
  }
  out << "\\sqrt{" << input[1].toString(theFormat) << "}";
  return true;
}

bool Expression::toStringSqrt3(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opSqrt(), 3)) {
    return false;
  }
  int thePower = 0;
  bool hasPowerTwo = input[1].isSmallInteger(&thePower);
  if (hasPowerTwo) {
    hasPowerTwo = (thePower == 2);
  }
  if (hasPowerTwo) {
    out << "\\sqrt{" << input[2].toString(theFormat) << "}";
  } else {
    out << "\\sqrt[" << input[1].toString(theFormat)
    << "]{" << input[2].toString(theFormat) << "}";
  }
  return true;
}

bool Expression::toStringGreaterThan(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.isListStartingWithAtom(input.owner->opGreaterThan())) {
    return false;
  }
  out << input[1].toString(theFormat) << "&gt;" << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringLessThanOrEqualTo(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.isListStartingWithAtom(input.owner->opLessThanOrEqualTo())) {
    return false;
  }
  out << input[1].toString(theFormat) << "\\leq " << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringLessThan(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.isListStartingWithAtom(input.owner->opLessThan())) {
    return false;
  }
  out << input[1].toString(theFormat) << "&lt;" << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringLimitProcess(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.isListStartingWithAtom(input.owner->opLimitProcess())) {
    return false;
  }
  out << input[1].toString(theFormat) << " \\to " << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringGreaterThanOrEqualTo(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.isListStartingWithAtom(input.owner->opGreaterThanOrEqualTo())) {
    return false;
  }
  out << input[1].toString(theFormat) << "\\geq " << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringError(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opError(), 2)) {
    return false;
  }
  input.checkInitialization();
  input.owner->NumErrors ++;
  out << "\\text{Error number "
  << input.owner->NumErrors
  << ": " << input[1].toString(theFormat) << "}"
  ;
  return true;
}

bool Expression::toStringSumOrIntegral(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (
    !input.startsWith(input.owner->opSum()) &&
    !input.startsWith(input.owner->opIntegral())
  ) {
    return false;
  }
  std::string opString = input[0].toString(theFormat);
  out << opString << " ";
  int firstIndex = 2;
  if (input.size() >= 2) {
    if (input[1].startsWith(input.owner->opLimitBoundary(), 3)) {
      out
      << "_{" << input[1][1].toString(theFormat) << "}"
      << "^{" << input[1][2].toString(theFormat) << "}";
    } else if (input[1].isOperationGiven(input.owner->opIndefiniteIndicator())) {
      firstIndex = 2;
    } else {
      firstIndex = 1;
    }
  }
  if (input.size() <= firstIndex + 1) {
    if (input.size() == firstIndex + 1) {
      out << input[firstIndex].toString(theFormat);
    }
  } else {
    out << "(";
    for (int i = firstIndex; i < input.size(); i ++) {
      out << input[i].toString(theFormat);
      if (i != input.size() - 1) {
        out << ", ";
      }
    }
    out << ")";
  }
  return true;
}

bool Expression::toStringLimit(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opLimit(), 3)) {
    return false;
  }
  out << "\\lim_{";
  if (!input[1].isSequenceNElements()) {
    out << input[1].toString(theFormat);
  } else {
    for (int i = 1; i < input[1].size(); i ++) {
      out << input[1][i].toString(theFormat);
      if (i != input[1].size() - 1) {
        out << ", ";
      }
    }
  }
  out << "}" << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringUnion(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.isListStartingWithAtom(input.owner->opUnion()) || input.size() != 3) {
    return false;
  }
  if (input[1].startsWith(input.owner->opIntersection())) {
    out << "\\left(" << input[1].toString(theFormat) <<  "\\right)"
    << "\\cup " << input[2].toString(theFormat);
  } else {
    out << input[1].toString(theFormat) << "\\cup " << input[2].toString(theFormat);
  }
  return true;
}

bool Expression::toStringUnionNoRepetition(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.isListStartingWithAtom(input.owner->opUnionNoRepetition())) {
    return false;
  }
  out << input[1].toString(theFormat) << "\\sqcup " << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringIntersection(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.isListStartingWithAtom(input.owner->opIntersection()) || input.size() != 3) {
    return false;
  }
  if (input[1].startsWith(input.owner->opUnion())) {
    out << "\\left(" << input[1].toString(theFormat) << "\\right)"
    << "\\cap " << input[2].toString(theFormat);
  } else {
    out << input[1].toString(theFormat) << "\\cap " << input[2].toString(theFormat);
  }
  return true;
}

std::string Expression::toStringWithStartingExpression(
  FormatExpressions* theFormat,
  Expression* startingExpression,
  std::stringstream& out,
  JSData* outputJS
) const {
  std::stringstream outTrue;
  std::string input, output;
  bool isFinal = true;
  MemorySaving<FormatExpressions> tempFormat;
  if (theFormat == nullptr) {
    theFormat = &tempFormat.getElement();
    theFormat->flagExpressionIsFinal = true;
  }
  isFinal = theFormat->flagExpressionIsFinal;
  outTrue << "<table class =\"tableCalculatorOutput\">";
  outTrue << "<tr><th>Input</th><th>Result</th></tr>";
  if (this->isListStartingWithAtom(this->owner->opEndStatement())) {
    outTrue << out.str();
  } else {
    if (theFormat != nullptr) {
      theFormat->flagDontCollalpseProductsByUnits = true;
    }
    input = HtmlRoutines::GetMathSpanPure(startingExpression->toString(theFormat), 1700);
    if (theFormat != nullptr) {
      theFormat->flagDontCollalpseProductsByUnits = false;
    }
    outTrue << "<tr><td class =\"cellCalculatorInput\">" << input << "</td>";
    if ((
        this->isOfType<std::string>() ||
        this->isOfType<Plot>() ||
        this->isOfType<SemisimpleSubalgebras>() ||
        this->isOfType<WeylGroupData>()
      ) && isFinal
    ) {
      output = out.str();
    } else {
      output = HtmlRoutines::GetMathSpanPure(out.str(), 1700);
    }
    outTrue << "<td class =\"cellCalculatorResult\">" << output << "</td></tr>";
    if (outputJS != nullptr) {
      (*outputJS)["input"] = input;
      (*outputJS)["output"] = output;
    }
  }
  outTrue << "</table>";
  return outTrue.str();
}

bool Expression::toStringMinus3(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opMinus(), 3)) {
    return false;
  }
  if (input.children.size != 3) {
    global.fatal << "This is a programming error: "
    << "the minus function expects 1 or 2 arguments, "
    << "instead there are " << input.children.size - 1
    << ". " << global.fatal;
  }
  out << input[1].toString(theFormat) << "-";
  if (input[2].startsWith(input.owner->opPlus()) || input[2].startsWith(input.owner->opMinus())) {
    out << "\\left(" << input[2].toString(theFormat) << "\\right)";
  } else {
    out << input[2].toString(theFormat);
  }
  return true;
}

bool Expression::toStringMinus(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opMinus())) {
    return false;
  }
  if (input.size() == 2) {
    return Expression::toStringMinus2(input, out, theFormat);
  }
  return Expression::toStringMinus3(input, out, theFormat);
}

bool Expression::toStringMinus2(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opMinus(), 2)) {
    return false;
  }
  if (
    input[1].startsWith(input.owner->opPlus()) ||
    input[1].startsWith(input.owner->opMinus())
  ) {
    out << "-\\left(" << input[1].toString(theFormat) << "\\right)";
  } else {
    out << "-" << input[1].toString(theFormat);
  }
  return true;
}

bool Expression::toStringOr(
  const Expression &input, std::stringstream& out, FormatExpressions* theFormat
) {
  (void) theFormat;
  if (!input.startsWith(input.owner->opOr(), 3)) {
    return false;
  }
  if (!input[1].isSequenceNElements()) {
    out << "\\left(" << input[1] << "\\right)";
  } else {
    out << input[1];
  }
  out << "\\ or\\ ";
  if (!input[2].isSequenceNElements()) {
    out << "\\left(" << input[2] << "\\right)";
  } else {
    out << input[2];
  }
  return true;
}

bool Expression::toStringIntervalOpen(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opIntervalOpen(), 3)) {
    return false;
  }
  if (!input.owner->flagUseBracketsForIntervals) {
    out << "IntervalOpen{}";
  }
  out << "\\left(" << input[1].toString(theFormat) << ", "
  << input[2].toString(theFormat) << "\\right)";
  return true;
}

bool Expression::toStringIntervalLeftClosed(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (input.startsWith(input.owner->opIntervalLeftClosed(), 2)) {
    if (input[1].isSequenceNElements(2)) {
      out << "\\left[" << input[1][1] << ", " << input[1][2] << "\\right)";
    } else {
      out << input[0].toString(theFormat) << "{}" << input[1].toString(theFormat);
    }
    return true;
  }
  if (input.startsWith(input.owner->opIntervalLeftClosed(), 3)) {
    out << "\\left[" << input[1].toString(theFormat) << ", "
    << input[2].toString(theFormat) << "\\right)";
    return true;
  }
  return false;
}

bool Expression::toStringIntervalRightClosed(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (
    input.startsWith(input.owner->opIntervalRightClosed(), 3)
  ) {
    out << "\\left(" << input[1].toString(theFormat) << ", "
    << input[2].toString(theFormat) << "\\right]";
    return true;
  }
  if (input.startsWith(input.owner->opIntervalRightClosed(), 2)) {
    if (input[1].isSequenceNElements(2)) {
      out << "\\left(" << input[1][1] << ", " << input[1][2] << "\\right]";
    } else {
      out << input[0].toString(theFormat) << "{}" << input[1].toString(theFormat);
    }
    return true;
  }
  return false;
}

bool Expression::toStringIntervalClosed(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (input.startsWith(input.owner->opIntervalClosed(), 2)) {
    if (input[1].isSequenceNElements(2)) {
      out << "\\left[" << input[1][1] << ", " << input[1][2] << "\\right]";
    } else {
      out << input[0].toString(theFormat) << "{}" << input[1].toString(theFormat);
    }
    return true;
  }
  if (input.startsWith(input.owner->opIntervalClosed(), 3)) {
    out << "\\left[" << input[1].toString(theFormat) << ", "
    << input[2].toString(theFormat) << "\\right]";
    return true;
  }
  return false;
}

bool Expression::toStringQuote(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  (void) theFormat;
  if (!input.startsWith(input.owner->opQuote(), 2)) {
    return false;
  }
  std::string theString;
  if (input[1].isOperation(&theString)) {
    out << "\"" << theString << "\"";
  } else {
    out << "(Corrupt string)";
  }
  return true;
}

bool Expression::toStringLogBase(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opLogBase(), 3)) {
    return false;
  }
  out << "\\log_{" << input[1].toString(theFormat) << "}"
  << "\\left(" << input[2].toString(theFormat) << "\\right)";
  return true;
}

bool Expression::toStringIsDenotedBy(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.isListStartingWithAtom(input.owner->opIsDenotedBy())) {
    return false;
  }
  out << input[1].toString(theFormat) << "=:" << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringDefineConditional(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opDefineConditional(), 4)) {
    return false;
  }
  out << input[1].toString(theFormat) << " :if \\left("
  << input[2].toString(theFormat) << "\\right)="
  << input[3].toString(theFormat);
  return true;
}

bool Expression::toStringTensor(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opTensor(), 3)) {
    return false;
  }
  input.toStringOpMultiplicative(out, "\\otimes", theFormat);
  return true;
}

bool Expression::toStringIn(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opIn(), 3)) {
    return false;
  }
  out << input[1].toString(theFormat) << "\\in "
  << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringAnd(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  (void) theFormat;
  if (!input.startsWith(input.owner->opAnd(), 3)) {
    return false;
  }
  out << "\\left(" << input[1] << "\\right)" << " and "
  << "\\left(" << input[2] << "\\right)" ;
  return true;
}

bool Expression::toStringBinom(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opBinom(), 3)) {
    return false;
  }
  out << "\\binom{" << input[1].toString(theFormat) << "}{ "
  << input[2].toString(theFormat) << "}";
  return true;
}

bool Expression::toStringUnderscore(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opUnderscore())) {
    return false;
  }
  out << "{" << input[1].toString(theFormat) << "}_{"
  << input[2].toString(theFormat) << "}";
  return true;
}

bool Expression::toStringSetMinus(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opSetMinus(), 3)) {
    return false;
  }
  out << input[1].toString(theFormat) << "\\setminus "
  << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringLimitBoundary(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opLimitBoundary(), 3)) {
    return false;
  }
  out << "\\limits_{" << input[1].toString(theFormat)
  << "}^{"
  << input[2].toString(theFormat)
  << "}";
  return true;
}

bool Expression::toStringCrossProduct(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opCrossProduct())) {
    return false;
  }
  input.toStringOpMultiplicative(out, "\\times", theFormat);
  return true;
}

bool Expression::toStringAbsoluteValue(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opAbsoluteValue(), 2)) {
    return false;
  }
  out << "\\left|" << input[1].toString(theFormat) << "\\right|";
  return true;
}

bool Expression::toStringBind(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.startsWith(input.owner->opBind(), 2)) {
    return false;
  }
  out << "{{" << input[1].toString(theFormat) << "}}";
  return true;
}

bool Expression::toStringMod(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.isListStartingWithAtom(input.owner->opMod())) {
    return false;
  }
  out << input[1].toString(theFormat) << " mod " << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringLieBracket(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.isListStartingWithAtom(input.owner->opLieBracket())) {
    return false;
  }
  out << "[" << input[1].toString(theFormat) << "," << input[2].toString(theFormat) << "]";
  return true;
}

bool Expression::toStringEqualEqual(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.isListStartingWithAtom(input.owner->opEqualEqual())) {
    return false;
  }
  out << input[1].toString(theFormat) << "==" << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringEqualEqualEqual(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.isListStartingWithAtom(input.owner->opEqualEqualEqual())) {
    return false;
  }
  out << input[1].toString(theFormat) << "===" << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringWithAtomHandler(
  std::stringstream& out, FormatExpressions* theFormat
) const {
  MacroRegisterFunctionWithName("Expression::toStringWithAtomHandler");
  if (!this->isList()) {
    return false;
  }
  if (!(*this)[0].isAtom()) {
    return false;
  }
  int atom = (*this)[0].theData;
  if (this->owner->toStringHandlersAtoms.contains(atom)) {
    Expression::ToStringHandler handler =
    this->owner->toStringHandlersAtoms.getValueNoFail(atom);
    return handler(*this, out, theFormat);
  }
  return false;
}

bool Expression::toStringWithCompositeHandler(
  std::stringstream& out, FormatExpressions* theFormat
) const {
  MacroRegisterFunctionWithName("Expression::toStringWithCompositeHandler");
  if (!this->isList()) {
    return false;
  }
  if (!(*this)[0].isList()) {
    return false;
  }
  if (!(*this)[0][0].isAtom()) {
    return false;
  }
  int atom = (*this)[0][0].theData;
  if (this->owner->toStringHandlersComposite.contains(atom)) {
    Expression::ToStringHandler handler =
    this->owner->toStringHandlersComposite.getValueNoFail(atom);
    return handler(*this, out, theFormat);
  }
  return false;
}

std::string Expression::toString(
  FormatExpressions* theFormat,
  Expression* startingExpression,
  bool unfoldCommandEnclosures,
  JSData* outputJS
) const {
  MacroRegisterFunctionWithName("Expression::toString");
  MemorySaving<FormatExpressions> tempFormat;
  if (theFormat == nullptr) {
    theFormat = &tempFormat.getElement();
    theFormat->flagUseQuotes = false;
  }
  if (this->owner != nullptr) {
    if (this->owner->RecursionDeptH + 1 > this->owner->MaxRecursionDeptH) {
      return "(...)";
    }
  } else {
    return "(Error:NoOwner)";
  }
  RecursionDepthCounter theRecursionCounter(&this->owner->RecursionDeptH);
  this->checkConsistency();
  if (startingExpression != nullptr && unfoldCommandEnclosures) {
    Expression newStart, newMe;
    if (
      this->owner->functionFlattenCommandEnclosuresOneLayer(*this->owner, *this, newMe) &&
      this->owner->functionFlattenCommandEnclosuresOneLayer(*this->owner, *startingExpression, newStart)
    ) {
      return newMe.toString(theFormat, &newStart, false, outputJS);
    }
  }
  int notationIndex = owner->theObjectContainer.ExpressionWithNotation.getIndex(*this);
  if (notationIndex != - 1) {
    return owner->theObjectContainer.ExpressionNotation[notationIndex];
  }
  std::stringstream out;
  if (
    !this->isOfType<std::string>() &&
    !this->startsWith(this->owner->opEndStatement())
  ) {
    if (startingExpression == nullptr) {
      theFormat->flagUseQuotes = true;
    } else {
      theFormat->flagUseQuotes = false;
    }
  }
  if (outputJS != nullptr) {
    outputJS->reset();
  }
  if (this->toStringData(out, theFormat)) {
  } else if (this->toStringWithAtomHandler(out, theFormat)) {
  } else if (this->toStringWithCompositeHandler(out, theFormat)) {
  } else if (this->toStringEndStatement(out, startingExpression, outputJS, theFormat)) {
  } else if (this->size() == 1) {
    out << (*this)[0].toString(theFormat);
  } else if (this->toStringGeneral(out, theFormat)) {
  } else { //<-not sure if this case is possible
    out << "(ProgrammingError:NotDocumented)";
  }
  if (startingExpression != nullptr) {
    return this->toStringWithStartingExpression(theFormat, startingExpression, out, outputJS);
  }
  if (outputJS != nullptr) {
    if (outputJS->theType == JSData::token::tokenUndefined) {
      (*outputJS) = out.str();
    }
  }
  return out.str();
}

std::string Expression::lispify() const {
  if (this->owner == nullptr) {
    return "Error: not initialized";
  }
  RecursionDepthCounter theCounter(&this->owner->RecursionDeptH);
  if (this->owner->RecursionDeptH > this->owner->MaxRecursionDeptH) {
    return "(error: max recursion depth ...)";
  }
  std::stringstream dataStream;
  if (this->toStringData(dataStream)) {
    return dataStream.str();
  }
  if (this->children.size == 0) {
    return this->toString();
  }
  std::stringstream out;
  out << "(";
  for (int i = 0; i < this->children.size; i ++) {
    out << " " << (*this)[i].lispify();
  }
  out << ")";
  return out.str();
}

bool Expression::isListStartingWithAtom(int theOp) const {
  if (!this->isList()) {
    return false;
  }
  if (!(*this)[0].isAtom()) {
    return false;
  }
  if (theOp == - 1) {
    return true;
  }
  return (*this)[0].theData == theOp;
}

bool Expression::isListNElements(int N) const {
  if (!this->isList()) {
    return false;
  }
  if (N == - 1) {
    return true;
  }
  return this->children.size == N;
}

bool Expression::isList() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->children.size <= 0) {
    return false;
  }
  if (this->theData != this->owner->opLisT()) {
    global.fatal << "This is a programming error. List expressions "
    << "must have data valule equal to Calculator::opList(). " << global.fatal;
  }
  return true;
}

bool Expression::isListOfTwoAtomsStartingWith(int theOp) const {
  if (!this->isListStartingWithAtom(theOp)) {
    return false;
  }
  if (this->children.size != 2) {
    return false;
  }
  return (*this)[1].isAtom();
}

bool Expression::isFrozen() const {
  if (!this->startsWith()) {
    return false;
  }
  return (*this)[0].isAtomThatFreezesArguments();
}

bool Expression::isAtomThatFreezesArguments(std::string* outputWhichAtom) const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->isList()) {
    return false;
  }
  if (this->theData < 0 || this->theData >= this->owner->GetOperations().size) {
    return false;
  }
  if (outputWhichAtom != nullptr) {
    *outputWhichAtom = this->owner->GetOperations()[this->theData];
  }
  return this->owner->atomsThatFreezeArguments.contains(this->owner->GetOperations()[this->theData]);
}

bool Expression::isPowerOfAtomWhoseExponentsAreInterpretedAsFunction() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (!this->startsWith(this->owner->opThePower(), 3)) {
    return false;
  }
  return (*this)[1].isAtomWhoseExponentsAreInterpretedAsFunction();
}

bool Expression::isAtomWhoseExponentsAreInterpretedAsFunction(std::string* outputWhichAtom) const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->isList()) {
    return false;
  }
  if (this->theData < 0 || this->theData >= this->owner->GetOperations().size) {
    return false;
  }
  if (outputWhichAtom != nullptr) {
    *outputWhichAtom = this->owner->GetOperations()[this->theData];
  }
  return this->owner->atomsWhoseExponentsAreInterpretedAsFunctions.contains(this->owner->GetOperations()[this->theData]);
}

bool Expression::isAtomNotInterpretedAsFunction(std::string* outputWhichAtom) const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->isList()) {
    return false;
  }
  if (this->theData < 0 || this->theData>= this->owner->GetOperations().size) {
    return false;
  }
  if (outputWhichAtom != nullptr) {
    *outputWhichAtom = this->owner->GetOperations()[this->theData];
  }
  return this->owner->atomsNotInterpretedAsFunctions.contains(this->owner->GetOperations()[this->theData]);
}

bool Expression::isAtom() const {
  if (this->owner == nullptr) {
    return false;
  }
  return !this->isList();
}

bool Expression::isOperation(std::string* outputWhichOperation) const {
  int index = - 1;
  if (!this->isOperation(index)) {
    return false;
  }
  if (outputWhichOperation != nullptr) {
    *outputWhichOperation = this->owner->operations.theKeys[index];
  }
  return true;
}

bool Expression::isOperation(int& outputWhichOperationIndex) const {
  if (!this->isAtom()) {
    return false;
  }
  if (this->theData < 0 || this->theData >= this->owner->operations.size()) {
    return false;
  }
  outputWhichOperationIndex = this->theData;
  return true;
}

bool Expression::isOperationGiven(const std::string& desiredAtom) const {
  if (this->isList()) {
    return false;
  }
  if (this->owner == nullptr) {
    return false;
  }
  return this->theData == this->owner->operations.getIndex(desiredAtom);
}

bool Expression::isOperationGiven(int desiredDataUseMinusOneForAny) const {
  if (this->isList()) {
    return false;
  }
  if (desiredDataUseMinusOneForAny == - 1) {
    return true;
  }
  return this->theData == desiredDataUseMinusOneForAny;
}

bool Expression::isKnownFunctionWithComplexRange(std::string* outputWhichOperation) const {
  if (this->owner == nullptr) {
    return false;
  }
  std::string operationName;
  if (outputWhichOperation == nullptr) {
    outputWhichOperation = &operationName;
  }
  if (!this->isBuiltInAtom(outputWhichOperation)) {
    return false;
  }
  return this->owner->knownFunctionsWithComplexRange.contains(operationName);
}

bool Expression::isCalculatorStatusChanger() const {
  if (this->owner == nullptr) {
    return false;
  }
  return
  this->startsWith(this->owner->opDefine()) ||
  this->startsWith(this->owner->opDefineConditional()) ||
  this->startsWith(this->owner->opRulesOff()) ||
  this->startsWith(this->owner->opRulesOn());
}

bool Expression::isArithmeticOperation(std::string* outputWhichOperation) const {
  if (this->owner == nullptr) {
    return false;
  }
  std::string operationName;
  if (outputWhichOperation == nullptr) {
    outputWhichOperation = &operationName;
  }
  if (!this->isBuiltInAtom(outputWhichOperation)) {
    return false;
  }
  return this->owner->arithmeticOperations.contains(operationName);
}

bool Expression::isBuiltInAtom(std::string* outputWhichOperation) const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->isList()) {
    return false;
  }
  if (this->theData < 0 || this->theData >= this->owner->GetOperations().size) {
    return false;
  }
  if (this->theData >= this->owner->NumPredefinedAtoms) {
    return false;
  }
  if (outputWhichOperation != nullptr) {
    *outputWhichOperation = this->owner->GetOperations()[this->theData];
  }
  return true;
}

bool Expression::isGoodForChainRuleFunction(std::string* outputWhichOperation) const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->isList()) {
    return false;
  }
  if (this->theData < 0 || this->theData >= this->owner->GetOperations().size) {
    return false;
  }
  if (outputWhichOperation != nullptr) {
    *outputWhichOperation = this->owner->GetOperations()[this->theData];
  }
  return !this->owner->atomsNotAllowingChainRule.contains(this->owner->GetOperations()[this->theData]);
}

bool Expression::hasContext() const {
  this->checkInitialization();
  if (!this->isBuiltInType() || !(this->size() == 3)) {
    return false;
  }
  return (*this)[1].isListStartingWithAtom(this->owner->opContext());
}

bool Expression::hasNonEmptyContext() const {
  if (!this->hasContext()) {
    return false;
  }
  return !this->getContext().isEmpty();
}

bool Expression::isCacheableExpression() const {
  MacroRegisterFunctionWithName("Expression::isCacheableExpression");
  if (this->owner == nullptr) {
    return true;
  }
  if (this->isBuiltInType()) {
    return true;
  }
  for (int i = 0; i < this->children.size; i ++) {
    if (!(*this)[i].isCacheableExpression()) {
      return false;
    }
  }
  if (this->children.size > 0) {
    return true;
  }
  Calculator& theBoss = *this->owner;
  if (this->theData < 0 || this->theData >= theBoss.operations.size()) {
    global.fatal << "Corrupted atom in Expression::isCacheableExpression. " << global.fatal;
  }
  return theBoss.atomsThatMustNotBeCached.contains(theBoss.operations.theKeys[this->theData]);
}

bool Expression::isBuiltInScalar() const {
  return this->isOfType<Rational>() || this->isOfType<Polynomial<Rational> >() ||
  this->isOfType<RationalFunction>() || this->isOfType<double>() || this->isOfType<AlgebraicNumber>();
}

bool Expression::isElementaryObject() const {
  if (this->isAtom()) {
    return true;
  }
  if (this->isBuiltInType()) {
    return true;
  }
  if (this->owner == nullptr) {
    return true;
  }
  if (this->startsWith(this->owner->opBind())) {
    return true;
  }
  return false;
}

bool Expression::isBuiltInType(std::string* outputWhichOperation) const {
  std::string tempS;
  if (!this->startsWith()) {
    return false;
  }
  if (!(*this)[0].isOperation(&tempS)) {
    return false;
  }
  if (this->owner->getBuiltInTypes().contains(tempS)) {
    if (outputWhichOperation != nullptr) {
      *outputWhichOperation = tempS;
    }
    return true;
  }
  return false;
}

bool Expression::isBuiltInType(int* outputWhichType) const {
  std::string theType;
  if (!this->isBuiltInType(&theType)) {
    return false;
  }
  if (outputWhichType != nullptr) {
    *outputWhichType = this->owner->GetOperations().getIndex(theType);
  }
  return true;
}

bool Expression::makeProduct(Calculator& owner, const Expression& left, const Expression& right) {
  return this->MakeXOX(owner, owner.opTimes(), left, right);
}

bool Expression::makeProduct(Calculator& owner, const List<Expression>& theMultiplicands) {
  if (theMultiplicands.size == 0) {
    return this->assignValue(1, owner);
  }
  return this->MakeXOXOdotsOX(owner, owner.opTimes(), theMultiplicands);
}

bool Expression::makeSum(Calculator& owner, const List<Expression>& theSummands) {
  MacroRegisterFunctionWithName("Expression::MakeSum");
  if (theSummands.size == 0) {
    return this->assignValue(0, owner);
  }
  return this->MakeXOXOdotsOX(owner, owner.opPlus(), theSummands);
}

bool Expression::MakeOXdotsX(Calculator& owner, int theOp, const List<Expression>& theOpands) {
  MacroRegisterFunctionWithName("Expression::MakeOXdotsX");
  if (theOpands.size == 0) {
    global.fatal << "zero opands not allowed at this point. " << global.fatal;
  }
  if (theOpands.size == 1) {
    *this = theOpands[0];
    return true;
  }
  *this = *theOpands.lastObject();
  for (int i = theOpands.size - 2; i >= 0; i --) {
    this->MakeXOX(owner, theOp, theOpands[i], *this);
  }
  this->CheckConsistencyRecursively();
  return true;
}

void ExpressionContext::makeOneVariableCreate(
  const std::string& variable
) {
  this->initialize(*this->owner);
  Expression variableWrapper;
  variableWrapper.makeAtom(variable, *this->owner);
  this->variables.addOnTop(variableWrapper);
}

void ExpressionContext::makeOneVariableOneDifferentialOperator(
  const Expression& inputVariable,
  const Expression& inputDifferentialOperatorVariable
) {
  this->initialize(*this->owner);
  this->variables.addOnTop(inputVariable);
  this->differentialOperatorVariables.addOnTop(inputDifferentialOperatorVariable);
}

void ExpressionContext::makeOneVariable(
  const Expression& polynomialVariable
) {
  this->initialize(*this->owner);
  this->variables.addOnTop(polynomialVariable);
}

void ExpressionContext::makeOneVariableFromString(
  const std::string& polynomialVariable
) {
  this->initialize(*this->owner);
  Expression converted;
  converted.makeAtom(polynomialVariable, *this->owner);
  this->variables.addOnTop(converted);
}

bool Expression::makeSqrt(Calculator& owner, const Rational& argument, const Rational& radicalSuperIndex) {
  MacroRegisterFunctionWithName("Expression::makeSqrt");
  Expression argumentE;
  argumentE.assignValue(argument, owner);
  return this->makeSqrt(owner, argumentE, radicalSuperIndex);
}

bool Expression::makeSqrt(Calculator& owner, const Expression& argument, const Rational& radicalSuperIndex) {
  MacroRegisterFunctionWithName("Expression::makeSqrt");
  this->reset(owner, 3);
  Expression radicalIndexE;
  radicalIndexE.assignValue(radicalSuperIndex, owner);
  this->addChildAtomOnTop(owner.opSqrt());
  this->addChildOnTop(radicalIndexE);
  return this->addChildOnTop(argument);
}

bool Expression::MakeXOX(Calculator& owner, int theOp, const Expression& left, const Expression& right) {
  MacroRegisterFunctionWithName("Expression::MakeXOX");
  if (&left == this || &right == this) {
    Expression leftCopy = left;
    Expression rightCopy = right;
    return this->MakeXOX(owner, theOp, leftCopy, rightCopy);
  }
  if (right.owner == nullptr && left.owner == nullptr) {
    global.fatal << "Cannot build an expression from two non-initialized expressions. " << global.fatal;
  }
  if (right.owner == nullptr) {
    Expression rightCopy;
    rightCopy.assignValue(right.theData, *left.owner);
    return this->MakeXOX(owner, theOp, left, rightCopy);
  }
 if (left.owner == nullptr) {
    Expression leftCopy;
    leftCopy.assignValue(left.theData, *right.owner);
    return this->MakeXOX(owner, theOp, leftCopy, right);
  }
  left.checkInitialization();
  right.checkInitialization();
  this->reset(owner, 3);
  this->theData = owner.opLisT();
  this->addChildAtomOnTop(theOp);
  this->addChildOnTop(left);
  return this->addChildOnTop(right);
}

bool Expression::MakeOX(Calculator& owner, int theOp, const Expression& opArgument) {
  if (&opArgument == this) {
    Expression copyE = opArgument;
    return this->MakeOX(owner, theOp, copyE);
  }
  this->reset(owner);
  this->theData = owner.opLisT();
  this->addChildAtomOnTop(theOp);
  return this->addChildOnTop(opArgument);
}

bool Expression::sequencefy() {
  this->checkInitialization();
  if (this->isSequenceNElements()) {
    return true;
  }
  return this->MakeOX(*this->owner, this->owner->opSequence(), *this);
}

bool Expression::operator==(int other) const {
  if (this->owner == nullptr) {
    return false;
  }
  int theInt;
  if (!this->isSmallInteger(&theInt)) {
    return false;
  }
  return theInt == other;
}

bool Expression::operator==(const std::string& other) const {
  return this->isOperationGiven(other);
}

std::string Expression::toUTF8String(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("Expression::toUTF8String");
  if (this->owner == nullptr) {
    return this->toString(theFormat);
  }
  std::stringstream out;
  Rational theRat;
  if (this->isOfType<Rational>(&theRat)) {
    FormatExpressions tempFormat;
    tempFormat.flagUseFrac = false;
    return theRat.toString(&tempFormat);
  } else if (this->isOperationGiven(this->owner->opPi())) {
    return "\\u03C0";
  } else if (this->startsWith(this->owner->opPlus(), 3)) {
    return (*this)[1].toUTF8String(theFormat) + "+" + (*this)[2].toUTF8String(theFormat);
  } else if (this->startsWith(this->owner->opTimes(), 3)) {
    std::string secondUTF8String = (*this)[2].toUTF8String(theFormat);
    std::string secondString = (*this)[2].toString(theFormat);
    if ((*this)[1].isOperationGiven(this->owner->opSqrt())) {
      out << "sqrt(" << secondUTF8String << ")";
    } else {
      std::string firstE = (*this)[1].toUTF8String(theFormat);
      bool firstNeedsBrackets = (*this)[1].needsParenthesisForMultiplication();
      bool secondNeedsBrackets = (*this)[2].needsParenthesisForMultiplicationWhenSittingOnTheRightMost(&((*this)[1]));
      bool mustHaveTimes = false;
      if (firstE == "- 1" || firstE == "-1") {
        firstE = "-";
        firstNeedsBrackets = false;
      }
      if (firstE == "1") {
        firstE = "";
      }
      if (firstNeedsBrackets) {
        out << "(" << firstE << ")";
      } else {
        out << firstE;
      }
      if (!firstNeedsBrackets && !secondNeedsBrackets && firstE != "" && firstE != "-") {
        if (MathRoutines::isADigit(firstE[firstE.size() - 1]) ) {
          if (MathRoutines::isADigit(secondUTF8String[0])) {
            mustHaveTimes = true;
          }
          if (StringRoutines::StringBeginsWith(secondString, "\\frac")) {
            mustHaveTimes = true;
          }
        }
      }
      if (mustHaveTimes) {
        out << "* ";
      } else {
        out << " ";
      }
      if (secondNeedsBrackets) {
        out << "(" << secondUTF8String << ")";
      } else {
        out << secondUTF8String;
      }
    }
    return out.str();
  } else if (this->startsWith(this->owner->opSequence())) {
    out << "(";
    for (int i = 1; i < this->size(); i ++) {
      out << (*this)[i].toUTF8String(theFormat);
      if (i < this->size() - 1) {
        out << ", ";
      }
    }
    out << ")";
    return out.str();
  }
  return this->toString(theFormat);
}
