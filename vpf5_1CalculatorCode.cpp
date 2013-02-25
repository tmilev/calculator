//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader2.h"
#include "vpfHeader1_4SemisimpleLieAlgebras.h"

static ProjectInformationInstance ProjectInfoVpf5_1cpp
(__FILE__, "Implementation file for the calculator parser part 3: meant for built-in functions. ");

bool CommandList::innerGCDOrLCM
(CommandList& theCommands, const Expression& input, Expression& output, bool doGCD)
{ MacroRegisterFunctionWithName("CommandList::fGCD");
  Vector<Polynomial<Rational> > thePolys;
  Expression theContext(theCommands);
  std::cout << "<br>Time elapsed before calling innerGCDOrLCM: "
  << theCommands.theGlobalVariableS->GetElapsedSeconds() << " seconds.";
  std::cout << "<br>Input lispified: " << input.Lispify();
  if (!theCommands.GetVectorFromFunctionArguments(input, thePolys, &theContext, 2, theCommands.innerPolynomial))
    return output.SetError("Failed to extract a list of 2 polynomials. ", theCommands);
  std::cout << "<br>Time elapsed after extracting two polynomials in innerGCDOrLCM: "
  << theCommands.theGlobalVariableS->GetElapsedSeconds() << " seconds.";
  Polynomial<Rational> outputP;
//  std::cout << "<br>context: " << theContext.ToString();
//  std::cout << "<br>The polys: " << thePolys.ToString();
  if (doGCD)
    RationalFunctionOld::gcd(thePolys[0], thePolys[1], outputP);
  else
    RationalFunctionOld::lcm(thePolys[0], thePolys[1], outputP);
  return output.AssignValueWithContext(outputP, theContext, theCommands);
}

bool CommandList::innerPolynomialDivisionRemainder
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::fPolynomialDivisionQuotientRemainder");
  Vector<Polynomial<Rational> > thePolys;
  Expression theContext(theCommands);
  if (!theCommands.GetVectorFromFunctionArguments(input, thePolys, &theContext, 0, theCommands.innerPolynomial))
    return output.SetError("Failed to extract list of polynomials. ", theCommands);
  if (thePolys.size<2)
    return output.SetError("Expression takes two or more arguments", theCommands);
  GroebnerBasisComputation theGB;
  theGB.theBasiS.SetSize(thePolys.size-1);
  for (int i=1; i<thePolys.size; i++)
  { if (thePolys[i].IsEqualToZero())
      return output.SetError("Division by zero.", theCommands);
    theGB.theBasiS[i-1]=thePolys[i];
  }
  std::cout << "<hr>The polys: " << thePolys.ToString() << "<br>The gb basis: "
  << theGB.theBasiS.ToString() << "<hr>";
  Polynomial<Rational> outputRemainder;
  theGB.initForDivisionAlone(theGB.theBasiS, theCommands.theGlobalVariableS);
  theGB.RemainderDivisionWithRespectToBasis
  (thePolys[0], &outputRemainder, theCommands.theGlobalVariableS, -1);
  return output.AssignValueWithContext(outputRemainder, theContext, theCommands);
}

bool CommandList::innerPolynomialDivisionVerbose
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerPolynomialDivisionVerbose");
  Vector<Polynomial<Rational> > thePolys;
  Expression theContext(theCommands);
  if (!theCommands.GetVectorFromFunctionArguments(input, thePolys, &theContext, 0, theCommands.innerPolynomial))
    return output.SetError("Failed to extract list of polynomials. ", theCommands);
  if (thePolys.size<2)
    return output.SetError("Expression takes two or more arguments", theCommands);
  GroebnerBasisComputation theGB;
  theGB.flagDoLogDivision=true;
  theGB.theBasiS.SetSize(thePolys.size-1);
  for (int i=1; i<thePolys.size; i++)
  { if (thePolys[i].IsEqualToZero())
      return output.SetError("Division by zero.", theCommands);
    theGB.theBasiS[i-1]=thePolys[i];
  }
  Polynomial<Rational> outputRemainder;
  theGB.initForDivisionAlone(theGB.theBasiS, theCommands.theGlobalVariableS);
  theGB.RemainderDivisionWithRespectToBasis
  (thePolys[0], &outputRemainder, theCommands.theGlobalVariableS, -1);
  return output.AssignValue(theGB.GetDivisionString(), theCommands);
}

bool CommandList::fSolveSeparableBilinearSystem
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::fSolveSeparableBilinearSystem");
  Vector<Polynomial<Rational> > thePolys;
  Expression theContext(theCommands);
  if (!theCommands.GetVectorFromFunctionArguments(input, thePolys, &theContext, 0, theCommands.innerPolynomial))
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

bool CommandList::innerSSsubalgebras
(CommandList& theCommands, const Expression& input, Expression& output)
{ //bool showIndicator=true;
  MacroRegisterFunctionWithName("CommandList::innerSSsubalgebras");
  std::string errorString;
  SemisimpleLieAlgebra* ownerSSPointer;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerSSLieAlgebra, input, ownerSSPointer, &errorString))
    return output.SetError(errorString, theCommands);

  SemisimpleLieAlgebra& ownerSS=*ownerSSPointer;
  std::stringstream out;
  if (ownerSS.GetRank()>4)
  { out << "<b>This code is completely experimental and has been set to run up to rank 4."
    << " As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
    return output.AssignValue(out.str(), theCommands);
  }
  else
    out << "<b>This code is completely experimental. Use the following printouts on "
    << "your own risk</b>";
  SemisimpleSubalgebras tempSSsas(ownerSS);
  SemisimpleSubalgebras& theSSsubalgebras=
  theCommands.theObjectContainer.theSSsubalgebras
  [theCommands.theObjectContainer.theSSsubalgebras.AddNoRepetitionOrReturnIndexFirst(tempSSsas)]
  ;
  theSSsubalgebras.FindTheSSSubalgebras(ownerSS, theCommands.theGlobalVariableS);
  return output.AssignValue(theSSsubalgebras, theCommands);
}

bool CommandList::innerEmbedSSalgInSSalg
(CommandList& theCommands, const Expression& input, Expression& output)
{ //bool showIndicator=true;
  MacroRegisterFunctionWithName("CommandList::innerEmbedSSalgInSSalg");
  if (!input.IsListNElements(3))
    return output.SetError("I expect two arguments - the two semisimple subalgebras.", theCommands);
  const Expression& EsmallSA=input[1];
  const Expression& ElargeSA=input[2];

  std::string errorString;
  SemisimpleLieAlgebra* theSmallSapointer;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerSSLieAlgebra, EsmallSA, theSmallSapointer, &errorString))
    return output.SetError(errorString, theCommands);
  SemisimpleLieAlgebra* thelargeSapointer;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerSSLieAlgebra, ElargeSA, thelargeSapointer, &errorString))
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
  SemisimpleSubalgebras tempSSsas(ownerSS);
  SemisimpleSubalgebras& theSSsubalgebras=
  theCommands.theObjectContainer.theSSsubalgebras
  [theCommands.theObjectContainer.theSSsubalgebras.AddNoRepetitionOrReturnIndexFirst(tempSSsas)]
  ;
  DynkinSimpleType theType;
  if (!smallSS.theWeyl.theDynkinType.IsSimple(&theType.theLetter, &theType.theRank))
    return output.SetError("I've been instructed to act on simple types only. ", theCommands);
  out << "Attempting to embed " << theType.ToString() << " in " << ownerSS.GetLieAlgebraName();
  theSSsubalgebras.FindAllEmbeddings
  (theType, ownerSS, theCommands.theGlobalVariableS);
  return output.AssignValue(theSSsubalgebras, theCommands);
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
  const Expression& numComputationsE=input[1];
  Rational upperBound=0;
  if (!numComputationsE.IsOfType(&upperBound))
    return output.SetError
    ("Failed to convert the first argument of the expression to rational number.", theCommands);
  if (upperBound>1000000)
    return output.SetError
    ("Error: your upper limit of polynomial operations exceeds 1000000, which is too large.\
     You may use negative or zero number give no computation bound, but please don't. ", theCommands);
  int upperBoundComputations=(int) upperBound.DoubleValue();
  output=input;
  output.children.RemoveIndexShiftDown(1);
  if (!theCommands.GetVectorFromFunctionArguments<Polynomial<Rational> >
      (output, inputVector, &theContext, -1, theCommands.innerPolynomial))
    return output.SetError("Failed to extract polynomial expressions", theCommands);
  //theContext.VariableImages.QuickSortAscending();
  //theCommands.GetVector<Polynomial<Rational> >
  //(output, inputVector, &theContext, -1, theCommands.innerPolynomial);

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
    <<  outputGroebner.size << " elements, computed using algorithm 1, "
    << " using " << theGroebnerComputation.NumberOfComputations << " polynomial operations. ";
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

bool CommandList::innerDeterminant
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerDeterminant");
  Matrix<Rational> matRat;
  Matrix<RationalFunctionOld> matRF;
  Expression theContext;
  if (theCommands.GetMatrix(input, matRat, 0, 0, 0))
  { if (matRat.NumRows==matRat.NumCols)
    { if (matRat.NumRows>100)
      { theCommands.Comments << "I have been instructed not to compute determinants of rational matrices larger than "
        << " 100 x 100 " << ", and your matrix had " << matRat.NumRows << " rows. " << "To lift the restriction "
        << "edit function located in file " << __FILE__ << ", line " << __LINE__ << ". ";
        return false;
      }
      //std::cout << " <br> ... and the matRat is: " << matRat.ToString();
      return output.AssignValue(matRat.GetDeterminant(), theCommands);
    } else
      return output.SetError("Requesting to compute determinant of non-square matrix. ", theCommands);
  }
  if (!theCommands.GetMatrix(input, matRF, &theContext, -1, theCommands.innerRationalFunction))
  { theCommands.Comments << "I have been instructed to only compute determinants of matrices whose entries are "
    << " ratinoal functions or rationals, and I failed to convert your matrix to either type. "
    << " If this is not how you expect this function to act, correct it: the code is located in  "
    << " file " << __FILE__ << ", line " << __LINE__ << ". ";
    return false;
  }
  if (matRF.NumRows==matRF.NumCols)
  { if (matRF.NumRows>10)
    { theCommands.Comments << "I have been instructed not to compute determinants of matrices of rational functions "
      << " larger than " << " 10 x 10, and your matrix had " << matRF.NumRows << " rows. "
      << "To lift the restriction "
      << "edit function located in file " << __FILE__ << ", line " << __LINE__ << ". ";
      return false;
    }
    return output.AssignValue(matRF.GetDeterminant(), theCommands);
  } else
    return output.SetError("Requesting to comptue determinant of non-square matrix. ", theCommands);
}

bool CommandList::innerMatrixRational
(CommandList& theCommands, const Expression& input, Expression& output)
{ Matrix<Rational> outputMat;
  if (!theCommands.GetMatriXFromArguments(input, outputMat, 0, -1, 0))
  { theCommands.Comments << "<br>Failed to get matrix of rationals. ";
    return false;
  }
  return output.AssignValue(outputMat, theCommands);
}

bool CommandList::innerDrawPolarRfunctionTheta
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerDrawPolarRfunctionTheta");
  if (!input.IsListNElements(4))
    return output.SetError
    ("Drawing polar coordinates takes three arguments: function, lower angle \
      bound and upper angle bound. ", theCommands);
  const Expression& lowerE=input[2];
  const Expression& upperE=input[3];
  Expression functionE;
  Rational upperBound, lowerBound;
  if (!lowerE.IsOfType(&upperBound) || !upperE.IsOfType(&lowerBound))
    return
    output.SetError
    ("Failed to convert upper and lower bounds of drawing function to rational numbers.", theCommands);
  if (upperBound<lowerBound)
    MathRoutines::swap(upperBound, lowerBound);
  if (! theCommands.innerSuffixNotationForPostScript(theCommands, input[1], functionE))
    return false;
  std::stringstream out, resultStream;
  out << CGI::GetHtmlMathSpanPure(input[1].ToString())
  << "<br>";
  resultStream << "\\documentclass{article}\\usepackage{pstricks}\\usepackage{pst-plot}"
  << "\\usepackage{pst-3dplot}\\begin{document} \\pagestyle{empty}";
  resultStream << " \\begin{pspicture}(-5, 5)(5,5)";
  resultStream << "\\psaxes[labels=none]{<->}(0,0)(-4.5,-4.5)(4.5,4.5)";
  resultStream << "\\parametricplot[linecolor=red, plotpoints=1000]{"
  << lowerBound.DoubleValue() << "}{" << upperBound.DoubleValue() << "}{";
  std::string funString=functionE.GetValuE<std::string>();
  resultStream << funString << " t 57.29578 mul cos mul " << funString
  << " t 57.29578 mul sin mul " << "}";
  resultStream << "\\end{pspicture}\\end{document}";
  out << theCommands.WriteDefaultLatexFileReturnHtmlLink(resultStream.str(), true);
  out << "<br><b>LaTeX code used to generate the output. </b><br>" << resultStream.str();
  return output.AssignValue(out.str(), theCommands);
}

void CalculusFunctionPlot::operator+=(const CalculusFunctionPlot& other)
{ this->theFunctions.AddListOnTop(other.theFunctions);
  this->lowerBounds.AddListOnTop(other.lowerBounds);
  this->upperBounds.AddListOnTop(other.upperBounds);
  this->theFunctionsCalculatorInput.AddListOnTop(other.theFunctionsCalculatorInput);
}

std::string CalculusFunctionPlot::GetPlotStringAddLatexCommands(bool useHtml)
{ std::stringstream resultStream;
  resultStream << "\\documentclass{article}\\usepackage{pstricks}"
  << "\\usepackage{pst-3dplot}\\usepackage{pst-plot}\\begin{document} \\pagestyle{empty}";
  if (useHtml)
    resultStream << "<br>";
  resultStream << " \\psset{xunit=1cm, yunit=1cm}";
  if (useHtml)
    resultStream << "<br>";
  resultStream << "\\begin{pspicture}(-5, -5)(5,5)\n\n";
  if (useHtml)
    resultStream << "<br>";
  resultStream << "\\psframe*[linecolor=white](-5,-5)(5,5)\n\n";
  if (useHtml)
    resultStream << "<br>";
  resultStream << " \\psaxes[ticks=none, labels=none]{<->}(0,0)(-4.5,-4.5)(4.5,4.5)";
  if (useHtml)
    resultStream << "<br>";
  for (int i=0; i<this->theFunctions.size; i++)
  { resultStream << "\n\n%Function formula: " << this->theFunctionsCalculatorInput[i] << "\n\n";
    if (useHtml)
      resultStream << "<br>";
    resultStream << "\\psplot[linecolor=red, plotpoints=1000]{"
    << this->lowerBounds[i].DoubleValue() << "}{"
    << this->upperBounds[i].DoubleValue() << "}{";
    resultStream << this->theFunctions[i] << "}";
  }
  if (useHtml)
    resultStream << "<br>";
  resultStream << "\\end{pspicture}\n\n";
  if (useHtml)
    resultStream << "<br>";
  resultStream << "\\end{document}";
  return resultStream.str();
}

bool CommandList::innerPlot2D
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerPlot2D");
  //std::cout << input.ToString();
  if (!input.IsListNElements(4))
    return output.SetError
    ("Plotting coordinates takes three arguments: function, lower and upper bound. ", theCommands);
  const Expression& lowerE=input[2];
  const Expression& upperE=input[3];
  Expression functionE;
  Rational upperBound, lowerBound;
  if (!lowerE.IsOfType(&upperBound) || !upperE.IsOfType(&lowerBound))
    return
    output.SetError
    ("Failed to convert upper and lower bounds of drawing function to rational numbers.", theCommands);
  if (upperBound<lowerBound)
    MathRoutines::swap(upperBound, lowerBound);
  if (! theCommands.innerSuffixNotationForPostScript(theCommands, input[1], functionE))
    return false;
  CalculusFunctionPlot thePlot;
  thePlot.theFunctions.AddOnTop(functionE.GetValuE<std::string>());
  thePlot.lowerBounds.AddOnTop(lowerBound);
  thePlot.upperBounds.AddOnTop(upperBound);
  thePlot.theFunctionsCalculatorInput.AddOnTop(input[1].ToString());
  return output.AssignValue(thePlot, theCommands);
}

bool CommandList::innerSuffixNotationForPostScript
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::fSuffixNotation");
  RecursionDepthCounter theCounter(&theCommands.RecursionDeptH);
  if (*theCounter.theCounter ==theCommands.MaxRecursionDeptH-2)
    return output.AssignValue((std::string) "...", theCommands);
  std::string currentString;
  if (input.IsOperation(&currentString))
  { if (input.theData>=theCommands.NumPredefinedVars)
      return output.AssignValue(currentString, theCommands);
    if (currentString=="+")
      return output.AssignValue<std::string>("add ", theCommands);
    if (currentString=="*")
      return output.AssignValue<std::string>("mul ", theCommands);
    if (currentString=="-")
      return output.AssignValue<std::string>("sub ", theCommands);
    if (currentString=="/")
      return output.AssignValue<std::string>("div ", theCommands);
    if (currentString=="^")
      return output.AssignValue<std::string>("exp ", theCommands);
    return output.SetError("Cannot convert "+currentString+ " to suffix notation.", theCommands);
  }
  std::stringstream out;
  if (input.IsOfType<Rational>())
  { out << input.GetValuE<Rational>().DoubleValue();
    return output.AssignValue(out.str(), theCommands);
  }
  Expression currentE;
  bool useUsualOrder=
  !input[0].IsAtoM(theCommands.opDivide()) &&
  !input[0].IsAtoM(theCommands.opThePower());
//  if (input[0].IsAtoM(theCommands.opDivide()))
//    std::cout << input.Lispify();
  if (useUsualOrder)
    for (int i=input.children.size-1; i>=1; i--)
    { if (!theCommands.innerSuffixNotationForPostScript(theCommands, input[i], currentE))
        return output.SetError("Failed to convert "+input[i].ToString(), theCommands);
      if (!currentE.IsOfType(&currentString))
        return output.SetError("Failed to convert "+input[i].ToString(), theCommands);
      out << currentString << " ";
    }
  else
    for (int i=1; i<input.children.size; i++)
    { if (!theCommands.innerSuffixNotationForPostScript(theCommands, input[i], currentE))
        return output.SetError("Failed to convert "+input[i].ToString(), theCommands);
      if (!currentE.IsOfType(&currentString))
        return output.SetError("Failed to convert "+input[i].ToString(), theCommands);
      out << currentString << " ";
    }
  if (!theCommands.innerSuffixNotationForPostScript(theCommands, input[0], currentE))
    return output.SetError("Failed to convert "+input[0].ToString(), theCommands);
  if (!currentE.IsOfType(&currentString))
    return output.SetError("Failed to convert "+input[0].ToString(), theCommands);
  if (currentString=="\\sin")
    currentString="sin";
  if (currentString=="\\cos")
    currentString="cos";
  if (currentString=="\\cot")
    currentString="cot";
  if (currentString=="\\tan")
    currentString="tan";
  if (currentString=="sin" || currentString=="cos" || currentString=="tan" || currentString=="cot")
    out << " 57.29578 mul ";
  out << currentString << " ";
  return output.AssignValue(out.str(), theCommands);
}

bool CommandList::innerCharacterSSLieAlgFD
  (CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerCharacterSSLieAlgFD");
  Vector<Rational> theHW;
  Selection parSel;
  SemisimpleLieAlgebra* ownerSSLiealg;
  Expression tempE, tempE2;
  if (!theCommands.innerGetTypeHighestWeightParabolic
      (theCommands, input, output, theHW, parSel, tempE, ownerSSLiealg, 0))
    return false;
  if (output.IsError())
    return true;
  if (!parSel.CardinalitySelection==0)
    return output.SetError
    ("I know only to compute with finite dimensional characters, for the time being.", theCommands);
  charSSAlgMod<Rational> theElt;
  theElt.MakeFromWeight
  (ownerSSLiealg->theWeyl.GetSimpleCoordinatesFromFundamental(theHW), *ownerSSLiealg);
  return output.AssignValue(theElt, theCommands);

}
