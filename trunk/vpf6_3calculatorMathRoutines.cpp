//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
#include "vpfHeader3Calculator2_InnerFunctions.h"
#include "vpfImplementationHeader2Math3_WeylAlgebra.h"
#include "vpfImplementationHeader2Math15_UniversalEnveloping.h"

ProjectInformationInstance ProjectInfoVpf6_3cpp(__FILE__, "Calculator built-in functions. ");

template <class theType>
bool MathRoutines::GenerateVectorSpaceClosedWRTOperation
(List<theType>& inputOutputElts, int upperDimensionBound, void (*theBinaryOperation)(const theType& left, const theType& right, theType& output),
 GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("MathRoutines::GenerateVectorSpaceClosedWRTOperation");
  inputOutputElts[0].GaussianEliminationByRowsDeleteZeroRows(inputOutputElts);
  theType theOpResult;
  ProgressReport theReport1(theGlobalVariables), theReport2(theGlobalVariables);
  if (theGlobalVariables!=0)
    theReport1.Report("Extending vector space to closed with respect to binary operation. ");
  List<theType> theEltsForGaussianElimination=inputOutputElts;
  for (int i=0; i<inputOutputElts.size; i++)
    for (int j=i; j<inputOutputElts.size; j++)
    { theBinaryOperation(inputOutputElts[i], inputOutputElts[j], theOpResult);
      //int oldNumElts=inputOutputElts.size;
      theEltsForGaussianElimination.AddOnTop(theOpResult);
      theEltsForGaussianElimination[0].GaussianEliminationByRowsDeleteZeroRows(theEltsForGaussianElimination);
      if (theEltsForGaussianElimination.size>inputOutputElts.size)
        inputOutputElts.AddOnTop(theOpResult);
      //if (oldNumElts<inputOutputElts.size)
        //std::cout << "<hr>Operation between <br>" << inputOutputElts[i].ToString() << " <br> " << inputOutputElts[j].ToString() << " <br>=<br> "
        //<< theOpResult.ToString();
      if (upperDimensionBound>0 && inputOutputElts.size>upperDimensionBound)
        return false;
      if (theGlobalVariables!=0)
      { std::stringstream reportStream;
        reportStream << "Accounted operation between elements " << i+1 << " and " << j+1 << " out of " << inputOutputElts.size;
        theReport2.Report(reportStream.str());
      }
    }
  inputOutputElts[0].GaussianEliminationByRowsDeleteZeroRows(inputOutputElts);
  return true;
}

bool CommandListFunctions::innerGenerateVectorSpaceClosedWRTLieBracket(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerGenerateVectorSpaceClosedWRTLieBracket");
  Vector<ElementWeylAlgebra<Rational> > theOps;
  Expression theContext;
  if (!input.children.size>1)
    return false;
  int upperBound=-1;
  if (!input[1].IsSmallInteger(&upperBound))
  { theCommands.Comments << "<hr>Failed to extract upper bound for the vector space dimension from the first argument. ";
    return false;
  }
  Expression inputModded=input;
  inputModded.children.RemoveIndexShiftDown(1);

  if (!theCommands.GetVectorFromFunctionArguments(inputModded, theOps, &theContext))
  { Vector<ElementUniversalEnveloping<RationalFunctionOld> > theLieAlgElts;
    theContext.MakeEmptyContext(theCommands);
    if (!theCommands.GetVectorFromFunctionArguments(inputModded, theLieAlgElts, &theContext))
    { theCommands.Comments << "<hr>Failed to extract elements of weyl algebra and failed to extract elements of UE algebra from input "
      << input.ToString();
      return false;
    }
    FormatExpressions theFormat;
    theContext.ContextGetFormatExpressions(theFormat);
    std::stringstream out;
    out << "Starting elements: <br>";
    for (int i=0; i<theLieAlgElts.size; i++)
      out << CGI::GetHtmlMathSpanPure(theLieAlgElts[i].ToString(&theFormat)) << "<br>";
    bool success=MathRoutines::GenerateVectorSpaceClosedWRTLieBracket(theLieAlgElts, upperBound, theCommands.theGlobalVariableS);
    if (!success)
      out << "<br>Did not succeed with generating vector space, instead got a vector space with basis " << theLieAlgElts.size << " exceeding the limit. "
      << "The basis generated before exceeding the limit was: " << theLieAlgElts.ToString();
    else
    { out << "<br>Lie bracket generates vector space of dimension " << theLieAlgElts.size << " with basis:";
      for (int i=0; i<theLieAlgElts.size; i++)
      { out << "<br>";
        if (theLieAlgElts.size>50)
          out << theLieAlgElts[i].ToString(&theFormat);
        else
          out << CGI::GetHtmlMathSpanPure(theLieAlgElts[i].ToString(&theFormat));
      }
    }
    return output.AssignValue(out.str(), theCommands);
  }
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
  std::stringstream out;
  out << "Starting elements: <br>";
  for (int i=0; i<theOps.size; i++)
    out << CGI::GetHtmlMathSpanPure(theOps[i].ToString(&theFormat)) << "<br>";
  bool success=MathRoutines::GenerateVectorSpaceClosedWRTLieBracket(theOps, upperBound, theCommands.theGlobalVariableS);
  if (!success)
    out << "<br>Did not succeed with generating vector space, instead got a vector space with basis " << theOps.size << " exceeding the limit. "
    << "The basis generated before exceeding the limit was: " << theOps.ToString();
  else
  { out << "<br>Lie bracket generates vector space of dimension " << theOps.size << " with basis:";
    for (int i=0; i<theOps.size; i++)
    { out << "<br>";
      if (theOps.size>50)
        out << theOps[i].ToString(&theFormat);
      else
        out << CGI::GetHtmlMathSpanPure(theOps[i].ToString(&theFormat));
    }
  }

  return output.AssignValue(out.str(), theCommands);
}

bool CommandListFunctions::innerFourierTransformEWA(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerFourierTransformEWA");
  if (!input.IsOfType<ElementWeylAlgebra<Rational> >())
    return false;
  ElementWeylAlgebra<Rational> theElt;
  input.GetValue<ElementWeylAlgebra<Rational> >().FourierTransform(theElt);
  return output.AssignValueWithContext(theElt, input.GetContext(), theCommands);
}

bool CommandListFunctions::innerCasimirWRTlevi(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerCasimir");
  RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(3))
    return false;
  SemisimpleLieAlgebra* theSSalg=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(Serialization::innerSSLieAlgebra, input[1], theSSalg))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  if (theCommands.theGlobalVariableS->MaxComputationTimeSecondsNonPositiveMeansNoLimit<50)
    theCommands.theGlobalVariableS->MaxComputationTimeSecondsNonPositiveMeansNoLimit=50;
  Vector<Rational> leviSelection;
  if(!theCommands.GetVectoR(input[2], leviSelection, 0, theSSalg->GetRank()))
  { theCommands.Comments << "<hr>Failed to extract parabolic selection. ";
    return false;
  }
  Selection theParSel;
  theParSel=leviSelection;
  theParSel.InvertSelection();
  ElementUniversalEnveloping<RationalFunctionOld> theCasimir;
  theCasimir.MakeCasimirWRTLeviParabolic(*theSSalg, theParSel);
//  theCasimir.Simplify(*theCommands.theGlobalVariableS);
  Expression contextE;
  contextE.MakeContextSSLieAlg(theCommands, *theSSalg);
  return output.AssignValueWithContext(theCasimir, contextE, theCommands);
}

bool CommandListFunctions::innerLog(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerLog");
  double theArgument;
  if (input.IsEqualToZero())
    return output.SetError("Logarithm of zero is undefined.", theCommands);
  if (!input.IsRealDouble(&theArgument))
  { if (input.IsAtomGivenData(theCommands.opE()))
      return output.AssignValue(1, theCommands);
    return false;
  }
  if (theArgument>0)
    return output.AssignValue(FloatingPoint::log(theArgument), theCommands);
  theArgument*=-1;
  Expression iE, ipiE, piE, lnPart;
  iE.MakeSqrt(theCommands, -1);
  piE.MakeAtom(theCommands.opPi(), theCommands);
  ipiE.MakeXOX(theCommands, theCommands.opTimes(), piE, iE);
  lnPart.AssignValue(FloatingPoint::log(theArgument), theCommands);
  return output.MakeXOX(theCommands, theCommands.opPlus(), lnPart, ipiE);
}

bool CommandListFunctions::innerSin(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerSin");
  if (input.IsAtomGivenData(theCommands.opPi()))
    return output.AssignValue(0, theCommands);
  double theArgument;
  if (!input.IsRealDouble(&theArgument))
    return false;
  return output.AssignValue(FloatingPoint::sin(theArgument), theCommands);
}

bool CommandListFunctions::innerFactorial(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerSin");
  int inputInt;
  if (!input.IsSmallInteger(&inputInt))
    return false;
  Rational result;
  return output.AssignValue(result.Factorial(inputInt), theCommands);
}

bool CommandListFunctions::innerCos(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerCos");
  if (input.IsAtomGivenData(theCommands.opPi()))
    return output.AssignValue(-1, theCommands);
  double theArgument;
  if (!input.IsRealDouble(&theArgument))
    return false;
  return output.AssignValue(FloatingPoint::cos(theArgument), theCommands );
}

bool CommandListFunctions::innerTan(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerTan");
  Expression num, den;
  num.MakeOX(theCommands, theCommands.opSin(), input);
  den.MakeOX(theCommands, theCommands.opCos(), input);
  return output.MakeXOX(theCommands, theCommands.opDivide(), num, den);
}

bool CommandListFunctions::innerCot(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerTan");
  Expression num, den;
  num.MakeOX(theCommands, theCommands.opCos(), input);
  den.MakeOX(theCommands, theCommands.opSin(), input);
  return output.MakeXOX(theCommands, theCommands.opDivide(), num, den);
}

bool CommandListFunctions::innerSec(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerSec");
  Expression num, den;
  num.AssignValue(1, theCommands);
  den.MakeOX(theCommands, theCommands.opCos(), input);
  return output.MakeXOX(theCommands, theCommands.opDivide(), num, den);
}

bool CommandListFunctions::innerCsc(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerCsc");
  Expression num, den;
  num.AssignValue(1, theCommands);
  den.MakeOX(theCommands, theCommands.opSin(), input);
  return output.MakeXOX(theCommands, theCommands.opDivide(), num, den);
}

bool CommandListFunctions::innerCompositeSequenceDereference(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerCompositeSequenceDereference");
  if (input.children.size!=2)
    return false;
  if (!input[0].IsListStartingWithAtom(theCommands.opSequence()))
    return false;
  int theIndex;
  if (!input[1].IsSmallInteger(&theIndex))
    return false;
  if (theIndex>0 && theIndex<input[0].children.size)
  { output=input[0][theIndex];
    return true;
  }
  return false;
}

bool CommandListFunctions::innerGetAlgebraicNumberFromMinPoly(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerGetAlgebraicNumberFromMinPoly");
  Expression polyE;
  if (!Serialization::innerPolynomial<AlgebraicNumber>(theCommands, input, polyE) )
  { theCommands.Comments << "<hr>Failed to convert " << input.ToString() << " to polynomial. ";
    return false;
  }
  Polynomial<AlgebraicNumber> thePoly;
  if (!polyE.IsOfType<Polynomial<AlgebraicNumber> >(&thePoly))
  { theCommands.Comments << "<hr>Failed to convert " << input.ToString() << " to polynomial, instead got " << polyE.ToString();
    return false;
  }
  if (polyE.GetNumContextVariables()!=1)
  { theCommands.Comments << "<hr>After conversion, I got the polynomial " << polyE.ToString() << ", which is not in one variable.";
    return false;
  }
  AlgebraicNumber theAN;
  if (!theAN.ConstructFromMinPoly(thePoly, theCommands.theObjectContainer.theAlgebraicClosure, theCommands.theGlobalVariableS))
    return false;
  return output.AssignValue(theAN, theCommands);
}

bool CommandListFunctions::innerCompositeApowerBevaluatedAtC(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerCompositeApowerBevaluatedAtC");
  //std::cout << "<hr>input be: " << input.ToString();
  if (!input.IsListNElements())
    return false;
  const Expression& firstE=input[0];
  if (!firstE.IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
    return false;
  theCommands.CheckInputNotSameAsOutput(input, output);
  Expression finalBase;
  finalBase.reset(theCommands, input.children.size);
  finalBase.AddChildOnTop(input[0][1]);
  for (int i=1; i<input.children.size; i++)
    finalBase.AddChildOnTop(input[i]);
  return output.MakeXOX(theCommands, theCommands.opThePower(), finalBase, input[0][2]);
}

bool CommandListFunctions::innerConstantFunction(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerConstantFunction");
  if (!input.IsListNElements())
    return false;
  if (!input[0].IsConstantNumber())
    return false;
  output=input[0];
  return true;
}

bool CommandListFunctions::innerCompositeConstTimesAnyActOn(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerCompositeConstTimesAnyActOn");
  if (!input.IsListNElements())
    return false;
  if (!input[0].IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
  if (!input[0][1].IsConstantNumber())
    return false;
  Expression functionActsOnE;
  functionActsOnE.reset(theCommands);
  functionActsOnE.AddChildOnTop(input[0][2]);
  functionActsOnE.AddChildOnTop(input[1]);
  theCommands.CheckInputNotSameAsOutput(input, output);
  return output.MakeXOX(theCommands, theCommands.opTimes(), input[0][1], functionActsOnE);
}

bool CommandListFunctions::innerCompositeEWAactOnPoly(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerCompositeEWAactOnPoly");
  if (input.children.size!=2)
    return false;
  Expression theEWAE=input[0];
  Expression theArgument=input[1];
  if (!theEWAE.IsListStartingWithAtom(theCommands.opElementWeylAlgebra()))
    return false;
  if (!theArgument.IsBuiltInType())
    return false;
  if (!theEWAE.MergeContexts(theEWAE, theArgument))
    return false;
  Polynomial<Rational> theArgumentPoly;
  Expression theArgumentConverted;
  if (theArgument.ConvertToType<Polynomial<Rational> >(theArgumentConverted))
    theArgumentPoly=theArgumentConverted.GetValue<Polynomial<Rational> >();
  else if (theArgument.ConvertToType<ElementWeylAlgebra<Rational> >(theArgumentConverted))
  { if(!theArgumentConverted.GetValue<ElementWeylAlgebra<Rational> >().IsPolynomial(&theArgumentPoly))
      return false;
  } else
    return false;
  const ElementWeylAlgebra<Rational>& theEWA=theEWAE.GetValue<ElementWeylAlgebra<Rational> >();
  if (theEWA.HasNonSmallPositiveIntegerDerivation())
  { theCommands.Comments << "<hr> I cannot apply " << theEWA.ToString() << " onto " << theArgumentPoly.ToString() << " as "
    << "the differential operator contains non-integral differential operator exponents. ";
    return false;
  }
  if (!theEWA.ActOnPolynomial(theArgumentPoly))
  { std::stringstream out;
    out << "Failed to act by operator " << theEWA.ToString() << " on polynomial " << theArgumentPoly.ToString()
    << " (possibly the weyl algebra element has non-integral exponents)";
    return output.SetError(out.str(), theCommands);
  }
  return output.AssignValueWithContext(theArgumentPoly, theEWAE.GetContext(), theCommands);
}

bool CommandListFunctions::innerMakeMakeFile(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerMakeMakeFile");
  ProjectInformation& theProjectInfo=ProjectInformation::GetMainProjectInfo();
  List<std::string> cppFilesNoExtension;
  for (int i=0; i<theProjectInfo.theFiles.size; i++)
  { std::string theFileNameWithPath=theProjectInfo.theFiles[i].FileName;
    if (theFileNameWithPath[theFileNameWithPath.size()-1]=='h')
      continue;
    theFileNameWithPath.resize(theFileNameWithPath.size()-4);
    std::string theFileNameNoPathNoExtensionReversed, theFileNameNoPathNoExtension;
    for (int j=theFileNameWithPath.size()-1; j>=0; j--)
      if (theFileNameWithPath[j]=='/')
        break;
      else
        theFileNameNoPathNoExtensionReversed.push_back(theFileNameWithPath[j]);
    for (int j=theFileNameNoPathNoExtensionReversed.size()-1; j>=0; j--)
      theFileNameNoPathNoExtension.push_back(theFileNameNoPathNoExtensionReversed[j]);
    cppFilesNoExtension.AddOnTop(theFileNameNoPathNoExtension);
  }
  std::fstream theFileStream;
  XML::OpenFileCreateIfNotPresent(theFileStream, theCommands.PhysicalPathOutputFolder+"makefile", false, true, false);
  std::stringstream outHtml;
  theFileStream << "all: directories calculator\n\n";
  theFileStream << "directories: Debug\n";
  theFileStream << "Debug:\n" << "\tmkdir ./Debug\n";
  theFileStream << "calculator: ";
  for (int i=0; i<cppFilesNoExtension.size; i++)
    theFileStream << cppFilesNoExtension[i] << ".o ";
  theFileStream << "\n\tg++ -std=c++11 -pthread ";
  for (int i=0; i<cppFilesNoExtension.size; i++)
    theFileStream << cppFilesNoExtension[i] << ".o ";
  theFileStream << "-o ./Debug/calculator\n\n";
  for (int i=0; i<cppFilesNoExtension.size; i++)
    theFileStream << cppFilesNoExtension[i] << ".o: " << cppFilesNoExtension[i] << ".cpp\n\tg++ -std=c++0x -pthread -c " << cppFilesNoExtension[i] << ".cpp\n\n";
  outHtml << "<a href=\" " << theCommands.DisplayPathOutputFolder << "makefile" << "\"> makefile </a>";
  return output.AssignValue(outHtml.str(), theCommands);
}

bool CommandListFunctions::innerDifferentiateConstPower(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerDifferentiateConstPower");
  //////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtom())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1];
  const Expression& theArgument=input[2];
  //////////////////////
  if (!theArgument.IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
    return false;
  if (!theArgument[2].IsConstantNumber())
    return false;
  Expression theMonomial, theTerm, theExponent, basePrime, minusOne;
  minusOne.AssignValue<Rational>(-1, theCommands);
  theExponent.MakeXOX(theCommands, theCommands.opPlus(), theArgument[2], minusOne);
  theMonomial.MakeXOX(theCommands, theCommands.opThePower(), theArgument[1], theExponent);
  basePrime.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[1]);
  theTerm.MakeXOX(theCommands, theCommands.opTimes(), theArgument[2], theMonomial);
  return output.MakeXOX(theCommands, theCommands.opTimes(), theTerm, basePrime);
}

bool CommandListFunctions::innerDifferentiateAPowerB(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerDifferentiateAPowerB");
  //////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtom())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1];
  const Expression& theArgument=input[2];
  //////////////////////
  //d/dx a^b= d/dx(e^{b\\ln a}) = a^b d/dx(b\\log a)
  if (!theArgument.IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
    return false;
  Expression logBase, exponentTimesLogBase, derivativeExponentTimesLogBase;
  logBase.reset(theCommands, 2);
  logBase.AddChildAtomOnTop(theCommands.opLog());
  logBase.AddChildOnTop(theArgument[1]);
  exponentTimesLogBase.MakeXOX(theCommands, theCommands.opTimes(), theArgument[2], logBase);
  derivativeExponentTimesLogBase.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, exponentTimesLogBase);
  return output.MakeXOX(theCommands, theCommands.opTimes(), theArgument, derivativeExponentTimesLogBase);
}

bool CommandListFunctions::innerDifferentiateConstant(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerDifferentiateConstant");
  //////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtom())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
    << " - possible user typo?";
  const Expression& theArgument=input[2];
  //////////////////////
  if (!theArgument.IsConstantNumber())
    return false;
  return output.AssignValue<Rational>(0, theCommands);
}

bool CommandListFunctions::innerDifferentiateX(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerDifferentiateX");
  //////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtom())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1];
  const Expression& theArgument=input[2];
  //////////////////////
  if (theArgument!=theDOvar)
    return false;
  return output.AssignValue<Rational>(1, theCommands);
}

bool CommandListFunctions::innerDifferentiateSinCos(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerDifferentiateSinCos");
  /////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtom())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString() << " - possible user typo?";
  const Expression& theArgument=input[2];
  //////////////////////
  if (theArgument.IsAtomGivenData(theCommands.opSin()))
    return output.MakeAtom(theCommands.opCos(), theCommands);
  if (theArgument.IsAtomGivenData(theCommands.opCos()))
  { Expression mOneE, sinE;
    mOneE.AssignValue<Rational>(-1, theCommands);
    sinE.MakeAtom(theCommands.opSin(), theCommands);
    return output.MakeXOX(theCommands, theCommands.opTimes(), mOneE, sinE);
  }
  return false;
}

bool CommandListFunctions::innerCompositeDifferentiateLog(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerCompositeDifferentiateLog");
  /////////////////////
//  std::cout << "<hr>input composite: " << input.ToString();
  if (input.children.size!=2)
    return false;
  if (!input[0].IsListNElementsStartingWithAtom(theCommands.opDifferentiate(), 3))
    return false;
  if (!input[0][2].IsAtomGivenData(theCommands.opLog()))
    return false;
  theCommands.CheckInputNotSameAsOutput(input, output);
  Expression OneE;
  OneE.AssignValue(1, theCommands);
  output.reset(theCommands, 2);
  return output.MakeXOX(theCommands, theCommands.opDivide(), OneE, input[1]);
}

bool CommandListFunctions::outerAssociateAdivBdivCpowerD(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::outerAssociateAdivBdivCpowerD");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  if (!input[2].IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
    return false;
  if (!input[2][1].IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  theCommands.CheckInputNotSameAsOutput(input, output);
  Expression numeratorE, numeratorLeftE, denominatorE;
  output.reset(theCommands, 3);
  numeratorLeftE.MakeXOX(theCommands, theCommands.opThePower(), input[2][1][2], input[2][2]);
  numeratorE.MakeXOX(theCommands, theCommands.opTimes(), input[1], numeratorLeftE);
  denominatorE.MakeXOX(theCommands, theCommands.opThePower(), input[2][1][1], input[2][2]);
  return output.MakeXOX(theCommands, theCommands.opDivide(), numeratorE, denominatorE);
}

bool CommandListFunctions::outerAssociateDivisionDivision(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::outerAssociateDivisionDivision");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  if (input[1].IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
  { Expression newRightE;
    newRightE.MakeXOX(theCommands, theCommands.opTimes(), input[2], input[1][2]);
    return output.MakeXOX(theCommands, theCommands.opDivide(), input[1][1], newRightE);
  }
  if (input[2].IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
  { Expression newLeftE;
    newLeftE.MakeXOX(theCommands, theCommands.opTimes(), input[1], input[2][2]);
    return output.MakeXOX(theCommands, theCommands.opDivide(), newLeftE, input[2][1]);
  }
  return false;
}

bool CommandListFunctions::innerDifferentiateChainRule(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerDifferentiateChainRule");
  /////////////////////
//  std::cout << "here be i!";
  if (input.children.size!=3)
    return false;
//  std::cout << "here be i number 2!";
  if (!input[1].IsAtom())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1], theArgument=input[2];
  //////////////////////
//  std::cout << "ere be chain rule! Argument be: " << theArgument.ToString() << " Argument[0] be: " << theArgument[0].ToString();
  if (!theArgument.IsListNElementsStartingWithAtom(-1, 2))
    return false;
//  std::cout << " continues to rule! The argument be: " << theArgument.ToString() << " theArgument.IsGoodForChainRuleFunction()= "
//  << theArgument.IsGoodForChainRuleFunction() << " theArgument[0].IsGoodForChainRuleFunction()="
//  << theArgument[0].IsGoodForChainRuleFunction() << "; ";
  if (!theArgument.IsGoodForChainRuleFunction() && !theArgument[0].IsGoodForChainRuleFunction())
    return false;
//  std::cout << " here be ruler #1!";
  theCommands.CheckInputNotSameAsOutput(input, output);
  output.reset(theCommands);
  Expression multiplicandleft, multiplicandleftFunction, multiplicandright;
  multiplicandleftFunction.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[0]);
  multiplicandleft.reset(theCommands);
  multiplicandleft.AddChildOnTop(multiplicandleftFunction);
  multiplicandleft.AddChildOnTop(theArgument[1]);
  multiplicandright.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[1]);
  return output.MakeXOX(theCommands, theCommands.opTimes(), multiplicandleft, multiplicandright);
}

bool CommandListFunctions::innerDifferentiateAplusB(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerDifferentiateAplusB");
  /////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtom())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1], theArgument=input[2];
  //////////////////////
  if (!theArgument.IsListNElementsStartingWithAtom(theCommands.opPlus(), 3))
    return false;
  theCommands.CheckInputNotSameAsOutput(input, output);
  output.reset(theCommands);
  Expression leftSummand, rightSummand;
  leftSummand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[1]);
  rightSummand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[2]);
  return output.MakeXOX(theCommands, theCommands.opPlus(), leftSummand, rightSummand);
}

bool CommandListFunctions::innerDifferentiateAtimesB(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerDifferentiateAtimesB");
    //////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtom())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1], theArgument=input[2];
  //////////////////////
  if (!theArgument.IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
  theCommands.CheckInputNotSameAsOutput(input, output);
  output.reset(theCommands);
  Expression changedMultiplicand, leftSummand, rightSummand;
  changedMultiplicand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[1]);
  leftSummand.MakeXOX(theCommands, theCommands.opTimes(), changedMultiplicand, theArgument[2]);
  changedMultiplicand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[2]);
  rightSummand.MakeXOX(theCommands, theCommands.opTimes(), theArgument[1], changedMultiplicand );
  return output.MakeXOX(theCommands, theCommands.opPlus(), leftSummand, rightSummand);
}

bool CommandListFunctions::innerPowerAnyToZero(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerPowerAnyToZero");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
    return false;
  if (!input[2].IsEqualToZero())
    return false;
//  std::cout << "input[1]: "<< input[1].ToString() << ", input[2]: " << input[2].ToString();
  if (input[1].IsEqualToZero())
    return output.SetError("Error: expression of the form 0^0 is illegal.", theCommands);
  return output.AssignValue<Rational>(1, theCommands);
}

bool CommandListFunctions::innerDifferentiateAdivideBCommutative(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerDifferentiateAdivideBCommutative");
    //////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtom())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression " << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1], theArgument=input[2];
  //////////////////////
  //Quotient rule (commutative): (a/b^n)':=(a'b-n a b')/b^{n+1}
  if (!theArgument.IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  theCommands.CheckInputNotSameAsOutput(input, output);
  output.reset(theCommands);
  Expression theDenominatorBase, eOne, theDenominatorExponentPlusOne, theDenominatorExponent, changedMultiplicand,
  leftSummand, rightSummand, theDenominatorFinal, numerator;
  eOne.AssignValue(1, theCommands);
  bool denBaseFound=false;
  if (theArgument[2].IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
    if (theArgument[2][2].IsConstantNumber())
    { denBaseFound=true;
      theDenominatorBase=theArgument[2][1];
      theDenominatorExponent=theArgument[2][2];
      theDenominatorExponentPlusOne.MakeXOX(theCommands, theCommands.opPlus(), theDenominatorExponent, eOne);
    }
  if (!denBaseFound)
  { theDenominatorBase=theArgument[2];
    theDenominatorExponentPlusOne.AssignValue(2, theCommands);
    theDenominatorExponent.AssignValue(1, theCommands);
  }
  theDenominatorFinal.MakeXOX(theCommands, theCommands.opThePower(), theDenominatorBase, theDenominatorExponentPlusOne);
  changedMultiplicand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[1]);
  leftSummand.MakeXOX(theCommands, theCommands.opTimes(), changedMultiplicand, theDenominatorBase);
  rightSummand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theDenominatorBase);
  changedMultiplicand.MakeXOX(theCommands, theCommands.opTimes(), theArgument[1], rightSummand);
  rightSummand.MakeXOX(theCommands, theCommands.opTimes(), theDenominatorExponent, changedMultiplicand);
  numerator.MakeXOX(theCommands, theCommands.opMinus(), leftSummand, rightSummand);
  return output.MakeXOX(theCommands, theCommands.opDivide(), numerator, theDenominatorFinal);
}

bool CommandListFunctions::innerDifferentiateAdivideBNONCommutative(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerDifferentiateAdivideBNONCommutative");
    //////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtom())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression " << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1], theArgument=input[2];
  //////////////////////
  //Quotient rule (non-commutative): (a/b)'=(ab^{-1})'=a' b - a b^{-1} b' b^{-1}
  if (!theArgument.IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  theCommands.CheckInputNotSameAsOutput(input, output);
  output.reset(theCommands);
  Expression changedMultiplicand, leftSummand, rightSummand;
  Expression bInverse, bPrime, eMOne;
  eMOne.AssignValue<Rational>(-1, theCommands);
  changedMultiplicand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[1]);
  leftSummand.MakeXOX(theCommands, theCommands.opDivide(), changedMultiplicand, theArgument[2]);
  bPrime.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[2]);
  bInverse.MakeXOX(theCommands, theCommands.opThePower(), theArgument[2], eMOne);
  rightSummand.MakeXOX(theCommands, theCommands.opTimes(), bPrime, bInverse); //rightSummand= b' b^{-1}
  changedMultiplicand.MakeXOX(theCommands, theCommands.opTimes(), bInverse, rightSummand); //changedMultiplicand= b^-1 b' b^-1
  rightSummand.MakeXOX(theCommands, theCommands.opTimes(), theArgument[1], changedMultiplicand);
  return output.MakeXOX(theCommands, theCommands.opMinus(), leftSummand, rightSummand);
}


bool CommandListFunctions::outerCommuteAtimesBifUnivariate(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::outerCommuteAtimesBifUnivariate");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
  if (input[1].IsConstantNumber())
    return false;
  HashedListSpecialized<Expression> theList;
  input.GetBlocksOfCommutativity(theList);
  if (theList.size!=1)
    return false;
  if (input[2]>input[1] || input[2]==input[1])
    return false;
//  std::cout << "<hr>" << input[2].ToString() << " less than " << input[1].ToString();
//  std::cout << "ere be i, number 1!";
  output=input;
  output.children.SwapTwoIndices(1,2);
  return true;
}

bool CommandListFunctions::outerCommuteAtimesBtimesCifUnivariate(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::outerCommuteAtimesBtimesCifUnivariate");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes(),3))
    return false;
  const Expression& leftE=input[1];
  if (leftE.IsConstantNumber())
    return false;
  if (!input[2].IsListNElementsStartingWithAtom(theCommands.opTimes(),3))
    return false;
  const Expression& rightE=input[2][1];
  HashedListSpecialized<Expression> theList;

  leftE.GetBlocksOfCommutativity(theList);
  rightE.GetBlocksOfCommutativity(theList);
  if (theList.size!=1)
    return false;
  if (rightE>leftE || leftE==rightE)
    return false;
//  std::cout << "<hr>" << rightE.ToString() << " less than " << leftE.ToString();
  Expression leftMultiplicand;
  leftMultiplicand.MakeXOX(theCommands, theCommands.opTimes(), rightE, leftE);
//  std::cout << "Left multiplicand: " << leftE.ToString() << ", right: " << rightE.ToString() << " ";
  return output.MakeXOX(theCommands, theCommands.opTimes(), leftMultiplicand, input[2][2]);
}

bool CommandListFunctions::outerDifferentiateWRTxTimesAny(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::outerDifferentiateWRTxTimesAny");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
  if (!input[1].IsListNElementsStartingWithAtom(theCommands.opDifferentiate(), 2))
    return false;
  if (input[2].IsBuiltInAtom())
    return false;
  theCommands.CheckInputNotSameAsOutput(input, output);
  output=input[1];
  return output.AddChildOnTop(input[2]);
}

bool CommandListFunctions::innerDdivDxToDifferentiation(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerDdivDxToDifferentiation");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  std::string denominatorString,numeratorString;
  if (!input[1].IsAtom(&numeratorString))
    return false;
  if (!input[2].IsAtom(&denominatorString))
    return false;
  if (numeratorString!="d")
    return false;
  if (denominatorString.size()<2)
    return false;
  if (denominatorString[0]!='d')
    return false;
  for (int i=0; i<((signed) denominatorString.size())-1; i++)
    denominatorString[i]=denominatorString[i+1];
  denominatorString.resize(denominatorString.size()-1);
  output.reset(theCommands, 2);
  output.AddChildAtomOnTop(theCommands.opDifferentiate());
  return output.AddChildAtomOnTop(theCommands.AddOperationNoRepetitionOrReturnIndexFirst(denominatorString));
}

bool CommandList::innerCrash(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerCrash");
  crash << "<hr>This is a test of the crashing mechanism of the calculator. Are log files created correctly? Check the /output/ directory." << crash;
  return output.AssignValue((std::string)"Crashed succesfully", theCommands);
}

bool CommandList::CheckConsistencyAfterInitializationExpressionStackEmpty()
{ this->theExpressionContainer.GrandMasterConsistencyCheck();
  this->ExpressionStack.GrandMasterConsistencyCheck();
  this->cachedExpressions.GrandMasterConsistencyCheck();
  if (this->cachedExpressions.size!=0 || this->imagesCachedExpressions.size!=0 || this->ExpressionStack.size!=0 || this->theExpressionContainer.size!=0)
    crash << "This is a programming error: cached expressions, images cached expressions, expression stack and expression container are supposed to be empty, but "
    << " instead they contain respectively " << this->cachedExpressions.size << ", " << this->imagesCachedExpressions.size << ", "
    << this->ExpressionStack.size << " and " << this->theExpressionContainer.size << " elements. " << crash;
  return true;
}

bool CommandListFunctions::outerAdivBpowerItimesBpowerJ(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::outerAdivBpowerItimesBpowerJ");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
  if (!input[1].IsListNElementsStartingWithAtom(theCommands.opDivide(),3))
    return false;
  Expression denominatorBase, denominatorExponent;
  Expression numeratorBase, numeratorExponent;
  input[1][2].GetBaseExponentForm(denominatorBase, denominatorExponent);
  input[2].GetBaseExponentForm(numeratorBase, numeratorExponent);
  if (denominatorBase!=numeratorBase)
    return false;
  theCommands.CheckInputNotSameAsOutput(input, output);
  Expression rightMultiplicand, rightMultiplicandExponent;
  rightMultiplicandExponent.MakeXOX(theCommands, theCommands.opMinus(), numeratorExponent, denominatorExponent);
  rightMultiplicand.MakeXOX(theCommands, theCommands.opThePower(), denominatorBase, rightMultiplicandExponent);
  return output.MakeXOX(theCommands, theCommands.opTimes(), input[1][1], rightMultiplicand);
}

bool Expression::SplitProduct(int numDesiredMultiplicandsLeft, Expression& outputLeftMultiplicand, Expression& outputRightMultiplicand)const
{ MacroRegisterFunctionWithName("Expression::SplitProduct");
  if (numDesiredMultiplicandsLeft<=0)
    return false;
  this->CheckInitialization();
  List<Expression> theMultiplicandsLeft, theMultiplicandsRight;
  this->theBoss->AppendOpandsReturnTrueIfOrderNonCanonical(*this, theMultiplicandsLeft, this->theBoss->opTimes());
  if (theMultiplicandsLeft.size<=numDesiredMultiplicandsLeft)
    return false;
  theMultiplicandsRight.SetExpectedSize(theMultiplicandsLeft.size-numDesiredMultiplicandsLeft);
  for (int i=numDesiredMultiplicandsLeft; i<theMultiplicandsLeft.size; i++)
    theMultiplicandsRight.AddOnTop(theMultiplicandsLeft[i]);
  theMultiplicandsLeft.SetSize(numDesiredMultiplicandsLeft);
  outputLeftMultiplicand.MakeXOdotsOX(*this->theBoss, this->theBoss->opTimes(), theMultiplicandsLeft);
  return outputRightMultiplicand.MakeXOdotsOX(*this->theBoss, this->theBoss->opTimes(), theMultiplicandsRight);
}

bool CommandListFunctions::outerAtimesBpowerJplusEtcDivBpowerI(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::outerAtimesBpowerJplusEtcDivBpowerI");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opDivide(),3))
    return false;
//  std::cout << "ere be i!";
  Expression denominatorBase, denominatorExponent;
  input[2].GetBaseExponentForm(denominatorBase, denominatorExponent);
  if (!denominatorBase.IsAtom())
    return false;
  MonomialCollection<Expression, Rational> numerators, numeratorsNew;
  theCommands.CollectSummands(theCommands, input[1], numerators);
  numeratorsNew.SetExpectedSize(numerators.size());
  numeratorsNew.MakeZero();
  Expression numeratorMultiplicandLeft, numeratorMultiplicandRight, numeratorBaseRight, numeratorExponentRight;
  Expression newNumSummand, newNumSummandRightPart, newNumExponent, mOneE;
  mOneE.AssignValue(-1, theCommands);
  for (int i=0; i<numerators.size(); i++)
  { if (numerators[i].IsConstantNumber())
    { newNumExponent.MakeXOX(theCommands, theCommands.opTimes(), mOneE, denominatorExponent);
      newNumSummand.MakeXOX(theCommands, theCommands.opThePower(), denominatorBase, newNumExponent);
      numeratorsNew.AddMonomial(newNumSummand, numerators.theCoeffs[i]);
      continue;
    }
    numerators[i].GetBaseExponentForm(numeratorBaseRight, numeratorExponentRight);
    if (numeratorBaseRight==denominatorBase)
    { newNumExponent.MakeXOX(theCommands, theCommands.opMinus(), numeratorExponentRight, denominatorExponent);
      newNumSummand.MakeXOX(theCommands, theCommands.opThePower(), denominatorBase, newNumExponent);
      numeratorsNew.AddMonomial(newNumSummand, numerators.theCoeffs[i]);
      continue;
    }
    bool isGood=false;
    for (int j=1; numerators[i].SplitProduct(j, numeratorMultiplicandLeft, numeratorMultiplicandRight); j++)
    { numeratorMultiplicandRight.GetBaseExponentForm(numeratorBaseRight, numeratorExponentRight);
      if (numeratorBaseRight!=denominatorBase)
        continue;
      newNumExponent.MakeXOX(theCommands, theCommands.opMinus(), numeratorExponentRight, denominatorExponent);
      newNumSummandRightPart.MakeXOX(theCommands, theCommands.opThePower(), denominatorBase, newNumExponent);
      newNumSummand.MakeXOX(theCommands, theCommands.opTimes(), numeratorMultiplicandLeft, newNumSummandRightPart);
      numeratorsNew.AddMonomial(newNumSummand, numerators.theCoeffs[i]);
      isGood=true;
      break;
    }
//    if (!isGood)
//      std:: cout << "- oh no: " << numerators[i] << " is no good! ";
    if (!isGood)
      return false;
  }
  return output.MakeSum(theCommands, numeratorsNew);
}

bool CommandListFunctions::innerSort(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerSort");
  if (!input.IsSequenceNElementS())
    return false;
  List<Expression> sortedExpressions;
  sortedExpressions.ReservE(input.children.size-1);
  for (int i=1; i<input.children.size; i++)
    sortedExpressions.AddOnTop(input[i]);
  sortedExpressions.QuickSortAscending();
  output.reset(theCommands, sortedExpressions.size+1);
  output.AddChildAtomOnTop(theCommands.opSequence());
  for (int i=0; i<sortedExpressions.size; i++)
    output.AddChildOnTop(sortedExpressions[i]);
  return true;
}

bool CommandListFunctions::innerGrowDynkinType(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerGrowDynkinType");
  if (input.children.size!=3)
    return false;
  const Expression& theSmallerTypeE=input[1];
  DynkinType theSmallDynkinType;
  if (!Serialization::innerLoadDynkinType(theCommands, theSmallerTypeE, theSmallDynkinType))
    return false;
  SemisimpleLieAlgebra* theSSalg=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(Serialization::innerSSLieAlgebra, input[2], theSSalg))
    return output.SetError("Error extracting ambient Lie algebra.", theCommands);
  SemisimpleSubalgebras tempSas;
  tempSas.owneR=theSSalg;
  tempSas.ownerField=&theCommands.theObjectContainer.theAlgebraicClosure;
  tempSas.theGlobalVariables=theCommands.theGlobalVariableS;
  tempSas.ComputeSl2sInitOrbitsForComputationOnDemand();
  if (!tempSas.RanksAndIndicesFit(theSmallDynkinType))
    return output.SetError("Error: type "+theSmallDynkinType.ToString()+" does not fit inside "+theSSalg->theWeyl.theDynkinType.ToString(), theCommands);
  List<DynkinType> largerTypes;
  List<List<int> > theRootInjections;
  if (!tempSas.GrowDynkinType(theSmallDynkinType, largerTypes, &theRootInjections))
    return output.SetError("Error: growing type "+theSmallDynkinType.ToString()+" inside "+theSSalg->theWeyl.theDynkinType.ToString() + " failed. ", theCommands);
  std::stringstream out;
  out << "Inside " << theSSalg->theWeyl.theDynkinType.ToString() << ", input type " << theSmallDynkinType.ToString();
  if (largerTypes.size==0)
    out << " cannot grow any further. ";
  else
  { CandidateSSSubalgebra tempCandidate;
    out << " can grow to the following types. <br>";
    out << "<table border=\"1\"><td>Larger type</td><td>Root injection</td><td>Highest weight module containing new simple generator</td></tr>";
    for(int i=0; i<largerTypes.size; i++)
    { out << "<tr><td>" << largerTypes[i].ToString() << "</td>";
      out << "<td>";
      for (int j=0; j<theRootInjections[i].size; j++)
      { out << "r_" << j+1 << " -> " << "r_" << theRootInjections[i][j]+1;
        if (j!=theRootInjections[i].size)
          out << ", ";
      }
      out << "</td><td>";
      out << CGI::GetHtmlMathSpanPure
      (tempSas.GetHighestWeightFundNewComponentFromRootInjection(largerTypes[i], theRootInjections[i], tempCandidate).ToStringLetterFormat("\\omega"));
      out << "</td></tr>";
    }
    out << "</table>";
  }
  return output.AssignValue(out.str(), theCommands);
}

void Expression::GetBlocksOfCommutativity(HashedListSpecialized<Expression>& inputOutputList)const
{ MacroRegisterFunctionWithName("Expression::GetBlocksOfCommutativity");
  this->CheckInitialization();
  if (this->IsAtom())
  { inputOutputList.AddOnTopNoRepetition(*this);
    return;
  }
  if (this->IsConstantNumber())
    return;
  std::string whichOperation;
  if ((*this)[0].IsAtom(&whichOperation))
    if (this->theBoss->atomsThatAllowCommutingOfCompositesStartingWithThem.Contains(whichOperation))
    { for (int i=1; i<this->children.size; i++)
        (*this)[i].GetBlocksOfCommutativity(inputOutputList);
      return;
    }
//  std::cout << "<hr>which operation=" << whichOperation;
  inputOutputList.AddOnTopNoRepetition(*this);
}

bool Expression::MakeSequence(CommandList& owner, List<Expression>& inputSequence)
{ this->reset(owner, inputSequence.size+1);
  this->AddChildAtomOnTop(owner.opSequence());
  for (int i=0; i<inputSequence.size; i++)
    this->AddChildOnTop(inputSequence[i]);
  return true;
}

bool CommandListFunctions::innerGetUserDefinedSubExpressions(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerGetUserDefinedSubExpressions");
  HashedListSpecialized<Expression> theList;
  input.GetBlocksOfCommutativity(theList);
  return output.MakeSequence(theCommands, theList);
}

bool CommandListFunctions::innerComputeSemisimpleSubalgebras(CommandList& theCommands, const Expression& input, Expression& output)
{ //bool showIndicator=true;
  MacroRegisterFunctionWithName("CommandList::innerComputeSemisimpleSubalgebras");
  SemisimpleLieAlgebra* ownerSSPointer;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(Serialization::innerSSLieAlgebra, input, ownerSSPointer))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  SemisimpleLieAlgebra& ownerSS=*ownerSSPointer;
  std::stringstream out;
  if (ownerSS.GetRank()>6)
  { out << "<b>This code is completely experimental and has been set to run up to rank 6. As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
    return output.AssignValue(out.str(), theCommands);
  } else
    out << "<b>This code is completely experimental. Use the following printouts on your own risk</b>";
  SemisimpleSubalgebras tempSSsas
  (ownerSS, &theCommands.theObjectContainer.theAlgebraicClosure, &theCommands.theObjectContainer.theLieAlgebras, &theCommands.theObjectContainer.theSltwoSAs, theCommands.theGlobalVariableS);
  SemisimpleSubalgebras& theSSsubalgebras=theCommands.theObjectContainer.theSSsubalgebras[theCommands.theObjectContainer.theSSsubalgebras.AddNoRepetitionOrReturnIndexFirst(tempSSsas)];
  theSSsubalgebras.flagComputePairingTable=false;
  theSSsubalgebras.flagComputeNilradicals=false;
  theSSsubalgebras.FindTheSSSubalgebras(ownerSS);
  return output.AssignValue(theSSsubalgebras, theCommands);
}

bool CommandListFunctions::innerPlotWedge(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerPlotWedge");
  if (input.children.size!=6)
  { theCommands.Comments << "<hr>innerPlotWedge takes as input 4 arguments: x,y coordinates of center, starting angle, final angle. ";
    return false;
  }
  Rational xCoord, yCoord, radius, startAngle, endAngle;
  if (!input[1].IsOfType<Rational>(&xCoord) || !input[2].IsOfType<Rational>(&yCoord) || !input[3].IsOfType<Rational>(&radius) ||
      !input[4].IsOfType<Rational>(&startAngle) || !input[5].IsOfType<Rational>(&endAngle))
  { theCommands.Comments << "<hr>Failed to extract x, y coordinate, radius, start angle, end angle from " << input.ToString() << ". ";
    return false;
  }
  std::stringstream out;
  CalculusFunctionPlot thePlot;
  double x1wedge= MathRoutines::ReducePrecision(xCoord.DoubleValue()+ radius.DoubleValue() *FloatingPoint::cos(startAngle.DoubleValue()));
  double y1wedge= MathRoutines::ReducePrecision(yCoord.DoubleValue()+ radius.DoubleValue() *FloatingPoint::sin(startAngle.DoubleValue()));
  double x2wedge= MathRoutines::ReducePrecision(xCoord.DoubleValue()+ radius.DoubleValue() *FloatingPoint::cos(endAngle.DoubleValue()));
  double y2wedge= MathRoutines::ReducePrecision(yCoord.DoubleValue()+ radius.DoubleValue() *FloatingPoint::sin(endAngle.DoubleValue()));
  double xCoordDouble=MathRoutines::ReducePrecision(xCoord.DoubleValue());
  double yCoordDouble=MathRoutines::ReducePrecision(yCoord.DoubleValue());
  double startAngleDouble=MathRoutines::ReducePrecision(startAngle.DoubleValue());
  double radiusDouble=MathRoutines::ReducePrecision(radius.DoubleValue());
  double endAngleDouble=MathRoutines::ReducePrecision(endAngle.DoubleValue());
  out << "\\pscustom*[linecolor=cyan]{ \\psparametricplot[algebraic,linecolor=\\psColorGraph]{" << startAngleDouble << "}{" << endAngleDouble
  << "}{" << xCoordDouble << "+" << radiusDouble << "*cos(t)| " << yCoordDouble << "+" << radiusDouble << "*sin(t)} \\psline("
  << x2wedge << ", " << y2wedge << ")(" << xCoordDouble << ", " << yCoordDouble << ")" << "(" << x1wedge << ", " << y1wedge << ")}";
  out << "\\pscustom[linecolor=blue]{ \\psparametricplot[algebraic,linecolor=\\psColorGraph]{" << startAngleDouble << "}{" << endAngleDouble
  << "}{" << xCoordDouble << "+" << radiusDouble << "*cos(t)| " << yCoordDouble << "+" << radiusDouble << "*sin(t)} \\psline("
  << x2wedge << ", " << y2wedge << ")(" << xCoordDouble << ", " << yCoordDouble << ")" << "(" << x1wedge << ", " << y1wedge << ")}";
  thePlot.lowerBounds.AddOnTop(-5);
  thePlot.upperBounds.AddOnTop(5);
  thePlot.thePlotElementS.AddOnTop(input);
  thePlot.thePlotStrings.AddOnTop(out.str());
  thePlot.thePlotStringsWithHtml.AddOnTop(out.str());
  return output.AssignValue(thePlot, theCommands);
}

bool CommandListFunctions::innerPlotConeUsualProjection(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerPlotConeUsualProjection");
  if (input.children.size!=5)
  { theCommands.Comments << "<hr>PlotConeUsualProjection takes 4 arguments. ";
    return false;
  }
  double radius, height, distance, viewPointHeight;
  if (!input[1].IsRealDouble(&radius) || !input[2].IsRealDouble(&height) || !input[3].IsRealDouble(&distance) || !input[4].IsRealDouble(&viewPointHeight) )
  { theCommands.Comments << "<hr> failed to extract radius, height, distance, viewpoint height from " << input.ToString();
    return false;
  }
  affineHyperplane<double> projectionPane;
  projectionPane.normal.MakeZero(3);
  projectionPane.normal[1]=-distance;
  projectionPane.normal[2]=-viewPointHeight;
  projectionPane.affinePoint.MakeZero(3);
  projectionPane.affinePoint[1]=distance;
  projectionPane.affinePoint[2]=viewPointHeight;
  Vector<double> pointCircle1, pointCircle2;
  Vector<double> axis1, axis2, axis3;

  pointCircle1.MakeZero(3);
  pointCircle2.MakeZero(3);
  pointCircle1[1]=radius;
  pointCircle2[1]=-radius;
  Vector<double> projection1= projectionPane.ProjectOnMe(pointCircle1);
  Vector<double> projection2= projectionPane.ProjectOnMe(pointCircle2);
  //std::cout << "<br>projection1: " << projection1.ToString();
  //std::cout << "<br>projection2: " << projection2.ToString();
  Vector<double> XdiameterVectorProjection=projection1-projection2;
  double theProjYradius= FloatingPoint::sqrt(XdiameterVectorProjection.ScalarEuclidean(XdiameterVectorProjection))/2;
  double theProjXradius=radius;
  //std::cout << "<br>theProjYradius: " << theProjYradius;
  //std::cout << "<br>theProjXradius: " << theProjXradius;
  CalculusFunctionPlot thePlot;
  std::stringstream out;
  Vector<double> tipOfTheCone, centerOfTheCone;
  centerOfTheCone.MakeZero(3);
  tipOfTheCone.MakeZero(3);
  tipOfTheCone[2]=height;
//  std::cout << "<br>radius, height, distance, view height: " << radius << ", " << height << ", " << distance << ", " << viewPointHeight;
//  std::cout << "<br>projectionPane.ProjectOnMe(tipOfTheCone): " << projectionPane.ProjectOnMe(tipOfTheCone);
  Vector<double> coneHeightVectorProjectedShifted= projectionPane.ProjectOnMe(tipOfTheCone)-projectionPane.ProjectOnMe(centerOfTheCone);
//  std::cout << "<br>coneHeightVectorProjectedShifted: " << coneHeightVectorProjectedShifted.ToString();
  double theConeProjectionHeight=FloatingPoint::sqrt(coneHeightVectorProjectedShifted.ScalarEuclidean(coneHeightVectorProjectedShifted));
//  std::cout << "<br>theConeProjectionHeight: " << theConeProjectionHeight;
  out << "\\psline[linecolor=black](0,0)(0," << MathRoutines::ReducePrecision(theConeProjectionHeight) << ")";
  if (theConeProjectionHeight>theProjYradius)
  { double yCoordPointTangency= theProjYradius*theProjYradius/theConeProjectionHeight;
    double xCoordPointTangency=theProjXradius*FloatingPoint::sqrt(1- yCoordPointTangency/theConeProjectionHeight);
    out << "\\psline[linecolor=\\psColorGraph](" << MathRoutines::ReducePrecision(xCoordPointTangency) << ", "
    << MathRoutines::ReducePrecision(yCoordPointTangency) << ")(0, " << MathRoutines::ReducePrecision(theConeProjectionHeight) << ")";
    out << "\\psline[linecolor=\\psColorGraph](" << MathRoutines::ReducePrecision(-xCoordPointTangency) << ", "
    << MathRoutines::ReducePrecision(yCoordPointTangency) << ")(0, " << MathRoutines::ReducePrecision(theConeProjectionHeight) << ")";
    double theAngleVisibleEnd=MathRoutines::Pi()/2;
    double theAngleVisibleStart=-theAngleVisibleEnd;
    double theAngleHiddenEnd=MathRoutines::Pi()/2;
    if (theProjXradius!=0)
    { theAngleVisibleEnd=FloatingPoint::arctan(yCoordPointTangency*theProjXradius/(xCoordPointTangency*theProjYradius));
      theAngleVisibleStart = -MathRoutines::Pi()-theAngleVisibleEnd;
      theAngleHiddenEnd=MathRoutines::Pi()-theAngleVisibleEnd;
    }
    out << "\\psparametricplot[algebraic,linecolor=\\psColorGraph]{" << MathRoutines::ReducePrecision(theAngleVisibleStart)
    << "}{" << MathRoutines::ReducePrecision(theAngleVisibleEnd) << "}{" << MathRoutines::ReducePrecision(theProjXradius)
    << "*cos(t) |" << MathRoutines::ReducePrecision(theProjYradius) << "*sin(t)}";
    out << "\\psparametricplot[algebraic, linestyle=dashed, linecolor=\\psColorGraph]{" << MathRoutines::ReducePrecision(theAngleVisibleEnd)
    << "}{" << MathRoutines::ReducePrecision(theAngleHiddenEnd) << "}{" << MathRoutines::ReducePrecision(theProjXradius)
    << "*cos(t) |" << MathRoutines::ReducePrecision(theProjYradius) << "*sin(t)}";
  } else
  { theCommands.Comments << "<hr>Cone is not high enough and therefore has no tip. ";
    out << "\\psparametricplot[algebraic,linecolor=\\psColorGraph]{0}{6.283185307}{cos(t)*" << MathRoutines::ReducePrecision(theProjXradius)
    << " |sin(t)*" << MathRoutines::ReducePrecision(theProjYradius) << "}";
  }
  thePlot.lowerBounds.AddOnTop(-5);
  thePlot.upperBounds.AddOnTop(5);
  thePlot.thePlotElementS.AddOnTop(input);
  thePlot.thePlotStrings.AddOnTop(out.str());
  thePlot.thePlotStringsWithHtml.AddOnTop(out.str());
  return output.AssignValue(thePlot, theCommands);
}

bool CommandListFunctions::innerComputePairingTablesAndFKFTsubalgebras(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerComputePairingTablesAndFKFTsubalgebras");
  if (!input.IsOfType<SemisimpleSubalgebras>())
  { theCommands.Comments << "<hr>Input of ComputeFKFT must be of type semisimple subalgebras. ";
    return false;
  }
  SemisimpleSubalgebras& theSAs=input.GetValueNonConst<SemisimpleSubalgebras>();
  theSAs.flagComputePairingTable=true;
  theSAs.flagComputeNilradicals=true;
  theSAs.ComputePairingTablesAndFKFTtypes();
  output=input;
  std::fstream theFile;
  std::string theFileName;
  theFileName=theCommands.PhysicalPathOutputFolder + "FKFTcomputation.html";
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML=true;
  tempFormat.flagUseLatex=true;
  tempFormat.flagUseHTML=true;
  tempFormat.flagCandidateSubalgebraShortReportOnly=false;
  XML::OpenFileCreateIfNotPresent(theFile, theFileName, false, true, false);
  theFile << theSAs.ToString(&tempFormat);
  std::stringstream out;
  out << "<a href=\"" << theCommands.DisplayPathOutputFolder << "FKFTcomputation.html\">FKFTcomputation.html</a>";
  return output.AssignValue(out.str(), theCommands);
}

bool CommandListFunctions::innerGetCentralizerChainsSemisimpleSubalgebras(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerGetCentralizerChainsSemisimpleSubalgebras");
  if (!input.IsOfType<SemisimpleSubalgebras>())
  { theCommands.Comments << "<hr>Input of GetCentralizerChains must be of type semisimple subalgebras. ";
    return false;
  }
  SemisimpleSubalgebras& theSAs=input.GetValueNonConst<SemisimpleSubalgebras>();
  List<List<int> > theChains;
  std::stringstream out;
  theSAs.GetCentralizerChains(theChains);
  Expression currentChainE;
  out << theChains.size << " chains total. <br>";
  for (int i=0; i<theChains.size; i++)
  { out << "<br>Chain " << i+1 << ": Load{}(Serialization{}(LoadSemisimpleSubalgebras, " << theSAs.owneR->theWeyl.theDynkinType.ToString() << ", (";
    for (int j=0; j<theChains[i].size; j++)
    { Serialization::innerStoreCandidateSA(theCommands, theSAs.theSubalgebraCandidates[theChains[i][j]], currentChainE);
      out << currentChainE.ToString();
      if (j!=theChains[i].size-1)
        out << ", ";
    }
    out << ") ) )";
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CommandList::innerEvaluateToDouble(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Expression::innerEvaluateToDouble");
//  std::cout << "<br>evaluatetodouble: " << input.ToString();
  if (input.IsOfType<double>())
  { output=input;
    return true;
  }
  if (input.IsOfType<Rational>())
    return output.AssignValue(input.GetValue<Rational>().DoubleValue(), theCommands);
  RecursionDepthCounter theCounter(&theCommands.RecursionDeptH);
  if (theCommands.RecursionDeptH >theCommands.MaxRecursionDeptH)
  { theCommands.Comments << "<hr>Recursion depth exceeded while evaluating innerEvaluateToDouble. This may be a programming error. ";
    return false;
  }
  if (input.IsAtomGivenData(theCommands.opE()))
    return output.AssignValue(MathRoutines::E(), theCommands);
  if (input.IsAtomGivenData(theCommands.opPi()))
    return output.AssignValue(MathRoutines::Pi(), theCommands);

  bool isArithmeticOperationTwoArguments=
  input.IsListNElementsStartingWithAtom(theCommands.opTimes(),3) ||
  input.IsListNElementsStartingWithAtom(theCommands.opPlus(),3) ||
  input.IsListNElementsStartingWithAtom(theCommands.opThePower(),3) ||
  input.IsListNElementsStartingWithAtom(theCommands.opDivide(),3) ||
  input.IsListNElementsStartingWithAtom(theCommands.opSqrt(),3)
  ;
  if (input.IsListNElementsStartingWithAtom(theCommands.opSqrt()))
    std::cout << "Starting with sqrt: " << input.ToStringFull();
  if (isArithmeticOperationTwoArguments)
  { double leftD, rightD;
    if (!input[1].IsRealDouble(&leftD) || !input[2].IsRealDouble(&rightD))
      return false;
    if (input.IsListNElementsStartingWithAtom(theCommands.opTimes(),3))
      return output.AssignValue(leftD*rightD, theCommands);
    if (input.IsListNElementsStartingWithAtom(theCommands.opPlus(),3))
      return output.AssignValue(leftD+rightD, theCommands);
    if (input.IsListNElementsStartingWithAtom(theCommands.opThePower(),3))
      return output.AssignValue(FloatingPoint::power(leftD, rightD) , theCommands);
    if (input.IsListNElementsStartingWithAtom(theCommands.opSqrt(),3))
      return output.AssignValue(FloatingPoint::power(rightD,1/leftD), theCommands);
    if (input.IsListNElementsStartingWithAtom(theCommands.opDivide(),3))
      return output.AssignValue(leftD/rightD, theCommands);
    crash << "This is a piece of code which should never be reached. " << crash;
  }
  if(input.IsListNElementsStartingWithAtom(theCommands.opSqrt(),2))
  { double argumentD;
    if (!input[1].IsRealDouble(&argumentD))
      return false;
    if (argumentD<0)
      return false;
    return output.AssignValue(FloatingPoint::sqrt(argumentD), theCommands);
  }
  return false;
}
