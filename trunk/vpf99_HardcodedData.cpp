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
  if (this->LoadCharTable())
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
  if (this->LoadConjugacyClasses())
  { if (reportStream!=0)
      *reportStream << "The conjugacy classes of " << this->theDynkinType.ToString()
      << " are precomputed (hard-coded in c++), loading directly. ";
    this->flagCCRepresentativesComputed=true;
    return;
  }
  this->ComputeCCfromAllElements(theGlobalVariables);
  this->flagCCRepresentativesComputed=true;
}

bool LoadConjugacyClassesF1_4(WeylGroup& output)
{ output.ComputeRho(true);
  WeylGroup::ConjugacyClass emptyClass;
  emptyClass.flagRepresentativeComputed=true;
  output.conjugacyClasseS.initFillInObject(25, emptyClass);
  output.conjugacyClasseS[0  ].representative.MakeFromReadableReflections(output, true, "");
  output.conjugacyClasseS[1  ].representative.MakeFromReadableReflections(output, true, "0,1,0,2,1,0,2,1,2,3,2,1,0,2,1,2,3,2,1,0,2,1,2,3");
  output.conjugacyClasseS[2  ].representative.MakeFromReadableReflections(output, true, "0");
  output.conjugacyClasseS[3  ].representative.MakeFromReadableReflections(output, true, "2");
  output.conjugacyClasseS[4  ].representative.MakeFromReadableReflections(output, true, "0,1,0,2,1,0,2,1,2");
  output.conjugacyClasseS[5  ].representative.MakeFromReadableReflections(output, true, "1,2,1,2,3,2,1,2,3");
  output.conjugacyClasseS[6  ].representative.MakeFromReadableReflections(output, true, "2,1,3,2,1,0,2,1,3,2,1,0");
  output.conjugacyClasseS[7  ].representative.MakeFromReadableReflections(output, true, "2,1,3,2,1,0,2,1");
  output.conjugacyClasseS[8  ].representative.MakeFromReadableReflections(output, true, "2,1,0,3,2,1,0,2,1,2,3,2,1,0,2,1");
  output.conjugacyClasseS[9  ].representative.MakeFromReadableReflections(output, true, "1,2,1,2");
  output.conjugacyClasseS[10 ].representative.MakeFromReadableReflections(output, true, "1,0");
  output.conjugacyClasseS[11 ].representative.MakeFromReadableReflections(output, true, "3,2");
  output.conjugacyClasseS[12 ].representative.MakeFromReadableReflections(output, true, "0,1,0,3,2,1,0,2,1,2");
  output.conjugacyClasseS[13 ].representative.MakeFromReadableReflections(output, true, "1,2,1,2,3,2,1,0,2,3");
  output.conjugacyClasseS[14 ].representative.MakeFromReadableReflections(output, true, "2,1");
  output.conjugacyClasseS[15 ].representative.MakeFromReadableReflections(output, true, "1,2,1,2,3,2,1,0,2,1,3,2,1,0");
  output.conjugacyClasseS[16 ].representative.MakeFromReadableReflections(output, true, "0,2");
  output.conjugacyClasseS[17 ].representative.MakeFromReadableReflections(output, true, "1,2,1,0,2");
  output.conjugacyClasseS[18 ].representative.MakeFromReadableReflections(output, true, "1,3,2,1,2");
  output.conjugacyClasseS[19 ].representative.MakeFromReadableReflections(output, true, "2,1,0");
  output.conjugacyClasseS[20 ].representative.MakeFromReadableReflections(output, true, "1,0,3");
  output.conjugacyClasseS[21 ].representative.MakeFromReadableReflections(output, true, "0,3,2");
  output.conjugacyClasseS[22 ].representative.MakeFromReadableReflections(output, true, "3,2,1");
  output.conjugacyClasseS[23 ].representative.MakeFromReadableReflections(output, true, "3,2,1,0");
  output.conjugacyClasseS[24 ].representative.MakeFromReadableReflections(output, true, "1,3,2,1,0,2");
  output.conjugacyClasseS[0  ].size=1;
  output.conjugacyClasseS[1  ].size=1;
  output.conjugacyClasseS[2  ].size=12;
  output.conjugacyClasseS[3  ].size=12;
  output.conjugacyClasseS[4  ].size=12;
  output.conjugacyClasseS[5  ].size=12;
  output.conjugacyClasseS[6  ].size=12;
  output.conjugacyClasseS[7  ].size=16;
  output.conjugacyClasseS[8  ].size=16;
  output.conjugacyClasseS[9  ].size=18;
  output.conjugacyClasseS[10 ].size=32;
  output.conjugacyClasseS[11 ].size=32;
  output.conjugacyClasseS[12 ].size=32;
  output.conjugacyClasseS[13 ].size=32;
  output.conjugacyClasseS[14 ].size=36;
  output.conjugacyClasseS[15 ].size=36;
  output.conjugacyClasseS[16 ].size=72;
  output.conjugacyClasseS[17 ].size=72;
  output.conjugacyClasseS[18 ].size=72;
  output.conjugacyClasseS[19 ].size=96;
  output.conjugacyClasseS[20 ].size=96;
  output.conjugacyClasseS[21 ].size=96;
  output.conjugacyClasseS[22 ].size=96;
  output.conjugacyClasseS[23 ].size=96;
  output.conjugacyClasseS[24 ].size=144;
  output.LoadConjugacyClassesHelper();
  return true;
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

bool WeylGroup::LoadConjugacyClassesHelper()
{ this->sizePrivate=0;
  for (int i=0; i<this->conjugacyClasseS.size; i++)
    this->sizePrivate+=this->conjugacyClasseS[i].size;
  this->flagCCRepresentativesComputed=true;
  this->flagCCsComputed=false;
  this->InitGenerators();
  return true;
}

bool WeylGroup::LoadConjugacyClasses()
{ if (this->theDynkinType.ToString()=="F^{1}_4")
    //return false;
    return LoadConjugacyClassesF1_4(*this);
  if (this->theDynkinType.ToString()=="E^{1}_6")
    return false;
//    return LoadConjugacyClassesE1_6(*this);

  return false;
}

bool WeylGroup::LoadCharTable()
{ if (this->theDynkinType.ToString()=="B^{1}_3")
    return LoadCharTableB1_3(*this);
  if (this->theDynkinType.ToString()=="F^{1}_4")
    return LoadCharTableF1_4(*this);
  return false;
}
