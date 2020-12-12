// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general_implementation.h"
#include "math_extra_semisimple_Lie_subalgebras.h"
#include "math_subsets_selections.h"
#include "math_general_polynomial_computations_advanced_implementation.h"
#include "math_extra_semisimple_Lie_algebras_implementation.h"
#include "math_extra_finite_groups_implementation.h"

template<>
List<DynkinSimpleType>::Comparator*
FormatExpressions::getMonomialOrder<DynkinSimpleType>() {
  return nullptr;
}

template<class Coefficient>
void SemisimpleLieAlgebra::generateLieSubalgebra(
  List<ElementSemisimpleLieAlgebra<Coefficient> >& inputOutputGenerators
) {
  ElementSemisimpleLieAlgebra<Coefficient> theBracket;
  HashedList<ChevalleyGenerator> seedMons;
  ProgressReport theReport;
  List<ElementSemisimpleLieAlgebra<Coefficient> > inputLinIndep;
  for (int i = 0; i < inputOutputGenerators.size; i ++) {
    inputLinIndep.addOnTop(inputOutputGenerators[i]);
    if (theBracket.getRankOfSpanOfElements(inputLinIndep, &seedMons) < inputLinIndep.size) {
      inputLinIndep.removeLastObject();
    }
  }
  inputOutputGenerators = inputLinIndep;
  for (int i = 0; i < inputOutputGenerators.size; i ++) {
    for (int j = i + 1; j < inputOutputGenerators.size; j ++) {
      if (theReport.tickAndWantReport()) {
        std::stringstream reportStream;
        reportStream << "Generating Lie subalgebra of a semisimple Lie algebra. "
        << "I am taking the Lie bracket of elements "
        << i + 1 << " and " << j + 1 << " out of " << inputOutputGenerators.size;
        theReport.report(reportStream.str());
      }
      this->lieBracket(inputOutputGenerators[i], inputOutputGenerators[j], theBracket);
      inputOutputGenerators.addOnTop(theBracket);
      if (theBracket.getRankOfSpanOfElements(inputOutputGenerators, &seedMons) < inputOutputGenerators.size) {
        inputOutputGenerators.removeLastObject();
      }
    }
  }
}

std::string SemisimpleLieAlgebra::toStringLieAlgebraNameFullHTML() const {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::toStringLieAlgebraNameFullHTML");
  std::stringstream out;
  if (this->theWeyl.theDynkinType.hasExceptionalComponent()) {
    out << "<div class='mathcalculator'>" << this->theWeyl.theDynkinType.toString() << "</div>";
    return out.str();
  }
  out << this->toStringLieAlgebraNameNonTechnicalHTML()
  << ", type \\(" << this->theWeyl.theDynkinType.toString() << "\\)";
  return out.str();
}

std::string SemisimpleLieAlgebra::toStringLieAlgebraName() const {
  return this->theWeyl.theDynkinType.toString();
}

std::string SemisimpleLieAlgebra::toStringLieAlgebraNameNonTechnicalHTML() const {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::toStringLieAlgebraNameNonTechnicalHTML");
  std::stringstream out;
  const DynkinType& theType = this->theWeyl.theDynkinType;
  for (int indexType = 0; indexType < theType.size(); indexType ++) {
    if (!(theType.coefficients[indexType] > 0)) {
      global.fatal << "Simple constituents must appear with positive coefficient. " << global.fatal;
    }
    const DynkinSimpleType& currentSimpleType = theType[indexType];
    for (int indexIsotypic = 0; indexIsotypic < theType.coefficients[indexType]; indexIsotypic ++) {
      out << currentSimpleType.ToStringNonTechnicalName(nullptr);
      if (indexIsotypic + 1 < theType.coefficients[indexType]) {
        out << "\\oplus";
      }
    }
    if (indexType != theType.size() - 1) {
      out << "\\oplus";
    }
  }
  return out.str();
}

bool SemisimpleLieAlgebra::isSimpleGenerator(int generatorIndex) {
  bool isNegativeGenerator = (generatorIndex < this->getNumberOfPositiveRoots()) &&
  (generatorIndex >= this->getNumberOfPositiveRoots() - this->getRank());
  bool isPositiveGenerator = (generatorIndex >= this->getNumberOfPositiveRoots() + this->getRank()) &&
  (generatorIndex < this->getNumberOfPositiveRoots() + this->getRank() * 2);
  return isNegativeGenerator || isPositiveGenerator;
}


bool SemisimpleLieAlgebra::checkConsistency() const {
  if (this->flagDeallocated) {
    global.fatal << "Use after free of SemisimpleLieAlgebra. " << global.fatal;
  }
  this->theWeyl.checkConsistency();
  return true;
}

bool SemisimpleLieAlgebra::attemptFindingHEF(
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& inputOutputH,
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& inputOutputE,
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& inputOutputF,
  std::stringstream* logStream
) {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::AttemptFindingHEF");
  List<Polynomial<AlgebraicNumber> > theSystem;
  PolynomialSystem<AlgebraicNumber> system;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > mustBeZero;
  this->lieBracket(inputOutputH, inputOutputE, mustBeZero);
  mustBeZero -= inputOutputE * 2;
  for (int i = 0; i < mustBeZero.size(); i ++) {
    theSystem.addOnTop(mustBeZero.coefficients[i]);
  }
  this->lieBracket(inputOutputH, inputOutputF, mustBeZero);
  mustBeZero += inputOutputF * 2;
  for (int i = 0; i < mustBeZero.size(); i ++) {
    theSystem.addOnTop(mustBeZero.coefficients[i]);
  }
  this->lieBracket(inputOutputE, inputOutputF, mustBeZero);
  mustBeZero -= inputOutputH;
  for (int i = 0; i < mustBeZero.size(); i ++) {
    theSystem.addOnTop(mustBeZero.coefficients[i]);
  }
  if (logStream != nullptr) {
    *logStream << "The system to solve: ";
    for (int i = 0; i < theSystem.size; i ++) {
      *logStream << "<br>" << theSystem[i].toString() << " = 0 ";
    }
  }
  system.maximumSerreSystemComputationsPreferred = 4001;
  system.groebner.maximumPolynomialComputations = 2001;
  system.groebner.polynomialOrder.monomialOrder.setComparison(MonomialP::greaterThan_rightLargerWins);
  system.solveSerreLikeSystem(theSystem);
  if (!system.flagSystemSolvedOverBaseField) {
    if (logStream != nullptr) {
      if (system.flagSystemProvenToHaveNoSolution) {
        *logStream << "<br><b>System proven to have no solution. </b>";
      }
      if (system.flagSystemProvenToHaveSolution) {
        *logStream << "<br><b>System proven to have solution, but I failed to find it.</b> "
        << "Either the solution lies outside of the base field, or my heuristics are not good enough.";
      }
    }
    return false;
  }
  if (logStream != nullptr) {
    *logStream << "Solved successfully! One solution: " << system.toStringSerreLikeSolution();
  }
  PolynomialSubstitution<AlgebraicNumber> theSolutionSub;
  system.getSubstitutionFromPartialSolutionSerreLikeSystem(theSolutionSub);
  inputOutputF.substitutionCoefficients(theSolutionSub);
  inputOutputH.substitutionCoefficients(theSolutionSub);
  inputOutputE.substitutionCoefficients(theSolutionSub);
  if (logStream != nullptr) {
    *logStream << "<br>H = " << inputOutputH.toString() << "<br>E = "
    << inputOutputE.toString() << "<br>F = " << inputOutputF.toString();
  }
  return true;
}

bool SemisimpleLieAlgebra::attemptExtendingEtoHEFwithHinCartan(
  ElementSemisimpleLieAlgebra<AlgebraicNumber>& theE,
  ElementSemisimpleLieAlgebra<AlgebraicNumber>& outputH,
  ElementSemisimpleLieAlgebra<AlgebraicNumber>& outputF,
  std::stringstream* logStream
) {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::attemptExtendingEtoHEFwithHinCartan");
  Matrix<AlgebraicNumber> theM;
  this->getAdjoint(theM, theE);
  MatrixTensor<AlgebraicNumber> theMatTensor, theId;
  theMatTensor = theM;
  theId.makeIdentity(theM.numberOfRows);
  MathRoutines::raiseToPower(theMatTensor, this->getNumberOfPositiveRoots(), theId);
  if (!theMatTensor.isEqualToZero()) {
    if (logStream != nullptr) {
      *logStream << "The input E element " << theE.toString()
      << " is not nilpotent. The matrix tensor is: " << theMatTensor.toString();
    }
    return false;
  }
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > unknownH, unknownF, knownE;
  knownE = theE;
  this->getGenericElementCartan(unknownH, 0);
  this->getGenericElementNegativeBorelNilradical(unknownF, this->getRank());
  bool success = this->attemptFindingHEF(unknownH, knownE, unknownF, logStream);
  if (!success) {
    return false;
  }
  outputH = unknownH;
  outputF = unknownF;
  return true;
}

SubalgebraSemisimpleLieAlgebra::SubalgebraSemisimpleLieAlgebra() {
  this->owner = nullptr;
}

bool SubalgebraSemisimpleLieAlgebra::checkInitialization() {
  if (this->owner == nullptr) {
    global.fatal << "Non-initilized (no owner) subalgebra of semisimple Lie algebra." << global.fatal;
  }
  return true;
}

std::string SubalgebraSemisimpleLieAlgebra::toString(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("SubalgebraSemisimpleLieAlgebra::toString");
  (void) theFormat; //avoid unused parameter warning in a portable way
  if (this->owner == nullptr) {
    return "A non-initialized subalgebra of a semisimple Lie algebra; ";
  }
  std::stringstream out;
  out << "A subalgebra of dimension " << this->theBasis.size << " lying in "
  << this->owner->theWeyl.theDynkinType.toString() << ".<br>The subalgebra has basis: "
  << this->theBasis.toStringCommaDelimited();
  return out.str();
}

void SubalgebraSemisimpleLieAlgebra::computeBasis() {
  MacroRegisterFunctionWithName("SubalgebraSemisimpleLieAlgebra::computeBasis");
  this->checkInitialization();
  this->theBasis = this->theGenerators;
  this->owner->generateLieSubalgebra(this->theBasis);
}

void SubalgebraSemisimpleLieAlgebra::computeCartanSubalgebra() {
  MacroRegisterFunctionWithName("SubalgebraSemisimpleLieAlgebra::computeCartanSubalgebra");
  this->checkInitialization();
  List<MatrixTensor<AlgebraicNumber> > theAds;
  MatrixTensor<AlgebraicNumber> theAd;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > currentCentralizer;
  this->cartanSubalgebra.setSize(0);
  currentCentralizer = this->theBasis;
  ElementSemisimpleLieAlgebra<AlgebraicNumber> newElt;
  ProgressReport theReport0, theReport1;
  std::stringstream reportStream0;
  reportStream0 << "Computing Cartan subalgebra of a subalgebra of "
  << this->owner->theWeyl.theDynkinType.toString()
  << " with basis " << this->theBasis.toStringCommaDelimited();
  theReport0.report(reportStream0.str());
  while (currentCentralizer.size > 0) {
    std::stringstream reportStream1;
    reportStream1 << "Currently, the Cartan subalgebra basis candidates are: "
    << this->cartanSubalgebra.toStringCommaDelimited() << "; remaining centralizer: "
    << currentCentralizer.toStringCommaDelimited();
    theReport1.report(reportStream1.str());
    theAds.setSize(currentCentralizer.size);
    for (int i = 0; i < currentCentralizer.size; i ++) {
      this->owner->getAdjoint(theAds[i], currentCentralizer[i]);
    }
    bool foundNewElement = false;
    for (int i = 0; i < theAds.size; i ++) {
      if (!theAds[i].isNilpotent()) {
        foundNewElement = true;
        newElt = currentCentralizer[i];
        break;
      }
    }
    if (!foundNewElement) {
      for (int i = 0; i < theAds.size; i ++) {
        for (int j = i + 1; j < theAds.size; j ++) {
          theAd = theAds[i] + theAds[j];
          if (!theAd.isNilpotent()) {
            newElt = currentCentralizer[i] + currentCentralizer[j];
            foundNewElement = true;
            i = theAds.size + 1;
            break;
          }
        }
      }
    }
    if (!foundNewElement) {
      global.fatal  << "This shouldn't happen: could not found a new nilpotent element. " << global.fatal;
    }
    this->cartanSubalgebra.addOnTop(newElt);
    this->owner->getCommonCentralizer(this->cartanSubalgebra, currentCentralizer);
    newElt.intersectVectorSpaces(currentCentralizer, this->theBasis, currentCentralizer, nullptr);
  }

}

void WeylGroupData::operator+=(const WeylGroupData& other) {
  DynkinType theType = this->theDynkinType;
  theType += other.theDynkinType;
  this->makeFromDynkinType(theType);
}

int SemisimpleSubalgebras::getIndexFullSubalgebra() const {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::getIndexFullSubalgebra");
  for (int i = 0; i < this->theSubalgebras.theValues.size; i ++) {
    if (this->owner->theWeyl.theDynkinType == this->theSubalgebras.theValues[i].weylNonEmbedded->theDynkinType) {
      return i;
    }
  }
  return - 1;
}

std::string SemisimpleSubalgebras::toStringSubalgebraNumberWithAmbientLink(
  int actualindexSubalgebra, FormatExpressions* theFormat
) const {
  std::stringstream out;
  CandidateSemisimpleSubalgebra& theCandidate = this->theSubalgebras.theValues[actualindexSubalgebra];
  out << "Subalgebra "
  << "\\(" << theCandidate.weylNonEmbedded->theDynkinType.toString() << "\\)"
  << " &#8618; " << "<a href=\"./" << this->displayNameMainFile1NoPath << "\">"
  << "\\(" << this->owner->theWeyl.theDynkinType.toString(theFormat) << "\\)" << "</a>";
  int displayIndex = this->getDisplayIndexFromActual(actualindexSubalgebra);
  out << "<br>" << displayIndex << " out of " << this->theSubalgebras.size() << "\n";
  return out.str();
}

int SemisimpleSubalgebras::getDisplayIndexFromActual(int actualindexSubalgebra) const {
  int result = 0;
  for (int i = 0; i <= actualindexSubalgebra; i ++) {
    if (!this->theSubalgebras.theValues[i].flagSystemProvedToHaveNoSolution) {
      result ++;
    }
  }
  return result;
}

std::string SemisimpleSubalgebras::getRelativePhysicalFileNameSubalgebra(int actualindexSubalgebra) const {
  std::stringstream out;
  out << this->owner->toStringVirtualFolderName();
  out << FileOperations::cleanUpForFileNameUse(this->owner->theWeyl.theDynkinType.toString())
  << "_subalgebra_" << this->getDisplayIndexFromActual(actualindexSubalgebra) << ".html";
  return out.str();
}

std::string SemisimpleSubalgebras::getRelativePhysicalFileNameFKFTNilradicals(int actualindexSubalgebra) const {
  std::stringstream out;
  out << this->owner->toStringVirtualFolderName();
  out << FileOperations::cleanUpForFileNameUse(this->owner->theWeyl.theDynkinType.toString())
  << "_subalgebra_" << this->getDisplayIndexFromActual(actualindexSubalgebra) << "_FKFTnilradicals.html";
  return out.str();
}

std::string SemisimpleSubalgebras::getDisplayFileNameSubalgebraAbsolute(int actualindexSubalgebra, FormatExpressions* theFormat) const {
  std::stringstream out;
  (void) theFormat;//avoid unused parameter warning in a portable way
  out << this->owner->toStringVirtualFolderName();
  out << FileOperations::cleanUpForFileNameUse(this->owner->theWeyl.theDynkinType.toString())
  << "_subalgebra_" << this->getDisplayIndexFromActual(actualindexSubalgebra) << ".html";
  return out.str();
}

std::string SemisimpleSubalgebras::getDisplayFileNameSubalgebraRelative(int actualindexSubalgebra, FormatExpressions* theFormat) const {
  std::stringstream out;
  (void) theFormat;//avoid unused parameter warning in a portable way
  //out << "./";
  out << FileOperations::cleanUpForFileNameUse(this->owner->theWeyl.theDynkinType.toString())
  << "_subalgebra_" << this->getDisplayIndexFromActual(actualindexSubalgebra) << ".html";
  return out.str();
}

std::string SemisimpleSubalgebras::getDisplayFileNameFKFTNilradicals(int actualindexSubalgebra, FormatExpressions* theFormat) const {
  std::stringstream out;
  (void) theFormat;//avoid unused parameter warning in a portable way
  //out << this->owner->toStringVirtualFolderName();
  out << FileOperations::cleanUpForFileNameUse(this->owner->theWeyl.theDynkinType.toString()) << "_subalgebra_"
  << this->getDisplayIndexFromActual(actualindexSubalgebra) << "_FKFTnilradicals.html";
  return out.str();
}

std::string DynkinType::toString(FormatExpressions* theFormat) const {
  return this->::LinearCombination<DynkinSimpleType, Rational>::toString(theFormat);
}

void SemisimpleSubalgebras::checkFileWritePermissions() {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::checkFileWritePermissions");
  this->checkConsistency();
  this->computeFolderNames(this->currentFormat);
  std::fstream testFile;
  std::string testFileNameRelative =
  this->owner->toStringVirtualFolderName() + "testFileWritePermissionsSSsas.txt";
  std::string testFileFolderPhysical;
  FileOperations::getPhysicalFileNameFromVirtual(
    this->owner->toStringVirtualFolderName(), testFileFolderPhysical, false, false, nullptr
  );
  global.externalCommandNoOutput("mkdir " + testFileFolderPhysical, true);

  if (!FileOperations::openFileCreateIfNotPresentVirtualCreateFoldersIfNeeded(testFile, testFileNameRelative, false, true, false)) {
    global.fatal << "<br>This may or may not be a programming error. I requested to create file "
    << this->virtualNameMainFile1
    << " for output. However, the file failed to create. "
    << "Possible explanations: "
    << "1. Programming error. "
    << "2. The calculator has no write permission to the "
    << "folder in which the file is located. "
    << "3. The folder does not exist for some reason lying outside of the calculator. " << global.fatal;
  }
  FileOperations::openFileCreateIfNotPresentVirtual(testFile, testFileNameRelative, false, true, false);
  testFile << "Write permissions test file.";
}

void SemisimpleSubalgebras::writeReportToFiles() {
  this->millisecondsComputationEnd = global.getElapsedMilliseconds();
  this->numAdditions = Rational::totalSmallAdditions + Rational::totalLargeAdditions;
  this->numMultiplications = Rational::totalSmallMultiplications + Rational::totalLargeMultiplications;
  this->currentFormat.flagUseHTML = true;
  this->currentFormat.flagUseHtmlAndStoreToHD = true;
  this->currentFormat.flagUseLatex = true;
  this->currentFormat.flagUseMathSpanPureVsMouseHover = true;
  std::fstream fileSlowLoad;
  FileOperations::openFileCreateIfNotPresentVirtual(
    fileSlowLoad, this->virtualNameMainFile1, false, true, false
  );
  std::stringstream commonHead;
  commonHead << "<html><title>Semisimple subalgebras of the semisimple Lie algebras: the subalgebras of "
  << this->owner->theWeyl.theDynkinType.toString()
  << "</title>";
  commonHead << HtmlRoutines::getCSSLinkLieAlgebrasAndCalculator("../../../");
  commonHead << HtmlRoutines::getJavascriptLinkGraphicsNDimensionsWithPanels("../../../");
  commonHead << HtmlRoutines::getJavascriptMathjax("../../../");
  commonHead << "<body>";

  fileSlowLoad << commonHead.str() << this->toString(&this->currentFormat);
  fileSlowLoad << "</body></html>";
}

void SemisimpleSubalgebras::computeFolderNames(FormatExpressions& inputFormat) {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::computeFolderNames");
  (void) inputFormat;//avoid unused parameter warning in a portable way
  this->checkConsistency();
  if (this->owner == nullptr) {
    global.fatal << "To compute semisimple subalgebra folders, "
    << "you need to specify the ambient Lie algebra. " << global.fatal;
  }

  this->displayNameMainFile1NoPath = this->owner->toStringFileNameNoPathSemisimpleSubalgebras();
  this->displayNameMainFile1WithPath = this->owner->toStringDisplayFolderName("../../../") + this->displayNameMainFile1NoPath;
  this->virtualNameMainFile1 =
  this->owner->toStringVirtualFolderName() + this->displayNameMainFile1NoPath;
}

std::string SemisimpleSubalgebras::toStringSemisimpleSubalgebraSummaryHTML(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::toStringSemisimpleSubalgebraSummaryHTML");
  (void) theFormat;//avoid unused parameter warning in a portable way
  if (!this->flagComputeNilradicals) {
    return "";
  }
  std::stringstream out;
  int numIsotypicallyCompleteNilrads = 0;
  int numFailingConeCondition = 0;
  int numNoLinfRelFound = 0;
  int numNonCentralizerConditionWithConeCondition = 0;
  int numBadParabolics = 0;
  for (int i = 0; i < this->theSubalgebras.theValues.size; i ++) {
    numIsotypicallyCompleteNilrads += this->theSubalgebras.theValues[i].FKNilradicalCandidates.size;
    numFailingConeCondition += this->theSubalgebras.theValues[i].NumConeIntersections;
    numNoLinfRelFound += this->theSubalgebras.theValues[i].NumCasesNoLinfiniteRelationFound;
    numNonCentralizerConditionWithConeCondition += this->theSubalgebras.theValues[i].NumCentralizerConditionFailsconeConditionHolds;
    numBadParabolics += this->theSubalgebras.theValues[i].NumBadParabolics;
  }
  out << "<br>There are " << numIsotypicallyCompleteNilrads
  << " possible isotypic nilradical extensions of the primal subalgebras. Of them "
  << numFailingConeCondition << " have intersecting cones. Of the remaining "
  << numIsotypicallyCompleteNilrads - numFailingConeCondition
  << " nilradical extensions with non-intersecting cones, "
  << numIsotypicallyCompleteNilrads - numFailingConeCondition - numNonCentralizerConditionWithConeCondition
  << " satisfy(ies) the centralizer condition and " << numNonCentralizerConditionWithConeCondition
  << " fail(s) the centralizer condition.";
  if (numBadParabolics > 0) {
    out << "<br><span style =\"color:#FF0000\">Of the subalgebras satisfying the centralizer condition there are  "
    << numBadParabolics
    << " pabolic subalgebra(s) that do not extend to parabolic subalgebra(s) of the ambient Lie algebra with Levi types A and C. "
    << " For these subalgebras the PSZ construction is not proven to hold. </span>";
  } else {
    out << "<br><span style =\"color:#0000FF\"> In each of "
    << numIsotypicallyCompleteNilrads - numFailingConeCondition - numNonCentralizerConditionWithConeCondition
    << " case(s) when the centralizer condition holds, "
    << "the parabolic subalgebra in the centralizer with Levi types A and C extends "
    << "to parabolic subalgebra of the ambient Lie algebra whose Levi types are A and C only. </span>";
  }
  if (numFailingConeCondition > 0) {
    if (numNoLinfRelFound > 0) {
      out << "<br><span style =\"color:#FF0000\">In " << numNoLinfRelFound
      << " cases no L-infinite relation was found. </span>";
    } else {
      out << "<br><span style =\"color:#0000FF\"> In each of " << numFailingConeCondition
      << " case(s) of intersecting cones, an L-infinite relation was found. </span>";
    }
  }
  return out.str();
}

std::string SemisimpleSubalgebras::toStringSemisimpleSubalgebrasSummaryLaTeX(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::toStringSemisimpleSubalgebrasSummaryLaTeX");
  if (!this->flagComputeNilradicals) {
    return "";
  }
  std::stringstream out;
  int numIsotypicallyCompleteNilrads = 0;
  int numFailingConeCondition = 0;
  int numNoLinfRelFound = 0;
  int numNonCentralizerConditionWithConeCondition = 0;
  int numBadParabolics = 0;

  for (int i = 0; i < this->theSubalgebras.theValues.size; i ++) {
    numIsotypicallyCompleteNilrads += this->theSubalgebras.theValues[i].FKNilradicalCandidates.size;
    numFailingConeCondition += this->theSubalgebras.theValues[i].NumConeIntersections;
    numNoLinfRelFound += this->theSubalgebras.theValues[i].NumCasesNoLinfiniteRelationFound;
    numNonCentralizerConditionWithConeCondition += this->theSubalgebras.theValues[i].NumCentralizerConditionFailsconeConditionHolds;
    numBadParabolics += this->theSubalgebras.theValues[i].NumBadParabolics;
  }
  if (numBadParabolics > 0) {
    out << "<br><span style =\"color:#FF0000\">There are " << numBadParabolics << " bad parabolic subalgebras!</span><br>";
  }
  out << "\n<br>\n\\begin{longtable}{ccp{3cm}p{3cm}cc}";
  out << "\\caption{Semisimple subalgebras in type $" << this->owner->theWeyl.theDynkinType.toString(theFormat)
  << "$ \\label{tableSSSubalgerbas" << this->owner->theWeyl.theDynkinType.toString(theFormat) << "}. ";
  out << "Number of isotypically complete nilradicals: " << numIsotypicallyCompleteNilrads << ", of them "
  << numFailingConeCondition << " fail the cone condition.";
  if (numNoLinfRelFound == 0) {
    out << "<br>In all " << numFailingConeCondition << " cases, an $\\mathfrak{l}$-infinite relation was found. ";
  } else {
    out << "<br>In " << numNoLinfRelFound << " cases, no L-infinite relation was found. <br>";
  }
  out << "}\\\\\n<br>\n";
  out << "Type $\\mathfrak s$ & Centralizer &Decomp. $\\mathfrak g$ over "
  << "$\\mathfrak s$ &Decomp. $\\mathfrak g$ over $\\mathfrak s\\oplus \\mathfrak h_c$"
  << "&\\#$\\mathfrak n_\\mathfrak l$& \\# cone failures\\\\\\hline\n<br>\n";
  DynkinType typeCentralizer;
  FormatExpressions tempCharFormat;
  for (int i = 0; i < this->theSubalgebras.theValues.size; i ++) {
    const CandidateSemisimpleSubalgebra& currentSA = this->theSubalgebras.theValues[i];
    if (currentSA.flagSystemProvedToHaveNoSolution) {
      continue;
    }
    out << "$" << currentSA.weylNonEmbedded->theDynkinType.toString(theFormat) << "$";
    if (!currentSA.flagCentralizerIsWellChosen) {
      continue;
    }
    typeCentralizer.makeZero();
    if (currentSA.indexMaxSSContainer != - 1) {
      typeCentralizer = this->theSubalgebras.theValues[currentSA.indexMaxSSContainer].weylNonEmbedded->theDynkinType -
      currentSA.weylNonEmbedded->theDynkinType;
    }
    out << "& $ ";
    if (!typeCentralizer.isEqualToZero()) {
      out << typeCentralizer.toString();
      if (currentSA.centralizerRank != typeCentralizer.getRank()) {
        out << "\\oplus";
      }
    }
    if (currentSA.centralizerRank != typeCentralizer.getRank()) {
      out << "\\mathfrak h_{" << (currentSA.centralizerRank - typeCentralizer.getRank()).toString() << "}";
    }
    out << "$";
    if (!currentSA.charFormaT.isZeroPointer()) {
      tempCharFormat = currentSA.charFormaT.getElementConst();
    }
    out << "&$" << currentSA.theCharNonPrimalFundCoords.toString(&tempCharFormat) << "$ ";
    out << "&$" << currentSA.thePrimalChaR.toString(&tempCharFormat) << "$ ";
    out << "& " << currentSA.FKNilradicalCandidates.size << "&" << currentSA.NumConeIntersections;
    out << "\\\\ \\hline \n<br>\n";
  }
  out << "\\end{longtable}\n<br>\n";
  out << "\\end{document}";
  return out.str();
}

std::string SemisimpleSubalgebras::toString(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::toString");
  HtmlRoutines::globalMathSpanID = 0;
  bool writingToHD = theFormat == nullptr ? false : theFormat->flagUseHtmlAndStoreToHD;
  std::stringstream out;
  int candidatesRealized = 0;
  int candidatesNotRealizedNotProvenImpossible = 0;
  int candidatesProvenImpossible = 0;
  for (int i = 0; i < this->theSubalgebras.theValues.size; i ++) {
    CandidateSemisimpleSubalgebra& currentSA = this->theSubalgebras.theValues[i];
    if (currentSA.flagSystemProvedToHaveNoSolution) {
      candidatesProvenImpossible ++;
    }
    if (currentSA.flagSystemSolved) {
      candidatesRealized ++;
    }
  }
  out << "<h1>Lie algebra " << this->owner->toStringLieAlgebraNameFullHTML()
  << "<br>Semisimple complex Lie subalgebras</h1>";
  out << this->owner->toStringHTMLMenuStructureSummary("", true, true, true, false);

  candidatesNotRealizedNotProvenImpossible = this->theSubalgebras.size() - candidatesRealized - candidatesProvenImpossible;
  if (!writingToHD) {
    out << candidatesRealized << " subalgebras realized.";
    out << "<br>Up to linear equivalence, there are " << this->theSubalgebras.size()
    << " = " << candidatesRealized << " realized + "
    << candidatesProvenImpossible << " proven impossible + " << candidatesNotRealizedNotProvenImpossible
    << " neither realized nor proven impossible. \n<hr>\n ";
  } else {
    out << "Up to linear equivalence, there are total " << candidatesRealized
    << " semisimple subalgebras (including the full subalgebra)";
    if (candidatesNotRealizedNotProvenImpossible != 0) {
      out << " and "  << candidatesNotRealizedNotProvenImpossible
      << " semisimple subalgebra candidate(s) which were not realized (but not proven impossible)";
    }
    out << ". ";
  }
  out << "The subalgebras are ordered by rank, "
  << "Dynkin indices of simple constituents and dimensions of simple constituents. <br>"
  << "The upper index indicates the Dynkin index, "
  << "the lower index indicates the rank of the subalgebra.<br>";
  out << this->toStringTableSubalgebraLinksTable(theFormat) << "<hr>";
  if (this->comments != "") {
    out << "<a href = '" << this->fileNameToLogComments << "'>Generation comments.</a>";
  }
  bool showTime = theFormat == nullptr ? true : theFormat->flagIncludeMutableInformation;
  if (showTime) {
    if (this->millisecondsComputationStart > 0 && this->millisecondsComputationEnd > 0) {
      out << "<br>Computation milliseconds: "
      << this->millisecondsComputationEnd - this->millisecondsComputationStart << ".";
    }
  }
  if (this->numAdditions > 0) {
    out << "<br>" << this->numAdditions + this->numMultiplications << " total arithmetic operations performed = "
    << this->numAdditions << " additions and "
    << this->numMultiplications << " multiplications. ";
  }
  out << this->toStringPart2(theFormat);
  return out.str();
}

std::string SemisimpleSubalgebras::toStringSubalgebrasNoHDWrite(FormatExpressions* theFormat) {
  std::stringstream out;
  for (int i = 0; i < this->theSubalgebras.theValues.size; i ++) {
    if (!this->theSubalgebras.theValues[i].flagSystemProvedToHaveNoSolution) {
      out << this->toStringSubalgebraNumberWithAmbientLink(i, theFormat) << "\n<br>\n";
    }
    out << this->theSubalgebras.theValues[i].toString(theFormat) << "\n<hr>\n ";
  }
  return out.str();
}

void SemisimpleSubalgebras::writeSubalgebraToFile(FormatExpressions *theFormat, int subalgebraIndex) {
  CandidateSemisimpleSubalgebra& currentSubalgebra = this->theSubalgebras.theValues[subalgebraIndex];
  if (currentSubalgebra.flagSystemProvedToHaveNoSolution) {
    return;
  }
  std::fstream outputFileSubalgebra;
  if (!FileOperations::openFileCreateIfNotPresentVirtual(
    outputFileSubalgebra, this->getRelativePhysicalFileNameSubalgebra(subalgebraIndex), false, true, false
  )) {
    global.fatal << "<br>This may or may not be a programming error. While processing subalgebra of actual index "
    << subalgebraIndex << " and display index "
    << this->getDisplayIndexFromActual(subalgebraIndex) << ", I requested to create file "
    << this->getRelativePhysicalFileNameSubalgebra(subalgebraIndex)
    << " for output. However, the file failed to create. "
    << "Possible explanations: 1. Programming error. "
    << "2. The calculator has no write permission to the folder in which the file is located. "
    << "3. The folder does not exist for some reason lying outside of the calculator. " << global.fatal;
  }
  outputFileSubalgebra << "<html>\n" << HtmlRoutines::getJavascriptMathjax("../../../")
  << "\n" << HtmlRoutines::getCSSLinkLieAlgebrasAndCalculator("../../../")
  << "\n" << HtmlRoutines::getJavascriptLinkGraphicsNDimensionsWithPanels("../../../")
  << "\n<body>";
  outputFileSubalgebra << this->toStringSubalgebraNumberWithAmbientLink(subalgebraIndex, theFormat);
  outputFileSubalgebra << "<div class = \"divSubalgebraInformation\">";
  outputFileSubalgebra << "Computations done by the " << HtmlRoutines::getHtmlLinkToGithubRepository("calculator project");
  outputFileSubalgebra << ".</div>";
  outputFileSubalgebra
  << "<br>" <<  currentSubalgebra.toString(theFormat);
  if (this->flagComputeNilradicals) {
    std::fstream outputFileFKFTnilradicals;
    if (!FileOperations::openFileCreateIfNotPresentVirtual(
      outputFileFKFTnilradicals, "output/" + this->getRelativePhysicalFileNameFKFTNilradicals(subalgebraIndex), false, true, false
    )) {
      global.fatal << "<br>This may or may not be a programming error. "
      << "While processing subalgebra of actual index " << subalgebraIndex
      << " and display index " << this->getDisplayIndexFromActual(subalgebraIndex)
      << ", I requested to create file "
      << this->getRelativePhysicalFileNameFKFTNilradicals(subalgebraIndex)
      << " for output. However, the file failed to create. "
      << "Possible explanations: 1. Programming error. "
      << "2. The calculator has no write permission to the "
      << "folder in which the file is located. "
      << "3. The folder does not exist for some reason lying outside of the calculator. " << global.fatal;
    }
    outputFileFKFTnilradicals << "<html>"
    << HtmlRoutines::getJavascriptMathjax("../../../")
    << HtmlRoutines::getCSSLinkLieAlgebrasAndCalculator("../../../")
    << "<body>"
    << this->toStringAlgebraLink(subalgebraIndex, theFormat)
    << currentSubalgebra.toStringNilradicals(theFormat) << "\n</body></html>";
  }
  outputFileSubalgebra << "\n</body></html>\n ";
}

std::string SemisimpleSubalgebras::toStringSubalgebrasWithHDWrite(FormatExpressions *theFormat) {
  std::stringstream out;
  if (theFormat != nullptr) {
    theFormat->flagCandidateSubalgebraShortReportOnly = true;
    theFormat->flagUseHtmlAndStoreToHD = true;
  }
  out << this->toStringSubalgebrasNoHDWrite(theFormat);
  FormatExpressions theFormatCopy;
  if (theFormat != nullptr) {
    theFormatCopy = *theFormat;
  }
  theFormatCopy.flagUseMathSpanPureVsMouseHover = true;
  theFormatCopy.flagCandidateSubalgebraShortReportOnly = false;
  for (int i = 0; i < this->theSubalgebras.theValues.size; i ++) {
    this->writeSubalgebraToFile(&theFormatCopy, i);
  }
  return out.str();
}

std::string SemisimpleSubalgebras::toStringTableSubalgebraLinksTable(FormatExpressions* theFormat) {
  (void) theFormat;
  std::stringstream out;
  int numberOfColumns = 6;
  out << "<table class = \"tableSemisimpleLieAlgebrasList\">";
  bool rowStarted = false;
  int displayedInCurrentRow = 0;
  for (int i = 0; i < this->theSubalgebras.size(); i ++) {
    CandidateSemisimpleSubalgebra& theSA = this->theSubalgebras.theValues[i];
    if (theSA.flagSystemProvedToHaveNoSolution) {
      continue;
    }
    displayedInCurrentRow ++;
    if (!rowStarted) {
      out << "<tr>";
      rowStarted = true;
    }
    int displayIndex = this->getDisplayIndexFromActual(i);
    out << "<td>" << displayIndex << ". " << "<a href = \"#semisimple_subalgebra_" << displayIndex
    << "\">\\(" << theSA.weylNonEmbedded->theDynkinType.toString() << "\\)</a></td>";
    if (displayedInCurrentRow >= numberOfColumns) {
      out << "</tr>";
      rowStarted = false;
      displayedInCurrentRow = 0;
    }
  }
  if (rowStarted) {
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

std::string SemisimpleSubalgebras::toStringPart2(FormatExpressions *theFormat) {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::toStringPart2");
  std::stringstream out;
  bool writingToHD = theFormat == nullptr ? false : theFormat->flagUseHtmlAndStoreToHD;
  out << "<hr>";
  out << "The base field over which the subalgebras were realized is: ";
  if (this->ownerField == nullptr) {
    out << HtmlRoutines::getMathNoDisplay("\\mathbb Q");
  } else {
    out << this->ownerField->toString();
  }
  out << "<hr> ";
  int numRegularSAs = 0;
  int numSl2s = 0;
  for (int i = 0; i < this->theSubalgebras.theValues.size; i ++) {
    if (this->theSubalgebras.theValues[i].amRegularSA()) {
      numRegularSAs ++;
    }
    if (this->theSubalgebras.theValues[i].weylNonEmbedded->theDynkinType.isTypeAOne()) {
      numSl2s ++;
    }
  }
  out << "Number of root subalgebras other than "
  << "the Cartan and full subalgebra: "
  << numRegularSAs - 1;
  out << "<br>Number of sl(2)'s: " << numSl2s << "<hr>";

  std::string summaryString = this->toStringSemisimpleSubalgebraSummaryHTML(theFormat);
  if (summaryString != "") {
    out << "Summary: " << summaryString << "<hr>";
    if (this->flagProduceLaTeXtables) {
      out << "Summary in LaTeX\n<br><br>" << this->toStringSemisimpleSubalgebrasSummaryLaTeX(theFormat) << "\n<br><br><hr>";
    }
  }
  if (!writingToHD) {
    out << this->toStringSubalgebrasNoHDWrite(theFormat);
  } else {
    out << this->toStringSubalgebrasWithHDWrite(theFormat);
  }
  out << this->toStringPart3(theFormat);
  return out.str();
}

std::string SemisimpleSubalgebras::toStringSl2s(FormatExpressions *theFormat) {
  if (this->theSl2s.size == 0) {
    return "";
  }
  std::stringstream out;
  int numComputedOrbits = 0;
  for (int i = 0; i < this->orbits.size; i ++) {
    if (this->orbits[i].flagOrbitIsBuffered) {
      numComputedOrbits ++;
    }
  }
  out << "<hr>Of the " << this->orbits.size
  << " h element conjugacy classes " << numComputedOrbits
  << " had their Weyl group automorphism orbits computed and buffered. "
  << "The h elements and their computed orbit sizes follow. ";
  out << "<table><tr><td>h element</td><td>orbit size</td></tr>";
  for (int i = 0; i < this->orbits.size; i ++) {
    out << "<tr><td>" << this->theSl2s[i].theH.getCartanPart().toString() << "</td>";
    if (this->orbits[i].orbitSize != - 1) {
      out << "<td>" << this->orbits[i].orbitSize;
      if (this->orbits[i].flagOrbitIsBuffered == 0)
        out << "<b>(orbit enumerated but not stored)</b>";
      out << "</td>";
    } else {
      out << "<td>size not computed</td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  out << this->theSl2s.toString(theFormat);
  return out.str();
}

std::string SemisimpleSubalgebras::toStringPart3(FormatExpressions* theFormat) {
  std::stringstream out;
  bool writingToHD = theFormat == nullptr ? false : theFormat->flagUseHtmlAndStoreToHD;
  if (!writingToHD) {
    out << this->toStringSl2s(theFormat);
    out << "<hr>Calculator input for loading subalgebras directly without recomputation. "
    << this->ToStringProgressReport(theFormat);
  } else {
    std::string sl2SubalgebraReports = this->owner->toStringVirtualFolderName() + "orbit_computation_information_" +
    FileOperations::cleanUpForFileNameUse(this->owner->theWeyl.theDynkinType.toString()) + ".html";
    std::string loadSubalgebrasFile = this->owner->toStringVirtualFolderName() + "load_algebra_" +
    FileOperations::cleanUpForFileNameUse(this->owner->theWeyl.theDynkinType.toString()) + ".html";

    out << "<a href = '" << sl2SubalgebraReports  << "'>Nilpotent orbit computation summary</a>.";
    out << "<hr><a href = '" << loadSubalgebrasFile  << "'>Calculator input for subalgebras load</a>.";
    std::stringstream fileSl2Content, fileLoadContent;
    fileSl2Content << "<html>"
    << HtmlRoutines::getJavascriptMathjax("../../../")
    << "<body>"
    << this->toStringSl2s()
    << "</body></html>";
    fileLoadContent << "<html>"
    << HtmlRoutines::getJavascriptMathjax("../../../")
    << "<body>"
    << this->ToStringProgressReport(theFormat)
    << "</body></html>";
    FileOperations::writeFileVirual(sl2SubalgebraReports, fileSl2Content.str(), nullptr);
    FileOperations::writeFileVirual(loadSubalgebrasFile, fileLoadContent.str(), nullptr);
  }
  return out.str();
}

void SemisimpleSubalgebras::getCentralizerChains(List<List<int> >& outputChains) {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::getCentralizerChains");
  outputChains.setSize(0);
  Selection Explored;
  Explored.initialize(this->theSubalgebras.theValues.size);
  outputChains.reserve(this->theSubalgebras.theValues.size);
  for (int i = 0; i < this->theSubalgebras.theValues.size; i ++) {
    if (!Explored.selected[i]) {
      outputChains.setSize(outputChains.size + 1);
      outputChains.lastObject()->setSize(0);
      outputChains.lastObject()->addOnTop(i);
      int maxSScontainer = this->theSubalgebras.theValues[i].indexMaxSSContainer;
      if (maxSScontainer != - 1) {
        outputChains.lastObject()->addOnTop(maxSScontainer);
        Explored.addSelectionAppendNewIndex(maxSScontainer);
      }
    }
  }
}

void SemisimpleSubalgebras::computeSl2sInitOrbitsForComputationOnDemand() {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::computeSl2sInitOrbitsForComputationOnDemand");
  this->getSemisimpleOwner().FindSl2Subalgebras(this->getSemisimpleOwner(), this->theSl2s);
  this->theOrbitHelementLengths.clear();
  this->theOrbitDynkinIndices.clear();
  this->theOrbitHelementLengths.setExpectedSize(this->theSl2s.size);
  this->theOrbitDynkinIndices.setExpectedSize(this->theSl2s.size);
  this->orbits.setSize(this->theSl2s.size);
  List<ElementWeylGroupAutomorphisms> theGens;
  WeylGroupAutomorphisms& theWeylAutomorphisms = this->theSl2s.theRootSAs.theWeylGroupAutomorphisms;
  theWeylAutomorphisms.computeOuterAutoGenerators();
  ElementWeylGroupAutomorphisms theElt;
  for (int i = 0; i < this->owner->getRank(); i ++) {
    theElt.makeSimpleReflection(i, theWeylAutomorphisms);
    theGens.addOnTop(theElt);
  }
  List<MatrixTensor<Rational> >& outerAutos = theWeylAutomorphisms.theOuterAutos.theGenerators;
  for (int i = 0; i < outerAutos.size; i ++) {
    if (!outerAutos[i].isIdentity()) {
      theElt.makeOuterAuto(i, theWeylAutomorphisms);
      theGens.addOnTop(theElt);
    }
  }
  for (int i = 0; i < this->theSl2s.size; i ++) {
    this->theOrbitHelementLengths.addOnTop(this->theSl2s[i].LengthHsquared);
    this->theOrbitDynkinIndices.addOnTop(DynkinSimpleType('A', 1, this->theSl2s[i].LengthHsquared));
    this->orbits[i].initialize(theGens, this->theSl2s[i].theH.getCartanPart());
  }
}

bool SemisimpleSubalgebras::loadState(
  List<int>& currentChainInt,
  List<int>& numExploredTypes,
  List<int>& numExploredHs,
  std::stringstream& reportStream
) {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::loadState");
  this->findTheSemisimpleSubalgebrasInitialize();
  if (currentChainInt.size != numExploredTypes.size || currentChainInt.size != numExploredHs.size) {
    reportStream << "<hr>Input state is corrupt: currentChainInt.size: "
    << currentChainInt.size << ", numExploredTypes.size: "
    << numExploredTypes.size << ", numExploredHs.size: " << numExploredHs.size;
    return false;
  }
  this->currentHCandidatesScaledToActByTwo.setSize(0);
  this->currentNumHcandidatesExplored.setSize(0);
  this->currentNumLargerTypesExplored.setSize(0);
  this->currentPossibleLargerDynkinTypes.setSize(0);
  this->currentRootInjections.setSize(0);
  this->currentSubalgebraChain.setSize(0);
  for (int i = 0; i < currentChainInt.size; i ++) {
    if (currentChainInt[i] == - 1 && i == 0) {
      CandidateSemisimpleSubalgebra emptySA;
      this->makeEmptyCandidateSubalgebra(emptySA);
      this->addSubalgebraToStack(emptySA, numExploredTypes[i], numExploredHs[i]);
      continue;
    }
    if (currentChainInt[i] < 0 || currentChainInt[i] >= this->theSubalgebras.theValues.size) {
      reportStream << "<hr>Corrupt subalgebra index: " << currentChainInt[i];
      return false;
    }
    CandidateSemisimpleSubalgebra& currentSA = this->theSubalgebras.theValues[currentChainInt[i]];
    if (!currentSA.computeAndVerifyFromGeneratorsAndHs()) {
      reportStream << "<hr>Subalgebra " << currentSA.weylNonEmbedded->theDynkinType.toString()
      << " is corrupt. " << currentSA.comments;
      return false;
    }
    bool isGood = true;
    if (!currentSA.theHs.containsAtLeastOneCopyOfEach(this->baseSubalgebra().theHsScaledToActByTwoInOrderOfCreation)) {
      isGood = false;
    } else {
      currentSA.theHsScaledToActByTwoInOrderOfCreation = this->baseSubalgebra().theHsScaledToActByTwoInOrderOfCreation;
      for (int i = 0; i < currentSA.theHs.size; i ++) {
        if (! this->baseSubalgebra().theHsScaledToActByTwoInOrderOfCreation.contains(currentSA.theHs[i])) {
          currentSA.theHsScaledToActByTwoInOrderOfCreation.addOnTop(currentSA.theHs[i]);
          if (currentSA.theHsScaledToActByTwoInOrderOfCreation.size > currentSA.theHs.size) {
            isGood = false;
            break;
          }
        }
      }
    }
    if (currentSA.theHs.size != this->baseSubalgebra().theHs.size + 1) {
      isGood = false;
    }
    if (!isGood) {
      reportStream << "<hr>Subalgebra "
      << currentSA.weylNonEmbedded->theDynkinType.toString()
      << " does not appear to be parabolically induced by "
      << this->baseSubalgebra().weylNonEmbedded->theDynkinType.toString()
      << ". More precisely, " << currentSA.weylNonEmbedded->theDynkinType.toString()
      << " has h-elements " << currentSA.theHs.toString() << " however "
      << this->baseSubalgebra().weylNonEmbedded->theDynkinType.toString()
      << " has h-elements in order of creation: "
      << this->baseSubalgebra().theHsScaledToActByTwoInOrderOfCreation.toString()
      << " and h-elements in order induced by the type: " << this->baseSubalgebra().theHs.toString();
      return false;
    }
    this->addSubalgebraToStack(currentSA, numExploredTypes[i], numExploredHs[i]);
  }
  return true;
}

bool SemisimpleSubalgebras::findTheSemisimpleSubalgebrasContinue() {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::findTheSemisimpleSubalgebrasContinue");
  ProgressReport theReport;
  std::stringstream reportstream;
  reportstream << "State at beginning of computation: " << this->ToStringProgressReport();
  theReport.report(reportstream.str());
  while (this->incrementReturnFalseIfPastLast()) {
    theReport.report(this->ToStringProgressReport());
    if (!this->flagRealizedAllCandidates) {
      this->comments = this->comments + "Failed to realize all candidates, aborting computation.";
      return false;
    }
  }
  ProgressReport theReport2;
  std::stringstream reportStream2;
  reportStream2 << "Found a total of " << this->theSubalgebras.theValues.size << " subalgebras. Proceding to"
  << " adjust centralizers. ";
  theReport2.report(reportStream2.str());
  if (this->targetDynkinType.isEqualToZero()) {
    this->hookUpCentralizers(false);
    if (this->flagComputeNilradicals) {
      this->computePairingTablesAndFKFTtypes();
    }
  }
  return true;
}

void SemisimpleSubalgebras::findTheSemisimpleSubalgebrasInitialize() {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::findTheSemisimpleSubalgebrasInitialize");
  this->checkConsistency();
  if (this->owner == nullptr) {
    global.fatal << "<hr>Owner of semisimple subalgebras is zero" << global.fatal;
  }
  if (global.theResponse.monitoringAllowed()) {
    this->fileNameToLogComments = "LogFileComments_" +
    FileOperations::cleanUpForFileNameUse(this->owner->theWeyl.theDynkinType.toString()) +
    ".html";
    std::fstream LogFile;
    if (!FileOperations::openFileCreateIfNotPresentVirtual(
      LogFile, this->owner->toStringVirtualFolderName() + this->fileNameToLogComments, true, false, false
    )) {
      global.fatal << "Failed to open/create log file " << this->fileNameToLogComments
      << ". This is not fatal but I am crashing to let you know. ";
    }
    LogFile.close();
  }
  this->computeSl2sInitOrbitsForComputationOnDemand();
  this->currentSubalgebraChain.setExpectedSize(this->owner->getRank() + 2);
  this->currentSubalgebraChain.setSize(0);
}

void SemisimpleSubalgebras::makeEmptyCandidateSubalgebra(CandidateSemisimpleSubalgebra& output) {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::makeEmptyCandidateSubalgebra");
  DynkinType zeroType;
  this->makeCandidateSubalgebra(zeroType, output);
  Matrix<Rational> theZeroCartan;
  output.theSubalgebraNonEmbeddedDefaultScale =
  &this->theSubalgebrasNonDefaultCartanAndScale.getValueCreateNoInit(theZeroCartan);
  output.indexNonEmbeddedMeNonStandardCartan =
  this->theSubalgebrasNonDefaultCartanAndScale.getIndex(theZeroCartan);
}

void SemisimpleSubalgebras::makeCandidateSubalgebra(const DynkinType& input, CandidateSemisimpleSubalgebra& output) {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::makeCandidateSubalgebra");
  output.owner = this;
  bool needsInit = false;
  if (!this->theSubalgebrasNonEmbedded->contains(input)) {
    needsInit = true;
  }
  output.weylNonEmbedded = &this->theSubalgebrasNonEmbedded->getValueCreateNoInit(input).theWeyl;
  output.indexNonEmbeddedMeStandard = this->theSubalgebrasNonEmbedded->getIndex(input);
  if (needsInit) {
    output.weylNonEmbedded->makeFromDynkinType(input);
  }
}

bool SemisimpleSubalgebras::findTheSemisimpleSubalgebrasFromScratch(
  SemisimpleLieAlgebra& newOwner,
  AlgebraicClosureRationals& ownerField,
  MapReferences<DynkinType, SemisimpleLieAlgebra>& containerSubalgebras,
  ListReferences<SlTwoSubalgebras>& containerSl2Subalgebras,
  const DynkinType* targetType
) {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::findTheSemisimpleSubalgebrasFromScratch");
  if (this->theSubalgebrasNonEmbedded == nullptr || this->owner == nullptr) {
    this->initHookUpPointers(newOwner, &ownerField, &containerSubalgebras, &containerSl2Subalgebras);
  }

  this->checkInitialization();
  this->owner->computeChevalleyConstants();
  this->targetDynkinType.makeZero();
  if (targetType != nullptr) {
    this->targetDynkinType = *targetType;
  }
  this->findTheSemisimpleSubalgebrasInitialize();
  CandidateSemisimpleSubalgebra emptyCandidate;
  this->makeEmptyCandidateSubalgebra(emptyCandidate);
  this->addSubalgebraToStack(emptyCandidate, 0, 0);
  return this->findTheSemisimpleSubalgebrasContinue();
}

bool SemisimpleSubalgebras::ranksAndIndicesFit(const DynkinType& input) const {
  if (input.getRank() > this->owner->getRank()) {
    return false;
  }
  for (int i = 0; i < input.size(); i ++) {
    if (!this->theOrbitHelementLengths.contains(input[i].CartanSymmetricInverseScale * 2)) {
      return false;
    }
  }
  return true;
}

bool SemisimpleSubalgebras::growDynkinType(
  const DynkinType& input, List<DynkinType>& output, List<List<int> >* outputImagesSimpleRoots
) const {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::growDynkinType");
  HashedList<Rational> theLengths;
  theLengths.setExpectedSize(this->theOrbitHelementLengths.size);
  for (int i = 0; i < this->theOrbitHelementLengths.size; i ++) {
    theLengths.addOnTopNoRepetition(this->theOrbitHelementLengths[i] / 2);
  }
  output.setSize(0);
  if (outputImagesSimpleRoots != nullptr) {
    outputImagesSimpleRoots->setSize(0);
  }
  return input.grow(theLengths, this->owner->getRank(), output, outputImagesSimpleRoots);
}

Vector<Rational> SemisimpleSubalgebras::getHighestWeightFundNewComponentFromImagesOldSimpleRootsAndNewRoot(
  const DynkinType& input,
  const List<int>& imagesOldSimpleRootsAndNewRoot,
  CandidateSemisimpleSubalgebra& theSSSubalgebraToBeModified
) {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::getHighestWeightFundNewComponentFromImagesOldSimpleRootsAndNewRoot");
  theSSSubalgebraToBeModified.owner = this;
  Vector<Rational> result;
  if (input.isEqualToZero()) {
    this->makeEmptyCandidateSubalgebra(theSSSubalgebraToBeModified);
    result.makeZero(0);
    return result;
  } else {
    this->makeCandidateSubalgebra(input, theSSSubalgebraToBeModified);
  }
  Vector<Rational> newSimpleRoot, highestRootInSimpleRootModuleSimpleCoords;
  WeylGroupData& theWeyl = *theSSSubalgebraToBeModified.weylNonEmbedded;
  theWeyl.computeRho(true);
  theSSSubalgebraToBeModified.checkCandidateInitialization();
  int newIndex = *imagesOldSimpleRootsAndNewRoot.lastObject();
  int newRank = theWeyl.getDimension();
  newSimpleRoot.makeEi(newRank, newIndex);
  Vectors<Rational> simpleBasisOld;
  simpleBasisOld.setSize(newRank - 1);
  if (imagesOldSimpleRootsAndNewRoot.size != newRank) {
    global.fatal << "The root images must be "
    << newRank << " but there are " << imagesOldSimpleRootsAndNewRoot.size << " elements instead. "
    << "The type is " << input.toString() << ". " << global.fatal;
  }
  for (int i = 0; i < newRank - 1; i ++) {
    simpleBasisOld[i].makeEi(newRank, imagesOldSimpleRootsAndNewRoot[i]);
  }
  theWeyl.computeExtremeRootInTheSameKMod(simpleBasisOld, newSimpleRoot, highestRootInSimpleRootModuleSimpleCoords, true);
  result.setSize(newRank - 1);
  for (int i = 0; i < simpleBasisOld.size; i ++) {
    result[i] = theWeyl.rootScalarCartanRoot(highestRootInSimpleRootModuleSimpleCoords, simpleBasisOld[i]) * 2 /
    theWeyl.rootScalarCartanRoot(simpleBasisOld[i], simpleBasisOld[i]);
  }
  return result;
}

void CandidateSemisimpleSubalgebra::setUpInjectionHs(
  const CandidateSemisimpleSubalgebra& baseSubalgebra,
  const DynkinType& theNewType,
  const List<int>& theRootInjection,
  Vector<Rational>* newHScaledToActByTwo
) {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::setUpInjectionHs");
  this->reset(baseSubalgebra.owner);
  this->owner->makeCandidateSubalgebra(theNewType, *this);
  this->checkCandidateInitialization();
  this->theHsScaledToActByTwoInOrderOfCreation.reserve(baseSubalgebra.theHsScaledToActByTwoInOrderOfCreation.size + 1);
  this->theHsScaledToActByTwoInOrderOfCreation = baseSubalgebra.theHsScaledToActByTwoInOrderOfCreation;
  if (newHScaledToActByTwo != nullptr) {
    this->theHsScaledToActByTwoInOrderOfCreation.addOnTop(*newHScaledToActByTwo);
  }
  DynkinSimpleType newComponent = this->weylNonEmbedded->theDynkinType.getSmallestSimpleType();
  this->cartanSubalgebrasByComponentScaledToActByTwo = baseSubalgebra.cartanSubalgebrasByComponentScaledToActByTwo;
  int indexOffset = this->weylNonEmbedded->theDynkinType.getRank() - newComponent.theRank;
  int newIndexInNewComponent = 0;
//  if (!newComponent.isEqualToZero())
  newIndexInNewComponent = *theRootInjection.lastObject() - indexOffset;
  if (newComponent.theRank == 1) {
    this->cartanSubalgebrasByComponentScaledToActByTwo.setSize(this->cartanSubalgebrasByComponentScaledToActByTwo.size + 1);
    this->cartanSubalgebrasByComponentScaledToActByTwo.lastObject()->setSize(1);
    if (newHScaledToActByTwo != nullptr) {
      (*this->cartanSubalgebrasByComponentScaledToActByTwo.lastObject())[0] = *newHScaledToActByTwo;
    } else {
      (*this->cartanSubalgebrasByComponentScaledToActByTwo.lastObject())[0].setSize(0);
    }
  } else {
    Vectors<Rational>& oldComponentHs = *baseSubalgebra.cartanSubalgebrasByComponentScaledToActByTwo.lastObject();
    Vectors<Rational>& currentHsScaledToActByTwo = *this->cartanSubalgebrasByComponentScaledToActByTwo.lastObject();
    currentHsScaledToActByTwo.setSize(currentHsScaledToActByTwo.size + 1);
    for (int i = 0; i < newComponent.theRank - 1; i ++) {
      currentHsScaledToActByTwo[theRootInjection[indexOffset + i] - indexOffset] = oldComponentHs[i];
    }
    if (newHScaledToActByTwo != nullptr) {
      currentHsScaledToActByTwo[newIndexInNewComponent] = *newHScaledToActByTwo;
    } else {
      currentHsScaledToActByTwo[newIndexInNewComponent].setSize(0);
    }
  }
  this->computeHsAndHsScaledToActByTwoFromComponents();
}

void CandidateSemisimpleSubalgebra::computeHsAndHsScaledToActByTwoFromComponents() {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::ComputeHsScaledToActByTwo");
  this->theHsScaledToActByTwo.assignListList(this->cartanSubalgebrasByComponentScaledToActByTwo);
  this->theHs.setSize(this->theHsScaledToActByTwo.size);
  Matrix<Rational> cartanInComponentOrder;
  this->weylNonEmbedded->theDynkinType.getCartanSymmetricDefaultLengthKeepComponentOrder(cartanInComponentOrder);
  this->theSubalgebraNonEmbeddedDefaultScale =
  &this->owner->theSubalgebrasNonDefaultCartanAndScale.getValueCreateNoInit(cartanInComponentOrder);
  this->theSubalgebraNonEmbeddedDefaultScale->theWeyl.makeFromDynkinTypeDefaultLengthKeepComponentOrder(
    this->weylNonEmbedded->theDynkinType
  );
  this->theSubalgebraNonEmbeddedDefaultScale->theWeyl.computeRho(true);
  this->theSubalgebraNonEmbeddedDefaultScale->computeChevalleyConstants();
  this->indexNonEmbeddedMeNonStandardCartan =
  this->owner->theSubalgebrasNonDefaultCartanAndScale.getIndex(cartanInComponentOrder);
  int counter = - 1;
  List<DynkinSimpleType> theTypes;
  this->weylNonEmbedded->theDynkinType.getTypesWithMults(theTypes);
  for (int i = 0; i < this->cartanSubalgebrasByComponentScaledToActByTwo.size; i ++) {
    for (int j = 0; j < this->cartanSubalgebrasByComponentScaledToActByTwo[i].size; j ++) {
      counter ++;
      this->theHs[counter] = (this->theHsScaledToActByTwo[counter] * theTypes[i].getDefaultRootLengthSquared(j)) / 2;
    }
  }
}

bool SemisimpleSubalgebras::setUpParabolicInductionDataPrecomputedSubalgebra(CandidateSemisimpleSubalgebra& theCandidate) {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::setUpParabolicInductionDataPrecomputedSubalgebra");
  int indexPrecomputed = this->theSubalgebras.getIndex(theCandidate.theHs);
  if (indexPrecomputed == - 1)
    return false;
  if (
    this->theSubalgebras.theValues[indexPrecomputed].theHsScaledToActByTwoInOrderOfCreation.size !=
    theCandidate.theHs.size
  ) {
    this->theSubalgebras.theValues[indexPrecomputed].theHsScaledToActByTwoInOrderOfCreation =
    theCandidate.theHsScaledToActByTwoInOrderOfCreation;
  }
  this->theSubalgebras.theValues[indexPrecomputed].indexIamInducedFrom = theCandidate.indexIamInducedFrom;
  this->theSubalgebras.theValues[indexPrecomputed].rootInjectionsFromInducer = theCandidate.rootInjectionsFromInducer;
  theCandidate = this->theSubalgebras.theValues[indexPrecomputed];
  return theCandidate.computeAndVerifyFromGeneratorsAndHs();
}

bool CandidateSemisimpleSubalgebra::createAndAddExtendBaseSubalgebra(
  const CandidateSemisimpleSubalgebra& baseSubalgebra,
  Vector<Rational>& newHrescaledToActByTwo,
  const DynkinType& theNewType,
  const List<int>& theRootInjection
) {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::createAndAddExtendBaseSubalgebra");
  this->setUpInjectionHs(baseSubalgebra, theNewType, theRootInjection, &newHrescaledToActByTwo);
  this->indexIamInducedFrom = baseSubalgebra.indexInOwner;
  this->rootInjectionsFromInducer = theRootInjection;
  //induction history is complete.
  if (this->owner->setUpParabolicInductionDataPrecomputedSubalgebra(*this)) {
    return true;
  }
  this->checkFullInitialization();
  if (!baseSubalgebra.weylNonEmbedded->theDynkinType.isEqualToZero() && baseSubalgebra.indexInOwner == - 1) {
    global.fatal << "Attempt to induce a subalgebra "
    << "from a non-registered base subalgebra of type "
    << baseSubalgebra.weylNonEmbedded->theDynkinType.toString() << ". " << global.fatal;
  }
  ProgressReport theReport;
  if (!this->computeCharacter(false)) {
    if (theReport.tickAndWantReport()) {
      theReport.report("Candidate " + this->weylNonEmbedded->theDynkinType.toString() + " doesn't have fitting chars.");
    }
    return false;
  }
  this->computeSystem(false, false);
  if (this->flagSystemProvedToHaveNoSolution) {
    if (theReport.tickAndWantReport()) {
      theReport.report("Candidate " + this->weylNonEmbedded->theDynkinType.toString() + " -> no system solution.");
    }
  }
  for (int i = 0; i < this->owner->theSubalgebras.theValues.size; i ++) {
    if (this->weylNonEmbedded->theDynkinType == this->owner->theSubalgebras.theValues[i].weylNonEmbedded->theDynkinType) {
      if (this->isDirectSummandOf(this->owner->theSubalgebras.theValues[i])) {
        return false;
      }
    } else {
      if (
        this->weylNonEmbedded->theDynkinType.toString() ==
        this->owner->theSubalgebras.theValues[i].weylNonEmbedded->theDynkinType.toString()
      ) {
        global.fatal << "This is not supposed to happen: different Dynkin types with equal toString outputs. "
        << global.fatal;
      }
    }
  }
  return !this->flagSystemProvedToHaveNoSolution;
}

const Vector<Rational>& OrbitIteratorRootActionWeylGroupAutomorphisms::getCurrentElement() {
  MacroRegisterFunctionWithName("OrbitIteratorRootActionWeylGroupAutomorphisms::getCurrentElement");
  if (this->flagOrbitIsBuffered) {
    return this->orbitBuffer[this->currentIndexInBuffer];
  }
  return this->theIterator.getCurrentElement();
}

bool OrbitIteratorRootActionWeylGroupAutomorphisms::checkConsistency() {
  if (this->flagOrbitIsBuffered) {
    if (this->currentIndexInBuffer >= this->orbitBuffer.size) {
      global.fatal << "Current buffer index is: " << this->currentIndexInBuffer << " but the orbit has "
      << this->orbitBuffer.size << " elements. " << global.fatal;
    }
  }
  if (this->flagOrbitEnumeratedOnce) {
    if (this->currentIndexInBuffer >= this->orbitSize) {
      global.fatal << "Current buffer index is: " << this->currentIndexInBuffer << " but the orbit was computed to have "
      << this->orbitSize << " elements. " << global.fatal;
    }
  }
  if (this->flagOrbitIsBuffered) {
    if (this->orbitSize != this->orbitBuffer.size) {
     global.fatal << "Orbit is supposed to be buffered but the orbit buffer has "
     << this->orbitBuffer.size << " elements "
     << "and the orbit size is reported to be: "
     << this->orbitSize << ". A detailed orbit printout: <br>"
     << this->toString() << global.fatal;
    }
  }
  if (!this->flagOrbitEnumeratedOnce && this->flagOrbitIsBuffered) {
    global.fatal << "Orbit reported to be buffered but at the same time it hasn't been enumerated yet. "
    << this->toString() << global.fatal;
  }
  return true;
}

bool OrbitIteratorRootActionWeylGroupAutomorphisms::incrementReturnFalseIfPastLast() {
  MacroRegisterFunctionWithName("OrbitIteratorRootActionWeylGroupAutomorphisms::incrementReturnFalseIfPastLast");
  this->checkConsistency();
  this->theIterator.checkInitialization();
  if (this->flagOrbitIsBuffered) {
    this->currentIndexInBuffer ++;
    if (this->currentIndexInBuffer >= this->orbitSize) {
      this->currentIndexInBuffer = - 1;
      return false;
    }
    return true;
  }
  this->currentIndexInBuffer ++;
  if (this->flagOrbitEnumeratedOnce) {
    return this->theIterator.incrementReturnFalseIfPastLastFALSE();
  }
  if (!this->theIterator.incrementReturnFalseIfPastLastFALSE()) {
    this->orbitSize = this->currentIndexInBuffer;
    if (this->computedSize != - 1) {
      if (this->computedSize != this->orbitSize) {
        global.fatal << "This is a mathematical error: "
        << "the computed size of the orbit is " << this->computedSize.toString() << " "
        << "but I enumerated an orbit of size " << this->orbitSize
        << ". More details on the orbit follow. "
        << this->toString() << global.fatal;
      }
    }
    this->currentIndexInBuffer = - 1;
    this->flagOrbitEnumeratedOnce = true;
    if (this->orbitSize <= this->maxOrbitBufferSize) {
      this->flagOrbitIsBuffered = true;
    } else {
      this->flagOrbitIsBuffered = false;
      this->orbitBuffer.setSize(0);
      this->orbitBuffer.releaseMemory();
    }
    this->checkConsistency();
    return false;
  }
  if (this->orbitBuffer.size < this->maxOrbitBufferSize) {
    this->orbitBuffer.addOnTop(this->theIterator.getCurrentElement());
  }
  return true;
}

void OrbitIteratorRootActionWeylGroupAutomorphisms::initialize() {
  MacroRegisterFunctionWithName("OrbitFDRepIteratorWeylGroupAutomorphisms::initialize");
  this->theIterator.checkInitialization();
  this->currentIndexInBuffer = 0;
  if (this->flagOrbitIsBuffered) {
    return;
  }
  this->theIterator.initialize(
    this->theIterator.theGroupGeneratingElements, this->orbitDefiningElement, this->theIterator.theGroupAction
  );
  if (this->theIterator.theGroupGeneratingElements.size > 0) {
    WeylGroupAutomorphisms& ownerGroup = *this->theIterator.theGroupGeneratingElements[0].owner;
    this->computedSize = ownerGroup.getOrbitSize(this->orbitDefiningElement);
    if (this->computedSize > this->maxOrbitBufferSize) {
      this->maxOrbitBufferSize = 0;
      this->orbitBuffer.setSize(0);
    }
  }
  this->theIterator.checkInitialization();
}

void OrbitIteratorRootActionWeylGroupAutomorphisms::initialize(
  const List<ElementWeylGroupAutomorphisms>& inputGenerators,
  const Vector<Rational>& inputElement
) {
  MacroRegisterFunctionWithName("OrbitFDRepIteratorWeylGroupAutomorphisms::initialize");
  if (
    this->orbitDefiningElement == inputElement
  ) {
    if (this->flagOrbitIsBuffered) {
      this->currentIndexInBuffer = 0;
      return;
    }
  } else {
    this->reset();
  }
  this->orbitDefiningElement = inputElement;
  if (this->maxOrbitBufferSize >= 1) {
    this->orbitBuffer.setSize(0);
    this->orbitBuffer.addOnTop(this->orbitDefiningElement);
  }
  this->theIterator.initialize(inputGenerators, this->orbitDefiningElement, this->theIterator.theGroupAction);
}

OrbitIteratorRootActionWeylGroupAutomorphisms::OrbitIteratorRootActionWeylGroupAutomorphisms() {
  this->theIterator.theGroupAction.name = "WeylGroupAutomorphismRootAction";
  this->theIterator.theGroupAction.actOn = ElementWeylGroupAutomorphisms::actOn;
  this->reset();
}

void OrbitIteratorRootActionWeylGroupAutomorphisms::reset() {
  this->flagOrbitIsBuffered = false;
  this->flagOrbitEnumeratedOnce = false;
  this->orbitSize = - 1;
  this->computedSize = - 1;
  this->currentIndexInBuffer = - 1;
  this->maxOrbitBufferSize = 5000000;
  this->orbitBuffer.setSize(0);
  this->theIterator.resetNoActionChange();
}

std::string OrbitIteratorRootActionWeylGroupAutomorphisms::toString() const {
  std::stringstream out;
  out << " The orbit defining element is: " << this->orbitDefiningElement.toString() << ". ";
  out << "<br>Current element number: " << this->currentIndexInBuffer + 1 << ". ";
  if (this->orbitSize != - 1) {
    out << "<br>The orbit size is: " << this->orbitSize << ".";
  }
  if (this->computedSize != - 1) {
    out << "<br>The computed orbit size is: " << this->computedSize.toString() << ". ";
  }
  if (this->flagOrbitIsBuffered) {
    out << "<br>The orbit is buffered, the orbit elements are: " << this->orbitBuffer.toString();
  } else {
    out << "<br>The orbit is either too large or not yet fully enumerated. "
    << this->theIterator.toStringLayerSize() << ". The current buffer size is: " << this->orbitBuffer.size << ". ";
    if (this->currentIndexInBuffer + 1 > this->orbitBuffer.size) {
      out << "The orbit buffer appears to have exceeded the allowed maximum, "
      << "I am enumerating without storing the elements. ";
    }
  }
  out << "<br> Max buffer size: " << this->maxOrbitBufferSize;
  return out.str();
}

std::string OrbitIteratorRootActionWeylGroupAutomorphisms::toStringSize() const {
  std::stringstream out;
  if (this->orbitSize != - 1) {
    out << this->orbitSize;
    if (!this->flagOrbitIsBuffered) {
      out << "(enumerated but not stored)";
    }
    out << "; ";
  } else {
    out << "n/a; ";
  }
  return out.str();
}

void SemisimpleSubalgebras::getHCandidates(
  Vectors<Rational>& outputHCandidatesScaledToActByTwo,
  CandidateSemisimpleSubalgebra& newCandidate,
  DynkinType& currentType,
  List<int>& currentRootInjection
) {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::getHCandidates");
  ProgressReport theReport1;
  ProgressReport theReport2;
  ProgressReport theReport3;
  int baseRank = currentType.getRank() - 1;
  DynkinSimpleType theSmallType = currentType.getSmallestSimpleType();
  if (theReport1.tickAndWantReport()) {
    std::stringstream reportStream;
    reportStream << "the latest root of the candidate simple component " << theSmallType.toString();
    theReport1.report(reportStream.str());
  }
  int indexNewRooT = *currentRootInjection.lastObject();
  int indexNewRootInSmallType = indexNewRooT - currentType.getRank() + theSmallType.theRank;
  Rational desiredHScaledToActByTwoLengthSquared = theSmallType.CartanSymmetricInverseScale * 4 /
  theSmallType.getDefaultRootLengthSquared(indexNewRootInSmallType);
  outputHCandidatesScaledToActByTwo.setSize(0);
  for (int j = 0; j < this->theSl2s.size; j ++) {
    if (theReport2.tickAndWantReport()) {
      std::stringstream reportStreamX;
      reportStreamX << "Trying to realize via orbit number " << j + 1 << ".";
      if (this->theSl2s[j].LengthHsquared != desiredHScaledToActByTwoLengthSquared) {
        reportStreamX << " The h element "
        << this->theSl2s[j].theH.getCartanPart().toString() << " of length "
        << this->theOrbitHelementLengths[j].toString()
        << " generating orbit number " << j + 1 << " out of "
        << this->theSl2s.size << " does not have the required length of "
        << desiredHScaledToActByTwoLengthSquared.toString();
      }
      theReport2.report(reportStreamX.str());
    }
    if (this->theSl2s[j].LengthHsquared != desiredHScaledToActByTwoLengthSquared) {
      continue;
    }
    if (baseRank == 0) {
      outputHCandidatesScaledToActByTwo.addOnTop(this->theSl2s[j].theH.getCartanPart());
      // Orbit of not generated because that is the very first H element selected.
      continue;
    }
    Vector<Rational> currentCandidate;
    OrbitIteratorRootActionWeylGroupAutomorphisms& currentOrbit = this->orbits[j];
    currentOrbit.initialize();
    do {
      currentCandidate = currentOrbit.getCurrentElement();
      if (newCandidate.isGoodHNewActingByTwo(currentCandidate, currentRootInjection)) {
        if (theReport3.tickAndWantReport()) {
          std::stringstream out2, out3;
          out3 << "So far, found " << outputHCandidatesScaledToActByTwo.size + 1 << " good candidates. ";
          theReport2.report(out3.str());
          out2 << "sl(2) orbit " << j + 1 << ". " << currentOrbit.toString();
          out2 << "Current element has desired scalar products. ";
          theReport3.report(out2.str());
        }
        outputHCandidatesScaledToActByTwo.addOnTop(currentOrbit.getCurrentElement());
      } else {
        if (theReport3.tickAndWantReport()) {
          std::stringstream out2;
          out2 << "sl(2) orbit " << j + 1 << ". " << currentOrbit.toString()
          << "\n<br>Current element is not a valid candidate. ";
          theReport3.report(out2.str());
        }
      }
    } while (currentOrbit.incrementReturnFalseIfPastLast());
    if (outputHCandidatesScaledToActByTwo.size == 0) {
      std::stringstream out2;
      out2 << "Sl(2) orbit " << j + 1 << ": extension to " << currentType.toString()
      << " not possible because there were no h candidates.";
      theReport1.report(out2.str());
    }
  }
}

const CandidateSemisimpleSubalgebra& SemisimpleSubalgebras::baseSubalgebra() {
  if (
    !this->currentSubalgebraChain.lastObject()->weylNonEmbedded->theDynkinType.isEqualToZero() &&
    this->currentSubalgebraChain.lastObject()->indexInOwner == - 1
  ) {
    global.fatal << "Base subalgebra has index in owner equal to - 1 yet "
    << "is of non-zero type: "
    << this->currentSubalgebraChain.lastObject()->weylNonEmbedded->theDynkinType.toString() << ". "
    << global.fatal;
  }
  return * this->currentSubalgebraChain.lastObject();
}

bool SemisimpleSubalgebras::removeLastSubalgebra() {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::removeLastSubalgebra");
  this->currentSubalgebraChain.setSize(this->currentSubalgebraChain.size- 1);
  this->currentPossibleLargerDynkinTypes.setSize(this->currentSubalgebraChain.size);
  this->currentNumLargerTypesExplored.setSize(this->currentSubalgebraChain.size);
  this->currentNumHcandidatesExplored.setSize(this->currentSubalgebraChain.size);
  this->currentHCandidatesScaledToActByTwo.setSize(this->currentSubalgebraChain.size);
  this->currentRootInjections.setSize(this->currentSubalgebraChain.size);
  return this->currentSubalgebraChain.size > 0;
}

bool SemisimpleSubalgebras::getCentralizerTypeIfComputableAndKnown(const DynkinType& input, DynkinType& output) {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::getCentralizerTypeIfComputableAndKnown");
  // This function is rudimentary and fails to return a good result in many cases when
  // a result is actually computable. This needs to be improved.
  if (!input.isSimple()) {
    return false;
  }
  int theIndex = - 1;
  for (int i = 0; i < this->theSl2s.theRootSAs.theSubalgebras.size; i ++) {
    if (this->theSl2s.theRootSAs.theSubalgebras[i].theDynkinType == input) {
      if (theIndex != - 1) {
        theIndex = - 1;
        break;
      }
      theIndex = i;
    }
  }
  if (theIndex == - 1) {
    return false;
  }
  output = this->theSl2s.theRootSAs.theSubalgebras[theIndex].theCentralizerDynkinType;
  return true;
}

void DynkinType::getDynkinIndicesSl2SubalgebrasSimpleType(
  const DynkinSimpleType& theType,
  List<List<Rational> >& precomputedDynkinIndicesSl2subalgebrasSimpleTypes,
  HashedList<DynkinSimpleType>& dynkinSimpleTypesWithComputedSl2Subalgebras,
  HashedList<Rational>& outputDynkinIndices
) {
  MacroRegisterFunctionWithName("DynkinType::getDynkinIndicesSl2SubalgebrasSimpleType");
  DynkinSimpleType theTypeDefaultScale(theType.theLetter, theType.theRank, 1);
  if (!dynkinSimpleTypesWithComputedSl2Subalgebras.contains(theTypeDefaultScale)) {
    HashedList<Rational> outputIndicesDefaultScale;
    SemisimpleLieAlgebra simpleAlgebra;
    SlTwoSubalgebras theSl2s;
    simpleAlgebra.theWeyl.makeArbitrarySimple(theTypeDefaultScale.theLetter, theTypeDefaultScale.theRank);
    simpleAlgebra.computeChevalleyConstants();
    simpleAlgebra.FindSl2Subalgebras(simpleAlgebra, theSl2s);
    dynkinSimpleTypesWithComputedSl2Subalgebras.addOnTop(theTypeDefaultScale);
    outputIndicesDefaultScale.setExpectedSize(theSl2s.size);
    for (int i = 0; i < theSl2s.size; i ++) {
      outputIndicesDefaultScale.addOnTopNoRepetition(theSl2s[i].LengthHsquared / 2);
    }
    precomputedDynkinIndicesSl2subalgebrasSimpleTypes.addOnTop(outputIndicesDefaultScale);
  }
  List<Rational>& outputIndicesDefaultScale = precomputedDynkinIndicesSl2subalgebrasSimpleTypes[
    dynkinSimpleTypesWithComputedSl2Subalgebras.getIndex(theTypeDefaultScale)
  ];
  outputDynkinIndices.setExpectedSize(outputIndicesDefaultScale.size);
  outputDynkinIndices.clear();
  for (int i = 0; i <outputIndicesDefaultScale.size; i ++) {
    outputDynkinIndices.addOnTop(outputIndicesDefaultScale[i] * theType.CartanSymmetricInverseScale);
  }
}

void DynkinType::getDynkinIndicesSl2Subalgebras(
  List<List<Rational> >& precomputedDynkinIndicesSl2subalgebrasSimpleTypes,
  HashedList<DynkinSimpleType>& dynkinSimpleTypesWithComputedSl2Subalgebras,
  HashedList<Rational>& outputDynkinIndices
) {
  MacroRegisterFunctionWithName("DynkinType::getDynkinIndicesSl2Subalgebras");
  List<DynkinSimpleType> theTypes;
  this->getTypesWithMults(theTypes);
  List<List<Rational> > DynkinIndicesPerType;
  HashedList<Rational> bufferIndices;
  for (int i = 0; i < theTypes.size; i ++) {
    this->getDynkinIndicesSl2SubalgebrasSimpleType(
      theTypes[i],
      precomputedDynkinIndicesSl2subalgebrasSimpleTypes,
      dynkinSimpleTypesWithComputedSl2Subalgebras,
      bufferIndices
    );
    DynkinIndicesPerType.setSize(DynkinIndicesPerType.size + 1);
    DynkinIndicesPerType.lastObject()->addOnTop(0);
    DynkinIndicesPerType.lastObject()->addListOnTop(bufferIndices);
  }
  SelectionWithDifferentMaxMultiplicities dynkinIndexSelector;
  List<int> theMults;
  theMults.setSize(theTypes.size);
  for (int i = 0; i < theMults.size; i ++) {
    theMults[i] = DynkinIndicesPerType[i].size - 1;
  }
  dynkinIndexSelector.initFromInts(theMults);
  outputDynkinIndices.clear();
  while (dynkinIndexSelector.incrementReturnFalseIfPastLast()) {
    Rational currentIndex = 0;
    for (int i = 0; i < dynkinIndexSelector.multiplicities.size; i ++) {
      currentIndex += DynkinIndicesPerType[i][dynkinIndexSelector.multiplicities[i]];
    }
    outputDynkinIndices.addOnTopNoRepetition(currentIndex);
  }
}

bool SemisimpleSubalgebras::centralizersComputedToHaveUnsuitableNilpotentOrbits() {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::centralizersComputedToHaveUnsuitableNilpotentOrbits");
  int stackIndex = this->currentSubalgebraChain.size - 1;
  int typeIndex = this->currentNumLargerTypesExplored[stackIndex];
  DynkinType& currentType = this->currentPossibleLargerDynkinTypes[stackIndex][typeIndex];
  SelectionWithDifferentMaxMultiplicities simpleSummandSelection;
  List<int> theMults;
  theMults.setSize(currentType.size());
  for (int i = 0; i < currentType.size(); i ++) {
    if (!currentType.coefficients[i].isSmallInteger(& theMults[i])) {
      global.fatal << "This is not supposed to happen: "
      << "Dynkin type with multiplicity that doesn't fit in a small int. "
      << global.fatal;
    }
  }
  simpleSummandSelection.initFromInts(theMults);
  DynkinType currentComplementSummand, centralizerOfComplementOfCurrentSummand, currentSummand;
  HashedList<Rational> theDynkinIndicesCurrentSummand, theDynkinIndicesCentralizerComplementCurrentSummand;
  while (simpleSummandSelection.incrementReturnFalseIfPastLast()) {
    currentComplementSummand.makeZero();
    for (int i = 0; i <simpleSummandSelection.multiplicities.size; i ++) {
      currentComplementSummand.addMonomial(currentType[i], simpleSummandSelection.multiplicities[i]);
    }
    if (this->getCentralizerTypeIfComputableAndKnown(currentComplementSummand, centralizerOfComplementOfCurrentSummand)) {
      currentSummand = currentType - currentComplementSummand;
      currentSummand.getDynkinIndicesSl2Subalgebras(
        this->cachedDynkinIndicesSl2subalgebrasSimpleTypes,
        this->cachedDynkinSimpleTypesWithComputedSl2Subalgebras,
        theDynkinIndicesCurrentSummand
      );
      centralizerOfComplementOfCurrentSummand.getDynkinIndicesSl2Subalgebras(
        this->cachedDynkinIndicesSl2subalgebrasSimpleTypes,
        this->cachedDynkinSimpleTypesWithComputedSl2Subalgebras,
        theDynkinIndicesCentralizerComplementCurrentSummand
      );
      if (!theDynkinIndicesCentralizerComplementCurrentSummand.contains(theDynkinIndicesCurrentSummand)) {
        std::stringstream reportStream;
        reportStream << "<hr>"
        << "I have rejected type "
        << currentType.toString() << " as non-realizable for the following reasons. "
        << "I computed that the type's summand " << currentSummand.toString()
        << " has complement summand " << currentComplementSummand.toString() << ". "
        << "Then I computed the latter complement summand has centralizer "
        << centralizerOfComplementOfCurrentSummand.toString() << ". "
        << "Then I computed the absolute Dynkin indices of "
        << "the centralizer's sl(2)-subalgebras, namely:<br> "
        << theDynkinIndicesCentralizerComplementCurrentSummand.toStringCommaDelimited()
        << ". If the type was realizable, those would have to contain "
        << "the absolute Dynkin indices of sl(2) subalgebras of the original summand. "
        << "However, that is not the case. "
        << "I can therefore conclude that the Dynkin type " << currentType.toString()
        << " is not realizable. "
        << "The absolute Dynkin indices of the sl(2) subalgebras of "
        << "the original summand I computed to be:<br> "
        << theDynkinIndicesCurrentSummand.toStringCommaDelimited() << ". ";
        this->comments += reportStream.str();
        std::fstream theLogFile;
        if (!FileOperations::openFileCreateIfNotPresentVirtual(
          theLogFile, this->owner->toStringVirtualFolderName() + this->fileNameToLogComments, true, false, false
        )) {
          global.fatal << "Failed to open log file: " << this->fileNameToLogComments << ". This is not fatal but "
          << " I am crashing to let you know. " << global.fatal;
        }
        theLogFile << reportStream.str();
        global.comments << reportStream.str();
        return true;
      }
    }
  }
  return false;
}

bool CandidateSemisimpleSubalgebra::computeCentralizerTypeFailureAllowed() {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::computeCentralizerTypeFailureAllowed");
  this->checkFullInitialization();
  if (this->getRank() != 1) {
    return false;
  }
  Vector<Rational> theH = this->theHsScaledToActByTwo[0];
  int indexSl2 = - 1;
  bool mustBeTrue =  this->owner->theSl2s.containsSl2WithGivenH(theH, &indexSl2);
  if (!mustBeTrue) {
    global.fatal << "Something went very wrong: the semisimple "
    << "subalgebra is of rank 1, hence an sl(2) subalgebra, yet "
    << "I can't find its H element in the list of sl(2) subalgebras. " << global.fatal;
  }
  const SlTwoSubalgebra& theSl2 = this->owner->theSl2s[indexSl2];
  if (!theSl2.flagCentralizerTypeComputed) {
    return false;
  }
  this->theCentralizerType = theSl2.CentralizerTypeIfKnown;
  this->flagCentralizerTypeIsComputed = true;
  return true;
}

bool SemisimpleSubalgebras::centralizerOfBaseComputedToHaveUnsuitableNilpotentOrbits() {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::centralizerOfBaseComputedToHaveUnsuitableNilpotentOrbits");
  if (!this->baseSubalgebra().flagCentralizerTypeIsComputed) {
    return false;
  }
  int stackIndex = this->currentSubalgebraChain.size - 1;
  int typeIndex = this->currentNumLargerTypesExplored[stackIndex];
  DynkinType& currentType = this->currentPossibleLargerDynkinTypes[stackIndex][typeIndex];
  DynkinType complementNewSummandType = this->baseSubalgebra().weylNonEmbedded->theDynkinType;
  DynkinType newSummandType = currentType-complementNewSummandType;
  DynkinType centralizerComplementNewSummandType = this->baseSubalgebra().theCentralizerType;
  if (newSummandType.size() != 1) {
    return false;
  }
  HashedList<Rational> theDynkinIndicesNewSummand, DynkinIndicesTheyGotToFitIn;
  centralizerComplementNewSummandType.getDynkinIndicesSl2Subalgebras(
    this->cachedDynkinIndicesSl2subalgebrasSimpleTypes,
    this->cachedDynkinSimpleTypesWithComputedSl2Subalgebras,
    DynkinIndicesTheyGotToFitIn
  );
  newSummandType.getDynkinIndicesSl2Subalgebras(
    this->cachedDynkinIndicesSl2subalgebrasSimpleTypes,
    this->cachedDynkinSimpleTypesWithComputedSl2Subalgebras,
    theDynkinIndicesNewSummand
  );
  if (DynkinIndicesTheyGotToFitIn.contains(theDynkinIndicesNewSummand)) {
    return false;
  }
  std::fstream theLogFile;
  if (!FileOperations::openFileCreateIfNotPresentVirtual(
    theLogFile, this->owner->toStringVirtualFolderName() + this->fileNameToLogComments, true, false, false
  )) {
    global.fatal << "Failed to open log file: " << this->fileNameToLogComments << ". This is not fatal but "
    << " I am crashing to let you know. " << global.fatal;
  }
  std::stringstream reportStream;
  reportStream << "<hr>"
  << "I have rejected type " << currentType.toString() << " as non-realizable for the following reasons. "
  << "The type's summand " << newSummandType.toString()
  << " has complement summand " << complementNewSummandType.toString() << ". "
  << "I computed the latter complement summand has centralizer "
  << centralizerComplementNewSummandType.toString() << ". "
  << "Then I computed the absolute Dynkin indices of the centralizer's sl(2)-subalgebras, namely:<br> "
  << DynkinIndicesTheyGotToFitIn.toStringCommaDelimited()
  << ". If the type was realizable, those would have to contain "
  << "the absolute Dynkin indices of sl(2) subalgebras "
  << "of the original summand. However, that is not the case. "
  << "I can therefore conclude that the Dynkin type " << currentType.toString() << " is not realizable. "
  << "The absolute Dynkin indices of the sl(2) subalgebras of the original summand I computed to be:<br> "
  << theDynkinIndicesNewSummand.toStringCommaDelimited() << ". ";
  theLogFile << reportStream.str();
  global.comments << reportStream.str();
  return true;
}

bool SemisimpleSubalgebras::combinatorialCriteriaAllowRealization() {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::combinatorialCriteriaAllowRealization");
  int stackIndex = this->currentSubalgebraChain.size - 1;
  int typeIndex = this->currentNumLargerTypesExplored[stackIndex];
  if (typeIndex>= this->currentPossibleLargerDynkinTypes[stackIndex].size) {
    return false;
  }
  DynkinType& currentType = this->currentPossibleLargerDynkinTypes[stackIndex][typeIndex];
  Rational candidatePrincipalLength = currentType.getPrincipalSlTwoCartanSymmetricInverseScale() * 2;
  if (!this->theOrbitHelementLengths.contains(candidatePrincipalLength)) {
    return false;
  }
  if (this->centralizersComputedToHaveUnsuitableNilpotentOrbits()) {
    return false;
  }
  if (this->centralizerOfBaseComputedToHaveUnsuitableNilpotentOrbits()) {
    return false;
  }
  return true;
}

bool SemisimpleSubalgebras::computeCurrentHCandidates() {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::computeCurrentHCandidates");
  int stackIndex = this->currentSubalgebraChain.size - 1;
  int typeIndex = this->currentNumLargerTypesExplored[stackIndex];
  this->currentHCandidatesScaledToActByTwo[stackIndex].setSize(0);
  if (typeIndex >= this->currentPossibleLargerDynkinTypes[stackIndex].size) {
    return true;
  }
  if (this->currentPossibleLargerDynkinTypes[stackIndex][typeIndex].getRootSystemSize() > this->owner->getNumberOfPositiveRoots() * 2) {
    return true;
  }
  if (!this->targetDynkinType.isEqualToZero()) {
    if (!this->currentPossibleLargerDynkinTypes[stackIndex][typeIndex].canBeExtendedParabolicallyOrIsEqualTo(
      this->targetDynkinType
    )) {
      return true;
    }
  }
  if (!this->combinatorialCriteriaAllowRealization()) {
    return true;
  }
  ProgressReport theReport0, theReport1;
  if (theReport0.tickAndWantReport()) {
    std::stringstream reportStream;
    reportStream << " Finding h-candidates for extension of "
    << this->baseSubalgebra().weylNonEmbedded->theDynkinType.toString()
    << " to " << this->currentPossibleLargerDynkinTypes[stackIndex][typeIndex].toString()
    << ": " << typeIndex + 1 << " out of "
    << this->currentPossibleLargerDynkinTypes[stackIndex].size << " possibilities. ";
    theReport0.report(reportStream.str());
  }
  CandidateSemisimpleSubalgebra newCandidate;
  newCandidate.owner = this;
  if (this->baseSubalgebra().getRank() != 0) {
    Vector<Rational> weightHElementWeAreLookingFor =
    this->getHighestWeightFundNewComponentFromImagesOldSimpleRootsAndNewRoot(
      this->currentPossibleLargerDynkinTypes[stackIndex][typeIndex],
      this->currentRootInjections[stackIndex][typeIndex],
      newCandidate
    );
    List<int> indicesModulesNewComponentExtensionMod;
    indicesModulesNewComponentExtensionMod.reserve(this->owner->theWeyl.rootSystem.size);
    indicesModulesNewComponentExtensionMod.setSize(0);
    for (int j = 0; j < this->baseSubalgebra().HighestWeightsNONPrimal.size; j ++) {
      if (this->baseSubalgebra().HighestWeightsNONPrimal[j] == weightHElementWeAreLookingFor) {
        indicesModulesNewComponentExtensionMod.addOnTop(j);
      }
    }
    if (indicesModulesNewComponentExtensionMod.size == 0) {
      if (theReport1.tickAndWantReport()) {
        std::stringstream reportStream;
        reportStream << " Extension " << typeIndex + 1
        << " out of " << this->currentPossibleLargerDynkinTypes[stackIndex].size
        << ", type  " << this->currentPossibleLargerDynkinTypes[stackIndex][typeIndex].toString()
        << " cannot be realized: no appropriate module: desired weight of h element is: "
        << weightHElementWeAreLookingFor.toString()
        << " but the highest weights of the base candidate are: "
        << this->baseSubalgebra().HighestWeightsNONPrimal.toString();
        theReport1.report(reportStream.str());
      }
      return true;
    }
  }

  newCandidate.setUpInjectionHs(
    this->baseSubalgebra(),
    this->currentPossibleLargerDynkinTypes[stackIndex][typeIndex],
    this->currentRootInjections[stackIndex][typeIndex]
  );
  this->getHCandidates(
    this->currentHCandidatesScaledToActByTwo[stackIndex],
    newCandidate, this->currentPossibleLargerDynkinTypes[stackIndex][typeIndex],
    this->currentRootInjections[stackIndex][typeIndex]
  );
  return true;
}

void SemisimpleSubalgebras::addSubalgebraIfNewSetToStackTop(CandidateSemisimpleSubalgebra& input) {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::addSubalgebraIfNewSetToStackTop");
  this->checkConsistencyHs();
  if (this->theSubalgebras.contains(input.theHs)) {
    input = this->theSubalgebras.theValues[this->theSubalgebras.getIndex(input.theHs)];
    if (!input.weylNonEmbedded->theDynkinType.isEqualToZero() && input.indexInOwner == - 1) {
      global.fatal << "This is not supposed to happen: subalgebra of type "
      << input.weylNonEmbedded->theDynkinType.toString() << " has index in owner - 1. " << global.fatal;
    }
  } else {
    input.indexInOwner = this->theSubalgebras.theValues.size;
    this->theSubalgebras.setKeyValue(input.theHs, input);
  }
  input.computeAndVerifyFromGeneratorsAndHs();
  this->addSubalgebraToStack(input, 0, 0);
}

void SemisimpleSubalgebras::addSubalgebraToStack(
  CandidateSemisimpleSubalgebra& input, int inputNumLargerTypesExplored, int inputNumHcandidatesExplored
) {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::addSubalgebraToStack");
  if (input.indexInOwner == - 1 && !input.weylNonEmbedded->theDynkinType.isEqualToZero()) {
    global.fatal << "Adding to stack subalgebra with indexInOwner equal to - 1 is forbidden. " << global.fatal;
  }
  if (input.theHs.size != input.theHsScaledToActByTwoInOrderOfCreation.size) {
    global.fatal << "In order to add subalgebra "
    << input.weylNonEmbedded->theDynkinType.toString()
    << " to the stack I need to know the order of creation of its h-vectors. "
    << global.fatal;
  }
  input.computeCentralizerTypeFailureAllowed();//<- trying to compute the centralizer of a subalgebra on the stack.
  this->currentSubalgebraChain.addOnTop(input);
  this->currentPossibleLargerDynkinTypes.setSize(this->currentSubalgebraChain.size);
  this->currentRootInjections.setSize(this->currentSubalgebraChain.size);

  this->growDynkinType(
    input.weylNonEmbedded->theDynkinType,
    *this->currentPossibleLargerDynkinTypes.lastObject(),
    this->currentRootInjections.lastObject()
  );
  ///////////
  this->currentNumLargerTypesExplored.addOnTop(inputNumLargerTypesExplored);
  // global.Comments << "<hr>" << this->currentPossibleLargerDynkinTypes.lastObject()->size
  // << " possible extensions of " << input.weylNonEmbedded->theDynkinType.toString() << ": ";
  // for (int i = 0; i < this->currentPossibleLargerDynkinTypes.lastObject()->size; i ++)
  //   global.Comments << (*this->currentPossibleLargerDynkinTypes.lastObject())[i].toString() << ", ";
  ///////////
  this->currentHCandidatesScaledToActByTwo.setSize(this->currentSubalgebraChain.size);
  this->currentNumHcandidatesExplored.addOnTop(inputNumHcandidatesExplored);

  this->computeCurrentHCandidates();
}

std::string SemisimpleSubalgebras::ToStringCurrentChain(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::ToStringCurrentChain");
  (void) theFormat;//avoid unused parameter warning in a portable way
  std::stringstream out;
  out << "<br>Current subalgebra chain length: " << this->currentSubalgebraChain.size << "<br>";
  for (int i = 0; i < this->currentSubalgebraChain.size; i ++) {
    out << this->currentSubalgebraChain[i].weylNonEmbedded->theDynkinType.toString();
    if (i != this->currentSubalgebraChain.size - 1) {
      out << "&lt;";
    }
  }
  return out.str();
}

std::string SemisimpleSubalgebras::toStringState(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::toStringState");
  std::stringstream out;
  out << "Subalgebras found so far: " << this->theSubalgebras.theValues.size << "<br>Orbit sizes: ";
  for (int i = 0; i < this->orbits.size; i ++) {
    out << "A^" << (this->theOrbitHelementLengths[i] / 2).toString() << "_1: "
    << this->orbits[i].toStringSize();
  }
  out << this->ToStringCurrentChain(theFormat);
  out << "<hr>";
  for (int i = 0; i < this->currentSubalgebraChain.size; i ++) {
    out << "<br>Extensions of " << this->currentSubalgebraChain[i].weylNonEmbedded->theDynkinType.toString()
    << ": &nbsp;&nbsp;&nbsp;&nbsp;";
    for (int j = 0; j < this->currentPossibleLargerDynkinTypes[i].size; j ++) {
      if (j == this->currentNumLargerTypesExplored[i]) {
        out << "<b>";
      }
      out << this->currentPossibleLargerDynkinTypes[i][j].toString();
      if (j == this->currentNumLargerTypesExplored[i]) {
        out << "</b>";
      }
      if (j != this->currentPossibleLargerDynkinTypes[i].size - 1) {
        out << ", ";
      }
    }
    out << "<br> " << this->currentNumLargerTypesExplored[i] << " out of "
    << this->currentPossibleLargerDynkinTypes[i].size << " types explored, current type: ";
    if (this->currentNumLargerTypesExplored[i] < this->currentPossibleLargerDynkinTypes[i].size) {
      out << "<b>" << this->currentPossibleLargerDynkinTypes[i][this->currentNumLargerTypesExplored[i]] << "</b>";
    } else {
      out << "<b>All types explored.</b>";
    }
    out << "<br>Explored " << this->currentNumHcandidatesExplored[i] << " out of "
    << this->currentHCandidatesScaledToActByTwo[i].size << " h-candidates. ";
  }
  return out.str();
}

std::string SemisimpleSubalgebras::ToStringProgressReport(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::ToStringProgressReport");
  std::stringstream out;
  out << this->toStringState(theFormat);
  if (this->ToStringExpressionString != nullptr) {
    out << "\n<hr>\n"
    << "LoadSemisimpleSubalgebras {}" << this->ToStringExpressionString(*this);
  }
  out << "\n\n<hr>";
  return out.str();
}

bool SemisimpleSubalgebras::incrementReturnFalseIfPastLast() {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::incrementReturnFalseIfPastLast");
  this->checkConsistency();
  if (this->currentSubalgebraChain.size == 0) {
    return false;
  }
  if (this->baseSubalgebra().theHs.size != this->baseSubalgebra().theHsScaledToActByTwoInOrderOfCreation.size) {
    global.fatal << " The order of creation of the elements of the Cartan is missing in base subalgebra of type "
    << this->baseSubalgebra().weylNonEmbedded->theDynkinType.toString()
    << ", and this order is needed to construct extensions. " << global.fatal;
  }
  ProgressReport theReport1;
  if (this->baseSubalgebra().getRank() >= this->owner->getRank()) {
    return this->removeLastSubalgebra();
  }
  int stackIndex = this->currentSubalgebraChain.size - 1;
  if (this->currentNumHcandidatesExplored[stackIndex] >= this->currentHCandidatesScaledToActByTwo[stackIndex].size) {
    this->currentNumLargerTypesExplored[stackIndex] ++;
    if (this->currentNumLargerTypesExplored[stackIndex] >= this->currentPossibleLargerDynkinTypes[stackIndex].size) {
      return this->removeLastSubalgebra();
    }
    this->currentNumHcandidatesExplored[stackIndex] = 0;
    return this->computeCurrentHCandidates();
  }
  int typeIndex = this->currentNumLargerTypesExplored[stackIndex];
  int hIndex = this->currentNumHcandidatesExplored[stackIndex];
  CandidateSemisimpleSubalgebra newCandidate;
  newCandidate.owner = this;

  bool newSubalgebraCreated =
  newCandidate.createAndAddExtendBaseSubalgebra(
    this->baseSubalgebra(),
    this->currentHCandidatesScaledToActByTwo[stackIndex][hIndex],
    this->currentPossibleLargerDynkinTypes[stackIndex][typeIndex],
    this->currentRootInjections[stackIndex][typeIndex]
  );
  this->checkConsistency();
  if (newSubalgebraCreated) {
    if (!newCandidate.flagSystemSolved && !newCandidate.flagSystemProvedToHaveNoSolution) {
      this->flagRealizedAllCandidates = false;
      std::stringstream out;
      out << "<hr>Failed to realize type " << newCandidate.weylNonEmbedded->theDynkinType.toString()
      << " because I couldn't handle the polynomial system. "
      << "one poly system that governs the embedding follows.<br>" << newCandidate.toStringSystemPart2() << "<hr>";
      this->comments = out.str();
      return true;
    }
  }
  this->currentNumHcandidatesExplored[stackIndex] ++;
  if (newSubalgebraCreated) {
    this->addSubalgebraIfNewSetToStackTop(newCandidate);
  } else {
    std::stringstream reportstream;
    reportstream << "h element " << hIndex + 1 << " out of " << this->currentHCandidatesScaledToActByTwo[stackIndex][hIndex].size
    << ": did not succeed extending. ";
    theReport1.report(reportstream.str());
  }
  return true;
}

/*
void SemisimpleSubalgebras::RegisterPossibleCandidate(CandidateSemisimpleSubalgebra& input) {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::RegisterPossibleCandidate");
  this->checkConsistency();
  if (input.theSubalgebraNonEmbeddedDefaultScale == 0)
    global.fatal << "Non-initialized non-default scale subalgebra in candidate subalgebra. " << global.fatal;
  this->checkInitialization();
}*/

bool DynkinType::isTypeAOne() const {
  if (this->size() != 1) {
    return false;
  }
  if (this->coefficients[0] != 1) {
    return false;
  }
  if ((*this)[0].theRank != 1) {
    return false;
  }
  return (*this)[0].theLetter == 'A';
}

void DynkinType::getDynkinTypeWithDefaultScales(DynkinType& output) const {
  if (&output == this) {
    DynkinType thisCopy =*this;
    thisCopy.getDynkinTypeWithDefaultScales(output);
    return;
  }
  output.makeZero();
  DynkinSimpleType tempType;
  for (int i = 0; i < this->size(); i ++) {
    tempType.makeArbitrary((*this)[i].theLetter, (*this)[i].theRank, 1);
    output.addMonomial(tempType, this->coefficients[i]);
  }
}

void DynkinSimpleType::getAutomorphismActingOnVectorColumn(MatrixTensor<Rational>& output, int AutoIndex) const {
  MacroRegisterFunctionWithName("DynkinSimpleType::getAutomorphismActingOnVectorColumn");
  if (
    AutoIndex == 0 ||
    this->theLetter == 'B' ||
    this->theLetter == 'C' ||
    this->theLetter == 'G' ||
    this->theLetter == 'F' ||
    this->theRank == 1 ||
    (this->theLetter == 'E' && this->theRank != 6)
  ) {
    output.makeIdentity(this->theRank);
    return;
  }
  output.makeZero();
  if (this->theLetter == 'A' && this->theRank != 1) {
    for (int i = 0; i < this->theRank; i ++) {
      output.addMonomial(MonomialMatrix(i, this->theRank - i - 1), 1);
    }
  }
  if (this->theLetter == 'D') {
    if (this->theRank == 4) {
      //Here be D_4 triality.
      //The automorphism group of Dynkin Diagram D_4 is S3
      //The triple node is always fixed:
      output.addMonomial(MonomialMatrix(1, 1), 1);
      if (AutoIndex == 1) {
        //Permutation (12), AutoIndex = 1
        output.addMonomial(MonomialMatrix(0, 2), 1);
        output.addMonomial(MonomialMatrix(2, 0), 1);
        output.addMonomial(MonomialMatrix(3, 3), 1);
      } else if (AutoIndex == 2) {
        //Permutation (23), AutoIndex =2
        output.addMonomial(MonomialMatrix(0, 0), 1);
        output.addMonomial(MonomialMatrix(2, 3), 1);
        output.addMonomial(MonomialMatrix(3, 2), 1);
      } else if (AutoIndex == 3) {
        //Permutation (12)(23)=(123), AutoIndex =3
        output.addMonomial(MonomialMatrix(0, 2), 1);
        output.addMonomial(MonomialMatrix(2, 3), 1);
        output.addMonomial(MonomialMatrix(3, 0), 1);
      } else if (AutoIndex == 4) {
        //Permutation (23)(12)=(132), AutoIndex =4
        output.addMonomial(MonomialMatrix(0, 3), 1);
        output.addMonomial(MonomialMatrix(2, 0), 1);
        output.addMonomial(MonomialMatrix(3, 2), 1);
      } else if (AutoIndex == 5) {
        //Permutation (12)(23)(12)=(13), AutoIndex =5
        output.addMonomial(MonomialMatrix(0, 3), 1);
        output.addMonomial(MonomialMatrix(2, 2), 1);
        output.addMonomial(MonomialMatrix(3, 0), 1);
      } else {
        global.fatal << "Requesting triality "
        << "automorphism with index not in the range 0-5. " << global.fatal;
      }
    } else {
      for (int i = 0; i < this->theRank - 2; i ++) {
        output.addMonomial(MonomialMatrix(i, i), 1);
      }
      output.addMonomial(MonomialMatrix(this->theRank - 1, this->theRank - 2), 1);
      output.addMonomial(MonomialMatrix(this->theRank - 2, this->theRank - 1), 1);
    }
  }
  if (this->theLetter == 'E' && this->theRank == 6) {
    //We gotta flip the Dynkin diagram of E_6. Note that the extra 1-length sticking root comes second and
    //and the triple node comes fourth.
    //Therefore, we must swap the 1st root with the 6th and the third root
    //with the 5th. Conventions, conventions... no way around 'em!
     output.addMonomial(MonomialMatrix(1, 1), 1);
     output.addMonomial(MonomialMatrix(3, 3), 1);
     output.addMonomial(MonomialMatrix(0, 5), 1);
     output.addMonomial(MonomialMatrix(5, 0), 1);
     output.addMonomial(MonomialMatrix(2, 4), 1);
     output.addMonomial(MonomialMatrix(4, 2), 1);
  }
  Rational tempRat = output.getDeterminant();
  if (tempRat != 1 && tempRat != - 1) {
    FormatExpressions theFormat;
    theFormat.flagUseHTML = false;
    theFormat.flagUseLatex = true;
    global.fatal << "The determinant of the automorphism matrix "
    << "of the Dynkin graph must be +/- 1, it is instead "
    << tempRat.toString() << ". The auto matrix is: "
    << HtmlRoutines::getMathNoDisplay(output.toStringMatrixForm(&theFormat)) << " and the dynkin type is: "
    << this->toString() << "." << global.fatal;
  }
}

DynkinSimpleType DynkinType::getSmallestSimpleType() const {
  if (this->size() == 0) {
    global.fatal << "Asking for the smallest simple type of a 0 dynkin type. " << global.fatal;
  }
  DynkinSimpleType result = (*this)[0];
  for (int i = 1; i < this->size(); i ++) {
    if ((*this)[i] < result) {
      result = (*this)[i];
    }
  }
  return result;
}

DynkinSimpleType DynkinType::getGreatestSimpleType() const {
  if (this->size() == 0) {
    global.fatal << "Asking for the greatest simple type of a 0 dynkin type. " << global.fatal;
  }
  DynkinSimpleType result = (*this)[0];
  for (int i = 1; i < this->size(); i ++) {
    if ((*this)[i] >result) {
      result = (*this)[i];
    }
  }
  return result;
}

Rational DynkinType::getPrincipalSlTwoCartanSymmetricInverseScale() const {
  MacroRegisterFunctionWithName("DynkinType::getPrincipalSlTwoCartanSymmetricInverseScale");
  Rational result = 0;
  for (int i = 0; i < this->size(); i ++) {
    result += this->coefficients[i] * (*this)[i].getPrincipalSlTwoCartanSymmetricInverseScale();
  }
  return result;
}

bool CandidateSemisimpleSubalgebra::checkBasicInitialization() const {
  if (this->flagDeallocated) {
    global.fatal << "Use after free of CandidateSemisimpleSubalgebra. " << global.fatal;
  }
  if (this->owner == nullptr) {
    global.fatal << "Use of non-initialized semisimple subalgebra candidate. " << global.fatal;
  }
  return true;
}

bool CandidateSemisimpleSubalgebra::checkCandidateInitialization() const {
  this->checkBasicInitialization();
  if (this->weylNonEmbedded == nullptr) {
    global.fatal << "Weyl group data not initialized for "
    << "a semisimple subalgebra candidate. " << global.fatal;
  }
  return true;
}

bool CandidateSemisimpleSubalgebra::checkFullInitialization() const {
  this->checkCandidateInitialization();
  if (this->theSubalgebraNonEmbeddedDefaultScale == nullptr) {
    global.fatal << "The semisimple default scale subalgebra is not initialized. "
    << global.fatal;
  }
  return true;
}

WeylGroupAutomorphisms& CandidateSemisimpleSubalgebra::getAmbientWeylAutomorphisms() const {
  this->checkBasicInitialization();
  return this->owner->theSl2s.theRootSAs.theWeylGroupAutomorphisms;
}

WeylGroupData& CandidateSemisimpleSubalgebra::getAmbientWeyl() const {
  this->checkBasicInitialization();
  return this->owner->getSemisimpleOwner().theWeyl;
}

SemisimpleLieAlgebra& CandidateSemisimpleSubalgebra::getAmbientSemisimpleLieAlgebra() const {
  this->checkBasicInitialization();
  return this->owner->getSemisimpleOwner();
}

void CandidateSemisimpleSubalgebra::addHIncomplete(const Vector<Rational>& theH) {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::addHIncomplete");
  this->checkBasicInitialization();
  if (this->cartanSubalgebrasByComponentScaledToActByTwo.size == 1) {
    if (this->cartanSubalgebrasByComponentScaledToActByTwo.lastObject()->size == 0) {
      this->PosRootsPerpendicularPrecedingWeights = this->getAmbientWeyl().rootsOfBorel;
    }
  }
  for (int i = 0; i < this->PosRootsPerpendicularPrecedingWeights.size; i ++) {
    if (this->getAmbientWeyl().rootScalarCartanRoot(this->PosRootsPerpendicularPrecedingWeights[i], theH) != 0) {
      this->PosRootsPerpendicularPrecedingWeights.removeIndexShiftDown(i);
      i --;
    }
  }
  this->cartanSubalgebrasByComponentScaledToActByTwo.lastObject()->addOnTop(theH);
}

bool CandidateSemisimpleSubalgebra::isGoodHNewActingByTwo(
  const Vector<Rational>& HNewActingByTwo, const List<int>& theRootInjections
) const {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::isGoodHnew");
  //check if input weight is maximally dominant:
  Rational theScalarProd;
  int indexHneW = *theRootInjections.lastObject();
  for  (int i = 0; i < this->getAmbientWeyl().rootsOfBorel.size; i ++) {
    Vector<Rational>& currentPosRoot = this->getAmbientWeyl().rootsOfBorel[i];
    bool canBeRaisingReflection = true;
    for (int l = 0; l< this->theHsScaledToActByTwoInOrderOfCreation.size && canBeRaisingReflection; l ++) {
      theScalarProd = this->getAmbientWeyl().rootScalarCartanRoot(
        currentPosRoot, this->theHsScaledToActByTwoInOrderOfCreation[l]
      );
      if (theScalarProd > 0) {
        canBeRaisingReflection = false;
      }
      if (theScalarProd < 0) {
        global.fatal << "While trying to realize type " << this->weylNonEmbedded->theDynkinType.toString()
        << ", the candidate h elements of the semisimple subalgebra are supposed to be maximally dominant, "
        << "however the scalar product of the positive root " << currentPosRoot.toString() << " with the subalgebra root "
        << this->theHsScaledToActByTwoInOrderOfCreation[l].toString()
        << " is negative, while the very same positive root has had zero scalar products with all "
        << " preceding roots. Hnew equals: " << HNewActingByTwo.toString() << " Here are all preceding roots: "
        << this->theHsScaledToActByTwoInOrderOfCreation.toString() << global.fatal;
      }
    }
    if (canBeRaisingReflection) {
      if (this->getAmbientWeyl().rootScalarCartanRoot(currentPosRoot, HNewActingByTwo)<0) {
        return false;
      }
    }
  }
  for (int i = 0; i < this->theHsScaledToActByTwo.size; i ++) {
    if (i != indexHneW) {
      theScalarProd = this->getAmbientWeyl().rootScalarCartanRoot(HNewActingByTwo, this->theHsScaledToActByTwo[i]);
    } else {
      theScalarProd = this->getAmbientWeyl().rootScalarCartanRoot(HNewActingByTwo, HNewActingByTwo);
    }
    if (theScalarProd != this->weylNonEmbedded->coCartanSymmetric(indexHneW, i)) {
      return false;
    }
  }
  return true;
}

template <class Coefficient>
int CharacterSemisimpleLieAlgebraModule<Coefficient>::getIndexExtremeWeightRelativeToWeyl(WeylGroupData& theWeyl) const {
  HashedList<Vector<Coefficient> > weightsSimpleCoords;
  weightsSimpleCoords.setExpectedSize(this->size());
  for (int i = 0; i < this->size(); i ++) {
    weightsSimpleCoords.addOnTop(theWeyl.getSimpleCoordinatesFromFundamental((*this)[i].weightFundamentalCoordS));
  }
  for (int i = 0; i <weightsSimpleCoords.size; i ++) {
    bool isGood = true;
    for (int j = 0; j < theWeyl.rootsOfBorel.size; j ++) {
      if (weightsSimpleCoords.contains((weightsSimpleCoords[i] + theWeyl.rootsOfBorel[j]))) {
        isGood = false;
        break;
      }
    }
    if (isGood) {
      return i;
    }
  }
  return - 1;
}

bool CandidateSemisimpleSubalgebra::isWeightSystemSpaceIndex(int theIndex, const Vector<Rational>& AmbientRootTestedForWeightSpace) {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::isWeightSystemSpaceIndex");
  for (int k = 0; k < this->theHs.size; k ++) {
    Rational desiredScalarProd = this->theSubalgebraNonEmbeddedDefaultScale->theWeyl.cartanSymmetric(theIndex, k);
    Rational actualScalar = this->getAmbientWeyl().rootScalarCartanRoot(this->theHs[k], AmbientRootTestedForWeightSpace);
    if (desiredScalarProd != actualScalar) {
      return false;
    }
  }
  return true;
}

bool CandidateSemisimpleSubalgebra::computeSystem(bool AttemptToChooseCentalizer, bool allowNonPolynomialSystemFailure) {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::computeSystem");
  ChevalleyGenerator currentGen, currentOpGen;
  this->theInvolvedNegGenerators.setSize(this->theHsScaledToActByTwo.size);
  this->theInvolvedPosGenerators.setSize(this->theHsScaledToActByTwo.size);
  for (int i = 0; i < this->theHsScaledToActByTwo.size; i ++) {
    List<ChevalleyGenerator>& currentInvolvedPosGens = this->theInvolvedPosGenerators[i];
    List<ChevalleyGenerator>& currentInvolvedNegGens = this->theInvolvedNegGenerators[i];
    currentInvolvedNegGens.setExpectedSize(this->getAmbientWeyl().getDimension() * 2);
    currentInvolvedPosGens.setExpectedSize(this->getAmbientWeyl().getDimension() * 2);
    currentInvolvedNegGens.setSize(0);
    currentInvolvedPosGens.setSize(0);
    for (int j = 0; j < this->getAmbientWeyl().rootSystem.size; j ++) {
      int indexCurGen = this->getAmbientSemisimpleLieAlgebra().getGeneratorFromRootIndex(j);
      int opIndex = this->getAmbientSemisimpleLieAlgebra().getGeneratorFromRoot(- this->getAmbientWeyl().rootSystem[j]);
      currentGen.makeGenerator(*this->owner->owner, indexCurGen);
      currentOpGen.makeGenerator(*this->owner->owner, opIndex);
      if (this->isWeightSystemSpaceIndex(i, this->getAmbientWeyl().rootSystem[j])) {
        currentInvolvedPosGens.addOnTop(currentGen);
        currentInvolvedNegGens.addOnTop(currentOpGen);
      }
    }
    if (currentInvolvedNegGens.size == 0) {
      if (currentInvolvedPosGens.size != 0) {
        global.fatal << "The number of involved negative generators is different "
        << "from the number of involved positive generators. " << global.fatal;
      }
      this->flagSystemProvedToHaveNoSolution = true;
      return false;
    }
  }
  this->flagUsedInducingSubalgebraRealization = true;
  return this->computeSystemPart2(AttemptToChooseCentalizer, allowNonPolynomialSystemFailure);
}

bool CandidateSemisimpleSubalgebra::checkGeneratorsBracketToHs() {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::checkGeneratorsBracketToHs");
  if (this->theNegGens.size != this->thePosGens.size || this->theNegGens.size != this->weylNonEmbedded->getDimension()) {
    return false;
  }
  ElementSemisimpleLieAlgebra<AlgebraicNumber> goalH, lieBracket;
  for (int i = 0; i < this->theNegGens.size; i ++) {
    goalH.makeCartanGenerator(this->theHsScaledToActByTwo[i], *this->owner->owner);
    this->getAmbientSemisimpleLieAlgebra().lieBracket(this->thePosGens[i], this->theNegGens[i], lieBracket);
    lieBracket -= goalH;
    if (!lieBracket.isEqualToZero()) {
      return false;
    }
  }
  return true;
}

bool CandidateSemisimpleSubalgebra::computeSystemPart2(bool AttemptToChooseCentalizer, bool allowNonPolynomialSystemFailure) {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::computeSystemPart2");
  theSystemToSolve.setSize(0);
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > lieBracketMinusGoalValue, goalValue;
  Vector<Polynomial<Rational> > desiredHpart;
  this->checkFullInitialization();
  const SemisimpleLieAlgebra& nonEmbeddedMe =
  this->owner->theSubalgebrasNonDefaultCartanAndScale.theValues[this->indexNonEmbeddedMeNonStandardCartan];
  this->totalNumUnknownsNoCentralizer = 0;
  if (this->theHs.size == 0) {
    global.fatal << "The number of involved H's cannot be zero. " << global.fatal;
  }
  if (this->theInvolvedNegGenerators.size != this->theHs.size) {
    global.fatal << "The number of involved negative generators: " << this->theInvolvedNegGenerators.size
    << " is not equal to the subalgebra rank: " << this->theHs.size << ". " << global.fatal;
  }
  for (int i = 0; i < this->theInvolvedNegGenerators.size; i ++) {
    this->totalNumUnknownsNoCentralizer += this->theInvolvedNegGenerators[i].size;
  }
  if (this->weylNonEmbedded->rootSystem.size == 0) {
    global.fatal << "The root system of the candidate subalgebra has not been computed " << global.fatal;
  }
  this->totalNumUnknownsNoCentralizer *= 2;
  this->totalNumUnknownsWithCentralizer = this->totalNumUnknownsNoCentralizer;
  this->theUnknownNegGens.setSize(this->theInvolvedNegGenerators.size);
  this->theUnknownPosGens.setSize(this->theInvolvedPosGenerators.size);
  if (!AttemptToChooseCentalizer) {
    this->theUnknownCartanCentralizerBasis.setSize(0);
  } else {
    int rankCentralizer = - 1;
    bool tempB = this->centralizerRank.isSmallInteger(&rankCentralizer);
    if (!tempB || rankCentralizer < 0 || rankCentralizer > this->getAmbientWeyl().getDimension()) {
      if (allowNonPolynomialSystemFailure) {
        return false;
      }
    }
    if (!tempB) {
      global.fatal << "Error: rankCentralizer is not a small integer. Detailed subalgebra printout: "
      << this->toString() << global.fatal;
    }
    if (rankCentralizer < 0) {
      global.fatal << "Centralizer rank extracted as a negative number. The centralizer rank is: "
      << this->centralizerRank.toString() << ". This most probably means the centralizer was not computed correctly. "
      << "Here's a full subalgebra printout" << this->toString()
      << global.fatal;
    }
    if (rankCentralizer>this->getAmbientWeyl().getDimension()) {
      global.fatal << " Currently rankCentralizer is computed to be " << rankCentralizer << " which is greater than the rank "
      << this->getAmbientWeyl().getDimension() << " of the ambient semisimple Lie algebra. Something has gone wrong. "
      << "Here is a detailed printout of the candidate subalgebra. "
      << this->toString() << global.fatal;
    }
    this->totalNumUnknownsWithCentralizer +=rankCentralizer*this->getAmbientWeyl().getDimension() + 1;
    this->theUnknownCartanCentralizerBasis.setSize(rankCentralizer);
  }
  if (this->indexIamInducedFrom == - 1) {
    this->flagUsedInducingSubalgebraRealization = false;
  }
  int indexNewRoot = - 1;
  if (this->flagUsedInducingSubalgebraRealization) {
    indexNewRoot =*this->rootInjectionsFromInducer.lastObject();
  }
  for (int i = 0; i < this->theInvolvedNegGenerators.size; i ++) {
    bool seedsHaveBeenSown = false;
    if (this->flagUsedInducingSubalgebraRealization) {
      CandidateSemisimpleSubalgebra& theInducer = this->owner->theSubalgebras.theValues[this->indexIamInducedFrom];
      if (theInducer.flagSystemSolved && i != indexNewRoot) {
        int preimageIndex = DynkinType::GetIndexPreimageFromRootInjection(i, this->rootInjectionsFromInducer);
        this->theUnknownNegGens[i] = theInducer.theNegGens[preimageIndex];//<-implicit type conversion from base field to polynomial here
        this->theUnknownPosGens[i] = theInducer.thePosGens[preimageIndex];//<-implicit type conversion from base field to polynomial here
        seedsHaveBeenSown = true;
      }
    }
    if (seedsHaveBeenSown) {
      continue;
    }
    this->getGenericNegativeGeneratorLinearCombination(i, this->theUnknownNegGens[i]);
    this->getGenericPositiveGeneratorLinearCombination(i, this->theUnknownPosGens[i]);
  }
  if (this->theUnknownCartanCentralizerBasis.size > 0) {
    Matrix<Polynomial<AlgebraicNumber> > theCentralizerCartanVars;
    Vectors<Polynomial<AlgebraicNumber> > theCentralizerCartanElts;
    theCentralizerCartanElts.setSize(this->theUnknownCartanCentralizerBasis.size);
    for (int i = 0; i < this->theUnknownCartanCentralizerBasis.size; i ++) {
      this->getGenericCartanCentralizerLinearCombination(i, this->theUnknownCartanCentralizerBasis[i]);
      theCentralizerCartanElts[i] = this->theUnknownCartanCentralizerBasis[i].getCartanPart();
    }
    theCentralizerCartanElts.getGramMatrix(theCentralizerCartanVars, &this->getAmbientWeyl().cartanSymmetric);
    Polynomial<AlgebraicNumber> theDeterminant, theDetMultiplier;
    theDeterminant.makeDeterminantFromSquareMatrix(theCentralizerCartanVars);
    theDetMultiplier.makeMonomial(this->totalNumUnknownsWithCentralizer - 1, 1, 1);
    theDeterminant *= theDetMultiplier;
    theDeterminant += - Rational(1);
    this->theSystemToSolve.addOnTop(theDeterminant);
  }
  if (this->theUnknownNegGens.size != this->theUnknownPosGens.size) {
    global.fatal << "Error: number of unknown negative generators differs from number of unknown positive ones. " << global.fatal;
  }
  for (int i = 0; i < this->theUnknownNegGens.size; i ++) {
    desiredHpart = this->theHsScaledToActByTwo[i];//<-implicit type conversion here!
    goalValue.makeCartanGenerator(desiredHpart, *this->owner->owner);
    this->getAmbientSemisimpleLieAlgebra().lieBracket(this->theUnknownPosGens[i], this->theUnknownNegGens[i], lieBracketMinusGoalValue);
    lieBracketMinusGoalValue -= goalValue;
    this->addToSystem(lieBracketMinusGoalValue);
    for (int j = 0; j < this->theUnknownCartanCentralizerBasis.size; j ++) {
      this->getAmbientSemisimpleLieAlgebra().lieBracket(this->theUnknownNegGens[i], this->theUnknownCartanCentralizerBasis[j], lieBracketMinusGoalValue);
      this->addToSystem(lieBracketMinusGoalValue);
      this->getAmbientSemisimpleLieAlgebra().lieBracket(this->theUnknownPosGens[i], this->theUnknownCartanCentralizerBasis[j], lieBracketMinusGoalValue);
      this->addToSystem(lieBracketMinusGoalValue);
    }
    for (int j = 0; j < this->theUnknownPosGens.size; j ++) {
      if (i != j) {
        this->getAmbientSemisimpleLieAlgebra().lieBracket(this->theUnknownNegGens[i], this->theUnknownPosGens[j], lieBracketMinusGoalValue);
        this->addToSystem(lieBracketMinusGoalValue);
        Vector<Rational> posRoot1, posRoot2;
        posRoot1.makeEi(this->weylNonEmbedded->getDimension(), i);
        posRoot2.makeEi(this->weylNonEmbedded->getDimension(), j);
        int alphaStringLength = - 1;
        if (!nonEmbeddedMe.getMaxQForWhichBetaMinusQAlphaisARoot(posRoot1, - posRoot2, alphaStringLength)) {
          global.fatal << "The alpha-string along " << posRoot1.toString()
          << " through " << (- posRoot2).toString()
          << " does not contain any root, which is impossible. " << global.fatal;
        }
        // positive-positive generator Serre relations
        lieBracketMinusGoalValue = this->theUnknownPosGens[j];
        for (int k = 0; k < alphaStringLength + 1; k ++) {
          this->getAmbientSemisimpleLieAlgebra().lieBracket(this->theUnknownPosGens[i], lieBracketMinusGoalValue, lieBracketMinusGoalValue);
        }
        this->addToSystem(lieBracketMinusGoalValue);
        // negative-negative generator Serre relations
        lieBracketMinusGoalValue = this->theUnknownNegGens[j];
        for (int k = 0; k < alphaStringLength + 1; k ++) {
          this->getAmbientSemisimpleLieAlgebra().lieBracket(this->theUnknownNegGens[i], lieBracketMinusGoalValue, lieBracketMinusGoalValue);
        }
        this->addToSystem(lieBracketMinusGoalValue);
      }
    }
  }
  this->flagSystemSolved = false;
  if (!AttemptToChooseCentalizer) {
    this->flagSystemSolved = this->checkGeneratorsBracketToHs();
  }
  if (!this->flagSystemSolved) {
    this->flagSystemGroebnerBasisFound = false;
    this->flagSystemProvedToHaveNoSolution = false;
    if (this->owner->flagAttemptToSolveSystems) {
      long long int startNumOps =Rational::totalArithmeticOperations();
      this->attemptToSolveSystem();
      this->totalArithmeticOpsToSolveSystem +=Rational::totalArithmeticOperations()-startNumOps;
    }
  } else {
    this->flagSystemGroebnerBasisFound = false;
    this->flagSystemProvedToHaveNoSolution = false;
  }
  if (!this->flagSystemSolved && this->flagUsedInducingSubalgebraRealization) {
    //bool seedHadNoSolution = this->flagSystemProvedToHaveNoSolution;
    this->flagUsedInducingSubalgebraRealization = false;
    bool result = this->computeSystemPart2(AttemptToChooseCentalizer, allowNonPolynomialSystemFailure);
    return result;
  }
  if (this->flagSystemProvedToHaveNoSolution) {
    return false;
  }
  if (this->flagSystemSolved) {
    this->theBasis = this->theNegGens;
    this->theBasis.addListOnTop(this->thePosGens);
    if (this->theBasis.size > 0) {
      this->owner->owner->generateLieSubalgebra(this->theBasis);
      if (this->theBasis.size != this->weylNonEmbedded->theDynkinType.getLieAlgebraDimension()) {
        if (!allowNonPolynomialSystemFailure) {
          global.fatal
          << "Lie subalgebra dimension doesn't fit: dimension of generated subalgebra is "
          << this->theBasis.size << ", must be " << this->weylNonEmbedded->theDynkinType.getLieAlgebraDimension()
          << ". The subalgebra is " << this->toString() << "<br>Involved generators: "
          << this->theInvolvedNegGenerators.toString()
          << "<br>and<br>" << this->theInvolvedPosGenerators.toString() << global.fatal;
        }
        return false;
      }
    }
    this->owner->owner->getCommonCentralizer(this->thePosGens, this->highestVectorsNonSorted);
    this->computeCartanOfCentralizer();
    this->computePrimalModuleDecompositionHWsHWVsOnly();
  }
  return true;
}

void CandidateSemisimpleSubalgebra::extendToModule(List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& inputOutput) {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::extendToModule");
  ElementSemisimpleLieAlgebra<AlgebraicNumber> tempElt;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > theVectorSpace;
  HashedList<ChevalleyGenerator> bufferList;
  theVectorSpace = inputOutput;
  ProgressReport theReport;
  for (int i = 0; i < inputOutput.size; i ++) {
    for (int j = 0; j < this->theNegGens.size; j ++) {
      std::stringstream reportStream;
      reportStream << "extendToModule: Lie bracket of element of index "
      << i + 1 << " and negative generator index " << j+ 1 << ".";
      theReport.report(reportStream.str());
      this->getAmbientSemisimpleLieAlgebra().lieBracket(this->theNegGens[j], inputOutput[i], tempElt);
      theVectorSpace.addOnTop(tempElt);
      tempElt.gaussianEliminationByRowsDeleteZeroRows(theVectorSpace, nullptr, &bufferList);
      if (theVectorSpace.size >inputOutput.size) {
        inputOutput.addOnTop(tempElt);
      }
    }
  }
}

template <class TemplateMonomial, class Coefficient>
template <class LinearCombinationTemplate>
void LinearCombination<TemplateMonomial, Coefficient>::intersectVectorSpaces(
  const List<LinearCombinationTemplate>& vectorSpace1,
  const List<LinearCombinationTemplate>& vectorSpace2,
  List<LinearCombinationTemplate>& outputIntersection,
  HashedList<TemplateMonomial>* seedMonomials
) {
  MacroRegisterFunctionWithName("MonomialCollection::intersectVectorSpaces");
  List<LinearCombinationTemplate> theVspaces =vectorSpace1;
  List<LinearCombinationTemplate> vectorSpace2eliminated =vectorSpace2;
  LinearCombination<TemplateMonomial, Coefficient>::gaussianEliminationByRowsDeleteZeroRows(vectorSpace2eliminated, nullptr, seedMonomials);
  LinearCombination<TemplateMonomial, Coefficient>::gaussianEliminationByRowsDeleteZeroRows(theVspaces, nullptr, seedMonomials);
  Matrix<Coefficient> theLinCombiMat;
  int firstSpaceDim = theVspaces.size;
  theLinCombiMat.makeIdentityMatrix(theVspaces.size +vectorSpace2eliminated.size);
  theVspaces.addListOnTop(vectorSpace2eliminated);
  vectorSpace2eliminated = theVspaces;
  LinearCombination<TemplateMonomial, Coefficient>::gaussianEliminationByRows(theVspaces, nullptr, seedMonomials, &theLinCombiMat);
  int dimResult = 0;
  for (int i = theVspaces.size - 1; i >= 0; i --) {
    if (theVspaces[i].isEqualToZero()) {
      dimResult ++;
    } else {
      break;
    }
  }
  outputIntersection.setSize(dimResult);
  int counter = - 1;
  LinearCombinationTemplate tempMCT;
  for (int i = theVspaces.size - 1; i >= 0; i --) {
    if (!theVspaces[i].isEqualToZero()) {
      break;
    }
    counter ++;
    outputIntersection[counter].makeZero();
    for (int j = 0; j < firstSpaceDim; j ++) {
      tempMCT = vectorSpace2eliminated[j];
      tempMCT *= theLinCombiMat(i, j);
      outputIntersection[counter] += tempMCT;
    }
  }
}

bool CandidateSemisimpleSubalgebra::checkModuleDimensions() const {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::checkModuleDimensions");
  int totalDim = 0;
  for (int i = 0; i < this->modules.size; i ++) {
    for (int j = 0; j < this->modules[i].size; j ++) {
      totalDim += this->modules[i][j].size;
    }
  }
  if (totalDim != this->getAmbientSemisimpleLieAlgebra().getNumberOfGenerators()) {
    FormatExpressions theFormat;
    theFormat.flagCandidateSubalgebraShortReportOnly = false;
    global.fatal << "<br><b>Something went very wrong with candidate "
    << this->weylNonEmbedded->theDynkinType.toString() << ": dimensions DONT FIT!!! More precisely, "
    << "I am getting total module dimension sum  " << totalDim
    << " instead of " << this->getAmbientSemisimpleLieAlgebra().getNumberOfGenerators()
    << ".</b> Here is a detailed subalgebra printout. " << this->toString(&theFormat) << global.fatal;
  }
  return true;
}

void CandidateSemisimpleSubalgebra::computeRatioKillingsByComponent() {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::computeRatioKillingsByComponent");
  ElementSemisimpleLieAlgebra<AlgebraicNumber> currentElt, adActionElt, adadActionElt;
  Vector<AlgebraicNumber> theLinearCombi;
  this->RatioKillingsByComponent.setSize(this->cartanSubalgebrasByComponentScaledToActByTwo.size);
  Matrix<AlgebraicNumber> theAdMat;
  for (int i = 0; i < this->cartanSubalgebrasByComponentScaledToActByTwo.size; i ++) {
    currentElt.makeCartanGenerator(this->cartanSubalgebrasByComponentScaledToActByTwo[i][0], this->getAmbientSemisimpleLieAlgebra());
    AlgebraicNumber result = 0;
    for (int k = 0; k < this->theBasis.size; k ++) {
      this->getAmbientSemisimpleLieAlgebra().lieBracket(currentElt, this->theBasis[k], adActionElt);
      this->getAmbientSemisimpleLieAlgebra().lieBracket(currentElt, adActionElt, adadActionElt);
      bool tempB= currentElt.linearSpanContainsGetFirstLinearCombination(this->theBasis, adadActionElt, theLinearCombi);
      if (!tempB) {
        global.fatal << "Candidate subalgebra not closed under Lie bracket. " << global.fatal;
      }
      result += theLinearCombi[k];
    }
    this->getAmbientSemisimpleLieAlgebra().getAdjoint(theAdMat, currentElt);
    theAdMat *= theAdMat;
    this->RatioKillingsByComponent[i] = theAdMat.getTrace();
    this->RatioKillingsByComponent[i] /= result;
  }
}

void CandidateSemisimpleSubalgebra::computePrimalModuleDecomposition() {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::computePrimalModuleDecomposition");
  for (int i = 0; i < this->modules.size; i ++) {
    for (int j = 0; j < this->modules[i].size; j ++) {
      this->extendToModule(this->modules[i][j]);
    }
  }
  this->checkModuleDimensions();
  this->ModulesIsotypicallyMerged.setSize(this->modules.size);
  for (int i = 0; i < this->modules.size; i ++) {
    this->ModulesIsotypicallyMerged[i].setSize(0);
    for (int j = 0; j < this->modules[i].size; j ++) {
      this->ModulesIsotypicallyMerged[i].addListOnTop(this->modules[i][j]);
    }
  }
  //please note: part of primalSubalgebraModules have already been computed.
  for (int i = 0; i < this->ModulesIsotypicallyMerged.size; i ++) {
    if (this->ModulesIsotypicallyMerged[i][0].isElementCartan()) {
      for (int j = 0; j < this->ModulesIsotypicallyMerged[i].size; j ++) {
        if (!this->ModulesIsotypicallyMerged[i][j].isElementCartan()) {
          global.fatal << "<br>This is a programming or mathematical error. Module " << this->ModulesIsotypicallyMerged[i].toString()
          << " has elements of the ambient Cartan and elements outside of the ambient Cartan, which is not allowed. "
          << "<br>Here is a detailed subalgebra printout. " << global.fatal;
        }
      }
      this->primalSubalgebraModules.addOnTop(i);
    }
  }
  this->fullBasisByModules.setSize(0);
  this->fullBasisOwnerModules.setSize(0);
  this->fullBasisByModules.reserve(this->getAmbientSemisimpleLieAlgebra().getNumberOfGenerators());
  this->fullBasisOwnerModules.reserve(this->getAmbientSemisimpleLieAlgebra().getNumberOfGenerators());
  for (int i = 0; i < this->ModulesIsotypicallyMerged.size; i ++) {
    for (int j = 0; j < this->ModulesIsotypicallyMerged[i].size; j ++) {
      this->fullBasisByModules.addOnTop(this->ModulesIsotypicallyMerged[i][j]);
      this->fullBasisOwnerModules.addOnTop(i);
    }
  }
  if (this->fullBasisByModules.size != this->getAmbientSemisimpleLieAlgebra().getNumberOfGenerators()) {
    global.fatal << "The full basis by modules "
    << "does not have same number of elements as the number of generators of the ambient Lie algebra. "
    << global.fatal;
  }
  this->WeightsModulesNONprimal.setSize(this->modules.size);
  this->WeightsModulesPrimal.setSize(this->modules.size);
  Vector<Rational> theProjection, thePrimalProjection;
  for (int i = 0; i < this->modules.size; i ++) {
    for (int k = 0; k < this->modules[i].size; k ++) {
      if (k == 0) {
        this->WeightsModulesNONprimal[i].setSize(this->modules[i][0].size);
        this->WeightsModulesPrimal[i].setSize(this->modules[i][0].size);
      }
      for (int j = 0; j < this->modules[i][k].size; j ++) {
        ElementSemisimpleLieAlgebra<AlgebraicNumber>& currentElt = this->modules[i][k][j];
        Vector<Rational> currentRoot = this->getAmbientSemisimpleLieAlgebra().getWeightOfGenerator(currentElt[0].generatorIndex);
        this->getWeightProjectionFundamentalCoordinates(currentRoot, theProjection);
        this->getPrimalWeightProjectionFundamentalCoordinates(currentRoot, thePrimalProjection);
        if (k == 0) {
          this->WeightsModulesNONprimal[i][j] = theProjection;
          this->WeightsModulesPrimal[i][j] = thePrimalProjection;
        } else {
          if (this->WeightsModulesNONprimal[i][j] != theProjection || this->WeightsModulesPrimal[i][j] != thePrimalProjection) {
            global.fatal << "This is a programming or mathematical error. Given two isomorphic modules over "
            << "the semisimple subalgebra (i.e., same highest weights), "
            << "and the same order of generation of weights, I got different order "
            << " of the lower weights of the modules. Something is very wrong. " << global.fatal;
          }
        }
      }
    }
  }
  this->centralizerSubalgebraModules.setSize(0);
  this->RootSystemCentralizerPrimalCoords.clear();
  for (int i = 0; i < this->ModulesIsotypicallyMerged.size; i ++) {
    if (this->ModulesIsotypicallyMerged[i].size == 1 || this->WeightsModulesPrimal[i][0].isEqualToZero()) {
      this->centralizerSubalgebraModules.addOnTop(i);
      if (!this->WeightsModulesPrimal[i][0].isEqualToZero()) {
        this->RootSystemCentralizerPrimalCoords.addOnTop(this->WeightsModulesPrimal[i][0]);
      }
    }
  }
  for (int i = 0; i < this->subalgebraModules.size; i ++) {
    for (int j = 0; j < this->WeightsModulesPrimal[this->subalgebraModules[i]].size; j ++) {
      if (!this->WeightsModulesPrimal[this->subalgebraModules[i]][j].isEqualToZero()) {
        this->RootSystemSubalgebraPrimalCoords.addOnTop(this->WeightsModulesPrimal[this->subalgebraModules[i]][j]);
      }
    }
  }
  Vectors<Rational> simpleSubSystem;
  simpleSubSystem = this->RootSystemCentralizerPrimalCoords;
  this->theCentralizerSubDiagram.computeDiagramTypeModifyInputRelative(
    simpleSubSystem, this->RootSystemCentralizerPrimalCoords, this->bilinearFormFundPrimal
  );
  this->theCentralizerSubDiagram.getDynkinType(this->theCentralizerType);
  this->flagCentralizerTypeIsComputed = true;
  this->computeCharactersPrimalModules();
 // this->computeRatioKillingsByComponent();
}

void CandidateSemisimpleSubalgebra::reset(SemisimpleSubalgebras* inputOwner) {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::reset");
  this->owner = inputOwner;
  this->indexInOwner = - 1;
  this->indexIamInducedFrom = - 1;
  this->indexNonEmbeddedMeNonStandardCartan = - 1;
  this->indexNonEmbeddedMeStandard = - 1;
  this->indexHcandidateBeingGrown = - 1;
  this->indexMaxSSContainer = - 1;
  this->indexSSPartCentralizer = - 1;
  this->flagSubalgebraPreloadedButNotVerified = false;
  this->flagSystemSolved = false;
  this->flagSystemProvedToHaveNoSolution = false;
  this->flagSystemGroebnerBasisFound = false;
  this->flagCentralizerIsWellChosen = false;
  this->flagCentralizerTypeIsComputed = false;
  this->flagUsedInducingSubalgebraRealization = true;
  this->totalNumUnknownsNoCentralizer = 0;
  this->totalNumUnknownsWithCentralizer = 0;
  this->totalArithmeticOpsToSolveSystem = 0;
  this->NumConeIntersections = - 1;
  this->NumCasesNoLinfiniteRelationFound = - 1;
  this->NumBadParabolics = 0;
  this->NumCentralizerConditionFailsconeConditionHolds = 0;
  this->centralizerRank = - 1;
  this->weylNonEmbedded = nullptr;
  this->theSubalgebraNonEmbeddedDefaultScale = nullptr;
}

CandidateSemisimpleSubalgebra::CandidateSemisimpleSubalgebra() {
  this->flagDeallocated = false;
  this->reset(nullptr);
}

CandidateSemisimpleSubalgebra::~CandidateSemisimpleSubalgebra() {
  this->flagDeallocated = true;
}

void CandidateSemisimpleSubalgebra::computePairKWeightElementAndModule(
  const ElementSemisimpleLieAlgebra<AlgebraicNumber>& leftKweightElt, int rightIndex, List<int>& output
) {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::computePairKWeightElementAndModule");
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& rightModule = this->ModulesIsotypicallyMerged[rightIndex];
  ElementSemisimpleLieAlgebra<AlgebraicNumber> theLieBracket;
  ProgressReport theReport;
  Vector<AlgebraicNumber> coordsInFullBasis;
  output.setSize(0);
  if (this->fullBasisByModules.size != this->getAmbientSemisimpleLieAlgebra().getNumberOfGenerators()) {
    global.fatal << "FullBasisByModules not computed when it should be. " << global.fatal;
  }
  for (int j = 0; j < rightModule.size; j ++) {
    this->getAmbientSemisimpleLieAlgebra().lieBracket(leftKweightElt, rightModule[j], theLieBracket);
    if (theReport.tickAndWantReport()) {
      std::stringstream reportStream;
      reportStream << "Bracketing index " << j + 1 << " with input element. ";
      theReport.report(reportStream.str());
    }
    bool tempbool = theLieBracket.getCoordinatesInBasis(this->fullBasisByModules, coordsInFullBasis);
    if (!tempbool) {
      global.fatal << "Something has gone very wrong: my k-weight basis "
      << this->fullBasisByModules.toString()
      << " does not contain " << theLieBracket.toString() << global.fatal;
    }
    for (int i = 0; i < coordsInFullBasis.size; i ++) {
      if (!coordsInFullBasis[i].isEqualToZero()) {
        output.addOnTopNoRepetition(this->fullBasisOwnerModules[i]);
      }
    }
  }
}

void CandidateSemisimpleSubalgebra::computeSinglePair(int leftIndex, int rightIndex, List<int>& output) {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::computeSinglePair");
  output.setSize(0);
  List<int> tempList;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& leftModule = this->ModulesIsotypicallyMerged[leftIndex];
  ProgressReport theReport;
  for (int i = 0; i < leftModule.size; i ++) {
    if (theReport.tickAndWantReport()) {
      std::stringstream reportStream;
      reportStream << "Bracketing element number " << i + 1 << " out of " << leftModule.size << " with other module. ";
      theReport.report(reportStream.str());
    }
    this->computePairKWeightElementAndModule(leftModule[i], rightIndex, tempList);
    output.addOnTopNoRepetition(tempList);
  }
}

void CandidateSemisimpleSubalgebra::computePairingTable() {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::computePairingTable");
  if (!this->flagCentralizerIsWellChosen)
    return;
  ProgressReport theReport;
  this->NilradicalPairingTable.setSize(this->ModulesIsotypicallyMerged.size);
  for (int i = 0; i < this->NilradicalPairingTable.size; i ++) {
    this->NilradicalPairingTable[i].setSize(this->ModulesIsotypicallyMerged.size);
  }
  for (int i = 0; i < this->NilradicalPairingTable.size; i ++) {
    for (int j = i; j < this->NilradicalPairingTable[i].size; j ++) {
      if (theReport.tickAndWantReport()) {
        std::stringstream reportStream;
        reportStream << "Pairing indices " << i + 1 << " and " << j + 1 << " out of "
        << this->NilradicalPairingTable.size << ".";
        theReport.report(reportStream.str());
      }
      this->computeSinglePair(i, j, this->NilradicalPairingTable[i][j]);
      if (j > i) {
        this->NilradicalPairingTable[j][i] = this->NilradicalPairingTable[i][j];
      }
    }
  }
  this->modulesWithZeroWeights.clear();
  for (int i = 0; i < this->NilradicalPairingTable.size; i ++) {
    for (int j = 0; j < this->ModulesIsotypicallyMerged[i].size; j ++) {
      if (this->getAmbientSemisimpleLieAlgebra().getWeightOfGenerator(
        this->ModulesIsotypicallyMerged[i][j][0].generatorIndex).isEqualToZero()
      ) {
        this->modulesWithZeroWeights.addOnTop(i);
        break;
      }
    }
  }
  this->OppositeModulesByStructure.setSize(this->NilradicalPairingTable.size);
  for (int i = 0; i < this->NilradicalPairingTable.size; i ++) {
    for (int j = 0; j < this->NilradicalPairingTable[i].size; j ++) {
      for (int k = 0; k < this->NilradicalPairingTable[i][j].size; k ++) {
        if (this->modulesWithZeroWeights.contains(this->NilradicalPairingTable[i][j][k])) {
          if (!(this->primalSubalgebraModules.contains(i) && this->primalSubalgebraModules.contains(j))) {
            this->OppositeModulesByStructure[i].addOnTopNoRepetition(j);
          }
        }
      }
    }
  }
  this->computeKsl2Triples();
}

void CandidateSemisimpleSubalgebra::computeCharactersPrimalModules() {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::computeCharactersPrimalModules");
  this->CharsPrimalModules.setSize(this->modules.size);
  this->CharsPrimalModulesMerged.setSize(this->modules.size);
  Weight<Rational> currentWeight;
  currentWeight.owner = nullptr;
  for (int i = 0; i < this->CharsPrimalModules.size; i ++) {
    this->CharsPrimalModules[i].makeZero();
    this->CharsPrimalModulesMerged[i].makeZero();
    for (int j = 0; j < this->WeightsModulesPrimal[i].size; j ++) {
      currentWeight.weightFundamentalCoordS = this->WeightsModulesPrimal[i][j];
      this->CharsPrimalModules[i].addMonomial(currentWeight, 1);
      this->CharsPrimalModulesMerged[i].addMonomial(currentWeight, this->modules[i].size);
    }
  }
  this->OppositeModulesByChar.setSize(this->modules.size);
  List<CharacterSemisimpleLieAlgebraModule<Rational> > theDualMods;
  theDualMods.setSize(this->modules.size);
  for (int i = 0; i < this->modules.size; i ++) {
    this->CharsPrimalModules[i].getDual(theDualMods[i]);
    this->OppositeModulesByChar[i].setSize(0);
  }
  for (int i = 0; i < this->modules.size; i ++) {
    for (int j = i; j < this->modules.size; j ++) {
      if ((this->CharsPrimalModules[i] - theDualMods[j]).isEqualToZero()) {
        this->OppositeModulesByChar[i].addOnTop(j);
        if (i != j) {
          this->OppositeModulesByChar[j].addOnTop(i);
        }
      }
    }
  }
}

void CandidateSemisimpleSubalgebra::computeKsl2TriplesGetOppositeElementsInOppositeModule(
  const Vector<Rational>& theElementWeight,
  const List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& inputOppositeModule,
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputElts
) {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::computeKsl2TriplesGetOppositeElementsInOppositeModule");
  outputElts.setSize(0);
  for (int i = 0; i < inputOppositeModule.size; i ++) {
    if ((this->getPrimalWeightFirstGen(inputOppositeModule[i]) +theElementWeight).isEqualToZero()) {
      outputElts.addOnTop(inputOppositeModule[i]);
    }
  }
}

Vector<Rational> CandidateSemisimpleSubalgebra::getPrimalWeightFirstGen(const ElementSemisimpleLieAlgebra<AlgebraicNumber>& input) const {
  Vector<Rational> output;
  Vector<Rational> theRoot = this->getAmbientSemisimpleLieAlgebra().getWeightOfGenerator(input[0].generatorIndex);
  this->getPrimalWeightProjectionFundamentalCoordinates(theRoot, output);
  return output;
}

Vector<Rational> CandidateSemisimpleSubalgebra::getNonPrimalWeightFirstGenerator(const ElementSemisimpleLieAlgebra<AlgebraicNumber>& input) const {
  Vector<Rational> output;
  Vector<Rational> theRoot = this->getAmbientSemisimpleLieAlgebra().getWeightOfGenerator(input[0].generatorIndex);
  this->getWeightProjectionFundamentalCoordinates(theRoot, output);
  return output;
}

void CandidateSemisimpleSubalgebra::computeKsl2TriplesGetOppositeElementsAll(
  const Vector<Rational>& theElementWeight, List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputElts
) {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::computeKsl2TriplesGetOppositeElementsAll");
  outputElts.setSize(0);
  for (int i = 0; i < this->modules.size; i ++) {
    for (int j = 0; j < this->WeightsModulesPrimal[i].size; j ++) {
      if ((theElementWeight + this->WeightsModulesPrimal[i][j]).isEqualToZero()) {
        for (int k = 0; k < this->modules[i].size; k ++) {
          outputElts.addOnTop(this->modules[i][k][j]);
          if (!(theElementWeight + this->getPrimalWeightFirstGen(this->modules[i][k][j])).isEqualToZero()) {
            global.fatal << "Element this->modules[i][k][j] does not have the primal weight it is supposed to have. "
            << global.fatal;
          }
        }
      }
    }
  }
}

bool CandidateSemisimpleSubalgebra::computeKsl2TriplesetUpAndSolveSystem(
  const ElementSemisimpleLieAlgebra<AlgebraicNumber>& theE,
  const List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& FisLinearCombiOf,
  ElementSemisimpleLieAlgebra<AlgebraicNumber>& outputF
) {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::computeKsl2TriplesetUpAndSolveSystem");
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > Ecopy, theH, theF, tempElt;
  Ecopy = theE;
  this->getAmbientSemisimpleLieAlgebra().getGenericElementCartan(theH, 0);
  theF.makeZero();
  Polynomial<AlgebraicNumber> tempP;
  for (int i = 0; i < FisLinearCombiOf.size; i ++) {
    tempElt = FisLinearCombiOf[i];
    tempP.makeMonomial(i + this->getAmbientSemisimpleLieAlgebra().getRank(), 1, 1);
    tempElt *= tempP;
    theF += tempElt;
  }
  if (!this->getAmbientSemisimpleLieAlgebra().attemptFindingHEF(theH, Ecopy, theF, nullptr)) {
    return false;
  }
  outputF = theF;
  return true;
}

void CandidateSemisimpleSubalgebra::computeKsl2Triples() {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::computeKsl2Triples");
  if (!this->owner->flagComputeNilradicals) {
    return;
  }
  this->modulesSl2opposite.setSize(this->modules.size);
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > FmustBeAlinCombiOf;
  for (int i = 0; i < this->modules.size; i ++) {
    this->modulesSl2opposite[i].setSize(this->modules[i].size);
    for (int j = 0; j < this->modules[i].size; j ++) {
      this->modulesSl2opposite[i][j].setSize(this->modules[i][j].size);
      for (int k = 0; k < this->modulesSl2opposite[i][j].size; k ++) {
        this->computeKsl2TriplesGetOppositeElementsInOppositeModule(
          this->WeightsModulesPrimal[i][k],
          this->ModulesIsotypicallyMerged[this->OppositeModulesByChar[i][0]],
          FmustBeAlinCombiOf
        );
        if (this->computeKsl2TriplesetUpAndSolveSystem(this->modules[i][j][k], FmustBeAlinCombiOf, this->modulesSl2opposite[i][j][k])) {
          continue;
        }
        this->computeKsl2TriplesGetOppositeElementsAll(this->WeightsModulesPrimal[i][k], FmustBeAlinCombiOf);
        this->computeKsl2TriplesetUpAndSolveSystem(this->modules[i][j][k], FmustBeAlinCombiOf, this->modulesSl2opposite[i][j][k]);
      }
    }
  }
}

int CandidateSemisimpleSubalgebra::getRank() const {
  return this->weylNonEmbedded->theDynkinType.getRank();
}

int CandidateSemisimpleSubalgebra::getPrimalRank() const {
  if (!this->flagCentralizerIsWellChosen) {
    return - 1;
  }
  int theRank = 0;
  if (!this->centralizerRank.isSmallInteger(&theRank)) {
    return - 1;
  }
  return theRank+this->weylNonEmbedded->getDimension();
}

void NilradicalCandidate::checkInitialization() const {
  if (this->owner == nullptr) {
    global.fatal << "NilradicalCandidate with non-initialized owner" << global.fatal;
  }
}

Vector<Rational> NilradicalCandidate::getConeStrongIntersectionWeight() const {
  Vector<Rational> theWeight;
  theWeight.makeZero(this->owner->getPrimalRank());
  for (int i = 0; i < this->theNilradicalWeights.size; i ++) {
    theWeight += this->theNilradicalWeights[i] * this->ConeStrongIntersection[i];
  }
  return theWeight;
}

bool NilradicalCandidate::isCommutingSelectionNilradicalElements(Selection& inputNilradSel) {
  if (inputNilradSel.cardinalitySelection == 1) {
    return true;
  }
  ElementSemisimpleLieAlgebra<AlgebraicNumber> mustBeZero;
  for (int i = 0; i < inputNilradSel.cardinalitySelection; i ++) {
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& leftElt = this->theNilradical[inputNilradSel.elements[i]];
    for (int j = 0; j < inputNilradSel.cardinalitySelection; j ++) {
      if (i == j) {
        continue;
      }
      ElementSemisimpleLieAlgebra<AlgebraicNumber>& rightEltPositive = this->theNilradical[inputNilradSel.elements[j]];
      ElementSemisimpleLieAlgebra<AlgebraicNumber>& rightEltNegative = this->theNilradicalElementOpposites[inputNilradSel.elements[j]];
      this->owner->getAmbientSemisimpleLieAlgebra().lieBracket(leftElt, rightEltPositive, mustBeZero);
      if (!mustBeZero.isEqualToZero()) {
        return false;
      }
      if (rightEltNegative.isEqualToZero()) {
        return false;
      }
      this->owner->getAmbientSemisimpleLieAlgebra().lieBracket(leftElt, rightEltNegative, mustBeZero);
      if (!mustBeZero.isEqualToZero()) {
        return false;
      }
    }
  }
  for (int i = 0; i < inputNilradSel.cardinalitySelection; i ++) {
    for (int j = i + 1; j < inputNilradSel.cardinalitySelection; j ++) {
      if (this->owner->getScalarSubalgebra(
          this->theNilradicalWeights[inputNilradSel.elements[i]], this->theNilradicalWeights[inputNilradSel.elements[j]]
        ) < 0
      ) {
        /*global.fatal << "<br>This is either a programming error, or I am missing some mathematical phenomenon: k-sl(2)-triples are "
        << "strongly orthogonal, but their k-weights aren't. Crashing to tactfully let you know. "
        << "The bad elements are: "
        << this->theNilradical[inputNilradSel.elements[i]].toString() << " of weight "
        << this->theNilradicalWeights[inputNilradSel.elements[i]].toString()
        << " and "
        << this->theNilradical[inputNilradSel.elements[j]].toString() << " of weight "
        << this->theNilradicalWeights[inputNilradSel.elements[j]].toString() << ". "
        //<< "The bilinear form is: " << this->owner->bilinearFormFundPrimal.toString() << ". "
        //<< " and the subalgebra in play is: " << this->owner->toString() << ". "
        //<< global.fatal;
        */
        return false;
      }
    }
  }
  return true;
}

Vector<Rational> NilradicalCandidate::getNilradicalLinearCombination() const {
  Vector<Rational> theNilradLinCombi = this->ConeStrongIntersection;
  theNilradLinCombi.setSize(this->theNilradicalWeights.size);
  return theNilradLinCombi;
}

void NilradicalCandidate::computeParabolicACExtendsToParabolicAC() {
  MacroRegisterFunctionWithName("NilradicalCandidate::computeParabolicACExtendsToParabolicAC");
  Vector<Rational> projectionRoot;
  WeylGroupData& theWeyl = this->owner->owner->owner->theWeyl;
  this->leviRootsAmbienT.reserve(theWeyl.rootSystem.size);
  this->leviRootsSmallPrimalFundCoords.reserve(theWeyl.rootSystem.size);
  Vectors<Rational> rootSystemProjections;
  Vectors<Rational> conesCombination;
  rootSystemProjections.setSize(theWeyl.rootSystem.size);
  for (int i = 0; i < theWeyl.rootSystem.size; i ++) {
    this->owner->getPrimalWeightProjectionFundamentalCoordinates(theWeyl.rootSystem[i], rootSystemProjections[i]);
  }
  conesCombination = this->theNilradicalWeights;
  for (int i = 0; i < this->theNonFKhws.size; i ++) {
    conesCombination.addOnTop(-this->theNonFKhws[i]);
  }
  this->ConeSeparatingNormal.perturbNormalRelativeToVectorsInGeneralPosition(conesCombination, rootSystemProjections);
  for (int i = 0; i < theWeyl.rootSystem.size; i ++) {
    this->owner->getPrimalWeightProjectionFundamentalCoordinates(theWeyl.rootSystem[i], projectionRoot);
    if (projectionRoot.scalarEuclidean(this->ConeSeparatingNormal) == 0) {
      this->leviRootsAmbienT.addOnTop(theWeyl.rootSystem[i]);
      if (this->owner->RootSystemCentralizerPrimalCoords.contains(projectionRoot)) {
        this->leviRootsSmallPrimalFundCoords.addOnTop(projectionRoot);
      }
    }
  }
  Vectors<Rational> tempVs = this->leviRootsAmbienT;
  this->theLeviDiagramAmbienT.computeDiagramTypeModifyInput(tempVs, theWeyl);
  this->theLeviDiagramSmalL.computeDiagramTypeModifyInputRelative(
    this->leviRootsSmallPrimalFundCoords,
    this->owner->RootSystemCentralizerPrimalCoords,
    this->owner->bilinearFormFundPrimal
  );
  bool ambientLeviHasBDGE = false;
  for (int i = 0; i < this->theLeviDiagramAmbienT.simpleComponentTypes.size; i ++) {
    DynkinSimpleType& currentType = this->theLeviDiagramAmbienT.simpleComponentTypes[i];
    if (
      currentType.theLetter == 'D' ||
      currentType.theLetter == 'G' ||
      currentType.theLetter == 'E' ||
      currentType.theLetter == 'E'
    ) {
      ambientLeviHasBDGE = true;
      break;
    }
    if (currentType.theLetter == 'B' && currentType.theRank > 2) {
      ambientLeviHasBDGE = true;
      break;
    }
  }
  bool smallLeviHasBDGE = false;
  for (int i = 0; i < this->theLeviDiagramSmalL.simpleComponentTypes.size; i ++) {
    DynkinSimpleType& currentType = this->theLeviDiagramSmalL.simpleComponentTypes[i];
    if (
      currentType.theLetter == 'D' ||
      currentType.theLetter == 'G' ||
      currentType.theLetter == 'E' ||
      currentType.theLetter == 'E'
    ) {
      smallLeviHasBDGE = true;
      break;
    }
    if (currentType.theLetter == 'B' && currentType.theRank > 2) {
      smallLeviHasBDGE = true;
      break;
    }
  }
  this->flagRestrictedCentralizerConditionHoldS = !smallLeviHasBDGE;
  if (!smallLeviHasBDGE) {
    this->flagParabolicACextendsToParabolicAC = !ambientLeviHasBDGE;
  }
  if (smallLeviHasBDGE) {
    this->flagParabolicACextendsToParabolicAC = true;
    if (!ambientLeviHasBDGE) {
      global.fatal << "This is a mathematical error. Something is very wrong. The ambient parabolic subalgebra has components "
      << " of type A and C, but intesects the centralizer in components of type B and D. This must be impossible according to "
      << "the PSZ paper and the restriction of Fernando's theorem to the centralizer. " << global.fatal;
    }
  }
}

bool NilradicalCandidate::tryFindingLInfiniteRelations() {
  MacroRegisterFunctionWithName("NilradicalCandidate::tryFindingLInfiniteRelations");
  //Vector<Rational> theNilradLinCombi = this->getNilradicalLinearCombination();
  //if (theNilradLinCombi.getNumberOfNonZeroCoordinates() == 1)
  //  return true;
//  Vectors<Rational> curentNilradicalCone;
  Vector<Rational> betterIntersection;
  this->theNilradSubsel.initialize(this->theNilradicalWeights.size);
  this->flagComputedRelativelyStrongIntersections = false;
  for (int i = 1; i < this->theNilradicalWeights.size && i < 5; i ++) {
    LargeInteger numberOfCycles = MathRoutines::nChooseK(this->theNilradicalWeights.size, i);
    this->theNilradSubsel.initSelectionFixedCardinality(i);
    for (int j = 0; j < numberOfCycles; j ++, this->theNilradSubsel.incrementSelectionFixedCardinality(i)) {
      if (this->isCommutingSelectionNilradicalElements(this->theNilradSubsel)) {
        this->theNilradicalWeights.subSelection(this->theNilradSubsel, this->theNilradicalSubsetWeights);
        if (this->theNilradicalSubsetWeights.conesIntersect(
          this->theNilradicalSubsetWeights, this->theNonFKhwsStronglyTwoSided, &betterIntersection, nullptr
        )) {
          betterIntersection.scaleNormalizeFirstNonZero();
          this->ConeStrongIntersection.makeZero(this->theNilradicalWeights.size +this->theNonFKhwsStronglyTwoSided.size);
          this->ConeRelativelyStrongIntersection.setSize(0);
          for (int k = 0; k < this->theNilradSubsel.cardinalitySelection; k ++) {
            this->ConeStrongIntersection[this->theNilradSubsel.elements[k]] = betterIntersection[k];
            this->ConeRelativelyStrongIntersection.addOnTop(betterIntersection[k]);
          }
          this->theNilradical.subSelection(this->theNilradSubsel, this->theNilradicalSubset);
          this->theNilradicalWeights.subSelection(this->theNilradSubsel, this->theNilradicalSubsetWeights);
          this->theNonFKhwVectorsStrongRelativeToSubset.setSize(0);
          this->theNonFKhwVectorsStrongRelativeToSubsetWeights.setSize(0);
          for (int k = 0; k < this->theNonFKhwsStronglyTwoSided.size; k ++) {
            this->ConeStrongIntersection[k + this->theNilradicalWeights.size] = betterIntersection[k + i];
            if (betterIntersection[k + i] != 0) {
              this->theNonFKhwVectorsStrongRelativeToSubset.addOnTop(this->theNonFKHVectorsStronglyTwoSided[k]);
              this->theNonFKhwVectorsStrongRelativeToSubsetWeights.addOnTop(this->theNonFKhwsStronglyTwoSided[k]);
              this->ConeRelativelyStrongIntersection.addOnTop(betterIntersection[k + i]);
            }
          }
          return true;
        }
      }
    }
  }
//  this->ConeStrongIntersection.makeZero(this->ConeStrongIntersection.size);
  ProgressReport theReport;
  this->flagComputedRelativelyStrongIntersections = true;
  for (int i = 1; i < this->theNilradicalWeights.size + 1 && i < 5; i ++) {
    LargeInteger numberOfCycles = MathRoutines::nChooseK(this->theNilradicalWeights.size, i);
    this->theNilradSubsel.initSelectionFixedCardinality(i);
//    this->FKnilradicalLog+= out.str();
    for (int j = 0; j < numberOfCycles; j ++, this->theNilradSubsel.incrementSelectionFixedCardinality(i)) {
      std::stringstream out;
      out << "<br>Trying " << i + 1 << "-tuples (up to 5-tuples): "
      << j + 1 << " out of " << numberOfCycles << " cycles to process. ";
      theReport.report(out.str());
      if (this->isCommutingSelectionNilradicalElements(this->theNilradSubsel)) {
        this->computeTheTwoConesRelativeToNilradicalSubset();
        if (this->theNilradicalSubsetWeights.conesIntersect(
          this->theNilradicalSubsetWeights,
          this->theNonFKhwVectorsStrongRelativeToSubsetWeights,
          &this->ConeRelativelyStrongIntersection,
          nullptr
        )) {
          this->ConeRelativelyStrongIntersection.scaleNormalizeFirstNonZero();
          return true;
        } else {
          this->FKnilradicalLog += "... but the cones dont intersect. ";
        }
      }
    }
  }
  return false;
}

bool NilradicalCandidate::isStronglySingular(int moduleIndex) {
  this->checkInitialization();
  for (int i = 0; i < this->theNilradicalSelection.size; i ++) {
    if (this->theNilradicalSelection[i] == 1 && !this->owner->primalSubalgebraModules.contains(i)) {
      for (int j = 0; j < this->owner->NilradicalPairingTable[moduleIndex][i].size; j ++) {
        if (this->theNilradicalSelection[this->owner->NilradicalPairingTable[moduleIndex][i][j]] != 1) {
          return false;
        }
        if (this->owner->primalSubalgebraModules.contains(this->owner->NilradicalPairingTable[moduleIndex][i][j])) {
          return false;
        }
      }
    }
  }
  return true;
}

bool NilradicalCandidate::isStronglySingularRelativeToSubset(int nonFKweightIndex) {
  MacroRegisterFunctionWithName("NilradicalCandidate::isStronglySingularRelativeToSubset");
  this->checkInitialization();
  ElementSemisimpleLieAlgebra<AlgebraicNumber> mustBeZero;
  for (int j = 0; j < this->theNilradSubsel.cardinalitySelection; j ++) {
    this->owner->owner->owner->lieBracket(
      this->theNilradical[this->theNilradSubsel.elements[j]],
      this->theNonFKhwVectors[nonFKweightIndex],
      mustBeZero
    );
    if (!mustBeZero.isEqualToZero()) {
      return false;
    }
  }
  return true;
}

NilradicalCandidate::NilradicalCandidate() {
  this->owner = nullptr;
  this->flagRestrictedCentralizerConditionHoldS = false;
  this->flagParabolicACextendsToParabolicAC = false;
  this->flagLinfiniteRelFound = false;
}

void NilradicalCandidate::reset() {
  this->theNilradicalWeights.setSize(0);
  this->theNonFKhws.setSize(0);
  this->theNonFKhwsStronglyTwoSided.setSize(0);
  this->theNonFKHVectorsStronglyTwoSided.setSize(0);
  this->theNilradical.setSize(0);
  this->theNilradicalElementOpposites.setSize(0);
  this->ownerModulesNilradicalElements.setSize(0);
  this->ownerModulestheNonFKhwVectors.setSize(0);
  this->theNilradSubsel.initialize(0);
  this->theNilradicalSubset.setSize(0);
  this->theNonFKhwVectorsStrongRelativeToSubset.setSize(0);
  this->theNilradicalSubsetWeights.setSize(0);
  this->theNonFKhwVectorsStrongRelativeToSubsetWeights.setSize(0);
}

void NilradicalCandidate::computeTheTwoConesRelativeToNilradicalSubset() {
  MacroRegisterFunctionWithName("NilradicalCandidate::computeTheTwoConesRelativeToNilradicalSubset");
  this->theNilradical.subSelection(this->theNilradSubsel, this->theNilradicalSubset);
  this->theNilradicalWeights.subSelection(this->theNilradSubsel, this->theNilradicalSubsetWeights);
  this->theNonFKhwVectorsStrongRelativeToSubset.setSize(0);
  this->theNonFKhwVectorsStrongRelativeToSubsetWeights.setSize(0);
  for (int i = 0; i < this->theNonFKhws.size; i ++) {
    if (this->isStronglySingularRelativeToSubset(i)) {
      this->theNonFKhwVectorsStrongRelativeToSubsetWeights.addOnTop(this->theNonFKhws[i]);
      this->theNonFKhwVectorsStrongRelativeToSubset.addOnTop(this->theNonFKhwVectors[i]);
    }
  }
}

void NilradicalCandidate::computeTheTwoCones() {
  MacroRegisterFunctionWithName("NilradicalCandidate::computeTheTwoCones");
  this->checkInitialization();
  this->reset();
  for (int i = 0; i < this->theNilradicalSelection.size; i ++) {
    if (!this->owner->primalSubalgebraModules.contains(i)) {
      if (this->theNilradicalSelection[i] == 0) {
        for (int k = 0; k < this->owner->modules[i].size; k ++) {
          this->theNonFKhws.addOnTop(this->owner->HighestWeightsPrimal[i]);
          this->theNonFKhwVectors.addOnTop(this->owner->highestVectors[i][k]);
          this->ownerModulestheNonFKhwVectors.addOnTop(i);
          if (this->isStronglySingular(i)) {
            this->theNonFKhwsStronglyTwoSided.addOnTop(this->owner->HighestWeightsPrimal[i]);
            this->theNonFKHVectorsStronglyTwoSided.addOnTop(this->owner->highestVectors[i][k]);
          }
        }
      } else if (this->theNilradicalSelection[i] == 1) {
        for (int k = 0; k < this->owner->modules[i].size; k ++) {
          this->theNilradicalWeights.addListOnTop(this->owner->WeightsModulesPrimal[i]);
          this->theNilradical.addListOnTop(this->owner->modules[i][k]);
          this->theNilradicalElementOpposites.addListOnTop(this->owner->modulesSl2opposite[i][k]);
          for (int l = 0; l < this->owner->modules[i][k].size; l ++) {
            this->ownerModulesNilradicalElements.addOnTop(i);
          }
        }
      }
    }
  }
  if (
    this->ownerModulesNilradicalElements.size != this->theNilradical.size ||
    this->theNonFKhws.size != this->ownerModulestheNonFKhwVectors.size
  ) {
    global.fatal << "Sizes of indexing arrasy in "
    << "Fernando Kac nilradical candidate don't match. " << global.fatal;
  }
}

void CandidateSemisimpleSubalgebra::enumerateAllNilradicals() {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::enumerateAllNilradicals");
  ProgressReport theReport;
  ProgressReport theReport2;
  std::stringstream reportStream;
  reportStream << "Enumerating recursively nilradicals of type " << this->toStringTypeAndHs() << "...";
  theReport.report(reportStream.str());
  this->FKNilradicalCandidates.setSize(0);
  List<int> theSel;
  this->RecursionDepthCounterForNilradicalGeneration = 0;
  //0 stands for not selected, 1 for selected from nilradical, 3 for selected from semisimple part, 2 stands for unknown.
  theSel.initializeFillInObject(this->NilradicalPairingTable.size, 2);
  for (int i = 0; i < this->primalSubalgebraModules.size; i ++) {
    theSel[this->primalSubalgebraModules[i]] = 1;
  }
  std::stringstream out;
  if (theSel.size != this->NilradicalPairingTable.size || theSel.size != this->ModulesIsotypicallyMerged.size) {
    global.fatal << "Selection has "
    << theSel.size << ", nilraidcal pairing table has " << this->NilradicalPairingTable.size
    << " elements and modules isotypically merged has " << this->ModulesIsotypicallyMerged.size
    << " elements." << global.fatal;
  }
  this->enumerateNilradicalsRecursively(theSel, &out);
  if (this->FKNilradicalCandidates.size < 1) {
    global.fatal << "While enumerating nilradicals of "
    << this->weylNonEmbedded->theDynkinType.toString()
    << " got 0 nilradical candidates which is impossible (the zero nilradical is always possible). " << global.fatal;
  }
  for (int i = 0; i < this->FKNilradicalCandidates.size; i ++) {
    std::stringstream reportStream2;
    reportStream2 << "Processing nilradical: " << i + 1 << " out of " << this->FKNilradicalCandidates.size;
    theReport2.report(reportStream2.str());
    this->FKNilradicalCandidates[i].processMe();
  }
  this->NumConeIntersections = 0;
  this->NumCasesNoLinfiniteRelationFound = 0;
  this->NumBadParabolics = 0;
  this->NumCentralizerConditionFailsconeConditionHolds = 0;
  for (int i = 0; i < this->FKNilradicalCandidates.size; i ++) {
    if (this->FKNilradicalCandidates[i].flagNilradicalConesIntersect) {
      this->NumConeIntersections ++;
      if (!this->FKNilradicalCandidates[i].flagLinfiniteRelFound) {
        this->NumCasesNoLinfiniteRelationFound ++;
      }
    } else {
      if (!this->FKNilradicalCandidates[i].flagParabolicACextendsToParabolicAC) {
        this->NumBadParabolics ++;
      }
      if (!this->FKNilradicalCandidates[i].flagRestrictedCentralizerConditionHoldS) {
        this->NumCentralizerConditionFailsconeConditionHolds ++;
      }
    }
  }
}

void Cone::getLinesContainedInCone(Vectors<Rational>& output) {
  output.setSize(0);
  for (int i = 0; i < this->vertices.size; i ++) {
    for (int j = i + 1; j < this->vertices.size; j ++) {
      if (this->vertices[i] == - this->vertices[j]) {
        output.addOnTop(this->vertices[i]);
      }
    }
  }
}

template<class Coefficient>
void Vector<Coefficient>::perturbNormalRelativeToVectorsInGeneralPosition(
  const Vectors<Rational>& nonStrictConeNonPositiveScalar, const List<Vector<Rational> >& vectorsToBeInGeneralPosition
) {
  MacroRegisterFunctionWithName("Vectors::PerturbSplittingNormal");
  for (int i = 0; i < nonStrictConeNonPositiveScalar.size; i ++) {
    if (this->scalarEuclidean(nonStrictConeNonPositiveScalar[i]) < 0) {
      global.fatal << "The splitting normal " << this->toString()
      << " is supposed to have non-negative scalar product with the vector "
      << nonStrictConeNonPositiveScalar[i].toString() << ", but it doesn't." << global.fatal;
    }
  }
  Vector<Rational> oldThis =*this;
  Cone theCone;
  theCone.createFromVertices(nonStrictConeNonPositiveScalar);
  Coefficient scalarThis;
  Coefficient scalarOther;
  Coefficient theScale;
  Vector<Rational> currentModifier;
  Vectors<Rational> allVectors = theCone.vertices;
  allVectors.addListOnTop(vectorsToBeInGeneralPosition);
  for (int i = 0; i < vectorsToBeInGeneralPosition.size; i ++) {
    if (this->scalarEuclidean(vectorsToBeInGeneralPosition[i]) == 0) {
      bool foundModifier = false;
      for (int j = 0; j < theCone.normals.size; j ++) {
        if (theCone.normals[j].scalarEuclidean(vectorsToBeInGeneralPosition[i]) != 0) {
          foundModifier = true;
          currentModifier = theCone.normals[j];
          break;
        }
      }
      if (!foundModifier) {
        continue;
      }
      theScale = 1;
      for (int j = 0; j < vectorsToBeInGeneralPosition.size; j ++) {
        scalarThis = this->scalarEuclidean(vectorsToBeInGeneralPosition[j]);
        scalarOther = currentModifier.scalarEuclidean(vectorsToBeInGeneralPosition[j]);
        if (scalarOther * scalarThis < 0) {
          theScale = MathRoutines::minimum(theScale, - (scalarThis / scalarOther) / 2);
        }
      }
    }
    *this += currentModifier * theScale;
    for (int i = 0; i < nonStrictConeNonPositiveScalar.size; i ++) {
      if (this->scalarEuclidean(nonStrictConeNonPositiveScalar[i]) < 0) {
        global.fatal << "<br>During perturbation, the splitting normal " << this->toString()
        << " is supposed to have non-negative scalar product with the vector " << nonStrictConeNonPositiveScalar[i].toString()
        << ", but it doesn't." << global.fatal;
      } else {
        if (
          this->scalarEuclidean(nonStrictConeNonPositiveScalar[i]) == 0 &&
          oldThis.scalarEuclidean(nonStrictConeNonPositiveScalar[i]) > 0
        ) {
          global.fatal << "<br>During perturbation, the splitting normal " << this->toString()
          << " lost  positive scalar product with " << nonStrictConeNonPositiveScalar[i].toString() << "." << global.fatal;
        }
      }
    }
  }
//  return true;
}

void NilradicalCandidate::processMe() {
  this->checkInitialization();
  this->flagComputedRelativelyStrongIntersections = false;
  this->flagRestrictedCentralizerConditionHoldS = false;
  this->flagParabolicACextendsToParabolicAC = true;
  this->computeTheTwoCones();
  this->flagNilradicalConesIntersect = this->theNilradicalWeights.conesIntersect(
    this->theNilradicalWeights, this->theNonFKhws, &this->ConeIntersection, &this->ConeSeparatingNormal
  );
  this->flagLinfiniteRelFound = false;
  if (!this->flagNilradicalConesIntersect) {
    if (this->theNonFKhws.size == 0 && this->theNilradicalWeights.size == 0) {
      this->flagRestrictedCentralizerConditionHoldS = true;
      this->flagParabolicACextendsToParabolicAC = true;
      return;
    }
    this->computeParabolicACExtendsToParabolicAC();
    return;
  }
  this->flagNilradicalConesStronglyIntersect = this->theNilradicalWeights.conesIntersect(
    this->theNilradicalWeights, this->theNonFKhwsStronglyTwoSided, &this->ConeStrongIntersection, nullptr
  );
  if (this->flagNilradicalConesStronglyIntersect) {
    this->flagLinfiniteRelFound = this->tryFindingLInfiniteRelations();
  }
}

std::string CandidateSemisimpleSubalgebra::toStringNilradicalSelection(const List<int>& theSelection) {
  Vector<Rational> undecided, included, excluded, tempV;
  included.makeZero(this->NilradicalPairingTable.size);
  undecided.makeZero(this->NilradicalPairingTable.size);
  excluded.makeZero(this->NilradicalPairingTable.size);
  for (int i = 0; i < theSelection.size; i ++) {
    tempV.makeEi(this->NilradicalPairingTable.size, i);
    if (theSelection[i] == 0) {
      excluded += tempV;
    } else if (theSelection[i] == 1) {
      included += tempV;
    } else {
      undecided += tempV;
    }
  }
  std::stringstream out;
  out << "included modules: " << included.toStringLetterFormat("V")
  << ", excluded modules: " << excluded.toStringLetterFormat("V")
  << ", undecided modulues: " << undecided.toStringLetterFormat("V");
  return out.str();
}

bool CandidateSemisimpleSubalgebra::isPossibleNilradicalCarryOutSelectionImplications(
  List<int>& theSelection, std::stringstream* logStream
) {
  if (this->FKNilradicalCandidates.size > 100) {
    if (logStream != nullptr) {
      std::string tempS = logStream->str();
      if (tempS.size() > 2) {
        if (tempS[tempS.size() - 1] != '.' || tempS[tempS.size() - 2] != '.') {
          *logStream << "<br>... number of nilradicals found exceeds 100, I will no longer log the computations ...";
        }
      }
    }
    logStream = nullptr;
  }
  if (logStream != nullptr) {
    *logStream << "<br>********Considering selection " << this->toStringNilradicalSelection(theSelection) << "********";
  }
  HashedList<int, MathRoutines::IntUnsignIdentity> selectedIndices;
  for (int i = 0; i < theSelection.size; i ++) {
    if (theSelection[i] == 1) {
      selectedIndices.addOnTop(i);
    }
  }
  MemorySaving<List<int> > oldSelection;
  if (logStream != nullptr) {
    oldSelection.getElement() = theSelection;
  }
  this->extendNilradicalSelectionToMultFreeOverSemisimplePartSubalgebra(selectedIndices);
  for (int i = 0; i < theSelection.size; i ++) {
    if (theSelection[i] == 0 && selectedIndices.contains(i)) {
      if (logStream != nullptr) {
        *logStream << " <br>The selection " << this->toStringNilradicalSelection(theSelection)
        << " is contradictory, as the only way to extend it to a subalgebra "
        << "(i.e., closed under Lie bracket) is by requesting that module V_{"
        << i + 1 << "} be included, but at the same time we have already "
        << "decided to exclude that module in one of our preceding choices. ";
      }
      return false;
    }
  }
  for (int i = 0; i <selectedIndices.size; i ++) {
    theSelection[selectedIndices[i]] = 1;
  }
  if (logStream != nullptr) {
    if (oldSelection.getElement() != theSelection) {
      *logStream << "<br>In order to be closed w.r.t. the Lie bracket, I extend the nilradical selection "
      << this->toStringNilradicalSelection(oldSelection.getElement()) << " to the following. "
      << this->toStringNilradicalSelection(theSelection);
    }
  }
  for (int i = 0; i <selectedIndices.size; i ++) {
    for (int j = 0; j < this->OppositeModulesByStructure[selectedIndices[i]].size; j ++) {
      if (theSelection[this->OppositeModulesByStructure[selectedIndices[i]][j]] == 1) {
        if (logStream != nullptr) {
          *logStream << "<br>The subalgebra selection " << this->toStringNilradicalSelection(theSelection)
          << " contains opposite modules and is therefore not allowed. ";
        }
        return false;
      }
      theSelection[this->OppositeModulesByStructure[selectedIndices[i]][j]] = 0;
    }
  }
  return true;
}

void CandidateSemisimpleSubalgebra::extendNilradicalSelectionToMultFreeOverSemisimplePartSubalgebra(
  HashedList<int, MathRoutines::IntUnsignIdentity>& inputOutput
) {
  for (int i = 0; i < inputOutput.size; i ++) {
    for (int j = 0; j < inputOutput.size; j ++) {
      for (int k = 0; k < this->NilradicalPairingTable[inputOutput[i] ][inputOutput[j]].size; k ++) {
        inputOutput.addOnTopNoRepetition(this->NilradicalPairingTable[inputOutput[i] ][inputOutput[j]][k]);
      }
    }
  }
}

void CandidateSemisimpleSubalgebra::enumerateNilradicalsRecursively(List<int>& theSelection, std::stringstream* logStream) {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::enumerateNilradicalsRecursively");
  RecursionDepthCounter theCounter(&this->RecursionDepthCounterForNilradicalGeneration);
  if (this->RecursionDepthCounterForNilradicalGeneration>this->NilradicalPairingTable.size + 1) {
    global.fatal << "<br>oh no... something went very wrong! The nilradical generation recursion "
    << "depth cannot exceed the number of nilradicals! "
    << global.fatal;
  }
  ProgressReport theReport;
  if (theReport.tickAndWantReport()) {
    std::stringstream out;
    out << "Enumerating nilradicals: " << this->FKNilradicalCandidates.size << " found so far. ";
    theReport.report(out.str());
  }
  if (!this->isPossibleNilradicalCarryOutSelectionImplications(theSelection, logStream)) {
    return;
  }
  List<int> newSelection;
  bool found = false;
  for (int i = 0; i < theSelection.size; i ++) {
    if (theSelection[i] == 2) {
      found = true;
      newSelection = theSelection;
      newSelection[i] = 0;
      this->enumerateNilradicalsRecursively(newSelection, logStream);
      newSelection = theSelection;
      newSelection[i] = 1;
      this->enumerateNilradicalsRecursively(newSelection, logStream);
      break;
      //newSelection
    }
  }
  if (found) {
    return;
  }
  NilradicalCandidate tempCandidate;
  tempCandidate.owner = this;
  tempCandidate.theNilradicalSelection = theSelection;
  this->FKNilradicalCandidates.addOnTop(tempCandidate);
}

void CandidateSemisimpleSubalgebra::computePrimalModuleDecompositionHighestWeightsOnly(HashedList<Vector<Rational> >& outputHWsDualCoords) {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::computePrimalModuleDecompositionHighestWeightsOnly");
  outputHWsDualCoords.clear();
  Vector<Rational> currentWeight, currentRootSpace;
  for (int i = 0; i < this->highestVectorsNonSorted.size; i ++) {
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& currentVector = this->highestVectorsNonSorted[i];
    for (int j = 0; j< currentVector.size(); j ++) {
      currentRootSpace = this->getAmbientSemisimpleLieAlgebra().getWeightOfGenerator(currentVector[j].generatorIndex);
      currentWeight.setSize(this->theHs.size + this->CartanOfCentralizer.size);
      for (int k = 0; k < this->theHs.size; k ++) {
        currentWeight[k] = this->getAmbientWeyl().rootScalarCartanRoot(currentRootSpace, this->theHs[k]);
      }
      for (int k = 0; k < this->CartanOfCentralizer.size; k ++) {
        currentWeight[k+this->theHs.size] = this->getAmbientWeyl().rootScalarCartanRoot(
          currentRootSpace, this->CartanOfCentralizer[k]
        );
      }
      outputHWsDualCoords.addOnTopNoRepetition(currentWeight);
    }
  }
  outputHWsDualCoords.quickSortAscending();
}

void CandidateSemisimpleSubalgebra::computePrimalModuleDecompositionHWsHWVsOnly() {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::computePrimalModuleDecompositionHWsHWVsOnly");
  HashedList<Vector<Rational> > theWeightsCartanRestrictedDualCoords;
  this->computePrimalModuleDecompositionHighestWeightsOnly(theWeightsCartanRestrictedDualCoords);
  this->computePrimalModuleDecompositionHWVsOnly(theWeightsCartanRestrictedDualCoords);
  this->computePrimalModuleDecompositionHWsHWVsOnlyLastPart();
}

bool CandidateSemisimpleSubalgebra::compareLeftGreaterThanRight(const Vector<Rational>& left, const Vector<Rational>& right) {
  Vector<Rational> leftSSpart = left;
  Vector<Rational> rightSSpart =right;
  leftSSpart.setSize(this->theHs.size);
  rightSSpart.setSize(this->theHs.size);
  if (leftSSpart > rightSSpart) {
    return true;
  }
  if (rightSSpart > leftSSpart) {
    return false;
  }
  Vector<Rational> leftCpart = left;
  Vector<Rational> rightCpart = right;
  leftCpart.shiftToTheLeft(this->theHs.size);
  rightCpart.shiftToTheLeft(this->theHs.size);
  return leftCpart > rightCpart;
}

void CandidateSemisimpleSubalgebra::getWeightProjectionFundamentalCoordinates(
  const Vector<Rational>& inputAmbientweight, Vector<Rational>& output
) const {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::getWeightProjectionFundamentalCoordinates");
  this->checkFullInitialization();
  output.setSize(this->theHs.size);
  for (int j = 0; j < this->theHs.size; j ++) {
    output[j] = this->getAmbientWeyl().rootScalarCartanRoot(inputAmbientweight, this->theHs[j]) * 2 /
    this->theSubalgebraNonEmbeddedDefaultScale->theWeyl.cartanSymmetric(j, j);
  }
}

void CandidateSemisimpleSubalgebra::getPrimalWeightProjectionFundamentalCoordinates(
  const Vector<Rational>& inputAmbientweight, Vector<Rational>& output
) const {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::getPrimalWeightProjectionFundamentalCoordinates");
  this->checkFullInitialization();
  output.setSize(this->theHs.size +this->CartanOfCentralizer.size);
  for (int j = 0; j < this->theHs.size; j ++) {
    output[j] = this->getAmbientWeyl().rootScalarCartanRoot(inputAmbientweight, this->theHs[j]) * 2 /
    this->theSubalgebraNonEmbeddedDefaultScale->theWeyl.cartanSymmetric(j, j);
  }
  for (int j = 0; j < this->CartanOfCentralizer.size; j ++) {
    output[j + this->theHs.size] = this->getAmbientWeyl().rootScalarCartanRoot(
      inputAmbientweight, this->CartanOfCentralizer[j]
    ) * 2;
  }
}

void CandidateSemisimpleSubalgebra::computePrimalModuleDecompositionHWsHWVsOnlyLastPart() {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::computePrimalModuleDecompositionHWsHWVsOnlyLastPart");
  this->HighestWeightsPrimalNonSorted.setSize(this->highestVectorsNonSorted.size);
  this->HighestWeightsNONprimalNonSorted.setSize(this->highestVectorsNonSorted.size);
  this->thePrimalChaR.makeZero();
  Weight<Rational> theWeight;
  theWeight.owner = nullptr;
  Vector<Rational> currentRoot;
  List<Vector<Rational> > sortingWeights;
  for (int i = 0; i < this->highestVectorsNonSorted.size; i ++) {
    currentRoot = this->getAmbientSemisimpleLieAlgebra().getWeightOfGenerator(this->highestVectorsNonSorted[i][0].generatorIndex);
    Vector<Rational>& currentHWPrimal = this->HighestWeightsPrimalNonSorted[i];
    this->getPrimalWeightProjectionFundamentalCoordinates(currentRoot, currentHWPrimal);
    sortingWeights.addOnTop(currentHWPrimal);
  }
  sortingWeights.quickSortAscendingCustom(*this, &this->highestVectorsNonSorted);
  List<List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > > tempModules;
  HashedList<Vector<Rational> > tempHWs;
  tempModules.setExpectedSize(this->highestVectorsNonSorted.size);
  tempHWs.setExpectedSize(this->highestVectorsNonSorted.size);
  for (int i = 0; i < this->highestVectorsNonSorted.size; i ++) {
    currentRoot = this->getAmbientSemisimpleLieAlgebra().getWeightOfGenerator(this->highestVectorsNonSorted[i][0].generatorIndex);
    Vector<Rational>& currentHWrelative = this->HighestWeightsNONprimalNonSorted[i];
    Vector<Rational>& currentHWPrimal = this->HighestWeightsPrimalNonSorted[i];
    this->getWeightProjectionFundamentalCoordinates(currentRoot, currentHWrelative);
    this->getPrimalWeightProjectionFundamentalCoordinates(currentRoot, currentHWPrimal);
    theWeight.weightFundamentalCoordS = currentHWPrimal;
    this->thePrimalChaR.addMonomial(theWeight, 1);
    int theIndex = tempHWs.getIndex(currentHWPrimal);
    if (theIndex == - 1) {
      tempModules.setSize(tempModules.size + 1);
      tempModules.lastObject()->setSize(0);
      theIndex = tempHWs.size;
      tempHWs.addOnTop(currentHWPrimal);
    }
    tempModules[theIndex].addOnTop(this->highestVectorsNonSorted[i]);
  }
  this->highestVectors.setExpectedSize(this->thePrimalChaR.size() +this->owner->owner->getRank());
  this->HighestWeightsPrimal.setExpectedSize(this->thePrimalChaR.size() +this->owner->owner->getRank());
  this->modules.setExpectedSize(this->thePrimalChaR.size() +this->owner->owner->getRank());
  this->highestVectors.setSize(0);
  this->HighestWeightsPrimal.clear();
  this->modules.setSize(0);
  this->primalSubalgebraModules.setSize(0);
  for (int i = 0; i < tempModules.size; i ++) {
    this->modules.setSize(this->modules.size + 1);
    this->highestVectors.setSize(this->modules.size);
    this->highestVectors.lastObject()->setSize(0);
    this->HighestWeightsPrimal.addOnTop(tempHWs[i]);
    if (LinearCombination<ChevalleyGenerator, Rational>::vectorSpacesIntersectionIsNonTrivial(tempModules[i], this->theBasis)) {
      LinearCombination<ChevalleyGenerator, AlgebraicNumber>::intersectVectorSpaces(
        tempModules[i], this->theBasis, *this->highestVectors.lastObject()
      );
      if (this->highestVectors.lastObject()->size != 1) {
        global.fatal << "Simple component "
        << "computed to have more than one highest weight vector. " << global.fatal;
      }
      this->primalSubalgebraModules.addOnTop(this->modules.size- 1);
      this->modules.lastObject()->setSize(1);
      *this->modules.lastObject()->lastObject()=*this->highestVectors.lastObject();
      if (tempModules[i].size >1) {
        this->modules.setSize(this->modules.size + 1);
        this->modules.lastObject()->setSize(tempModules[i].size- 1);
        this->highestVectors.setSize(this->modules.size);
        this->highestVectors.lastObject()->setSize(0);
        this->HighestWeightsPrimal.addOnTop(tempHWs[i]);
        this->highestVectors.lastObject()->addListOnTop(this->highestVectors[this->highestVectors.size-2]);
        for (int j = 0; j < tempModules[i].size; j ++) {
          this->highestVectors.lastObject()->addOnTop(tempModules[i][j]);
          if (
            LinearCombination<ChevalleyGenerator, Rational>::getRankOfSpanOfElements(*this->highestVectors.lastObject()) <
            this->highestVectors.lastObject()->size
          ) {
            this->highestVectors.lastObject()->removeLastObject();
          }
        }
        this->highestVectors.lastObject()->removeIndexSwapWithLast(0);
        if (this->highestVectors.lastObject()->size != tempModules[i].size - 1) {
          global.fatal << "Wrong number of hwv's: got "
          << this->highestVectors.lastObject()->size << ", must have "
          << tempModules[i].size- 1 << ". " << global.fatal;
        }
        for (int j = 0; j < this->highestVectors.lastObject()->size; j ++) {
          (*this->modules.lastObject())[j].setSize(1);
          (*this->modules.lastObject())[j][0] =(*this->highestVectors.lastObject())[j];
        }
      }
    } else {
      *this->highestVectors.lastObject()= tempModules[i];
      this->modules.lastObject()->setSize(tempModules[i].size);
      for (int j = 0; j < this->highestVectors.lastObject()->size; j ++) {
        (*this->modules.lastObject())[j].setSize(1);
        (*this->modules.lastObject())[j][0] = (*this->highestVectors.lastObject())[j];
      }
    }
  }
  this->HighestWeightsNONPrimal.setSize(this->modules.size);
  for (int i = 0; i < this->modules.size; i ++) {
    this->HighestWeightsNONPrimal[i] = this->getNonPrimalWeightFirstGenerator(this->modules[i][0][0]);
  }
  this->subalgebraModules = this->primalSubalgebraModules;
  this->charFormaT.getElement().customPlusSign = "\\oplus ";
  int theRank = this->weylNonEmbedded->getDimension();
  this->charFormaT.getElement().vectorSpaceEiBasisNames.setSize(theRank+this->CartanOfCentralizer.size);
  for (int i = 0; i < this->charFormaT.getElement().vectorSpaceEiBasisNames.size; i ++) {
    std::stringstream tempStream;
    if (i < theRank) {
      tempStream << "\\omega_{" << i + 1 << "}";
    } else {
      tempStream << "\\psi";
      if (this->CartanOfCentralizer.size > 1) {
        tempStream << "_{" << i - theRank + 1 << "}";
      }
    }
    this->charFormaT.getElement().vectorSpaceEiBasisNames[i] = tempStream.str();
  }
  int numMods = 0;
  for (int i = 0; i < this->modules.size; i ++) {
    numMods += this->modules[i].size;
  }
  for (int i = 0; i < this->modules.size; i ++) {
    for (int j = 0; j < this->modules[i].size; j ++) {
      if (this->modules[i][j].size != 1) {
        global.fatal << "Empty module! " << global.fatal;
      }
    }
  }
  if (this->thePrimalChaR.getCoefficientsSum() != numMods) {
    global.fatal << "The sum of the coeffs of the primal char is "
    << this->thePrimalChaR.getCoefficientsSum().toString()
    << " but there are  " << numMods << " modules. Tempmodules variable: "
    << tempModules.toString() << "<br>Candidate details: " << this->toString() << global.fatal;
  }
}

void CandidateSemisimpleSubalgebra::computePrimalModuleDecompositionHWVsOnly(HashedList<Vector<Rational> >& inputHws) {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::computePrimalModuleDecompositionHWVsOnly");
  this->checkConsistency();
  List<Matrix<AlgebraicNumber> > theAdsOfHs;
  Matrix<AlgebraicNumber> tempAd, temp, commonAd, adIncludingCartanActions;
  for (int i = 0; i < this->thePosGens.size; i ++) {
    this->getAmbientSemisimpleLieAlgebra().getAdjoint(tempAd, this->thePosGens[i]);
    commonAd.appendMatrixToTheBottom(tempAd);
  }
  ElementSemisimpleLieAlgebra<AlgebraicNumber> tempElt;
  Vectors<Rational> allHs;
  allHs.addListOnTop(this->theHs);
  allHs.addListOnTop(this->CartanOfCentralizer);
  theAdsOfHs.setSize(allHs.size);
  for (int j = 0; j < allHs.size; j ++) {
    tempElt.makeCartanGenerator(allHs[j], this->getAmbientSemisimpleLieAlgebra());
    this->getAmbientSemisimpleLieAlgebra().getAdjoint(theAdsOfHs[j], tempElt);
  }
  Vectors<AlgebraicNumber> outputV;
  this->highestVectorsNonSorted.setSize(0);
  for (int i = 0; i < inputHws.size; i ++) {
    adIncludingCartanActions = commonAd;
    for (int j = 0; j < allHs.size; j ++) {
      tempAd = theAdsOfHs[j];
      temp.makeIdentityMatrix(this->getAmbientSemisimpleLieAlgebra().getNumberOfGenerators());
      temp *= inputHws[i][j];
      tempAd -= temp;
      adIncludingCartanActions.appendMatrixToTheBottom(tempAd);
    }
    adIncludingCartanActions.getZeroEigenSpace(outputV);
    for (int j = 0; j < outputV.size; j ++) {
      outputV[j].scaleNormalizeFirstNonZero();
      tempElt.assignVectorNegRootSpacesCartanPosRootSpaces(outputV[j], this->getAmbientSemisimpleLieAlgebra());
      this->highestVectorsNonSorted.addOnTop(tempElt);
    }
  }
}

bool SemisimpleSubalgebras::checkConsistencyHs() const {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::checkConsistencyHs");
  this->checkInitialization();
  for (int i = 0; i < this->theSubalgebras.theValues.size; i ++) {
    if (this->theSubalgebras.theKeys[i] != this->theSubalgebras.theValues[i].theHs) {
      global.fatal << "List this->theHsOfSubalgebras does not match this->theSubalgebras" << global.fatal;
    }
  }
  return true;
}

bool SemisimpleSubalgebras::checkInitialization() const {
  this->checkConsistency();
  if (this->owner == nullptr) {
    global.fatal << "No owner semisimple Lie algebra. " << global.fatal;
  }
  if (this->theSubalgebrasNonEmbedded == nullptr) {
    global.fatal << "No container for non-embedded subalgebras. " << global.fatal;
  }
  return true;
}

bool SemisimpleSubalgebras::checkConsistency() const {
  if (this->flagDeallocated) {
    global.fatal << "Use after free of semisimple subalgebras. " << global.fatal;
  }
  return true;
}

void SemisimpleSubalgebras::initHookUpPointers(
  SemisimpleLieAlgebra& inputOwner,
  AlgebraicClosureRationals* theField,
  MapReferences<DynkinType, SemisimpleLieAlgebra>* inputSubalgebrasNonEmbedded,
  ListReferences<SlTwoSubalgebras>* inputSl2sOfSubalgebras
) {
  (void) inputSl2sOfSubalgebras;
  this->owner = &inputOwner;
  this->theSl2s.owner = &inputOwner;
  this->ownerField = theField;
  this->theSubalgebrasNonEmbedded = inputSubalgebrasNonEmbedded;
}

void SemisimpleSubalgebras::reset() {
  this->ToStringExpressionString = nullptr;
  this->owner = nullptr;
  this->ownerField = nullptr;
  this->theSl2s.owner = nullptr;
  this->flagRealizedAllCandidates = true;
  this->flagAttemptToSolveSystems = true;
  this->flagcomputeModuleDecompositionsition = true;
  this->flagcomputePairingTable = false;
  this->flagComputeNilradicals = false;
  this->flagProduceLaTeXtables = false;
  this->flagAttemptToAdjustCentralizers = true;
  this->millisecondsComputationStart = - 1;
  this->millisecondsComputationEnd = - 1;
  this->numAdditions = 0;
  this->numMultiplications = 0;
  this->theSubalgebrasNonEmbedded = nullptr;
}

bool CandidateSemisimpleSubalgebra::attemptToSolveSystem() {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::attemptToSolveSystem");
  this->checkFullInitialization();
  this->transformedSystem = this->theSystemToSolve;
  ProgressReport theReport;
  std::stringstream reportstream;
  reportstream << "<hr>In order to realize type " << this->weylNonEmbedded->theDynkinType.toString()
  << " I need to solve the following system." << this->toStringSystemPart2();
  theReport.report(reportstream.str());
  PolynomialSystem<AlgebraicNumber> system;
  system.groebner.polynomialOrder.monomialOrder.setComparison(MonomialP::greaterThan_totalDegree_rightSmallerWins);
  for (int i = 500; i < 200000; i += 100000) {
    system.groebner.maximumPolynomialComputations = i;
    system.maximumSerreSystemComputationsPreferred = i;
    system.theAlgebraicClosure = this->owner->ownerField;
    system.solveSerreLikeSystem(this->transformedSystem);
    if (system.flagSystemProvenToHaveNoSolution || system.flagSystemProvenToHaveSolution) {
      break;
    }
  }
  this->flagSystemSolved = system.flagSystemSolvedOverBaseField;
  this->flagSystemProvedToHaveNoSolution = system.flagSystemProvenToHaveNoSolution;
  this->flagSystemGroebnerBasisFound = this->flagSystemSolved;
  if (this->flagSystemSolved) {
    this->theNegGens.setSize(this->theUnknownNegGens.size);
    this->thePosGens.setSize(this->theUnknownPosGens.size);
    PolynomialSubstitution<AlgebraicNumber> theSub;
    theSub.setSize(system.systemSolution.size);
    for (int i = 0; i < theSub.size; i ++) {
      theSub[i].makeConstant(system.systemSolution[i]);
    }
    ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > currentNegElt;
    ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > currentPosElt;
    for (int i = 0; i < this->theUnknownNegGens.size; i ++) {
      currentNegElt = this->theUnknownNegGens[i];
      currentPosElt = this->theUnknownPosGens[i];
      currentNegElt.substitutionCoefficients(theSub);
      currentPosElt.substitutionCoefficients(theSub);
      this->theNegGens[i] = currentNegElt;//<-implicit type conversion here, will crash if currentNegElt has non-const coefficients
      this->thePosGens[i] = currentPosElt;//<-implicit type conversion here, will crash if currentNegElt has non-const coefficients
    }
    if (!this->checkGeneratorsBracketToHs()) {
      global.fatal << "I just solved the "
      << "Serre-Like system governing the subalgebra embedding, but the Lie brackets of the "
      << "resulting positive and negative generators are not what they should be. "
      << "Something has gone very wrong. " << global.fatal;
    }
  } else {
    //if (this->flagSystemProvedToHaveNoSolution)
    //  global.Comments << "System " << this->transformedSystem.toString() << " <b> proven contradictory, good. </b>";
    //else
    //  global.Comments << "System " << this->transformedSystem.toString() << " <b> not solved! </b>";
  }
  return !this->flagSystemProvedToHaveNoSolution;
}

void CandidateSemisimpleSubalgebra::getGenericNegativeGeneratorLinearCombination(
  int indexNegGens, ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& output
) {
  int offsetIndex = 0;
  for (int i = 0; i < indexNegGens; i ++) {
    offsetIndex += this->theInvolvedNegGenerators[i].size;
  }
  this->getGenericLinearCombination(
    this->totalNumUnknownsWithCentralizer, offsetIndex, this->theInvolvedNegGenerators[indexNegGens], output
  );
}

void CandidateSemisimpleSubalgebra::getGenericCartanCentralizerLinearCombination(
  int indexCartanCentralizerGen, ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& output
) {
  int offsetIndex = this->totalNumUnknownsNoCentralizer+ indexCartanCentralizerGen*this->getAmbientWeyl().getDimension() ;
  List<ChevalleyGenerator> eltsCartan;
  eltsCartan.setSize(this->getAmbientWeyl().getDimension());
  for (int i = 0; i < eltsCartan.size; i ++) {
    eltsCartan[i].generatorIndex = this->getAmbientSemisimpleLieAlgebra().getNumberOfPositiveRoots() + i;
    eltsCartan[i].owner = &this->getAmbientSemisimpleLieAlgebra();
  }
  this->getGenericLinearCombination(this->totalNumUnknownsWithCentralizer, offsetIndex, eltsCartan, output);
}

void CandidateSemisimpleSubalgebra::getGenericPositiveGeneratorLinearCombination(
  int indexPosGens, ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& output
) {
  int offsetIndex = 0;
  for (int i = 0; i < indexPosGens; i ++) {
    offsetIndex += this->theInvolvedPosGenerators[i].size;
  }
  offsetIndex += this->totalNumUnknownsNoCentralizer / 2;
  this->getGenericLinearCombination(
    this->totalNumUnknownsWithCentralizer, offsetIndex, this->theInvolvedPosGenerators[indexPosGens], output
  );
}

void CandidateSemisimpleSubalgebra::addToSystem(const ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& elementThatMustVanish) {
  Polynomial<AlgebraicNumber> thePoly;
  for (int i = 0; i < elementThatMustVanish.size(); i ++) {
    thePoly = elementThatMustVanish.coefficients[i];
    thePoly.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
    this->theSystemToSolve.addOnTopNoRepetition(thePoly);
  }
}

void CandidateSemisimpleSubalgebra::getGenericLinearCombination(
  int numVars,
  int varOffset,
  List<ChevalleyGenerator>& involvedGens,
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& output
) {
  Polynomial<AlgebraicNumber> theCF;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > tempMon;
  output.makeZero();
  for (int i = 0; i < involvedGens.size; i ++) {
    theCF.makeDegreeOne(numVars, varOffset + i, 1);
    tempMon.makeGenerator(involvedGens[i].generatorIndex, *this->owner->owner);
    tempMon*= theCF;
    output += tempMon;
  }
}

bool CandidateSemisimpleSubalgebra::computeCharacter(bool allowBadCharacter) {
  if (this->indexNonEmbeddedMeStandard == - 1) {
    global.fatal << "Attempting to "
    << "compute char of candidate subalgebra that has not been initialized properly. " << global.fatal;
  }
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::computeCharacter");
  this->checkCandidateInitialization();
  this->weylNonEmbedded->computeRho(true);
  Weight<Rational> tempMon;
  tempMon.weightFundamentalCoordS.makeZero(this->weylNonEmbedded->getDimension());
  tempMon.owner = nullptr;
  this->theCharFundamentalCoordsRelativeToCartan.makeZero();
  this->theCharFundamentalCoordsRelativeToCartan.addMonomial(tempMon, this->getAmbientSemisimpleLieAlgebra().getRank());
  List<DynkinSimpleType> theTypes;
  this->weylNonEmbedded->theDynkinType.getTypesWithMults(theTypes);
/*  global.Comments << "<br>Cartan symmetric, type  "
  << this->weylNonEmbedded->theDynkinType.toString() << " <br>"
  << this->weylNonEmbedded->cartanSymmetric.toString()
  << "<br> Co-Cartan symmetric: "
  << this->weylNonEmbedded->coCartanSymmetric.toString()
  << "<br>Cartan by compo: " << this->cartanSubalgebrasByComponentScaledToActByTwo.toString()
  << "<br> hs scaled to act by two: " << this->theHsScaledToActByTwo.toString()
  << "<br> hs non-scaled: " << this->theHs.toString();*/
  Matrix<Rational> coCartanCandidate;
  this->theHsScaledToActByTwo.getGramMatrix(coCartanCandidate, &this->getAmbientWeyl().cartanSymmetric);
  if (coCartanCandidate != this->weylNonEmbedded->coCartanSymmetric) {
    if (!allowBadCharacter) {
      global.fatal << "This is a mathematical error. The co-Cartan symmetric matrix is: "
      << this->weylNonEmbedded->coCartanSymmetric.toString()
      << " but co-Cartan of the computed candidate elements of the Cartan, "
      << this->theHsScaledToActByTwo.toString()
      << " is different, namely, " << coCartanCandidate.toString();
    }
  }
  for (int k = 0; k < this->getAmbientWeyl().rootSystem.size; k ++) {
    int counter = - 1;
    for (int i = 0; i < this->cartanSubalgebrasByComponentScaledToActByTwo.size; i ++) {
      for (int j = 0; j < this->cartanSubalgebrasByComponentScaledToActByTwo[i].size; j ++) {
        counter ++;
        tempMon.weightFundamentalCoordS[counter] = (
          this->getAmbientWeyl().rootScalarCartanRoot(this->getAmbientWeyl().rootSystem[k], this->theHsScaledToActByTwo[counter])
        );
        if (!tempMon.weightFundamentalCoordS[counter].isInteger()) {
          if (!allowBadCharacter) {
            global.fatal << "Function computeCharacter "
            << "called with Cartan that suggests non-integral characters. At "
            << "the same time, an option banning this possibility has been explicitly selected. " << global.fatal;
          }
          return false;
        }
      }
    }
    this->theCharFundamentalCoordsRelativeToCartan.addMonomial(tempMon, 1);
  }
  CharacterSemisimpleLieAlgebraModule<Rational> accumChar, freudenthalChar, outputChar;
  accumChar = this->theCharFundamentalCoordsRelativeToCartan;
  SemisimpleLieAlgebra* nonEmbeddedMe =
  & (*this->owner->theSubalgebrasNonEmbedded).theValues[this->indexNonEmbeddedMeStandard];
  this->theCharNonPrimalFundCoords.makeZero();

  while (accumChar.size() > 0) {
    int currentIndex = accumChar.getIndexExtremeWeightRelativeToWeyl(*this->weylNonEmbedded);
    if (currentIndex == - 1) {
      global.fatal << "While decomposing ambient Lie algebra over the candidate subalgebra, I got "
      << "that there is no extreme weight. This is impossible: something has gone very wrong. " << global.fatal;
    }
    if (accumChar.coefficients[currentIndex] < 0) {
      return false;
    }
    for (int i = 0; i < accumChar[currentIndex].weightFundamentalCoordS.size; i ++) {
      if (accumChar[currentIndex].weightFundamentalCoordS[i] < 0) {
        return false;
      }
    }
    freudenthalChar.makeZero();
    tempMon =accumChar[currentIndex];
    tempMon.owner = nonEmbeddedMe;
    freudenthalChar.addMonomial(tempMon, accumChar.coefficients[currentIndex]);
    this->theCharNonPrimalFundCoords.addMonomial(accumChar[currentIndex], accumChar.coefficients[currentIndex]);
    std::string tempS;
    bool tempBool = freudenthalChar.freudenthalEvaluateMeFullCharacter(outputChar, - 1, &tempS);
    if (!tempBool && !allowBadCharacter) {
      global.fatal << "Failed to evaluate full character "
      << "via the Freudenthal formula on a relatively small example, namely "
      << freudenthalChar.toString() << ". The failure message was: "
      << tempS << ". This shouldn't happen. " << global.fatal;
      return false;
    }
    accumChar -= outputChar;
  }
  return true;
}

void SlTwoSubalgebra::toStringModuleDecompositionMinimalContainingRegularSAs(
  bool useLatex, bool useHtml, SlTwoSubalgebras& owner, std::string& output
) const {
  std::stringstream out;
  std::string tempS;
  if (useLatex) {
    out << "$";
  }
  if (useHtml) {
    out << "<table><tr><td align =\"center\">Char.</td>";
    for (int i = 0; i < this->IndicesMinimalContainingRootSAs.size; i ++) {
      RootSubalgebra& theSA = owner.theRootSAs.theSubalgebras[this->IndicesMinimalContainingRootSAs[i]];
      out << "<td align =\"center\">Decomp. "
      << theSA.theDynkinDiagram.toString() << "</td>";
    }
    out << "</tr>\n";
  }
  out << "<tr><td align =\"center\"> " << this->hCharacteristic.toString() << "</td>";
  for (int k = 0; k < this->IndicesMinimalContainingRootSAs.size; k ++) {
    RootSubalgebra& theSA = owner.theRootSAs.theSubalgebras[this->IndicesMinimalContainingRootSAs[k]];
    tempS = theSA.theDynkinDiagram.toString();
    if (useHtml) {
      out << "<td align =\"center\">";
    }
    out << this->moduleDecompositionMinimalContainingRootSAs[k].toString();
    if (useHtml) {
      out << "</td>";
    }
    out << "\n";
  }
  if (useHtml) {
    out << "</tr></table>";
  }
  if (useLatex) {
    out << "$";
  }
  output = out.str();
}

const WeylGroupData& SlTwoSubalgebra::getOwnerWeyl() const {
  if (this->owner == nullptr) {
    global.fatal << "Weyl group with non-initialized owner. " << global.fatal;
  }
  return this->owner->theWeyl;
}

bool SlTwoSubalgebra::operator>(const SlTwoSubalgebra& right) const {
  MacroRegisterFunctionWithName("SlTwoSubalgebra::operatorGT");
  if (this->owner != right.owner) {
    global.fatal << "Error: comparing sl(2) subalgebras with different owners." << global.fatal;
  }
  if ( this->LengthHsquared > right.LengthHsquared) {
    return true;
  }
  if (right.LengthHsquared > this->LengthHsquared) {
    return false;
  }
  return this->hCharacteristic > right.hCharacteristic;
}

bool SlTwoSubalgebra::operator==(const SlTwoSubalgebra& right) const {
 // See Dynkin, Semisimple Lie subalgebras of semisimple Lie algebras, chapter 7- 10
  if (this->owner != right.owner) {
    global.fatal << "Comparing sl(2) "
    << "subalgebras that have different ambient Lie algebras. " << global.fatal;
  }
  return this->hCharacteristic == right.hCharacteristic;
}

std::string SlTwoSubalgebra::toString(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("SlTwoSubalgebra::toString");
  if (this->container == nullptr) {
    return "sl(2) subalgebra not initialized.";
  }
  std::stringstream out;  std::string tempS;
  out << "<a name =\"sl2index" << indexInContainer << "\">h-characteristic: " << this->hCharacteristic.toString() << "</a>";
  out << "<br>Length of the weight dual to h: " << this->LengthHsquared;
  tempS = this->preferredAmbientSimpleBasis.toString();
  std::string virtualPath = "";
  std::string htmlPathServer = "";
  //bool usePNG = false;
  bool useHtml = true;
  bool useLatex = false;
  virtualPath = this->owner->toStringVirtualFolderName() + "sl2s/";
  htmlPathServer = this->owner->toStringDisplayFolderName("../../../") + "sl2s/";
  if (virtualPath == "" || htmlPathServer == "") {
    //usePNG = false;
    useHtml = false;
  }
  if (useHtml) {
    out << "<br>";
  }
  out << "\nSimple basis ambient algebra w.r.t defining h: " << tempS;
  if (useHtml) {
    out << "<br>";
  }
  if (this->IndicesContainingRootSAs.size > 1) {
    out << "Number of containing regular semisimple subalgebras: " << this->IndicesContainingRootSAs.size;
    if (useHtml) {
      out << "<br>";
    }
  }
  FormatExpressions localFormat, latexFormat;
  localFormat.flagUseHTML = useHtml;
  localFormat.flagUseLatex = useLatex;
  latexFormat.flagUseHTML = false;
  latexFormat.flagUseLatex = true;
  for (int i = 0; i < this->IndicesContainingRootSAs.size; i ++) {
    out << "\nContaining regular semisimple subalgebra number " << i + 1 << ": ";
    RootSubalgebra& currentSA = this->container->theRootSAs.theSubalgebras[this->IndicesContainingRootSAs[i]];
    if (useHtml) {
      out << "<a href=\"" << htmlPathServer << "../rootSubalgebra_" << this->IndicesContainingRootSAs[i] + 1 << ".html\">";
      tempS = currentSA.theDynkinDiagram.toString();
    }
    out << tempS;
    if (useHtml) {
      out << "</a>";
    }
  }
  if (useHtml) {
    out << "<br>";
  }
  out << "\nsl(2)-module decomposition of the ambient Lie algebra: ";
  FormatExpressions formatCharacter;
  formatCharacter.vectorSpaceEiBasisNames.addOnTop("\\psi");
  out << HtmlRoutines::getMathNoDisplay((this->moduleDecompositionAmbientSA.toString(&formatCharacter))) << "\n<br>\n";
  out << "\nBelow is one possible realization of the sl(2) subalgebra.";
  if (useHtml) {
    out << "\n<br>\n";
  }
  std::stringstream tempStreamH, tempStreamE, tempStreamF, tempStreamHE, tempStreamHF, tempStreamEF;
  tempS = this->theH.toString(theFormat);
  tempStreamH << "\nh = " << tempS << "";
  out << HtmlRoutines::getMathNoDisplay(tempStreamH.str()) << "\n<br>\n";
  tempStreamE << "\ne = " << this->theE.toString(theFormat) << "\n<br>\n";
  out << HtmlRoutines::getMathNoDisplay(tempStreamE.str()) << "\n<br>\n";
  tempStreamF << "\nf= " << this->theF.toString(theFormat) << "\n<br>\n";
/*  out << HtmlRoutines::getMathNoDisplay(tempStreamF.str()) << "\n<br>\n";
  out << "\nLie brackets of the above elements. Printed for debugging.";
  if (useHtml)
    out << "\n<br>\n";
  tempStreamEF << "\n[e, f] =" <<  this->bufferEbracketF.toString(theFormat) << "";
  out << HtmlRoutines::getMathNoDisplay(tempStreamEF.str()) << "\n<br>\n";
  tempStreamHE << "\n[h, e] =" << this->bufferHbracketE.toString(theFormat) << "";
  out << HtmlRoutines::getMathNoDisplay(tempStreamHE.str()) << "\n<br>\n";
  tempStreamHF << "\n[h, f] = " << this->bufferHbracketF.toString(theFormat) << "";
  out << HtmlRoutines::getMathNoDisplay(tempStreamHF.str()) << "\n<br>\n";*/
  //this->theSystemMatrixForm.toString(tempS);
  //out << "\nSystem matrix form we try to solve:\n" << tempS;
  //this->theSystemColumnVector.toString(tempS);
  //out << "\nColumn vector of the system:\n" <<tempS;
  std::stringstream latexStreamActual;
  latexStreamActual << "\\begin{array}{l}";
  for (int i = 0; i < this->theSystemToBeSolved.size; i ++) {
    latexStreamActual << this->theSystemToBeSolved[i].toString(theFormat) << "~\\\\";
  }
  latexStreamActual << "\\end{array}";
  out << "\nThe polynomial system that corresponds to finding the h, e, f triple:\n";
  if (useHtml) {
    out << "\n<br>\n";
  }
  out << HtmlRoutines::getMathNoDisplay(latexStreamActual.str()) << "\n<br>\n";
  return out.str();
}

void SlTwoSubalgebra::initialize() {
  this->owner = nullptr;
  this->container = nullptr;
  this->indexInContainer = - 1;
  this->flagCentralizerTypeComputed = false;
  this->flagCentralizerIsRegular = false;
  this->dimensionCentralizer = - 1;
}

bool SlTwoSubalgebra::checkConsistency() const {
  if (this->flagDeallocated) {
    global.fatal << "Use after free of SlTwoSubalgebra. " << global.fatal;
  }
  if (this->owner != nullptr) {
    this->owner->checkConsistency();
  }
  return true;
}

void SlTwoSubalgebra::computeDynkinsEpsilon(WeylGroupData& theWeyl) {
 //outdates, must be erased as soon as I implement an equivalent
  this->DynkinsEpsilon = this->DiagramM.numberRootsGeneratedByDiagram() + this->DiagramM.rankTotal();
  int r = 0;
  for (int i = 0; i < this->hCharacteristic.size; i ++) {
    if (!this->hCharacteristic[i].isEqualToZero()) {
      r ++;
    }
  }
  this->DynkinsEpsilon += r;
  for (int i = 0; i < theWeyl.rootSystem.size; i ++) {
    int NumNonZeroFound = 0;
    for (int j = 0; j < this->hCharacteristic.size; j ++) {
      if (theWeyl.rootSystem[i][j] == 1) {
        NumNonZeroFound ++;
      }
      if (NumNonZeroFound > 1) {
        break;
      }
    }
    if (NumNonZeroFound == 1) {
      this->DynkinsEpsilon --;
    }
  }
  this->DynkinsEpsilon = 0;
  for (int i = 0; i < this->hCharacteristic.size; i ++) {
    if (this->hCharacteristic[i] == 1) {
      this->DynkinsEpsilon = - 1;
    }
  }
  this->DynkinsEpsilon = 0;
}

bool SlTwoSubalgebra::moduleDecompositionFitsInto(const SlTwoSubalgebra& other) const {
  return this->moduleDecompositionLeftFitsIntoRight(
    this->moduleDecompositionAmbientSA, other.moduleDecompositionAmbientSA
  );
}

bool SlTwoSubalgebra::moduleDecompositionLeftFitsIntoRight(
  const CharacterSemisimpleLieAlgebraModule<Rational>& moduleDecompoLeft, const CharacterSemisimpleLieAlgebraModule<Rational>& moduleDecompoRight
) {
  MacroRegisterFunctionWithName("SlTwoSubalgebra::moduleDecompositionLeftFitsIntoRight");
  CharacterSemisimpleLieAlgebraModule<Rational> moduleDifference = moduleDecompoRight - moduleDecompoLeft;
  for (int i = 0; i < moduleDifference.size(); i ++) {
    if (moduleDifference.coefficients[i] < 0) {
      return false;
    }
  }
  return true;
}

void SlTwoSubalgebra::toHTML(std::string& filePath) {
  std::fstream theFile;
  std::string theFileName = filePath;
  theFileName.append("theSlTwo.txt");
  FileOperations::openFileCreateIfNotPresentVirtual(theFile, "output/" + filePath, false, true, false);
}

void SlTwoSubalgebras::reset(SemisimpleLieAlgebra& inputOwner) {
  MacroRegisterFunctionWithName("SlTwoSubalgebras::reset");
  this->IndicesSl2sContainedInRootSA.setSize(0);
  this->IndicesSl2decompositionFlas.setSize(0);
  this->BadHCharacteristics.setSize(0);
  this->IndexZeroWeight = - 1;
  this->owner = & inputOwner;
  this->theRootSAs.owner = &inputOwner;
}

void SemisimpleLieAlgebra::FindSl2Subalgebras(SemisimpleLieAlgebra& inputOwner, SlTwoSubalgebras& output) {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::FindSl2Subalgebras");
  ProgressReport theReport0;
  if (theReport0.tickAndWantReport()) {
    std::stringstream reportStream0;
    reportStream0 << "Finding sl(2)-subalgebras (and thus a full list of the nilpotent orbits) of "
    << inputOwner.theWeyl.theDynkinType.toString();
    theReport0.report(reportStream0.str());
  }
  inputOwner.checkConsistency();
  output.reset(inputOwner);
  output.checkConsistency();
  output.getOwner().computeChevalleyConstants();
  output.theRootSAs.owner = &inputOwner;
  output.theRootSAs.computeAllReductiveRootSubalgebrasUpToIsomorphism();
  //output.theRootSAs.ComputeDebugString(false, false, false, 0, 0, global);
  output.IndicesSl2sContainedInRootSA.setSize(output.theRootSAs.theSubalgebras.size);
  output.IndicesSl2sContainedInRootSA.reserve(output.theRootSAs.theSubalgebras.size * 2);
  for (int i = 0; i < output.IndicesSl2sContainedInRootSA.size; i ++) {
    output.IndicesSl2sContainedInRootSA[i].size = 0;
  }
  ProgressReport theReport;
  for (int i = 0; i < output.theRootSAs.theSubalgebras.size; i ++) {
    std::stringstream tempStream;
    tempStream << "\nExploring root subalgebra "
    << output.theRootSAs.theSubalgebras[i].theDynkinDiagram.toString()
    << " (" << (i + 1) << " out of " << output.theRootSAs.theSubalgebras.size << ")\n";
    theReport.report(tempStream.str());
    output.theRootSAs.theSubalgebras[i].getSsl2SubalgebrasAppendListNoRepetition(output, i);
  }
  //sort subalgebras by dynkin index
  List<int> thePermutation, theIndexMap;
  thePermutation.setSize(output.size);
  theIndexMap.setSize(thePermutation.size);
  for (int i = 0; i < thePermutation.size; i ++) {
    thePermutation[i] = i;
  }
  output.quickSortDescending(nullptr, &thePermutation);
  for (int i = 0; i < theIndexMap.size; i ++) {
    theIndexMap[thePermutation[i]] = i;
  }
  for (int j = 0; j < output.IndicesSl2sContainedInRootSA.size; j ++) {
    for (int k = 0; k < output.IndicesSl2sContainedInRootSA[j].size; k ++) {
      output.IndicesSl2sContainedInRootSA[j][k] = theIndexMap[output.IndicesSl2sContainedInRootSA[j][k]];
    }
  }
  inputOwner.checkConsistency();
  for (int i = 0; i <output.size; i ++) {
    //SlTwoSubalgebra& theSl2= output.getElement(i);
    output.getElement(i).IndicesMinimalContainingRootSAs.reserve(output.getElement(i).IndicesContainingRootSAs.size);
    output.getElement(i).IndicesMinimalContainingRootSAs.size = 0;
    for (int j = 0; j < output.getElement(i).IndicesContainingRootSAs.size; j ++) {
      bool isMinimalContaining = true;
//      RootSubalgebra& currentRootSA = output.theRootSAs.objects[];
      for (int k = 0; k < output.getElement(i).IndicesContainingRootSAs.size; k ++) {
        RootSubalgebra& theOtherRootSA = output.theRootSAs.theSubalgebras[output.getElement(i).IndicesContainingRootSAs[k]];
        if (theOtherRootSA.indicesSubalgebrasContainingK.contains(output.getElement(i).IndicesContainingRootSAs[j])) {
          isMinimalContaining= false;
          break;
        }
      }
      if (isMinimalContaining) {
        output.getElement(i).IndicesMinimalContainingRootSAs.addOnTopNoRepetition(output.getElement(i).IndicesContainingRootSAs[j]);
      }
    }
    output.checkConsistency();
    output.computeModuleDecompositionsitionsOfAmbientLieAlgebra();
  }
}

bool CandidateSemisimpleSubalgebra::checkConsistency() const {
  if (this->flagDeallocated) {
    global.fatal << "Use after free of CandidateSemisimpleSubalgebra. " << global.fatal;
  }
  return true;
}

bool CandidateSemisimpleSubalgebra::computeAndVerifyFromGeneratorsAndHs() {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::computeAndVerifyFromGeneratorsAndHs");
  if (!this->flagSubalgebraPreloadedButNotVerified) {
    return true;
  }
  Matrix<Rational> actualCoCartan;
  this->theHsScaledToActByTwo.getGramMatrix(actualCoCartan, &this->owner->getSemisimpleOwner().theWeyl.cartanSymmetric);
  std::stringstream out;
  this->flagSubalgebraPreloadedButNotVerified = false;
  if (!(this->weylNonEmbedded->coCartanSymmetric == actualCoCartan)) {
    out << "<b>Corrupt semisimple subalgebra: the gram matrix of the elements of its Cartan, "
    << this->theHsScaledToActByTwo.toString() << " is " << actualCoCartan.toString() << "; it should be "
    << this->weylNonEmbedded->coCartanSymmetric.toString() << ".</b>";
    this->flagSystemProvedToHaveNoSolution = true;
  }
  this->computeSystem(false, true);
  if (!this->computeCharacter(true)) {
    out << "<b>Corrupt semisimple subalgebra: the ambient Lie algebra does not decompose "
    << "properly over the candidate subalgebra. </b>";
    this->flagSystemProvedToHaveNoSolution = true;
  }
  if (!this->checkGeneratorsBracketToHs()) {
    out << "<b>Corrput semisimple subalgebra: Lie brackets of generators do not equal the desired elements of the Cartan. ";
    this->flagSystemProvedToHaveNoSolution = true;
  }
  this->comments = out.str();
  return !this->flagSystemProvedToHaveNoSolution;
}

bool CandidateSemisimpleSubalgebra::checkMaximalDominance() const {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::checkMaximalDominance");
  this->checkBasicInitialization();
  Rational theScalarProd;
  for (int i = 0; i < this->getAmbientWeyl().rootsOfBorel.size; i ++) {
    Vector<Rational>& currentPosRoot = this->getAmbientWeyl().rootsOfBorel[i];
    bool canBeRaisingReflection = true;
    for (int k = 0; k < this->cartanSubalgebrasByComponentScaledToActByTwo.size && canBeRaisingReflection; k ++) {
      for (int l = 0; l < this->cartanSubalgebrasByComponentScaledToActByTwo[k].size && canBeRaisingReflection; l ++) {
        theScalarProd = this->getAmbientWeyl().rootScalarCartanRoot(
          currentPosRoot, this->cartanSubalgebrasByComponentScaledToActByTwo[k][l]
        );
        if (theScalarProd > 0) {
          canBeRaisingReflection = false;
        }
        if (theScalarProd < 0) {
          global.fatal << "The candidate h elements "
          << "of the semisimple subalgebra are supposed to be maximally dominant, "
          << "however the scalar product of the positive root " << currentPosRoot.toString()
          << " with the subalgebra root "
          << this->cartanSubalgebrasByComponentScaledToActByTwo[k][l].toString()
          << " is negative, while the very same positive root has had zero scalar products with all "
          << "preceding roots. Here are all preceding roots: "
          << this->cartanSubalgebrasByComponentScaledToActByTwo.toString() << global.fatal;
          return false;
        }
      }
    }
  }
  return true;
}

bool SlTwoSubalgebras::checkConsistency() const {
  if (this->flagDeallocated) {
    global.fatal << "Use after free of SemisimpleLieAlgebra. " << global.fatal;
  }
  if (this->owner != nullptr) {
    this->owner->checkConsistency();
  }
  for (int i = 0; i < this->size; i ++) {
    (*this)[i].checkConsistency();
  }
  return true;
}

void SlTwoSubalgebras::computeModuleDecompositionsitionsOfAmbientLieAlgebra() {
  this->grandMasterConsistencyCheck();
  this->checkConsistency();
  for (int i = 0; i < this->size; i ++) {
    (*this).getElement(i).computeModuleDecompositionsitionAmbientLieAlgebra();
  }
  this->grandMasterConsistencyCheck();
}

bool SlTwoSubalgebras::containsSl2WithGivenH(Vector<Rational>& theH, int* outputIndex) {
  if (outputIndex != nullptr) {
    *outputIndex = - 1;
  }
  ElementSemisimpleLieAlgebra<Rational> tempH;
  this->checkInitialization();
  tempH.makeCartanGenerator(theH, *this->owner);
  for (int i = 0; i < this->size; i ++) {
    if (this->objects[i].theH == tempH) {
      if (outputIndex != nullptr) {
        *outputIndex = i;
      }
      return true;
    }
  }
  return false;
}

bool SlTwoSubalgebras::containsSl2WithGivenHCharacteristic(Vector<Rational>& theHCharacteristic, int* outputIndex) {
  if (outputIndex != nullptr) {
    *outputIndex = - 1;
  }
  for (int i = 0; i < this->size; i ++) {
    if ((*this)[i].hCharacteristic == theHCharacteristic) {
      if (outputIndex != nullptr) {
        *outputIndex = i;
      }
      return true;
    }
  }
  return false;
}

bool SlTwoSubalgebra::attemptToComputeCentralizer() {
  MacroRegisterFunctionWithName("SlTwoSubalgebra::attemptToComputeCentralizer");
  this->flagCentralizerIsRegular = false;
  this->flagCentralizerTypeComputed = false;
  Weight<Rational> zeroWeight;
  zeroWeight.weightFundamentalCoordS.makeZero(1);
  if (
    !this->moduleDecompositionAmbientSA.getCoefficientOf(zeroWeight).isSmallInteger(
      &this->dimensionCentralizer
  )) {
    global.fatal << "Dimension of centralizer of sl(2) subalgebra is not a small integer. This shouldn't happen. " << global.fatal;
  }
  for (int i = 0; i < this->IndicesMinimalContainingRootSAs.size; i ++) {
    RootSubalgebra& currentMinimalContainer =
    this->container->theRootSAs.theSubalgebras[this->IndicesMinimalContainingRootSAs[i]];
    Rational dimOfSSpartOfCentralizerOfRootSA =
    currentMinimalContainer.theCentralizerDynkinType.getRankRational() +
    currentMinimalContainer.theCentralizerDynkinType.getRootSystemSize();
    this->dimCentralizerToralPart =
    this->owner->getRank() -
    currentMinimalContainer.theDynkinType.getRank() -
    currentMinimalContainer.theCentralizerDynkinType.getRank();

    Rational totalCentalizerCandidateDim = dimOfSSpartOfCentralizerOfRootSA + this->dimCentralizerToralPart;

    if (totalCentalizerCandidateDim == this->dimensionCentralizer) {
      this->flagCentralizerIsRegular = true;
      this->flagCentralizerTypeComputed = true;
      this->CentralizerTypeIfKnown = currentMinimalContainer.theCentralizerDynkinType;
      return true;
    }
  }
  return false;
}

void SlTwoSubalgebra::computeModuleDecompositionsitionAmbientLieAlgebra() {
  this->checkConsistency();
  this->computeModuleDecompositionsition(
    this->getOwnerWeyl().rootsOfBorel,
    this->getOwnerWeyl().cartanSymmetric.numberOfRows,
    this->moduleDecompositionAmbientSA,
    this->moduleDimensions
  );
  this->attemptToComputeCentralizer();
}

void SlTwoSubalgebra::computeModuleDecompositionsitionOfMinimalContainingRegularSAs(SlTwoSubalgebras& owner) {
  MacroRegisterFunctionWithName("SlTwoSubalgebra::computeModuleDecompositionsitionOfMinimalContainingRegularSAs");
  this->moduleDecompositionMinimalContainingRootSAs.setSize(this->IndicesMinimalContainingRootSAs.size);
  List<int> buffer;
  for (int i = 0; i < this->IndicesMinimalContainingRootSAs.size; i ++) {
    RootSubalgebra& theSA = owner.theRootSAs.theSubalgebras[this->IndicesMinimalContainingRootSAs[i]];
    this->computeModuleDecompositionsition(
      theSA.positiveRootsReductiveSubalgebra,
      theSA.simpleRootsReductiveSubalgebra.size,
      this->moduleDecompositionMinimalContainingRootSAs[i],
      buffer
    );
  }
}

void SlTwoSubalgebra::makeReportPrecomputations(
  int indexMinimalContainingRegularSA, RootSubalgebra& MinimalContainingRegularSubalgebra
) {
  MacroRegisterFunctionWithName("SlTwoSubalgebra::makeReportPrecomputations");
  int theDimension = this->getOwnerSemisimpleAlgebra().getRank();
  this->IndicesContainingRootSAs.size = 0;
  Vectors<Rational> tempRoots;
  tempRoots = MinimalContainingRegularSubalgebra.simpleRootsReductiveSubalgebra;
  this->getOwnerSemisimpleAlgebra().theWeyl.transformToSimpleBasisGeneratorsWithRespectToH(tempRoots, this->theH.getCartanPart());
  DynkinDiagramRootSubalgebra theDiagram;
  theDiagram.ambientBilinearForm = this->getOwnerWeyl().cartanSymmetric;
  theDiagram.ambientRootSystem = this->getOwnerWeyl().rootSystem;
  theDiagram.computeDiagramInputIsSimple(tempRoots);
  this->IndicesContainingRootSAs.addOnTop(indexMinimalContainingRegularSA);
  tempRoots.makeEiBasis(theDimension);
  this->getOwnerSemisimpleAlgebra().theWeyl.transformToSimpleBasisGeneratorsWithRespectToH(tempRoots, this->theH.getCartanPart());
  DynkinDiagramRootSubalgebra tempDiagram;
  tempDiagram.ambientBilinearForm = this->getOwnerWeyl().cartanSymmetric;
  tempDiagram.ambientRootSystem = this->getOwnerWeyl().rootSystem;
  tempDiagram.computeDiagramInputIsSimple(tempRoots);
  this->preferredAmbientSimpleBasis = tempRoots;
  this->hCharacteristic.setSize(theDimension);
  for (int i = 0; i < theDimension; i ++) {
    this->hCharacteristic[i] = this->getOwnerSemisimpleAlgebra().theWeyl.rootScalarCartanRoot(
      this->theH.getCartanPart(), this->preferredAmbientSimpleBasis[i]
    );
  }
  //this->hCharacteristic.ComputeDebugString();
//  if (this->theE.NonZeroElements.maximumSize == this->owner->theWeyl.RootSystem.size
//      && this->theF.NonZeroElements.maximumSize == this->owner->theWeyl.RootSystem.size
//      && this->theH.NonZeroElements.maximumSize == this->owner->theWeyl.RootSystem.size)
  this->getOwnerSemisimpleAlgebra().lieBracket(this->theE, this->theF, this->bufferEbracketF);
  this->getOwnerSemisimpleAlgebra().lieBracket(this->theH, this->theE, this->bufferHbracketE);
  this->getOwnerSemisimpleAlgebra().lieBracket(this->theH, this->theF, this->bufferHbracketF);

  //theSl2.hCharacteristic.ComputeDebugString();
//  this->computePrimalModuleDecomposition();
}

//The below code is related to sl(2) subalgebras of simple Lie algebras
void SlTwoSubalgebra::computeModuleDecompositionsition(
  const Vectors<Rational>& positiveRootsContainingRegularSA,
  int dimensionContainingRegularSA,
  CharacterSemisimpleLieAlgebraModule<Rational>& outputHWs,
  List<int>& outputModuleDimensions
) {
  MacroRegisterFunctionWithName("SlTwoSubalgebra::computePrimalModuleDecomposition");
  this->checkConsistency();
  positiveRootsContainingRegularSA.checkConsistency();
  if (positiveRootsContainingRegularSA.size <= 0) {
    global.fatal << "positiveRootsContainingRegularSA has less than one element. " << global.fatal;
  }
  int IndexZeroWeight = positiveRootsContainingRegularSA.size * 2;
  outputModuleDimensions.initializeFillInObject(4 * positiveRootsContainingRegularSA.size + 1, 0);
  outputModuleDimensions[IndexZeroWeight] = dimensionContainingRegularSA;
  List<int> BufferHighestWeights;
  Rational tempRat;
  Vectors<Rational> coordsInPreferredSimpleBasis;
  positiveRootsContainingRegularSA.getCoordinatesInBasis(
    this->preferredAmbientSimpleBasis, coordsInPreferredSimpleBasis
  );
  for (int k = 0; k < positiveRootsContainingRegularSA.size; k ++) {
    tempRat = this->hCharacteristic.scalarEuclidean(coordsInPreferredSimpleBasis[k]);
    if (tempRat.denominatorShort != 1) {
      global.fatal << "Characteristic must be integer. " << global.fatal;
    }
    if (tempRat>positiveRootsContainingRegularSA.size * 2) {
      global.fatal << "The scalar product of the h-Characteristic "
      << this->hCharacteristic.toString()
      << " with the simple root " << coordsInPreferredSimpleBasis[k].toString()
      << " is larger than " << positiveRootsContainingRegularSA.size * 2
      << ". The affected sl(2) subalgebra is " << this->toString() << ". " << global.fatal;
      break;
    }
    outputModuleDimensions[IndexZeroWeight + tempRat.numeratorShort] ++;
    outputModuleDimensions[IndexZeroWeight - tempRat.numeratorShort] ++;
  }
  BufferHighestWeights = (outputModuleDimensions);
  outputHWs.setExpectedSize( positiveRootsContainingRegularSA.size * 2);
  outputHWs.makeZero();
  Weight<Rational> currentHW;
  currentHW.weightFundamentalCoordS.makeEi(1, 0);
  for (int j = BufferHighestWeights.size - 1; j >= IndexZeroWeight; j --) {
    int topMult = BufferHighestWeights[j];
    if (topMult < 0) {
      global.fatal << "The sl(2)-module decomposition shows an sl(2)-module with highest weight "
      << topMult << " which is impossible. Here is the sl(2) subalgebra. " << this->toString() << "." << global.fatal;
    }
    if (topMult > 0) {
      currentHW.weightFundamentalCoordS[0] = j - IndexZeroWeight;
      outputHWs.addMonomial(currentHW, topMult);
      if (j != IndexZeroWeight) {
        BufferHighestWeights[IndexZeroWeight * 2 - j] -= topMult;
      }
      for (int k = j - 2; k >= IndexZeroWeight; k -= 2) {
        BufferHighestWeights[k] -= topMult;
        if (k != IndexZeroWeight) {
          BufferHighestWeights[IndexZeroWeight * 2 - k] -= topMult;
        }
        if (BufferHighestWeights[k] < 0 || !(BufferHighestWeights[k] == BufferHighestWeights[IndexZeroWeight * 2 - k])) {
          std::stringstream crashStream;
          crashStream
          << "An error check has failed. While trying to decompose with respect to  h-characteristic <br> "
          << this->hCharacteristic.toString()
          << ". The positive root system of the containing root subalgebra is <br>"
          << positiveRootsContainingRegularSA.toString()
          << ". <br>The preferred simple basis is <br>" << this->preferredAmbientSimpleBasis.toString()
          << ". The coordinates relative to the preferred simple basis are<br>"
          << coordsInPreferredSimpleBasis.toString() << " The starting weights list is <br>"
          << outputModuleDimensions << ". "
          << " I got that the root space of index  " <<  k + 1 << " has negative dimension. Something is wrong. ";
          global.fatal << crashStream.str() << global.fatal;
        }
      }
    }
  }
}

void SlTwoSubalgebras::toStringModuleDecompositionMinimalContainingRegularSAs(
  std::string& output, bool useLatex, bool useHtml
) {
  std::string tempS;
  std::stringstream out;
  for (int i = 0; i < this->size; i ++) {
    (*this)[i].toStringModuleDecompositionMinimalContainingRegularSAs(useLatex, useHtml, *this, tempS);
    out << tempS;
    if (useHtml) {
      out << "\n<br>";
    }
    out << "\n";
  }
  output = out.str();
}

Rational SlTwoSubalgebra::getDynkinIndex() const {
  return this->LengthHsquared * this->getOwnerWeyl().getLongestRootLengthSquared() / 4;
}

std::string SlTwoSubalgebras::descriptionHCharacteristic =
"Let h be in the Cartan subalgebra. Let \\(\\alpha_1, ..., \\alpha_n\\) "
"be simple roots with respect to h. Then the h-characteristic, as defined by E. Dynkin, "
"is the n-tuple \\((\\alpha_1(h), ..., \\alpha_n(h))\\).";

std::string SlTwoSubalgebras::descriptionHRealization =
"The actual realization of h. "
"The coordinates of h are given with respect to the fixed original simple basis. "
"Note that the h-characteristic is computed using <b>a possibly different simple basis</b>, "
"more precisely, with respect to any h-positive simple basis. ";

std::string SlTwoSubalgebras::descriptionMinimalContainingRegularSubalgebras =
"A regular semisimple subalgebra might contain an sl(2) such that the sl(2) "
"has no centralizer in the regular semisimple subalgebra, "
"but the regular semisimple subalgebra might fail to be minimal containing. "
"This happens when another minimal containing regular semisimple subalgebra "
"of equal rank nests as a root subalgebra in the containing SA. "
"See Dynkin, Semisimple Lie subalgebras of semisimple "
"Lie algebras, remark before Theorem 10.4.";

std::string SlTwoSubalgebras::descriptionModuleDecompositionOverSl2 =
"The \\(sl(2)\\) submodules of the ambient Lie algebra are parametrized by their "
"highest weight with respect to the Cartan element h of \\(sl(2)\\). "
"In turn, the highest weight is a positive integer multiple of the fundamental highest weight \\(\\psi\\). "
"\\(V_{l\\psi}\\) is \\(l + 1\\)-dimensional. ";

std::string SlTwoSubalgebras::toStringSummary(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("SlTwoSubalgebras::toStringSummary");
  std::stringstream out;
  bool useHtml = theFormat == nullptr ? true : theFormat->flagUseHTML;
  std::string displayPathAlgebra;
  displayPathAlgebra = "../";
  out << "Number of sl(2) subalgebras: " << this->size << ".\n";
  std::stringstream out2;
  out2 << "<br>Length longest root ambient algebra squared/4= "
  << this->getOwnerWeyl().getLongestRootLengthSquared() / 4 << "<br>";
  out2 << "<br> Given a root subsystem P, and a root subsubsystem P_0, "
  << "in (10.2) of Semisimple subalgebras of semisimple Lie algebras, "
  << "E. Dynkin defines "
  << " a numerical constant e(P, P_0) (which we call Dynkin epsilon). "
  << "<br>In Theorem 10.3, Dynkin proves that if an sl(2) is an "
  << "S-subalgebra in "
  << " the root subalgebra generated by P, such that it has "
  << "characteristic 2 for all simple roots of P lying in P_0, then "
  << "e(P, P_0)= 0. ";

  if (this->BadHCharacteristics.size > 0) {
    bool allbadAreGoodInTheirBadness = true;
    for (int i = 0; i < this->BadHCharacteristics.size; i ++) {
      bool isGoodInItsbadness = false;
      for (int j = 0; j < this->size; j ++) {
        if (this->BadHCharacteristics[i] == (*this)[j].theH.getCartanPart()) {
          isGoodInItsbadness = true;
          break;
        }
      }
      if (!isGoodInItsbadness) {
        allbadAreGoodInTheirBadness = false;
        out << "<b>Bad characteristic: " << this->BadHCharacteristics[i] << "</b>";
      } else {
        out2 << "<br><b>It turns out that in the current case of Cartan element h = "
        << this->BadHCharacteristics[i] << " we have that, for a certain P, "
        << " e(P, P_0) equals 0, but I failed to realize the corresponding sl(2) as a subalgebra of that P. "
        << "However, it turns out that h "
        << " is indeed an S-subalgebra of a smaller root subalgebra P'.</b>";
      }
    }
    if (!allbadAreGoodInTheirBadness) {
      out << "<b>found bad characteristics!</b>";
    }
  } else {
    out2 << "It turns out by direct computation that, in the current case of "
    << this->getOwner().toStringLieAlgebraName()
    << ",  e(P,P_0)= 0 implies that an S-sl(2) subalgebra "
    << "of the root subalgebra generated by P with characteristic with 2's in the simple roots of P_0 "
    << " always exists. Note that Theorem 10.3 is stated in one direction only.";
  }
  if (useHtml) {
    std::string idSpanHCharacteristicDescription = "spanDynkinHCharacteristicDescription";
    out << "<div id = '" << idSpanHCharacteristicDescription << "'>" << this->descriptionHCharacteristic << "</div><br>";
    out << "<div id = 'idCartanElementRealization'>" << this->descriptionHRealization << "</div>";
    out << "<div id = 'idMinimalContainingRegularSA'>" << this->descriptionMinimalContainingRegularSubalgebras << "</div>";
    out << "<div id = 'idModuleDecomposition'>" << this->descriptionModuleDecompositionOverSl2 << "</div>";
    out
    << "<br><br><table><tr><td>Type + realization link</td><td style =\"padding-right:20px\">"
    "<a href=\"#"
    << idSpanHCharacteristicDescription
    << "\">h-Characteristic</a>"
    << "</td>";
    out << "<td align =\"center\" style =\"white-space: nowrap\">"
    << "<a href = '#idCartanElementRealization'>Realization of h</a>"
    << "</td>"
    << "<td style =\"padding-left:20px\">"
    << "<a href=\"#idModuleDecomposition\">"
    << "sl(2)-module decomposition of the ambient Lie algebra</a> <br> "
    << "\\(\\psi=\\) the fundamental \\(sl(2)\\)-weight. "
    << "</a></td>"
    << "<td>Centralizer dimension</td>"
    << "<td>Type of semisimple part of centralizer, if known</td>"
    << "<td>The square of the length of the weight dual to h.</td>"
    << "<td>Dynkin index </td><td>Minimal containing regular semisimple SAs</td><td>"
    << "<a href = '#idMinimalContainingRegularSA'>Containing regular semisimple SAs in which the sl(2) has no centralizer</a>"
    << "</td></tr>";
  }
  for (int i = 0; i < this->size; i ++) {
    const SlTwoSubalgebra& theSl2 = (*this)[i];
    if (useHtml) {
      out << "<tr>"
      << "<td style =\"padding-right:20px\"><a href=\"#sl2index"
      << i << "\" >\\(A^{" << theSl2.getDynkinIndex() << "}_1" << "\\)</a></td>";
      out << "<td>";
    }
    out << theSl2.hCharacteristic.toString();
    if (useHtml) {
      out << "</td>";
      out << "<td style =\"white-space: nowrap\">";
    }
    out << theSl2.theH.getCartanPart().toString();
    if (!this->getOwnerWeyl().isDominantWeight(theSl2.theH.getCartanPart())) {
      out << "<b>Something has gone very wrong! The h is not dual to a dominant weight. This shouldn't happen: "
      << "this is either a programming or mathematical error. </b>";
    }
    if (useHtml) {
      out << "</td><td style =\"padding-left:20px\">";
    }
    FormatExpressions formatCharacter;
    formatCharacter.vectorSpaceEiBasisNames.addOnTop("\\psi");
    out << HtmlRoutines::getMathNoDisplay((theSl2.moduleDecompositionAmbientSA.toString(&formatCharacter)))
    << "\n<br>\n";
    if (useHtml) {
      out << "</td>";
    }
    out << "<td>" << theSl2.dimensionCentralizer << "</td>";
    if (theSl2.flagCentralizerTypeComputed) {
      out << "<td> " << HtmlRoutines::getMathNoDisplay(theSl2.CentralizerTypeIfKnown.toString()) << "</td>";
    } else {
      out << "<td> not computed</td>";
    }
    if (useHtml) {
      out << "<td>";
    }
    out << theSl2.LengthHsquared;
    if (useHtml) {
      out << "</td><td>";
    }
    out << theSl2.getDynkinIndex();
    if (useHtml) {
      out << "</td><td>";
    }
    for (int j = 0; j < theSl2.IndicesMinimalContainingRootSAs.size; j ++) {
      RootSubalgebra& currentSA = this->theRootSAs.theSubalgebras[theSl2.IndicesMinimalContainingRootSAs[j]];
      out << "<a href=\"" << displayPathAlgebra << "rootSubalgebra_"
      << theSl2.IndicesMinimalContainingRootSAs[j] + 1 << ".html\">"
      << currentSA.theDynkinDiagram.toString() << "</a>" << ";  ";
    }
    if (useHtml) {
      out << "</td><td>";
    }
    for (int j = 0; j < theSl2.IndicesContainingRootSAs.size; j ++) {
      RootSubalgebra& currentSA = this->theRootSAs.theSubalgebras[theSl2.IndicesContainingRootSAs[j]];
      out << "<a href=\"" <<  displayPathAlgebra << "rootSubalgebra_" << theSl2.IndicesContainingRootSAs[j] + 1 << ".html\">"
      << currentSA.theDynkinDiagram.toString() << "</a>" << ";  ";
    }
    if (useHtml) {
      out << "</td></tr>\n";
    }
  }
  if (useHtml) {
    out << "</table><HR width =\"100%\">";
  }
  out << out2.str() << "<hr>";
  return out.str();
}

std::string SlTwoSubalgebras::toString(FormatExpressions* theFormat) {
  std::string tempS;
  std::stringstream out;
  std::stringstream body;
  bool useHtml = theFormat == nullptr ? true : theFormat->flagUseHTML;
  for (int i = 0; i < this->size; i ++) {
    tempS = (*this)[i].toString(theFormat);
  //  body<< "Index " << i << ": ";
    if (useHtml) {
      body << "<br>";
    }
    body << tempS;
    if (useHtml) {
      body << "<HR width =\"100%\">";
    }
  }
  if (useHtml) {
    out << "<br>";
  }
  out << this->toStringSummary(theFormat);
  out << body.str();
  return out.str();
}

void SlTwoSubalgebras::toHTML(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("SlTwoSubalgebras::toHTML");
  std::string virtualFileName = this->owner->toStringVirtualFolderName() + this->owner->toStringFileNameRelativePathSlTwoSubalgebras();
  ProgressReport theReport;
  theReport.report("Preparing html pages for sl(2) subalgebras. This might take a while.");
  this->theRootSAs.toHTML(theFormat);
  std::stringstream out;
  out << "<html><title>sl(2)-subalgebras of "
  << this->theRootSAs.theSubalgebras[0].theDynkinDiagram.toString() << "</title>";
  out << HtmlRoutines::getJavascriptMathjax("../../../../");
  out << HtmlRoutines::getCSSLinkLieAlgebrasAndCalculator("../../../../");
  out << "<meta name =\"keywords\" content =\""
  <<  this->theRootSAs.theSubalgebras[0].theDynkinDiagram.toString()
  << " sl(2)-triples, sl(2)-subalgebras, nilpotent orbits simple Lie algebras, nilpotent orbits of "
  <<  this->theRootSAs.theSubalgebras[0].theDynkinDiagram.toString()
  << ", sl(2)-triples of "
  << this->theRootSAs.theSubalgebras[0].theDynkinDiagram.toString()
  << " \">";
  out << "<body>";
  out << "<h1> \\(sl(2)\\)-subalgebras of " << this->owner->toStringLieAlgebraNameFullHTML() << "</h1>";
  out << this->owner->toStringHTMLMenuStructureSummary("../", true, true, false, true);
  out << this->toString(theFormat);
  out << "</body></html>";
  std::stringstream commentsOnError;
  if (!FileOperations::writeFileVirual(virtualFileName, out.str(), &commentsOnError)) {
    global.fatal << "Failed to write sl(2)-subalgebras. " << commentsOnError.str() << global.fatal;
  }
}

bool CandidateSemisimpleSubalgebra::isExtremeWeight(int moduleIndex, int indexInIsoComponent) const {
  Vector<Rational>& currentWeight = this->WeightsModulesPrimal[moduleIndex][indexInIsoComponent];
  Vectors<Rational>& currentWeights = this->WeightsModulesPrimal[moduleIndex];
  for (int j = 0; j < this->primalSubalgebraModules.size; j ++) {
    for (int k = 0; k < this->WeightsModulesPrimal[this->primalSubalgebraModules[j]].size; k ++) {
      if (!this->WeightsModulesPrimal[this->primalSubalgebraModules[j]][k].isEqualToZero()) {
        Vector<Rational>& currentRoot = this->WeightsModulesPrimal[this->primalSubalgebraModules[j]][k];
        if (currentWeights.contains(currentWeight-currentRoot) && currentWeights.contains(currentWeight + currentRoot)) {
          return false;
        }
      }
    }
  }
  return true;
}

Rational CandidateSemisimpleSubalgebra::getScalarSubalgebra(
  const Vector<Rational>& primalWeightLeft, const Vector<Rational>& primalWeightRight
) const {
  return primalWeightLeft.scalarProduct(primalWeightRight, this->bilinearFormFundPrimal);
}

std::string CandidateSemisimpleSubalgebra::toStringDrawWeightsHelper(int indexModule, const Vector<Rational>& theWeight) const {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::toStringDrawWeightsHelper");
  std::stringstream out;
  List<List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > >& currentMod = this->modules[indexModule];
  FormatExpressions charFormat;
  charFormat.vectorSpaceEiBasisNames.setSize(this->getPrimalRank());
  for (int i = 0; i < this->getRank(); i ++) {
    std::stringstream tempStream;
    tempStream << "\\\\omega_{" << i + 1 << "}";
    charFormat.vectorSpaceEiBasisNames[i] = tempStream.str();
  }
  for (int i = this->getRank(); i < this->getPrimalRank(); i ++) {
    std::stringstream tempStream;
    tempStream << "\\\\psi_{" << i + 1 << "}";
    charFormat.vectorSpaceEiBasisNames[i] = tempStream.str();
  }
  for (int i = 0; i < currentMod.size; i ++) {
    out << "<table style =\\\"border:1px solid #000;\\\">";
    out << "<tr><td colspan =\\\"3\\\">"
    << "<span class =\\\"math\\\">"
    << "V_{" << this->HighestWeightsPrimal[indexModule].toStringLetterFormat("\\\\omega", &charFormat) << "}"
    << " </span></td></tr>"
    << "<tr>"
    << "<td style =\\\"text-align: center;\\\">basis</td>"
    << "<td style =\\\"text-align: center; border-left:1px solid #000; white-space: nowrap;\\\">weights simple coords.</td>"
    << "<td style =\\\"text-align: center; border-left:1px solid #000; white-space: nowrap;\\\">weights fund.coords.</td>"
    << "<td style =\\\"text-align: center; border-left:1px solid #000; white-space: nowrap;\\\">weights primal</td>"
    << "</tr>";
    for (int j = 0; j < currentMod[i].size; j ++) {
      std::string openTag = "", closeTag = "";
      if (this->WeightsModulesPrimal[indexModule][j] == theWeight) {
        openTag = "<span style =\\\"color:#FF0000\\\">";
        closeTag = "</span>";
      }
      out << "<tr>";

      out << "<td style =\\\"text-align: center;\\\">" << openTag;
      out << "<span class =\\\"math\\\">" << HtmlRoutines::doubleBackslashes(currentMod[i][j].toString()) << "</span>";
      out << closeTag << "</td>";

      Vector<Rational> tempV;
      this->matMultiplyFundCoordsToGetSimple.actOnVectorColumn(this->WeightsModulesPrimal[indexModule][j],tempV);
      out << "<td style =\\\"text-align: center; border-left:1px solid #000;\\\">" << openTag;
      out << tempV.toString();
      out << closeTag << "</td>";

      out << "<td style =\\\"text-align: center; border-left:1px solid #000;\\\">" << openTag;
      out << "<span class =\\\"math\\\">"
      << this->WeightsModulesNONprimal[indexModule][j].toStringLetterFormat("\\\\omega", &charFormat) << "</span>";
      out << closeTag << "</td>";

      out << "<td style =\\\"text-align: center; border-left:1px solid #000;\\\">" << openTag;
      out << "<span class =\\\"math\\\">"
      << this->WeightsModulesPrimal[indexModule][j].toStringLetterFormat("\\\\omega", &charFormat) << "</span>";
      out << closeTag << "</td>";
      out << "</tr>";
    }
    out << "</table>";
  }
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::toStringDrawWeights(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::toStringDrawWeights");
  if (!this->flagCentralizerIsWellChosen) {
    return "";
  }
  if (theFormat != nullptr) {
    if (!theFormat->flagIncludeMutableInformation) {
      return "<br>Weight diagram not drawn to avoid javascript problems "
      "(use command PrintSemisimpleSubalgebras if you want to see the weight diagram). ";
    }
  }
  int thePrimalRank = - 1;
  (this->centralizerRank + this->theHs.size).isSmallInteger(&thePrimalRank);
  if (thePrimalRank < 2) {
    return "";
  }
  std::stringstream out;
  if (thePrimalRank != this->bilinearFormFundPrimal.numberOfColumns) {
    out << "<br>The primal rank was computed to be " << thePrimalRank
    << " but the bilinear form in fundamental coordinates relative to "
    << " the subalgebra was not computed: this->bilinearFormFundPrimal has "
    << this->bilinearFormFundPrimal.numberOfRows << " rows. ";
    return out.str();
  }
  out << "<br>Weight diagram. The coordinates corresponding to the simple roots of the "
  << "subalgerba are fundamental.  "
  << "<br>The bilinear form is therefore given relative to the fundamental coordinates.<br> ";

//  out << "Embeddings fund coords into fund coords ambient.";
//  Matrix<Rational> theMat = this->matMultiplyFundCoordsToGetSimple;
//  theMat.


  Vectors<Rational> basisToDrawCirclesAt;
  DrawingVariables theDV;
  theDV.theBuffer.theBilinearForm.initialize(thePrimalRank, thePrimalRank);
  for (int i = 0; i < thePrimalRank; i ++) {
    for (int j = 0; j < thePrimalRank; j ++) {
      theDV.theBuffer.theBilinearForm(i, j) = this->bilinearFormFundPrimal(i, j).getDoubleValue();
    }
  }
  Vector<Rational> zeroVector;
  zeroVector.makeZero(thePrimalRank);
  basisToDrawCirclesAt.makeEiBasis(thePrimalRank);
  Vectors<Rational> cornerWeights;
  int maxModDim = 0;
  for (int i = 0; i < this->modules.size; i ++) {
    maxModDim = MathRoutines::maximum(this->modules[i][0].size, maxModDim);
  }
  theDV.DefaultHtmlHeight = MathRoutines::maximum(600, maxModDim * 35);
  for (int i = 0; i < this->modules.size; i ++) {
    cornerWeights.setSize(0);
    for (int j = 0; j < this->modules[i].size; j ++) {
      for (int k = 0; k < this->modules[i][j].size; k ++) {
        if (j == 0) {
          std::string color = "#959500";
          if (this->primalSubalgebraModules.contains(i)) {
            theDV.drawLineBetweenTwoVectorsBufferRational(zeroVector, this->WeightsModulesPrimal[i][k], "green");
          }
          theDV.drawCircleAtVectorBufferRational(this->WeightsModulesPrimal[i][k], color, 2);
          if (this->isExtremeWeight(i, k)) {
            cornerWeights.addOnTop(this->WeightsModulesPrimal[i][k]);
          }
        }
        if (k == this->modules[i][j].size - 1 && basisToDrawCirclesAt.size < thePrimalRank) {
          basisToDrawCirclesAt.addOnTop(this->WeightsModulesPrimal[i][k]);
          if (basisToDrawCirclesAt.getRankOfSpanOfElements() != basisToDrawCirclesAt.size) {
            basisToDrawCirclesAt.removeLastObject();
          }
        }
      }
    }
    std::string color = "#f0f000";
    if (this->primalSubalgebraModules.contains(i)) {
      color = "blue";
    }
    for (int j = 0; j < cornerWeights.size; j ++) {
      Rational minDist = 0;
      for (int k = 0; k < cornerWeights.size; k ++) {
        Rational tempRat = Vector<Rational>::scalarProduct(
          cornerWeights[k] - cornerWeights[j],
          cornerWeights[k] - cornerWeights[j],
          this->bilinearFormFundPrimal
        );
        if (minDist == 0) {
          minDist = tempRat;
        } else if (tempRat != 0) {
          minDist = MathRoutines::minimum(tempRat, minDist);
        }
      }
      for (int k = j + 1; k < cornerWeights.size; k ++) {
        if (minDist == Vector<Rational>::scalarProduct(
          cornerWeights[k] - cornerWeights[j],
          cornerWeights[k] - cornerWeights[j],
          this->bilinearFormFundPrimal
        )) {
          theDV.drawLineBetweenTwoVectorsBufferRational(cornerWeights[k], cornerWeights[j], color, 1);
        }
      }
    }
  }
  HashedList<Vector<Rational> > currentWeights;

  Vectors<double> highlightGroup;
  List<std::string> highlightLabels;
  for (int i = 0; i < this->modules.size; i ++) {
    currentWeights.clear();
    currentWeights.addOnTopNoRepetition(this->WeightsModulesPrimal[i]);
    Vectors<double> currentWeightsDouble;
    currentWeightsDouble.setSize(currentWeights.size);
    for (int j = 0; j < currentWeightsDouble.size; j ++) {
      currentWeightsDouble[j] = currentWeights[j].getVectorDouble();
    }
    highlightGroup.setSize(currentWeights.size);
    highlightLabels.setSize(currentWeights.size);
    for (int j = 0; j < currentWeights.size; j ++) {
      highlightGroup[j] = currentWeights[j].getVectorDouble();
      highlightLabels[j] = currentWeights[j].toString();
    }
    theDV.theBuffer.drawHighlightGroup(highlightGroup, highlightLabels, "black", 5);
  }
  theDV.theBuffer.basisToDrawCirclesAt.setSize(basisToDrawCirclesAt.size);
  for (int i = 0; i < theDV.theBuffer.basisToDrawCirclesAt.size; i ++) {
    theDV.theBuffer.basisToDrawCirclesAt[i].setSize(thePrimalRank);
    for (int j = 0; j < thePrimalRank; j ++) {
      theDV.theBuffer.basisToDrawCirclesAt[i][j] = basisToDrawCirclesAt[i][j].getDoubleValue();
    }
    theDV.drawCircleAtVectorBufferRational(basisToDrawCirclesAt[i], "red", 4);
  }
  theDV.theBuffer.BasisProjectionPlane.makeEiBasis(thePrimalRank);
  out << theDV.getHTMLDiv(thePrimalRank);
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::toStringModuleDecompositionLaTeX(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::toStringModuleDecompositionLaTeX");
  if (this->modules.size <= 0) {
    return "";
  }
  if (!this->owner->flagcomputeModuleDecompositionsition) {
    return "";
  }
  if (!this->flagCentralizerIsWellChosen) {
    return "";
  }
  std::stringstream out;
  //out << "Isotypic module decomposition over primal subalgebra (total " << this->modules.size << " isotypic components). ";
  FormatExpressions tempCharFormat;
  if (!this->charFormaT.isZeroPointer()) {
    tempCharFormat = this->charFormaT.getElementConst();
  }
  //bool useModuleIndex = false;
  out << "\\documentclass{article}\\usepackage{amssymb}\\usepackage{longtable}"
  << "\\usepackage{multirow}\\begin{document}" ;
  out << "<br> \\begin{longtable}{c|c|c|c|c|c}\n<br>\n\\caption{Module decomposition of $"
  << this->owner->owner->theWeyl.theDynkinType.toString() << "$ over $"
  << this->weylNonEmbedded->theDynkinType.toString()
  << " \\oplus \\mathfrak h_c$\\label{tableModuleDecompo} }\\\\"
  << "Component &Module & elements & elt. weights& $h$-element "
  << "$\\mathfrak k-sl(2)$-triple& $f$-element $\\mathfrak k-sl(2)$-triple\\\\";
  ElementSemisimpleLieAlgebra<AlgebraicNumber> tempLieBracket;
  for (int i = 0; i < this->modules.size; i ++) {
    int numRowsIsoCompo = this->modules[i].size * this->modules[i][0].size;
    if (numRowsIsoCompo > 1) {
      out << "\\multirow{" << numRowsIsoCompo << "}{*}";
    }
    out << "{$W_{" << i + 1 << "} $}";
    out << "&";
    for (int j = 0; j < this->modules[i].size; j ++) {
      if (j > 0) {
        out << " & ";
      }
      if (this->modules[i][j].size > 1) {
        out << "\\multirow{" << this->modules[i][j].size << "}{*}";
      }
      out << "{$V_{" << this->HighestWeightsPrimal[i].toStringLetterFormat("\\omega", &tempCharFormat) << "}$} ";
      out << "&";
      for (int k = 0; k < this->modules[i][j].size; k ++) {
        if (k > 0) {
          out << "&&";
        }
        out << "$" << this->modules[i][j][k].toString(theFormat) << "$";
        out << "&";
        out << "$" << this->WeightsModulesPrimal[i][k].toStringLetterFormat("\\omega", &tempCharFormat) << "$" << "& ";
        bool OpsAreGood = false;
        if (i < this->modulesSl2opposite.size) {
          if (j < this->modulesSl2opposite[i].size) {
            if (k < this->modulesSl2opposite[i][j].size) {
              OpsAreGood = true;
            }
          }
        }
        if (OpsAreGood) {
          List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& currentOpModule = this->modulesSl2opposite[i][j];
          if (!currentOpModule[k].isEqualToZero()) {
            this->getAmbientSemisimpleLieAlgebra().lieBracket(this->modules[i][j][k], currentOpModule[k], tempLieBracket);
            out << "$" << tempLieBracket.toString() << "$&";
            out << "$" << currentOpModule[k].toString() << "$";
          } else {
            out << "$N/A$&N/A";
          }
        }
        out << "\\\\" << " <br>";
      }
      out << "\\cline{2-6}";
    }
    out << "\\hline";
  }
  out << "\\end{longtable}<br>\n\\end{document}";
  return out.str();
}


std::string CandidateSemisimpleSubalgebra::toStringModuleDecomposition(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::toStringModuleDecomposition");
  if (this->modules.size <= 0) {
    return "";
  }
  bool useMouseHover = theFormat == nullptr ? true : !theFormat->flagUseMathSpanPureVsMouseHover;
  std::stringstream out;
  out << "Isotypic module decomposition over primal subalgebra (total " << this->modules.size << " isotypic components). ";
  out << "<table class = 'tableStandard'>";
  FormatExpressions tempCharFormat;
  if (!this->charFormaT.isZeroPointer()) {
    tempCharFormat = this->charFormaT.getElementConst();
  }
  out << "<tr><td>Isotypical components + highest weight</td>";
  for (int i = 0; i < this->HighestWeightsPrimal.size; i ++) {
    bool visible = true;
    bool isDouble = false;
    if (i > 0) {
      if (this->HighestWeightsPrimal[i] == this->HighestWeightsPrimal[i - 1]) {
        visible = false;
      }
    }
    if (i < this->HighestWeightsPrimal.size - 1) {
      if (this->HighestWeightsPrimal[i] == this->HighestWeightsPrimal[i + 1]) {
        isDouble = true;
      }
    }
    if (visible) {
      if (isDouble) {
        out << "<td colspan =\"2\">";
      } else {
        out << "<td>";
      }
      std::stringstream tempStream;
      tempStream << "V_{" << this->HighestWeightsPrimal[i].toStringLetterFormat("\\omega", &tempCharFormat) << "} ";
      if (useMouseHover) {
        out << HtmlRoutines::getMathNoDisplay(tempStream.str()) << " &rarr; " << this->HighestWeightsPrimal[i].toString();
      } else {
        out << HtmlRoutines::getMathNoDisplay(tempStream.str()) << " &rarr; " << this->HighestWeightsPrimal[i].toString();
      }
      out << "</td>";
    }
  }
  out << "</tr>";
  out << "<tr><td>Module label </td>";
  for (int i = 0; i < this->HighestWeightsPrimal.size; i ++) {
    out << "<td>\\(" << "W_{" << i + 1 << "}" << "\\)</td>";
  }
  out << "</tr>";
  out << "<tr><td>Module elements (weight vectors). <span class = 'fElement'>In blue - corresp. F element</span>. "
  << "<span class = 'hElement'>In red -corresp. H element</span>. </td>";
  ElementSemisimpleLieAlgebra<AlgebraicNumber> tempLieBracket;
  for (int i = 0; i < this->modules.size; i ++) {
    out << "<td>";
    if (this->primalSubalgebraModules.contains(i)) {
      if (this->subalgebraModules.contains(i)) {
        out << "Semisimple subalgebra component. ";
      } else {
        out << "Cartan of centralizer component. ";
      }
    }
    out << "<table class = 'tableNone'><tr>";
    for (int j = 0; j < this->modules[i].size; j ++) {
      List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& currentModule = this->modules[i][j];
      out << "<td>";
      out << "<table class = 'tableModuleDecomposition'>";
      for (int k = 0; k < currentModule.size; k ++) {
        out << "<tr><td>\\(" << currentModule[k].toString() << "\\)</td>";
        bool OpsAreGood = false;
        if (i < this->modulesSl2opposite.size) {
          if (j < this->modulesSl2opposite[i].size) {
            if (k < this->modulesSl2opposite[i][j].size) {
              OpsAreGood = true;
            }
          }
        }
        if (OpsAreGood) {
          List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& currentOpModule = this->modulesSl2opposite[i][j];
          if (!currentOpModule[k].isEqualToZero()) {
            out << "<td><span style =\"color:#0000FF\">" << currentOpModule[k].toString() << "</span></td>";
            this->getAmbientSemisimpleLieAlgebra().lieBracket(currentModule[k], currentOpModule[k], tempLieBracket);
            out << "<td><span style =\"color:#FF0000\">" << tempLieBracket.toString() << "</span></td>";
          } else {
            out << "<td><span style =\"color:#0000FF\"> N/A </span></td>";
          }
        }
        out << "</tr>";
      }
      out << "</table>";
      out << "</td>";
    }
//  out << "<td>|||</td><td>Union of the isotypic components:<br>";
//  for (int j = 0; j < this->ModulesIsotypicallyMerged[i].size; j ++)
//  { out << this->ModulesIsotypicallyMerged[i][j].toString() << "<br>";
//  }
//  out << "</td>";
    out << "</tr></table>";
    out << "</td>";
  }
  out << "</tr>";
  out << "<tr>";
  out << "<td>Weights of elements in fundamental coords w.r.t. Cartan of subalgebra in same order as above</td>";
  for (int i = 0; i < this->WeightsModulesNONprimal.size; i ++) {
    out << "<td>";
    for (int j = 0; j < this->WeightsModulesNONprimal[i].size; j ++) {
      out << "\\("
      << this->WeightsModulesNONprimal[i][j].toStringLetterFormat("\\omega", &tempCharFormat)
      << "\\)";
      if (j != this->WeightsModulesNONprimal[i].size - 1) {
        out << "<br>";
      }
    }
    out << "</td>";
  }
  out << "</tr>";
  out << "<tr>";
  out << "<td>Weights of elements in (fundamental coords w.r.t. Cartan of subalgebra) + Cartan centralizer</td>";
  for (int i = 0; i < this->WeightsModulesPrimal.size; i ++) {
    out << "<td>";
    for (int j = 0; j < this->WeightsModulesPrimal[i].size; j ++) {
      out << "\\("
      << this->WeightsModulesPrimal[i][j].toStringLetterFormat("\\omega", &tempCharFormat)
      << "\\)";
      if (j != this->WeightsModulesPrimal[i].size - 1) {
        out << "<br>";
      }
    }
    out << "</td>";
  }
  out << "</tr>";
  out << "<tr>";
  tempCharFormat.FDrepLetter = "M";
  out << "<td>Single module character over Cartan of s.a.+ Cartan of centralizer of s.a.</td>";
  if (this->CharsPrimalModules.size > 0) {
    for (int i = 0; i < this->CharsPrimalModules.size; i ++) {
      out << "<td>";
      if (useMouseHover) {
        out << HtmlRoutines::getMathNoDisplay(this->CharsPrimalModules[i].toString(&tempCharFormat));
      } else {
        out << HtmlRoutines::getMathNoDisplay(this->CharsPrimalModules[i].toString(&tempCharFormat));
      }
      out << "</td>";
    }
  } else {
    for (int i = 0; i < this->modules.size; i ++) {
      out << "<td>Not computed.</td>";
    }
  }
  out << "</tr>";
  out << "<tr>";
  out << "<td>Isotypic character</td>";
  if (this->CharsPrimalModulesMerged.size > 0) {
    for (int i = 0; i < this->CharsPrimalModulesMerged.size; i ++) {
      out << "<td>";
      if (useMouseHover) {
        out << HtmlRoutines::getMathNoDisplay(this->CharsPrimalModulesMerged[i].toString(&tempCharFormat));
      } else {
        out << HtmlRoutines::getMathNoDisplay(this->CharsPrimalModulesMerged[i].toString(&tempCharFormat));
      }
      out << "</td>";
    }
  } else {
    for (int i = 0; i < this->modules.size; i ++) {
      out << "<td>Not computed.</td>";
    }
  }
  out << "</tr>";
  out << "</table>";
  Vector<Rational> semisimpleSAv, centralizerV;
  semisimpleSAv.makeZero(this->modules.size);
  centralizerV.makeZero(this->modules.size);
  for (int i = 0; i < this->subalgebraModules.size; i ++) {
    semisimpleSAv[this->subalgebraModules[i]] += 1;
  }
  for (int i = 0; i < this->centralizerSubalgebraModules.size; i ++) {
    centralizerV[this->centralizerSubalgebraModules[i]] += 1;
  }
  out << "<br>Semisimple subalgebra: " << semisimpleSAv.toStringLetterFormat("W");
  out << "<br>Centralizer extension: " << centralizerV.toStringLetterFormat("W");
  return out.str();
}

std::string NilradicalCandidate::toStringTableElementWithWeights(
  const List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& theElts, const Vectors<Rational>& theWeights
) const {
  std::stringstream out;
  out << "<table border =\"1\"><tr>";
  for (int i = 0; i < theWeights.size; i ++) {
    out << "<td><table><tr><td>" << theWeights[i].toString() << "</td></tr>";
    for (int j = 0; j < theWeights.size; j ++) {
      if (theWeights[i] == theWeights[j]) {
        out << "<tr><td>" << theElts[j].toString() << "</td></tr>";
      }
    }
    out << "</table></td>";
  }
  out << "</tr></table>";
  return out.str();
}

std::string NilradicalCandidate::toString(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("NilradicalCandidate::toString");
  (void) theFormat; //avoid unused parameter warning in a portable way.
  std::stringstream out;
  out << this->FKnilradicalLog;
  Vector<Rational> currentNilrad;
  currentNilrad = this->theNilradicalSelection;
  out << currentNilrad.toStringLetterFormat("W");
  if (this->flagNilradicalConesIntersect) {
    out << ". Cones intersect. ";
  } else {
    out << ". Cones don't intersect. ";
  }
  if (this->theNilradicalWeights.size > 0) {
    out << "<br>Nilradical cone: <br>"
    << this->toStringTableElementWithWeights(this->theNilradical, this->theNilradicalWeights);
  }
  out << "<br> Highest weight cone:<br> " << this->toStringTableElementWithWeights(this->theNonFKhwVectors, this->theNonFKhws);
  if (this->flagNilradicalConesIntersect) {
    out << "<br>Strongly singular weights: " << this->theNonFKhwsStronglyTwoSided.toString();
    out << "<br>Cone intersection: " << this->ConeIntersection.toStringLetterFormat("w");
    out << "<br> ";
    FormatExpressions tempFormat;
    tempFormat.vectorSpaceEiBasisNames.setSize(this->ConeIntersection.size);
    for (int j = 0; j < this->theNilradicalWeights.size; j ++) {
      tempFormat.vectorSpaceEiBasisNames[j] = this->theNilradicalWeights[j].toString();
    }
    for (int j = 0; j < this->theNonFKhws.size; j ++) {
      tempFormat.vectorSpaceEiBasisNames[j + this->theNilradicalWeights.size] = this->theNonFKhws[j].toString();
    }
    out << this->ConeIntersection.toStringLetterFormat("w", &tempFormat);
    if (this->flagNilradicalConesStronglyIntersect) {
      for (int j = 0; j < this->theNonFKhwsStronglyTwoSided.size; j ++) {
        tempFormat.vectorSpaceEiBasisNames[j + this->theNilradicalWeights.size] = this->theNonFKhwsStronglyTwoSided[j].toString();
      }
      out << "<br>In addition, the nilradical cones intersect strongly "
      << "at weight " << this->getConeStrongIntersectionWeight().toString();
      out << "<br>" << this->ConeStrongIntersection.toStringLetterFormat("w", &tempFormat);
      out << "<br>The involved nilradical elements: " << "<br><table><tr>";
      for (int i = 0; i < this->theNilradicalWeights.size; i ++) {
        if (this->ConeStrongIntersection[i] != 0) {
          Vector<Rational>& currentNilradWeight = this->theNilradicalWeights[i];
          out << "<td><table border =\"1\"><tr><td>" << currentNilradWeight.toString() << "</td></tr>";
          for (int j = 0; j < this->theNilradicalWeights.size; j ++) {
            if (currentNilradWeight == this->theNilradicalWeights[j]) {
              out << "<tr><td>" << this->theNilradical[j].toString() << "</td></tr>";
              out << "<tr><td>" << this->theNilradicalElementOpposites[j].toString() << "</td></tr>";
            }
          }
          out << "<tr><td>Scal. prod. <br> L-inf. rel.: "
          << this->owner->getScalarSubalgebra(currentNilradWeight, this->getConeStrongIntersectionWeight())
          << "</td></td>";
          out << "</table>";
          out << "</td>";
        }
      }
      out << "</tr></table>";
    }
    if (this->flagComputedRelativelyStrongIntersections && this->flagLinfiniteRelFound) {
      tempFormat.vectorSpaceEiBasisNames.setSize(this->ConeRelativelyStrongIntersection.size);
      for (int j = 0; j < this->theNilradicalSubset.size; j ++) {
        tempFormat.vectorSpaceEiBasisNames[j] = this->theNilradicalSubsetWeights[j].toString();
      }
      for (int j = 0; j < this->theNonFKhwVectorsStrongRelativeToSubset.size; j ++) {
        tempFormat.vectorSpaceEiBasisNames[j + this->theNilradicalSubset.size] =
        this->theNonFKhwVectorsStrongRelativeToSubsetWeights[j].toString();
      }
      out << "<br>In addition, strong nilradical cone intersections did not suffice, I needed to compute relatively strong intersections. ";
      out << "<br>The nilradical subset:" << this->toStringTableElementWithWeights(this->theNilradicalSubset, this->theNilradicalSubsetWeights);
      out << "<br>The highest weight cone consisting of elements commuting with the nilradical subset: "
      << this->toStringTableElementWithWeights(this->theNonFKhwVectorsStrongRelativeToSubset, this->theNonFKhwVectorsStrongRelativeToSubsetWeights);
      out << "<br>Relation: " << this->ConeRelativelyStrongIntersection.toStringLetterFormat("w", &tempFormat);
    }
    if (this->flagLinfiniteRelFound) {
      out << "<br><b>L-infinite relation exists!</b>";
    } else {
      out << "<br><span style =\"color:#FF0000\"><b>No L-infinite relation found.</b></span>";
    }
  } else {
    if (!this->flagParabolicACextendsToParabolicAC) {
      out << "<br><b style = 'color:red'>Strong centralizer condition not proven to hold.</b>";
    }
    if (this->flagRestrictedCentralizerConditionHoldS) {
      out << "<br>The centralizer condition holds. ";
    }
    out << "<br>Levi roots primal fundamental coordinates: " << this->leviRootsSmallPrimalFundCoords.toString()
    << "<br>Levi components in centralizer: "
    << this->theLeviDiagramSmalL.toString()
    << "<br>Levi roots containing parabolic: " << this->leviRootsAmbienT.toString()
    << "<br>Levi components containing parabolic: "
    << this->theLeviDiagramAmbienT.toString();
    out << "<br>Separating hyperplane: " << this->ConeSeparatingNormal.toStringLetterFormat("u");
  }
  return out.str();
}

void NilradicalCandidate::getModGeneratedByNonHighestWeightVectorAndNilradElement(
  int indexInNilradSubset,
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputLeft,
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputRight,
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputBrackets
) const {
  MacroRegisterFunctionWithName("NilradicalCandidate::getModGeneratedByNonHighestWeightVectorAndNilradElement");
  outputBrackets.setSize(0);
  outputBrackets.addOnTop(this->theNilradicalSubset[indexInNilradSubset]);
  outputRight.setSize(1);
  outputLeft.setSize(1);
  ElementSemisimpleLieAlgebra<AlgebraicNumber> theLieBracket;
  for (int i = 0; i < outputBrackets.size; i ++) {
    if (!outputBrackets[i].isEqualToZero()) {
      for (int j = 0; j < this->theNonFKhwVectorsStrongRelativeToSubset.size; j ++) {
        this->owner->owner->owner->lieBracket(this->theNonFKhwVectorsStrongRelativeToSubset[j], outputBrackets[i], theLieBracket);
        outputBrackets.addOnTop(theLieBracket);
        outputLeft.addOnTop(this->theNonFKhwVectorsStrongRelativeToSubset[j]);
        outputRight.addOnTop(outputBrackets[i]);
      }
    }
  }
  outputLeft.popIndexShiftDown(0);
  outputRight.popIndexShiftDown(0);
  outputBrackets.popIndexShiftDown(0);
}

std::string CandidateSemisimpleSubalgebra::toStringNilradicalsSummary(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::toStringNilradicalsSummary");
  (void) theFormat; //avoid unused parameter warning in a portable way.
  if (this->FKNilradicalCandidates.size == 0) {
    return "";
  }
  std::stringstream out;
  out << "<br>There are " << this->FKNilradicalCandidates.size
  << " possible isotypic nilradical extensions of the primal subalgebra. Of them "
  << this->NumConeIntersections << " have intersecting cones. Of the remaining "
  << this->FKNilradicalCandidates.size - this->NumConeIntersections << " nilradical extensions with non-intersecting cones, "
  << this->FKNilradicalCandidates.size - this->NumConeIntersections - this->NumCentralizerConditionFailsconeConditionHolds
  << " satisfy the centralizer condition and " << this->NumCentralizerConditionFailsconeConditionHolds
  << " fail the centralizer condition.";
  if (this->NumBadParabolics > 0) {
    out << "<br><span style =\"color:#FF0000\">Of the subalgebra(s) satisfying the centralizer condition,  "
    << this->NumBadParabolics << " have (has) centralizer parabolics that do not extend to  "
    << " parabolics of the ambient Lie algebra with Levi types A and C. "
    << "For these subalgebras the PSZ construction is not proven to hold. </span>";
  } else {
    out << "<br><span style =\"color:#0000FF\"> In each of "
    << this->FKNilradicalCandidates.size - this->NumConeIntersections - this->NumCentralizerConditionFailsconeConditionHolds
    << " case(s) when the centralizer condition holds, "
    << "the parabolic subalgebra in the centralizer with Levi types A and C extends "
    << "to parabolic subalgebra of the ambient Lie algebra whose Levi types are A and C only. </span>";
  }
  if (this->NumConeIntersections > 0) {
    if (this->NumCasesNoLinfiniteRelationFound > 0) {
      out << "<br><span style =\"color:#FF0000\">In " << this->NumCasesNoLinfiniteRelationFound
      << " cases no L-infinite relation was found. </span>";
    } else {
      out << "<br><span style =\"color:#0000FF\"> In each of " << this->NumConeIntersections
      << " case(s) of intersecting cones, an L-infinite relation was found. </span>";
    }
  }
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::toStringNilradicals(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::toStringNilradicals");
  if (this->FKNilradicalCandidates.size == 0) {
    if (this->owner->flagComputeNilradicals) {
      if (this->amRegularSA()) {
        return "Nilradicals not computed, but that is OK because this a root subalgebra. ";
      } else {
        return "<b>Nilradicals not computed AND this is not a root subalgebra.</b>";
      }
    } else {
      return "";
    }
  }
  std::stringstream out;
  out << this->toStringNilradicalsSummary(theFormat);
  Vector<Rational> primalBase;
  primalBase = this->FKNilradicalCandidates[0].theNilradicalSelection;
  out << "<br>The primal extension of the semisimple subalgerba equals: " << primalBase.toStringLetterFormat("W");
  if (this->owner->flagProduceLaTeXtables) {
    Vector<Rational> currentNilradVector;
    out << "<br><br>A summary in LaTeX <br>\\begin{longtable}{c|c|c|c }"
    << "\\caption{Nilradicals\\label{tableNilrads} }\\\\ $ \\mathfrak n _{\\mathfrak l} $ & Cones intersect & Cone intersection ";
    FormatExpressions tempFormat;
    if (!this->charFormaT.isZeroPointer()) {
      tempFormat = this->charFormaT.getElementConst();
    }
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > RelevantBracketsLeft, RelevantBracketsRight, RelevantBrackets;
    for (int i = 0; i < this->FKNilradicalCandidates.size; i ++) {
      const NilradicalCandidate& currentNilrad = this->FKNilradicalCandidates[i];
      currentNilradVector = currentNilrad.theNilradicalSelection;
      for (int j = 0; j < this->primalSubalgebraModules.size; j ++) {
        currentNilradVector[this->primalSubalgebraModules[j]] -= 1;
      }
      out << "\\\\\\hline<br>\n";
      out << "$" << currentNilradVector.toStringLetterFormat("W") << "$ &";
      if (currentNilrad.flagNilradicalConesIntersect) {
        out << "yes";
      } else {
        out << "no";
      }
      out << "&";
      if (currentNilrad.flagLinfiniteRelFound) {
        out << "\\begin{tabular}{l";
        for (
          int j = 0;
          j < currentNilrad.theNonFKhwVectorsStrongRelativeToSubsetWeights.size + currentNilrad.theNilradicalSubsetWeights.size;
          j ++
        ) {
          out << "cc";
        }
        out << "}Relation&";
        for (int j = 0; j < currentNilrad.theNonFKhwVectorsStrongRelativeToSubsetWeights.size; j ++) {
          Rational theCF = currentNilrad.ConeRelativelyStrongIntersection[currentNilrad.theNilradicalSubsetWeights.size + j];
          theCF.negate();
          out << "$"
          << (currentNilrad.theNonFKhwVectorsStrongRelativeToSubsetWeights[j] * theCF).toStringLetterFormat("\\omega", &tempFormat)
          << "$";
          out << " & ";
          if (j != currentNilrad.theNonFKhwVectorsStrongRelativeToSubsetWeights.size - 1) {
            out << "+&";
          } else {
            out << "= &";
          }
        }
        for (int j = 0; j < currentNilrad.theNilradicalSubsetWeights.size; j ++) {
          Rational theCF = currentNilrad.ConeRelativelyStrongIntersection[j];
          out << "$" << (currentNilrad.theNilradicalSubsetWeights[j] * theCF).toStringLetterFormat("\\omega", &tempFormat) << "$";
          out << " & ";
          if (j != currentNilrad.theNilradicalSubsetWeights.size - 1) {
            out << "+&";
          }
        }
        out << "\\\\<br>\n Elts.& ";
        for (int j = 0; j < currentNilrad.theNonFKhwVectorsStrongRelativeToSubset.size; j ++) {
          out << "$" << currentNilrad.theNonFKhwVectorsStrongRelativeToSubset[j].toString() << "$";
          out << " & &";
        }
        for (int j = 0; j < currentNilrad.theNilradicalSubset.size; j ++) {
          out << "$" << currentNilrad.theNilradicalSubset[j].toString() << "$";
          out << " & ";
          if (j != currentNilrad.theNilradicalSubset.size - 1) {
            out << " &";
          }
        }
        out << "\\\\<br>\n Opposite elts. &";
        for (int j = 0; j < currentNilrad.theNonFKhwVectorsStrongRelativeToSubset.size; j ++) {
          out << " & &";
        }
        for (int j = 0; j < currentNilrad.theNilradicalSubset.size; j ++) {
          out << "$" << currentNilrad.theNilradicalElementOpposites[currentNilrad.theNilradSubsel.elements[j]].toString() << "$";
          out << " & ";
          if (j != currentNilrad.theNilradicalSubset.size - 1) {
            out << " &";
          }
        }
        out << "\\end{tabular}\\\\\n<br>";
        out << "&& Relevant Lie brackets: ";
        ElementSemisimpleLieAlgebra<AlgebraicNumber> tempElt;
        std::stringstream tempStream;
        for (int j = 0; j < currentNilrad.theNilradicalSubset.size; j ++) {
          currentNilrad.getModGeneratedByNonHighestWeightVectorAndNilradElement(j, RelevantBracketsLeft, RelevantBracketsRight, RelevantBrackets);
          for (int k = 0; k < RelevantBrackets.size; k ++) {
            tempStream << "$[" << RelevantBracketsLeft[k].toString() << ", " << RelevantBracketsRight[k].toString() << "] ="
            << RelevantBrackets[k].toString() << "$, ";
          }
        }
        for (int j = 0; j < currentNilrad.theNilradicalSubset.size; j ++) {
          for (int k = 0; k < currentNilrad.theNilradicalSubset.size; k ++) {
            if (k != j) {
              this->owner->owner->lieBracket(
                currentNilrad.theNilradicalSubset[j],
                currentNilrad.theNilradicalElementOpposites[currentNilrad.theNilradSubsel.elements[k]],
                tempElt
              );
              tempStream << "$[" << currentNilrad.theNilradicalSubset[j].toString() << ", "
              << currentNilrad.theNilradicalElementOpposites[currentNilrad.theNilradSubsel.elements[k]].toString()
              << "] =" << tempElt.toString() << ", ";
            }
          }
        }
        std::string tempS = tempStream.str();
        tempS.resize(tempS.size() - 2);
        out << tempS;
      }

  //    for (int j = 0; j < currentNilrad.theNonFKhws.size; j ++)
  //    { out << " $" << currentNilrad.theNonFKhws[j].toStringLetterFormat("\\omega", &tempFormat) << "$";
  //      if (j != currentNilrad.theNonFKhws.size- 1)
  //        out << ", ";
  //    }
    }
    out << "\\end{longtable}";
  }
  for (int i = 0; i < this->FKNilradicalCandidates.size; i ++) {
    out << "<hr>Subalgebra " << i + 1 << ": " << this->FKNilradicalCandidates[i].toString(theFormat);
  }
  if (this->nilradicalGenerationLog != "") {
    out << "<br>Nilradical generation log:" << this->nilradicalGenerationLog;
  }
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::toStringPairingTableLaTeX(FormatExpressions* theFormat) const {
  if (!(this->NilradicalPairingTable.size > 0)) {
    return "";
  }
  (void) theFormat; //avoid unused parameter warning in a portable way.
  std::stringstream out;
  out << "\\documentclass{article}\\usepackage{longtable} \\begin{document}<br>";
  out << "modules that have a zero weight (" << this->modulesWithZeroWeights.size << " total): ";
  for (int i = 0; i < this->modulesWithZeroWeights.size; i ++) {
    out << "$W_{" << this->modulesWithZeroWeights[i] + 1 << "}$";
    if (i != this->modulesWithZeroWeights.size - 1) {
      out << ", ";
    }
  }
  out << "<br>Opposite modules <br>\n" << " \\begin{longtable}{c";
  for (int i = 0; i < this->OppositeModulesByStructure.size; i ++) {
    out << "|c";
  }
  out << "}\\\\ ";
  for (int i = 0; i < this->OppositeModulesByStructure.size; i ++) {
    out << "&$W_{" << i + 1 << "}$";
  }
  out << "\\\\\\hline Opp. by structure:";
  for (int i = 0; i < this->OppositeModulesByStructure.size; i ++) {
    out << "&";
    for (int j = 0; j < this->OppositeModulesByStructure[i].size; j ++) {
      out << "$W_{" << this->OppositeModulesByStructure[i][j] + 1 << "}$";
      if (j != this->OppositeModulesByStructure[i].size - 1) {
        out << ", ";
      }
    }
  }
  out << "\\\\\\hline Opposite character";
  for (int i = 0; i < this->OppositeModulesByChar.size; i ++) {
    out << "&";
    for (int j = 0; j < this->OppositeModulesByChar[i].size; j ++) {
      out << "$W_{" << this->OppositeModulesByChar[i][j] + 1 << "} $";
      if (j != this->OppositeModulesByChar[i].size - 1) {
        out << ", ";
      }
    }
  }
  out << "\\end{longtable}";
  out << "<br>\\begin{longtable}{c";
  for (int i = 0; i < this->modules.size; i ++) {
    out << "|c";
  }
  out << "} \\caption{Pairing table\\label{tablePairingTable}}\\\\<br>\n";
  FormatExpressions tempCharFormat;
  if (!this->charFormaT.isZeroPointer()) {
    tempCharFormat = this->charFormaT.getElementConst();
  }
  out << "<br>\n";
  for (int i = 0; i < this->NilradicalPairingTable.size; i ++) {
    out << "&$W_{" << i + 1 << "}$";
  }
  for (int i = 0; i < this->NilradicalPairingTable.size; i ++) {
    out << "\\\\\\hline<br\n>";
    out << "$W_{" << i + 1 << "}$";
    for (int j = 0; j < this->NilradicalPairingTable.size; j ++) {
      out << "&";
      for (int k = 0; k < this->NilradicalPairingTable[i][j].size; k ++) {
        out << "$W_{" << this->NilradicalPairingTable[j][i][k] + 1 << "}$";
        if (k != this->NilradicalPairingTable[i][j].size- 1) {
          out << ", ";
        }
      }
    }
  }
  out << "\\end{longtable}";
  out << "<br>\\end{document}";
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::toStringPairingTable(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::toStringPairingTable");
  (void) theFormat;
  if (!(this->NilradicalPairingTable.size > 0)) {
    return "";
  }
  std::stringstream out;
  out << "<br>Below is the pairing table of the modules, defined as follows. "
  << "Let g' be the subalgebra obtained by extending the current semisimple subalgebra "
  << "with the Cartan of the centralizer "
  << "(following Penkov, Serganova, Zuckermann,  we call a subalgebra g' arising in this fashion ``primal''). "
  << "Let A, B, C be  isotypic components in the decomposition of the ambient Lie algebra over g'. "
  << "We say that A and B pair to C if there exist "
  << "elements a in A and b in B such that  0\\neq [a,b]\\in C. "
  << "Note that, in general, A and B may pair to multiple modules C', C'', etc. We note that "
  << "if A and B pair to C then clearly C is isomorphic to some component "
  << "in the decomposition of A\\otimes B over g'. <br> We recall that W_{1}, W_{2}, ... are abbreviated notation "
  << "for the primal subalgebra modules indicated in the table above. ";
  out << "<br>modules that have a zero weight (" << this->modulesWithZeroWeights.size << " total): ";
  for (int i = 0; i < this->modulesWithZeroWeights.size; i ++) {
    out << "V_{" << this->modulesWithZeroWeights[i] + 1 << "}";
    if (i != this->modulesWithZeroWeights.size - 1) {
      out << ", ";
    }
  }
  out << "<br><table border =\"1\"><tr><td></td>";
  for (int i = 0; i < this->OppositeModulesByStructure.size; i ++) {
    out << "<td>W_{" << i + 1 << "}" << "</td>";
  }
  out << "</tr>";
  out << "<tr> <td>Non-compatible (a.k.a. ``opposite'') modules:</td>";
  for (int i = 0; i < this->OppositeModulesByStructure.size; i ++) {
    out << "<td>";
    for (int j = 0; j < this->OppositeModulesByStructure[i].size; j ++) {
      out << "W_{" << this->OppositeModulesByStructure[i][j] + 1 << "}";
      if (j != this->OppositeModulesByStructure[i].size - 1) {
        out << ", ";
      }
    }
    out << "</td>";
  }
  out << "</tr>";
  out << "<tr> <td>Opposite modules by character:</td>";
  for (int i = 0; i < this->OppositeModulesByChar.size; i ++) {
    out << "<td>";
    for (int j = 0; j < this->OppositeModulesByChar[i].size; j ++) {
      out << "W_{" << this->OppositeModulesByChar[i][j] + 1 << "}";
      if (j != this->OppositeModulesByChar[i].size - 1) {
        out << ", ";
      }
    }
    out << "</td>";
  }
  out << "</tr>";

  out << "</table>";
  out << "<br>modules corresponding to the semisimple subalgebra: ";
  Vector<Rational> theSAvector, tempV;
  theSAvector.makeZero(this->NilradicalPairingTable.size);
  for (int i = 0; i < this->subalgebraModules.size; i ++) {
    tempV.makeEi(this->NilradicalPairingTable.size, this->subalgebraModules[i]);
    theSAvector += tempV;
  }
  out << theSAvector.toStringLetterFormat("V");
  out << "<br><table><tr><td>modules</td>";
  FormatExpressions tempCharFormat;
  if (!this->charFormaT.isZeroPointer()) {
    tempCharFormat = this->charFormaT.getElementConst();
  }
  for (int i = 0; i < this->NilradicalPairingTable.size; i ++) {
    out << "<td><b>" << "W_{" << i + 1 << "}"
    //    << "=" << this->thePrimalChar[i].toString(&tempCharFormat)
    << "</b></td>";
  }
  out << "</tr>";
  for (int i = 0; i < this->NilradicalPairingTable.size; i ++) {
    out << "<tr><td> <b>" << "W_{" << i + 1 << "}</b></td>";
    for (int j = 0; j < this->NilradicalPairingTable.size; j ++) {
      out << "<td>";
      for (int k = 0; k < this->NilradicalPairingTable[i][j].size; k ++) {
        out << "W_{" << this->NilradicalPairingTable[j][i][k] + 1 << "}";
        if (k != this->NilradicalPairingTable[i][j].size - 1) {
          out << ", ";
        }
      }
      out << "</td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

void DynkinType::scaleFirstCoRootSquaredLength(const Rational& multiplyCoRootSquaredLengthBy) {
  DynkinType result;
  result.makeZero();
  result.setExpectedSize(this->size());
  DynkinSimpleType tempType;
  for (int i = 0; i < this->size(); i ++) {
    tempType = (*this)[i];
    tempType.CartanSymmetricInverseScale *= multiplyCoRootSquaredLengthBy;
    result.addMonomial(tempType, this->coefficients[i]);
  }
  *this = result;
}

std::string SemisimpleSubalgebras::toStringAlgebraLink(int actualindexSubalgebra, FormatExpressions* theFormat) const {
  if (actualindexSubalgebra < 0) {
    return "(non-initialized)";
  }
//  bool shortReportOnly = theFormat == 0 ? true : theFormat->flagCandidateSubalgebraShortReportOnly;
  bool useMouseHover = theFormat == nullptr ? true : !theFormat->flagUseMathSpanPureVsMouseHover;
  std::stringstream out;
  bool makeLink = theFormat == nullptr ? false : theFormat->flagUseHtmlAndStoreToHD;
  if (this->theSubalgebras.theValues[actualindexSubalgebra].flagSystemProvedToHaveNoSolution) {
    makeLink = false;
  }
  std::string theTypeString = this->theSubalgebras.theValues[actualindexSubalgebra].weylNonEmbedded->theDynkinType.toString();
  if (makeLink) {
    out << "<a href=\"" << this->getDisplayFileNameSubalgebraRelative(actualindexSubalgebra, theFormat)
    << "\" id = \"semisimple_subalgebra_" << this->getDisplayIndexFromActual(actualindexSubalgebra)
    << "\" style = \"text-decoration: none\">";
    if (useMouseHover) {
      out << HtmlRoutines::getMathNoDisplay(theTypeString);
    } else {
      out << HtmlRoutines::getMathNoDisplay(theTypeString);
    }
    out << "</a> ";
  } else {
    out << theTypeString << "";
  }
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::toStringCartanSubalgebra(FormatExpressions* theFormat) const {
  std::stringstream out;
  bool useLaTeX = theFormat == nullptr ? true : theFormat->flagUseLatex;
  bool useHtml = theFormat == nullptr ? true : theFormat->flagUseHTML;
  bool useMouseHover = theFormat == nullptr ? true : !theFormat->flagUseMathSpanPureVsMouseHover;

  List<DynkinSimpleType> theSimpleTypes;
  this->weylNonEmbedded->theDynkinType.getTypesWithMults(theSimpleTypes);
  FormatExpressions tempFormat;
  tempFormat.ambientWeylLetter = this->getAmbientWeyl().theDynkinType[0].theLetter;
  tempFormat.ambientCartanSymmetricInverseScale = this->getAmbientWeyl().theDynkinType[0].CartanSymmetricInverseScale;
  out << "<br>Elements Cartan subalgebra scaled to act by two by components: ";
  for (int i = 0; i < this->cartanSubalgebrasByComponentScaledToActByTwo.size; i ++) {
    if (useLaTeX && useHtml) {
      if (useMouseHover) {
        out << HtmlRoutines::getMathNoDisplay(theSimpleTypes[i].toString(&tempFormat), 1000) << ": ";
      } else {
        out << HtmlRoutines::getMathNoDisplay(theSimpleTypes[i].toString(&tempFormat), 1000) << ": ";
      }
    } else {
      out << theSimpleTypes[i].toString(&tempFormat) << ":";
    }
    for (int j = 0; j < this->cartanSubalgebrasByComponentScaledToActByTwo[i].size; j ++) {
      out << this->cartanSubalgebrasByComponentScaledToActByTwo[i][j].toString() << ": "
      << this->getAmbientWeyl().rootScalarCartanRoot(
        this->cartanSubalgebrasByComponentScaledToActByTwo[i][j],this->cartanSubalgebrasByComponentScaledToActByTwo[i][j]
      );
      if (j != this->cartanSubalgebrasByComponentScaledToActByTwo[i].size - 1 || i != this->cartanSubalgebrasByComponentScaledToActByTwo.size - 1) {
        out << ", ";
      }
    }
  }
  return out.str();
}

int CandidateSemisimpleSubalgebra::getSemisimplePartCentralizerOfSemisimplePartCentralizer() const {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::getSemisimplePartCentralizerOfSemisimplePartCentralizer");
  if (!this->flagCentralizerIsWellChosen) {
    return - 1;
  }
  if (this->indexSSPartCentralizer == - 1) {
    return this->owner->getIndexFullSubalgebra();
  }
  return this->owner->theSubalgebras.theValues[this->indexSSPartCentralizer].indexSSPartCentralizer;
}

std::string CandidateSemisimpleSubalgebra::toStringCentralizer(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::toStringCentralizer");
  if (this->flagSystemProvedToHaveNoSolution) {
    return "";
  }
  std::stringstream out;
  bool useMouseHover = theFormat == nullptr ? true : !theFormat->flagUseMathSpanPureVsMouseHover;
  if (this->flagCentralizerIsWellChosen && this->centralizerRank != 0 ) {
    out << "<br>Centralizer: ";
    Rational dimToralPartCentralizer = this->centralizerRank;
    if (this->indexSSPartCentralizer != - 1) {
      CandidateSemisimpleSubalgebra& centralizerSSpart = this->owner->theSubalgebras.theValues[this->indexSSPartCentralizer];
      out << this->owner->toStringAlgebraLink(this->indexSSPartCentralizer, theFormat);
      dimToralPartCentralizer -= centralizerSSpart.getRank();
      if (dimToralPartCentralizer != 0) {
        out << " + ";
      }
    } else if (!this->theCentralizerType.isEqualToZero()) {
      if (useMouseHover) {
        out << HtmlRoutines::getMathNoDisplay(this->theCentralizerType.toString());
      } else {
        out << HtmlRoutines::getMathNoDisplay(this->theCentralizerType.toString());
      }
      out << " (can't determine subalgebra number - subalgebras computed partially?)";
      dimToralPartCentralizer -= this->theCentralizerType.getRank();
    }
    if (dimToralPartCentralizer != 0) {
      std::stringstream toralPartStream;
      toralPartStream << "T_{" << dimToralPartCentralizer.toString() << "}";
      if (useMouseHover) {
        out << HtmlRoutines::getMathNoDisplay(toralPartStream.str());
      } else {
        out << HtmlRoutines::getMathNoDisplay(toralPartStream.str());
      }
      out << " (toral part, subscript = dimension)";
    }
    out << ". ";
  }
  if (this->flagCentralizerIsWellChosen && this->centralizerRank == 0) {
    out << "<br>Centralizer: 0";
  }
  if (this->flagCentralizerIsWellChosen && this->getSemisimplePartCentralizerOfSemisimplePartCentralizer() != - 1) {
    out << "<br>The semisimple part of the centralizer of the semisimple part of my centralizer: "
    << this->owner->toStringAlgebraLink(this->getSemisimplePartCentralizerOfSemisimplePartCentralizer(), theFormat);
  }
  if (!this->flagCentralizerIsWellChosen) {
    out << "<br><b style = 'color:red'>The Cartan of the centralizer "
    << "does not lie in the ambient Cartan (the computation was too large?).</b> "
    << "The intersection of the ambient Cartan with the centralizer is of dimension "
    << this->CartanOfCentralizer.size << " instead of the desired dimension "
    << this->centralizerRank.toString() << ". ";
    //else
    //  out << "<br><span style =\"color:#FF0000\"><b>I was unable to compute the centralizer (not all subalgebras were computed?).</b></span> ";
  }
  if (this->centralizerRank != 0) {
    out << "<br>Basis of Cartan of centralizer: ";
    out << this->CartanOfCentralizer.toString();
  }
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::toStringCentralizerDebugData(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::toStringCentralizerDebugData");
  (void) theFormat; //avoid unused parameter warning in a portable way.
  std::stringstream out;
  out << "Subalgebra of type: " << this->weylNonEmbedded->theDynkinType.toString()
  << ". this->centralizerRank computed: " << this->centralizerRank.toString()
  << ". this->centralizerDimension: " << this->centralizerDimension.toString()
  << ". The max semisimple subalgebra container computed: "
  << this->owner->theSubalgebras.theValues[this->indexMaxSSContainer].weylNonEmbedded->theDynkinType.toString()
  << ". The rank of the ambient Lie algebra is " << this->owner->owner->getRank()
  << ". The indices of the direct containers of the subalgebra are: ";
  for (int k = 0; k < this->indicesDirectSummandSuperAlgebra.size; k ++) {
    out << this->owner->theSubalgebras.theValues[this->indicesDirectSummandSuperAlgebra[k]].weylNonEmbedded->theDynkinType.toString()
    << ", ";
  }
  return out.str();
}

void CandidateSemisimpleSubalgebra::computeCentralizerIsWellChosen() {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::computeCentralizerIsWellChosen");
  if (this->flagSystemProvedToHaveNoSolution) {
    global.comments << "This is unexpected, but not considered an error: "
    << "I am asked to compute the centralizer of a candidate for a subalgebra which cannot be realized. ";
    this->flagCentralizerIsWellChosen = false;
    return;
  }
  Weight<Rational> theZeroWeight;
  theZeroWeight.owner = nullptr;
  theZeroWeight.weightFundamentalCoordS.makeZero(this->theHs.size);
  this->centralizerDimension = this->theCharNonPrimalFundCoords.getCoefficientOf(theZeroWeight);
  this->centralizerRank = this->centralizerDimension;
  if (this->centralizerRank == 0) {
    this->flagCentralizerIsWellChosen = true;
    return;
  }
  ProgressReport theReport1;
  DynkinType centralizerTypeAlternative;
  if (this->indexMaxSSContainer != - 1) {
    centralizerTypeAlternative = this->owner->theSubalgebras.theValues[this->indexMaxSSContainer].weylNonEmbedded->theDynkinType;
    centralizerTypeAlternative -= this->weylNonEmbedded->theDynkinType;
    if (theReport1.tickAndWantReport()) {
      std::stringstream reportStream;
      reportStream << "<hr>The centralizer of subalgebra of type "
      << this->weylNonEmbedded->theDynkinType.toString() << " is of type "
      << centralizerTypeAlternative.toString();
      theReport1.report(reportStream.str());
    }
    this->centralizerRank -= centralizerTypeAlternative.getRootSystemSize();
    if (this->RootSystemCentralizerPrimalCoords.size > 0) {
      if (centralizerTypeAlternative != this->theCentralizerType) {
        global.fatal << "Two different methods "
        << "for computing the centralizer type yield different results: "
        << "by sub-diagram I computed the type as "
        << this->theCentralizerType.toString()
        << " but looking at subalgerba containing the current one I got centralizer type "
        << centralizerTypeAlternative.toString() << global.fatal;
      }
    }
    if (this->centralizerRank>this->owner->owner->getRank()) {
      global.fatal << "Something is wrong." << this->toStringCentralizerDebugData() << global.fatal;
    }
  } else {
    if (this->centralizerDimension + this->getRank() > this->owner->owner->getRank()) {
      global.fatal << "Something is wrong. " << this->toStringCentralizerDebugData() << global.fatal;
    }
  }
  this->flagCentralizerIsWellChosen = (this->centralizerRank == this->CartanOfCentralizer.size);
  if (this->flagCentralizerIsWellChosen) {
    this->flagCentralizerTypeIsComputed = true;
  }
  if (this->indexMaxSSContainer != - 1 && this->flagCentralizerIsWellChosen) {
    for (int i = 0; i < this->owner->theSubalgebras.theValues.size; i ++) {
      if (centralizerTypeAlternative == this->owner->theSubalgebras.theValues[i].weylNonEmbedded->theDynkinType) {
        if (this->owner->theSubalgebras.theValues[i].indicesDirectSummandSuperAlgebra.contains(this->indexMaxSSContainer)) {
          this->indexSSPartCentralizer = i;
          break;
        }
      }
    }
  }
}

std::string CandidateSemisimpleSubalgebra::toStringSystem(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::toStringSystem");
  std::stringstream out;
  if (this->flagSystemSolved) {
    out << "<br>Subalgebra realized. ";
    if (!this->flagCentralizerIsWellChosen) {
      out << "<b>However, the centralizer is not well chosen.</b>";
    }
  } else if (this->flagSystemProvedToHaveNoSolution) {
    out << "<br><b> Subalgebra candidate cannot be realized. </b> ";
  } else {
    out << "<br><b>Subalgebra candidate not realized, and this may or may not be possible.</b>";
  }
  out << "<br>" << this->theUnknownNegGens.size << "*2 (unknown) gens:<br>(";
  for (int i = 0; i < this->theUnknownNegGens.size; i ++) {
    out << "<br>" << this->theUnknownNegGens[i].toString(theFormat) << ", " ;
    out << this->theUnknownPosGens[i].toString(theFormat);
    if (i != this->theUnknownNegGens.size - 1) {
      out << ", ";
    }
  }
  out << ")<br>";
  if (this->theUnknownCartanCentralizerBasis.size > 0) {
    out << "<br>Unknown splitting cartan of centralizer.<br>\n";
    for (int i = 0; i < this->theUnknownCartanCentralizerBasis.size; i ++) {
      out << this->theUnknownCartanCentralizerBasis[i].toString();
      if (i != this->theUnknownCartanCentralizerBasis.size - 1) {
        out << ", ";
      }
    }
    out << "<br>";
  }
  for (int i = 0; i < this->theHs.size; i ++) {
    out << "h: " << this->theHs[i];
    if (i < this->theInvolvedPosGenerators.size) {
      out << ", e = combination of " << this->theInvolvedPosGenerators[i].toString();
    } else {
      out << ", involved positive generators unknown";
    }
    if (i < this->theInvolvedNegGenerators.size) {
      out << ", f= combination of " << this->theInvolvedNegGenerators[i].toString();
    } else {
      out << ", involved negative generators unknown.";
    }
  }
  out << "Positive weight subsystem: " << this->weylNonEmbedded->rootsOfBorel.toString();
  if (this->PosRootsPerpendicularPrecedingWeights.size > 0) {
    out << " Positive roots that commute with the weight subsystem: "
    << this->PosRootsPerpendicularPrecedingWeights.toString();
  }
  out << "<br>Symmetric Cartan default scale: "
  << this->theSubalgebraNonEmbeddedDefaultScale->theWeyl.cartanSymmetric.toString(theFormat);
  out << "Character ambient Lie algebra: " << this->theCharFundamentalCoordsRelativeToCartan.toString();
  out << "<br>A necessary system to realize the candidate subalgebra.  ";
  FormatExpressions tempFormat;
  for (int i = 0; i < this->theSystemToSolve.size; i ++) {
    out << "<br>" << this->theSystemToSolve[i].toString(&tempFormat) << "= 0";
  }
  out << "<br>The above system after transformation.  ";
  for (int i = 0; i < this->transformedSystem.size; i ++) {
    out << "<br>" << this->transformedSystem[i].toString(&tempFormat) << "= 0";
  }
  if (!this->flagSystemGroebnerBasisFound) {
    out << "<br><b>Failed to find Groebner basis of the above system (the computation is too large).</b>";
  }
  out << this->toStringSystemPart2(theFormat);
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::toStringSystemPart2(FormatExpressions* theFormat) const {
  (void) theFormat;//avoid unused parameter warning in a portable way
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::toStringSystem");
  std::stringstream out;
  out << "<br><b>For the calculator: </b><br>\n" << this->toStringLoadUnknown() << ";"
  << "<br>findOneSolutionSerreLikePolynomialSystem{}( ";
  for (int i = 0; i < this->theSystemToSolve.size; i ++) {
    out << this->theSystemToSolve[i].toString();
    if (i != this->theSystemToSolve.size - 1) {
      out << ", ";
    }
  }
  out << " )";
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::toStringLoadUnknown(FormatExpressions* theFormat) const {
  (void) theFormat;//avoid unused parameter warning in a portable way.
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::toStringLoadUnknown");
  std::stringstream out;
  out << "(";
  out << "DynkinType =" << this->weylNonEmbedded->theDynkinType.toString() << "; ElementsCartan =";
  out << "(";
  for (int i = 0; i < this->theHs.size; i ++) {
    out << this->theHs[i].toString();
    if (i != this->theHs.size - 1) {
      out << ", ";
    }
  }
  out << ")";
  out << "; generators =(";
  for (int i = 0; i < this->theUnknownNegGens.size; i ++) {
    out << this->theUnknownNegGens[i].toString() << ", " <<  this->theUnknownPosGens[i].toString();
    if (i != this->theUnknownNegGens.size - 1) {
      out << ", ";
    }
  }
  out << ") )";
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::toStringType(FormatExpressions* theFormat) const {
  return this->weylNonEmbedded->theDynkinType.toString(theFormat);
}

std::string CandidateSemisimpleSubalgebra::toStringTypeAndHs(FormatExpressions* theFormat) const {
  std::stringstream out;
  out << this->weylNonEmbedded->theDynkinType.toString(theFormat) << ", Cartan: " << this->theHs.toString(theFormat);
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::toStringGenerators(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::toStringGenerators");
  if (this->theBasis.size == 0) {
    return "";
  }
  bool useLaTeX = theFormat == nullptr ? true : theFormat->flagUseLatex;
  bool useHtml = theFormat == nullptr ? true : theFormat->flagUseHTML;
  bool shortReportOnly = theFormat == nullptr ? true : theFormat->flagCandidateSubalgebraShortReportOnly;
  bool useMouseHover = theFormat == nullptr ? true : ((shortReportOnly && !theFormat->flagUseMathSpanPureVsMouseHover) || !theFormat->flagUseHtmlAndStoreToHD);
  std::stringstream out;
  out << "<br>Dimension of subalgebra generated by predefined or computed generators: "
  << this->theBasis.size << ". " << "<br>Negative simple generators: ";
  for (int i = 0; i < this->theNegGens.size; i ++) {
    if (useHtml && useLaTeX) {
      if (useMouseHover) {
        out << HtmlRoutines::getMathNoDisplay(this->theNegGens[i].toString(theFormat), 2000);
      } else {
        out << HtmlRoutines::getMathNoDisplay(this->theNegGens[i].toString(theFormat), 2000);
      }
    } else {
      out << this->theNegGens[i].toString(theFormat);
    }
    if (i != this->theNegGens.size - 1) {
      out << ", ";
    }
  }
  out << "<br>Positive simple generators: ";
  for (int i = 0; i < this->thePosGens.size; i ++) {
    if (useHtml && useLaTeX) {
      if (useMouseHover) {
        out << HtmlRoutines::getMathNoDisplay(this->thePosGens[i].toString(theFormat), 2000);
      } else {
        out << HtmlRoutines::getMathNoDisplay(this->thePosGens[i].toString(theFormat), 2000);
      }
    } else {
      out << this->thePosGens[i].toString(theFormat);
    }
    if (i != this->thePosGens.size - 1) {
      out << ", ";
    }
  }
  return out.str();
}

bool CandidateSemisimpleSubalgebra::amRegularSA() const {
  for (int i = 0; i < this->theNegGens.size; i ++) {
    if (this->theNegGens[i].size() > 1 || this->thePosGens[i].size() > 1) {
      return false;
    }
  }
  return true;
}

std::string CandidateSemisimpleSubalgebra::toString(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::toString");
  std::stringstream out;
  bool useLaTeX = theFormat == nullptr ? true : theFormat->flagUseLatex;
  bool useHtml = theFormat == nullptr ? true : theFormat->flagUseHTML;
  bool writingToHD = theFormat == nullptr ? false : theFormat->flagUseHtmlAndStoreToHD;
  bool shortReportOnly = theFormat == nullptr ? true : theFormat->flagCandidateSubalgebraShortReportOnly;
  bool useMouseHover = theFormat == nullptr ? true : !theFormat->flagUseMathSpanPureVsMouseHover;
  out << "Subalgebra type: " << this->owner->toStringAlgebraLink(this->indexInOwner, theFormat)
  << " (click on type for detailed printout).\n";
  out << this->comments;
  if (this->amRegularSA()) {
    out << "<br>The subalgebra is regular (= the semisimple part of a root subalgebra). ";
  }
  if (this->indexIamInducedFrom != - 1) {
    out << "<br>Subalgebra is (parabolically) induced from "
    << this->owner->toStringAlgebraLink(this->indexIamInducedFrom, theFormat) << ". ";
  }
  if (!this->flagCentralizerIsWellChosen) {
    out << "<br>The dimension of the centralizer is: " << this->centralizerDimension.toString();
  }
  out << this->toStringCentralizer(theFormat);
  if (this->flagSystemProvedToHaveNoSolution) {
    out << " <b> Subalgebra candidate proved to be impossible! </b> ";
    return out.str();
  }
  bool weightsAreCoordinated = true;
  if (this->modules.size != this->WeightsModulesNONprimal.size) {
    weightsAreCoordinated = false;
  } else {
    for (int i = 0; i < this->modules.size; i ++) {
      if (this->modules[i].size != this->WeightsModulesNONprimal[i].size) {
        weightsAreCoordinated = false;
        break;
      }
    }
  }
  if (this->indicesDirectSummandSuperAlgebra.size > 0) {
    out << "<br>Contained up to conjugation as a direct summand of: ";
    for (int i = 0; i < this->indicesDirectSummandSuperAlgebra.size; i ++) {
      out << this->owner->toStringAlgebraLink(this->indicesDirectSummandSuperAlgebra[i], theFormat);
      if (i != this->indicesDirectSummandSuperAlgebra.size - 1) {
        out << ", ";
      }
    }
    out << ". ";
  }
  out << "<br>" << this->toStringCartanSubalgebra(theFormat);
  bool displayNilradSummary = (this->owner->flagComputeNilradicals && this->flagCentralizerIsWellChosen && this->flagSystemSolved);
  if (displayNilradSummary) {
    displayNilradSummary = !shortReportOnly || (this->NumBadParabolics > 0);
  }
  if (displayNilradSummary) {
    out << "<hr>" << this->toStringNilradicalsSummary(theFormat);
    out << "<a href=\"" << this->owner->getDisplayFileNameFKFTNilradicals(this->indexInOwner, theFormat)
    << "\"> Detailed information on isotypical nilradicals. </a><hr>";
  }
  out << this->toStringGenerators(theFormat);
  FormatExpressions tempFormat;
  tempFormat.flagUseLatex = true;
  tempFormat.flagUseHTML = false;
  out << "<br>Cartan symmetric matrix: ";
  if (useLaTeX && useHtml) {
    if (useMouseHover) {
      out << HtmlRoutines::getMathNoDisplay(this->weylNonEmbedded->cartanSymmetric.toString(&tempFormat));
    } else {
      out << HtmlRoutines::getMathNoDisplay(this->weylNonEmbedded->cartanSymmetric.toString(&tempFormat));
    }
  } else {
    out << this->weylNonEmbedded->cartanSymmetric.toString(theFormat);
  }
  out << "<br>Scalar products of elements of Cartan subalgebra scaled to act by 2 (co-symmetric Cartan matrix): ";
  if (useLaTeX && useHtml) {
    if (useMouseHover) {
      out << HtmlRoutines::getMathNoDisplay(this->weylNonEmbedded->coCartanSymmetric.toString(&tempFormat));
    } else {
      out << HtmlRoutines::getMathNoDisplay(this->weylNonEmbedded->coCartanSymmetric.toString(&tempFormat));
    }
  } else {
    out << this->weylNonEmbedded->coCartanSymmetric.toString(theFormat);
  }
  FormatExpressions charFormatNonConst;
  if (!this->charFormaT.isZeroPointer()) {
    charFormatNonConst = this->charFormaT.getElementConst();
  }
  out << "<br>Decomposition of ambient Lie algebra: ";
  if (useLaTeX) {
    if (useMouseHover) {
      out << HtmlRoutines::getMathNoDisplay(this->theCharNonPrimalFundCoords.toString(&charFormatNonConst), 20000);
    } else {
      out << HtmlRoutines::getMathNoDisplay(this->theCharNonPrimalFundCoords.toString(&charFormatNonConst), 20000);
    }
  } else {
    out << this->theCharNonPrimalFundCoords.toString(&charFormatNonConst);
  }
  if (this->CartanOfCentralizer.size > 0) {
    out << "<br>Primal decomposition of the ambient Lie algebra. "
    << "This decomposition refines the above decomposition (please note the order is not the same as above). ";
    if (useLaTeX) {
      if (useMouseHover) {
        out << HtmlRoutines::getMathNoDisplay(this->thePrimalChaR.toString(&charFormatNonConst), 20000);
      } else {
        out << HtmlRoutines::getMathNoDisplay(this->thePrimalChaR.toString(&charFormatNonConst), 20000);
      }
    } else
      out << this->thePrimalChaR.toString(&charFormatNonConst);
  }
  if (this->flagCentralizerIsWellChosen&& weightsAreCoordinated) {
    int numZeroWeights = 0;
    out << "<br>The number of zero weights w.r.t. the "
    << "Cartan subalgebra minus the dimension of the centralizer of the subalgebra equals " ;
    for (int i = 0; i < this->modules.size; i ++) {
      for (int j = 0; j < this->WeightsModulesNONprimal[i].size; j ++) {
        if (this->WeightsModulesNONprimal[i][j].isEqualToZero()) {
          numZeroWeights += this->modules[i].size;
        }
      }
    }
    out << numZeroWeights << " - " << this->centralizerRank << "="
    << ((this->centralizerRank - numZeroWeights) * (- 1)).toString() << ".";
  }
  if (this->flagSystemSolved && !shortReportOnly) {
    out << "<br>In the table below we indicate the highest weight "
    << "vectors of the decomposition of "
    << " the ambient Lie algebra as a module over the semisimple part. The second row indicates "
    << " weights of the highest weight vectors relative to the Cartan of the semisimple subalgebra. ";
    if (this->flagCentralizerIsWellChosen && this->CartanOfCentralizer.size > 0) {
      out << "As the centralizer is well-chosen and the centralizer of our subalgebra is "
      << "non-trivial, we may in addition split highest weight vectors"
      << " with the same weight over the semisimple part over the centralizer "
      << "(recall that the centralizer preserves the weights over the subalgebra "
      << " and in particular acts on the highest weight vectors). "
      << "Therefore we have chosen our highest weight vectors to be, in addition, weight vectors"
      << " over the Cartan of the centralizer of the  starting subalgebra. "
      << "Their weight over the sum of the Cartans of the semisimple subalgebra and "
      << " its centralizer is indicated in the third row. "
      << "The weights corresponding to the Cartan of the "
      << "centralizer are again indicated with the letter"
      << " \\omega. As there is no preferred way of chosing a basis of "
      << "the Cartan of the centralizer (unlike the starting semisimple Lie algebra:"
      << " there we have a preferred basis induced by the fundamental weights), "
      << "our centralizer weights are simply given by the constant by which the "
      << " k^th basis element of the Cartan of the centralizer acts on the "
      << "highest weight vector. Here, we use the choice for basis of the Cartan "
      << "of the centralizer given at the start of the page.<br><br>";
    }
    out << "<table class = 'tableStandard'><tr><td>Highest vectors of representations (total "
    << this->highestVectorsNonSorted.size << ") ";
    if (this->flagCentralizerIsWellChosen) {
      out << "; the vectors are over the primal subalgebra.";
    }
    out << "</td>";
    for (int i = 0; i < this->highestVectorsNonSorted.size; i ++) {
      out << "<td>\\(" << this->highestVectorsNonSorted[i].toString() << "\\)</td>";
    }
    out << "</tr><tr><td>weight</td>";
    for (int i = 0; i < this->HighestWeightsNONprimalNonSorted.size; i ++) {
      out << "<td>\\("
      << this->HighestWeightsNONprimalNonSorted[i].toStringLetterFormat("\\omega", &charFormatNonConst)
      << "\\)</td>";
    }
    out << "</tr>";
    if (this->flagCentralizerIsWellChosen && this->CartanOfCentralizer.size > 0) {
      out << "<tr><td>weights rel. to Cartan of (centralizer+semisimple s.a.). </td>";
      for (int i = 0; i < this->HighestWeightsPrimalNonSorted.size; i ++) {
        out << "<td>\\("
        << this->HighestWeightsPrimalNonSorted[i].toStringLetterFormat("\\omega", &charFormatNonConst)
        << "\\)</td>";
      }
      out << "</tr>";
    }
    out << "</table>";
  }
  if (!shortReportOnly) {
    out << this->toStringModuleDecomposition(theFormat);
    if (this->owner->flagProduceLaTeXtables) {
      out << "LaTeX version of module decomposition: <br><br>" << this->toStringModuleDecompositionLaTeX(theFormat) << "<br><br>";
    }
    out << this->toStringPairingTable(theFormat);
    if (this->owner->flagProduceLaTeXtables) {
      out << "LaTeX version of pairing table: <br><br>" << this->toStringPairingTableLaTeX(theFormat) << "<br><br>";
    }
    out << "<br>" << this->toStringDrawWeights(theFormat) << "<br>";
  }
  bool shouldDisplaySystem = false;
  if (this->totalArithmeticOpsToSolveSystem != 0) {
    out << "<br>Made total " << totalArithmeticOpsToSolveSystem
    << " arithmetic operations while solving the Serre relations polynomial system. ";
    if (this->totalArithmeticOpsToSolveSystem > 1000000 && !shortReportOnly) {
      shouldDisplaySystem = true;
      out << "<br>The total number of arithmetic operations "
      << "I needed to solve the Serre relations "
      << "polynomial system was larger than 1 000 000. I am printing out "
      << "the Serre relations system for you: maybe "
      << "that can help improve the polynomial system algorithms. ";
    }
  }
  shouldDisplaySystem = shouldDisplaySystem || !this->flagSystemSolved || !this->flagCentralizerIsWellChosen;
  if (writingToHD) {
    shouldDisplaySystem = shouldDisplaySystem && !shortReportOnly;
  }
  if (shouldDisplaySystem) {
    out << this->toStringSystem(theFormat);
  }
  return out.str();
}

void CandidateSemisimpleSubalgebra::GetHsScaledToActByTwoByType(
  List<List<Vectors<Rational> > >& outputHsByType, List<DynkinSimpleType>& outputTypeList
) const {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::GetHsByType");
  List<DynkinSimpleType> allTypes;
  this->weylNonEmbedded->theDynkinType.getTypesWithMults(allTypes);
  outputHsByType.setSize(0);
  outputTypeList.setSize(0);
  if (allTypes.size != this->cartanSubalgebrasByComponentScaledToActByTwo.size) {
    global.fatal << "AllTypes.size must equal this->cartanSubalgebrasByComponentScaledToActByTwo.size. " << global.fatal;
  }
  for (int i = 0; i < allTypes.size; i ++) {
    bool shouldOpenNewType = true;
    if (i != 0) {
      shouldOpenNewType = !(allTypes[i] == allTypes[i - 1]);
    }
    if (shouldOpenNewType) {
      outputHsByType.setSize(outputHsByType.size + 1);
      outputHsByType.lastObject()->setSize(0);
      outputTypeList.addOnTop(allTypes[i]);
    }
    outputHsByType.lastObject()->addOnTop(this->cartanSubalgebrasByComponentScaledToActByTwo[i]);
  }
}

bool CandidateSemisimpleSubalgebra::hasHsScaledByTwoConjugateTo(List<Vector<Rational> >& input) const {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::HasConjugateHsTo");
  if (input.size != this->theHsScaledToActByTwo.size) {
    return false;
  }
  List<Vector<Rational> > raisedInput = input;
  List<Vector<Rational> > myVectors = this->theHsScaledToActByTwo;
  WeylGroupAutomorphisms& ambientWeylAutomorphisms = this->getAmbientWeylAutomorphisms();

  ambientWeylAutomorphisms.raiseToMaximallyDominant(raisedInput);
  ambientWeylAutomorphisms.raiseToMaximallyDominant(myVectors);
  return myVectors == raisedInput;
}

std::string SimpleReflectionOrOuterAutomorphism::toString() const {
  std::stringstream out;
  if (!this->flagIsOuter) {
    out << "s_{";
    out << this->index + 1;
    out << "}";
  } else {
    out << "a_{" << this->index << "}";
  }
  return out.str();
}

std::string SimpleReflection::toString() const {
  std::stringstream out;
  out << "s_{";
  out << this->index + 1;
  out << "}";
  return out.str();
}

bool CandidateSemisimpleSubalgebra::isDirectSummandOf(const CandidateSemisimpleSubalgebra& other) {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::isDirectSummandOf");
  if (other.flagSystemProvedToHaveNoSolution) {
    return false;
  }
  DynkinType theDifference;
  theDifference = other.weylNonEmbedded->theDynkinType;
  theDifference -= this->weylNonEmbedded->theDynkinType;
  for (int i = 0; i < theDifference.size(); i ++) {
    if (theDifference.coefficients[i] < 0) {
      return false;
    }
  }
  Incrementable<SelectionFixedRank> selectedTypes;
  List<DynkinSimpleType> isoTypes;
  SelectionFixedRank currentTypeSelection;
  List<List<Vectors<Rational> > > theHsScaledToActByTwoByType;
  other.GetHsScaledToActByTwoByType(theHsScaledToActByTwoByType, isoTypes);
  for (int i = 0; i < isoTypes.size; i ++) {
    Rational ratMult = this->weylNonEmbedded->theDynkinType.getCoefficientOf(isoTypes[i]);
    int intMult = 0;
    if (!ratMult.isSmallInteger(&intMult)) {
      return false;
    }
    currentTypeSelection.setNumberOfItemsAndDesiredSubsetSize(intMult, theHsScaledToActByTwoByType[i].size);
    selectedTypes.theElements.addOnTop(currentTypeSelection);
  }
  FinitelyGeneratedMatrixMonoid<Rational> theOuterAutos;
  this->weylNonEmbedded->theDynkinType.getOuterAutosGeneratorsActOnVectorColumn(theOuterAutos.theGenerators);
  for (int i = 0; i < theOuterAutos.theGenerators.size; i ++) {
    theOuterAutos.theGenerators[i].transpose();
  }
  bool mustBeTrue = theOuterAutos.generateElements(100000);
  if (!mustBeTrue) {
    global.fatal << "Failed to generate outer automorphisms of Dynkin simple type. "
    << "The upper limit for such automorphism group size is 100000. " << global.fatal;
  }
  Rational numCyclesFromTypes = selectedTypes.totalCombinations();
  if (!numCyclesFromTypes.isSmallInteger()) {
    global.fatal << "Computation is too large: "
    << "I am crashing to let you know that the program cannot "
    << "handle such a large number of outer automorphisms. " << global.fatal;
  }
  List<Vector<Rational> > conjugationCandidates;
  Vectors<Rational> currentComponent;
  ProgressReport theReport;
  if (theReport.tickAndWantReport()) {
    std::stringstream reportStream;
    reportStream << "Computing whether "
    << this->weylNonEmbedded->theDynkinType << " is direct summand of "
    << other.weylNonEmbedded->theDynkinType.toString()
    << ". The outer automorphisms of the smaller type are: ";
    FormatExpressions theFormat;
    theFormat.flagUseHTML = false;
    theFormat.flagUseLatex = true;
    for (int i = 0; i < theOuterAutos.theElements.size; i ++) {
      if (i >= 100) {
        reportStream << "... and so on, only the first 100 elements printed out of total "
        << theOuterAutos.theElements.size << ". ";
      } else {
        reportStream << "<br>" << HtmlRoutines::getMathNoDisplay(theOuterAutos.theElements[i].toStringMatrixForm(&theFormat));
      }
    }
    theReport.report(reportStream.str());
  }
  do {
    for (int k = 0; k < theOuterAutos.theElements.size; k ++) {
      conjugationCandidates.setSize(0);
      for (int i = 0; i < selectedTypes.theElements.size; i ++) {
        Selection& currentSel = selectedTypes.theElements[i].theSelection;
        for (int j = 0; j < currentSel.cardinalitySelection; j ++) {
          currentComponent = theHsScaledToActByTwoByType[i][currentSel.elements[j]];
          conjugationCandidates.addListOnTop(currentComponent);
        }
      }
      theOuterAutos.theElements[k].actOnVectorROWSOnTheLeft(conjugationCandidates, conjugationCandidates);
      if (this->hasHsScaledByTwoConjugateTo(conjugationCandidates)) {
        return true;
      }
    }
  } while (selectedTypes.incrementReturnFalseIfPastLast());
  return false;
}

void CandidateSemisimpleSubalgebra::adjustCentralizerAndRecompute(bool allowNonPolynomialSystemFailure) {
  MacroRegisterFunctionWithName("CandidateSemisimpleSubalgebra::adjustCentralizerAndRecompute");
  if (this->flagSystemProvedToHaveNoSolution) {
    return;
  }
  this->computeCentralizerIsWellChosen();
  if (this->owner->flagAttemptToAdjustCentralizers) {
    if (!this->flagCentralizerIsWellChosen) {
      this->computeSystem(true, allowNonPolynomialSystemFailure);
      this->computeCentralizerIsWellChosen();
    }
  }
}

int CandidateSemisimpleSubalgebra::getNumberOfModules() const {
  int result = 0;
  for (int i = 0; i < this->modules.size; i ++) {
    result += this->modules[i].size;
  }
  return result;
}

void SemisimpleSubalgebras::computePairingTablesAndFKFTtypes() {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::computePairingTablesAndFKFTtypes");
  ProgressReport theReport;
  for (int i = 0; i < this->theSubalgebras.theValues.size; i ++) {
    CandidateSemisimpleSubalgebra& currentSA = this->theSubalgebras.theValues[i];
    if (!currentSA.flagCentralizerIsWellChosen || !currentSA.flagSystemSolved) {
      continue;
    }
    if (!this->flagcomputePairingTable) {
      continue;
    }
    if (theReport.tickAndWantReport()) {
      std::stringstream reportStream2;
      reportStream2 << "Computing pairing table of subalgebra number " << i + 1
      << " out of " << this->theSubalgebras.theValues.size
      << ". The subalgebra is of type " << this->theSubalgebras.theValues[i].toStringTypeAndHs() << "... ";
      theReport.report(reportStream2.str());
    }
    if (currentSA.amRegularSA()) {
      continue;
    }
    currentSA.computePairingTable();
    if (theReport.tickAndWantReport()) {
      std::stringstream reportStream2;
      reportStream2 << "Computing pairing table of subalgebra number " << i + 1
      << " out of " << this->theSubalgebras.theValues.size
      << ". The subalgebra is of type " << this->theSubalgebras.theValues[i].toStringTypeAndHs()
      << "... DONE. Computing Fernando-Kac subalgebra candidates.";
      theReport.report(reportStream2.str());
    }
    if (this->flagComputeNilradicals && !this->theSubalgebras.theValues[i].amRegularSA()) {
      currentSA.enumerateAllNilradicals();
    }
  }
}

void SemisimpleSubalgebras::hookUpCentralizers(bool allowNonPolynomialSystemFailure) {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::hookUpCentralizers");
  List<int> theCandidatePermutation;
  theCandidatePermutation.setSize(this->theSubalgebras.theValues.size);
  for (int i = 0; i < theCandidatePermutation.size; i ++) {
    theCandidatePermutation[i] = i;
  }

  this->theSubalgebras.theValues.quickSortAscending(nullptr, &theCandidatePermutation);
  List<Vectors<Rational> > hsCopy;
  hsCopy.setSize(theCandidatePermutation.size);
  for (int i = 0; i < theCandidatePermutation.size; i ++) {
    hsCopy[i] = this->theSubalgebras.theKeys[theCandidatePermutation[i]];
  }
  this->theSubalgebras.theKeys = hsCopy;

  HashedList<int, MathRoutines::IntUnsignIdentity> theCandidatePermutationHashed;
  theCandidatePermutationHashed = theCandidatePermutation;
  ProgressReport theReport1, theReport2;
  theReport1.report("<hr>\nHooking up centralizers ");
  for (int i = 0; i < this->theSubalgebras.theValues.size; i ++) {
    if (!this->theSubalgebras.theValues[i].flagSystemSolved) {
      continue;
    }
    if (!this->theSubalgebras.theValues[i].computeAndVerifyFromGeneratorsAndHs()) {
      continue;
    }
    CandidateSemisimpleSubalgebra& currentSA = this->theSubalgebras.theValues[i];
    std::stringstream reportStream2;
    reportStream2 << "Computing centralizer of subalgebra number " << i + 1
    << " out of " << this->theSubalgebras.theValues.size
    << ". The subalgebra is of type " << currentSA.toStringTypeAndHs() << ". ";
    theReport2.report(reportStream2.str());
    currentSA.indexInOwner = i;
    currentSA.indexIamInducedFrom = theCandidatePermutationHashed.getIndex(currentSA.indexIamInducedFrom);
    currentSA.indicesDirectSummandSuperAlgebra.setSize(0);
    currentSA.indexMaxSSContainer = - 1;
    for (int j = 0; j < this->theSubalgebras.theValues.size; j ++) {
      if (i == j) {
        continue;
      }
      CandidateSemisimpleSubalgebra& otherSA = this->theSubalgebras.theValues[j];
      if (currentSA.isDirectSummandOf(otherSA)) {
        currentSA.indicesDirectSummandSuperAlgebra.addOnTop(j);
        if (currentSA.indexMaxSSContainer == - 1) {
          currentSA.indexMaxSSContainer = j;
        }
        if (
          this->theSubalgebras.theValues[currentSA.indexMaxSSContainer].weylNonEmbedded->theDynkinType.getLieAlgebraDimension() <
          otherSA.weylNonEmbedded->theDynkinType.getLieAlgebraDimension()
        ) {
          currentSA.indexMaxSSContainer = j;
        }
      }
    }
    currentSA.computeCentralizerIsWellChosen();
    if (currentSA.centralizerRank > this->owner->getRank()) {
      global.fatal << "Subalgebra " << currentSA.toStringType() << " has rank "
      << currentSA.centralizerRank.toString()
      << " but the ambient Lie algebra isonly of rank "
      << this->owner->getRank() << ". " << global.fatal;
    }
  }
  theReport1.report("<hr>\nCentralizers computed, adjusting centralizers with respect to the Cartan subalgebra.");
  for (int i = 0; i < this->theSubalgebras.theValues.size; i ++) {
    if (!this->theSubalgebras.theValues[i].flagSystemSolved) {
      continue;
    }
    std::stringstream reportStream2;
    reportStream2 << "Adjusting the centralizer of subalgebra number " << i + 1
    << " out of " << this->theSubalgebras.theValues.size
    << ". The subalgebra is of type " << this->theSubalgebras.theValues[i].toStringTypeAndHs() << ". ";
    theReport2.report(reportStream2.str());
    this->theSubalgebras.theValues[i].adjustCentralizerAndRecompute(allowNonPolynomialSystemFailure);
  }
  theReport1.report("<hr>\nComputing pairing tables.");
  if (this->flagcomputeModuleDecompositionsition) {
    for (int i = 0; i < this->theSubalgebras.theValues.size; i ++) {
      if (this->theSubalgebras.theValues[i].flagCentralizerIsWellChosen && this->theSubalgebras.theValues[i].flagSystemSolved) {
        std::stringstream reportStream2;
        reportStream2 << "Computing primal module decomposition of subalgebra number " << i + 1
        << " out of " << this->theSubalgebras.theValues.size
        << ". The subalgebra is of type " << this->theSubalgebras.theValues[i].toStringTypeAndHs() << ". ";
        theReport2.report(reportStream2.str());
        this->theSubalgebras.theValues[i].computePrimalModuleDecomposition();
      }
    }
  }
}

bool DynkinType::operator>(const DynkinType& other) const {
  if (this->getRank() > other.getRank()) {
    return true;
  }
  if (other.getRank() > this->getRank()) {
    return false;
  }
  DynkinType difference;
  difference = *this;
  difference -= other;
  if (difference.isEqualToZero()) {
    return false;
  }
  DynkinSimpleType highestSimpleTypeDifference = difference[0];
  Rational maxComponentDifferenceMult = difference.coefficients[0];
  for (int i = 1; i < difference.size(); i ++) {
    if (difference[i] > highestSimpleTypeDifference) {
      maxComponentDifferenceMult = difference.coefficients[i];
      highestSimpleTypeDifference = difference[i];
    }
  }
  return maxComponentDifferenceMult > 0;
}

bool CandidateSemisimpleSubalgebra::operator>(const CandidateSemisimpleSubalgebra& other) const {
  return this->weylNonEmbedded->theDynkinType > other.weylNonEmbedded->theDynkinType;
}

void CandidateSemisimpleSubalgebra::computeCartanOfCentralizer() {
  Vectors<AlgebraicNumber> theHWsNonSorted;
  Vectors<AlgebraicNumber> theCartan;
  theHWsNonSorted.setSize(this->highestVectorsNonSorted.size);
  for (int i = 0; i < this->highestVectorsNonSorted.size; i ++) {
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& currentElt = this->highestVectorsNonSorted[i];
    currentElt.elementToVectorNegativeRootSpacesFirst(theHWsNonSorted[i]);
  }
  ElementSemisimpleLieAlgebra<AlgebraicNumber> tempElt;
  Vector<Rational> tempV;
  theCartan.setSize(this->getAmbientSemisimpleLieAlgebra().getRank());
  for (int i = 0; i < this->getAmbientSemisimpleLieAlgebra().getRank(); i ++) {
    tempV.makeEi(this->getAmbientSemisimpleLieAlgebra().getRank(), i);
    tempElt.makeCartanGenerator(tempV, this->getAmbientSemisimpleLieAlgebra());
    tempElt.elementToVectorNegativeRootSpacesFirst(theCartan[i]);
  }
  Vectors<AlgebraicNumber> outputCartanCentralizer;
  Vector<AlgebraicNumber> theCentralizerH;
  theHWsNonSorted.intersectTwoLinearSpaces(theHWsNonSorted, theCartan, outputCartanCentralizer);
  this->CartanOfCentralizer.setSize(outputCartanCentralizer.size);
  AlgebraicNumber theFirstNonZeroCoeff;
  for (int i = 0; i < this->CartanOfCentralizer.size; i ++) {
    tempElt.assignVectorNegRootSpacesCartanPosRootSpaces(outputCartanCentralizer[i], *this->owner->owner);
    theCentralizerH = tempElt.getCartanPart();
    theFirstNonZeroCoeff = theCentralizerH[theCentralizerH.getIndexFirstNonZeroCoordinate()];
    if (!theFirstNonZeroCoeff.isRational()) {
      theCentralizerH /= theFirstNonZeroCoeff;
    }
    this->CartanOfCentralizer[i] = theCentralizerH;
    this->CartanOfCentralizer[i].scaleNormalizeFirstNonZero();
  }
  ////////////////
  this->bilinearFormSimplePrimal = this->weylNonEmbedded->cartanSymmetric;
  Matrix<Rational> centralizerPart, matFundCoordsSimple, diagMat, diagMatrix2, bilinearFormInverted;
  // global.Comments << "<hr>Cartan of Centralizer: " << this->CartanOfCentralizer.toString() << "<br>Cartan symmetric: "
  // << this->owner->owner->theWeyl.cartanSymmetric.toString();
  this->CartanOfCentralizer.getGramMatrix(centralizerPart, &this->owner->owner->theWeyl.cartanSymmetric);
  this->bilinearFormSimplePrimal.directSumWith(centralizerPart);
  bilinearFormInverted = this->bilinearFormSimplePrimal;
  bilinearFormInverted.invert();
  diagMat.initialize(this->bilinearFormSimplePrimal.numberOfRows, this->bilinearFormSimplePrimal.numberOfColumns);
  diagMatrix2.initialize(this->bilinearFormSimplePrimal.numberOfRows, this->bilinearFormSimplePrimal.numberOfColumns);
  diagMat.makeZero();
  diagMatrix2.makeZero();
  for (int i = 0; i < this->bilinearFormSimplePrimal.numberOfRows; i ++) {
    if (i < this->theHs.size) {
      diagMat(i, i) = this->theSubalgebraNonEmbeddedDefaultScale->theWeyl.cartanSymmetric(i, i) / 2;
      diagMatrix2(i, i) = this->weylNonEmbedded->cartanSymmetric(i, i) / 2;
    } else {
      diagMat(i, i).assignNumeratorAndDenominator(1, 2);
      diagMatrix2(i, i) = 1;
    }
  }
  matFundCoordsSimple = bilinearFormInverted;
  matFundCoordsSimple *= diagMat;
  this->matMultiplyFundCoordsToGetSimple = bilinearFormInverted;
  this->matMultiplyFundCoordsToGetSimple *= diagMatrix2;
  this->bilinearFormFundPrimal = matFundCoordsSimple;
  this->bilinearFormFundPrimal.transpose();
  this->bilinearFormFundPrimal *= this->bilinearFormSimplePrimal;
  this->bilinearFormFundPrimal *= matFundCoordsSimple;
/*  this->inducedEmbeddingPrimalFundCoordsIntoSimpleAmbientCoords.initialize(this->getAmbientWeyl().getDimension(), this->getPrimalRank());
  for (int i = 0; i < this->getRank(); i ++)
    this->inducedEmbeddingPrimalFundCoordsIntoSimpleAmbientCoords.assignVectorToColumnKeepOtherColsIntactNoInit
    (i, this->theHs[i]);
  for (int i = this->getRank(); i < this->getPrimalRank(); i ++)
    this->inducedEmbeddingPrimalFundCoordsIntoSimpleAmbientCoords.assignVectorToColumnKeepOtherColsIntactNoInit
    (i, this->CartanOfCentralizer[i-this->getRank()]);
*/
}
