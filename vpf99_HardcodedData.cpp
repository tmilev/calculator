#include "vpfImplementationHeader2Math3_FiniteGroups.h"

static ProjectInformationInstance ProjectInfoVpfHardCodedLoader(__FILE__, "Hard-coded data loading");

template<class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::MakeFromReadableReflections
(WeylGroup& input, bool dontMakeCanonical_SET_TRUE_ON_YOUR_OWN_RISK, const std::string& inputReflections)
{ this->owner=&input;
  Vector<Rational> theReflections;
  theReflections.AssignString(inputReflections);
  this->generatorsLastAppliedFirst.SetSize(theReflections.size);
//  std::cout << "<br>Read reflection from " << inputReflections << " to get: " << theReflections.ToString();
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
  output.ccCarterLabels.SetSize(0);
  output.ccCarterLabels.AddOnTop(" "         );
  output.ccCarterLabels.AddOnTop("4A_1"      );
  output.ccCarterLabels.AddOnTop("2A_1"      );
  output.ccCarterLabels.AddOnTop("A_2"       );
  output.ccCarterLabels.AddOnTop("D_4"       );
  output.ccCarterLabels.AddOnTop("D_4(a_1)"  );
  output.ccCarterLabels.AddOnTop("A^2_2"     );
  output.ccCarterLabels.AddOnTop("C_3+A_1"   );
  output.ccCarterLabels.AddOnTop("A_2+A^2_2" );
  output.ccCarterLabels.AddOnTop("F_4(a_1)"  );
  output.ccCarterLabels.AddOnTop("F_4"       );
  output.ccCarterLabels.AddOnTop("A_1"       );
  output.ccCarterLabels.AddOnTop("3A_1"      );
  output.ccCarterLabels.AddOnTop("A^2_2+A_1" );
  output.ccCarterLabels.AddOnTop("C_3"       );
  output.ccCarterLabels.AddOnTop("A_3"       );
  output.ccCarterLabels.AddOnTop("A^2_1"      );
  output.ccCarterLabels.AddOnTop("2A_1+A^2_1");
  output.ccCarterLabels.AddOnTop("A_2+A^2_1" );
  output.ccCarterLabels.AddOnTop("B_3"       );
  output.ccCarterLabels.AddOnTop("B_2+A_1"   );
  output.ccCarterLabels.AddOnTop("A^2_1+A_1" );
  output.ccCarterLabels.AddOnTop("B_2"       );
  output.ccCarterLabels.AddOnTop("A_3+A^2_1" );
  output.ccCarterLabels.AddOnTop("B_4"       );

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
  ClassFunction<FiniteGroup<ElementWeylGroup<WeylGroup> >, Rational> currentCF;
  currentCF.G=&output;


  currentCF.data.AssignString("[  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, -1, -1, -1, -1, -1,  1,  1,  1,  1,  1, -1, -1, -1, -1 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  1,  1,  1,  1 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[  2,  2,  2,  2,  2,  2, -1, -1, -1, -1, -1,  2,  2, -1, -1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[  2,  2,  2,  2,  2,  2, -1, -1, -1, -1, -1, -2, -2,  1,  1, -2,  0,  0,  0,  0,  0,  0,  0,  0,  0 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[  2,  2,  2, -1, -1,  2,  2,  2, -1, -1, -1,  0,  0,  0,  0,  0, -2, -2,  1,  1, -2,  0,  0,  0,  0 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[  4,  4,  4, -2, -2,  4, -2, -2,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[  9,  9,  1,  0,  0, -3,  0,  0,  0,  0,  0,  3,  3,  0,  0, -1,  3,  3,  0,  0, -1,  1,  1,  1, -1 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[  2,  2,  2, -1, -1,  2,  2,  2, -1, -1, -1,  0,  0,  0,  0,  0,  2,  2, -1, -1,  2,  0,  0,  0,  0 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[  9,  9,  1,  0,  0, -3,  0,  0,  0,  0,  0,  3,  3,  0,  0, -1, -3, -3,  0,  0,  1, -1, -1, -1,  1 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[  9,  9,  1,  0,  0, -3,  0,  0,  0,  0,  0, -3, -3,  0,  0,  1,  3,  3,  0,  0, -1, -1, -1, -1,  1 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[  9,  9,  1,  0,  0, -3,  0,  0,  0,  0,  0, -3, -3,  0,  0,  1, -3, -3,  0,  0,  1,  1,  1,  1, -1 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[  6,  6, -2,  0,  0,  2,  0,  0,  3,  3, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2, -2, -2,  0 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[  6,  6, -2,  0,  0,  2,  0,  0,  3,  3, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -2,  2,  2,  0 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[ 12, 12, -4,  0,  0,  4,  0,  0, -3, -3,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[  4, -4,  0,  1, -1,  0,  1, -1, -2,  2,  0,  2, -2, -1,  1,  0,  2, -2, -1,  1,  0,  0,  2, -2,  0 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[  4, -4,  0,  1, -1,  0,  1, -1, -2,  2,  0,  2, -2, -1,  1,  0, -2,  2,  1, -1,  0,  0, -2,  2,  0 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[  4, -4,  0,  1, -1,  0,  1, -1, -2,  2,  0, -2,  2,  1, -1,  0,  2, -2, -1,  1,  0,  0, -2,  2,  0 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[  4, -4,  0,  1, -1,  0,  1, -1, -2,  2,  0, -2,  2,  1, -1,  0, -2,  2,  1, -1,  0,  0,  2, -2,  0 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[  8, -8,  0,  2, -2,  0, -1,  1,  2, -2,  0,  4, -4,  1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[  8, -8,  0,  2, -2,  0, -1,  1,  2, -2,  0, -4,  4, -1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[  8, -8,  0, -1,  1,  0,  2, -2,  2, -2,  0,  0,  0,  0,  0,  0,  4, -4,  1, -1,  0,  0,  0,  0,  0 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[  8, -8,  0, -1,  1,  0,  2, -2,  2, -2,  0,  0,  0,  0,  0,  0, -4,  4, -1,  1,  0,  0,  0,  0,  0 ]"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("[ 16, -16, 0, -2,  2,  0, -2,  2, -2,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 ]"); output.characterTable.AddOnTop(currentCF);
  output.irrepsCarterLabels.SetSize(0);
  output.irrepsCarterLabels.AddOnTop("\\phi_{1,0}   ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{1,12}''");
  output.irrepsCarterLabels.AddOnTop("\\phi_{1,12}' ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{1,24}  ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{2,4}'' ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{2,16}' ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{2,4}'  ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{2,16}''");
  output.irrepsCarterLabels.AddOnTop("\\phi_{4,8}   ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{9,2}   ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{9,6}'' ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{9,6}'  ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{9,10}  ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{6,6}'  ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{6,6}'' ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{12,4}  ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{4,1}   ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{4,7}'' ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{4,7}'  ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{4,13}  ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{8,3}'' ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{8,9}'  ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{8,3}'  ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{8,9}'' ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{16,5}  ");
  return true;
}

bool LoadConjugacyClassesG1_2(WeylGroup& output)
{ output.ComputeRho(true);
  WeylGroup::ConjugacyClass emptyClass;
  emptyClass.flagRepresentativeComputed=true;
  output.conjugacyClasseS.initFillInObject(6, emptyClass);
  output.conjugacyClasseS[0  ].representative.MakeFromReadableReflections(output, false, "");
  output.conjugacyClasseS[1  ].representative.MakeFromReadableReflections(output, false, "1");
  output.conjugacyClasseS[2  ].representative.MakeFromReadableReflections(output, false, "2");
  output.conjugacyClasseS[3  ].representative.MakeFromReadableReflections(output, false, "2,1");
  output.conjugacyClasseS[4  ].representative.MakeFromReadableReflections(output, false, "2,1,2,1");
  output.conjugacyClasseS[5  ].representative.MakeFromReadableReflections(output, false, "1,2,1,2,1,2");
  output.conjugacyClasseS[0  ].size=1;
  output.conjugacyClasseS[1  ].size=3;
  output.conjugacyClasseS[2  ].size=3;
  output.conjugacyClasseS[3  ].size=2;
  output.conjugacyClasseS[4  ].size=2;
  output.conjugacyClasseS[5  ].size=1;
  output.ccCarterLabels.SetSize(0);
  output.ccCarterLabels.AddOnTop("0");
  output.ccCarterLabels.AddOnTop("A^{3}_1");
  output.ccCarterLabels.AddOnTop("A_1");
  output.ccCarterLabels.AddOnTop("G_2");
  output.ccCarterLabels.AddOnTop("A_2");
  output.ccCarterLabels.AddOnTop("A^{3}_1+A_1");
  output.LoadConjugacyClassesHelper();
  return true;
}

bool LoadCharTableG1_2(WeylGroup& output)
{ output.characterTable.SetExpectedSize(12); output.characterTable.SetSize(0);
  ClassFunction<FiniteGroup<ElementWeylGroup<WeylGroup> >, Rational> currentCF;
  currentCF.G=&output;
  currentCF.data.AssignString("(1  , -1 , -1 , 1  , 1  , 1  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(1  , -1 , 1  , -1 , 1  , -1 )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(1  , 1  , -1 , -1 , 1  , -1 )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(1  , 1  , 1  , 1  , 1  , 1  )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(2  , 0  , 0  , 1  , -1 , -2 )"); output.characterTable.AddOnTop(currentCF);
  currentCF.data.AssignString("(2  , 0  , 0  , -1 , -1 , 2  )"); output.characterTable.AddOnTop(currentCF);
  output.irrepsCarterLabels.SetSize(0);
  output.irrepsCarterLabels.AddOnTop("\\phi_{1,6}  ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{1,3}''");
  output.irrepsCarterLabels.AddOnTop("\\phi_{1,3}' ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{1,0}  ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{2,1}  ");
  output.irrepsCarterLabels.AddOnTop("\\phi_{2,2}  ");
  return true;
}

bool LoadConjugacyClassesE1_6(WeylGroup& output)
{ output.ComputeRho(true);
  WeylGroup::ConjugacyClass emptyClass;
  emptyClass.flagRepresentativeComputed=true;
  output.conjugacyClasseS.initFillInObject(25, emptyClass);
  output.conjugacyClasseS[0  ].representative.MakeFromReadableReflections(output, false, "   ");
  output.conjugacyClasseS[1  ].representative.MakeFromReadableReflections(output, false, "3, 4, 3, 2, 4, 3, 5, 4, 3, 2, 4, 5 ");
  output.conjugacyClasseS[2  ].representative.MakeFromReadableReflections(output, false, "1, 4 ");
  output.conjugacyClasseS[3  ].representative.MakeFromReadableReflections(output, false, "1, 3, 1, 4, 3, 1, 2, 4, 5, 4, 3, 1, 2, 4, 3, 5, 6, 5, 4, 3, 2, 4, 5, 6");
  output.conjugacyClasseS[4  ].representative.MakeFromReadableReflections(output, false, "1, 3 ");
  output.conjugacyClasseS[5  ].representative.MakeFromReadableReflections(output, false, "1, 3, 5, 6  ");
  output.conjugacyClasseS[6  ].representative.MakeFromReadableReflections(output, false, "3, 4, 3, 2, 4, 5  ");
  output.conjugacyClasseS[7  ].representative.MakeFromReadableReflections(output, false, "1, 4, 3, 6 ");
  output.conjugacyClasseS[8  ].representative.MakeFromReadableReflections(output, false, "1, 4, 3, 2  ");
  output.conjugacyClasseS[9  ].representative.MakeFromReadableReflections(output, false, "1, 2, 3, 1, 5, 4, 6, 5, 4, 2, 3, 4 ");
  output.conjugacyClasseS[10 ].representative.MakeFromReadableReflections(output, false, "3, 4, 2, 5  ");
  output.conjugacyClasseS[11 ].representative.MakeFromReadableReflections(output, false, "1, 2, 3, 4, 2, 3, 4, 6, 5, 4, 2, 3, 4, 5 ");
  output.conjugacyClasseS[12 ].representative.MakeFromReadableReflections(output, false, "1, 3, 2, 5 ");
  output.conjugacyClasseS[13 ].representative.MakeFromReadableReflections(output, false, "1, 3, 4, 3, 2, 4, 5, 6  ");
  output.conjugacyClasseS[14 ].representative.MakeFromReadableReflections(output, false, "1, 4, 6, 2, 3, 5 ");
  output.conjugacyClasseS[15 ].representative.MakeFromReadableReflections(output, false, "1 ");
  output.conjugacyClasseS[16 ].representative.MakeFromReadableReflections(output, false, "1, 4, 6  ");
  output.conjugacyClasseS[17 ].representative.MakeFromReadableReflections(output, false, "1, 3, 4, 3, 2, 4, 3, 5, 4, 3, 2, 4, 5 ");
  output.conjugacyClasseS[18 ].representative.MakeFromReadableReflections(output, false, "1, 4, 3  ");
  output.conjugacyClasseS[19 ].representative.MakeFromReadableReflections(output, false, "1, 3, 2  ");
  output.conjugacyClasseS[20 ].representative.MakeFromReadableReflections(output, false, "1, 3, 2, 5, 6  ");
  output.conjugacyClasseS[21 ].representative.MakeFromReadableReflections(output, false, "1, 4, 6, 3, 5  ");
  output.conjugacyClasseS[22 ].representative.MakeFromReadableReflections(output, false, "1, 3, 4, 2, 5  ");
  output.conjugacyClasseS[23 ].representative.MakeFromReadableReflections(output, false, "1, 4, 3, 2, 6  ");
  output.conjugacyClasseS[24 ].representative.MakeFromReadableReflections(output, false, "1, 4, 2, 5, 4, 2, 3 ");

  output.ccCarterLabels.SetSize(0);
  output.ccCarterLabels.AddOnTop("0"        );
  output.ccCarterLabels.AddOnTop("4A_1"     );
  output.ccCarterLabels.AddOnTop("2A_1"     );
  output.ccCarterLabels.AddOnTop("3A_2"     );
  output.ccCarterLabels.AddOnTop("A_2"      );
  output.ccCarterLabels.AddOnTop("2A_2"     );
  output.ccCarterLabels.AddOnTop("D_4(a_1)" );
  output.ccCarterLabels.AddOnTop("A_3+A_1"  );
  output.ccCarterLabels.AddOnTop("A_4"      );
  output.ccCarterLabels.AddOnTop("E_6(a_2)" );
  output.ccCarterLabels.AddOnTop("D_4"      );
  output.ccCarterLabels.AddOnTop("A_5+A_1"  );
  output.ccCarterLabels.AddOnTop("A_2+2A_1" );
  output.ccCarterLabels.AddOnTop("E_6(a_1)" );
  output.ccCarterLabels.AddOnTop("E_6"      );
  output.ccCarterLabels.AddOnTop("A_1"      );
  output.ccCarterLabels.AddOnTop("3A_1"     );
  output.ccCarterLabels.AddOnTop("A_3+2A_1" );
  output.ccCarterLabels.AddOnTop("A_3"      );
  output.ccCarterLabels.AddOnTop("A_2+A_1"  );
  output.ccCarterLabels.AddOnTop("2A_2+A_1" );
  output.ccCarterLabels.AddOnTop("A_5"      );
  output.ccCarterLabels.AddOnTop("D_5"      );
  output.ccCarterLabels.AddOnTop("A_4+A_1"  );
  output.ccCarterLabels.AddOnTop("D_5(a_1)" );

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
  ClassFunction<FiniteGroup<ElementWeylGroup<WeylGroup> >, Rational> currentCF;
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
  output.irrepsCarterLabels.SetSize(0);
  output.irrepsCarterLabels.AddOnTop("\\phi_{1,0}  " );
  output.irrepsCarterLabels.AddOnTop("\\phi_{1,36} " );
  output.irrepsCarterLabels.AddOnTop("\\phi_{10,9} " );
  output.irrepsCarterLabels.AddOnTop("\\phi_{6,1}  " );
  output.irrepsCarterLabels.AddOnTop("\\phi_{6,25} " );
  output.irrepsCarterLabels.AddOnTop("\\phi_{20,10}" );
  output.irrepsCarterLabels.AddOnTop("\\phi_{15,5} " );
  output.irrepsCarterLabels.AddOnTop("\\phi_{15,17}" );
  output.irrepsCarterLabels.AddOnTop("\\phi_{15,4} " );
  output.irrepsCarterLabels.AddOnTop("\\phi_{15,16}" );
  output.irrepsCarterLabels.AddOnTop("\\phi_{20,2} " );
  output.irrepsCarterLabels.AddOnTop("\\phi_{20,20}" );
  output.irrepsCarterLabels.AddOnTop("\\phi_{24,6} " );
  output.irrepsCarterLabels.AddOnTop("\\phi_{24,12}" );
  output.irrepsCarterLabels.AddOnTop("\\phi_{30,3} " );
  output.irrepsCarterLabels.AddOnTop("\\phi_{30,15}" );
  output.irrepsCarterLabels.AddOnTop("\\phi_{60,8} " );
  output.irrepsCarterLabels.AddOnTop("\\phi_{80,7} " );
  output.irrepsCarterLabels.AddOnTop("\\phi_{90,8} " );
  output.irrepsCarterLabels.AddOnTop("\\phi_{60,5} " );
  output.irrepsCarterLabels.AddOnTop("\\phi_{60,11}" );
  output.irrepsCarterLabels.AddOnTop("\\phi_{64,4} " );
  output.irrepsCarterLabels.AddOnTop("\\phi_{64,13}" );
  output.irrepsCarterLabels.AddOnTop("\\phi_{81,6} " );
  output.irrepsCarterLabels.AddOnTop("\\phi_{81,10}" );
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
  this->CheckConjugacyClassRepsMatchCCsizes(0);
  return true;
}

bool WeylGroup::LoadConjugacyClasses()
{ if (this->theDynkinType.ToString()=="G^{1}_2")
    return LoadConjugacyClassesG1_2(*this);
  if (this->theDynkinType.ToString()=="F^{1}_4")
    return LoadConjugacyClassesF1_4(*this);
  if (this->theDynkinType.ToString()=="E^{1}_6")
    return LoadConjugacyClassesE1_6(*this);

  return false;
}

bool WeylGroup::LoadCharTable()
{ if (this->theDynkinType.ToString()=="G^{1}_2")
    return LoadCharTableG1_2(*this);
  if (this->theDynkinType.ToString()=="F^{1}_4")
    return LoadCharTableF1_4(*this);
  if (this->theDynkinType.ToString()=="E^{1}_6")
    return LoadCharTableE1_6(*this);
  return false;
}

bool LoadGAPRootSystemE1_6(HashedList<Vector<Rational> >& outputRootSystem)
{ outputRootSystem.Clear();
  Vector<Rational> theRoot;
  theRoot.AssignString("[ 1, 0, 0, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 0, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 1, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 0, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 0, 0, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 0, 1, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 0, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 1, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 0, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 0, 1, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 0, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 1, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 0, 1, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 0, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 1, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 0, 1, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 2, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 2, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 2, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 2, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 2, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 2, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 2, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 2, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 2, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 2, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  return true;
}

bool LoadGAPRootSystemE1_7(HashedList<Vector<Rational> >& outputRootSystem)
{ outputRootSystem.Clear();
  Vector<Rational> theRoot;
  theRoot.AssignString("[ 1, 0, 0, 0, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 0, 0, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 1, 0, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 1, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 0, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 0, 0, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 0, 0, 0, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 0, 1, 0, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 0, 1, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 1, 1, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 1, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 0, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 0, 0, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 0, 1, 1, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 1, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 0, 1, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 1, 1, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 1, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 0, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 1, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 0, 1, 1, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 1, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 0, 1, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 1, 1, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 1, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 1, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 0, 1, 1, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 2, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 1, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 0, 1, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 1, 1, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 2, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 1, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 0, 1, 1, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 2, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 1, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 2, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 2, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 1, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 2, 2, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 2, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 2, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 2, 2, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 2, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 2, 2, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 2, 2, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 2, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 2, 2, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 2, 2, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 3, 2, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 2, 2, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 2, 2, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 2, 3, 2, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 3, 2, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 2, 2, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 2, 3, 2, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 3, 2, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 2, 3, 2, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 3, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 2, 3, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 2, 4, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 3, 4, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 2, 2, 3, 4, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  return true;
}

bool LoadGAPRootSystemE1_8(HashedList<Vector<Rational> >& outputRootSystem)
{ outputRootSystem.Clear();
  Vector<Rational> theRoot;
  theRoot.AssignString("[ 1, 0, 0, 0, 0, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 0, 0, 0, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 1, 0, 0, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 1, 0, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 0, 1, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 0, 0, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 0, 0, 0, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 0, 0, 0, 0, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 0, 1, 0, 0, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 0, 1, 0, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 1, 1, 0, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 1, 1, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 0, 1, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 0, 0, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 0, 0, 0, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 0, 1, 1, 0, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 1, 0, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 0, 1, 1, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 1, 1, 1, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 1, 1, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 0, 1, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 0, 0, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 1, 0, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 0, 1, 1, 1, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 1, 1, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 0, 1, 1, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 1, 1, 1, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 1, 1, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 0, 1, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 1, 1, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 0, 1, 1, 1, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 2, 1, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 1, 1, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 0, 1, 1, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 1, 1, 1, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 0, 1, 1, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 2, 1, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 1, 1, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 0, 1, 1, 1, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 2, 1, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 1, 1, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 0, 1, 1, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 0, 1, 1, 1, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 2, 1, 0, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 2, 1, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 1, 1, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 0, 1, 1, 1, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 2, 2, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 2, 1, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 1, 1, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 2, 1, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 2, 2, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 2, 1, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 1, 1, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 2, 2, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 2, 1, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 2, 2, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 2, 1, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 2, 2, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 2, 1, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 2, 2, 2, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 2, 2, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 3, 2, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 2, 2, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 2, 1, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 2, 2, 2, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 2, 2, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 2, 2, 2, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 2, 3, 2, 1, 0, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 3, 2, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 2, 2, 2, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 2, 2, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 2, 2, 2, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 0, 1, 1, 2, 2, 2, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 2, 3, 2, 1, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 3, 2, 2, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 3, 2, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 2, 2, 2, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 1, 2, 2, 2, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 2, 3, 2, 2, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 2, 3, 2, 1, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 3, 3, 2, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 3, 2, 2, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 2, 2, 2, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 2, 3, 3, 2, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 2, 3, 2, 2, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 3, 3, 2, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 3, 2, 2, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 2, 4, 3, 2, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 2, 3, 3, 2, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 2, 3, 2, 2, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 3, 3, 2, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 3, 4, 3, 2, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 2, 4, 3, 2, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 2, 3, 3, 2, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 1, 2, 3, 3, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 2, 2, 3, 4, 3, 2, 1, 0 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 3, 4, 3, 2, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 2, 4, 3, 2, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 2, 3, 3, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 2, 2, 3, 4, 3, 2, 1, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 3, 4, 3, 2, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 2, 4, 3, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 2, 2, 3, 4, 3, 2, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 3, 4, 3, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 2, 4, 4, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 2, 2, 3, 4, 3, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 3, 4, 4, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 2, 2, 3, 4, 4, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 2, 3, 5, 4, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 2, 2, 3, 5, 4, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 1, 3, 3, 5, 4, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 2, 3, 3, 5, 4, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 2, 2, 4, 5, 4, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 2, 3, 4, 5, 4, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 2, 3, 4, 6, 4, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 2, 3, 4, 6, 5, 3, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 2, 3, 4, 6, 5, 4, 2, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 2, 3, 4, 6, 5, 4, 3, 1 ]"); outputRootSystem.AddOnTop(theRoot);
  theRoot.AssignString("[ 2, 3, 4, 6, 5, 4, 3, 2 ]"); outputRootSystem.AddOnTop(theRoot);
  return true;
}

bool WeylGroup::LoadGAPRootSystem(HashedList<Vector<Rational> >& outputPositiveRootSystem)const
{ bool result=false;
  if (this->theDynkinType.IsOfSimpleType('E', 8))
    result=LoadGAPRootSystemE1_8(outputPositiveRootSystem);
  if (this->theDynkinType.IsOfSimpleType('E', 7))
    result=LoadGAPRootSystemE1_7(outputPositiveRootSystem);
  if (this->theDynkinType.IsOfSimpleType('E', 6))
    result=LoadGAPRootSystemE1_6(outputPositiveRootSystem);
  if (!result)
    return result;
  if (outputPositiveRootSystem.size!=this->RootsOfBorel.size)
    crash << "Wrong number of GAP roots!" << crash;
  for (int i=0; i<this->RootsOfBorel.size; i++)
    if (!outputPositiveRootSystem.Contains(this->RootsOfBorel[i]))
      crash << " Positive root " << outputPositiveRootSystem[i].ToString() << " is not a GAP root. " << crash;
  return true;
}
