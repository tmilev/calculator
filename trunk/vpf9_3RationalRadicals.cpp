//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfImplementationHeader2Math051_PolynomialComputations_Basic.h"
#include "vpfHeader2Math2_AlgebraicNumbers.h"
#include "vpfHeader2Math5_SubsetsSelections.h"
ProjectInformationInstance ProjectInfoVpf9_3cpp(__FILE__, "Implementation of rational radical extensions. ");

std::string MonomialVector::ToString(FormatExpressions* theFormat)const
{ if (theFormat!=0)
    if (this->theIndex< theFormat->vectorSpaceEiBasisNames.size && this->theIndex>=0)
      return theFormat->vectorSpaceEiBasisNames[this->theIndex];
  std::stringstream out;
  out << "e_{" << this->theIndex+1 << "}";
  return out.str();
}

bool AlgebraicClosureRationals::CheckConsistency()const
{ if (this->thePairPairing.size!=this->thePairs.size || this->injectionsLeftParenT.size !=this->thePairs.size || this->injectionsRightParenT.size!=this->thePairs.size)
  { std::cout << "This is a programming error: I have " << this->thePairs.size << " pairs, " << this->thePairPairing.size << " pair pairings, "
    << this->injectionsLeftParenT.size << " left injections, " << this->injectionsRightParenT.size << " right injections."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return true;
}

std::string AlgebraicClosureRationals::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  out << "Total " << this->theAlgebraicExtensions.size << " algebraic extensions. ";
  for (int i=0; i<this->theAlgebraicExtensions.size; i++)
  { out << "<br>Extension " << i+1 << ": " << this->theAlgebraicExtensions[i].ToString();
  }
  return out.str();
}

void AlgebraicClosureRationals::AddPairWithInjection
(const AlgebraicExtensionRationals& left, const AlgebraicExtensionRationals& right, const AlgebraicExtensionRationals& tensorProd,
 MatrixTensor<Rational>& inputInjectionFromLeft, MatrixTensor<Rational>& inputInjectionFromRight)
{ Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity> currentPair;
  currentPair.Object1=left.indexInOwner;
  currentPair.Object2=right.indexInOwner;
  if (this->thePairs.Contains(currentPair))
    return;
  this->thePairs.AddOnTop(currentPair);
  this->thePairPairing.AddOnTop(tensorProd.indexInOwner);
//  std::cout << "<br><br>\n\n\n\n<br><br>left and right injections: "
//  << (inputInjectionFromLeft.ToString())
//  << " and<br><br> "
//  << (inputInjectionFromRight.ToString());
  this->injectionsLeftParenT.AddOnTop(inputInjectionFromLeft);
  this->injectionsRightParenT.AddOnTop(inputInjectionFromRight);
}

AlgebraicExtensionRationals* AlgebraicClosureRationals::MergeTwoExtensionsAddOutputToMe(AlgebraicExtensionRationals& left, AlgebraicExtensionRationals& right)
{ MacroRegisterFunctionWithName("AlgebraicClosureRationals::MergeTwoExtensionsAddOutputToMe");
  if (left.owner!=this || right.owner!=this || left.indexInOwner==-1 || right.indexInOwner==-1)
  { std::cout << "This is a programming error: I am asked to merge fields that do not belong to me. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  if (&left==&right)
    return &this->theAlgebraicExtensions[left.indexInOwner];
  this->CheckConsistency();
  Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity> currentPair;
  currentPair.Object1=left.indexInOwner;
  currentPair.Object2=right.indexInOwner;
  int indexPairing=this->thePairs.GetIndex(currentPair);
  if (indexPairing!=-1)
    return &this->theAlgebraicExtensions[this->thePairPairing[indexPairing]];
  MatrixTensor<Rational> leftInjection, rightInjection;
  AlgebraicExtensionRationals output;
  this->MergeTwoExtensions(left, right, output, &leftInjection, &rightInjection);
  if (this->theAlgebraicExtensions.Contains(output))
  { this->AddPairWithInjection(left, right, output, leftInjection, rightInjection);
    return &this->theAlgebraicExtensions[this->theAlgebraicExtensions.GetIndex(output)];
  }
  if (!output.flagIsQuadraticRadicalExtensionRationals)
    for (int i=0; i<this->theAlgebraicExtensions.size; i++)
      if (output.DimOverRationals==this->theAlgebraicExtensions[i].DimOverRationals)
      { AlgebraicExtensionRationals tempExtension;
        MatrixTensor<Rational> leftInjectionSecond, rightInjectionSecond, theIso;
        this->MergeTwoExtensions(output, this->theAlgebraicExtensions[i], tempExtension, &leftInjectionSecond, &rightInjectionSecond);
        if (tempExtension.DimOverRationals==output.DimOverRationals)
        { theIso=rightInjectionSecond;
          theIso.Invert();
          theIso*=leftInjectionSecond;
          leftInjection.MultiplyOnTheLeft(theIso);
          rightInjection.MultiplyOnTheLeft(theIso);
          this->AddPairWithInjection(left, right, this->theAlgebraicExtensions[i], leftInjection, rightInjection);
          return &this->theAlgebraicExtensions[i];
        }
      }
  this->AddMustBeNew(output);
  this->AddPairWithInjection(left, right, this->theAlgebraicExtensions.LastObject(), leftInjection, rightInjection);
  return &this->theAlgebraicExtensions[this->theAlgebraicExtensions.size-1];
}

void AlgebraicExtensionRationals::GetMultiplicativeOperatorFromRadicalSelection(const Selection& theSel, MatrixTensor<Rational>& outputOp)
{ outputOp.MakeZero();
  Selection vectorActedOnSel, resultVectorSel;
  vectorActedOnSel.init(this->theQuadraticRadicals.size);
  resultVectorSel.init(this->theQuadraticRadicals.size);
  Rational theCoeff;
  MonomialMatrix tempM;
  do
  { theCoeff=1;
    for (int i=0; i<this->theQuadraticRadicals.size; i++)
    { if (vectorActedOnSel.selected[i] && theSel.selected[i])
      { resultVectorSel.selected[i]=false;
        theCoeff*=this->theQuadraticRadicals[i];
      } else if (!vectorActedOnSel.selected[i] && !theSel.selected[i])
        resultVectorSel.selected[i]=false;
      else
        resultVectorSel.selected[i]=true;
    }
    resultVectorSel.ComputeIndicesFromSelection();
    tempM.MakeEij(this->GetIndexFromRadicalSelection(resultVectorSel), this->GetIndexFromRadicalSelection(vectorActedOnSel));
    outputOp.AddMonomial(tempM, theCoeff);
  } while (vectorActedOnSel.IncrementReturnFalseIfBackToBeginning());
}

void AlgebraicExtensionRationals::ComputeDisplayStringsFromRadicals()
{ if (!this->flagIsQuadraticRadicalExtensionRationals)
    return;
  this->DisplayNamesBasisElements.SetSize(this->DimOverRationals);
  Selection theSel;
  theSel.init(this->theQuadraticRadicals.size);
  do
  { std::stringstream out;
    for (int i=0; i<theSel.CardinalitySelection; i++)
    { const LargeInt& theRad=this->theQuadraticRadicals[theSel.elements[i]];
      out << "\\sqrt{" << theRad.ToString() << "}";
    }
    this->DisplayNamesBasisElements[this->GetIndexFromRadicalSelection(theSel)]=out.str();
  } while (theSel.IncrementReturnFalseIfBackToBeginning());
}

int AlgebraicExtensionRationals::GetIndexFromRadicalSelection(const Selection& theSel)
{ if (theSel.MaxSize>30)
  { std::cout << "This is a programming error: the algebraic extension is too large to be handled by the current data structures. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  int result=0;
  for (int i=theSel.MaxSize-1; i>=0; i--)
  { result*=2;
    if (theSel.selected[i])
      result+=1;
  }
  return result;
}

void AlgebraicClosureRationals::MergeTwoQuadraticRadicalExtensions
(AlgebraicExtensionRationals& left, AlgebraicExtensionRationals& right, AlgebraicExtensionRationals& output, MatrixTensor<Rational>* injectionFromLeftParent,
 MatrixTensor<Rational>* injectionFromRightParent)
{ MacroRegisterFunctionWithName("AlgebraicClosureRationals::MergeTwoQuadraticRadicalExtensions");
  if (!left.flagIsQuadraticRadicalExtensionRationals || !right.flagIsQuadraticRadicalExtensionRationals)
  { std::cout << "Thi is a programming error: AlgebraicClosureRationals::MergeTwoQuadraticRadicalExtensions "
    << "with at least one of two arguments that is not a quadratic radical extension of the rationals. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  //refine primes:
  output.theQuadraticRadicals.Clear();
  output.theQuadraticRadicals.AddOnTopNoRepetition(left.theQuadraticRadicals);
  output.theQuadraticRadicals.AddOnTopNoRepetition(right.theQuadraticRadicals);
  bool found=true;
  LargeIntUnsigned candidateGCD, leftQuotient, rightQuotient;
  while (found)
  { found=false;
    for (int i=0; i<output.theQuadraticRadicals.size; i++)
      for (int j=i+1; j<output.theQuadraticRadicals.size; j++)
      { if (output.theQuadraticRadicals[i]==-1 || output.theQuadraticRadicals[j]==-1)
          continue;
        LargeIntUnsigned::gcd(output.theQuadraticRadicals[i].value, output.theQuadraticRadicals[j].value, candidateGCD);
        if (candidateGCD>1)
        { leftQuotient=output.theQuadraticRadicals[i].value/candidateGCD;
          rightQuotient=output.theQuadraticRadicals[j].value/candidateGCD;
          output.theQuadraticRadicals.RemoveIndexSwapWithLast(j);
          output.theQuadraticRadicals.RemoveIndexSwapWithLast(i);
          if (leftQuotient>1)
            output.theQuadraticRadicals.AddOnTopNoRepetition((LargeInt) leftQuotient);
          if (rightQuotient>1)
            output.theQuadraticRadicals.AddOnTopNoRepetition((LargeInt) rightQuotient);
          output.theQuadraticRadicals.AddOnTopNoRepetition((LargeInt) candidateGCD);
          found=true;
        }
      }
  }
  output.theQuadraticRadicals.QuickSortAscending();
  output.flagIsQuadraticRadicalExtensionRationals=true;
  Selection largerFieldSel;
  largerFieldSel.init(output.theQuadraticRadicals.size);
  if (output.theQuadraticRadicals.size>16)
  { std::cout << "Computing with fields whose dimension over the rationals is greater than 2^16 is not allowed. "
    << "Such computations are too large for the current implementation of algberaic extensions of the rationals. I am crashing "
    << " to let you know the computational limit is reached. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  output.DimOverRationals=MathRoutines::TwoToTheNth(output.theQuadraticRadicals.size);
  output.AlgebraicBasisElements.SetSize(output.DimOverRationals);
  do
    output.GetMultiplicativeOperatorFromRadicalSelection(largerFieldSel, output.AlgebraicBasisElements[output.GetIndexFromRadicalSelection(largerFieldSel)]);
  while(largerFieldSel.IncrementReturnFalseIfBackToBeginning());
  Selection smallerFieldSel;
  AlgebraicExtensionRationals* smallerField=&left;
  MatrixTensor<Rational>* currentInjection=injectionFromLeftParent;
  for (int i=0; i<2; i++, smallerField=&right, currentInjection=injectionFromRightParent)
    if (currentInjection!=0)
    { currentInjection->MakeZero();
      smallerFieldSel.init(smallerField->theQuadraticRadicals.size);
      do
      { largerFieldSel.initNoMemoryAllocation();
        for (int j=0; j<smallerField->theQuadraticRadicals.size; j++)
          if (smallerFieldSel.selected[j])
          { if (smallerField->theQuadraticRadicals[j]==-1)
            { largerFieldSel.AddSelectionAppendNewIndex(output.theQuadraticRadicals.GetIndex(-1));
              continue;
            }
            for (int k=0; k<output.theQuadraticRadicals.size; k++)
              if (smallerField->theQuadraticRadicals[j]%output.theQuadraticRadicals[k]==0 && output.theQuadraticRadicals[k]!=-1)
                largerFieldSel.AddSelectionAppendNewIndex(k);
          }
//        std::cout << "<hr>smaller field sel: " << smallerFieldSel.ToString() << " larger field sel: " << largerFieldSel.ToString();
        (*currentInjection).AddMonomial(MonomialMatrix(output.GetIndexFromRadicalSelection(largerFieldSel), smallerField->GetIndexFromRadicalSelection(smallerFieldSel)),1);
      } while (smallerFieldSel.IncrementReturnFalseIfBackToBeginning());
    }
//  std::cout << "<hr>Computing display strings";
  output.ComputeDisplayStringsFromRadicals();
}

void AlgebraicClosureRationals::MergeTwoExtensions
(AlgebraicExtensionRationals& left, AlgebraicExtensionRationals& right, AlgebraicExtensionRationals& output, MatrixTensor<Rational>* injectionFromLeftParent,
 MatrixTensor<Rational>* injectionFromRightParent)
{ MacroRegisterFunctionWithName("AlgebraicClosureRationals::MergeTwoExtensions");
  if (&left==&right)
  { output=left;
    return;
  }
  if (left.flagIsQuadraticRadicalExtensionRationals && right.flagIsQuadraticRadicalExtensionRationals)
  { this->MergeTwoQuadraticRadicalExtensions(left, right, output, injectionFromLeftParent, injectionFromRightParent);
    return;
  }
  output.DimOverRationals=left.DimOverRationals*right.DimOverRationals;
  output.AlgebraicBasisElements.SetSize(output.DimOverRationals);
  for (int i=0; i<left.AlgebraicBasisElements.size; i++)
    for (int j=0; j<right.AlgebraicBasisElements.size; j++)
      output.AlgebraicBasisElements[i*right.DimOverRationals+j].
      AssignTensorProduct(left.AlgebraicBasisElements[i], right.AlgebraicBasisElements[j]);
  if (injectionFromLeftParent!=0)
  { injectionFromLeftParent->MakeZero();
    for (int i=0; i<left.AlgebraicBasisElements.size; i++)
      injectionFromLeftParent->AddMonomial(MonomialMatrix(i*right.DimOverRationals, i),1);
  }
  if (injectionFromRightParent!=0)
  { injectionFromRightParent->MakeZero();
    for (int i=0; i<right.AlgebraicBasisElements.size; i++)
      injectionFromRightParent->AddMonomial(MonomialMatrix(i, i),1);
  }
  if (left.DimOverRationals==left.DisplayNamesBasisElements.size && right.DimOverRationals==right.DisplayNamesBasisElements.size)
  { output.DisplayNamesBasisElements.SetSize(left.DisplayNamesBasisElements.size*right.DisplayNamesBasisElements.size);
    for (int i=0; i<left.AlgebraicBasisElements.size; i++)
      for (int j=0; j<right.AlgebraicBasisElements.size; j++)
      { if (left.DisplayNamesBasisElements[i]=="1")
          output.DisplayNamesBasisElements[i*right.DimOverRationals+j]=right.DisplayNamesBasisElements[j];
        else if (right.DisplayNamesBasisElements[j]=="1")
          output.DisplayNamesBasisElements[i*right.DimOverRationals+j]=left.DisplayNamesBasisElements[i];
        else
          output.DisplayNamesBasisElements[i*right.DimOverRationals+j]=left.DisplayNamesBasisElements[i]+right.DisplayNamesBasisElements[j];
      }
  }
  double timeStart=0;
  if (this->theGlobalVariables!=0)
    timeStart = this->theGlobalVariables->GetElapsedSeconds();
  output.ReduceMeOnCreation(injectionFromLeftParent, injectionFromRightParent);
  if (this->theGlobalVariables!=0)
    std::cout << "<hr> Time needed to reduce me on creation: " << this->theGlobalVariables->GetElapsedSeconds()-timeStart;
}

void AlgebraicClosureRationals::AddMustBeNew(AlgebraicExtensionRationals& input)
{ MacroRegisterFunctionWithName("AlgebraicClosureRationals::AddMustBeNew");
  if (this->theAlgebraicExtensions.Contains(input))
  { std::cout << "This is a programming error: attempting to add an algebraic extension already contained in the "
    << "list of algebraic extensions. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  int theIndex=this->theAlgebraicExtensions.size;
  this->theAlgebraicExtensions.AddOnTop(input);
  this->theAlgebraicExtensions[theIndex].indexInOwner=theIndex;
  this->theAlgebraicExtensions[theIndex].owner=this;
  this->theAlgebraicExtensions[theIndex].GeneratingElemenT.owner=&this->theAlgebraicExtensions[theIndex];
}

bool AlgebraicExtensionRationals::operator==(const AlgebraicExtensionRationals& other)const
{ return this->AlgebraicBasisElements==other.AlgebraicBasisElements;
}

bool AlgebraicExtensionRationals::CheckBasicConsistency()const
{ if (this->DimOverRationals!=this->AlgebraicBasisElements.size)
  { std::cout << "This is a programming error. " << "Dimension over the rationals equals " << this->DimOverRationals << " but I have "
    << this->AlgebraicBasisElements.size << " basis elements. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return true;
}

void AlgebraicExtensionRationals::ChooseGeneratingElement()
{ MacroRegisterFunctionWithName("AlgebraicExtensionRationals::ChooseGeneratingElement");
  this->CheckBasicConsistency();
  SelectionPositiveIntegers theSel;
  theSel.init(this->DimOverRationals);
  this->theGeneratingElementPowersBasis.SetSize(0);
  Vector<Rational> currentVect;
//  std::cout << "Dim over rationals: " << this->DimOverRationals;
//  int counter =0;
  for (theSel.IncrementReturnFalseIfBackToBeginning(); ; theSel.IncrementReturnFalseIfBackToBeginning())
  { this->GeneratingElemenT.owner=this;
    this->GeneratingElemenT.theElt.MakeZero();
    for (int i=0; i<theSel.theInts.size; i++)
    { MonomialVector tempV;
      tempV.MakeEi(i);
      this->GeneratingElemenT.theElt.AddMonomial(tempV, theSel.theInts[i]);
    }
//    std::cout << "<br>selection: " << theSel.ToString() << ", generator: " << this->GeneratingElemenT.theElt.ToString();
    this->GeneratingElemenT.GetMultiplicationByMeMatrix(this->GeneratingElementTensorForm);
//    std::cout << ", current generator= " << this->GeneratingElementTensorForm.ToStringMatForm();
    this->GeneratingElementTensorForm.GetMatrix(this->GeneratingElementMatForm, this->DimOverRationals);
//    std::cout << ", in mat form= " << this->GeneratingElementMatForm.ToString();
    this->theGeneratingElementPowersBasis.SetSize(0);
    currentVect.MakeEi(this->DimOverRationals, 0);
    this->theGeneratingElementPowersBasis.AddOnTop(currentVect);
    do
    { //counter ++;
      //if (counter>1000)
      //  assert(false);
      this->GeneratingElementMatForm.ActOnVectorColumn(currentVect);
      this->theGeneratingElementPowersBasis.AddOnTop(currentVect);
      //std::cout << "<br>The basis: " << this->theGeneratingElementPowersBasis.ToString()
      //<< " has rank: " << this->theGeneratingElementPowersBasis.GetRankOfSpanOfElements();
      if (this->theGeneratingElementPowersBasis.size>this->theGeneratingElementPowersBasis.GetRankOfSpanOfElements())
      { this->theGeneratingElementPowersBasis.SetSize(this->theGeneratingElementPowersBasis.size-1);
        break;
      }
    } while (true);
    if (this->theGeneratingElementPowersBasis.size==this->DimOverRationals)
      break;
  }
}

void AlgebraicExtensionRationals::ReduceMeOnCreation(MatrixTensor<Rational>* injectionFromLeftParent, MatrixTensor<Rational>* injectionFromRightParent)
{ MacroRegisterFunctionWithName("AlgebraicExtensionRationals::ReduceMeOnCreation");
//  double timeStart=0;
//  if (this->owner!=0)
//    if (this->owner->theGlobalVariables!=0)
//      timeStart=this->owner->theGlobalVariables->GetElapsedSeconds();
  this->ChooseGeneratingElement();
//  if (this->owner!=0)
//    if (this->owner->theGlobalVariables!=0)
//      std::cout << "<hr> Time needed to chose generating element: " << this->owner->theGlobalVariables->GetElapsedSeconds()-timeStart;
//  std::cout << "Reducing: " << this->ToString();
  if (this->flagIsQuadraticRadicalExtensionRationals)
    return;
  Polynomial<Rational> theMinPoly, smallestFactor;
  theMinPoly.AssignMinPoly(this->GeneratingElementMatForm);
  Rational oldDeg=theMinPoly.TotalDegree();
//  std::cout << "<hr><br>Factoring: " << theMinPoly.ToString() << "</b></hr>";
  bool mustBeTrue=theMinPoly.FactorMeOutputIsSmallestDivisor(smallestFactor, 0);
  if (!mustBeTrue)
  { std::cout << "This is a programming error: failed to factor polynomial " << theMinPoly.ToString()
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  std::cout << "<br>After factoring, min poly=" << theMinPoly.ToString() << " factor= " << smallestFactor.ToString();
  if (smallestFactor.TotalDegree()==oldDeg)
    return;
  std::cout << "<br>Min poly factors.";
  MatrixTensor<Rational> theBasisChangeMat, theBasisChangeMatInverse;
  theBasisChangeMat.AssignVectorsToColumns(this->theGeneratingElementPowersBasis);
  theBasisChangeMatInverse=theBasisChangeMat;
  theBasisChangeMatInverse.Invert();
  if (injectionFromLeftParent!=0)
    injectionFromLeftParent->MultiplyOnTheLeft(theBasisChangeMatInverse);
  if (injectionFromRightParent!=0)
    injectionFromRightParent->MultiplyOnTheLeft(theBasisChangeMatInverse);
  Polynomial<Rational> zToTheNth, remainderAfterReduction, tempP;
  MatrixTensor<Rational> theProjection;
  int smallestFactorDegree=-1;
  smallestFactor.TotalDegree().IsSmallInteger(&smallestFactorDegree);
  //theProjection.init(smallestFactorDegree, this->DimOverRationals);
  theProjection.MakeZero();
  for (int i=0; i<smallestFactorDegree; i++)
    theProjection.AddMonomial(MonomialMatrix(i,i),1);
  for (int i=smallestFactorDegree; i<this->DimOverRationals; i++)
  { zToTheNth.MakeMonomiaL(0, i, 1, 1);
    zToTheNth.DivideBy(smallestFactor, tempP, remainderAfterReduction);
    for (int j=0; j<remainderAfterReduction.size(); j++)
    { int theIndex=-1;
      remainderAfterReduction[j](0).IsSmallInteger(&theIndex);
      theProjection.AddMonomial(MonomialMatrix(theIndex, i),remainderAfterReduction.theCoeffs[j]);
    }
  }
  if (injectionFromLeftParent!=0)
    injectionFromLeftParent->MultiplyOnTheLeft(theProjection);
  if (injectionFromRightParent!=0)
    injectionFromRightParent->MultiplyOnTheLeft(theProjection);

  this->DimOverRationals=smallestFactorDegree;
  this->AlgebraicBasisElements.SetSize(this->DimOverRationals);
  MonomialMatrix tempM;
  for (int i=0; i<this->DimOverRationals; i++)
  { this->AlgebraicBasisElements[i].MakeZero();
    for (int j=0; j<this->DimOverRationals; j++)
    { zToTheNth.MakeMonomiaL(0, i+j, 1, 1);
      zToTheNth.DivideBy(smallestFactor, tempP, remainderAfterReduction);
      for (int k=0; k<remainderAfterReduction.size(); k++)
      { int theIndex;
        remainderAfterReduction[k](0).IsSmallInteger(&theIndex);
        tempM.vIndex=theIndex;
        tempM.dualIndex=j;
        this->AlgebraicBasisElements[i].AddMonomial(tempM, remainderAfterReduction.theCoeffs[k]);
      }
    }
  }
  this->GeneratingElemenT.owner=this;
  if (this->DimOverRationals>1)
    this->GeneratingElemenT.theElt.MaKeEi(1, 1);
  else
    this->GeneratingElemenT.theElt.MaKeEi(0, 1);
  this->GeneratingElemenT.GetMultiplicationByMeMatrix(this->GeneratingElementTensorForm);
  this->GeneratingElementTensorForm.GetMatrix(this->GeneratingElementMatForm, this->DimOverRationals);
}

bool AlgebraicExtensionRationals::CheckNonZeroOwner()const
{ if (this->owner==0)
  { std::cout << "This is a programming error: algebraic extension of rationals is not allowed to have zero owner in the current context. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return true;
}

void AlgebraicExtensionRationals::MakeRationals(AlgebraicClosureRationals& inputOwner)
{ this->owner=&inputOwner;
  this->AlgebraicBasisElements.SetSize(1);
  this->AlgebraicBasisElements[0].MakeId(1);
  this->DimOverRationals=1;
  this->GeneratingElemenT.theElt.MaKeEi(0, 1);
}

void AlgebraicNumber::GetMultiplicationByMeMatrix(MatrixTensor<Rational>& output)
{ MacroRegisterFunctionWithName("AlgebraicNumber::GetMultiplicationByMeMatrix");
  this->CheckNonZeroOwner();
  output.MakeZero();
  MatrixTensor<Rational> tempMat;
  for (int i=0; i<this->theElt.size(); i++)
  { tempMat=this->owner->AlgebraicBasisElements[this->theElt[i].theIndex];
    tempMat*=this->theElt.theCoeffs[i];
    output+=tempMat;
  }
}

Rational AlgebraicNumber::GetDenominatorRationalPart()const
{ LargeIntUnsigned resultLCM=1;
  for (int i=0; i<this->theElt.size(); i++)
    resultLCM=LargeIntUnsigned::lcm(resultLCM, this->theElt.theCoeffs[i].GetDenominator());
  return resultLCM;
}

Rational AlgebraicNumber::GetNumeratorRationalPart()const
{ LargeInt resultGCD=1;
  LargeIntUnsigned tempR;
  if (this->theElt.size()>0)
    resultGCD= this->theElt.theCoeffs[0].GetNumerator();
  for (int i=1; i<this->theElt.size(); i++)
  { tempR=resultGCD.value;
    LargeIntUnsigned::gcd(tempR, this->theElt.theCoeffs[i].GetNumerator().value, resultGCD.value);
  }
  return resultGCD;
}

unsigned int AlgebraicNumber::HashFunction()const
{ if (this->owner==0)
    return 0;
  return this->owner->HashFunction()+this->theElt.HashFunction();
}

bool AlgebraicNumber::operator==(int other)const
{ if (this->theElt.IsEqualToZero())
    return other==0;
  if (this->theElt.size()!=1)
    return false;
  if (this->theElt[0]==0)
    return false;
  return this->theElt[0]==other;
}

bool AlgebraicNumber::NeedsBrackets()const
{ return (this->theElt.size()>1);
}

bool AlgebraicNumber::CheckNonZeroOwner()const
{ if (this->owner==0)
  { std::cout << "This is a programming error: algebraic number with non-initialized owner not permitted in the current context."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return true;
}

int AlgebraicClosureRationals::GetIndexIMustContainPair(const AlgebraicExtensionRationals* left, const AlgebraicExtensionRationals* right)
{ Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity> currentPair;
  currentPair.Object1=left->indexInOwner;
  currentPair.Object2=right->indexInOwner;
  return this->thePairs.GetIndexIMustContainTheObject(currentPair);
}

void AlgebraicClosureRationals::GetLeftAndRightInjectionsTensorForm
(const AlgebraicExtensionRationals* left, const AlgebraicExtensionRationals* right, MatrixTensor<Rational>*& outputInjectionFromLeft, MatrixTensor<Rational>*& outputInjectionFromRight)
{ int theIndex=this->GetIndexIMustContainPair(left, right);
  outputInjectionFromLeft=&this->injectionsLeftParenT[theIndex];
  outputInjectionFromRight=&this->injectionsRightParenT[theIndex];
}

void AlgebraicNumber::ConvertToCommonOwner(AlgebraicNumber& left, AlgebraicNumber& right)
{ MacroRegisterFunctionWithName("AlgebraicNumber::ConvertToCommonOwner");
  if (left.owner==right.owner)
    return;
  if (left.owner==0)
  { left.owner=right.owner;
    return;
  }
  if (right.owner==0)
  { right.owner=left.owner;
    return;
  }
  left.CheckNonZeroOwner();
  right.CheckNonZeroOwner();
  AlgebraicClosureRationals* theBigBadOwner=left.owner->owner;
  AlgebraicExtensionRationals* newOwner= theBigBadOwner->MergeTwoExtensionsAddOutputToMe(*left.owner, *right.owner);
  MatrixTensor<Rational>* leftInjection;
  MatrixTensor<Rational>* rightInjection;
  theBigBadOwner->GetLeftAndRightInjectionsTensorForm(left.owner, right.owner, leftInjection, rightInjection);
  left.owner=newOwner;
  right.owner=newOwner;
  leftInjection->ActOnVectorColumn(left.theElt);
  rightInjection->ActOnVectorColumn(right.theElt);
//  std::cout << "<hr>Algebraic closure: " << theBigBadOwner->ToString();
}

void AlgebraicNumber::operator+=(const AlgebraicNumber& other)
{ MacroRegisterFunctionWithName("AlgebraicNumber::operator+=");
  AlgebraicNumber otherCopy=other;
  AlgebraicNumber::ConvertToCommonOwner(*this, otherCopy);
  this->theElt+=otherCopy.theElt;
}

void AlgebraicNumber::operator-=(const AlgebraicNumber& other)
{ MacroRegisterFunctionWithName("AlgebraicNumber::operator-=");
  AlgebraicNumber otherCopy=other;
  AlgebraicNumber::ConvertToCommonOwner(*this, otherCopy);
  this->theElt-=otherCopy.theElt;
}

void AlgebraicNumber::Invert()
{ MacroRegisterFunctionWithName("AlgebraicNumber::Invert");
  if (this->owner==0)
  { if (this->theElt.IsEqualToZero())
    { std::cout << "This is a programming error: division by zero. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    bool isGood=(this->theElt.size()==1);
    if (isGood)
      isGood=(this->theElt[0].theIndex==0);
    if (!isGood)
    { std::cout << "This is a programming error: Algebraic number has no owner, so it must be rational, but it appears to be not. "
      << " as its theElt vector is: " << this->theElt.ToString() << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    this->theElt.theCoeffs[0].Invert();
    return;
  }
  MatrixTensor<Rational> theInverted;
  Matrix<Rational> tempMat2;
  this->GetMultiplicationByMeMatrix(theInverted);
  theInverted.GetMatrix(tempMat2, this->owner->DimOverRationals);
  tempMat2.Invert();
  theInverted=tempMat2;
  this->theElt.MaKeEi(0);
  theInverted.ActOnVectorColumn(this->theElt);
}

void AlgebraicNumber::operator/=(const AlgebraicNumber& other)
{ MacroRegisterFunctionWithName("AlgebraicNumber::operator/=");
  AlgebraicNumber otherCopy=other;
  otherCopy.Invert();
//  std::cout << "<hr>other: " << other.theElt.ToString() << " inverted: " << otherCopy.ToString();
  *this*=otherCopy;
}

void AlgebraicNumber::operator*=(const Rational& other)
{ this->theElt*=(other);
}

void AlgebraicNumber::operator*=(const AlgebraicNumber& other)
{ MacroRegisterFunctionWithName("AlgebraicNumber::operator*=");
  if (other.owner==0)
  { this->theElt*=other.theElt.theCoeffs[0];
    return;
  }
  if (this->owner==0)
  { Rational tempRat=this->theElt.GetMonomialCoefficient(MonomialVector(0));
    *this=other;
    this->theElt*=tempRat;
    return;
  }
  this->CheckNonZeroOwner();
  other.CheckNonZeroOwner();
  AlgebraicNumber otherCopy=other;
//  std::cout << "Converting <hr>" << CGI::GetHtmlMathSpanPure(this->ToString()) << " and <br><br>\n\n\n\n<br><br>"
//  << CGI::GetHtmlMathSpanPure(otherCopy.ToString());
  AlgebraicNumber::ConvertToCommonOwner(*this, otherCopy);
//  std::cout << " <br><br>To get: " << CGI::GetHtmlMathSpanPure(this->ToString()) << "<br>\n\n and  <br><br>\n\n\n\n<br><br> \n"
//  << CGI::GetHtmlMathSpanPure(otherCopy.ToString()) ;
  //std::cout << " <hr>multiplying " << this->theElt.ToString() << " by " << other.theElt.ToString() << " ";
  MatrixTensor<Rational> leftMat, rightMat;
  FormatExpressions tempformat;
  tempformat.flagUseLatex=true;
  tempformat.flagUseHTML=false;
  this->GetMultiplicationByMeMatrix(leftMat);
  otherCopy.GetMultiplicationByMeMatrix(rightMat);
//  std::cout << "<br><br>\n\n\n\n<br><br> in matrix form: " << CGI::GetHtmlMathSpanPure(leftMat.ToStringMatForm(&tempformat)) << " by "
//  << CGI::GetHtmlMathSpanPure(rightMat.ToStringMatForm(&tempformat));
  leftMat*=rightMat;
  this->theElt.MaKeEi(0);
  //std::cout << "matrix " << CGI::GetHtmlMathSpanPure(leftMat.ToStringMatForm(&tempformat));
  leftMat.ActOnVectorColumn(this->theElt);
  //std::cout << this->theElt.ToString();
}

void AlgebraicNumber::SqrtMeDefault()
{ this->RadicalMeDefault(2);
}

void AlgebraicNumber::AssignRational(const Rational& input, AlgebraicClosureRationals& inputOwner)
{ this->owner=0;
  this->theElt.MaKeEi(0, input);
}

bool AlgebraicNumber::AssignRationalQuadraticRadical(const Rational& input, AlgebraicClosureRationals& inputOwner)
{ MacroRegisterFunctionWithName("AlgebraicNumber::AssignRationalRadical");
  if (input==0)
    return false;
  if (input<0 && input!=-1)
  { AlgebraicNumber i;
    i.AssignRationalQuadraticRadical(-1, inputOwner);
    if (!this->AssignRationalQuadraticRadical(-input, inputOwner))
      return false;
    *this*=i;
    return true;
  }
  AlgebraicExtensionRationals theExtension;
  theExtension.owner=&inputOwner;
  theExtension.AlgebraicBasisElements.SetSize(0);
  MatrixTensor<Rational> theOp;
  theExtension.DimOverRationals=2;
  theOp.MakeId(2);
  theExtension.AlgebraicBasisElements.AddOnTop(theOp);
  MonomialMatrix theM;
  theM.MakeEij(1, 0);
  theOp.MakeZero();
  theOp.AddMonomial(theM, 1);
  theM.MakeEij(0, 1);
  LargeInt squareFreeInput=input.GetNumerator();
  squareFreeInput*=input.GetDenominator();
  List<unsigned int> primeFactors;
  List<int> theMults;
  if (!squareFreeInput.value.Factor(primeFactors, theMults))
    return false;
  squareFreeInput.value=1;
  Rational squareRootRationalPart=1;
  squareRootRationalPart/=input.GetDenominator();
  for (int i=0; i<primeFactors.size; i++)
  { if (theMults[i]%2==1)
      squareFreeInput*=primeFactors[i];
    Rational tempLI=primeFactors[i];
    tempLI.RaiseToPower(theMults[i]/2);
    squareRootRationalPart*=tempLI;
  }
//  squareRootRationalPart/=input.GetDenominator();
  if (squareFreeInput.IsEqualToOne())
  { this->AssignRational(squareRootRationalPart, inputOwner);
    return true;
  }
  theOp.AddMonomial(theM, squareFreeInput);
  this->theElt.MaKeEi(1, squareRootRationalPart);
  theExtension.theQuadraticRadicals.AddOnTop(squareFreeInput);
  theExtension.flagIsQuadraticRadicalExtensionRationals=true;
  theExtension.AlgebraicBasisElements.AddOnTop(theOp);
  theExtension.ReduceMeOnCreation();
  theExtension.ComputeDisplayStringsFromRadicals();
  if (!inputOwner.theAlgebraicExtensions.Contains(theExtension))
    inputOwner.AddMustBeNew(theExtension);
  int theIndex=inputOwner.theAlgebraicExtensions.GetIndex(theExtension);
  this->owner=&inputOwner.theAlgebraicExtensions[theIndex];
  return true;
}

void AlgebraicNumber::RadicalMeDefault(int theRad)
{ std::cout << "Not implemented yet!" << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  assert(false);
/*  MatrixTensor<Rational> theRadicalOp;
  theRadicalOp.MakeZero();
  MonomialTensor tempM;
  for (int i=0; i<this->GetMinPoly().size; i++)
  { tempM=this->GetMinPoly()[i];
    tempM.ExponentMeBy(theRad);
    newMinPoly.AddMonomial(tempM, this->GetMinPoly().theCoeffs[i]);
  }
  this->rootIndex=0;
  this->minPolyIndex= this->theRegistry->theMinPolys.AddNoRepetitionOrReturnIndexFirst(newMinPoly);*/
}

std::string AlgebraicExtensionRationals::ToString(FormatExpressions* theFormat)
{ std::stringstream out;
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML=false;
  tempFormat.flagUseLatex=true;
  for (int i=0; i<this->AlgebraicBasisElements.size; i++)
  { if (i<this->DisplayNamesBasisElements.size)
    { if (this->DisplayNamesBasisElements[i]=="")
        out << "1";
      else
        out << this->DisplayNamesBasisElements[i];
    } else
      out << " e_{" << i+1 << "}";
    out << ":=" << this->AlgebraicBasisElements[i].ToStringMatForm(&tempFormat) << ",  ";
  }
  out << "~Generating ~element: " << this->GeneratingElementMatForm.ToString(&tempFormat);
  //out << " <br>Field pairings: (not implemented yet).";
  return out.str();
}

bool AlgebraicNumber::IsRational(Rational* whichRational)const
{ for (int i=0; i<this->theElt.size(); i++)
    if (this->theElt[i].theIndex!=0)
      return false;
    else
      if (whichRational!=0)
        *whichRational=this->theElt.theCoeffs[i];
  return true;
}

bool AlgebraicNumber::IsEqualToZero()const
{ return this->theElt.IsEqualToZero();
}

std::string AlgebraicNumber::ToString(FormatExpressions* theFormat)const
{ if (this->owner==0)
  { if (this->theElt.IsEqualToZero())
      return "0";
    return this->theElt.theCoeffs[0].ToString();
  }
  std::stringstream out;
  FormatExpressions tempFormat;
  tempFormat.vectorSpaceEiBasisNames=this->owner->DisplayNamesBasisElements;
  out << this->theElt.ToString(&tempFormat); //<< "~ in~ the~ field~ " << this->owner->ToString();
  return out.str();
}

bool AlgebraicNumber::operator==(const AlgebraicNumber& other)const
{ if (this->owner!=other.owner)
    return false;
  return this->theElt==other.theElt;
}

void AlgebraicNumber::operator=(const Rational& other)
{ this->owner=0;
  this->theElt.MaKeEi(0, other);
}

std::string ElementZmodP::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  out << "(" << this->theValue.ToString() << " ~mod~ " << this->theModulo.ToString() << ")";
  return out.str();
}

bool ElementZmodP::AssignRational(const Rational& other)
{ this->CheckIamInitialized();
  *this= other.GetNumerator();
  ElementZmodP den;
  den.theModulo=this->theModulo;
  den=other.GetDenominator();
  if (den.IsEqualToZero())
    return false;
  *this/=den;
  return true;
}

void ElementZmodP::operator/=(const ElementZmodP& other)
{ this->CheckIamInitialized();
  this->CheckEqualModuli(other);
  LargeInt theInverted, otherValue, theMod;
  theMod=this->theModulo;
  otherValue=other.theValue;
  theInverted=MathRoutines::InvertXModN(otherValue, theMod);
  *this*=theInverted;
}
