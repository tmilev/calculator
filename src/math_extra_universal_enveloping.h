 // The current file is licensed under the license terms found in the main header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_math_extra_universal_enveloping_ALREADY_INCLUDED
#define header_math_extra_universal_enveloping_ALREADY_INCLUDED

#include "math_general.h"
#include "math_extra_lie_theory_extras.h"

template <class Coefficient>
class MonomialUniversalEnveloping: public MonomialTensor<Coefficient> {
private:
public:
  std::string toString(FormatExpressions* format = nullptr) const;
  SemisimpleLieAlgebra* owner;
  // SelectedIndices gives the non-zero powers of the chevalley generators
  // participating in the monomial
  // powers gives the powers of the Chevalley generators in the order they
  // appear in generatorsIndices
  friend std::ostream& operator<<(
    std::ostream& output,
    const MonomialUniversalEnveloping<Coefficient>& monomial
  ) {
    output << monomial.toString();
    return output;
  }
  bool isConstant() const {
    return this->isEqualToOne();
  }
  bool adjointRepresentationAction(
    const ElementUniversalEnveloping<Coefficient>& input,
    ElementUniversalEnveloping<Coefficient>& output
  ) const;
  template <class OtherType>
  void operator=(const MonomialUniversalEnveloping<OtherType>& other) {
    this->MonomialTensor<Coefficient>::operator=(other);
    this->owners = other.owners;
    this->indexInOwners = other.indexInOwners;
  }
  SemisimpleLieAlgebra& getOwner() const {
    return *this->owner;
  }
  void makeGenerator(int generatorIndex, SemisimpleLieAlgebra& inputOwner) {
    if (
      generatorIndex < 0 || generatorIndex > inputOwner.getNumberOfGenerators()
    ) {
      global.fatal
      << "Attempt to assign impossible index to monomial UE. "
      << global.fatal;
    }
    this->owner = &inputOwner;
    this->generatorsIndices.setSize(1);
    this->generatorsIndices[0] = generatorIndex;
    this->powers.setSize(1);
    this->powers[0] = 1;
  }
  bool highestWeightTransposeAntiAutomorphismBilinearForm(
    const MonomialUniversalEnveloping<Coefficient>& right,
    Coefficient& output,
    const Vector<Coefficient>* substitutionHiGoesToIthElement,
    const Coefficient& ringUnit,
    const Coefficient& ringZero,
    std::stringstream* logStream = nullptr
  );
  void modOutVermaRelations(
    Coefficient& outputCoeff,
    const Vector<Coefficient>* substitutionHiGoesToIthElement = 0,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero = 0
  );
  void setNumberOfVariables(int newNumberOfVariables);
  void substitution(const PolynomialSubstitution<Rational>& substitution);
  unsigned int hashFunction() const {
    return this->::MonomialTensor<Coefficient>::hashFunction();
  }
  static inline unsigned int hashFunction(
    const MonomialUniversalEnveloping<Coefficient>& input
  ) {
    return input.hashFunction();
  }
  void getDegree(Coefficient& output) const {
    if (this->powers.size == 0) {
      output.makeZero();
      return;
    }
    output = this->powers[0];
    for (int i = 1; i < this->generatorsIndices.size; i ++) {
      output += (this->powers[i]);
    }
  }
  bool commutingABntoBnAPlusLowerOrderAllowed(
    Coefficient& leftPower,
    int leftGeneratorIndex,
    Coefficient& rightPower,
    int rightGeneratorIndex
  );
  bool commutingAnBtoBAnPlusLowerOrderAllowed(
    Coefficient& leftPower,
    int leftGeneratorIndex,
    Coefficient& rightPower,
    int rightGeneratorIndex
  );
  bool switchConsecutiveIndicesIfTheyCommute(int leftIndex);
  void makeOne(SemisimpleLieAlgebra& inputOwner) {
    this->generatorsIndices.size = 0;
    this->powers.size = 0;
    this->owner = &inputOwner;
  }
  // We assume the standard order for being simplified to be Ascending.
  // This way the positive roots will end up being in the end, which is very
  // convenient for computations with Verma modules.
  // Format of the order of the generators:
  // first come the negative roots, in increasing height, then the elements of
  // the Cartan subalgebra, then the positive roots, in increasing height
  // The order of the positive roots is the same as the order in which roots are
  // kept
  // in WeylGroup::RootSystem.
  // The "zero level roots" - i.e. the elements of the Cartan subalgebra lie in
  // between
  // the negative and positive rootss.
  void simplify(
    ElementUniversalEnveloping<Coefficient>& output,
    const Coefficient& ringUnit = 1
  );
  void commuteABntoBnAPlusLowerOrder(
    int index,
    ElementUniversalEnveloping<Coefficient>& output,
    const Coefficient& ringUnit = 1
  );
  void commuteAnBtoBAnPlusLowerOrder(
    int indexA,
    ElementUniversalEnveloping<Coefficient>& output,
    const Coefficient& ringUnit = 1
  );
  MonomialUniversalEnveloping(): owner(nullptr) {}
  bool operator>(const MonomialUniversalEnveloping& other) const {
    return this->::MonomialTensor<Coefficient>::operator>(other);
  }
  bool operator==(const MonomialUniversalEnveloping& other) const {
    return
    this->owner == other.owner &&
    this->powers == other.powers &&
    this->generatorsIndices == other.generatorsIndices;
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
class ElementUniversalEnveloping: public ElementMonomialAlgebra<
  MonomialUniversalEnveloping<Coefficient>, Coefficient
> {
private:
  void cleanUpZeroCoefficient();
  friend class MonomialUniversalEnveloping<Coefficient>;
public:
  SemisimpleLieAlgebra* owner;
  bool adjointRepresentationAction(
    const ElementUniversalEnveloping<Coefficient>& input,
    ElementUniversalEnveloping<Coefficient>& output
  ) const;
  bool convertToRationalCoefficient(
    ElementUniversalEnveloping<Rational>& output
  );
  bool isEqualToZero() const {
    return this->size() == 0;
  }
  bool highestWeightMTAbilinearForm(
    const ElementUniversalEnveloping<Coefficient>& right,
    Coefficient& output,
    const Vector<Coefficient>* substitutionHiGoesToIthElement,
    const Coefficient& ringUnit,
    const Coefficient& ringZero,
    std::stringstream* logStream = nullptr
  );
  std::string isInProperSubmodule(
    const Vector<Coefficient>* substitutionHiGoesToIthElement,
    const Coefficient& ringUnit,
    const Coefficient& ringZero
  );
  bool highestWeightTransposeAntiAutomorphismBilinearForm(
    const ElementUniversalEnveloping<Coefficient>& right,
    Coefficient& output,
    const Vector<Coefficient>* substitutionHiGoesToIthElement,
    const Coefficient& ringUnit,
    const Coefficient& ringZero,
    std::stringstream* logStream = nullptr
  ) const;
  bool highestWeightTransposeAntiAutomorphismBilinearForm(
    const MonomialUniversalEnveloping<Coefficient>& right,
    Coefficient& output,
    const Vector<Coefficient>* substitutionHiGoesToIthElement,
    const Coefficient& ringUnit,
    const Coefficient& ringZero,
    std::stringstream* logStream = nullptr
  ) const {
    ElementUniversalEnveloping<Coefficient> element;
    element.makeZero(*this->owner);
    element.addMonomial(right, ringUnit);
    return
    this->highestWeightTransposeAntiAutomorphismBilinearForm(
      element,
      output,
      substitutionHiGoesToIthElement,
      ringUnit,
      ringZero,
      logStream
    );
  }
  bool needsBracketForMultiplication() {
    return this->size > 1;
  }
  bool applyMinusTransposeAutoOnMe();
  bool applyTransposeAntiAutoOnMe();
  void makeCartanGenerator(
    const Vector<Rational>& input, SemisimpleLieAlgebra& inputOwner
  );
  void assignElementLieAlgebra(
    const ElementSemisimpleLieAlgebra<Rational>& input,
    SemisimpleLieAlgebra& inputOwner,
    const Coefficient& ringUnit = 1
  );
  void makeOneGenerator(
    int index,
    SemisimpleLieAlgebra& inputOwner,
    const Coefficient& ringUnit
  );
  void makeOneGeneratorCoefficientOne(
    int index,
    SemisimpleLieAlgebra& inputOwners,
    const Coefficient& ringUnit = 1
  );
  void makeOneGeneratorCoefficientOne(
    int index, int numberOfVariables, SemisimpleLieAlgebra& inputOwner
  );
  void makeOneGeneratorCoefficientOne(
    const Vector<Rational>& rootSpace,
    SemisimpleLieAlgebra& inputOwner,
    const Coefficient& ringUnit = 1
  ) {
    this->makeOneGeneratorCoefficientOne(
      inputOwner.getGeneratorIndexFromRoot(rootSpace),
      inputOwner,
      ringUnit
    );
  }
  Coefficient getKillingFormProduct(
    const ElementUniversalEnveloping<Coefficient>& right
  ) const;
  void makeZero(SemisimpleLieAlgebra& inputOwner);
  bool isLieAlgebraElement(
    ElementSemisimpleLieAlgebra<Coefficient>& whichElement
  ) const;
  bool isLieAlgebraElementRational(
    ElementSemisimpleLieAlgebra<Rational>& whichElement
  ) const;
  void makeConstant(
    const Coefficient& coefficient, SemisimpleLieAlgebra& inputOwner
  ) {
    this->makeZero(inputOwner);
    MonomialUniversalEnveloping<Coefficient> tempMon;
    tempMon.makeOne(inputOwner);
    this->addMonomial(tempMon, coefficient);
  }
  void simplify(const Coefficient& ringUnit = Coefficient::one());
  int minimalNumberOfVariables() const {
    int result = 0;
    for (int i = 0; i < this->size; i ++) {
      result =
      MathRoutines::maximum(
        result, this->coefficients[i].minimalNumberOfVariables()
      );
      result =
      MathRoutines::maximum(
        result, (*this)[i].minimalNumberOfVariables()
      );
    }
    return result;
  }
  void multiplyBy(
    const MonomialUniversalEnveloping<Coefficient>& standsOnTheRight,
    const Coefficient& coefficient
  );
  void modToMinDegreeFormFDRels(
    const Vector<Rational>& highestWeightInSimpleCoordinates,
    const Coefficient& ringUnit,
    const Coefficient& ringZero
  );
  void modOutVermaRelations(
    const Vector<Coefficient>* substitutionHiGoesToIthElement,
    const Coefficient& ringUnit = Coefficient::one(),
    const Coefficient& ringZero = Coefficient::zero()
  );
  static void getCoordinateFormOfSpanOfElements(
    List<ElementUniversalEnveloping<Coefficient> >& elements,
    Vectors<Coefficient>& outputCoordinates,
    ElementUniversalEnveloping<Coefficient>& outputCorrespondingMonomials
  );
  bool getCoordinatesInBasis(
    List<ElementUniversalEnveloping<Coefficient> >& basis,
    Vector<Coefficient>& output,
    const Coefficient& ringUnit,
    const Coefficient& ringZero
  ) const;
  static inline unsigned int hashFunction(
    const ElementUniversalEnveloping<Coefficient>& input
  ) {
    return input.hashFunction();
  }
  unsigned int hashFunction() const {
    return
    this->::LinearCombination<
      MonomialUniversalEnveloping<Coefficient>, Coefficient
    >::hashFunction();
  }
  template <class CoefficientTypeQuotientField>
  static bool getBasisFromSpanOfElements(
    List<ElementUniversalEnveloping<Coefficient> >& elements,
    Vectors<CoefficientTypeQuotientField>& outputCoords,
    List<ElementUniversalEnveloping<Coefficient> >& outputBasis,
    const CoefficientTypeQuotientField& fieldUnit,
    const CoefficientTypeQuotientField& fieldZero
  );
  void assignFromCoordinateFormWithRespectToBasis(
    List<ElementUniversalEnveloping<Coefficient> >& basis,
    Vector<Coefficient>& input,
    SemisimpleLieAlgebra& owner
  );
  void raiseToPower(int power);
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
    PolynomialSubstitution<Rational>& polynomialSubstitution,
    const Coefficient& ringUnit,
    const Coefficient& ringZero
  );
  void substitution(
    const PolynomialSubstitution<Rational>& polynomialSubstitution
  );
  void makeCasimir(SemisimpleLieAlgebra& inputOwner);
  void makeCasimirWRTLeviParabolic(
    SemisimpleLieAlgebra& owner, const Selection& leviRoots
  );
  static void lieBracket(
    const ElementUniversalEnveloping<Coefficient>& left,
    const ElementUniversalEnveloping<Coefficient>& right,
    ElementUniversalEnveloping<Coefficient>& output
  ) {
    left.lieBracketOnTheRight(right, output);
    output.simplify();
  }
  void lieBracketOnTheRight(
    const ElementUniversalEnveloping<Coefficient>& right,
    ElementUniversalEnveloping<Coefficient>& output
  ) const;
  void lieBracketOnTheLeft(
    const ElementSemisimpleLieAlgebra<Rational>& left
  );
  void assignInteger(
    int coefficient, int numberOfVariables, SemisimpleLieAlgebra& owner
  ) {
    Rational coefficientRational = coefficient;
    this->makeConstant(coefficientRational, numberOfVariables, &owner);
  }
  SemisimpleLieAlgebra& getOwner() const {
    return *this->owner;
  }
  template <class otherType>
  void assign(const ElementUniversalEnveloping<otherType>& other) {
    this->owners = other.owners;
    this->indexInOwners = other.indexInOwners;
    MonomialUniversalEnveloping<Coefficient> tempMon;
    Coefficient coefficient;
    for (int i = 0; i < other.size; i ++) {
      tempMon = other[i];
      coefficient = other.coefficients[i];
      this->addMonomial(tempMon, coefficient);
    }
  }
  void operator=(const Rational& other) {
    this->makeConstant(other, 0, *this->owner);
  }
  void operator=(const ElementUniversalEnveloping<Coefficient>& other) {
    this->::LinearCombination<
      MonomialUniversalEnveloping<Coefficient>, Coefficient
    >::operator=(other);
    this->owner = other.owner;
  }
  void operator*=(
    const ElementUniversalEnveloping<Coefficient>& standsOnTheRight
  );
  void operator*=(const Coefficient& other) {
    this->::LinearCombination<
      MonomialUniversalEnveloping<Coefficient>, Coefficient
    >::operator*=(other);
  }
  template <class otherType>
  void operator/=(const otherType& other) {
    this->::LinearCombination<
      MonomialUniversalEnveloping<Coefficient>, Coefficient
    >::operator/=(other);
  }
  ElementUniversalEnveloping<Coefficient>(): owner(nullptr) {}
  ElementUniversalEnveloping<Coefficient>(
    const ElementUniversalEnveloping<Coefficient>& other
  ) {
    this->operator=(other);
  }
};

#endif // header_math_extra_universal_enveloping_ALREADY_INCLUDED
