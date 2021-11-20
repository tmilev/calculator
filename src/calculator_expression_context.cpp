// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_interface.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_extra_weyl_algebras_implementation.h"
#include "math_extra_modules_semisimple_lie_algebras_implementation.h"
#include "math_rational_function_implementation.h"

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
  this->differentialOperatorVariables == other.differentialOperatorVariables &&
  this->defaultModulus == other.defaultModulus
  ;
}

template <class Coefficient>
void ExpressionContext::polynomialSubstitutionNoFailure(
  const ExpressionContext& largerContext,
  PolynomialSubstitution<Coefficient>& output,
  const Coefficient& one
) const {
  bool mustBeTrue = this->polynomialSubstitution(largerContext, output, one);
  if (!mustBeTrue) {
    global.fatal << "Unable to "
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

bool ExpressionContext::hasAtomicUserDefinedVariables() const {
  for (int i = 0; i < this->variables.size; i ++) {
    if (!this->variables[i].isAtomUserDefined()) {
      return false;
    }
  }
  return true;
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
  if (this->defaultModulus != 0) {
    result.addChildOnTop(this->toExpressionDefaultModulus());
  }
  if (this->variables.size > 0) {
    result.addChildOnTop(this->toExpressionPolynomialVariables());
  }
  if (this->differentialOperatorVariables.size > 0) {
    result.addChildOnTop(this->toExpressionDifferntialOperators());
  }
  return result;
}

Expression ExpressionContext::toExpressionDefaultModulus() const {
  Expression modularContext(*this->owner);
  modularContext.addChildAtomOnTop(this->owner->opMod());
  modularContext.addChildRationalOnTop(this->defaultModulus);
  return modularContext;
}

Expression ExpressionContext::toExpressionSemisimpleLieAlgebra() const {
  Expression algebraContext(*this->owner);
  algebraContext.addChildAtomOnTop(this->owner->opSemisimpleLieAlgebra());
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
    current.makeAtom(*this->owner, inputVariables[i]);
    this->variables.addOnTop(current);
  }
  return true;
}

bool Expression::mergeContexts(Expression& leftE, Expression& rightE) {
  MacroRegisterFunctionWithName("Expression::mergeContexts");
  if (!leftE.hasContext() || !rightE.hasContext()) {
    return false;
  }
  ExpressionContext leftC = leftE.getContext();
  ExpressionContext rightC = rightE.getContext();
  ExpressionContext outputC(*leftE.owner);
  if (!leftC.mergeContexts(rightC, outputC)) {
    return false;
  }
  if (!leftE.setContextAtLeastEqualTo(outputC, nullptr)) {
    return false;
  }
  return rightE.setContextAtLeastEqualTo(outputC, nullptr);
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
  return &this->owner->objectContainer.semisimpleLieAlgebras.values[
    this->indexAmbientSemisimpleLieAlgebra
  ];
}

std::string ExpressionContext::toString() const {
  std::stringstream out;
  // this->checkInitialization();
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
    << this->getAmbientSemisimpleLieAlgebra()->weylGroup.dynkinType.toString();
  }
  return out.str();
}

Expression ExpressionContext::getVariable(int variableIndex) const {
  MacroRegisterFunctionWithName("Expression::getVariable");
  if (variableIndex < 0 || variableIndex >= this->variables.size) {
    Expression errorE;
    std::stringstream out;
    out << "Context does not have variable index " << variableIndex + 1 << ". ";
    return errorE.assignError(*this->owner, out.str());
  }
  return this->variables[variableIndex];
}

void ExpressionContext::setDefaultModulus(const LargeIntegerUnsigned& input) {
  this->defaultModulus = input;
}

LargeIntegerUnsigned ExpressionContext::getDefaultModulus() {
  return this->defaultModulus;
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
  MapReferences<DynkinType, SemisimpleLieAlgebra>& algebras = this->owner->objectContainer.semisimpleLieAlgebras;
  this->indexAmbientSemisimpleLieAlgebra = algebras.getIndex(
    algebra.weylGroup.dynkinType
  );
  if (this->indexAmbientSemisimpleLieAlgebra == - 1) {
    global.fatal << "Attempt to add semisimple algebra of type "
    << algebra.weylGroup.dynkinType << " which is unknown to the calculator. "
    << global.fatal;
  }
  return true;
}

bool Expression::contextSetDifferentialOperatorVariable(
  const Expression& polynomialVariable, const Expression& differentialOperatorVariable
) {
  if (!this->isContext()) {
    global.fatal
    << "Calling Expression::contextSetDifferentialOperatorVariable on a non-context expression. "
    << global.fatal;
  }
  Expression diffVarsE, polyVarsE;
  diffVarsE.reset(*this->owner, 2);
  diffVarsE.addChildAtomOnTop(this->owner->opWeylAlgebraVariables());
  diffVarsE.addChildOnTop(differentialOperatorVariable);
  polyVarsE.reset(*this->owner, 2);
  polyVarsE.addChildAtomOnTop(this->owner->opPolynomialVariables());
  polyVarsE.addChildOnTop(polynomialVariable);
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
  if (input.startsWith(this->owner->opSemisimpleLieAlgebra())) {
    return this->fromExpressionSemisimpleLieAlgebra(input);
  }
  if (input.startsWith(this->owner->opMod())) {
    return this->fromExpressionDefaultModulus(input);
  }
  return *this->owner << "Uknown context type. ";
}

bool ExpressionContext::fromExpression(const Expression& input) {
  input.checkInitialization();
  Calculator& commands = *input.owner;
  this->initialize(commands);
  if (!input.isContext()) {
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
  this->indexAmbientSemisimpleLieAlgebra = input[1].data;
  return true;
}

bool ExpressionContext::fromExpressionDefaultModulus(
  const Expression& input
) {
  MacroRegisterFunctionWithName("Expression::fromExpressionSemisimpleLieAlgebra");
  if (input.size() != 2) {
    return *this->owner << "Corrupt modulus " << input.toString();
  }
  if (!input[1].isIntegerNonNegative(&this->defaultModulus)) {
    return *this->owner
    << "Corrupt modulus: modulus needs to be a non-negative integer: "
    << input.toString();
  }
  return true;
}

bool ExpressionContext::mergeModuli(
  const ExpressionContext& other, ExpressionContext& outputContext
) {
  outputContext.defaultModulus = this->defaultModulus;
  if (outputContext.defaultModulus == 0) {
    outputContext.defaultModulus = other.defaultModulus;
  }
  if (other.defaultModulus == 0 || this->defaultModulus == 0) {
    return true;
  }
  return outputContext.defaultModulus == other.defaultModulus;
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
    int index = outputContext.variables.getIndex(variable);
    if (differentialOperatorVariablesFound.selected[index]) {
      if (
        differentialOperatorVariable !=
        outputContext.differentialOperatorVariables[index]
      ) {
        return *this->owner
        << "<hr>Failed to merge context "
        << this->toString() << " into "
        << outputContext.toString()
        << " because " << variable.toString()
        << " has two different corresponding differential operator variables: "
        << outputContext.differentialOperatorVariables[index].toString()
        << " and " << differentialOperatorVariable.toString();
      }
    }
    differentialOperatorVariablesFound.addSelectionAppendNewIndex(index);
    outputContext.differentialOperatorVariables[index] = this->differentialOperatorVariables[i];
  }
  // Generate missing differntial operator variables.
  for (int i = 0; i < this->differentialOperatorVariables.size; i ++) {
    if (differentialOperatorVariablesFound.selected[i]) {
      continue;
    }
    Expression differentialOperator(*this->owner);
    differentialOperator.addChildAtomOnTop("\\partial");
    Expression variableIndex;
    variableIndex.assignValueOLD(i, *this->owner);
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
  MacroRegisterFunctionWithName("Expression::mergeContexts");
  if (this == &outputContext || &other == &outputContext) {
    ExpressionContext leftCopy = *this;
    ExpressionContext rightCopy = other;
    return leftCopy.mergeContexts(rightCopy, outputContext);
  }
  if (this->owner != nullptr) {
    outputContext.owner = this->owner;
  } else {
    outputContext.owner = other.owner;
  }
  if (this->owner != nullptr && other.owner != nullptr) {
    if (this->owner != other.owner) {
      return false;
    }
  }
  if (!this->mergeModuli(other, outputContext)) {
    return false;
  }
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
  output.weylAlgebraDefaultLetter = "\\partial";
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
  const ExpressionContext& largerContext,
  PolynomialSubstitution<Coefficient>& output,
  const Coefficient& one
) const {
  output.setSize(this->variables.size);
  for (int i = 0; i < this->variables.size; i ++) {
    int theNewIndex = largerContext.variables.getIndex(this->variables[i]);
    if (theNewIndex == - 1) {
      return false;
    }
    output[i].makeMonomial(theNewIndex, 1, one);
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
    global.fatal
    << "Unable to extract a polynomial / differential "
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
  if (!this->polynomialSubstitution(
    largerContext, outputPolynomialPart, Rational::one()
  )) {
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

template<>
bool WithContext<Rational>::extendContext(
  ExpressionContext& newContext, std::stringstream* commentsOnFailure
) {
  return this->extendContextTrivially(newContext, commentsOnFailure);
}

template<>
bool WithContext<std::string>::extendContext(
  ExpressionContext& newContext, std::stringstream* commentsOnFailure
) {
  return this->extendContextTrivially(newContext, commentsOnFailure);
}

template<>
bool WithContext<ElementWeylGroup>::extendContext(
  ExpressionContext& newContext, std::stringstream* commentsOnFailure
) {
  return this->extendContextTrivially(newContext, commentsOnFailure);
}

template<>
bool WithContext<AlgebraicNumber>::extendContext(
  ExpressionContext& newContext, std::stringstream* commentsOnFailure
) {
  return this->extendContextTrivially(newContext, commentsOnFailure);
}

template<>
bool WithContext<double>::extendContext(
  ExpressionContext& newContext, std::stringstream* commentsOnFailure
) {
  return this->extendContextTrivially(newContext, commentsOnFailure);
}

template<>
bool WithContext<ElementZmodP>::extendContext(
  ExpressionContext& newContext, std::stringstream* commentsOnFailure
) {
  LargeIntegerUnsigned contextModulus = newContext.getDefaultModulus();
  if (this->content.modulus != contextModulus) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "The modulus of " << this->content.toString()
      << " does not equal incoming context modulus: " << contextModulus << ". ";
    }
    return false;
  }
  this->context = newContext;
  return true;
}

template<>
bool WithContext<ElementUniversalEnveloping<RationalFraction<Rational> > >::extendContext(
  ExpressionContext& newContext, std::stringstream* commentsOnFailure
){
  (void) commentsOnFailure;
  PolynomialSubstitution<Rational> substitution;
  this->context.polynomialSubstitutionNoFailure(newContext, substitution, Rational::one());
  this->content.substitution(substitution);
  this->context = newContext;
  return true;
}

template<>
bool WithContext<Polynomial<Rational> >::extendContext(
  ExpressionContext& newContext, std::stringstream* commentsOnFailure
){
  (void) commentsOnFailure;
  PolynomialSubstitution<Rational> substitution;
  this->context.polynomialSubstitutionNoFailure(newContext, substitution, Rational::one());
  if (!this->content.substitution(substitution, Rational::one())) {
    return false;
  }
  this->context = newContext;
  return true;
}

template<>
bool WithContext<Polynomial<ElementZmodP> >::extendContext(
  ExpressionContext& newContext, std::stringstream* commentsOnFailure
){
  (void) commentsOnFailure;
  if (!this->content.isEqualToZero()) {
    PolynomialSubstitution<ElementZmodP> substitution;
    const ElementZmodP& one = this->content.coefficients[0].one();
    this->context.polynomialSubstitutionNoFailure(
      newContext, substitution, one
    );
    if (!this->content.substitution(substitution, one)) {
      return false;
    }
  }
  this->context = newContext;
  return true;
}

template<>
bool WithContext<Polynomial<AlgebraicNumber> >::extendContext(
  ExpressionContext& newContext, std::stringstream* commentsOnFailure
){
  (void) commentsOnFailure;
  PolynomialSubstitution<AlgebraicNumber> substitution;
  this->context.polynomialSubstitutionNoFailure<AlgebraicNumber>(
    newContext,
    substitution,
    this->context.owner->objectContainer.algebraicClosure.one()
  );
  if (!this->content.substitution(substitution, Rational::one())) {
    return false;
  }
  this->context = newContext;
  return true;
}

template<>
bool WithContext<ElementWeylAlgebra<Rational> >::extendContext(
  ExpressionContext& newContext, std::stringstream* commentsOnFailure
){
  MacroRegisterFunctionWithName("WithContext_ElementWeylAlgebra_Rational::extendContext");
  PolynomialSubstitution<Rational> substitutionDifferentialOperatorPart;
  PolynomialSubstitution<Rational> substitutionPolynomialPart;
  global.comments << "<hr>DEBUG: setting context " << newContext.toString()
  << " to: " << this->toString() << "<br>";
  this->context.polynomialAndWeylAlgebraSubstitutionNoFailure(
    newContext, substitutionPolynomialPart, substitutionDifferentialOperatorPart
  );
  global.comments << "DEBUG: Resulting subs: "
  << substitutionDifferentialOperatorPart.toString() << "<br>";
  if (!this->content.substitution(
    substitutionPolynomialPart, substitutionDifferentialOperatorPart
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "<hr>Failed to convert "
      << this->content.toString() << ": failed to carry out substitution "
      << substitutionDifferentialOperatorPart.toString()
      << ", " << substitutionPolynomialPart.toString();
    }
    global.comments << "DEBUG: FAILED TO CONVERT " << this->toString() << "<hr>";
    return false;
  }
  global.comments << "DEBUG: final: " << this->toString() << "<hr>";
  this->context = newContext;
  return true;
}

template<>
bool WithContext<RationalFraction<Rational> >::extendContext(
  ExpressionContext& newContext, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("WithContext_RationalFunction_Rational::extendContext");
  PolynomialSubstitution<Rational> substitution;
  this->context.polynomialSubstitutionNoFailure(newContext, substitution, Rational::one());
  if (!this->content.substitution(substitution, Rational::one(), commentsOnFailure)) {
    // This is not supposed to happen.
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "<hr>Failed to apply substitution: "
      << substitution.toString()
      << " to " << this->content.toString();
    }
    return false;
  }
  this->context = newContext;
  return true;
}

template<>
bool WithContext<RationalFraction<AlgebraicNumber> >::extendContext(
  ExpressionContext& newContext, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("WithContext_RationalFunction_AlgebraicNumber::extendContext");
  PolynomialSubstitution<AlgebraicNumber> substitution;
  AlgebraicClosureRationals& closure = this->context.owner->objectContainer.algebraicClosure;
  this->context.polynomialSubstitutionNoFailure(newContext, substitution, closure.one());
  if (!this->content.substitution(substitution, closure.one(), commentsOnFailure)) {
    // This is not supposed to happen.
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "<hr>Failed to apply substitution: "
      << substitution.toString()
      << " to " << this->content.toString();
    }
    return false;
  }
  this->context = newContext;
  return true;
}

template<>
bool WithContext<ElementTensorsGeneralizedVermas<RationalFraction<Rational> > >::extendContext(
  ExpressionContext& newContext, std::stringstream* commentsOnFailure
) {
  (void) commentsOnFailure;
  PolynomialSubstitution<Rational> substitution;
  this->context.polynomialSubstitutionNoFailure(newContext, substitution, Rational::one());
  this->content.substitution(substitution, this->context.owner->objectContainer.categoryOModules);
  this->context = newContext;
  return true;
}

template<>
bool WithContext<Weight<Polynomial<Rational> > >::extendContext(
  ExpressionContext& newContext, std::stringstream* commentsOnFailure
) {
  (void) commentsOnFailure;
  PolynomialSubstitution<Rational> substitution;
  this->context.polynomialSubstitution(newContext, substitution, Rational::one());
  for (int i = 0; i < this->content.weightFundamentalCoordinates.size; i ++) {
    this->content.weightFundamentalCoordinates[i].substitution(substitution, Rational::one());
  }
  this->context = newContext;
  return true;
}

bool Expression::setContextAtLeastEqualTo(
  ExpressionContext& inputOutputMinContext, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Expression::setContextAtLeastEqualTo");
  this->checkInitialization();
  if (!this->isBuiltInType()) {
    global.fatal << "Calling "
    << "Expression::setContextAtLeastEqualTo on an expression "
    << "that is not of built-in type. "
    << "Contexts are Reserved for built-in data types. " << global.fatal;
  }
  WithContext<Rational> rational;
  if (this->isOfTypeWithContext(&rational)) {
    return rational.setContextAndSerialize(inputOutputMinContext, *this, commentsOnFailure);
  }
  WithContext<std::string> stringValue;
  if (this->isOfTypeWithContext(&stringValue)) {
    return stringValue.setContextAndSerialize(inputOutputMinContext, *this, commentsOnFailure);
  }
  WithContext<ElementZmodP> modularElement;
  if (this->isOfTypeWithContext(&modularElement)) {
    return modularElement.setContextAndSerialize(inputOutputMinContext, *this, commentsOnFailure);
  }
  WithContext<double> floatingPointNumber;
  if (this->isOfTypeWithContext(&floatingPointNumber)) {
    return floatingPointNumber.setContextAndSerialize(
      inputOutputMinContext, *this, commentsOnFailure
    );
  }
  WithContext<ElementWeylGroup> elementWeylGroup;
  if (this->isOfTypeWithContext(&elementWeylGroup)) {
    return elementWeylGroup.setContextAndSerialize(inputOutputMinContext, *this, commentsOnFailure);
  }
  WithContext<AlgebraicNumber> algebraicNumber;
  if (this->isOfTypeWithContext(&algebraicNumber)) {
    return algebraicNumber.setContextAndSerialize(inputOutputMinContext, *this, commentsOnFailure);
  }
  WithContext<ElementUniversalEnveloping<RationalFraction<Rational> > > elementUniversalEnveloping;
  if (this->isOfTypeWithContext(&elementUniversalEnveloping)) {
    return elementUniversalEnveloping.setContextAndSerialize(inputOutputMinContext, *this, commentsOnFailure);
  }
  WithContext<Polynomial<Rational> > polynomial;
  if (this->isOfTypeWithContext(&polynomial)) {
    return polynomial.setContextAndSerialize(inputOutputMinContext, *this, commentsOnFailure);
  }
  WithContext<Polynomial<ElementZmodP> > polynomialModular;
  if (this->isOfTypeWithContext(&polynomialModular)) {
    return polynomialModular.setContextAndSerialize(inputOutputMinContext, *this, commentsOnFailure);
  }
  WithContext<Polynomial<AlgebraicNumber> > polynomialAlgebraic;
  if (this->isOfTypeWithContext(&polynomialAlgebraic)) {
    return polynomialAlgebraic.setContextAndSerialize(inputOutputMinContext, *this, commentsOnFailure);
  }
  WithContext<RationalFraction<Rational> > rationalFunction;
  if (this->isOfTypeWithContext(&rationalFunction)) {
    return rationalFunction.setContextAndSerialize(inputOutputMinContext, *this, commentsOnFailure);
  }
  WithContext<RationalFraction<AlgebraicNumber> > rationalFunctionAlgebraic;
  if (this->isOfTypeWithContext(&rationalFunctionAlgebraic)) {
    return rationalFunctionAlgebraic.setContextAndSerialize(inputOutputMinContext, *this, commentsOnFailure);
  }
  WithContext<ElementWeylAlgebra<Rational> > elementWeylAlgebra;
  if (this->isOfTypeWithContext(&elementWeylAlgebra)) {
    return elementWeylAlgebra.setContextAndSerialize(inputOutputMinContext, *this, commentsOnFailure);
  }
  WithContext<ElementTensorsGeneralizedVermas<RationalFraction<Rational> > > elementTensorProductGeneralizedVermaModules;
  if (this->isOfTypeWithContext(&elementTensorProductGeneralizedVermaModules)) {
    return elementTensorProductGeneralizedVermaModules.setContextAndSerialize(inputOutputMinContext, *this, commentsOnFailure);
  }
  WithContext<Weight<Polynomial<Rational> > > weightPolynomial;
  if (this->isOfTypeWithContext(&weightPolynomial)) {
    return weightPolynomial.setContextAndSerialize(inputOutputMinContext, *this, commentsOnFailure);
  }
  if (this->isMatrixOfType<RationalFraction<Rational> >()) {
    ExpressionContext newContext(*this->owner);
    ExpressionContext oldContext = this->getContext();
    if (!oldContext.mergeContexts(inputOutputMinContext, newContext)) {
      return false;
    }
    if (oldContext == newContext) {
      return true;
    }
    inputOutputMinContext = newContext;
    Matrix<RationalFraction<Rational> > newMatrix;
    this->owner->functionGetMatrix(*this, newMatrix, &newContext);
    PolynomialSubstitution<Rational> substitution;
    oldContext.polynomialSubstitutionNoFailure<Rational>(newContext, substitution, Rational::one());
    for (int i = 0; i < newMatrix.numberOfRows; i ++) {
      for (int j = 0; j < newMatrix.numberOfColumns; j ++) {
        if (!newMatrix(i, j).substitution(substitution, Rational::one(), commentsOnFailure)) {
          return *this->owner << "Failed to carry out the substitution "
          << substitution.toString() << " in the matrix " << this->toString() << ". ";
        }
      }
    }
    return this->makeMatrix(*this->owner, newMatrix, &inputOutputMinContext);
  }
  this->owner->comments << "Expression " << this->toString()
  << " is of built-in type but is not handled by Expression::setContextAtLeastEqualTo. ";
  return false;
}
