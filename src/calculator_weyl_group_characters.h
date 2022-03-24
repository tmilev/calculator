// The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_calculator_weyl_group_character_ALREADY_INCLUDED
#define header_calculator_weyl_group_character_ALREADY_INCLUDED

#include "calculator.h"

class CalculatorFunctionsWeylGroup {
public:
  static bool weylOrbit(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool useFundCoords,
    bool useRho
  );
  static bool weylRaiseToMaximallyDominant(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool useOuter
  );
  static bool weylOuterRaiseToMaximallyDominant(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctionsWeylGroup::weylRaiseToMaximallyDominant(
      calculator, input, output, true
    );
  }
  static bool weylNoOuterRaiseToMaximallyDominant(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctionsWeylGroup::weylRaiseToMaximallyDominant(
      calculator, input, output, false
    );
  }
  static bool testSpechtModules(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool weylGroupOrbitSize(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool weylGroupOrbitFund(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool weylGroupOrbitSimple(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool weylGroupOrbitOuterSimple(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool weylGroupOrbitFundRho(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool weylGroupOuterAutoGeneratorsPrint(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool weylGroupConjugacyClasseS(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool weylGroupConjugacyClassesLoad(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool weylGroupConjugacyClassesFromAllElements(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool weylGroupOuterConjugacyClassesFromAllElements(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool weylGroupConjugacyClassesRepresentatives(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool spechtModule(
    Calculator& calculator, const Expression& input, Expression& out
  );
  static bool kostkaNumber(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool allSelectionsFixedRank(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool representElementHyperOctahedral(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool hyperOctahedralAllModulesInducedFromSpechtModules(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool hyperOctahedralGetOneRepresentation(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool hyperOctahedralPrintGeneratorCommutationRelations(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool weylGroupElement(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool lieAlgebraWeight(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool lieAlgebraRhoWeight(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool macdonaldPolys(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool weylGroupLoadOrComputeCharTable(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool weylGroupIrrepsAndCharTableComputeFromScratch(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool weylGroupNaturalRep(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool decomposeWeylRep(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool tensorWeylReps(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool tensorAndDecomposeWeylReps(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool makeVirtualWeylRep(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool isOuterAutoWeylGroup(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool signSignatureRootSubsystems(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool signSignatureRootSubsystemsFromKostkaNumbers(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool getElementWeylGroup(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

#endif // header_calculator_weyl_group_character_ALREADY_INCLUDED
