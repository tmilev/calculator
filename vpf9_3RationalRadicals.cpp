//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1_3.h"
#include "vpf1_5SubsetsSelections.h"
ProjectInformationInstance ProjectInfoVpf9_3cpp(__FILE__, "Implementation of rational radical extensions. ");

std::string MonomialVector::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  out << "e_{" << this->theIndex+1 << "}";
  return out.str();
}

AlgebraicExtensionRationals* AlgebraicClosureRationals::MergeTwoExtensions
  (const AlgebraicExtensionRationals& left, const AlgebraicExtensionRationals& right)
{ MacroRegisterFunctionWithName("AlgebraicClosureRationals::MergeTwoExtensions");
  if (&left==&right)
    return &this->theAlgebraicExtensions[left.indexInOwner];
  AlgebraicExtensionRationals output;
  output.DimOverRationals=left.DimOverRationals*right.DimOverRationals;
  output.AlgebraicBasisElements.SetSize(output.DimOverRationals);
  for (int i=0; i<left.AlgebraicBasisElements.size; i++)
    for (int j=0; j<right.AlgebraicBasisElements.size; j++)
      output.AlgebraicBasisElements[i*right.DimOverRationals+j].
      AssignTensorProduct(left.AlgebraicBasisElements[i], right.AlgebraicBasisElements[j]);
  output.leftParent=&left;
  output.rightParent=&right;
  output.injectionFromLeftParent.MakeZeroMatrix(output.DimOverRationals, left.DimOverRationals);
  output.injectionFromRightParent.MakeZeroMatrix(output.DimOverRationals, right.DimOverRationals);
  for (int i=0; i<left.AlgebraicBasisElements.size; i++)
    output.injectionFromLeftParent(i*right.DimOverRationals, i)=1;
  for (int i=0; i<right.AlgebraicBasisElements.size; i++)
    output.injectionFromRightParent(i, i)=1;
  output.ReduceMeOnCreation();
  return this->ReduceAndAdd(output);
}

AlgebraicExtensionRationals* AlgebraicClosureRationals::ReduceAndAdd(AlgebraicExtensionRationals& input)
{ MacroRegisterFunctionWithName("AlgebraicClosureRationals::ReduceAndAdd");
  int theIndex=this->theAlgebraicExtensions.AddNoRepetitionOrReturnIndexFirst(input);
  this->theAlgebraicExtensions[theIndex].indexInOwner=theIndex;
  this->theAlgebraicExtensions[theIndex].owner=this;
  this->theAlgebraicExtensions[theIndex].GeneratingElemenT.owner=&this->theAlgebraicExtensions[theIndex];
  return &this->theAlgebraicExtensions[theIndex];
}

AlgebraicExtensionRationals* AlgebraicClosureRationals::GetRationals()
{ return &this->theAlgebraicExtensions[0];
}

bool AlgebraicExtensionRationals::operator==(const AlgebraicExtensionRationals& other)const
{ if (this->owner==other.owner && this->owner!=0)
    return this->indexInOwner==other.indexInOwner;
  return this->AlgebraicBasisElements==other.AlgebraicBasisElements;
}

bool AlgebraicExtensionRationals::CheckBasicConsistency()const
{ if (this->DimOverRationals!=this->AlgebraicBasisElements.size)
  { std::cout << "This is a programming error. " << "Dimension over the rationals equals " << this->DimOverRationals
    << " but I have " << this->AlgebraicBasisElements.size << " basis elements. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
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
  std::cout << "Dim over rationals: " << this->DimOverRationals;
  int counter =0;
  for (theSel.IncrementReturnFalseIfBackToBeginning(); ; theSel.IncrementReturnFalseIfBackToBeginning())
  { this->GeneratingElemenT.owner=this;
    this->GeneratingElemenT.theElt.MakeZero();
    for (int i=0; i<theSel.theInts.size; i++)
    { MonomialVector tempV;
      tempV.MakeEi(i);
      this->GeneratingElemenT.theElt.AddMonomial(tempV, theSel.theInts[i]);
    }
    std::cout << "<br>selection: " << theSel.ToString() << ", generator: " << this->GeneratingElemenT.theElt.ToString();
    this->GeneratingElemenT.GetMultiplicationByMeMatrix(this->GeneratingElementTensorForm);
    std::cout << ", current generator= " << this->GeneratingElementTensorForm.ToStringMatForm();
    this->GeneratingElementTensorForm.GetMatrix(this->GeneratingElementMatForm, this->DimOverRationals);
    std::cout << ", in mat form= " << this->GeneratingElementMatForm.ToString();
    this->theGeneratingElementPowersBasis.SetSize(0);
    currentVect.MakeEi(this->DimOverRationals, 0);
    this->theGeneratingElementPowersBasis.AddOnTop(currentVect);
    do
    { counter ++;
      if (counter>1000)
        assert(false);
      this->GeneratingElementMatForm.ActOnVectorColumn(currentVect);
      this->theGeneratingElementPowersBasis.AddOnTop(currentVect);
      std::cout << "<br>The basis: " << this->theGeneratingElementPowersBasis.ToString()
      << " has rank: " << this->theGeneratingElementPowersBasis.GetRankOfSpanOfElements();
      if (this->theGeneratingElementPowersBasis.size>this->theGeneratingElementPowersBasis.GetRankOfSpanOfElements())
      { this->theGeneratingElementPowersBasis.SetSize(this->theGeneratingElementPowersBasis.size-1);
        break;
      }
    } while (true);
    if (this->theGeneratingElementPowersBasis.size==this->DimOverRationals)
      break;
  }
}

void AlgebraicExtensionRationals::ReduceMeOnCreationPart2()
{
}

void AlgebraicExtensionRationals::ReduceMeOnCreation()
{ MacroRegisterFunctionWithName("AlgebraicExtensionRationals::ReduceMeOnCreation");
  this->ChooseGeneratingElement();
  Polynomial<Rational> theMinPoly, smallestFactor;
  theMinPoly.AssignMinPoly(this->GeneratingElementMatForm);
  Rational oldDeg=theMinPoly.TotalDegree();
  std::cout << "<hr><br>Factoring: " << theMinPoly.ToString() << "</b></hr>";
  bool mustBeTrue=theMinPoly.FactorMeOutputIsSmallestDivisor(smallestFactor, 0);
  if (!mustBeTrue)
  { std::cout << "This is a programming error: failed to factor polynomial " << theMinPoly.ToString()
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  std::cout << "<br>After factoring, min poly=" << theMinPoly.ToString() << " factor= " << smallestFactor.ToString();
  if (smallestFactor.TotalDegree()==oldDeg)
  { this->ReduceMeOnCreationPart2();
    return;
  }
  Matrix<Rational> theBasisChangeMat, theBasisChangeMatInverse;
  theBasisChangeMat.AssignVectorsToColumns(this->theGeneratingElementPowersBasis);
  theBasisChangeMatInverse=theBasisChangeMat;
  theBasisChangeMatInverse.Invert();
  if (this->leftParent!=0)
    this->injectionFromLeftParent.MultiplyOnTheLeft(theBasisChangeMatInverse);
  if (this->rightParent!=0)
    this->injectionFromRightParent.MultiplyOnTheLeft(theBasisChangeMatInverse);
  Polynomial<Rational> zToTheNth, remainderAfterReduction, tempP;
  Matrix<Rational> theProjection;
  int smallestFactorDegree=-1;
  smallestFactor.TotalDegree().IsSmallInteger(&smallestFactorDegree);
  theProjection.init(smallestFactorDegree, this->DimOverRationals);
  theProjection.NullifyAll();
  for (int i=0; i<smallestFactorDegree; i++)
    theProjection(i,i)=1;
  for (int i=smallestFactorDegree; i<this->DimOverRationals; i++)
  { zToTheNth.MakeMonomiaL(0, i, 1, 1);
    zToTheNth.DivideBy(smallestFactor, tempP, remainderAfterReduction);
    for (int j=0; j<remainderAfterReduction.size(); j++)
    { int theIndex;
      remainderAfterReduction[j](0).IsSmallInteger(&theIndex);
      theProjection(theIndex, i)=remainderAfterReduction.theCoeffs[j];
    }
  }
  if (this->leftParent!=0)
    this->injectionFromLeftParent.MultiplyOnTheLeft(theProjection);
  if (this->rightParent!=0)
    this->injectionFromRightParent.MultiplyOnTheRight(theProjection);
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
    this->GeneratingElemenT.theElt.MakeEi(-1, 1, 1);
  else
    this->GeneratingElemenT.theElt.MakeEi(-1, 0, 1);
  this->GeneratingElemenT.GetMultiplicationByMeMatrix(this->GeneratingElementTensorForm);
  this->GeneratingElementTensorForm.GetMatrix(this->GeneratingElementMatForm, this->DimOverRationals);
  this->ReduceMeOnCreationPart2();
}

bool AlgebraicExtensionRationals::CheckNonZeroOwner()const
{ if (this->owner==0)
  { std::cout << "This is a programming error: algebraic extension of rationals is not allowed to have "
    << "zero owner in the current context. "
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
  this->GeneratingElemenT.theElt.MakeEi(-1, 0, 1);
}

void AlgebraicNumberOld::operator=(const Rational& other)
{ this->minPolyIndex=this->theRegistry->RegisterRational(other);
  this->rootIndex=0;
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

unsigned int AlgebraicNumber::HashFunction()const
{ if (this->owner==0)
    return 0;
  return this->owner->HashFunction()+this->theElt.HashFunction();
}

bool AlgebraicNumber::CheckNonZeroOwner()const
{ if (this->owner==0)
  { std::cout << "This is a programming error: algebraic number with non-initialized owner not permitted in the "
    << "current context." << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return true;
}

void AlgebraicNumber::operator+=(const AlgebraicNumber& other)
{ MacroRegisterFunctionWithName("AlgebraicNumber::operator+=");
  this->CheckNonZeroOwner();
  other.CheckNonZeroOwner();
  this->owner= this->owner->owner->MergeTwoExtensions(*this->owner, *other.owner);
  this->theElt.MakeZero();
}

void AlgebraicNumber::SqrtMeDefault()
{ this->RadicalMeDefault(2);
}

void AlgebraicNumber::AssignRationalRadical(const Rational& input, AlgebraicClosureRationals& inputOwner)
{ MacroRegisterFunctionWithName("AlgebraicNumber::AssignRationalRadical");
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
  theOp.AddMonomial(theM, input);
  this->theElt.MakeEi(-1, 1, 1);
  theExtension.AlgebraicBasisElements.AddOnTop(theOp);
  theExtension.ReduceMeOnCreation();
  this->owner= inputOwner.ReduceAndAdd(theExtension);
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

std::string AlgebraicNumberOld::ToString(FormatExpressions* theFormat)const
{ if (this->DisplayString!="")
    return this->DisplayString;
  if (this->minPolyIndex==-1 || this->theRegistry==0)
    return "(NonInitializedAlgebraicNumber)";
  std::stringstream out;
  out << "(rootIndex: " << this->rootIndex << ", minpoly: " << this->GetMinPoly().ToString(theFormat) << ")";
  return out.str();
}

std::string AlgebraicExtensionRationals::ToString(FormatExpressions* theFormat)
{ std::stringstream out;
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML=false;
  tempFormat.flagUseLatex=true;
  for (int i=0; i<this->AlgebraicBasisElements.size; i++)
    out << " \\alpha_{" << i+1 << "}:=" << this->AlgebraicBasisElements[i].ToStringMatForm(&tempFormat)
    << ",  ";
  out << "~Generating ~element: " << this->GeneratingElementMatForm.ToString(&tempFormat);
  return out.str();
}

std::string AlgebraicNumber::ToString(FormatExpressions* theFormat)const
{ if (this->owner==0)
    return "(non-initialized)";
  std::stringstream out;
  out << this->theElt.ToString() << "~ in~ the~ field~ " << this->owner->ToString();
  return out.str();
}

bool AlgebraicNumber::operator==(const AlgebraicNumber& other)const
{ if (this->owner!=other.owner)
    return false;
  return this->theElt==other.theElt;
}

void AlgebraicNumber::operator=(const Rational& other)
{ this->owner=this->owner->owner->GetRationals();
  this->theElt.MakeEi(-1, 0, other);
}

void AlgebraicNumberOld::
ReduceMod
(Polynomial<Rational>& toBeReduced, const List<Polynomial<Rational> >& thePolys,
 List<int>& theNs, Polynomial<Rational>& buffer
 )const
{ if (toBeReduced.IsEqualToZero())
    return;
  if (toBeReduced.GetMinNumVars()!=theNs.size)
  { std::cout << "This is a programming error: function AlgebraicNumberOld::ReduceModAnBm"
    << " expects as input a polynomial of  " << theNs.size << " variables, but got the "
    << toBeReduced.ToString() << " polynomial of "
    << toBeReduced.GetMinNumVars() << "variables instead. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  Rational currentCoeff;
  MonomialP tempM;
//  std::cout << "<br>Reducing " << toBeReduced.ToString() << " mod "
//  << "x_1^" << theN << "- (" << An.ToString() << ") (it has " << An.NumVars << " variables)  and mod "
//  << "x_2^" << theM << "- (" << Bm.ToString() << ") (it has " << Bm.NumVars << " variables)";
  for (int i=0; i<toBeReduced.size(); i++)
    for (int j=0; j<theNs.size; j++)
      if (toBeReduced[i](j)>=theNs[j])
      { toBeReduced.PopMonomial(i, tempM, currentCoeff);
  //      std::cout << " select monomial " << tempM.ToString() << " with coeff " << currentCoeff;
        int thePower=tempM[j].NumShort/theNs[j];
        tempM[j]-=theNs[j]*thePower;
        buffer=thePolys[j];
        buffer.RaiseToPower(thePower);
        buffer.MultiplyBy(tempM, currentCoeff);
  //      std::cout << "toBeReduced: " << toBeReduced.ToString() << " buffer: " << buffer.ToString();
        toBeReduced+=buffer;
        i=-1;
        break;
  //      std::cout << "<br>=" <<  toBeReduced.ToString();
      }
//  std::cout << " <br>to get:  " << toBeReduced.ToString();
}

bool AlgebraicNumberOld::AssignOperation
  (Polynomial<Rational>& theOperationIsModified, const List<AlgebraicNumberOld>& theOperationArguments)
{ MacroRegisterFunctionWithName("AlgebraicNumberOld::AssignOperation");
  List<Polynomial<Rational> > thePolys;
  List<int> theNs;
  MonomialP tempM;
  Rational tempRat;
  Polynomial<Rational> buffer1, buffer2;
  thePolys.SetSize(theOperationArguments.size);
  theNs.SetSize(theOperationArguments.size);
  int ProductNs=1;
  int tempN=0;
  for (int i=0; i<theOperationArguments.size; i++)
  { thePolys[i]=theOperationArguments[i].GetMinPoly();
    thePolys[i].ShiftVariableIndicesToTheRight(i);
    if (!thePolys[i].TotalDegree().IsSmallInteger(&tempN))
    { std::cout << "This may or may not be a programming error. "
      << "I am getting an algebraic operation with a polynomial whose exponent is too large "
      << " or is not an integer. If this is not an error , it "
      << "should be handled at a higher level, however"
      << " I don't have time to fix this right now. Crashing to let you know of the situation. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    theNs[i]=tempN;
    ProductNs*=theNs[i];
    thePolys[i].PopMonomial
    (thePolys[i].GetIndexMaxMonomialLexicographicLastVariableStrongest(), tempM, tempRat);
    thePolys[i]/=tempRat*(-1);
  }
  int ProductNsPlusOne= ProductNs+1;
  if (ProductNsPlusOne>=LargeIntUnsigned::SquareRootOfCarryOverBound)
  { std::cout << "This is a programming error. "
    << "Minimal polynomial out of bounds: the upper bound for the minimal poly "
    << " degree is <" << LargeIntUnsigned::SquareRootOfCarryOverBound
    << " which has been exceeded. Until proper error handling is implemented, I shall crash. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  Matrix<Rational> theDep;
  theDep.init(ProductNs, ProductNsPlusOne);
  theDep.NullifyAll();
  this->ReduceMod(theOperationIsModified, thePolys, theNs, buffer1);
  buffer2.MakeOne(theNs.size);
  for (int i=0; i<ProductNsPlusOne; i++)
  { for (int j=0; j<buffer2.size(); j++)
    { const MonomialP& currentMon=buffer2[j];
      int theIndex=currentMon(0).NumShort;
      for (int k=1; k<theNs.size; k++)
        theIndex=theIndex*theNs[k]+ currentMon(k).NumShort;
      theDep(theIndex, i)=buffer2.theCoeffs[j];
    }
    if (i!=ProductNs)
    { buffer2*=theOperationIsModified;
      this->ReduceMod(buffer2, thePolys, theNs, buffer1);
    }
  }
  std::cout << "<br>The matrix: " << theDep.ToString();
  Vectors<Rational> theEigenVectors;
  theDep.GetZeroEigenSpace(theEigenVectors);
  std::cout << "<br>result eigenvectors: " << theEigenVectors.ToString();
  if (theEigenVectors.size<1)
  { std::cout << "This is a programing error: I am asked to find the eigenspace of "
    << " a matrix with more columns than rows, but I get "
    << " zero eigenvectors. Something is very wrong. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  Polynomial<Rational> newMinPoly;
  newMinPoly.MakeZero();
  tempM.MakeOne(1);
  for (int i=0; i<theEigenVectors[0].size; i++)
  { tempM[0]=i;
    newMinPoly.AddMonomial(tempM, theEigenVectors[0][i]);
  }
  newMinPoly.ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy();
  this->minPolyIndex= this->theRegistry->theMinPolys.AddNoRepetitionOrReturnIndexFirst(newMinPoly);
//  this->theRegistry->theOrigins.SetSize(this->theRegistry.theOrigins.size+1);
  return true;
}

const Polynomial<Rational>& AlgebraicNumberOld::GetMinPoly()const
{ return this->theRegistry->theMinPolys[this->minPolyIndex];
}

bool AlgebraicNumberOld::AssignRadical(const LargeInt& undertheRadical, int theRadical)
{ Polynomial<Rational> theMinPoly;
  MonomialP tempM;
  tempM.MakeOne(1);
  tempM[0]=theRadical;
  theMinPoly.MakeZero();
  theMinPoly.AddMonomial(tempM, 1);
  tempM[0]=0;
  theMinPoly.AddMonomial(tempM, undertheRadical);
  this->rootIndex=0;
  this->minPolyIndex=this->theRegistry->theMinPolys.AddNoRepetitionOrReturnIndexFirst(theMinPoly);
//  this->theRegistry.theOrigins.SetSize(this->theRegistry.theOrigins.size+1);
  std::stringstream out;
  out << "\\sqrt{{}" << undertheRadical.ToString() << "}";
  this->DisplayString= out.str();
  return true;
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
