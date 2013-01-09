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
    theOperation.MakeZero();
    MonomialP tempM;
    tempM.MakeOne(2);
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
    theOperation.MakeZero();
    MonomialP tempM;
    tempM.MakeOne(2);
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
    output.MakeZero();
    for (int i=0; i<input.size; i++)
    { theCF=input.theCoeffs[i];
      output.AddMonomial(input[i], theCF);
    }
  }
};
#endif
