#include "math_extra_semisimple_lie_subalgebras.h"
#include "test.h"

namespace Testing {

bool SemisimpleSubalgebrasTest::all() {
  SemisimpleSubalgebrasTest::constructAllB3Subalgebras();
  return true;
}

bool SemisimpleSubalgebrasTest::constructAllB3Subalgebras() {
  STACK_TRACE("SemisimpleSubalgebrasTest::constructAllB3Subalgebras");
  SemisimpleSubalgebras subalgebras;
  SemisimpleLieAlgebra b3;
  b3.weylGroup.makeArbitrarySimple('B', 3);
  AlgebraicClosureRationals algebraicClosure;
  MapReferences<std::string, AlgebraicClosureRationals>
  algebraicClosuresForLargeComputations;
  MapReferences<DynkinType, SemisimpleLieAlgebra> subalgebrasNonEmbedded;
  ListReferences<SlTwoSubalgebras> sl2sOfSubalgebras;
  subalgebras.findSemisimpleSubalgebrasFromScratch(
    b3,
    algebraicClosure,
    algebraicClosuresForLargeComputations,
    subalgebrasNonEmbedded,
    sl2sOfSubalgebras,
    nullptr
  );
  int expected = 16;
  if (subalgebras.subalgebras.size() != expected) {
    global.fatal
    << "B3 subalgebra count is wrong: got: "
    << subalgebras.subalgebras.size()
    << ", expected: "
    << expected
    << "."
    << global.fatal;
  }
  return true;
}
} // namespace: Testing.
