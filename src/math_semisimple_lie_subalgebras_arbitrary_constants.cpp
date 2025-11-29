#include "general_lists.h"
#include "math_extra_semisimple_lie_subalgebras.h"

class CandidateSemisimpleSubalgebraArbitraryContants {
public:
  char dynkinType;
  int rank;
  CandidateSemisimpleSubalgebra& output;
  bool loadE8();
  bool loadE6();
  bool load();
  CandidateSemisimpleSubalgebraArbitraryContants(
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
};

bool CandidateSemisimpleSubalgebra::loadBuiltInPartialRealization() {
  CandidateSemisimpleSubalgebraArbitraryContants constants(*this);
  return constants.load();
}

bool CandidateSemisimpleSubalgebraArbitraryContants::load() {
  if (
    !this->output.getAmbientWeyl().dynkinType.isSimple(
      &this->dynkinType, &this->rank
    )
  ) {
    return false;
  }
  if (dynkinType == 'E' && rank == 6) {
    return CandidateSemisimpleSubalgebraArbitraryContants::loadE6();
  }
  if (dynkinType == 'E' && rank == 8) {
    return CandidateSemisimpleSubalgebraArbitraryContants::loadE8();
  }
  return false;
}

bool CandidateSemisimpleSubalgebraArbitraryContants::loadE6() {
  std::string typeToBeRealized =
  this->output.weylNonEmbedded->dynkinType.toString();
  if (typeToBeRealized == "A^{36}_1") {
    this->output.unknownNegativeGenerators.clear();
    this->output.unknownNegativeGenerators.addOnTop(
      g(- 6) + g(- 7) + g(- 8) + g(- 9) + g(- 10) + g(- 19)
    );
    return true;
  }
  return false;
}

bool CandidateSemisimpleSubalgebraArbitraryContants::loadE8() {
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
CandidateSemisimpleSubalgebraArbitraryContants::g(int displayIndex) {
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > result;
  result.makeGeneratorFromDisplayIndex(
    displayIndex, this->output.getAmbientSemisimpleLieAlgebra()
  );
  return result;
}
