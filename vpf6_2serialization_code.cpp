#include "vpfHeader2.h"
static ProjectInformationInstance ProjectInfoVpf5_1cpp
(__FILE__, "C++ object to calculator expression serialization/deserialization.");

template <>
bool Serialization::SerializeMon<MonomialP>
(CommandList& theCommands, const MonomialP& input, const Expression& theContext, Expression& output, bool& isNonConst)
{ MacroRegisterFunctionWithName("Serialization::SerializeMon_MonomialP");
  Expression exponentE, monE, tempE, letterE;
  isNonConst=false;
  output.reset(theCommands);
  for (int j=input.GetMinNumVars()-1; j>=0; j--)
    if (input(j)!=0)
    { letterE=theContext.ContextGetContextVariable(j);
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
bool Serialization::SerializeMon<DynkinSimpleType>
(CommandList& theCommands, const DynkinSimpleType& input, const Expression& theContext, Expression& output, bool& isNonConst)
{ MacroRegisterFunctionWithName("Serialization::DynkinSimpleType");
  Expression letterE, rankE, letterAndIndexE, indexE;
  std::string letterS;
  letterS=input.theLetter;
  letterE.MakeAtom(theCommands.operationS.AddNoRepetitionOrReturnIndexFirst(letterS), theCommands);
  indexE.AssignValue(input.lengthFirstCoRootSquared, theCommands);
  rankE.AssignValue(input.theRank, theCommands);
  letterAndIndexE.MakeXOX(theCommands, theCommands.opThePower(), letterE, indexE);
  output.reset(theCommands);
  output.format=output.formatFunctionUseUnderscore;
  output.AddChildOnTop(letterAndIndexE);
  output.AddChildOnTop(rankE);
//  std::cout << "output: " << output.ToString();
  return true;
}

template <class dataType>
bool CommandList::innerExtractPMTDtreeContext
(CommandList& theCommands, const Expression& input, Expression& output)
{ RecursionDepthCounter theRecursionCounter(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("CommandList::innerExtractPMTDtreeContext");
  if (theCommands.RecursionDeptH>theCommands.MaxRecursionDeptH)
  { std::stringstream out;
    out << "Max recursion depth of " << theCommands.MaxRecursionDeptH
    << " exceeded while trying to evaluate to polynomial an expression "
    << "(i.e., your polynomial expression is too large).";
    return output.SetError(out.str(), theCommands);
  }
//  std::cout << "<br>Extracting context from: " << input.ToString();
  if (input.IsListStartingWithAtom(theCommands.opTimes()) ||
      input.IsListStartingWithAtom(theCommands. opPlus()) ||
      input.IsListStartingWithAtom(theCommands.opMinus()) )
  { output.reset(theCommands, 1);
    output.AssignChildAtomValue(0, theCommands.opContext(), theCommands);
    Expression newContext, intermediateContext;
    for (int i=1; i<input.children.size; i++)
      if (theCommands.innerExtractPMTDtreeContext<dataType>(theCommands, input[i], newContext))
      { if (!Expression::ContextMergeContexts(newContext, output, intermediateContext))
          return false;
        output=intermediateContext;
      } else
        return false;
    return true;
  }
  int thePower;
  if (input.IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
    if (input[2].IsSmallInteger(&thePower))
      return theCommands.innerExtractPMTDtreeContext<dataType>(theCommands, input[1], output);
  if(input.IsAtoM(theCommands.opContext()))
  { theCommands.Comments << "Error in context extraction: encountered context keyword. ";
    return false;
  }
  Expression theContext;
  input.GetContextForConversionIgnoreMyContext<dataType>(theContext);
  return Expression::ContextMergeContexts(theContext, input.GetContext(), output);
}

template <class dataType>
bool CommandList::outerExtractAndEvaluatePMTDtree
(CommandList& theCommands, const Expression& input, Expression& output)
{ Expression contextE;
  if (!theCommands.innerExtractPMTDtreeContext<dataType>(theCommands, input, contextE))
    return false;
//  std::cout << "<br>Extracted context from " << input.ToString() << ": "
//  << contextE.ToString();
  return theCommands.EvaluatePMTDtree<dataType>(contextE, input, output);
}

template <class dataType>
bool CommandList::EvaluatePMTDtree
(const Expression& inputContext, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::EvaluatePMTDtree");
  RecursionDepthCounter theRecursionCounter(&this->RecursionDeptH);
  if (this->RecursionDeptH>this->MaxRecursionDeptH)
  { this->Comments << "Max recursion depth of " << this->MaxRecursionDeptH
    << " exceeded while trying to evaluate polynomial expression "
    << "(i.e. your polynomial expression is too large).";
    return false;
  }
  dataType outputData;
//  std::cout << "<br>Context:" <<  inputContext.ToString()
//  << " Evaluating PMTD tree of " << input.ToString();
  if (input.IsListStartingWithAtom(this->opTimes()) ||
      input.IsListStartingWithAtom(this-> opPlus()) )
  { for (int i=1; i<input.children.size; i++)
    { if (!this->EvaluatePMTDtree<dataType>(inputContext, input[i], output))
        return false;
//      std::cout << "<br>Evaluated " << input[i].ToString() << " to " << output.ToString();
      if (input[0].IsAtoM(this->opTimes()))
      { if (i==1)
          outputData=output.GetValuE<dataType>();
        else
        { //std::cout << "<hr>Multiplying: " << outputBuffer.ToString(&this->theGlobalVariableS->theDefaultLieFormat)
          //<< " and " << bufferData.ToString(&this->theGlobalVariableS->theDefaultLieFormat);
          outputData*=output.GetValuE<dataType>();
          //std::cout << "<br>Result: " << outputBuffer.ToString(&this->theGlobalVariableS->theDefaultLieFormat) << "<br>";
        }
      } else if (input[0].IsAtoM(this->opPlus()))
      { //std::cout << "<hr>Status outputBuffer data before addition: " << outputBuffer.ToString(this->theGlobalVariableS->theDefaultLieFormat);
        if (i==1)
        { outputData=output.GetValuE<dataType>();
//          std::cout << "<hr> outputBuffer has been set to: " << outputBuffer.ToString(&this->theGlobalVariableS->theDefaultLieFormat)
//          << ", which should equal the bufferData: " << bufferData.ToString(&this->theGlobalVariableS->theDefaultLieFormat);
        } else
        { //std::cout << "<hr>Adding: " << outputBuffer.ToString(&this->theGlobalVariableS->theDefaultLieFormat)
          //<< " and " << bufferData.ToString(&this->theGlobalVariableS->theDefaultLieFormat);
          outputData+=output.GetValuE<dataType>();
          //std::cout << "<hr>Result: " << outputBuffer.ToString(&this->theGlobalVariableS->theDefaultLieFormat) << "<br>";
        }
      }
    }
    return output.AssignValueWithContext(outputData, inputContext, *this);
  }
  int thePower;
  if (input.IsListNElementsStartingWithAtom(this->opThePower(), 3))
    if (input[2].IsSmallInteger(&thePower))
    { if(!this->EvaluatePMTDtree<dataType>(inputContext, input[1], output))
        return false;
      outputData=output.GetValuE<dataType>();
      outputData.RaiseToPower(thePower);
      return output.AssignValueWithContext(outputData, inputContext, *this);
    }
  Expression intermediate=input;
  Expression tempContext=inputContext;
  if (!intermediate.SetContextAtLeastEqualTo(tempContext))
  { this->Comments << "Failed to set context " << tempContext.ToString()
    << " onto expression " << intermediate.ToString() << "."
    << " <b>This could be a programmSetContextAtLeastEqualToing error.</b> ";
    return false;
  }
  if (!intermediate.ConvertToType<dataType>(output))
  { this->Comments << "Failed to convert " << intermediate.ToString()
    << " to the desired type. ";
    return false;
  }
  return true;
}

bool CommandList::innerSSLieAlgebra
(CommandList& theCommands, const Expression& input, Expression& output)
{ RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("CommandList::innerSSLieAlgebra");
  //std::cout << "<br>Now I'm here!";
  if (!theCommands.innerPolynomial(theCommands, input, output))
    return output.SetError
    ("Failed to extract the semismiple Lie algebra type from " + input.ToString(), theCommands);
  if (output.IsError())
    return true;
  Polynomial<Rational> theType=output.GetValuE<Polynomial<Rational> >();
  Expression theContext=output.GetContext();
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
  SemisimpleLieAlgebra tempSSalgebra;
  DynkinType theDynkinType;
  DynkinSimpleType simpleComponent;
  theDynkinType.MakeZero();
  char theWeylLetter='X';
  for (int i=0; i<theType.size; i++)
  { MonomialP& currentMon=theType[i];
    int variableIndex;
    if (!currentMon.IsOneLetterFirstDegree(&variableIndex))
      return output.SetError
      ("Failed to extract type from monomial "+ currentMon.ToString(&theFormat), theCommands);
    Expression typEE= theContext.ContextGetContextVariable(variableIndex);
    if (typEE.children.size!=2)
      return output.SetError
      ("The monomial "+ currentMon.ToString(&theFormat)+
       " appears not to be a Dynkin simple type ", theCommands);
    Expression rankE=typEE[1];
    Expression typeLetter=typEE[0];
    Rational firstCoRootSquaredLength=2;
    bool foundLengthFromExpression=false;
    if (typeLetter.IsListStartingWithAtom(theCommands.opThePower()))
    { if (!typeLetter[2].IsOfType<Rational>(&firstCoRootSquaredLength))
        return output.SetError
        ("Couldn't extract first co-root length from " + currentMon.ToString(&theFormat), theCommands);
      if (firstCoRootSquaredLength<=0)
        return output.SetError
        ("Couldn't extract positive rational first co-root length from " + currentMon.ToString(&theFormat), theCommands);
      typeLetter.AssignMeMyChild(1);
      foundLengthFromExpression=true;
    }
    std::string theTypeName;
    if (!typeLetter.IsOperation(&theTypeName))
      return output.SetError
      ("I couldn't extract a type letter from "+ currentMon.ToString(&theFormat), theCommands);
    if (theTypeName.size()!=1)
      return output.SetError
      ("The type of a simple Lie algebra must be the letter A, B, C, D, E, F or G.\
        Instead, it is "+ theTypeName + "; error while processing "
       + currentMon.ToString(&theFormat), theCommands);
    theWeylLetter=theTypeName[0];
    if (theWeylLetter=='a') theWeylLetter='A';
    if (theWeylLetter=='b') theWeylLetter='B';
    if (theWeylLetter=='c') theWeylLetter='C';
    if (theWeylLetter=='d') theWeylLetter='D';
    if (theWeylLetter=='e') theWeylLetter='E';
    if (theWeylLetter=='f') theWeylLetter='F';
    if (theWeylLetter=='g') theWeylLetter='G';
    if (!(theWeylLetter=='A' || theWeylLetter=='B' || theWeylLetter=='C'
          || theWeylLetter=='D' || theWeylLetter=='E' || theWeylLetter=='F'
          || theWeylLetter=='G'))
      return output.SetError
      ("The type of a simple Lie algebra must be the letter A, B, C, D, E, F or G; \
       error while processing "
       + currentMon.ToString(&theFormat), theCommands);
    int theRank;
    if (!rankE.IsSmallInteger(&theRank))
      return output.SetError
      ("I wasn't able to extract rank from " + currentMon.ToString(&theFormat), theCommands);
    if (theRank<1 || theRank>20)
      return output.SetError
      ("The rank of a simple Lie algebra must be between 1 and 20; error while processing "
       + currentMon.ToString(&theFormat), theCommands);
    if (theWeylLetter=='E' &&(theRank>8 || theRank<3))
      return output.SetError("Type E must have rank 6,7 or 8 ", theCommands);
    simpleComponent.theLetter=theWeylLetter;
    simpleComponent.theRank= theRank;
    if (!foundLengthFromExpression)
      if (theWeylLetter=='F')
        firstCoRootSquaredLength=1;
    simpleComponent.lengthFirstCoRootSquared= firstCoRootSquaredLength;
    int theMultiplicity=-1;
    if (!theType.theCoeffs[i].IsSmallInteger(&theMultiplicity))
      theMultiplicity=-1;
    if (theMultiplicity<0)
    { std::stringstream out;
      out << "I failed to convert the coefficient " << theType.theCoeffs[i]
      << " of " << currentMon.ToString(&theFormat) << " to a small integer";
      return output.SetError(out.str(), theCommands);
    }
    theDynkinType.AddMonomial(simpleComponent, theMultiplicity);
  }
  if (theDynkinType.GetRank()>20)
  { std::stringstream out;
    out << "I have been instructed to allow semisimple Lie algebras of rank 20 maximum. "
    << " If you would like to relax this limitation edit file " << __FILE__ << " line "
    << __LINE__ << ". Note that the Chevalley constant computation reserves a dim(g)*dim(g)"
    << " table of RAM memory, which means the RAM memory rises with the 4^th power of dim(g). "
    << " You have been warned. "
    << " Alternatively, you may want to implement a sparse structure constant table "
    << "(write me an email if you want to do that, I will help you). ";
    return output.SetError(out.str(), theCommands);
  }
  tempSSalgebra.theWeyl.MakeFromDynkinType(theDynkinType);
  int indexInOwner=theCommands.theObjectContainer.theLieAlgebras.GetIndex(tempSSalgebra);
  bool feelsLikeTheVeryFirstTime=(indexInOwner==-1);
  if (feelsLikeTheVeryFirstTime)
  { indexInOwner=theCommands.theObjectContainer.theLieAlgebras.size;
    theCommands.theObjectContainer.theLieAlgebras.AddOnTop(tempSSalgebra);
  }
  SemisimpleLieAlgebra& theSSalgebra=theCommands.theObjectContainer.theLieAlgebras[indexInOwner];
  output.AssignValue(theSSalgebra, theCommands);
  if (feelsLikeTheVeryFirstTime)
  { theSSalgebra.ComputeChevalleyConstantS(theCommands.theGlobalVariableS);
    Expression tempE;
    theCommands.innerPrintSSLieAlgebra(theCommands, output, tempE, false);
    theCommands.Comments << tempE.GetValuE<std::string>();
  }
  //theSSalgebra.TestForConsistency(*theCommands.theGlobalVariableS);
  return true;
}

bool Serialization::innerSerializeSemisimpleLieAlgebra
  (CommandList& theCommands, const Expression& input, Expression& output)
{ if (!input.IsOfType<SemisimpleLieAlgebra>())
    return output.SetError
    ("Asking serialization of non-semisimple Lie algebra to semisimple Lie algebra not allowed. ",
     theCommands);
  SemisimpleLieAlgebra& owner=input.GetValuENonConstUseWithCaution<SemisimpleLieAlgebra>();
  output.reset(theCommands);
  Expression emptyC;
  emptyC.MakeEmptyContext(theCommands);
  Expression tempE;
  tempE.MakeAtom(theCommands.opSerialization(), theCommands);
  output.AddChildOnTop(tempE);
  tempE.MakeAtom(theCommands.opSSLieAlg(), theCommands);
  output.AddChildOnTop(tempE);
  if (!Serialization::SerializeMonCollection
  (theCommands, owner.theWeyl.theDynkinType, emptyC, tempE))
    return false;
  output.AddChildOnTop(tempE);
  output.format=output.formatDefault;
  return true;
}

bool CommandList::innerSerialize
  (CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("innerSerialize");
  int theType;
  if (!input.IsBuiltInType(&theType))
    return false;
  if (theType==theCommands.opSSLieAlg())
    return Serialization::innerSerializeSemisimpleLieAlgebra(theCommands, input, output);
  if (theType==theCommands.opPoly())
    return Serialization::innerSerializePoly(theCommands, input, output);
  return output.SetError("Serialization not implemented for this data type.", theCommands);
}

bool CommandList::innerDeSerialize
  (CommandList& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListStartingWithAtom(theCommands.opSerialization()))
    return false;
  if (input.children.size<2)
    return false;
  output=input;
  output.children.PopIndexShiftDown(0);
  return true;
}

bool CommandList::innerPolynomial
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerPolynomial");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  //std::cout << "<br>Evaluating innerPolynomial on: " << input.ToString();
  //std::cout << "<br>First elt input is:" << input[0].ToString();
  return theCommands.outerExtractAndEvaluatePMTDtree<Polynomial<Rational> >
  (theCommands, input, output);
}

bool Serialization::innerSerializePoly
  (CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Serialization::innerSerializePoly");
  Polynomial<Rational> thePoly;
  if (!input.IsOfType(&thePoly))
    return output.SetError
    ("Asking serialization of a non-polynomial to a polynomial is not allowed. ", theCommands);
  Expression theContext=input.GetContext();
  Expression tempE, resultE;
  if (!Serialization::SerializeMonCollection<MonomialP, Rational>(theCommands, thePoly, theContext, resultE))
    return false;
  resultE.CheckInitialization();
  output.reset(theCommands);
  tempE.MakeAtom(theCommands.opSerialization(), theCommands);
  output.AddChildOnTop(tempE);
  tempE.MakeAtom(theCommands.operationS.GetIndexIMustContainTheObject("Polynomial"), theCommands);
  output.AddChildOnTop(tempE);
  output.AddChildOnTop(resultE);
  output.format=output.formatDefault;
  return true;
}

bool CommandList::innerRationalFunction
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerPolynomial");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  //std::cout << "<br>Evaluating innerPolynomial on: " << input.ToString();
  //std::cout << "<br>First elt input is:" << input[0].ToString();
  bool result= theCommands.outerExtractAndEvaluatePMTDtree<RationalFunctionOld>
  (theCommands, input, output);
  if (!output.IsOfType<RationalFunctionOld>())
  { std::cout << "<br>This is a programming error: outerExtractAndEvaluatePMTDtree returned true "
    << "from input" << input.ToString()
    << " but the result is not of type RationalFunctionOld, instead it is "
    << output.ToString() << ". "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return result;
}

bool CommandList::innerElementUniversalEnvelopingAlgebra
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerElementUniversalEnvelopingAlgebra");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (input.IsOfType<ElementUniversalEnveloping<RationalFunctionOld > >())
  { output=input;
    return true;
  }
  if (!theCommands.outerExtractAndEvaluatePMTDtree<ElementUniversalEnveloping<RationalFunctionOld > >
      (theCommands, input, output))
    return output.SetError
    ("Failed to convert " +input.ToString() + " to element universal enveloping.", theCommands);
  ElementUniversalEnveloping<RationalFunctionOld> outputUE;
  if (!output.IsOfType(&outputUE))
    return output.SetError("Failed to convert to element universal enveloping.", theCommands);
//  std::cout << "<br>innerElementUniversalEnvelopingAlgebra: output.Context(): "
//  << output.GetContext().ToString();
  outputUE.Simplify(*theCommands.theGlobalVariableS, 1, 0);
  return output.AssignValueWithContext(outputUE, output.GetContext(), theCommands);
}
