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
  void DiffereintialMe()
  {
  }
  unsigned int HashFunction()const
  { return this->HashFunction(*this);
  }
  static unsigned int HashFunction(const DifferentialForm<CoefficientType>& input)
  { return ElementAssociativeAlgebra<MonomialDiffForm, CoefficientType>::HashFunction(input);
  }
};

class RationalAlgebraic
{
public:
  //The mathematical definition of an algebraic integer
  //is an algebraic number whose mathematical minimal polynomial has leading coefficient 1.
  //Format of minPoly.
  //0. If the algebraic integer is in fact an honest integer
  //"minPoly" is defined to be the empty polynomial. This is different from the mathematical
  //minimal polynomial.
  //1. If the number is an algebraic integer but not an integer, minPoly is defined
  // to be the minimal polynomial of the number with the following assumptions:
  //1.1. Minimal polynomials have relatively prime coefficients.
  //1.2. The leading coefficient of the minimal polynomial is 1.
  int rootIndex;
  Polynomial<Rational> minPoly;
  std::string DisplayString;
  friend std::ostream& operator << (std::ostream& output, const RationalAlgebraic& theRat)
  { output << theRat.ToString();
    return output;
  }

  void SqrtMe();
  void RadicalMe(int theRad);
  unsigned int HashFunction()const
  { return this->HashFunction(*this);
  }
  static unsigned int HashFunction(const RationalAlgebraic& input)
  { return input.minPoly.HashFunction()*SomeRandomPrimes[0]+input.rootIndex*SomeRandomPrimes[1];
  }

  RationalAlgebraic(){};
  RationalAlgebraic(const RationalAlgebraic& other)
  { this->operator=(other);
  }
  void operator=(const RationalAlgebraic& other)
  { this->minPoly=other.minPoly;
    this->rootIndex=other.rootIndex;
  }
  void MakeOneVarPolyIntoTwoVarPoly(Polynomial<Rational>& thePoly)
  { Polynomial<Rational> output;
    MonomialP tempM;
    tempM.MakeConst(2);
    output.MakeZero(2);
    for (int i =0; i<thePoly.size; i++)
    { tempM[1]=thePoly[i][0];
      output.AddMonomial(tempM, thePoly.theCoeffs[i]);
    }
    thePoly=output;
  }
  void operator=(const Rational& other);
  void AssignOperation
  (Polynomial<Rational>& theOperationIsModified, const RationalAlgebraic& other)
  ;
  void ReduceModAnBm
(Polynomial<Rational>& toBeReduced, const Polynomial<Rational>& An,
 const Polynomial<Rational>& Bm, int theN, int theM, Polynomial<Rational>& buffer
 )const
  ;
  void operator+=(const RationalAlgebraic& other)
  { MacroRegisterFunctionWithName("RationalAlgebraic::operator+=");
    Polynomial<Rational> theOperation;
    theOperation.MakeZero(2);
    MonomialP tempM;
    tempM.MakeConst(2);
    tempM[0]=1;
    theOperation.AddMonomial(tempM, 1);
    tempM[0]=0;
    tempM[1]=1;
    theOperation.AddMonomial(tempM, 1);
    this->AssignOperation(theOperation, other);
  }
  std::string ToString(FormatExpressions* theFormat=0)const;
  void AssignRadical(const LargeInt& undertheRadical, int theRadical)
  { MonomialP tempM;
    tempM.monBody.SetSize(1);
    tempM.monBody[0]=theRadical;
    this->minPoly.MakeZero(1);
    this->minPoly.AddMonomial(tempM, 1);
    tempM.monBody[0]=0;
    this->minPoly.AddMonomial(tempM, undertheRadical);
    this->rootIndex=0;
    std::stringstream out;
    out << "\\sqrt{{}" << undertheRadical.ToString() << "}";
    this->DisplayString= out.str();
  }
  bool IsAConstant()
  { return this->minPoly.IsEqualToZero();
  }
  bool operator==(const RationalAlgebraic& other)const
  { return this->rootIndex==other.rootIndex && this->minPoly==other.minPoly;
  }
  inline bool IsEqualToZero()const
  { return false;
  }
  bool operator> (const RationalAlgebraic& other) const
  { if (this->minPoly>other.minPoly)
      return true;
    if (other.minPoly>this->minPoly)
      return false;
    return this->rootIndex>other.rootIndex;
  }
};

#endif
