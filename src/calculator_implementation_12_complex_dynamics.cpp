// The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_inner_functions.h"

class RealAndImaginaryPartExtractor {
  Calculator* owner;
  Expression original;
public:
  Expression variable;
  Expression imaginaryPartVariable;
  Expression realPartVariable;
  Expression realPart;
  Expression imaginaryPart;
  HashedList<InputBox> parameters;
  List<std::string> parametersJS;
  bool encounteredNonConvertible;
  RealAndImaginaryPartExtractor(Calculator* inputOwner) {
    this->owner = inputOwner;
    this->encounteredNonConvertible = false;
  }
  void computeParameters();
  bool extract(
    const Expression& input, std::stringstream* commentsOnFailure
  );
  bool extractRecursive(
    const Expression& input,
    Expression& outputRealPart,
    Expression& outputImaginaryPart,
    std::stringstream* commentsOnFailure
  );
  bool extractFromDivision(
    const Expression& numerator,
    const Expression& denominator,
    Expression& outputRealPart,
    Expression& outputImaginaryPart,
    std::stringstream* commentsOnFailure
  );
  bool extractFromPower(
    const Expression& base,
    const Expression& exponent,
    Expression& outputRealPart,
    Expression& outputImaginaryPart,
    std::stringstream* commentsOnFailure
  );
  bool extractFromSum(
    const Expression& left,
    const Expression& right,
    Expression& outputRealPart,
    Expression& outputImaginaryPart,
    std::stringstream* commentsOnFailure
  );
  bool extractFromProduct(
    const Expression& left,
    const Expression& right,
    Expression& outputRealPart,
    Expression& outputImaginaryPart,
    std::stringstream* commentsOnFailure
  );
};

bool CalculatorFunctionsComplexDynamics::plotEscapeMap(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsComplexDynamics::plotEscapeMap");
  if (input.size() < 2) {
    return false;
  }
  RealAndImaginaryPartExtractor extractor(&calculator);
  if (!extractor.extract(input[1], &calculator.comments)) {
    return false;
  }
  Expression realPartJavascript;
  Expression imaginaryPartJavascript;
  CalculatorFunctionsPlot::functionMakeJavascriptExpression(
    calculator, extractor.realPart, realPartJavascript
  );
  CalculatorFunctionsPlot::functionMakeJavascriptExpression(
    calculator, extractor.imaginaryPart, imaginaryPartJavascript
  );
  std::string javascriptX;
  std::string javascriptY;
  if (
    !realPartJavascript.isOfType(&javascriptX) ||
    !imaginaryPartJavascript.isOfType(&javascriptY)
  ) {
    return
    calculator
    << "Failed to extract javascript from the real part: "
    << extractor.realPart
    << " and/or the imaginary part: "
    << extractor.imaginaryPart
    << ".";
    return false;
  }
  Plot escapeMap;
  escapeMap.dimension = 2;
  escapeMap.drawEscapeMap(
    extractor.realPart,
    javascriptX,
    extractor.realPartVariable.toString(),
    extractor.imaginaryPart,
    javascriptY,
    extractor.imaginaryPartVariable.toString(),
    extractor.parametersJS
  );
  escapeMap.drawGrid();
  escapeMap.setViewWindow(- 4, - 4, 4, 4);
  std::stringstream out;
  out
  << "Variable:<br>"
  << "\\("
  << extractor.variable
  << "="
  << extractor.realPartVariable.toString()
  << "+i\\left("
  << extractor.imaginaryPartVariable.toString()
  << "\\right)\\)";
  out
  << "<br>Function:<br>"
  << "\\("
  << input[1]
  << "="
  << extractor.realPart.toString()
  << "+i\\left("
  << extractor.imaginaryPart.toString()
  << "\\right)"
  << "\\)";
  out << "<br>";
  out << escapeMap.getPlotHtml2d(calculator);
  return output.assignValue(calculator, out.str());
}

bool RealAndImaginaryPartExtractor::extract(
  const Expression& input, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("RealAndImaginaryPartExtractor::extract");
  this->original = input;
  HashedList<Expression> variables;
  input.getFreeVariables(variables, true);
  Expression i;
  i.makeAtom(*this->owner, "i");
  variables.removeFirstOccurenceSwapWithLast(i);
  if (variables.size != 1) {
    return false;
  }
  this->variable = variables[0];
  this->realPartVariable = this->owner->getNewAtom("x");
  this->imaginaryPartVariable = this->owner->getNewAtom("y");
  bool result =
  this->extractRecursive(
    this->original,
    this->realPart,
    this->imaginaryPart,
    commentsOnFailure
  );
  if (result) {
    this->computeParameters();
  }
  return result;
}

void RealAndImaginaryPartExtractor::computeParameters() {
  for (int i = 0; i < this->parameters.size; i ++) {
    this->parametersJS.addOnTop(this->parameters[i].getSliderName());
  }
}

bool RealAndImaginaryPartExtractor::extractRecursive(
  const Expression& input,
  Expression& outputRealPart,
  Expression& outputImaginaryPart,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("RealAndImaginaryPartExtractor::extractRecursive");
  if (input.startsWith(this->owner->opDivide(), 3)) {
    return
    this->extractFromDivision(
      input[1],
      input[2],
      outputRealPart,
      outputImaginaryPart,
      commentsOnFailure
    );
  }
  if (input.startsWith(this->owner->opPower(), 3)) {
    return
    this->extractFromPower(
      input[1],
      input[2],
      outputRealPart,
      outputImaginaryPart,
      commentsOnFailure
    );
  }
  if (input.startsWith(this->owner->opPlus(), 3)) {
    return
    this->extractFromSum(
      input[1],
      input[2],
      outputRealPart,
      outputImaginaryPart,
      commentsOnFailure
    );
  }
  if (input.startsWith(this->owner->opTimes(), 3)) {
    return
    this->extractFromProduct(
      input[1],
      input[2],
      outputRealPart,
      outputImaginaryPart,
      commentsOnFailure
    );
  }
  Rational constant;
  if (input.isOfType(&constant)) {
    outputRealPart = input;
    outputImaginaryPart = this->owner->expressionZero();
    return true;
  }
  if (input == "i") {
    outputRealPart = input;
    outputImaginaryPart = this->owner->expressionOne();
    return true;
  }
  InputBox inputBox;
  if (input.isOfType<InputBox>(&inputBox)) {
    this->parameters.addOnTopNoRepetition(inputBox);
    outputRealPart = input;
    outputImaginaryPart = this->owner->expressionZero();
    return true;
  }
  if (input == this->variable) {
    outputRealPart = this->realPartVariable;
    outputImaginaryPart = this->imaginaryPartVariable;
    return true;
  }
  if (!this->encounteredNonConvertible && commentsOnFailure != nullptr) {
    this->encounteredNonConvertible = true;
    *commentsOnFailure
    << "Failed to extract real/imaginary part from: "
    << input.toString();
  }
  return false;
}

bool RealAndImaginaryPartExtractor::extractFromDivision(
  const Expression& numerator,
  const Expression& denominator,
  Expression& outputRealPart,
  Expression& outputImaginaryPart,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("RealAndImaginaryPartExtractor::extractFromDivision");
  Expression numeratorRealPart;
  Expression numeratorImaginaryPart;
  Expression denominatorRealPart;
  Expression denominatorImaginaryPart;
  if (
    !this->extractRecursive(
      numerator,
      numeratorRealPart,
      numeratorImaginaryPart,
      commentsOnFailure
    ) ||
    !this->extractRecursive(
      denominator,
      denominatorRealPart,
      denominatorImaginaryPart,
      commentsOnFailure
    )
  ) {
    return false;
  }
  if (denominatorImaginaryPart.isEqualToZero()) {
    outputRealPart = numeratorRealPart / denominatorRealPart;
    outputImaginaryPart = numeratorImaginaryPart / denominatorRealPart;
    return true;
  }
  Expression rho =
  denominatorRealPart * denominatorRealPart +
  denominatorImaginaryPart * denominatorImaginaryPart;
  outputRealPart =
  numeratorRealPart * denominatorRealPart +
  numeratorImaginaryPart * denominatorImaginaryPart;
  outputRealPart /= rho;
  outputImaginaryPart =
  this->owner->expressionMinusOne() * numeratorRealPart *
  denominatorImaginaryPart +
  numeratorImaginaryPart * denominatorRealPart;
  outputImaginaryPart /= rho;
  return true;
}

bool RealAndImaginaryPartExtractor::extractFromPower(
  const Expression& base,
  const Expression& exponent,
  Expression& outputRealPart,
  Expression& outputImaginaryPart,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("RealAndImaginaryPartExtractor::extractFromPower");
  int exponentInteger = 0;
  Expression baseRealPart;
  Expression baseImaginaryPart;
  if (!exponent.isSmallInteger(&exponentInteger)) {
    return false;
  }
  if (exponentInteger < 0) {
    exponentInteger *= - 1;
    Expression transformed;
    Expression negatedExponent;
    negatedExponent.assignValue(*this->owner, exponentInteger);
    transformed.makeExponentReduce(*this->owner, base, exponentInteger);
    transformed = this->owner->expressionOne() / transformed;
    return
    this->extractRecursive(
      transformed, outputRealPart, outputImaginaryPart, commentsOnFailure
    );
  }
  if (
    !this->extractRecursive(
      base, baseRealPart, baseImaginaryPart, commentsOnFailure
    )
  ) {
    return false;
  }
  if (exponentInteger > 20) {
    return false;
  }
  List<Expression> real;
  List<Expression> imaginary;
  for (int k = 0; k <= exponentInteger; k ++) {
    List<Expression> multiplicands;
    Rational coefficient = Rational::nChooseK(exponentInteger, k);
    int exponentMinusK = exponentInteger - k;
    if (k % 4 == 2 || k % 4 == 3) {
      coefficient *= - 1;
    }
    Expression contributionX;
    Expression contributionY;
    contributionX.makeExponentReduce(
      *this->owner, baseRealPart, exponentMinusK
    );
    contributionY.makeExponentReduce(*this->owner, baseImaginaryPart, k);
    multiplicands.addOnTop(this->owner->expressionRational(coefficient));
    multiplicands.addOnTop(contributionX);
    multiplicands.addOnTop(contributionY);
    Expression next;
    next.makeProductReduceOnes(*this->owner, multiplicands);
    if (k % 2 == 0) {
      real.addOnTop(next);
    } else {
      imaginary.addOnTop(next);
    }
  }
  outputRealPart.makeSum(*this->owner, real);
  outputImaginaryPart.makeSum(*this->owner, imaginary);
  return true;
}

bool RealAndImaginaryPartExtractor::extractFromSum(
  const Expression& left,
  const Expression& right,
  Expression& outputRealPart,
  Expression& outputImaginaryPart,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("RealAndImaginaryPartExtractor::extractFromSum");
  Expression leftRealPart;
  Expression leftImaginaryPart;
  Expression rightRealPart;
  Expression rightImaginaryPart;
  if (
    !this->extractRecursive(
      left, leftRealPart, leftImaginaryPart, commentsOnFailure
    ) ||
    !this->extractRecursive(
      right, rightRealPart, rightImaginaryPart, commentsOnFailure
    )
  ) {
    return false;
  }
  outputRealPart = leftRealPart + rightRealPart;
  outputImaginaryPart = leftImaginaryPart + rightImaginaryPart;
  return true;
}

bool RealAndImaginaryPartExtractor::extractFromProduct(
  const Expression& left,
  const Expression& right,
  Expression& outputRealPart,
  Expression& outputImaginaryPart,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("RealAndImaginaryPartExtractor::extractFromProduct");
  Expression leftRealPart;
  Expression leftImaginaryPart;
  Expression rightRealPart;
  Expression rightImaginaryPart;
  if (
    !this->extractRecursive(
      left, leftRealPart, leftImaginaryPart, commentsOnFailure
    ) ||
    !this->extractRecursive(
      right, rightRealPart, rightImaginaryPart, commentsOnFailure
    )
  ) {
    return false;
  }

  outputRealPart =
  leftRealPart * rightRealPart - leftImaginaryPart * rightImaginaryPart;
  outputImaginaryPart =
  leftImaginaryPart * rightRealPart + leftRealPart * rightImaginaryPart;
  return true;
}
