//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader1_3_h_already_included
#define vpfHeader1_3_h_already_included

#include "vpfHeader2Math0_General.h"
static ProjectInformationInstance ProjectInfoVpfHeader1_3(__FILE__, "Header, math routines. ");

class AlgebraicExtensionRationals;
class AlgebraicClosureRationals;
class AlgebraicNumber
{
  friend std::ostream& operator <<(std::ostream& output, const AlgebraicNumber& theNumber)
  { output << theNumber.ToString();
    return output;
  }
  public:
  AlgebraicExtensionRationals* owner;
  VectorSparse<Rational> theElt;
  AlgebraicNumber():owner(0){}
  AlgebraicNumber(const Rational& other):owner(0)
  { this->operator=(other);
  }
  AlgebraicNumber(int other):owner(0)
  { this->operator=((Rational)other);
  }
  bool NeedsBrackets()const;
  bool CheckNonZeroOwner()const;
  unsigned int HashFunction()const;
  static inline unsigned int HashFunction(const AlgebraicNumber& input)
  { return input.HashFunction();
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
  void GetMultiplicationByMeMatrix(MatrixTensor<Rational>& output);
  void operator=(const Rational& other);
  void operator=(int other)
  { *this=(Rational) other;
  }
  bool AssignRationalQuadraticRadical(const Rational& input, AlgebraicClosureRationals& inputOwner);
  void AssignRational(const Rational& input, AlgebraicClosureRationals& inputOwner);
  Rational GetDenominatorRationalPart()const;
  Rational GetNumeratorRationalPart()const;
  void SqrtMeDefault();
  static void ConvertToCommonOwner(AlgebraicNumber& left, AlgebraicNumber& right);
  void RadicalMeDefault(int theRad);
  void Invert();
  void operator/=(const AlgebraicNumber& other);
  bool operator==(const AlgebraicNumber& other)const;
  bool operator==(int other)const;
  inline bool operator!=(const AlgebraicNumber& other)const
  { return !(*this==other);
  }
  inline bool operator!=(int other)const
  { return !(*this==other);
  }
  void operator+=(const AlgebraicNumber& other);
  void operator-=(const AlgebraicNumber& other);
  void operator*=(const AlgebraicNumber& other);
  void operator*=(const Rational& other);
  void operator*=(int other)
  { this->operator*=((Rational)other);
  }
  std::string ToString(FormatExpressions* theFormat=0)const;
};

class AlgebraicExtensionRationals
{
  public:
  List<MatrixTensor<Rational> > AlgebraicBasisElements;
  MatrixTensor<Rational> GeneratingElementTensorForm;
  Matrix<Rational> GeneratingElementMatForm;
  AlgebraicNumber GeneratingElemenT;
  Vectors<Rational> theGeneratingElementPowersBasis;

  AlgebraicClosureRationals* owner;
  bool flagIsQuadraticRadicalExtensionRationals;
  HashedList<LargeInt> theQuadraticRadicals;
//  Matrix<Rational> injectionFromLeftParent;
//  Matrix<Rational> injectionFromRightParent;
//  Matrix<Rational> injectionFromLeftParentToMeMatForm;
//  MatrixTensor<Rational> injectionFromLeftParentToMeTensorForm;
//  Matrix<Rational> injectionFromRightParentToMeMatForm;
//  MatrixTensor<Rational> injectionFromRightParentToMeTensorForm;
//  AlgebraicExtensionRationals* leftParent;
//  AlgebraicExtensionRationals* rightParent;
//  AlgebraicExtensionRationals* heir;
  List<std::string> DisplayNamesBasisElements;
  int indexInOwner;
  int DimOverRationals;
  AlgebraicExtensionRationals(): owner(0), flagIsQuadraticRadicalExtensionRationals(false), indexInOwner(-1), DimOverRationals(0)//, leftParent(0), rightParent(0), heir(0), indexInOwner(-1), DimOverRationals(-1)
  {}
  bool CheckNonZeroOwner()const;
  bool CheckBasicConsistency()const;
  void MakeRationals(AlgebraicClosureRationals& inputOwners);
  inline unsigned int HashFunction()const
  { return this->AlgebraicBasisElements.HashFunction();
  }
  void ChooseGeneratingElement();
  void ComputeDisplayStringsFromRadicals();
  int GetIndexFromRadicalSelection(const Selection& theSel);
  void GetMultiplicativeOperatorFromRadicalSelection(const Selection& theSel, MatrixTensor<Rational>& outputOp);
  bool operator==(const AlgebraicExtensionRationals& input)const;
  static inline unsigned int HashFunction(const AlgebraicExtensionRationals& input)
  { return input.HashFunction();
  }
  void ReduceMeOnCreation(MatrixTensor<Rational>* injectionFromLeftParent=0, MatrixTensor<Rational>* injectionFromRightParent=0);
  std::string ToString(FormatExpressions* theFormat=0);
};

class AlgebraicClosureRationals
{
public:
  ListReferences<AlgebraicExtensionRationals> theAlgebraicExtensions;
  HashedList<Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity> > theAlgebraicExtensionPairs;
  List<int> thePairPairing;
  List<MatrixTensor<Rational> > injectionsLeftParenT;
  List<MatrixTensor<Rational> > injectionsRightParenT;
  GlobalVariables* theGlobalVariables;

  bool CheckConsistency()const;
  AlgebraicClosureRationals():theGlobalVariables(0){}
  int GetIndexIMustContainPair(const AlgebraicExtensionRationals* left, const AlgebraicExtensionRationals* right);
  void GetLeftAndRightInjections
  (const AlgebraicExtensionRationals* left, const AlgebraicExtensionRationals* right, Matrix<Rational>*& outputInjectionFromLeft, Matrix<Rational>*& outputInjectionFromRight);
  void GetLeftAndRightInjectionsTensorForm
  (const AlgebraicExtensionRationals* left, const AlgebraicExtensionRationals* right, MatrixTensor<Rational>*& outputInjectionFromLeft,
   MatrixTensor<Rational>*& outputInjectionFromRight);
  void AddPairWithInjection
  (const AlgebraicExtensionRationals& left, const AlgebraicExtensionRationals& right, const AlgebraicExtensionRationals& tensorProd, MatrixTensor<Rational>& inputInjectionFromLeft,
   MatrixTensor<Rational>& inputInjectionFromRight);
  void MergeTwoExtensions
  (AlgebraicExtensionRationals& left, AlgebraicExtensionRationals& right, AlgebraicExtensionRationals& output,
   MatrixTensor<Rational>* injectionFromLeftParent=0, MatrixTensor<Rational>* injectionFromRightParent=0);
  void MergeTwoQuadraticRadicalExtensions
  (AlgebraicExtensionRationals& left, AlgebraicExtensionRationals& right, AlgebraicExtensionRationals& output,
   MatrixTensor<Rational>* injectionFromLeftParent=0, MatrixTensor<Rational>* injectionFromRightParent=0);
  AlgebraicExtensionRationals* MergeTwoExtensionsAddOutputToMe(AlgebraicExtensionRationals& left, AlgebraicExtensionRationals& right);
  std::string ToString(FormatExpressions* theFormat=0)const;
  void AddMustBeNew(AlgebraicExtensionRationals& input);
};

class ElementZmodP
{
public:
  LargeIntUnsigned theModulo;
  LargeIntUnsigned theValue;
  unsigned int HashFunction()const
  { if (this->theValue.IsEqualToZero())
      return 0;
    return this->theValue.HashFunction()*SomeRandomPrimes[0]+this->theModulo.HashFunction()*SomeRandomPrimes[1];
  }
  static unsigned int HashFunction(const ElementZmodP& input)
  { return input.HashFunction();
  }
  ElementZmodP(){}
  ElementZmodP(const ElementZmodP& other){this->operator=(other);}
  void CheckIamInitialized()
  { if (this->theModulo.IsEqualToZero())
    { std::cout << "This is a programming error: computing with non-initialized element the ring Z mod p (the number p has not been initialized!)."
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
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
    { std::cout << "This is a programming error: attempting to make an operation with two elemetns of Z mod P with different moduli, "
      << this->theModulo.ToString() << " and " << other.theModulo.ToString() << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
  }
  bool operator==(int other)const
  { ElementZmodP tempElt;
    tempElt.theModulo=this->theModulo;
    tempElt=(LargeInt) other;
    return *this==other;
  }
  bool operator==(const ElementZmodP& other)const
  { return this->theModulo==other.theModulo && this->theValue==other.theValue;
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
    { std::cout << "This is a programming error: using ElementZmodP::operator= to assign a Rational number failed. "
      << " Operator= does not allow failure. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
  }
  void operator/=(const ElementZmodP& den);
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
