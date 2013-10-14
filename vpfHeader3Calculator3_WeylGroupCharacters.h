#ifndef vpfCharacterHeaderCalculatorInterface
#define vpfCharacterHeaderCalculatorInterface
#include "vpf.h"

static ProjectInformationInstance ProjectInfoVpfCharactersCalculatorInterfaceHeader(__FILE__, "Header, calculator built-in functions for Weyl groups.");

class WeylGroupCalculatorFunctions
{
public:
  static bool innerWeylOrbit
(CommandList& theCommands, const Expression& input, Expression& output,
 bool useFundCoords, bool useRho)
 ;
  static bool innerWeylRaiseToMaximallyDominant
(CommandList& theCommands, const Expression& input, Expression& output, bool useOuter)
 ;
  static bool innerWeylOuterRaiseToMaximallyDominant
(CommandList& theCommands, const Expression& input, Expression& output)
 { return WeylGroupCalculatorFunctions::innerWeylRaiseToMaximallyDominant(theCommands, input, output, true);
 }
  static bool innerWeylNoOuterRaiseToMaximallyDominant
(CommandList& theCommands, const Expression& input, Expression& output)
 { return WeylGroupCalculatorFunctions::innerWeylRaiseToMaximallyDominant(theCommands, input, output, false);
 }
  static bool innerWeylGroupOrbitFund
(CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerWeylGroupOrbitSimple
(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupOrbitOuterSimple
(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupOrbitFundRho
(CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerWeylGroupOuterAutoGeneratorsPrint
(CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerWeylGroupConjugacyClasses
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerCoxeterElement
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerMacdonaldPolys
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerWeylGroupIrrepsAndCharTable
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerWeylGroupNaturalRep
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerDecomposeWeylRep
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerTensorWeylReps
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerTensorAndDecomposeWeylReps
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerMakeVirtualWeylRep
  (CommandList& theCommands, const Expression& input, Expression& output)
;
};

#endif
