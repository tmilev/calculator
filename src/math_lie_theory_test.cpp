#include "math_extra_lie_theory_extras.h" // IWYU pragma: keep: breaks the build.
#include "math_general_implementation.h" // IWYU pragma: keep: breaks the build.

template < >
bool ElementUniversalEnveloping<Rational>::Test::casimirElement() {
  STACK_TRACE("ElementUniversalEnveloping::Test::casimirElement");
  List<std::string> typesToCheck = List<std::string>({
      "A_1",
      "A_2",
      "A_3",
      "B_2",
      "B_3",
      "C_3",
      "D_4",
      "G_2",
      "F_4",
      "E_6",
      "E_7",
      "E_8"
    });
  int64_t startTime = global.getElapsedMilliseconds();
  for (const std::string& dynkinType : typesToCheck) {
    SemisimpleLieAlgebra algebra;
    algebra.weylGroup.dynkinType.fromString(dynkinType);
    algebra.weylGroup.makeFromDynkinType(algebra.weylGroup.dynkinType);
    algebra.computeChevalleyConstants();
    ElementUniversalEnveloping<Rational> casimirMethod1;
    ElementUniversalEnveloping<Rational> casimirMethod2;
    casimirMethod1.makeCasimir(algebra);
    casimirMethod2.makeCasimirErrorCheck(algebra);
    casimirMethod1.scaleNormalizeLeadingMonomial(nullptr);
    casimirMethod2.scaleNormalizeLeadingMonomial(nullptr);
    if (casimirMethod1 != casimirMethod2) {
      global.fatal
      << "Element computed with method 1: "
      << casimirMethod1.toStringPretty()
      << " does not equal the element computed with method 2: "
      << casimirMethod2.toStringPretty()
      << global.fatal;
    }
  }
  int64_t finishTime = global.getElapsedMilliseconds();
  global
  << "Computed the Casimir elements of: "
  << typesToCheck
  << " in: "
  << Logger::purple
  << (finishTime - startTime)
  << " milliseconds. "
  << Logger::endL;
  return true;
}

template < >
bool ElementUniversalEnveloping<Rational>::Test::all() {
  ElementUniversalEnveloping::Test::casimirElement();
  return true;
}
