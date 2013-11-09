#ifndef vpfCharacterHeaderCalculatorInterface
#define vpfCharacterHeaderCalculatorInterface
#include "vpf.h"

static ProjectInformationInstance ProjectInfoVpfCharactersCalculatorInterfaceHeader(__FILE__, "Header, calculator built-in functions for Weyl groups.");

class CalculatorFunctionsWeylGroup
{
public:
  static bool innerWeylOrbit(Calculator& theCommands, const Expression& input, Expression& output, bool useFundCoords, bool useRho);
  static bool innerWeylRaiseToMaximallyDominant(Calculator& theCommands, const Expression& input, Expression& output, bool useOuter);
  static bool innerWeylOuterRaiseToMaximallyDominant(Calculator& theCommands, const Expression& input, Expression& output)
 { return CalculatorFunctionsWeylGroup::innerWeylRaiseToMaximallyDominant(theCommands, input, output, true);
 }
  static bool innerWeylNoOuterRaiseToMaximallyDominant(Calculator& theCommands, const Expression& input, Expression& output)
 { return CalculatorFunctionsWeylGroup::innerWeylRaiseToMaximallyDominant(theCommands, input, output, false);
 }
  static bool innerWeylGroupOrbitFund(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupOrbitSimple(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupOrbitOuterSimple(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupOrbitFundRho(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupOuterAutoGeneratorsPrint(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupConjugacyClasses(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCoxeterElement(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMacdonaldPolys(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupIrrepsAndCharTable(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupNaturalRep(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDecomposeWeylRep(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTensorWeylReps(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTensorAndDecomposeWeylReps(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMakeVirtualWeylRep(Calculator& theCommands, const Expression& input, Expression& output);
};

#endif
