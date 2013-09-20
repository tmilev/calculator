//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfImplementationHeaderPolyComputationsAdvanced_already_included
#define vpfImplementationHeaderPolyComputationsAdvanced_already_included

#include "vpfImplementationHeader2Math051_PolynomialComputations_Basic.h"
static ProjectInformationInstance ProjectInfovpfImplementationHeaderPolynomialComputations(__FILE__, "Header, implementation polynomial computations, more advanced. ");

template <class coefficient>
bool GroebnerBasisComputation<coefficient>::TransformToReducedGroebnerBasis(List<Polynomial<coefficient> >& inputOutpuT, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("RationalFunctionOld::TransformToReducedGroebnerBasis");
  this->initForGroebnerComputation(inputOutpuT, theGlobalVariables);
  this->basisCandidates=inputOutpuT;
  ProgressReport theReport(theGlobalVariables);
 // std::string tempS;
//  bool changed=true;
  this->flagBasisGuaranteedToGenerateIdeal=false;
  this->AddPolyAndReduceBasis(theGlobalVariables);
  this->flagBasisGuaranteedToGenerateIdeal=true;
  bool changed=true;
  while (changed)
  { changed=false;
    for (int i=0; i<this->theBasiS.size; i++)
      for (int j=i+1; j<this->theBasiS.size && i<this->theBasiS.size; j++)
      { Polynomial<coefficient>& currentLeft= this->theBasiS[i];
        Polynomial<coefficient>& currentRight= this->theBasiS[j];
        int leftIndex=currentLeft.GetIndexMaxMonomial(this->theMonOrdeR);
        int rightIndex=currentRight.GetIndexMaxMonomial(this->theMonOrdeR);
        const MonomialP& leftHighestMon=currentLeft[leftIndex];
        const MonomialP& rightHighestMon=currentRight[rightIndex];
        int numVars=
        MathRoutines::Maximum(leftHighestMon.GetMinNumVars(), rightHighestMon.GetMinNumVars());
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
        this->NumberOfComputations++;
        if (this->MaxNumComputations>0)
          if (this->NumberOfComputations>this->MaxNumComputations)
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
    if (this->MaxNumComputations>0)
      if (this->NumberOfComputations>this->MaxNumComputations)
        return false;
  }
//  this->MakeMinimalBasis();
  inputOutpuT=this->theBasiS;
  return true;
}

template<class coefficient>
bool GroebnerBasisComputation<coefficient>::AddPolyAndReduceBasis(GlobalVariables* theGlobalVariables)
{ bool changed=false;
  ProgressReport theReport(theGlobalVariables);
  while (this->basisCandidates.size>0)
  { bool addedNew=false;
    while (this->basisCandidates.size>0)
    { this->RemainderDivisionWithRespectToBasis(*this->basisCandidates.LastObject(), &this->remainderDivision, theGlobalVariables);
      this->basisCandidates.RemoveLastObject();
      if(this->AddRemainderToBasis(theGlobalVariables))
      { changed=true;
        addedNew=true;
      }
      this->NumberOfComputations++;
      if (this->MaxNumComputations>0)
        if (this->NumberOfComputations>this->MaxNumComputations)
        { if (!this->flagBasisGuaranteedToGenerateIdeal)
            this->theBasiS.AddListOnTop(this->basisCandidates);
          this->ConsistencyCheck();
          return true;
        }
      if (theGlobalVariables!=0 && this->flagDoProgressReport)
      { std::stringstream out;
        out << "Reducing: " << theBasiS.size << " basis elements, " << this->basisCandidates.size << " candidates. ";
        theReport.Report(out.str());
//          std::cout << "<br>" << out.str();
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
//        std::cout << "<br>" << out.str();
      }
      this->NumberOfComputations++;
      this->RemainderDivisionWithRespectToBasis(this->bufPolyForGaussianElimination, &this->remainderDivision, theGlobalVariables, i);
      if (this->MaxNumComputations>0)
        if (this->NumberOfComputations>this->MaxNumComputations)
        { this->ConsistencyCheck();
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
{ MacroRegisterFunctionWithName("RationalFunctionOld::GroebnerBasisMakeMinimal");
/*  std::cout << "<br><br> and the leading monomials are: ";
  for (int i=0; i<LeadingCoeffs.size; i++)
    std::cout << LeadingCoeffs[i].ToString() << ", ";*/
  for (int i=0; i<this->theBasiS.size; i++)
    for (int j=0; j<this->leadingMons.size; j++)
      if (i!=j)
        if (this->leadingMons[i].IsDivisibleBy(this->leadingMons[j]))
        { /*std::cout << "<br>" << LeadingCoeffs[i].ToString() << " is divisible by "
          << LeadingCoeffs[j].ToString();*/
          this->leadingMons.RemoveIndexSwapWithLast(i);
          this->theBasiS.RemoveIndexSwapWithLast(i);
          i--;
          break;
        } //else
        //{ std::cout << "<br>" << LeadingCoeffs[i].ToString() << " is NOT divisible by "
        //  << LeadingCoeffs[j].ToString();
        //
/*  std::cout << "<br><br> final leading monomials are: ";
  for (int i=0; i<LeadingCoeffs.size; i++)
    std::cout <<"<br>" << LeadingCoeffs[i].ToString() << " of " << theBasis[i].ToString();
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
  HashedListSpecialized<PairInts > indexPairs;
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
  { std::cout << "This is a programming error: transforming to Groebner basis not allowed for empty basis. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
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
//        std::cout << "<br>testing sopoly: "
//        << leftBuf.ToString(&theGlobalVariables->theDefaultFormat)
//        ;
        this->RemainderDivisionWithRespectToBasis(leftBuf, &outputRemainder, theGlobalVariables);
        if (theGlobalVariables!=0 && this->flagDoProgressReport)
        { std::stringstream out;
          out << "Basis size: " << this->theBasiS.size << ".\n<br>Remaining cases current round: " << indexPairs.size;
          reportOuter.Report(out.str());
        }
//        std::cout << "<br>and sopoly divided by "
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
        //std::cout << "<br>" << leftHighestMon.ToString(&theGlobalVariables->theDefaultFormat)
        //<< " and " << rightHighestMon.ToString(&theGlobalVariables->theDefaultFormat)
        //<< " failed the closh criterium";
    //else
      //std::cout << "<br>" << leftHighestMon.ToString(&theGlobalVariables->theDefaultFormat)
      //<< " and " << rightHighestMon.ToString(&theGlobalVariables->theDefaultFormat)
      //<< " failed the lcm criterium";
    indexPairs.RemoveIndexSwapWithLast(currentPairIndex);
  }
  this->MakeMinimalBasis();
  inputOutpuT=this->theBasiS;
  return true;
}

template <class coefficient>
std::string GroebnerBasisComputation<coefficient>::GetDivisionString(FormatExpressions* theFormat)
{ std::stringstream out;
  List<Polynomial<Rational> >& theRemainders=this->intermediateRemainders.GetElement();
  List<Polynomial<Rational> >& theSubtracands=this->intermediateSubtractands.GetElement();
  if (theFormat!=0)
    theFormat->thePolyMonOrder=this->theMonOrdeR;
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
  totalMonCollection.QuickSortDescending(this->theMonOrdeR);
//  std::cout << "<hr>The monomials in play ordered: " << totalMonCollection.ToString(theFormat);
  int numVars=0;
  for (int i=0; i<totalMonCollection.size; i++)
  { const MonomialP& currentMon=totalMonCollection[i];
    numVars=MathRoutines::Maximum(currentMon.GetMinNumVars(), numVars);
  }
  out << "The variables: ";
  for (int i=0; i<numVars; i++)
  { MonomialP tempMon;
    tempMon.MakeEi(i, 1);
    out << tempMon.ToString(theFormat);
    if (i!=numVars-1)
      out << ", ";
  }
  out << "<br>";
  out << theRemainders.size  << " division steps total.<br>";
  out << "<table style=\"white-space: nowrap; border:1px solid black;\">";
  out << "<tr><td " << underlineStyle << "><b>Remainder:</b></td>";
  out << this->GetPolynomialStringSpacedMonomials(this->remainderDivision, totalMonCollection, underlineStyle, HighlightedStyle, theFormat, &this->remainderDivision.theMonomials) << "</td></tr>";
  for (int i=0; i<this->theBasiS.size; i++)
  { //if (i==this->theBasiS.size-1)
//    else
    out << "<tr>";
    out << "<td style=\"border-right:1px solid black; border-bottom: 1px solid gray;\">" << this->theBasiS[i].ToString(theFormat);
    out << "</td>";
    out << "<td style=\"border-bottom:1px solid gray;\" colspan=\"" << totalMonCollection.size+1 << "\">";
    out << "<table><tr>";
    for (int j=0; j<theRemainders.size; j++)
    { if (this->intermediateSelectedDivisors.GetElement()[j]!=i)
      { out << "<td></td>";
        continue;
      }
      out << "<td>" << Polynomial<Rational>::GetBlendCoeffAndMon
      (this->intermediateHighestMonDivHighestMon.GetElement()[j], this->intermediateCoeffs.GetElement()[j], true, theFormat);
      out << "</td>";
    }
    out << "</tr></table>";
    out << "</td></tr>";
  }
  out << "<tr><td></td>";
  out << this->GetPolynomialStringSpacedMonomials
  (this->startingPoly.GetElement(), totalMonCollection, "", HighlightedStyle, theFormat, &this->intermediateHighlightedMons.GetElement()[0]);
  out << "</tr>";
  for (int i=0; i<theRemainders.size; i++)
  { out << "<tr><td>-</td></tr>";
    out << "<tr><td></td>"  << this->GetPolynomialStringSpacedMonomials(theSubtracands[i], totalMonCollection, underlineStyle, underlineStyleHighlighted, theFormat)
    << "</tr>";
    out << "<tr><td></td>"
    << this->GetPolynomialStringSpacedMonomials(theRemainders[i], totalMonCollection, "", HighlightedStyle, theFormat, &this->intermediateHighlightedMons.GetElement()[i+1])
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
  { std::cout
    << "This is a programming error: the input, the output and the buffer member object must be pairwise distinct when carrying out "
    << " multi-polynomial division. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
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
    int indexLeadingMonRemainder= currentRemainder.GetIndexMaxMonomial(this->theMonOrdeR);
    leadingMonCoeff=currentRemainder.theCoeffs[indexLeadingMonRemainder];
    highestMonCurrentDivHighestMonOther=currentRemainder[indexLeadingMonRemainder];
    while (i<this->theBasiS.size && !divisionOcurred)
    { MonomialP& highestMonBasis=this->leadingMons[i];
      bool shouldDivide=(i==basisIndexToIgnore) ? false :highestMonCurrentDivHighestMonOther.IsDivisibleBy(highestMonBasis);
      if (shouldDivide)
      { numIntermediateRemainders++;
        highestMonCurrentDivHighestMonOther/=highestMonBasis;
        if (!highestMonCurrentDivHighestMonOther.HasPositiveOrZeroExponents())
        { std::cout << "This is a programming error: the pivot monomial in the polynomial division algorithm has negative exponent(s). "
          << "This is not allowed. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
          assert(false);
        }
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
          out << "Total number of polynomial operations so far: " << this->NumberOfComputations;
          if (this->MaxNumComputations>0)
            out << ", with a limit of no more than " << this->MaxNumComputations << " operations.";
          out
          << "\n<br>Number of intermediate remainders: " << numIntermediateRemainders << "\n<br> Highest mon of current remainder: "
          << currentRemainder[indexLeadingMonRemainder].ToString() << ". \n<br>Current index we are dividing by: " << i+1
          << " out of " << this->theBasiS.size << "\n<br>" << currentRemainder.size() << " monomials in current remainder."
          << "\n<br>" << outputRemainder->size() << " monomials in output remainder.";
          theReport.Report(out.str());
          //std::cout << out.str();
        }
/*        if (this->NumberOfComputations>this->MaxNumComputations+1000)
        { std::cout << "<br>Dividing "
          << currentRemainder.ToString()
          <<  " by " << theBasiS[i].ToString() << "<br>i.e. subtracting "
          << this->bufPoly.ToString() ;
          Polynomial<Rational> currentRemainder1;
          currentRemainder1=currentRemainder;
          currentRemainder1-=this->bufPoly;
          std::cout << " I must get: " << currentRemainder1.ToString();
        }*/
        //std::cout << " <hr>Subtracting " << this->bufPoly.ToString()
        //<< " from " << currentRemainder.ToString();
        currentRemainder-=this->bufPoly;
        //std::cout << " to get " << currentRemainder.ToString();
        if (this->flagDoLogDivision)
          this->intermediateRemainders.GetElement().AddOnTop(currentRemainder);
        divisionOcurred=true;
/*        if (this->NumberOfComputations>this->MaxNumComputations+1000)
        { std::cout << "<br>Result:<br> " << currentRemainder.ToString()
          << "<br>Current divisor index: " << i+1;
          if(this->NumberOfComputations>this->MaxNumComputations+1010)
          { std::cout
            << "<br>This may or may not be a programming error. While handling computation excess limit, "
            << " I got that NumberOfComputations is much larger than MaxNumComputations. "
            << " I have no explanation for this issue right now, so I am crashing to let you know "
            << " something is fishy. ";
            std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
            assert(false);
          }
        }*/
        this->NumberOfComputations++;
        //std::cout << " to get " << currentRemainder.ToString(&theGlobalVariables->theDefaultFormat);
      } else
        i++;
    }
    if (!divisionOcurred)
    { outputRemainder->AddMonomial(highestMonCurrentDivHighestMonOther, leadingMonCoeff);
      if (this->flagDoLogDivision)
        (*this->intermediateHighlightedMons.GetElement().LastObject()).AddOnTop(highestMonCurrentDivHighestMonOther);
      currentRemainder.PopMonomial(indexLeadingMonRemainder);
      this->NumberOfComputations++;
      /*if (this->NumberOfComputations>this->MaxNumComputations+1000)
      { std::cout
        << "This may or may not be a programming error. While handling computation excess limit, "
        << " I got that NumberOfComputations is much larger than MaxNumComputations. "
        << " I have no explanation for this issue right now, so I am crashing to let you know "
        << " something is fishy. ";
        std::cout << "<br>Current remainder:<br> " << currentRemainder.ToString();

        std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }*/
      if (theGlobalVariables!=0 && this->flagDoProgressReport)
      { std::stringstream out;
        out << "Number of intermediate remainders: " << numIntermediateRemainders << "\n<br> Highest mon of current remainder is no longer reducible. "
        << "\n<br>" << currentRemainder.size() << " monomials in current remainder.\n<br>" << outputRemainder->size() << " monomials in output remainder.";
        theReport.Report(out.str());
          //std::cout << out.str();
      }
    }
  }
//  std::cout << " <br>final remainder: "
//  << outputRemainder.ToString(&theGlobalVariables->theDefaultFormat)
//  << "<hr>";
}

template <class coefficient>
bool GroebnerBasisComputation<coefficient>::AddRemainderToBasis(GlobalVariables* theGlobalVariables)
{ if (this->leadingMons.size!=this->theBasiS.size)
  { std::cout << "This is a programming error: the number of leading monomials does not equal the number of polynomials. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  MacroRegisterFunctionWithName("GroebnerBasisComputation::AddPolyToBasis");
  if (this->remainderDivision.IsEqualToZero())
    return false;
  this->remainderDivision.ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
  int indexMaxMon=this->remainderDivision.GetIndexMaxMonomial(this->theMonOrdeR);
  const MonomialP& theNewLeadingMon=this->remainderDivision[indexMaxMon];
  if (this->flagDoSortBasis)
  { this->theBasiS.SetSize(this->theBasiS.size+1);
    this->leadingMons.SetSize(this->theBasiS.size);
    this->leadingCoeffs.SetSize(this->theBasiS.size);
    for (int i=theBasiS.size-1; i>=0; i--)
    { bool shouldAddHere=(i==0) ? true: theNewLeadingMon>this->theBasiS[i-1].GetMaxMonomial(this->theMonOrdeR);
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
{ this->theMonOrdeR=MonomialP::LeftIsGEQLexicographicLastVariableStrongest;
  this->NumberOfComputations=0;
  this->flagDoProgressReport=true;
  this->flagDoSortBasis=true;
  this->flagBasisGuaranteedToGenerateIdeal=false;
  this->flagDoLogDivision=false;
  this->flagSystemProvenToHaveNoSolution=false;
  this->flagSystemSolvedOverBaseField=false;
  this->MaxNumComputations=0;
}

template <class coefficient>
void GroebnerBasisComputation <coefficient>::initForDivisionAlone(List<Polynomial<coefficient> >& inputOutpuT, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("GroebnerBasisComputation::initForDivisionAlone");
  if (inputOutpuT.size<=0)
  { std::cout << "This is a programming error: I cannot transform an empty list to a Groebner basis. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  this->theBasiS=inputOutpuT;
  this->leadingMons.SetSize(inputOutpuT.size);
  this->leadingCoeffs.SetSize(inputOutpuT.size);
  for (int i=0; i<this->theBasiS.size; i++)
  { Polynomial<Rational>& curPoly=theBasiS[i];
    int theIndex=curPoly.GetIndexMaxMonomial(this->theMonOrdeR);
    if (theIndex==-1)
    { std::cout << "This is a programming error: initialization for polynomial division with respect to at least one zero polynomial. "
      << "If this is a bad user input, it should be handled at an earlier level. Here is the current basis by which we need to divide. "
      << this->theBasiS.ToString() << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    this->leadingMons[i]=curPoly[theIndex];
    this->leadingCoeffs[i]=curPoly.theCoeffs[theIndex];
  }
  this->NumberOfComputations=0;
}

template <class coefficient>
void GroebnerBasisComputation<coefficient>::initForGroebnerComputation(List<Polynomial<coefficient> >& inputOutpuT, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("GroebnerBasisComputation::initForGroebnerComputation");
  if (inputOutpuT.size<=0)
  { std::cout << "This is a programming error: I cannot transform an empty list to a Groebner basis. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  this->basisCandidates.SetSize(0);
  this->theBasiS.SetSize(0);
  this->theBasiS.ReservE(inputOutpuT.size);
  this->leadingMons.SetSize(0);
  this->leadingMons.ReservE(inputOutpuT.size);
  this->leadingCoeffs.SetSize(0);
  this->leadingCoeffs.ReservE(inputOutpuT.size);
  this->NumberOfComputations=0;
}

template<class coefficient>
void GroebnerBasisComputation<coefficient>::ConsistencyCheck()
{ if (this->NumberOfComputations>this->MaxNumComputations+1000)
  { std::cout
    << "This may or may not be a programming error. While handling computation excess limit, I got that NumberOfComputations is much larger than MaxNumComputations. "
    << " I have no explanation for this issue right now, so I am crashing to let you know something is fishy. ";
    std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
}

template<class coefficient>
std::string GroebnerBasisComputation<coefficient>::GetPolynomialStringSpacedMonomials
(const Polynomial<coefficient>& thePoly, const HashedList<MonomialP>& theMonomialOrder, const std::string& extraStyle,
 const std::string& extraHighlightStyle, FormatExpressions* theFormat, List<MonomialP>* theHighLightedMons)
{ std::stringstream out;
  bool wasFirst=true;
  bool isFirst;
  int countMons=0;
  for (int i=0; i<theMonomialOrder.size; i++)
  { int theIndex= thePoly.theMonomials.GetIndex(theMonomialOrder[i]);
    if (theIndex==-1)
    { out << "<td" << extraStyle << ">" << "</td>";
      continue;
    }
    countMons++;
    isFirst=wasFirst;
    wasFirst=false;
    bool useHighlightStyle=false;
    if (theHighLightedMons!=0)
      if (theHighLightedMons->Contains(theMonomialOrder[i]))
        useHighlightStyle=true;
    out << "<td" << extraStyle << ">";
    if (useHighlightStyle)
      out << "<span style=\"color:red\">";
    out << Polynomial<Rational>::GetBlendCoeffAndMon(thePoly[theIndex], thePoly.theCoeffs[theIndex], !isFirst, theFormat);
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
{ return false;
  int indexVar=-1;
  if (!thePoly.IsOneVariableNonConstPoly(&indexVar))
  { std::cout << "This is a programming error: I am being asked to find a solution of a polynomial which is not a one-variable polynomial. The input poly is: "
    << thePoly.ToString() << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  PolynomialSubstitution<coefficient> theSub;
  theSub.MakeIdSubstitution(indexVar+1);
  theSub[indexVar].MakeMonomiaL(0, 1, 1);
  Polynomial<coefficient> minPoly, polyVariableIsFirst=thePoly;
  polyVariableIsFirst.Substitution(theSub);
  assert(false);
}

template <class coefficient>
bool GroebnerBasisComputation<coefficient>::HasImpliedSubstitutions
(List<Polynomial<coefficient> >& inputSystem, PolynomialSubstitution<coefficient>& outputSub, AlgebraicClosureRationals* theAlgebraicClosure, GlobalVariables* theGlobalVariables)
{ int numVars=this->systemSolution.GetElement().size;
  MonomialP tempM;
  Polynomial<coefficient> tempP;
  coefficient theCF;
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
//      std::cout << "<hr>Output sub is: x_{" << j+1 << "}=" << outputSub[j].ToString();
//      if (outputSub[j].IsAConstant(&theConst))
//        this->SetSerreLikeSolutionIndex(j, theConst);
      //std::cout << "<br>Current solution candidate is: " << this->systemSolution.GetElement().ToString();
      return true;
    }
    int oneVarIndex;
    if (tempP.IsOneVariableNonConstPoly(&oneVarIndex))
      if (theAlgebraicClosure!=0)
        if (this->GetOneVarPolySolution(tempP, theCF, *theAlgebraicClosure, theGlobalVariables))
        { outputSub.MakeIdSubstitution(numVars);
          outputSub[oneVarIndex].MakeConst(theCF);
          return true;
        }
  }
  return false;
}

template <class coefficient>
int GroebnerBasisComputation<coefficient>::GetPreferredSerreSystemSubIndex()
{ const MonomialP& theMon=this->theBasiS[0].GetMaxMonomial(this->theMonOrdeR);
  for (int i=0; i<theMon.GetMinNumVars(); i++)
    if (theMon(i)>0)
      return i;
  return -1;
}

template <class coefficient>
void GroebnerBasisComputation<coefficient>::BackSubstituteIntoSinglePoly(Polynomial<coefficient>& thePoly, int theIndex, PolynomialSubstitution<coefficient>& theFinalSub, GlobalVariables* theGlobalVariables)
{ Polynomial<coefficient> tempP;
  tempP.MakeMonomiaL(theIndex, 1, 1);
  if (thePoly==tempP)
    return;
  //std::cout << "<hr> Back substituting in x_{" << theIndex+1 << "}:=" << thePoly.ToString();
  thePoly.Substitution(theFinalSub);
  bool changed=false;
  for (int i=0; i<thePoly.size(); i++)
    for (int j=0; j<thePoly[i].GetMinNumVars(); j++)
      if (thePoly[i](j)!=0)
      { if (!this->solutionsFound.GetElement().selected[j])
          this->SetSerreLikeSolutionIndex(j, 0);
        else
          if (this->systemSolution.GetElement()[j]!=0)
          { std::cout << "This is a programming error: variable index " << j+1 << " is supposed to be a free parameter, i.e., be set to zero, but "
            << "instead it has a non-zero value. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
            assert(false);
          }
        theFinalSub[j]=0;
        changed=true;
      }
  if (changed)
    thePoly.Substitution(theFinalSub);
  coefficient tempCF;
  if (!thePoly.IsAConstant(&tempCF))
  { std::cout << "\n<br>\nThis is a programming error: after carrying all implied substitutions the polynomial is not a constant, rather equals "
    << thePoly.ToString() << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
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
bool GroebnerBasisComputation<coefficient>::IsContradictoryReducedSystem(const List<Polynomial<coefficient> >& input)
{ if (input.size==1)
    if (input[0].IsAConstant())
      return true;
  return false;
}

template <class coefficient>
void GroebnerBasisComputation<coefficient>::SolveSerreLikeSystemRecursively
(List<Polynomial<coefficient> >& inputSystem, AlgebraicClosureRationals* theAlgebraicClosure, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("GroebnerBasisComputation::SolveSerreLikeSystemRecursively");
  RecursionDepthCounter theCounter(&this->RecursionCounterSerreLikeSystem);
  ProgressReport theReport1(theGlobalVariables);
  ProgressReport theReport2(theGlobalVariables);
  if (theGlobalVariables!=0)
  { std::stringstream out;
    out << "<br>Recursion depth: " << this->RecursionCounterSerreLikeSystem << "<br>Solving the system\n<br>\n" << inputSystem.ToString();
    theReport1.Report(out.str());
  }
  bool changed=true;
  PolynomialSubstitution<coefficient> theSub;
  List<PolynomialSubstitution<coefficient> > theImpliedSubs;
  theImpliedSubs.ReservE(inputSystem.size);
  while (changed)
  { this->NumberOfComputations=0;
    //std::cout << "<br>Transforming to reduced groebner basis: " << inputSystem.ToString();
    bool success=this->TransformToReducedGroebnerBasis(inputSystem, theGlobalVariables);
    //if (!success)
    //  std::cout << "<br>Failed to reduce system!";
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
    changed=this->HasImpliedSubstitutions(inputSystem, theSub, theAlgebraicClosure, theGlobalVariables);
    if (changed)
    { theImpliedSubs.AddOnTop(theSub);
      for (int i=0; i<inputSystem.size; i++)
        inputSystem[i].Substitution(theSub);
    }
  }
  //std::cout << "<br>System has no more implied subs. At the moment, the system is: " << inputSystem.ToString();
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
  List<Polynomial<coefficient> > oldSystem=inputSystem;
  GroebnerBasisComputation newComputation;
  newComputation.RecursionCounterSerreLikeSystem=this->RecursionCounterSerreLikeSystem;

  newComputation.MaxNumComputations=this->MaxNumComputations;
  newComputation.systemSolution=this->systemSolution;
  newComputation.solutionsFound=this->solutionsFound;
  newComputation.flagSystemProvenToHaveNoSolution=false;
  newComputation.flagSystemSolvedOverBaseField=false;
  newComputation.flagSystemProvenToHaveSolution=false;

  int theVarIndex=this->GetPreferredSerreSystemSubIndex();
  if (theVarIndex==-1)
  { std::cout << "This is a programming error: preferred substitution variable index is -1. Input system equals: " << inputSystem.ToString()
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  theSub.MakeIdSubstitution(this->systemSolution.GetElement().size);
  theSub[theVarIndex]=0;
  //std::cout << "<br>Setting x_{" << theVarIndex+1 << "}:=0";
  newComputation.SetSerreLikeSolutionIndex(theVarIndex, 0);
  //std::cout << "<br>Input system before sub first recursive call. " << inputSystem.ToString();
  for (int i=0; i<inputSystem.size; i++)
    inputSystem[i].Substitution(theSub);
  //std::cout << "<br>Input system after sub first recursive call. " << inputSystem.ToString();

  newComputation.SolveSerreLikeSystemRecursively(inputSystem, theAlgebraicClosure, theGlobalVariables);
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
  newComputation.SolveSerreLikeSystemRecursively(inputSystem, theAlgebraicClosure, theGlobalVariables);
  if (newComputation.flagSystemSolvedOverBaseField)
  { *this=newComputation;
    this->BackSubstituteIntoPolySystem(theImpliedSubs, theGlobalVariables);
    return;
  }
  if (newComputation.flagSystemProvenToHaveSolution)
    this->flagSystemProvenToHaveSolution=true;
  inputSystem=oldSystem;
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
  this->MaxNumComputations=1000;
  int numVars=0;
//  std::cout << "<hr>" << this->GetCalculatorInputFromSystem(inputSystem) << "<hr>";
  List<Polynomial<coefficient> > workingSystem=inputSystem;
  for (int i=0; i<workingSystem.size; i++)
    numVars=MathRoutines::Maximum(numVars, workingSystem[i].GetMinNumVars());
  this->systemSolution.GetElement().initFillInObject(numVars, 0);
  this->solutionsFound.GetElement().init(numVars);
  this->SolveSerreLikeSystemRecursively(workingSystem, theAlgebraicClosure, theGlobalVariables);
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
      { std::cout << "<br>This is a programming error. Function SolveSerreLikeSystem reports to have found a solution over the base field, "
        << "but substituting the solution back to the original system does not yield a zero system of equations. More precisely, "
        << "the reported solution was " << this->ToStringSerreLikeSolution() << " but substitution in equation " << inputSystem[i].ToString()
        << " yields " << workingSystem[i].ToString() << ". Calculator input: <br>" << this->GetCalculatorInputFromSystem(inputSystem) << " <br>";
        std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }
    }
  }
}

template <class coefficient>
std::string GroebnerBasisComputation<coefficient>::ToStringSerreLikeSolution(FormatExpressions* theFormat)
{ std::stringstream out;
  Polynomial<Rational> tempP;
  for (int i=0; i<this->systemSolution.GetElement().size; i++)
    if (this->solutionsFound.GetElement().selected[i])
    { tempP.MakeMonomiaL(i, 1, 1);
      out << " " << tempP.ToString(theFormat) << " := " << this->systemSolution.GetElement()[i] << ";";
    }
  return out.str();
}

template <class coefficient>
void GroebnerBasisComputation<coefficient>::SetSerreLikeSolutionIndex(int theIndex, const coefficient& theConst)
{ this->systemSolution.GetElement()[theIndex]=theConst;
  if (this->solutionsFound.GetElement().selected[theIndex])
  { std::cout << "This a programming error: attempting to set value to a variable whose value has already been computed. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  this->solutionsFound.GetElement().AddSelectionAppendNewIndex(theIndex);
}
#endif
