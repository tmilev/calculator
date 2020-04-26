// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_interface.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_extra_Weyl_algebras_implementation.h"
#include "math_extra_modules_semisimple_Lie_algebras_implementation.h"

ExpressionContext::ExpressionContext(Calculator& inputOwner) {
  this->initialize(inputOwner);
}

ExpressionContext::ExpressionContext() {
  this->owner = nullptr;
  this->indexAmbientSemisimpleLieAlgebra = - 1;
}

void ExpressionContext::initialize(Calculator& inputOwner) {
  this->owner = &inputOwner;
  this->checkInitialization();
  this->indexAmbientSemisimpleLieAlgebra = - 1;
  this->variables.clear();
  this->differentialOperatorVariables.setSize(0);
}

bool ExpressionContext::operator==(const ExpressionContext& other) const {
  return
  this->owner == other.owner &&
  this->indexAmbientSemisimpleLieAlgebra == other.indexAmbientSemisimpleLieAlgebra &&
  this->variables == other.variables &&
  this->differentialOperatorVariables == other.differentialOperatorVariables;
}

template <class Coefficient>
void ExpressionContext::polynomialSubstitutionNoFailure(
  const ExpressionContext& largerContext,
  PolynomialSubstitution<Coefficient>& output
) const {
  bool mustBeTrue = this->polynomialSubstitution(largerContext, output);
  if (!mustBeTrue) {
    global.fatal << "This is a programming error: I was not able to "
    << "extract a polynomial substitution from smaller context "
    << this->toString() << " relative to larger context "
    << largerContext.toString() << ". " << global.fatal;
  }
}

bool ExpressionContext::checkInitialization() const {
  if (this->owner == nullptr) {
    global.fatal
    << "Expression context not initialized when it should be. "
    << global.fatal;
  }
  return false;
}

int ExpressionContext::numberOfVariables() const {
  return this->variables.size;
}

Expression ExpressionContext::toExpression() const {
  this->checkInitialization();
  Expression result(*this->owner);
  result.addChildAtomOnTop(this->owner->opContext());
  if (this->indexAmbientSemisimpleLieAlgebra != - 1) {
    result.addChildOnTop(this->toExpressionSemisimpleLieAlgebra());
  }
  if (this->variables.size > 0) {
    result.addChildOnTop(this->toExpressionPolynomialVariables());
  }
  if (this->differentialOperatorVariables.size > 0) {
    result.addChildOnTop(this->toExpressionDifferntialOperators());
  }
  return result;
}

Expression ExpressionContext::toExpressionSemisimpleLieAlgebra() const {
  Expression algebraContext(*this->owner);
  algebraContext.addChildAtomOnTop(this->owner->opSemisimpleLieAlgebrA());
  algebraContext.addChildOnTop(this->indexAmbientSemisimpleLieAlgebra);
  return algebraContext;
}

Expression ExpressionContext::toExpressionDifferntialOperators() const {
  Expression diffVarsE(*this->owner);
  diffVarsE.addChildAtomOnTop(this->owner->opWeylAlgebraVariables());
  for (int i = 0; i < this->differentialOperatorVariables.size; i ++) {
    diffVarsE.addChildOnTop(this->differentialOperatorVariables[i]);
  }
  return diffVarsE;
}

Expression ExpressionContext::toExpressionPolynomialVariables() const {
  Expression polynomialVariables;
  polynomialVariables.reset(*this->owner);
  polynomialVariables.addChildAtomOnTop(this->owner->opPolynomialVariables());
  for (int i = 0; i < this->variables.size; i ++) {
    polynomialVariables.addChildOnTop(this->variables[i]);
  }
  return polynomialVariables;
}

void ExpressionContext::addVariable(
  const Expression& inputVariable
) {
  this->variables.addOnTop(inputVariable);
}

bool ExpressionContext::setVariables(
  const List<Expression>& inputVariables
) {
  this->variables = inputVariables;
  return true;
}

bool ExpressionContext::setVariablesFromStrings(
  const List<std::string>& inputVariables
) {
  this->variables.clear();
  for (int i = 0; i < inputVariables.size; i ++) {
    Expression current;
    current.makeAtom(inputVariables[i], *this->owner);
    this->variables.addOnTop(current);
  }
  return true;
}

bool Expression::MergeContexts(Expression& leftE, Expression& rightE) {
  MacroRegisterFunctionWithName("Expression::MergeContexts");
  if (!leftE.hasContext() || !rightE.hasContext()) {
    return false;
  }
  ExpressionContext leftC = leftE.getContext();
  ExpressionContext rightC = rightE.getContext();
  ExpressionContext outputC(*leftE.owner);
  if (!leftC.mergeContexts(rightC, outputC)) {
    return false;
  }
  if (!leftE.setContextAtLeastEqualTo(outputC)) {
    return false;
  }
  return rightE.setContextAtLeastEqualTo(outputC);
}

bool ExpressionContext::isEmpty() {
  return
  this->indexAmbientSemisimpleLieAlgebra == - 1 &&
  this->variables.size == 0 &&
  this->differentialOperatorVariables.size == 0;
}

SemisimpleLieAlgebra* ExpressionContext::getAmbientSemisimpleLieAlgebra() const {
  if (this->indexAmbientSemisimpleLieAlgebra == - 1) {
    return nullptr;
  }
  return &this->owner->theObjectContainer.semisimpleLieAlgebras.theValues[
    this->indexAmbientSemisimpleLieAlgebra
  ];
}

std::string ExpressionContext::toString() const {
  std::stringstream out;
  this->checkInitialization();
  out << "Context. ";
  if (this->variables.size > 0) {
    out << "Variables: " << this->variables.toStringCommaDelimited() << ". ";
  }
  if (this->differentialOperatorVariables.size > 0) {
    out << "Differential operators: "
    << this->variables.toStringCommaDelimited() << ". ";
  }
  if (this->indexAmbientSemisimpleLieAlgebra != - 1) {
    out << "Ambient semisimple Lie algebra: "
    << this->getAmbientSemisimpleLieAlgebra()->theWeyl.theDynkinType.toString();
  }
  return out.str();
}

Expression ExpressionContext::getVariable(int variableIndex) const {
  MacroRegisterFunctionWithName("Expression::getVariable");
  if (variableIndex < 0 || variableIndex >= this->variables.size) {
    Expression errorE;
    std::stringstream out;
    out << "Context does not have variable index " << variableIndex + 1 << ". ";
    return errorE.makeError(out.str(), *this->owner);
  }
  return this->variables[variableIndex];
}

void ExpressionContext::setIndexAmbientSemisimpleLieAlgebra(
  int index
) {
  this->indexAmbientSemisimpleLieAlgebra = index;
}

bool ExpressionContext::setAmbientSemisimpleLieAlgebra(
  SemisimpleLieAlgebra& algebra
) {
  this->checkInitialization();
  MapReferences<DynkinType, SemisimpleLieAlgebra>& algebras = this->owner->theObjectContainer.semisimpleLieAlgebras;
  this->indexAmbientSemisimpleLieAlgebra = algebras.getIndex(
    algebra.theWeyl.theDynkinType
  );
  if (this->indexAmbientSemisimpleLieAlgebra == - 1) {
    global.fatal << "Attempt to add semisimple algebra of type "
    << algebra.theWeyl.theDynkinType << " which is unknown to the calculator. "
    << global.fatal;
  }
  return true;
}

bool Expression::ContextSetDiffOperatorVar(const Expression& thePolyVar, const Expression& theDiffOpVar) {
  if (!this->IsContext()) {
    global.fatal << "This is a programming error: calling "
    << "Expression::ContextSetDiffOperatorVar on a non-context expression. " << global.fatal;
  }
  Expression diffVarsE, polyVarsE;
  diffVarsE.reset(*this->owner, 2);
  diffVarsE.addChildAtomOnTop(this->owner->opWeylAlgebraVariables());
  diffVarsE.addChildOnTop(theDiffOpVar);
  polyVarsE.reset(*this->owner, 2);
  polyVarsE.addChildAtomOnTop(this->owner->opPolynomialVariables());
  polyVarsE.addChildOnTop(thePolyVar);
  bool foundDiffVarsE = false;
  bool foundPolyVarsE = false;
  for (int i = 0; i < this->children.size; i ++) {
    if ((*this)[i].startsWith(this->owner->opWeylAlgebraVariables())) {
      this->setChild(i, diffVarsE);
      foundDiffVarsE = true;
    } else if ((*this)[i].startsWith(this->owner->opPolynomialVariables())) {
      this->setChild(i, polyVarsE);
      foundPolyVarsE = true;
    }
  }
  if (!foundPolyVarsE) {
    this->addChildOnTop(polyVarsE);
  }
  if (!foundDiffVarsE) {
    this->addChildOnTop(diffVarsE);
  }
  return true;
}

bool ExpressionContext::fromExpressionOneContext(
  const Expression& input
) {
  if (input.startsWith(this->owner->opPolynomialVariables())) {
    return this->fromExpressionPolynomialVariables(input);
  }
  if (input.startsWith(this->owner->opWeylAlgebraVariables())) {
    return this->fromExpressionDifferentialOperatorVariables(input);
  }
  if (input.startsWith(this->owner->opSemisimpleLieAlgebrA())) {
    return this->fromExpressionSemisimpleLieAlgebra(input);
  }
  return *this->owner << "Uknown context type. ";
}

bool ExpressionContext::fromExpression(const Expression& input) {
  input.checkInitialization();
  Calculator& commands = *input.owner;
  this->initialize(commands);
  if (!input.IsContext()) {
    return commands
    << "Not allowed: call getContext from non-context expression: "
    << input << ". ";
  }
  for (int i = 1; i < input.size(); i ++) {
    if (!this->fromExpressionOneContext(input[i])) {
      return false;
    }
  }
  return true;
}

bool ExpressionContext::fromExpressionPolynomialVariables(
  const Expression& input
) {
  MacroRegisterFunctionWithName("Expression::fromExpressionPolynomialVariables");
  for (int i = 1; i < input.size(); i ++) {
    this->variables.addOnTop(input[i]);
  }
  return true;
}

bool ExpressionContext::fromExpressionDifferentialOperatorVariables(
  const Expression& input
) {
  MacroRegisterFunctionWithName("Expression::fromExpressionDifferentialOperatorVariables");
  for (int i = 1; i < input.size(); i ++) {
    this->differentialOperatorVariables.addOnTop(input[i]);
  }
  return true;
}

bool ExpressionContext::fromExpressionSemisimpleLieAlgebra(
  const Expression& input
) {
  MacroRegisterFunctionWithName("Expression::fromExpressionSemisimpleLieAlgebra");
  if (input.size() != 2) {
    return *this->owner << "Corrupt semisimple Lie algebra context: " << input.toString();
  }
  this->indexAmbientSemisimpleLieAlgebra = input[1].theData;
  return true;
}

bool ExpressionContext::mergeSemisimpleLieAlgebraContexts(
  const ExpressionContext& other,
  ExpressionContext& outputContext
) {
  int left = this->indexAmbientSemisimpleLieAlgebra;
  int right = other.indexAmbientSemisimpleLieAlgebra;
  if (left == - 1) {
    left = right;
  }
  if (right == - 1) {
    right = left;
  }
  if (left != right) {
    return false;
  }
  outputContext.indexAmbientSemisimpleLieAlgebra = left;
  return true;
}

bool ExpressionContext::mergeVariables(
  const ExpressionContext& other,
  ExpressionContext& outputContext
) {
  outputContext.variables.addOnTopNoRepetition(this->variables);
  outputContext.variables.addOnTopNoRepetition(other.variables);
  outputContext.variables.quickSortAscending();
  return true;
}

bool ExpressionContext::mergeDifferentialOperatorsOnce(
  Selection& differentialOperatorVariablesFound,
  ExpressionContext& outputContext
) const {
  if (this->variables.size != this->differentialOperatorVariables.size) {
    return *this->owner << "Context merge fail: variable "
    << "count does not match differential variable count. ";
  }
  for (int i = 0; i < this->variables.size; i ++) {
    const Expression& variable= this->variables[i];
    const Expression& differentialOperatorVariable = this->differentialOperatorVariables[i];
    int theIndex = outputContext.variables.getIndex(variable);
    if (differentialOperatorVariablesFound.selected[theIndex]) {
      if (
        differentialOperatorVariable !=
        outputContext.differentialOperatorVariables[theIndex]
      ) {
        return *this->owner
        << "<hr>Failed to merge context "
        << this->toString() << " into "
        << outputContext.toString()
        << " because " << variable.toString()
        << " has two different corresponding differential operator variables: "
        << outputContext.differentialOperatorVariables[theIndex].toString()
        << " and " << differentialOperatorVariable.toString();
      }
    }
    differentialOperatorVariablesFound.AddSelectionAppendNewIndex(theIndex);
    outputContext.differentialOperatorVariables[theIndex] = this->differentialOperatorVariables[i];
  }
  // Generate missing differntial operator variables.
  for (int i = 0; i < this->differentialOperatorVariables.size; i ++) {
    if (differentialOperatorVariablesFound.selected[i]) {
      continue;
    }
    Expression differentialOperator(*this->owner);
    differentialOperator.addChildAtomOnTop("\\partial");
    Expression variableIndex;
    variableIndex.assignValue(i, *this->owner);
    differentialOperator.addChildOnTop(variableIndex);
    if (outputContext.differentialOperatorVariables.contains(
      differentialOperator
    )) {
      return *this->owner << "<hr>Failed to merge context "
      << this->toString() << " into" << outputContext.toString()
      << ": " << outputContext.variables[i].toString()
      << " had no differential letter assigned to it. "
      << "I tried to assign automatically  "
      << differentialOperator.toString()
      << " but it was already taken. ";
    }
    outputContext.differentialOperatorVariables[i] = differentialOperator;
  }
  return true;
}

bool ExpressionContext::mergeDifferentialOperators(
  const ExpressionContext& other,
  ExpressionContext& outputContext
) {
  if (other.differentialOperatorVariables.size == 0) {
    outputContext.differentialOperatorVariables = this->differentialOperatorVariables;
    return true;
  }
  if (this->differentialOperatorVariables.size == 0) {
    outputContext.differentialOperatorVariables = other.differentialOperatorVariables;
    return true;
  }
  Selection foundEWAVar;
  foundEWAVar.initialize(outputContext.variables.size);
  outputContext.differentialOperatorVariables.setSize(outputContext.variables.size);
  if (!this->mergeDifferentialOperatorsOnce(foundEWAVar, outputContext)) {
    return false;
  }
  if (!other.mergeDifferentialOperatorsOnce(foundEWAVar, outputContext)) {
    return false;
  }
  return true;
}

bool ExpressionContext::mergeContexts(
  const ExpressionContext& other,
  ExpressionContext& outputContext
) {
  MacroRegisterFunctionWithName("Expression::ContextMergeContexts");
  if (this == &outputContext || &other == &outputContext) {
    ExpressionContext leftCopy = *this;
    ExpressionContext rightCopy = other;
    return leftCopy.mergeContexts(rightCopy, outputContext);
  }
  if (this->owner != other.owner) {
    return false;
  }
  outputContext.owner = this->owner;
  if (!this->mergeSemisimpleLieAlgebraContexts(other, outputContext)) {
    return false;
  }
  if (!this->mergeVariables(other, outputContext)) {
    return false;
  }
  if (!this->mergeDifferentialOperators(other, outputContext)) {
    return false;
  }
  return true;
}

FormatExpressions ExpressionContext::getFormat() const {
  FormatExpressions result;
  this->getFormat(result);
  return result;
}

void ExpressionContext::getFormat(FormatExpressions& output) const {
  output.polynomialAlphabet.setSize(0);
  output.polyDefaultLetter = "x";
  output.WeylAlgebraDefaultLetter = "\\partial";
  if (this->owner == nullptr) {
    return;
  }
  output.polynomialAlphabet.setSize(this->variables.size);
  for (int i = 0; i < this->variables.size; i ++) {
    output.polynomialAlphabet[i] = this->variables[i].toString();
  }
  output.weylAlgebraLetters.setSize(this->differentialOperatorVariables.size);
  for (int i = 0; i < this->differentialOperatorVariables.size; i ++) {
    output.weylAlgebraLetters[i] =
    this->differentialOperatorVariables[i].toString();
  }
}

template <class Coefficient>
bool ExpressionContext::polynomialSubstitution(
  const ExpressionContext& largerContext, PolynomialSubstitution<Coefficient>& output
) const {
  output.setSize(this->variables.size);
  for (int i = 0; i < this->variables.size; i ++) {
    int theNewIndex = largerContext.variables.getIndex(this->variables[i]);
    if (theNewIndex == - 1) {
      return false;
    }
    output[i].makeMonomial(theNewIndex, 1, 1);
  }
  return true;
}

template <class Coefficient>
void ExpressionContext::polynomialAndWeylAlgebraSubstitutionNoFailure(
  const ExpressionContext& largerContext,
  PolynomialSubstitution<Coefficient>& outputPolyPart,
  PolynomialSubstitution<Coefficient>& outputDifferntialPart
) const {
  bool mustBeTrue = this->polynomialAndWeylAlgebraSubstitution(
    largerContext, outputPolyPart, outputDifferntialPart
  );
  if (!mustBeTrue) {
    global.fatal << "This is a programming error: "
    << "I was not able to extract a polynomial / differential "
    << "operator substitution from smaller context "
    << this->toString() << " relative to larger context "
    << largerContext.toString() << global.fatal;
  }
}

template <class Coefficient>
bool ExpressionContext::polynomialAndWeylAlgebraSubstitution(
  const ExpressionContext& largerContext,
  PolynomialSubstitution<Coefficient>& outputPolynomialPart,
  PolynomialSubstitution<Coefficient>& outputDifferentialPart
) const {
  if (!this->polynomialSubstitution(largerContext, outputPolynomialPart)) {
    return false;
  }
  outputDifferentialPart.setSize(this->differentialOperatorVariables.size);
  for (int i = 0; i < this->differentialOperatorVariables.size; i ++) {
    int newIndex = largerContext.differentialOperatorVariables.getIndex(
      this->differentialOperatorVariables[i]
    );
    if (newIndex == - 1) {
      return false;
    }
    outputDifferentialPart[i].makeMonomial(newIndex, 1, 1);
  }
  return true;
}

bool Expression::setContextAtLeastEqualTo(ExpressionContext& inputOutputMinContext) {
  MacroRegisterFunctionWithName("Expression::setContextAtLeastEqualTo");
  this->checkInitialization();
  if (!this->isBuiltInType()) {
    global.fatal << "This is a programming error: calling "
    << "Expression::setContextAtLeastEqualTo on an expression "
    << "that is not of built-in type. "
    << "Contexts are Reserved for built-in data types. " << global.fatal;
  }
  ExpressionContext newContext(*this->owner);
  ExpressionContext oldContext = this->getContext();
  if (!oldContext.mergeContexts(inputOutputMinContext, newContext)) {
    return false;
  }
  if (oldContext == newContext) {
    return true;
  }
  inputOutputMinContext = newContext;
  if (this->isOfType<Rational>()) {
    this->setChild(1, inputOutputMinContext.toExpression());
    return true;
  }
  if (this->isOfType<ElementWeylGroup>()) {
    return this->assignValueWithContext(
      this->getValue<ElementWeylGroup>(), inputOutputMinContext, *this->owner
    );
  }
  if (this->isOfType<AlgebraicNumber>()) {
    return this->setChild(1, inputOutputMinContext.toExpression());
  }
  if (this->isOfType<ElementUniversalEnveloping<RationalFunction> > ()) {
    ElementUniversalEnveloping<RationalFunction> newUE = this->getValue<ElementUniversalEnveloping<RationalFunction> >();
    PolynomialSubstitution<Rational> subPolyPart;
    oldContext.polynomialSubstitutionNoFailure<Rational>(newContext, subPolyPart);
    newUE.substitution(subPolyPart);
    return this->assignValueWithContext(newUE, inputOutputMinContext, *this->owner);
  }
  if (this->isOfType<Polynomial<Rational> >()) {
    Polynomial<Rational> newPoly = this->getValue<Polynomial<Rational> >();
    PolynomialSubstitution<Rational> subPolyPart;
    oldContext.polynomialSubstitutionNoFailure<Rational>(newContext, subPolyPart);
    if (!newPoly.substitution(subPolyPart)) {
      return false;
    }
    return this->assignValueWithContext(newPoly, inputOutputMinContext, *this->owner);
  }
  if (this->isOfType<Polynomial<AlgebraicNumber> >()) {
    Polynomial<AlgebraicNumber> newPoly = this->getValue<Polynomial<AlgebraicNumber> >();
    PolynomialSubstitution<AlgebraicNumber> subPolyPart;
    oldContext.polynomialSubstitutionNoFailure<AlgebraicNumber>(newContext, subPolyPart);
    if (!newPoly.substitution(subPolyPart)) {
      return false;
    }
    return this->assignValueWithContext(newPoly, inputOutputMinContext, *this->owner);
  }
  if (this->isOfType<ElementWeylAlgebra<Rational> >()) {
    PolynomialSubstitution<Rational> subEWApart;
    PolynomialSubstitution<Rational> subPolyPart;
    oldContext.polynomialAndWeylAlgebraSubstitutionNoFailure(newContext, subPolyPart, subEWApart);
    ElementWeylAlgebra<Rational> outputEWA = this->getValue<ElementWeylAlgebra<Rational> >();
    if (!outputEWA.substitution(subPolyPart, subEWApart)) {
      this->owner->comments << "<hr>Failed to convert "
      << outputEWA.toString() << ": failed to carry out substitution "
      << subEWApart.toString() << ", " << subPolyPart.toString();
      return false;
    }
    return this->assignValueWithContext(outputEWA, inputOutputMinContext, *this->owner);
  }
  if (this->isOfType<RationalFunction>()) {
    RationalFunction newRF = this->getValue<RationalFunction>();
    PolynomialSubstitution<Rational> subPolyPart;
    oldContext.polynomialSubstitutionNoFailure(newContext, subPolyPart);
    newRF.substitution(subPolyPart);
    return this->assignValueWithContext(newRF, inputOutputMinContext, *this->owner);
  }
  if (this->isOfType<ElementTensorsGeneralizedVermas<RationalFunction> >()) {
    ElementTensorsGeneralizedVermas<RationalFunction> newETGV;
    newETGV = this->getValue<ElementTensorsGeneralizedVermas<RationalFunction> >();
    PolynomialSubstitution<Rational> subPolyPart;
    oldContext.polynomialSubstitutionNoFailure(newContext, subPolyPart);
    newETGV.substitution(subPolyPart, this->owner->theObjectContainer.theCategoryOmodules);
    return this->assignValueWithContext(newETGV, inputOutputMinContext, *this->owner);
  }
  if (this->isOfType<Weight<Polynomial<Rational> > >()) {
    PolynomialSubstitution<Rational> subPolyPart;
    oldContext.polynomialSubstitution(newContext, subPolyPart);
    Weight<Polynomial<Rational> > theWeight = this->getValue<Weight<Polynomial<Rational> > >();
    for (int i = 0; i < theWeight.weightFundamentalCoordS.size; i ++) {
      theWeight.weightFundamentalCoordS[i].substitution(subPolyPart);
    }
    return this->assignValueWithContext(theWeight, inputOutputMinContext, *this->owner);
  }
  if (this->isMatrixOfType<RationalFunction>()) {
    Matrix<RationalFunction> newMat;
    this->owner->functionGetMatrix(*this, newMat, &newContext);
    PolynomialSubstitution<Rational> subPolyPart;
    oldContext.polynomialSubstitutionNoFailure<Rational>(newContext, subPolyPart);
    for (int i = 0; i < newMat.numberOfRows; i ++) {
      for (int j = 0; j < newMat.numberOfColumns; j ++) {
        if (!newMat(i, j).substitution(subPolyPart)) {
          return *this->owner << "Failed to carry out the substitution "
          << subPolyPart.toString() << " in the matrix " << this->toString() << ". ";
        }
      }
    }
    return this->AssignMatrix(newMat, *this->owner, &inputOutputMinContext);
  }
  this->owner->comments << "Expression " << this->toString()
  << " is of built-in type but is not handled by Expression::setContextAtLeastEqualTo. ";
  return false;
}
