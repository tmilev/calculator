#include "math_extra_semisimple_lie_algebras_implementation.h"
#include "math_extra_weyl_algebras_implementation.h"
#include "calculator_interface.h"
#include "calculator_inner_functions.h"
#include "calculator_inner_typed_functions.h"
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_rational_function_implementation.h"
#include "calculator_lie_theory.h"

// Start WithContext specializations.
template < >
void WithContext<Polynomial<Rational> >::toExpression(
  Calculator& calculator, Expression& output
) {
  CalculatorConversions::expressionFromPolynomial<Rational>(
    calculator, this->content, output, &this->context
  );
}

template < >
void WithContext<Polynomial<AlgebraicNumber> >::toExpression(
  Calculator& calculator, Expression& output
) {
  CalculatorConversions::expressionFromPolynomial<AlgebraicNumber>(
    calculator, this->content, output, &this->context
  );
}

// End WithContext specializations.
// start CalculatorConversions::convertWithoutComputation specializations.
template < >
bool CalculatorConversions::convertWithoutComputation<
  Polynomial<AlgebraicNumber>
>(
  Calculator& calculator,
  const Expression& input,
  WithContext<Polynomial<AlgebraicNumber> >& output,
  bool* outputConversionByOtherMeansNotDesired
) {
  STACK_TRACE("CalculatorConversions::convertWithoutComputation");
  (void) calculator;
  input.checkInitialization();
  WithContext<Rational> rational;
  if (input.isOfTypeWithContext(&rational)) {
    output.context = rational.context;
    output.content.makeConstant(rational.content);
    return true;
  }
  WithContext<AlgebraicNumber> algebraicNumber;
  if (input.isOfTypeWithContext(&algebraicNumber)) {
    output.context = algebraicNumber.context;
    output.content.makeConstant(algebraicNumber.content);
    return true;
  }
  WithContext<Polynomial<Rational> > polynomialRational;
  if (input.isOfTypeWithContext(&polynomialRational)) {
    // Converts types.
    output.content = polynomialRational.content;
    output.context = polynomialRational.context;
    return true;
  }
  if (outputConversionByOtherMeansNotDesired != nullptr) {
    if (input.isOfType<Polynomial<ElementZmodP> >()) {
      *outputConversionByOtherMeansNotDesired = true;
    }
  }
  return input.isOfTypeWithContext(&output);
}

template < >
bool CalculatorConversions::convertWithoutComputation<Polynomial<Rational> >(
  Calculator& calculator,
  const Expression& input,
  WithContext<Polynomial<Rational> >& output,
  bool* outputConversionByOtherMeansNotDesired
) {
  STACK_TRACE("CalculatorConversions::convertWithoutComputation");
  (void) calculator;
  input.checkInitialization();
  WithContext<Rational> rational;
  if (input.isOfTypeWithContext(&rational)) {
    output.content.makeConstant(rational.content);
    output.context = rational.context;
    return true;
  }
  WithContext<ElementWeylAlgebra<Rational> > elementWeylAlgebra;
  if (input.isOfTypeWithContext(&elementWeylAlgebra)) {
    if (elementWeylAlgebra.content.isPolynomial(&output.content)) {
      // An element of a weyl algebra that happens to be a polynomial.
      output.context = elementWeylAlgebra.context;
      return true;
    }
    // An element of a Weyl algebra that is not a polynomial.
    return false;
  }
  if (outputConversionByOtherMeansNotDesired != nullptr) {
    if (
      input.isOfType<Polynomial<ElementZmodP> >() ||
      input.isOfType<Polynomial<ElementZmodP> >()
    ) {
      *outputConversionByOtherMeansNotDesired = true;
    }
  }
  return input.isOfTypeWithContext(&output);
}

template < >
bool CalculatorConversions::convertWithoutComputation<
  Polynomial<ElementZmodP>
>(
  Calculator& calculator,
  const Expression& input,
  WithContext<Polynomial<ElementZmodP> >& output,
  bool* outputConversionByOtherMeansNotDesired
) {
  (void) calculator;
  (void) outputConversionByOtherMeansNotDesired;
  return input.isOfTypeWithContext(&output);
}

template < >
bool CalculatorConversions::convertWithoutComputation<
  PolynomialModuloPolynomial<ElementZmodP>
>(
  Calculator& calculator,
  const Expression& input,
  WithContext<PolynomialModuloPolynomial<ElementZmodP> >& output,
  bool* outputConversionByOtherMeansNotDesired
) {
  (void) calculator;
  (void) outputConversionByOtherMeansNotDesired;
  return input.isOfTypeWithContext(&output);
}

template < >
bool CalculatorConversions::convertWithoutComputation<
  RationalFraction<Rational>
>(
  Calculator& calculator,
  const Expression& input,
  WithContext<RationalFraction<Rational> >& output,
  bool* outputConversionByOtherMeansNotDesired
) {
  STACK_TRACE("CalculatorConversions::convertWithoutComputation");
  (void) calculator;
  (void) outputConversionByOtherMeansNotDesired;
  input.checkInitialization();
  WithContext<Rational> rational;
  if (input.isOfTypeWithContext(&rational)) {
    output.content = rational.content;
    output.context = rational.context;
    return true;
  }
  WithContext<AlgebraicNumber> algebraicNumber;
  if (input.isOfTypeWithContext(&algebraicNumber)) {
    Rational rationalValue;
    if (
      !input.getValue<AlgebraicNumber>().isRational(&rationalValue)
    ) {
      return false;
    }
    output.content = rationalValue;
    output.context = algebraicNumber.context;
    return true;
  }
  WithContext<Polynomial<Rational> > polynomial;
  if (input.isOfTypeWithContext<Polynomial<Rational> >(&polynomial)) {
    output.content = polynomial.content;
    output.context = polynomial.context;
    return true;
  }
  return input.isOfTypeWithContext(&output);
}

template < >
bool CalculatorConversions::convertWithoutComputation<
  RationalFraction<AlgebraicNumber>
>(
  Calculator& calculator,
  const Expression& input,
  WithContext<RationalFraction<AlgebraicNumber> >& output,
  bool* outputConversionByOtherMeansNotDesired
) {
  STACK_TRACE(
    "CalculatorConversions::"
    "convertWithoutComputation_RationalFraction_AlgebraicNumber"
  );
  (void) outputConversionByOtherMeansNotDesired;
  input.checkInitialization();
  AlgebraicClosureRationals* closure =
  &calculator.objectContainer.algebraicClosure;
  WithContext<Rational> rational;
  if (input.isOfTypeWithContext(&rational)) {
    AlgebraicNumber value;
    value.assignRational(rational.content, closure);
    output.content = value;
    output.context = rational.context;
    return true;
  }
  WithContext<AlgebraicNumber> algebraicNumber;
  if (input.isOfTypeWithContext(&algebraicNumber)) {
    output.content = algebraicNumber.content;
    output.context = algebraicNumber.context;
    return true;
  }
  // TODO(tmilev): please implement conversion from Polynomial<Rational> to
  // Polynomial<AlgebraicNumber>.
  WithContext<Polynomial<AlgebraicNumber> > polynomial;
  if (input.isOfTypeWithContext(&polynomial)) {
    output.content = polynomial.content;
    output.context = polynomial.context;
    return true;
  }
  return input.isOfTypeWithContext(&output);
}

template < >
bool CalculatorConversions::convertWithoutComputation<
  ElementWeylAlgebra<Rational>
>(
  Calculator& calculator,
  const Expression& input,
  WithContext<ElementWeylAlgebra<Rational> >& output,
  bool* outputConversionByOtherMeansNotDesired
) {
  STACK_TRACE(
    "CalculatorConversions::"
    "convertWithoutComputation_ElementWeylAlgebra_Rational"
  );
  (void) calculator;
  (void) outputConversionByOtherMeansNotDesired;
  input.checkInitialization();
  WithContext<Rational> rational;
  if (input.isOfTypeWithContext(&rational)) {
    // Rational is allowed to have non-empty context generated by merger of
    // contexts.
    output.content.makeConstant(input.getValue<Rational>());
    output.context = rational.context;
    return true;
  }
  WithContext<Polynomial<Rational> > polynomial;
  if (input.isOfTypeWithContext(&polynomial)) {
    output.context = polynomial.context;
    output.content.assignPolynomial(polynomial.content);
    return true;
  }
  bool result = input.isOfTypeWithContext(&output);
  return result;
}

template < >
bool CalculatorConversions::convertWithoutComputation<
  Weight<Polynomial<Rational> >
>(
  Calculator& calculator,
  const Expression& input,
  WithContext<Weight<Polynomial<Rational> > >& output,
  bool* outputConversionByOtherMeansNotDesired
) {
  (void) calculator;
  (void) outputConversionByOtherMeansNotDesired;
  input.checkInitialization();
  return input.isOfTypeWithContext(&output);
}

template < >
bool CalculatorConversions::convertWithoutComputation<
  ElementUniversalEnveloping<RationalFraction<Rational> >
>(
  Calculator& calculator,
  const Expression& input,
  WithContext<ElementUniversalEnveloping<RationalFraction<Rational> > >& output
  ,
  bool* outputConversionByOtherMeansNotDesired
) {
  STACK_TRACE(
    "CalculatorConversions::"
    "convertWithoutComputation_ElementUniversalEnveloping"
  );
  (void) outputConversionByOtherMeansNotDesired;
  SemisimpleLieAlgebra* owner =
  input.getAmbientSemisimpleLieAlgebraNonConstUseWithCaution();
  if (owner == nullptr) {
    return false;
  }
  WithContext<RationalFraction<Rational> > rationalFraction;
  if (
    CalculatorConversions::convertWithoutComputation(
      calculator, input, rationalFraction
    )
  ) {
    output.content.makeConstant(rationalFraction.content, *owner);
    output.context = rationalFraction.context;
    return true;
  }
  bool result = input.isOfTypeWithContext(&output);
  return result;
}

template < >
bool CalculatorConversions::convertWithoutComputation<
  ElementSemisimpleLieAlgebra<AlgebraicNumber>
>(
  Calculator& calculator,
  const Expression& input,
  WithContext<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& output,
  bool* outputConversionByOtherMeansNotDesired
) {
  STACK_TRACE(
    "CalculatorConversions::"
    "convertWithoutComputation_ElementSemisimpleLieAlgebra"
  );
  (void) outputConversionByOtherMeansNotDesired;
  WithContext<ElementUniversalEnveloping<RationalFraction<Rational> > >
  element;
  if (
    !CalculatorConversions::convertWithoutComputation(
      calculator, input, element
    )
  ) {
    return false;
  }
  ElementSemisimpleLieAlgebra<Rational> lieAlgebraElement;
  // Convert coefficients here.
  if (!element.content.isLieAlgebraElementRational(lieAlgebraElement)) {
    return false;
  }
  output.context = element.context;
  output.content = lieAlgebraElement;
  return true;
}

template < >
bool CalculatorConversions::convertWithoutComputation<
  ElementTensorsGeneralizedVermas<RationalFraction<Rational> >
>(
  Calculator& calculator,
  const Expression& input,
  WithContext<
    ElementTensorsGeneralizedVermas<RationalFraction<Rational> >
  >& output,
  bool* outputConversionByOtherMeansNotDesired
) {
  (void) calculator;
  (void) outputConversionByOtherMeansNotDesired;
  return input.isOfTypeWithContext(&output);
}

template < >
bool CalculatorConversions::convertWithoutComputation<Rational>(
  Calculator& calculator,
  const Expression& input,
  WithContext<Rational>& output,
  bool* outputConversionByOtherMeansNotDesired
) {
  (void) calculator;
  (void) outputConversionByOtherMeansNotDesired;
  return input.isOfTypeWithContext(&output);
}

template < >
bool CalculatorConversions::convertWithoutComputation<ElementZmodP>(
  Calculator& calculator,
  const Expression& input,
  WithContext<ElementZmodP>& output,
  bool* outputConversionByOtherMeansNotDesired
) {
  (void) calculator;
  (void) outputConversionByOtherMeansNotDesired;
  return input.isOfTypeWithContext(&output);
}

template < >
bool CalculatorConversions::convertWithoutComputation<AlgebraicNumber>(
  Calculator& calculator,
  const Expression& input,
  WithContext<AlgebraicNumber>& output,
  bool* outputConversionByOtherMeansNotDesired
) {
  STACK_TRACE("CalculatorConversions::convertWithoutComputation");
  (void) outputConversionByOtherMeansNotDesired;
  WithContext<Rational> rational;
  if (input.isOfTypeWithContext(&rational)) {
    output.context = rational.context;
    output.content.owner = &calculator.objectContainer.algebraicClosure;
    output.content = rational.content;
    return true;
  }
  return input.isOfTypeWithContext(&output);
}

template < >
bool CalculatorConversions::convertWithoutComputation<double>(
  Calculator& calculator,
  const Expression& input,
  WithContext<double>& output,
  bool* outputConversionByOtherMeansNotDesired
) {
  (void) calculator;
  (void) outputConversionByOtherMeansNotDesired;
  return input.isOfTypeWithContext(&output);
}

template < >
bool CalculatorConversions::convertWithoutComputation<std::string>(
  Calculator& calculator,
  const Expression& input,
  WithContext<std::string>& output,
  bool* outputConversionByOtherMeansNotDesired
) {
  (void) calculator;
  (void) outputConversionByOtherMeansNotDesired;
  return input.isOfTypeWithContext(&output);
}

// end Expression::convertWithoutComputation specializations.
// start Expression::convert specializations.
template < >
bool CalculatorConversions::convert<Polynomial<Rational> >(
  Calculator& calculator,
  const Expression& input,
  WithContext<Polynomial<Rational> >& output
) {
  return
  CalculatorConversions::functionPolynomial(
    calculator, input, output, - 1, - 1, false
  );
}

template < >
bool CalculatorConversions::convert<Polynomial<ElementZmodP> >(
  Calculator& calculator,
  const Expression& input,
  WithContext<Polynomial<ElementZmodP> >& output
) {
  return
  CalculatorConversions::convertWithoutComputation(
    calculator, input, output, nullptr
  );
}

template < >
bool CalculatorConversions::convert<Rational>(
  Calculator& calculator,
  const Expression& input,
  WithContext<Rational>& output
) {
  (void) calculator;
  return input.isOfTypeWithContext(&output);
}

template < >
bool CalculatorConversions::convert<AlgebraicNumber>(
  Calculator& calculator,
  const Expression& input,
  WithContext<AlgebraicNumber>& output
) {
  (void) calculator;
  return input.isOfTypeWithContext(&output);
}

template < >
bool CalculatorConversions::convert<double>(
  Calculator& calculator,
  const Expression& input,
  WithContext<double>& output
) {
  if (
    CalculatorConversions::convertWithoutComputation(
      calculator, input, output
    )
  ) {
    return true;
  }
  double outputDouble = 0;
  if (!input.evaluatesToDouble(&outputDouble)) {
    return false;
  }
  output.context.initialize(calculator);
  output.content = outputDouble;
  return true;
}

template < >
bool CalculatorConversions::convert(
  Calculator& calculator,
  const Expression& input,
  WithContext<RationalFraction<Rational> >& output
) {
  return
  CalculatorConversions::functionRationalFraction(
    calculator, input, output, false
  );
}

template < >
bool CalculatorConversions::convert(
  Calculator& calculator,
  const Expression& input,
  WithContext<SemisimpleLieAlgebra*>& output
) {
  Expression converted;
  if (input.isOfTypeWithContext(&output)) {
    return true;
  }
  return
  CalculatorConversions::functionSemisimpleLieAlgebraFromDynkinType(
    calculator, input, converted, output
  );
}

template < >
bool CalculatorConversions::convert(
  Calculator& calculator,
  const Expression& input,
  WithContext<ElementWeylAlgebra<Rational> >& output
) {
  return
  CalculatorConversions::convertWithoutComputation(calculator, input, output);
}

template < >
bool CalculatorConversions::convert(
  Calculator& calculator,
  const Expression& input,
  WithContext<ElementUniversalEnveloping<RationalFraction<Rational> > >& output
) {
  return
  CalculatorConversions::convertWithoutComputation(calculator, input, output);
}

template < >
bool CalculatorConversions::convert(
  Calculator& calculator,
  const Expression& input,
  WithContext<Polynomial<AlgebraicNumber> >& output
) {
  return
  CalculatorConversions::functionPolynomial(
    calculator, input, output, - 1, - 1, false
  );
}

template < >
bool CalculatorConversions::convert(
  Calculator& calculator,
  const Expression& input,
  WithContext<std::string>& output
) {
  return
  CalculatorConversions::convertWithoutComputation(calculator, input, output);
}

template < >
bool CalculatorConversions::convert(
  Calculator& calculator,
  const Expression& input,
  WithContext<ElementZmodP>& output
) {
  return
  CalculatorConversions::convertWithoutComputation(calculator, input, output);
}

// end Expression::convert specializations.
bool CalculatorConversions::expressionFromChevalleyGenerator(
  Calculator& calculator,
  const ChevalleyGenerator& input,
  Expression& output
) {
  STACK_TRACE("CalculatorConversions::expressionFromChevalleyGenerator");
  input.checkInitialization();
  output.reset(calculator, 2);
  Expression generatorLetterE, generatorIndexE;
  if (
    input.generatorIndex >= input.owner->getNumberOfPositiveRoots() &&
    input.generatorIndex < input.owner->getNumberOfPositiveRoots() +
    input.owner->getRank()
  ) {
    generatorLetterE.makeAtom(
      calculator,
      calculator.addOperationNoRepetitionOrReturnIndexFirst("h")
    );
  } else {
    generatorLetterE.makeAtom(
      calculator,
      calculator.addOperationNoRepetitionOrReturnIndexFirst("g")
    );
  }
  generatorIndexE.assignValue(
    calculator,
    input.owner->getDisplayIndexFromGenerator(input.generatorIndex)
  );
  return
  output.makeXOX(
    calculator,
    calculator.opUnderscore(),
    generatorLetterE,
    generatorIndexE
  );
}

bool CalculatorConversions::functionSemisimpleLieAlgebraFromDynkinType(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorConversions::functionSemisimpleLieAlgebraFromDynkinType"
  );
  WithContext<SemisimpleLieAlgebra*> unused;
  return
  CalculatorConversions::functionSemisimpleLieAlgebraFromDynkinType(
    calculator, input, output, unused
  );
}

bool CalculatorConversions::semisimpleLieAlgebra(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorConversions::semisimpleLieAlgebra");
  if (input.size() != 2) {
    return calculator << "Semisimple Lie algebra expects a single argument. ";
  }
  WithContext<SemisimpleLieAlgebra*> unused;
  return
  CalculatorConversions::functionSemisimpleLieAlgebraFromDynkinType(
    calculator, input[1], output, unused
  );
}

bool CalculatorConversions::loadWeylGroup(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorConversions::loadWeylGroup");
  DynkinType dynkinType;
  if (!CalculatorConversions::dynkinType(calculator, input, dynkinType)) {
    return false;
  }
  SemisimpleLieAlgebra& subalgebra =
  calculator.objectContainer.getLieAlgebraCreateIfNotPresent(dynkinType);
  return output.assignValue(calculator, subalgebra.weylGroup);
}

bool CalculatorConversions::dynkinSimpleType(
  Calculator& calculator,
  const Expression& input,
  DynkinSimpleType& outputMon
) {
  STACK_TRACE("CalculatorConversions::dynkinSimpleType");
  if (input.size() != 2) {
    return
    calculator
    << "Dynkin simple type function requires a single argument. ";
  }
  return
  CalculatorConversions::functionDynkinSimpleType(
    calculator, input[1], outputMon
  );
}

bool CalculatorConversions::functionDynkinSimpleType(
  Calculator& calculator,
  const Expression& input,
  DynkinSimpleType& outputMon
) {
  STACK_TRACE("CalculatorConversions::functionDynkinSimpleType");
  Expression rankE, typeLetterE, scaleE;
  if (input.startsWith(calculator.opUnderscore(), 3)) {
    rankE = input[2];
    typeLetterE = input[1];
    if (typeLetterE.startsWith(calculator.opPower(), 3)) {
      scaleE = typeLetterE[2];
      typeLetterE = typeLetterE[1];
    } else {
      scaleE.assignValue(calculator, 1);
    }
  } else if (input.startsWith(calculator.opPower(), 3)) {
    scaleE = input[2];
    if (!input[1].startsWith(calculator.opUnderscore(), 3)) {
      return
      calculator
      << "<hr>Failed to extract rank, type from "
      << input[1].toString()
      << ". The expression does not have two children.";
    }
    rankE = input[1][2];
    typeLetterE = input[1][1];
  } else {
    return
    calculator
    << "<hr>Failed to extract rank, type, "
    << "first co-root length from: "
    << input.toString()
    << ". ";
  }
  Rational scale;
  if (!scaleE.isOfType<Rational>(&scale)) {
    return
    calculator
    << "<hr>Failed to extract first co-root length: "
    << "expression "
    << scaleE.toString()
    << " is not a rational number.";
  }
  if (scale <= 0) {
    return
    calculator
    << "<hr>Couldn't extract first co-root length: "
    << scale.toString()
    << " is non-positive.";
  }
  std::string typeName;
  if (!typeLetterE.isOperation(&typeName)) {
    return
    calculator
    << "I couldn't extract a type letter from "
    << typeLetterE.toString();
  }
  if (typeName.size() != 1) {
    return
    calculator
    << "<hr>Error while extracting Dynkin simple type: "
    << "the type of a simple Lie algebra "
    << "must be the letter A, B, C, D, E, F or G. "
    << "Instead, it is "
    << typeName + ". Error encountered while processing "
    << input.toString();
  }
  char weylLetter = typeName[0];
  if (weylLetter == 'a') {
    weylLetter = 'A';
  }
  if (weylLetter == 'b') {
    weylLetter = 'B';
  }
  if (weylLetter == 'c') {
    weylLetter = 'C';
  }
  if (weylLetter == 'd') {
    weylLetter = 'D';
  }
  if (weylLetter == 'e') {
    weylLetter = 'E';
  }
  if (weylLetter == 'f') {
    weylLetter = 'F';
  }
  if (weylLetter == 'g') {
    weylLetter = 'G';
  }
  if (
    !(
      weylLetter == 'A' ||
      weylLetter == 'B' ||
      weylLetter == 'C' ||
      weylLetter == 'D' ||
      weylLetter == 'E' ||
      weylLetter == 'F' ||
      weylLetter == 'G'
    )
  ) {
    return
    calculator
    << "The type of a simple Lie algebra must be "
    << "the letter A, B, C, D, E, F or G; error while processing "
    << input.toString();
  }
  int rank;
  if (!rankE.isSmallInteger(&rank)) {
    return
    calculator
    << "I wasn't able to extract rank from "
    << input.toString();
  }
  if (rank < 1 || rank > 20) {
    return
    calculator
    << "<hr>The rank of a simple Lie algebra "
    << "must be between 1 and 20; error while processing "
    << input.toString();
  }
  if (weylLetter == 'E' && (rank > 8 || rank < 3)) {
    return calculator << "<hr>Type E must have rank 6, 7 or 8 ";
  }
  if (weylLetter == 'D' && (rank < 3)) {
    return
    calculator
    << "<hr>Type D is expected to have rank 4 or more, "
    << "your input was of rank "
    << rank
    << ". ";
  }
  outputMon.makeArbitrary(weylLetter, rank, scale);
  return true;
}

bool CalculatorConversions::dynkinType(
  Calculator& calculator, const Expression& input, DynkinType& output
) {
  STACK_TRACE("CalculatorConversions::dynkinType");
  RecursionDepthCounter recursionCounter(&calculator.recursionDepth);
  if (input.size() != 2) {
    return calculator << "Dynkin type takes as input one argument. ";
  }
  return
  CalculatorConversions::functionDynkinType(calculator, input[1], output);
}

bool CalculatorConversions::functionDynkinType(
  Calculator& calculator, const Expression& input, DynkinType& output
) {
  STACK_TRACE("CalculatorConversions::functionDynkinType");
  LinearCombination<Expression, Rational> typeComputer;
  if (
    !calculator.functionCollectSummandsCombine(
      calculator, input, typeComputer
    )
  ) {
    return false;
  }
  DynkinSimpleType simpleComponent;
  output.makeZero();
  for (int i = 0; i < typeComputer.size(); i ++) {
    if (
      !CalculatorConversions::functionDynkinSimpleType(
        calculator, typeComputer[i], simpleComponent
      )
    ) {
      return false;
    }
    int multiplicity = - 1;
    if (!typeComputer.coefficients[i].isSmallInteger(&multiplicity)) {
      multiplicity = - 1;
    }
    if (multiplicity < 0) {
      return
      calculator
      << "<hr>Failed to convert the coefficient "
      << typeComputer.coefficients[i]
      << " of "
      << typeComputer[i].toString()
      << " to a small positive integer. ";
    }
    output.addMonomial(simpleComponent, multiplicity);
  }
  return !typeComputer.isEqualToZero();
}

bool CalculatorConversions::functionSemisimpleLieAlgebraFromDynkinType(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  WithContext<SemisimpleLieAlgebra*>& outputPointer
) {
  STACK_TRACE(
    "CalculatorConversions::functionSemisimpleLieAlgebraFromDynkinType"
  );
  RecursionDepthCounter recursionCounter(&calculator.recursionDepth);
  DynkinType dynkinType;
  outputPointer.content = nullptr;
  outputPointer.context.initialize(calculator);
  if (
    !CalculatorConversions::functionDynkinType(calculator, input, dynkinType)
  ) {
    return
    calculator
    << "Failed to extract Dynkin type from: "
    << input.toString()
    << ". ";
  }
  if (dynkinType.getRank() > 20) {
    return
    calculator
    << "I have been instructed to allow "
    << "semisimple Lie algebras of rank 20 maximum. "
    << "If you would like to relax this limitation edit file "
    << __FILE__
    << " line "
    << __LINE__
    << ". Note that the Chevalley "
    << "constant computation Reserves a dim(g)*dim(g) "
    << "table of RAM memory, which means the RAM memory "
    << "rises with the 4^th power of rank(g). "
    << "You have been warned. Alternatively, you may "
    << "want to implement a sparse structure constant table "
    << "(write me an email if you want to do that, I will help you). ";
  }
  bool newlyCreated =
  !calculator.objectContainer.semisimpleLieAlgebras.contains(dynkinType);
  outputPointer.content =
  &calculator.objectContainer.getLieAlgebraCreateIfNotPresent(dynkinType);
  outputPointer.content->checkConsistency();
  output.assignValueWithContext(
    calculator, outputPointer.content, outputPointer.context
  );
  if (newlyCreated) {
    outputPointer.content->computeChevalleyConstants();
    Expression converter;
    CalculatorLieTheory::functionWriteToHardDiskOrPrintSemisimpleLieAlgebra(
      calculator, output, converter, false, false
    );
    calculator << converter.getValue<std::string>();
  }
  return true;
}

bool CalculatorConversions::expressionFromDynkinSimpleType(
  Calculator& calculator,
  const DynkinSimpleType& input,
  Expression& output
) {
  STACK_TRACE("CalculatorConversions::expressionFromDynkinSimpleType");
  Expression letterE, rankE, letterAndIndexE, indexE;
  std::string letterS;
  letterS = input.letter;
  letterE.makeAtom(
    calculator,
    calculator.addOperationNoRepetitionOrReturnIndexFirst(letterS)
  );
  indexE.assignValue(calculator, input.cartanSymmetricInverseScale);
  rankE.assignValue(calculator, input.rank);
  letterAndIndexE.makeXOX(
    calculator, calculator.opPower(), letterE, indexE
  );
  return
  output.makeXOX(
    calculator, calculator.opUnderscore(), letterAndIndexE, rankE
  );
}

bool CalculatorConversions::storeSemisimpleLieAlgebra(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (!input[1].isOfType<SemisimpleLieAlgebra*>()) {
    return
    output.assignError(
      calculator,
      "Asking to store non-semisimple Lie algebra as such is not allowed. "
    );
  }
  SemisimpleLieAlgebra * owner = input[1].getValueNonConst<SemisimpleLieAlgebra*>();
  return
  CalculatorConversions::expressionFromDynkinType(
    calculator, owner->weylGroup.dynkinType, output
  );
}

bool CalculatorConversions::expressionFromElementSemisimpleLieAlgebraRationals(
  Calculator& calculator,
  const ElementSemisimpleLieAlgebra<Rational>& input,
  Expression& output
) {
  STACK_TRACE(
    "CalculatorConversions::"
    "expressionFromElementSemisimpleLieAlgebraRationals"
  );
  LinearCombination<Expression, Rational> monomials;
  monomials.makeZero();
  Expression currentMon;
  for (int i = 0; i < input.size(); i ++) {
    CalculatorConversions::expressionFromChevalleyGenerator(
      calculator, input[i], currentMon
    );
    monomials.addMonomial(currentMon, input.coefficients[i]);
  }
  return output.makeSum(calculator, monomials);
}

bool CalculatorConversions::expressionFromDynkinType(
  Calculator& calculator, const DynkinType& input, Expression& output
) {
  STACK_TRACE("CalculatorConversions::expressionFromDynkinType");
  LinearCombination<Expression, AlgebraicNumber> monomials;
  monomials.makeZero();
  Expression currentMonomial;
  for (int i = 0; i < input.size(); i ++) {
    CalculatorConversions::expressionFromDynkinSimpleType(
      calculator, input[i], currentMonomial
    );
    monomials.addMonomial(currentMonomial, input.coefficients[i]);
  }
  return output.makeSum(calculator, monomials);
}

bool CalculatorConversions::
expressionFromElementSemisimpleLieAlgebraAlgebraicNumbers(
  Calculator& calculator,
  const ElementSemisimpleLieAlgebra<AlgebraicNumber>& input,
  Expression& output
) {
  STACK_TRACE(
    "CalculatorConversions::"
    "expressionFromElementSemisimpleLieAlgebraAlgebraicNumbers"
  );
  LinearCombination<Expression, AlgebraicNumber> monomials;
  monomials.makeZero();
  Expression currentMonomial;
  for (int i = 0; i < input.size(); i ++) {
    CalculatorConversions::expressionFromChevalleyGenerator(
      calculator, input[i], currentMonomial
    );
    input.coefficients[i].checkConsistency();
    monomials.addMonomial(currentMonomial, input.coefficients[i]);
  }
  return output.makeSum(calculator, monomials);
}

bool CalculatorConversions::slTwoSubalgebraPrecomputed(
  Calculator& calculator, const Expression& input, SlTwoSubalgebra& output
) {
  STACK_TRACE("CalculatorConversions::slTwoSubalgebraPrecomputed");
  if (!input.isListNElements(4)) {
    return
    calculator
    << "<hr>input of innerLoadFromObject has "
    << input.size()
    << " children, 4 expected. ";
  }
  const Expression& ownerExpression = input[1];
  Expression tempE;
  WithContext<SemisimpleLieAlgebra*> algebraWithContext;
  algebraWithContext.content = nullptr;
  if (
    !CalculatorConversions::functionSemisimpleLieAlgebraFromDynkinType(
      calculator, ownerExpression, tempE, algebraWithContext
    )
  ) {
    return
    calculator
    << "<hr>Failed to extract semisimple Lie algebra from "
    << ownerExpression.toString()
    << " while extracting its sl(2) subalgebra.";
  }
  output.owner = algebraWithContext.content;
  const Expression& expressionF = input[2];
  const Expression& expressionE = input[3];
  ElementSemisimpleLieAlgebra<Rational> elementF, elementE;
  if (
    !CalculatorConversions::loadElementSemisimpleLieAlgebraRationalCoefficients
    (calculator, expressionF, elementF, *output.owner)
  ) {
    return
    calculator
    << "<hr>Failed to extract f element while loading sl(2) subalgebra<hr>";
  }
  if (
    !CalculatorConversions::loadElementSemisimpleLieAlgebraRationalCoefficients
    (calculator, expressionE, elementE, *output.owner)
  ) {
    return
    calculator
    << "<hr>Failed to extract e element while loading sl(2) subalgebra<hr>";
  }
  if (elementE.isEqualToZero() || elementF.isEqualToZero()) {
    return
    calculator
    << "<hr>Failed to load sl(2) subalgebra: "
    << "either e or f is equal to zero. e and f are: "
    << elementE.toString()
    << ", "
    << elementF.toString()
    << ". ";
  }
  if (elementE.getOwner() != elementF.getOwner()) {
    return
    calculator
    << "<hr>Failed to load sl(2): E and F "
    << "element of sl(2) have different owners. "
    << "More precisely, the owner of e is "
    << elementE.getOwner()->toString()
    << " and the owner of f is "
    << elementF.getOwner()->toString();
  }
  output.eElement = elementE;
  output.fElement = elementF;
  output.owner = elementE.getOwner();
  DynkinType& dynkinType = output.owner->weylGroup.dynkinType;
  SemisimpleSubalgebras& ownerSubalgebras =
  calculator.objectContainer.getSemisimpleSubalgebrasCreateIfNotPresent(
    dynkinType
  );
  output.container = &ownerSubalgebras.slTwoSubalgebras;
  return true;
}

bool CalculatorConversions::slTwoSubalgebraPrecomputed(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorConversions::slTwoSubalgebraPrecomputed");
  SlTwoSubalgebra tempSL2;
  if (
    !CalculatorConversions::slTwoSubalgebraPrecomputed(
      calculator, input, tempSL2
    )
  ) {
    return calculator << "<hr>Failed to load sl(2) subalgebra. ";
  }
  return output.assignValue(calculator, tempSL2.toString());
}

bool CalculatorConversions::algebraicNumber(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorConversions::algebraicNumber");
  if (input.isOfType<AlgebraicNumber>()) {
    output = input;
    return true;
  }
  if (input.isOfType<Rational>()) {
    AlgebraicNumber number;
    number = input.getValue<Rational>();
    return output.assignValue(calculator, number);
  }
  return false;
}

bool CalculatorConversions::loadKeysFromStatementList(
  Calculator& calculator,
  const Expression& input,
  MapList<
    std::string,
    Expression,
    HashFunctions::hashFunction<std::string>
  >& output,
  bool allowFailure,
  List<Expression>* outputNotAKeyValuePair,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("CalculatorConversions::loadKeysFromStatementList");
  MapList<Expression, Expression> outputExpressionFormat;
  if (
    !CalculatorConversions::loadKeysFromStatementList(
      calculator,
      input,
      outputExpressionFormat,
      allowFailure,
      outputNotAKeyValuePair,
      commentsOnFailure
    )
  ) {
    return false;
  }
  output.clear();
  std::string keyName;
  for (int i = 0; i < outputExpressionFormat.keys.size; i ++) {
    if (
      !outputExpressionFormat.keys[i].isOfType<std::string>(&keyName)
    ) {
      keyName = outputExpressionFormat.keys[i].toString();
    }
    output.setKeyValue(keyName, outputExpressionFormat.values[i]);
  }
  return true;
}

bool CalculatorConversions::loadKeysFromStatementList(
  Calculator& calculator,
  const Expression& input,
  MapList<Expression, Expression>& output,
  bool allowFailure,
  List<Expression>* outputNotAKeyValuePair,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("CalculatorConversions::loadKeysFromStatementList");
  output.clear();
  for (int i = 1; i < input.size(); i ++) {
    if (input[i].startsWith(calculator.opDefine(), 3)) {
      output.setKeyValue(input[i][1], input[i][2]);
    } else if (allowFailure) {
      if (outputNotAKeyValuePair != nullptr) {
        outputNotAKeyValuePair->addOnTop(input[i]);
      }
    } else {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Could not extract key-value pair from: "
        << input.toString()
        << ": failed to process: "
        << input[i].toString();
      }
      return false;
    }
  }
  return true;
}

bool CalculatorConversions::loadKey(
  Calculator& calculator,
  const Expression& inputStatementList,
  const std::string& inputKey,
  Expression& output
) {
  STACK_TRACE("CalculatorConversions::loadKey");
  Expression keyExpression;
  keyExpression.makeAtom(calculator, inputKey);
  for (int i = 0; i < inputStatementList.size(); i ++) {
    if (inputStatementList[i].startsWith(calculator.opDefine(), 3)) {
      if (inputStatementList[i][1] == keyExpression) {
        output = inputStatementList[i][2];
        return true;
      }
    }
  }
  return
  calculator
  << "<hr>Key "
  << inputKey
  << " not found in expression "
  << inputStatementList.toString()
  << ".";
}

bool CalculatorConversions::storeCandidateSubalgebra(
  Calculator& calculator,
  const CandidateSemisimpleSubalgebra& input,
  Expression& output
) {
  STACK_TRACE("CalculatorConversions::storeCandidateSubalgebra");
  Expression currentE;
  List<std::string> keys;
  List<Expression> values;
  input.checkBasicInitialization();
  CalculatorConversions::expressionFromDynkinType(
    calculator, input.weylNonEmbedded->dynkinType, currentE
  );
  keys.addOnTop("DynkinType");
  values.addOnTop(currentE);
  Matrix<Rational> conversionMatrix;
  conversionMatrix.assignVectorsToRows(input.cartanElementsScaledToActByTwo);
  currentE.makeMatrix(calculator, conversionMatrix, nullptr, false);
  keys.addOnTop("ElementsCartan");
  values.addOnTop(currentE);
  input.checkAll();
  if (input.flagSystemSolved) {
    Expression listGenerators;
    listGenerators.makeSequence(calculator);
    for (int i = 0; i < input.negativeGenerators.size; i ++) {
      CalculatorConversions::
      expressionFromElementSemisimpleLieAlgebraAlgebraicNumbers(
        calculator, input.negativeGenerators[i], currentE
      );
      listGenerators.addChildOnTop(currentE);
      CalculatorConversions::
      expressionFromElementSemisimpleLieAlgebraAlgebraicNumbers(
        calculator, input.positiveGenerators[i], currentE
      );
      listGenerators.addChildOnTop(currentE);
    }
    keys.addOnTop("generators");
    values.addOnTop(listGenerators);
  }
  return output.makeSequenceCommands(calculator, keys, values);
}

bool CalculatorConversions::candidateSubalgebraPrecomputed(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  CandidateSemisimpleSubalgebra& outputSubalgebra,
  SemisimpleSubalgebras& owner
) {
  STACK_TRACE("CalculatorConversions::candidateSubalgebraPrecomputed");
  owner.checkInitialization();
  ProgressReport report;
  std::stringstream reportStream;
  reportStream << "Loading precomputed semisimple subalgebra. ";
  report.report(reportStream.str());
  Expression dynkinTypeE, ElementsCartanE, generatorsE;
  if (
    !CalculatorConversions::loadKey(
      calculator, input, "DynkinType", dynkinTypeE
    ) ||
    !CalculatorConversions::loadKey(
      calculator, input, "ElementsCartan", ElementsCartanE
    )
  ) {
    return false;
  }
  reportStream << "Extracted types: " << dynkinTypeE.toString() << ". ";
  report.report(reportStream.str());
  outputSubalgebra.owner = &owner;
  DynkinType nonEmbeddedDynkinType;
  if (
    !CalculatorConversions::functionDynkinType(
      calculator, dynkinTypeE, nonEmbeddedDynkinType
    )
  ) {
    return
    calculator
    << "<hr>Failed to load dynkin type of candidate subalgebra from "
    << dynkinTypeE.toString()
    << "<hr>";
  }
  reportStream
  << "Non embedded Dynkin type: "
  << dynkinTypeE.toString()
  << ". ";
  report.report(reportStream.str());
  outputSubalgebra.weylNonEmbedded =
  &calculator.objectContainer.getWeylGroupDataCreateIfNotPresent(
    nonEmbeddedDynkinType
  );
  outputSubalgebra.weylNonEmbedded->makeFromDynkinType(nonEmbeddedDynkinType);
  int rank = owner.owner->getRank();
  reportStream << "Extracting matrix of Cartan elements. ";
  report.report(reportStream.str());
  Matrix<Rational> hElements;
  if (
    !CalculatorConversions::functionGetMatrix(
      calculator,
      ElementsCartanE,
      hElements,
      false,
      nullptr,
      rank,
      nullptr
    )
  ) {
    return
    calculator
    << "<hr>Failed to load Cartan elements for candidate subalgebra of type "
    << outputSubalgebra.weylNonEmbedded->dynkinType
    << "<hr>";
  }
  if (
    hElements.numberOfRows != outputSubalgebra.weylNonEmbedded->getDimension()
  ) {
    return
    calculator
    << "<hr>Failed to load Cartan elements: I expected "
    << outputSubalgebra.weylNonEmbedded->getDimension()
    << " elements, but failed to get them.";
  }
  List<int> ranks, multiplicities;
  outputSubalgebra.weylNonEmbedded->dynkinType.getLettersTypesMultiplicities(
    nullptr, &ranks, &multiplicities, nullptr
  );
  outputSubalgebra.cartanSubalgebrasByComponentScaledToActByTwo.setSize(
    outputSubalgebra.weylNonEmbedded->dynkinType.getNumberOfSimpleComponents()
  );
  int componentCounter = - 1;
  int counter = - 1;
  for (int i = 0; i < multiplicities.size; i ++) {
    for (int j = 0; j < multiplicities[i]; j ++) {
      componentCounter ++;
      Vectors<Rational>& currentComponent =
      outputSubalgebra.cartanSubalgebrasByComponentScaledToActByTwo[
        componentCounter
      ];
      currentComponent.setSize(ranks[i]);
      for (int k = 0; k < ranks[i]; k ++) {
        counter ++;
        hElements.getVectorFromRow(counter, currentComponent[k]);
      }
    }
  }
  reportStream << "Extracting generators ... ";
  report.report(reportStream.str());
  outputSubalgebra.positiveGenerators.setSize(0);
  outputSubalgebra.negativeGenerators.setSize(0);
  if (
    CalculatorConversions::loadKey(
      calculator, input, "generators", generatorsE
    )
  ) {
    generatorsE.sequencefy();
    ElementSemisimpleLieAlgebra<AlgebraicNumber> currentGeneratorAlgebraic;
    for (int i = 1; i < generatorsE.size(); i ++) {
      if (
        !CalculatorConversions::loadElementSemisimpleLieAlgebraAlgebraicNumbers
        (
          calculator,
          generatorsE[i],
          currentGeneratorAlgebraic,
          *owner.owner
        )
      ) {
        return
        calculator
        << "<hr>Failed to load semisimple Lie algebra element from expression "
        << generatorsE[i].toString()
        << ". ";
      }
      if (i % 2 == 1) {
        outputSubalgebra.negativeGenerators.addOnTop(
          currentGeneratorAlgebraic
        );
      } else {
        outputSubalgebra.positiveGenerators.addOnTop(
          currentGeneratorAlgebraic
        );
      }
    }
    outputSubalgebra.flagSystemProvedToHaveNoSolution = false;
    outputSubalgebra.flagSystemSolved = true;
  } else {
    return
    calculator
    << "<hr>Failed to extract subalgebra generators from expression "
    << input.toString()
    << ". ";
  }
  SemisimpleLieAlgebra& currentNonEmbededSubalgebra =
  owner.subalgebrasNonEmbedded->getValueCreateNoInitialization(
    outputSubalgebra.weylNonEmbedded->dynkinType
  );
  outputSubalgebra.indexNonEmbeddedMeStandard =
  owner.subalgebrasNonEmbedded->getIndex(
    outputSubalgebra.weylNonEmbedded->dynkinType
  );
  currentNonEmbededSubalgebra.weylGroup.computeRho(true);
  outputSubalgebra.weylNonEmbedded->computeRho(true);
  // <- this line may be unnecessary, the
  // two Weyl groups may coincide depending on some implementation decisions I
  // am about to take
  // some time soon.
  outputSubalgebra.computeHsAndHsScaledToActByTwoFromComponents();
  outputSubalgebra.flagSubalgebraPreloadedButNotVerified = true;
  return
  output.assignError(
    calculator,
    "Candidate subalgebra is not a stand-alone "
    "object and its Expression output should not be used. "
  );
}

bool CalculatorConversions::loadSemisimpleSubalgebras(
  Calculator& calculator, const Expression& inpuT, Expression& output
) {
  STACK_TRACE("CalculatorConversions::loadSemisimpleSubalgebras");
  Expression input = inpuT;
  Expression
  ambientTypeE,
  numExploredHsE,
  numExploredTypesE,
  subalgebrasE,
  currentChainE;
  if (
    !CalculatorConversions::loadKey(
      calculator, input, "AmbientDynkinType", ambientTypeE
    )
  ) {
    return
    calculator
    << "<hr>Failed to load Dynkin type from: "
    << input.toString();
  }
  if (
    !CalculatorConversions::loadKey(
      calculator, input, "NumExploredHs", numExploredHsE
    )
  ) {
    return
    calculator
    << "<hr>Failed to load numExploredHs list from: "
    << input.toString();
  }
  if (
    !CalculatorConversions::loadKey(
      calculator, input, "NumExploredTypes", numExploredTypesE
    )
  ) {
    return
    calculator
    << "<hr>Failed to load NumExploredTypes list from: "
    << input.toString();
  }
  if (
    !CalculatorConversions::loadKey(
      calculator, input, "Subalgebras", subalgebrasE
    )
  ) {
    return
    calculator
    << "<hr>Failed to load Subalgebras list from: "
    << input.toString();
  }
  if (
    !CalculatorConversions::loadKey(
      calculator, input, "CurrentChain", currentChainE
    )
  ) {
    return
    calculator
    << "<hr>Failed to load CurrentChain from: "
    << input.toString();
  }
  List<int> currentChainInt, numExploredTypes, numExploredHs;
  if (!calculator.getVectorInt(currentChainE, currentChainInt)) {
    return false;
  }
  if (!calculator.getVectorInt(numExploredHsE, numExploredHs)) {
    return false;
  }
  if (!calculator.getVectorInt(numExploredTypesE, numExploredTypes)) {
    return false;
  }
  WithContext<SemisimpleLieAlgebra*> ownerSemisimple;
  ownerSemisimple.content = nullptr;
  Expression tempE;
  ProgressReport report;
  std::stringstream reportStream;
  reportStream << "Extracting semisimple Lie algebra ... ";
  report.report(reportStream.str());
  if (
    !CalculatorConversions::functionSemisimpleLieAlgebraFromDynkinType(
      calculator, ambientTypeE, tempE, ownerSemisimple
    )
  ) {
    return
    calculator
    << "<hr>Error loading semisimple subalgebras: "
    << "failed to extract ambient semisimple Lie algebra. ";
  }
  reportStream
  << " type: "
  << ownerSemisimple.content->weylGroup.dynkinType.toString()
  << ". ";
  report.report(reportStream.str());
  SemisimpleSubalgebras& subalgebras =
  calculator.objectContainer.getSemisimpleSubalgebrasCreateIfNotPresent(
    ownerSemisimple.content->weylGroup.dynkinType
  );
  subalgebras.subalgebrasNonEmbedded =
  &calculator.objectContainer.semisimpleLieAlgebras;
  subalgebras.owner = ownerSemisimple.content;
  reportStream << " Initializing data structures... ";
  report.report(reportStream.str());
  reportStream << " done. Fetching subalgebra list ... ";
  report.report(reportStream.str());
  subalgebrasE.sequencefy();
  subalgebras.subalgebras.setExpectedSize(subalgebrasE.size() - 1);
  subalgebras.subalgebras.clear();
  subalgebras.subalgebrasNonEmbedded->setExpectedSize(
    subalgebrasE.size() - 1
  );
  subalgebras.flagAttemptToSolveSystems = true;
  subalgebras.flagComputeModuleDecomposition = true;
  subalgebras.flagComputePairingTable = false;
  subalgebras.flagComputeNilradicals = false;
  subalgebras.millisecondsComputationStart = global.getElapsedMilliseconds();
  reportStream
  << " done. <br>Total subalgebras: "
  << subalgebrasE.size() - 1
  << ". ";
  report.report(reportStream.str());
  for (int i = 1; i < subalgebrasE.size(); i ++) {
    std::stringstream reportStream2;
    reportStream2
    << reportStream.str()
    << "Subalgebra "
    << i
    << " is being loaded from expression "
    << subalgebrasE[i].toString()
    << ".";
    report.report(reportStream2.str());
    CandidateSemisimpleSubalgebra currentCandidate;
    if (
      !CalculatorConversions::candidateSubalgebraPrecomputed(
        calculator,
        subalgebrasE[i],
        tempE,
        currentCandidate,
        subalgebras
      )
    ) {
      return
      calculator
      << "<hr>Error loading candidate subalgebra: "
      << "failed to load candidate number "
      << i
      << " extracted from expression: "
      << subalgebrasE[i].toString()
      << ". <hr>";
    }
    currentCandidate.checkFullInitialization();
    if (
      subalgebras.subalgebras.contains(
        currentCandidate.cartanElementsSubalgebra
      )
    ) {
      calculator
      << "<hr>Did not load subalgebra of type "
      << currentCandidate.weylNonEmbedded->toString()
      << " because I've already loaded a subalgebra with "
      << "the same Cartan subalgebra. ";
      continue;
    }
    subalgebras.subalgebras.setKeyValue(
      currentCandidate.cartanElementsSubalgebra, currentCandidate
    );
    subalgebras.subalgebras.values.lastObject().indexInOwner =
    subalgebras.subalgebras.values.size - 1;
  }
  reportStream
  << "Subalgebra loading done, total "
  << subalgebras.subalgebras.values.size
  << " subalgebras loaded. ";
  report.report(reportStream.str());
  subalgebras.toStringExpressionString =
  CalculatorConversions::stringFromSemisimpleSubalgebras;
  if (
    !subalgebras.loadState(
      currentChainInt,
      numExploredTypes,
      numExploredHs,
      calculator.comments
    )
  ) {
    return false;
  }
  subalgebras.flagAttemptToAdjustCentralizers = false;
  if (!subalgebras.findSemisimpleSubalgebrasContinue()) {
    std::stringstream out;
    out
    << "<br>Failed to realize all subalgebras, "
    << "computation aborted. The failure report follows. "
    << subalgebras.comments
    << "<br>The progress report for the "
    << "entire computation follows.<br>"
    << subalgebras.toStringProgressReport();
    return output.assignValue(calculator, out.str());
  }
  subalgebras.millisecondsComputationEnd = global.getElapsedMilliseconds();
  return output.assignValue(calculator, subalgebras);
}

std::string CalculatorConversions::stringFromSemisimpleSubalgebras(
  SemisimpleSubalgebras& input
) {
  STACK_TRACE("CalculatorConversions::stringFromSemisimpleSubalgebras");
  Expression expression;
  FormatExpressions format;
  CalculatorConversions::storeSemisimpleSubalgebras(
    global.calculator().getElement(), input, expression
  );
  format.flagUseHTML = true;
  return expression.toString(&format);
}

bool CalculatorConversions::storeSemisimpleSubalgebras(
  Calculator& calculator,
  const SemisimpleSubalgebras& input,
  Expression& output
) {
  STACK_TRACE("CalculatorConversions::storeSemisimpleSubalgebras");
  Expression dynkinTypeE;
  List<std::string> keys;
  List<Expression> values;
  if (
    !CalculatorConversions::expressionFromDynkinType(
      calculator, input.owner->weylGroup.dynkinType, dynkinTypeE
    )
  ) {
    return false;
  }
  keys.addOnTop("AmbientDynkinType");
  values.addOnTop(dynkinTypeE);
  Expression currentChainE, numericalConvertorE(calculator);
  currentChainE.makeSequence(calculator);
  for (int i = 0; i < input.currentSubalgebraChain.size; i ++) {
    numericalConvertorE = input.currentSubalgebraChain[i].indexInOwner;
    currentChainE.addChildOnTop(numericalConvertorE);
  }
  keys.addOnTop("CurrentChain");
  values.addOnTop(currentChainE);
  Expression numTypesExploredE;
  numTypesExploredE.makeSequence(calculator);
  for (
    int i = 0; i < input.currentNumberOfLargerTypesExplored.size; i ++
  ) {
    numericalConvertorE = input.currentNumberOfLargerTypesExplored[i];
    numTypesExploredE.addChildOnTop(numericalConvertorE);
  }
  keys.addOnTop("NumExploredTypes");
  values.addOnTop(numTypesExploredE);
  Expression numHsExploredE;
  numHsExploredE.makeSequence(calculator);
  for (
    int i = 0; i < input.currentNumberOfHCandidatesExplored.size; i ++
  ) {
    numericalConvertorE = input.currentNumberOfHCandidatesExplored[i];
    numHsExploredE.addChildOnTop(numericalConvertorE);
  }
  keys.addOnTop("NumExploredHs");
  values.addOnTop(numHsExploredE);
  Expression subalgebrasListE, candidateE;
  subalgebrasListE.makeSequence(calculator);
  subalgebrasListE.setExpectedSize(input.subalgebras.values.size + 1);
  input.checkAll();
  for (int i = 0; i < input.subalgebras.values.size; i ++) {
    if (
      !CalculatorConversions::storeCandidateSubalgebra(
        calculator, input.subalgebras.values[i], candidateE
      )
    ) {
      return false;
    }
    subalgebrasListE.addChildOnTop(candidateE);
  }
  keys.addOnTop("Subalgebras");
  values.addOnTop(subalgebrasListE);
  return output.makeSequenceCommands(calculator, keys, values);
}

bool CalculatorConversions::expressionFromMonomialUniversalEnveloping(
  Calculator& calculator,
  const MonomialUniversalEnveloping<RationalFraction<Rational> >& input,
  Expression& output,
  ExpressionContext* inputContext
) {
  STACK_TRACE(
    "CalculatorConversions::expressionFromMonomialUniversalEnveloping"
  );
  if (input.isConstant()) {
    return output.assignValue(calculator, 1);
  }
  ChevalleyGenerator chevalleyGenerator;
  chevalleyGenerator.owner = input.owner;
  Expression chevalleyExpression, powerE, termE;
  List<Expression> terms;
  for (int i = 0; i < input.generatorsIndices.size; i ++) {
    chevalleyGenerator.generatorIndex = input.generatorsIndices[i];
    CalculatorConversions::expressionFromChevalleyGenerator(
      calculator, chevalleyGenerator, chevalleyExpression
    );
    CalculatorConversions::expressionFromRationalFraction<Rational>(
      calculator, input.powers[i], powerE, inputContext
    );
    termE.makeXOX(
      calculator, calculator.opPower(), chevalleyExpression, powerE
    );
    terms.addOnTop(termE);
  }
  return output.makeProduct(calculator, terms);
}

bool CalculatorConversions::expressionFromElementUniversalEnveloping(
  Calculator& calculator,
  const ElementUniversalEnveloping<RationalFraction<Rational> >& input,
  Expression& output,
  ExpressionContext* inputContext
) {
  STACK_TRACE(
    "CalculatorConversions::expressionFromElementUniversalEnveloping"
  );
  LinearCombination<Expression, RationalFraction<Rational> >
  elementUniversalEnveloping;
  elementUniversalEnveloping.makeZero();
  Expression currentMonE;
  for (int i = 0; i < input.size(); i ++) {
    if (
      !CalculatorConversions::expressionFromMonomialUniversalEnveloping(
        calculator, input[i], currentMonE, inputContext
      )
    ) {
      return calculator << "<hr>Failed to store " << input.toString();
    }
    elementUniversalEnveloping.addMonomial(
      currentMonE, input.coefficients[i]
    );
  }
  return output.makeSum(calculator, elementUniversalEnveloping);
}

bool CalculatorConversions::loadElementSemisimpleLieAlgebraRationalCoefficients
(
  Calculator& calculator,
  const Expression& input,
  ElementSemisimpleLieAlgebra<Rational>& output,
  SemisimpleLieAlgebra& owner
) {
  STACK_TRACE(
    "CalculatorConversions::"
    "loadElementSemisimpleLieAlgebraRationalCoefficients"
  );
  ElementSemisimpleLieAlgebra<AlgebraicNumber> outputAlgebraic;
  if (
    !CalculatorConversions::loadElementSemisimpleLieAlgebraAlgebraicNumbers(
      calculator, input, outputAlgebraic, owner
    )
  ) {
    return
    calculator
    << "<hr> Failed to obtain semisimple Lie algebra element from "
    << input.toString()
    << ". ";
  }
  for (int i = 0; i < outputAlgebraic.coefficients.size; i ++) {
    if (!outputAlgebraic.coefficients[i].isRational()) {
      return
      calculator
      << "<hr>From input: "
      << input.toString()
      << ", I managed to extract element: "
      << outputAlgebraic.toString()
      << " but that appears to not have rational coefficients.";
    }
  }
  output = outputAlgebraic;
  // <-implicit conversion here!
  return true;
}

bool CalculatorConversions::loadElementSemisimpleLieAlgebraAlgebraicNumbers(
  Calculator& calculator,
  const Expression& input,
  ElementSemisimpleLieAlgebra<AlgebraicNumber>& output,
  SemisimpleLieAlgebra& owner
) {
  STACK_TRACE(
    "CalculatorConversions::loadElementSemisimpleLieAlgebraAlgebraicNumbers"
  );
  WithContext<Polynomial<AlgebraicNumber> > polynomialFormWithContext;
  if (
    !CalculatorConversions::functionPolynomial<AlgebraicNumber>(
      calculator, input, polynomialFormWithContext, 300, 2, false
    )
  ) {
    return
    calculator
    << "<hr>Failed to convert "
    << input.toString()
    << " to polynomial.<hr>";
  }
  Polynomial<AlgebraicNumber> polynomialForm =
  polynomialFormWithContext.content;
  ChevalleyGenerator chevalleyGenerator;
  ElementSemisimpleLieAlgebra<AlgebraicNumber> currentElement;
  chevalleyGenerator.owner = &owner;
  output.makeZero();
  ExpressionContext context = polynomialFormWithContext.context;
  for (int j = 0; j < polynomialForm.size(); j ++) {
    const MonomialPolynomial& currentMon = polynomialForm[j];
    int chevalleyGeneratorIndex = 0;
    if (!currentMon.isOneLetterFirstDegree(&chevalleyGeneratorIndex)) {
      return
      calculator
      << "<hr>Failed to convert semisimple Lie algebra input to linear poly: "
      << input.toString()
      << ".<hr>";
    }
    Expression chevalleyGeneratorExpression =
    context.getVariable(chevalleyGeneratorIndex);
    if (
      !chevalleyGeneratorExpression.startsWith(
        calculator.opUnderscore(), 3
      )
    ) {
      return
      calculator
      << "<hr>Failed to convert: "
      << chevalleyGeneratorExpression.toString()
      << "(summand of: "
      << input.toString()
      << ") to Chevalley generator.<hr>";
    }
    std::string letter;
    if (
      !chevalleyGeneratorExpression[1].isOperation(&letter) ||
      !chevalleyGeneratorExpression[2].isSmallInteger(
        &chevalleyGenerator.generatorIndex
      )
    ) {
      return
      calculator
      << "<hr>Failed to convert summand "
      << chevalleyGeneratorExpression.toString()
      << " to Chevalley generator of "
      << owner.toStringLieAlgebraName();
    }
    bool isGood = true;
    if (letter == "g") {
      chevalleyGenerator.generatorIndex =
      owner.getGeneratorFromDisplayIndex(chevalleyGenerator.generatorIndex);
      if (
        chevalleyGenerator.generatorIndex < 0 ||
        chevalleyGenerator.generatorIndex >= owner.getNumberOfGenerators()
      ) {
        isGood = false;
      }
      output.addMonomial(
        chevalleyGenerator, polynomialForm.coefficients[j]
      );
    } else if (letter == "h") {
      int rootIndex =
      owner.getRootIndexFromDisplayIndex(chevalleyGenerator.generatorIndex);
      if (rootIndex < 0) {
        isGood = false;
      } else {
        currentElement.makeCartanGenerator(
          owner.weylGroup.rootSystem[rootIndex], owner
        );
        currentElement *= polynomialForm.coefficients[j];
        output += currentElement;
      }
    } else {
      isGood = false;
    }
    if (!isGood) {
      return
      calculator
      << "<hr>Failed to convert summand "
      << chevalleyGeneratorExpression.toString()
      << " to Chevalley generator of "
      << owner.toStringLieAlgebraName();
    }
  }
  return true;
}

bool CalculatorConversions::elementUniversalEnveloping(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  SemisimpleLieAlgebra& owner
) {
  STACK_TRACE("CalculatorConversions::elementUniversalEnveloping");
  if (input.size() != 2) {
    return
    calculator
    << "Universal enveloping algebra element expects a single argument. ";
  }
  ChevalleyGenerator chevalleyGenerator;
  chevalleyGenerator.owner = &owner;
  ElementUniversalEnveloping<RationalFraction<Rational> > outputUE;
  ElementUniversalEnveloping<RationalFraction<Rational> > currentSummand;
  ElementUniversalEnveloping<RationalFraction<Rational> > currentMultiplicand;
  MonomialPolynomial currentMultiplicandRFpartMon;
  Polynomial<Rational> currentPMultiplicand;
  RationalFraction<Rational> currentMultiplicandRFpart;
  outputUE.makeZero(owner);
  WithContext<Polynomial<Rational> > polynomial;
  if (
    !CalculatorConversions::functionPolynomial<Rational>(
      calculator, input[1], polynomial, - 1, - 1, false
    )
  ) {
    return
    calculator
    << "<hr>Failed to convert "
    << input[1].toString()
    << " to polynomial.<hr>";
  }
  ExpressionContext context = polynomial.context;
  HashedList<Expression> polynomialVariables;
  for (int j = 0; j < polynomial.content.size(); j ++) {
    const MonomialPolynomial& currentMon = polynomial.content[j];
    currentSummand.makeConstant(
      polynomial.content.coefficients[j], owner
    );
    currentMultiplicandRFpartMon.makeOne();
    for (int i = 0; i < currentMon.minimalNumberOfVariables(); i ++) {
      int power = - 1;
      if (!currentMon(i).isSmallInteger(&power)) {
        return
        calculator
        << "<hr>Failed to convert one of the exponents appearing in "
        << input[1].toString()
        << " to  a small integer polynomial.<hr>";
      }
      if (power == 0) {
        continue;
      }
      Expression singleChevGenE = context.getVariable(i);
      if (!singleChevGenE.isListNElements(2)) {
        return
        calculator
        << "<hr>Failed to convert "
        << input[1].toString()
        << " to polynomial.<hr>";
      }
      std::string letter;
      if (
        !singleChevGenE[0].isOperation(&letter) ||
        !singleChevGenE[1].isSmallInteger(
          &chevalleyGenerator.generatorIndex
        )
      ) {
        return
        calculator
        << "<hr>Failed to convert summand "
        << singleChevGenE.toString()
        << " to Chevalley generator of "
        << owner.toStringLieAlgebraName();
      }
      bool isGood = true;
      bool isHonestElementUE = true;
      if (letter == "g") {
        chevalleyGenerator.generatorIndex =
        owner.getGeneratorFromDisplayIndex(chevalleyGenerator.generatorIndex);
        if (
          chevalleyGenerator.generatorIndex < 0 ||
          chevalleyGenerator.generatorIndex >= owner.getNumberOfGenerators()
        ) {
          isGood = false;
        }
      } else if (letter == "h") {
        if (
          chevalleyGenerator.generatorIndex < 1 ||
          chevalleyGenerator.generatorIndex > owner.getRank()
        ) {
          isGood = false;
        } else {
          chevalleyGenerator.generatorIndex =
          chevalleyGenerator.generatorIndex +
          owner.getNumberOfPositiveRoots() - 1;
        }
      } else {
        isHonestElementUE = false;
      }
      if (!isGood) {
        return
        calculator
        << "<hr>Failed to convert summand "
        << singleChevGenE.toString()
        << " to Chevalley generator of "
        << owner.toStringLieAlgebraName();
      }
      if (isHonestElementUE) {
        currentMultiplicand.makeOneGenerator(
          chevalleyGenerator.generatorIndex, owner, Rational::one()
        );
        currentMultiplicand.raiseToPower(power);
        currentSummand *= currentMultiplicand;
      } else {
        int variableIndex =
        polynomialVariables.addNoRepetitionOrReturnIndexFirst(singleChevGenE);
        currentMultiplicandRFpartMon.setVariable(variableIndex, power);
      }
    }
    currentPMultiplicand.makeZero();
    currentPMultiplicand.addMonomial(currentMultiplicandRFpartMon, 1);
    currentMultiplicandRFpart = currentPMultiplicand;
    currentSummand *= currentMultiplicandRFpart;
    outputUE += currentSummand;
  }
  ExpressionContext outputContext(calculator);
  outputContext.setVariables(polynomialVariables);
  outputContext.setAmbientSemisimpleLieAlgebra(owner);
  return output.assignValueWithContext(calculator, outputUE, outputContext);
}

bool CalculatorConversions::expressionFromBuiltInType(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (input.size() != 2) {
    return false;
  }
  return
  CalculatorConversions::functionExpressionFromBuiltInType(
    calculator, input[1], output
  );
}

bool CalculatorConversions::functionExpressionFromBuiltInType(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorConversions::functionExpressionFromBuiltInType");
  if (
    input.isOfType<Rational>() ||
    input.isOfType<double>() ||
    input.isOfType<AlgebraicNumber>()
  ) {
    output = input;
    return true;
  }
  if (input.isOfType<Polynomial<Rational> >()) {
    return
    CalculatorConversions::functionExpressionFromPolynomial<Rational>(
      calculator, input, output
    );
  }
  if (input.isOfType<Polynomial<AlgebraicNumber> >()) {
    return
    CalculatorConversions::functionExpressionFromPolynomial<AlgebraicNumber>(
      calculator, input, output
    );
  }
  if (input.isOfType<RationalFraction<Rational> >()) {
    return
    CalculatorConversions::expressionFromRationalFraction<Rational>(
      calculator, input, output
    );
  }
  if (input.isOfType<RationalFraction<AlgebraicNumber> >()) {
    return
    CalculatorConversions::expressionFromRationalFraction<AlgebraicNumber>(
      calculator, input, output
    );
  }
  return false;
}

bool CalculatorConversions::expressionFromElementUniversalEnveloping(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorConversions::expressionFromElementUniversalEnveloping"
  );
  ElementUniversalEnveloping<RationalFraction<Rational> >
  elementUniversalEnveloping;
  if (
    !input.isOfType<
      ElementUniversalEnveloping<RationalFraction<Rational> >
    >(&elementUniversalEnveloping)
  ) {
    return
    calculator
    << "<hr>Expression "
    << input.toString()
    << " is not an element of universal "
    << "enveloping, can't convert to expression";
  }
  return
  CalculatorConversions::expressionFromElementUniversalEnveloping(
    calculator, elementUniversalEnveloping, output
  );
}

bool CalculatorConversions::rationalFunction(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorConversions::rationalFunction");
  if (input.size() != 2) {
    return false;
  }
  WithContext<RationalFraction<Rational> > outputCandidate;
  if (
    !CalculatorConversions::functionRationalFraction(
      calculator, input[1], outputCandidate, true
    )
  ) {
    return false;
  }
  return output.assignWithContext(calculator, outputCandidate);
}

bool CalculatorConversions::functionMatrixRational(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorConversions::functionMatrixRational");
  Matrix<Rational> outputMatrix;
  if (
    !CalculatorConversions::functionGetMatrix(
      calculator, input, outputMatrix, false, nullptr, - 1, nullptr
    )
  ) {
    return calculator << "<br>Failed to get matrix of rationals. ";
  }
  return output.makeMatrix(calculator, outputMatrix);
}

bool CalculatorConversions::matrixRationalTensorForm(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorConversions::matrixRationalTensorForm");
  if (!CalculatorConversions::makeMatrix(calculator, input, output)) {
    return calculator << "Failed to extract matrix of rationals. ";
  }
  Matrix<Rational> matrixRational;
  if (
    !CalculatorConversions::functionGetMatrixNoComputation(
      calculator, output, matrixRational
    )
  ) {
    return false;
  }
  MatrixTensor<Rational> outputMatrixTensor;
  outputMatrixTensor = matrixRational;
  return output.assignValue(calculator, outputMatrixTensor);
}

bool CalculatorConversions::functionMatrixRationalTensorForm(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorConversions::functionMatrixRationalTensorForm");
  Matrix<Rational> matrixRational;
  if (
    !CalculatorConversions::functionGetMatrixNoComputation(
      calculator, input, matrixRational
    )
  ) {
    return false;
  }
  MatrixTensor<Rational> outputMatrixTensor;
  outputMatrixTensor = matrixRational;
  return output.assignValue(calculator, outputMatrixTensor);
}

bool CalculatorConversions::outerMatrixExpressionsToMatrixOfType(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorConversions::outerMatrixExpressionsToMatrixOfType");
  if (!input.isMatrix()) {
    return false;
  }
  if (input.size() < 1) {
    return false;
  }
  if (input[0].size() != 1) {
    return false;
  }
  if (input[0][0].data != calculator.opMatrix()) {
    return false;
  }
  Matrix<Expression> matrix;
  if (!calculator.getMatrixExpressionsFromArguments(input, matrix)) {
    return false;
  }
  Expression conversionAttempt;
  conversionAttempt.assignMatrixExpressions(matrix, calculator, true, false);
  if (!conversionAttempt.isMatrix()) {
    output = conversionAttempt;
    return true;
  }
  if (conversionAttempt.size() >= 1) {
    if (conversionAttempt[0].startsWith(calculator.opMatrix())) {
      if (conversionAttempt[0].size() > 1) {
        output = conversionAttempt;
        return true;
      }
    }
  }
  return false;
}

bool CalculatorConversions::makeMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorConversions::makeMatrix");
  Matrix<Expression> outputMatrix;
  if (calculator.getMatrixExpressionsFromArguments(input, outputMatrix)) {
    return
    output.assignMatrixExpressions(outputMatrix, calculator, true, true);
  }
  return false;
}

bool CalculatorConversions::functionMatrixAlgebraic(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorConversions::functionMatrixAlgebraic");
  Matrix<AlgebraicNumber> outputMatrix;
  if (
    !CalculatorConversions::functionGetMatrixNoComputation(
      calculator, input, outputMatrix
    )
  ) {
    return false;
  }
  return output.makeMatrix(calculator, outputMatrix);
}

bool CalculatorConversions::matrixRationalFunction(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorConversions::matrixRationalFunction");
  return
  CalculatorConversions::functionMatrixRationalFunction(
    calculator, input, output
  );
}

bool CalculatorConversions::functionMatrixRationalFunction(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorConversions::functionMatrixRationalFunction");
  Matrix<RationalFraction<Rational> > outputMatrix;
  ExpressionContext context(calculator);
  if (
    !CalculatorConversions::functionGetMatrix(
      calculator,
      input,
      outputMatrix,
      true,
      &context,
      - 1,
      &calculator.comments
    )
  ) {
    return calculator << "<hr>Failed to get matrix of rational functions. ";
  }
  output.makeMatrix(calculator, outputMatrix, &context);
  output.checkConsistency();
  return true;
}

bool CalculatorConversions::loadFileIntoString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorConversions::loadFileIntoString");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  std::string relativeFileName;
  if (!argument.isOfType<std::string>(&relativeFileName)) {
    calculator
    << "Input of load file string command is supposed to be a string. "
    << "Converting your expression to a string and using that instead. ";
    relativeFileName = argument.toString();
  }
  std::string outputString;
  if (
    !FileOperations::loadFileToStringVirtual(
      relativeFileName, outputString, false, &calculator.comments
    )
  ) {
    return false;
  }
  return output.assignValue(calculator, outputString);
}

bool CalculatorConversions::makeElementHyperOctahedral(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorConversions::makeElementHyperOctahedral");
  std::string inputStringFormat;
  ElementHyperoctahedralGroupR2 element;
  if (input.isOfType<std::string>(&inputStringFormat)) {
    element.makeFromString(inputStringFormat);
    return output.assignValue(calculator, element);
  }
  if (input.size() < 3) {
    return
    calculator
    << "To make elements of hyperoctahedral group we need at least 3 inputs. ";
  }
  List<int> oneCycle;
  if (!calculator.getVectorInt(input[1], oneCycle)) {
    return
    calculator
    << "Failed to extract a cycle structure from the first argument of input: "
    << input.toString();
  }
  for (int i = 0; i < oneCycle.size; i ++) {
    if (oneCycle[i] < 1) {
      return
      calculator
      << "Your input: "
      << input[1].toString()
      << " had integers that were too small. ";
    }
    oneCycle[i] --;
    if (oneCycle[i] > 1000) {
      return
      calculator
      << "Your input: "
      << input[1].toString()
      << " had integers that were too large. ";
    }
  }
  element.h.addCycle(oneCycle);
  for (int i = 2; i < input.size(); i ++) {
    if (input[i].isEqualToOne()) {
      element.k.toggleBit(i - 2);
    } else if (!input[i].isEqualToZero()) {
      return
      calculator
      << "Your input: "
      << input.toString()
      << " had bit values that were not ones and zeroes.";
    }
  }
  return output.assignValue(calculator, element);
}

bool CalculatorConversions::polynomialModuloInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorConversions::polynomialModuloInteger");
  if (input.size() != 3) {
    return
    calculator
    << "Expected two arguments, polynomial and positive integer.";
  }
  WithContext<Polynomial<Rational> > polynomial;
  if (
    !CalculatorConversions::convert(calculator, input[1], polynomial)
  ) {
    return false;
  }
  LargeInteger modulus;
  if (!input[2].isInteger(&modulus)) {
    return false;
  }
  if (modulus <= 0) {
    return
    calculator
    << "The modulus: "
    << modulus
    << " must be non-negative. ";
  }
  Polynomial<ElementZmodP> converted;
  ElementZmodP::convertModuloIntegerAfterScalingToIntegral(
    polynomial.content, converted, modulus.value
  );
  polynomial.context.setDefaultModulus(modulus.value);
  return
  output.assignValueWithContext(calculator, converted, polynomial.context);
}
