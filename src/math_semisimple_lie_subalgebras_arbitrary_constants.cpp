#include "general_lists.h"
#include "math_extra_semisimple_lie_subalgebras.h"
#include "math_general_polynomial_computations_basic_implementation.h" // IWYU pragma: keep; operator overloading requires this.

class CandidateSemisimpleSubalgebraArbitraryConstants {
public:
  char dynkinType;
  int rank;
  CandidateSemisimpleSubalgebra& output;
  bool loadBuiltInGeneratorHintsE8();
  bool loadBuiltInGeneratorHintsE6();
  bool loadBuiltInGeneratorHints();
  CandidateSemisimpleSubalgebraArbitraryConstants(
    CandidateSemisimpleSubalgebra& outputSubalgebra
  ):
  output(outputSubalgebra) {
    this->dynkinType = 0;
    this->rank = 0;
  }
  // Constructs a chevalley generator of the ambient Lie algebra with
  // the given display index.
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > g(
    int displayIndex
  );
  void configurePolynomialSystem();
  void configurePolynomialSystemE8();
  // Makes a polynomial of the form x_{display_index}.
  Polynomial<AlgebraicNumber> x(int displayIndex) {
    Polynomial<AlgebraicNumber> result;
    result.makeMonomial(
      displayIndex - 1, 1, this->output.owner->ownerField->one()
    );
    return result;
  }
  // Makes a dynkin type A.
  DynkinType A(int dynkinIndex, int rank) const;
  // Makes a dynkin type B.
  DynkinType B(int dynkinIndex, int rank) const;
  // Makes dynkin type E.
  DynkinType E(int dynkinIndex, int rank) const;
};

void CandidateSemisimpleSubalgebra::configurePolynomialSystem() {
  CandidateSemisimpleSubalgebraArbitraryConstants constants(*this);
  return constants.configurePolynomialSystem();
}

bool CandidateSemisimpleSubalgebra::loadBuiltInPartialRealization() {
  CandidateSemisimpleSubalgebraArbitraryConstants constants(*this);
  return constants.loadBuiltInGeneratorHints();
}

void CandidateSemisimpleSubalgebraArbitraryConstants::configurePolynomialSystem
() {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebraArbitraryConstants::"
    "configurePolynomialSystem"
  );
  int maximumPolynomialDivisions = 1000;
  int maximumMonomialOperations = 10000;
  this->output.configuredSystemToSolve.algebraicClosure =
  this->output.owner->ownerField;
  std::string ambientLieAlgebraName =
  this->output.owner->owner->toStringLieAlgebraName();
  if (ambientLieAlgebraName == "C^{1}_5") {
    // Known to run slow in this type.
    maximumPolynomialDivisions = 100;
    maximumMonomialOperations = 2000;
  }
  DynkinType& embeddedType = this->output.weylNonEmbedded->dynkinType;
  DynkinType& ambientType = this->output.owner->owner->weylGroup.dynkinType;
  if (ambientType == E(1, 8)) {
    this->configurePolynomialSystemE8();
    return;
  }
  this->output.configuredSystemToSolve.substitutionsProvider.
  oneIsFirstWhenRecursionDepthIsMultipleOf =
  3;
  if (embeddedType.getRank() == 1) {
    maximumPolynomialDivisions = 2000;
    maximumMonomialOperations = 10000;
    this->output.configuredSystemToSolve.substitutionsProvider.
    flagChooseSmallestIndexVariableFirst =
    true;
  }
  std::string embeddingLieAlgebraName = embeddedType.toString();
  if (
    embeddingLieAlgebraName == "B^{3}_2" && ambientLieAlgebraName == "E^{1}_6"
  ) {
    this->output.configuredSystemToSolve.substitutionsProvider.
    oneIsFirstWhenRecursionDepthIsMultipleOf =
    1;
    this->output.configuredSystemToSolve.hintEquations.addOnTop(x(1) - 1);
    // System known to be contradictory but takes extra computational time.
    maximumPolynomialDivisions = 1000;
    maximumMonomialOperations = 20000;
  }
  this->output.configuredSystemToSolve.groebner.maximumMonomialOperations =
  maximumMonomialOperations;
  this->output.configuredSystemToSolve.groebner.maximumPolynomialDivisions =
  maximumPolynomialDivisions;
}

void CandidateSemisimpleSubalgebraArbitraryConstants::
configurePolynomialSystemE8() {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebraArbitraryConstants::"
    "configurePolynomialSystemE8"
  );
  this->output.configuredSystemToSolve.algebraicClosure =
  this->output.owner->ownerField;
  int& maximumPolynomialDivisions =
  this->output.configuredSystemToSolve.groebner.maximumPolynomialDivisions;
  int& maximumMonomialOperations =
  this->output.configuredSystemToSolve.groebner.maximumMonomialOperations;
  maximumMonomialOperations = 10000;
  maximumPolynomialDivisions = 1000;
  DynkinType& embeddedType = this->output.weylNonEmbedded->dynkinType;
  this->output.configuredSystemToSolve.substitutionsProvider.
  oneIsFirstWhenRecursionDepthIsMultipleOf =
  3;
  if (embeddedType.getRank() == 1) {
    maximumPolynomialDivisions = 2000;
    maximumMonomialOperations = 10000;
    this->output.configuredSystemToSolve.substitutionsProvider.
    flagChooseSmallestIndexVariableFirst =
    true;
  }
  std::string embeddingLieAlgebraName = embeddedType.toString();
  if (embeddedType == A(20, 1) + A(4, 1)) {
    // System known to be contradictory but takes extra computational time.
    maximumPolynomialDivisions = 2000;
    maximumMonomialOperations = 10000;
  }
  if (embeddedType == A(16, 1) + A(8, 1)) {
    maximumPolynomialDivisions = 200;
    maximumMonomialOperations = 400;
  }
  if (embeddedType == A(84, 1) + A(4, 1)) {
    maximumPolynomialDivisions = 1000;
    maximumMonomialOperations = 10000;
    this->output.configuredSystemToSolve.substitutionsProvider.
    oneIsFirstWhenRecursionDepthIsMultipleOf =
    0;
  }
  if (embeddedType == A(28, 1) + A(3, 1) + A(3, 1)) {
    // System known to be contradictory.
    maximumPolynomialDivisions = 10000;
    maximumMonomialOperations = 40000;
    this->output.configuredSystemToSolve.substitutionsProvider.
    oneIsFirstWhenRecursionDepthIsMultipleOf =
    0;
  }
  if (embeddedType == A(36, 1) + A(4, 1)) {
    this->output.configuredSystemToSolve.substitutionsProvider.
    oneIsFirstWhenRecursionDepthIsMultipleOf =
    2;
  }
  if (embeddedType == A(22, 1) + A(2, 1)) {
    // Has multiple h candidates, some of them do not yield solutions.
    maximumPolynomialDivisions = 10000;
    maximumMonomialOperations = 40000;
  }
  if (
    embeddedType == A(14, 1) + A(2, 1) &&
    this->output.cartanElementsSubalgebra[1] == List<Rational>(
      {0, 1, 1, 2, 1, 0, 1, 1}
    )
  ) {
    // System known to be contradictory.
    maximumPolynomialDivisions = 40000;
    maximumMonomialOperations = 40000;
  }
}

bool CandidateSemisimpleSubalgebraArbitraryConstants::loadBuiltInGeneratorHints
() {
  if (
    !this->output.getAmbientWeyl().dynkinType.isSimple(
      &this->dynkinType, &this->rank
    )
  ) {
    return false;
  }
  if (dynkinType == 'E' && rank == 6) {
    return
    CandidateSemisimpleSubalgebraArbitraryConstants::
    loadBuiltInGeneratorHintsE6();
  }
  if (dynkinType == 'E' && rank == 8) {
    return
    CandidateSemisimpleSubalgebraArbitraryConstants::
    loadBuiltInGeneratorHintsE8();
  }
  return false;
}

bool CandidateSemisimpleSubalgebraArbitraryConstants::
loadBuiltInGeneratorHintsE6() {
  std::string typeToBeRealized =
  this->output.weylNonEmbedded->dynkinType.toString();
  if (typeToBeRealized == "A^{36}_1") {
    this->output.unknownNegativeGenerators[0] =
    g(- 6) + g(- 7) + g(- 8) + g(- 9) + g(- 10) + g(- 19);
    return true;
  }
  if (typeToBeRealized == "A^{8}_1+A^{4}_1") {
    this->output.unknownNegativeGenerators[0] =
    g(- 12) + g(- 18) + g(- 20) + g(- 21) + g(- 22);
    this->output.unknownNegativeGenerators[1] = g(- 10) + g(- 13) + g(- 19);
  }
  return false;
}

bool CandidateSemisimpleSubalgebraArbitraryConstants::
loadBuiltInGeneratorHintsE8() {
  DynkinType& type = this->output.weylNonEmbedded->dynkinType;
  if (type == A(14, 1)) {
    this->output.unknownNegativeGenerators.clear();
    this->output.unknownNegativeGenerators.addOnTop(
      g(- 45) + g(- 47) + g(- 49) + g(- 57) + g(- 60) + g(- 70) + g(- 82)
    );
    return true;
  }
  if (
    type == A(84, 1) + A(4, 1) &&
    this->output.cartanElementsSubalgebra[0].isEqualTo(
      List<Rational>({24, 35, 47, 70, 57, 44, 30, 15})
    )
  ) {
    this->output.unknownNegativeGenerators[0] =
    g(- 21) + g(- 22) + g(- 23) * 12 + g(- 24) + g(- 26) + g(- 27) + g(- 31);
    return true;
  }
  if (
    type == A(62, 1) + A(2, 1) &&
    this->output.cartanElementsSubalgebra[1].isEqualTo(
      List<Rational>({0, 0, 0, 1, 2, 1, 0, 0})
    )
  ) {
    this->output.unknownNegativeGenerators[0] =
    g(- 8) +
    g(- 25) +
    g(- 30) +
    g(- 31) +
    g(- 34) +
    g(- 37) +
    g(- 38) +
    g(- 39) +
    g(- 40) +
    g(- 45);
    this->output.unknownNegativeGenerators[1] = - g(- 5) + g(- 20);
    return true;
  }
  if (
    type == A(62, 1) + A(1, 1) &&
    this->output.cartanElementsSubalgebra[1].isEqualTo(
      List<Rational>({0, 1, 1, 2, 2, 1, 0, 0})
    )
  ) {
    this->output.unknownNegativeGenerators[0] =
    g(- 8) +
    g(- 21) +
    g(- 28) +
    g(- 32) +
    g(- 35) +
    g(- 37) +
    g(- 38) +
    g(- 41) +
    g(- 44) +
    g(- 45) *(- 9);
    this->output.unknownNegativeGenerators[1] = g(- 46);
    return true;
  }
  if (
    type == A(60, 1) + A(1, 1) &&
    this->output.cartanElementsSubalgebra[1].isEqualTo(
      List<Rational>({0, 1, 1, 2, 2, 1, 0, 0})
    )
  ) {
    this->output.unknownNegativeGenerators[0] =
    g(- 8) + g(- 34) + g(- 35) + g(- 37) + g(- 38);
    this->output.unknownNegativeGenerators[1] = g(- 46);
    return true;
  }
  if (
    type == A(38, 1) + A(1, 1) &&
    this->output.cartanElementsSubalgebra[1].isEqualTo(
      List<Rational>({0, 0, 0, 1, 1, 1, 0, 0})
    )
  ) {
    this->output.unknownNegativeGenerators[0] =
    g(- 31) +
    g(- 33) +
    g(- 37) *(- 3) +
    g(- 38) * 3 +
    g(- 40) +
    g(- 42) +
    g(- 43);
    this->output.unknownNegativeGenerators[1] = g(- 20);
    return true;
  }
  if (
    type == A(37, 1) + A(3, 1) &&
    this->output.cartanElementsSubalgebra[1].isEqualTo(
      List<Rational>({0, 1, 1, 2, 0, 1, 0, 0})
    )
  ) {
    this->output.unknownNegativeGenerators[0] =
    g(- 28) +
    g(- 32) +
    g(- 34) +
    g(- 35) +
    g(- 36) +
    g(- 37) +
    g(- 38) +
    g(- 46) - g(- 50);
    this->output.unknownNegativeGenerators[1] =
    g(- 4) + g(- 6) + g(- 11) + g(- 17);
    return true;
  }
  if (
    type == A(36, 1) + A(28, 1) &&
    this->output.cartanElementsSubalgebra[0].isEqualTo(
      List<Rational>({16, 22, 30, 44, 36, 28, 20, 10})
    )
  ) {
    this->output.unknownNegativeGenerators[0] =
    g(- 28) +
    g(- 32) +
    g(- 36) +
    g(- 41) +
    g(- 42) +
    g(- 43) - g(- 44) +
    g(- 50);
    this->output.unknownNegativeGenerators[1] =
    - g(- 4) + g(- 5) + g(- 6) + g(- 17);
    return true;
  }
  if (
    type == A(36, 1) + A(4, 1) &&
    this->output.cartanElementsSubalgebra[0].isEqualTo(
      List<Rational>({16, 22, 30, 44, 36, 28, 20, 10})
    )
  ) {
    this->output.unknownNegativeGenerators[0] =
    g(- 28) +
    g(- 32) +
    g(- 36) +
    g(- 41) +
    g(- 42) +
    g(- 43) +
    g(- 44) +
    g(- 50);
    this->output.unknownNegativeGenerators[1] =
    - g(- 12) + g(- 13) + g(- 25) + g(- 39);
    return true;
  }
  if (
    type == A(36, 1) + A(3, 1) &&
    this->output.cartanElementsSubalgebra[0].isEqualTo(
      List<Rational>({16, 22, 30, 44, 36, 28, 20, 10})
    )
  ) {
    this->output.unknownNegativeGenerators[0] =
    g(- 28) +
    g(- 32) +
    g(- 36) +
    g(- 41) +
    g(- 42) +
    g(- 43) +
    g(- 44) +
    g(- 50);
    this->output.unknownNegativeGenerators[1] = - g(- 20) - g(- 31) + g(- 33);
    return true;
  }
  if (
    type == A(34, 1) + A(6, 1) &&
    this->output.cartanElementsSubalgebra[1].isEqualTo(
      List<Rational>({1, 2, 0, 2, 2, 0, 1, 0})
    )
  ) {
    this->output.unknownNegativeGenerators[0] =
    g(- 29) +
    g(- 38) * 5 +
    g(- 40) *(- 2) +
    g(- 41) * 7 +
    g(- 44) +
    g(- 45) +
    g(- 46) +
    g(- 48);
    this->output.unknownNegativeGenerators[1] =
    g(- 1) + g(- 2) + g(- 5) *(- 6) + g(- 7) - g(- 12);
    return true;
  }
  if (
    type == A(32, 1) + A(8, 1) &&
    this->output.cartanElementsSubalgebra[1].isEqualTo(
      List<Rational>({2, 0, 4, 4, 4, 4, 2, 0})
    )
  ) {
    this->output.unknownNegativeGenerators[0] =
    g(- 36) + g(- 38) + g(- 41) + g(- 44) + g(- 46) + g(- 48) + g(- 59);
    this->output.unknownNegativeGenerators[1] =
    g(- 6) + g(- 11) + g(- 21) * 2 + g(- 24) * 2 + g(- 28);
    return true;
  }
  if (
    type == A(30, 1) + A(10, 1) &&
    this->output.cartanElementsSubalgebra[0].isEqualTo(
      List<Rational>({14, 20, 27, 40, 33, 26, 18, 10})
    ) &&
    this->output.cartanElementsSubalgebra[1].isEqualTo(
      List<Rational>({0, 4, 3, 6, 3, 0, 0, 0})
    )
  ) {
    this->output.unknownNegativeGenerators[0] =
    g(- 8) + g(- 45) + g(- 57) + g(- 58) + g(- 59) + g(- 60);
    this->output.unknownNegativeGenerators[1] = g(- 2) + g(- 11) + g(- 12);
    return true;
  }
  if (
    type == A(30, 1) + A(10, 1) &&
    this->output.cartanElementsSubalgebra[0].isEqualTo(
      List<Rational>({14, 21, 28, 42, 34, 26, 18, 9})
    ) &&
    this->output.cartanElementsSubalgebra[1].isEqualTo(
      List<Rational>({2, 1, 2, 0, 2, 2, 0, 1})
    )
  ) {
    this->output.unknownNegativeGenerators[0] =
    g(- 40) + g(- 41) + g(- 42) + g(- 43) + g(- 44) + g(- 45) + g(- 46);
    this->output.unknownNegativeGenerators[1] =
    g(- 1) - g(- 2) + g(- 3) * 2 - g(- 5) + g(- 6) * 2 + g(- 8);
    return true;
  }
  if (
    type == A(25, 1) + A(15, 1) &&
    this->output.cartanElementsSubalgebra[0].isEqualTo(
      List<Rational>({13, 19, 26, 38, 31, 24, 16, 8})
    ) &&
    this->output.cartanElementsSubalgebra[1].isEqualTo(
      List<Rational>({1, 3, 0, 4, 3, 0, 2, 2})
    )
  ) {
    this->output.unknownNegativeGenerators[0] =
    g(- 44) + g(- 45) + g(- 46) + g(- 47) + g(- 48) + g(- 49) + g(- 50);
    this->output.unknownNegativeGenerators[1] =
    g(- 1) + g(- 2) *(- 3) + g(- 4) *(- 2) - g(- 5) - g(- 7) + g(- 8) *(- 2);
    return true;
  }
  if (
    type == A(24, 1) + A(16, 1) &&
    this->output.cartanElementsSubalgebra[0].isEqualTo(
      List<Rational>({12, 18, 24, 36, 30, 24, 16, 8})
    ) &&
    this->output.cartanElementsSubalgebra[1].isEqualTo(
      List<Rational>({4, 4, 6, 8, 4, 0, 2, 2})
    )
  ) {
    this->output.unknownNegativeGenerators[0] =
    g(- 36) +
    g(- 40) +
    g(- 42) +
    g(- 43) +
    g(- 45) +
    g(- 47) +
    g(- 54) * 2 - g(- 57);
    this->output.unknownNegativeGenerators[1] =
    g(- 4) +
    g(- 7) +
    g(- 8) +
    g(- 9) +
    g(- 11) +
    g(- 12) * 3 +
    g(- 17) +
    g(- 18) * 3 - g(- 19);
    return true;
  }
  if (
    type == A(24, 1) + A(15, 1) &&
    this->output.cartanElementsSubalgebra[0].isEqualTo(
      List<Rational>({12, 18, 24, 36, 30, 24, 16, 8})
    ) &&
    this->output.cartanElementsSubalgebra[1].isEqualTo(
      List<Rational>({4, 4, 6, 7, 4, 0, 2, 2})
    )
  ) {
    this->output.unknownNegativeGenerators[0] =
    g(- 40) * 3 +
    g(- 42) * 3 +
    g(- 43) * 3 +
    g(- 51) *(- 3) +
    g(- 52) +
    g(- 54) * 2;
    this->output.unknownNegativeGenerators[1] =
    g(- 1) * 3 + g(- 7) * 3 + g(- 8) * 3 + g(- 17) * 3 - g(- 18) + g(- 19) * 3;
    return true;
  }
  if (
    type == A(22, 1) + A(2, 1) &&
    this->output.cartanElementsSubalgebra[0].isEqualTo(
      List<Rational>({12, 18, 24, 36, 29, 22, 15, 8})
    ) &&
    this->output.cartanElementsSubalgebra[1].isEqualTo(
      List<Rational>({0, 0, 0, 0, 1, 2, 1, 0})
    )
  ) {
    this->output.unknownNegativeGenerators[0] =
    g(- 42) + g(- 43) + g(- 45) + g(- 46) + g(- 48) + g(- 59) + g(- 64);
    this->output.unknownNegativeGenerators[1] = - g(- 13) + g(- 14);
    return true;
  }
  if (
    type == A(20, 1) + A(1, 1) &&
    this->output.cartanElementsSubalgebra[0].isEqualTo(
      List<Rational>({12, 16, 22, 32, 26, 20, 14, 8})
    ) &&
    this->output.cartanElementsSubalgebra[1].isEqualTo(
      List<Rational>({0, 1, 1, 2, 2, 2, 1, 0})
    )
  ) {
    this->output.unknownNegativeGenerators[0] =
    g(- 22) + g(- 40) + g(- 42) + g(- 43) + g(- 52) + g(- 58) *(- 6);
    this->output.unknownNegativeGenerators[1] = g(- 60);
    return true;
  }
  if (
    type == A(16, 1) + A(6, 1) &&
    this->output.cartanElementsSubalgebra[0].isEqualTo(
      List<Rational>({10, 16, 20, 30, 24, 18, 12, 6})
    ) &&
    this->output.cartanElementsSubalgebra[1].isEqualTo(
      List<Rational>({2, 0, 3, 4, 4, 4, 3, 2})
    )
  ) {
    this->output.unknownNegativeGenerators[0] =
    g(- 45) - g(- 51) +
    g(- 52) +
    g(- 53) +
    g(- 55) +
    g(- 56) +
    g(- 57) +
    g(- 60) +
    g(- 64);
    this->output.unknownNegativeGenerators[1] =
    g(- 16) * 2 + g(- 20) - g(- 27) + g(- 28) - g(- 29);
    return true;
  }
  if (
    type == B(12, 2) &&
    this->output.cartanElementsSubalgebra[0].isEqualTo(
      List<Rational>({9, 13, 18, 26, 21, 16, 11, 6})
    ) &&
    this->output.cartanElementsSubalgebra[1].isEqualTo(
      List<Rational>({- 4, - 5, - 9, - 11, - 8, - 6, - 4, - 3})
    )
  ) {
    this->output.unknownNegativeGenerators[0] =
    g(- 55) +
    g(- 61) +
    g(- 62) +
    g(- 68) +
    g(- 69) +
    g(- 70) +
    g(- 71) +
    g(- 76);
    this->output.unknownNegativeGenerators[1] =
    g(39) * "89/36" - g(35) * "1/3" - g(33) * "7/3" +
    g(32) +
    g(31) * "11/36" - g(25) * "1/3" +
    g(23) * 2 - g(22) +
    g(15);
    return true;
  }
  return false;
}

ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >
CandidateSemisimpleSubalgebraArbitraryConstants::g(int displayIndex) {
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > result;
  result.makeGeneratorFromDisplayIndex(
    displayIndex, this->output.getAmbientSemisimpleLieAlgebra()
  );
  return result;
}

DynkinType CandidateSemisimpleSubalgebraArbitraryConstants::A(
  int dynkinIndex, int rank
) const {
  DynkinType result;
  Rational dynkinIndexRational = dynkinIndex;
  result.makeSimpleType('A', rank, &dynkinIndexRational);
  return result;
}

DynkinType CandidateSemisimpleSubalgebraArbitraryConstants::B(
  int dynkinIndex, int rank
) const {
  DynkinType result;
  Rational dynkinIndexRational = dynkinIndex;
  result.makeSimpleType('B', rank, &dynkinIndexRational);
  return result;
}

DynkinType CandidateSemisimpleSubalgebraArbitraryConstants::E(
  int dynkinIndex, int rank
) const {
  DynkinType result;
  Rational dynkinIndexRational = dynkinIndex;
  result.makeSimpleType('E', rank, &dynkinIndexRational);
  return result;
}
