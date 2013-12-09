#include "vpfHeader2Math9_HardCodedData.h"
#include "vpfHeader2Math3_FiniteGroups.h"

static ProjectInformationInstance ProjectInfoVpfHardCodedLoader(__FILE__, "Hard-coded data loading");

void HardCodedData::LoadConjugacyClassesWeylGroupNoFail(WeylGroup& inputOutput)
{ if (!HardCodedData::LoadConjugacyClassesWeylGroup(inputOutput))
    crash << "Function LoadConjugacyClassesWeylGroupNoFail not allowed to fail. Input: " << inputOutput.theDynkinType.ToString() << crash;
}

bool LoadConjugacyClassesF4(WeylGroup& output)
{
  return true;
}

bool HardCodedData::LoadConjugacyClassesWeylGroup(WeylGroup& inputOutput)
{ if (inputOutput.theDynkinType.ToString()=="F^{1}_4")
    return false; //LoadConjugacyClassesF4(output);

  return false;
}



