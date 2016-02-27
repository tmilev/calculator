//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader3Calculator0_Interface.h"
#include "vpfHeader2Math1_SemisimpleLieAlgebras.h"
#include "vpfImplementationHeader2Math15_UniversalEnveloping.h"
#include "vpfImplementationHeader2Math051_PolynomialComputations_Basic.h"
#include "vpfImplementationHeader2Math052_PolynomialComputations_Advanced.h"
#include "vpfImplementationHeader2Math1_SemisimpleLieAlgebras.h"
#include "vpfImplementationHeader2Math3_FiniteGroups.h" // undefined reference to `WeylGroupRepresentation<Rational>::reset()

static ProjectInformationInstance ProjectInfoVpf5_1cpp(__FILE__, "Calculator built-in functions. ");

template<class Element>
bool Matrix<Element>::SystemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution
(Matrix<Element>& matA, Matrix<Element>& matb, Vector<Element>* outputSolution)
//this function return true if Ax=b>=0 has a solution with x>=0 and records a solution x at outputPoint
//else returns false, where b is a given nonnegative column vector, A is an n by m matrix
//and x is a column vector with m entries
{ Matrix<Rational> tempMatA;
  Vector<Rational> matX;
  Selection BaseVariables;
  Rational GlobalGoal;
  GlobalGoal.MakeZero();
  if (matA.NumRows!= matb.NumRows)
    crash << crash;
  for (int j=0; j<matb.NumRows; j++)
  { GlobalGoal+=(matb.elements[j][0]);
    if(matb.elements[j][0].IsNegative())
      crash << crash;
  }
//  stOutput << "<hr>Starting matrix A: " << matA.ToString();
//  stOutput << "<hr>Starting matrix b: " << matb.ToString();
  if (GlobalGoal.IsEqualToZero())
    return false;
  int NumTrueVariables=matA.NumCols;
  //tempMatb.Assign(matb);
  tempMatA.init(matA.NumRows, NumTrueVariables+matA.NumRows);
  MemorySaving<HashedList<Selection> > tempSelList;
  HashedList<Selection> VisitedVertices;
  VisitedVertices.Clear();
  BaseVariables.init(tempMatA.NumCols);
  tempMatA.MakeZero();
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
        Matrix<Rational> ::ComputePotentialChangeGradient(tempMatA, BaseVariables, NumTrueVariables, i, PotentialChangeGradient, hasAPotentialLeavingVariable);
        if (PotentialChangeGradient.IsGreaterThanOrEqualTo(ChangeGradient) && hasAPotentialLeavingVariable)
        { EnteringVariable= i;
          ChangeGradient.Assign(PotentialChangeGradient);
        }
      }
    if (EnteringVariable!=-1)
    { int LeavingVariableRow;  Rational MaxMovement;
      Matrix<Rational>::GetMaxMovementAndLeavingVariableRow(MaxMovement, LeavingVariableRow, EnteringVariable, NumTrueVariables, tempMatA, matX, BaseVariables);
      Rational tempRat, tempTotalChange;
      if (tempMatA.elements[LeavingVariableRow][EnteringVariable].IsEqualToZero())
        crash << crash;
      tempRat.Assign(tempMatA.elements[LeavingVariableRow][EnteringVariable]);
      tempRat.Invert();
  //    if (BaseVariables.elements[LeavingVariableRow]==34)
  //      tempMatA.ComputeDebugString();
      for (int i=0; i<tempMatA.NumRows; i++)
        if (!tempMatA.elements[i][BaseVariables.elements[i]].IsEqualTo(1))
          crash << crash;
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
      if(!matX[BaseVariables.elements[LeavingVariableRow]].IsEqualToZero())
        crash << crash;
      BaseVariables.selected[BaseVariables.elements[LeavingVariableRow]]=false;
      BaseVariables.elements[LeavingVariableRow]= EnteringVariable;
      BaseVariables.selected[EnteringVariable]= true;
      //BaseVariables.ComputeDebugString();
      for (int i=0; i<tempMatA.NumRows; i++)
        if(!tempMatA.elements[i][BaseVariables.elements[i]].IsEqualTo(1))
          crash << crash;
    }
//    if (::Matrix<Rational> ::flagAnErrorHasOccurredTimeToPanic)
//    { Matrix<Rational>  tempMat;
//      tempMat.Assign(matX);
//      tempMat.ComputeDebugString();
//      tempDebugMat.ComputeDebugString();
//      tempMat.MultiplyOnTheLeft(tempDebugMat);
//      tempMat.ComputeDebugString();
//      if(!tempMat.IsEqualTo(matb)) crash << crash;
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
(List<Vector<Rational> >& StrictCone, List<Vector<Rational> >& NonStrictCone, Vector<Rational>* outputLinearCombo, Vector<Rational>* outputSplittingNormal)
{ Matrix<Rational> matA;
  Matrix<Rational> matb;
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
  matb.MakeZero(); matb.elements[theDimension][0].MakeOne();
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
  if (!Matrix<Rational>::SystemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution(matA, matb, outputLinearCombo))
  { if (outputSplittingNormal!=0)
    { bool tempBool=Vectors<coefficient>::GetNormalSeparatingCones(StrictCone, NonStrictCone, *outputSplittingNormal);
      if (!tempBool)
        crash << "This is an algorithmic/mathematical (hence also programming) error: I get that two cones do not intersect, yet there exists no plane separating them. "
        << "Something is wrong with the implementation of the simplex algorithm. The input which manifested the problem was: <br>StrictCone: <br>"
        << StrictCone.ToString() << "<br>Non-strict cone: <br>" << NonStrictCone.ToString() << "<br>" << crash;
    }
    return false;
  }
  if (outputLinearCombo!=0)
    for (int i=StrictCone.size; i<outputLinearCombo->size; i++)
      (*outputLinearCombo)[i]*=-1;
  return true;
}

bool Calculator::innerGCDOrLCM(Calculator& theCommands, const Expression& input, Expression& output, bool doGCD)
{ MacroRegisterFunctionWithName("Calculator::fGCD");
  Vector<Polynomial<Rational> > thePolys;
  Expression theContext(theCommands);
//  stOutput << "<br>Time elapsed before calling innerGCDOrLCM: " << theGlobalVariables.GetElapsedSeconds() << " seconds.";
//  stOutput << "<br>Input lispified: " << input.Lispify();
  if (!theCommands.GetVectorFromFunctionArguments(input, thePolys, &theContext, 2, CalculatorConversions::innerPolynomial<Rational>))
    return output.MakeError("Failed to extract a list of 2 polynomials. ", theCommands);
//  stOutput << "<br>Time elapsed after extracting two polynomials in innerGCDOrLCM: " << theGlobalVariables.GetElapsedSeconds() << " seconds.";
  Polynomial<Rational> outputP;
//  stOutput << "<br>context: " << theContext.ToString();
//  stOutput << "<br>The polys: " << thePolys.ToString();
  if (doGCD)
    RationalFunctionOld::gcd(thePolys[0], thePolys[1], outputP);
  else
    RationalFunctionOld::lcm(thePolys[0], thePolys[1], outputP);
  return output.AssignValueWithContext(outputP, theContext, theCommands);
}

bool Calculator::GetListPolysVariableLabelsInLex(const Expression& input, Vector<Polynomial<Rational> >& output, Expression& outputContext)
{ Expression theContextStart(*this);
  if (!this->GetVectorFromFunctionArguments(input, output, &theContextStart, 0, CalculatorConversions::innerPolynomial<Rational>))
    return false;
  if (output.size<2)
    return false;
  int numVars=theContextStart.ContextGetNumContextVariables();
  HashedList<Expression> theVars;
  theVars.SetExpectedSize(numVars);
  for (int i=0; i<numVars; i++)
    theVars.AddOnTop(theContextStart.ContextGetContextVariable(i));
  theVars.QuickSortAscending();
//  stOutput << "<hr>the vars: " << theVars.ToString();
  PolynomialSubstitution<Rational> theSub;
  theSub.SetSize(numVars);
  for (int i=0; i<theSub.size; i++)
    theSub[i].MakeMonomiaL(theVars.GetIndex(theContextStart.ContextGetContextVariable(i)), 1, 1, numVars);
  outputContext.MakeEmptyContext(*this);
  Expression PolyVarsE, tempE;
  PolyVarsE.reset(*this);
  PolyVarsE.children.Reserve(numVars+1);
  tempE.MakeAtom(this->opPolynomialVariables(), *this);
  PolyVarsE.AddChildOnTop(tempE);

  for (int i=0; i<numVars; i++)
  { PolyVarsE.AddChildOnTop(theVars[i]);
    Polynomial<Rational>& currentP=output[i];
    currentP.Substitution(theSub);
  }
  return outputContext.AddChildOnTop(PolyVarsE);
}

bool Calculator::innerPolynomialDivisionRemainder(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::fPolynomialDivisionQuotientRemainder");
  Expression theContext;
  Vector<Polynomial<Rational> > thePolys;
  if (!theCommands.GetListPolysVariableLabelsInLex(input, thePolys, theContext))
    return output.MakeError("Failed to extract list of polynomials. ", theCommands);
  GroebnerBasisComputation<Rational> theGB;
  theGB.theBasiS.SetSize(thePolys.size-1);
  for (int i=1; i<thePolys.size; i++)
  { if (thePolys[i].IsEqualToZero())
      return output.MakeError("Division by zero.", theCommands);
    theGB.theBasiS[i-1]=thePolys[i];
  }
//  stOutput << "<hr>The polys: " << thePolys.ToString() << "<br>The gb basis: "
//  << theGB.theBasiS.ToString() << "<hr>";
  Polynomial<Rational> outputRemainder;
  theGB.initForDivisionAlone(theGB.theBasiS);
  theGB.RemainderDivisionWithRespectToBasis(thePolys[0], &outputRemainder, -1);
  return output.AssignValueWithContext(outputRemainder, theContext, theCommands);
}

bool Calculator::innerPolynomialDivisionVerboseGrLex(Calculator& theCommands, const Expression& input, Expression& output)
{ return theCommands.innerPolynomialDivisionVerbose(theCommands, input, output, MonomialP::LeftGreaterThanTotalDegThenLexicographicLastVariableStrongest);
}

bool Calculator::innerPolynomialDivisionVerboseGrLexRev(Calculator& theCommands, const Expression& input, Expression& output)
{ return theCommands.innerPolynomialDivisionVerbose(theCommands, input, output, MonomialP::LeftGreaterThanTotalDegThenLexicographicLastVariableWeakest);
}

bool Calculator::innerPolynomialDivisionVerboseLex(Calculator& theCommands, const Expression& input, Expression& output)
{ return theCommands.innerPolynomialDivisionVerbose(theCommands, input, output, MonomialP::LeftGreaterThanLexicographicLastVariableStrongest);
}

bool Calculator::innerPolynomialDivisionVerboseLexRev(Calculator& theCommands, const Expression& input, Expression& output)
{ return theCommands.innerPolynomialDivisionVerbose(theCommands, input, output, MonomialP::LeftGreaterThanLexicographicLastVariableWeakest);
}

bool Calculator::innerPolynomialDivisionVerbose(Calculator& theCommands, const Expression& input, Expression& output, List<MonomialP>::OrderLeftGreaterThanRight theMonOrder)
{ MacroRegisterFunctionWithName("Calculator::innerPolynomialDivisionVerbose");
  Expression theContext;
  Vector<Polynomial<Rational> > thePolys;
  if (!theCommands.GetListPolysVariableLabelsInLex(input, thePolys, theContext))
    return output.MakeError("Failed to extract list of polynomials. ", theCommands);
  GroebnerBasisComputation<Rational> theGB;
  theGB.flagDoLogDivision=true;
  theGB.theBasiS.SetSize(thePolys.size-1);
  for (int i=1; i<thePolys.size; i++)
  { if (thePolys[i].IsEqualToZero())
      return output.MakeError("Division by zero.", theCommands);
    theGB.theBasiS[i-1]=thePolys[i];
  }
//  Polynomial<Rational> outputRemainder;
  theGB.initForDivisionAlone(theGB.theBasiS);
  theGB.thePolynomialOrder.theMonOrder= theMonOrder;
  theGB.RemainderDivisionWithRespectToBasis(thePolys[0], &theGB.remainderDivision, -1);
  theContext.ContextGetFormatExpressions(theGB.theFormat);
//  stOutput << "context vars: " << theFormat.polyAlphabeT;
  theGB.theFormat.flagUseLatex=true;
  theGB.theFormat.flagUseFrac=true;
  std::stringstream latexOutput;
  latexOutput <<
  "<br>In latex: <br>\\documentclass{article}\\usepackage{longtable}\\usepackage{xcolor}\\usepackage{multicol}"
  << "\\begin{document} "
  << theGB.GetDivisionStringLaTeX()
  << "\\end{document}";
  return output.AssignValue
  (theGB.GetDivisionStringHtml()+latexOutput.str(), theCommands);
}

bool DynkinSimpleType::HasEasySubalgebras()const
{ if (this->theLetter=='F')
    return true;
  if (this->theLetter=='G')
    return true;
  if (this->theLetter=='A' && this->theRank<=6)
    return true;
  if (this->theLetter=='B' && this->theRank<=4)
    return true;
  if (this->theLetter=='D' && this->theRank<=4)
    return true;
  if (this->theLetter=='C' && this->theRank<=5)
    return true;
  if (this->theLetter=='E' && this->theRank==6)
    return true;
  return false;
}

const std::string GlobalVariables::hopefullyPermanentWebAdressOfServerExecutable=
"http://calculator-algebra.org/calculator";
const std::string GlobalVariables::hopefullyPermanent_HTTPS_WebAdressOfServerExecutable=
"https://calculator-algebra.org/calculator";
const std::string GlobalVariables::hopefullyPermanentWebAdressOfServerOutputFolder=
"http://calculator-algebra.org/output/";

std::string Calculator::ToStringLinksToCalculatorDirectlyFromHD(const DynkinType& theType, FormatExpressions* theFormat)
{ std::stringstream out;
  std::string theTitlePageFileNameNoPathSlowLoad= "SemisimpleSubalgebras_" + theType.ToString() + ".html";
  std::string theTitlePageFileNameNoPathFastLoad= "SemisimpleSubalgebras_FastLoad_" + theType.ToString() + ".html";
  out << "<tr><td><a href=\"" << GlobalVariables::hopefullyPermanentWebAdressOfServerExecutable
  << "?request=compute&mainInput=printSemisimpleLieAlgebra%7B%7D"
  << theType[0].theLetter << "_" << theType[0].theRank << "\">"
  << theType[0].theLetter << theType[0].theRank << "</a></td>\n ";
  if (theType[0].HasEasySubalgebras())
  { out << "<td><a href=\"" << GlobalVariables::hopefullyPermanentWebAdressOfServerOutputFolder
    << theType.ToString() << "/" << theTitlePageFileNameNoPathSlowLoad << "\">"
    << theType[0].theLetter << theType[0].theRank << " semisimple subalgebras</a><br>"
    << "<a href=\"" <<  GlobalVariables::hopefullyPermanentWebAdressOfServerOutputFolder
    << theType.ToString() << "/" << theTitlePageFileNameNoPathFastLoad << "\">"
    << theType[0].theLetter << theType[0].theRank << " semisipmles subalgebras, fast load</a></td>\n ";
  }
  else
    out << "<td>Not available</td>\n";
  out << "<td><a href=\"" << GlobalVariables::hopefullyPermanentWebAdressOfServerExecutable
  << "?request=compute&mainInput=printSlTwoSubalgebras%7B%7D%28"
  << theType[0].theLetter << "_" << theType[0].theRank << "%29\">" << theType[0].theLetter << theType[0].theRank << " sl(2) triples</a></td>\n";
  out << "<td><a href=\""
  << GlobalVariables::hopefullyPermanentWebAdressOfServerExecutable << "?request=compute&mainInput=printRootSubalgebras%7B%7D%28"
  << theType[0].theLetter << "_" << theType[0].theRank << "%29\">" << theType[0].theLetter << theType[0].theRank << " root subalgebras</a></td>\n";
  return out.str();
}

std::string Calculator::ToStringLinksToCalculator(const DynkinType& theType, FormatExpressions* theFormat)
{ std::stringstream out;
  out << "<tr><td><a href=\"" << GlobalVariables::hopefullyPermanentWebAdressOfServerExecutable
  << "?request=compute&mainInput=printSemisimpleLieAlgebra%7B%7D"
  << theType[0].theLetter << "_" << theType[0].theRank << "\">" << theType[0].theLetter << theType[0].theRank << "</a></td>\n ";
  if (theType[0].HasEasySubalgebras())
    out << "<td><a href=\"" << GlobalVariables::hopefullyPermanentWebAdressOfServerExecutable << "?request=compute&mainInput=printSemisimpleSubalgebras%7B%7D%28"
    << theType[0].theLetter << "_" << theType[0].theRank << "%29\">" << theType[0].theLetter << theType[0].theRank << " semisimple subalgebras</a></td>\n ";
  else
    out << "<td>Not available</td>\n";
  out << "<td><a href=\"" << GlobalVariables::hopefullyPermanentWebAdressOfServerExecutable << "?request=compute&mainInput=printSlTwoSubalgebras%7B%7D%28"
  << theType[0].theLetter << "_" << theType[0].theRank << "%29\">" << theType[0].theLetter << theType[0].theRank << " sl(2) triples</a></td>\n";
  out << "<td><a href=\"" << GlobalVariables::hopefullyPermanentWebAdressOfServerExecutable << "?request=compute&mainInput=printRootSubalgebras%7B%7D%28"
  << theType[0].theLetter << "_" << theType[0].theRank << "%29\">" << theType[0].theLetter << theType[0].theRank << " root subalgebras</a></td>\n";
  return out.str();
}

bool Calculator::innerGetLinksToSimpleLieAlgerbas(Calculator& theCommands, const Expression& input, Expression& output)
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
  out2 << theCommands.ToStringLinksToCalculatorDirectlyFromHD(theType);
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

bool Calculator::innerPrintSSsubalgebrasNilradicals(Calculator& theCommands, const Expression& input, Expression& output)
{ return theCommands.innerPrintSSsubalgebras(theCommands, input, output, true, true, true, true, true, true);
}

bool Calculator::innerPrintSSsubalgebrasRecompute(Calculator& theCommands, const Expression& input, Expression& output)
{ return theCommands.innerPrintSSsubalgebras(theCommands, input, output, true, true, false, true, false, true);
}

bool Calculator::innerPrintSSsubalgebrasNoSolutions(Calculator& theCommands, const Expression& input, Expression& output)
{ return theCommands.innerPrintSSsubalgebras(theCommands, input, output, true, false, false, false, false, false);
}

bool Calculator::innerPrintSSsubalgebrasNoCentralizers(Calculator& theCommands, const Expression& input, Expression& output)
{ return theCommands.innerPrintSSsubalgebras(theCommands, input, output, true, true, false, true, false, false);
}

bool Calculator::innerPrintSSsubalgebrasRegular(Calculator& theCommands, const Expression& input, Expression& output)
{ return theCommands.innerPrintSSsubalgebras(theCommands, input, output, false, true, false, true, false, true);
}

bool Calculator::innerPrintSSsubalgebras
(Calculator& theCommands, const Expression& input, Expression& output, bool doForceRecompute, bool doAttemptToSolveSystems,
 bool doComputePairingTable, bool doComputeModuleDecomposition, bool doComputeNilradicals, bool doAdjustCentralizers)
{ //bool showIndicator=true;
  MacroRegisterFunctionWithName("Calculator::innerPrintSSsubalgebras");
//  stOutput << "Does this show in the output? ";
  if (theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection!=0)
    theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection();
//  stOutput << "After changing, does this show in output? ";
//  crash << crash;
  std::stringstream out;
  SemisimpleLieAlgebra* ownerSSPointer=0;
  bool isAlreadySubalgebrasObject=input.IsOfType<SemisimpleSubalgebras>();
  if (!isAlreadySubalgebrasObject)
  { if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, input, ownerSSPointer))
      return output.MakeError("Error extracting Lie algebra.", theCommands);
    if (ownerSSPointer->GetRank()>8)
    { out << "<b>This code is completely experimental and has been set to run up to rank 6. As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
      return output.AssignValue(out.str(), theCommands);
    } else
      out << "<b>This code is completely experimental. Use the following printouts on your own risk</b>";
  } else
    ownerSSPointer=input.GetValue<SemisimpleSubalgebras>().owner;
  if (ownerSSPointer==0)
    crash << crash;
  SemisimpleLieAlgebra& ownerSS=*ownerSSPointer;
  SemisimpleSubalgebras tempSSsas
  (ownerSS, &theCommands.theObjectContainer.theAlgebraicClosure, &theCommands.theObjectContainer.theLieAlgebras,
  &theCommands.theObjectContainer.theSltwoSAs, &theGlobalVariables);
  int indexInContainer=theCommands.theObjectContainer.theSSsubalgebras.AddNoRepetitionOrReturnIndexFirst(tempSSsas);
  SemisimpleSubalgebras& theSSsubalgebras= isAlreadySubalgebrasObject ? input.GetValueNonConst<SemisimpleSubalgebras>() :
  theCommands.theObjectContainer.theSSsubalgebras[indexInContainer];
  theSSsubalgebras.ToStringExpressionString=CalculatorConversions::innerStringFromSemisimpleSubalgebras;
  theSSsubalgebras.ComputeFolderNames(theSSsubalgebras.currentFormat);
  out << "<br>Output file: <a href= \""
  << theSSsubalgebras.DisplayNameMainFile1WithPath << "\"> " << theSSsubalgebras.DisplayNameMainFile1NoPath << "</a>";
  out << "<br>Output file, fast load, hover mouse over math expressions to get formulas: <a href= \""
  << theSSsubalgebras.DisplayNameMainFile2FastLoadWithPath << "\"> "
  << theSSsubalgebras.DisplayNameMainFile2FastLoadNoPath << "</a>";
  //  out << "<script> var ReservedCountDownToRefresh = 5; setInterval(function(){document.getElementById('ReservedCountDownToRefresh').innerHTML "
  //  << "= --ReservedCountDownToRefresh;}, 1000); </script>";
  //  out << "<b>... Redirecting to output file in <span style=\"font-size:36pt;\"><span id=\"ReservedCountDownToRefresh\">5</span></span> "
  //  << "seconds...  </b>"
  //<< "<meta http-equiv=\"refresh\" content=\"5; url="
  //<< displayFolder << theTitlePageFileNameNoPath
  //<< "\">"
  ;
  if (!FileOperations::FileExistsOnTopOfOutputFolder(theSSsubalgebras.RelativePhysicalNameRelativeMainFile1)|| doForceRecompute)
  { if (!isAlreadySubalgebrasObject)
      theSSsubalgebras.timeComputationStartInSeconds=theGlobalVariables.GetElapsedSeconds();
    theSSsubalgebras.flagComputeNilradicals=doComputeNilradicals;
    theSSsubalgebras.flagComputeModuleDecomposition=doComputeModuleDecomposition;
    theSSsubalgebras.flagAttemptToSolveSystems=doAttemptToSolveSystems;
    theSSsubalgebras.flagComputePairingTable=doComputePairingTable;
    theSSsubalgebras.flagAttemptToAdjustCentralizers=doAdjustCentralizers;
    theSSsubalgebras.CheckFileWritePermissions();
    if (!isAlreadySubalgebrasObject)
    { theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=-1;
      theSSsubalgebras.FindTheSSSubalgebrasFromScratch(ownerSS);
    }
    theSSsubalgebras.WriteReportToFiles();
  }
  return output.AssignValue(out.str(), theCommands);
}

bool MathRoutines::IsPrime(int theInt)
{ if (theInt<=1)
    return false;
  for (int i=2; i*i<=theInt; i+=2)
    if (theInt% i==0)
      return false;
  return true;
}

bool Calculator::innerAttemptExtendingEtoHEFwithHinCartan(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerAttemptExtendingEtoHEFwithHinCartan");
  if (input.children.size!=3)
    return output.MakeError("Function takes 2 arguments - type and an element of the Lie algebra.", theCommands);
  SemisimpleLieAlgebra* ownerSS=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, input[1], ownerSS))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  ElementSemisimpleLieAlgebra<Rational> theErational;
  if (!CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs(theCommands, input[2], theErational, *ownerSS))
    return output.MakeError("Failed to extract element of semisimple Lie algebra. ", theCommands);
  ElementSemisimpleLieAlgebra<AlgebraicNumber> theF, theH, theE;
  theE=theErational;
  std::stringstream out, logStream;
  bool success=ownerSS->AttemptExtendingEtoHEFwithHinCartan(theE, theH, theF, &logStream, &theGlobalVariables);
//  stOutput << "<br>The elts: " <<  theOperators.ToString();
//  stOutput << "<br> The common ad: " << commonAd.ToString();
  if (success)
    out << CGI::GetMathSpanPure("F="+theF.ToString() + ";") << "<br>" << CGI::GetMathSpanPure("H="+theH.ToString() + ";") << "<br>"
    << CGI::GetMathSpanPure("E="+theE.ToString() + ";") << "<br><br>The log stream of the computation follows. " << logStream.str();
  else
    out << "<br>Couldn't extend E to sl(2)-triple. The log stream follows. " << logStream.str();
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerAdCommonEigenSpaces(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerAdCommonEigenSpaces");
  if (input.children.size<3)
    return output.MakeError("Function ad common eigenspaces needs at least 2 arguments - type and at least one element of the algebra.", theCommands);
  SemisimpleLieAlgebra* ownerSS;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, input[1], ownerSS))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  List<ElementSemisimpleLieAlgebra<Rational> > theOperators, outputElts;
  theOperators.Reserve(input.children.size-2);
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  for (int i=2; i<input.children.size; i++)
  { if (!CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs(theCommands, input[i], tempElt, *ownerSS))
      return output.MakeError("Failed to extract element of semisimple Lie algebra. ", theCommands);
    theOperators.AddOnTop(tempElt);
  }
  ownerSS->GetCommonCentralizer(theOperators, outputElts);
//  stOutput << "<br>The elts: " <<  theOperators.ToString();
//  stOutput << "<br> The common ad: " << commonAd.ToString();
  std::stringstream out;
  out << "<br>EigenSpace basis (" << outputElts.size << " elements total):<br> (";
  for (int i=0; i<outputElts.size; i++)
  { ElementSemisimpleLieAlgebra<Rational>& currentElt=outputElts[i];
    out << currentElt.ToString();
    if (i!=outputElts.size-1)
      out << ", ";
  }
  out << ")";
  output.AssignValue(out.str(), theCommands);
  return true;
}

bool Calculator::innerGroebner
(Calculator& theCommands, const Expression& input, Expression& output, bool useGr, bool useRevLex, bool useModZp)
{ MacroRegisterFunctionWithName("Calculator::innerGroebner");
 /* if (input.IsSequenceNElementS())
  { output=input;
    if (!useGr && !useRevLex)
      return output.SetChildAtomValue(0, "GroebnerLexUpperLimit");
    if (!useGr && useRevLex)
      return output.SetChildAtomValue(0, "GroebnerRevLexUpperLimit");
    if (useGr)
      return output.SetChildAtomValue(0, "GroebnerGrLexUpperLimit");
  }*/
  Vector<Polynomial<Rational> > inputVector;
  Vector<Polynomial<ElementZmodP> > inputVectorZmodP;
  Expression theContext;
  if (input.children.size<3)
    return output.MakeError("Function takes at least two arguments. ", theCommands);
  const Expression& numComputationsE=input[1];
  Rational upperBound=0;
  if (!numComputationsE.IsOfType(&upperBound))
    return output.MakeError("Failed to convert the first argument of the expression to rational number.", theCommands);
  if (upperBound>1000000)
    return output.MakeError
    ("Error: your upper limit of polynomial operations exceeds 1000000, which is too large.\
     You may use negative or zero number give no computation bound, but please don't. ", theCommands);
  int upperBoundComputations=(int) upperBound.GetDoubleValue();
  output.reset(theCommands);
  for (int i=1; i<input.children.size; i++)
    output.children.AddOnTop(input.children[i]);
  int theMod;
  if (useModZp)
  { if (!output[1].IsSmallInteger(&theMod))
      return output.MakeError("Error: failed to extract modulo from the second argument. ", theCommands);
    if (!MathRoutines::IsPrime(theMod))
      return output.MakeError("Error: modulus not prime. ", theCommands);
  }
  if (!theCommands.GetVectorFromFunctionArguments<Polynomial<Rational> >(output, inputVector, &theContext, -1, CalculatorConversions::innerPolynomial<Rational>))
    return output.MakeError("Failed to extract polynomial expressions", theCommands);
  //theCommands.GetVector<Polynomial<Rational> >
  //(output, inputVector, &theContext, -1, CalculatorConversions::innerPolynomial);
  for (int i=0; i<inputVector.size; i++)
    inputVector[i].ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
  GroebnerBasisComputation<AlgebraicNumber> theGroebnerComputation;
  theContext.ContextGetFormatExpressions(theGroebnerComputation.theFormat);
//  stOutput << "context vars: " << theFormat.polyAlphabeT;

  theContext.ContextGetFormatExpressions(theGlobalVariables.theDefaultFormat.GetElement());
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
  List<Polynomial<AlgebraicNumber> > outputGroebner, outputGroebner2;
  outputGroebner=inputVector;
  outputGroebner2=inputVector;
//  stOutput << outputGroebner.ToString(&theFormat);
  if (useGr)
  { if (!useRevLex)
      theGroebnerComputation.thePolynomialOrder.theMonOrder=MonomialP::LeftGreaterThanTotalDegThenLexicographicLastVariableStrongest;
    else
      theGroebnerComputation.thePolynomialOrder.theMonOrder=MonomialP::LeftGreaterThanTotalDegThenLexicographicLastVariableWeakest;
  } else if (!useRevLex)
    theGroebnerComputation.thePolynomialOrder.theMonOrder=MonomialP::LeftGreaterThanLexicographicLastVariableStrongest;
  else
    theGroebnerComputation.thePolynomialOrder.theMonOrder=MonomialP::LeftGreaterThanLexicographicLastVariableWeakest;
  theGroebnerComputation.theFormat.thePolyMonOrder=theGroebnerComputation.thePolynomialOrder.theMonOrder;
  theGroebnerComputation.MaxNumGBComputations=upperBoundComputations;


  bool success=theGroebnerComputation.TransformToReducedGroebnerBasis(outputGroebner);
  std::stringstream out;
  out << theGroebnerComputation.ToStringLetterOrder(false);
  out << "Letter/expression order: ";
  for (int i=0; i<theContext.ContextGetNumContextVariables(); i++)
  { out << theContext.ContextGetContextVariable(i).ToString();
    if (i!=theContext.ContextGetNumContextVariables()-1)
      out << (useRevLex ? "&gt;": "&lt;");
  }
  out << "<br>Starting basis (" << inputVector.size  << " elements): ";
  for(int i=0; i<inputVector.size; i++)
    out << "<br>" << CGI::GetMathSpanPure(inputVector[i].ToString(&theGroebnerComputation.theFormat));
  if (success)
  { out << "<br>Minimal Groebner basis with " << outputGroebner.size << " elements, computed using algorithm 1, using "
    << theGroebnerComputation.NumberGBComputations << " polynomial operations. ";
    for(int i=0; i<outputGroebner.size; i++)
      out << "<br> " << CGI::GetMathSpanPure(outputGroebner[i].ToString(&theGroebnerComputation.theFormat));
    out << "<br>Output in calculator-ready format: ";
    out << "<br>(";
    for(int i=0; i<outputGroebner.size; i++)
    { out << outputGroebner[i].ToString(&theGroebnerComputation.theFormat);
      if (i!=outputGroebner.size-1)
        out << ", <br>";
    }
    out << ")";
  } else
  { out << "<br>Minimal Groebner basis not computed: exceeded the user-given limit of " << upperBoundComputations << " polynomial operations. ";
    out << "<br>An intermediate non-Groebner basis containing total " << theGroebnerComputation.theBasiS.size
    << " basis elements: ";
    out << "<br>GroebnerLexUpperLimit{}(10000, <br>";
    for (int i=0; i<theGroebnerComputation.theBasiS.size; i++)
    { out << theGroebnerComputation.theBasiS[i].ToString(&theGroebnerComputation.theFormat);
      if (i!=theGroebnerComputation.theBasiS.size-1)
        out << ", <br>";
    }
    out << ");";
  }
/*  theGroebnerComputation.TransformToReducedGroebnerBasisImprovedAlgorithm
(outputGroebner2, &theGlobalVariables);

  out << "<br>Minimal Groebner basis algorithm 2 (" << outputGroebner2.size << " elements):";
  for(int i=0; i<outputGroebner2.size; i++)
    out << "<br> "
    << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL(outputGroebner2[i].ToString(&theFormat))
  ;*/
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerDeterminantPolynomial(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerDeterminantPolynomial");
  Matrix<Polynomial<Rational> > matPol;
  Expression theContext;
  if (!theCommands.GetMatrix(input, matPol, &theContext, -1, CalculatorConversions::innerPolynomial<Rational>))
    return theCommands << "<hr>Failed to convert the input to matrix of polynomials. ";
  if (matPol.NumRows!=matPol.NumCols)
    return output.MakeError("<hr>Failed to compute determinant: matrix is non-square. ", theCommands);
  if (matPol.NumRows>8)
    return theCommands << "<hr>Failed to compute determinant: matrix is larger than 8 x 8, and your matrix had "
    << matPol.NumRows << " rows. Note that you can compute determinant using the \\det function which does Gaussian elimination "
    << " and will work for large rational matrices. This function is meant to be used with honest polynomial entries. ";
  Polynomial<Rational> outputPoly;
  outputPoly.MakeDeterminantFromSquareMatrix(matPol);
  return output.AssignValueWithContext(outputPoly, theContext, theCommands);
}

bool Calculator::innerTranspose(Calculator& theCommands, const Expression& input, Expression& output)
{ Matrix<Expression> theMat;
  output=input;
  output.SetChildAtomValue(0, theCommands.opSequence());
  theCommands.GetMatrixExpressions(output, theMat);
  theMat.Transpose();
  return output.AssignMatrixExpressions(theMat, theCommands);
}

void Plot::operator+=(const Plot& other)
{ this->thePlots.AddListOnTop(other.thePlots);
}

void Plot::operator+=(const PlotObject& other)
{ this->thePlots.AddOnTop(other);
}

void PlotObject::CreatePlotFunction
(const Expression& inputE, const std::string& inputPostfixNotation, double inputLowerBound, double inputUpperBound,
 double inputYmin, double inputYmax, Vectors<double>* inputPoints, int* inputColorRGB)
{ MacroRegisterFunctionWithName("PlotObject::Create");
  this->xLow=inputLowerBound;
  this->xHigh=inputUpperBound;
  this->yLow=inputYmin;
  this->yHigh=inputYmax;
  this->thePlotElement=(inputE);
  if (inputColorRGB!=0)
    this->colorRGB=*inputColorRGB;
  else
    this->colorRGB=CGI::RedGreenBlue(0,0,0);
  this->thePlotString=
  GetPlotStringFromFunctionStringAndRanges
  (false, inputPostfixNotation, inputE.ToString(), inputLowerBound, inputUpperBound);
  this->thePlotStringWithHtml=
  this->GetPlotStringFromFunctionStringAndRanges
  (true, inputPostfixNotation, inputE.ToString(), inputLowerBound, inputUpperBound);
  if (inputPoints!=0)
    this->thePoints=*inputPoints;
  this->thePlotType="plotFunction";
}

void Plot::AddFunctionPlotOnTop
(const Expression& inputE, const std::string& inputPostfixNotation, double inputLowerBound, double inputUpperBound,
 double inputYmin, double inputYmax, Vectors<double>* inputPoints, int* colorRGB)
{ PlotObject thePlot;
  thePlot.CreatePlotFunction
  (inputE, inputPostfixNotation, inputLowerBound, inputUpperBound, inputYmin, inputYmax, inputPoints, colorRGB);
  this->thePlots.AddOnTop(thePlot);
}

bool PlotObject::operator==(const PlotObject& other)const
{ return this->thePlotStringWithHtml==other.thePlotStringWithHtml &&
  this->xLow==other.xLow&&
  this->xHigh==other.xHigh &&
  this->yLow==other.yLow &&
  this->yHigh==other.yHigh &&
  this->thePlotElement==other.thePlotElement &&
  this->thePlotType==other.thePlotType;
}

PlotObject::PlotObject()
{ this->xLow=(0);
  this->xHigh=(0);
  this->yLow=(0);
  this->yHigh=(0);
  this->colorRGB=0;
}

void PlotObject::ComputeYbounds()
{ MacroRegisterFunctionWithName("PlotObject::ComputeYbounds");
  for (int i=0; i<this->thePoints.size; i++)
  { this->yHigh=MathRoutines::Maximum(this->yHigh, this->thePoints[i][1]);
    this->yLow=MathRoutines::Minimum(this->yLow, this->thePoints[i][1]);
  }
}

std::string PlotObject::GetPlotStringFromFunctionStringAndRanges
(bool useHtml, const std::string& functionStringPostfixNotation, const std::string& functionStringCalculatorFormat,
 double inputLowerBound, double inputUpperBound)
{ std::stringstream out;
  if (useHtml)
    out << "<br>";
  out << "\n\n%Function formula: " << functionStringCalculatorFormat << "\n\n";
  if (useHtml)
    out << "<br>";
  //out << "\\rput(1,3){$y=" << functionStringCalculatorFormat << "$}\n\n";
  //if (useHtml)
  //  out << "<br>\n";
  out << "\\psplot[linecolor=\\fcColorGraph, plotpoints=1000]{"
  << FloatingPoint::DoubleToString(inputLowerBound) << "}{" << FloatingPoint::DoubleToString(inputUpperBound) << "}{";
  out << functionStringPostfixNotation << "}";
  return out.str();
}

Plot::Plot()
{ this->theLowerBoundAxes=-0.5;
  this->theUpperBoundAxes=1;
  this->lowBoundY=-0.5;
  this->highBoundY=0.5;
  this->flagIncludeExtraHtmlDescriptions=true;
  this->DesiredHtmlHeightInPixels=400;
  this->DesiredHtmlWidthInPixels=600;
}

void Plot::ComputeAxesAndBoundingBox()
{ MacroRegisterFunctionWithName("Plot::ComputeAxesAndBoundingBox");
  this->theLowerBoundAxes=-0.5;
  this->theUpperBoundAxes=1.1;
  this->lowBoundY=-0.5;
  this->highBoundY=1.1;
  for (int i=0; i<this->thePlots.size; i++)
  { this->thePlots[i].ComputeYbounds();
    this->theLowerBoundAxes=MathRoutines::Minimum(this->thePlots[i].xLow, theLowerBoundAxes);
    this->theUpperBoundAxes=MathRoutines::Maximum(this->thePlots[i].xHigh, theUpperBoundAxes);
    this->lowBoundY=MathRoutines::Minimum(this->thePlots[i].yLow, this->lowBoundY);
    this->highBoundY=MathRoutines::Maximum(this->thePlots[i].yHigh, this->highBoundY);
  }
}

std::string Plot::GetPlotHtml()
{ MacroRegisterFunctionWithName("Plot::GetPlotHtml");
/*  if (this->flagIncludeExtraHtmlDescriptions)
    stOutput << "including xtra html";
  else
    stOutput << "NOT including xtra html";*/
  this->ComputeAxesAndBoundingBox();
  DrawingVariables theDVs;
  theDVs.DefaultHtmlHeight=this->DesiredHtmlHeightInPixels;
  theDVs.DefaultHtmlWidth=this->DesiredHtmlWidthInPixels;
  Vector<double> v1;
  Vector<double> v2;
  double theWidth=(this->theUpperBoundAxes-this->theLowerBoundAxes)*1.2+0.1;
  double theHeight=(this->highBoundY-this->lowBoundY)*1.2+0.1;
  theDVs.theBuffer.centerX[0]=((-this->theLowerBoundAxes+0.1)/theWidth)*((double) theDVs.DefaultHtmlWidth);
  theDVs.theBuffer.centerY[0]=((this->highBoundY+0.1)/theHeight)*((double) theDVs.DefaultHtmlHeight);

  double widthUnit=((double)theDVs.DefaultHtmlWidth)/theWidth;
  double heightUnit=((double)theDVs.DefaultHtmlHeight)/theHeight;
  theDVs.theBuffer.GraphicsUnit[0]= heightUnit<widthUnit ? heightUnit : widthUnit;
  v1.MakeZero(2);
  v2.MakeZero(2);
  v1[0]=this->theLowerBoundAxes-0.1;
  v1[1]=0;
  v2[0]=this->theUpperBoundAxes+0.1;
  v2[1]=0;
  theDVs.drawLineBetweenTwoVectorsBufferDouble
  (v1, v2, ((uint32_t) theDVs.PenStyleNormal), ((int) CGI::RedGreenBlue(0,0,0)));
  v1[0]=0;
  v1[1]=this->lowBoundY-0.1;
  v2[0]=0;
  v2[1]=this->highBoundY+0.1;
  theDVs.drawLineBetweenTwoVectorsBufferDouble(v1, v2, theDVs.PenStyleNormal, CGI::RedGreenBlue(0,0,0));
  v1[0]=1;
  v1[1]=-0.1;
  v2[0]=1;
  v2[1]=0.1;
  theDVs.drawLineBetweenTwoVectorsBufferDouble(v1, v2, theDVs.PenStyleNormal, CGI::RedGreenBlue(0,0,0));
  v1.SwapTwoIndices(0,1);
  v2.SwapTwoIndices(0,1);
  theDVs.drawLineBetweenTwoVectorsBufferDouble(v1, v2, theDVs.PenStyleNormal, CGI::RedGreenBlue(0,0,0));
  v1[0]=1;
  v1[1]=-0.2;
  theDVs.drawTextAtVectorBuffer(v1, (std::string)"1", CGI::RedGreenBlue(0,0,0), theDVs.TextStyleNormal,0);
  v1.SwapTwoIndices(0,1);
  theDVs.drawTextAtVectorBuffer(v1, (std::string)"1", CGI::RedGreenBlue(0,0,0), theDVs.TextStyleNormal,0);
  for (int i=0; i<this->thePlots.size; i++)
    for (int j=1; j<thePlots[i].thePoints.size; j++)
      theDVs.drawLineBetweenTwoVectorsBufferDouble
      (thePlots[i].thePoints[j-1], thePlots[i].thePoints[j], theDVs.PenStyleNormal,
       thePlots[i].colorRGB);
  std::stringstream resultStream;
  theDVs.flagIncludeExtraHtmlDescriptions=this->flagIncludeExtraHtmlDescriptions;
  resultStream << theDVs.GetHtmlFromDrawOperationsCreateDivWithUniqueName(2);
  return resultStream.str();
}

std::string Plot::GetPlotStringAddLatexCommands(bool useHtml)
{ MacroRegisterFunctionWithName("Plot::GetPlotStringAddLatexCommands");
  std::stringstream resultStream;
  this->ComputeAxesAndBoundingBox();
  std::string lineSeparator= useHtml ? "<br>\n" : "\n";
  resultStream << "\\documentclass{article}\\usepackage{pstricks}\\usepackage{auto-pst-pdf}\\usepackage{pst-math}\\usepackage{pst-plot}";
  resultStream << lineSeparator << "\\newcommand{\\fcLabels}[2]{\\rput[t](#1, -0.1){$x$}\\rput[r](-0.1, #2){$y$}}" << lineSeparator;
  resultStream << "\\addtolength{\\hoffset}{-3.5cm}\\addtolength{\\textwidth}{6.8cm}\\addtolength{\\voffset}{-3.2cm}\\addtolength{\\textheight}{6.3cm}"
  << lineSeparator;
  resultStream << "\\newcommand{\\fcColorGraph}{red}" << lineSeparator << "\\begin{document} \\pagestyle{empty}" << lineSeparator
  << "\\newcommand{\\fcColorAreaUnderGraph}{cyan}" << lineSeparator << "\\newcommand{\\fcColorNegativeAreaUnderGraph}{orange}"
  << "\\newcommand{\\fcAxesStandard}[4]{ \\psframe*[linecolor=white](! #1 #2)(! #3 0.1 add #4 01 add) \\psaxes[ticks=none, labels=none]{<->}(0,0)(#1, #2)(#3, #4)\\fcLabels{#3}{#4}}"
  << lineSeparator << " \\psset{xunit=1cm, yunit=1cm}";
  resultStream << lineSeparator;
  resultStream << "\\begin{pspicture}(" << FloatingPoint::DoubleToString(theLowerBoundAxes-0.4) << ", "
  << FloatingPoint::DoubleToString(lowBoundY-0.4) << ")("
  << FloatingPoint::DoubleToString(theUpperBoundAxes+0.4)
  << "," << FloatingPoint::DoubleToString(highBoundY+0.5) << ")\n\n";
  resultStream << lineSeparator << "\\tiny\n" << lineSeparator;
  resultStream << " \\fcAxesStandard{" << FloatingPoint::DoubleToString(theLowerBoundAxes-0.15)
  << "}{" << FloatingPoint::DoubleToString(lowBoundY-0.15) << "}{"
  << FloatingPoint::DoubleToString(theUpperBoundAxes+0.15) << "}{"
  << FloatingPoint::DoubleToString(highBoundY+0.15) << "}" << lineSeparator;
  for (int i=0; i<this->thePlots.size; i++)
    if (useHtml)
      resultStream << this->thePlots[i].thePlotStringWithHtml << lineSeparator;
    else
      resultStream << this->thePlots[i].thePlotString << lineSeparator;
  resultStream << "\\end{pspicture}\n\n" << lineSeparator << "\\end{document}";
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

bool Calculator::innerSuffixNotationForPostScript(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerSuffixNotationForPostScript");
  RecursionDepthCounter theCounter(&theCommands.RecursionDeptH);
//  stOutput << "<hr>Converting: " << input.ToString();
  if (*theCounter.theCounter ==theCommands.MaxRecursionDeptH-2)
    return output.AssignValue((std::string) "...", theCommands);
  std::string currentString;
  if (input.IsAtom(&currentString))
  { if (input.ToString()=="e")
      return output.AssignValue<std::string>(" 2.718281828 ", theCommands);
    if (input.ToString()=="\\pi")
      return output.AssignValue<std::string>(" 3.141592654 ", theCommands);
    if (input.theData>=theCommands.NumPredefinedAtoms)
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
    if (currentString=="\\log")
      return output.AssignValue<std::string>("ln ", theCommands);
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
    return output.MakeError("Cannot convert "+currentString+ " to suffix notation.", theCommands);
  }
  std::stringstream out;
  out.precision(7);
  bool hasDoubleValue=false;;
  double theDoubleValue=-1;
  if (input.IsOfType<Rational>())
  { hasDoubleValue=true;
    theDoubleValue=input.GetValue<Rational>().GetDoubleValue();
  }
  if (input.IsOfType<AlgebraicNumber>())
    hasDoubleValue=input.GetValue<AlgebraicNumber>().EvaluatesToDouble(&theDoubleValue);
  if (input.IsOfType<double>())
  { hasDoubleValue=true;
    theDoubleValue=input.GetValue<double>();
  }
  if (hasDoubleValue)
  { out << " " << FloatingPoint::DoubleToString(theDoubleValue);
    return output.AssignValue(out.str(), theCommands);
  }
  Expression currentE;
  bool useUsualOrder=!input[0].IsAtomGivenData(theCommands.opDivide()) && !input[0].IsAtomGivenData(theCommands.opThePower());
//  if (input[0].IsAtoM(theCommands.opDivide()))
//    stOutput << input.Lispify();
  if (useUsualOrder)
    for (int i=input.children.size-1; i>=1; i--)
    { if (!theCommands.innerSuffixNotationForPostScript(theCommands, input[i], currentE))
        return output.MakeError("Failed to convert "+input[i].ToString(), theCommands);
      if (!currentE.IsOfType(&currentString))
        return output.MakeError("Failed to convert "+input[i].ToString(), theCommands);
      out << currentString << " ";
    }
  else
    for (int i=1; i<input.children.size; i++)
    { if (!theCommands.innerSuffixNotationForPostScript(theCommands, input[i], currentE))
        return output.MakeError("Failed to convert "+input[i].ToString(), theCommands);
      if (!currentE.IsOfType(&currentString))
        return output.MakeError("Failed to convert "+input[i].ToString(), theCommands);
      out << currentString << " ";
    }
  if (!theCommands.innerSuffixNotationForPostScript(theCommands, input[0], currentE))
    return output.MakeError("Failed to convert "+input[0].ToString(), theCommands);
  if (!currentE.IsOfType(&currentString))
    return output.MakeError("Failed to convert "+input[0].ToString(), theCommands);
  out << currentString << " ";
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerCharacterSSLieAlgFD(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerCharacterSSLieAlgFD");
  Vector<Rational> theHW;
  Selection parSel;
  SemisimpleLieAlgebra* ownerSSLiealg;
  Expression tempE, tempE2;
  if (!theCommands.GetTypeHighestWeightParabolic(theCommands, input, output, theHW, parSel, tempE, ownerSSLiealg, 0))
    return false;
  if (output.IsError())
    return true;
  if (!parSel.CardinalitySelection==0)
    return output.MakeError("I know only to compute with finite dimensional characters, for the time being.", theCommands);
  charSSAlgMod<Rational> theElt;
  theElt.MakeFromWeight(ownerSSLiealg->theWeyl.GetSimpleCoordinatesFromFundamental(theHW), ownerSSLiealg);
  return output.AssignValue(theElt, theCommands);
}

bool Calculator::innerConesIntersect(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerConesIntersect");
  if (!input.IsListNElements(3))
    return theCommands << "Function ConesIntersection expects 2 arguments, got " << input.children.size-1 << " instead. ";
  Matrix<Rational> coneNonStrictMatForm;
  Matrix<Rational> coneStrictMatForm;
  Vectors<Rational> coneNonStrictGens;
  Vectors<Rational> coneStrictGens;
  if (!theCommands.GetMatrix(input[1], coneStrictMatForm))
    return theCommands << "Failed to extract matrix from the first argument, " << input[1].ToString();
  if (!theCommands.GetMatrix(input[2], coneNonStrictMatForm))
    return theCommands << "Failed to extract matrix from the second argument, " << input[2].ToString();
  std::stringstream out;
  if (coneNonStrictMatForm.NumCols!=coneStrictMatForm.NumCols)
  { out << "I got as input vectors of different dimensions, first groups had vectors of dimension " << coneNonStrictMatForm.NumCols
    << " and second of dimension " << coneStrictMatForm.NumCols << " which is not allowed. ";
    return output.MakeError(out.str(), theCommands);
  }
  coneNonStrictMatForm.GetVectorsFromRows(coneNonStrictGens);
  coneStrictMatForm.GetVectorsFromRows(coneStrictGens);
  out << "<br>Input non-strict (i.e., over Z_{&gt;=0}) cone: ";
  for (int i=0; i<coneStrictGens.size; i++)
    out << "<br>v_{" << i+1 << "}=" << coneStrictGens[i].ToString() << ";";
  out << "<br>Input strict (i.e., over Z_{&gt;0}) cone: ";
  for (int i=0; i<coneNonStrictGens.size; i++)
    out << "<br>v_{" << coneStrictGens.size+ i+1 << "}=" << coneNonStrictGens[i].ToString() << ";";
  Vector<Rational> outputIntersection, outputSeparatingNormal;
  bool conesDoIntersect=coneNonStrictGens.ConesIntersect(coneStrictGens, coneNonStrictGens, &outputIntersection, &outputSeparatingNormal);
  if (conesDoIntersect)
  { Vector<Rational> checkVector;
    checkVector.MakeZero(coneStrictMatForm.NumCols);
    for (int i=0; i<coneStrictGens.size; i++)
      checkVector+=coneStrictGens[i]*outputIntersection[i];
    for (int i=0; i<coneNonStrictGens.size; i++)
      checkVector+=coneNonStrictGens[i]*outputIntersection[coneStrictGens.size+i];
    if (!checkVector.IsEqualToZero())
      crash << "<br>This is a programming error: the output linear combination" << outputIntersection.ToString()
      << " corresponds to the cone intersection " << checkVector.ToString() << " and is not equal to zero! Here is the cone output so far: "
      << out.str() << crash;
    out << "<br>Cones intersect, here is one intersection: 0= " << outputIntersection.ToStringLetterFormat("v");
  } else
  { out << "<br>Cones have empty intersection.";
    out << "<br> A normal separating the cones is: n=" << outputSeparatingNormal.ToString() << ". Indeed, ";
    for (int i=0; i<coneStrictGens.size; i++)
      out << "<br>\\langle v_{" << i+1 << "}, n\\rangle = " << outputSeparatingNormal.ScalarEuclidean(coneStrictGens[i]).ToString();
    for (int i=0; i<coneNonStrictGens.size; i++)
      out << "<br>\\langle v_{" << i+1 + coneStrictGens.size << "}, n\\rangle = " << outputSeparatingNormal.ScalarEuclidean(coneNonStrictGens[i]).ToString();
  }
  return output.AssignValue(out.str(), theCommands);
}


bool Calculator::innerReverseOrderRecursively(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerReverseOrderRecursively");
  if (input.IsBuiltInType()||input.IsAtom())
  { output=input;
    return true;
  }
  if (input[0]=="ReverseRecursively")
  { Expression tempE=input;
    tempE.SetChildAtomValue(0, theCommands.opSequence());
    return theCommands.innerReverseOrderRecursively(theCommands, tempE, output);
  }
  output.reset(theCommands, input.children.size);
  output.AddChildOnTop(input[0]);
  for (int i=input.children.size-1; i>=1; i--)
  { Expression currentE=input[i];
    Expression reversedCurrentE;
    if (!theCommands.innerReverseOrderRecursively(theCommands,  currentE, reversedCurrentE))
      return false;
    output.AddChildOnTop(reversedCurrentE);
  }
  return true;
}

bool Calculator::innerReverseOrdeR(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerReverse");
  if (input.IsBuiltInType()||input.IsAtom())
  { output=input;
    return true;
  }
  if (input[0]=="Reverse")
  { Expression tempE=input;
    tempE.SetChildAtomValue(0, theCommands.opSequence());
    return theCommands.innerReverseOrdeR(theCommands, tempE, output);
  }
  output.reset(theCommands, input.children.size);
  output.AddChildOnTop(input[0]);
  for (int i=input.children.size-1; i>=1; i--)
    output.AddChildOnTop(input[i]);
  return true;
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
  stOutput << "<hr>";
  for (int i=0; i<theOwner->GetNumGenerators(); i++)
  { baseGen.MakeGenerator(i, *theOwner);
    adadAppliedToMon.MakeZero(*theOwner);
    adadAppliedToMon.AddMonomial(baseGen,1);
    right.AdjointRepresentationAction(adadAppliedToMon, tempElt);
    tempElt.Simplify();
    stOutput << "<br>acting by " << right.ToString() << " on " << adadAppliedToMon.ToString() << " to get " << tempElt.ToString();
    this->AdjointRepresentationAction(tempElt, adadAppliedToMon);
    adadAppliedToMon.Simplify();
    stOutput << " acting by " << this->ToString() << " on " << tempElt.ToString() << " to get " << adadAppliedToMon.ToString();
    stOutput << "; coeff of " << baseGen.ToString() << " = " << adadAppliedToMon.GetMonomialCoefficient(baseGen).ToString();
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

bool Calculator::innerKillingForm(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerKillingForm");
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
  if (!leftE.IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >(&left) || !rightE.IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >(&right))
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

bool Calculator::innerRootSubsystem(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerRootSubsystem");
  if (input.children.size<3)
    return false;
  SemisimpleLieAlgebra* theSSlieAlg=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, input[1], theSSlieAlg))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  int theRank=theSSlieAlg->GetRank();
  Vector<Rational> currentRoot;
  Vectors<Rational> outputRoots;
  WeylGroup& theWeyl=theSSlieAlg->theWeyl;
  if (!theWeyl.theDynkinType.IsSimple())
    return theCommands << "<hr>Function root subsystem works for simple ambient types only.";
  for (int i=2; i<input.children.size; i++)
  { if (!theCommands.GetVectoR(input[i], currentRoot, 0, theRank, 0))
      return false;
    if (!theWeyl.RootSystem.Contains(currentRoot))
      return output.MakeError("Input vector " + currentRoot.ToString() + " is not a root. ", theCommands);
    outputRoots.AddOnTop(currentRoot);
  }
  std::stringstream out;
  DynkinDiagramRootSubalgebra theDiagram;
  theWeyl.TransformToSimpleBasisGenerators(outputRoots, theWeyl.RootSystem);
  theDiagram.AmbientBilinearForm=theWeyl.CartanSymmetric;
  theDiagram.AmbientRootSystem =theWeyl.RootSystem;
  theDiagram.ComputeDiagramInputIsSimple(outputRoots);
  out << "Diagram final: " << theDiagram.ToString()
  << ". Simple basis: " << theDiagram.SimpleBasesConnectedComponents.ToString();
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerPerturbSplittingNormal(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerPerturbSplittingNormal");
  std::stringstream out;
  if (input.children.size!=4)
  { out << "Perturbing splitting normal takes 3 arguments: normal, positive vectors, and vectors relative to which to perturb. "
    << "Instead I got " << input.children.size-1 << ". ";
    return output.MakeError(out.str(), theCommands);
  }
  Vector<Rational> splittingNormal;
  if (!theCommands.GetVectoR(input[1], splittingNormal, 0))
    return output.MakeError("Failed to extract normal from first argument. ", theCommands);
  Matrix<Rational> theMat;
  Vectors<Rational> NonStrictCone, VectorsToPerturbRelativeTo;
  if (!theCommands.GetMatrix(input[2], theMat, 0, splittingNormal.size, 0))
    return output.MakeError("Failed to extract matrix from second argument. ", theCommands);
  NonStrictCone.AssignMatrixRows(theMat);
  if (!theCommands.GetMatrix(input[3], theMat, 0, splittingNormal.size, 0))
    return output.MakeError("Failed to extract matrix from third argument. ", theCommands);
  VectorsToPerturbRelativeTo.AssignMatrixRows(theMat);
  for (int i =0; i<NonStrictCone.size; i++)
    if (splittingNormal.ScalarEuclidean(NonStrictCone[i])<0)
      return output.MakeError("The normal vector " + splittingNormal.ToString() + " is has negative scalar product with " + NonStrictCone[i].ToString(), theCommands);
  out << "Perturbing " << splittingNormal.ToString() << " relative to cone " << NonStrictCone.ToString() << " and vectors " << VectorsToPerturbRelativeTo.ToString();
  splittingNormal.PerturbNormalRelativeToVectorsInGeneralPosition(NonStrictCone, VectorsToPerturbRelativeTo);
  out << "<br>End result: " << splittingNormal.ToString();
  return output.AssignValue(out.str(), theCommands);
}
