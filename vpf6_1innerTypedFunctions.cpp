//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
ProjectInformationInstance ProjectInfoVpf6_1cpp
(__FILE__, "Implementation file for the calculator parser: implementation of inner binary typed functions. ");


bool CommandList::innerAddRatToRat
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerAddRatToRat");
  if (input.children.size!=2)
    return false;
  Rational leftR, rightR;
  if (!input[0].IsOfType(&leftR) || !input[1].IsOfType(&rightR))
    return false;
  return output.AssignValue(leftR+rightR, theCommands);
}

bool CommandList::innerMultiplyRatByRat
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerMultiplyRatByRat");
  if (input.children.size!=2)
    return false;
  Rational leftR, rightR;
  if (!input[0].IsOfType(&leftR) || !input[1].IsOfType(&rightR))
    return false;
  return output.AssignValue(leftR*rightR, theCommands);
}

bool CommandList::innerDivideRatByRat
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerDivideRatByRat");
  if (input.children.size!=2)
    return false;
  Rational leftR, rightR;
  if (!input[0].IsOfType(&leftR) || !input[1].IsOfType(&rightR))
    return false;
  if (rightR.IsEqualToZero())
    return output.SetError("Division by zero.", theCommands);
  return output.AssignValue(leftR/rightR, theCommands);
}
