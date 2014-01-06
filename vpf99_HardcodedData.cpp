#include "vpfHeader2Math3_FiniteGroups.h"

static ProjectInformationInstance ProjectInfoVpfHardCodedLoader(__FILE__, "Hard-coded data loading");

template<class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::MakeFromReadableReflections
(WeylGroup& input, bool dontMakeCanonical_SET_TRUE_ON_YOUR_OWN_RISK, const std::string& inputReflections)
{ this->owner=&input;
  Vector<Rational> theReflections;
  theReflections.AssignString(inputReflections);
  this->generatorsLastAppliedFirst.SetSize(theReflections.size);
  for (int i=0; i<theReflections.size; i++)
  { if (!theReflections[i].IsSmallInteger(&this->generatorsLastAppliedFirst[i]))
      crash << "Bad reflection list." << crash;
    this->generatorsLastAppliedFirst[i]--;
    if (this->generatorsLastAppliedFirst[i]<0 || this->generatorsLastAppliedFirst[i]>=input.GetDim())
      crash << "Bad reflection index: " << this->generatorsLastAppliedFirst[i] << crash;
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
  output.conjugacyClasseS[1  ].representative.MakeFromReadableReflections(output, true, "1,2,1,3,2,1,3,2,3,4,3,2,1,3,2,3,4,3,2,1,3,2,3,4");
  output.conjugacyClasseS[2  ].representative.MakeFromReadableReflections(output, true, "1");
  output.conjugacyClasseS[3  ].representative.MakeFromReadableReflections(output, true, "3");
  output.conjugacyClasseS[4  ].representative.MakeFromReadableReflections(output, true, "1,2,1,3,2,1,3,2,3");
  output.conjugacyClasseS[5  ].representative.MakeFromReadableReflections(output, true, "2,3,2,3,4,3,2,3,4");
  output.conjugacyClasseS[6  ].representative.MakeFromReadableReflections(output, true, "3,2,4,3,2,1,3,2,4,3,2,1");
  output.conjugacyClasseS[7  ].representative.MakeFromReadableReflections(output, true, "3,2,4,3,2,1,3,2");
  output.conjugacyClasseS[8  ].representative.MakeFromReadableReflections(output, true, "3,2,1,4,3,2,1,3,2,3,4,3,2,1,3,2");
  output.conjugacyClasseS[9  ].representative.MakeFromReadableReflections(output, true, "2,3,2,3");
  output.conjugacyClasseS[10 ].representative.MakeFromReadableReflections(output, true, "2,1");
  output.conjugacyClasseS[11 ].representative.MakeFromReadableReflections(output, true, "4,3");
  output.conjugacyClasseS[12 ].representative.MakeFromReadableReflections(output, true, "1,2,1,4,3,2,1,3,2,3");
  output.conjugacyClasseS[13 ].representative.MakeFromReadableReflections(output, true, "2,3,2,3,4,3,2,1,3,4");
  output.conjugacyClasseS[14 ].representative.MakeFromReadableReflections(output, true, "3,2");
  output.conjugacyClasseS[15 ].representative.MakeFromReadableReflections(output, true, "2,3,2,3,4,3,2,1,3,2,4,3,2,1");
  output.conjugacyClasseS[16 ].representative.MakeFromReadableReflections(output, true, "1,3");
  output.conjugacyClasseS[17 ].representative.MakeFromReadableReflections(output, true, "2,3,2,1,3");
  output.conjugacyClasseS[18 ].representative.MakeFromReadableReflections(output, true, "2,4,3,2,3");
  output.conjugacyClasseS[19 ].representative.MakeFromReadableReflections(output, true, "3,2,1");
  output.conjugacyClasseS[20 ].representative.MakeFromReadableReflections(output, true, "2,1,4");
  output.conjugacyClasseS[21 ].representative.MakeFromReadableReflections(output, true, "1,4,3");
  output.conjugacyClasseS[22 ].representative.MakeFromReadableReflections(output, true, "4,3,2");
  output.conjugacyClasseS[23 ].representative.MakeFromReadableReflections(output, true, "4,3,2,1");
  output.conjugacyClasseS[24 ].representative.MakeFromReadableReflections(output, true, "2,4,3,2,1,3");
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

bool LoadConjugacyClassesE1_6(WeylGroup& output)
{ output.ComputeRho(true);
  WeylGroup::ConjugacyClass emptyClass;
  emptyClass.flagRepresentativeComputed=true;
  output.conjugacyClasseS.initFillInObject(25, emptyClass);
  output.conjugacyClasseS[0  ].representative.MakeFromReadableReflections(output, true, "   ");
  output.conjugacyClasseS[1  ].representative.MakeFromReadableReflections(output, true, "3, 4, 3, 2, 4, 3, 5, 4, 3, 2, 4, 5 ");
  output.conjugacyClasseS[2  ].representative.MakeFromReadableReflections(output, true, "1, 4 ");
  output.conjugacyClasseS[3  ].representative.MakeFromReadableReflections(output, true, "1, 3, 1, 4, 3, 1, 2, 4, 5, 4, 3, 1, 2, 4, 3, 5, 6, 5, 4, 3, 2, 4, 5, 6");
  output.conjugacyClasseS[4  ].representative.MakeFromReadableReflections(output, true, "1, 3 ");
  output.conjugacyClasseS[5  ].representative.MakeFromReadableReflections(output, true, "1, 3, 5, 6  ");
  output.conjugacyClasseS[6  ].representative.MakeFromReadableReflections(output, true, "3, 4, 3, 2, 4, 5  ");
  output.conjugacyClasseS[7  ].representative.MakeFromReadableReflections(output, true, "1, 4, 3, 6 ");
  output.conjugacyClasseS[8  ].representative.MakeFromReadableReflections(output, true, "1, 4, 3, 2  ");
  output.conjugacyClasseS[9  ].representative.MakeFromReadableReflections(output, true, "1, 2, 3, 1, 5, 4, 6, 5, 4, 2, 3, 4 ");
  output.conjugacyClasseS[10 ].representative.MakeFromReadableReflections(output, true, "3, 4, 2, 5  ");
  output.conjugacyClasseS[11 ].representative.MakeFromReadableReflections(output, true, "1, 2, 3, 4, 2, 3, 4, 6, 5, 4, 2, 3, 4, 5 ");
  output.conjugacyClasseS[12 ].representative.MakeFromReadableReflections(output, true, "1, 3, 2, 5 ");
  output.conjugacyClasseS[13 ].representative.MakeFromReadableReflections(output, true, "1, 3, 4, 3, 2, 4, 5, 6  ");
  output.conjugacyClasseS[14 ].representative.MakeFromReadableReflections(output, true, "1, 4, 6, 2, 3, 5 ");
  output.conjugacyClasseS[15 ].representative.MakeFromReadableReflections(output, true, "1 ");
  output.conjugacyClasseS[16 ].representative.MakeFromReadableReflections(output, true, "1, 4, 6  ");
  output.conjugacyClasseS[17 ].representative.MakeFromReadableReflections(output, true, "1, 3, 4, 3, 2, 4, 3, 5, 4, 3, 2, 4, 5 ");
  output.conjugacyClasseS[18 ].representative.MakeFromReadableReflections(output, true, "1, 4, 3  ");
  output.conjugacyClasseS[19 ].representative.MakeFromReadableReflections(output, true, "1, 3, 2  ");
  output.conjugacyClasseS[20 ].representative.MakeFromReadableReflections(output, true, "1, 3, 2, 5, 6  ");
  output.conjugacyClasseS[21 ].representative.MakeFromReadableReflections(output, true, "1, 4, 6, 3, 5  ");
  output.conjugacyClasseS[22 ].representative.MakeFromReadableReflections(output, true, "1, 3, 4, 2, 5  ");
  output.conjugacyClasseS[23 ].representative.MakeFromReadableReflections(output, true, "1, 4, 3, 2, 6  ");
  output.conjugacyClasseS[24 ].representative.MakeFromReadableReflections(output, true, "1, 4, 2, 5, 4, 2, 3 ");
  output.conjugacyClasseS[0  ].size=1   ;
  output.conjugacyClasseS[1  ].size=45  ;
  output.conjugacyClasseS[2  ].size=270 ;
  output.conjugacyClasseS[3  ].size=80  ;
  output.conjugacyClasseS[4  ].size=240 ;
  output.conjugacyClasseS[5  ].size=480 ;
  output.conjugacyClasseS[6  ].size=540 ;
  output.conjugacyClasseS[7  ].size=3240;
  output.conjugacyClasseS[8  ].size=5184;
  output.conjugacyClasseS[9  ].size=720 ;
  output.conjugacyClasseS[10 ].size=1440;
  output.conjugacyClasseS[11 ].size=1440;
  output.conjugacyClasseS[12 ].size=2160;
  output.conjugacyClasseS[13 ].size=5760;
  output.conjugacyClasseS[14 ].size=4320;
  output.conjugacyClasseS[15 ].size=36  ;
  output.conjugacyClasseS[16 ].size=540 ;
  output.conjugacyClasseS[17 ].size=540 ;
  output.conjugacyClasseS[18 ].size=1620;
  output.conjugacyClasseS[19 ].size=1440;
  output.conjugacyClasseS[20 ].size=1440;
  output.conjugacyClasseS[21 ].size=4320;
  output.conjugacyClasseS[22 ].size=6480;
  output.conjugacyClasseS[23 ].size=5184;
  output.conjugacyClasseS[24 ].size=4320;
  output.LoadConjugacyClassesHelper();
  return true;
}

bool LoadCharTableE1_6(WeylGroup& output)
{ output.characterTable.SetExpectedSize(48); output.characterTable.SetSize(0);
  ClassFunction<Rational> currentCF;
  currentCF.G=&output;
  currentCF.data.AssignString("(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1              )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1    )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(10, -6, 2, 1, -2, 4, 2, -2, 0, -3, 0, 0, 2, 1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0        )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(6, -2, 2, -3, 3, 0, 2, 0, 1, 1, 1, -2, -1, 0, -1, 4, 0, -2, 2, 1, -2, 0,0, -1, 1       )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(6, -2, 2, -3, 3, 0, 2, 0, 1, 1, 1, -2, -1, 0, -1, -4, 0, 2, -2, -1, 2, 0,0, 1, -1      )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(20, 4, -4, -7, 2, 2, 4, 0, 0, 1, -2, -2, 2, -1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0        )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(15, -1, -1, 6, 3, 0, 3, -1, 0, 2, -1, 2, -1, 0, 0, 5, -3, 1, 1, -1, 2, 0,-1, 0, 1      )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(15, -1, -1, 6, 3, 0, 3, -1, 0, 2, -1, 2, -1, 0, 0, -5, 3, -1, -1, 1, -2, 0, 1, 0, -1   )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(15, 7, 3, -3, 0, 3, -1, 1, 0, 1, -2, 1, 0, 0, -1, 5, 1, 3, -1, 2, -1, 1, -1, 0, 0      )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(15, 7, 3, -3, 0, 3, -1, 1, 0, 1, -2, 1, 0, 0, -1, -5, -1, -3, 1, -2, 1, -1, 1, 0, 0    )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(20, 4, 4, 2, 5, -1, 0, 0, 0, -2, 1, 1, 1, -1, 0, 10, 2, 2, 2, 1, 1, -1, 0, 0, -1       )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(20, 4, 4, 2, 5, -1, 0, 0, 0, -2, 1, 1, 1, -1, 0, -10, -2, -2, -2, -1, -1,1, 0, 0, 1    )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(24, 8, 0, 6, 0, 3, 0, 0, -1, 2, 2, -1, 0, 0, 0, 4, 4, 0, 0, -2, 1, 1, 0, -1, 0         )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(24, 8, 0, 6, 0, 3, 0, 0, -1, 2, 2, -1, 0, 0, 0, -4, -4, 0, 0, 2, -1, -1, 0, 1, 0       )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(30, -10, 2, 3, 3, 3, -2, 0, 0, -1, -1, -1, -1, 0, 1, 10, -2, -4, 0, 1, 1,1, 0, 0, -1   )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(30, -10, 2, 3, 3, 3, -2, 0, 0, -1, -1, -1, -1, 0, 1, -10, 2, 4, 0, -1, -1, -1, 0, 0, 1 )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(60, 12, 4, -3, -6, 0, 4, 0, 0, -3, 0, 0, -2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0        )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(80, -16, 0, -10, -4, 2, 0, 0, 0, 2, 2, 2, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0       )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(90, -6, -6, 9, 0, 0, 2, 2, 0, -3, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0,0, 0          )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(60, -4, 4, 6, -3, -3, 0, 0, 0, 2, -1, -1, 1, 0, 0, 10, 2, -2, -2, 1, 1, -1, 0, 0, 1    )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(60, -4, 4, 6, -3, -3, 0, 0, 0, 2, -1, -1, 1, 0, 0, -10, -2, 2, 2, -1, -1, 1, 0, 0, -1  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(64, 0, 0, -8, 4, -2, 0, 0, -1, 0, 0, 0, 0, 1, 0, 16, 0, 0, 0, -2, -2, 0, 0, 1, 0       )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(64, 0, 0, -8, 4, -2, 0, 0, -1, 0, 0, 0, 0, 1, 0, -16, 0, 0, 0, 2, 2, 0, 0, -1, 0       )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(81, 9, -3, 0, 0, 0, -3, -1, 1, 0, 0, 0, 0, 0, 0, 9, -3, 3, -1, 0, 0, 0, 1, -1, 0       )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(81, 9, -3, 0, 0, 0, -3, -1, 1, 0, 0, 0, 0, 0, 0, -9, 3, -3, 1, 0, 0, 0, -1, 1, 0       )"); output.characterTable.AddOnTop(currentCF);
  return true;
}

bool WeylGroup::LoadConjugacyClassesHelper()
{ this->sizePrivate=0;
  for (int i=0; i<this->conjugacyClasseS.size; i++)
    this->sizePrivate+=this->conjugacyClasseS[i].size;
  if (this->sizePrivate!=this->GetGroupSizeByFormula())
    crash << "Corrupt hard-coded data: the size of the group does not work out according to formula. " << crash;
  this->flagCCRepresentativesComputed=true;
  this->flagCCsComputed=false;
  this->InitGenerators();
  return true;
}

bool WeylGroup::LoadConjugacyClasses()
{ if (this->theDynkinType.ToString()=="F^{1}_4")
    return LoadConjugacyClassesF1_4(*this);
  if (this->theDynkinType.ToString()=="E^{1}_6")
    return LoadConjugacyClassesE1_6(*this);

  return false;
}

bool WeylGroup::LoadCharTable()
{ if (this->theDynkinType.ToString()=="F^{1}_4")
    return LoadCharTableF1_4(*this);
  if (this->theDynkinType.ToString()=="E^{1}_6")
    return LoadCharTableE1_6(*this);
  return false;
}
