//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".#include "vpfHeader3Calculator0_Interface.h"
#include "vpfHeader3Calculator0_Interface.h"
#include "vpfHeader3Calculator1_InnerTypedFunctions.h"
static ProjectInformationInstance ProjectInfoVpf5_1cpp(__FILE__, "C++ object <-> calculator expression conversions.");

bool CalculatorConversions::innerExpressionFromChevalleyGenerator(Calculator& theCommands, const ChevalleyGenerator& input, Expression& output)
{ MacroRegisterFunctionWithName("innerExpressionFromChevalleyGenerator");
  input.CheckInitialization();
  output.reset(theCommands, 2);
  Expression generatorLetterE, generatorIndexE;
  if (input.theGeneratorIndex>=input.owneR->GetNumPosRoots() && input.theGeneratorIndex< input.owneR->GetNumPosRoots()+input.owneR->GetRank())
    generatorLetterE.MakeAtom(theCommands.AddOperationNoRepetitionOrReturnIndexFirst("h"), theCommands);
  else
    generatorLetterE.MakeAtom(theCommands.AddOperationNoRepetitionOrReturnIndexFirst("g"), theCommands);
  output.AddChildOnTop(generatorLetterE);
  output.format=output.formatFunctionUseUnderscore;
  generatorIndexE.AssignValue(input.owneR->GetDisplayIndexFromGenerator(input.theGeneratorIndex), theCommands);
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
  WeylGroup theWeyl;
  theWeyl.MakeFromDynkinType(theType);
//  stOutput << "got ere2!";
  return output.AssignValue(theWeyl, theCommands);
}

bool CalculatorConversions::innerDynkinSimpleType(Calculator& theCommands, const Expression& input, DynkinSimpleType& outputMon)
{ MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::DeSerializeMon_DynkinSimpleType");
  Expression rankE, typeLetterE, scaleE;
  if (input.children.size==2)
  { rankE=input[1];
    typeLetterE=input[0];
    if (typeLetterE.StartsWith(theCommands.opThePower(),3))
    { scaleE=typeLetterE[2];
      typeLetterE=typeLetterE[1];
    } else
      scaleE.AssignValue(1, theCommands);
  } else if (input.children.size==3)
  { if (!input.StartsWith(theCommands.opThePower(),3))
      return theCommands << "<hr>Failed to extract rank, type, first co-root length - input has 3 children but was not exponent."
      << " Input is " << input.ToString() << ".";
    scaleE=input[2];
    if (!input[1].IsListNElements(2))
      return theCommands << "<hr>Failed to extract rank, type from " << input[1].ToString() << ". The expression does not have two children.";
    rankE=input[1][1];
    typeLetterE=input[1][0];
  } else
    return theCommands << "<hr>Failed to extract rank, type, first co-root length from expression " << input.ToString();
  Rational theScale;
  if (!scaleE.IsOfType<Rational>(&theScale))
    return theCommands << "<hr>Failed to extract first co-root length: expression " << scaleE.ToString()
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
  return true;
}

bool CalculatorConversions::innerSSLieAlgebra(Calculator& theCommands, const Expression& input, Expression& output, SemisimpleLieAlgebra*& outputPointer)
{ RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("Calculator::innerLoadSSLieAlgebra");
  DynkinType theDynkinType;
//  stOutput << "<br>Now I'm here!";
//  stOutput.flush();
  outputPointer=0;
  if(!CalculatorConversions::innerDynkinType(theCommands, input, theDynkinType))
  { //  stOutput << "got to error";
  //stOutput.flush();
    return output.MakeError("Failed to extract Dynkin type.", theCommands);
  }
//  stOutput << "got to making semisimple Lie algebra of type " << theDynkinType.ToString() << " from input expression: "
//  << input.ToString();
  if (theDynkinType.GetRank()>20)
  { std::stringstream out;
    out << "I have been instructed to allow semisimple Lie algebras of rank 20 maximum. If you would like to relax this limitation edit file " << __FILE__
    << " line " << __LINE__ << ". Note that the Chevalley constant computation reserves a dim(g)*dim(g) table of RAM memory, which means the RAM memory rises with the 4^th power of rank(g). "
    << " You have been warned. Alternatively, you may want to implement a sparse structure constant table (write me an email if you want to do that, I will help you). ";
    return output.MakeError(out.str(), theCommands);
  }
  SemisimpleLieAlgebra tempSSalgebra;
  tempSSalgebra.theWeyl.MakeFromDynkinType(theDynkinType);
  int indexInOwner=theCommands.theObjectContainer.theLieAlgebras.GetIndex(tempSSalgebra);
  //stOutput << "processing " << theDynkinType.ToString();
  //stOutput.flush();

  bool feelsLikeTheVeryFirstTime=(indexInOwner==-1);
  if (feelsLikeTheVeryFirstTime)
  { indexInOwner=theCommands.theObjectContainer.theLieAlgebras.size;
    theCommands.theObjectContainer.theLieAlgebras.AddOnTop(tempSSalgebra);
  }
  outputPointer=&theCommands.theObjectContainer.theLieAlgebras.GetElement(indexInOwner);
  SemisimpleLieAlgebra& theSSalgebra=theCommands.theObjectContainer.theLieAlgebras.GetElement(indexInOwner);
  output.AssignValue(theSSalgebra, theCommands);
  if (feelsLikeTheVeryFirstTime)
  { //crash << theSSalgebra.theWeyl.theDynkinType.ToString() << crash;
    theSSalgebra.ComputeChevalleyConstants(theCommands.theGlobalVariableS);
    Expression tempE;
    theCommands.innerPrintSSLieAlgebra(theCommands, output, tempE, false);
    theCommands << tempE.GetValue<std::string>();
  }
  //theSSalgebra.TestForConsistency(*theCommands.theGlobalVariableS);
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
  output.format=output.formatFunctionUseUnderscore;
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
  if (!CalculatorConversions::innerSSLieAlgebra(theCommands, theOwnerE, tempE, output.owneR))
    return theCommands << "<hr>Failed to extract semisimple Lie algebra from " << theOwnerE.ToString()
    << " while extracting its sl(2) subalgebra.";
  const Expression& theF=input[2];
  const Expression& theE=input[3];
  ElementSemisimpleLieAlgebra<Rational> eltF, eltE;
  if (!CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs(theCommands, theF, eltF, *output.owneR))
    return theCommands << "<hr>Failed to extract f element while loading sl(2) subalgebra<hr>";
  if (!CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs(theCommands, theE, eltE, *output.owneR))
    return theCommands << "<hr>Failed to extract e element while loading sl(2) subalgebra<hr>";
  if (eltE.IsEqualToZero() || eltF.IsEqualToZero())
    return theCommands << "<hr>Failed to load sl(2) subalgebra: either e or f is equal to zero. e and f are: " << eltE.ToString()
    << ", " << eltF.ToString() << ". ";
  if (eltE.GetOwner()!=eltF.GetOwner())
    return theCommands << "<hr>Failed to load sl(2): E and F element of sl(2) have different owners. More precisely, the owner of e is "
    << eltE.GetOwner()->ToString() << " and the owner of f is " << eltF.GetOwner()->ToString();
  output.theE=eltE;
  output.theF=eltF;
  output.owneR=eltE.GetOwner();
  SemisimpleSubalgebras tempSubalgebras;
  tempSubalgebras.owneR=output.owneR;
  int indexSubalgebras=
  theCommands.theObjectContainer.theSSsubalgebras.AddNoRepetitionOrReturnIndexFirst(tempSubalgebras);
  SemisimpleSubalgebras& ownerSubalgebras=theCommands.theObjectContainer.theSSsubalgebras[indexSubalgebras];
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

bool CalculatorConversions::innerLoadSltwoSubalgebras(Calculator& theCommands, const Expression& input, Expression& output)
{
  return false;
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
  CalculatorConversions::innerExpressionFromDynkinType(theCommands, input.theWeylNonEmbeddeD.theDynkinType, currentE);
  keys.AddOnTop("DynkinType");
  values.AddOnTop(currentE);
  Matrix<Rational> conversionMat;
  conversionMat.AssignVectorsToRows(input.theHs);
  currentE.AssignMatrix(conversionMat, theCommands);
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
  if (!CalculatorConversions::innerDynkinType(theCommands, DynkinTypeE, outputSubalgebra.theWeylNonEmbeddeD.theDynkinType))
    return theCommands << "<hr> Failed to load dynkin type of candidate subalgebra from " << DynkinTypeE.ToString() << "<hr>";
  //stOutput << "<br> input[2]: " << input[2].ToString();
  //if (input[2].ToString()=="(C)^{2}_{3}+(A)^{2}_{1}")
  //  stOutput << "<br> loading " << input[2].ToString() << " to get "
  //  << outputSubalgebra.theWeylNonEmbeddeD.theDynkinType.ToString();
//  stOutput << "<hr>Making subalgebra from type " << outputSubalgebra.theWeylNonEmbeddeD.theDynkinType.ToString();
  outputSubalgebra.theWeylNonEmbeddeD.MakeFromDynkinType(outputSubalgebra.theWeylNonEmbeddeD.theDynkinType);
//  stOutput << "Corresponding Co-Cartan symmetric: " << outputSubalgebra.theWeylNonEmbeddeD.CoCartanSymmetric.ToString();
  //int theSmallRank=outputSubalgebra.theWeylNonEmbeddeD.GetDim();
  int theRank=owner.owneR->GetRank();
  Matrix<Rational> theHs;
  if (!theCommands.GetMatrix(ElementsCartanE, theHs, 0, theRank, 0))
    return theCommands << "<hr>Failed to load Cartan elements for candidate subalgebra of type " << outputSubalgebra.theWeylNonEmbeddeD.theDynkinType << "<hr>";
  if (theHs.NumRows!=outputSubalgebra.theWeylNonEmbeddeD.GetDim())
    return theCommands << "<hr>Failed to load cartan elements: I expected " << outputSubalgebra.theWeylNonEmbeddeD.GetDim() << " elements, but failed to get them.";
  List<int> theRanks, theMults;
  outputSubalgebra.theWeylNonEmbeddeD.theDynkinType.GetLettersTypesMults(0, &theRanks, &theMults, 0);
  outputSubalgebra.CartanSAsByComponent.SetSize(outputSubalgebra.theWeylNonEmbeddeD.theDynkinType.GetNumSimpleComponents());
  int componentCounter=-1;
  int counter=-1;
  for (int i=0; i<theMults.size; i++)
    for (int j=0; j<theMults[i]; j++)
    { componentCounter++;
      Vectors<Rational>& currentComponent=outputSubalgebra.CartanSAsByComponent[componentCounter];
      currentComponent.SetSize(theRanks[i]);
      for (int k=0; k<theRanks[i]; k++)
      { counter++;
        theHs.GetVectorFromRow(counter, currentComponent[k]);
      }
    }
  outputSubalgebra.theHs.AssignListList(outputSubalgebra.CartanSAsByComponent);
  outputSubalgebra.thePosGens.SetSize(0);
  outputSubalgebra.theNegGens.SetSize(0);
  if (CalculatorConversions::innerLoadKey(theCommands, input, "generators", generatorsE))
  { generatorsE.Sequencefy();
    ElementSemisimpleLieAlgebra<AlgebraicNumber> curGenAlgebraic;
    for (int i=1; i<generatorsE.children.size; i++)
    { if (!CalculatorConversions::innerLoadElementSemisimpleLieAlgebraAlgebraicNumbers(theCommands, generatorsE[i], curGenAlgebraic, *owner.owneR))
        return theCommands << "<hr>Failed to load semisimple Lie algebra element from expression " << generatorsE[i].ToString() << ". ";
      if (i%2 ==1)
        outputSubalgebra.theNegGens.AddOnTop(curGenAlgebraic);
      else
        outputSubalgebra.thePosGens.AddOnTop(curGenAlgebraic);
    }
    outputSubalgebra.flagSystemProvedToHaveNoSolution=false;
    outputSubalgebra.flagSystemSolved=true;
  }
  SemisimpleLieAlgebra tempSA;
  tempSA.theWeyl.MakeFromDynkinType(outputSubalgebra.theWeylNonEmbeddeD.theDynkinType);
  outputSubalgebra.indexInOwnersOfNonEmbeddedMe=owner.theSubalgebrasNonEmbedded->AddNoRepetitionOrReturnIndexFirst(tempSA);
  owner.theSubalgebrasNonEmbedded->GetElement(outputSubalgebra.indexInOwnersOfNonEmbeddedMe).theWeyl.ComputeRho(true);
  outputSubalgebra.theWeylNonEmbeddeD.ComputeRho(true);
  //stOutput << "Before calling compute system, output subalgebra is: " << outputSubalgebra.ToString();
  outputSubalgebra.computeHsScaledToActByTwo();
  Matrix<Rational> tempMat1;
  outputSubalgebra.theHsScaledToActByTwo.GetGramMatrix(tempMat1, &owner.GetSSowner().theWeyl.CartanSymmetric);
  if (!(outputSubalgebra.theWeylNonEmbeddeD.CoCartanSymmetric== tempMat1))
    return theCommands << "<hr>Failed to load semisimple subalgebra: the gram matrix of the elements of its Cartan, "
    << outputSubalgebra.theHsScaledToActByTwo.ToString() << " is " << tempMat1.ToString() << "; it should be "
    << outputSubalgebra.theWeylNonEmbeddeD.CoCartanSymmetric.ToString() << ".";
  outputSubalgebra.ComputeSystem(false, true);
  if (!outputSubalgebra.ComputeChar(true))
    return theCommands << "<hr>Failed to load semisimple Lie subalgebra: the ambient Lie algebra does not decompose properly over the candidate subalgebra. ";
  if (input.children.size==4)
    if (!outputSubalgebra.CheckGensBracketToHs())
      return theCommands << "<hr>Lie brackets of generators do not equal the desired elements of the Cartan. ";
  //CalculatorConversions::innerLoadFromObject(theCommands,
  return output.MakeError("Candidate subalgebra is not a stand-alone object and its Expression output should not be used. ", theCommands);
}

bool CalculatorConversions::innerLoadSemisimpleSubalgebras(Calculator& theCommands, const Expression& inpuT, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerLoadSemisimpleSubalgebras");
  Expression input=inpuT;
  theCommands.theGlobalVariableS->MaxComputationTimeSecondsNonPositiveMeansNoLimit=10000;
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
  SemisimpleLieAlgebra* ownerSS=0;
  Expression tempE;
  if (!CalculatorConversions::innerSSLieAlgebra(theCommands, theAmbientTypeE, tempE, ownerSS))
    return theCommands << "<hr>Error loading semisimple subalgebras: failed to extract ambient semisimple Lie algebra. ";
//  stOutput << "Loaded type: " << ownerSS->theWeyl.theDynkinType.ToString();
  if (ownerSS==0)
    crash << "Loaded zero subalgebra " << crash;
  SemisimpleSubalgebras tempSAs;
  tempSAs.owneR=ownerSS;
  for (int i =0; i<theCommands.theObjectContainer.theSSsubalgebras.size; i++)
    if (theCommands.theObjectContainer.theSSsubalgebras[i].owneR==0)
      crash << "semisimple subalgebra with index " << i << " has zero owner. " << crash;
  SemisimpleSubalgebras& theSAs=theCommands.theObjectContainer.theSSsubalgebras[theCommands.theObjectContainer.theSSsubalgebras.AddNoRepetitionOrReturnIndexFirst(tempSAs)];
  theSAs.initHookUpPointers
  (*ownerSS, &theCommands.theObjectContainer.theAlgebraicClosure, &theCommands.theObjectContainer.theLieAlgebras,
   &theCommands.theObjectContainer.theSltwoSAs, theCommands.theGlobalVariableS);
  //stOutput << "Owner ss is here:  " << ownerSS->ToString();
  theSAsE.Sequencefy();
  theSAs.theSubalgebras.ReservE(theSAsE.children.size-1);
  theSAs.theSubalgebras.SetSize(0);
  theSAs.theSubalgebrasNonEmbedded->SetExpectedSize(theSAsE.children.size-1);
  ProgressReport theReport(theCommands.theGlobalVariableS);
  theSAs.flagAttemptToSolveSystems=true;
  theSAs.flagComputeModuleDecomposition=true;
  theSAs.flagComputePairingTable=false;
  theSAs.flagComputeNilradicals=false;
  theSAs.theGlobalVariables=theCommands.theGlobalVariableS;
  theSAs.timeComputationStartInSeconds=theCommands.theGlobalVariableS->GetElapsedSeconds();
  for (int i=1; i<theSAsE.children.size; i++)
  { std::stringstream reportStream;
    reportStream << "Loading subalgebra " << i << " out of " << theSAsE.children.size-1 << " from expression "
    << theSAsE[i].ToString();
    //stOutput << reportStream.str();
    theReport.Report(reportStream.str());
    CandidateSSSubalgebra tempCandidate;
    if (!CalculatorConversions::innerCandidateSAPrecomputed(theCommands, theSAsE[i], tempE, tempCandidate, theSAs))
      return theCommands << "<hr>Error loading candidate subalgebra: failed to load candidate number " << i << " of type "
      << tempCandidate.theWeylNonEmbeddeD.theDynkinType.ToString() << " extracted from expression: "
      << theSAsE[i].ToString() << ". <hr>";
    //stOutput << "<hr>read cartan elements: " << tempCandidate.theHs.size;
    theSAs.theSubalgebras.AddOnTop(tempCandidate);
  }
  std::stringstream reportStream;
  reportStream << "Loaded " << theSAs.theSubalgebras.size << " subalgebras total. ";
  theReport.Report(reportStream.str());
  theSAs.ToStringExpressionString=CalculatorConversions::innerStringFromSemisimpleSubalgebras;
//  if (theCommands.theGlobalVariableS->WebServerReturnDisplayIndicatorCloseConnection!=0)
//    theCommands.theGlobalVariableS->WebServerReturnDisplayIndicatorCloseConnection();
  if (!theSAs.LoadState(currentChainInt, numExploredTypes, numExploredHs, theCommands.Comments))
    return false;
  theSAs.FindTheSSSubalgebrasContinue();
//  stOutput << "centralizers off";
  theSAs.flagAttemptToAdjustCentralizers=false;
  theSAs.HookUpCentralizers(true);
  //stOutput << "<hr>And the pointer is ....: " << &theSAs << "<br>";
  //stOutput << "<hr>And the other pointer is: " << &theCommands.theObjectContainer.theSSsubalgebras[0];
  //stOutput << theCommands.theObjectContainer.ToString();
  theSAs.timeComputationEndInSeconds=theCommands.theGlobalVariableS->GetElapsedSeconds();
  return output.AssignValue(theSAs, theCommands);
}

Calculator tempCalculator; //<-needs a rewrite
std::string CalculatorConversions::innerStringFromSemisimpleSubalgebras(SemisimpleSubalgebras& input)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerStringFromSemisimpleSubalgebras");
  static bool staticFirstRun=true; //<-this needs a rewrite
  if (staticFirstRun)
  { staticFirstRun=false;
    tempCalculator.init(*input.theGlobalVariables);
  }
  Expression tempE;
  CalculatorConversions::innerStoreSemisimpleSubalgebras(tempCalculator, input, tempE);
//  stOutput << "<hr>The fucking output: " << tempE.ToString();
  return tempE.ToString();
}

bool CalculatorConversions::innerStoreSemisimpleSubalgebras(Calculator& theCommands, const SemisimpleSubalgebras& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerStoreSemisimpleSubalgebras");
  Expression dynkinTypeE;
  List<std::string> theKeys;
  List<Expression> theValues;
  if (!CalculatorConversions::innerExpressionFromDynkinType(theCommands, input.owneR->theWeyl.theDynkinType, dynkinTypeE))
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
  for (int i=0; i<input.currentNumLargerTypesExplored.size; i++)
  { numericalConvertorE=input.currentNumLargerTypesExplored[i];
    numHsExploredE.AddChildOnTop(numericalConvertorE);
  }
  theKeys.AddOnTop("NumExploredHs");
  theValues.AddOnTop(numHsExploredE);
  Expression subalgebrasListE, candidateE;
  subalgebrasListE.MakeSequence(theCommands);
  subalgebrasListE.children.ReservE(input.theSubalgebras.size+1);
  for (int i=0; i<input.theSubalgebras.size; i++)
  { if (!CalculatorConversions::innerStoreCandidateSA(theCommands, input.theSubalgebras[i], candidateE))
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
  theGen.owneR=input.owneR;
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
  Expression genE;
  if (!CalculatorConversions::innerElementUE(theCommands, input, genE, owner))
    return theCommands << "<hr> Failed to load element UE from " << input.ToString() << ". ";
  if (genE.IsError())
    return theCommands << "<hr>Failed to load generator with error message " << genE.ToString();
  ElementUniversalEnveloping<RationalFunctionOld> curGenUErf=genE.GetValue<ElementUniversalEnveloping<RationalFunctionOld> > ();
  if (!curGenUErf.GetLieAlgebraElementIfPossible(output))
    return theCommands << "<hr> Failed to convert the UE element " << curGenUErf.ToString() << " to an honest Lie algebra element. ";
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
  theChevGen.owneR=&owner;
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
    } else if (theLetter=="h")
    { if (theChevGen.theGeneratorIndex <1 || theChevGen.theGeneratorIndex>owner.GetRank())
        isGood=false;
      else
        theChevGen.theGeneratorIndex=theChevGen.theGeneratorIndex+owner.GetNumPosRoots()-1;
    } else
      isGood=false;
    if (!isGood)
      return theCommands << "<hr>Failed to convert summand " << singleChevGenE.ToString() << " to Chevalley generator of "
      << owner.GetLieAlgebraName();
    output.AddMonomial(theChevGen, polyForm.theCoeffs[j]);
  }
  return true;
}

bool CalculatorConversions::innerElementUE(Calculator& theCommands, const Expression& input, Expression& output, SemisimpleLieAlgebra& owner)
{ ChevalleyGenerator theChevGen;
  theChevGen.owneR=&owner;
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
    { int thePower;
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
  outputContext.ContextSetSSLieAlgebrA(theCommands.theObjectContainer.theLieAlgebras.GetIndex(owner), theCommands);
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
  CalculatorConversions::innerExpressionFromPoly<Rational>(theCommands, numP, numE, inputContext);
  CalculatorConversions::innerExpressionFromPoly<Rational>(theCommands, denP, denE, inputContext);
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
    return theCommands << "<hr>Failed to raise " << input[1].ToString() << " to power " << input[2].ToString()
    << ": failed to convert the power to small integer";
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
  theRF.MakeOneLetterMoN(0, 1, theCommands.theGlobalVariableS);
  return output.AssignValueWithContext(theRF, theContext, theCommands);
}
