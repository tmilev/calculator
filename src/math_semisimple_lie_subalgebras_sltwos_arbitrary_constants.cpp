#include "general_lists.h"
#include "math_extra_semisimple_lie_subalgebras_sltwos.h"

Rational SlTwoSubalgebraCandidate::fArbitraryCoefficient(
  int coefficientIndex,
  char type,
  int rank,
  int dynkinIndex,
  const Vector<Rational>& targetH
) {
  List<Rational> arbitraryCoefficients =
  SlTwoSubalgebraCandidate::fArbitraryCoefficients(
    type, rank, dynkinIndex, targetH
  );
  if (coefficientIndex < arbitraryCoefficients.size) {
    return arbitraryCoefficients[coefficientIndex];
  }
  return coefficientIndex * coefficientIndex + 1;
}

List<Rational> SlTwoSubalgebraCandidate::fArbitraryCoefficients(
  char type, int rank, int dynkinIndex, const Vector<Rational>& targetH
) {
  STACK_TRACE("SlTwoSubalgebraCandidate::fArbitraryCoefficients");
  std::string h = targetH.toString();
  // Coefficients found by manual experimentation with the computation
  // end-to-end.
  // Do not work in all cases, found out by quick computational experiments.
  if (type == 'C' && rank == 4 && dynkinIndex == 4) {
    return List<Rational>({1, 1, - 1, 1});
  }
  if (type == 'D' && rank == 4 && dynkinIndex == 12) {
    return List<Rational>({1, - 1, 2, - 2, 1, - 1});
  }
  if (type == 'F' && dynkinIndex == 28) {
    return List<Rational>({1, 1, 1, 1});
  }
  if (type == 'C' && rank == 5) {
    if (dynkinIndex == 5) {
      return List<Rational>({1, - 1, 3, - 2, 3});
    }
    if (dynkinIndex == 4) {
      return List<Rational>({1, 1, 1, 1, 1});
    }
  }
  if (type == 'D' && rank == 5) {
    if (dynkinIndex == 30) {
      return List<Rational>({1, - 2, 2, - 1, - 1, 1, - 1});
    }
  }
  if (type == 'B' && rank == 6) {
    if (dynkinIndex == 32) {
      return List<Rational>({1, - 1, 3, - 1, 3, - 3});
    }
    if (dynkinIndex == 8) {
      return List<Rational>({1, 1, 1, 1, - 1, 1});
    }
    if (dynkinIndex == 6) {
      return List<Rational>({1, 1, - 1, 1, 1, 1});
    }
    if (dynkinIndex == 3) {
      return List<Rational>({1, - 1, 1, 1, 1, - 1});
    }
  }
  if (type == 'C' && rank == 6) {
    if (dynkinIndex == 11) {
      return List<Rational>({1, - 1, 1, - 1, 1, - 1});
    }
    if (dynkinIndex == 8) {
      return List<Rational>({1, 1, 1, 1, 1, 1});
    } else if (dynkinIndex == 6) {
      return List<Rational>({1, 1, 1, 1, 1, 1});
    } else if (dynkinIndex == 5) {
      return List<Rational>({1, - 1, 1, 1, 1, 1});
    } else if (dynkinIndex == 4) {
      return List<Rational>({1, 1, 1, 1, 1, 1});
    } else if (dynkinIndex == 3) {
      return List<Rational>({1, 1, 1});
    }
  }
  if (type == 'D' && rank == 6) {
    if (dynkinIndex == 62) {
      return List<Rational>({1, - 1, 2, - 2, 1, - 1, 1, 1, 1, 1, 1});
    }
    if (dynkinIndex == 38) {
      return List<Rational>({1, - 1, 2, - 2, 3, - 3, 1, 1, 1, 1, 1});
    }
    if (dynkinIndex == 13) {
      return List<Rational>({1, - 1, 2, - 2, 1, - 1, 1, 1, 1, 1, 1});
    }
    if (dynkinIndex == 6) {
      return List<Rational>({1, 1, 1, 1, 1, 1});
    }
    if (dynkinIndex == 2) {
      return List<Rational>({1, 1, 1, 1, 1, 1});
    }
  }
  if (type == 'E' && rank == 6) {
    if (dynkinIndex == 84) {
      return List<Rational>({1, - 3, 2, - 2, 1, - 1, 1});
    }
    if (dynkinIndex == 30) {
      return List<Rational>({1, - 2, 2, - 1, 1, - 1, 1});
    }
  }
  if (type == 'B' && rank == 7) {
    if (dynkinIndex == 40) {
      return
      List<Rational>({1, - 1, 1, - 1, 1, - 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1});
    }
  }
  if (type == 'C' && rank == 7) {
    if (dynkinIndex == 15) {
      return List<Rational>({1, 1, 1, 1, 1, 0, 0, 0, 0});
    }
    if (dynkinIndex == 12) {
      return List<Rational>({1, 1, 1, 1, 1, 1, 1});
    }
    if (dynkinIndex == 7) {
      return List<Rational>({1, 1, 1, 1, 0, 0, 0});
    }
    if (dynkinIndex == 6) {
      return List<Rational>({1, 1, 1, 1, 0, 0, 0});
    }
    if (dynkinIndex == 5) {
      return List<Rational>({1, 1, 1, 0, 0, 0, 0});
    }
    if (dynkinIndex == 4) {
      return List<Rational>({1, 1, 1, 0, 0, 0, 0});
    }
    if (dynkinIndex == 3) {
      return List<Rational>({1, 1, 0, 0, 0, 0, 0});
    }
  }
  if (type == 'D' && rank == 7) {
    if (dynkinIndex == 112) {
      return List<Rational>({1, - 1, 2, - 2, 3, - 3, 1, 1, 1, 1, 1});
    }
    if (dynkinIndex == 70) {
      return List<Rational>({1, - 1, 1, - 1, 1, - 1, 1, - 1, 1, 1, 1});
    }
    if (dynkinIndex == 31) {
      return List<Rational>({1, - 3, 2, - 1, 1, - 1, 1, - 1});
    }
    if (dynkinIndex == 16) {
      return List<Rational>({1, - 1, 2, - 2, 1, - 1, 1, - 1, 1, - 1, 1});
    }
    if (dynkinIndex == 10 && h == "(4, 6, 6, 6, 6, 3, 3)") {
      return List<Rational>({1, 1, 1, 0, 0, 0, 0});
    }
    if (dynkinIndex == 6) {
      return List<Rational>({1, 1, 1, 1, 0, 0, 0});
    }
  }
  if (type == 'E' && rank == 7) {
    if (dynkinIndex == 231) {
      return List<Rational>({1, - 1, 2, - 2, 3, - 3, 4, - 4, 1, 1, 1});
    }
    if (dynkinIndex == 159) {
      return List<Rational>({1, - 1, 2, - 2, 3, - 3, 4, - 4, 1, 1, 1});
    }
    if (dynkinIndex == 63) {
      return
      List<Rational>(
        {1, - 1, 2, - 2, 3, - 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
      );
    }
    if (dynkinIndex == 62) {
      return List<Rational>({1, - 1, 2, - 2, 3, - 3, 1, 1, 1, 1, 1});
    }
    if (dynkinIndex == 38) {
      return List<Rational>({1, - 1, 2, - 2, 3, - 3, 4, - 4, 1, 1, 1});
    }
    if (dynkinIndex == 35) {
      return List<Rational>({1, 1, 1, 1, 1, 0, 0, 0});
    }
    if (dynkinIndex == 29) {
      return List<Rational>({1, 1, 1, 1, 1, 0, 0, 0});
    }
    if (dynkinIndex == 28) {
      return List<Rational>({1, 1, 1, 1, 0, 0, 0, 0});
    }
    if (dynkinIndex == 11) {
      return List<Rational>({1, 1, 1, 1, 0, 0, 0, 0});
    }
    if (dynkinIndex == 10) {
      return List<Rational>({1, 1, 1, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 8) {
      return List<Rational>({1, 1, 1, 1, 0, 0, 0, 0});
    }
    if (dynkinIndex == 7) {
      return List<Rational>({1, 1, 1, 1, 1, 0, 0, 0});
    }
    if (dynkinIndex == 6) {
      return List<Rational>({1, - 1, 2, - 2, 3, - 3, 4, - 4, 1, 1, 1});
    }
    if (dynkinIndex == 4) {
      return List<Rational>({1, 1, 1, 1, 0, 0, 0, 0});
    }
    if (dynkinIndex == 3) {
      return List<Rational>({1, 1, 1, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 2) {
      return List<Rational>({1, 1, 0, 0, 0, 0, 0, 0});
    }
  }
  if (type == 'A' && rank == 8) {
    if (dynkinIndex == 8) {
      return List<Rational>({1, 1, 1, 1, 0, 0, 0, 0});
    }
    if (dynkinIndex == 7) {
      return List<Rational>({1, 1, 1, 1, 1, 0, 0, 0});
    }
    if (dynkinIndex == 4) {
      return List<Rational>({1, 1, 1, 1, 0, 0, 0, 0});
    }
    if (dynkinIndex == 3) {
      return List<Rational>({1, 1, 1, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 2) {
      return List<Rational>({1, 1, 0, 0, 0, 0, 0, 0});
    }
  }
  if (type == 'B' && rank == 8) {
    if (dynkinIndex == 72) {
      return
      List<Rational>(
        {1, - 1, 2, - 2, 3, - 3, 4, - 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
      );
    }
    if (dynkinIndex == 62) {
      return List<Rational>({1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1});
    }
    if (dynkinIndex == 35) {
      return List<Rational>({1, 1, 1, 1, 1, 0, 0, 0});
    }
    if (dynkinIndex == 30) {
      return List<Rational>({1, 1, 1, 1, 1, 1, 0, 0});
    }
    if (dynkinIndex == 20) {
      return List<Rational>({1, 1, 1, 1, 1, 1, 0, 0});
    }
    if (dynkinIndex == 14) {
      return List<Rational>({1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1});
    }
    if (dynkinIndex == 13) {
      return List<Rational>({1, 1, 1, 1, 1, 0, 0, 0});
    }
    if (dynkinIndex == 12) {
      return List<Rational>({1, 1, 1, 1, 1, 0, 0, 0});
    }
    if (dynkinIndex == 11) {
      return List<Rational>({1, 1, 1, 1, 0, 0, 0, 0});
    }
    if (dynkinIndex == 10) {
      return List<Rational>({1, 1, 1, 1, 1, 0, 0, 0});
    }
    if (dynkinIndex == 8) {
      return List<Rational>({1, 1, 1, 1, 1, 0, 0, 0});
    }
    if (dynkinIndex == 7) {
      return List<Rational>({1, 1, 1, 1, 0, 0, 0, 0});
    }
    if (dynkinIndex == 6) {
      return List<Rational>({1, 1, 1, 1, 0, 0, 0, 0});
    }
    if (dynkinIndex == 5) {
      return List<Rational>({1, 1, 1, 1, 0, 0, 0, 0});
    }
    if (dynkinIndex == 4) {
      return List<Rational>({1, 1, 1, 1, 0, 0, 0, 0});
    }
    if (dynkinIndex == 3) {
      return List<Rational>({1, 1, 1, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 2) {
      return List<Rational>({1, 1, 0, 0, 0, 0, 0, 0});
    }
  }
  if (type == 'C' && rank == 8) {
    if (
      dynkinIndex == 40 &&
      targetH == List<Rational>({10, 16, 18, 20, 22, 24, 26, 14})
    ) {
      return
      List<Rational>(
        {1, - 1, 2, - 2, 3, - 3, 4, - 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
      );
    }
    if (dynkinIndex == 16 && h == "(6, 8, 10, 12, 14, 16, 18, 10)") {
      return
      List<Rational>({1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 15) {
      return
      List<Rational>({1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 13 && h == "(6, 8, 10, 12, 14, 14, 14, 7)") {
      return
      List<Rational>({1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 13 && h == "(4, 8, 10, 12, 14, 16, 18, 9)") {
      return
      List<Rational>({1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 11 && h == "(4, 8, 10, 12, 14, 14, 14, 7)") {
      return
      List<Rational>(
        {1, - 1, 1, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
      );
    }
    if (dynkinIndex == 8 && h == "(2, 4, 6, 8, 10, 12, 14, 8)") {
      return
      List<Rational>({1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 8 && h == "(4, 8, 8, 8, 8, 8, 8, 4)") {
      return
      List<Rational>({1, - 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 7) {
      return
      List<Rational>({1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 6) {
      return
      List<Rational>({1, - 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 5) {
      return
      List<Rational>({1, - 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 4) {
      return
      List<Rational>({1, - 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 3) {
      return
      List<Rational>({1, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    }
  }
  if (type == 'D' && rank == 8) {
    if (dynkinIndex == 184) {
      return
      List<Rational>({1, - 1, 1, - 1, 1, - 1, 1, - 2, 1, 1, 1, 1, 1, 1, 1});
    }
    if (dynkinIndex == 120) {
      return
      List<Rational>({1, - 1, 1, - 1, 1, - 1, 1, - 2, 1, 1, 1, 1, 1, 1, 1});
    }
    if (dynkinIndex == 110) {
      return
      List<Rational>({1, - 1, 1, - 1, 1, - 1, 1, - 2, 1, 1, 1, 1, 1, 1, 1});
    }
    if (dynkinIndex == 88) {
      return
      List<Rational>({1, - 1, 2, - 2, 3, - 3, 4, - 4, 1, 1, 1, 1, 1, 1});
    }
    if (dynkinIndex == 63) {
      return
      List<Rational>({1, - 1, 1, - 1, 1, - 1, 1, - 2, 1, 1, 1, 1, 1, 1, 1});
    }
    if (dynkinIndex == 60) {
      return
      List<Rational>({1, - 1, 1, - 1, 1, - 1, 1, - 2, 1, 1, 1, 1, 1, 1, 1});
    }
    if (dynkinIndex == 40) {
      return
      List<Rational>({
          1,
          - 1,
          1,
          - 1,
          1,
          - 1,
          1,
          - 2,
          1,
          1,
          1,
          1,
          1,
          1,
          1,
          1,
          1,
          1,
          1,
          1,
          1,
          1
        });
    }
    if (dynkinIndex == 39) {
      return
      List<Rational>({1, - 1, 1, - 1, 1, - 1, 1, - 2, 1, 1, 1, 1, 1, 1, 1});
    }
    if (dynkinIndex == 34) {
      return
      List<Rational>({1, - 1, 1, - 1, 1, - 1, 1, - 2, 1, 1, 1, 1, 1, 1, 1});
    }
    if (dynkinIndex == 22) {
      return
      List<Rational>({1, - 1, 1, - 1, 1, - 1, 1, - 2, 1, 1, 1, 1, 1, 1, 1});
    }
    if (dynkinIndex == 14 && h == "(4, 6, 8, 10, 10, 10, 5, 5)") {
      return List<Rational>({1, - 1, 1, - 1, 1, 0, 0, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 14 && h == "(4, 6, 8, 9, 10, 11, 6, 6)") {
      return
      List<Rational>({1, - 1, 1, - 1, 1, - 1, 1, - 2, 1, 1, 1, 1, 1, 1, 1});
    }
    if (dynkinIndex == 8) {
      return List<Rational>({1, - 1, 1, - 1, 0, - 1, 1, - 2, 0, 0, 0, 0});
    }
    if (dynkinIndex == 7) {
      return List<Rational>({1, - 1, 1, - 1, 1, 0, 0, - 2, 0, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 6) {
      return List<Rational>({1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 5 && h == "(2, 4, 5, 6, 6, 6, 3, 3)") {
      return List<Rational>({1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 5 && h == "(2, 3, 4, 5, 6, 7, 4, 4)") {
      return List<Rational>({1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0});
    }
  }
  if (type == 'E' && rank == 8) {
    if (dynkinIndex == 760) {
      return List<Rational>({1, - 1, 1, - 1, 1, - 1, 1, 1, 1, 0, 0});
    }
    if (dynkinIndex == 520) {
      return List<Rational>({1, - 1, 1, - 1, 1, - 1, 1, 1, 1, 1, 1, 1, 1});
    }
    if (dynkinIndex == 232) {
      return
      List<Rational>(
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}
      );
    }
    if (dynkinIndex == 231) {
      return
      List<Rational>({
          1,
          - 1,
          2,
          - 2,
          3,
          - 3,
          4,
          - 4,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0
        });
    }
    if (dynkinIndex == 184) {
      return
      List<Rational>({
          1,
          - 1,
          1,
          - 1,
          1,
          - 1,
          1,
          - 1,
          1,
          - 1,
          1,
          - 1,
          1,
          1,
          1,
          1,
          1,
          1,
          1,
          1
        });
    }
    if (dynkinIndex == 159) {
      return List<Rational>({1, - 1, 1, - 1, 1, - 1, 1, 1, 1, 1, 1,});
    }
    if (dynkinIndex == 88) {
      return
      List<Rational>({
          1,
          1,
          1,
          1,
          1,
          1,
          1,
          1,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          1,
          1,
          1,
          1,
          1,
          1,
          1,
          1,
          1
        });
    }
    if (dynkinIndex == 63) {
      return
      List<Rational>(
        {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0}
      );
    }
    if (dynkinIndex == 34) {
      return
      List<Rational>(
        {1, - 1, 2, - 2, 3, - 3, 4, - 4, 1, - 1, 1, - 1, 1, - 1, 1, 1, 1}
      );
    }
    if (dynkinIndex == 16) {
      return
      List<Rational>({1, 0, 2, - 2, 3, - 3, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 8) {
      return List<Rational>({1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 6) {
      return List<Rational>({1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 4) {
      return List<Rational>({1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 3) {
      return List<Rational>({1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    }
    if (dynkinIndex == 2) {
      return List<Rational>({1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    }
  }
  return
  List<Rational>(
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
  );
}

int CentralizerComputer::arbitraryCartanWeight(
  int index,
  char ambientSimpleType,
  int ambientRank,
  const Rational& dynkinIndexOfSlTwo
) {
  STACK_TRACE("CentralizerComputer::arbitraryCartanWeight");
  List<int> preferredHardCodedCoefficients =
  CentralizerComputer::hardCodedArbitraryCoefficientsCartan(
    ambientSimpleType, ambientRank, dynkinIndexOfSlTwo
  );
  if (index < preferredHardCodedCoefficients.size) {
    return preferredHardCodedCoefficients[index];
  }
  return 1;
}

List<int> CentralizerComputer::hardCodedArbitraryCoefficientsCartan(
  char ambientSimpleType, int ambientRank, const Rational& dynkinIndexOfSlTwo
) {
  if (ambientSimpleType == 'B' && ambientRank == 4 && dynkinIndexOfSlTwo == 2) {
    return List<int>({1, 2});
  }
  if (ambientSimpleType == 'D' && ambientRank == 4 && dynkinIndexOfSlTwo == 2) {
    return List<int>({1, 2});
  }
  if (ambientSimpleType == 'F' && ambientRank == 4 && dynkinIndexOfSlTwo == 8) {
    return List<int>({1, 3});
  }
  if (ambientSimpleType == 'A' && ambientRank == 5 && dynkinIndexOfSlTwo == 3) {
    return List<int>({1, 3});
  }
  if (ambientSimpleType == 'B' && ambientRank == 5) {
    if (dynkinIndexOfSlTwo == 4 || dynkinIndexOfSlTwo == 2) {
      return List<int>({1, 2});
    }
  }
  if (ambientSimpleType == 'D' && ambientRank == 5) {
    if (
      dynkinIndexOfSlTwo == 10 ||
      dynkinIndexOfSlTwo == 3 ||
      dynkinIndexOfSlTwo == 2
    ) {
      return List<int>({1, 2, - 2});
    }
  }
  if (ambientSimpleType == 'A' && ambientRank == 6 && dynkinIndexOfSlTwo == 3) {
    return List<int>({1, 1, - 1});
  }
  if (ambientSimpleType == 'B' && ambientRank == 6) {
    if (dynkinIndexOfSlTwo == 12) {
      return List<int>({1, 2, 1});
    }
    if (dynkinIndexOfSlTwo == 10) {
      return List<int>({1, 1, - 1});
    }
    if (dynkinIndexOfSlTwo == 6) {
      return List<int>({1, 2, - 1});
    }
    if (dynkinIndexOfSlTwo == 4) {
      return List<int>({1, 2, - 1});
    }
    if (dynkinIndexOfSlTwo == 3) {
      return List<int>({1, - 3, 2});
    }
    if (dynkinIndexOfSlTwo == 2) {
      return List<int>({1, - 2, 3, - 4});
    }
  }
  if (ambientSimpleType == 'C' && ambientRank == 6) {
    if (dynkinIndexOfSlTwo == 16) {
      return List<int>({1, 2});
    }
    if (dynkinIndexOfSlTwo == 8) {
      return List<int>({1, 5, - 2, 3});
    }
    if (dynkinIndexOfSlTwo == 3) {
      return List<int>({1, 2, 3});
    }
  }
  if (ambientSimpleType == 'D' && ambientRank == 6) {
    if (dynkinIndexOfSlTwo == 28) {
      return List<int>({1, 3});
    }
    if (dynkinIndexOfSlTwo == 10) {
      return List<int>({1, 3, - 1});
    }
    if (dynkinIndexOfSlTwo == 8) {
      return List<int>({1, 3, - 1});
    }
    if (dynkinIndexOfSlTwo == 4) {
      return List<int>({1, 3});
    }
    if (dynkinIndexOfSlTwo == 3) {
      return List<int>({1, 7, - 3, 4});
    }
    if (dynkinIndexOfSlTwo == 2) {
      return List<int>({1, - 1, 1, - 1});
    }
    return List<int>({});
  }
  if (ambientSimpleType == 'E' && ambientRank == 6) {
    if (dynkinIndexOfSlTwo == 8) {
      return List<int>({1, 3});
    }
    if (dynkinIndexOfSlTwo == 2) {
      return List<int>({1, 4, 2, 1});
    }
  }
  if (ambientSimpleType == 'A' && ambientRank == 7) {
    if (dynkinIndexOfSlTwo == 4) {
      return List<int>({1, - 1, 1, - 1});
    }
    if (dynkinIndexOfSlTwo == 3) {
      return List<int>({1, - 1, 1, - 1});
    }
    if (dynkinIndexOfSlTwo == 2) {
      return List<int>({1, 2, - 2, - 1, 1});
    }
  }
  if (ambientSimpleType == 'B' && ambientRank == 7) {
    if (dynkinIndexOfSlTwo == 30) {
      return List<int>({1, 5});
    }
    if (dynkinIndexOfSlTwo == 12) {
      return List<int>({1, 5});
    }
    if (dynkinIndexOfSlTwo == 10) {
      return List<int>({1, 3, - 1, 2});
    }
    if (dynkinIndexOfSlTwo == 6) {
      return List<int>({1, 4});
    }
    if (dynkinIndexOfSlTwo == 5) {
      return List<int>({1, 5, - 2});
    }
    if (dynkinIndexOfSlTwo == 4) {
      return List<int>({1, 3, - 1});
    }
    if (dynkinIndexOfSlTwo == 3) {
      return List<int>({1, 5, - 2, 1});
    }
    if (dynkinIndexOfSlTwo == 2) {
      return List<int>({1, 5, - 2, 1, - 3});
    }
  }
  if (ambientSimpleType == 'C' && ambientRank == 7) {
    if (dynkinIndexOfSlTwo == 17) {
      return List<int>({1, 7});
    }
    if (dynkinIndexOfSlTwo == 16) {
      return List<int>({1, 5, - 3});
    }
    if (dynkinIndexOfSlTwo == 15) {
      return List<int>({1, 5,});
    }
    if (dynkinIndexOfSlTwo == 9) {
      return List<int>({1, 5, - 3, - 4, 2});
    }
    if (dynkinIndexOfSlTwo == 8) {
      return List<int>({1, 5, - 3, - 4, 2});
    }
    if (dynkinIndexOfSlTwo == 7) {
      return List<int>({1, 5, - 3});
    }
    if (dynkinIndexOfSlTwo == 6) {
      return List<int>({1, 5, - 3, 2, 1});
    }
    if (dynkinIndexOfSlTwo == 5) {
      return List<int>({1, 5, - 3, 2, 1});
    }
    if (dynkinIndexOfSlTwo == 4) {
      return List<int>({1, 5, - 3, 2, 1});
    }
    if (dynkinIndexOfSlTwo == 3) {
      return List<int>({1, 7, - 3, 6, 1});
    }
  }
  if (ambientSimpleType == 'D' && ambientRank == 7) {
    if (dynkinIndexOfSlTwo == 60) {
      return List<int>({1, 5});
    }
    if (dynkinIndexOfSlTwo == 28) {
      return List<int>({1, 5, - 6});
    }
    if (dynkinIndexOfSlTwo == 12) {
      return List<int>({1, 5,});
    }
    if (dynkinIndexOfSlTwo == 11) {
      return List<int>({1, 5, - 6});
    }
    if (dynkinIndexOfSlTwo == 10) {
      return List<int>({1, 9, - 7, - 5, 8});
    }
    if (dynkinIndexOfSlTwo == 6) {
      return List<int>({1, 5, - 6, - 3, - 2});
    }
    if (dynkinIndexOfSlTwo == 4) {
      return List<int>({1, 9, - 7, - 5, 8});
    }
    if (dynkinIndexOfSlTwo == 3) {
      return List<int>({1, 9, - 7, - 5, 8});
    }
    if (dynkinIndexOfSlTwo == 2) {
      return List<int>({1, 11, - 7, - 5, - 8, 11});
    }
  }
  if (ambientSimpleType == 'E' && ambientRank == 7) {
    if (dynkinIndexOfSlTwo == 35) {
      return List<int>({1, 5});
    }
    if (dynkinIndexOfSlTwo == 29) {
      return List<int>({1, 5});
    }
    if (dynkinIndexOfSlTwo == 28) {
      return List<int>({1, 7, - 3});
    }
    if (dynkinIndexOfSlTwo == 12) {
      return List<int>({1, 7, 3, 1});
    }
    if (dynkinIndexOfSlTwo == 11) {
      return List<int>({1, 7, - 3});
    }
    if (dynkinIndexOfSlTwo == 10) {
      return List<int>({1, 7, - 3, - 5});
    }
    if (dynkinIndexOfSlTwo == 8) {
      return List<int>({1, 5});
    }
    if (dynkinIndexOfSlTwo == 7) {
      return List<int>({1, 5});
    }
    if (dynkinIndexOfSlTwo == 4) {
      return List<int>({1, 7, - 3});
    }
    if (dynkinIndexOfSlTwo == 3) {
      return List<int>({1, 7, - 3, - 5, 4, 6});
    }
    if (dynkinIndexOfSlTwo == 2) {
      return List<int>({1, 9, - 7, - 5, 4, 7});
    }
  }
  if (ambientSimpleType == 'A' && ambientRank == 8) {
    if (dynkinIndexOfSlTwo == 8) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 4) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 2) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
  }
  if (ambientSimpleType == 'B' && ambientRank == 8) {
    if (dynkinIndexOfSlTwo == 62) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 35) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 30) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 14) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 13) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 12) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 11) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 10) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 8) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 6) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 5) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 4) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 3) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 2) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
  }
  if (ambientSimpleType == 'C' && ambientRank == 8) {
    if (dynkinIndexOfSlTwo == 40) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 26) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 18) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 16) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 15) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 13) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 11) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 10) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 9) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 8) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 7) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 5) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 4) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 3) {
      return List<int>({1, 11, - 9, - 5, 6, 7, 2});
    }
  }
  if (ambientSimpleType == 'D' && ambientRank == 8) {
    if (dynkinIndexOfSlTwo == 110) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 60) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 30) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 29) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 28) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 14) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 12) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 11) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 10) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 9) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 6) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 5) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 4) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 3) {
      return List<int>({1, 11, - 9, - 7, 5, 6, 2});
    }
    if (dynkinIndexOfSlTwo == 2) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
  }
  if (ambientSimpleType == 'E' && ambientRank == 8) {
    if (dynkinIndexOfSlTwo == 156) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 110) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 60) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 36) {
      return List<int>({1, 11, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 35) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 29) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 28) {
      return List<int>({1, 9, - 7, - 5, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 20) {
      return List<int>({1, 11, - 9, - 7, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 12) {
      return List<int>({1, 11, - 9, - 7, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 11) {
      return List<int>({1, 11, - 9, - 7, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 10) {
      return List<int>({1, 11, - 9, - 7, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 9) {
      return List<int>({1, 11, - 9, - 7, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 8) {
      return List<int>({1, 11, - 9, - 7, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 6) {
      return List<int>({1, 11, - 9, - 7, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 4) {
      return List<int>({1, 11, - 9, - 7, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 3) {
      return List<int>({1, 11, - 9, - 7, 4, 3, 2});
    }
    if (dynkinIndexOfSlTwo == 2) {
      return List<int>({- 7, 19, - 17, - 13, 11, 7, - 5, 3});
    }
  }
  return List<int>({});
}

Rational CentralizerComputer::arbitraryCoefficientToFormSemisimpleElement(
  int index,
  char ambientSimpleType,
  int ambientRank,
  const Rational& dynkinIndexOfSlTwo,
  int centralizerDimension
) {
  STACK_TRACE(
    "CentralizerComputer::arbitraryCoefficientToFormSemisimpleElement"
  );
  List<int> hardCoded =
  CentralizerComputer::hardCodedCoefficientsToFormSemisimpleElement(
    ambientSimpleType, ambientRank, dynkinIndexOfSlTwo, centralizerDimension
  );
  if (index < hardCoded.size) {
    return hardCoded[index];
  }
  return 1;
}

List<int> CentralizerComputer::onesAtPositions(
  const List<int>& positions, int desiredSize
) {
  List<int> result;
  result.initializeFillInObject(desiredSize, 0);
  for (int index : positions) {
    result[index] = 1;
  }
  return result;
}

List<int> CentralizerComputer::hardCodedCoefficientsToFormSemisimpleElement(
  char ambientSimpleType,
  int ambientRank,
  const Rational& dynkinIndexOfSlTwo,
  int centralizerDimension
) {
  if (ambientSimpleType == 'B' && ambientRank == 6) {
    if (dynkinIndexOfSlTwo == 6) {
      return List<int>({1, - 1, 1, - 1, 1, - 1, 1, - 1, 1});
    }
  }
  if (ambientSimpleType == 'C' && ambientRank == 6) {
    if (dynkinIndexOfSlTwo == 11) {
      return List<int>({1, - 1, 1, - 1, 1, - 1});
    }
    if (dynkinIndexOfSlTwo == 6) {
      return List<int>({1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    }
    if (dynkinIndexOfSlTwo == 3) {
      return
      CentralizerComputer::onesAtPositions(
        List<int>({8, 9, 10, 11, 12, 14}), 24
      );
    }
  }
  if (ambientSimpleType == 'D' && ambientRank == 6) {
    if (dynkinIndexOfSlTwo == 6) {
      return List<int>({1, 1, 0, 0, 0, 0});
    }
    if (dynkinIndexOfSlTwo == 3) {
      return
      CentralizerComputer::onesAtPositions(
        List<int>({12, 13, 14, 15, 16}), 36
      );
    }
    if (dynkinIndexOfSlTwo == 2) {
      if (centralizerDimension == 36) {
        return
        List<int>({
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1,
            5,
            - 3,
            2,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
          });
      }
      if (centralizerDimension == 16) {
        return List<int>({0, 0, 0, 0, 1, 5, - 3, 2, 0, 0, 0, 0, 0, 0, 0, 0});
      }
    }
  }
  if (ambientSimpleType == 'B' && ambientRank == 7) {
    if (dynkinIndexOfSlTwo == 8) {
      return List<int>({1, 1, 1, 0, 0, 0, 0, 0, 0});
    }
    if (dynkinIndexOfSlTwo == 6) {
      return
      List<int>({1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,});
    }
  }
  if (ambientSimpleType == 'C' && ambientRank == 7) {
    if (dynkinIndexOfSlTwo == 12) {
      return List<int>({1, 1, 1, 0, 0, 0, 0, 0, 0});
    }
    if (dynkinIndexOfSlTwo == 7) {
      return CentralizerComputer::onesAtPositions(List<int>({0, 1, 4}), 21);
    }
  }
  if (ambientSimpleType == 'E' && ambientRank == 8) {
    if (dynkinIndexOfSlTwo == 2) {
      return
      List<int>({
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          1,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          1,
          9,
          13,
          17,
          25,
          2,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          1,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0
        });
    }
  }
  return List<int>({});
}
