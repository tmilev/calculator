#ifndef header_math_extra_universal_enveloping_ALREADY_INCLUDED
#define header_math_extra_universal_enveloping_ALREADY_INCLUDED

#include "math_extra_semisimple_lie_algebras.h"
#include "math_general.h"

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
    const MonomialUniversalEnveloping<Coefficient>&
    monomial
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
      generatorIndex < 0 ||
      generatorIndex > inputOwner.getNumberOfGenerators()) {
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
    std::stringstream* logStream =
    nullptr
  );
  void modOutVermaRelations(
    Coefficient& outputCoeff,
    const Vector<Coefficient>* substitutionHiGoesToIthElement = 0,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero =
    0
  );
  void setNumberOfVariables(int newNumberOfVariables);
  void substitute(const PolynomialSubstitution<Rational>& substitution);
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
  // The order of the positive roots is the same as the order in which roots
  // are
  // kept
  // in WeylGroup::RootSystem.
  // The "zero level roots" - i.e. the elements of the Cartan subalgebra lie in
  // between
  // the negative and positive rootss.
  void simplify(
    ElementUniversalEnveloping<Coefficient>& output,
    const Coefficient& ringUnit =
    1
  );
  void commuteABntoBnAPlusLowerOrder(
    int index,
    ElementUniversalEnveloping<Coefficient>& output,
    const Coefficient& ringUnit =
    1
  );
  void commuteAnBtoBAnPlusLowerOrder(
    int indexA,
    ElementUniversalEnveloping<Coefficient>& output,
    const Coefficient& ringUnit =
    1
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
    std::stringstream* logStream =
    nullptr
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
    std::stringstream* logStream =
    nullptr
  ) const;
  bool highestWeightTransposeAntiAutomorphismBilinearForm(
    const MonomialUniversalEnveloping<Coefficient>& right,
    Coefficient& output,
    const Vector<Coefficient>* substitutionHiGoesToIthElement,
    const Coefficient& ringUnit,
    const Coefficient& ringZero,
    std::stringstream* logStream =
    nullptr
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
    const Coefficient& ringUnit =
    1
  );
  void makeOneGenerator(
    int index, SemisimpleLieAlgebra& inputOwner, const Coefficient& ringUnit
  );
  void makeOneGeneratorCoefficientOne(
    int index,
    SemisimpleLieAlgebra& inputOwners,
    const Coefficient& ringUnit =
    1
  );
  void makeOneGeneratorCoefficientOne(
    int index, int numberOfVariables, SemisimpleLieAlgebra& inputOwner
  );
  void makeOneGeneratorCoefficientOne(
    const Vector<Rational>& rootSpace,
    SemisimpleLieAlgebra& inputOwner,
    const Coefficient& ringUnit =
    1
  ) {
    this->makeOneGeneratorCoefficientOne(
      inputOwner.getGeneratorIndexFromRoot(rootSpace), inputOwner, ringUnit
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
    MonomialUniversalEnveloping<Coefficient> monomial;
    monomial.makeOne(inputOwner);
    this->addMonomial(monomial, coefficient);
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
      MathRoutines::maximum(result, (*this)[i].minimalNumberOfVariables());
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
    Vectors<CoefficientTypeQuotientField>& outputCoordinates,
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
  void substituteInCoefficients(
    PolynomialSubstitution<Rational>& polynomialSubstitution,
    const Coefficient& ringUnit,
    const Coefficient& ringZero
  );
  void substitute(
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
  void lieBracketOnTheLeft(const ElementSemisimpleLieAlgebra<Rational>& left);
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
    MonomialUniversalEnveloping<Coefficient> monomial;
    Coefficient coefficient;
    for (int i = 0; i < other.size; i ++) {
      monomial = other[i];
      coefficient = other.coefficients[i];
      this->addMonomial(monomial, coefficient);
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

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::makeZero(
  SemisimpleLieAlgebra& inputOwner
) {
  this->::LinearCombination<
    MonomialUniversalEnveloping<Coefficient>, Coefficient
  >::makeZero();
  this->owner = &inputOwner;
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::substitute(
  const PolynomialSubstitution<Rational>& polynomialSubstitution
) {
  ElementUniversalEnveloping<Coefficient> output;
  output.makeZero(*this->owner);
  MonomialUniversalEnveloping<Coefficient> monomial;
  Coefficient tempCF;
  for (int i = 0; i < this->size(); i ++) {
    monomial = (*this)[i];
    monomial.substitute(polynomialSubstitution);
    tempCF = this->coefficients[i];
    tempCF.substitute(polynomialSubstitution, 1, nullptr);
    output.addMonomial(monomial, tempCF);
  }
  *this = output;
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::makeCasimirWRTLeviParabolic(
  SemisimpleLieAlgebra& owner, const Selection& leviRoots
) {
  STACK_TRACE("ElementUniversalEnveloping::makeCasimirWRTLeviParabolic");
  if (leviRoots.cardinalitySelection == 0) {
    this->makeZero(owner);
    return;
  }
  Coefficient result = 0;
  ElementSemisimpleLieAlgebra<Rational> leftE;
  ElementSemisimpleLieAlgebra<Rational> rightE;
  ChevalleyGenerator baseGen;
  Selection rootsNotInLEvi = leviRoots;
  rootsNotInLEvi.invertSelection();
  Vector<Rational> rootsNotInLeviVectorForm = rootsNotInLEvi;
  Vector<Rational> weightLeft, weightRight;
  this->makeZero(owner);
  MonomialUniversalEnveloping<Coefficient> monomial;
  Rational coefficient;
  // Coefficient coefficientconverted;
  for (int i = 0; i < owner.getNumberOfGenerators(); i ++) {
    weightLeft = owner.getWeightOfGenerator(i);
    if (weightLeft.scalarEuclidean(rootsNotInLeviVectorForm) != 0) {
      continue;
    }
    if (weightLeft.isEqualToZero()) {
      continue;
    }
    monomial.makeOne(owner);
    int indexOpposite = owner.getGeneratorIndexFromRoot(- weightLeft);
    monomial.generatorsIndices.addOnTop(i);
    monomial.generatorsIndices.addOnTop(indexOpposite);
    monomial.powers.addOnTop(1);
    monomial.powers.addOnTop(1);
    leftE.makeGenerator(i, owner);
    rightE.makeGenerator(indexOpposite, owner);
    coefficient =
    owner.getKillingFormProductWRTLevi(leftE, rightE, rootsNotInLEvi);
    coefficient.invert();
    this->addMonomial(monomial, coefficient);
  }
  Matrix<Rational> killingRestrictedToCartan;
  killingRestrictedToCartan.initialize(
    leviRoots.cardinalitySelection, leviRoots.cardinalitySelection
  );
  for (int i = 0; i < leviRoots.cardinalitySelection; i ++) {
    for (int j = i; j < leviRoots.cardinalitySelection; j ++) {
      weightLeft.makeEi(owner.getRank(), leviRoots.elements[i]);
      weightRight.makeEi(owner.getRank(), leviRoots.elements[j]);
      leftE.makeCartanGenerator(weightLeft, owner);
      rightE.makeCartanGenerator(weightRight, owner);
      killingRestrictedToCartan(i, j) =
      owner.getKillingFormProductWRTLevi(leftE, rightE, rootsNotInLEvi);
      killingRestrictedToCartan(j, i) = killingRestrictedToCartan(i, j);
    }
  }
  killingRestrictedToCartan.invert();
  ElementUniversalEnveloping<Coefficient> leftUE, rightUE;
  Vector<Rational> currentEj;
  for (int i = 0; i < leviRoots.cardinalitySelection; i ++) {
    weightLeft.makeEi(owner.getRank(), leviRoots.elements[i]);
    weightRight.makeZero(owner.getRank());
    for (int j = 0; j < leviRoots.cardinalitySelection; j ++) {
      currentEj.makeEi(owner.getRank(), leviRoots.elements[j]);
      weightRight += currentEj * killingRestrictedToCartan(i, j);
    }
    leftUE.makeCartanGenerator(weightLeft, owner);
    rightUE.makeCartanGenerator(weightRight, owner);
    leftUE *= rightUE;
    *this += leftUE;
  }
  this->simplify();
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::modOutVermaRelations(
  const Vector<Coefficient>* substitutionHiGoesToIthElement,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  MonomialUniversalEnveloping<Coefficient> monomial;
  ElementUniversalEnveloping<Coefficient> output;
  output.makeZero(*this->owner);
  Coefficient acquiredCoefficient;
  for (int i = 0; i < this->size(); i ++) {
    monomial = (*this)[i];
    monomial.modOutVermaRelations(
      acquiredCoefficient, substitutionHiGoesToIthElement, ringUnit, ringZero
    );
    acquiredCoefficient *= this->coefficients[i];
    output.addMonomial(monomial, acquiredCoefficient);
  }
  this->operator=(output);
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::lieBracketOnTheLeft(
  const ElementSemisimpleLieAlgebra<Rational>& left
) {
  if (this->isEqualToZero()) {
    this->makeZero(*this->owner);
    return;
  }
  ElementUniversalEnveloping<Coefficient> element1, element2;
  element1.assignElementLieAlgebra(
    left, *this->owner, this->coefficients[0].one()
  );
  element2 = *this;
  element2.lieBracketOnTheRight(element1, *this);
}

template <class Coefficient>
bool MonomialUniversalEnveloping<Coefficient>::adjointRepresentationAction(
  const ElementUniversalEnveloping<Coefficient>& input,
  ElementUniversalEnveloping<Coefficient>& output
) const {
  output.makeZero(*this->owner);
  ElementSemisimpleLieAlgebra<Rational> element;
  output = input;
  for (int i = this->generatorsIndices.size - 1; i >= 0; i --) {
    int nextCycleSize;
    if (!this->powers[i].isSmallInteger(&nextCycleSize)) {
      return false;
    }
    element.makeGenerator(this->generatorsIndices[i], *this->owner);
    for (int j = 0; j < nextCycleSize; j ++) {
      output.lieBracketOnTheLeft(element);
    }
    output.simplify();
  }
  return true;
}

template <class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::adjointRepresentationAction(
  const ElementUniversalEnveloping<Coefficient>& input,
  ElementUniversalEnveloping<Coefficient>& output
) const {
  if (&input == &output) {
    global.fatal
    << "Input not allowed to coincide with output. "
    << global.fatal;
  }
  output.makeZero(*this->owner);
  ElementUniversalEnveloping<Coefficient> summand;
  for (int i = 0; i < this->size(); i ++) {
    if (!(*this)[i].adjointRepresentationAction(input, summand)) {
      return false;
    }
    summand *= this->coefficients[i];
    output += summand;
  }
  return true;
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::simplify(
  const Coefficient& ringUnit
) {
  ElementUniversalEnveloping<Coefficient> buffer;
  ElementUniversalEnveloping<Coefficient> outpuT;
  MonomialUniversalEnveloping<Coefficient> monomial;
  Coefficient currentCoefficient;
  outpuT.makeZero(*this->owner);
  for (; this->size() > 0;) {
    this->popMonomial(this->size() - 1, monomial, currentCoefficient);
    bool reductionOccurred = false;
    for (int i = 0; i < monomial.generatorsIndices.size - 1; i ++) {
      if (
        !this->getOwner().areOrderedProperly(
          monomial.generatorsIndices[i], monomial.generatorsIndices[i + 1]
        )
      ) {
        if (monomial.switchConsecutiveIndicesIfTheyCommute(i)) {
          this->addMonomial(monomial, currentCoefficient);
          reductionOccurred = true;
          break;
        }
        if (
          monomial.commutingAnBtoBAnPlusLowerOrderAllowed(
            monomial.powers[i],
            monomial.generatorsIndices[i],
            monomial.powers[i + 1],
            monomial.generatorsIndices[i + 1]
          )
        ) {
          monomial.commuteAnBtoBAnPlusLowerOrder(i, buffer, ringUnit);
          buffer *= currentCoefficient;
          *this += buffer;
          reductionOccurred = true;
          break;
        }
        if (
          monomial.commutingABntoBnAPlusLowerOrderAllowed(
            monomial.powers[i],
            monomial.generatorsIndices[i],
            monomial.powers[i + 1],
            monomial.generatorsIndices[i + 1]
          )
        ) {
          monomial.commuteABntoBnAPlusLowerOrder(i, buffer, ringUnit);
          buffer *= currentCoefficient;
          *this += buffer;
          reductionOccurred = true;
          break;
        }
      }
    }
    if (!reductionOccurred) {
      outpuT.addMonomial(monomial, currentCoefficient);
    }
  }
  *this = outpuT;
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::substituteInCoefficients(
  PolynomialSubstitution<Rational>& polynomialSubstitution,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  ElementUniversalEnveloping<Coefficient> endResult;
  MonomialUniversalEnveloping<Coefficient> monomial;
  endResult.makeZero(*this->owner);
  Coefficient tempCF;
  for (int i = 0; i < this->size; i ++) {
    monomial = this->objects[i];
    this->coefficients[i].substitute(polynomialSubstitution);
    endResult.addMonomial(monomial, tempCF);
  }
  endResult.simplify(ringUnit, ringZero);
  this->operator=(endResult);
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::lieBracketOnTheRight(
  const ElementUniversalEnveloping<Coefficient>& right,
  ElementUniversalEnveloping<Coefficient>& output
) const {
  ElementUniversalEnveloping<Coefficient> element, element2;
  element = *this;
  element *= right;
  element2 = right;
  element2 *= *this;
  output = element;
  output -= element2;
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::cleanUpZeroCoefficient() {
  for (int i = 0; i < this->size; i ++) {
    if ((*this)[i].coefficient.isEqualToZero()) {
      this->removeIndexSwapWithLast(i);
      i --;
    }
  }
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::makeOneGenerator(
  int index, SemisimpleLieAlgebra& inputOwner, const Coefficient& ringUnit
) {
  this->makeZero(inputOwner);
  MonomialUniversalEnveloping<Coefficient> result;
  result.makeOne(inputOwner);
  result.multiplyByGeneratorPowerOnTheRight(index, ringUnit);
  this->addMonomial(result, ringUnit);
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::makeOneGeneratorCoefficientOne(
  int index, SemisimpleLieAlgebra& inputOwner, const Coefficient& ringUnit
) {
  this->makeZero(inputOwner);
  MonomialUniversalEnveloping<Coefficient> result;
  result.makeOne(inputOwner);
  result.multiplyByGeneratorPowerOnTheRight(index, ringUnit);
  this->addMonomial(result, ringUnit);
}

template <class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::isLieAlgebraElement(
  ElementSemisimpleLieAlgebra<Coefficient>& whichElement
) const {
  whichElement.makeZero();
  Coefficient exponent;
  ChevalleyGenerator chevalleyGenerator;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialUniversalEnveloping<Coefficient>& monomial = (*this)[i];
    monomial.getDegree(exponent);
    if (!exponent.isEqualToOne()) {
      return false;
    }
    chevalleyGenerator.makeGenerator(
      *this->owner, monomial.generatorsIndices[0]
    );
    whichElement.addMonomial(chevalleyGenerator, this->coefficients[i]);
  }
  return true;
}

template <class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::isLieAlgebraElementRational(
  ElementSemisimpleLieAlgebra<Rational>& whichElement
) const {
  whichElement.makeZero();
  Coefficient exponent;
  ChevalleyGenerator chevalleyGenerator;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialUniversalEnveloping<Coefficient>& monomial = (*this)[i];
    monomial.getDegree(exponent);
    if (!exponent.isEqualToOne()) {
      return false;
    }
    Rational coefficient;
    if (!this->coefficients[i].isConstant(&coefficient)) {
      return false;
    }
    chevalleyGenerator.makeGenerator(
      *this->owner, monomial.generatorsIndices[0]
    );
    whichElement.addMonomial(chevalleyGenerator, coefficient);
  }
  return true;
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::assignElementLieAlgebra(
  const ElementSemisimpleLieAlgebra<Rational>& input,
  SemisimpleLieAlgebra& inputOwner,
  const Coefficient& ringUnit
) {
  this->makeZero(inputOwner);
  MonomialUniversalEnveloping<Coefficient> monomial;
  monomial.makeOne(inputOwner);
  monomial.generatorsIndices.setSize(1);
  monomial.powers.setSize(1);
  monomial.powers[0] = ringUnit;
  Coefficient tempCF;
  for (int i = 0; i < input.size(); i ++) {
    tempCF = ringUnit;
    // <- to facilitate implicit type conversion: ringUnit does not have to be
    // of type Rational
    tempCF *= input.coefficients[i];
    // <- to facilitate implicit type conversion: ringUnit does not have to be
    // of type Rational
    monomial.generatorsIndices[0] = input[i].generatorIndex;
    this->addMonomial(monomial, tempCF);
  }
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::getCoordinateFormOfSpanOfElements
(
  List<ElementUniversalEnveloping<Coefficient> >& elements,
  Vectors<Coefficient>& outputCoordinates,
  ElementUniversalEnveloping<Coefficient>& outputCorrespondingMonomials
) {
  if (elements.size == 0) {
    return;
  }
  outputCorrespondingMonomials.makeZero(
    *elements[0].owners, elements[0].indexInOwners
  );
  for (int i = 0; i < elements.size; i ++) {
    for (int j = 0; j < elements[i].size; j ++) {
      outputCorrespondingMonomials.addOnTopNoRepetition(elements[i][j]);
    }
  }
  outputCoordinates.setSize(elements.size);
  Polynomial<Rational> zeroPoly;
  zeroPoly.makeZero();
  for (int i = 0; i < elements.size; i ++) {
    Vector<Coefficient>& current = outputCoordinates[i];
    current.initializeFillInObject(
      outputCorrespondingMonomials.size, zeroPoly
    );
    ElementUniversalEnveloping& currentElement = elements[i];
    for (int j = 0; j < currentElement.size; j ++) {
      MonomialUniversalEnveloping<Coefficient>& monomial = currentElement[j];
      current[outputCorrespondingMonomials.getIndex(monomial)] =
      currentElement.coefficients[j];
    }
  }
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::makeCartanGenerator(
  const Vector<Rational>& input, SemisimpleLieAlgebra& inputOwner
) {
  MonomialUniversalEnveloping<Coefficient> monomial;
  this->makeZero(inputOwner);
  monomial.makeOne(inputOwner);
  int dimension = this->getOwner().weylGroup.cartanSymmetric.numberOfRows;
  int totalPositiveRoots = this->getOwner().weylGroup.rootsOfBorel.size;
  monomial.generatorsIndices.setSize(1);
  monomial.powers.setSize(1);
  Coefficient tempCF;
  for (int i = 0; i < dimension; i ++) {
    if (!input[i].isEqualToZero()) {
      (*monomial.generatorsIndices.lastObject()) = i + totalPositiveRoots;
      *monomial.powers.lastObject() = 1;
      tempCF = 1;
      // <- to facilitate type conversion
      tempCF *= input[i];
      // <- to facilitate type conversion we call extra assignment
      this->addMonomial(monomial, tempCF);
    }
  }
}

template <class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::applyTransposeAntiAutoOnMe() {
  MonomialUniversalEnveloping<Coefficient> monomial;
  ElementUniversalEnveloping<Coefficient> result;
  result.makeZero(*this->owner);
  int totalPositiveRoots = this->getOwner().getNumberOfPositiveRoots();
  int rank = this->getOwner().getRank();
  Coefficient coefficient;
  this->checkNumberOfCoefficientsConsistency();
  for (int i = 0; i < this->size(); i ++) {
    const MonomialUniversalEnveloping<Coefficient>& startingMonomial = (
      *this
    )[i];
    coefficient = this->coefficients[i];
    monomial.owner = startingMonomial.owner;
    monomial.powers.size = 0;
    monomial.generatorsIndices.size = 0;
    for (int j = startingMonomial.powers.size - 1; j >= 0; j --) {
      int power;
      if (!startingMonomial.powers[j].isSmallInteger(&power)) {
        return false;
      }
      int generatorIndex = startingMonomial.generatorsIndices[j];
      if (generatorIndex < totalPositiveRoots) {
        generatorIndex = 2 * totalPositiveRoots + rank - 1 - generatorIndex;
      } else if (generatorIndex >= totalPositiveRoots + rank) {
        generatorIndex = - generatorIndex + 2 * totalPositiveRoots + rank - 1;
      }
      monomial.multiplyByGeneratorPowerOnTheRight(
        generatorIndex, startingMonomial.powers[j]
      );
    }
    result.addMonomial(monomial, coefficient);
  }
  *this = result;
  return true;
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::multiplyBy(
  const MonomialUniversalEnveloping<Coefficient>& standsOnTheRight,
  const Coefficient& coefficient
) {
  if (standsOnTheRight.generatorsIndices.size == 0) {
    return;
  }
  ElementUniversalEnveloping<Coefficient> output;
  output.setExpectedSize(this->size);
  output.makeZero(*this->owner);
  MonomialUniversalEnveloping<Coefficient> monomial;
  Coefficient newCoefficient;
  for (int i = 0; i < this->size; i ++) {
    monomial = (*this)[i];
    monomial *= standsOnTheRight;
    newCoefficient = this->coefficients[i];
    newCoefficient *= coefficient;
    output.addMonomial(monomial, newCoefficient);
  }
  *this = output;
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::operator*=(
  const ElementUniversalEnveloping& standsOnTheRight
) {
  this->::ElementMonomialAlgebra<
    MonomialUniversalEnveloping<Coefficient>, Coefficient
  >::operator*=(
    static_cast<
      ElementMonomialAlgebra<
        MonomialUniversalEnveloping<Coefficient>, Coefficient
      >
    >(standsOnTheRight)
  );
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::raiseToPower(int power) {
  ElementUniversalEnveloping<Coefficient> buffer;
  buffer = *this;
  if (this->size() == 0) {
    return;
  }
  this->makeConstant(this->coefficients[0].one(), *this->owner);
  for (int i = 0; i < power; i ++) {
    this->operator*=(buffer);
  }
}

template <class Coefficient>
Coefficient ElementUniversalEnveloping<Coefficient>::getKillingFormProduct(
  const ElementUniversalEnveloping<Coefficient>& right
) const {
  STACK_TRACE("ElementUniversalEnveloping::getKillingFormProduct");
  if (this->isEqualToZero()) {
    return 0;
  }
  Coefficient result = 0;
  ElementUniversalEnveloping<Coefficient> adadAppliedToMon, element;
  SemisimpleLieAlgebra* owner = &this->getOwner();
  MonomialUniversalEnveloping<Coefficient> baseGen;
  for (int i = 0; i < owner->getNumberOfGenerators(); i ++) {
    baseGen.makeGenerator(i, *owner);
    adadAppliedToMon.makeZero(*owner);
    adadAppliedToMon.addMonomial(baseGen, 1);
    right.adjointRepresentationAction(adadAppliedToMon, element);
    element.simplify(Coefficient::one());
    this->adjointRepresentationAction(element, adadAppliedToMon);
    adadAppliedToMon.simplify(Coefficient::one());
    result += adadAppliedToMon.getCoefficientOf(baseGen);
  }
  return result;
}

template <class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::
highestWeightTransposeAntiAutomorphismBilinearForm(
  const ElementUniversalEnveloping<Coefficient>& right,
  Coefficient& output,
  const Vector<Coefficient>* substitutionHiGoesToIthElement,
  const Coefficient& ringUnit,
  const Coefficient& ringZero,
  std::stringstream* logStream
) const {
  output = ringZero;
  ElementUniversalEnveloping<Coefficient> taLeft;
  taLeft = *this;
  if (!taLeft.applyTransposeAntiAutoOnMe()) {
    return false;
  }
  ElementUniversalEnveloping<Coefficient> accumulator;
  ElementUniversalEnveloping<Coefficient> intermediate;
  ElementUniversalEnveloping<Coefficient> element;
  ElementUniversalEnveloping<Coefficient> startingElement;
  List<int> oldOrder = this->getOwner().universalEnvelopingGeneratorOrder;
  int numberOfPositiveRoots = this->getOwner().getNumberOfPositiveRoots();
  for (int i = 0; i < numberOfPositiveRoots; i ++) {
    this->getOwner().universalEnvelopingGeneratorOrder[i] = - 1;
  }
  accumulator.makeZero(this->getOwner());
  MonomialUniversalEnveloping<Coefficient> constMon;
  constMon.makeOne(this->getOwner());
  if (logStream != nullptr) {
    *logStream
    << "left element transposed: "
    << taLeft.toString(&global.defaultFormat.getElement())
    << "<br>";
    *logStream
    << "right element: "
    << right.toString(&global.defaultFormat.getElement())
    << "<br>";
  }
  startingElement = right;
  startingElement.simplify(ringUnit);
  if (logStream != nullptr) {
    *logStream
    << "right element after simplification: "
    << startingElement.toString(&global.defaultFormat.getElement())
    << "<br>";
  }
  startingElement.modOutVermaRelations(
    substitutionHiGoesToIthElement, ringUnit
  );
  if (logStream != nullptr) {
    *logStream
    << "right element after Verma rels: "
    << startingElement.toString(&global.defaultFormat.getElement())
    << "<br>";
  }
  Coefficient leftMonomialCoefficient;
  for (int j = 0; j < taLeft.size(); j ++) {
    intermediate = startingElement;
    const MonomialUniversalEnveloping<Coefficient>& leftMonomial = taLeft[j];
    leftMonomialCoefficient = taLeft.coefficients[j];
    int power;
    for (int i = leftMonomial.powers.size - 1; i >= 0; i --) {
      if (leftMonomial.powers[i].isSmallInteger(&power)) {
        for (int k = 0; k < power; k ++) {
          element.makeOneGenerator(
            leftMonomial.generatorsIndices[i], this->getOwner(), ringUnit
          );
          MathRoutines::swap(element, intermediate);
          if (logStream != nullptr) {
            *logStream
            << "intermediate before mult: "
            << intermediate.toString(&global.defaultFormat.getElement())
            << "<br>";
          }
          intermediate *= element;
          if (logStream != nullptr) {
            *logStream
            << "intermediate before simplification: "
            << intermediate.toString(&global.defaultFormat.getElement())
            << "<br>";
          }
          intermediate.simplify(ringUnit);
          if (logStream != nullptr) {
            *logStream
            << "intermediate after simplification: "
            << intermediate.toString(&global.defaultFormat.getElement())
            << "<br>";
          }
          intermediate.modOutVermaRelations(
            substitutionHiGoesToIthElement, ringUnit, ringZero
          );
          if (logStream != nullptr) {
            *logStream
            << "intermediate after Verma rels: "
            << intermediate.toString(&global.defaultFormat.getElement())
            << "<br>";
          }
        }
      } else {
        this->getOwner().universalEnvelopingGeneratorOrder = oldOrder;
        return false;
      }
    }
    intermediate *= leftMonomialCoefficient;
    accumulator += intermediate;
    int index = intermediate.monomials.getIndex(constMon);
    if (index != - 1) {
      output += intermediate.coefficients[index];
    }
  }
  if (logStream != nullptr) {
    *logStream
    << "final UE element: "
    << accumulator.toString(&global.defaultFormat.getElement());
  }
  this->getOwner().universalEnvelopingGeneratorOrder = oldOrder;
  return true;
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::makeCasimir(
  SemisimpleLieAlgebra& inputOwner
) {
  // std::stringstream out;
  this->makeZero(inputOwner);
  WeylGroupData& weylGroup = this->getOwner().weylGroup;
  int dimension = weylGroup.cartanSymmetric.numberOfRows;
  Vector<Rational> root1;
  Vector<Rational> root2;
  //  Matrix<Rational> killingForm;
  //  killingForm.initialize(dimension, dimension);
  //  for (int i = 0; i < dimension; i ++)
  //  { root1.makeEi(dimension, i);
  //    for (int j = 0; j < dimension; j ++)
  //    { killingForm.elements[i][j] = 0;
  //      root2.makeEi(dimension, j);
  //      for (int k = 0; k < weylGroup.RootSystem.size; k++)
  // killingForm.elements[i][j] += weylGroup.rootScalarCartanRoot(root1,
  // weylGroup.RootSystem.objects[k])* weylGroup.rootScalarCartanRoot(root2,
  // weylGroup.RootSystem.objects[k]);
  //    }
  //  }
  //  killingForm.invert(global);
  //  out << killingForm.toString(true, false);
  ElementUniversalEnveloping<Coefficient> element1, element2;
  // this code is to check a math formula:
  //  ElementUniversalEnveloping checkElement;
  //  checkElement.makeZero(owner);
  Matrix<Rational> invertedSymCartan;
  invertedSymCartan = weylGroup.cartanSymmetric;
  invertedSymCartan.invert();
  for (int i = 0; i < dimension; i ++) {
    root1.makeEi(dimension, i);
    // implementation without the ninja formula:
    //    killingForm.actOnVectorColumn(root1, root2);
    //    element1.makeCartanGenerator(root1, vars, owner);
    //    element2.makeCartanGenerator(root2, vars, owner);
    //    element1*= element2;
    //    *this+= element1;
    // Alternative implementation using a ninja formula I cooked up after
    // looking at the printouts:
    invertedSymCartan.actOnVectorColumn(root1, root2);
    element1.makeCartanGenerator(root1, inputOwner);
    element2.makeCartanGenerator(root2, inputOwner);
    element1 *= element2;
    *this += element1;
  }
  // Rational rational;
  for (int i = 0; i < weylGroup.rootSystem.size; i ++) {
    // Implementation without the ninja formula:
    //    rational = 0;
    //    Vector<Rational> & root = weylGroup.RootSystem.objects[i];
    //    int indexOfOpposite = weylGroup.RootSystem.getIndex(-root);
    // Vector<Rational> & opposite =
    // weylGroup.RootSystem.objects[indexOfOpposite];
    //    for (int j = 0; j < weylGroup.RootSystem.size; j ++)
    //    { Vector<Rational> & current = weylGroup.RootSystem.objects[j];
    //      if (current == opposite)
    //        rational +=2;
    //       else
    //       { int indexOfSum= weylGroup.RootSystem.getIndex(current +root);
    //         if (indexOfSum!= - 1)
    // rational
    // +=(owner.ChevalleyConstants.elements[i][j]*owner.ChevalleyConstants.elements[indexOfOpposite][indexOfSum]);
    //       }
    //     }
    //     rational +=2;
    //     rational = 1/rational;
    //     element2.makeOneGeneratorCoefficientOne(opposite, vars, owner);
    //     element1.makeOneGeneratorCoefficientOne(root, vars, owner);
    //     element2*= element1;
    //
    //     element2*= rational;
    //     *this+= element2;
    // The ninja formula alternative implementation:
    const Vector<Rational>& root = weylGroup.rootSystem[i];
    element2.makeOneGeneratorCoefficientOne(- root, inputOwner);
    element1.makeOneGeneratorCoefficientOne(root, inputOwner);
    element2 *= element1;
    element2 *=
    weylGroup.rootScalarCartanRoot(
      weylGroup.rootSystem[i], weylGroup.rootSystem[i]
    ) /
    2;
    *this += element2;
  }
  *this /= weylGroup.getKillingDividedByTraceRatio();
  // check that the ninja formula is correct:
  // this->DebugString= out.str();
  //  Vector<Rational> root;
  //  for (int i = 0; i < dimension; i ++)
  //  { root.makeEi(dimension, i);
  //    if (!length1Explored)
  //    { length1= weylGroup.rootScalarCartanRoot(root, root);
  //      length1Explored = true;
  //      coefficient1= 0;
  //      for (int j = 0; j < weylGroup.rootsOfBorel.size; j ++)
  // { coefficient1+= weylGroup.rootScalarCartanRoot(root,
  // weylGroup.rootsOfBorel.objects[j])*weylGroup.rootScalarCartanRoot(root,
  // weylGroup.rootsOfBorel.objects[j]);
  //        coef
  //      }
  //    }
  //  }
  this->simplify(0);
}

template <class Coefficient>
void MonomialUniversalEnveloping<Coefficient>::substitute(
  const PolynomialSubstitution<Rational>& substitution
) {
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    this->powers[i].substitute(substitution, 1, nullptr);
  }
  this->simplifyEqualConsecutiveGenerators(0);
}

#endif // header_math_extra_universal_enveloping_ALREADY_INCLUDED
