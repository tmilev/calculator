// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeaderSemisimpleLieAlgebrasIncluded
#define vpfHeaderSemisimpleLieAlgebrasIncluded

#include "math_extra_finite_groups.h"

class Plot;
class CartanInvolution;

// From
// Shôrô Araki,
// "On root systems and an infinitesimal
// classification of irreducible symmetric spaces", 1962.
class SatakeDiagram {
private:
public:
  enum DiagramType {
    AI,
    AII,
    AIII,
    BI,
    CI,
    DI,
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
    DIII,
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
    std::string, VoganDiagram::DiagramType, MathRoutines::hashString
  > mapStringToType;
  VoganDiagram();
  void computeMapStringToType();
  bool assignParameter(
    const std::string& input,
    int inputRank,
    int inputParameter,
    std::stringstream* commentsOnFailure
  );
  bool dynkinTypeAmbient(
    DynkinType& output,
    std::stringstream* commentsOnFailure
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
  void operator=(const SemisimpleLieAlgebra& other);
  // <-semisimple Lie algebra can't be moved once created.
  // <-Too many objects have pointers to it.
public:
  bool flagAnErrorHasOccurredTimeToPanic;
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
  // this->theWyl.RootSystem and \beta -
  // the root  at the j^th position, then
  // the Chevalley constant N_{\alpha, \beta}
  // given by [g_\alpha, g_\beta] = N_{\alpha, \beta}g_{\alpha +\beta}
  // will be located at the ij^{th} entry in the ChevalleyConstants matrix.
  // Let $\alpha$ be a root.
  // Then our choice of elements h_\alpha
  // of the Cartan subalgebra is such that
  // 1. [g_{\alpha}, g_{-\alpha}] = h_\alpha * (2 / \langle\alpha, \alpha\rangle)
  // 2. [h_{\alpha}, g_\beta] = \langle \alpha, \beta \rangle g^\beta
  // Reference: Samelson, Notes on Lie algebras, pages 46-51
  // We fix a total order to all generators (i.e., linear space basis vectors).
  // We put first the generators corresponding to the negative roots in ascending order,
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
    std::string fileNameWithPathStructureConstants(const std::string& baseFolder) const;
    std::string virtualFolderName() const;
    std::string virtualFileNameWithPathStructureConstants() const;
    std::string fileNameNoPathStructureConstants() const;
    std::string fileNameNoPathSemisimpleSubalgebras() const;
    std::string fileNameNoPathRootSubalgebras() const;
    std::string fileNameRelativePathSlTwoSubalgebras() const;
    std::string fileNameSlTwoRealFormSubalgebraStructure() const;
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
        *this,
        this->getGeneratorIndexFromNonZeroCoefficientIndexInCartan(i)
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
  void generateLieSubalgebra(List<ElementSemisimpleLieAlgebra<Coefficient> >& inputOutputGenerators);
  void computeLieBracketTable();
  bool isOfSimpleType(char desiredType, int desiredRank) const {
    return this->weylGroup.isOfSimpleType(desiredType, desiredRank);
  }
  template <class Coefficient>
  void getCommonCentralizer(
    const List<ElementSemisimpleLieAlgebra<Coefficient> >& inputElementsToCentralize,
    List<ElementSemisimpleLieAlgebra<Coefficient> >& outputCentralizingElements
  );
  void getChevalleyGeneratorAsLieBracketsSimpleGenerators(
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
  std::string getStringFromChevalleyGenerator(int index, FormatExpressions* polynomialFormat) const;
  std::string toStringLieAlgebraNameFullHTML() const;
  std::string toStringLieAlgebraName() const;
  std::string toStringLieAlgebraNameNonTechnicalHTML() const;
  int getNumberOfGenerators() const {
    return this->weylGroup.cartanSymmetric.numberOfRows + this->weylGroup.rootSystem.size;
  }
  int getNumberOfPositiveRoots() const {
    return this->weylGroup.rootsOfBorel.size;
  }
  int getRank() const {
    return this->weylGroup.cartanSymmetric.numberOfRows;
  }
  void orderNilradical(const Selection& parabolicSelectionZeroMeansLeviPart, bool useNilWeight, bool ascending);
  void orderNilradicalFirstTotalWeightAscending(const Selection& parabolicSelectionZeroMeansLeviPart);
  void orderNilradicalFirstTotalWeightDescending(const Selection& parabolicSelectionZeroMeansLeviPart);
  void orderNilradicalNilWeightAscending(const Selection& parSelZeroMeansLeviPart);
  void orderNilradicalNilWeightDescending(const Selection& parSelZeroMeansLeviPart);
  void orderStandardAscending();
  void orderStandardDescending();

  void orderSSalgebraForHWbfComputation();
  int getGeneratorIndexFromRoot(const Vector<Rational>& input) {
    return this->getGeneratorFromRootIndex(this->weylGroup.rootSystem.getIndex(input));
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
  int getGeneratorIndexFromNonZeroCoefficientIndexInCartan(int simpleRootIndex) const;
  int getCartanCoordinateIndexFromCartanGeneratorIndex(int generatorIndex) const;
  int getDisplayIndexFromGenerator(int index) const;
  bool areOrderedProperly(int leftIndex, int rightIndex);
  bool isGeneratorFromCartan(int index) const;
  bool areOppositeRootSpaces(int leftIndex, int rightIndex) {
    if (this->isGeneratorFromCartan(leftIndex) || this->isGeneratorFromCartan(rightIndex)) {
      return false;
    }
    int left = this->getRootIndexFromGenerator(leftIndex);
    int right = this->getRootIndexFromGenerator(rightIndex);
    return (this->weylGroup.rootSystem[left] + this->weylGroup.rootSystem[right]).isEqualToZero();
  }
  void computeChevalleyConstants();
  template<class Coefficient>
  Coefficient getKillingForm(
    const ElementSemisimpleLieAlgebra<Coefficient>& left,
    const ElementSemisimpleLieAlgebra<Coefficient>& right
  );
  template<class Coefficient>
  void getKillingFormMatrix(
    Matrix<Coefficient>& output
  );
  template <class Coefficient>
  Coefficient getKillingFormProductWRTLevi(
    const ElementSemisimpleLieAlgebra<Coefficient>& left,
    const ElementSemisimpleLieAlgebra<Coefficient>& right,
    const Selection& rootsNotInLevi
  );
  template<class Coefficient>
  void lieBracket(
    const ElementSemisimpleLieAlgebra<Coefficient>& g1,
    const ElementSemisimpleLieAlgebra<Coefficient>& g2,
    ElementSemisimpleLieAlgebra<Coefficient>& output
  );
  // Setup: \gamma +\delta =\epsilon +\zeta =\eta is a Vector<Rational> .
  // then the below function computes n_{-\epsilon, -\zeta}
  void computeOneChevalleyConstant(int indexGamma, int indexDelta, int indexMinusEpsilon, int indexMinusZeta, int indexEta);
  void exploitSymmetryAndCyclicityChevalleyConstants(int indexI, int indexJ);
  void exploitSymmetryChevalleyConstants(int indexI, int indexJ);
  void exploitTheCyclicTrick(int i, int j, int k);
  bool getMaxQForWhichBetaMinusQAlphaisARoot(const Vector<Rational>& alpha, const Vector<Rational>& beta, int& output) const;
  Rational getConstant(const Vector<Rational>& root1, const Vector<Rational>& root2);
  Vector<Rational> getWeightOfGenerator(int index);
  //returns true if returning constant, false if returning element of h
  bool getConstantOrHElement(
    const Vector<Rational>& root1, const Vector<Rational>& root2, Rational& outputRat, Vector<Rational>& outputH
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
  static void findSl2Subalgebras(
    SemisimpleLieAlgebra& inputOwner,
    SlTwoSubalgebras& output,
    bool computeRealForm,
    AlgebraicClosureRationals* algebraicClosure
  );
  template<class Coefficient>
  void getAdjoint(Matrix<Coefficient>& output, ElementSemisimpleLieAlgebra<Coefficient>& e);
  template<class Coefficient>
  void getAdjoint(MatrixTensor<Coefficient>& output, ElementSemisimpleLieAlgebra<Coefficient>& e);
  void makeChevalleyTestReport(int i, int j, int k, int Total);
  bool isInTheWeightSupport(Vector<Rational>& weight, Vector<Rational>& highestWeight);
  void createEmbeddingFromFDModuleHaving1dimWeightSpaces(Vector<Rational>& highestWeight);
  int getLengthStringAlongAlphaThroughBeta(
    Vector<Rational>& alpha,
    Vector<Rational>& beta,
    int& distanceToHighestWeight,
    Vectors<Rational>& weightSupport
  );
  void computeOneAutomorphism(Matrix<Rational>& outputAutomorphism, bool useNegativeRootsFirst);
  bool operator==(const SemisimpleLieAlgebra& other) const {
    return this->weylGroup == other.weylGroup;
  }
  // Returns the matrix realization of an element for one particular
  // preferred "standard" representation of the lie algebra.
  // As of writing, the "standard" representation is only implemented for
  // sl(n) - the natural (n x n)- matrix realization.
  template<typename Coefficient>
  bool getElementStandardRepresentation(
    const ElementSemisimpleLieAlgebra<Coefficient>& element,
    Matrix<Coefficient>& output
  );
  // Same as the previous function but for elements of the Universal Enveloping Algebra.
  template<typename Coefficient>
  bool getElementStandardRepresentation(
    const ElementUniversalEnveloping<Coefficient>& element,
    Matrix<Coefficient>& output,
    std::stringstream* commentsOnFailure
  );
  // Same as the previous function but for elements of the Universal Enveloping Algebra.
  template<typename Coefficient>
  bool getElementAdjointRepresentation(
    const ElementUniversalEnveloping<Coefficient>& element,
    Matrix<Coefficient>& output,
    std::stringstream* commentsOnFailure
  );
  template<typename Coefficient>
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
  // Whether the ambient Lie algebra has a Cartan involution that has been implemented.
  bool hasImplementedCartanInvolution(
    const VoganDiagram& voganDiagram,
    CartanInvolution* whichInvolution,
    std::stringstream* commentsOnFailure
  );
  template <typename Coefficient>
  bool hasImplementedCartanInvolutionMaximallyCompactCase(
    LinearMapSemisimpleLieAlgebra<Coefficient>* whichInvolution
  );
};

template <class Coefficient>
class Weight {
public:
  SemisimpleLieAlgebra* owner;
  Vector<Coefficient> weightFundamentalCoordinates;
  friend std::ostream& operator << (std::ostream& output, const Weight<Coefficient>& input) {
    output << input.toString();
    return output;
  }
  Weight(): owner(nullptr) {
  }
  void checkNonZeroOwner() const {
    if (this->owner != nullptr) {
      return;
    }
    global.fatal << "Monomial char has zero owner, "
    << "which is not allowed by the current function call. " << global.fatal;
  }
  void accountSingleWeight(
    const Vector<Rational>& currentWeightSimpleCoords,
    const Vector<Rational>& otherHighestWeightSimpleCoords,
    Rational& theMult,
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
      global.fatal << "Attempting to add weights with different owner algebras. " << global.fatal;
    }
    this->weightFundamentalCoordinates += other.weightFundamentalCoordinates;
  }
  void operator*=(const Coefficient& other) {
    this->weightFundamentalCoordinates *= other;
  }
  inline bool operator==(const Weight<Coefficient>& other) const {
    return this->weightFundamentalCoordinates == other.weightFundamentalCoordinates && this->owner == other.owner;
  }
  inline bool operator>(const Weight<Coefficient>& other) const {
    return this->weightFundamentalCoordinates > other.weightFundamentalCoordinates;
  }
};

template <class Coefficient>
class CharacterSemisimpleLieAlgebraModule : public LinearCombination<Weight<Coefficient>, Coefficient> {
  public:
  void checkConsistency() const {
    if (this->size() == 0) {
      return;
    }
    const SemisimpleLieAlgebra* owner = (*this)[0].owner;
    for (int i = 1; i < this->size(); i ++) {
      if ((*this)[i].owner != owner) {
        global.fatal << "CharacterSemisimpleLieAlgebraModule contains "
        << "elements belonging to different semisimple Lie algebras. " << global.fatal;
      }
    }
  }
  void checkNonZeroOwner() const {
    this->checkConsistency();
    if (this->getOwner() == 0) {
      global.fatal << "CharacterSemisimpleLieAlgebraModule has no owner semisimple Lie algebra, "
      << "which is not allowed at by the calling function. " << global.fatal;
    }
  }
  bool isEqualToZero() {
    return this->size() == 0;
  }
  unsigned int hashFunction() const {
    return this->hashFunction(*this);
  }
  static unsigned int hashFunction(const CharacterSemisimpleLieAlgebraModule<Coefficient>& input) {
    return input.::LinearCombination<Weight<Coefficient>, Coefficient>::hashFunction(input);
  }
  void getDual(CharacterSemisimpleLieAlgebraModule<Coefficient>& output) const;
  void makeFromWeight(const Vector<Coefficient>& inputWeightSimpleCoords, SemisimpleLieAlgebra* inputOwner);
  bool splitCharacterOverReductiveSubalgebra(
    std::string* report,
    CharacterSemisimpleLieAlgebraModule& output,
    BranchingData& inputData
  );
  bool freudenthalEvalMeDominantWeightsOnly(
    CharacterSemisimpleLieAlgebraModule<Coefficient>& outputCharOwnerSetToZero,
    int upperBoundNumDominantWeights,
    std::string* outputDetails
  );
  bool freudenthalEvaluateMeFullCharacter(
    CharacterSemisimpleLieAlgebraModule<Coefficient>& outputCharOwnerSetToZero,
    int upperBoundNumDominantWeights,
    std::string* outputDetails
  );
  std::string toStringFullCharacterWeightsTable();
  bool drawMeNoMultiplicities(std::string& outputDetails, DrawingVariables& drawingVariables, int upperBoundWeights) {
    return this->drawMe(outputDetails, drawingVariables, upperBoundWeights, false);
  }
  int getPositiveNStringSuchThatWeightMinusNAlphaIsWeight(
    const Weight<Coefficient>& weightInFundamentalCoordinates,
    const Vector<Coefficient>& alphaInFundamentalCoordinates
  );
  bool drawMeWithMultiplicities(std::string& outputDetails, DrawingVariables& drawingVariables, int upperBoundWeights) {
    return this->drawMe(outputDetails, drawingVariables, upperBoundWeights, true);
  }
  void drawMeAssumeCharIsOverCartan(WeylGroupData& actualAmbientWeyl, DrawingVariables& drawingVariables) const;
  SemisimpleLieAlgebra* getOwner() const {
    if (this->size() == 0) {
      global.fatal << "Requesting owner semisimple Lie algebra of zero character. " << global.fatal;
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
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& outputWeylSubgroup
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
    this->::LinearCombination<Weight<Coefficient>, Coefficient>::operator*=(other);
  }
};

class HomomorphismSemisimpleLieAlgebra {
public:
  SemisimpleLieAlgebra* domain;
  SemisimpleLieAlgebra* coDomain;
  // Images of g_1, g_2, ..., g_{rank}, in this order.
  List<ElementSemisimpleLieAlgebra<Rational> > imagesPositiveSimpleChevalleyGenerators;
  // Images of g_-1, g_-2, ..., g_{-rank}, in this order.
  List<ElementSemisimpleLieAlgebra<Rational> > imagesNegativeSimpleChevalleyGenerators;
  // imagesAllChevalleyGenerators are given in the same order as
  // the one used in MonomialUniversalEnveloping.
  List<ElementSemisimpleLieAlgebra<Rational> > imagesAllChevalleyGenerators;
  List<ElementSemisimpleLieAlgebra<Rational> > domainAllChevalleyGenerators;
  List<ElementSemisimpleLieAlgebra<Rational> > gModK;
  Vectors<Rational> restrictedRootSystem;
  Vectors<Rational> imagesCartanDomain;
  SemisimpleLieAlgebra& domainAlgebra() {
    if (this->domain == nullptr) {
      global.fatal << "Non-initialized HomomorphismSemisimpleLieAlgebra. " << global.fatal;
    }
    return *this->domain;
  }
  SemisimpleLieAlgebra& coDomainAlgebra() {
    if (this->coDomain == nullptr) {
      global.fatal << "Non-initialized HomomorphismSemisimpleLieAlgebra. " << global.fatal;
    }
    return *this->coDomain;
  }
  HomomorphismSemisimpleLieAlgebra(): domain(nullptr), coDomain(nullptr) {
  }
  void getWeightsGmodKInSimpleCoordinatesK(Vectors<Rational>& outputWeights) {
    this->getWeightsRelativeToKInSimpleKCoordinates(outputWeights, this->gModK);
  }
  void getWeightsRelativeToKInSimpleKCoordinates(
    Vectors<Rational>& outputWeights, List<ElementSemisimpleLieAlgebra<Rational> >& inputElements
  );
  void toString(std::string& output) {
    this->toString(output, false);
  }
  void toString(std::string& output, bool useHtml);
  void makeGInGWithIdentity(SemisimpleLieAlgebra& owner);
  void projectOntoSmallCartan(Vector<Rational>& input, Vector<Rational> & output);
  void projectOntoSmallCartan(Vectors<Rational>& input, Vectors<Rational>& output);
  void getMapSmallCartanDualToLargeCartanDual(Matrix<Rational> & output);
  std::string toString(bool useHtml = true);
  void getRestrictionAmbientRootSystemToTheSmallerCartanSubalgebra(Vectors<Rational>& output);
  bool computeHomomorphismFromImagesSimpleChevalleyGenerators(std::stringstream* commentsOnFailure);
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
  bool computeSimpleRootImagesTypeAI(
    std::stringstream* commentsOnFailure
  );
  bool computeSimpleRootImagesTypeAII(
    std::stringstream* commentsOnFailure
  );
  bool computeSimpleRootImagesTypeAIII(
    std::stringstream* commentsOnFailure
  );
  bool computeSimpleRootImagesTypeBI(
    std::stringstream* commentsOnFailure
  );
  bool computeSimpleRootImagesTypeCI(
    std::stringstream* commentsOnFailure
  );
  bool computeSimpleRootImagesTypeD(
    std::stringstream* commentsOnFailure,
    bool useAutomorphism
  );
  bool computeSimpleRootImagesTypeDI(
    std::stringstream* commentsOnFailure
  );
  bool computeSimpleRootImagesTypeDII(
    std::stringstream* commentsOnFailure
  );
  bool computeSimpleRootImagesTypeEI(
    std::stringstream* commentsOnFailure
  );
  bool computeSimpleRootImagesTypeEII(
    std::stringstream* commentsOnFailure
  );
  bool computeSimpleRootImagesTypeEIII(
    std::stringstream* commentsOnFailure
  );
  bool computeSimpleRootImagesTypeEIV(
    std::stringstream* commentsOnFailure
  );
  bool computeSimpleRootImages(
    std::stringstream* commentsOnFailure
  );
  bool computeFromDiagram(
    const VoganDiagram& inputDiagram,
    SemisimpleLieAlgebra& inputOwner,
    std::stringstream* commentsOnFailure
  );
  void plotVoganDiagram(Plot& output);
  void plotSatakeDiagram(Plot& output, int verticalOffset);
};

#endif
