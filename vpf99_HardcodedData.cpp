#include "vpfHeader2Math9_HardCodedData.h"
#include "vpfHeader2Math3_FiniteGroups.h"

static ProjectInformationInstance ProjectInfoVpfHardCodedLoader(__FILE__, "Hard-coded data loading");

void HardCodedData::LoadConjugacyClassesWeylGroupNoFail(WeylGroup& inputOutput)
{ if (!HardCodedData::LoadConjugacyClassesWeylGroup(inputOutput))
    crash << "Function LoadConjugacyClassesWeylGroupNoFail not allowed to fail. Input: " << inputOutput.theDynkinType.ToString() << crash;
}

template<class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::MakeFromReadableReflections
(WeylGroup& input, bool dontMakeCanonical_SET_TRUE_ON_YOUR_OWN_RISK, const std::string& inputReflections)
{ this->owner=&input;
  this->generatorsLastAppliedFirst.ReservE(input.RootsOfBorel.size/2); //<- this will work for most elements
  this->generatorsLastAppliedFirst.SetSize(0);
  int currentReflection=-1;
  //algorithm: if next symbol is not a digit, the current reflection is pushed on top of the list of reflections
  // else the new digit is added to the current reflection (if the currentReflection is -1, the digit replaces currentReflection, else
  //currentReflection is replaced by currentReflection*1+theDigit (we are working in the decimal system of course).
  int sizePlusOne=inputReflections.size()+1;
  for (int i=0; i< sizePlusOne; i++)
  { int theDigit=-1;
    char currentChar= i<(signed) inputReflections.size() ? inputReflections[i] : ')'; //<-forcing a terminating character (lazy programmer tolerance).
    if (!MathRoutines::isADigit(currentChar, &theDigit))
    { if (currentReflection!=-1)
        this->generatorsLastAppliedFirst.AddOnTop(currentReflection);
      currentReflection=-1;
      continue;
    }
    if (currentReflection==-1)
    { currentReflection=theDigit;
      continue;
    }
    currentReflection*=10;
    currentReflection+=theDigit;
  }
  if (!dontMakeCanonical_SET_TRUE_ON_YOUR_OWN_RISK)
    this->MakeCanonical();
}

bool LoadConjugacyClassesE1_6(WeylGroup& output)
{ output.ComputeRho(true);
  output.conjugacyClassRepresentatives.SetSize(25);
  output.conjugacyClassRepresentatives[0  ].MakeFromReadableReflections(output, true, "");
  output.conjugacyClassRepresentatives[1  ].MakeFromReadableReflections(output, true, "0");
  output.conjugacyClassRepresentatives[2  ].MakeFromReadableReflections(output, true, "1,2,3,1,2,3,4,3,1,2,3,4");
  output.conjugacyClassRepresentatives[3  ].MakeFromReadableReflections(output, true, "0,1,2,0,3,1,2,0,3,4,3,1,2,0,3,4,5,4,3,1,2,3,4,5");
  output.conjugacyClassRepresentatives[4  ].MakeFromReadableReflections(output, true, "0,2");
  output.conjugacyClassRepresentatives[5  ].MakeFromReadableReflections(output, true, "0,1");
  output.conjugacyClassRepresentatives[6  ].MakeFromReadableReflections(output, true, "0,2,4,5");
  output.conjugacyClassRepresentatives[7  ].MakeFromReadableReflections(output, true, "0,1,4");
  output.conjugacyClassRepresentatives[8  ].MakeFromReadableReflections(output, true, "1,2,3,1,4,3");
  output.conjugacyClassRepresentatives[9  ].MakeFromReadableReflections(output, true, "0,1,2,3,1,2,3,4,3,1,2,3,4");
  output.conjugacyClassRepresentatives[10 ].MakeFromReadableReflections(output, true, "0,1,2,0,3,1,2,3,4,3,5,4");
  output.conjugacyClassRepresentatives[11 ].MakeFromReadableReflections(output, true, "0,1,2");
  output.conjugacyClassRepresentatives[12 ].MakeFromReadableReflections(output, true, "1,2,3,4");
  output.conjugacyClassRepresentatives[13 ].MakeFromReadableReflections(output, true, "0,1,2,4,5");
  output.conjugacyClassRepresentatives[14 ].MakeFromReadableReflections(output, true, "0,1,2,3,1,2,3,4,3,1,2,3,4,5");
  output.conjugacyClassRepresentatives[15 ].MakeFromReadableReflections(output, true, "0,2,3");
  output.conjugacyClassRepresentatives[16 ].MakeFromReadableReflections(output, true, "0,1,2,4");
  output.conjugacyClassRepresentatives[17 ].MakeFromReadableReflections(output, true, "0,1,3,4");
  output.conjugacyClassRepresentatives[18 ].MakeFromReadableReflections(output, true, "0,2,3,4,5");
  output.conjugacyClassRepresentatives[19 ].MakeFromReadableReflections(output, true, "0,1,2,3,4,5");
  output.conjugacyClassRepresentatives[20 ].MakeFromReadableReflections(output, true, "0,1,2,3,1,4,3");
  output.conjugacyClassRepresentatives[21 ].MakeFromReadableReflections(output, true, "0,1,2,3");
  output.conjugacyClassRepresentatives[22 ].MakeFromReadableReflections(output, true, "0,1,2,3,5");
  output.conjugacyClassRepresentatives[23 ].MakeFromReadableReflections(output, true, "0,1,2,3,1,4,3,5");
  output.conjugacyClassRepresentatives[24 ].MakeFromReadableReflections(output, true, "0,1,2,3,4");
  output.conjugacyClassesSizes.SetSize(25);
  output.conjugacyClassesSizes[0  ]=1;
  output.conjugacyClassesSizes[1  ]=36;
  output.conjugacyClassesSizes[2  ]=45;
  output.conjugacyClassesSizes[3  ]=80;
  output.conjugacyClassesSizes[4  ]=240;
  output.conjugacyClassesSizes[5  ]=270;
  output.conjugacyClassesSizes[6  ]=480;
  output.conjugacyClassesSizes[7  ]=540;
  output.conjugacyClassesSizes[8  ]=540;
  output.conjugacyClassesSizes[9  ]=540;
  output.conjugacyClassesSizes[10 ]=720;
  output.conjugacyClassesSizes[11 ]=1440;
  output.conjugacyClassesSizes[12 ]=1440;
  output.conjugacyClassesSizes[13 ]=1440;
  output.conjugacyClassesSizes[14 ]=1440;
  output.conjugacyClassesSizes[15 ]=1620;
  output.conjugacyClassesSizes[16 ]=2160;
  output.conjugacyClassesSizes[17 ]=3240;
  output.conjugacyClassesSizes[18 ]=4320;
  output.conjugacyClassesSizes[19 ]=4320;
  output.conjugacyClassesSizes[20 ]=4320;
  output.conjugacyClassesSizes[21 ]=5184;
  output.conjugacyClassesSizes[22 ]=5184;
  output.conjugacyClassesSizes[23 ]=5760;
  output.conjugacyClassesSizes[24 ]=6480;
  return true;
}

bool LoadConjugacyClassesF1_4(WeylGroup& output)
{ output.ComputeRho(true);
  output.conjugacyClassRepresentatives.SetSize(25);
  output.conjugacyClassRepresentatives[0  ].MakeFromReadableReflections(output, true, "");
  output.conjugacyClassRepresentatives[1  ].MakeFromReadableReflections(output, true, "0,1,0,2,1,0,2,1,2,3,2,1,0,2,1,2,3,2,1,0,2,1,2,3");
  output.conjugacyClassRepresentatives[2  ].MakeFromReadableReflections(output, true, "0");
  output.conjugacyClassRepresentatives[3  ].MakeFromReadableReflections(output, true, "2");
  output.conjugacyClassRepresentatives[4  ].MakeFromReadableReflections(output, true, "0,1,0,2,1,0,2,1,2");
  output.conjugacyClassRepresentatives[5  ].MakeFromReadableReflections(output, true, "1,2,1,2,3,2,1,2,3");
  output.conjugacyClassRepresentatives[6  ].MakeFromReadableReflections(output, true, "0,1,0,2,1,0,2,3,2,1,2,3");
  output.conjugacyClassRepresentatives[7  ].MakeFromReadableReflections(output, true, "0,1,0,2,1,2,3,2");
  output.conjugacyClassRepresentatives[8  ].MakeFromReadableReflections(output, true, "0,1,0,2,1,0,2,3,2,1,0,2,1,2,3,2");
  output.conjugacyClassRepresentatives[9  ].MakeFromReadableReflections(output, true, "1,2,1,2");
  output.conjugacyClassRepresentatives[10 ].MakeFromReadableReflections(output, true, "0,1");
  output.conjugacyClassRepresentatives[11 ].MakeFromReadableReflections(output, true, "2,3");
  output.conjugacyClassRepresentatives[12 ].MakeFromReadableReflections(output, true, "0,1,0,2,1,0,2,1,2,3");
  output.conjugacyClassRepresentatives[13 ].MakeFromReadableReflections(output, true, "0,1,2,1,2,3,2,1,2,3");
  output.conjugacyClassRepresentatives[14 ].MakeFromReadableReflections(output, true, "1,2");
  output.conjugacyClassRepresentatives[15 ].MakeFromReadableReflections(output, true, "0,1,0,2,1,0,2,1,2,3,2,1,2,3");
  output.conjugacyClassRepresentatives[16 ].MakeFromReadableReflections(output, true, "0,2");
  output.conjugacyClassRepresentatives[17 ].MakeFromReadableReflections(output, true, "0,1,2,1,2");
  output.conjugacyClassRepresentatives[18 ].MakeFromReadableReflections(output, true, "1,2,1,2,3");
  output.conjugacyClassRepresentatives[19 ].MakeFromReadableReflections(output, true, "0,1,2");
  output.conjugacyClassRepresentatives[20 ].MakeFromReadableReflections(output, true, "0,1,3");
  output.conjugacyClassRepresentatives[21 ].MakeFromReadableReflections(output, true, "0,2,3");
  output.conjugacyClassRepresentatives[22 ].MakeFromReadableReflections(output, true, "1,2,3");
  output.conjugacyClassRepresentatives[23 ].MakeFromReadableReflections(output, true, "0,1,2,3");
  output.conjugacyClassRepresentatives[24 ].MakeFromReadableReflections(output, true, "0,1,2,1,2,3");
  output.conjugacyClassesSizes.SetSize(25);
  output.conjugacyClassesSizes[0  ]=1;
  output.conjugacyClassesSizes[1  ]=1;
  output.conjugacyClassesSizes[2  ]=12;
  output.conjugacyClassesSizes[3  ]=12;
  output.conjugacyClassesSizes[4  ]=12;
  output.conjugacyClassesSizes[5  ]=12;
  output.conjugacyClassesSizes[6  ]=12;
  output.conjugacyClassesSizes[7  ]=16;
  output.conjugacyClassesSizes[8  ]=16;
  output.conjugacyClassesSizes[9  ]=18;
  output.conjugacyClassesSizes[10 ]=32;
  output.conjugacyClassesSizes[11 ]=32;
  output.conjugacyClassesSizes[12 ]=32;
  output.conjugacyClassesSizes[13 ]=32;
  output.conjugacyClassesSizes[14 ]=36;
  output.conjugacyClassesSizes[15 ]=36;
  output.conjugacyClassesSizes[16 ]=72;
  output.conjugacyClassesSizes[17 ]=72;
  output.conjugacyClassesSizes[18 ]=72;
  output.conjugacyClassesSizes[19 ]=96;
  output.conjugacyClassesSizes[20 ]=96;
  output.conjugacyClassesSizes[21 ]=96;
  output.conjugacyClassesSizes[22 ]=96;
  output.conjugacyClassesSizes[23 ]=96;
  output.conjugacyClassesSizes[24 ]=144;
  return true;
}

bool HardCodedData::LoadConjugacyClassesWeylGroup(WeylGroup& inputOutput)
{ if (inputOutput.theDynkinType.ToString()=="F^{1}_4")
    return LoadConjugacyClassesF1_4(inputOutput);
  if (inputOutput.theDynkinType.ToString()=="E^{1}_6")
    return LoadConjugacyClassesE1_6(inputOutput);

  return false;
}



