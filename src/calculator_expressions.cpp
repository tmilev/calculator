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

static ProjectInformationInstance projectInfoCalculatorExpressionsCPP(__FILE__, "Calculator expression implementation. ");

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
List<Expression>::OrderLeftGreaterThanRight
FormatExpressions::GetMonOrder<Expression>() {
  return nullptr;
}

//Expression::GetTypeOperation specializations follow
template < >
int Expression::GetTypeOperation<ElementEllipticCurve<Rational> >() const {
  this->CheckInitialization();
  return this->owner->opEllipticCurveElementsRational();
}

template < >
int Expression::GetTypeOperation<ElementEllipticCurve<ElementZmodP> >() const {
  this->CheckInitialization();
  return this->owner->opEllipticCurveElementsZmodP();
}

template < >
int Expression::GetTypeOperation<RationalFunctionOld>() const {
  this->CheckInitialization();
  return this->owner->opRationalFunction();
}

template < >
int Expression::GetTypeOperation<InputBox>() const {
  this->CheckInitialization();
  return this->owner->opUserInputTextBox();
}

template < >
int Expression::GetTypeOperation<Rational>() const {
  this->CheckInitialization();
  return this->owner->opRational();
}

template < >
int Expression::GetTypeOperation<ElementHyperoctahedralGroupR2>() const {
  this->CheckInitialization();
  return this->owner->opElementHyperOctahedral();
}

template < >
int Expression::GetTypeOperation<GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> >() const {
  this->CheckInitialization();
  return this->owner->opHyperoctahedralGroupRep();
}

template < >
int Expression::GetTypeOperation<std::string>() const {
  this->CheckInitialization();
  return this->owner->opString();
}

template < >
int Expression::GetTypeOperation<AlgebraicNumber>() const {
  this->CheckInitialization();
  return this->owner->opAlgNumber();
}

template < >
int Expression::GetTypeOperation<ElementZmodP>() const {
  this->CheckInitialization();
  return this->owner->opEltZmodP();
}

template < >
int Expression::GetTypeOperation<ElementUniversalEnveloping<RationalFunctionOld> >() const {
  this->CheckInitialization();
  return this->owner->opElementUEoverRF();
}

template < >
int Expression::GetTypeOperation<Weight<Rational> >() const {
  this->CheckInitialization();
  return this->owner->opWeightLieAlg();
}

template < >
int Expression::GetTypeOperation<Weight<Polynomial<Rational> > >() const {
  this->CheckInitialization();
  return this->owner->opWeightLieAlgPoly();
}

template < >
int Expression::GetTypeOperation<MatrixTensor<Rational> >() const {
  this->CheckInitialization();
  return this->owner->opMatTensorRat();
}

template < >
int Expression::GetTypeOperation<int>() const {
  this->CheckInitialization();
  return this->owner->opRational();
}

template < >
int Expression::GetTypeOperation<LargeIntegerUnsigned>() const {
  this->CheckInitialization();
  return this->owner->opRational();
}

template < >
int Expression::GetTypeOperation<double>() const {
  this->CheckInitialization();
  return this->owner->opDouble();
}

template < >
int Expression::GetTypeOperation<Polynomial<Rational> >() const {
  this->CheckInitialization();
  return this->owner->opPoly();
}

template < >
int Expression::GetTypeOperation<Polynomial<AlgebraicNumber> >() const {
  this->CheckInitialization();
  return this->owner->opPolyOverANs();
}

template < >
int Expression::GetTypeOperation<ElementWeylAlgebra<Rational> >() const {
  this->CheckInitialization();
  return this->owner->opElementWeylAlgebra();
}

template < >
int Expression::GetTypeOperation<SemisimpleLieAlgebra>() const {
  this->CheckInitialization();
  return this->owner->opSSLieAlg();
}

template < >
int Expression::GetTypeOperation<LittelmannPath>() const {
  this->CheckInitialization();
  return this->owner->opLittelmannPath();
}

template < >
int Expression::GetTypeOperation<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >() const {
  this->CheckInitialization();
  return this->owner->opLRO();
}

template < >
int Expression::GetTypeOperation<ElementTensorsGeneralizedVermas<RationalFunctionOld> >() const {
  this->CheckInitialization();
  return this->owner->opElementTensorGVM();
}

template < >
int Expression::GetTypeOperation<charSSAlgMod<Rational> >() const {
  this->CheckInitialization();
  return this->owner->opCharSSAlgMod();
}

template < >
int Expression::GetTypeOperation<Plot>() const {
  this->CheckInitialization();
  return this->owner->opCalculusPlot();
}

template < >
int Expression::GetTypeOperation<PlotObject>() const {
  this->CheckInitialization();
  return this->owner->opCalculusPlot();
}

template < >
int Expression::GetTypeOperation<SemisimpleSubalgebras>() const {
  this->CheckInitialization();
  return this->owner->opSemisimpleSubalgebras();
}

template < >
int Expression::GetTypeOperation<WeylGroupData>() const {
  this->CheckInitialization();
  return this->owner->opWeylGroup();
}

template < >
int Expression::GetTypeOperation<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >() const {
  this->CheckInitialization();
  return this->owner->opWeylGroupRep();
}

template < >
int Expression::GetTypeOperation<ElementWeylGroup>() const {
  this->CheckInitialization();
  return this->owner->opWeylGroupElement();
}

template < >
int Expression::GetTypeOperation<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >() const {
  this->CheckInitialization();
  return this->owner->opWeylGroupVirtualRep();
}

//Expression::GetTypeOperation specializations end.
//Expression::ConvertsToType specializations follow.
template < >
bool Expression::ConvertsToType(
ElementSemisimpleLieAlgebra<AlgebraicNumber>* whichElement
) const {
  MacroRegisterFunctionWithName("Expression::ConvertsToType");
  if (this->owner == nullptr) {
    return false;
  }
  ElementUniversalEnveloping<RationalFunctionOld> theUEElt;
  if (!this->IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >(&theUEElt)) {
    return false;
  }
  ElementSemisimpleLieAlgebra<Rational> theRatElt;
  if (!theUEElt.GetLieAlgebraElementIfPossible(theRatElt)) {
    return false;
  }
  if (whichElement != nullptr) {
    *whichElement = theRatElt;
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
  return this->owner->theObjectContainer.theSSSubalgebraS.GetIndex(inputValue.owner->theWeyl.theDynkinType);
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
  .GetIndex(inputValue.name);
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
RationalFunctionOld
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
ElementUniversalEnveloping<RationalFunctionOld>
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
  return this->owner->theObjectContainer.thePolys
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
Polynomial<AlgebraicNumber>
& inputValue) const {
  this->CheckInitialization();
  return this->owner->theObjectContainer.thePolysOverANs
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
int Expression::AddObjectReturnIndex(const
SemisimpleLieAlgebra
& inputValue) const {
  this->CheckInitialization();
  if (!this->owner->theObjectContainer.theSSLieAlgebras.Contains(inputValue.theWeyl.theDynkinType)) {
    global.fatal << "Semisimple Lie algebra must be allocated directly in the object container. " << global.fatal;
  }
  return this->owner->theObjectContainer.theSSLieAlgebras.GetIndex(inputValue.theWeyl.theDynkinType);
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
ElementTensorsGeneralizedVermas<RationalFunctionOld>
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
  return this->owner->theObjectContainer.theSSLieAlgebras.GetIndex(inputValue.theDynkinType);
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
    << "The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theRationals.GetElement(this->GetLastChild().theData);
}

template < >
ElementEllipticCurve<Rational>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<ElementEllipticCurve<Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type Rational. "
    << "The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.EllipticCurveElementsRational.GetElement(this->GetLastChild().theData);
}

template < >
ElementEllipticCurve<ElementZmodP>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<ElementEllipticCurve<ElementZmodP> >()) {
    global.fatal << "This is a programming error: expression not of required type Rational. The expression equals "
    << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.EllipticCurveElementsZmodP.GetElement(this->GetLastChild().theData);
}

template < >
ElementHyperoctahedralGroupR2& Expression::GetValueNonConst() const {
  if (!this->IsOfType<ElementHyperoctahedralGroupR2>()) {
    global.fatal << "This is a programming error: expression not of required type Rational. "
    << "The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theElementsHyperOctGroup.GetElement(this->GetLastChild().theData);
}

template < >
InputBox& Expression::GetValueNonConst() const {
  if (!this->IsOfType<InputBox>()) {
    global.fatal << "This is a programming error: expression not of required type Rational. "
    << "The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theUserInputTextBoxesWithValues.theValues[this->GetLastChild().theData];
}

template < >
GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> & Expression::GetValueNonConst() const {
  if (!this->IsOfType<GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type Rational. "
    << "The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theHyperoctahedralReps[this->GetLastChild().theData];
}

template < >
ElementZmodP& Expression::GetValueNonConst() const {
  if (!this->IsOfType<ElementZmodP>())
    global.fatal << "This is a programming error: expression not of required type ElementZmodP. "
    << "The expression equals " << this->ToString() << "." << global.fatal;
  return this->owner->theObjectContainer.theEltsModP.GetElement(this->GetLastChild().theData);
}

template < >
AlgebraicNumber& Expression::GetValueNonConst() const {
  if (!this->IsOfType<AlgebraicNumber>()) {
    global.fatal << "This is a programming error: expression not of required type AlgebraicNumber. "
    << "The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theAlgebraicNumbers.GetElement(this->GetLastChild().theData);
}

template < >
double& Expression::GetValueNonConst() const {
  if (!this->IsOfType<double>()) {
    global.fatal << "This is a programming error: expression not of required type double. "
    << "The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theDoubles.GetElement(this->GetLastChild().theData);
}

template < >
std::string& Expression::GetValueNonConst() const {
  if (!this->IsOfType<std::string>()) {
    global.fatal << "This is a programming error: expression not of required type std::string. "
    << "The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theStrings.GetElement(this->GetLastChild().theData);
}

template < >
RationalFunctionOld& Expression::GetValueNonConst() const {
  if (!this->IsOfType<RationalFunctionOld>()) {
    global.fatal << "This is a programming error: expression not of required type RationalFunctionOld. "
    << "The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theRFs.GetElement(this->GetLastChild().theData);
}

template < >
ElementUniversalEnveloping<RationalFunctionOld>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "ElementUniversalEnveloping_RationalFunctionOld. The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theUEs.GetElement(this->GetLastChild().theData);
}

template < >
Polynomial<Rational>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<Polynomial<Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type Polynomial_Rational. "
    << "The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.thePolys.GetElement(this->GetLastChild().theData);
}

template < >
Polynomial<AlgebraicNumber>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<Polynomial<AlgebraicNumber> >()) {
    global.fatal << "This is a programming error: expression not of required type Polynomial_AlgebraicNumber. "
    << "The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.thePolysOverANs.GetElement(this->GetLastChild().theData);
}

template < >
ElementWeylAlgebra<Rational>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<ElementWeylAlgebra<Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "ElementWeylAlgebra_Rational. The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theWeylAlgebraElements.GetElement(this->GetLastChild().theData);
}

template < >
LittelmannPath& Expression::GetValueNonConst() const {
  if (!this->IsOfType<LittelmannPath>()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "LittelmannPath. The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theLSpaths.GetElement(this->GetLastChild().theData);
}

template < >
Weight<Polynomial<Rational> >& Expression::GetValueNonConst() const {
  if (!this->IsOfType<Weight<Polynomial<Rational> > >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "LittelmannPath. The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theWeightsPoly.GetElement(this->GetLastChild().theData);
}

template < >
MonomialTensor<int, MathRoutines::IntUnsignIdentity>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "MonomialTensor. The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theLittelmannOperators.GetElement(this->GetLastChild().theData);
}

template < >
ElementTensorsGeneralizedVermas<RationalFunctionOld>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >()) {
    global.fatal << "This is a programming error: expression not of "
    << "required type ElementTGVM_RationalFunctionOld. The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theTensorElts.GetElement(this->GetLastChild().theData);
}

template < >
charSSAlgMod<Rational>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<charSSAlgMod<Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "charSSAlgMod_Rational. The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theCharsSSLieAlgFD.GetElement(this->GetLastChild().theData);
}

template < >
SemisimpleLieAlgebra& Expression::GetValueNonConst() const {
  if (!this->IsOfType<SemisimpleLieAlgebra>()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "SemisimpleLieAlgebra. The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theSSLieAlgebras.theValues[this->GetLastChild().theData];
}

template < >
MatrixTensor<Rational>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<MatrixTensor<Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "MatrixTensorRational. The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theMatTensorRats.GetElement(this->GetLastChild().theData);
}

template < >
SemisimpleSubalgebras& Expression::GetValueNonConst() const {
  if (!this->IsOfType<SemisimpleSubalgebras>()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "SemisimpleSubalgebras. The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theSSSubalgebraS.theValues[this->GetLastChild().theData];
}

template < >
Plot& Expression::GetValueNonConst() const {
  if (!this->IsOfType<Plot>()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "Plot. The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.thePlots[this->GetLastChild().theData];
}

template < >
WeylGroupData& Expression::GetValueNonConst() const {
  if (!this->IsOfType<WeylGroupData>()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "WeylGroupData. The expression equals "
    << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theSSLieAlgebras.theValues[this->GetLastChild().theData].theWeyl;
}

template < >
GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "WeylGroupRepresentation_Rational. The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theWeylGroupReps.GetElement(this->GetLastChild().theData);
}

template < >
ElementWeylGroup& Expression::GetValueNonConst() const {
  if (!this->IsOfType<ElementWeylGroup>()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "ElementWeylGroup. The expression equals " << this->ToString() << "." << global.fatal;
  }
  return this->owner->theObjectContainer.theWeylGroupElements.GetElement(this->GetLastChild().theData);
}

template < >
VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& Expression::GetValueNonConst() const {
  if (!this->IsOfType<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >()) {
    global.fatal << "This is a programming error: expression not of required type "
    << "WeylGroupVirtualRepresentation. The expression equals "
    << this->ToString() << "." << global.fatal;
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
//start Expression::ConvertToType specializations.
template< >
bool Expression::ConvertToType<Polynomial<AlgebraicNumber> >(Expression& output) const {
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
bool Expression::ConvertToType<Polynomial<Rational> >(Expression& output) const {
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
bool Expression::ConvertToType<RationalFunctionOld>(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_RationalFunctionOld");
  this->CheckInitialization();
  if (this->IsOfType<Rational>()) {
    RationalFunctionOld resultRF;
    resultRF.MakeConst(this->GetValue<Rational>());
    return output.AssignValueWithContext(
      resultRF, this->GetContext(), *this->owner
    );
  }
  if (this->IsOfType<Polynomial<Rational> >()) {
    RationalFunctionOld resultRF;
    resultRF = this->GetValue<Polynomial<Rational> >();
    return output.AssignValueWithContext(
      resultRF, this->GetContext(), *this->owner
    );
  }
  if (this->IsOfType<RationalFunctionOld>()) {
    output = *this;
    return true;
  }
  (*this->owner)
  << "<hr>ConvertToType_RationalFunctionOld: Failed to convert "
  << this->ToString() << " to rational function. ";
  return false;
}

template< >
bool Expression::ConvertToType<ElementWeylAlgebra<Rational> >(Expression& output) const {
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
bool Expression::ConvertToType<Weight<Polynomial<Rational> > >(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_Weight_Polynomial");
  this->CheckInitialization();
  if (this->IsOfType<Weight<Polynomial<Rational> > >()) {
    output = *this;
    return true;
  }
  return false;
}

template< >
bool Expression::ConvertToType<ElementUniversalEnveloping<RationalFunctionOld> >(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_RationalFunctionOld");
  this->CheckInitialization();
  if (!this->IsBuiltInTypE())
    return false;
  if (this->IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >()) {
    output = *this;
    return true;
  }
  SemisimpleLieAlgebra* theOwner = this->GetAmbientSSAlgebraNonConstUseWithCaution();
  if (theOwner == nullptr) {
    this->owner->Comments << "<hr>Failed to convert " << this->ToString() << " (Lispified: " << this->ToStringFull()
    << ") to element of universal enveloping -  failed to extract ambient Lie algebra. ";
    return false;
  }
  ElementUniversalEnveloping<RationalFunctionOld> outputUE;
  if (this->IsOfType<Rational>()) {
    outputUE.MakeConst(this->GetValue<Rational>(), *theOwner);
    return output.AssignValueWithContext(outputUE, this->GetContext(), *this->owner);
  } else if (this->IsOfType<Polynomial<Rational> >()) {
    RationalFunctionOld tempRF;
    tempRF = this->GetValue<Polynomial<Rational> >();
    outputUE.MakeConst(tempRF, *theOwner);
    return output.AssignValueWithContext(outputUE, this->GetContext(), *this->owner);
  } else if (this->IsOfType<RationalFunctionOld>()) {
    outputUE.MakeConst(this->GetValue<RationalFunctionOld>(), *theOwner);
    return output.AssignValueWithContext(outputUE, this->GetContext(), *this->owner);
  }
  return *this->owner << "<hr>Failed to convert " << this->ToString()
  << " to element of universal enveloping -  I don't know how. ";
}

template< >
bool Expression::ConvertToType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_RationalFunctionOld");
  this->CheckInitialization();
  if (this->IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >()) {
    output = *this;
    return true;
  }
  return false;
}

template< >
bool Expression::ConvertToType<Rational>(Expression& output) const {
  MacroRegisterFunctionWithName("ConvertToType_Rational");
  this->CheckInitialization();
  if (this->IsOfType<Rational>()) {
    output = *this;
    return true;
  }
  return false;
}

template< >
bool Expression::ConvertToType<AlgebraicNumber>(Expression& output) const {
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
bool Expression::ConvertToType<double>(Expression& output) const {
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
  // warning: do not use ToString method from here: ToString itself calls CheckConosistency,
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
      << "children: type, context, and index in Calculator. The expression is " << this->ToStringFull()
      << global.fatal;
    }
    const Expression& mustBeTheContext = (*this)[1];
    if (!mustBeTheContext.StartsWith(this->owner->opContexT())) {
      global.fatal << "This is a programming error. At the moment of writing, "
      << "the second child of a built-in type must be a context. It is instead "
      << mustBeTheContext.ToStringFull() << global.fatal;
    }
    for (int i = 1; i < mustBeTheContext.children.size; i ++) {
      bool isGood = false;
      const Expression& currentE = mustBeTheContext[i];
      if (currentE.StartsWith(this->owner->opPolynomialVariables())) {
        isGood = true;
      }
      if (currentE.StartsWith(this->owner->opSSLieAlg())) {
        isGood = true;
      }
      if (currentE.StartsWith(this->owner->opWeylAlgebraVariables())) {
        isGood = true;
      }
      if (!isGood) {
        global.fatal << "This is a programming error. The context "
        << mustBeTheContext.ToStringFull() << " has an entry which I do not recognize, namely, "
        << currentE.ToStringFull() << ". " << global.fatal;
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

bool Expression::AddChildRationalOnTop(const Rational& inputRat) {
  this->CheckInitialization();
  Expression ratE;
  ratE.AssignValue(inputRat, *this->owner);
  return this->AddChildOnTop(ratE);
}

bool Expression::AddChildOnTop(const Expression& inputChild) {
  this->CheckInitialization();
  this->children.AddOnTop(
    this->owner->theExpressionContainer.AddNoRepetitionOrReturnIndexFirst(inputChild)
  );
  return true;
}

bool Expression::SetChildAtomValue(int childIndex, int TheAtomValue) {
  this->CheckInitialization();
  Expression tempE;
  tempE.MakeAtom(TheAtomValue, *this->owner);
  this->children.SetObjectAtIndex(
    childIndex, this->owner->theExpressionContainer.AddNoRepetitionOrReturnIndexFirst(tempE)
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
  int theOpIndex = this->owner->operations.GetIndex(theOperation);
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

int Expression::ContextGetNumContextVariables() const {
  if (this->owner == nullptr) {
    return 0;
  }
  if (!this->StartsWith(this->owner->opContexT())) {
    return 0;
  }
  for (int i = 1; i < this->children.size; i ++) {
    if ((*this)[i].StartsWith(this->owner->opPolynomialVariables())) {
      return (*this)[i].children.size - 1;
    }
  }
  return 0;
}

bool Expression::SetContextAtLeastEqualTo(Expression& inputOutputMinContext) {
  MacroRegisterFunctionWithName("Expression::SetContextAtLeastEqualTo");
  this->CheckInitialization();
  if (!this->IsBuiltInTypE()) {
    global.fatal << "This is a programming error: calling "
    << "Expression::SetContextAtLeastEqualTo on an expression "
    << "that is not of built-in type. "
    << "Contexts are Reserved for built-in data types. " << global.fatal;
  }
  if (!inputOutputMinContext.IsContext()) {
    this->owner->Comments << "<br>Warning: non-initialized input context in "
    << "Expression::SetContextAtLeastEqualTo. Stack trace: "
    << global.fatal.GetStackTraceEtcErrorMessageHTML();
    inputOutputMinContext.MakeEmptyContext(*this->owner);
  }
  Expression newContext;
  Expression myOldContext = this->GetContext();
  if (!this->ContextMergeContexts(myOldContext, inputOutputMinContext, newContext)) {
    return false;
  }
  if (myOldContext == newContext) {
    return true;
  }
  inputOutputMinContext = newContext;
  if (this->IsOfType<Rational>()) {
    this->SetChilD(1, inputOutputMinContext);
    return true;
  }
  if (this->IsOfType<ElementWeylGroup>()) {
    return this->AssignValueWithContext(
      this->GetValue<ElementWeylGroup>(), inputOutputMinContext, *this->owner
    );
  }
  if (this->IsOfType<AlgebraicNumber>()) {
    return this->SetChilD(1, inputOutputMinContext);
  }
  if (this->IsOfType<ElementUniversalEnveloping<RationalFunctionOld> > ()) {
    ElementUniversalEnveloping<RationalFunctionOld> newUE = this->GetValue<ElementUniversalEnveloping<RationalFunctionOld> >();
    PolynomialSubstitution<Rational> subPolyPart;
    myOldContext.ContextGetPolySubFromSuperContextNoFailure<Rational>(newContext, subPolyPart);
    newUE.Substitution(subPolyPart);
    return this->AssignValueWithContext(newUE, inputOutputMinContext, *this->owner);
  }
  if (this->IsOfType<Polynomial<Rational> >()) {
    Polynomial<Rational> newPoly = this->GetValue<Polynomial<Rational> >();
    PolynomialSubstitution<Rational> subPolyPart;
    myOldContext.ContextGetPolySubFromSuperContextNoFailure<Rational>(newContext, subPolyPart);
    if (!newPoly.Substitution(subPolyPart)) {
      return false;
    }
    return this->AssignValueWithContext(newPoly, inputOutputMinContext, *this->owner);
  }
  if (this->IsOfType<Polynomial<AlgebraicNumber> >()) {
    Polynomial<AlgebraicNumber> newPoly = this->GetValue<Polynomial<AlgebraicNumber> >();
    PolynomialSubstitution<AlgebraicNumber> subPolyPart;
    myOldContext.ContextGetPolySubFromSuperContextNoFailure<AlgebraicNumber>(newContext, subPolyPart);
    if (!newPoly.Substitution(subPolyPart)) {
      return false;
    }
    return this->AssignValueWithContext(newPoly, inputOutputMinContext, *this->owner);
  }
  if (this->IsOfType<ElementWeylAlgebra<Rational> >()) {
    PolynomialSubstitution<Rational> subEWApart;
    PolynomialSubstitution<Rational> subPolyPart;
    myOldContext.ContextGetPolyAndEWASubFromSuperContextNoFailure(newContext, subPolyPart, subEWApart);
    ElementWeylAlgebra<Rational> outputEWA = this->GetValue<ElementWeylAlgebra<Rational> >();
    if (!outputEWA.Substitution(subPolyPart, subEWApart)) {
      this->owner->Comments << "<hr>Failed to convert " << outputEWA.ToString() << ": failed to carry out substitution "
      << subEWApart.ToString() << ", " << subPolyPart.ToString();
      return false;
    }
    return this->AssignValueWithContext(outputEWA, inputOutputMinContext, *this->owner);
  }
  if (this->IsOfType<RationalFunctionOld>()) {
    RationalFunctionOld newRF = this->GetValue<RationalFunctionOld>();
    PolynomialSubstitution<Rational> subPolyPart;
    myOldContext.ContextGetPolySubFromSuperContextNoFailure(newContext, subPolyPart);
    newRF.Substitution(subPolyPart);
    return this->AssignValueWithContext(newRF, inputOutputMinContext, *this->owner);
  }
  if (this->IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >()) {
    ElementTensorsGeneralizedVermas<RationalFunctionOld> newETGV =
    this->GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();
    PolynomialSubstitution<Rational> subPolyPart;
    myOldContext.ContextGetPolySubFromSuperContextNoFailure(newContext, subPolyPart);
    newETGV.Substitution(subPolyPart, this->owner->theObjectContainer.theCategoryOmodules);
    return this->AssignValueWithContext(newETGV, inputOutputMinContext, *this->owner);
  }
  if (this->IsOfType<Weight<Polynomial<Rational> > >()) {
    PolynomialSubstitution<Rational> subPolyPart;
    myOldContext.ContextGetPolySubFromSuperContextNoFailure(newContext, subPolyPart);
    Weight<Polynomial<Rational> > theWeight = this->GetValue<Weight<Polynomial<Rational> > >();
    for (int i = 0; i < theWeight.weightFundamentalCoordS.size; i ++) {
      theWeight.weightFundamentalCoordS[i].Substitution(subPolyPart);
    }
    return this->AssignValueWithContext(theWeight, inputOutputMinContext, *this->owner);
  }
  if (this->IsMatrixOfType<RationalFunctionOld>()) {
    Matrix<RationalFunctionOld> newMat;
    this->owner->functionGetMatrix(*this, newMat, &newContext);
    PolynomialSubstitution<Rational> subPolyPart;
    myOldContext.ContextGetPolySubFromSuperContextNoFailure<Rational>(newContext, subPolyPart);
    for (int i = 0; i < newMat.NumRows; i ++) {
      for (int j = 0; j < newMat.NumCols; j ++) {
        if (!newMat(i, j).Substitution(subPolyPart)) {
          return *this->owner << "Failed to carry out the substitution "
          << subPolyPart.ToString() << " in the matrix " << this->ToString() << ". ";
        }
      }
    }
    return this->AssignMatrix(newMat, *this->owner, &inputOutputMinContext);
  }
  this->owner->Comments << "Expression " << this->ToString()
  << " is of built-in type but is not handled by Expression::SetContextAtLeastEqualTo. ";
  return false;
}

bool Expression::MergeContexts(Expression& leftE, Expression& rightE) {
  MacroRegisterFunctionWithName("Expression::MergeContexts");
  if (!leftE.HasContext() || !rightE.HasContext()) {
    return false;
  }
  Expression leftC = leftE.GetContext();
  Expression rightC = rightE.GetContext();
  Expression outputC;
  if (!leftC.ContextMergeContexts(leftC, rightC, outputC)) {
    return false;
  }
  if (!leftE.SetContextAtLeastEqualTo(outputC)) {
    return false;
  }
  return rightE.SetContextAtLeastEqualTo(outputC);
}

Expression Expression::ContextGetContextVariable(int variableIndex) const {
  MacroRegisterFunctionWithName("Expression::ContextGetContextVariable");
  Expression thePolVars = this->ContextGetPolynomialVariables();
  if (thePolVars.children.size <= variableIndex + 1) {
    Expression errorE;
    std::stringstream out;
    out << "Context ~does ~not ~have ~variable ~index ~" << variableIndex + 1 << ". ";
    errorE.MakeError(out.str(), *this->owner);
    return errorE;
  }
  return thePolVars[variableIndex + 1];
}

bool Expression::ContextSetSSLieAlgebrA(int indexInOwners, Calculator& owner) {
  if (!this->IsContext()) {
    global.fatal << "This is a programming error: calling Expression::ContextSetSSLieAlgebrA "
    << "on a non-context expression. " << global.fatal;
  }
  int index = this->ContextGetIndexAmbientSSalg();
  if (index != - 1 && index != indexInOwners) {
    return false;
  }
  if (index == indexInOwners) {
    return true;
  }
  Expression LieAlgContextE;
  Expression emptyContext;
  emptyContext.MakeEmptyContext(owner);
  LieAlgContextE.reset(owner, 3);
  LieAlgContextE.AddChildAtomOnTop(owner.opSSLieAlg());
  LieAlgContextE.AddChildOnTop(emptyContext);
  LieAlgContextE.AddChildAtomOnTop(indexInOwners);
  return this->AddChildOnTop(LieAlgContextE);
}

bool Expression::ContextSetDiffOperatorVar(const Expression& thePolyVar, const Expression& theDiffOpVar) {
  if (!this->IsContext()) {
    global.fatal << "This is a programming error: calling "
    << "Expression::ContextSetDiffOperatorVar on a non-context expression. " << global.fatal;
  }
  Expression diffVarsE, polyVarsE;
  diffVarsE.reset(*this->owner, 2);
  diffVarsE.AddChildAtomOnTop(this->owner->opWeylAlgebraVariables());
  diffVarsE.AddChildOnTop(theDiffOpVar);
  polyVarsE.reset(*this->owner, 2);
  polyVarsE.AddChildAtomOnTop(this->owner->opPolynomialVariables());
  polyVarsE.AddChildOnTop(thePolyVar);
  bool foundDiffVarsE = false;
  bool foundPolyVarsE = false;
  for (int i = 0; i < this->children.size; i ++) {
    if ((*this)[i].StartsWith(this->owner->opWeylAlgebraVariables())) {
      this->SetChilD(i, diffVarsE);
      foundDiffVarsE = true;
    } else if ((*this)[i].StartsWith(this->owner->opPolynomialVariables())) {
      this->SetChilD(i, polyVarsE);
      foundPolyVarsE = true;
    }
  }
  if (!foundPolyVarsE) {
    this->AddChildOnTop(polyVarsE);
  }
  if (!foundDiffVarsE) {
    this->AddChildOnTop(diffVarsE);
  }
  return true;
}

Expression Expression::ContextGetDifferentialOperatorVariables() const {
  this->CheckInitialization();
  return this->ContextGetContextType(this->owner->opWeylAlgebraVariables());
}

Expression Expression::ContextGetPolynomialVariables() const {
  this->CheckInitialization();
  return this->ContextGetContextType(this->owner->opPolynomialVariables());
}

Expression Expression::ContextGetContextType(int theType) const {
  MacroRegisterFunctionWithName("Expression::ContextGetPolynomialVariables");
  this->CheckInitialization();
  if (!this->IsContext()) {
    //global.fatal << "This is a programming error: calling Expression::ContextGetPolynomialVariables"
    //<< " on a non-context expression. "
    //<< global.fatal;
  }
  for (int i = 1; i < this->children.size; i ++) {
    if ((*this)[i].StartsWith(theType)) {
      return (*this)[i];
    }
  }
  Expression output;
  output.reset(*this->owner, 1);
  output.AddChildAtomOnTop(theType);
  return output;
}

bool Expression::ContextMergeContexts(const Expression& leftContext, const Expression& rightContext, Expression& outputContext) {
  MacroRegisterFunctionWithName("Expression::ContextMergeContexts");
  if (&leftContext == &outputContext || &rightContext == &outputContext) {
    Expression leftCopy = leftContext;
    Expression rightCopy = rightContext;
    return Expression::ContextMergeContexts(leftCopy, rightCopy, outputContext);
  }
  int leftSSindex = leftContext.ContextGetIndexAmbientSSalg();
  int rightSSindex = rightContext.ContextGetIndexAmbientSSalg();
  if (leftSSindex == - 1) {
    leftSSindex = rightSSindex;
  }
  if (rightSSindex == - 1) {
    rightSSindex = leftSSindex;
  }
  if (leftSSindex != rightSSindex) {
    return false;
  }
  Expression leftPolyV = leftContext.ContextGetPolynomialVariables();
  Expression rightPolyV = rightContext.ContextGetPolynomialVariables();
  HashedList<Expression> polyVarUnion;
  polyVarUnion.SetExpectedSize(leftPolyV.size() + rightPolyV.size() - 2);
  for (int i = 1; i < leftPolyV.size(); i ++) {
    polyVarUnion.AddOnTopNoRepetition(leftPolyV[i]);
  }
  for (int i = 1; i < rightPolyV.size(); i ++) {
    polyVarUnion.AddOnTopNoRepetition(rightPolyV[i]);
  }
  polyVarUnion.QuickSortAscending();
  Calculator& owner = *leftContext.owner;
  outputContext.reset(owner, 5 + polyVarUnion.size);
  outputContext.AddChildAtomOnTop(owner.opContexT());
  if (polyVarUnion.size > 0) {
    Expression polyVarsE;
    polyVarsE.reset(owner, polyVarUnion.size + 1);
    polyVarsE.AddChildAtomOnTop(owner.opPolynomialVariables());
    for (int i = 0; i < polyVarUnion.size; i ++) {
      polyVarsE.AddChildOnTop(polyVarUnion[i]);
    }
    outputContext.AddChildOnTop(polyVarsE);
  }
  //Converting differential operators if needed.
  Expression leftEWAV = leftContext.ContextGetDifferentialOperatorVariables();
  Expression rightEWAV = rightContext.ContextGetDifferentialOperatorVariables();
  if (leftEWAV.size() > 1 || rightEWAV.size() > 1) {
    Selection foundEWAVar;
    List<Expression> EWAVars;
    foundEWAVar.init(polyVarUnion.size);
    EWAVars.SetSize(polyVarUnion.size);
    Expression* currentPolyV = &leftPolyV;
    Expression* currentEWAV = &leftEWAV;
    for (int k = 0; k < 2; k ++, currentPolyV = &rightPolyV, currentEWAV = &rightEWAV) {
      for (int i = 1; i < currentEWAV->size(); i ++) {
        int theIndex = polyVarUnion.GetIndex((*currentPolyV)[i]);
        if (foundEWAVar.selected[theIndex]) {
          if ((*currentEWAV)[i] != EWAVars[theIndex]) {
            return owner << "<hr>Failed to merge contexts " << leftContext.ToString() << " and "
            << rightContext.ToString()
            << " because " << (*currentPolyV)[i].ToString()
            << " has two different corresponding differential operator variables: "
            << EWAVars[theIndex].ToString() << " and " << (*currentEWAV)[i].ToString();
          }
        }
        foundEWAVar.AddSelectionAppendNewIndex(theIndex);
        EWAVars[theIndex] = (*currentEWAV)[i];
      }
    }
    for (int i = 0; i < foundEWAVar.MaxSize; i ++) {
      if (!foundEWAVar.selected[i]) {
        Expression currentEWAVar;
        currentEWAVar.reset(owner, 2);
        currentEWAVar.AddChildAtomOnTop(owner.AddOperationNoRepetitionOrReturnIndexFirst("\\partial"));
        Expression indexE;
        indexE.AssignValue(i, owner);
        currentEWAVar.AddChildOnTop(indexE);
        if (EWAVars.Contains(currentEWAVar)) {
          return owner << "<hr>Failed to merge contexts " << leftContext.ToString() << " and " << rightContext.ToString()
          << ": " << polyVarUnion[i].ToString()
          << " had no differential letter assigned to it. I tried to assign automatically  "
          << currentEWAVar.ToString() << " as differential operator letter, but it was already taken. ";
        }
      }
    }
    Expression diffVarsE;
    diffVarsE.reset(owner, polyVarUnion.size + 1);
    diffVarsE.AddChildAtomOnTop(owner.opWeylAlgebraVariables());
    for (int i = 0; i < EWAVars.size; i ++) {
      diffVarsE.AddChildOnTop(EWAVars[i]);
    }
    outputContext.AddChildOnTop(diffVarsE);
  }
  if (leftSSindex != - 1) {
    Expression ssAlgE;
    Expression emptyContext;
    emptyContext.MakeEmptyContext(owner);
    ssAlgE.reset(owner, 2);
    ssAlgE.AddChildAtomOnTop(owner.opSSLieAlg());
    ssAlgE.AddChildOnTop(emptyContext);
    ssAlgE.AddChildAtomOnTop(leftSSindex);
    outputContext.AddChildOnTop(ssAlgE);
  }
  return true;
}

void Expression::ContextGetFormatExpressions(FormatExpressions& output) const {
  output.polyAlphabeT.SetSize(0);
  output.polyDefaultLetter = "x";
  if (this->owner == nullptr) {
    return;
  }
  if (!this->StartsWith(this->owner->opContexT())) {
    return;
  }
  Expression thePolyE = this->ContextGetPolynomialVariables();
  output.polyAlphabeT.SetSize(thePolyE.children.size - 1);
  for (int i = 1; i < thePolyE.children.size; i ++) {
    output.polyAlphabeT[i - 1] = thePolyE[i].ToString();
  }
  Expression theEWAE = this->ContextGetDifferentialOperatorVariables();
  output.weylAlgebraLetters.SetSize(theEWAE.children.size - 1);
  for (int i = 1; i < theEWAE.size(); i ++) {
    output.weylAlgebraLetters[i - 1] = theEWAE[i].ToString();
  }
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
    outputAccumulateLeafs.AddOnTopNoRepetition(*this);
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
    outputAccumulateBoundVariables.AddOnTopNoRepetition((*this)[1]);
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
      outputAccumulateFreeVariables.AddOnTopNoRepetition(*this);
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
  return this->StartsWith(this->owner->opContexT());
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

bool Expression::IsMonEqualToZero() const {
  return this->IsEqualToZero();
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
  if (this->IsOfType<RationalFunctionOld>()) {
    return this->GetValue<RationalFunctionOld>().IsEqualToZero();
  }
  return false;
}

bool Expression::MakeError(const std::string& theError, Calculator& owner, bool isPublicError) {
  this->reset(owner, 2);
  this->AddChildAtomOnTop(owner.opError());
  if (isPublicError) {
    if (owner.errorsPublic.str() == "") {
      owner.errorsPublic << theError;
    }
  }
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

template <class coefficient>
bool Expression::ContextGetPolySubFromSuperContextNoFailure(
  const Expression& largerContext, PolynomialSubstitution<coefficient>& output
) const {
  bool mustBeTrue = this->ContextGetPolySubFromSuperContext(largerContext, output);
  if (!mustBeTrue) {
    global.fatal << "This is a programming error: I was not able to "
    << "extract a polynomial substitution from smaller context "
    << this->ToString() << " relative to larger context "
    << largerContext.ToString() << ". " << global.fatal;
  }
  return mustBeTrue;
}

template <class coefficient>
bool Expression::ContextGetPolySubFromSuperContext(
  const Expression& largerContext, PolynomialSubstitution<coefficient>& output
) const {
  Expression polyVarsElargerContext = largerContext.ContextGetPolynomialVariables();
  Expression polyVarsEsmallContext = this->ContextGetPolynomialVariables();
  output.SetSize(polyVarsElargerContext.children.size - 1);
  int numVars = polyVarsElargerContext.children.size - 1;
  for (int i = 1; i < polyVarsEsmallContext.children.size; i ++) {
    int theNewIndex = polyVarsElargerContext.GetIndexChild(polyVarsEsmallContext[i]);
    if (theNewIndex == - 1) {
      return false;
    }
    output[i - 1].MakeMonomiaL(theNewIndex - 1, 1, 1, numVars);
  }
  return true;
}

bool Expression::ContextGetPolyAndEWASubFromSuperContextNoFailure(
  const Expression& largerContext,
  PolynomialSubstitution<Rational>& outputPolyPart,
  PolynomialSubstitution<Rational>& outputEWApart
) const {
  bool mustBeTrue = this->ContextGetPolyAndEWASubFromSuperContext(largerContext, outputPolyPart, outputEWApart);
  if (!mustBeTrue) {
    global.fatal << "This is a programming error: "
    << "I was not able to extract a polynomial/differential operator substitution from smaller context "
    << this->ToString() << " relative to larger context " << largerContext.ToString() << global.fatal;
  }
  return mustBeTrue;
}

bool Expression::ContextGetPolyAndEWASubFromSuperContext(
  const Expression& largerContext,
  PolynomialSubstitution<Rational>& outputPolyPart,
  PolynomialSubstitution<Rational>& outputEWApart
) const {
  if (!this->ContextGetPolySubFromSuperContext(largerContext, outputPolyPart)) {
    return false;
  }
  Expression EWAVarsElargerContext = largerContext.ContextGetDifferentialOperatorVariables();
  Expression EWAVarsEsmallContext = this->ContextGetDifferentialOperatorVariables();
  outputEWApart.SetSize(EWAVarsElargerContext.children.size - 1);
  int numVars = EWAVarsElargerContext.children.size - 1;
  for (int i = 1; i < EWAVarsEsmallContext.children.size; i ++) {
    int theNewIndex = EWAVarsElargerContext.GetIndexChild(EWAVarsEsmallContext[i]);
    if (theNewIndex == - 1) {
      return false;
    }
    outputEWApart[i - 1].MakeMonomiaL(theNewIndex - 1, 1, 1, numVars);
  }
  return true;
}

bool Expression::MakeAtom(const std::string& atomName, Calculator& newBoss) {
  this->reset(newBoss);
  this->theData = newBoss.AddOperationNoRepetitionOrReturnIndexFirst(atomName);
  return true;
}

int Expression::ContextGetIndexAmbientSSalg() const {
  if (!this->IsContext()) {
    return - 1;
  }
  for (int i = 1; i < this->children.size; i ++) {
    if ((*this)[i].StartsWith(this->owner->opSSLieAlg(), 3)) {
      return (*this)[i][2].theData;
    }
  }
  return - 1;
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

Expression Expression::GetContext() const {
  this->CheckInitialization();
  if (this->IsBuiltInTypE()) {
    return (*this)[1];
  }
  if (this->IsMatrix()) {
    if ((*this)[0].StartsWith(this->owner->opMatriX())) {
      if ((*this)[0].size() > 2) {
        return (*this)[0][2];
      }
    }
  }
  Expression output;
  output.MakeEmptyContext(*this->owner);
  global.fatal << "This is a programming error: GetContext called on an Expression"
  << " that is not a built-in data type. "
  << " I can't display the expression as this may cause ``infinite'' "
  << "recursion if the error is caused by the ToString method. Here is however the lisp form "
  << this->ToStringFull() << " of the expression. " << "Here's  a stack trace. " << global.fatal;
  return output;
}

int Expression::GetNumContextVariables() const {
  return this->GetContext().ContextGetPolynomialVariables().children.size - 1;
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
    outputAppendList.SetSize(0);
  }
  if (!this->StartsWith(this->owner->opTimes())) {
    outputAppendList.AddOnTop(*this);
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
    outputAppendList.SetSize(0);
  }
  if (!this->StartsWith(this->owner->opTimes()) && !this->StartsWith(this->owner->opDivide())) {
    outputAppendList.AddOnTop(*this);
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

bool Expression::ToStringData(std::string& output, FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("Expression::ToStringData");
  if (this->owner == nullptr) {
    output = "(non-initialized)";
    return true;
  }
  std::stringstream out;
  bool result = false;
  bool isFinal = theFormat == nullptr ? true : theFormat->flagExpressionIsFinal;
  bool useQuotes = theFormat == nullptr ? false : theFormat->flagUseQuotes;
  MemorySaving<FormatExpressions> contextFormat;
  bool showContext = this->owner == nullptr ? false : owner->flagDisplayContext;
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
      out << "(unknown~ atom~ of~ value~ " << this->theData << ")";
    }
    result = true;
  } else if (this->IsOfType<std::string>()) {
    if (!useQuotes) {
      if (isFinal) {
        out
        << StringRoutines::ConvertStringToCalculatorDisplay(this->GetValue<std::string>());
      } else {
        out << HtmlRoutines::ConvertStringToHtmlString(this->GetValue<std::string>(), false);
      }
    } else {
      out << "\"" << StringRoutines::ConvertStringToJavascriptString(this->GetValue<std::string>()) << "\"";
    }
    result = true;
  } else if (this->IsOfType<Rational>()) {
    if (this->HasNonEmptyContext()) {
      out << "Rational{}(" << (*this)[1].ToString() << ", ";
    }
    if (!this->owner->flagUseFracInRationalLaTeX) {
      out << this->GetValue<Rational>().ToString();
    } else {
      out << this->GetValue<Rational>().ToStringFrac();
    }
    if (this->HasNonEmptyContext()) {
      out << ")";
    }
    result = true;
  } else if (this->IsOfType<ElementEllipticCurve<Rational> >()) {
    this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    contextFormat.GetElement().flagUseFrac = true;
    out << this->GetValue<ElementEllipticCurve<Rational> >().ToString(&contextFormat.GetElement());
    result = true;
  } else if (this->IsOfType<ElementEllipticCurve<ElementZmodP> >()) {
    this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    contextFormat.GetElement().flagUseFrac = true;
    out << this->GetValue<ElementEllipticCurve<ElementZmodP> >().ToString(&contextFormat.GetElement());
    result = true;
  } else if (this->IsOfType<ElementZmodP>()) {
    out << this->GetValue<ElementZmodP>().ToString();
    result = true;
  } else if (this->IsOfType<InputBox>()) {
    out << this->GetValue<InputBox>().GetUserInputBox();
    result = true;
  } else if (this->IsOfType<GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> >()) {
    out << this->GetValue<GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> >().ToString();
    result = true;
  } else if (this->IsOfType<ElementHyperoctahedralGroupR2>()) {
    out << this->GetValue<ElementHyperoctahedralGroupR2>().ToString(&contextFormat.GetElement());
    result = true;
  } else if (this->IsOfType<Polynomial<Rational> >()) {
    this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    contextFormat.GetElement().flagUseFrac = true;
    if (!this->owner->flagHidePolynomialBuiltInTypeIndicator) {
      out << "Polynomial{}(";
    }
    out << this->GetValue<Polynomial<Rational> >().ToString(&contextFormat.GetElement());
    if (!this->owner->flagHidePolynomialBuiltInTypeIndicator) {
      out << ")";
    }
    if (showContext) {
      out << "[" << this->GetContext().ToString() << "]";
    }
    result = true;
  } else if (this->IsOfType<Polynomial<AlgebraicNumber> >()) {
    this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    contextFormat.GetElement().flagUseFrac = true;
    out << "PolynomialAlgebraicNumbers{}(";
    std::string currentString= this->GetValue<Polynomial<AlgebraicNumber> >().ToString(&contextFormat.GetElement());
    if (currentString.size() > 0) {
      if (currentString[0] == '-') {
        currentString = currentString.substr(1);
        out << "-";
      }
    }
    if (this->owner->flagUseNumberColors) {
      out << "\\color{red}{";
    }
    out << currentString;
    if (this->owner->flagUseNumberColors) {
      out << "}";
    }
    out << ")";
    if (showContext) {
      out << "[" << this->GetContext().ToString() << "]";
    }
    result = true;
  } else if (this->IsOfType<RationalFunctionOld>()) {
    this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    contextFormat.GetElement().flagUseFrac = true;
    out << "MakeRationalFunction{}("
    << this->GetValue<RationalFunctionOld>().ToString(&contextFormat.GetElement()) << ")";
    if (showContext) {
      out << "[" << this->GetContext().ToString() << "]";
    }
    result = true;
  } else if (this->IsOfType<Weight<Polynomial<Rational> > >()) {
    this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    contextFormat.GetElement().flagFormatWeightAsVectorSpaceIndex = false;
    out << this->GetValue<Weight<Polynomial<Rational> > >().ToString(&contextFormat.GetElement());
    result = true;
  } else if (this->IsOfType<SemisimpleLieAlgebra>()) {
    out << "SSLieAlg{}("
    << this->GetValue<SemisimpleLieAlgebra>().ToStringLieAlgebraName()
    << ")";
    result = true;
  } else if (this->IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >()) {
    this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    out << "UEE{}(" //<< this->GetContext().ToString() << ", "
    << this->GetValue<ElementUniversalEnveloping<RationalFunctionOld> >().ToString(&contextFormat.GetElement())
    << ")";
    result = true;
  } else if (this->IsOfType<MatrixTensor<Rational> >()) {
    this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    contextFormat.GetElement().flagUseLatex = true;
    contextFormat.GetElement().flagUseHTML = false;
    if (this->GetValue<MatrixTensor<Rational> >().GetMinNumColsNumRows() < 20) {
      out << "MatrixRationalsTensorForm{}("
      << this->GetValue<MatrixTensor<Rational> > ().ToStringMatForm(&contextFormat.GetElement())
      << ")";
    } else {
      out << this->GetValue<MatrixTensor<Rational> >().ToString();
    }
    result = true;
  } /*else if (this->IsMatrixGivenType<Rational>()) {
    this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    contextFormat.GetElement().flagUseLatex = true;
    contextFormat.GetElement().flagUseHTML = false;
    Matrix<Rational> theMat;
    this->IsMatrixGivenType(0, 0, &theMat);
    out << theMat.ToString(&contextFormat.GetElement());
    result = true;
  } else if (this->IsMatrixGivenType<double>()) {
    out.precision(4);
    Matrix<double> theMat;
    this->IsMatrixGivenType(0, 0, &theMat);
    out << std::fixed << theMat;
    result = true;
  } else if (this->IsMatrixGivenType<AlgebraicNumber>()) {
    this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    contextFormat.GetElement().flagUseLatex = true;
    contextFormat.GetElement().flagUseHTML = false;
    Matrix<AlgebraicNumber> theMat;
    this->IsMatrixGivenType(0, 0, &theMat);
    out << theMat.ToString(&contextFormat.GetElement());
    result = true;
  } */else if (this->IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >()) {
    this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    out << "ETGVM{}(";
    out << this->GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >().ToString(&contextFormat.GetElement());
    out << ")";
    result = true;
  } else if (this->IsOfType<Plot>()) {
    if (isFinal) {
      Plot& thePlot = this->GetValueNonConst<Plot>();
      thePlot.flagIncludeExtraHtmlDescriptions = (theFormat == nullptr) ? true : theFormat->flagIncludeExtraHtmlDescriptionsInPlots;
      thePlot.flagPlotShowJavascriptOnly = this->owner->flagPlotShowJavascriptOnly;
      out << thePlot.GetPlotHtml(*this->owner);
      if (this->owner->flagWriteLatexPlots) {
        out << this->owner->WriteDefaultLatexFileReturnHtmlLink(thePlot.GetPlotStringAddLatexCommands(false), nullptr, true);
        out << "<br><b>LaTeX code used to generate the output. </b><br>"
        << thePlot.GetPlotStringAddLatexCommands(true);
      }
    } else {
      out << "(plot not shown)";
    }
    result = true;
  } else if (this->IsOfType<WeylGroupData>()) {
    WeylGroupData& theGroup = this->GetValueNonConst<WeylGroupData>();
    contextFormat.GetElement().flagUseLatex = true;
    contextFormat.GetElement().flagUseHTML = false;
    contextFormat.GetElement().flagUseReflectionNotation = true;
    out << theGroup.ToString(&contextFormat.GetElement());
    result = true;
  } else if (this->IsOfType<ElementWeylGroup>()) {
    const ElementWeylGroup& theElt = this->GetValue<ElementWeylGroup>();
    contextFormat.GetElement().flagUseLatex = true;
    contextFormat.GetElement().flagUseHTML = false;
    contextFormat.GetElement().flagUseReflectionNotation = true;
    out << theElt.ToString(&contextFormat.GetElement());
    result = true;
  } else if (this->IsOfType<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >()) {
    const GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& theElt =
    this->GetValue<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >();
    contextFormat.GetElement().flagUseLatex = true;
    contextFormat.GetElement().flagUseHTML = false;
    contextFormat.GetElement().flagUseReflectionNotation = true;
    out << theElt.ToString(&contextFormat.GetElement());
    result = true;
  } else if (this->IsOfType<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >()) {
    const VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& theElt =
    this->GetValue<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >();
    contextFormat.GetElement().flagUseLatex = true;
    contextFormat.GetElement().flagUseHTML = false;
    contextFormat.GetElement().flagUseReflectionNotation = true;
    out << theElt.ToString(&contextFormat.GetElement());
    result = true;
  } else if (this->IsOfType<charSSAlgMod<Rational> >()) {
    charSSAlgMod<Rational> theElt = this->GetValue<charSSAlgMod<Rational> >();
    out << theElt.ToString();
    result = true;
  } else if (this->IsOfType<SemisimpleSubalgebras>()) {
    SemisimpleSubalgebras& theSubalgebras = this->GetValueNonConst<SemisimpleSubalgebras>();
    contextFormat.GetElement().flagUseLatex = true;
    contextFormat.GetElement().flagUseHTML = true;
    contextFormat.GetElement().flagCandidateSubalgebraShortReportOnly = false;
    contextFormat.GetElement().flagIncludeMutableInformation = false;
    contextFormat.GetElement().flagUseMathSpanPureVsMouseHover = false;
    out << theSubalgebras.ToString(&contextFormat.GetElement());
    result = true;
  } else if (this->IsOfType<double>()) {
    std::string currentString = FloatingPoint::DoubleToString(this->GetValue<double>());
    if (currentString.size() > 0) {
      if (currentString[0] == '-') {
        currentString = currentString.substr(1);
        out << "-";
      }
    }
    if (this->owner->flagUseNumberColors) {
      out << "\\color{blue}{";
    }
    out << currentString;
    if (this->owner->flagUseNumberColors) {
      out << "}";
    }
    result = true;
  } else if (this->IsOfType<AlgebraicNumber>()) {
    if (this->owner->flagUseFracInRationalLaTeX) {
      contextFormat.GetElement().flagUseFrac = true;
    }
    std::string currentString = this->GetValue<AlgebraicNumber>().ToString(&contextFormat.GetElement());
    if (currentString.size() > 0) {
      if (currentString[0] == '-') {
        currentString = currentString.substr(1);
        out << "-";
      }
    }
    if (this->owner->flagUseNumberColors) {
      out << "\\color{red}{";
    }
    out << currentString;
    if (this->owner->flagUseNumberColors) {
      out << "}";
    }
    result = true;
  } else if (this->IsOfType<LittelmannPath>()) {
    out << this->GetValue<LittelmannPath>().ToString();
    result = true;
  } else if (this->IsMatrixOfType<RationalFunctionOld>()) {
    this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    contextFormat.GetElement().flagUseHTML = false;
    contextFormat.GetElement().flagUseLatex = true;
    Matrix<RationalFunctionOld> theMat;
    this->owner->functionGetMatrix(*this, theMat);
    out << theMat.ToString(&contextFormat.GetElement());
    result = true;
  } else if (this->IsOfType<ElementWeylAlgebra<Rational> >()) {
    this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    contextFormat.GetElement().flagUseHTML = false;
    contextFormat.GetElement().flagUseLatex = true;
    out << "ElementWeylAlgebra{}(";
    out << this->GetValue<ElementWeylAlgebra<Rational> >().ToString(&contextFormat.GetElement());
    out << ")";
    if (showContext) {
      out << "[" << this->GetContext().ToString() << "]";
    }
    result = true;
  }
  output = out.str();
  return result;
}

std::string Expression::ToStringSemiFull() const {
  std::stringstream out;
  std::string tempS;
  if (this->ToStringData(tempS)) {
    out << tempS << " ";
  } else {
    if (this->children.size > 0) {
      out << "(";
      for (int i = 0; i < this->children.size; i ++) {
        out << (*this)[i].ToStringSemiFull();
        if (i != this->children.size - 1) {
          out << ", ";
        }
      }
      out << ")";
    }
  }
  return out.str();
}

std::string Expression::ToStringFull() const {
  std::stringstream out;
  if (this->IsAtom()) {
    out << this->theData << " ";
  }
  if (this->children.size > 0) {
    out << "(";
    for (int i = 0; i < this->children.size; i ++) {
      out << (*this)[i].ToStringFull();
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

bool Expression::NeedsParenthesisForMultiplication() const {
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
    return this->GetValue<Rational>().NeedsParenthesisForMultiplication();
  }
  if (this->IsOfType<AlgebraicNumber>()) {
    return this->GetValue<AlgebraicNumber>().NeedsParenthesisForMultiplication();
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

std::string Expression::ToStringAllSlidersInExpression() const {
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
    int theIndex = theSliders.GetIndex(boxNames[i]);
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
          input[i - 1] = startingExpression[i].ToString(theFormat);
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
        out << currentE.ToString(theFormat);
      } else {
        out << HtmlRoutines::GetMathSpanPure(currentE.ToString(theFormat), 1700);
      }
      out << currentE.ToStringAllSlidersInExpression();
      output[i - 1] = out.str();
    }
  } else {
    input[0] = HtmlRoutines::GetMathSpanPure(startingExpression.ToString(theFormat), 1700);
    if (
      this->IsOfType<std::string>() || this->IsOfType<Plot>() ||
      this->IsOfType<SemisimpleSubalgebras>() || this->IsOfType<WeylGroupData>()
    ) {
      output[0] = this->ToString(theFormat);
    } else {
      output[0] = HtmlRoutines::GetMathSpanPure(this->ToString(theFormat), 1700);
    }
  }
  result["input"] = input;
  result["output"] = output;
  return result;
}

void Expression::ToStringOpTimes(
  std::stringstream& out, FormatExpressions *theFormat
) const {
  if (!this->StartsWith(this->owner->opTimes(), 3)) {
    global.fatal << "Bad call of tostring function. " << global.fatal;
  }
  std::string secondE = (*this)[2].ToString(theFormat);
  if ((*this)[1].IsOperationGiven(this->owner->opSqrt())) {
    // A malformed expression such as: "\sqrt 3" will be parsed as "sqrt * 3"
    // and later corrected to "\sqrt{3}".
    out << "\\sqrt{" << secondE << "}";
    return;
  }
  std::string firstE = (*this)[1].ToString(theFormat);
  bool firstNeedsBrackets = (*this)[1].NeedsParenthesisForMultiplication();
  bool secondNeedsBrackets = (*this)[2].NeedsParenthesisForMultiplicationWhenSittingOnTheRightMost(&((*this)[1]));
  if (secondE.size() > 0) {
    if (secondE[0] == '-') {
      secondNeedsBrackets = true;
    }
  }
  bool mustHaveTimes = false;
  if (firstE == "-1" || firstE == "- 1") {
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
}

void Expression::ToStringOpMultiplicative(
  std::stringstream& out,
  const std::string& operation,
  FormatExpressions* theFormat
) const {
  std::string secondE = (*this)[2].ToString(theFormat);
  std::string firstE = (*this)[1].ToString(theFormat);
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

std::string Expression::ToString(
  FormatExpressions* theFormat,
  Expression* startingExpression,
  bool unfoldCommandEnclosures,
  JSData* outputJS
) const {
  MacroRegisterFunctionWithName("Expression::ToString");
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
      return newMe.ToString(theFormat, &newStart, false, outputJS);
    }
  }
  int notationIndex = owner->theObjectContainer.ExpressionWithNotation.GetIndex(*this);
  if (notationIndex != - 1) {
    return owner->theObjectContainer.ExpressionNotation[notationIndex];
  }
  std::stringstream out;
  bool isFinal = theFormat->flagExpressionIsFinal;
  bool allowNewLine = theFormat->flagExpressionNewLineAllowed;
  bool oldAllowNewLine = allowNewLine;
  bool useFrac = this->owner->flagUseFracInRationalLaTeX;
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
  int lineBreak = 50;
  int charCounter = 0;
  std::string tempS;
  if (outputJS != nullptr) {
    outputJS->reset();
  }
  if (this->ToStringData(tempS, theFormat)) {
    out << tempS;
  } else if (this->StartsWith(this->owner->opDefine(), 3)) {
    std::string firstE  = (*this)[1].ToString(theFormat);
    std::string secondE = (*this)[2].ToString(theFormat);
    if (
      (*this)[1].IsListStartingWithAtom(this->owner->opDefine()) ||
      (*this)[1].IsListStartingWithAtom(this->owner->opGreaterThan()) ||
      (*this)[1].IsListStartingWithAtom(this->owner->opGreaterThanOrEqualTo()) ||
      (*this)[1].IsListStartingWithAtom(this->owner->opLessThan()) ||
      (*this)[1].IsListStartingWithAtom(this->owner->opLessThanOrEqualTo())
    ) {
      out << "\\left(" << firstE << "\\right)";
    } else {
      out << firstE;
    }
    out << "=";
    if ((*this)[2].IsListStartingWithAtom(this->owner->opDefine())) {
      out << "\\left(" << secondE << "\\right)";
    } else {
      out << secondE;
    }
  } else if (this->IsListStartingWithAtom(this->owner->opIsDenotedBy())) {
    out << (*this)[1].ToString(theFormat) << "=:" << (*this)[2].ToString(theFormat);
  } else if (
    this->owner->flagUseLnAbsInsteadOfLogForIntegrationNotation &&
    this->StartsWith(this->owner->opLog(), 2)
  ) {
    std::string theArg = (*this)[1].ToString(theFormat);
    if (!StringRoutines::StringBeginsWith(theArg, "\\left|")) {
      out << "\\ln \\left|" << theArg << "\\right|";
    } else {
      out << "\\ln " << theArg;
    }
  } else if (this->StartsWith(this->owner->opLogBase(), 3)) {
    out << "\\log_{" << (*this)[1].ToString(theFormat) << "}"
    << "\\left(" << (*this)[2].ToString(theFormat) << "\\right)";
  } else if (this->StartsWith(this->owner->opIntervalOpen(), 3)) {
    if (!this->owner->flagUseBracketsForIntervals) {
      out << "IntervalOpen{}";
    }
    out << "\\left(" << (*this)[1].ToString(theFormat) << ", "
    << (*this)[2].ToString(theFormat) << "\\right)";
  } else if (this->StartsWith(this->owner->opIntervalClosed(), 3)) {
    out << "\\left[" << (*this)[1].ToString(theFormat) << ", "
    << (*this)[2].ToString(theFormat) << "\\right]";
  } else if (this->StartsWith(this->owner->opIntervalLeftClosed(), 3)) {
    out << "\\left[" << (*this)[1].ToString(theFormat) << ", "
    << (*this)[2].ToString(theFormat) << "\\right)";
  } else if (this->StartsWith(this->owner->opIntervalRightClosed(), 3)) {
    out << "\\left(" << (*this)[1].ToString(theFormat) << ", "
    << (*this)[2].ToString(theFormat) << "\\right]";
  } else if (this->StartsWith(this->owner->opIntervalLeftClosed(), 2)) {
    if ((*this)[1].IsSequenceNElementS(2)) {
      out << "\\left[" << (*this)[1][1] << ", " << (*this)[1][2] << "\\right)";
    } else {
      out << (*this)[0].ToString(theFormat) << "{}" << (*this)[1].ToString(theFormat);
    }
  } else if (this->StartsWith(this->owner->opIntervalClosed(), 2)) {
    if ((*this)[1].IsSequenceNElementS(2)) {
      out << "\\left[" << (*this)[1][1] << ", " << (*this)[1][2] << "\\right]";
    } else {
      out << (*this)[0].ToString(theFormat) << "{}" << (*this)[1].ToString(theFormat);
    }
  } else if (this->StartsWith(this->owner->opIntervalRightClosed(), 2)) {
    if ((*this)[1].IsSequenceNElementS(2)) {
      out << "\\left(" << (*this)[1][1] << ", " << (*this)[1][2] << "\\right]";
    } else {
      out << (*this)[0].ToString(theFormat) << "{}" << (*this)[1].ToString(theFormat);
    }
  } else if (this->StartsWith(this->owner->opQuote(), 2)) {
    if ((*this)[1].IsOperation(&tempS)) {
      out << "\"" <<  tempS << "\"";
    } else {
      out << "(Corrupt string)";
    }
  } else if (this->StartsWith(this->owner->opDefineConditional(),4)) {
    out << (*this)[1].ToString(theFormat) << " :if \\left("
    << (*this)[2].ToString(theFormat) << "\\right)="
    << (*this)[3].ToString(theFormat);
  } else if (this->StartsWith(this->owner->opDivide(), 3)) {
    bool doUseFrac = this->owner->flagUseFracInRationalLaTeX;
    if (
      doUseFrac &&
      ((*this)[1].StartsWith(this->owner->opTimes()) || (*this)[1].StartsWith(this->owner->opDivide()))
    ) {
      List<Expression> multiplicandsLeft;
      this->GetMultiplicandsDivisorsRecursive(multiplicandsLeft, 0);
      for (int i = 0; i < multiplicandsLeft.size; i ++) {
        if (
          multiplicandsLeft[i].StartsWith(this->owner->opIntegral()) ||
          multiplicandsLeft[i].IsOperationGiven(this->owner->opIntegral())
        ) {
          doUseFrac = false;
          break;
        }
      }
    }
    if (!doUseFrac) {
      std::string firstE = (*this)[1].ToString(theFormat);
      std::string secondE = (*this)[2].ToString(theFormat);
      bool firstNeedsBrackets =
        !((*this)[1].IsListStartingWithAtom(this->owner->opTimes()) ||
        (*this)[1].IsListStartingWithAtom(this->owner->opDivide()));
      bool secondNeedsBrackets = true;
      if ((*this)[2].IsOfType<Rational>()) {
        if ((*this)[2].GetValue<Rational>().IsInteger()) {
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
      if (theFormat != nullptr) {
        theFormat->flagExpressionNewLineAllowed = false;
      }
      std::string firstE = (*this)[1].ToString(theFormat);
      std::string secondE = (*this)[2].ToString(theFormat);
      out << "\\frac{" << firstE << "}{" << secondE << "}";
      if (theFormat != nullptr) {
        theFormat->flagExpressionNewLineAllowed = oldAllowNewLine;
      }
    }
  } else if (this->StartsWith(this->owner->opTensor(), 3)) {
    this->ToStringOpMultiplicative(out, "\\otimes", theFormat);
  } else if (this->StartsWith(this->owner->opIn(), 3)) {
    out << (*this)[1].ToString(theFormat) << "\\in "
    << (*this)[2].ToString(theFormat);
  } else if (this->StartsWith(this->owner->opOr(), 3)) {
    if (!(*this)[1].IsSequenceNElementS()) {
      out << "\\left(" << (*this)[1] << "\\right)";
    } else {
      out << (*this)[1];
    }
    out << "\\ or\\ ";
    if (!(*this)[2].IsSequenceNElementS()) {
      out << "\\left(" << (*this)[2] << "\\right)";
    } else {
      out << (*this)[2];
    }
  } else if (this->StartsWith(this->owner->opAnd(), 3)) {
    out << "\\left(" << (*this)[1] << "\\right)" << " and "
    << "\\left(" << (*this)[2] << "\\right)" ;
  } else if (this->StartsWith(this->owner->opBinom(), 3)) {
    out << "\\binom{" << (*this)[1].ToString(theFormat) << "}{ "
    << (*this)[2].ToString(theFormat) << "}";
  } else if (this->StartsWith(this->owner->opUnderscore())) {
    out << "{" << (*this)[1].ToString(theFormat) << "}_{"
    << (*this)[2].ToString(theFormat) << "}";
  } else if (this->StartsWith(this->owner->opSetMinus(), 3)) {
    out << (*this)[1].ToString(theFormat) << "\\setminus "
    << (*this)[2].ToString(theFormat);
  } else if (this->StartsWith(this->owner->opLimitBoundary(), 3)) {
    out << "\\limits_{" << (*this)[1].ToString(theFormat)
    << "}^{"
    << (*this)[2].ToString(theFormat)
    << "}";
  } else if (this->StartsWith(this->owner->opTimes(), 3)) {
    this->ToStringOpTimes(out, theFormat);
  } else if (this->StartsWith(this->owner->opCrossProduct())) {
    this->ToStringOpMultiplicative(out, "\\times", theFormat);
  } else if (this->StartsWith(this->owner->opSqrt(), 3)) {
    int thePower = 0;
    bool hasPowerTwo = (*this)[1].IsSmallInteger(&thePower);
    if (hasPowerTwo) {
      hasPowerTwo = (thePower == 2);
    }
    if (hasPowerTwo) {
      out << "\\sqrt{" << (*this)[2].ToString(theFormat) << "}";
    } else {
      out << "\\sqrt[" << (*this)[1].ToString(theFormat)
      << "]{" << (*this)[2].ToString(theFormat) << "}";
    }
  } else if (this->StartsWith(this->owner->opFactorial(), 2)) {
    if ((*this)[1].NeedsParenthesisForBaseOfExponent()) {
      out << "\\left(" << (*this)[1].ToString(theFormat) << "\\right) !";
    } else {
      out << (*this)[1].ToString(theFormat) << "!";
    }
  } else if (this->StartsWith(this->owner->opAbsoluteValue(), 2)) {
    out << "\\left|" << (*this)[1].ToString(theFormat) << "\\right|";
  } else if (this->StartsWith(this->owner->opThePower(), 3)) {
    bool involvesExponentsInterpretedAsFunctions = false;
    const Expression& firstE = (*this)[1];
    const Expression& secondE = (*this)[2];
    if (firstE.StartsWith(- 1, 2)) {
      bool shouldProceed = firstE[0].IsAtomWhoseExponentsAreInterpretedAsFunction() &&
      !secondE.IsEqualToMOne() && secondE.IsRational();
      if (
        shouldProceed && firstE[0].IsOperationGiven(this->owner->opLog()) &&
        this->owner->flagUseLnAbsInsteadOfLogForIntegrationNotation
      ) {
        shouldProceed = false;
      }
      if (shouldProceed) {
        involvesExponentsInterpretedAsFunctions = true;
        Expression newFunE;
        newFunE.MakeXOX(*this->owner, this->owner->opThePower(), firstE[0], (*this)[2]);
        newFunE.CheckConsistency();
        out << "{" << newFunE.ToString(theFormat) << "}{}";
        if (
          firstE[1].NeedsParenthesisForMultiplicationWhenSittingOnTheRightMost() ||
          firstE[1].StartsWith(this->owner->opTimes())
        ) {
          out << "\\left(" << firstE[1].ToString(theFormat) << "\\right)";
        } else {
          out << firstE[1].ToString(theFormat);
        }
      }
    }
    if (!involvesExponentsInterpretedAsFunctions) {
      bool isSqrt = false;
      if ((*this)[2].IsOfType<Rational>()) {
        if ((*this)[2].GetValue<Rational>().IsEqualTo(Rational(1, 2))) {
          isSqrt = true;
        }
      }
      if (isSqrt) {
        out << "\\sqrt{" << (*this)[1].ToString(theFormat) << "}";
      } else {
        std::string secondEstr = (*this)[2].ToString(theFormat);
        std::string firstEstr = (*this)[1].ToString(theFormat);
        if ((*this)[1].NeedsParenthesisForBaseOfExponent()) {
          bool useBigParenthesis = true;
          if ((*this)[1].StartsWith(this->owner->opDivide())) {
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
  } else if (this->StartsWith(this->owner->opPlus())) {
    if (this->children.size < 3) {
      global.fatal << "Plus operation takes at least 2 arguments, whereas this expression has "
      << this->children.size - 1 << " arguments. " << global.fatal;
    }
    const Expression& left = (*this)[1];
    const Expression& right = (*this)[2];
    std::string leftString;
    if (left.NeedsParenthesisForAddition()) {
      leftString = "\\left(" + left.ToString(theFormat) + "\\right)";
    } else {
      leftString = left.ToString(theFormat);
    }
    out << leftString;
    if (allowNewLine && !useFrac && leftString.size() > static_cast<unsigned>(lineBreak)) {
      out << "\\\\\n";
    }
    std::string rightString = right.NeedsParenthesisForAddition() ?
    ("\\left(" + right.ToString(theFormat) + "\\right)") : right.ToString(theFormat);
    if (rightString.size() > 0) {
      if (rightString[0] != '-') {
        out << "+";
      }
    }
    out << rightString;
  } else if (this->IsListStartingWithAtom(this->owner->opDirectSum())) {
    if (this->children.size < 3) {
      global.fatal << "Direct sum operation takes at least 2 arguments, whereas this expression has "
      << this->children.size << " arguments. " << global.fatal;
    }
    const Expression& left = (*this)[1];
    const Expression& right = (*this)[2];
    std::string leftString;
    if (left.NeedsParenthesisForAddition()) {
      leftString = "\\left(" + left.ToString(theFormat) + "\\right)";
    } else {
      leftString = left.ToString(theFormat);
    }
    out << leftString;
    if (allowNewLine && !useFrac && leftString.size() > static_cast<unsigned>(lineBreak)) {
      out << "\\\\\n";
    }
    std::string rightString = right.NeedsParenthesisForAddition() ?
    ("\\left(" + right.ToString(theFormat) + "\\right)") : right.ToString(theFormat);
    if (rightString.size() > 0) {
      if (rightString[0] != '-') {
        out << "\\oplus ";
      }
    }
    out << rightString;
  } else if (this->StartsWith(this->owner->opMinus(), 2)) {
    if ((*this)[1].StartsWith(this->owner->opPlus()) || (*this)[1].StartsWith(this->owner->opMinus())) {
      out << "-\\left(" << (*this)[1].ToString(theFormat) << "\\right)";
    } else {
      out << "-" << (*this)[1].ToString(theFormat);
    }
  } else if (this->StartsWith(this->owner->opSqrt(), 2)) {
    out << "\\sqrt{" << (*this)[1].ToString(theFormat) << "}";
  } else if (this->StartsWith(this->owner->opMinus(), 3)) {
    if (!(this->children.size == 3)) {
      global.fatal << "This is a programming error: "
      << "the minus function expects 1 or 2 arguments, "
      << "instead there are " << this->children.size - 1
      << ". " << global.fatal;
    }
    out << (*this)[1].ToString(theFormat) << "-";
    if ((*this)[2].StartsWith(this->owner->opPlus()) || (*this)[2].StartsWith(this->owner->opMinus())) {
      out << "\\left(" << (*this)[2].ToString(theFormat) << "\\right)";
    } else {
      out << (*this)[2].ToString(theFormat);
    }
  } else if (this->StartsWith(this->owner->opBind(), 2)) {
    out << "{{" << (*this)[1].ToString(theFormat) << "}}";
  } else if (this->IsListStartingWithAtom(this->owner->opEqualEqual())) {
    out << (*this)[1].ToString(theFormat) << "==" << (*this)[2].ToString(theFormat);
  } else if (this->IsListStartingWithAtom(this->owner->opEqualEqualEqual())) {
    out << (*this)[1].ToString(theFormat) << "===" << (*this)[2].ToString(theFormat);
  } else if (this->StartsWith(this->owner->opDifferentiate(), 3)) {
    out << "\\frac{\\text{d}} ";
    if ((*this)[2].NeedsParenthesisForMultiplication()) {
      out << "\\left(" << (*this)[2].ToString(theFormat)
      << "\\right)";
    } else {
      out << (*this)[2].ToString(theFormat);
    }
    out << "}{{\\text{d}} "
    << (*this)[1].ToString(theFormat)  << "}";
  } else if (this->StartsWith(this->owner->opDifferential(), 3)) {
    bool needsParen = (*this)[2].NeedsParenthesisForMultiplication() ||
    (*this)[2].NeedsParenthesisForMultiplicationWhenSittingOnTheRightMost();
    if ((*this)[2].StartsWith(this->owner->opDivide())) {
      needsParen = false;
    }
    bool rightNeedsParen = (!(*this)[1].IsAtom()) && (!(*this)[1].IsBuiltInTypE());
    std::string theCoeff = (*this)[2].ToString(theFormat);
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
    out << (*this)[1].ToString(theFormat);
    if (rightNeedsParen) {
      out << "\\right)";
    }
  } else if (this->StartsWith(this->owner->opDifferential(), 2)) {
    bool needsParen = (!(*this)[1].IsAtom()) && (!(*this)[1].IsBuiltInTypE());
    out << "{\\text{d}}{}";
    if (needsParen) {
      out << "\\left(";
    }
    out << (*this)[1].ToString(theFormat);
    if (needsParen) {
      out << "\\right)";
    }
  } else if (this->StartsWith(this->owner->opSum()) || this->StartsWith(this->owner->opIntegral())) {
    std::string opString = (*this)[0].ToString(theFormat);
    out << opString << " ";
    int firstIndex = 2;
    if (this->size() >= 2) {
      if ((*this)[1].StartsWith(this->owner->opLimitBoundary(), 3)) {
        out
        << "_{" << (*this)[1][1].ToString(theFormat) << "}"
        << "^{" << (*this)[1][2].ToString(theFormat) << "}";
      } else if ((*this)[1].IsOperationGiven(this->owner->opIndefiniteIndicator())) {
        firstIndex = 2;
      } else {
        firstIndex = 1;
      }
    }
    if (this->size() <= firstIndex + 1) {
      if (this->size() == firstIndex + 1) {
        out << (*this)[firstIndex].ToString(theFormat);
      }
    } else {
      out << "(";
      for (int i = firstIndex; i < this->size(); i ++) {
        out << (*this)[i].ToString(theFormat);
        if (i != this->size() - 1) {
          out << ", ";
        }
      }
      out << ")";
    }
  } else if (this->IsListStartingWithAtom(this->owner->opGreaterThan())) {
    out << (*this)[1].ToString(theFormat) << "&gt;" << (*this)[2].ToString(theFormat);
  } else if (this->IsListStartingWithAtom(this->owner->opGreaterThanOrEqualTo())) {
    out << (*this)[1].ToString(theFormat) << "\\geq " << (*this)[2].ToString(theFormat);
  } else if (this->IsListStartingWithAtom(this->owner->opLessThanOrEqualTo())) {
    out << (*this)[1].ToString(theFormat) << "\\leq " << (*this)[2].ToString(theFormat);
  } else if (this->StartsWith(this->owner->opLimit(), 3)) {
    out << "\\lim_{";
    if (!(*this)[1].IsSequenceNElementS()) {
      out << (*this)[1].ToString(theFormat);
    } else {
      for (int i = 1; i < (*this)[1].size(); i ++) {
        out << (*this)[1][i].ToString(theFormat);
        if (i != (*this)[1].size() - 1) {
          out << ", ";
        }
      }
    }
    out << "}" << (*this)[2].ToString(theFormat);
  } else if (this->IsListStartingWithAtom(this->owner->opLimitProcess())) {
    out << (*this)[1].ToString(theFormat) << " \\to " << (*this)[2].ToString(theFormat);
  } else if (this->IsListStartingWithAtom(this->owner->opLessThan())) {
    out << (*this)[1].ToString(theFormat) << "&lt;" << (*this)[2].ToString(theFormat);
  } else if (this->IsMatrix()) {
    if (theFormat->flagUseLatex && !theFormat->flagUsePmatrix) {
      out << "\\left(";
    }
    if (!theFormat->flagUsePmatrix) {
      int numCols = this->GetNumCols();
      out << "\\begin{array}{";
      for (int i = 0; i < numCols; i ++) {
        out << "c";
      }
      out << "} ";
    } else {
      out << "\\begin{pmatrix}";
    }
    for (int i = 1; i < this->size(); i ++) {
      for (int j = 1; j < (*this)[i].size(); j ++) {
        out << (*this)[i][j].ToString(theFormat);
        if (j != (*this)[i].size() - 1) {
          out << " & ";
        }
      }
      if (i != this->size() - 1) {
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
  } else if (this->IsListStartingWithAtom(this->owner->opSequence())) {
    if (this->size() == 2) {
      out << "{Sequence{}";
    }
    out << "\\left(";
    for (int i = 1; i < this->size(); i ++) {
      std::string currentChildString = (*this)[i].ToString(theFormat);
      out << currentChildString;
      charCounter += currentChildString.size();
      if (i != this->children.size - 1) {
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
    if (this->size() == 2) {
      out << "}";
    }
  } else if (this->IsListStartingWithAtom(this->owner->opLieBracket())) {
    out << "[" << (*this)[1].ToString(theFormat) << "," << (*this)[2].ToString(theFormat) << "]";
  } else if (this->IsListStartingWithAtom(this->owner->opMod())) {
    out << (*this)[1].ToString(theFormat) << " mod " << (*this)[2].ToString(theFormat);
  } else if (this->IsListStartingWithAtom(this->owner->opUnion()) && this->size() == 3) {
    if ((*this)[1].StartsWith(this->owner->opIntersection())) {
      out << "\\left(" << (*this)[1].ToString(theFormat) <<  "\\right)"
      << "\\cup " << (*this)[2].ToString(theFormat);
    } else {
      out << (*this)[1].ToString(theFormat) << "\\cup " << (*this)[2].ToString(theFormat);
    }
  } else if (this->IsListStartingWithAtom(this->owner->opIntersection()) && this->size() == 3) {
    if ((*this)[1].StartsWith(this->owner->opUnion())) {
      out << "\\left(" << (*this)[1].ToString(theFormat) << "\\right)"
      << "\\cap " << (*this)[2].ToString(theFormat);
    } else {
      out << (*this)[1].ToString(theFormat) << "\\cap " << (*this)[2].ToString(theFormat);
    }
  } else if (this->IsListStartingWithAtom(this->owner->opUnionNoRepetition())) {
    out << (*this)[1].ToString(theFormat) << "\\sqcup " << (*this)[2].ToString(theFormat);
  } else if (this->IsListStartingWithAtom(this->owner->opEndStatement())) {
    bool createTable = (startingExpression != nullptr);
    bool createSingleTable = false;
    if (createTable == false && theFormat != nullptr && !global.flagRunningCommandLine) {
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
            currentInput = HtmlRoutines::GetMathSpanPure((*startingExpression)[i].ToString(theFormat));
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
          currentOutput = currentE.ToString(theFormat);
        } else {
          currentOutput = HtmlRoutines::GetMathSpanPure(currentE.ToString(theFormat), 1700);
        }
        currentOutput += currentE.ToStringAllSlidersInExpression();
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
        outWithDelimiter << currentE.ToString(theFormat);
        if (createSingleTable && addLatexDelimiter) {
          outWithDelimiter << "\\)";
        }
        outWithDelimiter << currentE.ToStringAllSlidersInExpression();
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
  } else if (this->StartsWith(this->owner->opError(), 2)) {
    this->owner->NumErrors ++;
    out << "(Error~ " << this->owner->NumErrors << ":~ see~ comments)";
    this->owner->Comments << "<br>Error "
    << this->owner->NumErrors << ". " << (*this)[1].ToString(theFormat);
  } else if (this->size() == 1) {
    out << (*this)[0].ToString(theFormat);
  } else if (this->size() >= 2) {
    out << (*this)[0].ToString(theFormat);
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
      out << (*this)[i].ToString(theFormat);
      if (i != this->children.size - 1) {
        out << ", ";
      }
    }
    if (needParenthesis) {
      out << "\\right)";
    }
  } else { //<-not sure if this case is possible
    out << "(ProgrammingError:NotDocumented)";
  }
  std::string input, output;
  if (startingExpression != nullptr) {
    std::stringstream outTrue;
    outTrue << "<table class =\"tableCalculatorOutput\">";
    outTrue << "<tr><th>Input</th><th>Result</th></tr>";
    if (this->IsListStartingWithAtom(this->owner->opEndStatement())) {
      outTrue << out.str();
    } else {
      input = HtmlRoutines::GetMathSpanPure(startingExpression->ToString(theFormat), 1700);
      outTrue << "<tr><td class =\"cellCalculatorInput\">" << input << "</td>";
      if ((
          this->IsOfType<std::string>() || this->IsOfType<Plot>() ||
          this->IsOfType<SemisimpleSubalgebras>() || this->IsOfType<WeylGroupData>()
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
  std::string tempS;
  if (this->ToStringData(tempS)) {
    return tempS;
  }
  if (this->children.size == 0) {
    return this->ToString();
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
  return this->theData == this->owner->operations.GetIndex(desiredAtom);
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
  if (!this->HasContext()) {
    return true;
  }
  this->children.RemoveIndexShiftDown(1);
  if (this->children.size == 1) {
    this->AssignMeMyChild(0);
  }
  return true;
}

bool Expression::HasContext() const {
  this->CheckInitialization();
  if (!this->IsBuiltInTypE() || !(this->size() == 3)) {
    return false;
  }
  return (*this)[1].IsListStartingWithAtom(this->owner->opContexT());
}

bool Expression::HasNonEmptyContext() const {
  if (!this->HasContext()) {
    return false;
  }
  return !this->GetContext().StartsWith(this->owner->opContexT(), 1);
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
  this->IsOfType<RationalFunctionOld>() || this->IsOfType<double>() || this->IsOfType<AlgebraicNumber>();
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
  if (this->owner->GetBuiltInTypes().Contains(tempS)) {
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
    *outputWhichType = this->owner->GetOperations().GetIndex(theType);
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

bool Expression::MakeSum(Calculator& owner, const List<Expression>& theSummands) {
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

bool Expression::MakeEmptyContext(Calculator& owner) {
  this->reset(owner, 1);
  return this->AddChildAtomOnTop(owner.opContexT());
}

bool Expression::ContextMakeContextWithPolyVars(Calculator& owner, const List<std::string>& inputPolyVarNames) {
  MacroRegisterFunctionWithName("Expression::ContextMakeContextWithPolyVars");
  List<Expression> thePolyVars;
  thePolyVars.SetSize(inputPolyVarNames.size);
  for (int i = 0; i < thePolyVars.size; i ++) {
    thePolyVars[i].MakeAtom(inputPolyVarNames[i], owner);
  }
  return this->ContextMakeContextWithPolyVars(owner, thePolyVars);
}

bool Expression::ContextMakeContextWithPolyVars(Calculator& owner, const List<Expression>& inputPolyVarEs) {
  this->MakeEmptyContext(owner);
  Expression thePolyVars;
  thePolyVars.reset(owner, 2);
  thePolyVars.AddChildAtomOnTop(owner.opPolynomialVariables());
  for (int i = 0; i< inputPolyVarEs.size; i ++) {
    thePolyVars.AddChildOnTop(inputPolyVarEs[i]);
  }
  return this->AddChildOnTop(thePolyVars);
}

bool Expression::ContextMakeContextWithOnePolyVar(Calculator& owner, const Expression& inputPolyVarE) {
  this->MakeEmptyContext(owner);
  Expression thePolyVars;
  thePolyVars.reset(owner, 2);
  thePolyVars.AddChildAtomOnTop(owner.opPolynomialVariables());
  thePolyVars.AddChildOnTop(inputPolyVarE);
  return this->AddChildOnTop(thePolyVars);
}

bool Expression::MakeContextWithOnePolyVarOneDiffVar(
  Calculator& owner, const Expression& inputPolyVarE, const Expression& inputDiffVarE
) {
  this->MakeEmptyContext(owner);
  return this->ContextSetDiffOperatorVar(inputPolyVarE, inputDiffVarE);
}

bool Expression::MakeContextSSLieAlg(Calculator& owner, const SemisimpleLieAlgebra& theSSLiealg) {
  MacroRegisterFunctionWithName("Expression::MakeContextSSLieAlg");
  theSSLiealg.CheckConsistency();
  this->MakeEmptyContext(owner);
  owner.theObjectContainer.GetLieAlgebraCreateIfNotPresent(theSSLiealg.theWeyl.theDynkinType);
  return this->ContextSetSSLieAlgebrA(
    owner.theObjectContainer.theSSLieAlgebras.GetIndex(theSSLiealg.theWeyl.theDynkinType), owner
  );
}

bool Expression::ContextMakeContextWithOnePolyVar(Calculator& owner, const std::string& inputPolyVarName) {
  Expression varNameE;
  varNameE.MakeAtom(owner.AddOperationNoRepetitionOrReturnIndexFirst(inputPolyVarName), owner);
  return this->ContextMakeContextWithOnePolyVar(owner, varNameE);
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

std::string Expression::ToUTF8String(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("Expression::ToUTF8String");
  if (this->owner == nullptr) {
    return this->ToString(theFormat);
  }
  std::stringstream out;
  Rational theRat;
  if (this->IsOfType<Rational>(&theRat)) {
    FormatExpressions tempFormat;
    tempFormat.flagUseFrac = false;
    return theRat.ToString(&tempFormat);
  } else if (this->IsOperationGiven(this->owner->opPi())) {
    return "\\u03C0";
  } else if (this->StartsWith(this->owner->opPlus(), 3)) {
    return (*this)[1].ToUTF8String(theFormat) + "+" + (*this)[2].ToUTF8String(theFormat);
  } else if (this->StartsWith(this->owner->opTimes(), 3)) {
    std::string secondUTF8String = (*this)[2].ToUTF8String(theFormat);
    std::string secondString = (*this)[2].ToString(theFormat);
    if ((*this)[1].IsOperationGiven(this->owner->opSqrt())) {
      out << "sqrt(" << secondUTF8String << ")";
    } else {
      std::string firstE = (*this)[1].ToUTF8String(theFormat);
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
      out << (*this)[i].ToUTF8String(theFormat);
      if (i < this->size() - 1) {
        out << ", ";
      }
    }
    out << ")";
    return out.str();
  }
  return this->ToString(theFormat);
}
