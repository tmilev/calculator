#include "general_file_operations_encodings.h"
#include "math_extra_finite_groups_implementation.h"
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
  << "coefficients are chosen to be the increasing odd "
  << "numbers 1, 3, 5, ....<br>"
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
  STACK_TRACE("SlTwoSubalgebra::fromSlTwoSubalgebraCandidate");
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
  STACK_TRACE("SlTwoSubalgebra::initializeUnknownTriples");
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
  //
  // Zero e and f for the arbitrary f choice optimization.
  this->eArbitraryUnknown.makeZero();
  // In the code below, f will be chosen arbitrarily.
  this->fArbitrary.makeZero();
  //
  // Zero e and f without arbitrary optimizations.
  this->eUnknown.makeZero();
  this->fUnknown.makeZero();
  //
  // Zero e  and f for the Kostant-Sekiguchi computation.
  this->eKostantSekiguchiUnknown.makeZero();
  this->fKostantSekiguchiUnknown.makeZero();
  for (int i = 0; i < this->participatingPositiveRoots.size; i ++) {
    // Initialize arbitrary triple
    ChevalleyGenerator negative;
    negative.makeGeneratorRootSpace(
      this->getOwnerSemisimpleAlgebra(),
      - this->participatingPositiveRoots[i]
    );
    this->fArbitrary.addMonomial(negative, i * i + 1);
    // (i % 2== 0)? 1: 2;
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
  AlgebraicClosureRationals* inputAlgebraicClosure
) {
  STACK_TRACE(
    "SlTwoSubalgebra::attemptExtendingHFtoHEFWithRespectToSubalgebra"
  );
  if (
    zeroCharacteristics.cardinalitySelection ==
    zeroCharacteristics.numberOfElements
  ) {
    return false;
  }
  this->algebraicClosure = inputAlgebraicClosure;
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
  // where
  // l/2 is the index of the rootattemptExtendingHFtoHEFWithRespectToSubalgebra
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
  STACK_TRACE("SlTwoSubalgebra::initializeHEFSystemFromFCoefficients");
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
  STACK_TRACE("SlTwoSubalgebra::initializeHEFSystemFromECoefficientsPartTwo");
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
  STACK_TRACE("SlTwoSubalgebra::attemptRealizingKostantSekiguchi");
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
  STACK_TRACE("SlTwoSubalgebra::checkConsistencyParticipatingRoots");
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
    "SlTwoSubalgebraCandidate::adjoinKostantSekiguchiRelationsToPolynomialSystem"
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
toStringModuleDecompositionMinimalContainingRegularSAs(
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

std::string SlTwoSubalgebras::toHTMLSummary(FormatExpressions* format) {
  STACK_TRACE("SlTwoSubalgebras::toStringSummary");
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
  if (this->unsuitableHs.size > 0) {
    bool allbadAreGoodInTheirBadness = true;
    for (int i = 0; i < this->unsuitableHs.size; i ++) {
      bool isGoodInItsbadness = false;
      for (const SlTwoSubalgebra& current : this->allSubalgebras) {
        if (this->unsuitableHs[i] == current.hElement.getCartanPart()) {
          isGoodInItsbadness = true;
          break;
        }
      }
      if (!isGoodInItsbadness) {
        allbadAreGoodInTheirBadness = false;
        out
        << "<br><b>Bad characteristic for element h: "
        << this->unsuitableHs[i]
        << "</b>";
      } else {
        out
        << "<br><b>"
        << "It turns out that in the current case of Cartan element h = "
        << this->unsuitableHs[i]
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
    out
    << "It turns out by direct computation that, in the current case of "
    << this->getOwner().toStringLieAlgebraName()
    << ",  e(P,P_0)= 0 implies that an S-sl(2) subalgebra "
    << "of the root subalgebra generated by P with "
    << "characteristic with 2's in the simple roots of P_0 "
    << "always exists. Note that "
    << "Theorem 10.3 is stated in one direction only.";
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
  std::string floatRow =
  "position: sticky; "
  "top: 0; "
  "background-color: #fafafa; "
  "z-index: 2; ";
  std::string floatColumn =
  "position: sticky; "
  "left: 0; "
  "background-color: #fafafa; "
  "z-index: 1;";
  out
  << "<br><br>"
  << "<div style='max-height: 80vh; max-width: 99%; overflow: auto;'>"
  << "<table><tr>"
  << "<th style='"
  << "position: sticky; "
  << "left: 0; "
  << "top: 0; "
  << "background-color: #fafafa; "
  << "z-index: 3; "
  << "'>Type + realization link</th>"
  << "<th style='padding-right:20px; "
  << floatRow
  << "'>"
  << "<a href='#"
  << idSpanHCharacteristicDescription
  << "'>h-Characteristic</a>"
  << "</th>";
  out
  << "<th align='center' style='white-space: nowrap; "
  << floatRow
  << "'>"
  << "<a href='#idCartanElementRealization'>Realization of h</a>"
  << "</th>"
  << "<th style='padding-left:20px; "
  << floatRow
  << "'>"
  << "<a href='#idModuleDecomposition'>"
  << "sl(2)-module decomposition of the ambient Lie algebra</a> <br> "
  << "\\(\\psi=\\) the fundamental \\(sl(2)\\)-weight. "
  << "</a></th>"
  << "<th style='"
  << floatRow
  << "'>Centralizer dimension</th>"
  << "<th style='"
  << floatRow
  << "'>Type of semisimple part of centralizer, if known</th>"
  << "<th style='"
  << floatRow
  << "'>The square of the length of the weight dual to h.</th>"
  << "<th style='"
  << floatRow
  << "'>Dynkin index </th>"
  << "<th style='"
  << floatRow
  << "'>Minimal containing regular semisimple SAs</th>"
  << "<th style='"
  << floatRow
  << "'>"
  << "<a href='#idMinimalContainingRegularSA'>"
  << "Containing regular semisimple SAs in "
  << "which the sl(2) has no centralizer</a>"
  << "</th></tr>";
  for (int i = 0; i < this->allSubalgebras.size; i ++) {
    const SlTwoSubalgebra& currentSubalgebra = this->allSubalgebras[i];
    out
    << "<tr>"
    << "<td style='padding-right:20px; "
    << floatColumn
    << "'>"
    << "<a href='#sl2index"
    << i
    << "'>\\(A^{"
    << currentSubalgebra.getDynkinIndex()
    << "}_1"
    << "\\)</a>"
    << "</td>";
    out << "<td>";
    out << "\\(" << currentSubalgebra.hCharacteristic.toString() << "\\)";
    out << "</td>";
    out << "<td style='white-space: nowrap'>";
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
    out << "</td><td style='padding-left:20px'>";
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
    out << "</td>";
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
    out << "<td>";
    out << currentSubalgebra.lengthHSquared;
    out << "</td><td>";
    out << currentSubalgebra.getDynkinIndex();
    out << "</td><td>";
    currentSubalgebra.checkIndicesMinimalContainingRootSubalgebras();
    out
    << currentSubalgebra.toStringMinimalContainingRootSubalgebras(
      displayPathAlgebra
    );
    out << "</td><td>";
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
