//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".#include "vpfHeader3Calculator0_Interface.h"
#include "vpfHeader3Calculator0_Interface.h"
#include "vpfHeader3Calculator1_InnerTypedFunctions.h"
static ProjectInformationInstance ProjectInfoVpf5_1cpp(__FILE__, "C++ object <-> calculator expression serialization/deserialization.");

bool CalculatorBuiltInTypeConversions::innerStoreChevalleyGenerator(Calculator& theCommands, const ChevalleyGenerator& input, Expression& output)
{ MacroRegisterFunctionWithName("innerStoreChevalleyGenerator");
  input.CheckInitialization();
  output.reset(theCommands, 2);
  Expression generatorLetterE, generatorIndexE;
  if (input.theGeneratorIndex>=input.owneR->GetNumPosRoots() && input.theGeneratorIndex< input.owneR->GetNumPosRoots()+input.owneR->GetRank())
    generatorLetterE.MakeAtom(theCommands.AddOperationNoRepetitionOrReturnIndexFirst("h"), theCommands);
  else
    generatorLetterE.MakeAtom(theCommands.AddOperationNoRepetitionOrReturnIndexFirst("g"), theCommands);
  output.AddChildOnTop(generatorLetterE);
  output.format=output.formatFunctionUseUnderscore;
  generatorIndexE.AssignValue
  (input.owneR->GetDisplayIndexFromGenerator(input.theGeneratorIndex), theCommands);
  return output.AddChildOnTop(generatorIndexE);
}

template <>
bool CalculatorBuiltInTypeConversions::DeSerializeMonGetContext<ChevalleyGenerator>(Calculator& theCommands, const Expression& input, Expression& outputContext)
{ if (!input.IsListNElements(4))
  { theCommands << "<hr>Failed to get ChevalleyGenerator context: input is not a sequence of 4 elements, instead it has "
    << input.children.size << " elements, i.e., is " << input.ToString() << "</hr>";
    return false;
  }
  DynkinType theType;
  if (!CalculatorBuiltInTypeConversions::DeSerializeMonCollection(theCommands, input[2], theType))
  { theCommands << "<hr>Failed to load dynkin type from " << input[2].ToString() << ".";
    return false;
  }
  SemisimpleLieAlgebra tempAlgebra;
  tempAlgebra.theWeyl.MakeFromDynkinType(theType);
  bool feelsLikeTheVeryFirstTime=
  theCommands.theObjectContainer.theLieAlgebras.Contains(tempAlgebra);
  int algebraIdentifier=theCommands.theObjectContainer.theLieAlgebras.AddNoRepetitionOrReturnIndexFirst(tempAlgebra);
  if (feelsLikeTheVeryFirstTime)
    theCommands.theObjectContainer.theLieAlgebras.GetElement(algebraIdentifier).ComputeChevalleyConstants(theCommands.theGlobalVariableS);
  outputContext.ContextMakeContextSSLieAlgebrA(algebraIdentifier, theCommands);
  return true;
}

template <>
bool CalculatorBuiltInTypeConversions::DeSerializeMonGetContext<DynkinSimpleType>
(Calculator& theCommands, const Expression& input, Expression& outputContext)
{ outputContext.MakeEmptyContext(theCommands);
  return true;
}

template <>
bool CalculatorBuiltInTypeConversions::DeSerializeMon(Calculator& theCommands, const Expression& input, const Expression& inputContext, ChevalleyGenerator& outputMon)
{ MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::DeSerializeMon");
//  stOutput << "here i am again -!. ";
//  stOutput.flush();
  int AlgIndex=inputContext.ContextGetIndexAmbientSSalg();
  if (AlgIndex==-1)
  { theCommands << "<hr>Can't load Chevalley generator: failed extract ambient algebra index from context " << inputContext.ToString();
    return false;
  }
  if (!input.IsListNElements(4))
  { theCommands << "<hr>Can't load Chevalley generator: input is not a list of 4 elements, instead it has " << input.children.size
    << " elements, i.e., is " << input.ToString();
    return false;
  }
  const Expression& generatorIndexE=input[3];
  int generatorIndex;
  if (!generatorIndexE.IsSmallInteger(&generatorIndex))
    return false;
  std::string theOperation;
  if (!input[1].IsAtom(&theOperation))
  { theCommands << "<hr>Can't load Chevalley generator: second argument is not an operation, instead it is " << input[1].ToString();
    return false;
  }
  outputMon.owneR=&theCommands.theObjectContainer.theLieAlgebras.GetElement(AlgIndex);
//  stOutput << "<hr>owner rank, owner num gens: " << outputMon.owneR->GetRank() << ", "
//  << outputMon.owneR->GetNumGenerators();
  if (theOperation=="getCartanGenerator")
    generatorIndex+=outputMon.owneR->GetNumPosRoots();
  else if (theOperation=="getChevalleyGenerator")
    generatorIndex=outputMon.owneR->GetGeneratorFromDisplayIndex(generatorIndex);
  else
  { theCommands << "<hr>Failed to load Chevalley generator: the generator name was  " << theOperation
    << "; it must either be getCartanGenerator or getChevalleyGenerator.";
    return false;
  }
  if (generatorIndex<0 || generatorIndex>=outputMon.owneR->GetNumGenerators())
  { theCommands << "<hr>Failed to load Chevalley generator: final generator index is " << generatorIndex << ". ";
    return false;
  }
  outputMon.theGeneratorIndex=generatorIndex;
  return true;
}

bool CalculatorBuiltInTypeConversions::innerStoreObject(Calculator& theCommands, const MonomialUniversalEnveloping<RationalFunctionOld>& input, Expression& output, Expression* theContext, bool* isNonConst)
{ output.reset(theCommands);
  if (input.IsEqualToOne())
  { if (isNonConst!=0)
      *isNonConst=false;
    return true;
  }
  ChevalleyGenerator currentGen;
  currentGen.owneR=input.owneR;
  Expression baseE, exponentE, nextTermE;
  currentGen.theGeneratorIndex=input.generatorsIndices[0];
  bool tempNonConst;
  if (!CalculatorBuiltInTypeConversions::innerStoreObject(theCommands, currentGen, baseE, theContext, &tempNonConst))
  { theCommands << "<hr>Failed to store " << currentGen.ToString() << ". ";
    return false;
  }
  if (!input.Powers[0].IsEqualToOne())
  { if (!CalculatorBuiltInTypeConversions::innerStoreObject(theCommands, input.Powers[0], exponentE, theContext))
    { theCommands << "<hr>Failed to store the exponent " << input.Powers[0].ToString();
      if (theContext!=0)
        theCommands << " with context " << theContext->ToString();
      else
        theCommands << " <b>without context!</b>";
      return false;
    }
    output.MakeXOX(theCommands, theCommands.opThePower(), baseE, exponentE);
  } else
    output=baseE;
  for (int i=1; i<input.generatorsIndices.size; i++)
  { currentGen.theGeneratorIndex=input.generatorsIndices[i];
    if (!CalculatorBuiltInTypeConversions::innerStoreObject(theCommands, currentGen, baseE, theContext, &tempNonConst))
    { theCommands << "<hr>Failed to store " << currentGen.ToString() << ". ";
      return false;
    }
    if (!input.Powers[0].IsEqualToOne())
    { if (!CalculatorBuiltInTypeConversions::innerStoreObject(theCommands, input.Powers[0], exponentE, theContext))
      { theCommands << "<hr>Failed to store the exponent " << input.Powers[0].ToString() << " with context "
        << theContext->ToString();
        return false;
      }
      nextTermE.MakeXOX(theCommands, theCommands.opThePower(), baseE, exponentE);
    } else
      nextTermE=baseE;
    output.MakeXOX(theCommands, theCommands.opTimes(), output, nextTermE);
  }
  return true;
}

bool CalculatorBuiltInTypeConversions::innerStoreObject(Calculator& theCommands, const MonomialP& input, Expression& output, Expression* theContext, bool* inputOutputNonConst)
{ MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::SerializeMon_MonomialP");
  if (theContext==0)
    crash << "This is a programming error: it is forbiddeen to call MonomialP storing without providing a context. " << crash;
  Expression exponentE, monE, tempE, letterE;
  bool tempB;
  bool& isNonConst= inputOutputNonConst==0 ? tempB : *inputOutputNonConst;
  isNonConst=false;
  output.reset(theCommands);
  for (int j=input.GetMinNumVars()-1; j>=0; j--)
    if (input(j)!=0)
    { letterE=theContext->ContextGetContextVariable(j);
      if (input(j)==1)
        monE=letterE;
      else
      { exponentE.AssignValue(input(j), theCommands);
        monE.MakeXOX(theCommands, theCommands.opThePower(), letterE, exponentE);
      }
      if (!isNonConst)
        output=monE;
      else
      { tempE=output;
        output.MakeXOX(theCommands, theCommands.opTimes(), monE, tempE);
      }
      isNonConst=true;
    }
  return true;
}

template <>
bool CalculatorBuiltInTypeConversions::DeSerializeMon<DynkinSimpleType>(Calculator& theCommands, const Expression& input, const Expression& inputContext, DynkinSimpleType& outputMon)
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
    { theCommands << "<hr>Failed to extract rank, type, first co-root length - input has 3 children but was not exponent."
      << " Input is " << input.ToString() << ".";
      return false;
    }
    scaleE=input[2];
    if (!input[1].IsListNElements(2))
    { theCommands << "<hr>Failed to extract rank, type from " << input[1].ToString() << ". The expression does not have two children.";
      return false;
    }
    rankE=input[1][1];
    typeLetterE=input[1][0];
  } else
  { theCommands << "<hr>Failed to extract rank, type, first co-root length from expression " << input.ToString();
    return false;
  }
  Rational theScale;
  if (!scaleE.IsOfType<Rational>(&theScale))
  { theCommands << "<hr>Failed to extract first co-root length: expression " << scaleE.ToString()
    << " is not a rational number.";
    return false;
  }
  if (theScale<=0)
  { theCommands << "<hr>Couldn't extract first co-root length: " << theScale.ToString() << " is non-positive.";
    return false;
  }
  std::string theTypeName;
  if (!typeLetterE.IsAtom(&theTypeName))
  { theCommands << "I couldn't extract a type letter from " << typeLetterE.ToString();
    return false;
  }
  if (theTypeName.size()!=1)
  { theCommands << "<hr>Error while extracting Dynkin simple type: The type of a simple Lie algebra must be the letter A, B, C, D, E, F or G."
    << "Instead, it is " << theTypeName + ". Error encountered while processing " << input.ToString();
    return false;
  }
  //stOutput << "here i am again 2. ";
  //stOutput.flush();
  char theWeylLetter=theTypeName[0];
  if (theWeylLetter=='a') theWeylLetter='A';
  if (theWeylLetter=='b') theWeylLetter='B';
  if (theWeylLetter=='c') theWeylLetter='C';
  if (theWeylLetter=='d') theWeylLetter='D';
  if (theWeylLetter=='e') theWeylLetter='E';
  if (theWeylLetter=='f') theWeylLetter='F';
  if (theWeylLetter=='g') theWeylLetter='G';
  if (!(theWeylLetter=='A' || theWeylLetter=='B' || theWeylLetter=='C' || theWeylLetter=='D' || theWeylLetter=='E' || theWeylLetter=='F' || theWeylLetter=='G'))
  { theCommands << "The type of a simple Lie algebra must be the letter A, B, C, D, E, F or G; error while processing " << input.ToString();
    return false;
  }
  int theRank;
  if (!rankE.IsSmallInteger(&theRank))
  { theCommands << "I wasn't able to extract rank from " << input.ToString();
    return false;
  }
  if (theRank<1 || theRank>20)
  { theCommands << "<hr>The rank of a simple Lie algebra must be between 1 and 20; error while processing " << input.ToString();
    return false;
  }
  if (theWeylLetter=='E' &&(theRank>8 || theRank<3))
  { theCommands << "<hr>Type E must have rank 6,7 or 8 ";
    return false;
  }
  if (theWeylLetter=='D' &&(theRank<3))
  { theCommands << "<hr>Type D is expected to have rank 4 or more, your input was of rank " << theRank << ". ";
    return false;
  }
  //stOutput << "here i am again 3. ";
  //stOutput.flush();
  outputMon.MakeArbitrary(theWeylLetter, theRank, theScale);
  return true;
}

bool CalculatorBuiltInTypeConversions::innerStoreObject(Calculator& theCommands, const DynkinSimpleType& input, Expression& output, Expression* theContext, bool* isNonConst)
{ MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::DynkinSimpleType");
  if (isNonConst!=0)
    *isNonConst=true;
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
  output.AddChildOnTop(rankE);
//  stOutput << "output: " << output.ToString();
  return true;
}

bool CalculatorBuiltInTypeConversions::innerSSLieAlgebra(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerSSLieAlgebra");
  CalculatorBuiltInTypeConversions::innerLoadSSLieAlgebra(theCommands, input, output, (SemisimpleLieAlgebra**) 0);
  //theCommands.ToString();
  //stOutput << "The semisimple lie alg: " << output.ToString();

  return true;
}

bool CalculatorBuiltInTypeConversions::innerSSLieAlgebra(Calculator& theCommands, const Expression& input, SemisimpleLieAlgebra*& output)
{ MacroRegisterFunctionWithName("Calculator::innerSSLieAlgebra");
  Expression tempE;
  if (!CalculatorBuiltInTypeConversions::innerLoadSSLieAlgebra(theCommands, input, tempE, &output))
    return false;
  if (tempE.IsError())
    return false;
  return true;
}

bool CalculatorBuiltInTypeConversions::innerLoadWeylGroup(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerLoadWeylGroup");
//  stOutput << "got ere!";
  DynkinType theType;
  if (!CalculatorBuiltInTypeConversions::innerLoadDynkinType(theCommands, input, theType))
    return false;
  WeylGroup theWeyl;
  theWeyl.MakeFromDynkinType(theType);
//  stOutput << "got ere2!";
  return output.AssignValue(theWeyl, theCommands);
}

bool CalculatorBuiltInTypeConversions::innerLoadDynkinType(Calculator& theCommands, const Expression& input, DynkinType& output)
{ RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("Calculator::innerLoadDynkinType");
  MonomialCollection<Expression, Rational> theType;
  if (!theCommands.CollectSummands(theCommands, input, theType))
    return false;
  DynkinSimpleType simpleComponent;
  output.MakeZero();
  Expression emptyE;
  for (int i=0; i<theType.size(); i++)
  { if (!CalculatorBuiltInTypeConversions::DeSerializeMon(theCommands, theType[i], emptyE, simpleComponent))
      return false;
    int theMultiplicity=-1;
    if (!theType.theCoeffs[i].IsSmallInteger(&theMultiplicity))
      theMultiplicity=-1;
    if (theMultiplicity<0)
    { theCommands << "<hr>Failed to convert the coefficient " << theType.theCoeffs[i] << " of " << theType[i].ToString()
      << " to a small positive integer. ";
      return false;
    }
    output.AddMonomial(simpleComponent, theMultiplicity);
  }
  return true;
}

bool CalculatorBuiltInTypeConversions::innerLoadSSLieAlgebra(Calculator& theCommands, const Expression& input, Expression& output, SemisimpleLieAlgebra** outputPointer)
{ RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("Calculator::innerLoadSSLieAlgebra");
  DynkinType theDynkinType;
//  stOutput << "<br>Now I'm here!";
//  stOutput.flush();
  *outputPointer=0;
  if(!CalculatorBuiltInTypeConversions::innerLoadDynkinType(theCommands, input, theDynkinType))
  { //  stOutput << "got to error";
  //stOutput.flush();
    return output.MakeError("Failed to extract Dynkin type.", theCommands);
  }
//  stOutput << "got to making the type, " << theDynkinType.ToString();
//  stOutput.flush();
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
  if (outputPointer!=0)
    *outputPointer=&theCommands.theObjectContainer.theLieAlgebras.GetElement(indexInOwner);
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

bool CalculatorBuiltInTypeConversions::innerStoreSemisimpleLieAlgebra(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsOfType<SemisimpleLieAlgebra>())
    return output.MakeError("Asking serialization of non-semisimple Lie algebra to semisimple Lie algebra not allowed. ", theCommands);
  SemisimpleLieAlgebra& owner=input.GetValueNonConst<SemisimpleLieAlgebra>();
  return CalculatorBuiltInTypeConversions::innerStoreObject(theCommands, owner, output);
}

void Expression::MakeSerialization(const std::string& secondEntry, Calculator& theCommands, int numElementsToReserve)
{ this->reset(theCommands);
  this->children.ReservE(numElementsToReserve+2);
  Expression tempE;
  tempE.MakeAtom(theCommands.opSerialization(), theCommands);
  this->AddChildOnTop(tempE);
  tempE.MakeAtom(theCommands.AddOperationNoRepetitionOrReturnIndexFirst(secondEntry), theCommands);
  this->AddChildOnTop(tempE);
}

bool CalculatorBuiltInTypeConversions::innerStoreObject(Calculator& theCommands, const Rational& input, Expression& output, Expression* theContext)
{ return output.AssignValue(input, theCommands);
}

bool CalculatorBuiltInTypeConversions::innerStoreElementSemisimpleLieAlgebraRationals(Calculator& theCommands, const ElementSemisimpleLieAlgebra<Rational>& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::innerStoreElementSemisimpleLieAlgebraRational");
  Expression tempContext;
  tempContext.MakeEmptyContext(theCommands);
  if (!input.IsEqualToZero())
  { tempContext.ContextMakeContextSSLieAlgebrA(theCommands.theObjectContainer.theLieAlgebras.AddNoRepetitionOrReturnIndexFirst(*input.GetOwner()), theCommands);
  }
  CalculatorBuiltInTypeConversions::innerStoreMonCollection(theCommands, input, output, &tempContext);
  return true;
}

bool CalculatorBuiltInTypeConversions::innerStoreElementSemisimpleLieAlgebraAlgebraicNumbers(Calculator& theCommands, const ElementSemisimpleLieAlgebra<AlgebraicNumber>& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::innerStoreElementSemisimpleLieAlgebraAlgebraicNumbers");
  Expression tempContext;
  tempContext.MakeEmptyContext(theCommands);
  if (!input.IsEqualToZero())
  { tempContext.ContextMakeContextSSLieAlgebrA(theCommands.theObjectContainer.theLieAlgebras.AddNoRepetitionOrReturnIndexFirst(*input.GetOwner()), theCommands);
  }
  CalculatorBuiltInTypeConversions::innerStoreMonCollection(theCommands, input, output, &tempContext);
  return true;
}

bool CalculatorBuiltInTypeConversions::innerStoreObject(Calculator& theCommands, const slTwoSubalgebra& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::innerStoreObject");
  output.MakeSerialization("LoadSltwoSubalgebra", theCommands);
  Expression tempE;
  CalculatorBuiltInTypeConversions::innerStoreObject(theCommands, input.theF, tempE);
  output.AddChildOnTop(tempE);
  CalculatorBuiltInTypeConversions::innerStoreObject(theCommands, input.theE, tempE);
  output.AddChildOnTop(tempE);

  /*List<int> highestWeights;
  List<int> multiplicitiesHighestWeights;
  List<int> weightSpaceDimensions;
  ElementSemisimpleLieAlgebra<Rational> theH, theE, theF;
  ElementSemisimpleLieAlgebra<Rational> bufferHbracketE, bufferHbracketF, bufferEbracketF;
  SemisimpleLieAlgebra* owneR;
  SltwoSubalgebras* container;
  Rational LengthHsquared;
  int indexInContainer;
  List<int> IndicesContainingRootSAs;
  List<int> IndicesMinimalContainingRootSA;
  List<List<int> > HighestWeightsDecompositionMinimalContainingRootSA;
  List<List<int> > MultiplicitiesDecompositionMinimalContainingRootSA;
  Vectors<Rational> preferredAmbientSimpleBasis;
  Vector<Rational> hCharacteristic;
  Vector<Rational> hElementCorrespondingToCharacteristic;
  Vectors<Rational> hCommutingRootSpaces;
  Vectors<Rational> RootsWithScalar2WithH;
  DynkinDiagramRootSubalgebra DiagramM;
  DynkinDiagramRootSubalgebra CentralizerDiagram;
  PolynomialSubstitution<Rational> theSystemToBeSolved;
  Matrix<Rational> theSystemMatrixForm;
  Matrix<Rational> theSystemColumnVector;
  bool DifferenceTwoHsimpleRootsIsARoot;*/
  return true;
}

bool CalculatorBuiltInTypeConversions::innerLoadFromObject(Calculator& theCommands, const Expression& input, slTwoSubalgebra& output)
{ MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::innerLoadFromObject slTwoSubalgebra");
  if (!input.IsListNElements(3))
  { theCommands << "<hr>input of innerLoadFromObject has " << input.children.size << " children, 3 expected. ";
    return false;
  }
  const Expression& theF=input[1];
  const Expression& theE=input[2];
  ElementSemisimpleLieAlgebra<Rational> eltF, eltE;
  if (!CalculatorBuiltInTypeConversions::DeSerializeMonCollection(theCommands, theF, eltF))
  { theCommands << "<hr>Failed to extract f element while loading sl(2) subalgebra<hr>";
    return false;
  }
  if (!CalculatorBuiltInTypeConversions::DeSerializeMonCollection(theCommands, theE, eltE))
  { theCommands << "<hr>Failed to extract e element while loading sl(2) subalgebra<hr>";
    return false;
  }
  if (eltE.IsEqualToZero() || eltF.IsEqualToZero())
  { theCommands << "<hr>Failed to load sl(2) subalgebra: either e or f is equal to zero. e and f are: " << eltE.ToString()
    << ", " << eltF.ToString() << ". ";
    return false;
  }
  if (eltE.GetOwner()!=eltF.GetOwner())
  { theCommands << "<hr>Failed to load sl(2): E and F element of sl(2) have different owners. More precisely, the owner of e is "
    << eltE.GetOwner()->ToString() << " and the owner of f is " << eltF.GetOwner()->ToString();
    return false;
  }
  output.theE=eltE;
  output.theF=eltF;
  output.owneR=eltE.GetOwner();
  SemisimpleSubalgebras tempSubalgebras;
  tempSubalgebras.owneR=output.owneR;
  int indexSubalgebras=
  theCommands.theObjectContainer.theSSsubalgebras.AddNoRepetitionOrReturnIndexFirst(tempSubalgebras);
  SemisimpleSubalgebras& ownerSubalgebras=
  theCommands.theObjectContainer.theSSsubalgebras[indexSubalgebras];
  output.container=&ownerSubalgebras.theSl2s;
  return true;
}

bool CalculatorBuiltInTypeConversions::innerLoadSltwoSubalgebra(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::innerLoadSltwoSubalgebra");
  slTwoSubalgebra tempSL2;
  if (!CalculatorBuiltInTypeConversions::innerLoadFromObject(theCommands, input, tempSL2))
  { theCommands << "<hr>Failed to load sl(2) subalgebra. ";
    return false;
  }
  return output.AssignValue(tempSL2.ToString(), theCommands);
}

bool CalculatorBuiltInTypeConversions::innerStoreObject(Calculator& theCommands, const SltwoSubalgebras& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::innerStoreObject");
  output.MakeSerialization("LoadSlTwoSubalgebras", theCommands, 1);
  Expression theSequence, tempE;
  theSequence.reset(theCommands);
  tempE.MakeAtom(theCommands.opSequence(), theCommands);
  theSequence.children.ReservE(input.size+1);
  theSequence.AddChildOnTop(tempE);
  for (int i=0; i<input.size; i++)
  { if (!CalculatorBuiltInTypeConversions::innerStoreObject(theCommands, input[i], tempE))
      return false;
    theSequence.AddChildOnTop(tempE);
  }
  output.AddChildOnTop(theSequence);
  return true;
}

bool CalculatorBuiltInTypeConversions::innerStoreObject(Calculator& theCommands, const SemisimpleLieAlgebra& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::innerStoreObject");
  output.MakeSerialization("SemisimpleLieAlgebra", theCommands, 1);
//  stOutput << "<hr>" << output.ToString() << "<br>";
  Expression emptyC, tempE;
  emptyC.MakeEmptyContext(theCommands);
  if (!CalculatorBuiltInTypeConversions::innerStoreMonCollection
      (theCommands, input.theWeyl.theDynkinType, tempE, &emptyC))
    return false;
//  stOutput << "<br>The mon collection: " << tempE.ToString();
  output.AddChildOnTop(tempE);
  output.format=output.formatDefault;
  return true;
}

bool CalculatorBuiltInTypeConversions::innerLoadSltwoSubalgebras(Calculator& theCommands, const Expression& input, Expression& output)
{
  return false;
}

bool CalculatorBuiltInTypeConversions::innerStoreSemisimpleSubalgebrasFromExpression(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("innerStoreSemisimpleSubalgebrass");
  if (!input.IsOfType<SemisimpleSubalgebras>())
    return false;
  SemisimpleSubalgebras& theSubalgebras=input.GetValueNonConst<SemisimpleSubalgebras>();
  return CalculatorBuiltInTypeConversions::innerStoreSemisimpleSubalgebras(theCommands, theSubalgebras, output);
}

bool CalculatorBuiltInTypeConversions::innerStoreCandidateSA(Calculator& theCommands, const CandidateSSSubalgebra& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::innerStoreObject CandidateSSSubalgebra");
  output.MakeSerialization("LoadCandidateSubalgebra", theCommands);
  Expression emptyContext;
  emptyContext.MakeEmptyContext(theCommands);
  Expression tempE;
  CalculatorBuiltInTypeConversions::innerStoreMonCollection(theCommands, input.theWeylNonEmbeddeD.theDynkinType, tempE, &emptyContext);
  output.AddChildOnTop(tempE);
  CalculatorBuiltInTypeConversions::innerStoreObject(theCommands, input.theHs, tempE);
  output.AddChildOnTop(tempE);
//  ElementSemisimpleLieAlgebra<Rational> convertedToRational;
  if (input.flagSystemSolved)
  { Expression listGenerators;
    listGenerators.reset(theCommands);
    listGenerators.AddChildAtomOnTop(theCommands.opSequence());
    for (int i=0; i<input.theNegGens.size; i++)
    { CalculatorBuiltInTypeConversions::innerStoreElementSemisimpleLieAlgebraAlgebraicNumbers(theCommands, input.theNegGens[i], tempE);
      listGenerators.AddChildOnTop(tempE);
      CalculatorBuiltInTypeConversions::innerStoreElementSemisimpleLieAlgebraAlgebraicNumbers(theCommands, input.thePosGens[i], tempE);
      listGenerators.AddChildOnTop(tempE);
    }
    output.AddChildOnTop(listGenerators);
  }
  //for (int i=0; i<this->
  return true;
}

bool CalculatorBuiltInTypeConversions::innerLoadCandidateSA(Calculator& theCommands, const Expression& input, Expression& output, CandidateSSSubalgebra& outputSubalgebra, SemisimpleSubalgebras& owner)
{ MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::innerLoadCandidateSA");
  if (!input.IsListNElements(4) && !input.IsListNElements(5))
  { theCommands << "<hr>Failed to load candidate subalgebra: I expect to get a list of 4 or 5 children, but got one with "
    << input.children.size << " children instead.<hr> ";
    return false;
  }
  outputSubalgebra.owner=&owner;
  Expression tempE;
  if (!CalculatorBuiltInTypeConversions::DeSerializeMonCollection(theCommands, input[2], outputSubalgebra.theWeylNonEmbeddeD.theDynkinType))
  { theCommands << "<hr> Failed to load dynkin type of candidate subalgebra from "<< input[2].ToString() << "<hr>";
    return false;
  }
  //stOutput << "<br> input[2]: " << input[2].ToString();
  //if (input[2].ToString()=="(C)^{2}_{3}+(A)^{2}_{1}")
  //  stOutput << "<br> loading " << input[2].ToString() << " to get "
  //  << outputSubalgebra.theWeylNonEmbeddeD.theDynkinType.ToString();
  //stOutput << "<hr>Making subalgebra from type " << outputSubalgebra.theWeylNonEmbeddeD.theDynkinType.ToString();
  outputSubalgebra.theWeylNonEmbeddeD.MakeFromDynkinType(outputSubalgebra.theWeylNonEmbeddeD.theDynkinType);
  //int theSmallRank=outputSubalgebra.theWeylNonEmbeddeD.GetDim();
  int theRank=owner.owneR->GetRank();
  Matrix<Rational> theHs;
  if (!theCommands.GetMatrix(input[3], theHs, 0, theRank, 0))
  { theCommands << "<hr>Failed to load matrix of Cartan elements for candidate subalgebra of type " << outputSubalgebra.theWeylNonEmbeddeD.theDynkinType << "<hr>";
    return false;
  }
  if (theHs.NumRows!=outputSubalgebra.theWeylNonEmbeddeD.GetDim())
  { theCommands << "<hr>Failed to load cartan elements: I expected " << outputSubalgebra.theWeylNonEmbeddeD.GetDim() << " elements, but failed to get them.";
    return false;
  }
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
  Matrix<Rational> tempMat1;
  outputSubalgebra.theHs.GetGramMatrix(tempMat1, &owner.GetSSowner().theWeyl.CartanSymmetric);
  if (!(outputSubalgebra.theWeylNonEmbeddeD.CartanSymmetric== tempMat1))
  { theCommands << "<hr>Failed to load semisimple subalgebra: the gram matrix of the elements of its cartan is "
    << tempMat1.ToString() << " but it should be " << outputSubalgebra.theWeylNonEmbeddeD.CartanSymmetric.ToString() << "instead.";
    return false;
  }
  outputSubalgebra.thePosGens.SetSize(0);
  outputSubalgebra.theNegGens.SetSize(0);
  if (input.children.size==5)
  { Expression theGensE=input[4];
    theGensE.Sequencefy();
    ElementSemisimpleLieAlgebra<AlgebraicNumber> curGenAlgebraic;
    for (int i=1; i<theGensE.children.size; i++)
    { if (!CalculatorBuiltInTypeConversions::innerLoadElementSemisimpleLieAlgebraAlgebraicNumbers(theCommands, theGensE[i], curGenAlgebraic, *owner.owneR))
      { theCommands << "<hr>Failed to load semisimple Lie algebra element from expression " << theGensE[i].ToString() << ". ";
        return false;
      }
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
  outputSubalgebra.ComputeSystem(false, true);
  if (!outputSubalgebra.ComputeChar(true))
  { theCommands << "<hr>Failed to load semisimple Lie subalgebra: the ambient Lie algebra does not decompose properly over the candidate subalgebra. ";
    return false;
//    outputSubalgebra.theCharNonPrimalFundCoords.MakeZero(owner.owneR);
//    outputSubalgebra.theCharFundamentalCoordsRelativeToCartan.MakeZero(owner.owneR);
  }
  if (input.children.size==5)
    if (!outputSubalgebra.CheckGensBracketToHs())
    { theCommands << "<hr>Lie brackets of generators do not equal the desired elements of the Cartan. ";
      return false;
    }

  //CalculatorBuiltInTypeConversions::innerLoadFromObject(theCommands,
  return output.MakeError("Candidate subalgebra is not a stand-alone object and its Expression output should not be used. ", theCommands);
}

bool CalculatorBuiltInTypeConversions::innerLoadSemisimpleSubalgebras(Calculator& theCommands, const Expression& inpuT, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::innerLoadSemisimpleSubalgebras");
  Expression input=inpuT;
  theCommands.theGlobalVariableS->MaxComputationTimeSecondsNonPositiveMeansNoLimit=10000;
  if (input.children.size!= 3)
  { theCommands << "<hr>Error loading semisimple subalgebras: I expect input with 3 children, got "
    << input.children.size << " children instead.<hr>";
    return false;
  }
  SemisimpleLieAlgebra* ownerSS=0;
  if (!CalculatorBuiltInTypeConversions::innerLoadSSLieAlgebra(theCommands, input[1], output, &ownerSS))
  { theCommands << "<hr>Error loading semisimple subalgebras: failed to extract ambient semisimple Lie algebra. ";
    return false;
  }
  if (ownerSS==0)
    crash << "Loaded zero subalgebra " << crash;
  SemisimpleSubalgebras tempSAs;
  tempSAs.owneR=ownerSS;
  stOutput << "here be i!";
  for (int i =0; i<theCommands.theObjectContainer.theSSsubalgebras.size; i++)
    if (theCommands.theObjectContainer.theSSsubalgebras[i].owneR==0)
      crash << "semisimple subalgebra with index " << i << " has zero owner. " << crash;
  SemisimpleSubalgebras& theSAs=theCommands.theObjectContainer.theSSsubalgebras[theCommands.theObjectContainer.theSSsubalgebras.AddNoRepetitionOrReturnIndexFirst(tempSAs)];
  stOutput << "here be i! - 2";
  theSAs.initHookUpPointers
  (*ownerSS, &theCommands.theObjectContainer.theAlgebraicClosure, &theCommands.theObjectContainer.theLieAlgebras,
   &theCommands.theObjectContainer.theSltwoSAs, theCommands.theGlobalVariableS);
  //FormatExpressions tempFormat;
//  stOutput << ownerSS->ToString();
  Expression theCandidatesE=input[2];
  theCandidatesE.Sequencefy();
  theSAs.theSubalgebraCandidates.ReservE(theCandidatesE.children.size-1);
  Expression tempE;
  theSAs.theSubalgebraCandidates.SetSize(0);
  theSAs.theSubalgebrasNonEmbedded->SetExpectedSize(theCandidatesE.children.size-1);
  ProgressReport theReport(theCommands.theGlobalVariableS);
  theSAs.flagAttemptToSolveSystems=true;
  theSAs.flagComputeModuleDecomposition=true;
  theSAs.flagComputePairingTable=false;
  theSAs.flagComputeNilradicals=false;
  theSAs.theGlobalVariables=theCommands.theGlobalVariableS;
  theSAs.timeComputationStartInSeconds=theCommands.theGlobalVariableS->GetElapsedSeconds();
  for (int i=1; i<theCandidatesE.children.size; i++)
  { std::stringstream reportStream;
    reportStream << "Loading subalgebra " << i << " out of " << theCandidatesE.children.size-1;
    theReport.Report(reportStream.str());
    CandidateSSSubalgebra tempCandidate;
    if (!CalculatorBuiltInTypeConversions::innerLoadCandidateSA(theCommands, theCandidatesE[i], tempE, tempCandidate, theSAs))
    { theCommands << "<hr>Error loading candidate subalgebra: failed to load candidate number " << i << " of type "
      << tempCandidate.theWeylNonEmbeddeD.theDynkinType.ToString() << ". <hr>";
      return false;
    }
    //stOutput << "<hr>read cartan elements: " << tempCandidate.theHs.size;
    theSAs.theSubalgebraCandidates.AddOnTop(tempCandidate);
  }
  stOutput << "centralizers off";
  theSAs.flagAttemptToAdjustCentralizers=false;
  theSAs.HookUpCentralizers(true);
  //stOutput << "<hr>And the pointer is ....: " << &theSAs << "<br>";
  //stOutput << "<hr>And the other pointer is: " << &theCommands.theObjectContainer.theSSsubalgebras[0];
  //stOutput << theCommands.theObjectContainer.ToString();
  theSAs.timeComputationEndInSeconds=theCommands.theGlobalVariableS->GetElapsedSeconds();
  return output.AssignValue(theSAs, theCommands);
}

bool CalculatorBuiltInTypeConversions::innerStoreSemisimpleSubalgebras(Calculator& theCommands, const SemisimpleSubalgebras& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::innerStoreSemisimpleSubalgebras");
  output.MakeSerialization("LoadSemisimpleSubalgebras", theCommands);
  Expression tempE, tempE2, candidateE;
  if (!CalculatorBuiltInTypeConversions::innerStoreMonCollection(theCommands, input.owneR->theWeyl.theDynkinType, tempE))
    return false;
  output.AddChildOnTop(tempE);
  tempE.reset(theCommands);
  tempE.children.ReservE(input.theSubalgebraCandidates.size+1);
  tempE2.MakeAtom(theCommands.opSequence(), theCommands);
  tempE.AddChildOnTop(tempE2);
  for (int i=0; i<input.theSubalgebraCandidates.size; i++)
  { if (!CalculatorBuiltInTypeConversions::innerStoreCandidateSA(theCommands, input.theSubalgebraCandidates[i], candidateE))
      return false;
    tempE.AddChildOnTop(candidateE);
  }
  output.AddChildOnTop(tempE);
  //if (!CalculatorBuiltInTypeConversions::innerStoreObject(theCommands, input.theSl2s, tempE))
  //  return false;
  //output.AddChildOnTop(tempE);
  return true;
}

bool CalculatorBuiltInTypeConversions::innerStoreUE(Calculator& theCommands, const Expression& input, Expression& output)
{ ElementUniversalEnveloping<RationalFunctionOld> theUE;
  if (!input.IsOfType(&theUE))
    return output.MakeError("To ask to store a non-elementUE as an elementUE is not allowed", theCommands);
  output.MakeSerialization("UE", theCommands, 1);
  Expression tempE;
  Expression theContext=input.GetContext();
  if(!CalculatorBuiltInTypeConversions::innerStoreMonCollection(theCommands, theUE, tempE, &theContext))
  { theCommands << "<hr>Failed to store " << theUE.ToString();
    return false;
  }
  return output.AddChildOnTop(tempE);
}

bool CalculatorBuiltInTypeConversions::innerLoadElementSemisimpleLieAlgebraRationalCoeffs
(Calculator& theCommands, const Expression& input, ElementSemisimpleLieAlgebra<Rational>& output, SemisimpleLieAlgebra& owner)
{ Expression genE;
  ElementUniversalEnveloping<RationalFunctionOld> curGenUErf;
  if (!CalculatorBuiltInTypeConversions::innerLoadElementSemisimpleLieAlgebraRationalCoeffs(theCommands, input, genE, owner))
  { theCommands << "<hr> Failed to load element UE from " << input.ToString() << ". ";
    return false;
  }
  if (genE.IsError())
  { theCommands << "<hr>Failed to load generator with error message " << genE.ToString();
    return false;
  }
  curGenUErf=genE.GetValue<ElementUniversalEnveloping<RationalFunctionOld> > ();
  if (!curGenUErf.GetLieAlgebraElementIfPossible(output))
  { theCommands << "<hr> Failed to convert the UE element " << curGenUErf.ToString() << " to an honest Lie algebra element. ";
    return false;
  }
  return true;
}

bool CalculatorBuiltInTypeConversions::innerLoadElementSemisimpleLieAlgebraAlgebraicNumbers
(Calculator& theCommands, const Expression& input, ElementSemisimpleLieAlgebra<AlgebraicNumber>& output, SemisimpleLieAlgebra& owner)
{ MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::innerLoadElementSemisimpleLieAlgebraAlgebraicNumbers");
  Expression polyFormE;
  Polynomial<AlgebraicNumber> polyForm;
  bool polyFormGood=CalculatorBuiltInTypeConversions::innerPolynomial<AlgebraicNumber>(theCommands, input, polyFormE);
  if (polyFormGood)
    polyFormGood=polyFormE.IsOfType<Polynomial<AlgebraicNumber> >(&polyForm);
  if (!polyFormGood)
  { theCommands << "<hr>Failed to convert " << input.ToString() << " to polynomial.<hr>";
    return false;
  }
  ChevalleyGenerator theChevGen;
  theChevGen.owneR=&owner;
  output.MakeZero();
  Expression theContext=polyFormE.GetContext();
  for (int j=0; j<polyForm.size(); j++)
  { const MonomialP& currentMon=polyForm[j];
    int theGenIndex=0;
    if (!currentMon.IsOneLetterFirstDegree(&theGenIndex))
    { theCommands << "<hr>Failed to convert semisimple Lie algebra input to linear poly: " << input.ToString() << ".<hr>";
      return false;
    }
    Expression singleChevGenE=theContext.ContextGetContextVariable(theGenIndex);
    if (!singleChevGenE.IsListNElements(2))
    { theCommands << "<hr>Failed to convert a summand of " << input.ToString() << " to Chevalley generator.<hr>";
      return false;
    }
    std::string theLetter;
    if (!singleChevGenE[0].IsAtom(&theLetter) || !singleChevGenE[1].IsSmallInteger(&theChevGen.theGeneratorIndex))
    { theCommands << "<hr>Failed to convert summand " << singleChevGenE.ToString() << " to Chevalley generator of "
      << owner.GetLieAlgebraName();
      return false;
    }
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
    { theCommands << "<hr>Failed to convert summand " << singleChevGenE.ToString() << " to Chevalley generator of "
      << owner.GetLieAlgebraName();
      return false;
    }
    output.AddMonomial(theChevGen, polyForm.theCoeffs[j]);
  }
  return true;
}

bool CalculatorBuiltInTypeConversions::innerLoadElementSemisimpleLieAlgebraRationalCoeffs(Calculator& theCommands, const Expression& input, Expression& output, SemisimpleLieAlgebra& owner)
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
  if (!CalculatorBuiltInTypeConversions::innerPolynomial<Rational>(theCommands, input, polyE))
  { theCommands << "<hr>Failed to convert " << input.ToString() << " to polynomial.<hr>";
    return false;
  }
  Polynomial<Rational> theP;
  if (polyE.IsError() || !polyE.IsOfType<Polynomial<Rational> >(&theP))
  { theCommands << "<hr>Failed to convert " << input.ToString() << " to polynomial. Instead I got " << polyE.ToString() << ". <hr>";
    return false;
  }
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
      { theCommands << "<hr>Failed to convert one of the exponents appearing in " << input.ToString()
        << " to  a small integer polynomial.<hr>";
        return false;
      }
      if (thePower==0)
        continue;
      Expression singleChevGenE=theContext.ContextGetContextVariable(i);
      if (!singleChevGenE.IsListNElements(2))
      { theCommands << "<hr>Failed to convert " << input.ToString() << " to polynomial.<hr>";
        return false;
      }
      std::string theLetter;
      if (!singleChevGenE[0].IsAtom(&theLetter) || !singleChevGenE[1].IsSmallInteger(&theChevGen.theGeneratorIndex))
      { theCommands << "<hr>Failed to convert summand " << singleChevGenE.ToString() << " to Chevalley generator of "
        << owner.GetLieAlgebraName();
        return false;
      }
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
      { theCommands << "<hr>Failed to convert summand " << singleChevGenE.ToString() << " to Chevalley generator of "
        << owner.GetLieAlgebraName();
        return false;
      }
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

bool CalculatorBuiltInTypeConversions::innerStorePoly(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::innerStorePoly");
  Polynomial<Rational> thePoly;
  if (!input.IsOfType(&thePoly))
    return output.MakeError("To ask to store a non-polynomial to a polynomial is not allowed. ", theCommands);
  Expression theContext=input.GetContext();
  Expression resultE;
  if (!CalculatorBuiltInTypeConversions::innerStoreMonCollection(theCommands, thePoly, resultE, &theContext))
    return false;
  resultE.CheckInitialization();
  output.MakeSerialization("Polynomial", theCommands, 1);
  output.AddChildOnTop(resultE);
  output.format=output.formatDefault;
  output.CheckInitialization();
  return true;
}

bool CalculatorBuiltInTypeConversions::innerStoreRationalFunction(Calculator& theCommands, const Expression& input, Expression& output)
{ RationalFunctionOld theRF;
  if (!input.IsOfType(&theRF))
    return output.MakeError("To ask to store a non-rational function as a rational function is not allowed.", theCommands);
  Expression contextE=input.GetContext();
  return CalculatorBuiltInTypeConversions::innerStoreObject(theCommands, theRF, output, &contextE);
}

bool CalculatorBuiltInTypeConversions::innerStoreObject(Calculator& theCommands, const RationalFunctionOld& input, Expression& output, Expression* theContext)
{ if (input.expressionType==input.typeRational)
    return output.AssignValue(input.ratValue, theCommands);
  Polynomial<Rational> theNumerator;
  input.GetNumerator(theNumerator);
  if (input.expressionType==input.typePoly)
    return CalculatorBuiltInTypeConversions::innerStoreMonCollection(theCommands, theNumerator, output, theContext);
  if (input.expressionType!=input.typeRationalFunction)
    crash << "This is a programming error: I am processing a rational function which is not of type rational polynomial or honest rataional "
    << "function. Something has gone very wrong. " << crash;
  Polynomial<Rational> theDenominator;
  input.GetDenominator(theDenominator);
  Expression denE, numE;
  if (!CalculatorBuiltInTypeConversions::innerStoreMonCollection(theCommands, theNumerator, numE, theContext))
  { theCommands << "<hr>Failed to serialize numerator of rational function. ";
    return false;
  }
  if (!CalculatorBuiltInTypeConversions::innerStoreMonCollection(theCommands, theDenominator, denE, theContext))
  { theCommands << "<hr>Failed to serialize denominator of rational function. ";
    return false;
  }
  return output.MakeXOX(theCommands, theCommands.opDivide(), numE, denE);
}

bool CalculatorBuiltInTypeConversions::innerRationalFunction(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorBuiltInTypeConversions::innerRationalFunction");
  Expression intermediate(theCommands);
  if (input.StartsWith(theCommands.opPlus(), 3) ||
      input.StartsWith(theCommands.opTimes(),3) ||
      input.StartsWith(theCommands.opDivide(),3))
  { Expression leftE, rightE;
    if (!CalculatorBuiltInTypeConversions::innerRationalFunction(theCommands, input[1], leftE) ||
        !CalculatorBuiltInTypeConversions::innerRationalFunction(theCommands, input[2], rightE) )
    { theCommands << "<hr> Failed to convert " << input[1].ToString() << " and " << input[2].ToString() << " to rational function. ";
      return false;
    }
    if (leftE.IsError() || rightE.IsError())
    { theCommands << "<hr> Conversion of " << input[1].ToString() << " and " << input[2].ToString() << "  returned error(s): "
      << leftE.ToString() << " and " << rightE.ToString();
      return false;
    }
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
  { //stOutput << "here be i!";
    if (input[2].IsSmallInteger(&theSmallPower))
    { Expression leftE;
      if (!CalculatorBuiltInTypeConversions::innerRationalFunction(theCommands, input[1], leftE))
      { theCommands << "<hr>CalculatorBuiltInTypeConversions::innerRationalFunction: failed to convert " << input[1].ToString() << " to rational function. ";
        return false;
      }
      if (leftE.IsError())
      { theCommands << "<hr> Conversion of " << input[1].ToString() << "  returned error: " << leftE.ToString();
        return false;
      }
      RationalFunctionOld theRF=leftE.GetValue<RationalFunctionOld>();
      theRF.RaiseToPower(theSmallPower);
      return output.AssignValueWithContext(theRF, leftE.GetContext(), theCommands);
    }
    theCommands << "<hr>Failed to raise " << input[1].ToString() << " to power " << input[2].ToString()
    << ": failed to convert the power to small integer";
    return false;
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
