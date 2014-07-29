//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader1_HeaderUniversalEnveloping
#define vpfHeader1_HeaderUniversalEnveloping
#include "vpfHeader2Math0_General.h"
#include "vpfHeader2Math1_LieTheoryExtras.h"

static ProjectInformationInstance ProjectInfovpfHeader1_HeaderUniversalEnveloping(__FILE__, "Header, universal enveloping algebras. ");
template<class coefficient>
class ElementUniversalEnvelopingOrdered;

template<class coefficient>
class MonomialUniversalEnvelopingOrdered
{
  void SimplifyAccumulateInOutputNoOutputInit(ElementUniversalEnvelopingOrdered<coefficient>& output, GlobalVariables* theContext, const coefficient& theRingUnit=1, const coefficient& theRingZero=0);
  MonomialUniversalEnvelopingOrdered(const MonomialUniversalEnvelopingOrdered& other);
public:
  SemisimpleLieAlgebraOrdered* owner;
  std::string DebugString;
  std::string ToString(bool useLatex, bool useCalculatorFormat, FormatExpressions* PolyFormatLocal, GlobalVariables& theGlobalVariables)const;
  void ComputeDebugString()
  { GlobalVariables theGlobalVariables;
    FormatExpressions PolyFormatLocal;
    this->DebugString=this->ToString(false, true, &PolyFormatLocal, theGlobalVariables);
  }
  // SelectedIndices gives the non-zero powers of the generators participating in the monomial
  // Powers gives the powers of the generators in the order specified in the owner
  List<int> generatorsIndices;
  List<coefficient> Powers;
  coefficient Coefficient;
  static bool flagAnErrorHasOccurredTimeToPanic;
  void MultiplyBy(const MonomialUniversalEnveloping<coefficient>& other, ElementUniversalEnvelopingOrdered<coefficient>& output);
  void MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, const coefficient& thePower);
  void MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, int thePower);
  void MultiplyByNoSimplify(const MonomialUniversalEnvelopingOrdered& other);
  void MakeZero(int numVars, SemisimpleLieAlgebraOrdered& theOwner, GlobalVariables* theContext);
  void MakeZero(const coefficient& theRingZero, SemisimpleLieAlgebraOrdered& theOwner);
  bool ModOutFDRelationsExperimental(GlobalVariables* theContext, const Vector<Rational> & theHWsimpleCoords, const coefficient& theRingUnit=1, const coefficient& theRingZero=0);
  void ModOutVermaRelations(GlobalVariables* theContext, const List<coefficient>* subHiGoesToIthElement=0, const coefficient& theRingUnit=1, const coefficient& theRingZero=0);
  void SetNumVariables(int newNumVars);
  unsigned int HashFunction() const;
  static inline unsigned int HashFunction(const MonomialUniversalEnvelopingOrdered<coefficient>& input)
  { return input.HashFunction();
  }
  void GetDegree(Polynomial<Rational> & output)
  { output.MakeZero(this->Coefficient.NumVars);
    for (int i=0; i<this->generatorsIndices.size; i++)
      output+=(this->Powers.TheObjects[i]);
  }
  bool GetElementUniversalEnveloping(ElementUniversalEnveloping<coefficient>& output, SemisimpleLieAlgebra& owner);
  bool IsEqualToZero()const{return this->Coefficient.IsEqualToZero();}
  bool CommutingLeftIndexAroundRightIndexAllowed(coefficient& theLeftPower, int leftGeneratorIndex, coefficient& theRightPower, int rightGeneratorIndex);
  bool CommutingRightIndexAroundLeftIndexAllowed(coefficient& theLeftPower, int leftGeneratorIndex, coefficient& theRightPower, int rightGeneratorIndex);
  bool SwitchConsecutiveIndicesIfTheyCommute(int theLeftIndex, MonomialUniversalEnvelopingOrdered<coefficient>& output, GlobalVariables* theContext, const coefficient& theRingZero=0);
  void MakeConst(const coefficient& theConst, SemisimpleLieAlgebraOrdered& theOwner){this->generatorsIndices.size=0; this->Powers.size=0; this->Coefficient=theConst; this->owner=&theOwner;}
  void Simplify(ElementUniversalEnvelopingOrdered<coefficient>& output, GlobalVariables* theContext, const coefficient& theRingUnit=1, const coefficient& theRingZero=0);
  void CommuteConsecutiveIndicesLeftIndexAroundRight
  (int theIndeX, ElementUniversalEnvelopingOrdered<coefficient>& output, GlobalVariables* theContext, const coefficient& theRingUnit=1, const coefficient& theRingZero=0);
  void CommuteConsecutiveIndicesRightIndexAroundLeft
  (int theIndeX, ElementUniversalEnvelopingOrdered<coefficient>& output, GlobalVariables* theContext, const coefficient& theRingUnit=1, const coefficient& theRingZero=0);
  MonomialUniversalEnvelopingOrdered()
  { this->owner=0;
  }
  void SubstitutionCoefficients(PolynomialSubstitution<Rational>& theSub);
  bool operator==(const MonomialUniversalEnvelopingOrdered& other)const
  { if(this->owner!=other.owner)
      crash << crash;
    return this->Powers==other.Powers && this->generatorsIndices==other.generatorsIndices;
  }
  void operator*=(const MonomialUniversalEnvelopingOrdered& other);
  template<class OtherCoefficientType>
  void AssignChangeCoefficientType(const MonomialUniversalEnvelopingOrdered<OtherCoefficientType>& other)
  { this->Coefficient=other.Coefficient;
    this->generatorsIndices=other.generatorsIndices.size;
    this->Powers.SetSize(other.Powers.size);
    for(int i=0; i<this->Powers.size; i++)
      this->Powers[i]=other.Powers[i];
  }
  inline void operator=(const MonomialUniversalEnvelopingOrdered& other)
  { this->generatorsIndices=(other.generatorsIndices);
    this->Powers=(other.Powers);
    this->Coefficient=(other.Coefficient);
    this->owner=other.owner;
  }
};

template <class coefficient>
bool MonomialUniversalEnvelopingOrdered<coefficient>::flagAnErrorHasOccurredTimeToPanic=false;

template <class coefficient>
class ElementUniversalEnvelopingOrdered : public HashedList<MonomialUniversalEnvelopingOrdered<coefficient> >
{
private:
  void AddMonomialNoCleanUpZeroCoeff(const MonomialUniversalEnvelopingOrdered<coefficient>& input);
  void CleanUpZeroCoeff();
  friend class MonomialUniversalEnvelopingOrdered<coefficient>;
public:
  std::string DebugString;
  GlobalVariables* context;
  void ToString(std::string& output, bool useLatex, bool useCalculatorFormat, FormatExpressions& PolyFormatLocal, GlobalVariables& theGlobalVariables)const;
  std::string ToString(bool useLatex, bool useCalculatorFormat, FormatExpressions& PolyFormatLocal, GlobalVariables& theGlobalVariables)const
  { std::string tempS;
    this->ToString(tempS, useLatex, useCalculatorFormat, PolyFormatLocal, theGlobalVariables);
    return tempS;
  }
  std::string ToString(FormatExpressions& PolyFormatLocal, GlobalVariables& theGlobalVariables)const
  { std::string tempS;
    this->ToString(tempS, true, true, PolyFormatLocal, theGlobalVariables);
    return tempS;
  }
  std::string ToString(FormatExpressions& PolyFormatLocal)const
  { GlobalVariables theGlobalVariables;
    return this->ToString(PolyFormatLocal, theGlobalVariables);
  }
  bool NeedsParenthesisForMultiplication()const
  { return this->size>1;
  }
  void ComputeDebugString()
  { FormatExpressions tempFormat;
    GlobalVariables theGlobalVariables;
    this->DebugString=this->ToString(tempFormat, theGlobalVariables);
  }
  SemisimpleLieAlgebraOrdered* owner;
  void AddMonomial(const MonomialUniversalEnvelopingOrdered<coefficient>& input);
  void MakeHgenerator(const Vector<Rational> & input, int numVars, SemisimpleLieAlgebraOrdered& theOwner);
  void AssignElementLieAlgebra(const ElementSemisimpleLieAlgebra<Rational>& input, const coefficient& theRingUnit, const coefficient& theRingZero, SemisimpleLieAlgebraOrdered& theOwner);
  void MakeOneGenerator
  (int theIndex, const coefficient& theCoeff, SemisimpleLieAlgebraOrdered& owner,
   GlobalVariables* theContext);
//  void MakeOneGeneratorCoeffOne(Vector<Rational> & rootSpace, int numVars, SemisimpleLieAlgebraOrdered& theOwner){this->MakeOneGeneratorCoeffOne(theOwner.GetGeneratorFromRoot(rootSpace), numVars, theOwner);};
  void MakeZero(SemisimpleLieAlgebraOrdered& theOwner);
  bool AssignElementUniversalEnveloping
  (ElementUniversalEnveloping<coefficient>& input, SemisimpleLieAlgebraOrdered& owner, const coefficient& theRingUnit, const coefficient& theRingZero, GlobalVariables* theContext);
  bool AssignMonomialUniversalEnveloping
  (MonomialUniversalEnveloping<coefficient>& input, const coefficient& inputCoeff, SemisimpleLieAlgebraOrdered& owner, const coefficient& theRingUnit, const coefficient& theRingZero, GlobalVariables* theContext);
  bool ModOutFDRelationsExperimental
  (GlobalVariables* theContext, const Vector<Rational> & theHWsimpleCoords, const coefficient& theRingUnit=1, const coefficient& theRingZero=0);
  bool IsEqualToZero()const
  { return this->size==0;
  }
  bool GetElementUniversalEnveloping(ElementUniversalEnveloping<coefficient>& output, SemisimpleLieAlgebra& inputOwner);
  bool GetLieAlgebraElementIfPossible(ElementSemisimpleLieAlgebra<Rational>& output)const;
  void SubstitutionCoefficients(PolynomialSubstitution<Rational>& theSub, GlobalVariables* theContext);
  void MakeConst(const coefficient& coeff, SemisimpleLieAlgebraOrdered& theOwner)
  { this->MakeZero(theOwner);
    MonomialUniversalEnvelopingOrdered<coefficient> tempMon;
    tempMon.MakeConst(coeff, theOwner);
    this->AddMonomial(tempMon);
  }
  void Simplify(GlobalVariables* theContext, const coefficient& theRingUnit=1,  const coefficient& theRingZero=0);
  int GetNumVars()const
  { if (this->size==0)
      return 0;
    else
      return this->TheObjects[0].Coefficient.NumVars;
  }
  inline void MultiplyBy(const ElementUniversalEnvelopingOrdered& other)
  { this->operator*=(other);
  }
  void ModOutVermaRelations(GlobalVariables* theContext, const List<coefficient>* subHiGoesToIthElement=0, const coefficient& theRingUnit=1, const coefficient& theRingZero=0);
  void ModOutVermaRelationSOld
  (bool SubHighestWeightWithZeroes, const PolynomialSubstitution<Rational>& highestWeightSub, GlobalVariables* theContext, const coefficient& theRingUnit);
  template<class CoefficientTypeQuotientField>
  static void GetBasisFromSpanOfElements
  (List<ElementUniversalEnvelopingOrdered<coefficient> >& theElements, Vectors<CoefficientTypeQuotientField>& outputCoords, List<ElementUniversalEnvelopingOrdered<coefficient> >& outputTheBasis,
   const CoefficientTypeQuotientField& theFieldUnit, const CoefficientTypeQuotientField& theFieldZero, GlobalVariables& theGlobalVariables);
  static void GetBasisFromSpanOfElements
  (List<ElementUniversalEnvelopingOrdered>& theElements, Vectors<Polynomial<coefficient> >& outputCoordinates, List<ElementUniversalEnvelopingOrdered>& outputTheBasis, GlobalVariables& theGlobalVariables);
  bool GetCoordsInBasis
  (List<ElementUniversalEnvelopingOrdered<coefficient> >& theBasis, Vector<coefficient>& output, const coefficient& theRingUnit, const coefficient& theRingZero,
   GlobalVariables& theGlobalVariables)const;
  static void GetCoordinateFormOfSpanOfElements
  (int numVars, List<ElementUniversalEnvelopingOrdered>& theElements, Vectors<Polynomial<coefficient> >& outputCoordinates, ElementUniversalEnvelopingOrdered& outputCorrespondingMonomials, GlobalVariables& theGlobalVariables);
//  static void GetCoordinateFormOfSpanOfElements
//  (List<ElementUniversalEnvelopingOrdered>& theElements, Vectors<Rational>& outputCoordinates, ElementUniversalEnvelopingOrdered& outputCorrespondingMonomials, GlobalVariables& theGlobalVariables)
//;
  void AssignFromCoordinateFormWRTBasis
  (List<ElementUniversalEnveloping<coefficient> >& theBasis, Vector<Polynomial<coefficient> >& input, SemisimpleLieAlgebraOrdered& owner);
  void RaiseToPower(int thePower, const coefficient& theRingUnit);
  bool IsAPowerOfASingleGenerator()
  { if (this->size!=1)
      return false;
    MonomialUniversalEnvelopingOrdered<coefficient>& tempMon=this->TheObjects[0];
    if (!tempMon.Coefficient.IsEqualToOne())
      return false;
    if (tempMon.generatorsIndices.size!=1)
      return false;
    return true;
  }
  void MakeCasimir(SemisimpleLieAlgebraOrdered& theOwner, int numVars, GlobalVariables& theGlobalVariables);
  void ActOnMe(const ElementSemisimpleLieAlgebra<Rational>& theElt, ElementUniversalEnvelopingOrdered& output);
  void LieBracketOnTheRight(const ElementUniversalEnvelopingOrdered& right, ElementUniversalEnvelopingOrdered& output);
  void LieBracketOnTheRight(const ElementSemisimpleLieAlgebra<Rational>& right, const coefficient& ringUnit, const coefficient& ringZero);
  void operator=(const ElementUniversalEnvelopingOrdered& other)
  { this->::HashedList<MonomialUniversalEnvelopingOrdered<coefficient> >::operator=(other);
    this->owner=other.owner;
  }
  template<class OtherCoefficientType>
  void AssignChangeCoefficientType (const ElementUniversalEnvelopingOrdered<OtherCoefficientType>& other)
  { this->MakeZero(*other.owner);
    MonomialUniversalEnvelopingOrdered<coefficient> tempMon;
    this->Reserve(other.size);
    for (int i=0; i<other.size; i++)
    { tempMon.AssignChangeCoefficientType<OtherCoefficientType>(other[i]);
      this->AddMonomial(tempMon);
    }
  }
  void operator+=(const ElementUniversalEnvelopingOrdered& other);
  void operator+=(int other);
  void operator+=(const Rational& other);
  void operator-=(const ElementUniversalEnvelopingOrdered& other);
  void operator*=(const ElementUniversalEnvelopingOrdered& other);
  template <class SecondType>
  void operator/=(const SecondType& other);
  template<class otherType>
  void operator*=(const otherType& other);
  ElementUniversalEnvelopingOrdered(){this->owner=0;}
  bool IsProportionalTo(const ElementUniversalEnvelopingOrdered<coefficient>& other, coefficient& outputTimesMeEqualsOther, const coefficient& theRingZero)const;
  ElementUniversalEnvelopingOrdered(const ElementUniversalEnvelopingOrdered& other)
  { this->operator=(other);
  }
  void ClearDenominators(coefficient& outputWasMultipliedBy, const coefficient& theRingUnit)
  { outputWasMultipliedBy=theRingUnit;
    coefficient currentCoeff;
    for (int i=0; i<this->size; i++)
    { MonomialUniversalEnvelopingOrdered<coefficient>& currentMon=this->TheObjects[i];
      currentMon.Coefficient.ClearDenominators(currentCoeff);
      for (int j=0; j<this->size; j++)
        if (j!=i)
          this->TheObjects[j].Coefficient*=currentCoeff;
      outputWasMultipliedBy*=currentCoeff;
    }
  }
};

template <class coefficient>
class MonomialUniversalEnveloping : public MonomialTensor<coefficient>
{
private:
public:
  std::string ToString(FormatExpressions* theFormat=0)const;
  SemisimpleLieAlgebra* owneR;
  // SelectedIndices gives the non-zero powers of the chevalley generators participating in the monomial
  // Powers gives the powers of the Chevalley generators in the order they appear in generatorsIndices
  friend std::ostream& operator << (std::ostream& output, const MonomialUniversalEnveloping<coefficient>& theMon)
  { output << theMon.ToString();
    return output;
  }
  bool IsConstant()const
  { return this->IsEqualToOne();
  }
  static const bool IsMonEqualToZero()
  { return false;
  }
  bool AdjointRepresentationAction(const ElementUniversalEnveloping<coefficient>& input, ElementUniversalEnveloping<coefficient>& output, GlobalVariables* theGlobalVariables=0)const;
  template<class otherType>
  void operator=(const MonomialUniversalEnveloping<otherType>& other)
  { this->MonomialTensor<coefficient>::operator=(other);
    this->owners=other.owners;
    this->indexInOwners=other.indexInOwners;
  }
  SemisimpleLieAlgebra& GetOwner()const
  { return *this->owneR;
  }
  void MakeGenerator(int generatorIndex, SemisimpleLieAlgebra& inputOwner)
  { if (generatorIndex<0 || generatorIndex>inputOwner.GetNumGenerators())
      crash << "This is a programming error: attempting to assign impossible index to monomial UE. " << crash;
    this->owneR=&inputOwner;
    this->generatorsIndices.SetSize(1);
    this->generatorsIndices[0]=generatorIndex;
    this->Powers.SetSize(1);
    this->Powers[0]=1;
  }
  bool HWTAAbilinearForm
  (const MonomialUniversalEnveloping<coefficient>&right, coefficient& output, const Vector<coefficient>* subHiGoesToIthElement, GlobalVariables& theGlobalVariables,
   const coefficient& theRingUnit, const coefficient& theRingZero, std::stringstream* logStream=0);
  void ModOutVermaRelations
  (coefficient& outputCoeff, GlobalVariables* theContext, const Vector<coefficient>* subHiGoesToIthElement=0, const coefficient& theRingUnit=1, const coefficient& theRingZero=0);
  void SetNumVariables(int newNumVars);
  void Substitution(const PolynomialSubstitution<Rational>& theSub);
  unsigned int HashFunction() const
  { return this->::MonomialTensor<coefficient>::HashFunction();
  }
  static inline unsigned int HashFunction(const MonomialUniversalEnveloping<coefficient>& input)
  { return input.HashFunction();
  }
  void GetDegree(coefficient& output)const
  { if (this->Powers.size==0)
    { output.MakeZero();
      return;
    }
    output=this->Powers[0];
    for (int i=1; i<this->generatorsIndices.size; i++)
      output+=(this->Powers[i]);
  }
  bool CommutingABntoBnAPlusLowerOrderAllowed(coefficient& theLeftPower, int leftGeneratorIndex, coefficient& theRightPower, int rightGeneratorIndex);
  bool CommutingAnBtoBAnPlusLowerOrderAllowed(coefficient& theLeftPower, int leftGeneratorIndex, coefficient& theRightPower, int rightGeneratorIndex);
  bool SwitchConsecutiveIndicesIfTheyCommute(int theLeftIndex);
  void MakeOne(SemisimpleLieAlgebra& inputOwner)
  { this->generatorsIndices.size=0;
    this->Powers.size=0;
    this->owneR=&inputOwner;
  }
  //we assume the standard order for being simplified to be Ascending.
  //this way the positive roots will end up being in the end, which is very
  //convenient for computing with Verma modules.
  //Format of the order of the generators:
  // first come the negative roots, in increasing height, then the elements of
  //the Cartan subalgebra, then the positive roots, in increasing height
  //The order of the positive roots is the same as the order in which roots are kept
  //in WeylGroup::RootSystem.
  // The "zero level roots" - i.e. the elements of the Cartan subalgebra lie in between
  // the negative and positive rootss.
  void Simplify(ElementUniversalEnveloping<coefficient>& output, GlobalVariables& theGlobalVariables, const coefficient& theRingUnit=1, const coefficient& theRingZero=0);
  void CommuteABntoBnAPlusLowerOrder(int theIndeX, ElementUniversalEnveloping<coefficient>& output, const coefficient& theRingUnit=1, const coefficient& theRingZero=0);
  void CommuteAnBtoBAnPlusLowerOrder(int indexA, ElementUniversalEnveloping<coefficient>& output, const coefficient& theRingUnit=1, const coefficient& theRingZero=0);
  MonomialUniversalEnveloping():owneR(0){}
  bool operator>(const MonomialUniversalEnveloping& other)
  { return this->::MonomialTensor<coefficient>::operator>(other);
  }
  bool operator==(const MonomialUniversalEnveloping& other)const
  { return this->owneR==other.owneR && this->Powers==other.Powers && this->generatorsIndices==other.generatorsIndices;
  }
  inline void operator=(const MonomialUniversalEnveloping& other)
  { this->::MonomialTensor<coefficient>::operator=(other);
    this->owneR=other.owneR;
  }
  inline void operator*=(const MonomialUniversalEnveloping& other)
  { this->::MonomialTensor<coefficient>::operator*=(other);
  }
};

template <class coefficient>
class ElementUniversalEnveloping: public ElementMonomialAlgebra<MonomialUniversalEnveloping<coefficient>, coefficient>
{
private:
  void CleanUpZeroCoeff();
  friend class MonomialUniversalEnveloping<coefficient>;
public:
  SemisimpleLieAlgebra* owneR;
  bool AdjointRepresentationAction(const ElementUniversalEnveloping<coefficient>& input, ElementUniversalEnveloping<coefficient>& output, GlobalVariables* theGlobalVariables=0)const;
  bool ConvertToRationalCoeff(ElementUniversalEnveloping<Rational>& output);
  bool IsEqualToZero()const
  { return this->size()==0;
  }
  bool HWMTAbilinearForm
  (const ElementUniversalEnveloping<coefficient>&right, coefficient& output, const Vector<coefficient>* subHiGoesToIthElement, GlobalVariables& theGlobalVariables,
   const coefficient& theRingUnit, const coefficient& theRingZero, std::stringstream* logStream=0);
  std::string IsInProperSubmodule
  (const Vector<coefficient>* subHiGoesToIthElement, GlobalVariables& theGlobalVariables, const coefficient& theRingUnit, const coefficient& theRingZero);
  bool HWTAAbilinearForm
  (const ElementUniversalEnveloping<coefficient>&right, coefficient& output, const Vector<coefficient>* subHiGoesToIthElement, GlobalVariables& theGlobalVariables,
   const coefficient& theRingUnit, const coefficient& theRingZero, std::stringstream* logStream=0)const;
  bool HWTAAbilinearForm
  (const MonomialUniversalEnveloping<coefficient>&right, coefficient& output, const Vector<coefficient>* subHiGoesToIthElement, GlobalVariables& theGlobalVariables, const coefficient& theRingUnit, const coefficient& theRingZero, std::stringstream* logStream=0)const
  { ElementUniversalEnveloping<coefficient> tempElt;
    tempElt.MakeZero(*this->owner);
    tempElt.AddMonomial(right, theRingUnit);
    return this->HWTAAbilinearForm(tempElt, output, subHiGoesToIthElement, theGlobalVariables, theRingUnit, theRingZero, logStream);
  }
  bool NeedsBracketForMultiplication()
  { return this->size>1;
  }
  bool ApplyMinusTransposeAutoOnMe();
  bool ApplyTransposeAntiAutoOnMe();
  void MakeHgenerator(const Vector<Rational>& input, SemisimpleLieAlgebra& inputOwner);
  void AssignElementLieAlgebra(const ElementSemisimpleLieAlgebra<Rational>& input, SemisimpleLieAlgebra& inputOwner, const coefficient& theRingUnit=1, const coefficient& theRingZero=0);
  bool GetWithSimpleGeneratorsOnly(MonomialCollection<MonomialTensor<coefficient>, coefficient>& output);
  void MakeOneGenerator(int theIndex, SemisimpleLieAlgebra& inputOwner, const coefficient& theRingUnit);
  void MakeOneGeneratorCoeffOne(int theIndex, SemisimpleLieAlgebra& inputOwners, const coefficient& theRingUnit=1, const coefficient& theRingZero=0);
  void MakeOneGeneratorCoeffOne(int theIndex, int numVars, SemisimpleLieAlgebra& inputOwner);
  void MakeOneGeneratorCoeffOne(const Vector<Rational>& rootSpace, SemisimpleLieAlgebra& inputOwner, const coefficient& theRingUnit=1, const coefficient& theRingZero=0)
  { this->MakeOneGeneratorCoeffOne(inputOwner.GetGeneratorFromRoot(rootSpace), inputOwner, theRingUnit, theRingZero);
  }
  coefficient GetKillingFormProduct(const ElementUniversalEnveloping<coefficient>& right)const;
  void MakeZero(SemisimpleLieAlgebra& inputOwner);
  bool GetLieAlgebraElementIfPossible(ElementSemisimpleLieAlgebra<Rational>& output)const;
  void MakeConst(const Rational& coeff, int numVars, SemisimpleLieAlgebra& inputOwner);
  void MakeConst(const coefficient& coeff, SemisimpleLieAlgebra& inputOwner)
  { this->MakeZero(inputOwner);
    MonomialUniversalEnveloping<coefficient> tempMon;
    tempMon.MakeOne(inputOwner);
    this->AddMonomial(tempMon, coeff);
  }
  void Simplify(GlobalVariables* theGlobalVariables=0, const coefficient& theRingUnit=1, const coefficient& theRingZero=0);
  int GetMinNumVars()const
  { int result=0;
    for (int i=0; i<this->size; i++)
    { result=MathRoutines::Maximum(result, this->theCoeffs[i].GetMinNumVars());
      result=MathRoutines::Maximum(result, (*this)[i].GetMinNumVars());
    }
    return result;
  }
  inline void MultiplyBy(const MonomialUniversalEnveloping<coefficient>& standsOnTheRight, const coefficient& theCoeff);
  void ModToMinDegreeFormFDRels(const Vector<Rational> & theHWinSimpleCoords, GlobalVariables& theGlobalVariables, const coefficient& theRingUnit, const coefficient& theRingZero);
  void ModOutVermaRelations(GlobalVariables* theContext, const Vector<coefficient>* subHiGoesToIthElement, const coefficient& theRingUnit=1, const coefficient& theRingZero=0);
  static void GetCoordinateFormOfSpanOfElements
  (List<ElementUniversalEnveloping<coefficient> >& theElements, Vectors<coefficient>& outputCoordinates, ElementUniversalEnveloping<coefficient>& outputCorrespondingMonomials,
   GlobalVariables& theGlobalVariables);
  bool GetCoordsInBasis
  (List<ElementUniversalEnveloping<coefficient> >& theBasis, Vector<coefficient>& output, const coefficient& theRingUnit, const coefficient& theRingZero, GlobalVariables& theGlobalVariables)const;
  static inline unsigned int HashFunction (const ElementUniversalEnveloping<coefficient>& input)
  { return input.HashFunction();
  }
  unsigned int HashFunction()const
  { return this->::MonomialCollection<MonomialUniversalEnveloping<coefficient>, coefficient>::HashFunction();
  }
  template<class CoefficientTypeQuotientField>
  static bool GetBasisFromSpanOfElements
  (List<ElementUniversalEnveloping<coefficient> >& theElements, Vectors<CoefficientTypeQuotientField>& outputCoords, List<ElementUniversalEnveloping<coefficient> >& outputTheBasis,
   const CoefficientTypeQuotientField& theFieldUnit, const CoefficientTypeQuotientField& theFieldZero, GlobalVariables& theGlobalVariables);
  void AssignFromCoordinateFormWRTBasis(List<ElementUniversalEnveloping<coefficient> >& theBasis, Vector<coefficient>& input, SemisimpleLieAlgebra& owner);
  void RaiseToPower(int thePower);
  bool IsAPowerOfASingleGenerator()const
  { if (this->size()!=1)
      return false;
    if (!this->theCoeffs[0].IsEqualToOne())
      return false;
    return (*this)[0].generatorsIndices.size==1;
  }
  void SubstitutionCoefficients(PolynomialSubstitution<Rational>& theSub, GlobalVariables* theContext, const coefficient& theRingUnit, const coefficient& theRingZero);
  void Substitution(const PolynomialSubstitution<Rational>& theSub);
  void MakeCasimir(SemisimpleLieAlgebra& theOwner);
  void MakeCasimirWRTLeviParabolic(SemisimpleLieAlgebra& theOwner, const Selection& theLeviRoots);
  static void LieBracket(const ElementUniversalEnveloping<coefficient>& left, const ElementUniversalEnveloping<coefficient>& right, ElementUniversalEnveloping<coefficient>& output)
  { left.LieBracketOnTheRight(right, output);
    output.Simplify();
  }
  void LieBracketOnTheRight(const ElementUniversalEnveloping<coefficient>& right, ElementUniversalEnveloping<coefficient>& output)const;
  void LieBracketOnTheLeft(const ElementSemisimpleLieAlgebra<Rational>& left);
  void AssignInt(int coeff, int numVars, SemisimpleLieAlgebra& theOwner)
  { Rational tempRat=coeff;
    this->MakeConst(tempRat, numVars, &theOwner);
  }
  SemisimpleLieAlgebra& GetOwner()const
  { return *this->owneR;
  }
  template <class otherType>
  void Assign(const ElementUniversalEnveloping<otherType>& other)
  { this->owners=other.owners;
    this->indexInOwners=other.indexInOwners;
    MonomialUniversalEnveloping<coefficient> tempMon;
    coefficient theCoeff;
    for (int i=0; i<other.size; i++)
    { tempMon=other[i];
      theCoeff=other.theCoeffs[i];
      this->AddMonomial(tempMon, theCoeff);
    }
  }
  void operator=(const Rational& other)
  { this->MakeConst(other, 0, *this->owner);
  }
  void operator=(const ElementUniversalEnveloping<coefficient>& other)
  { this->::MonomialCollection<MonomialUniversalEnveloping<coefficient>, coefficient>::operator=(other);
    this->owneR=other.owneR;
  }
  void operator*=(const ElementUniversalEnveloping<coefficient>& standsOnTheRight);
  void operator*=(const coefficient& other)
  { this->::MonomialCollection<MonomialUniversalEnveloping<coefficient>, coefficient>
    ::operator*=(other);
  }
  template<class otherType>
  void operator/=(const otherType& other)
  { this->::MonomialCollection<MonomialUniversalEnveloping<coefficient>, coefficient>
    ::operator/=(other);
  }
  ElementUniversalEnveloping<coefficient>():owneR(0){}
  ElementUniversalEnveloping<coefficient>(const ElementUniversalEnveloping<coefficient>& other){this->operator=(other);}
};

template <class coefficient>
class ElementVermaModuleOrdered
{
public:
  ElementUniversalEnvelopingOrdered<coefficient> theElT;
  std::string DebugString;
  PolynomialSubstitution<coefficient> theSubNthElementIsImageNthCoordSimpleBasis;
  void ComputeDebugString()
  { FormatExpressions tempFormat;
    this->DebugString=this->ToString(tempFormat);
  }
  std::string ToString(const FormatExpressions& theFormat)const;
  void ToString(std::string& output)const
  { output=this->ToString();
  }
  bool IsEqualToZero()const
  { return this->theElT.IsEqualToZero();
  }
  bool NeedsParenthesisForMultiplication()const;
  void AssignElementUniversalEnvelopingOrderedTimesHighestWeightVector
  (ElementUniversalEnvelopingOrdered<coefficient>& input, const ElementVermaModuleOrdered<coefficient>& theRingZero, GlobalVariables* theContext, const coefficient& theRingUnit);
  void ActOnMe
  (const ElementSemisimpleLieAlgebra<Rational>& actingElt, ElementVermaModuleOrdered<coefficient>& output, SemisimpleLieAlgebra& owner, const coefficient& theRingUnit, const coefficient& theRingZero, GlobalVariables* theContext)const;
  static void GetBasisFromSpanOfElements
  (List<ElementVermaModuleOrdered<coefficient> >& theElements, Vectors<RationalFunctionOld>& outputCoordinates, List<ElementVermaModuleOrdered>& outputTheBasis,
   const RationalFunctionOld& RFOne, const RationalFunctionOld& RFZero, GlobalVariables& theGlobalVariables);
  bool GetCoordsInBasis
  (const List<ElementVermaModuleOrdered<coefficient> >& theBasis, Vector<coefficient>& output, const coefficient& theRingUnit, const coefficient& theRingZero, GlobalVariables& theGlobalVariables)const;
  bool IsProportionalTo(const ElementVermaModuleOrdered<coefficient>& other, coefficient& outputTimesMeEqualsOther, const coefficient& theRingZero)const
  { return this->theElT.IsProportionalTo(other.theElT, outputTimesMeEqualsOther, theRingZero);
  }
  void MakeZero(SemisimpleLieAlgebraOrdered& owner, PolynomialSubstitution<Rational>& incomingSub)
  { this->theElT.MakeZero(owner);
    this->theSubNthElementIsImageNthCoordSimpleBasis=incomingSub;
  }
  template <class CoefficientTypeOther>
  void operator*=(const CoefficientTypeOther& theConst)
  { this->theElT.operator*=<CoefficientTypeOther>(theConst);
  }
  void MultiplyOnTheLeft
  (const ElementSemisimpleLieAlgebra<Rational>& other, ElementVermaModuleOrdered<coefficient>& output, const coefficient& theRingUnit,
   const coefficient& theRingZero, GlobalVariables* theContext);
  void ClearDenominators(coefficient& outputWasMultipliedBy, const coefficient& theRingUnit)
  { this->theElT.ClearDenominators(outputWasMultipliedBy, theRingUnit);
  }
  void operator/=(const coefficient& theConst){this->theElT.operator/=(theConst);}
  void operator-=(const ElementVermaModuleOrdered& other)
  { this->theElT-=other.theElT;
  }
  void operator+=(const ElementVermaModuleOrdered& other)
  { this->theElT+=other.theElT;
  }
  void operator=(const ElementVermaModuleOrdered& other)
  { this->theElT=other.theElT;
    this->theSubNthElementIsImageNthCoordSimpleBasis=other.theSubNthElementIsImageNthCoordSimpleBasis;
  }
};


#endif
