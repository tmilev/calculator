//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader1_3_h_already_included
#define vpfHeader1_3_h_already_included

#include "vpfHeader1_2.h"
static ProjectInformationInstance ProjectInfoVpfHeader1_3
(__FILE__, "Header, math routines. ");

class AlgebraicExtensionRationals;
class AlgebraicClosureRationals;
class AlgebraicNumber
{
  public:
  AlgebraicExtensionRationals* owner;
  VectorSparse<Rational> theElt;
  AlgebraicNumber():owner(0){}
  bool CheckNonZeroOwner()const;
  unsigned int HashFunction()const;
  static inline unsigned int HashFunction(const AlgebraicNumber& input)
  { return input.HashFunction();
  }
  bool IsEqualToZero()const;
  void GetMultiplicationByMeMatrix(MatrixTensor<Rational>& output);
  void operator=(const Rational& other);
  bool AssignRationalQuadraticRadical(const Rational& input, AlgebraicClosureRationals& inputOwner);
  void AssignRational(const Rational& input, AlgebraicClosureRationals& inputOwner);

  void SqrtMeDefault();
  static void ConvertToCommonOwner(AlgebraicNumber& left, AlgebraicNumber& right);
  void RadicalMeDefault(int theRad);
  void Invert();
  void operator/=(const AlgebraicNumber& other);
  bool operator==(const AlgebraicNumber& other)const;
  void operator+=(const AlgebraicNumber& other);
  void operator*=(const AlgebraicNumber& other);
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
  HashedList<Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity> > thePairs;
  List<int> thePairPairing;
  List<MatrixTensor<Rational> > injectionsLeftParenT;
  List<MatrixTensor<Rational> > injectionsRightParenT;
  GlobalVariables* theGlobalVariables;

  bool CheckConsistency()const;
  AlgebraicClosureRationals():theGlobalVariables(0){}
  int GetIndexIMustContainPair
  (const AlgebraicExtensionRationals* left, const AlgebraicExtensionRationals* right)
  ;
  void GetLeftAndRightInjections
  (const AlgebraicExtensionRationals* left, const AlgebraicExtensionRationals* right,
   Matrix<Rational>*& outputInjectionFromLeft, Matrix<Rational>*& outputInjectionFromRight)
   ;
  void GetLeftAndRightInjectionsTensorForm
  (const AlgebraicExtensionRationals* left, const AlgebraicExtensionRationals* right,
   MatrixTensor<Rational>*& outputInjectionFromLeft, MatrixTensor<Rational>*& outputInjectionFromRight)
   ;
  void AddPairWithInjection
  (const AlgebraicExtensionRationals& left, const AlgebraicExtensionRationals& right,
   const AlgebraicExtensionRationals& tensorProd, MatrixTensor<Rational>& inputInjectionFromLeft,
   MatrixTensor<Rational>& inputInjectionFromRight)
  ;

  void MergeTwoExtensions
  (AlgebraicExtensionRationals& left, AlgebraicExtensionRationals& right, AlgebraicExtensionRationals& output,
   MatrixTensor<Rational>* injectionFromLeftParent=0, MatrixTensor<Rational>* injectionFromRightParent=0)
  ;
  void MergeTwoQuadraticRadicalExtensions
  (AlgebraicExtensionRationals& left, AlgebraicExtensionRationals& right, AlgebraicExtensionRationals& output,
   MatrixTensor<Rational>* injectionFromLeftParent=0, MatrixTensor<Rational>* injectionFromRightParent=0)
  ;
  AlgebraicExtensionRationals* MergeTwoExtensionsAddOutputToMe
  (AlgebraicExtensionRationals& left, AlgebraicExtensionRationals& right)
  ;
  std::string ToString(FormatExpressions* theFormat=0)const;
  void AddMustBeNew(AlgebraicExtensionRationals& input);
};

class AlgebraicNumberRegistryOld;

class AlgebraicNumberOld
{
  friend class List<AlgebraicNumberOld>;
  friend class Matrix<AlgebraicNumberOld>;
  AlgebraicNumberOld():rootIndex(-1),minPolyIndex(-1), theRegistry(0){}

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
  AlgebraicNumberRegistryOld* theRegistry;
  friend std::ostream& operator << (std::ostream& output, const AlgebraicNumberOld& theRat)
  { output << theRat.ToString();
    return output;
  }
  unsigned int HashFunction()const
  { return this->HashFunction(*this);
  }
  static unsigned int HashFunction(const AlgebraicNumberOld& input)
  { return input.minPolyIndex*SomeRandomPrimes[0]+input.rootIndex*SomeRandomPrimes[1];
  }
  AlgebraicNumberOld(AlgebraicNumberRegistryOld& owner) :rootIndex(-1), minPolyIndex(-1)
  { this->theRegistry=&owner;
  }
  AlgebraicNumberOld(const AlgebraicNumberOld& other)
  { this->operator=(other);
  }
  void operator=(const AlgebraicNumberOld& other)
  { this->minPolyIndex=other.minPolyIndex;
    this->rootIndex=other.rootIndex;
    this->theRegistry=other.theRegistry;
  }
  void operator=(const Rational& other);
  bool AssignOperation
  (Polynomial<Rational>& theOperationIsModified, const List<AlgebraicNumberOld>& theOperationArguments)
  ;
  void ReduceMod
(Polynomial<Rational>& toBeReduced, const List<Polynomial<Rational> >& thePolys,
 List<int>& theNs, Polynomial<Rational>& buffer
 )const
  ;
  bool operator+=(const AlgebraicNumberOld& other)
  { MacroRegisterFunctionWithName("AlgebraicNumberOld::operator+=");
    Polynomial<Rational> theOperation;
    theOperation.MakeZero();
    MonomialP tempM;
    tempM.MakeOne(2);
    tempM[0]=1;
    theOperation.AddMonomial(tempM, 1);
    tempM[0]=0;
    tempM[1]=1;
    theOperation.AddMonomial(tempM, 1);
    List<AlgebraicNumberOld> tempList;
    tempList.SetSize(2);
    tempList[0]=*this;
    tempList[1]=other;
    return this->AssignOperation(theOperation, tempList);
  }
  bool operator*=(const AlgebraicNumberOld& other)
  { MacroRegisterFunctionWithName("AlgebraicNumberOld::operator+=");
    Polynomial<Rational> theOperation;
    theOperation.MakeZero();
    MonomialP tempM;
    tempM.MakeOne(2);
    tempM[0]=1;
    tempM[1]=1;
    theOperation.AddMonomial(tempM, 1);
    List<AlgebraicNumberOld> tempList;
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
  bool operator==(const AlgebraicNumberOld& other)const
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
  bool operator> (const AlgebraicNumberOld& other) const
  { if (this->GetMinPoly()>other.GetMinPoly())
      return true;
    if (other.GetMinPoly()>this->GetMinPoly())
      return false;
    return this->rootIndex>other.rootIndex;
  }
};

class AlgebraicNumberOriginOld
{
  public:
  List<List<AlgebraicNumberOld> > theParents;
  List<Polynomial<Rational> > theOriginOperation;
};

class AlgebraicNumberRegistryOld
{
  public:
  HashedList<Polynomial<Rational> > theMinPolys;
  List<AlgebraicNumberOriginOld> theOrigins;
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
    { std::cout << "This is a programming error: computing with non-initialized "
      << " element the ring Z mod p (the number p has not been initialized!)."
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
  }
  std::string ToString(FormatExpressions* theFormat=0)const
  ;
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
    { std::cout << "This is a programming error: "
      << " attempting to make an operation with two elemetns of Z mod P with different moduli, "
      << this->theModulo.ToString() << " and " << other.theModulo.ToString()
      << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
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
  void ScaleToIntegralMinHeightAndGetPoly
  (const Polynomial<Rational>& input, Polynomial<ElementZmodP>& output,
   const LargeIntUnsigned& newModulo)
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
