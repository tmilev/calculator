// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".#include "calculator_interface.h"
#include "math_extra_semisimple_Lie_algebras_implementation.h"
#include "calculator_interface.h"
#include "calculator_inner_typed_functions.h"
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_extra_universal_enveloping_implementation.h" // undefined reference to `ElementUniversalEnveloping<RationalFunctionOld>::MakeZero(SemisimpleLieAlgebra&)'

template <>
bool Expression::ConvertInternally<RationalFunction>(Expression& output) const;

bool CalculatorConversions::innerExpressionFromChevalleyGenerator(
  Calculator& theCommands, const ChevalleyGenerator& input, Expression& output
) {
  MacroRegisterFunctionWithName("innerExpressionFromChevalleyGenerator");
  input.CheckInitialization();
  output.reset(theCommands, 2);
  Expression generatorLetterE, generatorIndexE;
  if (
    input.theGeneratorIndex >= input.owner->GetNumPosRoots() &&
    input.theGeneratorIndex < input.owner->GetNumPosRoots() + input.owner->GetRank()
  ) {
    generatorLetterE.MakeAtom(theCommands.AddOperationNoRepetitionOrReturnIndexFirst("h"), theCommands);
  } else {
    generatorLetterE.MakeAtom(theCommands.AddOperationNoRepetitionOrReturnIndexFirst("g"), theCommands);
  }
  generatorIndexE.AssignValue(input.owner->GetDisplayIndexFromGenerator(input.theGeneratorIndex), theCommands);
  return output.MakeXOX(theCommands, theCommands.opUnderscore(), generatorLetterE, generatorIndexE);
}

bool CalculatorConversions::functionSemisimpleLieAlgebra(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::functionSemisimpleLieAlgebra");
  SemisimpleLieAlgebra* dummySA;
  return CalculatorConversions::functionSemisimpleLieAlgebra(
    theCommands, input, output, dummySA
  );
}

bool CalculatorConversions::innerSemisimpleLieAlgebra(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerSemisimpleLieAlgebra");
  if (input.size() != 2) {
    return theCommands << "Semisimple Lie algebra expects a single argument. ";
  }
  SemisimpleLieAlgebra* dummySA;
  return CalculatorConversions::functionSemisimpleLieAlgebra(
    theCommands, input[1], output, dummySA
  );
}

bool CalculatorConversions::innerLoadWeylGroup(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerLoadWeylGroup");
  DynkinType theType;
  if (!CalculatorConversions::innerDynkinTypE(theCommands, input, theType)) {
    return false;
  }
  SemisimpleLieAlgebra& theSA = theCommands.theObjectContainer.GetLieAlgebraCreateIfNotPresent(theType);
  return output.AssignValue(theSA.theWeyl, theCommands);
}

bool CalculatorConversions::innerDynkinSimpleTypE(
  Calculator& theCommands, const Expression& input, DynkinSimpleType& outputMon
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerDynkinSimpleTypE");
  if (input.size() != 2) {
    return theCommands << "Dynkin simple type function requires a single argument. ";
  }
  return CalculatorConversions::functionDynkinSimpleType(theCommands, input[1], outputMon);
}

bool CalculatorConversions::functionDynkinSimpleType(
  Calculator& theCommands, const Expression& input, DynkinSimpleType& outputMon
) {
  MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::DeSerializeMon_DynkinSimpleType");
  Expression rankE, typeLetterE, scaleE;
  if (input.StartsWith(theCommands.opUnderscore(), 3)) {
    rankE = input[2];
    typeLetterE = input[1];
    if (typeLetterE.StartsWith(theCommands.opThePower(),3)) {
      scaleE = typeLetterE[2];
      typeLetterE = typeLetterE[1];
    } else {
      scaleE.AssignValue(1, theCommands);
    }
  } else if (input.StartsWith(theCommands.opThePower(), 3)) {
    scaleE = input[2];
    if (!input[1].StartsWith(theCommands.opUnderscore(), 3)) {
      return theCommands << "<hr>Failed to extract rank, type from "
      << input[1].toString()
      << ". The expression does not have two children.";
    }
    rankE = input[1][2];
    typeLetterE = input[1][1];
  } else {
    return theCommands << "<hr>Failed to extract rank, type, "
    << "first co-root length from: " << input.toString() << ". ";
  }
  Rational theScale;
  if (!scaleE.IsOfType<Rational>(&theScale)) {
    return theCommands << "<hr>Failed to extract first co-root length: "
    << "expression " << scaleE.toString()
    << " is not a rational number.";
  }
  if (theScale <= 0) {
    return theCommands << "<hr>Couldn't extract first co-root length: " << theScale.toString() << " is non-positive.";
  }
  std::string theTypeName;
  if (!typeLetterE.IsOperation(&theTypeName)) {
    return theCommands << "I couldn't extract a type letter from " << typeLetterE.toString();
  }
  if (theTypeName.size() != 1) {
    return theCommands << "<hr>Error while extracting Dynkin simple type: "
    << "the type of a simple Lie algebra must be the letter A, B, C, D, E, F or G."
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
    return theCommands << "The type of a simple Lie algebra must be "
    << "the letter A, B, C, D, E, F or G; error while processing " << input.toString();
  }
  int theRank;
  if (!rankE.IsSmallInteger(&theRank)) {
    return theCommands << "I wasn't able to extract rank from " << input.toString();
  }
  if (theRank < 1 || theRank > 20) {
    return theCommands << "<hr>The rank of a simple Lie algebra "
    << "must be between 1 and 20; error while processing " << input.toString();
  }
  if (theWeylLetter == 'E' && (theRank > 8 || theRank < 3)) {
    return theCommands << "<hr>Type E must have rank 6, 7 or 8 ";
  }
  if (theWeylLetter == 'D' && (theRank < 3)) {
    return theCommands << "<hr>Type D is expected to have rank 4 or more, "
    << "your input was of rank " << theRank << ". ";
  }
  outputMon.MakeArbitrary(theWeylLetter, theRank, theScale);
  return true;
}

bool CalculatorConversions::innerDynkinTypE(Calculator& theCommands, const Expression& input, DynkinType& output) {
  MacroRegisterFunctionWithName("Calculator::innerDynkinTypE");
  RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  if (input.size() != 2) {
    return theCommands << "Dynkin type takes as input one argument. ";
  }
  return CalculatorConversions::functionDynkinType(theCommands, input[1], output);
}

bool CalculatorConversions::functionDynkinType(
  Calculator& theCommands, const Expression& input, DynkinType& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::functionDynkinType");
  LinearCombination<Expression, Rational> theType;
  if (!theCommands.functionCollectSummands(theCommands, input, theType)) {
    return false;
  }
  DynkinSimpleType simpleComponent;
  output.MakeZero();
  for (int i = 0; i < theType.size(); i ++) {
    if (!CalculatorConversions::functionDynkinSimpleType(
      theCommands, theType[i], simpleComponent
    )) {
      return false;
    }
    int theMultiplicity = - 1;
    if (!theType.coefficients[i].IsSmallInteger(&theMultiplicity)) {
      theMultiplicity = - 1;
    }
    if (theMultiplicity < 0) {
      return theCommands << "<hr>Failed to convert the coefficient "
      << theType.coefficients[i] << " of " << theType[i].toString()
      << " to a small positive integer. ";
    }
    output.AddMonomial(simpleComponent, theMultiplicity);
  }
  return !theType.IsEqualToZero();
}

bool CalculatorConversions::functionSemisimpleLieAlgebra(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  SemisimpleLieAlgebra*& outputPointer
) {
  RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("Calculator::functionSemisimpleLieAlgebra");
  DynkinType theDynkinType;
  outputPointer = nullptr;
  if (!CalculatorConversions::functionDynkinType(
    theCommands, input, theDynkinType
  )) {
    return theCommands << "Failed to extract Dynkin type from: " << input.toString();
  }
  if (theDynkinType.GetRank() > 20) {
    return theCommands
    << "I have been instructed to allow semisimple Lie algebras of rank 20 maximum. "
    << "If you would like to relax this limitation edit file " << __FILE__
    << " line " << __LINE__ << ". Note that the Chevalley "
    << "constant computation Reserves a dim(g)*dim(g) "
    << "table of RAM memory, which means the RAM memory rises with the 4^th power of rank(g). "
    << "You have been warned. Alternatively, you may want to implement a sparse structure constant table "
    << "(write me an email if you want to do that, I will help you). ";
  }
  bool newlyCreated = !theCommands.theObjectContainer.semisimpleLieAlgebras.Contains(theDynkinType);
  outputPointer = &theCommands.theObjectContainer.GetLieAlgebraCreateIfNotPresent(theDynkinType);
  outputPointer->CheckConsistency();
  output.AssignValue(outputPointer, theCommands);
  if (newlyCreated) {
    outputPointer->ComputeChevalleyConstants();
    Expression tempE;
    theCommands.functionWriteToHDOrPrintSSLieAlgebra(
      theCommands, output, tempE, false, false
    );
    theCommands << tempE.GetValue<std::string>();
  }
  return true;
}

bool CalculatorConversions::innerExpressionFromDynkinSimpleType(
  Calculator& theCommands, const DynkinSimpleType& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::DynkinSimpleType");
  Expression letterE, rankE, letterAndIndexE, indexE;
  std::string letterS;
  letterS = input.theLetter;
  letterE.MakeAtom(theCommands.AddOperationNoRepetitionOrReturnIndexFirst(letterS), theCommands);
  indexE.AssignValue(input.CartanSymmetricInverseScale, theCommands);
  rankE.AssignValue(input.theRank, theCommands);
  letterAndIndexE.MakeXOX(theCommands, theCommands.opThePower(), letterE, indexE);
  return output.MakeXOX(theCommands, theCommands.opUnderscore(), letterAndIndexE, rankE);
}

template < >
SemisimpleLieAlgebra*& Expression::GetValueNonConst() const;

bool CalculatorConversions::innerStoreSemisimpleLieAlgebra(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  if (!input.IsOfType<SemisimpleLieAlgebra*>()) {
    return output.MakeError("Asking to store non-semisimple Lie algebra as such is not allowed. ", theCommands);
  }
  SemisimpleLieAlgebra* owner = input.GetValueNonConst<SemisimpleLieAlgebra*>();
  if (owner == nullptr) {
    global.fatal << "Unexpected null pointer" << global.fatal;
  }
  return CalculatorConversions::innerExpressionFromDynkinType(theCommands, owner->theWeyl.theDynkinType, output);
}

bool CalculatorConversions::innerExpressionFromElementSemisimpleLieAlgebraRationals(
  Calculator& theCommands, const ElementSemisimpleLieAlgebra<Rational>& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerStoreElementSemisimpleLieAlgebraRational");
  LinearCombination<Expression, Rational> theMons;
  theMons.MakeZero();
  Expression currentMon;
  for (int i = 0; i < input.size(); i ++) {
    CalculatorConversions::innerExpressionFromChevalleyGenerator(theCommands, input[i], currentMon);
    theMons.AddMonomial(currentMon, input.coefficients[i]);
  }
  return output.makeSum(theCommands, theMons);
}

bool CalculatorConversions::innerExpressionFromDynkinType(
  Calculator& theCommands, const DynkinType& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromDynkinType");
  LinearCombination<Expression, AlgebraicNumber> theMons;
  theMons.MakeZero();
  Expression currentMon;
  for (int i = 0; i < input.size(); i ++) {
    CalculatorConversions::innerExpressionFromDynkinSimpleType(theCommands, input[i], currentMon);
    theMons.AddMonomial(currentMon, input.coefficients[i]);
  }
  return output.makeSum(theCommands, theMons);
}

bool CalculatorConversions::innerExpressionFromElementSemisimpleLieAlgebraAlgebraicNumbers(
  Calculator& theCommands, const ElementSemisimpleLieAlgebra<AlgebraicNumber>& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromElementSemisimpleLieAlgebraAlgebraicNumbers");
  LinearCombination<Expression, AlgebraicNumber> theMons;
  theMons.MakeZero();
  Expression currentMon;
  for (int i = 0; i < input.size(); i ++) {
    CalculatorConversions::innerExpressionFromChevalleyGenerator(theCommands, input[i], currentMon);
    theMons.AddMonomial(currentMon, input.coefficients[i]);
  }
  return output.makeSum(theCommands, theMons);
}

bool CalculatorConversions::innerSlTwoSubalgebraPrecomputed(
  Calculator& theCommands, const Expression& input, slTwoSubalgebra& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerLoadFromObject slTwoSubalgebra");
  if (!input.IsListNElements(4)) {
    return theCommands << "<hr>input of innerLoadFromObject has " << input.children.size << " children, 4 expected. ";
  }
  const Expression& theOwnerE = input[1];
  Expression tempE;
  if (!CalculatorConversions::functionSemisimpleLieAlgebra(
    theCommands, theOwnerE, tempE, output.owner
  )) {
    return theCommands << "<hr>Failed to extract semisimple Lie algebra from " << theOwnerE.toString()
    << " while extracting its sl(2) subalgebra.";
  }
  const Expression& theF = input[2];
  const Expression& theE = input[3];
  ElementSemisimpleLieAlgebra<Rational> eltF, eltE;
  if (!CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs(theCommands, theF, eltF, *output.owner)) {
    return theCommands << "<hr>Failed to extract f element while loading sl(2) subalgebra<hr>";
  }
  if (!CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs(theCommands, theE, eltE, *output.owner)) {
    return theCommands << "<hr>Failed to extract e element while loading sl(2) subalgebra<hr>";
  }
  if (eltE.IsEqualToZero() || eltF.IsEqualToZero()) {
    return theCommands << "<hr>Failed to load sl(2) subalgebra: either e or f is equal to zero. e and f are: "
    << eltE.toString()
    << ", " << eltF.toString() << ". ";
  }
  if (eltE.GetOwner() != eltF.GetOwner()) {
    return theCommands << "<hr>Failed to load sl(2): E and F element of sl(2) have different owners. "
    << "More precisely, the owner of e is "
    << eltE.GetOwner()->toString() << " and the owner of f is " << eltF.GetOwner()->toString();
  }
  output.theE = eltE;
  output.theF= eltF;
  output.owner = eltE.GetOwner();
  DynkinType& theType = output.owner->theWeyl.theDynkinType;
  SemisimpleSubalgebras& ownerSubalgebras =
  theCommands.theObjectContainer.GetSemisimpleSubalgebrasCreateIfNotPresent(theType);
  output.container = &ownerSubalgebras.theSl2s;
  return true;
}

bool CalculatorConversions::innerSlTwoSubalgebraPrecomputed(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerSlTwoSubalgebraPrecomputed");
  slTwoSubalgebra tempSL2;
  if (!CalculatorConversions::innerSlTwoSubalgebraPrecomputed(theCommands, input, tempSL2)) {
    return theCommands << "<hr>Failed to load sl(2) subalgebra. ";
  }
  return output.AssignValue(tempSL2.toString(), theCommands);
}

bool CalculatorConversions::innerAlgebraicNumber(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerAlgebraicNumber");
  if (input.IsOfType<AlgebraicNumber>()) {
    output = input;
    return true;
  }
  if (input.IsOfType<Rational>()) {
    AlgebraicNumber theNum;
    theNum = input.GetValue<Rational>();
    return output.AssignValue(theNum, theCommands);
  }
  return false;
}

bool CalculatorConversions::innerLoadKeysFromStatementLisT(
  Calculator& theCommands,
  const Expression& input,
  MapList<std::string, Expression, MathRoutines::HashString>& output,
  std::stringstream* commentsOnFailure,
  bool allowFailure
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerLoadKeysFromStatementLisT");
  MapList<Expression, Expression> outputExpressionFormat;
  if (!CalculatorConversions::innerLoadKeysFromStatementLisT(
    theCommands, input, outputExpressionFormat, commentsOnFailure, allowFailure
  )) {
    return false;
  }
  output.Clear();
  std::string keyName;
  for (int i = 0; i < outputExpressionFormat.theKeys.size; i ++) {
    if (!outputExpressionFormat.theKeys[i].IsOfType<std::string>(&keyName)) {
      keyName = outputExpressionFormat.theKeys[i].toString();
    }
    output.SetKeyValue(keyName, outputExpressionFormat.theValues[i]);
  }
  return true;
}

bool CalculatorConversions::innerLoadKeysFromStatementLisT(
  Calculator& theCommands,
  const Expression& input,
  MapList<Expression, Expression>& output,
  std::stringstream* commentsOnFailure,
  bool allowFailure
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerLoadKeysFromStatementLisT");
  output.Clear();
  for (int i = 1; i < input.size(); i ++) {
    if (input[i].StartsWith(theCommands.opDefine(), 3)) {
      output.SetKeyValue(input[i][1], input[i][2]);
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
  Calculator& theCommands,
  const Expression& inputStatementList,
  const std::string& inputKey,
  Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerLoadKey");
  Expression theKeyE;
  theKeyE.MakeAtom(inputKey, theCommands);
  for (int i = 0; i < inputStatementList.children.size; i ++) {
    if (inputStatementList[i].StartsWith(theCommands.opDefine(), 3)) {
      if (inputStatementList[i][1] == theKeyE) {
        output = inputStatementList[i][2];
        return true;
      }
    }
  }
  return theCommands << "<hr>Key " << inputKey << " not found in expression "
  << inputStatementList.toString() << ".";
}

bool CalculatorConversions::innerStoreCandidateSA(
  Calculator& theCommands, const CandidateSSSubalgebra& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerStoreCandidateSA");
  Expression currentE;
  List<std::string> keys;
  List<Expression> values;
  input.CheckBasicInitialization();
  CalculatorConversions::innerExpressionFromDynkinType(
    theCommands, input.theWeylNonEmbedded->theDynkinType, currentE
  );
  keys.AddOnTop("DynkinType");
  values.AddOnTop(currentE);
  Matrix<Rational> conversionMat;
  conversionMat.AssignVectorsToRows(input.theHsScaledToActByTwo);
  currentE.AssignMatrix(conversionMat, theCommands, nullptr, false);
  keys.AddOnTop("ElementsCartan");
  values.AddOnTop(currentE);
  if (input.flagSystemSolved) {
    Expression listGenerators;
    listGenerators.MakeSequence(theCommands);
    for (int i = 0; i < input.theNegGens.size; i ++) {
      CalculatorConversions::innerExpressionFromElementSemisimpleLieAlgebraAlgebraicNumbers(
        theCommands, input.theNegGens[i], currentE
      );
      listGenerators.AddChildOnTop(currentE);
      CalculatorConversions::innerExpressionFromElementSemisimpleLieAlgebraAlgebraicNumbers(
        theCommands, input.thePosGens[i], currentE
      );
      listGenerators.AddChildOnTop(currentE);
    }
    keys.AddOnTop("generators");
    values.AddOnTop(listGenerators);
  }
  return output.MakeSequenceCommands(theCommands, keys, values);
}

bool CalculatorConversions::innerCandidateSAPrecomputed(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  CandidateSSSubalgebra& outputSubalgebra,
  SemisimpleSubalgebras& owner
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerCandidateSAPrecomputed");
  owner.CheckInitialization();
  ProgressReport theReport;
  std::stringstream reportStream;
  reportStream << "Loading precomputed semisimple subalgebra. ";
  theReport.Report(reportStream.str());
  Expression DynkinTypeE, ElementsCartanE, generatorsE;
  if (
    !CalculatorConversions::innerLoadKey(theCommands, input, "DynkinType", DynkinTypeE) ||
    !CalculatorConversions::innerLoadKey(theCommands, input, "ElementsCartan", ElementsCartanE)
  ) {
    return false;
  }
  reportStream << "Extracted types: " << DynkinTypeE.toString() << ". ";
  theReport.Report(reportStream.str());
  outputSubalgebra.owner = &owner;
  DynkinType theNonEmbeddedDynkinType;
  if (!CalculatorConversions::functionDynkinType(
    theCommands, DynkinTypeE, theNonEmbeddedDynkinType
  )) {
    return theCommands
    << "<hr>Failed to load dynkin type of candidate subalgebra from "
    << DynkinTypeE.toString() << "<hr>";
  }
  reportStream << "Non embedded Dynkin type: " << DynkinTypeE.toString() << ". ";
  theReport.Report(reportStream.str());
  outputSubalgebra.theWeylNonEmbedded = &
  theCommands.theObjectContainer.GetWeylGroupDataCreateIfNotPresent(theNonEmbeddedDynkinType);
  outputSubalgebra.theWeylNonEmbedded->MakeFromDynkinType(theNonEmbeddedDynkinType);
  int theRank = owner.owner->GetRank();
  reportStream << "Extracting matrix of Cartan elements. ";
  theReport.Report(reportStream.str());
  Matrix<Rational> theHs;
  if (!theCommands.functionGetMatrix(ElementsCartanE, theHs, nullptr, theRank, nullptr)) {
    return theCommands << "<hr>Failed to load Cartan elements for candidate subalgebra of type "
    << outputSubalgebra.theWeylNonEmbedded->theDynkinType << "<hr>";
  }
  if (theHs.NumRows != outputSubalgebra.theWeylNonEmbedded->GetDim()) {
    return theCommands << "<hr>Failed to load Cartan elements: I expected "
    << outputSubalgebra.theWeylNonEmbedded->GetDim() << " elements, but failed to get them.";
  }
  List<int> theRanks, theMults;
  outputSubalgebra.theWeylNonEmbedded->theDynkinType.GetLettersTypesMults(nullptr, &theRanks, &theMults, nullptr);
  outputSubalgebra.CartanSAsByComponentScaledToActByTwo.SetSize(
    outputSubalgebra.theWeylNonEmbedded->theDynkinType.GetNumSimpleComponents()
  );
  int componentCounter = - 1;
  int counter = - 1;
  for (int i = 0; i < theMults.size; i ++) {
    for (int j = 0; j < theMults[i]; j ++) {
      componentCounter ++;
      Vectors<Rational>& currentComponent = outputSubalgebra.CartanSAsByComponentScaledToActByTwo[componentCounter];
      currentComponent.SetSize(theRanks[i]);
      for (int k = 0; k < theRanks[i]; k ++) {
        counter ++;
        theHs.GetVectorFromRow(counter, currentComponent[k]);
      }
    }
  }
  reportStream << "Extracting generators ... ";
  theReport.Report(reportStream.str());
  outputSubalgebra.thePosGens.SetSize(0);
  outputSubalgebra.theNegGens.SetSize(0);
  if (CalculatorConversions::innerLoadKey(theCommands, input, "generators", generatorsE)) {
    generatorsE.Sequencefy();
    ElementSemisimpleLieAlgebra<AlgebraicNumber> curGenAlgebraic;
    for (int i = 1; i < generatorsE.children.size; i ++) {
      if (!CalculatorConversions::innerLoadElementSemisimpleLieAlgebraAlgebraicNumbers(
        theCommands, generatorsE[i], curGenAlgebraic, *owner.owner
      )) {
        return theCommands << "<hr>Failed to load semisimple Lie algebra element from expression "
        << generatorsE[i].toString() << ". ";
      }
      if (i % 2 == 1) {
        outputSubalgebra.theNegGens.AddOnTop(curGenAlgebraic);
      } else {
        outputSubalgebra.thePosGens.AddOnTop(curGenAlgebraic);
      }
    }
    outputSubalgebra.flagSystemProvedToHaveNoSolution = false;
    outputSubalgebra.flagSystemSolved = true;
  } else {
    return theCommands << "<hr>Failed to extract subalgebra generators from expression " << input.toString() << ". ";
  }
  SemisimpleLieAlgebra& currentNonEmbededSA =
  owner.theSubalgebrasNonEmbedded->GetValueCreateNoInit(outputSubalgebra.theWeylNonEmbedded->theDynkinType);
  outputSubalgebra.indexNonEmbeddedMeStandard =
  owner.theSubalgebrasNonEmbedded->GetIndex(outputSubalgebra.theWeylNonEmbedded->theDynkinType);
  currentNonEmbededSA.theWeyl.ComputeRho(true);
  outputSubalgebra.theWeylNonEmbedded->ComputeRho(true); //<- this line may be unnecessary, the
  //two Weyl groups may coincide depending on some implementation decisions I am about to take
  //some time soon.
  outputSubalgebra.ComputeHsAndHsScaledToActByTwoFromComponents();
  outputSubalgebra.flagSubalgebraPreloadedButNotVerified = true;
  return output.MakeError(
    "Candidate subalgebra is not a stand-alone object and its Expression output should not be used. ",
    theCommands
  );
}

bool CalculatorConversions::innerLoadSemisimpleSubalgebras(
  Calculator& theCommands, const Expression& inpuT, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerLoadSemisimpleSubalgebras");
  Expression input = inpuT;
  Expression theAmbientTypeE, numExploredHsE, numExploredTypesE, theSAsE, currentChainE;
  if (!CalculatorConversions::innerLoadKey(theCommands, input, "AmbientDynkinType", theAmbientTypeE)) {
    return theCommands << "<hr>Failed to load Dynkin type from: " << input.toString();
  }
  if (!CalculatorConversions::innerLoadKey(theCommands, input, "NumExploredHs", numExploredHsE)) {
    return theCommands << "<hr>Failed to load numExploredHs list from: " << input.toString();
  }
  if (!CalculatorConversions::innerLoadKey(theCommands, input, "NumExploredTypes", numExploredTypesE)) {
    return theCommands << "<hr>Failed to load NumExploredTypes list from: " << input.toString();
  }
  if (!CalculatorConversions::innerLoadKey(theCommands, input, "Subalgebras", theSAsE)) {
    return theCommands << "<hr>Failed to load Subalgebras list from: " << input.toString();
  }
  if (!CalculatorConversions::innerLoadKey(theCommands, input, "CurrentChain", currentChainE)) {
    return theCommands << "<hr>Failed to load CurrentChain from: " << input.toString();
  }
  List<int> currentChainInt, numExploredTypes, numExploredHs;
  if (!theCommands.GetVectoRInt(currentChainE, currentChainInt)) {
    return false;
  }
  if (!theCommands.GetVectoRInt(numExploredHsE, numExploredHs)) {
    return false;
  }
  if (!theCommands.GetVectoRInt(numExploredTypesE, numExploredTypes)) {
    return false;
  }
  SemisimpleLieAlgebra* ownerSemisimple = nullptr;
  Expression tempE;
  ProgressReport theReport;
  std::stringstream reportStream;
  reportStream << "Extracting semisimple Lie algebra ... ";
  theReport.Report(reportStream.str());
  if (!CalculatorConversions::functionSemisimpleLieAlgebra(
    theCommands, theAmbientTypeE, tempE, ownerSemisimple
  )) {
    return theCommands << "<hr>Error loading semisimple subalgebras: "
    << "failed to extract ambient semisimple Lie algebra. ";
  }
  reportStream << " type: " << ownerSemisimple->theWeyl.theDynkinType.toString() << ". ";
  theReport.Report(reportStream.str());

  SemisimpleSubalgebras& theSAs =
  theCommands.theObjectContainer.GetSemisimpleSubalgebrasCreateIfNotPresent(ownerSemisimple->theWeyl.theDynkinType);
  theSAs.theSubalgebrasNonEmbedded = &theCommands.theObjectContainer.semisimpleLieAlgebras;
  theSAs.owner = ownerSemisimple;
  reportStream << " Initializing data structures... ";
  theReport.Report(reportStream.str());
  reportStream << " done. Fetching subalgebra list ... ";
  theReport.Report(reportStream.str());
  theSAsE.Sequencefy();
  theSAs.theSubalgebras.SetExpectedSize(theSAsE.children.size - 1);
  theSAs.theSubalgebras.Clear();
  theSAs.theSubalgebrasNonEmbedded->SetExpectedSize(theSAsE.children.size - 1);
  theSAs.flagAttemptToSolveSystems = true;
  theSAs.flagComputeModuleDecomposition = true;
  theSAs.flagComputePairingTable = false;
  theSAs.flagComputeNilradicals = false;
  theReport.Report("Got to here pt 3");
  theSAs.millisecondsComputationStart = global.GetElapsedMilliseconds();
  reportStream << " done. <br>Total subalgebras: " << theSAsE.children.size - 1 << ". ";
  theReport.Report(reportStream.str());

  for (int i = 1; i < theSAsE.children.size; i ++) {
    std::stringstream reportStream2;
    reportStream2 << reportStream.str() << "Subalgebra "
    << i << " is being loaded from expression "
    << theSAsE[i].toString() << ".";
    theReport.Report(reportStream2.str());
    CandidateSSSubalgebra currentCandidate;
    if (!CalculatorConversions::innerCandidateSAPrecomputed(
      theCommands, theSAsE[i], tempE, currentCandidate, theSAs
    )) {
      return theCommands
      << "<hr>Error loading candidate subalgebra: failed to load candidate number "
      << i << " extracted from expression: "
      << theSAsE[i].toString() << ". <hr>";
    }

    currentCandidate.CheckFullInitializatioN();
    if (theSAs.theSubalgebras.Contains(currentCandidate.theHs)) {
      theCommands << "<hr>Did not load subalgebra of type "
      << currentCandidate.theWeylNonEmbedded->toString()
      << " because I've already loaded a subalgebra with "
      << "the same Cartan subalgebra. ";
      continue;
    }
    theSAs.theSubalgebras.SetKeyValue(currentCandidate.theHs, currentCandidate);
    theSAs.theSubalgebras.theValues.LastObject().indexInOwner = theSAs.theSubalgebras.theValues.size - 1;
  }
  reportStream << "Subalgebra loading done, total "
  << theSAs.theSubalgebras.theValues.size << " subalgebras loaded. ";
  theReport.Report(reportStream.str());
  theSAs.ToStringExpressionString = CalculatorConversions::innerStringFromSemisimpleSubalgebras;
  if (!theSAs.LoadState(currentChainInt, numExploredTypes, numExploredHs, theCommands.Comments)) {
    return false;
  }
  theSAs.flagAttemptToAdjustCentralizers = false;
  if (!theSAs.FindTheSSSubalgebrasContinue()) {
    std::stringstream out;
    out << "<br>Failed to realize all subalgebras, "
    << "computation aborted. The failure report follows. "
    << theSAs.comments << "<br>The progress report for the "
    << "entire computation follows.<br>"
    << theSAs.ToStringProgressReport();
    return output.AssignValue(out.str(), theCommands);
  }
  theSAs.millisecondsComputationEnd = global.GetElapsedMilliseconds();
  return output.AssignValue(theSAs, theCommands);
}

std::string CalculatorConversions::innerStringFromSemisimpleSubalgebras(SemisimpleSubalgebras& input) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerStringFromSemisimpleSubalgebras");
  Expression tempE;
  FormatExpressions theFormat;
  CalculatorConversions::innerStoreSemisimpleSubalgebras(global.calculator().GetElement(), input, tempE);
  theFormat.flagUseHTML = true;
  return tempE.toString(&theFormat);
}

bool CalculatorConversions::innerStoreSemisimpleSubalgebras(
  Calculator& theCommands, const SemisimpleSubalgebras& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerStoreSemisimpleSubalgebras");
  Expression dynkinTypeE;
  List<std::string> theKeys;
  List<Expression> theValues;
  if (!CalculatorConversions::innerExpressionFromDynkinType(
    theCommands, input.owner->theWeyl.theDynkinType, dynkinTypeE
  )) {
    return false;
  }
  theKeys.AddOnTop("AmbientDynkinType");
  theValues.AddOnTop(dynkinTypeE);

  Expression currentChainE, numericalConvertorE(theCommands);
  currentChainE.MakeSequence(theCommands);
  for (int i = 0; i < input.currentSubalgebraChain.size; i ++) {
    numericalConvertorE = input.currentSubalgebraChain[i].indexInOwner;
    currentChainE.AddChildOnTop(numericalConvertorE);
  }
  theKeys.AddOnTop("CurrentChain");
  theValues.AddOnTop(currentChainE);
  Expression numTypesExploredE;
  numTypesExploredE.MakeSequence(theCommands);
  for (int i = 0; i < input.currentNumLargerTypesExplored.size; i ++) {
    numericalConvertorE = input.currentNumLargerTypesExplored[i];
    numTypesExploredE.AddChildOnTop(numericalConvertorE);
  }
  theKeys.AddOnTop("NumExploredTypes");
  theValues.AddOnTop(numTypesExploredE);
  Expression numHsExploredE;
  numHsExploredE.MakeSequence(theCommands);
  for (int i = 0; i < input.currentNumHcandidatesExplored.size; i ++) {
    numericalConvertorE = input.currentNumHcandidatesExplored[i];
    numHsExploredE.AddChildOnTop(numericalConvertorE);
  }
  theKeys.AddOnTop("NumExploredHs");
  theValues.AddOnTop(numHsExploredE);
  Expression subalgebrasListE, candidateE;
  subalgebrasListE.MakeSequence(theCommands);
  subalgebrasListE.children.Reserve(input.theSubalgebras.theValues.size + 1);
  for (int i = 0; i < input.theSubalgebras.theValues.size; i ++) {
    if (!CalculatorConversions::innerStoreCandidateSA(theCommands, input.theSubalgebras.theValues[i], candidateE)) {
      return false;
    }
    subalgebrasListE.AddChildOnTop(candidateE);
  }
  theKeys.AddOnTop("Subalgebras");
  theValues.AddOnTop(subalgebrasListE);
  return output.MakeSequenceCommands(theCommands, theKeys, theValues);
}

bool CalculatorConversions::innerExpressionFromMonomialUE(
  Calculator& theCommands,
  const MonomialUniversalEnveloping<RationalFunction>& input,
  Expression& output,
  Expression* inputContext
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromMonomialUE");
  if (input.IsConstant()) {
    return output.AssignValue(1, theCommands);
  }
  ChevalleyGenerator theGen;
  theGen.owner = input.owner;
  Expression chevGenE, powerE, termE;
  List<Expression> theTerms;
  for (int i = 0; i < input.generatorsIndices.size; i ++) {
    theGen.theGeneratorIndex = input.generatorsIndices[i];
    CalculatorConversions::innerExpressionFromChevalleyGenerator(theCommands, theGen, chevGenE);
    CalculatorConversions::innerExpressionFromRF(theCommands, input.Powers[i], powerE, inputContext);
    termE.MakeXOX(theCommands, theCommands.opThePower(), chevGenE, powerE);
    theTerms.AddOnTop(termE);
  }
  return output.MakeProducT(theCommands, theTerms);
}

bool CalculatorConversions::innerExpressionFromUE(
  Calculator& theCommands,
  const ElementUniversalEnveloping<RationalFunction>& input,
  Expression& output,
  Expression* inputContext
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromUE");
  LinearCombination<Expression, RationalFunction> theUEE;
  theUEE.MakeZero();
  Expression currentMonE;
  for (int i = 0; i < input.size(); i ++) {
    if (!CalculatorConversions::innerExpressionFromMonomialUE(theCommands, input[i], currentMonE, inputContext)) {
      return theCommands << "<hr>Failed to store " << input.toString();
    }
    theUEE.AddMonomial(currentMonE, input.coefficients[i]);
  }
  return output.makeSum(theCommands, theUEE);
}

bool CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs(
  Calculator& theCommands,
  const Expression& input,
  ElementSemisimpleLieAlgebra<Rational>& output,
  SemisimpleLieAlgebra& owner
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs");
  ElementSemisimpleLieAlgebra<AlgebraicNumber> outputAlgebraic;
  if (!CalculatorConversions::innerLoadElementSemisimpleLieAlgebraAlgebraicNumbers(
    theCommands, input, outputAlgebraic, owner
  )) {
    return theCommands << "<hr> Failed to obtain semisimple Lie algebra element from "
    << input.toString() << ". ";
  }
  for (int i = 0; i < outputAlgebraic.coefficients.size; i ++) {
    if (!outputAlgebraic.coefficients[i].IsRational()) {
      return theCommands << "<hr>From input: " << input.toString() << ", I managed to extract element: "
      << outputAlgebraic.toString() << " but that appears to not have rational coefficients.";
    }
  }
  output = outputAlgebraic; //<-implicit conversion here!
  return true;
}

bool CalculatorConversions::innerLoadElementSemisimpleLieAlgebraAlgebraicNumbers(
  Calculator& theCommands,
  const Expression& input,
  ElementSemisimpleLieAlgebra<AlgebraicNumber>& output,
  SemisimpleLieAlgebra& owner
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerLoadElementSemisimpleLieAlgebraAlgebraicNumbers");
  Expression polyFormE;
  Polynomial<AlgebraicNumber> polyForm;
  bool polyFormGood = CalculatorConversions::functionPolynomial<AlgebraicNumber>(theCommands, input, polyFormE);
  if (polyFormGood) {
    polyFormGood = polyFormE.IsOfType<Polynomial<AlgebraicNumber> >(&polyForm);
  }
  if (!polyFormGood) {
    return theCommands << "<hr>Failed to convert " << input.toString() << " to polynomial.<hr>";
  }
  ChevalleyGenerator theChevGen;
  ElementSemisimpleLieAlgebra<AlgebraicNumber> currentElt;
  theChevGen.owner = &owner;
  output.MakeZero();
  Expression theContext = polyFormE.GetContext();
  for (int j = 0; j < polyForm.size(); j ++) {
    const MonomialP& currentMon = polyForm[j];
    int theGenIndex = 0;
    if (!currentMon.IsOneLetterFirstDegree(&theGenIndex)) {
      return theCommands << "<hr>Failed to convert semisimple Lie algebra input to linear poly: "
      << input.toString() << ".<hr>";
    }
    Expression singleChevGenE = theContext.ContextGetContextVariable(theGenIndex);
    if (!singleChevGenE.StartsWith(theCommands.opUnderscore(), 3)) {
      return theCommands << "<hr>Failed to convert: "
      << singleChevGenE.toString()
      << "(summand of: "
      << input.toString() << ") to Chevalley generator.<hr>";
    }
    std::string theLetter;
    if (
      !singleChevGenE[1].IsOperation(&theLetter) ||
      !singleChevGenE[2].IsSmallInteger(&theChevGen.theGeneratorIndex)
    ) {
      return theCommands << "<hr>Failed to convert summand "
      << singleChevGenE.toString() << " to Chevalley generator of "
      << owner.ToStringLieAlgebraName();
    }
    bool isGood = true;
    if (theLetter == "g") {
      theChevGen.theGeneratorIndex = owner.GetGeneratorFromDisplayIndex(theChevGen.theGeneratorIndex);
      if (theChevGen.theGeneratorIndex < 0 || theChevGen.theGeneratorIndex >= owner.GetNumGenerators()) {
        isGood = false;
      }
      output.AddMonomial(theChevGen, polyForm.coefficients[j]);
    } else if (theLetter == "h") {
      int theRootIndex = owner.GetRootIndexFromDisplayIndex(theChevGen.theGeneratorIndex);
      if (theRootIndex < 0) {
        isGood = false;
      } else {
        currentElt.MakeHgenerator(owner.theWeyl.RootSystem[theRootIndex], owner);
        currentElt *= polyForm.coefficients[j];
        output += currentElt;
      }
    } else {
      isGood = false;
    }
    if (!isGood) {
      return theCommands << "<hr>Failed to convert summand "
      << singleChevGenE.toString() << " to Chevalley generator of "
      << owner.ToStringLieAlgebraName();
    }
  }
  return true;
}

bool CalculatorConversions::innerElementUE(
  Calculator& theCommands, const Expression& input, Expression& output, SemisimpleLieAlgebra& owner
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerElementUE");
  if (input.size() != 2) {
    return theCommands << "Universal enveloping algebra element expects a single argument. ";
  }
  ChevalleyGenerator theChevGen;
  theChevGen.owner = &owner;
  ElementUniversalEnveloping<RationalFunction> outputUE;
  ElementUniversalEnveloping<RationalFunction> currentSummand;
  ElementUniversalEnveloping<RationalFunction> currentMultiplicand;
  MonomialP currentMultiplicandRFpartMon;
  Polynomial<Rational> currentPMultiplicand;
  RationalFunction currentMultiplicandRFpart;
  outputUE.MakeZero(owner);
  Expression polyE;
  if (!CalculatorConversions::functionPolynomial<Rational>(theCommands, input[1], polyE)) {
    return theCommands << "<hr>Failed to convert " << input[1].toString() << " to polynomial.<hr>";
  }
  Polynomial<Rational> theP;
  if (polyE.IsError() || !polyE.IsOfType<Polynomial<Rational> >(&theP)) {
    return theCommands << "<hr>Failed to convert " << input[1].toString()
    << " to polynomial. Instead I got " << polyE.toString() << ". <hr>";
  }
  Expression theContext = polyE.GetContext();
  Expression outputPolyVars;
  outputPolyVars.reset(theCommands, 1);
  outputPolyVars.AddChildAtomOnTop(theCommands.opPolynomialVariables());
  for (int j = 0; j < theP.size(); j ++) {
    const MonomialP& currentMon = theP[j];
    currentSummand.MakeConst(theP.coefficients[j], owner);
    currentMultiplicandRFpartMon.MakeOne();
    for (int i = 0; i < currentMon.minimalNumberOfVariables(); i ++) {
      int thePower = - 1;
      if (!currentMon(i).IsSmallInteger(&thePower)) {
        return theCommands
        << "<hr>Failed to convert one of the exponents appearing in "
        << input[1].toString()
        << " to  a small integer polynomial.<hr>";
      }
      if (thePower == 0) {
        continue;
      }
      Expression singleChevGenE = theContext.ContextGetContextVariable(i);
      if (!singleChevGenE.IsListNElements(2)) {
        return theCommands << "<hr>Failed to convert " << input[1].toString() << " to polynomial.<hr>";
      }
      std::string theLetter;
      if (
        !singleChevGenE[0].IsOperation(&theLetter) ||
        !singleChevGenE[1].IsSmallInteger(&theChevGen.theGeneratorIndex)
      ) {
        return theCommands << "<hr>Failed to convert summand "
        << singleChevGenE.toString() << " to Chevalley generator of "
        << owner.ToStringLieAlgebraName();
      }
      bool isGood = true;
      bool isHonestElementUE = true;
      if (theLetter == "g") {
        theChevGen.theGeneratorIndex = owner.GetGeneratorFromDisplayIndex(theChevGen.theGeneratorIndex);
        if (theChevGen.theGeneratorIndex < 0 || theChevGen.theGeneratorIndex >= owner.GetNumGenerators()) {
          isGood = false;
        }
      } else if (theLetter == "h") {
        if (theChevGen.theGeneratorIndex < 1 || theChevGen.theGeneratorIndex>owner.GetRank()) {
          isGood = false;
        } else {
          theChevGen.theGeneratorIndex = theChevGen.theGeneratorIndex +owner.GetNumPosRoots() - 1;
        }
      } else {
        isHonestElementUE = false;
      }
      if (!isGood) {
        return theCommands << "<hr>Failed to convert summand "
        << singleChevGenE.toString() << " to Chevalley generator of "
        << owner.ToStringLieAlgebraName();
      }
      if (isHonestElementUE) {
        currentMultiplicand.MakeOneGenerator(theChevGen.theGeneratorIndex, owner, 1);
        currentMultiplicand.RaiseToPower(thePower);
        currentSummand*= currentMultiplicand;
      } else {
        int varIndex = outputPolyVars.GetIndexChild(singleChevGenE);
        if (varIndex == - 1) {
          outputPolyVars.AddChildOnTop(singleChevGenE);
          varIndex = outputPolyVars.children.size - 1;
        } else {
          varIndex --;
        }
        currentMultiplicandRFpartMon[varIndex] = thePower;
      }
    }
    currentPMultiplicand.MakeZero();
    currentPMultiplicand.AddMonomial(currentMultiplicandRFpartMon, 1);
    currentMultiplicandRFpart = currentPMultiplicand;
    currentSummand *= currentMultiplicandRFpart;
    outputUE += currentSummand;
  }
  Expression outputContext;
  outputContext.MakeEmptyContext(theCommands);
  outputContext.AddChildOnTop(outputPolyVars);
  outputContext.ContextSetSSLieAlgebrA(
    theCommands.theObjectContainer.semisimpleLieAlgebras.GetIndex(owner.theWeyl.theDynkinType),
    theCommands
  );
  return output.AssignValueWithContext(outputUE, outputContext, theCommands);
}

bool CalculatorConversions::innerExpressionFromBuiltInTypE(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  if (input.size() != 2) {
    return false;
  }
  return CalculatorConversions::functionExpressionFromBuiltInType(theCommands, input[1], output);
}

bool CalculatorConversions::functionExpressionFromBuiltInType(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromBuiltInType");
  if (input.IsOfType<Rational>() || input.IsOfType<double>() || input.IsOfType<AlgebraicNumber>()) {
    output = input;
    return true;
  }
  if (input.IsOfType<Polynomial<Rational> >()) {
    return CalculatorConversions::functionExpressionFromPoly<Rational>(theCommands, input, output);
  }
  if (input.IsOfType<Polynomial<AlgebraicNumber> >()) {
    return CalculatorConversions::functionExpressionFromPoly<AlgebraicNumber>(theCommands, input, output);
  }
  if (input.IsOfType<RationalFunction>()) {
    return CalculatorConversions::innerExpressionFromRF(theCommands, input, output);
  }
  return false;
}

bool CalculatorConversions::innerExpressionFromUE(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromUE");
  ElementUniversalEnveloping<RationalFunction> theUE;
  if (!input.IsOfType<ElementUniversalEnveloping<RationalFunction> >(&theUE)) {
    return theCommands << "<hr>Expression " << input.toString()
    << " is not an element of universal enveloping, can't convert to expression";
  }
  return CalculatorConversions::innerExpressionFromUE(theCommands, theUE, output);
}

bool CalculatorConversions::innerExpressionFromRF(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromRF");
  if (!input.IsOfType<RationalFunction>()) {
    return false;
  }
  const RationalFunction& theRF= input.GetValue<RationalFunction>();
  Expression theContext = input.GetContext();
  return CalculatorConversions::innerExpressionFromRF(theCommands, theRF, output, &theContext);
}

bool CalculatorConversions::innerExpressionFromRF(
  Calculator& theCommands, const RationalFunction& input, Expression& output, Expression* inputContext
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromRF");
  Rational aConst;
  if (input.IsConstant(&aConst)) {
    return output.AssignValue(aConst, theCommands);
  }
  Polynomial<Rational> numP, denP;
  input.GetNumerator(numP);

  if (input.IsConstant() || input.expressionType == input.typePoly) {
    return CalculatorConversions::innerExpressionFromPoly<Rational>(theCommands, numP, output, inputContext);
  }
  Expression numE, denE;
  input.GetDenominator(denP);
  Polynomial<Rational> numRescaled = numP;
  Polynomial<Rational> denRescaled = denP;
  Rational topMultiple = numRescaled.scaleNormalizeLeadingMonomial();
  Rational bottomMultiple = denRescaled.scaleNormalizeLeadingMonomial();
  Rational multipleTopBottom = bottomMultiple / topMultiple;
  numRescaled *= multipleTopBottom.GetNumerator();
  denRescaled *= multipleTopBottom.GetDenominator();
  CalculatorConversions::innerExpressionFromPoly<Rational>(theCommands, numRescaled, numE, inputContext);
  CalculatorConversions::innerExpressionFromPoly<Rational>(theCommands, denRescaled, denE, inputContext);
  return output.MakeXOX(theCommands, theCommands.opDivide(), numE, denE);
}

bool CalculatorConversions::innerRationalFunctioN(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerRationalFunctioN");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorConversions::functionRationalFunction(theCommands, input[1], output);
}

bool CalculatorConversions::functionRationalFunction(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::functionRationalFunction");
  Expression intermediate(theCommands);
  if (
    input.StartsWith(theCommands.opPlus(), 3) ||
    input.StartsWith(theCommands.opTimes(), 3) ||
    input.StartsWith(theCommands.opDivide(), 3)
  ) {
    Expression leftE, rightE;
    if (
      !CalculatorConversions::functionRationalFunction(theCommands, input[1], leftE) ||
      !CalculatorConversions::functionRationalFunction(theCommands, input[2], rightE)
    ) {
      return theCommands << "<hr> Failed to convert " << input[1].toString()
      << " and " << input[2].toString() << " to rational function. ";
    }
    if (leftE.IsError() || rightE.IsError()) {
      return theCommands << "<hr> Conversion of " << input[1].toString()
      << " and " << input[2].toString() << "  returned error(s): "
      << leftE.toString() << " and " << rightE.toString();
    }
    intermediate.AddChildOnTop(input[0]);
    intermediate.AddChildOnTop(leftE);
    intermediate.AddChildOnTop(rightE);
    if (input.StartsWith(theCommands.opPlus())) {
      return CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrRFToRatOrPolyOrRF(
        theCommands, intermediate, output
      );
    }
    if (input.StartsWith(theCommands.opTimes())) {
      return CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF(
        theCommands, intermediate, output
      );
    }
    if (input.StartsWith(theCommands.opDivide())) {
      return CalculatorFunctionsBinaryOps::innerDivideRFOrPolyOrRatByRFOrPoly(
        theCommands, intermediate, output
      );
    }
    global.fatal << "This line of code should never be reached, something has gone wrong." << global.fatal;
  }
  int theSmallPower = - 1;
  if (input.StartsWith(theCommands.opThePower(), 3) ) {
    if (input[2].IsSmallInteger(&theSmallPower)) {
      Expression leftE;
      if (!CalculatorConversions::functionRationalFunction(theCommands, input[1], leftE)) {
        return theCommands << "<hr>CalculatorConversions::innerRationalFunction: failed to convert "
        << input[1].toString() << " to rational function. ";
      }
      if (leftE.IsError()) {
        return theCommands << "<hr> Conversion of " << input[1].toString() << " returned error: " << leftE.toString();
      }
      RationalFunction theRF = leftE.GetValue<RationalFunction>();
      theRF.RaiseToPower(theSmallPower);
      return output.AssignValueWithContext(theRF, leftE.GetContext(), theCommands);
    }
    theCommands << "<hr>Warning: failed to raise " << input[1].toString() << " to power " << input[2].toString()
    << ": failed to convert the power to small integer. I am treating " << input.toString()
    << " as a single variable: please make sure that is what you want.";
  }
  if (input.IsOfType<RationalFunction>()) {
    output = input;
    return true;
  }
  if (input.IsOfType<Polynomial<Rational> >() || input.IsOfType<Rational>()) {
    return input.ConvertInternally<RationalFunction>(output);
  }
  if (input.IsOfType<AlgebraicNumber>()) {
    AlgebraicNumber theNumber = input.GetValue<AlgebraicNumber>();
    Rational theRat;
    if (theNumber.IsRational(&theRat)) {
      Expression tempE;
      tempE.AssignValue(theRat, theCommands);
      return tempE.ConvertInternally<RationalFunction> (output);
    }
  }
  Expression theContext;
  theContext.ContextMakeContextWithOnePolyVar(theCommands, input);
  RationalFunction theRF;
  theRF.MakeOneLetterMoN(0, 1);
  return output.AssignValueWithContext(theRF, theContext, theCommands);
}

bool CalculatorConversions::functionMatrixRational(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::functionMatrixRational");
  Matrix<Rational> outputMat;
  if (!theCommands.functionGetMatrix(input, outputMat, nullptr, - 1, nullptr)) {
    return theCommands << "<br>Failed to get matrix of rationals. ";
  }
  return output.AssignMatrix(outputMat, theCommands);
}

bool CalculatorConversions::innerMatrixRationalTensorForM(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerMatrixRationalTensorForM");
  if (!CalculatorConversions::innerMakeMatrix(theCommands, input, output)) {
    return theCommands << "Failed to extract matrix of rationals. ";
  }
  Matrix<Rational> matRat;
  if (!theCommands.functionGetMatrix(output, matRat)) {
    return false;
  }
  MatrixTensor<Rational> outputMatTensor;
  outputMatTensor = matRat;
  return output.AssignValue(outputMatTensor, theCommands);
}

bool CalculatorConversions::functionMatrixRationalTensorForm(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::functionMatrixRationalTensorForm");
  Matrix<Rational> matRat;
  if (!theCommands.functionGetMatrix(input, matRat)) {
    return false;
  }
  MatrixTensor<Rational> outputMatTensor;
  outputMatTensor = matRat;
  return output.AssignValue(outputMatTensor, theCommands);
}

bool CalculatorConversions::outerMatrixExpressionsToMatrixOfType(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::outerMatrixExpressionsToMatrixOfType");
  if (!input.IsMatrix()) {
    return false;
  }
  if (input.size() < 1) {
    return false;
  }
  if (input[0].size() != 1) {
    return false;
  }
  if (input[0][0].theData != theCommands.opMatriX()) {
    return false;
  }
  Matrix<Expression> theMatrix;
  if (!theCommands.GetMatrixExpressionsFromArguments(input, theMatrix)) {
    return false;
  }
  Expression conversionAttempt;
  conversionAttempt.AssignMatrixExpressions(theMatrix, theCommands, true, false);
  if (!conversionAttempt.IsMatrix()) {
    output = conversionAttempt;
    return true;
  }
  if (conversionAttempt.size() >= 1) {
    if (conversionAttempt[0].StartsWith(theCommands.opMatriX())) {
      if (conversionAttempt[0].size() > 1) {
        output = conversionAttempt;
        return true;
      }
    }
  }
  return false;
}

bool CalculatorConversions::innerMakeMatrix(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerMakeMatrix");
  Matrix<Expression> outMat;
  if (theCommands.GetMatrixExpressionsFromArguments(input, outMat)) {
    return output.AssignMatrixExpressions(outMat, theCommands, true, true);
  }
  return false;
}

bool CalculatorConversions::functionMatrixAlgebraic(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::functionMatrixAlgebraic");
  Matrix<AlgebraicNumber> outputMat;
  if (!theCommands.functionGetMatrix(input, outputMat)) {
    return false;
  }
  return output.AssignMatrix(outputMat, theCommands);
}

bool CalculatorConversions::innerMatrixRationalFunction(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  return CalculatorConversions::functionMatrixRationalFunction(
    theCommands, input, output
  );
}

bool CalculatorConversions::functionMatrixRationalFunction(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::functionMatrixRationalFunction");
  Matrix<RationalFunction> outputMat;
  Expression ContextE;
  if (!theCommands.functionGetMatrix(
    input,
    outputMat,
    &ContextE,
    - 1,
    CalculatorConversions::functionRationalFunction
  )) {
    return theCommands << "<hr>Failed to get matrix of rational functions. ";
  }
  output.AssignMatrix(outputMat, theCommands, &ContextE);
  output.CheckConsistency();
  return true;
}

bool CalculatorConversions::innerLoadFileIntoString(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerLoadFileIntoString");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  std::string theRelativeFileName;
  if (!argument.IsOfType<std::string>(&theRelativeFileName)) {
    theCommands << "Input of load file string command is supposed to be a string. "
    << "Converting your expression to a string and using that instead. ";
    theRelativeFileName = argument.toString();
  }
  std::string outputString;
  if (!FileOperations::LoadFileToStringVirtual(
    theRelativeFileName, outputString, false, &theCommands.Comments
  )) {
    return false;
  }
  return output.AssignValue(outputString, theCommands);
}

bool CalculatorConversions::innerMakeElementHyperOctahedral(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerMakeElementHyperOctahedral");
  std::string inputStringFormat;
  ElementHyperoctahedralGroupR2 theElement;
  if (input.IsOfType<std::string>(&inputStringFormat)) {
    theElement.MakeFromString(inputStringFormat);
    return output.AssignValue(theElement, theCommands);
  }
  if (input.children.size < 3) {
    return theCommands << "To make elements of hyperoctahedral group we need at least 3 inputs. ";
  }
  List<int> oneCycle;
  if (!theCommands.GetVectoRInt(input[1], oneCycle)) {
    return theCommands
    << "Failed to extract a cycle structure from the first argument of input: "
    << input.toString();
  }
  for (int i = 0; i < oneCycle.size; i ++) {
    if (oneCycle[i] < 1) {
      return theCommands << "Your input: " << input[1].toString()
      << " had integers that were too small. ";
    }
    oneCycle[i] --;
    if (oneCycle[i] > 1000) {
      return theCommands << "Your input: " << input[1].toString()
      << " had integers that were too large. ";
    }
  }
  theElement.h.AddCycle(oneCycle);
  for (int i = 2; i < input.children.size; i ++) {
    if (input[i].IsEqualToOne()) {
      theElement.k.ToggleBit(i - 2);
    } else if (!input[i].IsEqualToZero()) {
      return theCommands << "Your input: " << input.toString()
      << " had bit values that were not ones and zeroes.";
    }
  }
  return output.AssignValue(theElement, theCommands);
}
