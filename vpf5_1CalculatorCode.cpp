//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
ProjectInformationInstance ProjectInfoVpf5_1cpp(__FILE__, "Implementation file for the calculator parser part 3: meant for built-in functions. ");

bool CommandList::fGCDOrLCM
(CommandList& theCommands, Expression& theExpression,
 std::stringstream* comments, bool doGCD)
{ MacroRegisterFunctionWithName("CommandList::fGCD");
  Vector<Polynomial<Rational> > thePolys;
  Context theContext(theCommands);
  if (!theCommands.GetVector(theExpression, thePolys, &theContext, 2, theCommands.fPolynomial, comments))
    return theExpression.SetError("Failed to extract a list of 2 polynomials. ");
  if (theExpression.errorString!="")
    return true;
  Polynomial<Rational> outputP;
//  std::cout << "context: " << theContext.VariableImages.ToString();
  if (doGCD)
    RationalFunctionOld::gcd(thePolys[0], thePolys[1], outputP);
  else
    RationalFunctionOld::lcm(thePolys[0], thePolys[1], outputP);
  Data tempData;
  int theContextIndex=
  theCommands.theObjectContainer.theContexts.AddNoRepetitionOrReturnIndexFirst(theContext);
  tempData.MakePoly(theCommands, outputP, theContextIndex);
  theExpression.MakeAtom(tempData, theCommands);
  return true;
}

bool CommandList::fPolynomialDivisionQuotientRemainder
(CommandList& theCommands, Expression& theExpression,
 std::stringstream* comments, bool returnQuotient)
{ MacroRegisterFunctionWithName("CommandList::fPolynomialDivisionQuotientRemainder");
  Vector<Polynomial<Rational> > thePolys;
  Context theContext(theCommands);
  if (!theCommands.GetVector(theExpression, thePolys, &theContext, 2, theCommands.fPolynomial, comments))
    return theExpression.SetError("Failed to extract a list of 2 polynomials. ");
  if (theExpression.errorString!="")
    return true;
  Polynomial<Rational> outputR, outputQ;
  thePolys[0].DivideBy(thePolys[1], outputQ, outputR);
  Data tempData;
  int theContextIndex=
  theCommands.theObjectContainer.theContexts.AddNoRepetitionOrReturnIndexFirst(theContext);
  if (returnQuotient)
    tempData.MakePoly(theCommands, outputQ, theContextIndex);
  else
    tempData.MakePoly(theCommands, outputR, theContextIndex);
  theExpression.MakeAtom(tempData, theCommands);
  return true;
}

bool CommandList::fSolveSeparableBilinearSystem
(CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
{ MacroRegisterFunctionWithName("CommandList::fSolveSeparableBilinearSystem");
  Vector<Polynomial<Rational> > thePolys;
  Context theContext(theCommands);
  if (!theCommands.GetVector(theExpression, thePolys, &theContext, 0, theCommands.fPolynomial, comments))
    return theExpression.SetError("Failed to extract list of polynomials. ");
  if (theExpression.errorString!="")
    return true;
  int numVars=theContext.VariableImages.size;
  HashedList<MonomialP> theMonsInPlay;
  FormatExpressions theFormat;
  theContext.GetFormatExpressions(theFormat);
  for (int i=0; i<thePolys.size; i++)
    theMonsInPlay.AddOnTopNoRepetition(thePolys[i]);
  std::cout << "<br>The context vars:<br>" << theContext.VariableImages.ToString();
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
  theExpression.MakeStringAtom(theCommands, out.str());
  return true;
}

bool CommandList::fSSsubalgebras
(CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
{ //bool showIndicator=true;
  MacroRegisterFunctionWithName("CommandList::fSSsubalgebras");
  if (!theCommands.CallCalculatorFunction(theCommands.fSSAlgebra, theExpression, comments))
    return false;
  SemisimpleLieAlgebra& ownerSS=theExpression.GetAtomicValue().GetAmbientSSAlgebra();
  std::stringstream out;
  if (ownerSS.GetRank()>4)
  { out << "<b>This code is completely experimental and has been set to run up to rank 4."
    << " As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
    theExpression.MakeStringAtom(theCommands, out.str());
    return true;
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
  theExpression.MakeStringAtom(theCommands, out.str());
  return true;
}

bool CommandList::fEmbedSSalgInSSalg
(CommandList& theCommands, Expression& theExpression, std::stringstream* comments)
{ //bool showIndicator=true;
  MacroRegisterFunctionWithName("CommandList::fEmbedSSalgInSSalg");
  if (theExpression.children.size!=2)
    return theExpression.SetError("I expect two arguments - the two semisimple subalgebras.");
  Expression& EsmallSA=theExpression.children[0];
  Expression& ElargeSA=theExpression.children[1];
  if (!theCommands.CallCalculatorFunction(theCommands.fSSAlgebra, EsmallSA, comments))
    return false;
  if (!theCommands.CallCalculatorFunction(theCommands.fSSAlgebra, ElargeSA, comments))
    return false;

  SemisimpleLieAlgebra& ownerSS=ElargeSA.GetAtomicValue().GetAmbientSSAlgebra();
  SemisimpleLieAlgebra& smallSS=EsmallSA.GetAtomicValue().GetAmbientSSAlgebra();
  std::stringstream out;
  if (ownerSS.GetRank()>4)
  { out << "<b>This code is completely experimental and has been set to run up to rank 4."
    << " As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
    theExpression.MakeStringAtom(theCommands, out.str());
    return true;
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
  theExpression.MakeStringAtom(theCommands, out.str());
  return true;
}

bool CommandList::fGroebner
(CommandList& theCommands, Expression& theExpression,
 std::stringstream* comments, bool useGr)
{ MacroRegisterFunctionWithName("CommandList::fGroebnerBuchberger");
  Vector<Polynomial<Rational> > inputVector;
  Vector<Polynomial<ElementZmodP> > inputVectorZmodP;
  List<Polynomial<Rational> > outputGroebner, outputGroebner2;
  Context theContext;
  if (theExpression.children.size<2)
    return theExpression.SetError("Function takes at least two arguments. ");
  Expression& numComputationsE=theExpression.children[0];
  Rational upperBound=0;
  if (!numComputationsE.EvaluatesToRational(&upperBound))
    return theExpression.SetError
    ("Failed to convert the first argument of the expression to rational number.");
  if (upperBound>1000000)
    return theExpression.SetError
    ("Error: your upper limit of polynomial operations exceeds 1000000, which is too large.\
     You may use negative or zero upper bound to give no bound, but please don't. ");
  int upperBoundComputations=(int) upperBound.DoubleValue();
  theExpression.children.PopIndexShiftDown(0);
  if (!theCommands.GetVector<Polynomial<Rational> >
      (theExpression, inputVector, &theContext, -1, theCommands.fPolynomial, comments))
    return theExpression.SetError("Failed to extract polynomial expressions");
  if (theExpression.errorString!="")
    return true;
  theContext.VariableImages.QuickSortAscending();
  theCommands.GetVector<Polynomial<Rational> >
  (theExpression, inputVector, &theContext, -1, theCommands.fPolynomial, comments);

  for (int i=0; i<inputVector.size; i++)
    inputVector[i].ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();

  FormatExpressions theFormat;
  theContext.GetFormatExpressions(theFormat);
//  int theNumVars=theContext.VariableImages.size;
  outputGroebner=inputVector;
  outputGroebner2=inputVector;
//  std::cout << outputGroebner.ToString(&theFormat);
  Polynomial<Rational> buffer1, buffer2, buffer3, buffer4;
  MonomialP bufferMon1, bufferMon2;
  theContext.GetFormatExpressions(theCommands.theGlobalVariableS->theDefaultFormat);

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
  for (int i=0; i<theContext.VariableImages.size; i++)
  { out << theContext.VariableImages[i].ToString();
    if (i!=theContext.VariableImages.size-1)
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
  theExpression.MakeStringAtom(theCommands, out.str());
  return true;
}
