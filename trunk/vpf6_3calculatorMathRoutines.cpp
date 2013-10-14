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
    return false;
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

bool CommandListFunctions::innerSin(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerSin");
  double theArgument;
  if (!input.IsDouble(&theArgument))
  { if (input.IsAtoM(theCommands.opPi()))
      return output.AssignValue(0, theCommands);
    if (input.IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
      if (input[1].IsDouble(&theArgument) && input[2].IsAtoM(theCommands.opPi()))
        return output.AssignValue(sin (theArgument*MathRoutines::Pi()), theCommands);
    return false;
  }
  return output.AssignValue(sin(theArgument), theCommands );
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
  double theArgument;
  if (!input.IsDouble(&theArgument))
  { if (input.IsAtoM(theCommands.opPi()))
      return output.AssignValue(-1, theCommands);
    if (input.IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
      if (input[1].IsDouble(&theArgument) && input[2].IsAtoM(theCommands.opPi()))
        return output.AssignValue(cos (theArgument*MathRoutines::Pi()), theCommands);
    return false;
  }
  return output.AssignValue(cos(theArgument), theCommands );
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

bool CommandListFunctions::innerCompositeConstTimesAnyActOn(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerCompositeConstTimesAnyActOn");
  if (!input.IsListNElements())
    return false;
  if (!input[0].IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
  if (!input[0][1].IsConstant())
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
  CGI::OpenFileCreateIfNotPresent(theFileStream, theCommands.PhysicalPathOutputFolder+"makefile", false, true, false);
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
  if (!input[1].IsAtoM())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1];
  const Expression& theArgument=input[2];
  //////////////////////
  if (!theArgument.IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
    return false;
  if (!theArgument[2].IsConstant())
    return false;
  Expression theMonomial, theTerm, theExponent, basePrime, minusOne;
  minusOne.AssignValue<Rational>(-1, theCommands);
  theExponent.MakeXOX(theCommands, theCommands.opPlus(), theArgument[2], minusOne);
  theMonomial.MakeXOX(theCommands, theCommands.opThePower(), theArgument[1], theExponent);
  basePrime.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[1]);
  theTerm.MakeXOX(theCommands, theCommands.opTimes(), theArgument[2], theMonomial);
  return output.MakeXOX(theCommands, theCommands.opTimes(), theTerm, basePrime);
}

bool CommandListFunctions::innerDifferentiateConstant(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerDifferentiateConstant");
  //////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtoM())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
    << " - possible user typo?";
  const Expression& theArgument=input[2];
  //////////////////////
  if (!theArgument.IsConstant())
    return false;
  return output.AssignValue<Rational>(0, theCommands);
}

bool CommandListFunctions::innerDifferentiateX(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerDifferentiateX");
  //////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtoM())
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
  if (!input[1].IsAtoM())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
    << " - possible user typo?";
  const Expression& theArgument=input[2];
  //////////////////////
  if (theArgument.IsAtoM(theCommands.opSin()))
    return output.MakeAtom(theCommands.opCos(), theCommands);
  if (theArgument.IsAtoM(theCommands.opCos()))
  { Expression mOneE, sinE;
    mOneE.AssignValue<Rational>(-1, theCommands);
    sinE.MakeAtom(theCommands.opSin(), theCommands);
    return output.MakeXOX(theCommands, theCommands.opTimes(), mOneE, sinE);
  }
  return false;
}

bool CommandListFunctions::innerDifferentiateChainRule(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerDifferentiateChainRule");
  /////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtoM())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1], theArgument=input[2];
  //////////////////////
//  std::cout << "ere be chain rule! Argument be: " << theArgument.ToString() << " Argument[0] be: " << theArgument[0].ToString();
  if (!theArgument.IsListNElementsStartingWithAtom(-1, 2))
    return false;
//  std::cout << " continues to rule!";
  if (theArgument.IsBuiltInType() || theArgument[0].IsBuiltInOperation())
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
  if (!input[1].IsAtoM())
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
  if (!input[1].IsAtoM())
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

bool CommandListFunctions::innerDifferentiateAdivideB(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerDifferentiateAdivideB");
    //////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtoM())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1], theArgument=input[2];
  //////////////////////
  //Quotient rule (non-commutative): (a/b)'=(ab^{-1})'=a' b - a b^{-1} b' b^{-1}
  if (!theArgument.IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  theCommands.CheckInputNotSameAsOutput(input, output);
  output.reset(theCommands);
  Expression changedMultiplicand, leftSummand, rightSummand;
  HashedListSpecialized<Expression> theUserExpressions;
  theArgument.GetUserDefinedSymbols(theUserExpressions);
  if (theUserExpressions.size==1)
  { changedMultiplicand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[1]);
    leftSummand.MakeXOX(theCommands, theCommands.opTimes(), changedMultiplicand, theArgument[2]);
    changedMultiplicand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[2]);
    rightSummand.MakeXOX(theCommands, theCommands.opTimes(), theArgument[1], changedMultiplicand);
    Expression numerator, twoE;
    numerator.MakeXOX(theCommands, theCommands.opMinus(), leftSummand, rightSummand);
    twoE.AssignValue(2, theCommands);
    changedMultiplicand.MakeXOX(theCommands, theCommands.opThePower(), theArgument[2], twoE);
    return output.MakeXOX(theCommands, theCommands.opDivide(), numerator, changedMultiplicand);
  }
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
  if (input[1].IsConstant())
    return false;
  HashedListSpecialized<Expression> theList;
  input.GetUserDefinedSymbols(theList);
  if (theList.size!=1)
    return false;
  if (input[2]>input[1] || input[2]==input[1])
    return false;
//  std::cout << "ere be i, number 1!";
  output=input;
  output.children.SwapTwoIndices(1,2);
  return true;
}

bool CommandListFunctions::outerCommuteAtimesBtimesCifUnivariate(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::outerCommuteAtimesBifUnivariate");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes(),3))
    return false;
  const Expression& leftE=input[1];
  if (leftE.IsConstant())
    return false;
  if (!input[2].IsListNElementsStartingWithAtom(theCommands.opTimes(),3))
    return false;
  const Expression& rightE=input[2][1];
  HashedListSpecialized<Expression> theList;

  leftE.GetUserDefinedSymbols(theList);
  rightE.GetUserDefinedSymbols(theList);
  if (theList.size!=1)
    return false;
  if (rightE>leftE || leftE==rightE)
    return false;
//  std::cout << "ere be i, number 2!";
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
  if (input[2].IsBuiltInOperation())
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
  if (!input[1].IsOperation(&numeratorString))
    return false;
  if (!input[2].IsOperation(&denominatorString))
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

bool CommandListFunctions::innerCrash(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerCrash");
  crash << "<hr>This is a test of the crashing mechanism of the calculator. Are log files created correctly? Check the /output/ directory." << crash;
  return output.AssignValue((std::string)"Crashed succesfully", theCommands);
}

void CommandList::AutomatedTestRun
(List<std::string>& inputStringsTest, List<std::string>& outputStringsTestWithInit, List<std::string>& outputStringsTestNoInit)
{ MacroRegisterFunctionWithName("CommandList::AutomatedTestRun");
  CommandList theTester;
  int numFunctionsToTest=0;
  for (int i=0; i<this->FunctionHandlers.size; i++)
    numFunctionsToTest+=this->FunctionHandlers[i].size;
  numFunctionsToTest+=operationsCompositeHandlers.size;
  inputStringsTest.SetExpectedSize(numFunctionsToTest);
  inputStringsTest.SetSize(0);
  for (int i=0; i<this->FunctionHandlers.size; i++)
    for (int j=0; j<this->FunctionHandlers[i].size; j++)
      if (this->FunctionHandlers[i][j].theFunction!=CommandList::innerAutomatedTest &&
          this->FunctionHandlers[i][j].theFunction!=CommandList::innerAutomatedTestSetKnownGoodCopy &&
          this->FunctionHandlers[i][j].theFunction!=CommandListFunctions::innerCrash
          )
        inputStringsTest.AddOnTop(this->FunctionHandlers[i][j].theExample);
  for (int i=0; i<this->operationsCompositeHandlers.size; i++)
    for (int j=0; j<this->operationsCompositeHandlers[i].size; j++)
      inputStringsTest.AddOnTop(this->operationsCompositeHandlers[i][j].theExample);
  outputStringsTestWithInit.SetSize(inputStringsTest.size);
  outputStringsTestNoInit.SetSize(inputStringsTest.size);
  ProgressReport theReport(this->theGlobalVariableS);
  for (int i=0; i<inputStringsTest.size; i++)
  { double startingTime=this->theGlobalVariableS->GetElapsedSeconds();
    theTester.init(*this->theGlobalVariableS);
    Expression dummyCommands, tempE;
    tempE.AssignValue<std::string>("Input suppressed", theTester);
    dummyCommands.reset(theTester);
    dummyCommands.AddChildAtomOnTop(theTester.opEndStatement());
    dummyCommands.AddChildOnTop(tempE);

    std::cout << "<hr>Evaluating command of index " << i << ": " << inputStringsTest[i];
    theTester.Evaluate(inputStringsTest[i]);
    outputStringsTestWithInit[i]=theTester.theProgramExpression.ToString(0, &dummyCommands);
    std::cout << "<br>To get: " << theTester.theProgramExpression.ToString();
    std::cout << "<br>Done in: " << this->theGlobalVariableS->GetElapsedSeconds()-startingTime << " seconds. ";
    std::stringstream reportStream;
    reportStream << "Testing expression " << i << " out of " << inputStringsTest.size << ". ";
    theReport.Report(reportStream.str());
  }
/*  theTester.init(*this->theGlobalVariableS);
  for (int i=0; i<inputStringsTest.size; i++)
  { double startingTime=this->theGlobalVariableS->GetElapsedSeconds();
    std::cout << "<hr>Evaluating without initialization: " << inputStringsTest[i];
    theTester.Evaluate(inputStringsTest[i]);
    outputStringsTestNoInit[i]=theTester.theProgramExpression.ToString();
    std::cout << "<br>To get: " << outputStringsTestNoInit[i];
    std::cout << "<br>Done in: " << this->theGlobalVariableS->GetElapsedSeconds()-startingTime << " seconds. ";
  }*/
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
  if (!denominatorBase.IsAtoM())
    return false;
  MonomialCollection<Expression, Rational> numerators, numeratorsNew;
  theCommands.CollectSummands(theCommands, input[1], numerators);
  numeratorsNew.SetExpectedSize(numerators.size());
  numeratorsNew.MakeZero();
  Expression numeratorMultiplicandLeft, numeratorMultiplicandRight, numeratorBaseRight, numeratorExponentRight;
  Expression newNumSummand, newNumSummandRightPart, newNumExponent, mOneE;
  mOneE.AssignValue(-1, theCommands);
  for (int i=0; i<numerators.size(); i++)
  { if (numerators[i].IsConstant())
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

void Expression::GetUserDefinedSymbols(HashedListSpecialized<Expression>& inputOutputList)const
{ MacroRegisterFunctionWithName("Expression::GetUserDefinedSymbols");
  if (this->IsBuiltInFunctionOrOperation() || this->IsBuiltInType())
    return;
  if (this->IsAtoM())
    inputOutputList.AddOnTopNoRepetition(*this);
  for (int i=0; i<this->children.size; i++)
    (*this)[i].GetUserDefinedSymbols(inputOutputList);
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
  input.GetUserDefinedSymbols(theList);
  return output.MakeSequence(theCommands, theList);
}
