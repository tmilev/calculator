// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfCharacterHeaderCalculatorInterface
#define vpfCharacterHeaderCalculatorInterface
#include "calculator.h"

class CalculatorFunctionsWeylGroup {
public:
  static bool innerWeylOrbit(Calculator& calculator, const Expression& input, Expression& output, bool useFundCoords, bool useRho);
  static bool innerWeylRaiseToMaximallyDominant(Calculator& calculator, const Expression& input, Expression& output, bool useOuter);
  static bool innerWeylOuterRaiseToMaximallyDominant(Calculator& calculator, const Expression& input, Expression& output) {
    return CalculatorFunctionsWeylGroup::innerWeylRaiseToMaximallyDominant(calculator, input, output, true);
  }
  static bool innerWeylNoOuterRaiseToMaximallyDominant(Calculator& calculator, const Expression& input, Expression& output) {
    return CalculatorFunctionsWeylGroup::innerWeylRaiseToMaximallyDominant(calculator, input, output, false);
  }
  static bool innerTestSpechtModules(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerWeylGroupOrbitSize(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerWeylGroupOrbitFund(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerWeylGroupOrbitSimple(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerWeylGroupOrbitOuterSimple(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerWeylGroupOrbitFundRho(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerWeylGroupOuterAutoGeneratorsPrint(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerWeylGroupConjugacyClasseS(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerWeylGroupConjugacyClassesLoad(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerWeylGroupConjugacyClassesFromAllElements(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerWeylGroupOuterConjugacyClassesFromAllElements(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerWeylGroupConjugacyClassesRepresentatives(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerSpechtModule(Calculator& calculator, const Expression& input, Expression& out);
  static bool innerKostkaNumber(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAllSelectionsFixedRank(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerRepresentElementHyperOctahedral(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerHyperOctahedralAllModulesInducedFromSpechtModules(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerHyperOctahedralGetOneRepresentation(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerHyperOctahedralPrintGeneratorCommutationRelations(
    Calculator& calculator, const Expression& input, Expression& output
  );

  static bool innerWeylGroupElement(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerLieAlgebraWeight(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerLieAlgebraRhoWeight(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMacdonaldPolys(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerWeylGroupLoadOrComputeCharTable(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerWeylGroupIrrepsAndCharTableComputeFromScratch(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerWeylGroupNaturalRep(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDecomposeWeylRep(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerTensorWeylReps(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerTensorAndDecomposeWeylReps(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMakeVirtualWeylRep(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsOuterAutoWeylGroup(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSignSignatureRootSubsystems(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSignSignatureRootSubsystemsFromKostkaNumbers(Calculator& calculator, const Expression& input, Expression& output);
};

#endif
