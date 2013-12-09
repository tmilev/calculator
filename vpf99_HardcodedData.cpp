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
  return true;
}

bool HardCodedData::LoadConjugacyClassesWeylGroup(WeylGroup& inputOutput)
{ if (inputOutput.theDynkinType.ToString()=="F^{1}_4")
    return LoadConjugacyClassesF1_4(inputOutput);

  return false;
}



