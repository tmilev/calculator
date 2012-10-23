//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader1_3_h_already_included
#define vpfHeader1_3_h_already_included

#include "vpfHeader1_2.h"
static ProjectInformationInstance ProjectInfoVpfHeader1_3
(__FILE__, "Header, math routines. ");

template <class CoefficientType>
class RationalFunction
{
  public:
  Polynomial<CoefficientType> polyForm;
  MemorySaving<List<Polynomial<CoefficientType> > > substitutions;
  //Criterion from Cox, Little, O'Shea:
  static bool CriterionCLOsh
(HashedListSpecialized
 <Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity> >&
 thePairs, List<MonomialP>& theLeadingMons, MonomialP& leadingTermLCM)
  ;
  static void TransformToReducedGroebnerBasisImprovedAlgorithm
  (List<Polynomial<CoefficientType> >& theBasis, MathRoutines::MonomialOrder theMonOrder,
   GlobalVariables* theGlobalVariables
  );
};

class MonomialDiffForm
{
public:
/*  Vector<Rational> theDiffPart;
  unsigned int HashFunction()const
  { return this->HashFunction(*this);
  }
  static unsigned int HashFunction(const MonomialDiffForm& input)
  { return
    input.theDiffPart.HashFunction()*SomeRandomPrimes[1];
  }
  void operator=(const MonomialDiffForm& other)
  { this->thePolyPart=other.thePolyPart;
    this->theDiffPart=other.theDiffPart;
  }
  bool operator==(const MonomialDiffForm& other)const
  { return this->thePolyPart==other.thePolyPart && this->theDiffPart==other.theDiffPart;
  }*/
};

//The default multiplication operation is the exterior product.
template <class CoefficientType>
class DifferentialForm: ElementAssociativeAlgebra<MonomialDiffForm, CoefficientType>
{
  public:
  void operator=(const Polynomial<CoefficientType>& other);
  void DifferentialMe()
  {
  }
  unsigned int HashFunction()const
  { return this->HashFunction(*this);
  }
  static unsigned int HashFunction(const DifferentialForm<CoefficientType>& input)
  { return ElementAssociativeAlgebra<MonomialDiffForm, CoefficientType>::HashFunction(input);
  }
};

class AlgebraicNumberRegistry;

class AlgebraicNumber
{
  friend class List<AlgebraicNumber>;
  friend class Matrix<AlgebraicNumber>;
  AlgebraicNumber():rootIndex(-1),minPolyIndex(-1), theRegistry(0){}

public:
  //Format of minPoly.
  //0. If the algebraic integer is in fact an honest integer
  //"minPoly" is defined to be the empty polynomial. This is different from the mathematical
  //minimal polynomial.
  //1. If the number is an algebraic integer but not an integer, minPoly is defined
  // to be the minimal polynomial of the number with the following assumptions:
  //1.1. Minimal polynomials have relatively prime coefficients.
  //1.2. The leading coefficient of the minimal polynomial is 1.
  int rootIndex;
  int minPolyIndex;
  //  Polynomial<Rational> minPoly;
  std::string DisplayString;
  AlgebraicNumberRegistry* theRegistry;
  friend std::ostream& operator << (std::ostream& output, const AlgebraicNumber& theRat)
  { output << theRat.ToString();
    return output;
  }
  void SqrtMe();
  void RadicalMe(int theRad);
  unsigned int HashFunction()const
  { return this->HashFunction(*this);
  }
  static unsigned int HashFunction(const AlgebraicNumber& input)
  { return input.minPolyIndex*SomeRandomPrimes[0]+input.rootIndex*SomeRandomPrimes[1];
  }
  AlgebraicNumber(AlgebraicNumberRegistry& owner) :rootIndex(-1), minPolyIndex(-1)
  { this->theRegistry=&owner;
  }
  AlgebraicNumber(const AlgebraicNumber& other)
  { this->operator=(other);
  }
  void operator=(const AlgebraicNumber& other)
  { this->minPolyIndex=other.minPolyIndex;
    this->rootIndex=other.rootIndex;
    this->theRegistry=other.theRegistry;
  }
  void MakeOneVarPolyIntoNVarPoly
  (const Polynomial<Rational>& thePoly, Polynomial<Rational>& theOutput, int numVars, int desiredIndex)
  { MonomialP tempM;
    tempM.MakeConst(numVars);
    theOutput.MakeZero(numVars);
    for (int i =0; i<thePoly.size; i++)
    { tempM[desiredIndex]=thePoly[i][0];
      theOutput.AddMonomial(tempM, thePoly.theCoeffs[i]);
    }
  }
  void operator=(const Rational& other);
  bool AssignOperation
  (Polynomial<Rational>& theOperationIsModified, const List<AlgebraicNumber>& theOperationArguments)
  ;
  void ReduceMod
(Polynomial<Rational>& toBeReduced, const List<Polynomial<Rational> >& thePolys,
 List<int>& theNs, Polynomial<Rational>& buffer
 )const
  ;
  bool operator+=(const AlgebraicNumber& other)
  { MacroRegisterFunctionWithName("AlgebraicNumber::operator+=");
    Polynomial<Rational> theOperation;
    theOperation.MakeZero(2);
    MonomialP tempM;
    tempM.MakeConst(2);
    tempM[0]=1;
    theOperation.AddMonomial(tempM, 1);
    tempM[0]=0;
    tempM[1]=1;
    theOperation.AddMonomial(tempM, 1);
    List<AlgebraicNumber> tempList;
    tempList.SetSize(2);
    tempList[0]=*this;
    tempList[1]=other;
    return this->AssignOperation(theOperation, tempList);
  }
  bool operator*=(const AlgebraicNumber& other)
  { MacroRegisterFunctionWithName("AlgebraicNumber::operator+=");
    Polynomial<Rational> theOperation;
    theOperation.MakeZero(2);
    MonomialP tempM;
    tempM.MakeConst(2);
    tempM[0]=1;
    tempM[1]=1;
    theOperation.AddMonomial(tempM, 1);
    List<AlgebraicNumber> tempList;
    tempList.SetSize(2);
    tempList[0]=*this;
    tempList[1]=other;
    return this->AssignOperation(theOperation, tempList);
  }
  std::string ToString(FormatExpressions* theFormat=0)const;
  bool AssignRadical(const LargeInt& undertheRadical, int theRadical);
  const Polynomial<Rational>& GetMinPoly()const;
  bool IsAConstant()
  { return this->GetMinPoly().TotalDegree()==1;
  }
  bool operator==(const AlgebraicNumber& other)const
  { if (this->theRegistry!=other.theRegistry)
    { std::cout << "This is a programming error: comparing two algebraic numbers whose "
      << " algebraic number registries are different."
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    return this->rootIndex==other.rootIndex && this->minPolyIndex==other.minPolyIndex;
  }
  inline bool IsEqualToZero()const
  { return false;
  }
  bool operator> (const AlgebraicNumber& other) const
  { if (this->GetMinPoly()>other.GetMinPoly())
      return true;
    if (other.GetMinPoly()>this->GetMinPoly())
      return false;
    return this->rootIndex>other.rootIndex;
  }
};

class AlgebraicNumberOrigin
{
  public:
  List<List<AlgebraicNumber> > theParents;
  List<Polynomial<Rational> > theOriginOperation;
};

class AlgebraicNumberRegistry
{
  public:
  HashedList<Polynomial<Rational> > theMinPolys;
  List<AlgebraicNumberOrigin> theOrigins;
  int RegisterRational(const Rational& theRational)
  { Polynomial<Rational> theMinP;
    theMinP.MakeDegreeOne(1,0, 1, -theRational);
    theMinP.ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy();
    if (!theMinPolys.Contains(theMinP))
    { this->theMinPolys.AddOnTop(theMinP);
//      this->theOrigins.SetSize(this->theOrigins.size+1);
      return this->theMinPolys.size-1;
    }
    return theMinPolys.GetIndex(theMinP);
  }
};

template <class CoefficientType>
bool RationalFunction<CoefficientType>::CriterionCLOsh
(HashedListSpecialized
 <Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity> >&
 thePairs, List<MonomialP>& theLeadingMons, MonomialP& leadingTermLCM)
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

template <class CoefficientType>
void RationalFunction<CoefficientType>::TransformToReducedGroebnerBasisImprovedAlgorithm
(List<Polynomial<CoefficientType> >& theBasis, MathRoutines::MonomialOrder theMonOrder,
 GlobalVariables* theGlobalVariables
)
{ MacroRegisterFunctionWithName
  ("RationalFunction_CoefficientType::TransformToReducedGroebnerBasisImprovedAlgorithm");
   //This is an implementation of the algorithm on page 106, Cox, Little, O'Shea,
  //Ideals, Varieties, algorithms
  HashedListSpecialized<PairInts > indexPairs;
//  Pair<int, int> currentPair;
  indexPairs.SetExpectedSize(theBasis.size*theBasis.size);
  List<MonomialP> theLeadingMons;
  List<CoefficientType> theLeadingCoeffs;
  theLeadingMons.SetExpectedSize(theBasis.size*2);
  for (int i=0; i<theBasis.size; i++)
  { for (int j=i+1; j<theBasis.size; j++)
      indexPairs.AddOnTop(PairInts (i,j));
    theBasis[i].ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
    int theIndex=theBasis[i].GetIndexMaxMonomial(theMonOrder);
    theLeadingMons.AddOnTop(theBasis[i][theIndex]);
    theLeadingCoeffs.AddOnTop(theBasis[i].theCoeffs[theIndex]);
  }
  if (theBasis.size<=0)
  { std::cout << "This is a programming error: "
    << "transforming to Groebner basis not allowed for empty basis. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  int theNumVars=theBasis[0].GetNumVars();
  MonomialP leftShift, rightShift, monLCM;
  leftShift.monBody.SetSize(theNumVars);
  rightShift.monBody.SetSize(theNumVars);
  Polynomial<CoefficientType> leftBuf, rightBuf, buffer1;
  Polynomial<CoefficientType>& outputRemainder=rightBuf; //to save some RAM
  ProgressReport reportOuter(theGlobalVariables);
  ProgressReport reportInner(theGlobalVariables);
  while (indexPairs.size>0)
  { PairInts& lastPair=*indexPairs.LastObject();
    int currentPairIndex=indexPairs.size-1;
    bool isGood=false;
    Polynomial<Rational>& currentLeft = theBasis[lastPair.Object1];
    Polynomial<Rational>& currentRight= theBasis[lastPair.Object2];
    MonomialP& leftHighestMon= theLeadingMons[lastPair.Object1];
    MonomialP& rightHighestMon=theLeadingMons[lastPair.Object2];
    if (theGlobalVariables!=0)
    { std::stringstream out;
      out << "Minimal remaining cases: " << indexPairs.size
      << ", processing " << currentLeft.ToString(&theGlobalVariables->theDefaultFormat)
      << " and " << currentRight.ToString(&theGlobalVariables->theDefaultFormat);
      reportOuter.Report(out.str());
    }
    for (int k=0; k<theNumVars; k++)
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
      if (!RationalFunction::CriterionCLOsh(indexPairs, theLeadingMons, monLCM))
      { leftBuf=currentLeft;
        rightBuf=currentRight;
        leftBuf.MultiplyBy(leftShift, theLeadingCoeffs[lastPair.Object2]);
        rightBuf.MultiplyBy(rightShift, theLeadingCoeffs[lastPair.Object1]);
        leftBuf-=rightBuf;
//        std::cout << "<br>testing sopoly: "
//        << leftBuf.ToString(&theGlobalVariables->theDefaultFormat)
//        ;
        if (theGlobalVariables!=0)
        { std::stringstream out;
          out << "Dividing spoly: " << leftBuf.ToString(& theGlobalVariables->theDefaultFormat);
          reportInner.Report(out.str());
        }
        RationalFunctionOld::RemainderDivisionWithRespectToBasis
        (leftBuf, theBasis, outputRemainder, buffer1, monLCM, theMonOrder, theGlobalVariables)
        ;
        if (theGlobalVariables!=0)
        { std::stringstream out;
          out << "and the remainder is: "
          << outputRemainder.ToString(& theGlobalVariables->theDefaultFormat);
          reportInner.Report(out.str());
        }
//        std::cout << "<br>and sopoly divided by "
//        << theBasis.ToString(&theGlobalVariables->theDefaultFormat) << " is "
//        << outputRemainder.ToString(&theGlobalVariables->theDefaultFormat);
        if (!outputRemainder.IsEqualToZero())
        { outputRemainder.ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
          theBasis.AddOnTop(outputRemainder);
          int theIndexMaxMon=theBasis.LastObject()->GetIndexMaxMonomial(theMonOrder);
          theLeadingMons.AddOnTop((*theBasis.LastObject())[theIndexMaxMon]);
          theLeadingCoeffs.AddOnTop(theBasis.LastObject()->theCoeffs[theIndexMaxMon]);
          for (int i=0; i<theBasis.size-1; i++)
            indexPairs.AddOnTop(PairInts(i, theBasis.size-1));
        }
      } //else
        //std::cout << "<br>" << leftHighestMon.ToString(&theGlobalVariables->theDefaultFormat)
        //<< " and " << rightHighestMon.ToString(&theGlobalVariables->theDefaultFormat)
        //<< " failed the closh criterium";
    //else
      //std::cout << "<br>" << leftHighestMon.ToString(&theGlobalVariables->theDefaultFormat)
      //<< " and " << rightHighestMon.ToString(&theGlobalVariables->theDefaultFormat)
      //<< " failed the lcm criterium";
    indexPairs.PopIndexSwapWithLast(currentPairIndex);
  }
  RationalFunctionOld::GroebnerBasisMakeMinimal(theBasis, theMonOrder);
}

class ElementZmodP
{
public:
  LargeIntUnsigned theModulo;
  LargeIntUnsigned theValue;
  ElementZmodP(){}
  ElementZmodP(const ElementZmodP& other){this->operator=(other);}
  void CheckIamInitialized()
  { if (this->theModulo.IsEqualToZero())
    { std::cout << "This is a programming error: computing with non-initialized "
      << " element the ring Z mod p (the number p has not been initialized!)."
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
  }
  bool IsEqualToZero()const
  { return this->theValue.IsEqualToZero();
  }
  void operator=(const ElementZmodP& other)
  { this->theModulo=other.theModulo;
    this->theValue=other.theValue;
  }
  void operator=(const LargeIntUnsigned& other)
  { this->CheckIamInitialized();
    this->theValue=other;
    this->theValue%=this->theModulo;
  }
  void MakeMOne(const LargeIntUnsigned& newModulo)
  { this->theModulo=newModulo;
    this->theValue=newModulo;
    this->theValue--;
  }
  void CheckEqualModuli(const ElementZmodP& other)
  { if (!this->theModulo.IsEqualTo(other.theModulo))
    { std::cout << "This is a programming error: "
      << " attempting to make an operation with two elemetns of Z mod P with different moduli, "
      << this->theModulo.ToString() << " and " << other.theModulo.ToString()
      << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
  }
  void operator*=(const ElementZmodP& other)
  { if (this==&other)
    { ElementZmodP other=*this;
      *this*=other;
      return;
    }
    this->CheckEqualModuli(other);
    this->theValue*=other.theValue;
    this->theValue%=this->theModulo;
  }
  void operator+=(const ElementZmodP& other)
  { if (this==&other)
    { ElementZmodP other=*this;
      *this+=other;
      return;
    }
    this->CheckEqualModuli(other);
    this->theValue+=other.theValue;
    this->theValue%=this->theModulo;
  }
  void operator=(const LargeInt& other)
  { this->CheckIamInitialized();
    this->theValue=other.value;
    this->theValue%=this->theModulo;
    if (other.sign==-1)
    { ElementZmodP mOne;
      mOne.MakeMOne(this->theModulo);
      *this*=mOne;
    }
  }
  void operator=(const Rational& other)
  { this->CheckIamInitialized();
    *this= other.GetNumerator();
    ElementZmodP den;
    den.theModulo=this->theModulo;
    den=other.GetDenominator();
    *this/=den;
  }
  void operator/=(const ElementZmodP& den)
  { std::cout << "Not implemented yet!";
    assert(false);
  }
  void ScaleToIntegralMinHeightAndGetPoly
  (const Polynomial<Rational>& input, Polynomial<ElementZmodP>& output,
   const LargeIntUnsigned& newModulo)
  { Polynomial<Rational> rescaled;
    rescaled=input;
    rescaled.ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
    output.SetExpectedSize(input.size);
    ElementZmodP theCF;
    theCF.theModulo=newModulo;
    output.MakeZero(input.GetNumVars());
    for (int i=0; i<input.size; i++)
    { theCF=input.theCoeffs[i];
      output.AddMonomial(input[i], theCF);
    }
  }
};
#endif
