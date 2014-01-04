#include "vpfHeader2Math3_FiniteGroups.h"

static ProjectInformationInstance ProjectInfoVpfHardCodedLoader(__FILE__, "Hard-coded data loading");

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

void WeylGroup::ComputeOrLoadCharacterTable(GlobalVariables* theGlobalVariables, std::stringstream* reportStream)
{ MacroRegisterFunctionWithName("WeylGroup::ComputeOrLoadCharacterTable");
  if (this->flagCharTableIsComputed)
    return;
  this->ComputeOrLoadConjugacyClasses(theGlobalVariables, reportStream);
  if (this->LoadCharTableWeylGroup())
  { if (reportStream!=0)
      *reportStream << "The character table of " << this->theDynkinType.ToString()
      << " is precomputed (hard-coded in c++), loading directly. ";
    this->flagCharTableIsComputed=true;
    return;
  }
  this->ComputeIrreducibleRepresentationsTodorsVersion(theGlobalVariables);
  this->flagCharTableIsComputed=true;
}

void WeylGroup::ComputeOrLoadConjugacyClasses(GlobalVariables* theGlobalVariables, std::stringstream* reportStream)
{ MacroRegisterFunctionWithName("WeylGroup::ComputeOrLoadCharacterTable");
  if (this->flagCCRepresentativesComputed)
    return;
  if (this->LoadConjugacyClassesWeylGroup())
  { if (reportStream!=0)
      *reportStream << "The conjugacy classes of " << this->theDynkinType.ToString()
      << " are precomputed (hard-coded in c++), loading directly. ";
    this->flagCCRepresentativesComputed=true;
    return;
  }
  this->ComputeCCfromAllElements(theGlobalVariables);
  this->flagCCRepresentativesComputed=true;
}

bool LoadCharTableF1_4(WeylGroup& output)
{ output.characterTable.SetExpectedSize(1152); output.characterTable.SetSize(0);
  ClassFunction<Rational> currentCF;
  currentCF.G=&output;
  currentCF.data.AssignString("(1  , 1  , -1 , -1 , -1 , -1 , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , -1 , -1 , -1 , -1 , -1 , -1 , 1  , 1  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(1  , 1  , -1 , 1  , 1  , -1 , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , -1 , -1 , -1 , -1 , 1  , 1  , 1  , -1 , -1 , 1  , -1 )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(1  , 1  , 1  , -1 , -1 , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , -1 , -1 , -1 , 1  , -1 , -1 , -1 , 1  , 1  , 1  , -1 )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(2  , 2  , -2 , 0  , 0  , -2 , 2  , -1 , -1 , 2  , 2  , -1 , -1 , 2  , 0  , 0  , 0  , -2 , 0  , 0  , 0  , 1  , 1  , -1 , 0  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(2  , 2  , 0  , -2 , -2 , 0  , 2  , -1 , -1 , 2  , -1 , 2  , 2  , -1 , 0  , 0  , 0  , 0  , -2 , 1  , 1  , 0  , 0  , -1 , 0  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(2  , 2  , 0  , 2  , 2  , 0  , 2  , -1 , -1 , 2  , -1 , 2  , 2  , -1 , 0  , 0  , 0  , 0  , 2  , -1 , -1 , 0  , 0  , -1 , 0  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(2  , 2  , 2  , 0  , 0  , 2  , 2  , -1 , -1 , 2  , 2  , -1 , -1 , 2  , 0  , 0  , 0  , 2  , 0  , 0  , 0  , -1 , -1 , -1 , 0  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(4  , -4 , -2 , -2 , 2  , 2  , 0  , 2  , -2 , 0  , 1  , 1  , -1 , -1 , 2  , -2 , 0  , 0  , 0  , -1 , 1  , 1  , -1 , 0  , 0  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(4  , -4 , -2 , 2  , -2 , 2  , 0  , 2  , -2 , 0  , 1  , 1  , -1 , -1 , -2 , 2  , 0  , 0  , 0  , 1  , -1 , 1  , -1 , 0  , 0  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(4  , -4 , 2  , -2 , 2  , -2 , 0  , 2  , -2 , 0  , 1  , 1  , -1 , -1 , -2 , 2  , 0  , 0  , 0  , -1 , 1  , -1 , 1  , 0  , 0  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(4  , -4 , 2  , 2  , -2 , -2 , 0  , 2  , -2 , 0  , 1  , 1  , -1 , -1 , 2  , -2 , 0  , 0  , 0  , 1  , -1 , -1 , 1  , 0  , 0  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(4  , 4  , 0  , 0  , 0  , 0  , 4  , 1  , 1  , 4  , -2 , -2 , -2 , -2 , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 1  , 0  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(6  , 6  , 0  , 0  , 0  , 0  , 2  , 3  , 3  , -2 , 0  , 0  , 0  , 0  , -2 , -2 , 2  , 0  , 0  , 0  , 0  , 0  , 0  , -1 , 0  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(6  , 6  , 0  , 0  , 0  , 0  , 2  , 3  , 3  , -2 , 0  , 0  , 0  , 0  , 2  , 2  , -2 , 0  , 0  , 0  , 0  , 0  , 0  , -1 , 0  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(8  , -8 , -4 , 0  , 0  , 4  , 0  , -2 , 2  , 0  , 2  , -1 , 1  , -2 , 0  , 0  , 0  , 0  , 0  , 0  , 0  , -1 , 1  , 0  , 0  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(8  , -8 , 0  , -4 , 4  , 0  , 0  , -2 , 2  , 0  , -1 , 2  , -2 , 1  , 0  , 0  , 0  , 0  , 0  , 1  , -1 , 0  , 0  , 0  , 0  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(8  , -8 , 0  , 4  , -4 , 0  , 0  , -2 , 2  , 0  , -1 , 2  , -2 , 1  , 0  , 0  , 0  , 0  , 0  , -1 , 1  , 0  , 0  , 0  , 0  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(8  , -8 , 4  , 0  , 0  , -4 , 0  , -2 , 2  , 0  , 2  , -1 , 1  , -2 , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 1  , -1 , 0  , 0  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(9  , 9  , -3 , -3 , -3 , -3 , -3 , 0  , 0  , 1  , 0  , 0  , 0  , 0  , 1  , 1  , 1  , 1  , 1  , 0  , 0  , 0  , 0  , 0  , -1 )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(9  , 9  , -3 , 3  , 3  , -3 , -3 , 0  , 0  , 1  , 0  , 0  , 0  , 0  , -1 , -1 , -1 , 1  , -1 , 0  , 0  , 0  , 0  , 0  , 1  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(9  , 9  , 3  , -3 , -3 , 3  , -3 , 0  , 0  , 1  , 0  , 0  , 0  , 0  , -1 , -1 , -1 , -1 , 1  , 0  , 0  , 0  , 0  , 0  , 1  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(9  , 9  , 3  , 3  , 3  , 3  , -3 , 0  , 0  , 1  , 0  , 0  , 0  , 0  , 1  , 1  , 1  , -1 , -1 , 0  , 0  , 0  , 0  , 0  , -1 )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(12 , 12 , 0  , 0  , 0  , 0  , 4  , -3 , -3 , -4 , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 1  , 0  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(16 , -16, 0  , 0  , 0  , 0  , 0  , 2  , -2 , 0  , -2 , -2 , 2  , 2  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  )"); output.characterTable.AddOnTop(currentCF);
  return true;
}

bool LoadCharTableB1_3(WeylGroup& output)
{ output.characterTable.SetExpectedSize(48); output.characterTable.SetSize(0);
  ClassFunction<Rational> currentCF;
  currentCF.G=&output;
  currentCF.data.AssignString("(1  , -1 , -1 , 1  , -1 , 1  , 1  , -1 , 1  , -1 )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(1  , -1 , -1 , 1  , 1  , -1 , -1 , 1  , 1  , -1 )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(1  , 1  , 1  , 1  , -1 , -1 , -1 , -1 , 1  , 1  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(2  , -2 , -2 , 2  , 0  , 0  , 0  , 0  , -1 , 1  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(2  , 2  , 2  , 2  , 0  , 0  , 0  , 0  , -1 , -1 )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(3  , -3 , 1  , -1 , -1 , 1  , -1 , 1  , 0  , 0  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(3  , -3 , 1  , -1 , 1  , -1 , 1  , -1 , 0  , 0  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(3  , 3  , -1 , -1 , -1 , -1 , 1  , 1  , 0  , 0  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(3  , 3  , -1 , -1 , 1  , 1  , -1 , -1 , 0  , 0  )"); output.characterTable.AddOnTop(currentCF);
  return true;
}

bool WeylGroup::LoadConjugacyClassesWeylGroup()
{ if (this->theDynkinType.ToString()=="F^{1}_4")
    return false;
//    return LoadConjugacyClassesF1_4(*this);
  if (this->theDynkinType.ToString()=="E^{1}_6")
    return false;
//    return LoadConjugacyClassesE1_6(*this);

  return false;
}

bool WeylGroup::LoadCharTableWeylGroup()
{ if (this->theDynkinType.ToString()=="B^{1}_3")
    return LoadCharTableB1_3(*this);
  if (this->theDynkinType.ToString()=="F^{1}_4")
    return LoadCharTableF1_4(*this);
  return false;
}
