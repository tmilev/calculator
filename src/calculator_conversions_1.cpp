// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".#include "calculator_interface.h"
#include "math_extra_semisimple_Lie_algebras_implementation.h"
#include "calculator_interface.h"
#include "calculator_inner_functions.h"
#include "calculator_inner_typed_functions.h"
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_extra_universal_enveloping_implementation.h" // undefined reference to `ElementUniversalEnveloping<RationalFunctionOld>::makeZero(SemisimpleLieAlgebra&)'
#include "math_rational_function_implementation.h"

template <>
bool Expression::convertInternally<RationalFunction<Rational> >(Expression& output) const;

bool CalculatorConversions::innerExpressionFromChevalleyGenerator(
  Calculator& calculator, const ChevalleyGenerator& input, Expression& output
) {
  MacroRegisterFunctionWithName("innerExpressionFromChevalleyGenerator");
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
  SemisimpleLieAlgebra* dummySA;
  return CalculatorConversions::functionSemisimpleLieAlgebra(
    calculator, input, output, dummySA
  );
}

bool CalculatorConversions::innerSemisimpleLieAlgebra(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerSemisimpleLieAlgebra");
  if (input.size() != 2) {
    return calculator << "Semisimple Lie algebra expects a single argument. ";
  }
  SemisimpleLieAlgebra* dummySA;
  return CalculatorConversions::functionSemisimpleLieAlgebra(
    calculator, input[1], output, dummySA
  );
}

bool CalculatorConversions::innerLoadWeylGroup(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerLoadWeylGroup");
  DynkinType theType;
  if (!CalculatorConversions::innerDynkinTypE(calculator, input, theType)) {
    return false;
  }
  SemisimpleLieAlgebra& theSA = calculator.theObjectContainer.getLieAlgebraCreateIfNotPresent(theType);
  return output.assignValue(theSA.theWeyl, calculator);
}

bool CalculatorConversions::innerDynkinSimpleTypE(
  Calculator& calculator, const Expression& input, DynkinSimpleType& outputMon
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerDynkinSimpleTypE");
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
    if (typeLetterE.startsWith(calculator.opThePower(),3)) {
      scaleE = typeLetterE[2];
      typeLetterE = typeLetterE[1];
    } else {
      scaleE.assignValue(1, calculator);
    }
  } else if (input.startsWith(calculator.opThePower(), 3)) {
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
  std::string theTypeName;
  if (!typeLetterE.isOperation(&theTypeName)) {
    return calculator << "I couldn't extract a type letter from " << typeLetterE.toString();
  }
  if (theTypeName.size() != 1) {
    return calculator << "<hr>Error while extracting Dynkin simple type: "
    << "the type of a simple Lie algebra must be the letter A, B, C, D, E, F or G. "
    << "Instead, it is " << theTypeName + ". Error encountered while processing " << input.toString();
  }
  char theWeylLetter = theTypeName[0];
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
  int theRank;
  if (!rankE.isSmallInteger(&theRank)) {
    return calculator << "I wasn't able to extract rank from " << input.toString();
  }
  if (theRank < 1 || theRank > 20) {
    return calculator << "<hr>The rank of a simple Lie algebra "
    << "must be between 1 and 20; error while processing " << input.toString();
  }
  if (theWeylLetter == 'E' && (theRank > 8 || theRank < 3)) {
    return calculator << "<hr>Type E must have rank 6, 7 or 8 ";
  }
  if (theWeylLetter == 'D' && (theRank < 3)) {
    return calculator << "<hr>Type D is expected to have rank 4 or more, "
    << "your input was of rank " << theRank << ". ";
  }
  outputMon.makeArbitrary(theWeylLetter, theRank, theScale);
  return true;
}

bool CalculatorConversions::innerDynkinTypE(Calculator& calculator, const Expression& input, DynkinType& output) {
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
  LinearCombination<Expression, Rational> theType;
  if (!calculator.functionCollectSummands(calculator, input, theType)) {
    return false;
  }
  DynkinSimpleType simpleComponent;
  output.makeZero();
  for (int i = 0; i < theType.size(); i ++) {
    if (!CalculatorConversions::functionDynkinSimpleType(
      calculator, theType[i], simpleComponent
    )) {
      return false;
    }
    int theMultiplicity = - 1;
    if (!theType.coefficients[i].isSmallInteger(&theMultiplicity)) {
      theMultiplicity = - 1;
    }
    if (theMultiplicity < 0) {
      return calculator << "<hr>Failed to convert the coefficient "
      << theType.coefficients[i] << " of " << theType[i].toString()
      << " to a small positive integer. ";
    }
    output.addMonomial(simpleComponent, theMultiplicity);
  }
  return !theType.isEqualToZero();
}

bool CalculatorConversions::functionSemisimpleLieAlgebra(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  SemisimpleLieAlgebra*& outputPointer
) {
  RecursionDepthCounter recursionCounter(&calculator.recursionDepth);
  MacroRegisterFunctionWithName("Calculator::functionSemisimpleLieAlgebra");
  DynkinType theDynkinType;
  outputPointer = nullptr;
  if (!CalculatorConversions::functionDynkinType(
    calculator, input, theDynkinType
  )) {
    return calculator << "Failed to extract Dynkin type from: " << input.toString();
  }
  if (theDynkinType.getRank() > 20) {
    return calculator
    << "I have been instructed to allow semisimple Lie algebras of rank 20 maximum. "
    << "If you would like to relax this limitation edit file " << __FILE__
    << " line " << __LINE__ << ". Note that the Chevalley "
    << "constant computation Reserves a dim(g)*dim(g) "
    << "table of RAM memory, which means the RAM memory rises with the 4^th power of rank(g). "
    << "You have been warned. Alternatively, you may want to implement a sparse structure constant table "
    << "(write me an email if you want to do that, I will help you). ";
  }
  bool newlyCreated = !calculator.theObjectContainer.semisimpleLieAlgebras.contains(theDynkinType);
  outputPointer = &calculator.theObjectContainer.getLieAlgebraCreateIfNotPresent(theDynkinType);
  outputPointer->checkConsistency();
  output.assignValue(outputPointer, calculator);
  if (newlyCreated) {
    outputPointer->computeChevalleyConstants();
    Expression tempE;
    CalculatorFunctions::functionWriteToHDOrPrintSSLieAlgebra(
      calculator, output, tempE, false, false
    );
    calculator << tempE.getValue<std::string>();
  }
  return true;
}

bool CalculatorConversions::innerExpressionFromDynkinSimpleType(
  Calculator& calculator, const DynkinSimpleType& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::innerExpressionFromDynkinSimpleType");
  Expression letterE, rankE, letterAndIndexE, indexE;
  std::string letterS;
  letterS = input.theLetter;
  letterE.makeAtom(calculator.addOperationNoRepetitionOrReturnIndexFirst(letterS), calculator);
  indexE.assignValue(input.CartanSymmetricInverseScale, calculator);
  rankE.assignValue(input.theRank, calculator);
  letterAndIndexE.makeXOX(calculator, calculator.opThePower(), letterE, indexE);
  return output.makeXOX(calculator, calculator.opUnderscore(), letterAndIndexE, rankE);
}

template < >
SemisimpleLieAlgebra*& Expression::getValueNonConst() const;

bool CalculatorConversions::innerStoreSemisimpleLieAlgebra(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (!input.isOfType<SemisimpleLieAlgebra*>()) {
    return output.makeError("Asking to store non-semisimple Lie algebra as such is not allowed. ", calculator);
  }
  SemisimpleLieAlgebra* owner = input.getValueNonConst<SemisimpleLieAlgebra*>();
  if (owner == nullptr) {
    global.fatal << "Unexpected null pointer" << global.fatal;
  }
  return CalculatorConversions::innerExpressionFromDynkinType(calculator, owner->theWeyl.theDynkinType, output);
}

bool CalculatorConversions::innerExpressionFromElementSemisimpleLieAlgebraRationals(
  Calculator& calculator, const ElementSemisimpleLieAlgebra<Rational>& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerStoreElementSemisimpleLieAlgebraRational");
  LinearCombination<Expression, Rational> theMons;
  theMons.makeZero();
  Expression currentMon;
  for (int i = 0; i < input.size(); i ++) {
    CalculatorConversions::innerExpressionFromChevalleyGenerator(calculator, input[i], currentMon);
    theMons.addMonomial(currentMon, input.coefficients[i]);
  }
  return output.makeSum(calculator, theMons);
}

bool CalculatorConversions::innerExpressionFromDynkinType(
  Calculator& calculator, const DynkinType& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromDynkinType");
  LinearCombination<Expression, AlgebraicNumber> theMons;
  theMons.makeZero();
  Expression currentMon;
  for (int i = 0; i < input.size(); i ++) {
    CalculatorConversions::innerExpressionFromDynkinSimpleType(calculator, input[i], currentMon);
    theMons.addMonomial(currentMon, input.coefficients[i]);
  }
  return output.makeSum(calculator, theMons);
}

bool CalculatorConversions::innerExpressionFromElementSemisimpleLieAlgebraAlgebraicNumbers(
  Calculator& calculator, const ElementSemisimpleLieAlgebra<AlgebraicNumber>& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromElementSemisimpleLieAlgebraAlgebraicNumbers");
  LinearCombination<Expression, AlgebraicNumber> theMons;
  theMons.makeZero();
  Expression currentMon;
  for (int i = 0; i < input.size(); i ++) {
    CalculatorConversions::innerExpressionFromChevalleyGenerator(calculator, input[i], currentMon);
    theMons.addMonomial(currentMon, input.coefficients[i]);
  }
  return output.makeSum(calculator, theMons);
}

bool CalculatorConversions::innerSlTwoSubalgebraPrecomputed(
  Calculator& calculator, const Expression& input, SlTwoSubalgebra& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerLoadFromObject SlTwoSubalgebra");
  if (!input.isListNElements(4)) {
    return calculator << "<hr>input of innerLoadFromObject has " << input.children.size << " children, 4 expected. ";
  }
  const Expression& theOwnerE = input[1];
  Expression tempE;
  if (!CalculatorConversions::functionSemisimpleLieAlgebra(
    calculator, theOwnerE, tempE, output.owner
  )) {
    return calculator << "<hr>Failed to extract semisimple Lie algebra from " << theOwnerE.toString()
    << " while extracting its sl(2) subalgebra.";
  }
  const Expression& theF = input[2];
  const Expression& theE = input[3];
  ElementSemisimpleLieAlgebra<Rational> eltF, eltE;
  if (!CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs(calculator, theF, eltF, *output.owner)) {
    return calculator << "<hr>Failed to extract f element while loading sl(2) subalgebra<hr>";
  }
  if (!CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs(calculator, theE, eltE, *output.owner)) {
    return calculator << "<hr>Failed to extract e element while loading sl(2) subalgebra<hr>";
  }
  if (eltE.isEqualToZero() || eltF.isEqualToZero()) {
    return calculator << "<hr>Failed to load sl(2) subalgebra: either e or f is equal to zero. e and f are: "
    << eltE.toString()
    << ", " << eltF.toString() << ". ";
  }
  if (eltE.getOwner() != eltF.getOwner()) {
    return calculator << "<hr>Failed to load sl(2): E and F element of sl(2) have different owners. "
    << "More precisely, the owner of e is "
    << eltE.getOwner()->toString() << " and the owner of f is " << eltF.getOwner()->toString();
  }
  output.theE = eltE;
  output.theF= eltF;
  output.owner = eltE.getOwner();
  DynkinType& theType = output.owner->theWeyl.theDynkinType;
  SemisimpleSubalgebras& ownerSubalgebras =
  calculator.theObjectContainer.getSemisimpleSubalgebrasCreateIfNotPresent(theType);
  output.container = &ownerSubalgebras.theSl2s;
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
  for (int i = 0; i < outputExpressionFormat.theKeys.size; i ++) {
    if (!outputExpressionFormat.theKeys[i].isOfType<std::string>(&keyName)) {
      keyName = outputExpressionFormat.theKeys[i].toString();
    }
    output.setKeyValue(keyName, outputExpressionFormat.theValues[i]);
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
        << input[i].toString() ;
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
  for (int i = 0; i < inputStatementList.children.size; i ++) {
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

bool CalculatorConversions::innerStoreCandidateSubalgebra(
  Calculator& calculator, const CandidateSemisimpleSubalgebra& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerStoreCandidateSubalgebra");
  Expression currentE;
  List<std::string> keys;
  List<Expression> values;
  input.checkBasicInitialization();
  CalculatorConversions::innerExpressionFromDynkinType(
    calculator, input.weylNonEmbedded->theDynkinType, currentE
  );
  keys.addOnTop("DynkinType");
  values.addOnTop(currentE);
  Matrix<Rational> conversionMat;
  conversionMat.assignVectorsToRows(input.theHsScaledToActByTwo);
  currentE.assignMatrix(conversionMat, calculator, nullptr, false);
  keys.addOnTop("ElementsCartan");
  values.addOnTop(currentE);
  if (input.flagSystemSolved) {
    Expression listGenerators;
    listGenerators.makeSequence(calculator);
    for (int i = 0; i < input.theNegGens.size; i ++) {
      CalculatorConversions::innerExpressionFromElementSemisimpleLieAlgebraAlgebraicNumbers(
        calculator, input.theNegGens[i], currentE
      );
      listGenerators.addChildOnTop(currentE);
      CalculatorConversions::innerExpressionFromElementSemisimpleLieAlgebraAlgebraicNumbers(
        calculator, input.thePosGens[i], currentE
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
  ProgressReport theReport;
  std::stringstream reportStream;
  reportStream << "Loading precomputed semisimple subalgebra. ";
  theReport.report(reportStream.str());
  Expression DynkinTypeE, ElementsCartanE, generatorsE;
  if (
    !CalculatorConversions::innerLoadKey(calculator, input, "DynkinType", DynkinTypeE) ||
    !CalculatorConversions::innerLoadKey(calculator, input, "ElementsCartan", ElementsCartanE)
  ) {
    return false;
  }
  reportStream << "Extracted types: " << DynkinTypeE.toString() << ". ";
  theReport.report(reportStream.str());
  outputSubalgebra.owner = &owner;
  DynkinType theNonEmbeddedDynkinType;
  if (!CalculatorConversions::functionDynkinType(
    calculator, DynkinTypeE, theNonEmbeddedDynkinType
  )) {
    return calculator
    << "<hr>Failed to load dynkin type of candidate subalgebra from "
    << DynkinTypeE.toString() << "<hr>";
  }
  reportStream << "Non embedded Dynkin type: " << DynkinTypeE.toString() << ". ";
  theReport.report(reportStream.str());
  outputSubalgebra.weylNonEmbedded = &
  calculator.theObjectContainer.getWeylGroupDataCreateIfNotPresent(theNonEmbeddedDynkinType);
  outputSubalgebra.weylNonEmbedded->makeFromDynkinType(theNonEmbeddedDynkinType);
  int theRank = owner.owner->getRank();
  reportStream << "Extracting matrix of Cartan elements. ";
  theReport.report(reportStream.str());
  Matrix<Rational> theHs;
  if (!calculator.functionGetMatrix(ElementsCartanE, theHs, nullptr, theRank, nullptr)) {
    return calculator << "<hr>Failed to load Cartan elements for candidate subalgebra of type "
    << outputSubalgebra.weylNonEmbedded->theDynkinType << "<hr>";
  }
  if (theHs.numberOfRows != outputSubalgebra.weylNonEmbedded->getDimension()) {
    return calculator << "<hr>Failed to load Cartan elements: I expected "
    << outputSubalgebra.weylNonEmbedded->getDimension() << " elements, but failed to get them.";
  }
  List<int> theRanks, theMults;
  outputSubalgebra.weylNonEmbedded->theDynkinType.getLettersTypesMultiplicities(nullptr, &theRanks, &theMults, nullptr);
  outputSubalgebra.cartanSubalgebrasByComponentScaledToActByTwo.setSize(
    outputSubalgebra.weylNonEmbedded->theDynkinType.getNumberOfSimpleComponents()
  );
  int componentCounter = - 1;
  int counter = - 1;
  for (int i = 0; i < theMults.size; i ++) {
    for (int j = 0; j < theMults[i]; j ++) {
      componentCounter ++;
      Vectors<Rational>& currentComponent = outputSubalgebra.cartanSubalgebrasByComponentScaledToActByTwo[componentCounter];
      currentComponent.setSize(theRanks[i]);
      for (int k = 0; k < theRanks[i]; k ++) {
        counter ++;
        theHs.getVectorFromRow(counter, currentComponent[k]);
      }
    }
  }
  reportStream << "Extracting generators ... ";
  theReport.report(reportStream.str());
  outputSubalgebra.thePosGens.setSize(0);
  outputSubalgebra.theNegGens.setSize(0);
  if (CalculatorConversions::innerLoadKey(calculator, input, "generators", generatorsE)) {
    generatorsE.sequencefy();
    ElementSemisimpleLieAlgebra<AlgebraicNumber> curGenAlgebraic;
    for (int i = 1; i < generatorsE.children.size; i ++) {
      if (!CalculatorConversions::innerLoadElementSemisimpleLieAlgebraAlgebraicNumbers(
        calculator, generatorsE[i], curGenAlgebraic, *owner.owner
      )) {
        return calculator << "<hr>Failed to load semisimple Lie algebra element from expression "
        << generatorsE[i].toString() << ". ";
      }
      if (i % 2 == 1) {
        outputSubalgebra.theNegGens.addOnTop(curGenAlgebraic);
      } else {
        outputSubalgebra.thePosGens.addOnTop(curGenAlgebraic);
      }
    }
    outputSubalgebra.flagSystemProvedToHaveNoSolution = false;
    outputSubalgebra.flagSystemSolved = true;
  } else {
    return calculator << "<hr>Failed to extract subalgebra generators from expression " << input.toString() << ". ";
  }
  SemisimpleLieAlgebra& currentNonEmbededSA =
  owner.theSubalgebrasNonEmbedded->getValueCreateNoInit(outputSubalgebra.weylNonEmbedded->theDynkinType);
  outputSubalgebra.indexNonEmbeddedMeStandard =
  owner.theSubalgebrasNonEmbedded->getIndex(outputSubalgebra.weylNonEmbedded->theDynkinType);
  currentNonEmbededSA.theWeyl.computeRho(true);
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
  Expression theAmbientTypeE, numExploredHsE, numExploredTypesE, theSAsE, currentChainE;
  if (!CalculatorConversions::innerLoadKey(calculator, input, "AmbientDynkinType", theAmbientTypeE)) {
    return calculator << "<hr>Failed to load Dynkin type from: " << input.toString();
  }
  if (!CalculatorConversions::innerLoadKey(calculator, input, "NumExploredHs", numExploredHsE)) {
    return calculator << "<hr>Failed to load numExploredHs list from: " << input.toString();
  }
  if (!CalculatorConversions::innerLoadKey(calculator, input, "NumExploredTypes", numExploredTypesE)) {
    return calculator << "<hr>Failed to load NumExploredTypes list from: " << input.toString();
  }
  if (!CalculatorConversions::innerLoadKey(calculator, input, "Subalgebras", theSAsE)) {
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
  ProgressReport theReport;
  std::stringstream reportStream;
  reportStream << "Extracting semisimple Lie algebra ... ";
  theReport.report(reportStream.str());
  if (!CalculatorConversions::functionSemisimpleLieAlgebra(
    calculator, theAmbientTypeE, tempE, ownerSemisimple
  )) {
    return calculator << "<hr>Error loading semisimple subalgebras: "
    << "failed to extract ambient semisimple Lie algebra. ";
  }
  reportStream << " type: " << ownerSemisimple->theWeyl.theDynkinType.toString() << ". ";
  theReport.report(reportStream.str());

  SemisimpleSubalgebras& theSAs =
  calculator.theObjectContainer.getSemisimpleSubalgebrasCreateIfNotPresent(ownerSemisimple->theWeyl.theDynkinType);
  theSAs.theSubalgebrasNonEmbedded = &calculator.theObjectContainer.semisimpleLieAlgebras;
  theSAs.owner = ownerSemisimple;
  reportStream << " Initializing data structures... ";
  theReport.report(reportStream.str());
  reportStream << " done. Fetching subalgebra list ... ";
  theReport.report(reportStream.str());
  theSAsE.sequencefy();
  theSAs.theSubalgebras.setExpectedSize(theSAsE.children.size - 1);
  theSAs.theSubalgebras.clear();
  theSAs.theSubalgebrasNonEmbedded->setExpectedSize(theSAsE.children.size - 1);
  theSAs.flagAttemptToSolveSystems = true;
  theSAs.flagcomputeModuleDecompositionsition = true;
  theSAs.flagcomputePairingTable = false;
  theSAs.flagComputeNilradicals = false;
  theSAs.millisecondsComputationStart = global.getElapsedMilliseconds();
  reportStream << " done. <br>Total subalgebras: " << theSAsE.children.size - 1 << ". ";
  theReport.report(reportStream.str());

  for (int i = 1; i < theSAsE.children.size; i ++) {
    std::stringstream reportStream2;
    reportStream2 << reportStream.str() << "Subalgebra "
    << i << " is being loaded from expression "
    << theSAsE[i].toString() << ".";
    theReport.report(reportStream2.str());
    CandidateSemisimpleSubalgebra currentCandidate;
    if (!CalculatorConversions::innerCandidateSubalgebraPrecomputed(
      calculator, theSAsE[i], tempE, currentCandidate, theSAs
    )) {
      return calculator
      << "<hr>Error loading candidate subalgebra: failed to load candidate number "
      << i << " extracted from expression: "
      << theSAsE[i].toString() << ". <hr>";
    }

    currentCandidate.checkFullInitialization();
    if (theSAs.theSubalgebras.contains(currentCandidate.theHs)) {
      calculator << "<hr>Did not load subalgebra of type "
      << currentCandidate.weylNonEmbedded->toString()
      << " because I've already loaded a subalgebra with "
      << "the same Cartan subalgebra. ";
      continue;
    }
    theSAs.theSubalgebras.setKeyValue(currentCandidate.theHs, currentCandidate);
    theSAs.theSubalgebras.theValues.lastObject().indexInOwner = theSAs.theSubalgebras.theValues.size - 1;
  }
  reportStream << "Subalgebra loading done, total "
  << theSAs.theSubalgebras.theValues.size << " subalgebras loaded. ";
  theReport.report(reportStream.str());
  theSAs.ToStringExpressionString = CalculatorConversions::innerStringFromSemisimpleSubalgebras;
  if (!theSAs.loadState(currentChainInt, numExploredTypes, numExploredHs, calculator.comments)) {
    return false;
  }
  theSAs.flagAttemptToAdjustCentralizers = false;
  if (!theSAs.findTheSemisimpleSubalgebrasContinue()) {
    std::stringstream out;
    out << "<br>Failed to realize all subalgebras, "
    << "computation aborted. The failure report follows. "
    << theSAs.comments << "<br>The progress report for the "
    << "entire computation follows.<br>"
    << theSAs.ToStringProgressReport();
    return output.assignValue(out.str(), calculator);
  }
  theSAs.millisecondsComputationEnd = global.getElapsedMilliseconds();
  return output.assignValue(theSAs, calculator);
}

std::string CalculatorConversions::innerStringFromSemisimpleSubalgebras(SemisimpleSubalgebras& input) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerStringFromSemisimpleSubalgebras");
  Expression tempE;
  FormatExpressions theFormat;
  CalculatorConversions::innerStoreSemisimpleSubalgebras(global.calculator().getElement(), input, tempE);
  theFormat.flagUseHTML = true;
  return tempE.toString(&theFormat);
}

bool CalculatorConversions::innerStoreSemisimpleSubalgebras(
  Calculator& calculator, const SemisimpleSubalgebras& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerStoreSemisimpleSubalgebras");
  Expression dynkinTypeE;
  List<std::string> theKeys;
  List<Expression> theValues;
  if (!CalculatorConversions::innerExpressionFromDynkinType(
    calculator, input.owner->theWeyl.theDynkinType, dynkinTypeE
  )) {
    return false;
  }
  theKeys.addOnTop("AmbientDynkinType");
  theValues.addOnTop(dynkinTypeE);

  Expression currentChainE, numericalConvertorE(calculator);
  currentChainE.makeSequence(calculator);
  for (int i = 0; i < input.currentSubalgebraChain.size; i ++) {
    numericalConvertorE = input.currentSubalgebraChain[i].indexInOwner;
    currentChainE.addChildOnTop(numericalConvertorE);
  }
  theKeys.addOnTop("CurrentChain");
  theValues.addOnTop(currentChainE);
  Expression numTypesExploredE;
  numTypesExploredE.makeSequence(calculator);
  for (int i = 0; i < input.currentNumLargerTypesExplored.size; i ++) {
    numericalConvertorE = input.currentNumLargerTypesExplored[i];
    numTypesExploredE.addChildOnTop(numericalConvertorE);
  }
  theKeys.addOnTop("NumExploredTypes");
  theValues.addOnTop(numTypesExploredE);
  Expression numHsExploredE;
  numHsExploredE.makeSequence(calculator);
  for (int i = 0; i < input.currentNumHcandidatesExplored.size; i ++) {
    numericalConvertorE = input.currentNumHcandidatesExplored[i];
    numHsExploredE.addChildOnTop(numericalConvertorE);
  }
  theKeys.addOnTop("NumExploredHs");
  theValues.addOnTop(numHsExploredE);
  Expression subalgebrasListE, candidateE;
  subalgebrasListE.makeSequence(calculator);
  subalgebrasListE.children.reserve(input.theSubalgebras.theValues.size + 1);
  for (int i = 0; i < input.theSubalgebras.theValues.size; i ++) {
    if (!CalculatorConversions::innerStoreCandidateSubalgebra(calculator, input.theSubalgebras.theValues[i], candidateE)) {
      return false;
    }
    subalgebrasListE.addChildOnTop(candidateE);
  }
  theKeys.addOnTop("Subalgebras");
  theValues.addOnTop(subalgebrasListE);
  return output.makeSequenceCommands(calculator, theKeys, theValues);
}

bool CalculatorConversions::innerExpressionFromMonomialUE(
  Calculator& calculator,
  const MonomialUniversalEnveloping<RationalFunction<Rational> >& input,
  Expression& output,
  ExpressionContext* inputContext
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromMonomialUE");
  if (input.isConstant()) {
    return output.assignValue(1, calculator);
  }
  ChevalleyGenerator theGen;
  theGen.owner = input.owner;
  Expression chevGenE, powerE, termE;
  List<Expression> theTerms;
  for (int i = 0; i < input.generatorsIndices.size; i ++) {
    theGen.generatorIndex = input.generatorsIndices[i];
    CalculatorConversions::innerExpressionFromChevalleyGenerator(calculator, theGen, chevGenE);
    CalculatorConversions::innerExpressionFromRF(calculator, input.powers[i], powerE, inputContext);
    termE.makeXOX(calculator, calculator.opThePower(), chevGenE, powerE);
    theTerms.addOnTop(termE);
  }
  return output.makeProduct(calculator, theTerms);
}

bool CalculatorConversions::innerExpressionFromUE(
  Calculator& calculator,
  const ElementUniversalEnveloping<RationalFunction<Rational> >& input,
  Expression& output,
  ExpressionContext* inputContext
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromUE");
  LinearCombination<Expression, RationalFunction<Rational> > theUEE;
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
  ChevalleyGenerator theChevGen;
  ElementSemisimpleLieAlgebra<AlgebraicNumber> currentElt;
  theChevGen.owner = &owner;
  output.makeZero();
  ExpressionContext theContext = polyFormE.getContext();
  for (int j = 0; j < polyForm.size(); j ++) {
    const MonomialP& currentMon = polyForm[j];
    int theGenIndex = 0;
    if (!currentMon.isOneLetterFirstDegree(&theGenIndex)) {
      return calculator << "<hr>Failed to convert semisimple Lie algebra input to linear poly: "
      << input.toString() << ".<hr>";
    }
    Expression singleChevGenE = theContext.getVariable(theGenIndex);
    if (!singleChevGenE.startsWith(calculator.opUnderscore(), 3)) {
      return calculator << "<hr>Failed to convert: "
      << singleChevGenE.toString()
      << "(summand of: "
      << input.toString() << ") to Chevalley generator.<hr>";
    }
    std::string theLetter;
    if (
      !singleChevGenE[1].isOperation(&theLetter) ||
      !singleChevGenE[2].isSmallInteger(&theChevGen.generatorIndex)
    ) {
      return calculator << "<hr>Failed to convert summand "
      << singleChevGenE.toString() << " to Chevalley generator of "
      << owner.toStringLieAlgebraName();
    }
    bool isGood = true;
    if (theLetter == "g") {
      theChevGen.generatorIndex = owner.getGeneratorFromDisplayIndex(theChevGen.generatorIndex);
      if (theChevGen.generatorIndex < 0 || theChevGen.generatorIndex >= owner.getNumberOfGenerators()) {
        isGood = false;
      }
      output.addMonomial(theChevGen, polyForm.coefficients[j]);
    } else if (theLetter == "h") {
      int theRootIndex = owner.getRootIndexFromDisplayIndex(theChevGen.generatorIndex);
      if (theRootIndex < 0) {
        isGood = false;
      } else {
        currentElt.makeCartanGenerator(owner.theWeyl.rootSystem[theRootIndex], owner);
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
  ChevalleyGenerator theChevGen;
  theChevGen.owner = &owner;
  ElementUniversalEnveloping<RationalFunction<Rational> > outputUE;
  ElementUniversalEnveloping<RationalFunction<Rational> > currentSummand;
  ElementUniversalEnveloping<RationalFunction<Rational> > currentMultiplicand;
  MonomialP currentMultiplicandRFpartMon;
  Polynomial<Rational> currentPMultiplicand;
  RationalFunction<Rational>  currentMultiplicandRFpart;
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
    const MonomialP& currentMon = theP[j];
    currentSummand.makeConstant(theP.coefficients[j], owner);
    currentMultiplicandRFpartMon.makeOne();
    for (int i = 0; i < currentMon.minimalNumberOfVariables(); i ++) {
      int thePower = - 1;
      if (!currentMon(i).isSmallInteger(&thePower)) {
        return calculator
        << "<hr>Failed to convert one of the exponents appearing in "
        << input[1].toString()
        << " to  a small integer polynomial.<hr>";
      }
      if (thePower == 0) {
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
        !singleChevGenE[1].isSmallInteger(&theChevGen.generatorIndex)
      ) {
        return calculator << "<hr>Failed to convert summand "
        << singleChevGenE.toString() << " to Chevalley generator of "
        << owner.toStringLieAlgebraName();
      }
      bool isGood = true;
      bool isHonestElementUE = true;
      if (theLetter == "g") {
        theChevGen.generatorIndex = owner.getGeneratorFromDisplayIndex(theChevGen.generatorIndex);
        if (theChevGen.generatorIndex < 0 || theChevGen.generatorIndex >= owner.getNumberOfGenerators()) {
          isGood = false;
        }
      } else if (theLetter == "h") {
        if (theChevGen.generatorIndex < 1 || theChevGen.generatorIndex>owner.getRank()) {
          isGood = false;
        } else {
          theChevGen.generatorIndex = theChevGen.generatorIndex +owner.getNumberOfPositiveRoots() - 1;
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
        currentMultiplicand.makeOneGenerator(theChevGen.generatorIndex, owner, 1);
        currentMultiplicand.raiseToPower(thePower);
        currentSummand*= currentMultiplicand;
      } else {
        int variableIndex = polynomialVariables.addNoRepetitionOrReturnIndexFirst(singleChevGenE);
        currentMultiplicandRFpartMon.setVariable(variableIndex, thePower);
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

bool CalculatorConversions::innerExpressionFromBuiltInTypE(
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
  MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromBuiltInType");
  if (input.isOfType<Rational>() || input.isOfType<double>() || input.isOfType<AlgebraicNumber>()) {
    output = input;
    return true;
  }
  if (input.isOfType<Polynomial<Rational> >()) {
    return CalculatorConversions::functionExpressionFromPoly<Rational>(calculator, input, output);
  }
  if (input.isOfType<Polynomial<AlgebraicNumber> >()) {
    return CalculatorConversions::functionExpressionFromPoly<AlgebraicNumber>(calculator, input, output);
  }
  if (input.isOfType<RationalFunction<Rational> >()) {
    return CalculatorConversions::innerExpressionFromRF(calculator, input, output);
  }
  return false;
}

bool CalculatorConversions::innerExpressionFromUE(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromUE");
  ElementUniversalEnveloping<RationalFunction<Rational> > theUE;
  if (!input.isOfType<ElementUniversalEnveloping<RationalFunction<Rational> > >(&theUE)) {
    return calculator << "<hr>Expression " << input.toString()
    << " is not an element of universal enveloping, can't convert to expression";
  }
  return CalculatorConversions::innerExpressionFromUE(calculator, theUE, output);
}

bool CalculatorConversions::innerExpressionFromRF(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromRF");
  if (!input.isOfType<RationalFunction<Rational> >()) {
    return false;
  }
  const RationalFunction<Rational>& theRF = input.getValue<RationalFunction<Rational> >();
  ExpressionContext context = input.getContext();
  return CalculatorConversions::innerExpressionFromRF(
    calculator, theRF, output, &context
  );
}

bool CalculatorConversions::innerExpressionFromRF(
  Calculator& calculator,
  const RationalFunction<Rational>& input,
  Expression& output,
  ExpressionContext* inputContext
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromRF");
  Rational aConst;
  if (input.isConstant(&aConst)) {
    return output.assignValue(aConst, calculator);
  }
  Polynomial<Rational> numP, denP;
  input.getNumerator(numP);

  if (input.isConstant() || input.expressionType == input.typePolynomial) {
    return CalculatorConversions::innerExpressionFromPoly<Rational>(calculator, numP, output, inputContext);
  }
  Expression numE, denE;
  input.getDenominator(denP);
  Polynomial<Rational> numRescaled = numP;
  Polynomial<Rational> denRescaled = denP;
  Rational topMultiple = numRescaled.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  Rational bottomMultiple = denRescaled.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  Rational multipleTopBottom = bottomMultiple / topMultiple;
  numRescaled *= multipleTopBottom.getNumerator();
  denRescaled *= multipleTopBottom.getDenominator();
  CalculatorConversions::innerExpressionFromPoly<Rational>(calculator, numRescaled, numE, inputContext);
  CalculatorConversions::innerExpressionFromPoly<Rational>(calculator, denRescaled, denE, inputContext);
  return output.makeXOX(calculator, calculator.opDivide(), numE, denE);
}

bool CalculatorConversions::innerRationalFunctioN(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerRationalFunctioN");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorConversions::functionRationalFunction(calculator, input[1], output);
}

bool CalculatorConversions::functionRationalFunction(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::functionRationalFunction");
  Expression intermediate(calculator);
  if (
    input.startsWith(calculator.opPlus(), 3) ||
    input.startsWith(calculator.opTimes(), 3) ||
    input.startsWith(calculator.opDivide(), 3)
  ) {
    Expression leftE, rightE;
    if (
      !CalculatorConversions::functionRationalFunction(calculator, input[1], leftE) ||
      !CalculatorConversions::functionRationalFunction(calculator, input[2], rightE)
    ) {
      return calculator << "<hr> Failed to convert " << input[1].toString()
      << " and " << input[2].toString() << " to rational function. ";
    }
    if (leftE.isError() || rightE.isError()) {
      return calculator << "<hr> Conversion of " << input[1].toString()
      << " and " << input[2].toString() << " returned error(s): "
      << leftE.toString() << " and " << rightE.toString();
    }
    intermediate.addChildOnTop(input[0]);
    intermediate.addChildOnTop(leftE);
    intermediate.addChildOnTop(rightE);
    if (input.startsWith(calculator.opPlus())) {
      return CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrRFToRatOrPolyOrRF(
        calculator, intermediate, output
      );
    }
    if (input.startsWith(calculator.opTimes())) {
      return CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF(
        calculator, intermediate, output
      );
    }
    if (input.startsWith(calculator.opDivide())) {
      return CalculatorFunctionsBinaryOps::innerDivideRationalFunctionOrPolynomialOrRationalByRationalFunctionOrPolynomial(
        calculator, intermediate, output
      );
    }
    global.fatal << "This line of code should never be reached, something has gone wrong." << global.fatal;
  }
  int theSmallPower = - 1;
  if (input.startsWith(calculator.opThePower(), 3) ) {
    if (input[2].isSmallInteger(&theSmallPower)) {
      Expression leftE;
      if (!CalculatorConversions::functionRationalFunction(calculator, input[1], leftE)) {
        return calculator << "<hr>CalculatorConversions::innerRationalFunction: failed to convert "
        << input[1].toString() << " to rational function. ";
      }
      if (leftE.isError()) {
        return calculator << "<hr> Conversion of " << input[1].toString() << " returned error: " << leftE.toString();
      }
      RationalFunction<Rational>  theRF = leftE.getValue<RationalFunction<Rational> >();
      theRF.raiseToPower(theSmallPower);
      return output.assignValueWithContext(theRF, leftE.getContext(), calculator);
    }
    calculator << "<hr>Warning: failed to raise "
    << input[1].toString() << " to power " << input[2].toString()
    << ": failed to convert the power to small integer. "
    << "I am treating " << input.toString()
    << " as a single variable: please make sure that is what you want.";
  }
  if (input.isOfType<RationalFunction<Rational> >()) {
    output = input;
    return true;
  }
  if (input.isOfType<Polynomial<Rational> >() || input.isOfType<Rational>()) {
    return input.convertInternally<RationalFunction<Rational> >(output);
  }
  if (input.isOfType<AlgebraicNumber>()) {
    AlgebraicNumber theNumber = input.getValue<AlgebraicNumber>();
    Rational theRat;
    if (theNumber.isRational(&theRat)) {
      Expression tempE;
      tempE.assignValue(theRat, calculator);
      return tempE.convertInternally<RationalFunction<Rational> > (output);
    }
  }
  ExpressionContext theContext(calculator);
  theContext.makeOneVariable(input);
  RationalFunction<Rational> theRF;
  theRF.makeOneLetterMonomial(0, 1);
  return output.assignValueWithContext(theRF, theContext, calculator);
}

bool CalculatorConversions::functionMatrixRational(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::functionMatrixRational");
  Matrix<Rational> outputMat;
  if (!calculator.functionGetMatrix(input, outputMat, nullptr, - 1, nullptr)) {
    return calculator << "<br>Failed to get matrix of rationals. ";
  }
  return output.assignMatrix(outputMat, calculator);
}

bool CalculatorConversions::innerMatrixRationalTensorForM(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerMatrixRationalTensorForM");
  if (!CalculatorConversions::innerMakeMatrix(calculator, input, output)) {
    return calculator << "Failed to extract matrix of rationals. ";
  }
  Matrix<Rational> matRat;
  if (!calculator.functionGetMatrix(output, matRat)) {
    return false;
  }
  MatrixTensor<Rational> outputMatTensor;
  outputMatTensor = matRat;
  return output.assignValue(outputMatTensor, calculator);
}

bool CalculatorConversions::functionMatrixRationalTensorForm(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::functionMatrixRationalTensorForm");
  Matrix<Rational> matRat;
  if (!calculator.functionGetMatrix(input, matRat)) {
    return false;
  }
  MatrixTensor<Rational> outputMatTensor;
  outputMatTensor = matRat;
  return output.assignValue(outputMatTensor, calculator);
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
  if (input[0][0].theData != calculator.opMatriX()) {
    return false;
  }
  Matrix<Expression> theMatrix;
  if (!calculator.getMatrixExpressionsFromArguments(input, theMatrix)) {
    return false;
  }
  Expression conversionAttempt;
  conversionAttempt.assignMatrixExpressions(theMatrix, calculator, true, false);
  if (!conversionAttempt.isMatrix()) {
    output = conversionAttempt;
    return true;
  }
  if (conversionAttempt.size() >= 1) {
    if (conversionAttempt[0].startsWith(calculator.opMatriX())) {
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
  Matrix<Expression> outMat;
  if (calculator.getMatrixExpressionsFromArguments(input, outMat)) {
    return output.assignMatrixExpressions(outMat, calculator, true, true);
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
  Matrix<RationalFunction<Rational> > outputMat;
  ExpressionContext context(calculator);
  if (!calculator.functionGetMatrix(
    input,
    outputMat,
    &context,
    - 1,
    CalculatorConversions::functionRationalFunction
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
  if (!FileOperations::loadFiletoStringVirtual(
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
  ElementHyperoctahedralGroupR2 theElement;
  if (input.isOfType<std::string>(&inputStringFormat)) {
    theElement.makeFromString(inputStringFormat);
    return output.assignValue(theElement, calculator);
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
  theElement.h.addCycle(oneCycle);
  for (int i = 2; i < input.size(); i ++) {
    if (input[i].isEqualToOne()) {
      theElement.k.toggleBit(i - 2);
    } else if (!input[i].isEqualToZero()) {
      return calculator << "Your input: " << input.toString()
      << " had bit values that were not ones and zeroes.";
    }
  }
  return output.assignValue(theElement, calculator);
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
  if (!calculator.convert(
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
