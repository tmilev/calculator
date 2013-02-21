//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader4_h_already_included
#define vpfHeader4_h_already_included

#include "vpfHeader2.h"
ProjectInformationInstance ProjectInfoVpfHeader4Serialization
(__FILE__, "Header file: cpp to expression serialization. ");

class Serialization
{
public:
static bool innerSerializePoly
(CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerSerializeSemisimpleLieAlgebra
(CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerSerializeSemisimpleSubalgebras
(CommandList& theCommands, const Expression& input, Expression& output)
;
template <class TemplateMonomial, typename CoefficientType>
static bool SerializeMonCollection
(CommandList& theCommands, const MonomialCollection<TemplateMonomial, CoefficientType>& input,
 const Expression& theContext, Expression& output)
;
template <class TemplateMonomial>
static bool SerializeMon
(CommandList& theCommands, const TemplateMonomial& input, const Expression& theContext,
 Expression& output, bool& isNonConst)
;
};

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
  std::cout << "output: " << output.ToString();
  return true;
}


template <class TemplateMonomial, typename CoefficientType>
bool Serialization::SerializeMonCollection
(CommandList& theCommands, const MonomialCollection<TemplateMonomial, CoefficientType>& input,
 const Expression& theContext, Expression& output)
{ MacroRegisterFunctionWithName("Serialization::SerializeMonCollection");
  Expression termE, coeffE, tempE;
  if (input.IsEqualToZero())
    return output.AssignValue(0, theCommands);
  for (int i=input.size-1; i>=0; i--)
  { TemplateMonomial& currentMon=input[i];
    bool isNonConst=true;
    if (!Serialization::SerializeMon<TemplateMonomial>
        (theCommands, currentMon, theContext, termE, isNonConst))
      return false;
    if (!isNonConst)
      termE.AssignValue(input.theCoeffs[i], theCommands);
    else
      if (input.theCoeffs[i]!=1)
      { tempE=termE;
        coeffE.AssignValue(input.theCoeffs[i], theCommands);
        termE.MakeXOX(theCommands, theCommands.opTimes(), coeffE, tempE);
      }
    if (i==input.size-1)
      output=termE;
    else
    { tempE=output;
      output.MakeXOX(theCommands, theCommands.opPlus(), termE, tempE);
    }
    output.CheckInitialization();
  }
  std::cout << " output: " << output.ToString();
  output.CheckInitialization();
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
#endif
