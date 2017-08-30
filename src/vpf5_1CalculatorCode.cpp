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
      Matrix<Rational>::GetMaxMovementAndLeavingVariableRow(MaxMovement, LeavingVariableRow, EnteringVariable, tempMatA, matX, BaseVariables);
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

bool Calculator::innerGCDOrLCMPoly(Calculator& theCommands, const Expression& input, Expression& output, bool doGCD)
{ MacroRegisterFunctionWithName("Calculator::innerGCDOrLCMPoly");
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
{ MacroRegisterFunctionWithName("Calculator::GetListPolysVariableLabelsInLex");
  Expression theContextStart(*this);
  if (!this->GetVectorFromFunctionArguments(input, output, &theContextStart, 0, CalculatorConversions::innerPolynomial<Rational>))
    return false;
  if (output.size<2)
    return false;
  int numVars=theContextStart.ContextGetNumContextVariables();
//  stOutput << "<hr>DEBIG: output: " << output.ToString();
 // stOutput << "<br>numVArs: " << numVars;
  HashedList<Expression> theVars;
  theVars.SetExpectedSize(numVars);
  for (int i=0; i<numVars; i++)
    theVars.AddOnTop(theContextStart.ContextGetContextVariable(i));
  theVars.QuickSortAscending();
  //stOutput << "<hr>DEBUG: he vars: " << theVars.ToString();
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
  for (int i=0; i<theVars.size; i++)
    PolyVarsE.AddChildOnTop(theVars[i]);
  //stOutput << "<hr>DEBUG: got ere " << theSub.ToString() << "theVars.size="  << theVars.size
  //<< "<br>num vars: " << numVars;
  for (int i=0; i< output.size; i++)
  { //stOutput << "<hr>DEBUG: adding on top: " << theVars[i].ToString();
    //stOutput << "<hr>got to here 1.5";
    Polynomial<Rational>& currentP=output[i];
    //stOutput << "<hr>subbing  in: " << currentP.ToString() << " sub: " << theSub.ToString();
    currentP.Substitution(theSub);
  }
//  stOutput << "<hr>DEBUG: got ere2 " << theSub.ToString();
  return outputContext.AddChildOnTop(PolyVarsE);
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

//WARNING
//Do not use the following web addresses to identify the server,
//EXCEPT in special circumstances described below.
//Instead, to get the web address of the
//calculator server,
//please use theGlobalVariables.hostNoPort.
//That address is extracted from the
//incoming message headers
//and is how our server is seen from the outside world.
//HOWEVER,
//the incoming message headers may be forged.
//In most cases, this is not a problem
//as the forgery will
//affect negatively only the forgerer.
//HOWEVER,
//we CANNOT rely on the headers when sending
//activation emails, as an attacker could claim my email,
//forge his headers, and send me activation email which would
//wrongly link to the host given in his forged headers
//- that would be an evil site prompting me for password.
//So, only in this very special circumstance,
//we can only rely on a hard-wired web address.

std::string GlobalVariables::hopefullyPermanentWebAdress=
"https://calculator-algebra.org";

std::string GlobalVariables::hopefullyPermanentWebAdressOfServerExecutable=
"http://calculator-algebra.org/cgi-bin/calculator";
std::string GlobalVariables::hopefullyPermanent_HTTPS_WebAdressOfServerExecutable=
"https://calculator-algebra.org/cgi-bin/calculator";
std::string GlobalVariables::hopefullyPermanentWebAdressOfServerOutputFolder=
"http://calculator-algebra.org/";
std::string GlobalVariables::hopefullyPermanent_HTTPS_WebAdressJavascriptFolder=
"https://calculator-algebra.org/";

std::string Calculator::ToStringLinksToCalculatorDirectlyFromHD(const DynkinType& theType, FormatExpressions* theFormat)
{ (void)theFormat;
  std::stringstream out;
  std::string theTitlePageFileNameNoPathSlowLoad= "SemisimpleSubalgebras_" + theType.ToString() + ".html";
  std::string theTitlePageFileNameNoPathFastLoad= "SemisimpleSubalgebras_FastLoad_" + theType.ToString() + ".html";
  out << "<tr><td><a href=\"" << GlobalVariables::hopefullyPermanentWebAdressOfServerExecutable
  << "?request=calculator&mainInput=PrintSemisimpleLieAlgebra%7B%7D"
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
  << "?request=calculator&mainInput=printSlTwoSubalgebras%7B%7D%28"
  << theType[0].theLetter << "_" << theType[0].theRank << "%29\">" << theType[0].theLetter << theType[0].theRank << " sl(2) triples</a></td>\n";
  out << "<td><a href=\""
  << GlobalVariables::hopefullyPermanentWebAdressOfServerExecutable << "?request=calculator&mainInput=printRootSubalgebras%7B%7D%28"
  << theType[0].theLetter << "_" << theType[0].theRank << "%29\">" << theType[0].theLetter << theType[0].theRank << " root subalgebras</a></td>\n";
  return out.str();
}

std::string Calculator::ToStringLinksToCalculator(const DynkinType& theType, FormatExpressions* theFormat)
{ (void) theFormat;//avoid unused parameter warning, portable
  std::stringstream out;
  out << "<tr><td><a href=\"" << GlobalVariables::hopefullyPermanentWebAdressOfServerExecutable
  << "?request=calculator&mainInput=PrintSemisimpleLieAlgebra%7B%7D"
  << theType[0].theLetter << "_" << theType[0].theRank << "\">" << theType[0].theLetter << theType[0].theRank << "</a></td>\n ";
  if (theType[0].HasEasySubalgebras())
    out << "<td><a href=\"" << GlobalVariables::hopefullyPermanentWebAdressOfServerExecutable << "?request=calculator&mainInput=printSemisimpleSubalgebras%7B%7D%28"
    << theType[0].theLetter << "_" << theType[0].theRank << "%29\">" << theType[0].theLetter << theType[0].theRank << " semisimple subalgebras</a></td>\n ";
  else
    out << "<td>Not available</td>\n";
  out << "<td><a href=\"" << GlobalVariables::hopefullyPermanentWebAdressOfServerExecutable << "?request=calculator&mainInput=printSlTwoSubalgebras%7B%7D%28"
  << theType[0].theLetter << "_" << theType[0].theRank << "%29\">" << theType[0].theLetter << theType[0].theRank << " sl(2) triples</a></td>\n";
  out << "<td><a href=\"" << GlobalVariables::hopefullyPermanentWebAdressOfServerExecutable << "?request=calculator&mainInput=printRootSubalgebras%7B%7D%28"
  << theType[0].theLetter << "_" << theType[0].theRank << "%29\">" << theType[0].theLetter << theType[0].theRank << " root subalgebras</a></td>\n";
  return out.str();
}

bool Calculator::innerGetLinksToSimpleLieAlgerbas(Calculator& theCommands, const Expression& input, Expression& output)
{ (void) input;//avoid unused parameter warning, portable
  std::stringstream out, out2;
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
  if (theGlobalVariables.flagAllowProcessMonitoring)
    if (theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection!=0)
      theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection();
  std::stringstream out;
  SemisimpleLieAlgebra* ownerSSPointer=0;
  bool isAlreadySubalgebrasObject=input.IsOfType<SemisimpleSubalgebras>();
  if (!isAlreadySubalgebrasObject)
  { if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
        (CalculatorConversions::innerSSLieAlgebra, input, ownerSSPointer))
      return output.MakeError("Error extracting Lie algebra.", theCommands);
    if (ownerSSPointer->GetRank()>8)
    { out << "<b>This code is completely experimental and has been set to run up to rank 6. As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
      return output.AssignValue(out.str(), theCommands);
    } else
      out << "<b>This code is completely experimental. Use the following printouts on your own risk.</b><br>";
  } else
    ownerSSPointer=input.GetValue<SemisimpleSubalgebras>().owner;
  if (ownerSSPointer==0)
    crash << "Zero pointer to semisimple Lie algebra: this shouldn't happen. " << crash;
  SemisimpleLieAlgebra& ownerSS=*ownerSSPointer;
  SemisimpleSubalgebras& theSSsubalgebras=
  theCommands.theObjectContainer.GetSemisimpleSubalgebrasCreateIfNotPresent(ownerSS.theWeyl.theDynkinType);

  theSSsubalgebras.ToStringExpressionString=CalculatorConversions::innerStringFromSemisimpleSubalgebras;
  theSSsubalgebras.ComputeFolderNames(theSSsubalgebras.currentFormat);
  //  out << "<script> var ReservedCountDownToRefresh = 5; setInterval(function(){document.getElementById('ReservedCountDownToRefresh').innerHTML "
  //  << "= --ReservedCountDownToRefresh;}, 1000); </script>";
  //  out << "<b>... Redirecting to output file in <span style=\"font-size:36pt;\"><span id=\"ReservedCountDownToRefresh\">5</span></span> "
  //  << "seconds...  </b>"
  //<< "<meta http-equiv=\"refresh\" content=\"5; url="
  //<< displayFolder << theTitlePageFileNameNoPath
  //<< "\">"
  ;
  if (!FileOperations::FileExistsVirtual(theSSsubalgebras.VirtualNameMainFile1)|| doForceRecompute)
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
  } else
    out << "Files precomputed, serving from HD. ";
  out << "<br>Output file: <a href= \""
  << theSSsubalgebras.DisplayNameMainFile1WithPath << "\"> " << theSSsubalgebras.DisplayNameMainFile1NoPath << "</a>";
  out << "<br>Output file, fast load, hover mouse over math expressions to get formulas: <a href= \""
  << theSSsubalgebras.DisplayNameMainFile2FastLoadWithPath << "\"> "
  << theSSsubalgebras.DisplayNameMainFile2FastLoadNoPath << "</a>";

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
  //stOutput << "DEBUG: got semisimple Lie algebra of type: "
  //<< ownerSS->theWeyl.theDynkinType.ToString();
  ElementSemisimpleLieAlgebra<Rational> theErational;
  if (!CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs(theCommands, input[2], theErational, *ownerSS))
    return output.MakeError("Failed to extract element of semisimple Lie algebra. ", theCommands);
  ElementSemisimpleLieAlgebra<AlgebraicNumber> theF, theH, theE;
  theE=theErational;
  std::stringstream out, logStream;
  bool success=ownerSS->AttemptExtendingEtoHEFwithHinCartan(theE, theH, theF, &logStream);
//  stOutput << "<br>The elts: " <<  theOperators.ToString();
//  stOutput << "<br> The common ad: " << commonAd.ToString();
  if (success)
    out << HtmlRoutines::GetMathSpanPure("F="+theF.ToString() + ";") << "<br>" << HtmlRoutines::GetMathSpanPure("H="+theH.ToString() + ";") << "<br>"
    << HtmlRoutines::GetMathSpanPure("E="+theE.ToString() + ";") << "<br><br>The log stream of the computation follows. " << logStream.str();
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
  //stOutput << "DEBUG: got semisimple Lie algebra of type: "
  //<< ownerSS->theWeyl.theDynkinType.ToString();
  List<ElementSemisimpleLieAlgebra<Rational> > theOperators, outputElts;
  theOperators.Reserve(input.size()-2);
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  for (int i=2; i<input.size(); i++)
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
    out << "<br>" << HtmlRoutines::GetMathSpanPure(inputVector[i].ToString(&theGroebnerComputation.theFormat));
  if (success)
  { out << "<br>Minimal Groebner basis with " << outputGroebner.size << " elements, computed using algorithm 1, using "
    << theGroebnerComputation.NumberGBComputations << " polynomial operations. ";
    for(int i=0; i<outputGroebner.size; i++)
      out << "<br> " << HtmlRoutines::GetMathSpanPure(outputGroebner[i].ToString(&theGroebnerComputation.theFormat));
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
    << HtmlRoutines::GetHtmlMathSpanNoButtonAddBeginArrayL(outputGroebner2[i].ToString(&theFormat))
  ;*/
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerDeterminantPolynomial(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerDeterminantPolynomial");
  Matrix<Polynomial<Rational> > matPol;
  Expression theContext;
  if (!theCommands.GetMatriXFromArguments
      (input, matPol, &theContext, -1,
       CalculatorConversions::innerPolynomial<Rational>))
    return theCommands << "<hr>Failed to convert the input to "
    << "matrix of polynomials. ";
  if (matPol.NumRows!=matPol.NumCols)
    return output.MakeError("<hr>Failed to compute determinant: matrix is non-square. ", theCommands);
  if (matPol.NumRows>8)
    return theCommands << "<hr>Failed to compute determinant: "
    << "matrix is larger than 8 x 8, and your matrix had "
    << matPol.NumRows << " rows. Note that you can compute "
    << "determinant using the \\det function which "
    << "does Gaussian elimination "
    << " and will work for large rational matrices. "
    << "This function is meant to be used with honest "
    << "polynomial entries. ";
  Polynomial<Rational> outputPoly;
  outputPoly.MakeDeterminantFromSquareMatrix(matPol);
  return output.AssignValueWithContext(outputPoly, theContext, theCommands);
}

bool Calculator::innerTranspose(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerTranspose");
  //stOutput << "DEBUG: got to here with nput: " << input.ToString();
  if (!input.IsSequenceNElementS() && !input.IsMatrix()
      && !input.StartsWithGivenAtom("Transpose") )
    return false;
  //  stOutput << "DEBUG: got to here";
  Matrix<Expression> theMat;
  if (input.StartsWithGivenAtom("Transpose"))
    theCommands.GetMatrixExpressionsFromArguments(input, theMat);
  else
    theCommands.GetMatrixExpressions(input, theMat);
  //  stOutput << "DEBUG: got to here, mat is: " << theMat.ToString();
  //The commented code used to be here. I don't remember why I added it, perhaps there was a solid reason for it?
  //If the code is uncommented, then ((1,2),(3,5))^t will not be transposed according to expectation.
  //If the commented code needs to be restored, please document why.
  //if (input.IsSequenceNElementS())
  //  if (theMat.NumRows!=1)
  //    return false;
  theMat.Transpose();
//  stOutput << "DEBUG: got to here pt2, mat is: " << theMat.ToString();
  return output.AssignMatrixExpressions(theMat, theCommands, true);
}

void Plot::operator+=(const Plot& other)
{ MacroRegisterFunctionWithName("Plot::operator+=");
  //stOutput << "<hr>DEBUG: Adding plot with name: "
  //<< this->canvasName << " to plot with name: "
  //<< other.canvasName << "<hr>";
  if (other.priorityCanvasName>this->priorityCanvasName)
    this->canvasName=other.canvasName;
  else if (this->canvasName=="" &&
           this->priorityCanvasName==other.priorityCanvasName)
    this->canvasName=other.canvasName;
  if (other.priorityCanvasName>this->priorityCanvasName)
    this->priorityCanvasName=other.priorityCanvasName;
  if (this->priorityCanvasName==0)
    this->canvasName="";
  if (this->dimension==-1)
    this->dimension=other.dimension;
  if (other.priorityViewRectangle>this->priorityViewRectangle)
  { this->highBoundY=other.highBoundY;
    this->lowBoundY=other.lowBoundY;
    this->theLowerBoundAxes=other.theLowerBoundAxes;
    this->theUpperBoundAxes=other.theUpperBoundAxes;
  }
  if (other.priorityViewRectangle==this->priorityViewRectangle)
  { this->highBoundY=MathRoutines::Maximum(this->highBoundY, other.highBoundY);
    this->lowBoundY =MathRoutines::Minimum(this->lowBoundY,  other.lowBoundY);
    this->theUpperBoundAxes=MathRoutines::Maximum(this->theUpperBoundAxes, other.theUpperBoundAxes);
    this->theLowerBoundAxes=MathRoutines::Minimum(this->theLowerBoundAxes, other.theLowerBoundAxes);
  }
  this->thePlots.AddListOnTop(other.thePlots);
  if (other.priorityWindow>this->priorityWindow)
  { this->DesiredHtmlHeightInPixels=other.DesiredHtmlHeightInPixels;
    this->DesiredHtmlWidthInPixels=other.DesiredHtmlWidthInPixels;
  } else if (this->priorityWindow==other.priorityWindow)
  { if (this->DesiredHtmlHeightInPixels<other.DesiredHtmlHeightInPixels)
      this->DesiredHtmlHeightInPixels=other.DesiredHtmlHeightInPixels;
    if (this->DesiredHtmlWidthInPixels<other.DesiredHtmlWidthInPixels)
      this->DesiredHtmlWidthInPixels=other.DesiredHtmlWidthInPixels;
  }
  if (!other.flagIncludeCoordinateSystem)
    this->flagIncludeCoordinateSystem=false;
  this->boxesThatUpdateMe.AddOnTopNoRepetition(other.boxesThatUpdateMe);
  this->priorityWindow=MathRoutines::Maximum(this->priorityWindow, other.priorityWindow);
  this->priorityViewRectangle=MathRoutines::Maximum(this->priorityViewRectangle, other.priorityViewRectangle);
}

bool Plot::operator==(const Plot& other)const
{ return
  this->priorityWindow==other.priorityWindow &&
  this->priorityCanvasName==other.priorityCanvasName &&
  this->priorityViewRectangle==other.priorityViewRectangle &&
  this->DesiredHtmlHeightInPixels==other.DesiredHtmlHeightInPixels &&
  this->DesiredHtmlWidthInPixels==other.DesiredHtmlWidthInPixels &&
  this->highBoundY==other.highBoundY &&
  this->lowBoundY==other.lowBoundY &&
  this->theLowerBoundAxes==other.theLowerBoundAxes &&
  this->theUpperBoundAxes==other.theUpperBoundAxes &&
  this->thePlots==other.thePlots &&
  this->boxesThatUpdateMe==other.boxesThatUpdateMe &&
  this->canvasName==other.canvasName &&
  this->dimension==other.dimension &&
  this->flagIncludeCoordinateSystem==other.flagIncludeCoordinateSystem
  ;
}

void Plot::operator+=(const PlotObject& other)
{ if (other.dimension!=-1 && this->dimension!=-1 && this->dimension!=other.dimension)
    crash << "Attempting to add plot of dimension: "
    << this->dimension << " to a plot of dimension: "
    << other.dimension << ". " << crash;
  if (this->dimension==-1)
    this->dimension=other.dimension;
  this->thePlots.AddOnTop(other);
  this->canvasName="";
}

bool PlotObject::operator==(const PlotObject& other)const
{ return
  this->thePlotString                == other.thePlotString                &&
  this->fillStyle                    == other.fillStyle                    &&
  this->thePlotStringWithHtml        == other.thePlotStringWithHtml        &&
  this->xLow                         == other.xLow                         &&
  this->xHigh                        == other.xHigh                        &&
  this->yLow                         == other.yLow                         &&
  this->yHigh                        == other.yHigh                        &&
  this->paramLow                     == other.paramLow                     &&
  this->paramHigh                    == other.paramHigh                    &&
  this->colorRGB                     == other.colorRGB                     &&
  this->colorFillRGB                 == other.colorFillRGB                 &&
  this->lineWidth                    == other.lineWidth                    &&
  this->dimension                    == other.dimension                    &&
  this->colorUV                      == other.colorUV                      &&
  this->colorVU                      == other.colorVU                      &&
  this->colorJS                      == other.colorJS                      &&
  this->lineWidthJS                  == other.lineWidthJS                  &&
  this->numSegmenTsJS                == other.numSegmenTsJS                &&
  this->thePointS                    == other.thePointS                    &&
  this->thePointsDouble              == other.thePointsDouble              &&
  this->thePointsJS                  == other.thePointsJS                  &&
  this->theRectangles                == other.theRectangles                &&
  this->thePlotType                  == other.thePlotType                  &&
  this->manifoldImmersion            == other.manifoldImmersion            &&
  this->coordinateFunctionsE         == other.coordinateFunctionsE         &&
  this->coordinateFunctionsJS        == other.coordinateFunctionsJS        &&
  this->variablesInPlay              == other.variablesInPlay              &&
  this->theVarRangesJS               == other.theVarRangesJS               &&
  this->leftPtE                      == other.leftPtE                      &&
  this->rightPtE                     == other.rightPtE                     &&
  this->paramLowE                    == other.paramLowE                    &&
  this->paramHighE                   == other.paramHighE                   &&
  this->numSegmentsE                 == other.numSegmentsE                 &&
  this->variablesInPlayJS            == other.variablesInPlayJS            &&
  this->leftBoundaryIsMinusInfinity  == other.leftBoundaryIsMinusInfinity  &&
  this->rightBoundaryIsMinusInfinity == other.rightBoundaryIsMinusInfinity &&
  this->leftPtJS                     == other.leftPtJS                     &&
  this->rightPtJS                    == other.rightPtJS                    &&
  this->colorFillJS                  == other.colorFillJS                  &&
  this->paramLowJS                   == other.paramLowJS                   &&
  this->paramHighJS                  == other.paramHighJS                  &&
  this->defaultLengthJS              == other.defaultLengthJS
  ;
}

PlotObject::PlotObject()
{ this->xLow         = 0;
  this->xHigh        = 0;
  this->paramLow     = 0;
  this->paramHigh    = 0;
  this->yLow         = 0;
  this->yHigh        = 0;
  this->colorRGB     = 0;
  this->lineWidth    = 1;
  this->colorFillRGB = 0;
  this->dimension    = -1;
  this->rightBoundaryIsMinusInfinity=false;
  this->leftBoundaryIsMinusInfinity=false;
}

void PlotObject::ComputeYbounds()
{ MacroRegisterFunctionWithName("PlotObject::ComputeYbounds");
  for (int i=0; i<this->thePointsDouble.size; i++)
  { this->yHigh=MathRoutines::Maximum(this->yHigh, this->thePointsDouble[i][1]);
    this->yLow=MathRoutines::Minimum(this->yLow, this->thePointsDouble[i][1]);
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
  this->DesiredHtmlHeightInPixels=350;
  this->DesiredHtmlWidthInPixels=350;
  this->defaultLineColor=0;
  this->flagPlotShowJavascriptOnly=false;
  this->priorityWindow=0;
  this->priorityViewRectangle=0;
  this->priorityCanvasName=0;
  this->dimension=-1;
  this->flagDivAlreadyDisplayed=false;
  this->flagIncludeCoordinateSystem=true;
}

void Plot::ComputeAxesAndBoundingBox()
{ MacroRegisterFunctionWithName("Plot::ComputeAxesAndBoundingBox");
  this->theLowerBoundAxes=-0.5;
  this->theUpperBoundAxes=1.1;
  this->lowBoundY=-0.5;
  this->highBoundY=1.1;
  for (int k=0; k<this->thePlots.size; k++)
  { this->thePlots[k].ComputeYbounds();
    this->theLowerBoundAxes=MathRoutines::Minimum(this->thePlots[k].xLow, theLowerBoundAxes);
    this->theUpperBoundAxes=MathRoutines::Maximum(this->thePlots[k].xHigh, theUpperBoundAxes);
    this->lowBoundY=MathRoutines::Minimum(this->thePlots[k].yLow, this->lowBoundY);
    this->highBoundY=MathRoutines::Maximum(this->thePlots[k].yHigh, this->highBoundY);
/*    for (int j=0; j<this->thePlots[k].theLines.size; j++)
    { List<Vector<double> > currentLine=this->thePlots[k].theLines[j];
      this->theLowerBoundAxes=MathRoutines::Minimum(this->theLowerBoundAxes, currentLine[0][0]);
      this->theLowerBoundAxes=MathRoutines::Minimum(this->theLowerBoundAxes, currentLine[1][0]);
      this->theUpperBoundAxes=MathRoutines::Maximum(this->theUpperBoundAxes, currentLine[0][0]);
      this->theUpperBoundAxes=MathRoutines::Maximum(this->theUpperBoundAxes, currentLine[1][0]);
      this->lowBoundY=MathRoutines::Minimum  (currentLine[0][1], this->lowBoundY);
      this->lowBoundY=MathRoutines::Minimum  (currentLine[1][1], this->lowBoundY);
      this->highBoundY=MathRoutines::Maximum (currentLine[0][1], this->highBoundY);
      this->highBoundY=MathRoutines::Maximum (currentLine[1][1], this->highBoundY);
    }*/
    for (int j=0; j<this->thePlots[k].thePointsDouble.size; j++)
    { Vector<double>& currentPoint=this->thePlots[k].thePointsDouble[j];
      if (!this->IsOKVector(currentPoint))
        continue;
      this->theLowerBoundAxes=MathRoutines::Minimum(this->theLowerBoundAxes, currentPoint[0]);
      this->theUpperBoundAxes=MathRoutines::Maximum(this->theUpperBoundAxes, currentPoint[0]);
      this->lowBoundY =MathRoutines::Minimum (currentPoint[1], this->lowBoundY);
      this->highBoundY=MathRoutines::Maximum (currentPoint[1], this->highBoundY);
    }
  }

}

void Plot::ComputeAxesAndBoundingBox3d()
{ MacroRegisterFunctionWithName("Plot::ComputeAxesAndBoundingBox3d");
  this->theLowerBoundAxes=-0.5;
  this->theUpperBoundAxes=1.1;
  this->lowBoundY=-0.5;
  this->highBoundY=1.1;
  for (int k=0; k<this->thePlots.size; k++)
  { this->thePlots[k].ComputeYbounds();
    this->theLowerBoundAxes=MathRoutines::Minimum(this->thePlots[k].xLow, theLowerBoundAxes);
    this->theUpperBoundAxes=MathRoutines::Maximum(this->thePlots[k].xHigh, theUpperBoundAxes);
    this->lowBoundY=MathRoutines::Minimum(this->thePlots[k].yLow, this->lowBoundY);
    this->highBoundY=MathRoutines::Maximum(this->thePlots[k].yHigh, this->highBoundY);
/*    for (int j=0; j<this->thePlots[k].theLines.size; j++)
    { List<Vector<double> > currentLine=this->thePlots[k].theLines[j];
      this->theLowerBoundAxes=MathRoutines::Minimum(this->theLowerBoundAxes, currentLine[0][0]);
      this->theLowerBoundAxes=MathRoutines::Minimum(this->theLowerBoundAxes, currentLine[1][0]);
      this->theUpperBoundAxes=MathRoutines::Maximum(this->theUpperBoundAxes, currentLine[0][0]);
      this->theUpperBoundAxes=MathRoutines::Maximum(this->theUpperBoundAxes, currentLine[1][0]);
      this->lowBoundY=MathRoutines::Minimum  (currentLine[0][1], this->lowBoundY);
      this->lowBoundY=MathRoutines::Minimum  (currentLine[1][1], this->lowBoundY);
      this->highBoundY=MathRoutines::Maximum (currentLine[0][1], this->highBoundY);
      this->highBoundY=MathRoutines::Maximum (currentLine[1][1], this->highBoundY);
    }*/
    for (int j=0; j<this->thePlots[k].thePointsDouble.size; j++)
    { Vector<double>& currentPoint=this->thePlots[k].thePointsDouble[j];
      if (!this->IsOKVector(currentPoint))
        continue;
      this->theLowerBoundAxes=MathRoutines::Minimum(this->theLowerBoundAxes, currentPoint[0]);
      this->theUpperBoundAxes=MathRoutines::Maximum(this->theUpperBoundAxes, currentPoint[0]);
      this->lowBoundY =MathRoutines::Minimum(currentPoint[1], this->lowBoundY);
      this->highBoundY=MathRoutines::Maximum(currentPoint[1], this->highBoundY);
    }
  }

}

bool Plot::IsOKVector(const Vector<double>& input)
{ for (int i=0; i<input.size; i++)
    if (std::isnan(input[i]))
      return false;
  return true;
}

std::string Plot::GetPlotHtml3d_New(Calculator& owner)
{ MacroRegisterFunctionWithName("Plot::GetPlotHtml3d_New");
  owner.flagHasGraphics=true;
  std::stringstream out;
  this->ComputeCanvasNameIfNecessary();
  //stOutput << "DEBUG: width: " << this->DesiredHtmlWidthInPixels
  //<< " height: " << this->DesiredHtmlHeightInPixels;
  if (!owner.flagPlotShowJavascriptOnly)
  { out << "<canvas width=\"" << this->DesiredHtmlWidthInPixels
    << "\" height=\"" << this->DesiredHtmlHeightInPixels << "\" "
    << "style=\"border:solid 1px\" id=\""
    << this->canvasName
    << "\" "
    << "onmousedown=\"calculatorCanvasClick(this, event);\" "
    << "onmouseup=\"calculatorCanvasMouseUp(this);\" "
    << "onmousemove=\"calculatorCanvasMouseMoveRedraw"
    << "(this, event.clientX, event.clientY);\""
    << " onmousewheel=\"calculatorCanvasMouseWheel(event);\""
    << " onDOMMouseScroll=\"calculatorCanvasMouseWheel(event);\""
    << ">"
    << "Your browser does not support the HTML5 canvas tag.</canvas><br>"
    << "<span id=\"" << this->canvasName << "Controls\"></span>"
    << "<span id=\"" << this->canvasName << "Messages\"></span>";
  }
  out << "<script language=\"javascript\">\n";
  std::string canvasFunctionName="functionMake"+ this->canvasName;
  out << "function " << canvasFunctionName << "()\n"
  << "{ ";
  for (int i=0; i<this->boxesThatUpdateMe.size; i++)
  { InputBox& currentBox=owner.theObjectContainer.theUserInputTextBoxesWithValues.GetValueCreateIfNotPresent
    (this->boxesThatUpdateMe[i]);
    out << "  calculatorPlotUpdaters['"
    << currentBox.GetSliderName() << "']=" << "'" << this->canvasName << "'"
    << ";\n";
  }
  List<std::string> the3dObjects;
  the3dObjects.SetSize(this->thePlots.size);
  int funCounter=0;
  for (int i=0; i<this->thePlots.size; i++)
  { PlotObject& currentO= this->thePlots[i];
    if (currentO.thePlotType=="surface")
    { out << currentO.GetJavascriptSurfaceImmersion(the3dObjects[i], this->canvasName, funCounter) << "\n "
      ;
    }
    if (currentO.thePlotType=="parametricCurve")
    { out << currentO.GetJavascriptCurveImmersionIn3d(the3dObjects[i], this->canvasName, funCounter) << "\n "
      ;
    }
  }
  out << "calculatorResetCanvas(document.getElementById('"
  << this->canvasName << "'));\n";
  out << "var theCanvas=calculatorGetCanvas(document.getElementById('"
  << this->canvasName
  << "'));\n"
  << "theCanvas.init('" << this->canvasName << "');\n";
  for (int i=0; i<this->thePlots.size; i++)
  { PlotObject& currentPlot=this->thePlots[i];
    if (currentPlot.thePlotType=="surface")
    { out
      << "theCanvas.drawSurface("
      <<  the3dObjects[i]
      << ");\n"
      ;
    }
    if (currentPlot.thePlotType=="parametricCurve")
    { //stOutput << "DEBUG: here be i";
      out
      << "theCanvas.drawCurve("
      <<  the3dObjects[i]
      << ");\n"
      ;
    }
    if (currentPlot.thePlotType=="setProjectionScreen" && currentPlot.thePointsDouble.size>=2)
    { out
      << "theCanvas.screenBasisUserDefault="
      << "["
      << currentPlot.thePointsDouble[0].ToStringSquareBracketsBasicType()
      << ","
      << currentPlot.thePointsDouble[1].ToStringSquareBracketsBasicType()
      << "];\n";
      out << "theCanvas.screenBasisUser=theCanvas.screenBasisUserDefault.slice();\n";
    }
    if (currentPlot.thePlotType=="label")
    { out
      << "theCanvas.drawText({"
      << "location: "
      << currentPlot.thePointsDouble[0].ToStringSquareBracketsBasicType()
      << ", "
      << "text:"
      << "'" << currentPlot.thePlotString << "'"
      << ", "
      << "color: "
      << "'"
      << currentPlot.colorJS
      << "'"
      << "});\n"
      ;

    }
    if (currentPlot.thePlotType=="segment" && currentPlot.thePointsDouble.size>=2)
    { out
      << "theCanvas.drawLine("
      << currentPlot.thePointsDouble[0].ToStringSquareBracketsBasicType()
      << ", "
      << currentPlot.thePointsDouble[1].ToStringSquareBracketsBasicType()
      << ", "
      << "'"
      << currentPlot.colorJS
      << "'"
      << ", "
      << currentPlot.lineWidth
      << ");\n"
      ;

    }
  }
  if (owner.flagPlotNoControls)
    out << "theCanvas.flagShowPerformance=false;\n";
  else
    out << "theCanvas.flagShowPerformance=true;\n";
  out << "theCanvas.setBoundingBoxAsDefaultViewWindow();\n";
  out
  << "theCanvas.redraw();\n"
  << "}\n"
  << "calculatorGetCanvas(document.getElementById('"
  << this->canvasName
  << "')).canvasResetFunction=" << canvasFunctionName << ";\n"
  << canvasFunctionName << "();\n"
  << "</script>"
  ;
  return out.str();
}

std::string Plot::ToStringDebug()
{ std::stringstream out;
  out <<  "Objects: " << this->thePlots.size << "<br>";
  for (int i=0; i<this->thePlots.size; i++)
  { if (this->thePlots[i].thePlotType=="surface")
    { PlotObject& theSurface= this->thePlots[i];
      out << theSurface.ToStringDebug();
      ;
    }
  }
  return out.str();
}

std::string PlotObject::ToStringDebug()
{ MacroRegisterFunctionWithName("PlotSurfaceIn3d::ToStringDebug");
  std::stringstream out;
//  out << "Surface: " << this->theSurface.ToString() << "<br>";
  out << "colorUV: " << this->colorUV << "<br>";
  out << "colorVU: " << this->colorVU << "<br>";
  out << "Coord f-ns: " << this->coordinateFunctionsE.ToStringCommaDelimited()
  << "<br>";
  out << "Vars: " << this->variablesInPlay << "<br>";
  out << "Var ranges: " << this->theVarRangesJS << "<br>";
  return out.str();
}

std::string PlotObject::GetJavascriptCurveImmersionIn3d
(std::string& outputCurveInstantiationJS, const std::string& canvasName,
 int& funCounter)
{ MacroRegisterFunctionWithName("PlotSurfaceIn3d::GetJavascriptCurveImmersionIn3d");
  std::stringstream out;
  funCounter++;
  std::stringstream fnNameStream;
  fnNameStream << "theCanvasSurfaceFn_" << canvasName << "_"
  << funCounter;
  std::string fnName=fnNameStream.str();

  if (this->coordinateFunctionsJS.size==3)
  { out << "function " << fnName
    << " (" << this->variablesInPlayJS[0]
    << "){\n";
    out << "return [ " << this->coordinateFunctionsJS[0] << ", "
    << this->coordinateFunctionsJS[1]
    << ", " << this->coordinateFunctionsJS[2] << "];\n";
    out << "}\n";
  } else
    out << "//this->theCoordinateFunctionsJS has "
    << this->coordinateFunctionsJS.size
    << " elements instead of 3 (expected).\n";
  std::stringstream curveInstStream;
  curveInstStream << "new CurveThreeD("
  << fnName
  << ", " << this->paramLowJS << ", " << this->paramHighJS
;
  if (this->numSegmenTsJS.size>0)
    curveInstStream << ", " << this->numSegmenTsJS[0];
  else
    curveInstStream << ", 100";
  curveInstStream << ", "
  << "\"" << this->colorJS << "\"";
  curveInstStream << ", " << this->lineWidth;
  curveInstStream << ")"
  ;
  outputCurveInstantiationJS=curveInstStream.str();

  return out.str();
}

std::string PlotObject::GetJavascriptSurfaceImmersion
(std::string& outputSurfaceInstantiationJS, const std::string& canvasName,
 int& funCounter)
{ MacroRegisterFunctionWithName("PlotSurfaceIn3d::GetJavascriptSurfaceImmersion");
  std::stringstream out;
  funCounter++;
  std::stringstream fnNameStream;
  fnNameStream << "theCanvasSurfaceFn_" << canvasName << "_" << funCounter;
  std::string fnName=fnNameStream.str();

  if (this->coordinateFunctionsJS.size==3)
  { out << "function " << fnName
    << " (" << this->variablesInPlayJS[0] << ","
    << this->variablesInPlayJS[1] << "){\n";
    out << "return [ " << this->coordinateFunctionsJS[0] << ", "
    << this->coordinateFunctionsJS[1]
    << ", " << this->coordinateFunctionsJS[2] << "];\n";
    out << "}\n";
  } else
  out << "//this->theCoordinateFunctionsJS has "
  << this->coordinateFunctionsJS.size
  << " elements instead of 3 (expected).\n";
  if (this->theVarRangesJS.size!=2)
    out << "//this->theVarRangesJS has " << this->theVarRangesJS.size << " elements instead of 2 (expected).";
  else if (this->theVarRangesJS[0].size!=2 || this->theVarRangesJS[1].size!=2)
    out << "//this->theVarRangesJS had unexpected value: "
    << this->theVarRangesJS.size;
  else
  { std::stringstream surfaceInstStream;
    surfaceInstStream << "new Surface("
    << fnName
    << ", [[" << this->theVarRangesJS[0][0] << "," << this->theVarRangesJS[1][0] << "],"
    << " ["   << this->theVarRangesJS[0][1] << ", " << this->theVarRangesJS[1][1] << "]], ";
    if (this->numSegmenTsJS.size>1)
      surfaceInstStream << "[" << this->numSegmenTsJS[0] << ","
      << this->numSegmenTsJS[1] << "], ";
    else
      surfaceInstStream << "[22, 4], ";
    surfaceInstStream << "{colorContour: \"black\", ";
    if (this->colorUV!="")
      surfaceInstStream << "colorUV: \"" << this->colorUV << "\",";
    else
      surfaceInstStream << "colorUV: \"red\",";
    if (this->colorVU!="")
      surfaceInstStream << "colorVU: \"" << this->colorVU << "\"";
    else
      surfaceInstStream << "colorVU: \"pink\"";
    surfaceInstStream << "}"
    << ",";
    if (this->lineWidthJS!="")
      surfaceInstStream << this->lineWidthJS;
    else
      surfaceInstStream << "1";
    surfaceInstStream << ")"
    ;
    outputSurfaceInstantiationJS=surfaceInstStream.str();
  }
  return out.str();
}

std::string Plot::GetPlotHtml(Calculator& owner)
{ MacroRegisterFunctionWithName("Plot::GetPlotHtml");
  if (this->dimension==3)
    return this->GetPlotHtml3d_New(owner);
  else if (this->dimension==2)
    return this->GetPlotHtml2d_New(owner);
  else
  { std::stringstream out;
    out << "Error:dimension=" << this->dimension;
    return out.str();
  }
}

int Plot::canvasCounteR=0;

std::string PlotObject::GetJavascriptParametricCurve2D
(std::string& outputPlotInstantiationJS,
 const std::string& canvasName,
 int& funCounter)
{ MacroRegisterFunctionWithName("PlotSurfaceIn3d::GetJavascript2dPlot");
  std::stringstream out;
  List<std::string> fnNames;
  fnNames.SetSize(this->coordinateFunctionsJS.size);
  for (int i=0; i<this->coordinateFunctionsJS.size; i++)
  { funCounter++;
    std::stringstream fnNameStream;
    fnNameStream << "theCanvasCoordinateFn_" << canvasName << "_"
    << funCounter;
    fnNames[i]=fnNameStream.str();
    if (this->variablesInPlayJS.size>0)
    { out << "function " << fnNames[i]
      << " (" << this->variablesInPlayJS[0] << "){\n";
      out << "return " << this->coordinateFunctionsJS[i] << ";\n";
      out << "}\n";
    } else
    { out << "console.log(\"Error: function with zero variables.\");";
    }
  }
  if (this->coordinateFunctionsJS.size!=2)
  { outputPlotInstantiationJS="";
    return out.str();
  }
  std::stringstream fnInstStream;
  fnInstStream.precision(7);
  fnInstStream << "drawCurve("
  << "[" << fnNames[0] << ", " << fnNames[1] << "]"
  << ", " << this->paramLowJS << ", " << this->paramHighJS << ", ";
  if (this->numSegmenTsJS.size>0)
    fnInstStream << this->numSegmenTsJS[0] << ", ";
  else
    fnInstStream << "200, ";
  fnInstStream << "'" << this->colorJS << "'";
  if (this->lineWidthJS!="")
    fnInstStream << ", " << this->lineWidthJS;
  else
    fnInstStream << ", " << this->lineWidth;
  fnInstStream << ");\n"
  ;
  outputPlotInstantiationJS=fnInstStream.str();
  return out.str();
}

std::string PlotObject::GetJavascriptDirectionField
(std::string& outputPlotInstantiationJS,
 const std::string& canvasName,
 int& funCounter)
{ MacroRegisterFunctionWithName("PlotSurfaceIn3d::GetJavascriptDirectionField");
  std::stringstream out;
  std::string fnName;
  funCounter++;
  std::stringstream fnNameStream;
  fnNameStream << "theCanvasCoordinateFn_" << canvasName << "_"
  << funCounter;
  fnName=fnNameStream.str();
  if (this->variablesInPlayJS.size>0)
  { out << "function " << fnName
    << " (";
    for (int i=0; i<this->variablesInPlayJS.size; i++)
    { out << this->variablesInPlayJS[i];
      if (i!=this->variablesInPlayJS.size-1)
        out << ", ";
    }
    out << "){\n";
    out << "return " << this->manifoldImmersionJS << ";\n";
    out << "}\n";
  } else
    out << "console.log(\"Error: function with zero variables.\");";
  std::stringstream fnInstStream;
  fnInstStream.precision(7);
  fnInstStream << "drawVectorField("
  << fnName
  << ", true"
  << ", ";
  for (int i=0; i<2; i++)
  { fnInstStream << "[";
    for (int j=0; j<this->theVarRangesJS.size; j++)
    { if (i<this->theVarRangesJS[j].size)
      { fnInstStream
        << this->theVarRangesJS[j][i];
        if (j!=this->theVarRangesJS.size-1)
          fnInstStream << ", ";
      } else
        fnInstStream << "(bad variable range)";
    }
    fnInstStream << "], ";
  }
  fnInstStream << "[";
  for (int i=0; i<this->numSegmenTsJS.size; i++)
  { fnInstStream << this->numSegmenTsJS[i];
    if (i!=this->numSegmenTsJS.size-1)
      fnInstStream << ", ";
  }
  fnInstStream << "], ";
  fnInstStream << this->defaultLengthJS << ", ";
  fnInstStream << "'" << this->colorJS << "'";
  if (this->lineWidthJS!="")
    fnInstStream << ", " << this->lineWidthJS;
  else
    fnInstStream << ", " << this->lineWidth;
  fnInstStream << ");\n"
  ;
  outputPlotInstantiationJS=fnInstStream.str();
  return out.str();
}

std::string PlotObject::GetJavascript2dPlot
(std::string& outputPlotInstantiationJS, const std::string& canvasName, int& funCounter)
{ MacroRegisterFunctionWithName("PlotSurfaceIn3d::GetJavascript2dPlot");
  std::stringstream out;
  std::stringstream fnNameStream;
  funCounter++;
  fnNameStream << "theCanvasPlotFn_" << canvasName << "_"
  << funCounter;
  std::string fnName=fnNameStream.str();
  if (this->variablesInPlayJS.size>0)
  { out << "function " << fnName
    << " (" << HtmlRoutines::GetJavascriptVariable(this->variablesInPlayJS[0]) << "){\n";
    out << "return " << this->coordinateFunctionsJS[0] << ";\n";
    out << "}\n";
  } else
  { out << "console.log(\"Error: function with zero variables.\");";
  }
  std::stringstream fnInstStream;
  fnInstStream << "drawFunction("
  << fnName
  << ", " << this->leftPtJS << ", " << this->rightPtJS << ", ";
  if (this->numSegmenTsJS.size>0)
    fnInstStream << this->numSegmenTsJS[0] << ", ";
  else
    fnInstStream << "200, ";
  fnInstStream << "'" << this->colorJS << "'"
  << ", ";
  if (this->lineWidthJS!="")
    fnInstStream << this->lineWidthJS;
  else
    fnInstStream << this->lineWidth;
  fnInstStream << ");\n";
  outputPlotInstantiationJS=fnInstStream.str();
  return out.str();
}

std::string PlotObject::GetJavascriptPoints
(std::string& outputPlotInstantiationJS, const std::string& canvasName, int& funCounter)
{ MacroRegisterFunctionWithName("PlotSurfaceIn3d::GetJavascriptPoints");
  (void) (canvasName);
  (void) funCounter;
  //stOutput << "<hr>DEBUG: Plotting points from: " << this->thePointsJS << "<hr>";
  std::stringstream fnInstStream;
  fnInstStream << "drawPoints([";
  for (int i=0; i<this->thePointsJS.NumRows; i++)
  { fnInstStream << "[";
    for (int j=0; j<this->thePointsJS.NumCols; j++)
    { fnInstStream << this->thePointsJS(i, j);
      if (j!=this->thePointsJS.NumCols-1)
        fnInstStream << ", ";
    }
    fnInstStream << "]";
    if (i!=this->thePointsJS.NumRows-1)
      fnInstStream << ", ";
  }
  fnInstStream << "], ";
  fnInstStream << "'" << this->colorJS << "');\n";
  outputPlotInstantiationJS=fnInstStream.str();
  return "";
}

std::string PlotObject::ToStringPointsList()
{ MacroRegisterFunctionWithName("PlotObject::ToStringPointsList");
  std::stringstream out;
  out << "[";
  for (int j=0; j<this->thePointsDouble.size; j++)
  { out << this->thePointsDouble[j].ToStringSquareBracketsBasicType();
    if (j!=this->thePointsDouble.size-1)
      out << ",";
  }
  out << "]";

  return out.str();
}

void Plot::ComputeCanvasNameIfNecessary()
{ if (this->canvasName!="")
    return;
  this->canvasCounteR++;
  //out << this->ToStringDebug();
  std::stringstream canvasNameStream;
  canvasNameStream << "theCanvas" << this->canvasCounteR;
  this->canvasName=canvasNameStream.str();
}

std::string Plot::GetPlotHtml2d_New(Calculator& owner)
{ MacroRegisterFunctionWithName("Plot::GetPlotHtml2d_New");
  owner.flagHasGraphics=true;
  if (this->flagDivAlreadyDisplayed)
    return "[plot alredy displayed]";
  this->flagDivAlreadyDisplayed=true;
  this->ComputeCanvasNameIfNecessary();
  std::stringstream out;
  if (this->priorityViewRectangle<=0)
    this->ComputeAxesAndBoundingBox();
    //out << this->ToStringDebug();
  if (!this->flagPlotShowJavascriptOnly)
  { out << "<canvas width=\"" << this->DesiredHtmlWidthInPixels
    << "\" height=\"" << this->DesiredHtmlHeightInPixels << "\" "
    << "style=\"border:solid 1px\" id=\""
    << this->canvasName
    << "\" "
    << "onmousedown=\"calculatorCanvasClick(this, event);\" "
    << "onmouseup=\"calculatorCanvasMouseUp(this);\" "
    << "onmousemove=\"calculatorCanvasMouseMoveRedraw"
    << "(this, event.clientX, event.clientY);\""
    << " onmousewheel=\"calculatorCanvasMouseWheel(event);\""
    << " onDOMMouseScroll=\"calculatorCanvasMouseWheel(event);\""
    << ">"
    << "Your browser does not support the HTML5 canvas tag.</canvas><br>"
    << "<span id=\"" << this->canvasName << "Controls\"></span>";
    if (!owner.flagPlotNoControls)
      out << "<br>";
    out << "<span id=\"" << this->canvasName << "Messages\"></span>"
    ;
  }
  out << "<script language=\"javascript\">\n";
  std::string canvasFunctionName = "functionMake"+ this->canvasName;
  out << "function " << canvasFunctionName << "()\n"
  << "{ ";
  for (int i=0; i<this->boxesThatUpdateMe.size; i++)
  { InputBox& currentBox=owner.theObjectContainer.theUserInputTextBoxesWithValues.
    GetValueCreateIfNotPresent(this->boxesThatUpdateMe[i]);
    out << " calculatorPlotUpdaters['"
    << currentBox.GetSliderName() << "']="
    << "'" << this->canvasName << "'"
    << ";\n";
  }
  int funCounter=0;
  List<std::string> theFnPlots;
  theFnPlots.SetSize(this->thePlots.size);
  for (int i=0; i<this->thePlots.size; i++)
  { PlotObject& currentPlot= this->thePlots[i];
    if (currentPlot.thePlotType=="plotFunction")
    { out << currentPlot.GetJavascript2dPlot
      (theFnPlots[i], this->canvasName, funCounter)
      << "\n "
      ;
    }
    if (currentPlot.thePlotType=="parametricCurve")
    { out << currentPlot.GetJavascriptParametricCurve2D
      (theFnPlots[i], this->canvasName, funCounter)
      << "\n "
      ;
    }
    if (currentPlot.thePlotType=="plotDirectionField")
    { out << currentPlot.GetJavascriptDirectionField
      (theFnPlots[i], this->canvasName, funCounter)
      << "\n "
      ;
    }
    if (currentPlot.thePlotType=="points")
      currentPlot.GetJavascriptPoints
      (theFnPlots[i], this->canvasName, funCounter);
  }
  out << "calculatorResetCanvas(document.getElementById('"
  << this->canvasName << "'));\n";
  out << "var theCanvas=calculatorGetCanvasTwoD(document.getElementById('"
  << this->canvasName
  << "'));\n"
  << "theCanvas.init('" << this->canvasName << "');\n";
  if (owner.flagPlotNoControls)
    out << "theCanvas.flagShowPerformance=false;\n";
  else
    out << "theCanvas.flagShowPerformance=true;\n";
  out.precision(7);
  for (int i=0; i<this->thePlots.size; i++)
  { PlotObject& currentPlot=this->thePlots[i];
    if (currentPlot.thePlotType=="plotFunction")
    { out << "theCanvas." << theFnPlots[i];
      continue;
    }
    if (currentPlot.thePlotType=="parametricCurve")
    { out << "theCanvas." << theFnPlots[i];
      continue;
    }
    if (currentPlot.thePlotType=="plotDirectionField")
    { out << "theCanvas." << theFnPlots[i];
      continue;
    }
    if (currentPlot.thePlotType=="points")
    { out
      << "theCanvas." << theFnPlots[i];
      continue;
    }
    if (currentPlot.thePlotType=="label")
    { if (currentPlot.thePointsDouble.size>0)
        out << "theCanvas.drawText("
        << currentPlot.thePointsDouble[0].ToStringSquareBracketsBasicType()
        << ", "
        << "'" << currentPlot.thePlotString << "'"
        << ", "
        << "'" << currentPlot.colorJS << "');\n" ;
      continue;
    }
    if (currentPlot.thePlotType=="plotFillStart")
    { out << "theCanvas.plotFillStart('" << currentPlot.colorFillJS << "');\n";
      continue;
    }
    if (currentPlot.thePlotType=="plotFillFinish")
    { out << "theCanvas.plotFillFinish();\n";
      continue;
    }
    if (currentPlot.thePlotType=="axesGrid")
    { out << "theCanvas.drawGrid();\n";
      continue;
    }
    if (currentPlot.thePlotType=="pathFilled")
    { out << "theCanvas.drawPathFilled( ";
      out << currentPlot.ToStringPointsList();
      out << ", "
      << "\"" << currentPlot.colorJS << "\""
      << ","
      << "\"" << currentPlot.colorFillJS << "\""
      << ");\n";
      continue;
    }
    out << "theCanvas.drawPath( ";
    out << currentPlot.ToStringPointsList();
    out << ", "
    << "\"" << DrawingVariables::GetColorHtmlFromColorIndex
    (currentPlot.colorRGB) << "\"";
    if (currentPlot.lineWidthJS!="")
      out << ", " << "\"" << currentPlot.lineWidthJS << "\"";
    else
      out << ", " << "\"" << currentPlot.lineWidth << "\"";
    out << ");\n";
  }
  if (this->flagIncludeCoordinateSystem)
  { //out << "theCanvas.drawLine([" << this->theLowerBoundAxes*1.10
    //<< ",0],[" << this->theUpperBoundAxes*1.10 << ",0], 'black',1);\n";
    //out << "theCanvas.drawLine([0," << this->lowBoundY *1.10
    //<< "],[0," << this->highBoundY*1.10 << "], 'black',1);\n";
    //out << "theCanvas.drawLine([1,-0.1],[1,0.1], 'black',1);\n";
    //out << "theCanvas.drawText([1,-0.2],'1','black');\n";
    out << "theCanvas.drawCoordinateAxes();\n";
  }
  //stOutput << "DEBUG: this->priorityViewRectangle=" << this->priorityViewRectangle;
  if (this->priorityViewRectangle>0)
  { out << "theCanvas.setViewWindow("
    << "[" << this->theLowerBoundAxes*1.10 << ", " << this->lowBoundY*1.10 << "]"
    << ", "
    << "[" << this->theUpperBoundAxes*1.10 << ", " << this->highBoundY*1.10 << "]"
    << ");\n";
  } else
    out << "theCanvas.computeViewWindow();\n";
  out
  << "theCanvas.redraw();\n"
  << "}\n"
  << "calculatorGetCanvasTwoD(document.getElementById('"
  << this->canvasName
  << "')).canvasResetFunction=" << canvasFunctionName << ";\n"
  << canvasFunctionName << "();\n"
  << "</script>"
  ;
  return out.str();
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

bool Expression::IsSuitableForSubstitution()const
{ if (this->owner==0)
    return false;
  if(this->IsBuiltInTypE() || this->StartsWith(this->owner->opBind()))
    return false;
  return true;
}

bool Expression::IsSuitableForRecursion()const
{ if (this->owner==0)
    return false;
  if(this->IsAtom() || this->IsBuiltInTypE() ||
     this->StartsWith(this->owner->opBind()))
    return false;
  return true;
}

void Expression::SubstituteRecursively(const Expression& toBeSubbed, const Expression& toBeSubbedWith)
{ if ((*this)==toBeSubbed)
  { (*this)=toBeSubbedWith;
    return;
  }
  this->SubstituteRecursivelyInChildren(toBeSubbed, toBeSubbedWith);
}

void Expression::SubstituteRecursivelyInChildren(const Expression& toBeSubbed, const Expression& toBeSubbedWith)
{ if (!this->IsSuitableForSubstitution())
    return;
  Expression tempE;
  for (int i=0; i<this->size(); i++)
    if (toBeSubbed==(*this)[i])
      this->SetChilD(i, toBeSubbedWith);
    else
    { tempE=(*this)[i];
      tempE.SubstituteRecursivelyInChildren(toBeSubbed, toBeSubbedWith);
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
    if (currentString=="|")
      return output.AssignValue<std::string>("abs ", theCommands);
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
    for (int i=input.size()-1; i>=1; i--)
    { if (!theCommands.innerSuffixNotationForPostScript(theCommands, input[i], currentE))
        return output.MakeError("Failed to convert "+input[i].ToString(), theCommands);
      if (!currentE.IsOfType(&currentString))
        return output.MakeError("Failed to convert "+input[i].ToString(), theCommands);
      out << currentString << " ";
    }
  else
    for (int i=1; i<input.size(); i++)
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
  if (!input.IsSuitableForRecursion())
  { output=input;
    return true;
  }
  if (input[0]=="ReverseRecursively")
  { Expression tempE=input;
    tempE.SetChildAtomValue(0, theCommands.opSequence());
    return theCommands.innerReverseOrderRecursively(theCommands, tempE, output);
  }
  output.reset(theCommands, input.size());
  output.AddChildOnTop(input[0]);
  for (int i=input.size()-1; i>=1; i--)
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
  if (!input.IsSuitableForRecursion())
  { output=input;
    return true;
  }
  if (input[0]=="Reverse")
  { Expression tempE=input;
    tempE.SetChildAtomValue(0, theCommands.opSequence());
    return theCommands.innerReverseOrdeR(theCommands, tempE, output);
  }
  output.reset(theCommands, input.size());
  output.AddChildOnTop(input[0]);
  for (int i=input.size()-1; i>=1; i--)
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
  if (!leftE.IsBuiltInTypE() || !rightE.IsBuiltInTypE())
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
  WeylGroupData& theWeyl=theSSlieAlg->theWeyl;
  if (!theWeyl.theDynkinType.IsSimple())
    return theCommands << "<hr>Function root subsystem works for simple ambient types only.";
  for (int i=2; i<input.size(); i++)
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
  theDiagram.AmbientRootSystem = theWeyl.RootSystem;
  theDiagram.ComputeDiagramInputIsSimple(outputRoots);
  out << "Diagram final: " << theDiagram.ToString()
  << ". Simple basis: " << theDiagram.SimpleBasesConnectedComponents.ToString();
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerPerturbSplittingNormal(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerPerturbSplittingNormal");
  std::stringstream out;
  if (input.size()!=4)
  { out << "Perturbing splitting normal takes 3 arguments: normal, positive vectors, and vectors relative to which to perturb. "
    << "Instead I got " << input.size()-1 << ". ";
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

bool Expression::GetListExpressionsFromExpressionHistoryRecursiveNested(Expression& outputAppend)const
{ MacroRegisterFunctionWithName("Expression::GetListExpressionsFromExpressionHistoryRecursiveNested");
  if (this->owner==0)
  { outputAppend.AddChildOnTop(*this);
    return true;
  }
  if (!this->StartsWith(this->owner->opExpressionHistory()))
  { outputAppend.AddChildOnTop(*this);
    return true;
  }
  if (this->size()==2)
    return true;
  int numTransformationPhases=0;
  for (int i=0; i<this->size(); i++)
    if ((*this)[i].IsSequenceNElementS() && (*this)[i].size()>2)
      if ((*this)[i][1].IsEqualToMOne())
        numTransformationPhases++;
  if (numTransformationPhases<=1)
    return true;
  Expression theTransformations;
  List<Expression> transformationPhases;
  transformationPhases.Reserve(numTransformationPhases);
  for (int i=0; i<this->size(); i++)
    if ((*this)[i].IsSequenceNElementS() && (*this)[i].size()>2)
      if ((*this)[i][1].IsEqualToMOne())
        transformationPhases.AddOnTop((*this)[i]);
  theTransformations.MakeSequence(*this->owner, &transformationPhases);
  outputAppend.AddChildOnTop(theTransformations);
  for (int i=0; i<this->size(); i++)
    if ((*this)[i].IsSequenceNElementS())
      if (!(*this)[i][1].IsEqualToMOne())
        (*this)[i][2].GetListExpressionsFromExpressionHistoryRecursiveNested(outputAppend);
  return true;
}

std::string Expression::ToStringExpressionHistoryRecursiveNested()
{ MacroRegisterFunctionWithName("Expression::ToStringExpressionHistoryRecursiveNested");
  std::stringstream out;
  if (this->owner==0)
    return "(no owner)";
  Expression processedHistory;
  processedHistory.reset(*this->owner);
  processedHistory.AddChildAtomOnTop(this->owner->opEndStatement());
  if (!this->GetListExpressionsFromExpressionHistoryRecursiveNested(processedHistory))
    out << "Failed to extract expression tree. ";
  else
  { for (int i=1; i<processedHistory.size(); i++)
    { out << "Step " << i << ": \\(\\begin{array}{ll|l}&";
      for (int j=1; j<processedHistory[i].size(); j++)
      { if (j>1)
          out << "=&";
        out << processedHistory[i][j][2].ToString();
        if (processedHistory[i][j].size()>3 )
        { std::string theString=processedHistory[i][j][3].ToString();
          if (theString!="")
            out << "&\\text{" << theString << "}";
        }
        out << "\\\\";
      }
      out << "\\end{array}\\)<hr>";
    }
    //out << "DEBUG: expression history: this->ToString(): <br>"
    //<< this->ToString();
  }
  return out.str();
}

void Calculator::ExpressionHistoryAddEmptyHistory()
{ MacroRegisterFunctionWithName("Calculator::ExpressionHistoryAddEmptyHistory");
  ListReferences<Expression>& currentExpressionHistoryStack=this->ExpressionHistoryStack.LastObject();
  currentExpressionHistoryStack.SetSize(currentExpressionHistoryStack.size+1);
  Expression& currentExpressionHistory= currentExpressionHistoryStack.LastObject();
  currentExpressionHistory.reset(*this);
  currentExpressionHistory.AddChildAtomOnTop(this->opExpressionHistory());
  this->ExpressionHistoryRuleNames.LastObject().SetSize(currentExpressionHistoryStack.size);
  this->ExpressionHistoryRuleNames.LastObject().LastObject()=(std::string) "";
}

void Calculator::ExpressionHistoryAdd
(Expression& theExpression, int expressionLabel)
{ MacroRegisterFunctionWithName("Calculator::ExpressionHistoryAdd");
  Expression theHistoryE;
  theHistoryE.reset(*this);
  theHistoryE.AddChildAtomOnTop(this->opSequence());
  Expression indexE;
  indexE.AssignValue(expressionLabel, *this);
  theHistoryE.AddChildOnTop(indexE);
  theHistoryE.AddChildOnTop(theExpression);
  Expression ruleNameE;
  ruleNameE.AssignValue(this->ExpressionHistoryRuleNames.LastObject().LastObject() , *this);
  theHistoryE.AddChildOnTop(ruleNameE);
  this->ExpressionHistoryRuleNames.LastObject().LastObject()="";
  this->ExpressionHistoryStack.LastObject().LastObject().AddChildOnTop(theHistoryE);
}

void Calculator::ExpressionHistoryPop()
{ MacroRegisterFunctionWithName("Calculator::ExpressionHistoryPop");
  this->ExpressionHistoryStack.LastObject().SetSize(this->ExpressionHistoryStack.LastObject().size-1);
  this->ExpressionHistoryRuleNames.LastObject().SetSize(this->ExpressionHistoryStack.LastObject().size);
}

void Calculator::ExpressionHistoryStackAdd()
{ MacroRegisterFunctionWithName("Calculator::ExpressionHistoryStackAdd");
  this->ExpressionHistoryStack.SetSize(this->ExpressionHistoryStack.size+1);
  this->ExpressionHistoryStack.LastObject().SetSize(0);
  this->ExpressionHistoryRuleNames.SetSize(this->ExpressionHistoryStack.size);
  this->ExpressionHistoryRuleNames.LastObject().SetSize(0);
}

void Calculator::ExpressionHistoryStackPop()
{ MacroRegisterFunctionWithName("Calculator::ExpressionHistoryStackPop");
  this->ExpressionHistoryStack.SetSize(this->ExpressionHistoryStack.size-1);
  this->ExpressionHistoryRuleNames.SetSize(this->ExpressionHistoryStack.size);
}

bool Calculator::innerLogEvaluationStepsHumanReadableNested(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerLogEvaluationStepsHumanReadableNested");
  Expression inputCopy=input;
  Expression outputTransformation;
  if (inputCopy.StartsWithGivenAtom("LogEvaluationStepsHumanReadableNested"))
    inputCopy.SetChildAtomValue(0, theCommands.opSequence());
  bool notUsed=false;
  theCommands.ExpressionHistoryStackAdd();
  theCommands.EvaluateExpression(theCommands, inputCopy, outputTransformation, notUsed);
  std::stringstream out;
  ListReferences<Expression>& currentStack=theCommands.ExpressionHistoryStack.LastObject();
  for (int i=0; i<currentStack.size; i++)
  { out << "Stack " << i+1 << ":<br>" << currentStack[i].ToStringExpressionHistoryRecursiveNested();
    if (i!=currentStack.size-1)
      out << "<hr>";
  }
  theCommands.ExpressionHistoryStackPop();
  return output.AssignValue(out.str(), theCommands);
}

#include "vpfHeader2Math4_5_Tree.h"

struct HistorySubExpression{
public:
  friend std::ostream& operator << (std::ostream& output, const HistorySubExpression& theH)
  { if (theH.currentE==0)
      output << "(no expression)";
    else
      output << theH.currentE->ToString();
    output << " active: " << theH.lastActiveSubexpression;
    //output << "<br>Lispified: " << theH.currentE->ToStringSemiFull();
    return output;
  }
  const Expression* currentE;
  int lastActiveSubexpression;
  HistorySubExpression(): currentE(0), lastActiveSubexpression(-1)
  {
  }
};

class ExpressionHistoryEnumerator{
public:
  Expression theHistory;
  Expression currentState;
  Tree<HistorySubExpression> currentSubTree;
  List<Expression> output;
  Calculator* owner;
  bool initialized;
  bool IncrementReturnFalseIfPastLast();
  bool initialize();
  std::stringstream errorStream;
  const Expression* getCurrentE(const List<Expression>& input);
  ExpressionHistoryEnumerator()
  { this->initialized=false;
    this->owner=0;
  }
  bool IncrementRecursivelyReturnFalseIfPastLast(TreeNode<HistorySubExpression>& currentNode);
  bool CheckInitialization()
  { if (this->owner==0)
      crash << "Expression history enumerator has zero owner. " << crash;
    return true;
  }
  Expression GetExpression(TreeNode<HistorySubExpression>& currentNode);
  void ComputeAll();
};

bool ExpressionHistoryEnumerator::initialize()
{ MacroRegisterFunctionWithName("ExpressionHistoryEnumerator::initialize");
  if (this->initialized)
    return true;
  this->initialized=true;
  this->CheckInitialization();
  if (!this->theHistory.StartsWith(this->owner->opExpressionHistory()) || this->theHistory.size()<2)
  { this->errorStream << "Expression history does not start with the right atom. ";
    return false;
  }
  if (!this->theHistory[1].StartsWith(this->owner->opSequence()) || this->theHistory[1].size()<3)
  { this->errorStream << "Expression history element is not a sequence. ";
    return false;
  }
  if (!this->theHistory[1][1].IsEqualToMOne())
  { this->errorStream << "Expression history element at start is not labeled by -1. ";
    return false;
  }
  this->currentSubTree.reset();
  HistorySubExpression currentNode;
  currentNode.currentE=&this->theHistory;
  currentNode.lastActiveSubexpression=1;
  this->currentSubTree.ResetAddRoot(currentNode);
  currentNode.currentE=&this->theHistory[1][2];
  currentNode.lastActiveSubexpression=-1;
  this->currentSubTree.theNodes[0].AddChild(currentNode);
  return true;
}

Expression ExpressionHistoryEnumerator::GetExpression(TreeNode<HistorySubExpression>& currentNode)
{ MacroRegisterFunctionWithName("ExpressionHistoryEnumerator::GetExpression");
  Expression result;
  if (currentNode.children.size==0)
  { result=*currentNode.theData.currentE;
    return result;
  }
  if (currentNode.children.size==1)
  { result=this->GetExpression(currentNode.GetChild(0));
    return result;
  }
  result.reset(*this->owner);
  for (int i=0; i<currentNode.children.size; i++)
  { Expression currentE=this->GetExpression(currentNode.GetChild(i)  );
    stOutput << "<br>Adding expression: " << currentE.ToStringFull();
    result.AddChildOnTop(currentE);
  }
  stOutput << "<br>Final expression: " << result.ToStringFull();
  return result;
}

void ExpressionHistoryEnumerator::ComputeAll()
{ MacroRegisterFunctionWithName("ExpressionHistoryEnumerator::ComputeAll");
  while (this->IncrementReturnFalseIfPastLast())
  { this->output.AddOnTop(this->GetExpression(this->currentSubTree.theNodes[0]));
    stOutput << "<br>DEBUG:<br>" << this->currentSubTree.theNodes[0].ToStringTextFormat(0)
    ;
  }
}

bool ExpressionHistoryEnumerator::IncrementRecursivelyReturnFalseIfPastLast(TreeNode<HistorySubExpression>& currentNode)
{ MacroRegisterFunctionWithName("ExpressionHistoryEnumerator::IncrementRecursivelyReturnFalseIfPastLast");
  bool didWork=false;
  for (int i=0; i<currentNode.children.size; i++)
    if (this->IncrementRecursivelyReturnFalseIfPastLast(this->currentSubTree.theNodes[currentNode.children[i]]))
      didWork=true;
  if (didWork)
    return true;
  if (!currentNode.theData.currentE->StartsWith(this->owner->opExpressionHistory()) )
    return false;
  const Expression& currentE=*currentNode.theData.currentE;
  HistorySubExpression nextChild;
  while (!didWork)
  { currentNode.RemoveAllChildren();
    if (currentNode.theData.lastActiveSubexpression==currentE.size()-1)
      return false;
    int nextGoodIndex=currentNode.theData.lastActiveSubexpression+1;
    for (;nextGoodIndex<currentE.size(); nextGoodIndex++)
      if (currentE[nextGoodIndex].IsSequenceNElementS() && currentE[nextGoodIndex].size()>2)
      { if (currentE[nextGoodIndex][1].IsEqualToMOne())
          if (currentNode.children.size>0)
            break;
        nextChild.currentE=&currentE[nextGoodIndex][2];
        nextChild.lastActiveSubexpression=0;
        currentNode.AddChild(nextChild);
        currentNode.theData.lastActiveSubexpression=nextGoodIndex;
        if (currentE[nextGoodIndex][1].IsEqualToMOne())
        { didWork=true;
          break;
        }
      }
    for (int i=0; i<currentNode.children.size; i++)
      if (this->IncrementRecursivelyReturnFalseIfPastLast(this->currentSubTree.theNodes[currentNode.children[i]]))
        didWork=true;
    if (nextGoodIndex>=currentE.size())
      break;
   }
  return didWork;
}

bool ExpressionHistoryEnumerator::IncrementReturnFalseIfPastLast()
{ MacroRegisterFunctionWithName("ExpressionHistoryEnumerator::IncrementReturnFalseIfPastLast");
  if (!this->initialized)
    return this->initialize();
  this->CheckInitialization();
  if (this->currentSubTree.theNodes.size<1)
    return false;
  bool result=this->IncrementRecursivelyReturnFalseIfPastLast(this->currentSubTree.theNodes[0]);
  return result;
}

bool Calculator::innerLogEvaluationStepsHumanReadableMerged(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerLogEvaluationStepsHumanReadableMerged");
  Expression inputCopy=input;
  Expression outputTransformation;
  if (inputCopy.StartsWithGivenAtom("LogEvaluationStepsHumanReadableMerged"))
    inputCopy.SetChildAtomValue(0, theCommands.opSequence());
  bool notUsed=false;
  theCommands.ExpressionHistoryStackAdd();
  theCommands.EvaluateExpression(theCommands, inputCopy, outputTransformation, notUsed);
  std::stringstream out;
  ListReferences<Expression>& currentStack=theCommands.ExpressionHistoryStack.LastObject();
  for (int i=0; i<currentStack.size; i++)
  { if (currentStack.size>1)
      out << "Stack " << i+1 << ":<br>";
    ExpressionHistoryEnumerator theHistoryEnumerator;
    theHistoryEnumerator.theHistory=currentStack[i];
    theHistoryEnumerator.owner=&theCommands;
    theHistoryEnumerator.ComputeAll();
    out << theHistoryEnumerator.errorStream.str();
    for (int j=0; j<theHistoryEnumerator.output.size; j++)
      out << theHistoryEnumerator.output[j].ToString() << "<hr>";
    out << "DEBUG: " << theHistoryEnumerator.theHistory.ToString();
    if (i!=currentStack.size-1)
      out << "<hr>";
  }
  theCommands.ExpressionHistoryStackPop();
  return output.AssignValue(out.str(), theCommands);
}
