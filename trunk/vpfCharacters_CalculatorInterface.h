#ifndef vpfCharacterHeaderCalculatorInterface
#define vpfCharacterHeaderCalculatorInterface
#include "vpf.h"

static ProjectInformationInstance ProjectInfoVpfCharactersCalculatorInterfaceHeader
(__FILE__, "Header file Weyl group calculator interface.");

class WeylGroupCalculatorFunctions{
public:
  static bool innerWeylOrbit
(CommandList& theCommands, const Expression& input, Expression& output,
 bool useFundCoords, bool useRho)
 ;
  static bool innerWeylGroupOrbitFund
(CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerWeylGroupOrbitSimple
(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerWeylGroupOrbitFundRho
(CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerWeylGroupConjugacyClasses
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerCoxeterElement
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerWeylGroupIrrepsAndCharTable
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerWeylGroupNaturalRep
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerDecomposeProductWeylIrreps
  (CommandList& theCommands, const Expression& input, Expression& output)
;
};

#endif
