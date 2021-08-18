// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "math_extra_weyl_algebras_implementation.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_extra_modules_semisimple_lie_algebras_implementation.h"
#include "math_extra_semisimple_lie_algebras_implementation.h"
#include "math_extra_finite_groups_implementation.h"
#include "math_extra_symmetric_groups_and_generalizations.h"
#include "calculator_html_functions.h"
#include "crypto.h"
#include "math_extra_elliptic_curves_implementation.h"
#include <cmath>
#include "math_rational_function_implementation.h"
#include "string_constants.h"

Expression operator*(const Expression& left, const Expression& right) {
  MacroRegisterFunctionWithName("operator*(Expression, Expression)");
  left.checkInitialization();
  Expression result;
  result.makeXOX(*left.owner, left.owner->opTimes(), left, right);
  return result;
}

Expression operator/(const Expression& left, const Expression& right) {
  left.checkInitialization();
  Expression result;
  result.makeXOX(*left.owner, left.owner->opDivide(), left, right);
  return result;
}

Expression operator+(const Expression& left, const Expression& right) {
  left.checkInitialization();
  Expression result;
  result.makeXOX(*left.owner, left.owner->opPlus(), left, right);
  return result;
}

Expression operator-(const Expression& left, const Expression& right) {
  left.checkInitialization();
  Expression result;
  result.makeXOX(*left.owner, left.owner->opMinus(), left, right);
  return result;
}

//If you get a specialization after instantiation error:
//the following template specialization funcitons must appear
//here and nowhere else (discovered through extremely painful experimentation).

template < >
List<Expression>::Comparator*
FormatExpressions::getMonomialOrder<Expression>() {
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
int Expression::getTypeOperation<RationalFraction<Rational> >() const {
  this->checkInitialization();
  return this->owner->opRationalFraction();
}

template < >
int Expression::getTypeOperation<RationalFraction<AlgebraicNumber> >() const {
  this->checkInitialization();
  return this->owner->opRationalFunctionAlgebraicCoefficients();
}

template < >
int Expression::getTypeOperation<RationalFraction<ElementZmodP> >() const {
  this->checkInitialization();
  return this->owner->opRationalFunctionModuloInteger();
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
int Expression::getTypeOperation<JSData>() const {
  this->checkInitialization();
  return this->owner->opJSON();
}

template < >
int Expression::getTypeOperation<std::string>() const {
  this->checkInitialization();
  return this->owner->opString();
}

template < >
int Expression::getTypeOperation<AlgebraicNumber>() const {
  this->checkInitialization();
  return this->owner->opAlgebraicNumber();
}

template < >
int Expression::getTypeOperation<ElementZmodP>() const {
  this->checkInitialization();
  return this->owner->opEltZmodP();
}

template < >
int Expression::getTypeOperation<ElementUniversalEnveloping<RationalFraction<Rational> > >() const {
  this->checkInitialization();
  return this->owner->opElementUEOverRF();
}

template < >
int Expression::getTypeOperation<Weight<Rational> >() const {
  this->checkInitialization();
  return this->owner->opWeightLieAlgebra();
}

template < >
int Expression::getTypeOperation<Weight<Polynomial<Rational> > >() const {
  this->checkInitialization();
  return this->owner->opWeightLieAlgPoly();
}

template < >
int Expression::getTypeOperation<MatrixTensor<Rational> >() const {
  this->checkInitialization();
  return this->owner->opMatrixTensorRational();
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
  return this->owner->opSemisimpleLieAlgebra();
}

template < >
int Expression::getTypeOperation<LittelmannPath>() const {
  this->checkInitialization();
  return this->owner->opLittelmannPath();
}

template < >
int Expression::getTypeOperation<MonomialTensor<int, HashFunctions::hashFunction> >() const {
  this->checkInitialization();
  return this->owner->opLRO();
}

template < >
int Expression::getTypeOperation<ElementTensorsGeneralizedVermas<RationalFraction<Rational> > >() const {
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
  ElementUniversalEnveloping<RationalFraction<Rational> > element;
  if (!this->isOfType<ElementUniversalEnveloping<RationalFraction<Rational> > >(&element)) {
    return false;
  }
  ElementSemisimpleLieAlgebra<Rational> elementSemisimple;
  if (!element.isLieAlgebraElementRational(elementSemisimple)) {
    return false;
  }
  if (whichElement != nullptr) {
    this->getContext(whichElement->context);
    whichElement->content = elementSemisimple;
  }
  return true;
}

// Expression::ConvertsToType specializations end.
// Expression::addObjectReturnIndex specializations follow

template < >
int Expression::addObjectReturnIndex(const
ElementEllipticCurve<Rational>
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.ellipticCurveElementsRational
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
ElementEllipticCurve<ElementZmodP>
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.ellipticCurveElementsZmodP
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
SemisimpleSubalgebras
& inputValue) const {
  this->checkInitialization();
  inputValue.checkInitialization();
  if (!this->owner->objectContainer.semisimpleSubalgebras.contains(inputValue.owner->weylGroup.dynkinType)) {
    global.fatal << "Semisimple subalgebras must be "
    << "allocated directly in the object container. " << global.fatal;
  }
  return this->owner->objectContainer.semisimpleSubalgebras.getIndex(inputValue.owner->weylGroup.dynkinType);
}

template < >
int Expression::addObjectReturnIndex(const
ElementHyperoctahedralGroupR2
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.elementsHyperOctGroup
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
InputBox
& inputValue) const {
  this->checkInitialization();
  this->owner->objectContainer.userInputTextBoxesWithValues
  .setKeyValue(inputValue.name, inputValue);
  return this->owner->objectContainer.userInputTextBoxesWithValues
  .getIndex(inputValue.name);
}

template < >
int Expression::addObjectReturnIndex(const
std::string
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.allStrings
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
JSData
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.jsonObjects
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational>
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.hyperoctahedralRepresentations
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
Rational
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.allRationals
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
double
& inputValue) const {
  this->checkInitialization();
  if (FloatingPoint::isNaN(inputValue)) {
    return 0;
  }
  return this->owner->objectContainer.doubleNumbers
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
RationalFraction<Rational>
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.rationalFunctions
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
RationalFraction<AlgebraicNumber>
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.rationalFunctionsAlgebraic
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
RationalFraction<ElementZmodP>
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.rationalFunctionsModular
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
MatrixTensor<Rational>
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.matrixTensorRationals
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
ElementUniversalEnveloping<RationalFraction<Rational> >
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.universalEnvelopingAlgebraElements
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
Weight<Rational>
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.weights
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
Weight<Polynomial<Rational> >
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.weightsPolynomial
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
int
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.allRationals
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
LargeIntegerUnsigned
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.allRationals
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
Polynomial<Rational>
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.polynomialsRational
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
Polynomial<ElementZmodP>
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.polynomialsModular
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
PolynomialModuloPolynomial<ElementZmodP>
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.polynomialQuotientsModular
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
Polynomial<AlgebraicNumber>
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.polynomialsAlgebraic
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
ElementWeylAlgebra<Rational>
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.weylAlgebraElements
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(
SemisimpleLieAlgebra* const
& inputValue) const {
  this->checkInitialization();
  if (!this->owner->objectContainer.semisimpleLieAlgebras.contains(inputValue->weylGroup.dynkinType)) {
    global.fatal << "Semisimple Lie algebra must be allocated directly in the object container. " << global.fatal;
  }
  int index = this->owner->objectContainer.semisimpleLieAlgebras.getIndex(inputValue->weylGroup.dynkinType);
  return index;
}

template < >
int Expression::addObjectReturnIndex(const
AlgebraicNumber
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.allAlgebraicNumbers
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
ElementZmodP
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.elementsModP
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
MonomialTensor<int, HashFunctions::hashFunction>
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.littelmannOperators
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
ElementTensorsGeneralizedVermas<RationalFraction<Rational> >
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.tensorElements
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
CharacterSemisimpleLieAlgebraModule<Rational>
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.charactersSemisimpleLieAlgebraFiniteDimensional
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
LittelmannPath
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.lakshmibaiSeshadriPaths
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
Plot
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.allPlots
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
PlotObject
& inputValue) const {
  this->checkInitialization();
  Plot plotContainer;
  plotContainer += inputValue;
  return this->owner->objectContainer.allPlots
  .addNoRepetitionOrReturnIndexFirst(plotContainer);
}

template < >
int Expression::addObjectReturnIndex(const
WeylGroupData
& inputValue) const {
  this->checkInitialization();
  this->owner->objectContainer.getLieAlgebraCreateIfNotPresent(inputValue.dynkinType);
  return this->owner->objectContainer.semisimpleLieAlgebras.getIndex(inputValue.dynkinType);
}

template < >
int Expression::addObjectReturnIndex(const
GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.weylGroupRepresentations
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.weylGroupVirtualRepresentations
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::addObjectReturnIndex(const
ElementWeylGroup
& inputValue) const {
  this->checkInitialization();
  return this->owner->objectContainer.weylGroupElements
  .addNoRepetitionOrReturnIndexFirst(inputValue);
}

//Expression::addObjectReturnIndex specializations end

//start Expression::getValueNonConst specializations.
template < >
Rational& Expression::getValueNonConst() const {
  if (!this->isOfType<Rational>()) {
    global.fatal << "Expression not of required type Rational. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.allRationals.getElement(this->getLastChild().data);
}

template < >
ElementEllipticCurve<Rational>& Expression::getValueNonConst() const {
  if (!this->isOfType<ElementEllipticCurve<Rational> >()) {
    global.fatal << "Expression not of required type Rational. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.ellipticCurveElementsRational.getElement(this->getLastChild().data);
}

template < >
Polynomial<ElementZmodP>& Expression::getValueNonConst() const {
  if (!this->isOfType<Polynomial<ElementZmodP> >()) {
    global.fatal << "Expression not of required type Polynomial mod integer. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.polynomialsModular.getElement(this->getLastChild().data);
}

template < >
PolynomialModuloPolynomial<ElementZmodP>& Expression::getValueNonConst() const {
  if (!this->isOfType<PolynomialModuloPolynomial<ElementZmodP> >()) {
    global.fatal
    << "Expression not of required type Polynomial mod Polynomial mod integer. "
    << "The expression equals "
    << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.polynomialQuotientsModular.getElement(this->getLastChild().data);
}

template < >
ElementEllipticCurve<ElementZmodP>& Expression::getValueNonConst() const {
  if (!this->isOfType<ElementEllipticCurve<ElementZmodP> >()) {
    global.fatal << "Expression not of required type Rational. The expression equals "
    << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.ellipticCurveElementsZmodP.getElement(this->getLastChild().data);
}

template < >
ElementHyperoctahedralGroupR2& Expression::getValueNonConst() const {
  if (!this->isOfType<ElementHyperoctahedralGroupR2>()) {
    global.fatal << "Expression not of required type Rational. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.elementsHyperOctGroup.getElement(this->getLastChild().data);
}

template < >
InputBox& Expression::getValueNonConst() const {
  if (!this->isOfType<InputBox>()) {
    global.fatal << "Expression not of required type Rational. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.userInputTextBoxesWithValues.values[this->getLastChild().data];
}

template < >
GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational>& Expression::getValueNonConst() const {
  if (!this->isOfType<GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> >()) {
    global.fatal << "Expression not of required type Rational. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.hyperoctahedralRepresentations[this->getLastChild().data];
}

template < >
ElementZmodP& Expression::getValueNonConst() const {
  if (!this->isOfType<ElementZmodP>())
    global.fatal << "Expression not of required type ElementZmodP. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  return this->owner->objectContainer.elementsModP.getElement(this->getLastChild().data);
}

template < >
AlgebraicNumber& Expression::getValueNonConst() const {
  if (!this->isOfType<AlgebraicNumber>()) {
    global.fatal << "Expression not of required type AlgebraicNumber. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.allAlgebraicNumbers.getElement(this->getLastChild().data);
}

template < >
double& Expression::getValueNonConst() const {
  if (!this->isOfType<double>()) {
    global.fatal << "Expression not of required type double. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.doubleNumbers.getElement(this->getLastChild().data);
}

template < >
std::string& Expression::getValueNonConst() const {
  if (!this->isOfType<std::string>()) {
    global.fatal << "Expression not of required type std::string. "
    << "The expression equals " << this->toString() << ". Comments so far: "
    << this->owner->comments.str() << global.fatal;
  }
  return this->owner->objectContainer.allStrings.getElement(this->getLastChild().data);
}

template < >
JSData& Expression::getValueNonConst() const {
  if (!this->isOfType<JSData>()) {
    global.fatal << "Expression not of required type std::string. "
    << "The expression equals " << this->toString() << ". Comments so far: "
    << this->owner->comments.str() << global.fatal;
  }
  return this->owner->objectContainer.jsonObjects.getElement(this->getLastChild().data);
}

template < >
RationalFraction<Rational>& Expression::getValueNonConst() const {
  if (!this->isOfType<RationalFraction<Rational> >()) {
    global.fatal << "Expression not of required type RationalFunction_Rational. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.rationalFunctions.getElement(this->getLastChild().data);
}

template < >
RationalFraction<AlgebraicNumber>& Expression::getValueNonConst() const {
  if (!this->isOfType<RationalFraction<AlgebraicNumber> >()) {
    global.fatal << "Expression not of required type RationalFunction_AlgebraicNumber. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.rationalFunctionsAlgebraic.getElement(this->getLastChild().data);
}

template < >
RationalFraction<ElementZmodP>& Expression::getValueNonConst() const {
  if (!this->isOfType<RationalFraction<ElementZmodP> >()) {
    global.fatal << "Expression not of required type RationalFunctionOld. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.rationalFunctionsModular.getElement(this->getLastChild().data);
}

template < >
ElementUniversalEnveloping<RationalFraction<Rational> >& Expression::getValueNonConst() const {
  if (!this->isOfType<ElementUniversalEnveloping<RationalFraction<Rational> > >()) {
    global.fatal << "Expression not of required type "
    << "ElementUniversalEnveloping_RationalFunctionOld. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.universalEnvelopingAlgebraElements.getElement(this->getLastChild().data);
}

template < >
Polynomial<Rational>& Expression::getValueNonConst() const {
  if (!this->isOfType<Polynomial<Rational> >()) {
    global.fatal << "Expression not of required type Polynomial_Rational. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.polynomialsRational.getElement(this->getLastChild().data);
}

template < >
Polynomial<AlgebraicNumber>& Expression::getValueNonConst() const {
  if (!this->isOfType<Polynomial<AlgebraicNumber> >()) {
    global.fatal << "Expression not of required type Polynomial_AlgebraicNumber. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.polynomialsAlgebraic.getElement(this->getLastChild().data);
}

template < >
ElementWeylAlgebra<Rational>& Expression::getValueNonConst() const {
  if (!this->isOfType<ElementWeylAlgebra<Rational> >()) {
    global.fatal << "Expression not of required type "
    << "ElementWeylAlgebra_Rational. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.weylAlgebraElements.getElement(this->getLastChild().data);
}

template < >
LittelmannPath& Expression::getValueNonConst() const {
  if (!this->isOfType<LittelmannPath>()) {
    global.fatal << "Expression not of required type "
    << "LittelmannPath. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.lakshmibaiSeshadriPaths.getElement(this->getLastChild().data);
}

template < >
Weight<Polynomial<Rational> >& Expression::getValueNonConst() const {
  if (!this->isOfType<Weight<Polynomial<Rational> > >()) {
    global.fatal << "Expression not of required type "
    << "LittelmannPath. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.weightsPolynomial.getElement(this->getLastChild().data);
}

template < >
MonomialTensor<int, HashFunctions::hashFunction>& Expression::getValueNonConst() const {
  if (!this->isOfType<MonomialTensor<int, HashFunctions::hashFunction> >()) {
    global.fatal << "Expression not of required type "
    << "MonomialTensor. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.littelmannOperators.getElement(this->getLastChild().data);
}

template < >
ElementTensorsGeneralizedVermas<RationalFraction<Rational> >& Expression::getValueNonConst() const {
  if (!this->isOfType<ElementTensorsGeneralizedVermas<RationalFraction<Rational> > >()) {
    global.fatal << "Expression not of "
    << "required type ElementTGVM_RationalFunctionOld. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.tensorElements.getElement(this->getLastChild().data);
}

template < >
CharacterSemisimpleLieAlgebraModule<Rational>& Expression::getValueNonConst() const {
  if (!this->isOfType<CharacterSemisimpleLieAlgebraModule<Rational> >()) {
    global.fatal << "Expression not of required type "
    << "charSSAlgMod_Rational. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.charactersSemisimpleLieAlgebraFiniteDimensional.getElement(this->getLastChild().data);
}

template < >
SemisimpleLieAlgebra*& Expression::getValueNonConst() const {
  MacroRegisterFunctionWithName("Expression::getValueNonConst(SemisimpleLieAlgebra*)");
  if (!this->isOfType<SemisimpleLieAlgebra*>()) {
    global.fatal << "Expression not of required type "
    << "SemisimpleLieAlgebra. The expression equals " << this->toString() << "." << global.fatal;
  }
  for (
    int i = this->owner->objectContainer.semisimpleLieAlgebraPointers.size;
    i < this->owner->objectContainer.semisimpleLieAlgebras.size();
    i ++
  ) {
    this->owner->objectContainer.semisimpleLieAlgebraPointers.addOnTop(
      &this->owner->objectContainer.semisimpleLieAlgebras.values[i]
    );
  }
  return this->owner->objectContainer.semisimpleLieAlgebraPointers[this->getLastChild().data];
}

template < >
MatrixTensor<Rational>& Expression::getValueNonConst() const {
  if (!this->isOfType<MatrixTensor<Rational> >()) {
    global.fatal << "Expression not of required type "
    << "MatrixTensorRational. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.matrixTensorRationals.getElement(this->getLastChild().data);
}

template < >
SemisimpleSubalgebras& Expression::getValueNonConst() const {
  if (!this->isOfType<SemisimpleSubalgebras>()) {
    global.fatal << "Expression not of required type "
    << "SemisimpleSubalgebras. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.semisimpleSubalgebras.values[this->getLastChild().data];
}

template < >
Plot& Expression::getValueNonConst() const {
  if (!this->isOfType<Plot>()) {
    global.fatal << "Expression not of required type "
    << "Plot. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.allPlots[this->getLastChild().data];
}

template < >
WeylGroupData& Expression::getValueNonConst() const {
  if (!this->isOfType<WeylGroupData>()) {
    global.fatal << "Expression not of required type "
    << "WeylGroupData. The expression equals "
    << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.semisimpleLieAlgebras.values[
    this->getLastChild().data
  ].weylGroup;
}

template < >
GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& Expression::getValueNonConst() const {
  if (!this->isOfType<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >()) {
    global.fatal << "Expression not of required type "
    << "WeylGroupRepresentation_Rational. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.weylGroupRepresentations.getElement(this->getLastChild().data);
}

template < >
ElementWeylGroup& Expression::getValueNonConst() const {
  if (!this->isOfType<ElementWeylGroup>()) {
    global.fatal << "Expression not of required type "
    << "ElementWeylGroup. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.weylGroupElements.getElement(this->getLastChild().data);
}

template < >
VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& Expression::getValueNonConst() const {
  if (!this->isOfType<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >()) {
    global.fatal << "Expression not of required type "
    << "WeylGroupVirtualRepresentation. The expression equals "
    << this->toString() << "." << global.fatal;
  }
  return this->owner->objectContainer.weylGroupVirtualRepresentations.getElement(this->getLastChild().data);
}

bool Expression::isMatrix(int* outputNumberOfRows, int* outputNumberOfColumns) const {
  MacroRegisterFunctionWithName("Expression::isMatrix");
  if (this->owner == nullptr) {
    return false;
  }
  if (this->size() < 1) {
    return false;
  }
  bool isGood = (*this)[0].startsWith(this->owner->opMatrix());
  if (!isGood) {
    return false;
  }
  if (outputNumberOfRows != nullptr) {
    *outputNumberOfRows = this->size() - 1;
  }
  if (outputNumberOfColumns != nullptr) {
    if (this->size() <= 1) {
      *outputNumberOfColumns = 0;
    } else {
      *outputNumberOfColumns = (*this)[1].size() - 1;
    }
  }
  return true;
}

//end Expression::getValueNonConst specializations.
//start Expression::convertInternally specializations.
template< >
bool Expression::convertInternally<Polynomial<AlgebraicNumber> >(Expression& output) const {
  MacroRegisterFunctionWithName("convertInternally_Polynomial_AlgebraicNumber");
  this->checkInitialization();
  if (this->isOfType<Rational>()) {
    Polynomial<AlgebraicNumber> resultP;
    resultP.makeConstant(this->getValue<Rational>());
    return output.assignValueWithContextOLD(resultP, this->getContext(), *this->owner);
  }
  if (this->isOfType<AlgebraicNumber>()) {
    Polynomial<AlgebraicNumber> resultP;
    resultP.makeConstant(this->getValue<AlgebraicNumber>());
    return output.assignValueWithContextOLD(resultP, this->getContext(), *this->owner);
  }
  if (this->isOfType<Polynomial<Rational> >()) {
    Polynomial<AlgebraicNumber> resultP;
    resultP = this->getValue<Polynomial<Rational> >();
    return output.assignValueWithContextOLD(resultP, this->getContext(), *this->owner);
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
    return output.assignValueWithContextOLD(resultP, this->getContext(), *this->owner);
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
bool Expression::convertInternally<RationalFraction<Rational> >(Expression& output) const {
  MacroRegisterFunctionWithName("Expression::convertInternally_RationalFunction_Rational");
  this->checkInitialization();
  if (this->isOfType<Rational>()) {
    RationalFraction<Rational> resultRF;
    resultRF.makeConstant(this->getValue<Rational>());
    return output.assignValueWithContextOLD(
      resultRF, this->getContext(), *this->owner
    );
  }
  if (this->isOfType<AlgebraicNumber>()) {
    Rational rationalValue;
    if (!this->getValue<AlgebraicNumber>().isRational(&rationalValue)) {
      return false;
    }
    RationalFraction<Rational> result;
    result.makeConstant(rationalValue);
    return output.assignValueWithContextOLD(result, this->getContext(), *this->owner);
  }
  if (this->isOfType<Polynomial<Rational> >()) {
    RationalFraction<Rational> result;
    result = this->getValue<Polynomial<Rational> >();
    return output.assignValueWithContextOLD(
      result, this->getContext(), *this->owner
    );
  }
  if (this->isOfType<RationalFraction<Rational> >()) {
    output = *this;
    return true;
  }
  (*this->owner)
  << "<hr>convertInternally_RationalFunction_Rational: Failed to convert "
  << this->toString() << " to rational function. ";
  return false;
}

template< >
bool Expression::convertInternally<RationalFraction<AlgebraicNumber> >(Expression& output) const {
  MacroRegisterFunctionWithName("Expression::converInternally_RationalFunction_AlgebraicNumber");
  this->checkInitialization();
  AlgebraicClosureRationals* closure = &this->owner->objectContainer.algebraicClosure;
  if (this->isOfType<Rational>()) {
    RationalFraction<AlgebraicNumber> result;
    AlgebraicNumber value;
    value.assignRational(this->getValue<Rational>(), closure);
    result.makeConstant(value);
    return output.assignValueWithContextOLD(
      result, this->getContext(), *this->owner
    );
  }
  if (this->isOfType<AlgebraicNumber>()) {
    RationalFraction<AlgebraicNumber> result;
    result.makeConstant(this->getValue<AlgebraicNumber>());
    return output.assignValueWithContextOLD(
      result, this->getContext(), *this->owner
    );
  }
  // TODO(tmilev): please implement conversion from Polynomial<Rational> to Polynomial<AlgebraicNumber>.
  if (this->isOfType<Polynomial<AlgebraicNumber> >()) {
    RationalFraction<AlgebraicNumber> result;
    result = this->getValue<Polynomial<AlgebraicNumber> >();
    return output.assignValueWithContextOLD(
      result, this->getContext(), *this->owner
    );
  }
  if (this->isOfType<RationalFraction<AlgebraicNumber> >()) {
    output = *this;
    return true;
  }
  (*this->owner)
  << "<hr>converInternally_RationalFunction_AlgebraicNumber: Failed to convert "
  << this->toString() << " to rational function with algebraic coefficients. ";
  return false;
}

template< >
bool Expression::convertInternally<ElementWeylAlgebra<Rational> >(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_ElementWeylAlgebra");
  this->checkInitialization();
  if (this->isOfType<Rational>()) {
    ElementWeylAlgebra<Rational> resultEWA;
    resultEWA.makeConstant(this->getValue<Rational>());
    return output.assignValueWithContextOLD(resultEWA, this->getContext(), *this->owner);
  }
  if (this->isOfType<Polynomial<Rational> >()) {
    ElementWeylAlgebra<Rational> resultEWA;
    resultEWA.assignPolynomial(this->getValue<Polynomial<Rational> >());
    return output.assignValueWithContextOLD(resultEWA, this->getContext(), *this->owner);
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
bool Expression::convertInternally<ElementUniversalEnveloping<RationalFraction<Rational> > >(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_RationalFunctionOld");
  this->checkInitialization();
  if (!this->isBuiltInType())
    return false;
  if (this->isOfType<ElementUniversalEnveloping<RationalFraction<Rational> > >()) {
    output = *this;
    return true;
  }
  SemisimpleLieAlgebra* theOwner = this->getAmbientSemisimpleLieAlgebraNonConstUseWithCaution();
  if (theOwner == nullptr) {
    this->owner->comments << "<hr>Failed to convert " << this->toString() << " (Lispified: " << this->toStringFull()
    << ") to element of universal enveloping -  failed to extract ambient Lie algebra. ";
    return false;
  }
  ElementUniversalEnveloping<RationalFraction<Rational> > outputUE;
  if (this->isOfType<Rational>()) {
    outputUE.makeConstant(this->getValue<Rational>(), *theOwner);
    return output.assignValueWithContextOLD(outputUE, this->getContext(), *this->owner);
  } else if (this->isOfType<Polynomial<Rational> >()) {
    RationalFraction<Rational> tempRF;
    tempRF = this->getValue<Polynomial<Rational> >();
    outputUE.makeConstant(tempRF, *theOwner);
    return output.assignValueWithContextOLD(outputUE, this->getContext(), *this->owner);
  } else if (this->isOfType<RationalFraction<Rational> >()) {
    outputUE.makeConstant(this->getValue<RationalFraction<Rational> >(), *theOwner);
    return output.assignValueWithContextOLD(outputUE, this->getContext(), *this->owner);
  }
  return *this->owner << "<hr>Failed to convert " << this->toString()
  << " to element of universal enveloping -  I don't know how. ";
}

template< >
bool Expression::convertInternally<ElementTensorsGeneralizedVermas<RationalFraction<Rational> > >(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_RationalFunctionOld");
  this->checkInitialization();
  if (this->isOfType<ElementTensorsGeneralizedVermas<RationalFraction<Rational> > >()) {
    output = *this;
    return true;
  }
  return false;
}

template< >
bool Expression::convertInternally<Rational>(Expression& output) const {
  MacroRegisterFunctionWithName("convertInternally");
  this->checkInitialization();
  if (this->isOfType<Rational>()) {
    output = *this;
    return true;
  }
  return false;
}

template< >
bool Expression::convertInternally<ElementZmodP>(Expression& output) const {
  MacroRegisterFunctionWithName("convertInternally");
  this->checkInitialization();
  if (this->isOfType<ElementZmodP>()) {
    output = *this;
    return true;
  }
  return false;
}

template< >
bool Expression::convertInternally<AlgebraicNumber>(Expression& output) const {
  MacroRegisterFunctionWithName("convertInternally");
  this->checkInitialization();
  if (this->isOfType<AlgebraicNumber>()) {
    output = *this;
    return true;
  }
  Rational rational;
  if (!this->isOfType<Rational>(&rational)) {
    return false;
  }
  AlgebraicNumber outRat = rational;
  return output.assignValueOLD(outRat, *this->owner);
}

template< >
bool Expression::convertInternally<double>(Expression& output) const {
  MacroRegisterFunctionWithName("convertInternally");
  this->checkInitialization();
  if (this->isOfType<double>()) {
    output = *this;
    return true;
  }
  double outputDouble = 0;
  if (this->evaluatesToDouble(&outputDouble)) {
    return output.assignValueOLD(outputDouble, *this->owner);
  }
  return false;
}
//end Expression::convertToType specializations.

bool Expression::checkConsistencyRecursively() const {
  MacroRegisterFunctionWithName("Expression::checkConsistencyRecursively");
  this->checkConsistency();
  for (int i = 0; i < this->children.size; i ++) {
    (*this)[i].checkConsistency();
  }
  return true;
}

bool Expression::checkConsistency() const {
  MacroRegisterFunctionWithName("Expression::checkConsistency");
  // warning: do not use toString method from here: toString itself calls CheckConsistency,
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
      global.fatal << "At the moment of writing, an expression of built-in type must have 3 "
      << "children: type, context, and index in Calculator. The expression is " << this->toStringFull()
      << global.fatal;
    }
    const Expression& mustBeTheContext = (*this)[1];
    if (!mustBeTheContext.startsWith(this->owner->opContext())) {
      global.fatal
      << "The second child of a built-in type must be a context. It is instead "
      << mustBeTheContext.toStringFull() << global.fatal;
    }
    for (int i = 1; i < mustBeTheContext.children.size; i ++) {
      bool isGood = false;
      const Expression& currentE = mustBeTheContext[i];
      if (currentE.startsWith(this->owner->opPolynomialVariables())) {
        isGood = true;
      }
      if (currentE.startsWith(this->owner->opSemisimpleLieAlgebra())) {
        isGood = true;
      }
      if (currentE.startsWith(this->owner->opWeylAlgebraVariables())) {
        isGood = true;
      }
      if (currentE.startsWith(this->owner->opMod())) {
        isGood = true;
      }
      if (!isGood) {
        global.fatal << "The context "
        << mustBeTheContext.toStringFull()
        << " has an entry which I do not recognize, namely, "
        << currentE.toStringFull() << ". " << global.fatal;
      }
    }
  }
  return true;
}

unsigned int Expression::hashFunction(const Expression& input) {
  return input.hashFunction();
}

unsigned int Expression::hashFunction() const {
  unsigned int result = static_cast<unsigned>(this->data) * someRandomPrimes[1];
  if (this->owner == nullptr) {
    if (this->children.size == 0) {
      return result;
    }
    global.fatal << "Uninitialized expression. " << global.fatal;
  }
  return this->children.hashFunction();
}

const Expression& Expression::operator[](int n) const {
  this->checkInitialization();
  int childIndex = this->children[n];
  if (childIndex < 0) {
    global.fatal << "<hr>The child of position "
    << n << " out of " << this->children.size - 1
    << " is not contained in the expression container. " << global.fatal;
  }
  return this->owner->allChildExpressions[childIndex];
}

Expression Expression::zero() {
  if (this->owner == nullptr) {
    return Expression::zeroStatic();
  }
  Expression result;
  return result.assignValueOLD(0, *this->owner);
}

Expression Expression::zeroStatic() {
  Expression result;
  return result;
}

bool Expression::addChildRationalOnTop(const Rational& inputRat) {
  this->checkInitialization();
  Expression ratE;
  ratE.assignValueOLD(inputRat, *this->owner);
  return this->addChildOnTop(ratE);
}

int Calculator::addChildExpression(const Expression& child) {
  int index = this->allChildExpressions.getIndex(child);
  if (index != - 1) {
    return index;
  }
  index = this->allChildExpressions.size;
  this->allChildExpressions.addOnTop(child);
  this->allChildExpressionHashes.addOnTop(child.hashFunction());
  return index;
}

bool Expression::addChildOnTop(const Expression& inputChild) {
  this->checkInitialization();
  this->children.addOnTop(
    this->owner->addChildExpression(inputChild)
  );
  return true;
}

bool Expression::setChildAtomValue(int childIndex, int atomValue) {
  this->checkInitialization();
  Expression atom;
  atom.makeAtom(atomValue, *this->owner);
  this->children.setObjectAtIndex(
    childIndex,
    this->owner->allChildExpressions.addNoRepetitionOrReturnIndexFirst(atom)
  );
  return true;
}

void Expression::removeLastChild() {
  this->children.removeLastObject();
}

void Expression::swapChildren(int left, int right) {
  this->children.swapTwoIndices(left, right);
}

bool Expression::setChildAtomValue(
  int childIndex, const std::string& atom
) {
  this->checkInitialization();
  Expression atomExpression;
  atomExpression.makeAtom(atom, *this->owner);
  this->children.setObjectAtIndex(
    childIndex, this->owner->addChildExpression(atomExpression)
  );
  return true;
}

void Expression::addChildIndices(List<int>& indices) {
  this->children.addListOnTop(indices);
}

void Expression::setExpectedSize(int expectedSize) {
  this->children.setExpectedSize(expectedSize);
}

void Expression::removeChildShiftDown(int childIndex) {
  this->children.removeIndexShiftDown(childIndex);
}

void Expression::setSize(int desiredSize) {
  this->checkInitialization();
  if (desiredSize > this->size()) {
    Expression empty(*this->owner);
    for (int i = this->children.size; i < desiredSize; i ++) {
      this->addChildOnTop(empty);
    }
    return;
  }
  this->children.setSize(desiredSize);
}

bool Expression::setChild(int childIndexInMe, const Expression& inputChild) {
  this->checkInitialization();
  int indexOfTheExpression = this->owner->addChildExpression(inputChild);
  this->children.setObjectAtIndex(childIndexInMe, indexOfTheExpression);
  return true;
}

bool Expression::setChild(int childIndexInMe, int childIndexInBoss) {
  this->checkInitialization();
  this->children.setObjectAtIndex(childIndexInMe, childIndexInBoss);
  if (this->children[childIndexInMe] != childIndexInBoss) {
    global.fatal << "Bad child index. " << global.fatal;
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
    this->owner->addOperationNoRepetitionOrReturnIndexFirst(theOperationString)
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

bool Expression::startsWithGivenOperation(const std::string& operation, int desiredChildren) const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->size() == 0) {
    return false;
  }
  int theOpIndex = this->owner->operations.getIndex(operation);
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

bool Expression::startsWith(int operation, int numberOfChildren) const {
  if (numberOfChildren != - 1) {
    if (this->children.size != numberOfChildren) {
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
  if (operation == - 1) {
    return true;
  }
  return (*this)[0].data == operation;
}

bool Expression::getExpressionLeafs(HashedList<Expression>& outputAccumulateLeafs) const {
  MacroRegisterFunctionWithName("Expression::getExpressionLeafs");
  if (this->owner == nullptr) {
    return true;
  }
  RecursionDepthCounter(&this->owner->recursionDepth);
  if (this->owner->recursionDepthExceededHandleRoughly("In Expression::getExpressionLeafs:")) {
    return false;
  }
  if (this->isBuiltInType() || this->isAtom()) {
    outputAccumulateLeafs.addOnTopNoRepetition(*this);
    return true;
  }
  for (int i = 0; i < this->size(); i ++) {
    if (!((*this)[i].getExpressionLeafs(outputAccumulateLeafs))) {
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
  RecursionDepthCounter(&this->owner->recursionDepth);
  if (this->owner->recursionDepthExceededHandleRoughly("In Expression::getFreeVariables:")) {
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

bool Expression::allowedAsFreeVariableAtom(const std::string &input) const {
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

bool Expression::getFreeVariables(
  HashedList<Expression>& outputAccumulateFreeVariables, bool excludeNamedConstants
) const {
  MacroRegisterFunctionWithName("Expression::getFreeVariables");
  if (this->owner == nullptr) {
    return true;
  }
  RecursionDepthCounter(&this->owner->recursionDepth);
  if (this->owner->recursionDepthExceededHandleRoughly("In Expression::getFreeVariables:")) {
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
      doAddExpression = this->allowedAsFreeVariableAtom(atomName);
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
    if (!((*this)[i].getFreeVariables(outputAccumulateFreeVariables, excludeNamedConstants))) {
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

bool Expression::containsAsSubExpressionNoBuiltInTypes(const Expression& input) const {
  if (this->owner == nullptr) {
    return false;
  }
  RecursionDepthCounter theCounter(&this->owner->recursionDepth);
  if (this->owner->recursionDepthExceededHandleRoughly("In Expression::ContainsAsSubExpression: ")) {
    return false;
  }
  if (*this == input) {
    return true;
  }
  if (!this->isSuitableForRecursion()) {
    return false;
  }
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].containsAsSubExpressionNoBuiltInTypes(input)) {
      return true;
    }
  }
  return false;
}

bool Expression::containsAsSubExpressionNoBuiltInTypes(int inputAtom) const {
  if (this->owner == nullptr) {
    return false;
  }
  Expression theE;
  theE.makeAtom(inputAtom, *this->owner);
  return this->containsAsSubExpressionNoBuiltInTypes(theE);
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

bool Expression::isSequenceNElements(int n) const {
  if (this->owner == nullptr) {
    return false;
  }
  return this->startsWith(this->owner->opSequence(), n + 1);
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

bool Expression::isEqualToTwo() const {
  int theInt;
  if (this->isSmallInteger(&theInt)) {
    return theInt == 2;
  }
  return false;
}

bool Expression::isEqualToHalf() const {
  Rational rational;
  if (this->isRational(&rational)) {
    Rational half(1, 2);
    return rational == half;
  }
  return false;
}

bool Expression::isKnownToBeNonNegative() const {
  MacroRegisterFunctionWithName("Expression::isKnownToBeNonNegative");
  double theDouble = 0;
  if (this->evaluatesToDouble(&theDouble)) {
    return theDouble >= 0;
  }
  if (this->owner == nullptr) {
    return false;
  }
  Expression testInequality, testResult;
  Calculator& calculator = *(this->owner);
  testInequality.makeXOX(calculator, calculator.opGreaterThan(), *this, calculator.expressionZero());
  if (this->owner->evaluateExpression(calculator, testInequality, testResult)) {
    if (testResult.isEqualToOne()) {
      return true;
    }
  }
  testInequality.makeXOX(calculator, calculator.opGreaterThanOrEqualTo(), *this, calculator.expressionZero());
  if (calculator.evaluateExpression(calculator, testInequality, testResult)) {
    if (testResult.isEqualToOne()) {
      return true;
    }
  }
  return false;
}

bool Expression::isNegativeConstant() const {
  if (this->owner == nullptr) {
    return false;
  }
  Rational rational;
  if (this->isOfType<Rational>(&rational)) {
    return rational.isNegative();
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

bool Expression::isEqualToMOne() const {
  MacroRegisterFunctionWithName("Expression::isEqualToMOne");
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
  if (this->isOfType<RationalFraction<Rational> >()) {
    return this->getValue<RationalFraction<Rational> >().isEqualToZero();
  }
  return false;
}

bool Expression::assignError(Calculator& owner, const std::string& error) {
  this->reset(owner, 2);
  this->checkConsistency();
  this->addChildAtomOnTop(owner.opError());
  this->checkConsistency();
  std::string errorReplacedWhiteSpace = StringRoutines::replaceAll(error, " ", "~");
  return this->addChildValueOnTop(errorReplacedWhiteSpace);
}

bool Expression::isSmallInteger(int* whichInteger) const {
  Rational rational;
  if (this->isOfType<Rational>(&rational)) {
    return rational.isSmallInteger(whichInteger);
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
  AlgebraicNumber algebraic;
  if (this->isOfType(&algebraic)) {
    return algebraic.isRational(whichRational);
  }
  return false;
}

bool Expression::divisionByMeShouldBeWrittenInExponentForm() const {
  if (this->owner == nullptr) {
    return false;
  }
  RecursionDepthCounter theCounter(&this->owner->recursionDepth);
  if (this->owner->recursionDepthExceededHandleRoughly()) {
    return false;
  }
  if (this->startsWith(this->owner->opTimes())) {
    for (int i = 1; i < this->children.size; i ++) {
      if (!(*this)[i].divisionByMeShouldBeWrittenInExponentForm()) {
        return false;
      }
    }
  }
  if (this->startsWith(this->owner->opPower(), 3)) {
    return (*this)[1].divisionByMeShouldBeWrittenInExponentForm();
  }
  if (this->startsWith(this->owner->opPlus())) {
    return false;
  }
  return true;
}

bool Expression::isPositiveNumber() const {
  MacroRegisterFunctionWithName("Expression::isPositiveNumber");
  Rational rational;
  if (this->isOfType<Rational>(&rational)) {
    return rational.isPositive();
  }
  double theDouble = - 1;
  if (!this->evaluatesToDouble(&theDouble)) {
    return false;
  }
  if (FloatingPoint::isNaN(theDouble)) {
    return false;
  }
  return theDouble > 0;
}

bool Expression::isConstantNumber() const {
  MacroRegisterFunctionWithName("Expression::isConstantNumber");
  if (this->owner == nullptr) {
    return false;
  }
  RecursionDepthCounter thecounter(&this->owner->recursionDepth);
  if (this->owner->recursionDepthExceededHandleRoughly("In Expression::isConstantNumber: ")) {
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

bool Expression::isAlgebraicRadical() const {
  MacroRegisterFunctionWithName("Expression::isAlgebraicRadical");
  if (this->owner == nullptr) {
    return false;
  }
  RecursionDepthCounter thecounter(&this->owner->recursionDepth);
  if (this->owner->recursionDepthExceededHandleRoughly("In Expression::isAlgebraicRadical: ")) {
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
      if (!(*this)[i].isAlgebraicRadical()) {
        return false;
      }
    }
    return true;
  }
  if (this->startsWith(this->owner->opPower(), 3)) {
    if (!(*this)[2].isRational()) {
      return false;
    }
    return (*this)[1].isAlgebraicRadical();
  }
  if (this->startsWith(this->owner->opSqrt(), 3)) {
    if (!(*this)[1].isRational()) {
      return false;
    }
    return (*this)[2].isAlgebraicRadical();
  }
  return false;
}

bool Expression::isInteger(LargeInteger* whichInteger) const {
  Rational rational;
  if (!this->isOfType<Rational>(&rational)) {
    return false;
  }
  return rational.isInteger(whichInteger);
}

bool Expression::isIntegerNonNegative(LargeIntegerUnsigned* whichInteger) const {
  Rational rational;
  if (!this->isOfType<Rational>(&rational)) {
    return false;
  }
  if (rational.isNonPositive()) {
    return false;
  }
  if (whichInteger == nullptr) {
    return rational.isInteger();
  }
  LargeInteger container;
  if (!rational.isInteger(&container)) {
    return false;
  }
  *whichInteger = container.value;
  return true;
}

bool Expression::isIntegerFittingInInt(int* whichInteger) const {
  Rational rational;
  if (!this->isOfType<Rational>(&rational)) {
    return false;
  }
  return rational.isIntegerFittingInInt(whichInteger);
}

bool Expression::makeAtom(const std::string& atomName, Calculator& newBoss) {
  this->reset(newBoss);
  this->data = newBoss.addOperationNoRepetitionOrReturnIndexFirst(atomName);
  return true;
}

void Expression::getBaseExponentForm(Expression& outputBase, Expression& outputExponent) const {
  this->checkInitialization();
  if (this->startsWith(this->owner->opPower(), 3)) {
    outputBase = (*this)[1];
    outputExponent = (*this)[2];
    return;
  }
  outputBase = *this;
  outputExponent.assignValueOLD(1, *this->owner);
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
    if ((*this)[0].startsWith(this->owner->opMatrix())) {
      if ((*this)[0].size() > 2) {
        result.fromExpression((*this)[0][2]);
        return result;
      }
    }
  }
  global.fatal
  << "getContext called on an Expression "
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
      theMax = MathRoutines::maximum((*this)[i].size() - 1, theMax);
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
  outputCoeff.assignValueOLD(1, *this->owner);
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
  RecursionDepthCounter theCounter(&this->owner->recursionDepth);
  if (this->owner->recursionDepthExceededHandleRoughly("While computing Expression::getExpressionTreeSize: ")) {
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
    if (FloatingPoint::isNaN(left) && FloatingPoint::isNaN(right)) {
      return false; //WARNING nan == nan evaluates to false!
    }
    // left != right generates a bizarre warning:
    // "comparing floating point with == or != is unsafe".
    if ((left - right) != 0.0) {
      if (FloatingPoint::isNaN(left)) {
        return true;
      }
      if (FloatingPoint::isNaN(right)) {
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
  bool result = leftMon.greaterThanNoCoefficient(rightMon);
  return result;
}

bool Expression::greaterThanNoCoefficient(const Expression& other) const {
  MacroRegisterFunctionWithName("Expression::greaterThanNoCoefficient");
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
    return this->data > other.data;
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
  FormatExpressions* format
) {
  bool useQuotes = format == nullptr ? false : format->flagUseQuotes;
  bool isFinal = format == nullptr ? true : format->flagExpressionIsFinal;
  if (!useQuotes) {
    if (isFinal) {
      out
      << StringRoutines::Conversions::stringToCalculatorDisplay(input.getValue<std::string>());
    } else {
      out << HtmlRoutines::convertStringToHtmlString(input.getValue<std::string>(), false);
    }
  } else {
    out << "\"" << StringRoutines::Conversions::escapeJavascriptLike(input.getValue<std::string>()) << "\"";
  }
  return true;
}

template<>
bool Expression::toStringBuiltIn<JSData>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  (void) format;
  JSData data = input.getValue<JSData>();
  std::string dataString = data.toString(&JSData::PrintOptions::HTML());
  out << dataString;
  return true;
}

template<>
bool Expression::toStringBuiltIn<Rational>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  (void) format;
  if (input.hasNonEmptyContext()) {
    out << "Rational{}(" << input[1].toString() << ", ";
  }
  if (!input.owner->flagUseFracInRationalLaTeX) {
    out << input.getValue<Rational>().toString();
  } else {
    out << input.getValue<Rational>().toStringFrac();
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
  FormatExpressions* format
) {
  (void) format;
  FormatExpressions formatEllipticCurve;
  input.getContext().getFormat(formatEllipticCurve);
  formatEllipticCurve.flagUseFrac = true;
  out << input.getValue<ElementEllipticCurve<Rational> >().toString(&formatEllipticCurve);
  return true;
}

template<>
bool Expression::toStringBuiltIn<ElementEllipticCurve<ElementZmodP> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  (void) format;
  FormatExpressions formatElementZModP;
  input.getContext().getFormat(formatElementZModP);
  formatElementZModP.flagUseFrac = true;
  out << input.getValue<ElementEllipticCurve<ElementZmodP> >().toString(&formatElementZModP);
  return true;
}

template<>
bool Expression::toStringBuiltIn<ElementZmodP>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  (void) format;
  out << input.getValue<ElementZmodP>().toString();
  return true;
}

template<>
bool Expression::toStringBuiltIn<InputBox>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  (void) format;
  out << input.getValue<InputBox>().getUserInputBox();
  return true;
}

template<>
bool Expression::toStringBuiltIn<GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  (void) format;
  out << input.getValue<GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> >().toString();
  return true;
}

template<>
bool Expression::toStringBuiltIn<ElementHyperoctahedralGroupR2>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  out << input.getValue<ElementHyperoctahedralGroupR2>().toString(format);
  return true;
}

template<>
bool Expression::toStringBuiltIn<Polynomial<Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  (void) format;
  bool showContext = input.owner == nullptr ? false : input.owner->flagDisplayContext;
  FormatExpressions formatLocal;
  input.getContext().getFormat(formatLocal);
  formatLocal.flagUseFrac = true;
  if (!input.owner->flagHidePolynomialBuiltInTypeIndicator) {
    out << "Polynomial{}(";
  }
  out << input.getValue<Polynomial<Rational> >().toString(&formatLocal);
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
  FormatExpressions* format
) {
  (void) format;
  bool showContext = input.owner == nullptr ? false : input.owner->flagDisplayContext;
  FormatExpressions formatLocal;
  const Polynomial<ElementZmodP>& polynomial = input.getValue<Polynomial<ElementZmodP> >();
  input.getContext().getFormat(formatLocal);
  formatLocal.flagSuppressModP = true;
  formatLocal.flagUseFrac = true;
  if (!polynomial.isEqualToZero()) {
    out << polynomial.coefficients[0].toStringPolynomial(polynomial, &formatLocal);
  } else {
    out << "PolynomialModP(0)";
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
  FormatExpressions* format
) {
  (void) format;
  FormatExpressions formatLocal;
  input.getContext().getFormat(formatLocal);
  bool showContext = input.owner == nullptr ? false : input.owner->flagDisplayContext;
  formatLocal.flagUseFrac = true;
  out << "PolynomialAlgebraicNumbers{}(";
  std::string currentString = input.getValue<Polynomial<AlgebraicNumber> >().toString(&formatLocal);
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
  FormatExpressions* format
) {
  (void) format;
  FormatExpressions formatLocal;
  input.getContext().getFormat(formatLocal);
  formatLocal.flagUseFrac = true;
  const PolynomialModuloPolynomial<ElementZmodP>& element = input.getValue<PolynomialModuloPolynomial<ElementZmodP> >();
  ElementZmodP sample;
  formatLocal.flagSuppressModP = true;
  if (!element.modulus.isEqualToZero()) {
    sample = element.modulus.coefficients[0];
    out
    << sample.toStringPolynomial(element.value, &formatLocal)
    << " mod "
    << sample.toStringPolynomial(element.modulus, &formatLocal);
  } else {
    out
    << element.value.toString(&formatLocal)
    << " mod "
    << element.modulus.toString(&formatLocal);
  }
  return true;
}

template<>
bool Expression::toStringBuiltIn<RationalFraction<Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  bool showContext = input.owner == nullptr ? false : input.owner->flagDisplayContext;
  (void) format;
  FormatExpressions formatLocal;
  input.getContext().getFormat(formatLocal);
  formatLocal.flagUseFrac = true;
  out << "MakeRationalFunction{}("
  << input.getValue<RationalFraction<Rational> >().toString(&formatLocal) << ")";
  if (showContext) {
    out << "[" << input.getContext().toString() << "]";
  }
  return true;
}

template<>
bool Expression::toStringBuiltIn<RationalFraction<AlgebraicNumber> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  bool showContext = input.owner == nullptr ? false : input.owner->flagDisplayContext;
  (void) format;
  FormatExpressions formatLocal;
  input.getContext().getFormat(formatLocal);
  formatLocal.flagUseFrac = true;
  out << "MakeRationalFunction{}("
  << input.getValue<RationalFraction<AlgebraicNumber> >().toString(&formatLocal) << ")";
  if (showContext) {
    out << "[" << input.getContext().toString() << "]";
  }
  return true;
}

template<>
bool Expression::toStringBuiltIn<RationalFraction<ElementZmodP> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  bool showContext = input.owner == nullptr ? false : input.owner->flagDisplayContext;
  (void) format;
  FormatExpressions formatLocal;
  input.getContext().getFormat(formatLocal);
  formatLocal.flagSuppressModP = true;
  formatLocal.flagUseFrac = true;
  const RationalFraction<ElementZmodP>& data = input.getValue<RationalFraction<ElementZmodP> >();
  if (data.expressionType == RationalFraction<ElementZmodP>::TypeExpression::typeRationalFunction) {
    ElementZmodP constantSample = data.numerator.getElementConst().coefficients[0];
    out << "\\frac{"
    << constantSample.toStringPolynomial(data.numerator.getElementConst(), &formatLocal)
    << "} {"
    << constantSample.toStringPolynomial(data.denominator.getElementConst(), &formatLocal)
    << "}";
  } else if (data.expressionType == RationalFraction<ElementZmodP>::TypeExpression::typePolynomial) {
    ElementZmodP constantSample = data.numerator.getElementConst().coefficients[0];
    out << constantSample.toStringPolynomial(data.numerator.getElementConst(), &formatLocal);
  } else {
    Polynomial<ElementZmodP> zero;
    out << data.constantValue.toStringPolynomial(zero, &formatLocal);
  }
  if (showContext) {
    out << "[" << input.getContext().toString() << "]";
  }
  return true;
}

template<>
bool Expression::toStringBuiltIn<Weight<Polynomial<Rational> > >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  (void) format;
  FormatExpressions formatLocal;
  input.getContext().getFormat(formatLocal);
  formatLocal.flagFormatWeightAsVectorSpaceIndex = false;
  out << input.getValue<Weight<Polynomial<Rational> > >().toString(&formatLocal);
  return true;
}

template<>
bool Expression::toStringBuiltIn<SemisimpleLieAlgebra*>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  MacroRegisterFunctionWithName("Expression::toStringBuiltIn(SemisimpleLieAlgebra*)");
  (void) format;
  out << "SSLieAlg{}("
  << input.getValue<SemisimpleLieAlgebra*>()->toStringLieAlgebraName()
  << ")";
  return true;
}

template<>
bool Expression::toStringBuiltIn<ElementUniversalEnveloping<RationalFraction<Rational> > >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  (void) format;
  FormatExpressions formatLocal;
  input.getContext().getFormat(formatLocal);
  out << "UEE{}(" //<< input.getContext().toString() << ", "
  << input.getValue<ElementUniversalEnveloping<RationalFraction<Rational> > >().toString(&formatLocal)
  << ")";
  return true;
}

template<>
bool Expression::toStringBuiltIn<MatrixTensor<Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  (void) format;
  FormatExpressions formatLocal;
  input.getContext().getFormat(formatLocal);
  formatLocal.flagUseLatex = true;
  formatLocal.flagUseHTML = false;
  if (input.getValue<MatrixTensor<Rational> >().getMinimumNumberOfColumnsNumberOfRows() < 20) {
    out << "MatrixRationalsTensorForm{}("
    << input.getValue<MatrixTensor<Rational> > ().toStringMatrixForm(&formatLocal)
    << ")";
  } else {
    out << input.getValue<MatrixTensor<Rational> >().toString();
  }
  return true;
}

template<>
bool Expression::toStringBuiltIn<ElementTensorsGeneralizedVermas<RationalFraction<Rational> > >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  (void) format;
  FormatExpressions localFormat;
  input.getContext().getFormat(localFormat);
  out << "ETGVM{}(";
  out << input.getValue<ElementTensorsGeneralizedVermas<RationalFraction<Rational> > >().toString(&localFormat);
  out << ")";
  return true;
}

template<>
bool Expression::toStringBuiltIn<Plot>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  bool isFinal = format == nullptr ? true : format->flagExpressionIsFinal;
  if (isFinal) {
    Plot& thePlot = input.getValueNonConst<Plot>();
    thePlot.flagIncludeExtraHtmlDescriptions = (format == nullptr) ? true : format->flagIncludeExtraHtmlDescriptionsInPlots;
    thePlot.flagPlotShowJavascriptOnly = input.owner->flagPlotShowJavascriptOnly;
    out << thePlot.getPlotHtml(*input.owner);
    if (input.owner->flagWriteLatexPlots) {
      out << input.owner->writeDefaultLatexFileReturnHtmlLink(thePlot.getPlotStringAddLatexCommands(false), nullptr, true);
      out << "<br><b>LaTeX code used to generate the output. </b><br>"
      << thePlot.getPlotStringAddLatexCommands(true);
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
  FormatExpressions* format
) {
  (void) format;
  FormatExpressions formatLocal;
  WeylGroupData& theGroup = input.getValueNonConst<WeylGroupData>();
  formatLocal.flagUseLatex = true;
  formatLocal.flagUseHTML = false;
  formatLocal.flagUseReflectionNotation = true;
  out << theGroup.toString(&formatLocal);
  return true;
}

template<>
bool Expression::toStringBuiltIn<ElementWeylGroup >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  (void) format;
  FormatExpressions localFormat;
  const ElementWeylGroup& element = input.getValue<ElementWeylGroup>();
  localFormat.flagUseLatex = true;
  localFormat.flagUseHTML = false;
  localFormat.flagUseReflectionNotation = true;
  out << element.toString(&localFormat);
  return true;
}

template<>
bool Expression::toStringBuiltIn<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  (void) format;
  FormatExpressions localFormat;
  const GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& element =
  input.getValue<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >();
  localFormat.flagUseLatex = true;
  localFormat.flagUseHTML = false;
  localFormat.flagUseReflectionNotation = true;
  out << element.toString(&localFormat);
  return true;
}

template<>
bool Expression::toStringBuiltIn<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  (void) format;
  FormatExpressions formatLocal;
  input.getContext().getFormat(formatLocal);
  const VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& element =
  input.getValue<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >();
  formatLocal.flagUseLatex = true;
  formatLocal.flagUseHTML = false;
  formatLocal.flagUseReflectionNotation = true;
  out << element.toString(&formatLocal);
  return true;
}

template<>
bool Expression::toStringBuiltIn<CharacterSemisimpleLieAlgebraModule<Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  (void) format;
  CharacterSemisimpleLieAlgebraModule<Rational> element = input.getValue<CharacterSemisimpleLieAlgebraModule<Rational> >();
  out << element.toString();
  return true;
}

template<>
bool Expression::toStringBuiltIn<SemisimpleSubalgebras>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  (void) format;
  FormatExpressions formatLocal;
  SemisimpleSubalgebras& subalgebras = input.getValueNonConst<SemisimpleSubalgebras>();
  formatLocal.flagUseLatex = true;
  formatLocal.flagUseHTML = true;
  formatLocal.flagCandidateSubalgebraShortReportOnly = false;
  formatLocal.flagIncludeMutableInformation = false;
  out << subalgebras.toString(&formatLocal, false);
  return true;
}

template<>
bool Expression::toStringBuiltIn<double>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  (void) format;
  std::string currentString = FloatingPoint::doubleToString(input.getValue<double>());
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
  FormatExpressions* format
) {
  (void) format;
  FormatExpressions formatLocal;
  if (input.owner->flagUseFracInRationalLaTeX) {
    formatLocal.flagUseFrac = true;
  }
  std::string currentString = input.getValue<AlgebraicNumber>().toString(&formatLocal);
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
  FormatExpressions* format
) {
  (void) format;
  out << input.getValue<LittelmannPath>().toString();
  return true;
}

template<>
bool Expression::toStringBuiltIn<ElementWeylAlgebra<Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  (void) format;
  FormatExpressions formatLocal;
  bool showContext = input.owner == nullptr ? false : input.owner->flagDisplayContext;
  input.getContext().getFormat(formatLocal);
  formatLocal.flagUseHTML = false;
  formatLocal.flagUseLatex = true;
  out << "ElementWeylAlgebra{}(";
  out << input.getValue<ElementWeylAlgebra<Rational> >().toString(&formatLocal);
  out << ")";
  if (showContext) {
    out << "[" << input.getContext().toString() << "]";
  }
  return true;
}

bool Expression::toStringData(std::stringstream& out, FormatExpressions* format) const {
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
    } else if (this->data < this->owner->getOperations().size && this->data >= 0) {
      out << this->owner->getOperations()[this->data];
    } else {
      out << "[unknown atom of value " << this->data << "]";
    }
    return true;
  }
  if (this->isMatrixOfType<RationalFraction<Rational> >()) {
    FormatExpressions format;
    this->getContext().getFormat(format);
    format.flagUseHTML = false;
    format.flagUseLatex = true;
    Matrix<RationalFraction<Rational> > matrix;
    this->owner->functionGetMatrix(*this, matrix);
    out << matrix.toString(&format);
    return true;
  }
  int typeIndex = - 1;
  if (!this->isBuiltInType(&typeIndex)) {
    return false;
  }
  Calculator& commands = *this->owner;
  // The following handlers are initialized in Calculator::initializeToStringHandlers.
  if (!commands.toStringDataHandlers.contains(typeIndex)) {
    return false;
  }
  // handler must hold the function pointer:
  // Expression::toStringBuiltIn<builtInType>,
  // where builtInType is one of the types registered in
  // Calculator::initializeToStringHandlers.
  Expression::ToStringHandler handler = commands.toStringDataHandlers.getValueNoFail(typeIndex);
  return handler(*this, out, format);
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
    out << this->data << " ";
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
    if (firstE.isOperationGiven(this->owner->opPower())) {
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

bool Expression::needsParenthesisForMultiplication(FormatExpressions* format) const {
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
    return this->getValue<Rational>().needsParenthesisForMultiplication(format);
  }
  if (this->isOfType<AlgebraicNumber>()) {
    return this->getValue<AlgebraicNumber>().needsParenthesisForMultiplication(format);
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
    if (firstE.isOperationGiven(this->owner->opPower())) {
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

bool CalculatorBasics::flattenCommandEnclosuresOneLayeR(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (input.size() != 2) {
    return false;
  }
  return CalculatorBasics::functionFlattenCommandEnclosuresOneLayer(
    calculator, input[1], output
  );
}

bool CalculatorBasics::functionFlattenCommandEnclosuresOneLayer(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerFlattenCommandEnclosuresOneLayer");
  if (input.startsWith(calculator.opCommandEnclosure())) {
    if (input.size() <= 1) {
      return false;
    }
    Expression result(calculator);
    result.addChildAtomOnTop(calculator.opCommandEnclosureStart());
    if (input.size() == 2) {
      result.addChildOnTop(input[1]);
      result.addChildAtomOnTop(calculator.opCommandEnclosureFinish());
      output = result;
      return true;
    }
    for (int i = 1; i < input.size(); i ++) {
      result.addChildOnTop(input[i]);
    }
    result.addChildAtomOnTop(calculator.opCommandEnclosureFinish());
    output = result;
    return true;
  }
  if (!input.startsWith(calculator.opCommandSequence())) {
    return false;
  }
  Expression result(calculator);
  for (int i = 0; i < input.size(); i ++) {
    if (input[i].startsWith(calculator.opCommandEnclosure())) {
      bool processed = false;
      result.addChildAtomOnTop(calculator.opCommandEnclosureStart());
      if (input[i].size() == 2) {
        if (input[i][1].startsWith(calculator.opCommandSequence())) {
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
      result.addChildAtomOnTop(calculator.opCommandEnclosureFinish());
    } else if (input[i].startsWithGivenOperation("MatchesPattern")) {
      result.addChildAtomOnTop(calculator.opCommandEnclosureStart());
      result.addChildOnTop(input[i]);
      result.addChildAtomOnTop(calculator.opCommandEnclosureFinish());
    } else {
      result.addChildOnTop(input[i]);
    }
  }
  output = result;
  return true;
}

std::string Expression::toStringAllSlidersInExpression() const {
  HashedList<std::string, HashFunctions::hashFunction> boxNames;
  if (!this->hasInputBoxVariables(&boxNames)) {
    return "";
  }
  if (
    this->owner->objectContainer.userInputBoxSliderDisplayed.size <
    this->owner->objectContainer.userInputTextBoxesWithValues.size()
  ) {
    this->owner->objectContainer.resetSliders();
  }
  MapReferences<std::string, InputBox, MathRoutines::hashString>&
  sliders = this->owner->objectContainer.userInputTextBoxesWithValues;
  std::stringstream out;
  for (int i = 0; i < boxNames.size; i ++) {
    if (!sliders.contains(boxNames[i])) {
      out << boxNames[i] << " not found. ";
      continue;
    }
    int index = sliders.getIndex(boxNames[i]);
    if (this->owner->objectContainer.userInputBoxSliderDisplayed[index]) {
      continue;
    }
    this->owner->objectContainer.userInputBoxSliderDisplayed[index] = true;
    InputBox& box = sliders.values[index];
    std::string theSliderName = box.getSliderName();
    out << "<input name='"
    << theSliderName
    << "' type='range'";
    double theReader = 0;
    out << std::fixed;
    out.precision(4);
    if (box.min.evaluatesToDouble(&theReader)) {
      out << "min='" << theReader << "' ";
    }
    if (box.max.evaluatesToDouble(&theReader)) {
      out << "max='" << theReader << "' ";
    }
    if (box.step.evaluatesToDouble(&theReader)) {
      out << "step ='" << theReader << "' ";
    }
    if (box.value.evaluatesToDouble(&theReader)) {
      out << "value='" << theReader << "' ";
    } else {
      out << "value='1' ";
    }
    out << "></input>";
  }
  return out.str();
}

bool Expression::requiresNoMathTags() const {
  return
  this->hasType<Plot> () ||
  this->isOfType<std::string>() ||
  this->isOfType<JSData> () ||
  this->isOfType<SemisimpleSubalgebras>() ||
  this->isOfType<WeylGroupData>() ||
  this->isOfType<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >();
}

bool Expression::toStringTimes(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opTimes(), 3)) {
    return false;
  }
  std::string secondE = input[2].toString(format);
  if (input[1].isOperationGiven(input.owner->opSqrt())) {
    // A malformed expression such as: "\sqrt 3" will be parsed as "sqrt * 3"
    // and later corrected to "\sqrt{3}".
    out << "\\sqrt{" << secondE << "}";
    return true;
  }
  std::string firstE = input[1].toString(format);
  bool firstNeedsBrackets = input[1].needsParenthesisForMultiplication();
  bool secondNeedsBrackets = input[2].needsParenthesisForMultiplicationWhenSittingOnTheRightMost(&(input[1]));
  if (secondE.size() > 0) {
    if (secondE[0] == '-') {
      secondNeedsBrackets = true;
    }
  }
  bool mustHaveTimes = false;
  bool collapseUnits = true;
  if (format != nullptr) {
    collapseUnits = !format->flagDontCollalpseProductsByUnits;
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
      if (StringRoutines::stringBeginsWith(secondE, "\\frac")) {
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
  FormatExpressions* format
) const {
  std::string secondE = (*this)[2].toString(format);
  std::string firstE = (*this)[1].toString(format);
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
  FormatExpressions* format
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
    std::string firstE = input[1].toString(format);
    std::string secondE = input[2].toString(format);
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
    if (format != nullptr) {
      maintain.initialize(format->flagExpressionNewLineAllowed);
      format->flagExpressionNewLineAllowed = false;
    } else {
      // Compilers will complain at certain optimization levels about
      // maintain not being initialized.
      maintain.toMaintain = nullptr;
      maintain.contentAtStart = false;
    }
    std::string firstE = input[1].toString(format);
    std::string secondE = input[2].toString(format);
    out << "\\frac{" << firstE << "}{" << secondE << "}";
  }
  return true;
}

bool Expression::toStringPower(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  Calculator& commands = *input.owner;
  if (!input.startsWith(commands.opPower(), 3)) {
    return false;
  }
  bool involvesExponentsInterpretedAsFunctions = false;
  const Expression& firstE = input[1];
  const Expression& secondE = input[2];
  bool isSuperScriptOfUnderscoredOperator = false;
  if (firstE.startsWith(commands.opUnderscore(), 3)) {
    if (firstE[1].isOperationGiven(commands.opIntegral())) {
      isSuperScriptOfUnderscoredOperator = true;
    }
  }
  if (isSuperScriptOfUnderscoredOperator) {
    out << firstE[1].toString(format) << "_{"
    << firstE[2].toString(format) << "}^{"
    << secondE.toString(format) << "}";
    return true;
  }

  if (firstE.startsWith(- 1, 2)) {
    bool shouldProceed = firstE[0].isAtomWhoseExponentsAreInterpretedAsFunction() &&
    !secondE.isEqualToMOne() && secondE.isRational();
    if (
      shouldProceed && firstE[0].isOperationGiven(commands.opLog()) &&
      commands.flagUseLnAbsInsteadOfLogForIntegrationNotation
    ) {
      shouldProceed = false;
    }
    if (shouldProceed) {
      involvesExponentsInterpretedAsFunctions = true;
      Expression newFunE;
      newFunE.makeXOX(*input.owner, commands.opPower(), firstE[0], input[2]);
      newFunE.checkConsistency();
      out << "{" << newFunE.toString(format) << "}{}";
      if (
        firstE[1].needsParenthesisForMultiplicationWhenSittingOnTheRightMost() ||
        firstE[1].startsWith(commands.opTimes())
      ) {
        out << "\\left(" << firstE[1].toString(format) << "\\right)";
      } else {
        out << firstE[1].toString(format);
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
      out << "\\sqrt{" << input[1].toString(format) << "}";
    } else {
      std::string secondEstr = input[2].toString(format);
      std::string firstEstr = input[1].toString(format);
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
  std::stringstream& out, FormatExpressions* format
) const {
  if (this->size() < 2) {
    return false;
  }
  out << (*this)[0].toString(format);
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
    out << (*this)[i].toString(format);
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
  FormatExpressions* format
) const {
  if (!this->isListStartingWithAtom(this->owner->opCommandSequence())) {
    return false;
  }
  MemorySaving<FormatExpressions> temporaryFormat;
  if (format == nullptr) {
    format = &temporaryFormat.getElement();
    format->flagExpressionIsFinal = true;
  }
  bool isFinal = format->flagExpressionIsFinal;
  bool createTable = (startingExpression != nullptr);
  bool createSingleTable = false;
  if (createTable == false && format != nullptr && !global.flagRunningConsoleRegular) {
    createSingleTable = format->flagMakingExpressionTableWithLatex;
    format->flagMakingExpressionTableWithLatex = false;
  }
  if (!createSingleTable && !createTable && this->size() > 2) {
    out << "(";
  }
  if (createSingleTable) {
    out << "<table class='tableCalculatorOutput'>";
  }
  std::string currentInput, currentOutput;
  if (outputJS != nullptr) {
    (*outputJS)["input"].elementType = JSData::token::tokenArray;
    (*outputJS)["output"].elementType = JSData::token::tokenArray;
  }
  for (int i = 1; i < this->size(); i ++) {
    const Expression currentE = (*this)[i];
    if (createTable) {
      out << "<tr><td class='cellCalculatorInput'>";
      if (!this->owner->flagHideLHS) {
        if (i < (*startingExpression).size()) {
          format->flagDontCollalpseProductsByUnits = true;
          currentInput = HtmlRoutines::getMathNoDisplay((*startingExpression)[i].toString(format));
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
      out << "</td><td class='cellCalculatorResult'>";
      if ((*this)[i].isOfType<std::string>() && isFinal) {
        currentOutput = StringRoutines::Conversions::stringToCalculatorDisplay(
          currentE.getValue<std::string>()
        );
      } else if (currentE.requiresNoMathTags() && isFinal) {
        format->flagDontCollalpseProductsByUnits = false;
        currentOutput = currentE.toString(format);
      } else {
        format->flagDontCollalpseProductsByUnits = false;
        currentOutput = HtmlRoutines::getMathNoDisplay(currentE.toString(format), Expression::maximumCharactersInLatexPrintout);
      }
      currentOutput += currentE.toStringAllSlidersInExpression();
      if (outputJS != nullptr) {
        (*outputJS)["output"][i - 1] = currentOutput;
      }
      out << currentOutput;
      out << "</td></tr>";
    } else {
      bool addLatexDelimiter = !currentE.isOfType<JSData>();
      std::stringstream outWithDelimiter;
      if (createSingleTable) {
        out << "<tr><td>\n";
        addLatexDelimiter = !currentE.hasType<Plot>() && !currentE.isOfType<JSData>();
        if (addLatexDelimiter) {
          outWithDelimiter << "\\(";
        }
      }
      outWithDelimiter << currentE.toString(format);
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
    if (format != nullptr) {
      format->flagExpressionIsFinal = isFinal;
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

bool Expression::toStringPlus(const Expression& input, std::stringstream& out, FormatExpressions* format) {
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
    leftString = "\\left(" + left.toString(format) + "\\right)";
  } else {
    leftString = left.toString(format);
  }
  out << leftString;
  bool allowNewLine = false;
  if (format != nullptr) {
    allowNewLine = format->flagExpressionNewLineAllowed;
  }
  bool useFrac = input.owner->flagUseFracInRationalLaTeX;
  if (allowNewLine && !useFrac && leftString.size() > static_cast<unsigned>(FormatExpressions::ExpressionLineBreak)) {
    out << "\\\\\n";
  }
  std::string rightString = right.toString(format);
  if (right.needsParenthesisForAddition()) {
    rightString = "\\left(" + right.toString(format) + "\\right)";
  }
  if (rightString.size() > 0) {
    if (rightString[0] != '-') {
      out << "+";
    }
  }
  out << rightString;
  return true;
}

bool Expression::toStringDirectSum(const Expression& input, std::stringstream& out, FormatExpressions* format) {
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
    leftString = "\\left(" + left.toString(format) + "\\right)";
  } else {
    leftString = left.toString(format);
  }
  out << leftString;
  bool allowNewLine = false;
  if (format != nullptr) {
    allowNewLine = format->flagExpressionNewLineAllowed;
  }
  bool useFrac = input.owner->flagUseFracInRationalLaTeX;
  if (allowNewLine && !useFrac && leftString.size() > static_cast<unsigned>(FormatExpressions::ExpressionLineBreak)) {
    out << "\\\\\n";
  }
  std::string rightString = right.needsParenthesisForAddition() ?
  ("\\left(" + right.toString(format) + "\\right)") : right.toString(format);
  if (rightString.size() > 0) {
    if (rightString[0] != '-') {
      out << "\\oplus ";
    }
  }
  out << rightString;
  return true;
}

bool Expression::toStringSequence(
  const Expression& input, std::stringstream& out, FormatExpressions* format
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
  if (format != nullptr) {
    allowNewLine = format->flagExpressionNewLineAllowed;
  }
  for (int i = 1; i < input.size(); i ++) {
    std::string currentChildString = input[i].toString(format);
    out << currentChildString;
    charCounter += currentChildString.size();
    if (i != input.children.size - 1) {
      out << ", ";
      if (format != nullptr) {
        if (allowNewLine && charCounter > 50) {
          if (format->flagUseLatex) {
            out << "\\\\\n";
          }
          if (format->flagUseHTML) {
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

bool Expression::toStringMatrix(const Expression& input, std::stringstream& out, FormatExpressions* format) {
  if (!input.isMatrix()) {
    return false;
  }
  if (format->flagUseLatex && !format->flagUsePmatrix) {
    out << "\\left(";
  }
  if (!format->flagUsePmatrix) {
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
      out << input[i][j].toString(format);
      if (j != input[i].size() - 1) {
        out << " & ";
      }
    }
    if (i != input.size() - 1) {
      out << "\\\\ \n";
    }
  }
  if (format->flagUsePmatrix) {
    out << " \\end{pmatrix}";
  } else {
    out << " \\end{array}";
  }
  if (format->flagUseLatex && !format->flagUsePmatrix) {
    out << "\\right)";
  }
  return true;
}

bool Expression::toStringDefine(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opDefine(), 3)) {
    return false;
  }
  std::string firstE  = input[1].toString(format);
  std::string secondE = input[2].toString(format);
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
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (
    !input.owner->flagUseLnAbsInsteadOfLogForIntegrationNotation ||
    !input.startsWith(input.owner->opLog(), 2)
  ) {
    return false;
  }
  std::string theArg = input[1].toString(format);
  if (!StringRoutines::stringBeginsWith(theArg, "\\left|")) {
    out << "\\ln \\left|" << theArg << "\\right|";
  } else {
    out << "\\ln " << theArg;
  }
  return true;
}

bool Expression::toStringDifferential(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opDifferential())) {
    return false;
  }
  if (input.size() == 2) {
    return Expression::toStringDifferential2(input, out, format);
  }
  return Expression::toStringDifferential3(input, out, format);
}

bool Expression::toStringDifferential3(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
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
  std::string coefficient = input[2].toString(format);
  if (coefficient == "1") {
    needsParen = false;
    coefficient = "";
  }
  if (needsParen) {
    out << "\\left(";
  }
  out << coefficient;
  if (needsParen) {
    out << "\\right)";
  }
  out << " {\\text{d}} ";
  if (rightNeedsParen) {
    out << "\\left(";
  }
  out << input[1].toString(format);
  if (rightNeedsParen) {
    out << "\\right)";
  }
  return true;
}

bool Expression::toStringDifferentiate(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opDifferentiate(), 3)) {
    return false;
  }
  out << "\\frac{\\text{d} ";
  if (input[2].needsParenthesisForMultiplication()) {
    out << "\\left(" << input[2].toString(format)
    << "\\right)";
  } else {
    out << input[2].toString(format);
  }
  out << "}{\\text{d} "
  << input[1].toString(format) << "}";
  return true;
}

bool Expression::toStringDifferential2(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opDifferential(), 2)) {
    return false;
  }
  bool needsParen = (!input[1].isAtom()) && (!input[1].isBuiltInType());
  out << "{\\text{d}}{}";
  if (needsParen) {
    out << "\\left(";
  }
  out << input[1].toString(format);
  if (needsParen) {
    out << "\\right)";
  }
  return true;
}

bool Expression::toStringFactorial(const Expression& input, std::stringstream& out, FormatExpressions* format) {
  if (!input.startsWith(input.owner->opFactorial(), 2)) {
    return false;
  }
  if (input[1].needsParenthesisForBaseOfExponent()) {
    out << "\\left(" << input[1].toString(format) << "\\right) !";
  } else {
    out << input[1].toString(format) << "!";
  }
  return true;
}

bool Expression::toStringSqrt(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opSqrt())) {
    return false;
  }
  if (input.size() == 2) {
    return Expression::toStringSqrt2(input, out, format);
  }
  return Expression::toStringSqrt3(input, out, format);
}

bool Expression::toStringSqrt2(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opSqrt(), 2)) {
    return false;
  }
  out << "\\sqrt{" << input[1].toString(format) << "}";
  return true;
}

bool Expression::toStringSqrt3(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opSqrt(), 3)) {
    return false;
  }
  int power = 0;
  bool hasPowerTwo = input[1].isSmallInteger(&power);
  if (hasPowerTwo) {
    hasPowerTwo = (power == 2);
  }
  if (hasPowerTwo) {
    out << "\\sqrt{" << input[2].toString(format) << "}";
  } else {
    out << "\\sqrt[" << input[1].toString(format)
    << "]{" << input[2].toString(format) << "}";
  }
  return true;
}

bool Expression::toStringGreaterThan(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.isListStartingWithAtom(input.owner->opGreaterThan())) {
    return false;
  }
  out << input[1].toString(format) << "&gt;" << input[2].toString(format);
  return true;
}

bool Expression::toStringLessThanOrEqualTo(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.isListStartingWithAtom(input.owner->opLessThanOrEqualTo())) {
    return false;
  }
  out << input[1].toString(format) << "\\leq " << input[2].toString(format);
  return true;
}

bool Expression::toStringLessThan(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.isListStartingWithAtom(input.owner->opLessThan())) {
    return false;
  }
  out << input[1].toString(format) << "&lt;" << input[2].toString(format);
  return true;
}

bool Expression::toStringLimitProcess(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.isListStartingWithAtom(input.owner->opLimitProcess())) {
    return false;
  }
  out << input[1].toString(format) << " \\to " << input[2].toString(format);
  return true;
}

bool Expression::toStringGreaterThanOrEqualTo(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.isListStartingWithAtom(input.owner->opGreaterThanOrEqualTo())) {
    return false;
  }
  out << input[1].toString(format) << "\\geq " << input[2].toString(format);
  return true;
}

bool Expression::toStringError(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opError(), 2)) {
    return false;
  }
  input.checkInitialization();
  out << "\\text{Error:~" << input[1].toString(format) << "}";
  return true;
}

bool Expression::toStringSumOrIntegral(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (
    !input.startsWith(input.owner->opSum()) &&
    !input.startsWith(input.owner->opIntegral())
  ) {
    return false;
  }
  std::string opString = input[0].toString(format);
  out << opString << " ";
  int firstIndex = 2;
  if (input.size() >= 2) {
    if (input[1].startsWith(input.owner->opLimitBoundary(), 3)) {
      out
      << "_{" << input[1][1].toString(format) << "}"
      << "^{" << input[1][2].toString(format) << "}";
    } else if (input[1].isOperationGiven(input.owner->opIndefiniteIndicator())) {
      firstIndex = 2;
    } else {
      firstIndex = 1;
    }
  }
  if (input.size() <= firstIndex + 1) {
    if (input.size() == firstIndex + 1) {
      out << input[firstIndex].toString(format);
    }
  } else {
    out << "(";
    for (int i = firstIndex; i < input.size(); i ++) {
      out << input[i].toString(format);
      if (i != input.size() - 1) {
        out << ", ";
      }
    }
    out << ")";
  }
  return true;
}

bool Expression::toStringLimit(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opLimit(), 3)) {
    return false;
  }
  out << "\\lim_{";
  if (!input[1].isSequenceNElements()) {
    out << input[1].toString(format);
  } else {
    for (int i = 1; i < input[1].size(); i ++) {
      out << input[1][i].toString(format);
      if (i != input[1].size() - 1) {
        out << ", ";
      }
    }
  }
  out << "}" << input[2].toString(format);
  return true;
}

bool Expression::toStringUnion(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.isListStartingWithAtom(input.owner->opUnion()) || input.size() != 3) {
    return false;
  }
  if (input[1].startsWith(input.owner->opIntersection())) {
    out << "\\left(" << input[1].toString(format) <<  "\\right)"
    << "\\cup " << input[2].toString(format);
  } else {
    out << input[1].toString(format) << "\\cup " << input[2].toString(format);
  }
  return true;
}

bool Expression::toStringUnionNoRepetition(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.isListStartingWithAtom(input.owner->opUnionNoRepetition())) {
    return false;
  }
  out << input[1].toString(format) << "\\sqcup " << input[2].toString(format);
  return true;
}

bool Expression::toStringIntersection(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.isListStartingWithAtom(input.owner->opIntersection()) || input.size() != 3) {
    return false;
  }
  if (input[1].startsWith(input.owner->opUnion())) {
    out << "\\left(" << input[1].toString(format) << "\\right)"
    << "\\cap " << input[2].toString(format);
  } else {
    out << input[1].toString(format) << "\\cap " << input[2].toString(format);
  }
  return true;
}

std::string Expression::toStringWithStartingExpression(
  FormatExpressions* format,
  Expression* startingExpression,
  std::stringstream& out,
  JSData* outputJS
) const {
  std::stringstream outTrue;
  std::string input, output;
  bool isFinal = true;
  MemorySaving<FormatExpressions> tempFormat;
  if (format == nullptr) {
    format = &tempFormat.getElement();
    format->flagExpressionIsFinal = true;
  }
  isFinal = format->flagExpressionIsFinal;
  outTrue << "<table class = 'tableCalculatorOutput'>";
  outTrue << "<tr><th>Input</th><th>Result</th></tr>";
  if (this->isListStartingWithAtom(this->owner->opCommandSequence())) {
    outTrue << out.str();
  } else {
    if (format != nullptr) {
      format->flagDontCollalpseProductsByUnits = true;
    }
    input = HtmlRoutines::getMathNoDisplay(startingExpression->toString(format), Expression::maximumCharactersInLatexPrintout);
    if (format != nullptr) {
      format->flagDontCollalpseProductsByUnits = false;
    }
    outTrue << "<tr><td class = 'cellCalculatorInput'>" << input << "</td>";
    if ((
        this->isOfType<std::string>() ||
        this->isOfType<Plot>() ||
        this->isOfType<SemisimpleSubalgebras>() ||
        this->isOfType<WeylGroupData>()
      ) && isFinal
    ) {
      output = out.str();
    } else {
      output = HtmlRoutines::getMathNoDisplay(out.str(), Expression::maximumCharactersInLatexPrintout);
    }
    outTrue << "<td class = 'cellCalculatorResult'>" << output << "</td></tr>";
    if (outputJS != nullptr) {
      (*outputJS)["input"] = input;
      (*outputJS)["output"] = output;
    }
  }
  outTrue << "</table>";
  return outTrue.str();
}

bool Expression::toStringMinus3(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opMinus(), 3)) {
    return false;
  }
  if (input.children.size != 3) {
    global.fatal
    << "The minus function expects 1 or 2 arguments, "
    << "instead there are " << input.children.size - 1
    << ". " << global.fatal;
  }
  out << input[1].toString(format) << "-";
  if (input[2].startsWith(input.owner->opPlus()) || input[2].startsWith(input.owner->opMinus())) {
    out << "\\left(" << input[2].toString(format) << "\\right)";
  } else {
    out << input[2].toString(format);
  }
  return true;
}

bool Expression::toStringMinus(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opMinus())) {
    return false;
  }
  if (input.size() == 2) {
    return Expression::toStringMinus2(input, out, format);
  }
  return Expression::toStringMinus3(input, out, format);
}

bool Expression::toStringMinus2(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opMinus(), 2)) {
    return false;
  }
  if (
    input[1].startsWith(input.owner->opPlus()) ||
    input[1].startsWith(input.owner->opMinus())
  ) {
    out << "-\\left(" << input[1].toString(format) << "\\right)";
  } else {
    out << "-" << input[1].toString(format);
  }
  return true;
}

bool Expression::toStringOr(
  const Expression &input, std::stringstream& out, FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opOr(), 3)) {
    return false;
  }
  if (!input[1].isSequenceNElements()) {
    out << "\\left(" << input[1] << "\\right)";
  } else {
    out << input[1];
  }
  out << " ~or~ ";
  if (!input[2].isSequenceNElements()) {
    out << "\\left(" << input[2] << "\\right)";
  } else {
    out << input[2];
  }
  return true;
}

bool Expression::toStringIntervalOpen(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opIntervalOpen(), 3)) {
    return false;
  }
  if (!input.owner->flagUseBracketsForIntervals) {
    out << "IntervalOpen{}";
  }
  out << "\\left(" << input[1].toString(format) << ", "
  << input[2].toString(format) << "\\right)";
  return true;
}

bool Expression::toStringIntervalLeftClosed(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (input.startsWith(input.owner->opIntervalLeftClosed(), 2)) {
    if (input[1].isSequenceNElements(2)) {
      out << "\\left[" << input[1][1] << ", " << input[1][2] << "\\right)";
    } else {
      out << input[0].toString(format) << "{}" << input[1].toString(format);
    }
    return true;
  }
  if (input.startsWith(input.owner->opIntervalLeftClosed(), 3)) {
    out << "\\left[" << input[1].toString(format) << ", "
    << input[2].toString(format) << "\\right)";
    return true;
  }
  return false;
}

bool Expression::toStringIntervalRightClosed(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (
    input.startsWith(input.owner->opIntervalRightClosed(), 3)
  ) {
    out << "\\left(" << input[1].toString(format) << ", "
    << input[2].toString(format) << "\\right]";
    return true;
  }
  if (input.startsWith(input.owner->opIntervalRightClosed(), 2)) {
    if (input[1].isSequenceNElements(2)) {
      out << "\\left(" << input[1][1] << ", " << input[1][2] << "\\right]";
    } else {
      out << input[0].toString(format) << "{}" << input[1].toString(format);
    }
    return true;
  }
  return false;
}

bool Expression::toStringIntervalClosed(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (input.startsWith(input.owner->opIntervalClosed(), 2)) {
    if (input[1].isSequenceNElements(2)) {
      out << "\\left[" << input[1][1] << ", " << input[1][2] << "\\right]";
    } else {
      out << input[0].toString(format) << "{}" << input[1].toString(format);
    }
    return true;
  }
  if (input.startsWith(input.owner->opIntervalClosed(), 3)) {
    out << "\\left[" << input[1].toString(format) << ", "
    << input[2].toString(format) << "\\right]";
    return true;
  }
  return false;
}

bool Expression::toStringQuote(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opQuote(), 2)) {
    return false;
  }
  std::string content;
  if (input[1].isOperation(&content)) {
    out << "\"" << content << "\"";
  } else {
    out << "(Corrupt string)";
  }
  return true;
}

bool Expression::toStringLogBase(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opLogBase(), 3)) {
    return false;
  }
  out << "\\log_{" << input[1].toString(format) << "}"
  << "\\left(" << input[2].toString(format) << "\\right)";
  return true;
}

bool Expression::toStringIsDenotedBy(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.isListStartingWithAtom(input.owner->opIsDenotedBy())) {
    return false;
  }
  out << input[1].toString(format) << "=:" << input[2].toString(format);
  return true;
}

bool Expression::toStringDefineConditional(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opDefineConditional(), 4)) {
    return false;
  }
  out << input[1].toString(format) << " :if \\left("
  << input[2].toString(format) << "\\right)="
  << input[3].toString(format);
  return true;
}

bool Expression::toStringTensor(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opTensor(), 3)) {
    return false;
  }
  input.toStringOpMultiplicative(out, "\\otimes", format);
  return true;
}

bool Expression::toStringIn(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opIn(), 3)) {
    return false;
  }
  out << input[1].toString(format) << "\\in "
  << input[2].toString(format);
  return true;
}

bool Expression::toStringAnd(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opAnd(), 3)) {
    return false;
  }
  out << "\\left(" << input[1] << "\\right)" << " and "
  << "\\left(" << input[2] << "\\right)" ;
  return true;
}

bool Expression::toStringBinom(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opBinom(), 3)) {
    return false;
  }
  out << "\\binom{" << input[1].toString(format) << "}{ "
  << input[2].toString(format) << "}";
  return true;
}

bool Expression::toStringUnderscore(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opUnderscore())) {
    return false;
  }
  out << "{" << input[1].toString(format) << "}_{"
  << input[2].toString(format) << "}";
  return true;
}

bool Expression::toStringSetMinus(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opSetMinus(), 3)) {
    return false;
  }
  out << input[1].toString(format) << "\\setminus "
  << input[2].toString(format);
  return true;
}

bool Expression::toStringLimitBoundary(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opLimitBoundary(), 3)) {
    return false;
  }
  out << "\\limits_{" << input[1].toString(format)
  << "}^{"
  << input[2].toString(format)
  << "}";
  return true;
}

bool Expression::toStringCrossProduct(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opCrossProduct())) {
    return false;
  }
  input.toStringOpMultiplicative(out, "\\times", format);
  return true;
}

bool Expression::toStringAbsoluteValue(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opAbsoluteValue(), 2)) {
    return false;
  }
  out << "\\left|" << input[1].toString(format) << "\\right|";
  return true;
}

bool Expression::toStringBind(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opBind(), 2)) {
    return false;
  }
  out << "{{" << input[1].toString(format) << "}}";
  return true;
}

bool Expression::toStringMod(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.isListStartingWithAtom(input.owner->opMod())) {
    return false;
  }
  if (input.size() == 2) {
    out << " mod " << input[1].toString(format);
    return true;
  }
  if (input.size() == 3) {
    out << input[1].toString(format) << " mod " << input[2].toString(format);
    return true;
  }
  return false;
}

bool Expression::toStringLieBracket(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.isListStartingWithAtom(input.owner->opLieBracket())) {
    return false;
  }
  out << "[" << input[1].toString(format) << "," << input[2].toString(format) << "]";
  return true;
}

bool Expression::toStringEqualEqual(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.isListStartingWithAtom(input.owner->opEqualEqual())) {
    return false;
  }
  out << input[1].toString(format) << "==" << input[2].toString(format);
  return true;
}

bool Expression::toStringEqualEqualEqual(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.isListStartingWithAtom(input.owner->opEqualEqualEqual())) {
    return false;
  }
  out << input[1].toString(format) << "===" << input[2].toString(format);
  return true;
}

bool Expression::toStringWithAtomHandler(
  std::stringstream& out, FormatExpressions* format
) const {
  MacroRegisterFunctionWithName("Expression::toStringWithAtomHandler");
  if (!this->isList()) {
    return false;
  }
  if (!(*this)[0].isAtom()) {
    return false;
  }
  int atom = (*this)[0].data;
  // The handlers are initialized in:
  // Calculator::initializeToStringHandlers
  if (this->owner->toStringHandlersAtoms.contains(atom)) {
    Expression::ToStringHandler handler =
    this->owner->toStringHandlersAtoms.getValueNoFail(atom);
    return handler(*this, out, format);
  }
  return false;
}

bool Expression::toStringWithCompositeHandler(
  std::stringstream& out, FormatExpressions* format
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
  int atom = (*this)[0][0].data;
  if (this->owner->toStringHandlersComposite.contains(atom)) {
    Expression::ToStringHandler handler =
    this->owner->toStringHandlersComposite.getValueNoFail(atom);
    return handler(*this, out, format);
  }
  return false;
}

std::string Expression::toString(
  FormatExpressions* format,
  Expression* startingExpression,
  bool unfoldCommandEnclosures,
  JSData* outputJS
) const {
  MacroRegisterFunctionWithName("Expression::toString");
  MemorySaving<FormatExpressions> tempFormat;
  if (format == nullptr) {
    format = &tempFormat.getElement();
    format->flagUseQuotes = false;
  }
  if (this->owner != nullptr) {
    if (this->owner->recursionDepth + 1 > this->owner->maximumRecursionDepth) {
      return "(...)";
    }
  } else {
    return "(Error:NoOwner)";
  }
  RecursionDepthCounter recursionCounter;
  recursionCounter.initialize(&this->owner->recursionDepth);
  this->checkConsistency();
  if (startingExpression != nullptr && unfoldCommandEnclosures) {
    Expression newStart, newMe;
    if (
      CalculatorBasics::functionFlattenCommandEnclosuresOneLayer(*this->owner, *this, newMe) &&
      CalculatorBasics::functionFlattenCommandEnclosuresOneLayer(*this->owner, *startingExpression, newStart)
    ) {
      return newMe.toString(format, &newStart, false, outputJS);
    }
  }
  int notationIndex = owner->objectContainer.expressionWithNotation.getIndex(*this);
  if (notationIndex != - 1) {
    return owner->objectContainer.expressionNotation[notationIndex];
  }
  std::stringstream out;
  if (
    !this->isOfType<std::string>() &&
    !this->startsWith(this->owner->opCommandSequence())
  ) {
    if (startingExpression == nullptr) {
      format->flagUseQuotes = true;
    } else {
      format->flagUseQuotes = false;
    }
  }
  if (outputJS != nullptr) {
    outputJS->reset();
  }
  if (this->toStringData(out, format)) {
  } else if (this->toStringWithAtomHandler(out, format)) {
  } else if (this->toStringWithCompositeHandler(out, format)) {
  } else if (this->toStringEndStatement(out, startingExpression, outputJS, format)) {
  } else if (this->size() == 1) {
    out << (*this)[0].toString(format);
  } else if (this->toStringGeneral(out, format)) {
  } else { //<-not sure if this case is possible
    out << "(ProgrammingError:NotDocumented)";
  }
  if (startingExpression != nullptr) {
    return this->toStringWithStartingExpression(format, startingExpression, out, outputJS);
  }
  if (outputJS != nullptr) {
    if (outputJS->elementType == JSData::token::tokenUndefined) {
      (*outputJS) = out.str();
    }
  }
  return out.str();
}

std::string Expression::lispify() const {
  if (this->owner == nullptr) {
    return "Error: not initialized";
  }
  RecursionDepthCounter depthCounter(&this->owner->recursionDepth);
  if (this->owner->recursionDepth > this->owner->maximumRecursionDepth) {
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

bool Expression::isListStartingWithAtom(int operation) const {
  if (!this->isList()) {
    return false;
  }
  if (!(*this)[0].isAtom()) {
    return false;
  }
  if (operation == - 1) {
    return true;
  }
  return (*this)[0].data == operation;
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
  if (this->data != this->owner->opList()) {
    global.fatal << "List expressions "
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
  if (this->data < 0 || this->data >= this->owner->getOperations().size) {
    return false;
  }
  if (outputWhichAtom != nullptr) {
    *outputWhichAtom = this->owner->getOperations()[this->data];
  }
  return this->owner->atomsThatFreezeArguments.contains(this->owner->getOperations()[this->data]);
}

bool Expression::isPowerOfAtomWhoseExponentsAreInterpretedAsFunction() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (!this->startsWith(this->owner->opPower(), 3)) {
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
  if (this->data < 0 || this->data >= this->owner->getOperations().size) {
    return false;
  }
  if (outputWhichAtom != nullptr) {
    *outputWhichAtom = this->owner->getOperations()[this->data];
  }
  return this->owner->atomsWhoseExponentsAreInterpretedAsFunctions.contains(this->owner->getOperations()[this->data]);
}

bool Expression::isAtomNotInterpretedAsFunction(std::string* outputWhichAtom) const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->isList()) {
    return false;
  }
  if (this->data < 0 || this->data >= this->owner->getOperations().size) {
    return false;
  }
  if (outputWhichAtom != nullptr) {
    *outputWhichAtom = this->owner->getOperations()[this->data];
  }
  return this->owner->atomsNotInterpretedAsFunctions.contains(this->owner->getOperations()[this->data]);
}

bool Expression::isAtom() const {
  if (this->owner == nullptr) {
    return false;
  }
  return !this->isList();
}

bool Expression::isAtomUserDefined(std::string* whichAtom) const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->isList()) {
    return false;
  }
  if (this->data < this->owner->numberOfPredefinedAtoms) {
    return false;
  }
  return this->isOperation(whichAtom);
}

bool Expression::isOperation(std::string* outputWhichOperation) const {
  int index = - 1;
  if (!this->isOperation(index)) {
    return false;
  }
  if (outputWhichOperation != nullptr) {
    *outputWhichOperation = this->owner->operations.keys[index];
  }
  return true;
}

bool Expression::isOperation(int& outputWhichOperationIndex) const {
  if (!this->isAtom()) {
    return false;
  }
  if (this->data < 0 || this->data >= this->owner->operations.size()) {
    return false;
  }
  outputWhichOperationIndex = this->data;
  return true;
}

bool Expression::isOperationGiven(const std::string& desiredAtom) const {
  if (this->isList()) {
    return false;
  }
  if (this->owner == nullptr) {
    return false;
  }
  return this->data == this->owner->operations.getIndex(desiredAtom);
}

bool Expression::isOperationGiven(int desiredDataUseMinusOneForAny) const {
  if (this->isList()) {
    return false;
  }
  if (desiredDataUseMinusOneForAny == - 1) {
    return true;
  }
  return this->data == desiredDataUseMinusOneForAny;
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
  if (this->data < 0 || this->data >= this->owner->getOperations().size) {
    return false;
  }
  if (this->data >= this->owner->numberOfPredefinedAtoms) {
    return false;
  }
  if (outputWhichOperation != nullptr) {
    *outputWhichOperation = this->owner->getOperations()[this->data];
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
  if (this->data < 0 || this->data >= this->owner->getOperations().size) {
    return false;
  }
  if (outputWhichOperation != nullptr) {
    *outputWhichOperation = this->owner->getOperations()[this->data];
  }
  return !this->owner->atomsNotAllowingChainRule.contains(this->owner->getOperations()[this->data]);
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
  if (this->data < 0 || this->data >= theBoss.operations.size()) {
    global.fatal << "Corrupted atom in Expression::isCacheableExpression. " << global.fatal;
  }
  return theBoss.atomsThatMustNotBeCached.contains(theBoss.operations.keys[this->data]);
}

bool Expression::isBuiltInScalar() const {
  return this->isOfType<Rational>() || this->isOfType<Polynomial<Rational> >() ||
  this->isOfType<RationalFraction<Rational> >() || this->isOfType<double>() || this->isOfType<AlgebraicNumber>();
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
  std::string typeName;
  if (!this->isBuiltInType(&typeName)) {
    return false;
  }
  if (outputWhichType != nullptr) {
    *outputWhichType = this->owner->getOperations().getIndex(typeName);
  }
  return true;
}

bool Expression::makeProduct(Calculator& owner, const Expression& left, const Expression& right) {
  return this->makeXOX(owner, owner.opTimes(), left, right);
}

bool Expression::makeProduct(Calculator& owner, const List<Expression>& multiplicands) {
  if (multiplicands.size == 0) {
    return this->assignValueOLD(1, owner);
  }
  return this->makeXOXOdotsOX(owner, owner.opTimes(), multiplicands);
}

bool Expression::makeSum(Calculator& owner, const List<Expression>& summands) {
  MacroRegisterFunctionWithName("Expression::makeSum");
  if (summands.size == 0) {
    return this->assignValueOLD(0, owner);
  }
  return this->makeXOXOdotsOX(owner, owner.opPlus(), summands);
}

bool Expression::makeOXdotsX(Calculator& owner, int operation, const List<Expression>& theOpands) {
  MacroRegisterFunctionWithName("Expression::makeOXdotsX");
  if (theOpands.size == 0) {
    global.fatal << "zero opands not allowed at this point. " << global.fatal;
  }
  if (theOpands.size == 1) {
    *this = theOpands[0];
    return true;
  }
  *this = *theOpands.lastObject();
  for (int i = theOpands.size - 2; i >= 0; i --) {
    this->makeXOX(owner, operation, theOpands[i], *this);
  }
  this->checkConsistencyRecursively();
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
  argumentE.assignValueOLD(argument, owner);
  return this->makeSqrt(owner, argumentE, radicalSuperIndex);
}

bool Expression::makeSqrt(Calculator& owner, const Expression& argument, const Rational& radicalSuperIndex) {
  MacroRegisterFunctionWithName("Expression::makeSqrt");
  this->reset(owner, 3);
  Expression radicalIndexE;
  radicalIndexE.assignValueOLD(radicalSuperIndex, owner);
  this->addChildAtomOnTop(owner.opSqrt());
  this->addChildOnTop(radicalIndexE);
  return this->addChildOnTop(argument);
}

bool Expression::makeXOX(Calculator& owner, int theOp, const Expression& left, const Expression& right) {
  MacroRegisterFunctionWithName("Expression::makeXOX");
  if (&left == this || &right == this) {
    Expression leftCopy = left;
    Expression rightCopy = right;
    return this->makeXOX(owner, theOp, leftCopy, rightCopy);
  }
  if (right.owner == nullptr && left.owner == nullptr) {
    global.fatal << "Cannot build an expression from two non-initialized expressions. " << global.fatal;
  }
  if (right.owner == nullptr) {
    Expression rightCopy;
    rightCopy.assignValueOLD(right.data, *left.owner);
    return this->makeXOX(owner, theOp, left, rightCopy);
  }
 if (left.owner == nullptr) {
    Expression leftCopy;
    leftCopy.assignValueOLD(left.data, *right.owner);
    return this->makeXOX(owner, theOp, leftCopy, right);
  }
  left.checkInitialization();
  right.checkInitialization();
  this->reset(owner, 3);
  this->data = owner.opList();
  this->addChildAtomOnTop(theOp);
  this->addChildOnTop(left);
  return this->addChildOnTop(right);
}

bool Expression::makeOX(Calculator& owner, int operation, const Expression& opArgument) {
  if (&opArgument == this) {
    Expression copyE = opArgument;
    return this->makeOX(owner, operation, copyE);
  }
  this->reset(owner);
  this->data = owner.opList();
  this->addChildAtomOnTop(operation);
  return this->addChildOnTop(opArgument);
}

bool Expression::sequencefy() {
  this->checkInitialization();
  if (this->isSequenceNElements()) {
    return true;
  }
  return this->makeOX(*this->owner, this->owner->opSequence(), *this);
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

std::string Expression::toUTF8String(FormatExpressions* format) const {
  MacroRegisterFunctionWithName("Expression::toUTF8String");
  if (this->owner == nullptr) {
    return this->toString(format);
  }
  std::stringstream out;
  Rational rational;
  if (this->isOfType<Rational>(&rational)) {
    FormatExpressions tempFormat;
    tempFormat.flagUseFrac = false;
    return rational.toString(&tempFormat);
  } else if (this->isOperationGiven(this->owner->opPi())) {
    return "\u03C0";
  } else if (this->startsWith(this->owner->opPlus(), 3)) {
    return (*this)[1].toUTF8String(format) + "+" + (*this)[2].toUTF8String(format);
  } else if (this->startsWith(this->owner->opTimes(), 3)) {
    std::string secondUTF8String = (*this)[2].toUTF8String(format);
    std::string secondString = (*this)[2].toString(format);
    if ((*this)[1].isOperationGiven(this->owner->opSqrt())) {
      out << "sqrt(" << secondUTF8String << ")";
    } else {
      std::string firstE = (*this)[1].toUTF8String(format);
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
          if (StringRoutines::stringBeginsWith(secondString, "\\frac")) {
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
      out << (*this)[i].toUTF8String(format);
      if (i < this->size() - 1) {
        out << ", ";
      }
    }
    out << ")";
    return out.str();
  }
  return this->toString(format);
}
