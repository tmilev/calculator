//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#ifndef vpfHeaderSemisimpleLieAlgebrasIncluded
#define vpfHeaderSemisimpleLieAlgebrasIncluded

#include "math_extra_finite_groups.h"
static ProjectInformationInstance ProjectInfoVpfHeaderSemisimpleLieAlgebras(__FILE__, "Header, semisimple Lie algebras. ");

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
  unsigned int HashFunction() const {
    return this->HashFunction(*this);
  }
  static unsigned int HashFunction(const SemisimpleLieAlgebra& input) {
    return input.theWeyl.HashFunction();
  }
  std::string ToStringDisplayFolderName(const std::string& baseFolder) const;
  std::string ToStringDisplayFileNameWithPathStructureConstants(const std::string& baseFolder) const;
  std::string ToStringVirtualFolderName() const;
  std::string ToStringVirtualFileNameWithPathStructureConstants() const;
  std::string ToStringFileNameNoPathStructureConstants() const;
  std::string ToStringFileNameNoPathSemisimpleSubalgebras() const;
  std::string ToStringFileNameNoPathRootSubalgebras() const;
  std::string ToStringFileNameRelativePathSlTwoSubalgebras() const;
  template <class coefficient>
  void GetGenericElementCartan(ElementSemisimpleLieAlgebra<Polynomial<coefficient> >& output, int indexFirstVar = 0) {
    output.MakeZero();
    ChevalleyGenerator theGen;
    Polynomial<coefficient> theCf;
    for (int i = 0; i < this->GetRank(); i ++) {
      theGen.MakeGenerator(*this, this->GetCartanGeneratorIndex(i));
      theCf.MakeMonomiaL(indexFirstVar + i, 1, 1);
      output.AddMonomial(theGen, theCf);
    }
  }
  template <class coefficient>
  void GetGenericElementNegativeBorelNilradical(
    ElementSemisimpleLieAlgebra<Polynomial<coefficient> >& output, int indexFirstVar = 0
  ) {
    output.MakeZero();
    ChevalleyGenerator theGen;
    Polynomial<coefficient> theCf;
    for (int i = 0; i < this->GetNumPosRoots(); i ++) {
      theGen.MakeGenerator(*this, i);
      theCf.MakeMonomiaL(indexFirstVar + i, 1, 1);
      output.AddMonomial(theGen, theCf);
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
  bool CheckConsistency() const;
  template <class coefficient>
  void GenerateLieSubalgebra(List<ElementSemisimpleLieAlgebra<coefficient> >& inputOutputGenerators);
  void ComputeMultTable();
  bool IsOfSimpleType(char desiredType, int desiredRank) const {
    return this->theWeyl.IsOfSimpleType(desiredType, desiredRank);
  }
  template <class coefficient>
  void GetCommonCentralizer(
    const List<ElementSemisimpleLieAlgebra<coefficient> >& inputElementsToCentralize,
    List<ElementSemisimpleLieAlgebra<coefficient> >& outputCentralizingElements
  );
  void GetChevalleyGeneratorAsLieBracketsSimpleGens(
    int generatorIndex, List<int>& outputIndicesFormatAd0Ad1Ad2etc, Rational& outputMultiplyLieBracketsToGetGenerator
  );
  std::string ToString(FormatExpressions* inputFormat = nullptr);
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
    return !this->theLiebrackets.elements[leftIndex][rightIndex].IsEqualToZero();
  }
  std::string ToStringLieAlgebraNameFullHTML() const;
  std::string ToStringLieAlgebraName() const;
  std::string ToStringLieAlgebraNameNonTechnicalHTML() const;
  void GetMinusTransposeAuto(const ElementSemisimpleLieAlgebra<Rational>& input, ElementSemisimpleLieAlgebra<Rational>& output);
  void GenerateWeightSupportMethod2(Vector<Rational>& theHighestWeight, Vectors<Rational>& output);
  inline int GetNumGenerators() const {
    return this->theWeyl.CartanSymmetric.NumRows + this->theWeyl.RootSystem.size;
  }
  inline int GetNumPosRoots() const {
    return this->theWeyl.RootsOfBorel.size;
  }
  inline int GetRank() const {
    return this->theWeyl.CartanSymmetric.NumRows;
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
    return this->GetGeneratorFromRootIndex(this->theWeyl.RootSystem.GetIndex(input));
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
    return (this->theWeyl.RootSystem[left] + this->theWeyl.RootSystem[right]).IsEqualToZero();
  }
  void GenerateVermaMonomials(Vector<Rational>& highestWeight);
  void ComputeChevalleyConstants();
  template<class coefficient>
  coefficient GetKillingForm(
    const ElementSemisimpleLieAlgebra<coefficient>& left, const ElementSemisimpleLieAlgebra<coefficient>& right
  );
  template <class coefficient>
  coefficient GetKillingFormProductWRTLevi(
    const ElementSemisimpleLieAlgebra<coefficient>& left,
    const ElementSemisimpleLieAlgebra<coefficient>& right,
    const Selection& rootsNotInLevi
  );
  template<class coefficient>
  void LieBracket(
    const ElementSemisimpleLieAlgebra<coefficient>& g1,
    const ElementSemisimpleLieAlgebra<coefficient>& g2,
    ElementSemisimpleLieAlgebra<coefficient>& output
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
    result.MakeZero(this->GetRank());
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
  bool AttemptExtendingEtoHEFwithHinCartan(
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
  template<class coefficient>
  void GetAd(Matrix<coefficient>& output, ElementSemisimpleLieAlgebra<coefficient>& e);
  template<class coefficient>
  void GetAd(MatrixTensor<coefficient>& output, ElementSemisimpleLieAlgebra<coefficient>& e);
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

template <class coefficient>
class Weight {
public:
  SemisimpleLieAlgebra* owner;
  Vector<coefficient> weightFundamentalCoordS;
  static bool IsMonEqualToZero() {
    return false;
  }
  friend std::ostream& operator << (std::ostream& output, const Weight<coefficient>& input) {
    output << input.ToString();
    return output;
  }
  Weight(): owner(nullptr) {
  }
  void CheckNonZeroOwner() const {
    if (this->owner != nullptr) {
      return;
    }
    crash << "This is a programming error: Monomial char has zero owner, "
    << "which is not allowed by the current function call. " << crash;
  }
  void AccountSingleWeight(
    const Vector<Rational>& currentWeightSimpleCoords,
    const Vector<Rational>& otherHighestWeightSimpleCoords,
    Rational& theMult,
    charSSAlgMod<coefficient>& outputAccum
  ) const;
  std::string TensorAndDecompose(const Weight<coefficient>& other, charSSAlgMod<coefficient>& output) const;
  std::string ToString(FormatExpressions* theFormat = nullptr) const;
  inline unsigned int HashFunction() const {
    return weightFundamentalCoordS.HashFunction();
  }
  static inline unsigned int HashFunction(const Weight<coefficient>& input) {
    return input.HashFunction();
  }
  void operator+=(const Weight<coefficient>& other) {
    if (this->owner != other.owner) {
      crash << "Attempting to add weights with different owner algebras. " << crash;
    }
    this->weightFundamentalCoordS += other.weightFundamentalCoordS;
  }
  void operator*=(const coefficient& other) {
    this->weightFundamentalCoordS *= other;
  }
  inline bool operator==(const Weight<coefficient>& other) const {
    return this->weightFundamentalCoordS == other.weightFundamentalCoordS && this->owner == other.owner;
  }
  inline bool operator>(const Weight<coefficient>& other) const {
    return this->weightFundamentalCoordS > other.weightFundamentalCoordS;
  }
};

template <class coefficient>
class charSSAlgMod : public MonomialCollection<Weight<coefficient>, coefficient> {
  public:
  void CheckConsistency() const {
    if (this->size() == 0) {
      return;
    }
    const SemisimpleLieAlgebra* owner = (*this)[0].owner;
    for (int i = 1; i < this->size(); i ++) {
      if ((*this)[i].owner != owner) {
        crash << "This is a programming error: charSSAlgMod contains "
        << "elements belonging to different semisimple Lie algebras. " << crash;
      }
    }
  }
  void CheckNonZeroOwner() const {
    this->CheckConsistency();
    if (this->GetOwner() == 0) {
      crash << "This is a programming error: charSSAlgMod has no owner semisimple Lie algebra, "
      << "which is not allowed at by the calling function. " << crash;
    }
  }
  bool IsEqualToZero() {
    return this->size() == 0;
  }
  unsigned int HashFunction() const {
    return this->HashFunction(*this);
  }
  static unsigned int HashFunction(const charSSAlgMod<coefficient>& input) {
    return input.::MonomialCollection<Weight<coefficient>, coefficient>::HashFunction(input);
  }
  void GetDual(charSSAlgMod<coefficient>& output) const;
  void MakeFromWeight(const Vector<coefficient>& inputWeightSimpleCoords, SemisimpleLieAlgebra* inputOwner);
  bool SplitCharOverRedSubalg(std::string* Report, charSSAlgMod& output, branchingData& inputData);
  bool GetDominantCharacterWRTsubalgebra(
    charSSAlgMod& outputCharOwnerSetToZero, std::string& outputDetails, int upperBoundNumDominantWeights
  );
  bool FreudenthalEvalMeDominantWeightsOnly(
    charSSAlgMod<coefficient>& outputCharOwnerSetToZero, int upperBoundNumDominantWeights, std::string* outputDetails
  );
  bool FreudenthalEvalMeFullCharacter(
    charSSAlgMod<coefficient>& outputCharOwnerSetToZero, int upperBoundNumDominantWeights, std::string* outputDetails
  );
  std::string ToStringFullCharacterWeightsTable();
  bool DrawMeNoMults(std::string& outputDetails, DrawingVariables& theDrawingVars, int upperBoundWeights) {
    return this->DrawMe(outputDetails, theDrawingVars, upperBoundWeights, false);
  }
  int GetPosNstringSuchThatWeightMinusNalphaIsWeight(
    const Weight<coefficient>& theWeightInFundCoords, const Vector<coefficient>& theAlphaInFundCoords
  );
  bool DrawMeWithMults(std::string& outputDetails, DrawingVariables& theDrawingVars, int upperBoundWeights) {
    return this->DrawMe(outputDetails, theDrawingVars, upperBoundWeights, true);
  }
  void DrawMeAssumeCharIsOverCartan(WeylGroupData& actualAmbientWeyl, DrawingVariables& theDrawingVars) const;
  SemisimpleLieAlgebra* GetOwner() const {
    if (this->size() == 0) {
      crash << "This is a programming error: requesting owner semisimple Lie algebra of zero character. " << crash;
    }
    return (*this)[0].owner;
  }
  bool DrawMe(std::string& outputDetails, DrawingVariables& theDrawingVars, int upperBoundWeights, bool useMults);
  bool SplitOverLeviMonsEncodeHIGHESTWeight(
    std::string* Report,
    charSSAlgMod& output,
    const Selection& splittingParSel,
    const Selection& ParSelFDInducingPart,
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& outputWeylSub
  );
  int GetIndexExtremeWeightRelativeToWeyl(WeylGroupData& theWeyl) const;
  void MakeTrivial(SemisimpleLieAlgebra& inputOwner);
  std::string MultiplyBy(const charSSAlgMod& other);
  std::string operator*=(const charSSAlgMod& other);
  std::string operator*=(const Weight<Rational>& other);
  charSSAlgMod<coefficient> operator-(const charSSAlgMod<coefficient>& right) const {
    charSSAlgMod<coefficient> output = *this;
    output -= right;
    return output;
  }
  void operator*=(const coefficient& other) {
    this->::MonomialCollection<Weight<coefficient>, coefficient>::operator*=(other);
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
      crash << "This is a programming error: non-initialized HomomorphismSemisimpleLieAlgebra. " << crash;
    }
    return *this->domainAlg;
  }
  SemisimpleLieAlgebra& theRange() {
    if (this->rangeAlg == nullptr) {
      crash << "This is a programming error: non-initialized HomomorphismSemisimpleLieAlgebra. " << crash;
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
  void ToString(std::string& output) {
    this->ToString(output, false);
  }
  void ToString(std::string& output, bool useHtml);
  void MakeGinGWithId(
    char theWeylLetter, int theWeylDim, MapReferenceS<DynkinType, SemisimpleLieAlgebra>& ownerOfAlgebras
  );
  void ProjectOntoSmallCartan(Vector<Rational>& input, Vector<Rational> & output);
  void ProjectOntoSmallCartan(Vectors<Rational>& input, Vectors<Rational>& output);
  void GetMapSmallCartanDualToLargeCartanDual(Matrix<Rational> & output);
  void ComputeDebugString() {
    this->ToString(this->DebugString);
  }
  void ComputeDebugString(bool useHtml) {
    this->ToString(this->DebugString, useHtml);
  }
  std::string ToString() {
    std::string tempS;
    this->ToString(tempS);
    return tempS;
  }
  void GetRestrictionAmbientRootSystemToTheSmallerCartanSA(Vectors<Rational>& output);
  bool ComputeHomomorphismFromImagesSimpleChevalleyGenerators();
  bool CheckClosednessLieBracket();
  void ApplyHomomorphism(const ElementSemisimpleLieAlgebra<Rational>& input, ElementSemisimpleLieAlgebra<Rational>& output);
  bool ApplyHomomorphism(
    const ElementUniversalEnveloping<RationalFunctionOld>& input, ElementUniversalEnveloping<RationalFunctionOld>& output
  );
  bool ApplyHomomorphism(
    const MonomialUniversalEnveloping<RationalFunctionOld>& input,
    const RationalFunctionOld& theCoeff,
    ElementUniversalEnveloping<RationalFunctionOld>& output
  );
};

#endif
