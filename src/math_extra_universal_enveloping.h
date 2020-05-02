// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader1_HeaderUniversalEnveloping
#define vpfHeader1_HeaderUniversalEnveloping
#include "math_general.h"
#include "math_extra_Lie_theory_extras.h"

template <class Coefficient>
class MonomialUniversalEnveloping : public MonomialTensor<Coefficient> {
private:
public:
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  SemisimpleLieAlgebra* owner;
  // SelectedIndices gives the non-zero powers of the chevalley generators participating in the monomial
  // Powers gives the powers of the Chevalley generators in the order they appear in generatorsIndices
  friend std::ostream& operator<<(std::ostream& output, const MonomialUniversalEnveloping<Coefficient>& theMon) {
    output << theMon.toString();
    return output;
  }
  bool isConstant() const {
    return this->isEqualToOne();
  }
  bool AdjointRepresentationAction(
    const ElementUniversalEnveloping<Coefficient>& input, ElementUniversalEnveloping<Coefficient>& output
  ) const;
  template<class otherType>
  void operator=(const MonomialUniversalEnveloping<otherType>& other) {
    this->MonomialTensor<Coefficient>::operator=(other);
    this->owners = other.owners;
    this->indexInOwners = other.indexInOwners;
  }
  SemisimpleLieAlgebra& getOwner() const {
    return *this->owner;
  }
  void MakeGenerator(int generatorIndex, SemisimpleLieAlgebra& inputOwner) {
    if (generatorIndex < 0 || generatorIndex>inputOwner.GetNumGenerators()) {
      global.fatal << "This is a programming error: attempting to assign impossible index to monomial UE. " << global.fatal;
    }
    this->owner = &inputOwner;
    this->generatorsIndices.setSize(1);
    this->generatorsIndices[0] = generatorIndex;
    this->Powers.setSize(1);
    this->Powers[0] = 1;
  }
  bool HWTAAbilinearForm(
    const MonomialUniversalEnveloping<Coefficient>& right,
    Coefficient& output,
    const Vector<Coefficient>* subHiGoesToIthElement,
    const Coefficient& theRingUnit,
    const Coefficient& theRingZero,
    std::stringstream* logStream = nullptr
  );
  void modOutVermaRelations(
    Coefficient& outputCoeff,
    const Vector<Coefficient>* subHiGoesToIthElement = 0,
    const Coefficient& theRingUnit = 1,
    const Coefficient& theRingZero = 0
  );
  void setNumberOfVariables(int newNumVars);
  void substitution(const PolynomialSubstitution<Rational>& theSub);
  unsigned int hashFunction() const {
    return this->::MonomialTensor<Coefficient>::hashFunction();
  }
  static inline unsigned int hashFunction(const MonomialUniversalEnveloping<Coefficient>& input) {
    return input.hashFunction();
  }
  void getDegree(Coefficient& output) const {
    if (this->Powers.size == 0) {
      output.makeZero();
      return;
    }
    output = this->Powers[0];
    for (int i = 1; i < this->generatorsIndices.size; i ++) {
      output += (this->Powers[i]);
    }
  }
  bool CommutingABntoBnAPlusLowerOrderAllowed(
    Coefficient& theLeftPower, int leftGeneratorIndex, Coefficient& theRightPower, int rightGeneratorIndex
  );
  bool CommutingAnBtoBAnPlusLowerOrderAllowed(
    Coefficient& theLeftPower, int leftGeneratorIndex, Coefficient& theRightPower, int rightGeneratorIndex
  );
  bool switchConsecutiveIndicesIfTheyCommute(int theLeftIndex);
  void makeOne(SemisimpleLieAlgebra& inputOwner) {
    this->generatorsIndices.size = 0;
    this->Powers.size = 0;
    this->owner = &inputOwner;
  }
  // We assume the standard order for being simplified to be Ascending.
  // This way the positive roots will end up being in the end, which is very
  // convenient for computations with Verma modules.
  // Format of the order of the generators:
  // first come the negative roots, in increasing height, then the elements of
  // the Cartan subalgebra, then the positive roots, in increasing height
  // The order of the positive roots is the same as the order in which roots are kept
  // in WeylGroup::RootSystem.
  // The "zero level roots" - i.e. the elements of the Cartan subalgebra lie in between
  // the negative and positive rootss.
  void simplify(ElementUniversalEnveloping<Coefficient>& output, const Coefficient& theRingUnit = 1);
  void CommuteABntoBnAPlusLowerOrder(
    int theIndeX, ElementUniversalEnveloping<Coefficient>& output, const Coefficient& theRingUnit = 1
  );
  void CommuteAnBtoBAnPlusLowerOrder(
    int indexA, ElementUniversalEnveloping<Coefficient>& output, const Coefficient& theRingUnit = 1
  );
  MonomialUniversalEnveloping(): owner(nullptr) {}
  bool operator>(const MonomialUniversalEnveloping& other) const {
    return this->::MonomialTensor<Coefficient>::operator>(other);
  }
  bool operator==(const MonomialUniversalEnveloping& other) const {
    return this->owner == other.owner && this->Powers == other.Powers && this->generatorsIndices == other.generatorsIndices;
  }
  inline void operator=(const MonomialUniversalEnveloping& other) {
    this->::MonomialTensor<Coefficient>::operator=(other);
    this->owner = other.owner;
  }
  inline void operator*=(const MonomialUniversalEnveloping& other) {
    this->::MonomialTensor<Coefficient>::operator*=(other);
  }
};

template <class Coefficient>
class ElementUniversalEnveloping: public ElementMonomialAlgebra<MonomialUniversalEnveloping<Coefficient>, Coefficient> {
private:
  void cleanUpZeroCoefficient();
  friend class MonomialUniversalEnveloping<Coefficient>;
public:
  SemisimpleLieAlgebra* owner;
  bool AdjointRepresentationAction(
    const ElementUniversalEnveloping<Coefficient>& input, ElementUniversalEnveloping<Coefficient>& output
  ) const;
  bool ConvertToRationalCoeff(ElementUniversalEnveloping<Rational>& output);
  bool isEqualToZero() const {
    return this->size() == 0;
  }
  bool HWMTAbilinearForm(
    const ElementUniversalEnveloping<Coefficient>& right,
    Coefficient& output,
    const Vector<Coefficient>* subHiGoesToIthElement,
    const Coefficient& theRingUnit,
    const Coefficient& theRingZero,
    std::stringstream* logStream = nullptr
  );
  std::string IsInProperSubmodule(
    const Vector<Coefficient>* subHiGoesToIthElement, const Coefficient& theRingUnit, const Coefficient& theRingZero
  );
  bool HWTAAbilinearForm(
    const ElementUniversalEnveloping<Coefficient>&right,
    Coefficient& output,
    const Vector<Coefficient>* subHiGoesToIthElement,
    const Coefficient& theRingUnit,
    const Coefficient& theRingZero,
    std::stringstream* logStream = nullptr
  ) const;
  bool HWTAAbilinearForm(
    const MonomialUniversalEnveloping<Coefficient>& right,
    Coefficient& output,
    const Vector<Coefficient>* subHiGoesToIthElement,
    const Coefficient& theRingUnit,
    const Coefficient& theRingZero,
    std::stringstream* logStream = nullptr
  ) const {
    ElementUniversalEnveloping<Coefficient> tempElt;
    tempElt.makeZero(*this->owner);
    tempElt.addMonomial(right, theRingUnit);
    return this->HWTAAbilinearForm(tempElt, output, subHiGoesToIthElement, theRingUnit, theRingZero, logStream);
  }
  bool NeedsBracketForMultiplication() {
    return this->size > 1;
  }
  bool ApplyMinusTransposeAutoOnMe();
  bool ApplyTransposeAntiAutoOnMe();
  void makeCartanGenerator(const Vector<Rational>& input, SemisimpleLieAlgebra& inputOwner);
  void assignElementLieAlgebra(
    const ElementSemisimpleLieAlgebra<Rational>& input, SemisimpleLieAlgebra& inputOwner, const Coefficient& theRingUnit = 1
  );
  bool GetWithSimpleGeneratorsOnly(LinearCombination<MonomialTensor<Coefficient>, Coefficient>& output);
  void MakeOneGenerator(int theIndex, SemisimpleLieAlgebra& inputOwner, const Coefficient& theRingUnit);
  void MakeOneGeneratorCoeffOne(int theIndex, SemisimpleLieAlgebra& inputOwners, const Coefficient& theRingUnit = 1);
  void MakeOneGeneratorCoeffOne(int theIndex, int numVars, SemisimpleLieAlgebra& inputOwner);
  void MakeOneGeneratorCoeffOne(
    const Vector<Rational>& rootSpace, SemisimpleLieAlgebra& inputOwner, const Coefficient& theRingUnit = 1
  ) {
    this->MakeOneGeneratorCoeffOne(inputOwner.GetGeneratorFromRoot(rootSpace), inputOwner, theRingUnit);
  }
  Coefficient GetKillingFormProduct(const ElementUniversalEnveloping<Coefficient>& right) const;
  void makeZero(SemisimpleLieAlgebra& inputOwner);
  bool getLieAlgebraElementIfPossible(ElementSemisimpleLieAlgebra<Rational>& output) const;
  void makeConstant(const Coefficient& coeff, SemisimpleLieAlgebra& inputOwner) {
    this->makeZero(inputOwner);
    MonomialUniversalEnveloping<Coefficient> tempMon;
    tempMon.makeOne(inputOwner);
    this->addMonomial(tempMon, coeff);
  }
  void simplify(const Coefficient& theRingUnit = 1);
  int minimalNumberOfVariables() const {
    int result = 0;
    for (int i = 0; i < this->size; i ++) {
      result = MathRoutines::Maximum(result, this->coefficients[i].minimalNumberOfVariables());
      result = MathRoutines::Maximum(result, (*this)[i].minimalNumberOfVariables());
    }
    return result;
  }
  void multiplyBy(const MonomialUniversalEnveloping<Coefficient>& standsOnTheRight, const Coefficient& theCoeff);
  void ModToMinDegreeFormFDRels(
    const Vector<Rational> & theHWinSimpleCoords, const Coefficient& theRingUnit, const Coefficient& theRingZero
  );
  void modOutVermaRelations(
    const Vector<Coefficient>* subHiGoesToIthElement, const Coefficient& theRingUnit = 1, const Coefficient& theRingZero = 0
  );
  static void GetCoordinateFormOfSpanOfElements(
    List<ElementUniversalEnveloping<Coefficient> >& theElements,
    Vectors<Coefficient>& outputCoordinates,
    ElementUniversalEnveloping<Coefficient>& outputCorrespondingMonomials
  );
  bool getCoordinatesInBasis(
    List<ElementUniversalEnveloping<Coefficient> >& theBasis,
    Vector<Coefficient>& output,
    const Coefficient& theRingUnit,
    const Coefficient& theRingZero
  ) const;
  static inline unsigned int hashFunction (const ElementUniversalEnveloping<Coefficient>& input) {
    return input.hashFunction();
  }
  unsigned int hashFunction() const {
    return this->::LinearCombination<MonomialUniversalEnveloping<Coefficient>, Coefficient>::hashFunction();
  }
  template<class CoefficientTypeQuotientField>
  static bool getBasisFromSpanOfElements(
    List<ElementUniversalEnveloping<Coefficient> >& theElements,
    Vectors<CoefficientTypeQuotientField>& outputCoords,
    List<ElementUniversalEnveloping<Coefficient> >& outputTheBasis,
    const CoefficientTypeQuotientField& theFieldUnit,
    const CoefficientTypeQuotientField& theFieldZero
  );
  void assignFromCoordinateFormWithRespectToBasis(
    List<ElementUniversalEnveloping<Coefficient> >& theBasis, Vector<Coefficient>& input, SemisimpleLieAlgebra& owner
  );
  void raiseToPower(int thePower);
  bool isPowerOfSingleGenerator() const {
    if (this->size() != 1) {
      return false;
    }
    if (!this->coefficients[0].isEqualToOne()) {
      return false;
    }
    return (*this)[0].generatorsIndices.size == 1;
  }
  void substitutionCoefficients(
    PolynomialSubstitution<Rational>& theSub, const Coefficient& theRingUnit, const Coefficient& theRingZero
  );
  void substitution(const PolynomialSubstitution<Rational>& theSub);
  void makeCasimir(SemisimpleLieAlgebra& theOwner);
  void MakeCasimirWRTLeviParabolic(SemisimpleLieAlgebra& theOwner, const Selection& theLeviRoots);
  static void LieBracket(
    const ElementUniversalEnveloping<Coefficient>& left,
    const ElementUniversalEnveloping<Coefficient>& right,
    ElementUniversalEnveloping<Coefficient>& output
  ) {
    left.lieBracketOnTheRight(right, output);
    output.simplify();
  }
  void lieBracketOnTheRight(
    const ElementUniversalEnveloping<Coefficient>& right, ElementUniversalEnveloping<Coefficient>& output
  ) const;
  void LieBracketOnTheLeft(const ElementSemisimpleLieAlgebra<Rational>& left);
  void AssignInt(int coeff, int numVars, SemisimpleLieAlgebra& theOwner) {
    Rational tempRat = coeff;
    this->makeConstant(tempRat, numVars, &theOwner);
  }
  SemisimpleLieAlgebra& getOwner() const {
    return *this->owner;
  }
  template <class otherType>
  void Assign(const ElementUniversalEnveloping<otherType>& other) {
    this->owners = other.owners;
    this->indexInOwners = other.indexInOwners;
    MonomialUniversalEnveloping<Coefficient> tempMon;
    Coefficient theCoeff;
    for (int i = 0; i < other.size; i ++) {
      tempMon = other[i];
      theCoeff = other.coefficients[i];
      this->addMonomial(tempMon, theCoeff);
    }
  }
  void operator=(const Rational& other) {
    this->makeConstant(other, 0, *this->owner);
  }
  void operator=(const ElementUniversalEnveloping<Coefficient>& other) {
    this->::LinearCombination<MonomialUniversalEnveloping<Coefficient>, Coefficient>::operator=(other);
    this->owner = other.owner;
  }
  void operator*=(const ElementUniversalEnveloping<Coefficient>& standsOnTheRight);
  void operator*=(const Coefficient& other) {
    this->::LinearCombination<MonomialUniversalEnveloping<Coefficient>, Coefficient>
    ::operator*=(other);
  }
  template<class otherType>
  void operator/=(const otherType& other) {
    this->::LinearCombination<MonomialUniversalEnveloping<Coefficient>, Coefficient>
    ::operator/=(other);
  }
  ElementUniversalEnveloping<Coefficient>(): owner(nullptr) {
  }
  ElementUniversalEnveloping<Coefficient>(const ElementUniversalEnveloping<Coefficient>& other) {
    this->operator=(other);
  }
};

#endif
