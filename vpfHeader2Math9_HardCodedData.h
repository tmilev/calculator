#ifndef vpfHeaderHardCodedDataLoaderClass
#define vpfHeaderHardCodedDataLoaderClass
#include "vpfMacros.h"
static ProjectInformationInstance ProjectInfoHeaderHardCodedData(__FILE__, "Header, root and sl2 subalgebras of semisimple Lie algebras. ");

class DynkinType;
class WeylGroup;

class HardCodedData
{
public:
  static void LoadConjugacyClassesWeylGroupNoFail(WeylGroup& inputOutput);
  static bool LoadConjugacyClassesWeylGroup(WeylGroup& inputOutput);
};

#endif
