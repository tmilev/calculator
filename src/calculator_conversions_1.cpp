// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".#include "calculator_interface.h"
#include "math_extra_semisimple_lie_algebras_implementation.h"
#include "calculator_interface.h"
#include "calculator_inner_functions.h"
#include "calculator_inner_typed_functions.h"
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_rational_function_implementation.h"
#include "calculator_lie_theory.h"

template <>
bool Expression::convertInternally<RationalFraction<Rational> >(Expression& output) const;

bool CalculatorConversions::expressionFromChevalleyGenerator(
  Calculator& calculator, const ChevalleyGenerator& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::expressionFromChevalleyGenerator");
  input.checkInitialization();
  output.reset(calculator, 2);
  Expression generatorLetterE, generatorIndexE;
  if (
    input.generatorIndex >= input.owner->getNumberOfPositiveRoots() &&
    input.generatorIndex < input.owner->getNumberOfPositiveRoots() + input.owner->getRank()
  ) {
    generatorLetterE.makeAtom(calculator.addOperationNoRepetitionOrReturnIndexFirst("h"), calculator);
  } else {
    generatorLetterE.makeAtom(calculator.addOperationNoRepetitionOrReturnIndexFirst("g"), calculator);
  }
  generatorIndexE.assignValue(input.owner->getDisplayIndexFromGenerator(input.generatorIndex), calculator);
  return output.makeXOX(calculator, calculator.opUnderscore(), generatorLetterE, generatorIndexE);
}

bool CalculatorConversions::functionSemisimpleLieAlgebra(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::functionSemisimpleLieAlgebra");
  SemisimpleLieAlgebra* dummySubalgebra;
  return CalculatorConversions::functionSemisimpleLieAlgebra(
    calculator, input, output, dummySubalgebra
  );
}

bool CalculatorConversions::innerSemisimpleLieAlgebra(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerSemisimpleLieAlgebra");
  if (input.size() != 2) {
    return calculator << "Semisimple Lie algebra expects a single argument. ";
  }
  SemisimpleLieAlgebra* dummySubalgebra;
  return CalculatorConversions::functionSemisimpleLieAlgebra(
    calculator, input[1], output, dummySubalgebra
  );
}

bool CalculatorConversions::innerLoadWeylGroup(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerLoadWeylGroup");
  DynkinType dynkinType;
  if (!CalculatorConversions::dynkinType(calculator, input, dynkinType)) {
    return false;
  }
  SemisimpleLieAlgebra& subalgebra = calculator.objectContainer.getLieAlgebraCreateIfNotPresent(dynkinType);
  return output.assignValue(subalgebra.weylGroup, calculator);
}

bool CalculatorConversions::dynkinSimpleType(
  Calculator& calculator, const Expression& input, DynkinSimpleType& outputMon
) {
  MacroRegisterFunctionWithName("CalculatorConversions::dynkinSimpleType");
  if (input.size() != 2) {
    return calculator << "Dynkin simple type function requires a single argument. ";
  }
  return CalculatorConversions::functionDynkinSimpleType(calculator, input[1], outputMon);
}

bool CalculatorConversions::functionDynkinSimpleType(
  Calculator& calculator, const Expression& input, DynkinSimpleType& outputMon
) {
  MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::DeSerializeMon_DynkinSimpleType");
  Expression rankE, typeLetterE, scaleE;
  if (input.startsWith(calculator.opUnderscore(), 3)) {
    rankE = input[2];
    typeLetterE = input[1];
    if (typeLetterE.startsWith(calculator.opPower(),3)) {
      scaleE = typeLetterE[2];
      typeLetterE = typeLetterE[1];
    } else {
      scaleE.assignValue(1, calculator);
    }
  } else if (input.startsWith(calculator.opPower(), 3)) {
    scaleE = input[2];
    if (!input[1].startsWith(calculator.opUnderscore(), 3)) {
      return calculator << "<hr>Failed to extract rank, type from "
      << input[1].toString()
      << ". The expression does not have two children.";
    }
    rankE = input[1][2];
    typeLetterE = input[1][1];
  } else {
    return calculator << "<hr>Failed to extract rank, type, "
    << "first co-root length from: " << input.toString() << ". ";
  }
  Rational theScale;
  if (!scaleE.isOfType<Rational>(&theScale)) {
    return calculator << "<hr>Failed to extract first co-root length: "
    << "expression " << scaleE.toString()
    << " is not a rational number.";
  }
  if (theScale <= 0) {
    return calculator << "<hr>Couldn't extract first co-root length: "
    << theScale.toString() << " is non-positive.";
  }
  std::string typeName;
  if (!typeLetterE.isOperation(&typeName)) {
    return calculator << "I couldn't extract a type letter from " << typeLetterE.toString();
  }
  if (typeName.size() != 1) {
    return calculator << "<hr>Error while extracting Dynkin simple type: "
    << "the type of a simple Lie algebra must be the letter A, B, C, D, E, F or G. "
    << "Instead, it is " << typeName + ". Error encountered while processing " << input.toString();
  }
  char theWeylLetter = typeName[0];
  if (theWeylLetter == 'a') {
    theWeylLetter ='A';
  }
  if (theWeylLetter == 'b') {
    theWeylLetter ='B';
  }
  if (theWeylLetter == 'c') {
    theWeylLetter ='C';
  }
  if (theWeylLetter == 'd') {
    theWeylLetter ='D';
  }
  if (theWeylLetter == 'e') {
    theWeylLetter ='E';
  }
  if (theWeylLetter == 'f') {
    theWeylLetter ='F';
  }
  if (theWeylLetter == 'g') {
    theWeylLetter ='G';
  }
  if (!(
    theWeylLetter == 'A' ||
    theWeylLetter == 'B' ||
    theWeylLetter == 'C' ||
    theWeylLetter == 'D' ||
    theWeylLetter == 'E' ||
    theWeylLetter == 'F' ||
    theWeylLetter == 'G'
  )) {
    return calculator << "The type of a simple Lie algebra must be "
    << "the letter A, B, C, D, E, F or G; error while processing " << input.toString();
  }
  int rank;
  if (!rankE.isSmallInteger(&rank)) {
    return calculator << "I wasn't able to extract rank from " << input.toString();
  }
  if (rank < 1 || rank > 20) {
    return calculator << "<hr>The rank of a simple Lie algebra "
    << "must be between 1 and 20; error while processing " << input.toString();
  }
  if (theWeylLetter == 'E' && (rank > 8 || rank < 3)) {
    return calculator << "<hr>Type E must have rank 6, 7 or 8 ";
  }
  if (theWeylLetter == 'D' && (rank < 3)) {
    return calculator << "<hr>Type D is expected to have rank 4 or more, "
    << "your input was of rank " << rank << ". ";
  }
  outputMon.makeArbitrary(theWeylLetter, rank, theScale);
  return true;
}

bool CalculatorConversions::dynkinType(Calculator& calculator, const Expression& input, DynkinType& output) {
  MacroRegisterFunctionWithName("Calculator::innerDynkinTypE");
  RecursionDepthCounter recursionCounter(&calculator.recursionDepth);
  if (input.size() != 2) {
    return calculator << "Dynkin type takes as input one argument. ";
  }
  return CalculatorConversions::functionDynkinType(calculator, input[1], output);
}

bool CalculatorConversions::functionDynkinType(
  Calculator& calculator, const Expression& input, DynkinType& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::functionDynkinType");
  LinearCombination<Expression, Rational> typeComputer;
  if (!calculator.functionCollectSummandsCombine(calculator, input, typeComputer)) {
    return false;
  }
  DynkinSimpleType simpleComponent;
  output.makeZero();
  for (int i = 0; i < typeComputer.size(); i ++) {
    if (!CalculatorConversions::functionDynkinSimpleType(
      calculator, typeComputer[i], simpleComponent
    )) {
      return false;
    }
    int theMultiplicity = - 1;
    if (!typeComputer.coefficients[i].isSmallInteger(&theMultiplicity)) {
      theMultiplicity = - 1;
    }
    if (theMultiplicity < 0) {
      return calculator << "<hr>Failed to convert the coefficient "
      << typeComputer.coefficients[i] << " of " << typeComputer[i].toString()
      << " to a small positive integer. ";
    }
    output.addMonomial(simpleComponent, theMultiplicity);
  }
  return !typeComputer.isEqualToZero();
}

bool CalculatorConversions::functionSemisimpleLieAlgebra(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  SemisimpleLieAlgebra*& outputPointer
) {
  MacroRegisterFunctionWithName("Calculator::functionSemisimpleLieAlgebra");
  RecursionDepthCounter recursionCounter(&calculator.recursionDepth);
  DynkinType dynkinType;
  outputPointer = nullptr;
  if (!CalculatorConversions::functionDynkinType(
    calculator, input, dynkinType
  )) {
    return calculator << "Failed to extract Dynkin type from: " << input.toString();
  }
  if (dynkinType.getRank() > 20) {
    return calculator
    << "I have been instructed to allow semisimple Lie algebras of rank 20 maximum. "
    << "If you would like to relax this limitation edit file " << __FILE__
    << " line " << __LINE__ << ". Note that the Chevalley "
    << "constant computation Reserves a dim(g)*dim(g) "
    << "table of RAM memory, which means the RAM memory rises with the 4^th power of rank(g). "
    << "You have been warned. Alternatively, you may want to implement a sparse structure constant table "
    << "(write me an email if you want to do that, I will help you). ";
  }
  bool newlyCreated = !calculator.objectContainer.semisimpleLieAlgebras.contains(dynkinType);
  outputPointer = &calculator.objectContainer.getLieAlgebraCreateIfNotPresent(dynkinType);
  outputPointer->checkConsistency();
  output.assignValue(outputPointer, calculator);
  if (newlyCreated) {
    outputPointer->computeChevalleyConstants();
    Expression converter;
    CalculatorLieTheory::functionWriteToHardDiskOrPrintSemisimpleLieAlgebra(
      calculator, output, converter, false, false
    );
    calculator << converter.getValue<std::string>();
  }
  return true;
}

bool CalculatorConversions::innerExpressionFromDynkinSimpleType(
  Calculator& calculator, const DynkinSimpleType& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromDynkinSimpleType");
  Expression letterE, rankE, letterAndIndexE, indexE;
  std::string letterS;
  letterS = input.letter;
  letterE.makeAtom(calculator.addOperationNoRepetitionOrReturnIndexFirst(letterS), calculator);
  indexE.assignValue(input.cartanSymmetricInverseScale, calculator);
  rankE.assignValue(input.rank, calculator);
  letterAndIndexE.makeXOX(calculator, calculator.opPower(), letterE, indexE);
  return output.makeXOX(calculator, calculator.opUnderscore(), letterAndIndexE, rankE);
}

bool CalculatorConversions::innerStoreSemisimpleLieAlgebra(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (!input[1].isOfType<SemisimpleLieAlgebra*>()) {
    return output.makeError("Asking to store non-semisimple Lie algebra as such is not allowed. ", calculator);
  }
  SemisimpleLieAlgebra* owner = input[1].getValueNonConst<SemisimpleLieAlgebra*>();
  return CalculatorConversions::expressionFromDynkinType(calculator, owner->weylGroup.dynkinType, output);
}

bool CalculatorConversions::innerExpressionFromElementSemisimpleLieAlgebraRationals(
  Calculator& calculator, const ElementSemisimpleLieAlgebra<Rational>& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerStoreElementSemisimpleLieAlgebraRational");
  LinearCombination<Expression, Rational> monomials;
  monomials.makeZero();
  Expression currentMon;
  for (int i = 0; i < input.size(); i ++) {
    CalculatorConversions::expressionFromChevalleyGenerator(calculator, input[i], currentMon);
    monomials.addMonomial(currentMon, input.coefficients[i]);
  }
  return output.makeSum(calculator, monomials);
}

bool CalculatorConversions::expressionFromDynkinType(
  Calculator& calculator, const DynkinType& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::expressionFromDynkinType");
  LinearCombination<Expression, AlgebraicNumber> monomials;
  monomials.makeZero();
  Expression currentMonomial;
  for (int i = 0; i < input.size(); i ++) {
    CalculatorConversions::innerExpressionFromDynkinSimpleType(calculator, input[i], currentMonomial);
    monomials.addMonomial(currentMonomial, input.coefficients[i]);
  }
  return output.makeSum(calculator, monomials);
}

bool CalculatorConversions::expressionFromElementSemisimpleLieAlgebraAlgebraicNumbers(
  Calculator& calculator, const ElementSemisimpleLieAlgebra<AlgebraicNumber>& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::expressionFromElementSemisimpleLieAlgebraAlgebraicNumbers");
  LinearCombination<Expression, AlgebraicNumber> monomials;
  monomials.makeZero();
  Expression currentMonomial;
  for (int i = 0; i < input.size(); i ++) {
    CalculatorConversions::expressionFromChevalleyGenerator(calculator, input[i], currentMonomial);
    input.coefficients[i].checkConsistency();
    monomials.addMonomial(currentMonomial, input.coefficients[i]);
  }
  return output.makeSum(calculator, monomials);
}

bool CalculatorConversions::innerSlTwoSubalgebraPrecomputed(
  Calculator& calculator, const Expression& input, SlTwoSubalgebra& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerSlTwoSubalgebraPrecomputed");
  if (!input.isListNElements(4)) {
    return calculator << "<hr>input of innerLoadFromObject has "
    << input.size() << " children, 4 expected. ";
  }
  const Expression& ownerExpression = input[1];
  Expression tempE;
  if (!CalculatorConversions::functionSemisimpleLieAlgebra(
    calculator, ownerExpression, tempE, output.owner
  )) {
    return calculator
    << "<hr>Failed to extract semisimple Lie algebra from "
    << ownerExpression.toString()
    << " while extracting its sl(2) subalgebra.";
  }
  const Expression& expressionF = input[2];
  const Expression& expressionE = input[3];
  ElementSemisimpleLieAlgebra<Rational> elementF, elementE;
  if (!CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs(calculator, expressionF, elementF, *output.owner)) {
    return calculator << "<hr>Failed to extract f element while loading sl(2) subalgebra<hr>";
  }
  if (!CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs(calculator, expressionE, elementE, *output.owner)) {
    return calculator << "<hr>Failed to extract e element while loading sl(2) subalgebra<hr>";
  }
  if (elementE.isEqualToZero() || elementF.isEqualToZero()) {
    return calculator << "<hr>Failed to load sl(2) subalgebra: either e or f is equal to zero. e and f are: "
    << elementE.toString()
    << ", " << elementF.toString() << ". ";
  }
  if (elementE.getOwner() != elementF.getOwner()) {
    return calculator << "<hr>Failed to load sl(2): E and F element of sl(2) have different owners. "
    << "More precisely, the owner of e is "
    << elementE.getOwner()->toString() << " and the owner of f is " << elementF.getOwner()->toString();
  }
  output.eElement = elementE;
  output.fElement= elementF;
  output.owner = elementE.getOwner();
  DynkinType& dynkinType = output.owner->weylGroup.dynkinType;
  SemisimpleSubalgebras& ownerSubalgebras =
  calculator.objectContainer.getSemisimpleSubalgebrasCreateIfNotPresent(dynkinType);
  output.container = &ownerSubalgebras.slTwoSubalgebras;
  return true;
}

bool CalculatorConversions::innerSlTwoSubalgebraPrecomputed(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerSlTwoSubalgebraPrecomputed");
  SlTwoSubalgebra tempSL2;
  if (!CalculatorConversions::innerSlTwoSubalgebraPrecomputed(calculator, input, tempSL2)) {
    return calculator << "<hr>Failed to load sl(2) subalgebra. ";
  }
  return output.assignValue(tempSL2.toString(), calculator);
}

bool CalculatorConversions::innerAlgebraicNumber(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerAlgebraicNumber");
  if (input.isOfType<AlgebraicNumber>()) {
    output = input;
    return true;
  }
  if (input.isOfType<Rational>()) {
    AlgebraicNumber theNum;
    theNum = input.getValue<Rational>();
    return output.assignValue(theNum, calculator);
  }
  return false;
}

bool CalculatorConversions::innerLoadKeysFromStatementList(
  Calculator& calculator,
  const Expression& input,
  MapList<std::string, Expression, MathRoutines::hashString>& output,
  std::stringstream* commentsOnFailure,
  bool allowFailure
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerLoadKeysFromStatementList");
  MapList<Expression, Expression> outputExpressionFormat;
  if (!CalculatorConversions::innerLoadKeysFromStatementList(
    calculator, input, outputExpressionFormat, commentsOnFailure, allowFailure
  )) {
    return false;
  }
  output.clear();
  std::string keyName;
  for (int i = 0; i < outputExpressionFormat.keys.size; i ++) {
    if (!outputExpressionFormat.keys[i].isOfType<std::string>(&keyName)) {
      keyName = outputExpressionFormat.keys[i].toString();
    }
    output.setKeyValue(keyName, outputExpressionFormat.values[i]);
  }
  return true;
}

bool CalculatorConversions::innerLoadKeysFromStatementList(
  Calculator& calculator,
  const Expression& input,
  MapList<Expression, Expression>& output,
  std::stringstream* commentsOnFailure,
  bool allowFailure
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerLoadKeysFromStatementList");
  output.clear();
  for (int i = 1; i < input.size(); i ++) {
    if (input[i].startsWith(calculator.opDefine(), 3)) {
      output.setKeyValue(input[i][1], input[i][2]);
    } else if (!allowFailure) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Could not extract key-value pair from: "
        << input.toString() << ": failed to process: "
        << input[i].toString();
      }
      return false;
    }
  }
  return true;
}

bool CalculatorConversions::innerLoadKey(
  Calculator& calculator,
  const Expression& inputStatementList,
  const std::string& inputKey,
  Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerLoadKey");
  Expression theKeyE;
  theKeyE.makeAtom(inputKey, calculator);
  for (int i = 0; i < inputStatementList.size(); i ++) {
    if (inputStatementList[i].startsWith(calculator.opDefine(), 3)) {
      if (inputStatementList[i][1] == theKeyE) {
        output = inputStatementList[i][2];
        return true;
      }
    }
  }
  return calculator << "<hr>Key " << inputKey << " not found in expression "
  << inputStatementList.toString() << ".";
}

bool CalculatorConversions::storeCandidateSubalgebra(
  Calculator& calculator, const CandidateSemisimpleSubalgebra& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::storeCandidateSubalgebra");
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
  currentE.assignMatrix(conversionMatrix, calculator, nullptr, false);
  keys.addOnTop("ElementsCartan");
  values.addOnTop(currentE);
  input.checkAll();
  if (input.flagSystemSolved) {
    Expression listGenerators;
    listGenerators.makeSequence(calculator);
    for (int i = 0; i < input.negativeGenerators.size; i ++) {
      CalculatorConversions::expressionFromElementSemisimpleLieAlgebraAlgebraicNumbers(
        calculator, input.negativeGenerators[i], currentE
      );
      listGenerators.addChildOnTop(currentE);
      CalculatorConversions::expressionFromElementSemisimpleLieAlgebraAlgebraicNumbers(
        calculator, input.positiveGenerators[i], currentE
      );
      listGenerators.addChildOnTop(currentE);
    }
    keys.addOnTop("generators");
    values.addOnTop(listGenerators);
  }
  return output.makeSequenceCommands(calculator, keys, values);
}

bool CalculatorConversions::innerCandidateSubalgebraPrecomputed(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  CandidateSemisimpleSubalgebra& outputSubalgebra,
  SemisimpleSubalgebras& owner
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerCandidateSubalgebraPrecomputed");
  owner.checkInitialization();
  ProgressReport report;
  std::stringstream reportStream;
  reportStream << "Loading precomputed semisimple subalgebra. ";
  report.report(reportStream.str());
  Expression dynkinTypeE, ElementsCartanE, generatorsE;
  if (
    !CalculatorConversions::innerLoadKey(calculator, input, "DynkinType", dynkinTypeE) ||
    !CalculatorConversions::innerLoadKey(calculator, input, "ElementsCartan", ElementsCartanE)
  ) {
    return false;
  }
  reportStream << "Extracted types: " << dynkinTypeE.toString() << ". ";
  report.report(reportStream.str());
  outputSubalgebra.owner = &owner;
  DynkinType theNonEmbeddedDynkinType;
  if (!CalculatorConversions::functionDynkinType(
    calculator, dynkinTypeE, theNonEmbeddedDynkinType
  )) {
    return calculator
    << "<hr>Failed to load dynkin type of candidate subalgebra from "
    << dynkinTypeE.toString() << "<hr>";
  }
  reportStream << "Non embedded Dynkin type: " << dynkinTypeE.toString() << ". ";
  report.report(reportStream.str());
  outputSubalgebra.weylNonEmbedded = &
  calculator.objectContainer.getWeylGroupDataCreateIfNotPresent(theNonEmbeddedDynkinType);
  outputSubalgebra.weylNonEmbedded->makeFromDynkinType(theNonEmbeddedDynkinType);
  int rank = owner.owner->getRank();
  reportStream << "Extracting matrix of Cartan elements. ";
  report.report(reportStream.str());
  Matrix<Rational> hElements;
  if (!calculator.functionGetMatrix(ElementsCartanE, hElements, nullptr, rank, nullptr)) {
    return calculator << "<hr>Failed to load Cartan elements for candidate subalgebra of type "
    << outputSubalgebra.weylNonEmbedded->dynkinType << "<hr>";
  }
  if (hElements.numberOfRows != outputSubalgebra.weylNonEmbedded->getDimension()) {
    return calculator << "<hr>Failed to load Cartan elements: I expected "
    << outputSubalgebra.weylNonEmbedded->getDimension() << " elements, but failed to get them.";
  }
  List<int> ranks, multiplicities;
  outputSubalgebra.weylNonEmbedded->dynkinType.getLettersTypesMultiplicities(nullptr, &ranks, &multiplicities, nullptr);
  outputSubalgebra.cartanSubalgebrasByComponentScaledToActByTwo.setSize(
    outputSubalgebra.weylNonEmbedded->dynkinType.getNumberOfSimpleComponents()
  );
  int componentCounter = - 1;
  int counter = - 1;
  for (int i = 0; i < multiplicities.size; i ++) {
    for (int j = 0; j < multiplicities[i]; j ++) {
      componentCounter ++;
      Vectors<Rational>& currentComponent = outputSubalgebra.cartanSubalgebrasByComponentScaledToActByTwo[componentCounter];
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
  if (CalculatorConversions::innerLoadKey(calculator, input, "generators", generatorsE)) {
    generatorsE.sequencefy();
    ElementSemisimpleLieAlgebra<AlgebraicNumber> curGenAlgebraic;
    for (int i = 1; i < generatorsE.size(); i ++) {
      if (!CalculatorConversions::innerLoadElementSemisimpleLieAlgebraAlgebraicNumbers(
        calculator, generatorsE[i], curGenAlgebraic, *owner.owner
      )) {
        return calculator << "<hr>Failed to load semisimple Lie algebra element from expression "
        << generatorsE[i].toString() << ". ";
      }
      if (i % 2 == 1) {
        outputSubalgebra.negativeGenerators.addOnTop(curGenAlgebraic);
      } else {
        outputSubalgebra.positiveGenerators.addOnTop(curGenAlgebraic);
      }
    }
    outputSubalgebra.flagSystemProvedToHaveNoSolution = false;
    outputSubalgebra.flagSystemSolved = true;
  } else {
    return calculator << "<hr>Failed to extract subalgebra generators from expression " << input.toString() << ". ";
  }
  SemisimpleLieAlgebra& currentNonEmbededSA =
  owner.subalgebrasNonEmbedded->getValueCreateNoInitialization(outputSubalgebra.weylNonEmbedded->dynkinType);
  outputSubalgebra.indexNonEmbeddedMeStandard =
  owner.subalgebrasNonEmbedded->getIndex(outputSubalgebra.weylNonEmbedded->dynkinType);
  currentNonEmbededSA.weylGroup.computeRho(true);
  outputSubalgebra.weylNonEmbedded->computeRho(true); //<- this line may be unnecessary, the
  //two Weyl groups may coincide depending on some implementation decisions I am about to take
  //some time soon.
  outputSubalgebra.computeHsAndHsScaledToActByTwoFromComponents();
  outputSubalgebra.flagSubalgebraPreloadedButNotVerified = true;
  return output.makeError(
    "Candidate subalgebra is not a stand-alone object and its Expression output should not be used. ",
    calculator
  );
}

bool CalculatorConversions::innerLoadSemisimpleSubalgebras(
  Calculator& calculator, const Expression& inpuT, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerLoadSemisimpleSubalgebras");
  Expression input = inpuT;
  Expression ambientTypeE, numExploredHsE, numExploredTypesE, subalgebrasE, currentChainE;
  if (!CalculatorConversions::innerLoadKey(calculator, input, "AmbientDynkinType", ambientTypeE)) {
    return calculator << "<hr>Failed to load Dynkin type from: " << input.toString();
  }
  if (!CalculatorConversions::innerLoadKey(calculator, input, "NumExploredHs", numExploredHsE)) {
    return calculator << "<hr>Failed to load numExploredHs list from: " << input.toString();
  }
  if (!CalculatorConversions::innerLoadKey(calculator, input, "NumExploredTypes", numExploredTypesE)) {
    return calculator << "<hr>Failed to load NumExploredTypes list from: " << input.toString();
  }
  if (!CalculatorConversions::innerLoadKey(calculator, input, "Subalgebras", subalgebrasE)) {
    return calculator << "<hr>Failed to load Subalgebras list from: " << input.toString();
  }
  if (!CalculatorConversions::innerLoadKey(calculator, input, "CurrentChain", currentChainE)) {
    return calculator << "<hr>Failed to load CurrentChain from: " << input.toString();
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
  SemisimpleLieAlgebra* ownerSemisimple = nullptr;
  Expression tempE;
  ProgressReport report;
  std::stringstream reportStream;
  reportStream << "Extracting semisimple Lie algebra ... ";
  report.report(reportStream.str());
  if (!CalculatorConversions::functionSemisimpleLieAlgebra(
    calculator, ambientTypeE, tempE, ownerSemisimple
  )) {
    return calculator << "<hr>Error loading semisimple subalgebras: "
    << "failed to extract ambient semisimple Lie algebra. ";
  }
  reportStream << " type: " << ownerSemisimple->weylGroup.dynkinType.toString() << ". ";
  report.report(reportStream.str());

  SemisimpleSubalgebras& subalgebras =
  calculator.objectContainer.getSemisimpleSubalgebrasCreateIfNotPresent(ownerSemisimple->weylGroup.dynkinType);
  subalgebras.subalgebrasNonEmbedded = &calculator.objectContainer.semisimpleLieAlgebras;
  subalgebras.owner = ownerSemisimple;
  reportStream << " Initializing data structures... ";
  report.report(reportStream.str());
  reportStream << " done. Fetching subalgebra list ... ";
  report.report(reportStream.str());
  subalgebrasE.sequencefy();
  subalgebras.subalgebras.setExpectedSize(subalgebrasE.size() - 1);
  subalgebras.subalgebras.clear();
  subalgebras.subalgebrasNonEmbedded->setExpectedSize(subalgebrasE.size() - 1);
  subalgebras.flagAttemptToSolveSystems = true;
  subalgebras.flagcomputeModuleDecompositionsition = true;
  subalgebras.flagcomputePairingTable = false;
  subalgebras.flagComputeNilradicals = false;
  subalgebras.millisecondsComputationStart = global.getElapsedMilliseconds();
  reportStream << " done. <br>Total subalgebras: " << subalgebrasE.size() - 1 << ". ";
  report.report(reportStream.str());

  for (int i = 1; i < subalgebrasE.size(); i ++) {
    std::stringstream reportStream2;
    reportStream2 << reportStream.str() << "Subalgebra "
    << i << " is being loaded from expression "
    << subalgebrasE[i].toString() << ".";
    report.report(reportStream2.str());
    CandidateSemisimpleSubalgebra currentCandidate;
    if (!CalculatorConversions::innerCandidateSubalgebraPrecomputed(
      calculator, subalgebrasE[i], tempE, currentCandidate, subalgebras
    )) {
      return calculator
      << "<hr>Error loading candidate subalgebra: failed to load candidate number "
      << i << " extracted from expression: "
      << subalgebrasE[i].toString() << ". <hr>";
    }

    currentCandidate.checkFullInitialization();
    if (subalgebras.subalgebras.contains(currentCandidate.cartanElementsSubalgebra)) {
      calculator << "<hr>Did not load subalgebra of type "
      << currentCandidate.weylNonEmbedded->toString()
      << " because I've already loaded a subalgebra with "
      << "the same Cartan subalgebra. ";
      continue;
    }
    subalgebras.subalgebras.setKeyValue(currentCandidate.cartanElementsSubalgebra, currentCandidate);
    subalgebras.subalgebras.values.lastObject().indexInOwner = subalgebras.subalgebras.values.size - 1;
  }
  reportStream << "Subalgebra loading done, total "
  << subalgebras.subalgebras.values.size << " subalgebras loaded. ";
  report.report(reportStream.str());
  subalgebras.toStringExpressionString = CalculatorConversions::stringFromSemisimpleSubalgebras;
  if (!subalgebras.loadState(currentChainInt, numExploredTypes, numExploredHs, calculator.comments)) {
    return false;
  }
  subalgebras.flagAttemptToAdjustCentralizers = false;
  if (!subalgebras.findSemisimpleSubalgebrasContinue()) {
    std::stringstream out;
    out << "<br>Failed to realize all subalgebras, "
    << "computation aborted. The failure report follows. "
    << subalgebras.comments << "<br>The progress report for the "
    << "entire computation follows.<br>"
    << subalgebras.toStringProgressReport();
    return output.assignValue(out.str(), calculator);
  }
  subalgebras.millisecondsComputationEnd = global.getElapsedMilliseconds();
  return output.assignValue(subalgebras, calculator);
}

std::string CalculatorConversions::stringFromSemisimpleSubalgebras(SemisimpleSubalgebras& input) {
  MacroRegisterFunctionWithName("CalculatorConversions::stringFromSemisimpleSubalgebras");
  Expression expression;
  FormatExpressions format;
  CalculatorConversions::storeSemisimpleSubalgebras(global.calculator().getElement(), input, expression);
  format.flagUseHTML = true;
  return expression.toString(&format);
}

bool CalculatorConversions::storeSemisimpleSubalgebras(
  Calculator& calculator, const SemisimpleSubalgebras& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::storeSemisimpleSubalgebras");
  Expression dynkinTypeE;
  List<std::string> keys;
  List<Expression> values;
  if (!CalculatorConversions::expressionFromDynkinType(
    calculator, input.owner->weylGroup.dynkinType, dynkinTypeE
  )) {
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
  for (int i = 0; i < input.currentNumberOfLargerTypesExplored.size; i ++) {
    numericalConvertorE = input.currentNumberOfLargerTypesExplored[i];
    numTypesExploredE.addChildOnTop(numericalConvertorE);
  }
  keys.addOnTop("NumExploredTypes");
  values.addOnTop(numTypesExploredE);
  Expression numHsExploredE;
  numHsExploredE.makeSequence(calculator);
  for (int i = 0; i < input.currentNumberOfHCandidatesExplored.size; i ++) {
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
    if (!CalculatorConversions::storeCandidateSubalgebra(calculator, input.subalgebras.values[i], candidateE)) {
      return false;
    }
    subalgebrasListE.addChildOnTop(candidateE);
  }
  keys.addOnTop("Subalgebras");
  values.addOnTop(subalgebrasListE);
  return output.makeSequenceCommands(calculator, keys, values);
}

bool CalculatorConversions::innerExpressionFromMonomialUE(
  Calculator& calculator,
  const MonomialUniversalEnveloping<RationalFraction<Rational> >& input,
  Expression& output,
  ExpressionContext* inputContext
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromMonomialUE");
  if (input.isConstant()) {
    return output.assignValue(1, calculator);
  }
  ChevalleyGenerator chevalleyGenerator;
  chevalleyGenerator.owner = input.owner;
  Expression chevGenE, powerE, termE;
  List<Expression> theTerms;
  for (int i = 0; i < input.generatorsIndices.size; i ++) {
    chevalleyGenerator.generatorIndex = input.generatorsIndices[i];
    CalculatorConversions::expressionFromChevalleyGenerator(calculator, chevalleyGenerator, chevGenE);
    CalculatorConversions::expressionFromRationalFraction<Rational>(calculator, input.powers[i], powerE, inputContext);
    termE.makeXOX(calculator, calculator.opPower(), chevGenE, powerE);
    theTerms.addOnTop(termE);
  }
  return output.makeProduct(calculator, theTerms);
}

bool CalculatorConversions::innerExpressionFromUE(
  Calculator& calculator,
  const ElementUniversalEnveloping<RationalFraction<Rational> >& input,
  Expression& output,
  ExpressionContext* inputContext
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromUE");
  LinearCombination<Expression, RationalFraction<Rational> > theUEE;
  theUEE.makeZero();
  Expression currentMonE;
  for (int i = 0; i < input.size(); i ++) {
    if (!CalculatorConversions::innerExpressionFromMonomialUE(
      calculator, input[i], currentMonE, inputContext
    )) {
      return calculator << "<hr>Failed to store " << input.toString();
    }
    theUEE.addMonomial(currentMonE, input.coefficients[i]);
  }
  return output.makeSum(calculator, theUEE);
}

bool CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs(
  Calculator& calculator,
  const Expression& input,
  ElementSemisimpleLieAlgebra<Rational>& output,
  SemisimpleLieAlgebra& owner
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs");
  ElementSemisimpleLieAlgebra<AlgebraicNumber> outputAlgebraic;
  if (!CalculatorConversions::innerLoadElementSemisimpleLieAlgebraAlgebraicNumbers(
    calculator, input, outputAlgebraic, owner
  )) {
    return calculator << "<hr> Failed to obtain semisimple Lie algebra element from "
    << input.toString() << ". ";
  }
  for (int i = 0; i < outputAlgebraic.coefficients.size; i ++) {
    if (!outputAlgebraic.coefficients[i].isRational()) {
      return calculator << "<hr>From input: "
      << input.toString() << ", I managed to extract element: "
      << outputAlgebraic.toString()
      << " but that appears to not have rational coefficients.";
    }
  }
  output = outputAlgebraic; //<-implicit conversion here!
  return true;
}

bool CalculatorConversions::innerLoadElementSemisimpleLieAlgebraAlgebraicNumbers(
  Calculator& calculator,
  const Expression& input,
  ElementSemisimpleLieAlgebra<AlgebraicNumber>& output,
  SemisimpleLieAlgebra& owner
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerLoadElementSemisimpleLieAlgebraAlgebraicNumbers");
  Expression polyFormE;
  Polynomial<AlgebraicNumber> polyForm;
  bool polyFormGood = CalculatorConversions::functionPolynomial<AlgebraicNumber>(calculator, input, polyFormE);
  if (polyFormGood) {
    polyFormGood = polyFormE.isOfType<Polynomial<AlgebraicNumber> >(&polyForm);
  }
  if (!polyFormGood) {
    return calculator << "<hr>Failed to convert " << input.toString() << " to polynomial.<hr>";
  }
  ChevalleyGenerator chevalleyGenerator;
  ElementSemisimpleLieAlgebra<AlgebraicNumber> currentElt;
  chevalleyGenerator.owner = &owner;
  output.makeZero();
  ExpressionContext theContext = polyFormE.getContext();
  for (int j = 0; j < polyForm.size(); j ++) {
    const MonomialPolynomial& currentMon = polyForm[j];
    int chevalleyGeneratorIndex = 0;
    if (!currentMon.isOneLetterFirstDegree(&chevalleyGeneratorIndex)) {
      return calculator << "<hr>Failed to convert semisimple Lie algebra input to linear poly: "
      << input.toString() << ".<hr>";
    }
    Expression singleChevGenE = theContext.getVariable(chevalleyGeneratorIndex);
    if (!singleChevGenE.startsWith(calculator.opUnderscore(), 3)) {
      return calculator << "<hr>Failed to convert: "
      << singleChevGenE.toString()
      << "(summand of: "
      << input.toString() << ") to Chevalley generator.<hr>";
    }
    std::string theLetter;
    if (
      !singleChevGenE[1].isOperation(&theLetter) ||
      !singleChevGenE[2].isSmallInteger(&chevalleyGenerator.generatorIndex)
    ) {
      return calculator << "<hr>Failed to convert summand "
      << singleChevGenE.toString() << " to Chevalley generator of "
      << owner.toStringLieAlgebraName();
    }
    bool isGood = true;
    if (theLetter == "g") {
      chevalleyGenerator.generatorIndex = owner.getGeneratorFromDisplayIndex(chevalleyGenerator.generatorIndex);
      if (chevalleyGenerator.generatorIndex < 0 || chevalleyGenerator.generatorIndex >= owner.getNumberOfGenerators()) {
        isGood = false;
      }
      output.addMonomial(chevalleyGenerator, polyForm.coefficients[j]);
    } else if (theLetter == "h") {
      int theRootIndex = owner.getRootIndexFromDisplayIndex(chevalleyGenerator.generatorIndex);
      if (theRootIndex < 0) {
        isGood = false;
      } else {
        currentElt.makeCartanGenerator(owner.weylGroup.rootSystem[theRootIndex], owner);
        currentElt *= polyForm.coefficients[j];
        output += currentElt;
      }
    } else {
      isGood = false;
    }
    if (!isGood) {
      return calculator << "<hr>Failed to convert summand "
      << singleChevGenE.toString() << " to Chevalley generator of "
      << owner.toStringLieAlgebraName();
    }
  }
  return true;
}

bool CalculatorConversions::innerElementUE(
  Calculator& calculator, const Expression& input, Expression& output, SemisimpleLieAlgebra& owner
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerElementUE");
  if (input.size() != 2) {
    return calculator << "Universal enveloping algebra element expects a single argument. ";
  }
  ChevalleyGenerator chevalleyGenerator;
  chevalleyGenerator.owner = &owner;
  ElementUniversalEnveloping<RationalFraction<Rational> > outputUE;
  ElementUniversalEnveloping<RationalFraction<Rational> > currentSummand;
  ElementUniversalEnveloping<RationalFraction<Rational> > currentMultiplicand;
  MonomialPolynomial currentMultiplicandRFpartMon;
  Polynomial<Rational> currentPMultiplicand;
  RationalFraction<Rational>  currentMultiplicandRFpart;
  outputUE.makeZero(owner);
  Expression polyE;
  if (!CalculatorConversions::functionPolynomial<Rational>(calculator, input[1], polyE)) {
    return calculator << "<hr>Failed to convert " << input[1].toString() << " to polynomial.<hr>";
  }
  Polynomial<Rational> theP;
  if (polyE.isError() || !polyE.isOfType<Polynomial<Rational> >(&theP)) {
    return calculator << "<hr>Failed to convert " << input[1].toString()
    << " to polynomial. Instead I got " << polyE.toString() << ". <hr>";
  }
  ExpressionContext theContext = polyE.getContext();
  HashedList<Expression> polynomialVariables;
  for (int j = 0; j < theP.size(); j ++) {
    const MonomialPolynomial& currentMon = theP[j];
    currentSummand.makeConstant(theP.coefficients[j], owner);
    currentMultiplicandRFpartMon.makeOne();
    for (int i = 0; i < currentMon.minimalNumberOfVariables(); i ++) {
      int power = - 1;
      if (!currentMon(i).isSmallInteger(&power)) {
        return calculator
        << "<hr>Failed to convert one of the exponents appearing in "
        << input[1].toString()
        << " to  a small integer polynomial.<hr>";
      }
      if (power == 0) {
        continue;
      }
      Expression singleChevGenE = theContext.getVariable(i);
      if (!singleChevGenE.isListNElements(2)) {
        return calculator << "<hr>Failed to convert "
        << input[1].toString() << " to polynomial.<hr>";
      }
      std::string theLetter;
      if (
        !singleChevGenE[0].isOperation(&theLetter) ||
        !singleChevGenE[1].isSmallInteger(&chevalleyGenerator.generatorIndex)
      ) {
        return calculator << "<hr>Failed to convert summand "
        << singleChevGenE.toString() << " to Chevalley generator of "
        << owner.toStringLieAlgebraName();
      }
      bool isGood = true;
      bool isHonestElementUE = true;
      if (theLetter == "g") {
        chevalleyGenerator.generatorIndex = owner.getGeneratorFromDisplayIndex(chevalleyGenerator.generatorIndex);
        if (chevalleyGenerator.generatorIndex < 0 || chevalleyGenerator.generatorIndex >= owner.getNumberOfGenerators()) {
          isGood = false;
        }
      } else if (theLetter == "h") {
        if (chevalleyGenerator.generatorIndex < 1 || chevalleyGenerator.generatorIndex>owner.getRank()) {
          isGood = false;
        } else {
          chevalleyGenerator.generatorIndex = chevalleyGenerator.generatorIndex +owner.getNumberOfPositiveRoots() - 1;
        }
      } else {
        isHonestElementUE = false;
      }
      if (!isGood) {
        return calculator << "<hr>Failed to convert summand "
        << singleChevGenE.toString() << " to Chevalley generator of "
        << owner.toStringLieAlgebraName();
      }
      if (isHonestElementUE) {
        currentMultiplicand.makeOneGenerator(chevalleyGenerator.generatorIndex, owner, Rational::one());
        currentMultiplicand.raiseToPower(power);
        currentSummand*= currentMultiplicand;
      } else {
        int variableIndex = polynomialVariables.addNoRepetitionOrReturnIndexFirst(singleChevGenE);
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
  return output.assignValueWithContext(outputUE, outputContext, calculator);
}

bool CalculatorConversions::expressionFromBuiltInType(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (input.size() != 2) {
    return false;
  }
  return CalculatorConversions::functionExpressionFromBuiltInType(calculator, input[1], output);
}

bool CalculatorConversions::functionExpressionFromBuiltInType(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::functionExpressionFromBuiltInType");
  if (input.isOfType<Rational>() || input.isOfType<double>() || input.isOfType<AlgebraicNumber>()) {
    output = input;
    return true;
  }
  if (input.isOfType<Polynomial<Rational> >()) {
    return CalculatorConversions::functionExpressionFromPolynomial<Rational>(calculator, input, output);
  }
  if (input.isOfType<Polynomial<AlgebraicNumber> >()) {
    return CalculatorConversions::functionExpressionFromPolynomial<AlgebraicNumber>(calculator, input, output);
  }
  if (input.isOfType<RationalFraction<Rational> >()) {
    return CalculatorConversions::expressionFromRationalFraction<Rational>(calculator, input, output);
  }
  if (input.isOfType<RationalFraction<AlgebraicNumber> >()) {
    return CalculatorConversions::expressionFromRationalFraction<AlgebraicNumber>(calculator, input, output);
  }
  return false;
}

bool CalculatorConversions::innerExpressionFromUE(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromUE");
  ElementUniversalEnveloping<RationalFraction<Rational> > elementUniversalEnveloping;
  if (!input.isOfType<ElementUniversalEnveloping<RationalFraction<Rational> > >(&elementUniversalEnveloping)) {
    return calculator << "<hr>Expression " << input.toString()
    << " is not an element of universal enveloping, can't convert to expression";
  }
  return CalculatorConversions::innerExpressionFromUE(calculator, elementUniversalEnveloping, output);
}

bool CalculatorConversions::rationalFunction(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::rationalFunction");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorConversions::functionRationalFunction<Rational>(calculator, input[1], output);
}

bool CalculatorConversions::functionMatrixRational(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::functionMatrixRational");
  Matrix<Rational> outputMatrix;
  if (!calculator.functionGetMatrix(input, outputMatrix, nullptr, - 1, nullptr)) {
    return calculator << "<br>Failed to get matrix of rationals. ";
  }
  return output.assignMatrix(outputMatrix, calculator);
}

bool CalculatorConversions::innerMatrixRationalTensorForM(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerMatrixRationalTensorForM");
  if (!CalculatorConversions::innerMakeMatrix(calculator, input, output)) {
    return calculator << "Failed to extract matrix of rationals. ";
  }
  Matrix<Rational> matrixRational;
  if (!calculator.functionGetMatrix(output, matrixRational)) {
    return false;
  }
  MatrixTensor<Rational> outputMatrixTensor;
  outputMatrixTensor = matrixRational;
  return output.assignValue(outputMatrixTensor, calculator);
}

bool CalculatorConversions::functionMatrixRationalTensorForm(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::functionMatrixRationalTensorForm");
  Matrix<Rational> matrixRational;
  if (!calculator.functionGetMatrix(input, matrixRational)) {
    return false;
  }
  MatrixTensor<Rational> outputMatrixTensor;
  outputMatrixTensor = matrixRational;
  return output.assignValue(outputMatrixTensor, calculator);
}

bool CalculatorConversions::outerMatrixExpressionsToMatrixOfType(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::outerMatrixExpressionsToMatrixOfType");
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

bool CalculatorConversions::innerMakeMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerMakeMatrix");
  Matrix<Expression> outputMatrix;
  if (calculator.getMatrixExpressionsFromArguments(input, outputMatrix)) {
    return output.assignMatrixExpressions(outputMatrix, calculator, true, true);
  }
  return false;
}

bool CalculatorConversions::functionMatrixAlgebraic(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::functionMatrixAlgebraic");
  Matrix<AlgebraicNumber> outputMat;
  if (!calculator.functionGetMatrix(input, outputMat)) {
    return false;
  }
  return output.assignMatrix(outputMat, calculator);
}

bool CalculatorConversions::innerMatrixRationalFunction(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return CalculatorConversions::functionMatrixRationalFunction(
    calculator, input, output
  );
}

bool CalculatorConversions::functionMatrixRationalFunction(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::functionMatrixRationalFunction");
  Matrix<RationalFraction<Rational> > outputMat;
  ExpressionContext context(calculator);
  if (!calculator.functionGetMatrix(
    input,
    outputMat,
    &context,
    - 1,
    CalculatorConversions::functionRationalFunction<Rational>
  )) {
    return calculator << "<hr>Failed to get matrix of rational functions. ";
  }
  output.assignMatrix(outputMat, calculator, &context);
  output.checkConsistency();
  return true;
}

bool CalculatorConversions::innerLoadFileIntoString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerLoadFileIntoString");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  std::string theRelativeFileName;
  if (!argument.isOfType<std::string>(&theRelativeFileName)) {
    calculator << "Input of load file string command is supposed to be a string. "
    << "Converting your expression to a string and using that instead. ";
    theRelativeFileName = argument.toString();
  }
  std::string outputString;
  if (!FileOperations::loadFileToStringVirtual(
    theRelativeFileName, outputString, false, &calculator.comments
  )) {
    return false;
  }
  return output.assignValue(outputString, calculator);
}

bool CalculatorConversions::innerMakeElementHyperOctahedral(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerMakeElementHyperOctahedral");
  std::string inputStringFormat;
  ElementHyperoctahedralGroupR2 element;
  if (input.isOfType<std::string>(&inputStringFormat)) {
    element.makeFromString(inputStringFormat);
    return output.assignValue(element, calculator);
  }
  if (input.size() < 3) {
    return calculator << "To make elements of hyperoctahedral group we need at least 3 inputs. ";
  }
  List<int> oneCycle;
  if (!calculator.getVectorInt(input[1], oneCycle)) {
    return calculator
    << "Failed to extract a cycle structure from the first argument of input: "
    << input.toString();
  }
  for (int i = 0; i < oneCycle.size; i ++) {
    if (oneCycle[i] < 1) {
      return calculator << "Your input: " << input[1].toString()
      << " had integers that were too small. ";
    }
    oneCycle[i] --;
    if (oneCycle[i] > 1000) {
      return calculator << "Your input: " << input[1].toString()
      << " had integers that were too large. ";
    }
  }
  element.h.addCycle(oneCycle);
  for (int i = 2; i < input.size(); i ++) {
    if (input[i].isEqualToOne()) {
      element.k.toggleBit(i - 2);
    } else if (!input[i].isEqualToZero()) {
      return calculator << "Your input: " << input.toString()
      << " had bit values that were not ones and zeroes.";
    }
  }
  return output.assignValue(element, calculator);
}

bool CalculatorConversions::innerPolynomialModuloInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPolynomialModuloInteger");
  if (input.size() != 3) {
    return calculator
    << "Expected two arguments, polynomial and positive integer.";
  }
  WithContext<Polynomial<Rational> > polynomial;
  if (!CalculatorConversions::convert(
    input[1],
    CalculatorConversions::functionPolynomial<Rational>,
    polynomial
  )) {
    return false;
  }
  LargeInteger modulus;
  if (!input[2].isInteger(&modulus)) {
    return false;
  }
  if (modulus <= 0) {
    return calculator << "The modulus: " << modulus << " must be non-negative. ";
  }
  Polynomial<ElementZmodP> converted;
  ElementZmodP::convertModuloIntegerAfterScalingToIntegral(
    polynomial.content, converted, modulus.value
  );
  polynomial.context.setDefaultModulus(modulus.value);
  return output.assignValueWithContext(converted, polynomial.context, calculator);
}
