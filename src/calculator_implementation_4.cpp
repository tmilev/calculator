 // The current file is licensed under the license terms found in the main header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "math_general_implementation.h"
#include "math_extra_weyl_algebras_implementation.h"
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_extra_modules_semisimple_lie_algebras_implementation.h"
#include "math_extra_semisimple_lie_algebras_implementation.h"
#include "math_extra_finite_groups_implementation.h"
#include "string_constants.h"
#include "math_rational_function_implementation.h"
#include "math_extra_differential_operators.h"
#include <cmath>
#include <cfloat>

Calculator::Calculator() {
  this->parser.initialize(this);
  this->numOutputFileS = 0;
  this->flagHideLHS = false;
  this->flagUseHtml = true;
  this->flagWriteLatexPlots = true;
  this->flagUseLnInsteadOfLog = false;
  this->flagPlotShowJavascriptOnly = false;
  this->numberExpectedExpressionsAtInitialization = - 1;
  this->mode = Calculator::Mode::full;
  this->examples.owner = this;
}

MemorySaving<Calculator>& GlobalVariables::calculator() {
  static MemorySaving<Calculator> result;
  return result;
}

std::string Calculator::writeDefaultLatexFileReturnHtmlLink(
  const std::string& fileContent,
  std::string* outputFileNameNoExtension,
  bool useLatexDviPSpsToPNG
) {
  std::fstream fileStream;
  std::stringstream fileName;
  std::stringstream systemCommand1, systemCommand2, systemCommand3;
  fileName << "defaultOutput" << HashFunctions::hashFunction(fileContent);
  if (
    !FileOperations::openFileCreateIfNotPresentVirtual(
      fileStream,
      global.displayPathOutputFolder + fileName.str() + ".tex",
      false,
      true,
      false
    )
  ) {
    return "failed to create file: " + fileName.str() + ".tex";
  }
  std::string baseFolder;
  std::string fileNameWithPathPhysical;
  FileOperations::getPhysicalFileNameFromVirtual(
    "output/", baseFolder, false, false, nullptr
  );
  FileOperations::getPhysicalFileNameFromVirtual(
    "output/" + fileName.str(),
    fileNameWithPathPhysical,
    false,
    false,
    nullptr
  );
  fileStream << fileContent;
  fileStream.flush();
  fileStream.close();
  systemCommand1
  << " latex -output-directory="
  << baseFolder
  << " "
  << fileName.str()
  << ".tex";
  global.externalCommandNoOutput(systemCommand1.str(), true);
  if (useLatexDviPSpsToPNG) {
    systemCommand2
    << " dvips -o "
    << fileNameWithPathPhysical
    << ".ps "
    << fileNameWithPathPhysical
    << ".dvi";
    global.externalCommandNoOutput(systemCommand2.str(), true);
    systemCommand3
    << " convert "
    << fileNameWithPathPhysical
    << ".ps "
    << fileNameWithPathPhysical
    << ".png";
    global.externalCommandNoOutput(systemCommand3.str(), true);
  }
  std::stringstream out;
  out
  << "<img src =\""
  << global.displayPathOutputFolder
  << fileName.str()
  << ".png\"></img><a href=\""
  << global.displayPathOutputFolder
  << fileName.str()
  << ".tex\">"
  << global.displayPathOutputFolder
  << fileName.str()
  << ".tex</a>";
  if (outputFileNameNoExtension != nullptr) {
    *outputFileNameNoExtension =
    global.displayPathOutputFolder + fileName.str();
  }
  this->numOutputFileS ++;
  return out.str();
}

bool Calculator::getVectorExpressions(
  const Expression& input,
  List<Expression>& output,
  int targetDimNonMandatory
) {
  MacroRegisterFunctionWithName("Calculator::getVectorExpressions");
  input.checkInitialization();
  output.reserve(input.size());
  output.setSize(0);
  if (
    !input.isSequenceNElements() &&
    !input.startsWith(this->opIntervalOpen())
  ) {
    if (targetDimNonMandatory > 0) {
      if (targetDimNonMandatory != 1) {
        return
        *this
        << "<hr>getVector failure: target dim is "
        << targetDimNonMandatory
        << " but the input "
        << input.toString()
        << " can only be interpreted as a single element";
      }
    }
    output.addOnTop(input);
    return true;
  }
  if (targetDimNonMandatory > 0) {
    if (targetDimNonMandatory != input.size() - 1) {
      return
      *this
      << "<hr>Failed to getVector: the input is required to have "
      << targetDimNonMandatory
      << " columns but it has "
      << input.size() - 1
      << " columns instead. <hr>";
    }
  }
  targetDimNonMandatory = input.size() - 1;
  for (int i = 0; i < targetDimNonMandatory; i ++) {
    const Expression& current = input[i + 1];
    current.checkInitialization();
    output.addOnTop(current);
  }
  return true;
}

template <class Element>
void MathRoutines::lieBracket(
  const Element& standsOnTheLeft,
  const Element& standsOnTheRight,
  Element& output
) {
  if (&standsOnTheLeft == &output || &standsOnTheRight == &output) {
    Element standsOnTheLeftNew, standsOnTheRightNew;
    standsOnTheLeftNew = standsOnTheLeft;
    standsOnTheRightNew = standsOnTheRight;
    MathRoutines::lieBracket(standsOnTheLeftNew, standsOnTheRightNew, output);
    return;
  }
  Element tempE;
  output = standsOnTheLeft;
  output *= standsOnTheRight;
  tempE = standsOnTheRight;
  tempE *= standsOnTheLeft;
  output -= tempE;
}

bool Calculator::recursionDepthExceededHandleRoughly(
  const std::string& additionalErrorInfo
) {
  if (this->recursionDepth <= this->maximumRecursionDepth) {
    return false;
  }
  if (!this->flagMaxRecursionErrorEncountered) {
    *this
    << additionalErrorInfo
    << "<span style =\"color:#FF0000\"><b> maximum recursion depth of "
    << this->maximumRecursionDepth
    << " exceeded. </b></span>"
    << "Aborting computation ASAP. ";
  }
  this->flagAbortComputationASAP = true;
  this->flagMaxRecursionErrorEncountered = true;
  return true;
}

bool Calculator::checkOperationHandlers() {
  MacroRegisterFunctionWithName("Calculator::checkOperationHandlers");
  for (int i = 0; i < this->operations.size(); i ++) {
    MemorySaving<Calculator::OperationHandlers>& current =
    this->operations.values[i];
    if (current.isZeroPointer()) {
      continue;
    }
    current.getElement().checkConsistency();
    Calculator::OperationHandlers& allHandlers = current.getElement();
    for (int j = 0; j < allHandlers.compositeHandlers.size; j ++) {
      allHandlers.compositeHandlers[j].checkConsistency();
    }
    for (int j = 0; j < allHandlers.handlers.size; j ++) {
      allHandlers.handlers[j].checkConsistency();
    }
  }
  return true;
}

bool Calculator::checkConsistencyAfterInitialization() {
  this->allChildExpressions.grandMasterConsistencyCheck();
  this->evaluatedExpressionsStack.grandMasterConsistencyCheck();
  if (this->numberExpectedExpressionsAtInitialization < 0) {
    this->numberExpectedExpressionsAtInitialization =
    this->allChildExpressions.size;
  } else if (
    this->allChildExpressions.size !=
    this->numberExpectedExpressionsAtInitialization
  ) {
    global.fatal
    << "Expression container expected to have "
    << this->numberExpectedExpressionsAtInitialization
    << " elements but instead it has "
    << this->allChildExpressions.size
    << ". Expression container: ";
    for (int i = 0; i < this->allChildExpressions.size; i ++) {
      global.fatal << this->allChildExpressions[i].toString() << ", ";
    }
    global.fatal << global.fatal;
  }
  if (
    this->cachedExpressions.size() != 0 ||
    this->evaluatedExpressionsStack.size != 0
  ) {
    global.fatal
    << "Cached expressions, evaluatedExpressionsStack "
    << "are supposed to be empty, but "
    << "instead they contain respectively "
    << this->cachedExpressions.size()
    << ", "
    << this->evaluatedExpressionsStack.size
    << " elements. "
    << global.fatal;
  }
  return this->objectContainer.checkConsistencyAfterReset();
}

bool Expression::checkInitializationRecursively() const {
  MacroRegisterFunctionWithName("Expression::checkInitializationRecursively");
  this->checkInitialization();
  for (int i = 0; i < this->children.size; i ++) {
    (*this)[i].checkInitializationRecursively();
  }
  return true;
}

bool Expression::checkInitialization() const {
  if (this->owner == nullptr) {
    global.fatal << "Expression has non-initialized owner. " << global.fatal;
    return false;
  }
  return true;
}

bool Expression::hasInputBoxVariables(
  HashedList<std::string>* outputBoxNames,
  HashedList<std::string>*
  outputBoxNamesJavascript
) const {
  MacroRegisterFunctionWithName("Expression::hasInputBoxVariables");
  if (this->owner == nullptr) {
    return false;
  }
  RecursionDepthCounter recursionCounter(&this->owner->recursionDepth);
  if (this->owner->recursionDepth > this->owner->maximumRecursionDepth) {
    global.fatal
    << "Function hasInputBoxVariables has exceeded "
    << "recursion depth limit. "
    << global.fatal;
  }
  InputBox box;
  if (this->isOfType<InputBox>(&box)) {
    if (outputBoxNames != nullptr) {
      outputBoxNames->addOnTopNoRepetition(box.name);
    }
    if (outputBoxNamesJavascript != nullptr) {
      outputBoxNamesJavascript->addOnTopNoRepetition(box.getSliderName());
    }
    return true;
  }
  bool result = false;
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].hasInputBoxVariables(
        outputBoxNames, outputBoxNamesJavascript
      )
    ) {
      if (outputBoxNames == nullptr && outputBoxNamesJavascript == nullptr) {
        // We don't care for the names of all input boxes.
        return true;
      }
      result = true;
    }
  }
  return result;
}

bool Expression::hasBoundVariables() const {
  if (this->owner == nullptr) {
    global.fatal
    << "Calling function "
    << "hasBoundVariables on non-initialized expression. "
    << global.fatal;
  }
  RecursionDepthCounter recursionCounter(&this->owner->recursionDepth);
  MacroRegisterFunctionWithName("Expression::hasBoundVariables");
  if (this->owner->recursionDepth > this->owner->maximumRecursionDepth) {
    global.fatal
    << "Function hasBoundVariables has exceeded recursion depth limit. "
    << global.fatal;
  }
  if (this->isListOfTwoAtomsStartingWith(this->owner->opBind())) {
    return true;
  }
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].hasBoundVariables()) {
      return true;
    }
  }
  return false;
}

bool Calculator::appendOpandsReturnTrueIfOrderNonCanonical(
  const Expression& input, List<Expression>& output, int operation
) {
  RecursionDepthCounter recursionCounter(&this->recursionDepth);
  if (this->recursionDepth > this->maximumRecursionDepth) {
    return false;
  }
  bool result = false;
  if (!input.isListStartingWithAtom(operation)) {
    output.addOnTop(input);
  } else {
    for (int i = 1; i < input.size(); i ++) {
      if (
        this->appendOpandsReturnTrueIfOrderNonCanonical(
          input[i], output, operation
        )
      ) {
        result = true;
      }
      if (
        i < input.size() - 1 &&
        input[i].isListStartingWithAtom(operation) &&
        input[i].size() > 2
      ) {
        result = true;
      }
    }
  }
  return result;
}

bool CalculatorBasics::tensorProductStandard(
  Calculator& calculator, const Expression& input, Expression& output
) {
  RecursionDepthCounter recursionIncrementer(&calculator.recursionDepth);
  MacroRegisterFunctionWithName("Calculator::outerTensorProductStandard");
  if (
    CalculatorBasics::distribute(
      calculator,
      input,
      output,
      calculator.opPlus(),
      calculator.opTensor()
    )
  ) {
    return true;
  }
  if (CalculatorBasics::associate(calculator, input, output)) {
    return true;
  }
  if (
    CalculatorBasics::extractBaseMultiplication(calculator, input, output)
  ) {
    return true;
  }
  return false;
}

bool CalculatorBasics::multiplyAtoXtimesAtoYequalsAtoXplusY(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName(
    "Calculator::multiplyAtoXtimesAtoYequalsAtoXplusY"
  );
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  Expression constPower, power;
  const Expression* left = &input[1];
  const Expression* right = &input[2];
  if (*left == *right) {
    constPower.assignValue(calculator, 2);
    output.makeXOX(calculator, calculator.opPower(), *left, constPower);
    return true;
  }
  for (int i = 0; i < 2; i ++, MathRoutines::swap(left, right)) {
    if (right->startsWith(calculator.opPower(), 3)) {
      if ((*right)[1] == (*left)) {
        bool isGood = true;
        if ((*right)[2].isOfType<Rational>()) {
          if (!(*right)[2].getValue<Rational>().isInteger()) {
            Rational ratBase;
            if ((*right)[1].isRational(&ratBase)) {
              if (ratBase > 0) {
                isGood = false;
              }
            }
          }
        }
        if (isGood) {
          power.makeXOX(
            calculator,
            calculator.opPlus(), (*right)[2],
            calculator.expressionOne()
          );
          return
          output.makeXOX(
            calculator, calculator.opPower(), *left, power
          );
        }
      }
      if (left->startsWith(calculator.opPower(), 3)) {
        if ((*left)[1] == (*right)[1]) {
          bool isGood = (*left)[2].isInteger() || (*right)[2].isInteger();
          if (!isGood) {
            isGood = (*left)[1].isKnownToBeNonNegative();
          }
          if (!isGood) {
            continue;
          }
          Rational leftRat, rightRat;
          if (!isGood) {
            if ((*left)[2].isOfType<Rational>(&leftRat) && (*right)[2].isOfType<
                Rational
              >(&rightRat)
            ) {
              if (leftRat.isInteger() && !rightRat.isInteger()) {
                isGood = true;
              }
              if (!leftRat.isInteger() && rightRat.isInteger()) {
                isGood = true;
              }
            }
          }
          if (!isGood) {
            continue;
          }
          power.makeXOX(
            calculator,
            calculator.opPlus(), (*left)[2], (*right)[2]
          );
          return
          output.makeXOX(
            calculator, calculator.opPower(), (*left)[1], power
          );
        }
      }
    }
  }
  return false;
}

bool CalculatorBasics::combineFractions(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (!input.startsWith(calculator.opPlus(), 3)) {
    return false;
  }
  const Expression* quotientE = nullptr;
  const Expression* summandE = nullptr;
  if (input[1].startsWith(calculator.opDivide(), 3)) {
    quotientE = &input[1];
    summandE = &input[2];
  } else if (input[2].startsWith(calculator.opDivide(), 3)) {
    quotientE = &input[2];
    summandE = &input[1];
  } else {
    return false;
  }
  const Expression& quotientNumeratorE = (*quotientE)[1];
  const Expression& quotientDenominatorE = (*quotientE)[2];
  Expression outputSummandE, outputNumE;
  outputSummandE.makeXOX(
    calculator, calculator.opTimes(), *summandE, quotientDenominatorE
  );
  outputNumE.makeXOX(
    calculator, calculator.opPlus(), quotientNumeratorE, outputSummandE
  );
  return
  output.makeXOX(
    calculator, calculator.opDivide(), outputNumE, quotientDenominatorE
  );
}

bool CalculatorBasics::checkRule(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (!input.startsWith(calculator.opDefine(), 3)) {
    return false;
  }
  if (input[1] != input[2]) {
    return false;
  }
  std::stringstream out;
  out
  << "Bad rule: you are asking me to substitute "
  << input[1]
  << " by itself ("
  << input[2]
  << ")"
  << " which is an infinite substitution cycle. ";
  return output.assignError(calculator, out.str());
}

bool CalculatorBasics::subZeroDivAnythingWithZero(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (!input.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  if (input[1].isEqualToZero()) {
    if (!input[2].isEqualToZero()) {
      return output.assignValue(calculator, 0);
    }
  }
  return false;
}

bool CalculatorBasics::cancelMultiplicativeInverse(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  if (!input[1].startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  if (input[1][2] == input[2]) {
    output = input[1][1];
    return true;
  }
  return false;
}

bool CalculatorBasics::associateTimesDivision(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  if (!input[2].startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  Expression newLeftE;
  newLeftE.makeXOX(
    calculator, calculator.opTimes(), input[1], input[2][1]
  );
  output.makeXOX(
    calculator, calculator.opDivide(), newLeftE, input[2][2]
  );
  return true;
}

bool CalculatorBasics::associate(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorBasics::associate");
  if (input.size() != 3) {
    return false;
  }
  int operation = input[0].data;
  if (
    !input[1].startsWith(operation) && !input[2].startsWith(operation)
  ) {
    return false;
  }
  // int64_t startTime = global.getElapsedMilliseconds();
  List<Expression> multiplicands;
  calculator.collectOpands(input, operation, multiplicands);
  Expression result;
  result.makeOXdotsX(calculator, operation, multiplicands);
  if (result == input) {
    return false;
  }
  output = result;
  return true;
}

bool CalculatorBasics::standardIsDenotedBy(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::standardIsDenotedBy");
  RecursionDepthCounter recursionIncrementer(&calculator.recursionDepth);
  if (!input.startsWith(calculator.opIsDenotedBy(), 3)) {
    return false;
  }
  const Expression& withNotation = input[2];
  const Expression& notation = input[1];
  calculator
  << "<br>Registering notation: globally, "
  << withNotation.toString()
  << " will be denoted by "
  << notation.toString();
  calculator.objectContainer.expressionNotation.addOnTop(
    notation.toString()
  );
  calculator.objectContainer.expressionWithNotation.addOnTop(withNotation);
  output = input;
  output.setChildAtomValue(0, calculator.opDefine());
  // //
  if (
    withNotation.isOfType<
      ElementTensorsGeneralizedVermas<RationalFraction<Rational> >
    >()
  ) {
    if (
      withNotation.getValue<
        ElementTensorsGeneralizedVermas<RationalFraction<Rational> >
      >().isHighestWeightVector()
    ) {
      MonomialGeneralizedVerma<RationalFraction<Rational> >& element =
      withNotation.getValue<
        ElementTensorsGeneralizedVermas<RationalFraction<Rational> >
      >()[0].monomials[0];
      element.getOwner().highestWeightVectorNotation = notation.toString();
    }
  }
  return true;
}

bool CalculatorBasics::multiplyByOne(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::multiplyByOne");
  if (
    !input.isListStartingWithAtom(calculator.opTimes()) || input.size() != 3
  ) {
    return false;
  }
  if (!input[1].isEqualToOne()) {
    return false;
  }
  output = input[2];
  return true;
}

bool Calculator::getVectorLargeIntegerFromFunctionArguments(
  const Expression& input, List<LargeInteger>& output
) {
  MacroRegisterFunctionWithName(
    "Calculator::getVectorLargeIntegerFromFunctionArguments"
  );
  Vector<Rational> rationals;
  if (!this->getVectorFromFunctionArguments(input, rationals)) {
    return false;
  }
  output.initializeFillInObject(rationals.size, 0);
  for (int i = 0; i < rationals.size; i ++) {
    if (!rationals[i].isInteger(&output[i])) {
      return
      *this
      << "<hr>Succeeded to convert "
      << input.toString()
      << " to the vector of rationals: "
      << rationals.toString()
      << " but failed to convert that to list of integers. ";
    }
  }
  return true;
}

bool Calculator::getVectorInt(const Expression& input, List<int>& output) {
  MacroRegisterFunctionWithName("Calculator::getVectorInt");
  Vector<Rational> rationals;
  if (!this->getVector(input, rationals)) {
    return false;
  }
  output.initializeFillInObject(rationals.size, 0);
  for (int i = 0; i < rationals.size; i ++) {
    if (!rationals[i].isSmallInteger(&output[i])) {
      return
      *this
      << "<hr>Succeeded to convert "
      << input.toString()
      << " to the vector of rationals: "
      << rationals.toString()
      << " but failed to convert that to list of small integers. ";
    }
  }
  return true;
}

bool CalculatorBasics::timesToFunctionApplication(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::outerTimesToFunctionApplication");
  if (!input.startsWith(calculator.opTimes())) {
    return false;
  }
  if (input.size() < 2) {
    return false;
  }
  const Expression& firstElt = input[1];
  if (!firstElt.isBuiltInAtom()) {
    if (!firstElt.startsWith(calculator.opPower(), 3)) {
      return false;
    }
    if (!firstElt[1].isAtomWhoseExponentsAreInterpretedAsFunction()) {
      return false;
    }
  }
  if (firstElt.isAtomNotInterpretedAsFunction()) {
    return false;
  }
  const Expression& secondElt = input[2];
  if (
    secondElt.isSequenceNElements() ||
    secondElt.startsWith(calculator.opIntervalOpen())
  ) {
    output = secondElt;
    return output.setChild(0, firstElt);
  }
  calculator.checkInputNotSameAsOutput(input, output);
  output.reset(calculator);
  for (int i = 1; i < input.size(); i ++) {
    output.addChildOnTop(input[i]);
  }
  return true;
}

bool CalculatorBasics::distribute(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  int additiveOperation,
  int multiplicativeOperation,
  bool constantsOnly
) {
  if (
    CalculatorBasics::leftDistributeBracketIsOnTheLeft(
      calculator,
      input,
      output,
      additiveOperation,
      multiplicativeOperation,
      constantsOnly
    )
  ) {
    return true;
  }
  return
  CalculatorBasics::rightDistributeBracketIsOnTheRight(
    calculator,
    input,
    output,
    additiveOperation,
    multiplicativeOperation,
    constantsOnly
  );
}

bool CalculatorBasics::distributeTimes(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return
  CalculatorBasics::distribute(
    calculator,
    input,
    output,
    calculator.opPlus(),
    calculator.opTimes(),
    false
  );
}

bool CalculatorBasics::distributeTimesConstant(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return
  CalculatorBasics::distribute(
    calculator,
    input,
    output,
    calculator.opPlus(),
    calculator.opTimes(),
    true
  );
}

bool CalculatorBasics::leftDistributeBracketIsOnTheLeft(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  int additiveOperation,
  int multiplicativeOperation,
  bool constantsOnly
) {
  MacroRegisterFunctionWithName(
    "CalculatorBasics::leftDistributeBracketIsOnTheLeft"
  );
  if (additiveOperation == - 1) {
    additiveOperation = calculator.opPlus();
  }
  if (multiplicativeOperation == - 1) {
    multiplicativeOperation = calculator.opTimes();
  }
  if (!input.startsWith(multiplicativeOperation, 3)) {
    return false;
  }
  if (!input[1].startsWith(additiveOperation, 3)) {
    return false;
  }
  if (constantsOnly) {
    if (!input[2].isConstantNumber()) {
      return false;
    }
  }
  Expression leftE, rightE;
  leftE.makeXOX(
    calculator, multiplicativeOperation, input[1][1], input[2]
  );
  rightE.makeXOX(
    calculator, multiplicativeOperation, input[1][2], input[2]
  );
  return output.makeXOX(calculator, additiveOperation, leftE, rightE);
}

bool CalculatorBasics::rightDistributeBracketIsOnTheRight(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  int additiveOperation,
  int multiplicativeOperation,
  bool constantsOnly
) {
  MacroRegisterFunctionWithName(
    "Calculator::rightDistributeBracketIsOnTheRight"
  );
  if (additiveOperation == - 1) {
    additiveOperation = calculator.opPlus();
  }
  if (multiplicativeOperation == - 1) {
    multiplicativeOperation = calculator.opTimes();
  }
  if (!input.startsWith(multiplicativeOperation, 3)) {
    return false;
  }
  if (!input[2].startsWith(additiveOperation, 3)) {
    return false;
  }
  if (constantsOnly) {
    if (!input[1].isConstantNumber()) {
      return false;
    }
  }
  //  int format = input.format;
  Expression leftE, rightE;
  leftE.makeXOX(
    calculator, multiplicativeOperation, input[1], input[2][1]
  );
  rightE.makeXOX(
    calculator, multiplicativeOperation, input[1], input[2][2]
  );
  return output.makeXOX(calculator, additiveOperation, leftE, rightE);
}

bool Calculator::collectCoefficientsPowersVariables(
  const Expression& input,
  const Expression& variable,
  VectorSparse<Expression>& outputPositionIiscoeffXtoIth
) {
  MacroRegisterFunctionWithName(
    "Calculator::collectCoefficientsPowersVariables"
  );
  List<Expression> summands, currentMultiplicands, remainingMultiplicands;
  Calculator& calculator = *input.owner;
  calculator.collectOpands(input, calculator.opPlus(), summands);
  Expression currentCoeff;
  outputPositionIiscoeffXtoIth.makeZero();
  for (int i = 0; i < summands.size; i ++) {
    calculator.collectOpands(
      summands[i], calculator.opTimes(), currentMultiplicands
    );
    bool found = false;
    for (int j = 0; j < currentMultiplicands.size; j ++) {
      const Expression& currentE = currentMultiplicands[j];
      remainingMultiplicands = currentMultiplicands;
      remainingMultiplicands.removeIndexShiftDown(j);
      if (remainingMultiplicands.size == 0) {
        currentCoeff.assignValue(calculator, 1);
      } else {
        currentCoeff.makeProduct(calculator, remainingMultiplicands);
      }
      if (currentE == variable) {
        outputPositionIiscoeffXtoIth.addMonomial(
          MonomialVector(1), currentCoeff
        );
        found = true;
        break;
      }
      if (currentE.startsWith(calculator.opPower(), 3)) {
        int power;
        if (currentE[1] == variable) {
          if (currentE[2].isSmallInteger(&power)) {
            outputPositionIiscoeffXtoIth.addMonomial(
              MonomialVector(power), currentCoeff
            );
            found = true;
            break;
          }
        }
      }
    }
    if (!found) {
      outputPositionIiscoeffXtoIth.addMonomial(
        MonomialVector(0), summands[i]
      );
    }
  }
  return true;
}

bool Calculator::collectOpands(
  const Expression& input, int operation, List<Expression>& outputOpands
) {
  MacroRegisterFunctionWithName("Calculator::collectOpands");
  outputOpands.setSize(0);
  return this->collectOpandsAccumulate(input, operation, outputOpands);
}

bool Calculator::collectOpandsAccumulate(
  const Expression& input, int operation, List<Expression>& outputOpands
) {
  MacroRegisterFunctionWithName("Calculator::collectOpandsAccumulate");
  if (!input.startsWith(operation)) {
    outputOpands.addOnTop(input);
    return true;
  }
  for (int i = 1; i < input.size(); i ++) {
    this->collectOpandsAccumulate(input[i], operation, outputOpands);
  }
  return true;
}

bool Calculator::functionCollectOneSummand(
  Calculator& calculator,
  const Expression& input,
  List<Expression>& outputMonomials,
  List<Rational>& outputCoefficients
) {
  MacroRegisterFunctionWithName("Calculator::functionCollectOneSummand");
  if (input.isEqualToZero()) {
    outputMonomials.addOnTop(calculator.expressionZero());
    outputCoefficients.addOnTop(1);
    return true;
  }
  Rational coefficient;
  if (input.isRational(&coefficient)) {
    outputMonomials.addOnTop(calculator.expressionOne());
    outputCoefficients.addOnTop(coefficient);
    return true;
  }
  if (!input.startsWith(calculator.opTimes(), 3)) {
    outputMonomials.addOnTop(input);
    outputCoefficients.addOnTop(1);
    return true;
  }
  if (input[1].isRational(&coefficient)) {
    outputMonomials.addOnTop(input[2]);
    outputCoefficients.addOnTop(coefficient);
    return true;
  }
  AlgebraicNumber coefficientAlgebraic;
  if (input[1].isOfType<AlgebraicNumber>(&coefficientAlgebraic)) {
    if (coefficientAlgebraic.isRational(&coefficient)) {
      outputMonomials.addOnTop(input[2]);
      outputCoefficients.addOnTop(coefficient);
      return true;
    }
  }
  outputMonomials.addOnTop(input);
  outputCoefficients.addOnTop(1);
  return true;
}

bool Calculator::functionCollectSummandsSeparatelyTrueIfOrderNonCanonical(
  Calculator& calculator,
  const Expression& input,
  List<Expression>& summands,
  List<Expression>& outputMonomials,
  List<Rational>& outputCoefficients
) {
  MacroRegisterFunctionWithName(
    "Calculator::functionCollectSummandsSeparatelyTrueIfOrderNonCanonical"
  );
  bool result =
  calculator.appendSummandsReturnTrueIfOrderNonCanonical(input, summands);
  for (int i = 0; i < summands.size; i ++) {
    Calculator::functionCollectOneSummand(
      calculator, summands[i], outputMonomials, outputCoefficients
    );
  }
  return result;
}

bool Calculator::functionCollectSummandsCombine(
  Calculator& calculator,
  const Expression& input,
  LinearCombination<Expression, Rational>& outputSum
) {
  MacroRegisterFunctionWithName("Calculator::functionCollectSummandsCombine");
  List<Expression> summands;
  calculator.appendSummandsReturnTrueIfOrderNonCanonical(input, summands);
  outputSum.makeZero();
  LinearCombination<Expression, AlgebraicNumber> sumOverAlgebraicNumbers;
  LinearCombination<Expression, double> sumOverDoubles;
  Rational coefficientRational = 1;
  AlgebraicNumber coefficientAlgebraic = 1;
  double coefficientDouble = 1;
  bool hasNAN = false;
  for (int i = 0; i < summands.size; i ++) {
    if (summands[i].isEqualToZero()) {
      continue;
    }
    if (summands[i].startsWith(calculator.opTimes(), 3)) {
      if (
        summands[i][1].isOfType<Rational>(&coefficientRational)
      ) {
        outputSum.addMonomial(summands[i][2], coefficientRational);
        continue;
      } else if (
        summands[i][1].isOfType<AlgebraicNumber>(&coefficientAlgebraic)
      ) {
        if (coefficientAlgebraic.isRational(&coefficientRational)) {
          outputSum.addMonomial(summands[i][2], coefficientRational);
          continue;
        }
        sumOverAlgebraicNumbers.addMonomial(
          summands[i][2], coefficientAlgebraic
        );
        continue;
      } else if (
        summands[i][1].isOfType<double>(&coefficientDouble)
      ) {
        sumOverDoubles.addMonomial(summands[i][2], coefficientDouble);
        if (FloatingPoint::isNaN(coefficientDouble)) {
          hasNAN = true;
        }
        continue;
      }
    }
    if (summands[i].isRational(&coefficientRational)) {
      outputSum.addMonomial(
        calculator.expressionOne(), coefficientRational
      );
    } else {
      outputSum.addMonomial(summands[i], 1);
    }
  }
  if (!sumOverDoubles.isEqualToZero() && !hasNAN) {
    sumOverDoubles.quickSortDescending();
    Expression doubleSum;
    doubleSum.makeSum(calculator, sumOverDoubles);
    outputSum.addMonomial(doubleSum, 1);
  }
  if (!sumOverAlgebraicNumbers.isEqualToZero()) {
    sumOverAlgebraicNumbers.quickSortDescending();
    Expression algebraicSum;
    algebraicSum.makeSum(calculator, sumOverAlgebraicNumbers);
    outputSum.addMonomial(algebraicSum, 1);
  }
  outputSum.quickSortDescending();
  return !hasNAN;
}

bool CalculatorBasics::associateExponentExponent(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::associateExponentExponent");
  int opPower = calculator.opPower();
  if (!input.startsWith(opPower, 3)) {
    return false;
  }
  if (!input[1].startsWith(opPower, 3)) {
    return false;
  }
  bool isGood = false;
  if (input[1][1].isKnownToBeNonNegative()) {
    isGood = true;
  }
  if (input[2].isInteger()) {
    isGood = true;
  }
  if (!isGood) {
    Rational powerInner;
    Rational powerOuter;
    if (
      input[2].isRational(&powerOuter) && input[1][2].isRational(&powerInner)
    ) {
      if ((powerInner* powerOuter).isEven()) {
        isGood = true;
      }
    }
  }
  if (!isGood) {
    return false;
  }
  Expression tempE;
  tempE.makeProduct(calculator, input[1][2], input[2]);
  output.makeXOX(calculator, opPower, input[1][1], tempE);
  return true;
}

bool Calculator::outerPowerRaiseToFirst(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::outerPowerRaiseToFirst");
  if (!input.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  if (
    input[1].startsWith(calculator.opIntegral(), 2) ||
    input[1].isOperationGiven(calculator.opIntegral())
  ) {
    return false;
  }
  if (input[2].isEqualToOne()) {
    output = input[1];
    return true;
  }
  return false;
}

bool Expression::makeXOXOdotsOX(
  Calculator& owner, int operation, const List<Expression>& opands
) {
  MacroRegisterFunctionWithName("Expression::makeXOXOdotsOX");
  if (opands.size == 0) {
    global.fatal
    << "Cannot create operation sequence from an empty list. "
    << global.fatal;
  }
  if (opands.size == 1) {
    *this = opands[0];
    return true;
  }
  this->makeXOX(
    owner,
    operation,
    opands[opands.size - 2],
    *opands.lastObject()
  );
  for (int i = opands.size - 3; i >= 0; i --) {
    this->makeXOX(owner, operation, opands[i], *this);
  }
  return true;
}

bool Expression::makeAtom(Calculator& newBoss, int input) {
  this->reset(newBoss);
  this->data = input;
  return true;
}

bool Expression::makeIdentityMatrixExpressions(
  int dimension, Calculator& inputBoss
) {
  Matrix<Expression> matrix;
  matrix.makeIdentityMatrix(
    dimension, inputBoss.expressionOne(), inputBoss.expressionZero()
  );
  return this->assignMatrixExpressions(matrix, inputBoss, false, true);
}

bool CalculatorBasics::evaluateIf(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (!input.startsWith(calculator.opDefineConditional(), 4)) {
    return
    output.assignError(
      calculator, "Error: operation :if = takes three arguments."
    );
  }
  Rational conditionRat;
  if (!input[1].isOfType<Rational>(&conditionRat)) {
    return false;
  }
  if (conditionRat.isEqualToOne()) {
    output = input[2];
    return true;
  }
  if (conditionRat.isEqualToZero()) {
    output = input[3];
    return true;
  }
  return false;
}

bool CalculatorBasics::minus(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (
    !(
      input.startsWith(calculator.opMinus(), 3) ||
      input.startsWith(calculator.opMinus(), 2)
    )
  ) {
    return false;
  }
  Expression tempE, minusOne;
  minusOne.assignValue(calculator, - 1);
  if (input.size() == 2) {
    output.makeXOX(
      calculator, calculator.opTimes(), minusOne, input[1]
    );
  } else {
    tempE.makeXOX(
      calculator, calculator.opTimes(), minusOne, input[2]
    );
    output.makeXOX(calculator, calculator.opPlus(), input[1], tempE);
  }
  return true;
}

void Expression::operator+=(const Expression& other) {
  MacroRegisterFunctionWithName("Expression::operator+=");
  if (this->owner == nullptr && other.owner == nullptr) {
    this->data += other.data;
    if (this->data != 1 && this->data != 0) {
      global.fatal
      << "Attempting to add non-initialized expressions"
      << global.fatal;
    }
    return;
  }
  if (other.owner == nullptr) {
    Expression otherCopy;
    otherCopy.assignValue(*this->owner, other.data);
    *this += otherCopy;
    return;
  }
  if (this->owner == nullptr) {
    this->assignValue(*other.owner, this->data);
  }
  if (this->owner != other.owner) {
    global.fatal
    << "Error: adding expressions with different owners. "
    << global.fatal;
  }
  Expression resultE;
  resultE.makeXOX(
    *this->owner, this->owner->opPlus(), *this, other
  );
  *this = resultE;
}

void Expression::operator-=(const Expression& other) {
  MacroRegisterFunctionWithName("Expression::operator+=");
  if (this->owner == nullptr && other.owner == nullptr) {
    this->data -= other.data;
    if (this->data != 1 && this->data != 0) {
      global.fatal
      << "Attempt to subtract non-initialized expressions. "
      << global.fatal;
    }
    return;
  }
  if (other.owner == nullptr) {
    Expression otherCopy;
    otherCopy.assignValue(*this->owner, other.data);
    (*this) -= otherCopy;
    return;
  }
  if (this->owner == nullptr) {
    this->assignValue(*other.owner, this->data);
  }
  if (this->owner != other.owner) {
    global.fatal
    << "Error: attempt to add expressions with different owners. "
    << global.fatal;
  }
  Expression resultE;
  resultE.makeXOX(
    *this->owner, this->owner->opMinus(), *this, other
  );
  *this = resultE;
}

Expression Expression::operator*(const Expression& other) {
  MacroRegisterFunctionWithName("Expression::operator*");
  Expression result;
  result = *this;
  result *= other;
  return result;
}

Expression Expression::operator*(int other) {
  MacroRegisterFunctionWithName("Expression::operator*");
  Expression result;
  if (this->owner == nullptr) {
    // perhaps we should allow the code below for convenience: really
    // hard to judge if the convenience is worth it, or whether it will cause
    // hard-to-detect bugs.
    // Rational resultRat = this->data;
    // resultRat*= other;
    // if (resultRat.isSmallInteger(&result.data))
    //  return result;
    global.fatal
    << "Multiplying non-initialized expression with data: "
    << this->data
    << " by integer "
    << other
    << " is not allowed. "
    << global.fatal;
  }
  Expression otherE;
  otherE.assignValue(*this->owner, other);
  result = *this;
  result *= otherE;
  return result;
}

Expression Expression::operator/(int other) {
  MacroRegisterFunctionWithName("Expression::operator/");
  Expression result;
  if (this->owner == nullptr) {
    global.fatal
    << "Multiplying non-initialized expression with data: "
    << this->data
    << " by integer "
    << other
    << " is not allowed. "
    << global.fatal;
  }
  Expression otherE;
  otherE.assignValue(*this->owner, other);
  result = *this;
  result /= otherE;
  return result;
}

Expression Expression::operator+(const Expression& other) {
  Expression result;
  result = *this;
  result += other;
  return result;
}

Expression Expression::operator-(const Expression& other) {
  Expression result;
  result = *this;
  result -= other;
  return result;
}

Expression Expression::operator/(const Expression& other) {
  MacroRegisterFunctionWithName("Expression::operator/");
  Expression result;
  result = *this;
  result /= other;
  return result;
}

void Expression::operator/=(const Expression& other) {
  MacroRegisterFunctionWithName("Expression::operator/=");
  if (this->owner == nullptr && other.owner == nullptr) {
    this->data /= other.data;
    if (this->data != 1 && this->data != 0) {
      global.fatal
      << "Attempting to divide non-initialized expressions. "
      << global.fatal;
    }
    return;
  }
  if (other.owner == nullptr) {
    Expression otherCopy;
    otherCopy.assignValue(*this->owner, other.data);
    (*this) /= otherCopy;
    return;
  }
  if (this->owner == nullptr) {
    this->assignValue(*other.owner, this->data);
  }
  if (this->owner != other.owner) {
    global.fatal
    << "Error: dividing expressions with different owners. "
    << global.fatal;
  }
  Expression resultE;
  resultE.makeXOX(
    *this->owner, this->owner->opDivide(), *this, other
  );
  *this = resultE;
}

void Expression::operator*=(const Expression& other) {
  MacroRegisterFunctionWithName("Expression::operator*=");
  if (this->owner == nullptr && other.owner == nullptr) {
    this->data *= other.data;
    if (this->data != 1 && this->data != 0) {
      global.fatal
      << "Attempting to add non-initialized expressions. "
      << global.fatal;
    }
    return;
  }
  if (other.owner == nullptr) {
    Expression otherCopy;
    otherCopy.assignValue(*this->owner, other.data);
    (*this) *= otherCopy;
    return;
  }
  if (this->owner == nullptr) {
    this->assignValue(*other.owner, this->data);
  }
  if (this->owner != other.owner) {
    global.fatal
    << "Error: adding expressions with different owners. "
    << global.fatal;
  }
  Expression resultE;
  resultE.makeXOX(
    *this->owner, this->owner->opTimes(), *this, other
  );
  *this = resultE;
}

bool Expression::operator==(const Expression& other) const {
  if (this->owner != other.owner) {
    return false;
  }
  return this->data == other.data && this->children == other.children;
}

bool Expression::isEqualToMathematically(const Expression& other) const {
  MacroRegisterFunctionWithName("Expression::isEqualToMathematically");
  if (this->owner == nullptr && other.owner == nullptr) {
    return this->data == other.data;
  }
  if (this->owner == nullptr) {
    return false;
  }
  if (*this == other) {
    return true;
  }
  Rational rational, rationalTwo;
  AlgebraicNumber algebraicNumber;
  if (this->isOfType(&rational) && other.isOfType(&rationalTwo)) {
    return rational == rationalTwo;
  }
  if (this->isOfType(&rational) && other.isOfType(&algebraicNumber)) {
    return algebraicNumber == rational;
  }
  if (other.isOfType(&rational) && this->isOfType(&algebraicNumber)) {
    return algebraicNumber == rational;
  }
  double leftD = - 1, rightD = - 1;
  if (
    this->evaluatesToDouble(&leftD) && other.evaluatesToDouble(&rightD)
  ) {
    return (leftD - rightD == 0.0);
  }
  Expression differenceE = *this;
  differenceE -= other;
  Expression differenceEsimplified;
  if (
    !this->owner->evaluateExpression(
      *this->owner, differenceE, differenceEsimplified
    )
  ) {
    return false;
  }
  if (differenceEsimplified.isEqualToZero()) {
    return true;
  }
  if (differenceEsimplified.isSequenceNElements()) {
    for (int i = 1; i < differenceEsimplified.size(); i ++) {
      if (!differenceEsimplified[i].isEqualToZero()) {
        return false;
      }
    }
    return true;
  }
  if (this->size() != other.size()) {
    return false;
  }
  if (this->size() == 0) {
    return this->data == other.data;
  }
  for (int i = 0; i < this->size(); i ++) {
    if (!(*this)[i].isEqualToMathematically(other[i])) {
      return false;
    }
  }
  return true;
}

SemisimpleLieAlgebra* Expression::
getAmbientSemisimpleLieAlgebraNonConstUseWithCaution() const {
  MacroRegisterFunctionWithName(
    "Expression::getAmbientSemisimpleLieAlgebraNonConstUseWithCaution"
  );
  this->checkInitialization();
  if (!this->isBuiltInType()) {
    return nullptr;
  }
  ExpressionContext myContext = this->getContext();
  int indexSemisimpleLieAlgebra = myContext.indexAmbientSemisimpleLieAlgebra;
  if (indexSemisimpleLieAlgebra == - 1) {
    return nullptr;
  }
  return
  &this->owner->objectContainer.semisimpleLieAlgebras.values[
    indexSemisimpleLieAlgebra
  ];
}

Function& Calculator::getFunctionHandlerFromNamedRule(
  const std::string& inputNamedRule
) {
  const Calculator::NamedRuleLocation& current =
  this->namedRules.getValueNoFail(inputNamedRule);
  const MemorySaving<Calculator::OperationHandlers>& currentOperation =
  this->operations.getValueNoFail(current.containerOperation);
  if (currentOperation.isZeroPointer()) {
    global.fatal
    << "Named rule "
    << inputNamedRule
    << " registered with operation "
    << current.containerOperation
    << " but the operation has no handlers. "
    << global.fatal;
  }
  if (current.isComposite) {
    return
    currentOperation.getElementConst().compositeHandlers[current.index];
  }
  return currentOperation.getElementConst().handlers[current.index];
}

int Calculator::addOperationNoRepetitionOrReturnIndexFirst(
  const std::string& operationName
) {
  int result = this->operations.getIndex(operationName);
  if (result == - 1) {
    result = this->operations.size();
    this->operations.getValueCreateEmpty(operationName);
  }
  return result;
}

void Calculator::addOperationBuiltInType(
  const std::string& operationBuiltIn
) {
  this->addOperationNoRepetitionAllowed(operationBuiltIn);
  this->builtInTypes.addOnTop(operationBuiltIn);
}

void Calculator::addOperationNoRepetitionAllowed(
  const std::string& operation
) {
  if (this->operations.contains(operation)) {
    global.fatal
    << "Operation "
    << operation
    << " already created. "
    << global.fatal;
  }
  this->operations.getValueCreateEmpty(operation);
}

Function::Function() {
  this->owner = nullptr;
  this->resetExceptOwner();
}

Function::Function(
  Calculator& inputOwner,
  int inputIndexOperation,
  const Expression::FunctionAddress& functionPointer,
  Expression* inputArgumentTypes,
  const std::string& description,
  const std::string& inputExample,
  const std::string& inputAdditionalIndentifier,
  const std::string& inputCalculatorIdentifier,
  const Options& inputOptions,
  const List<int>& inputParentsThatBanHandler
) {
  this->owner = nullptr;
  if (&this->options == &inputOptions) {
    global.fatal
    << "Input options and target "
    << "options have the same address. "
    << global.fatal;
  }
  this->reset(inputOwner);
  this->indexOperation = inputIndexOperation;
  this->options = inputOptions;
  this->functionAddress = functionPointer;
  this->description = description;
  this->example = inputExample;
  this->additionalIdentifier = inputAdditionalIndentifier;
  this->calculatorIdentifier = inputCalculatorIdentifier;
  if (inputArgumentTypes != nullptr) {
    this->argumentTypes = *inputArgumentTypes;
  }
  this->parentsThatBanHandler = inputParentsThatBanHandler;
}

void Calculator::addOperationBinaryInnerHandlerWithTypes(
  const std::string& operation,
  Expression::FunctionAddress innerHandler,
  int leftType,
  int rightType,
  const std::string& opDescription,
  const std::string& opExample,
  const std::string& inputAdditionalIdentifier,
  const std::string& inputCalculatorIdentifier,
  const Function::Options& options
) {
  int indexOperation = this->operations.getIndex(operation);
  if (indexOperation == - 1) {
    indexOperation = this->operations.size();
    this->operations.getValueCreateEmpty(operation);
  }
  List<int> empty;
  Function innerFunction(
    *this,
    indexOperation,
    innerHandler,
    nullptr,
    opDescription,
    opExample,
    inputAdditionalIdentifier,
    inputCalculatorIdentifier,
    options,
    empty
  );
  innerFunction.argumentTypes.reset(*this, 2);
  innerFunction.argumentTypes.addChildAtomOnTop(leftType);
  innerFunction.argumentTypes.addChildAtomOnTop(rightType);
  this->registerCalculatorFunction(innerFunction, indexOperation);
}

void Calculator::registerCalculatorFunction(
  Function& inputFunction, int indexOperation
) {
  MacroRegisterFunctionWithName("Calculator::registerCalculatorFunction");
  if (indexOperation < 0 || indexOperation >= this->operations.size()) {
    global.fatal
    << "Invalid index operation: "
    << indexOperation
    << ", there are: "
    << this->operations.size()
    << " operations total."
    << global.fatal;
  }
  MemorySaving<Calculator::OperationHandlers>& handlerPointer =
  this->operations.values[indexOperation];
  Calculator::OperationHandlers& handler = handlerPointer.getElement();
  handler.checkConsistency();
  if (inputFunction.options.flagIsCompositeHandler) {
    inputFunction.indexInOperationHandlers = handler.compositeHandlers.size;
    handler.compositeHandlers.addOnTop(inputFunction);
    if (handler.compositeHandlers.size <= 0) {
      global.fatal << "Composite handlers cannot be empty. " << global.fatal;
    }
  } else {
    inputFunction.indexInOperationHandlers = handler.handlers.size;
    handler.handlers.addOnTop(inputFunction);
    if (handler.handlers.size <= 0) {
      global.fatal << "Handlers cannot be empty. " << global.fatal;
    }
  }
  if (inputFunction.calculatorIdentifier == "") {
    return;
  }
  Calculator::NamedRuleLocation namedRule;
  namedRule.containerOperation = this->operations.keys[indexOperation];
  namedRule.index = inputFunction.indexInOperationHandlers;
  namedRule.isComposite = inputFunction.options.flagIsCompositeHandler;
  this->namedRules.setKeyValue(
    inputFunction.calculatorIdentifier, namedRule
  );
  if (inputFunction.options.freezesArguments) {
    this->atomsThatFreezeArguments.addOnTopNoRepetitionMustBeNew(
      namedRule.containerOperation
    );
  }
}

void Calculator::addOperationHandler(
  const std::string& operation,
  Expression::FunctionAddress handler,
  const std::string& opArgumentListIgnoredForTheTimeBeing,
  const std::string& opDescription,
  const std::string& opExample,
  const std::string& inputAdditionalIdentifier,
  const std::string& inputCalculatorIdentifier,
  const Function::Options& options,
  const List<std::string>* parentsThatBanHandler
) {
  if (opArgumentListIgnoredForTheTimeBeing != "") {
    global.fatal
    <<
    "This section of code is not implemented yet. Crashing to let you know. "
    << global.fatal;
  }
  int indexOperation = this->operations.getIndex(operation);
  if (indexOperation == - 1) {
    indexOperation = this->operations.size();
    this->operations.getValueCreateEmpty(operation);
  }
  List<int> parentOperationsThatBanHandler;
  if (parentsThatBanHandler != nullptr) {
    for (int i = 0; i < parentsThatBanHandler->size; i ++) {
      int atom =
      this->operations.getIndexNoFail((*parentsThatBanHandler)[i]);
      parentOperationsThatBanHandler.addOnTop(atom);
    }
  }
  Function functionWrapper(
    *this,
    indexOperation,
    handler,
    nullptr,
    opDescription,
    opExample,
    inputAdditionalIdentifier,
    inputCalculatorIdentifier,
    options,
    parentOperationsThatBanHandler
  );
  if (
    functionWrapper.functionAddress == nullptr ||
    functionWrapper.owner == nullptr
  ) {
    global.fatal << "Function not initialized properly. " << global.fatal;
  }
  this->registerCalculatorFunction(functionWrapper, indexOperation);
}

Function::Options Function::Options::adminNoTestInvisibleOffByDefault() {
  Function::Options result;
  result.flagIsInner = true;
  result.dontTestAutomatically = true;
  result.adminOnly = true;
  result.visible = false;
  result.disabledByUser = true;
  result.disabledByUserDefault = true;
  return result;
}

Function::Options Function::Options::compositeStandard() {
  Function::Options result;
  result.flagIsCompositeHandler = true;
  result.flagIsInner = true;
  return result;
}

Function::Options Function::Options::innerFreezesArguments() {
  Function::Options result;
  result.flagIsInner = true;
  result.freezesArguments = true;
  return result;
}

Function::Options Function::Options::innerAdminNoTest() {
  Function::Options result;
  result.flagIsInner = true;
  result.dontTestAutomatically = true;
  result.adminOnly = true;
  return result;
}

Function::Options Function::Options::administrativeExperimentalTested() {
  Function::Options result;
  result.flagIsExperimental = true;
  result.adminOnly = true;
  result.dontTestAutomatically = false;
  return result;
}

Function::Options Function::Options::administrativeTested() {
  Function::Options result;
  result.adminOnly = true;
  result.dontTestAutomatically = false;
  return result;
}

Function::Options Function::Options::innerAdminNoTestExperimental() {
  Function::Options result = Function::Options::innerAdminNoTest();
  result.flagIsExperimental = true;
  return result;
}

Function::Options Function::Options::innerInvisibleExperimental() {
  Function::Options result;
  result.flagIsInner = true;
  result.flagIsExperimental = true;
  result.visible = false;
  return result;
}

Function::Options Function::Options::experimental() {
  Function::Options result;
  result.flagIsInner = true;
  result.flagIsExperimental = true;
  return result;
}

Function::Options Function::Options::standard() {
  Function::Options result;
  result.flagIsInner = true;
  return result;
}

Function::Options Function::Options::innerInvisible() {
  Function::Options result;
  result.flagIsInner = true;
  result.visible = false;
  return result;
}

Function::Options Function::Options::innerNoTest() {
  Function::Options result;
  result.flagIsInner = true;
  result.dontTestAutomatically = true;
  return result;
}

Function::Options Function::Options::innerNoTestExperimental() {
  Function::Options result;
  result.flagIsInner = true;
  result.dontTestAutomatically = true;
  result.flagIsExperimental = true;
  return result;
}

Function::Options Function::Options::invisibleExperimentalNoTest() {
  Function::Options result;
  result.flagIsInner = true;
  result.flagIsExperimental = true;
  result.dontTestAutomatically = true;
  result.visible = false;
  return result;
}

Function::Options Function::Options::outerOffByDefault() {
  Function::Options result;
  result.flagIsInner = false;
  result.disabledByUserDefault = true;
  result.disabledByUser = true;
  return result;
}

Function::Options Function::Options::invisibleNoTest() {
  Function::Options result;
  result.flagIsInner = true;
  result.visible = false;
  result.dontTestAutomatically = true;
  return result;
}

Function::Options Function::Options::adminNoTest() {
  Function::Options result;
  result.flagIsInner = true;
  result.dontTestAutomatically = true;
  result.adminOnly = true;
  return result;
}

Function::Options Function::Options::adminNoTestInvisibleExperimental() {
  Function::Options result;
  result.flagIsInner = true;
  result.dontTestAutomatically = true;
  result.adminOnly = true;
  result.visible = false;
  return result;
}

void Function::Options::reset() {
  this->flagIsCompositeHandler = false;
  this->flagIsInner = true;
  this->flagMayActOnBoundVars = false;
  this->visible = true;
  this->flagIsExperimental = false;
  this->disabledByUser = false;
  this->disabledByUserDefault = false;
  this->dontTestAutomatically = false;
  this->adminOnly = false;
  this->freezesArguments = false;
}

Function::Options::Options() {
  this->reset();
}

bool Function::inputFitsMyInnerType(const Expression& input) {
  if (!this->options.flagIsInner) {
    return false;
  }
  if (this->argumentTypes.size() != 2) {
    return true;
  }
  if (input.size() != 3) {
    return false;
  }
  bool argument1Good = this->argumentTypes[0].data ==
  - 1 ?
  true :
  input[1].isListStartingWithAtom(this->argumentTypes[0].data);
  bool argument2Good = this->argumentTypes[1].data ==
  - 1 ?
  true :
  input[2].isListStartingWithAtom(this->argumentTypes[1].data);
  return argument1Good && argument2Good;
}

std::string Function::toStringShort() const {
  if (this->owner == nullptr) {
    return "(non-initialized)";
  }
  std::stringstream out;
  out << this->owner->operations.keys[this->indexOperation];
  MemorySaving<Calculator::OperationHandlers>& handlerPointer =
  this->owner->operations.values[this->indexOperation];
  Calculator::OperationHandlers& handler = handlerPointer.getElement();
  if (this->options.flagIsCompositeHandler) {
    out << " (composite) (" << this->indexInOperationHandlers + 1
    << " out of "
    << handler.compositeHandlers.size
    << ") ";
  } else {
    out << "(" << this->indexInOperationHandlers + 1
    << " out of "
    << handler.handlers.size
    << "). ";
  }
  return out.str();
}

std::string Function::toStringSummary() const {
  if (this->owner == nullptr) {
    return "(non-initialized)";
  }
  std::stringstream out;
  out << this->toStringShort();
  if (this->calculatorIdentifier != "") {
    out
    << "Rule name: <span style ='color:blue'>"
    << this->calculatorIdentifier
    << "</span>. ";
  }
  if (this->additionalIdentifier != "") {
    out << "Handler: " << this->additionalIdentifier << ". ";
  }
  return out.str();
}

bool Function::shouldBeApplied(int parentOperationIfAvailable) {
  if (this->options.disabledByUser) {
    return false;
  }
  if (this->parentsThatBanHandler.contains(parentOperationIfAvailable)) {
    return false;
  }
  if (this->options.adminOnly) {
    if (!global.userDefaultHasAdminRights()) {
      return (*this->owner)
      << "Rule "
      << this->calculatorIdentifier
      << " requires logged-in administrator account. ";
    }
  }
  return true;
}

JSData Function::toJSON() const {
  MacroRegisterFunctionWithName("Function::toJSON");
  JSData result;
  result.elementType = JSData::token::tokenObject;
  if (this->owner == nullptr) {
    result[WebAPI::result::error] = "bad_owner";
    return result;
  }
  if (this->options.visible) {
    result["visible"] = true;
  } else {
    result["visible"] = false;
  }
  Calculator::OperationHandlers& operationHandlers =
  this->owner->operations.values[this->indexOperation].getElement();
  result["number"] = this->indexInOperationHandlers + 1;
  if (this->options.flagIsCompositeHandler) {
    result["composite"] = true;
    result["total"] = operationHandlers.compositeHandlers.size;
    result["atom"] = this->owner->operations.keys[this->indexOperation];
  } else {
    result["composite"] = false;
    result["total"] = operationHandlers.handlers.size;
    result["atom"] = this->owner->operations.keys[this->indexOperation];
  }
  if (this->options.flagIsExperimental) {
    result["experimental"] = true;
  } else {
    result["experimental"] = false;
  }
  if (this->options.adminOnly) {
    result["administrative"] = true;
  } else {
    result["administrative"] = false;
  }
  result["description"] = this->description;
  if (this->calculatorIdentifier != "") {
    result["ruleName"] = this->calculatorIdentifier;
  }
  if (this->additionalIdentifier != "") {
    result["additionalIdentifier"] = this->additionalIdentifier;
  }
  std::stringstream functionAddress;
  functionAddress << std::hex << reinterpret_cast<unsigned long>(
    this->functionAddress
  );
  result["memoryAddress"] = functionAddress.str();
  if (this->options.flagIsInner) {
    result["inner"] = true;
  } else {
    result["inner"] = false;
  }
  if (this->example != "") {
    result["example"] = this->example;
  }
  return result;
}

std::string Function::toStringFull() const {
  if (!this->options.visible) {
    return "";
  }
  if (this->owner == nullptr) {
    return "(non-intialized)";
  }
  std::stringstream out2;
  out2 << this->toStringShort();
  if (!this->options.flagIsExperimental) {
    std::stringstream out;
    out << this->description;
    if (this->calculatorIdentifier != "") {
      out << "Rule name: " << this->calculatorIdentifier << ". ";
    }
    if (this->additionalIdentifier != "") {
      out << "Handler: " << this->additionalIdentifier << ". ";
    }
    // use of unsigned long is correct on i386 and amd64
    // uintptr_t is only available in c++ 0x
    // Please fix if the following code is not portable:
    out
    << "Function memory address: "
    << std::hex
    << reinterpret_cast<unsigned long>(this->functionAddress)
    << ". ";
    if (!this->options.flagIsInner) {
      out
      <<
      "This is a <b>``law''</b> - substitution takes place only if output expression is different from input. "
      ;
    }
    if (this->example != "") {
      out << " <br> " << this->example << "&nbsp&nbsp&nbsp";
    }
    if (this->example != "") {
      out2
      << "<a href=\""
      << global.displayNameExecutable
      << "?request=calculator&showExamples = true&mainInput="
      << HtmlRoutines::convertStringToURLString(this->example, false)
      << "\"> "
      << " Example"
      << "</a>";
    }
  } else {
    out2 << "<b>Experimental, please don't use.</b>";
  }
  return out2.str();
}

std::string ObjectContainer::toString() {
  MacroRegisterFunctionWithName("ObjectContainer::toString");
  std::stringstream out;
  if (this->semisimpleLieAlgebras.values.size > 0) {
    out
    << "Lie algebras created ("
    << this->semisimpleLieAlgebras.values.size
    << " total): ";
    for (int i = 0; i < this->semisimpleLieAlgebras.values.size; i ++) {
      out << this->semisimpleLieAlgebras.values[i].toStringLieAlgebraName();
      if (i != this->semisimpleLieAlgebras.values.size - 1) {
        out << ", ";
      }
    }
  }
  if (this->semisimpleSubalgebras.values.size > 0) {
    out
    << "<br>Lie semisimple subalgebras computation data structures ("
    << this->semisimpleSubalgebras.values.size
    << " total): ";
    for (int i = 0; i < this->semisimpleSubalgebras.values.size; i ++) {
      out
      << " Type "
      << this->semisimpleSubalgebras.values[i].owner->toStringLieAlgebraName()
      << " with "
      << this->semisimpleSubalgebras.values[i].subalgebras.values.size
      << " candidates";
      if (i != this->semisimpleSubalgebras.values.size - 1) {
        out << ", ";
      }
    }
  }
  return out.str();
}

JSData Calculator::toJSONOutputAndSpecials() {
  MacroRegisterFunctionWithName("Calculator::toJSONOutputAndSpecials");
  JSData result = this->outputJS;
  if (this->inputString == "") {
    return result;
  }
  result[WebAPI::result::performance] = this->toJSONPerformance();
  result[WebAPI::result::parsingLog] = this->parser.parsingLog;
  return result;
}

std::string Calculator::toStringOutputAndSpecials() {
  MacroRegisterFunctionWithName("Calculator::toStringOutputAndSpecials");
  return this->toJSONOutputAndSpecials().toString(nullptr);
}

void Calculator::writeAutoCompleteKeyWordsToFile() {
  MacroRegisterFunctionWithName("Calculator::writeAutoCompleteKeyWordsToFile");
  std::stringstream out;
  out
  <<
  "\"use strict\"; //This file is automatically generated, please do not modify.\n"
  ;
  out << "  var autoCompleteDictionary = [\n  ";
  for (int i = 0; i < this->autoCompleteKeyWords.size; i ++) {
    if (this->autoCompleteKeyWords[i].size() > 2) {
      out
      << "\""
      << HtmlRoutines::convertStringEscapeNewLinesQuotesBackslashes(
        this->autoCompleteKeyWords[i]
      )
      << "\"";
      if (i != this->autoCompleteKeyWords.size - 1) {
        out << ", ";
      }
    }
  }
  out << "];\n";
  out << "module.exports = {autoCompleteDictionary};";
  std::fstream fileStream;
  FileOperations::openFileCreateIfNotPresentVirtual(
    fileStream,
    "/calculator_html/autocomplete_keywords.js",
    false,
    true,
    false,
    false
  );
  fileStream << out.str();
}

void Calculator::computeAutoCompleteKeyWords() {
  MacroRegisterFunctionWithName("Calculator::computeAutoCompleteKeyWords");
  this->autoCompleteKeyWords.setExpectedSize(this->operations.size() * 2);
  for (int i = 0; i < this->operations.size(); i ++) {
    this->autoCompleteKeyWords.addOnTopNoRepetition(
      this->operations.keys[i]
    );
  }
  for (int i = 0; i < this->namedRules.size(); i ++) {
    this->autoCompleteKeyWords.addOnTopNoRepetition(
      this->namedRules.keys[i]
    );
  }
  for (int i = 0; i < this->parser.controlSequences.size; i ++) {
    if (this->parser.controlSequences[i].size() > 0) {
      if (
        MathRoutines::isLatinLetter(
          this->parser.controlSequences[i][0]
        )
      ) {
        autoCompleteKeyWords.addOnTopNoRepetition(
          this->parser.controlSequences[i]
        );
      }
    }
  }
}

JSData Calculator::toJSONPerformance() {
  MacroRegisterFunctionWithName("Calculator::toStringPerformance");
  int64_t elapsedMilliseconds = global.getElapsedMilliseconds();
  int64_t computationMilliseconds =
  elapsedMilliseconds - this->statistics.startTimeEvaluationMilliseconds;
  int64_t requestMilliseconds =
  elapsedMilliseconds - global.millisecondsComputationStart;
  int64_t waitingMilliseconds = elapsedMilliseconds - requestMilliseconds;
  JSData result;
  std::stringstream moreDetails;
  moreDetails
  << "<br>Max computation time soft: "
  << (static_cast<double>(global.millisecondsMaxComputation) / 2000)
  << " s [calculator error when exceeded]. ";
  moreDetails
  << "<br>Max computation time hard: "
  << (static_cast<double>(global.millisecondsMaxComputation) / 1000)
  << " s [worker crash when exceeded]. ";
  moreDetails
  << "<br>Total process request time: "
  << requestMilliseconds
  << " ms (~"
  << (static_cast<double>(requestMilliseconds) / 1000)
  << " s). ";
  moreDetails
  << "<br>Time waiting on open connection: "
  << waitingMilliseconds
  << " ms (~"
  << (static_cast<double>(waitingMilliseconds) / 1000)
  << " s).";
  moreDetails
  << "<br>Runs of evaluation loop: "
  << this->statistics.totalEvaluationLoops
  << ". ";
  moreDetails
  << "<br>Expressions generated: "
  << this->allChildExpressions.size
  << ". ";
  moreDetails
  << "<br>Cached expression at last rule stack: "
  << this->cachedExpressions.size()
  << ". ";
  moreDetails
  << "<br>Expressions evaluated: "
  << this->statistics.expressionsEvaluated
  << ". ";
  moreDetails
  << "<br>Total number of pattern matches performed: "
  << this->statistics.totalPatternMatchesPerformed
  << "";
  if (this->depthRecursionReached > 0) {
    moreDetails
    << "<br>maximum recursion depth reached: "
    << this->depthRecursionReached
    << ".";
  }
  moreDetails
  << "<br>Lists created: "
  << "computation: "
  << (
    GlobalStatistics::numberOfListsCreated - static_cast<unsigned>(
      this->statistics.numberOfListsStart
    )
  )
  << ", total: "
  << GlobalStatistics::numberOfListsCreated;
  moreDetails
  << "<br> # List resizes: computation: "
  << (
    GlobalStatistics::numberOfListResizesTotal - static_cast<unsigned>(
      this->statistics.numberListResizesStart
    )
  )
  << ", total: "
  << GlobalStatistics::numberOfListResizesTotal
  << "<br> # hash resizing: computation: "
  << (
    GlobalStatistics::numberOfHashResizes - static_cast<unsigned>(
      this->statistics.numberHashResizesStart
    )
  )
  << ", total: "
  << GlobalStatistics::numberOfHashResizes;
  if (Rational::totalSmallAdditions > 0) {
    moreDetails
    << "<br>Small rational additions: computation: "
    << Rational::totalSmallAdditions - static_cast<unsigned long long>(
      this->statistics.numberOfSmallAdditionsStart
    )
    << ", total: "
    << Rational::totalSmallAdditions;
  }
  if (Rational::totalSmallMultiplications > 0) {
    moreDetails
    << "<br>Small rational multiplications: computation: "
    << Rational::totalSmallMultiplications - static_cast<unsigned long long>(
      this->statistics.numberOfSmallMultiplicationsStart
    )
    << ", total: "
    << Rational::totalSmallMultiplications;
  }
  if (Rational::totalSmallGreatestCommonDivisors > 0) {
    moreDetails
    << "<br>Small gcd calls: computation: "
    << Rational::totalSmallGreatestCommonDivisors -
    static_cast<unsigned long long>(
      this->statistics.numberOfSmallGreatestCommonDivisorsStart
    )
    << ", total: "
    << Rational::totalSmallGreatestCommonDivisors;
  }
  if (Rational::totalLargeAdditions > 0) {
    moreDetails
    << "<br>Large integer additions: "
    << Rational::totalLargeAdditions - static_cast<unsigned long long>(
      this->statistics.numberOfLargeAdditionsStart
    )
    << ", total: "
    << Rational::totalLargeAdditions;
  }
  if (Rational::totalLargeMultiplications > 0) {
    moreDetails
    << "<br>Large integer multiplications: computation: "
    << Rational::totalLargeMultiplications - static_cast<unsigned long long>(
      this->statistics.numberOfLargeMultiplicationsStart
    )
    << ", total: "
    << Rational::totalLargeMultiplications;
  }
  if (Rational::totalLargeGreatestCommonDivisors > 0) {
    moreDetails
    << "<br>Large gcd calls: "
    << Rational::totalLargeGreatestCommonDivisors -
    static_cast<unsigned long long>(
      this->statistics.numberOfLargeGreatestCommonDivisorsStart
    )
    << ", total: "
    << Rational::totalLargeGreatestCommonDivisors;
  }
  std::stringstream millisecondsStream;
  millisecondsStream << computationMilliseconds << " ms";
  result[WebAPI::result::computationTime] = millisecondsStream.str();
  result[WebAPI::result::comments] = moreDetails.str();
  return result;
}

std::string Calculator::toString() {
  MacroRegisterFunctionWithName("Calculator::toString");
  std::stringstream out2;
  std::string openTag1 = "<span style='color:blue'>";
  std::string closeTag1 = "</span>";
  if (global.millisecondsMaxComputation > 0) {
    out2
    << "Computation time limit: "
    << global.millisecondsMaxComputation
    << " ms.<hr>";
  } else {
    out2 << "No computation time limit.<hr> ";
  }
  if (this->ruleStack.size() > 1) {
    out2 << "<b>Predefined rules.</b><br>";
    for (int i = 1; i < this->ruleStack.size(); i ++) {
      out2 << this->ruleStack[i].toString();
      if (i != this->ruleStack.size() - 1) {
        out2 << "<br>";
      }
    }
  }
  if (!global.userDebugFlagOn()) {
    return out2.str();
  }
  std::stringstream out;
  out2 << "<hr><b>Further calculator details.</b>";
  out
  << "<br><b>Object container information</b>. "
  << "The object container is the data "
  << "structure storing all c++ built-in data types "
  << "requested by the user<br> "
  << this->objectContainer.toString();
  out
  << "<hr>Control sequences ("
  << this->parser.controlSequences.size
  << " total):\n<br>\n";
  for (int i = 0; i < this->parser.controlSequences.size; i ++) {
    out << openTag1 << this->parser.controlSequences[i] << closeTag1;
    if (i != this->parser.controlSequences.size) {
      out << ", ";
    }
  }
  out
  << "<br>\n User or run-time defined atoms = "
  << this->operations.size()
  << " (= "
  << this->numberOfPredefinedAtoms
  << " predefined + "
  << this->operations.size() - this->numberOfPredefinedAtoms
  << " user-defined):<br>\n";
  for (int i = 0; i < this->operations.size(); i ++) {
    out
    << "\n"
    << i
    << ": "
    << openTag1
    << this->operations.keys[i]
    << closeTag1;
    if (i != this->operations.size() - 1) {
      out << ", ";
    }
  }
  out << "<hr>";
  out
  << "Children expressions ("
  << this->allChildExpressions.size
  << " total): <br>";
  int numExpressionsToDisplay = this->allChildExpressions.size;
  if (this->allChildExpressions.size > 1000) {
    numExpressionsToDisplay = 1000;
    out
    << " <b>Displaying first "
    << numExpressionsToDisplay
    << " only </b><br>";
  }
  for (int i = 0; i < numExpressionsToDisplay; i ++) {
    out << this->allChildExpressions[i].toString() << ", ";
  }
  out << "<hr>";
  out
  << "\n Cached expressions ("
  << this->cachedExpressions.size()
  << " total):\n<br>\n";
  numExpressionsToDisplay = this->cachedExpressions.size();
  if (numExpressionsToDisplay > 1000) {
    numExpressionsToDisplay = 1000;
    out
    << "<b>Displaying first "
    << numExpressionsToDisplay
    << " expressions only. </b><br>";
  }
  for (int i = 0; i < numExpressionsToDisplay; i ++) {
    out
    << this->cachedExpressions.keys[i].toString()
    << " -> "
    << this->cachedExpressions.values[i].reducesTo.toString();
    if (i != this->cachedExpressions.size() - 1) {
      out << "<br>";
    }
  }
  out2 << out.str();
  return out2.str();
}

std::string CalculatorParser::toStringSyntacticStackHTMLTable(
  bool includeLispifiedExpressions, bool ignoreCommandEnclosures
) {
  MacroRegisterFunctionWithName("Calculator::toStringSyntacticStackHTMLTable");
  std::stringstream out;
  if ((*this->currentSyntacticStack).size < this->numberOfEmptyTokensStart
  ) {
    return "Not enough empty tokens in the start of the syntactic stack.";
  }
  bool isBad = ((*this->currentSyntacticStack).size >
    this->numberOfEmptyTokensStart + 1
  );
  SyntacticElement& lastSyntacticElt =
  *(*this->currentSyntacticStack).lastObject();
  if ((*this->currentSyntacticStack).size == this->numberOfEmptyTokensStart + 1
  ) {
    if (lastSyntacticElt.controlIndex != this->conExpression()) {
      isBad = true;
    }
  }
  if (!isBad) {
    out
    << this->currentSyntacticStack->lastObject()->toStringHumanReadable(
      *this, includeLispifiedExpressions
    );
    return out.str();
  }
  out << "<table style='vertical-align:top;border-spacing:0px 0px;'><tr>";
  int counter = 0;
  for (
    int i = this->numberOfEmptyTokensStart; i < (
      *this->currentSyntacticStack
    ).size; i ++
  ) {
    SyntacticElement& currentElt = (*this->currentSyntacticStack)[i];
    if (ignoreCommandEnclosures) {
      if (currentElt.isCommandEnclosure()) {
        continue;
      }
    }
    out
    << "<td style='vertical-align:top;background-color:"
    << ((counter % 2 == 0) ? "#FAFAFA" : "#F0F0F0")
    << "'>"
    << currentElt.toStringHumanReadable(*this, includeLispifiedExpressions)
    << "</td>";
    counter ++;
  }
  out << "</tr></table>";
  return out.str();
}

SemisimpleSubalgebras& ObjectContainer::
getSemisimpleSubalgebrasCreateIfNotPresent(const DynkinType& input) {
  MacroRegisterFunctionWithName(
    "ObjectContainer::getSemisimpleSubalgebrasCreateIfNotPresent"
  );
  SemisimpleSubalgebras& result =
  this->semisimpleSubalgebras.getValueCreateNoInitialization(input);
  return result;
}

SemisimpleLieAlgebra& ObjectContainer::getLieAlgebraCreateIfNotPresent(
  const DynkinType& input
) {
  MacroRegisterFunctionWithName(
    "ObjectContainer::getLieAlgebraCreateIfNotPresent"
  );
  bool needToInit = false;
  if (!this->semisimpleLieAlgebras.contains(input)) {
    needToInit = true;
  }
  SemisimpleLieAlgebra& lieAlgebra =
  this->semisimpleLieAlgebras.getValueCreateNoInitialization(input);
  if (needToInit) {
    lieAlgebra.weylGroup.makeFromDynkinType(input);
  }
  return lieAlgebra;
}

WeylGroupData& ObjectContainer::getWeylGroupDataCreateIfNotPresent(
  const DynkinType& input
) {
  MacroRegisterFunctionWithName(
    "ObjectContainer::getWeylGroupDataCreateIfNotPresent"
  );
  return this->getLieAlgebraCreateIfNotPresent(input).weylGroup;
}

std::string ObjectContainer::toStringJavascriptForUserInputBoxes() {
  JSData inputBoxes;
  JSData inputBoxNames = JSData::makeEmptyArray();
  JSData inputBoxToSliderUpdaters;
  for (int i = 0; i < this->userInputTextBoxesWithValues.size(); i ++) {
    InputBox& currentBox = this->userInputTextBoxesWithValues.values[i];
    inputBoxNames[i] = currentBox.name;
  }
  for (int i = 0; i < this->userInputTextBoxesWithValues.size(); i ++) {
    InputBox& currentBox = this->userInputTextBoxesWithValues.values[i];
    inputBoxToSliderUpdaters[currentBox.name] = currentBox.getSliderName();
  }
  inputBoxes["inputBoxNames"] = inputBoxNames;
  inputBoxes["inputBoxToSliderUpdaters"] = inputBoxToSliderUpdaters;
  return HtmlRoutines::scriptFromJSON("userInputBoxes", inputBoxes);
}

void ObjectContainer::resetPlots() {
  this->canvasPlotCounter = 0;
  for (int i = 0; i < this->allPlots.size; i ++) {
    this->allPlots[i].flagDivAlreadyDisplayed = false;
  }
}

void ObjectContainer::resetSliders() {
  this->userInputBoxSliderDisplayed.initializeFillInObject(
    this->userInputTextBoxesWithValues.size(), false
  );
}

bool ObjectContainer::checkConsistencyAfterReset() {
  MacroRegisterFunctionWithName("ObjectContainer::checkConsistencyAfterReset");
  if (this->weylGroupElements.size != 0) {
    global.fatal
    << "WeylGroupElements expected to be empty, got "
    << this->weylGroupElements.size
    << " elements. "
    << global.fatal;
  }
  if (this->semisimpleLieAlgebras.size() != 0) {
    global.fatal
    << "theSSLieAlgebras expected to be empty, got "
    << this->semisimpleLieAlgebras.size()
    << " elements. "
    << global.fatal;
  }
  if (this->weylGroupRepresentations.size != 0) {
    global.fatal
    << "weylGroupRepresentations expected to be empty, got "
    << this->weylGroupRepresentations.size
    << " elements. "
    << global.fatal;
  }
  if (this->weylGroupVirtualRepresentations.size != 0) {
    global.fatal
    << "weylGroupVirtualRepresentations expected to be empty, got "
    << this->weylGroupVirtualRepresentations.size
    << " elements. "
    << global.fatal;
  }
  if (this->polynomialsRational.size != 0) {
    global.fatal
    << "The rational polynomials are expected to be empty, have: "
    << this->polynomialsRational.size
    << " elements instead. "
    << global.fatal;
  }
  if (this->allRationals.size != 0) {
    global.fatal
    << "Rationals expected to be empty, have: "
    << this->allRationals.size
    << " elements instead. "
    << global.fatal;
  }
  if (this->doubleNumbers.size != 1) {
    global.fatal
    << "Doubles expected to be have exactly 1 element (namely, nan), have: "
    << this->doubleNumbers.size
    << " elements instead. "
    << global.fatal;
  }
  if (this->allAlgebraicNumbers.size != 0) {
    global.fatal
    << "Algebraic numbers expected to be empty, have: "
    << this->allAlgebraicNumbers.size
    << " elements instead. "
    << global.fatal;
  }
  return true;
}

void ObjectContainer::reset() {
  MacroRegisterFunctionWithName("ObjectContainer::reset");
  this->weylGroupElements.clear();
  this->weylGroupRepresentations.clear();
  this->weylGroupVirtualRepresentations.clear();
  this->categoryOModules.setSize(0);
  this->semisimpleLieAlgebras.clear();
  this->semisimpleLieAlgebraPointers.clear();
  this->semisimpleSubalgebras.clear();
  this->tensorElements.clear();
  this->polynomialsRational.clear();
  this->polynomialsAlgebraic.clear();
  this->polynomialsModular.clear();
  this->weylAlgebraElements.clear();
  this->universalEnvelopingAlgebraElements.clear();
  this->rationalFunctions.clear();
  this->rationalFunctionsAlgebraic.clear();
  this->rationalFunctionsModular.clear();
  this->allRationals.clear();
  this->charactersSemisimpleLieAlgebraFiniteDimensional.clear();
  this->doubleNumbers.clear();
  this->doubleNumbers.addOnTop(std::nan(""));
  this->allStrings.clear();
  this->jsonObjects.clear();
  this->expressionNotation.clear();
  this->expressionWithNotation.clear();
  this->constraints.clear();
  this->lakshmibaiSeshadriPaths.clear();
  this->matrixTensorRationals.clear();
  this->elementsModP.clear();
  this->allPlots.setSize(0);
  this->algebraicClosure.reset();
  this->allAlgebraicNumbers.clear();
  this->littelmannOperators.clear();
  this->slTwoSubalgebras.setSize(0);
  this->weights.clear();
  this->weightsPolynomial.clear();
  this->hyperOctahedralGroups.setSize(0);
  this->elementsHyperOctGroup.clear();
  this->pseudoRandom.setRandomSeedSmall(
    static_cast<uint32_t>(time(nullptr))
  );
  this->userInputTextBoxesWithValues.clear();
  this->graphicsScripts.clear();
  this->ellipticCurveElementsZmodP.clear();
  this->ellipticCurveElementsRational.clear();
  this->canvasPlotCounter = 0;
  this->elementsOfSemisimpleLieAlgebrasWithAlgebraicCoefficients.clear();
  this->resetPlots();
  this->resetSliders();
}

bool Expression::isMeltable(int* numberOfResultingChildren) const {
  this->checkInitialization();
  if (!this->startsWith(this->owner->opMelt(), 2)) {
    return false;
  }
  if (numberOfResultingChildren != nullptr) {
    if (
      !(*this)[1].startsWith(this->owner->opCommandSequence())
    ) {
      *numberOfResultingChildren = 1;
    } else {
      *numberOfResultingChildren = (*this)[1].children.size - 1;
    }
  }
  return true;
}

bool Expression::mergeContextsMyAruments(
  Expression& output, std::stringstream* commentsOnFailure
) const {
  MacroRegisterFunctionWithName("Expression::mergeContextsMyAruments");
  this->checkInitialization();
  if (this->size() < 2) {
    return false;
  }
  for (int i = 1; i < this->size(); i ++) {
    if (!(*this)[i].isBuiltInType()) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "<hr>Failed to merge the arguments of the expression "
        << this->toString()
        << ": the argument "
        << (*this)[i].toString()
        << " is not of built-in type";
      }
      return false;
    }
  }
  ExpressionContext commonContext = (*this)[1].getContext();
  bool needsMerge = false;
  for (int i = 2; i < this->size(); i ++) {
    if (!(commonContext == (*this)[i].getContext())) {
      needsMerge = true;
      break;
    }
  }
  if (!needsMerge) {
    output = *this;
    return true;
  }
  for (int i = 2; i < this->size(); i ++) {
    if (!(*this)[i].isBuiltInType()) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "<hr>Failed to merge contexts of arguments: "
        << "an argument is not of built-in type. ";
      }
      return false;
    }
    ExpressionContext currentContext = (*this)[i].getContext();
    if (!commonContext.mergeContexts(currentContext, commonContext)) {
      *this->owner
      << "<hr>Failed to merge context "
      << commonContext.toString()
      << " with "
      << (*this)[i].getContext().toString();
      return false;
    }
  }
  output.reset(*this->owner, this->size());
  output.addChildOnTop((*this)[0]);
  Expression convertedE;
  for (int i = 1; i < this->size(); i ++) {
    convertedE = (*this)[i];
    if (
      !convertedE.setContextAtLeastEqualTo(commonContext, commentsOnFailure)
    ) {
      *this->owner
      << "<hr>Failed to convert "
      << convertedE.toString()
      << " to context "
      << commonContext.toString();
      return false;
    }
    output.addChildOnTop(convertedE);
  }
  return true;
}

bool CalculatorBasics::meltBrackets(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::outerMeltBrackets");
  RecursionDepthCounter counter(&calculator.recursionDepth);
  if (!input.startsWith(calculator.opCommandSequence())) {
    return false;
  }
  int tempInt;
  int childIncrease = 0;
  bool found = false;
  for (int i = 0; i < input.size(); i ++) {
    const Expression& currentChild = input[i];
    if (currentChild.isMeltable(&tempInt)) {
      found = true;
      childIncrease += tempInt - 1;
    }
  }
  if (!found) {
    return false;
  }
  output.reset(calculator, input.size() + childIncrease);
  output.addChildAtomOnTop(calculator.opCommandSequence());
  for (int i = 1; i < input.size(); i ++) {
    const Expression& currentChild = input[i];
    if (!currentChild.isMeltable()) {
      output.addChildOnTop(input[i]);
      continue;
    }
    if (
      !currentChild[1].startsWith(calculator.opCommandSequence())
    ) {
      output.addChildOnTop(currentChild[1]);
      continue;
    }
    for (int j = 1; j < currentChild[1].size(); j ++) {
      output.addChildOnTop(currentChild[1][j]);
    }
  }
  return true;
}
