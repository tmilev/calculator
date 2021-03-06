// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeaderSemisimpleLieAlgebrasIncluded
#define vpfHeaderSemisimpleLieAlgebrasIncluded

#include "math_extra_finite_groups.h"

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
  //  this->theWyl.RootSystem and \beta -
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
  Matrix<Rational> ChevalleyConstants;
  Matrix<bool> Computed;
  //
  // Matrix<int> theLiebracketPairingIndices;
  Matrix<ElementSemisimpleLieAlgebra<Rational> > theLiebrackets;
  // List<int> OppositeRootSpaces;
  List<int> UEGeneratorOrderIncludingCartanElts;
  // std::string pathVirtualNameOutput;
  // std::string pathRelativePhysicalOutput;

  bool flagHasNilradicalOrder;
  bool flagDeallocated;
  bool hasComputedStructureConstants();
  unsigned int hashFunction() const {
    return this->hashFunction(*this);
  }
  static unsigned int hashFunction(const SemisimpleLieAlgebra& input) {
    return input.weylGroup.hashFunction();
  }
  std::string toStringDisplayFolderName(const std::string& baseFolder) const;
  std::string toStringDisplayFileNameWithPathStructureConstants(const std::string& baseFolder) const;
  std::string toStringVirtualFolderName() const;
  std::string toStringVirtualFileNameWithPathStructureConstants() const;
  std::string toStringFileNameNoPathStructureConstants() const;
  std::string toStringFileNameNoPathSemisimpleSubalgebras() const;
  std::string toStringFileNameNoPathRootSubalgebras() const;
  std::string toStringFileNameRelativePathSlTwoSubalgebras() const;
  template <class Coefficient>
  void getGenericElementCartan(ElementSemisimpleLieAlgebra<Polynomial<Coefficient> >& output, int indexFirstVar = 0) {
    output.makeZero();
    ChevalleyGenerator theGen;
    Polynomial<Coefficient> theCf;
    for (int i = 0; i < this->getRank(); i ++) {
      theGen.makeGenerator(*this, this->getCartanGeneratorIndex(i));
      theCf.makeMonomial(indexFirstVar + i, 1, 1);
      output.addMonomial(theGen, theCf);
    }
  }
  template <class Coefficient>
  void getGenericElementNegativeBorelNilradical(
    ElementSemisimpleLieAlgebra<Polynomial<Coefficient> >& output, int indexFirstVar = 0
  ) {
    output.makeZero();
    ChevalleyGenerator theGen;
    Polynomial<Coefficient> theCf;
    for (int i = 0; i < this->getNumberOfPositiveRoots(); i ++) {
      theGen.makeGenerator(*this, i);
      theCf.makeMonomial(indexFirstVar + i, 1, 1);
      output.addMonomial(theGen, theCf);
    }
  }
  int getOppositeGeneratorIndex(int theIndex) {
    return this->getNumberOfGenerators() - theIndex - 1;
  }
  bool isSimpleGenerator(int generatorIndex);
  SemisimpleLieAlgebra() {
    this->flagHasNilradicalOrder = false;
    this->flagDeallocated = false;
  }
  ~SemisimpleLieAlgebra() {
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
    int generatorIndex, List<int>& outputIndicesFormatAd0Ad1Ad2etc, Rational& outputMultiplyLieBracketsToGetGenerator
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
  void writeHTML(bool verbose, bool flagWriteLatexPlots);
  std::string toHTML(bool verbose, bool flagWriteLatexPlots);
  static std::string toHTMLCalculatorHeadElements(
    const std::string& relativeTo = "../../.."
  );
  static std::string toHTMLCalculatorBodyOnload();
  std::string getStringFromChevalleyGenerator(int theIndex, FormatExpressions* thePolynomialFormat) const;
  std::string toStringLieAlgebraNameFullHTML() const;
  std::string toStringLieAlgebraName() const;
  std::string toStringLieAlgebraNameNonTechnicalHTML() const;
  inline int getNumberOfGenerators() const {
    return this->weylGroup.cartanSymmetric.numberOfRows + this->weylGroup.rootSystem.size;
  }
  inline int getNumberOfPositiveRoots() const {
    return this->weylGroup.rootsOfBorel.size;
  }
  inline int getRank() const {
    return this->weylGroup.cartanSymmetric.numberOfRows;
  }
  void orderNilradical(const Selection& parSelZeroMeansLeviPart, bool useNilWeight, bool ascending);
  void orderNilradicalFirstTotalWeightAscending(const Selection& parSelZeroMeansLeviPart);
  void orderNilradicalFirstTotalWeightDescending(const Selection& parSelZeroMeansLeviPart);
  void orderNilradicalNilWeightAscending(const Selection& parSelZeroMeansLeviPart);
  void orderNilradicalNilWeightDescending(const Selection& parSelZeroMeansLeviPart);
  void orderStandardAscending();
  void orderStandardDescending();

  void orderSSalgebraForHWbfComputation();
  int getCartanGeneratorIndex(int simpleRootIndex) {
    return this->weylGroup.rootsOfBorel.size + simpleRootIndex;
  }
  int getGeneratorFromRoot(const Vector<Rational>& input) {
    return this->getGeneratorFromRootIndex(this->weylGroup.rootSystem.getIndex(input));
  }
  int getRootIndexFromDisplayIndex(int theIndex);
  int getGeneratorFromDisplayIndex(int theIndex) {
    if (theIndex < 0) {
      return theIndex + this->getNumberOfPositiveRoots();
    }
    return theIndex + this->getNumberOfPositiveRoots() + this->getRank() - 1;
  }
  int getGeneratorFromRootIndex(int theIndex) const;
  int GetDisplayIndexFromRootIndex(int theIndex) const;
  //the below function returns an negative number if the chevalley generator is an element of the Cartan subalgebra
  int getRootIndexFromGenerator(int theIndex) const;
  int getCartanIndexFromGenerator(int theIndex) {
    return theIndex + this->weylGroup.rootsOfBorel.size;
  }
  int getDisplayIndexFromGenerator(int theIndex) const {
    if (theIndex < this->getNumberOfPositiveRoots()) {
      return theIndex - this->getNumberOfPositiveRoots();
    }
    if (theIndex >= this->getNumberOfPositiveRoots() + this->getRank()) {
      return theIndex + 1 - this->getNumberOfPositiveRoots() - this->getRank();
    }
    return theIndex - this->getNumberOfPositiveRoots();
  }
  bool areOrderedProperly(int leftIndex, int rightIndex);
  bool isGeneratorFromCartan(int theIndex) const {
    return theIndex >= this->getNumberOfPositiveRoots() && theIndex < this->getNumberOfPositiveRoots() + this->getRank();
  }
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
    const ElementSemisimpleLieAlgebra<Coefficient>& left, const ElementSemisimpleLieAlgebra<Coefficient>& right
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
  //Setup: \gamma +\delta =\epsilon +\zeta =\eta is a Vector<Rational> .
  //then the below function computes n_{-\epsilon, -\zeta}
  void computeOneChevalleyConstant(int indexGamma, int indexDelta, int indexMinusEpsilon, int indexMinusZeta, int indexEta);
  void exploitSymmetryAndCyclicityChevalleyConstants(int indexI, int indexJ);
  void exploitSymmetryChevalleyConstants(int indexI, int indexJ);
  void exploitTheCyclicTrick(int i, int j, int k);
  bool getMaxQForWhichBetaMinusQAlphaisARoot(const Vector<Rational>& alpha, const Vector<Rational>& beta, int& output) const;
  Rational getConstant(const Vector<Rational>& root1, const Vector<Rational>& root2);
  Vector<Rational> getWeightOfGenerator(int index) {
    if (index < this->getNumberOfPositiveRoots()) {
      return this->weylGroup.rootSystem[index];
    }
    if (index >= this->getRank() + this->getNumberOfPositiveRoots()) {
      return this->weylGroup.rootSystem[index - this->getRank()];
    }
    Vector<Rational> result;
    result.makeZero(this->getRank());
    return result;
  }
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
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& theE,
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& outputH,
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& outputF,
    std::stringstream* logStream = nullptr
  );
  static void findSl2Subalgebras(SemisimpleLieAlgebra& inputOwner, SlTwoSubalgebras& output);
  void GetSl2SubalgebraFromRootSA();
  template<class Coefficient>
  void getAdjoint(Matrix<Coefficient>& output, ElementSemisimpleLieAlgebra<Coefficient>& e);
  template<class Coefficient>
  void getAdjoint(MatrixTensor<Coefficient>& output, ElementSemisimpleLieAlgebra<Coefficient>& e);
  void makeChevalleyTestReport(int i, int j, int k, int Total);
  bool isInTheWeightSupport(Vector<Rational>& theWeight, Vector<Rational>& highestWeight);
  void createEmbeddingFromFDModuleHaving1dimWeightSpaces(Vector<Rational>& theHighestWeight);
  int getLengthStringAlongAlphaThroughBeta(
    Vector<Rational>& alpha, Vector<Rational>& beta, int& distanceToHighestWeight, Vectors<Rational>& weightSupport
  );
  void computeOneAutomorphism(Matrix<Rational>& outputAuto,  bool useNegativeRootsFirst);
  bool operator==(const SemisimpleLieAlgebra& other) const {
    return this->weylGroup == other.weylGroup;
  }
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
    CharacterSemisimpleLieAlgebraModule<Coefficient>& outputCharOwnerSetToZero, int upperBoundNumDominantWeights, std::string* outputDetails
  );
  bool freudenthalEvaluateMeFullCharacter(
    CharacterSemisimpleLieAlgebraModule<Coefficient>& outputCharOwnerSetToZero, int upperBoundNumDominantWeights, std::string* outputDetails
  );
  std::string toStringFullCharacterWeightsTable();
  bool drawMeNoMultiplicities(std::string& outputDetails, DrawingVariables& theDrawingVars, int upperBoundWeights) {
    return this->drawMe(outputDetails, theDrawingVars, upperBoundWeights, false);
  }
  int getPositiveNStringSuchThatWeightMinusNAlphaIsWeight(
    const Weight<Coefficient>& theWeightInFundCoords, const Vector<Coefficient>& theAlphaInFundCoords
  );
  bool drawMeWithMultiplicities(std::string& outputDetails, DrawingVariables& theDrawingVars, int upperBoundWeights) {
    return this->drawMe(outputDetails, theDrawingVars, upperBoundWeights, true);
  }
  void drawMeAssumeCharIsOverCartan(WeylGroupData& actualAmbientWeyl, DrawingVariables& theDrawingVars) const;
  SemisimpleLieAlgebra* getOwner() const {
    if (this->size() == 0) {
      global.fatal << "Requesting owner semisimple Lie algebra of zero character. " << global.fatal;
    }
    return (*this)[0].owner;
  }
  bool drawMe(std::string& outputDetails, DrawingVariables& theDrawingVars, int upperBoundWeights, bool useMults);
  bool splitOverLeviMonomialsEncodeHighestWeight(
    std::string* report,
    CharacterSemisimpleLieAlgebraModule& output,
    const Selection& splittingParSel,
    const Selection& ParSelFDInducingPart,
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& outputWeylSub
  );
  int getIndexExtremeWeightRelativeToWeyl(WeylGroupData& theWeyl) const;
  void makeTrivial(SemisimpleLieAlgebra& inputOwner);
  std::string multiplyBy(const CharacterSemisimpleLieAlgebraModule& other);
  std::string operator*=(const CharacterSemisimpleLieAlgebraModule& other);
  std::string operator*=(const Weight<Rational>& other);
  CharacterSemisimpleLieAlgebraModule<Coefficient> operator-(const CharacterSemisimpleLieAlgebraModule<Coefficient>& right) const {
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
  SemisimpleLieAlgebra* domainAlg;
  SemisimpleLieAlgebra* rangeAlg;
  //Let rk=Rank(Domain)
  //format of ImagesSimpleChevalleyGenerators: the first rk elements give
  //the images of the Chevalley generators corresponding to simple positive roots
  //the second rk elements give the images of the Chevalley generators corresponding to simple
  //negative roots
  List<ElementSemisimpleLieAlgebra<Rational> > imagesSimpleChevalleyGenerators;
  //format of ImagesAllChevalleyGenerators: the Generators are given in the same order as
  //the one used in MonomialUniversalEnveloping
  List<ElementSemisimpleLieAlgebra<Rational> > imagesAllChevalleyGenerators;
  List<ElementSemisimpleLieAlgebra<Rational> > domainAllChevalleyGenerators;
  List<ElementSemisimpleLieAlgebra<Rational> > GmodK;
  Vectors<Rational> RestrictedRootSystem;
  Vectors<Rational> ImagesCartanDomain;
  SemisimpleLieAlgebra& theDomain() {
    if (this->domainAlg == nullptr) {
      global.fatal << "Non-initialized HomomorphismSemisimpleLieAlgebra. " << global.fatal;
    }
    return *this->domainAlg;
  }
  SemisimpleLieAlgebra& theRange() {
    if (this->rangeAlg == nullptr) {
      global.fatal << "Non-initialized HomomorphismSemisimpleLieAlgebra. " << global.fatal;
    }
    return *this->rangeAlg;
  }
  HomomorphismSemisimpleLieAlgebra(): domainAlg(nullptr), rangeAlg(nullptr) {
  }
  void getWeightsGmodKInSimpleCoordinatesK(Vectors<Rational>& outputWeights) {
    this->getWeightsWrtKInSimpleCoordinatesK(outputWeights, this->GmodK);
  }
  void getWeightsKInSimpleCoordinatesK(Vectors<Rational>& outputWeights) {
    this->getWeightsWrtKInSimpleCoordinatesK(outputWeights, this->imagesAllChevalleyGenerators);
  }
  void getWeightsWrtKInSimpleCoordinatesK(
    Vectors<Rational>& outputWeights, List<ElementSemisimpleLieAlgebra<Rational> >& inputElts
  );
  void toString(std::string& output) {
    this->toString(output, false);
  }
  void toString(std::string& output, bool useHtml);
  void makeGinGWithIdentity(
    char theWeylLetter, int theWeylDim, MapReferences<DynkinType, SemisimpleLieAlgebra>& ownerOfAlgebras
  );
  void projectOntoSmallCartan(Vector<Rational>& input, Vector<Rational> & output);
  void projectOntoSmallCartan(Vectors<Rational>& input, Vectors<Rational>& output);
  void getMapSmallCartanDualToLargeCartanDual(Matrix<Rational> & output);
  std::string toString() {
    std::string tempS;
    this->toString(tempS);
    return tempS;
  }
  void getRestrictionAmbientRootSystemToTheSmallercartanSubalgebra(Vectors<Rational>& output);
  bool computeHomomorphismFromImagesSimpleChevalleyGenerators();
  bool checkClosednessLieBracket();
  void applyHomomorphism(const ElementSemisimpleLieAlgebra<Rational>& input, ElementSemisimpleLieAlgebra<Rational>& output);
  bool applyHomomorphism(
    const ElementUniversalEnveloping<RationalFunction<Rational> >& input,
    ElementUniversalEnveloping<RationalFunction<Rational> >& output
  );
  bool applyHomomorphism(
    const MonomialUniversalEnveloping<RationalFunction<Rational> >& input,
    const RationalFunction<Rational>& theCoeff,
    ElementUniversalEnveloping<RationalFunction<Rational> >& output
  );
};

#endif
