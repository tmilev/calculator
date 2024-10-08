#include "general_file_operations_encodings.h"
#include "math_extra_finite_groups_implementation.h"
#include "math_extra_semisimple_lie_algebras.h"
#include "math_extra_semisimple_lie_subalgebras.h"
#include "math_general_implementation.h"
#include "math_general_polynomial_computations_advanced_implementation.h" // IWYU pragma: keep: breaks web assembly build.
#include "math_general_polynomial_computations_basic_implementation.h" // IWYU pragma: keep: breaks the build.
#include "math_subsets_selections.h"
#include "math_vector_partition_functions.h"

template < >
List<DynkinSimpleType>::Comparator* FormatExpressions::getMonomialOrder<
  DynkinSimpleType
>() {
  return nullptr;
}

template <class Coefficient>
void SemisimpleLieAlgebra::generateLieSubalgebra(
  List<ElementSemisimpleLieAlgebra<Coefficient> >& inputOutputGenerators
) {
  STACK_TRACE("SemisimpleLieAlgebra::generateLieSubalgebra");
  ElementSemisimpleLieAlgebra<Coefficient> bracket;
  HashedList<ChevalleyGenerator> seedMonomials;
  ProgressReport report;
  List<ElementSemisimpleLieAlgebra<Coefficient> > inputLinearlyIndependent;
  for (int i = 0; i < inputOutputGenerators.size; i ++) {
    inputLinearlyIndependent.addOnTop(inputOutputGenerators[i]);
    if (
      bracket.getRankElementSpan(inputLinearlyIndependent, &seedMonomials) <
      inputLinearlyIndependent.size
    ) {
      inputLinearlyIndependent.removeLastObject();
    }
  }
  inputOutputGenerators = inputLinearlyIndependent;
  for (int i = 0; i < inputOutputGenerators.size; i ++) {
    for (int j = i + 1; j < inputOutputGenerators.size; j ++) {
      if (report.tickAndWantReport()) {
        std::stringstream reportStream;
        reportStream
        << "Generating Lie subalgebra of a semisimple Lie algebra. "
        << "I am taking the Lie bracket of elements "
        << i + 1
        << " and "
        << j + 1
        << " out of "
        << inputOutputGenerators.size;
        report.report(reportStream.str());
      }
      this->lieBracket(
        inputOutputGenerators[i], inputOutputGenerators[j], bracket
      );
      inputOutputGenerators.addOnTop(bracket);
      if (
        bracket.getRankElementSpan(inputOutputGenerators, &seedMonomials) <
        inputOutputGenerators.size
      ) {
        inputOutputGenerators.removeLastObject();
      }
    }
  }
}

std::string SemisimpleLieAlgebra::toStringLieAlgebraNameFullHTML() const {
  STACK_TRACE("SemisimpleLieAlgebra::toStringLieAlgebraNameFullHTML");
  std::stringstream out;
  if (this->weylGroup.dynkinType.hasExceptionalComponent()) {
    out
    << "<div class='mathcalculator'>"
    << this->weylGroup.dynkinType.toString()
    << "</div>";
    return out.str();
  }
  out
  << this->toStringLieAlgebraNameNonTechnicalHTML()
  << ", type \\("
  << this->weylGroup.dynkinType.toString()
  << "\\)";
  return out.str();
}

std::string SemisimpleLieAlgebra::toStringLieAlgebraName() const {
  return this->weylGroup.dynkinType.toString();
}

std::string SemisimpleLieAlgebra::toStringLieAlgebraNameNonTechnicalHTML()
const {
  STACK_TRACE("SemisimpleLieAlgebra::toStringLieAlgebraNameNonTechnicalHTML");
  std::stringstream out;
  const DynkinType& dynkinType = this->weylGroup.dynkinType;
  for (int indexType = 0; indexType < dynkinType.size(); indexType ++) {
    if (!(dynkinType.coefficients[indexType] > 0)) {
      global.fatal
      << "Simple constituents must appear with positive coefficient. "
      << global.fatal;
    }
    const DynkinSimpleType& currentSimpleType = dynkinType[indexType];
    for (
      int indexIsotypic = 0; indexIsotypic < dynkinType.coefficients[
        indexType
      ]; indexIsotypic ++
    ) {
      out << currentSimpleType.toStringNonTechnicalName(nullptr);
      if (indexIsotypic + 1 < dynkinType.coefficients[indexType]) {
        out << "\\oplus";
      }
    }
    if (indexType != dynkinType.size() - 1) {
      out << "\\oplus";
    }
  }
  return out.str();
}

bool SemisimpleLieAlgebra::isSimpleGenerator(int generatorIndex) {
  bool isNegativeGenerator = (
    generatorIndex < this->getNumberOfPositiveRoots()
  ) && (
    generatorIndex >= this->getNumberOfPositiveRoots() - this->getRank()
  );
  bool isPositiveGenerator = (
    generatorIndex >= this->getNumberOfPositiveRoots() + this->getRank()
  ) && (
    generatorIndex < this->getNumberOfPositiveRoots() + this->getRank() * 2
  );
  return isNegativeGenerator || isPositiveGenerator;
}

bool SemisimpleLieAlgebra::checkConsistency() const {
  if (this->flagDeallocated) {
    global.fatal << "Use after free of SemisimpleLieAlgebra. " << global.fatal;
  }
  this->weylGroup.checkConsistency();
  return true;
}

bool SemisimpleLieAlgebra::attemptFindingHEF(
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& inputOutputH,
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& inputOutputE,
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& inputOutputF,
  std::stringstream* logStream
) {
  STACK_TRACE("SemisimpleLieAlgebra::AttemptFindingHEF");
  List<Polynomial<AlgebraicNumber> > input;
  PolynomialSystem<AlgebraicNumber> system;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > mustBeZero;
  this->lieBracket(inputOutputH, inputOutputE, mustBeZero);
  mustBeZero -= inputOutputE * 2;
  for (int i = 0; i < mustBeZero.size(); i ++) {
    input.addOnTop(mustBeZero.coefficients[i]);
  }
  this->lieBracket(inputOutputH, inputOutputF, mustBeZero);
  mustBeZero += inputOutputF * 2;
  for (int i = 0; i < mustBeZero.size(); i ++) {
    input.addOnTop(mustBeZero.coefficients[i]);
  }
  this->lieBracket(inputOutputE, inputOutputF, mustBeZero);
  mustBeZero -= inputOutputH;
  for (int i = 0; i < mustBeZero.size(); i ++) {
    input.addOnTop(mustBeZero.coefficients[i]);
  }
  if (logStream != nullptr) {
    *logStream << "The system to solve: ";
    for (int i = 0; i < input.size; i ++) {
      *logStream << "<br>" << input[i].toString() << " = 0 ";
    }
  }
  system.groebner.maximumPolynomialDivisions = 2001;
  system.groebner.polynomialOrder.monomialOrder.setComparison(
    MonomialPolynomial::greaterThan_rightLargerWins
  );
  system.solveSerreLikeSystem(input);
  if (!system.flagSystemSolvedOverBaseField) {
    if (logStream != nullptr) {
      if (system.flagSystemProvenToHaveNoSolution) {
        *logStream << "<br><b>System proven to have no solution. </b>";
      }
      if (system.flagSystemProvenToHaveSolution) {
        *logStream
        << "<br><b>System proven to have solution, "
        << "but I failed to find it.</b> "
        << "Either the solution lies outside of the base field, "
        << "or my heuristics are not good enough.";
      }
    }
    return false;
  }
  if (logStream != nullptr) {
    *logStream
    << "Solved successfully! One solution: "
    << system.toStringSerreLikeSolution();
  }
  PolynomialSubstitution<AlgebraicNumber> solutionSubstitution;
  system.getSubstitutionFromPartialSolutionSerreLikeSystem(
    solutionSubstitution
  );
  inputOutputF.substituteInCoefficients(solutionSubstitution);
  inputOutputH.substituteInCoefficients(solutionSubstitution);
  inputOutputE.substituteInCoefficients(solutionSubstitution);
  if (logStream != nullptr) {
    *logStream
    << "<br>H = "
    << inputOutputH.toString()
    << "<br>E = "
    << inputOutputE.toString()
    << "<br>F = "
    << inputOutputF.toString();
  }
  return true;
}

bool SemisimpleLieAlgebra::attemptExtendingEtoHEFwithHinCartan(
  ElementSemisimpleLieAlgebra<AlgebraicNumber>& elementE,
  ElementSemisimpleLieAlgebra<AlgebraicNumber>& outputH,
  ElementSemisimpleLieAlgebra<AlgebraicNumber>& outputF,
  std::stringstream* logStream
) {
  STACK_TRACE("SemisimpleLieAlgebra::attemptExtendingEtoHEFwithHinCartan");
  Matrix<AlgebraicNumber> matrix;
  this->getAdjoint(matrix, elementE);
  MatrixTensor<AlgebraicNumber> matrixTensor;
  MatrixTensor<AlgebraicNumber> identity;
  matrixTensor = matrix;
  identity.makeIdentity(matrix.numberOfRows);
  MathRoutines::raiseToPower(
    matrixTensor, this->getNumberOfPositiveRoots(), identity
  );
  if (!matrixTensor.isEqualToZero()) {
    if (logStream != nullptr) {
      *logStream
      << "The input E element "
      << elementE.toString()
      << " is not nilpotent. The matrix tensor is: "
      << matrixTensor.toString();
    }
    return false;
  }
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > unknownH;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > unknownF;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > knownE;
  knownE = elementE;
  this->getGenericElementCartan(unknownH, 0);
  this->getGenericElementNegativeBorelNilradical(
    unknownF, this->getRank()
  );
  bool success =
  this->attemptFindingHEF(unknownH, knownE, unknownF, logStream);
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
    global.fatal
    << "Non-initilized (no owner) subalgebra of semisimple Lie algebra."
    << global.fatal;
  }
  return true;
}

std::string SubalgebraSemisimpleLieAlgebra::toString(
  FormatExpressions* format
) {
  STACK_TRACE("SubalgebraSemisimpleLieAlgebra::toString");
  (void) format;
  // avoid unused parameter warning in a portable way
  if (this->owner == nullptr) {
    return "A non-initialized subalgebra of a semisimple Lie algebra; ";
  }
  std::stringstream out;
  out
  << "A subalgebra of dimension "
  << this->basis.size
  << " lying in "
  << this->owner->weylGroup.dynkinType.toString()
  << ".<br>The subalgebra has basis: "
  << this->basis.toStringCommaDelimited();
  return out.str();
}

void SubalgebraSemisimpleLieAlgebra::computeBasis() {
  STACK_TRACE("SubalgebraSemisimpleLieAlgebra::computeBasis");
  this->checkInitialization();
  this->basis = this->generators;
  this->owner->generateLieSubalgebra(this->basis);
}

bool SubalgebraSemisimpleLieAlgebra::findCartanSubalgebraCandidate(
  const List<MatrixTensor<AlgebraicNumber> >& adjointOperators,
  ElementSemisimpleLieAlgebra<AlgebraicNumber>& outputCandidate,
  const List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& currentCentralizer
) {
  STACK_TRACE("SubalgebraSemisimpleLieAlgebra::findCartanSubalgebraCandidate");
  MatrixTensor<AlgebraicNumber> candidateMatrix;
  ElementSemisimpleLieAlgebra<AlgebraicNumber> candidate;
  for (int i = 0; i < adjointOperators.size; i ++) {
    if (adjointOperators[i].isNilpotent()) {
      continue;
    }
    candidate = currentCentralizer[i];
    if (candidate.linearSpanContains(this->cartanSubalgebra, candidate)) {
      continue;
    }
    outputCandidate = candidate;
    return true;
  }
  for (int i = 0; i < adjointOperators.size; i ++) {
    for (int j = i + 1; j < adjointOperators.size; j ++) {
      candidateMatrix = adjointOperators[i] + adjointOperators[j];
      if (candidateMatrix.isNilpotent()) {
        continue;
      }
      candidate = currentCentralizer[i] + currentCentralizer[j];
      if (
        candidate.linearSpanContains(this->cartanSubalgebra, candidate)
      ) {
        continue;
      }
      outputCandidate = candidate;
      return true;
    }
  }
  return false;
}

void SubalgebraSemisimpleLieAlgebra::computeCartanSubalgebra() {
  STACK_TRACE("SubalgebraSemisimpleLieAlgebra::computeCartanSubalgebra");
  this->checkInitialization();
  List<MatrixTensor<AlgebraicNumber> > adjointOperators;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > currentCentralizer;
  this->cartanSubalgebra.setSize(0);
  currentCentralizer = this->basis;
  ElementSemisimpleLieAlgebra<AlgebraicNumber> candidateElement;
  ProgressReport report0;
  ProgressReport report1;
  std::stringstream reportStream0;
  reportStream0
  << "Computing Cartan subalgebra of a subalgebra of "
  << this->owner->weylGroup.dynkinType.toString()
  << " with basis "
  << this->basis.toStringCommaDelimited();
  report0.report(reportStream0.str());
  while (currentCentralizer.size > this->cartanSubalgebra.size) {
    std::stringstream reportStream1;
    reportStream1
    << "Currently, the Cartan subalgebra basis candidates are: "
    << this->cartanSubalgebra.toStringCommaDelimited()
    << "; remaining centralizer: "
    << currentCentralizer.toStringCommaDelimited();
    report1.report(reportStream1.str());
    if (this->cartanSubalgebra.size > this->owner->getRank()) {
      // We must have made a programming error.
      global.fatal
      << "Cartan subalgebra's rank exceeded the rank of the base algebra. "
      << this->cartanSubalgebra.toStringCommaDelimited()
      << global.fatal;
    }
    adjointOperators.setSize(currentCentralizer.size);
    for (int i = 0; i < currentCentralizer.size; i ++) {
      this->owner->getAdjoint(
        adjointOperators[i], currentCentralizer[i]
      );
    }
    if (
      !this->findCartanSubalgebraCandidate(
        adjointOperators, candidateElement, currentCentralizer
      )
    ) {
      global.fatal
      << "This shouldn't happen: could not found "
      << "a new non-nilpotent element among: "
      << currentCentralizer.toStringCommaDelimited()
      << ". "
      << global.fatal;
    }
    global.comments
    << "<br>Found candidate: "
    << candidateElement.toString()
    << "<br>";
    this->cartanSubalgebra.addOnTop(candidateElement);
    this->owner->getCommonCentralizer(
      this->cartanSubalgebra, currentCentralizer
    );
    candidateElement.intersectVectorSpaces(
      currentCentralizer, this->basis, currentCentralizer, nullptr
    );
  }
}

void WeylGroupData::operator+=(const WeylGroupData& other) {
  DynkinType currentType = this->dynkinType;
  currentType += other.dynkinType;
  this->makeFromDynkinType(currentType);
}

int SemisimpleSubalgebras::getIndexFullSubalgebra() const {
  STACK_TRACE("SemisimpleSubalgebras::getIndexFullSubalgebra");
  for (int i = 0; i < this->subalgebras.values.size; i ++) {
    if (
      this->owner->weylGroup.dynkinType ==
      this->subalgebras.values[i].weylNonEmbedded->dynkinType
    ) {
      return i;
    }
  }
  return - 1;
}

std::string SemisimpleSubalgebras::toStringSubalgebraNumberWithAmbientLink(
  int actualIndexSubalgebra, FormatExpressions* format
) const {
  std::stringstream out;
  CandidateSemisimpleSubalgebra& candidate =
  this->subalgebras.values[actualIndexSubalgebra];
  out
  << "Subalgebra "
  << "\\("
  << candidate.weylNonEmbedded->dynkinType.toString()
  << "\\)"
  << " &#8618; "
  << "<a href=\"./"
  << this->displayNameMainFile1NoPath
  << "\">"
  << "\\("
  << this->owner->weylGroup.dynkinType.toString(format)
  << "\\)"
  << "</a>";
  int displayIndex = this->getDisplayIndexFromActual(actualIndexSubalgebra);
  out
  << "<br>"
  << displayIndex
  << " out of "
  << this->subalgebras.size()
  << "\n";
  return out.str();
}

int SemisimpleSubalgebras::getDisplayIndexFromActual(int actualindexSubalgebra)
const {
  int result = 0;
  for (int i = 0; i <= actualindexSubalgebra; i ++) {
    if (!this->subalgebras.values[i].flagSystemProvedToHaveNoSolution) {
      result ++;
    }
  }
  return result;
}

std::string SemisimpleSubalgebras::getRelativePhysicalFileNameSubalgebra(
  int actualIndexSubalgebra
) const {
  std::stringstream out;
  out << this->owner->fileNames.virtualFolderName();
  out << this->fileNamePrefix;
  out
  << FileOperations::cleanUpForFileNameUse(
    this->owner->weylGroup.dynkinType.toString()
  )
  << "_subalgebra_"
  << this->getDisplayIndexFromActual(actualIndexSubalgebra)
  << ".html";
  return out.str();
}

std::string SemisimpleSubalgebras::getRelativePhysicalFileNameFKFTNilradicals(
  int actualindexSubalgebra
) const {
  std::stringstream out;
  out << this->owner->fileNames.virtualFolderName();
  out
  << FileOperations::cleanUpForFileNameUse(
    this->owner->weylGroup.dynkinType.toString()
  )
  << "_subalgebra_"
  << this->getDisplayIndexFromActual(actualindexSubalgebra)
  << "_FKFTnilradicals.html";
  return out.str();
}

std::string SemisimpleSubalgebras::getDisplayFileNameSubalgebraAbsolute(
  int actualindexSubalgebra, FormatExpressions* format
) const {
  std::stringstream out;
  (void) format;
  // avoid unused parameter warning in a portable way
  out << this->owner->fileNames.virtualFolderName();
  out
  << FileOperations::cleanUpForFileNameUse(
    this->owner->weylGroup.dynkinType.toString()
  )
  << "_subalgebra_"
  << this->getDisplayIndexFromActual(actualindexSubalgebra)
  << ".html";
  return out.str();
}

std::string SemisimpleSubalgebras::getDisplayFileNameSubalgebraRelative(
  int actualindexSubalgebra, FormatExpressions* format
) const {
  std::stringstream out;
  (void) format;
  // avoid unused parameter warning in a portable way
  // out << "./";
  out
  << this->fileNamePrefix
  << FileOperations::cleanUpForFileNameUse(
    this->owner->weylGroup.dynkinType.toString()
  )
  << "_subalgebra_"
  << this->getDisplayIndexFromActual(actualindexSubalgebra)
  << ".html";
  return out.str();
}

std::string SemisimpleSubalgebras::getDisplayFileNameFKFTNilradicals(
  int actualindexSubalgebra, FormatExpressions* format
) const {
  std::stringstream out;
  (void) format;
  // avoid unused parameter warning in a portable way
  // out << this->owner->toStringVirtualFolderName();
  out
  << this->fileNamePrefix
  << FileOperations::cleanUpForFileNameUse(
    this->owner->weylGroup.dynkinType.toString()
  )
  << "_subalgebra_"
  << this->getDisplayIndexFromActual(actualindexSubalgebra)
  << "_FKFTnilradicals.html";
  return out.str();
}

std::string DynkinType::toString(FormatExpressions* format) const {
  return
  this->::LinearCombination<DynkinSimpleType, Rational>::toString(format);
}

void SemisimpleSubalgebras::checkFileWritePermissions() {
  STACK_TRACE("SemisimpleSubalgebras::checkFileWritePermissions");
  this->checkConsistency();
  this->computeFolderNames(this->currentFormat);
  std::fstream testFile;
  std::string testFileNameRelative =
  this->owner->fileNames.virtualFolderName() +
  "testFileWritePermissionsSSsas.txt";
  std::string testFileFolderPhysical;
  FileOperations::getPhysicalFileNameFromVirtual(
    this->owner->fileNames.virtualFolderName(),
    testFileFolderPhysical,
    false,
    false,
    nullptr
  );
  if (
    !FileOperations::openFileCreateIfNotPresentVirtualCreateFoldersIfNeeded(
      testFile, testFileNameRelative, false, true, false
    )
  ) {
    global.fatal
    << "<br>This may or may not be a programming error. "
    << "I requested to create file "
    << this->virtualNameMainFile1
    << " which resolves to "
    << testFileFolderPhysical
    << " for output. However, the file failed to create. "
    << "Possible explanations: "
    << "1. Programming error. "
    << "2. The calculator has no write permission to the "
    << "folder in which the file is located. "
    << "3. The folder does not exist for some "
    << "reason lying outside of the calculator. "
    << global.fatal;
  }
  FileOperations::openFileCreateIfNotPresentVirtual(
    testFile, testFileNameRelative, false, true, false
  );
  testFile << "Write permissions test file.";
}

void SemisimpleSubalgebras::writeReportToFiles() {
  this->millisecondsComputationEnd = global.getElapsedMilliseconds();
  this->numberOfAdditions =
  Rational::totalSmallAdditions + Rational::totalLargeAdditions;
  this->numberOfMultiplications =
  Rational::totalSmallMultiplications + Rational::totalLargeMultiplications;
  this->currentFormat.flagUseHTML = true;
  this->currentFormat.flagUseLatex = true;
  std::stringstream out;
  if (
    !FileOperations::writeFileVirtual(
      this->virtualNameMainFile1, this->toStringHTML(), &out
    )
  ) {
    global.comments << out.str();
  }
}

std::string SemisimpleSubalgebras::toStringHTML() {
  std::stringstream out;
  out
  << "<html><title>Semisimple subalgebras of "
  << "the semisimple Lie algebras: the subalgebras of "
  << this->owner->weylGroup.dynkinType.toString()
  << "</title>";
  out << SemisimpleLieAlgebra::toHTMLCalculatorHeadElements();
  out << SemisimpleLieAlgebra::toHTMLCalculatorBodyOnload();
  out << this->toString(&this->currentFormat, true);
  out << "</body></html>";
  return out.str();
}

void SemisimpleSubalgebras::computeFolderNames(FormatExpressions& inputFormat)
{
  STACK_TRACE("SemisimpleSubalgebras::computeFolderNames");
  (void) inputFormat;
  // avoid unused parameter warning in a portable way
  this->checkConsistency();
  if (this->owner == nullptr) {
    global.fatal
    << "To compute semisimple subalgebra folders, "
    << "you need to specify the ambient Lie algebra. "
    << global.fatal;
  }
  this->displayNameMainFile1NoPath =
  this->fileNamePrefix +
  this->owner->fileNames.fileNameNoPathSemisimpleSubalgebras();
  this->displayNameMainFile1WithPath =
  this->owner->fileNames.displayFolderName("../../output/") +
  this->displayNameMainFile1NoPath;
  this->virtualNameMainFile1 =
  this->owner->fileNames.virtualFolderName() +
  this->displayNameMainFile1NoPath;
}

std::string SemisimpleSubalgebras::toStringSemisimpleSubalgebraSummaryHTML(
  FormatExpressions* format
) const {
  STACK_TRACE(
    "SemisimpleSubalgebras::toStringSemisimpleSubalgebraSummaryHTML"
  );
  (void) format;
  // avoid unused parameter warning in a portable way
  if (!this->flagComputeNilradicals) {
    return "";
  }
  std::stringstream out;
  int totalIsotypicallyCompleteNilradicals = 0;
  int numberOfFailingConeConditions = 0;
  int numberOfNoLinfRelationsFound = 0;
  int numberOfNonCentralizerConditionWithConeCondition = 0;
  int totalBadParabolics = 0;
  for (int i = 0; i < this->subalgebras.values.size; i ++) {
    totalIsotypicallyCompleteNilradicals +=
    this->subalgebras.values[i].fernandoKacNilradicalCandidates.size;
    numberOfFailingConeConditions +=
    this->subalgebras.values[i].numberOfConeIntersections;
    numberOfNoLinfRelationsFound +=
    this->subalgebras.values[i].numberOfCasesNoLInfiniteRelationFound;
    numberOfNonCentralizerConditionWithConeCondition +=
    this->subalgebras.values[i].
    numberOfCentralizerConditionFailsConeConditionHolds;
    totalBadParabolics += this->subalgebras.values[i].numberOfBadParabolics;
  }
  out
  << "<br>There are "
  << totalIsotypicallyCompleteNilradicals
  << " possible isotypic nilradical extensions "
  << "of the primal subalgebras. Of them "
  << numberOfFailingConeConditions
  << " have intersecting cones. Of the remaining "
  << totalIsotypicallyCompleteNilradicals - numberOfFailingConeConditions
  << " nilradical extensions with non-intersecting cones, "
  << totalIsotypicallyCompleteNilradicals -
  numberOfFailingConeConditions -
  numberOfNonCentralizerConditionWithConeCondition
  << " satisfy(ies) the centralizer condition and "
  << numberOfNonCentralizerConditionWithConeCondition
  << " fail(s) the centralizer condition.";
  if (totalBadParabolics > 0) {
    out
    << "<br><span style='color:red'>Of the subalgebras "
    << "satisfying the centralizer condition there are "
    << totalBadParabolics
    << " pabolic subalgebra(s) that do not extend to "
    << "parabolic subalgebra(s) of the ambient "
    << "Lie algebra with Levi types A and C. "
    << "For these subalgebras the PSZ construction "
    << "is not proven to hold. </span>";
  } else {
    out
    << "<br><span style='color:blue'> In each of "
    << totalIsotypicallyCompleteNilradicals -
    numberOfFailingConeConditions -
    numberOfNonCentralizerConditionWithConeCondition
    << " case(s) when the centralizer condition holds, "
    << "the parabolic subalgebra in the centralizer "
    << "with Levi types A and C extends "
    << "to parabolic subalgebra of the ambient "
    << "Lie algebra whose Levi types are A and C only. </span>";
  }
  if (numberOfFailingConeConditions > 0) {
    if (numberOfNoLinfRelationsFound > 0) {
      out
      << "<br><span style='color:red'>In "
      << numberOfNoLinfRelationsFound
      << " cases no L-infinite relation was found. </span>";
    } else {
      out
      << "<br><span style='color:blue'> In each of "
      << numberOfFailingConeConditions
      << " case(s) of intersecting cones, an "
      << "L-infinite relation was found. </span>";
    }
  }
  return out.str();
}

std::string SemisimpleSubalgebras::toStringSemisimpleSubalgebrasSummaryLaTeX(
  FormatExpressions* format
) const {
  STACK_TRACE(
    "SemisimpleSubalgebras::toStringSemisimpleSubalgebrasSummaryLaTeX"
  );
  if (!this->flagComputeNilradicals) {
    return "";
  }
  std::stringstream out;
  int numberOfIsotypicallyCompleteNilradicals = 0;
  int numberOfFailingConeCondition = 0;
  int numberNoLinfRelationsFound = 0;
  int totalBadParabolics = 0;
  for (int i = 0; i < this->subalgebras.values.size; i ++) {
    numberOfIsotypicallyCompleteNilradicals +=
    this->subalgebras.values[i].fernandoKacNilradicalCandidates.size;
    numberOfFailingConeCondition +=
    this->subalgebras.values[i].numberOfConeIntersections;
    numberNoLinfRelationsFound +=
    this->subalgebras.values[i].numberOfCasesNoLInfiniteRelationFound;
    totalBadParabolics += this->subalgebras.values[i].numberOfBadParabolics;
  }
  if (totalBadParabolics > 0) {
    out
    << "<br><span style='color:red'>There are "
    << totalBadParabolics
    << " bad parabolic subalgebras!</span><br>";
  }
  out << "\n<br>\n\\begin{longtable}{ccp{3cm}p{3cm}cc}";
  out
  << "\\caption{Semisimple subalgebras in type $"
  << this->owner->weylGroup.dynkinType.toString(format)
  << "$ \\label{tableSSSubalgerbas"
  << this->owner->weylGroup.dynkinType.toString(format)
  << "}. ";
  out
  << "Number of isotypically complete nilradicals: "
  << numberOfIsotypicallyCompleteNilradicals
  << ", of them "
  << numberOfFailingConeCondition
  << " fail the cone condition.";
  if (numberNoLinfRelationsFound == 0) {
    out
    << "<br>In all "
    << numberOfFailingConeCondition
    << " cases, an $\\mathfrak{l}$-infinite relation was found. ";
  } else {
    out
    << "<br>In "
    << numberNoLinfRelationsFound
    << " cases, no L-infinite relation was found. <br>";
  }
  out << "}\\\\\n<br>\n";
  out
  << "Type $\\mathfrak s$ & Centralizer &Decomp. $\\mathfrak g$ over "
  << "$\\mathfrak s$ &Decomp. $\\mathfrak g$ "
  << "over $\\mathfrak s\\oplus \\mathfrak h_c$"
  << "&\\#$\\mathfrak n_\\mathfrak l$& \\# cone failures\\\\\\hline\n<br>\n";
  DynkinType typeCentralizer;
  FormatExpressions characterFormat;
  for (int i = 0; i < this->subalgebras.values.size; i ++) {
    const CandidateSemisimpleSubalgebra& currentSubalgebra =
    this->subalgebras.values[i];
    if (currentSubalgebra.flagSystemProvedToHaveNoSolution) {
      continue;
    }
    out
    << "$"
    << currentSubalgebra.weylNonEmbedded->dynkinType.toString(format)
    << "$";
    if (!currentSubalgebra.flagCentralizerIsWellChosen) {
      continue;
    }
    typeCentralizer.makeZero();
    if (currentSubalgebra.indexMaximalSemisimpleContainer != - 1) {
      typeCentralizer =
      this->subalgebras.values[
        currentSubalgebra.indexMaximalSemisimpleContainer
      ].weylNonEmbedded->dynkinType -
      currentSubalgebra.weylNonEmbedded->dynkinType;
    }
    out << "& $ ";
    if (!typeCentralizer.isEqualToZero()) {
      out << typeCentralizer.toString();
      if (currentSubalgebra.centralizerRank != typeCentralizer.getRank()) {
        out << "\\oplus";
      }
    }
    if (currentSubalgebra.centralizerRank != typeCentralizer.getRank()) {
      out
      << "\\mathfrak h_{"
      << (currentSubalgebra.centralizerRank - typeCentralizer.getRank()).
      toString()
      << "}";
    }
    out << "$";
    if (!currentSubalgebra.characterFormat.isZeroPointer()) {
      characterFormat = currentSubalgebra.characterFormat.getElementConst();
    }
    out
    << "&$"
    << currentSubalgebra.characterNonPrimalFundamentalCoordinates.toString(
      &characterFormat
    )
    << "$ ";
    out
    << "&$"
    << currentSubalgebra.primalCharacter.toString(&characterFormat)
    << "$ ";
    out
    << "& "
    << currentSubalgebra.fernandoKacNilradicalCandidates.size
    << "&"
    << currentSubalgebra.numberOfConeIntersections;
    out << "\\\\ \\hline \n<br>\n";
  }
  out << "\\end{longtable}\n<br>\n";
  out << "\\end{document}";
  return out.str();
}

std::string SemisimpleSubalgebras::toStringShort() const {
  if (this->owner == nullptr) {
    return "(Uninitialized subalgebras)";
  }
  std::stringstream out;
  out
  << "Semisimple subalgebras of Lie algebra of type: "
  << this->owner->toStringLieAlgebraName();
  return out.str();
}

std::string SemisimpleSubalgebras::toString(
  FormatExpressions* format, bool writeToHardDisk
) {
  STACK_TRACE("SemisimpleSubalgebras::toString");
  std::stringstream out;
  int candidatesRealized = 0;
  int candidatesNotRealizedNotProvenImpossible = 0;
  int candidatesProvenImpossible = 0;
  for (int i = 0; i < this->subalgebras.values.size; i ++) {
    CandidateSemisimpleSubalgebra& currentSubalgebra =
    this->subalgebras.values[i];
    if (currentSubalgebra.flagSystemProvedToHaveNoSolution) {
      candidatesProvenImpossible ++;
    }
    if (currentSubalgebra.flagSystemSolved) {
      candidatesRealized ++;
    }
  }
  out << "<h1>Lie algebra " << this->owner->toStringLieAlgebraNameFullHTML();
  if (!this->flagRealForms) {
    out << "<br>Semisimple complex Lie subalgebras";
  } else {
    out << "<br>sl(2)-subalgebras and W-conjecture";
  }
  out << "</h1>";
  out
  << this->owner->toStringHTMLMenuStructureSummary(
    "", true, true, true, false
  );
  if (this->flagRealForms) {
    if (this->wConjecture.wConjectureHolds(*this)) {
      out << "<b style='color:green'>W-conjecture holds.</b><br>";
    } else {
      out << "<b style='color:red'>W-conjecture does not hold.</b><br>";
    }
    out
    << "There are "
    << this->wConjecture.numberOfSmallSlTwos
    << " small sl(2)'s out of "
    << this->subalgebras.size()
    << "<br>";
  }
  candidatesNotRealizedNotProvenImpossible =
  this->subalgebras.size() - candidatesRealized - candidatesProvenImpossible;
  if (!writeToHardDisk) {
    out << candidatesRealized << " subalgebras realized.";
    out
    << "<br>Up to linear equivalence, there are "
    << this->subalgebras.size()
    << " = "
    << candidatesRealized
    << " realized + "
    << candidatesProvenImpossible
    << " proven impossible + "
    << candidatesNotRealizedNotProvenImpossible
    << " neither realized nor proven impossible. \n<hr>\n ";
  } else {
    out
    << "Up to linear equivalence, there are total "
    << candidatesRealized
    << " semisimple subalgebras (including the full subalgebra)";
    if (candidatesNotRealizedNotProvenImpossible != 0) {
      out
      << " and "
      << candidatesNotRealizedNotProvenImpossible
      << " semisimple subalgebra candidate(s) "
      << "which were not realized (but not proven impossible)";
    }
    out << ". ";
  }
  out
  << "The subalgebras are ordered by rank, "
  << "Dynkin indices of simple constituents "
  << "and dimensions of simple constituents.<br>"
  << "The upper index indicates the Dynkin index, "
  << "the lower index indicates the rank of the subalgebra.<br>";
  out << this->toStringTableSubalgebraLinksTable(format) << "<hr>";
  if (this->flagRealForms) {
    out << this->wConjecture.toStringRealForm(*this) << "<br>";
  }
  if (this->comments != "") {
    out
    << "<a href='"
    << this->fileNameToLogComments
    << "'>Generation comments.</a>";
  }
  bool showTime = format ==
  nullptr ? true : format->flagIncludeMutableInformation;
  if (showTime) {
    if (
      this->millisecondsComputationStart > 0 &&
      this->millisecondsComputationEnd > 0
    ) {
      out
      << "<br>Computation milliseconds: "
      << this->millisecondsComputationEnd - this->millisecondsComputationStart
      << ".";
    }
  }
  if (this->numberOfAdditions > 0) {
    out
    << "<br>"
    << this->numberOfAdditions + this->numberOfMultiplications
    << " total arithmetic operations performed = "
    << this->numberOfAdditions
    << " additions and "
    << this->numberOfMultiplications
    << " multiplications. ";
  }
  out << this->toStringPart2(format, writeToHardDisk);
  return out.str();
}

std::string SemisimpleSubalgebras::toStringSubalgebrasNoHDWrite(
  FormatExpressions* format, bool generateLinks
) {
  std::stringstream out;
  for (int i = 0; i < this->subalgebras.values.size; i ++) {
    if (!this->subalgebras.values[i].flagSystemProvedToHaveNoSolution) {
      out
      << this->toStringSubalgebraNumberWithAmbientLink(i, format)
      << "\n<br>\n";
    }
    out
    << this->subalgebras.values[i].toString(format, generateLinks)
    << "\n<hr>\n ";
  }
  return out.str();
}

void SemisimpleSubalgebras::writeSubalgebraToFile(
  FormatExpressions* format, int subalgebraIndex
) {
  CandidateSemisimpleSubalgebra& currentSubalgebra =
  this->subalgebras.values[subalgebraIndex];
  if (currentSubalgebra.flagSystemProvedToHaveNoSolution) {
    return;
  }
  std::stringstream out;
  std::string fileName =
  this->getRelativePhysicalFileNameSubalgebra(subalgebraIndex);
  out
  << "<html>"
  << SemisimpleLieAlgebra::toHTMLCalculatorHeadElements()
  << SemisimpleLieAlgebra::toHTMLCalculatorBodyOnload();
  out
  << this->toStringSubalgebraNumberWithAmbientLink(subalgebraIndex, format);
  out << "<div class='divSubalgebraInformation'>";
  out
  << "Computations done by the "
  << HtmlRoutines::getHtmlLinkToGithubRepository("calculator project");
  out << ".</div>";
  out << "<br>" << currentSubalgebra.toString(format, true);
  std::stringstream commentsOnError;
  if (this->flagComputeNilradicals) {
    std::stringstream outputFileFiniteTypeNilradicals;
    std::string fileName =
    this->getRelativePhysicalFileNameFKFTNilradicals(subalgebraIndex);
    outputFileFiniteTypeNilradicals
    << "<html>"
    << SemisimpleLieAlgebra::toHTMLCalculatorHeadElements()
    << "<body>"
    << this->toStringAlgebraLink(subalgebraIndex, format, true)
    << currentSubalgebra.toStringNilradicals(format)
    << "\n</body></html>";
    if (
      !FileOperations::writeFileVirtual(
        fileName,
        outputFileFiniteTypeNilradicals.str(),
        &commentsOnError
      )
    ) {
      std::string physicalFileName;
      global.fatal
      << "Failed to write your file: "
      << fileName
      << ". "
      << commentsOnError.str()
      << global.fatal;
    }
  }
  out << "\n</body></html>\n ";
  if (
    !FileOperations::writeFileVirtual(
      fileName, out.str(), &commentsOnError
    )
  ) {
    global.fatal
    << "<br>Failed to store your subalgebra file: "
    << fileName
    << ". "
    << commentsOnError.str()
    << global.fatal;
  }
}

std::string SemisimpleSubalgebras::toStringSubalgebrasWithHDWrite(
  FormatExpressions* format
) {
  std::stringstream out;
  if (format != nullptr) {
    format->flagCandidateSubalgebraShortReportOnly = true;
  }
  out << this->toStringSubalgebrasNoHDWrite(format, true);
  FormatExpressions formatCopy;
  if (format != nullptr) {
    formatCopy = *format;
  }
  formatCopy.flagCandidateSubalgebraShortReportOnly = false;
  for (int i = 0; i < this->subalgebras.values.size; i ++) {
    this->writeSubalgebraToFile(&formatCopy, i);
  }
  return out.str();
}

std::string SemisimpleSubalgebras::toStringTableSubalgebraLinksTable(
  FormatExpressions* format
) {
  (void) format;
  std::stringstream out;
  int numberOfColumns = 6;
  out << "<table class='tableSemisimpleLieAlgebrasList'>";
  bool rowStarted = false;
  int displayedInCurrentRow = 0;
  for (int i = 0; i < this->subalgebras.size(); i ++) {
    CandidateSemisimpleSubalgebra& subalgebra = this->subalgebras.values[i];
    if (subalgebra.flagSystemProvedToHaveNoSolution) {
      continue;
    }
    displayedInCurrentRow ++;
    if (!rowStarted) {
      out << "<tr>";
      rowStarted = true;
    }
    int displayIndex = this->getDisplayIndexFromActual(i);
    out
    << "<td>"
    << displayIndex
    << ". "
    << "<a href='#semisimple_subalgebra_"
    << displayIndex
    << "'>\\("
    << subalgebra.weylNonEmbedded->dynkinType.toString()
    << "\\)</a></td>";
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

std::string SemisimpleSubalgebras::toStringPart2(
  FormatExpressions* format, bool writeToHardDisk
) {
  STACK_TRACE("SemisimpleSubalgebras::toStringPart2");
  std::stringstream out;
  out << "<hr>";
  out << "The base field over which the subalgebras were realized is: ";
  if (this->ownerField == nullptr) {
    out << HtmlRoutines::getMathNoDisplay("\\mathbb Q");
  } else {
    out << this->ownerField->toString();
  }
  out << "<hr> ";
  int numberOfRegularSubalgebras = 0;
  int numberOfSl2s = 0;
  for (int i = 0; i < this->subalgebras.size(); i ++) {
    if (this->subalgebras.values[i].isRegularSubalgebra()) {
      numberOfRegularSubalgebras ++;
    }
    if (
      this->subalgebras.values[i].weylNonEmbedded->dynkinType.isTypeAOne()
    ) {
      numberOfSl2s ++;
    }
  }
  out
  << "Number of root subalgebras other than "
  << "the Cartan and full subalgebra: "
  << numberOfRegularSubalgebras - 1;
  out << "<br>Number of sl(2)'s: " << numberOfSl2s << "<hr>";
  std::string summaryString =
  this->toStringSemisimpleSubalgebraSummaryHTML(format);
  if (summaryString != "") {
    out << "Summary: " << summaryString << "<hr>";
    if (this->flagProduceLaTeXTables) {
      out
      << "Summary in LaTeX\n<br><br>"
      << this->toStringSemisimpleSubalgebrasSummaryLaTeX(format)
      << "\n<br><br><hr>";
    }
  }
  if (!writeToHardDisk) {
    out << this->toStringSubalgebrasNoHDWrite(format, false);
  } else {
    out << this->toStringSubalgebrasWithHDWrite(format);
  }
  out << this->toStringPart3(format, writeToHardDisk);
  return out.str();
}

std::string SemisimpleSubalgebras::toStringSl2s(FormatExpressions* format) {
  if (this->slTwoSubalgebras.size == 0) {
    return "";
  }
  std::stringstream out;
  int totalComputedOrbits = 0;
  for (int i = 0; i < this->orbits.size; i ++) {
    if (this->orbits[i].flagOrbitIsBuffered) {
      totalComputedOrbits ++;
    }
  }
  out
  << "<hr>Of the "
  << this->orbits.size
  << " h element conjugacy classes "
  << totalComputedOrbits
  << " had their Weyl group automorphism orbits computed and buffered. "
  << "The h elements and their computed orbit sizes follow. ";
  out << "<table><tr><td>h element</td><td>orbit size</td></tr>";
  for (int i = 0; i < this->orbits.size; i ++) {
    out
    << "<tr><td>"
    << this->slTwoSubalgebras[i].hElement.getCartanPart().toString()
    << "</td>";
    if (this->orbits[i].orbitSize != - 1) {
      out << "<td>" << this->orbits[i].orbitSize;
      if (this->orbits[i].flagOrbitIsBuffered == 0) {
        out << "<b>(orbit enumerated but not stored)</b>";
      }
      out << "</td>";
    } else {
      out << "<td>size not computed</td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  out << this->slTwoSubalgebras.toString(format);
  return out.str();
}

std::string SemisimpleSubalgebras::toStringPart3(
  FormatExpressions* format, bool writeToHardDisk
) {
  std::stringstream out;
  if (!writeToHardDisk) {
    out << this->toStringSl2s(format);
    out
    << "<hr>Calculator input for loading "
    << "subalgebras directly without recomputation. "
    << this->toStringProgressReport(format);
    return out.str();
  }
  std::string sl2SubalgebraReports =
  this->owner->fileNames.virtualFolderName() +
  "orbit_computation_information_" +
  FileOperations::cleanUpForFileNameUse(
    this->owner->weylGroup.dynkinType.toString()
  ) +
  ".html";
  std::string loadSubalgebrasFile =
  this->owner->fileNames.virtualFolderName() +
  "load_algebra_" +
  FileOperations::cleanUpForFileNameUse(
    this->owner->weylGroup.dynkinType.toString()
  ) +
  ".html";
  out
  << "<a href='"
  << sl2SubalgebraReports
  << "'>Nilpotent orbit computation summary</a>.";
  out
  << "<hr><a href='"
  << loadSubalgebrasFile
  << "'>Calculator input for subalgebras load</a>.";
  std::stringstream fileSl2Content, fileLoadContent;
  fileSl2Content
  << "<html>"
  << "<body>"
  << this->toStringSl2s()
  << "</body></html>";
  fileLoadContent
  << "<html>"
  << "<body>"
  << this->toStringProgressReport(format)
  << "</body></html>";
  FileOperations::writeFileVirtual(
    sl2SubalgebraReports, fileSl2Content.str(), nullptr
  );
  FileOperations::writeFileVirtual(
    loadSubalgebrasFile, fileLoadContent.str(), nullptr
  );
  return out.str();
}

void SemisimpleSubalgebras::getCentralizerChains(
  List<List<int> >& outputChains
) {
  STACK_TRACE("SemisimpleSubalgebras::getCentralizerChains");
  outputChains.setSize(0);
  Selection explored;
  explored.initialize(this->subalgebras.values.size);
  outputChains.reserve(this->subalgebras.values.size);
  for (int i = 0; i < this->subalgebras.values.size; i ++) {
    if (!explored.selected[i]) {
      outputChains.setSize(outputChains.size + 1);
      outputChains.lastObject()->setSize(0);
      outputChains.lastObject()->addOnTop(i);
      int maximalSemisimpleContainer =
      this->subalgebras.values[i].indexMaximalSemisimpleContainer;
      if (maximalSemisimpleContainer != - 1) {
        outputChains.lastObject()->addOnTop(maximalSemisimpleContainer);
        explored.addSelectionAppendNewIndex(maximalSemisimpleContainer);
      }
    }
  }
}

void SemisimpleSubalgebras::computeSl2sInitOrbitsForComputationOnDemand(
  bool computeRealSlTwos
) {
  STACK_TRACE(
    "SemisimpleSubalgebras::computeSl2sInitOrbitsForComputationOnDemand"
  );
  this->getSemisimpleOwner().findSl2Subalgebras(
    this->getSemisimpleOwner(),
    this->slTwoSubalgebras,
    computeRealSlTwos,
    this->ownerField
  );
  this->orbitHElementLengths.clear();
  this->orbitDynkinIndices.clear();
  this->orbitHElementLengths.setExpectedSize(this->slTwoSubalgebras.size);
  this->orbitDynkinIndices.setExpectedSize(this->slTwoSubalgebras.size);
  this->orbits.setSize(this->slTwoSubalgebras.size);
  List<ElementWeylGroupAutomorphisms> generators;
  WeylGroupAutomorphisms& weylAutomorphisms =
  this->slTwoSubalgebras.rootSubalgebras.weylGroupAutomorphisms;
  weylAutomorphisms.computeOuterAutoGenerators();
  ElementWeylGroupAutomorphisms element;
  for (int i = 0; i < this->owner->getRank(); i ++) {
    element.makeSimpleReflection(i, weylAutomorphisms);
    generators.addOnTop(element);
  }
  List<MatrixTensor<Rational> >& outerAutomorphisms =
  weylAutomorphisms.outerAutomorphisms.generators;
  for (int i = 0; i < outerAutomorphisms.size; i ++) {
    if (!outerAutomorphisms[i].isIdentity()) {
      element.makeOuterAuto(i, weylAutomorphisms);
      generators.addOnTop(element);
    }
  }
  for (int i = 0; i < this->slTwoSubalgebras.size; i ++) {
    this->orbitHElementLengths.addOnTop(
      this->slTwoSubalgebras[i].lengthHSquared
    );
    this->orbitDynkinIndices.addOnTop(
      DynkinSimpleType(
        'A', 1, this->slTwoSubalgebras[i].lengthHSquared
      )
    );
    this->orbits[i].initialize(
      generators,
      this->slTwoSubalgebras[i].hElement.getCartanPart()
    );
  }
}

bool SemisimpleSubalgebras::loadState(
  List<int>& currentChainIntegers,
  List<int>& numberOfExploredTypes,
  List<int>& numberOfExploredHs,
  std::stringstream& reportStream
) {
  STACK_TRACE("SemisimpleSubalgebras::loadState");
  this->findSemisimpleSubalgebrasInitialize();
  if (
    currentChainIntegers.size != numberOfExploredTypes.size ||
    currentChainIntegers.size != numberOfExploredHs.size
  ) {
    reportStream
    << "<hr>Input state is corrupt: currentChainInt.size: "
    << currentChainIntegers.size
    << ", numExploredTypes.size: "
    << numberOfExploredTypes.size
    << ", numExploredHs.size: "
    << numberOfExploredHs.size;
    return false;
  }
  this->currentHCandidatesScaledToActByTwo.setSize(0);
  this->currentNumberOfHCandidatesExplored.setSize(0);
  this->currentNumberOfLargerTypesExplored.setSize(0);
  this->currentPossibleLargerDynkinTypes.setSize(0);
  this->currentRootInjections.setSize(0);
  this->currentSubalgebraChain.setSize(0);
  for (int i = 0; i < currentChainIntegers.size; i ++) {
    if (currentChainIntegers[i] == - 1 && i == 0) {
      CandidateSemisimpleSubalgebra emptySubalgebra;
      this->makeEmptyCandidateSubalgebra(emptySubalgebra);
      this->addSubalgebraToStack(
        emptySubalgebra,
        numberOfExploredTypes[i],
        numberOfExploredHs[i]
      );
      continue;
    }
    if (
      currentChainIntegers[i] < 0 ||
      currentChainIntegers[i] >= this->subalgebras.values.size
    ) {
      reportStream
      << "<hr>Corrupt subalgebra index: "
      << currentChainIntegers[i];
      return false;
    }
    CandidateSemisimpleSubalgebra& currentSubalgebra =
    this->subalgebras.values[currentChainIntegers[i]];
    if (!currentSubalgebra.computeAndVerifyFromGeneratorsAndHs()) {
      reportStream
      << "<hr>Subalgebra "
      << currentSubalgebra.weylNonEmbedded->dynkinType.toString()
      << " is corrupt. "
      << currentSubalgebra.comments;
      return false;
    }
    bool isGood = true;
    if (
      !currentSubalgebra.cartanElementsSubalgebra.containsAtLeastOneCopyOfEach(
        this->baseSubalgebra().hsScaledToActByTwoInOrderOfCreation
      )
    ) {
      isGood = false;
    } else {
      currentSubalgebra.hsScaledToActByTwoInOrderOfCreation =
      this->baseSubalgebra().hsScaledToActByTwoInOrderOfCreation;
      for (
        int i = 0; i < currentSubalgebra.cartanElementsSubalgebra.size; i ++
      ) {
        if (
          !this->baseSubalgebra().hsScaledToActByTwoInOrderOfCreation.contains(
            currentSubalgebra.cartanElementsSubalgebra[i]
          )
        ) {
          currentSubalgebra.hsScaledToActByTwoInOrderOfCreation.addOnTop(
            currentSubalgebra.cartanElementsSubalgebra[i]
          );
          if (
            currentSubalgebra.hsScaledToActByTwoInOrderOfCreation.size >
            currentSubalgebra.cartanElementsSubalgebra.size
          ) {
            isGood = false;
            break;
          }
        }
      }
    }
    if (
      currentSubalgebra.cartanElementsSubalgebra.size !=
      this->baseSubalgebra().cartanElementsSubalgebra.size + 1
    ) {
      isGood = false;
    }
    if (!isGood) {
      reportStream
      << "<hr>Subalgebra "
      << currentSubalgebra.weylNonEmbedded->dynkinType.toString()
      << " does not appear to be parabolically induced by "
      << this->baseSubalgebra().weylNonEmbedded->dynkinType.toString()
      << ". More precisely, "
      << currentSubalgebra.weylNonEmbedded->dynkinType.toString()
      << " has h-elements "
      << currentSubalgebra.cartanElementsSubalgebra.toString()
      << " however "
      << this->baseSubalgebra().weylNonEmbedded->dynkinType.toString()
      << " has h-elements in order of creation: "
      << this->baseSubalgebra().hsScaledToActByTwoInOrderOfCreation.toString()
      << " and h-elements in order induced by the type: "
      << this->baseSubalgebra().cartanElementsSubalgebra.toString();
      return false;
    }
    this->addSubalgebraToStack(
      currentSubalgebra,
      numberOfExploredTypes[i],
      numberOfExploredHs[i]
    );
  }
  return true;
}

bool SemisimpleSubalgebras::findSemisimpleSubalgebrasContinue() {
  STACK_TRACE("SemisimpleSubalgebras::findSemisimpleSubalgebrasContinue");
  ProgressReport report;
  std::stringstream reportstream;
  this->checkAll();
  reportstream
  << "State at beginning of computation: "
  << this->toStringProgressReport();
  report.report(reportstream.str());
  while (this->incrementReturnFalseIfPastLast()) {
    this->checkAll();
    report.report(this->toStringProgressReport());
    if (!this->flagRealizedAllCandidates) {
      this->comments =
      this->comments +
      "Failed to realize all candidates, aborting computation.";
      return false;
    }
  }
  this->checkAll();
  ProgressReport report2;
  std::stringstream reportStream2;
  reportStream2
  << "Found a total of "
  << this->subalgebras.values.size
  << " subalgebras. Proceding to "
  << "adjust centralizers. ";
  report2.report(reportStream2.str());
  if (this->targetDynkinType.isEqualToZero()) {
    this->hookUpCentralizers(false);
    if (this->flagComputeNilradicals) {
      this->computePairingTablesAndFKFTtypes();
    }
  }
  this->checkAll();
  return true;
}

void SemisimpleSubalgebras::findSemisimpleSubalgebrasInitialize() {
  STACK_TRACE("SemisimpleSubalgebras::findSemisimpleSubalgebrasInitialize");
  this->checkConsistency();
  if (this->owner == nullptr) {
    global.fatal
    << "<hr>Owner of semisimple subalgebras is zero."
    << global.fatal;
  }
  if (global.response.monitoringAllowed()) {
    this->fileNameToLogComments =
    "LogFileComments_" +
    FileOperations::cleanUpForFileNameUse(
      this->owner->weylGroup.dynkinType.toString()
    ) +
    ".html";
    std::fstream logFile;
    if (
      !FileOperations::openFileCreateIfNotPresentVirtual(
        logFile,
        this->owner->fileNames.virtualFolderName() +
        this->fileNameToLogComments,
        true,
        false,
        false
      )
    ) {
      global.fatal
      << "Failed to open/create log file "
      << this->fileNameToLogComments
      << ". This is not fatal but I am crashing to let you know. ";
    }
    logFile.close();
  }
  this->computeSl2sInitOrbitsForComputationOnDemand(false);
  this->currentSubalgebraChain.setExpectedSize(
    this->owner->getRank() + 2
  );
  this->currentSubalgebraChain.setSize(0);
}

void SemisimpleSubalgebras::makeEmptyCandidateSubalgebra(
  CandidateSemisimpleSubalgebra& output
) {
  STACK_TRACE("SemisimpleSubalgebras::makeEmptyCandidateSubalgebra");
  DynkinType zeroType;
  this->makeCandidateSubalgebra(zeroType, output);
  Matrix<Rational> zeroCartan;
  output.subalgebraNonEmbeddedDefaultScale =
  &this->subalgebrasNonDefaultCartanAndScale.getValueCreateNoInitialization(
    zeroCartan
  );
  output.indexNonEmbeddedMeNonStandardCartan =
  this->subalgebrasNonDefaultCartanAndScale.getIndex(zeroCartan);
}

void SemisimpleSubalgebras::makeCandidateSubalgebra(
  const DynkinType& input, CandidateSemisimpleSubalgebra& output
) {
  STACK_TRACE("SemisimpleSubalgebras::makeCandidateSubalgebra");
  output.owner = this;
  bool needsInitialization = false;
  if (!this->subalgebrasNonEmbedded->contains(input)) {
    needsInitialization = true;
  }
  output.weylNonEmbedded =
  &this->subalgebrasNonEmbedded->getValueCreateNoInitialization(input).
  weylGroup;
  output.indexNonEmbeddedMeStandard =
  this->subalgebrasNonEmbedded->getIndex(input);
  if (needsInitialization) {
    output.weylNonEmbedded->makeFromDynkinType(input);
  }
}

void DynkinType::getPrecomputedDynkinTypes(List<DynkinType>& output) {
  STACK_TRACE("DynkinType::getPrecomputedDynkinTypes");
  output.setSize(0);
  DynkinType dynkinType;
  dynkinType.makeSimpleType('F', 4);
  output.addOnTop(dynkinType);
  for (int i = 6; i <= 8; i ++) {
    dynkinType.makeSimpleType('E', i);
    output.addOnTop(dynkinType);
  }
  dynkinType.makeSimpleType('G', 2);
  output.addOnTop(dynkinType);
  for (int i = 1; i <= 11; i ++) {
    dynkinType.makeSimpleType('A', i);
    output.addOnTop(dynkinType);
  }
  for (int i = 4; i <= 8; i ++) {
    dynkinType.makeSimpleType('D', i);
    output.addOnTop(dynkinType);
  }
  for (int i = 2; i <= 8; i ++) {
    dynkinType.makeSimpleType('B', i);
    output.addOnTop(dynkinType);
  }
  for (int i = 3; i <= 8; i ++) {
    dynkinType.makeSimpleType('C', i);
    output.addOnTop(dynkinType);
  }
}

bool SemisimpleSubalgebras::computeStructureWriteFiles(
  SemisimpleLieAlgebra& newOwner,
  AlgebraicClosureRationals& ownerField,
  MapReferences<DynkinType, SemisimpleLieAlgebra>& containerSubalgebras,
  ListReferences<SlTwoSubalgebras>& containerSl2Subalgebras,
  std::string(*toStringExpression)(SemisimpleSubalgebras&),
  std::stringstream* outputStream,
  bool forceRecompute,
  bool doFullInitialization,
  bool computeNilradicals,
  bool computeModuleDecomposition,
  bool attemptToSolveSystems,
  bool computePairingTable,
  bool adjustCentralizers,
  const std::string& extraDynkinDiagramPlot
) {
  STACK_TRACE("SemisimpleSubalgebras::computeStructureWriteFiles");
  this->toStringExpressionString = toStringExpression;
  this->owner = &newOwner;
  this->computeFolderNames(this->currentFormat);
  if (
    !FileOperations::fileExistsVirtual(this->virtualNameMainFile1) ||
    forceRecompute
  ) {
    if (doFullInitialization) {
      this->millisecondsComputationStart = global.getElapsedMilliseconds();
    }
    this->flagComputeNilradicals = computeNilradicals;
    this->flagComputeModuleDecomposition = computeModuleDecomposition;
    this->flagAttemptToSolveSystems = attemptToSolveSystems;
    this->flagComputePairingTable = computePairingTable;
    this->flagAttemptToAdjustCentralizers = adjustCentralizers;
    this->checkFileWritePermissions();
    if (doFullInitialization) {
      this->findTheSemisimpleSubalgebrasFromScratch(
        newOwner,
        ownerField,
        containerSubalgebras,
        containerSl2Subalgebras,
        nullptr
      );
    }
    this->writeReportToFiles();
    this->slTwoSubalgebras.writeHTML();
    this->owner->writeHTML(true, false, extraDynkinDiagramPlot);
  } else {
    if (outputStream != nullptr) {
      *outputStream << "Files precomputed, serving from HD. ";
    }
  }
  if (outputStream != nullptr) {
    *outputStream
    << "<hr>Semisimple subalgebras: <a href='"
    << this->displayNameMainFile1WithPath
    << "' target='_blank'>"
    << this->displayNameMainFile1NoPath
    << "</a>"
    << "<hr>"
    << "<a href='"
    << this->owner->fileNames.displayFolderName("../../output/")
    << this->owner->fileNames.fileNameRelativePathSlTwoSubalgebras()
    << "' target='_blank'>sl(2)-sublagberas</a>";
  }
  return true;
}

bool SemisimpleSubalgebras::computeStructureRealFormsWriteFiles(
  SemisimpleLieAlgebra& newOwner,
  AlgebraicClosureRationals& ownerField,
  MapReferences<DynkinType, SemisimpleLieAlgebra>& containerSubalgebras,
  ListReferences<SlTwoSubalgebras>& containerSl2Subalgebras,
  std::stringstream* outputStream
) {
  STACK_TRACE("SemisimpleSubalgebras::computeStructureRealFormsWriteFiles");
  this->computeStructureRealForms(
    newOwner, ownerField, containerSubalgebras, containerSl2Subalgebras
  );
  this->writeFilesRealForms(outputStream);
  return true;
}

bool SemisimpleSubalgebras::computeStructureRealFormsInitialize(
  SemisimpleLieAlgebra& newOwner,
  AlgebraicClosureRationals& ownerField,
  MapReferences<DynkinType, SemisimpleLieAlgebra>& containerSubalgebras,
  ListReferences<SlTwoSubalgebras>& containerSl2Subalgebras
) {
  STACK_TRACE("SemisimpleSubalgebras::computeStructureRealFormsInitialize");
  if (this->subalgebrasNonEmbedded == nullptr || this->owner == nullptr) {
    this->initHookUpPointers(
      newOwner,
      &ownerField,
      &containerSubalgebras,
      &containerSl2Subalgebras
    );
  }
  this->checkInitialization();
  this->owner->computeChevalleyConstants();
  this->targetDynkinType.makeZero();
  return true;
}

bool SemisimpleSubalgebras::computeStructureRealForms(
  SemisimpleLieAlgebra& newOwner,
  AlgebraicClosureRationals& ownerField,
  MapReferences<DynkinType, SemisimpleLieAlgebra>& containerSubalgebras,
  ListReferences<SlTwoSubalgebras>& containerSl2Subalgebras
) {
  STACK_TRACE("SemisimpleSubalgebras::computeStructureRealForms");
  this->computeStructureRealFormsInitialize(
    newOwner, ownerField, containerSubalgebras, containerSl2Subalgebras
  );
  this->slTwoSubalgebras.checkMinimalContainingRootSubalgebras();
  this->owner->findSl2Subalgebras(
    newOwner, this->slTwoSubalgebras, true, &ownerField
  );
  this->computeStructureRealFormsSlTwos();
  return true;
}

bool SemisimpleSubalgebras::computeStructureRealFormsSlTwos() {
  STACK_TRACE("SemisimpleSubalgebras::computeStructureRealFormsSlTwos");
  CandidateSemisimpleSubalgebra emptyCandidate;
  this->makeEmptyCandidateSubalgebra(emptyCandidate);
  this->addSubalgebraToStack(emptyCandidate, 0, 0);
  this->flagComputeModuleDecomposition = true;
  this->flagRealForms = true;
  this->wConjecture.computeBeforeSubalgebras(*this);
  for (int i = 0; i < this->slTwoSubalgebras.size; i ++) {
    this->computeStructureRealFormOneSlTwo(
      this->slTwoSubalgebras.getElement(i), i
    );
  }
  // this->hookUpCentralizers(true);
  this->wConjecture.computeAfterSubalgebras(*this);
  return true;
}

void SemisimpleSubalgebras::makeCandidateFromSlTwo(
  SlTwoSubalgebra& input, CandidateSemisimpleSubalgebra& candidate
) {
  STACK_TRACE("SemisimpleSubalgebras::makeCandidateFromSlTwo");
  candidate.indexInducedFrom = - 1;
  DynkinType incomingType;
  input.computeDynkinTypeEmbedded(incomingType);
  this->makeCandidateSubalgebra(incomingType, candidate);
  Matrix<Rational> matrix;
  matrix.makeZeroMatrix(1, 0);
  matrix(0, 0) = 2;
  candidate.subalgebraNonEmbeddedDefaultScale =
  &this->subalgebrasNonDefaultCartanAndScale.getValueCreateNoInitialization(
    matrix
  );
  DynkinType aOneDefaultScale;
  aOneDefaultScale.makeSimpleType('A', 1);
  candidate.subalgebraNonEmbeddedDefaultScale->weylGroup.makeFromDynkinType(
    aOneDefaultScale
  );
  Vector<Rational> hElement = input.hElement.getCartanPart();
  candidate.cartanElementsScaledToActByTwo.addOnTop(hElement);
  candidate.cartanElementsSubalgebra.addOnTop(hElement);
  candidate.hsScaledToActByTwoInOrderOfCreation.addOnTop(hElement);
  candidate.cartanSubalgebrasByComponentScaledToActByTwo.setSize(1);
  candidate.cartanSubalgebrasByComponentScaledToActByTwo[0].setSize(1);
  candidate.cartanSubalgebrasByComponentScaledToActByTwo[0][0] = hElement;
}

bool SemisimpleSubalgebras::computeStructureRealFormOneSlTwo(
  SlTwoSubalgebra& input, int indexInSlTwos
) {
  STACK_TRACE("SemisimpleSubalgebras::computeStructureRealFormOneSlTwo");
  CandidateSemisimpleSubalgebra candidate;
  this->makeCandidateFromSlTwo(input, candidate);
  candidate.indexInSlTwos = indexInSlTwos;
  candidate.positiveGenerators.addOnTop(input.eKostantSekiguchi);
  candidate.negativeGenerators.addOnTop(input.fKostantSekiguchi);
  candidate.flagSystemSolved = true;
  if (!candidate.computeFromGenerators(false)) {
    return false;
  }
  candidate.adjustCentralizerAndRecompute(false);
  candidate.computeCharacter(false);
  candidate.computePrimalModuleDecomposition();
  CandidateSemisimpleSubalgebra* added =
  this->addSubalgebraIfNewSetToStackTop(candidate);
  return this->computeStructureRealFormOneSlTwoPartTwo(*added, input);
}

bool SemisimpleSubalgebras::computeStructureRealFormOneSlTwoPartTwo(
  CandidateSemisimpleSubalgebra& added, SlTwoSubalgebra& input
) {
  added.wConjecture.compute(added, input);
  this->removeLastSubalgebraFromStack();
  return true;
}

bool SemisimpleSubalgebras::writeFilesRealForms(
  std::stringstream* outputStream
) {
  STACK_TRACE("SemisimpleSubalgebras::writeFilesRealForms");
  this->fileNamePrefix = "real_form_";
  FormatExpressions format;
  this->computeFolderNames(format);
  std::stringstream content;
  content
  << "<html><title>"
  << "Semisimple subalgebras of the semisimple Lie algebras: "
  << "the subalgebras of "
  << this->owner->weylGroup.dynkinType.toString()
  << "</title>";
  content << SemisimpleLieAlgebra::toHTMLCalculatorHeadElements();
  content << SemisimpleLieAlgebra::toHTMLCalculatorBodyOnload();
  content << this->toString(&this->currentFormat, true);
  content << "</body></html>";
  std::string fileName =
  this->owner->fileNames.virtualFolderName() +
  this->owner->fileNames.fileNameSlTwoRealFormSubalgebraStructure();
  this->slTwoSubalgebras.writeHTML(nullptr);
  return
  FileOperations::writeFileVirtual(fileName, content.str(), outputStream);
}

bool SemisimpleSubalgebras::checkIsEmpty() const {
  STACK_TRACE("SemisimpleSubalgebras::checkIsEmpty");
  if (this->subalgebras.size() != 0) {
    global.fatal
    << "Not expected: subalgebras are supposed to be empty, but there are: "
    << this->subalgebras.size()
    << " of them."
    << global.fatal;
  }
  this->checkInitialization();
  this->checkAll();
  return true;
}

bool SemisimpleSubalgebras::findTheSemisimpleSubalgebrasFromScratch(
  SemisimpleLieAlgebra& newOwner,
  AlgebraicClosureRationals& ownerField,
  MapReferences<DynkinType, SemisimpleLieAlgebra>& containerSubalgebras,
  ListReferences<SlTwoSubalgebras>& containerSl2Subalgebras,
  const DynkinType* targetType
) {
  STACK_TRACE(
    "SemisimpleSubalgebras::findTheSemisimpleSubalgebrasFromScratch"
  );
  this->resetComputations();
  this->initHookUpPointers(
    newOwner,
    &ownerField,
    &containerSubalgebras,
    &containerSl2Subalgebras
  );
  this->checkIsEmpty();
  this->owner->computeChevalleyConstants();
  this->targetDynkinType.makeZero();
  if (targetType != nullptr) {
    this->targetDynkinType = *targetType;
  }
  this->findSemisimpleSubalgebrasInitialize();
  CandidateSemisimpleSubalgebra emptyCandidate;
  this->makeEmptyCandidateSubalgebra(emptyCandidate);
  this->addSubalgebraToStack(emptyCandidate, 0, 0);
  this->checkAll();
  return this->findSemisimpleSubalgebrasContinue();
}

bool SemisimpleSubalgebras::ranksAndIndicesFit(const DynkinType& input) const {
  if (input.getRank() > this->owner->getRank()) {
    return false;
  }
  for (int i = 0; i < input.size(); i ++) {
    if (
      !this->orbitHElementLengths.contains(
        input[i].cartanSymmetricInverseScale * 2
      )
    ) {
      return false;
    }
  }
  return true;
}

bool SemisimpleSubalgebras::growDynkinType(
  const DynkinType& input,
  List<DynkinType>& output,
  List<List<int> >* outputImagesSimpleRoots
) const {
  STACK_TRACE("SemisimpleSubalgebras::growDynkinType");
  HashedList<Rational> lengths;
  lengths.setExpectedSize(this->orbitHElementLengths.size);
  for (int i = 0; i < this->orbitHElementLengths.size; i ++) {
    lengths.addOnTopNoRepetition(this->orbitHElementLengths[i] / 2);
  }
  output.setSize(0);
  if (outputImagesSimpleRoots != nullptr) {
    outputImagesSimpleRoots->setSize(0);
  }
  return
  input.grow(
    lengths, this->owner->getRank(), output, outputImagesSimpleRoots
  );
}

Vector<Rational> SemisimpleSubalgebras::
getHighestWeightFundamentalNewComponentFromImagesOldSimpleRootsAndNewRoot(
  const DynkinType& input,
  const List<int>& imagesOldSimpleRootsAndNewRoot,
  CandidateSemisimpleSubalgebra& subalgebraToBeModified
) {
  STACK_TRACE(
    "SemisimpleSubalgebras::"
    "getHighestWeightFundNewComponentFromImagesOldSimpleRootsAndNewRoot"
  );
  subalgebraToBeModified.owner = this;
  Vector<Rational> result;
  if (input.isEqualToZero()) {
    this->makeEmptyCandidateSubalgebra(subalgebraToBeModified);
    result.makeZero(0);
    return result;
  } else {
    this->makeCandidateSubalgebra(input, subalgebraToBeModified);
  }
  Vector<Rational> newSimpleRoot;
  Vector<Rational> highestRootInSimpleRootModuleSimpleCoordinates;
  WeylGroupData& weyl = *subalgebraToBeModified.weylNonEmbedded;
  weyl.computeRho(true);
  subalgebraToBeModified.checkCandidateInitialization();
  int newIndex = *imagesOldSimpleRootsAndNewRoot.lastObject();
  int newRank = weyl.getDimension();
  newSimpleRoot.makeEi(newRank, newIndex);
  Vectors<Rational> simpleBasisOld;
  simpleBasisOld.setSize(newRank - 1);
  if (imagesOldSimpleRootsAndNewRoot.size != newRank) {
    global.fatal
    << "The root images must be "
    << newRank
    << " but there are "
    << imagesOldSimpleRootsAndNewRoot.size
    << " elements instead. "
    << "The type is "
    << input.toString()
    << ". "
    << global.fatal;
  }
  for (int i = 0; i < newRank - 1; i ++) {
    simpleBasisOld[i].makeEi(newRank, imagesOldSimpleRootsAndNewRoot[i]);
  }
  weyl.computeExtremeRootInTheSameKMod(
    simpleBasisOld,
    newSimpleRoot,
    highestRootInSimpleRootModuleSimpleCoordinates,
    true
  );
  result.setSize(newRank - 1);
  for (int i = 0; i < simpleBasisOld.size; i ++) {
    result[i] =
    weyl.rootScalarCartanRoot(
      highestRootInSimpleRootModuleSimpleCoordinates, simpleBasisOld[i]
    ) *
    2 /
    weyl.rootScalarCartanRoot(simpleBasisOld[i], simpleBasisOld[i]);
  }
  return result;
}

void CandidateSemisimpleSubalgebra::setUpInjectionHs(
  const CandidateSemisimpleSubalgebra& baseSubalgebra,
  const DynkinType& newType,
  const List<int>& rootInjection,
  Vector<Rational>* newHScaledToActByTwo
) {
  STACK_TRACE("CandidateSemisimpleSubalgebra::setUpInjectionHs");
  this->reset(baseSubalgebra.owner);
  this->owner->makeCandidateSubalgebra(newType, *this);
  this->checkCandidateInitialization();
  this->hsScaledToActByTwoInOrderOfCreation.reserve(
    baseSubalgebra.hsScaledToActByTwoInOrderOfCreation.size + 1
  );
  this->hsScaledToActByTwoInOrderOfCreation =
  baseSubalgebra.hsScaledToActByTwoInOrderOfCreation;
  if (newHScaledToActByTwo != nullptr) {
    this->hsScaledToActByTwoInOrderOfCreation.addOnTop(*newHScaledToActByTwo);
  }
  DynkinSimpleType newComponent =
  this->weylNonEmbedded->dynkinType.getSmallestSimpleType();
  this->cartanSubalgebrasByComponentScaledToActByTwo =
  baseSubalgebra.cartanSubalgebrasByComponentScaledToActByTwo;
  int indexOffset =
  this->weylNonEmbedded->dynkinType.getRank() - newComponent.rank;
  int newIndexInNewComponent = 0;
  newIndexInNewComponent = *rootInjection.lastObject() - indexOffset;
  if (newComponent.rank == 1) {
    this->cartanSubalgebrasByComponentScaledToActByTwo.setSize(
      this->cartanSubalgebrasByComponentScaledToActByTwo.size + 1
    );
    this->cartanSubalgebrasByComponentScaledToActByTwo.lastObject()->setSize(
      1
    );
    if (newHScaledToActByTwo != nullptr) {
      (*this->cartanSubalgebrasByComponentScaledToActByTwo.lastObject())[0] =
      *newHScaledToActByTwo;
    } else {
      (*this->cartanSubalgebrasByComponentScaledToActByTwo.lastObject())[0].
      setSize(0);
    }
  } else {
    Vectors<Rational>& oldComponentHs =
    *baseSubalgebra.cartanSubalgebrasByComponentScaledToActByTwo.lastObject();
    Vectors<Rational>& currentHsScaledToActByTwo =
    *this->cartanSubalgebrasByComponentScaledToActByTwo.lastObject();
    currentHsScaledToActByTwo.setSize(currentHsScaledToActByTwo.size + 1);
    for (int i = 0; i < newComponent.rank - 1; i ++) {
      currentHsScaledToActByTwo[
        rootInjection[indexOffset + i] - indexOffset
      ] =
      oldComponentHs[i];
    }
    if (newHScaledToActByTwo != nullptr) {
      currentHsScaledToActByTwo[newIndexInNewComponent] =
      *newHScaledToActByTwo;
    } else {
      currentHsScaledToActByTwo[newIndexInNewComponent].setSize(0);
    }
  }
  this->computeHsAndHsScaledToActByTwoFromComponents();
}

void CandidateSemisimpleSubalgebra::
computeHsAndHsScaledToActByTwoFromComponents() {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebra::"
    "computeHsAndHsScaledToActByTwoFromComponents"
  );
  this->cartanElementsScaledToActByTwo.assignListList(
    this->cartanSubalgebrasByComponentScaledToActByTwo
  );
  this->cartanElementsSubalgebra.setSize(
    this->cartanElementsScaledToActByTwo.size
  );
  Matrix<Rational> cartanInComponentOrder;
  this->weylNonEmbedded->dynkinType.
  getCartanSymmetricDefaultLengthKeepComponentOrder(cartanInComponentOrder);
  this->subalgebraNonEmbeddedDefaultScale =
  &this->owner->subalgebrasNonDefaultCartanAndScale.
  getValueCreateNoInitialization(cartanInComponentOrder);
  this->subalgebraNonEmbeddedDefaultScale->weylGroup.
  makeFromDynkinTypeDefaultLengthKeepComponentOrder(
    this->weylNonEmbedded->dynkinType
  );
  this->subalgebraNonEmbeddedDefaultScale->weylGroup.computeRho(true);
  this->subalgebraNonEmbeddedDefaultScale->computeChevalleyConstants();
  this->indexNonEmbeddedMeNonStandardCartan =
  this->owner->subalgebrasNonDefaultCartanAndScale.getIndex(
    cartanInComponentOrder
  );
  int counter = - 1;
  List<DynkinSimpleType> dynkinTypes;
  this->weylNonEmbedded->dynkinType.getTypesWithMults(dynkinTypes);
  for (
    int i = 0; i < this->cartanSubalgebrasByComponentScaledToActByTwo.size; i
    ++
  ) {
    for (
      int j = 0; j < this->cartanSubalgebrasByComponentScaledToActByTwo[i].
      size; j ++
    ) {
      counter ++;
      this->cartanElementsSubalgebra[counter] = (
        this->cartanElementsScaledToActByTwo[counter] *
        dynkinTypes[i].getDefaultRootLengthSquared(j)
      ) /
      2;
    }
  }
}

bool SemisimpleSubalgebras::setUpParabolicInductionDataPrecomputedSubalgebra(
  CandidateSemisimpleSubalgebra& candidate
) {
  STACK_TRACE(
    "SemisimpleSubalgebras::"
    "setUpParabolicInductionDataPrecomputedSubalgebra"
  );
  int indexPrecomputed =
  this->subalgebras.getIndex(candidate.cartanElementsSubalgebra);
  if (indexPrecomputed == - 1) {
    return false;
  }
  if (
    this->subalgebras.values[indexPrecomputed].
    hsScaledToActByTwoInOrderOfCreation.size !=
    candidate.cartanElementsSubalgebra.size
  ) {
    this->subalgebras.values[indexPrecomputed].
    hsScaledToActByTwoInOrderOfCreation =
    candidate.hsScaledToActByTwoInOrderOfCreation;
  }
  this->subalgebras.values[indexPrecomputed].indexInducedFrom =
  candidate.indexInducedFrom;
  this->subalgebras.values[indexPrecomputed].rootInjectionsFromInducer =
  candidate.rootInjectionsFromInducer;
  candidate = this->subalgebras.values[indexPrecomputed];
  return candidate.computeAndVerifyFromGeneratorsAndHs();
}

bool CandidateSemisimpleSubalgebra::createAndAddExtendBaseSubalgebra(
  const CandidateSemisimpleSubalgebra& baseSubalgebra,
  Vector<Rational>& newHRescaledToActByTwo,
  const DynkinType& newType,
  const List<int>& rootInjection
) {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebra::"
    "createAndAddExtendBaseSubalgebra"
  );
  this->setUpInjectionHs(
    baseSubalgebra, newType, rootInjection, &newHRescaledToActByTwo
  );
  this->indexInducedFrom = baseSubalgebra.indexInOwner;
  if (this->getRank() == 1) {
    this->indexInSlTwos = - 1;
    this->owner->slTwoSubalgebras.containsSl2WithGivenH(
      newHRescaledToActByTwo, &this->indexInSlTwos
    );
  }
  this->rootInjectionsFromInducer = rootInjection;
  // induction history is complete.
  if (
    this->owner->setUpParabolicInductionDataPrecomputedSubalgebra(*this)
  ) {
    return true;
  }
  this->checkFullInitialization();
  if (
    !baseSubalgebra.weylNonEmbedded->dynkinType.isEqualToZero() &&
    baseSubalgebra.indexInOwner == - 1
  ) {
    global.fatal
    << "Attempt to induce a subalgebra "
    << "from a non-registered base subalgebra of type "
    << baseSubalgebra.weylNonEmbedded->dynkinType.toString()
    << ". "
    << global.fatal;
  }
  ProgressReport report;
  if (!this->computeCharacter(false)) {
    if (report.tickAndWantReport()) {
      report.report(
        "Candidate " +
        this->weylNonEmbedded->dynkinType.toString() +
        " doesn't have fitting chars."
      );
    }
    return false;
  }
  this->computeSystem(false, false);
  if (this->flagSystemProvedToHaveNoSolution) {
    if (report.tickAndWantReport()) {
      report.report(
        "Candidate " +
        this->weylNonEmbedded->dynkinType.toString() +
        " -> no system solution."
      );
    }
  }
  for (int i = 0; i < this->owner->subalgebras.values.size; i ++) {
    if (
      this->weylNonEmbedded->dynkinType ==
      this->owner->subalgebras.values[i].weylNonEmbedded->dynkinType
    ) {
      if (
        this->isDirectSummandOf(this->owner->subalgebras.values[i])
      ) {
        return false;
      }
    } else {
      if (
        this->weylNonEmbedded->dynkinType.toString() ==
        this->owner->subalgebras.values[i].weylNonEmbedded->dynkinType.toString
        ()
      ) {
        global.fatal
        << "This is not supposed to happen: "
        << "different Dynkin types with equal toString outputs. "
        << global.fatal;
      }
    }
  }
  return !this->flagSystemProvedToHaveNoSolution;
}

const Vector<Rational>& OrbitIteratorRootActionWeylGroupAutomorphisms::
getCurrentElement() {
  STACK_TRACE(
    "OrbitIteratorRootActionWeylGroupAutomorphisms"
    "::getCurrentElement"
  );
  if (this->flagOrbitIsBuffered) {
    return this->orbitBuffer[this->currentIndexInBuffer];
  }
  return this->iterator.getCurrentElement();
}

bool OrbitIteratorRootActionWeylGroupAutomorphisms::checkConsistency() {
  if (this->flagOrbitIsBuffered) {
    if (this->currentIndexInBuffer >= this->orbitBuffer.size) {
      global.fatal
      << "Current buffer index is: "
      << this->currentIndexInBuffer
      << " but the orbit has "
      << this->orbitBuffer.size
      << " elements. "
      << global.fatal;
    }
  }
  if (this->flagOrbitEnumeratedOnce) {
    if (this->currentIndexInBuffer >= this->orbitSize) {
      global.fatal
      << "Current buffer index is: "
      << this->currentIndexInBuffer
      << " but the orbit was computed to have "
      << this->orbitSize
      << " elements. "
      << global.fatal;
    }
  }
  if (this->flagOrbitIsBuffered) {
    if (this->orbitSize != this->orbitBuffer.size) {
      global.fatal
      << "Orbit is supposed to be buffered but the orbit buffer has "
      << this->orbitBuffer.size
      << " elements "
      << "and the orbit size is reported to be: "
      << this->orbitSize
      << ". A detailed orbit printout: <br>"
      << this->toString()
      << global.fatal;
    }
  }
  if (!this->flagOrbitEnumeratedOnce && this->flagOrbitIsBuffered) {
    global.fatal
    << "Orbit reported to be buffered but at "
    << "the same time it hasn't been enumerated yet. "
    << this->toString()
    << global.fatal;
  }
  return true;
}

bool OrbitIteratorRootActionWeylGroupAutomorphisms::
incrementReturnFalseIfPastLast() {
  STACK_TRACE(
    "OrbitIteratorRootActionWeylGroupAutomorphisms"
    "::incrementReturnFalseIfPastLast"
  );
  this->checkConsistency();
  this->iterator.checkInitialization();
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
    return this->iterator.incrementReturnFalseIfPastLastFALSE();
  }
  if (!this->iterator.incrementReturnFalseIfPastLastFALSE()) {
    this->orbitSize = this->currentIndexInBuffer;
    if (this->computedSize != - 1) {
      if (this->computedSize != this->orbitSize) {
        global.fatal
        << "This is a mathematical error: "
        << "the computed size of the orbit is "
        << this->computedSize.toString()
        << " "
        << "but I enumerated an orbit of size "
        << this->orbitSize
        << ". More details on the orbit follow. "
        << this->toString()
        << global.fatal;
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
    this->orbitBuffer.addOnTop(this->iterator.getCurrentElement());
  }
  return true;
}

void OrbitIteratorRootActionWeylGroupAutomorphisms::initialize() {
  STACK_TRACE("OrbitFDRepIteratorWeylGroupAutomorphisms::initialize");
  this->iterator.checkInitialization();
  this->currentIndexInBuffer = 0;
  if (this->flagOrbitIsBuffered) {
    return;
  }
  this->iterator.initialize(
    this->iterator.groupGeneratingElements,
    this->orbitDefiningElement,
    this->iterator.groupAction
  );
  if (this->iterator.groupGeneratingElements.size > 0) {
    WeylGroupAutomorphisms& ownerGroup =
    *this->iterator.groupGeneratingElements[0].owner;
    this->computedSize = ownerGroup.getOrbitSize(this->orbitDefiningElement);
    if (this->computedSize > this->maxOrbitBufferSize) {
      this->maxOrbitBufferSize = 0;
      this->orbitBuffer.setSize(0);
    }
  }
  this->iterator.checkInitialization();
}

void OrbitIteratorRootActionWeylGroupAutomorphisms::initialize(
  const List<ElementWeylGroupAutomorphisms>& inputGenerators,
  const Vector<Rational>& inputElement
) {
  STACK_TRACE("OrbitFDRepIteratorWeylGroupAutomorphisms::initialize");
  if (this->orbitDefiningElement == inputElement) {
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
  this->iterator.initialize(
    inputGenerators,
    this->orbitDefiningElement,
    this->iterator.groupAction
  );
}

OrbitIteratorRootActionWeylGroupAutomorphisms::
OrbitIteratorRootActionWeylGroupAutomorphisms() {
  this->iterator.groupAction.name = "WeylGroupAutomorphismRootAction";
  this->iterator.groupAction.actOn = ElementWeylGroupAutomorphisms::actOn;
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
  this->iterator.resetNoActionChange();
}

std::string OrbitIteratorRootActionWeylGroupAutomorphisms::toString() const {
  std::stringstream out;
  out
  << " The orbit defining element is: "
  << this->orbitDefiningElement.toString()
  << ". ";
  out
  << "<br>Current element number: "
  << this->currentIndexInBuffer + 1
  << ". ";
  if (this->orbitSize != - 1) {
    out << "<br>The orbit size is: " << this->orbitSize << ".";
  }
  if (this->computedSize != - 1) {
    out
    << "<br>The computed orbit size is: "
    << this->computedSize.toString()
    << ". ";
  }
  if (this->flagOrbitIsBuffered) {
    out
    << "<br>The orbit is buffered, the orbit elements are: "
    << this->orbitBuffer.toString();
  } else {
    out
    << "<br>The orbit is either too large or not yet fully enumerated. "
    << this->iterator.toStringLayerSize()
    << ". The current buffer size is: "
    << this->orbitBuffer.size
    << ". ";
    if (this->currentIndexInBuffer + 1 > this->orbitBuffer.size) {
      out
      << "The orbit buffer appears to have exceeded the allowed maximum, "
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
  STACK_TRACE("SemisimpleSubalgebras::getHCandidates");
  ProgressReport report1;
  ProgressReport report2;
  ProgressReport report3;
  int baseRank = currentType.getRank() - 1;
  DynkinSimpleType smallType = currentType.getSmallestSimpleType();
  if (report1.tickAndWantReport()) {
    std::stringstream reportStream;
    reportStream
    << "the latest root of the candidate simple component "
    << smallType.toString();
    report1.report(reportStream.str());
  }
  int indexNewRooT = *currentRootInjection.lastObject();
  int indexNewRootInSmallType =
  indexNewRooT - currentType.getRank() + smallType.rank;
  Rational desiredHScaledToActByTwoLengthSquared =
  smallType.cartanSymmetricInverseScale * 4 /
  smallType.getDefaultRootLengthSquared(indexNewRootInSmallType);
  outputHCandidatesScaledToActByTwo.setSize(0);
  for (int j = 0; j < this->slTwoSubalgebras.size; j ++) {
    if (report2.tickAndWantReport()) {
      std::stringstream reportStreamX;
      reportStreamX << "Trying to realize via orbit number " << j + 1 << ".";
      if (
        this->slTwoSubalgebras[j].lengthHSquared !=
        desiredHScaledToActByTwoLengthSquared
      ) {
        reportStreamX
        << " The h element "
        << this->slTwoSubalgebras[j].hElement.getCartanPart().toString()
        << " of length "
        << this->orbitHElementLengths[j].toString()
        << " generating orbit number "
        << j + 1
        << " out of "
        << this->slTwoSubalgebras.size
        << " does not have the required length of "
        << desiredHScaledToActByTwoLengthSquared.toString();
      }
      report2.report(reportStreamX.str());
    }
    if (
      this->slTwoSubalgebras[j].lengthHSquared !=
      desiredHScaledToActByTwoLengthSquared
    ) {
      continue;
    }
    if (baseRank == 0) {
      outputHCandidatesScaledToActByTwo.addOnTop(
        this->slTwoSubalgebras[j].hElement.getCartanPart()
      );
      // Orbit of not generated because that is the very first H element
      // selected.
      continue;
    }
    Vector<Rational> currentCandidate;
    OrbitIteratorRootActionWeylGroupAutomorphisms& currentOrbit =
    this->orbits[j];
    currentOrbit.initialize();
    do {
      currentCandidate = currentOrbit.getCurrentElement();
      if (
        newCandidate.isGoodHNewActingByTwo(
          currentCandidate, currentRootInjection
        )
      ) {
        if (report3.tickAndWantReport()) {
          std::stringstream out2, out3;
          out3
          << "So far, found "
          << outputHCandidatesScaledToActByTwo.size + 1
          << " good candidates. ";
          report2.report(out3.str());
          out2 << "sl(2) orbit " << j + 1 << ". " << currentOrbit.toString();
          out2 << "Current element has desired scalar products. ";
          report3.report(out2.str());
        }
        outputHCandidatesScaledToActByTwo.addOnTop(
          currentOrbit.getCurrentElement()
        );
      } else {
        if (report3.tickAndWantReport()) {
          std::stringstream out2;
          out2
          << "sl(2) orbit "
          << j + 1
          << ". "
          << currentOrbit.toString()
          << "\n<br>Current element is not a valid candidate. ";
          report3.report(out2.str());
        }
      }
    } while (currentOrbit.incrementReturnFalseIfPastLast());
    if (outputHCandidatesScaledToActByTwo.size == 0) {
      std::stringstream out2;
      out2
      << "Sl(2) orbit "
      << j + 1
      << ": extension to "
      << currentType.toString()
      << " not possible because there were no h candidates.";
      report1.report(out2.str());
    }
  }
}

const CandidateSemisimpleSubalgebra& SemisimpleSubalgebras::baseSubalgebra() {
  if (
    !this->currentSubalgebraChain.lastObject()->weylNonEmbedded->dynkinType.
    isEqualToZero() &&
    this->currentSubalgebraChain.lastObject()->indexInOwner == - 1
  ) {
    global.fatal
    << "Base subalgebra has index in owner equal to - 1 yet "
    << "is of non-zero type: "
    << this->currentSubalgebraChain.lastObject()->weylNonEmbedded->dynkinType.
    toString()
    << ". "
    << global.fatal;
  }
  return *this->currentSubalgebraChain.lastObject();
}

bool SemisimpleSubalgebras::removeLastSubalgebraFromStack() {
  STACK_TRACE("SemisimpleSubalgebras::removeLastSubalgebraFromStack");
  this->currentSubalgebraChain.setSize(
    this->currentSubalgebraChain.size - 1
  );
  this->currentPossibleLargerDynkinTypes.setSize(
    this->currentSubalgebraChain.size
  );
  this->currentNumberOfLargerTypesExplored.setSize(
    this->currentSubalgebraChain.size
  );
  this->currentNumberOfHCandidatesExplored.setSize(
    this->currentSubalgebraChain.size
  );
  this->currentHCandidatesScaledToActByTwo.setSize(
    this->currentSubalgebraChain.size
  );
  this->currentRootInjections.setSize(this->currentSubalgebraChain.size);
  return this->currentSubalgebraChain.size > 0;
}

bool SemisimpleSubalgebras::getCentralizerTypeIfComputableAndKnown(
  const DynkinType& input, DynkinType& output
) {
  STACK_TRACE("SemisimpleSubalgebras::getCentralizerTypeIfComputableAndKnown");
  // This function is rudimentary and fails to return a good result in many
  // cases when
  // a result is actually computable. This needs to be improved.
  if (!input.isSimple()) {
    return false;
  }
  int index = - 1;
  for (
    int i = 0; i < this->slTwoSubalgebras.rootSubalgebras.subalgebras.size; i
    ++
  ) {
    if (
      this->slTwoSubalgebras.rootSubalgebras.subalgebras[i].dynkinType == input
    ) {
      if (index != - 1) {
        index = - 1;
        break;
      }
      index = i;
    }
  }
  if (index == - 1) {
    return false;
  }
  output =
  this->slTwoSubalgebras.rootSubalgebras.subalgebras[index].
  centralizerDynkinType;
  return true;
}

void DynkinType::getDynkinIndicesSl2SubalgebrasSimpleType(
  const DynkinSimpleType& desiredType,
  List<List<Rational> >& precomputedDynkinIndicesSl2subalgebrasSimpleTypes,
  HashedList<DynkinSimpleType>& dynkinSimpleTypesWithComputedSl2Subalgebras,
  HashedList<Rational>& outputDynkinIndices,
  AlgebraicClosureRationals* algebraicClosure
) {
  STACK_TRACE("DynkinType::getDynkinIndicesSl2SubalgebrasSimpleType");
  DynkinSimpleType typeDefaultScale(
    desiredType.letter, desiredType.rank, 1
  );
  if (
    !dynkinSimpleTypesWithComputedSl2Subalgebras.contains(typeDefaultScale)
  ) {
    HashedList<Rational> outputIndicesDefaultScale;
    SemisimpleLieAlgebra simpleAlgebra;
    SlTwoSubalgebras sl2s;
    simpleAlgebra.weylGroup.makeArbitrarySimple(
      typeDefaultScale.letter, typeDefaultScale.rank
    );
    simpleAlgebra.computeChevalleyConstants();
    simpleAlgebra.findSl2Subalgebras(
      simpleAlgebra, sl2s, false, algebraicClosure
    );
    dynkinSimpleTypesWithComputedSl2Subalgebras.addOnTop(typeDefaultScale);
    outputIndicesDefaultScale.setExpectedSize(sl2s.size);
    for (int i = 0; i < sl2s.size; i ++) {
      outputIndicesDefaultScale.addOnTopNoRepetition(
        sl2s[i].lengthHSquared / 2
      );
    }
    precomputedDynkinIndicesSl2subalgebrasSimpleTypes.addOnTop(
      outputIndicesDefaultScale
    );
  }
  List<Rational>& outputIndicesDefaultScale =
  precomputedDynkinIndicesSl2subalgebrasSimpleTypes[
    dynkinSimpleTypesWithComputedSl2Subalgebras.getIndex(typeDefaultScale)
  ];
  outputDynkinIndices.setExpectedSize(outputIndicesDefaultScale.size);
  outputDynkinIndices.clear();
  for (int i = 0; i < outputIndicesDefaultScale.size; i ++) {
    outputDynkinIndices.addOnTop(
      outputIndicesDefaultScale[i] * desiredType.cartanSymmetricInverseScale
    );
  }
}

void DynkinType::getDynkinIndicesSl2Subalgebras(
  List<List<Rational> >& precomputedDynkinIndicesSl2subalgebrasSimpleTypes,
  HashedList<DynkinSimpleType>& dynkinSimpleTypesWithComputedSl2Subalgebras,
  HashedList<Rational>& outputDynkinIndices,
  AlgebraicClosureRationals* algebraicClosure
) {
  STACK_TRACE("DynkinType::getDynkinIndicesSl2Subalgebras");
  List<DynkinSimpleType> dynkinTypes;
  this->getTypesWithMults(dynkinTypes);
  List<List<Rational> > dynkinIndicesPerType;
  HashedList<Rational> bufferIndices;
  for (int i = 0; i < dynkinTypes.size; i ++) {
    this->getDynkinIndicesSl2SubalgebrasSimpleType(
      dynkinTypes[i],
      precomputedDynkinIndicesSl2subalgebrasSimpleTypes,
      dynkinSimpleTypesWithComputedSl2Subalgebras,
      bufferIndices,
      algebraicClosure
    );
    dynkinIndicesPerType.setSize(dynkinIndicesPerType.size + 1);
    dynkinIndicesPerType.lastObject()->addOnTop(0);
    dynkinIndicesPerType.lastObject()->addListOnTop(bufferIndices);
  }
  SelectionWithDifferentMaxMultiplicities dynkinIndexSelector;
  List<int> multiplicities;
  multiplicities.setSize(dynkinTypes.size);
  for (int i = 0; i < multiplicities.size; i ++) {
    multiplicities[i] = dynkinIndicesPerType[i].size - 1;
  }
  dynkinIndexSelector.initializeFromIntegers(multiplicities);
  outputDynkinIndices.clear();
  while (dynkinIndexSelector.incrementReturnFalseIfPastLast()) {
    Rational currentIndex = 0;
    for (int i = 0; i < dynkinIndexSelector.multiplicities.size; i ++) {
      currentIndex +=
      dynkinIndicesPerType[i][dynkinIndexSelector.multiplicities[i]];
    }
    outputDynkinIndices.addOnTopNoRepetition(currentIndex);
  }
}

bool SemisimpleSubalgebras::centralizersComputedToHaveUnsuitableNilpotentOrbits
() {
  STACK_TRACE(
    "SemisimpleSubalgebras::"
    "centralizersComputedToHaveUnsuitableNilpotentOrbits"
  );
  int stackIndex = this->currentSubalgebraChain.size - 1;
  int typeIndex = this->currentNumberOfLargerTypesExplored[stackIndex];
  DynkinType& currentType =
  this->currentPossibleLargerDynkinTypes[stackIndex][typeIndex];
  SelectionWithDifferentMaxMultiplicities simpleSummandSelection;
  List<int> multiplicities;
  multiplicities.setSize(currentType.size());
  for (int i = 0; i < currentType.size(); i ++) {
    if (
      !currentType.coefficients[i].isSmallInteger(&multiplicities[i])
    ) {
      global.fatal
      << "This is not supposed to happen: "
      << "Dynkin type with multiplicity that doesn't fit in a small int. "
      << global.fatal;
    }
  }
  simpleSummandSelection.initializeFromIntegers(multiplicities);
  DynkinType currentComplementSummand;
  DynkinType centralizerOfComplementOfCurrentSummand;
  DynkinType currentSummand;
  HashedList<Rational> dynkinIndicesCurrentSummand;
  HashedList<Rational> dynkinIndicesCentralizerComplementCurrentSummand;
  while (simpleSummandSelection.incrementReturnFalseIfPastLast()) {
    currentComplementSummand.makeZero();
    for (int i = 0; i < simpleSummandSelection.multiplicities.size; i ++) {
      currentComplementSummand.addMonomial(
        currentType[i], simpleSummandSelection.multiplicities[i]
      );
    }
    if (
      this->getCentralizerTypeIfComputableAndKnown(
        currentComplementSummand, centralizerOfComplementOfCurrentSummand
      )
    ) {
      currentSummand = currentType - currentComplementSummand;
      currentSummand.getDynkinIndicesSl2Subalgebras(
        this->cachedDynkinIndicesSl2subalgebrasSimpleTypes,
        this->cachedDynkinSimpleTypesWithComputedSl2Subalgebras,
        dynkinIndicesCurrentSummand,
        this->ownerField
      );
      centralizerOfComplementOfCurrentSummand.getDynkinIndicesSl2Subalgebras(
        this->cachedDynkinIndicesSl2subalgebrasSimpleTypes,
        this->cachedDynkinSimpleTypesWithComputedSl2Subalgebras,
        dynkinIndicesCentralizerComplementCurrentSummand,
        this->ownerField
      );
      if (
        !dynkinIndicesCentralizerComplementCurrentSummand.contains(
          dynkinIndicesCurrentSummand
        )
      ) {
        std::stringstream reportStream;
        reportStream
        << "<hr>"
        << "I have rejected type "
        << currentType.toString()
        << " as non-realizable for the following reasons. "
        << "I computed that the type's summand "
        << currentSummand.toString()
        << " has complement summand "
        << currentComplementSummand.toString()
        << ". "
        << "Then I computed the latter complement summand has centralizer "
        << centralizerOfComplementOfCurrentSummand.toString()
        << ". "
        << "Then I computed the absolute Dynkin indices of "
        << "the centralizer's sl(2)-subalgebras, namely:<br> "
        << dynkinIndicesCentralizerComplementCurrentSummand.
        toStringCommaDelimited()
        << ". If the type was realizable, those would have to contain "
        << "the absolute Dynkin indices of sl(2) "
        << "subalgebras of the original summand. "
        << "However, that is not the case. "
        << "I can therefore conclude that the Dynkin type "
        << currentType.toString()
        << " is not realizable. "
        << "The absolute Dynkin indices of the sl(2) subalgebras of "
        << "the original summand I computed to be:<br> "
        << dynkinIndicesCurrentSummand.toStringCommaDelimited()
        << ". ";
        this->comments += reportStream.str();
        std::fstream logFile;
        if (
          !FileOperations::openFileCreateIfNotPresentVirtual(
            logFile,
            this->owner->fileNames.virtualFolderName() +
            this->fileNameToLogComments,
            true,
            false,
            false
          )
        ) {
          global.fatal
          << "Failed to open log file: "
          << this->fileNameToLogComments
          << ". This is not fatal but "
          << "I am crashing to let you know. "
          << global.fatal;
        }
        logFile << reportStream.str();
        return true;
      }
    }
  }
  return false;
}

bool CandidateSemisimpleSubalgebra::computeCentralizerTypeFailureAllowed() {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebra::computeCentralizerTypeFailureAllowed"
  );
  this->checkFullInitialization();
  if (this->getRank() != 1) {
    return false;
  }
  Vector<Rational> hElement = this->cartanElementsScaledToActByTwo[0];
  int indexSl2 = - 1;
  bool mustBeTrue =
  this->owner->slTwoSubalgebras.containsSl2WithGivenH(hElement, &indexSl2);
  if (!mustBeTrue) {
    global.fatal
    << "Something went very wrong: the semisimple "
    << "subalgebra is of rank 1, hence an sl(2) subalgebra, yet "
    << "I can't find its H element in the list of sl(2) subalgebras. "
    << global.fatal;
  }
  const SlTwoSubalgebra& sl2 = this->owner->slTwoSubalgebras[indexSl2];
  if (!sl2.flagCentralizerTypeComputed) {
    return false;
  }
  this->centralizerType = sl2.centralizerTypeIfKnown;
  this->flagCentralizerTypeIsComputed = true;
  return true;
}

bool SemisimpleSubalgebras::
centralizerOfBaseComputedToHaveUnsuitableNilpotentOrbits() {
  STACK_TRACE(
    "SemisimpleSubalgebras::"
    "centralizerOfBaseComputedToHaveUnsuitableNilpotentOrbits"
  );
  if (!this->baseSubalgebra().flagCentralizerTypeIsComputed) {
    return false;
  }
  int stackIndex = this->currentSubalgebraChain.size - 1;
  int typeIndex = this->currentNumberOfLargerTypesExplored[stackIndex];
  DynkinType& currentType =
  this->currentPossibleLargerDynkinTypes[stackIndex][typeIndex];
  DynkinType complementNewSummandType =
  this->baseSubalgebra().weylNonEmbedded->dynkinType;
  DynkinType newSummandType = currentType - complementNewSummandType;
  DynkinType centralizerComplementNewSummandType =
  this->baseSubalgebra().centralizerType;
  if (newSummandType.size() != 1) {
    return false;
  }
  HashedList<Rational> dynkinIndicesNewSummand, dynkinIndicesTheyGotToFitIn;
  centralizerComplementNewSummandType.getDynkinIndicesSl2Subalgebras(
    this->cachedDynkinIndicesSl2subalgebrasSimpleTypes,
    this->cachedDynkinSimpleTypesWithComputedSl2Subalgebras,
    dynkinIndicesTheyGotToFitIn,
    this->ownerField
  );
  newSummandType.getDynkinIndicesSl2Subalgebras(
    this->cachedDynkinIndicesSl2subalgebrasSimpleTypes,
    this->cachedDynkinSimpleTypesWithComputedSl2Subalgebras,
    dynkinIndicesNewSummand,
    this->ownerField
  );
  if (dynkinIndicesTheyGotToFitIn.contains(dynkinIndicesNewSummand)) {
    return false;
  }
  std::fstream logFile;
  if (
    !FileOperations::openFileCreateIfNotPresentVirtual(
      logFile,
      this->owner->fileNames.virtualFolderName() + this->fileNameToLogComments,
      true,
      false,
      false
    )
  ) {
    global.fatal
    << "Failed to open log file: "
    << this->fileNameToLogComments
    << ". This is not fatal but "
    << " I am crashing to let you know. "
    << global.fatal;
  }
  std::stringstream reportStream;
  reportStream
  << "<hr>"
  << "I have rejected type "
  << currentType.toString()
  << " as non-realizable for the following reasons. "
  << "The type's summand "
  << newSummandType.toString()
  << " has complement summand "
  << complementNewSummandType.toString()
  << ". "
  << "I computed the latter complement summand has centralizer "
  << centralizerComplementNewSummandType.toString()
  << ". "
  << "Then I computed the absolute Dynkin "
  << "indices of the centralizer's sl(2)-subalgebras, namely:<br> "
  << dynkinIndicesTheyGotToFitIn.toStringCommaDelimited()
  << ". If the type was realizable, those would have to contain "
  << "the absolute Dynkin indices of sl(2) subalgebras "
  << "of the original summand. However, that is not the case. "
  << "I can therefore conclude that the Dynkin type "
  << currentType.toString()
  << " is not realizable. "
  << "The absolute Dynkin indices of the sl(2) subalgebras "
  << "of the original summand I computed to be:<br> "
  << dynkinIndicesNewSummand.toStringCommaDelimited()
  << ". ";
  logFile << reportStream.str();
  global.comments << reportStream.str();
  return true;
}

bool SemisimpleSubalgebras::combinatorialCriteriaAllowRealization() {
  STACK_TRACE("SemisimpleSubalgebras::combinatorialCriteriaAllowRealization");
  int stackIndex = this->currentSubalgebraChain.size - 1;
  int typeIndex = this->currentNumberOfLargerTypesExplored[stackIndex];
  if (
    typeIndex >= this->currentPossibleLargerDynkinTypes[stackIndex].size
  ) {
    return false;
  }
  DynkinType& currentType =
  this->currentPossibleLargerDynkinTypes[stackIndex][typeIndex];
  Rational candidatePrincipalLength =
  currentType.getPrincipalSlTwoCartanSymmetricInverseScale() * 2;
  if (!this->orbitHElementLengths.contains(candidatePrincipalLength)) {
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
  STACK_TRACE("SemisimpleSubalgebras::computeCurrentHCandidates");
  int stackIndex = this->currentSubalgebraChain.size - 1;
  int typeIndex = this->currentNumberOfLargerTypesExplored[stackIndex];
  this->currentHCandidatesScaledToActByTwo[stackIndex].setSize(0);
  if (
    typeIndex >= this->currentPossibleLargerDynkinTypes[stackIndex].size
  ) {
    return true;
  }
  if (
    this->currentPossibleLargerDynkinTypes[stackIndex][typeIndex].
    getRootSystemSize() >
    this->owner->getNumberOfPositiveRoots() * 2
  ) {
    return true;
  }
  if (!this->targetDynkinType.isEqualToZero()) {
    if (
      !this->currentPossibleLargerDynkinTypes[stackIndex][typeIndex].
      canBeExtendedParabolicallyOrIsEqualTo(this->targetDynkinType)
    ) {
      return true;
    }
  }
  if (!this->combinatorialCriteriaAllowRealization()) {
    return true;
  }
  ProgressReport report0;
  ProgressReport report1;
  if (report0.tickAndWantReport()) {
    std::stringstream reportStream;
    reportStream
    << " Finding h-candidates for extension of "
    << this->baseSubalgebra().weylNonEmbedded->dynkinType.toString()
    << " to "
    << this->currentPossibleLargerDynkinTypes[stackIndex][typeIndex].toString()
    << ": "
    << typeIndex + 1
    << " out of "
    << this->currentPossibleLargerDynkinTypes[stackIndex].size
    << " possibilities. ";
    report0.report(reportStream.str());
  }
  CandidateSemisimpleSubalgebra newCandidate;
  newCandidate.owner = this;
  if (this->baseSubalgebra().getRank() != 0) {
    Vector<Rational> weightHElementWeAreLookingFor =
    this->
    getHighestWeightFundamentalNewComponentFromImagesOldSimpleRootsAndNewRoot(
      this->currentPossibleLargerDynkinTypes[stackIndex][typeIndex],
      this->currentRootInjections[stackIndex][typeIndex],
      newCandidate
    );
    List<int> indicesModulesNewComponentExtensionMod;
    indicesModulesNewComponentExtensionMod.reserve(
      this->owner->weylGroup.rootSystem.size
    );
    indicesModulesNewComponentExtensionMod.setSize(0);
    for (
      int j = 0; j < this->baseSubalgebra().highestWeightsNonPrimal.size; j ++
    ) {
      if (
        this->baseSubalgebra().highestWeightsNonPrimal[j] ==
        weightHElementWeAreLookingFor
      ) {
        indicesModulesNewComponentExtensionMod.addOnTop(j);
      }
    }
    if (indicesModulesNewComponentExtensionMod.size == 0) {
      if (report1.tickAndWantReport()) {
        std::stringstream reportStream;
        reportStream
        << " Extension "
        << typeIndex + 1
        << " out of "
        << this->currentPossibleLargerDynkinTypes[stackIndex].size
        << ", type  "
        << this->currentPossibleLargerDynkinTypes[stackIndex][typeIndex].
        toString()
        << " cannot be realized: no appropriate module: "
        << "desired weight of h element is: "
        << weightHElementWeAreLookingFor.toString()
        << " but the highest weights of the base candidate are: "
        << this->baseSubalgebra().highestWeightsNonPrimal.toString();
        report1.report(reportStream.str());
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
    newCandidate,
    this->currentPossibleLargerDynkinTypes[stackIndex][typeIndex],
    this->currentRootInjections[stackIndex][typeIndex]
  );
  return true;
}

CandidateSemisimpleSubalgebra* SemisimpleSubalgebras::
addSubalgebraIfNewSetToStackTop(const CandidateSemisimpleSubalgebra& input) {
  STACK_TRACE("SemisimpleSubalgebras::addSubalgebraIfNewSetToStackTop");
  this->checkConsistencyHs();
  CandidateSemisimpleSubalgebra* storedInput = nullptr;
  if (this->subalgebras.contains(input.cartanElementsSubalgebra)) {
    storedInput =
    &this->subalgebras.values[
      this->subalgebras.getIndex(input.cartanElementsSubalgebra)
    ];
    if (
      !storedInput->weylNonEmbedded->dynkinType.isEqualToZero() &&
      storedInput->indexInOwner == - 1
    ) {
      global.fatal
      << "This is not supposed to happen: subalgebra of type "
      << storedInput->weylNonEmbedded->dynkinType.toString()
      << " has index in owner - 1. "
      << global.fatal;
    }
  } else {
    int indexInOwner = this->subalgebras.values.size;
    this->subalgebras.setKeyValue(input.cartanElementsSubalgebra, input);
    storedInput = &this->subalgebras.values[indexInOwner];
    storedInput->indexInOwner = indexInOwner;
  }
  storedInput->computeAndVerifyFromGeneratorsAndHs();
  this->addSubalgebraToStack(*storedInput, 0, 0);
  return storedInput;
}

void SemisimpleSubalgebras::addSubalgebraToStack(
  CandidateSemisimpleSubalgebra& input,
  int inputNumberOfLargerTypesExplored,
  int inputNumberOfHCandidatesExplored
) {
  STACK_TRACE("SemisimpleSubalgebras::addSubalgebraToStack");
  input.checkFullInitialization();
  if (
    input.indexInOwner == - 1 &&
    !input.weylNonEmbedded->dynkinType.isEqualToZero()
  ) {
    global.fatal
    << "Adding to stack subalgebra with "
    << "indexInOwner equal to - 1 is forbidden. "
    << global.fatal;
  }
  if (
    input.cartanElementsSubalgebra.size !=
    input.hsScaledToActByTwoInOrderOfCreation.size
  ) {
    global.fatal
    << "In order to add subalgebra "
    << input.weylNonEmbedded->dynkinType.toString()
    << " to the stack I need to know the order of creation of its h-vectors. "
    << global.fatal;
  }
  if (input.indexInOwner != - 1) {
    if (&input != &this->subalgebras.values[input.indexInOwner]) {
      global.fatal
      << "A non-zero subalgebra pushed to the stack "
      << "must be owned by the current collection of semisimple subalgebras. "
      << global.fatal;
    }
  }
  input.computeCentralizerTypeFailureAllowed();
  // <- trying to compute the centralizer of a subalgebra on the stack.
  this->currentSubalgebraChain.addOnTop(input);
  this->currentPossibleLargerDynkinTypes.setSize(
    this->currentSubalgebraChain.size
  );
  this->currentRootInjections.setSize(this->currentSubalgebraChain.size);
  this->growDynkinType(
    input.weylNonEmbedded->dynkinType,
    *this->currentPossibleLargerDynkinTypes.lastObject(),
    this->currentRootInjections.lastObject()
  );
  // /////////
  this->currentNumberOfLargerTypesExplored.addOnTop(
    inputNumberOfLargerTypesExplored
  );
  // global.Comments << "<hr>" <<
  // this->currentPossibleLargerDynkinTypes.lastObject()->size
  // << " possible extensions of " <<
  // input.weylNonEmbedded->dynkinType.toString() << ": ";
  // for (int i = 0; i <
  // this->currentPossibleLargerDynkinTypes.lastObject()->size; i ++)
  // global.Comments <<
  // (*this->currentPossibleLargerDynkinTypes.lastObject())[i].toString() << ",
  // ";
  // /////////
  this->currentHCandidatesScaledToActByTwo.setSize(
    this->currentSubalgebraChain.size
  );
  this->currentNumberOfHCandidatesExplored.addOnTop(
    inputNumberOfHCandidatesExplored
  );
  this->computeCurrentHCandidates();
}

std::string SemisimpleSubalgebras::toStringCurrentChain(
  FormatExpressions* format
) {
  STACK_TRACE("SemisimpleSubalgebras::ToStringCurrentChain");
  (void) format;
  // avoid unused parameter warning in a portable way
  std::stringstream out;
  out
  << "<br>Current subalgebra chain length: "
  << this->currentSubalgebraChain.size
  << "<br>";
  for (int i = 0; i < this->currentSubalgebraChain.size; i ++) {
    out
    << this->currentSubalgebraChain[i].weylNonEmbedded->dynkinType.toString();
    if (i != this->currentSubalgebraChain.size - 1) {
      out << "&lt;";
    }
  }
  return out.str();
}

std::string SemisimpleSubalgebras::toStringState(FormatExpressions* format) {
  STACK_TRACE("SemisimpleSubalgebras::toStringState");
  std::stringstream out;
  out
  << "Subalgebras found so far: "
  << this->subalgebras.values.size
  << "<br>Orbit sizes: ";
  for (int i = 0; i < this->orbits.size; i ++) {
    out
    << "A^"
    << (this->orbitHElementLengths[i] / 2).toString()
    << "_1: "
    << this->orbits[i].toStringSize();
  }
  out << this->toStringCurrentChain(format);
  out << "<hr>";
  for (int i = 0; i < this->currentSubalgebraChain.size; i ++) {
    out
    << "<br>Extensions of "
    << this->currentSubalgebraChain[i].weylNonEmbedded->dynkinType.toString()
    << ": &nbsp;&nbsp;&nbsp;&nbsp;";
    for (
      int j = 0; j < this->currentPossibleLargerDynkinTypes[i].size; j ++
    ) {
      if (j == this->currentNumberOfLargerTypesExplored[i]) {
        out << "<b>";
      }
      out << this->currentPossibleLargerDynkinTypes[i][j].toString();
      if (j == this->currentNumberOfLargerTypesExplored[i]) {
        out << "</b>";
      }
      if (j != this->currentPossibleLargerDynkinTypes[i].size - 1) {
        out << ", ";
      }
    }
    out
    << "<br> "
    << this->currentNumberOfLargerTypesExplored[i]
    << " out of "
    << this->currentPossibleLargerDynkinTypes[i].size
    << " types explored, current type: ";
    if (
      this->currentNumberOfLargerTypesExplored[i] < this->
      currentPossibleLargerDynkinTypes[i].size
    ) {
      out
      << "<b>"
      << this->currentPossibleLargerDynkinTypes[i][
        this->currentNumberOfLargerTypesExplored[i]
      ]
      << "</b>";
    } else {
      out << "<b>All types explored.</b>";
    }
    out
    << "<br>Explored "
    << this->currentNumberOfHCandidatesExplored[i]
    << " out of "
    << this->currentHCandidatesScaledToActByTwo[i].size
    << " h-candidates. ";
  }
  return out.str();
}

std::string SemisimpleSubalgebras::toStringProgressReport(
  FormatExpressions* format
) {
  STACK_TRACE("SemisimpleSubalgebras::toStringProgressReport");
  std::stringstream out;
  out << this->toStringState(format);
  if (this->toStringExpressionString != nullptr) {
    out
    << "\n<hr>\n"
    << "LoadSemisimpleSubalgebras {}"
    << this->toStringExpressionString(*this);
  }
  out << "\n\n<hr>";
  return out.str();
}

bool SemisimpleSubalgebras::incrementReturnFalseIfPastLast() {
  STACK_TRACE("SemisimpleSubalgebras::incrementReturnFalseIfPastLast");
  this->checkConsistency();
  if (this->currentSubalgebraChain.size == 0) {
    return false;
  }
  if (
    this->baseSubalgebra().cartanElementsSubalgebra.size !=
    this->baseSubalgebra().hsScaledToActByTwoInOrderOfCreation.size
  ) {
    global.fatal
    << " The order of creation of the elements "
    << "of the Cartan is missing in base subalgebra of type "
    << this->baseSubalgebra().weylNonEmbedded->dynkinType.toString()
    << ", and this order is needed to construct extensions. "
    << global.fatal;
  }
  if (this->baseSubalgebra().getRank() >= this->owner->getRank()) {
    return this->removeLastSubalgebraFromStack();
  }
  int stackIndex = this->currentSubalgebraChain.size - 1;
  if (
    this->currentNumberOfHCandidatesExplored[stackIndex] >=
    this->currentHCandidatesScaledToActByTwo[stackIndex].size
  ) {
    this->currentNumberOfLargerTypesExplored[stackIndex] ++;
    if (
      this->currentNumberOfLargerTypesExplored[stackIndex] >=
      this->currentPossibleLargerDynkinTypes[stackIndex].size
    ) {
      return this->removeLastSubalgebraFromStack();
    }
    this->currentNumberOfHCandidatesExplored[stackIndex] = 0;
    return this->computeCurrentHCandidates();
  }
  int typeIndex = this->currentNumberOfLargerTypesExplored[stackIndex];
  int hIndex = this->currentNumberOfHCandidatesExplored[stackIndex];
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
    if (
      !newCandidate.flagSystemSolved &&
      !newCandidate.flagSystemProvedToHaveNoSolution
    ) {
      this->flagRealizedAllCandidates = false;
      std::stringstream out;
      out
      << "<hr>Failed to realize type "
      << newCandidate.weylNonEmbedded->dynkinType.toString()
      << " because I couldn't handle the polynomial system. "
      << "one poly system that governs the embedding follows.<br>"
      << newCandidate.toStringSystemPart2()
      << "<hr>";
      this->comments = out.str();
      return true;
    }
  }
  this->currentNumberOfHCandidatesExplored[stackIndex] ++;
  ProgressReport report1;
  if (newSubalgebraCreated) {
    this->addSubalgebraIfNewSetToStackTop(newCandidate);
  } else {
    std::stringstream reportstream;
    reportstream
    << "h element "
    << hIndex + 1
    << " out of "
    << this->currentHCandidatesScaledToActByTwo[stackIndex][hIndex].size
    << ": did not succeed extending. ";
    report1.report(reportstream.str());
  }
  return true;
}

bool DynkinType::isTypeAOne() const {
  if (this->size() != 1) {
    return false;
  }
  if (this->coefficients[0] != 1) {
    return false;
  }
  if ((*this)[0].rank != 1) {
    return false;
  }
  return (*this)[0].letter == 'A';
}

void DynkinType::getDynkinTypeWithDefaultScales(DynkinType& output) const {
  if (&output == this) {
    DynkinType thisCopy = *this;
    thisCopy.getDynkinTypeWithDefaultScales(output);
    return;
  }
  output.makeZero();
  DynkinSimpleType currentType;
  for (int i = 0; i < this->size(); i ++) {
    currentType.makeArbitrary((*this)[i].letter, (*this)[i].rank, 1
    );
    output.addMonomial(currentType, this->coefficients[i]);
  }
}

void DynkinSimpleType::getAutomorphismActingOnVectorColumn(
  MatrixTensor<Rational>& output, int autoIndex
) const {
  STACK_TRACE("DynkinSimpleType::getAutomorphismActingOnVectorColumn");
  if (
    autoIndex == 0 ||
    this->letter == 'B' ||
    this->letter == 'C' ||
    this->letter == 'G' ||
    this->letter == 'F' ||
    this->rank == 1 || (this->letter == 'E' && this->rank != 6)
  ) {
    output.makeIdentity(this->rank);
    return;
  }
  output.makeZero();
  if (this->letter == 'A' && this->rank != 1) {
    for (int i = 0; i < this->rank; i ++) {
      output.addMonomial(MonomialMatrix(i, this->rank - i - 1), 1);
    }
  }
  if (this->letter == 'D') {
    if (this->rank == 4) {
      // Here be D_4 triality.
      // The automorphism group of Dynkin Diagram D_4 is S3
      // The triple node is always fixed:
      output.addMonomial(MonomialMatrix(1, 1), 1);
      if (autoIndex == 1) {
        // Permutation (12), AutoIndex = 1
        output.addMonomial(MonomialMatrix(0, 2), 1);
        output.addMonomial(MonomialMatrix(2, 0), 1);
        output.addMonomial(MonomialMatrix(3, 3), 1);
      } else if (autoIndex == 2) {
        // Permutation (23), AutoIndex =2
        output.addMonomial(MonomialMatrix(0, 0), 1);
        output.addMonomial(MonomialMatrix(2, 3), 1);
        output.addMonomial(MonomialMatrix(3, 2), 1);
      } else if (autoIndex == 3) {
        // Permutation (12)(23)=(123), AutoIndex =3
        output.addMonomial(MonomialMatrix(0, 2), 1);
        output.addMonomial(MonomialMatrix(2, 3), 1);
        output.addMonomial(MonomialMatrix(3, 0), 1);
      } else if (autoIndex == 4) {
        // Permutation (23)(12)=(132), AutoIndex =4
        output.addMonomial(MonomialMatrix(0, 3), 1);
        output.addMonomial(MonomialMatrix(2, 0), 1);
        output.addMonomial(MonomialMatrix(3, 2), 1);
      } else if (autoIndex == 5) {
        // Permutation (12)(23)(12)=(13), AutoIndex =5
        output.addMonomial(MonomialMatrix(0, 3), 1);
        output.addMonomial(MonomialMatrix(2, 2), 1);
        output.addMonomial(MonomialMatrix(3, 0), 1);
      } else {
        global.fatal
        << "Requesting triality "
        << "automorphism with index not in the range 0-5. "
        << global.fatal;
      }
    } else {
      for (int i = 0; i < this->rank - 2; i ++) {
        output.addMonomial(MonomialMatrix(i, i), 1);
      }
      output.addMonomial(
        MonomialMatrix(this->rank - 1, this->rank - 2), 1
      );
      output.addMonomial(
        MonomialMatrix(this->rank - 2, this->rank - 1), 1
      );
    }
  }
  if (this->letter == 'E' && this->rank == 6) {
    // We gotta flip the Dynkin diagram of E_6.
    // Note that the extra 1-length sticking root comes second and
    // and the triple node comes fourth.
    // Therefore, we must swap the 1st root with the 6th and the third root
    // with the 5th. Conventions, conventions... no way around 'em!
    output.addMonomial(MonomialMatrix(1, 1), 1);
    output.addMonomial(MonomialMatrix(3, 3), 1);
    output.addMonomial(MonomialMatrix(0, 5), 1);
    output.addMonomial(MonomialMatrix(5, 0), 1);
    output.addMonomial(MonomialMatrix(2, 4), 1);
    output.addMonomial(MonomialMatrix(4, 2), 1);
  }
  Rational determinant = output.getDeterminant();
  if (determinant != 1 && determinant != - 1) {
    FormatExpressions format;
    format.flagUseHTML = false;
    format.flagUseLatex = true;
    global.fatal
    << "The determinant of the automorphism matrix "
    << "of the Dynkin graph must be +/- 1, it is instead "
    << determinant.toString()
    << ". The auto matrix is: "
    << HtmlRoutines::getMathNoDisplay(output.toStringMatrixForm(&format))
    << " and the dynkin type is: "
    << this->toString()
    << "."
    << global.fatal;
  }
}

DynkinSimpleType DynkinType::getSmallestSimpleType() const {
  if (this->size() == 0) {
    global.fatal
    << "Asking for the smallest simple type of a 0 dynkin type. "
    << global.fatal;
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
    global.fatal
    << "Asking for the greatest simple type of a 0 dynkin type. "
    << global.fatal;
  }
  DynkinSimpleType result = (*this)[0];
  for (int i = 1; i < this->size(); i ++) {
    if ((*this)[i] > result) {
      result = (*this)[i];
    }
  }
  return result;
}

Rational DynkinType::getPrincipalSlTwoCartanSymmetricInverseScale() const {
  STACK_TRACE("DynkinType::getPrincipalSlTwoCartanSymmetricInverseScale");
  Rational result = 0;
  for (int i = 0; i < this->size(); i ++) {
    result +=
    this->coefficients[i] *(*this)[i].
    getPrincipalSlTwoCartanSymmetricInverseScale();
  }
  return result;
}

bool CandidateSemisimpleSubalgebra::checkElementSemisimpleLieAlgebra(
  const ElementSemisimpleLieAlgebra<AlgebraicNumber>& toBeChecked
) {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebra"
    "::checkElementSemisimpleLieAlgebra"
  );
  for (int i = 0; i < toBeChecked.size(); i ++) {
    toBeChecked.coefficients[i].checkConsistency();
  }
  return true;
}

bool CandidateSemisimpleSubalgebra::checkAll() const {
  STACK_TRACE("CandidateSemisimpleSubalgebra::checkAll");
  this->checkFullInitialization();
  this->checkCandidateInitialization();
  for (int i = 0; i < this->negativeGenerators.size; i ++) {
    this->checkElementSemisimpleLieAlgebra(this->negativeGenerators[i]);
  }
  for (int i = 0; i < this->positiveGenerators.size; i ++) {
    this->checkElementSemisimpleLieAlgebra(this->positiveGenerators[i]);
  }
  return true;
}

bool CandidateSemisimpleSubalgebra::checkBasicInitialization() const {
  if (this->flagDeallocated) {
    global.fatal
    << "Use after free of CandidateSemisimpleSubalgebra. "
    << global.fatal;
  }
  if (this->owner == nullptr) {
    global.fatal
    << "Use of non-initialized semisimple subalgebra candidate. "
    << global.fatal;
  }
  this->owner->checkInitialization();
  return true;
}

bool CandidateSemisimpleSubalgebra::checkCandidateInitialization() const {
  this->checkBasicInitialization();
  if (this->weylNonEmbedded == nullptr) {
    global.fatal
    << "Weyl group data not initialized for "
    << "a semisimple subalgebra candidate. "
    << global.fatal;
  }
  return true;
}

bool CandidateSemisimpleSubalgebra::checkFullInitialization() const {
  this->checkCandidateInitialization();
  if (this->subalgebraNonEmbeddedDefaultScale == nullptr) {
    global.fatal
    << "The semisimple default scale subalgebra is not initialized. "
    << global.fatal;
  }
  return true;
}

WeylGroupAutomorphisms& CandidateSemisimpleSubalgebra::
getAmbientWeylAutomorphisms() const {
  this->checkBasicInitialization();
  return this->owner->slTwoSubalgebras.rootSubalgebras.weylGroupAutomorphisms;
}

WeylGroupData& CandidateSemisimpleSubalgebra::getAmbientWeyl() const {
  this->checkBasicInitialization();
  return this->owner->getSemisimpleOwner().weylGroup;
}

SemisimpleLieAlgebra& CandidateSemisimpleSubalgebra::
getAmbientSemisimpleLieAlgebra() const {
  this->checkBasicInitialization();
  return this->owner->getSemisimpleOwner();
}

void CandidateSemisimpleSubalgebra::addHIncomplete(
  const Vector<Rational>& hElement
) {
  STACK_TRACE("CandidateSemisimpleSubalgebra::addHIncomplete");
  this->checkBasicInitialization();
  if (this->cartanSubalgebrasByComponentScaledToActByTwo.size == 1) {
    if (
      this->cartanSubalgebrasByComponentScaledToActByTwo.lastObject()->size ==
      0
    ) {
      this->posistiveRootsPerpendicularToPrecedingWeights =
      this->getAmbientWeyl().rootsOfBorel;
    }
  }
  for (
    int i = 0; i < this->posistiveRootsPerpendicularToPrecedingWeights.size; i
    ++
  ) {
    if (
      this->getAmbientWeyl().rootScalarCartanRoot(
        this->posistiveRootsPerpendicularToPrecedingWeights[i], hElement
      ) !=
      0
    ) {
      this->posistiveRootsPerpendicularToPrecedingWeights.removeIndexShiftDown(
        i
      );
      i --;
    }
  }
  this->cartanSubalgebrasByComponentScaledToActByTwo.lastObject()->addOnTop(
    hElement
  );
}

bool CandidateSemisimpleSubalgebra::isGoodHNewActingByTwo(
  const Vector<Rational>& hNewActingByTwo,
  const List<int>& rootInjections
) const {
  STACK_TRACE("CandidateSemisimpleSubalgebra::isGoodHNewActingByTwo");
  // check if input weight is maximally dominant:
  Rational scalarProduct;
  int indexHneW = *rootInjections.lastObject();
  for (int i = 0; i < this->getAmbientWeyl().rootsOfBorel.size; i ++) {
    Vector<Rational>& currentPositiveRoot =
    this->getAmbientWeyl().rootsOfBorel[i];
    bool canBeRaisingReflection = true;
    for (
      int l = 0; l < this->hsScaledToActByTwoInOrderOfCreation.size &&
      canBeRaisingReflection; l ++
    ) {
      scalarProduct =
      this->getAmbientWeyl().rootScalarCartanRoot(
        currentPositiveRoot, this->hsScaledToActByTwoInOrderOfCreation[l]
      );
      if (scalarProduct > 0) {
        canBeRaisingReflection = false;
      }
      if (scalarProduct < 0) {
        global.fatal
        << "While trying to realize type "
        << this->weylNonEmbedded->dynkinType.toString()
        << ", the candidate h elements of the semisimple "
        << "subalgebra are supposed to be maximally dominant, "
        << "however the scalar product of the positive root "
        << currentPositiveRoot.toString()
        << " with the subalgebra root "
        << this->hsScaledToActByTwoInOrderOfCreation[l].toString()
        << " is negative, while the very same positive "
        << "root has had zero scalar products with all "
        << "preceding roots. Hnew equals: "
        << hNewActingByTwo.toString()
        << " Here are all preceding roots: "
        << this->hsScaledToActByTwoInOrderOfCreation.toString()
        << global.fatal;
      }
    }
    if (canBeRaisingReflection) {
      if (
        this->getAmbientWeyl().rootScalarCartanRoot(
          currentPositiveRoot, hNewActingByTwo
        ) <
        0
      ) {
        return false;
      }
    }
  }
  for (int i = 0; i < this->cartanElementsScaledToActByTwo.size; i ++) {
    if (i != indexHneW) {
      scalarProduct =
      this->getAmbientWeyl().rootScalarCartanRoot(
        hNewActingByTwo, this->cartanElementsScaledToActByTwo[i]
      );
    } else {
      scalarProduct =
      this->getAmbientWeyl().rootScalarCartanRoot(
        hNewActingByTwo, hNewActingByTwo
      );
    }
    if (
      scalarProduct != this->weylNonEmbedded->coCartanSymmetric(indexHneW, i)
    ) {
      return false;
    }
  }
  return true;
}

template <class Coefficient>
int CharacterSemisimpleLieAlgebraModule<Coefficient>::
getIndexExtremeWeightRelativeToWeyl(WeylGroupData& weyl) const {
  HashedList<Vector<Coefficient> > weightsSimpleCoordinates;
  weightsSimpleCoordinates.setExpectedSize(this->size());
  for (int i = 0; i < this->size(); i ++) {
    weightsSimpleCoordinates.addOnTop(
      weyl.getSimpleCoordinatesFromFundamental((*this)[i].
        weightFundamentalCoordinates
      )
    );
  }
  for (int i = 0; i < weightsSimpleCoordinates.size; i ++) {
    bool isGood = true;
    for (int j = 0; j < weyl.rootsOfBorel.size; j ++) {
      if (
        weightsSimpleCoordinates.contains((
            weightsSimpleCoordinates[i] + weyl.rootsOfBorel[j]
          )
        )
      ) {
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

bool CandidateSemisimpleSubalgebra::isWeightSystemSpaceIndex(
  int index, const Vector<Rational>& ambientRootTestedForWeightSpace
) {
  STACK_TRACE("CandidateSemisimpleSubalgebra::isWeightSystemSpaceIndex");
  for (int k = 0; k < this->cartanElementsSubalgebra.size; k ++) {
    Rational desiredScalarProduct =
    this->subalgebraNonEmbeddedDefaultScale->weylGroup.cartanSymmetric(
      index, k
    );
    Rational actualScalar =
    this->getAmbientWeyl().rootScalarCartanRoot(
      this->cartanElementsSubalgebra[k], ambientRootTestedForWeightSpace
    );
    if (desiredScalarProduct != actualScalar) {
      return false;
    }
  }
  return true;
}

bool CandidateSemisimpleSubalgebra::computeSystem(
  bool attemptToChooseCentalizer, bool allowNonPolynomialSystemFailure
) {
  STACK_TRACE("CandidateSemisimpleSubalgebra::computeSystem");
  ChevalleyGenerator currentGen, currentOpGen;
  this->involvedNegativeGenerators.setSize(
    this->cartanElementsScaledToActByTwo.size
  );
  this->involvedPositiveGenerators.setSize(
    this->cartanElementsScaledToActByTwo.size
  );
  for (int i = 0; i < this->cartanElementsScaledToActByTwo.size; i ++) {
    List<ChevalleyGenerator>& currentInvolvedPositiveGenerators =
    this->involvedPositiveGenerators[i];
    List<ChevalleyGenerator>& currentInvolvedNegativeGenerators =
    this->involvedNegativeGenerators[i];
    currentInvolvedNegativeGenerators.setExpectedSize(
      this->getAmbientWeyl().getDimension() * 2
    );
    currentInvolvedPositiveGenerators.setExpectedSize(
      this->getAmbientWeyl().getDimension() * 2
    );
    currentInvolvedNegativeGenerators.setSize(0);
    currentInvolvedPositiveGenerators.setSize(0);
    for (int j = 0; j < this->getAmbientWeyl().rootSystem.size; j ++) {
      int indexCurrentGenerator =
      this->getAmbientSemisimpleLieAlgebra().getGeneratorFromRootIndex(j);
      int opIndex =
      this->getAmbientSemisimpleLieAlgebra().getGeneratorIndexFromRoot(
        - this->getAmbientWeyl().rootSystem[j]
      );
      currentGen.makeGenerator(
        *this->owner->owner, indexCurrentGenerator
      );
      currentOpGen.makeGenerator(*this->owner->owner, opIndex);
      if (
        this->isWeightSystemSpaceIndex(
          i, this->getAmbientWeyl().rootSystem[j]
        )
      ) {
        currentInvolvedPositiveGenerators.addOnTop(currentGen);
        currentInvolvedNegativeGenerators.addOnTop(currentOpGen);
      }
    }
    if (currentInvolvedNegativeGenerators.size == 0) {
      if (currentInvolvedPositiveGenerators.size != 0) {
        global.fatal
        << "The number of involved negative generators is different "
        << "from the number of involved positive generators. "
        << global.fatal;
      }
      this->flagSystemProvedToHaveNoSolution = true;
      return false;
    }
  }
  this->flagUsedInducingSubalgebraRealization = true;
  return
  this->computeSystemPart2(
    attemptToChooseCentalizer, allowNonPolynomialSystemFailure
  );
}

bool CandidateSemisimpleSubalgebra::checkGeneratorsBracketToHs() {
  STACK_TRACE("CandidateSemisimpleSubalgebra::checkGeneratorsBracketToHs");
  if (
    this->negativeGenerators.size != this->positiveGenerators.size ||
    this->negativeGenerators.size != this->weylNonEmbedded->getDimension()
  ) {
    return false;
  }
  ElementSemisimpleLieAlgebra<AlgebraicNumber> goalH, lieBracket;
  for (int i = 0; i < this->negativeGenerators.size; i ++) {
    goalH.makeCartanGenerator(
      this->cartanElementsScaledToActByTwo[i], *this->owner->owner
    );
    this->getAmbientSemisimpleLieAlgebra().lieBracket(
      this->positiveGenerators[i],
      this->negativeGenerators[i],
      lieBracket
    );
    lieBracket -= goalH;
    if (!lieBracket.isEqualToZero()) {
      return false;
    }
  }
  return true;
}

bool CandidateSemisimpleSubalgebra::computeSystemPart2(
  bool attemptToChooseCentalizer, bool allowNonPolynomialSystemFailure
) {
  STACK_TRACE("CandidateSemisimpleSubalgebra::computeSystemPart2");
  systemToSolve.setSize(0);
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >
  lieBracketMinusGoalValue;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > goalValue;
  Vector<Polynomial<Rational> > desiredHpart;
  this->checkFullInitialization();
  const SemisimpleLieAlgebra& nonEmbeddedMe =
  this->owner->subalgebrasNonDefaultCartanAndScale.values[
    this->indexNonEmbeddedMeNonStandardCartan
  ];
  this->totalUnknownsNoCentralizer = 0;
  if (this->cartanElementsSubalgebra.size == 0) {
    global.fatal
    << "The number of involved H's cannot be zero. "
    << global.fatal;
  }
  if (
    this->involvedNegativeGenerators.size !=
    this->cartanElementsSubalgebra.size
  ) {
    global.fatal
    << "The number of involved negative generators: "
    << this->involvedNegativeGenerators.size
    << " is not equal to the subalgebra rank: "
    << this->cartanElementsSubalgebra.size
    << ". "
    << global.fatal;
  }
  for (int i = 0; i < this->involvedNegativeGenerators.size; i ++) {
    this->totalUnknownsNoCentralizer +=
    this->involvedNegativeGenerators[i].size;
  }
  if (this->weylNonEmbedded->rootSystem.size == 0) {
    global.fatal
    << "The root system of the candidate subalgebra has not been computed "
    << global.fatal;
  }
  this->totalUnknownsNoCentralizer *= 2;
  this->totalUnknownsWithCentralizer = this->totalUnknownsNoCentralizer;
  this->unknownNegativeGenerators.setSize(
    this->involvedNegativeGenerators.size
  );
  this->unknownPositiveGenerators.setSize(
    this->involvedPositiveGenerators.size
  );
  if (!attemptToChooseCentalizer) {
    this->unknownCartanCentralizerBasis.setSize(0);
  } else {
    int rankCentralizer = - 1;
    bool tempB = this->centralizerRank.isSmallInteger(&rankCentralizer);
    if (
      !tempB ||
      rankCentralizer < 0 ||
      rankCentralizer > this->getAmbientWeyl().getDimension()
    ) {
      if (allowNonPolynomialSystemFailure) {
        return false;
      }
    }
    if (!tempB) {
      global.fatal
      << "Error: rankCentralizer is not a small integer. "
      << "Detailed subalgebra printout: "
      << this->toString(nullptr, false)
      << global.fatal;
    }
    if (rankCentralizer < 0) {
      global.fatal
      << "Centralizer rank extracted as a negative number. "
      << "The centralizer rank is: "
      << this->centralizerRank.toString()
      << ". This most probably means the "
      << "centralizer was not computed correctly. "
      << "Here's a full subalgebra printout"
      << this->toString(nullptr, false)
      << global.fatal;
    }
    if (rankCentralizer > this->getAmbientWeyl().getDimension()) {
      global.fatal
      << " Currently rankCentralizer is computed to be "
      << rankCentralizer
      << " which is greater than the rank "
      << this->getAmbientWeyl().getDimension()
      << " of the ambient semisimple Lie algebra. Something has gone wrong. "
      << "Here is a detailed printout of the candidate subalgebra. "
      << this->toString(nullptr, false)
      << global.fatal;
    }
    this->totalUnknownsWithCentralizer +=
    rankCentralizer * this->getAmbientWeyl().getDimension() + 1;
    this->unknownCartanCentralizerBasis.setSize(rankCentralizer);
  }
  if (this->indexInducedFrom == - 1) {
    this->flagUsedInducingSubalgebraRealization = false;
  }
  int indexNewRoot = - 1;
  if (this->flagUsedInducingSubalgebraRealization) {
    indexNewRoot = *this->rootInjectionsFromInducer.lastObject();
  }
  for (int i = 0; i < this->involvedNegativeGenerators.size; i ++) {
    bool seedsHaveBeenSown = false;
    if (this->flagUsedInducingSubalgebraRealization) {
      CandidateSemisimpleSubalgebra& inducer =
      this->owner->subalgebras.values[this->indexInducedFrom];
      if (inducer.flagSystemSolved && i != indexNewRoot) {
        int preimageIndex =
        DynkinType::getIndexPreimageFromRootInjection(
          i, this->rootInjectionsFromInducer
        );
        this->unknownNegativeGenerators[i] =
        inducer.negativeGenerators[preimageIndex];
        // <-implicit type conversion from base field to polynomial here
        this->unknownPositiveGenerators[i] =
        inducer.positiveGenerators[preimageIndex];
        // <-implicit type conversion from base field to polynomial here
        seedsHaveBeenSown = true;
      }
    }
    if (seedsHaveBeenSown) {
      continue;
    }
    this->getGenericNegativeGeneratorLinearCombination(
      i, this->unknownNegativeGenerators[i]
    );
    this->getGenericPositiveGeneratorLinearCombination(
      i, this->unknownPositiveGenerators[i]
    );
  }
  if (this->unknownCartanCentralizerBasis.size > 0) {
    Matrix<Polynomial<AlgebraicNumber> > centralizerCartanVars;
    Vectors<Polynomial<AlgebraicNumber> > centralizerCartanElements;
    centralizerCartanElements.setSize(
      this->unknownCartanCentralizerBasis.size
    );
    for (int i = 0; i < this->unknownCartanCentralizerBasis.size; i ++) {
      this->getGenericCartanCentralizerLinearCombination(
        i, this->unknownCartanCentralizerBasis[i]
      );
      centralizerCartanElements[i] =
      this->unknownCartanCentralizerBasis[i].getCartanPart();
    }
    centralizerCartanElements.getGramMatrix(
      centralizerCartanVars, &this->getAmbientWeyl().cartanSymmetric
    );
    Polynomial<AlgebraicNumber> determinant;
    Polynomial<AlgebraicNumber> determinantMultiplier;
    determinant.makeDeterminantFromSquareMatrix(centralizerCartanVars);
    determinantMultiplier.makeMonomial(
      this->totalUnknownsWithCentralizer - 1, 1, 1
    );
    determinant *= determinantMultiplier;
    determinant += - Rational(1);
    this->systemToSolve.addOnTop(determinant);
  }
  if (
    this->unknownNegativeGenerators.size !=
    this->unknownPositiveGenerators.size
  ) {
    global.fatal
    << "Error: number of unknown negative generators "
    << "differs from number of unknown positive ones. "
    << global.fatal;
  }
  for (int i = 0; i < this->unknownNegativeGenerators.size; i ++) {
    desiredHpart = this->cartanElementsScaledToActByTwo[i];
    // <-implicit type conversion here!
    goalValue.makeCartanGenerator(desiredHpart, *this->owner->owner);
    this->getAmbientSemisimpleLieAlgebra().lieBracket(
      this->unknownPositiveGenerators[i],
      this->unknownNegativeGenerators[i],
      lieBracketMinusGoalValue
    );
    lieBracketMinusGoalValue -= goalValue;
    this->addToSystem(lieBracketMinusGoalValue);
    for (int j = 0; j < this->unknownCartanCentralizerBasis.size; j ++) {
      this->getAmbientSemisimpleLieAlgebra().lieBracket(
        this->unknownNegativeGenerators[i],
        this->unknownCartanCentralizerBasis[j],
        lieBracketMinusGoalValue
      );
      this->addToSystem(lieBracketMinusGoalValue);
      this->getAmbientSemisimpleLieAlgebra().lieBracket(
        this->unknownPositiveGenerators[i],
        this->unknownCartanCentralizerBasis[j],
        lieBracketMinusGoalValue
      );
      this->addToSystem(lieBracketMinusGoalValue);
    }
    for (int j = 0; j < this->unknownPositiveGenerators.size; j ++) {
      if (i != j) {
        this->getAmbientSemisimpleLieAlgebra().lieBracket(
          this->unknownNegativeGenerators[i],
          this->unknownPositiveGenerators[j],
          lieBracketMinusGoalValue
        );
        this->addToSystem(lieBracketMinusGoalValue);
        Vector<Rational> positiveRoot1;
        Vector<Rational> positiveRoot2;
        positiveRoot1.makeEi(this->weylNonEmbedded->getDimension(), i);
        positiveRoot2.makeEi(this->weylNonEmbedded->getDimension(), j);
        int alphaStringLength = - 1;
        if (
          !nonEmbeddedMe.getMaxQForWhichBetaMinusQAlphaisARoot(
            positiveRoot1, - positiveRoot2, alphaStringLength
          )
        ) {
          global.fatal
          << "The alpha-string along "
          << positiveRoot1.toString()
          << " through "
          << (- positiveRoot2).toString()
          << " does not contain any root, which is impossible. "
          << global.fatal;
        }
        // positive-positive generator Serre relations
        lieBracketMinusGoalValue = this->unknownPositiveGenerators[j];
        for (int k = 0; k < alphaStringLength + 1; k ++) {
          this->getAmbientSemisimpleLieAlgebra().lieBracket(
            this->unknownPositiveGenerators[i],
            lieBracketMinusGoalValue,
            lieBracketMinusGoalValue
          );
        }
        this->addToSystem(lieBracketMinusGoalValue);
        // negative-negative generator Serre relations
        lieBracketMinusGoalValue = this->unknownNegativeGenerators[j];
        for (int k = 0; k < alphaStringLength + 1; k ++) {
          this->getAmbientSemisimpleLieAlgebra().lieBracket(
            this->unknownNegativeGenerators[i],
            lieBracketMinusGoalValue,
            lieBracketMinusGoalValue
          );
        }
        this->addToSystem(lieBracketMinusGoalValue);
      }
    }
  }
  this->flagSystemSolved = false;
  if (!attemptToChooseCentalizer) {
    this->flagSystemSolved = this->checkGeneratorsBracketToHs();
  }
  if (!this->flagSystemSolved) {
    this->flagSystemGroebnerBasisFound = false;
    this->flagSystemProvedToHaveNoSolution = false;
    if (this->owner->flagAttemptToSolveSystems) {
      long long int startNumberOfOperations =
      Rational::totalArithmeticOperations();
      this->attemptToSolveSystem();
      this->totalArithmeticOpsToSolveSystem +=
      Rational::totalArithmeticOperations() - startNumberOfOperations;
    }
  } else {
    this->flagSystemGroebnerBasisFound = false;
    this->flagSystemProvedToHaveNoSolution = false;
  }
  if (
    !this->flagSystemSolved && this->flagUsedInducingSubalgebraRealization
  ) {
    // bool seedHadNoSolution = this->flagSystemProvedToHaveNoSolution;
    this->flagUsedInducingSubalgebraRealization = false;
    bool result =
    this->computeSystemPart2(
      attemptToChooseCentalizer, allowNonPolynomialSystemFailure
    );
    return result;
  }
  if (this->flagSystemProvedToHaveNoSolution) {
    return false;
  }
  return this->computeFromGenerators(allowNonPolynomialSystemFailure);
}

bool CandidateSemisimpleSubalgebra::computeFromGenerators(
  bool allowNonPolynomialSystemFailure
) {
  STACK_TRACE("CandidateSemisimpleSubalgebra::computeFromGenerators");
  if (!this->flagSystemSolved) {
    return true;
  }
  this->basis = this->negativeGenerators;
  this->basis.addListOnTop(this->positiveGenerators);
  if (this->basis.size > 0) {
    this->owner->owner->generateLieSubalgebra(this->basis);
    if (
      this->basis.size !=
      this->weylNonEmbedded->dynkinType.getLieAlgebraDimension()
    ) {
      if (!allowNonPolynomialSystemFailure) {
        global.fatal
        << "Lie subalgebra dimension doesn't fit: "
        << "dimension of generated subalgebra is "
        << this->basis.size
        << ", must be "
        << this->weylNonEmbedded->dynkinType.getLieAlgebraDimension()
        << ". The subalgebra is "
        << this->toString(nullptr, false)
        << "<br>Involved generators: "
        << this->involvedNegativeGenerators.toString()
        << "<br>and<br>"
        << this->involvedPositiveGenerators.toString()
        << global.fatal;
      }
      return false;
    }
  }
  this->owner->owner->getCommonCentralizer(
    this->positiveGenerators, this->highestVectorsNonSorted
  );
  this->computeCartanOfCentralizer();
  this->computePrimalModuleDecompositionHighestWeightsAndHighestWeightVectors(
  );
  return true;
}

void CandidateSemisimpleSubalgebra::extendToModule(
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& inputOutput
) {
  STACK_TRACE("CandidateSemisimpleSubalgebra::extendToModule");
  ElementSemisimpleLieAlgebra<AlgebraicNumber> element;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > vectorSpace;
  HashedList<ChevalleyGenerator> bufferList;
  vectorSpace = inputOutput;
  ProgressReport report;
  for (int i = 0; i < inputOutput.size; i ++) {
    for (int j = 0; j < this->negativeGenerators.size; j ++) {
      std::stringstream reportStream;
      reportStream
      << "extendToModule: Lie bracket of element of index "
      << i + 1
      << " and negative generator index "
      << j + 1
      << ".";
      report.report(reportStream.str());
      this->getAmbientSemisimpleLieAlgebra().lieBracket(
        this->negativeGenerators[j], inputOutput[i], element
      );
      vectorSpace.addOnTop(element);
      element.gaussianEliminationByRowsDeleteZeroRows(
        vectorSpace, nullptr, &bufferList
      );
      if (vectorSpace.size > inputOutput.size) {
        inputOutput.addOnTop(element);
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
  STACK_TRACE("LinearCombination::intersectVectorSpaces");
  List<LinearCombinationTemplate> workingSpace = vectorSpace1;
  List<LinearCombinationTemplate> vectorSpace2Eliminated = vectorSpace2;
  LinearCombination<TemplateMonomial, Coefficient>::
  gaussianEliminationByRowsDeleteZeroRows(
    vectorSpace2Eliminated, nullptr, seedMonomials
  );
  LinearCombination<TemplateMonomial, Coefficient>::
  gaussianEliminationByRowsDeleteZeroRows(
    workingSpace, nullptr, seedMonomials
  );
  Matrix<Coefficient> linearCombinationMatrix;
  int dimensionFirstSpace = workingSpace.size;
  linearCombinationMatrix.makeIdentityMatrix(
    workingSpace.size + vectorSpace2Eliminated.size
  );
  workingSpace.addListOnTop(vectorSpace2Eliminated);
  vectorSpace2Eliminated = workingSpace;
  LinearCombination<TemplateMonomial, Coefficient>::gaussianEliminationByRows(
    workingSpace, nullptr, seedMonomials, &linearCombinationMatrix
  );
  int resultDimension = 0;
  for (int i = workingSpace.size - 1; i >= 0; i --) {
    if (workingSpace[i].isEqualToZero()) {
      resultDimension ++;
    } else {
      break;
    }
  }
  outputIntersection.setSize(resultDimension);
  int counter = - 1;
  LinearCombinationTemplate current;
  for (int i = workingSpace.size - 1; i >= 0; i --) {
    if (!workingSpace[i].isEqualToZero()) {
      break;
    }
    counter ++;
    outputIntersection[counter].makeZero();
    for (int j = 0; j < dimensionFirstSpace; j ++) {
      current = vectorSpace2Eliminated[j];
      current *= linearCombinationMatrix(i, j);
      outputIntersection[counter] += current;
    }
  }
}

std::string SemisimpleSubalgebras::WConjecture::
toStringElementSemisimpleLieAlgebraOrMatrix(
  const SemisimpleSubalgebras& owner,
  const List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& input,
  const std::string& notationLetter
) const {
  STACK_TRACE(
    "SemisimpleSubalgebras::WConjecture::"
    "toStringElementSemisimpleLieAlgebraOrMatrix"
  );
  std::stringstream generatorForm;
  std::stringstream matrixFormStream;
  FormatExpressions format;
  format.flagUseLatex = true;
  bool hasMatrixForm = false;
  for (int i = 0; i < input.size; i ++) {
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& current = input[i];
    Matrix<AlgebraicNumber> matrixForm;
    std::stringstream notationStream;
    if (notationLetter != "") {
      notationStream << notationLetter << "_{" << i + 1 << "}=";
    }
    if (
      owner.owner->getElementStandardRepresentation(current, matrixForm)
    ) {
      hasMatrixForm = true;
      matrixFormStream
      << notationStream.str()
      << HtmlRoutines::getMathNoDisplay(matrixForm.toString(&format));
    }
    generatorForm << notationStream.str() << current.toString();
    if (i != input.size - 1) {
      generatorForm << ", ";
      matrixFormStream << ", ";
    }
  }
  std::stringstream out;
  out << HtmlRoutines::getMathNoDisplay(generatorForm.str());
  if (hasMatrixForm) {
    out << "<br>Matrix form:<br>" << matrixFormStream.str();
  }
  return out.str();
}

std::string SemisimpleSubalgebras::WConjecture::toStringRealForm(
  const SemisimpleSubalgebras& owner
) const {
  STACK_TRACE("SemisimpleSubalgebras::WConjecture::toStringRealForm");
  std::stringstream out;
  FormatExpressions matrixFormat;
  matrixFormat.flagUseLatex = true;
  out << "Killing form: ";
  out << "<div class='lieAlgebraPanel'><div>";
  out
  << HtmlRoutines::getMathNoDisplay(
    this->killingForm.toString(&matrixFormat)
  );
  out << "</div></div>";
  out << "Killing form rescaled: ";
  out << "<div class='lieAlgebraPanel'><div>";
  out
  << HtmlRoutines::getMathNoDisplay(
    this->killingFormRescaled.toString(&matrixFormat)
  );
  out << "</div></div>";
  out << "Basis of k (1-eigenspace of Cartan involution): ";
  out << "<div class='lieAlgebraPanel'><div>";
  out
  << this->toStringElementSemisimpleLieAlgebraOrMatrix(
    owner, this->basisKAmbient, ""
  );
  out << "</div></div>";
  out << "<br>Basis of p (-1-eigenspace of Cartan involution): ";
  out << "<div class='lieAlgebraPanel'><div>";
  out
  << this->toStringElementSemisimpleLieAlgebraOrMatrix(
    owner, this->basisPAmbient, ""
  );
  out << "</div></div>";
  return out.str();
}

bool SemisimpleSubalgebras::WConjecture::wConjectureHolds(
  SemisimpleSubalgebras& owner
) {
  for (int i = 0; i < owner.subalgebras.size(); i ++) {
    if (
      !owner.subalgebras.values[i].wConjecture.flagWConjectureHolds
    ) {
      return false;
    }
  }
  return true;
}

void SemisimpleSubalgebras::WConjecture::computeBeforeSubalgebras(
  SemisimpleSubalgebras& owner
) {
  STACK_TRACE("SemisimpleSubalgebras::WConjecture::computeBeforeSubalgebras");
  if (!owner.flagRealForms) {
    return;
  }
  if (
    !owner.owner->hasImplementedCartanInvolutionMaximallyCompactCase(
      &this->cartanInvolutionAmbient
    )
  ) {
    global.comments
    << "I couldn't figure out the required Cartan involution. ";
    return;
  }
  ProgressReport report;
  report.report("Computing Killing form.");
  owner.owner->getKillingFormMatrix(this->killingForm);
  Matrix<Rational> killingFormRescaledRational;
  killingFormRescaledRational = this->killingForm;
  killingFormRescaledRational.
  scaleToIntegralForMinimalRationalHeightNoSignChange();
  this->killingFormRescaled = killingFormRescaledRational;
  report.report("Computing k, p");
  this->cartanInvolutionAmbient.findEigenSpace(1, this->basisKAmbient);
  this->cartanInvolutionAmbient.findEigenSpace(- 1, this->basisPAmbient);
}

void SemisimpleSubalgebras::WConjecture::computeAfterSubalgebras(
  SemisimpleSubalgebras& owner
) {
  this->numberOfSmallSlTwos = 0;
  for (int i = 0; i < owner.subalgebras.size(); i ++) {
    if (owner.subalgebras.values[i].wConjecture.flagSlTwoIsSmall) {
      this->numberOfSmallSlTwos ++;
    }
  }
}

bool CandidateSemisimpleSubalgebra::checkModuleDimensions() const {
  STACK_TRACE("CandidateSemisimpleSubalgebra::checkModuleDimensions");
  int totalDimension = 0;
  for (int i = 0; i < this->modules.size; i ++) {
    for (int j = 0; j < this->modules[i].size; j ++) {
      totalDimension += this->modules[i][j].size;
    }
  }
  if (
    totalDimension !=
    this->getAmbientSemisimpleLieAlgebra().getNumberOfGenerators()
  ) {
    FormatExpressions format;
    format.flagCandidateSubalgebraShortReportOnly = false;
    global.fatal
    << "<br><b>Fatal error while computing candidate "
    << this->weylNonEmbedded->dynkinType.toString()
    << ": dimensions do not add up correctly. "
    << "I am getting total module dimension sum "
    << totalDimension
    << " instead of "
    << this->getAmbientSemisimpleLieAlgebra().getNumberOfGenerators()
    << ".</b> Here is a detailed subalgebra printout. "
    << this->toString(&format, false)
    << global.fatal;
  }
  return true;
}

void CandidateSemisimpleSubalgebra::computeRatioKillingsByComponent() {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebra::computeRatioKillingsByComponent"
  );
  ElementSemisimpleLieAlgebra<AlgebraicNumber> currentElement;
  ElementSemisimpleLieAlgebra<AlgebraicNumber> adActionElement;
  ElementSemisimpleLieAlgebra<AlgebraicNumber> adAdActionElement;
  Vector<AlgebraicNumber> linearCombination;
  this->ratiosKillingsByComponent.setSize(
    this->cartanSubalgebrasByComponentScaledToActByTwo.size
  );
  Matrix<AlgebraicNumber> adMatrix;
  for (
    int i = 0; i < this->cartanSubalgebrasByComponentScaledToActByTwo.size; i
    ++
  ) {
    currentElement.makeCartanGenerator(
      this->cartanSubalgebrasByComponentScaledToActByTwo[i][0],
      this->getAmbientSemisimpleLieAlgebra()
    );
    AlgebraicNumber result = 0;
    for (int k = 0; k < this->basis.size; k ++) {
      this->getAmbientSemisimpleLieAlgebra().lieBracket(
        currentElement, this->basis[k], adActionElement
      );
      this->getAmbientSemisimpleLieAlgebra().lieBracket(
        currentElement, adActionElement, adAdActionElement
      );
      bool tempB =
      currentElement.linearSpanContainsGetFirstLinearCombination(
        this->basis, adAdActionElement, linearCombination
      );
      if (!tempB) {
        global.fatal
        << "Candidate subalgebra not closed under Lie bracket. "
        << global.fatal;
      }
      result += linearCombination[k];
    }
    this->getAmbientSemisimpleLieAlgebra().getAdjoint(
      adMatrix, currentElement
    );
    adMatrix *= adMatrix;
    this->ratiosKillingsByComponent[i] = adMatrix.getTrace();
    this->ratiosKillingsByComponent[i] /= result;
  }
}

std::string CandidateSemisimpleSubalgebra::WConjecture::
toStringAdjointActionNegativeOne() const {
  std::stringstream out;
  out << "Element e: \\(" << this->elementE.toString() << "\\)<br>";
  out
  << "Generic negative 1 weight element: \\("
  << this->genericNegativeOneWeightElement.toString()
  << "\\)<br>";
  out
  << "Adjoint: \\("
  << this->genericNegativeOneWeightElement.toString()
  << "\\)";
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::WConjecture::
toStringWConjectureHolds() const {
  std::stringstream out;
  if (this->triplesViolatingWConjecture.size == 0) {
    if (this->flagSlTwoIsSmall) {
      out << "<b style='color:green'>W-conjecture holds</b>";
    } else {
      out
      << "<b style='color:blue'>[W,W]-condition holds</b>"
      << " but the subalgebra is not small.";
    }
  } else {
    if (!this->flagWConjectureHolds) {
      out << "<b style='color:red'>W-conjecture does not hold</b>";
    } else {
      out
      << "<b style='color:blue'>The [W,W]-condition</b> "
      << "<b style='color:red'>fails</b> "
      << "but that's OK as the subalgebra is not small.</b>";
    }
  }
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::WConjecture::toString(
  const CandidateSemisimpleSubalgebra& owner
) const {
  STACK_TRACE("CandidateSemisimpleSubalgebra::WConjecture::toString");
  std::stringstream out;
  if (this->flagSlTwoIsSmall) {
    out << "sl(2)-subalgebra is <b style='color:blue'>small</b><br>";
  } else {
    out << "sl(2)-subalgebra is <b>not small</b><br>";
  }
  out << "Centalizer of H: <br>";
  out << "<div class='lieAlgebraPanel'>";
  out << "<div>";
  out
  << owner.owner->wConjecture.toStringElementSemisimpleLieAlgebraOrMatrix(
    *owner.owner, this->basisCentralizerOfH, ""
  );
  out << "</div></div>";
  out << "\\(\\mathfrak p ^H\\) = centralizer of H in p: <br>";
  out << "<div class='lieAlgebraPanel'>";
  out << "<div>";
  out
  << owner.owner->wConjecture.toStringElementSemisimpleLieAlgebraOrMatrix(
    *owner.owner, this->basisCentralizerOfHInP, ""
  );
  out << "</div></div>";
  out
  << "\\(\\mathfrak g^{\\mathfrak s} \\) "
  << "= centralizer of sl(2): <br>";
  out << "<div class='lieAlgebraPanel'>";
  out << "<div>";
  out
  << owner.owner->wConjecture.toStringElementSemisimpleLieAlgebraOrMatrix(
    *owner.owner, this->basisCentralizerOfSl2, ""
  );
  out << "</div></div>";
  out
  << "\\(\\mathfrak p ^{\\mathfrak s}\\) "
  << "= centralizer of sl(2) in \\(\\mathfrak p\\): <br>";
  out << "<div class='lieAlgebraPanel'>";
  out << "<div>";
  out
  << owner.owner->wConjecture.toStringElementSemisimpleLieAlgebraOrMatrix(
    *owner.owner, this->basisCentralizerOfSl2InP, ""
  );
  out << "</div></div>";
  out
  << "\\( (\\mathfrak p ^\\mathfrak s)^\\perp \\) "
  << "= orthogonal complement of centalizer of sl(2) in p: <br>";
  out << "<div class='lieAlgebraPanel'>";
  out << "<div>";
  out
  << owner.owner->wConjecture.toStringElementSemisimpleLieAlgebraOrMatrix(
    *owner.owner,
    this->basisOrthogonalComplementOfCentralizerOfSl2InP,
    ""
  );
  out << "</div></div>";
  out
  << "\\(W=\\mathfrak p ^H \\cap (\\mathfrak p ^\\mathfrak s)^\\perp \\) "
  << "= centralizer of H intersected with "
  << "orthogonal complement of centralizer "
  << "of the sl(2) intersected with p. <br>";
  out << "<div class='lieAlgebraPanel'>";
  out << "<div>";
  out
  << owner.owner->wConjecture.toStringElementSemisimpleLieAlgebraOrMatrix(
    *owner.owner,
    this->
    basisOrthogonalComplementOfCentralizerOfSl2InPIntersectedWithCentralizerOfHInP
    ,
    "w"
  );
  out << "</div></div>";
  out << this->toStringWConjectureHolds();
  if (this->triplesNotViolatingWConjecture.size > 0) {
    out
    << "<br>Lie brackets between the basis elements of \\(W\\) "
    << "that <b style='color:green'>lie in "
    << "\\(\\mathfrak k^{\\mathfrak s}\\)</b>. "
    << "All zero Lie brackets are omitted.<br>";
    out
    << this->toStringLieBracketTriples(this->triplesNotViolatingWConjecture);
  }
  out << this->toStringAdjointActionNegativeOne();
  if (this->triplesViolatingWConjecture.size > 0) {
    out
    << "<br>Lie brackets between the basis elements of \\(W\\) "
    << "that <b style='color:red'>"
    << "do not lie in \\(\\mathfrak k^{\\mathfrak s}\\)</b>.<br>";
    out << this->toStringLieBracketTriples(this->triplesViolatingWConjecture);
  }
  return out.str();
}

CandidateSemisimpleSubalgebra::WConjecture::WConjecture() {
  this->flagWConjectureHolds = false;
  this->flagSlTwoIsSmall = false;
}

std::string CandidateSemisimpleSubalgebra::WConjecture::
toStringLieBracketTriples(const List<std::string>& triple) const {
  std::stringstream out;
  out << "<div class='lieAlgebraPanel'>";
  out << "<div>";
  out << "\\( \\begin{array}{rcrcl}";
  for (int i = 0; i < triple.size; i ++) {
    out << triple[i];
  }
  out << "\\end{array} \\)";
  out << "</div></div>";
  return out.str();
}

void CandidateSemisimpleSubalgebra::WConjecture::compute(
  CandidateSemisimpleSubalgebra& owner, SlTwoSubalgebra& ownerSl2
) {
  STACK_TRACE("CandidateSemisimpleSubalgebra::WConjecture::compute");
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > hWrapper;
  hWrapper.addOnTop(ownerSl2.hAlgebraic);
  owner.owner->owner->getCommonCentralizer(
    hWrapper, this->basisCentralizerOfH
  );
  ElementSemisimpleLieAlgebra<AlgebraicNumber>::intersectVectorSpaces(
    this->basisCentralizerOfH,
    owner.owner->wConjecture.basisPAmbient,
    this->basisCentralizerOfHInP
  );
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > slTwoWrapper;
  slTwoWrapper.addOnTop(ownerSl2.hAlgebraic);
  slTwoWrapper.addOnTop(ownerSl2.eKostantSekiguchi);
  slTwoWrapper.addOnTop(ownerSl2.fKostantSekiguchi);
  owner.owner->owner->getCommonCentralizer(
    slTwoWrapper, this->basisCentralizerOfSl2
  );
  ElementSemisimpleLieAlgebra<AlgebraicNumber>::intersectVectorSpaces(
    this->basisCentralizerOfSl2,
    owner.owner->wConjecture.basisPAmbient,
    this->basisCentralizerOfSl2InP
  );
  Vectors<AlgebraicNumber> centralizerOfSl2InP;
  centralizerOfSl2InP.setSize(this->basisCentralizerOfSl2InP.size);
  for (int i = 0; i < this->basisCentralizerOfSl2InP.size; i ++) {
    this->basisCentralizerOfSl2InP[i].toVectorNegativeRootSpacesFirst(
      centralizerOfSl2InP[i]
    );
  }
  Vectors<AlgebraicNumber> orthogonalComplement;
  centralizerOfSl2InP.getOrthogonalComplement(
    orthogonalComplement, &owner.owner->wConjecture.killingFormRescaled
  );
  this->basisOrthogonalComplementOfCentralizerOfSl2InP.setSize(
    orthogonalComplement.size
  );
  for (int i = 0; i < orthogonalComplement.size; i ++) {
    this->basisOrthogonalComplementOfCentralizerOfSl2InP[i].
    assignVectorNegativeRootSpacesCartanPosistiveRootSpaces(
      orthogonalComplement[i], *owner.owner->owner
    );
  }
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& wSpace =
  this->
  basisOrthogonalComplementOfCentralizerOfSl2InPIntersectedWithCentralizerOfHInP
  ;
  ElementSemisimpleLieAlgebra<AlgebraicNumber>::intersectVectorSpaces(
    this->basisOrthogonalComplementOfCentralizerOfSl2InP,
    this->basisCentralizerOfHInP,
    wSpace
  );
  ElementSemisimpleLieAlgebra<AlgebraicNumber>::intersectVectorSpaces(
    this->basisCentralizerOfSl2,
    owner.owner->wConjecture.basisKAmbient,
    this->basisCentralizerOfSl2InK
  );
  for (int i = 0; i < wSpace.size; i ++) {
    for (int j = i + 1; j < wSpace.size; j ++) {
      this->processOnePair(i, j);
    }
  }
  this->computeSmallOrbits(owner);
  this->computeAdEAsPolynomialMap(owner);
  this->flagWConjectureHolds = true;
  if (
    this->flagSlTwoIsSmall && this->triplesViolatingWConjecture.size > 0
  ) {
    this->flagWConjectureHolds = false;
  }
}

void CandidateSemisimpleSubalgebra::WConjecture::computeAdEAsPolynomialMap(
  const CandidateSemisimpleSubalgebra& owner
) {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebra::WConjecture::"
    "computeAdEAsPolynomialMap"
  );
  this->elementE = owner.positiveGenerators[0];
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >
  basisElementUnknownCoefficient;
  Polynomial<AlgebraicNumber> unknownCoefficient;
  int elementsFound = 0;
  // Iterate over all modules of a given primal highest weight.
  // Recall in our terminology,
  // primal means the weight is taken with respect to
  // the Cartan of k and the
  // Cartan of its centralizer.
  for (int i = 0; i < owner.modules.size; i ++) {
    // Among the modules with given primal highest weight, iterate over the
    // individual ones.
    // Two modules with identical highest weight may be ind
    for (int j = 0; j < owner.modules[i].size; j ++) {
      List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& module =
      owner.modules[i][j];
      if ((module.size % 2) != 0) {
        // A module has an element of weight - 1 only when the module
        // has even dimension (0-dimension not allowed).
        continue;
      }
      // The last element of the module is the lowest-weight element.
      basisElementUnknownCoefficient = module[module.size - 1];
      unknownCoefficient.makeMonomial(elementsFound, 1, 1);
      basisElementUnknownCoefficient *= unknownCoefficient;
      this->genericNegativeOneWeightElement += basisElementUnknownCoefficient;
    }
  }
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > adEOfGNegativeTwo;
  owner.owner->owner->lieBracket(
    this->elementE,
    this->genericNegativeOneWeightElement,
    this->adEOfGenericNegativeOneWeightElement
  );
}

void CandidateSemisimpleSubalgebra::WConjecture::computeSmallOrbits(
  const CandidateSemisimpleSubalgebra& owner
) {
  this->flagSlTwoIsSmall = false;
  for (
    int i = 0; i < owner.characterNonPrimalFundamentalCoordinates.size(); i ++
  ) {
    const Weight<Rational>& weight =
    owner.characterNonPrimalFundamentalCoordinates.monomials[i];
    if (weight.weightFundamentalCoordinates.size != 1) {
      global.fatal
      << "Small orbits can only be computed for sl(2)-subalgebras, "
      << "whose non-primal characters should be 1-dimensional."
      << global.fatal;
    }
    if (weight.weightFundamentalCoordinates[0] > 2) {
      return;
    }
  }
  this->flagSlTwoIsSmall = true;
}

void CandidateSemisimpleSubalgebra::WConjecture::processOnePair(
  int indexLeft, int indexRight
) {
  STACK_TRACE("CandidateSemisimpleSubalgebra::WConjecture::processOnePair");
  std::stringstream out;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& wSpace =
  this->
  basisOrthogonalComplementOfCentralizerOfSl2InPIntersectedWithCentralizerOfHInP
  ;
  ElementSemisimpleLieAlgebra<AlgebraicNumber>& left = wSpace[indexLeft];
  ElementSemisimpleLieAlgebra<AlgebraicNumber>& right = wSpace[indexRight];
  ElementSemisimpleLieAlgebra<AlgebraicNumber> lieBracket;
  left.getOwner()->lieBracket(left, right, lieBracket);
  if (lieBracket.isEqualToZero()) {
    return;
  }
  out
  << "["
  << "w_{"
  << indexLeft + 1
  << "}"
  << ", "
  << "w_{"
  << indexRight + 1
  << "}"
  << "] &=& "
  << "["
  << left.toString()
  << ", "
  << right.toString()
  << "] &=&"
  << lieBracket.toString()
  << "\\\\\n";
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > lieBracketWrapper;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > intersection;
  lieBracketWrapper.addOnTop(lieBracket);
  ElementSemisimpleLieAlgebra<AlgebraicNumber>::intersectVectorSpaces(
    this->basisCentralizerOfSl2InK, lieBracketWrapper, intersection
  );
  if (intersection.size > 0) {
    this->triplesNotViolatingWConjecture.addOnTop(out.str());
  } else {
    this->triplesViolatingWConjecture.addOnTop(out.str());
  }
}

void CandidateSemisimpleSubalgebra::computePrimalModuleDecomposition() {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebra::computePrimalModuleDecomposition"
  );
  for (int i = 0; i < this->modules.size; i ++) {
    for (int j = 0; j < this->modules[i].size; j ++) {
      this->extendToModule(this->modules[i][j]);
    }
  }
  this->checkModuleDimensions();
  this->modulesIsotypicallyMerged.setSize(this->modules.size);
  for (int i = 0; i < this->modules.size; i ++) {
    this->modulesIsotypicallyMerged[i].setSize(0);
    for (int j = 0; j < this->modules[i].size; j ++) {
      this->modulesIsotypicallyMerged[i].addListOnTop(
        this->modules[i][j]
      );
    }
  }
  // Part of primalSubalgebraModules has already been computed.
  for (int i = 0; i < this->modulesIsotypicallyMerged.size; i ++) {
    if (this->modulesIsotypicallyMerged[i][0].isElementCartan()) {
      for (
        int j = 0; j < this->modulesIsotypicallyMerged[i].size; j ++
      ) {
        if (
          !this->modulesIsotypicallyMerged[i][j].isElementCartan()
        ) {
          global.fatal
          << "<br>This is a programming or mathematical error. Module "
          << this->modulesIsotypicallyMerged[i].toString()
          << " has elements of the ambient Cartan "
          << "and elements outside of the ambient Cartan, "
          << "which is not allowed. "
          << "<br>Here is a detailed subalgebra printout. "
          << global.fatal;
        }
      }
      this->primalSubalgebraModules.addOnTop(i);
    }
  }
  this->fullBasisByModules.setSize(0);
  this->fullBasisOwnerModules.setSize(0);
  this->fullBasisByModules.reserve(
    this->getAmbientSemisimpleLieAlgebra().getNumberOfGenerators()
  );
  this->fullBasisOwnerModules.reserve(
    this->getAmbientSemisimpleLieAlgebra().getNumberOfGenerators()
  );
  for (int i = 0; i < this->modulesIsotypicallyMerged.size; i ++) {
    for (int j = 0; j < this->modulesIsotypicallyMerged[i].size; j ++) {
      this->fullBasisByModules.addOnTop(
        this->modulesIsotypicallyMerged[i][j]
      );
      this->fullBasisOwnerModules.addOnTop(i);
    }
  }
  if (
    this->fullBasisByModules.size !=
    this->getAmbientSemisimpleLieAlgebra().getNumberOfGenerators()
  ) {
    global.fatal
    << "The full basis by modules "
    << "does not have same number of elements as the number "
    << "of generators of the ambient Lie algebra. "
    << global.fatal;
  }
  this->weightsModulesNONprimal.setSize(this->modules.size);
  this->weightsModulesPrimal.setSize(this->modules.size);
  Vector<Rational> projection;
  Vector<Rational> primalProjection;
  for (int i = 0; i < this->modules.size; i ++) {
    for (int k = 0; k < this->modules[i].size; k ++) {
      if (k == 0) {
        this->weightsModulesNONprimal[i].setSize(
          this->modules[i][0].size
        );
        this->weightsModulesPrimal[i].setSize(
          this->modules[i][0].size
        );
      }
      for (int j = 0; j < this->modules[i][k].size; j ++) {
        ElementSemisimpleLieAlgebra<AlgebraicNumber>& currentElement =
        this->modules[i][k][j];
        Vector<Rational> currentRoot =
        this->getAmbientSemisimpleLieAlgebra().getWeightOfGenerator(
          currentElement[0].generatorIndex
        );
        this->getWeightProjectionFundamentalCoordinates(
          currentRoot, projection
        );
        this->getPrimalWeightProjectionFundamentalCoordinates(
          currentRoot, primalProjection
        );
        if (k == 0) {
          this->weightsModulesNONprimal[i][j] = projection;
          this->weightsModulesPrimal[i][j] = primalProjection;
        } else {
          if (
            this->weightsModulesNONprimal[i][j] != projection ||
            this->weightsModulesPrimal[i][j] != primalProjection
          ) {
            global.fatal
            << "This is a programming or mathematical error. "
            << "Given two isomorphic modules over "
            << "the semisimple subalgebra (i.e., same highest weights), "
            << "and the same order of generation of weights, "
            << "I got different order "
            << "of the lower weights of the modules. "
            << "Something is very wrong. "
            << global.fatal;
          }
        }
      }
    }
  }
  this->centralizerSubalgebraModules.setSize(0);
  this->rootSystemCentralizerPrimalCoordinates.clear();
  for (int i = 0; i < this->modulesIsotypicallyMerged.size; i ++) {
    if (
      this->modulesIsotypicallyMerged[i].size == 1 ||
      this->weightsModulesPrimal[i][0].isEqualToZero()
    ) {
      this->centralizerSubalgebraModules.addOnTop(i);
      if (!this->weightsModulesPrimal[i][0].isEqualToZero()) {
        this->rootSystemCentralizerPrimalCoordinates.addOnTop(
          this->weightsModulesPrimal[i][0]
        );
      }
    }
  }
  for (int i = 0; i < this->subalgebraModules.size; i ++) {
    for (
      int j = 0; j < this->weightsModulesPrimal[
        this->subalgebraModules[i]
      ].size; j ++
    ) {
      if (
        !this->weightsModulesPrimal[this->subalgebraModules[i]][j].
        isEqualToZero()
      ) {
        this->rootSystemSubalgebraPrimalCoordinates.addOnTop(
          this->weightsModulesPrimal[this->subalgebraModules[i]][j]
        );
      }
    }
  }
  Vectors<Rational> simpleSubSystem;
  simpleSubSystem = this->rootSystemCentralizerPrimalCoordinates;
  this->centralizerSubDiagram.computeDiagramTypeModifyInputRelative(
    simpleSubSystem,
    this->rootSystemCentralizerPrimalCoordinates,
    this->bilinearFormFundamentalPrimal
  );
  this->centralizerSubDiagram.getDynkinType(this->centralizerType);
  this->flagCentralizerTypeIsComputed = true;
  this->computeCharactersPrimalModules();
  // this->computeRatioKillingsByComponent();
}

void CandidateSemisimpleSubalgebra::reset(SemisimpleSubalgebras* inputOwner) {
  STACK_TRACE("CandidateSemisimpleSubalgebra::reset");
  this->owner = inputOwner;
  this->indexInSlTwos = - 1;
  this->indexInOwner = - 1;
  this->indexInducedFrom = - 1;
  this->indexNonEmbeddedMeNonStandardCartan = - 1;
  this->indexNonEmbeddedMeStandard = - 1;
  this->indexHcandidateBeingGrown = - 1;
  this->indexMaximalSemisimpleContainer = - 1;
  this->indexSemisimplePartCentralizer = - 1;
  this->flagSubalgebraPreloadedButNotVerified = false;
  this->flagSystemSolved = false;
  this->flagSystemProvedToHaveNoSolution = false;
  this->flagSystemGroebnerBasisFound = false;
  this->flagCentralizerIsWellChosen = false;
  this->flagCentralizerTypeIsComputed = false;
  this->flagUsedInducingSubalgebraRealization = true;
  this->totalUnknownsNoCentralizer = 0;
  this->totalUnknownsWithCentralizer = 0;
  this->totalArithmeticOpsToSolveSystem = 0;
  this->numberOfConeIntersections = - 1;
  this->numberOfCasesNoLInfiniteRelationFound = - 1;
  this->numberOfBadParabolics = 0;
  this->numberOfCentralizerConditionFailsConeConditionHolds = 0;
  this->centralizerRank = - 1;
  this->weylNonEmbedded = nullptr;
  this->subalgebraNonEmbeddedDefaultScale = nullptr;
}

CandidateSemisimpleSubalgebra::CandidateSemisimpleSubalgebra() {
  this->flagDeallocated = false;
  this->reset(nullptr);
}

CandidateSemisimpleSubalgebra::~CandidateSemisimpleSubalgebra() {
  this->flagDeallocated = true;
}

void CandidateSemisimpleSubalgebra::computePairKWeightElementAndModule(
  const ElementSemisimpleLieAlgebra<AlgebraicNumber>& leftKWeightElement,
  int rightIndex,
  List<int>& output
) {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebra::"
    "computePairKWeightElementAndModule"
  );
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& rightModule =
  this->modulesIsotypicallyMerged[rightIndex];
  ElementSemisimpleLieAlgebra<AlgebraicNumber> lieBracket;
  ProgressReport report;
  Vector<AlgebraicNumber> coordinatesInFullBasis;
  output.setSize(0);
  if (
    this->fullBasisByModules.size !=
    this->getAmbientSemisimpleLieAlgebra().getNumberOfGenerators()
  ) {
    global.fatal
    << "FullBasisByModules not computed when it should be. "
    << global.fatal;
  }
  for (int j = 0; j < rightModule.size; j ++) {
    this->getAmbientSemisimpleLieAlgebra().lieBracket(
      leftKWeightElement, rightModule[j], lieBracket
    );
    if (report.tickAndWantReport()) {
      std::stringstream reportStream;
      reportStream << "Bracketing index " << j + 1 << " with input element. ";
      report.report(reportStream.str());
    }
    bool tempbool =
    lieBracket.getCoordinatesInBasis(
      this->fullBasisByModules, coordinatesInFullBasis
    );
    if (!tempbool) {
      global.fatal
      << "Something has gone very wrong: my k-weight basis "
      << this->fullBasisByModules.toString()
      << " does not contain "
      << lieBracket.toString()
      << global.fatal;
    }
    for (int i = 0; i < coordinatesInFullBasis.size; i ++) {
      if (!coordinatesInFullBasis[i].isEqualToZero()) {
        output.addOnTopNoRepetition(this->fullBasisOwnerModules[i]);
      }
    }
  }
}

void CandidateSemisimpleSubalgebra::computeSinglePair(
  int leftIndex, int rightIndex, List<int>& output
) {
  STACK_TRACE("CandidateSemisimpleSubalgebra::computeSinglePair");
  output.setSize(0);
  List<int> tempList;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& leftModule =
  this->modulesIsotypicallyMerged[leftIndex];
  ProgressReport report;
  for (int i = 0; i < leftModule.size; i ++) {
    if (report.tickAndWantReport()) {
      std::stringstream reportStream;
      reportStream
      << "Bracketing element number "
      << i + 1
      << " out of "
      << leftModule.size
      << " with other module. ";
      report.report(reportStream.str());
    }
    this->computePairKWeightElementAndModule(
      leftModule[i], rightIndex, tempList
    );
    output.addOnTopNoRepetition(tempList);
  }
}

void CandidateSemisimpleSubalgebra::computePairingTable() {
  STACK_TRACE("CandidateSemisimpleSubalgebra::computePairingTable");
  if (!this->flagCentralizerIsWellChosen) {
    return;
  }
  ProgressReport report;
  this->nilradicalPairingTable.setSize(
    this->modulesIsotypicallyMerged.size
  );
  for (int i = 0; i < this->nilradicalPairingTable.size; i ++) {
    this->nilradicalPairingTable[i].setSize(
      this->modulesIsotypicallyMerged.size
    );
  }
  for (int i = 0; i < this->nilradicalPairingTable.size; i ++) {
    for (int j = i; j < this->nilradicalPairingTable[i].size; j ++) {
      if (report.tickAndWantReport()) {
        std::stringstream reportStream;
        reportStream
        << "Pairing indices "
        << i + 1
        << " and "
        << j + 1
        << " out of "
        << this->nilradicalPairingTable.size
        << ".";
        report.report(reportStream.str());
      }
      this->computeSinglePair(
        i, j, this->nilradicalPairingTable[i][j]
      );
      if (j > i) {
        this->nilradicalPairingTable[j][i] =
        this->nilradicalPairingTable[i][j];
      }
    }
  }
  this->modulesWithZeroWeights.clear();
  for (int i = 0; i < this->nilradicalPairingTable.size; i ++) {
    for (int j = 0; j < this->modulesIsotypicallyMerged[i].size; j ++) {
      if (
        this->getAmbientSemisimpleLieAlgebra().getWeightOfGenerator(
          this->modulesIsotypicallyMerged[i][j][0].generatorIndex
        ).isEqualToZero()
      ) {
        this->modulesWithZeroWeights.addOnTop(i);
        break;
      }
    }
  }
  this->oppositeModulesByStructure.setSize(
    this->nilradicalPairingTable.size
  );
  for (int i = 0; i < this->nilradicalPairingTable.size; i ++) {
    for (int j = 0; j < this->nilradicalPairingTable[i].size; j ++) {
      for (
        int k = 0; k < this->nilradicalPairingTable[i][j].size; k ++
      ) {
        if (
          this->modulesWithZeroWeights.contains(
            this->nilradicalPairingTable[i][j][k]
          )
        ) {
          if (
            !(
              this->primalSubalgebraModules.contains(i) &&
              this->primalSubalgebraModules.contains(j)
            )
          ) {
            this->oppositeModulesByStructure[i].addOnTopNoRepetition(j);
          }
        }
      }
    }
  }
  this->computeKsl2Triples();
}

void CandidateSemisimpleSubalgebra::computeCharactersPrimalModules() {
  STACK_TRACE("CandidateSemisimpleSubalgebra::computeCharactersPrimalModules");
  this->charactersPrimalModules.setSize(this->modules.size);
  this->charactersPrimalModulesMerged.setSize(this->modules.size);
  Weight<Rational> currentWeight;
  currentWeight.owner = nullptr;
  for (int i = 0; i < this->charactersPrimalModules.size; i ++) {
    this->charactersPrimalModules[i].makeZero();
    this->charactersPrimalModulesMerged[i].makeZero();
    for (int j = 0; j < this->weightsModulesPrimal[i].size; j ++) {
      currentWeight.weightFundamentalCoordinates =
      this->weightsModulesPrimal[i][j];
      this->charactersPrimalModules[i].addMonomial(currentWeight, 1);
      this->charactersPrimalModulesMerged[i].addMonomial(
        currentWeight, this->modules[i].size
      );
    }
  }
  this->oppositeModulesByChar.setSize(this->modules.size);
  List<CharacterSemisimpleLieAlgebraModule<Rational> > dualModules;
  dualModules.setSize(this->modules.size);
  for (int i = 0; i < this->modules.size; i ++) {
    this->charactersPrimalModules[i].getDual(dualModules[i]);
    this->oppositeModulesByChar[i].setSize(0);
  }
  for (int i = 0; i < this->modules.size; i ++) {
    for (int j = i; j < this->modules.size; j ++) {
      if ((this->charactersPrimalModules[i] - dualModules[j]).isEqualToZero()
      ) {
        this->oppositeModulesByChar[i].addOnTop(j);
        if (i != j) {
          this->oppositeModulesByChar[j].addOnTop(i);
        }
      }
    }
  }
}

void CandidateSemisimpleSubalgebra::
computeKsl2TriplesGetOppositeElementsInOppositeModule(
  const Vector<Rational>& weight,
  const List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >&
  inputOppositeModule,
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputElements
) {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebra::"
    "computeKsl2TriplesGetOppositeElementsInOppositeModule"
  );
  outputElements.setSize(0);
  for (int i = 0; i < inputOppositeModule.size; i ++) {
    if ((
        this->getPrimalWeightFirstGenerator(inputOppositeModule[i]) + weight
      ).isEqualToZero()
    ) {
      outputElements.addOnTop(inputOppositeModule[i]);
    }
  }
}

Vector<Rational> CandidateSemisimpleSubalgebra::getPrimalWeightFirstGenerator(
  const ElementSemisimpleLieAlgebra<AlgebraicNumber>& input
) const {
  Vector<Rational> output;
  Vector<Rational> root =
  this->getAmbientSemisimpleLieAlgebra().getWeightOfGenerator(
    input[0].generatorIndex
  );
  this->getPrimalWeightProjectionFundamentalCoordinates(root, output);
  return output;
}

Vector<Rational> CandidateSemisimpleSubalgebra::
getNonPrimalWeightFirstGenerator(
  const ElementSemisimpleLieAlgebra<AlgebraicNumber>& input
) const {
  Vector<Rational> output;
  Vector<Rational> root =
  this->getAmbientSemisimpleLieAlgebra().getWeightOfGenerator(
    input[0].generatorIndex
  );
  this->getWeightProjectionFundamentalCoordinates(root, output);
  return output;
}

void CandidateSemisimpleSubalgebra::computeKsl2TriplesGetOppositeElementsAll(
  const Vector<Rational>& weight,
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputElements
) {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebra::"
    "computeKsl2TriplesGetOppositeElementsAll"
  );
  outputElements.setSize(0);
  for (int i = 0; i < this->modules.size; i ++) {
    for (int j = 0; j < this->weightsModulesPrimal[i].size; j ++) {
      if ((weight + this->weightsModulesPrimal[i][j]).isEqualToZero()
      ) {
        for (int k = 0; k < this->modules[i].size; k ++) {
          outputElements.addOnTop(this->modules[i][k][j]);
          if (
            !(
              weight +
              this->getPrimalWeightFirstGenerator(
                this->modules[i][k][j]
              )
            ).isEqualToZero()
          ) {
            global.fatal
            << "Element this->modules[i][k][j] does not have "
            << "the primal weight it is supposed to have. "
            << global.fatal;
          }
        }
      }
    }
  }
}

bool CandidateSemisimpleSubalgebra::computeKsl2TripleSetUpAndSolveSystem(
  const ElementSemisimpleLieAlgebra<AlgebraicNumber>& element,
  const List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >&
  fIsLinearCombinationOf,
  ElementSemisimpleLieAlgebra<AlgebraicNumber>& outputF
) {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebra::computeKsl2TriplesetUpAndSolveSystem"
  );
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > eCopy;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > hElement;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > fElement;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > basisElement;
  eCopy = element;
  this->getAmbientSemisimpleLieAlgebra().getGenericElementCartan(eCopy, 0);
  fElement.makeZero();
  Polynomial<AlgebraicNumber> polynomial;
  for (int i = 0; i < fIsLinearCombinationOf.size; i ++) {
    basisElement = fIsLinearCombinationOf[i];
    polynomial.makeMonomial(
      i + this->getAmbientSemisimpleLieAlgebra().getRank(), 1, 1
    );
    basisElement *= polynomial;
    fElement += basisElement;
  }
  if (
    !this->getAmbientSemisimpleLieAlgebra().attemptFindingHEF(
      eCopy, eCopy, fElement, nullptr
    )
  ) {
    return false;
  }
  outputF = fElement;
  return true;
}

void CandidateSemisimpleSubalgebra::computeKsl2Triples() {
  STACK_TRACE("CandidateSemisimpleSubalgebra::computeKsl2Triples");
  if (!this->owner->flagComputeNilradicals) {
    return;
  }
  this->modulesSl2Opposite.setSize(this->modules.size);
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >
  fMustBeLinearCombinationOf;
  for (int i = 0; i < this->modules.size; i ++) {
    this->modulesSl2Opposite[i].setSize(this->modules[i].size);
    for (int j = 0; j < this->modules[i].size; j ++) {
      this->modulesSl2Opposite[i][j].setSize(
        this->modules[i][j].size
      );
      for (
        int k = 0; k < this->modulesSl2Opposite[i][j].size; k ++
      ) {
        this->computeKsl2TriplesGetOppositeElementsInOppositeModule(
          this->weightsModulesPrimal[i][k],
          this->modulesIsotypicallyMerged[
            this->oppositeModulesByChar[i][0]
          ],
          fMustBeLinearCombinationOf
        );
        if (
          this->computeKsl2TripleSetUpAndSolveSystem(
            this->modules[i][j][k],
            fMustBeLinearCombinationOf,
            this->modulesSl2Opposite[i][j][k]
          )
        ) {
          continue;
        }
        this->computeKsl2TriplesGetOppositeElementsAll(
          this->weightsModulesPrimal[i][k], fMustBeLinearCombinationOf
        );
        this->computeKsl2TripleSetUpAndSolveSystem(
          this->modules[i][j][k],
          fMustBeLinearCombinationOf,
          this->modulesSl2Opposite[i][j][k]
        );
      }
    }
  }
}

int CandidateSemisimpleSubalgebra::getRank() const {
  return this->weylNonEmbedded->dynkinType.getRank();
}

int CandidateSemisimpleSubalgebra::getPrimalRank() const {
  if (!this->flagCentralizerIsWellChosen) {
    return - 1;
  }
  int rank = 0;
  if (!this->centralizerRank.isSmallInteger(&rank)) {
    return - 1;
  }
  return rank + this->weylNonEmbedded->getDimension();
}

void NilradicalCandidate::checkInitialization() const {
  if (this->owner == nullptr) {
    global.fatal
    << "NilradicalCandidate with non-initialized owner"
    << global.fatal;
  }
}

Vector<Rational> NilradicalCandidate::getConeStrongIntersectionWeight() const {
  Vector<Rational> weight;
  weight.makeZero(this->owner->getPrimalRank());
  for (int i = 0; i < this->nilradicalWeights.size; i ++) {
    weight += this->nilradicalWeights[i] * this->coneStrongIntersection[i];
  }
  return weight;
}

bool NilradicalCandidate::isCommutingSelectionNilradicalElements(
  Selection& inputNilradicalSelection
) {
  if (inputNilradicalSelection.cardinalitySelection == 1) {
    return true;
  }
  ElementSemisimpleLieAlgebra<AlgebraicNumber> mustBeZero;
  for (int i = 0; i < inputNilradicalSelection.cardinalitySelection; i ++) {
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& leftElement =
    this->nilradical[inputNilradicalSelection.elements[i]];
    for (
      int j = 0; j < inputNilradicalSelection.cardinalitySelection; j ++
    ) {
      if (i == j) {
        continue;
      }
      ElementSemisimpleLieAlgebra<AlgebraicNumber>& rightElementPositive =
      this->nilradical[inputNilradicalSelection.elements[j]];
      ElementSemisimpleLieAlgebra<AlgebraicNumber>& rightElementNegative =
      this->nilradicalElementOpposites[
        inputNilradicalSelection.elements[j]
      ];
      this->owner->getAmbientSemisimpleLieAlgebra().lieBracket(
        leftElement, rightElementPositive, mustBeZero
      );
      if (!mustBeZero.isEqualToZero()) {
        return false;
      }
      if (rightElementNegative.isEqualToZero()) {
        return false;
      }
      this->owner->getAmbientSemisimpleLieAlgebra().lieBracket(
        leftElement, rightElementNegative, mustBeZero
      );
      if (!mustBeZero.isEqualToZero()) {
        return false;
      }
    }
  }
  for (int i = 0; i < inputNilradicalSelection.cardinalitySelection; i ++) {
    for (
      int j = i + 1; j < inputNilradicalSelection.cardinalitySelection; j ++
    ) {
      if (
        this->owner->getScalarSubalgebra(
          this->nilradicalWeights[inputNilradicalSelection.elements[i]],
          this->nilradicalWeights[inputNilradicalSelection.elements[j]]
        ) <
        0
      ) {
        return false;
      }
    }
  }
  return true;
}

Vector<Rational> NilradicalCandidate::getNilradicalLinearCombination() const {
  Vector<Rational> nilradLinearCombination = this->coneStrongIntersection;
  nilradLinearCombination.setSize(this->nilradicalWeights.size);
  return nilradLinearCombination;
}

void NilradicalCandidate::computeParabolicACExtendsToParabolicAC() {
  STACK_TRACE("NilradicalCandidate::computeParabolicACExtendsToParabolicAC");
  Vector<Rational> projectionRoot;
  WeylGroupData& weyl = this->owner->owner->owner->weylGroup;
  this->leviRootsAmbient.reserve(weyl.rootSystem.size);
  this->leviRootsSmallPrimalFundamentalCoordinates.reserve(
    weyl.rootSystem.size
  );
  Vectors<Rational> rootSystemProjections;
  Vectors<Rational> conesCombination;
  rootSystemProjections.setSize(weyl.rootSystem.size);
  for (int i = 0; i < weyl.rootSystem.size; i ++) {
    this->owner->getPrimalWeightProjectionFundamentalCoordinates(
      weyl.rootSystem[i], rootSystemProjections[i]
    );
  }
  conesCombination = this->nilradicalWeights;
  for (int i = 0; i < this->nonFernandoKacHighestWeights.size; i ++) {
    conesCombination.addOnTop(- this->nonFernandoKacHighestWeights[i]);
  }
  this->coneSeparatingNormal.perturbNormalRelativeToVectorsInGeneralPosition(
    conesCombination, rootSystemProjections
  );
  for (int i = 0; i < weyl.rootSystem.size; i ++) {
    this->owner->getPrimalWeightProjectionFundamentalCoordinates(
      weyl.rootSystem[i], projectionRoot
    );
    if (projectionRoot.scalarEuclidean(this->coneSeparatingNormal) == 0) {
      this->leviRootsAmbient.addOnTop(weyl.rootSystem[i]);
      if (
        this->owner->rootSystemCentralizerPrimalCoordinates.contains(
          projectionRoot
        )
      ) {
        this->leviRootsSmallPrimalFundamentalCoordinates.addOnTop(
          projectionRoot
        );
      }
    }
  }
  Vectors<Rational> tempVs = this->leviRootsAmbient;
  this->leviDiagramAmbient.computeDiagramTypeModifyInput(tempVs, weyl);
  this->leviDiagramSmall.computeDiagramTypeModifyInputRelative(
    this->leviRootsSmallPrimalFundamentalCoordinates,
    this->owner->rootSystemCentralizerPrimalCoordinates,
    this->owner->bilinearFormFundamentalPrimal
  );
  bool ambientLeviHasBDGE = false;
  for (
    int i = 0; i < this->leviDiagramAmbient.simpleComponentTypes.size; i ++
  ) {
    DynkinSimpleType& currentType =
    this->leviDiagramAmbient.simpleComponentTypes[i];
    if (
      currentType.letter == 'D' ||
      currentType.letter == 'G' ||
      currentType.letter == 'E' ||
      currentType.letter == 'E'
    ) {
      ambientLeviHasBDGE = true;
      break;
    }
    if (currentType.letter == 'B' && currentType.rank > 2) {
      ambientLeviHasBDGE = true;
      break;
    }
  }
  bool smallLeviHasBDGE = false;
  for (
    int i = 0; i < this->leviDiagramSmall.simpleComponentTypes.size; i ++
  ) {
    DynkinSimpleType& currentType =
    this->leviDiagramSmall.simpleComponentTypes[i];
    if (
      currentType.letter == 'D' ||
      currentType.letter == 'G' ||
      currentType.letter == 'E' ||
      currentType.letter == 'E'
    ) {
      smallLeviHasBDGE = true;
      break;
    }
    if (currentType.letter == 'B' && currentType.rank > 2) {
      smallLeviHasBDGE = true;
      break;
    }
  }
  this->flagRestrictedCentralizerConditionHoldS = !smallLeviHasBDGE;
  if (!smallLeviHasBDGE) {
    this->flagParabolicACExtendsToParabolicAC = !ambientLeviHasBDGE;
  }
  if (smallLeviHasBDGE) {
    this->flagParabolicACExtendsToParabolicAC = true;
    if (!ambientLeviHasBDGE) {
      global.fatal
      << "This is a mathematical error. "
      << "Something is very wrong. "
      << "The ambient parabolic subalgebra has components "
      << "of type A and C, but intesects the centralizer "
      << "in components of type B and D. "
      << "This must be impossible according to "
      << "the PSZ paper and the restriction of "
      << "Fernando's theorem to the centralizer. "
      << global.fatal;
    }
  }
}

bool NilradicalCandidate::tryFindingLInfiniteRelations() {
  STACK_TRACE("NilradicalCandidate::tryFindingLInfiniteRelations");
  Vector<Rational> betterIntersection;
  this->nilradicalSubselection.initialize(this->nilradicalWeights.size);
  this->flagComputedRelativelyStrongIntersections = false;
  for (int i = 1; i < this->nilradicalWeights.size && i < 5; i ++) {
    LargeInteger numberOfCycles =
    MathRoutines::nChooseK(this->nilradicalWeights.size, i);
    this->nilradicalSubselection.initSelectionFixedCardinality(i);
    for (
      int j = 0; j < numberOfCycles;
      j ++,
      this->nilradicalSubselection.
      incrementSelectionFixedCardinalityReturnFalseIfPastLast(i)
    ) {
      if (
        this->isCommutingSelectionNilradicalElements(
          this->nilradicalSubselection
        )
      ) {
        this->nilradicalWeights.subSelection(
          this->nilradicalSubselection, this->nilradicalSubsetWeights
        );
        if (
          this->nilradicalSubsetWeights.conesIntersect(
            this->nilradicalSubsetWeights,
            this->nonFernandoKacHighestWeightsStronglyTwoSided,
            &betterIntersection,
            nullptr
          )
        ) {
          betterIntersection.scaleNormalizeFirstNonZero();
          this->coneStrongIntersection.makeZero(
            this->nilradicalWeights.size +
            this->nonFernandoKacHighestWeightsStronglyTwoSided.size
          );
          this->coneRelativelyStrongIntersection.setSize(0);
          for (
            int k = 0; k < this->nilradicalSubselection.cardinalitySelection; k
            ++
          ) {
            this->coneStrongIntersection[
              this->nilradicalSubselection.elements[k]
            ] =
            betterIntersection[k];
            this->coneRelativelyStrongIntersection.addOnTop(
              betterIntersection[k]
            );
          }
          this->nilradical.subSelection(
            this->nilradicalSubselection, this->nilradicalSubset
          );
          this->nilradicalWeights.subSelection(
            this->nilradicalSubselection, this->nilradicalSubsetWeights
          );
          this->nonFernandoKacHighestWeightVectorsStrongRelativeToSubset.
          setSize(0);
          this->nonFernandoKacHighestWeightVectorsStrongRelativeToSubsetWeights
          .setSize(0);
          for (
            int k = 0; k < this->nonFernandoKacHighestWeightsStronglyTwoSided.
            size; k ++
          ) {
            this->coneStrongIntersection[k + this->nilradicalWeights.size] =
            betterIntersection[k + i];
            if (betterIntersection[k + i] != 0) {
              this->nonFernandoKacHighestWeightVectorsStrongRelativeToSubset.
              addOnTop(
                this->nonFernandoKacHighestVectorsStronglyTwoSided[k]
              );
              this->
              nonFernandoKacHighestWeightVectorsStrongRelativeToSubsetWeights.
              addOnTop(
                this->nonFernandoKacHighestWeightsStronglyTwoSided[k]
              );
              this->coneRelativelyStrongIntersection.addOnTop(
                betterIntersection[k + i]
              );
            }
          }
          return true;
        }
      }
    }
  }
  //  this->ConeStrongIntersection.makeZero(this->ConeStrongIntersection.size);
  ProgressReport report;
  this->flagComputedRelativelyStrongIntersections = true;
  for (int i = 1; i < this->nilradicalWeights.size + 1 && i < 5; i ++) {
    LargeInteger numberOfCycles =
    MathRoutines::nChooseK(this->nilradicalWeights.size, i);
    this->nilradicalSubselection.initSelectionFixedCardinality(i);
    //    this->FKnilradicalLog+= out.str();
    for (
      int j = 0; j < numberOfCycles;
      j ++,
      this->nilradicalSubselection.
      incrementSelectionFixedCardinalityReturnFalseIfPastLast(i)
    ) {
      std::stringstream out;
      out
      << "<br>Trying "
      << i + 1
      << "-tuples (up to 5-tuples): "
      << j + 1
      << " out of "
      << numberOfCycles
      << " cycles to process. ";
      report.report(out.str());
      if (
        this->isCommutingSelectionNilradicalElements(
          this->nilradicalSubselection
        )
      ) {
        this->computeTheTwoConesRelativeToNilradicalSubset();
        if (
          this->nilradicalSubsetWeights.conesIntersect(
            this->nilradicalSubsetWeights,
            this->
            nonFernandoKacHighestWeightVectorsStrongRelativeToSubsetWeights,
            &this->coneRelativelyStrongIntersection,
            nullptr
          )
        ) {
          this->coneRelativelyStrongIntersection.scaleNormalizeFirstNonZero();
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
  for (int i = 0; i < this->nilradicalSelection.size; i ++) {
    if (
      this->nilradicalSelection[i] == 1 &&
      !this->owner->primalSubalgebraModules.contains(i)
    ) {
      for (
        int j = 0; j < this->owner->nilradicalPairingTable[moduleIndex][i].
        size; j ++
      ) {
        if (
          this->nilradicalSelection[
            this->owner->nilradicalPairingTable[moduleIndex][i][j]
          ] !=
          1
        ) {
          return false;
        }
        if (
          this->owner->primalSubalgebraModules.contains(
            this->owner->nilradicalPairingTable[moduleIndex][i][j]
          )
        ) {
          return false;
        }
      }
    }
  }
  return true;
}

bool NilradicalCandidate::isStronglySingularRelativeToSubset(
  int nonFernandoKacWeightIndex
) {
  STACK_TRACE("NilradicalCandidate::isStronglySingularRelativeToSubset");
  this->checkInitialization();
  ElementSemisimpleLieAlgebra<AlgebraicNumber> mustBeZero;
  for (
    int j = 0; j < this->nilradicalSubselection.cardinalitySelection; j ++
  ) {
    this->owner->owner->owner->lieBracket(
      this->nilradical[this->nilradicalSubselection.elements[j]],
      this->nonFernandoKacHighestWeightVectors[nonFernandoKacWeightIndex],
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
  this->flagParabolicACExtendsToParabolicAC = false;
  this->flagLinfiniteRelFound = false;
}

void NilradicalCandidate::reset() {
  this->nilradicalWeights.setSize(0);
  this->nonFernandoKacHighestWeights.setSize(0);
  this->nonFernandoKacHighestWeightsStronglyTwoSided.setSize(0);
  this->nonFernandoKacHighestVectorsStronglyTwoSided.setSize(0);
  this->nilradical.setSize(0);
  this->nilradicalElementOpposites.setSize(0);
  this->ownerModulesNilradicalElements.setSize(0);
  this->ownerModulesNonFernandoKacHighestWeightVectors.setSize(0);
  this->nilradicalSubselection.initialize(0);
  this->nilradicalSubset.setSize(0);
  this->nonFernandoKacHighestWeightVectorsStrongRelativeToSubset.setSize(0);
  this->nilradicalSubsetWeights.setSize(0);
  this->nonFernandoKacHighestWeightVectorsStrongRelativeToSubsetWeights.setSize
  (0);
}

void NilradicalCandidate::computeTheTwoConesRelativeToNilradicalSubset() {
  STACK_TRACE(
    "NilradicalCandidate::computeTheTwoConesRelativeToNilradicalSubset"
  );
  this->nilradical.subSelection(
    this->nilradicalSubselection, this->nilradicalSubset
  );
  this->nilradicalWeights.subSelection(
    this->nilradicalSubselection, this->nilradicalSubsetWeights
  );
  this->nonFernandoKacHighestWeightVectorsStrongRelativeToSubset.setSize(0);
  this->nonFernandoKacHighestWeightVectorsStrongRelativeToSubsetWeights.setSize
  (0);
  for (int i = 0; i < this->nonFernandoKacHighestWeights.size; i ++) {
    if (this->isStronglySingularRelativeToSubset(i)) {
      this->nonFernandoKacHighestWeightVectorsStrongRelativeToSubsetWeights.
      addOnTop(this->nonFernandoKacHighestWeights[i]);
      this->nonFernandoKacHighestWeightVectorsStrongRelativeToSubset.addOnTop(
        this->nonFernandoKacHighestWeightVectors[i]
      );
    }
  }
}

void NilradicalCandidate::computeTheTwoCones() {
  STACK_TRACE("NilradicalCandidate::computeTheTwoCones");
  this->checkInitialization();
  this->reset();
  for (int i = 0; i < this->nilradicalSelection.size; i ++) {
    if (!this->owner->primalSubalgebraModules.contains(i)) {
      if (this->nilradicalSelection[i] == 0) {
        for (int k = 0; k < this->owner->modules[i].size; k ++) {
          this->nonFernandoKacHighestWeights.addOnTop(
            this->owner->highestWeightsPrimal[i]
          );
          this->nonFernandoKacHighestWeightVectors.addOnTop(
            this->owner->highestVectors[i][k]
          );
          this->ownerModulesNonFernandoKacHighestWeightVectors.addOnTop(i);
          if (this->isStronglySingular(i)) {
            this->nonFernandoKacHighestWeightsStronglyTwoSided.addOnTop(
              this->owner->highestWeightsPrimal[i]
            );
            this->nonFernandoKacHighestVectorsStronglyTwoSided.addOnTop(
              this->owner->highestVectors[i][k]
            );
          }
        }
      } else if (this->nilradicalSelection[i] == 1) {
        for (int k = 0; k < this->owner->modules[i].size; k ++) {
          this->nilradicalWeights.addListOnTop(
            this->owner->weightsModulesPrimal[i]
          );
          this->nilradical.addListOnTop(this->owner->modules[i][k]);
          this->nilradicalElementOpposites.addListOnTop(
            this->owner->modulesSl2Opposite[i][k]
          );
          for (
            int l = 0; l < this->owner->modules[i][k].size; l ++
          ) {
            this->ownerModulesNilradicalElements.addOnTop(i);
          }
        }
      }
    }
  }
  if (
    this->ownerModulesNilradicalElements.size != this->nilradical.size ||
    this->nonFernandoKacHighestWeights.size !=
    this->ownerModulesNonFernandoKacHighestWeightVectors.size
  ) {
    global.fatal
    << "Sizes of indexing arrasy in "
    << "Fernando Kac nilradical candidate don't match. "
    << global.fatal;
  }
}

void CandidateSemisimpleSubalgebra::enumerateAllNilradicals() {
  STACK_TRACE("CandidateSemisimpleSubalgebra::enumerateAllNilradicals");
  ProgressReport report;
  ProgressReport report2;
  std::stringstream reportStream;
  reportStream
  << "Enumerating recursively nilradicals of type "
  << this->toStringTypeAndHs()
  << "...";
  report.report(reportStream.str());
  this->fernandoKacNilradicalCandidates.setSize(0);
  List<int> selection;
  this->recursionDepthCounterForNilradicalGeneration = 0;
  // 0 stands for not selected, 1 for selected from nilradical, 3 for selected
  // from semisimple part, 2 stands for unknown.
  selection.initializeFillInObject(this->nilradicalPairingTable.size, 2);
  for (int i = 0; i < this->primalSubalgebraModules.size; i ++) {
    selection[this->primalSubalgebraModules[i]] = 1;
  }
  std::stringstream out;
  if (
    selection.size != this->nilradicalPairingTable.size ||
    selection.size != this->modulesIsotypicallyMerged.size
  ) {
    global.fatal
    << "Selection has "
    << selection.size
    << ", nilraidcal pairing table has "
    << this->nilradicalPairingTable.size
    << " elements and modules isotypically merged has "
    << this->modulesIsotypicallyMerged.size
    << " elements."
    << global.fatal;
  }
  this->enumerateNilradicalsRecursively(selection, &out);
  if (this->fernandoKacNilradicalCandidates.size < 1) {
    global.fatal
    << "While enumerating nilradicals of "
    << this->weylNonEmbedded->dynkinType.toString()
    << " got 0 nilradical candidates which is impossible ("
    << "the zero nilradical is always possible). "
    << global.fatal;
  }
  for (int i = 0; i < this->fernandoKacNilradicalCandidates.size; i ++) {
    std::stringstream reportStream2;
    reportStream2
    << "Processing nilradical: "
    << i + 1
    << " out of "
    << this->fernandoKacNilradicalCandidates.size;
    report2.report(reportStream2.str());
    this->fernandoKacNilradicalCandidates[i].processMe();
  }
  this->numberOfConeIntersections = 0;
  this->numberOfCasesNoLInfiniteRelationFound = 0;
  this->numberOfBadParabolics = 0;
  this->numberOfCentralizerConditionFailsConeConditionHolds = 0;
  for (int i = 0; i < this->fernandoKacNilradicalCandidates.size; i ++) {
    if (
      this->fernandoKacNilradicalCandidates[i].flagNilradicalConesIntersect
    ) {
      this->numberOfConeIntersections ++;
      if (
        !this->fernandoKacNilradicalCandidates[i].flagLinfiniteRelFound
      ) {
        this->numberOfCasesNoLInfiniteRelationFound ++;
      }
    } else {
      if (
        !this->fernandoKacNilradicalCandidates[i].
        flagParabolicACExtendsToParabolicAC
      ) {
        this->numberOfBadParabolics ++;
      }
      if (
        !this->fernandoKacNilradicalCandidates[i].
        flagRestrictedCentralizerConditionHoldS
      ) {
        this->numberOfCentralizerConditionFailsConeConditionHolds ++;
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

template <class Coefficient>
void Vector<Coefficient>::perturbNormalRelativeToVectorsInGeneralPosition(
  const Vectors<Rational>& nonStrictConeNonPositiveScalar,
  const List<Vector<Rational> >& vectorsToBeInGeneralPosition
) {
  STACK_TRACE("Vector::perturbNormalRelativeToVectorsInGeneralPosition");
  for (int i = 0; i < nonStrictConeNonPositiveScalar.size; i ++) {
    if (this->scalarEuclidean(nonStrictConeNonPositiveScalar[i]) < 0) {
      global.fatal
      << "The splitting normal "
      << this->toString()
      << " is supposed to have non-negative scalar product with the vector "
      << nonStrictConeNonPositiveScalar[i].toString()
      << ", but it doesn't."
      << global.fatal;
    }
  }
  Vector<Rational> oldThis = *this;
  Cone cone;
  cone.createFromVertices(nonStrictConeNonPositiveScalar);
  Coefficient scalarThis;
  Coefficient scalarOther;
  Coefficient scale;
  Vector<Rational> currentModifier;
  Vectors<Rational> allVectors = cone.vertices;
  allVectors.addListOnTop(vectorsToBeInGeneralPosition);
  for (int i = 0; i < vectorsToBeInGeneralPosition.size; i ++) {
    bool foundModifier = false;
    if (this->scalarEuclidean(vectorsToBeInGeneralPosition[i]) == 0) {
      for (int j = 0; j < cone.walls.size; j ++) {
        if (
          cone.walls[j].normal.scalarEuclidean(
            vectorsToBeInGeneralPosition[i]
          ) !=
          0
        ) {
          foundModifier = true;
          currentModifier = cone.walls[j].normal;
          break;
        }
      }
      if (!foundModifier) {
        continue;
      }
      scale = 1;
      for (int j = 0; j < vectorsToBeInGeneralPosition.size; j ++) {
        scalarThis = this->scalarEuclidean(vectorsToBeInGeneralPosition[j]);
        scalarOther =
        currentModifier.scalarEuclidean(vectorsToBeInGeneralPosition[j]);
        if (scalarOther * scalarThis < 0) {
          scale =
          MathRoutines::minimum(scale, -(scalarThis / scalarOther) / 2);
        }
      }
    }
    if (!foundModifier) {
      continue;
    }
    *this += currentModifier * scale;
    for (int k = 0; k < nonStrictConeNonPositiveScalar.size; k ++) {
      if (this->scalarEuclidean(nonStrictConeNonPositiveScalar[k]) < 0) {
        global.fatal
        << "<br>During perturbation, the splitting normal "
        << this->toString()
        << " is supposed to have non-negative scalar product with the vector "
        << nonStrictConeNonPositiveScalar[k].toString()
        << ", but it doesn't."
        << global.fatal;
      } else {
        if (
          this->scalarEuclidean(nonStrictConeNonPositiveScalar[k]) == 0 &&
          oldThis.scalarEuclidean(nonStrictConeNonPositiveScalar[k]) > 0
        ) {
          global.fatal
          << "<br>During perturbation, the splitting normal "
          << this->toString()
          << " lost positive scalar product with "
          << nonStrictConeNonPositiveScalar[k].toString()
          << "."
          << global.fatal;
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
  this->flagParabolicACExtendsToParabolicAC = true;
  this->computeTheTwoCones();
  this->flagNilradicalConesIntersect =
  this->nilradicalWeights.conesIntersect(
    this->nilradicalWeights,
    this->nonFernandoKacHighestWeights,
    &this->coneIntersection,
    &this->coneSeparatingNormal
  );
  this->flagLinfiniteRelFound = false;
  if (!this->flagNilradicalConesIntersect) {
    if (
      this->nonFernandoKacHighestWeights.size == 0 &&
      this->nilradicalWeights.size == 0
    ) {
      this->flagRestrictedCentralizerConditionHoldS = true;
      this->flagParabolicACExtendsToParabolicAC = true;
      return;
    }
    this->computeParabolicACExtendsToParabolicAC();
    return;
  }
  this->flagNilradicalConesStronglyIntersect =
  this->nilradicalWeights.conesIntersect(
    this->nilradicalWeights,
    this->nonFernandoKacHighestWeightsStronglyTwoSided,
    &this->coneStrongIntersection,
    nullptr
  );
  if (this->flagNilradicalConesStronglyIntersect) {
    this->flagLinfiniteRelFound = this->tryFindingLInfiniteRelations();
  }
}

std::string CandidateSemisimpleSubalgebra::toStringNilradicalSelection(
  const List<int>& selection
) {
  Vector<Rational> undecided;
  Vector<Rational> included;
  Vector<Rational> excluded;
  Vector<Rational> workingVector;
  included.makeZero(this->nilradicalPairingTable.size);
  undecided.makeZero(this->nilradicalPairingTable.size);
  excluded.makeZero(this->nilradicalPairingTable.size);
  for (int i = 0; i < selection.size; i ++) {
    workingVector.makeEi(this->nilradicalPairingTable.size, i);
    if (selection[i] == 0) {
      excluded += workingVector;
    } else if (selection[i] == 1) {
      included += workingVector;
    } else {
      undecided += workingVector;
    }
  }
  std::stringstream out;
  out
  << "included modules: "
  << included.toStringLetterFormat("V")
  << ", excluded modules: "
  << excluded.toStringLetterFormat("V")
  << ", undecided modulues: "
  << undecided.toStringLetterFormat("V");
  return out.str();
}

bool CandidateSemisimpleSubalgebra::
isPossibleNilradicalCarryOutSelectionImplications(
  List<int>& selection, std::stringstream* logStream
) {
  if (this->fernandoKacNilradicalCandidates.size > 100) {
    if (logStream != nullptr) {
      std::string currentString = logStream->str();
      if (currentString.size() > 2) {
        if (
          currentString[currentString.size() - 1] != '.' ||
          currentString[currentString.size() - 2] != '.'
        ) {
          *logStream
          << "<br>... number of nilradicals found exceeds 100, "
          << "I will no longer log the computations ...";
        }
      }
    }
    logStream = nullptr;
  }
  if (logStream != nullptr) {
    *logStream
    << "<br>********Considering selection "
    << this->toStringNilradicalSelection(selection)
    << "********";
  }
  HashedList<int, HashFunctions::hashFunction> selectedIndices;
  for (int i = 0; i < selection.size; i ++) {
    if (selection[i] == 1) {
      selectedIndices.addOnTop(i);
    }
  }
  MemorySaving<List<int> > oldSelection;
  if (logStream != nullptr) {
    oldSelection.getElement() = selection;
  }
  this->extendNilradicalSelectionToMultFreeOverSemisimplePartSubalgebra(
    selectedIndices
  );
  for (int i = 0; i < selection.size; i ++) {
    if (selection[i] == 0 && selectedIndices.contains(i)) {
      if (logStream != nullptr) {
        *logStream
        << " <br>The selection "
        << this->toStringNilradicalSelection(selection)
        << " is contradictory, as the only way to extend it to a subalgebra "
        << "(i.e., closed under Lie bracket) is by requesting that module V_{"
        << i + 1
        << "} be included, but at the same time we have already "
        << "decided to exclude that module in one of our preceding choices. ";
      }
      return false;
    }
  }
  for (int i = 0; i < selectedIndices.size; i ++) {
    selection[selectedIndices[i]] = 1;
  }
  if (logStream != nullptr) {
    if (oldSelection.getElement() != selection) {
      *logStream
      << "<br>In order to be closed w.r.t. the "
      << "Lie bracket, I extend the nilradical selection "
      << this->toStringNilradicalSelection(oldSelection.getElement())
      << " to the following. "
      << this->toStringNilradicalSelection(selection);
    }
  }
  for (int i = 0; i < selectedIndices.size; i ++) {
    for (
      int j = 0; j < this->oppositeModulesByStructure[selectedIndices[i]].size;
      j ++
    ) {
      if (
        selection[
          this->oppositeModulesByStructure[selectedIndices[i]][j]
        ] ==
        1
      ) {
        if (logStream != nullptr) {
          *logStream
          << "<br>The subalgebra selection "
          << this->toStringNilradicalSelection(selection)
          << " contains opposite modules and is therefore not allowed. ";
        }
        return false;
      }
      selection[
        this->oppositeModulesByStructure[selectedIndices[i]][j]
      ] =
      0;
    }
  }
  return true;
}

void CandidateSemisimpleSubalgebra::
extendNilradicalSelectionToMultFreeOverSemisimplePartSubalgebra(
  HashedList<int, HashFunctions::hashFunction>& inputOutput
) {
  for (int i = 0; i < inputOutput.size; i ++) {
    for (int j = 0; j < inputOutput.size; j ++) {
      for (
        int k = 0; k < this->nilradicalPairingTable[inputOutput[i]][
          inputOutput[j]
        ].size; k ++
      ) {
        inputOutput.addOnTopNoRepetition(
          this->nilradicalPairingTable[inputOutput[i]][inputOutput[j]][k]
        );
      }
    }
  }
}

void CandidateSemisimpleSubalgebra::enumerateNilradicalsRecursively(
  List<int>& selection, std::stringstream* logStream
) {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebra::"
    "enumerateNilradicalsRecursively"
  );
  RecursionDepthCounter counter(
    &this->recursionDepthCounterForNilradicalGeneration
  );
  if (
    this->recursionDepthCounterForNilradicalGeneration >
    this->nilradicalPairingTable.size + 1
  ) {
    global.fatal
    << "<br>oh no... something went very wrong! "
    << "The nilradical generation recursion "
    << "depth cannot exceed the number of nilradicals! "
    << global.fatal;
  }
  ProgressReport report;
  if (report.tickAndWantReport()) {
    std::stringstream out;
    out
    << "Enumerating nilradicals: "
    << this->fernandoKacNilradicalCandidates.size
    << " found so far. ";
    report.report(out.str());
  }
  if (
    !this->isPossibleNilradicalCarryOutSelectionImplications(
      selection, logStream
    )
  ) {
    return;
  }
  List<int> newSelection;
  bool found = false;
  for (int i = 0; i < selection.size; i ++) {
    if (selection[i] == 2) {
      found = true;
      newSelection = selection;
      newSelection[i] = 0;
      this->enumerateNilradicalsRecursively(newSelection, logStream);
      newSelection = selection;
      newSelection[i] = 1;
      this->enumerateNilradicalsRecursively(newSelection, logStream);
      break;
      // newSelection
    }
  }
  if (found) {
    return;
  }
  NilradicalCandidate candidate;
  candidate.owner = this;
  candidate.nilradicalSelection = selection;
  this->fernandoKacNilradicalCandidates.addOnTop(candidate);
}

void CandidateSemisimpleSubalgebra::
computePrimalModuleDecompositionHighestWeights(
  HashedList<Vector<Rational> >& outputHighestWeightsCoordinates
) {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebra::"
    "computePrimalModuleDecompositionHighestWeightsOnly"
  );
  outputHighestWeightsCoordinates.clear();
  Vector<Rational> currentWeight;
  Vector<Rational> currentRootSpace;
  for (int i = 0; i < this->highestVectorsNonSorted.size; i ++) {
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& currentVector =
    this->highestVectorsNonSorted[i];
    for (int j = 0; j < currentVector.size(); j ++) {
      currentRootSpace =
      this->getAmbientSemisimpleLieAlgebra().getWeightOfGenerator(
        currentVector[j].generatorIndex
      );
      currentWeight.setSize(
        this->cartanElementsSubalgebra.size + this->cartanOfCentralizer.size
      );
      for (int k = 0; k < this->cartanElementsSubalgebra.size; k ++) {
        currentWeight[k] =
        this->getAmbientWeyl().rootScalarCartanRoot(
          currentRootSpace, this->cartanElementsSubalgebra[k]
        );
      }
      for (int k = 0; k < this->cartanOfCentralizer.size; k ++) {
        currentWeight[k + this->cartanElementsSubalgebra.size] =
        this->getAmbientWeyl().rootScalarCartanRoot(
          currentRootSpace, this->cartanOfCentralizer[k]
        );
      }
      outputHighestWeightsCoordinates.addOnTopNoRepetition(currentWeight);
    }
  }
  outputHighestWeightsCoordinates.quickSortAscending();
}

void CandidateSemisimpleSubalgebra::
computePrimalModuleDecompositionHighestWeightsAndHighestWeightVectors() {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebra::"
    "computePrimalModuleDecompositionHighestWeightsAndHighestWeightVectors"
  );
  HashedList<Vector<Rational> > weightsCartanRestrictedDualCoordinates;
  this->computePrimalModuleDecompositionHighestWeights(
    weightsCartanRestrictedDualCoordinates
  );
  this->computePrimalModuleDecompositionHighestWeightVectors(
    weightsCartanRestrictedDualCoordinates
  );
  this->computePrimalModuleDecompositionHighestWeightsLastPart();
}

bool CandidateSemisimpleSubalgebra::compareLeftGreaterThanRight(
  const Vector<Rational>& left, const Vector<Rational>& right
) {
  Vector<Rational> leftSSpart = left;
  Vector<Rational> rightSSpart = right;
  leftSSpart.setSize(this->cartanElementsSubalgebra.size);
  rightSSpart.setSize(this->cartanElementsSubalgebra.size);
  if (leftSSpart > rightSSpart) {
    return true;
  }
  if (rightSSpart > leftSSpart) {
    return false;
  }
  Vector<Rational> leftCpart = left;
  Vector<Rational> rightCpart = right;
  leftCpart.shiftToTheLeft(this->cartanElementsSubalgebra.size);
  rightCpart.shiftToTheLeft(this->cartanElementsSubalgebra.size);
  return leftCpart > rightCpart;
}

void CandidateSemisimpleSubalgebra::getWeightProjectionFundamentalCoordinates(
  const Vector<Rational>& inputAmbientWeight, Vector<Rational>& output
) const {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebra::"
    "getWeightProjectionFundamentalCoordinates"
  );
  this->checkFullInitialization();
  output.setSize(this->cartanElementsSubalgebra.size);
  for (int j = 0; j < this->cartanElementsSubalgebra.size; j ++) {
    output[j] =
    this->getAmbientWeyl().rootScalarCartanRoot(
      inputAmbientWeight, this->cartanElementsSubalgebra[j]
    ) *
    2 /
    this->subalgebraNonEmbeddedDefaultScale->weylGroup.cartanSymmetric(j, j);
  }
}

void CandidateSemisimpleSubalgebra::
getPrimalWeightProjectionFundamentalCoordinates(
  const Vector<Rational>& inputAmbientWeight, Vector<Rational>& output
) const {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebra::getPrimalWeightProjectionFundamentalCoordinates"
  );
  this->checkFullInitialization();
  output.setSize(
    this->cartanElementsSubalgebra.size + this->cartanOfCentralizer.size
  );
  for (int j = 0; j < this->cartanElementsSubalgebra.size; j ++) {
    output[j] =
    this->getAmbientWeyl().rootScalarCartanRoot(
      inputAmbientWeight, this->cartanElementsSubalgebra[j]
    ) *
    2 /
    this->subalgebraNonEmbeddedDefaultScale->weylGroup.cartanSymmetric(j, j);
  }
  for (int j = 0; j < this->cartanOfCentralizer.size; j ++) {
    output[j + this->cartanElementsSubalgebra.size] =
    this->getAmbientWeyl().rootScalarCartanRoot(
      inputAmbientWeight, this->cartanOfCentralizer[j]
    ) *
    2;
  }
}

void CandidateSemisimpleSubalgebra::
computePrimalModuleDecompositionHighestWeightsLastPart() {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebra::"
    "computePrimalModuleDecompositionHighestWeightsLastPart"
  );
  this->highestWeightsPrimalNonSorted.setSize(
    this->highestVectorsNonSorted.size
  );
  this->highestWeightsNonPrimalNonSorted.setSize(
    this->highestVectorsNonSorted.size
  );
  this->primalCharacter.makeZero();
  Weight<Rational> weight;
  weight.owner = nullptr;
  Vector<Rational> currentRoot;
  List<Vector<Rational> > sortingWeights;
  for (int i = 0; i < this->highestVectorsNonSorted.size; i ++) {
    currentRoot =
    this->getAmbientSemisimpleLieAlgebra().getWeightOfGenerator(
      this->highestVectorsNonSorted[i][0].generatorIndex
    );
    Vector<Rational>& currentHighestWeightPrimal =
    this->highestWeightsPrimalNonSorted[i];
    this->getPrimalWeightProjectionFundamentalCoordinates(
      currentRoot, currentHighestWeightPrimal
    );
    sortingWeights.addOnTop(currentHighestWeightPrimal);
  }
  sortingWeights.quickSortAscendingCustom(
    *this, &this->highestVectorsNonSorted
  );
  List<List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > > modulesComputer;
  HashedList<Vector<Rational> > highestWeights;
  modulesComputer.setExpectedSize(this->highestVectorsNonSorted.size);
  highestWeights.setExpectedSize(this->highestVectorsNonSorted.size);
  for (int i = 0; i < this->highestVectorsNonSorted.size; i ++) {
    currentRoot =
    this->getAmbientSemisimpleLieAlgebra().getWeightOfGenerator(
      this->highestVectorsNonSorted[i][0].generatorIndex
    );
    Vector<Rational>& currentHighestWeightRelative =
    this->highestWeightsNonPrimalNonSorted[i];
    Vector<Rational>& currentHighestWeightPrimal =
    this->highestWeightsPrimalNonSorted[i];
    this->getWeightProjectionFundamentalCoordinates(
      currentRoot, currentHighestWeightRelative
    );
    this->getPrimalWeightProjectionFundamentalCoordinates(
      currentRoot, currentHighestWeightPrimal
    );
    weight.weightFundamentalCoordinates = currentHighestWeightPrimal;
    this->primalCharacter.addMonomial(weight, 1);
    int moduleIndex = highestWeights.getIndex(currentHighestWeightPrimal);
    if (moduleIndex == - 1) {
      modulesComputer.setSize(modulesComputer.size + 1);
      modulesComputer.lastObject()->setSize(0);
      moduleIndex = highestWeights.size;
      highestWeights.addOnTop(currentHighestWeightPrimal);
    }
    modulesComputer[moduleIndex].addOnTop(
      this->highestVectorsNonSorted[i]
    );
  }
  this->highestVectors.setExpectedSize(
    this->primalCharacter.size() + this->owner->owner->getRank()
  );
  this->highestWeightsPrimal.setExpectedSize(
    this->primalCharacter.size() + this->owner->owner->getRank()
  );
  this->modules.setExpectedSize(
    this->primalCharacter.size() + this->owner->owner->getRank()
  );
  this->highestVectors.setSize(0);
  this->highestWeightsPrimal.clear();
  this->modules.setSize(0);
  this->primalSubalgebraModules.setSize(0);
  for (int i = 0; i < modulesComputer.size; i ++) {
    this->modules.setSize(this->modules.size + 1);
    this->highestVectors.setSize(this->modules.size);
    this->highestVectors.lastObject()->setSize(0);
    this->highestWeightsPrimal.addOnTop(highestWeights[i]);
    if (
      LinearCombination<ChevalleyGenerator, Rational>::
      vectorSpacesIntersectionIsNonTrivial(
        modulesComputer[i], this->basis
      )
    ) {
      LinearCombination<ChevalleyGenerator, AlgebraicNumber>::
      intersectVectorSpaces(
        modulesComputer[i],
        this->basis,
        *this->highestVectors.lastObject()
      );
      if (this->highestVectors.lastObject()->size != 1) {
        global.fatal
        << "Simple component "
        << "does not have one highest weight vector, but has instead: "
        << this->highestVectors.lastObject()->toStringCommaDelimited()
        << ". Obtained as intersection of: "
        << modulesComputer[i].toStringCommaDelimited()
        << " and "
        << this->basis.toStringCommaDelimited()
        << "."
        << global.fatal;
      }
      this->primalSubalgebraModules.addOnTop(this->modules.size - 1);
      this->modules.lastObject()->setSize(1);
      *this->modules.lastObject()->lastObject() =
      *this->highestVectors.lastObject();
      if (modulesComputer[i].size > 1) {
        this->modules.setSize(this->modules.size + 1);
        this->modules.lastObject()->setSize(modulesComputer[i].size - 1);
        this->highestVectors.setSize(this->modules.size);
        this->highestVectors.lastObject()->setSize(0);
        this->highestWeightsPrimal.addOnTop(highestWeights[i]);
        this->highestVectors.lastObject()->addListOnTop(
          this->highestVectors[this->highestVectors.size - 2]
        );
        for (int j = 0; j < modulesComputer[i].size; j ++) {
          this->highestVectors.lastObject()->addOnTop(
            modulesComputer[i][j]
          );
          if (
            LinearCombination<ChevalleyGenerator, Rational>::getRankElementSpan
            (*this->highestVectors.lastObject()) <
            this->highestVectors.lastObject()->size
          ) {
            this->highestVectors.lastObject()->removeLastObject();
          }
        }
        this->highestVectors.lastObject()->removeIndexSwapWithLast(0);
        if (
          this->highestVectors.lastObject()->size !=
          modulesComputer[i].size - 1
        ) {
          global.fatal
          << "Wrong number of hwv's: got "
          << this->highestVectors.lastObject()->size
          << ", must have "
          << modulesComputer[i].size - 1
          << ". "
          << global.fatal;
        }
        for (
          int j = 0; j < this->highestVectors.lastObject()->size; j ++
        ) {
          (*this->modules.lastObject())[j].setSize(1);
          (*this->modules.lastObject())[j][0] = (
            *this->highestVectors.lastObject()
          )[j];
        }
      }
    } else {
      *this->highestVectors.lastObject() = modulesComputer[i];
      this->modules.lastObject()->setSize(modulesComputer[i].size);
      for (
        int j = 0; j < this->highestVectors.lastObject()->size; j ++
      ) {
        (*this->modules.lastObject())[j].setSize(1);
        (*this->modules.lastObject())[j][0] = (
          *this->highestVectors.lastObject()
        )[j];
      }
    }
  }
  this->highestWeightsNonPrimal.setSize(this->modules.size);
  for (int i = 0; i < this->modules.size; i ++) {
    this->highestWeightsNonPrimal[i] =
    this->getNonPrimalWeightFirstGenerator(this->modules[i][0][0]);
  }
  this->subalgebraModules = this->primalSubalgebraModules;
  this->characterFormat.getElement().customPlusSign = "\\oplus ";
  int rank = this->weylNonEmbedded->getDimension();
  this->characterFormat.getElement().vectorSpaceEiBasisNames.setSize(
    rank + this->cartanOfCentralizer.size
  );
  for (
    int i = 0; i < this->characterFormat.getElement().vectorSpaceEiBasisNames.
    size; i ++
  ) {
    std::stringstream currentStream;
    if (i < rank) {
      currentStream << "\\omega_{" << i + 1 << "}";
    } else {
      currentStream << "\\psi";
      if (this->cartanOfCentralizer.size > 1) {
        currentStream << "_{" << i - rank + 1 << "}";
      }
    }
    this->characterFormat.getElement().vectorSpaceEiBasisNames[i] =
    currentStream.str();
  }
  int numberOfModules = 0;
  for (int i = 0; i < this->modules.size; i ++) {
    numberOfModules += this->modules[i].size;
  }
  for (int i = 0; i < this->modules.size; i ++) {
    for (int j = 0; j < this->modules[i].size; j ++) {
      if (this->modules[i][j].size != 1) {
        global.fatal << "Empty module! " << global.fatal;
      }
    }
  }
  if (this->primalCharacter.getCoefficientsSum() != numberOfModules) {
    global.fatal
    << "The sum of the coefficients of the primal character is "
    << this->primalCharacter.getCoefficientsSum().toString()
    << " but there are  "
    << numberOfModules
    << " modules. modulesComputer variable: "
    << modulesComputer.toString()
    << "<br>Candidate details: "
    << this->toString(nullptr, false)
    << global.fatal;
  }
}

void CandidateSemisimpleSubalgebra::
computePrimalModuleDecompositionHighestWeightVectors(
  HashedList<Vector<Rational> >& inputHighestWeights
) {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebra::"
    "computePrimalModuleDecompositionHighestWeightVectors"
  );
  this->checkConsistency();
  List<Matrix<AlgebraicNumber> > adjointActionsOfHs;
  Matrix<AlgebraicNumber> tempAd;
  Matrix<AlgebraicNumber> temp;
  Matrix<AlgebraicNumber> commonAd;
  Matrix<AlgebraicNumber> adIncludingCartanActions;
  for (int i = 0; i < this->positiveGenerators.size; i ++) {
    this->getAmbientSemisimpleLieAlgebra().getAdjoint(
      tempAd, this->positiveGenerators[i]
    );
    commonAd.appendMatrixToTheBottom(tempAd);
  }
  ElementSemisimpleLieAlgebra<AlgebraicNumber> element;
  Vectors<Rational> allHs;
  allHs.addListOnTop(this->cartanElementsSubalgebra);
  allHs.addListOnTop(this->cartanOfCentralizer);
  adjointActionsOfHs.setSize(allHs.size);
  for (int j = 0; j < allHs.size; j ++) {
    element.makeCartanGenerator(
      allHs[j], this->getAmbientSemisimpleLieAlgebra()
    );
    this->getAmbientSemisimpleLieAlgebra().getAdjoint(
      adjointActionsOfHs[j], element
    );
  }
  Vectors<AlgebraicNumber> currentHighestVectors;
  this->highestVectorsNonSorted.setSize(0);
  for (int i = 0; i < inputHighestWeights.size; i ++) {
    adIncludingCartanActions = commonAd;
    for (int j = 0; j < allHs.size; j ++) {
      tempAd = adjointActionsOfHs[j];
      temp.makeIdentityMatrix(
        this->getAmbientSemisimpleLieAlgebra().getNumberOfGenerators()
      );
      temp *= inputHighestWeights[i][j];
      tempAd -= temp;
      adIncludingCartanActions.appendMatrixToTheBottom(tempAd);
    }
    currentHighestVectors.setSize(0);
    adIncludingCartanActions.getZeroEigenSpace(currentHighestVectors);
    for (int j = 0; j < currentHighestVectors.size; j ++) {
      currentHighestVectors[j].scaleNormalizeFirstNonZero();
      element.assignVectorNegativeRootSpacesCartanPosistiveRootSpaces(
        currentHighestVectors[j],
        this->getAmbientSemisimpleLieAlgebra()
      );
      this->highestVectorsNonSorted.addOnTop(element);
    }
  }
}

bool SemisimpleSubalgebras::checkConsistencyHs() const {
  STACK_TRACE("SemisimpleSubalgebras::checkConsistencyHs");
  this->checkInitialization();
  for (int i = 0; i < this->subalgebras.values.size; i ++) {
    if (
      this->subalgebras.keys[i] !=
      this->subalgebras.values[i].cartanElementsSubalgebra
    ) {
      global.fatal
      << "List this->hElementsOfSubalgebras "
      << "does not match this->theSubalgebras. "
      << global.fatal;
    }
  }
  return true;
}

bool SemisimpleSubalgebras::checkInitialization() const {
  this->checkConsistency();
  if (this->owner == nullptr) {
    global.fatal << "No owner semisimple Lie algebra. " << global.fatal;
  }
  if (this->subalgebrasNonEmbedded == nullptr) {
    global.fatal
    << "No container for non-embedded subalgebras. "
    << global.fatal;
  }
  return true;
}

bool SemisimpleSubalgebras::checkConsistency() const {
  if (this->flagDeallocated) {
    global.fatal
    << "Use after free of semisimple subalgebras. "
    << global.fatal;
  }
  return true;
}

bool SemisimpleSubalgebras::checkAll() const {
  STACK_TRACE("SemisimpleSubalgebras::checkAll");
  for (int i = 0; i < this->subalgebras.size(); i ++) {
    this->subalgebras.values[i].checkAll();
  }
  return true;
}

void SemisimpleSubalgebras::initHookUpPointers(
  SemisimpleLieAlgebra& inputOwner,
  AlgebraicClosureRationals* field,
  MapReferences<DynkinType, SemisimpleLieAlgebra>* inputSubalgebrasNonEmbedded,
  ListReferences<SlTwoSubalgebras>* inputSl2sOfSubalgebras
) {
  (void) inputSl2sOfSubalgebras;
  this->owner = &inputOwner;
  this->slTwoSubalgebras.owner = &inputOwner;
  this->ownerField = field;
  this->subalgebrasNonEmbedded = inputSubalgebrasNonEmbedded;
}

void SemisimpleSubalgebras::resetPointers() {
  this->toStringExpressionString = nullptr;
  this->owner = nullptr;
  this->ownerField = nullptr;
  this->slTwoSubalgebras.owner = nullptr;
  this->flagRealizedAllCandidates = true;
  this->flagAttemptToSolveSystems = true;
  this->flagComputeModuleDecomposition = true;
  this->flagComputePairingTable = false;
  this->flagComputeNilradicals = false;
  this->flagProduceLaTeXTables = false;
  this->flagAttemptToAdjustCentralizers = true;
  this->flagRealForms = false;
  this->millisecondsComputationStart = - 1;
  this->millisecondsComputationEnd = - 1;
  this->numberOfAdditions = 0;
  this->numberOfMultiplications = 0;
  this->subalgebrasNonEmbedded = nullptr;
}

void SemisimpleSubalgebras::resetComputations() {
  this->subalgebrasNonDefaultCartanAndScale.clear();
  this->cachedDynkinIndicesSl2subalgebrasSimpleTypes.clear();
  this->cachedDynkinSimpleTypesWithComputedSl2Subalgebras.clear();
  this->orbits.clear();
  this->orbitHElementLengths.clear();
  this->orbitDynkinIndices.clear();
  this->maxStoredOrbitSize = - 1;
  this->currentSubalgebraChain.clear();
  this->currentNumberOfLargerTypesExplored.clear();
  this->currentNumberOfHCandidatesExplored.clear();
  this->currentPossibleLargerDynkinTypes.clear();
  this->currentRootInjections.clear();
  this->currentHCandidatesScaledToActByTwo.clear();
  this->slTwoSubalgebrasRealForm.clear();
  this->subalgebras.clear();
}

bool CandidateSemisimpleSubalgebra::attemptToSolveSystem() {
  STACK_TRACE("CandidateSemisimpleSubalgebra::attemptToSolveSystem");
  this->checkFullInitialization();
  this->transformedSystem = this->systemToSolve;
  ProgressReport report;
  std::stringstream reportstream;
  reportstream
  << "<hr>In order to realize type "
  << this->weylNonEmbedded->dynkinType.toString()
  << " in "
  << this->owner->owner->toStringLieAlgebraName()
  << " I need to solve the following system."
  << this->toStringSystemPart2();
  int maximumPolynomialDivisions = 300;
  int maximumMonomialOperations = 200000;
  if (
    this->owner->owner->toStringLieAlgebraName() == "C^{1}_5" ||
    this->owner->owner->toStringLieAlgebraName() == "C_5"
  ) {
    reportstream
    << "<hr>This algorithm is known to run very slow for C_5. "
    << "Reducing the computation limits. ";
    maximumPolynomialDivisions = 100;
    maximumMonomialOperations = 2000;
  }
  report.report(reportstream.str());
  PolynomialSystem<AlgebraicNumber> system;
  system.groebner.polynomialOrder.monomialOrder.setComparison(
    MonomialPolynomial::greaterThan_totalDegree_rightSmallerWins
  );
  system.groebner.maximumMonomialOperations = maximumMonomialOperations;
  system.groebner.maximumPolynomialDivisions = maximumPolynomialDivisions;
  system.algebraicClosure = this->owner->ownerField;
  system.solveSerreLikeSystem(this->transformedSystem);
  this->flagSystemSolved = system.flagSystemSolvedOverBaseField;
  this->flagSystemProvedToHaveNoSolution =
  system.flagSystemProvenToHaveNoSolution;
  this->flagSystemGroebnerBasisFound = this->flagSystemSolved;
  this->verifySolution(system);
  return !this->flagSystemProvedToHaveNoSolution;
}

bool CandidateSemisimpleSubalgebra::verifySolution(
  PolynomialSystem<AlgebraicNumber>& system
) {
  if (!this->flagSystemSolved) {
    return true;
  }
  this->negativeGenerators.setSize(this->unknownNegativeGenerators.size);
  this->positiveGenerators.setSize(this->unknownPositiveGenerators.size);
  PolynomialSubstitution<AlgebraicNumber> substitution;
  substitution.setSize(system.systemSolution.size);
  for (int i = 0; i < substitution.size; i ++) {
    substitution[i].makeConstant(system.systemSolution[i]);
  }
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > currentNegative;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > currentPositive;
  for (int i = 0; i < this->unknownNegativeGenerators.size; i ++) {
    currentNegative = this->unknownNegativeGenerators[i];
    currentPositive = this->unknownPositiveGenerators[i];
    currentNegative.substituteInCoefficients(substitution);
    currentPositive.substituteInCoefficients(substitution);
    this->negativeGenerators[i] = currentNegative;
    // <-implicit type conversion here, will crash if currentNegativeElement
    // has
    // non-const coefficients
    this->positiveGenerators[i] = currentPositive;
    // <-implicit type conversion here, will crash if currentNegElt has
    // non-const coefficients
  }
  if (!this->checkGeneratorsBracketToHs()) {
    global.fatal
    << "I just solved the "
    << "Serre-Like system governing the subalgebra embedding, "
    << "but the Lie brackets of the "
    << "resulting positive and negative "
    << "generators are not what they should be. "
    << "Something has gone very wrong. "
    << global.fatal;
  }
  return true;
}

void CandidateSemisimpleSubalgebra::
getGenericNegativeGeneratorLinearCombination(
  int indexNegativeGenerators,
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& output
) {
  int offsetIndex = 0;
  for (int i = 0; i < indexNegativeGenerators; i ++) {
    offsetIndex += this->involvedNegativeGenerators[i].size;
  }
  this->getGenericLinearCombination(
    this->totalUnknownsWithCentralizer,
    offsetIndex,
    this->involvedNegativeGenerators[indexNegativeGenerators],
    output
  );
}

void CandidateSemisimpleSubalgebra::
getGenericCartanCentralizerLinearCombination(
  int indexCartanCentralizerGenerator,
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& output
) {
  int offsetIndex =
  this->totalUnknownsNoCentralizer +
  indexCartanCentralizerGenerator * this->getAmbientWeyl().getDimension();
  List<ChevalleyGenerator> elementsOfCartan;
  elementsOfCartan.setSize(this->getAmbientWeyl().getDimension());
  for (int i = 0; i < elementsOfCartan.size; i ++) {
    elementsOfCartan[i].generatorIndex =
    this->getAmbientSemisimpleLieAlgebra().getNumberOfPositiveRoots() + i;
    elementsOfCartan[i].owner = &this->getAmbientSemisimpleLieAlgebra();
  }
  this->getGenericLinearCombination(
    this->totalUnknownsWithCentralizer,
    offsetIndex,
    elementsOfCartan,
    output
  );
}

void CandidateSemisimpleSubalgebra::
getGenericPositiveGeneratorLinearCombination(
  int indexPositiveGenerators,
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& output
) {
  int offsetIndex = 0;
  for (int i = 0; i < indexPositiveGenerators; i ++) {
    offsetIndex += this->involvedPositiveGenerators[i].size;
  }
  offsetIndex += this->totalUnknownsNoCentralizer / 2;
  this->getGenericLinearCombination(
    this->totalUnknownsWithCentralizer,
    offsetIndex,
    this->involvedPositiveGenerators[indexPositiveGenerators],
    output
  );
}

void CandidateSemisimpleSubalgebra::addToSystem(
  const ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >&
  elementThatMustVanish
) {
  Polynomial<AlgebraicNumber> polynomial;
  for (int i = 0; i < elementThatMustVanish.size(); i ++) {
    polynomial = elementThatMustVanish.coefficients[i];
    polynomial.scaleNormalizeLeadingMonomial(
      &MonomialPolynomial::orderDefault()
    );
    this->systemToSolve.addOnTopNoRepetition(polynomial);
  }
}

void CandidateSemisimpleSubalgebra::getGenericLinearCombination(
  int numberOfVariables,
  int variableOffset,
  List<ChevalleyGenerator>& involvedGenerators,
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& output
) {
  Polynomial<AlgebraicNumber> coefficient;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > monomial;
  output.makeZero();
  for (int i = 0; i < involvedGenerators.size; i ++) {
    coefficient.makeDegreeOne(numberOfVariables, variableOffset + i, 1);
    monomial.makeGenerator(
      involvedGenerators[i].generatorIndex, *this->owner->owner
    );
    monomial *= coefficient;
    output += monomial;
  }
}

bool CandidateSemisimpleSubalgebra::computeCharacter(bool allowBadCharacter) {
  if (this->indexNonEmbeddedMeStandard == - 1) {
    global.fatal
    << "Attempting to "
    << "compute char of candidate subalgebra that "
    << "has not been initialized properly. "
    << global.fatal;
  }
  STACK_TRACE("CandidateSemisimpleSubalgebra::computeCharacter");
  this->checkCandidateInitialization();
  this->weylNonEmbedded->computeRho(true);
  Weight<Rational> monomial;
  monomial.weightFundamentalCoordinates.makeZero(
    this->weylNonEmbedded->getDimension()
  );
  monomial.owner = nullptr;
  this->characterFundamentalCoordinatesRelativeToCartan.makeZero();
  this->characterFundamentalCoordinatesRelativeToCartan.addMonomial(
    monomial, this->getAmbientSemisimpleLieAlgebra().getRank()
  );
  List<DynkinSimpleType> dynkinTypes;
  this->weylNonEmbedded->dynkinType.getTypesWithMults(dynkinTypes);
  Matrix<Rational> coCartanCandidate;
  this->cartanElementsScaledToActByTwo.getGramMatrix(
    coCartanCandidate, &this->getAmbientWeyl().cartanSymmetric
  );
  if (coCartanCandidate != this->weylNonEmbedded->coCartanSymmetric) {
    if (!allowBadCharacter) {
      global.fatal
      << "This is a mathematical error. "
      << "The co-Cartan symmetric matrix is: "
      << this->weylNonEmbedded->coCartanSymmetric.toString()
      << " but co-Cartan of the computed candidate elements of the Cartan, "
      << this->cartanElementsScaledToActByTwo.toString()
      << " is different, namely, "
      << coCartanCandidate.toString();
    }
  }
  for (int k = 0; k < this->getAmbientWeyl().rootSystem.size; k ++) {
    int counter = - 1;
    for (
      int i = 0; i < this->cartanSubalgebrasByComponentScaledToActByTwo.size; i
      ++
    ) {
      for (
        int j = 0; j < this->cartanSubalgebrasByComponentScaledToActByTwo[i].
        size; j ++
      ) {
        counter ++;
        monomial.weightFundamentalCoordinates[counter] = (
          this->getAmbientWeyl().rootScalarCartanRoot(
            this->getAmbientWeyl().rootSystem[k],
            this->cartanElementsScaledToActByTwo[counter]
          )
        );
        if (
          !monomial.weightFundamentalCoordinates[counter].isInteger()
        ) {
          if (!allowBadCharacter) {
            global.fatal
            << "Function computeCharacter "
            << "called with Cartan that suggests non-integral characters. At "
            << "the same time, an option banning this "
            << "possibility has been explicitly selected. "
            << global.fatal;
          }
          return false;
        }
      }
    }
    this->characterFundamentalCoordinatesRelativeToCartan.addMonomial(
      monomial, 1
    );
  }
  CharacterSemisimpleLieAlgebraModule<Rational> totalCharacter;
  CharacterSemisimpleLieAlgebraModule<Rational> freudenthalChar;
  CharacterSemisimpleLieAlgebraModule<Rational> outputChar;
  totalCharacter = this->characterFundamentalCoordinatesRelativeToCartan;
  SemisimpleLieAlgebra* nonEmbeddedMe =
  &(*this->owner->subalgebrasNonEmbedded).values[
    this->indexNonEmbeddedMeStandard
  ];
  this->characterNonPrimalFundamentalCoordinates.makeZero();
  while (totalCharacter.size() > 0) {
    int currentIndex =
    totalCharacter.getIndexExtremeWeightRelativeToWeyl(
      *this->weylNonEmbedded
    );
    if (currentIndex == - 1) {
      global.fatal
      << "While decomposing ambient Lie algebra "
      << "over the candidate subalgebra, I got "
      << "that there is no extreme weight. "
      << "This is impossible: something has gone very wrong. "
      << global.fatal;
    }
    if (totalCharacter.coefficients[currentIndex] < 0) {
      return false;
    }
    for (
      int i = 0; i < totalCharacter[currentIndex].weightFundamentalCoordinates.
      size; i ++
    ) {
      if (
        totalCharacter[currentIndex].weightFundamentalCoordinates[i] < 0
      ) {
        return false;
      }
    }
    freudenthalChar.makeZero();
    monomial = totalCharacter[currentIndex];
    monomial.owner = nonEmbeddedMe;
    freudenthalChar.addMonomial(
      monomial, totalCharacter.coefficients[currentIndex]
    );
    this->characterNonPrimalFundamentalCoordinates.addMonomial(
      totalCharacter[currentIndex],
      totalCharacter.coefficients[currentIndex]
    );
    std::string currentString;
    bool tempBool =
    freudenthalChar.freudenthalEvaluateMeFullCharacter(
      outputChar, - 1, &currentString
    );
    if (!tempBool && !allowBadCharacter) {
      global.fatal
      << "Failed to evaluate full character "
      << "via the Freudenthal formula on a relatively small example, namely "
      << freudenthalChar.toString()
      << ". The failure message was: "
      << currentString
      << ". This shouldn't happen. "
      << global.fatal;
      return false;
    }
    totalCharacter -= outputChar;
  }
  return true;
}

void SlTwoSubalgebra::
toStringModuleDecompositionMinimalContainingRegularSubalgebras(
  bool useLatex,
  bool useHtml,
  SlTwoSubalgebras& owner,
  std::string& output
) const {
  std::stringstream out;
  std::string currentString;
  if (useLatex) {
    out << "$";
  }
  if (useHtml) {
    out << "<table><tr><td align='center'>Char.</td>";
    for (
      int i = 0; i < this->indicesMinimalContainingRootSubalgebras.size; i ++
    ) {
      RootSubalgebra& rootSubalgebra =
      owner.rootSubalgebras.subalgebras[
        this->indicesMinimalContainingRootSubalgebras[i]
      ];
      out
      << "<td align='center'>Decomp. "
      << rootSubalgebra.dynkinDiagram.toString()
      << "</td>";
    }
    out << "</tr>\n";
  }
  out
  << "<tr><td align='center'> "
  << this->hCharacteristic.toString()
  << "</td>";
  this->checkIndicesMinimalContainingRootSubalgebras();
  for (
    int k = 0; k < this->indicesMinimalContainingRootSubalgebras.size; k ++
  ) {
    RootSubalgebra& rootSubalgebra =
    owner.rootSubalgebras.subalgebras[
      this->indicesMinimalContainingRootSubalgebras[k]
    ];
    currentString = rootSubalgebra.dynkinDiagram.toString();
    if (useHtml) {
      out << "<td align='center'>";
    }
    out
    << this->moduleDecompositionMinimalContainingRootSubalgebras[k].toString();
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
  return this->owner->weylGroup;
}

std::string SlTwoSubalgebra::toStringTripleStandardRealization() const {
  Matrix<Rational> matrixH, matrixE, matrixF;
  if (
    !this->owner->getElementStandardRepresentation(this->hElement, matrixH) ||
    !this->owner->getElementStandardRepresentation(this->eElement, matrixE) ||
    !this->owner->getElementStandardRepresentation(this->fElement, matrixF)
  ) {
    return "";
  }
  std::stringstream out;
  FormatExpressions format;
  format.flagUseHTML = false;
  format.flagUseLatex = true;
  out << "<br>Matrix realizations in a standard representation: ";
  out << "<div class='lieAlgebraPanel'>";
  out << "<div>";
  out << "\\(\\begin{array}{rcl}";
  out << "h&=&" << matrixH.toString(&format) << "\\\\\n";
  out << "e&=&" << matrixE.toString(&format) << "\\\\\n";
  out << "f&=&" << matrixF.toString(&format);
  out << "\\end{array}\\)";
  out << "<br>";
  std::stringstream calculatorLink;
  calculatorLink
  << "h="
  << matrixH.toString(&format)
  << ";\n"
  << "e="
  << matrixE.toString(&format)
  << ";\n"
  << "f="
  << matrixF.toString(&format)
  << ";\n";
  out
  << "Calculator link: "
  << HtmlRoutines::getCalculatorComputationAnchorThisServer(
    calculatorLink.str(), ""
  );
  out << "</div>";
  out << "</div>";
  return out.str();
}

std::string SlTwoSubalgebra::toStringKostantSekiguchiTripleStandardRealization(
) const {
  Matrix<AlgebraicNumber> matrixH;
  Matrix<AlgebraicNumber> matrixE;
  Matrix<AlgebraicNumber> matrixF;
  if (
    !this->owner->getElementStandardRepresentation(
      this->hAlgebraic, matrixH
    ) ||
    !this->owner->getElementStandardRepresentation(
      this->eKostantSekiguchi, matrixE
    ) ||
    !this->owner->getElementStandardRepresentation(
      this->fKostantSekiguchi, matrixF
    )
  ) {
    return "";
  }
  std::stringstream out;
  FormatExpressions format;
  format.flagUseHTML = false;
  format.flagUseLatex = true;
  out << "<br>Matrix realizations in a standard representation: ";
  out << "<div class='lieAlgebraPanel'>";
  out << "<div>";
  out << "\\(\\begin{array}{rcl}";
  out << "h&=&" << matrixH.toString(&format) << "\\\\\n";
  out << "e&=&" << matrixE.toString(&format) << "\\\\\n";
  out << "f&=&" << matrixF.toString(&format);
  out << "\\end{array}\\)";
  out << "<br>";
  std::stringstream calculatorLink;
  calculatorLink
  << "h="
  << matrixH.toString(&format)
  << ";\n"
  << "e="
  << matrixE.toString(&format)
  << ";\n"
  << "f="
  << matrixF.toString(&format)
  << ";\n";
  out
  << "Calculator link: "
  << HtmlRoutines::getCalculatorComputationAnchorThisServer(
    calculatorLink.str(), ""
  );
  out << "</div>";
  out << "</div>";
  return out.str();
}

std::string SlTwoSubalgebra::toString(FormatExpressions* format) const {
  STACK_TRACE("SlTwoSubalgebra::toString");
  if (this->container == nullptr) {
    return "sl(2) subalgebra not initialized.";
  }
  std::stringstream out;
  std::string currentString;
  out
  << "<a name ='sl2index"
  << this->indexInContainer
  << "'>h-characteristic: "
  << this->hCharacteristic.toString()
  << "</a>";
  out << "<br>Length of the weight dual to h: " << this->lengthHSquared;
  currentString = this->preferredAmbientSimpleBasis.toString();
  std::string virtualPath = "";
  std::string htmlPathServer = "";
  // bool usePNG = false;
  bool useHtml = true;
  bool useLatex = false;
  virtualPath = this->owner->fileNames.virtualFolderName() + "sl2s/";
  htmlPathServer =
  this->owner->fileNames.displayFolderName("../../") + "sl2s/";
  if (virtualPath == "" || htmlPathServer == "") {
    // usePNG = false;
    useHtml = false;
  }
  if (useHtml) {
    out << "<br>";
  }
  out << "\nSimple basis ambient algebra w.r.t defining h: " << currentString;
  if (useHtml) {
    out << "<br>";
  }
  if (this->indicesContainingRootSubalgebras.size > 1) {
    out
    << "Number of containing regular semisimple subalgebras: "
    << this->indicesContainingRootSubalgebras.size;
    if (useHtml) {
      out << "<br>";
    }
  }
  FormatExpressions localFormat;
  FormatExpressions latexFormat;
  localFormat.flagUseHTML = useHtml;
  localFormat.flagUseLatex = useLatex;
  latexFormat.flagUseHTML = false;
  latexFormat.flagUseLatex = true;
  for (int i = 0; i < this->indicesContainingRootSubalgebras.size; i ++) {
    out
    << "\nContaining regular semisimple subalgebra number "
    << i + 1
    << ": ";
    RootSubalgebra& currentSubalgebra =
    this->container->rootSubalgebras.subalgebras[
      this->indicesContainingRootSubalgebras[i]
    ];
    std::string dynkinType = currentSubalgebra.dynkinDiagram.toString();
    if (useHtml) {
      out
      << "<a href='../rootSubalgebra_"
      << this->indicesContainingRootSubalgebras[i] + 1
      << ".html'>";
    }
    out << "\\(" << dynkinType << "\\)";
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
  out
  << HtmlRoutines::getMathNoDisplay((
      this->moduleDecompositionAmbientSubalgebra.toString(&formatCharacter)
    )
  )
  << "\n<br>\n";
  out << "\nBelow is one possible realization of the sl(2) subalgebra.";
  if (useHtml) {
    out << "\n<br>\n";
  }
  out << this->toStringTriple(format);
  out << this->toStringTripleStandardRealization();
  out << this->toStringKostantSekiguchiTriple(format);
  out << this->toStringKostantSekiguchiTripleStandardRealization();
  out << this->toStringTripleVerification(format);
  out << "<br>Unfold the hidden panel for more information.<br>";
  out << "<div class='lieAlgebraPanel'><div>";
  out << this->toStringTripleUnknowns(format);
  out << this->toStringTripleUnknownsPolynomialSystem(format);
  out << this->toStringTripleArbitrary(format);
  out << this->toStringTripleArbitraryMatrix();
  out << this->toStringKostantSekiguchiTripleInternals(format);
  out << "</div></div>";
  return out.str();
}

void SlTwoSubalgebra::initialize() {
  this->algebraicClosure = nullptr;
  this->owner = nullptr;
  this->container = nullptr;
  this->indexInContainer = - 1;
  this->flagCentralizerTypeComputed = false;
  this->flagCentralizerIsRegular = false;
  this->dimensionCentralizer = - 1;
}

void SlTwoSubalgebra::computeDynkinTypeEmbedded(DynkinType& output) const {
  Rational halfLength = this->lengthHSquared / 2;
  output.makeSimpleType('A', 1, &halfLength);
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

void SlTwoSubalgebra::computeDynkinsEpsilon(WeylGroupData& weyl) {
  this->dynkinsEpsilon =
  this->diagramM.numberRootsGeneratedByDiagram() + this->diagramM.rankTotal();
  int r = 0;
  for (int i = 0; i < this->hCharacteristic.size; i ++) {
    if (!this->hCharacteristic[i].isEqualToZero()) {
      r ++;
    }
  }
  this->dynkinsEpsilon += r;
  for (int i = 0; i < weyl.rootSystem.size; i ++) {
    int nonZeroFound = 0;
    for (int j = 0; j < this->hCharacteristic.size; j ++) {
      if (weyl.rootSystem[i][j] == 1) {
        nonZeroFound ++;
      }
      if (nonZeroFound > 1) {
        break;
      }
    }
    if (nonZeroFound == 1) {
      this->dynkinsEpsilon --;
    }
  }
  this->dynkinsEpsilon = 0;
  for (int i = 0; i < this->hCharacteristic.size; i ++) {
    if (this->hCharacteristic[i] == 1) {
      this->dynkinsEpsilon = - 1;
    }
  }
  this->dynkinsEpsilon = 0;
}

bool SlTwoSubalgebra::moduleDecompositionFitsInto(
  const SlTwoSubalgebra& other
) const {
  return
  this->moduleDecompositionLeftFitsIntoRight(
    this->moduleDecompositionAmbientSubalgebra,
    other.moduleDecompositionAmbientSubalgebra
  );
}

bool SlTwoSubalgebra::moduleDecompositionLeftFitsIntoRight(
  const CharacterSemisimpleLieAlgebraModule<Rational>& moduleDecompoLeft,
  const CharacterSemisimpleLieAlgebraModule<Rational>& moduleDecompoRight
) {
  STACK_TRACE("SlTwoSubalgebra::moduleDecompositionLeftFitsIntoRight");
  CharacterSemisimpleLieAlgebraModule<Rational> moduleDifference =
  moduleDecompoRight - moduleDecompoLeft;
  for (int i = 0; i < moduleDifference.size(); i ++) {
    if (moduleDifference.coefficients[i] < 0) {
      return false;
    }
  }
  return true;
}

void SlTwoSubalgebra::toHTML(std::string& filePath) {
  std::fstream file;
  std::string fileName = filePath;
  fileName.append("theSlTwo.txt");
  FileOperations::openFileCreateIfNotPresentVirtual(
    file, "output/" + filePath, false, true, false
  );
}

void SlTwoSubalgebras::reset(SemisimpleLieAlgebra& inputOwner) {
  STACK_TRACE("SlTwoSubalgebras::reset");
  this->indicesSl2sContainedInRootSubalgebras.setSize(0);
  this->indicesSl2DecompositionFormulas.setSize(0);
  this->badHCharacteristics.setSize(0);
  this->indexZeroWeight = - 1;
  this->owner = &inputOwner;
  this->rootSubalgebras.owner = &inputOwner;
  this->clear();
}

void SemisimpleLieAlgebra::findSl2Subalgebras(
  SemisimpleLieAlgebra& inputOwner,
  SlTwoSubalgebras& output,
  bool computeRealForm,
  AlgebraicClosureRationals* algebraicClosure
) {
  STACK_TRACE("SemisimpleLieAlgebra::findSl2Subalgebras");
  ProgressReport report0;
  if (report0.tickAndWantReport()) {
    std::stringstream reportStream0;
    reportStream0
    << "Finding sl(2)-subalgebras (and thus a "
    << "full list of the nilpotent orbits) of "
    << inputOwner.weylGroup.dynkinType.toString();
    report0.report(reportStream0.str());
  }
  inputOwner.checkConsistency();
  output.reset(inputOwner);
  output.checkConsistency();
  output.getOwner().computeChevalleyConstants();
  output.rootSubalgebras.owner = &inputOwner;
  output.rootSubalgebras.computeAllReductiveRootSubalgebrasUpToIsomorphism();
  output.indicesSl2sContainedInRootSubalgebras.setSize(
    output.rootSubalgebras.subalgebras.size
  );
  output.indicesSl2sContainedInRootSubalgebras.reserve(
    output.rootSubalgebras.subalgebras.size * 2
  );
  output.checkMinimalContainingRootSubalgebras();
  for (
    int i = 0; i < output.indicesSl2sContainedInRootSubalgebras.size; i ++
  ) {
    output.indicesSl2sContainedInRootSubalgebras[i].size = 0;
  }
  ProgressReport report;
  for (int i = 0; i < output.rootSubalgebras.subalgebras.size; i ++) {
    std::stringstream currentStream;
    currentStream
    << "\nExploring root subalgebra "
    << output.rootSubalgebras.subalgebras[i].dynkinDiagram.toString()
    << " ("
    << (i + 1)
    << " out of "
    << output.rootSubalgebras.subalgebras.size
    << ")\n";
    report.report(currentStream.str());
    output.rootSubalgebras.subalgebras[i].
    getSsl2SubalgebrasAppendListNoRepetition(
      output, i, computeRealForm, algebraicClosure
    );
  }
  // sort subalgebras by dynkin index
  List<int> permutation;
  List<int> indexMap;
  permutation.setSize(output.size);
  indexMap.setSize(permutation.size);
  for (int i = 0; i < permutation.size; i ++) {
    permutation[i] = i;
  }
  output.quickSortDescending(nullptr, &permutation);
  for (int i = 0; i < indexMap.size; i ++) {
    indexMap[permutation[i]] = i;
  }
  for (
    int j = 0; j < output.indicesSl2sContainedInRootSubalgebras.size; j ++
  ) {
    for (
      int k = 0; k < output.indicesSl2sContainedInRootSubalgebras[j].size; k ++
    ) {
      output.indicesSl2sContainedInRootSubalgebras[j][k] =
      indexMap[output.indicesSl2sContainedInRootSubalgebras[j][k]];
    }
  }
  inputOwner.checkConsistency();
  output.checkMinimalContainingRootSubalgebras();
  for (int i = 0; i < output.size; i ++) {
    SlTwoSubalgebra& currentSubalgebra = output.getElement(i);
    currentSubalgebra.indicesMinimalContainingRootSubalgebras.reserve(
      currentSubalgebra.indicesContainingRootSubalgebras.size
    );
    currentSubalgebra.indicesMinimalContainingRootSubalgebras.size = 0;
    for (
      int j = 0; j < currentSubalgebra.indicesContainingRootSubalgebras.size; j
      ++
    ) {
      bool isMinimalContaining = true;
      for (
        int k = 0; k < currentSubalgebra.indicesContainingRootSubalgebras.size;
        k ++
      ) {
        RootSubalgebra& otherRootSubalgebra =
        output.rootSubalgebras.subalgebras[
          currentSubalgebra.indicesContainingRootSubalgebras[k]
        ];
        if (
          otherRootSubalgebra.indicesSubalgebrasContainingK.contains(
            currentSubalgebra.indicesContainingRootSubalgebras[j]
          )
        ) {
          isMinimalContaining = false;
          break;
        }
      }
      if (isMinimalContaining) {
        currentSubalgebra.indicesMinimalContainingRootSubalgebras.
        addOnTopNoRepetition(
          currentSubalgebra.indicesContainingRootSubalgebras[j]
        );
      }
    }
    output.checkConsistency();
    output.computeModuleDecompositionsitionsOfAmbientLieAlgebra();
  }
}

bool CandidateSemisimpleSubalgebra::checkConsistency() const {
  if (this->flagDeallocated) {
    global.fatal
    << "Use after free of CandidateSemisimpleSubalgebra. "
    << global.fatal;
  }
  return true;
}

bool CandidateSemisimpleSubalgebra::computeAndVerifyFromGeneratorsAndHs() {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebra::computeAndVerifyFromGeneratorsAndHs"
  );
  if (!this->flagSubalgebraPreloadedButNotVerified) {
    return true;
  }
  Matrix<Rational> actualCoCartan;
  this->cartanElementsScaledToActByTwo.getGramMatrix(
    actualCoCartan,
    &this->owner->getSemisimpleOwner().weylGroup.cartanSymmetric
  );
  std::stringstream out;
  this->flagSubalgebraPreloadedButNotVerified = false;
  if (!(this->weylNonEmbedded->coCartanSymmetric == actualCoCartan)) {
    out
    << "<b>Corrupt semisimple subalgebra: "
    << "the gram matrix of the elements of its Cartan, "
    << this->cartanElementsScaledToActByTwo.toString()
    << " is "
    << actualCoCartan.toString()
    << "; it should be "
    << this->weylNonEmbedded->coCartanSymmetric.toString()
    << ".</b>";
    this->flagSystemProvedToHaveNoSolution = true;
  }
  this->computeSystem(false, true);
  if (!this->computeCharacter(true)) {
    out
    << "<b>Corrupt semisimple subalgebra: "
    << "the ambient Lie algebra does not decompose "
    << "properly over the candidate subalgebra. </b>";
    this->flagSystemProvedToHaveNoSolution = true;
  }
  if (!this->checkGeneratorsBracketToHs()) {
    out
    << "<b>Corrput semisimple subalgebra: "
    << "Lie brackets of generators do not equal "
    << "the desired elements of the Cartan. ";
    this->flagSystemProvedToHaveNoSolution = true;
  }
  this->comments = out.str();
  return !this->flagSystemProvedToHaveNoSolution;
}

bool CandidateSemisimpleSubalgebra::checkMaximalDominance() const {
  STACK_TRACE("CandidateSemisimpleSubalgebra::checkMaximalDominance");
  this->checkBasicInitialization();
  Rational scalarProduct;
  for (int i = 0; i < this->getAmbientWeyl().rootsOfBorel.size; i ++) {
    Vector<Rational>& currentPositiveRoot =
    this->getAmbientWeyl().rootsOfBorel[i];
    bool canBeRaisingReflection = true;
    for (
      int k = 0; k < this->cartanSubalgebrasByComponentScaledToActByTwo.size &&
      canBeRaisingReflection; k ++
    ) {
      for (
        int l = 0; l < this->cartanSubalgebrasByComponentScaledToActByTwo[k].
        size &&
        canBeRaisingReflection; l ++
      ) {
        scalarProduct =
        this->getAmbientWeyl().rootScalarCartanRoot(
          currentPositiveRoot,
          this->cartanSubalgebrasByComponentScaledToActByTwo[k][l]
        );
        if (scalarProduct > 0) {
          canBeRaisingReflection = false;
        }
        if (scalarProduct < 0) {
          global.fatal
          << "The candidate h elements "
          << "of the semisimple subalgebra are "
          << "supposed to be maximally dominant, "
          << "however the scalar product of the positive root "
          << currentPositiveRoot.toString()
          << " with the subalgebra root "
          << this->cartanSubalgebrasByComponentScaledToActByTwo[k][l].toString(
          )
          << " is negative, while the very same positive root "
          << "has had zero scalar products with all "
          << "preceding roots. Here are all preceding roots: "
          << this->cartanSubalgebrasByComponentScaledToActByTwo.toString()
          << global.fatal;
          return false;
        }
      }
    }
  }
  return true;
}

bool SlTwoSubalgebras::checkMinimalContainingRootSubalgebras() const {
  this->checkInitialization();
  this->checkConsistency();
  for (int i = 0; i < this->size; i ++) {
    const SlTwoSubalgebra& current = (*this)[i];
    if (current.container != this) {
      global.fatal << "Corrupt sl(2) subalgebra." << global.fatal;
    }
    (*this)[i].checkIndicesMinimalContainingRootSubalgebras();
  }
  return true;
}

void SlTwoSubalgebras::checkInitialization() const {
  if (this->owner == nullptr) {
    global.fatal
    << "<br>Object SlTwoSubalgebras "
    << "is not initialized, although it is supposed to be. "
    << global.fatal;
  }
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

bool SlTwoSubalgebras::containsSl2WithGivenH(
  Vector<Rational>& elementH, int* outputIndex
) {
  if (outputIndex != nullptr) {
    *outputIndex = - 1;
  }
  ElementSemisimpleLieAlgebra<Rational> tempH;
  this->checkInitialization();
  tempH.makeCartanGenerator(elementH, *this->owner);
  for (int i = 0; i < this->size; i ++) {
    if (this->objects[i].hElement == tempH) {
      if (outputIndex != nullptr) {
        *outputIndex = i;
      }
      return true;
    }
  }
  return false;
}

bool SlTwoSubalgebras::containsSl2WithGivenHCharacteristic(
  Vector<Rational>& hCharacteristic, int* outputIndex
) {
  if (outputIndex != nullptr) {
    *outputIndex = - 1;
  }
  for (int i = 0; i < this->size; i ++) {
    if ((*this)[i].hCharacteristic == hCharacteristic) {
      if (outputIndex != nullptr) {
        *outputIndex = i;
      }
      return true;
    }
  }
  return false;
}

bool SlTwoSubalgebra::checkIndicesMinimalContainingRootSubalgebras() const {
  for (
    int i = 0; i < this->indicesMinimalContainingRootSubalgebras.size; i ++
  ) {
    int indexMinimalContainer =
    this->indicesMinimalContainingRootSubalgebras[i];
    if (
      indexMinimalContainer >=
      this->container->rootSubalgebras.subalgebras.size
    ) {
      global.fatal
      << "Corrupt root subalgebra index: "
      << indexMinimalContainer
      << global.fatal;
    }
  }
  return true;
}

bool SlTwoSubalgebra::attemptToComputeCentralizer() {
  STACK_TRACE("SlTwoSubalgebra::attemptToComputeCentralizer");
  this->flagCentralizerIsRegular = false;
  this->flagCentralizerTypeComputed = false;
  Weight<Rational> zeroWeight;
  zeroWeight.weightFundamentalCoordinates.makeZero(1);
  if (
    !this->moduleDecompositionAmbientSubalgebra.getCoefficientOf(zeroWeight).
    isSmallInteger(&this->dimensionCentralizer)
  ) {
    global.fatal
    << "Dimension of centralizer of sl(2)-subalgebra "
    << "is not a small integer. This shouldn't happen. "
    << global.fatal;
  }
  this->checkIndicesMinimalContainingRootSubalgebras();
  for (
    int i = 0; i < this->indicesMinimalContainingRootSubalgebras.size; i ++
  ) {
    int indexMinimalContainer =
    this->indicesMinimalContainingRootSubalgebras[i];
    RootSubalgebra& currentMinimalContainer =
    this->container->rootSubalgebras.subalgebras[indexMinimalContainer];
    Rational dimOfSSpartOfCentralizerOfRootSA =
    currentMinimalContainer.centralizerDynkinType.getRankRational() +
    currentMinimalContainer.centralizerDynkinType.getRootSystemSize();
    this->dimCentralizerToralPart =
    this->owner->getRank() -
    currentMinimalContainer.dynkinType.getRank() -
    currentMinimalContainer.centralizerDynkinType.getRank();
    Rational totalCentalizerCandidateDim =
    dimOfSSpartOfCentralizerOfRootSA + this->dimCentralizerToralPart;
    if (totalCentalizerCandidateDim == this->dimensionCentralizer) {
      this->flagCentralizerIsRegular = true;
      this->flagCentralizerTypeComputed = true;
      this->centralizerTypeIfKnown =
      currentMinimalContainer.centralizerDynkinType;
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
    this->moduleDecompositionAmbientSubalgebra,
    this->moduleDimensions
  );
  this->attemptToComputeCentralizer();
}

void SlTwoSubalgebra::
computeModuleDecompositionsitionOfMinimalContainingRegularSAs(
  SlTwoSubalgebras& owner
) {
  STACK_TRACE(
    "SlTwoSubalgebra::"
    "computeModuleDecompositionsitionOfMinimalContainingRegularSAs"
  );
  this->moduleDecompositionMinimalContainingRootSubalgebras.setSize(
    this->indicesMinimalContainingRootSubalgebras.size
  );
  List<int> buffer;
  this->checkIndicesMinimalContainingRootSubalgebras();
  for (
    int i = 0; i < this->indicesMinimalContainingRootSubalgebras.size; i ++
  ) {
    RootSubalgebra& subalgebra =
    owner.rootSubalgebras.subalgebras[
      this->indicesMinimalContainingRootSubalgebras[i]
    ];
    this->computeModuleDecompositionsition(
      subalgebra.positiveRootsReductiveSubalgebra,
      subalgebra.simpleRootsReductiveSubalgebra.size,
      this->moduleDecompositionMinimalContainingRootSubalgebras[i],
      buffer
    );
  }
}

void SlTwoSubalgebra::makeReportPrecomputations(
  int indexMinimalContainingRegularSA,
  RootSubalgebra& minimalContainingRegularSubalgebra
) {
  STACK_TRACE("SlTwoSubalgebra::makeReportPrecomputations");
  int dimension = this->getOwnerSemisimpleAlgebra().getRank();
  this->indicesContainingRootSubalgebras.size = 0;
  Vectors<Rational> roots;
  roots = minimalContainingRegularSubalgebra.simpleRootsReductiveSubalgebra;
  this->getOwnerSemisimpleAlgebra().weylGroup.
  transformToSimpleBasisGeneratorsWithRespectToH(
    roots, this->hElement.getCartanPart()
  );
  DynkinDiagramRootSubalgebra diagram;
  diagram.ambientBilinearForm = this->getOwnerWeyl().cartanSymmetric;
  diagram.ambientRootSystem = this->getOwnerWeyl().rootSystem;
  diagram.computeDiagramInputIsSimple(roots);
  this->indicesContainingRootSubalgebras.addOnTop(
    indexMinimalContainingRegularSA
  );
  roots.makeEiBasis(dimension);
  this->getOwnerSemisimpleAlgebra().weylGroup.
  transformToSimpleBasisGeneratorsWithRespectToH(
    roots, this->hElement.getCartanPart()
  );
  DynkinDiagramRootSubalgebra tempDiagram;
  tempDiagram.ambientBilinearForm = this->getOwnerWeyl().cartanSymmetric;
  tempDiagram.ambientRootSystem = this->getOwnerWeyl().rootSystem;
  tempDiagram.computeDiagramInputIsSimple(roots);
  this->preferredAmbientSimpleBasis = roots;
  this->hCharacteristic.setSize(dimension);
  for (int i = 0; i < dimension; i ++) {
    this->hCharacteristic[i] =
    this->getOwnerSemisimpleAlgebra().weylGroup.rootScalarCartanRoot(
      this->hElement.getCartanPart(),
      this->preferredAmbientSimpleBasis[i]
    );
  }
  this->getOwnerSemisimpleAlgebra().lieBracket(
    this->eElement, this->fElement, this->eBracketF
  );
  this->getOwnerSemisimpleAlgebra().lieBracket(
    this->hElement, this->eElement, this->hBracketE
  );
  this->getOwnerSemisimpleAlgebra().lieBracket(
    this->hElement, this->fElement, this->hBracketF
  );
}

unsigned int SlTwoSubalgebra::hashFunction() const {
  unsigned int result = 0;
  int j = 0;
  for (int i = 0; i < this->hCharacteristic.size; i ++, j ++) {
    result +=
    this->hCharacteristic.hashFunction() *
    HashConstants::getConstantIncrementCounter(j);
  }
  return result;
}

// The below code is related to sl(2) subalgebras of simple Lie algebras.
void SlTwoSubalgebra::computeModuleDecompositionsition(
  const Vectors<Rational>& positiveRootsContainingRegularSubalgebra,
  int dimensionContainingRegularSubalgebra,
  CharacterSemisimpleLieAlgebraModule<Rational>& outputHighestWeights,
  List<int>& outputModuleDimensions
) {
  STACK_TRACE("SlTwoSubalgebra::computeModuleDecompositionsition");
  this->checkConsistency();
  positiveRootsContainingRegularSubalgebra.checkConsistency();
  if (positiveRootsContainingRegularSubalgebra.size <= 0) {
    global.fatal
    << "positiveRootsContainingRegularSA has less than one element. "
    << global.fatal;
  }
  int indexZeroWeight = positiveRootsContainingRegularSubalgebra.size * 2;
  outputModuleDimensions.initializeFillInObject(
    4 * positiveRootsContainingRegularSubalgebra.size + 1, 0
  );
  outputModuleDimensions[indexZeroWeight] =
  dimensionContainingRegularSubalgebra;
  List<int> bufferHighestWeights;
  Rational scalarProduct;
  Vectors<Rational> coordinatesInPreferredSimpleBasis;
  positiveRootsContainingRegularSubalgebra.getCoordinatesInBasis(
    this->preferredAmbientSimpleBasis, coordinatesInPreferredSimpleBasis
  );
  for (int k = 0; k < positiveRootsContainingRegularSubalgebra.size; k ++) {
    scalarProduct =
    this->hCharacteristic.scalarEuclidean(
      coordinatesInPreferredSimpleBasis[k]
    );
    if (scalarProduct.denominatorShort != 1) {
      global.fatal << "Characteristic must be integer. " << global.fatal;
    }
    if (scalarProduct > positiveRootsContainingRegularSubalgebra.size * 2) {
      global.fatal
      << "The scalar product of the h-Characteristic "
      << this->hCharacteristic.toString()
      << " with the simple root "
      << coordinatesInPreferredSimpleBasis[k].toString()
      << " is larger than "
      << positiveRootsContainingRegularSubalgebra.size * 2
      << ". The affected sl(2) subalgebra is "
      << this->toString()
      << ". "
      << global.fatal;
      break;
    }
    outputModuleDimensions[indexZeroWeight + scalarProduct.numeratorShort] ++;
    outputModuleDimensions[indexZeroWeight - scalarProduct.numeratorShort] ++;
  }
  bufferHighestWeights = (outputModuleDimensions);
  outputHighestWeights.setExpectedSize(
    positiveRootsContainingRegularSubalgebra.size * 2
  );
  outputHighestWeights.makeZero();
  Weight<Rational> currentHighestWeight;
  currentHighestWeight.weightFundamentalCoordinates.makeEi(1, 0);
  for (int j = bufferHighestWeights.size - 1; j >= indexZeroWeight; j --) {
    int topMult = bufferHighestWeights[j];
    if (topMult < 0) {
      global.fatal
      << "The sl(2)-module decomposition "
      << "shows an sl(2)-module with highest weight "
      << topMult
      << " which is impossible. Here is the sl(2) subalgebra. "
      << this->toString()
      << "."
      << global.fatal;
    }
    if (topMult > 0) {
      currentHighestWeight.weightFundamentalCoordinates[0] =
      j - indexZeroWeight;
      outputHighestWeights.addMonomial(currentHighestWeight, topMult);
      if (j != indexZeroWeight) {
        bufferHighestWeights[indexZeroWeight * 2 - j] -= topMult;
      }
      for (int k = j - 2; k >= indexZeroWeight; k -= 2) {
        bufferHighestWeights[k] -= topMult;
        if (k != indexZeroWeight) {
          bufferHighestWeights[indexZeroWeight * 2 - k] -= topMult;
        }
        if (
          bufferHighestWeights[k] < 0 ||
          !(
            bufferHighestWeights[k] ==
            bufferHighestWeights[indexZeroWeight * 2 - k]
          )
        ) {
          std::stringstream crashStream;
          crashStream
          << "An error check has failed. "
          << "While trying to decompose with respect "
          << "to h-characteristic <br> "
          << this->hCharacteristic.toString()
          << ". The positive root system of "
          << "the containing root subalgebra is <br>"
          << positiveRootsContainingRegularSubalgebra.toString()
          << ". <br>The preferred simple basis is <br>"
          << this->preferredAmbientSimpleBasis.toString()
          << ". The coordinates relative to the preferred simple basis are<br>"
          << coordinatesInPreferredSimpleBasis.toString()
          << " The starting weights list is <br>"
          << outputModuleDimensions
          << ". "
          << " I got that the root space of index  "
          << k + 1
          << " has negative dimension. Something is wrong. ";
          global.fatal << crashStream.str() << global.fatal;
        }
      }
    }
  }
}

void SlTwoSubalgebras::toStringModuleDecompositionMinimalContainingRegularSAs(
  std::string& output, bool useLatex, bool useHtml
) {
  std::string currentString;
  std::stringstream out;
  for (int i = 0; i < this->size; i ++) {
    (*this)[i].toStringModuleDecompositionMinimalContainingRegularSubalgebras(
      useLatex, useHtml, *this, currentString
    );
    out << currentString;
    if (useHtml) {
      out << "\n<br>";
    }
    out << "\n";
  }
  output = out.str();
}

Rational SlTwoSubalgebra::getDynkinIndex() const {
  return
  this->lengthHSquared * this->getOwnerWeyl().getLongestRootLengthSquared() /
  4;
}

std::string SlTwoSubalgebras::descriptionHCharacteristic =
"Let h be in the Cartan subalgebra. Let \\(\\alpha_1, ..., \\alpha_n\\) "
"be simple roots with respect to h. "
"Then the h-characteristic, as defined by E. Dynkin, "
"is the n-tuple \\((\\alpha_1(h), ..., \\alpha_n(h))\\).";
std::string SlTwoSubalgebras::descriptionHRealization =
"The actual realization of h. "
"The coordinates of h are given with "
"respect to the fixed original simple basis. "
"Note that the h-characteristic is computed using "
"<b>a possibly different simple basis</b>, "
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
"The \\(sl(2)\\) submodules of the "
"ambient Lie algebra are parametrized by their "
"highest weight with respect to the Cartan element h of \\(sl(2)\\). "
"In turn, the highest weight is a positive integer "
"multiple of the fundamental highest weight \\(\\psi\\). "
"\\(V_{k\\psi}\\) is \\(k + 1\\)-dimensional. ";

std::string SlTwoSubalgebras::toStringSummary(FormatExpressions* format) {
  STACK_TRACE("SlTwoSubalgebras::toStringSummary");
  std::stringstream out;
  bool useHtml = format == nullptr ? true : format->flagUseHTML;
  std::string displayPathAlgebra;
  displayPathAlgebra = "../";
  out << "Number of sl(2) subalgebras: " << this->size << ".\n";
  std::stringstream out2;
  out2
  << "<br>Length longest root ambient algebra squared/4= "
  << this->getOwnerWeyl().getLongestRootLengthSquared() / 4
  << "<br>";
  out2
  << "<br> Given a root subsystem P, and a root subsubsystem P_0, "
  << "in (10.2) of Semisimple subalgebras of semisimple Lie algebras, "
  << "E. Dynkin defines "
  << "a numerical constant e(P, P_0) (which we call Dynkin epsilon). "
  << "<br>In Theorem 10.3, Dynkin proves that if an sl(2) is an "
  << "S-subalgebra in "
  << "the root subalgebra generated by P, such that it has "
  << "characteristic 2 for all simple roots of P lying in P_0, then "
  << "e(P, P_0)= 0. ";
  if (this->badHCharacteristics.size > 0) {
    bool allbadAreGoodInTheirBadness = true;
    for (int i = 0; i < this->badHCharacteristics.size; i ++) {
      bool isGoodInItsbadness = false;
      for (int j = 0; j < this->size; j ++) {
        if (
          this->badHCharacteristics[i] == (*this)[j].hElement.getCartanPart()
        ) {
          isGoodInItsbadness = true;
          break;
        }
      }
      if (!isGoodInItsbadness) {
        allbadAreGoodInTheirBadness = false;
        out
        << "<br><b>Bad characteristic: "
        << this->badHCharacteristics[i]
        << "</b>";
      } else {
        out2
        << "<br><b>"
        << "It turns out that in the current case of Cartan element h = "
        << this->badHCharacteristics[i]
        << " we have that, for a certain P, "
        << "e(P, P_0) equals 0, but I failed to realize the "
        << "corresponding sl(2) as a subalgebra of that P. "
        << "However, it turns out that h "
        << "is indeed an S-subalgebra of a smaller root subalgebra P'.</b>";
      }
    }
    if (!allbadAreGoodInTheirBadness) {
      out << "<br><b>Found bad characteristics!</b><br>";
    }
  } else {
    out2
    << "It turns out by direct computation that, in the current case of "
    << this->getOwner().toStringLieAlgebraName()
    << ",  e(P,P_0)= 0 implies that an S-sl(2) subalgebra "
    << "of the root subalgebra generated by P with "
    << "characteristic with 2's in the simple roots of P_0 "
    << "always exists. Note that "
    << "Theorem 10.3 is stated in one direction only.";
  }
  if (useHtml) {
    std::string idSpanHCharacteristicDescription =
    "spanDynkinHCharacteristicDescription";
    out
    << "<div id = '"
    << idSpanHCharacteristicDescription
    << "'>"
    << this->descriptionHCharacteristic
    << "</div><br>";
    out
    << "<div id = 'idCartanElementRealization'>"
    << this->descriptionHRealization
    << "</div>";
    out
    << "<div id = 'idMinimalContainingRegularSA'>"
    << this->descriptionMinimalContainingRegularSubalgebras
    << "</div>";
    out
    << "<div id = 'idModuleDecomposition'>"
    << this->descriptionModuleDecompositionOverSl2
    << "</div>";
    out
    << "<br><br><table><tr>"
    << "<td>Type + realization link</td>"
    << "<td style =\"padding-right:20px\">"
    << "<a href=\"#"
    << idSpanHCharacteristicDescription
    << "\">h-Characteristic</a>"
    << "</td>";
    out
    << "<td align='center' style =\"white-space: nowrap\">"
    << "<a href='#idCartanElementRealization'>Realization of h</a>"
    << "</td>"
    << "<td style =\"padding-left:20px\">"
    << "<a href=\"#idModuleDecomposition\">"
    << "sl(2)-module decomposition of the ambient Lie algebra</a> <br> "
    << "\\(\\psi=\\) the fundamental \\(sl(2)\\)-weight. "
    << "</a></td>"
    << "<td>Centralizer dimension</td>"
    << "<td>Type of semisimple part of centralizer, if known</td>"
    << "<td>The square of the length of the weight dual to h.</td>"
    << "<td>Dynkin index </td>"
    << "<td>Minimal containing regular semisimple SAs</td><td>"
    << "<a href='#idMinimalContainingRegularSA'>"
    << "Containing regular semisimple SAs in "
    << "which the sl(2) has no centralizer</a>"
    << "</td></tr>";
  }
  for (int i = 0; i < this->size; i ++) {
    const SlTwoSubalgebra& currentSubalgebra = (*this)[i];
    if (useHtml) {
      out
      << "<tr>"
      << "<td style='padding-right:20px'><a href='#sl2index"
      << i
      << "'>\\(A^{"
      << currentSubalgebra.getDynkinIndex()
      << "}_1"
      << "\\)</a></td>";
      out << "<td>";
    }
    out << "\\(" << currentSubalgebra.hCharacteristic.toString() << "\\)";
    if (useHtml) {
      out << "</td>";
      out << "<td style='white-space: nowrap'>";
    }
    out << currentSubalgebra.hElement.getCartanPart().toString();
    if (
      !this->getOwnerWeyl().isDominantWeight(
        currentSubalgebra.hElement.getCartanPart()
      )
    ) {
      out
      << "<b>Something has gone very wrong! "
      << "The h is not dual to a dominant weight. "
      << "This shouldn't happen: "
      << "this is either a programming or mathematical error. </b>";
    }
    if (useHtml) {
      out << "</td><td style='padding-left:20px'>";
    }
    FormatExpressions formatCharacter;
    formatCharacter.vectorSpaceEiBasisNames.addOnTop("\\psi");
    out
    << HtmlRoutines::getMathNoDisplay((
        currentSubalgebra.moduleDecompositionAmbientSubalgebra.toString(
          &formatCharacter
        )
      )
    )
    << "\n<br>\n";
    if (useHtml) {
      out << "</td>";
    }
    out << "<td>" << currentSubalgebra.dimensionCentralizer << "</td>";
    if (currentSubalgebra.flagCentralizerTypeComputed) {
      out
      << "<td> "
      << HtmlRoutines::getMathNoDisplay(
        currentSubalgebra.centralizerTypeIfKnown.toString()
      )
      << "</td>";
    } else {
      out << "<td> not computed</td>";
    }
    if (useHtml) {
      out << "<td>";
    }
    out << currentSubalgebra.lengthHSquared;
    if (useHtml) {
      out << "</td><td>";
    }
    out << currentSubalgebra.getDynkinIndex();
    if (useHtml) {
      out << "</td><td>";
    }
    currentSubalgebra.checkIndicesMinimalContainingRootSubalgebras();
    for (
      int j = 0; j < currentSubalgebra.indicesMinimalContainingRootSubalgebras.
      size; j ++
    ) {
      RootSubalgebra& currentRootSubalgebra =
      this->rootSubalgebras.subalgebras[
        currentSubalgebra.indicesMinimalContainingRootSubalgebras[j]
      ];
      out
      << "<a href='"
      << displayPathAlgebra
      << "rootSubalgebra_"
      << currentSubalgebra.indicesMinimalContainingRootSubalgebras[j] + 1
      << ".html'>"
      << "\\("
      << currentRootSubalgebra.dynkinDiagram.toString()
      << "\\)"
      << "</a>";
    }
    if (useHtml) {
      out << "</td><td>";
    }
    for (
      int j = 0; j < currentSubalgebra.indicesContainingRootSubalgebras.size; j
      ++
    ) {
      RootSubalgebra& currentRootSubalgebra =
      this->rootSubalgebras.subalgebras[
        currentSubalgebra.indicesContainingRootSubalgebras[j]
      ];
      out
      << "<a href='"
      << displayPathAlgebra
      << "rootSubalgebra_"
      << currentSubalgebra.indicesContainingRootSubalgebras[j] + 1
      << ".html'>"
      << "\\("
      << currentRootSubalgebra.dynkinDiagram.toString()
      << "\\)"
      << "</a>";
    }
    if (useHtml) {
      out << "</td></tr>\n";
    }
  }
  if (useHtml) {
    out << "</table><hr>";
  }
  out << out2.str() << "<hr>";
  return out.str();
}

std::string SlTwoSubalgebras::toString(FormatExpressions* format) {
  std::string currentString;
  std::stringstream out;
  std::stringstream body;
  bool useHtml = format == nullptr ? true : format->flagUseHTML;
  for (int i = 0; i < this->size; i ++) {
    currentString = (*this)[i].toString(format);
    //  body<< "Index " << i << ": ";
    if (useHtml) {
      body << "<br>";
    }
    body << currentString;
    if (useHtml) {
      body << "<hr>";
    }
  }
  if (useHtml) {
    out << "<br>";
  }
  out << this->toStringSummary(format);
  out << body.str();
  return out.str();
}

void SlTwoSubalgebras::writeHTML(FormatExpressions* format) {
  STACK_TRACE("SlTwoSubalgebras::writeHTML");
  std::string virtualFileName =
  this->owner->fileNames.virtualFolderName() +
  this->owner->fileNames.fileNameRelativePathSlTwoSubalgebras();
  ProgressReport report;
  report.report(
    "Preparing html pages for sl(2) subalgebras. This might take a while."
  );
  this->rootSubalgebras.toHTML(format);
  std::stringstream out;
  out
  << "<html><title>sl(2)-subalgebras of "
  << this->rootSubalgebras.subalgebras[0].dynkinDiagram.toString()
  << "</title>";
  out << SemisimpleLieAlgebra::toHTMLCalculatorHeadElements("../../../..");
  out
  << "<meta name='keywords' content='"
  << this->rootSubalgebras.subalgebras[0].dynkinDiagram.toString()
  << " sl(2)-triples, sl(2)-subalgebras, "
  << "nilpotent orbits simple Lie algebras, nilpotent orbits of "
  << this->rootSubalgebras.subalgebras[0].dynkinDiagram.toString()
  << ", sl(2)-triples of "
  << this->rootSubalgebras.subalgebras[0].dynkinDiagram.toString()
  << " '>";
  out << SemisimpleLieAlgebra::toHTMLCalculatorBodyOnload();
  out << this->owner->toHTMLCalculatorMainDiv();
  out
  << "<h1>\\(sl(2)\\)-subalgebras of "
  << this->owner->toStringLieAlgebraNameFullHTML()
  << "</h1>";
  out
  << this->owner->toStringHTMLMenuStructureSummary(
    "../", true, true, false, true
  );
  out << this->toString(format);
  out << "</div>";
  out << "</body>";
  out << "</html>";
  std::stringstream commentsOnError;
  if (
    !FileOperations::writeFileVirtual(
      virtualFileName, out.str(), &commentsOnError
    )
  ) {
    global.fatal
    << "Failed to write sl(2)-subalgebras. "
    << commentsOnError.str()
    << global.fatal;
  }
}

bool CandidateSemisimpleSubalgebra::isExtremeWeight(
  int moduleIndex, int indexInIsomorphismComponent
) const {
  Vector<Rational>& currentWeight =
  this->weightsModulesPrimal[moduleIndex][indexInIsomorphismComponent];
  Vectors<Rational>& currentWeights = this->weightsModulesPrimal[moduleIndex];
  for (int j = 0; j < this->primalSubalgebraModules.size; j ++) {
    for (
      int k = 0; k < this->weightsModulesPrimal[
        this->primalSubalgebraModules[j]
      ].size; k ++
    ) {
      if (
        !this->weightsModulesPrimal[this->primalSubalgebraModules[j]][k].
        isEqualToZero()
      ) {
        Vector<Rational>& currentRoot =
        this->weightsModulesPrimal[this->primalSubalgebraModules[j]][k];
        if (
          currentWeights.contains(currentWeight - currentRoot) &&
          currentWeights.contains(currentWeight + currentRoot)
        ) {
          return false;
        }
      }
    }
  }
  return true;
}

Rational CandidateSemisimpleSubalgebra::getScalarSubalgebra(
  const Vector<Rational>& primalWeightLeft,
  const Vector<Rational>& primalWeightRight
) const {
  return
  primalWeightLeft.scalarProduct(
    primalWeightRight, this->bilinearFormFundamentalPrimal
  );
}

std::string CandidateSemisimpleSubalgebra::toStringDrawWeightsHelper(
  int indexModule, const Vector<Rational>& weight
) const {
  STACK_TRACE("CandidateSemisimpleSubalgebra::toStringDrawWeightsHelper");
  std::stringstream out;
  List<List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > >& currentModule =
  this->modules[indexModule];
  FormatExpressions charFormat;
  charFormat.vectorSpaceEiBasisNames.setSize(this->getPrimalRank());
  for (int i = 0; i < this->getRank(); i ++) {
    std::stringstream currentStream;
    currentStream << "\\\\omega_{" << i + 1 << "}";
    charFormat.vectorSpaceEiBasisNames[i] = currentStream.str();
  }
  for (int i = this->getRank(); i < this->getPrimalRank(); i ++) {
    std::stringstream currentStream;
    currentStream << "\\\\psi_{" << i + 1 << "}";
    charFormat.vectorSpaceEiBasisNames[i] = currentStream.str();
  }
  for (int i = 0; i < currentModule.size; i ++) {
    out << "<table style =\\\"border:1px solid #000;\\\">";
    out
    << "<tr><td colspan =\\\"3\\\">"
    << "<span class =\\\"math\\\">"
    << "V_{"
    << this->highestWeightsPrimal[indexModule].toStringLetterFormat(
      "\\\\omega", &charFormat
    )
    << "}"
    << " </span></td></tr>"
    << "<tr>"
    << "<td style =\\\"text-align: center;\\\">basis</td>"
    << "<td style =\\\"text-align: center; "
    << "border-left:1px solid #000; white-space: nowrap;\\\">"
    << "weights simple coords.</td>"
    << "<td style =\\\"text-align: center; "
    << "border-left:1px solid #000; white-space: nowrap;\\\">"
    << "weights fund.coords.</td>"
    << "<td style =\\\"text-align: center; "
    << "border-left:1px solid #000; white-space: nowrap;\\\">"
    << "weights primal</td>"
    << "</tr>";
    for (int j = 0; j < currentModule[i].size; j ++) {
      std::string openTag = "", closeTag = "";
      if (this->weightsModulesPrimal[indexModule][j] == weight) {
        openTag = "<span style =\\\"color:#FF0000\\\">";
        closeTag = "</span>";
      }
      out << "<tr>";
      out << "<td style =\\\"text-align: center;\\\">" << openTag;
      out
      << "<span class =\\\"math\\\">"
      << HtmlRoutines::doubleBackslashes(
        currentModule[i][j].toString()
      )
      << "</span>";
      out << closeTag << "</td>";
      Vector<Rational> tempV;
      this->matrixMultiplyFundamentalCoordinatesToGetSimple.actOnVectorColumn(
        this->weightsModulesPrimal[indexModule][j], tempV
      );
      out
      << "<td style =\\\"text-align: center; border-left:1px solid #000;\\\">"
      << openTag;
      out << tempV.toString();
      out << closeTag << "</td>";
      out
      << "<td style =\\\"text-align: center; border-left:1px solid #000;\\\">"
      << openTag;
      out
      << "<span class =\\\"math\\\">"
      << this->weightsModulesNONprimal[indexModule][j].toStringLetterFormat(
        "\\\\omega", &charFormat
      )
      << "</span>";
      out << closeTag << "</td>";
      out
      << "<td style =\\\"text-align: center; border-left:1px solid #000;\\\">"
      << openTag;
      out
      << "<span class =\\\"math\\\">"
      << this->weightsModulesPrimal[indexModule][j].toStringLetterFormat(
        "\\\\omega", &charFormat
      )
      << "</span>";
      out << closeTag << "</td>";
      out << "</tr>";
    }
    out << "</table>";
  }
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::toStringDrawWeights(
  FormatExpressions* format
) const {
  STACK_TRACE("CandidateSemisimpleSubalgebra::toStringDrawWeights");
  if (!this->flagCentralizerIsWellChosen) {
    return "";
  }
  if (format != nullptr) {
    if (!format->flagIncludeMutableInformation) {
      return
      "<br>Weight diagram not drawn to avoid javascript problems "
      "(use command PrintSemisimpleSubalgebras "
      "if you want to see the weight diagram). ";
    }
  }
  int primalRank = - 1;
  (this->centralizerRank + this->cartanElementsSubalgebra.size).isSmallInteger(
    &primalRank
  );
  if (primalRank < 2) {
    return "";
  }
  std::stringstream out;
  if (primalRank != this->bilinearFormFundamentalPrimal.numberOfColumns) {
    out
    << "<br>The primal rank was computed to be "
    << primalRank
    << " but the bilinear form in fundamental coordinates relative to "
    << " the subalgebra was not computed: this->bilinearFormFundPrimal has "
    << this->bilinearFormFundamentalPrimal.numberOfRows
    << " rows. ";
    return out.str();
  }
  out
  << "<br>Weight diagram. The coordinates "
  << "corresponding to the simple roots of the "
  << "subalgerba are fundamental.  "
  << "<br>The bilinear form is therefore given "
  << "relative to the fundamental coordinates.<br> ";
  Vectors<Rational> basisToDrawCirclesAt;
  DrawingVariables drawingVariables;
  drawingVariables.bilinearForm.initialize(primalRank, primalRank);
  for (int i = 0; i < primalRank; i ++) {
    for (int j = 0; j < primalRank; j ++) {
      drawingVariables.bilinearForm(i, j) =
      this->bilinearFormFundamentalPrimal(i, j).getDoubleValue();
    }
  }
  Vector<Rational> zeroVector;
  zeroVector.makeZero(primalRank);
  basisToDrawCirclesAt.makeEiBasis(primalRank);
  Vectors<Rational> cornerWeights;
  int largestModuleDimension = 0;
  for (int i = 0; i < this->modules.size; i ++) {
    largestModuleDimension =
    MathRoutines::maximum(
      this->modules[i][0].size, largestModuleDimension
    );
  }
  drawingVariables.defaultHtmlHeight =
  MathRoutines::maximum(600, largestModuleDimension* 35);
  for (int i = 0; i < this->modules.size; i ++) {
    cornerWeights.setSize(0);
    for (int j = 0; j < this->modules[i].size; j ++) {
      for (int k = 0; k < this->modules[i][j].size; k ++) {
        if (j == 0) {
          std::string color = "#959500";
          if (this->primalSubalgebraModules.contains(i)) {
            drawingVariables.drawLineBetweenTwoVectorsBufferRational(
              zeroVector, this->weightsModulesPrimal[i][k], "green"
            );
          }
          drawingVariables.drawCircleAtVectorBufferRational(
            this->weightsModulesPrimal[i][k], color, 2
          );
          if (this->isExtremeWeight(i, k)) {
            cornerWeights.addOnTop(this->weightsModulesPrimal[i][k]);
          }
        }
        if (
          k == this->modules[i][j].size - 1 &&
          basisToDrawCirclesAt.size < primalRank
        ) {
          basisToDrawCirclesAt.addOnTop(
            this->weightsModulesPrimal[i][k]
          );
          if (
            basisToDrawCirclesAt.getRankElementSpan() !=
            basisToDrawCirclesAt.size
          ) {
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
      Rational minimalDistance = 0;
      for (int k = 0; k < cornerWeights.size; k ++) {
        Rational scalarProduct = Vector<Rational>::scalarProduct(
          cornerWeights[k] - cornerWeights[j],
          cornerWeights[k] - cornerWeights[j],
          this->bilinearFormFundamentalPrimal
        );
        if (minimalDistance == 0) {
          minimalDistance = scalarProduct;
        } else if (scalarProduct != 0) {
          minimalDistance =
          MathRoutines::minimum(scalarProduct, minimalDistance);
        }
      }
      for (int k = j + 1; k < cornerWeights.size; k ++) {
        if (
          minimalDistance == Vector<Rational>::scalarProduct(
            cornerWeights[k] - cornerWeights[j],
            cornerWeights[k] - cornerWeights[j],
            this->bilinearFormFundamentalPrimal
          )
        ) {
          drawingVariables.drawLineBetweenTwoVectorsBufferRational(
            cornerWeights[k], cornerWeights[j], color, 1
          );
        }
      }
    }
  }
  HashedList<Vector<Rational> > currentWeights;
  Vectors<double> highlightGroup;
  List<std::string> highlightLabels;
  for (int i = 0; i < this->modules.size; i ++) {
    currentWeights.clear();
    currentWeights.addOnTopNoRepetition(this->weightsModulesPrimal[i]);
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
    drawingVariables.drawHighlightGroup(
      highlightGroup, highlightLabels, "black", 5
    );
  }
  drawingVariables.basisToDrawCirclesAt.setSize(basisToDrawCirclesAt.size);
  for (int i = 0; i < drawingVariables.basisToDrawCirclesAt.size; i ++) {
    drawingVariables.basisToDrawCirclesAt[i].setSize(primalRank);
    for (int j = 0; j < primalRank; j ++) {
      drawingVariables.basisToDrawCirclesAt[i][j] =
      basisToDrawCirclesAt[i][j].getDoubleValue();
    }
    drawingVariables.drawCircleAtVectorBufferRational(
      basisToDrawCirclesAt[i], "red", 4
    );
  }
  drawingVariables.basisProjectionPlane.makeEiBasis(primalRank);
  out << drawingVariables.getHTMLDiv(primalRank, true);
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::toStringModuleDecompositionLaTeX(
  FormatExpressions* format
) const {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebra::toStringModuleDecompositionLaTeX"
  );
  if (this->modules.size <= 0) {
    return "";
  }
  if (!this->owner->flagComputeModuleDecomposition) {
    return "";
  }
  if (!this->flagCentralizerIsWellChosen) {
    return "";
  }
  std::stringstream out;
  // out << "Isotypic module decomposition over primal subalgebra (total " <<
  // this->modules.size << " isotypic components). ";
  FormatExpressions characterFormat;
  if (!this->characterFormat.isZeroPointer()) {
    characterFormat = this->characterFormat.getElementConst();
  }
  // bool useModuleIndex = false;
  out
  << "\\documentclass{article}\\usepackage{amssymb}\\usepackage{longtable}"
  << "\\usepackage{multirow}\\begin{document}";
  out
  << "<br> \\begin{longtable}{c|c|c|c|c|c}\n<br>\n"
  << "\\caption{Module decomposition of $"
  << this->owner->owner->weylGroup.dynkinType.toString()
  << "$ over $"
  << this->weylNonEmbedded->dynkinType.toString()
  << " \\oplus \\mathfrak h_c$\\label{tableModuleDecompo} }\\\\"
  << "Component &Module & elements & elt. weights& $h$-element "
  << "$\\mathfrak k-sl(2)$-triple& "
  << "$f$-element $\\mathfrak k-sl(2)$-triple\\\\";
  ElementSemisimpleLieAlgebra<AlgebraicNumber> tempLieBracket;
  for (int i = 0; i < this->modules.size; i ++) {
    int numberOfRowsIsotypicComposition =
    this->modules[i].size * this->modules[i][0].size;
    if (numberOfRowsIsotypicComposition > 1) {
      out << "\\multirow{" << numberOfRowsIsotypicComposition << "}{*}";
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
      out
      << "{$V_{"
      << this->highestWeightsPrimal[i].toStringLetterFormat(
        "\\omega", &characterFormat
      )
      << "}$} ";
      out << "&";
      for (int k = 0; k < this->modules[i][j].size; k ++) {
        if (k > 0) {
          out << "&&";
        }
        out << "$" << this->modules[i][j][k].toString(format) << "$";
        out << "&";
        out
        << "$"
        << this->weightsModulesPrimal[i][k].toStringLetterFormat(
          "\\omega", &characterFormat
        )
        << "$"
        << "& ";
        bool oppositesAreGood = false;
        if (i < this->modulesSl2Opposite.size) {
          if (j < this->modulesSl2Opposite[i].size) {
            if (k < this->modulesSl2Opposite[i][j].size) {
              oppositesAreGood = true;
            }
          }
        }
        if (oppositesAreGood) {
          List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& currentOpModule
          =
          this->modulesSl2Opposite[i][j];
          if (!currentOpModule[k].isEqualToZero()) {
            this->getAmbientSemisimpleLieAlgebra().lieBracket(
              this->modules[i][j][k],
              currentOpModule[k],
              tempLieBracket
            );
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

std::string CandidateSemisimpleSubalgebra::toStringModuleDecomposition(
  FormatExpressions* format
) const {
  STACK_TRACE("CandidateSemisimpleSubalgebra::toStringModuleDecomposition");
  (void) format;
  if (this->modules.size <= 0) {
    return "";
  }
  std::stringstream out;
  out
  << "Isotypic module decomposition over primal subalgebra (total "
  << this->modules.size
  << " isotypic components). ";
  out << "<table class='tableStandard'>";
  FormatExpressions characterFormat;
  if (!this->characterFormat.isZeroPointer()) {
    characterFormat = this->characterFormat.getElementConst();
  }
  out << "<tr><td>Isotypic components + highest weight</td>";
  for (int i = 0; i < this->highestWeightsPrimal.size; i ++) {
    bool visible = true;
    bool isDouble = false;
    if (i > 0) {
      if (
        this->highestWeightsPrimal[i] == this->highestWeightsPrimal[i - 1]
      ) {
        visible = false;
      }
    }
    if (i < this->highestWeightsPrimal.size - 1) {
      if (
        this->highestWeightsPrimal[i] == this->highestWeightsPrimal[i + 1]
      ) {
        isDouble = true;
      }
    }
    if (visible) {
      if (isDouble) {
        out << "<td colspan='2'>";
      } else {
        out << "<td>";
      }
      std::stringstream currentStream;
      currentStream
      << "V_{"
      << this->highestWeightsPrimal[i].toStringLetterFormat(
        "\\omega", &characterFormat
      )
      << "} ";
      out
      << HtmlRoutines::getMathNoDisplay(currentStream.str())
      << " &rarr; "
      << this->highestWeightsPrimal[i].toString();
      out << "</td>";
    }
  }
  out << "</tr>";
  out << "<tr><td>Module label </td>";
  for (int i = 0; i < this->highestWeightsPrimal.size; i ++) {
    out << "<td>\\(" << "W_{" << i + 1 << "}" << "\\)</td>";
  }
  out << "</tr>";
  out
  << "<tr><td>Module elements (weight vectors). "
  << "<span class='fElement'>In blue - corresp. F element</span>. "
  << "<span class='hElement'>In red -corresp. H element</span>. </td>";
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
    out << "<table class='tableNone'><tr>";
    for (int j = 0; j < this->modules[i].size; j ++) {
      List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& currentModule =
      this->modules[i][j];
      out << "<td>";
      out << "<table class='tableModuleDecomposition'>";
      for (int k = 0; k < currentModule.size; k ++) {
        out << "<tr><td>\\(" << currentModule[k].toString() << "\\)</td>";
        bool oppositesAreGood = false;
        if (i < this->modulesSl2Opposite.size) {
          if (j < this->modulesSl2Opposite[i].size) {
            if (k < this->modulesSl2Opposite[i][j].size) {
              oppositesAreGood = true;
            }
          }
        }
        if (oppositesAreGood) {
          List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& currentOpModule
          =
          this->modulesSl2Opposite[i][j];
          if (!currentOpModule[k].isEqualToZero()) {
            out
            << "<td><span style='color:blue'>"
            << currentOpModule[k].toString()
            << "</span></td>";
            this->getAmbientSemisimpleLieAlgebra().lieBracket(
              currentModule[k], currentOpModule[k], tempLieBracket
            );
            out
            << "<td><span style='color:red'>"
            << tempLieBracket.toString()
            << "</span></td>";
          } else {
            out << "<td><span style='color:blue'>N/A</span></td>";
          }
        }
        out << "</tr>";
      }
      out << "</table>";
      out << "</td>";
    }
    out << "</tr></table>";
    out << "</td>";
  }
  out << "</tr>";
  out << "<tr>";
  out
  << "<td>Weights of elements in fundamental coords w.r.t. "
  << "Cartan of subalgebra in same order as above</td>";
  for (int i = 0; i < this->weightsModulesNONprimal.size; i ++) {
    out << "<td>";
    for (int j = 0; j < this->weightsModulesNONprimal[i].size; j ++) {
      out
      << "\\("
      << this->weightsModulesNONprimal[i][j].toStringLetterFormat(
        "\\omega", &characterFormat
      )
      << "\\)";
      if (j != this->weightsModulesNONprimal[i].size - 1) {
        out << "<br>";
      }
    }
    out << "</td>";
  }
  out << "</tr>";
  out << "<tr>";
  out
  << "<td>Weights of elements in "
  << "(fundamental coords w.r.t. Cartan of subalgebra) "
  << "+ Cartan centralizer</td>";
  for (int i = 0; i < this->weightsModulesPrimal.size; i ++) {
    out << "<td>";
    for (int j = 0; j < this->weightsModulesPrimal[i].size; j ++) {
      out
      << "\\("
      << this->weightsModulesPrimal[i][j].toStringLetterFormat(
        "\\omega", &characterFormat
      )
      << "\\)";
      if (j != this->weightsModulesPrimal[i].size - 1) {
        out << "<br>";
      }
    }
    out << "</td>";
  }
  out << "</tr>";
  out << "<tr>";
  characterFormat.finiteDimensionalRepresentationLetter = "M";
  out
  << "<td>Single module character over Cartan of "
  << "s.a.+ Cartan of centralizer of s.a.</td>";
  if (this->charactersPrimalModules.size > 0) {
    for (int i = 0; i < this->charactersPrimalModules.size; i ++) {
      out << "<td>";
      out
      << HtmlRoutines::getMathNoDisplay(
        this->charactersPrimalModules[i].toString(&characterFormat)
      );
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
  if (this->charactersPrimalModulesMerged.size > 0) {
    for (int i = 0; i < this->charactersPrimalModulesMerged.size; i ++) {
      out << "<td>";
      out
      << HtmlRoutines::getMathNoDisplay(
        this->charactersPrimalModulesMerged[i].toString(&characterFormat)
      );
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
  out
  << "<br>Semisimple subalgebra: "
  << semisimpleSAv.toStringLetterFormat("W");
  out
  << "<br>Centralizer extension: "
  << centralizerV.toStringLetterFormat("W");
  return out.str();
}

std::string NilradicalCandidate::toStringTableElementWithWeights(
  const List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& elements,
  const Vectors<Rational>& weights
) const {
  std::stringstream out;
  out << "<table border='1'><tr>";
  for (int i = 0; i < weights.size; i ++) {
    out << "<td><table><tr><td>" << weights[i].toString() << "</td></tr>";
    for (int j = 0; j < weights.size; j ++) {
      if (weights[i] == weights[j]) {
        out << "<tr><td>" << elements[j].toString() << "</td></tr>";
      }
    }
    out << "</table></td>";
  }
  out << "</tr></table>";
  return out.str();
}

std::string NilradicalCandidate::toString(FormatExpressions* format) const {
  STACK_TRACE("NilradicalCandidate::toString");
  (void) format;
  // avoid unused parameter warning in a portable way.
  std::stringstream out;
  out << this->FKnilradicalLog;
  Vector<Rational> currentNilradical;
  currentNilradical = this->nilradicalSelection;
  out << currentNilradical.toStringLetterFormat("W");
  if (this->flagNilradicalConesIntersect) {
    out << ". Cones intersect. ";
  } else {
    out << ". Cones don't intersect. ";
  }
  if (this->nilradicalWeights.size > 0) {
    out
    << "<br>Nilradical cone: <br>"
    << this->toStringTableElementWithWeights(
      this->nilradical, this->nilradicalWeights
    );
  }
  out
  << "<br> Highest weight cone:<br> "
  << this->toStringTableElementWithWeights(
    this->nonFernandoKacHighestWeightVectors,
    this->nonFernandoKacHighestWeights
  );
  if (this->flagNilradicalConesIntersect) {
    out
    << "<br>Strongly singular weights: "
    << this->nonFernandoKacHighestWeightsStronglyTwoSided.toString();
    out
    << "<br>Cone intersection: "
    << this->coneIntersection.toStringLetterFormat("w");
    out << "<br> ";
    FormatExpressions currentFormat;
    currentFormat.vectorSpaceEiBasisNames.setSize(
      this->coneIntersection.size
    );
    for (int j = 0; j < this->nilradicalWeights.size; j ++) {
      currentFormat.vectorSpaceEiBasisNames[j] =
      this->nilradicalWeights[j].toString();
    }
    for (int j = 0; j < this->nonFernandoKacHighestWeights.size; j ++) {
      currentFormat.vectorSpaceEiBasisNames[
        j + this->nilradicalWeights.size
      ] =
      this->nonFernandoKacHighestWeights[j].toString();
    }
    out << this->coneIntersection.toStringLetterFormat("w", &currentFormat);
    if (this->flagNilradicalConesStronglyIntersect) {
      for (
        int j = 0; j < this->nonFernandoKacHighestWeightsStronglyTwoSided.size;
        j ++
      ) {
        currentFormat.vectorSpaceEiBasisNames[
          j + this->nilradicalWeights.size
        ] =
        this->nonFernandoKacHighestWeightsStronglyTwoSided[j].toString();
      }
      out
      << "<br>In addition, the nilradical cones intersect strongly "
      << "at weight "
      << this->getConeStrongIntersectionWeight().toString();
      out
      << "<br>"
      << this->coneStrongIntersection.toStringLetterFormat(
        "w", &currentFormat
      );
      out << "<br>The involved nilradical elements: " << "<br><table><tr>";
      for (int i = 0; i < this->nilradicalWeights.size; i ++) {
        if (this->coneStrongIntersection[i] != 0) {
          Vector<Rational>& currentNilradicalWeight =
          this->nilradicalWeights[i];
          out
          << "<td><table border =\"1\"><tr><td>"
          << currentNilradicalWeight.toString()
          << "</td></tr>";
          for (int j = 0; j < this->nilradicalWeights.size; j ++) {
            if (currentNilradicalWeight == this->nilradicalWeights[j]) {
              out
              << "<tr><td>"
              << this->nilradical[j].toString()
              << "</td></tr>";
              out
              << "<tr><td>"
              << this->nilradicalElementOpposites[j].toString()
              << "</td></tr>";
            }
          }
          out
          << "<tr><td>Scal. prod. <br> L-inf. rel.: "
          << this->owner->getScalarSubalgebra(
            currentNilradicalWeight,
            this->getConeStrongIntersectionWeight()
          )
          << "</td></td>";
          out << "</table>";
          out << "</td>";
        }
      }
      out << "</tr></table>";
    }
    if (
      this->flagComputedRelativelyStrongIntersections &&
      this->flagLinfiniteRelFound
    ) {
      currentFormat.vectorSpaceEiBasisNames.setSize(
        this->coneRelativelyStrongIntersection.size
      );
      for (int j = 0; j < this->nilradicalSubset.size; j ++) {
        currentFormat.vectorSpaceEiBasisNames[j] =
        this->nilradicalSubsetWeights[j].toString();
      }
      for (
        int j = 0; j < this->
        nonFernandoKacHighestWeightVectorsStrongRelativeToSubset.size; j ++
      ) {
        currentFormat.vectorSpaceEiBasisNames[
          j + this->nilradicalSubset.size
        ] =
        this->nonFernandoKacHighestWeightVectorsStrongRelativeToSubsetWeights[
          j
        ].toString();
      }
      out
      << "<br>In addition, strong nilradical "
      << "cone intersections did not suffice,"
      << " I needed to compute relatively strong intersections. ";
      out
      << "<br>The nilradical subset:"
      << this->toStringTableElementWithWeights(
        this->nilradicalSubset, this->nilradicalSubsetWeights
      );
      out
      << "<br>The highest weight cone consisting of "
      << "elements commuting with the nilradical subset: "
      << this->toStringTableElementWithWeights(
        this->nonFernandoKacHighestWeightVectorsStrongRelativeToSubset,
        this->nonFernandoKacHighestWeightVectorsStrongRelativeToSubsetWeights
      );
      out
      << "<br>Relation: "
      << this->coneRelativelyStrongIntersection.toStringLetterFormat(
        "w", &currentFormat
      );
    }
    if (this->flagLinfiniteRelFound) {
      out << "<br><b>L-infinite relation exists!</b>";
    } else {
      out << "<br><b style='color:red'>No L-infinite relation found.</b>";
    }
  } else {
    if (!this->flagParabolicACExtendsToParabolicAC) {
      out
      << "<br><b style = 'color:red'>"
      << "Strong centralizer condition not proven to hold.</b>";
    }
    if (this->flagRestrictedCentralizerConditionHoldS) {
      out << "<br>The centralizer condition holds. ";
    }
    out
    << "<br>Levi roots primal fundamental coordinates: "
    << this->leviRootsSmallPrimalFundamentalCoordinates.toString()
    << "<br>Levi components in centralizer: "
    << this->leviDiagramSmall.toString()
    << "<br>Levi roots containing parabolic: "
    << this->leviRootsAmbient.toString()
    << "<br>Levi components containing parabolic: "
    << this->leviDiagramAmbient.toString();
    out
    << "<br>Separating hyperplane: "
    << this->coneSeparatingNormal.toStringLetterFormat("u");
  }
  return out.str();
}

void NilradicalCandidate::
getModGeneratedByNonHighestWeightVectorAndNilradElement(
  int indexInNilradSubset,
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputLeft,
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputRight,
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputBrackets
) const {
  STACK_TRACE(
    "NilradicalCandidate::"
    "getModGeneratedByNonHighestWeightVectorAndNilradElement"
  );
  outputBrackets.setSize(0);
  outputBrackets.addOnTop(this->nilradicalSubset[indexInNilradSubset]);
  outputRight.setSize(1);
  outputLeft.setSize(1);
  ElementSemisimpleLieAlgebra<AlgebraicNumber> lieBracket;
  for (int i = 0; i < outputBrackets.size; i ++) {
    if (!outputBrackets[i].isEqualToZero()) {
      for (
        int j = 0; j < this->
        nonFernandoKacHighestWeightVectorsStrongRelativeToSubset.size; j ++
      ) {
        this->owner->owner->owner->lieBracket(
          this->nonFernandoKacHighestWeightVectorsStrongRelativeToSubset[j],
          outputBrackets[i],
          lieBracket
        );
        outputBrackets.addOnTop(lieBracket);
        outputLeft.addOnTop(
          this->nonFernandoKacHighestWeightVectorsStrongRelativeToSubset[j]
        );
        outputRight.addOnTop(outputBrackets[i]);
      }
    }
  }
  outputLeft.popIndexShiftDown(0);
  outputRight.popIndexShiftDown(0);
  outputBrackets.popIndexShiftDown(0);
}

std::string CandidateSemisimpleSubalgebra::toStringNilradicalsSummary(
  FormatExpressions* format
) const {
  STACK_TRACE("CandidateSemisimpleSubalgebra::toStringNilradicalsSummary");
  (void) format;
  // avoid unused parameter warning in a portable way.
  if (this->fernandoKacNilradicalCandidates.size == 0) {
    return "";
  }
  std::stringstream out;
  out
  << "<br>There are "
  << this->fernandoKacNilradicalCandidates.size
  << " possible isotypic nilradical extensions "
  << "of the primal subalgebra. Of them "
  << this->numberOfConeIntersections
  << " have intersecting cones. Of the remaining "
  << this->fernandoKacNilradicalCandidates.size -
  this->numberOfConeIntersections
  << " nilradical extensions with non-intersecting cones, "
  << this->fernandoKacNilradicalCandidates.size -
  this->numberOfConeIntersections -
  this->numberOfCentralizerConditionFailsConeConditionHolds
  << " satisfy the centralizer condition and "
  << this->numberOfCentralizerConditionFailsConeConditionHolds
  << " fail the centralizer condition.";
  if (this->numberOfBadParabolics > 0) {
    out
    << "<br><span style =\"color:#FF0000\">"
    << "Of the subalgebra(s) satisfying the centralizer condition,  "
    << this->numberOfBadParabolics
    << " have (has) centralizer parabolics that do not extend to  "
    << " parabolics of the ambient Lie algebra with Levi types A and C. "
    << "For these subalgebras the PSZ "
    << "construction is not proven to hold. </span>";
  } else {
    out
    << "<br><span style =\"color:#0000FF\"> In each of "
    << this->fernandoKacNilradicalCandidates.size -
    this->numberOfConeIntersections -
    this->numberOfCentralizerConditionFailsConeConditionHolds
    << " case(s) when the centralizer condition holds, "
    << "the parabolic subalgebra in the "
    << "centralizer with Levi types A and C extends "
    << "to parabolic subalgebra of the ambient "
    << "Lie algebra whose Levi types are A and C only. </span>";
  }
  if (this->numberOfConeIntersections > 0) {
    if (this->numberOfCasesNoLInfiniteRelationFound > 0) {
      out
      << "<br><span style='color:red'>In "
      << this->numberOfCasesNoLInfiniteRelationFound
      << " cases no L-infinite relation was found. </span>";
    } else {
      out
      << "<br><span style='color:red'> In each of "
      << this->numberOfConeIntersections
      << " case(s) of intersecting cones, an "
      << "L-infinite relation was found. </span>";
    }
  }
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::toStringNilradicals(
  FormatExpressions* format
) const {
  STACK_TRACE("CandidateSemisimpleSubalgebra::toStringNilradicals");
  if (this->fernandoKacNilradicalCandidates.size == 0) {
    if (this->owner->flagComputeNilradicals) {
      if (this->isRegularSubalgebra()) {
        return
        "Nilradicals not computed, but "
        "that is OK because this a root subalgebra. ";
      } else {
        return
        "<b>Nilradicals not computed AND this is not a root subalgebra.</b>";
      }
    } else {
      return "";
    }
  }
  std::stringstream out;
  out << this->toStringNilradicalsSummary(format);
  Vector<Rational> primalBase;
  primalBase = this->fernandoKacNilradicalCandidates[0].nilradicalSelection;
  out
  << "<br>The primal extension of the semisimple subalgerba equals: "
  << primalBase.toStringLetterFormat("W");
  if (this->owner->flagProduceLaTeXTables) {
    Vector<Rational> currentNilradicalVector;
    out
    << "<br><br>A summary in LaTeX <br>\\begin{longtable}{c|c|c|c }"
    << "\\caption{Nilradicals\\label{tableNilrads} }\\\\ "
    << "$ \\mathfrak n _{\\mathfrak l} "
    << "$ & Cones intersect & Cone intersection ";
    FormatExpressions currentFormat;
    if (!this->characterFormat.isZeroPointer()) {
      currentFormat = this->characterFormat.getElementConst();
    }
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >
    relevantBracketsLeft,
    relevantBracketsRight,
    relevantBrackets;
    for (int i = 0; i < this->fernandoKacNilradicalCandidates.size; i ++) {
      const NilradicalCandidate& currentNilradical =
      this->fernandoKacNilradicalCandidates[i];
      currentNilradicalVector = currentNilradical.nilradicalSelection;
      for (int j = 0; j < this->primalSubalgebraModules.size; j ++) {
        currentNilradicalVector[this->primalSubalgebraModules[j]] -= 1;
      }
      out << "\\\\\\hline<br>\n";
      out << "$" << currentNilradicalVector.toStringLetterFormat("W") << "$ &";
      if (currentNilradical.flagNilradicalConesIntersect) {
        out << "yes";
      } else {
        out << "no";
      }
      out << "&";
      if (currentNilradical.flagLinfiniteRelFound) {
        out << "\\begin{tabular}{l";
        for (
          int j = 0; j < currentNilradical.
          nonFernandoKacHighestWeightVectorsStrongRelativeToSubsetWeights.size
          +
          currentNilradical.nilradicalSubsetWeights.size; j ++
        ) {
          out << "cc";
        }
        out << "}Relation&";
        for (
          int j = 0; j < currentNilradical.
          nonFernandoKacHighestWeightVectorsStrongRelativeToSubsetWeights.size;
          j ++
        ) {
          Rational coefficient =
          currentNilradical.coneRelativelyStrongIntersection[
            currentNilradical.nilradicalSubsetWeights.size + j
          ];
          coefficient.negate();
          out
          << "$"
          << (
            currentNilradical.
            nonFernandoKacHighestWeightVectorsStrongRelativeToSubsetWeights[j]
            *
            coefficient
          ).toStringLetterFormat("\\omega", &currentFormat)
          << "$";
          out << " & ";
          if (
            j !=
            currentNilradical.
            nonFernandoKacHighestWeightVectorsStrongRelativeToSubsetWeights.
            size -
            1
          ) {
            out << "+&";
          } else {
            out << "= &";
          }
        }
        for (
          int j = 0; j < currentNilradical.nilradicalSubsetWeights.size; j ++
        ) {
          Rational coefficient =
          currentNilradical.coneRelativelyStrongIntersection[j];
          out
          << "$"
          << (currentNilradical.nilradicalSubsetWeights[j] * coefficient).
          toStringLetterFormat("\\omega", &currentFormat)
          << "$";
          out << " & ";
          if (j != currentNilradical.nilradicalSubsetWeights.size - 1) {
            out << "+&";
          }
        }
        out << "\\\\<br>\n Elts.& ";
        for (
          int j = 0; j < currentNilradical.
          nonFernandoKacHighestWeightVectorsStrongRelativeToSubset.size; j ++
        ) {
          out
          << "$"
          << currentNilradical.
          nonFernandoKacHighestWeightVectorsStrongRelativeToSubset[j].toString(
          )
          << "$";
          out << " & &";
        }
        for (
          int j = 0; j < currentNilradical.nilradicalSubset.size; j ++
        ) {
          out
          << "$"
          << currentNilradical.nilradicalSubset[j].toString()
          << "$";
          out << " & ";
          if (j != currentNilradical.nilradicalSubset.size - 1) {
            out << " &";
          }
        }
        out << "\\\\<br>\n Opposite elts. &";
        for (
          int j = 0; j < currentNilradical.
          nonFernandoKacHighestWeightVectorsStrongRelativeToSubset.size; j ++
        ) {
          out << " & &";
        }
        for (
          int j = 0; j < currentNilradical.nilradicalSubset.size; j ++
        ) {
          out
          << "$"
          << currentNilradical.nilradicalElementOpposites[
            currentNilradical.nilradicalSubselection.elements[j]
          ].toString()
          << "$";
          out << " & ";
          if (j != currentNilradical.nilradicalSubset.size - 1) {
            out << " &";
          }
        }
        out << "\\end{tabular}\\\\\n<br>";
        out << "&& Relevant Lie brackets: ";
        ElementSemisimpleLieAlgebra<AlgebraicNumber> element;
        std::stringstream currentStream;
        for (
          int j = 0; j < currentNilradical.nilradicalSubset.size; j ++
        ) {
          currentNilradical.
          getModGeneratedByNonHighestWeightVectorAndNilradElement(
            j, relevantBracketsLeft, relevantBracketsRight, relevantBrackets
          );
          for (int k = 0; k < relevantBrackets.size; k ++) {
            currentStream
            << "$["
            << relevantBracketsLeft[k].toString()
            << ", "
            << relevantBracketsRight[k].toString()
            << "] ="
            << relevantBrackets[k].toString()
            << "$, ";
          }
        }
        for (
          int j = 0; j < currentNilradical.nilradicalSubset.size; j ++
        ) {
          for (
            int k = 0; k < currentNilradical.nilradicalSubset.size; k ++
          ) {
            if (k != j) {
              this->owner->owner->lieBracket(
                currentNilradical.nilradicalSubset[j],
                currentNilradical.nilradicalElementOpposites[
                  currentNilradical.nilradicalSubselection.elements[k]
                ],
                element
              );
              currentStream
              << "$["
              << currentNilradical.nilradicalSubset[j].toString()
              << ", "
              << currentNilradical.nilradicalElementOpposites[
                currentNilradical.nilradicalSubselection.elements[k]
              ].toString()
              << "] ="
              << element.toString()
              << ", ";
            }
          }
        }
        std::string currentString = currentStream.str();
        currentString.resize(currentString.size() - 2);
        out << currentString;
      }
    }
    out << "\\end{longtable}";
  }
  for (int i = 0; i < this->fernandoKacNilradicalCandidates.size; i ++) {
    out
    << "<hr>Subalgebra "
    << i + 1
    << ": "
    << this->fernandoKacNilradicalCandidates[i].toString(format);
  }
  if (this->nilradicalGenerationLog != "") {
    out << "<br>Nilradical generation log:" << this->nilradicalGenerationLog;
  }
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::toStringPairingTableLaTeX(
  FormatExpressions* format
) const {
  if (!(this->nilradicalPairingTable.size > 0)) {
    return "";
  }
  (void) format;
  // avoid unused parameter warning in a portable way.
  std::stringstream out;
  out
  << "\\documentclass{article}\\usepackage{longtable} \\begin{document}<br>";
  out
  << "modules that have a zero weight ("
  << this->modulesWithZeroWeights.size
  << " total): ";
  for (int i = 0; i < this->modulesWithZeroWeights.size; i ++) {
    out << "$W_{" << this->modulesWithZeroWeights[i] + 1 << "}$";
    if (i != this->modulesWithZeroWeights.size - 1) {
      out << ", ";
    }
  }
  out << "<br>Opposite modules <br>\n" << " \\begin{longtable}{c";
  for (int i = 0; i < this->oppositeModulesByStructure.size; i ++) {
    out << "|c";
  }
  out << "}\\\\ ";
  for (int i = 0; i < this->oppositeModulesByStructure.size; i ++) {
    out << "&$W_{" << i + 1 << "}$";
  }
  out << "\\\\\\hline Opp. by structure:";
  for (int i = 0; i < this->oppositeModulesByStructure.size; i ++) {
    out << "&";
    for (
      int j = 0; j < this->oppositeModulesByStructure[i].size; j ++
    ) {
      out << "$W_{" << this->oppositeModulesByStructure[i][j] + 1 << "}$";
      if (j != this->oppositeModulesByStructure[i].size - 1) {
        out << ", ";
      }
    }
  }
  out << "\\\\\\hline Opposite character";
  for (int i = 0; i < this->oppositeModulesByChar.size; i ++) {
    out << "&";
    for (int j = 0; j < this->oppositeModulesByChar[i].size; j ++) {
      out << "$W_{" << this->oppositeModulesByChar[i][j] + 1 << "} $";
      if (j != this->oppositeModulesByChar[i].size - 1) {
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
  FormatExpressions characterFormat;
  if (!this->characterFormat.isZeroPointer()) {
    characterFormat = this->characterFormat.getElementConst();
  }
  out << "<br>\n";
  for (int i = 0; i < this->nilradicalPairingTable.size; i ++) {
    out << "&$W_{" << i + 1 << "}$";
  }
  for (int i = 0; i < this->nilradicalPairingTable.size; i ++) {
    out << "\\\\\\hline<br\n>";
    out << "$W_{" << i + 1 << "}$";
    for (int j = 0; j < this->nilradicalPairingTable.size; j ++) {
      out << "&";
      for (
        int k = 0; k < this->nilradicalPairingTable[i][j].size; k ++
      ) {
        out << "$W_{" << this->nilradicalPairingTable[j][i][k] + 1 << "}$";
        if (k != this->nilradicalPairingTable[i][j].size - 1) {
          out << ", ";
        }
      }
    }
  }
  out << "\\end{longtable}";
  out << "<br>\\end{document}";
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::toStringPairingTable(
  FormatExpressions* format
) const {
  STACK_TRACE("CandidateSemisimpleSubalgebra::toStringPairingTable");
  (void) format;
  if (!(this->nilradicalPairingTable.size > 0)) {
    return "";
  }
  std::stringstream out;
  out
  << "<br>Below is the pairing table of the modules, defined as follows. "
  << "Let g' be the subalgebra "
  << "obtained by extending the current semisimple subalgebra "
  << "with the Cartan of the centralizer "
  << "(following Penkov, Serganova, Zuckermann, we "
  << "call a subalgebra g' arising in this fashion ``primal''). "
  << "Let A, B, C be  isotypic components in "
  << "the decomposition of the ambient Lie algebra over g'. "
  << "We say that A and B pair to C if there exist "
  << "elements a in A and b in B such that  0\\neq [a,b]\\in C. "
  << "Note that, in general, A and B may "
  << "pair to multiple modules C', C'', etc. We note that "
  << "if A and B pair to C then clearly C is isomorphic to some component "
  << "in the decomposition of A\\otimes B over g'. "
  << "<br> We recall that W_{1}, W_{2}, ... are abbreviated notation "
  << "for the primal subalgebra modules indicated in the table above. ";
  out
  << "<br>modules that have a zero weight ("
  << this->modulesWithZeroWeights.size
  << " total): ";
  for (int i = 0; i < this->modulesWithZeroWeights.size; i ++) {
    out << "V_{" << this->modulesWithZeroWeights[i] + 1 << "}";
    if (i != this->modulesWithZeroWeights.size - 1) {
      out << ", ";
    }
  }
  out << "<br><table border =\"1\"><tr><td></td>";
  for (int i = 0; i < this->oppositeModulesByStructure.size; i ++) {
    out << "<td>W_{" << i + 1 << "}" << "</td>";
  }
  out << "</tr>";
  out << "<tr> <td>Non-compatible (a.k.a. ``opposite'') modules:</td>";
  for (int i = 0; i < this->oppositeModulesByStructure.size; i ++) {
    out << "<td>";
    for (
      int j = 0; j < this->oppositeModulesByStructure[i].size; j ++
    ) {
      out << "W_{" << this->oppositeModulesByStructure[i][j] + 1 << "}";
      if (j != this->oppositeModulesByStructure[i].size - 1) {
        out << ", ";
      }
    }
    out << "</td>";
  }
  out << "</tr>";
  out << "<tr> <td>Opposite modules by character:</td>";
  for (int i = 0; i < this->oppositeModulesByChar.size; i ++) {
    out << "<td>";
    for (int j = 0; j < this->oppositeModulesByChar[i].size; j ++) {
      out << "W_{" << this->oppositeModulesByChar[i][j] + 1 << "}";
      if (j != this->oppositeModulesByChar[i].size - 1) {
        out << ", ";
      }
    }
    out << "</td>";
  }
  out << "</tr>";
  out << "</table>";
  out << "<br>modules corresponding to the semisimple subalgebra: ";
  Vector<Rational> subalgebraVector;
  Vector<Rational> currentVector;
  subalgebraVector.makeZero(this->nilradicalPairingTable.size);
  for (int i = 0; i < this->subalgebraModules.size; i ++) {
    currentVector.makeEi(
      this->nilradicalPairingTable.size, this->subalgebraModules[i]
    );
    subalgebraVector += currentVector;
  }
  out << subalgebraVector.toStringLetterFormat("V");
  out << "<br><table><tr><td>modules</td>";
  FormatExpressions characterFormat;
  if (!this->characterFormat.isZeroPointer()) {
    characterFormat = this->characterFormat.getElementConst();
  }
  for (int i = 0; i < this->nilradicalPairingTable.size; i ++) {
    out << "<td><b>" << "W_{" << i + 1 << "}" << "</b></td>";
  }
  out << "</tr>";
  for (int i = 0; i < this->nilradicalPairingTable.size; i ++) {
    out << "<tr><td> <b>" << "W_{" << i + 1 << "}</b></td>";
    for (int j = 0; j < this->nilradicalPairingTable.size; j ++) {
      out << "<td>";
      for (
        int k = 0; k < this->nilradicalPairingTable[i][j].size; k ++
      ) {
        out << "W_{" << this->nilradicalPairingTable[j][i][k] + 1 << "}";
        if (k != this->nilradicalPairingTable[i][j].size - 1) {
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

void DynkinType::scaleFirstCoRootSquaredLength(
  const Rational& multiplyCoRootSquaredLengthBy
) {
  DynkinType result;
  result.makeZero();
  result.setExpectedSize(this->size());
  DynkinSimpleType simpleType;
  for (int i = 0; i < this->size(); i ++) {
    simpleType = (*this)[i];
    simpleType.cartanSymmetricInverseScale *= multiplyCoRootSquaredLengthBy;
    result.addMonomial(simpleType, this->coefficients[i]);
  }
  *this = result;
}

std::string SemisimpleSubalgebras::toStringAlgebraLink(
  int actualindexSubalgebra, FormatExpressions* format, bool writeToHardDisk
) const {
  if (actualindexSubalgebra < 0) {
    return "(non-initialized)";
  }
  std::stringstream out;
  bool makeLink = writeToHardDisk;
  if (
    this->subalgebras.values[actualindexSubalgebra].
    flagSystemProvedToHaveNoSolution
  ) {
    makeLink = false;
  }
  std::string typeString =
  this->subalgebras.values[actualindexSubalgebra].weylNonEmbedded->dynkinType.
  toString();
  if (makeLink) {
    out
    << "<a href='"
    << this->getDisplayFileNameSubalgebraRelative(
      actualindexSubalgebra, format
    )
    << "' id='semisimple_subalgebra_"
    << this->getDisplayIndexFromActual(actualindexSubalgebra)
    << "' style='text-decoration: none'>";
    out << HtmlRoutines::getMathNoDisplay(typeString);
    out << "</a> ";
  } else {
    out << typeString << "";
  }
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::toStringCartanSubalgebra(
  FormatExpressions* format
) const {
  std::stringstream out;
  bool useLaTeX = format == nullptr ? true : format->flagUseLatex;
  bool useHtml = format == nullptr ? true : format->flagUseHTML;
  List<DynkinSimpleType> simpleTypes;
  this->weylNonEmbedded->dynkinType.getTypesWithMults(simpleTypes);
  FormatExpressions currentFormat;
  currentFormat.ambientWeylLetter =
  this->getAmbientWeyl().dynkinType[0].letter;
  currentFormat.ambientCartanSymmetricInverseScale =
  this->getAmbientWeyl().dynkinType[0].cartanSymmetricInverseScale;
  out << "<br>Elements Cartan subalgebra scaled to act by two by components: ";
  if (this->cartanSubalgebrasByComponentScaledToActByTwo.size == 0) {
    out << "[not computed]";
  }
  for (
    int i = 0; i < this->cartanSubalgebrasByComponentScaledToActByTwo.size; i
    ++
  ) {
    if (useLaTeX && useHtml) {
      out
      << HtmlRoutines::getMathNoDisplay(
        simpleTypes[i].toString(&currentFormat)
      )
      << ": ";
    } else {
      out << simpleTypes[i].toString(&currentFormat) << ":";
    }
    for (
      int j = 0; j < this->cartanSubalgebrasByComponentScaledToActByTwo[i].
      size; j ++
    ) {
      out
      << this->cartanSubalgebrasByComponentScaledToActByTwo[i][j].toString()
      << ": "
      << this->getAmbientWeyl().rootScalarCartanRoot(
        this->cartanSubalgebrasByComponentScaledToActByTwo[i][j],
        this->cartanSubalgebrasByComponentScaledToActByTwo[i][j]
      );
      if (
        j != this->cartanSubalgebrasByComponentScaledToActByTwo[i].size - 1 ||
        i != this->cartanSubalgebrasByComponentScaledToActByTwo.size - 1
      ) {
        out << ", ";
      }
    }
  }
  return out.str();
}

int CandidateSemisimpleSubalgebra::
getSemisimplePartCentralizerOfSemisimplePartCentralizer() const {
  STACK_TRACE(
    "CandidateSemisimpleSubalgebra::"
    "getSemisimplePartCentralizerOfSemisimplePartCentralizer"
  );
  if (!this->flagCentralizerIsWellChosen) {
    return - 1;
  }
  if (this->indexSemisimplePartCentralizer == - 1) {
    return this->owner->getIndexFullSubalgebra();
  }
  return
  this->owner->subalgebras.values[this->indexSemisimplePartCentralizer].
  indexSemisimplePartCentralizer;
}

std::string CandidateSemisimpleSubalgebra::toStringCentralizer(
  FormatExpressions* format, bool writeToHardDisk
) const {
  STACK_TRACE("CandidateSemisimpleSubalgebra::toStringCentralizer");
  if (this->flagSystemProvedToHaveNoSolution) {
    return "";
  }
  std::stringstream out;
  if (this->flagCentralizerIsWellChosen && this->centralizerRank != 0) {
    out << "<br>Centralizer: ";
    Rational dimToralPartCentralizer = this->centralizerRank;
    if (this->indexSemisimplePartCentralizer != - 1) {
      CandidateSemisimpleSubalgebra& centralizerSSpart =
      this->owner->subalgebras.values[this->indexSemisimplePartCentralizer];
      out
      << this->owner->toStringAlgebraLink(
        this->indexSemisimplePartCentralizer, format, writeToHardDisk
      );
      dimToralPartCentralizer -= centralizerSSpart.getRank();
      if (dimToralPartCentralizer != 0) {
        out << " + ";
      }
    } else if (!this->centralizerType.isEqualToZero()) {
      out
      << HtmlRoutines::getMathNoDisplay(this->centralizerType.toString());
      out
      << " (can't determine subalgebra number "
      << "- subalgebras computed partially?)";
      dimToralPartCentralizer -= this->centralizerType.getRank();
    }
    if (dimToralPartCentralizer != 0) {
      std::stringstream toralPartStream;
      toralPartStream << "T_{" << dimToralPartCentralizer.toString() << "}";
      out << HtmlRoutines::getMathNoDisplay(toralPartStream.str());
      out << " (toral part, subscript = dimension)";
    }
    out << ". ";
  }
  if (this->flagCentralizerIsWellChosen && this->centralizerRank == 0) {
    out << "<br>Centralizer: 0";
  }
  if (
    this->flagCentralizerIsWellChosen &&
    this->getSemisimplePartCentralizerOfSemisimplePartCentralizer() != - 1
  ) {
    out
    << "<br>The semisimple part of the centralizer "
    << "of the semisimple part of my centralizer: "
    << this->owner->toStringAlgebraLink(
      this->getSemisimplePartCentralizerOfSemisimplePartCentralizer(),
      format,
      writeToHardDisk
    );
  }
  if (!this->flagCentralizerIsWellChosen) {
    if (this->centralizerRank >= 0) {
      out
      << "<br><b style='color:red'>The Cartan of the centralizer "
      << "does not lie in the ambient Cartan "
      << "(the computation was too large?).</b> "
      << "The intersection of the ambient "
      << "Cartan with the centralizer is of dimension "
      << this->cartanOfCentralizer.size
      << " instead of the desired dimension "
      << this->centralizerRank.toString()
      << ". ";
    } else {
      out << "<br><b style='color:red'>Centralizer type not computed.</b>";
    }
  }
  if (this->centralizerRank != 0) {
    out << "<br>Basis of Cartan of centralizer: ";
    out << this->cartanOfCentralizer.toString();
  }
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::toStringCentralizerDebugData(
  FormatExpressions* format
) const {
  STACK_TRACE("CandidateSemisimpleSubalgebra::toStringCentralizerDebugData");
  (void) format;
  // avoid unused parameter warning in a portable way.
  std::stringstream out;
  out
  << "Subalgebra of type: "
  << this->weylNonEmbedded->dynkinType.toString()
  << ". this->centralizerRank computed: "
  << this->centralizerRank.toString()
  << ". this->centralizerDimension: "
  << this->centralizerDimension.toString()
  << ". The max semisimple subalgebra container computed: "
  << this->owner->subalgebras.values[this->indexMaximalSemisimpleContainer].
  weylNonEmbedded->dynkinType.toString()
  << ". The rank of the ambient Lie algebra is "
  << this->owner->owner->getRank()
  << ". The indices of the direct containers of the subalgebra are: ";
  for (int k = 0; k < this->indicesDirectSummandSuperAlgebra.size; k ++) {
    out
    << this->owner->subalgebras.values[
      this->indicesDirectSummandSuperAlgebra[k]
    ].weylNonEmbedded->dynkinType.toString()
    << ", ";
  }
  return out.str();
}

void CandidateSemisimpleSubalgebra::computeCentralizerManually() {
  STACK_TRACE("CandidateSemisimpleSubalgebra::computeCentralizerManually");
  this->owner->owner->getCommonCentralizer(
    this->basis, this->centralizerManualBasis
  );
  this->centralizerRank = - 1;
  this->flagCentralizerTypeIsComputed = false;
  global.comments
  << "Centralizer manual basis: "
  << this->centralizerManualBasis.toString();
  this->killingFormCentralizer.initialize(
    this->centralizerManualBasis.size,
    this->centralizerManualBasis.size
  );
  for (int i = 0; i < this->centralizerManualBasis.size; i ++) {
    for (int j = 0; j < this->centralizerManualBasis.size; j ++) {
      this->killingFormCentralizer(i, j) =
      this->owner->owner->getKillingForm(
        this->centralizerManualBasis[i],
        this->centralizerManualBasis[j]
      );
    }
  }
}

void CandidateSemisimpleSubalgebra::computeCentralizerIsWellChosen() {
  STACK_TRACE("CandidateSemisimpleSubalgebra::computeCentralizerIsWellChosen");
  this->flagCentralizerIsWellChosen = false;
  if (this->flagSystemProvedToHaveNoSolution) {
    global.comments
    << "This is unexpected, but not considered an error: "
    << "I am asked to compute the centralizer of "
    << "a candidate for a subalgebra which cannot be realized. ";
    this->flagCentralizerIsWellChosen = false;
    return;
  }
  Weight<Rational> zeroWeight;
  zeroWeight.owner = nullptr;
  zeroWeight.weightFundamentalCoordinates.makeZero(
    this->cartanElementsSubalgebra.size
  );
  this->centralizerDimension =
  this->characterNonPrimalFundamentalCoordinates.getCoefficientOf(zeroWeight);
  this->centralizerRank = this->centralizerDimension;
  if (this->centralizerRank == 0) {
    this->flagCentralizerIsWellChosen = true;
    return;
  }
  ProgressReport report1;
  DynkinType centralizerTypeAlternative;
  if (this->indexMaximalSemisimpleContainer != - 1) {
    centralizerTypeAlternative =
    this->owner->subalgebras.values[this->indexMaximalSemisimpleContainer].
    weylNonEmbedded->dynkinType;
    centralizerTypeAlternative -= this->weylNonEmbedded->dynkinType;
    if (report1.tickAndWantReport()) {
      std::stringstream reportStream;
      reportStream
      << "<hr>The centralizer of subalgebra of type "
      << this->weylNonEmbedded->dynkinType.toString()
      << " is of type "
      << centralizerTypeAlternative.toString();
      report1.report(reportStream.str());
    }
    this->centralizerRank -= centralizerTypeAlternative.getRootSystemSize();
    if (this->rootSystemCentralizerPrimalCoordinates.size > 0) {
      if (centralizerTypeAlternative != this->centralizerType) {
        global.fatal
        << "Two different methods "
        << "for computing the centralizer type yield different results: "
        << "by sub-diagram I computed the type as "
        << this->centralizerType.toString()
        << " but looking at subalgerba containing "
        << "the current one I got centralizer type "
        << centralizerTypeAlternative.toString()
        << global.fatal;
      }
    }
    if (this->centralizerRank > this->owner->owner->getRank()) {
      global.fatal
      << "Something is wrong: the rank of the centralizer "
      << "is larger than the rank of the owner."
      << this->toStringCentralizerDebugData()
      << global.fatal;
    }
  } else {
    if (
      this->centralizerDimension + this->getRank() >
      this->owner->owner->getRank()
    ) {
      // When this happens, one of two things holds.
      // 1. We don't have the max containing semisimple container computed,
      // say,
      // this is a partial run
      // where not all semisimple algebras were found.
      // 2. This is a semisimple subalgebra that is not contained in another
      // larger proper semisimple subalgebra.
      // In this case, the centralizer of the present subalgebra must
      // necessarily be
      // diagonal (inside the ambient Cartan). But as the rank computation
      // above
      // shows, that is not
      // the case. We must have made a serious programming mistake.
      // We assume the best - that 1) holds.
      this->computeCentralizerManually();
      if (!this->flagCentralizerIsWellChosen) {
        return;
      }
    }
  }
  this->flagCentralizerIsWellChosen = (
    this->centralizerRank == this->cartanOfCentralizer.size
  );
  if (this->flagCentralizerIsWellChosen) {
    this->flagCentralizerTypeIsComputed = true;
  }
  if (
    this->indexMaximalSemisimpleContainer != - 1 &&
    this->flagCentralizerIsWellChosen
  ) {
    for (int i = 0; i < this->owner->subalgebras.values.size; i ++) {
      if (
        centralizerTypeAlternative ==
        this->owner->subalgebras.values[i].weylNonEmbedded->dynkinType
      ) {
        if (
          this->owner->subalgebras.values[i].indicesDirectSummandSuperAlgebra.
          contains(this->indexMaximalSemisimpleContainer)
        ) {
          this->indexSemisimplePartCentralizer = i;
          break;
        }
      }
    }
  }
}

std::string CandidateSemisimpleSubalgebra::toStringSystem(
  FormatExpressions* format
) const {
  STACK_TRACE("CandidateSemisimpleSubalgebra::toStringSystem");
  std::stringstream out;
  if (this->flagSystemSolved) {
    out << "<br>Subalgebra realized. ";
    if (!this->flagCentralizerIsWellChosen) {
      out << "<b>However, the centralizer is not well chosen.</b>";
    }
  } else if (this->flagSystemProvedToHaveNoSolution) {
    out << "<br><b> Subalgebra candidate cannot be realized. </b> ";
  } else {
    out
    << "<br><b>Subalgebra candidate not realized, "
    << "and this may or may not be possible.</b>";
  }
  out
  << "<br>"
  << this->unknownNegativeGenerators.size
  << "*2 (unknown) gens:<br>(";
  for (int i = 0; i < this->unknownNegativeGenerators.size; i ++) {
    out
    << "<br>"
    << this->unknownNegativeGenerators[i].toString(format)
    << ", ";
    out << this->unknownPositiveGenerators[i].toString(format);
    if (i != this->unknownNegativeGenerators.size - 1) {
      out << ", ";
    }
  }
  out << ")<br>";
  if (this->unknownCartanCentralizerBasis.size > 0) {
    out << "<br>Unknown splitting cartan of centralizer.<br>\n";
    for (int i = 0; i < this->unknownCartanCentralizerBasis.size; i ++) {
      out << this->unknownCartanCentralizerBasis[i].toString();
      if (i != this->unknownCartanCentralizerBasis.size - 1) {
        out << ", ";
      }
    }
    out << "<br>";
  }
  for (int i = 0; i < this->cartanElementsSubalgebra.size; i ++) {
    out << "h: " << this->cartanElementsSubalgebra[i];
    if (i < this->involvedPositiveGenerators.size) {
      out
      << ", e = combination of "
      << this->involvedPositiveGenerators[i].toString();
    } else {
      out << ", involved positive generators unknown";
    }
    if (i < this->involvedNegativeGenerators.size) {
      out
      << ", f= combination of "
      << this->involvedNegativeGenerators[i].toString();
    } else {
      out << ", involved negative generators unknown.";
    }
  }
  out
  << "Positive weight subsystem: "
  << this->weylNonEmbedded->rootsOfBorel.toString();
  if (this->posistiveRootsPerpendicularToPrecedingWeights.size > 0) {
    out
    << " Positive roots that commute with the weight subsystem: "
    << this->posistiveRootsPerpendicularToPrecedingWeights.toString();
  }
  out
  << "<br>Symmetric Cartan default scale: "
  << this->subalgebraNonEmbeddedDefaultScale->weylGroup.cartanSymmetric.
  toString(format);
  out
  << "Character ambient Lie algebra: "
  << this->characterFundamentalCoordinatesRelativeToCartan.toString();
  out << "<br>A necessary system to realize the candidate subalgebra.  ";
  FormatExpressions currentFormat;
  for (int i = 0; i < this->systemToSolve.size; i ++) {
    out << "<br>" << this->systemToSolve[i].toString(&currentFormat) << "= 0";
  }
  out << "<br>The above system after transformation.  ";
  for (int i = 0; i < this->transformedSystem.size; i ++) {
    out
    << "<br>"
    << this->transformedSystem[i].toString(&currentFormat)
    << "= 0";
  }
  if (!this->flagSystemGroebnerBasisFound) {
    out
    << "<br><b>Failed to find Groebner basis "
    << "of the above system (the computation is too large).</b>";
  }
  out << this->toStringSystemPart2(format);
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::toStringSystemPart2(
  FormatExpressions* format
) const {
  (void) format;
  // avoid unused parameter warning in a portable way
  STACK_TRACE("CandidateSemisimpleSubalgebra::toStringSystemPart2");
  std::stringstream out;
  out
  << "<br><b>For the calculator:</b><br>\n"
  << this->toStringLoadUnknown()
  << ";"
  << "<br>FindOneSolutionSerreLikePolynomialSystem{}( ";
  for (int i = 0; i < this->systemToSolve.size; i ++) {
    out << this->systemToSolve[i].toString();
    if (i != this->systemToSolve.size - 1) {
      out << ", ";
    }
  }
  out << " )";
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::toStringLoadUnknown(
  FormatExpressions* format
) const {
  (void) format;
  STACK_TRACE("CandidateSemisimpleSubalgebra::toStringLoadUnknown");
  std::stringstream out;
  out << "(";
  out
  << "DynkinType ="
  << this->weylNonEmbedded->dynkinType.toString()
  << "; ElementsCartan =";
  out << "(";
  for (int i = 0; i < this->cartanElementsSubalgebra.size; i ++) {
    out << this->cartanElementsSubalgebra[i].toString();
    if (i != this->cartanElementsSubalgebra.size - 1) {
      out << ", ";
    }
  }
  out << ")";
  out << "; generators =(";
  for (int i = 0; i < this->unknownNegativeGenerators.size; i ++) {
    out
    << this->unknownNegativeGenerators[i].toString()
    << ", "
    << this->unknownPositiveGenerators[i].toString();
    if (i != this->unknownNegativeGenerators.size - 1) {
      out << ", ";
    }
  }
  out << ") )";
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::toStringType(
  FormatExpressions* format
) const {
  return this->weylNonEmbedded->dynkinType.toString(format);
}

std::string CandidateSemisimpleSubalgebra::toStringTypeAndHs(
  FormatExpressions* format
) const {
  std::stringstream out;
  out
  << this->weylNonEmbedded->dynkinType.toString(format)
  << ", Cartan: "
  << this->cartanElementsSubalgebra.toString(format);
  return out.str();
}

std::string CandidateSemisimpleSubalgebra::toStringGenerators(
  FormatExpressions* format
) const {
  STACK_TRACE("CandidateSemisimpleSubalgebra::toStringGenerators");
  if (this->basis.size == 0) {
    return "";
  }
  std::stringstream out;
  out
  << "<br>Dimension of subalgebra generated "
  << "by predefined or computed generators: "
  << this->basis.size
  << "."
  << "<br>Negative simple generators: ";
  for (int i = 0; i < this->negativeGenerators.size; i ++) {
    out
    << HtmlRoutines::getMathNoDisplay(
      this->negativeGenerators[i].toString(format)
    );
    if (i != this->negativeGenerators.size - 1) {
      out << ", ";
    }
  }
  out << "<br>Positive simple generators: ";
  for (int i = 0; i < this->positiveGenerators.size; i ++) {
    out
    << HtmlRoutines::getMathNoDisplay(
      this->positiveGenerators[i].toString(format)
    );
    if (i != this->positiveGenerators.size - 1) {
      out << ", ";
    }
  }
  return out.str();
}

bool CandidateSemisimpleSubalgebra::isRegularSubalgebra() const {
  for (int i = 0; i < this->negativeGenerators.size; i ++) {
    if (
      this->negativeGenerators[i].size() > 1 ||
      this->positiveGenerators[i].size() > 1
    ) {
      return false;
    }
  }
  return true;
}

std::string CandidateSemisimpleSubalgebra::toString(
  FormatExpressions* format, bool generateLinks
) const {
  STACK_TRACE("CandidateSemisimpleSubalgebra::toString");
  std::stringstream out;
  bool shortReportOnly = format ==
  nullptr ? true : format->flagCandidateSubalgebraShortReportOnly;
  out
  << "Subalgebra type: "
  << this->owner->toStringAlgebraLink(
    this->indexInOwner, format, generateLinks
  )
  << " (click on type for detailed printout).\n";
  out << this->comments;
  if (this->isRegularSubalgebra()) {
    out
    << "<br>The subalgebra is regular "
    << "(= the semisimple part of a root subalgebra). ";
  }
  if (this->indexInSlTwos != - 1) {
    out << "<br>" << this->wConjecture.toStringWConjectureHolds();
    out
    << "<hr style='max-width:300px;margin:0px;'>"
    << "This is an <b>sl(2) subalgebra</b>; "
    << "details follow [info from sl(2) tables].<br>";
    out << this->owner->slTwoSubalgebras[this->indexInSlTwos].toString();
    out << "<hr style='max-width:100px;margin:0px;'>";
  }
  if (this->indexInducedFrom != - 1) {
    out
    << "<br>Subalgebra is (parabolically) induced from "
    << this->owner->toStringAlgebraLink(
      this->indexInducedFrom, format, generateLinks
    )
    << ". ";
  }
  if (!this->flagCentralizerIsWellChosen) {
    out
    << "<br>The dimension of the centralizer is: "
    << this->centralizerDimension.toString();
  }
  out << this->toStringCentralizer(format, generateLinks);
  if (this->flagSystemProvedToHaveNoSolution) {
    out << " <b>Subalgebra candidate proved to be impossible!</b> ";
    return out.str();
  }
  bool weightsAreCoordinated = true;
  if (this->modules.size != this->weightsModulesNONprimal.size) {
    weightsAreCoordinated = false;
  } else {
    for (int i = 0; i < this->modules.size; i ++) {
      if (
        this->modules[i].size != this->weightsModulesNONprimal[i].size
      ) {
        weightsAreCoordinated = false;
        break;
      }
    }
  }
  if (this->indicesDirectSummandSuperAlgebra.size > 0) {
    out << "<br>Contained up to conjugation as a direct summand of: ";
    for (
      int i = 0; i < this->indicesDirectSummandSuperAlgebra.size; i ++
    ) {
      out
      << this->owner->toStringAlgebraLink(
        this->indicesDirectSummandSuperAlgebra[i], format, generateLinks
      );
      if (i != this->indicesDirectSummandSuperAlgebra.size - 1) {
        out << ", ";
      }
    }
    out << ". ";
  }
  out << "<br>" << this->toStringCartanSubalgebra(format);
  bool displayNilradSummary = (
    this->owner->flagComputeNilradicals &&
    this->flagCentralizerIsWellChosen &&
    this->flagSystemSolved
  );
  if (displayNilradSummary) {
    displayNilradSummary = !shortReportOnly || (
      this->numberOfBadParabolics > 0
    );
  }
  if (displayNilradSummary) {
    out << "<hr>" << this->toStringNilradicalsSummary(format);
    out
    << "<a href='"
    << this->owner->getDisplayFileNameFKFTNilradicals(
      this->indexInOwner, format
    )
    << "'>Detailed information on isotypical nilradicals. </a><hr>";
  }
  out << this->toStringGenerators(format);
  FormatExpressions currentFormat;
  currentFormat.flagUseLatex = true;
  currentFormat.flagUseFrac = true;
  currentFormat.flagUseHTML = false;
  out << "<br>Cartan symmetric matrix: ";
  out
  << HtmlRoutines::getMathNoDisplay(
    this->weylNonEmbedded->cartanSymmetric.toString(&currentFormat)
  );
  out
  << "<br>Scalar products of elements of "
  << "Cartan subalgebra scaled to act by 2 "
  << "(co-symmetric Cartan matrix): ";
  out
  << HtmlRoutines::getMathNoDisplay(
    this->weylNonEmbedded->coCartanSymmetric.toString(&currentFormat)
  );
  if (this->owner->flagRealForms) {
    out << "<br>" << this->wConjecture.toString(*this);
  }
  FormatExpressions characterFormatNonConstant;
  if (!this->characterFormat.isZeroPointer()) {
    characterFormatNonConstant = this->characterFormat.getElementConst();
  }
  out << "<br>Decomposition of ambient Lie algebra: ";
  out
  << HtmlRoutines::getMathNoDisplay(
    this->characterNonPrimalFundamentalCoordinates.toString(
      &characterFormatNonConstant
    )
  );
  if (this->cartanOfCentralizer.size > 0) {
    out
    << "<br>Primal decomposition of the ambient Lie algebra. "
    << "This refines the above decomposition "
    << "(please note the order is not the same as above). ";
    out
    << HtmlRoutines::getMathNoDisplay(
      this->primalCharacter.toString(&characterFormatNonConstant)
    );
  }
  if (this->flagCentralizerIsWellChosen && weightsAreCoordinated) {
    int numberOfZeroWeights = 0;
    out
    << "<br>The number of zero weights w.r.t. the "
    << "Cartan subalgebra minus the dimension "
    << "of the centralizer of the subalgebra equals ";
    for (int i = 0; i < this->modules.size; i ++) {
      for (int j = 0; j < this->weightsModulesNONprimal[i].size; j ++) {
        if (this->weightsModulesNONprimal[i][j].isEqualToZero()) {
          numberOfZeroWeights += this->modules[i].size;
        }
      }
    }
    out
    << numberOfZeroWeights
    << " - "
    << this->centralizerRank
    << "="
    << ((this->centralizerRank - numberOfZeroWeights) *(- 1)).toString()
    << ".";
  }
  if (this->flagSystemSolved && !shortReportOnly) {
    out
    << "<br>In the table below we indicate the highest weight "
    << "vectors of the decomposition of "
    << "the ambient Lie algebra as a module over the semisimple part. "
    << "The second row indicates "
    << "weights of the highest weight vectors relative "
    << "to the Cartan of the semisimple subalgebra. ";
    if (
      this->flagCentralizerIsWellChosen && this->cartanOfCentralizer.size > 0
    ) {
      out
      << "As the centralizer is well-chosen and "
      << "the centralizer of our subalgebra is "
      << "non-trivial, we may in addition split highest weight vectors "
      << "with the same weight over the semisimple part over the centralizer "
      << "(recall that the centralizer preserves "
      << "the weights over the subalgebra "
      << "and in particular acts on the highest weight vectors). "
      << "Therefore we have chosen our highest "
      << "weight vectors to be, in addition, weight vectors "
      << "over the Cartan of the centralizer of the starting subalgebra. "
      << "Their weight over the sum of the "
      << "Cartans of the semisimple subalgebra and "
      << "its centralizer is indicated in the third row. "
      << "The weights corresponding to the Cartan of the "
      << "centralizer are again indicated with the letter "
      << "\\omega. As there is no preferred way of chosing a basis of "
      << "the Cartan of the centralizer "
      << "(unlike the starting semisimple Lie algebra: "
      << "there we have a preferred basis "
      << "induced by the fundamental weights), "
      << "our centralizer weights are simply "
      << "given by the constant by which the "
      << "k^th basis element of the Cartan of the centralizer acts on the "
      << "highest weight vector. Here, we use "
      << "the choice for basis of the Cartan "
      << "of the centralizer given at the start of the page.<br><br>";
    }
    out
    << "<table class='tableStandard'><tr>"
    << "<td>Highest vectors of representations (total "
    << this->highestVectorsNonSorted.size
    << ")";
    if (this->flagCentralizerIsWellChosen) {
      out << "; the vectors are over the primal subalgebra.";
    }
    out << "</td>";
    for (int i = 0; i < this->highestVectorsNonSorted.size; i ++) {
      out
      << "<td>\\("
      << this->highestVectorsNonSorted[i].toString()
      << "\\)</td>";
    }
    out << "</tr><tr><td>weight</td>";
    for (
      int i = 0; i < this->highestWeightsNonPrimalNonSorted.size; i ++
    ) {
      out
      << "<td>\\("
      << this->highestWeightsNonPrimalNonSorted[i].toStringLetterFormat(
        "\\omega", &characterFormatNonConstant
      )
      << "\\)</td>";
    }
    out << "</tr>";
    if (
      this->flagCentralizerIsWellChosen && this->cartanOfCentralizer.size > 0
    ) {
      out
      << "<tr><td>weights rel. to Cartan of "
      << "(centralizer+semisimple s.a.). </td>";
      for (int i = 0; i < this->highestWeightsPrimalNonSorted.size; i ++) {
        out
        << "<td>\\("
        << this->highestWeightsPrimalNonSorted[i].toStringLetterFormat(
          "\\omega", &characterFormatNonConstant
        )
        << "\\)</td>";
      }
      out << "</tr>";
    }
    out << "</table>";
  }
  if (!shortReportOnly) {
    out << this->toStringModuleDecomposition(format);
    if (this->owner->flagProduceLaTeXTables) {
      out
      << "LaTeX version of module decomposition: <br><br>"
      << this->toStringModuleDecompositionLaTeX(format)
      << "<br><br>";
    }
    out << this->toStringPairingTable(format);
    if (this->owner->flagProduceLaTeXTables) {
      out
      << "LaTeX version of pairing table: <br><br>"
      << this->toStringPairingTableLaTeX(format)
      << "<br><br>";
    }
    out << "<br>" << this->toStringDrawWeights(format) << "<br>";
  }
  bool shouldDisplaySystem = false;
  if (this->totalArithmeticOpsToSolveSystem != 0) {
    out
    << "<br>Made total "
    << totalArithmeticOpsToSolveSystem
    << " arithmetic operations while solving "
    << "the Serre relations polynomial system. ";
    if (this->totalArithmeticOpsToSolveSystem > 1000000 && !shortReportOnly) {
      shouldDisplaySystem = true;
      out
      << "<br>The total number of arithmetic operations "
      << "I needed to solve the Serre relations "
      << "polynomial system was larger than 1 000 000. I am printing out "
      << "the Serre relations system for you: maybe "
      << "that can help improve the polynomial system algorithms. ";
    }
  }
  shouldDisplaySystem = shouldDisplaySystem ||
  !this->flagSystemSolved ||
  !this->flagCentralizerIsWellChosen;
  if (generateLinks) {
    shouldDisplaySystem = shouldDisplaySystem && !shortReportOnly;
  }
  if (shouldDisplaySystem) {
    out << this->toStringSystem(format);
  }
  return out.str();
}

void CandidateSemisimpleSubalgebra::getHsScaledToActByTwoByType(
  List<List<Vectors<Rational> > >& outputHsByType,
  List<DynkinSimpleType>& outputTypeList
) const {
  STACK_TRACE("CandidateSemisimpleSubalgebra::getHsScaledToActByTwoByType");
  List<DynkinSimpleType> allTypes;
  this->weylNonEmbedded->dynkinType.getTypesWithMults(allTypes);
  outputHsByType.setSize(0);
  outputTypeList.setSize(0);
  if (
    allTypes.size != this->cartanSubalgebrasByComponentScaledToActByTwo.size
  ) {
    global.fatal
    << "AllTypes.size must equal "
    << "this->cartanSubalgebrasByComponentScaledToActByTwo.size. "
    << global.fatal;
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
    outputHsByType.lastObject()->addOnTop(
      this->cartanSubalgebrasByComponentScaledToActByTwo[i]
    );
  }
}

bool CandidateSemisimpleSubalgebra::hasHsScaledByTwoConjugateTo(
  List<Vector<Rational> >& input
) const {
  STACK_TRACE("CandidateSemisimpleSubalgebra::hasHsScaledByTwoConjugateTo");
  if (input.size != this->cartanElementsScaledToActByTwo.size) {
    return false;
  }
  List<Vector<Rational> > raisedInput = input;
  List<Vector<Rational> > myVectors = this->cartanElementsScaledToActByTwo;
  WeylGroupAutomorphisms& ambientWeylAutomorphisms =
  this->getAmbientWeylAutomorphisms();
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

bool CandidateSemisimpleSubalgebra::isDirectSummandOf(
  const CandidateSemisimpleSubalgebra& other
) {
  STACK_TRACE("CandidateSemisimpleSubalgebra::isDirectSummandOf");
  if (other.flagSystemProvedToHaveNoSolution) {
    return false;
  }
  DynkinType difference;
  difference = other.weylNonEmbedded->dynkinType;
  difference -= this->weylNonEmbedded->dynkinType;
  for (int i = 0; i < difference.size(); i ++) {
    if (difference.coefficients[i] < 0) {
      return false;
    }
  }
  Incrementable<SelectionFixedRank> selectedTypes;
  List<DynkinSimpleType> isoTypes;
  SelectionFixedRank currentTypeSelection;
  List<List<Vectors<Rational> > > hElementsScaledToActByTwoByType;
  other.getHsScaledToActByTwoByType(hElementsScaledToActByTwoByType, isoTypes);
  for (int i = 0; i < isoTypes.size; i ++) {
    Rational ratMult =
    this->weylNonEmbedded->dynkinType.getCoefficientOf(isoTypes[i]);
    int intMult = 0;
    if (!ratMult.isSmallInteger(&intMult)) {
      return false;
    }
    currentTypeSelection.setNumberOfItemsAndDesiredSubsetSize(
      intMult, hElementsScaledToActByTwoByType[i].size
    );
    selectedTypes.elements.addOnTop(currentTypeSelection);
  }
  FinitelyGeneratedMatrixMonoid<Rational> outerAutomorphisms;
  this->weylNonEmbedded->dynkinType.getOuterAutosGeneratorsActOnVectorColumn(
    outerAutomorphisms.generators
  );
  for (int i = 0; i < outerAutomorphisms.generators.size; i ++) {
    outerAutomorphisms.generators[i].transpose();
  }
  bool mustBeTrue = outerAutomorphisms.generateElements(100000);
  if (!mustBeTrue) {
    global.fatal
    << "Failed to generate outer automorphisms of Dynkin simple type. "
    << "The upper limit for such automorphism group size is 100000. "
    << global.fatal;
  }
  Rational numberOfCyclesFromTypes = selectedTypes.totalCombinations();
  if (!numberOfCyclesFromTypes.isSmallInteger()) {
    global.fatal
    << "Computation is too large: "
    << "I am crashing to let you know that the program cannot "
    << "handle such a large number of outer automorphisms. "
    << global.fatal;
  }
  List<Vector<Rational> > conjugationCandidates;
  Vectors<Rational> currentComponent;
  ProgressReport report;
  if (report.tickAndWantReport()) {
    std::stringstream reportStream;
    reportStream
    << "Computing whether "
    << this->weylNonEmbedded->dynkinType
    << " is direct summand of "
    << other.weylNonEmbedded->dynkinType.toString()
    << ". The outer automorphisms of the smaller type are: ";
    FormatExpressions format;
    format.flagUseHTML = false;
    format.flagUseLatex = true;
    for (int i = 0; i < outerAutomorphisms.elements.size; i ++) {
      if (i >= 100) {
        reportStream
        << "... and so on, only the first 100 elements printed out of total "
        << outerAutomorphisms.elements.size
        << ". ";
      } else {
        reportStream
        << "<br>"
        << HtmlRoutines::getMathNoDisplay(
          outerAutomorphisms.elements[i].toStringMatrixForm(&format)
        );
      }
    }
    report.report(reportStream.str());
  }
  do {
    for (int k = 0; k < outerAutomorphisms.elements.size; k ++) {
      conjugationCandidates.setSize(0);
      for (int i = 0; i < selectedTypes.elements.size; i ++) {
        Selection& currentSel = selectedTypes.elements[i].selection;
        for (int j = 0; j < currentSel.cardinalitySelection; j ++) {
          currentComponent =
          hElementsScaledToActByTwoByType[i][currentSel.elements[j]];
          conjugationCandidates.addListOnTop(currentComponent);
        }
      }
      outerAutomorphisms.elements[k].actOnVectorROWSOnTheLeft(
        conjugationCandidates, conjugationCandidates
      );
      if (this->hasHsScaledByTwoConjugateTo(conjugationCandidates)) {
        return true;
      }
    }
  } while (selectedTypes.incrementReturnFalseIfPastLast());
  return false;
}

void CandidateSemisimpleSubalgebra::adjustCentralizerAndRecompute(
  bool allowNonPolynomialSystemFailure
) {
  STACK_TRACE("CandidateSemisimpleSubalgebra::adjustCentralizerAndRecompute");
  if (this->flagSystemProvedToHaveNoSolution) {
    return;
  }
  this->computeCentralizerIsWellChosen();
  if (!this->owner->flagAttemptToAdjustCentralizers) {
    return;
  }
  if (this->flagCentralizerIsWellChosen) {
    return;
  }
  if (!this->flagCentralizerTypeIsComputed) {
    return;
  }
  this->computeSystem(true, allowNonPolynomialSystemFailure);
  this->computeCentralizerIsWellChosen();
}

int CandidateSemisimpleSubalgebra::getNumberOfModules() const {
  int result = 0;
  for (int i = 0; i < this->modules.size; i ++) {
    result += this->modules[i].size;
  }
  return result;
}

void SemisimpleSubalgebras::computePairingTablesAndFKFTtypes() {
  STACK_TRACE("SemisimpleSubalgebras::computePairingTablesAndFKFTtypes");
  ProgressReport report;
  for (int i = 0; i < this->subalgebras.values.size; i ++) {
    CandidateSemisimpleSubalgebra& currentSubalgebra =
    this->subalgebras.values[i];
    if (
      !currentSubalgebra.flagCentralizerIsWellChosen ||
      !currentSubalgebra.flagSystemSolved
    ) {
      continue;
    }
    if (!this->flagComputePairingTable) {
      continue;
    }
    if (report.tickAndWantReport()) {
      std::stringstream reportStream2;
      reportStream2
      << "Computing pairing table of subalgebra number "
      << i + 1
      << " out of "
      << this->subalgebras.values.size
      << ". The subalgebra is of type "
      << this->subalgebras.values[i].toStringTypeAndHs()
      << "... ";
      report.report(reportStream2.str());
    }
    if (currentSubalgebra.isRegularSubalgebra()) {
      continue;
    }
    currentSubalgebra.computePairingTable();
    if (report.tickAndWantReport()) {
      std::stringstream reportStream2;
      reportStream2
      << "Computing pairing table of subalgebra number "
      << i + 1
      << " out of "
      << this->subalgebras.values.size
      << ". The subalgebra is of type "
      << this->subalgebras.values[i].toStringTypeAndHs()
      << "... DONE. Computing Fernando-Kac subalgebra candidates.";
      report.report(reportStream2.str());
    }
    if (
      this->flagComputeNilradicals &&
      !this->subalgebras.values[i].isRegularSubalgebra()
    ) {
      currentSubalgebra.enumerateAllNilradicals();
    }
  }
}

void SemisimpleSubalgebras::hookUpCentralizers(
  bool allowNonPolynomialSystemFailure
) {
  STACK_TRACE("SemisimpleSubalgebras::hookUpCentralizers");
  this->checkAll();
  List<int> candidatePermutation;
  candidatePermutation.setSize(this->subalgebras.values.size);
  for (int i = 0; i < candidatePermutation.size; i ++) {
    candidatePermutation[i] = i;
  }
  this->subalgebras.values.quickSortAscending(nullptr, &candidatePermutation);
  List<Vectors<Rational> > hsCopy;
  hsCopy.setSize(candidatePermutation.size);
  for (int i = 0; i < candidatePermutation.size; i ++) {
    hsCopy[i] = this->subalgebras.keys[candidatePermutation[i]];
  }
  this->subalgebras.keys = hsCopy;
  HashedList<int, HashFunctions::hashFunction> candidatePermutationHashed;
  candidatePermutationHashed = candidatePermutation;
  ProgressReport report1, report2;
  report1.report("<hr>\nHooking up centralizers ");
  for (int i = 0; i < this->subalgebras.values.size; i ++) {
    if (!this->subalgebras.values[i].flagSystemSolved) {
      continue;
    }
    if (
      !this->subalgebras.values[i].computeAndVerifyFromGeneratorsAndHs()
    ) {
      continue;
    }
    CandidateSemisimpleSubalgebra& currentSubalgebra =
    this->subalgebras.values[i];
    std::stringstream reportStream2;
    reportStream2
    << "Computing centralizer of subalgebra number "
    << i + 1
    << " out of "
    << this->subalgebras.values.size
    << ". The subalgebra is of type "
    << currentSubalgebra.toStringTypeAndHs()
    << ". ";
    report2.report(reportStream2.str());
    currentSubalgebra.indexInOwner = i;
    currentSubalgebra.indexInducedFrom =
    candidatePermutationHashed.getIndex(currentSubalgebra.indexInducedFrom);
    currentSubalgebra.indicesDirectSummandSuperAlgebra.setSize(0);
    currentSubalgebra.indexMaximalSemisimpleContainer = - 1;
    for (int j = 0; j < this->subalgebras.values.size; j ++) {
      if (i == j) {
        continue;
      }
      CandidateSemisimpleSubalgebra & otherSubalgebra =
      this->subalgebras.values[j];
      if (currentSubalgebra.isDirectSummandOf(otherSubalgebra)) {
        currentSubalgebra.indicesDirectSummandSuperAlgebra.addOnTop(j);
        if (currentSubalgebra.indexMaximalSemisimpleContainer == - 1) {
          currentSubalgebra.indexMaximalSemisimpleContainer = j;
        }
        if (
          this->subalgebras.values[
            currentSubalgebra.indexMaximalSemisimpleContainer
          ].weylNonEmbedded->dynkinType.getLieAlgebraDimension() <
          otherSubalgebra.weylNonEmbedded->dynkinType.getLieAlgebraDimension()
        ) {
          currentSubalgebra.indexMaximalSemisimpleContainer = j;
        }
      }
    }
    currentSubalgebra.computeCentralizerIsWellChosen();
    if (currentSubalgebra.centralizerRank > this->owner->getRank()) {
      global.fatal
      << "Subalgebra "
      << currentSubalgebra.toStringType()
      << " has rank "
      << currentSubalgebra.centralizerRank.toString()
      << " but the ambient Lie algebra is only of rank "
      << this->owner->getRank()
      << ". "
      << global.fatal;
    }
  }
  report1.report(
    "<hr>\nCentralizers computed, adjusting "
    "centralizers with respect to the Cartan subalgebra."
  );
  for (int i = 0; i < this->subalgebras.values.size; i ++) {
    CandidateSemisimpleSubalgebra& currentSubalgebra =
    this->subalgebras.values[i];
    if (!currentSubalgebra.flagSystemSolved) {
      continue;
    }
    if (!currentSubalgebra.flagCentralizerTypeIsComputed) {
      continue;
    }
    std::stringstream reportStream2;
    reportStream2
    << "Adjusting the centralizer of subalgebra number "
    << i + 1
    << " out of "
    << this->subalgebras.values.size
    << ". The subalgebra is of type "
    << this->subalgebras.values[i].toStringTypeAndHs()
    << ". ";
    report2.report(reportStream2.str());
    this->subalgebras.values[i].adjustCentralizerAndRecompute(
      allowNonPolynomialSystemFailure
    );
  }
  report1.report("<hr>\nComputing pairing tables.");
  if (this->flagComputeModuleDecomposition) {
    for (int i = 0; i < this->subalgebras.values.size; i ++) {
      if (
        this->subalgebras.values[i].flagCentralizerIsWellChosen &&
        this->subalgebras.values[i].flagSystemSolved
      ) {
        std::stringstream reportStream2;
        reportStream2
        << "Computing primal module decomposition of subalgebra number "
        << i + 1
        << " out of "
        << this->subalgebras.values.size
        << ". The subalgebra is of type "
        << this->subalgebras.values[i].toStringTypeAndHs()
        << ". ";
        report2.report(reportStream2.str());
        this->subalgebras.values[i].computePrimalModuleDecomposition();
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

bool CandidateSemisimpleSubalgebra::operator>(
  const CandidateSemisimpleSubalgebra& other
) const {
  return this->weylNonEmbedded->dynkinType > other.weylNonEmbedded->dynkinType;
}

void CandidateSemisimpleSubalgebra::computeCartanOfCentralizer() {
  STACK_TRACE("CandidateSemisimpleSubalgebra::computeCartanOfCentralizer");
  Vectors<AlgebraicNumber> highestWeightsNonSorted;
  Vectors<AlgebraicNumber> cartanGenerators;
  highestWeightsNonSorted.setSize(this->highestVectorsNonSorted.size);
  for (int i = 0; i < this->highestVectorsNonSorted.size; i ++) {
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& currentElement =
    this->highestVectorsNonSorted[i];
    currentElement.toVectorNegativeRootSpacesFirst(
      highestWeightsNonSorted[i]
    );
  }
  ElementSemisimpleLieAlgebra<AlgebraicNumber> element;
  Vector<Rational> tempV;
  cartanGenerators.setSize(
    this->getAmbientSemisimpleLieAlgebra().getRank()
  );
  for (
    int i = 0; i < this->getAmbientSemisimpleLieAlgebra().getRank(); i ++
  ) {
    tempV.makeEi(this->getAmbientSemisimpleLieAlgebra().getRank(), i);
    element.makeCartanGenerator(
      tempV, this->getAmbientSemisimpleLieAlgebra()
    );
    element.toVectorNegativeRootSpacesFirst(cartanGenerators[i]);
  }
  Vectors<AlgebraicNumber> outputCartanCentralizer;
  Vector<AlgebraicNumber> centralizerH;
  highestWeightsNonSorted.intersectTwoLinearSpaces(
    highestWeightsNonSorted, cartanGenerators, outputCartanCentralizer
  );
  this->cartanOfCentralizer.setSize(outputCartanCentralizer.size);
  AlgebraicNumber firstNonZeroCoefficient;
  for (int i = 0; i < this->cartanOfCentralizer.size; i ++) {
    element.assignVectorNegativeRootSpacesCartanPosistiveRootSpaces(
      outputCartanCentralizer[i], *this->owner->owner
    );
    centralizerH = element.getCartanPart();
    firstNonZeroCoefficient =
    centralizerH[centralizerH.getIndexFirstNonZeroCoordinate()];
    if (!firstNonZeroCoefficient.isRational()) {
      centralizerH /= firstNonZeroCoefficient;
    }
    this->cartanOfCentralizer[i] = centralizerH;
    this->cartanOfCentralizer[i].scaleNormalizeFirstNonZero();
  }
  // //////////////
  this->bilinearFormSimplePrimal = this->weylNonEmbedded->cartanSymmetric;
  Matrix<Rational> centralizerPart;
  Matrix<Rational> matrixFundamentalCoordinatesSimple;
  Matrix<Rational> diagonalMatrix;
  Matrix<Rational> diagMatrix2;
  Matrix<Rational> bilinearFormInverted;
  this->cartanOfCentralizer.getGramMatrix(
    centralizerPart, &this->owner->owner->weylGroup.cartanSymmetric
  );
  this->bilinearFormSimplePrimal.directSumWith(centralizerPart);
  bilinearFormInverted = this->bilinearFormSimplePrimal;
  bilinearFormInverted.invert();
  diagonalMatrix.initialize(
    this->bilinearFormSimplePrimal.numberOfRows,
    this->bilinearFormSimplePrimal.numberOfColumns
  );
  diagMatrix2.initialize(
    this->bilinearFormSimplePrimal.numberOfRows,
    this->bilinearFormSimplePrimal.numberOfColumns
  );
  diagonalMatrix.makeZero();
  diagMatrix2.makeZero();
  for (int i = 0; i < this->bilinearFormSimplePrimal.numberOfRows; i ++) {
    if (i < this->cartanElementsSubalgebra.size) {
      diagonalMatrix(i, i) =
      this->subalgebraNonEmbeddedDefaultScale->weylGroup.cartanSymmetric(i, i)
      /
      2;
      diagMatrix2(i, i) = this->weylNonEmbedded->cartanSymmetric(i, i) / 2;
    } else {
      diagonalMatrix(i, i).assignNumeratorAndDenominator(1, 2);
      diagMatrix2(i, i) = 1;
    }
  }
  matrixFundamentalCoordinatesSimple = bilinearFormInverted;
  matrixFundamentalCoordinatesSimple *= diagonalMatrix;
  this->matrixMultiplyFundamentalCoordinatesToGetSimple = bilinearFormInverted;
  this->matrixMultiplyFundamentalCoordinatesToGetSimple *= diagMatrix2;
  this->bilinearFormFundamentalPrimal = matrixFundamentalCoordinatesSimple;
  this->bilinearFormFundamentalPrimal.transpose();
  this->bilinearFormFundamentalPrimal *= this->bilinearFormSimplePrimal;
  this->bilinearFormFundamentalPrimal *= matrixFundamentalCoordinatesSimple;
}
