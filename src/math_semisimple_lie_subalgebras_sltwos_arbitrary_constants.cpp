#include "general_file_operations_encodings.h"
#include "math_extra_finite_groups_implementation.h"
#include "math_extra_semisimple_lie_algebras.h"
#include "math_extra_semisimple_lie_subalgebras_sltwos.h"
#include "math_general_polynomial_computations_advanced_implementation.h" // IWYU pragma: keep: breaks g++ -02 optimization build.
#include "progress_report.h"

Rational SlTwoSubalgebraCandidate::fArbitraryCoefficient(
  int coefficientIndex, char type, int rank, int dynkinIndex
) {
  List<Rational> arbitraryCoefficients =
  SlTwoSubalgebraCandidate::fArbitraryCoefficients(type, rank, dynkinIndex);
  if (coefficientIndex < arbitraryCoefficients.size) {
    return arbitraryCoefficients[coefficientIndex];
  }
  return coefficientIndex * coefficientIndex + 1;
}

List<Rational> SlTwoSubalgebraCandidate::fArbitraryCoefficients(
  char type, int rank, int dynkinIndex
) {
  // Coefficients found by manual experimentation with the computation
  // end-to-end.
  // Do not work in all cases, found out by quick computational experiments.
  if (type == 'C' && rank == 4 && dynkinIndex == 4) {
    return List<Rational>({1, 1, - 1, 1});
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
    if (dynkinIndex == 6) {
      return List<Rational>({1, 1, 1, 1, 1, 1});
    }
    if (dynkinIndex == 2) {
      return List<Rational>({1, 1, 1, 1, 1, 1});
    }
  }
  if (type == 'E' && rank == 6) {
    if (dynkinIndex == 2) {
      return List<Rational>({1, 1, 1, 1, 1, 1});
    }
  }
  return List<Rational>({1, - 1, 2, - 2, 3, - 3, 4, - 4});
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
    if (dynkinIndexOfSlTwo == 4) {
      return List<int>({1, 3});
    }
    if (dynkinIndexOfSlTwo == 3) {
      return List<int>({1, 3, 1});
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
  return List<int>({});
}
