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
  // Makes a polynomial of the form x_{display_index}.
  Polynomial<AlgebraicNumber> x(int displayIndex) {
    Polynomial<AlgebraicNumber> result;
    result.makeMonomial(
      displayIndex - 1, 1, this->output.owner->ownerField->one()
    );
    return result;
  }
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
  int maximumPolynomialDivisions = 1000;
  int maximumMonomialOperations = 10000;
  std::string ambientLieAlgebraName =
  this->output.owner->owner->toStringLieAlgebraName();
  if (ambientLieAlgebraName == "C^{1}_5") {
    // Known to run slow in this type.
    maximumPolynomialDivisions = 100;
    maximumMonomialOperations = 2000;
  }
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
  if (embeddingLieAlgebraName == "A^{20}_1+A^{4}_1") {
    // System known to be contradictory but takes extra computational time.
    maximumPolynomialDivisions = 2000;
    maximumMonomialOperations = 10000;
  }
  // Polynomial systems that were solved manually (with the help of computer):
  // 1. A^{32}_1+A^{8}_1 in E_8.
  // 2. A^24_1 in E_8.
  // 3. A^{20}_1+A^{4}_1 in E_8
  if (embeddingLieAlgebraName == "A^{16}_1+A^{8}_1") {
    maximumPolynomialDivisions = 200;
    maximumMonomialOperations = 400;
  }
  if (embeddingLieAlgebraName == "A^{15}_1") {
    maximumPolynomialDivisions = 200;
    maximumMonomialOperations = 1000;
  }
  if (embeddingLieAlgebraName == "A^{40}_1") {
    // System known to require more computations than most.
    maximumPolynomialDivisions = 4000;
    maximumMonomialOperations = 20000;
  }
  if (
    embeddingLieAlgebraName == "A^{84}_1+A^{4}_1" &&
    ambientLieAlgebraName == "E^{1}_8"
  ) {
    maximumPolynomialDivisions = 1000;
    maximumMonomialOperations = 10000;
    this->output.configuredSystemToSolve.substitutionsProvider.
    oneIsFirstWhenRecursionDepthIsMultipleOf =
    0;
  }
  if (embeddingLieAlgebraName == "A^{36}_1+A^{4}_1") {
    this->output.configuredSystemToSolve.substitutionsProvider.
    oneIsFirstWhenRecursionDepthIsMultipleOf =
    2;
  }
  this->output.configuredSystemToSolve.groebner.maximumMonomialOperations =
  maximumMonomialOperations;
  this->output.configuredSystemToSolve.groebner.maximumPolynomialDivisions =
  maximumPolynomialDivisions;
  this->output.configuredSystemToSolve.algebraicClosure =
  this->output.owner->ownerField;
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
  std::string typeToBeRealized =
  this->output.weylNonEmbedded->dynkinType.toString();
  if (typeToBeRealized == "A^{14}_1") {
    this->output.unknownNegativeGenerators.clear();
    this->output.unknownNegativeGenerators.addOnTop(
      g(- 45) + g(- 47) + g(- 49) + g(- 57) + g(- 60) + g(- 70) + g(- 82)
    );
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
