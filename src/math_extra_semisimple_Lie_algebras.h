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
  WeylGroupData theWeyl;
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
  bool HasComputedStructureConstants();
  unsigned int hashFunction() const {
    return this->hashFunction(*this);
  }
  static unsigned int hashFunction(const SemisimpleLieAlgebra& input) {
    return input.theWeyl.hashFunction();
  }
  std::string ToStringDisplayFolderName(const std::string& baseFolder) const;
  std::string ToStringDisplayFileNameWithPathStructureConstants(const std::string& baseFolder) const;
  std::string ToStringVirtualFolderName() const;
  std::string ToStringVirtualFileNameWithPathStructureConstants() const;
  std::string ToStringFileNameNoPathStructureConstants() const;
  std::string ToStringFileNameNoPathSemisimpleSubalgebras() const;
  std::string ToStringFileNameNoPathRootSubalgebras() const;
  std::string ToStringFileNameRelativePathSlTwoSubalgebras() const;
  template <class Coefficient>
  void GetGenericElementCartan(ElementSemisimpleLieAlgebra<Polynomial<Coefficient> >& output, int indexFirstVar = 0) {
    output.makeZero();
    ChevalleyGenerator theGen;
    Polynomial<Coefficient> theCf;
    for (int i = 0; i < this->GetRank(); i ++) {
      theGen.MakeGenerator(*this, this->GetCartanGeneratorIndex(i));
      theCf.makeMonomial(indexFirstVar + i, 1, 1);
      output.addMonomial(theGen, theCf);
    }
  }
  template <class Coefficient>
  void GetGenericElementNegativeBorelNilradical(
    ElementSemisimpleLieAlgebra<Polynomial<Coefficient> >& output, int indexFirstVar = 0
  ) {
    output.makeZero();
    ChevalleyGenerator theGen;
    Polynomial<Coefficient> theCf;
    for (int i = 0; i < this->GetNumPosRoots(); i ++) {
      theGen.MakeGenerator(*this, i);
      theCf.makeMonomial(indexFirstVar + i, 1, 1);
      output.addMonomial(theGen, theCf);
    }
  }
  int GetOppositeGeneratorIndex(int theIndex) {
    return this->GetNumGenerators() - theIndex - 1;
  }
  bool IsASimpleGenerator(int generatorIndex) {
    bool isNegativeGenerator = (generatorIndex < this->GetNumPosRoots()) &&
    (generatorIndex >= this->GetNumPosRoots() - this->GetRank());
    bool isPositiveGenerator = (generatorIndex >= this->GetNumPosRoots() + this->GetRank()) &&
    (generatorIndex < this->GetNumPosRoots() + this->GetRank() * 2);
    return isNegativeGenerator || isPositiveGenerator;
  }
  SemisimpleLieAlgebra() {
    this->flagHasNilradicalOrder = false;
    this->flagDeallocated = false;
  }
  ~SemisimpleLieAlgebra() {
    this->flagDeallocated = true;
  }
  bool checkConsistency() const;
  template <class Coefficient>
  void GenerateLieSubalgebra(List<ElementSemisimpleLieAlgebra<Coefficient> >& inputOutputGenerators);
  void ComputeMultTable();
  bool IsOfSimpleType(char desiredType, int desiredRank) const {
    return this->theWeyl.IsOfSimpleType(desiredType, desiredRank);
  }
  template <class Coefficient>
  void GetCommonCentralizer(
    const List<ElementSemisimpleLieAlgebra<Coefficient> >& inputElementsToCentralize,
    List<ElementSemisimpleLieAlgebra<Coefficient> >& outputCentralizingElements
  );
  void GetChevalleyGeneratorAsLieBracketsSimpleGens(
    int generatorIndex, List<int>& outputIndicesFormatAd0Ad1Ad2etc, Rational& outputMultiplyLieBracketsToGetGenerator
  );
  std::string toString(FormatExpressions* inputFormat = nullptr);
  std::string ToStringHTMLMenuStructureSummary(
    const std::string& relativeTo,
    bool includeStructureConstants,
    bool includeRootSubalgebras,
    bool includeSl2Subalgebras,
    bool includeSemisimpleSubalgebras
  );

  std::string ToStringMenuStructurePages(
    const std::string& relativeTo,
    bool includeStructureConstants,
    bool includeRootSubalgebras,
    bool includeSl2Subalgebras,
    bool includeSemisimpleSubalgebras
  ) const;
  std::string ToHTMLCalculator(bool Verbose, bool writeToHD, bool flagWriteLatexPlots);
  std::string GetStringFromChevalleyGenerator(int theIndex, FormatExpressions* thePolynomialFormat) const;
  bool CommutatorIsNonZero(int leftIndex, int rightIndex) {
    return !this->theLiebrackets.elements[leftIndex][rightIndex].isEqualToZero();
  }
  std::string ToStringLieAlgebraNameFullHTML() const;
  std::string ToStringLieAlgebraName() const;
  std::string ToStringLieAlgebraNameNonTechnicalHTML() const;
  void GetMinusTransposeAuto(const ElementSemisimpleLieAlgebra<Rational>& input, ElementSemisimpleLieAlgebra<Rational>& output);
  void GenerateWeightSupportMethod2(Vector<Rational>& theHighestWeight, Vectors<Rational>& output);
  inline int GetNumGenerators() const {
    return this->theWeyl.cartanSymmetric.numberOfRows + this->theWeyl.RootSystem.size;
  }
  inline int GetNumPosRoots() const {
    return this->theWeyl.RootsOfBorel.size;
  }
  inline int GetRank() const {
    return this->theWeyl.cartanSymmetric.numberOfRows;
  }
  void OrderNilradical(const Selection& parSelZeroMeansLeviPart, bool useNilWeight, bool ascending);
  void OrderNilradicalFirstTotalWeightAscending(const Selection& parSelZeroMeansLeviPart);
  void OrderNilradicalFirstTotalWeightDescending(const Selection& parSelZeroMeansLeviPart);
  void OrderNilradicalNilWeightAscending(const Selection& parSelZeroMeansLeviPart);
  void OrderNilradicalNilWeightDescending(const Selection& parSelZeroMeansLeviPart);
  void OrderStandardAscending();
  void OrderStandardDescending();

  void OrderSSalgebraForHWbfComputation();
  int GetCartanGeneratorIndex(int simpleRootIndex) {
    return this->theWeyl.RootsOfBorel.size + simpleRootIndex;
  }
  int GetGeneratorFromRoot(const Vector<Rational>& input) {
    return this->GetGeneratorFromRootIndex(this->theWeyl.RootSystem.getIndex(input));
  }
  int GetRootIndexFromDisplayIndex(int theIndex);
  int GetGeneratorFromDisplayIndex(int theIndex) {
    if (theIndex < 0) {
      return theIndex + this->GetNumPosRoots();
    }
    return theIndex + this->GetNumPosRoots() + this->GetRank() - 1;
  }
  int GetGeneratorFromRootIndex(int theIndex) const;
  int GetDisplayIndexFromRootIndex(int theIndex) const;
  //the below function returns an negative number if the chevalley generator is an element of the Cartan subalgebra
  int GetRootIndexFromGenerator(int theIndex) const;
  int GetCartanIndexFromGenerator(int theIndex) {
    return theIndex + this->theWeyl.RootsOfBorel.size;
  }
  int GetDisplayIndexFromGenerator(int theIndex) const {
    if (theIndex < this->GetNumPosRoots()) {
      return theIndex - this->GetNumPosRoots();
    }
    if (theIndex >= this->GetNumPosRoots() + this->GetRank()) {
      return theIndex + 1 - this->GetNumPosRoots() - this->GetRank();
    }
    return theIndex - this->GetNumPosRoots();
  }
  bool AreOrderedProperly(int leftIndex, int rightIndex);
  bool IsGeneratorFromCartan(int theIndex) const {
    return theIndex >= this->GetNumPosRoots() && theIndex < this->GetNumPosRoots() + this->GetRank();
  }
  bool AreOppositeRootSpaces(int leftIndex, int rightIndex) {
    if (this->IsGeneratorFromCartan(leftIndex) || this->IsGeneratorFromCartan(rightIndex)) {
      return false;
    }
    int left = this->GetRootIndexFromGenerator(leftIndex);
    int right = this->GetRootIndexFromGenerator(rightIndex);
    return (this->theWeyl.RootSystem[left] + this->theWeyl.RootSystem[right]).isEqualToZero();
  }
  void GenerateVermaMonomials(Vector<Rational>& highestWeight);
  void ComputeChevalleyConstants();
  template<class Coefficient>
  Coefficient getKillingForm(
    const ElementSemisimpleLieAlgebra<Coefficient>& left, const ElementSemisimpleLieAlgebra<Coefficient>& right
  );
  template <class Coefficient>
  Coefficient GetKillingFormProductWRTLevi(
    const ElementSemisimpleLieAlgebra<Coefficient>& left,
    const ElementSemisimpleLieAlgebra<Coefficient>& right,
    const Selection& rootsNotInLevi
  );
  template<class Coefficient>
  void LieBracket(
    const ElementSemisimpleLieAlgebra<Coefficient>& g1,
    const ElementSemisimpleLieAlgebra<Coefficient>& g2,
    ElementSemisimpleLieAlgebra<Coefficient>& output
  );
  //Setup: \gamma +\delta =\epsilon +\zeta =\eta is a Vector<Rational> .
  //then the below function computes n_{-\epsilon, -\zeta}
  void ComputeOneChevalleyConstant(int indexGamma, int indexDelta, int indexMinusEpsilon, int indexMinusZeta, int indexEta);
  void ExploitSymmetryAndCyclicityChevalleyConstants(int indexI, int indexJ);
  void ExploitSymmetryChevalleyConstants(int indexI, int indexJ);
  void ExploitTheCyclicTrick(int i, int j, int k);
  bool GetMaxQForWhichBetaMinusQAlphaIsARoot(const Vector<Rational>& alpha, const Vector<Rational>& beta, int& output) const;
  Rational GetConstant(const Vector<Rational>& root1, const Vector<Rational>& root2);
  bool CheckClosedness(std::string& output);
  void ElementToStringVermaMonomials(std::string& output);
  void ElementToStringEmbedding(std::string& output);
  Vector<Rational> GetWeightOfGenerator(int index) {
    if (index < this->GetNumPosRoots()) {
      return this->theWeyl.RootSystem[index];
    }
    if (index >= this->GetRank() + this->GetNumPosRoots()) {
      return this->theWeyl.RootSystem[index - this->GetRank()];
    }
    Vector<Rational> result;
    result.makeZero(this->GetRank());
    return result;
  }
  //returns true if returning constant, false if returning element of h
  bool GetConstantOrHElement(
    const Vector<Rational>& root1, const Vector<Rational>& root2, Rational& outputRat, Vector<Rational>& outputH
  );
  bool TestForConsistency();
  bool AttempTFindingHEF(
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
  bool AttemptExtendingHtoHEFwithHinCartan(
    ElementSemisimpleLieAlgebra<Rational>& theH,
    ElementSemisimpleLieAlgebra<Rational>& outputE,
    ElementSemisimpleLieAlgebra<Rational>& outputF
  );
  static void FindSl2Subalgebras(SemisimpleLieAlgebra& inputOwner, SltwoSubalgebras& output);
  void GetSl2SubalgebraFromRootSA();
  template<class Coefficient>
  void GetAd(Matrix<Coefficient>& output, ElementSemisimpleLieAlgebra<Coefficient>& e);
  template<class Coefficient>
  void GetAd(MatrixTensor<Coefficient>& output, ElementSemisimpleLieAlgebra<Coefficient>& e);
  void MakeChevalleyTestReport(int i, int j, int k, int Total);
  bool IsInTheWeightSupport(Vector<Rational>& theWeight, Vector<Rational>& highestWeight);
  void GenerateOneMonomialPerWeightInTheWeightSupport(Vector<Rational>& theHighestWeight);
  void CreateEmbeddingFromFDModuleHaving1dimWeightSpaces(Vector<Rational>& theHighestWeight);
  int GetLengthStringAlongAlphaThroughBeta(
    Vector<Rational>& alpha, Vector<Rational>& beta, int& distanceToHighestWeight, Vectors<Rational>& weightSupport
  );
  void ComputeOneAutomorphism(Matrix<Rational>& outputAuto,  bool useNegativeRootsFirst);
  bool operator==(const SemisimpleLieAlgebra& other) const {
    return this->theWeyl == other.theWeyl;
  }
};

template <class Coefficient>
class Weight {
public:
  SemisimpleLieAlgebra* owner;
  Vector<Coefficient> weightFundamentalCoordS;
  friend std::ostream& operator << (std::ostream& output, const Weight<Coefficient>& input) {
    output << input.toString();
    return output;
  }
  Weight(): owner(nullptr) {
  }
  void CheckNonZeroOwner() const {
    if (this->owner != nullptr) {
      return;
    }
    global.fatal << "This is a programming error: Monomial char has zero owner, "
    << "which is not allowed by the current function call. " << global.fatal;
  }
  void AccountSingleWeight(
    const Vector<Rational>& currentWeightSimpleCoords,
    const Vector<Rational>& otherHighestWeightSimpleCoords,
    Rational& theMult,
    CharacterSemisimpleLieAlgebraModule<Coefficient>& outputAccum
  ) const;
  std::string TensorAndDecompose(const Weight<Coefficient>& other, CharacterSemisimpleLieAlgebraModule<Coefficient>& output) const;
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  inline unsigned int hashFunction() const {
    return weightFundamentalCoordS.hashFunction();
  }
  static inline unsigned int hashFunction(const Weight<Coefficient>& input) {
    return input.hashFunction();
  }
  void operator+=(const Weight<Coefficient>& other) {
    if (this->owner != other.owner) {
      global.fatal << "Attempting to add weights with different owner algebras. " << global.fatal;
    }
    this->weightFundamentalCoordS += other.weightFundamentalCoordS;
  }
  void operator*=(const Coefficient& other) {
    this->weightFundamentalCoordS *= other;
  }
  inline bool operator==(const Weight<Coefficient>& other) const {
    return this->weightFundamentalCoordS == other.weightFundamentalCoordS && this->owner == other.owner;
  }
  inline bool operator>(const Weight<Coefficient>& other) const {
    return this->weightFundamentalCoordS > other.weightFundamentalCoordS;
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
        global.fatal << "This is a programming error: CharacterSemisimpleLieAlgebraModule contains "
        << "elements belonging to different semisimple Lie algebras. " << global.fatal;
      }
    }
  }
  void CheckNonZeroOwner() const {
    this->checkConsistency();
    if (this->GetOwner() == 0) {
      global.fatal << "This is a programming error: CharacterSemisimpleLieAlgebraModule has no owner semisimple Lie algebra, "
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
  void GetDual(CharacterSemisimpleLieAlgebraModule<Coefficient>& output) const;
  void MakeFromWeight(const Vector<Coefficient>& inputWeightSimpleCoords, SemisimpleLieAlgebra* inputOwner);
  bool SplitCharOverRedSubalg(
    std::string* report,
    CharacterSemisimpleLieAlgebraModule& output,
    BranchingData& inputData
  );
  bool GetDominantCharacterWRTsubalgebra(
    CharacterSemisimpleLieAlgebraModule& outputCharOwnerSetToZero, std::string& outputDetails, int upperBoundNumDominantWeights
  );
  bool FreudenthalEvalMeDominantWeightsOnly(
    CharacterSemisimpleLieAlgebraModule<Coefficient>& outputCharOwnerSetToZero, int upperBoundNumDominantWeights, std::string* outputDetails
  );
  bool FreudenthalEvalMeFullCharacter(
    CharacterSemisimpleLieAlgebraModule<Coefficient>& outputCharOwnerSetToZero, int upperBoundNumDominantWeights, std::string* outputDetails
  );
  std::string toStringFullCharacterWeightsTable();
  bool DrawMeNoMults(std::string& outputDetails, DrawingVariables& theDrawingVars, int upperBoundWeights) {
    return this->DrawMe(outputDetails, theDrawingVars, upperBoundWeights, false);
  }
  int GetPosNstringSuchThatWeightMinusNalphaIsWeight(
    const Weight<Coefficient>& theWeightInFundCoords, const Vector<Coefficient>& theAlphaInFundCoords
  );
  bool DrawMeWithMults(std::string& outputDetails, DrawingVariables& theDrawingVars, int upperBoundWeights) {
    return this->DrawMe(outputDetails, theDrawingVars, upperBoundWeights, true);
  }
  void DrawMeAssumeCharIsOverCartan(WeylGroupData& actualAmbientWeyl, DrawingVariables& theDrawingVars) const;
  SemisimpleLieAlgebra* GetOwner() const {
    if (this->size() == 0) {
      global.fatal << "This is a programming error: requesting owner semisimple Lie algebra of zero character. " << global.fatal;
    }
    return (*this)[0].owner;
  }
  bool DrawMe(std::string& outputDetails, DrawingVariables& theDrawingVars, int upperBoundWeights, bool useMults);
  bool SplitOverLeviMonsEncodeHIGHESTWeight(
    std::string* report,
    CharacterSemisimpleLieAlgebraModule& output,
    const Selection& splittingParSel,
    const Selection& ParSelFDInducingPart,
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& outputWeylSub
  );
  int GetIndexExtremeWeightRelativeToWeyl(WeylGroupData& theWeyl) const;
  void MakeTrivial(SemisimpleLieAlgebra& inputOwner);
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
      global.fatal << "This is a programming error: non-initialized HomomorphismSemisimpleLieAlgebra. " << global.fatal;
    }
    return *this->domainAlg;
  }
  SemisimpleLieAlgebra& theRange() {
    if (this->rangeAlg == nullptr) {
      global.fatal << "This is a programming error: non-initialized HomomorphismSemisimpleLieAlgebra. " << global.fatal;
    }
    return *this->rangeAlg;
  }
  HomomorphismSemisimpleLieAlgebra(): domainAlg(nullptr), rangeAlg(nullptr) {
  }
  std::string DebugString;
  void GetWeightsGmodKInSimpleCoordsK(Vectors<Rational>& outputWeights) {
    this->GetWeightsWrtKInSimpleCoordsK(outputWeights, this->GmodK);
  }
  void GetWeightsKInSimpleCoordsK(Vectors<Rational>& outputWeights) {
    this->GetWeightsWrtKInSimpleCoordsK(outputWeights, this->imagesAllChevalleyGenerators);
  }
  void GetWeightsWrtKInSimpleCoordsK(
    Vectors<Rational>& outputWeights, List<ElementSemisimpleLieAlgebra<Rational> >& inputElts
  );
  void toString(std::string& output) {
    this->toString(output, false);
  }
  void toString(std::string& output, bool useHtml);
  void MakeGinGWithId(
    char theWeylLetter, int theWeylDim, MapReferences<DynkinType, SemisimpleLieAlgebra>& ownerOfAlgebras
  );
  void ProjectOntoSmallCartan(Vector<Rational>& input, Vector<Rational> & output);
  void ProjectOntoSmallCartan(Vectors<Rational>& input, Vectors<Rational>& output);
  void GetMapSmallCartanDualToLargeCartanDual(Matrix<Rational> & output);
  void ComputeDebugString() {
    this->toString(this->DebugString);
  }
  void ComputeDebugString(bool useHtml) {
    this->toString(this->DebugString, useHtml);
  }
  std::string toString() {
    std::string tempS;
    this->toString(tempS);
    return tempS;
  }
  void GetRestrictionAmbientRootSystemToTheSmallerCartanSA(Vectors<Rational>& output);
  bool ComputeHomomorphismFromImagesSimpleChevalleyGenerators();
  bool CheckClosednessLieBracket();
  void applyHomomorphism(const ElementSemisimpleLieAlgebra<Rational>& input, ElementSemisimpleLieAlgebra<Rational>& output);
  bool applyHomomorphism(
    const ElementUniversalEnveloping<RationalFunction>& input, ElementUniversalEnveloping<RationalFunction>& output
  );
  bool applyHomomorphism(
    const MonomialUniversalEnveloping<RationalFunction>& input,
    const RationalFunction& theCoeff,
    ElementUniversalEnveloping<RationalFunction>& output
  );
};

#endif
