#ifndef vpfCharacterHeaderCalculatorInterface
#define vpfCharacterHeaderCalculatorInterface
#include "vpf.h"

static ProjectInformationInstance ProjectInfoVpfCharactersCalculatorInterfaceHeader(
  __FILE__, "Header, calculator built-in functions for Weyl groups."
);

class CalculatorFunctionsWeylGroup {
public:
  static bool innerWeylOrbit(Calculator& theCommands, const Expression& input, Expression& output, bool useFundCoords, bool useRho);
  static bool innerWeylRaiseToMaximallyDominant(Calculator& theCommands, const Expression& input, Expression& output, bool useOuter);
  static bool innerWeylOuterRaiseToMaximallyDominant(Calculator& theCommands, const Expression& input, Expression& output) {
    return CalculatorFunctionsWeylGroup::innerWeylRaiseToMaximallyDominant(theCommands, input, output, true);
  }
  static bool innerWeylNoOuterRaiseToMaximallyDominant(Calculator& theCommands, const Expression& input, Expression& output) {
    return CalculatorFunctionsWeylGroup::innerWeylRaiseToMaximallyDominant(theCommands, input, output, false);
  }
  static bool innerTestSpechtModules(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupOrbitSize(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupOrbitFund(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupOrbitSimple(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupOrbitOuterSimple(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupOrbitFundRho(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupOuterAutoGeneratorsPrint(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupConjugacyClasseS(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupConjugacyClassesLoad(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupConjugacyClassesFromAllElements(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupOuterConjugacyClassesFromAllElements(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupConjugacyClassesRepresentatives(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerSpechtModule(Calculator& theCommands, const Expression& input, Expression& out);
  static bool innerKostkaNumber(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAllSelectionsFixedRank(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerRepresentElementHyperOctahedral(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerHyperOctahedralAllModulesInducedFromSpechtModules(
    Calculator& theCommands, const Expression& input, Expression& output
  );
  static bool innerHyperOctahedralGetOneRepresentation(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerHyperOctahedralPrintGeneratorCommutationRelations(
    Calculator& theCommands, const Expression& input, Expression& output
  );

  static bool innerWeylGroupElement(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLieAlgebraWeight(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLieAlgebraRhoWeight(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMacdonaldPolys(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupLoadOrComputeCharTable(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupIrrepsAndCharTableComputeFromScratch(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupNaturalRep(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDecomposeWeylRep(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTensorWeylReps(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTensorAndDecomposeWeylReps(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMakeVirtualWeylRep(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIsOuterAutoWeylGroup(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSignSignatureRootSubsystems(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSignSignatureRootSubsystemsFromKostkaNumbers(Calculator& theCommands, const Expression& input, Expression& output);
};

#endif
