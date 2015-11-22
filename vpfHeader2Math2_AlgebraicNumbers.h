//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader1_3_h_already_included
#define vpfHeader1_3_h_already_included

#include "vpfHeader2Math0_General.h"
static ProjectInformationInstance ProjectInfoVpfHeader1_3(__FILE__, "Header, algebraic numbers. ");

class AlgebraicClosureRationals;
class AlgebraicNumber
{
  friend std::ostream& operator <<(std::ostream& output, const AlgebraicNumber& theNumber)
  { output << theNumber.ToString();
    return output;
  }
  friend AlgebraicNumber operator-(const AlgebraicNumber& argument)
  { argument.CheckConsistency();
    AlgebraicNumber result=argument;
    result.CheckConsistency();
    result*=-1;
//    stOutput << "here be unary minus!";
    result.CheckConsistency();
    return result;
  }
  public:
  AlgebraicClosureRationals* owner;
  int basisIndex;
  VectorSparse<Rational> theElT;
  bool flagDeallocated;
  AlgebraicNumber():owner(0), basisIndex(0), flagDeallocated(false) {}
  AlgebraicNumber(const Rational& other):owner(0), basisIndex(0), flagDeallocated(false)
  { this->operator=(other);
  }
  AlgebraicNumber(int other):owner(0), basisIndex(0), flagDeallocated(false)
  { this->operator=((Rational)other);
  }
  AlgebraicNumber(const AlgebraicNumber& other):owner(0), basisIndex(0), flagDeallocated(false)
  { this->operator=(other);
  }
  bool IsExpressedViaLatestBasis()const;
  void ExpressViaLatestBasis();
  bool AssignCosRationalTimesPi(const Rational& input, AlgebraicClosureRationals& inputOwner);
  bool AssignSinRationalTimesPi(const Rational& input, AlgebraicClosureRationals& inputOwner)
  { Rational rHalf(1,2);
    return this->AssignCosRationalTimesPi(rHalf-input, inputOwner);
  }

  bool NeedsParenthesisForMultiplication()const;
  bool CheckConsistency()const;
  bool CheckNonZeroOwner()const;
  bool CheckCommonOwner(const AlgebraicNumber& other)const;
  unsigned int HashFunction()const;
  static inline unsigned int HashFunction(const AlgebraicNumber& input)
  { return input.HashFunction();
  }
  inline LargeIntUnsigned GetNumerator()const
  { return this->theElT.FindGCDCoefficientNumeratorsOverRationals().GetNumerator().value;
  }
  inline LargeIntUnsigned GetDenominator()const
  { return this->GetDenominatorRationalPart().GetDenominator();
  }
  bool IsPositive()
  { Rational ratPart;
    if (this->IsRational(&ratPart))
      return ratPart.IsPositive();
    return false;
  }
  bool IsRational(Rational* whichRational=0)const;
  bool IsNegative()const
  { Rational theRationalValue;
    if (this->IsRational(&theRationalValue))
      return theRationalValue.IsNegative();
    return false;
  }
  bool IsEqualToZero()const;
  bool IsEqualToOne()const
  { return (*this==1);
  }
  void operator=(const AlgebraicNumber& other)
  { this->basisIndex=other.basisIndex;
    this->owner=other.owner;
    this->theElT=other.theElT;
  }
  void operator=(const Rational& other);
  void operator=(int other)
  { *this=(Rational) other;
  }
  bool ConstructFromMinPoly(const Polynomial<AlgebraicNumber>& thePoly, AlgebraicClosureRationals& inputOwner, GlobalVariables* theGlobalVariables);
  bool ConstructFromMinPoly(const Polynomial<Rational>& thePoly, AlgebraicClosureRationals& inputOwner, GlobalVariables* theGlobalVariables)
  { Polynomial<AlgebraicNumber> polyConverted;
    polyConverted=thePoly;
    return this->ConstructFromMinPoly(polyConverted, inputOwner, theGlobalVariables);
  }
  bool AssignRationalQuadraticRadical(const Rational& inpuT, AlgebraicClosureRationals& inputOwner);
  void AssignRational(const Rational& input, AlgebraicClosureRationals& inputOwner);
  Rational GetDenominatorRationalPart()const;
  Rational GetNumeratorRationalPart()const;
  void SqrtMeDefault();
  bool IsSmallInteger(int* whichInteger)const
  { Rational theRat;
    if (!this->IsRational(&theRat))
      return false;
    return theRat.IsSmallInteger(whichInteger);
  }
  void RadicalMeDefault(int theRad);
  void Invert();
  bool EvaluatesToDouble(double* outputWhichDouble)const;
  void operator/=(const AlgebraicNumber& other);
  bool operator==(const AlgebraicNumber& other)const;
  bool operator==(const Rational& other)const;
  bool operator==(int other)const
  { return *this==(Rational)other;
  }

  inline bool operator!=(const AlgebraicNumber& other)const
  { return !(*this==other);
  }
  inline bool operator!=(int other)const
  { return !(*this==other);
  }
  void Minus()
  { this->theElT*=-1;
  }
  void operator=(const Polynomial<AlgebraicNumber>& other);
  AlgebraicNumber operator+(const AlgebraicNumber& other)const
  { this->CheckConsistency();
    AlgebraicNumber result=*this;
    result.CheckConsistency();
    other.CheckConsistency();
    result+=other;
    result.CheckConsistency();
    return result;
  }
  AlgebraicNumber operator-(const AlgebraicNumber& other)const
  { AlgebraicNumber result=*this;
    result-=other;
    return result;
  }
  AlgebraicNumber operator*(const AlgebraicNumber& other)const
  { AlgebraicNumber result=*this;
    result*=other;
    return result;
  }
  AlgebraicNumber operator/(const AlgebraicNumber& other)const
  { this->CheckConsistency();
    other.CheckConsistency();
    AlgebraicNumber result=*this;

    result/=other;
    return result;
  }
  void operator+=(const AlgebraicNumber& other);
  void operator-=(const AlgebraicNumber& other);
  void operator*=(const AlgebraicNumber& other);
  void operator*=(const Rational& other);
  void operator*=(LargeInt other)
  { this->operator*=((Rational)other);
  }
  void operator*=(int other)
  { this->operator*=((Rational)other);
  }
  bool operator>(const AlgebraicNumber& other)const;
  std::string ToString(FormatExpressions* theFormat=0)const;
};

class AlgebraicClosureRationals
{
public:
  List<MatrixTensor<Rational> > theBasisMultiplicative;
  List<List<VectorSparse<Rational> > > theBasesAdditive;

  MatrixTensor<Rational> GeneratingElementTensorForm;
  Matrix<Rational> GeneratingElementMatForm;
  AlgebraicNumber GeneratingElemenT;
  Vectors<Rational> theGeneratingElementPowersBasis;

  bool flagIsQuadraticRadicalExtensionRationals;
  HashedList<LargeInt> theQuadraticRadicals;

  GlobalVariables* theGlobalVariables;

  List<std::string> DisplayNamesBasisElements;
  void AddNewBasis();

  void RegisterNewBasis(const MatrixTensor<Rational>& theInjection);
  void reset();
  bool CheckConsistency()const;
  AlgebraicClosureRationals():theGlobalVariables(0)
  { this->reset();
  }
  bool MergeRadicals(const List<LargeInt>& theRadicals);
  void ChooseGeneratingElement();
  bool ReduceMe();
  void ComputeDisplayStringsFromRadicals();
  bool GetRadicalSelectionFromIndex(int inputIndex, Selection& theSel);

  static int GetIndexFromRadicalSelection(const Selection& theSel);
  void GetMultiplicativeOperatorFromRadicalSelection(const Selection& theSel, MatrixTensor<Rational>& outputOp);
  void GetMultiplicationBy(const AlgebraicNumber& input, MatrixTensor<Rational>& output);
  void GetAdditionTo(const AlgebraicNumber& input, VectorSparse<Rational>& output);
  void ConvertPolyDependingOneVariableToPolyDependingOnFirstVariableNoFail
  (const Polynomial<AlgebraicNumber>& input, Polynomial<AlgebraicNumber>& output);
  bool AdjoinRootMinPoly(const Polynomial<AlgebraicNumber>& thePoly, AlgebraicNumber& outputRoot, GlobalVariables* theGlobalVariables);
  bool AdjoinRootQuadraticPolyToQuadraticRadicalExtension(const Polynomial<AlgebraicNumber>& thePoly, AlgebraicNumber& outputRoot, GlobalVariables* theGlobalVariables);
  std::string ToString(FormatExpressions* theFormat=0)const;
  bool splitToPartialFractionsOverRealAlgebraicNumbers
  (RationalFunctionOld& inputRF, List<Polynomial<AlgebraicNumber> >& outputNumerators, List<Polynomial<AlgebraicNumber> >& outputDenominators,
   std::stringstream* reportStream=0);
};

class ElementZmodP
{
public:
  LargeIntUnsigned theModulo;
  LargeIntUnsigned theValue;
  bool flagDeallocated;
  unsigned int HashFunction()const
  { if (this->theValue.IsEqualToZero())
      return 0;
    return this->theValue.HashFunction()*SomeRandomPrimes[0]+this->theModulo.HashFunction()*SomeRandomPrimes[1];
  }
  static unsigned int HashFunction(const ElementZmodP& input)
  { return input.HashFunction();
  }
  ElementZmodP(){this->flagDeallocated=false;}
  ElementZmodP(const ElementZmodP& other){this->flagDeallocated=false; this->operator=(other);}
  ~ElementZmodP(){this->flagDeallocated=true;}
  void CheckIamInitialized()const
  { if (this->theModulo.IsEqualToZero())
      crash << "This is a programming error: computing with non-initialized element the ring Z mod p (the number p has not been initialized!)." << crash;
    if (this->flagDeallocated)
      crash << "This is a programming error: use after free of element z mod p. " << crash;
  }
  std::string ToString(FormatExpressions* theFormat=0)const;
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
  { if (this->theModulo!=other.theModulo)
      crash << "This is a programming error: attempting to make an operation with two elemetns of Z mod P with different moduli, "
      << this->theModulo.ToString() << " and " << other.theModulo.ToString() << ". " << crash;
  }
  bool operator==(int other)const;
  bool operator==(const ElementZmodP& other)const;
  void operator*=(const ElementZmodP& other);
  void operator*=(const LargeInt& other)
  { this->theValue*=other.value;
    if (other.IsNegative())
    { this->theValue*=this->theModulo-1;
      this->theValue%=this->theModulo;
    }
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
  bool AssignRational(const Rational& other);
  void operator=(const Rational& other)
  { bool tempB=this->AssignRational(other);
    if (!tempB)
      crash << "This is a programming error: using ElementZmodP::operator= to assign a Rational number failed. "
      << " Operator= does not allow failure. " << crash;
  }
  bool operator/=(const ElementZmodP& den);
  void ScaleToIntegralMinHeightAndGetPoly(const Polynomial<Rational>& input, Polynomial<ElementZmodP>& output, const LargeIntUnsigned& newModulo)
  { Polynomial<Rational> rescaled;
    rescaled=input;
    rescaled.ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
    output.SetExpectedSize(input.size());
    ElementZmodP theCF;
    theCF.theModulo=newModulo;
    output.MakeZero();
    for (int i=0; i<input.size(); i++)
    { theCF=input.theCoeffs[i];
      output.AddMonomial(input[i], theCF);
    }
  }
};
#endif
