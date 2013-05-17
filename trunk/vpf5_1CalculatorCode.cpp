//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader2.h"
#include "vpfHeader1_4SemisimpleLieAlgebras.h"

static ProjectInformationInstance ProjectInfoVpf5_1cpp
(__FILE__, "Implementation file for the calculator parser part 3: meant for built-in functions. ");

template<class Element>
bool Matrix<Element>::SystemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution
(Matrix<Element>& matA, Matrix<Element>& matb, Vector<Element>* outputSolution,
 GlobalVariables* theGlobalVariables)
//this function return true if Ax=b>=0 has a solution with x>=0 and records a solution x at outputPoint
//else returns false, where b is a given nonnegative column vector, A is an n by m matrix
//and x is a column vector with m entries
{ MemorySaving<Matrix<Rational> > tempA;
  MemorySaving<Vector<Rational> > tempX;
  Matrix<Rational>& tempMatA=
  theGlobalVariables==0 ? tempA.GetElement() : theGlobalVariables->matSimplexAlgorithm1.GetElement();
  Vector<Rational>& matX=
  theGlobalVariables==0 ? tempX.GetElement() : theGlobalVariables->vectConeCondition3.GetElement();

  MemorySaving<Selection> tempSel;
  Selection& BaseVariables =
  theGlobalVariables==0 ? tempSel.GetElement() : theGlobalVariables->selSimplexAlg2.GetElement();
  Rational GlobalGoal;
  GlobalGoal.MakeZero();
  assert (matA.NumRows== matb.NumRows);
  for (int j=0; j<matb.NumRows; j++)
  { GlobalGoal+=(matb.elements[j][0]);
    assert(!matb.elements[j][0].IsNegative());
  }
//  std::cout << "<hr>Starting matrix A: " << matA.ToString();
//  std::cout << "<hr>Starting matrix b: " << matb.ToString();
  if (GlobalGoal.IsEqualToZero())
    return false;
  int NumTrueVariables=matA.NumCols;
  //tempMatb.Assign(matb);
  tempMatA.init(matA.NumRows, NumTrueVariables+matA.NumRows);
  MemorySaving<HashedList<Selection> > tempSelList;
  HashedList<Selection>& VisitedVertices =
  theGlobalVariables==0 ? tempSelList.GetElement() : theGlobalVariables->hashedSelSimplexAlg.GetElement();
  VisitedVertices.Clear();
  BaseVariables.init(tempMatA.NumCols);
  tempMatA.NullifyAll();
  matX.MakeZero(tempMatA.NumCols);
  for (int j=0; j<matA.NumCols; j++)
    for (int i=0; i<matA.NumRows; i++)
      tempMatA.elements[i][j].Assign(matA.elements[i][j]);
  for (int j=0; j<matA.NumRows; j++)
  { tempMatA.elements[j][j+NumTrueVariables].MakeOne();
    matX[j+NumTrueVariables]=(matb.elements[j][0]);
    BaseVariables.AddSelectionAppendNewIndex(j+NumTrueVariables);
  }
  Rational PotentialChangeGradient;
  Rational ChangeGradient; //Change, PotentialChange;
  int EnteringVariable=0;
  bool WeHaveNotEnteredACycle=true;
//  int ProblemCounter=0;
  while (EnteringVariable!=-1 && WeHaveNotEnteredACycle && GlobalGoal.IsPositive())
  {//  ProblemCounter++;
  //  if (ProblemCounter==8)
    //{ BaseVariables.ComputeDebugString();
    //}
    //tempMatA.ComputeDebugString(); matX.ComputeDebugString();
    EnteringVariable=-1; ChangeGradient.MakeZero();
    for (int i=0; i<tempMatA.NumCols; i++)
      if (!BaseVariables.selected[i])
      { Rational PotentialChangeGradient; bool hasAPotentialLeavingVariable;
        Matrix<Rational> ::ComputePotentialChangeGradient
        (tempMatA, BaseVariables, NumTrueVariables, i, PotentialChangeGradient, hasAPotentialLeavingVariable);
        if (PotentialChangeGradient.IsGreaterThanOrEqualTo(ChangeGradient) && hasAPotentialLeavingVariable)
        { EnteringVariable= i;
          ChangeGradient.Assign(PotentialChangeGradient);
        }
      }
    if (EnteringVariable!=-1)
    { int LeavingVariableRow;  Rational MaxMovement;
      Matrix<Rational>::GetMaxMovementAndLeavingVariableRow
      (MaxMovement, LeavingVariableRow, EnteringVariable, NumTrueVariables, tempMatA, matX, BaseVariables);
      Rational tempRat, tempTotalChange;
      assert(!tempMatA.elements[LeavingVariableRow][EnteringVariable].IsEqualToZero());
      tempRat.Assign(tempMatA.elements[LeavingVariableRow][EnteringVariable]);
      tempRat.Invert();
  //    if (BaseVariables.elements[LeavingVariableRow]==34)
  //      tempMatA.ComputeDebugString();
      for (int i=0; i<tempMatA.NumRows; i++)
        assert(tempMatA.elements[i][BaseVariables.elements[i]].IsEqualTo(1));
      tempMatA.RowTimesScalar(LeavingVariableRow, tempRat);
      //if (BaseVariables.elements[LeavingVariableRow]==34)
      //  tempMatA.ComputeDebugString();
      tempTotalChange.Assign(MaxMovement);
      tempTotalChange.MultiplyBy(ChangeGradient);
      matX[EnteringVariable]+=(MaxMovement);
      if (!tempTotalChange.IsEqualToZero())
      { VisitedVertices.Clear();
        GlobalGoal.Subtract(tempTotalChange);
      } else
      { //BaseVariables.ComputeDebugString();
        int tempI= VisitedVertices.GetIndex(BaseVariables);
        if (tempI==-1)
          VisitedVertices.AddOnTop(BaseVariables);
        else
          WeHaveNotEnteredACycle=false;
      }
      //if (BaseVariables.elements[LeavingVariableRow]==34)
      //  tempMatA.ComputeDebugString();
      for (int i=0; i<tempMatA.NumRows; i++)
      { if (!tempMatA.elements[i][EnteringVariable].IsEqualToZero()&& i!=LeavingVariableRow)
        { tempRat.Assign(tempMatA.elements[i][EnteringVariable]);
          tempRat.MultiplyBy(MaxMovement);
          matX[BaseVariables.elements[i]]-=tempRat;
          tempRat.Assign(tempMatA.elements[i][EnteringVariable]);
          tempRat.Minus();
          tempMatA.AddTwoRows(LeavingVariableRow, i, 0, tempRat);
        }
        if (i==LeavingVariableRow)
          matX[BaseVariables.elements[i]]=0;
        //tempMatA.ComputeDebugString();
        //matX.ComputeDebugString();
      }
      assert(matX[BaseVariables.elements[LeavingVariableRow]].IsEqualToZero());
      BaseVariables.selected[BaseVariables.elements[LeavingVariableRow]]=false;
      BaseVariables.elements[LeavingVariableRow]= EnteringVariable;
      BaseVariables.selected[EnteringVariable]= true;
      //BaseVariables.ComputeDebugString();
      for (int i=0; i<tempMatA.NumRows; i++)
        assert(tempMatA.elements[i][BaseVariables.elements[i]].IsEqualTo(1));
    }
//    if (::Matrix<Rational> ::flagAnErrorHasOccurredTimeToPanic)
//    { Matrix<Rational>  tempMat;
//      tempMat.Assign(matX);
//      tempMat.ComputeDebugString();
//      tempDebugMat.ComputeDebugString();
//      tempMat.MultiplyOnTheLeft(tempDebugMat);
//      tempMat.ComputeDebugString();
//      assert(tempMat.IsEqualTo(matb));
//    }
  }
//  std::string tempS;
//  std::stringstream out;
//  for (int i=0; i<BaseVariables.CardinalitySelection; i++)
//  { int tempI=BaseVariables.elements[i];
//    matX.elements[tempI][0].ToString(tempS);
//    out << tempS <<"(";
//    if (tempI<matA.NumCols)
//    {  for (int j=0; j<matA.NumRows; j++)
//      { matA.elements[j][tempI].ToString(tempS);
//        out << tempS;
//        if (j!=matA.NumRows-1)
//          out <<", ";
//      }
//    } else
//      out<<"dummy column " << i <<" ";
//    out <<")";
//    if (i!=BaseVariables.CardinalitySelection-1)
//      out <<"+";
//  }
//  tempS=out.str();
  for(int i=NumTrueVariables; i<matX.size; i++)
    if (matX[i].IsPositive())
      return false;
  if (outputSolution!=0)
  { outputSolution->SetSize(NumTrueVariables);
    for (int i=0; i<NumTrueVariables; i++)
      (*outputSolution)[i]=matX[i];
  }
  return true;
}

template <class CoefficientType>
bool Vectors<CoefficientType>::ConesIntersect
(List<Vector<Rational> >& StrictCone, List<Vector<Rational> >& NonStrictCone,
 Vector<Rational>* outputLinearCombo, Vector<Rational>* outputSplittingNormal,
 GlobalVariables* theGlobalVariables)
{ MemorySaving<Matrix<Rational> > tempA, tempB;
  Matrix<Rational>& matA=
  theGlobalVariables==0 ? tempA.GetElement() : theGlobalVariables->matConeCondition1.GetElement();
  Matrix<Rational>& matb=
  theGlobalVariables==0 ? tempB.GetElement() : theGlobalVariables->matConeCondition2.GetElement();
  if (StrictCone.size==0)
    return false;
  int theDimension= StrictCone[0].size;
  int numCols= StrictCone.size + NonStrictCone.size;
  matA.init((int)theDimension+1, (int)numCols);
  matb.init((int)theDimension+1, 1);
  matb.NullifyAll(); matb.elements[theDimension][0].MakeOne();
  for (int i=0; i<StrictCone.size; i++)
  { for (int k=0; k<theDimension; k++)
      matA.elements[k][i].Assign(StrictCone[i][k]);
    matA.elements[theDimension][i].MakeOne();
  }
  for (int i=0; i<NonStrictCone.size; i++)
  { int currentCol=i+StrictCone.size;
    for (int k=0; k<theDimension; k++)
    { matA.elements[k][currentCol].Assign(NonStrictCone[i][k]);
      matA.elements[k][currentCol].Minus();
    }
    matA.elements[theDimension][currentCol].MakeZero();
  }
  //matA.ComputeDebugString();
  //matb.ComputeDebugString();
  //matX.ComputeDebugString();
  if (!Matrix<Rational>::SystemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution
      (matA, matb, outputLinearCombo, theGlobalVariables))
  { if (outputSplittingNormal!=0)
    { bool tempBool=Vectors<CoefficientType>::GetNormalSeparatingCones
      (StrictCone, NonStrictCone, *outputSplittingNormal, theGlobalVariables);
      if (!tempBool)
      { std::cout << "This is an algorithmic/mathematical (hence also programming) error: "
        << "I get that two cones do not intersect, yet there exists no plane separating them. "
        << "Something is wrong with the implementation of the simplex algorithm. "
        << "The input which manifested the problem was: <br>StrictCone: <br>" << StrictCone.ToString()
        << "<br>Non-strict cone: <br>" << NonStrictCone.ToString()
        << "<br>" << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }
    }
    return false;
  }
  if (outputLinearCombo!=0)
    for (int i=StrictCone.size; i<outputLinearCombo->size; i++)
      (*outputLinearCombo)[i]*=-1;
  return true;
}

template <class CoefficientType>
void SemisimpleLieAlgebra::GetCommonCentralizer
(const List<ElementSemisimpleLieAlgebra<CoefficientType> >& inputElementsToCentralize,
 List<ElementSemisimpleLieAlgebra<CoefficientType> >& outputCentralizingElements)
{ Matrix<Rational> tempAd, commonAd;
  for (int i=0; i<inputElementsToCentralize.size; i++)
  { this->GetAd(tempAd, inputElementsToCentralize[i]);
    //tempAd.Transpose();
    commonAd.AppendMatrixToTheBottom(tempAd);
  }
  Vectors<Rational> outputV;
  commonAd.GetZeroEigenSpace(outputV);
//  std::cout << "<br>Common ad: " << commonAd.ToString();
//  std::cout << "<br>Eigenvectors: " << outputV.ToString();
  outputCentralizingElements.SetSize(outputV.size);
  for (int i=0; i<outputV.size; i++)
  { ElementSemisimpleLieAlgebra<Rational>& currentElt=outputCentralizingElements[i];
    currentElt.AssignVectorNegRootSpacesCartanPosRootSpaces(outputV[i], *this);
  }
}

template <class CoefficientType>
void SemisimpleLieAlgebra::GetAd
(Matrix<CoefficientType>& output, ElementSemisimpleLieAlgebra<CoefficientType>& e)
{ int NumGenerators=this->GetNumGenerators();
  output.init(NumGenerators, NumGenerators);
  output.NullifyAll();
  ElementSemisimpleLieAlgebra<CoefficientType> theGen, theResult;
  for (int i=0; i<NumGenerators; i++)
  { theGen.MakeGenerator(i, *this);
    this->LieBracket(e, theGen, theResult);
    for (int j=0; j<theResult.size; j++)
      output(theResult[j].theGeneratorIndex, i)=theResult.theCoeffs[j];
  }
}

bool CommandList::innerGCDOrLCM
(CommandList& theCommands, const Expression& input, Expression& output, bool doGCD)
{ MacroRegisterFunctionWithName("CommandList::fGCD");
  Vector<Polynomial<Rational> > thePolys;
  Expression theContext(theCommands);
  std::cout << "<br>Time elapsed before calling innerGCDOrLCM: "
  << theCommands.theGlobalVariableS->GetElapsedSeconds() << " seconds.";
  std::cout << "<br>Input lispified: " << input.Lispify();
  if (!theCommands.GetVectorFromFunctionArguments(input, thePolys, &theContext, 2, Serialization::innerPolynomial))
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

bool CommandList::GetListPolysVariableLabelsInLex
(const Expression& input, Vector<Polynomial<Rational> >& output, Expression& outputContext)
{ Expression theContextStart(*this);
  if (!this->GetVectorFromFunctionArguments
      (input, output, &theContextStart, 0, Serialization::innerPolynomial))
    return false;
  if (output.size<2)
    return false;
  int numVars=theContextStart.ContextGetNumContextVariables();
  HashedList<Expression> theVars;
  theVars.SetExpectedSize(numVars);
  for (int i=0; i<numVars; i++)
    theVars.AddOnTop(theContextStart.ContextGetContextVariable(i));
  theVars.QuickSortAscending();
//  std::cout << "<hr>the vars: " << theVars.ToString();
  PolynomialSubstitution<Rational> theSub;
  theSub.SetSize(numVars);
  for (int i=0; i<theSub.size; i++)
    theSub[i].MakeMonomiaL(theVars.GetIndex(theContextStart.ContextGetContextVariable(i)), 1, 1, numVars);
  outputContext.MakeEmptyContext(*this);
  Expression PolyVarsE, tempE;
  PolyVarsE.reset(*this);
  PolyVarsE.children.ReservE(numVars+1);
  tempE.MakeAtom(this->opPolynomialVariables(), *this);
  PolyVarsE.AddChildOnTop(tempE);

  for (int i=0; i<numVars; i++)
  { PolyVarsE.AddChildOnTop(theVars[i]);
    Polynomial<Rational>& currentP=output[i];
    currentP.Substitution(theSub);
  }
  return outputContext.AddChildOnTop(PolyVarsE);
}

bool CommandList::innerPolynomialDivisionRemainder
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::fPolynomialDivisionQuotientRemainder");
  Expression theContext;
  Vector<Polynomial<Rational> > thePolys;
  if (!theCommands.GetListPolysVariableLabelsInLex(input, thePolys, theContext))
    return output.SetError("Failed to extract list of polynomials. ", theCommands);
  GroebnerBasisComputation<Rational> theGB;
  theGB.theBasiS.SetSize(thePolys.size-1);
  for (int i=1; i<thePolys.size; i++)
  { if (thePolys[i].IsEqualToZero())
      return output.SetError("Division by zero.", theCommands);
    theGB.theBasiS[i-1]=thePolys[i];
  }
//  std::cout << "<hr>The polys: " << thePolys.ToString() << "<br>The gb basis: "
//  << theGB.theBasiS.ToString() << "<hr>";
  Polynomial<Rational> outputRemainder;
  theGB.initForDivisionAlone(theGB.theBasiS, theCommands.theGlobalVariableS);
  theGB.RemainderDivisionWithRespectToBasis
  (thePolys[0], &outputRemainder, theCommands.theGlobalVariableS, -1);
  return output.AssignValueWithContext(outputRemainder, theContext, theCommands);
}

bool CommandList::innerPolynomialDivisionVerboseGrLex
(CommandList& theCommands, const Expression& input, Expression& output)
{ return theCommands.innerPolynomialDivisionVerbose
  (theCommands, input, output, MonomialP::LeftGreaterThanTotalDegThenLexicographicLastVariableStrongest);
}

bool CommandList::innerPolynomialDivisionVerboseGrLexRev
(CommandList& theCommands, const Expression& input, Expression& output)
{ return theCommands.innerPolynomialDivisionVerbose
  (theCommands, input, output, MonomialP::LeftGreaterThanTotalDegThenLexicographicLastVariableWeakest);
}

bool CommandList::innerPolynomialDivisionVerboseLex
(CommandList& theCommands, const Expression& input, Expression& output)
{ return theCommands.innerPolynomialDivisionVerbose
  (theCommands, input, output, MonomialP::LeftGreaterThanLexicographicLastVariableStrongest);
}

bool CommandList::innerPolynomialDivisionVerboseLexRev
(CommandList& theCommands, const Expression& input, Expression& output)
{ return theCommands.innerPolynomialDivisionVerbose
  (theCommands, input, output, MonomialP::LeftGreaterThanLexicographicLastVariableWeakest);
}

bool CommandList::innerPolynomialDivisionVerbose
(CommandList& theCommands, const Expression& input, Expression& output,
 List<MonomialP>::OrderLeftGreaterThanRight theMonOrder)
{ MacroRegisterFunctionWithName("CommandList::innerPolynomialDivisionVerbose");
  Expression theContext;
  Vector<Polynomial<Rational> > thePolys;
  if (!theCommands.GetListPolysVariableLabelsInLex(input, thePolys, theContext))
    return output.SetError("Failed to extract list of polynomials. ", theCommands);
  GroebnerBasisComputation<Rational> theGB;
  theGB.flagDoLogDivision=true;
  theGB.theBasiS.SetSize(thePolys.size-1);
  for (int i=1; i<thePolys.size; i++)
  { if (thePolys[i].IsEqualToZero())
      return output.SetError("Division by zero.", theCommands);
    theGB.theBasiS[i-1]=thePolys[i];
  }
//  Polynomial<Rational> outputRemainder;
  theGB.initForDivisionAlone(theGB.theBasiS, theCommands.theGlobalVariableS);
  theGB.theMonOrdeR= theMonOrder;
  theGB.RemainderDivisionWithRespectToBasis
  (thePolys[0], &theGB.remainderDivision, theCommands.theGlobalVariableS, -1);
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
  return output.AssignValue(theGB.GetDivisionString(&theFormat), theCommands);
}

bool CommandList::innerPrintSSsubalgebras
(CommandList& theCommands, const Expression& input, Expression& output, bool forceRecompute)
{ //bool showIndicator=true;
  MacroRegisterFunctionWithName("CommandList::innerSSsubalgebras");
  std::stringstream out;
  SemisimpleLieAlgebra* ownerSSPointer;
  bool isAlreadySubalgebrasObject=input.IsOfType<SemisimpleSubalgebras>();
  if (!isAlreadySubalgebrasObject)
  { std::string errorString;
    if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
        (Serialization::innerSSLieAlgebra, input, ownerSSPointer, &errorString))
      return output.SetError(errorString, theCommands);
    if (ownerSSPointer->GetRank()>4)
    { out << "<b>This code is completely experimental and has been set to run up to rank 4."
      << " As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
      return output.AssignValue(out.str(), theCommands);
    } else
      out << "<b>This code is completely experimental. Use the following printouts on "
      << "your own risk</b>";
  } else
    ownerSSPointer=input.GetValuE<SemisimpleSubalgebras>().owneR;
  SemisimpleLieAlgebra& ownerSS=*ownerSSPointer;
  std::string physicalFolder, displayFolder;
  FormatExpressions theFormat;
  theCommands.GetOutputFolders(ownerSS.theWeyl.theDynkinType, physicalFolder, displayFolder, theFormat);
  std::string theTitlePageFileNameNoPath=
  "SemisimpleSubalgebras_" + ownerSS.theWeyl.theDynkinType.ToString() + ".html";
  std::string theTitlePageFileName= physicalFolder+theTitlePageFileNameNoPath;
  out << "<br>Output file: <a href= \"" << displayFolder
  << theTitlePageFileNameNoPath << "\"> " << theTitlePageFileNameNoPath << "</a>";
  out << "<script> var reservedCountDownToRefresh = 5; "
  << "setInterval(function(){document.getElementById('reservedCountDownToRefresh').innerHTML "
  << "= --reservedCountDownToRefresh;}, 1000); </script>";
  out << "<b>... Redirecting to output file in <span style=\"font-size:36pt;\"><span id=\"reservedCountDownToRefresh\">5</span></span> "
  << "seconds...  </b>"
  << "<meta http-equiv=\"refresh\" content=\"5; url="
  << displayFolder << theTitlePageFileNameNoPath
  << "\">"
  ;
  if (!CGI::FileExists(theTitlePageFileName)|| forceRecompute)
  { SemisimpleSubalgebras tempSSsas(ownerSS);
    SemisimpleSubalgebras& theSSsubalgebras= isAlreadySubalgebrasObject  ?
    input.GetValuENonConstUseWithCaution<SemisimpleSubalgebras>() :
    theCommands.theObjectContainer.theSSsubalgebras
    [theCommands.theObjectContainer.theSSsubalgebras.AddNoRepetitionOrReturnIndexFirst(tempSSsas)]
    ;
    double startTime=theCommands.theGlobalVariableS->GetElapsedSeconds();
    if (!isAlreadySubalgebrasObject)
      theSSsubalgebras.FindTheSSSubalgebras(ownerSS, theCommands.theGlobalVariableS);
    theSSsubalgebras.timeComputationStartInSeconds
    =theCommands.theGlobalVariableS->GetElapsedSeconds()-startTime;
    theSSsubalgebras.numAdditions=Rational::TotalSmallAdditions+Rational::TotalLargeAdditions;
    theSSsubalgebras.numMultiplications=Rational::TotalSmallMultiplications+
    Rational::TotalLargeMultiplications;

    std::fstream theFile;
    theCommands.theGlobalVariableS->System("mkdir " +physicalFolder);
    CGI::OpenFileCreateIfNotPresent(theFile, theTitlePageFileName, false, true, false);
    theFormat.flagUseHTML=true;
    theFormat.flagUseHtmlAndStoreToHD=true;
    theFormat.flagUseLatex=true;
    theFile << "<html>" << "<script src=\"" << theCommands.DisplayPathServerBase
    << "jsmath/easy/load.js\"></script> " << "<body>"
    << theSSsubalgebras.ToString(&theFormat) << "</body></html>";
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CommandList::innerSSsubalgebras
(CommandList& theCommands, const Expression& input, Expression& output)
{ //bool showIndicator=true;
  MacroRegisterFunctionWithName("CommandList::innerSSsubalgebras");
  std::string errorString;
  SemisimpleLieAlgebra* ownerSSPointer;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerSSLieAlgebra, input, ownerSSPointer, &errorString))
    return output.SetError(errorString, theCommands);
  SemisimpleLieAlgebra& ownerSS=*ownerSSPointer;
  std::stringstream out;
  if (ownerSS.GetRank()>4)
  { out << "<b>This code is completely experimental and has been set to run up to rank 4."
    << " As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
    return output.AssignValue(out.str(), theCommands);
  } else
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
      (Serialization::innerSSLieAlgebra, EsmallSA, theSmallSapointer, &errorString))
    return output.SetError(errorString, theCommands);
  SemisimpleLieAlgebra* thelargeSapointer;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerSSLieAlgebra, ElargeSA, thelargeSapointer, &errorString))
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

bool MathRoutines::IsPrime(int theInt)
{ if (theInt<=1)
    return false;
  for (int i=2; i*i<=theInt; i+=2)
    if (theInt% i==0)
      return false;
  return true;
}

bool CommandList::innerAdCommonEigenSpaces
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerAdCommonEigenSpaces");
  if (input.children.size<3)
    return output.SetError
    ("Function ad common eigenspaces needs at least 2 arguments \
      - type and at least one element of UE.", theCommands);
  SemisimpleLieAlgebra* ownerSS;
  std::string errorString;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerSSLieAlgebra, input[1], ownerSS, &errorString))
    return output.SetError(errorString, theCommands);
  List<ElementSemisimpleLieAlgebra<Rational> > theOperators, outputElts;
  theOperators.ReservE(input.children.size-2);
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  for (int i=2; i<input.children.size; i++)
  { if (!Serialization::innerLoadElementSemisimpleLieAlgebraRationalCoeffs
        (theCommands, input[i], tempElt, *ownerSS))
      return output.SetError("Failed to extract element of UE. ", theCommands);
    theOperators.AddOnTop(tempElt);
  }
  ownerSS->GetCommonCentralizer(theOperators, outputElts);
//  std::cout << "<br>The elts: " <<  theOperators.ToString();
//  std::cout << "<br> The common ad: " << commonAd.ToString();
  std::stringstream out;
  out << "<br>EigenSpace basis (" << outputElts.size << " elements total): ";
  for (int i=0; i<outputElts.size; i++)
  { ElementSemisimpleLieAlgebra<Rational>& currentElt=outputElts[i];
    out << "<br>" << currentElt.ToString();
  }
  output.AssignValue(out.str(), theCommands);
  return true;
}

bool CommandList::innerGroebner
(CommandList& theCommands, const Expression& input, Expression& output, bool useGr,
   bool useRevLex, bool useModZp)
{ MacroRegisterFunctionWithName("CommandList::innerGroebner");
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
  output.reset(theCommands);
  for (int i=1; i<input.children.size; i++)
    output.children.AddOnTop(input.children[i]);
  int theMod;
  if (useModZp)
  { if (!output[1].IsSmallInteger(&theMod))
      return output.SetError("Error: failed to extract modulo from the second argument. ", theCommands);
    if (!MathRoutines::IsPrime(theMod))
      return output.SetError("Error: modulo not prime. ", theCommands);
  }
  if (!theCommands.GetVectorFromFunctionArguments<Polynomial<Rational> >
      (output, inputVector, &theContext, -1, Serialization::innerPolynomial))
    return output.SetError("Failed to extract polynomial expressions", theCommands);
  //theCommands.GetVector<Polynomial<Rational> >
  //(output, inputVector, &theContext, -1, Serialization::innerPolynomial);
  for (int i=0; i<inputVector.size; i++)
    inputVector[i].ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
  theContext.ContextGetFormatExpressions(theCommands.theGlobalVariableS->theDefaultFormat);
  if (useModZp)
  { ElementZmodP tempElt;
    tempElt.MakeMOne(theMod);
    inputVectorZmodP.SetSize(inputVector.size);
    for (int i=0; i<inputVector.size; i++)
    { inputVectorZmodP[i].MakeZero();
      for (int j=0; j<inputVector[i].size; j++)
      { tempElt=inputVector[i].theCoeffs[j];
        inputVectorZmodP[i].AddMonomial(inputVector[i][j], tempElt);
      }
    }
    GroebnerBasisComputation<ElementZmodP> theGroebnerComputationZmodP;
  }
//  int theNumVars=theContext.VariableImages.size;
  outputGroebner=inputVector;
  outputGroebner2=inputVector;
//  std::cout << outputGroebner.ToString(&theFormat);

  GroebnerBasisComputation<Rational> theGroebnerComputation;
  if (useGr)
  { if (!useRevLex)
    { theGroebnerComputation.theMonOrdeR=MonomialP::LeftIsGEQTotalDegThenLexicographicLastVariableStrongest;
      theFormat.thePolyMonOrder=MonomialP::LeftGreaterThanTotalDegThenLexicographicLastVariableStrongest;
    } else
    { std::cout << "This is not programmed yet! Crashing to let you know. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
  } else if (!useRevLex)
  { theGroebnerComputation.theMonOrdeR=MonomialP::LeftIsGEQLexicographicLastVariableStrongest;
    theFormat.thePolyMonOrder=MonomialP::LeftGreaterThanLexicographicLastVariableStrongest;
  } else
  { theGroebnerComputation.theMonOrdeR=MonomialP::LeftIsGEQLexicographicLastVariableWeakest;
    theFormat.thePolyMonOrder=MonomialP::LeftGreaterThanLexicographicLastVariableWeakest;
  }
  theGroebnerComputation.MaxNumComputations=upperBoundComputations;
  bool success=
  theGroebnerComputation.TransformToReducedGroebnerBasis
  (outputGroebner, theCommands.theGlobalVariableS);
  std::stringstream out;
  out << "Letter/expression ordrer: ";
  for (int i=0; i<theContext.ContextGetNumContextVariables(); i++)
  { out << theContext.ContextGetContextVariable(i).ToString();
    if (i!=theContext.ContextGetNumContextVariables()-1)
      out << (useRevLex ? "&gt;": "&lt;");
  }
  out << "<br>Starting basis (" << inputVector.size  << " elements): ";
  for(int i=0; i<inputVector.size; i++)
    out << "<br>"
    << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL(inputVector[i].ToString(&theFormat));
  if (success)
  { out << "<br>Minimal Groebner basis with "
    << outputGroebner.size << " elements, computed using algorithm 1, "
    << " using " << theGroebnerComputation.NumberOfComputations << " polynomial operations. ";
    for(int i=0; i<outputGroebner.size; i++)
      out << "<br> "
      << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL(outputGroebner[i].ToString(&theFormat));
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
      { theCommands.Comments << "<hr>I have been instructed not to compute determinants of rational matrices larger than "
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
  { theCommands.Comments << "<hr>I have been instructed to only compute determinants of matrices whose entries are "
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
  if (input.IsOfType<Matrix<Rational> >())
  { output=input;
    return true;
  }
  if (!theCommands.GetMatriXFromArguments(input, outputMat, 0, -1, 0))
  { theCommands.Comments << "<br>Failed to get matrix of rationals. ";
    return false;
  }
  return output.AssignValue<Matrix<Rational> >(outputMat, theCommands);
}

bool CommandList::innerMatrixRationalFunction
  (CommandList& theCommands, const Expression& input, Expression& output)
{ Matrix<RationalFunctionOld> outputMat;
  Expression ContextE;
  if (!theCommands.GetMatriXFromArguments
      (input, outputMat, &ContextE, -1, CommandList::innerRationalFunction))
  { theCommands.Comments << "<hr>Failed to get matrix of rational functions. ";
    return false;
  }
  std::cout << "<hr>And the context is: " << ContextE.ToString();
  return output.AssignValueWithContext(outputMat, ContextE, theCommands);
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
{ this->thePlotElements.AddListOnTop(other.thePlotElements);
  this->thePlotElementsWithHtml.AddListOnTop(other.thePlotElementsWithHtml);
}

std::string CalculusFunctionPlot::GetPlotStringFromFunctionStringAndRanges
(bool useHtml, const std::string& functionStringPostfixNotation,
 const std::string& functionStringCalculatorFormat, const Rational& lowerBound,
 const Rational& upperBound)
{ std::stringstream out;
  out << "\n\n%Function formula: " << functionStringCalculatorFormat << "\n\n";
  if (useHtml)
    out << "<br>";
  out << "\\rput(1,3){$y=" << functionStringCalculatorFormat << "$}\n\n";
  if (useHtml)
    out << "<br>\n";
  out << "\\psplot[linecolor=red, plotpoints=1000]{"
  << lowerBound.DoubleValue() << "}{"
  << upperBound.DoubleValue() << "}{";
  out << functionStringPostfixNotation << "}";
  return out.str();
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
  resultStream << " \\psaxes[ticks=none, labels=none]{<->}(0,0)(-4.5,-4.5)(4.5,4.5)\\tiny";
  if (useHtml)
    resultStream << "<br>";
  for (int i=0; i<this->thePlotElements.size; i++)
    if (useHtml)
      resultStream << this->thePlotElementsWithHtml[i];
    else
      resultStream << this->thePlotElements[i];
  if (useHtml)
    resultStream << "<br>";
  resultStream << "\\end{pspicture}\n\n";
  if (useHtml)
    resultStream << "<br>";
  resultStream << "\\end{document}";
  return resultStream.str();
}

void Expression::Substitute(const Expression& toBeSubbed, Expression& toBeSubbedWith)
{ if(this->IsBuiltInType())
    return;
  Expression tempE;
  for (int i=0; i<this->children.size; i++)
    if (toBeSubbed==(*this)[i])
      this->SetChilD(i, toBeSubbedWith);
    else
    { tempE=(*this)[i];
      tempE.Substitute(toBeSubbed, toBeSubbedWith);
      if (!(tempE==(*this)[i]))
        this->SetChilD(i, tempE);
    }
}

bool CommandList::innerPlot2DWithBars
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerPlot2DWithBars");
  //std::cout << input.ToString();
  if (input.children.size<5)
    return output.SetError
    ("Plotting coordinates takes three or more arguments: function, lower and upper bound. ", theCommands);
  bool tempB=theCommands.innerPlot2D(theCommands, input, output);
  if (!tempB || !output.IsOfType<CalculusFunctionPlot>())
  { theCommands.Comments << "<hr>Failed to a plot from " << input.ToString()
    << ", not proceding with bar plot.";
    return false;
  }
  const Expression& lowerE=input[2];
  const Expression& upperE=input[3];
  const Expression& deltaE=input[4];
  Rational theDeltaNoSign, theDeltaWithSign;
  if (!deltaE.IsOfType<Rational>(&theDeltaWithSign))
    return false;
  theDeltaNoSign=theDeltaWithSign;
  if (theDeltaNoSign<0)
    theDeltaNoSign*=-1;
  if (theDeltaNoSign==0)
    theDeltaNoSign=1;
  Rational upperBound, lowerBound;
  if (!lowerE.IsOfType(&upperBound) || !upperE.IsOfType(&lowerBound))
    return
    output.SetError
    ("Failed to convert upper and lower bounds of drawing function to rational numbers.", theCommands);
  if (upperBound<lowerBound)
    MathRoutines::swap(upperBound, lowerBound);
  Expression tempE1, tempE2, tempX, tempResult;
  tempX.MakeAtom(theCommands.AddOperationNoRepetitionOrReturnIndexFirst("x"), theCommands);
  List<double> xValues;
  List<double> fValues;
  List<Rational> rValues;
  for (Rational i=lowerBound; i<=upperBound; i+=theDeltaNoSign)
  { if (theDeltaWithSign<0 && i==lowerBound)
      continue;
    rValues.AddOnTop(i);
    if (theDeltaWithSign>0 && i==upperBound)
      continue;
    tempE1.AssignValue(i, theCommands);
    tempE2=input[1];
    tempE2.Substitute(tempX, tempE1);
//    std::cout << "<br>substitution result:" << tempE2.ToString();
    BoundVariablesSubstitution tempSub;
    bool tempB;
    if (!theCommands.EvaluateExpression(tempE2, tempResult, tempSub, tempB))
      return false;
//    std::cout << "and after evaluation: " << tempResult.ToString();
    Rational finalResultRat;
    double finalREsultDouble;
    if (!tempResult.IsOfType<Rational>(&finalResultRat))
    { if (!tempResult.IsOfType<double>(&finalREsultDouble))
      { theCommands.Comments << "<hr>Failed to evaluate your function at point " << i << ", instead "
        << "I evaluated to " << tempResult.ToString();
        return false;
      }
    } else finalREsultDouble=finalResultRat.DoubleValue();
    xValues.AddOnTop(i.DoubleValue());
    fValues.AddOnTop(finalREsultDouble);
  }
  std::stringstream outTex, outHtml;
  for (int k=0; k<2; k++)
    for (int i=0; i<xValues.size; i++)
    { bool includePsLine=false;
      bool useNegativePattern=false;
      if (i==0)
      { includePsLine=true;
        useNegativePattern=(fValues[i]<0);
      }
      if (i>0)
        if (fValues[i]*fValues[i-1]<=0)
        { includePsLine=true;
          useNegativePattern=!(fValues[i]>fValues[i-1]);
        }
      if (includePsLine)
      { if (k==0 && useNegativePattern)
        { outTex << "\\psline*[linecolor=orange, linewidth=0.1pt]";
          outHtml << "<br>\\psline*[linecolor=orange, linewidth=0.1pt]";
        }
        if (k==0 && !useNegativePattern)
        { outTex << "\\psline*[linecolor=cyan, linewidth=0.1pt]";
          outHtml << "<br>\\psline*[linecolor=cyan, linewidth=0.1pt]";
        }
        if (k>0 && useNegativePattern)
        { outTex << "\\psline[linecolor=brown, linewidth=0.1pt]";
          outHtml << "<br>\\psline[linecolor=brown, linewidth=0.1pt]";
        }
        if (k>0 && !useNegativePattern)
        { outTex << "\\psline[linecolor=blue, linewidth=0.1pt]";
          outHtml << "<br>\\psline[linecolor=blue, linewidth=0.1pt]";
        }
      }
      outTex << "(" << MathRoutines::ReducePrecision(xValues[i]) << ", 0)("
      << MathRoutines::ReducePrecision(xValues[i]) << ", "
      << MathRoutines::ReducePrecision(fValues[i]) << ")"
      << "(" << MathRoutines::ReducePrecision(xValues[i]+theDeltaWithSign.DoubleValue())
      << ", " << MathRoutines::ReducePrecision(fValues[i]) << ")("
      << MathRoutines::ReducePrecision(xValues[i]+theDeltaWithSign.DoubleValue()) << ", 0)";
      outHtml << "(" << MathRoutines::ReducePrecision(xValues[i]) << ", 0)("
      << MathRoutines::ReducePrecision(xValues[i]) << ", " << MathRoutines::ReducePrecision(fValues[i])
      << ")"
      << "(" << MathRoutines::ReducePrecision(xValues[i]+theDeltaWithSign.DoubleValue())
      << ", " << MathRoutines::ReducePrecision(fValues[i]) << ")("
      << MathRoutines::ReducePrecision(xValues[i]+theDeltaWithSign.DoubleValue()) << ", 0)";
    }
  outHtml << "<br>";
  for (int i=0; i<rValues.size; i++)
  { std::stringstream tempStream;
    tempStream << "\\rput[t](" << MathRoutines::ReducePrecision(rValues[i].DoubleValue()) << ",-0.03)"
    << "{$";
    if (rValues[i].IsInteger())
      tempStream << rValues[i].ToString();
    else
      tempStream << "\\frac{" << rValues[i].GetNumerator().ToString() << "}"
      << "{" << rValues[i].GetDenominator().ToString() << "}";
    tempStream << "$}";
    outHtml << tempStream.str();
    outTex << tempStream.str();
  }
  outHtml << "<br>";
  CalculusFunctionPlot thePlot;
  thePlot.thePlotElements.AddOnTop(outTex.str());
  thePlot.thePlotElementsWithHtml.AddOnTop(outHtml.str());
  thePlot+=output.GetValuE<CalculusFunctionPlot>();
  return output.AssignValue(thePlot, theCommands);
}

bool CommandList::innerPlot2D
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerPlot2D");
  //std::cout << input.ToString();
  if (input.children.size<4)
    return output.SetError
    ("Plotting coordinates takes at least three arguments: function, lower and upper bound. ", theCommands);
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
  thePlot.thePlotElements.AddOnTop
  (thePlot.GetPlotStringFromFunctionStringAndRanges
  (false, functionE.GetValuE<std::string>(), input[1].ToString(), lowerBound, upperBound));
  thePlot.thePlotElementsWithHtml.AddOnTop
  (thePlot.GetPlotStringFromFunctionStringAndRanges
  (true, functionE.GetValuE<std::string>(), input[1].ToString(), lowerBound, upperBound));
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
  (ownerSSLiealg->theWeyl.GetSimpleCoordinatesFromFundamental(theHW), ownerSSLiealg);
  return output.AssignValue(theElt, theCommands);
}

bool CommandList::innerConesIntersect
  (CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerConesIntersect");
  if (!input.IsListNElements(3))
  { theCommands.Comments << "Function ConesIntersection expects 2 arguments, got " << input.children.size-1
    << " instead. ";
    return false;
  }
  Matrix<Rational> coneNonStrictMatForm;
  Matrix<Rational> coneStrictMatForm;
  Vectors<Rational> coneNonStrictGens;
  Vectors<Rational> coneStrictGens;
  if (!theCommands.GetMatrix(input[1], coneStrictMatForm))
  { theCommands.Comments << "Failed to extract matrix from the first argument, " << input[1].ToString();
    return false;
  }
  if (!theCommands.GetMatrix(input[2], coneNonStrictMatForm))
  { theCommands.Comments << "Failed to extract matrix from the second argument, " << input[2].ToString();
    return false;
  }
  std::stringstream out;
  if (coneNonStrictMatForm.NumCols!=coneStrictMatForm.NumCols)
  { out << "I got as input vectors of different dimensions, first groups had vectors of dimension "
    << coneNonStrictMatForm.NumCols << " and second of dimension " << coneStrictMatForm.NumCols
    << " which is not allowed. ";
    return output.SetError(out.str(), theCommands);
  }
  coneNonStrictMatForm.GetVectorsFromRows(coneNonStrictGens);
  coneStrictMatForm.GetVectorsFromRows(coneStrictGens);
  out << "<br>Input non-strict (i.e., over Z_{&gt;=0}) cone: ";
  for (int i=0; i<coneStrictGens.size; i++)
    out << "<br>v_{" << i+1 << "}:=" << coneStrictGens[i].ToString() << ";";
  out << "<br>Input strict (i.e., over Z_{&gt;0}) cone: ";
  for (int i=0; i<coneNonStrictGens.size; i++)
    out << "<br>v_{" << coneStrictGens.size+ i+1 << "}:=" << coneNonStrictGens[i].ToString() << ";";
  Vector<Rational> outputIntersection, outputSeparatingNormal;
  bool conesDoIntersect=
  coneNonStrictGens.ConesIntersect
  (coneStrictGens, coneNonStrictGens, &outputIntersection, &outputSeparatingNormal, theCommands.theGlobalVariableS);
  if (conesDoIntersect)
  { Vector<Rational> checkVector;
    checkVector.MakeZero(coneStrictMatForm.NumCols);
    for (int i=0; i<coneStrictGens.size; i++)
      checkVector+=coneStrictGens[i]*outputIntersection[i];
    for (int i=0; i<coneNonStrictGens.size; i++)
      checkVector+=coneNonStrictGens[i]*outputIntersection[coneStrictGens.size+i];
    if (!checkVector.IsEqualToZero())
    { std::cout << "<br>This is a programming error: the output linear combination"
      << outputIntersection.ToString()
      << " corresponds to the cone intersection " << checkVector.ToString()
      << " and is not equal to zero! Here is the cone output so far: "
      << out.str() << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    out << "<br>Cones intersect, here is one intersection: 0= "
    << outputIntersection.ToStringLetterFormat("v");
  } else
  { out << "<br>Cones have empty intersection.";
    out << "<br> A normal separating the cones is: n:=" << outputSeparatingNormal.ToString()
    << ". Indeed, ";
    for (int i=0; i<coneStrictGens.size; i++)
      out << "<br>\\langle v_{" << i+1 << "}, n\\rangle = "
      << outputSeparatingNormal.ScalarEuclidean(coneStrictGens[i]).ToString();
    for (int i=0; i<coneNonStrictGens.size; i++)
      out << "<br>\\langle v_{" << i+1 + coneStrictGens.size << "}, n\\rangle = "
      << outputSeparatingNormal.ScalarEuclidean(coneNonStrictGens[i]).ToString();
  }
  return output.AssignValue(out.str(), theCommands);
}

template <class CoefficientType>
void GroebnerBasisComputation<CoefficientType>::GetSubFromPartialSolutionSerreLikeSystem
(PolynomialSubstitution<CoefficientType>& outputSub)
{ outputSub.MakeIdSubstitution(this->systemSolution.GetElement().size);
  for (int i=0; i<this->solutionsFound.GetElement().CardinalitySelection; i++)
    outputSub[this->solutionsFound.GetElement().elements[i]]=
    this->systemSolution.GetElement()[this->solutionsFound.GetElement().elements[i]];
}

template <class CoefficientType>
bool GroebnerBasisComputation<CoefficientType>::HasImpliedSubstitutions
 (List<Polynomial<CoefficientType> >& inputSystem, PolynomialSubstitution<CoefficientType>& outputSub)
{ int numVars=this->systemSolution.GetElement().size;
  MonomialP tempM;
  Polynomial<CoefficientType> tempP;
  for (int i=0; i<inputSystem.size; i++)
  { tempP=inputSystem[i];
    for (int j=0; j<numVars; j++)
    { tempM.MakeEi(j, 1, numVars);
      int indexTempM=tempP.GetIndex(tempM);
      if (indexTempM==-1)
        continue;
      CoefficientType tempCF=tempP.theCoeffs[indexTempM];
      tempP.SubtractMonomial(tempM, tempCF);
      bool isGood=true;
      for (int k=0; k<tempP.size; k++)
        if (!(tempP[k](j)==0))
        { isGood=false;
          tempP.AddMonomial(tempM, tempCF);
          break;
        }
      if (!isGood)
        continue;
      outputSub.MakeIdSubstitution(numVars);
      outputSub[j]=tempP;
      tempCF*=-1;
      outputSub[j]/=tempCF;
      CoefficientType theConst;
//      std::cout << "<hr>Output sub is: x_{" << j+1 << "}=" << outputSub[j].ToString();
//      if (outputSub[j].IsAConstant(&theConst))
//        this->SetSerreLikeSolutionIndex(j, theConst);
      //std::cout << "<br>Current solution candidate is: " << this->systemSolution.GetElement().ToString();
      return true;
    }
  }
  return false;
}

template <class CoefficientType>
int GroebnerBasisComputation<CoefficientType>::GetPreferredSerreSystemSubIndex()
{ const MonomialP& theMon=this->theBasiS[0].GetMaxMonomial(this->theMonOrdeR);
  for (int i=0; i<theMon.GetMinNumVars(); i++)
    if (theMon(i)>0)
      return i;
  return -1;
}

template <class CoefficientType>
void GroebnerBasisComputation<CoefficientType>::BackSubstituteIntoSinglePoly
 (Polynomial<CoefficientType>& thePoly, int theIndex,
  PolynomialSubstitution<CoefficientType>& theFinalSub,
  GlobalVariables* theGlobalVariables)
{ Polynomial<CoefficientType> tempP;
  tempP.MakeMonomiaL(theIndex, 1, 1);
  if (thePoly==tempP)
    return;
  //std::cout << "<hr> Back substituting in x_{" << theIndex+1 << "}:=" << thePoly.ToString();
  thePoly.Substitution(theFinalSub);
  bool changed=false;
  for (int i=0; i<thePoly.size; i++)
    for (int j=0; j<thePoly[i].GetMinNumVars(); j++)
      if (thePoly[i](j)!=0)
      { if (!this->solutionsFound.GetElement().selected[j])
          this->SetSerreLikeSolutionIndex(j, 0);
        else
          if (this->systemSolution.GetElement()[j]!=0)
          { std::cout << "This is a programming error: variable index " << j+1
            << " is supposed to be a free parameter, i.e., be set to zero, but "
            << "instead it has a non-zero value. "
            << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
            assert(false);
          }
        theFinalSub[j]=0;
        changed=true;
      }
  if (changed)
    thePoly.Substitution(theFinalSub);
  CoefficientType tempCF;
  if (!thePoly.IsAConstant(&tempCF))
  { std::cout << "\n<br>\nThis is a programming error: after carrying all implied substitutions "
    << " the polynomial is not a constant, rather equals "
    << thePoly.ToString() << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  theFinalSub[theIndex]=tempCF;
  this->SetSerreLikeSolutionIndex(theIndex, tempCF);
}

template <class CoefficientType>
void GroebnerBasisComputation<CoefficientType>::BackSubstituteIntoPolySystem
 (List<PolynomialSubstitution<CoefficientType> >& theImpliedSubs, GlobalVariables* theGlobalVariables)
{ PolynomialSubstitution<CoefficientType> FinalSub;
  this->GetSubFromPartialSolutionSerreLikeSystem(FinalSub);
  for (int i=theImpliedSubs.size-1; i>=0; i--)
    for (int j=0; j<theImpliedSubs[i].size; j++)
      this->BackSubstituteIntoSinglePoly(theImpliedSubs[i][j], j, FinalSub, theGlobalVariables);
}

template <class CoefficientType>
void GroebnerBasisComputation<CoefficientType>::SolveSerreLikeSystemRecursively
 (List<Polynomial<CoefficientType> >& inputSystem, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("GroebnerBasisComputation::SolveSerreLikeSystemRecursively");
  RecursionDepthCounter theCounter(&this->RecursionCounterSerreLikeSystem);
  ProgressReport theReport1(theGlobalVariables);
  ProgressReport theReport2(theGlobalVariables);
  if (theGlobalVariables!=0)
  { std::stringstream out;
    out << "<br>Recursion depth: " << this->RecursionCounterSerreLikeSystem
    << "<br>Solving the system" << inputSystem.ToString();
    theReport1.Report(out.str());
  }
  bool changed=true;
  PolynomialSubstitution<CoefficientType> theSub;
  List<PolynomialSubstitution<CoefficientType> > theImpliedSubs;
  theImpliedSubs.ReservE(inputSystem.size);
  while (changed)
  { this->NumberOfComputations=0;
    bool success=this->TransformToReducedGroebnerBasis(inputSystem, theGlobalVariables);
    if (success)
    { //std::cout << "<hr>System groebner reduced successfully, output:";
      //for (int i=0; i <inputSystem.size; i++)
        //std::cout << "<br>" << CGI::GetHtmlMathSpanPure(inputSystem[i].ToString());
      if (this->IsContradictoryReducedSystem(inputSystem))
      { this->flagSystemProvenToHaveNoSolution=true;
        this->flagSystemSolvedOverBaseField=false;
        this->flagSystemProvenToHaveSolution=false;
        return;
      } else
      { this->flagSystemProvenToHaveSolution=true;
        if (inputSystem.size==0)
        { this->flagSystemProvenToHaveNoSolution=false;
          this->flagSystemSolvedOverBaseField=true;
          this->BackSubstituteIntoPolySystem(theImpliedSubs, theGlobalVariables);
          return;
        }
      }
    }
    //std::cout << "<hr>input system: " << inputSystem.ToString();
    changed=this->HasImpliedSubstitutions(inputSystem, theSub);
    if (changed)
    { theImpliedSubs.AddOnTop(theSub);
      for (int i=0; i<inputSystem.size; i++)
        inputSystem[i].Substitution(theSub);
    }
  }
//  std::cout << "<br>System has no more implied subs. ";
  if (theGlobalVariables!=0)
  { //std::cout << "<hr>The system solution candidate at recursion depth "
    //<< this->RecursionCounterSerreLikeSystem << ": "
    //<< this->ToStringSerreLikeSolution(&theGlobalVariables->theDefaultFormat);
    //std::cout << "<br>The system so far: (";
    //for (int i=0; i<inputSystem.size; i++)
    //{ std::cout << inputSystem[i].ToString(&theGlobalVariables->theDefaultFormat);
    //  if (i!=inputSystem.size-1)
    //    std::cout << ", ";
    //}
    //std::cout << ")";
  }
  List<Polynomial<CoefficientType> > oldSystem=inputSystem;
  GroebnerBasisComputation newComputation;
  newComputation.RecursionCounterSerreLikeSystem=this->RecursionCounterSerreLikeSystem;

  newComputation.MaxNumComputations=this->MaxNumComputations;
  newComputation.systemSolution=this->systemSolution;
  newComputation.solutionsFound=this->solutionsFound;
  newComputation.flagSystemProvenToHaveNoSolution=false;
  newComputation.flagSystemSolvedOverBaseField=false;
  newComputation.flagSystemProvenToHaveSolution=false;

  int theVarIndex=this->GetPreferredSerreSystemSubIndex();
  theSub.MakeIdSubstitution(this->systemSolution.GetElement().size);
  theSub[theVarIndex]=0;
  //std::cout << "<br>Setting x_{" << theVarIndex+1 << "}:=0";
  newComputation.SetSerreLikeSolutionIndex(theVarIndex, 0);
  //std::cout << "<br>Input system before sub first recursive call. " << inputSystem.ToString();
  for (int i=0; i<inputSystem.size; i++)
    inputSystem[i].Substitution(theSub);
  //std::cout << "<br>Input system after sub first recursive call. " << inputSystem.ToString();

  newComputation.SolveSerreLikeSystemRecursively(inputSystem, theGlobalVariables);
  if (newComputation.flagSystemSolvedOverBaseField)
  { *this=newComputation;
    this->BackSubstituteIntoPolySystem(theImpliedSubs, theGlobalVariables);
    return;
  }
  if (newComputation.flagSystemProvenToHaveSolution)
    this->flagSystemProvenToHaveSolution=true;
  inputSystem=oldSystem;

  newComputation.MaxNumComputations=this->MaxNumComputations;
  newComputation.systemSolution=this->systemSolution;
  newComputation.solutionsFound=this->solutionsFound;
  newComputation.flagSystemProvenToHaveNoSolution=false;
  newComputation.flagSystemSolvedOverBaseField=false;
  newComputation.flagSystemProvenToHaveSolution=false;

  theSub.MakeIdSubstitution(this->systemSolution.GetElement().size);
  theSub[theVarIndex]=1;
  //std::cout << "<hr>Setting x_{" << theVarIndex+1 << "}:=1";
  newComputation.SetSerreLikeSolutionIndex(theVarIndex, 1);

  //std::cout << "<hr>Input system before second recursive call. " << inputSystem.ToString();
  //std::cout << "<br>Solution before second recursive call. "
  //<< newComputation.systemSolution.GetElement().ToString();
  for (int i=0; i<inputSystem.size; i++)
    inputSystem[i].Substitution(theSub);
  //std::cout << "<br>Input system after sub second recursive call. " << inputSystem.ToString();
  newComputation.SolveSerreLikeSystemRecursively(inputSystem, theGlobalVariables);
  if (newComputation.flagSystemSolvedOverBaseField)
  { *this=newComputation;
    this->BackSubstituteIntoPolySystem(theImpliedSubs, theGlobalVariables);
    return;
  }
  if (newComputation.flagSystemProvenToHaveSolution)
    this->flagSystemProvenToHaveSolution=true;
  inputSystem=oldSystem;
}

template <class CoefficientType>
std::string GroebnerBasisComputation<CoefficientType>::GetCalculatorInputFromSystem
(const List<Polynomial<CoefficientType> >& inputSystem)
{ std::stringstream out;
  out << "FindOneSolutionSerreLikePolynomialSystem{}(";
  for (int j=0; j<inputSystem.size; j++)
  { out << inputSystem[j].ToString();
    if (j!=inputSystem.size-1)
      out << ", ";
  }
  out << ")";
  return out.str();
}

template <class CoefficientType>
void GroebnerBasisComputation<CoefficientType>::SolveSerreLikeSystem
 (List<Polynomial<CoefficientType> >& inputSystem, GlobalVariables* theGlobalVariables)
{ this->flagSystemProvenToHaveNoSolution=false;
  this->flagSystemSolvedOverBaseField=false;
  this->flagSystemProvenToHaveSolution=false;
  this->RecursionCounterSerreLikeSystem=0;
  this->MaxNumComputations=1000;
  int numVars=0;
//  std::cout << "<hr>" << this->GetCalculatorInputFromSystem(inputSystem) << "<hr>";
  List<Polynomial<CoefficientType> > workingSystem=inputSystem;
  for (int i=0; i<workingSystem.size; i++)
    numVars=MathRoutines::Maximum(numVars, workingSystem[i].GetMinNumVars());
  this->systemSolution.GetElement().initFillInObject(numVars, 0);
  this->solutionsFound.GetElement().init(numVars);
  this->SolveSerreLikeSystemRecursively(workingSystem, theGlobalVariables);
  if (this->flagSystemSolvedOverBaseField)
  { if (this->solutionsFound.GetElement().CardinalitySelection!=
        this->solutionsFound.GetElement().MaxSize)
      for (int i=0; i<this->solutionsFound.GetElement().MaxSize; i++)
        if (!this->solutionsFound.GetElement().selected[i])
          this->SetSerreLikeSolutionIndex(i, 0);
    PolynomialSubstitution<CoefficientType> theSub;
    this->GetSubFromPartialSolutionSerreLikeSystem(theSub);
    workingSystem=inputSystem;
    for (int i=0; i<workingSystem.size; i++)
    { workingSystem[i].Substitution(theSub);
      if (!workingSystem[i].IsEqualToZero())
      { std::cout << "<br>This is a programming error. Function SolveSerreLikeSystem "
        << "reports to have found a solution over the base field, but substituting the solution "
        << " back to the original system does not yield a zero system of equations. More precisely, "
        << "the reported solution was " << this->ToStringSerreLikeSolution()
        << " but substitution in equation " << inputSystem[i].ToString() << " yields "
        << workingSystem[i].ToString() << ". "
        << " Calculator input: "
        << "<br>" << this->GetCalculatorInputFromSystem(inputSystem) << " <br>";
        std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }
    }
  }
}

template <class CoefficientType>
std::string GroebnerBasisComputation<CoefficientType>::ToStringSerreLikeSolution(FormatExpressions* theFormat)
{ std::stringstream out;
  Polynomial<Rational> tempP;
  for (int i=0; i<this->systemSolution.GetElement().size; i++)
    if (this->solutionsFound.GetElement().selected[i])
    { tempP.MakeMonomiaL(i, 1, 1);
      out << " " << tempP.ToString(theFormat) << " := "
      << this->systemSolution.GetElement()[i] << ";";
    }
  return out.str();
}

template <class CoefficientType>
void GroebnerBasisComputation<CoefficientType>::SetSerreLikeSolutionIndex
(int theIndex, const CoefficientType& theConst)
{ this->systemSolution.GetElement()[theIndex]=theConst;
  if (this->solutionsFound.GetElement().selected[theIndex])
  { std::cout << "This a programming error: attempting to set value to a variable "
    << " whose value has already been computed. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  this->solutionsFound.GetElement().AddSelectionAppendNewIndex(theIndex);
}

bool Expression::operator==(const std::string& other)const
{ std::string tempS;
  if (!this->IsOperation(&tempS))
    return false;
  return tempS==other;
}

bool CommandList::innerReverseOrder
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerReverse");
  if (input.IsBuiltInType()||input.IsAtoM())
  { output=input;
    return true;
  }
  if (input[0]=="Reverse")
  { Expression tempE=input;
    tempE.SetChildAtomValue(0, theCommands.opSequence());
    return theCommands.innerReverseOrder(theCommands, tempE, output);
  }
  output.reset(theCommands, input.children.size);
  output.AddChildOnTop(input[0]);
  for (int i=input.children.size-1; i>=1; i--)
  { Expression currentE=input[i];
    Expression reversedCurrentE;
    if (!theCommands.innerReverseOrder(theCommands,  currentE, reversedCurrentE))
      return false;
    output.AddChildOnTop(reversedCurrentE);
  }
  return true;
}

bool CommandList::innerSolveSerreLikeSystem
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerSolveSerreLikeSystem");
  Vector<Polynomial<Rational> > thePolys;
  Expression theContext(theCommands);
  if (!theCommands.GetVectorFromFunctionArguments(input, thePolys, &theContext, 0, Serialization::innerPolynomial))
    return output.SetError("Failed to extract list of polynomials. ", theCommands);
  //int numVars=theContext.GetNumContextVariables();
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
  GroebnerBasisComputation<Rational> theComputation;
  theComputation.MaxNumComputations=1000;
  theCommands.theGlobalVariableS->theDefaultFormat=theFormat;
  std::cout << "<br>The context vars:<br>" << theContext.ToString();
  theComputation.SolveSerreLikeSystem(thePolys, theCommands.theGlobalVariableS);
  std::stringstream out;
  out << "<br>The context vars:<br>" << theContext.ToString();
  out << "<br>The polynomials: " << thePolys.ToString(&theFormat);
  out << "<br>Total number of polynomial computations: " << theComputation.NumberOfComputations;
  if (theComputation.flagSystemProvenToHaveNoSolution)
    out << "<br>The system does have a solution. ";
  else if(theComputation.flagSystemProvenToHaveSolution)
    out << "<br>System proven to have solution.";
  if (!theComputation.flagSystemProvenToHaveNoSolution)
  { if (theComputation.flagSystemSolvedOverBaseField)
      out << "<br>One solution follows. " << theComputation.ToStringSerreLikeSolution(&theFormat);
    else
      out << "However, I was unable to find such a solution: either my heuristics were not good enough, "
      << " or no solution is rational.";
  }
  return output.AssignValue(out.str(), theCommands);
}
