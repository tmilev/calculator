//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfImplementationHeaderPolyComputationsAdvanced_already_included
#define vpfImplementationHeaderPolyComputationsAdvanced_already_included

#include "vpfImplementationHeader2Math051_PolynomialComputations_Basic.h"
#include "vpfHeader2Math2_AlgebraicNumbers.h"
static ProjectInformationInstance ProjectInfovpfImplementationHeaderPolynomialComputations(__FILE__, "Implementation header, polynomial computations, more advanced. ");

template <class coefficient>
bool GroebnerBasisComputation<coefficient>::TransformToReducedGroebnerBasis(List<Polynomial<coefficient> >& inputOutpuT, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("GroebnerBasisComputation::TransformToReducedGroebnerBasis");
  //stOutput << "<hr>Processing" << inputOutpuT.ToString() << ".<br>";
  this->initForGroebnerComputation(inputOutpuT, theGlobalVariables);
  this->basisCandidates=inputOutpuT;
  ProgressReport theReport(theGlobalVariables);
 // std::string tempS;
//  bool changed=true;
  this->flagBasisGuaranteedToGenerateIdeal=false;
  int oldMaxNumComputations=this->MaxNumGBComputations;
  this->MaxNumGBComputations=-1;
  this->AddPolyAndReduceBasis(theGlobalVariables);
  this->MaxNumGBComputations=oldMaxNumComputations;
  this->flagBasisGuaranteedToGenerateIdeal=true;

  if (this->theBasiS.size==1)
  { inputOutpuT=this->theBasiS;
    return true;
  }
  bool changed=true;
  while (changed)
  { changed=false;
    for (int i=0; i<this->theBasiS.size; i++)
      for (int j=i+1; j<this->theBasiS.size && i<this->theBasiS.size; j++)
      { Polynomial<coefficient>& currentLeft= this->theBasiS[i];
        Polynomial<coefficient>& currentRight= this->theBasiS[j];
        int leftIndex=currentLeft.GetIndexMaxMonomial(this->thePolynomialOrder.theMonOrder);
        int rightIndex=currentRight.GetIndexMaxMonomial(this->thePolynomialOrder.theMonOrder);
        const MonomialP& leftHighestMon=currentLeft[leftIndex];
        const MonomialP& rightHighestMon=currentRight[rightIndex];
        int numVars=MathRoutines::Maximum(leftHighestMon.GetMinNumVars(), rightHighestMon.GetMinNumVars());
        this->SoPolyLeftShift.MakeOne(numVars);
        this->SoPolyRightShift.MakeOne(numVars);
        for (int k=0; k<numVars; k++)
          if (leftHighestMon(k)>rightHighestMon(k))
          { this->SoPolyRightShift[k]=leftHighestMon(k)-rightHighestMon(k);
            this->SoPolyLeftShift[k]=0;
          } else
          { this->SoPolyLeftShift[k]=rightHighestMon(k)-leftHighestMon(k);
            this->SoPolyRightShift[k]=0;
          }
        if (theGlobalVariables!=0 && this->flagDoProgressReport)
        { std::stringstream out;
          out << "Computing Sopoly of indices " << i +1 << " out of " << this->theBasiS.size << " and " << j+1 << " out of " << this->theBasiS.size;
          theReport.Report(out.str());
        }
        this->bufPoly=currentLeft;
        this->bufPoly.MultiplyBy(this->SoPolyLeftShift, currentRight.theCoeffs[rightIndex]);
        this->SoPolyBuf=currentRight;
        this->SoPolyBuf.MultiplyBy(this->SoPolyRightShift, currentLeft.theCoeffs[leftIndex]);
        this->SoPolyBuf-=(this->bufPoly);
        this->basisCandidates.AddOnTop(this->SoPolyBuf);
        this->NumberGBComputations++;
        if (this->MaxNumGBComputations>0)
          if (this->NumberGBComputations>this->MaxNumGBComputations)
            return false;
//        if (this->AddPolyAndReduceBasis(theGlobalVariables))
//        { i=0;
//          j=-1;
//          changed=true;
//        }
      }
    this->flagBasisGuaranteedToGenerateIdeal=true;
    changed=this->AddPolyAndReduceBasis(theGlobalVariables);
    this->flagBasisGuaranteedToGenerateIdeal=true;
    if (this->MaxNumGBComputations>0)
      if (this->NumberGBComputations>this->MaxNumGBComputations)
        return false;
  }
//  this->MakeMinimalBasis();
  this->theBasiS.QuickSortAscendingCustom(this->thePolynomialOrder);
  inputOutpuT=this->theBasiS;
  return true;
}

template<class coefficient>
bool GroebnerBasisComputation<coefficient>::AddPolyAndReduceBasis(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("GroebnerBasisComputation::AddPolyAndReduceBasis");
  bool changed=false;
  ProgressReport theReport(theGlobalVariables);
  ProgressReport theReport2(theGlobalVariables);
  if (theGlobalVariables!=0)
  { std::stringstream out;
    out << "Reducing: " << theBasiS.size << " basis elements, " << this->basisCandidates.size << " candidates. ";
//    out << "Reducing basis. Current reduced basis has " << this->theBasiS.size << " elements: <br>" << this->theBasiS.ToString()
//    << "<br> and there are " << this->basisCandidates.size << " polynomials to adjoin."; //<br> " << this->basisCandidates.ToString();
    theReport2.Report(out.str());
  }
  while (this->basisCandidates.size>0)
  { bool addedNew=false;
    for (int i=this->basisCandidates.size-1; i>=0; i--)
      if (this->basisCandidates[i].IsEqualToZero())
        this->basisCandidates.RemoveIndexSwapWithLast(i);
    while (this->basisCandidates.size>0)
    { int indexElementMinMaxMon=this->basisCandidates.size-1;
      MonomialP maxMon=this->basisCandidates[indexElementMinMaxMon].GetMaxMonomial();
      for (int i=this->basisCandidates.size-2; i>=0; i--)
        if (maxMon> this->basisCandidates[i].GetMaxMonomial())
        { maxMon=this->basisCandidates[i].GetMaxMonomial();
          indexElementMinMaxMon=i;
        }
      this->RemainderDivisionWithRespectToBasis
      (this->basisCandidates[indexElementMinMaxMon], &this->remainderDivision, theGlobalVariables);
      this->basisCandidates.RemoveIndexSwapWithLast(indexElementMinMaxMon);
      if(this->AddRemainderToBasis(theGlobalVariables))
      { changed=true;
        addedNew=true;
      }
      this->NumberGBComputations++;
      if (this->MaxNumGBComputations>0)
        if (this->NumberGBComputations>this->MaxNumGBComputations)
        { if (!this->flagBasisGuaranteedToGenerateIdeal)
            this->theBasiS.AddListOnTop(this->basisCandidates);
          this->CheckConsistency();
          return true;
        }
      if (theGlobalVariables!=0 && this->flagDoProgressReport)
      { std::stringstream out;
        out << "Reducing: " << theBasiS.size << " basis elements, " << this->basisCandidates.size << " candidates. ";
        theReport.Report(out.str());
//          stOutput << "<br>" << out.str();
      }
    }
    this->flagBasisGuaranteedToGenerateIdeal=true;
    if (!addedNew)
      break;
    for (int i=0; i<this->theBasiS.size; i++)
    { this->bufPolyForGaussianElimination=this->theBasiS[i];
      if (theGlobalVariables!=0 && this->flagDoProgressReport)
      { std::stringstream out;
        out << "Verifying basis element " << i+1 << " out of " << theBasiS.size << " basis elements, " << this->basisCandidates.size << " candidates. ";
        theReport.Report(out.str());
//        stOutput << "<br>" << out.str();
      }
      this->NumberGBComputations++;
      this->RemainderDivisionWithRespectToBasis(this->bufPolyForGaussianElimination, &this->remainderDivision, theGlobalVariables, i);
      if (this->MaxNumGBComputations>0)
        if (this->NumberGBComputations>this->MaxNumGBComputations)
        { this->CheckConsistency();
          return true;
        }
      if (!(this->remainderDivision==this->theBasiS[i]))
      { this->flagBasisGuaranteedToGenerateIdeal=false;
        this->basisCandidates.AddOnTop(this->remainderDivision);
        this->leadingMons.RemoveIndexShiftDown(i);
        this->leadingCoeffs.RemoveIndexShiftDown(i);
        this->theBasiS.RemoveIndexShiftDown(i);
        i--;
        changed=true;
      }
    }
  }
  return changed;
}

template<class coefficient>
void GroebnerBasisComputation<coefficient>::MakeMinimalBasis()
{ MacroRegisterFunctionWithName("GroebnerBasisComputation::MakeMinimalBasis");
/*  stOutput << "<br><br> and the leading monomials are: ";
  for (int i=0; i<LeadingCoeffs.size; i++)
    stOutput << LeadingCoeffs[i].ToString() << ", ";*/
  for (int i=0; i<this->theBasiS.size; i++)
    for (int j=0; j<this->leadingMons.size; j++)
      if (i!=j)
        if (this->leadingMons[i].IsDivisibleBy(this->leadingMons[j]))
        { /*stOutput << "<br>" << LeadingCoeffs[i].ToString() << " is divisible by "
          << LeadingCoeffs[j].ToString();*/
          this->leadingMons.RemoveIndexSwapWithLast(i);
          this->theBasiS.RemoveIndexSwapWithLast(i);
          i--;
          break;
        } //else
        //{ stOutput << "<br>" << LeadingCoeffs[i].ToString() << " is NOT divisible by "
        //  << LeadingCoeffs[j].ToString();
        //
/*  stOutput << "<br><br> final leading monomials are: ";
  for (int i=0; i<LeadingCoeffs.size; i++)
    stOutput <<"<br>" << LeadingCoeffs[i].ToString() << " of " << theBasis[i].ToString();
*/
}

template<class coefficient>
bool GroebnerBasisComputation<coefficient>::CriterionCLOsh
(HashedListSpecialized<Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity> >&thePairs, List<MonomialP>& theLeadingMons, MonomialP& leadingTermLCM)
{ //page 107,  Cox, Little, O'Shea,
  //Ideals, Varieties, algorithms
  Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity >&
  lastPair=*thePairs.LastObject();
  Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity >
  pairBeingTested1, pairBeingTested2;
  for (int k=0; k<theLeadingMons.size; k++)
    if (k!=lastPair.Object1 && k!=lastPair.Object2)
      if (leadingTermLCM.IsDivisibleBy(theLeadingMons[k]))
      { pairBeingTested1.Object1=MathRoutines::Minimum(lastPair.Object1, k);
        pairBeingTested1.Object2=MathRoutines::Maximum(lastPair.Object1, k);
        pairBeingTested2.Object1=MathRoutines::Minimum(lastPair.Object2, k);
        pairBeingTested2.Object2=MathRoutines::Maximum(lastPair.Object2, k);
        if (!thePairs.Contains(pairBeingTested1) && !thePairs.Contains(pairBeingTested2))
          return true;
      }
  return false;
}

template<class coefficient>
bool GroebnerBasisComputation<coefficient>::TransformToReducedGroebnerBasisImprovedAlgorithm(List<Polynomial<coefficient> >& inputOutpuT, GlobalVariables* theGlobalVariables, int upperComputationBound)
{ MacroRegisterFunctionWithName("RationalFunction_CoefficientType::TransformToReducedGroebnerBasisImprovedAlgorithm");
   //This is an implementation of the algorithm on page 106, Cox, Little, O'Shea,
  //Ideals, Varieties, algorithms
  this->initForGroebnerComputation(inputOutpuT, theGlobalVariables);
  this->theBasiS=inputOutpuT;
  HashedListSpecialized<PairInts> indexPairs;
//  Pair<int, int> currentPair;
  indexPairs.SetExpectedSize(this->theBasiS.size*this->theBasiS.size);
  this->leadingMons.SetExpectedSize(this->theBasiS.size*2);
  for (int i=0; i<this->theBasiS.size; i++)
  { for (int j=i+1; j<this->theBasiS.size; j++)
      indexPairs.AddOnTop(PairInts (i,j));
    this->theBasiS[i].ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
    int theIndex=this->theBasiS[i].GetIndexMaxMonomial(this->theMonOrdeR);
    this->leadingMons.AddOnTop(this->theBasiS[i][theIndex]);
    this->leadingCoeffs.AddOnTop(this->theBasiS[i].theCoeffs[theIndex]);
  }
  if (this->theBasiS.size<=0)
    crash << "This is a programming error: transforming to Groebner basis not allowed for empty basis. " << crash;
  MonomialP leftShift, rightShift, monLCM;
  Polynomial<Rational> leftBuf, rightBuf, buffer1;
  Polynomial<Rational>& outputRemainder=rightBuf; //to save some RAM
  ProgressReport reportOuter(theGlobalVariables);
  ProgressReport reportInner(theGlobalVariables);
  while (indexPairs.size>0)
  { PairInts& lastPair=*indexPairs.LastObject();
    int currentPairIndex=indexPairs.size-1;
    bool isGood=false;
    Polynomial<Rational>& currentLeft = this->theBasiS[lastPair.Object1];
    Polynomial<Rational>& currentRight= this->theBasiS[lastPair.Object2];
    MonomialP& leftHighestMon= this->leadingMons[lastPair.Object1];
    MonomialP& rightHighestMon=this->leadingMons[lastPair.Object2];
    if (theGlobalVariables!=0 && this->flagDoProgressReport)
    { std::stringstream out;
      out << "Basis size: " << this->theBasiS.size << ".\n<br>Remaining cases current round: " << indexPairs.size;
      reportOuter.Report(out.str());
    }
    int numVars=MathRoutines::Maximum(leftHighestMon.GetMinNumVars(), rightHighestMon.GetMinNumVars());
    leftShift.MakeOne(numVars);
    rightShift.MakeOne(numVars);
    for (int k=0; k<numVars; k++)
    { if (leftHighestMon[k]>0 && rightHighestMon[k]>0)
        isGood=true;
      if (leftHighestMon[k]>rightHighestMon[k])
      { rightShift[k]=leftHighestMon[k]-rightHighestMon[k];
        leftShift[k]=0;
      } else
      { leftShift[k]=rightHighestMon[k]-leftHighestMon[k] ;
        rightShift[k]=0;
      }
    }
    monLCM=leftHighestMon;
    monLCM*=leftShift;
    if (isGood)
      if (!this->CriterionCLOsh(indexPairs, this->leadingMons, monLCM))
      { leftBuf=currentLeft;
        rightBuf=currentRight;
        leftBuf.MultiplyBy(leftShift, this->leadingCoeffs[lastPair.Object2]);
        rightBuf.MultiplyBy(rightShift, this->leadingCoeffs[lastPair.Object1]);
        leftBuf-=rightBuf;
//        stOutput << "<br>testing sopoly: "
//        << leftBuf.ToString(&theGlobalVariables->theDefaultFormat)
//        ;
        this->RemainderDivisionWithRespectToBasis(leftBuf, &outputRemainder, theGlobalVariables);
        if (theGlobalVariables!=0 && this->flagDoProgressReport)
        { std::stringstream out;
          out << "Basis size: " << this->theBasiS.size << ".\n<br>Remaining cases current round: " << indexPairs.size;
          reportOuter.Report(out.str());
        }
//        stOutput << "<br>and sopoly divided by "
//        << theBasis.ToString(&theGlobalVariables->theDefaultFormat) << " is "
//        << outputRemainder.ToString(&theGlobalVariables->theDefaultFormat);
        if (!outputRemainder.IsEqualToZero())
        { outputRemainder.ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
          this->theBasiS.AddOnTop(outputRemainder);
          int theIndexMaxMon=this->theBasiS.LastObject()->GetIndexMaxMonomial(this->theMonOrdeR);
          this->leadingMons.AddOnTop((*this->theBasiS.LastObject())[theIndexMaxMon]);
          this->leadingCoeffs.AddOnTop(this->theBasiS.LastObject()->theCoeffs[theIndexMaxMon]);
          for (int i=0; i<this->theBasiS.size-1; i++)
            indexPairs.AddOnTop(PairInts(i, this->theBasiS.size-1));
        }
      } //else
        //stOutput << "<br>" << leftHighestMon.ToString(&theGlobalVariables->theDefaultFormat)
        //<< " and " << rightHighestMon.ToString(&theGlobalVariables->theDefaultFormat)
        //<< " failed the closh criterium";
    //else
      //stOutput << "<br>" << leftHighestMon.ToString(&theGlobalVariables->theDefaultFormat)
      //<< " and " << rightHighestMon.ToString(&theGlobalVariables->theDefaultFormat)
      //<< " failed the lcm criterium";
    indexPairs.RemoveIndexSwapWithLast(currentPairIndex);
  }
  this->MakeMinimalBasis();
  inputOutpuT=this->theBasiS;
  return true;
}

template <class coefficient>
int GroebnerBasisComputation<coefficient>::GetNumVars()const
{ MacroRegisterFunctionWithName("GroebnerBasisComputation::GetNumVars");
  int result=0;
  for (int i=0; i<this->theBasiS.size; i++)
    for (int j=0; j<this->theBasiS[i].size(); j++)
    { const MonomialP& currentMon=this->theBasiS[i][j];
      result=MathRoutines::Maximum(currentMon.GetMinNumVars(), result);
    }
  return result;
}

template <class coefficient>
std::string GroebnerBasisComputation<coefficient>::ToStringLetterOrder
()const
{ MacroRegisterFunctionWithName("GroebnerBasisComputation::ToStringLetterOrder");
  std::stringstream out;
  int numVars=this->GetNumVars();
  List<MonomialP> theVars;
  out << "Variable name(s): ";
  theVars.SetSize(numVars);
  for (int i=0; i<theVars.size; i++)
    theVars[i].MakeEi(i, 1);
  theVars.QuickSortAscending(this->thePolynomialOrder.theMonOrder);
  FormatExpressions tempFormat=this->theFormat;
  for (int i=0; i<numVars; i++)
  { out << theVars[i].ToString(&tempFormat);
    if (i!=numVars-1)
      out << " < ";
  }
  return out.str();
}

template <class coefficient>
std::string GroebnerBasisComputation<coefficient>::GetDivisionString()
{ std::stringstream out;
  List<Polynomial<Rational> >& theRemainders=this->intermediateRemainders.GetElement();
  List<Polynomial<Rational> >& theSubtracands=this->intermediateSubtractands.GetElement();
  this->theFormat.thePolyMonOrder=this->thePolynomialOrder.theMonOrder;
  HashedList<MonomialP> totalMonCollection;
  std::string underlineStyle=" style=\"white-space: nowrap; border-bottom:1px solid black;\"";
  std::string underlineStyleHighlighted=" style=\"textcolor=red; white-space: nowrap; border-bottom:1px solid black;\"";
  std::string HighlightedStyle=" style=\"textcolor=red; white-space: nowrap;\"";
  totalMonCollection.AddOnTopNoRepetition(this->startingPoly.GetElement().theMonomials);
  for (int i=0; i<theRemainders.size; i++)
  { totalMonCollection.AddOnTopNoRepetition(theRemainders[i].theMonomials);
    totalMonCollection.AddOnTopNoRepetition(theSubtracands[i].theMonomials);
  }
  //List<std::string> basisColorStyles;
  //basisColorStyles.SetSize(this->theBasiS.size);
  totalMonCollection.QuickSortDescending(this->thePolynomialOrder.theMonOrder);
//  stOutput << "<hr>The monomials in play ordered: " << totalMonCollection.ToString(theFormat);
//  int numVars=this->GetNumVars();
  out << this->ToStringLetterOrder();
  out << "<br>";
  out << theRemainders.size << " division steps total.<br>";
  out << "<table style=\"white-space: nowrap; border:1px solid black;\">";
  out << "<tr><td " << underlineStyle << "><b>Remainder:</b></td>";
  out << this->GetPolynomialStringSpacedMonomials(this->remainderDivision, totalMonCollection, underlineStyle, HighlightedStyle, &this->remainderDivision.theMonomials)
  << "</td></tr>";
  out << "<tr><td style=\"border-right:1px solid black;\"><b>Divisor(s)</b></td><td colspan=\"" << totalMonCollection.size+1 << "\"><b>Quotient(s) </b></td>"
  << "</tr>";
  for (int i=0; i<this->theBasiS.size; i++)
  { //if (i==this->theBasiS.size-1)
//    else
    out << "<tr>";
    out << "<td style=\"border-right:1px solid black; border-bottom: 1px solid gray;\">";
    if (this->theFormat.flagUseLatex)
      out << CGI::GetMathSpanPure(this->theBasiS[i].ToString(&this->theFormat),-1 );
    else
      out << this->theBasiS[i].ToString(&this->theFormat);
    out << "</td>";
    out << "<td style=\"border-bottom:1px solid gray;\" colspan=\""
    << totalMonCollection.size+1 << "\">";
    out << "<table><tr>";
    bool found=false;
    for (int j=0; j<theRemainders.size; j++)
    { if (this->intermediateSelectedDivisors.GetElement()[j]!=i)
      { out << "<td></td>";
        continue;
      }
      out << "<td>";
      if (this->theFormat.flagUseLatex)
        out << CGI::GetMathSpanPure( Polynomial<Rational>::GetBlendCoeffAndMon
        (this->intermediateHighestMonDivHighestMon.GetElement()[j], this->intermediateCoeffs.GetElement()[j], found, &this->theFormat),-1);
      else
        out << Polynomial<Rational>::GetBlendCoeffAndMon
        (this->intermediateHighestMonDivHighestMon.GetElement()[j], this->intermediateCoeffs.GetElement()[j], found, &this->theFormat);
      found=true;
      out << "</td>";
    }
    out << "</tr></table>";
    out << "</td></tr>";
  }
  out << "<tr><td style=\"border-right:1px solid black;\"></td><td colspan=\"" << totalMonCollection.size+1
  << "\"><b>Divident </b></td>" << "</tr>";
  out << "<tr><td style=\"border-right:1px solid black;\"></td>";
  out << this->GetPolynomialStringSpacedMonomials
  (this->startingPoly.GetElement(), totalMonCollection, "", HighlightedStyle, &this->intermediateHighlightedMons.GetElement()[0]);
  out << "</tr>";
  for (int i=0; i<theRemainders.size; i++)
  { out << "<tr><td>-</td></tr>";
    out << "<tr><td></td>" << this->GetPolynomialStringSpacedMonomials(theSubtracands[i], totalMonCollection, underlineStyle, underlineStyleHighlighted)
    << "</tr>";
    out << "<tr><td></td>"
    << this->GetPolynomialStringSpacedMonomials(theRemainders[i], totalMonCollection, "", HighlightedStyle, &this->intermediateHighlightedMons.GetElement()[i+1])
    << "</tr>";
  }
  out << "</table>";
  return out.str();
}

template <class coefficient>
void GroebnerBasisComputation<coefficient>::RemainderDivisionWithRespectToBasis
(Polynomial<coefficient>& inputOutput, Polynomial<coefficient>* outputRemainder, GlobalVariables* theGlobalVariables, int basisIndexToIgnore)
{ //Reference: Cox, Little, O'Shea, Ideals, Varieties and Algorithms, page 62
  MacroRegisterFunctionWithName("GroebnerBasisComputation::RemainderDivisionWithRespectToBasis");
  if (&inputOutput==outputRemainder || &inputOutput==&this->bufPoly || outputRemainder==&this->bufPoly)
    crash << "This is a programming error: the input, the output and the buffer member object must be pairwise distinct when carrying out "
    << " multi-polynomial division. " << crash;
  MemorySaving<Polynomial<coefficient> > tempPoly;
  if (outputRemainder==0)
    outputRemainder=&tempPoly.GetElement();
  ProgressReport theReportStart(theGlobalVariables);
  ProgressReport theReport(theGlobalVariables);
  if (theGlobalVariables!=0 && this->flagDoProgressReport)
  { theReportStart.Report("Computing remainder  division");
    //+     inputOutput.ToString(&theGlobalVariables->theDefaultFormat) + " mod " +theBasiS.ToString(&theGlobalVariables->theDefaultFormat));
  }
  outputRemainder->MakeZero();
  Polynomial<coefficient>& currentRemainder=inputOutput;
  if (this->flagDoLogDivision)
    this->startingPoly.GetElement()=currentRemainder;
  coefficient leadingMonCoeff;
  MonomialP& highestMonCurrentDivHighestMonOther=this->bufferMoN1;
  int numIntermediateRemainders=0;
  if (this->flagDoLogDivision)
  { this->intermediateCoeffs.GetElement().size=0;
    this->intermediateHighestMonDivHighestMon.GetElement().size=0;
    this->intermediateRemainders.GetElement().size=0;
    this->intermediateSubtractands.GetElement().size=0;
    this->intermediateHighlightedMons.GetElement().SetSize(1);
    this->intermediateHighlightedMons.GetElement().LastObject()->SetSize(0);
  }
  while (!currentRemainder.IsEqualToZero())
  { bool divisionOcurred=false;
    int i=0;
    int indexLeadingMonRemainder= currentRemainder.GetIndexMaxMonomial(this->thePolynomialOrder.theMonOrder);
    leadingMonCoeff=currentRemainder.theCoeffs[indexLeadingMonRemainder];
    highestMonCurrentDivHighestMonOther=currentRemainder[indexLeadingMonRemainder];
    while (i<this->theBasiS.size && !divisionOcurred)
    { MonomialP& highestMonBasis=this->leadingMons[i];
      bool shouldDivide=(i==basisIndexToIgnore) ? false :highestMonCurrentDivHighestMonOther.IsDivisibleBy(highestMonBasis);
      if (shouldDivide)
      { numIntermediateRemainders++;
        highestMonCurrentDivHighestMonOther/=highestMonBasis;
        if (!highestMonCurrentDivHighestMonOther.HasPositiveOrZeroExponents())
          crash << "This is a programming error: the pivot monomial in the polynomial division algorithm has negative exponent(s). "
          << "This is not allowed. " << crash;
        if (this->flagDoLogDivision)
        { this->intermediateHighestMonDivHighestMon.GetElement().
          AddOnTop(highestMonCurrentDivHighestMonOther);
          this->intermediateSelectedDivisors.GetElement().AddOnTop(i);
          this->intermediateHighlightedMons.GetElement().SetSize(this->intermediateHighlightedMons.GetElement().size+1);
          this->intermediateHighlightedMons.GetElement().LastObject()->size=0;
        }
        this->bufPoly=this->theBasiS[i];
        leadingMonCoeff/=this->leadingCoeffs[i];
        if (this->flagDoLogDivision)
          this->intermediateCoeffs.GetElement().AddOnTop(leadingMonCoeff);
        this->bufPoly.MultiplyBy(highestMonCurrentDivHighestMonOther, leadingMonCoeff);
        if (this->flagDoLogDivision)
          this->intermediateSubtractands.GetElement().AddOnTop(this->bufPoly);
        if (theGlobalVariables!=0 && this->flagDoProgressReport)
        { std::stringstream out;
          out << "Total number of polynomial operations so far: " << this->NumberGBComputations;
          if (this->MaxNumGBComputations>0)
            out << ", with a limit of " << this->MaxNumGBComputations << " operations.";
          out << "\n<br>Number of intermediate remainders: " << numIntermediateRemainders << "\n<br> Highest mon of current remainder: "
          << currentRemainder[indexLeadingMonRemainder].ToString() << ". \n<br>Current index we are dividing by: " << i+1
          << " out of " << this->theBasiS.size << "\n<br>" << currentRemainder.size() << " monomials in current remainder."
          << "\n<br>" << outputRemainder->size() << " monomials in output remainder.";
          theReport.Report(out.str());
          //stOutput << out.str();
        }
/*        if (this->NumberOfComputations>this->MaxNumGBComputations+1000)
        { stOutput << "<br>Dividing "
          << currentRemainder.ToString()
          <<  " by " << theBasiS[i].ToString() << "<br>i.e. subtracting "
          << this->bufPoly.ToString() ;
          Polynomial<Rational> currentRemainder1;
          currentRemainder1=currentRemainder;
          currentRemainder1-=this->bufPoly;
          stOutput << " I must get: " << currentRemainder1.ToString();
        }*/
        //stOutput << " <hr>Subtracting " << this->bufPoly.ToString()
        //<< " from " << currentRemainder.ToString();
        currentRemainder-=this->bufPoly;
        //stOutput << " to get " << currentRemainder.ToString();
        if (this->flagDoLogDivision)
          this->intermediateRemainders.GetElement().AddOnTop(currentRemainder);
        divisionOcurred=true;
/*        if (this->NumberOfComputations>this->MaxNumGBComputations+1000)
        { stOutput << "<br>Result:<br> " << currentRemainder.ToString()
          << "<br>Current divisor index: " << i+1;
          if(this->NumberOfComputations>this->MaxNumGBComputations+1010)
            crash
            << "<br>This may or may not be a programming error. While handling computation excess limit, "
            << " I got that NumberOfComputations is much larger than MaxNumGBComputations. "
            << " I have no explanation for this issue right now, so I am crashing to let you know "
            << " something is fishy. "<< crash;
        }*/
        this->NumberGBComputations++;
        //stOutput << " to get " << currentRemainder.ToString(&theGlobalVariables->theDefaultFormat);
      } else
        i++;
    }
    if (!divisionOcurred)
    { outputRemainder->AddMonomial(highestMonCurrentDivHighestMonOther, leadingMonCoeff);
      if (this->flagDoLogDivision)
        (*this->intermediateHighlightedMons.GetElement().LastObject()).AddOnTop(highestMonCurrentDivHighestMonOther);
      currentRemainder.PopMonomial(indexLeadingMonRemainder);
      this->NumberGBComputations++;
      /*if (this->NumberGBComputations>this->MaxNumGBComputations+1000)
        crash
        << "This may or may not be a programming error. While handling computation excess limit, "
        << " I got that NumberGBComputations is much larger than MaxNumGBComputations. "
        << " I have no explanation for this issue right now, so I am crashing to let you know "
        << " something is fishy. " << "<br>Current remainder:<br> " << currentRemainder.ToString() << crash;
        */
      if (theGlobalVariables!=0 && this->flagDoProgressReport)
      { std::stringstream out;
        out << "Number of intermediate remainders: " << numIntermediateRemainders << "\n<br> Highest mon of current remainder is no longer reducible. "
        << "\n<br>" << currentRemainder.size() << " monomials in current remainder.\n<br>" << outputRemainder->size() << " monomials in output remainder.";
        theReport.Report(out.str());
          //stOutput << out.str();
      }
    }
  }
//  stOutput << " <br>final remainder: "
//  << outputRemainder.ToString(&theGlobalVariables->theDefaultFormat)
//  << "<hr>";
}

template <class coefficient>
bool GroebnerBasisComputation<coefficient>::AddRemainderToBasis(GlobalVariables* theGlobalVariables)
{ if (this->leadingMons.size!=this->theBasiS.size)
    crash << "This is a programming error: the number of leading monomials does not equal the number of polynomials. " << crash;
  MacroRegisterFunctionWithName("GroebnerBasisComputation::AddPolyToBasis");
  if (this->remainderDivision.IsEqualToZero())
    return false;
  this->remainderDivision.ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
  int indexMaxMon=this->remainderDivision.GetIndexMaxMonomial(this->thePolynomialOrder.theMonOrder);
  const MonomialP& theNewLeadingMon=this->remainderDivision[indexMaxMon];
  if (this->flagDoSortBasis)
  { this->theBasiS.SetSize(this->theBasiS.size+1);
    this->leadingMons.SetSize(this->theBasiS.size);
    this->leadingCoeffs.SetSize(this->theBasiS.size);
    for (int i=theBasiS.size-1; i>=0; i--)
    { bool shouldAddHere=(i==0) ? true: theNewLeadingMon>this->theBasiS[i-1].GetMaxMonomial(this->thePolynomialOrder.theMonOrder);
      if (shouldAddHere)
      { this->theBasiS[i]=this->remainderDivision;
        this->leadingMons[i]=theNewLeadingMon;
        this->leadingCoeffs[i]=this->remainderDivision.theCoeffs[indexMaxMon];
        break;
      } else
      { this->theBasiS[i]=this->theBasiS[i-1];
        this->leadingMons[i]=this->leadingMons[i-1];
        this->leadingCoeffs[i]=this->leadingCoeffs[i-1];
      }
    }
  } else
  { this->theBasiS.AddOnTop(this->remainderDivision);
    this->leadingMons.AddOnTop(theNewLeadingMon);
    this->leadingCoeffs.AddOnTop(this->remainderDivision.theCoeffs[indexMaxMon]);
  }
//  this->theBasiS.AddOnTop(this->remainderDivision);
//  this->leadingMons.AddOnTop(theNewLeadingMon);
//  this->leadingCoeffs.AddOnTop(this->remainderDivision.theCoeffs[indexMaxMon]);
  return true;
}

template <class coefficient>
GroebnerBasisComputation<coefficient>::GroebnerBasisComputation()
{ this->thePolynomialOrder.theMonOrder=MonomialP::LeftIsGEQLexicographicLastVariableStrongest;
  this->NumberGBComputations=0;
  this->NumberSerreSystemComputations=0;
  this->flagDoProgressReport=true;
  this->flagDoSortBasis=true;
  this->flagBasisGuaranteedToGenerateIdeal=false;
  this->flagDoLogDivision=false;
  this->flagSystemProvenToHaveNoSolution=false;
  this->flagSystemSolvedOverBaseField=false;
  this->MaxNumGBComputations=0;
  this->MaxNumSerreSystemComputationsPreferred=0;
}

template <class coefficient>
void GroebnerBasisComputation<coefficient>::initForDivisionAlone(List<Polynomial<coefficient> >& inputOutpuT, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("GroebnerBasisComputation::initForDivisionAlone");
  if (inputOutpuT.size<=0)
    crash << "This is a programming error: I cannot transform an empty list to a Groebner basis. " << crash;
  this->theBasiS=inputOutpuT;
  this->leadingMons.SetSize(inputOutpuT.size);
  this->leadingCoeffs.SetSize(inputOutpuT.size);
  for (int i=0; i<this->theBasiS.size; i++)
  { Polynomial<Rational>& curPoly=theBasiS[i];
    int theIndex=curPoly.GetIndexMaxMonomial(this->thePolynomialOrder.theMonOrder);
    if (theIndex==-1)
      crash << "This is a programming error: initialization for polynomial division with respect to at least one zero polynomial. "
      << "If this is a bad user input, it should be handled at an earlier level. Here is the current basis by which we need to divide. "
      << this->theBasiS.ToString() << crash;
    this->leadingMons[i]=curPoly[theIndex];
    this->leadingCoeffs[i]=curPoly.theCoeffs[theIndex];
  }
  this->NumberGBComputations=0;
}

template <class coefficient>
void GroebnerBasisComputation<coefficient>::initForGroebnerComputation(List<Polynomial<coefficient> >& inputOutpuT, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("GroebnerBasisComputation::initForGroebnerComputation");
  if (inputOutpuT.size<=0)
    crash << "This is a programming error: I cannot transform an empty list to a Groebner basis. " << crash;
  this->basisCandidates.SetSize(0);
  this->theBasiS.SetSize(0);
  this->theBasiS.ReservE(inputOutpuT.size);
  this->leadingMons.SetSize(0);
  this->leadingMons.ReservE(inputOutpuT.size);
  this->leadingCoeffs.SetSize(0);
  this->leadingCoeffs.ReservE(inputOutpuT.size);
  this->NumberGBComputations=0;
}

template<class coefficient>
void GroebnerBasisComputation<coefficient>::CheckConsistency()
{ //if (this->NumberOfComputations>this->MaxNumComputations+1000)
    //crash << "This may or may not be a programming error. While handling computation excess limit, I got that NumberOfComputations is much larger than MaxNumComputations. "
    //<< " I have no explanation for this issue right now, so I am crashing to let you know something is fishy. "
    //<< crash;
}

template<class coefficient>
std::string GroebnerBasisComputation<coefficient>::GetPolynomialStringSpacedMonomials
(const Polynomial<coefficient>& thePoly, const HashedList<MonomialP>& theMonomialOrder, const std::string& extraStyle, const std::string& extraHighlightStyle,
  List<MonomialP>* theHighLightedMons)
{ std::stringstream out;
  bool found=false;
  int countMons=0;
  for (int i=0; i<theMonomialOrder.size; i++)
  { int theIndex= thePoly.theMonomials.GetIndex(theMonomialOrder[i]);
    if (theIndex==-1)
    { out << "<td" << extraStyle << ">" << "</td>";
      continue;
    }
    countMons++;
    bool useHighlightStyle=false;
    if (theHighLightedMons!=0)
      if (theHighLightedMons->Contains(theMonomialOrder[i]))
        useHighlightStyle=true;
    out << "<td" << extraStyle << ">";
    if (useHighlightStyle)
      out << "<span style=\"color:red\">";
    if (this->theFormat.flagUseLatex)
      out << CGI::GetMathSpanPure(Polynomial<Rational>::GetBlendCoeffAndMon(thePoly[theIndex], thePoly.theCoeffs[theIndex], found, &this->theFormat));
    else
      out << Polynomial<Rational>::GetBlendCoeffAndMon(thePoly[theIndex], thePoly.theCoeffs[theIndex], found, &this->theFormat);
    found=true;
    if (useHighlightStyle)
      out << "</span>";
    out << "</td>";
  }
  if (countMons!=thePoly.size())
    out << "<td><b>Programming ERROR!</b></td>";
  return out.str();
}

template <class coefficient>
void GroebnerBasisComputation<coefficient>::GetSubFromPartialSolutionSerreLikeSystem(PolynomialSubstitution<coefficient>& outputSub)
{ outputSub.MakeIdSubstitution(this->systemSolution.GetElement().size);
  for (int i=0; i<this->solutionsFound.GetElement().CardinalitySelection; i++)
    outputSub[this->solutionsFound.GetElement().elements[i]]= this->systemSolution.GetElement()[this->solutionsFound.GetElement().elements[i]];
}

template <class coefficient>
bool GroebnerBasisComputation<coefficient>::GetOneVarPolySolution(const Polynomial<coefficient>& thePoly, coefficient& outputSolution, AlgebraicClosureRationals& theAlgebraicClosure, GlobalVariables* theGlobalVariables)
{ AlgebraicNumber theAN;
  if (!theAN.ConstructFromMinPoly(thePoly, theAlgebraicClosure, theGlobalVariables))
    return false;
  outputSolution=theAN;
  return true;
}

template <class coefficient>
bool GroebnerBasisComputation<coefficient>::HasImpliedSubstitutions
(List<Polynomial<coefficient> >& inputSystem, PolynomialSubstitution<coefficient>& outputSub, AlgebraicClosureRationals* theAlgebraicClosure, GlobalVariables* theGlobalVariables)
{ int numVars=this->systemSolution.GetElement().size;
  MonomialP tempM;
  Polynomial<coefficient> tempP;
  coefficient theCF;
//  if (theAlgebraicClosure!=0)
//  { stOutput << "running with non-zero algebraic closure";
//  }
  for (int i=0; i<inputSystem.size; i++)
  { tempP=inputSystem[i];
    for (int j=0; j<numVars; j++)
    { tempM.MakeEi(j, 1, numVars);
      int indexTempM=tempP.theMonomials.GetIndex(tempM);
      if (indexTempM==-1)
        continue;
      theCF=tempP.theCoeffs[indexTempM];
      tempP.SubtractMonomial(tempM, theCF);
      bool isGood=true;
      for (int k=0; k<tempP.size(); k++)
        if (!(tempP[k](j)==0))
        { isGood=false;
          tempP.AddMonomial(tempM, theCF);
          break;
        }
      if (!isGood)
        continue;
      outputSub.MakeIdSubstitution(numVars);
      outputSub[j]=tempP;
      theCF*=-1;
      outputSub[j]/=theCF;
//      coefficient theConst;
//      stOutput << "<hr>Output sub is: x_{" << j+1 << "}=" << outputSub[j].ToString();
//      if (outputSub[j].IsConstant(&theConst))
//        this->SetSerreLikeSolutionIndex(j, theConst);
      //stOutput << "<br>Current solution candidate is: " << this->systemSolution.GetElement().ToString();
      return true;
    }
    int oneVarIndex;
    if (tempP.IsOneVariableNonConstPoly(&oneVarIndex))
      if (theAlgebraicClosure!=0)
        if (this->GetOneVarPolySolution(tempP, theCF, *theAlgebraicClosure, theGlobalVariables))
        { //stOutput << "<br>adjoining root of " << tempP.ToString();
          outputSub.MakeIdSubstitution(numVars);
          outputSub[oneVarIndex].MakeConst(theCF);
          //check our work:
          tempP.Substitution(outputSub);
          if (!tempP.IsEqualToZero())
            crash << "This is a programming error: I was solving the polynomial equation " << inputSystem[i].ToString()
            << ", which resulted in the substitution " << outputSub.ToString()
            << ". However, after carrying out the substitution in the polynomial, I got " << tempP.ToString() << ". "
            << crash;
          //
          return true;
        }
  }
  return false;
}

template <class coefficient>
int GroebnerBasisComputation<coefficient>::GetNumEquationsThatWouldBeLinearIfIsubbedVar(int theVarIndex, List<Polynomial<coefficient> >& input)
{ int result=0;
  for (int i=0; i<input.size; i++)
  { Rational degExcludingVar=0;
    for (int j=0; j<input[i].size(); j++)
      degExcludingVar=MathRoutines::Maximum(input[i][j].TotalDegree()-input[i][j](theVarIndex), degExcludingVar);
    if (degExcludingVar<2)
      result++;
  }
  return result;
}

template <class coefficient>
int GroebnerBasisComputation<coefficient>::GetPreferredSerreSystemSubIndex(List<Polynomial<coefficient> >& inputSystem)
{ Selection varSel;
  this->GetVarsToSolveFor(inputSystem, varSel);
  if (varSel.CardinalitySelection==0)
    return -1;
  int championIndex=varSel.elements[0];
  int championImprovement=GroebnerBasisComputation<coefficient>::GetNumEquationsThatWouldBeLinearIfIsubbedVar(championIndex, inputSystem);
  for (int i=1; i<varSel.CardinalitySelection; i++)
    if (GroebnerBasisComputation<coefficient>::GetNumEquationsThatWouldBeLinearIfIsubbedVar
        (varSel.elements[i], inputSystem)>championImprovement)
  { championIndex=varSel.elements[i];
    championImprovement=GroebnerBasisComputation<coefficient>::GetNumEquationsThatWouldBeLinearIfIsubbedVar(varSel.elements[i], inputSystem);
  }
  return championIndex;
}

template <class coefficient>
void GroebnerBasisComputation<coefficient>::BackSubstituteIntoSinglePoly(Polynomial<coefficient>& thePoly, int theIndex, PolynomialSubstitution<coefficient>& theFinalSub, GlobalVariables* theGlobalVariables)
{ Polynomial<coefficient> tempP;
  tempP.MakeMonomiaL(theIndex, 1, 1);
  if (thePoly==tempP)
    return;
  //stOutput << "<hr> Back substituting in x_{" << theIndex+1 << "}:=" << thePoly.ToString();
  thePoly.Substitution(theFinalSub);
  bool changed=false;
  for (int i=0; i<thePoly.size(); i++)
    for (int j=0; j<thePoly[i].GetMinNumVars(); j++)
      if (thePoly[i](j)!=0)
      { if (!this->solutionsFound.GetElement().selected[j])
          this->SetSerreLikeSolutionIndex(j, 0);
        else
          if (this->systemSolution.GetElement()[j]!=0)
            crash << "This is a programming error: variable index " << j+1 << " is supposed to be a free parameter, i.e., be set to zero, but "
            << "instead it has a non-zero value. " << crash;
        theFinalSub[j]=0;
        changed=true;
      }
  if (changed)
    thePoly.Substitution(theFinalSub);
  coefficient tempCF;
  if (!thePoly.IsConstant(&tempCF))
    crash << "\n<br>\nThis is a programming error: after carrying all implied substitutions the polynomial is not a constant, rather equals "
    << thePoly.ToString() << ". " << crash;
  theFinalSub[theIndex]=tempCF;
  this->SetSerreLikeSolutionIndex(theIndex, tempCF);
}

template <class coefficient>
void GroebnerBasisComputation<coefficient>::BackSubstituteIntoPolySystem(List<PolynomialSubstitution<coefficient> >& theImpliedSubs, GlobalVariables* theGlobalVariables)
{ PolynomialSubstitution<coefficient> FinalSub;
  this->GetSubFromPartialSolutionSerreLikeSystem(FinalSub);
  for (int i=theImpliedSubs.size-1; i>=0; i--)
    for (int j=0; j<theImpliedSubs[i].size; j++)
      this->BackSubstituteIntoSinglePoly(theImpliedSubs[i][j], j, FinalSub, theGlobalVariables);
}

template <class coefficient>
int GroebnerBasisComputation<coefficient>::GetNumVarsToSolveFor(const List<Polynomial<coefficient> >& input)
{ Selection theSel;
  GroebnerBasisComputation<coefficient>::GetVarsToSolveFor(input, theSel);
  return theSel.CardinalitySelection;
}

template <class coefficient>
void GroebnerBasisComputation<coefficient>::GetVarsToSolveFor(const List<Polynomial<coefficient> >& input, Selection& output)
{ MacroRegisterFunctionWithName("GroebnerBasisComputation::GetVarsToSolveFor");
  int NumVars=0;
  for (int i=0; i<input.size; i++)
    NumVars=MathRoutines::Maximum(NumVars, input[i].GetMinNumVars());
  output.init(NumVars);
  for (int i=0; i<input.size && output.CardinalitySelection<output.MaxSize; i++)
    for (int j=0; j<input[i].size() && output.CardinalitySelection<output.MaxSize; j++)
      for (int k=0; k<input[i][j].GetMinNumVars() && output.CardinalitySelection<output.MaxSize; k++)
        if (input[i][j](k)!=0)
          output.AddSelectionAppendNewIndex(k);
}

template <class coefficient>
bool GroebnerBasisComputation<coefficient>::IsContradictoryReducedSystem(const List<Polynomial<coefficient> >& input)
{ if (input.size==1)
    if (input[0].IsConstant())
      return true;
  return false;
}

template <class coefficient>
void GroebnerBasisComputation<coefficient>::SolveSerreLikeSystemRecursively
(List<Polynomial<coefficient> >& inputSystem, AlgebraicClosureRationals* theAlgebraicClosure, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("GroebnerBasisComputation::SolveSerreLikeSystemRecursively");
  RecursionDepthCounter theCounter(&this->RecursionCounterSerreLikeSystem);
  ProgressReport theReport1(theGlobalVariables);
  int numVarsToSolveForStart=this->GetNumVarsToSolveFor(inputSystem);
  if (theGlobalVariables!=0)
  { std::stringstream out;
    out << "<br>Solving Serre-like polynomial system, recursion depth: " << this->RecursionCounterSerreLikeSystem;
    theReport1.Report(out.str());
  }
//  stOutput << "<hr><hr>Recursion depth: " << this->RecursionCounterSerreLikeSystem << "<br>Solving the system\n<br>\n";
//  for (int i=0; i<inputSystem.size; i++)
//    stOutput << "<br>" << CGI::GetMathSpanPure(inputSystem[i].ToString());
  bool changed=true;
  PolynomialSubstitution<coefficient> theSub;
  List<PolynomialSubstitution<coefficient> > theImpliedSubs;
  theImpliedSubs.ReservE(inputSystem.size);
  List<Polynomial<coefficient> > startingSystemNoModifications=inputSystem;
//  int startingMaxNumSerreSystemComputations=this->MaxNumSerreSystemComputations;
  while (changed)
  { this->NumberGBComputations=0;
    //stOutput << "<br>Transforming to reduced groebner basis: " << inputSystem.ToString();
    bool success=this->TransformToReducedGroebnerBasis(inputSystem, theGlobalVariables);
    this->NumberSerreSystemComputations+=this->NumberGBComputations;
    //if (!success)
    //  stOutput << "<br>Failed to reduce system!";
    if (success)
    { //stOutput << "<hr>System groebner reduced successfully, output:";
      //for (int i=0; i <inputSystem.size; i++)
        //stOutput << "<br>" << CGI::GetMathSpanPure(inputSystem[i].ToString());
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
    changed = this->HasImpliedSubstitutions(inputSystem, theSub, theAlgebraicClosure, theGlobalVariables);
    if (changed)
    { theImpliedSubs.AddOnTop(theSub);
      //stOutput << "<hr>Carrying out IMPLIED sub: " << CGI::GetMathSpanPure(theImpliedSubs.ToString()) << " in the system ";
      //for (int i=0; i<inputSystem.size; i++)
      //  stOutput << "<br>" << CGI::GetMathSpanPure(inputSystem[i].ToString());
      for (int i=0; i<inputSystem.size; i++)
        inputSystem[i].Substitution(theSub);
      //stOutput << " ... to get: ";
      //for (int i=0; i<inputSystem.size; i++)
      //  stOutput << "<br>" << CGI::GetMathSpanPure(inputSystem[i].ToString());
//      this->MaxNumSerreSystemComputations+=startingMaxNumSerreSystemComputations;
    }
  }
  std::stringstream impliedSubsReport;
  if (theGlobalVariables!=0 && theImpliedSubs.size>0)
  { impliedSubsReport << "Implied subs: ";
    for (int i=0; i<theImpliedSubs.size; i++)
      for (int j=0; j<theImpliedSubs[i].size; j++)
      { int letterIndex=-1;
        if (theImpliedSubs[i][j].IsOneLetterFirstDegree(&letterIndex))
          if (letterIndex==j)
            continue;
        impliedSubsReport << "<br>" << (MonomialP(j)).ToString(&this->theFormat) << ":=" << theImpliedSubs[i][j].ToString(&this->theFormat) << "; ";
      }
  }
  //stOutput << "<br>System has no more implied subs. At the moment, the system is: " << inputSystem.ToString();
  List<Polynomial<coefficient> > systemBeforeHeuristics=inputSystem;
  int numVariablesToSolveForAfterReduction=this->GetNumVarsToSolveFor(inputSystem);
  List<int> twoSolutionsToTry;
  twoSolutionsToTry.SetSize(2);
  twoSolutionsToTry[0]=0;
  twoSolutionsToTry[1]=1;

  GroebnerBasisComputation computationFirstTry;
  computationFirstTry.RecursionCounterSerreLikeSystem=this->RecursionCounterSerreLikeSystem;
  computationFirstTry.MaxNumGBComputations=this->MaxNumGBComputations;
  computationFirstTry.MaxNumSerreSystemComputationsPreferred=this->MaxNumSerreSystemComputationsPreferred;
  computationFirstTry.systemSolution=this->systemSolution;
  computationFirstTry.solutionsFound=this->solutionsFound;
  computationFirstTry.flagSystemProvenToHaveNoSolution=false;
  computationFirstTry.flagSystemSolvedOverBaseField=false;
  computationFirstTry.flagSystemProvenToHaveSolution=false;

//  if (this->RecursionCounterSerreLikeSystem==1)
//    twoSolutionsToTry.SwapTwoIndices(0,1);

  int theVarIndex=this->GetPreferredSerreSystemSubIndex(inputSystem);
  if (theVarIndex==-1)
    crash << "This is a programming error: preferred substitution variable index is -1. Input system in calculator-input format: <br>"
    << this->GetCalculatorInputFromSystem(inputSystem) << "<br>" << crash;
  theSub.MakeIdSubstitution(this->systemSolution.GetElement().size);
  theSub[theVarIndex]=twoSolutionsToTry[0];
  //stOutput << "<br>Setting x_{" << theVarIndex+1 << "}:=0";
  if (theGlobalVariables!=0)
  { std::stringstream out;
    out << "Solving Serre-like polynomial system, recursion depth: " << this->RecursionCounterSerreLikeSystem << ". Managed to reduce "
    << numVarsToSolveForStart-numVariablesToSolveForAfterReduction << " variables. Number remaining variables to solve for: "
    << numVariablesToSolveForAfterReduction << ". Try 1 out of 2. "
    << impliedSubsReport.str() << "<br>Substitution attempts:<br>" << "x_{" << theVarIndex+1 << "}:=0;";
    theReport1.Report(out.str());
  }

  computationFirstTry.SetSerreLikeSolutionIndex(theVarIndex, twoSolutionsToTry[0]);
  //stOutput << "<br>Input system before sub first recursive call. " << inputSystem.ToString();
  for (int i=0; i<inputSystem.size; i++)
    inputSystem[i].Substitution(theSub);
//  stOutput << "<hr>Input system after sub first recursive call. " << inputSystem.ToString();

  computationFirstTry.SolveSerreLikeSystemRecursively(inputSystem, theAlgebraicClosure, theGlobalVariables);
  this->NumberSerreSystemComputations+=computationFirstTry.NumberSerreSystemComputations;
  if (computationFirstTry.flagSystemSolvedOverBaseField)
  { //stOutput << "<hr>System solved after first recursive call. The input system before back sub: " << CGI::GetMathSpanPure(inputSystem.ToString());
    computationFirstTry.NumberSerreSystemComputations=this->NumberSerreSystemComputations;
    *this=computationFirstTry;
    this->BackSubstituteIntoPolySystem(theImpliedSubs, theGlobalVariables);
    return;
  }
  if (computationFirstTry.flagSystemProvenToHaveSolution)
    this->flagSystemProvenToHaveSolution=true;
  inputSystem=systemBeforeHeuristics;

  GroebnerBasisComputation computationSecondTry;
  computationSecondTry.RecursionCounterSerreLikeSystem=this->RecursionCounterSerreLikeSystem;
  computationSecondTry.MaxNumGBComputations=this->MaxNumGBComputations;
  computationSecondTry.MaxNumSerreSystemComputationsPreferred=
  this->MaxNumSerreSystemComputationsPreferred;

  computationSecondTry.systemSolution=this->systemSolution;
  computationSecondTry.solutionsFound=this->solutionsFound;
  computationSecondTry.flagSystemProvenToHaveNoSolution=false;
  computationSecondTry.flagSystemSolvedOverBaseField=false;
  computationSecondTry.flagSystemProvenToHaveSolution=false;

  theSub.MakeIdSubstitution(this->systemSolution.GetElement().size);
  theSub[theVarIndex]=twoSolutionsToTry[1];
  //stOutput << "<hr>Setting x_{" << theVarIndex+1 << "}:=1";
  computationSecondTry.SetSerreLikeSolutionIndex(theVarIndex, twoSolutionsToTry[1]);

  //stOutput << "<hr>Input system before second recursive call. " << inputSystem.ToString();
  //stOutput << "<br>Solution before second recursive call. "
  //<< newComputation.systemSolution.GetElement().ToString();
  for (int i=0; i<inputSystem.size; i++)
    inputSystem[i].Substitution(theSub);
  //stOutput << "<br>Input system after sub second recursive call. " << inputSystem.ToString();
  if (theGlobalVariables!=0)
  { std::stringstream out;
    out << "Solving Serre-like polynomial system, recursion depth: " << this->RecursionCounterSerreLikeSystem << ". Managed to reduce "
    << numVarsToSolveForStart-numVariablesToSolveForAfterReduction << " variables. Number remaining variables to solve for: "
    << numVariablesToSolveForAfterReduction << ". Try 2 out of 2. "
    << impliedSubsReport.str() << "<br>Substitution attempts:<br>" << "x_{" << theVarIndex+1 << "}:=1;";
    theReport1.Report(out.str());
  }
  computationSecondTry.SolveSerreLikeSystemRecursively(inputSystem, theAlgebraicClosure, theGlobalVariables);
  this->NumberSerreSystemComputations+=computationSecondTry.NumberSerreSystemComputations;
  if (computationSecondTry.flagSystemSolvedOverBaseField)
  { //stOutput << "<hr>System solved after second recursive call. The input system before back sub: " << CGI::GetMathSpanPure(inputSystem.ToString());
    computationSecondTry.NumberSerreSystemComputations=this->NumberSerreSystemComputations;
    *this=computationSecondTry;
    this->BackSubstituteIntoPolySystem(theImpliedSubs, theGlobalVariables);
    return;
  }
  if (computationSecondTry.flagSystemProvenToHaveSolution)
    this->flagSystemProvenToHaveSolution=true;
  inputSystem=startingSystemNoModifications;
}

template <class coefficient>
std::string GroebnerBasisComputation<coefficient>::GetCalculatorInputFromSystem(const List<Polynomial<coefficient> >& inputSystem)
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

template <class coefficient>
void GroebnerBasisComputation<coefficient>::SolveSerreLikeSystem(List<Polynomial<coefficient> >& inputSystem, AlgebraicClosureRationals* theAlgebraicClosure, GlobalVariables* theGlobalVariables)
{ this->flagSystemProvenToHaveNoSolution=false;
  this->flagSystemSolvedOverBaseField=false;
  this->flagSystemProvenToHaveSolution=false;
  this->RecursionCounterSerreLikeSystem=0;
  int numVars=0;
//  stOutput << "<hr>" << this->GetCalculatorInputFromSystem(inputSystem) << "<hr>";
  List<Polynomial<coefficient> > workingSystem=inputSystem;
  for (int i=0; i<workingSystem.size; i++)
    numVars=MathRoutines::Maximum(numVars, workingSystem[i].GetMinNumVars());
  this->systemSolution.GetElement().initFillInObject(numVars, 0);
  this->solutionsFound.GetElement().init(numVars);
  ProgressReport theReport(theGlobalVariables);
  std::stringstream reportStream;
  if (theGlobalVariables!=0)
  { reportStream << "Solving system " << this->GetCalculatorInputFromSystem(inputSystem);
    theReport.Report(reportStream.str());
  }
  this->SolveSerreLikeSystemRecursively(workingSystem, 0, theGlobalVariables);
  this->NumberSerreVariablesOneGenerator= workingSystem[0].GetMinNumVars()/2;
  if (theAlgebraicClosure!=0)
    if (!this->flagSystemSolvedOverBaseField && !this->flagSystemProvenToHaveNoSolution)
    { if (theGlobalVariables!=0)
      { reportStream << "<br><b>Failed to solve system over the rationals... attempting to solve allowing algebraic extensions.</b> ";
        theReport.Report(reportStream.str());
      }
      this->SolveSerreLikeSystemRecursively(workingSystem, theAlgebraicClosure, theGlobalVariables);
    }
  if (this->flagSystemSolvedOverBaseField)
  { if (this->solutionsFound.GetElement().CardinalitySelection!= this->solutionsFound.GetElement().MaxSize)
      for (int i=0; i<this->solutionsFound.GetElement().MaxSize; i++)
        if (!this->solutionsFound.GetElement().selected[i])
          this->SetSerreLikeSolutionIndex(i, 0);
    PolynomialSubstitution<coefficient> theSub;
    this->GetSubFromPartialSolutionSerreLikeSystem(theSub);
    workingSystem=inputSystem;
    for (int i=0; i<workingSystem.size; i++)
    { workingSystem[i].Substitution(theSub);
      if (!workingSystem[i].IsEqualToZero())
        crash << "<br>This is a programming error. Function SolveSerreLikeSystem reports to have found a solution over the base field, "
        << "but substituting the solution back to the original system does not yield a zero system of equations. More precisely, "
        << "the reported solution was " << this->ToStringSerreLikeSolution() << " but substitution in equation " << inputSystem[i].ToString()
        << " yields " << workingSystem[i].ToString() << ". Calculator input: <br>" << this->GetCalculatorInputFromSystem(inputSystem) << " <br>"
        << crash;
    }
  }
}

template <class coefficient>
std::string GroebnerBasisComputation<coefficient>::ToStringSerreLikeSolution()
{ std::stringstream out;
  Polynomial<Rational> tempP;
  for (int i=0; i<this->systemSolution.GetElement().size; i++)
    if (this->solutionsFound.GetElement().selected[i])
    { tempP.MakeMonomiaL(i, 1, 1);
      out << " " << tempP.ToString(&this->theFormat) << " := " << this->systemSolution.GetElement()[i] << ";";
    }
  return out.str();
}

template <class coefficient>
void GroebnerBasisComputation<coefficient>::SetSerreLikeSolutionIndex(int theIndex, const coefficient& theConst)
{ this->systemSolution.GetElement()[theIndex]=theConst;
  if (this->solutionsFound.GetElement().selected[theIndex])
    crash << "This a programming error: attempting to set value to a variable whose value has already been computed. " << crash;
  this->solutionsFound.GetElement().AddSelectionAppendNewIndex(theIndex);
}
#endif
