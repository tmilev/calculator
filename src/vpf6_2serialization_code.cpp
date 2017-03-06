//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".#include "vpfHeader3Calculator0_Interface.h"
#include "vpfHeader3Calculator0_Interface.h"
#include "vpfHeader3Calculator1_InnerTypedFunctions.h"
#include "vpfImplementationHeader2Math051_PolynomialComputations_Basic.h"
#include "vpfImplementationHeader2Math15_UniversalEnveloping.h" // undefined reference to `ElementUniversalEnveloping<RationalFunctionOld>::MakeZero(SemisimpleLieAlgebra&)'
static ProjectInformationInstance ProjectInfoVpf5_1cpp(__FILE__, "C++ object <-> calculator expression conversions.");

bool CalculatorConversions::innerExpressionFromChevalleyGenerator(Calculator& theCommands, const ChevalleyGenerator& input, Expression& output)
{ MacroRegisterFunctionWithName("innerExpressionFromChevalleyGenerator");
  input.CheckInitialization();
  output.reset(theCommands, 2);
  Expression generatorLetterE, generatorIndexE;
  if (input.theGeneratorIndex>=input.owner->GetNumPosRoots() && input.theGeneratorIndex< input.owner->GetNumPosRoots()+input.owner->GetRank())
    generatorLetterE.MakeAtom(theCommands.AddOperationNoRepetitionOrReturnIndexFirst("h"), theCommands);
  else
    generatorLetterE.MakeAtom(theCommands.AddOperationNoRepetitionOrReturnIndexFirst("g"), theCommands);
  output.AddChildOnTop(generatorLetterE);
  generatorIndexE.AssignValue(input.owner->GetDisplayIndexFromGenerator(input.theGeneratorIndex), theCommands);
  return output.AddChildOnTop(generatorIndexE);
}

bool CalculatorConversions::innerSSLieAlgebra(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerSSLieAlgebra");
  SemisimpleLieAlgebra* dummySA;
  CalculatorConversions::innerSSLieAlgebra(theCommands, input, output, dummySA);
  //theCommands.ToString();
  //stOutput << "The semisimple lie alg: " << output.ToString();

  return true;
}

bool CalculatorConversions::innerLoadWeylGroup(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerLoadWeylGroup");
//  stOutput << "got ere!";
  DynkinType theType;
  if (!CalculatorConversions::innerDynkinType(theCommands, input, theType))
    return false;
  SemisimpleLieAlgebra& theSA=theCommands.theObjectContainer.GetLieAlgebraCreateIfNotPresent(theType);
  return output.AssignValue(theSA.theWeyl, theCommands);
}

bool CalculatorConversions::innerDynkinSimpleType(Calculator& theCommands, const Expression& input, DynkinSimpleType& outputMon)
{ MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::DeSerializeMon_DynkinSimpleType");
  Expression rankE, typeLetterE, scaleE;
  if (input.StartsWith(theCommands.opUnderscore(),3))
  { rankE=input[2];
    typeLetterE=input[1];
    if (typeLetterE.StartsWith(theCommands.opThePower(),3))
    { scaleE=typeLetterE[2];
      typeLetterE=typeLetterE[1];
    } else
      scaleE.AssignValue(1, theCommands);
  } else if (input.StartsWith(theCommands.opThePower(),3))
  { scaleE=input[2];
    if (!input[1].StartsWith(theCommands.opUnderscore(), 3))
      return theCommands << "<hr>Failed to extract rank, type from "
      << input[1].ToString()
      << ". The expression does not have two children.";
    rankE=input[1][2];
    typeLetterE=input[1][1];
  } else
    return theCommands << "<hr>Failed to extract rank, type, "
    << "first co-root length from: " << input.ToString() << ". ";
  Rational theScale;
  if (!scaleE.IsOfType<Rational>(&theScale))
    return theCommands << "<hr>Failed to extract first co-root length: "
    << "expression " << scaleE.ToString()
    << " is not a rational number.";
  if (theScale<=0)
    return theCommands << "<hr>Couldn't extract first co-root length: " << theScale.ToString() << " is non-positive.";
  std::string theTypeName;
  if (!typeLetterE.IsAtom(&theTypeName))
    return theCommands << "I couldn't extract a type letter from " << typeLetterE.ToString();
  if (theTypeName.size()!=1)
    return theCommands << "<hr>Error while extracting Dynkin simple type: The type of a simple Lie algebra must be the letter A, B, C, D, E, F or G."
    << "Instead, it is " << theTypeName + ". Error encountered while processing " << input.ToString();
  char theWeylLetter=theTypeName[0];
  if (theWeylLetter=='a') theWeylLetter='A';
  if (theWeylLetter=='b') theWeylLetter='B';
  if (theWeylLetter=='c') theWeylLetter='C';
  if (theWeylLetter=='d') theWeylLetter='D';
  if (theWeylLetter=='e') theWeylLetter='E';
  if (theWeylLetter=='f') theWeylLetter='F';
  if (theWeylLetter=='g') theWeylLetter='G';
  if (!(theWeylLetter=='A' || theWeylLetter=='B' || theWeylLetter=='C' || theWeylLetter=='D' || theWeylLetter=='E' || theWeylLetter=='F' || theWeylLetter=='G'))
    return theCommands << "The type of a simple Lie algebra must be the letter A, B, C, D, E, F or G; error while processing " << input.ToString();
  int theRank;
  if (!rankE.IsSmallInteger(&theRank))
    return theCommands << "I wasn't able to extract rank from " << input.ToString();
  if (theRank<1 || theRank>20)
    return theCommands << "<hr>The rank of a simple Lie algebra must be between 1 and 20; error while processing " << input.ToString();
  if (theWeylLetter=='E' &&(theRank>8 || theRank<3))
    return theCommands << "<hr>Type E must have rank 6,7 or 8 ";
  if (theWeylLetter=='D' &&(theRank<3))
    return theCommands << "<hr>Type D is expected to have rank 4 or more, your input was of rank " << theRank << ". ";
  outputMon.MakeArbitrary(theWeylLetter, theRank, theScale);
  return true;
}

bool CalculatorConversions::innerDynkinType(Calculator& theCommands, const Expression& input, DynkinType& output)
{ RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("Calculator::innerLoadDynkinType");
  MonomialCollection<Expression, Rational> theType;
  if (!theCommands.CollectSummands(theCommands, input, theType))
    return false;
  DynkinSimpleType simpleComponent;
  output.MakeZero();
  for (int i=0; i<theType.size(); i++)
  { if (!CalculatorConversions::innerDynkinSimpleType(theCommands, theType[i], simpleComponent))
      return false;
    int theMultiplicity=-1;
    if (!theType.theCoeffs[i].IsSmallInteger(&theMultiplicity))
      theMultiplicity=-1;
    if (theMultiplicity<0)
      return theCommands << "<hr>Failed to convert the coefficient " << theType.theCoeffs[i] << " of " << theType[i].ToString()
      << " to a small positive integer. ";
    output.AddMonomial(simpleComponent, theMultiplicity);
  }
  return !theType.IsEqualToZero();
}

bool CalculatorConversions::innerSSLieAlgebra(Calculator& theCommands, const Expression& input, Expression& output, SemisimpleLieAlgebra*& outputPointer)
{ RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("Calculator::innerSSLieAlgebra");
//  stOutput << "<br>DEBUG: calling innerSSLieAlgebra";
//  double startTimeDebug=theGlobalVariables.GetElapsedSeconds();
  DynkinType theDynkinType;
//  stOutput.flush();
  outputPointer=0;
  if(!CalculatorConversions::innerDynkinType(theCommands, input, theDynkinType))
    return theCommands << "Failed to extract Dynkin type from: " << input.ToString();
//  stOutput << "got to making semisimple Lie algebra of type " << theDynkinType.ToString() << " from input expression: "
//  << input.ToString();
  if (theDynkinType.GetRank()>20)
    return theCommands << "I have been instructed to allow semisimple Lie algebras of rank 20 maximum. "
    << "If you would like to relax this limitation edit file " << __FILE__
    << " line " << __LINE__ << ". Note that the Chevalley constant computation Reserves a dim(g)*dim(g) "
    << "table of RAM memory, which means the RAM memory rises with the 4^th power of rank(g). "
    << " You have been warned. Alternatively, you may want to implement a sparse structure constant table "
    << "(write me an email if you want to do that, I will help you). ";
  bool newlyCreated=!theCommands.theObjectContainer.theSSLieAlgebras.Contains(theDynkinType);
  outputPointer=&theCommands.theObjectContainer.GetLieAlgebraCreateIfNotPresent(theDynkinType);
  outputPointer->CheckConsistency();
  output.AssignValue(*outputPointer, theCommands);
  if (newlyCreated)
  { //crash << theSSalgebra.theWeyl.theDynkinType.ToString() << crash;
    //stOutput << "<br>Elapsed time before computing Chevalley consts: "
    //<< theGlobalVariables.GetElapsedSeconds()-startTimeDebug;
    outputPointer->ComputeChevalleyConstants();
    //stOutput << "<br>Elapsed time after computing Chevalley consts: "
    //<< theGlobalVariables.GetElapsedSeconds()-startTimeDebug;
    Expression tempE;
    //stOutput << "<br>Elapsed time before printing subalgebra: "
    //<< theGlobalVariables.GetElapsedSeconds()-startTimeDebug;
    theCommands.innerPrintSSLieAlgebra(theCommands, output, tempE, false);
    //stOutput << "<br>after printing time: "
    //<< theGlobalVariables.GetElapsedSeconds()-startTimeDebug;
    theCommands << tempE.GetValue<std::string>();
  }
//  stOutput << "<br>DEBUG: needed: " <<
//  theGlobalVariables.GetElapsedSeconds()-startTimeDebug << " seconds for generating the Lie algebra. ";
  //theSSalgebra.TestForConsistency(theGlobalVariables);
  return true;
}

bool CalculatorConversions::innerExpressionFromDynkinSimpleType
(Calculator& theCommands, const DynkinSimpleType& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::DynkinSimpleType");
  Expression letterE, rankE, letterAndIndexE, indexE;
  std::string letterS;
  letterS=input.theLetter;
  letterE.MakeAtom(theCommands.AddOperationNoRepetitionOrReturnIndexFirst(letterS), theCommands);
  indexE.AssignValue(input.CartanSymmetricInverseScale, theCommands);
  rankE.AssignValue(input.theRank, theCommands);
  letterAndIndexE.MakeXOX(theCommands, theCommands.opThePower(), letterE, indexE);
  output.reset(theCommands);
  output.AddChildOnTop(letterAndIndexE);
  return output.AddChildOnTop(rankE);
}

bool CalculatorConversions::innerStoreSemisimpleLieAlgebra(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsOfType<SemisimpleLieAlgebra>())
    return output.MakeError("Asking to store non-semisimple Lie algebra as such is not allowed. ", theCommands);
  SemisimpleLieAlgebra& owner=input.GetValueNonConst<SemisimpleLieAlgebra>();
  return CalculatorConversions::innerExpressionFromDynkinType(theCommands, owner.theWeyl.theDynkinType, output);
}

bool CalculatorConversions::innerExpressionFromElementSemisimpleLieAlgebraRationals(Calculator& theCommands, const ElementSemisimpleLieAlgebra<Rational>& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerStoreElementSemisimpleLieAlgebraRational");
  MonomialCollection<Expression, Rational> theMons;
  theMons.MakeZero();
  Expression currentMon;
  for (int i=0; i<input.size(); i++)
  { CalculatorConversions::innerExpressionFromChevalleyGenerator(theCommands, input[i], currentMon);
    theMons.AddMonomial(currentMon, input.theCoeffs[i]);
  }
  return output.MakeSum(theCommands, theMons);
}

bool CalculatorConversions::innerExpressionFromDynkinType
(Calculator& theCommands, const DynkinType& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromDynkinType");
  MonomialCollection<Expression, AlgebraicNumber> theMons;
  theMons.MakeZero();
  Expression currentMon;
  for (int i=0; i<input.size(); i++)
  { CalculatorConversions::innerExpressionFromDynkinSimpleType(theCommands, input[i], currentMon);
    theMons.AddMonomial(currentMon, input.theCoeffs[i]);
  }
  return output.MakeSum(theCommands, theMons);
}

bool CalculatorConversions::innerExpressionFromElementSemisimpleLieAlgebraAlgebraicNumbers
(Calculator& theCommands, const ElementSemisimpleLieAlgebra<AlgebraicNumber>& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromElementSemisimpleLieAlgebraAlgebraicNumbers");
  MonomialCollection<Expression, AlgebraicNumber> theMons;
  theMons.MakeZero();
  Expression currentMon;
  for (int i=0; i<input.size(); i++)
  { CalculatorConversions::innerExpressionFromChevalleyGenerator(theCommands, input[i], currentMon);
    theMons.AddMonomial(currentMon, input.theCoeffs[i]);
  }
  return output.MakeSum(theCommands, theMons);
}

bool CalculatorConversions::innerSlTwoSubalgebraPrecomputed(Calculator& theCommands, const Expression& input, slTwoSubalgebra& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerLoadFromObject slTwoSubalgebra");
  if (!input.IsListNElements(4))
    return theCommands << "<hr>input of innerLoadFromObject has " << input.children.size << " children, 4 expected. ";
  const Expression& theOwnerE=input[1];
  Expression tempE;
  if (!CalculatorConversions::innerSSLieAlgebra(theCommands, theOwnerE, tempE, output.owner))
    return theCommands << "<hr>Failed to extract semisimple Lie algebra from " << theOwnerE.ToString()
    << " while extracting its sl(2) subalgebra.";
  const Expression& theF=input[2];
  const Expression& theE=input[3];
  ElementSemisimpleLieAlgebra<Rational> eltF, eltE;
  if (!CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs(theCommands, theF, eltF, *output.owner))
    return theCommands << "<hr>Failed to extract f element while loading sl(2) subalgebra<hr>";
  if (!CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs(theCommands, theE, eltE, *output.owner))
    return theCommands << "<hr>Failed to extract e element while loading sl(2) subalgebra<hr>";
  if (eltE.IsEqualToZero() || eltF.IsEqualToZero())
    return theCommands << "<hr>Failed to load sl(2) subalgebra: either e or f is equal to zero. e and f are: " << eltE.ToString()
    << ", " << eltF.ToString() << ". ";
  if (eltE.GetOwner()!=eltF.GetOwner())
    return theCommands << "<hr>Failed to load sl(2): E and F element of sl(2) have different owners. More precisely, the owner of e is "
    << eltE.GetOwner()->ToString() << " and the owner of f is " << eltF.GetOwner()->ToString();
  output.theE=eltE;
  output.theF=eltF;
  output.owner=eltE.GetOwner();
  DynkinType& theType=output.owner->theWeyl.theDynkinType;
  SemisimpleSubalgebras& ownerSubalgebras=
  theCommands.theObjectContainer.GetSemisimpleSubalgebrasCreateIfNotPresent(theType);
  output.container=&ownerSubalgebras.theSl2s;
  return true;
}

bool CalculatorConversions::innerSlTwoSubalgebraPrecomputed(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerSlTwoSubalgebraPrecomputed");
  slTwoSubalgebra tempSL2;
  if (!CalculatorConversions::innerSlTwoSubalgebraPrecomputed(theCommands, input, tempSL2))
    return theCommands << "<hr>Failed to load sl(2) subalgebra. ";
  return output.AssignValue(tempSL2.ToString(), theCommands);
}

bool CalculatorConversions::innerAlgebraicNumber(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerAlgebraicNumber");
  if(input.IsOfType<AlgebraicNumber>())
  { output=input;
    return true;
  }
  if (input.IsOfType<Rational>())
  { AlgebraicNumber theNum;
    theNum=input.GetValue<Rational>();
    return output.AssignValue(theNum, theCommands);
  }
  return false;
}

bool CalculatorConversions::innerLoadKeysFromStatementList
(Calculator& theCommands, const Expression& input,
 MapLisT<std::string, Expression, MathRoutines::hashString>& output,
 std::stringstream* commentsOnFailure, bool allowFailure)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerLoadKeysFromStatementList");
  MapLisT<Expression, Expression> outputExpressionFormat;
  if (!CalculatorConversions::innerLoadKeysFromStatementList(theCommands, input, outputExpressionFormat, commentsOnFailure, allowFailure))
    return false;
  output.Clear();
  std::string keyName;
  for (int i=0; i<outputExpressionFormat.theKeys.size; i++)
  { if (!outputExpressionFormat.theKeys[i].IsOfType<std::string>(&keyName))
      keyName=outputExpressionFormat.theKeys[i].ToString();
    output.SetKeyValue(keyName, outputExpressionFormat[i]);
  }
  return true;
}

bool CalculatorConversions::innerLoadKeysFromStatementList
(Calculator& theCommands, const Expression& input,
 MapLisT<Expression, Expression>& output, std::stringstream* commentsOnFailure, bool allowFailure)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerLoadKeysFromStatementList");
  output.Clear();
  if (input.StartsWith(theCommands.opDefine(), 3))
  { output.SetKeyValue(input[1], input[2]);
    return true;
  }
  for (int i=1; i<input.size(); i++)
    if (input[i].StartsWith(theCommands.opDefine(), 3))
      output.SetKeyValue(input[i][1], input[i][2]);
    else if (!allowFailure)
    { if (commentsOnFailure!=0)
        *commentsOnFailure << "Could not extract key-value pair from: " << input.ToString() << ": failed to process: "
        << input[i].ToString() ;
      return false;
    }
  return true;
}

bool CalculatorConversions::innerLoadKey
(Calculator& theCommands, const Expression& inputStatementList, const std::string& inputKey, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerLoadKey");
  Expression theKeyE;
  theKeyE.MakeAtom(inputKey, theCommands);
  for (int i=0; i<inputStatementList.children.size; i++)
    if (inputStatementList[i].StartsWith(theCommands.opDefine(), 3))
      if (inputStatementList[i][1]==theKeyE)
      { output=inputStatementList[i][2];
        return true;
      }
  return theCommands << "<hr>Key " << inputKey << " not found in expression " << inputStatementList.ToString() << ".";
}

bool CalculatorConversions::innerStoreCandidateSA(Calculator& theCommands, const CandidateSSSubalgebra& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerStoreCandidateSA");
  Expression currentE;
  List<std::string> keys;
  List<Expression> values;
  input.CheckBasicInitialization();
  CalculatorConversions::innerExpressionFromDynkinType
  (theCommands, input.theWeylNonEmbedded->theDynkinType, currentE);
  keys.AddOnTop("DynkinType");
  values.AddOnTop(currentE);
  Matrix<Rational> conversionMat;
  conversionMat.AssignVectorsToRows(input.theHsScaledToActByTwo);
  currentE.AssignMatrix(conversionMat, theCommands, false);
  keys.AddOnTop("ElementsCartan");
  values.AddOnTop(currentE);
  //  ElementSemisimpleLieAlgebra<Rational> convertedToRational;
  if (input.flagSystemSolved)
  { Expression listGenerators;
    listGenerators.MakeSequence(theCommands);
    for (int i=0; i<input.theNegGens.size; i++)
    { CalculatorConversions::innerExpressionFromElementSemisimpleLieAlgebraAlgebraicNumbers(theCommands, input.theNegGens[i], currentE);
      listGenerators.AddChildOnTop(currentE);
      CalculatorConversions::innerExpressionFromElementSemisimpleLieAlgebraAlgebraicNumbers(theCommands, input.thePosGens[i], currentE);
      listGenerators.AddChildOnTop(currentE);
    }
    keys.AddOnTop("generators");
    values.AddOnTop(listGenerators);
  }
  return output.MakeSequenceCommands(theCommands, keys, values);
}

bool CalculatorConversions::innerCandidateSAPrecomputed(Calculator& theCommands, const Expression& input, Expression& output, CandidateSSSubalgebra& outputSubalgebra, SemisimpleSubalgebras& owner)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerCandidateSAPrecomputed");
  Expression DynkinTypeE, ElementsCartanE, generatorsE;
  if (!CalculatorConversions::innerLoadKey(theCommands, input, "DynkinType", DynkinTypeE) ||
      !CalculatorConversions::innerLoadKey(theCommands, input, "ElementsCartan", ElementsCartanE))
    return false;
  outputSubalgebra.owner=&owner;
  DynkinType theNonEmbeddedDynkinType;
  if (!CalculatorConversions::innerDynkinType(theCommands, DynkinTypeE, theNonEmbeddedDynkinType))
    return theCommands << "<hr> Failed to load dynkin type of candidate subalgebra from " << DynkinTypeE.ToString() << "<hr>";
  outputSubalgebra.theWeylNonEmbedded=&
  theCommands.theObjectContainer.GetWeylGroupDataCreateIfNotPresent(theNonEmbeddedDynkinType);
  outputSubalgebra.theWeylNonEmbedded->MakeFromDynkinType(theNonEmbeddedDynkinType);

  int theRank=owner.owner->GetRank();
  Matrix<Rational> theHs;
  if (!theCommands.GetMatrix(ElementsCartanE, theHs, 0, theRank, 0))
    return theCommands << "<hr>Failed to load Cartan elements for candidate subalgebra of type "
    << outputSubalgebra.theWeylNonEmbedded->theDynkinType << "<hr>";
  if (theHs.NumRows!=outputSubalgebra.theWeylNonEmbedded->GetDim())
    return theCommands << "<hr>Failed to load Cartan elements: I expected "
    << outputSubalgebra.theWeylNonEmbedded->GetDim() << " elements, but failed to get them.";
  List<int> theRanks, theMults;
  outputSubalgebra.theWeylNonEmbedded->theDynkinType.GetLettersTypesMults(0, &theRanks, &theMults, 0);
  outputSubalgebra.CartanSAsByComponentScaledToActByTwo.SetSize
  (outputSubalgebra.theWeylNonEmbedded->theDynkinType.GetNumSimpleComponents());
  int componentCounter=-1;
  int counter=-1;
  for (int i=0; i<theMults.size; i++)
    for (int j=0; j<theMults[i]; j++)
    { componentCounter++;
      Vectors<Rational>& currentComponent=outputSubalgebra.CartanSAsByComponentScaledToActByTwo[componentCounter];
      currentComponent.SetSize(theRanks[i]);
      for (int k=0; k<theRanks[i]; k++)
      { counter++;
        theHs.GetVectorFromRow(counter, currentComponent[k]);
      }
    }
  outputSubalgebra.thePosGens.SetSize(0);
  outputSubalgebra.theNegGens.SetSize(0);
  if (CalculatorConversions::innerLoadKey(theCommands, input, "generators", generatorsE))
  { generatorsE.Sequencefy();
    ElementSemisimpleLieAlgebra<AlgebraicNumber> curGenAlgebraic;
    for (int i=1; i<generatorsE.children.size; i++)
    { if (!CalculatorConversions::innerLoadElementSemisimpleLieAlgebraAlgebraicNumbers(theCommands, generatorsE[i], curGenAlgebraic, *owner.owner))
        return theCommands << "<hr>Failed to load semisimple Lie algebra element from expression " << generatorsE[i].ToString() << ". ";
      if (i%2 ==1)
        outputSubalgebra.theNegGens.AddOnTop(curGenAlgebraic);
      else
        outputSubalgebra.thePosGens.AddOnTop(curGenAlgebraic);
    }
    outputSubalgebra.flagSystemProvedToHaveNoSolution=false;
    outputSubalgebra.flagSystemSolved=true;
  } else
    return theCommands << "<hr>Failed to extract subalgebra generators from expression " << input.ToString() << ". ";
  SemisimpleLieAlgebra& currentNonEmbededSA=
  owner.theSubalgebrasNonEmbedded->GetValueCreateIfNotPresent(outputSubalgebra.theWeylNonEmbedded->theDynkinType);
  outputSubalgebra.indexNonEmbeddedMeStandard =
  owner.theSubalgebrasNonEmbedded->GetIndex(outputSubalgebra.theWeylNonEmbedded->theDynkinType);
  currentNonEmbededSA.theWeyl.ComputeRho(true);
  outputSubalgebra.theWeylNonEmbedded->ComputeRho(true); //<- this line may be unnecessary, the
  //two Weyl groups may coincide depending on some implementation decisions I am about to take
  //some time soon.
  //stOutput << "Before calling compute system, output subalgebra is: " << outputSubalgebra.ToString();
  outputSubalgebra.ComputeHsAndHsScaledToActByTwoFromComponents();
  outputSubalgebra.flagSubalgebraPreloadedButNotVerified=true;
  return output.MakeError("Candidate subalgebra is not a stand-alone object and its Expression output should not be used. ", theCommands);
}

bool CalculatorConversions::innerLoadSemisimpleSubalgebras(Calculator& theCommands, const Expression& inpuT, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerLoadSemisimpleSubalgebras");
  Expression input=inpuT;
  theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=-1;
  Expression theAmbientTypeE, numExploredHsE, numExploredTypesE, theSAsE, currentChainE;
  if (!CalculatorConversions::innerLoadKey(theCommands, input, "AmbientDynkinType", theAmbientTypeE))
    return theCommands << "<hr>Failed to load Dynkin type from: " << input.ToString();
  if (!CalculatorConversions::innerLoadKey(theCommands, input, "NumExploredHs", numExploredHsE))
    return theCommands << "<hr>Failed to load numExploredHs list from: " << input.ToString();
  if (!CalculatorConversions::innerLoadKey(theCommands, input, "NumExploredTypes", numExploredTypesE))
    return theCommands << "<hr>Failed to load NumExploredTypes list from: " << input.ToString();
  if (!CalculatorConversions::innerLoadKey(theCommands, input, "Subalgebras", theSAsE))
    return theCommands << "<hr>Failed to load Subalgebras list from: " << input.ToString();
  if (!CalculatorConversions::innerLoadKey(theCommands, input, "CurrentChain", currentChainE))
    return theCommands << "<hr>Failed to load CurrentChain from: " << input.ToString();
//  stOutput << "<br>dynkinTypeE: " << theAmbientTypeE.ToString();
//  stOutput << "<br>numExploredHsE: " << numExploredHsE.ToString();
//  stOutput << "<br>numExploredTypesE: " << numExploredTypesE.ToString();
//  stOutput << "<br>theSAsE: " << theSAsE.ToString();
//  stOutput << "<br>currentChainE: " << currentChainE.ToString();
  List<int> currentChainInt, numExploredTypes, numExploredHs;
  if (!theCommands.GetVectoRInt(currentChainE, currentChainInt))
    return false;
  if (!theCommands.GetVectoRInt(numExploredHsE, numExploredHs))
    return false;
  if (!theCommands.GetVectoRInt(numExploredTypesE, numExploredTypes))
    return false;
//  stOutput << "<br>numExploredHs: " << numExploredHs;
//  stOutput << "<br>numExploredTypes: " << numExploredTypes;
  SemisimpleLieAlgebra* ownerSS=0;
  Expression tempE;
  ProgressReport theReport;
  std::stringstream reportStream;
  reportStream << "Extracting semisimple Lie algebra ... ";
  theReport.Report(reportStream.str());
  if (!CalculatorConversions::innerSSLieAlgebra(theCommands, theAmbientTypeE, tempE, ownerSS))
    return theCommands << "<hr>Error loading semisimple subalgebras: failed to extract ambient semisimple Lie algebra. ";
  reportStream << " type: " << ownerSS->theWeyl.theDynkinType.ToString() << ". ";
  theReport.Report(reportStream.str());

  SemisimpleSubalgebras& theSAs=
  theCommands.theObjectContainer.GetSemisimpleSubalgebrasCreateIfNotPresent(ownerSS->theWeyl.theDynkinType);
  reportStream << " Initializing data structures... ";
  theReport.Report(reportStream.str());
  reportStream << " done. Fetching subalgebra list ... ";
  theReport.Report(reportStream.str());
  //stOutput << "Owner ss is here:  " << ownerSS->ToString();
  theSAsE.Sequencefy();
  theSAs.theSubalgebras.SetExpectedSize(theSAsE.children.size-1);
  theSAs.theSubalgebras.Clear();
  theSAs.theSubalgebrasNonEmbedded->SetExpectedSize(theSAsE.children.size-1);
  theSAs.flagAttemptToSolveSystems=true;
  theSAs.flagComputeModuleDecomposition=true;
  theSAs.flagComputePairingTable=false;
  theSAs.flagComputeNilradicals=false;
  theSAs.timeComputationStartInSeconds=theGlobalVariables.GetElapsedSeconds();
  reportStream << " done. <br>Total subalgebras: " << theSAsE.children.size-1 << ". ";
  theReport.Report(reportStream.str());

  for (int i=1; i<theSAsE.children.size; i++)
  { std::stringstream reportStream2;
    reportStream2 << reportStream.str() << "Subalgebra " << i << " is being loaded from expression "
    << theSAsE[i].ToString() << ".";
    //stOutput << reportStream.str();
    theReport.Report(reportStream2.str());
    CandidateSSSubalgebra currentCandidate;
    if (!CalculatorConversions::innerCandidateSAPrecomputed(theCommands, theSAsE[i], tempE, currentCandidate, theSAs))
      return theCommands << "<hr>Error loading candidate subalgebra: failed to load candidate number "
      << i << " extracted from expression: "
      << theSAsE[i].ToString() << ". <hr>";
    currentCandidate.CheckFullInitializatioN();
    //stOutput << "<hr>read cartan elements: " << currentCandidate.theHs.size;
    if (theSAs.theSubalgebras.Contains(currentCandidate.theHs))
    { theCommands << "<hr>Did not load subalgebra of type " << currentCandidate.theWeylNonEmbedded->ToString()
      << " because I've already loaded a subalgebra with the same cartan subalgebra. ";
      continue;
    }
    theSAs.theSubalgebras.SetKeyValue(currentCandidate.theHs, currentCandidate);
    theSAs.theSubalgebras.theValues.LastObject().indexInOwner=theSAs.theSubalgebras.theValues.size-1;
  }
  reportStream << "Subalgebra loading done, total  " << theSAs.theSubalgebras.theValues.size << " subalgebras loaded. ";
  theReport.Report(reportStream.str());
  theSAs.ToStringExpressionString=CalculatorConversions::innerStringFromSemisimpleSubalgebras;
//  if (theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection!=0)
//    theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection();
  if (!theSAs.LoadState(currentChainInt, numExploredTypes, numExploredHs, theCommands.Comments))
    return false;
//  reportStream << "<br> Computation state loaded: <br>" << theSAs.ToStringState();
//  theReport.Report(reportStream.str());
  theSAs.flagAttemptToAdjustCentralizers=false;
  if (! theSAs.FindTheSSSubalgebrasContinue())
  { std::stringstream out;
    out << "<br>Failed to realize all subalgebras, computation aborted. The failure report follows. "
    << theSAs.comments << "<br>The progress report for the entire computation follows.<br>"
    << theSAs.ToStringProgressReport();
    return output.AssignValue(out.str(), theCommands);
  }
//  theReport.Report(reportStream.str());
  //stOutput << "<hr>And the pointer is ....: " << &theSAs << "<br>";
  //stOutput << "<hr>And the other pointer is: " << &theCommands.theObjectContainer.theSSsubalgebras[0];
  //stOutput << theCommands.theObjectContainer.ToString();
  theSAs.timeComputationEndInSeconds=theGlobalVariables.GetElapsedSeconds();
  return output.AssignValue(theSAs, theCommands);
}

Calculator tempCalculator; //<-needs a rewrite
std::string CalculatorConversions::innerStringFromSemisimpleSubalgebras(SemisimpleSubalgebras& input)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerStringFromSemisimpleSubalgebras");
  static bool staticFirstRun=true; //<-this needs a rewrite
  if (staticFirstRun)
  { staticFirstRun=false;
    tempCalculator.init();
  }
  Expression tempE;
  FormatExpressions theFormat;
  CalculatorConversions::innerStoreSemisimpleSubalgebras(tempCalculator, input, tempE);
  theFormat.flagUseHTML=true;
  return tempE.ToString(&theFormat);
}

bool CalculatorConversions::innerStoreSemisimpleSubalgebras(Calculator& theCommands, const SemisimpleSubalgebras& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerStoreSemisimpleSubalgebras");
  Expression dynkinTypeE;
  List<std::string> theKeys;
  List<Expression> theValues;
  if (!CalculatorConversions::innerExpressionFromDynkinType(theCommands, input.owner->theWeyl.theDynkinType, dynkinTypeE))
    return false;
  theKeys.AddOnTop("AmbientDynkinType");
  theValues.AddOnTop(dynkinTypeE);

  Expression currentChainE, numericalConvertorE(theCommands);
  currentChainE.MakeSequence(theCommands);
  for (int i=0; i<input.currentSubalgebraChain.size; i++)
  { numericalConvertorE= input.currentSubalgebraChain[i].indexInOwner;
    currentChainE.AddChildOnTop(numericalConvertorE);
  }
  theKeys.AddOnTop("CurrentChain");
  theValues.AddOnTop(currentChainE);
  Expression numTypesExploredE;
  numTypesExploredE.MakeSequence(theCommands);
  for (int i=0; i<input.currentNumLargerTypesExplored.size; i++)
  { numericalConvertorE=input.currentNumLargerTypesExplored[i];
    numTypesExploredE.AddChildOnTop(numericalConvertorE);
  }
  theKeys.AddOnTop("NumExploredTypes");
  theValues.AddOnTop(numTypesExploredE);
  Expression numHsExploredE;
  numHsExploredE.MakeSequence(theCommands);
  for (int i=0; i<input.currentNumHcandidatesExplored.size; i++)
  { numericalConvertorE=input.currentNumHcandidatesExplored[i];
    numHsExploredE.AddChildOnTop(numericalConvertorE);
  }
  theKeys.AddOnTop("NumExploredHs");
  theValues.AddOnTop(numHsExploredE);
  Expression subalgebrasListE, candidateE;
  subalgebrasListE.MakeSequence(theCommands);
  subalgebrasListE.children.Reserve(input.theSubalgebras.theValues.size+1);
  for (int i=0; i<input.theSubalgebras.theValues.size; i++)
  { if (!CalculatorConversions::innerStoreCandidateSA(theCommands, input.theSubalgebras.theValues[i], candidateE))
      return false;
    subalgebrasListE.AddChildOnTop(candidateE);
  }
  theKeys.AddOnTop("Subalgebras");
  theValues.AddOnTop(subalgebrasListE);
  return output.MakeSequenceCommands(theCommands, theKeys, theValues);
//  stOutput << "<hr>Subalgebras to string: " << output.ToString();
  //return true;
}

bool CalculatorConversions::innerExpressionFromMonomialUE
(Calculator& theCommands, const MonomialUniversalEnveloping<RationalFunctionOld>& input, Expression& output, Expression* inputContext)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromMonomialUE");
  if (input.IsConstant())
    return output.AssignValue(1, theCommands);
  ChevalleyGenerator theGen;
  theGen.owner=input.owner;
  Expression chevGenE, powerE, termE;
  List<Expression> theTerms;
  for (int i=0; i<input.generatorsIndices.size; i++)
  { theGen.theGeneratorIndex=input.generatorsIndices[i];
    CalculatorConversions::innerExpressionFromChevalleyGenerator(theCommands, theGen, chevGenE);
    CalculatorConversions::innerExpressionFromRF(theCommands, input.Powers[i], powerE, inputContext);
    termE.MakeXOX(theCommands, theCommands.opThePower(), chevGenE, powerE);
    theTerms.AddOnTop(termE);
  }
  return output.MakeProducT(theCommands, theTerms);
}

bool CalculatorConversions::innerExpressionFromUE
  (Calculator& theCommands, const ElementUniversalEnveloping<RationalFunctionOld>& input, Expression& output, Expression* inputContext)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromUE");
  MonomialCollection<Expression, RationalFunctionOld> theUEE;
  theUEE.MakeZero();
  Expression currentMonE;
  for (int i=0; i<input.size(); i++)
  { if (!CalculatorConversions::innerExpressionFromMonomialUE(theCommands, input[i], currentMonE, inputContext))
      return theCommands << "<hr>Failed to store " << input.ToString();
    theUEE.AddMonomial(currentMonE, input.theCoeffs[i]);
  }
  return output.MakeSum(theCommands, theUEE);
}

bool CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs
(Calculator& theCommands, const Expression& input, ElementSemisimpleLieAlgebra<Rational>& output, SemisimpleLieAlgebra& owner)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs");
  ElementSemisimpleLieAlgebra<AlgebraicNumber> outputAlgebraic;
  if (!CalculatorConversions::innerLoadElementSemisimpleLieAlgebraAlgebraicNumbers(theCommands, input, outputAlgebraic, owner))
    return theCommands << "<hr> Failed to semisimple Lie algebra element from " << input.ToString() << ". ";
  for (int i=0; i<outputAlgebraic.theCoeffs.size; i++)
    if (!outputAlgebraic.theCoeffs[i].IsRational())
      return theCommands << "<hr>From input: " << input.ToString() << ", I managed to extract element: "
      << outputAlgebraic.ToString() << " but that appears to not have rational coefficients.";
  output=outputAlgebraic; //<-implicit conversion here!
//  if (!curGenUErf.GetLieAlgebraElementIfPossible(output))
//    return theCommands << "<hr> Failed to convert the UE element " << curGenUErf.ToString() << " to an honest Lie algebra element. ";
  return true;
}

bool CalculatorConversions::innerLoadElementSemisimpleLieAlgebraAlgebraicNumbers
(Calculator& theCommands, const Expression& input, ElementSemisimpleLieAlgebra<AlgebraicNumber>& output, SemisimpleLieAlgebra& owner)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerLoadElementSemisimpleLieAlgebraAlgebraicNumbers");
  Expression polyFormE;
  Polynomial<AlgebraicNumber> polyForm;
  bool polyFormGood=CalculatorConversions::innerPolynomial<AlgebraicNumber>(theCommands, input, polyFormE);
  if (polyFormGood)
    polyFormGood=polyFormE.IsOfType<Polynomial<AlgebraicNumber> >(&polyForm);
  if (!polyFormGood)
    return theCommands << "<hr>Failed to convert " << input.ToString() << " to polynomial.<hr>";
  ChevalleyGenerator theChevGen;
  ElementSemisimpleLieAlgebra<AlgebraicNumber> currentElt;
  theChevGen.owner=&owner;
  output.MakeZero();
  Expression theContext=polyFormE.GetContext();
  for (int j=0; j<polyForm.size(); j++)
  { const MonomialP& currentMon=polyForm[j];
    int theGenIndex=0;
    if (!currentMon.IsOneLetterFirstDegree(&theGenIndex))
      return theCommands << "<hr>Failed to convert semisimple Lie algebra input to linear poly: " << input.ToString() << ".<hr>";
    Expression singleChevGenE=theContext.ContextGetContextVariable(theGenIndex);
    if (!singleChevGenE.IsListNElements(2))
      return theCommands << "<hr>Failed to convert a summand of " << input.ToString() << " to Chevalley generator.<hr>";
    std::string theLetter;
    if (!singleChevGenE[0].IsAtom(&theLetter) || !singleChevGenE[1].IsSmallInteger(&theChevGen.theGeneratorIndex))
      return theCommands << "<hr>Failed to convert summand " << singleChevGenE.ToString() << " to Chevalley generator of "
      << owner.GetLieAlgebraName();
    bool isGood=true;
    if (theLetter=="g")
    { theChevGen.theGeneratorIndex=owner.GetGeneratorFromDisplayIndex(theChevGen.theGeneratorIndex);
      if (theChevGen.theGeneratorIndex<0 || theChevGen.theGeneratorIndex>=owner.GetNumGenerators())
        isGood=false;
      output.AddMonomial(theChevGen, polyForm.theCoeffs[j]);
    } else if (theLetter=="h")
    { //stOutput << "HERE I AM";
      int theRootIndex=owner.GetRootIndexFromDisplayIndex(theChevGen.theGeneratorIndex);
      if (theRootIndex<0)
        isGood=false;
      else
      { currentElt.MakeHgenerator(owner.theWeyl.RootSystem[theRootIndex], owner);
        currentElt*=polyForm.theCoeffs[j];
        output+=currentElt;
      }
    } else
      isGood=false;
    if (!isGood)
      return theCommands << "<hr>Failed to convert summand " << singleChevGenE.ToString() << " to Chevalley generator of "
      << owner.GetLieAlgebraName();
  }
  return true;
}

bool CalculatorConversions::innerElementUE(Calculator& theCommands, const Expression& input, Expression& output, SemisimpleLieAlgebra& owner)
{ ChevalleyGenerator theChevGen;
  theChevGen.owner=&owner;
  ElementUniversalEnveloping<RationalFunctionOld> outputUE;
  ElementUniversalEnveloping<RationalFunctionOld> currentSummand;
  ElementUniversalEnveloping<RationalFunctionOld> currentMultiplicand;
  MonomialP currentMultiplicandRFpartMon;
  Polynomial<Rational> currentPMultiplicand;
  RationalFunctionOld currentMultiplicandRFpart;
  outputUE.MakeZero(owner);
  Expression polyE;
  if (!CalculatorConversions::innerPolynomial<Rational>(theCommands, input, polyE))
    return theCommands << "<hr>Failed to convert " << input.ToString() << " to polynomial.<hr>";
  Polynomial<Rational> theP;
  if (polyE.IsError() || !polyE.IsOfType<Polynomial<Rational> >(&theP))
    return theCommands << "<hr>Failed to convert " << input.ToString() << " to polynomial. Instead I got " << polyE.ToString() << ". <hr>";
  Expression theContext=polyE.GetContext();
  Expression outputPolyVars;
  outputPolyVars.reset(theCommands, 1);
  outputPolyVars.AddChildAtomOnTop(theCommands.opPolynomialVariables());
  for (int j=0; j<theP.size(); j++)
  { const MonomialP& currentMon=theP[j];
    currentSummand.MakeConst(theP.theCoeffs[j], owner);
    currentMultiplicandRFpartMon.MakeOne();
    for (int i=0; i<currentMon.GetMinNumVars(); i++)
    { int thePower=-1;
      if (!currentMon(i).IsSmallInteger(&thePower))
        return theCommands << "<hr>Failed to convert one of the exponents appearing in " << input.ToString()
        << " to  a small integer polynomial.<hr>";
      if (thePower==0)
        continue;
      Expression singleChevGenE=theContext.ContextGetContextVariable(i);
      if (!singleChevGenE.IsListNElements(2))
        return theCommands << "<hr>Failed to convert " << input.ToString() << " to polynomial.<hr>";
      std::string theLetter;
      if (!singleChevGenE[0].IsAtom(&theLetter) || !singleChevGenE[1].IsSmallInteger(&theChevGen.theGeneratorIndex))
        return theCommands << "<hr>Failed to convert summand " << singleChevGenE.ToString() << " to Chevalley generator of "
        << owner.GetLieAlgebraName();
      bool isGood=true;
      bool isHonestElementUE=true;
      if (theLetter=="g")
      { theChevGen.theGeneratorIndex=owner.GetGeneratorFromDisplayIndex(theChevGen.theGeneratorIndex);
        if (theChevGen.theGeneratorIndex<0 || theChevGen.theGeneratorIndex>=owner.GetNumGenerators())
          isGood=false;
      } else if (theLetter=="h")
      { if (theChevGen.theGeneratorIndex <1 || theChevGen.theGeneratorIndex>owner.GetRank())
          isGood=false;
        else
          theChevGen.theGeneratorIndex=theChevGen.theGeneratorIndex+owner.GetNumPosRoots()-1;
      } else
        isHonestElementUE=false;
      if (!isGood)
        return theCommands << "<hr>Failed to convert summand " << singleChevGenE.ToString() << " to Chevalley generator of "
        << owner.GetLieAlgebraName();
      if (isHonestElementUE)
      { currentMultiplicand.MakeOneGenerator(theChevGen.theGeneratorIndex, owner, 1);
        currentMultiplicand.RaiseToPower(thePower);
        currentSummand*=currentMultiplicand;
      } else
      { int varIndex=outputPolyVars.GetIndexChild(singleChevGenE);
        if (varIndex==-1)
        { outputPolyVars.AddChildOnTop(singleChevGenE);
          varIndex=outputPolyVars.children.size-1;
        } else
          varIndex--;
        currentMultiplicandRFpartMon[varIndex]=thePower;
      }
    }
    currentPMultiplicand.MakeZero();
    currentPMultiplicand.AddMonomial(currentMultiplicandRFpartMon, 1);
    currentMultiplicandRFpart=currentPMultiplicand;
    currentSummand*=currentMultiplicandRFpart;
    outputUE+=currentSummand;
  }
//  stOutput << "<hr>outputUE: " << outputUE.ToString();
  Expression outputContext;
  outputContext.MakeEmptyContext(theCommands);
  outputContext.AddChildOnTop(outputPolyVars);
  outputContext.ContextSetSSLieAlgebrA
  (theCommands.theObjectContainer.theSSLieAlgebras.GetIndex(owner.theWeyl.theDynkinType), theCommands);
  return output.AssignValueWithContext(outputUE, outputContext, theCommands);
}

template <class coefficient>
bool CalculatorConversions::innerExpressionFromPoly
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromPoly");
  if (!input.IsOfType<Polynomial<coefficient> >() )
    return false;
  const Polynomial<coefficient>& thePoly=input.GetValue<Polynomial<coefficient> >();
  Expression theContext=input.GetContext();
  return CalculatorConversions::innerExpressionFromPoly(theCommands, thePoly, output, &theContext);
}

template <class coefficient>
bool CalculatorConversions::innerExpressionFromPoly
(Calculator& theCommands, const Polynomial<coefficient>& input, Expression& output, Expression* inputContext)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromPoly");
  MonomialCollection<Expression, coefficient> theTerms;
  Expression currentBase, currentPower, currentTerm, currentMultTermE;
  if (!input.IsConstant() && inputContext==0)
    theCommands << "While converting polynomial to expression, I was given no variable names. Using the "
    << "default variable names x_1, x_2, ... Please make sure you are not using those variables for other purposes.";
  for (int i=0; i<input.size(); i++)
  { if (input[i].IsConstant())
    { currentTerm.AssignValue(1, theCommands);
      theTerms.AddMonomial(currentTerm, input.theCoeffs[i]);
      continue;
    }
    bool found=false;
    for(int j=0; j<input[i].GetMinNumVars(); j++)
      if (input[i](j)!=0)
      { if (inputContext!=0)
          currentBase = inputContext->ContextGetContextVariable(j);
        else
        { currentBase.reset(theCommands);
          currentBase.AddChildAtomOnTop("x");
          currentBase.AddChildValueOnTop((Rational) j);
        }
        if (input[i](j)==1)
          currentMultTermE=currentBase;
        else
        { currentPower.AssignValue(input[i](j), theCommands);
          currentMultTermE.MakeXOX(theCommands, theCommands.opThePower(), currentBase, currentPower);
        }
        if (!found)
          currentTerm=currentMultTermE;
        else
          currentTerm*=currentMultTermE;
        found=true;
      }
    theTerms.AddMonomial(currentTerm, input.theCoeffs[i]);
  }
//  stOutput << "<br>Extracted expressions: " << theTerms.ToString();
  return output.MakeSum(theCommands, theTerms);
}

bool CalculatorConversions::innerExpressionFromBuiltInType(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromBuiltInType");
  if (input.IsOfType<Polynomial<Rational> >())
    return CalculatorConversions::innerExpressionFromPoly<Rational>(theCommands, input, output);
  if (input.IsOfType<Polynomial<AlgebraicNumber> >())
    return CalculatorConversions::innerExpressionFromPoly<AlgebraicNumber>(theCommands, input, output);
  if (input.IsOfType<RationalFunctionOld>())
    return CalculatorConversions::innerExpressionFromRF(theCommands, input, output);
  return false;
}

bool CalculatorConversions::innerExpressionFromUE(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromUE");
  ElementUniversalEnveloping<RationalFunctionOld> theUE;
  if (!input.IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >(&theUE))
    return theCommands << "<hr>Expression " << input.ToString()
    << " is not an element of universal enveloping, can't convert to expression";
  return CalculatorConversions::innerExpressionFromUE(theCommands, theUE, output);
}

bool CalculatorConversions::innerExpressionFromRF(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromRF");
  if (!input.IsOfType<RationalFunctionOld>() )
    return false;
  const RationalFunctionOld& theRF=input.GetValue<RationalFunctionOld>();
  Expression theContext= input.GetContext();
  return CalculatorConversions::innerExpressionFromRF(theCommands, theRF, output, &theContext);
}

bool CalculatorConversions::innerExpressionFromRF
(Calculator& theCommands, const RationalFunctionOld& input, Expression& output, Expression* inputContext)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerExpressionFromRF");
  Rational aConst;
  if (input.IsConstant(&aConst))
    return output.AssignValue(aConst, theCommands);
  Polynomial<Rational> numP, denP;
  input.GetNumerator(numP);

  if (input.IsConstant() || input.expressionType==input.typePoly)
    return CalculatorConversions::innerExpressionFromPoly<Rational>(theCommands, numP, output, inputContext);
  Expression numE, denE;
  input.GetDenominator(denP);
  Polynomial<Rational> numRescaled=numP;
  Polynomial<Rational> denRescaled=denP;
  Rational topMultiple=  numRescaled.ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
  Rational bottomMultiple=denRescaled.ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
  Rational multipleTopBottom=bottomMultiple/topMultiple;
  numRescaled*=multipleTopBottom.GetNumerator();
  denRescaled*=multipleTopBottom.GetDenominator();
  CalculatorConversions::innerExpressionFromPoly<Rational>(theCommands, numRescaled, numE, inputContext);
  CalculatorConversions::innerExpressionFromPoly<Rational>(theCommands, denRescaled, denE, inputContext);
  return output.MakeXOX(theCommands, theCommands.opDivide(), numE, denE);
}

bool CalculatorConversions::innerRationalFunction(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerRationalFunction");
  Expression intermediate(theCommands);
  if (input.StartsWith(theCommands.opPlus(), 3) ||
      input.StartsWith(theCommands.opTimes(),3) ||
      input.StartsWith(theCommands.opDivide(),3))
  { Expression leftE, rightE;
    if (!CalculatorConversions::innerRationalFunction(theCommands, input[1], leftE) ||
        !CalculatorConversions::innerRationalFunction(theCommands, input[2], rightE) )
      return theCommands << "<hr> Failed to convert " << input[1].ToString() << " and " << input[2].ToString() << " to rational function. ";
    if (leftE.IsError() || rightE.IsError())
      return theCommands << "<hr> Conversion of " << input[1].ToString() << " and " << input[2].ToString() << "  returned error(s): "
      << leftE.ToString() << " and " << rightE.ToString();
//    stOutput << "<br>converting to rf: " << input.ToString();
//    stOutput << "<br> context of leftE: " << leftE.ToString() << " is: " << leftE.GetContext().ToString()
//    << "<br>context of rightE: " << rightE.ToString() << "is : " << rightE.GetContext().ToString();
    intermediate.AddChildOnTop(input[0]);
    intermediate.AddChildOnTop(leftE);
    intermediate.AddChildOnTop(rightE);
    if (input.StartsWith(theCommands.opPlus()))
      return CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrRFToRatOrPolyOrRF(theCommands, intermediate, output);
    if (input.StartsWith(theCommands.opTimes()))
      return CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF(theCommands, intermediate, output);
    if (input.StartsWith(theCommands.opDivide()))
      return CalculatorFunctionsBinaryOps::innerDivideRFOrPolyOrRatByRFOrPoly(theCommands, intermediate, output);
    crash << "This line of code should never be reached, something has gone wrong." << crash;
  }
  int theSmallPower=-1;
  if (input.StartsWith(theCommands.opThePower(), 3) )
  { if (input[2].IsSmallInteger(&theSmallPower))
    { Expression leftE;
      if (!CalculatorConversions::innerRationalFunction(theCommands, input[1], leftE))
        return theCommands << "<hr>CalculatorConversions::innerRationalFunction: failed to convert " << input[1].ToString() << " to rational function. ";
      if (leftE.IsError())
        return theCommands << "<hr> Conversion of " << input[1].ToString() << "  returned error: " << leftE.ToString();
      RationalFunctionOld theRF=leftE.GetValue<RationalFunctionOld>();
      theRF.RaiseToPower(theSmallPower);
      return output.AssignValueWithContext(theRF, leftE.GetContext(), theCommands);
    }
    //return
    theCommands << "<hr>Warning: failed to raise " << input[1].ToString() << " to power " << input[2].ToString()
    << ": failed to convert the power to small integer. I am treating " << input.ToString()
    << " as a single variable: please make sure that is what you want.";
  }
  if (input.IsOfType<RationalFunctionOld>())
  { output=input;
    return true;
  }
  if (input.IsOfType<Polynomial<Rational> >() || input.IsOfType<Rational>())
  { //stOutput << "<hr>Converting to rational function old: " << input.ToString();
    return input.ConvertToType<RationalFunctionOld> (output);
  }
  if (input.IsOfType<AlgebraicNumber>())
  { AlgebraicNumber theNumber= input.GetValue<AlgebraicNumber>();
    Rational theRat;
    if (theNumber.IsRational(&theRat))
    { Expression tempE;
      tempE.AssignValue(theRat, theCommands);
      return tempE.ConvertToType<RationalFunctionOld> (output);
    }
  }
  Expression theContext;
  theContext.ContextMakeContextWithOnePolyVar(theCommands, input);
  //stOutput << "<br> context monomial: " << theContext.ToString();
  //std::string tempS;
  //if (input.IsBuiltInType(&tempS))
  //  stOutput << ", input is of type: " << tempS;
  RationalFunctionOld theRF;
  theRF.MakeOneLetterMoN(0, 1);
  return output.AssignValueWithContext(theRF, theContext, theCommands);
}

bool CalculatorConversions::innerMatrixRational(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerMatrixRational");
//  stOutput << "ere i am, jh";
  Matrix<Rational> outputMat;
  if (input.IsOfType<Matrix<Rational> >())
  { output=input;
    return true;
  }
  if (!theCommands.GetMatriXFromArguments(input, outputMat, 0, -1, 0))
    return theCommands << "<br>Failed to get matrix of rationals. ";
  return output.AssignValue(outputMat, theCommands);
}

bool CalculatorConversions::innerMatrixRationalTensorForm(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerMatrixRationalTensorForm");
  MatrixTensor<Rational> outputMat;
  output=input;
  if (!output.IsOfType<Matrix<Rational> >())
    if (!CalculatorConversions::innerMatrixRational(theCommands, input, output))
      return false;
  if (!output.IsOfType<Matrix<Rational> >())
    return false;
  outputMat=output.GetValue<Matrix<Rational> >();
  return output.AssignValue(outputMat, theCommands);
}

bool CalculatorConversions::innerMakeMatrix(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerMakeMatrix");
  if (CalculatorConversions::innerMatrixRational(theCommands, input, output))
    return true;
  if (CalculatorConversions::innerMatrixAlgebraic(theCommands, input, output))
    return true;
  if (CalculatorConversions::innerMatrixDouble(theCommands, input, output))
    return true;
  return false;
}

bool CalculatorConversions::innerMatrixAlgebraic(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerMatrixAlgebraic");
//  stOutput << "ere i am, jh";
  Matrix<AlgebraicNumber> outputMat;
  if (input.IsOfType<Matrix<AlgebraicNumber> >())
  { output=input;
    return true;
  }
  if (!theCommands.GetMatriXFromArguments(input, outputMat, 0, -1, CalculatorConversions::innerAlgebraicNumber))
    return theCommands << "<br>Failed to get matrix of algebraic numbers. ";
  return output.AssignValue(outputMat, theCommands);
}

bool CalculatorConversions::innerMatrixRationalFunction(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerMatrixRationalFunction");
  Matrix<RationalFunctionOld> outputMat;
  Expression ContextE;
  if (!theCommands.GetMatriXFromArguments(input, outputMat, &ContextE, -1, CalculatorConversions::innerRationalFunction))
    return theCommands << "<hr>Failed to get matrix of rational functions. ";
  //stOutput << "<hr>And the context is: " << ContextE.ToString() << " with outputMat: " << outputMat.ToString();
  output.AssignValueWithContext(outputMat, ContextE, theCommands);
  output.CheckConsistency();
  return true;
}

bool CalculatorConversions::innerLoadFileIntoString(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerLoadFileIntoString");
  std::string theRelativeFileName;
  if (!input.IsOfType<std::string>(&theRelativeFileName))
  { theCommands << "Input of load file string command is supposed to be a string. Converting your expression to a string and using that instead. ";
    theRelativeFileName=input.ToString();
  }
  std::string outputString;
  if (!FileOperations::LoadFileToStringVirtual(theRelativeFileName, outputString, theCommands.Comments))
    return false;
  return output.AssignValue(outputString, theCommands);
}

bool CalculatorConversions::innerMakeElementHyperOctahedral(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerMakeElementHyperOctahedral");
  std::string inputStringFormat;
  ElementHyperoctahedralGroupR2 theElement;
  if (input.IsOfType<std::string>(&inputStringFormat))
  { theElement.MakeFromString(inputStringFormat);
    return output.AssignValue(theElement, theCommands);
  }
  if (input.children.size<3)
    return theCommands << "To make elements of hyperoctahedral group we need at least 3 inputs";
  List<int> oneCycle;
  if (!theCommands.GetVectoRInt(input[1], oneCycle))
    return theCommands << "Failed to extract a cycle structure from the first argument of input: " << input.ToString();
  for (int i=0; i<oneCycle.size; i++)
  { if (oneCycle[i]<1)
      return theCommands << "Your input: " << input[1].ToString() << " had integers that were too small.";
    oneCycle[i]--;
    if (oneCycle[i]>1000)
      return theCommands << "Your input: " << input[1].ToString() << " had integers that were too large.";
  }
  theElement.h.AddCycle(oneCycle);
  for (int i=2; i<input.children.size; i++)
  { if (input[i].IsEqualToOne())
      theElement.k.ToggleBit(i-2);
    else if (!input[i].IsEqualToZero())
      return theCommands << "Your input: " << input.ToString() << " had bit values that were not ones and zeroes.";
  }
  return output.AssignValue(theElement, theCommands);
}
