#ifndef header_math_extra_semisimple_lie_subalgebras_sltwos_ALREADY_INCLUDED
#define header_math_extra_semisimple_lie_subalgebras_sltwos_ALREADY_INCLUDED

#include "math_extra_algebraic_numbers.h"
#include "math_extra_semisimple_lie_algebras.h"
#include "math_extra_semisimple_lie_algebras_root_subalgebras.h"
#include "math_general.h"
#include "math_polynomials.h"

class CentralizerComputer {
public:
  SemisimpleLieAlgebra* owner;
  List<ElementSemisimpleLieAlgebra<Rational> > generatorsToCentralize;
  List<ElementSemisimpleLieAlgebra<Rational> > centralizerBasis;
  List<ElementSemisimpleLieAlgebra<Rational> > centralizerCartan;
  ElementSemisimpleLieAlgebra<Rational> semisimpleElement;
  Matrix<Rational> adjointActionOfSemisimpleElement;
  Polynomial<Rational> characteristicPolynomialAdjointActionSemisimpleElement;
  PolynomialFactorizationUnivariate<Rational>
  factorizationCharacteristicPolynomial;
  DynkinType typeIfKnown;
  bool flagTypeComputed;
  bool flagBasisComputed;
  bool flagCartanSelected;
  CentralizerComputer();
  std::string toString() const;
  bool compute();
  bool intersectAmbientCartanWithCentralizer();
  bool trySemisimpleElement(ElementSemisimpleLieAlgebra<Rational>& candidate);
};

// The sl(2)-subalgebra candidate from which the sl(2) is to be realized.
// Contains internal information about the computation,
// intermediate systems of equations, etc.
class SlTwoSubalgebraCandidate {
public:
  // Owner semisimple Lie algebra.
  SemisimpleLieAlgebra* owner;
  // sl(2)-subalgebra container.
  SlTwoSubalgebras* container;
  // Algebraic closure. Not needed over the complex numbers
  // as sl(2)'s are realizable
  // as rational linear combinations of the Chevalley generators.
  // However, we need this for real form computations.
  AlgebraicClosureRationals* algebraicClosure;
  // Initialized.
  Rational lengthHSquared;
  Vector<Rational> candidateH;
  ElementSemisimpleLieAlgebra<AlgebraicNumber> hAlgebraic;
  Vectors<Rational> participatingPositiveRoots;
  Vectors<Rational> rootsWithScalar2WithH;
  Vectors<Rational> preferredAmbientSimpleBasis;
  ElementSemisimpleLieAlgebra<Rational> hElement;
  // One possible element e for which [e, f] = h
  // [h, e] = 2e.
  ElementSemisimpleLieAlgebra<Rational> eElement;
  // One possible element f for which [e, f] = h
  // [h, f] = -2f.
  ElementSemisimpleLieAlgebra<Rational> fElement;
  ElementSemisimpleLieAlgebra<Polynomial<Rational> > eUnknown;
  ElementSemisimpleLieAlgebra<Polynomial<Rational> > fUnknown;
  ElementSemisimpleLieAlgebra<Polynomial<Rational> > eArbitraryUnknown;
  // Mathematically equal to the previous.
  ElementSemisimpleLieAlgebra<Polynomial<Rational> > hPolynomialRational;
  // Mathematically equal to the previous.
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >
  hPolynomialAlgebraic;
  // Arbitrarily chosen coefficients of the F element over the rationals.
  // This arbitrary choice of F allows to reduce
  // the polynomial system needed to realize the E and F
  // to a linear one.
  // The rationale for this arbitrary choice:
  // 1) Speed (linear systems are easer to solve than polynomial systems).
  // 2) Algorithmic simplicity. When this function was first designed,
  // we did not have machinery for solving polynomial systems
  // so this used to be an algorithmic necessity.
  //
  // Here, we recall that the element H is known and computed by algorithms
  // following Dynkin.
  ElementSemisimpleLieAlgebra<Polynomial<Rational> > fArbitrary;
  // The matrix of the linear system given by systemToSolveArbitrary.
  Matrix<Rational> systemArbitraryMatrix;
  // The column-vector of the linear system given by systemToSolveArbitrary.
  Matrix<Rational> systemArbitraryColumnVector;
  ElementSemisimpleLieAlgebra<Polynomial<Rational> >
  eBracketFMinusHArbitraryUnknown;
  ElementSemisimpleLieAlgebra<Polynomial<Rational> > eBracketFMinusHUnknown;
  // An element of the form x_1 g_{k_1} + ... + x_s g_{k_s} where
  // x_1, ..., x_s are unknowns and g_{?} are the Chevalley generators
  // of the root spaces given by participatingPositiveRoots.
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >
  eKostantSekiguchiUnknown;
  // An element of the form x_1 g_{-k_1} + ... + x_s g_{-k_s} where
  // x_1, ..., x_s are unknowns and g_{-?} are the Chevalley generators
  // of the root spaces given by the negatives of the
  // participatingPositiveRoots.
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >
  fKostantSekiguchiUnknown;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >
  eBracketFMinusHUnknownKostantSekiguchi;
  // The polynomial system required to solve to find a Kostant-sekiguchi triple
  // e, f, h.
  // Here, we assume that theta(h) = - h is already satisfied.
  PolynomialSubstitution<AlgebraicNumber> systemToSolveKostantSekiguchi;
  // The polynomial (actually, linear) system required to solve for
  // eArbitraryUnknown.
  PolynomialSubstitution<Rational> systemToSolveArbitrary;
  // The polynomial system required to solve to find e, f.
  PolynomialSubstitution<Rational> systemToSolve;
  // Fix one Cartan involution theta for which theta(h) = -h.
  //
  // The following element has all properties that the rational element e has,
  // as well as the following additional property:
  //
  // 1) e - f is invariant with respect the cartan involution, i.e.,
  // theta(e-f)=e-f.
  //
  // An h, e, f-triple as above is called a Kostant-Sekiguchi sl(2)-triple.
  ElementSemisimpleLieAlgebra<AlgebraicNumber> eKostantSekiguchi;
  // F element of the Kostant-Sekiguchi sl(2)-triple.
  ElementSemisimpleLieAlgebra<AlgebraicNumber> fKostantSekiguchi;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > eMinusFUnknown;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >
  involutionAppliedToEMinusF;
  // Initializes the h,e,f computation with f arbitrarily chosen.
  // See the preceding comments on why f is chosen arbitrarily.
  void initializeUnknownTriples(const Vector<Rational>& targetH);
  bool attemptExtendingHFtoHEFWithRespectToSubalgebra(
    Vectors<Rational>& rootsWithCharacteristic2,
    Selection& zeroCharacteristics,
    Vectors<Rational>& simpleBasisSubalgebras,
    Vector<Rational>& h,
    bool computeRealForm,
    AlgebraicClosureRationals* inputAlgebraicClosure
  );
  SemisimpleLieAlgebra& getOwnerSemisimpleAlgebra();
  void initializeHEFSystemFromFCoefficients(
    const Vector<Rational>& targetH,
    LinearMapSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >*
    cartanInvolutionPreservedByEMinusF
  );
  void initializeHEFSystemFromFCoefficientsPartTwo();
  void computeLieBracketsUnknowns();
  void computePolynomialSystems();
  const WeylGroupData& getOwnerWeyl() const;
  bool attemptRealizingKostantSekiguchi();
  bool checkConsistencyParticipatingRoots(const Vector<Rational>& targetH);
  void adjoinKostantSekiguchiRelationsToPolynomialSystem(
    LinearMapSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >*
    cartanInvolutionPreservedByEMinusF
  );
};

class SlTwoSubalgebra {
public:
  friend std::ostream& operator<<(
    std::ostream& output, const SlTwoSubalgebra& sl2
  ) {
    output << sl2.toString();
    return output;
  }
  SemisimpleLieAlgebra* owner;
  SlTwoSubalgebras* container;
  AlgebraicClosureRationals* algebraicClosure;
  CharacterSemisimpleLieAlgebraModule<Rational>
  moduleDecompositionAmbientSubalgebra;
  List<int> moduleDimensions;
  int indexInContainer;
  int dimensionCentralizer;
  Rational dimensionCentralizerToralPart;
  bool flagCentralizerIsRegular;
  bool flagTryToComputeCentralizerFully;
  List<int> indicesContainingRootSubalgebras;
  List<int> indicesMinimalContainingRootSubalgebras;
  Vectors<Rational> participatingPositiveRoots;
  // Mathematically equal to the previous.
  ElementSemisimpleLieAlgebra<AlgebraicNumber> hAlgebraic;
  // Used to check our work over the rationals
  ElementSemisimpleLieAlgebra<Rational> hBracketE;
  ElementSemisimpleLieAlgebra<Rational> hBracketF;
  ElementSemisimpleLieAlgebra<Rational> eBracketF;
  // The following describes an optional Cartan involution.
  // A map \theta is a Cartan involution if the following hold.
  // \theta is linear
  // \theta^2 = id.
  // \theta is a lie bracket automorphism, i.e., [\theta(a), \theta(b)] =
  // \theta([a,b]).
  // Here, if theta is not null, we request that it
  // correspond to the maximally compact real form and have the property
  // \theta(h) = -h.
  // TODO(tmilev): does the property above need to be tweaked when
  // we compute outside of the maximally compact real form?
  LinearMapSemisimpleLieAlgebra<Rational>* cartanInvolutionPreservedByEMinusF;
  // Same as hKostantSekiguchi but with polynomial coefficients.
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >
  hKnownKostantSekiguchi;
  List<CharacterSemisimpleLieAlgebraModule<Rational> >
  moduleDecompositionMinimalContainingRootSubalgebras;
  Vector<Rational> hElementCorrespondingToCharacteristic;
  Vectors<Rational> hCommutingRootSpaces;
  DynkinDiagramRootSubalgebra diagramM;
  int dynkinsEpsilon;
  bool flagDeallocated;
  // A list of desired scalar product of the h element with the simple roots.
  // The terminology comes from Dynkin's paper on semisimple Lie subalgebras.
  Vector<Rational> hCharacteristic;
  Vectors<Rational> preferredAmbientSimpleBasis;
  Matrix<Rational> systemArbitraryMatrix;
  Matrix<Rational> systemArbitraryColumnVector;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >
  hPolynomialAlgebraic;
  ElementSemisimpleLieAlgebra<Polynomial<Rational> > fArbitrary;
  ElementSemisimpleLieAlgebra<Rational> hElement;
  ElementSemisimpleLieAlgebra<Rational> eElement;
  ElementSemisimpleLieAlgebra<Rational> fElement;
  ElementSemisimpleLieAlgebra<Polynomial<Rational> > eArbitraryUnknown;
  ElementSemisimpleLieAlgebra<Polynomial<Rational> > eUnknown;
  ElementSemisimpleLieAlgebra<Polynomial<Rational> > fUnknown;
  ElementSemisimpleLieAlgebra<Polynomial<Rational> > eBracketFMinusHUnknown;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >
  eKostantSekiguchiUnknown;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >
  fKostantSekiguchiUnknown;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >
  eBracketFMinusHUnknownKostantSekiguchi;
  PolynomialSubstitution<Rational> systemToSolve;
  PolynomialSubstitution<AlgebraicNumber> systemToSolveKostantSekiguchi;
  ElementSemisimpleLieAlgebra<AlgebraicNumber> eKostantSekiguchi;
  ElementSemisimpleLieAlgebra<AlgebraicNumber> fKostantSekiguchi;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > eMinusFUnknown;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >
  involutionAppliedToEMinusF;
  Rational lengthHSquared;
  CentralizerComputer centralizerComputer;
  void fromSlTwoSubalgebraCandidate(SlTwoSubalgebraCandidate& input);
  void initialize();
  SlTwoSubalgebra() {
    this->flagDeallocated = false;
    this->initialize();
  }
  ~SlTwoSubalgebra() {
    this->flagDeallocated = true;
  }
  void computeDynkinTypeEmbedded(DynkinType& output) const;
  bool checkConsistency() const;
  SlTwoSubalgebras& getContainerSl2s() {
    if (this->container == nullptr) {
      global.fatal
      << "Attempt to "
      << "access the container list of "
      << "a non-initialized sl(2)-subalgebra. "
      << global.fatal;
    }
    return *this->container;
  }
  const WeylGroupData& getOwnerWeyl() const;
  SemisimpleLieAlgebra& getOwnerSemisimpleAlgebra() {
    if (this->owner == nullptr) {
      global.fatal
      << "Attempt to access "
      << "the ambient Lie algebra of a "
      << "non-initialized sl(2)-subalgebra. "
      << global.fatal;
    }
    return *this->owner;
  }
  std::string toStringTriple(FormatExpressions* format) const;
  std::string toStringTripleStandardRealization() const;
  std::string toStringTripleVerification(FormatExpressions* format) const;
  std::string toStringCentralizer() const;
  std::string toStringTripleUnknowns(FormatExpressions* format) const;
  std::string toStringTripleUnknownsPolynomialSystem(
    FormatExpressions* format = nullptr
  ) const;
  template <typename Coefficient>
  std::string toStringPolynomialSystem(
    const PolynomialSubstitution<Coefficient>& system,
    FormatExpressions* format = nullptr
  ) const;
  std::string toStringTripleArbitrary(FormatExpressions* format) const;
  std::string toStringTripleArbitraryMatrix() const;
  std::string toString(FormatExpressions* format = nullptr) const;
  std::string toStringKostantSekiguchiTripleInternals(
    FormatExpressions* format
  ) const;
  std::string toStringKostantSekiguchiTriple(FormatExpressions* format) const;
  std::string toStringKostantSekiguchiTripleStandardRealization() const;
  void toHTML(std::string& filePath);
  void toStringModuleDecompositionMinimalContainingRegularSubalgebras(
    bool useLatex, bool useHtml, SlTwoSubalgebras& owner, std::string& output
  ) const;
  std::string toStringMinimalContainingRootSubalgebras(
    const std::string& displayPathAlgebra
  ) const;
  std::string toStringContainingRootSubalgebras(
    const std::string& displayPathAlgebra
  ) const;
  void computeModuleDecompositionsition(
    const Vectors<Rational>& positiveRootsContainingRegularSubalgebra,
    int dimensionContainingRegularSubalgebra,
    CharacterSemisimpleLieAlgebraModule<Rational>& outputHighestWeights,
    List<int>& outputModuleDimensions
  );
  Rational getDynkinIndex() const;
  bool checkIndicesMinimalContainingRootSubalgebras() const;
  void computeModuleDecompositionsitionAmbientLieAlgebra();
  bool attemptToComputeCentralizer();
  bool moduleDecompositionFitsInto(const SlTwoSubalgebra& other) const;
  static bool moduleDecompositionLeftFitsIntoRight(
    const CharacterSemisimpleLieAlgebraModule<Rational>& moduleDecompoLeft,
    const CharacterSemisimpleLieAlgebraModule<Rational>& moduleDecompoRight
  );
  // the below is outdated, must be deleted as soon as equivalent code is
  // written.
  void computeDynkinsEpsilon(WeylGroupData& weyl);
  bool operator==(const SlTwoSubalgebra& right) const;
  bool operator>(const SlTwoSubalgebra& right) const;
  unsigned int hashFunction() const;
  static inline unsigned int hashFunction(const SlTwoSubalgebra& input) {
    return input.hashFunction();
  }
  void computeModuleDecompositionsitionOfMinimalContainingRegularSAs(
    SlTwoSubalgebras& owner
  );
  void makeReportPrecomputations(
    RootSubalgebra& minimalContainingRegularSubalgebra
  );
};

class SlTwoSubalgebras {
  friend class SemisimpleSubalgebras;
  SemisimpleLieAlgebra* owner;
public:
  static std::string descriptionHCharacteristic;
  static std::string descriptionModuleDecompositionOverSl2;
  static std::string descriptionHRealization;
  static std::string descriptionMinimalContainingRegularSubalgebras;
  HashedList<SlTwoSubalgebra> allSubalgebras;
  List<List<int> > indicesSl2sContainedInRootSubalgebras;
  List<int> indicesSl2DecompositionFormulas;
  Vectors<Rational> unsuitableHs;
  int indexZeroWeight;
  RootSubalgebras rootSubalgebras;
  ~SlTwoSubalgebras() {}
  SlTwoSubalgebras(): owner(nullptr) {}
  SlTwoSubalgebras(SemisimpleLieAlgebra& inputOwner): owner(&inputOwner) {}
  bool operator==(const SlTwoSubalgebras& other) const {
    if (this->owner == nullptr) {
      return other.owner == nullptr;
    }
    if (other.owner == nullptr) {
      return false;
    }
    return this->getOwner() == other.getOwner();
  }
  bool checkConsistency() const;
  bool checkMinimalContainingRootSubalgebras() const;
  void checkInitialization() const;
  WeylGroupData& getOwnerWeyl() const {
    return this->getOwner().weylGroup;
  }
  SemisimpleLieAlgebra& getOwner() const {
    this->checkInitialization();
    return *this->owner;
  }
  void computeModuleDecompositionsitionsOfAmbientLieAlgebra();
  void reset(SemisimpleLieAlgebra& inputOwners);
  bool containsSl2WithGivenH(Vector<Rational>& elementH, int* outputIndex);
  void writeHTML(FormatExpressions* format = nullptr);
  std::string toString(FormatExpressions* format = nullptr);
  std::string toHTMLSummaryTable(FormatExpressions* format = nullptr);
  std::string toHTMLSummary(FormatExpressions* format = nullptr);
  std::string toStringModuleDecompositionMinimalContainingRegularSAs(
    bool useLatex, bool useHtml
  );
  void computeRootSubalgebraContainers();
  void computeOneRootSubalgebraContainers(SlTwoSubalgebra& output);
  void computeCentralizers();
};

#endif // header_math_extra_semisimple_lie_subalgebras_sltwos_ALREADY_INCLUDED
