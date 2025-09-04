#include "general_file_operations_encodings.h"
#include "math_extra_finite_groups_implementation.h"
#include "math_extra_semisimple_lie_algebras.h"
#include "math_extra_semisimple_lie_subalgebras_sltwos.h"
#include "math_general_polynomial_computations_advanced_implementation.h" // IWYU pragma: keep: breaks g++ -02 optimization build.
#include "progress_report.h"

bool SlTwoSubalgebra::operator>(const SlTwoSubalgebra& right) const {
  STACK_TRACE("SlTwoSubalgebra::operatorGreaterThan");
  if (this->owner != right.owner) {
    global.fatal
    << "Error: comparing sl(2) subalgebras with different owners."
    << global.fatal;
  }
  if (this->lengthHSquared > right.lengthHSquared) {
    return true;
  }
  if (right.lengthHSquared > this->lengthHSquared) {
    return false;
  }
  return this->hCharacteristic > right.hCharacteristic;
}

bool SlTwoSubalgebra::operator==(const SlTwoSubalgebra& right) const {
  // See Dynkin, Semisimple Lie subalgebras of semisimple Lie algebras,
  // chapter 7-10.
  if (this->owner != right.owner) {
    global.fatal
    << "Comparing sl(2) "
    << "subalgebras that have different ambient Lie algebras. "
    << global.fatal;
  }
  return this->hCharacteristic == right.hCharacteristic;
}

std::string SlTwoSubalgebra::toStringCentralizer() const {
  return this->centralizerComputer.toString();
}

std::string SlTwoSubalgebra::toStringTripleVerification(
  FormatExpressions* format
) const {
  std::stringstream out;
  bool useHtml = format == nullptr ? true : format->flagUseHTML;
  out << "<br>Lie brackets of the above elements.<br>";
  if (useHtml) {
    out << "\n<br>\n";
  }
  out
  << "\\(\\begin{array}{rcl}"
  << "[e, f]&=&"
  << this->eBracketF.toString(format)
  << "\\\\\n"
  << "[h, e]&=&"
  << this->hBracketE.toString(format)
  << "\\\\\n"
  << "[h, f]&=&"
  << this->hBracketF.toString(format)
  << "\\end{array}\\)";
  return out.str();
}

std::string SlTwoSubalgebra::toStringTripleArbitrary(
  FormatExpressions* format
) const {
  std::stringstream out;
  out
  << "Starting h, e, f triple. H is computed according to Dynkin, "
  << "and the coefficients of f are arbitrarily chosen. "
  << "<br>More precisely, the chevalley generators "
  << "participating in f are ordered in the order "
  << "in which their roots appear, and the "
  << "coefficients are chosen "
  << "with an arbitrary formula. "
  << "More precisely, the n^th coefficient equals (n-1)^2+1.<br>"
  << "This arbitrary (but well-defined) choice of "
  << "f guarantees that the computation is linear and fast. <br>\n";
  out
  << "\\(\\begin{array}{rcl}"
  << "h&=&"
  << this->hPolynomialAlgebraic.toString(format)
  << "\\\\"
  << "e&=&"
  << this->eArbitraryUnknown.toString(format)
  << "\\\\"
  << "f&=&"
  << this->fArbitrary.toString(format)
  << "\\end{array}\\)";
  return out.str();
}

std::string SlTwoSubalgebra::toStringContainingRootSubalgebras(
  const std::string& displayPathAlgebra
) const {
  STACK_TRACE("SlTwoSubalgebra::toStringContainingRootSubalgebras");
  std::stringstream out;
  RootSubalgebras& rootSubalgebras = this->container->rootSubalgebras;
  for (int j = 0; j < this->indicesContainingRootSubalgebras.size; j ++) {
    int rootSubalgebraIndex = this->indicesContainingRootSubalgebras[j];
    RootSubalgebra& currentRootSubalgebra =
    rootSubalgebras.subalgebras[rootSubalgebraIndex];
    out
    << "<a href='"
    << displayPathAlgebra
    << "rootSubalgebra_"
    << rootSubalgebraIndex + 1
    << ".html'>"
    << "\\("
    << currentRootSubalgebra.dynkinDiagram.toString()
    << "\\)"
    << "</a>";
    if (j != this->indicesContainingRootSubalgebras.size - 1) {
      out << ", ";
    }
  }
  return out.str();
}

std::string SlTwoSubalgebra::toStringTripleArbitraryMatrix() const {
  std::stringstream out;
  out << "<br>Matrix form of the system we are trying to solve:\n";
  FormatExpressions formatLatex;
  formatLatex.flagUseLatex = true;
  formatLatex.flagUseHTML = false;
  out
  << "\\("
  << this->systemArbitraryMatrix.toString(&formatLatex)
  << "[col. vect.]"
  << "="
  << this->systemArbitraryColumnVector.toString(&formatLatex)
  << "\\)";
  return out.str();
}

std::string SlTwoSubalgebra::toStringTripleUnknowns(FormatExpressions* format)
const {
  std::stringstream out;
  out << "<hr>Unknown elements.<br>";
  out
  << "\\(\\begin{array}{rcl}"
  << "h&=&"
  << this->hPolynomialAlgebraic.toString(format)
  << "\\\\\n"
  << "e&=&"
  << this->eUnknown.toString(format)
  << "\\\\\n"
  << "f&=&"
  << this->fUnknown.toString(format)
  << "\\end{array}\\)";
  out
  << "<br>Participating positive roots: "
  << this->participatingPositiveRoots.toString()
  << ".";
  out << "<br>Lie brackets of the unknowns.<br>";
  out
  << "\\([e,f] - h = "
  << this->eBracketFMinusHUnknown.toString(format)
  << "\\)";
  return out.str();
}

std::string SlTwoSubalgebra::toStringKostantSekiguchiTriple(
  FormatExpressions* format
) const {
  if (this->eKostantSekiguchi.isEqualToZero()) {
    return "";
  }
  std::stringstream out;
  out << "<br>Kostant-Sekiguchi elements.<br>";
  out << "\\(";
  out << "\\begin{array}{rcl}";
  out << "h&=&" << this->hPolynomialAlgebraic.toString(format) << "\\\\\n";
  out << "e&=&" << this->eKostantSekiguchi.toString(format) << "\\\\\n";
  out << "f&=&" << this->fKostantSekiguchi.toString(format);
  out << "\\end{array}\\)";
  return out.str();
}

std::string SlTwoSubalgebra::toStringDynkinType() const {
  DynkinType dynkinType;
  this->computeDynkinTypeEmbedded(dynkinType);
  return dynkinType.toStringPretty();
}

std::string SlTwoSubalgebra::toString(FormatExpressions* format) const {
  STACK_TRACE("SlTwoSubalgebra::toString");
  if (this->container == nullptr) {
    return "sl(2) subalgebra not initialized.";
  }
  std::stringstream out;
  out << "\\(" << this->toStringDynkinType() << "\\)\n<br>\n";
  out
  << "<a name='sl2index"
  << this->indexInContainer
  << "'>h-characteristic: "
  << this->hCharacteristic.toString()
  << "</a>";
  out << "<br>Length of the weight dual to h: " << this->lengthHSquared;
  std::string currentString = this->preferredAmbientSimpleBasis.toString();
  std::string virtualPath = "";
  std::string htmlPathServer = "";
  bool useHtml = true;
  bool useLatex = false;
  virtualPath = this->owner->fileNames.virtualFolderName() + "sl2s/";
  htmlPathServer =
  this->owner->fileNames.displayFolderName("../../") + "sl2s/";
  if (virtualPath == "" || htmlPathServer == "") {
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
  // Don't break lines.
  latexFormat.maximumLineLength = - 1;
  for (int i = 0; i < this->indicesContainingRootSubalgebras.size; i ++) {
    out
    << "\nContaining regular semisimple subalgebra number "
    << i + 1
    << ": ";
    int rootSubalgebraIndex = this->indicesContainingRootSubalgebras[i];
    RootSubalgebra& currentSubalgebra =
    this->container->rootSubalgebras.subalgebras[rootSubalgebraIndex];
    std::string dynkinType = currentSubalgebra.dynkinDiagram.toString();
    if (useHtml) {
      out
      << "<a href='../rootSubalgebra_"
      << rootSubalgebraIndex + 1
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
  out << this->toStringTriple(&latexFormat);
  out << this->toStringTripleStandardRealization();
  out << this->toStringKostantSekiguchiTriple(&latexFormat);
  out << this->toStringKostantSekiguchiTripleStandardRealization();
  out << this->toStringTripleVerification(&latexFormat);
  out << "\n<br>" << this->toStringCentralizer();
  out << "\n<br>Unfold the hidden panel for more information.<br>\n";
  out << "\n<div class='lieAlgebraPanel'><div>\n";
  out << this->toStringTripleUnknowns(&latexFormat);
  out << this->toStringTripleUnknownsPolynomialSystem(&latexFormat);
  out << this->toStringTripleArbitrary(&latexFormat);
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
  this->flagCentralizerIsRegular = false;
  this->dimensionCentralizer = - 1;
  this->flagTryToComputeCentralizerFully = true;
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

const WeylGroupData& SlTwoSubalgebra::getOwnerWeyl() const {
  if (this->owner == nullptr) {
    global.fatal << "Weyl group with non-initialized owner. " << global.fatal;
  }
  return this->owner->weylGroup;
}

std::string SlTwoSubalgebra::toStringTripleStandardRealization() const {
  Matrix<Rational> matrixH;
  Matrix<Rational> matrixE;
  Matrix<Rational> matrixF;
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
    !this->owner->getElementStandardRepresentation(this->hAlgebraic, matrixH)
    ||
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

bool SlTwoSubalgebra::moduleDecompositionFitsInto(const SlTwoSubalgebra& other)
const {
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

std::string SlTwoSubalgebra::toStringMinimalContainingRootSubalgebras(
  const std::string& displayPathAlgebra
) const {
  STACK_TRACE("SlTwoSubalgebra::toStringMinimalContainingRootSubalgebras");
  std::stringstream out;
  RootSubalgebras& rootSubalgebras = this->container->rootSubalgebras;
  for (
    int j = 0; j < this->indicesMinimalContainingRootSubalgebras.size; j ++
  ) {
    int rootSubalgebraIndex =
    this->indicesMinimalContainingRootSubalgebras[j];
    RootSubalgebra& currentRootSubalgebra =
    rootSubalgebras.subalgebras[rootSubalgebraIndex];
    out
    << "<a href='"
    << displayPathAlgebra
    << "rootSubalgebra_"
    << rootSubalgebraIndex + 1
    << ".html'>"
    << "\\("
    << currentRootSubalgebra.dynkinDiagram.toString()
    << "\\)"
    << "</a>";
    if (j != this->indicesMinimalContainingRootSubalgebras.size - 1) {
      out << ", ";
    }
  }
  return out.str();
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
  this->unsuitableHs.setSize(0);
  this->indexZeroWeight = - 1;
  this->owner = &inputOwner;
  this->rootSubalgebras.owner = &inputOwner;
  this->allSubalgebras.clear();
}

std::string SlTwoSubalgebra::toStringKostantSekiguchiTripleInternals(
  FormatExpressions* format
) const {
  STACK_TRACE("SlTwoSubalgebra::toStringKostantSekiguchiTripleInternals");
  std::stringstream out;
  out << "<br>The unknown Kostant-Sekiguchi elements.<br>";
  std::stringstream tripleSystem;
  tripleSystem << "\\begin{array}{rcl}";
  tripleSystem
  << "h&=&"
  << this->hPolynomialAlgebraic.toString(format)
  << "\\\\\n";
  tripleSystem
  << "e&=&"
  << this->eKostantSekiguchiUnknown.toString(format)
  << "\\\\\n";
  tripleSystem << "f&=&" << this->fKostantSekiguchiUnknown.toString(format);
  tripleSystem << "\\end{array}";
  out << HtmlRoutines::getMathNoDisplay(tripleSystem.str());
  out << "<br>\\(e-f=" << this->eMinusFUnknown.toString(format) << "\\)";
  out
  << "<br>\\(\\theta(e-f)="
  << this->involutionAppliedToEMinusF.toString(format)
  << "\\)";
  out << "<br>The polynomial system we need to solve.<br>";
  out
  << this->toStringPolynomialSystem(
    this->systemToSolveKostantSekiguchi, format
  );
  return out.str();
}

template <typename Coefficient>
std::string SlTwoSubalgebra::toStringPolynomialSystem(
  const PolynomialSubstitution<Coefficient>& system, FormatExpressions* format
) const {
  std::stringstream latexStreamActual;
  latexStreamActual << "\\begin{array}{rcl}";
  for (int i = 0; i < system.size; i ++) {
    latexStreamActual << system[i].toString(format) << "&=&0\\\\";
  }
  latexStreamActual << "\\end{array}";
  return HtmlRoutines::getMathNoDisplay(latexStreamActual.str());
}

std::string SlTwoSubalgebra::toStringTripleUnknownsPolynomialSystem(
  FormatExpressions* format
) const {
  std::stringstream out;
  out
  << "<br>The polynomial system that corresponds "
  << "to finding the h, e, f triple:<br>\n";
  out
  << this->toStringPolynomialSystem(this->systemToSolve, format)
  << "\n<br>\n";
  return out.str();
}

std::string SlTwoSubalgebra::toStringTriple(FormatExpressions* format) const {
  std::stringstream out;
  out << "\\begin{array}{rcl}";
  out << "h&=&" << this->hElement.toString(format) << "\\\\\n";
  out << "e&=&" << this->eElement.toString(format) << "\\\\\n";
  out << "f&=&" << this->fElement.toString(format);
  out << "\\end{array}";
  return HtmlRoutines::getMathNoDisplay(out.str());
}

void SlTwoSubalgebraCandidate::computeLieBracketsUnknowns() {
  STACK_TRACE("SlTwoSubalgebraCandidate::computeLieBracketsUnknowns");
  this->getOwnerSemisimpleAlgebra().lieBracket(
    this->eArbitraryUnknown,
    this->fArbitrary,
    this->eBracketFMinusHArbitraryUnknown
  );
  this->eBracketFMinusHArbitraryUnknown -= this->hPolynomialRational;
  this->getOwnerSemisimpleAlgebra().lieBracket(
    this->eUnknown, this->fUnknown, this->eBracketFMinusHUnknown
  );
  this->eBracketFMinusHUnknown -= this->hPolynomialRational;
  this->getOwnerSemisimpleAlgebra().lieBracket(
    this->eKostantSekiguchiUnknown,
    this->fKostantSekiguchiUnknown,
    this->eBracketFMinusHUnknownKostantSekiguchi
  );
  this->eBracketFMinusHUnknownKostantSekiguchi -= this->hPolynomialAlgebraic;
}

void SlTwoSubalgebra::fromSlTwoSubalgebraCandidate(
  SlTwoSubalgebraCandidate& input
) {
  STACK_TRACE("SlTwoSubalgebraCandidate::fromSlTwoSubalgebraCandidate");
  this->owner = input.owner;
  this->container = input.container;
  this->algebraicClosure = input.algebraicClosure;
  this->preferredAmbientSimpleBasis = input.preferredAmbientSimpleBasis;
  this->systemArbitraryMatrix = input.systemArbitraryMatrix;
  this->systemArbitraryColumnVector = input.systemArbitraryColumnVector;
  this->hPolynomialAlgebraic = input.hPolynomialAlgebraic;
  this->fArbitrary = input.fArbitrary;
  this->hElement = input.hElement;
  this->eElement = input.eElement;
  this->fElement = input.fElement;
  this->eArbitraryUnknown = input.eArbitraryUnknown;
  this->eUnknown = input.eUnknown;
  this->fUnknown = input.fUnknown;
  this->eBracketFMinusHUnknown = input.eBracketFMinusHUnknown;
  this->eKostantSekiguchiUnknown = input.eKostantSekiguchiUnknown;
  this->fKostantSekiguchiUnknown = input.fKostantSekiguchiUnknown;
  this->eBracketFMinusHUnknownKostantSekiguchi =
  input.eBracketFMinusHUnknownKostantSekiguchi;
  this->systemToSolve = input.systemToSolve;
  this->systemToSolveKostantSekiguchi = input.systemToSolveKostantSekiguchi;
  this->eKostantSekiguchi = input.eKostantSekiguchi;
  this->fKostantSekiguchi = input.fKostantSekiguchi;
  this->eMinusFUnknown = input.eMinusFUnknown;
  this->involutionAppliedToEMinusF = input.involutionAppliedToEMinusF;
  this->lengthHSquared = input.lengthHSquared;
}

bool SlTwoSubalgebra::attemptToComputeCentralizer() {
  STACK_TRACE("SlTwoSubalgebra::attemptToComputeCentralizer");
  this->flagCentralizerIsRegular = false;
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
    Rational dimensionSemisimplePartOfCentralizerRootSubalgebra =
    currentMinimalContainer.centralizerDynkinType.getRankRational() +
    currentMinimalContainer.centralizerDynkinType.getRootSystemSize();
    this->dimensionCentralizerToralPart =
    this->owner->getRank() -
    currentMinimalContainer.dynkinType.getRank() -
    currentMinimalContainer.centralizerDynkinType.getRank();
    Rational totalCentalizerCandidateDimension =
    dimensionSemisimplePartOfCentralizerRootSubalgebra +
    this->dimensionCentralizerToralPart;
    if (totalCentalizerCandidateDimension == this->dimensionCentralizer) {
      this->flagCentralizerIsRegular = true;
      this->centralizerComputer.flagTypeComputed = true;
      this->centralizerComputer.typeIfKnown =
      currentMinimalContainer.centralizerDynkinType;
      return true;
    }
  }
  this->centralizerComputer.generatorsToCentralize =
  List<ElementSemisimpleLieAlgebra<Rational> >(
    {this->eElement, this->hElement, this->fElement}
  );
  Rational dynkinIndex = this->getDynkinIndex();
  this->centralizerComputer.initialize(
    this->owner, this->algebraicClosure, &dynkinIndex
  );
  this->centralizerComputer.label = this->toStringDynkinType();
  if (!this->flagTryToComputeCentralizerFully) {
    // Guard until the centralizer computation algorithm
    // is properly implemented.
    return false;
  }
  return this->centralizerComputer.compute();
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

Rational SlTwoSubalgebra::getDynkinIndex() const {
  return
  this->lengthHSquared * this->getOwnerWeyl().getLongestRootLengthSquared() /
  4;
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
  positiveRootsContainingRegularSubalgebra.coordinatesInBasis(
    this->preferredAmbientSimpleBasis,
    coordinatesInPreferredSimpleBasis,
    nullptr
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
    int topMultiplicity = bufferHighestWeights[j];
    if (topMultiplicity < 0) {
      global.fatal
      << "The sl(2)-module decomposition "
      << "shows an sl(2)-module with highest weight "
      << topMultiplicity
      << " which is impossible. Here is the sl(2) subalgebra. "
      << this->toString()
      << "."
      << global.fatal;
    }
    if (topMultiplicity > 0) {
      currentHighestWeight.weightFundamentalCoordinates[0] =
      j - indexZeroWeight;
      outputHighestWeights.addMonomial(currentHighestWeight, topMultiplicity);
      if (j != indexZeroWeight) {
        bufferHighestWeights[indexZeroWeight * 2 - j] -= topMultiplicity;
      }
      for (int k = j - 2; k >= indexZeroWeight; k -= 2) {
        bufferHighestWeights[k] -= topMultiplicity;
        if (k != indexZeroWeight) {
          bufferHighestWeights[indexZeroWeight * 2 - k] -= topMultiplicity;
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

void SlTwoSubalgebra::computeModuleDecompositionsitionAmbientLieAlgebra() {
  this->checkConsistency();
  this->computeModuleDecompositionsition(
    this->getOwnerWeyl().rootsOfBorel,
    this->getOwnerWeyl().cartanSymmetric.numberOfRows,
    this->moduleDecompositionAmbientSubalgebra,
    this->moduleDimensions
  );
}

void SlTwoSubalgebra::
computeModuleDecompositionMinimalContainingRegularSubalgebras(
  SlTwoSubalgebras& owner
) {
  STACK_TRACE(
    "SlTwoSubalgebra::"
    "computeModuleDecompositionMinimalContainingRegularSubalgebras"
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
  RootSubalgebra& minimalContainingRegularSubalgebra
) {
  STACK_TRACE("SlTwoSubalgebra::makeReportPrecomputations");
  int dimension = this->getOwnerSemisimpleAlgebra().getRank();
  Vectors<Rational> roots;
  roots = minimalContainingRegularSubalgebra.simpleRootsReductiveSubalgebra;
  this->getOwnerSemisimpleAlgebra().weylGroup.
  transformToSimpleBasisGeneratorsWithRespectToH(
    roots, this->hElement.getCartanPart()
  );
  DynkinDiagramRootSubalgebra diagram;
  diagram.ambientBilinearForm = this->getOwnerWeyl().cartanSymmetric;
  diagram.ambientRootSystem = this->getOwnerWeyl().rootSystem;
  diagram.computeDiagramInputIsSimpleBasis(roots);
  roots.makeEiBasis(dimension);
  this->getOwnerSemisimpleAlgebra().weylGroup.
  transformToSimpleBasisGeneratorsWithRespectToH(
    roots, this->hElement.getCartanPart()
  );
  DynkinDiagramRootSubalgebra diagramComputer;
  diagramComputer.ambientBilinearForm = this->getOwnerWeyl().cartanSymmetric;
  diagramComputer.ambientRootSystem = this->getOwnerWeyl().rootSystem;
  diagramComputer.computeDiagramInputIsSimpleBasis(roots);
  this->preferredAmbientSimpleBasis = roots;
  this->hCharacteristic.setSize(dimension);
  for (int i = 0; i < dimension; i ++) {
    this->hCharacteristic[i] =
    this->getOwnerSemisimpleAlgebra().weylGroup.rootScalarCartanRoot(
      this->hElement.getCartanPart(), this->preferredAmbientSimpleBasis[i]
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

const WeylGroupData& SlTwoSubalgebraCandidate::getOwnerWeyl() const {
  if (this->owner == nullptr) {
    global.fatal << "Weyl group with non-initialized owner. " << global.fatal;
  }
  return this->owner->weylGroup;
}

SemisimpleLieAlgebra& SlTwoSubalgebraCandidate::getOwnerSemisimpleAlgebra() {
  if (this->owner == nullptr) {
    global.fatal
    << "Attempt to access "
    << "the ambient Lie algebra of a "
    << "non-initialized sl(2)-subalgebra. "
    << global.fatal;
  }
  return *this->owner;
}

void SlTwoSubalgebraCandidate::initializeUnknownTriples(
  const Vector<Rational>& targetH
) {
  STACK_TRACE("SlTwoSubalgebraCandidate::initializeUnknownTriples");
  if (this->algebraicClosure == nullptr) {
    global.fatal << "The algebraic closure is required." << global.fatal;
  }
  // Set the h element.
  this->hPolynomialRational.makeZero();
  this->hPolynomialRational.makeCartanGenerator(
    targetH, this->getOwnerSemisimpleAlgebra()
  );
  // Set the h element but over the algebraic numbers.
  this->hPolynomialAlgebraic.makeZero();
  this->hPolynomialAlgebraic.makeCartanGenerator(
    targetH, this->getOwnerSemisimpleAlgebra()
  );
  // Zero e and f for the arbitrary f choice optimization.
  this->eArbitraryUnknown.makeZero();
  // In the code below, f will be chosen arbitrarily.
  this->fArbitrary.makeZero();
  // Zero e and f without arbitrary optimizations.
  this->eUnknown.makeZero();
  this->fUnknown.makeZero();
  // Zero e  and f for the Kostant-Sekiguchi computation.
  this->eKostantSekiguchiUnknown.makeZero();
  this->fKostantSekiguchiUnknown.makeZero();
  int rank = this->owner->weylGroup.dynkinType.getRank();
  char type = 0;
  if (!this->owner->weylGroup.dynkinType.isSimple(&type)) {
    type = 'X';
  }
  Rational targetHLength =
  targetH.scalarProduct(targetH, this->getOwnerWeyl().cartanSymmetric);
  int dynkinIndex = 0;
  (targetHLength / 2).isSmallInteger(&dynkinIndex);
  for (int i = 0; i < this->participatingPositiveRoots.size; i ++) {
    // Initialize arbitrary triple
    ChevalleyGenerator negative;
    negative.makeGeneratorRootSpace(
      this->getOwnerSemisimpleAlgebra(),
      - this->participatingPositiveRoots[i]
    );
    Polynomial<Rational> fArbitraryConstant;
    fArbitraryConstant.makeConstant(
      SlTwoSubalgebraCandidate::fArbitraryCoefficient(
        i, type, rank, dynkinIndex
      )
    );
    this->fArbitrary.addMonomial(negative, fArbitraryConstant);
    ChevalleyGenerator positive;
    positive.makeGeneratorRootSpace(
      this->getOwnerSemisimpleAlgebra(), this->participatingPositiveRoots[i]
    );
    Polynomial<Rational> unknownCoefficient;
    unknownCoefficient.makeMonomial(i, 1, 1);
    this->eArbitraryUnknown.addMonomial(positive, unknownCoefficient);
    // Initialize true unknown triple.
    unknownCoefficient.makeMonomial(i, 1, 1);
    this->eUnknown.addMonomial(positive, unknownCoefficient);
    unknownCoefficient.makeMonomial(
      i + this->participatingPositiveRoots.size, 1, 1
    );
    this->fUnknown.addMonomial(negative, unknownCoefficient);
    Polynomial<AlgebraicNumber> unknownCoefficientAlgebraic;
    unknownCoefficientAlgebraic.makeMonomial(
      i, 1, this->algebraicClosure->one()
    );
    this->eKostantSekiguchiUnknown.addMonomial(
      positive, unknownCoefficientAlgebraic
    );
    unknownCoefficientAlgebraic.makeMonomial(
      i + this->participatingPositiveRoots.size,
      1,
      this->algebraicClosure->one()
    );
    this->fKostantSekiguchiUnknown.addMonomial(
      negative, unknownCoefficientAlgebraic
    );
  }
}

bool SlTwoSubalgebraCandidate::attemptExtendingHFtoHEFWithRespectToSubalgebra(
  Vectors<Rational>& rootsWithCharacteristic2,
  Selection& zeroCharacteristics,
  Vectors<Rational>& simpleBasisSubalgebras,
  Vector<Rational>& h,
  bool computeRealForm,
  MapReferences<std::string, AlgebraicClosureRationals>& algebraicClosures
) {
  STACK_TRACE(
    "SlTwoSubalgebraCandidate::attemptExtendingHFtoHEFWithRespectToSubalgebra"
  );
  if (
    zeroCharacteristics.cardinalitySelection ==
    zeroCharacteristics.numberOfElements
  ) {
    return false;
  }
  std::stringstream keyStream;
  keyStream << this->owner->weylGroup.dynkinType.toString() << h.toString();
  this->algebraicClosure =
  &algebraicClosures.getValueCreateNoInitialization(keyStream.str());
  this->algebraicClosure->reset();
  this->hAlgebraic.makeCartanGenerator(h, this->getOwnerSemisimpleAlgebra());
  this->participatingPositiveRoots.size = 0;
  int relativeDimension = simpleBasisSubalgebras.size;
  if (relativeDimension != zeroCharacteristics.numberOfElements) {
    global.fatal << "Relative dimension is incorrect. " << global.fatal;
  }
  // Format. We are looking for an sl(2) for which
  // e = a_0 g^\alpha_0+...a_kg^\alpha_k,
  // and
  // f=b_0 g^{-\alpha_0}+... +b_kg^{-\alpha_k}
  // where the first \alpha's are ordered as in rootsInPlay.
  // Those are ordered as follows. First come the simple roots of
  // characteristic 2,
  // and the last \alpha's are the members of SelectedExtraPositiveRoots
  // (i.e. root equal to the sum of one simple root
  // of characteristic 2 with a simple roots of characteristic 0).
  // Then the first k variables of the polynomials below will be
  // a_0, ..., a_k and
  // the last k variables will be the b_i's,
  // the l^th polynomial will correspond to the coefficient of g^\alpha_{l/2},
  // where l/2 is the index of the
  // rootattemptExtendingHFtoHEFWithRespectToSubalgebra
  // of SelectedExtraPositiveRoots, if l is even, and to the
  // coefficient of  g^{-\alpha_{(l+ 1)/2}} otherwise.
  for (int i = 0; i < relativeDimension; i ++) {
    if (!zeroCharacteristics.selected[i]) {
      this->participatingPositiveRoots.addOnTop(simpleBasisSubalgebras[i]);
    }
  }
  Vectors<Rational> selectedExtraPositiveRoots;
  for (int i = 0; i < rootsWithCharacteristic2.size; i ++) {
    if (!simpleBasisSubalgebras.contains(rootsWithCharacteristic2[i])) {
      selectedExtraPositiveRoots.addOnTop(rootsWithCharacteristic2[i]);
    }
  }
  this->participatingPositiveRoots.addListOnTop(selectedExtraPositiveRoots);
  LinearMapSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >
  cartanInvolutionStandard;
  LinearMapSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >*
  cartanInvolutionToRespect =
  nullptr;
  if (computeRealForm) {
    if (
      this->owner->hasImplementedCartanInvolutionMaximallyCompactCase(
        &cartanInvolutionStandard
      )
    ) {
      cartanInvolutionToRespect = &cartanInvolutionStandard;
    }
  }
  this->initializeHEFSystemFromFCoefficients(h, cartanInvolutionToRespect);
  Matrix<Rational> matrix;
  Matrix<Rational> adjoinedColumn;
  Matrix<Rational> result;
  matrix = this->systemArbitraryMatrix;
  adjoinedColumn = this->systemArbitraryColumnVector;
  this->fElement.makeZero();
  this->eElement.makeZero();
  ChevalleyGenerator generator;
  if (
    !Matrix<Rational>::solveAxEqualsBModifyInputReturnFirstSolutionIfExists(
      matrix, adjoinedColumn, result
    )
  ) {
    return false;
  }
  this->fElement = this->fArbitrary;
  for (int i = 0; i < this->participatingPositiveRoots.size; i ++) {
    generator.makeGenerator(
      this->getOwnerSemisimpleAlgebra(),
      this->getOwnerSemisimpleAlgebra().getGeneratorIndexFromRoot(
        this->participatingPositiveRoots[i]
      )
    );
    this->eElement.addMonomial(generator, result.elements[i][0]);
  }
  if (computeRealForm) {
    this->attemptRealizingKostantSekiguchi();
  }
  return true;
}

void SlTwoSubalgebraCandidate::initializeHEFSystemFromFCoefficients(
  const Vector<Rational>& targetH,
  LinearMapSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >*
  cartanInvolutionPreservedByEMinusF
) {
  STACK_TRACE(
    "SlTwoSubalgebraCandidate::initializeHEFSystemFromFCoefficients"
  );
  this->checkConsistencyParticipatingRoots(targetH);
  this->initializeUnknownTriples(targetH);
  this->computeLieBracketsUnknowns();
  this->computePolynomialSystems();
  this->adjoinKostantSekiguchiRelationsToPolynomialSystem(
    cartanInvolutionPreservedByEMinusF
  );
  this->initializeHEFSystemFromFCoefficientsPartTwo();
}

void SlTwoSubalgebraCandidate::initializeHEFSystemFromFCoefficientsPartTwo() {
  STACK_TRACE(
    "SlTwoSubalgebraCandidate::"
    "initializeHEFSystemFromFCoefficientsPartTwo"
  );
  this->systemArbitraryMatrix.initialize(
    this->systemToSolve.size, this->participatingPositiveRoots.size
  );
  this->systemArbitraryColumnVector.initialize(this->systemToSolve.size, 1);
  this->systemArbitraryMatrix.makeZero();
  this->systemArbitraryColumnVector.makeZero();
  this->systemToSolveArbitrary.getLinearSystemFromLinearPolynomials(
    this->systemArbitraryMatrix, this->systemArbitraryColumnVector
  );
}

bool SlTwoSubalgebraCandidate::attemptRealizingKostantSekiguchi() {
  STACK_TRACE("SlTwoSubalgebraCandidate::attemptRealizingKostantSekiguchi");
  PolynomialSystem<AlgebraicNumber> computation;
  computation.algebraicClosure = this->algebraicClosure;
  computation.initializeForSystemSolution();
  computation.groebner.polynomialOrder.monomialOrder =
  MonomialPolynomial::orderDefault();
  computation.solveSerreLikeSystem(this->systemToSolveKostantSekiguchi);
  if (!computation.flagSystemSolvedOverBaseField) {
    return false;
  }
  PolynomialSubstitution<AlgebraicNumber> solution;
  solution.setSize(computation.systemSolution.size);
  for (int i = 0; i < solution.size; i ++) {
    solution[i].makeConstant(computation.systemSolution[i]);
  }
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > eSolved;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > fSolved;
  eSolved = this->eKostantSekiguchiUnknown;
  fSolved = this->fKostantSekiguchiUnknown;
  eSolved.substituteInCoefficients(solution);
  fSolved.substituteInCoefficients(solution);
  this->eKostantSekiguchi = eSolved;
  this->fKostantSekiguchi = fSolved;
  return true;
}

bool SlTwoSubalgebraCandidate::checkConsistencyParticipatingRoots(
  const Vector<Rational>& targetH
) {
  STACK_TRACE("SlTwoSubalgebraCandidate::checkConsistencyParticipatingRoots");
  for (int i = 0; i < this->participatingPositiveRoots.size; i ++) {
    if (
      this->getOwnerWeyl().rootScalarCartanRoot(
        targetH, this->participatingPositiveRoots[i]
      ) ==
      2
    ) {
      continue;
    }
    global.fatal
    << "The scalar product of the h element: "
    << targetH.toString()
    << " and the root in play "
    << this->participatingPositiveRoots[i].toString()
    << " must be 2, but equals instead "
    << this->getOwnerWeyl().rootScalarCartanRoot(
      targetH, this->participatingPositiveRoots[i]
    ).toString()
    << global.fatal;
  }
  return true;
}

void SlTwoSubalgebraCandidate::computePolynomialSystems() {
  this->systemToSolveArbitrary.clear();
  for (int i = 0; i < this->eBracketFMinusHArbitraryUnknown.size(); i ++) {
    this->systemToSolveArbitrary.addOnTop(
      this->eBracketFMinusHArbitraryUnknown.coefficients[i]
    );
  }
  this->systemToSolve.clear();
  for (int i = 0; i < this->eBracketFMinusHUnknown.size(); i ++) {
    this->systemToSolve.addOnTop(
      this->eBracketFMinusHUnknown.coefficients[i]
    );
  }
  this->systemToSolveKostantSekiguchi.clear();
  for (
    int i = 0; i < this->eBracketFMinusHUnknownKostantSekiguchi.size(); i ++
  ) {
    this->systemToSolveKostantSekiguchi.addOnTop(
      this->eBracketFMinusHUnknownKostantSekiguchi.coefficients[i]
    );
  }
}

void SlTwoSubalgebraCandidate::
adjoinKostantSekiguchiRelationsToPolynomialSystem(
  LinearMapSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >*
  cartanInvolutionPreservedByEMinusF
) {
  STACK_TRACE(
    "SlTwoSubalgebraCandidate::"
    "adjoinKostantSekiguchiRelationsToPolynomialSystem"
  );
  if (cartanInvolutionPreservedByEMinusF == nullptr) {
    return;
  }
  this->eMinusFUnknown =
  this->eKostantSekiguchiUnknown - this->fKostantSekiguchiUnknown;
  cartanInvolutionPreservedByEMinusF->applyTo(
    this->eMinusFUnknown, this->involutionAppliedToEMinusF
  );
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > mustBeZero;
  mustBeZero = this->eMinusFUnknown - this->involutionAppliedToEMinusF;
  for (int i = 0; i < mustBeZero.size(); i ++) {
    this->systemToSolveKostantSekiguchi.addOnTop(mustBeZero.coefficients[i]);
  }
}

bool SlTwoSubalgebras::checkMinimalContainingRootSubalgebras() const {
  this->checkInitialization();
  this->checkConsistency();
  for (const SlTwoSubalgebra& current : this->allSubalgebras) {
    if (current.container != this) {
      global.fatal << "Corrupt sl(2) subalgebra." << global.fatal;
    }
    current.checkIndicesMinimalContainingRootSubalgebras();
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
  if (this->allSubalgebras.flagDeallocated) {
    global.fatal << "Use after free of SemisimpleLieAlgebra. " << global.fatal;
  }
  if (this->owner != nullptr) {
    this->owner->checkConsistency();
  }
  for (const SlTwoSubalgebra& current : this->allSubalgebras) {
    current.checkConsistency();
  }
  return true;
}

void SlTwoSubalgebras::findSl2Subalgebras(
  SemisimpleLieAlgebra& inputOwner,
  SlTwoSubalgebras& output,
  bool computeRealForm,
  MapReferences<std::string, AlgebraicClosureRationals>& algebraicClosures,
  DynkinSimpleType* restrictToThisSl2Triple
) {
  STACK_TRACE("SlTwoSubalgebras::findSl2Subalgebras");
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
    output.appendSSl2SubalgebrasFromRootSubalgebra(
      output.rootSubalgebras.subalgebras[i],
      output,
      i,
      computeRealForm,
      algebraicClosures,
      restrictToThisSl2Triple
    );
  }
  // Sort subalgebras by dynkin index.
  List<int> permutation;
  List<int> indexMap;
  permutation.setSize(output.allSubalgebras.size);
  indexMap.setSize(permutation.size);
  for (int i = 0; i < permutation.size; i ++) {
    permutation[i] = i;
  }
  output.allSubalgebras.quickSortDescending(nullptr, &permutation);
  for (int i = 0; i < indexMap.size; i ++) {
    indexMap[permutation[i]] = i;
  }
  for (
    int j = 0; j < output.indicesSl2sContainedInRootSubalgebras.size; j ++
  ) {
    for (
      int k = 0; k < output.indicesSl2sContainedInRootSubalgebras[j].size; k
      ++
    ) {
      output.indicesSl2sContainedInRootSubalgebras[j][k] =
      indexMap[output.indicesSl2sContainedInRootSubalgebras[j][k]];
    }
  }
  inputOwner.checkConsistency();
  output.checkMinimalContainingRootSubalgebras();
  output.computeRootSubalgebraContainers();
  output.computeCentralizers();
}

void SlTwoSubalgebras::appendSSl2SubalgebrasFromRootSubalgebra(
  RootSubalgebra& container,
  SlTwoSubalgebras& output,
  int indexRootSubalgebraInContainer,
  bool computeRealForm,
  MapReferences<std::string, AlgebraicClosureRationals>& algebraicClosures,
  DynkinSimpleType* restrictToThisSl2Triple
) {
  STACK_TRACE("SlTwoSubalgebras::appendSSl2SubalgebrasFromRootSubalgebra");
  // reference: Dynkin, semisimple Lie algebras of simple lie algebras,
  // theorems
  // 10.1 - 10.4
  int relativeDimension = container.simpleRootsReductiveSubalgebra.size;
  if (relativeDimension == 0) {
    return;
  }
  Selection selectionRootsWithZeroCharacteristic;
  Selection simpleRootsChar2;
  Vectors<Rational> rootsScalarProduct2HNonRaised;
  Vectors<Rational> reflectedSimpleBasisK;
  rootsScalarProduct2HNonRaised.reserve(
    container.positiveRootsReductiveSubalgebra.size
  );
  ElementWeylGroup raisingElement;
  selectionRootsWithZeroCharacteristic.initialize(relativeDimension);
  Matrix<Rational> invertedRelativeKillingForm;
  invertedRelativeKillingForm.initialize(relativeDimension, relativeDimension);
  WeylGroupData& ambientWeyl = container.getAmbientWeyl();
  for (int k = 0; k < relativeDimension; k ++) {
    for (int j = 0; j < relativeDimension; j ++) {
      invertedRelativeKillingForm(k, j) =
      ambientWeyl.rootScalarCartanRoot(
        container.simpleRootsReductiveSubalgebra[k],
        container.simpleRootsReductiveSubalgebra[j]
      );
    }
  }
  invertedRelativeKillingForm.invert();
  int numberOfCycles =
  MathRoutines::twoToTheNth(
    selectionRootsWithZeroCharacteristic.numberOfElements
  );
  ProgressReport report;
  Vectors<Rational> rootsZeroChar;
  rootsZeroChar.reserve(
    selectionRootsWithZeroCharacteristic.numberOfElements
  );
  Vectors<Rational> relativeRootSystem;
  container.positiveRootsReductiveSubalgebra.coordinatesInBasis(
    container.simpleRootsReductiveSubalgebra, relativeRootSystem, nullptr
  );
  SlTwoSubalgebraCandidate candidate;
  candidate.container = &output;
  candidate.owner = &container.getOwnerLieAlgebra();
  SemisimpleLieAlgebra& lieAlgebra = container.getOwnerLieAlgebra();
  DynkinDiagramRootSubalgebra diagramZeroCharacteristicRoots;
  for (
    int cycleCounter = 0; cycleCounter < numberOfCycles;
    cycleCounter ++,
    selectionRootsWithZeroCharacteristic.incrementSelection()
  ) {
    container.simpleRootsReductiveSubalgebra.subSelection(
      selectionRootsWithZeroCharacteristic, rootsZeroChar
    );
    diagramZeroCharacteristicRoots.computeDiagramTypeModifyInput(
      rootsZeroChar, ambientWeyl
    );
    int slack = 0;
    rootsScalarProduct2HNonRaised.size = 0;
    simpleRootsChar2 = selectionRootsWithZeroCharacteristic;
    simpleRootsChar2.invertSelection();
    Vector<Rational> simpleRootsChar2Vector;
    simpleRootsChar2Vector = simpleRootsChar2;
    for (int j = 0; j < relativeRootSystem.size; j ++) {
      if (simpleRootsChar2Vector.scalarEuclidean(relativeRootSystem[j]) == 1) {
        slack ++;
        rootsScalarProduct2HNonRaised.addOnTop(
          container.positiveRootsReductiveSubalgebra[j]
        );
      }
    }
    int dynkinEpsilon =
    diagramZeroCharacteristicRoots.numberRootsGeneratedByDiagram() +
    relativeDimension - slack;
    // If Dynkin's epsilon is not zero the subalgebra cannot be an S sl(2)
    // subalgebra.
    // otherwise, as far as I understand, it always is //
    // except for G_2 (go figure!).
    // (but selectionRootsWithZeroCharacteristic still have to be found)
    // this is done in the below code.
    if (dynkinEpsilon != 0) {
      continue;
    }
    Vector<Rational> relativeCharacteristic;
    Vector<Rational> relativeSimpleCoordinates;
    relativeCharacteristic.makeZero(relativeDimension);
    for (int k = 0; k < relativeDimension; k ++) {
      if (!selectionRootsWithZeroCharacteristic.selected[k]) {
        relativeCharacteristic[k] = 2;
      }
    }
    invertedRelativeKillingForm.actOnVectorColumn(
      relativeCharacteristic, relativeSimpleCoordinates
    );
    candidate.candidateH.makeZero(lieAlgebra.getRank());
    for (int j = 0; j < relativeDimension; j ++) {
      candidate.candidateH +=
      container.simpleRootsReductiveSubalgebra[j] *
      relativeSimpleCoordinates[j];
    }
    Rational dynkinIndex =
    ambientWeyl.rootScalarCartanRoot(
      candidate.candidateH, candidate.candidateH
    ) /
    2;
    DynkinSimpleType dynkinType;
    dynkinType.makeArbitrary('A', 1, dynkinIndex);
    if (
      restrictToThisSl2Triple != nullptr && (*restrictToThisSl2Triple) !=
      dynkinType
    ) {
      // Skip this type.
      continue;
    }
    for (int k = 0; k < rootsScalarProduct2HNonRaised.size; k ++) {
      if (
        ambientWeyl.rootScalarCartanRoot(
          candidate.candidateH, rootsScalarProduct2HNonRaised[k]
        ) !=
        2
      ) {
        global.fatal
        << "CharacteristicH is: "
        << candidate.candidateH.toString()
        << "; rootsWithScalarProduct2NonRaised: "
        << rootsScalarProduct2HNonRaised.toString()
        << "; the scalar product with vector "
        << rootsScalarProduct2HNonRaised[k].toString()
        << " is:  "
        << ambientWeyl.rootScalarCartanRoot(
          candidate.candidateH, rootsScalarProduct2HNonRaised[k]
        ).toString()
        << " which is supposed to equal 2. "
        << global.fatal;
      }
    }
    ambientWeyl.raiseToDominantWeight(
      candidate.candidateH, nullptr, nullptr, &raisingElement
    );
    reflectedSimpleBasisK = container.simpleRootsReductiveSubalgebra;
    for (int k = 0; k < reflectedSimpleBasisK.size; k ++) {
      ambientWeyl.actOn(raisingElement, reflectedSimpleBasisK[k]);
    }
    candidate.rootsWithScalar2WithH = rootsScalarProduct2HNonRaised;
    for (int k = 0; k < candidate.rootsWithScalar2WithH.size; k ++) {
      ambientWeyl.actOn(raisingElement, candidate.rootsWithScalar2WithH[k]);
    }
    for (int i = 0; i < candidate.rootsWithScalar2WithH.size; i ++) {
      if (
        ambientWeyl.rootScalarCartanRoot(
          candidate.candidateH, candidate.rootsWithScalar2WithH[i]
        ) !=
        2
      ) {
        global.fatal
        << "Bad scalar product after raising: raised characteristic: "
        << candidate.candidateH.toString()
        << " simplebasisK: "
        << container.simpleRootsReductiveSubalgebra.toString()
        << "raised by: "
        << raisingElement.toString()
        << " to get: "
        << reflectedSimpleBasisK.toString()
        << " theSl2.RootsWithScalar2WithH: "
        << candidate.rootsWithScalar2WithH.toString()
        << ", theSl2.RootsWithScalar2WithH[i]: "
        << candidate.rootsWithScalar2WithH[i].toString()
        << " scalar product: "
        << ambientWeyl.rootScalarCartanRoot(
          candidate.candidateH, candidate.rootsWithScalar2WithH[i]
        ).toString()
        << ". The inverted relative cartan: "
        << invertedRelativeKillingForm.toString()
        << ". The cartan: "
        << ambientWeyl.cartanSymmetric.toString()
        << ". "
        << global.fatal;
      }
    }
    candidate.hElement.makeCartanGenerator(candidate.candidateH, lieAlgebra);
    candidate.lengthHSquared =
    candidate.getOwnerSemisimpleAlgebra().weylGroup.rootScalarCartanRoot(
      candidate.candidateH, candidate.candidateH
    );
    candidate.eElement.makeZero();
    candidate.fElement.makeZero();
    if (
      candidate.attemptExtendingHFtoHEFWithRespectToSubalgebra(
        candidate.rootsWithScalar2WithH,
        selectionRootsWithZeroCharacteristic,
        reflectedSimpleBasisK,
        candidate.candidateH,
        computeRealForm,
        algebraicClosures
      )
    ) {
      container.addSlTwoSubalgebraIfNew(
        candidate, output, indexRootSubalgebraInContainer
      );
    } else {
      output.unsuitableHs.addOnTop(candidate.candidateH);
      DynkinType tempType;
      diagramZeroCharacteristicRoots.getDynkinType(tempType);
      global.comments
      << "<br>obtained bad characteristic "
      << candidate.candidateH.toString()
      << ". The zero char root diagram is "
      << tempType.toString()
      << "; the Dynkin epsilon is "
      << dynkinEpsilon
      << "= the num roots generated by diagram "
      << diagramZeroCharacteristicRoots.numberRootsGeneratedByDiagram()
      << " + the relative dimension "
      << relativeDimension
      << " - the slack "
      << slack
      << "<br>The relative root system is: "
      << relativeRootSystem.toString();
    }
    std::stringstream out;
    out
    << "Exploring Dynkin characteristics case "
    << cycleCounter + 1
    << " out of "
    << numberOfCycles;
    report.report(out.str());
  }
}

void SlTwoSubalgebras::computeCentralizers() {
  for (SlTwoSubalgebra& currentSubalgebra : this->allSubalgebras) {
    currentSubalgebra.attemptToComputeCentralizer();
  }
}

void SlTwoSubalgebras::computeOneRootSubalgebraContainers(
  SlTwoSubalgebra& output
) {
  STACK_TRACE("SlTwoSubalgebras::computeOneRootSubalgebraContainers");
  output.indicesMinimalContainingRootSubalgebras.clear();
  HashedList<int> containersOfContainers;
  for (int j : output.indicesContainingRootSubalgebras) {
    RootSubalgebra& container = this->rootSubalgebras.subalgebras[j];
    containersOfContainers.addOnTopNoRepetition(container.containerIndices);
  }
  for (int j : output.indicesContainingRootSubalgebras) {
    if (containersOfContainers.contains(j)) {
      continue;
    }
    output.indicesMinimalContainingRootSubalgebras.addOnTop(j);
  }
}

void SlTwoSubalgebras::computeRootSubalgebraContainers() {
  STACK_TRACE("SlTwoSubalgebras::computeRootSubalgebraContainers");
  for (int i = 0; i < this->allSubalgebras.size; i ++) {
    SlTwoSubalgebra& currentSubalgebra = this->allSubalgebras.getElement(i);
    currentSubalgebra.indexInContainer = i;
    this->computeOneRootSubalgebraContainers(currentSubalgebra);
    this->checkConsistency();
  }
  this->computeModuleDecompositionsitionsOfAmbientLieAlgebra();
}

void SlTwoSubalgebras::computeModuleDecompositionsitionsOfAmbientLieAlgebra() {
  this->checkConsistency();
  for (SlTwoSubalgebra& current : this->allSubalgebras) {
    current.computeModuleDecompositionsitionAmbientLieAlgebra();
  }
  this->allSubalgebras.grandMasterConsistencyCheck();
}

bool SlTwoSubalgebras::containsSl2WithGivenH(
  Vector<Rational>& elementH, int* outputIndex
) {
  if (outputIndex != nullptr) {
    *outputIndex = - 1;
  }
  ElementSemisimpleLieAlgebra<Rational> cartanElement;
  this->checkInitialization();
  cartanElement.makeCartanGenerator(elementH, *this->owner);
  for (int i = 0; i < this->allSubalgebras.size; i ++) {
    if (this->allSubalgebras[i].hElement == cartanElement) {
      if (outputIndex != nullptr) {
        *outputIndex = i;
      }
      return true;
    }
  }
  return false;
}

std::string SlTwoSubalgebras::
toStringModuleDecompositionMinimalContainingRegularSubalgebras(
  bool useLatex, bool useHtml
) {
  std::string currentString;
  std::stringstream out;
  for (SlTwoSubalgebra& current : this->allSubalgebras) {
    current.toStringModuleDecompositionMinimalContainingRegularSubalgebras(
      useLatex, useHtml, *this, currentString
    );
    out << currentString;
    if (useHtml) {
      out << "\n<br>";
    }
    out << "\n";
  }
  return out.str();
}

bool SlTwoSubalgebras::isHOfConstructedSlTwo(const Vector<Rational>& h) const {
  for (const SlTwoSubalgebra& current : this->allSubalgebras) {
    if (h == current.hElement.getCartanPart()) {
      return true;
    }
  }
  return false;
}

std::string SlTwoSubalgebras::toHTMLFieldReport() const {
  std::stringstream out;
  HashedList<std::string> allFieldsUsed;
  for (const SlTwoSubalgebra& subalgebra : this->allSubalgebras) {
    allFieldsUsed.addOnTopNoRepetition(
      subalgebra.algebraicClosure->toString()
    );
  }
  out << "Extensions of the rationals used";
  if (allFieldsUsed.size > 0) {
    out << " (" << allFieldsUsed.size << " total)";
  }
  out << ": " << allFieldsUsed.toStringCommaDelimited();
  return out.str();
}

std::string SlTwoSubalgebras::toHTMLSummary(FormatExpressions* format) {
  STACK_TRACE("SlTwoSubalgebras::toHTMLSummary");
  std::stringstream out;
  out << "Number of sl(2) subalgebras: " << this->allSubalgebras.size << ".\n";
  out << this->toHTMLSummaryTable(format);
  out << "<hr>";
  out
  << "<br>Length longest root ambient algebra squared/4= "
  << this->getOwnerWeyl().getLongestRootLengthSquared() / 4
  << "<br>";
  out
  << "<br> Given a root subsystem P, and a root subsubsystem P_0, "
  << "in (10.2) of Semisimple subalgebras of semisimple Lie algebras, "
  << "E. Dynkin defines "
  << "a numerical constant e(P, P_0) (which we call Dynkin epsilon). "
  << "<br>In Theorem 10.3, Dynkin proves that if an sl(2) is an "
  << "S-subalgebra in "
  << "the root subalgebra generated by P, such that it has "
  << "characteristic 2 for all simple roots of P lying in P_0, then "
  << "e(P, P_0)= 0. ";
  if (this->unsuitableHs.size == 0) {
    out
    << "It turns out by direct computation that, in the current case of "
    << this->getOwner().toStringLieAlgebraName()
    << ",  e(P,P_0)= 0 implies that an S-sl(2) subalgebra "
    << "of the root subalgebra generated by P with "
    << "characteristic with 2's in the simple roots of P_0 "
    << "always exists. Note that "
    << "Theorem 10.3 is stated in one direction only.";
    return out.str();
  }
  bool allGood = true;
  for (const Vector<Rational>& h : this->unsuitableHs) {
    Rational length = this->getOwnerWeyl().rootScalarCartanRoot(h, h);
    out
    << "H that wasn't realized immediately: "
    << h.toString()
    << ", <b style='color:blue'>Type: "
    << "\\(A^{"
    << (length / 2).toString()
    << "}_{"
    << 1
    << "}\\)</b>. ";
    if (!this->isHOfConstructedSlTwo(h)) {
      allGood = false;
      continue;
    }
    out
    << "<br>"
    << "It turns out that in the current case of Cartan element h = "
    << h
    << " we have that, for a certain P, "
    << "e(P, P_0) equals 0, but I failed to realize the "
    << "corresponding sl(2) as a subalgebra of that P. "
    << "<b style='color:green'>However, it turns out that h "
    << "is indeed an S-subalgebra of a smaller root subalgebra P'.</b>";
  }
  if (!allGood) {
    out << "<br><b style='color:red'>Found bad characteristics!</b>";
  }
  return out.str();
}

std::string SlTwoSubalgebras::toHTMLSummaryTable(FormatExpressions* format) {
  (void) format;
  std::stringstream out;
  std::string displayPathAlgebra = "../";
  std::string idSpanHCharacteristicDescription =
  "spanDynkinHCharacteristicDescription";
  out
  << "<div id='"
  << idSpanHCharacteristicDescription
  << "'>"
  << this->descriptionHCharacteristic
  << "</div><br>";
  out
  << "<div id='idCartanElementRealization'>"
  << this->descriptionHRealization
  << "</div>";
  out
  << "<div id='idMinimalContainingRegularSA'>"
  << this->descriptionMinimalContainingRegularSubalgebras
  << "</div>";
  out
  << "<div id='idModuleDecomposition'>"
  << this->descriptionModuleDecompositionOverSl2
  << "</div>";
  out
  << "<br><br>"
  << "<div class='containerTableSummarySlTwo'>"
  << "<table class='tableSummarySlTwo'>"
  << "<tr>"
  << "<th class='tableHeaderSummarySlTwoType'>Type + realization link</th>"
  << "<th class='tableHeaderSummarySlTwoDefault' style='padding-right:20px;'>"
  << "<a href='#"
  << idSpanHCharacteristicDescription
  << "'>h-Characteristic</a>"
  << "</th>";
  out
  << "<th class='tableHeaderSummarySlTwoDefault' "
  << "align='center' "
  << "style='white-space: nowrap;'"
  << "'>"
  << "<a href='#idCartanElementRealization'>Realization of h</a>"
  << "</th>"
  << "<th class='tableHeaderSummarySlTwoDefault' style='padding-left:20px;'>"
  << "<a href='#idModuleDecomposition'>"
  << "sl(2)-module decomposition of the ambient Lie algebra</a> <br> "
  << "\\(\\psi=\\) the fundamental \\(sl(2)\\)-weight. "
  << "</a></th>"
  << "<th class='tableHeaderSummarySlTwoDefault'>"
  << "Centralizer dimension</th>"
  << "<th class='tableHeaderSummarySlTwoDefault'>"
  << "Type of semisimple part of centralizer, if known</th>"
  << "<th class='tableHeaderSummarySlTwoDefault'>"
  << "The square of the length of the weight dual to h.</th>"
  << "<th class='tableHeaderSummarySlTwoDefault'>"
  << "Dynkin index </th>"
  << "<th class='tableHeaderSummarySlTwoDefault'>"
  << "Minimal containing regular semisimple SAs</th>"
  << "<th class='tableHeaderSummarySlTwoDefault'>"
  << "<a href='#idMinimalContainingRegularSA'>"
  << "Containing regular semisimple SAs in "
  << "which the sl(2) has no centralizer</a>"
  << "</th></tr>";
  FormatExpressions formatCharacter;
  formatCharacter.vectorSpaceEiBasisNames.addOnTop("\\psi");
  for (int i = 0; i < this->allSubalgebras.size; i ++) {
    const SlTwoSubalgebra& currentSubalgebra = this->allSubalgebras[i];
    out
    << "<tr>"
    << "<td class='tableSummarySlTwoType'>"
    << "<a href='#sl2index"
    << i
    << "'>\\(A^{"
    << currentSubalgebra.getDynkinIndex()
    << "}_1"
    << "\\)</a>"
    << "</td>";
    out << "<td class='tableSummarySlTwoDefault'>";
    out << "\\(" << currentSubalgebra.hCharacteristic.toString() << "\\)";
    out << "</td>";
    out
    << "<td class='tableSummarySlTwoDefault' style='white-space: nowrap;'>";
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
    out
    << "</td><td class='tableSummarySlTwoDefault' style='padding-left:20px;'>";
    out
    << HtmlRoutines::getMathNoDisplay((
        currentSubalgebra.moduleDecompositionAmbientSubalgebra.toString(
          &formatCharacter
        )
      )
    )
    << "\n<br>\n";
    out << "</td>";
    out
    << "<td class='tableSummarySlTwoDefault'>"
    << currentSubalgebra.dimensionCentralizer
    << "</td>";
    if (currentSubalgebra.centralizerComputer.flagTypeComputed) {
      out
      << "<td class='tableSummarySlTwoDefault'> "
      << HtmlRoutines::getMathNoDisplay(
        currentSubalgebra.centralizerComputer.typeIfKnown.toString()
      )
      << "</td>";
    } else {
      out << "<td class='tableSummarySlTwoDefault'> not computed</td>";
    }
    out << "<td class='tableSummarySlTwoDefault'>";
    out << currentSubalgebra.lengthHSquared;
    out << "</td>" << "<td class='tableSummarySlTwoDefault'>";
    out << currentSubalgebra.getDynkinIndex();
    out << "</td>" << "<td class='tableSummarySlTwoDefault'>";
    currentSubalgebra.checkIndicesMinimalContainingRootSubalgebras();
    out
    << currentSubalgebra.toStringMinimalContainingRootSubalgebras(
      displayPathAlgebra
    );
    out << "</td>" << "<td class='tableSummarySlTwoDefault'>";
    out
    << currentSubalgebra.toStringContainingRootSubalgebras(
      displayPathAlgebra
    );
    out << "</td></tr>\n";
  }
  out << "</table>" << "</div>" << "<hr>";
  return out.str();
}

std::string SlTwoSubalgebras::toString(FormatExpressions* format) {
  STACK_TRACE("SlTwoSubalgebras::toString");
  std::string currentString;
  std::stringstream out;
  std::stringstream body;
  bool useHtml = format == nullptr ? true : format->flagUseHTML;
  for (const SlTwoSubalgebra& current : this->allSubalgebras) {
    currentString = current.toString(format);
    body << currentString;
    if (useHtml) {
      body << "<hr>";
    }
  }
  if (useHtml) {
    out << "<br>";
  }
  out << this->toHTMLSummary(format);
  out << "<hr>";
  out << this->toHTMLFieldReport();
  out << "<hr>" << body.str();
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

CentralizerComputer::CentralizerComputer() {
  this->flagTypeComputed = false;
  this->flagBasisComputed = false;
  this->flagCartanSelected = false;
  this->owner = nullptr;
  this->algebraicClosureRationals = nullptr;
}

void CentralizerComputer::initialize(
  SemisimpleLieAlgebra* inputOwner,
  AlgebraicClosureRationals* inputAlgebraicClosure,
  Rational* inputDynkinIndexOfCentralizedComponent
) {
  this->owner = inputOwner;
  this->algebraicClosureRationals = inputAlgebraicClosure;
  this->semisimpleElementAdjointEigenvalueFinder.initialize(
    inputAlgebraicClosure
  );
  if (inputDynkinIndexOfCentralizedComponent == nullptr) {
    this->dynkinIndexOfCentralizedComponent = 0;
  } else {
    this->dynkinIndexOfCentralizedComponent =
    *inputDynkinIndexOfCentralizedComponent;
  }
}

std::string CentralizerComputer::toString() const {
  STACK_TRACE("CentralizerComputer::toString");
  std::stringstream out;
  out << "Centralizer type: ";
  if (this->flagTypeComputed) {
    out << this->typeIfKnown.toString();
    if (this->typeIfKnown.isEqualToZero()) {
      return out.str();
    }
  } else {
    out << " not computed";
  }
  if (this->owner == nullptr) {
    return out.str();
  }
  Rational killingSquare = this->owner->killingSquareOfDualOfAmbientLongRoot();
  out
  << "\n<br>\nKilling form square of "
  << "Cartan element dual to ambient long root: "
  << this->owner->killingSquareOfDualOfAmbientLongRoot().toString();
  out << "\n<br>\n" << "Basis of the centralizer";
  if (this->flagBasisComputed) {
    out
    << " (dimension: "
    << this->centralizerBasis.size
    << "): "
    << this->centralizerBasis.toStringCommaDelimited();
    out
    << "\n<br>\nBasis of centralizer intersected with cartan (dimension: "
    << this->centralizerIntersectedWithAmbientCartan.size
    << "): "
    << this->centralizerIntersectedWithAmbientCartan.toStringCommaDelimited();
    out
    << "\n<br>\nCartan of centralizer (dimension: "
    << this->centralizerCartan.size
    << "): "
    << this->centralizerCartan.toStringCommaDelimited();
  } else {
    out << ": not computed.";
  }
  out
  << "\n<br>\nCartan-generating semisimple element: "
  << this->semisimpleElement.toString();
  out
  << "\n<br>\nCharacteristic polynomial ad H: "
  << this->semisimpleElementAdjointEigenvalueFinder.characteristicPolynomial.
  toStringPretty();
  out
  << "\n<br>\nFactorization of characteristic polynomial of ad H: "
  << this->semisimpleElementAdjointEigenvalueFinder.
  factorizationMinimalPolynomial().toStringResult();
  out
  << "\n<br>\nEigenvalues of ad H: "
  << this->semisimpleElementAdjointEigenvalueFinder.
  eigenValuesWithoutMultiplicity.toStringCommaDelimited();
  out
  << "\n<br>\n"
  << this->semisimpleElementAdjointEigenvalueFinder.numberOfEigenVectors()
  << " eigenvectors of ad H: "
  << this->semisimpleElementAdjointEigenvalueFinder.eigenvectors.
  toStringCommaDelimited();
  out << "\n<br>\nCentralizer type: " << this->typeIfKnown.toString();
  out
  << "\n<br>\nReductive components ("
  << this->simpleComponents.size
  << " total):";
  out << this->simpleComponents.toStringWithSeparator("\n<br>\n");
  return out.str();
}

bool CentralizerComputer::compute() {
  this->owner->getCommonCentralizer(
    this->generatorsToCentralize, this->centralizerBasis
  );
  this->centralizerBasisOverAlgebraicNumbers = this->centralizerBasis;
  this->flagBasisComputed = true;
  if (this->centralizerBasis.size == 0) {
    return true;
  }
  this->flagCartanSelected = this->intersectAmbientCartanWithCentralizer();
  char ambientSimpleType = 'X';
  int rank = 1;
  this->owner->weylGroup.dynkinType.isSimple(&ambientSimpleType, &rank);
  if (this->centralizerIntersectedWithAmbientCartan.size > 0) {
    ElementSemisimpleLieAlgebra<Rational> semisimpleCandidate;
    // Construct an arbitrary linear combination of the elements of
    // the intersection of the ambient Cartan with the centralizer.
    // We want the arbitrary linear combination to have maximal rank
    // of its adjoint action.
    // This happens with probability 1 for a random linear combination.
    // However, when choosing small coefficients, there is a
    // high chance that the rank will actually not be maximal.
    // We are choosing choosing a linear combination with
    // an arbitary formula, whose only goal is to avoid
    // the chance of non-maximal rank of the adjoint action.
    for (
      int i = 0; i < this->centralizerIntersectedWithAmbientCartan.size; i ++
    ) {
      const ElementSemisimpleLieAlgebra<Rational>& summand =
      this->centralizerIntersectedWithAmbientCartan[i];
      Rational arbitraryCoefficient;
      arbitraryCoefficient =
      this->arbitraryCartanWeight(
        i, ambientSimpleType, rank, this->dynkinIndexOfCentralizedComponent
      );
      semisimpleCandidate += summand * arbitraryCoefficient;
    }
    if (this->trySemisimpleElement(semisimpleCandidate)) {
      return true;
    }
  }
  ElementSemisimpleLieAlgebra<Rational> semisimpleCandidate;
  for (int i = 0; i < this->centralizerBasis.size; i ++) {
    const ElementSemisimpleLieAlgebra<Rational>& summand =
    this->centralizerBasis[i];
    Rational coefficient =
    this->arbitraryCoefficientToFormSemisimpleElement(
      i,
      ambientSimpleType,
      rank,
      this->dynkinIndexOfCentralizedComponent,
      this->centralizerBasis.size
    );
    semisimpleCandidate += summand * coefficient;
  }
  if (this->trySemisimpleElement(semisimpleCandidate)) {
    return true;
  }
  return false;
}

void CentralizerComputer::getCentralizerElementFromCoordinates(
  const Vector<AlgebraicNumber>& vector,
  ElementSemisimpleLieAlgebra<AlgebraicNumber>& output
) {
  output.makeZero();
  for (int i = 0; i < vector.size; i ++) {
    const AlgebraicNumber& coordinate = vector[i];
    output.addOtherTimesConst(
      this->centralizerBasisOverAlgebraicNumbers[i], coordinate
    );
  }
}

bool CentralizerComputer::intersectAmbientCartanWithCentralizer() {
  STACK_TRACE("CentralizerComputer::intersectAmbientCartanWithCentralizer");
  List<ElementSemisimpleLieAlgebra<Rational> > ambientCartanBasis;
  for (int i = 0; i < this->owner->getRank(); i ++) {
    ElementSemisimpleLieAlgebra<Rational> h;
    h.makeCartanGeneratorHi(i, *this->owner);
    ambientCartanBasis.addOnTop(h);
  }
  ElementSemisimpleLieAlgebra<Rational>::intersectVectorSpaces(
    ambientCartanBasis,
    this->centralizerBasis,
    this->centralizerIntersectedWithAmbientCartan
  );
  return true;
}

bool CentralizerComputer::computeRootSpaces() {
  STACK_TRACE("CentralizerComputer::computeRootSpaces");
  HashedList<AlgebraicNumber> processedEigenValues;
  processedEigenValues.addOnTop(this->algebraicClosureRationals->zero());
  if (!this->computeCartanOfCentalizerFromSemisimpleElement()) {
    // The centralizer is not generic enough, and so the
    // elements that centralize the semisimple element
    // do not commute with one another.
    return false;
  }
  // Find all non-zero eigenspaces.
  for (
    const AlgebraicNumber& eigenValue :
    this->semisimpleElementAdjointEigenvalueFinder.
    eigenValuesWithoutMultiplicity
  ) {
    AlgebraicNumber minusEigenValue = eigenValue;
    minusEigenValue.negate();
    if (processedEigenValues.contains(minusEigenValue)) {
      // The negative of this eigenvalue was already processed.
      continue;
    }
    processedEigenValues.addOnTopNoRepetition(eigenValue);
    if (!this->computeRootSpaceForNonZeroEigenvalue(eigenValue)) {
      return false;
    }
  }
  return true;
}

bool CentralizerComputer::computeCartanOfCentalizerFromSemisimpleElement() {
  STACK_TRACE(
    "CentralizerComputer::computeCartanOfCentalizerFromSemisimpleElement"
  );
  List<ElementSemisimpleLieAlgebra<Rational> > centralizerOfSemisimpleElement;
  ElementSemisimpleLieAlgebra<Rational> semisimpleElementOverTheRationals;
  // Our starting semisimple element is supposed to be defined over the
  // rationals.
  // Convert back to an element over the rationals or throw:
  semisimpleElementOverTheRationals = this->semisimpleElement.h;
  this->owner->getCommonCentralizer(
    List<ElementSemisimpleLieAlgebra<Rational> >(
      {semisimpleElementOverTheRationals}
    ),
    centralizerOfSemisimpleElement
  );
  ElementSemisimpleLieAlgebra<Rational>::intersectVectorSpaces(
    centralizerOfSemisimpleElement,
    this->centralizerBasis,
    this->centralizerCartan
  );
  ElementSemisimpleLieAlgebra<Rational> mustBeZero;
  for (int i = 0; i < this->centralizerCartan.size; i ++) {
    for (int j = i + 1; j < this->centralizerCartan.size; j ++) {
      this->centralizerCartan[i].lieBracketOnTheRight(
        this->centralizerCartan[j], mustBeZero
      );
      if (!mustBeZero.isEqualToZero()) {
        // The elements centralizing the semisimple element do not
        // commute pairwise.
        // The original element was not semisimple or was not generic enough.
        return false;
      }
    }
  }
  // Reset the compute-on-demand centralizerCartanCandidates.
  this->centralizerCartanCandidates.clear();
  return true;
}

bool CentralizerComputer::computeOnDemandCartanOfCentralizerMatrixForm() {
  STACK_TRACE(
    "CentralizerComputer::computeOnDemandCartanOfCentralizerMatrixForm"
  );
  if (this->centralizerCartanCandidates.size > 0) {
    // Already computed.
    return true;
  }
  ElementSemisimpleLieAlgebra<Rational> zero;
  for (
    const ElementSemisimpleLieAlgebra<Rational>& splitter :
    this->centralizerCartan
  ) {
    CartanElementCandidate splitterCandidate;
    if (!this->makeCartanCandidate(splitter, zero, zero, splitterCandidate)) {
      return false;
    }
    this->centralizerCartanCandidates.addOnTop(splitterCandidate);
  }
  return true;
}

bool CentralizerComputer::computeRootSpaceForNonZeroEigenvalue(
  const AlgebraicNumber& eigenvalue
) {
  STACK_TRACE("CentralizerComputer::computeRootSpaceForNonZeroEigenvalue");
  Vectors<AlgebraicNumber> rootSpace;
  Vectors<AlgebraicNumber> negativeRootSpace;
  this->semisimpleElementAdjointEigenvalueFinder.eigenVectorsFromEigenvalue(
    eigenvalue, rootSpace
  );
  AlgebraicNumber minusEigenvalue = eigenvalue;
  minusEigenvalue.negate();
  this->semisimpleElementAdjointEigenvalueFinder.eigenVectorsFromEigenvalue(
    minusEigenvalue, negativeRootSpace
  );
  if (rootSpace.size == 1 && negativeRootSpace.size == 1) {
    return
    this->computeRootSpaceFromEAndF(
      eigenvalue, rootSpace[0], negativeRootSpace[0]
    );
  }
  // The root spaces are of higher dimension. We need to separate them further.
  // We will attempt to do so by subdividing the root spaces into eigenvectors
  // with respect to other elements of the centalizer.
  if (!this->computeOnDemandCartanOfCentralizerMatrixForm()) {
    return false;
  }
  return this->subsplitRootSpaces(eigenvalue, rootSpace, negativeRootSpace);
}

bool CentralizerComputer::subsplitRootSpaces(
  const AlgebraicNumber& startingEigenvalue,
  const List<Vector<AlgebraicNumber> >& rootSpace,
  const List<Vector<AlgebraicNumber> >& negativeRootSpace
) {
  STACK_TRACE("CentralizerComputer::subsplitRootSpaces");
  List<RootSpacePair> nonSplitPairs;
  List<RootSpacePair> fullySplitPairs;
  RootSpacePair onePair;
  onePair.negativeSpace = negativeRootSpace;
  onePair.positiveSpace = rootSpace;
  nonSplitPairs.addOnTop(onePair);
  while (nonSplitPairs.size > 0) {
    RootSpacePair currentPair = nonSplitPairs.popLastObject();
    if (this->subsplitRootSpacesOnce(currentPair, nonSplitPairs)) {
      continue;
    }
    if (
      currentPair.negativeSpace.size != 1 ||
      currentPair.positiveSpace.size != 1
    ) {
      // Could not properly split the current pair of root spaces.
      return false;
    }
    fullySplitPairs.addOnTop(currentPair);
  }
  for (const RootSpacePair& pair : fullySplitPairs) {
    if (
      !this->computeRootSpaceFromEAndF(
        startingEigenvalue, pair.positiveSpace[0], pair.negativeSpace[0]
      )
    ) {
      return false;
    }
  }
  return true;
}

bool CentralizerComputer::subsplitRootSpacesOnce(
  const RootSpacePair& rootSpacePair, List<RootSpacePair>& output
) {
  STACK_TRACE("CentralizerComputer::subsplitRootSpacesOnce");
  if (
    rootSpacePair.negativeSpace.size == 1 &&
    rootSpacePair.positiveSpace.size == 1
  ) {
    return false;
  }
  for (
    const CartanElementCandidate& elementCentralizerCartan :
    this->centralizerCartanCandidates
  ) {
    if (
      this->subsplitRootSpacesOnceAgainstCartanElement(
        rootSpacePair, elementCentralizerCartan, output
      )
    ) {
      return true;
    }
  }
  return false;
}

bool CentralizerComputer::subsplitRootSpacesOnceAgainstCartanElement(
  const RootSpacePair& rootSpacePair,
  const CartanElementCandidate& splittingElementCentralizerCartan,
  List<RootSpacePair>& output
) {
  STACK_TRACE(
    "CentralizerComputer::subsplitRootSpacesOnceAgainstCartanElement"
  );
  if (rootSpacePair.negativeSpace.size == 1) {
    // Already fully split.
    return false;
  }
  Matrix<AlgebraicNumber> matrixPositive;
  Matrix<AlgebraicNumber> matrixNegative;
  if (
    !splittingElementCentralizerCartan.adjointAction.changeBasis(
      rootSpacePair.positiveSpace, matrixPositive, nullptr
    ) ||
    !splittingElementCentralizerCartan.adjointAction.changeBasis(
      rootSpacePair.negativeSpace, matrixNegative, nullptr
    )
  ) {
    return false;
  }
  MatrixEigenvalueFinder positiveRootSplitter;
  MatrixEigenvalueFinder negativeRootSplitter;
  positiveRootSplitter.initialize(this->algebraicClosureRationals);
  negativeRootSplitter.initialize(this->algebraicClosureRationals);
  if (
    !positiveRootSplitter.findEigenValuesAndEigenspaces(
      matrixPositive, nullptr
    ) ||
    !negativeRootSplitter.findEigenValuesAndEigenspaces(
      matrixNegative, nullptr
    )
  ) {
    return false;
  }
  List<RootSpacePair> newPairs;
  for (
    const AlgebraicNumber& eigenvalue :
    positiveRootSplitter.eigenValuesWithoutMultiplicity
  ) {
    AlgebraicNumber minusEigenvalue = eigenvalue;
    minusEigenvalue.negate();
    if (
      !negativeRootSplitter.eigenValuesWithoutMultiplicity.contains(
        minusEigenvalue
      )
    ) {
      return false;
    }
    Vectors<AlgebraicNumber> subsplitPositiveRootsRestricted;
    Vectors<AlgebraicNumber> subsplitNegativeRootsRestricted;
    positiveRootSplitter.eigenVectorsFromEigenvalue(
      eigenvalue, subsplitPositiveRootsRestricted
    );
    negativeRootSplitter.eigenVectorsFromEigenvalue(
      minusEigenvalue, subsplitNegativeRootsRestricted
    );
    if (
      subsplitNegativeRootsRestricted.size !=
      subsplitPositiveRootsRestricted.size
    ) {
      return false;
    }
    if (
      subsplitPositiveRootsRestricted.size == rootSpacePair.positiveSpace.size
    ) {
      return false;
    }
    RootSpacePair newPair =
    rootSpacePair.constructFromLinearCombinations(
      subsplitPositiveRootsRestricted, subsplitNegativeRootsRestricted
    );
    newPairs.addOnTop(newPair);
  }
  output.addListOnTop(newPairs);
  return true;
}

RootSpacePair RootSpacePair::constructFromLinearCombinations(
  const List<Vector<AlgebraicNumber> >& positiveCoordinates,
  const List<Vector<AlgebraicNumber> >& negativeCoordinates
) const {
  STACK_TRACE("RootSpacePair::constructFromLinearCombinations");
  RootSpacePair result;
  for (int i = 0; i < positiveCoordinates.size; i ++) {
    this->appendLinearCombinations(
      positiveCoordinates[i], negativeCoordinates[i], result
    );
  }
  return result;
}

void RootSpacePair::appendLinearCombinations(
  const Vector<AlgebraicNumber>& positiveCoordinates,
  const Vector<AlgebraicNumber>& negativeCoordinates,
  RootSpacePair& output
) const {
  Vector<AlgebraicNumber> positiveRoot;
  Vector<AlgebraicNumber> negativeRoot;
  int dimension = this->positiveSpace[0].size;
  positiveRoot.makeZero(dimension);
  negativeRoot.makeZero(dimension);
  for (int j = 0; j < positiveCoordinates.size; j ++) {
    positiveRoot.addOtherTimesScalar(
      this->positiveSpace[j], positiveCoordinates[j]
    );
    negativeRoot.addOtherTimesScalar(
      this->negativeSpace[j], negativeCoordinates[j]
    );
  }
  output.positiveSpace.addOnTop(positiveRoot);
  output.negativeSpace.addOnTop(negativeRoot);
}

bool CentralizerComputer::computeRootSpaceFromEAndF(
  const AlgebraicNumber& eigenvalue,
  const Vector<AlgebraicNumber>& eInCentralizerCoordinates,
  const Vector<AlgebraicNumber>& fInCentralizerCoordinates
) {
  STACK_TRACE("CentralizerComputer::computeRootSpaceFromEAndF");
  int centralizerDimension = this->centralizerBasis.size;
  if (
    eInCentralizerCoordinates.size != centralizerDimension ||
    fInCentralizerCoordinates.size != centralizerDimension
  ) {
    global.fatal
    << "Bad coordinates: the centralizer dimension is: "
    << centralizerDimension
    << " but the coordinates given are: "
    << eInCentralizerCoordinates.toString()
    << " and "
    << fInCentralizerCoordinates.toString()
    << global.fatal;
  }
  ElementSemisimpleLieAlgebra<AlgebraicNumber> e;
  ElementSemisimpleLieAlgebra<AlgebraicNumber> f;
  this->getCentralizerElementFromCoordinates(eInCentralizerCoordinates, e);
  this->getCentralizerElementFromCoordinates(fInCentralizerCoordinates, f);
  ElementSemisimpleLieAlgebra<AlgebraicNumber> h;
  e.lieBracketOnTheRight(f, h);
  ElementSemisimpleLieAlgebra<AlgebraicNumber> proportionalToE;
  h.lieBracketOnTheRight(e, proportionalToE);
  AlgebraicNumber coefficientOrProportionality;
  bool mustBeTrue =
  proportionalToE.isProportionalTo(e, coefficientOrProportionality);
  if (!mustBeTrue) {
    global.fatal
    << "In type: "
    << this->label
    << ". Corrupt h element: "
    << h.toStringPretty()
    << " constructed as h=[e,f] where \\(e="
    << e.toStringPretty()
    << "\\) and \\(f="
    << f.toStringPretty()
    << "\\). "
    << "We have that \\([h,e]="
    << proportionalToE.toStringPretty()
    << "\\) which is not proportional to e "
    << "and eigenvalue equal to: "
    << eigenvalue.toString()
    << global.fatal;
  }
  h *= coefficientOrProportionality * 2;
  CartanElementCandidate hWithAction;
  this->makeCartanCandidate(h, e, f, hWithAction);
  this->dualsOfRootSpaces.addOnTop(hWithAction);
  h *= - 1;
  this->makeCartanCandidate(h, f, e, hWithAction);
  this->dualsOfRootSpaces.addOnTop(hWithAction);
  return true;
}

bool CentralizerComputer::trySemisimpleElement(
  ElementSemisimpleLieAlgebra<Rational>& candidate
) {
  STACK_TRACE("CentralizerComputer::trySemisimpleElement");
  ElementSemisimpleLieAlgebra<Rational> zero;
  this->makeCartanCandidateNoFailure(
    candidate, zero, zero, this->semisimpleElement
  );
  this->semisimpleElementAdjointEigenvalueFinder.initialize(
    this->algebraicClosureRationals
  );
  if (
    !this->semisimpleElementAdjointEigenvalueFinder.
    findEigenValuesAndEigenspaces(
      this->semisimpleElement.adjointAction, nullptr
    )
  ) {
    return false;
  }
  if (
    this->semisimpleElementAdjointEigenvalueFinder.numberOfEigenVectors() !=
    this->centralizerBasis.size
  ) {
    // The element has fewer eigenvectors than
    // the dimension of the space,
    // i.e., it has non-trivial Jordan normal form.
    // i.e., the candidate element is not semisimple.
    return false;
  }
  if (!this->computeRootSpaces()) {
    return false;
  }
  if (!this->computeTypes()) {
    return false;
  }
  if (
    this->typeIfKnown.getLieAlgebraDimension() > this->centralizerBasis.size
  ) {
    global.fatal
    << "Programming error: the dimension of the allegedly "
    << "computed type of the centralizer "
    << "is larger than the has dimension of the actual centralizer."
    << global.fatal;
  }
  return true;
}

bool CentralizerComputer::makeCartanCandidate(
  const ElementSemisimpleLieAlgebra<Rational>& inputH,
  const ElementSemisimpleLieAlgebra<Rational>& inputE,
  const ElementSemisimpleLieAlgebra<Rational>& inputF,
  CartanElementCandidate& output
) {
  ElementSemisimpleLieAlgebra<AlgebraicNumber> converterH;
  ElementSemisimpleLieAlgebra<AlgebraicNumber> converterE;
  ElementSemisimpleLieAlgebra<AlgebraicNumber> converterF;
  converterH = inputH;
  converterE = inputE;
  converterF = inputF;
  return this->makeCartanCandidate(converterH, converterE, converterF, output);
}

bool CentralizerComputer::makeCartanCandidate(
  const ElementSemisimpleLieAlgebra<AlgebraicNumber>& inputH,
  const ElementSemisimpleLieAlgebra<AlgebraicNumber>& inputE,
  const ElementSemisimpleLieAlgebra<AlgebraicNumber>& inputF,
  CartanElementCandidate& output
) {
  STACK_TRACE("CentralizerComputer::makeCartanCandidate");
  output.h = inputH;
  bool result =
  output.h.computeAdjointActionWithRespectToBasis(
    this->centralizerBasisOverAlgebraicNumbers, output.adjointAction
  );
  output.e = inputE;
  output.f = inputF;
  return result;
}

void CentralizerComputer::makeCartanCandidateNoFailure(
  const ElementSemisimpleLieAlgebra<Rational>& inputH,
  const ElementSemisimpleLieAlgebra<Rational>& inputE,
  const ElementSemisimpleLieAlgebra<Rational>& inputF,
  CartanElementCandidate& output
) {
  STACK_TRACE("CentralizerComputer::makeCartanCandidateNoFailure");
  bool mustBeTrue = this->makeCartanCandidate(inputH, inputE, inputF, output);
  if (!mustBeTrue) {
    global.fatal
    << "Failed to compute the adjoint action of "
    << inputH.toString()
    << " in basis: "
    << this->centralizerBasis.toStringCommaDelimited()
    << global.fatal;
  }
}

void CentralizerComputer::mergeReductiveComponents(
  List<int>& indicesOfComponentsToBeMergedSorted
) {
  STACK_TRACE("CentralizerComputer::mergeReductiveComponents");
  if (indicesOfComponentsToBeMergedSorted.size == 0) {
    // Nothing to merge.
    return;
  }
  int smallestIndex = indicesOfComponentsToBeMergedSorted[0];
  for (int i = 1; i < indicesOfComponentsToBeMergedSorted.size; i ++) {
    int indexOfDissolved = indicesOfComponentsToBeMergedSorted[i];
    const SimpleSubalgebraComponent& toBeDissolved =
    this->simpleComponents[indexOfDissolved];
    // The present loop modifies the reductiveComponents array,
    // so to reduce the danger of use-after-free,
    // it is safest to take a reference of the subalgebra element
    // on each run.
    SimpleSubalgebraComponent& simpleSubalgebraComponent =
    this->simpleComponents[smallestIndex];
    for (const CartanElementCandidate& element : toBeDissolved.dualsToRoots) {
      simpleSubalgebraComponent.dualsToRoots.addOnTop(element);
    }
    // We are modifying the reductive components array which
    // we took a reference from earlier in the loop.
    // Since we took the reference recently, so
    // there is no danger of use-after-free.
    this->simpleComponents.removeIndexSwapWithLast(indexOfDissolved);
  }
}

void CentralizerComputer::computeReductiveComponentsBases() {
  STACK_TRACE("CentralizerComputer::computeReductiveComponentsBases");
  for (const CartanElementCandidate& candidate : this->dualsOfRootSpaces) {
    this->mergeOneRootSpaceCandidate(candidate);
  }
}

void CentralizerComputer::mergeOneRootSpaceCandidate(
  const CartanElementCandidate& candidate
) {
  STACK_TRACE("CentralizerComputer::mergeOneRootSpaceCandidate");
  List<int> indicesLinkedToCandidate;
  for (int i = 0; i < this->simpleComponents.size; i ++) {
    if (this->simpleComponents[i].isLinkedTo(candidate)) {
      indicesLinkedToCandidate.addOnTop(i);
    }
  }
  this->mergeReductiveComponents(indicesLinkedToCandidate);
  if (indicesLinkedToCandidate.size == 0) {
    SimpleSubalgebraComponent semisimpleComponent;
    semisimpleComponent.initialize(
      this->owner,
      this->algebraicClosureRationals,
      this->semisimpleElement,
      this
    );
    std::stringstream out;
    out << "S(" << this->label << ")";
    semisimpleComponent.label = out.str();
    semisimpleComponent.dualsToRoots.addOnTop(candidate);
    this->simpleComponents.addOnTop(semisimpleComponent);
    return;
  }
  SimpleSubalgebraComponent& container =
  this->simpleComponents[indicesLinkedToCandidate[0]];
  container.dualsToRoots.addOnTop(candidate);
}

bool CentralizerComputer::computeTypes() {
  STACK_TRACE("CentralizerComputer::computeTypes");
  this->computeReductiveComponentsBases();
  this->typeIfKnown.makeZero();
  for (SimpleSubalgebraComponent& simpleComponent : this->simpleComponents) {
    if (!simpleComponent.compute()) {
      return false;
    }
    this->typeIfKnown += simpleComponent.simpleType;
  }
  this->flagTypeComputed = true;
  return true;
}

bool PolynomialQuadraticRootFinder::hasRationalPolynomial(
  Polynomial<Rational>& whichPolynomial
) {
  whichPolynomial.makeZero();
  for (int i = 0; i < this->polynomial.size(); i ++) {
    Rational rationalCoefficient;
    if (!this->polynomial.coefficients[i].isRational(&rationalCoefficient)) {
      return false;
    }
    whichPolynomial.addMonomial(
      this->polynomial.monomials[i], rationalCoefficient
    );
  }
  return true;
}

bool PolynomialQuadraticRootFinder::factorRationalInput(
  const Polynomial<Rational>& input
) {
  PolynomialFactorizationFiniteFields algorithm;
  PolynomialFactorizationUnivariate<Rational> rationalFactorization;
  bool factorizationSuccess =
  rationalFactorization.factor(input, algorithm, nullptr, nullptr);
  if (!factorizationSuccess) {
    return false;
  }
  // The following assignments convert from rational numbers
  // to algebraic numbers.
  this->factorization.clear();
  this->factorization.original = input;
  this->factorization.constantFactor = rationalFactorization.constantFactor;
  this->factorization.reduced = rationalFactorization.reduced;
  this->factorization.computations = rationalFactorization.computations;
  return true;
}

bool PolynomialQuadraticRootFinder::factorInput() {
  STACK_TRACE("PolynomialQuadraticRootFinder::factorInput");
  Polynomial<Rational> polynomialRational;
  if (this->hasRationalPolynomial(polynomialRational)) {
    return this->factorRationalInput(polynomialRational);
  }
  this->factorization.clear();
  this->factorization.original = this->polynomial;
  this->factorization.constantFactor = 1;
  if (this->polynomial.constantTerm().isEqualToZero()) {
    this->rootHints.addOnTop(0);
  }
  Polynomial<AlgebraicNumber> quotient = this->polynomial;
  for (const AlgebraicNumber& hint : this->rootHints) {
    while (true) {
      Polynomial<AlgebraicNumber> divisor;
      Polynomial<AlgebraicNumber> maybeNextQuotient;
      Polynomial<AlgebraicNumber> remainder;
      divisor.makeMonomial(0, 1, 1);
      divisor -= hint;
      quotient.divideBy(divisor, maybeNextQuotient, remainder, nullptr);
      if (remainder.isEqualToZero()) {
        // We found a factorization.
        this->factorization.reduced.addOnTop(divisor);
        quotient = maybeNextQuotient;
      } else {
        break;
      }
    }
  }
  AlgebraicNumber remainingConstant;
  if (quotient.isConstant(&remainingConstant)) {
    // We fully factored the polynomial into linear terms.
    this->factorization.constantFactor = remainingConstant;
    return true;
  }
  this->factorization.reduced.addOnTop(quotient);
  if (quotient.totalDegree() <= 2) {
    // We factored the polynomial into a product of linear terms and
    // a quadratic term.
    // Assuming we don't run into computuational issues while taking
    // the square root of the discriminant
    // - the discriminant need not be rational -
    // we will additionally factor the quadratic term later using the
    // quadratic formula.
    return true;
  }
  // The remaining quotient is of degree 3 or above.
  // For now, give up.
  // In the future, we could assume that is the minimial polynomial of
  // an element and attempt to extend with it the base field over
  // which we are computing.
  return false;
}

bool PolynomialQuadraticRootFinder::findRoots(
  Polynomial<AlgebraicNumber>& input
) {
  STACK_TRACE("PolynomialQuadraticRootFinder::findRoots");
  this->roots.clear();
  this->polynomial = input;
  if (!this->factorInput()) {
    return false;
  }
  if (this->factorization.nonReduced.size > 0) {
    // We failed to fully factor the characteristic polynomial.
    return false;
  }
  if (!this->factorization.hasLinearAndQuadraticFactorsOnly()) {
    // The factors of the characteristic polynomial are not quadratic.
    // Using this requires non-quadratic extensions of the rationals,
    // let's give up for now.
    // We haven't observed this happening in practice.
    // Is there a mathematical reason (i.e., a theorem) for this?
    return false;
  }
  for (
    const Polynomial<AlgebraicNumber>& factor : this->factorization.reduced
  ) {
    if (factor.totalDegree() == 1) {
      if (!this->addRootsOfLinearFactor(factor)) {
        return false;
      }
      continue;
    }
    if (factor.totalDegree() == 2) {
      if (!this->addRootsOfQuadraticFactor(factor)) {
        return false;
      }
      continue;
    }
    global.fatal
    << "In polynomial root finder: this piece of code should be unreachable! "
    << global.fatal;
  }
  return true;
}

bool PolynomialQuadraticRootFinder::addRootsOfLinearFactor(
  const Polynomial<AlgebraicNumber>& factor
) {
  STACK_TRACE("PolynomialQuadraticRootFinder::addRootsOfLinearFactor");
  AlgebraicNumber rootOfLinearFactor;
  rootOfLinearFactor.owner = this->algebraicClosure;
  rootOfLinearFactor = - factor.coefficientOfXZeroPowerK(0) /
  factor.coefficientOfXZeroPowerK(1);
  this->roots.addOnTop(rootOfLinearFactor);
  return true;
}

bool PolynomialQuadraticRootFinder::addRootsOfQuadraticFactor(
  const Polynomial<AlgebraicNumber>& factor
) {
  STACK_TRACE("PolynomialQuadraticRootFinder::addRootsOfQuadraticFactor");
  Polynomial<AlgebraicNumber> rescaled = factor;
  rescaled.scaleNormalizeLeadingMonomial(nullptr);
  AlgebraicNumber a = rescaled.coefficientOfXZeroPowerK(2);
  AlgebraicNumber b = rescaled.coefficientOfXZeroPowerK(1);
  AlgebraicNumber c = rescaled.coefficientOfXZeroPowerK(0);
  AlgebraicNumber discriminant = b * b - a * c * 4;
  AlgebraicNumber squareRootOfDiscriminant = discriminant;
  if (squareRootOfDiscriminant.owner == nullptr) {
    squareRootOfDiscriminant.owner = this->algebraicClosure;
  }
  std::stringstream commentsOnFailure;
  if (!squareRootOfDiscriminant.radicalMeDefault(2, &commentsOnFailure)) {
    // We failed to take the square root of the rational;
    // possibly due to a computational throttle.
    return false;
  }
  AlgebraicNumber root1 = (squareRootOfDiscriminant - b) / (a* 2);
  AlgebraicNumber root2 = (squareRootOfDiscriminant *(- 1) - b) / (a* 2);
  this->roots.addOnTop(root1);
  this->roots.addOnTop(root2);
  return true;
}

void PolynomialQuadraticRootFinder::initialize(
  AlgebraicClosureRationals* inputAlgebraicClosure
) {
  this->algebraicClosure = inputAlgebraicClosure;
  this->roots.clear();
  this->polynomial.makeZero();
  this->factorization.clear();
}

std::string PolynomialQuadraticRootFinder::toString(FormatExpressions* format)
const {
  (void) format;
  std::stringstream out;
  out
  << "\n<br>\nPolynomial: "
  << this->polynomial.toString()
  << "\n<br>\nFactorization: "
  << this->factorization.toStringResult(format)
  << "\n<br>\nRoots of polynomial: "
  << this->roots.toStringCommaDelimited();
  return out.str();
}

PolynomialQuadraticRootFinder::PolynomialQuadraticRootFinder() {
  this->algebraicClosure = nullptr;
}

AlgebraicNumber CartanElementCandidate::scalarProductKilling(
  const CartanElementCandidate& other
) const {
  Matrix<AlgebraicNumber> matrix;
  matrix = this->adjointAction;
  matrix.multiplyOnTheRight(other.adjointAction);
  return matrix.trace();
}

Rational CartanElementCandidate::scalarProductKillingMustBeRational(
  const CartanElementCandidate& other
) const {
  Matrix<AlgebraicNumber> matrix;
  matrix = this->adjointAction;
  matrix.multiplyOnTheRight(other.adjointAction);
  AlgebraicNumber result = matrix.trace();
  Rational resultRational;
  if (!result.isRational(&resultRational)) {
    global.fatal
    << "The killing scalar product is not rational "
    << global.fatal;
  }
  return resultRational;
}

std::string CartanElementCandidate::toString() const {
  std::stringstream out;
  out << "\\(" << this->h.toStringPretty() << "\\), ";
  if (!this->e.isEqualToZero()) {
    out
    << "matching e: \\("
    << this->e.toStringPretty()
    << "\\), verification: \\( [h,e]-2e=";
    ElementSemisimpleLieAlgebra<AlgebraicNumber> computer;
    this->h.lieBracketOnTheRight(this->e, computer);
    computer -= this->e * 2;
    out << computer.toString() << "\\), ";
  }
  out
  << "adjoint action: "
  << "\\("
  << this->adjointAction.toStringLatex()
  << "\\)";
  return out.str();
}

SimpleSubalgebraComponent::SimpleSubalgebraComponent() {
  this->owner = nullptr;
}

bool SimpleSubalgebraComponent::isLinkedTo(
  const CartanElementCandidate& candidate
) const {
  STACK_TRACE("SimpleSubalgebraComponent::isLinkedTo");
  for (const CartanElementCandidate& myRootSpace : this->dualsToRoots) {
    if (!myRootSpace.scalarProductKilling(candidate).isEqualToZero()) {
      return true;
    }
  }
  return false;
}

std::string SimpleSubalgebraComponent::toString() const {
  std::stringstream out;
  out
  << "\n<br>\nScalar product computed: \\("
  << this->dynkinDiagramComputer.ambientBilinearForm.toStringLatex()
  << "\\)";
  out
  << "\n<br>\nSimple basis of Cartan of centralizer ("
  << this->simpleDualsOfRootSpaces.size
  << " total):<br>"
  << this->simpleDualsOfRootSpaces.toStringWithSeparator("\n<br>\n");
  out
  << "\n<br>\nLinear space basis of intersection "
  << "of centralizer and ambient Cartan:<br>\n"
  << this->simpleDualsOfRootSpaces.toStringWithSeparator("\n<br>\n");
  out
  << "\n<br>\nElements in Cartan dual to root system: "
  << this->dualsToRootsAlgebraic.toStringCommaDelimited();
  out
  << "\n<br>\nCo-symmetric Cartan Matrix of centralizer, "
  << "scaled by ambient killing form: "
  << "\\("
  << this->coSymmetricCartanMatrixCentralizerAmbientKilling.toStringLatex()
  << "\\)";
  return out.str();
}

bool SimpleSubalgebraComponent::compute() {
  STACK_TRACE("SimpleSubalgebraComponent::compute");
  this->positiveDualsOfRootSpaces.clear();
  this->positiveDualsOfRootSpaces.addListOnTop(this->dualsToRoots);
  AlgebraicNumber scalarProduct;
  Complex<double> scalarProductComplex;
  for (int i = 0; i < this->positiveDualsOfRootSpaces.size; i ++) {
    scalarProduct =
    this->positiveDualsOfRootSpaces[i].scalarProductKilling(
      this->definingSemisimpleElement
    );
    if (!scalarProduct.evaluatesToComplex(&scalarProductComplex)) {
      return false;
    }
    if (
      scalarProductComplex.realPart < 0 || (
        scalarProductComplex.realPart == 0 &&
        scalarProductComplex.imaginaryPart < 0
      )
    ) {
      // Discard the elements of the cartan whose scalar product
      // with the defining semisimple element is in the second or
      // third quadrant.
      this->positiveDualsOfRootSpaces.removeIndexSwapWithLast(i);
      i --;
    }
  }
  if (this->positiveDualsOfRootSpaces.size * 2 != this->dualsToRoots.size) {
    global.fatal
    << "Exactly half of the roots must be positive. "
    << global.fatal;
  }
  this->simpleDualsOfRootSpaces.clear();
  this->simpleDualsOfRootSpaces.addListOnTop(this->positiveDualsOfRootSpaces);
  for (int i = 0; i < this->simpleDualsOfRootSpaces.size; i ++) {
    if (!this->isSimpleIndex(i)) {
      this->simpleDualsOfRootSpaces.removeIndexSwapWithLast(i);
      i --;
    }
  }
  this->simpleDualsOfRootSpacesAmbientAdjoint.clear();
  for (
    const CartanElementCandidate& candidate : this->simpleDualsOfRootSpaces
  ) {
    CartanElementCandidate candidateWithAmbientAdjoint = candidate;
    this->owner->getAdjoint(
      candidateWithAmbientAdjoint.adjointAction, candidateWithAmbientAdjoint.h
    );
    this->simpleDualsOfRootSpacesAmbientAdjoint.addOnTop(
      candidateWithAmbientAdjoint
    );
  }
  this->coSymmetricCartanMatrixCentralizerAmbientKilling.resize(
    this->simpleDualsOfRootSpaces.size,
    this->simpleDualsOfRootSpaces.size,
    false
  );
  this->killingSquareProductShortestDual = 0;
  for (int i = 0; i < this->simpleDualsOfRootSpacesAmbientAdjoint.size; i ++) {
    for (
      int j = 0; j < this->simpleDualsOfRootSpacesAmbientAdjoint.size; j ++
    ) {
      this->coSymmetricCartanMatrixCentralizerAmbientKilling(i, j) =
      this->computeSimpleRootScalarProductAmbientKilling(i, j);
    }
    Rational currentSquareScalarProduct =
    this->coSymmetricCartanMatrixCentralizerAmbientKilling(i, i);
    if (i == 0) {
      this->killingSquareProductShortestDual = currentSquareScalarProduct;
    }
    if (this->killingSquareProductShortestDual > currentSquareScalarProduct) {
      this->killingSquareProductShortestDual = currentSquareScalarProduct;
    }
  }
  this->dynkinDiagramComputer.ambientBilinearForm.resize(
    this->simpleDualsOfRootSpaces.size,
    this->simpleDualsOfRootSpaces.size,
    false
  );
  for (int i = 0; i < this->simpleDualsOfRootSpaces.size; i ++) {
    for (int j = 0; j < this->simpleDualsOfRootSpaces.size; j ++) {
      this->dynkinDiagramComputer.ambientBilinearForm(i, j) =
      this->coSymmetricCartanMatrixCentralizerAmbientKilling(i, j) * 4 /
      this->coSymmetricCartanMatrixCentralizerAmbientKilling(j, j) /
      this->coSymmetricCartanMatrixCentralizerAmbientKilling(i, i);
    }
  }
  Vectors<Rational> simpleBasis;
  simpleBasis.makeEiBasis(this->simpleDualsOfRootSpaces.size);
  this->dynkinDiagramComputer.computeDiagramInputIsSimpleBasis(simpleBasis);
  if (this->owner->killingSquareOfDualOfAmbientLongRoot().isEqualToZero()) {
    // The owner is not simple, so we can't get a long root.
    // We could attempt to make a Dynkin index definition
    // in such a situation, but for now, let us simply give up.
    return false;
  }
  if (this->dynkinDiagramComputer.simpleComponentTypes.size != 1) {
    global.fatal
    << "The dynkin diagram computer should hold a single simple component."
    << global.fatal;
  }
  const DynkinSimpleType& simpleType =
  this->dynkinDiagramComputer.simpleComponentTypes[0];
  Rational longRootRatio = this->killingSquareProductShortestDual /
  this->owner->killingSquareOfDualOfAmbientLongRoot();
  Rational scale = longRootRatio * simpleType.getLongRootLengthSquared() / 2;
  this->dynkinDiagramComputer.getDynkinType(this->simpleType);
  this->simpleType.scaleFirstCoRootSquaredLength(scale);
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > simpleHs;
  for (const CartanElementCandidate& simpleH : this->simpleDualsOfRootSpaces) {
    simpleHs.addOnTop(simpleH.h);
  }
  this->dualsToRootsAlgebraic.clear();
  for (const CartanElementCandidate& dualRootSpace : this->dualsToRoots) {
    Vector<AlgebraicNumber> coordinateVector;
    bool mustBeTrue =
    dualRootSpace.h.coordinatesInBasis(simpleHs, coordinateVector);
    if (!mustBeTrue) {
      global.fatal
      << "Unexpected failure: the simple duals don't span the duals. "
      << global.fatal;
    }
    this->dualsToRootsAlgebraic.addOnTop(coordinateVector);
  }
  return true;
}

void SimpleSubalgebraComponent::initialize(
  SemisimpleLieAlgebra* inputOwner,
  AlgebraicClosureRationals* inputAlgebraicClosure,
  const CartanElementCandidate& inputDefiningSemisimpleElement,
  CentralizerComputer* inputOptionalContainer
) {
  this->owner = inputOwner;
  this->algebraicClosure = inputAlgebraicClosure;
  this->definingSemisimpleElement = inputDefiningSemisimpleElement;
  this->optionalContainer = inputOptionalContainer;
}

Rational SimpleSubalgebraComponent::
computeSimpleRootScalarProductAmbientKilling(int i, int j) const {
  return
  this->simpleDualsOfRootSpacesAmbientAdjoint[i].
  scalarProductKillingMustBeRational(
    this->simpleDualsOfRootSpacesAmbientAdjoint[j]
  );
}

bool SimpleSubalgebraComponent::isSimpleIndex(int i) {
  const CartanElementCandidate& underTest = this->simpleDualsOfRootSpaces[i];
  CartanElementCandidate difference;
  for (
    const CartanElementCandidate& positiveRootSpace :
    this->positiveDualsOfRootSpaces
  ) {
    difference = underTest - positiveRootSpace;
    if (this->positiveDualsOfRootSpaces.contains(difference)) {
      return false;
    }
  }
  return true;
}
