#ifndef header_math_extra_semisimple_lie_algebras_ALREADY_INCLUDED
#define header_math_extra_semisimple_lie_algebras_ALREADY_INCLUDED

#include "math_extra_drawing_variables.h"
#include "math_extra_finite_groups.h"
#include "math_lie_theory.h"
#include "math_vectors.h"

class Plot;
class CartanInvolution;

template <class Coefficient>
class ElementSemisimpleLieAlgebra: public LinearCombination<
  ChevalleyGenerator, Coefficient
> {
public:
  bool checkConsistency() const {
    if (this->size() == 0) {
      return true;
    }
    SemisimpleLieAlgebra * owner = (*this)[0].owner;
    for (int i = 1; i < this->size(); i ++) {
      if (owner != (*this)[i].owner) {
        global.fatal
        << "ElementSemisimpleLieAlgebra "
        << "contains Chevalley generators with different owners. "
        << global.fatal;
      }
    }
    return true;
  }
  bool needsParenthesisForMultiplication() const;
  Vector<Coefficient> getCartanPart() const;
  void makeGGenerator(
    const Vector<Rational>& root, SemisimpleLieAlgebra& inputOwner
  );
  bool isElementCartan() const;
  void makeCartanGenerator(
    const Vector<Coefficient>& elementH, SemisimpleLieAlgebra& owner
  );
  // Creates the Cartan generator h_i.
  // Here, i is the index of the i^th simple root.
  void makeCartanGeneratorHi(int index, SemisimpleLieAlgebra& owner);
  void makeGenerator(int generatorIndex, SemisimpleLieAlgebra& inputOwner);
  void toVectorNegativeRootSpacesFirst(Vector<Coefficient>& output) const;
  void toVectorNegativeRootSpacesFirst(
    Vector<Coefficient>& output, SemisimpleLieAlgebra& owner
  ) const;
  void assignVectorNegativeRootSpacesCartanPosistiveRootSpaces(
    const Vector<Coefficient>& input, SemisimpleLieAlgebra& owner
  );
  bool coordinatesInBasis(
    const List<ElementSemisimpleLieAlgebra<Coefficient> >& basis,
    Vector<Coefficient>& output
  ) const;
  SemisimpleLieAlgebra* getOwner() const {
    this->checkConsistency();
    if (this->size() == 0) {
      return nullptr;
    }
    return (*this)[0].owner;
  }
  static void getBasisFromSpanOfElements(
    List<ElementSemisimpleLieAlgebra>& elements,
    List<ElementSemisimpleLieAlgebra>& outputBasis
  );
  void lieBracketOnTheRight(
    const ElementSemisimpleLieAlgebra<Coefficient>& element,
    ElementSemisimpleLieAlgebra<Coefficient>& output
  ) const;
  void actOnMe(
    const ElementSemisimpleLieAlgebra<Coefficient>& element,
    ElementSemisimpleLieAlgebra<Coefficient>& output
  ) const;
  void actOnMe(
    const ElementSemisimpleLieAlgebra<Coefficient>& element,
    ElementSemisimpleLieAlgebra<Coefficient>& output,
    SemisimpleLieAlgebra& owner,
    const RationalFraction<Rational>& ringUnit,
    const RationalFraction<Rational>& ringZero
  );
  bool isCoefficientOneChevalleyGenerator();
  bool isProportionalTo(const ElementSemisimpleLieAlgebra<Coefficient>& input)
  const {
    Coefficient unused;
    return this->isProportionalTo(input, unused);
  }
  bool isProportionalTo(
    const ElementSemisimpleLieAlgebra<Coefficient>& input,
    Coefficient& outputTimesThisEqualsInput
  ) const {
    Vector<Coefficient> thisCoordinates;
    Vector<Coefficient> inputCoordinates;
    this->toVectorNegativeRootSpacesFirst(thisCoordinates);
    input.toVectorNegativeRootSpacesFirst(inputCoordinates);
    return
    thisCoordinates.isProportionalTo(
      inputCoordinates, outputTimesThisEqualsInput
    );
  }
  unsigned int hashFunction() const {
    return
    this->::LinearCombination<ChevalleyGenerator, Coefficient>::hashFunction()
    *
    HashConstants::constant1;
  }
  static unsigned int hashFunction(
    const ElementSemisimpleLieAlgebra<Coefficient>& input
  ) {
    return input.hashFunction();
  }
  template <class otherElement>
  void operator=(const otherElement& other) {
    this->::LinearCombination<ChevalleyGenerator, Coefficient>::operator=(
      other
    );
  }
  Vector<Rational> getRootIMustBeWeight() const;
  // Computes the adjoint action of the element in a given basis.
  // Returns false if the basis in not stable with respect to
  // the adjoint action of the element.
  bool computeAdjointActionWithRespectToBasis(
    List<ElementSemisimpleLieAlgebra<Coefficient> >& basis,
    Matrix<Coefficient>& output
  );
};

// Linear map from a semisimple lie algebra to itself.
template <class Coefficient>
class LinearMapSemisimpleLieAlgebra {
public:
  List<ElementSemisimpleLieAlgebra<Coefficient> > imagesChevalleyGenerators;
  void applyTo(
    const ElementSemisimpleLieAlgebra<Coefficient>& input,
    ElementSemisimpleLieAlgebra<Coefficient>& output
  );
  void getMatrix(Matrix<Coefficient>& output);
  void findEigenSpace(
    const Coefficient& eigenValue,
    List<ElementSemisimpleLieAlgebra<Coefficient> >& outputBasis
  );
  // Returns the owner semisimple Lie algebra.
  // Crashes if the map is zero (uninitialized map is automatically zero).
  SemisimpleLieAlgebra& owner();
};

// From
// Shôrô Araki,
// "On root systems and an infinitesimal
// classification of irreducible symmetric spaces", 1962.
class SatakeDiagram {
private:
public:
  enum DiagramType {
    AI, AII, AIII, BI, CI, DI, DII,
  };
  DiagramType diagram;
  // Some of the classical Satake types
  // depend on an additional parameter,
  // smaller than the rank of the ambient complex Lie algebra.
  int parameter;
  // Rank of the simple root system.
  int rank;
  void plot(Plot& output, int verticalOffset);
  void plotAI(Plot& output, int verticalOffset);
  void plotAII(Plot& output, int verticalOffset);
  void plotAIII(Plot& output, int verticalOffset);
  void plotBI(Plot& output, int verticalOffset);
  void plotCI(Plot& output, int verticalOffset);
};

// Represents a Vogan diagram.
// We use the Vogan diagram to define a Cartan involution.
// More precisely, we use use the images of the simple Cartan generators
// given in page 404,
// Knapp, Lie groups beyond an introduction, 2ed, 2002.
// We then extend the map to a Lie algebra homomorphism using
// the straightforward algorithms from class
// HomomorphismSemisimpleLieAlgebra.
class VoganDiagram {
private:
public:
  // This is the column 1 entry of Table 5.11 in the paper mentioned above.
  enum DiagramType {
    AI,
    AII,
    AIII,
    BI,
    CI,
    DI,
    DII,
    EI,
    EII,
    EIII,
    EIV,
    EV,
    EVI,
    EVII,
    EVIII,
    EIX,
    FI,
    FII,
    G
  };
  DiagramType diagram;
  static const int radiusOfRootCircle = 1;
  static const int distanceBetweenRootCenters = 20;
  // Some of the classical Vogan types depend on an additional parameter,
  // smaller than the rank of the ambient complex Lie algebra.
  int parameter;
  // Rank of the simple root system.
  int rank;
  static MapList<
    std::string,
    VoganDiagram::DiagramType,
    HashFunctions::hashFunction<std::string>
  > mapStringToType;
  VoganDiagram();
  void computeMapStringToType();
  bool assignParameter(
    const std::string& input,
    int inputRank,
    int inputParameter,
    std::stringstream* commentsOnFailure
  );
  bool adjustRank();
  bool adjustParameter();
  bool parameterChecks(std::stringstream* commentsOnFailure) const;
  bool dynkinTypeAmbient(
    DynkinType& output, std::stringstream* commentsOnFailure
  );
  DynkinType dynkinTypeAmbientNoFailure();
  std::string toString();
  void plot(Plot& output);
  void plotAI(Plot& output);
  void plotAII(Plot& output);
  void plotAIII(Plot& output);
  void plotBI(Plot& output);
  void plotCI(Plot& output);
  void plotDI(Plot& output);
  void plotDII(Plot& output);
  void plotEI(Plot& output);
  void plotEII(Plot& output);
  void plotEIII(Plot& output);
  void plotEIV(Plot& output);
  static void plotTwoElementOrbit(
    Plot& output,
    int leftIndex,
    int rightIndex,
    int verticalOffset,
    int rootsOnThisPlotRow
  );
  SatakeDiagram computeSatakeDiagram();
};

class SemisimpleLieAlgebra {
private:
  // Semisimple Lie algebras can't be moved once created.
  // Reason: the object is complicated, with lots of
  // compute-on-demand large structures and
  // too many other objects have pointers to it.
  // Therefore the equals operator for the semisimple Lie algebra
  // is private and has no implementation to prevent
  // accidental copying of the object.
  void operator=(const SemisimpleLieAlgebra& other);
  // The killing form square of the dual to the ambient long root.
  // Computed on demand, access through the similarly named function.
  MemorySaving<Rational> cachedKillingSquareOfDualOfAmbientLongRoot;
public:
  WeylGroupData weylGroup;
  // We fix the usual linear space basis for our Lie algebra.
  // The basis consists of Chevalley generators (weight elements),
  // indexed by elements of the root system denoted by greek letters:
  // g_{\alpha}, g_{\beta}, ....
  // In addition to the Chevalley generators,
  // the basis includes of a (vector space) basis of the
  // Cartan subalgebra,
  // indexed by a set of roots forming a simple basis:
  // h_\alpha, h_\beta, ...
  //
  // The Chevalley constants are listed in the same order
  // as the root system of the Weyl group.
  // If \alpha is the root at the i^th position in
  // this->weylGroup.RootSystem and \beta -
  // the root  at the j^th position, then
  // the Chevalley constant N_{\alpha, \beta}
  // given by [g_\alpha, g_\beta] = N_{\alpha, \beta}g_{\alpha +\beta}
  // will be located at the ij^{th} entry in the ChevalleyConstants matrix.
  // Let $\alpha$ be a root.
  // Then our choice of elements h_\alpha
  // of the Cartan subalgebra is such that
  // 1. [g_{\alpha}, g_{-\alpha}] = h_\alpha * (2 / \langle\alpha,
  // \alpha\rangle)
  // 2. [h_{\alpha}, g_\beta] = \langle \alpha, \beta \rangle g^\beta
  // Reference: Samelson, Notes on Lie algebras, pages 46-51
  // We fix a total order to all generators (i.e., linear space basis vectors).
  // We put first the generators corresponding to the negative roots in
  // ascending order,
  // we put second the elements of the Cartan
  // we put last the positive roots in ascending order.
  Matrix<Rational> chevalleyConstants;
  Matrix<bool> computedChevalleyConstants;
  Matrix<ElementSemisimpleLieAlgebra<Rational> > lieBrackets;
  // Order on all generators.
  List<int> universalEnvelopingGeneratorOrder;
  bool flagHasNilradicalOrder;
  bool flagDeallocated;
  bool hasComputedStructureConstants();
  unsigned int hashFunction() const {
    return this->hashFunction(*this);
  }
  static unsigned int hashFunction(const SemisimpleLieAlgebra& input) {
    return input.weylGroup.hashFunction();
  }
  class FileNames {
  public:
    SemisimpleLieAlgebra* owner;
    std::string displayFolderName(const std::string& baseFolder) const;
    std::string fileNameWithPathStructureConstants(
      const std::string& baseFolder
    ) const;
    std::string virtualFolderName() const;
    std::string virtualFileNameWithPathStructureConstants() const;
    std::string fileNameNoPathStructureConstants() const;
    std::string fileNameNoPathSemisimpleSubalgebras() const;
    std::string fileNameNoPathRootSubalgebras() const;
    std::string fileNameRelativePathSlTwoSubalgebras() const;
    std::string fileNameSlTwoRealFormSubalgebraStructure() const;
    std::string virtualFilenameProgressSemisimpleSubalgebras() const;
  };

  SemisimpleLieAlgebra::FileNames fileNames;
  template <class Coefficient>
  void getGenericElementCartan(
    ElementSemisimpleLieAlgebra<Polynomial<Coefficient> >& output,
    int indexFirstVariable = 0
  ) {
    output.makeZero();
    ChevalleyGenerator generator;
    Polynomial<Coefficient> coefficient;
    for (int i = 0; i < this->getRank(); i ++) {
      generator.makeGenerator(
        *this, this->getGeneratorIndexFromNonZeroCoefficientIndexInCartan(i)
      );
      coefficient.makeMonomial(indexFirstVariable + i, 1, 1);
      output.addMonomial(generator, coefficient);
    }
  }
  template <class Coefficient>
  void getGenericElementNegativeBorelNilradical(
    ElementSemisimpleLieAlgebra<Polynomial<Coefficient> >& output,
    int indexFirstVariable = 0
  ) {
    output.makeZero();
    ChevalleyGenerator generator;
    Polynomial<Coefficient> coefficient;
    for (int i = 0; i < this->getNumberOfPositiveRoots(); i ++) {
      generator.makeGenerator(*this, i);
      coefficient.makeMonomial(indexFirstVariable + i, 1, 1);
      output.addMonomial(generator, coefficient);
    }
  }
  int getOppositeGeneratorIndex(int index) {
    return this->getNumberOfGenerators() - index - 1;
  }
  // Returns whether the generator is in a root space that
  // is a simple one. For sl(n), the simple roots are
  // e_1-e_2, ..., e_{n-1}-e_n; roots such as e_1-e_3 are not simple.
  bool isSimpleGenerator(int generatorIndex);
  SemisimpleLieAlgebra() {
    this->flagHasNilradicalOrder = false;
    this->flagDeallocated = false;
    this->fileNames.owner = this;
  }
  ~SemisimpleLieAlgebra() {
    this->fileNames.owner = nullptr;
    this->flagDeallocated = true;
  }
  bool checkConsistency() const;
  template <class Coefficient>
  void generateLieSubalgebra(
    List<ElementSemisimpleLieAlgebra<Coefficient> >& inputOutputGenerators
  );
  void computeLieBracketTable();
  bool isOfSimpleType(char desiredType, int desiredRank) const {
    return this->weylGroup.isOfSimpleType(desiredType, desiredRank);
  }
  template <class Coefficient>
  void getCommonCentralizer(
    const List<ElementSemisimpleLieAlgebra<Coefficient> >&
    inputElementsToCentralize,
    List<ElementSemisimpleLieAlgebra<Coefficient> >& outputCentralizingElements
  );
  void chevalleyGeneratorAsLieBracketsSimpleGenerators(
    int generatorIndex,
    List<int>& outputIndicesFormatAd0Ad1Ad2etc,
    Rational& outputMultiplyLieBracketsToGetGenerator
  );
  std::string toString(FormatExpressions* inputFormat = nullptr);
  std::string toStringHTMLMenuStructureSummary(
    const std::string& relativeTo,
    bool includeStructureConstants,
    bool includeRootSubalgebras,
    bool includeSl2Subalgebras,
    bool includeSemisimpleSubalgebras
  );
  std::string toStringMenuStructurePages(
    const std::string& relativeTo,
    bool includeStructureConstants,
    bool includeRootSubalgebras,
    bool includeSl2Subalgebras,
    bool includeSemisimpleSubalgebras
  ) const;
  void writeHTML(
    bool verbose,
    bool flagWriteLatexPlots,
    const std::string& extraDynkinDiagramPlot
  );
  std::string toHTML(
    bool verbose,
    bool flagWriteLatexPlots,
    const std::string& extraDynkinDiagramPlot
  );
  static std::string toHTMLCalculatorHeadElements(
    const std::string& relativeTo = "../../.."
  );
  static std::string toHTMLCalculatorBodyOnload();
  static std::string toHTMLCalculatorMainDiv();
  std::string getStringFromChevalleyGenerator(
    int index, FormatExpressions* polynomialFormat
  ) const;
  std::string toStringLieAlgebraNameFullHTML() const;
  std::string toStringLieAlgebraName() const;
  std::string toStringLieAlgebraNameNonTechnicalHTML() const;
  int getNumberOfGenerators() const {
    return
    this->weylGroup.cartanSymmetric.numberOfRows +
    this->weylGroup.rootSystem.size;
  }
  int getNumberOfPositiveRoots() const {
    return this->weylGroup.rootsOfBorel.size;
  }
  int getRank() const {
    return this->weylGroup.cartanSymmetric.numberOfRows;
  }
  void orderNilradical(
    const Selection& parabolicSelectionZeroMeansLeviPart,
    bool useNilWeight,
    bool ascending
  );
  void orderNilradicalFirstTotalWeightAscending(
    const Selection& parabolicSelectionZeroMeansLeviPart
  );
  void orderNilradicalFirstTotalWeightDescending(
    const Selection& parabolicSelectionZeroMeansLeviPart
  );
  void orderNilradicalNilWeightAscending(
    const Selection& parabolicSelectionZeroMeansLeviPart
  );
  void orderNilradicalNilWeightDescending(
    const Selection& parabolicSelectionZeroMeansLeviPart
  );
  void orderStandardAscending();
  void orderStandardDescending();
  void orderSSalgebraForHWbfComputation();
  int getGeneratorIndexFromRoot(const Vector<Rational>& input) {
    return
    this->getGeneratorFromRootIndex(
      this->weylGroup.rootSystem.getIndex(input)
    );
  }
  int getRootIndexFromDisplayIndex(int index);
  int getGeneratorFromDisplayIndex(int index) {
    if (index < 0) {
      return index + this->getNumberOfPositiveRoots();
    }
    return index + this->getNumberOfPositiveRoots() + this->getRank() - 1;
  }
  int getGeneratorFromRootIndex(int index) const;
  int getDisplayIndexFromRootIndex(int index) const;
  // The function below returns an negative number if the chevalley generator
  // is an element of the Cartan subalgebra.
  int getRootIndexFromGenerator(int index) const;
  int getGeneratorIndexFromNonZeroCoefficientIndexInCartan(
    int simpleRootIndex
  ) const;
  int getCartanCoordinateIndexFromCartanGeneratorIndex(int generatorIndex)
  const;
  int getDisplayIndexFromGenerator(int index) const;
  bool areOrderedProperly(int leftIndex, int rightIndex);
  bool isGeneratorFromCartan(int index) const;
  bool areOppositeRootSpaces(int leftIndex, int rightIndex) {
    if (
      this->isGeneratorFromCartan(leftIndex) ||
      this->isGeneratorFromCartan(rightIndex)
    ) {
      return false;
    }
    int left = this->getRootIndexFromGenerator(leftIndex);
    int right = this->getRootIndexFromGenerator(rightIndex);
    return (
      this->weylGroup.rootSystem[left] + this->weylGroup.rootSystem[right]
    ).isEqualToZero();
  }
  void computeChevalleyConstants();
  template <class Coefficient>
  Coefficient getKillingForm(
    const ElementSemisimpleLieAlgebra<Coefficient>& left,
    const ElementSemisimpleLieAlgebra<Coefficient>& right
  );
  template <class Coefficient>
  void getKillingFormMatrix(Matrix<Coefficient>& output);
  template <class Coefficient>
  Coefficient getKillingFormProductWRTLevi(
    const ElementSemisimpleLieAlgebra<Coefficient>& left,
    const ElementSemisimpleLieAlgebra<Coefficient>& right,
    const Selection& rootsNotInLevi
  );
  template <class Coefficient>
  void lieBracket(
    const ElementSemisimpleLieAlgebra<Coefficient>& g1,
    const ElementSemisimpleLieAlgebra<Coefficient>& g2,
    ElementSemisimpleLieAlgebra<Coefficient>& output
  );
  // Setup: \gamma +\delta =\epsilon +\zeta =\eta is a Vector<Rational> .
  // then the below function computes n_{-\epsilon, -\zeta}
  void computeOneChevalleyConstant(
    int indexGamma,
    int indexDelta,
    int indexMinusEpsilon,
    int indexMinusZeta,
    int indexEta
  );
  void exploitSymmetryAndCyclicityChevalleyConstants(int indexI, int indexJ);
  void exploitSymmetryChevalleyConstants(int indexI, int indexJ);
  void exploitTheCyclicTrick(int i, int j, int k);
  bool getMaxQForWhichBetaMinusQAlphaIsARoot(
    const Vector<Rational>& alpha, const Vector<Rational>& beta, int& output
  ) const;
  Rational getConstant(
    const Vector<Rational>& root1, const Vector<Rational>& root2
  );
  Vector<Rational> getWeightOfGenerator(int index);
  // returns true if returning constant, false if returning element of h
  bool getConstantOrHElement(
    const Vector<Rational>& root1,
    const Vector<Rational>& root2,
    Rational& outputRat,
    Vector<Rational>& outputH
  );
  bool testForConsistency();
  bool attemptFindingHEF(
    ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& inputOutputH,
    ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& inputOutputE,
    ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& inputOutputF,
    std::stringstream* logStream = nullptr
  );
  bool attemptExtendingEtoHEFwithHinCartan(
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& elementE,
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& outputH,
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& outputF,
    std::stringstream* logStream = nullptr
  );
  template <class Coefficient>
  void getAdjoint(
    Matrix<Coefficient>& output, ElementSemisimpleLieAlgebra<Coefficient>& e
  );
  template <class Coefficient>
  void getAdjoint(
    MatrixTensor<Coefficient>& output,
    ElementSemisimpleLieAlgebra<Coefficient>& e
  );
  void makeChevalleyTestReport(int i, int j, int k, int total);
  bool isInTheWeightSupport(
    Vector<Rational>& weight, Vector<Rational>& highestWeight
  );
  void createEmbeddingFromFDModuleHaving1dimWeightSpaces(
    Vector<Rational>& highestWeight
  );
  // For a simple Lie algebra, returns an index of a long simple root.
  // When the semisimple Lie algebra is not simple, returns -1.
  int longRootIndex();
  int getLengthStringAlongAlphaThroughBeta(
    Vector<Rational>& alpha,
    Vector<Rational>& beta,
    int& distanceToHighestWeight,
    Vectors<Rational>& weightSupport
  );
  void computeOneAutomorphism(
    Matrix<Rational>& outputAutomorphism, bool useNegativeRootsFirst
  );
  bool operator==(const SemisimpleLieAlgebra& other) const {
    return this->weylGroup == other.weylGroup;
  }
  // Returns the matrix realization of an element for one particular
  // preferred "standard" representation of the lie algebra.
  // As of writing, the "standard" representation is only implemented for
  // sl(n) - the natural (n x n)- matrix realization.
  template <typename Coefficient>
  bool getElementStandardRepresentation(
    const ElementSemisimpleLieAlgebra<Coefficient>& element,
    Matrix<Coefficient>& output
  );
  // Same as the previous function but for elements of the Universal Enveloping
  // Algebra.
  template <typename Coefficient>
  bool getElementStandardRepresentation(
    const ElementUniversalEnveloping<Coefficient>& element,
    Matrix<Coefficient>& output,
    std::stringstream* commentsOnFailure
  );
  // Same as the previous function but for elements of the Universal Enveloping
  // Algebra.
  template <typename Coefficient>
  bool getElementAdjointRepresentation(
    const ElementUniversalEnveloping<Coefficient>& element,
    Matrix<Coefficient>& output,
    std::stringstream* commentsOnFailure
  );
  // Given an element acting by the adjoint action on an
  // ad-stable vector subspace of the Lie algebra, returns the matrix
  // of the action with coordinates corresponding to the
  // given vector subspace basis.
  template <typename Coefficient>
  bool getElementAdjointRepresentationWithRespectToBasis(
    const ElementSemisimpleLieAlgebra<Coefficient>& element,
    List<ElementSemisimpleLieAlgebra<Coefficient> >& basisInvariantSpace,
    Matrix<Coefficient>& output,
    std::stringstream* commentsOnFailure = nullptr
  );
  template <typename Coefficient>
  bool accumulateChevalleyGeneratorStandardRepresentation(
    const ChevalleyGenerator& element,
    const Coefficient& coefficient,
    Matrix<Coefficient>& output
  );
  template <typename Coefficient>
  bool accumulateChevalleyGeneratorStandardRepresentationInTypeA(
    const ChevalleyGenerator& element,
    const Coefficient& coefficient,
    Matrix<Coefficient>& output
  );
  // Whether the ambient Lie algebra has a Cartan involution that has been
  // implemented.
  bool hasImplementedCartanInvolution(
    const VoganDiagram& voganDiagram,
    CartanInvolution* whichInvolution,
    std::stringstream* commentsOnFailure
  );
  template <typename Coefficient>
  bool hasImplementedCartanInvolutionMaximallyCompactCase(
    LinearMapSemisimpleLieAlgebra<Coefficient>* whichInvolution
  );
  // If the subalgebra is simple, computes or fetches from cache
  // the killing form square of the dual of the long root.
  // If the subalgebra is not simple, the notion of "long root"
  // is regarded as ill-defined and the function returns 0.
  Rational killingSquareOfDualOfAmbientLongRoot();
};

template <class Coefficient>
class Weight {
public:
  SemisimpleLieAlgebra* owner;
  Vector<Coefficient> weightFundamentalCoordinates;
  friend std::ostream& operator<<(
    std::ostream& output, const Weight<Coefficient>& input
  ) {
    output << input.toString();
    return output;
  }
  Weight(): owner(nullptr) {}
  void checkNonZeroOwner() const {
    if (this->owner != nullptr) {
      return;
    }
    global.fatal
    << "Monomial char has zero owner, "
    << "which is not allowed by the current function call. "
    << global.fatal;
  }
  void accountSingleWeight(
    const Vector<Rational>& currentWeightSimpleCoordinates,
    const Vector<Rational>& otherhighestWeightSimpleCoordinates,
    Rational& multiplicity,
    CharacterSemisimpleLieAlgebraModule<Coefficient>& outputAccum
  ) const;
  std::string tensorAndDecompose(
    const Weight<Coefficient>& other,
    CharacterSemisimpleLieAlgebraModule<Coefficient>& output,
    const Coefficient& zero = Coefficient::zero()
  ) const;
  std::string toString(FormatExpressions* format = nullptr) const;
  inline unsigned int hashFunction() const {
    return weightFundamentalCoordinates.hashFunction();
  }
  static inline unsigned int hashFunction(const Weight<Coefficient>& input) {
    return input.hashFunction();
  }
  void operator+=(const Weight<Coefficient>& other) {
    if (this->owner != other.owner) {
      global.fatal
      << "Attempting to add weights with different owner algebras. "
      << global.fatal;
    }
    this->weightFundamentalCoordinates += other.weightFundamentalCoordinates;
  }
  void operator*=(const Coefficient& other) {
    this->weightFundamentalCoordinates *= other;
  }
  inline bool operator==(const Weight<Coefficient>& other) const {
    return
    this->weightFundamentalCoordinates == other.weightFundamentalCoordinates &&
    this->owner == other.owner;
  }
  inline bool operator>(const Weight<Coefficient>& other) const {
    return
    this->weightFundamentalCoordinates >
    other.weightFundamentalCoordinates;
  }
};

template <class Coefficient>
class CharacterSemisimpleLieAlgebraModule: public LinearCombination<
  Weight<Coefficient>, Coefficient
> {
public:
  void checkConsistency() const {
    if (this->size() == 0) {
      return;
    }
    const SemisimpleLieAlgebra* owner = (*this)[0].owner;
    for (int i = 1; i < this->size(); i ++) {
      if ((*this)[i].owner != owner) {
        global.fatal
        << "CharacterSemisimpleLieAlgebraModule contains "
        << "elements belonging to different semisimple Lie algebras. "
        << global.fatal;
      }
    }
  }
  void checkNonZeroOwner() const {
    this->checkConsistency();
    if (this->getOwner() == 0) {
      global.fatal
      <<
      "CharacterSemisimpleLieAlgebraModule has no owner semisimple Lie algebra, "
      << "which is not allowed at by the calling function. "
      << global.fatal;
    }
  }
  bool isEqualToZero() {
    return this->size() == 0;
  }
  unsigned int hashFunction() const {
    return this->hashFunction(*this);
  }
  static unsigned int hashFunction(
    const CharacterSemisimpleLieAlgebraModule<Coefficient>& input
  ) {
    return
    input.::LinearCombination<Weight<Coefficient>, Coefficient>::hashFunction(
      input
    );
  }
  void getDual(CharacterSemisimpleLieAlgebraModule<Coefficient>& output) const;
  void makeFromWeight(
    const Vector<Coefficient>& inputweightSimpleCoordinates,
    SemisimpleLieAlgebra* inputOwner
  );
  bool splitCharacterOverReductiveSubalgebra(
    std::string* report,
    CharacterSemisimpleLieAlgebraModule& output,
    BranchingData& inputData
  );
  bool freudenthalEvalMeDominantWeightsOnly(
    CharacterSemisimpleLieAlgebraModule<Coefficient>& outputCharOwnerSetToZero,
    int upperBoundTotalDominantWeights,
    std::string* outputDetails
  );
  bool freudenthalEvaluateMeFullCharacter(
    CharacterSemisimpleLieAlgebraModule<Coefficient>& outputCharOwnerSetToZero,
    int upperBoundTotalDominantWeights,
    std::string* outputDetails
  );
  std::string toStringFullCharacterWeightsTable();
  bool drawMeNoMultiplicities(
    std::string& outputDetails,
    DrawingVariables& drawingVariables,
    int upperBoundWeights
  ) {
    return
    this->drawMe(outputDetails, drawingVariables, upperBoundWeights, false);
  }
  int getPositiveNStringSuchThatWeightMinusNAlphaIsWeight(
    const Weight<Coefficient>& weightInFundamentalCoordinates,
    const Vector<Coefficient>& alphaInFundamentalCoordinates
  );
  bool drawMeWithMultiplicities(
    std::string& outputDetails,
    DrawingVariables& drawingVariables,
    int upperBoundWeights
  ) {
    return
    this->drawMe(outputDetails, drawingVariables, upperBoundWeights, true);
  }
  void drawMeAssumeCharIsOverCartan(
    WeylGroupData& actualAmbientWeyl, DrawingVariables& drawingVariables
  ) const;
  SemisimpleLieAlgebra* getOwner() const {
    if (this->size() == 0) {
      global.fatal
      << "Requesting owner semisimple Lie algebra of zero character. "
      << global.fatal;
    }
    return (*this)[0].owner;
  }
  bool drawMe(
    std::string& outputDetails,
    DrawingVariables& drawingVariables,
    int upperBoundWeights,
    bool useMultiplicities
  );
  bool splitOverLeviMonomialsEncodeHighestWeight(
    std::string* report,
    CharacterSemisimpleLieAlgebraModule& output,
    const Selection& splittingParabolicSelection,
    const Selection& parabolicSelectionFiniteDimensionalInducingPart,
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms&
    outputWeylSubgroup
  );
  int getIndexExtremeWeightRelativeToWeyl(WeylGroupData& weyl) const;
  void makeTrivial(SemisimpleLieAlgebra& inputOwner);
  std::string multiplyBy(const CharacterSemisimpleLieAlgebraModule& other);
  std::string operator*=(const CharacterSemisimpleLieAlgebraModule& other);
  std::string operator*=(const Weight<Rational>& other);
  CharacterSemisimpleLieAlgebraModule<Coefficient> operator-(
    const CharacterSemisimpleLieAlgebraModule<Coefficient>& right
  ) const {
    CharacterSemisimpleLieAlgebraModule<Coefficient> output = *this;
    output -= right;
    return output;
  }
  void operator*=(const Coefficient& other) {
    this->::LinearCombination<Weight<Coefficient>, Coefficient>::operator*=(
      other
    );
  }
};

class HomomorphismSemisimpleLieAlgebra {
public:
  SemisimpleLieAlgebra* domain;
  SemisimpleLieAlgebra* coDomain;
  // Images of g_1, g_2, ..., g_{rank}, in this order.
  List<ElementSemisimpleLieAlgebra<Rational> >
  imagesPositiveSimpleChevalleyGenerators;
  // Images of g_-1, g_-2, ..., g_{-rank}, in this order.
  List<ElementSemisimpleLieAlgebra<Rational> >
  imagesNegativeSimpleChevalleyGenerators;
  // imagesAllChevalleyGenerators are given in the same order as
  // the one used in MonomialUniversalEnveloping.
  List<ElementSemisimpleLieAlgebra<Rational> > imagesAllChevalleyGenerators;
  List<ElementSemisimpleLieAlgebra<Rational> > domainAllChevalleyGenerators;
  List<ElementSemisimpleLieAlgebra<Rational> > gModK;
  Vectors<Rational> restrictedRootSystem;
  Vectors<Rational> imagesCartanDomain;
  SemisimpleLieAlgebra& domainAlgebra() {
    if (this->domain == nullptr) {
      global.fatal
      << "Non-initialized HomomorphismSemisimpleLieAlgebra. "
      << global.fatal;
    }
    return *this->domain;
  }
  SemisimpleLieAlgebra& coDomainAlgebra() {
    if (this->coDomain == nullptr) {
      global.fatal
      << "Non-initialized HomomorphismSemisimpleLieAlgebra. "
      << global.fatal;
    }
    return *this->coDomain;
  }
  HomomorphismSemisimpleLieAlgebra(): domain(nullptr), coDomain(nullptr) {}
  void getWeightsGmodKInSimpleCoordinatesK(Vectors<Rational>& outputWeights) {
    this->getWeightsRelativeToKInSimpleKCoordinates(
      outputWeights, this->gModK
    );
  }
  void getWeightsRelativeToKInSimpleKCoordinates(
    Vectors<Rational>& outputWeights,
    List<ElementSemisimpleLieAlgebra<Rational> >& inputElements
  );
  void toString(std::string& output) {
    this->toString(output, false);
  }
  void toString(std::string& output, bool useHtml);
  void makeGInGWithIdentity(SemisimpleLieAlgebra& owner);
  void projectOntoSmallCartan(
    Vector<Rational>& input, Vector<Rational>& output
  );
  void projectOntoSmallCartan(
    Vectors<Rational>& input, Vectors<Rational>& output
  );
  void getMapSmallCartanDualToLargeCartanDual(Matrix<Rational>& output);
  std::string toString(bool useHtml = true);
  void getRestrictionAmbientRootSystemToTheSmallerCartanSubalgebra(
    Vectors<Rational>& output
  );
  bool computeHomomorphismFromImagesSimpleChevalleyGenerators(
    std::stringstream* commentsOnFailure
  );
  bool checkIsHomomorphism();
  bool checkClosednessLieBracket();
  bool checkInitialization();
  void applyHomomorphism(
    const ElementSemisimpleLieAlgebra<Rational>& input,
    ElementSemisimpleLieAlgebra<Rational>& output
  );
  bool applyHomomorphism(
    const ElementUniversalEnveloping<RationalFraction<Rational> >& input,
    ElementUniversalEnveloping<RationalFraction<Rational> >& output
  );
  bool applyHomomorphism(
    const MonomialUniversalEnveloping<RationalFraction<Rational> >& input,
    const RationalFraction<Rational>& coefficient,
    ElementUniversalEnveloping<RationalFraction<Rational> >& output
  );
};

class CartanInvolution {
private:
  // The cartan involution swaps the two simple generators.
  void setSimpleRootSwap(int indexLeft, int indexRight);
  // The cartan involution acts by - 1 on the given simple generator.
  void setFilledSimpleRoot(int index);
  // The cartan involution as the identity on the given simple generator.
  void setHollowSimpleRoot(int index);
public:
  SemisimpleLieAlgebra* owner;
  VoganDiagram voganDiagram;
  LinearMapSemisimpleLieAlgebra<Rational> linearMap;
  HomomorphismSemisimpleLieAlgebra automorphism;
  bool dynkinTypeAmbient(
    DynkinType& output, std::stringstream* commentsOnFailure
  );
  DynkinType dynkinTypeAmbientNoFailure();
  std::string toString();
  CartanInvolution();
  bool computeSimpleRootImagesTypeAI(std::stringstream* commentsOnFailure);
  bool computeSimpleRootImagesTypeAII(std::stringstream* commentsOnFailure);
  bool computeSimpleRootImagesTypeAIII(std::stringstream* commentsOnFailure);
  bool computeSimpleRootImagesTypeBI(std::stringstream* commentsOnFailure);
  bool computeSimpleRootImagesTypeCI(std::stringstream* commentsOnFailure);
  bool computeSimpleRootImagesTypeD(
    std::stringstream* commentsOnFailure, bool useAutomorphism
  );
  bool computeSimpleRootImagesTypeDI(std::stringstream* commentsOnFailure);
  bool computeSimpleRootImagesTypeDII(std::stringstream* commentsOnFailure);
  bool computeSimpleRootImagesTypeEI(std::stringstream* commentsOnFailure);
  bool computeSimpleRootImagesTypeEII(std::stringstream* commentsOnFailure);
  bool computeSimpleRootImagesTypeEIII(std::stringstream* commentsOnFailure);
  bool computeSimpleRootImagesTypeEIV(std::stringstream* commentsOnFailure);
  bool computeSimpleRootImages(std::stringstream* commentsOnFailure);
  bool computeFromDiagram(
    const VoganDiagram& inputDiagram,
    SemisimpleLieAlgebra& inputOwner,
    std::stringstream* commentsOnFailure
  );
  void plotVoganDiagram(Plot& output);
  void plotSatakeDiagram(Plot& output, int verticalOffset);
};

template <class Coefficient>
bool ElementSemisimpleLieAlgebra<Coefficient>::coordinatesInBasis(
  const List<ElementSemisimpleLieAlgebra>& basis, Vector<Coefficient>& output
) const {
  STACK_TRACE("ElementSemisimpleLieAlgebra::coordinatesInBasis");
  if (basis.size == 0) {
    return false;
  }
  if (this->isEqualToZero()) {
    output.makeZero(basis.size);
    return true;
  }
  Vectors<Coefficient> basisVectors;
  Vector<Coefficient> element;
  basisVectors.setSize(basis.size);
  for (int i = 0; i < basis.size; i ++) {
    basis[i].toVectorNegativeRootSpacesFirst(basisVectors[i]);
  }
  this->toVectorNegativeRootSpacesFirst(element);
  return element.coordinatesInBasis(basisVectors, output);
}

template <class Coefficient>
void ElementSemisimpleLieAlgebra<Coefficient>::makeGenerator(
  int generatorIndex, SemisimpleLieAlgebra& inputOwner
) {
  this->makeZero();
  ChevalleyGenerator resultGenerator;
  resultGenerator.makeGenerator(inputOwner, generatorIndex);
  this->addMonomial(resultGenerator, 1);
}

template <class Coefficient>
std::string Weight<Coefficient>::toString(FormatExpressions* format) const {
  std::stringstream out;
  bool formatWeightAsIndexVectorSpace = format ==
  nullptr ? true : format->flagFormatWeightAsVectorSpaceIndex;
  if (!formatWeightAsIndexVectorSpace) {
    if (this->owner == nullptr) {
      return
      this->weightFundamentalCoordinates.toStringLetterFormat("\\psi", format);
    }
    Vector<Coefficient> weightEpsilonCoordinates;
    Vector<Coefficient> weightSimpleCoordinates;
    weightSimpleCoordinates =
    this->owner->weylGroup.getSimpleCoordinatesFromFundamental(
      this->weightFundamentalCoordinates, Coefficient::zero()
    );
    this->owner->weylGroup.getEpsilonCoordinates(
      weightSimpleCoordinates, weightEpsilonCoordinates
    );
    return
    weightEpsilonCoordinates.toStringLetterFormat("\\varepsilon", format);
  }
  bool useOmega = true;
  std::string oldCustomPlus;
  std::string VectorSpaceLetter = "V";
  if (format != nullptr) {
    useOmega = (format->fundamentalWeightLetter == "");
    oldCustomPlus = format->customPlusSign;
    format->customPlusSign = "";
    VectorSpaceLetter = format->finiteDimensionalRepresentationLetter;
  }
  if (useOmega) {
    out
    << VectorSpaceLetter
    << "_{"
    << this->weightFundamentalCoordinates.toStringLetterFormat(
      "\\omega", format
    )
    << "}";
  } else {
    out
    << VectorSpaceLetter
    << "_{"
    << this->weightFundamentalCoordinates.toStringLetterFormat(
      format->fundamentalWeightLetter, format
    )
    << "}";
  }
  if (format != nullptr) {
    format->customPlusSign = oldCustomPlus;
  }
  return out.str();
}

template <class Coefficient>
void Weight<Coefficient>::accountSingleWeight(
  const Vector<Rational>& currentWeightSimpleCoordinates,
  const Vector<Rational>& otherhighestWeightSimpleCoordinates,
  Rational& multiplicity,
  CharacterSemisimpleLieAlgebraModule<Coefficient>& outputAccum
) const {
  // This is the Brauer-Klimyk formula. Reference:
  // Humphreys J., Introduction to Lie algebras and representation theory
  // page 142, exercise 9.
  STACK_TRACE("Weight_CoefficientType::accountSingleWeight");
  this->checkNonZeroOwner();
  Vector<Rational> dominant = currentWeightSimpleCoordinates;
  dominant += otherhighestWeightSimpleCoordinates;
  WeylGroupData& weylGroup = this->owner->weylGroup;
  dominant += weylGroup.rho;
  int sign;
  // a weight has no stabilizer if and only if it is not stabilized by all root
  // reflections.
  for (int i = 0; i < weylGroup.rootsOfBorel.size; i ++) {
    if (
      weylGroup.rootScalarCartanRoot(dominant, weylGroup.rootsOfBorel[i]).
      isEqualToZero()
    ) {
      return;
    }
  }
  weylGroup.raiseToDominantWeight(dominant, &sign);
  dominant -= weylGroup.rho;
  if (!weylGroup.isDominantWeight(dominant)) {
    return;
  }
  Weight<Rational> monomial;
  monomial.owner = this->owner;
  monomial.weightFundamentalCoordinates =
  weylGroup.getFundamentalCoordinatesFromSimple(dominant);
  Coefficient coefficientChange;
  coefficientChange = multiplicity;
  coefficientChange *= sign;
  outputAccum.addMonomial(monomial, coefficientChange);
}

template <class Coefficient>
std::string Weight<Coefficient>::tensorAndDecompose(
  const Weight<Coefficient>& other,
  CharacterSemisimpleLieAlgebraModule<Coefficient>& output,
  const Coefficient& zero
) const {
  // This is the Brauer-Klimyk formula. Reference:
  // Humphreys J. Introduction to Lie algebras and representation theory
  // page 142, exercise 9.
  STACK_TRACE("Weight_CoefficientType::tensorAndDecompose");
  this->checkNonZeroOwner();
  std::stringstream errorLog;
  std::string currentString;
  output.makeZero();
  WeylGroupData& weylGrouop = this->owner->weylGroup;
  Vector<Rational> lefthighestWeightFundamentalCoordinates;
  lefthighestWeightFundamentalCoordinates = this->weightFundamentalCoordinates;
  Vector<Rational> righthighestWeightFundamentalCoordinates;
  righthighestWeightFundamentalCoordinates =
  other.weightFundamentalCoordinates;
  Rational leftTotalDim =
  weylGrouop.weylDimFormulaFundamentalCoordinates(
    lefthighestWeightFundamentalCoordinates
  );
  Rational rightTotalDim =
  weylGrouop.weylDimFormulaFundamentalCoordinates(
    righthighestWeightFundamentalCoordinates
  );
  if (leftTotalDim > rightTotalDim) {
    MathRoutines::swap(leftTotalDim, rightTotalDim);
    MathRoutines::swap(
      lefthighestWeightFundamentalCoordinates,
      righthighestWeightFundamentalCoordinates
    );
  }
  HashedList<Vector<Coefficient> > weightsLeftSimpleCoordinates;
  List<Rational> multiplicitiesLeft;
  if (
    !weylGrouop.freudenthalFormula(
      lefthighestWeightFundamentalCoordinates,
      weightsLeftSimpleCoordinates,
      multiplicitiesLeft,
      &currentString,
      1000000
    )
  ) {
    errorLog << "Freudenthal formula generated error: " << currentString;
    return errorLog.str();
  }
  HashedList<Vector<Coefficient> > currentOrbit;
  const int orbitSizeHardLimit = 10000000;
  Vector<Rational> righthighestWeightSimpleCoordinates =
  weylGrouop.getSimpleCoordinatesFromFundamental(
    righthighestWeightFundamentalCoordinates, zero
  );
  Vectors<Rational> roots;
  roots.setSize(1);
  for (int i = 0; i < weightsLeftSimpleCoordinates.size; i ++) {
    roots[0] = weightsLeftSimpleCoordinates[i];
    weylGrouop.generateOrbit(
      roots, false, currentOrbit, false, 0, 0, orbitSizeHardLimit
    );
    if (currentOrbit.size >= orbitSizeHardLimit) {
      errorLog
      << "Error: orbit layer size exceeded hard-coded limit of "
      << orbitSizeHardLimit
      << ".";
      return errorLog.str();
    }
    for (int j = 0; j < currentOrbit.size; j ++) {
      this->accountSingleWeight(
        currentOrbit[j],
        righthighestWeightSimpleCoordinates,
        multiplicitiesLeft[i],
        output
      );
    }
  }
  return errorLog.str();
}

template <class Coefficient>
bool CharacterSemisimpleLieAlgebraModule<Coefficient>::
freudenthalEvaluateMeFullCharacter(
  CharacterSemisimpleLieAlgebraModule<Coefficient>& outputCharOwnerSetToZero,
  int upperBoundTotalDominantWeights,
  std::string* outputDetails
) {
  STACK_TRACE(
    "charSSAlgMod_CoefficientType::freudenthalEvaluateMeFullCharacter"
  );
  this->checkNonZeroOwner();
  CharacterSemisimpleLieAlgebraModule<Coefficient> domChar;
  if (
    !this->freudenthalEvalMeDominantWeightsOnly(
      domChar, upperBoundTotalDominantWeights, outputDetails
    )
  ) {
    return false;
  }
  outputCharOwnerSetToZero.makeZero();
  Vectors<Rational> vector;
  HashedList<Vector<Coefficient> > orbit;
  vector.setSize(1);
  Weight<Coefficient> monomial;
  monomial.owner = nullptr;
  for (int i = 0; i < domChar.size(); i ++) {
    vector[0] =
    this->getOwner()->weylGroup.getSimpleCoordinatesFromFundamental(
      domChar[i].weightFundamentalCoordinates, Rational::zero()
    );
    if (
      !(
        this->getOwner()->weylGroup.generateOrbit(
          vector, false, orbit, false, - 1, 0, upperBoundTotalDominantWeights
        )
      )
    ) {
      if (outputDetails != nullptr) {
        *outputDetails = "failed to generate orbit (possibly too large?)";
      }
      return false;
    }
    int orbitSize = orbit.size;
    for (int j = 0; j < orbitSize; j ++) {
      monomial.weightFundamentalCoordinates =
      this->getOwner()->weylGroup.getFundamentalCoordinatesFromSimple(
        orbit[j]
      );
      outputCharOwnerSetToZero.addMonomial(monomial, domChar.coefficients[i]);
    }
  }
  return true;
}

template <class Coefficient>
void CharacterSemisimpleLieAlgebraModule<Coefficient>::getDual(
  CharacterSemisimpleLieAlgebraModule<Coefficient>& output
) const {
  if (&output == this) {
    CharacterSemisimpleLieAlgebraModule<Coefficient> thisCopy = *this;
    thisCopy.getDual(output);
    return;
  }
  Weight<Coefficient> monomial;
  output.makeZero();
  for (int i = 0; i < this->size(); i ++) {
    monomial = (*this)[i];
    monomial.weightFundamentalCoordinates.negate();
    output.addMonomial(monomial, this->coefficients[i]);
  }
}

template <class Coefficient>
void CharacterSemisimpleLieAlgebraModule<Coefficient>::makeFromWeight(
  const Vector<Coefficient>& inputweightSimpleCoordinates,
  SemisimpleLieAlgebra* inputOwner
) {
  this->makeZero();
  if (inputweightSimpleCoordinates.size != inputOwner->getRank()) {
    global.fatal
    << "Attempting to create a character from highest weight in simple coords "
    << inputweightSimpleCoordinates.toString()
    << "("
    << inputweightSimpleCoordinates.size
    << " coordinates) while the owner semisimple "
    << " Lie algebra is of rank "
    << (inputOwner->getRank())
    << global.fatal;
  }
  Weight<Coefficient> monomial;
  monomial.owner = inputOwner;
  monomial.weightFundamentalCoordinates =
  inputOwner->weylGroup.getFundamentalCoordinatesFromSimple(
    inputweightSimpleCoordinates
  );
  this->addMonomial(monomial, 1);
}

template <class Coefficient>
bool CharacterSemisimpleLieAlgebraModule<Coefficient>::
freudenthalEvalMeDominantWeightsOnly(
  CharacterSemisimpleLieAlgebraModule<Coefficient>& outputCharOwnerSetToZero,
  int upperBoundTotalDominantWeights,
  std::string* outputDetails
) {
  STACK_TRACE(
    "charSSAlgMod_CoefficientType::freudenthalEvalMeDominantWeightsOnly"
  );
  if (&outputCharOwnerSetToZero == this) {
    CharacterSemisimpleLieAlgebraModule<Coefficient> thisCopy = *this;
    return
    thisCopy.freudenthalEvalMeDominantWeightsOnly(
      outputCharOwnerSetToZero, upperBoundTotalDominantWeights, outputDetails
    );
  }
  this->checkNonZeroOwner();
  outputCharOwnerSetToZero.makeZero();
  Vector<Coefficient> currentweightFundamentalCoordinates;
  List<Coefficient> currentMults;
  HashedList<Vector<Coefficient> > currentWeights;
  std::stringstream localErrors, localDetails;
  std::string localDetail;
  Weight<Coefficient> monomial;
  monomial.owner = nullptr;
  Coefficient bufferCoefficient;
  for (int i = 0; i < this->size(); i ++) {
    currentweightFundamentalCoordinates = (*this)[i].
    weightFundamentalCoordinates;
    if (
      !this->getOwner()->weylGroup.freudenthalFormula(
        currentweightFundamentalCoordinates,
        currentWeights,
        currentMults,
        &localDetail,
        upperBoundTotalDominantWeights
      )
    ) {
      if (outputDetails != nullptr) {
        localErrors
        << "Encountered error while evaluating "
        << "Freudenthal formula. Error details: "
        << localDetail
        << "<br> Further computation detail: "
        << localDetails.str();
        *outputDetails = localErrors.str();
      }
      return false;
    }
    if (localDetail != "") {
      localDetails
      << "<br>MonomialPolynomial "
      << i + 1
      << " computation details: "
      << localDetail;
    }
    for (int j = 0; j < currentWeights.size; j ++) {
      monomial.weightFundamentalCoordinates =
      this->getOwner()->weylGroup.getFundamentalCoordinatesFromSimple(
        currentWeights[j]
      );
      bufferCoefficient = this->coefficients[i];
      bufferCoefficient *= currentMults[j];
      outputCharOwnerSetToZero.addMonomial(monomial, bufferCoefficient);
    }
  }
  if (outputDetails != nullptr) {
    *outputDetails = localDetails.str();
  }
  return true;
}

template <class Coefficient>
void SemisimpleLieAlgebra::getAdjoint(
  MatrixTensor<Coefficient>& output,
  ElementSemisimpleLieAlgebra<Coefficient>& e
) {
  Matrix<Coefficient> matForm;
  this->getAdjoint(matForm, e);
  output = matForm;
}

template <class Coefficient>
void SemisimpleLieAlgebra::getAdjoint(
  Matrix<Coefficient>& output, ElementSemisimpleLieAlgebra<Coefficient>& e
) {
  int numberOfGenerators = this->getNumberOfGenerators();
  output.initialize(numberOfGenerators, numberOfGenerators);
  output.makeZero();
  ElementSemisimpleLieAlgebra<Coefficient> generator, result;
  for (int i = 0; i < numberOfGenerators; i ++) {
    generator.makeGenerator(i, *this);
    this->lieBracket(e, generator, result);
    for (int j = 0; j < result.size(); j ++) {
      output(result[j].generatorIndex, i) = result.coefficients[j];
    }
  }
}

template <class Coefficient>
void SemisimpleLieAlgebra::getCommonCentralizer(
  const List<ElementSemisimpleLieAlgebra<Coefficient> >&
  inputElementsToCentralize,
  List<ElementSemisimpleLieAlgebra<Coefficient> >& outputCentralizingElements
) {
  STACK_TRACE("SemisimpleLieAlgebra::getCommonCentralizer");
  Matrix<Coefficient> currentAdjointOperator;
  Matrix<Coefficient> commonAdjointOperator;
  for (int i = 0; i < inputElementsToCentralize.size; i ++) {
    this->getAdjoint(currentAdjointOperator, inputElementsToCentralize[i]);
    commonAdjointOperator.appendMatrixToTheBottom(currentAdjointOperator);
  }
  Vectors<Coefficient> outputVectors;
  commonAdjointOperator.getZeroEigenSpace(outputVectors);
  outputCentralizingElements.setSize(outputVectors.size);
  for (int i = 0; i < outputVectors.size; i ++) {
    ElementSemisimpleLieAlgebra<Coefficient>& currentElement =
    outputCentralizingElements[i];
    currentElement.assignVectorNegativeRootSpacesCartanPosistiveRootSpaces(
      outputVectors[i], *this
    );
  }
}

template <class Coefficient>
void SemisimpleLieAlgebra::lieBracket(
  const ElementSemisimpleLieAlgebra<Coefficient>& g1,
  const ElementSemisimpleLieAlgebra<Coefficient>& g2,
  ElementSemisimpleLieAlgebra<Coefficient>& output
) {
  if (&output == &g1 || &output == &g2) {
    ElementSemisimpleLieAlgebra<Coefficient> outputNew;
    this->lieBracket(g1, g2, outputNew);
    output = outputNew;
    return;
  }
  output.makeZero();
  if (g1.isEqualToZero() || g2.isEqualToZero()) {
    return;
  }
  int maximumMonomialsFinal = g1.size() * g2.size();
  output.setExpectedSize(maximumMonomialsFinal);
  Coefficient coefficient;
  ElementSemisimpleLieAlgebra<Coefficient> buffer;
  for (int i = 0; i < g1.size(); i ++) {
    for (int j = 0; j < g2.size(); j ++) {
      buffer =
      this->lieBrackets.elements[g1[i].generatorIndex][g2[j].generatorIndex];
      coefficient = g1.coefficients[i];
      coefficient *= g2.coefficients[j];
      buffer *= coefficient;
      output += buffer;
    }
  }
}

template <class Coefficient>
void ElementSemisimpleLieAlgebra<Coefficient>::actOnMe(
  const ElementSemisimpleLieAlgebra<Coefficient>& element,
  ElementSemisimpleLieAlgebra<Coefficient>& output
) const {
  element.lieBracketOnTheRight(*this, output);
}

template <class Coefficient>
void ElementSemisimpleLieAlgebra<Coefficient>::lieBracketOnTheRight(
  const ElementSemisimpleLieAlgebra<Coefficient>& element,
  ElementSemisimpleLieAlgebra<Coefficient>& output
) const {
  if (this->isEqualToZero()) {
    output = *this;
    return;
  }
  this->getOwner()->lieBracket(*this, element, output);
}

template <class Coefficient>
bool ElementSemisimpleLieAlgebra<Coefficient>::
computeAdjointActionWithRespectToBasis(
  List<ElementSemisimpleLieAlgebra<Coefficient> >& basis,
  Matrix<Coefficient>& output
) {
  if (this->isEqualToZero()) {
    output.makeZeroMatrix(basis.size, 0);
    return true;
  }
  return
  this->getOwner()->getElementAdjointRepresentationWithRespectToBasis(
    *this, basis, output
  );
}

template <typename Coefficient>
bool SemisimpleLieAlgebra::getElementAdjointRepresentationWithRespectToBasis(
  const ElementSemisimpleLieAlgebra<Coefficient>& element,
  List<ElementSemisimpleLieAlgebra<Coefficient> >& basisInvariantSpace,
  Matrix<Coefficient>& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE(
    "SemisimpleLieAlgebra::"
    "getElementAdjointRepresentationWithRespectToBasis"
  );
  if (
    ElementSemisimpleLieAlgebra<Coefficient>::getRankElementSpan(
      basisInvariantSpace
    ) !=
    basisInvariantSpace.size
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "The given invariant space basis is not linearly independent: "
      << basisInvariantSpace.toStringCommaDelimited();
    }
    return false;
  }
  output.makeZeroMatrix(basisInvariantSpace.size, 0);
  ElementSemisimpleLieAlgebra<Coefficient> actionOnBasisElement;
  Vector<Coefficient> basisElementTransformation;
  for (int i = 0; i < basisInvariantSpace.size; i ++) {
    const ElementSemisimpleLieAlgebra<Coefficient>& basisElement =
    basisInvariantSpace[i];
    element.lieBracketOnTheRight(basisElement, actionOnBasisElement);
    if (
      !actionOnBasisElement.coordinatesInBasis(
        basisInvariantSpace, basisElementTransformation
      )
    ) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "The lie bracket of "
        << element.toString()
        << " with basis element:"
        << basisElement.toString()
        << " does not lie in the vector space generated by: "
        << basisInvariantSpace.toStringCommaDelimited();
      }
      return false;
    }
    output.assignVectorToColumnKeepOtherColumnsIntact(
      i, basisElementTransformation
    );
  }
  return true;
}

template <class Coefficient>
Vector<Coefficient> ElementSemisimpleLieAlgebra<Coefficient>::getCartanPart()
const {
  Vector<Coefficient> result;
  if (this->isEqualToZero()) {
    result.makeZero(0);
    return result;
  }
  ChevalleyGenerator tempGen;
  SemisimpleLieAlgebra* owner = this->getOwner();
  int rank = owner->getRank();
  int numberOfPositiveRoots = owner->getNumberOfPositiveRoots();
  result.makeZero(rank);
  if (rank <= 0 || owner == nullptr) {
    global.fatal
    << "The owner of "
    << "a semisimple Lie algebra element is non-present or corrupted. "
    << global.fatal;
  }
  for (int i = 0; i < rank; i ++) {
    tempGen.makeGenerator(*owner, i + numberOfPositiveRoots);
    int currentIndex = this->monomials.getIndex(tempGen);
    if (currentIndex != - 1) {
      result[i] += this->coefficients[currentIndex];
    }
  }
  return result;
}

template <class Coefficient>
void ElementSemisimpleLieAlgebra<Coefficient>::toVectorNegativeRootSpacesFirst(
  Vector<Coefficient>& output, SemisimpleLieAlgebra& owner
) const {
  if (this->isEqualToZero()) {
    output.makeZero(owner.getNumberOfGenerators());
    return;
  }
  return this->toVectorNegativeRootSpacesFirst(output);
}

template <class Coefficient>
void ElementSemisimpleLieAlgebra<Coefficient>::toVectorNegativeRootSpacesFirst(
  Vector<Coefficient>& output
) const {
  if (this->isEqualToZero()) {
    output.makeZero(0);
    return;
  }
  output.makeZero(this->getOwner()->getNumberOfGenerators());
  for (int i = 0; i < this->size(); i ++) {
    output[(*this)[i].generatorIndex] = this->coefficients[i];
  }
}

template <class Coefficient>
void ElementSemisimpleLieAlgebra<Coefficient>::makeGGenerator(
  const Vector<Rational>& root, SemisimpleLieAlgebra& inputOwner
) {
  int generatorIndex = inputOwner.getGeneratorIndexFromRoot(root);
  if (generatorIndex < 0) {
    global.fatal
    << "Function makeGGenerator does not allow non-root input: "
    << root.toString()
    << global.fatal;
  }
  this->makeGenerator(generatorIndex, inputOwner);
}

template <class Coefficient>
void ElementSemisimpleLieAlgebra<Coefficient>::
assignVectorNegativeRootSpacesCartanPosistiveRootSpaces(
  const Vector<Coefficient>& input, SemisimpleLieAlgebra& owner
) {
  // Changing RootSystem order invalidates this function!
  this->makeZero();
  ChevalleyGenerator generator;
  for (int i = 0; i < input.size; i ++) {
    if (input[i] == 0) {
      continue;
    }
    generator.makeGenerator(owner, i);
    this->addMonomial(generator, input[i]);
  }
}

template <class Coefficient>
bool ElementSemisimpleLieAlgebra<Coefficient>::isElementCartan() const {
  if (this->size() == 0) {
    return true;
  }
  SemisimpleLieAlgebra * owner = (*this)[0].owner;
  for (int i = 0; i < this->size(); i ++) {
    if (!owner->isGeneratorFromCartan((*this)[i].generatorIndex)) {
      return false;
    }
  }
  return true;
}

template <class Coefficient>
void ElementSemisimpleLieAlgebra<Coefficient>::makeCartanGeneratorHi(
  int index, SemisimpleLieAlgebra& owner
) {
  this->makeZero();
  ChevalleyGenerator generator;
  generator.makeGenerator(
    owner, owner.getGeneratorIndexFromNonZeroCoefficientIndexInCartan(index)
  );
  this->addMonomial(generator, 1);
}

template <class Coefficient>
void ElementSemisimpleLieAlgebra<Coefficient>::makeCartanGenerator(
  const Vector<Coefficient>& elementH, SemisimpleLieAlgebra& owner
) {
  ChevalleyGenerator generator;
  this->makeZero();
  for (int i = 0; i < elementH.size; i ++) {
    generator.makeGenerator(
      owner, owner.getGeneratorIndexFromNonZeroCoefficientIndexInCartan(i)
    );
    this->addMonomial(generator, elementH[i]);
  }
}

template <class Coefficient>
bool CharacterSemisimpleLieAlgebraModule<Coefficient>::drawMe(
  std::string& outputDetails,
  DrawingVariables& drawingVariables,
  int upperBoundWeights,
  bool useMultiplicities
) {
  STACK_TRACE("CharacterSemisimpleLieAlgebraModule::drawMe");
  this->checkNonZeroOwner();
  CharacterSemisimpleLieAlgebraModule<Coefficient> characteristicCartan;
  bool result =
  this->freudenthalEvalMeDominantWeightsOnly(
    characteristicCartan, upperBoundWeights, &outputDetails
  );
  std::stringstream out;
  WeylGroupData& weylGroup = this->getOwner()->weylGroup;
  weylGroup.drawRootSystem(drawingVariables, false, false);
  int totalWeights = 0;
  Vectors<Coefficient> dominantWeightsNonHashed;
  HashedList<Vector<Coefficient> > finalWeights;
  Vector<Rational> convertor;
  for (int i = 0; i < characteristicCartan.size(); i ++) {
    const Weight<Coefficient>& monomial = characteristicCartan[i];
    dominantWeightsNonHashed.size = 0;
    dominantWeightsNonHashed.addOnTop(
      weylGroup.getSimpleCoordinatesFromFundamental(
        monomial.weightFundamentalCoordinates
      )
    );
    bool isTrimmed =
    !weylGroup.generateOrbit(
      dominantWeightsNonHashed,
      false,
      finalWeights,
      false,
      0,
      0,
      upperBoundWeights
    );
    totalWeights += finalWeights.size;
    if (isTrimmed || totalWeights > upperBoundWeights) {
      out << "Did not generate all weights of the module due to RAM limits. ";
      result = false;
      break;
    }
    for (int j = 0; j < finalWeights.size; j ++) {
      convertor = finalWeights[j].getVectorRational();
      drawingVariables.drawCircleAtVectorBufferRational(convertor, "black", 3);
      if (useMultiplicities) {
        drawingVariables.drawTextAtVector(
          convertor,
          characteristicCartan.coefficients[i].toString(),
          "black",
          drawingVariables.fontSizeNormal
        );
      }
    }
  }
  out << "<br>Number of computed weights: " << totalWeights << ". ";
  if (result && totalWeights < upperBoundWeights) {
    out << "<br>All weights were computed and are drawn. <br>";
  } else {
    out << "<br><b> Not all weights were computed. </b>";
  }
  outputDetails = out.str();
  return result;
}

template <class Coefficient>
void CharacterSemisimpleLieAlgebraModule<Coefficient>::
drawMeAssumeCharIsOverCartan(
  WeylGroupData& actualAmbientWeyl, DrawingVariables& drawingVariables
) const {
  if (actualAmbientWeyl.getDimension() < 2) {
    return;
  }
  Vector<Coefficient> actualWeight;
  Vector<Rational> actualWeightRationalPart;
  actualAmbientWeyl.drawRootSystem(
    drawingVariables, true, false, nullptr, false
  );
  for (int j = 0; j < this->size(); j ++) {
    actualWeight =
    actualAmbientWeyl.getSimpleCoordinatesFromFundamental((*this)[j].
      weightFundamentalCoordinates
    );
    actualWeightRationalPart = actualWeight.getVectorRational();
    // <-type conversion here!
    drawingVariables.drawCircleAtVectorBufferRational(
      actualWeightRationalPart, "black", 5
    );
    drawingVariables.drawTextAtVector(
      actualWeightRationalPart, this->coefficients[j].toString(), "black"
    );
  }
}

template <class Coefficient>
void CharacterSemisimpleLieAlgebraModule<Coefficient>::makeTrivial(
  SemisimpleLieAlgebra& inputOwner
) {
  this->makeZero();
  Weight<Rational> monomial;
  monomial.owner = &inputOwner;
  monomial.weightFundamentalCoordinates.makeZero(inputOwner.getRank());
  this->addMonomial(monomial, 1);
}

template <class Coefficient>
std::string CharacterSemisimpleLieAlgebraModule<Coefficient>::operator*=(
  const CharacterSemisimpleLieAlgebraModule& other
) {
  return this->multiplyBy(other);
}

template <class Coefficient>
std::string CharacterSemisimpleLieAlgebraModule<Coefficient>::multiplyBy(
  const CharacterSemisimpleLieAlgebraModule& other
) {
  if (this->getOwner() != other.getOwner() || this->getOwner() == 0) {
    global.fatal
    << "Attempt to multiply characters of "
    << "different or non-initialized semisimple Lie algebras."
    << global.fatal;
  }
  this->setExpectedSize(other.size() + this->size());
  CharacterSemisimpleLieAlgebraModule result, summand;
  result.makeZero();
  std::string potentialError;
  Coefficient coefficient;
  for (int i = 0; i < this->size(); i ++) {
    for (int j = 0; j < other.size(); j ++) {
      const Weight<Rational>& left = (*this)[i];
      const Weight<Rational>& right = other[j];
      potentialError = left.tensorAndDecompose(right, summand);
      if (potentialError != "") {
        return potentialError;
      }
      coefficient = this->coefficients[i];
      coefficient *= other.coefficients[j];
      summand *= coefficient;
      result += summand;
    }
  }
  this->operator=(result);
  return "";
}

template <class Coefficient>
void SemisimpleLieAlgebra::getKillingFormMatrix(Matrix<Coefficient>& output) {
  output.makeZeroMatrix(this->getNumberOfGenerators(), 0);
  ElementSemisimpleLieAlgebra<Coefficient> left, right;
  for (int i = 0; i < this->getNumberOfGenerators(); i ++) {
    left.makeGenerator(i, *this);
    for (int j = 0; j < this->getNumberOfGenerators(); j ++) {
      right.makeGenerator(j, *this);
      output(i, j) = this->getKillingForm(left, right);
    }
  }
}

template <class Coefficient>
Coefficient SemisimpleLieAlgebra::getKillingForm(
  const ElementSemisimpleLieAlgebra<Coefficient>& left,
  const ElementSemisimpleLieAlgebra<Coefficient>& right
) {
  STACK_TRACE("SemisimpleLieAlgebra::getKillingForm");
  Coefficient result = 0;
  ElementSemisimpleLieAlgebra<Coefficient> adadAppliedToMonomial;
  ElementSemisimpleLieAlgebra<Coefficient> element;
  ChevalleyGenerator baseGenerator;
  for (int i = 0; i < this->getNumberOfGenerators(); i ++) {
    baseGenerator.makeGenerator(*this, i);
    adadAppliedToMonomial.makeZero();
    adadAppliedToMonomial.addMonomial(baseGenerator, 1);
    this->lieBracket(right, adadAppliedToMonomial, element);
    this->lieBracket(left, element, adadAppliedToMonomial);
    result += adadAppliedToMonomial.getCoefficientOf(baseGenerator);
  }
  return result;
}

template <typename Coefficient>
bool SemisimpleLieAlgebra::getElementStandardRepresentation(
  const ElementSemisimpleLieAlgebra<Coefficient>& element,
  Matrix<Coefficient>& output
) {
  STACK_TRACE("SemisimpleLieAlgebra::getElementStandardRepresentation");
  char dynkinType = 0;
  if (!this->weylGroup.dynkinType.isSimple(&dynkinType, nullptr)) {
    return false;
  }
  if (dynkinType != 'A') {
    // Only type A is supported as of writing.
    return false;
  }
  output.makeZeroMatrix(this->getRank() + 1, 0);
  for (int i = 0; i < element.size(); i ++) {
    this->accumulateChevalleyGeneratorStandardRepresentation(
      element[i], element.coefficients[i], output
    );
  }
  return true;
}

template <typename Coefficient>
bool SemisimpleLieAlgebra::getElementStandardRepresentation(
  const ElementUniversalEnveloping<Coefficient>& element,
  Matrix<Coefficient>& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("SemisimpleLieAlgebra::getElementStandardRepresentation");
  char dynkinType = 0;
  if (!this->weylGroup.dynkinType.isSimple(&dynkinType, nullptr)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "getElementStandardRepresentation "
      << "implemented only for simple Lie algebras.";
    }
    return false;
  }
  if (dynkinType != 'A') {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "getElementStandardRepresentation implemented only for type A.";
    }
    // Only type A is supported as of writing.
    return false;
  }
  int dimension = this->getRank() + 1;
  output.makeZeroMatrix(dimension, 0);
  Matrix<Coefficient> generatorMatrix, monomialMatrix, powerMatrix, identity;
  identity.makeIdentity(output, 0, 1);
  Coefficient one, zero;
  one = 1;
  zero = 0;
  for (int i = 0; i < element.size(); i ++) {
    const MonomialUniversalEnveloping<Coefficient>& monomial = element[i];
    monomialMatrix.makeIdentity(output, 0, 1);
    for (int j = 0; j < monomial.generatorsIndices.size; j ++) {
      ChevalleyGenerator generator;
      generator.makeGenerator(*this, monomial.generatorsIndices[j]);
      powerMatrix.initialize(dimension, dimension);
      powerMatrix.makeZero(zero);
      this->accumulateChevalleyGeneratorStandardRepresentation(
        generator, one, powerMatrix
      );
      int power = 0;
      if (!monomial.powers[j].isSmallInteger(&power)) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "Failed to extract powers. ";
        }
        return false;
      }
      MathRoutines::raiseToPower(powerMatrix, power, identity);
      monomialMatrix.multiplyOnTheRight(powerMatrix);
    }
    monomialMatrix *= element.coefficients[i];
    output += monomialMatrix;
  }
  return true;
}

template <typename Coefficient>
bool SemisimpleLieAlgebra::accumulateChevalleyGeneratorStandardRepresentation(
  const ChevalleyGenerator& element,
  const Coefficient& coefficient,
  Matrix<Coefficient>& output
) {
  char dynkinType = 0;
  int rank = 0;
  if (!this->weylGroup.dynkinType.isSimple(&dynkinType, &rank)) {
    return false;
  }
  if (dynkinType != 'A') {
    // Only type A is supported as of writing.
    return false;
  }
  if (element.owner != this) {
    global.fatal
    << "Generator does not belong to this semisimple Lie algebra."
    << global.fatal;
  }
  return
  accumulateChevalleyGeneratorStandardRepresentationInTypeA(
    element, coefficient, output
  );
}

template <typename Coefficient>
bool SemisimpleLieAlgebra::
accumulateChevalleyGeneratorStandardRepresentationInTypeA(
  const ChevalleyGenerator& element,
  const Coefficient& coefficient,
  Matrix<Coefficient>& output
) {
  STACK_TRACE(
    "SemisimpleLieAlgebra::"
    "accumulateChevalleyGeneratorStandardRepresentationInTypeA"
  );
  if (element.isInCartan(nullptr)) {
    int index =
    this->getCartanCoordinateIndexFromCartanGeneratorIndex(
      element.generatorIndex
    );
    output(index, index) += coefficient;
    output(index + 1, index + 1) -= coefficient;
    return true;
  }
  Vector<Rational> vector;
  if (!element.isInRootSpace(&vector)) {
    global.fatal
    << "Chevalley generator is neither in a "
    << "root space nor in the Cartan subalgebra. "
    << global.fatal;
  }
  int firstNonZeroIndex = 0;
  for (; firstNonZeroIndex < vector.size; firstNonZeroIndex ++) {
    if (vector[firstNonZeroIndex] != 0) {
      break;
    }
  }
  int lastNonZeroIndex = firstNonZeroIndex;
  for (; lastNonZeroIndex < vector.size; lastNonZeroIndex ++) {
    if (vector[lastNonZeroIndex] == 0) {
      break;
    }
  }
  int rowIndex = 0;
  int columnIndex = 0;
  if (vector[firstNonZeroIndex] > 0) {
    rowIndex = firstNonZeroIndex;
    columnIndex = lastNonZeroIndex;
  } else {
    rowIndex = lastNonZeroIndex;
    columnIndex = firstNonZeroIndex;
  }
  output(rowIndex, columnIndex) += coefficient;
  return true;
}

template <typename Coefficient>
bool SemisimpleLieAlgebra::hasImplementedCartanInvolutionMaximallyCompactCase(
  LinearMapSemisimpleLieAlgebra<Coefficient>* whichInvolution
) {
  char dynkinType = 0;
  if (!this->weylGroup.dynkinType.isSimple(&dynkinType)) {
    return false;
  }
  if (dynkinType != 'A') {
    return false;
  }
  if (whichInvolution == nullptr) {
    return true;
  }
  whichInvolution->imagesChevalleyGenerators.setSize(
    this->getNumberOfGenerators()
  );
  int numberOfPositiveRoots = this->getNumberOfPositiveRoots();
  int rank = this->getRank();
  for (int i = 0; i < numberOfPositiveRoots; i ++) {
    const Vector<Rational>& root = this->weylGroup.rootsOfBorel[i];
    ChevalleyGenerator positive;
    positive.makeGeneratorRootSpace(*this, root);
    ChevalleyGenerator negative;
    negative.makeGeneratorRootSpace(*this, - root);
    whichInvolution->imagesChevalleyGenerators[positive.generatorIndex].
    makeGGenerator(- root, *this);
    whichInvolution->imagesChevalleyGenerators[positive.generatorIndex] *= - 1;
    whichInvolution->imagesChevalleyGenerators[negative.generatorIndex].
    makeGGenerator(root, *this);
    whichInvolution->imagesChevalleyGenerators[negative.generatorIndex] *= - 1;
  }
  for (int i = 0; i < rank; i ++) {
    ElementSemisimpleLieAlgebra<Coefficient>& current =
    whichInvolution->imagesChevalleyGenerators[numberOfPositiveRoots + i];
    Vector<Rational> root;
    root.makeEi(rank, i);
    current.makeCartanGenerator(root, *this);
    current *= - 1;
  }
  return true;
}

template <class Coefficient>
void LinearMapSemisimpleLieAlgebra<Coefficient>::applyTo(
  const ElementSemisimpleLieAlgebra<Coefficient>& input,
  ElementSemisimpleLieAlgebra<Coefficient>& output
) {
  STACK_TRACE("LinearMapSemisimpleLieAlgebra::applyTo");
  if (&input == &output) {
    ElementSemisimpleLieAlgebra<Coefficient> copy = input;
    this->applyTo(copy, output);
    return;
  }
  output.makeZero();
  ElementSemisimpleLieAlgebra<Coefficient> contribution;
  for (int i = 0; i < input.size(); i ++) {
    const ChevalleyGenerator& current = input.monomials[i];
    ElementSemisimpleLieAlgebra<Coefficient> contribution =
    this->imagesChevalleyGenerators[current.generatorIndex];
    contribution *= input.coefficients[i];
    output += contribution;
  }
}

template <class Coefficient>
SemisimpleLieAlgebra&LinearMapSemisimpleLieAlgebra<Coefficient>::owner() {
  for (int i = 0; i < this->imagesChevalleyGenerators.size; i ++) {
    ElementSemisimpleLieAlgebra<Coefficient>& current =
    this->imagesChevalleyGenerators[i];
    if (current.size() == 0) {
      continue;
    }
    return *current.getOwner();
  }
  global.fatal
  << "owner() function called on zero or empty "
  << "linear map of semisimple lie algebras. "
  << global.fatal;
  SemisimpleLieAlgebra* empty = nullptr;
  return *empty;
}

template <class Coefficient>
void LinearMapSemisimpleLieAlgebra<Coefficient>::findEigenSpace(
  const Coefficient& eigenValue,
  List<ElementSemisimpleLieAlgebra<Coefficient> >& outputBasis
) {
  STACK_TRACE("LinearMapSemisimpleLieAlgebra::findEigenSpace");
  outputBasis.clear();
  Matrix<Coefficient> matrix;
  this->getMatrix(matrix);
  List<Vector<Coefficient> > eigenSpace;
  matrix.getEigenspaceModifyMe(eigenValue, eigenSpace);
  ChevalleyGenerator generator;
  SemisimpleLieAlgebra& ownerAlgebra = this->owner();
  for (int i = 0; i < eigenSpace.size; i ++) {
    ElementSemisimpleLieAlgebra<Coefficient> next;
    Vector<Coefficient>& current = eigenSpace[i];
    for (int j = 0; j < current.size; j ++) {
      generator.makeGenerator(ownerAlgebra, j);
      next.addMonomial(generator, current[j]);
    }
    outputBasis.addOnTop(next);
  }
}

template <class Coefficient>
void LinearMapSemisimpleLieAlgebra<Coefficient>::getMatrix(
  Matrix<Coefficient>& output
) {
  STACK_TRACE("LinearMapSemisimpleLieAlgebra::getMatrix");
  if (this->imagesChevalleyGenerators.size == 0) {
    output.makeZero();
    return;
  }
  output.makeZeroMatrix(this->imagesChevalleyGenerators.size, 0);
  for (int i = 0; i < this->imagesChevalleyGenerators.size; i ++) {
    ElementSemisimpleLieAlgebra<Coefficient>& current =
    this->imagesChevalleyGenerators[i];
    for (int j = 0; j < current.size(); j ++) {
      const ChevalleyGenerator& monomial = current.monomials[j];
      int rowIndex = monomial.generatorIndex;
      output(rowIndex, i) = current.coefficients[j];
    }
  }
}

#endif // header_math_extra_semisimple_lie_algebras_ALREADY_INCLUDED
