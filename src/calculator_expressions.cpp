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
  left.CheckInitialization();
  Expression result;
  result.MakeXOX(*left.owner, left.owner->opTimes(), left, right);
  return result;
}

Expression operator/(const Expression& left, const Expression& right) {
  left.CheckInitialization();
  Expression result;
  result.MakeXOX(*left.owner, left.owner->opDivide(), left, right);
  return result;
}

Expression operator+(const Expression& left, const Expression& right) {
  left.CheckInitialization();
  Expression result;
  result.MakeXOX(*left.owner, left.owner->opPlus(), left, right);
  return result;
}

Expression operator-(const Expression& left, const Expression& right) {
  left.CheckInitialization();
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
  this->CheckInitialization();
  return this->owner->opEllipticCurveElementsRational();
}

template < >
int Expression::getTypeOperation<ElementEllipticCurve<ElementZmodP> >() const {
  this->CheckInitialization();
  return this->owner->opEllipticCurveElementsZmodP();
}

template < >
int Expression::getTypeOperation<RationalFunction>() const {
  this->CheckInitialization();
  return this->owner->opRationalFunction();
}

template < >
int Expression::getTypeOperation<InputBox>() const {
  this->CheckInitialization();
  return this->owner->opUserInputTextBox();
}

template < >
int Expression::getTypeOperation<Rational>() const {
  this->CheckInitialization();
  return this->owner->opRational();
}

template < >
int Expression::getTypeOperation<ElementHyperoctahedralGroupR2>() const {
  this->CheckInitialization();
  return this->owner->opElementHyperOctahedral();
}

template < >
int Expression::getTypeOperation<GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> >() const {
  this->CheckInitialization();
  return this->owner->opHyperoctahedralGroupRep();
}

template < >
int Expression::getTypeOperation<std::string>() const {
  this->CheckInitialization();
  return this->owner->opString();
}

template < >
int Expression::getTypeOperation<AlgebraicNumber>() const {
  this->CheckInitialization();
  return this->owner->opAlgNumber();
}

template < >
int Expression::getTypeOperation<ElementZmodP>() const {
  this->CheckInitialization();
  return this->owner->opEltZmodP();
}

template < >
int Expression::getTypeOperation<ElementUniversalEnveloping<RationalFunction> >() const {
  this->CheckInitialization();
  return this->owner->opElementUEoverRF();
}

template < >
int Expression::getTypeOperation<Weight<Rational> >() const {
  this->CheckInitialization();
  return this->owner->opWeightLieAlg();
}

template < >
int Expression::getTypeOperation<Weight<Polynomial<Rational> > >() const {
  this->CheckInitialization();
  return this->owner->opWeightLieAlgPoly();
}

template < >
int Expression::getTypeOperation<MatrixTensor<Rational> >() const {
  this->CheckInitialization();
  return this->owner->opMatTensorRat();
}

template < >
int Expression::getTypeOperation<int>() const {
  this->CheckInitialization();
  return this->owner->opRational();
}

template < >
int Expression::getTypeOperation<LargeIntegerUnsigned>() const {
  this->CheckInitialization();
  return this->owner->opRational();
}

template < >
int Expression::getTypeOperation<double>() const {
  this->CheckInitialization();
  return this->owner->opDouble();
}

template < >
int Expression::getTypeOperation<Polynomial<Rational> >() const {
  this->CheckInitialization();
  return this->owner->opPolynomialRational();
}

template < >
int Expression::getTypeOperation<Polynomial<AlgebraicNumber> >() const {
  this->CheckInitialization();
  return this->owner->opPolynomialAlgebraicNumbers();
}

template < >
int Expression::getTypeOperation<Polynomial<ElementZmodP> >() const {
  this->CheckInitialization();
  return this->owner->opPolynomialModuloInteger();
}

template < >
int Expression::getTypeOperation<ElementWeylAlgebra<Rational> >() const {
  this->CheckInitialization();
  return this->owner->opElementWeylAlgebra();
}

template < >
int Expression::getTypeOperation<SemisimpleLieAlgebra*>() const {
  this->CheckInitialization();
  return this->owner->opSemisimpleLieAlgebrA();
}

template < >
int Expression::getTypeOperation<LittelmannPath>() const {
  this->CheckInitialization();
  return this->owner->opLittelmannPath();
}

template < >
int Expression::getTypeOperation<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >() const {
  this->CheckInitialization();
  return this->owner->opLRO();
}

template < >
int Expression::getTypeOperation<ElementTensorsGeneralizedVermas<RationalFunction> >() const {
  this->CheckInitialization();
  return this->owner->opElementTensorGVM();
}

template < >
int Expression::getTypeOperation<charSSAlgMod<Rational> >() const {
  this->CheckInitialization();
  return this->owner->opCharSSAlgMod();
}

template < >
int Expression::getTypeOperation<Plot>() const {
  this->CheckInitialization();
  return this->owner->opCalculusPlot();
}

template < >
int Expression::getTypeOperation<PlotObject>() const {
  this->CheckInitialization();
  return this->owner->opCalculusPlot();
}

template < >
int Expression::getTypeOperation<SemisimpleSubalgebras>() const {
  this->CheckInitialization();
  return this->owner->opSemisimpleSubalgebras();
}

template < >
int Expression::getTypeOperation<WeylGroupData>() const {
  this->CheckInitialization();
  return this->owner->opWeylGroup();
}

template < >
int Expression::getTypeOperation<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >() const {
  this->CheckInitialization();
  return this->owner->opWeylGroupRep();
}

template < >
int Expression::getTypeOperation<ElementWeylGroup>() const {
  this->CheckInitialization();
  return this->owner->opWeylGroupElement();
}

template < >
int Expression::getTypeOperation<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >() const {
  this->CheckInitialization();
  return this->owner->opWeylGroupVirtualRep();
}

// Expression::getTypeOperation specializations end.
// Expression::ConvertsInternally specializations follow.
template < >
bool Expression::ConvertsInternally(
  WithContext<ElementSemisimpleLieAlgebra<AlgebraicNumber> >* whichElement
) const {
  MacroRegisterFunctionWithName("Expression::ConvertsInternally");
  if (this->owner == nullptr) {
    return false;
  }
  ElementUniversalEnveloping<RationalFunction> element;
  if (!this->IsOfType<ElementUniversalEnveloping<RationalFunction> >(&element)) {
    return false;
  }
  ElementSemisimpleLieAlgebra<Rational> elementSemisimple;
  if (!element.GetLieAlgebraElementIfPossible(elementSemisimple)) {
    return false;
  }
  if (whichElement != nullptr) {
    this->GetContext(whichElement->context);
    whichElement->content = elementSemisimple;
  }
  return true;
}

//Expression::ConvertsToType specializations end.
//Expression::AddObjectReturnIndex specializations follow

template < >
int Expression::AddObjectReturnIndex(const
ElementEllipticCurve<Rational>
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.EllipticCurveElementsRational
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
ElementEllipticCurve<ElementZmodP>
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.EllipticCurveElementsZmodP
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
SemisimpleSubalgebras
& inputValue) const {
  this->CheckInitialization();
  inputValue.CheckInitialization();
  if (!this->owner->theObjectContainer.theSSSubalgebraS.Contains(inputValue.owner->theWeyl.theDynkinType)) {
    global.fatal << "Semisimple subalgebras must be allocated directly in the object container. " << global.fatal;
  }
  return this->owner->theObjectContainer.theSSSubalgebraS.getIndex(inputValue.owner->theWeyl.theDynkinType);
}

template < >
int Expression::AddObjectReturnIndex(const
ElementHyperoctahedralGroupR2
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.theElementsHyperOctGroup
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
InputBox
& inputValue) const {
  this->CheckInitialization();
  this->owner->theObjectContainer.theUserInputTextBoxesWithValues
  .SetKeyValue(inputValue.name, inputValue);
  return this->owner->theObjectContainer.theUserInputTextBoxesWithValues
  .getIndex(inputValue.name);
}

template < >
int Expression::AddObjectReturnIndex(const
std::string
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.theStrings
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational>
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.theHyperoctahedralReps
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
Rational
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.theRationals
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
double
& inputValue) const {
  this->CheckInitialization();
  if (std::isnan(inputValue)) {
    return 0;
  }
  return this->owner->theObjectContainer.theDoubles
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
RationalFunction
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.theRFs
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
MatrixTensor<Rational>
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.theMatTensorRats
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
ElementUniversalEnveloping<RationalFunction>
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.theUEs
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
Weight<Rational>
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.theWeights
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
Weight<Polynomial<Rational> >
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.theWeightsPoly
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
int
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.theRationals
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
LargeIntegerUnsigned
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.theRationals
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
Polynomial<Rational>
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.polynomialsRational
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
Polynomial<ElementZmodP>
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.polynomialsModular
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
Polynomial<AlgebraicNumber>
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.polynomialsAlgebraic
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
ElementWeylAlgebra<Rational>
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.theWeylAlgebraElements
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(
SemisimpleLieAlgebra* const
& inputValue) const {
  this->CheckInitialization();
  if (!this->owner->theObjectContainer.semisimpleLieAlgebras.Contains(inputValue->theWeyl.theDynkinType)) {
    global.fatal << "Semisimple Lie algebra must be allocated directly in the object container. " << global.fatal;
  }
  int index = this->owner->theObjectContainer.semisimpleLieAlgebras.getIndex(inputValue->theWeyl.theDynkinType);
  return index;
}

template < >
int Expression::AddObjectReturnIndex(const
AlgebraicNumber
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.theAlgebraicNumbers
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
ElementZmodP
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.theEltsModP
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
MonomialTensor<int, MathRoutines::IntUnsignIdentity>
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.theLittelmannOperators
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
ElementTensorsGeneralizedVermas<RationalFunction>
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.theTensorElts
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
charSSAlgMod<Rational>
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.theCharsSSLieAlgFD
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
LittelmannPath
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.theLSpaths
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
Plot
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.thePlots
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
PlotObject
& inputValue) const {
  this->CheckInitialization();
  Plot plotContainer;
  plotContainer += inputValue;
  return this->owner->theObjectContainer.thePlots
  .AddNoRepetitionOrReturnIndexFirst(plotContainer);
}

template < >
int Expression::AddObjectReturnIndex(const
WeylGroupData
& inputValue) const {
  this->CheckInitialization();
  this->owner->theObjectContainer.GetLieAlgebraCreateIfNotPresent(inputValue.theDynkinType);
  return this->owner->theObjectContainer.semisimpleLieAlgebras.getIndex(inputValue.theDynkinType);
}

template < >
int Expression::AddObjectReturnIndex(const
GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.theWeylGroupReps
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.theWeylGroupVirtualReps
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
ElementWeylGroup
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.theWeylGroupElements
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

//Expression::AddObjectReturnIndex specializations end

//start Expression::GetValueNonConst specializations.
template < >
Rational& Expression::GetValueNonConst() const {
  if (!this->IsOfType<Rational>()) {
    global.fatal << "This is a programming error: expression not of required type Rational. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theRationals.GetElement(this->GetLastChild().theData);
}

template < >
ElementEllipticCurve<Rational>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<ElementEllipticCurve<Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type Rational. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.EllipticCurveElementsRational.GetElement(this->GetLastChild().theData);
}

template < >
Polynomial<ElementZmodP> & Expression::GetValueNonConst() const {
  if (!this->IsOfType<Polynomial<ElementZmodP> >()) {
    global.fatal << "This is a programming error: expression not of required type Polynomial mod integer. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.polynomialsModular.GetElement(this->GetLastChild().theData);
}

template < >
ElementEllipticCurve<ElementZmodP>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<ElementEllipticCurve<ElementZmodP> >()) {
    global.fatal << "This is a programming error: expression not of required type Rational. The expression equals "
    << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.EllipticCurveElementsZmodP.GetElement(this->GetLastChild().theData);
}

template < >
ElementHyperoctahedralGroupR2& Expression::GetValueNonConst() const {
  if (!this->IsOfType<ElementHyperoctahedralGroupR2>()) {
    global.fatal << "This is a programming error: expression not of required type Rational. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theElementsHyperOctGroup.GetElement(this->GetLastChild().theData);
}

template < >
InputBox& Expression::GetValueNonConst() const {
  if (!this->IsOfType<InputBox>()) {
    global.fatal << "This is a programming error: expression not of required type Rational. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theUserInputTextBoxesWithValues.theValues[this->GetLastChild().theData];
}

template < >
GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> & Expression::GetValueNonConst() const {
  if (!this->IsOfType<GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type Rational. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theHyperoctahedralReps[this->GetLastChild().theData];
}

template < >
ElementZmodP& Expression::GetValueNonConst() const {
  if (!this->IsOfType<ElementZmodP>())
    global.fatal << "This is a programming error: expression not of required type ElementZmodP. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  return this->owner->theObjectContainer.theEltsModP.GetElement(this->GetLastChild().theData);
}

template < >
AlgebraicNumber& Expression::GetValueNonConst() const {
  if (!this->IsOfType<AlgebraicNumber>()) {
    global.fatal << "This is a programming error: expression not of required type AlgebraicNumber. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theAlgebraicNumbers.GetElement(this->GetLastChild().theData);
}

template < >
double& Expression::GetValueNonConst() const {
  if (!this->IsOfType<double>()) {
    global.fatal << "This is a programming error: expression not of required type double. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theDoubles.GetElement(this->GetLastChild().theData);
}

template < >
std::string& Expression::GetValueNonConst() const {
  if (!this->IsOfType<std::string>()) {
    global.fatal << "This is a programming error: expression not of required type std::string. "
    << "The expression equals " << this->toString() << ". Comments so far: "
    << this->owner->Comments.str() << global.fatal;
  }
  return this->owner->theObjectContainer.theStrings.GetElement(this->GetLastChild().theData);
}

template < >
RationalFunction& Expression::GetValueNonConst() const {
  if (!this->IsOfType<RationalFunction>()) {
    global.fatal << "This is a programming error: expression not of required type RationalFunctionOld. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theRFs.GetElement(this->GetLastChild().theData);
}

template < >
ElementUniversalEnveloping<RationalFunction>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<ElementUniversalEnveloping<RationalFunction> >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "ElementUniversalEnveloping_RationalFunctionOld. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theUEs.GetElement(this->GetLastChild().theData);
}

template < >
Polynomial<Rational>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<Polynomial<Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type Polynomial_Rational. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.polynomialsRational.GetElement(this->GetLastChild().theData);
}

template < >
Polynomial<AlgebraicNumber>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<Polynomial<AlgebraicNumber> >()) {
    global.fatal << "This is a programming error: expression not of required type Polynomial_AlgebraicNumber. "
    << "The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.polynomialsAlgebraic.GetElement(this->GetLastChild().theData);
}

template < >
ElementWeylAlgebra<Rational>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<ElementWeylAlgebra<Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "ElementWeylAlgebra_Rational. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theWeylAlgebraElements.GetElement(this->GetLastChild().theData);
}

template < >
LittelmannPath& Expression::GetValueNonConst() const {
  if (!this->IsOfType<LittelmannPath>()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "LittelmannPath. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theLSpaths.GetElement(this->GetLastChild().theData);
}

template < >
Weight<Polynomial<Rational> >& Expression::GetValueNonConst() const {
  if (!this->IsOfType<Weight<Polynomial<Rational> > >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "LittelmannPath. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theWeightsPoly.GetElement(this->GetLastChild().theData);
}

template < >
MonomialTensor<int, MathRoutines::IntUnsignIdentity>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "MonomialTensor. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theLittelmannOperators.GetElement(this->GetLastChild().theData);
}

template < >
ElementTensorsGeneralizedVermas<RationalFunction>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<ElementTensorsGeneralizedVermas<RationalFunction> >()) {
    global.fatal << "This is a programming error: expression not of "
    << "required type ElementTGVM_RationalFunctionOld. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theTensorElts.GetElement(this->GetLastChild().theData);
}

template < >
charSSAlgMod<Rational>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<charSSAlgMod<Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "charSSAlgMod_Rational. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theCharsSSLieAlgFD.GetElement(this->GetLastChild().theData);
}

template < >
SemisimpleLieAlgebra*& Expression::GetValueNonConst() const {
  if (!this->IsOfType<SemisimpleLieAlgebra*>()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "SemisimpleLieAlgebra. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.semisimpleLieAlgebraPointers[this->GetLastChild().theData];
}

template < >
MatrixTensor<Rational>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<MatrixTensor<Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "MatrixTensorRational. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theMatTensorRats.GetElement(this->GetLastChild().theData);
}

template < >
SemisimpleSubalgebras& Expression::GetValueNonConst() const {
  if (!this->IsOfType<SemisimpleSubalgebras>()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "SemisimpleSubalgebras. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theSSSubalgebraS.theValues[this->GetLastChild().theData];
}

template < >
Plot& Expression::GetValueNonConst() const {
  if (!this->IsOfType<Plot>()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "Plot. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.thePlots[this->GetLastChild().theData];
}

template < >
WeylGroupData& Expression::GetValueNonConst() const {
  if (!this->IsOfType<WeylGroupData>()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "WeylGroupData. The expression equals "
    << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.semisimpleLieAlgebras.theValues[
    this->GetLastChild().theData
  ].theWeyl;
}

template < >
GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "WeylGroupRepresentation_Rational. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theWeylGroupReps.GetElement(this->GetLastChild().theData);
}

template < >
ElementWeylGroup& Expression::GetValueNonConst() const {
  if (!this->IsOfType<ElementWeylGroup>()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "ElementWeylGroup. The expression equals " << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theWeylGroupElements.GetElement(this->GetLastChild().theData);
}

template < >
VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "WeylGroupVirtualRepresentation. The expression equals "
    << this->toString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theWeylGroupVirtualReps.GetElement(this->GetLastChild().theData);
}

bool Expression::IsMatrix(int* outputNumRows, int* outputNumCols) const {
  MacroRegisterFunctionWithName("Expression::IsMatrix");
  if (this->owner == nullptr) {
    return false;
  }
  if (this->size() < 1) {
    return false;
  }
  bool isGood = (*this)[0].StartsWith(this->owner->opMatriX());
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

//end Expression::GetValueNonConst specializations.
//start Expression::ConvertInternally specializations.
template< >
bool Expression::ConvertInternally<Polynomial<AlgebraicNumber> >(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_Polynomial_AlgebraicNumber");
  this->CheckInitialization();
  if (this->IsOfType<Rational>()) {
    Polynomial<AlgebraicNumber> resultP;
    resultP.MakeConst(this->GetValue<Rational>());
    return output.AssignValueWithContext(resultP, this->GetContext(), *this->owner);
  }
  if (this->IsOfType<AlgebraicNumber>()) {
    Polynomial<AlgebraicNumber> resultP;
    resultP.MakeConst(this->GetValue<AlgebraicNumber>());
    return output.AssignValueWithContext(resultP, this->GetContext(), *this->owner);
  }
  if (this->IsOfType<Polynomial<Rational> >()) {
    Polynomial<AlgebraicNumber> resultP;
    resultP = this->GetValue<Polynomial<Rational> >();
    return output.AssignValueWithContext(resultP, this->GetContext(), *this->owner);
  }
  if (this->IsOfType<Polynomial<AlgebraicNumber> >()) {
    output = *this;
    return true;
  }
  return false;
}

template< >
bool Expression::ConvertInternally<Polynomial<Rational> >(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_Polynomial_Rational");
  this->CheckInitialization();
  if (this->IsOfType<Rational>()) {
    Polynomial<Rational> resultP;
    resultP.MakeConst(this->GetValue<Rational>());
    return output.AssignValueWithContext(resultP, this->GetContext(), *this->owner);
  }
  if (this->IsOfType<Polynomial<Rational> >()) {
    output = *this;
    return true;
  }
  return false;
}

template< >
bool Expression::ConvertInternally<Polynomial<ElementZmodP> >(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_Polynomial_Rational");
  this->CheckInitialization();
  if (this->IsOfType<Polynomial<ElementZmodP> >()) {
    output = *this;
    return true;
  }
  return false;
}

template< >
bool Expression::ConvertInternally<RationalFunction>(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_RationalFunctionOld");
  this->CheckInitialization();
  if (this->IsOfType<Rational>()) {
    RationalFunction resultRF;
    resultRF.MakeConst(this->GetValue<Rational>());
    return output.AssignValueWithContext(
      resultRF, this->GetContext(), *this->owner
    );
  }
  if (this->IsOfType<Polynomial<Rational> >()) {
    RationalFunction resultRF;
    resultRF = this->GetValue<Polynomial<Rational> >();
    return output.AssignValueWithContext(
      resultRF, this->GetContext(), *this->owner
    );
  }
  if (this->IsOfType<RationalFunction>()) {
    output = *this;
    return true;
  }
  (*this->owner)
  << "<hr>ConvertToType_RationalFunctionOld: Failed to convert "
  << this->toString() << " to rational function. ";
  return false;
}

template< >
bool Expression::ConvertInternally<ElementWeylAlgebra<Rational> >(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_ElementWeylAlgebra");
  this->CheckInitialization();
  if (this->IsOfType<Rational>()) {
    ElementWeylAlgebra<Rational> resultEWA;
    resultEWA.MakeConst(this->GetValue<Rational>());
    return output.AssignValueWithContext(resultEWA, this->GetContext(), *this->owner);
  }
  if (this->IsOfType<Polynomial<Rational> >()) {
    ElementWeylAlgebra<Rational> resultEWA;
    resultEWA.AssignPolynomial(this->GetValue<Polynomial<Rational> >());
    return output.AssignValueWithContext(resultEWA, this->GetContext(), *this->owner);
  }
  if (this->IsOfType<ElementWeylAlgebra<Rational> >()) {
    output = *this;
    return true;
  }
  return false;
}

template< >
bool Expression::ConvertInternally<Weight<Polynomial<Rational> > >(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_Weight_Polynomial");
  this->CheckInitialization();
  if (this->IsOfType<Weight<Polynomial<Rational> > >()) {
    output = *this;
    return true;
  }
  return false;
}

template< >
bool Expression::ConvertInternally<ElementUniversalEnveloping<RationalFunction> >(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_RationalFunctionOld");
  this->CheckInitialization();
  if (!this->IsBuiltInTypE())
    return false;
  if (this->IsOfType<ElementUniversalEnveloping<RationalFunction> >()) {
    output = *this;
    return true;
  }
  SemisimpleLieAlgebra* theOwner = this->GetAmbientSSAlgebraNonConstUseWithCaution();
  if (theOwner == nullptr) {
    this->owner->Comments << "<hr>Failed to convert " << this->toString() << " (Lispified: " << this->toStringFull()
    << ") to element of universal enveloping -  failed to extract ambient Lie algebra. ";
    return false;
  }
  ElementUniversalEnveloping<RationalFunction> outputUE;
  if (this->IsOfType<Rational>()) {
    outputUE.MakeConst(this->GetValue<Rational>(), *theOwner);
    return output.AssignValueWithContext(outputUE, this->GetContext(), *this->owner);
  } else if (this->IsOfType<Polynomial<Rational> >()) {
    RationalFunction tempRF;
    tempRF = this->GetValue<Polynomial<Rational> >();
    outputUE.MakeConst(tempRF, *theOwner);
    return output.AssignValueWithContext(outputUE, this->GetContext(), *this->owner);
  } else if (this->IsOfType<RationalFunction>()) {
    outputUE.MakeConst(this->GetValue<RationalFunction>(), *theOwner);
    return output.AssignValueWithContext(outputUE, this->GetContext(), *this->owner);
  }
  return *this->owner << "<hr>Failed to convert " << this->toString()
  << " to element of universal enveloping -  I don't know how. ";
}

template< >
bool Expression::ConvertInternally<ElementTensorsGeneralizedVermas<RationalFunction> >(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_RationalFunctionOld");
  this->CheckInitialization();
  if (this->IsOfType<ElementTensorsGeneralizedVermas<RationalFunction> >()) {
    output = *this;
    return true;
  }
  return false;
}

template< >
bool Expression::ConvertInternally<Rational>(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_Rational");
  this->CheckInitialization();
  if (this->IsOfType<Rational>()) {
    output = *this;
    return true;
  }
  return false;
}

template< >
bool Expression::ConvertInternally<AlgebraicNumber>(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_AlgebraicNumber");
  this->CheckInitialization();
  if (this->IsOfType<AlgebraicNumber>()) {
    output = *this;
    return true;
  }
  Rational theRat;
  if (!this->IsOfType<Rational>(&theRat)) {
    return false;
  }
  AlgebraicNumber outRat = theRat;
  return output.AssignValue(outRat, *this->owner);
}

template< >
bool Expression::ConvertInternally<double>(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_double");
  this->CheckInitialization();
  if (this->IsOfType<double>()) {
    output = *this;
    return true;
  }
  double outputDouble = 0;
  if (this->EvaluatesToDouble(&outputDouble)) {
    return output.AssignValue(outputDouble, *this->owner);
  }
  return false;
}
//end Expression::ConvertToType specializations.

bool Expression::CheckConsistencyRecursively() const {
  MacroRegisterFunctionWithName("Expression::CheckConsistencyRecursively");
  this->CheckConsistency();
  for (int i = 0; i < this->children.size; i ++) {
    (*this)[i].CheckConsistency();
  }
  return true;
}

bool Expression::CheckConsistency() const {
  MacroRegisterFunctionWithName("Expression::CheckConsistency");
  // warning: do not use toString method from here: toString itself calls CheckConosistency,
  // so that causes an "infinite" recursion call cycle,
  // i.e., stack overflow.
  if (this->flagDeallocated) {
    global.fatal << "Use after free of Expression. " << global.fatal;
  }
  this->CheckInitialization();
  if (this->children.size == 0) {
    return false;
  }
  if (this->IsBuiltInTypE()) {
    if (this->children.size != 3) {
      global.fatal << "This is a programming error. At the moment of writing, an expression of built-in type must have 3 "
      << "children: type, context, and index in Calculator. The expression is " << this->toStringFull()
      << global.fatal;
    }
    const Expression& mustBeTheContext = (*this)[1];
    if (!mustBeTheContext.StartsWith(this->owner->opContext())) {
      global.fatal << "This is a programming error. At the moment of writing, "
      << "the second child of a built-in type must be a context. It is instead "
      << mustBeTheContext.toStringFull() << global.fatal;
    }
    for (int i = 1; i < mustBeTheContext.children.size; i ++) {
      bool isGood = false;
      const Expression& currentE = mustBeTheContext[i];
      if (currentE.StartsWith(this->owner->opPolynomialVariables())) {
        isGood = true;
      }
      if (currentE.StartsWith(this->owner->opSemisimpleLieAlgebrA())) {
        isGood = true;
      }
      if (currentE.StartsWith(this->owner->opWeylAlgebraVariables())) {
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
  this->CheckInitialization();
  int childIndex = this->children[n];
  if (childIndex < 0) {
    global.fatal << "<hr>This is a programming error: the child of position "
    << n << " out of " << this->children.size - 1
    << " is not contained in the expression container. " << global.fatal;
  }
  return this->owner->theExpressionContainer[childIndex];
}

Expression Expression::zero() {
  Expression result;
  return result;
}

bool Expression::AddChildRationalOnTop(const Rational& inputRat) {
  this->CheckInitialization();
  Expression ratE;
  ratE.AssignValue(inputRat, *this->owner);
  return this->AddChildOnTop(ratE);
}

bool Expression::AddChildOnTop(const Expression& inputChild) {
  this->CheckInitialization();
  this->children.addOnTop(
    this->owner->theExpressionContainer.AddNoRepetitionOrReturnIndexFirst(inputChild)
  );
  return true;
}

bool Expression::SetChildAtomValue(int childIndex, int TheAtomValue) {
  this->CheckInitialization();
  Expression tempE;
  tempE.MakeAtom(TheAtomValue, *this->owner);
  this->children.SetObjectAtIndex(
    childIndex,
    this->owner->theExpressionContainer.AddNoRepetitionOrReturnIndexFirst(tempE)
  );
  return true;
}

bool Expression::SetChildAtomValue(int childIndex, const std::string& theAtom) {
  this->CheckInitialization();
  Expression tempE;
  tempE.MakeAtom(theAtom, *this->owner);
  this->children.SetObjectAtIndex(
    childIndex, this->owner->theExpressionContainer.AddNoRepetitionOrReturnIndexFirst(tempE)
  );
  return true;
}

bool Expression::SetChilD(int childIndexInMe, const Expression& inputChild) {
  this->CheckInitialization();
  int theIndexOfTheExpression = this->owner->theExpressionContainer.AddNoRepetitionOrReturnIndexFirst(inputChild);
  this->children.SetObjectAtIndex(childIndexInMe, theIndexOfTheExpression);
  return true;
}

bool Expression::SetChilD(int childIndexInMe, int childIndexInBoss) {
  this->CheckInitialization();
  this->children.SetObjectAtIndex(childIndexInMe, childIndexInBoss);
  if (this->children[childIndexInMe] != childIndexInBoss) {
    global.fatal << "This shouldn't happen." << global.fatal;
  }
  return true;
}

bool Expression::AddChildAtomOnTop(int theOp) {
  this->CheckInitialization();
  Expression tempE;
  tempE.MakeAtom(theOp, *this->owner);
  return this->AddChildOnTop(tempE);
}

bool Expression::AddChildAtomOnTop(const std::string& theOperationString) {
  this->CheckInitialization();
  return this->AddChildAtomOnTop(
    this->owner->AddOperationNoRepetitionOrReturnIndexFirst(theOperationString)
  );
}

bool Expression::StartsWithBuiltInAtom() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->IsAtom() || this->children.size < 1) {
    return false;
  }
  return (*this)[0].IsBuiltInAtom();
}

bool Expression::StartsWithArithmeticOperation() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->IsAtom() || this->children.size < 1) {
    return false;
  }
  return (*this)[0].IsArithmeticOperation();
}

bool Expression::StartsWithFunctionWithComplexRange() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->IsAtom() || this->children.size < 1) {
    return false;
  }
  return (*this)[0].IsKnownFunctionWithComplexRange();
}

bool Expression::StartsWithGivenOperation(const std::string& theOperation, int desiredChildren) const {
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
  return (*this)[0].IsOperationGiven(theOpIndex);
}

bool Expression::StartsWith(int theOp, int N) const {
  if (N != - 1) {
    if (this->children.size != N) {
      return false;
    }
  } else {
    if (this->children.size == 0) {
      return false;
    }
  }
  if (!(*this)[0].IsAtom()) {
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
  if (this->IsBuiltInTypE() || this->IsAtom()) {
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

bool Expression::GetBoundVariables(HashedList<Expression>& outputAccumulateBoundVariables) const {
  MacroRegisterFunctionWithName("Expression::GetBoundVariables");
  if (this->owner == nullptr) {
    return true;
  }
  RecursionDepthCounter(&this->owner->RecursionDeptH);
  if (this->owner->RecursionDepthExceededHandleRoughly("In Expression::GetFreeVariables:")) {
    return false;
  }
  if (this->IsBuiltInTypE()) { //<- this may need to be rewritten as some built in types will store free variables in their context.
    return true;
  }
  if (this->StartsWith(this->owner->opBind(), 2)) {
    outputAccumulateBoundVariables.addOnTopNoRepetition((*this)[1]);
    return true;
  }
  for (int i = 0; i < this->size(); i ++) {
    if (!((*this)[i].GetBoundVariables(outputAccumulateBoundVariables))) {
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
  if (this->IsBuiltInTypE()) { //<- this may need to be rewritten as some built in types will store free variables in their context.
    return true;
  }
  if (this->IsOfType<InputBox>()) {
    return true;
  }
  std::string atomName;
  if (this->IsOperation(&atomName)) {
    bool doAddExpression = !this->IsKnownFunctionWithComplexRange();
    if (doAddExpression) {
      doAddExpression = this->AllowedAsFreeVariableAtom(atomName);
    }
    if (doAddExpression && excludeNamedConstants) {
      if (this->owner->knownDoubleConstants.Contains(*this)) {
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

bool Expression::IsIntegraLfdx(
  Expression* differentialVariable, Expression* functionToIntegrate, Expression* integrationSet
) const {
  MacroRegisterFunctionWithName("Expression::IsIntegraLfdx");
  if (this->owner == nullptr) {
    return false;
  }
  if (!this->StartsWith(this->owner->opIntegral(), 3)) {
    return false;
  }
  if (!(*this)[2].IsDifferentialOneFormOneVariablE(differentialVariable, functionToIntegrate)) {
    return false;
  }
  if (integrationSet != nullptr) {
    *integrationSet = (*this)[1];
  }
  return true;
}

bool Expression::IsIndefiniteIntegralfdx(
  Expression* differentialVariable, Expression* functionToIntegrate, Expression* integrationSet
) const {
  MacroRegisterFunctionWithName("Expression::IsIndefiniteIntegralfdx");
  this->CheckConsistency();
  Expression tempE;
  if (integrationSet == nullptr) {
    integrationSet = &tempE;
  }
  if (!this->IsIntegraLfdx(differentialVariable, functionToIntegrate, integrationSet)) {
    return false;
  }
  return integrationSet->IsOperationGiven(this->owner->opIndefiniteIndicator());
}

bool Expression::IsDefiniteIntegralOverIntervalfdx(
  Expression* differentialVariable, Expression* functionToIntegrate, Expression* integrationSet
) const {
  MacroRegisterFunctionWithName("Expression::IsDefiniteIntegralOverIntervalfdx");
  this->CheckConsistency();
  Expression tempE;
  if (integrationSet == nullptr) {
    integrationSet = &tempE;
  }
  if (!this->IsIntegraLfdx(differentialVariable, functionToIntegrate, integrationSet)) {
    return false;
  }
  return integrationSet->StartsWith(this->owner->opLimitBoundary(), 3);
}

bool Expression::IsDifferentialOneFormOneVariablE(
  Expression* outputDifferentialOfWhat, Expression* outputCoeffInFrontOfDifferential
) const {
  MacroRegisterFunctionWithName("Expression::IsDifferentialOneFormOneVariablE");
  if (this->owner == nullptr) {
    return false;
  }
  if (!this->StartsWith(this->owner->opDifferential()) || this->size() != 3) {
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
  theE.MakeAtom(inputAtom, *this->owner);
  return this->ContainsAsSubExpressionNoBuiltInTypes(theE);
}

bool Expression::IsContext() const {
  if (this->owner == nullptr) {
    return false;
  }
  return this->StartsWith(this->owner->opContext());
}

bool Expression::IsError(std::string* outputErrorMessage) const {
  if (this->owner == nullptr) {
    return false;
  }
  if (!this->StartsWith(this->owner->opError(), 2)) {
    return false;
  }
  return (*this)[1].IsOfType<std::string>(outputErrorMessage);
}

bool Expression::IsSequenceDoubleButNotTripleNested() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (!this->IsSequenceNElementS()) {
    return false;
  }
  for (int i = 1; i < this->size(); i ++) {
    if (!((*this)[i].IsSequenceNElementS())) {
      return false;
    }
  }
  for (int i = 0; i < this->size(); i ++) {
    for (int j = 0; j < (*this)[i].size(); j ++) {
      if (!(*this)[i][j].IsSequenceNElementS()) {
        return true;
      }
    }
  }
  return false;
}

bool Expression::IsSequenceNElementS(int N) const {
  if (this->owner == nullptr) {
    return false;
  }
  return this->StartsWith(this->owner->opSequence(), N + 1);
}

bool Expression::IsIntervalRealLine() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->StartsWith(this->owner->opIntervalOpen())) {
    return true;
  }
  if (this->StartsWith(this->owner->opIntervalClosed(), 3)) {
    return true;
  }
  if (this->StartsWith(this->owner->opIntervalLeftClosed(), 3)) {
    return true;
  }
  if (this->StartsWith(this->owner->opIntervalRightClosed(), 3)) {
    return true;
  }
  return false;
}

bool Expression::IsEqualToOne() const {
  int theInt;
  if (this->IsSmallInteger(&theInt)) {
    return theInt == 1;
  }
  return false;
}

bool Expression::IsEqualToTwo() const {
  int theInt;
  if (this->IsSmallInteger(&theInt)) {
    return theInt == 2;
  }
  return false;
}

bool Expression::IsEqualToHalf() const {
  Rational theRat;
  if (this->IsRational(&theRat)) {
    Rational half(1, 2);
    return theRat == half;
  }
  return false;
}

bool Expression::IsKnownToBeNonNegative() const {
  MacroRegisterFunctionWithName("Expression::IsKnownToBeNonNegative");
  double theDouble = 0;
  if (this->EvaluatesToDouble(&theDouble)) {
    return theDouble >= 0;
  }
  if (this->owner == nullptr) {
    return false;
  }
  Expression testInequality, testResult;
  Calculator& theCommands = *(this->owner);
  testInequality.MakeXOX(theCommands, theCommands.opGreaterThan(), *this, theCommands.EZero());
  if (this->owner->EvaluateExpression(theCommands, testInequality, testResult)) {
    if (testResult.IsEqualToOne()) {
      return true;
    }
  }
  testInequality.MakeXOX(theCommands, theCommands.opGreaterThanOrEqualTo(), *this, theCommands.EZero());
  if (theCommands.EvaluateExpression(theCommands, testInequality, testResult)) {
    if (testResult.IsEqualToOne()) {
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
  if (this->IsOfType<Rational>(&theRat)) {
    return theRat.IsNegative();
  }
  if (!this->IsConstantNumber()) {
    return false;
  }
  double tempD = - 1;
  if (!this->EvaluatesToDouble(&tempD)) {
    return false;
  }
  return tempD < 0;
}

bool Expression::IsEqualToMOne() const {
  MacroRegisterFunctionWithName("Expression::IsEqualToMOne");
  if (!this->IsConstantNumber()) {
    return false;
  }
  if (this->IsOfType<Rational>()) {
    return this->GetValue<Rational>() == - 1;
  }
  return false;
}

bool Expression::IsEqualToZero() const {
  if (this->IsOfType<Rational>()) {
    return this->GetValue<Rational>().IsEqualToZero();
  }
  if (this->IsOfType<double>()) {
    return this->GetValue<double>() == 0.0;
  }
  if (this->IsOfType<AlgebraicNumber>()) {
    return this->GetValue<AlgebraicNumber>().IsEqualToZero();
  }
  if (this->IsOfType<Polynomial<Rational> >()) {
    return this->GetValue<Polynomial<Rational> >().IsEqualToZero();
  }
  if (this->IsOfType<RationalFunction>()) {
    return this->GetValue<RationalFunction>().IsEqualToZero();
  }
  return false;
}

bool Expression::MakeError(const std::string& theError, Calculator& owner) {
  this->reset(owner, 2);
  this->CheckConsistency();
  this->AddChildAtomOnTop(owner.opError());
  this->CheckConsistency();
  return this->AddChildValueOnTop(theError);
}

bool Expression::IsSmallInteger(int* whichInteger) const {
  Rational theRat;
  if (this->IsOfType<Rational>(&theRat)) {
    return theRat.IsSmallInteger(whichInteger);
  }
  AlgebraicNumber theAlgNum;
  if (this->IsOfType<AlgebraicNumber>(&theAlgNum)) {
    return theAlgNum.IsSmallInteger(whichInteger);
  }
  return false;
}

bool Expression::IsRational(Rational* whichRational) const {
  if (this->IsOfType<Rational>(whichRational)) {
    return true;
  }
  AlgebraicNumber theAlgNum;
  if (this->IsOfType<AlgebraicNumber>(&theAlgNum)) {
    return theAlgNum.IsRational(whichRational);
  }
  return false;
}

bool Expression::DivisionByMeShouldBeWrittenInExponentForm() const {
  if (this->owner == nullptr) {
    return false;
  }
  RecursionDepthCounter theCounter(&this->owner->RecursionDeptH);
  if (this->owner->RecursionDepthExceededHandleRoughly()) {
    return false;
  }
  if (this->StartsWith(this->owner->opTimes())) {
    for (int i = 1; i < this->children.size; i ++) {
      if (!(*this)[i].DivisionByMeShouldBeWrittenInExponentForm()) {
        return false;
      }
    }
  }
  if (this->StartsWith(this->owner->opThePower(), 3)) {
    return (*this)[1].DivisionByMeShouldBeWrittenInExponentForm();
  }
  if (this->StartsWith(this->owner->opPlus())) {
    return false;
  }
  return true;
}

bool Expression::IsPositiveNumber() const {
  MacroRegisterFunctionWithName("Expression::IsPositiveNumber");
  Rational theRat;
  if (this->IsOfType<Rational>(&theRat)) {
    return theRat.IsPositive();
  }
  double theDouble = - 1;
  if (!this->EvaluatesToDouble(&theDouble)) {
    return false;
  }
  if (std::isnan(theDouble)) {
    return false;
  }
  return theDouble > 0;
}

bool Expression::IsConstantNumber() const {
  MacroRegisterFunctionWithName("Expression::IsConstantNumber");
  if (this->owner == nullptr) {
    return false;
  }
  RecursionDepthCounter thecounter(&this->owner->RecursionDeptH);
  if (this->owner->RecursionDepthExceededHandleRoughly("In Expression::IsConstantNumber: ")) {
    return false;
  }
  if (this->IsOfType<Rational>() || this->IsOfType<AlgebraicNumber>() || this->IsOfType<double>()) {
    return true;
  }
  if (this->IsOperationGiven(this->owner->opPi())) {
    return true;
  }
  if (this->IsOperationGiven(this->owner->opE())) {
    return true;
  }
  if (this->StartsWithFunctionWithComplexRange()) {
    for (int i = 1; i < this->size(); i ++) {
      if (!(*this)[i].IsConstantNumber()) {
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
  if (this->IsOfType<Rational>() || this->IsOfType<AlgebraicNumber>()) {
    return true;
  }
  if (
    this->StartsWith(this->owner->opPlus()) ||
    this->StartsWith(this->owner->opMinus())||
    this->StartsWith(this->owner->opTimes()) ||
    this->StartsWith(this->owner->opDivide())
  ) {
    for (int i = 1; i < this->size(); i ++) {
      if (!(*this)[i].IsAlgebraicRadical()) {
        return false;
      }
    }
    return true;
  }
  if (this->StartsWith(this->owner->opThePower(), 3)) {
    if (!(*this)[2].IsRational()) {
      return false;
    }
    return (*this)[1].IsAlgebraicRadical();
  }
  if (this->StartsWith(this->owner->opSqrt(), 3)) {
    if (!(*this)[1].IsRational()) {
      return false;
    }
    return (*this)[2].IsAlgebraicRadical();
  }
  return false;
}

bool Expression::IsInteger(LargeInteger* whichInteger) const {
  Rational theRat;
  if (!this->IsOfType<Rational>(&theRat)) {
    return false;
  }
  return theRat.IsInteger(whichInteger);
}

bool Expression::IsIntegerNonNegative(LargeIntegerUnsigned* whichInteger) const {
  Rational theRat;
  if (!this->IsOfType<Rational>(&theRat)) {
    return false;
  }
  if (theRat.IsNonPositive()) {
    return false;
  }
  if (whichInteger == nullptr) {
    return theRat.IsInteger();
  }
  LargeInteger container;
  if (!theRat.IsInteger(&container)) {
    return false;
  }
  *whichInteger = container.value;
  return true;
}

bool Expression::IsIntegerFittingInInt(int* whichInteger) const {
  Rational theRat;
  if (!this->IsOfType<Rational>(&theRat)) {
    return false;
  }
  return theRat.IsIntegerFittingInInt(whichInteger);
}

bool Expression::MakeAtom(const std::string& atomName, Calculator& newBoss) {
  this->reset(newBoss);
  this->theData = newBoss.AddOperationNoRepetitionOrReturnIndexFirst(atomName);
  return true;
}

void Expression::GetBaseExponentForm(Expression& outputBase, Expression& outputExponent) const {
  this->CheckInitialization();
  if (this->StartsWith(this->owner->opThePower(), 3)) {
    outputBase = (*this)[1];
    outputExponent = (*this)[2];
    return;
  }
  outputBase = *this;
  outputExponent.AssignValue(1, *this->owner);
}

bool Expression::GetContext(ExpressionContext& output) const {
  output = this->GetContext();
  return true;
}

ExpressionContext Expression::GetContext() const {
  this->CheckInitialization();
  ExpressionContext result(*this->owner);
  if (this->IsBuiltInTypE()) {
    result.fromExpression((*this)[1]);
    return result;
  }
  if (this->IsMatrix()) {
    if ((*this)[0].StartsWith(this->owner->opMatriX())) {
      if ((*this)[0].size() > 2) {
        result.fromExpression((*this)[0][2]);
        return result;
      }
    }
  }
  global.fatal
  << "This is a programming error: GetContext called on an Expression "
  << "that is not a built-in data type. "
  << "I can't display the expression as this may cause ``infinite'' "
  << "recursion if the error is caused by the toString method. "
  << "Here is however the lisp form "
  << this->toStringFull() << " of the expression. "
  << "Here's stack trace. " << global.fatal;
  return ExpressionContext(*this->owner);
}

int Expression::GetNumCols() const {
  MacroRegisterFunctionWithName("Expression::GetNumCols");
  if (!this->IsSequenceNElementS()) {
    return - 1;
  }
  int theMax = 1;
  for (int i = 1; i < this->size(); i ++) {
    if ((*this)[i].IsSequenceNElementS()) {
      theMax = MathRoutines::Maximum((*this)[i].size() - 1, theMax);
    }
  }
  return theMax;
}

void Expression::GetMultiplicandsRecursive(List<Expression>& outputAppendList, int depth) const {
  MacroRegisterFunctionWithName("Expression::GetMultiplicandsRecursive");
  this->CheckInitialization();
  if (depth == 0) {
    outputAppendList.setSize(0);
  }
  if (!this->StartsWith(this->owner->opTimes())) {
    outputAppendList.addOnTop(*this);
    return;
  }
  for (int i = 1; i < this->size(); i ++) {
    (*this)[i].GetMultiplicandsDivisorsRecursive(outputAppendList, depth + 1);
  }
}

void Expression::GetMultiplicandsDivisorsRecursive(List<Expression>& outputAppendList, int depth) const {
  MacroRegisterFunctionWithName("Expression::GetMultiplicandsDivisorsRecursive");
  this->CheckInitialization();
  if (depth == 0) {
    outputAppendList.setSize(0);
  }
  if (!this->StartsWith(this->owner->opTimes()) && !this->StartsWith(this->owner->opDivide())) {
    outputAppendList.addOnTop(*this);
    return;
  }
  for (int i = 1; i < this->size(); i ++) {
    (*this)[i].GetMultiplicandsDivisorsRecursive(outputAppendList, depth + 1);
  }
}

void Expression::GetCoefficientMultiplicandForm(Expression& outputCoeff, Expression& outputNoCoeff) const {
  MacroRegisterFunctionWithName("Expression::GetCoefficientMultiplicandForm");
  this->CheckInitialization();
  if (this->StartsWith(this->owner->opTimes(), 3)) {
    if ((*this)[1].IsConstantNumber()) {
      outputNoCoeff = (*this)[2];
      outputCoeff = (*this)[1];
      return;
    }
  }
  outputCoeff.AssignValue(1, *this->owner);
  outputNoCoeff = *this;
}

void Expression::GetCoefficientMultiplicandForm(Rational& outputCoeff, Expression& outputNoCoeff) const {
  this->CheckInitialization();
  if (this->StartsWith(this->owner->opTimes(), 3)) {
    if ((*this)[1].IsOfType(&outputCoeff)) {
      outputNoCoeff = (*this)[2];
      return;
    }
  }
  outputCoeff = 1;
  outputNoCoeff = *this;
  return;
}

int Expression::GetExpressionTreeSize() const {
  if (this->owner == nullptr) {
    return 1;
  }
  this->CheckInitialization();
  RecursionDepthCounter theCounter(&this->owner->RecursionDeptH);
  if (this->owner->RecursionDepthExceededHandleRoughly("While computing Expression::GetExpressionTreeSize: ")) {
    return 1;
  }
  if (this->IsAtom()) {
    return 1;
  }
  int result = 0;
  for (int i = 0; i < this->children.size; i ++) {
    result += (*this)[i].GetExpressionTreeSize();
  }
  return result;
}

bool Expression::operator>(const Expression& other) const {
  MacroRegisterFunctionWithName("Expression::operatorGreaterThan");
  double left = 0, right = 0;
  bool leftEvalsToDouble = this->EvaluatesToDouble(&left);
  bool rightEvalsToDouble = other.EvaluatesToDouble(&right);
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
  this->GetCoefficientMultiplicandForm(leftCoeff, leftMon);
  other.GetCoefficientMultiplicandForm(rightCoeff, rightMon);
  if (leftMon == rightMon) {
    bool result = leftCoeff > rightCoeff;
    return result;
  }
  bool result = leftMon.GreaterThanNoCoeff(rightMon);
  return result;
}

bool Expression::GreaterThanNoCoeff(const Expression& other) const {
  MacroRegisterFunctionWithName("Expression::GreaterThanNoCoeff");
  if (this->IsOfType<Rational>() && other.IsOfType<Rational>()) {
    return this->GetValue<Rational>() > other.GetValue<Rational>();
  }
  if (this->IsBuiltInTypE() && !other.IsBuiltInTypE()) {
    return false;
  }
  if (!this->IsBuiltInTypE() && other.IsBuiltInTypE()) {
    return true;
  }
  int thisExpressionTreeSize = this->GetExpressionTreeSize();
  int otherExpressionTreeSize = other.GetExpressionTreeSize();
  if (thisExpressionTreeSize > otherExpressionTreeSize) {
    return true;
  }
  if (otherExpressionTreeSize > thisExpressionTreeSize) {
    return false;
  }
  if (this->children.size == 0 && other.children.size == 0) {
    std::string leftS, rightS;
    if (this->IsOperation(&leftS)) {
      if (other.IsOperation(&rightS)) {
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
      << StringRoutines::ConvertStringToCalculatorDisplay(input.GetValue<std::string>());
    } else {
      out << HtmlRoutines::ConvertStringToHtmlString(input.GetValue<std::string>(), false);
    }
  } else {
    out << "\"" << StringRoutines::ConvertStringToJavascriptString(input.GetValue<std::string>()) << "\"";
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
    out << input.GetValue<Rational>().toString();
  } else {
    out << input.GetValue<Rational>().ToStringFrac();
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
  input.GetContext().getFormat(format);
  format.flagUseFrac = true;
  out << input.GetValue<ElementEllipticCurve<Rational> >().toString(&format);
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
  input.GetContext().getFormat(format);
  format.flagUseFrac = true;
  out << input.GetValue<ElementEllipticCurve<ElementZmodP> >().toString(&format);
  return true;
}

template<>
bool Expression::toStringBuiltIn<ElementZmodP>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  out << input.GetValue<ElementZmodP>().toString();
  return true;
}

template<>
bool Expression::toStringBuiltIn<InputBox>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  out << input.GetValue<InputBox>().GetUserInputBox();
  return true;
}

template<>
bool Expression::toStringBuiltIn<GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  out << input.GetValue<GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> >().toString();
  return true;
}

template<>
bool Expression::toStringBuiltIn<ElementHyperoctahedralGroupR2>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  out << input.GetValue<ElementHyperoctahedralGroupR2>().toString(theFormat);
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
  input.GetContext().getFormat(format);
  format.flagUseFrac = true;
  if (!input.owner->flagHidePolynomialBuiltInTypeIndicator) {
    out << "Polynomial{}(";
  }
  out << input.GetValue<Polynomial<Rational> >().toString(&format);
  if (!input.owner->flagHidePolynomialBuiltInTypeIndicator) {
    out << ")";
  }
  if (showContext) {
    out << "[" << input.GetContext().toString() << "]";
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
  format.flagSuppressModP = true;
  const Polynomial<ElementZmodP>& polynomial = input.GetValue<Polynomial<ElementZmodP> >();
  if (!polynomial.IsEqualToZero()) {
    format.suffixLinearCombination = "(" + polynomial.coefficients[0].toStringModP() + ")" ;
  }
  input.GetContext().getFormat(format);
  format.flagUseFrac = true;
  if (!input.owner->flagHidePolynomialBuiltInTypeIndicator) {
    out << "Polynomial{}(";
  }
  out << polynomial.toString(&format);
  if (!input.owner->flagHidePolynomialBuiltInTypeIndicator) {
    out << ")";
  }
  if (showContext) {
    out << "[" << input.GetContext().toString() << "]";
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
  input.GetContext().getFormat(format);
  bool showContext = input.owner == nullptr ? false : input.owner->flagDisplayContext;
  format.flagUseFrac = true;
  out << "PolynomialAlgebraicNumbers{}(";
  std::string currentString = input.GetValue<Polynomial<AlgebraicNumber> >().toString(&format);
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
    out << "[" << input.GetContext().toString() << "]";
  }
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
  input.GetContext().getFormat(format);
  format.flagUseFrac = true;
  out << "MakeRationalFunction{}("
  << input.GetValue<RationalFunction>().toString(&format) << ")";
  if (showContext) {
    out << "[" << input.GetContext().toString() << "]";
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
  input.GetContext().getFormat(format);
  format.flagFormatWeightAsVectorSpaceIndex = false;
  out << input.GetValue<Weight<Polynomial<Rational> > >().toString(&format);
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
  << input.GetValue<SemisimpleLieAlgebra*>()->ToStringLieAlgebraName()
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
  input.GetContext().getFormat(format);
  out << "UEE{}(" //<< input.GetContext().toString() << ", "
  << input.GetValue<ElementUniversalEnveloping<RationalFunction> >().toString(&format)
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
  input.GetContext().getFormat(format);
  format.flagUseLatex = true;
  format.flagUseHTML = false;
  if (input.GetValue<MatrixTensor<Rational> >().GetMinNumColsNumRows() < 20) {
    out << "MatrixRationalsTensorForm{}("
    << input.GetValue<MatrixTensor<Rational> > ().toStringMatrixForm(&format)
    << ")";
  } else {
    out << input.GetValue<MatrixTensor<Rational> >().toString();
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
  input.GetContext().getFormat(format);
  out << "ETGVM{}(";
  out << input.GetValue<ElementTensorsGeneralizedVermas<RationalFunction> >().toString(&format);
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
    Plot& thePlot = input.GetValueNonConst<Plot>();
    thePlot.flagIncludeExtraHtmlDescriptions = (theFormat == nullptr) ? true : theFormat->flagIncludeExtraHtmlDescriptionsInPlots;
    thePlot.flagPlotShowJavascriptOnly = input.owner->flagPlotShowJavascriptOnly;
    out << thePlot.GetPlotHtml(*input.owner);
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
  WeylGroupData& theGroup = input.GetValueNonConst<WeylGroupData>();
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
  const ElementWeylGroup& theElt = input.GetValue<ElementWeylGroup>();
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
  input.GetValue<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >();
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
  input.GetContext().getFormat(format);
  const VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& theElt =
  input.GetValue<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >();
  format.flagUseLatex = true;
  format.flagUseHTML = false;
  format.flagUseReflectionNotation = true;
  out << theElt.toString(&format);
  return true;
}

template<>
bool Expression::toStringBuiltIn<charSSAlgMod<Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* theFormat
) {
  (void) theFormat;
  charSSAlgMod<Rational> theElt = input.GetValue<charSSAlgMod<Rational> >();
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
  SemisimpleSubalgebras& theSubalgebras = input.GetValueNonConst<SemisimpleSubalgebras>();
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
  std::string currentString = FloatingPoint::DoubleToString(input.GetValue<double>());
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
  std::string currentString = input.GetValue<AlgebraicNumber>().toString(&format);
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
  out << input.GetValue<LittelmannPath>().toString();
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
  input.GetContext().getFormat(format);
  format.flagUseHTML = false;
  format.flagUseLatex = true;
  out << "ElementWeylAlgebra{}(";
  out << input.GetValue<ElementWeylAlgebra<Rational> >().toString(&format);
  out << ")";
  if (showContext) {
    out << "[" << input.GetContext().toString() << "]";
  }
  return true;
}

bool Expression::toStringData(std::stringstream& out, FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("Expression::toStringData");
  if (this->owner == nullptr) {
    out << "(non-initialized)";
    return true;
  }
  if (this->IsAtom()) {
    if (this->IsOperationGiven(this->owner->opDifferential())) {
      out << "{\\text{d}}";
    } else if (this->IsOperationGiven(this->owner->opPhantom())) {
      out << "\\phantom{}";
    } else if (this->owner->flagUseLnInsteadOfLog && this->IsOperationGiven(this->owner->opLog())) {
      out << "\\ln";
    } else if (this->theData < this->owner->GetOperations().size && this->theData >= 0) {
      out << this->owner->GetOperations()[this->theData];
    } else {
      out << "[unknown atom of value " << this->theData << "]";
    }
    return true;
  }
  if (this->IsMatrixOfType<RationalFunction>()) {
    FormatExpressions format;
    this->GetContext().getFormat(format);
    format.flagUseHTML = false;
    format.flagUseLatex = true;
    Matrix<RationalFunction> theMat;
    this->owner->functionGetMatrix(*this, theMat);
    out << theMat.toString(&format);
    return true;
  }
  int theType = - 1;
  if (!this->IsBuiltInTypE(&theType)) {
    return false;
  }
  Calculator& commands = *this->owner;
  // The following handlers are initialized in Calculator::initializeToStringHandlers.
  if (!commands.toStringDataHandlers.Contains(theType)) {
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
  if (this->IsAtom()) {
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

bool Expression::NeedsParenthesisForBaseOfExponent() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->StartsWith(this->owner->opBind())) {
    return (*this)[1].IsLisT();
  }
  if (this->StartsWith(this->owner->opPlus()) || this->StartsWith(this->owner->opMinus())) {
    return true;
  }
  if (this->IsOfType<Rational>()) {
    return this->GetValue<Rational>() < 0 || !this->GetValue<Rational>().IsInteger();
  }
  if (this->IsOfType<double>()) {
    return this->GetValue<double>() < 0;
  }
  if (this->IsOfType<AlgebraicNumber>()) {
    LargeInteger tempI;
    if (!this->GetValue<AlgebraicNumber>().IsInteger(&tempI)) {
      return true;
    }
    return tempI > 0;
  }
  if (this->IsAtom() || this->children.size == 0) {
    return false;
  }
  return true;
}

bool Expression::NeedsParenthesisForMultiplicationWhenSittingOnTheRightMost(const Expression* leftNeighbor) const {
  if (this->owner == nullptr) {
    return false;
  }
  if (leftNeighbor != nullptr) {
    if (leftNeighbor->StartsWith(this->owner->opDivide(), 3)) {
      if ((*leftNeighbor)[1] == "Differential") {
        return true;
      }
    }
  }
  if (
    this->StartsWith(this->owner->opPlus()) ||
    this->StartsWith(this->owner->opMinus()) ||
    this->StartsWith(this->owner->opDefine())
  ) {
    return true;
  }
  if (this->IsOfType<Rational>()) {
    return this->GetValue<Rational>().NeedsParenthesisForMultiplicationWhenSittingOnTheRightMost();
  }
  if (this->IsOfType<AlgebraicNumber>()) {
    return this->GetValue<AlgebraicNumber>().NeedsParenthesisForMultiplicationWhenSittingOnTheRightMost();
  }
  if (this->IsAtom() || this->size() == 0) {
    return false;
  }
  if (this->StartsWith(this->owner->opUnderscore())) {
    return false;
  }
  if (this->StartsWith(this->owner->opTimes(), 3)) {
    if ((*this)[1].IsOfType<Rational>()) {
      return (*this)[1].GetValue<Rational>().NeedsParenthesisForMultiplicationWhenSittingOnTheRightMost();
    }
    return false;
  }
  if (this->size() > 1) {
    const Expression& firstE = (*this)[0];
    if (firstE.IsOperationGiven(this->owner->opTimes())) {
      return
      (*this)[1].NeedsParenthesisForMultiplicationWhenSittingOnTheRightMost() ||
      (*this)[1].NeedsParenthesisForMultiplication();
    }
    if (firstE.IsOperationGiven(this->owner->opThePower())) {
      return false;
    }
    if (this->StartsWith(this->owner->opBind())) {
      return (*this)[1].NeedsParenthesisForMultiplication();
    }
    return false;
  }
  return false;
}

bool Expression::NeedsParenthesisForAddition() const {
  MacroRegisterFunctionWithName("Expression::NeedsParenthesisForAddition");
  if (this->owner == nullptr) {
    return false;
  }
  if (this->StartsWith(this->owner->opDefine())) {
    return true;
  }
  return false;
}

bool Expression::NeedsParenthesisForMultiplication(FormatExpressions* theFormat) const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->IsOfType<std::string>()) {
    return false;
  }
  if (this->StartsWith(this->owner->opAbsoluteValue())) {
    return false;
  }
  if (this->IsMatrix()) {
    return false;
  }
  if (
    this->StartsWith(this->owner->opPlus()) ||
    this->StartsWith(this->owner->opMinus()) ||
    this->StartsWith(this->owner->opDefine()) ||
    this->StartsWith(this->owner->opLimit())
  ) {
    return true;
  }
  if (this->StartsWith(this->owner->opDivide(), 3)) {
    if ((*this)[1] == "Differential") {
      return false;
    }
  }
  if (this->IsSequenceNElementS()) {
    if (this->size() > 2) {
      return false;
    }
  }
  if (this->IsOfType<Rational>()) {
    return this->GetValue<Rational>().NeedsParenthesisForMultiplication(theFormat);
  }
  if (this->IsOfType<AlgebraicNumber>()) {
    return this->GetValue<AlgebraicNumber>().NeedsParenthesisForMultiplication(theFormat);
  }
  if (this->IsAtom() || this->children.size == 0) {
    return false;
  }
  if (this->StartsWith(this->owner->opUnderscore())) {
    return false;
  }
  if (this->size() > 1) {
    const Expression& firstE = (*this)[0];
    if (firstE.IsOperationGiven(this->owner->opTimes())) {
      return false;
    }
    if (firstE.IsOperationGiven(this->owner->opThePower())) {
      const Expression& base = (*this)[1];
      if (base.children.size > 0) {
        if (base[0].IsAtomWhoseExponentsAreInterpretedAsFunction()) {
          return true;
        }
      }
      return false;
    }
    if (this->StartsWith(this->owner->opBind())) {
      return (*this)[1].NeedsParenthesisForMultiplication();
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
  if (input.StartsWith(theCommands.opCommandEnclosure())) {
    if (input.size() <= 1) {
      return false;
    }
    Expression result(theCommands);
    result.AddChildAtomOnTop(theCommands.opCommandEnclosureStart());
    if (input.size() == 2) {
      result.AddChildOnTop(input[1]);
      result.AddChildAtomOnTop(theCommands.opCommandEnclosureFinish());
      output = result;
      return true;
    }
    for (int i = 1; i < input.size(); i ++) {
      result.AddChildOnTop(input[i]);
    }
    result.AddChildAtomOnTop(theCommands.opCommandEnclosureFinish());
    output = result;
    return true;
  }
  if (!input.StartsWith(theCommands.opEndStatement())) {
    return false;
  }
  Expression result(theCommands);
  for (int i = 0; i < input.size(); i ++) {
    if (input[i].StartsWith(theCommands.opCommandEnclosure())) {
      bool processed = false;
      result.AddChildAtomOnTop(theCommands.opCommandEnclosureStart());
      if (input[i].size() == 2) {
        if (input[i][1].StartsWith(theCommands.opEndStatement())) {
          for (int j = 1; j < input[i][1].size(); j ++) {
            result.AddChildOnTop(input[i][1][j]);
          }
          processed = true;
        }
      }
      if (!processed) {
        for (int j = 1; j < input[i].size(); j ++) {
          result.AddChildOnTop(input[i][j]);
        }
      }
      result.AddChildAtomOnTop(theCommands.opCommandEnclosureFinish());
    } else if (input[i].StartsWithGivenOperation("MatchesPattern")) {
      result.AddChildAtomOnTop(theCommands.opCommandEnclosureStart());
      result.AddChildOnTop(input[i]);
      result.AddChildAtomOnTop(theCommands.opCommandEnclosureFinish());
    } else {
      result.AddChildOnTop(input[i]);
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
    if (!theSliders.Contains(boxNames[i])) {
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
    if (theBox.min.EvaluatesToDouble(&theReader)) {
      out << "min =\"" << theReader << "\" ";
    }
    if (theBox.max.EvaluatesToDouble(&theReader)) {
      out << "max =\"" << theReader << "\" ";
    }
    if (theBox.step.EvaluatesToDouble(&theReader)) {
      out << "step =\"" << theReader << "\" ";
    }
    if (theBox.value.EvaluatesToDouble(&theReader)) {
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
  if (this->IsListStartingWithAtom(this->owner->opEndStatement())) {
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
      if (currentE.IsOfType<std::string>()) {
        out << currentE.GetValue<std::string>();
      } else if (
        currentE.HasType<Plot> () ||
        currentE.IsOfType<SemisimpleSubalgebras>() ||
        currentE.IsOfType<WeylGroupData>() ||
        currentE.IsOfType<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >()
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
      this->IsOfType<std::string>() || this->IsOfType<Plot>() ||
      this->IsOfType<SemisimpleSubalgebras>() || this->IsOfType<WeylGroupData>()
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
  if (!input.StartsWith(input.owner->opTimes(), 3)) {
    return false;
  }
  std::string secondE = input[2].toString(theFormat);
  if (input[1].IsOperationGiven(input.owner->opSqrt())) {
    // A malformed expression such as: "\sqrt 3" will be parsed as "sqrt * 3"
    // and later corrected to "\sqrt{3}".
    out << "\\sqrt{" << secondE << "}";
    return true;
  }
  std::string firstE = input[1].toString(theFormat);
  bool firstNeedsBrackets = input[1].NeedsParenthesisForMultiplication();
  bool secondNeedsBrackets = input[2].NeedsParenthesisForMultiplicationWhenSittingOnTheRightMost(&(input[1]));
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
  bool firstNeedsBrackets = (*this)[1].NeedsParenthesisForMultiplication();
  bool secondNeedsBrackets = (*this)[2].NeedsParenthesisForMultiplication();
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
    if (data.str() == "" && this->IsOfType<std::string>()) {
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
  if (!input.StartsWith(commands.opDivide(), 3)) {
    return false;
  }
  bool doUseFrac = commands.flagUseFracInRationalLaTeX;
  if (
    doUseFrac &&
    (input[1].StartsWith(commands.opTimes()) || input[1].StartsWith(commands.opDivide()))
  ) {
    List<Expression> multiplicandsLeft;
    input.GetMultiplicandsDivisorsRecursive(multiplicandsLeft, 0);
    for (int i = 0; i < multiplicandsLeft.size; i ++) {
      if (
        multiplicandsLeft[i].StartsWith(commands.opIntegral()) ||
        multiplicandsLeft[i].IsOperationGiven(commands.opIntegral())
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
      !(input[1].IsListStartingWithAtom(commands.opTimes()) ||
      input[1].IsListStartingWithAtom(commands.opDivide()));
    bool secondNeedsBrackets = true;
    if (input[2].IsOfType<Rational>()) {
      if (input[2].GetValue<Rational>().IsInteger()) {
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
  if (!input.StartsWith(commands.opThePower(), 3)) {
    return false;
  }
  bool involvesExponentsInterpretedAsFunctions = false;
  const Expression& firstE = input[1];
  const Expression& secondE = input[2];
  if (firstE.StartsWith(- 1, 2)) {
    bool shouldProceed = firstE[0].IsAtomWhoseExponentsAreInterpretedAsFunction() &&
    !secondE.IsEqualToMOne() && secondE.IsRational();
    if (
      shouldProceed && firstE[0].IsOperationGiven(commands.opLog()) &&
      commands.flagUseLnAbsInsteadOfLogForIntegrationNotation
    ) {
      shouldProceed = false;
    }
    if (shouldProceed) {
      involvesExponentsInterpretedAsFunctions = true;
      Expression newFunE;
      newFunE.MakeXOX(*input.owner, commands.opThePower(), firstE[0], input[2]);
      newFunE.CheckConsistency();
      out << "{" << newFunE.toString(theFormat) << "}{}";
      if (
        firstE[1].NeedsParenthesisForMultiplicationWhenSittingOnTheRightMost() ||
        firstE[1].StartsWith(commands.opTimes())
      ) {
        out << "\\left(" << firstE[1].toString(theFormat) << "\\right)";
      } else {
        out << firstE[1].toString(theFormat);
      }
    }
  }
  if (!involvesExponentsInterpretedAsFunctions) {
    bool isSqrt = false;
    if (input[2].IsOfType<Rational>()) {
      if (input[2].GetValue<Rational>().IsEqualTo(Rational(1, 2))) {
        isSqrt = true;
      }
    }
    if (isSqrt) {
      out << "\\sqrt{" << input[1].toString(theFormat) << "}";
    } else {
      std::string secondEstr = input[2].toString(theFormat);
      std::string firstEstr = input[1].toString(theFormat);
      if (input[1].NeedsParenthesisForBaseOfExponent()) {
        bool useBigParenthesis = true;
        if (input[1].StartsWith(commands.opDivide())) {
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
    if ((*this)[0].IsAtomWhoseExponentsAreInterpretedAsFunction()) {
      needParenthesis = !(*this)[1].IsAtom();
    }
    if ((*this)[0].IsPowerOfAtomWhoseExponentsAreInterpretedAsFunction()) {
      needParenthesis = !(*this)[1].IsAtom();
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
  if (!this->IsListStartingWithAtom(this->owner->opEndStatement())) {
    return false;
  }
  MemorySaving<FormatExpressions> temporaryFormat;
  if (theFormat == nullptr) {
    theFormat = &temporaryFormat.GetElement();
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
      if ((*this)[i].IsOfType<std::string>() && isFinal) {
        currentOutput = StringRoutines::ConvertStringToCalculatorDisplay(currentE.GetValue<std::string>());
      } else if ((
          currentE.HasType<Plot> () ||
          currentE.IsOfType<SemisimpleSubalgebras>() ||
          currentE.IsOfType<WeylGroupData>() ||
          currentE.IsOfType<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >()
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
        addLatexDelimiter = !currentE.HasType<Plot>();
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
  if (!input.StartsWith(input.owner->opPlus())) {
    return false;
  }
  if (input.children.size < 3) {
    global.fatal << "Plus operation takes at least 2 arguments, whereas this expression has "
    << input.children.size - 1 << " arguments. " << global.fatal;
  }
  const Expression& left = input[1];
  const Expression& right = input[2];
  std::string leftString;
  if (left.NeedsParenthesisForAddition()) {
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
  if (right.NeedsParenthesisForAddition()) {
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
  if (!input.IsListStartingWithAtom(input.owner->opDirectSum())) {
    return false;
  }
  if (input.children.size < 3) {
    global.fatal << "Direct sum operation takes at least 2 arguments, whereas this expression has "
    << input.children.size << " arguments. " << global.fatal;
  }
  const Expression& left = input[1];
  const Expression& right = input[2];
  std::string leftString;
  if (left.NeedsParenthesisForAddition()) {
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
  std::string rightString = right.NeedsParenthesisForAddition() ?
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
  if (!input.IsListStartingWithAtom(input.owner->opSequence())) {
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
  if (!input.IsMatrix()) {
    return false;
  }
  if (theFormat->flagUseLatex && !theFormat->flagUsePmatrix) {
    out << "\\left(";
  }
  if (!theFormat->flagUsePmatrix) {
    int numCols = input.GetNumCols();
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
  if (!input.StartsWith(input.owner->opDefine(), 3)) {
    return false;
  }
  std::string firstE  = input[1].toString(theFormat);
  std::string secondE = input[2].toString(theFormat);
  if (
    input[1].IsListStartingWithAtom(input.owner->opDefine()) ||
    input[1].IsListStartingWithAtom(input.owner->opGreaterThan()) ||
    input[1].IsListStartingWithAtom(input.owner->opGreaterThanOrEqualTo()) ||
    input[1].IsListStartingWithAtom(input.owner->opLessThan()) ||
    input[1].IsListStartingWithAtom(input.owner->opLessThanOrEqualTo())
  ) {
    out << "\\left(" << firstE << "\\right)";
  } else {
    out << firstE;
  }
  out << "=";
  if (input[2].IsListStartingWithAtom(input.owner->opDefine())) {
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
    !input.StartsWith(input.owner->opLog(), 2)
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
  if (!input.StartsWith(input.owner->opDifferential())) {
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
  if (!input.StartsWith(input.owner->opDifferential(), 3)) {
    return false;
  }
  bool needsParen = input[2].NeedsParenthesisForMultiplication() ||
  input[2].NeedsParenthesisForMultiplicationWhenSittingOnTheRightMost();
  if (input[2].StartsWith(input.owner->opDivide())) {
    needsParen = false;
  }
  bool rightNeedsParen = (!input[1].IsAtom()) && (!input[1].IsBuiltInTypE());
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
  if (!input.StartsWith(input.owner->opDifferentiate(), 3)) {
    return false;
  }
  out << "\\frac{\\text{d}} ";
  if (input[2].NeedsParenthesisForMultiplication()) {
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
  if (!input.StartsWith(input.owner->opDifferential(), 2)) {
    return false;
  }
  bool needsParen = (!input[1].IsAtom()) && (!input[1].IsBuiltInTypE());
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
  if (!input.StartsWith(input.owner->opFactorial(), 2)) {
    return false;
  }
  if (input[1].NeedsParenthesisForBaseOfExponent()) {
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
  if (!input.StartsWith(input.owner->opSqrt())) {
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
  if (!input.StartsWith(input.owner->opSqrt(), 2)) {
    return false;
  }
  out << "\\sqrt{" << input[1].toString(theFormat) << "}";
  return true;
}

bool Expression::toStringSqrt3(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.StartsWith(input.owner->opSqrt(), 3)) {
    return false;
  }
  int thePower = 0;
  bool hasPowerTwo = input[1].IsSmallInteger(&thePower);
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
  if (!input.IsListStartingWithAtom(input.owner->opGreaterThan())) {
    return false;
  }
  out << input[1].toString(theFormat) << "&gt;" << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringLessThanOrEqualTo(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.IsListStartingWithAtom(input.owner->opLessThanOrEqualTo())) {
    return false;
  }
  out << input[1].toString(theFormat) << "\\leq " << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringLessThan(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.IsListStartingWithAtom(input.owner->opLessThan())) {
    return false;
  }
  out << input[1].toString(theFormat) << "&lt;" << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringLimitProcess(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.IsListStartingWithAtom(input.owner->opLimitProcess())) {
    return false;
  }
  out << input[1].toString(theFormat) << " \\to " << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringGreaterThanOrEqualTo(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.IsListStartingWithAtom(input.owner->opGreaterThanOrEqualTo())) {
    return false;
  }
  out << input[1].toString(theFormat) << "\\geq " << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringError(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.StartsWith(input.owner->opError(), 2)) {
    return false;
  }
  input.CheckInitialization();
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
    !input.StartsWith(input.owner->opSum()) &&
    !input.StartsWith(input.owner->opIntegral())
  ) {
    return false;
  }
  std::string opString = input[0].toString(theFormat);
  out << opString << " ";
  int firstIndex = 2;
  if (input.size() >= 2) {
    if (input[1].StartsWith(input.owner->opLimitBoundary(), 3)) {
      out
      << "_{" << input[1][1].toString(theFormat) << "}"
      << "^{" << input[1][2].toString(theFormat) << "}";
    } else if (input[1].IsOperationGiven(input.owner->opIndefiniteIndicator())) {
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
  if (!input.StartsWith(input.owner->opLimit(), 3)) {
    return false;
  }
  out << "\\lim_{";
  if (!input[1].IsSequenceNElementS()) {
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
  if (!input.IsListStartingWithAtom(input.owner->opUnion()) || input.size() != 3) {
    return false;
  }
  if (input[1].StartsWith(input.owner->opIntersection())) {
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
  if (!input.IsListStartingWithAtom(input.owner->opUnionNoRepetition())) {
    return false;
  }
  out << input[1].toString(theFormat) << "\\sqcup " << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringIntersection(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.IsListStartingWithAtom(input.owner->opIntersection()) || input.size() != 3) {
    return false;
  }
  if (input[1].StartsWith(input.owner->opUnion())) {
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
    theFormat = &tempFormat.GetElement();
    theFormat->flagExpressionIsFinal = true;
  }
  isFinal = theFormat->flagExpressionIsFinal;
  outTrue << "<table class =\"tableCalculatorOutput\">";
  outTrue << "<tr><th>Input</th><th>Result</th></tr>";
  if (this->IsListStartingWithAtom(this->owner->opEndStatement())) {
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
        this->IsOfType<std::string>() ||
        this->IsOfType<Plot>() ||
        this->IsOfType<SemisimpleSubalgebras>() ||
        this->IsOfType<WeylGroupData>()
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
  if (!input.StartsWith(input.owner->opMinus(), 3)) {
    return false;
  }
  if (input.children.size != 3) {
    global.fatal << "This is a programming error: "
    << "the minus function expects 1 or 2 arguments, "
    << "instead there are " << input.children.size - 1
    << ". " << global.fatal;
  }
  out << input[1].toString(theFormat) << "-";
  if (input[2].StartsWith(input.owner->opPlus()) || input[2].StartsWith(input.owner->opMinus())) {
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
  if (!input.StartsWith(input.owner->opMinus())) {
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
  if (!input.StartsWith(input.owner->opMinus(), 2)) {
    return false;
  }
  if (
    input[1].StartsWith(input.owner->opPlus()) ||
    input[1].StartsWith(input.owner->opMinus())
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
  if (!input.StartsWith(input.owner->opOr(), 3)) {
    return false;
  }
  if (!input[1].IsSequenceNElementS()) {
    out << "\\left(" << input[1] << "\\right)";
  } else {
    out << input[1];
  }
  out << "\\ or\\ ";
  if (!input[2].IsSequenceNElementS()) {
    out << "\\left(" << input[2] << "\\right)";
  } else {
    out << input[2];
  }
  return true;
}

bool Expression::toStringIntervalOpen(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.StartsWith(input.owner->opIntervalOpen(), 3)) {
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
  if (input.StartsWith(input.owner->opIntervalLeftClosed(), 2)) {
    if (input[1].IsSequenceNElementS(2)) {
      out << "\\left[" << input[1][1] << ", " << input[1][2] << "\\right)";
    } else {
      out << input[0].toString(theFormat) << "{}" << input[1].toString(theFormat);
    }
    return true;
  }
  if (input.StartsWith(input.owner->opIntervalLeftClosed(), 3)) {
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
    input.StartsWith(input.owner->opIntervalRightClosed(), 3)
  ) {
    out << "\\left(" << input[1].toString(theFormat) << ", "
    << input[2].toString(theFormat) << "\\right]";
    return true;
  }
  if (input.StartsWith(input.owner->opIntervalRightClosed(), 2)) {
    if (input[1].IsSequenceNElementS(2)) {
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
  if (input.StartsWith(input.owner->opIntervalClosed(), 2)) {
    if (input[1].IsSequenceNElementS(2)) {
      out << "\\left[" << input[1][1] << ", " << input[1][2] << "\\right]";
    } else {
      out << input[0].toString(theFormat) << "{}" << input[1].toString(theFormat);
    }
    return true;
  }
  if (input.StartsWith(input.owner->opIntervalClosed(), 3)) {
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
  if (!input.StartsWith(input.owner->opQuote(), 2)) {
    return false;
  }
  std::string theString;
  if (input[1].IsOperation(&theString)) {
    out << "\"" << theString << "\"";
  } else {
    out << "(Corrupt string)";
  }
  return true;
}

bool Expression::toStringLogBase(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.StartsWith(input.owner->opLogBase(), 3)) {
    return false;
  }
  out << "\\log_{" << input[1].toString(theFormat) << "}"
  << "\\left(" << input[2].toString(theFormat) << "\\right)";
  return true;
}

bool Expression::toStringIsDenotedBy(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.IsListStartingWithAtom(input.owner->opIsDenotedBy())) {
    return false;
  }
  out << input[1].toString(theFormat) << "=:" << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringDefineConditional(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.StartsWith(input.owner->opDefineConditional(), 4)) {
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
  if (!input.StartsWith(input.owner->opTensor(), 3)) {
    return false;
  }
  input.toStringOpMultiplicative(out, "\\otimes", theFormat);
  return true;
}

bool Expression::toStringIn(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.StartsWith(input.owner->opIn(), 3)) {
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
  if (!input.StartsWith(input.owner->opAnd(), 3)) {
    return false;
  }
  out << "\\left(" << input[1] << "\\right)" << " and "
  << "\\left(" << input[2] << "\\right)" ;
  return true;
}

bool Expression::toStringBinom(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.StartsWith(input.owner->opBinom(), 3)) {
    return false;
  }
  out << "\\binom{" << input[1].toString(theFormat) << "}{ "
  << input[2].toString(theFormat) << "}";
  return true;
}

bool Expression::toStringUnderscore(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.StartsWith(input.owner->opUnderscore())) {
    return false;
  }
  out << "{" << input[1].toString(theFormat) << "}_{"
  << input[2].toString(theFormat) << "}";
  return true;
}

bool Expression::toStringSetMinus(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.StartsWith(input.owner->opSetMinus(), 3)) {
    return false;
  }
  out << input[1].toString(theFormat) << "\\setminus "
  << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringLimitBoundary(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.StartsWith(input.owner->opLimitBoundary(), 3)) {
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
  if (!input.StartsWith(input.owner->opCrossProduct())) {
    return false;
  }
  input.toStringOpMultiplicative(out, "\\times", theFormat);
  return true;
}

bool Expression::toStringAbsoluteValue(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.StartsWith(input.owner->opAbsoluteValue(), 2)) {
    return false;
  }
  out << "\\left|" << input[1].toString(theFormat) << "\\right|";
  return true;
}

bool Expression::toStringBind(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.StartsWith(input.owner->opBind(), 2)) {
    return false;
  }
  out << "{{" << input[1].toString(theFormat) << "}}";
  return true;
}

bool Expression::toStringMod(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.IsListStartingWithAtom(input.owner->opMod())) {
    return false;
  }
  out << input[1].toString(theFormat) << " mod " << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringLieBracket(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.IsListStartingWithAtom(input.owner->opLieBracket())) {
    return false;
  }
  out << "[" << input[1].toString(theFormat) << "," << input[2].toString(theFormat) << "]";
  return true;
}

bool Expression::toStringEqualEqual(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.IsListStartingWithAtom(input.owner->opEqualEqual())) {
    return false;
  }
  out << input[1].toString(theFormat) << "==" << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringEqualEqualEqual(
  const Expression& input, std::stringstream& out, FormatExpressions* theFormat
) {
  if (!input.IsListStartingWithAtom(input.owner->opEqualEqualEqual())) {
    return false;
  }
  out << input[1].toString(theFormat) << "===" << input[2].toString(theFormat);
  return true;
}

bool Expression::toStringWithAtomHandler(
  std::stringstream& out, FormatExpressions* theFormat
) const {
  MacroRegisterFunctionWithName("Expression::toStringWithAtomHandler");
  if (!this->IsLisT()) {
    return false;
  }
  if (!(*this)[0].IsAtom()) {
    return false;
  }
  int atom = (*this)[0].theData;
  if (this->owner->toStringHandlersAtoms.Contains(atom)) {
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
  if (!this->IsLisT()) {
    return false;
  }
  if (!(*this)[0].IsLisT()) {
    return false;
  }
  if (!(*this)[0][0].IsAtom()) {
    return false;
  }
  int atom = (*this)[0][0].theData;
  if (this->owner->toStringHandlersComposite.Contains(atom)) {
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
    theFormat = &tempFormat.GetElement();
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
  this->CheckConsistency();
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
    !this->IsOfType<std::string>() &&
    !this->StartsWith(this->owner->opEndStatement())
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

std::string Expression::Lispify() const {
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
    out << " " << (*this)[i].Lispify();
  }
  out << ")";
  return out.str();
}

bool Expression::IsLisT() const {
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

bool Expression::IsFrozen() const {
  if (!this->StartsWith()) {
    return false;
  }
  return (*this)[0].IsAtomThatFreezesArguments();
}

bool Expression::IsAtomThatFreezesArguments(std::string* outputWhichAtom) const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->IsLisT()) {
    return false;
  }
  if (this->theData < 0 || this->theData >= this->owner->GetOperations().size) {
    return false;
  }
  if (outputWhichAtom != nullptr) {
    *outputWhichAtom = this->owner->GetOperations()[this->theData];
  }
  return this->owner->atomsThatFreezeArguments.Contains(this->owner->GetOperations()[this->theData]);
}

bool Expression::IsPowerOfAtomWhoseExponentsAreInterpretedAsFunction() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (!this->StartsWith(this->owner->opThePower(), 3)) {
    return false;
  }
  return (*this)[1].IsAtomWhoseExponentsAreInterpretedAsFunction();
}

bool Expression::IsAtomWhoseExponentsAreInterpretedAsFunction(std::string* outputWhichAtom) const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->IsLisT()) {
    return false;
  }
  if (this->theData < 0 || this->theData >= this->owner->GetOperations().size) {
    return false;
  }
  if (outputWhichAtom != nullptr) {
    *outputWhichAtom = this->owner->GetOperations()[this->theData];
  }
  return this->owner->atomsWhoseExponentsAreInterpretedAsFunctions.Contains(this->owner->GetOperations()[this->theData]);
}

bool Expression::IsAtomNotInterpretedAsFunction(std::string* outputWhichAtom) const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->IsLisT()) {
    return false;
  }
  if (this->theData < 0 || this->theData>= this->owner->GetOperations().size) {
    return false;
  }
  if (outputWhichAtom != nullptr) {
    *outputWhichAtom = this->owner->GetOperations()[this->theData];
  }
  return this->owner->atomsNotInterpretedAsFunctions.Contains(this->owner->GetOperations()[this->theData]);
}

bool Expression::IsAtom() const {
  if (this->owner == nullptr) {
    return false;
  }
  return !this->IsLisT();
}

bool Expression::IsOperation(std::string* outputWhichOperation) const {
  int index = - 1;
  if (!this->IsOperation(index)) {
    return false;
  }
  if (outputWhichOperation != nullptr) {
    *outputWhichOperation = this->owner->operations.theKeys[index];
  }
  return true;
}

bool Expression::IsOperation(int& outputWhichOperationIndex) const {
  if (!this->IsAtom()) {
    return false;
  }
  if (this->theData < 0 || this->theData >= this->owner->operations.size()) {
    return false;
  }
  outputWhichOperationIndex = this->theData;
  return true;
}

bool Expression::IsOperationGiven(const std::string& desiredAtom) const {
  if (this->IsLisT()) {
    return false;
  }
  if (this->owner == nullptr) {
    return false;
  }
  return this->theData == this->owner->operations.getIndex(desiredAtom);
}

bool Expression::IsOperationGiven(int desiredDataUseMinusOneForAny) const {
  if (this->IsLisT()) {
    return false;
  }
  if (desiredDataUseMinusOneForAny == - 1) {
    return true;
  }
  return this->theData == desiredDataUseMinusOneForAny;
}

bool Expression::IsKnownFunctionWithComplexRange(std::string* outputWhichOperation) const {
  if (this->owner == nullptr) {
    return false;
  }
  std::string operationName;
  if (outputWhichOperation == nullptr) {
    outputWhichOperation = &operationName;
  }
  if (!this->IsBuiltInAtom(outputWhichOperation)) {
    return false;
  }
  return this->owner->knownFunctionsWithComplexRange.Contains(operationName);
}

bool Expression::IsCalculatorStatusChanger() const {
  if (this->owner == nullptr) {
    return false;
  }
  return
  this->StartsWith(this->owner->opDefine()) ||
  this->StartsWith(this->owner->opDefineConditional()) ||
  this->StartsWith(this->owner->opRulesOff()) ||
  this->StartsWith(this->owner->opRulesOn());
}

bool Expression::IsArithmeticOperation(std::string* outputWhichOperation) const {
  if (this->owner == nullptr) {
    return false;
  }
  std::string operationName;
  if (outputWhichOperation == nullptr) {
    outputWhichOperation = &operationName;
  }
  if (!this->IsBuiltInAtom(outputWhichOperation)) {
    return false;
  }
  return this->owner->arithmeticOperations.Contains(operationName);
}

bool Expression::IsBuiltInAtom(std::string* outputWhichOperation) const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->IsLisT()) {
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

bool Expression::IsGoodForChainRuleFunction(std::string* outputWhichOperation) const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->IsLisT()) {
    return false;
  }
  if (this->theData < 0 || this->theData >= this->owner->GetOperations().size) {
    return false;
  }
  if (outputWhichOperation != nullptr) {
    *outputWhichOperation = this->owner->GetOperations()[this->theData];
  }
  return !this->owner->atomsNotAllowingChainRule.Contains(this->owner->GetOperations()[this->theData]);
}

bool Expression::RemoveContext() {
  this->CheckInitialization();
  if (!this->hasContext()) {
    return true;
  }
  this->children.RemoveIndexShiftDown(1);
  if (this->children.size == 1) {
    this->AssignMeMyChild(0);
  }
  return true;
}

bool Expression::hasContext() const {
  this->CheckInitialization();
  if (!this->IsBuiltInTypE() || !(this->size() == 3)) {
    return false;
  }
  return (*this)[1].IsListStartingWithAtom(this->owner->opContext());
}

bool Expression::hasNonEmptyContext() const {
  if (!this->hasContext()) {
    return false;
  }
  return !this->GetContext().isEmpty();
}

bool Expression::IsCacheableExpression() const {
  MacroRegisterFunctionWithName("Expression::IsCacheableExpression");
  if (this->owner == nullptr) {
    return true;
  }
  if (this->IsBuiltInTypE()) {
    return true;
  }
  for (int i = 0; i < this->children.size; i ++) {
    if (!(*this)[i].IsCacheableExpression()) {
      return false;
    }
  }
  if (this->children.size > 0) {
    return true;
  }
  Calculator& theBoss = *this->owner;
  if (this->theData < 0 || this->theData >= theBoss.operations.size()) {
    global.fatal << "This is a programming error: corrupted atom in Expression::IsCacheableExpression. " << global.fatal;
  }
  return theBoss.atomsThatMustNotBeCached.Contains(theBoss.operations.theKeys[this->theData]);
}

bool Expression::IsBuiltInScalar() const {
  return this->IsOfType<Rational>() || this->IsOfType<Polynomial<Rational> >() ||
  this->IsOfType<RationalFunction>() || this->IsOfType<double>() || this->IsOfType<AlgebraicNumber>();
}

bool Expression::IsElementaryObject() const {
  if (this->IsAtom()) {
    return true;
  }
  if (this->IsBuiltInTypE()) {
    return true;
  }
  if (this->owner == nullptr) {
    return true;
  }
  if (this->StartsWith(this->owner->opBind())) {
    return true;
  }
  return false;
}

bool Expression::IsBuiltInTypE(std::string* outputWhichOperation) const {
  std::string tempS;
  if (!this->StartsWith()) {
    return false;
  }
  if (!(*this)[0].IsOperation(&tempS)) {
    return false;
  }
  if (this->owner->getBuiltInTypes().Contains(tempS)) {
    if (outputWhichOperation != nullptr) {
      *outputWhichOperation = tempS;
    }
    return true;
  }
  return false;
}

bool Expression::IsBuiltInTypE(int* outputWhichType) const {
  std::string theType;
  if (!this->IsBuiltInTypE(&theType)) {
    return false;
  }
  if (outputWhichType != nullptr) {
    *outputWhichType = this->owner->GetOperations().getIndex(theType);
  }
  return true;
}

bool Expression::MakeProducT(Calculator& owner, const Expression& left, const Expression& right) {
  return this->MakeXOX(owner, owner.opTimes(), left, right);
}

bool Expression::MakeProducT(Calculator& owner, const List<Expression>& theMultiplicands) {
  if (theMultiplicands.size == 0) {
    return this->AssignValue(1, owner);
  }
  return this->MakeXOXOdotsOX(owner, owner.opTimes(), theMultiplicands);
}

bool Expression::makeSum(Calculator& owner, const List<Expression>& theSummands) {
  MacroRegisterFunctionWithName("Expression::MakeSum");
  if (theSummands.size == 0) {
    return this->AssignValue(0, owner);
  }
  return this->MakeXOXOdotsOX(owner, owner.opPlus(), theSummands);
}

bool Expression::MakeOXdotsX(Calculator& owner, int theOp, const List<Expression>& theOpands) {
  MacroRegisterFunctionWithName("Expression::MakeOXdotsX");
  if (theOpands.size == 0) {
    global.fatal << "Zero opands not allowed at this point. " << global.fatal;
  }
  if (theOpands.size == 1) {
    *this = theOpands[0];
    return true;
  }
  *this = *theOpands.LastObject();
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
  variableWrapper.MakeAtom(variable, *this->owner);
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
  converted.MakeAtom(polynomialVariable, *this->owner);
  this->variables.addOnTop(converted);
}

bool Expression::MakeSqrt(Calculator& owner, const Rational& argument, const Rational& radicalSuperIndex) {
  MacroRegisterFunctionWithName("Expression::MakeSqrt");
  Expression argumentE;
  argumentE.AssignValue(argument, owner);
  return this->MakeSqrt(owner, argumentE, radicalSuperIndex);
}

bool Expression::MakeSqrt(Calculator& owner, const Expression& argument, const Rational& radicalSuperIndex) {
  MacroRegisterFunctionWithName("Expression::MakeSqrt");
  this->reset(owner,3);
  Expression radicalIndexE;
  radicalIndexE.AssignValue(radicalSuperIndex, owner);
  this->AddChildAtomOnTop(owner.opSqrt());
  this->AddChildOnTop(radicalIndexE);
  return this->AddChildOnTop(argument);
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
    rightCopy.AssignValue(right.theData, *left.owner);
    return this->MakeXOX(owner, theOp, left, rightCopy);
  }
 if (left.owner == nullptr) {
    Expression leftCopy;
    leftCopy.AssignValue(left.theData, *right.owner);
    return this->MakeXOX(owner, theOp, leftCopy, right);
  }
  left.CheckInitialization();
  right.CheckInitialization();
  this->reset(owner, 3);
  this->theData = owner.opLisT();
  this->AddChildAtomOnTop(theOp);
  this->AddChildOnTop(left);
  return this->AddChildOnTop(right);
}

bool Expression::MakeOX(Calculator& owner, int theOp, const Expression& opArgument) {
  if (&opArgument == this) {
    Expression copyE = opArgument;
    return this->MakeOX(owner, theOp, copyE);
  }
  this->reset(owner);
  this->theData = owner.opLisT();
  this->AddChildAtomOnTop(theOp);
  return this->AddChildOnTop(opArgument);
}

bool Expression::Sequencefy() {
  this->CheckInitialization();
  if (this->IsSequenceNElementS()) {
    return true;
  }
  return this->MakeOX(*this->owner, this->owner->opSequence(), *this);
}

bool Expression::operator==(int other) const {
  if (this->owner == nullptr) {
    return false;
  }
  int theInt;
  if (!this->IsSmallInteger(&theInt)) {
    return false;
  }
  return theInt == other;
}

bool Expression::operator==(const std::string& other) const {
  return this->IsOperationGiven(other);
}

std::string Expression::toUTF8String(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("Expression::toUTF8String");
  if (this->owner == nullptr) {
    return this->toString(theFormat);
  }
  std::stringstream out;
  Rational theRat;
  if (this->IsOfType<Rational>(&theRat)) {
    FormatExpressions tempFormat;
    tempFormat.flagUseFrac = false;
    return theRat.toString(&tempFormat);
  } else if (this->IsOperationGiven(this->owner->opPi())) {
    return "\\u03C0";
  } else if (this->StartsWith(this->owner->opPlus(), 3)) {
    return (*this)[1].toUTF8String(theFormat) + "+" + (*this)[2].toUTF8String(theFormat);
  } else if (this->StartsWith(this->owner->opTimes(), 3)) {
    std::string secondUTF8String = (*this)[2].toUTF8String(theFormat);
    std::string secondString = (*this)[2].toString(theFormat);
    if ((*this)[1].IsOperationGiven(this->owner->opSqrt())) {
      out << "sqrt(" << secondUTF8String << ")";
    } else {
      std::string firstE = (*this)[1].toUTF8String(theFormat);
      bool firstNeedsBrackets = (*this)[1].NeedsParenthesisForMultiplication();
      bool secondNeedsBrackets = (*this)[2].NeedsParenthesisForMultiplicationWhenSittingOnTheRightMost(&((*this)[1]));
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
  } else if (this->StartsWith(this->owner->opSequence())) {
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
