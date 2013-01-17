//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
ProjectInformationInstance ProjectInfoVpf5_1cpp(__FILE__, "Implementation file for the calculator parser part 3: meant for built-in functions. ");

bool CommandList::fGCDOrLCM
(CommandList& theCommands, const Expression& input, Expression& output, bool doGCD)
{ MacroRegisterFunctionWithName("CommandList::fGCD");
  Vector<Polynomial<Rational> > thePolys;
  Expression theContext(theCommands);
  if (!theCommands.GetVector(input, thePolys, &theContext, 2, theCommands.fPolynomial))
    return output.SetError("Failed to extract a list of 2 polynomials. ", theCommands);
  Polynomial<Rational> outputP;
//  std::cout << "context: " << theContext.VariableImages.ToString();
  if (doGCD)
    RationalFunctionOld::gcd(thePolys[0], thePolys[1], outputP);
  else
    RationalFunctionOld::lcm(thePolys[0], thePolys[1], outputP);
  return output.AssignValueWithContext(outputP, theContext, theCommands);
}

bool CommandList::fPolynomialDivisionQuotientRemainder
(CommandList& theCommands, const Expression& input, Expression& output, bool returnQuotient)
{ MacroRegisterFunctionWithName("CommandList::fPolynomialDivisionQuotientRemainder");
  Vector<Polynomial<Rational> > thePolys;
  Expression theContext(theCommands);
  if (!theCommands.GetVector(input, thePolys, &theContext, 2, theCommands.fPolynomial))
    return output.SetError("Failed to extract a list of 2 polynomials. ", theCommands);
  Polynomial<Rational> outputR, outputQ;
  thePolys[0].DivideBy(thePolys[1], outputQ, outputR);
  if (returnQuotient)
    return output.AssignValueWithContext(outputQ, theContext, theCommands);
  else
    return output.AssignValueWithContext(outputR, theContext, theCommands);
}

bool CommandList::fSolveSeparableBilinearSystem
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::fSolveSeparableBilinearSystem");
  Vector<Polynomial<Rational> > thePolys;
  Expression theContext(theCommands);
  if (!theCommands.GetVector(input, thePolys, &theContext, 0, theCommands.fPolynomial))
    return output.SetError("Failed to extract list of polynomials. ", theCommands);
  int numVars=theContext.GetNumContextVariables();
  HashedList<MonomialP> theMonsInPlay;
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
  for (int i=0; i<thePolys.size; i++)
    theMonsInPlay.AddOnTopNoRepetition(thePolys[i]);
  std::cout << "<br>The context vars:<br>" << theContext.ToString();
  std::cout << "<br>The mons in play: <br>" << theMonsInPlay.ToString();
  std::cout << "<br>The mons in play, formatted: <br>" << theMonsInPlay.ToString(&theFormat);
  Selection linearIndices;
  linearIndices.MakeFullSelection(numVars);
  for (int i=0; i<theMonsInPlay.size; i++)
  { bool foundIndex=false;
    for (int j=theMonsInPlay[i].GetMinNumVars()-1; j>=0; j--)
      if (theMonsInPlay[i][j]!=0)
      { if (foundIndex)
          linearIndices.RemoveSelection(j);
        foundIndex=true;
      }
  }
  Matrix<RationalFunctionOld> theSystem, theConstantTerms;
  theSystem.init(thePolys.size, linearIndices.CardinalitySelection);
  theConstantTerms.init(thePolys.size, 1);
  RationalFunctionOld theZero;
  theZero.MakeZero(theCommands.theGlobalVariableS);
  theSystem.NullifyAll(theZero);
  theConstantTerms.NullifyAll(theZero);
  MonomialP newMon;
  Polynomial<Rational> tempP;
  for (int j=0; j<thePolys.size; j++)
    for(int k=0; k<thePolys[j].size; k++)
    { MonomialP& curMon=thePolys[j][k];
      bool found=false;
      for (int i=0; i<linearIndices.CardinalitySelection; i++)
        if (curMon[i]!=0)
        { newMon=curMon;
          newMon[i]=0;
          tempP.MakeZero();
          tempP.AddMonomial(newMon, thePolys[j].theCoeffs[k]);
          theSystem(j,i)+=tempP;
          found=true;
          break;
        }
      if (!found)
      { tempP.MakeZero();
        tempP.SubtractMonomial(curMon, thePolys[j].theCoeffs[k]);
        theConstantTerms(j,0)+=tempP;
      }
    }
  std::cout << "<br>The system matrix: <br>" << theSystem.ToString(&theFormat);
  std::cout << "<br>The column vector: <br>" << theConstantTerms.ToString(&theFormat);
  Selection tempSel;
  bool oldReportFlag=true;
  MathRoutines::swap(oldReportFlag, theCommands.theGlobalVariableS->flagGaussianEliminationProgressReport);
  theSystem.GaussianEliminationByRows(theConstantTerms, tempSel, theCommands.theGlobalVariableS);
  MathRoutines::swap(oldReportFlag, theCommands.theGlobalVariableS->flagGaussianEliminationProgressReport);
  std::stringstream out;
  out << theSystem.ToString(&theFormat) << theConstantTerms.ToString(&theFormat);
  return output.AssignValue(out.str(), theCommands);
}

bool CommandList::fSSsubalgebras
(CommandList& theCommands, const Expression& input, Expression& output)
{ //bool showIndicator=true;
  MacroRegisterFunctionWithName("CommandList::fSSsubalgebras");

  std::string errorString;
  SemisimpleLieAlgebra* theSSowner;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.fSSAlgebra, input[1], theSSowner, &errorString))
    return output.SetError(errorString, theCommands);

  SemisimpleLieAlgebra& ownerSS=*theSSowner;
  std::stringstream out;
  if (ownerSS.GetRank()>4)
  { out << "<b>This code is completely experimental and has been set to run up to rank 4."
    << " As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
    return output.AssignValue(out.str(), theCommands);
  }
  else
    out << "<b>This code is completely experimental. Use the following printouts on "
    << "your own risk</b>";
  SemisimpleSubalgebras theSSsubalgebras(ownerSS.owner, ownerSS.indexInOwner);
  theSSsubalgebras.FindTheSSSubalgebras
  (ownerSS.owner, ownerSS.indexInOwner, theCommands.theGlobalVariableS);
  FormatExpressions theFormat;
  std::stringstream out1, out2;
  out1 << theCommands.PhysicalPathOutputFolder
  << ownerSS.theWeyl.WeylLetter << ownerSS.GetRank() << "/";
  out2 << theCommands.DisplayPathOutputFolder
  << ownerSS.theWeyl.WeylLetter << ownerSS.GetRank() << "/";
  theFormat.physicalPath=out1.str();
  theFormat.htmlPathServer=out2.str();
  out << "<br>" << theSSsubalgebras.ToString(&theFormat);
  return output.AssignValue(out.str(), theCommands);
}

bool CommandList::fEmbedSSalgInSSalg
(CommandList& theCommands, const Expression& input, Expression& output)
{ //bool showIndicator=true;
  MacroRegisterFunctionWithName("CommandList::fEmbedSSalgInSSalg");
  if (!input.IsListNElements(3))
    return output.SetError("I expect two arguments - the two semisimple subalgebras.", theCommands);
  Expression& EsmallSA=input[1];
  Expression& ElargeSA=input[2];

  std::string errorString;
  SemisimpleLieAlgebra* theSmallSapointer;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.fSSAlgebra, EsmallSA, theSmallSapointer, &errorString))
    return output.SetError(errorString, theCommands);
  SemisimpleLieAlgebra* thelargeSapointer;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.fSSAlgebra, ElargeSA, thelargeSapointer, &errorString))
    return output.SetError(errorString, theCommands);

  SemisimpleLieAlgebra& ownerSS=*thelargeSapointer;
  SemisimpleLieAlgebra& smallSS=*theSmallSapointer;
  std::stringstream out;
  if (ownerSS.GetRank()>4)
  { out << "<b>This code is completely experimental and has been set to run up to rank 4."
    << " As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
    return output.AssignValue(out.str(), theCommands);
  }
  else
    out << "<b>This code is completely experimental. Use the following printouts on "
    << "your own risk</b>";
  SemisimpleSubalgebras theSSsubalgebras(ownerSS.owner, ownerSS.indexInOwner);
  DynkinSimpleType theType;
  theType.theLetter=smallSS.theWeyl.WeylLetter;
  theType.theRank=smallSS.GetRank();
  out << "Attempting to embed " << theType.ToString() << " in " << ownerSS.GetLieAlgebraName();
  theSSsubalgebras.FindAllEmbeddings
  (theType, ownerSS.owner, ownerSS.indexInOwner, theCommands.theGlobalVariableS);
  FormatExpressions theFormat;
  std::stringstream out1, out2;
  out1 << theCommands.PhysicalPathOutputFolder
  << ownerSS.theWeyl.WeylLetter << ownerSS.GetRank() << "/";
  out2 << theCommands.DisplayPathOutputFolder
  << ownerSS.theWeyl.WeylLetter << ownerSS.GetRank() << "/";
  theFormat.physicalPath=out1.str();
  theFormat.htmlPathServer=out2.str();
  out << "<br>" << theSSsubalgebras.ToString(&theFormat);
  return output.AssignValue(out.str(), theCommands);
}

bool CommandList::fGroebner
(CommandList& theCommands, const Expression& input, Expression& output, bool useGr)
{ MacroRegisterFunctionWithName("CommandList::fGroebnerBuchberger");
  Vector<Polynomial<Rational> > inputVector;
  Vector<Polynomial<ElementZmodP> > inputVectorZmodP;
  List<Polynomial<Rational> > outputGroebner, outputGroebner2;
  Expression theContext;
  if (input.children.size<3)
    return output.SetError("Function takes at least two arguments. ", theCommands);
  Expression& numComputationsE=input[1];
  Rational upperBound=0;
  if (!numComputationsE.IsOfType(&upperBound))
    return output.SetError
    ("Failed to convert the first argument of the expression to rational number.", theCommands);
  if (upperBound>1000000)
    return output.SetError
    ("Error: your upper limit of polynomial operations exceeds 1000000, which is too large.\
     You may use negative or zero upper bound to give no bound, but please don't. ", theCommands);
  int upperBoundComputations=(int) upperBound.DoubleValue();
  output=input;
  output.children.PopIndexShiftDown(1);
  if (!theCommands.GetVector<Polynomial<Rational> >
      (output, inputVector, &theContext, -1, theCommands.fPolynomial))
    return output.SetError("Failed to extract polynomial expressions", theCommands);
  //theContext.VariableImages.QuickSortAscending();
  //theCommands.GetVector<Polynomial<Rational> >
  //(output, inputVector, &theContext, -1, theCommands.fPolynomial);

  for (int i=0; i<inputVector.size; i++)
    inputVector[i].ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();

  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
//  int theNumVars=theContext.VariableImages.size;
  outputGroebner=inputVector;
  outputGroebner2=inputVector;
//  std::cout << outputGroebner.ToString(&theFormat);
  Polynomial<Rational> buffer1, buffer2, buffer3, buffer4;
  MonomialP bufferMon1, bufferMon2;
  theContext.ContextGetFormatExpressions(theCommands.theGlobalVariableS->theDefaultFormat);

  GroebnerBasisComputation theGroebnerComputation;
  theGroebnerComputation.theMonOrdeR=
  useGr ? MonomialP::LeftIsGEQTotalDegThenLexicographicLastVariableStrongest :
  MonomialP::LeftIsGEQLexicographicLastVariableStrongest;
  theGroebnerComputation.MaxNumComputations=upperBoundComputations;
  bool success=
  theGroebnerComputation.TransformToReducedGroebnerBasis
  (outputGroebner, theCommands.theGlobalVariableS);
  std::stringstream out;
  out << "Letter/expression ordrer: ";
  for (int i=0; i<theContext.GetNumContextVariables(); i++)
  { out << theContext.ContextGetContextVariable(i).ToString();
    if (i!=theContext.GetNumContextVariables()-1)
      out << ", ";
  }
  out << "<br>Starting basis (" << inputVector.size  << " elements): ";
  theFormat.thePolyMonOrder= useGr ?
  MonomialP::LeftGreaterThanTotalDegThenLexicographicLastVariableStrongest :
  MonomialP::LeftGreaterThanLexicographicLastVariableStrongest;
  for(int i=0; i<inputVector.size; i++)
    out << "<br>"
    << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL(inputVector[i].ToString(&theFormat));
  if (success)
  { out << "<br>Minimal Groebner basis with "
    <<  outputGroebner.size << " elements, computed using algorithm 1.";
    for(int i=0; i<outputGroebner.size; i++)
      out << "<br> "
      << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL( outputGroebner[i].ToString(&theFormat));
  } else
  { out << "<br>Minimal Groebner basis not computed due to exceeding the user-given limit of  "
    << upperBoundComputations << " polynomial operations. ";
    out << "<br>A partial result, a (non-Groebner) basis of the ideal with "
    << theGroebnerComputation.theBasiS.size
    << " elements follows ";
    out << "<br>GroebnerLexUpperLimit{}(";
    for (int i=0; i<theGroebnerComputation.theBasiS.size; i++)
    { out << theGroebnerComputation.theBasiS[i].ToString(&theFormat);
      if (i!=theGroebnerComputation.theBasiS.size-1)
        out << ", <br>";
    }
    out << ");";
  }
/*  theGroebnerComputation.TransformToReducedGroebnerBasisImprovedAlgorithm
(outputGroebner2, theCommands.theGlobalVariableS);

  out << "<br>Minimal Groebner basis algorithm 2 (" << outputGroebner2.size << " elements):";
  for(int i=0; i<outputGroebner2.size; i++)
    out << "<br> "
    << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL(outputGroebner2[i].ToString(&theFormat))
  ;*/
  return output.AssignValue(out.str(), theCommands);
}
