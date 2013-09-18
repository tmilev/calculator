//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader3Calculator0_Interface.h"
#include "vpfHeader2Math1_SemisimpleLieAlgebras.h"
#include "vpfImplementationHeader2Math15_UniversalEnveloping.h"

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

template <class coefficient>
bool Vectors<coefficient>::ConesIntersect
(List<Vector<Rational> >& StrictCone, List<Vector<Rational> >& NonStrictCone,
 Vector<Rational>* outputLinearCombo, Vector<Rational>* outputSplittingNormal,
 GlobalVariables* theGlobalVariables)
{ MemorySaving<Matrix<Rational> > tempA, tempB;
  Matrix<Rational>& matA=
  theGlobalVariables==0 ? tempA.GetElement() : theGlobalVariables->matConeCondition1.GetElement();
  Matrix<Rational>& matb=
  theGlobalVariables==0 ? tempB.GetElement() : theGlobalVariables->matConeCondition2.GetElement();
  if (StrictCone.size==0)
  { if (outputSplittingNormal!=0)
      if (NonStrictCone.size>0)
        outputSplittingNormal->MakeZero(NonStrictCone[0].size);
    return false;
  }
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
    { bool tempBool=Vectors<coefficient>::GetNormalSeparatingCones
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

template <class coefficient>
void SemisimpleLieAlgebra::GetCommonCentralizer
(const List<ElementSemisimpleLieAlgebra<coefficient> >& inputElementsToCentralize,
 List<ElementSemisimpleLieAlgebra<coefficient> >& outputCentralizingElements)
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

template <class coefficient>
void SemisimpleLieAlgebra::GetAd
(Matrix<coefficient>& output, ElementSemisimpleLieAlgebra<coefficient>& e)
{ int NumGenerators=this->GetNumGenerators();
  output.init(NumGenerators, NumGenerators);
  output.NullifyAll();
  ElementSemisimpleLieAlgebra<coefficient> theGen, theResult;
  for (int i=0; i<NumGenerators; i++)
  { theGen.MakeGenerator(i, *this);
    this->LieBracket(e, theGen, theResult);
    for (int j=0; j<theResult.size(); j++)
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
//  std::cout << "<br>Input lispified: " << input.Lispify();
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

bool DynkinSimpleType::HasEasySubalgebras()const
{ if(this->theLetter=='F')
    return true;
  if (this->theLetter=='G')
    return true;
  if (this->theLetter=='A' && (this->theRank==2 || this->theRank==3 || this->theRank==4))
    return true;
  if (this->theLetter=='B' && (this->theRank==2 || this->theRank==3 || this->theRank==4))
    return true;
  if (this->theLetter=='C' && (this->theRank==3 || this->theRank==4))
    return true;
  return false;
}

std::string CommandList::ToStringLinksToCalculatorDirectlyFromHD(const DynkinType& theType, FormatExpressions* theFormat)
{ std::stringstream out;
  std::string theTitlePageFileNameNoPath= "SemisimpleSubalgebras_" + theType.ToString() + ".html";
  out << "<tr><td><a href=\"http://vector-partition.jacobs-university.de/vpf/cgi-bin/calculator?textInput=printSemisimpleLieAlgebra%7B%7D"
  << theType[0].theLetter << "_" << theType[0].theRank << "\">" << theType[0].theLetter << theType[0].theRank << "</a></td>\n ";
  if (theType[0].HasEasySubalgebras())
    out << "<td><a href=\"http://vector-partition.jacobs-university.de/vpf/output/"
    << theType.ToString() << "/" << theTitlePageFileNameNoPath << "\">"
    << theType[0].theLetter << theType[0].theRank << " semisimple subalgebras</a></td>\n ";
  else
    out << "<td>Not available</td>\n";
  out << "<td><a href=\"http://vector-partition.jacobs-university.de/vpf/cgi-bin/calculator?%20textType=Calculator&textDim=1&textInput=printSlTwoSubalgebras%7B%7D%28"
  << theType[0].theLetter << "_" << theType[0].theRank << "%29\">"
  << theType[0].theLetter << theType[0].theRank << " sl(2) triples</a></td>\n";
  out << "<td><a href=\"http://vector-partition.jacobs-university.de/vpf/cgi-bin/calculator?%20textType=Calculator&textDim=1&textInput=printRootSubalgebras%7B%7D%28"
  << theType[0].theLetter << "_" << theType[0].theRank << "%29\">"
  << theType[0].theLetter << theType[0].theRank << " root subalgebras</a></td>\n";
  return out.str();
}

std::string CommandList::ToStringLinksToCalculator(const DynkinType& theType, FormatExpressions* theFormat)
{ std::stringstream out;
  out << "<tr><td><a href=\"http://vector-partition.jacobs-university.de/vpf/cgi-bin/calculator?textInput=printSemisimpleLieAlgebra%7B%7D"
  << theType[0].theLetter << "_" << theType[0].theRank << "\">"
  << theType[0].theLetter << theType[0].theRank << "</a></td>\n ";
  if (theType[0].HasEasySubalgebras())
    out << "<td><a href=\"http://vector-partition.jacobs-university.de/vpf/cgi-bin/calculator?%20textType=Calculator&textDim=1&textInput=experimentalPrintSemisimpleSubalgebras%7B%7D%28"
    << theType[0].theLetter << "_" << theType[0].theRank << "%29\">"
    << theType[0].theLetter << theType[0].theRank << " semisimple subalgebras</a></td>\n ";
  else
    out << "<td>Not available</td>\n";
  out << "<td><a href=\"http://vector-partition.jacobs-university.de/vpf/cgi-bin/calculator?%20textType=Calculator&textDim=1&textInput=printSlTwoSubalgebras%7B%7D%28"
  << theType[0].theLetter << "_" << theType[0].theRank << "%29\">"
  << theType[0].theLetter << theType[0].theRank << " sl(2) triples</a></td>\n";
  out << "<td><a href=\"http://vector-partition.jacobs-university.de/vpf/cgi-bin/calculator?%20textType=Calculator&textDim=1&textInput=printRootSubalgebras%7B%7D%28"
  << theType[0].theLetter << "_" << theType[0].theRank << "%29\">"
  << theType[0].theLetter << theType[0].theRank << " root subalgebras</a></td>\n";
  return out.str();
}

bool CommandList::innerGetLinksToSimpleLieAlgerbas
(CommandList& theCommands, const Expression& input, Expression& output)
{ std::stringstream out, out2;
  out << "\n\n<p>\n<table><tr><td>Structure constants </td><td>Semisimple subalgebras</td> "
  << "<td>sl(2) subalgebras</td><td>root subalgebras</td> </tr>\n";
  out2 << "\n\n<p>\n\n<table><tr><td>Structure constants </td><td>Semisimple subalgebras</td> "
  << "<td>sl(2) subalgebras</td><td>root subalgebras</td> </tr>\n";
  DynkinType theType;
  theType.MakeSimpleType('F', 4);
  out << theCommands.ToStringLinksToCalculator(theType);
  out2 << theCommands.ToStringLinksToCalculatorDirectlyFromHD(theType);
  for (int i=6; i<=8; i++)
  { theType.MakeSimpleType('E', i);
    out << theCommands.ToStringLinksToCalculator(theType);
    out2 << theCommands.ToStringLinksToCalculatorDirectlyFromHD(theType);
  }
  theType.MakeSimpleType('G', 2);
  out << theCommands.ToStringLinksToCalculator(theType);
  for (int i=1; i<=8; i++)
  { theType.MakeSimpleType('A', i);
    out << theCommands.ToStringLinksToCalculator(theType);
    out2 << theCommands.ToStringLinksToCalculatorDirectlyFromHD(theType);
  }
  for (int i=4; i<=8; i++)
  { theType.MakeSimpleType('D', i);
    out << theCommands.ToStringLinksToCalculator(theType);
    out2 << theCommands.ToStringLinksToCalculatorDirectlyFromHD(theType);
  }
  for (int i=2; i<=8; i++)
  { theType.MakeSimpleType('B', i);
    out << theCommands.ToStringLinksToCalculator(theType);
    out2 << theCommands.ToStringLinksToCalculatorDirectlyFromHD(theType);
  }
  for (int i=3; i<=8; i++)
  { theType.MakeSimpleType('C', i);
    out << theCommands.ToStringLinksToCalculator(theType);
    out2 << theCommands.ToStringLinksToCalculatorDirectlyFromHD(theType);
  }
  out << "</table></p>" ;
  out2 << "</table></p>\n\n\n<br><br><p>Below are some links that cause conditional re-computation of some of the tables. "
  << "Please do not use these links; they are meant for computer debugging purposes only. </p><br>\n" << out.str()
  << "\n\n\n\n\n\n\n\n";
  return output.AssignValue(out2.str(), theCommands);
}

bool CommandList::innerPrintSSsubalgebras
  (CommandList& theCommands, const Expression& input, Expression& output, bool doForceRecompute,
   bool doAttemptToSolveSystems, bool doComputePairingTable, bool doComputeModuleDecomposition, bool doComputeNilradicals
   )
{ //bool showIndicator=true;
  MacroRegisterFunctionWithName("CommandList::innerSSsubalgebras");
  std::stringstream out;
  SemisimpleLieAlgebra* ownerSSPointer;
  bool isAlreadySubalgebrasObject=input.IsOfType<SemisimpleSubalgebras>();
  if (!isAlreadySubalgebrasObject)
  { if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(Serialization::innerSSLieAlgebra, input, ownerSSPointer))
      return output.SetError("Error extracting Lie algebra.", theCommands);
    if (ownerSSPointer->GetRank()>4)
    { out << "<b>This code is completely experimental and has been set to run up to rank 4."
      << " As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
      return output.AssignValue(out.str(), theCommands);
    } else
      out << "<b>This code is completely experimental. Use the following printouts on "
      << "your own risk</b>";
  } else
    ownerSSPointer=input.GetValue<SemisimpleSubalgebras>().owneR;
  SemisimpleLieAlgebra& ownerSS=*ownerSSPointer;
  std::string physicalFolder, displayFolder;
  FormatExpressions theFormat;
  theCommands.GetOutputFolders(ownerSS.theWeyl.theDynkinType, physicalFolder, displayFolder, theFormat);
  std::string theTitlePageFileNameNoPath= "SemisimpleSubalgebras_" + ownerSS.theWeyl.theDynkinType.ToString() + ".html";
  std::string theTitlePageFileName= physicalFolder+theTitlePageFileNameNoPath;
  out << "<br>Output file: <a href= \"" << displayFolder << theTitlePageFileNameNoPath << "\"> " << theTitlePageFileNameNoPath << "</a>";
  out << "<script> var reservedCountDownToRefresh = 5; "
  << "setInterval(function(){document.getElementById('reservedCountDownToRefresh').innerHTML "
  << "= --reservedCountDownToRefresh;}, 1000); </script>";
  out << "<b>... Redirecting to output file in <span style=\"font-size:36pt;\"><span id=\"reservedCountDownToRefresh\">5</span></span> "
  << "seconds...  </b>"
  << "<meta http-equiv=\"refresh\" content=\"5; url="
  << displayFolder << theTitlePageFileNameNoPath
  << "\">"
  ;
  if (!CGI::FileExists(theTitlePageFileName)|| doForceRecompute)
  { SemisimpleSubalgebras tempSSsas(ownerSS);
    SemisimpleSubalgebras& theSSsubalgebras= isAlreadySubalgebrasObject  ?
    input.GetValueNonConst<SemisimpleSubalgebras>() :
    theCommands.theObjectContainer.theSSsubalgebras
    [theCommands.theObjectContainer.theSSsubalgebras.AddNoRepetitionOrReturnIndexFirst(tempSSsas)]
    ;
    if (!isAlreadySubalgebrasObject)
      theSSsubalgebras.timeComputationStartInSeconds=theCommands.theGlobalVariableS->GetElapsedSeconds();
    theSSsubalgebras.flagComputeNilradicals=doComputeNilradicals;
    theSSsubalgebras.flagComputeModuleDecomposition=doComputeModuleDecomposition;
    theSSsubalgebras.flagAttemptToSolveSystems=doAttemptToSolveSystems;
    theSSsubalgebras.flagComputePairingTable=doComputePairingTable;
    std::fstream theFile;
    theCommands.theGlobalVariableS->System("mkdir " +physicalFolder);
    if(!CGI::OpenFileCreateIfNotPresent(theFile, theTitlePageFileName, false, true, false))
    { std::cout << "<br>This may or may not be a programming error. I requested to create file " << theTitlePageFileName
      << " for output. However, the file failed to create. "
      << " Possible explanations: 1. Programming error. 2. The calculator has no write permission to the"
      << " folder in which the file is located. 3. The folder does not exist for some reason lying outside of the calculator. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    if (!isAlreadySubalgebrasObject)
      theSSsubalgebras.FindTheSSSubalgebras(ownerSS, theCommands.theGlobalVariableS);
    theSSsubalgebras.timeComputationEndInSeconds=theCommands.theGlobalVariableS->GetElapsedSeconds();
    theSSsubalgebras.numAdditions=Rational::TotalSmallAdditions+Rational::TotalLargeAdditions;
    theSSsubalgebras.numMultiplications=Rational::TotalSmallMultiplications+
    Rational::TotalLargeMultiplications;

    theFormat.flagUseHTML=true;
    theFormat.flagUseHtmlAndStoreToHD=true;
    theFormat.flagUseLatex=true;
    theFile << "<html><title>Semisimple subalgebras of the semisimple Lie algebras: the subalgebras of "
    << theSSsubalgebras.owneR->theWeyl.theDynkinType.ToString() << "</title>" << "<script src=\"" << theCommands.DisplayPathServerBase
    << "jsmath/easy/load.js\"></script> " << "<body>" << theSSsubalgebras.ToString(&theFormat)
    << "<hr><hr>Calculator input for loading subalgebras directly without recomputation.\n<br>\n";
    Expression theSSE;
    Serialization::innerStoreSemisimpleSubalgebras(theCommands, theSSsubalgebras, theSSE);
    theFile << "Load{}" << theSSE.ToString();
    theFile << "</body></html>";
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CommandList::innerSSsubalgebras
(CommandList& theCommands, const Expression& input, Expression& output)
{ //bool showIndicator=true;
  MacroRegisterFunctionWithName("CommandList::innerSSsubalgebras");
  SemisimpleLieAlgebra* ownerSSPointer;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(Serialization::innerSSLieAlgebra, input, ownerSSPointer))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  SemisimpleLieAlgebra& ownerSS=*ownerSSPointer;
  std::stringstream out;
  if (ownerSS.GetRank()>4)
  { out << "<b>This code is completely experimental and has been set to run up to rank 4."
    << " As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
    return output.AssignValue(out.str(), theCommands);
  } else
    out << "<b>This code is completely experimental. Use the following printouts on your own risk</b>";
  SemisimpleSubalgebras tempSSsas(ownerSS);
  SemisimpleSubalgebras& theSSsubalgebras=
  theCommands.theObjectContainer.theSSsubalgebras[theCommands.theObjectContainer.theSSsubalgebras.AddNoRepetitionOrReturnIndexFirst(tempSSsas)];
  theSSsubalgebras.FindTheSSSubalgebras(ownerSS, theCommands.theGlobalVariableS);
  return output.AssignValue(theSSsubalgebras, theCommands);
}

bool CommandList::innerEmbedSSalgInSSalg(CommandList& theCommands, const Expression& input, Expression& output)
{ //bool showIndicator=true;
  MacroRegisterFunctionWithName("CommandList::innerEmbedSSalgInSSalg");
  if (!input.IsListNElements(3))
    return output.SetError("I expect two arguments - the two semisimple subalgebras.", theCommands);
  const Expression& EsmallSA=input[1];
  const Expression& ElargeSA=input[2];

  SemisimpleLieAlgebra* theSmallSapointer=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(Serialization::innerSSLieAlgebra, EsmallSA, theSmallSapointer))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  SemisimpleLieAlgebra* thelargeSapointer=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(Serialization::innerSSLieAlgebra, ElargeSA, thelargeSapointer))
    return output.SetError("Error extracting Lie algebra.", theCommands);

  SemisimpleLieAlgebra& ownerSS=*thelargeSapointer;
  SemisimpleLieAlgebra& smallSS=*theSmallSapointer;
  std::stringstream out;
  if (ownerSS.GetRank()>4)
  { out << "<b>This code is completely experimental and has been set to run up to rank 4."
    << " As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
    return output.AssignValue(out.str(), theCommands);
  }
  else
    out << "<b>This code is completely experimental. Use the following printouts on your own risk</b>";
  SemisimpleSubalgebras tempSSsas(ownerSS);
  SemisimpleSubalgebras& theSSsubalgebras=
  theCommands.theObjectContainer.theSSsubalgebras[theCommands.theObjectContainer.theSSsubalgebras.AddNoRepetitionOrReturnIndexFirst(tempSSsas)];
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

bool CommandList::innerAttemptExtendingEtoHEFwithHinCartan
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerAttemptExtendingEtoHEFwithHinCartan");
  if (input.children.size!=3)
    return output.SetError("Function takes 2 arguments - type and an element of the Lie algebra.", theCommands);
  SemisimpleLieAlgebra* ownerSS=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(Serialization::innerSSLieAlgebra, input[1], ownerSS))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  ElementSemisimpleLieAlgebra<Rational> theE;
  if (!Serialization::innerLoadElementSemisimpleLieAlgebraRationalCoeffs(theCommands, input[2], theE, *ownerSS))
    return output.SetError("Failed to extract element of semisimple Lie algebra. ", theCommands);
  ElementSemisimpleLieAlgebra<Rational> theF, theH;
  std::stringstream out, logStream;
  bool success=
  ownerSS->AttemptExtendingEtoHEFwithHinCartan(theE, theH, theF, &logStream, theCommands.theGlobalVariableS);
//  std::cout << "<br>The elts: " <<  theOperators.ToString();
//  std::cout << "<br> The common ad: " << commonAd.ToString();
  if (success)
    out << CGI::GetHtmlMathSpanPure("F:="+theF.ToString() + ";") << "<br>" << CGI::GetHtmlMathSpanPure("H:="+theH.ToString() + ";") << "<br>"
    << CGI::GetHtmlMathSpanPure("E:="+theE.ToString() + ";") << "<br><br>The log stream of the computation follows. " << logStream.str();
  else
    out << "<br>Couldn't extend E to sl(2)-triple. The log stream follows. " << logStream.str();
  return output.AssignValue(out.str(), theCommands);
}

bool CommandList::innerAdCommonEigenSpaces(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerAdCommonEigenSpaces");
  if (input.children.size<3)
    return output.SetError("Function ad common eigenspaces needs at least 2 arguments - type and at least one element of the algebra.", theCommands);
  SemisimpleLieAlgebra* ownerSS;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(Serialization::innerSSLieAlgebra, input[1], ownerSS))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  List<ElementSemisimpleLieAlgebra<Rational> > theOperators, outputElts;
  theOperators.ReservE(input.children.size-2);
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  for (int i=2; i<input.children.size; i++)
  { if (!Serialization::innerLoadElementSemisimpleLieAlgebraRationalCoeffs(theCommands, input[i], tempElt, *ownerSS))
      return output.SetError("Failed to extract element of semisimple Lie algebra. ", theCommands);
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
(CommandList& theCommands, const Expression& input, Expression& output, bool useGr, bool useRevLex, bool useModZp)
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
    return output.SetError("Failed to convert the first argument of the expression to rational number.", theCommands);
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
      for (int j=0; j<inputVector[i].size(); j++)
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
    << " rational functions or rationals, and I failed to convert your matrix to either type. "
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

bool CommandList::innerDeterminantPolynomial
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerDeterminantPolynomial");
  Matrix<Polynomial<Rational> > matPol;
  Expression theContext;
  if (!theCommands.GetMatrix(input, matPol, &theContext, -1, Serialization::innerPolynomial))
  { theCommands.Comments << "<hr>Failed to convert the input to matrix of polynomials. ";
    return false;
  }
  if (matPol.NumRows!=matPol.NumCols)
    return output.SetError("<hr>Failed to compute determinant: matrix is non-square. ", theCommands);
  if (matPol.NumRows>8)
  { theCommands.Comments << "<hr>Failed to compute determinant: matrix is larger than 8 x 8, and your matrix had "
    << matPol.NumRows << " rows. Note that you can compute determinant using the \\det "
    << "function which does Gaussian elimination "
    << " and will work for large rational matrices. This function is meant to be used with honest polynomial entries. "
    ;
    return false;
  }
  Polynomial<Rational> outputPoly;
  outputPoly.MakeDeterminantFromSquareMatrix(matPol);
  return output.AssignValueWithContext(outputPoly, theContext, theCommands);
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
  return output.AssignValue(outputMat, theCommands);
}

bool CommandList::innerTranspose
(CommandList& theCommands, const Expression& input, Expression& output)
{ //std::cout << "here i am, input is: " << input.ToString() << ", in full detail: " << input.ToStringFull();
  Matrix<Expression> theMat;
  output=input;
  output.SetChildAtomValue(0, theCommands.opSequence());
  theCommands.GetMatrixExpressions(output, theMat);
  //std::cout << "<br>" << theMat.ToString();
  theMat.Transpose();
  return output.AssignMatrixExpressions(theMat, theCommands);
}

bool CommandList::innerMatrixRationalTensorForm
(CommandList& theCommands, const Expression& input, Expression& output)
{ MatrixTensor<Rational> outputMat;
  output=input;
  if (!output.IsOfType<Matrix<Rational> >())
    if (!theCommands.innerMatrixRational(theCommands, input, output))
      return false;
  if (!output.IsOfType<Matrix<Rational> >())
    return false;
  outputMat=output.GetValue<Matrix<Rational> >();
  return output.AssignValue(outputMat, theCommands);
}

bool CommandList::innerMatrixRationalFunction(CommandList& theCommands, const Expression& input, Expression& output)
{ Matrix<RationalFunctionOld> outputMat;
  Expression ContextE;
  if (!theCommands.GetMatriXFromArguments(input, outputMat, &ContextE, -1, CommandList::innerRationalFunction))
  { theCommands.Comments << "<hr>Failed to get matrix of rational functions. ";
    return false;
  }
//  std::cout << "<hr>And the context is: " << ContextE.ToString();
  return output.AssignValueWithContext(outputMat, ContextE, theCommands);
}

bool CommandList::innerDrawPolarRfunctionTheta(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerDrawPolarRfunctionTheta");
  if (!input.IsListNElements(4))
    return output.SetError("Drawing polar coordinates takes three arguments: function, lower angle bound and upper angle bound. ", theCommands);
  const Expression& lowerE=input[2];
  const Expression& upperE=input[3];
  Expression functionE;
  Rational upperBound, lowerBound;
  if (!lowerE.IsOfType(&upperBound) || !upperE.IsOfType(&lowerBound))
    return output.SetError("Failed to convert upper and lower bounds of drawing function to rational numbers.", theCommands);
  if (upperBound<lowerBound)
    MathRoutines::swap(upperBound, lowerBound);
  if (!theCommands.CallCalculatorFunction(theCommands.innerSuffixNotationForPostScript, input[1], functionE))
    return false;
  std::stringstream out, resultStream;
  out << CGI::GetHtmlMathSpanPure(input[1].ToString()) << "<br>";
  resultStream << "\\documentclass{article}\\usepackage{pstricks}\\usepackage{auto-pst-pdf}\\usepackage{pst-plot}\\usepackage{pst-3dplot}\\begin{document} \\pagestyle{empty}";
  resultStream << " \\begin{pspicture}(-5, 5)(5,5)";
  resultStream << "\\psaxes[labels=none]{<->}(0,0)(-4.5,-4.5)(4.5,4.5)";
  resultStream << "\\parametricplot[linecolor=red, plotpoints=1000]{" << lowerBound.DoubleValue() << "}{" << upperBound.DoubleValue() << "}{";
  std::string funString=functionE.GetValue<std::string>();
  resultStream << funString << " t 57.29578 mul cos mul " << funString << " t 57.29578 mul sin mul " << "}";
  resultStream << "\\end{pspicture}\\end{document}";
  out << theCommands.WriteDefaultLatexFileReturnHtmlLink(resultStream.str(), true);
  out << "<br><b>LaTeX code used to generate the output. </b><br>" << resultStream.str();
  return output.AssignValue(out.str(), theCommands);
}

void CalculusFunctionPlot::operator+=(const CalculusFunctionPlot& other)
{ this->thePlotElementS.AddListOnTop(other.thePlotElementS);
  this->thePlotStrings.AddListOnTop(other.thePlotStrings);
  this->thePlotStringsWithHtml.AddListOnTop(other.thePlotStringsWithHtml);
  this->upperBounds.AddListOnTop(other.upperBounds);
  this->lowerBounds.AddListOnTop(other.lowerBounds);
}

void CalculusFunctionPlot::AddPlotOnTop
(const Expression& inputE, const std::string& inputPostfixNotation, const Rational& inputLowerBound, const Rational& inputUpperBound)
{ this->upperBounds.AddOnTop(inputUpperBound);
  this->lowerBounds.AddOnTop(inputLowerBound);
  this->thePlotElementS.AddOnTop(inputE);
  this->thePlotStrings.AddOnTop
  (this->GetPlotStringFromFunctionStringAndRanges(false, inputPostfixNotation, inputE.ToString(), inputLowerBound, inputUpperBound));
  this->thePlotStringsWithHtml.AddOnTop
  (this->GetPlotStringFromFunctionStringAndRanges(true, inputPostfixNotation, inputE.ToString(), inputLowerBound, inputUpperBound));
}

std::string CalculusFunctionPlot::GetPlotStringFromFunctionStringAndRanges
(bool useHtml, const std::string& functionStringPostfixNotation, const std::string& functionStringCalculatorFormat, const Rational& lowerBound, const Rational& upperBound)
{ std::stringstream out;
  out << "\n\n%Function formula: " << functionStringCalculatorFormat << "\n\n";
  if (useHtml)
    out << "<br>";
  out << "\\rput(1,3){$y=" << functionStringCalculatorFormat << "$}\n\n";
  if (useHtml)
    out << "<br>\n";
  out << "\\psplot[linecolor=\\psColorGraph, plotpoints=1000]{" << lowerBound.DoubleValue() << "}{" << upperBound.DoubleValue() << "}{";
  out << functionStringPostfixNotation << "}";
  return out.str();
}

std::string CalculusFunctionPlot::GetPlotStringAddLatexCommands(bool useHtml)
{ MacroRegisterFunctionWithName("CalculusFunctionPlot::GetPlotStringAddLatexCommands");
  std::stringstream resultStream;
  Rational theLowerBoundAxes(-1,2), theUpperBoundAxes=1, Rhalf(1/2);
  for (int i=0; i<this->lowerBounds.size; i++)
  { theLowerBoundAxes=MathRoutines::Minimum(this->lowerBounds[i], theLowerBoundAxes);
    theUpperBoundAxes=MathRoutines::Maximum(this->upperBounds[i], theUpperBoundAxes);
  }
  Rational theLowerBoundFrame=theLowerBoundAxes-Rhalf;
  Rational theUpperBoundFrame=theUpperBoundAxes+Rhalf;
  std::string lineSeparator= useHtml ? "<br>\n" : "\n";
  resultStream << "\\documentclass{article}\\usepackage{pstricks}\\usepackage{auto-pst-pdf}\\usepackage{pst-3dplot}\\usepackage{pst-plot}";
  resultStream << lineSeparator << "\\newcommand{\\psLabels}[2]{\\rput[t](#1, -0.1){$x$}\\rput[r](-0.1, #2){$y$}}" << lineSeparator;
  resultStream << "\\newcommand{\\psColorGraph}{red}" << lineSeparator << "\\begin{document} \\pagestyle{empty}" << lineSeparator
  << "\\newcommand{\\psColorAreaUnderGraph}{cyan}" << lineSeparator << "\\newcommand{\\psColorNegativeAreaUnderGraph}{orange}"
  << lineSeparator << " \\psset{xunit=1cm, yunit=1cm}";
  resultStream << lineSeparator;
  resultStream << "\\begin{pspicture}(" << theLowerBoundFrame.DoubleValue() << ", -5)(" << theUpperBoundFrame.DoubleValue() << ",5)\n\n";
  resultStream << lineSeparator;
  resultStream << "\\psframe*[linecolor=white](" << theLowerBoundFrame.DoubleValue() << ",-5)(" << theUpperBoundFrame.DoubleValue() << ",5)\n\n";
  resultStream << lineSeparator << "\\tiny\n" << lineSeparator;
  resultStream << " \\psaxes[ticks=none, labels=none]{<->}(0,0)(" << theLowerBoundAxes.DoubleValue() << ",-4.5)(" << theUpperBoundAxes.DoubleValue() << ",4.5)";
  resultStream << lineSeparator << "\\psLabels{" << theUpperBoundAxes.DoubleValue() << "}{5}" << lineSeparator;
  for (int i=0; i<this->thePlotStringsWithHtml.size; i++)
    if (useHtml)
      resultStream << this->thePlotStringsWithHtml[i] << lineSeparator;
    else
      resultStream << this->thePlotStrings[i] << lineSeparator;
  resultStream << lineSeparator << "\\end{pspicture}\n\n" << lineSeparator << "\\end{document}";
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

bool CommandList::innerPlot2DWithBars(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerPlot2DWithBars");
  //std::cout << input.ToString();
  if (input.children.size<6)
    return output.SetError("Plotting coordinates takes the following arguments: lower function, upper function, lower and upper bound, delta x. ", theCommands);
  Expression lowerEplot=input, upperEplot=input;
  lowerEplot.children.RemoveIndexShiftDown(2);
  upperEplot.children.RemoveIndexShiftDown(1);
  bool tempB=theCommands.innerPlot2D(theCommands, lowerEplot, output);
  CalculusFunctionPlot outputPlot;
  if (!tempB || !output.IsOfType<CalculusFunctionPlot>(&outputPlot))
  { theCommands.Comments << "<hr>Failed to get a plot from " << lowerEplot.ToString() << ", not proceding with bar plot.";
    return false;
  }
  tempB=theCommands.innerPlot2D(theCommands, upperEplot, output);
  if (!tempB || !output.IsOfType<CalculusFunctionPlot>())
  { theCommands.Comments << "<hr>Failed to get a plot from " << upperEplot.ToString() << ", not proceding with bar plot.";
    return false;
  }
  outputPlot+=output.GetValue<CalculusFunctionPlot>();
  const Expression& lowerFunctionE=input[1];
  const Expression& upperFunctionE=input[2];
  const Expression& lowerE=input[3];
  const Expression& upperE=input[4];
  const Expression& deltaE=input[5];
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
    return output.SetError("Failed to convert upper and lower bounds of drawing function to rational numbers.", theCommands);
  if (upperBound<lowerBound)
    MathRoutines::swap(upperBound, lowerBound);
  Expression xValueE, xExpression, theFunValueEnonEvaluated, theFunValueFinal;
  xExpression.MakeAtom(theCommands.AddOperationNoRepetitionOrReturnIndexFirst("x"), theCommands);
  List<double> xValues;
  List<double> fValuesLower;
  List<double> fValuesUpper;
  List<Rational> rValues;
  for (Rational i=lowerBound; i<=upperBound; i+=theDeltaNoSign)
    for (int j=0; j<2; j++)
    { if (theDeltaWithSign<0 && i==lowerBound)
        continue;
      rValues.AddOnTop(i);
      if (theDeltaWithSign>0 && i==upperBound)
        continue;
      xValueE.AssignValue(i, theCommands);
      theFunValueEnonEvaluated=(j==0) ? lowerFunctionE : upperFunctionE;
      theFunValueEnonEvaluated.Substitute(xExpression, xValueE);
  //    std::cout << "<br>substitution result:" << tempE2.ToString();
      BoundVariablesSubstitution tempSub;
      bool tempB;
      if (!theCommands.EvaluateExpression(theFunValueEnonEvaluated, theFunValueFinal, tempSub, tempB))
        return false;
  //    std::cout << "and after evaluation: " << theFunValueFinal.ToString();
      Rational finalResultRat;
      double finalResultDouble;
      if (!theFunValueFinal.IsOfType<Rational>(&finalResultRat))
      { if (!theFunValueFinal.IsOfType<double>(&finalResultDouble))
        { theCommands.Comments << "<hr>Failed to evaluate your function at point " << i << ", instead " << "I evaluated to " << theFunValueFinal.ToString();
          return false;
        }
      } else finalResultDouble=finalResultRat.DoubleValue();
      if (j==0)
      { xValues.AddOnTop(i.DoubleValue());
        fValuesLower.AddOnTop(finalResultDouble);
      } else
        fValuesUpper.AddOnTop(finalResultDouble);
    }
  std::stringstream outTex, outHtml;
  for (int k=0; k<2; k++)
    for (int i=0; i<xValues.size; i++)
    { //bool includePsLine=false;
      bool useNegativePattern=(fValuesLower[i]>fValuesUpper[i]);
      if (k==0 && useNegativePattern)
      { outTex << "\\psline*[linecolor=\\psColorNegativeAreaUnderGraph, linewidth=0.1pt]";
        outHtml << "<br>\\psline*[linecolor=\\psColorNegativeAreaUnderGraph, linewidth=0.1pt]";
      }
      if (k==0 && !useNegativePattern)
      { outTex << "\\psline*[linecolor=\\psColorAreaUnderGraph, linewidth=0.1pt]";
        outHtml << "<br>\\psline*[linecolor=\\psColorAreaUnderGraph, linewidth=0.1pt]";
      }
      if (k>0 && useNegativePattern)
      { outTex << "\\psline[linecolor=brown, linewidth=0.1pt]";
        outHtml << "<br>\\psline[linecolor=brown, linewidth=0.1pt]";
      }
      if (k>0 && !useNegativePattern)
      { outTex << "\\psline[linecolor=blue, linewidth=0.1pt]";
        outHtml << "<br>\\psline[linecolor=blue, linewidth=0.1pt]";
      }
      outTex << "(" << MathRoutines::ReducePrecision(xValues[i]) << ", " << MathRoutines::ReducePrecision(fValuesLower[i])
      << ")(" << MathRoutines::ReducePrecision(xValues[i]) << ", " << MathRoutines::ReducePrecision(fValuesUpper[i]) << ")"
      << "(" << MathRoutines::ReducePrecision(xValues[i]+theDeltaWithSign.DoubleValue()) << ", " << MathRoutines::ReducePrecision(fValuesUpper[i]) << ")("
      << MathRoutines::ReducePrecision(xValues[i]+theDeltaWithSign.DoubleValue()) << ", " << MathRoutines::ReducePrecision(fValuesLower[i]) << ")"
      << "(" << MathRoutines::ReducePrecision(xValues[i]) << ", " << MathRoutines::ReducePrecision(fValuesLower[i])<< ")";
      outHtml << "(" << MathRoutines::ReducePrecision(xValues[i]) << ", " << MathRoutines::ReducePrecision(fValuesLower[i])
      << ")(" << MathRoutines::ReducePrecision(xValues[i]) << ", " << MathRoutines::ReducePrecision(fValuesUpper[i]) << ")"
      << "(" << MathRoutines::ReducePrecision(xValues[i]+theDeltaWithSign.DoubleValue()) << ", " << MathRoutines::ReducePrecision(fValuesUpper[i]) << ")("
      << MathRoutines::ReducePrecision(xValues[i]+theDeltaWithSign.DoubleValue()) << ", " << MathRoutines::ReducePrecision(fValuesLower[i]) << ")"
      << "(" << MathRoutines::ReducePrecision(xValues[i]) << ", " << MathRoutines::ReducePrecision(fValuesLower[i]) << ")";
    }
  outHtml << "<br>";
  for (int i=0; i<rValues.size; i++)
  { std::stringstream tempStream;
    tempStream << "\\rput[t](" << MathRoutines::ReducePrecision(rValues[i].DoubleValue()) << ",-0.03)" << "{$";
    if (rValues[i].IsInteger())
      tempStream << rValues[i].ToString();
    else
      tempStream << "\\frac{" << rValues[i].GetNumerator().ToString() << "}" << "{" << rValues[i].GetDenominator().ToString() << "}";
    tempStream << "$}";
    outHtml << tempStream.str();
    outTex << tempStream.str();
  }
  outHtml << "<br>";
  CalculusFunctionPlot thePlot;
  thePlot.thePlotStrings.AddOnTop(outTex.str());
  thePlot.thePlotStringsWithHtml.AddOnTop(outHtml.str());
  thePlot.lowerBounds.AddOnTop(lowerBound);
  thePlot.upperBounds.AddOnTop(upperBound);
  thePlot.thePlotElementS.AddOnTop(input[1]);
  thePlot+=outputPlot;
  return output.AssignValue(thePlot, theCommands);
}

bool CommandList::innerPlot2D(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerPlot2D");
  //std::cout << input.ToString();
  if (input.children.size<4)
    return output.SetError("Plotting coordinates takes at least three arguments: function, lower and upper bound. ", theCommands);
  const Expression& lowerE=input[2];
  const Expression& upperE=input[3];
  Expression functionE;
  Rational upperBound, lowerBound;
  if (!lowerE.IsOfType(&upperBound) || !upperE.IsOfType(&lowerBound))
    return output.SetError("Failed to convert upper and lower bounds of drawing function to rational numbers.", theCommands);
  if (upperBound<lowerBound)
    MathRoutines::swap(upperBound, lowerBound);
  if (!theCommands.CallCalculatorFunction(theCommands.innerSuffixNotationForPostScript, input[1], functionE))
  { std::stringstream out;
    out << "Failed to convert expression " << input[1].ToString() << " to postfix notation. ";
    return output.SetError(out.str(), theCommands);
  }
  CalculusFunctionPlot thePlot;
  thePlot.AddPlotOnTop(input[1], functionE.GetValue<std::string>(), lowerBound, upperBound);
  return output.AssignValue(thePlot, theCommands);
}

bool CommandList::innerSuffixNotationForPostScript(CommandList& theCommands, const Expression& input, Expression& output)
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
    if (currentString=="\\sin")
      return output.AssignValue<std::string>(" 57.29578 mul sin ", theCommands);
    if (currentString=="\\cos")
      return output.AssignValue<std::string>(" 57.29578 mul cos ", theCommands);
    if (currentString=="\\tan")
      return output.AssignValue<std::string>(" 57.29578 mul tan ", theCommands);
    if (currentString=="\\arctan")
      return output.AssignValue<std::string>("ATAN ", theCommands);
    if (currentString=="\\arcsin")
      return output.AssignValue<std::string>("ASIN ", theCommands);
    if (currentString=="\\arccos")
      return output.AssignValue<std::string>("ACOS ", theCommands);
    if (currentString=="\\sqrt")
      return output.AssignValue<std::string>("sqrt ", theCommands);
    return output.SetError("Cannot convert "+currentString+ " to suffix notation.", theCommands);
  }
  std::stringstream out;
  if (input.IsOfType<Rational>())
  { out << input.GetValue<Rational>().DoubleValue();
    return output.AssignValue(out.str(), theCommands);
  }
  Expression currentE;
  bool useUsualOrder=!input[0].IsAtoM(theCommands.opDivide()) && !input[0].IsAtoM(theCommands.opThePower());
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
  out << currentString << " ";
  return output.AssignValue(out.str(), theCommands);
}

bool CommandList::innerCharacterSSLieAlgFD(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerCharacterSSLieAlgFD");
  Vector<Rational> theHW;
  Selection parSel;
  SemisimpleLieAlgebra* ownerSSLiealg;
  Expression tempE, tempE2;
  if (!theCommands.GetTypeHighestWeightParabolic(theCommands, input, output, theHW, parSel, tempE, ownerSSLiealg, 0))
    return false;
  if (output.IsError())
    return true;
  if (!parSel.CardinalitySelection==0)
    return output.SetError("I know only to compute with finite dimensional characters, for the time being.", theCommands);
  charSSAlgMod<Rational> theElt;
  theElt.MakeFromWeight(ownerSSLiealg->theWeyl.GetSimpleCoordinatesFromFundamental(theHW), ownerSSLiealg);
  return output.AssignValue(theElt, theCommands);
}

bool CommandList::innerConesIntersect(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerConesIntersect");
  if (!input.IsListNElements(3))
  { theCommands.Comments << "Function ConesIntersection expects 2 arguments, got " << input.children.size-1 << " instead. ";
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
  { out << "I got as input vectors of different dimensions, first groups had vectors of dimension " << coneNonStrictMatForm.NumCols
    << " and second of dimension " << coneStrictMatForm.NumCols << " which is not allowed. ";
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
  bool conesDoIntersect=coneNonStrictGens.ConesIntersect(coneStrictGens, coneNonStrictGens, &outputIntersection, &outputSeparatingNormal, theCommands.theGlobalVariableS);
  if (conesDoIntersect)
  { Vector<Rational> checkVector;
    checkVector.MakeZero(coneStrictMatForm.NumCols);
    for (int i=0; i<coneStrictGens.size; i++)
      checkVector+=coneStrictGens[i]*outputIntersection[i];
    for (int i=0; i<coneNonStrictGens.size; i++)
      checkVector+=coneNonStrictGens[i]*outputIntersection[coneStrictGens.size+i];
    if (!checkVector.IsEqualToZero())
    { std::cout << "<br>This is a programming error: the output linear combination" << outputIntersection.ToString()
      << " corresponds to the cone intersection " << checkVector.ToString() << " and is not equal to zero! Here is the cone output so far: "
      << out.str() << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    out << "<br>Cones intersect, here is one intersection: 0= " << outputIntersection.ToStringLetterFormat("v");
  } else
  { out << "<br>Cones have empty intersection.";
    out << "<br> A normal separating the cones is: n:=" << outputSeparatingNormal.ToString() << ". Indeed, ";
    for (int i=0; i<coneStrictGens.size; i++)
      out << "<br>\\langle v_{" << i+1 << "}, n\\rangle = " << outputSeparatingNormal.ScalarEuclidean(coneStrictGens[i]).ToString();
    for (int i=0; i<coneNonStrictGens.size; i++)
      out << "<br>\\langle v_{" << i+1 + coneStrictGens.size << "}, n\\rangle = " << outputSeparatingNormal.ScalarEuclidean(coneNonStrictGens[i]).ToString();
  }
  return output.AssignValue(out.str(), theCommands);
}

bool Expression::operator==(const std::string& other)const
{ std::string tempS;
  if (!this->IsOperation(&tempS))
    return false;
  return tempS==other;
}

bool CommandList::innerReverseOrder(CommandList& theCommands, const Expression& input, Expression& output)
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

bool CommandList::innerSolveSerreLikeSystem(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerSolveSerreLikeSystem");
  Vector<Polynomial<Rational> > thePolysRational;
  Expression theContext(theCommands);
  if (input.IsListNElementsStartingWithAtom(theCommands.GetOperations().GetIndexIMustContainTheObject("FindOneSolutionSerreLikePolynomialSystem")))
  { if (!theCommands.GetVectorFromFunctionArguments(input, thePolysRational, &theContext, 0, Serialization::innerPolynomial))
      return output.SetError("Failed to extract list of polynomials. ", theCommands);
  } else
    if (!theCommands.GetVectoR(input, thePolysRational, &theContext, 0, Serialization::innerPolynomial))
      return output.SetError("Failed to extract list of polynomials. ", theCommands);
  Vector<Polynomial<AlgebraicNumber> > thePolysAlgebraic;
  thePolysAlgebraic=thePolysRational;
  //int numVars=theContext.GetNumContextVariables();
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
  GroebnerBasisComputation<AlgebraicNumber> theComputation;
  theComputation.MaxNumComputations=1000;
  theCommands.theGlobalVariableS->theDefaultFormat=theFormat;
  std::cout << "<br>The context vars:<br>" << theContext.ToString();
  theComputation.SolveSerreLikeSystem(thePolysAlgebraic, theCommands.theGlobalVariableS);
  std::stringstream out;
  out << "<br>The context vars:<br>" << theContext.ToString();
  out << "<br>The polynomials: " << thePolysAlgebraic.ToString(&theFormat);
  out << "<br>Total number of polynomial computations: " << theComputation.NumberOfComputations;
  if (theComputation.flagSystemProvenToHaveNoSolution)
    out << "<br>The system does have a solution. ";
  else if(theComputation.flagSystemProvenToHaveSolution)
    out << "<br>System proven to have solution.";
  if (!theComputation.flagSystemProvenToHaveNoSolution)
  { if (theComputation.flagSystemSolvedOverBaseField)
      out << "<br>One solution follows. " << theComputation.ToStringSerreLikeSolution(&theFormat);
    else
      out << " However, I was unable to find such a solution: either my heuristics were not good enough, or no solution is rational.";
  }
  return output.AssignValue(out.str(), theCommands);
}

template <class coefficient>
coefficient ElementUniversalEnveloping<coefficient>::GetKillingFormProduct(const ElementUniversalEnveloping<coefficient>& right)const
{ MacroRegisterFunctionWithName("ElementUniversalEnveloping::GetKillingFormProduct");
  if (this->IsEqualToZero())
    return 0;
  coefficient result=0;
  ElementUniversalEnveloping<coefficient> adadAppliedToMon, tempElt;
  SemisimpleLieAlgebra* theOwner;
  theOwner=&this->GetOwner();
  MonomialUniversalEnveloping<coefficient> baseGen;
  std::cout << "<hr>";
  for (int i=0; i<theOwner->GetNumGenerators(); i++)
  { baseGen.MakeGenerator(i, *theOwner);
    adadAppliedToMon.MakeZero(*theOwner);
    adadAppliedToMon.AddMonomial(baseGen,1);
    right.AdjointRepresentationAction(adadAppliedToMon, tempElt);
    tempElt.Simplify();
    std::cout << "<br>acting by " << right.ToString() << " on " << adadAppliedToMon.ToString() << " to get " << tempElt.ToString();
    this->AdjointRepresentationAction(tempElt, adadAppliedToMon);
    adadAppliedToMon.Simplify();
    std::cout << " acting by " << this->ToString() << " on " << tempElt.ToString() << " to get " << adadAppliedToMon.ToString();
    std::cout << "; coeff of " << baseGen.ToString() << " = " << adadAppliedToMon.GetMonomialCoefficient(baseGen).ToString();
    result+=adadAppliedToMon.GetMonomialCoefficient(baseGen);
  }
  return result;
}

template <class coefficient>
coefficient SemisimpleLieAlgebra::GetKillingForm(const ElementSemisimpleLieAlgebra<coefficient>& left, const ElementSemisimpleLieAlgebra<coefficient>& right)
{ MacroRegisterFunctionWithName("SemisimpleLieAlgebra::GetKillingForm");
  coefficient result=0;
  ElementSemisimpleLieAlgebra<coefficient> adadAppliedToMon, tempElt;
  ChevalleyGenerator baseGen;
  for (int i=0; i<this->GetNumGenerators(); i++)
  { baseGen.MakeGenerator(*this, i);
    adadAppliedToMon.MakeZero();
    adadAppliedToMon.AddMonomial(baseGen, 1);
    this->LieBracket(right, adadAppliedToMon, tempElt);
    this->LieBracket(left, tempElt, adadAppliedToMon);
    result+=adadAppliedToMon.GetMonomialCoefficient(baseGen);
  }
  return result;
}

bool CommandList::innerKillingForm(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerKillingForm");
  if (!input.IsListNElements(3))
    return false;
  Expression leftE=input[1];
  Expression rightE=input[2];
  if (!leftE.IsBuiltInType() || !rightE.IsBuiltInType())
    return false;
  if (!Expression::MergeContexts(leftE, rightE))
    return false;
  Expression theContext=leftE.GetContext();
  ElementUniversalEnveloping<RationalFunctionOld> left, right;
  if (!leftE.IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >(&left) ||
      !rightE.IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >(&right))
    return false;
  if (left.IsEqualToZero() || right.IsEqualToZero())
    return output.AssignValue(0, theCommands);
  if (&left.GetOwner()!=&right.GetOwner())
    return false;
  ElementSemisimpleLieAlgebra<Rational> leftEltSS, rightEltSS;
  if (left.GetLieAlgebraElementIfPossible(leftEltSS) && right.GetLieAlgebraElementIfPossible(rightEltSS))
    return output.AssignValue(leftEltSS.GetOwner()->GetKillingForm(leftEltSS, rightEltSS), theCommands);
  return output.AssignValueWithContext(left.GetKillingFormProduct(right), theContext, theCommands);
}

bool CommandList::innerRootSubsystem(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerRootSubsystem");
  if (input.children.size<3)
    return false;
  SemisimpleLieAlgebra* theSSlieAlg=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(Serialization::innerSSLieAlgebra, input[1], theSSlieAlg))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  int theRank=theSSlieAlg->GetRank();
  Vector<Rational> currentRoot;
  Vectors<Rational> outputRoots;
  WeylGroup& theWeyl=theSSlieAlg->theWeyl;
  if (!theWeyl.theDynkinType.IsSimple())
  { theCommands.Comments << "<hr>Function root subsystem works for simple ambient types only.";
    return false;
  }
  for (int i=2; i<input.children.size; i++)
  { if (!theCommands.GetVectoR(input[i], currentRoot, 0, theRank, 0))
      return false;
    if (!theWeyl.RootSystem.Contains(currentRoot))
      return output.SetError("Input vector " + currentRoot.ToString() + " is not a root. ", theCommands);
    outputRoots.AddOnTop(currentRoot);
  }
  std::stringstream out;
  DynkinDiagramRootSubalgebra theDiagram;
  theWeyl.TransformToSimpleBasisGenerators(outputRoots, theWeyl.RootSystem);
  theDiagram.ComputeDiagramTypeKeepInput(outputRoots, theWeyl.CartanSymmetric);
  out << "Diagram final: " << theDiagram.ToStringRelativeToAmbientType(theWeyl.theDynkinType[0])
  << ". Simple basis: " << theDiagram.SimpleBasesConnectedComponents.ToString();
  return output.AssignValue(out.str(), theCommands);
}

bool CommandList::innerPerturbSplittingNormal(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerPerturbSplittingNormal");
  std::stringstream out;
  if (input.children.size!=4)
  { out << "Perturbing splitting normal takes 3 arguments: normal, positive vectors, and vectors relative to which to perturb. "
    << "Instead I got " << input.children.size-1 << ". ";
    return output.SetError(out.str(), theCommands);
  }
  Vector<Rational> splittingNormal;
  if (!theCommands.GetVectoR(input[1], splittingNormal, 0))
    return output.SetError("Failed to extract normal from first argument. ", theCommands);
  Matrix<Rational> theMat;
  Vectors<Rational> NonStrictCone, VectorsToPerturbRelativeTo;
  if (!theCommands.GetMatrix(input[2], theMat, 0, splittingNormal.size, 0))
    return output.SetError("Failed to extract matrix from second argument. ", theCommands);
  NonStrictCone.AssignMatrixRows(theMat);
  if (!theCommands.GetMatrix(input[3], theMat, 0, splittingNormal.size, 0))
    return output.SetError("Failed to extract matrix from third argument. ", theCommands);
  VectorsToPerturbRelativeTo.AssignMatrixRows(theMat);
  for (int i =0; i<NonStrictCone.size; i++)
    if (splittingNormal.ScalarEuclidean(NonStrictCone[i])<0)
      return output.SetError
      ("The normal vector " + splittingNormal.ToString() + " is has negative scalar product with " + NonStrictCone[i].ToString(), theCommands);
  out << "Perturbing " << splittingNormal.ToString() << " relative to cone " << NonStrictCone.ToString() << " and vectors " << VectorsToPerturbRelativeTo.ToString();
  splittingNormal.PerturbNormalRelativeToVectorsInGeneralPosition(NonStrictCone, VectorsToPerturbRelativeTo);
  out << "<br>End result: " << splittingNormal.ToString();
  return output.AssignValue(out.str(), theCommands);
}
