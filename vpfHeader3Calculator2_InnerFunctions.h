//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader6_2InnerFns
#define vpfHeader6_2InnerFns
#include "vpfHeader3Calculator0_Interface.h"

static ProjectInformationInstance ProjectInfoVpf6_2Header
(__FILE__, "Math calculator routines . ");

class CommandListFunctions
{
public:
  static bool innerGenerateVectorSpaceClosedWRTLieBracket(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerFourierTransformEWA(CommandList& theCommands, const Expression& input, Expression& output);

};

#endif

