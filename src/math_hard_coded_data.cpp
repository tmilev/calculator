#include "math_extra_finite_groups_implementation.h"
#include "math_general_polynomial_computations_advanced_implementation.h"
#include "math_extra_symmetric_groups_and_generalizations.h"
#include "json.h"

void ElementWeylGroup::makeFromReadableReflections(
  WeylGroupData& input, bool dontmakeCanonical_SET_TRUE_ON_YOUR_OWN_RISK, const std::string& inputReflections
) {
  this->owner = &input;
  Vector<Rational> theReflections;
  theReflections.assignString(inputReflections);
  this->generatorsLastAppliedFirst.setSize(theReflections.size);
  for (int i = 0; i < theReflections.size; i ++) {
    if (!theReflections[i].isSmallInteger(&this->generatorsLastAppliedFirst[i].index)) {
      global.fatal << "Bad reflection list." << global.fatal;
    }
    this->generatorsLastAppliedFirst[i].index --;
    if (this->generatorsLastAppliedFirst[i].index < 0 || this->generatorsLastAppliedFirst[i].index >= input.getDimension()) {
      global.fatal << "Bad reflection index: " << this->generatorsLastAppliedFirst[i].toString() << global.fatal;
    }
  }
  if (!dontmakeCanonical_SET_TRUE_ON_YOUR_OWN_RISK) {
    this->makeCanonical();
  }
}

void WeylGroupData::computeOrLoadCharacterTable(std::stringstream* reportStream) {
  MacroRegisterFunctionWithName("WeylGroup::computeOrLoadCharacterTable");
  if (this->flagCharTableIsComputed) {
    return;
  }
  this->computeOrLoadConjugacyClasses(reportStream);
  if (this->loadCharacterTable()) {
    if (reportStream != nullptr) {
      *reportStream << "The character table of " << this->theDynkinType.toString()
      << " is precomputed (hard-coded in c++), loading directly. ";
    }
    this->flagCharTableIsComputed = true;
    return;
  }
  if (this->theGroup.ComputeIrreducibleRepresentationsWithFormulas) {
    this->theGroup.ComputeIrreducibleRepresentationsWithFormulas(this->theGroup);
  }
  if (this->theGroup.irreps.size < this->theGroup.conjugacyClasses.size) {
    this->computeInitialIrreducibleRepresentations();
    this->theGroup.computeIrreducibleRepresentationsTodorsVersion();
  }
  // this flag should already be set
  this->flagCharTableIsComputed = true;
}

void WeylGroupData::computeOrLoadConjugacyClasses(std::stringstream* reportStream) {
  MacroRegisterFunctionWithName("WeylGroupData::computeOrLoadConjugacyClasses");
  if (this->theGroup.flagCCRepresentativesComputed) {
    return;
  }
  if (this->loadConjugacyClasses()) {
    if (reportStream != nullptr) {
      *reportStream << "The conjugacy classes of "
      << this->theDynkinType.toString()
      << " are precomputed (hard-coded in c++), loading directly. ";
    }
    this->theGroup.flagCCRepresentativesComputed = true;
    return;
  }
  this->theGroup.computeConjugacyClassesFromAllElements();
  this->theGroup.flagCCRepresentativesComputed = true;
}

bool LoadConjugacyClassesF1_4(WeylGroupData& output) {
  output.computeRho(true);
  FiniteGroup<ElementWeylGroup>::ConjugacyClass theClass;
  theClass.flagRepresentativeComputed = true;
  output.theGroup.conjugacyClasses.setSize(0);
  theClass.representative.makeFromReadableReflections(output, false, "[  ]                                                                      "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 1, 2, 1, 3, 2, 1, 3, 2, 3, 4, 3, 2, 1, 3, 2, 3, 4, 3, 2, 1, 3, 2, 3, 4 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 3, 2, 3 ]                                                            "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 1 ]                                                                  "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 3, 2, 3, 4, 3, 2, 1, 3, 4 ]                                          "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 3, 2, 4, 3, 2, 1, 3, 2, 4, 3, 2, 1 ]                                    "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 4, 3 ]                                                                  "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 1, 2, 1, 4, 3, 2, 1, 3, 2, 3 ]                                          "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 3, 2, 1, 4, 3, 2, 1, 3, 2, 3, 4, 3, 2, 1, 3, 2 ]                        "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 3, 2, 4, 3, 2, 1, 3, 2 ]                                                "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 4, 3, 2, 1 ]                                                            "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 1 ]                                                                     "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 3, 2, 3, 4, 3, 2, 3, 4 ]                                             "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 1, 4, 3 ]                                                               "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 4, 3, 2 ]                                                               "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 3, 2, 1, 3 ]                                                         "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 3 ]                                                                     "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 1, 2, 1, 3, 2, 1, 3, 2, 3 ]                                             "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 1, 4 ]                                                               "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 3, 2, 1 ]                                                               "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 4, 3, 2, 3 ]                                                         "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 1, 3 ]                                                                  "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 3, 2 ]                                                                  "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 3, 2, 3, 4, 3, 2, 1, 3, 2, 4, 3, 2, 1 ]                              "); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 4, 3, 2, 1, 3 ]                                                      "); output.theGroup.conjugacyClasses.addOnTop(theClass);

  output.ccCarterLabels.setSize(0);
  output.ccCarterLabels.addOnTop(" "         );
  output.ccCarterLabels.addOnTop("4A_1"      );
  output.ccCarterLabels.addOnTop("2A_1"      );
  output.ccCarterLabels.addOnTop("A_2"       );
  output.ccCarterLabels.addOnTop("D_4"       );
  output.ccCarterLabels.addOnTop("D_4(a_1)"  );
  output.ccCarterLabels.addOnTop("A^2_2"     );
  output.ccCarterLabels.addOnTop("C_3+A_1"   );
  output.ccCarterLabels.addOnTop("A_2+A^2_2" );
  output.ccCarterLabels.addOnTop("F_4(a_1)"  );
  output.ccCarterLabels.addOnTop("F_4"       );
  output.ccCarterLabels.addOnTop("A_1"       );
  output.ccCarterLabels.addOnTop("3A_1"      );
  output.ccCarterLabels.addOnTop("A^2_2+A_1" );
  output.ccCarterLabels.addOnTop("C_3"       );
  output.ccCarterLabels.addOnTop("A_3"       );
  output.ccCarterLabels.addOnTop("A^2_1"      );
  output.ccCarterLabels.addOnTop("2A_1+A^2_1");
  output.ccCarterLabels.addOnTop("A_2+A^2_1" );
  output.ccCarterLabels.addOnTop("B_3"       );
  output.ccCarterLabels.addOnTop("B_2+A_1"   );
  output.ccCarterLabels.addOnTop("A^2_1+A_1" );
  output.ccCarterLabels.addOnTop("B_2"       );
  output.ccCarterLabels.addOnTop("A_3+A^2_1" );
  output.ccCarterLabels.addOnTop("B_4"       );
  int i = - 1;
  i ++; output.theGroup.conjugacyClasses[i].size = 1  ;
  i ++; output.theGroup.conjugacyClasses[i].size = 1  ;
  i ++; output.theGroup.conjugacyClasses[i].size = 18 ;
  i ++; output.theGroup.conjugacyClasses[i].size = 32 ;
  i ++; output.theGroup.conjugacyClasses[i].size = 32 ;
  i ++; output.theGroup.conjugacyClasses[i].size = 12 ;
  i ++; output.theGroup.conjugacyClasses[i].size = 32 ;
  i ++; output.theGroup.conjugacyClasses[i].size = 32 ;
  i ++; output.theGroup.conjugacyClasses[i].size = 16 ;
  i ++; output.theGroup.conjugacyClasses[i].size = 16 ;
  i ++; output.theGroup.conjugacyClasses[i].size = 96 ;
  i ++; output.theGroup.conjugacyClasses[i].size = 12 ;
  i ++; output.theGroup.conjugacyClasses[i].size = 12 ;
  i ++; output.theGroup.conjugacyClasses[i].size = 96 ;
  i ++; output.theGroup.conjugacyClasses[i].size = 96 ;
  i ++; output.theGroup.conjugacyClasses[i].size = 72 ;
  i ++; output.theGroup.conjugacyClasses[i].size = 12 ;
  i ++; output.theGroup.conjugacyClasses[i].size = 12 ;
  i ++; output.theGroup.conjugacyClasses[i].size = 96 ;
  i ++; output.theGroup.conjugacyClasses[i].size = 96 ;
  i ++; output.theGroup.conjugacyClasses[i].size = 72 ;
  i ++; output.theGroup.conjugacyClasses[i].size = 72 ;
  i ++; output.theGroup.conjugacyClasses[i].size = 36 ;
  i ++; output.theGroup.conjugacyClasses[i].size = 36 ;
  i ++; output.theGroup.conjugacyClasses[i].size = 144;
  output.loadConjugacyClassesHelper();
  return true;
}

bool LoadConjugacyClassesE1_7(WeylGroupData& output) {
  output.computeRho(true);
  FiniteGroup<ElementWeylGroup>::ConjugacyClass theClass;
  theClass.flagRepresentativeComputed = true;
  output.theGroup.conjugacyClasses.setSize(0);
  theClass.representative.makeFromReadableReflections(output, false, "[ ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 7, 5, 6, 7, 4, 5, 6, 7, 2, 4, 5, 6, 7, 3, 4, 5, 6, 7, 2, 4, 5, 6, 3, 4, 5, 2, 4, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 5, 4, 5, 2, 4, 5, 3, 4, 5, 2, 4, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 2, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 6, 5, 6, 4, 5, 6, 2, 4, 3, 4, 5, 6, 2, 4, 5, 3, 1, 3, 4, 5, 2, 4, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 4, 2 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 5, 4, 5, 2, 4, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 6, 2 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 4, 5, 2, 4, 5, 3, 4, 5, 2, 4, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 7, 5, 6, 4, 5, 2, 4, 5, 3, 4, 5, 2, 4, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 6, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 5, 4 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 5, 4, 5, 2, 4, 5, 3, 4, 5, 2, 4, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 5, 4, 2, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 1, 2, 3, 1, 5, 4, 6, 5, 4, 2, 3, 4 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 4, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 7, 5, 6, 4, 5, 2, 4, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 1, 2, 3, 4, 2, 3, 4, 6, 5, 4, 2, 3, 4, 5 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 2, 6, 4, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 5, 4, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 2, 3, 4, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 3, 4, 2, 3, 4, 7, 6, 5 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 6, 5, 4, 5, 2, 4, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 5, 4, 2, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 6, 2, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 4, 5, 2, 4, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 6, 4, 1, 5, 3, 2 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 4, 2, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 7, 5, 6, 7, 4, 5, 6, 7, 2, 4, 5, 6, 7, 3, 4, 5, 6, 7, 2, 4, 5, 6, 3, 4, 5, 2, 4, 3, 1, 3, 4, 5, 6, 7, 2, 4, 5, 6, 3, 4, 5, 2, 4, 3, 1, 3, 4, 5, 6, 7, 2, 4, 5, 6, 3, 4, 5, 2, 4, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 2 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 4, 5, 2, 4, 5, 3, 4, 5, 2, 4, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 7, 5, 6, 7, 4, 5, 6, 7, 2, 4, 5, 6, 7, 3, 4, 5, 6, 7, 2, 4, 5, 6, 3, 4, 5, 2, 4, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 7, 5, 6, 7, 4, 5, 6, 2, 4, 3, 4, 5, 2, 4, 3, 1, 3, 4, 2 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 7, 5, 6, 4, 5, 6, 2, 4, 5, 6, 3, 4, 5, 6, 1, 3, 4, 5, 2, 4, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 7, 5, 6, 7, 4, 5, 6, 7, 2, 4, 5, 6, 7, 3, 4, 5, 6, 7, 2, 4, 5, 6, 3, 1, 3, 4, 5, 2, 4, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 6, 5, 4, 5, 2, 4, 5, 3, 4, 5, 2, 4, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 6 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 4, 5, 2, 4, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 6, 2, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 5, 4, 5, 2, 4, 5, 3, 4, 5, 2, 4, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 4 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 2, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 5, 6, 4, 5, 6, 2, 4, 3, 4, 5, 6, 2, 4, 5, 3, 1, 3, 4, 5, 2, 4, 3, 1]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 4, 2, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 4, 2, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 2, 6, 4 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 3, 6, 4 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 5, 4, 5, 2, 4, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 6, 5, 4, 2, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 7, 5, 6, 4, 5, 2, 4, 5, 3, 4, 5, 2, 4, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 5, 4, 2, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 6, 4, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 3, 4, 2, 3, 4, 6, 5 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 6, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 7, 5, 6, 4, 5, 2, 4, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 4, 2, 3, 5, 4, 2, 7, 6, 5, 4, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);

  output.ccCarterLabels.setSize(0);
  output.ccCarterLabels.addOnTop(" "              );
  output.ccCarterLabels.addOnTop("6A_1"           );
  output.ccCarterLabels.addOnTop("4A_1''"         );
  output.ccCarterLabels.addOnTop("2A_1"           );
  output.ccCarterLabels.addOnTop("4A_1'"          );
  output.ccCarterLabels.addOnTop("A_2"            );
  output.ccCarterLabels.addOnTop("3A_2"           );
  output.ccCarterLabels.addOnTop("2A_2"           );
  output.ccCarterLabels.addOnTop("D_4(a_1)"       );
  output.ccCarterLabels.addOnTop("A_3+A_1'"       );
  output.ccCarterLabels.addOnTop("A_3+3A_1"       );
  output.ccCarterLabels.addOnTop("D_4(a_1) + 2A_1");
  output.ccCarterLabels.addOnTop("A_3+A_1''"      );
  output.ccCarterLabels.addOnTop("A_4"            );
  output.ccCarterLabels.addOnTop("D_4+2A_1"       );
  output.ccCarterLabels.addOnTop("D_4"            );
  output.ccCarterLabels.addOnTop("E_6(a_2)"       );
  output.ccCarterLabels.addOnTop("A_2+2A_1"       );
  output.ccCarterLabels.addOnTop("D_6(a_2)"       );
  output.ccCarterLabels.addOnTop("A_5+A_1''"      );
  output.ccCarterLabels.addOnTop("A_5+A_1'"       );
  output.ccCarterLabels.addOnTop("A_6"            );
  output.ccCarterLabels.addOnTop("D_5+A_1"        );
  output.ccCarterLabels.addOnTop("D_6(a_1)"       );
  output.ccCarterLabels.addOnTop("E_6(a_1)"       );
  output.ccCarterLabels.addOnTop("D_6"            );
  output.ccCarterLabels.addOnTop("A_3+A_2+A_1"    );
  output.ccCarterLabels.addOnTop("D_5(a_1) +A_1"   );
  output.ccCarterLabels.addOnTop("E_6"            );
  output.ccCarterLabels.addOnTop("A_4+A_2"        );
  output.ccCarterLabels.addOnTop("7A_1"           );
  output.ccCarterLabels.addOnTop("A_1"            );
  output.ccCarterLabels.addOnTop("3A_1'"          );
  output.ccCarterLabels.addOnTop("5A_1"           );
  output.ccCarterLabels.addOnTop("3A_1''"         );
  output.ccCarterLabels.addOnTop("D_4+3A_1"       );
  output.ccCarterLabels.addOnTop("E_7(a_4)"       );
  output.ccCarterLabels.addOnTop("D_6(a_2) +A_1"   );
  output.ccCarterLabels.addOnTop("2A_3+A_1"       );
  output.ccCarterLabels.addOnTop("A_3+2A_1''"     );
  output.ccCarterLabels.addOnTop("A_3"            );
  output.ccCarterLabels.addOnTop("D_4(a_1) +A_1"   );
  output.ccCarterLabels.addOnTop("A_3+2A_1'"      );
  output.ccCarterLabels.addOnTop("D_6+A_1"        );
  output.ccCarterLabels.addOnTop("A_2+A_1"        );
  output.ccCarterLabels.addOnTop("A_2+3A_1"       );
  output.ccCarterLabels.addOnTop("A_5+A_2"        );
  output.ccCarterLabels.addOnTop("D_4+A_1"        );
  output.ccCarterLabels.addOnTop("2A_2+A_1"       );
  output.ccCarterLabels.addOnTop("A_5'"           );
  output.ccCarterLabels.addOnTop("A_5''"          );
  output.ccCarterLabels.addOnTop("E_7(a_1)"       );
  output.ccCarterLabels.addOnTop("D_5"            );
  output.ccCarterLabels.addOnTop("A_7"            );
  output.ccCarterLabels.addOnTop("E_7"            );
  output.ccCarterLabels.addOnTop("A_4+A_1"        );
  output.ccCarterLabels.addOnTop("D_5(a_1)"       );
  output.ccCarterLabels.addOnTop("A_3+A_2"        );
  output.ccCarterLabels.addOnTop("E_7(a_2)"       );
  output.ccCarterLabels.addOnTop("E_7(a_3)"       );
  int i = - 1;
  i ++; output.theGroup.conjugacyClasses[i].size = 1           ;
  i ++; output.theGroup.conjugacyClasses[i].size = 63          ;
  i ++; output.theGroup.conjugacyClasses[i].size = 315         ;
  i ++; output.theGroup.conjugacyClasses[i].size = 945         ;
  i ++; output.theGroup.conjugacyClasses[i].size = 3780        ;
  i ++; output.theGroup.conjugacyClasses[i].size = 672         ;
  i ++; output.theGroup.conjugacyClasses[i].size = 2240        ;
  i ++; output.theGroup.conjugacyClasses[i].size = 13440       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 3780        ;
  i ++; output.theGroup.conjugacyClasses[i].size = 7560        ;
  i ++; output.theGroup.conjugacyClasses[i].size = 7560        ;
  i ++; output.theGroup.conjugacyClasses[i].size = 11340       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 45360       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 48384       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 10080       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 10080       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 20160       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 30240       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 40320       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 40320       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 120960      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 207360      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 90720       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 90720       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 161280      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 145152      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 60480       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 60480       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 120960      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 96768       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 1           ;
  i ++; output.theGroup.conjugacyClasses[i].size = 63          ;
  i ++; output.theGroup.conjugacyClasses[i].size = 315         ;
  i ++; output.theGroup.conjugacyClasses[i].size = 945         ;
  i ++; output.theGroup.conjugacyClasses[i].size = 3780        ;
  i ++; output.theGroup.conjugacyClasses[i].size = 672         ;
  i ++; output.theGroup.conjugacyClasses[i].size = 2240        ;
  i ++; output.theGroup.conjugacyClasses[i].size = 13440       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 3780        ;
  i ++; output.theGroup.conjugacyClasses[i].size = 7560        ;
  i ++; output.theGroup.conjugacyClasses[i].size = 7560        ;
  i ++; output.theGroup.conjugacyClasses[i].size = 11340       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 45360       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 48384       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 10080       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 10080       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 20160       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 30240       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 40320       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 40320       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 120960      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 207360      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 90720       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 90720       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 161280      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 145152      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 60480       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 60480       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 120960      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 96768       ;

  output.loadConjugacyClassesHelper();
  return true;
}


bool LoadConjugacyClassesE1_8(WeylGroupData& output) {
  output.computeRho(true);
  FiniteGroup<ElementWeylGroup>::ConjugacyClass theClass;
  theClass.flagRepresentativeComputed = true;
  output.theGroup.conjugacyClasses.setSize(0);



  theClass.representative.makeFromReadableReflections(output, false, "[ ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 7, 8, 6, 7, 8, 5, 6, 7, 8, 4, 5, 6, 7, 8, 2, 4, 5, 6, 7, 8, 3, 4, 5, 6, 7, 8, 2, 4, 5, 6, 7, 3, 4, 5, 6, 2, 4, 5, 3, 4, 2, 1, 3, 4, 5, 6, 7, 8, 2, 4, 5, 6, 7, 3, 4, 5, 6, 2, 4, 5, 3, 4, 2, 1, 3, 4, 5, 6, 7, 8, 2, 4, 5, 6, 7, 3, 4, 5, 6, 2, 4, 5, 3, 4, 2, 1, 3, 4, 5, 6, 7, 8, 2, 4, 5, 6, 3, 4, 5, 2, 4, 3, 1, 3, 4, 5, 6, 7, 2, 4, 5, 6, 3, 4, 5, 2, 4, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 5, 4, 5, 2, 4, 5, 3, 4, 5, 2, 4, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 6, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 7, 5, 6, 7, 4, 5, 6, 7, 2, 4, 5, 6, 7, 3, 4, 5, 6, 7, 2, 4, 5, 6, 3, 4, 5, 2, 4, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 7, 6, 7, 5, 6, 4, 5, 6, 7, 2, 4, 5, 6, 3, 4, 5, 6, 7, 8, 2, 4, 5, 6, 3, 4, 5, 1, 3, 4, 5, 6, 7, 8, 2, 4, 5, 6, 7, 3, 4, 5, 6, 2, 4, 5, 3, 4, 2, 1, 3, 4, 5, 6, 7, 2, 4, 5, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 2, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 6, 7 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 1, 2, 3, 1, 4, 2, 3, 1, 4, 3, 5, 4, 2, 3, 1, 4, 3, 5, 4, 2, 6, 5, 4, 2, 3, 1, 4, 3, 5, 4, 2, 6, 5, 4, 3, 1, 7, 6, 5, 4, 2, 3, 1, 4, 3, 5, 4, 2, 6, 5, 4, 3, 1, 7, 6, 5, 4, 2, 3, 4, 5, 6, 7, 8 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 7, 8, 6, 7, 5, 6, 7, 4, 2, 4, 5, 3, 4, 5, 6, 7, 8, 2, 4, 5, 6, 3, 4, 5, 2, 1, 3, 4, 5, 6, 7, 8, 2, 4, 5, 6, 7, 3, 4, 5, 6, 2, 4, 5, 3, 4, 1, 3, 4, 5, 6, 7, 8, 2, 4, 5, 6, 7, 3, 4, 5, 6, 2, 4, 5, 3, 4, 2, 1, 3, 4, 5, 6, 2, 4, 5, 3, 4, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 5, 6, 4, 3, 4, 5, 6, 7, 8, 2, 4, 5, 6, 3, 4, 2, 1, 3, 4, 5, 6, 7, 8, 2, 4, 5, 6, 7, 3, 4, 5, 2, 1, 3, 4, 5, 6, 7, 8, 2 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 6, 5, 2, 4, 5, 6, 3, 4, 5, 2, 4, 3, 1, 3, 4, 5, 6, 2, 4, 5, 3, 4, 1, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 3, 4, 2, 3, 4, 5, 4, 2, 3, 1, 4, 5, 6, 8, 7, 6, 5, 4, 2, 3, 1, 4, 3, 5, 4, 2, 6, 5, 4, 3, 1, 7, 6, 5, 4, 2, 3, 4, 5, 6, 7 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 8, 2, 4 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 4, 2, 5, 4, 2, 6, 5, 4, 2, 3, 4, 5, 6, 7, 6, 5, 4, 2, 3, 4, 5, 6, 7, 8, 7, 6, 5, 4, 2, 3, 1, 4, 3, 5, 4, 2, 6, 5, 4, 3, 1, 7, 8 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 4, 2, 4, 3, 4, 5 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 1, 2, 3, 1, 4, 2, 3, 1, 4, 3, 5, 4, 2, 3, 1, 4, 3, 5, 4, 2, 6, 5, 4, 2, 3, 1, 4, 3, 5, 4, 2, 6, 5, 4, 3, 1, 7, 6, 5, 4, 2, 3, 1, 4, 3, 5, 4, 2, 6, 5, 4, 3, 1, 7, 8, 7, 6, 5, 4, 2, 3, 4, 5, 6, 7, 8 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 3, 4, 2, 3, 4, 6, 5, 7, 6, 5, 4, 2, 3, 4, 5 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 3, 7, 6, 8 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 1, 2, 3, 4, 2, 3, 4, 5, 4, 2, 3, 4, 5, 7 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 4, 5, 6, 7, 3, 4, 5, 2, 4, 3, 1, 3, 4, 5, 6, 7, 8, 2, 4, 5, 3, 4, 2, 1, 3, 4, 5, 6, 7, 8 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 8, 1, 4, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 5, 6, 3, 4 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 3, 4, 3, 5, 4, 3, 6, 5, 4, 3, 8, 7, 6, 5, 4, 2, 3, 1, 4, 3, 5, 4, 2, 6, 5, 4, 3, 7, 6, 5, 4, 2 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 6, 2, 4, 5, 3, 4, 5, 6, 7, 8, 2, 4, 5, 6, 7, 1, 3, 4, 5, 6, 7, 8, 2, 4, 5, 6, 7, 3, 4, 5, 2, 4, 3, 1, 3, 4, 5, 6, 7, 8, 2, 4, 5, 6, 3, 4 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 7, 6, 7, 5, 4, 5, 6, 2, 4, 3, 4, 5, 6, 7, 1, 3, 4, 5, 6, 2, 4, 5, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 7, 5, 4, 5, 2, 4, 5, 3, 4, 5, 2, 4, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 5, 4, 2, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 1, 2, 3, 1, 4, 2, 3, 1, 4, 5, 4, 2, 3, 1, 4, 3, 6, 5, 4, 2, 3, 1, 4, 3, 5, 4, 2, 6, 5, 4, 3, 1, 7, 6, 5, 4, 2, 3, 4, 5, 6, 7, 8, 7 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 4, 5, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 5, 4, 5, 2, 4, 5, 3, 4, 5, 6, 7, 2, 4, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 7, 6, 7, 5, 2, 4, 3, 4, 5, 2, 4, 1, 3, 4, 5, 6, 2, 4, 5 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 3, 4, 2, 3, 5, 4, 2, 3, 1, 4, 5, 6 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 4, 3, 5, 4, 2, 3, 4, 5, 6, 5, 4, 2, 3, 4, 5, 6, 7, 6, 5, 4, 2, 3, 4, 5, 6, 7, 8, 7, 6, 5, 4, 2, 3, 1, 4, 3, 5, 4, 2, 6, 5, 4, 3, 1, 7, 8 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 3, 4, 2, 3, 4, 5, 4, 2, 3, 1, 4, 5, 6 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 7, 5, 2, 4, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 8, 5, 2, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 6, 5, 6, 7, 4, 2, 4, 5, 3, 4 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 5, 4, 6, 5, 4, 2, 3, 7, 6, 5, 4, 2, 3, 8, 7, 6, 5, 4, 2, 3, 1, 4 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 6, 4, 5, 2, 7, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 5, 6, 7, 2, 4 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 7, 6, 5, 4, 2, 3, 4, 8, 7, 6, 5, 4, 2, 3, 1, 4, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 5, 6, 2, 3, 4 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 4, 2, 3, 4, 5, 6, 7 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 4, 2, 5, 4, 2, 3, 6, 5, 4, 2, 3, 4, 5, 6, 7, 6, 5, 4, 2, 3, 1, 4, 3, 5, 4, 2, 6, 5, 4, 3, 1, 7, 8 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 5, 3, 4, 5, 6, 2, 4, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 3, 4, 3, 5, 4, 2, 3, 1, 4, 3, 5, 4, 2, 8, 7, 6 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 1, 3, 4, 3, 1, 5, 4, 2, 3, 4, 5, 8, 7, 6, 5, 4, 2, 3, 1, 4, 3, 5, 4, 2, 6, 5, 7, 6 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 3, 4, 3, 1, 5, 4, 2, 3, 4, 5, 6, 7, 8 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 6, 7, 8, 5, 2, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 6, 2, 3, 4 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 6, 7, 8, 5, 6, 7, 4, 5, 3, 4, 2, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 8, 4, 2, 3, 4, 5, 2 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 1, 3, 1, 4, 5, 4, 3, 6, 5, 4, 2, 3, 1, 4, 3, 5, 4, 2, 7, 6, 5, 4, 2, 3, 1, 4, 3, 5, 4, 2, 6, 5, 4, 3, 1, 7, 6, 5, 4, 2, 3, 4, 5, 6, 7, 8 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 1, 2, 3, 4, 2, 3, 1, 4, 3, 5, 4, 2, 3, 1, 4, 5, 6, 5, 4, 2, 3, 4, 5, 6, 8, 7 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 1, 2, 3, 1, 4, 2, 5, 4, 2, 3, 1, 4, 6, 5, 4, 2, 3, 4, 5, 6, 7, 8 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 6, 4, 1, 5, 3, 2 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 1, 2, 3, 1, 4, 2, 3, 1, 4, 3, 5, 4, 3, 1, 6, 5, 4, 2, 3, 4, 5, 6, 7, 8 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 5, 7, 6, 2, 1, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 1, 2, 3, 4, 2, 5, 4 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 7, 6, 2, 4, 5, 3, 4, 1, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 7, 6, 5, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 3, 4, 2, 3, 5, 4, 2, 3, 4, 6, 5, 4, 2, 3, 7, 6, 5, 4, 2, 3, 1, 4, 5, 6, 7, 8 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 6, 5, 4, 2, 3, 4, 5, 6, 7, 1, 3, 4, 5, 2, 4 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 8, 6, 2, 1, 3, 4, 5 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 7, 5, 6, 7, 4, 5, 6, 7, 2, 4, 5, 6, 7, 3, 4, 5, 6, 7, 2, 4, 5, 6, 3, 4, 5, 2, 4, 3, 1, 3, 4, 5, 6, 7, 2, 4, 5, 6, 3, 4, 5, 2, 4, 3, 1, 3, 4, 5, 6, 7, 2, 4, 5, 6, 3, 4, 5, 2, 4, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 6, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 4, 5, 2, 4, 5, 3, 4, 5, 2, 4, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 4, 1, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 3, 4, 2, 3, 4, 5, 4, 2, 3, 4, 5, 6, 5, 4, 2, 3, 4, 5, 6, 7, 6, 5, 4, 2, 3, 4, 5, 6, 7, 8 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 3, 4, 2, 3, 4, 5, 4, 2, 3, 4, 5, 6 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 5, 4, 2, 3, 4, 2 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 4, 2, 3, 4, 5, 4, 2, 3, 1, 4, 3, 5, 6, 5, 4, 2, 3, 4, 5, 6, 7, 6, 5, 4, 2, 3, 1, 4, 3, 5, 6, 7 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 3, 4, 5, 4, 2, 3, 4, 5, 7, 6, 8, 7, 6, 5, 4, 2, 3, 4, 5, 6 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 5, 2, 4, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 4, 2, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 3, 4, 2, 3, 4, 5, 4, 2, 3, 4, 5, 6, 5, 4, 2, 3, 4, 5, 6, 7, 6, 5, 4, 2, 3, 1, 4, 5, 6, 7 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 5, 6, 4, 5, 6, 3, 4, 5, 6, 2, 4, 3, 1, 3, 4, 5, 6, 2, 4, 5, 3, 4, 2, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 5, 2, 4, 5, 1, 3, 4, 5, 6, 2, 4, 5, 3, 4, 2, 1, 3, 4, 5, 6, 7 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 5, 2, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 4, 2, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 6, 5, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 3, 4, 2, 3, 1, 4, 5, 4, 2, 6, 5, 4, 2, 3, 1, 4, 3, 5, 4, 6, 5, 7 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 1, 2, 3, 1, 4, 2, 3, 5, 4, 2, 3, 1, 4, 3, 5, 4, 6, 5, 4, 2, 3, 4, 5, 6, 7 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 1, 2, 3, 4, 2, 6, 5, 4, 2, 3, 4, 5, 8 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 5, 2, 6, 4 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 2, 3, 4, 2, 3, 4, 5, 4, 2, 3, 1, 4, 5, 6 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 3, 5, 1, 4 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 3, 4, 2, 3, 4, 5, 4, 2, 3, 4, 5, 7, 6, 8 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 3, 5, 6, 5, 4, 2, 3, 4, 7, 6, 5, 4, 2, 3, 1, 4 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 4, 6, 8, 1, 3, 5, 7 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 5, 6, 2, 4, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 6, 7, 5, 4, 5, 2, 4, 5, 1, 3, 4, 5, 2, 4, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 3, 4, 2, 3, 4, 5, 4, 2, 3, 1, 4, 5, 8, 7 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 4, 2, 5, 4, 2, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 6, 4, 2, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 3, 4, 2, 3, 4, 5, 7, 6, 5, 4, 2, 3, 4, 5, 6, 8 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 1, 4, 2, 5, 4, 2, 3, 7, 8 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 6, 7, 8, 5, 4, 2, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 6, 4, 1, 2, 3, 5 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 4, 2, 5, 4, 3, 1, 6, 5, 7, 6, 5 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 6, 7, 2, 4, 5, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 5, 3, 4, 5, 6, 7, 2, 4, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 5, 6, 3, 4, 2, 1, 3, 4 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 6, 7, 5, 2, 4, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 6, 7, 4, 1, 2, 3 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 8, 5, 6, 4, 2, 4, 3, 4 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 7, 8, 4, 5, 2, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 4, 3, 5, 4, 2, 3, 4, 5, 6, 8, 7 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 8, 6, 7, 5, 2, 3, 1 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);
  theClass.representative.makeFromReadableReflections(output, false, "[ 2, 4, 2, 3, 1, 6, 5, 4, 2, 3, 4, 5, 7 ]"); output.theGroup.conjugacyClasses.addOnTop(theClass);

  output.ccCarterLabels.setSize(0);
  output.ccCarterLabels.addOnTop(" "              );
  output.ccCarterLabels.addOnTop("8A_1"           );
  output.ccCarterLabels.addOnTop("4A_1'"          );
  output.ccCarterLabels.addOnTop("2A_1"           );
  output.ccCarterLabels.addOnTop("6A_1"           );
  output.ccCarterLabels.addOnTop("2D_4(a_1)"      );
  output.ccCarterLabels.addOnTop("4A_1''"         );
  output.ccCarterLabels.addOnTop("A_2"            );
  output.ccCarterLabels.addOnTop("D_4+4A_1"       );
  output.ccCarterLabels.addOnTop("4A_2"           );
  output.ccCarterLabels.addOnTop("E_8(a_8)"       );
  output.ccCarterLabels.addOnTop("3A_2"           );
  output.ccCarterLabels.addOnTop("E_7(a_4) +A_1"   );
  output.ccCarterLabels.addOnTop("2A_2"           );
  output.ccCarterLabels.addOnTop("2D_4"           );
  output.ccCarterLabels.addOnTop("D_4(a_1)"       );
  output.ccCarterLabels.addOnTop("2A_3+2A_1"      );
  output.ccCarterLabels.addOnTop("2A_3'"          );
  output.ccCarterLabels.addOnTop("A_3+A_1"        );
  output.ccCarterLabels.addOnTop("A_3+3A_1"       );
  output.ccCarterLabels.addOnTop("D_8(a_3)"       );
  output.ccCarterLabels.addOnTop("2A_3''"         );
  output.ccCarterLabels.addOnTop("A_4"            );
  output.ccCarterLabels.addOnTop("D_6+2A_1"       );
  output.ccCarterLabels.addOnTop("2A_4"           );
  output.ccCarterLabels.addOnTop("E_8(a_6)"       );
  output.ccCarterLabels.addOnTop("A_2+4A_1"       );
  output.ccCarterLabels.addOnTop("D_4"            );
  output.ccCarterLabels.addOnTop("E_6(a_2) +A_2"   );
  output.ccCarterLabels.addOnTop("A_2+2A_1"       );
  output.ccCarterLabels.addOnTop("D_4+2A_1"       );
  output.ccCarterLabels.addOnTop("E_8(a_3)"       );
  output.ccCarterLabels.addOnTop("E_6(a_2)"       );
  output.ccCarterLabels.addOnTop("A_5+A_2+A_1"    );
  output.ccCarterLabels.addOnTop("A_5+A_1'"       );
  output.ccCarterLabels.addOnTop("D_4+A_2"        );
  output.ccCarterLabels.addOnTop("2A_2+2A_1"      );
  output.ccCarterLabels.addOnTop("D_6(a_2)"       );
  output.ccCarterLabels.addOnTop("D_8(a_1)"       );
  output.ccCarterLabels.addOnTop("A_5+A_1''"      );
  output.ccCarterLabels.addOnTop("A_6"            );
  output.ccCarterLabels.addOnTop("D_8"            );
  output.ccCarterLabels.addOnTop("D_5+A_1"        );
  output.ccCarterLabels.addOnTop("D_6(a_1)"       );
  output.ccCarterLabels.addOnTop("A_7+A_1"        );
  output.ccCarterLabels.addOnTop("E_6(a_1)"       );
  output.ccCarterLabels.addOnTop("E_7+A_1"        );
  output.ccCarterLabels.addOnTop("A_8"            );
  output.ccCarterLabels.addOnTop("E_8(a_4)"       );
  output.ccCarterLabels.addOnTop("A_4+2A_1"       );
  output.ccCarterLabels.addOnTop("D_6"            );
  output.ccCarterLabels.addOnTop("E_8(a_2)"       );
  output.ccCarterLabels.addOnTop("D_4(a_1) +A_2"   );
  output.ccCarterLabels.addOnTop("D_5(a_1) +A_3"   );
  output.ccCarterLabels.addOnTop("E_6+A_2"        );
  output.ccCarterLabels.addOnTop("E_8(a_7)"       );
  output.ccCarterLabels.addOnTop("E_6"            );
  output.ccCarterLabels.addOnTop("E_7(a_2) +A_1"   );
  output.ccCarterLabels.addOnTop("A_3+A_2+A_1"    );
  output.ccCarterLabels.addOnTop("D_5(a_1) +A_1"   );
  output.ccCarterLabels.addOnTop("E_8(a_1)"       );
  output.ccCarterLabels.addOnTop("A_4+A_2"        );
  output.ccCarterLabels.addOnTop("D_8(a_2)"       );
  output.ccCarterLabels.addOnTop("E_8(a_5)"       );
  output.ccCarterLabels.addOnTop("E_8"            );
  output.ccCarterLabels.addOnTop("A_1"            );
  output.ccCarterLabels.addOnTop("7A_1"           );
  output.ccCarterLabels.addOnTop("3A_1"           );
  output.ccCarterLabels.addOnTop("5A_1"           );
  output.ccCarterLabels.addOnTop("A_3"            );
  output.ccCarterLabels.addOnTop("A_3+4A_1"       );
  output.ccCarterLabels.addOnTop("A_3+2A_1'"      );
  output.ccCarterLabels.addOnTop("D_4(a_1) +A_1"   );
  output.ccCarterLabels.addOnTop("2A_3+A_1"       );
  output.ccCarterLabels.addOnTop("D_4(a_1) +A_3"   );
  output.ccCarterLabels.addOnTop("A_3+2A_1''"     );
  output.ccCarterLabels.addOnTop("A_2+A_1"        );
  output.ccCarterLabels.addOnTop("D_4+3A_1"       );
  output.ccCarterLabels.addOnTop("3A_2+A_1"       );
  output.ccCarterLabels.addOnTop("E_7(a_4)"       );
  output.ccCarterLabels.addOnTop("A_2+3A_1"       );
  output.ccCarterLabels.addOnTop("D_4+A_1"        );
  output.ccCarterLabels.addOnTop("2A_2+A_1"       );
  output.ccCarterLabels.addOnTop("D_6(a_2) +A_1"   );
  output.ccCarterLabels.addOnTop("A_5+A_2"        );
  output.ccCarterLabels.addOnTop("E_6(a_2) +A_1"   );
  output.ccCarterLabels.addOnTop("A_5"            );
  output.ccCarterLabels.addOnTop("A_5+2A_1"       );
  output.ccCarterLabels.addOnTop("D_5"            );
  output.ccCarterLabels.addOnTop("D_5+2A_1"       );
  output.ccCarterLabels.addOnTop("A_7'"           );
  output.ccCarterLabels.addOnTop("A_7''"          );
  output.ccCarterLabels.addOnTop("A_4+A_1"        );
  output.ccCarterLabels.addOnTop("D_6+A_1"        );
  output.ccCarterLabels.addOnTop("A_3+A_2+2A_1"   );
  output.ccCarterLabels.addOnTop("D_5(a_1)"       );
  output.ccCarterLabels.addOnTop("A_3+A_2"        );
  output.ccCarterLabels.addOnTop("D_4+A_3"        );
  output.ccCarterLabels.addOnTop("D_5(a_1) +A_2"   );
  output.ccCarterLabels.addOnTop("D_7"            );
  output.ccCarterLabels.addOnTop("E_6+A_1"        );
  output.ccCarterLabels.addOnTop("E_7(a_2)"       );
  output.ccCarterLabels.addOnTop("A_6+A_1"        );
  output.ccCarterLabels.addOnTop("E_7(a_1)"       );
  output.ccCarterLabels.addOnTop("E_6(a_1) +A_1"   );
  output.ccCarterLabels.addOnTop("E_7"            );
  output.ccCarterLabels.addOnTop("A_4+A_3"        );
  output.ccCarterLabels.addOnTop("D_7(a_1)"       );
  output.ccCarterLabels.addOnTop("D_5+A_2"        );
  output.ccCarterLabels.addOnTop("D_7(a_2)"       );
  output.ccCarterLabels.addOnTop("A_4+A_2+ 1"      );
  output.ccCarterLabels.addOnTop("E_7(a_3)"       );
  int i = - 1;
  i ++; output.theGroup.conjugacyClasses[i].size = 1           ;
  i ++; output.theGroup.conjugacyClasses[i].size = 1           ;
  i ++; output.theGroup.conjugacyClasses[i].size = 3150        ;
  i ++; output.theGroup.conjugacyClasses[i].size = 3780        ;
  i ++; output.theGroup.conjugacyClasses[i].size = 3780        ;
  i ++; output.theGroup.conjugacyClasses[i].size = 15120       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 113400      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 2240        ;
  i ++; output.theGroup.conjugacyClasses[i].size = 2240        ;
  i ++; output.theGroup.conjugacyClasses[i].size = 4480        ;
  i ++; output.theGroup.conjugacyClasses[i].size = 4480        ;
  i ++; output.theGroup.conjugacyClasses[i].size = 89600       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 89600       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 268800      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 268800      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 37800       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 37800       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 680400      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 907200      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 907200      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 3628800     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 5443200     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 580608      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 580608      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 1161216     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 1161216     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 100800      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 100800      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 403200      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 604800      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 604800      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 2419200     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 806400      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 806400      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 1612800     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 3225600     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 2419200     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 2419200     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 9676800     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 14515200    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 24883200    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 24883200    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 10886400    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 5443200     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 5443200     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 6451200     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 6451200     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 12902400    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 12902400    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 8709120     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 8709120     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 34836480    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 1209600     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 1209600     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 2419200     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 2419200     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 4838400     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 4838400     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 7257600     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 7257600     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 29030400    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 11612160    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 11612160    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 23224320    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 23224320    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 120         ;
  i ++; output.theGroup.conjugacyClasses[i].size = 120         ;
  i ++; output.theGroup.conjugacyClasses[i].size = 37800       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 37800       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 45360       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 45360       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 151200      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 453600      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 453600      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 907200      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 2721600     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 80640       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 80640       ;
  i ++; output.theGroup.conjugacyClasses[i].size = 268800      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 268800      ;
  i ++; output.theGroup.conjugacyClasses[i].size = 1209600     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 1209600     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 1612800     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 1612800     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 2419200     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 2419200     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 4838400     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 4838400     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 1814400     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 1814400     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 10886400    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 43545600    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 5806080     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 5806080     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 1209600     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 1209600     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 3628800     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 3628800     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 9676800     ;
  i ++; output.theGroup.conjugacyClasses[i].size = 29030400    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 14515200    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 14515200    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 24883200    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 24883200    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 19353600    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 19353600    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 17418240    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 17418240    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 14515200    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 14515200    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 11612160    ;
  i ++; output.theGroup.conjugacyClasses[i].size = 11612160    ;
  output.loadConjugacyClassesHelper();
  return true;
}

bool LoadCharTableF1_4(WeylGroupData& output) {
  FiniteGroup<ElementWeylGroup>& outputG = output.theGroup;
  outputG.characterTable.setExpectedSize(112); outputG.characterTable.setSize(0);
  ClassFunction<FiniteGroup<ElementWeylGroup>, Rational> currentCF;
  currentCF.G = &outputG;
  currentCF.data.assignString("[  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, - 1, - 1, - 1, - 1, - 1,  1,  1,  1,  1,  1, - 1, - 1, - 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,  1,  1,  1,  1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[  2,  2,  2,  2,  2,  2, - 1, - 1, - 1, - 1, - 1,  2,  2, - 1, - 1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[  2,  2,  2,  2,  2,  2, - 1, - 1, - 1, - 1, - 1, -2, -2,  1,  1, -2,  0,  0,  0,  0,  0,  0,  0,  0,  0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[  2,  2,  2, - 1, - 1,  2,  2,  2, - 1, - 1, - 1,  0,  0,  0,  0,  0, -2, -2,  1,  1, -2,  0,  0,  0,  0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[  4,  4,  4, -2, -2,  4, -2, -2,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[  9,  9,  1,  0,  0, -3,  0,  0,  0,  0,  0,  3,  3,  0,  0, - 1,  3,  3,  0,  0, - 1,  1,  1,  1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[  2,  2,  2, - 1, - 1,  2,  2,  2, - 1, - 1, - 1,  0,  0,  0,  0,  0,  2,  2, - 1, - 1,  2,  0,  0,  0,  0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[  9,  9,  1,  0,  0, -3,  0,  0,  0,  0,  0,  3,  3,  0,  0, - 1, -3, -3,  0,  0,  1, - 1, - 1, - 1,  1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[  9,  9,  1,  0,  0, -3,  0,  0,  0,  0,  0, -3, -3,  0,  0,  1,  3,  3,  0,  0, - 1, - 1, - 1, - 1,  1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[  9,  9,  1,  0,  0, -3,  0,  0,  0,  0,  0, -3, -3,  0,  0,  1, -3, -3,  0,  0,  1,  1,  1,  1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[  6,  6, -2,  0,  0,  2,  0,  0,  3,  3, - 1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2, -2, -2,  0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[  6,  6, -2,  0,  0,  2,  0,  0,  3,  3, - 1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -2,  2,  2,  0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 12, 12, -4,  0,  0,  4,  0,  0, -3, -3,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[  4, -4,  0,  1, - 1,  0,  1, - 1, -2,  2,  0,  2, -2, - 1,  1,  0,  2, -2, - 1,  1,  0,  0,  2, -2,  0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[  4, -4,  0,  1, - 1,  0,  1, - 1, -2,  2,  0,  2, -2, - 1,  1,  0, -2,  2,  1, - 1,  0,  0, -2,  2,  0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[  4, -4,  0,  1, - 1,  0,  1, - 1, -2,  2,  0, -2,  2,  1, - 1,  0,  2, -2, - 1,  1,  0,  0, -2,  2,  0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[  4, -4,  0,  1, - 1,  0,  1, - 1, -2,  2,  0, -2,  2,  1, - 1,  0, -2,  2,  1, - 1,  0,  0,  2, -2,  0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[  8, -8,  0,  2, -2,  0, - 1,  1,  2, -2,  0,  4, -4,  1, - 1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[  8, -8,  0,  2, -2,  0, - 1,  1,  2, -2,  0, -4,  4, - 1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[  8, -8,  0, - 1,  1,  0,  2, -2,  2, -2,  0,  0,  0,  0,  0,  0,  4, -4,  1, - 1,  0,  0,  0,  0,  0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[  8, -8,  0, - 1,  1,  0,  2, -2,  2, -2,  0,  0,  0,  0,  0,  0, -4,  4, - 1,  1,  0,  0,  0,  0,  0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 16, - 16, 0, -2,  2,  0, -2,  2, -2,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 ]"); outputG.characterTable.addOnTop(currentCF);
  output.irrepsCarterLabels.setSize(0);
  output.irrepsCarterLabels.addOnTop("\\phi_{1,0}   ");
  output.irrepsCarterLabels.addOnTop("\\phi_{1,12}''");
  output.irrepsCarterLabels.addOnTop("\\phi_{1,12}' ");
  output.irrepsCarterLabels.addOnTop("\\phi_{1,24}  ");
  output.irrepsCarterLabels.addOnTop("\\phi_{2,4}'' ");
  output.irrepsCarterLabels.addOnTop("\\phi_{2,16}' ");
  output.irrepsCarterLabels.addOnTop("\\phi_{2,4}'  ");
  output.irrepsCarterLabels.addOnTop("\\phi_{2,16}''");
  output.irrepsCarterLabels.addOnTop("\\phi_{4,8}   ");
  output.irrepsCarterLabels.addOnTop("\\phi_{9,2}   ");
  output.irrepsCarterLabels.addOnTop("\\phi_{9,6}'' ");
  output.irrepsCarterLabels.addOnTop("\\phi_{9,6}'  ");
  output.irrepsCarterLabels.addOnTop("\\phi_{9,10}  ");
  output.irrepsCarterLabels.addOnTop("\\phi_{6,6}'  ");
  output.irrepsCarterLabels.addOnTop("\\phi_{6,6}'' ");
  output.irrepsCarterLabels.addOnTop("\\phi_{12,4}  ");
  output.irrepsCarterLabels.addOnTop("\\phi_{4,1}   ");
  output.irrepsCarterLabels.addOnTop("\\phi_{4,7}'' ");
  output.irrepsCarterLabels.addOnTop("\\phi_{4,7}'  ");
  output.irrepsCarterLabels.addOnTop("\\phi_{4,13}  ");
  output.irrepsCarterLabels.addOnTop("\\phi_{8,3}'' ");
  output.irrepsCarterLabels.addOnTop("\\phi_{8,9}'  ");
  output.irrepsCarterLabels.addOnTop("\\phi_{8,3}'  ");
  output.irrepsCarterLabels.addOnTop("\\phi_{8,9}'' ");
  output.irrepsCarterLabels.addOnTop("\\phi_{16,5}  ");
  return true;
}

bool LoadCharTableE1_7(WeylGroupData& output) {
  FiniteGroup<ElementWeylGroup>& outputG = output.theGroup;
  outputG.characterTable.setExpectedSize(112); outputG.characterTable.setSize(0);
  ClassFunction<FiniteGroup<ElementWeylGroup>, Rational> currentCF;
  currentCF.G = &outputG;
  currentCF.data.assignString("[ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,1, 1, 1, 1, 1, 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 7, -5, - 1, 3, - 1, 4, -2, 1, 3, 1, -3, - 1, 1, 2, -2, 2, 2, 0, 1, - 1, - 1,0, - 1, 1, 1, 0, -2, 0, 0, - 1, 7, -5, - 1, 3, - 1, 4, -2, 1, 3, 1, -3, - 1, 1, 2, -2, 2, 2, 0, 1, - 1, - 1, 0, - 1, 1, 1, 0, -2, 0, 0, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 7, -5, - 1, 3, - 1, 4, -2, 1, 3, 1, -3, - 1, 1, 2, -2, 2, 2, 0, 1, - 1, - 1,      0, - 1, 1, 1, 0, -2, 0, 0, - 1, -7, 5, 1, -3, 1, -4, 2, - 1, -3, - 1, 3, 1,- 1, -2, 2, -2, -2, 0, - 1, 1, 1, 0, 1, - 1, - 1, 0, 2, 0, 0, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 15, -5, 7, 3, - 1, 0, -3, 3, - 1, -3, 1, 3, 1, 0, -2, -2, 1, 0, 1, 1, - 1,1, 1, - 1, 0, 0, 0, -2, - 1, 0, 15, -5, 7, 3, - 1, 0, -3, 3, - 1, -3, 1, 3,1, 0, -2, -2, 1, 0, 1, 1, - 1, 1, 1, - 1, 0, 0, 0, -2, - 1, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 15, -5, 7, 3, - 1, 0, -3, 3, - 1, -3, 1, 3, 1, 0, -2, -2, 1, 0, 1, 1, - 1,1, 1, - 1, 0, 0, 0, -2, - 1, 0, - 15, 5, -7, -3, 1, 0, 3, -3, 1, 3, - 1,-3, - 1, 0, 2, 2, - 1, 0, - 1, - 1, 1, - 1, - 1, 1, 0, 0, 0, 2, 1, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 21, 9, -3, 1, -3, 6, 3, 0, 5, - 1, 3, 1, - 1, 1, 0, 0, 3, -2, 0, 0, 0, 0,- 1, 1, 0, - 1, 2, 0, - 1, 1, 21, 9, -3, 1, -3, 6, 3, 0, 5, - 1, 3, 1, - 1,1, 0, 0, 3, -2, 0, 0, 0, 0, - 1, 1, 0, - 1, 2, 0, - 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 21, 9, -3, 1, -3, 6, 3, 0, 5, - 1, 3, 1, - 1, 1, 0, 0, 3, -2, 0, 0, 0, 0,- 1, 1, 0, - 1, 2, 0, - 1, 1, -21, -9, 3, - 1, 3, -6, -3, 0, -5, 1, -3, - 1,1, - 1, 0, 0, -3, 2, 0, 0, 0, 0, 1, - 1, 0, 1, -2, 0, 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 21, - 11, 5, 5, -3, 6, 3, 0, 1, -3, -3, 1, 1, 1, -2, 2, - 1, 2, -2, 2, 0,0, - 1, - 1, 0, - 1, 0, 0, 1, 1, 21, - 11, 5, 5, -3, 6, 3, 0, 1, -3, -3, 1,1, 1, -2, 2, - 1, 2, -2, 2, 0, 0, - 1, - 1, 0, - 1, 0, 0, 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 21, - 11, 5, 5, -3, 6, 3, 0, 1, -3, -3, 1, 1, 1, -2, 2, - 1, 2, -2, 2, 0,0, - 1, - 1, 0, - 1, 0, 0, 1, 1, -21, 11, -5, -5, 3, -6, -3, 0, - 1, 3, 3,- 1, - 1, - 1, 2, -2, 1, -2, 2, -2, 0, 0, 1, 1, 0, 1, 0, 0, - 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 27, 15, 3, 7, 3, 9, 0, 0, 3, 1, 5, - 1, 1, 2, 3, 3, 0, 1, 0, 0, 0, - 1, 1,- 1, 0, 0, 1, - 1, 0, - 1, 27, 15, 3, 7, 3, 9, 0, 0, 3, 1, 5, - 1, 1, 2, 3, 3, 0, 1, 0, 0, 0, - 1, 1, - 1, 0, 0, 1, - 1, 0, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 27, 15, 3, 7, 3, 9, 0, 0, 3, 1, 5, - 1, 1, 2, 3, 3, 0, 1, 0, 0, 0, - 1, 1,- 1, 0, 0, 1, - 1, 0, - 1, -27, - 15, -3, -7, -3, -9, 0, 0, -3, - 1, -5, 1,- 1, -2, -3, -3, 0, - 1, 0, 0, 0, 1, - 1, 1, 0, 0, - 1, 1, 0, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 35, -5, 3, -5, 3, 5, - 1, 2, 7, - 1, - 1, - 1, - 1, 0, 1, -3, 3, 1, -2, 0, 0,0, 1, 1, - 1, 0, - 1, - 1, 1, 0, 35, -5, 3, -5, 3, 5, - 1, 2, 7, - 1, - 1,- 1, - 1, 0, 1, -3, 3, 1, -2, 0, 0, 0, 1, 1, - 1, 0, - 1, - 1, 1, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 35, -5, 3, -5, 3, 5, - 1, 2, 7, - 1, - 1, - 1, - 1, 0, 1, -3, 3, 1, -2, 0, 0,0, 1, 1, - 1, 0, - 1, - 1, 1, 0, -35, 5, -3, 5, -3, -5, 1, -2, -7, 1, 1,1, 1, 0, - 1, 3, -3, - 1, 2, 0, 0, 0, - 1, - 1, 1, 0, 1, 1, - 1, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 35, 15, 11, 7, 3, 5, - 1, 2, - 1, 5, 1, 3, 1, 0, 3, - 1, - 1, 1, 0, 2, 0, 0, - 1, 1, - 1, 0, - 1, 1, - 1, 0, 35, 15, 11, 7, 3, 5, - 1, 2, - 1, 5, 1, 3, 1,      0, 3, - 1, - 1, 1, 0, 2, 0, 0, - 1, 1, - 1, 0, - 1, 1, - 1, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 35, 15, 11, 7, 3, 5, - 1, 2, - 1, 5, 1, 3, 1, 0, 3, - 1, - 1, 1, 0, 2, 0, 0, - 1, 1, - 1, 0, - 1, 1, - 1, 0, -35, - 15, - 11, -7, -3, -5, 1, -2, 1, -5,      - 1, -3, - 1, 0, -3, 1, 1, - 1, 0, -2, 0, 0, 1, - 1, 1, 0, 1, - 1, 1, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 56, -24, -8, 8, 0, 11, 2, 2, 0, 4, -4, 0, 0, 1, -3, 1, -2, - 1, 0, -2, 0, 0, 0, 0, - 1, 1, 1, - 1, 0, 1, 56, -24, -8, 8, 0, 11, 2, 2, 0, 4, -4, 0,      0, 1, -3, 1, -2, - 1, 0, -2, 0, 0, 0, 0, - 1, 1, 1, - 1, 0, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 56, -24, -8, 8, 0, 11, 2, 2, 0, 4, -4, 0, 0, 1, -3, 1, -2, - 1, 0, -2, 0, 0, 0, 0, - 1, 1, 1, - 1, 0, 1, -56, 24, 8, -8, 0, - 11, -2, -2, 0, -4, 4,      0, 0, - 1, 3, - 1, 2, 1, 0, 2, 0, 0, 0, 0, 1, - 1, - 1, 1, 0, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 70, - 10, - 10, 6, -2, -5, 7, 1, 2, 2, 2, 2, -2, 0, - 1, - 1, - 1, 3, - 1, - 1, 1, 0, 0, 0, 1, 0, - 1, - 1, - 1, 0, 70, - 10, - 10, 6, -2, -5, 7, 1, 2, 2,2, 2, -2, 0, - 1, - 1, - 1, 3, - 1, - 1, 1, 0, 0, 0, 1, 0, - 1, - 1, - 1, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 70, - 10, - 10, 6, -2, -5, 7, 1, 2, 2, 2, 2, -2, 0, - 1, - 1, - 1, 3, - 1, - 1, 1, 0, 0, 0, 1, 0, - 1, - 1, - 1, 0, -70, 10, 10, -6, 2, 5, -7, - 1, -2, -2, -2, -2, 2, 0, 1, 1, 1, -3, 1, 1, - 1, 0, 0, 0, - 1, 0, 1, 1, 1, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 84, 4, 20, 4, 4, -6, 3, 3, 4, 0, 0, 4, 0, - 1, -2, 2, - 1, -2, 1, - 1, 1, 0, 0, 0, 0, - 1, 0, 0, 1, - 1, 84, 4, 20, 4, 4, -6, 3, 3, 4, 0, 0, 4, 0, - 1,     -2, 2, - 1, -2, 1, - 1, 1, 0, 0, 0, 0, - 1, 0, 0, 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 84, 4, 20, 4, 4, -6, 3, 3, 4, 0, 0, 4, 0, - 1, -2, 2, - 1, -2, 1, - 1, 1, 0,      0, 0, 0, - 1, 0, 0, 1, - 1, -84, -4, -20, -4, -4, 6, -3, -3, -4, 0, 0,      -4, 0, 1, 2, -2, 1, 2, - 1, 1, - 1, 0, 0, 0, 0, 1, 0, 0, - 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 105, -35, 1, 5, 1, 15, -3, -3, 5, - 1, -5, 1, - 1, 0, 1, 1, 1, - 1, 1, 1, 1,      0, 1, - 1, 0, 0, - 1, 1, - 1, 0, 105, -35, 1, 5, 1, 15, -3, -3, 5, - 1, -5,      1, - 1, 0, 1, 1, 1, - 1, 1, 1, 1, 0, 1, - 1, 0, 0, - 1, 1, - 1, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 105, -35, 1, 5, 1, 15, -3, -3, 5, - 1, -5, 1, - 1, 0, 1, 1, 1, - 1, 1, 1, 1,      0, 1, - 1, 0, 0, - 1, 1, - 1, 0, - 105, 35, - 1, -5, - 1, - 15, 3, 3, -5, 1,      5, - 1, 1, 0, - 1, - 1, - 1, 1, - 1, - 1, - 1, 0, - 1, 1, 0, 0, 1, - 1, 1, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 105, 25, -7, 9, 1, 0, 6, 3, -3, -3, -3, -3, 1, 0, 4, -4, 2, 0, 1, - 1, 1,      0, - 1, - 1, 0, 0, 0, 0, 0, 0, 105, 25, -7, 9, 1, 0, 6, 3, -3, -3, -3,      -3, 1, 0, 4, -4, 2, 0, 1, - 1, 1, 0, - 1, - 1, 0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 105, 25, -7, 9, 1, 0, 6, 3, -3, -3, -3, -3, 1, 0, 4, -4, 2, 0, 1, - 1, 1,      0, - 1, - 1, 0, 0, 0, 0, 0, 0, - 105, -25, 7, -9, - 1, 0, -6, -3, 3, 3, 3,      3, - 1, 0, -4, 4, -2, 0, - 1, 1, - 1, 0, 1, 1, 0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 105, 5, 17, -3, -7, 0, 6, 3, -3, 3, - 1, 1, - 1, 0, 2, 2, 2, 0, - 1, - 1, - 1,      0, 1, - 1, 0, 0, 0, 2, 0, 0, 105, 5, 17, -3, -7, 0, 6, 3, -3, 3, - 1, 1,      - 1, 0, 2, 2, 2, 0, - 1, - 1, - 1, 0, 1, - 1, 0, 0, 0, 2, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 105, 5, 17, -3, -7, 0, 6, 3, -3, 3, - 1, 1, - 1, 0, 2, 2, 2, 0, - 1, - 1, - 1,      0, 1, - 1, 0, 0, 0, 2, 0, 0, - 105, -5, - 17, 3, 7, 0, -6, -3, 3, -3, 1,      - 1, 1, 0, -2, -2, -2, 0, 1, 1, 1, 0, - 1, 1, 0, 0, 0, -2, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 120, 40, -8, 8, 0, 15, -6, 0, 0, -4, 4, 0, 0, 0, 1, 1, -2, - 1, -2, -2, 0,      1, 0, 0, 0, 0, - 1, 1, 0, 0, 120, 40, -8, 8, 0, 15, -6, 0, 0, -4, 4, 0,      0, 0, 1, 1, -2, - 1, -2, -2, 0, 1, 0, 0, 0, 0, - 1, 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 120, 40, -8, 8, 0, 15, -6, 0, 0, -4, 4, 0, 0, 0, 1, 1, -2, - 1, -2, -2, 0,      1, 0, 0, 0, 0, - 1, 1, 0, 0, - 120, -40, 8, -8, 0, - 15, 6, 0, 0, 4, -4,      0, 0, 0, - 1, - 1, 2, 1, 2, 2, 0, - 1, 0, 0, 0, 0, 1, - 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 168, 40, 8, 8, 8, 6, 6, -3, 0, 0, 0, 0, 0, -2, -2, 2, 2, 2, 1, - 1, - 1, 0,      0, 0, 0, 0, 0, 0, 0, 1, 168, 40, 8, 8, 8, 6, 6, -3, 0, 0, 0, 0, 0, -2,      -2, 2, 2, 2, 1, - 1, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 168, 40, 8, 8, 8, 6, 6, -3, 0, 0, 0, 0, 0, -2, -2, 2, 2, 2, 1, - 1, - 1, 0,      0, 0, 0, 0, 0, 0, 0, 1, - 168, -40, -8, -8, -8, -6, -6, 3, 0, 0, 0, 0,      0, 2, 2, -2, -2, -2, - 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 189, 21, -3, - 11, -3, 9, 0, 0, 9, 1, 1, 1, 1, - 1, -3, -3, 0, 1, 0, 0, 0,      0, - 1, - 1, 0, 1, 1, 1, 0, - 1, 189, 21, -3, - 11, -3, 9, 0, 0, 9, 1, 1,      1, 1, - 1, -3, -3, 0, 1, 0, 0, 0, 0, - 1, - 1, 0, 1, 1, 1, 0, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 189, 21, -3, - 11, -3, 9, 0, 0, 9, 1, 1, 1, 1, - 1, -3, -3, 0, 1, 0, 0, 0,      0, - 1, - 1, 0, 1, 1, 1, 0, - 1, - 189, -21, 3, 11, 3, -9, 0, 0, -9, - 1,      - 1, - 1, - 1, 1, 3, 3, 0, - 1, 0, 0, 0, 0, 1, 1, 0, - 1, - 1, - 1, 0, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 189, -51, -3, 13, -3, 9, 0, 0, -3, 1, 1, -3, 1, - 1, -3, -3, 0, 1, 0, 0,      0, 0, 1, 1, 0, - 1, 1, 1, 0, - 1, 189, -51, -3, 13, -3, 9, 0, 0, -3, 1,      1, -3, 1, - 1, -3, -3, 0, 1, 0, 0, 0, 0, 1, 1, 0, - 1, 1, 1, 0, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 189, -51, -3, 13, -3, 9, 0, 0, -3, 1, 1, -3, 1, - 1, -3, -3, 0, 1, 0, 0,      0, 0, 1, 1, 0, - 1, 1, 1, 0, - 1, - 189, 51, 3, - 13, 3, -9, 0, 0, 3, - 1,      - 1, 3, - 1, 1, 3, 3, 0, - 1, 0, 0, 0, 0, - 1, - 1, 0, 1, - 1, - 1, 0, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 189, -39, 21, 1, -3, 9, 0, 0, -3, -5, - 1, 1, - 1, - 1, 3, 3, 0, 1, 0, 0, 0,      0, - 1, 1, 0, 1, 1, - 1, 0, - 1, 189, -39, 21, 1, -3, 9, 0, 0, -3, -5, - 1,      1, - 1, - 1, 3, 3, 0, 1, 0, 0, 0, 0, - 1, 1, 0, 1, 1, - 1, 0, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 189, -39, 21, 1, -3, 9, 0, 0, -3, -5, - 1, 1, - 1, - 1, 3, 3, 0, 1, 0, 0, 0,      0, - 1, 1, 0, 1, 1, - 1, 0, - 1, - 189, 39, -21, - 1, 3, -9, 0, 0, 3, 5, 1,      - 1, 1, 1, -3, -3, 0, - 1, 0, 0, 0, 0, 1, - 1, 0, - 1, - 1, 1, 0, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 210, 50, 2, 2, -6, 15, 3, 0, -2, 2, 2, -2, -2, 0, - 1, - 1, - 1, - 1, 2, 2,      0, 0, 0, 0, 0, 0, - 1, - 1, 1, 0, 210, 50, 2, 2, -6, 15, 3, 0, -2, 2, 2,      -2, -2, 0, - 1, - 1, - 1, - 1, 2, 2, 0, 0, 0, 0, 0, 0, - 1, - 1, 1, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 210, 50, 2, 2, -6, 15, 3, 0, -2, 2, 2, -2, -2, 0, - 1, - 1, - 1, - 1, 2, 2,      0, 0, 0, 0, 0, 0, - 1, - 1, 1, 0, -210, -50, -2, -2, 6, - 15, -3, 0, 2,      -2, -2, 2, 2, 0, 1, 1, 1, 1, -2, -2, 0, 0, 0, 0, 0, 0, 1, 1, - 1, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 210, 10, - 14, 10, 2, - 15, -6, 3, 6, -2, -2, -2, -2, 0, 1, 1, -2, 1, 1, 1,      - 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 210, 10, - 14, 10, 2, - 15, -6, 3, 6, -2,      -2, -2, -2, 0, 1, 1, -2, 1, 1, 1, - 1, 0, 0, 0, 0, 0, 1, 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 210, 10, - 14, 10, 2, - 15, -6, 3, 6, -2, -2, -2, -2, 0, 1, 1, -2, 1, 1, 1,      - 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, -210, - 10, 14, - 10, -2, 15, 6, -3, -6,      2, 2, 2, 2, 0, - 1, - 1, 2, - 1, - 1, - 1, 1, 0, 0, 0, 0, 0, - 1, - 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 216, -24, 24, 8, 0, -9, 0, 0, 0, -4, 4, 0, 0, 1, -3, -3, 0, - 1, 0, 0, 0,      - 1, 0, 0, 0, 1, - 1, 1, 0, 1, 216, -24, 24, 8, 0, -9, 0, 0, 0, -4, 4, 0,0, 1, -3, -3, 0, - 1, 0, 0, 0, - 1, 0, 0, 0, 1, - 1, 1, 0, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 216, -24, 24, 8, 0, -9, 0, 0, 0, -4, 4, 0, 0, 1, -3, -3, 0, - 1, 0, 0, 0,      - 1, 0, 0, 0, 1, - 1, 1, 0, 1, -216, 24, -24, -8, 0, 9, 0, 0, 0, 4, -4,      0, 0, - 1, 3, 3, 0, 1, 0, 0, 0, 1, 0, 0, 0, - 1, 1, - 1, 0, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 280, -40, -8, -8, 8, 10, 10, 1, 0, 0, 0, 0, 0, 0, 2, -2, -2, -2, - 1, 1,      - 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 280, -40, -8, -8, 8, 10, 10, 1, 0, 0, 0,      0, 0, 0, 2, -2, -2, -2, - 1, 1, - 1, 0, 0, 0, 1, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 280, -40, -8, -8, 8, 10, 10, 1, 0, 0, 0, 0, 0, 0, 2, -2, -2, -2, - 1, 1,      - 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, -280, 40, 8, 8, -8, - 10, - 10, - 1, 0, 0,      0, 0, 0, 0, -2, 2, 2, 2, 1, - 1, 1, 0, 0, 0, - 1, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 280, 40, 24, 8, 0, -5, -8, -2, 0, 4, -4, 0, 0, 0, 1, -3, 0, - 1, -2, 0, 0,      0, 0, 0, 1, 0, 1, - 1, 0, 0, 280, 40, 24, 8, 0, -5, -8, -2, 0, 4, -4, 0,      0, 0, 1, -3, 0, - 1, -2, 0, 0, 0, 0, 0, 1, 0, 1, - 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 280, 40, 24, 8, 0, -5, -8, -2, 0, 4, -4, 0, 0, 0, 1, -3, 0, - 1, -2, 0, 0,      0, 0, 0, 1, 0, 1, - 1, 0, 0, -280, -40, -24, -8, 0, 5, 8, 2, 0, -4, 4,      0, 0, 0, - 1, 3, 0, 1, 2, 0, 0, 0, 0, 0, - 1, 0, - 1, 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 315, -45, -21, 3, 3, 0, -9, 0, -5, 3, 3, 3, - 1, 0, 0, 0, 3, 0, 0, 0, 0,      0, - 1, - 1, 0, 0, 0, 0, 1, 0, 315, -45, -21, 3, 3, 0, -9, 0, -5, 3, 3,      3, - 1, 0, 0, 0, 3, 0, 0, 0, 0, 0, - 1, - 1, 0, 0, 0, 0, 1, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 315, -45, -21, 3, 3, 0, -9, 0, -5, 3, 3, 3, - 1, 0, 0, 0, 3, 0, 0, 0, 0,      0, - 1, - 1, 0, 0, 0, 0, 1, 0, -315, 45, 21, -3, -3, 0, 9, 0, 5, -3, -3,      -3, 1, 0, 0, 0, -3, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, - 1, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 336, - 16, 16, - 16, 0, 6, -6, 0, 0, 0, 0, 0, 0, 1, 2, -2, -2, 2, 2, -2, 0,      0, 0, 0, 0, - 1, 0, 0, 0, 1, 336, - 16, 16, - 16, 0, 6, -6, 0, 0, 0, 0, 0,      0, 1, 2, -2, -2, 2, 2, -2, 0, 0, 0, 0, 0, - 1, 0, 0, 0, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 336, - 16, 16, - 16, 0, 6, -6, 0, 0, 0, 0, 0, 0, 1, 2, -2, -2, 2, 2, -2, 0,      0, 0, 0, 0, - 1, 0, 0, 0, 1, -336, 16, - 16, 16, 0, -6, 6, 0, 0, 0, 0, 0,      0, - 1, -2, 2, 2, -2, -2, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 378, -30, -6, 2, -6, -9, 0, 0, 6, 2, 2, -2, 2, -2, 3, 3, 0, - 1, 0, 0, 0,      0, 0, 0, 0, 0, - 1, - 1, 0, 1, 378, -30, -6, 2, -6, -9, 0, 0, 6, 2, 2,      -2, 2, -2, 3, 3, 0, - 1, 0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, 0, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 378, -30, -6, 2, -6, -9, 0, 0, 6, 2, 2, -2, 2, -2, 3, 3, 0, - 1, 0, 0, 0,      0, 0, 0, 0, 0, - 1, - 1, 0, 1, -378, 30, 6, -2, 6, 9, 0, 0, -6, -2, -2,      2, -2, 2, -3, -3, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 405, 45, -27, -3, -3, 0, 0, 0, -3, -3, -3, 5, 1, 0, 0, 0, 0, 0, 0, 0, 0,      - 1, 1, 1, 0, 0, 0, 0, 0, 0, 405, 45, -27, -3, -3, 0, 0, 0, -3, -3, -3,      5, 1, 0, 0, 0, 0, 0, 0, 0, 0, - 1, 1, 1, 0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 405, 45, -27, -3, -3, 0, 0, 0, -3, -3, -3, 5, 1, 0, 0, 0, 0, 0, 0, 0, 0,      - 1, 1, 1, 0, 0, 0, 0, 0, 0, -405, -45, 27, 3, 3, 0, 0, 0, 3, 3, 3, -5,      - 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, - 1, - 1, 0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 420, 20, 4, - 12, 4, 0, -3, 3, -4, 0, 0, -4, 0, 0, -4, 4, 1, 0, - 1, 1, 1,      0, 0, 0, 0, 0, 0, 0, - 1, 0, 420, 20, 4, - 12, 4, 0, -3, 3, -4, 0, 0, -4,      0, 0, -4, 4, 1, 0, - 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, - 1, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 420, 20, 4, - 12, 4, 0, -3, 3, -4, 0, 0, -4, 0, 0, -4, 4, 1, 0, - 1, 1, 1,      0, 0, 0, 0, 0, 0, 0, - 1, 0, -420, -20, -4, 12, -4, 0, 3, -3, 4, 0, 0,      4, 0, 0, 4, -4, - 1, 0, 1, - 1, - 1, 0, 0, 0, 0, 0, 0, 0, 1, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 512, 0, 0, 0, 0, - 16, 8, -4, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0,      0, - 1, 0, 0, 0, 0, - 1, 512, 0, 0, 0, 0, - 16, 8, -4, 0, 0, 0, 0, 0, 2,      0, 0, 0, 0, 0, 0, 0, 1, 0, 0, - 1, 0, 0, 0, 0, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 512, 0, 0, 0, 0, - 16, 8, -4, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0,      0, - 1, 0, 0, 0, 0, - 1, -512, 0, 0, 0, 0, 16, -8, 4, 0, 0, 0, 0, 0, -2,      0, 0, 0, 0, 0, 0, 0, - 1, 0, 0, 1, 0, 0, 0, 0, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  output.irrepsCarterLabels.setSize(0);
  output.irrepsCarterLabels.addOnTop("\\phi_{1,0}"   );
  output.irrepsCarterLabels.addOnTop("\\phi_{1,63}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{7,46}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{7,1}"   );
  output.irrepsCarterLabels.addOnTop("\\phi_{15,28}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{15,7}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{21,6}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{21,33}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{21,36}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{21,3}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{27,2}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{27,37}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{35,22}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{35,13}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{35,4}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{35,31}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{56,30}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{56,3}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{70,18}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{70,9}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{84,12}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{84,15}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{105,26}");
  output.irrepsCarterLabels.addOnTop("\\phi_{105,5}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{105,6}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{105,21}");
  output.irrepsCarterLabels.addOnTop("\\phi_{105,12}");
  output.irrepsCarterLabels.addOnTop("\\phi_{105,15}");
  output.irrepsCarterLabels.addOnTop("\\phi_{120,4}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{120,25}");
  output.irrepsCarterLabels.addOnTop("\\phi_{168,6}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{168,21}");
  output.irrepsCarterLabels.addOnTop("\\phi_{189,10}");
  output.irrepsCarterLabels.addOnTop("\\phi_{189,17}");
  output.irrepsCarterLabels.addOnTop("\\phi_{189,22}");
  output.irrepsCarterLabels.addOnTop("\\phi_{189,5}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{189,20}");
  output.irrepsCarterLabels.addOnTop("\\phi_{189,7}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{210,6}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{210,21}");
  output.irrepsCarterLabels.addOnTop("\\phi_{210,10}");
  output.irrepsCarterLabels.addOnTop("\\phi_{210,13}");
  output.irrepsCarterLabels.addOnTop("\\phi_{216,16}");
  output.irrepsCarterLabels.addOnTop("\\phi_{216,9}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{280,18}");
  output.irrepsCarterLabels.addOnTop("\\phi_{280,9}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{280,8}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{280,17}");
  output.irrepsCarterLabels.addOnTop("\\phi_{315,16}");
  output.irrepsCarterLabels.addOnTop("\\phi_{315,7}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{336,14}");
  output.irrepsCarterLabels.addOnTop("\\phi_{336,11}");
  output.irrepsCarterLabels.addOnTop("\\phi_{378,14}");
  output.irrepsCarterLabels.addOnTop("\\phi_{378,9}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{405,8}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{405,15}");
  output.irrepsCarterLabels.addOnTop("\\phi_{420,10}");
  output.irrepsCarterLabels.addOnTop("\\phi_{420,13}");
  output.irrepsCarterLabels.addOnTop("\\phi_{512,12}");
  output.irrepsCarterLabels.addOnTop("\\phi_{512,11}");
  return true;
}

bool LoadCharTableE1_8(WeylGroupData& output) {
  FiniteGroup<ElementWeylGroup>& outputG = output.theGroup;
  outputG.characterTable.setExpectedSize(112); outputG.characterTable.setSize(0);
  ClassFunction<FiniteGroup<ElementWeylGroup>, Rational> currentCF;
  currentCF.G = &outputG;

  currentCF.data.assignString("[ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ]"                                                                         ); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, - 1, - 1, - 1, - 1, - 1,      - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,      - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,      - 1, - 1, - 1, - 1, - 1, - 1 ]"              ); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 28, 28, -4, 4, 4, 4, -4, 10, 10, 10, 10, 1, 1, 1, 1, 8, 8, 0, 0, 0, 0, 0,3, 3, 3, 3, 2, 2, 2, -2, -2, -2, 5, 5, - 1, - 1, 1, 1, 1, - 1, 0, 0, -2,      2, 2, 1, 1, 1, 1, - 1, - 1, - 1, 2, 2, 2, 2, - 1, - 1, 0, 0, 0, 0, 0, 0, 0,      14, 14, -2, -2, 6, 6, -2, 2, 2, 2, -2, 2, 2, 5, 5, -2, -2, - 1, - 1, 1,      1, 1, 1, 0, 0, 0, 0, - 1, - 1, 4, 4, 0, 0, 1, - 1, - 1, - 1, 0, 0, - 1, - 1,      1, 1, 0, 0, 2, 2 ]"                              ); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 28, 28, -4, 4, 4, 4, -4, 10, 10, 10, 10, 1, 1, 1, 1, 8, 8, 0, 0, 0, 0, 0,      3, 3, 3, 3, 2, 2, 2, -2, -2, -2, 5, 5, - 1, - 1, 1, 1, 1, - 1, 0, 0, -2,      2, 2, 1, 1, 1, 1, - 1, - 1, - 1, 2, 2, 2, 2, - 1, - 1, 0, 0, 0, 0, 0, 0, 0,      - 14, - 14, 2, 2, -6, -6, 2, -2, -2, -2, 2, -2, -2, -5, -5, 2, 2, 1, 1,      - 1, - 1, - 1, - 1, 0, 0, 0, 0, 1, 1, -4, -4, 0, 0, - 1, 1, 1, 1, 0, 0, 1,      1, - 1, - 1, 0, 0, -2, -2 ]"                 ); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 35, 35, 3, 11, 11, -5, 3, 14, 14, 5, 5, - 1, - 1, 2, 2, 7, 7, - 1, 3, 3, - 1,      - 1, 5, 5, 0, 0, 6, 6, -3, 2, 2, 1, 3, 3, 0, 0, 2, 2, -2, 0, 0, 0, 1, 1,      1, 2, 2, - 1, - 1, 1, 1, 0, -2, -2, 1, 1, 1, 1, 0, 0, - 1, - 1, - 1, 0, 0,      21, 21, 5, 5, 9, 9, 1, 1, 1, -3, 1, 6, 6, 3, 3, 2, 2, 0, 0, - 1, - 1, 2,      2, 3, 3, - 1, - 1, 1, 1, 4, 4, 0, 0, -2, 0, 1, 1, 0, 0, 0, 0, - 1, - 1, 0,      0, 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 35, 35, 3, 11, 11, -5, 3, 14, 14, 5, 5, - 1, - 1, 2, 2, 7, 7, - 1, 3, 3, - 1,      - 1, 5, 5, 0, 0, 6, 6, -3, 2, 2, 1, 3, 3, 0, 0, 2, 2, -2, 0, 0, 0, 1, 1,      1, 2, 2, - 1, - 1, 1, 1, 0, -2, -2, 1, 1, 1, 1, 0, 0, - 1, - 1, - 1, 0, 0,      -21, -21, -5, -5, -9, -9, - 1, - 1, - 1, 3, - 1, -6, -6, -3, -3, -2, -2, 0,      0, 1, 1, -2, -2, -3, -3, 1, 1, - 1, - 1, -4, -4, 0, 0, 2, 0, - 1, - 1, 0,      0, 0, 0, 1, 1, 0, 0, - 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 70, 70, 6, - 10, - 10, 6, 6, 10, 10, 19, 19, -2, -2, 4, 4, 14, 14, -2, -2,-2, 2, -2, 0, 0, 5, 5, -6, -6, 3, 2, 2, 3, 6, 6, 0, 0, -4, -4, 0, 0, 0,      0, 2, 2, 2, -2, -2, 1, 1, 0, 0, 1, 2, 2, - 1, - 1, 2, 2, -2, -2, - 1, 0,      0, - 1, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 50, 50, 18, 10, 10, 10, 2, 5, 5, 5, 5, -4, -4, 5, 5, -2, -2, 6, 2, 2, 2,      2, 0, 0, 0, 0, -3, -3, -3, 1, 1, 1, 0, 0, 3, 3, 1, 1, 1, - 1, 1, 1, 0,      0, 0, - 1, - 1, - 1, - 1, 0, 0, 0, 1, 1, 1, 1, -2, -2, - 1, - 1, - 1, 0, 0, 0,      0, 20, 20, 4, 4, 0, 0, 8, 0, 0, 4, 0, 5, 5, 2, 2, 1, 1, - 1, - 1, -2, -2,      1, 1, -2, -2, 2, 0, 0, 0, - 1, - 1, 3, 3, - 1, 1, 0, 0, - 1, - 1, - 1, - 1, 0,      0, 1, 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 50, 50, 18, 10, 10, 10, 2, 5, 5, 5, 5, -4, -4, 5, 5, -2, -2, 6, 2, 2, 2,      2, 0, 0, 0, 0, -3, -3, -3, 1, 1, 1, 0, 0, 3, 3, 1, 1, 1, - 1, 1, 1, 0,      0, 0, - 1, - 1, - 1, - 1, 0, 0, 0, 1, 1, 1, 1, -2, -2, - 1, - 1, - 1, 0, 0, 0,      0, -20, -20, -4, -4, 0, 0, -8, 0, 0, -4, 0, -5, -5, -2, -2, - 1, - 1, 1,      1, 2, 2, - 1, - 1, 2, 2, -2, 0, 0, 0, 1, 1, -3, -3, 1, - 1, 0, 0, 1, 1, 1,      1, 0, 0, - 1, - 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 84, 84, 20, 20, 20, 4, 4, 21, 21, -6, -6, 3, 3, 3, 3, 4, 4, 4, 4, 4, 0,      0, 4, 4, - 1, - 1, 5, 5, 2, 5, 5, -2, - 1, - 1, 5, - 1, - 1, - 1, 1, 1, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, 1, 1, -2, -2, 1, 1, 1, 1, 0, 1, 1, - 1,      - 1, 42, 42, 10, 10, 10, 10, 10, 2, 2, 2, 2, 9, 9, -3, -3, 1, 1, 3, 3,      1, 1, 1, 1, 2, 2, 2, 0, 2, 2, 1, 1, 1, 1, 1, - 1, - 1, - 1, 0, 0, 0, 0, 0,      0, - 1, - 1, - 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 84, 84, 20, 20, 20, 4, 4, 21, 21, -6, -6, 3, 3, 3, 3, 4, 4, 4, 4, 4, 0,      0, 4, 4, - 1, - 1, 5, 5, 2, 5, 5, -2, - 1, - 1, 5, - 1, - 1, - 1, 1, 1, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, 1, 1, -2, -2, 1, 1, 1, 1, 0, 1, 1, - 1,      - 1, -42, -42, - 10, - 10, - 10, - 10, - 10, -2, -2, -2, -2, -9, -9, 3, 3,      - 1, - 1, -3, -3, - 1, - 1, - 1, - 1, -2, -2, -2, 0, -2, -2, - 1, - 1, - 1, - 1,      - 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 168, 168, 40, 8, 8, 24, 8, - 12, - 12, 15, 15, 6, 6, 6, 6, 8, 8, 8, 0, 0,      4, 0, -2, -2, 3, 3, 4, 4, 7, -4, -4, 3, -2, -2, -2, 4, 2, 2, 0, 2, 0,      0, 0, 0, 0, 0, 0, 0, 0, -2, -2, - 1, -4, -4, - 1, - 1, 2, 2, 0, 0, 1, -2,      -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 175, 175, - 17, 15, 15, 15, - 1, -5, -5, -5, -5, 13, 13, 4, 4, - 1, - 1, - 1,      - 1, - 1, - 1, 3, 0, 0, 0, 0, -5, -5, -5, 3, 3, 3, 1, 1, -2, -2, 0, 0, 0,      2, 0, 0, - 1, - 1, - 1, 1, 1, 1, 1, 0, 0, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1,      - 1, - 1, 0, 0, 0, 0, 35, 35, 3, 3, -5, -5, -5, -5, -5, 3, 3, 5, 5, - 1,      - 1, -3, -3, 2, 2, 3, 3, 0, 0, - 1, - 1, - 1, 1, 0, 0, 1, 1, 1, 1, -2, 0,      1, 1, 0, 0, - 1, - 1, 0, 0, - 1, - 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 175, 175, - 17, 15, 15, 15, - 1, -5, -5, -5, -5, 13, 13, 4, 4, - 1, - 1, - 1,      - 1, - 1, - 1, 3, 0, 0, 0, 0, -5, -5, -5, 3, 3, 3, 1, 1, -2, -2, 0, 0, 0,      2, 0, 0, - 1, - 1, - 1, 1, 1, 1, 1, 0, 0, 0, - 1, - 1, - 1, - 1, - 1, - 1, - 1,      - 1, - 1, 0, 0, 0, 0, -35, -35, -3, -3, 5, 5, 5, 5, 5, -3, -3, -5, -5, 1,      1, 3, 3, -2, -2, -3, -3, 0, 0, 1, 1, 1, - 1, 0, 0, - 1, - 1, - 1, - 1, 2, 0,      - 1, - 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 210, 210, - 14, 26, 26, 10, 2, 39, 39, - 15, - 15, -6, -6, 3, 3, 6, 6, -2,      2, 2, -2, 2, 5, 5, 0, 0, 7, 7, 1, - 1, - 1, 1, -2, -2, -5, 1, - 1, - 1, 1,- 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 3, 3, -3, -3, 0, 0, - 1, - 1, 1,      - 1, - 1, 0, 0, 84, 84, 4, 4, 16, 16, -8, 0, 0, 4, 0, 9, 9, -6, -6, 1, 1,      -3, -3, -2, -2, 1, 1, 2, 2, -2, 0, - 1, - 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,      0, 0, 0, 1, 1, - 1, - 1, - 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 210, 210, - 14, 26, 26, 10, 2, 39, 39, - 15, - 15, -6, -6, 3, 3, 6, 6, -2,      2, 2, -2, 2, 5, 5, 0, 0, 7, 7, 1, - 1, - 1, 1, -2, -2, -5, 1, - 1, - 1, 1,      - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 3, 3, -3, -3, 0, 0, - 1, - 1, 1,      - 1, - 1, 0, 0, -84, -84, -4, -4, - 16, - 16, 8, 0, 0, -4, 0, -9, -9, 6, 6,      - 1, - 1, 3, 3, 2, 2, - 1, - 1, -2, -2, 2, 0, 1, 1, - 1, - 1, - 1, - 1, - 1, - 1,      0, 0, 0, 0, 0, 0, - 1, - 1, 1, 1, 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 420, 420, -28, 20, 20, 36, 4, -30, -30, 24, 24, - 12, - 12, 6, 6, 12, 12,      -4, -4, -4, 0, 4, 0, 0, 5, 5, 2, 2, 8, 2, 2, 0, -4, -4, 2, -4, 2, 2, 0,      -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -6, -6, 0, 0, 0, 0, 2, 2, 0, 0,      0, - 1, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 300, 300, 12, 20, 20, 20, 12, 30, 30, 30, 30, 3, 3, -6, -6, 8, 8, 0, 0,      0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 2, 2, 2, 3, 3, 0, 0, 2, 2, 2, 0, - 1, - 1,      2, -2, -2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, - 1, - 1, 0, 0, 0, 0, 0, 0,      0, 90, 90, 10, 10, 10, 10, 2, -2, -2, 6, 2, 0, 0, 9, 9, 4, 4, 0, 0, 1,      1, -2, -2, 0, 0, 0, 0, 0, 0, 2, 2, -2, -2, 2, 0, 1, 1, - 1, - 1, 0, 0, 0,      0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 300, 300, 12, 20, 20, 20, 12, 30, 30, 30, 30, 3, 3, -6, -6, 8, 8, 0, 0,      0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 2, 2, 2, 3, 3, 0, 0, 2, 2, 2, 0, - 1, - 1,      2, -2, -2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, - 1, - 1, 0, 0, 0, 0, 0, 0,      0, -90, -90, - 10, - 10, - 10, - 10, -2, 2, 2, -6, -2, 0, 0, -9, -9, -4,      -4, 0, 0, - 1, - 1, 2, 2, 0, 0, 0, 0, 0, 0, -2, -2, 2, 2, -2, 0, - 1, - 1,      1, 1, 0, 0, 0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 350, 350, -2, - 10, - 10, - 10, -2, 35, 35, 35, 35, - 1, - 1, - 1, - 1, 26, 26,      2, -2, -2, -2, 2, 0, 0, 0, 0, -5, -5, -5, - 1, - 1, - 1, 7, 7, 1, 1, - 1,      - 1, - 1, 1, 0, 0, 0, 0, 0, - 1, - 1, - 1, - 1, 0, 0, 0, - 1, - 1, - 1, - 1, - 1,      - 1, 1, 1, 1, 0, 0, 0, 0, 70, 70, - 10, - 10, 10, 10, 2, 2, 2, -2, 2, -5,      -5, 7, 7, - 1, - 1, 1, 1, - 1, - 1, - 1, - 1, -4, -4, 0, 0, 0, 0, 5, 5, 1, 1,      - 1, 1, - 1, - 1, 0, 0, 1, 1, 0, 0, - 1, - 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 350, 350, -2, - 10, - 10, - 10, -2, 35, 35, 35, 35, - 1, - 1, - 1, - 1, 26, 26,      2, -2, -2, -2, 2, 0, 0, 0, 0, -5, -5, -5, - 1, - 1, - 1, 7, 7, 1, 1, - 1,      - 1, - 1, 1, 0, 0, 0, 0, 0, - 1, - 1, - 1, - 1, 0, 0, 0, - 1, - 1, - 1, - 1, - 1,      - 1, 1, 1, 1, 0, 0, 0, 0, -70, -70, 10, 10, - 10, - 10, -2, -2, -2, 2, -2,      5, 5, -7, -7, 1, 1, - 1, - 1, 1, 1, 1, 1, 4, 4, 0, 0, 0, 0, -5, -5, - 1,      - 1, 1, - 1, 1, 1, 0, 0, - 1, - 1, 0, 0, 1, 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 525, 525, 45, 5, 5, 5, - 19, 30, 30, 30, 30, 12, 12, 3, 3, -7, -7, 1, -3,      -3, -3, 1, 0, 0, 0, 0, 6, 6, 6, 2, 2, 2, 0, 0, 3, 3, - 1, - 1, - 1, - 1, 0,      0, - 1, - 1, - 1, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0,      0, 105, 105, -7, -7, 5, 5, 13, -3, -3, 1, -3, 0, 0, 6, 6, -4, -4, 3, 3,      2, 2, - 1, - 1, 3, 3, - 1, - 1, 0, 0, -2, -2, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 525, 525, 45, 5, 5, 5, - 19, 30, 30, 30, 30, 12, 12, 3, 3, -7, -7, 1, -3,      -3, -3, 1, 0, 0, 0, 0, 6, 6, 6, 2, 2, 2, 0, 0, 3, 3, - 1, - 1, - 1, - 1, 0,      0, - 1, - 1, - 1, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0,      0, - 105, - 105, 7, 7, -5, -5, - 13, 3, 3, - 1, 3, 0, 0, -6, -6, 4, 4, -3,      -3, -2, -2, 1, 1, -3, -3, 1, 1, 0, 0, 2, 2, -2, -2, - 1, - 1, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 567, 567, -9, 39, 39, -9, -9, 81, 81, 0, 0, 0, 0, 0, 0, 15, 15, - 1, - 1,      - 1, 3, - 1, 7, 7, -3, -3, 9, 9, 0, -3, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, - 1, - 1, - 1, 0, 0, 0, 0, - 1, - 1, 1, -3, -3, 0, 0, 0, 0, - 1, - 1, 0, 1,      1, 0, 0, 189, 189, -3, -3, 29, 29, -3, -3, -3, -3, -3, 9, 9, 0, 0, -3,      -3, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, - 1, - 1, 3, 3, - 1, - 1, 0, 0, 0, 0, 0,      0, 0, 0, - 1, - 1, 1, 1, - 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 567, 567, -9, 39, 39, -9, -9, 81, 81, 0, 0, 0, 0, 0, 0, 15, 15, - 1, - 1,      - 1, 3, - 1, 7, 7, -3, -3, 9, 9, 0, -3, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, - 1, - 1, - 1, 0, 0, 0, 0, - 1, - 1, 1, -3, -3, 0, 0, 0, 0, - 1, - 1, 0, 1,      1, 0, 0, - 189, - 189, 3, 3, -29, -29, 3, 3, 3, 3, 3, -9, -9, 0, 0, 3, 3,      0, 0, 0, 0, 0, 0, - 1, - 1, - 1, - 1, 1, 1, -3, -3, 1, 1, 0, 0, 0, 0, 0, 0,      0, 0, 1, 1, - 1, - 1, 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 1134, 1134, - 18, - 18, - 18, 30, - 18, 0, 0, 81, 81, 0, 0, 0, 0, 30, 30, -2,      6, 6, 2, -2, -6, -6, 4, 4, 0, 0, 9, 0, 0, -3, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, -2, -2, -2, 0, 0, 0, 0, 2, 2, 0, 0, 0, -3, -3, 0, 0, 0, 0, - 1, 0,      0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 700, 700, 92, 20, 20, 20, -4, -20, -20, -20, -20, -2, -2, 7, 7, 0, 0, 8,      0, 0, 0, 0, 0, 0, 0, 0, -4, -4, -4, -4, -4, -4, 2, 2, - 1, - 1, - 1, - 1,      - 1, - 1, 0, 0, 2, -2, -2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 70, 70, - 10, - 10, - 10, - 10, 14, -6, -6, 2, -2, 10, 10,      -2, -2, 2, 2, -5, -5, 2, 2, - 1, - 1, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, - 1,      - 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 700, 700, 92, 20, 20, 20, -4, -20, -20, -20, -20, -2, -2, 7, 7, 0, 0, 8,      0, 0, 0, 0, 0, 0, 0, 0, -4, -4, -4, -4, -4, -4, 2, 2, - 1, - 1, - 1, - 1,      - 1, - 1, 0, 0, 2, -2, -2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, -70, -70, 10, 10, 10, 10, - 14, 6, 6, -2, 2, - 10, - 10, 2,      2, -2, -2, 5, 5, -2, -2, 1, 1, 0, 0, 0, 0, 0, 0, -2, -2, -2, -2, 1, 1,      0, 0, 0, 0, - 1, - 1, 0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 700, 700, -4, 60, 60, -20, 12, 55, 55, 10, 10, 7, 7, 4, 4, -4, -4, -4, 4,      4, 0, 0, 0, 0, 0, 0, - 1, - 1, 2, 3, 3, -2, - 1, - 1, -4, 2, 0, 0, -2, 0,0, 0, 0, 0, 0, -2, -2, 1, 1, 0, 0, 0, - 1, - 1, 2, 2, - 1, - 1, 1, 1, 0, 0,      0, 0, 0, 210, 210, 18, 18, 10, 10, -6, 2, 2, -6, 2, 15, 15, 3, 3, 3, 3,      0, 0, 3, 3, 0, 0, -2, -2, -2, 0, 0, 0, -3, -3, 1, 1, 0, 0, - 1, - 1, 0,      0, 0, 0, 0, 0, 1, 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 700, 700, -4, 60, 60, -20, 12, 55, 55, 10, 10, 7, 7, 4, 4, -4, -4, -4, 4,      4, 0, 0, 0, 0, 0, 0, - 1, - 1, 2, 3, 3, -2, - 1, - 1, -4, 2, 0, 0, -2, 0,      0, 0, 0, 0, 0, -2, -2, 1, 1, 0, 0, 0, - 1, - 1, 2, 2, - 1, - 1, 1, 1, 0, 0,      0, 0, 0, -210, -210, - 18, - 18, - 10, - 10, 6, -2, -2, 6, -2, - 15, - 15,      -3, -3, -3, -3, 0, 0, -3, -3, 0, 0, 2, 2, 2, 0, 0, 0, 3, 3, - 1, - 1, 0,      0, 1, 1, 0, 0, 0, 0, 0, 0, - 1, - 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 1400, 1400, -8, -40, -40, 40, 24, 20, 20, 65, 65, 14, 14, 8, 8, -8, -8,      -8, 0, 0, 4, 0, 0, 0, 0, 0, 4, 4, 1, -4, -4, 1, -2, -2, 4, -2, -4, -4,      -2, 0, 0, 0, 0, 0, 0, 2, 2, - 1, - 1, 0, 0, 0, 4, 4, 1, 1, -2, -2, 0, 0,      1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 840, 840, 8, 24, 24, -40, 8, -24, -24, 30, 30, 3, 3, 3, 3, 16, 16, 0, 0,      0, 0, 0, -5, -5, 0, 0, 8, 8, - 10, 0, 0, 2, - 1, - 1, - 1, - 1, 3, 3, - 1,      - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, 0, 4, 4, -2, -2, 1, 1, 0, 0, 0,      1, 1, 0, 0, 84, 84, 20, 20, -4, -4, -4, 4, 4, -4, -4, -6, -6, 3, 3, 2,      2, 3, 3, - 1, - 1, - 1, - 1, 0, 0, 0, 0, - 1, - 1, 2, 2, 2, 2, - 1, - 1, 1, 1,      0, 0, 0, 0, 1, 1, 0, 0, - 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 840, 840, 8, 24, 24, -40, 8, -24, -24, 30, 30, 3, 3, 3, 3, 16, 16, 0, 0,      0, 0, 0, -5, -5, 0, 0, 8, 8, - 10, 0, 0, 2, - 1, - 1, - 1, - 1, 3, 3, - 1,      - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, 0, 4, 4, -2, -2, 1, 1, 0, 0, 0,      1, 1, 0, 0, -84, -84, -20, -20, 4, 4, 4, -4, -4, 4, 4, 6, 6, -3, -3,      -2, -2, -3, -3, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, -2, -2, -2, -2, 1, 1, - 1,      - 1, 0, 0, 0, 0, - 1, - 1, 0, 0, 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 1680, 1680, 16, -80, -80, - 16, 16, 60, 60, 6, 6, 6, 6, 6, 6, 32, 32, 0,      0, 0, 0, 0, 0, 0, -5, -5, -20, -20, -2, 4, 4, 2, -2, -2, -2, -2, -2,      -2, 2, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, -4, -4, 2, 2, 2, 2, 0,      0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 972, 972, 108, 36, 36, 36, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0,      0, -3, -3, -3, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1,      -2, 2, 2, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      162, 162, 18, 18, -6, -6, 18, 6, 6, 6, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, -3, -3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, - 1, - 1,0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 972, 972, 108, 36, 36, 36, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0,      0, -3, -3, -3, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1,      -2, 2, 2, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      - 162, - 162, - 18, - 18, 6, 6, - 18, -6, -6, -6, -2, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, 0, 0,      1, 1, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 1050, 1050, 58, 50, 50, -30, -6, 15, 15, 15, 15, -3, -3, 6, 6, - 10, - 10,      -2, 2, 2, -2, -2, 0, 0, 0, 0, - 17, - 17, 7, - 1, - 1, 3, 1, 1, 4, -2, 2,      2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, - 1, - 1, - 1, - 1,      - 1, - 1, 1, 0, 0, 0, 0, 210, 210, 2, 2, - 10, - 10, 14, -2, -2, -6, -2,      15, 15, 3, 3, - 1, - 1, 0, 0, - 1, - 1, 2, 2, -4, -4, 0, 0, 0, 0, - 1, - 1,      - 1, - 1, 2, 0, 1, 1, 0, 0, 0, 0, 0, 0, - 1, - 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 1050, 1050, 58, 50, 50, -30, -6, 15, 15, 15, 15, -3, -3, 6, 6, - 10, - 10,      -2, 2, 2, -2, -2, 0, 0, 0, 0, - 17, - 17, 7, - 1, - 1, 3, 1, 1, 4, -2, 2,      2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, - 1, - 1, - 1, - 1,      - 1, - 1, 1, 0, 0, 0, 0, -210, -210, -2, -2, 10, 10, - 14, 2, 2, 6, 2,      - 15, - 15, -3, -3, 1, 1, 0, 0, 1, 1, -2, -2, 4, 4, 0, 0, 0, 0, 1, 1, 1,      1, -2, 0, - 1, - 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 2100, 2100, 116, -60, -60, 20, - 12, 30, 30, 30, 30, -6, -6, 12, 12, -20,      -20, -4, -4, -4, 0, -4, 0, 0, 0, 0, 14, 14, - 10, 6, 6, 2, 2, 2, -4, 2,      0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -2, -2, -2, -2, -2, -2,      2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 1344, 1344, 64, 64, 64, 0, 0, 84, 84, -24, -24, -6, -6, -6, -6, 0, 0, 0,      0, 0, 0, 0, - 1, - 1, 4, 4, 4, 4, -8, 4, 4, 0, -2, -2, 4, -2, -2, -2, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 1,      - 1, 1, 1, 336, 336, 16, 16, 16, 16, 16, 0, 0, 0, 0, 6, 6, -6, -6, -2,      -2, 0, 0, -2, -2, -2, -2, 0, 0, 0, 0, 1, 1, -2, -2, -2, -2, -2, 0, 0,      0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 1344, 1344, 64, 64, 64, 0, 0, 84, 84, -24, -24, -6, -6, -6, -6, 0, 0, 0,      0, 0, 0, 0, - 1, - 1, 4, 4, 4, 4, -8, 4, 4, 0, -2, -2, 4, -2, -2, -2, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 1,      - 1, 1, 1, -336, -336, - 16, - 16, - 16, - 16, - 16, 0, 0, 0, 0, -6, -6, 6,      6, 2, 2, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, - 1, - 1, 2, 2, 2, 2, 2, 0, 0, 0,      0, 0, 0, 0, - 1, - 1, 0, 0, - 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 2688, 2688, 128, 0, 0, 64, 0, -48, -48, 60, 60, - 12, - 12, - 12, - 12, 0, 0,      0, 0, 0, 0, 0, 8, 8, 3, 3, - 16, - 16, -4, 0, 0, 4, -4, -4, -4, 2, 0, 0,      -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,      2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 1400, 1400, -72, 40, 40, 40, -8, 50, 50, 50, 50, -4, -4, 5, 5, - 16, - 16,      0, 0, 0, 0, 0, 0, 0, 0, 0, -6, -6, -6, -2, -2, -2, 0, 0, -3, -3, 1, 1,      1, 1, 0, 0, 0, 0, 0, - 1, - 1, - 1, - 1, 0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0,      0, 0, 0, 0, 0, 280, 280, -8, -8, 0, 0, - 16, 0, 0, 8, 0, 10, 10, 10, 10,      -2, -2, 1, 1, -2, -2, 1, 1, 0, 0, 0, 0, 0, 0, -4, -4, 0, 0, - 1, - 1, 0,      0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 1400, 1400, -72, 40, 40, 40, -8, 50, 50, 50, 50, -4, -4, 5, 5, - 16, - 16,      0, 0, 0, 0, 0, 0, 0, 0, 0, -6, -6, -6, -2, -2, -2, 0, 0, -3, -3, 1, 1,      1, 1, 0, 0, 0, 0, 0, - 1, - 1, - 1, - 1, 0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0,      0, 0, 0, 0, 0, -280, -280, 8, 8, 0, 0, 16, 0, 0, -8, 0, - 10, - 10, - 10,      - 10, 2, 2, - 1, - 1, 2, 2, - 1, - 1, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 1, 1, 0,      0, 0, 0, - 1, - 1, 0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 1575, 1575, -57, 15, 15, 15, -9, 90, 90, -45, -45, 9, 9, 0, 0, 11, 11, 3,      - 1, - 1, - 1, - 1, 0, 0, 0, 0, -6, -6, 3, -6, -6, 3, -3, -3, 0, 0, 0, 0,      0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 2, 2, - 1, - 1, - 1, - 1, 2, 2,      - 1, 0, 0, 0, 0, 315, 315, -21, -21, 15, 15, -9, 7, 7, 3, - 1, 0, 0, -9,      -9, 0, 0, 0, 0, 3, 3, 0, 0, -3, -3, 1, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,      1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 1575, 1575, -57, 15, 15, 15, -9, 90, 90, -45, -45, 9, 9, 0, 0, 11, 11, 3,      - 1, - 1, - 1, - 1, 0, 0, 0, 0, -6, -6, 3, -6, -6, 3, -3, -3, 0, 0, 0, 0,      0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 2, 2, - 1, - 1, - 1, - 1, 2, 2,      - 1, 0, 0, 0, 0, -315, -315, 21, 21, - 15, - 15, 9, -7, -7, -3, 1, 0, 0,      9, 9, 0, 0, 0, 0, -3, -3, 0, 0, 3, 3, - 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,      - 1, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 3150, 3150, - 114, 30, 30, 30, - 18, -90, -90, 45, 45, 18, 18, 0, 0, 22,      22, 6, -2, -2, -2, -2, 0, 0, 0, 0, 6, 6, -3, 6, 6, -3, -6, -6, 0, 0, 0,      0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, -2, -2, 1, 1, -2, -2, -2,      -2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 2100, 2100, 52, -60, -60, 20, 4, 75, 75, -60, -60, -6, -6, 3, 3, 12, 12,      -4, -4, -4, 0, 0, 0, 0, 0, 0, -5, -5, 4, 3, 3, -4, -2, -2, 1, 1, 3, 3,      - 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, - 1, - 1, 0,      0, 0, 0, 0, 210, 210, - 14, - 14, 10, 10, 10, -6, -6, 2, 2, - 15, - 15, -6,      -6, 1, 1, 3, 3, -2, -2, 1, 1, -2, -2, -2, 0, 0, 0, 1, 1, 1, 1, 1, - 1,      0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 2100, 2100, 52, -60, -60, 20, 4, 75, 75, -60, -60, -6, -6, 3, 3, 12, 12,      -4, -4, -4, 0, 0, 0, 0, 0, 0, -5, -5, 4, 3, 3, -4, -2, -2, 1, 1, 3, 3,      - 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, - 1, - 1, 0,      0, 0, 0, 0, -210, -210, 14, 14, - 10, - 10, - 10, 6, 6, -2, -2, 15, 15, 6,      6, - 1, - 1, -3, -3, 2, 2, - 1, - 1, 2, 2, 2, 0, 0, 0, - 1, - 1, - 1, - 1, - 1,      1, 0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 4200, 4200, 104, 40, 40, -40, 8, - 120, - 120, 15, 15, - 12, - 12, 6, 6, 24,      24, -8, 0, 0, -4, 0, 0, 0, 0, 0, 8, 8, - 1, -8, -8, - 1, -4, -4, 2, 2,      -2, -2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0,      0, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 2240, 2240, -64, 64, 64, 0, 0, -4, -4, -40, -40, - 10, - 10, 2, 2, 0, 0, 0,      0, 0, 0, 0, -5, -5, 0, 0, -4, -4, 8, 4, 4, 0, 2, 2, -4, 2, -2, -2, 0,      0, 0, 0, 0, 0, 0, 2, 2, - 1, - 1, - 1, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      1, 1, 0, 0, 336, 336, 16, 16, - 16, - 16, - 16, 0, 0, 0, 0, 6, 6, -6, -6,      -2, -2, 0, 0, -2, -2, -2, -2, 0, 0, 0, 0, 1, 1, 2, 2, 2, 2, 2, 0, 0, 0,      0, 0, 0, 0, - 1, - 1, 0, 0, 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 2240, 2240, -64, 64, 64, 0, 0, -4, -4, -40, -40, - 10, - 10, 2, 2, 0, 0, 0,      0, 0, 0, 0, -5, -5, 0, 0, -4, -4, 8, 4, 4, 0, 2, 2, -4, 2, -2, -2, 0,      0, 0, 0, 0, 0, 0, 2, 2, - 1, - 1, - 1, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      1, 1, 0, 0, -336, -336, - 16, - 16, 16, 16, 16, 0, 0, 0, 0, -6, -6, 6, 6,      2, 2, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, - 1, - 1, -2, -2, -2, -2, -2, 0, 0,      0, 0, 0, 0, 0, 1, 1, 0, 0, - 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 4480, 4480, - 128, 0, 0, 64, 0, -80, -80, -44, -44, -20, -20, 4, 4, 0, 0,      0, 0, 0, 0, 0, 0, 0, -5, -5, 16, 16, 4, 0, 0, 4, 4, 4, 4, -2, 0, 0, -2,      0, 0, 0, 0, 0, 0, -2, -2, 1, 1, 0, 0, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 2268, 2268, -36, 12, 12, -36, 12, 81, 81, 0, 0, 0, 0, 0, 0, - 12, - 12, 4,      -4, -4, 0, 0, -2, -2, 3, 3, 9, 9, 0, -3, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, - 1, -3, -3, 0, 0, 0, 0, - 1, - 1, 0, 1,      1, 0, 0, 378, 378, -6, -6, 10, 10, -6, -6, -6, -6, 2, -9, -9, 0, 0, 3,      3, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, -2, -2, -3, -3, 1, 1, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, - 1, - 1, 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 2268, 2268, -36, 12, 12, -36, 12, 81, 81, 0, 0, 0, 0, 0, 0, - 12, - 12, 4,      -4, -4, 0, 0, -2, -2, 3, 3, 9, 9, 0, -3, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, - 1, -3, -3, 0, 0, 0, 0, - 1, - 1, 0, 1,      1, 0, 0, -378, -378, 6, 6, - 10, - 10, 6, 6, 6, 6, -2, 9, 9, 0, 0, -3,      -3, 0, 0, 0, 0, 0, 0, -2, -2, -2, 0, 2, 2, 3, 3, - 1, - 1, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 1, 1, - 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 4536, 4536, -72, -72, -72, -24, 24, 0, 0, 81, 81, 0, 0, 0, 0, -24, -24,      8, 0, 0, -4, 0, 6, 6, 1, 1, 0, 0, 9, 0, 0, -3, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, -2, -2, 1, 0, 0, -3, -3, 0, 0, 0, 0, - 1, 0,      0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 2835, 2835, -45, 51, 51, -45, 3, -81, -81, 0, 0, 0, 0, 0, 0, 3, 3, 3, -5,      -5, 3, - 1, 5, 5, 0, 0, -9, -9, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, - 1, - 1, - 1, 0, 0, 0, 0, 1, 1, 0, 3, 3, 0, 0, 0, 0, 1, 1, 0, - 1, - 1,      0, 0, 189, 189, -3, -3, - 19, - 19, -3, -3, -3, -3, 5, 9, 9, 0, 0, -3,      -3, 0, 0, 0, 0, 0, 0, 1, 1, 1, - 1, - 1, - 1, 3, 3, - 1, - 1, 0, 0, 0, 0, 0,      0, 0, 0, 1, 1, 1, 1, - 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 2835, 2835, -45, 51, 51, -45, 3, -81, -81, 0, 0, 0, 0, 0, 0, 3, 3, 3, -5,      -5, 3, - 1, 5, 5, 0, 0, -9, -9, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, - 1, - 1, - 1, 0, 0, 0, 0, 1, 1, 0, 3, 3, 0, 0, 0, 0, 1, 1, 0, - 1, - 1,      0, 0, - 189, - 189, 3, 3, 19, 19, 3, 3, 3, 3, -5, -9, -9, 0, 0, 3, 3, 0,      0, 0, 0, 0, 0, - 1, - 1, - 1, 1, 1, 1, -3, -3, 1, 1, 0, 0, 0, 0, 0, 0, 0,      0, - 1, - 1, - 1, - 1, 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 5670, 5670, -90, -90, -90, 6, 6, 0, 0, -81, -81, 0, 0, 0, 0, 6, 6, 6, 6,      6, -2, -2, 0, 0, 5, 5, 0, 0, -9, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      -2, -2, -2, 0, 0, 0, 0, 0, 0, 1, 0, 0, 3, 3, 0, 0, 0, 0, 1, 0, 0, - 1,      - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 3200, 3200, 128, 0, 0, 0, 0, -40, -40, -40, -40, 14, 14, -4, -4,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 0, 0, 0, 2, 2, -4, -4, 0, 0,      0, 0, 1, 1, 0, 0, 0, - 1, - 1, - 1, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 160, 160, 32, 32, 0, 0, 0, 0, 0, 0, 0, -20, -20, -2, -2,      -4, -4, -2, -2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      - 1, - 1, 1, 1, 0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 3200, 3200, 128, 0, 0, 0, 0, -40, -40, -40, -40, 14, 14, -4, -4, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 0, 0, 0, 2, 2, -4, -4, 0, 0, 0, 0, 1,      1, 0, 0, 0, - 1, - 1, - 1, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, - 160, - 160, -32, -32, 0, 0, 0, 0, 0, 0, 0, 20, 20, 2, 2, 4, 4, 2,      2, -2, -2, -2, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, - 1,      - 1, 0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 4096, 4096, 0, 0, 0, 0, 0, 64, 64, 64, 64, -8, -8, -8, -8, 0, 0, 0, 0, 0,      0, 0, -4, -4, -4, -4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,      0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, - 1,      - 1, 512, 512, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 16, - 16, 8, 8, 0, 0, -4, -4,      0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, - 1, - 1, 0,      0, 0, 0, - 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 4096, 4096, 0, 0, 0, 0, 0, 64, 64, 64, 64, -8, -8, -8, -8, 0, 0, 0, 0, 0,      0, 0, -4, -4, -4, -4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,      0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, - 1,      - 1, -512, -512, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 16, -8, -8, 0, 0, 4, 4,      0, 0, 0, 0, 0, 0, 0, 0, -2, -2, 0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, 1, 1,      0, 0, 0, 0, 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 4200, 4200, -24, 40, 40, 40, 8, -30, -30, -30, -30, 15, 15, -3, -3, -8,      -8, -8, 0, 0, 0, 0, 0, 0, 0, 0, -6, -6, -6, -2, -2, -2, 3, 3, 3, 3, 1,      1, 1, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -2, -2, -2, -2, 1, 1, 0,      0, 0, 0, 0, 0, 0, 420, 420, 4, 4, -20, -20, -4, -4, -4, 4, -4, 0, 0,      -3, -3, 4, 4, 3, 3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 2, 2, -2, -2, - 1, 1,      - 1, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 4200, 4200, -24, 40, 40, 40, 8, -30, -30, -30, -30, 15, 15, -3, -3, -8,      -8, -8, 0, 0, 0, 0, 0, 0, 0, 0, -6, -6, -6, -2, -2, -2, 3, 3, 3, 3, 1,      1, 1, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -2, -2, -2, -2, 1, 1, 0,      0, 0, 0, 0, 0, 0, -420, -420, -4, -4, 20, 20, 4, 4, 4, -4, 4, 0, 0, 3,      3, -4, -4, -3, -3, - 1, - 1, - 1, - 1, 0, 0, 0, 0, 0, 0, -2, -2, 2, 2, 1,      - 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 6075, 6075, 27, -45, -45, -45, -21, 0, 0, 0, 0, 0, 0, 0, 0, -9, -9, - 1,      3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 1,      - 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 405, 405, -27, -27, - 15, - 15, 9, 9, 9, -3, 1, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 3, 3, - 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, 0, 0,      0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 6075, 6075, 27, -45, -45, -45, -21, 0, 0, 0, 0, 0, 0, 0, 0, -9, -9, - 1,      3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 1,      - 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, -405, -405, 27, 27, 15, 15, -9, -9, -9, 3, - 1, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, -3, -3, 1, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0,      0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 8, -8, 0, 4, -4, 0, 0, 5, -5, -4, 4, - 1, 1, 2, -2, 4, -4, 0, 2, -2, 0, 0,      3, -3, -2, 2, -3, 3, 0, 1, - 1, 0, 3, -3, 0, 0, -2, 2, 0, 0, 1, - 1, 0,      2, -2, 2, -2, - 1, 1, - 1, 1, 0, 1, - 1, -2, 2, 1, - 1, - 1, 1, 0, 0, 0, 1,      - 1, 6, -6, 2, -2, 4, -4, 0, 2, -2, 0, 0, 3, -3, -3, 3, - 1, 1, 0, 0, - 1,      1, 2, -2, 2, -2, 0, 0, 1, - 1, -3, 3, 1, - 1, 0, 0, - 1, 1, - 1, 1, 0, 0,      - 1, 1, - 1, 1, -2, 2 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 8, -8, 0, 4, -4, 0, 0, 5, -5, -4, 4, - 1, 1, 2, -2, 4, -4, 0, 2, -2, 0, 0,      3, -3, -2, 2, -3, 3, 0, 1, - 1, 0, 3, -3, 0, 0, -2, 2, 0, 0, 1, - 1, 0,      2, -2, 2, -2, - 1, 1, - 1, 1, 0, 1, - 1, -2, 2, 1, - 1, - 1, 1, 0, 0, 0, 1,      - 1, -6, 6, -2, 2, -4, 4, 0, -2, 2, 0, 0, -3, 3, 3, -3, 1, - 1, 0, 0, 1,      - 1, -2, 2, -2, 2, 0, 0, - 1, 1, 3, -3, - 1, 1, 0, 0, 1, - 1, 1, - 1, 0, 0,      1, - 1, 1, - 1, 2, -2 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 56, -56, 0, -4, 4, 0, 0, 11, - 11, - 16, 16, 2, -2, 2, -2, 12, - 12, 0, -2,      2, 0, 0, 1, - 1, -4, 4, 3, -3, 0, - 1, 1, 0, 6, -6, 0, 0, 2, -2, 0, 0, 0,      0, 0, 2, -2, - 1, 1, - 1, 1, 1, - 1, 0, 3, -3, 0, 0, 0, 0, 1, - 1, 0, 1,      - 1, - 1, 1, 14, - 14, -6, 6, 4, -4, 0, 2, -2, 0, 0, - 1, 1, -4, 4, 3, -3,      2, -2, 0, 0, 0, 0, -2, 2, 0, 0, - 1, 1, -3, 3, 1, - 1, 0, 0, 2, -2, 0, 0,      - 1, 1, - 1, 1, 1, - 1, - 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 56, -56, 0, -4, 4, 0, 0, 11, - 11, - 16, 16, 2, -2, 2, -2, 12, - 12, 0, -2,      2, 0, 0, 1, - 1, -4, 4, 3, -3, 0, - 1, 1, 0, 6, -6, 0, 0, 2, -2, 0, 0, 0,      0, 0, 2, -2, - 1, 1, - 1, 1, 1, - 1, 0, 3, -3, 0, 0, 0, 0, 1, - 1, 0, 1,      - 1, - 1, 1, - 14, 14, 6, -6, -4, 4, 0, -2, 2, 0, 0, 1, - 1, 4, -4, -3, 3,      -2, 2, 0, 0, 0, 0, 2, -2, 0, 0, 1, - 1, 3, -3, - 1, 1, 0, 0, -2, 2, 0, 0,      1, - 1, 1, - 1, - 1, 1, 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 112, - 112, 0, 24, -24, 0, 0, 31, -31, 4, -4, 4, -4, 4, -4, 8, -8, 0, 4,      -4, 0, 0, 7, -7, 2, -2, -9, 9, 0, 3, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 1, - 1, 1, - 1, - 1, 1, 0, - 1, 1, 2, -2, 2, -2, 1, - 1, 0, 1,      - 1, - 1, 1, 56, -56, 8, -8, 16, - 16, 0, 0, 0, 0, 0, 11, - 11, 2, -2, - 1,      1, 2, -2, 2, -2, 2, -2, 4, -4, 0, 0, 1, - 1, -3, 3, 1, - 1, 0, 0, 0, 0,      0, 0, - 1, 1, 1, - 1, 1, - 1, 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 112, - 112, 0, 24, -24, 0, 0, 31, -31, 4, -4, 4, -4, 4, -4, 8, -8, 0, 4,      -4, 0, 0, 7, -7, 2, -2, -9, 9, 0, 3, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 1, - 1, 1, - 1, - 1, 1, 0, - 1, 1, 2, -2, 2, -2, 1, - 1, 0, 1,      - 1, - 1, 1, -56, 56, -8, 8, - 16, 16, 0, 0, 0, 0, 0, - 11, 11, -2, 2, 1,      - 1, -2, 2, -2, 2, -2, 2, -4, 4, 0, 0, - 1, 1, 3, -3, - 1, 1, 0, 0, 0, 0,      0, 0, 1, - 1, - 1, 1, - 1, 1, - 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 160, - 160, 0, 16, - 16, 0, 0, 34, -34, -20, 20, -2, 2, -2, 2, 16, - 16, 0,      0, 0, 0, 0, 5, -5, 0, 0, -6, 6, 0, -2, 2, 0, 6, -6, 0, 0, -2, 2, 0, 0,      - 1, 1, 0, 0, 0, 1, - 1, 1, - 1, 1, - 1, 0, -2, 2, -2, 2, -2, 2, 0, 0, 0,      - 1, 1, 0, 0, 64, -64, 0, 0, 16, - 16, 0, 0, 0, 0, 0, 4, -4, -8, 8, 0, 0,      -2, 2, 0, 0, 0, 0, 0, 0, 0, 0, - 1, 1, -6, 6, -2, 2, 0, 0, 0, 0, 1, - 1,      1, - 1, 1, - 1, 0, 0, - 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 160, - 160, 0, 16, - 16, 0, 0, 34, -34, -20, 20, -2, 2, -2, 2, 16, - 16, 0,      0, 0, 0, 0, 5, -5, 0, 0, -6, 6, 0, -2, 2, 0, 6, -6, 0, 0, -2, 2, 0, 0,      - 1, 1, 0, 0, 0, 1, - 1, 1, - 1, 1, - 1, 0, -2, 2, -2, 2, -2, 2, 0, 0, 0,      - 1, 1, 0, 0, -64, 64, 0, 0, - 16, 16, 0, 0, 0, 0, 0, -4, 4, 8, -8, 0, 0,      2, -2, 0, 0, 0, 0, 0, 0, 0, 0, 1, - 1, 6, -6, 2, -2, 0, 0, 0, 0, - 1, 1,      - 1, 1, - 1, 1, 0, 0, 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 448, -448, 0, -32, 32, 0, 0, 28, -28, -44, 44, -2, 2, 4, -4, 32, -32, 0,      0, 0, 0, 0, -2, 2, -2, 2, 12, - 12, 0, 4, -4, 0, 6, -6, 0, 0, 4, -4, 0,      0, 0, 0, 0, 0, 0, -2, 2, 1, - 1, -2, 2, 0, -4, 4, 2, -2, 2, -2, 0, 0, 0,      -2, 2, 1, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 400, -400, 0, 40, -40, 0, 0, 25, -25, -20, 20, 4, -4, 10, - 10, -8, 8, 0,      4, -4, 0, 0, 0, 0, 0, 0, 9, -9, 0, 1, - 1, 0, 0, 0, 0, 0, -2, 2, 0, 0,      1, - 1, 0, 0, 0, -2, 2, 1, - 1, 0, 0, 0, 1, - 1, -2, 2, -2, 2, 1, - 1, 0,      0, 0, 0, 0, 120, - 120, 8, -8, 0, 0, 0, 0, 0, 0, 0, 15, - 15, -6, 6, - 1,      1, 0, 0, 2, -2, 2, -2, -4, 4, 0, 0, 0, 0, 3, -3, 3, -3, 0, 0, 0, 0, 1,      - 1, 0, 0, 0, 0, - 1, 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 400, -400, 0, 40, -40, 0, 0, 25, -25, -20, 20, 4, -4, 10, - 10, -8, 8, 0,      4, -4, 0, 0, 0, 0, 0, 0, 9, -9, 0, 1, - 1, 0, 0, 0, 0, 0, -2, 2, 0, 0,      1, - 1, 0, 0, 0, -2, 2, 1, - 1, 0, 0, 0, 1, - 1, -2, 2, -2, 2, 1, - 1, 0,      0, 0, 0, 0, - 120, 120, -8, 8, 0, 0, 0, 0, 0, 0, 0, - 15, 15, 6, -6, 1,      - 1, 0, 0, -2, 2, -2, 2, 4, -4, 0, 0, 0, 0, -3, 3, -3, 3, 0, 0, 0, 0,      - 1, 1, 0, 0, 0, 0, 1, - 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 448, -448, 0, 32, -32, 0, 0, 16, - 16, 16, - 16, 16, - 16, -2, 2, 0, 0, 0,      0, 0, 0, 0, -2, 2, -2, 2, 0, 0, 0, 8, -8, 0, 0, 0, 0, 0, 2, -2, 0, 0,      0, 0, 0, 0, 0, 1, - 1, 1, - 1, 2, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,      - 1, 1, - 1, 112, - 112, 16, - 16, 0, 0, 0, 0, 0, 0, 0, 4, -4, 4, -4, 4,      -4, 4, -4, 4, -4, -2, 2, 0, 0, 0, 0, 2, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 1, - 1, 0, 0, 0, 0, - 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 448, -448, 0, 32, -32, 0, 0, 16, - 16, 16, - 16, 16, - 16, -2, 2, 0, 0, 0,      0, 0, 0, 0, -2, 2, -2, 2, 0, 0, 0, 8, -8, 0, 0, 0, 0, 0, 2, -2, 0, 0,      0, 0, 0, 0, 0, 1, - 1, 1, - 1, 2, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,      - 1, 1, - 1, - 112, 112, - 16, 16, 0, 0, 0, 0, 0, 0, 0, -4, 4, -4, 4, -4,      4, -4, 4, -4, 4, 2, -2, 0, 0, 0, 0, -2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, - 1, 1, 0, 0, 0, 0, 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 560, -560, 0, 56, -56, 0, 0, 74, -74, 20, -20, -7, 7, 2, -2, 8, -8, 0, 4,      -4, 0, 0, 5, -5, 0, 0, -6, 6, 0, 2, -2, 0, -3, 3, 0, 0, 2, -2, 0, 0, 0,      0, 0, 0, 0, - 1, 1, - 1, 1, 1, - 1, 0, 2, -2, 2, -2, - 1, 1, -2, 2, 0, - 1,      1, 0, 0, 196, - 196, 12, - 12, 24, -24, 0, 4, -4, 0, 0, 16, - 16, 7, -7,      0, 0, -2, 2, -3, 3, 0, 0, 0, 0, 0, 0, 1, - 1, 0, 0, 0, 0, 0, 0, 1, - 1,      0, 0, 1, - 1, - 1, 1, 0, 0, 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 560, -560, 0, 56, -56, 0, 0, 74, -74, 20, -20, -7, 7, 2, -2, 8, -8, 0, 4,      -4, 0, 0, 5, -5, 0, 0, -6, 6, 0, 2, -2, 0, -3, 3, 0, 0, 2, -2, 0, 0, 0,      0, 0, 0, 0, - 1, 1, - 1, 1, 1, - 1, 0, 2, -2, 2, -2, - 1, 1, -2, 2, 0, - 1,      1, 0, 0, - 196, 196, - 12, 12, -24, 24, 0, -4, 4, 0, 0, - 16, 16, -7, 7,      0, 0, 2, -2, 3, -3, 0, 0, 0, 0, 0, 0, - 1, 1, 0, 0, 0, 0, 0, 0, - 1, 1,      0, 0, - 1, 1, 1, - 1, 0, 0, - 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 1344, - 1344, 0, 32, -32, 0, 0, -60, 60, -60, 60, -6, 6, 12, - 12, 32, -32,      0, 0, 0, 0, 0, -6, 6, -6, 6, - 12, 12, 0, -4, 4, 0, -6, 6, 0, 0, -4, 4,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, -2, 0, -4, 4, 2, -2, 2, -2, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 840, -840, 0, 4, -4, 0, 0, 21, -21, -60, 60, 3, -3, -6, 6, 20, -20, 0, 2,      -2, 0, 0, -5, 5, 0, 0, -3, 3, 0, 1, - 1, 0, 3, -3, 0, 0, -2, 2, 0, 0, 0,      0, 0, -2, 2, 0, 0, 0, 0, - 1, 1, 0, 5, -5, 2, -2, - 1, 1, - 1, 1, 0, 1,      - 1, 0, 0, 126, - 126, 10, - 10, 4, -4, 0, 2, -2, 0, 0, -9, 9, -9, 9, -5,      5, 0, 0, 1, - 1, -2, 2, -2, 2, 0, 0, 1, - 1, -3, 3, 1, - 1, 0, 0, - 1, 1,      0, 0, 0, 0, - 1, 1, 1, - 1, 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 840, -840, 0, 4, -4, 0, 0, 21, -21, -60, 60, 3, -3, -6, 6, 20, -20, 0, 2,      -2, 0, 0, -5, 5, 0, 0, -3, 3, 0, 1, - 1, 0, 3, -3, 0, 0, -2, 2, 0, 0, 0,      0, 0, -2, 2, 0, 0, 0, 0, - 1, 1, 0, 5, -5, 2, -2, - 1, 1, - 1, 1, 0, 1,      - 1, 0, 0, - 126, 126, - 10, 10, -4, 4, 0, -2, 2, 0, 0, 9, -9, 9, -9, 5,      -5, 0, 0, - 1, 1, 2, -2, 2, -2, 0, 0, - 1, 1, 3, -3, - 1, 1, 0, 0, 1, - 1,      0, 0, 0, 0, 1, - 1, - 1, 1, - 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 1008, - 1008, 0, 24, -24, 0, 0, 90, -90, 36, -36, 9, -9, 0, 0, 8, -8, 0,      -4, 4, 0, 0, 3, -3, -2, 2, -6, 6, 0, -6, 6, 0, -3, 3, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, 1, 0, 2, -2, 2, -2, - 1, 1, 2, -2, 0, 0,      0, 1, - 1, 252, -252, - 12, 12, 24, -24, 0, -4, 4, 0, 0, 0, 0, 9, -9, 0,      0, 0, 0, 3, -3, 0, 0, 0, 0, 0, 0, -3, 3, 0, 0, 0, 0, 0, 0, - 1, 1, 0, 0,      0, 0, - 1, 1, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 1008, - 1008, 0, 24, -24, 0, 0, 90, -90, 36, -36, 9, -9, 0, 0, 8, -8, 0,      -4, 4, 0, 0, 3, -3, -2, 2, -6, 6, 0, -6, 6, 0, -3, 3, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, 1, 0, 2, -2, 2, -2, - 1, 1, 2, -2, 0, 0,      0, 1, - 1, -252, 252, 12, - 12, -24, 24, 0, 4, -4, 0, 0, 0, 0, -9, 9, 0,      0, 0, 0, -3, 3, 0, 0, 0, 0, 0, 0, 3, -3, 0, 0, 0, 0, 0, 0, 1, - 1, 0, 0,      0, 0, 1, - 1, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 2016, -2016, 0, 48, -48, 0, 0, -90, 90, -36, 36, 18, - 18, 0, 0, 16, - 16,      0, -8, 8, 0, 0, 6, -6, -4, 4, 6, -6, 0, 6, -6, 0, -6, 6, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -2, 2, 0, -2, 2, -2, 2, -2, 2, -2, 2, 0,      0, 0, - 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 1296, - 1296, 0, -24, 24, 0, 0, 81, -81, 0, 0, 0, 0, 0, 0, 24, -24, 0, -4,      4, 0, 0, 1, - 1, 6, -6, 9, -9, 0, -3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,      - 1, 0, 0, 0, 0, 0, 0, 0, 1, - 1, 0, -3, 3, 0, 0, 0, 0, - 1, 1, 0, 1, - 1,      0, 0, 216, -216, -24, 24, 16, - 16, 0, 0, 0, 0, 0, -9, 9, 0, 0, 3, -3,      0, 0, 0, 0, 0, 0, -4, 4, 0, 0, 1, - 1, -3, 3, 1, - 1, 0, 0, 0, 0, - 1, 1,      0, 0, 1, - 1, - 1, 1, 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 1296, - 1296, 0, -24, 24, 0, 0, 81, -81, 0, 0, 0, 0, 0, 0, 24, -24, 0, -4,      4, 0, 0, 1, - 1, 6, -6, 9, -9, 0, -3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,      - 1, 0, 0, 0, 0, 0, 0, 0, 1, - 1, 0, -3, 3, 0, 0, 0, 0, - 1, 1, 0, 1, - 1,      0, 0, -216, 216, 24, -24, - 16, 16, 0, 0, 0, 0, 0, 9, -9, 0, 0, -3, 3,      0, 0, 0, 0, 0, 0, 4, -4, 0, 0, - 1, 1, 3, -3, - 1, 1, 0, 0, 0, 0, 1, - 1,      0, 0, - 1, 1, 1, - 1, - 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 1400, - 1400, 0, 60, -60, 0, 0, -25, 25, 20, -20, - 13, 13, 8, -8, -4, 4,      0, -2, 2, 0, 0, 0, 0, 0, 0, 15, - 15, 0, 3, -3, 0, 3, -3, 0, 0, 0, 0, 0,      0, 0, 0, 0, -2, 2, 2, -2, - 1, 1, 0, 0, 0, - 1, 1, 2, -2, - 1, 1, 1, - 1,      0, 0, 0, 0, 0, 210, -210, 6, -6, -20, 20, 0, - 10, 10, 0, 0, 15, - 15, 3,      -3, 3, -3, 0, 0, -3, 3, 0, 0, -2, 2, 0, 0, 0, 0, -3, 3, 1, - 1, 0, 0,      - 1, 1, 0, 0, 0, 0, 0, 0, 1, - 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 1400, - 1400, 0, 60, -60, 0, 0, -25, 25, 20, -20, - 13, 13, 8, -8, -4, 4,      0, -2, 2, 0, 0, 0, 0, 0, 0, 15, - 15, 0, 3, -3, 0, 3, -3, 0, 0, 0, 0, 0,      0, 0, 0, 0, -2, 2, 2, -2, - 1, 1, 0, 0, 0, - 1, 1, 2, -2, - 1, 1, 1, - 1,      0, 0, 0, 0, 0, -210, 210, -6, 6, 20, -20, 0, 10, - 10, 0, 0, - 15, 15,      -3, 3, -3, 3, 0, 0, 3, -3, 0, 0, 2, -2, 0, 0, 0, 0, 3, -3, - 1, 1, 0, 0,      1, - 1, 0, 0, 0, 0, 0, 0, - 1, 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 1400, - 1400, 0, 60, -60, 0, 0, 95, -95, -40, 40, -4, 4, -4, 4, -4, 4, 0,      -2, 2, 0, 0, 0, 0, 0, 0, -9, 9, 0, 3, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, -2, 2, - 1, 1, - 1, 1, 0, 0, 0, - 1, 1, -4, 4, 2, -2, 1, - 1, 0, 0,      0, 0, 0, 350, -350, 10, - 10, 20, -20, 0, -6, 6, 0, 0, 5, -5, - 10, 10,      1, - 1, 2, -2, -2, 2, -2, 2, 2, -2, 0, 0, 0, 0, 3, -3, - 1, 1, 0, 0, 0,      0, 0, 0, - 1, 1, 0, 0, - 1, 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 1400, - 1400, 0, 60, -60, 0, 0, 95, -95, -40, 40, -4, 4, -4, 4, -4, 4, 0,      -2, 2, 0, 0, 0, 0, 0, 0, -9, 9, 0, 3, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, -2, 2, - 1, 1, - 1, 1, 0, 0, 0, - 1, 1, -4, 4, 2, -2, 1, - 1, 0, 0,      0, 0, 0, -350, 350, - 10, 10, -20, 20, 0, 6, -6, 0, 0, -5, 5, 10, - 10,      - 1, 1, -2, 2, 2, -2, 2, -2, -2, 2, 0, 0, 0, 0, -3, 3, 1, - 1, 0, 0, 0,      0, 0, 0, 1, - 1, 0, 0, 1, - 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 2400, -2400, 0, -80, 80, 0, 0, 60, -60, 60, -60, -3, 3, 6, -6, 16, - 16,      0, 0, 0, 0, 0, 0, 0, 0, 0, 12, - 12, 0, 4, -4, 0, -3, 3, 0, 0, -2, 2, 0,      0, - 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, -4, -2, 2, 1, - 1, 0, 0, 0,      0, 0, 0, 0, 120, - 120, -24, 24, 0, 0, 0, 8, -8, 0, 0, 0, 0, 3, -3, 0,      0, 6, -6, -3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, 1, 1,      - 1, 0, 0, 0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 2400, -2400, 0, -80, 80, 0, 0, 60, -60, 60, -60, -3, 3, 6, -6, 16, - 16,      0, 0, 0, 0, 0, 0, 0, 0, 0, 12, - 12, 0, 4, -4, 0, -3, 3, 0, 0, -2, 2, 0,      0, - 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, -4, -2, 2, 1, - 1, 0, 0, 0,      0, 0, 0, 0, - 120, 120, 24, -24, 0, 0, 0, -8, 8, 0, 0, 0, 0, -3, 3, 0,      0, -6, 6, 3, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, - 1, - 1,      1, 0, 0, 0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 2800, -2800, 0, -40, 40, 0, 0, 55, -55, -80, 80, -8, 8, 10, - 10, -24, 24,      0, -4, 4, 0, 0, 0, 0, 0, 0, -9, 9, 0, - 1, 1, 0, 0, 0, 0, 0, 2, -2, 0,      0, 0, 0, 0, 0, 0, 1, - 1, 1, - 1, 0, 0, 0, 3, -3, 0, 0, 0, 0, - 1, 1, 0,      0, 0, 0, 0, 280, -280, -24, 24, 0, 0, 0, 0, 0, 0, 0, -5, 5, -8, 8, 3,      -3, -2, 2, 0, 0, 0, 0, 4, -4, 0, 0, 0, 0, 3, -3, 3, -3, 0, 0, 0, 0, 0,      0, 1, - 1, 0, 0, 1, - 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 2800, -2800, 0, -40, 40, 0, 0, 55, -55, -80, 80, -8, 8, 10, - 10, -24, 24,      0, -4, 4, 0, 0, 0, 0, 0, 0, -9, 9, 0, - 1, 1, 0, 0, 0, 0, 0, 2, -2, 0,      0, 0, 0, 0, 0, 0, 1, - 1, 1, - 1, 0, 0, 0, 3, -3, 0, 0, 0, 0, - 1, 1, 0,      0, 0, 0, 0, -280, 280, 24, -24, 0, 0, 0, 0, 0, 0, 0, 5, -5, 8, -8, -3,      3, 2, -2, 0, 0, 0, 0, -4, 4, 0, 0, 0, 0, -3, 3, -3, 3, 0, 0, 0, 0, 0,      0, - 1, 1, 0, 0, - 1, 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 5600, -5600, 0, -80, 80, 0, 0, - 10, 10, - 100, 100, 2, -2, -4, 4, 16, - 16,      0, 8, -8, 0, 0, 0, 0, 0, 0, 6, -6, 0, -2, 2, 0, -6, 6, 0, 0, 4, -4, 0,      0, 0, 0, 0, 0, 0, 2, -2, - 1, 1, 0, 0, 0, -2, 2, -2, 2, -2, 2, 2, -2, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 3240, -3240, 0, 84, -84, 0, 0, 81, -81, 0, 0, 0, 0, 0, 0, - 12, 12, 0, 2,      -2, 0, 0, -5, 5, 0, 0, 9, -9, 0, -3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 1,      1, 0, 2, -2, 0, 0, 0, 0, - 1, 1, 0, -3, 3, 0, 0, 0, 0, - 1, 1, 0, 1, - 1,      0, 0, 594, -594, 6, -6, -4, 4, 0, 6, -6, 0, 0, 9, -9, 0, 0, -3, 3, 0,      0, 0, 0, 0, 0, -2, 2, 0, 0, - 1, 1, 3, -3, - 1, 1, 0, 0, 0, 0, - 1, 1, 0,      0, 1, - 1, 1, - 1, - 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 3240, -3240, 0, 84, -84, 0, 0, 81, -81, 0, 0, 0, 0, 0, 0, - 12, 12, 0, 2,      -2, 0, 0, -5, 5, 0, 0, 9, -9, 0, -3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 1,      1, 0, 2, -2, 0, 0, 0, 0, - 1, 1, 0, -3, 3, 0, 0, 0, 0, - 1, 1, 0, 1, - 1,      0, 0, -594, 594, -6, 6, 4, -4, 0, -6, 6, 0, 0, -9, 9, 0, 0, 3, -3, 0,      0, 0, 0, 0, 0, 2, -2, 0, 0, 1, - 1, -3, 3, 1, - 1, 0, 0, 0, 0, 1, - 1, 0,      0, - 1, 1, - 1, 1, 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 3360, -3360, 0, 16, - 16, 0, 0, -6, 6, -60, 60, 12, - 12, -6, 6, - 16, 16,      0, 0, 0, 0, 0, 5, -5, 0, 0, 18, - 18, 0, -2, 2, 0, 0, 0, 0, 0, -2, 2, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, - 1, 0, 2, -2, 2, -2, 2, -2, 0, 0, 0,      - 1, 1, 0, 0, 336, -336, - 16, 16, - 16, 16, 0, 0, 0, 0, 0, 6, -6, -6, 6,      2, -2, 0, 0, 2, -2, 2, -2, 0, 0, 0, 0, 1, - 1, 0, 0, -4, 4, 0, 0, 0, 0,      0, 0, 0, 0, - 1, 1, 0, 0, 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 3360, -3360, 0, 16, - 16, 0, 0, -6, 6, -60, 60, 12, - 12, -6, 6, - 16, 16,      0, 0, 0, 0, 0, 5, -5, 0, 0, 18, - 18, 0, -2, 2, 0, 0, 0, 0, 0, -2, 2, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, - 1, 0, 2, -2, 2, -2, 2, -2, 0, 0, 0,      - 1, 1, 0, 0, -336, 336, 16, - 16, 16, - 16, 0, 0, 0, 0, 0, -6, 6, 6, -6,      -2, 2, 0, 0, -2, 2, -2, 2, 0, 0, 0, 0, - 1, 1, 0, 0, 4, -4, 0, 0, 0, 0,      0, 0, 0, 0, 1, - 1, 0, 0, - 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 7168, -7168, 0, 0, 0, 0, 0, - 128, 128, 16, - 16, -32, 32, -8, 8, 0, 0, 0,      0, 0, 0, 0, 8, -8, -2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, -2, 2, 1, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, -2, 1,      - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 4096, -4096, 0, 0, 0, 0, 0, 64, -64, 64, -64, -8, 8, -8, 8, 0,      0, 0, 0, 0, 0, 0, -4, 4, -4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 1, - 1, 0, 0, 0, 1, - 1, 1, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      - 1, 1, - 1, 1, 512, -512, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 16, 16, 8, -8, 0,      0, -4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 2, -2, 0, 0, 0, 0, 0, 0, 0, 0, 1, - 1,      - 1, 1, 0, 0, 0, 0, - 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 4096, -4096, 0, 0, 0, 0, 0, 64, -64, 64, -64, -8, 8, -8, 8, 0, 0, 0, 0,      0, 0, 0, -4, 4, -4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, - 1,      0, 0, 0, 1, - 1, 1, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, 1, - 1,      1, -512, 512, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, - 16, -8, 8, 0, 0, 4, -4,      0, 0, 0, 0, 0, 0, 0, 0, -2, 2, 0, 0, 0, 0, 0, 0, 0, 0, - 1, 1, 1, - 1, 0,      0, 0, 0, 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 4200, -4200, 0, 20, -20, 0, 0, -75, 75, 60, -60, 15, - 15, 6, -6, 4, -4,      0, 2, -2, 0, 0, 0, 0, 0, 0, -3, 3, 0, -7, 7, 0, 3, -3, 0, 0, 2, -2, 0,      0, 0, 0, 0, -2, 2, 0, 0, 0, 0, 0, 0, 0, 1, - 1, -2, 2, 1, - 1, - 1, 1, 0,      0, 0, 0, 0, 210, -210, -26, 26, -20, 20, 0, -2, 2, 0, 0, 15, - 15, 3,      -3, -5, 5, 0, 0, 1, - 1, -2, 2, 2, -2, 0, 0, 0, 0, -3, 3, 1, - 1, 0, 0,      1, - 1, 0, 0, 0, 0, 0, 0, - 1, 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 4200, -4200, 0, 20, -20, 0, 0, -75, 75, 60, -60, 15, - 15, 6, -6, 4, -4,      0, 2, -2, 0, 0, 0, 0, 0, 0, -3, 3, 0, -7, 7, 0, 3, -3, 0, 0, 2, -2, 0,      0, 0, 0, 0, -2, 2, 0, 0, 0, 0, 0, 0, 0, 1, - 1, -2, 2, 1, - 1, - 1, 1, 0,      0, 0, 0, 0, -210, 210, 26, -26, 20, -20, 0, 2, -2, 0, 0, - 15, 15, -3,      3, 5, -5, 0, 0, - 1, 1, 2, -2, -2, 2, 0, 0, 0, 0, 3, -3, - 1, 1, 0, 0,      - 1, 1, 0, 0, 0, 0, 0, 0, 1, - 1, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 4536, -4536, 0, 60, -60, 0, 0, -81, 81, 0, 0, 0, 0, 0, 0, 12, - 12, 0, -2,      2, 0, 0, -4, 4, 6, -6, -9, 9, 0, 3, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 2, -2, 0, 0, 0, 0, 0, 0, 0, 3, -3, 0, 0, 0, 0, 1, - 1, 0, - 1, 1,      0, 0, 378, -378, 30, -30, -20, 20, 0, 6, -6, 0, 0, -9, 9, 0, 0, 3, -3,      0, 0, 0, 0, 0, 0, 2, -2, 0, 0, -2, 2, -3, 3, 1, - 1, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, - 1, 1, 1, - 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 4536, -4536, 0, 60, -60, 0, 0, -81, 81, 0, 0, 0, 0, 0, 0, 12, - 12, 0, -2,      2, 0, 0, -4, 4, 6, -6, -9, 9, 0, 3, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      0, 0, 2, -2, 0, 0, 0, 0, 0, 0, 0, 3, -3, 0, 0, 0, 0, 1, - 1, 0, - 1, 1,      0, 0, -378, 378, -30, 30, 20, -20, 0, -6, 6, 0, 0, 9, -9, 0, 0, -3, 3,      0, 0, 0, 0, 0, 0, -2, 2, 0, 0, 2, -2, 3, -3, - 1, 1, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 1, - 1, - 1, 1 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 5600, -5600, 0, -80, 80, 0, 0, 20, -20, 20, -20, 11, - 11, 2, -2, - 16, 16,      0, 0, 0, 0, 0, 0, 0, 0, 0, - 12, 12, 0, 4, -4, 0, 3, -3, 0, 0, -2, 2, 0,      0, 0, 0, 0, 0, 0, - 1, 1, - 1, 1, 0, 0, 0, -4, 4, 2, -2, - 1, 1, 0, 0, 0,      0, 0, 0, 0, 280, -280, 8, -8, 0, 0, 0, -8, 8, 0, 0, -20, 20, 1, - 1, -4,      4, 4, -4, - 1, 1, 2, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, - 1, 0,      0, 1, - 1, 0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("[ 5600, -5600, 0, -80, 80, 0, 0, 20, -20, 20, -20, 11, - 11, 2, -2, - 16, 16,     0, 0, 0, 0, 0, 0, 0, 0, 0, - 12, 12, 0, 4, -4, 0, 3, -3, 0, 0, -2, 2, 0,      0, 0, 0, 0, 0, 0, - 1, 1, - 1, 1, 0, 0, 0, -4, 4, 2, -2, - 1, 1, 0, 0, 0,      0, 0, 0, 0, -280, 280, -8, 8, 0, 0, 0, 8, -8, 0, 0, 20, -20, - 1, 1, 4,      -4, -4, 4, 1, - 1, -2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, 1, 0,      0, - 1, 1, 0, 0, 0, 0, 0, 0 ]"); outputG.characterTable.addOnTop(currentCF);

  output.irrepsCarterLabels.setSize(0);


  output.irrepsCarterLabels.addOnTop("\\phi_{1,0}"     );
  output.irrepsCarterLabels.addOnTop("\\phi_{1,120}"   );
  output.irrepsCarterLabels.addOnTop("\\phi_{28,8}"    );
  output.irrepsCarterLabels.addOnTop("\\phi_{28,68}"   );
  output.irrepsCarterLabels.addOnTop("\\phi_{35,2}"    );
  output.irrepsCarterLabels.addOnTop("\\phi_{35,74}"   );
  output.irrepsCarterLabels.addOnTop("\\phi_{70,32}"   );
  output.irrepsCarterLabels.addOnTop("\\phi_{50,8}"    );
  output.irrepsCarterLabels.addOnTop("\\phi_{50,56}"   );
  output.irrepsCarterLabels.addOnTop("\\phi_{84,4}"    );
  output.irrepsCarterLabels.addOnTop("\\phi_{84,64}"   );
  output.irrepsCarterLabels.addOnTop("\\phi_{168,24}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{175,12}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{175,36}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{210,4}"   );
  output.irrepsCarterLabels.addOnTop("\\phi_{210,52}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{420,20}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{300,8}"   );
  output.irrepsCarterLabels.addOnTop("\\phi_{300,44}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{350,14}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{350,38}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{525,12}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{525,36}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{567,6}"   );
  output.irrepsCarterLabels.addOnTop("\\phi_{567,46}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{1134,20}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{700,16}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{700,28}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{700,6}"   );
  output.irrepsCarterLabels.addOnTop("\\phi_{700,42}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{1400,20}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{840,14}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{840,26}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{1680,22}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{972,12}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{972,32}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{1050,10}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{1050,34}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{2100,20}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{1344,8}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{1344,38}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{2688,20}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{1400,8}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{1400,32}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{1575,10}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{1575,34}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{3150,18}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{2100,16}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{2100,28}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{4200,18}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{2240,10}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{2240,28}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{4480,16}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{2268,10}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{2268,30}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{4536,18}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{2835,14}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{2835,22}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{5670,18}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{3200,16}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{3200,22}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{4096,12}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{4096,26}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{4200,12}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{4200,24}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{6075,14}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{6075,22}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{8,1}"     );
  output.irrepsCarterLabels.addOnTop("\\phi_{8,91}"    );
  output.irrepsCarterLabels.addOnTop("\\phi_{56,19}"   );
  output.irrepsCarterLabels.addOnTop("\\phi_{56,49}"   );
  output.irrepsCarterLabels.addOnTop("\\phi_{112,3}"   );
  output.irrepsCarterLabels.addOnTop("\\phi_{112,63}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{160,7}"   );
  output.irrepsCarterLabels.addOnTop("\\phi_{160,55}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{448,25}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{400,7}"   );
  output.irrepsCarterLabels.addOnTop("\\phi_{400,43}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{448,9}"   );
  output.irrepsCarterLabels.addOnTop("\\phi_{448,39}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{560,5}"   );
  output.irrepsCarterLabels.addOnTop("\\phi_{560,47}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{1344,19}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{840,13}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{840,31}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{1008,9}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{1008,39}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{2016,19}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{1296,13}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{1296,33}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{1400,11}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{1400,29}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{1400,7}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{1400,37}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{2400,17}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{2400,23}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{2800,13}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{2800,25}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{5600,19}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{3240,9}"  );
  output.irrepsCarterLabels.addOnTop("\\phi_{3240,31}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{3360,13}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{3360,25}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{7168,17}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{4096,11}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{4096,27}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{4200,15}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{4200,21}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{4536,13}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{4536,23}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{5600,15}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{5600,21}" );
  return true;
}

bool LoadOutputSubgroupsFromJSData(JSData& input, WeylGroupData& inputGroup, List<SubgroupDataRootReflections>& outputSubgroups) {
  MacroRegisterFunctionWithName("LoadOutputSubgroupsFromJSData");
  HashedList<Vector<Rational> > gapRootSystem;
  if (!inputGroup.loadGAPRootSystem(gapRootSystem)) {
    return false;
  }
  SubgroupDataRootReflections readerSubgroup;
  for (int i = 0; i < input.theList.size; i ++) {
    JSData& currentSGdata = input.theList[i];
    if (currentSGdata.theList[0].theList[0].theString == "parabolic") {
      readerSubgroup.flagIsParabolic = true;
      readerSubgroup.flagIsExtendedParabolic = true;
    } else if (currentSGdata.theList[0].theList[0].theString == "pseudoParabolicNonParabolic") {
      readerSubgroup.flagIsExtendedParabolic = true;
      readerSubgroup.flagIsParabolic = false;
    } else if (currentSGdata.theList[0].theList[0].theString == "nonPseudoParabolic") {
      readerSubgroup.flagIsExtendedParabolic = false;
      readerSubgroup.flagIsParabolic = false;
    } else {
      global.fatal << "Corrupt JSON data, entry number "
      << i + 1 << ": the group labels are: "
      << currentSGdata.theList[0].toString(nullptr) << global.fatal;
    }
    std::string sgString = currentSGdata.theList[0].theList[1].theString;
    readerSubgroup.generatingSimpleRoots.setSize(0);
    for (int j = 0; j < currentSGdata.theList[0].theList[2].theList.size; j ++) {
      int theInt = 0;
      currentSGdata.theList[0].theList[2].theList[j].theInteger.getElement().isIntegerFittingInInt(&theInt);
      readerSubgroup.generatingSimpleRoots.addOnTop(gapRootSystem[- 1 + theInt]);
    }
    DynkinDiagramRootSubalgebra theSAdiagram;
    theSAdiagram.computeDiagramTypeModifyInput(readerSubgroup.generatingSimpleRoots, inputGroup);
    theSAdiagram.getDynkinType(readerSubgroup.theDynkinType);
    if (readerSubgroup.theDynkinType.toString() != sgString) {
      global.fatal << "Corrupt JSon data: hard-coded Dynkin type is: "
      << sgString << " but dynkin type is computed to be: "
      << readerSubgroup.theDynkinType.toString() << global.fatal;
    }
    if (currentSGdata.theList[1].theList.size != inputGroup.theGroup.characterTable.size) {
      global.fatal << "Corrupt JSon or non-initialized Weyl group: tau signature has " << currentSGdata.theList[1].theList.size
      << " entries "
      << " but Weyl group has " << inputGroup.theGroup.characterTable.size << " irreps. " << global.fatal;
    }
    if (
      inputGroup.theGroup.characterTable.size <= 0 ||
      inputGroup.theGroup.characterTable.size != inputGroup.theGroup.conjugacyClassCount()
    ) {
      global.fatal << "Bad input group. " << global.fatal;
    }
    if (inputGroup.theDynkinType.toString() == "F^{1}_4") {
      if (inputGroup.theGroup.conjugacyClassCount() != 25) {
        global.fatal << "Bad f4. " << global.fatal;
      }
    }
    readerSubgroup.tauSignature.setSize(inputGroup.theGroup.characterTable.size);
    for (int j = 0; j < readerSubgroup.tauSignature.size; j ++) {
      readerSubgroup.tauSignature[j] = currentSGdata.theList[1].theList[j].theInteger.getElement();
    }
    outputSubgroups.addOnTop(readerSubgroup);
  }
  return true;
}

bool LoadSignSignatureF1_4(WeylGroupData& inputGroup, List<SubgroupDataRootReflections>& outputSubgroups) {
  JSData theJSreader;
  //double startTime = global->getElapsedSeconds();
  theJSreader.readstring("[ [ [ \"parabolic\", \"0\", [ ] ], [ 1, 1, 1, 1, 2, 2, 2, 2, 4, 9, 9, 9, 9, 6, 6, 12, 4, 4, 4, 4, 8, 8, 8, 8, 16 ] ], [ [ \"parabolic\", \"A^{1}_1\", [ 24 ] ], [ 0, 0, 1, 1, 0, 2, 1, 1, 2, 3, 3, 6, 6, 3, 3, 6, 1, 1, 3, 3, 2, 6, 4, 4, 8 ] ], [ [ \"parabolic\", \"A^{2}_1\", [ 21 ] ], [ 0, 1, 0, 1, 1, 1, 0, 2, 2, 3, 6, 3, 6, 3, 3, 6, 1, 3, 1, 3, 4, 4, 2, 6, 8 ] ], [ [ \"parabolic\", \"A^{1}_2\", [ 24, 1 ] ], [ 0, 0, 1, 1, 0, 2, 0, 0, 0, 0, 0, 3, 3, 1, 1, 2, 0, 0, 2, 2, 0, 4, 1, 1, 2 ] ], [ [ \"parabolic\", \"B^{1}_2\", [ 24, 8 ] ], [ 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 3, 0, 1, 1, 0, 0, 0, 2, 0, 2, 0, 2, 2 ] ], [ [ \"parabolic\", \"A^{2}_1+A^{1}_1\", [ 21, 14 ] ], [ 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 2, 2, 4, 2, 1, 3, 0, 1, 1, 2, 1, 3, 1, 3, 4 ] ], [ [ \"parabolic\", \"A^{2}_2\", [ 21, 4 ] ], [ 0, 1, 0, 1, 0, 0, 0, 2, 0, 0, 3, 0, 3, 1, 1, 2, 0, 2, 0, 2, 1, 1, 0, 4, 2 ] ], [ [ \"parabolic\", \"B^{1}_3\", [ 24, 1, 6 ] ], [ 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0 ] ], [ [ \"parabolic\", \"A^{1}_2+A^{2}_1\", [ 21, 14, 2 ] ], [ 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 2, 1, 0, 1, 0, 0, 1, 1, 0, 2, 0, 1, 1 ] ], [ [ \"parabolic\", \"A^{2}_2+A^{1}_1\", [ 21, 4, 5 ] ], [ 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 2, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 1 ] ], [ [ \"parabolic\", \"C^{1}_3\", [ 21, 4, 9 ] ], [ 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0 ] ], [ [ \"parabolic\", \"F^{1}_4\", [ 24, 1, 6, 4 ] ], [ 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"2A^{1}_1\", [ 24, 16 ] ], [ 0, 0, 1, 1, 0, 2, 1, 1, 2, 1, 1, 4, 4, 1, 1, 2, 0, 0, 2, 2, 0, 4, 2, 2, 4 ] ], [ [ \"pseudoParabolicNonParabolic\", \"B^{1}_2+A^{1}_1\", [ 24, 8, 9 ] ], [ 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1 ] ], [ [ \"pseudoParabolicNonParabolic\", \"A^{1}_3\", [ 24, 1, 2 ] ], [ 0, 0, 1, 1, 0, 2, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 2, 0, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"A^{2}_1+2A^{1}_1\", [ 21, 14, 1 ] ], [ 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 3, 1, 0, 1, 0, 0, 1, 1, 0, 2, 1, 1, 2 ] ], [ [ \"pseudoParabolicNonParabolic\", \"B^{1}_4\", [ 24, 1, 2, 3 ] ], [ 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"A^{1}_3+A^{2}_1\", [ 21, 14, 2, 1 ] ] , [ 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"A^{2}_2+A^{1}_2\", [ 21, 4, 5, 2 ] ], [ 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"C^{1}_3+A^{1}_1\", [ 21, 4, 9, 2 ] ], [ 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"nonPseudoParabolic\", \"3A^{1}_1\", [ 24, 16, 9 ] ], [ 0, 0, 1, 1, 0, 2, 1, 1, 2, 0, 0, 3, 3, 0, 0, 0, 0, 0, 1, 1, 0, 2, 1, 1, 2 ] ], [ [ \"nonPseudoParabolic\", \"4A^{1}_1\", [ 24, 16, 9, 2 ] ], [ 0, 0, 1, 1, 0, 2, 1, 1, 2, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"nonPseudoParabolic\", \"B^{1}_2+2A^{1}_1\", [ 24, 8, 9, 2 ] ], [ 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"nonPseudoParabolic\", \"D^{1}_4\", [ 24, 1, 2, 9 ] ], [ 0, 0, 1, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ] ]");
  return LoadOutputSubgroupsFromJSData(theJSreader, inputGroup, outputSubgroups);
}

bool LoadSignSignatureE1_6(WeylGroupData& inputGroup, List<SubgroupDataRootReflections>& outputSubgroups) {
  JSData theJSreader;
  //double startTime = global->getElapsedSeconds();
  theJSreader.readstring("[ [ [ \"parabolic\", \"0\", [ ] ], [ 1, 1, 10, 6, 6, 20, 15, 15, 15, 15, 20, 20, 24, 24, 30, 30, 60, 80, 90, 60, 60, 64, 64, 81, 81 ] ], [ [ \"parabolic\", \"A^{1}_1\", [ 36 ] ], [ 0, 1, 5, 1, 5, 10, 5, 10, 5, 10, 5, 15, 10, 14, 10, 20, 30, 40, 45, 25, 35, 24, 40, 36, 45 ] ], [ [ \"parabolic\", \"2A^{1}_1\", [ 36, 23 ] ], [ 0, 1, 3, 0, 4, 4, 1, 6, 2, 7, 1, 11, 4, 8, 3, 13, 16, 20, 21, 11, 21, 8, 24, 15, 24 ] ], [ [ \"parabolic\", \"A^{1}_2\", [ 36, 2 ] ], [ 0, 1, 1, 0, 4, 4, 1, 6, 0, 5, 0, 10, 2, 6, 1, 11, 8, 12, 15, 4, 14, 4, 20, 9, 18 ] ], [ [ \"parabolic\", \"3A^{1}_1\", [ 36, 23, 15 ] ], [ 0, 1, 2, 0, 3, 1, 0, 3, 1, 5, 0, 8, 1, 5, 1, 8, 9, 10, 9, 5, 13, 2, 14, 6, 12 ] ], [ [ \"parabolic\", \"A^{1}_2+A^{1}_1\", [ 36, 2, 7 ] ], [ 0, 1, 1, 0, 3, 1, 0, 3, 0, 4, 0, 7, 1, 3, 0, 7, 5, 6, 6, 2, 9, 1, 11, 3, 9 ] ], [ [ \"parabolic\", \"A^{1}_3\", [ 36, 2, 4 ] ], [ 0, 1, 0, 0, 3, 1, 0, 3, 0, 2, 0, 6, 0, 2, 0, 5, 1, 2, 3, 0, 4, 0, 8, 1, 5 ] ], [ [ \"parabolic\", \"A^{1}_2+2A^{1}_1\", [ 36, 2, 7, 11 ] ], [ 0, 1, 1, 0, 2, 0, 0, 1, 0, 3, 0, 5, 0, 2, 0, 4, 3, 3, 2, 1, 6, 0, 6, 1, 4 ] ], [ [ \"parabolic\", \"2A^{1}_2\", [ 36, 2, 7, 3 ] ], [ 0, 1, 1, 0, 2, 0, 0, 1, 0, 3, 0, 4, 1, 1, 0, 4, 2, 2, 1, 0, 4, 0, 4, 0, 3 ] ], [ [ \"parabolic\", \"A^{1}_3+A^{1}_1\", [ 36, 2, 4, 1 ] ], [ 0, 1, 0, 0, 2, 0, 0, 1, 0, 2, 0, 4, 0, 1, 0, 3, 1, 1, 1, 0, 3, 0, 4, 0, 2 ] ], [ [ \"parabolic\", \"A^{1}_4\", [ 36, 2, 4, 5 ] ], [ 0, 1, 0, 0, 2, 0, 0, 1, 0, 1, 0, 3, 0, 0, 0, 2, 0, 0, 0, 0, 1, 0, 2, 0, 1 ] ], [ [ \"parabolic\", \"D^{1}_4\", [ 36, 2, 4, 15 ] ], [ 0, 1, 0, 0, 2, 0, 0, 1, 0, 0, 0, 3, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 2, 0, 0 ] ], [ [ \"parabolic\", \"2A^{1}_2+A^{1}_1\", [ 36, 2, 7, 3, 11 ] ], [ 0, 1, 1, 0, 1, 0, 0, 0, 0, 2, 0, 3, 0, 1, 0, 2, 1, 1, 0, 0, 3, 0, 2, 0, 1 ] ], [ [ \"parabolic\", \"A^{1}_4+A^{1}_1\", [ 36, 2, 4, 5, 1 ] ], [ 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0 ] ], [ [ \"parabolic\", \"A^{1}_5\", [ 36, 2, 4, 5, 6 ] ], [ 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"parabolic\", \"D^{1}_5\", [ 36, 2, 4, 5, 3 ] ], [ 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"parabolic\", \"E^{1}_6\", [ 36, 3, 2, 4, 5, 6 ] ], [ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"4A^{1}_1\", [ 36, 23, 15, 4 ] ], [ 0, 1, 1, 0, 2, 0, 0, 1, 1, 4, 0, 6, 0, 4, 0, 4, 6, 4, 3, 2, 8, 0, 8, 3, 6 ] ], [ [ \"pseudoParabolicNonParabolic\", \"A^{1}_3+2A^{1}_1\", [ 36, 2, 4, 1, 6 ] ], [ 0, 1, 0, 0, 1, 0, 0, 0, 0, 2, 0, 3, 0, 1, 0, 1, 1, 0, 0, 0, 2, 0, 2, 0, 1 ] ], [ [ \"pseudoParabolicNonParabolic\", \"3A^{1}_2\", [ 36, 2, 7, 3, 11, 6 ] ], [ 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 0, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"A^{1}_5+A^{1}_1\", [ 36, 2, 4, 5, 6, 1 ] ], [ 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ] ]");
  return LoadOutputSubgroupsFromJSData(theJSreader, inputGroup, outputSubgroups);
}

bool LoadSignSignatureE1_7(WeylGroupData& inputGroup, List<SubgroupDataRootReflections>& outputSubgroups) {
  JSData theJSreader;
  //double startTime = global->getElapsedSeconds();
  theJSreader.readstring("  [ [ \"parabolic\", \"0\", [ ] ], [ 1, 1, 7, 7, 15, 15, 21, 21, 21, 21, 27, 27, 35, 35, 35, 35, 56, 56, 70, 70, 84, 84, 105, 105, 105, 105, 105, 105, 120, 120, 168, 168, 189, 189, 189, 189, 189, 189, 210, 210, 210, 210, 216, 216, 280, 280, 280, 280, 315, 315, 336, 336, 378, 378, 405, 405, 420, 420, 512, 512 ] ], [ [ \"parabolic\", \"A^{1}_1\", [ 63 ] ], [ 0, 1, 6, 1, 10, 5, 6, 15, 16, 5, 6, 21, 20, 15, 10, 25, 40, 16, 40, 30, 40, 44, 70, 35, 40, 65, 50, 55, 40, 80, 64, 104, 84, 105, 120, 69, 114, 75, 80, 130, 100, 110, 120, 96, 160, 120, 120, 160, 180, 135, 176, 160, 204, 174, 180, 225, 200, 220, 256, 256 ] ], [ [ \"parabolic\", \"2A^{1}_1\", [ 63, 49 ] ], [ 0, 1, 5, 0, 7, 2, 1, 10, 12, 1, 1, 16, 10, 5, 3, 18, 28, 4, 24, 14, 20, 24, 45, 10, 16, 41, 23, 28, 12, 52, 24, 64, 34, 55, 76, 25, 67, 28, 28, 78, 50, 60, 68, 44, 88, 48, 52, 92, 102, 57, 88, 72, 110, 80, 78, 123, 92, 112, 128, 128 ] ], [ [ \"parabolic\", \"A^{1}_2\", [ 63, 1 ] ], [ 0, 1, 5, 0, 5, 0, 1, 10, 11, 0, 0, 15, 10, 5, 0, 15, 25, 1, 15, 5, 10, 14, 40, 5, 5, 30, 15, 20, 5, 45, 10, 50, 24, 45, 60, 9, 54, 15, 15, 65, 25, 35, 45, 21, 70, 30, 25, 65, 75, 30, 66, 50, 75, 45, 45, 90, 60, 80, 80, 80 ] ], [ [ \"parabolic\", \"3A^{1}_1\", [ 63, 49, 28 ] ], [ 0, 1, 4, 0, 5, 1, 0, 6, 9, 0, 0, 12, 4, 1, 1, 13, 19, 1, 15, 7, 10, 14, 28, 2, 7, 26, 11, 13, 3, 33, 8, 40, 12, 27, 48, 9, 39, 9, 9, 45, 26, 34, 39, 21, 46, 18, 23, 53, 57, 24, 42, 30, 60, 36, 33, 66, 40, 56, 64, 64 ] ], [ [ \"parabolic\", \"3A^{1}_1\", [ 63, 49, 7 ] ], [ 0, 1, 4, 0, 4, 2, 0, 6, 8, 1, 0, 12, 4, 1, 0, 14, 20, 0, 16, 6, 8, 16, 28, 2, 8, 25, 8, 16, 4, 32, 8, 40, 12, 27, 48, 9, 36, 12, 8, 46, 28, 32, 36, 24, 48, 16, 20, 56, 60, 21, 40, 32, 60, 36, 36, 63, 40, 56, 64, 64 ] ], [ [ \"parabolic\", \"A^{1}_2+A^{1}_1\", [ 63, 1, 30 ] ], [ 0, 1, 4, 0, 4, 0, 0, 6, 8, 0, 0, 11, 4, 1, 0, 11, 17, 0, 10, 3, 6, 8, 24, 1, 2, 20, 6, 10, 1, 28, 4, 30, 8, 21, 38, 3, 30, 5, 4, 37, 14, 21, 27, 10, 36, 10, 11, 38, 42, 12, 30, 20, 40, 21, 18, 48, 26, 38, 40, 40 ] ], [ [ \"parabolic\", \"A^{1}_3\", [ 63, 1, 3 ] ], [ 0, 1, 4, 0, 2, 0, 0, 6, 7, 0, 0, 10, 4, 1, 0, 8, 14, 0, 4, 0, 1, 3, 20, 0, 0, 11, 3, 5, 0, 22, 0, 20, 4, 15, 25, 0, 21, 1, 1, 27, 3, 7, 12, 2, 24, 4, 2, 20, 24, 3, 18, 10, 20, 6, 6, 27, 11, 21, 16, 16 ] ], [ [ \"parabolic\", \"4A^{1}_1\", [ 63, 49, 28, 2 ] ], [ 0, 1, 3, 0, 3, 1, 0, 3, 6, 0, 0, 9, 1, 0, 0, 10, 13, 0, 10, 3, 4, 10, 17, 0, 4, 16, 4, 6, 1, 20, 2, 26, 3, 12, 30, 3, 21, 3, 2, 25, 15, 19, 21, 12, 24, 6, 9, 32, 33, 9, 18, 12, 33, 15, 15, 33, 16, 28, 32, 32 ] ], [ [ \"parabolic\", \"A^{1}_2+2A^{1}_1\", [ 63, 1, 30, 18 ] ], [ 0, 1, 3, 0, 3, 0, 0, 3, 6, 0, 0, 8, 1, 0, 0, 8, 11, 0, 7, 2, 3, 5, 14, 0, 1, 13, 3, 4, 0, 17, 1, 19, 2, 9, 24, 1, 17, 1, 1, 20, 8, 13, 16, 5, 17, 3, 5, 22, 23, 5, 13, 7, 22, 9, 7, 25, 10, 18, 20, 20 ] ], [ [ \"parabolic\", \"2A^{1}_2\", [ 63, 1, 30, 7 ] ], [ 0, 1, 3, 0, 3, 0, 0, 3, 5, 0, 0, 7, 1, 0, 0, 7, 10, 0, 5, 1, 3, 3, 11, 0, 0, 11, 1, 4, 0, 14, 1, 13, 1, 6, 19, 0, 12, 1, 0, 16, 5, 9, 12, 2, 13, 1, 2, 16, 17, 2, 8, 4, 14, 6, 3, 18, 7, 11, 12, 12 ] ], [ [ \"parabolic\", \"A^{1}_3+A^{1}_1\", [ 63, 1, 3, 19 ] ], [ 0, 1, 3, 0, 2, 0, 0, 3, 5, 0, 0, 7, 1, 0, 0, 6, 9, 0, 3, 0, 1, 2, 11, 0, 0, 8, 1, 2, 0, 13, 0, 12, 1, 6, 16, 0, 11, 0, 0, 14, 2, 5, 8, 1, 11, 1, 1, 12, 13, 1, 7, 3, 11, 3, 2, 14, 4, 9, 8, 8 ] ] , [ [ \"parabolic\", \"A^{1}_3+A^{1}_1\", [ 63, 1, 3, 2 ] ], [ 0, 1, 3, 0, 1, 0, 0, 3, 4, 0, 0, 7, 1, 0, 0, 7, 10, 0, 4, 0, 0, 3, 11, 0, 0, 7, 0, 4, 0, 12, 0, 12, 1, 6, 16, 0, 9, 1, 0, 15, 3, 4, 6, 2, 12, 0, 0, 14, 15, 0, 6, 4, 11, 3, 3, 12, 4, 9, 8, 8 ] ], [ [ \"parabolic\", \"A^{1}_4\", [ 63, 1, 3, 4 ] ], [ 0, 1, 3, 0, 1, 0, 0, 3, 4, 0, 0, 6, 1, 0, 0, 4, 7, 0, 1, 0, 0, 0, 8, 0, 0, 4, 0, 1, 0, 9, 0, 6, 0, 3, 9, 0, 6, 0, 0, 9, 0, 1, 3, 0, 6, 0, 0, 5, 6, 0, 3, 1, 3, 0, 0, 6, 1, 3, 2, 2 ] ], [ [ \"parabolic\", \"D^{1}_4\", [ 63, 1, 3, 28 ] ], [ 0, 1, 3, 0, 0, 0, 0, 3, 4, 0, 0, 6, 1, 0, 0, 3, 6, 0, 0, 0, 0, 1, 8, 0, 0, 1, 1, 0, 0, 8, 0, 6, 0, 3, 6, 0, 6, 0, 0, 7, 0, 0, 0, 0, 4, 0, 0, 2, 3, 0, 2, 0, 3, 0, 0, 3, 0, 3, 0, 0 ] ], [ [ \"parabolic\", \"A^{1}_2+3A^{1}_1\", [ 63, 1, 30, 18, 5 ] ], [ 0, 1, 2, 0, 2, 0, 0, 1, 4, 0, 0, 6, 0, 0, 0, 6, 7, 0, 5, 1, 1, 4, 8, 0, 1, 8, 1, 1, 0, 10, 0, 13, 0, 3, 15, 0, 9, 0, 0, 10, 5, 8, 9, 3, 8, 1, 2, 13, 13, 2, 5, 2, 12, 3, 3, 12, 3, 9, 10, 10 ] ], [ [ \"parabolic\", \"2A^{1}_2+A^{1}_1\", [ 63, 1, 30, 7, 11 ] ], [ 0, 1, 2, 0, 2, 0, 0, 1, 4, 0, 0, 5, 0, 0, 0, 5, 6, 0, 4, 1, 1, 2, 6, 0, 0, 7, 1, 1, 0, 8, 0, 9, 0, 2, 12, 0, 7, 0, 0, 8, 3, 6, 7, 1, 5, 0, 1, 9, 9, 1, 3, 1, 8, 2, 1, 9, 2, 5, 6, 6 ] ], [ [ \"parabolic\", \"A^{1}_3+2A^{1}_1\", [ 63, 1, 3, 2, 19 ] ], [ 0, 1, 2, 0, 1, 0, 0, 1, 3, 0, 0, 5, 0, 0, 0, 5, 6, 0, 3, 0, 0, 2, 6, 0, 0, 5, 0, 1, 0, 7, 0, 8, 0, 2, 10, 0, 5, 0, 0, 7, 2, 3, 4, 1, 5, 0, 0, 8, 8, 0, 2, 1, 6, 1, 1, 6, 1, 4, 4, 4 ] ], [ [ \"parabolic\", \"A^{1}_3+A^{1}_2\", [ 63, 1, 3, 19, 7 ] ], [ 0, 1, 2, 0, 2, 0, 0, 1, 3, 0, 0, 4, 0, 0, 0, 4, 5, 0, 2, 0, 1, 1, 4, 0, 0, 5, 0, 1, 0, 6, 0, 5, 0, 1, 8, 0, 4, 0, 0, 5, 1, 3, 4, 0, 3, 0, 0, 5, 5, 0, 1, 0, 4, 1, 0, 5, 1, 2, 2, 2 ] ], [ [ \"parabolic\", \"A^{1}_4+A^{1}_1\", [ 63, 1, 3, 4, 13 ] ], [ 0, 1, 2, 0, 1, 0, 0, 1, 3, 0, 0, 4, 0, 0, 0, 3, 4, 0, 1, 0, 0, 0, 4, 0, 0, 3, 0, 0, 0, 5, 0, 4, 0, 1, 6, 0, 3, 0, 0, 4, 0, 1, 2, 0, 2, 0, 0, 3, 3, 0, 1, 0, 2, 0, 0, 3, 0, 1, 1, 1 ] ], [ [ \"parabolic\", \"D^{1}_4+A^{1}_1\", [ 63, 1, 3, 28, 2 ] ], [ 0, 1, 2, 0, 0, 0, 0, 1, 2, 0, 0, 4, 0, 0, 0, 3, 4, 0, 0, 0, 0, 1, 4, 0, 0, 1, 0, 0, 0, 4, 0, 4, 0, 1, 4, 0, 2, 0, 0, 3, 0, 0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 1, 0, 1, 0, 0 ] ], [ [ \"parabolic\", \"A^{1}_5\", [ 63, 1, 3, 4, 5 ] ], [ 0, 1, 2, 0, 1, 0, 0, 1, 2, 0, 0, 3, 0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 3, 0, 1, 0, 0, 3, 0, 1, 0, 0, 2, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 ] ], [ [ \"parabolic\", \"A^{1}_5\", [ 63, 1, 3, 4, 2 ] ], [ 0, 1, 2, 0, 0, 0, 0, 1, 1, 0, 0, 3, 0, 0, 0, 3, 4, 0, 1, 0, 0, 0, 2, 0, 0, 1, 0, 1, 0, 2, 0, 1, 0, 0, 3, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"parabolic\", \"D^{1}_5\", [ 63, 1, 3, 4, 16 ] ], [ 0, 1, 2, 0, 0, 0, 0, 1, 2, 0, 0, 3, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"parabolic\", \"A^{1}_3+A^{1}_2+A^{1}_1\", [ 63, 1, 3, 19, 7, 2 ] ], [ 0, 1, 1, 0, 1, 0, 0, 0, 2, 0, 0, 3, 0, 0, 0, 3, 3, 0, 2, 0, 0, 1, 2, 0, 0, 3, 0, 0, 0, 3, 0, 4, 0, 0, 5, 0, 2, 0, 0, 2, 1, 2, 2, 0, 1, 0, 0, 3, 3, 0, 0, 0, 2, 0, 0, 2, 0, 1, 1, 1 ] ], [ [ \"parabolic\", \"A^{1}_4+A^{1}_2\", [ 63, 1, 3, 4, 13, 7 ] ], [ 0, 1, 1, 0, 1, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 2, 2, 0, 1, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 2, 0, 2, 0, 0, 3, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0 ] ], [ [ \"parabolic\", \"A^{1}_5+A^{1}_1\", [ 63, 1, 3, 4, 2, 13 ] ], [ 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 2, 2, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"parabolic\", \"D^{1}_5+A^{1}_1\", [ 63, 1, 3, 4, 16, 7 ] ], [ 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"parabolic\", \"A^{1}_6\", [ 63, 1, 3, 4, 5, 6 ] ], [ 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"parabolic\", \"D^{1}_6\", [ 63, 1, 3, 4, 5, 2 ] ], [ 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"parabolic\", \"E^{1}_6\", [ 63, 16, 1, 3, 4, 12 ] ], [ 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"parabolic\", \"E^{1}_7\", [ 63, 16, 1, 3, 4, 12, 7 ] ], [ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"4A^{1}_1\", [ 63, 49, 28, 3 ] ], [ 0, 1, 3, 0, 4, 1, 0, 3, 7, 0, 0, 9, 1, 0, 1, 10, 12, 0, 9, 3, 6, 10, 17, 0, 3, 16, 7, 6, 0, 20, 2, 26, 3, 12, 30, 3, 24, 3, 3, 25, 13, 19, 24, 12, 22, 6, 12, 32, 30, 9, 20, 12, 33, 15, 12, 33, 16, 28, 32, 32 ] ], [ [ \"pseudoParabolicNonParabolic\", \"5A^{1}_1\", [ 63, 49, 28, 2, 3 ] ], [ 0, 1, 2, 0, 2, 1, 0, 1, 4, 0, 0, 7, 0, 0, 0, 8, 8, 0, 6, 1, 2, 8, 10, 0, 2, 10, 2, 2, 0, 12, 0, 18, 0, 4, 18, 1, 12, 1, 0, 13, 8, 11, 12, 8, 12, 2, 4, 20, 18, 3, 8, 4, 18, 5, 6, 15, 6, 14, 16, 16 ] ], [ [ \"pseudoParabolicNonParabolic\", \"A^{1}_3+2A^{1}_1\", [ 63, 1, 3, 19, 6 ] ], [ 0, 1, 2, 0, 2, 0, 0, 1, 4, 0, 0, 5, 0, 0, 0, 5, 5, 0, 2, 0, 1, 2, 6, 0, 0, 5, 1, 1, 0, 7, 0, 8, 0, 2, 10, 0, 7, 0, 0, 7, 1, 3, 6, 1, 4, 0, 1, 8, 6, 0, 3, 1, 6, 1, 0, 6, 1, 4, 4, 4 ] ], [ [ \"pseudoParabolicNonParabolic\", \"3A^{1}_2\", [ 63, 1, 30, 7, 11, 5 ] ] , [ 0, 1, 1, 0, 1, 0, 0, 0, 3, 0, 0, 3, 0, 0, 0, 3, 3, 0, 3, 1, 0, 1, 2, 0, 0, 4, 1, 0, 0, 3, 0, 5, 0, 0, 6, 0, 3, 0, 0, 3, 1, 3, 3, 0, 1, 0, 0, 3, 3, 0, 0, 0, 3, 0, 0, 3, 0, 1, 2, 2 ] ], [ [ \"pseudoParabolicNonParabolic\", \"A^{1}_3+3A^{1}_1\", [ 63, 1, 3, 2, 19, 6 ] ], [ 0, 1, 1, 0, 1, 0, 0, 0, 2, 0, 0, 4, 0, 0, 0, 4, 3, 0, 2, 0, 0, 2, 3, 0, 0, 3, 0, 0, 0, 4, 0, 6, 0, 0, 6, 0, 3, 0, 0, 3, 1, 2, 3, 1, 2, 0, 0, 5, 4, 0, 1, 0, 3, 0, 0, 2, 0, 2, 2, 2 ] ], [ [ \"pseudoParabolicNonParabolic\", \"2A^{1}_3\", [ 63, 1, 3, 19, 7, 6 ] ], [ 0, 1, 1, 0, 2, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 3, 2, 0, 1, 0, 1, 1, 1, 0, 0, 2, 0, 1, 0, 2, 0, 2, 0, 0, 3, 0, 2, 0, 0, 1, 0, 1, 2, 0, 0, 0, 0, 2, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"D^{1}_4+2A^{1}_1\", [ 63, 1, 3, 28, 2, 5 ] ], [ 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0, 3, 2, 0, 0, 0, 0, 1, 2, 0, 0, 1, 0, 0, 0, 2, 0, 3, 0, 0, 2, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 2, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"A^{1}_5+A^{1}_1\", [ 63, 1, 3, 4, 5, 7 ] ], [ 0, 1, 1, 0, 1, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 2, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"2A^{1}_3+A^{1}_1\", [ 63, 1, 3, 19, 7, 6, 2 ] ], [ 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 2, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 2, 0, 0, 2, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"A^{1}_5+A^{1}_2\", [ 63, 1, 3, 4, 2, 13, 7 ] ], [ 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"D^{1}_6+A^{1}_1\", [ 63, 1, 3, 4, 5, 2, 7 ] ], [ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"A^{1}_7\", [ 63, 1, 3, 4, 5, 6, 7 ] ] , [ 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"nonPseudoParabolic\", \"6A^{1}_1\", [ 63, 49, 28, 2, 3, 5 ] ], [ 0, 1, 1, 0, 1, 1, 0, 0, 2, 0, 0, 6, 0, 0, 0, 7, 4, 0, 3, 0, 1, 7, 5, 0, 1, 7, 1, 0, 0, 8, 0, 14, 0, 0, 9, 0, 6, 0, 0, 7, 4, 7, 6, 6, 6, 0, 2, 14, 9, 0, 4, 0, 9, 0, 3, 6, 3, 7, 8, 8 ] ], [ [ \"nonPseudoParabolic\", \"7A^{1}_1\", [ 63, 49, 28, 2, 3, 5, 7 ] ], [ 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 6, 0, 0, 0, 7, 0, 0, 0, 0, 0, 7, 0, 0, 0, 7, 0, 0, 0, 8, 0, 14, 0, 0, 0, 0, 0, 0, 0, 7, 0, 7, 0, 6, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 6, 0, 7, 0, 8 ] ], [ [ \"nonPseudoParabolic\", \"D^{1}_4+3A^{1}_1\", [ 63, 1, 3, 28, 2, 5, 7 ] ], [ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ] ");
  return LoadOutputSubgroupsFromJSData(theJSreader, inputGroup, outputSubgroups);
}

bool LoadSignSignatureE1_8(WeylGroupData& inputGroup, List<SubgroupDataRootReflections>& outputSubgroups) {
  JSData theJSreader;
  //double startTime = global->getElapsedSeconds();
  theJSreader.readstring("  [ [ \"parabolic\", \"0\", [ ] ], [ 1, 1, 28, 28, 35, 35, 70, 50, 50, 84, 84, 168, 175, 175, 210, 210, 420, 300, 300, 350, 350, 525, 525, 567, 567, 1134, 700, 700, 700, 700, 1400, 840, 840, 1680, 972, 972, 1050, 1050, 2100, 1344, 1344, 2688, 1400, 1400, 1575, 1575, 3150, 2100, 2100, 4200, 2240, 2240, 4480, 2268, 2268, 4536, 2835, 2835, 5670, 3200, 3200, 4096, 4096, 4200, 4200, 6075, 6075, 8, 8, 56, 56, 112, 112, 160, 160, 448, 400, 400, 448, 448, 560, 560, 1344, 840, 840, 1008, 1008, 2016, 1296, 1296, 1400, 1400, 1400, 1400, 2400, 2400, 2800, 2800, 5600, 3240, 3240, 3360, 3360, 7168, 4096, 4096, 4200, 4200, 4536, 4536, 5600, 5600 ] ], [ [ \"parabolic\", \"A^{1}_1\", [ 120 ] ], [ 0, 1, 7, 21, 7, 28, 35, 15, 35, 21, 63, 84, 70, 105, 63, 147, 210, 105, 195, 140, 210, 210, 315, 189, 378, 567, 315, 385, 245, 455, 700, 378, 462, 840, 405, 567, 420, 630, 1050, 504, 840, 1344, 560, 840, 630, 945, 1575, 945, 1155, 2100, 952, 1288, 2240, 945, 1323, 2268, 1323, 1512, 2835, 1520, 1680, 1792, 2304, 1890, 2310, 2835, 3240, 1, 7, 21, 35, 28, 84, 48, 112, 224, 140, 260, 168, 280, 182, 378, 672, 357, 483, 378, 630, 1008, 540, 756, 595, 805, 525, 875, 1140, 1260, 1260, 1540, 2800, 1323, 1917, 1512, 1848, 3584, 1792, 2304, 1995, 2205, 2079, 2457, 2660, 2940 ] ], [ [ \"parabolic\", \"2A^{1}_1\", [ 120, 97 ] ], [ 0, 1, 1, 15, 1, 22, 15, 5, 25, 5, 47, 44, 30, 65, 17, 101, 110, 35, 125, 50, 120, 80, 185, 57, 246, 279, 145, 215, 85, 295, 340, 174, 258, 400, 171, 333, 170, 380, 510, 184, 520, 672, 220, 500, 240, 555, 795, 405, 615, 1060, 408, 744, 1120, 381, 759, 1116, 627, 816, 1395, 720, 880, 768, 1280, 850, 1270, 1305, 1710, 0, 6, 6, 20, 6, 62, 12, 76, 104, 50, 170, 64, 176, 56, 252, 344, 148, 274, 132, 384, 516, 210, 426, 260, 470, 190, 540, 520, 640, 550, 830, 1380, 534, 1128, 676, 1012, 1792, 768, 1280, 950, 1160, 960, 1338, 1240, 1520 ] ], [ [ \"parabolic\", \"A^{1}_2\", [ 120, 8 ] ], [ 0, 1, 1, 15, 0, 21, 15, 0, 20, 0, 42, 24, 10, 45, 6, 90, 60, 15, 105, 35, 105, 45, 150, 27, 216, 189, 75, 145, 30, 240, 240, 90, 174, 300, 81, 243, 75, 285, 360, 84, 420, 432, 110, 390, 135, 450, 495, 270, 480, 660, 204, 540, 720, 216, 594, 756, 351, 540, 945, 440, 600, 448, 960, 480, 900, 810, 1215, 0, 6, 6, 20, 1, 57, 6, 70, 84, 15, 135, 24, 136, 20, 216, 204, 84, 210, 72, 324, 306, 135, 351, 120, 330, 90, 440, 360, 480, 345, 625, 930, 270, 864, 390, 726, 1152, 448, 960, 570, 780, 540, 918, 800, 1080 ] ], [ [ \"parabolic\", \"3A^{1}_1\", [ 120, 97, 61 ] ], [ 0, 1, 0, 10, 0, 17, 5, 2, 18, 1, 35, 24, 14, 41, 4, 68, 60, 10, 80, 15, 65, 29, 106, 15, 156, 135, 70, 120, 29, 191, 160, 80, 148, 180, 72, 198, 71, 229, 240, 64, 320, 336, 86, 294, 87, 318, 405, 163, 317, 540, 176, 432, 560, 147, 429, 540, 303, 444, 675, 336, 464, 320, 704, 382, 698, 594, 891, 0, 5, 1, 10, 1, 45, 2, 50, 44, 19, 111, 24, 112, 16, 166, 180, 58, 155, 42, 228, 270, 75, 231, 118, 277, 65, 330, 228, 312, 233, 437, 670, 213, 660, 302, 550, 896, 320, 704, 457, 608, 444, 735, 564, 776 ] ], [ [ \"parabolic\", \"A^{1}_2+A^{1}_1\", [ 120, 8, 69 ] ], [ 0, 1, 0, 10, 0, 16, 5, 0, 15, 0, 31, 14, 5, 30, 1, 60, 35, 5, 65, 10, 55, 15, 85, 6, 135, 90, 35, 85, 10, 155, 110, 45, 99, 130, 36, 144, 30, 175, 165, 29, 255, 216, 40, 230, 45, 255, 255, 105, 240, 340, 89, 315, 360, 81, 330, 360, 171, 300, 450, 210, 310, 184, 520, 215, 495, 360, 630, 0, 5, 1, 10, 0, 41, 1, 45, 34, 5, 90, 10, 86, 5, 141, 110, 34, 115, 21, 189, 165, 45, 186, 55, 200, 30, 265, 150, 230, 140, 325, 445, 105, 504, 170, 396, 576, 184, 520, 270, 415, 255, 504, 360, 540 ] ], [ [ \"parabolic\", \"A^{1}_3\", [ 120, 8, 7 ] ], [ 0, 1, 0, 10, 0, 15, 5, 0, 10, 0, 26, 4, 0, 15, 0, 50, 10, 0, 50, 5, 45, 5, 60, 1, 110, 45, 10, 40, 0, 110, 60, 10, 50, 80, 6, 84, 5, 105, 90, 4, 180, 96, 10, 150, 15, 180, 105, 50, 160, 140, 20, 180, 160, 26, 220, 180, 55, 140, 225, 80, 160, 64, 320, 70, 270, 150, 345, 0, 5, 1, 10, 0, 36, 0, 40, 24, 0, 60, 0, 56, 0, 110, 40, 10, 75, 6, 144, 60, 20, 136, 10, 105, 5, 190, 80, 140, 60, 200, 220, 25, 320, 60, 220, 256, 64, 320, 105, 200, 80, 259, 160, 300 ] ], [ [ \"parabolic\", \"4A^{1}_1\", [ 120, 97, 61, 32 ] ], [ 0, 1, 0, 6, 0, 13, 1, 1, 13, 0, 26, 14, 7, 26, 1, 45, 34, 2, 52, 3, 33, 9, 58, 3, 96, 63, 36, 66, 10, 124, 74, 36, 88, 76, 30, 120, 31, 137, 108, 20, 196, 168, 34, 170, 30, 177, 207, 60, 158, 278, 76, 252, 280, 54, 240, 258, 150, 243, 321, 152, 248, 128, 384, 172, 384, 267, 456, 0, 4, 0, 4, 0, 32, 0, 32, 16, 8, 72, 8, 72, 4, 108, 96, 20, 88, 12, 132, 144, 24, 120, 56, 164, 20, 200, 96, 144, 96, 224, 320, 84, 384, 136, 296, 448, 128, 384, 224, 316, 204, 408, 248, 392 ] ], [ [ \"parabolic\", \"A^{1}_2+2A^{1}_1\", [ 120, 8, 69, 31 ] ], [ 0, 1, 0, 6, 0, 12, 1, 0, 11, 0, 23, 8, 3, 20, 0, 39, 21, 1, 41, 2, 27, 5, 47, 1, 82, 42, 18, 48, 3, 100, 48, 21, 59, 52, 15, 87, 13, 106, 73, 9, 155, 108, 15, 133, 14, 140, 133, 37, 116, 176, 39, 185, 180, 28, 181, 168, 86, 167, 210, 96, 164, 72, 280, 96, 272, 159, 321, 0, 4, 0, 4, 0, 29, 0, 28, 12, 2, 59, 4, 56, 1, 91, 60, 12, 64, 5, 107, 91, 13, 94, 27, 121, 9, 159, 60, 104, 55, 164, 209, 40, 292, 75, 213, 288, 72, 280, 131, 217, 119, 281, 156, 268 ] ], [ [ \"parabolic\", \"2A^{1}_2\", [ 120, 8, 69, 2 ] ], [ 0, 1, 0, 6, 0, 11, 1, 0, 10, 0, 20, 6, 1, 16, 0, 34, 14, 1, 31, 1, 21, 2, 37, 0, 69, 27, 8, 38, 1, 81, 32, 15, 39, 34, 9, 63, 4, 84, 48, 4, 120, 68, 5, 105, 5, 110, 85, 22, 82, 114, 20, 136, 116, 15, 135, 108, 48, 117, 135, 64, 104, 40, 200, 53, 193, 90, 225, 0, 4, 0, 4, 0, 26, 0, 24, 8, 0, 50, 2, 42, 0, 76, 40, 8, 44, 2, 86, 58, 6, 72, 12, 92, 4, 124, 34, 74, 30, 120, 134, 18, 222, 38, 154, 184, 40, 200, 74, 154, 72, 192, 98, 178 ] ], [ [ \"parabolic\", \"A^{1}_3+A^{1}_1\", [ 120, 8, 7, 44 ] ], [ 0, 1, 0, 6, 0, 11, 1, 0, 8, 0, 19, 3, 0, 11, 0, 32, 7, 0, 30, 1, 21, 1, 32, 0, 65, 21, 5, 25, 0, 71, 25, 6, 30, 30, 3, 51, 2, 66, 37, 1, 107, 48, 3, 87, 4, 97, 56, 16, 73, 75, 9, 107, 80, 8, 117, 81, 28, 81, 102, 38, 82, 24, 168, 31, 149, 63, 174, 0, 4, 0, 4, 0, 25, 0, 24, 8, 0, 41, 0, 36, 0, 70, 24, 4, 40, 1, 79, 35, 5, 66, 5, 67, 1, 111, 29, 61, 21, 98, 101, 9, 185, 25, 119, 128, 24, 168, 50, 108, 39, 145, 67, 143 ] ], [ [ \"parabolic\", \"A^{1}_4\", [ 120, 8, 7, 6 ] ], [ 0, 1, 0, 6, 0, 10, 1, 0, 5, 0, 15, 0, 0, 5, 0, 25, 1, 0, 20, 0, 15, 0, 20, 0, 49, 6, 0, 10, 0, 45, 10, 0, 10, 14, 0, 24, 0, 35, 15, 0, 66, 16, 0, 50, 0, 60, 15, 5, 40, 20, 0, 50, 24, 1, 66, 30, 5, 30, 36, 10, 30, 4, 84, 5, 65, 15, 75, 0, 4, 0, 4, 0, 21, 0, 20, 4, 0, 25, 0, 20, 0, 50, 4, 0, 20, 0, 54, 9, 1, 42, 0, 30, 0, 70, 10, 30, 5, 50, 35, 0, 100, 5, 55, 40, 4, 84, 10, 40, 6, 56, 20, 60 ] ], [ [ \"parabolic\", \"D^{1}_4\", [ 120, 8, 7, 61 ] ], [ 0, 1, 0, 6, 0, 10, 1, 0, 3, 0, 14, 1, 0, 1, 0, 23, 0, 0, 20, 0, 15, 1, 17, 0, 45, 6, 2, 2, 0, 36, 7, 0, 10, 10, 0, 18, 0, 19, 12, 0, 56, 4, 0, 34, 0, 48, 6, 2, 34, 9, 0, 32, 12, 0, 54, 15, 0, 9, 21, 0, 24, 0, 64, 0, 38, 9, 45, 0, 4, 0, 4, 0, 20, 0, 20, 4, 0, 16, 0, 16, 0, 44, 4, 0, 20, 0, 48, 0, 0, 36, 0, 12, 0, 60, 8, 20, 4, 36, 20, 0, 72, 0, 28, 16, 0, 64, 8, 16, 0, 36, 8, 44 ] ], [ [ \"parabolic\", \"A^{1}_2+3A^{1}_1\", [ 120, 8, 69, 31, 19 ] ], [ 0, 1, 0, 3, 0, 9, 0, 0, 8, 0, 17, 5, 2, 13, 0, 25, 13, 0, 27, 0, 12, 1, 24, 0, 48, 18, 10, 26, 1, 65, 21, 9, 37, 19, 6, 54, 6, 63, 30, 2, 94, 54, 6, 75, 4, 74, 69, 11, 54, 92, 17, 109, 90, 9, 99, 78, 45, 93, 96, 41, 89, 26, 150, 43, 150, 69, 159, 0, 3, 0, 1, 0, 20, 0, 17, 3, 1, 38, 1, 37, 0, 58, 33, 3, 36, 1, 59, 51, 3, 45, 14, 73, 2, 95, 23, 43, 21, 80, 96, 15, 168, 34, 113, 144, 26, 150, 66, 111, 54, 159, 64, 132 ] ], [ [ \"parabolic\", \"2A^{1}_2+A^{1}_1\", [ 120, 8, 69, 2, 9 ] ], [ 0, 1, 0, 3, 0, 8, 0, 0, 7, 0, 15, 3, 1, 11, 0, 21, 9, 0, 20, 0, 9, 1, 20, 0, 40, 12, 5, 20, 0, 52, 12, 6, 25, 11, 3, 39, 2, 50, 20, 1, 73, 34, 2, 59, 1, 57, 46, 6, 37, 59, 9, 81, 58, 4, 72, 48, 26, 66, 60, 27, 57, 14, 106, 23, 106, 39, 111, 0, 3, 0, 1, 0, 18, 0, 14, 2, 0, 32, 1, 29, 0, 48, 22, 2, 25, 0, 46, 34, 1, 33, 7, 56, 1, 74, 12, 30, 11, 57, 60, 6, 127, 17, 81, 92, 14, 106, 37, 78, 33, 110, 39, 87 ] ], [ [ \"parabolic\", \"A^{1}_3+2A^{1}_1\", [ 120, 8, 7, 44, 18 ] ], [ 0, 1, 0, 3, 0, 8, 0, 0, 6, 0, 14, 2, 0, 8, 0, 20, 5, 0, 19, 0, 9, 0, 16, 0, 37, 9, 3, 14, 0, 46, 10, 3, 19, 10, 1, 32, 1, 40, 14, 0, 64, 24, 1, 49, 1, 50, 30, 4, 32, 40, 4, 64, 40, 2, 62, 36, 15, 47, 45, 16, 44, 8, 88, 14, 82, 26, 85, 0, 3, 0, 1, 0, 17, 0, 14, 2, 0, 27, 0, 24, 0, 44, 14, 1, 22, 0, 42, 21, 1, 30, 3, 42, 0, 65, 10, 24, 7, 46, 45, 3, 106, 11, 63, 64, 8, 88, 25, 56, 18, 83, 26, 68 ] ], [ [ \"parabolic\", \"A^{1}_3+A^{1}_2\", [ 120, 8, 7, 44, 3 ] ], [ 0, 1, 0, 3, 0, 7, 0, 0, 6, 0, 12, 2, 0, 7, 0, 17, 4, 0, 13, 0, 6, 0, 13, 0, 30, 6, 1, 13, 0, 37, 6, 3, 13, 5, 1, 23, 0, 33, 9, 0, 48, 14, 0, 39, 0, 38, 20, 2, 20, 27, 2, 48, 26, 1, 44, 21, 8, 34, 27, 12, 26, 4, 60, 7, 58, 13, 59, 0, 3, 0, 1, 0, 15, 0, 11, 1, 0, 24, 0, 18, 0, 36, 11, 1, 14, 0, 32, 14, 0, 21, 1, 34, 0, 49, 4, 16, 3, 33, 27, 1, 80, 4, 45, 40, 4, 60, 13, 42, 12, 57, 16, 42 ] ], [ [ \"parabolic\", \"A^{1}_4+A^{1}_1\", [ 120, 8, 7, 6, 23 ] ], [ 0, 1, 0, 3, 0, 7, 0, 0, 4, 0, 11, 0, 0, 4, 0, 15, 1, 0, 12, 0, 6, 0, 10, 0, 27, 3, 0, 6, 0, 29, 3, 0, 7, 4, 0, 15, 0, 22, 5, 0, 39, 8, 0, 28, 0, 30, 9, 1, 16, 11, 0, 31, 12, 0, 33, 12, 3, 18, 15, 4, 16, 1, 43, 2, 36, 6, 36, 0, 3, 0, 1, 0, 14, 0, 11, 1, 0, 17, 0, 14, 0, 31, 3, 0, 11, 0, 27, 6, 0, 18, 0, 20, 0, 40, 3, 11, 1, 22, 15, 0, 57, 2, 29, 20, 1, 43, 5, 21, 3, 33, 7, 27 ] ], [ [ \"parabolic\", \"D^{1}_4+A^{1}_1\", [ 120, 8, 7, 61, 32 ] ], [ 0, 1, 0, 3, 0, 7, 0, 0, 3, 0, 10, 1, 0, 1, 0, 14, 0, 0, 12, 0, 6, 0, 7, 0, 24, 3, 1, 2, 0, 24, 3, 0, 7, 3, 0, 12, 0, 13, 3, 0, 32, 2, 0, 19, 0, 24, 3, 0, 13, 6, 0, 20, 6, 0, 27, 6, 0, 6, 9, 0, 12, 0, 32, 0, 22, 3, 21, 0, 3, 0, 1, 0, 13, 0, 11, 1, 0, 12, 0, 10, 0, 27, 3, 0, 11, 0, 24, 0, 0, 15, 0, 9, 0, 33, 2, 7, 1, 15, 9, 0, 42, 0, 15, 8, 0, 32, 4, 10, 0, 21, 2, 19 ] ], [ [ \"parabolic\", \"A^{1}_5\", [ 120, 8, 7, 6, 5 ] ], [ 0, 1, 0, 3, 0, 6, 0, 0, 3, 0, 8, 0, 0, 2, 0, 11, 0, 0, 6, 0, 3, 0, 5, 0, 18, 0, 0, 3, 0, 17, 1, 0, 1, 1, 0, 6, 0, 12, 1, 0, 20, 2, 0, 15, 0, 16, 1, 0, 6, 2, 0, 12, 2, 0, 15, 3, 0, 6, 3, 1, 3, 0, 16, 0, 13, 0, 12, 0, 3, 0, 1, 0, 11, 0, 8, 0, 0, 11, 0, 6, 0, 20, 0, 0, 3, 0, 16, 1, 0, 9, 0, 9, 0, 21, 0, 4, 0, 9, 3, 0, 27, 0, 12, 4, 0, 16, 0, 7, 0, 9, 1, 7 ] ], [ [ \"parabolic\", \"D^{1}_5\", [ 120, 8, 7, 6, 48 ] ], [ 0, 1, 0, 3, 0, 6, 0, 0, 1, 0, 7, 0, 0, 0, 0, 9, 0, 0, 6, 0, 3, 0, 4, 0, 15, 0, 0, 0, 0, 10, 0, 0, 1, 0, 0, 3, 0, 3, 1, 0, 14, 0, 0, 6, 0, 9, 0, 0, 4, 0, 0, 4, 0, 0, 9, 0, 0, 0, 0, 0, 2, 0, 8, 0, 3, 0, 3, 0, 3, 0, 1, 0, 10, 0, 8, 0, 0, 4, 0, 4, 0, 15, 0, 0, 3, 0, 12, 0, 0, 6, 0, 1, 0, 15, 0, 1, 0, 4, 0, 0, 12, 0, 2, 0, 0, 8, 0, 0, 0, 3, 0, 3 ] ], [ [ \"parabolic\", \"2A^{1}_2+2A^{1}_1\", [ 120, 8, 69, 2, 9, 13 ] ], [ 0, 1, 0, 1, 0, 6, 0, 0, 5, 0, 11, 2, 1, 7, 0, 13, 6, 0, 14, 0, 3, 0, 9, 0, 22, 4, 3, 10, 0, 34, 5, 2, 17, 3, 1, 25, 1, 29, 7, 0, 44, 17, 1, 32, 0, 28, 24, 1, 16, 31, 4, 48, 29, 1, 39, 22, 15, 37, 26, 10, 32, 4, 56, 10, 59, 16, 52, 0, 2, 0, 0, 0, 12, 0, 8, 0, 0, 20, 0, 20, 0, 30, 12, 0, 14, 0, 24, 20, 0, 14, 4, 34, 0, 44, 4, 10, 4, 26, 26, 2, 72, 8, 42, 46, 4, 56, 20, 38, 14, 64, 14, 42 ] ], [ [ \"parabolic\", \"A^{1}_3+A^{1}_2+A^{1}_1\", [ 120, 8, 7, 44, 3, 2 ] ], [ 0, 1, 0, 1, 0, 5, 0, 0, 4, 0, 9, 1, 0, 5, 0, 10, 3, 0, 9, 0, 2, 0, 6, 0, 16, 2, 1, 6, 0, 24, 2, 1, 9, 1, 0, 15, 0, 19, 3, 0, 29, 7, 0, 21, 0, 18, 11, 0, 8, 14, 1, 29, 13, 0, 23, 9, 5, 20, 11, 4, 15, 1, 31, 3, 32, 5, 27, 0, 2, 0, 0, 0, 10, 0, 6, 0, 0, 15, 0, 13, 0, 22, 6, 0, 8, 0, 16, 9, 0, 8, 1, 21, 0, 29, 1, 5, 1, 14, 11, 0, 45, 2, 23, 20, 1, 31, 7, 20, 5, 34, 5, 20 ] ], [ [ \"parabolic\", \"2A^{1}_3\", [ 120, 8, 7, 44, 3, 4 ] ], [ 0, 1, 0, 1, 0, 4, 0, 0, 4, 0, 7, 1, 0, 4, 0, 8, 2, 0, 5, 0, 1, 0, 4, 0, 11, 1, 0, 5, 0, 17, 1, 1, 5, 0, 0, 9, 0, 13, 1, 0, 18, 2, 0, 14, 0, 11, 5, 0, 3, 7, 0, 18, 6, 0, 13, 3, 1, 11, 4, 2, 6, 0, 16, 1, 17, 1, 14, 0, 2, 0, 0, 0, 8, 0, 4, 0, 0, 12, 0, 8, 0, 16, 4, 0, 4, 0, 10, 4, 0, 4, 0, 14, 0, 18, 0, 2, 0, 8, 4, 0, 28, 0, 12, 8, 0, 16, 2, 12, 2, 18, 2, 8 ] ], [ [ \"parabolic\", \"A^{1}_4+2A^{1}_1\", [ 120, 8, 7, 6, 23, 11 ] ], [ 0, 1, 0, 1, 0, 5, 0, 0, 3, 0, 8, 0, 0, 3, 0, 9, 1, 0, 8, 0, 2, 0, 4, 0, 14, 1, 0, 3, 0, 19, 1, 0, 5, 1, 0, 10, 0, 13, 1, 0, 23, 4, 0, 15, 0, 14, 5, 0, 6, 6, 0, 19, 6, 0, 17, 5, 2, 11, 6, 1, 9, 0, 22, 1, 20, 2, 16, 0, 2, 0, 0, 0, 9, 0, 6, 0, 0, 11, 0, 10, 0, 19, 2, 0, 6, 0, 13, 4, 0, 7, 0, 13, 0, 23, 1, 3, 0, 9, 6, 0, 32, 1, 15, 10, 0, 22, 3, 10, 1, 20, 2, 12 ] ], [ [ \"parabolic\", \"A^{1}_4+A^{1}_2\", [ 120, 8, 7, 6, 23, 2 ] ], [ 0, 1, 0, 1, 0, 4, 0, 0, 3, 0, 7, 0, 0, 3, 0, 7, 1, 0, 5, 0, 1, 0, 4, 0, 11, 1, 0, 3, 0, 15, 0, 0, 4, 0, 0, 7, 0, 11, 1, 0, 17, 2, 0, 12, 0, 10, 4, 0, 3, 4, 0, 15, 4, 0, 11, 2, 1, 8, 3, 1, 5, 0, 14, 0, 14, 1, 11, 0, 2, 0, 0, 0, 8, 0, 4, 0, 0, 10, 0, 8, 0, 15, 2, 0, 4, 0, 9, 3, 0, 4, 0, 11, 0, 17, 0, 2, 0, 6, 3, 0, 24, 0, 10, 6, 0, 14, 1, 8, 1, 14, 1, 7 ] ], [ [ \"parabolic\", \"D^{1}_4+A^{1}_2\", [ 120, 8, 7, 61, 32, 4 ] ], [ 0, 1, 0, 1, 0, 4, 0, 0, 3, 0, 6, 1, 0, 1, 0, 7, 0, 0, 5, 0, 1, 0, 2, 0, 9, 1, 0, 2, 0, 13, 1, 0, 4, 0, 0, 6, 0, 7, 0, 0, 13, 0, 0, 8, 0, 8, 1, 0, 2, 3, 0, 10, 2, 0, 9, 1, 0, 3, 2, 0, 3, 0, 10, 0, 9, 0, 6, 0, 2, 0, 0, 0, 7, 0, 4, 0, 0, 8, 0, 5, 0, 13, 2, 0, 4, 0, 8, 0, 0, 3, 0, 6, 0, 13, 0, 1, 0, 4, 2, 0, 18, 0, 5, 2, 0, 10, 1, 5, 0, 9, 0, 4 ] ], [ [ \"parabolic\", \"A^{1}_5+A^{1}_1\", [ 120, 8, 7, 6, 5, 9 ] ], [ 0, 1, 0, 1, 0, 4, 0, 0, 2, 0, 6, 0, 0, 2, 0, 6, 0, 0, 4, 0, 1, 0, 2, 0, 9, 0, 0, 1, 0, 11, 0, 0, 1, 0, 0, 4, 0, 7, 0, 0, 12, 1, 0, 8, 0, 7, 1, 0, 2, 1, 0, 8, 1, 0, 7, 1, 0, 4, 1, 0, 2, 0, 8, 0, 7, 0, 5, 0, 2, 0, 0, 0, 7, 0, 4, 0, 0, 7, 0, 5, 0, 12, 0, 0, 2, 0, 7, 1, 0, 3, 0, 6, 0, 12, 0, 1, 0, 3, 1, 0, 15, 0, 6, 2, 0, 8, 0, 3, 0, 6, 0, 3 ] ], [ [ \"parabolic\", \"D^{1}_5+A^{1}_1\", [ 120, 8, 7, 6, 48, 17 ] ], [ 0, 1, 0, 1, 0, 4, 0, 0, 1, 0, 5, 0, 0, 0, 0, 5, 0, 0, 4, 0, 1, 0, 1, 0, 7, 0, 0, 0, 0, 7, 0, 0, 1, 0, 0, 2, 0, 2, 0, 0, 8, 0, 0, 3, 0, 4, 0, 0, 1, 0, 0, 3, 0, 0, 4, 0, 0, 0, 0, 0, 1, 0, 4, 0, 2, 0, 1, 0, 2, 0, 0, 0, 6, 0, 4, 0, 0, 3, 0, 3, 0, 9, 0, 0, 2, 0, 5, 0, 0, 2, 0, 1, 0, 8, 0, 0, 0, 1, 0, 0, 7, 0, 1, 0, 0, 4, 0, 0, 0, 2, 0, 1 ] ], [ [ \"parabolic\", \"A^{1}_6\", [ 120, 8, 7, 6, 5, 4 ] ], [ 0, 1, 0, 1, 0, 3, 0, 0, 2, 0, 4, 0, 0, 1, 0, 4, 0, 0, 1, 0, 0, 0, 1, 0, 5, 0, 0, 1, 0, 6, 0, 0, 0, 0, 0, 1, 0, 4, 0, 0, 5, 0, 0, 4, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 2, 0, 2, 0, 1, 0, 2, 0, 0, 0, 5, 0, 2, 0, 0, 5, 0, 2, 0, 7, 0, 0, 0, 0, 3, 0, 0, 1, 0, 3, 0, 5, 0, 0, 0, 1, 0, 0, 6, 0, 2, 0, 0, 2, 0, 1, 0, 1, 0, 0 ] ], [ [ \"parabolic\", \"D^{1}_6\", [ 120, 8, 7, 6, 5, 32 ] ], [ 0, 1, 0, 1, 0, 3, 0, 0, 1, 0, 3, 0, 0, 0, 0, 3, 0, 0, 1, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 2, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 4, 0, 2, 0, 0, 2, 0, 0, 0, 4, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"parabolic\", \"E^{1}_6\", [ 120, 48, 8, 7, 6, 24 ] ], [ 0, 1, 0, 1, 0, 3, 0, 0, 0, 0, 3, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 1, 0, 3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 4, 0, 2, 0, 0, 0, 0, 1, 0, 3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"parabolic\", \"A^{1}_4+A^{1}_2+A^{1}_1\", [ 120, 8, 7, 6, 23, 2, 3 ] ] , [ 0, 1, 0, 0, 0, 3, 0, 0, 2, 0, 5, 0, 0, 2, 0, 4, 1, 0, 4, 0, 0, 0, 1, 0, 5, 0, 0, 1, 0, 10, 0, 0, 3, 0, 0, 5, 0, 6, 0, 0, 10, 1, 0, 6, 0, 4, 2, 0, 1, 2, 0, 9, 2, 0, 6, 1, 1, 5, 1, 0, 3, 0, 7, 0, 8, 0, 4, 0, 1, 0, 0, 0, 5, 0, 2, 0, 0, 6, 0, 6, 0, 9, 1, 0, 2, 0, 4, 2, 0, 1, 0, 7, 0, 10, 0, 0, 0, 2, 1, 0, 13, 0, 5, 3, 0, 7, 1, 3, 0, 9, 0, 3 ] ], [ [ \"parabolic\", \"A^{1}_4+A^{1}_3\", [ 120, 8, 7, 6, 23, 2, 4 ] ], [ 0, 1, 0, 0, 0, 2, 0, 0, 2, 0, 4, 0, 0, 2, 0, 3, 1, 0, 2, 0, 0, 0, 1, 0, 3, 0, 0, 1, 0, 7, 0, 0, 2, 0, 0, 3, 0, 4, 0, 0, 6, 0, 0, 4, 0, 2, 1, 0, 0, 1, 0, 6, 1, 0, 3, 0, 0, 3, 0, 0, 1, 0, 3, 0, 4, 0, 2, 0, 1, 0, 0, 0, 4, 0, 1, 0, 0, 5, 0, 4, 0, 6, 1, 0, 1, 0, 2, 1, 0, 0, 0, 5, 0, 6, 0, 0, 0, 1, 0, 0, 8, 0, 2, 1, 0, 3, 0, 2, 0, 5, 0, 1 ] ], [ [ \"parabolic\", \"D^{1}_5+A^{1}_2\", [ 120, 8, 7, 6, 48, 17, 3 ] ], [ 0, 1, 0, 0, 0, 2, 0, 0, 1, 0, 3, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 4, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 3, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 3, 0, 1, 0, 0, 2, 0, 2, 0, 4, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 3, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 ] ], [ [ \"parabolic\", \"A^{1}_6+A^{1}_1\", [ 120, 8, 7, 6, 5, 4, 1 ] ], [ 0, 1, 0, 0, 0, 2, 0, 0, 1, 0, 3, 0, 0, 1, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 3, 0, 0, 2, 0, 1, 0, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 3, 0, 1, 0, 0, 3, 0, 2, 0, 4, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 0, 3, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0 ] ], [ [ \"parabolic\", \"E^{1}_6+A^{1}_1\", [ 120, 48, 8, 7, 6, 24, 11 ] ], [ 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 1, 0, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"parabolic\", \"A^{1}_7\", [ 120, 8, 7, 6, 5, 4, 3 ] ], [ 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 2, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"parabolic\", \"D^{1}_7\", [ 120, 8, 7, 6, 5, 4, 17 ] ], [ 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"parabolic\", \"E^{1}_7\", [ 120, 48, 8, 7, 6, 24, 2 ] ], [ 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"parabolic\", \"E^{1}_8\", [ 120, 48, 8, 7, 6, 24, 2, 4 ] ], [ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"4A^{1}_1\", [ 120, 97, 61, 7 ] ], [ 0, 1, 0, 6, 0, 13, 1, 2, 14, 1, 27, 16, 6, 25, 0, 44, 32, 2, 52, 3, 33, 13, 62, 3, 96, 63, 42, 72, 9, 123, 72, 36, 88, 76, 36, 126, 35, 141, 116, 24, 200, 176, 30, 166, 27, 174, 201, 63, 161, 284, 72, 248, 272, 51, 237, 252, 147, 240, 315, 160, 256, 128, 384, 170, 382, 270, 459, 0, 4, 0, 4, 0, 32, 0, 32, 16, 8, 72, 8, 72, 4, 108, 96, 20, 88, 12, 132, 144, 24, 120, 56, 164, 20, 200, 96, 144, 96, 224, 320, 84, 384, 136, 296, 448, 128, 384, 224, 316, 204, 408, 248, 392 ] ], [ [ \"pseudoParabolicNonParabolic\", \"5A^{1}_1\", [ 120, 97, 61, 32, 2 ] ], [ 0, 1, 0, 3, 0, 10, 0, 1, 10, 0, 20, 10, 3, 16, 0, 29, 19, 0, 35, 0, 15, 3, 31, 0, 57, 27, 23, 38, 3, 81, 34, 15, 55, 30, 15, 78, 16, 83, 49, 6, 122, 88, 12, 94, 9, 93, 102, 21, 77, 148, 30, 146, 136, 18, 132, 120, 75, 132, 147, 68, 140, 48, 208, 77, 211, 120, 228, 0, 3, 0, 1, 0, 22, 0, 20, 4, 4, 46, 2, 46, 1, 69, 52, 5, 50, 3, 75, 78, 6, 60, 28, 97, 5, 120, 38, 62, 38, 112, 150, 33, 222, 62, 158, 224, 48, 208, 112, 163, 93, 228, 104, 196 ] ], [ [ \"pseudoParabolicNonParabolic\", \"A^{1}_3+2A^{1}_1\", [ 120, 8, 7, 44, 1 ] ], [ 0, 1, 0, 3, 0, 8, 0, 0, 7, 0, 15, 3, 0, 7, 0, 19, 4, 0, 19, 0, 9, 1, 19, 0, 37, 9, 5, 18, 0, 45, 9, 3, 19, 10, 3, 36, 2, 43, 18, 1, 67, 28, 0, 46, 0, 48, 27, 5, 34, 43, 3, 61, 36, 1, 60, 33, 14, 45, 42, 20, 48, 8, 88, 13, 81, 27, 87, 0, 3, 0, 1, 0, 17, 0, 14, 2, 0, 27, 0, 24, 0, 44, 14, 1, 22, 0, 42, 21, 1, 30, 3, 42, 0, 65, 10, 24, 7, 46, 45, 3, 106, 11, 63, 64, 8, 88, 25, 56, 18, 83, 26, 68 ] ], [ [ \"pseudoParabolicNonParabolic\", \"A^{1}_2+4A^{1}_1\", [ 120, 8, 69, 31, 19, 4 ] ], [ 0, 1, 0, 1, 0, 7, 0, 0, 6, 0, 13, 4, 1, 8, 0, 16, 8, 0, 19, 0, 4, 0, 11, 0, 27, 6, 7, 14, 0, 43, 10, 3, 25, 6, 3, 36, 3, 37, 12, 0, 58, 28, 2, 40, 1, 36, 34, 3, 24, 50, 6, 64, 44, 3, 54, 36, 24, 51, 42, 16, 52, 8, 80, 19, 83, 30, 75, 0, 2, 0, 0, 0, 13, 0, 10, 0, 1, 24, 0, 24, 0, 36, 18, 0, 20, 0, 32, 29, 0, 21, 8, 44, 0, 56, 8, 16, 7, 38, 43, 6, 96, 16, 60, 72, 8, 80, 34, 56, 24, 90, 24, 64 ] ], [ [ \"pseudoParabolicNonParabolic\", \"3A^{1}_2\", [ 120, 8, 69, 2, 9, 3 ] ] , [ 0, 1, 0, 1, 0, 5, 0, 0, 4, 0, 10, 1, 1, 7, 0, 10, 4, 0, 10, 0, 2, 1, 9, 0, 18, 3, 2, 7, 0, 27, 2, 1, 12, 1, 0, 18, 0, 23, 5, 0, 34, 10, 0, 25, 0, 21, 18, 0, 10, 19, 2, 36, 18, 0, 27, 12, 9, 27, 15, 7, 21, 2, 38, 5, 42, 9, 36, 0, 2, 0, 0, 0, 11, 0, 6, 0, 0, 17, 1, 17, 0, 24, 8, 0, 10, 0, 18, 15, 0, 9, 2, 26, 0, 34, 1, 7, 2, 17, 15, 0, 54, 4, 30, 28, 2, 38, 11, 27, 9, 45, 8, 28 ] ], [ [ \"pseudoParabolicNonParabolic\", \"A^{1}_3+3A^{1}_1\", [ 120, 8, 7, 44, 1, 18 ] ], [ 0, 1, 0, 1, 0, 6, 0, 0, 5, 0, 11, 2, 0, 5, 0, 12, 3, 0, 13, 0, 3, 0, 8, 0, 20, 3, 3, 9, 0, 30, 4, 1, 13, 3, 1, 23, 1, 25, 6, 0, 40, 14, 0, 25, 0, 23, 14, 1, 14, 23, 1, 37, 18, 0, 32, 15, 8, 26, 18, 7, 27, 2, 46, 6, 45, 11, 40, 0, 2, 0, 0, 0, 11, 0, 8, 0, 0, 17, 0, 16, 0, 27, 8, 0, 12, 0, 22, 13, 0, 13, 2, 26, 0, 38, 3, 8, 2, 21, 19, 1, 60, 5, 33, 32, 2, 46, 13, 28, 8, 48, 9, 32 ] ], [ [ \"pseudoParabolicNonParabolic\", \"2A^{1}_3\", [ 120, 8, 7, 44, 3, 1 ] ] , [ 0, 1, 0, 1, 0, 4, 0, 0, 5, 0, 8, 2, 0, 3, 0, 7, 1, 0, 5, 0, 1, 0, 6, 0, 11, 1, 1, 8, 0, 16, 0, 1, 5, 0, 1, 12, 0, 15, 3, 0, 20, 4, 0, 12, 0, 10, 4, 0, 4, 8, 0, 16, 4, 0, 12, 2, 1, 10, 3, 4, 8, 0, 16, 0, 16, 1, 15, 0, 2, 0, 0, 0, 8, 0, 4, 0, 0, 12, 0, 8, 0, 16, 4, 0, 4, 0, 10, 4, 0, 4, 0, 14, 0, 18, 0, 2, 0, 8, 4, 0, 28, 0, 12, 8, 0, 16, 2, 12, 2, 18, 2, 8 ] ], [ [ \"pseudoParabolicNonParabolic\", \"D^{1}_4+2A^{1}_1\", [ 120, 8, 7, 61, 32, 2 ] ], [ 0, 1, 0, 1, 0, 5, 0, 0, 3, 0, 8, 1, 0, 1, 0, 8, 0, 0, 8, 0, 2, 0, 3, 0, 12, 1, 1, 2, 0, 16, 1, 0, 5, 1, 0, 10, 0, 9, 1, 0, 20, 2, 0, 9, 0, 10, 1, 0, 5, 4, 0, 12, 2, 0, 13, 2, 0, 4, 3, 0, 8, 0, 16, 0, 12, 1, 9, 0, 2, 0, 0, 0, 8, 0, 6, 0, 0, 8, 0, 6, 0, 16, 2, 0, 6, 0, 12, 0, 0, 6, 0, 6, 0, 18, 0, 2, 0, 6, 4, 0, 24, 0, 8, 4, 0, 16, 2, 6, 0, 12, 0, 8 ] ], [ [ \"pseudoParabolicNonParabolic\", \"A^{1}_5+A^{1}_1\", [ 120, 8, 7, 6, 5, 2 ] ], [ 0, 1, 0, 1, 0, 4, 0, 0, 3, 0, 7, 0, 0, 1, 0, 5, 0, 0, 4, 0, 1, 0, 4, 0, 9, 0, 0, 3, 0, 10, 0, 0, 1, 0, 0, 6, 0, 9, 1, 0, 14, 2, 0, 6, 0, 6, 0, 0, 3, 2, 0, 6, 0, 0, 6, 0, 0, 3, 0, 1, 3, 0, 8, 0, 7, 0, 6, 0, 2, 0, 0, 0, 7, 0, 4, 0, 0, 7, 0, 5, 0, 12, 0, 0, 2, 0, 7, 1, 0, 3, 0, 6, 0, 12, 0, 1, 0, 3, 1, 0, 15, 0, 6, 2, 0, 8, 0, 3, 0, 6, 0, 3 ] ], [ [ \"pseudoParabolicNonParabolic\", \"3A^{1}_2+A^{1}_1\", [ 120, 8, 69, 2, 9, 3, 13 ] ], [ 0, 1, 0, 0, 0, 4, 0, 0, 3, 0, 7, 1, 1, 4, 0, 6, 3, 0, 8, 0, 0, 0, 3, 0, 9, 0, 1, 3, 0, 18, 1, 0, 9, 0, 0, 12, 0, 13, 1, 0, 20, 5, 0, 13, 0, 9, 9, 0, 4, 10, 1, 21, 9, 0, 15, 6, 6, 15, 6, 2, 12, 0, 20, 2, 24, 3, 15, 0, 1, 0, 0, 0, 7, 0, 3, 0, 0, 10, 0, 12, 0, 15, 4, 0, 5, 0, 9, 9, 0, 3, 1, 16, 0, 20, 0, 1, 1, 7, 6, 0, 30, 2, 15, 14, 0, 20, 7, 12, 3, 27, 2, 13 ] ], [ [ \"pseudoParabolicNonParabolic\", \"A^{1}_3+A^{1}_2+2A^{1}_1\", [ 120, 8, 7, 44, 3, 2, 5 ] ], [ 0, 1, 0, 0, 0, 4, 0, 0, 3, 0, 7, 1, 0, 3, 0, 6, 2, 0, 7, 0, 0, 0, 2, 0, 8, 0, 1, 3, 0, 16, 1, 0, 7, 0, 0, 11, 0, 11, 1, 0, 18, 4, 0, 10, 0, 7, 5, 0, 3, 8, 0, 17, 6, 0, 12, 4, 3, 11, 4, 1, 10, 0, 16, 1, 18, 2, 11, 0, 1, 0, 0, 0, 6, 0, 3, 0, 0, 9, 0, 9, 0, 13, 3, 0, 4, 0, 8, 6, 0, 3, 1, 13, 0, 17, 0, 1, 0, 6, 4, 0, 25, 1, 12, 10, 0, 16, 4, 9, 2, 20, 1, 9 ] ], [ [ \"pseudoParabolicNonParabolic\", \"2A^{1}_3+A^{1}_1\", [ 120, 8, 7, 44, 3, 1, 2 ] ], [ 0, 1, 0, 0, 0, 3, 0, 0, 3, 0, 6, 1, 0, 2, 0, 4, 1, 0, 4, 0, 0, 0, 2, 0, 5, 0, 1, 3, 0, 11, 0, 0, 4, 0, 0, 8, 0, 8, 1, 0, 12, 2, 0, 6, 0, 4, 2, 0, 1, 4, 0, 10, 2, 0, 6, 1, 1, 6, 1, 1, 5, 0, 8, 0, 9, 0, 6, 0, 1, 0, 0, 0, 5, 0, 2, 0, 0, 7, 0, 6, 0, 9, 2, 0, 2, 0, 5, 3, 0, 1, 0, 9, 0, 11, 0, 0, 0, 3, 1, 0, 15, 0, 6, 4, 0, 8, 1, 5, 1, 11, 0, 4 ] ], [ [ \"pseudoParabolicNonParabolic\", \"D^{1}_4+A^{1}_3\", [ 120, 8, 7, 61, 32, 4, 5 ] ], [ 0, 1, 0, 0, 0, 2, 0, 0, 3, 0, 4, 1, 0, 1, 0, 3, 0, 0, 2, 0, 0, 0, 1, 0, 2, 0, 0, 2, 0, 6, 0, 0, 2, 0, 0, 4, 0, 3, 0, 0, 5, 0, 0, 2, 0, 1, 0, 0, 0, 1, 0, 4, 0, 0, 2, 0, 0, 1, 0, 0, 1, 0, 2, 0, 2, 0, 1, 0, 1, 0, 0, 0, 3, 0, 1, 0, 0, 4, 0, 2, 0, 5, 1, 0, 1, 0, 2, 0, 0, 0, 0, 3, 0, 4, 0, 0, 0, 1, 0, 0, 6, 0, 1, 0, 0, 2, 0, 2, 0, 3, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"A^{1}_5+2A^{1}_1\", [ 120, 8, 7, 6, 5, 9, 2 ] ], [ 0, 1, 0, 0, 0, 3, 0, 0, 2, 0, 5, 0, 0, 1, 0, 3, 0, 0, 3, 0, 0, 0, 1, 0, 4, 0, 0, 1, 0, 7, 0, 0, 1, 0, 0, 4, 0, 5, 0, 0, 8, 1, 0, 3, 0, 2, 0, 0, 1, 1, 0, 4, 0, 0, 3, 0, 0, 2, 0, 0, 2, 0, 4, 0, 4, 0, 2, 0, 1, 0, 0, 0, 4, 0, 2, 0, 0, 4, 0, 4, 0, 7, 0, 0, 1, 0, 3, 1, 0, 1, 0, 4, 0, 7, 0, 0, 0, 1, 0, 0, 8, 0, 3, 1, 0, 4, 0, 1, 0, 4, 0, 1 ] ], [ [ \"pseudoParabolicNonParabolic\", \"A^{1}_5+A^{1}_2\", [ 120, 8, 7, 6, 5, 9, 3 ] ], [ 0, 1, 0, 0, 0, 2, 0, 0, 1, 0, 4, 0, 0, 2, 0, 2, 0, 0, 2, 0, 0, 0, 1, 0, 3, 0, 0, 0, 0, 6, 0, 0, 1, 0, 0, 2, 0, 3, 0, 0, 5, 0, 0, 3, 0, 2, 1, 0, 0, 0, 0, 4, 0, 0, 2, 0, 0, 2, 0, 0, 1, 0, 2, 0, 3, 0, 1, 0, 1, 0, 0, 0, 4, 0, 1, 0, 0, 4, 0, 4, 0, 5, 0, 0, 1, 0, 2, 1, 0, 0, 0, 3, 0, 5, 0, 0, 0, 0, 0, 0, 6, 0, 2, 0, 0, 2, 0, 1, 0, 3, 0, 1 ] ], [ [ \"pseudoParabolicNonParabolic\", \"D^{1}_5+2A^{1}_1\", [ 120, 8, 7, 6, 48, 17, 4 ] ], [ 0, 1, 0, 0, 0, 3, 0, 0, 1, 0, 4, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 5, 0, 0, 1, 0, 0, 2, 0, 1, 0, 0, 5, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 0, 0, 0, 1, 0, 0, 0, 3, 0, 2, 0, 0, 2, 0, 2, 0, 5, 0, 0, 1, 0, 2, 0, 0, 1, 0, 1, 0, 4, 0, 0, 0, 0, 0, 0, 4, 0, 1, 0, 0, 2, 0, 0, 0, 1, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"D^{1}_6+A^{1}_1\", [ 120, 8, 7, 6, 5, 32, 2 ] ], [ 0, 1, 0, 0, 0, 2, 0, 0, 1, 0, 3, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 1, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"A^{1}_7\", [ 120, 8, 7, 6, 5, 4, 2 ] ], [ 0, 1, 0, 0, 0, 1, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"2A^{1}_4\", [ 120, 8, 7, 6, 23, 2, 4, 3 ] ], [ 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 2, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"A^{1}_5+A^{1}_2+A^{1}_1\", [ 120, 8, 7, 6, 5, 9, 3, 2 ] ], [ 0, 1, 0, 0, 0, 2, 0, 0, 1, 0, 3, 0, 0, 1, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 0, 0, 1, 0, 0, 2, 0, 2, 0, 0, 3, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 3, 0, 3, 0, 0, 0, 0, 1, 1, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 0, 3, 0, 1, 0, 0, 1, 0, 0, 0, 2, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"D^{1}_5+A^{1}_3\", [ 120, 8, 7, 6, 48, 17, 3, 4 ] ], [ 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 2, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"E^{1}_6+A^{1}_2\", [ 120, 48, 8, 7, 6, 24, 11, 4 ] ], [ 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"A^{1}_7+A^{1}_1\", [ 120, 8, 7, 6, 5, 4, 2, 1 ] ], [ 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"E^{1}_7+A^{1}_1\", [ 120, 48, 8, 7, 6, 24, 2, 3 ] ], [ 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"A^{1}_8\", [ 120, 8, 7, 6, 5, 4, 3, 1 ] ], [ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"pseudoParabolicNonParabolic\", \"D^{1}_8\", [ 120, 8, 7, 6, 5, 4, 3, 2 ] ], [ 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"nonPseudoParabolic\", \"6A^{1}_1\", [ 120, 97, 61, 32, 2, 3 ] ], [ 0, 1, 0, 1, 0, 8, 0, 1, 8, 0, 16, 8, 1, 10, 0, 19, 11, 0, 25, 0, 5, 1, 15, 0, 33, 9, 17, 22, 1, 55, 16, 5, 37, 10, 9, 54, 10, 51, 21, 2, 78, 48, 4, 50, 3, 45, 48, 7, 35, 82, 10, 86, 64, 6, 72, 54, 39, 72, 63, 28, 84, 16, 112, 35, 117, 54, 108, 0, 2, 0, 0, 0, 14, 0, 12, 0, 2, 28, 0, 28, 0, 42, 28, 0, 28, 0, 42, 42, 0, 30, 14, 56, 0, 70, 14, 26, 14, 56, 70, 12, 126, 28, 84, 112, 16, 112, 56, 84, 42, 126, 42, 98 ] ], [ [ \"nonPseudoParabolic\", \"7A^{1}_1\", [ 120, 97, 61, 32, 2, 3, 5 ] ], [ 0, 1, 0, 0, 0, 7, 0, 1, 7, 0, 14, 7, 0, 7, 0, 14, 7, 0, 20, 0, 0, 0, 7, 0, 21, 0, 14, 14, 0, 42, 7, 0, 28, 0, 6, 42, 7, 35, 7, 0, 56, 28, 0, 28, 0, 21, 21, 0, 14, 49, 0, 56, 28, 0, 42, 21, 21, 42, 21, 8, 56, 0, 64, 14, 70, 21, 48, 0, 1, 0, 0, 0, 7, 0, 6, 0, 1, 14, 0, 14, 0, 21, 14, 0, 14, 0, 21, 21, 0, 15, 7, 28, 0, 35, 7, 13, 7, 28, 35, 6, 63, 14, 42, 56, 8, 56, 28, 42, 21, 63, 21, 49 ] ], [ [ \"nonPseudoParabolic\", \"A^{1}_3+4A^{1}_1\", [ 120, 8, 7, 44, 1, 18, 4 ] ], [ 0, 1, 0, 0, 0, 5, 0, 0, 4, 0, 9, 2, 0, 3, 0, 8, 2, 0, 10, 0, 0, 0, 3, 0, 11, 0, 3, 5, 0, 21, 2, 0, 10, 0, 1, 17, 1, 15, 2, 0, 26, 8, 0, 12, 0, 9, 6, 0, 5, 14, 0, 22, 8, 0, 17, 6, 5, 14, 6, 2, 18, 0, 24, 3, 25, 5, 16, 0, 1, 0, 0, 0, 6, 0, 4, 0, 0, 10, 0, 10, 0, 15, 4, 0, 6, 0, 11, 8, 0, 6, 1, 16, 0, 21, 1, 3, 0, 10, 8, 0, 33, 2, 18, 16, 0, 24, 6, 14, 3, 27, 3, 15 ] ], [ [ \"nonPseudoParabolic\", \"D^{1}_4+3A^{1}_1\", [ 120, 8, 7, 61, 32, 2, 3 ] ], [ 0, 1, 0, 0, 0, 4, 0, 0, 3, 0, 7, 1, 0, 1, 0, 5, 0, 0, 6, 0, 0, 0, 1, 0, 6, 0, 1, 2, 0, 12, 0, 0, 4, 0, 0, 9, 0, 7, 0, 0, 14, 2, 0, 4, 0, 3, 0, 0, 1, 3, 0, 8, 0, 0, 6, 0, 0, 3, 0, 0, 6, 0, 8, 0, 7, 0, 3, 0, 1, 0, 0, 0, 4, 0, 3, 0, 0, 4, 0, 3, 0, 8, 1, 0, 3, 0, 6, 0, 0, 3, 0, 3, 0, 9, 0, 1, 0, 3, 2, 0, 12, 0, 4, 2, 0, 8, 1, 3, 0, 6, 0, 4 ] ], [ [ \"nonPseudoParabolic\", \"8A^{1}_1\", [ 120, 97, 61, 32, 2, 3, 5, 7 ] ], [ 0, 1, 0, 0, 0, 7, 0, 1, 7, 0, 14, 7, 0, 7, 0, 14, 7, 0, 20, 0, 0, 0, 7, 0, 21, 0, 14, 14, 0, 42, 7, 0, 28, 0, 6, 42, 7, 35, 7, 0, 56, 28, 0, 28, 0, 21, 21, 0, 14, 49, 0, 56, 28, 0, 42, 21, 21, 42, 21, 8, 56, 0, 64, 14, 70, 21, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"nonPseudoParabolic\", \"4A^{1}_2\", [ 120, 8, 69, 2, 9, 3, 13, 6 ] ], [ 0, 1, 0, 0, 0, 3, 0, 0, 2, 0, 4, 1, 1, 2, 0, 2, 2, 0, 6, 0, 0, 0, 1, 0, 3, 0, 0, 0, 0, 10, 1, 0, 6, 0, 0, 6, 0, 6, 0, 0, 8, 2, 0, 6, 0, 1, 4, 0, 0, 3, 0, 8, 2, 0, 6, 3, 3, 6, 0, 0, 4, 0, 8, 0, 10, 0, 3, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 4, 0, 8, 0, 8, 0, 0, 0, 0, 4, 4, 0, 0, 0, 8, 0, 8, 0, 0, 0, 0, 0, 0, 12, 0, 4, 4, 0, 8, 4, 4, 0, 12, 0, 4 ] ], [ [ \"nonPseudoParabolic\", \"2A^{1}_3+2A^{1}_1\", [ 120, 8, 7, 44, 3, 1, 2, 5 ] ], [ 0, 1, 0, 0, 0, 3, 0, 0, 2, 0, 5, 1, 0, 1, 0, 3, 1, 0, 4, 0, 0, 0, 0, 0, 3, 0, 1, 1, 0, 8, 0, 0, 4, 0, 0, 6, 0, 4, 0, 0, 8, 1, 0, 2, 0, 1, 1, 0, 0, 3, 0, 6, 1, 0, 3, 0, 1, 3, 0, 0, 4, 0, 4, 0, 5, 0, 1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 4, 0, 4, 0, 4, 0, 0, 0, 0, 2, 2, 0, 0, 0, 6, 0, 6, 0, 0, 0, 2, 0, 0, 8, 0, 4, 2, 0, 4, 0, 2, 0, 6, 0, 2 ] ], [ [ \"nonPseudoParabolic\", \"D^{1}_4+4A^{1}_1\", [ 120, 8, 7, 61, 32, 2, 3, 5 ] ], [ 0, 1, 0, 0, 0, 4, 0, 0, 3, 0, 7, 1, 0, 1, 0, 5, 0, 0, 6, 0, 0, 0, 1, 0, 6, 0, 1, 2, 0, 12, 0, 0, 4, 0, 0, 9, 0, 7, 0, 0, 14, 2, 0, 4, 0, 3, 0, 0, 1, 3, 0, 8, 0, 0, 6, 0, 0, 3, 0, 0, 6, 0, 8, 0, 7, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"nonPseudoParabolic\", \"2D^{1}_4\", [ 120, 8, 7, 61, 32, 4, 5, 3 ] ], [ 0, 1, 0, 0, 0, 1, 0, 0, 3, 0, 3, 1, 0, 1, 0, 2, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 3, 0, 0, 1, 0, 0, 3, 0, 1, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ], [ [ \"nonPseudoParabolic\", \"D^{1}_6+2A^{1}_1\", [ 120, 8, 7, 6, 5, 32, 2, 3 ] ], [ 0, 1, 0, 0, 0, 2, 0, 0, 1, 0, 3, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ] ] ");
  return LoadOutputSubgroupsFromJSData(theJSreader, inputGroup, outputSubgroups);
}

bool LoadConjugacyClassesG1_2(WeylGroupData& output) {
  output.computeRho(true);
  FiniteGroup<ElementWeylGroup>::ConjugacyClass emptyClass;
  emptyClass.flagRepresentativeComputed = true;
  output.theGroup.conjugacyClasses.initializeFillInObject(6, emptyClass);
  output.theGroup.conjugacyClasses[0  ].representative.makeFromReadableReflections(output, false, "");
  output.theGroup.conjugacyClasses[1  ].representative.makeFromReadableReflections(output, false, "1");
  output.theGroup.conjugacyClasses[2  ].representative.makeFromReadableReflections(output, false, "2");
  output.theGroup.conjugacyClasses[3  ].representative.makeFromReadableReflections(output, false, "2,1");
  output.theGroup.conjugacyClasses[4  ].representative.makeFromReadableReflections(output, false, "2,1,2,1");
  output.theGroup.conjugacyClasses[5  ].representative.makeFromReadableReflections(output, false, "1,2,1,2,1,2");
  output.theGroup.conjugacyClasses[0  ].size = 1;
  output.theGroup.conjugacyClasses[1  ].size = 3;
  output.theGroup.conjugacyClasses[2  ].size = 3;
  output.theGroup.conjugacyClasses[3  ].size = 2;
  output.theGroup.conjugacyClasses[4  ].size = 2;
  output.theGroup.conjugacyClasses[5  ].size = 1;
  output.ccCarterLabels.setSize(0);
  output.ccCarterLabels.addOnTop("0");
  output.ccCarterLabels.addOnTop("A^{3}_1");
  output.ccCarterLabels.addOnTop("A_1");
  output.ccCarterLabels.addOnTop("G_2");
  output.ccCarterLabels.addOnTop("A_2");
  output.ccCarterLabels.addOnTop("A^{3}_1+A_1");
  output.loadConjugacyClassesHelper();
  return true;
}

bool LoadCharTableG1_2(WeylGroupData& output) {
  FiniteGroup<ElementWeylGroup>& outputG = output.theGroup;
  outputG.characterTable.setExpectedSize(12); outputG.characterTable.setSize(0);
  ClassFunction<FiniteGroup<ElementWeylGroup>, Rational> currentCF;
  currentCF.G = &outputG;
  currentCF.data.assignString("(1, - 1, - 1,   1,   1,   1)"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(1, - 1,   1, - 1,   1, - 1)"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(1,   1, - 1, - 1,   1, - 1)"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(1,   1,   1,   1,   1,   1)"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(2,   0,   0,   1, - 1, - 2)"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(2,   0,   0, - 1, - 1,   2)"); outputG.characterTable.addOnTop(currentCF);
  output.irrepsCarterLabels.setSize(0);
  output.irrepsCarterLabels.addOnTop("\\phi_{1,6}  ");
  output.irrepsCarterLabels.addOnTop("\\phi_{1,3}''");
  output.irrepsCarterLabels.addOnTop("\\phi_{1,3}' ");
  output.irrepsCarterLabels.addOnTop("\\phi_{1,0}  ");
  output.irrepsCarterLabels.addOnTop("\\phi_{2,1}  ");
  output.irrepsCarterLabels.addOnTop("\\phi_{2,2}  ");
  return true;
}

bool LoadConjugacyClassesE1_6(WeylGroupData& output) {
  output.computeRho(true);
  FiniteGroup<ElementWeylGroup>::ConjugacyClass emptyClass;
  emptyClass.flagRepresentativeComputed = true;
  output.theGroup.conjugacyClasses.initializeFillInObject(25, emptyClass);
  output.theGroup.conjugacyClasses[0  ].representative.makeFromReadableReflections(output, false, "   ");
  output.theGroup.conjugacyClasses[1  ].representative.makeFromReadableReflections(output, false, "3, 4, 3, 2, 4, 3, 5, 4, 3, 2, 4, 5 ");
  output.theGroup.conjugacyClasses[2  ].representative.makeFromReadableReflections(output, false, "1, 4 ");
  output.theGroup.conjugacyClasses[3  ].representative.makeFromReadableReflections(output, false, "1, 3, 1, 4, 3, 1, 2, 4, 5, 4, 3, 1, 2, 4, 3, 5, 6, 5, 4, 3, 2, 4, 5, 6");
  output.theGroup.conjugacyClasses[4  ].representative.makeFromReadableReflections(output, false, "1, 3 ");
  output.theGroup.conjugacyClasses[5  ].representative.makeFromReadableReflections(output, false, "1, 3, 5, 6  ");
  output.theGroup.conjugacyClasses[6  ].representative.makeFromReadableReflections(output, false, "3, 4, 3, 2, 4, 5  ");
  output.theGroup.conjugacyClasses[7  ].representative.makeFromReadableReflections(output, false, "1, 4, 3, 6 ");
  output.theGroup.conjugacyClasses[8  ].representative.makeFromReadableReflections(output, false, "1, 4, 3, 2  ");
  output.theGroup.conjugacyClasses[9  ].representative.makeFromReadableReflections(output, false, "1, 2, 3, 1, 5, 4, 6, 5, 4, 2, 3, 4 ");
  output.theGroup.conjugacyClasses[10 ].representative.makeFromReadableReflections(output, false, "3, 4, 2, 5  ");
  output.theGroup.conjugacyClasses[11 ].representative.makeFromReadableReflections(output, false, "1, 2, 3, 4, 2, 3, 4, 6, 5, 4, 2, 3, 4, 5 ");
  output.theGroup.conjugacyClasses[12 ].representative.makeFromReadableReflections(output, false, "1, 3, 2, 5 ");
  output.theGroup.conjugacyClasses[13 ].representative.makeFromReadableReflections(output, false, "1, 3, 4, 3, 2, 4, 5, 6  ");
  output.theGroup.conjugacyClasses[14 ].representative.makeFromReadableReflections(output, false, "1, 4, 6, 2, 3, 5 ");
  output.theGroup.conjugacyClasses[15 ].representative.makeFromReadableReflections(output, false, "1 ");
  output.theGroup.conjugacyClasses[16 ].representative.makeFromReadableReflections(output, false, "1, 4, 6  ");
  output.theGroup.conjugacyClasses[17 ].representative.makeFromReadableReflections(output, false, "1, 3, 4, 3, 2, 4, 3, 5, 4, 3, 2, 4, 5 ");
  output.theGroup.conjugacyClasses[18 ].representative.makeFromReadableReflections(output, false, "1, 4, 3  ");
  output.theGroup.conjugacyClasses[19 ].representative.makeFromReadableReflections(output, false, "1, 3, 2  ");
  output.theGroup.conjugacyClasses[20 ].representative.makeFromReadableReflections(output, false, "1, 3, 2, 5, 6  ");
  output.theGroup.conjugacyClasses[21 ].representative.makeFromReadableReflections(output, false, "1, 4, 6, 3, 5  ");
  output.theGroup.conjugacyClasses[22 ].representative.makeFromReadableReflections(output, false, "1, 3, 4, 2, 5  ");
  output.theGroup.conjugacyClasses[23 ].representative.makeFromReadableReflections(output, false, "1, 4, 3, 2, 6  ");
  output.theGroup.conjugacyClasses[24 ].representative.makeFromReadableReflections(output, false, "1, 4, 2, 5, 4, 2, 3 ");

  output.ccCarterLabels.setSize(0);
  output.ccCarterLabels.addOnTop("0"        );
  output.ccCarterLabels.addOnTop("4A_1"     );
  output.ccCarterLabels.addOnTop("2A_1"     );
  output.ccCarterLabels.addOnTop("3A_2"     );
  output.ccCarterLabels.addOnTop("A_2"      );
  output.ccCarterLabels.addOnTop("2A_2"     );
  output.ccCarterLabels.addOnTop("D_4(a_1)" );
  output.ccCarterLabels.addOnTop("A_3+A_1"  );
  output.ccCarterLabels.addOnTop("A_4"      );
  output.ccCarterLabels.addOnTop("E_6(a_2)" );
  output.ccCarterLabels.addOnTop("D_4"      );
  output.ccCarterLabels.addOnTop("A_5+A_1"  );
  output.ccCarterLabels.addOnTop("A_2+2A_1" );
  output.ccCarterLabels.addOnTop("E_6(a_1)" );
  output.ccCarterLabels.addOnTop("E_6"      );
  output.ccCarterLabels.addOnTop("A_1"      );
  output.ccCarterLabels.addOnTop("3A_1"     );
  output.ccCarterLabels.addOnTop("A_3+2A_1" );
  output.ccCarterLabels.addOnTop("A_3"      );
  output.ccCarterLabels.addOnTop("A_2+A_1"  );
  output.ccCarterLabels.addOnTop("2A_2+A_1" );
  output.ccCarterLabels.addOnTop("A_5"      );
  output.ccCarterLabels.addOnTop("D_5"      );
  output.ccCarterLabels.addOnTop("A_4+A_1"  );
  output.ccCarterLabels.addOnTop("D_5(a_1)" );

  output.theGroup.conjugacyClasses[0  ].size = 1   ;
  output.theGroup.conjugacyClasses[1  ].size = 45  ;
  output.theGroup.conjugacyClasses[2  ].size = 270 ;
  output.theGroup.conjugacyClasses[3  ].size = 80  ;
  output.theGroup.conjugacyClasses[4  ].size = 240 ;
  output.theGroup.conjugacyClasses[5  ].size = 480 ;
  output.theGroup.conjugacyClasses[6  ].size = 540 ;
  output.theGroup.conjugacyClasses[7  ].size = 3240;
  output.theGroup.conjugacyClasses[8  ].size = 5184;
  output.theGroup.conjugacyClasses[9  ].size = 720 ;
  output.theGroup.conjugacyClasses[10 ].size = 1440;
  output.theGroup.conjugacyClasses[11 ].size = 1440;
  output.theGroup.conjugacyClasses[12 ].size = 2160;
  output.theGroup.conjugacyClasses[13 ].size = 5760;
  output.theGroup.conjugacyClasses[14 ].size = 4320;
  output.theGroup.conjugacyClasses[15 ].size = 36  ;
  output.theGroup.conjugacyClasses[16 ].size = 540 ;
  output.theGroup.conjugacyClasses[17 ].size = 540 ;
  output.theGroup.conjugacyClasses[18 ].size = 1620;
  output.theGroup.conjugacyClasses[19 ].size = 1440;
  output.theGroup.conjugacyClasses[20 ].size = 1440;
  output.theGroup.conjugacyClasses[21 ].size = 4320;
  output.theGroup.conjugacyClasses[22 ].size = 6480;
  output.theGroup.conjugacyClasses[23 ].size = 5184;
  output.theGroup.conjugacyClasses[24 ].size = 4320;
  output.loadConjugacyClassesHelper();
  return true;
}

bool LoadCharTableE1_6(WeylGroupData& output) {
  FiniteGroup<ElementWeylGroup>& outputG = output.theGroup;
  outputG.characterTable.setExpectedSize(48); outputG.characterTable.setSize(0);
  ClassFunction<FiniteGroup<ElementWeylGroup>, Rational> currentCF;
  currentCF.G = &outputG;
  currentCF.data.assignString("(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1              )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1    )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(10, -6, 2, 1, -2, 4, 2, -2, 0, -3, 0, 0, 2, 1, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0        )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(6, -2, 2, -3, 3, 0, 2, 0, 1, 1, 1, -2, - 1, 0, - 1, 4, 0, -2, 2, 1, -2, 0,0, - 1, 1       )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(6, -2, 2, -3, 3, 0, 2, 0, 1, 1, 1, -2, - 1, 0, - 1, -4, 0, 2, -2, - 1, 2, 0,0, 1, - 1      )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(20, 4, -4, -7, 2, 2, 4, 0, 0, 1, -2, -2, 2, - 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0        )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(15, - 1, - 1, 6, 3, 0, 3, - 1, 0, 2, - 1, 2, - 1, 0, 0, 5, -3, 1, 1, - 1, 2, 0,- 1, 0, 1      )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(15, - 1, - 1, 6, 3, 0, 3, - 1, 0, 2, - 1, 2, - 1, 0, 0, -5, 3, - 1, - 1, 1, -2, 0, 1, 0, - 1   )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(15, 7, 3, -3, 0, 3, - 1, 1, 0, 1, -2, 1, 0, 0, - 1, 5, 1, 3, - 1, 2, - 1, 1, - 1, 0, 0      )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(15, 7, 3, -3, 0, 3, - 1, 1, 0, 1, -2, 1, 0, 0, - 1, -5, - 1, -3, 1, -2, 1, - 1, 1, 0, 0    )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(20, 4, 4, 2, 5, - 1, 0, 0, 0, -2, 1, 1, 1, - 1, 0, 10, 2, 2, 2, 1, 1, - 1, 0, 0, - 1       )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(20, 4, 4, 2, 5, - 1, 0, 0, 0, -2, 1, 1, 1, - 1, 0, - 10, -2, -2, -2, - 1, - 1,1, 0, 0, 1    )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(24, 8, 0, 6, 0, 3, 0, 0, - 1, 2, 2, - 1, 0, 0, 0, 4, 4, 0, 0, -2, 1, 1, 0, - 1, 0         )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(24, 8, 0, 6, 0, 3, 0, 0, - 1, 2, 2, - 1, 0, 0, 0, -4, -4, 0, 0, 2, - 1, - 1, 0, 1, 0       )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(30, - 10, 2, 3, 3, 3, -2, 0, 0, - 1, - 1, - 1, - 1, 0, 1, 10, -2, -4, 0, 1, 1,1, 0, 0, - 1   )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(30, - 10, 2, 3, 3, 3, -2, 0, 0, - 1, - 1, - 1, - 1, 0, 1, - 10, 2, 4, 0, - 1, - 1, - 1, 0, 0, 1 )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(60, 12, 4, -3, -6, 0, 4, 0, 0, -3, 0, 0, -2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0        )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(80, - 16, 0, - 10, -4, 2, 0, 0, 0, 2, 2, 2, 0, - 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0       )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(90, -6, -6, 9, 0, 0, 2, 2, 0, -3, 0, 0, 0, 0, - 1, 0, 0, 0, 0, 0, 0, 0, 0,0, 0          )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(60, -4, 4, 6, -3, -3, 0, 0, 0, 2, - 1, - 1, 1, 0, 0, 10, 2, -2, -2, 1, 1, - 1, 0, 0, 1    )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(60, -4, 4, 6, -3, -3, 0, 0, 0, 2, - 1, - 1, 1, 0, 0, - 10, -2, 2, 2, - 1, - 1, 1, 0, 0, - 1  )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(64, 0, 0, -8, 4, -2, 0, 0, - 1, 0, 0, 0, 0, 1, 0, 16, 0, 0, 0, -2, -2, 0, 0, 1, 0       )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(64, 0, 0, -8, 4, -2, 0, 0, - 1, 0, 0, 0, 0, 1, 0, - 16, 0, 0, 0, 2, 2, 0, 0, - 1, 0       )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(81, 9, -3, 0, 0, 0, -3, - 1, 1, 0, 0, 0, 0, 0, 0, 9, -3, 3, - 1, 0, 0, 0, 1, - 1, 0       )"); outputG.characterTable.addOnTop(currentCF);
  currentCF.data.assignString("(81, 9, -3, 0, 0, 0, -3, - 1, 1, 0, 0, 0, 0, 0, 0, -9, 3, -3, 1, 0, 0, 0, - 1, 1, 0       )"); outputG.characterTable.addOnTop(currentCF);
  output.irrepsCarterLabels.setSize(0);
  output.irrepsCarterLabels.addOnTop("\\phi_{1,0}  " );
  output.irrepsCarterLabels.addOnTop("\\phi_{1,36} " );
  output.irrepsCarterLabels.addOnTop("\\phi_{10,9} " );
  output.irrepsCarterLabels.addOnTop("\\phi_{6,1}  " );
  output.irrepsCarterLabels.addOnTop("\\phi_{6,25} " );
  output.irrepsCarterLabels.addOnTop("\\phi_{20,10}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{15,5} " );
  output.irrepsCarterLabels.addOnTop("\\phi_{15,17}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{15,4} " );
  output.irrepsCarterLabels.addOnTop("\\phi_{15,16}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{20,2} " );
  output.irrepsCarterLabels.addOnTop("\\phi_{20,20}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{24,6} " );
  output.irrepsCarterLabels.addOnTop("\\phi_{24,12}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{30,3} " );
  output.irrepsCarterLabels.addOnTop("\\phi_{30,15}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{60,8} " );
  output.irrepsCarterLabels.addOnTop("\\phi_{80,7} " );
  output.irrepsCarterLabels.addOnTop("\\phi_{90,8} " );
  output.irrepsCarterLabels.addOnTop("\\phi_{60,5} " );
  output.irrepsCarterLabels.addOnTop("\\phi_{60,11}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{64,4} " );
  output.irrepsCarterLabels.addOnTop("\\phi_{64,13}" );
  output.irrepsCarterLabels.addOnTop("\\phi_{81,6} " );
  output.irrepsCarterLabels.addOnTop("\\phi_{81,10}" );
  return true;
}

bool WeylGroupData::loadConjugacyClassesHelper() {
  this->theGroup.sizePrivate = 0;
  for (int i = 0; i < this->theGroup.conjugacyClasses.size; i ++) {
    this->theGroup.sizePrivate += this->theGroup.conjugacyClasses[i].size;
  }
  if (this->theGroup.sizePrivate != this->theGroup.GetSizeByFormula(this->theGroup)) {
    global.fatal << "Corrupt hard-coded data: the size of the "
    << "group does not work out according to formula. " << global.fatal;
  }
  this->theGroup.flagCCRepresentativesComputed = true;
  this->theGroup.flagCCsComputed = false;
  this->theGroup.initializeGenerators();
  return true;
}

bool WeylGroupData::loadSignSignatures(List<SubgroupDataRootReflections>& outputSubgroups) {
  if (!this->loadConjugacyClasses()) {
    return false;
  }
  if (!this->loadCharacterTable()) {
    return false;
  }
  if (this->theDynkinType.toString() == "F^{1}_4") {
    return LoadSignSignatureF1_4(*this, outputSubgroups);
  }
  if (this->theDynkinType.toString() == "E^{1}_6") {
    return LoadSignSignatureE1_6(*this, outputSubgroups);
  }
  if (this->theDynkinType.toString() == "E^{1}_7") {
    return LoadSignSignatureE1_7(*this, outputSubgroups);
  }
  if (this->theDynkinType.toString() == "E^{1}_8") {
    return LoadSignSignatureE1_8(*this, outputSubgroups);
  }
  return false;
}

bool WeylGroupData::loadConjugacyClasses() {
  if (this->theDynkinType.toString() == "G^{1}_2") {
    return LoadConjugacyClassesG1_2(*this);
  }
  if (this->theDynkinType.toString() == "F^{1}_4") {
    return LoadConjugacyClassesF1_4(*this);
  }
  if (this->theDynkinType.toString() == "E^{1}_6") {
    return LoadConjugacyClassesE1_6(*this);
  }
  if (this->theDynkinType.toString() == "E^{1}_7") {
    return LoadConjugacyClassesE1_7(*this);
  }
  if (this->theDynkinType.toString() == "E^{1}_8") {
    return LoadConjugacyClassesE1_8(*this);
  }
  return false;
}

bool WeylGroupData::loadCharacterTable() {
  bool result = false;
  if (this->theDynkinType.toString() == "G^{1}_2") {
    result = LoadCharTableG1_2(*this);
  }
  if (this->theDynkinType.toString() == "F^{1}_4") {
    result = LoadCharTableF1_4(*this);
  }
  if (this->theDynkinType.toString() == "E^{1}_6") {
    result = LoadCharTableE1_6(*this);
  }
  if (this->theDynkinType.toString() == "E^{1}_7") {
    result = LoadCharTableE1_7(*this);
  }
  if (this->theDynkinType.toString() == "E^{1}_8") {
    result = LoadCharTableE1_8(*this);
  }
  if (result) {
    this->theGroup.checkOrthogonalityCharacterTable();
  }
  return result;
}

bool LoadGAPRootSystemF1_4(HashedList<Vector<Rational> >& outputRootSystem) {
  outputRootSystem.clear();
  Vector<Rational> theRoot;
  theRoot.assignString("[ 1, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 2, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 2, 2 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 2 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 3, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 2 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 3, 2 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 4, 2 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 3, 4, 2 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 2, 3, 4, 2 ]"); outputRootSystem.addOnTop(theRoot);
  return true;
}

bool LoadGAPRootSystemE1_6(HashedList<Vector<Rational> >& outputRootSystem) {
  outputRootSystem.clear();
  Vector<Rational> theRoot;
  theRoot.assignString("[ 1, 0, 0, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 0, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 1, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 0, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 0, 0, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 0, 1, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 0, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 1, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 0, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 0, 1, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 0, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 1, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 0, 1, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 0, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 1, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 0, 1, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 2, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 2, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 2, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 2, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 2, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 2, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 2, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 2, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 2, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  return true;
}

bool LoadGAPRootSystemE1_7(HashedList<Vector<Rational> >& outputRootSystem) {
  outputRootSystem.clear();
  Vector<Rational> theRoot;
  theRoot.assignString("[ 1, 0, 0, 0, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 0, 0, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 1, 0, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 1, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 0, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 0, 0, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 0, 0, 0, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 0, 1, 0, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 0, 1, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 1, 1, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 1, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 0, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 0, 0, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 0, 1, 1, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 1, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 0, 1, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 1, 1, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 1, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 0, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 1, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 0, 1, 1, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 1, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 0, 1, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 1, 1, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 1, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 1, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 0, 1, 1, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 2, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 1, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 0, 1, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 1, 1, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 2, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 1, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 0, 1, 1, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 2, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 1, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 2, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 2, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 1, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 2, 2, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 2, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 2, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 2, 2, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 2, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 2, 2, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 2, 2, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 2, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 2, 2, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 2, 2, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 3, 2, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 2, 2, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 2, 2, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 3, 2, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 3, 2, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 2, 2, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 3, 2, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 3, 2, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 3, 2, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 3, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 3, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 4, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 3, 4, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 2, 2, 3, 4, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  return true;
}

bool LoadGAPRootSystemE1_8(HashedList<Vector<Rational> >& outputRootSystem) {
  outputRootSystem.clear();
  Vector<Rational> theRoot;
  theRoot.assignString("[ 1, 0, 0, 0, 0, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 0, 0, 0, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 1, 0, 0, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 1, 0, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 0, 1, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 0, 0, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 0, 0, 0, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 0, 0, 0, 0, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 0, 1, 0, 0, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 0, 1, 0, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 1, 1, 0, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 1, 1, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 0, 1, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 0, 0, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 0, 0, 0, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 0, 1, 1, 0, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 1, 0, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 0, 1, 1, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 1, 1, 1, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 1, 1, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 0, 1, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 0, 0, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 1, 0, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 0, 1, 1, 1, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 1, 1, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 0, 1, 1, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 1, 1, 1, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 1, 1, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 0, 1, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 1, 1, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 0, 1, 1, 1, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 2, 1, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 1, 1, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 0, 1, 1, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 1, 1, 1, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 0, 1, 1, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 2, 1, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 1, 1, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 0, 1, 1, 1, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 2, 1, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 1, 1, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 0, 1, 1, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 0, 1, 1, 1, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 2, 1, 0, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 2, 1, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 1, 1, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 0, 1, 1, 1, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 2, 2, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 2, 1, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 1, 1, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 2, 1, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 2, 2, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 2, 1, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 1, 1, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 2, 2, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 2, 1, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 2, 2, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 2, 1, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 2, 2, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 2, 1, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 2, 2, 2, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 2, 2, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 3, 2, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 2, 2, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 2, 1, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 2, 2, 2, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 2, 2, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 2, 2, 2, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 3, 2, 1, 0, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 3, 2, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 2, 2, 2, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 2, 2, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 2, 2, 2, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 0, 1, 1, 2, 2, 2, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 3, 2, 1, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 3, 2, 2, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 3, 2, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 2, 2, 2, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 1, 2, 2, 2, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 3, 2, 2, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 3, 2, 1, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 3, 3, 2, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 3, 2, 2, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 2, 2, 2, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 3, 3, 2, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 3, 2, 2, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 3, 3, 2, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 3, 2, 2, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 4, 3, 2, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 3, 3, 2, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 3, 2, 2, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 3, 3, 2, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 3, 4, 3, 2, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 4, 3, 2, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 3, 3, 2, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 1, 2, 3, 3, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 2, 2, 3, 4, 3, 2, 1, 0 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 3, 4, 3, 2, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 4, 3, 2, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 3, 3, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 2, 2, 3, 4, 3, 2, 1, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 3, 4, 3, 2, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 4, 3, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 2, 2, 3, 4, 3, 2, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 3, 4, 3, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 2, 4, 4, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 2, 2, 3, 4, 3, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 3, 4, 4, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 2, 2, 3, 4, 4, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 2, 3, 5, 4, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 2, 2, 3, 5, 4, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 1, 3, 3, 5, 4, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 2, 3, 3, 5, 4, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 2, 2, 4, 5, 4, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 2, 3, 4, 5, 4, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 2, 3, 4, 6, 4, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 2, 3, 4, 6, 5, 3, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 2, 3, 4, 6, 5, 4, 2, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 2, 3, 4, 6, 5, 4, 3, 1 ]"); outputRootSystem.addOnTop(theRoot);
  theRoot.assignString("[ 2, 3, 4, 6, 5, 4, 3, 2 ]"); outputRootSystem.addOnTop(theRoot);
  return true;
}

bool WeylGroupData::loadGAPRootSystem(HashedList<Vector<Rational> >& outputPositiveRootSystem) const {
  bool result = false;
  if (this->theDynkinType.isOfSimpleType('F', 4)) {
    result = LoadGAPRootSystemF1_4(outputPositiveRootSystem);
  }
  if (this->theDynkinType.isOfSimpleType('E', 8)) {
    result = LoadGAPRootSystemE1_8(outputPositiveRootSystem);
  }
  if (this->theDynkinType.isOfSimpleType('E', 7)) {
    result = LoadGAPRootSystemE1_7(outputPositiveRootSystem);
  }
  if (this->theDynkinType.isOfSimpleType('E', 6)) {
    result = LoadGAPRootSystemE1_6(outputPositiveRootSystem);
  }
  if (!result) {
    return result;
  }
  if (outputPositiveRootSystem.size != this->RootsOfBorel.size) {
    global.fatal << "Wrong number of GAP roots! " << global.fatal;
  }
  for (int i = 0; i < this->RootsOfBorel.size; i ++) {
    if (!outputPositiveRootSystem.contains(this->RootsOfBorel[i])) {
      global.fatal << " Positive root " << outputPositiveRootSystem[i].toString() << " is not a GAP root. " << global.fatal;
    }
  }
  return true;
}
