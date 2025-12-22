#ifndef header_math_lie_theory_ALREADY_INCLUDED
#define header_math_lie_theory_ALREADY_INCLUDED

#include "general_strings.h"
#include "math_general.h"
#include "math_large_integers.h"
#include "math_linear_combination.h"
#include "math_subsets_selections.h"
#include "math_vectors.h"

// An externally defined class used to plot Dynkin diagrams.
class Plot;
class WeylGroupData;
class AlgebraicClosureRationals;
template <class Coefficient>
class MatrixTensor;

class ChevalleyGenerator {
public:
  SemisimpleLieAlgebra* owner;
  // generatorIndex = 0 stands for g_{- N} and all larger indices stand for the
  // remaining generators in the Lie algebra basis:
  // g_{- N}, ... g_{- 1}, h_1, ..., h_k, g_1, ..., g_N.
  // Here h_1, ..., h_k are elements of the Cartan subalgebra, the elements
  // g_{- N}, ..., g_{- 1} are the negative Chevalley generators,
  // and g_1, ..., g_N are the positive.
  // The generators are ordered in the standard order assumed in our system,
  // i.e., in the graded lexicographic order with
  // respect to the simple coordinates of the weights of the generators.
  int generatorIndex;
  ChevalleyGenerator(): owner(nullptr), generatorIndex(- 1) {}
  friend std::ostream& operator<<(
    std::ostream& output, const ChevalleyGenerator& generator
  ) {
    output << generator.toString();
    return output;
  }
  bool checkInitialization() const;
  static unsigned int hashFunction(const ChevalleyGenerator& input) {
    return static_cast<unsigned>(input.generatorIndex);
  }
  unsigned int hashFunction() const {
    return this->hashFunction(*this);
  }
  void makeGenerator(
    SemisimpleLieAlgebra& inputOwner, int inputGeneratorIndex
  );
  void makeGeneratorRootSpace(
    SemisimpleLieAlgebra& inputOwner, const Vector<Rational>& root
  );
  void operator=(const ChevalleyGenerator& other) {
    this->owner = other.owner;
    this->generatorIndex = other.generatorIndex;
  }
  bool operator>(const ChevalleyGenerator& other) const;
  std::string toString(const FormatExpressions* inputFormat = nullptr) const;
  std::string toMathML(
    const FormatExpressions* inputFormat = nullptr,
    MathExpressionFormattingProperties* outputProperties = nullptr
  ) const;
  void checkConsistencyWithOther(const ChevalleyGenerator& other) const;
  bool operator==(const ChevalleyGenerator& other) const {
    this->checkConsistencyWithOther(other);
    return this->generatorIndex == other.generatorIndex;
  }
  bool isInCartan(Vector<Rational>* whichElement) const;
  // Returns the root of the generator if the element
  // is not in the Cartan subalgebra.
  // If the element is in the Cartan subalgebra, returns the zero vector.
  bool isInRootSpace(Vector<Rational>* whichRoot) const;
  class Test {
  public:
    static bool all();
    static bool basic();
  };
};

class DynkinSimpleType {
  friend std::ostream& operator<<(
    std::ostream& output, const DynkinSimpleType& monomial
  ) {
    output << monomial.toString();
    return output;
  }
public:
  char letter;
  int rank;
  Rational cartanSymmetricInverseScale;
  // Constants used to plot Dynkin diagrams.
  static const int radiusOfRootCircle = 1;
  static const int distanceBetweenRootCenters = 20;
  static const int labelDistance = 5;
  DynkinSimpleType(): letter('X'), rank(- 1), cartanSymmetricInverseScale(0) {}
  DynkinSimpleType(const DynkinSimpleType& other) {
    *this = other;
  }
  DynkinSimpleType(
    char inputLetter, int inputRank, const Rational& inputScale = 1
  ):
  letter(inputLetter),
  rank(inputRank),
  cartanSymmetricInverseScale(inputScale) {}
  int getRootSystemSize() const;
  int getLieAlgebraDimension() const {
    return this->getRootSystemSize() + this->rank;
  }
  void makeArbitrary(
    char inputLetter,
    int inputRank,
    const Rational& inputLengthFirstCoRootSquared
  );
  Rational getPrincipalSlTwoCartanSymmetricInverseScale() const;
  void getCoCartanSymmetric(Matrix<Rational>& output) const;
  void getCartanSymmetric(Matrix<Rational>& output) const;
  void getAn(int n, Matrix<Rational>& output) const;
  void getBn(int n, Matrix<Rational>& output) const;
  void getCn(int n, Matrix<Rational>& output) const;
  void getDn(int n, Matrix<Rational>& output) const;
  void getEn(int n, Matrix<Rational>& output) const;
  void getF4(Matrix<Rational>& output) const;
  void getG2(Matrix<Rational>& output) const;
  void grow(
    List<DynkinSimpleType>& output, List<List<int> >* outputPermutationRoots
  ) const;
  void operator=(const DynkinSimpleType& other) {
    this->letter = other.letter;
    this->rank = other.rank;
    this->cartanSymmetricInverseScale = other.cartanSymmetricInverseScale;
  }
  bool operator==(const DynkinSimpleType& other) const {
    return
    this->letter == other.letter &&
    this->rank == other.rank &&
    this->cartanSymmetricInverseScale == other.cartanSymmetricInverseScale;
  }
  bool operator!=(const DynkinSimpleType& other) const {
    return !((*this) == other);
  }
  static unsigned int hashFunction(const DynkinSimpleType& input) {
    return
    static_cast<unsigned int>(input.letter) * 2 + static_cast<unsigned int>(
      input.rank
    ) +
    HashConstants::constant0 *
    input.cartanSymmetricInverseScale.hashFunction();
  }
  unsigned int hashFunction() const {
    return this->hashFunction(*this);
  }
  void getAutomorphismActingOnVectorColumn(
    MatrixTensor<Rational>& output, int autoIndex
  ) const;
  Rational getDefaultCoRootLengthSquared(int rootIndex) const;
  Rational getDefaultRootLengthSquared(int rootIndex) const;
  Rational getDefaultLongRootLengthSquared() const;
  Rational getEpsilonRealizationLongRootLengthSquared() const;
  static Rational getDefaultLongRootLengthSquared(char inputType) {
    DynkinSimpleType dynkinType(inputType, 2);
    return dynkinType.getDefaultLongRootLengthSquared();
  }
  Rational getLongRootLengthSquared() const;
  Rational getRatioRootSquaredToFirstSquared(int rootIndex) const;
  static Rational getRatioLongRootToFirst(char inputWeylLetter, int inputRank);
  static Rational getDynkinIndexParabolicallyInducingSubalgebra(
    char inputType
  );
  bool canBeExtendedParabolicallyTo(const DynkinSimpleType& otherType) const;
  bool canBeExtendedParabolicallyOrIsEqualTo(const DynkinSimpleType& otherType)
  const {
    if (*this == otherType) {
      return true;
    }
    return this->canBeExtendedParabolicallyTo(otherType);
  }
  bool hasPrecomputedSubalgebras() const;
  Rational getRatioLongRootToFirst() const {
    return this->getRatioLongRootToFirst(this->letter, this->rank);
  }
  std::string toString(const FormatExpressions* format = nullptr) const;
  std::string toStringNonTechnicalName(
    const FormatExpressions* format = nullptr
  ) const;
  std::string toMathMLFinal(const FormatExpressions* format) const;
  std::string toMathML(
    const FormatExpressions* format,
    MathExpressionFormattingProperties* outputProperties = nullptr
  ) const;
  void operator++(int);
  bool operator>(const DynkinSimpleType& other) const;
  static void getEpsilonMatrix(
    char weylLetter, int weylRank, Matrix<Rational>& output
  );
  bool operator<(const DynkinSimpleType& other) const {
    return other > *this;
  }
  bool operator<(int otherRank) const;
  void plot(Plot& output, int verticalOffset) const;
  static void plotOneRoot(
    Plot& output, int index, bool filled, int verticalOffset
  );
  static void plotHorizontalChainOfRoots(
    Plot& output, int count, Selection* filledNodes
  );
  static void plotHorizontalChainOfRoots(
    Plot& output,
    int verticalOffset,
    int count,
    Selection* filledNodes,
    List<std::string>* labels
  );
  static void plotE6(Plot& output, int verticalOffset);
  static void plotE7(Plot& output, int verticalOffset);
  static void plotE8(Plot& output, int verticalOffset);
  static void plotF4(Plot& output, int verticalOffset);
  static void plotG2(Plot& output, int verticalOffset);
  static void appendOneSingleConnectedRootToTheRight(
    Plot& output,
    int segmentsSoFar,
    int verticalOffset,
    const std::string& label,
    bool filled
  );
  static void plotAn(
    Plot& output, int rank, Selection* filledRoots, int verticalOffset
  );
  static void plotBn(
    Plot& output, int rank, Selection* filledRoots, int verticalOffset
  );
  static void plotBC(
    Plot& output, int rank, Selection* filledRoots, int verticalOffset
  );
  static void plotCn(
    Plot& output, int rank, Selection* filledRoots, int verticalOffset
  );
  static void plotDn(
    Plot& output, int rank, Selection* filledRoots, int verticalOffset
  );
};

// This class may need a modification: perhaps it should not inherit monomial
// collection,
// but rather have a monomial collection as a private member.
// Many important operations with Dynkin types require
// fixed order of monomials, which may impose this reorganization.
class DynkinType: public LinearCombination<DynkinSimpleType, Rational> {
private:
public:
  void getLettersTypesMultiplicities(
    List<char>* outputLetters = nullptr,
    List<int>* outputRanks = nullptr,
    List<int>* outputMults = nullptr,
    List<Rational>* outputFirstCoRootLengthsSquared = nullptr
  ) const;
  void getTypesWithMults(List<DynkinSimpleType>& output) const;
  static void getOuterAutosGeneratorsOneTypeActOnVectorColumn(
    List<MatrixTensor<Rational> >& output,
    const DynkinSimpleType& dynkinType,
    int multiplicity
  );
  unsigned int hashFunction() const {
    return this->LinearCombination<DynkinSimpleType, Rational>::hashFunction();
  }
  static unsigned int hashFunction(const DynkinType& input) {
    return LinearCombination<DynkinSimpleType, Rational>::hashFunction(input);
  }
  void getOuterAutosGeneratorsActOnVectorColumn(
    List<MatrixTensor<Rational> >& output
  );
  bool isSimpleOfType(char desiredType, int desiredRank) const;
  bool isSimpleOfType(
    char desiredType,
    int* outputRank = nullptr,
    Rational* outputLength = nullptr
  ) const;
  bool isSimple(
    char* outputtype = nullptr,
    int* outputRank = nullptr,
    Rational* outputLength = nullptr
  ) const;
  void getSortedDynkinTypes(List<DynkinSimpleType>& output) const;
  Rational getPrincipalSlTwoCartanSymmetricInverseScale() const;
  void sortDynkinTypes();
  bool grow(
    const List<Rational>& allowedInverseScales,
    int ambientWeylDimension,
    List<DynkinType>& output,
    List<List<int> >* outputPermutationRoots
  ) const;
  bool hasPrecomputedSubalgebras() const;
  std::string toStringVirtualNameFolder() const;
  bool containsType(char typeLetter) const;
  void getDynkinTypeWithDefaultScales(DynkinType& output) const;
  static void getPrecomputedDynkinTypes(List<DynkinType>& output);
  DynkinSimpleType getGreatestSimpleType() const;
  DynkinSimpleType getSmallestSimpleType() const;
  LargeInteger getWeylGroupSizeByFormula() const;
  std::string toString(FormatExpressions* format = nullptr) const;
  void scaleFirstCoRootSquaredLength(
    const Rational& multiplyCoRootSquaredLengthBy
  );
  int getMultiplicity(int simpleTypeIdentifier) const;
  int getNumberOfSimpleComponentsOfGivenRank(int desiredRank) const;
  int getNumberOfSimpleComponents() const;
  Rational getRankRational() const;
  int getRank() const;
  int getRootSystemSize() const;
  int getLieAlgebraDimension() const;
  bool isTypeAOne() const;
  static int getIndexPreimageFromRootInjection(
    int inputIndex, const List<int>& inputRootInjection
  );
  bool canBeExtendedParabolicallyTo(const DynkinType& other) const;
  bool canBeExtendedParabolicallyOrIsEqualTo(const DynkinType& other) const;
  void makeSimpleType(
    char type, int rank, const Rational* inputFirstCoRootSquareLength = nullptr
  );
  void getEpsilonMatrix(Matrix<Rational>& output) const;
  void getCoCartanSymmetric(Matrix<Rational>& output) const;
  void getCartanSymmetric(Matrix<Rational>& output) const;
  void getCartanSymmetricDefaultLengthKeepComponentOrder(
    Matrix<Rational>& output
  ) const;
  int getCoxeterEdgeWeight(int v, int w);
  std::string getWeylGroupName(FormatExpressions* format = nullptr) const;
  static void getDynkinIndicesSl2SubalgebrasSimpleType(
    const DynkinSimpleType& desiredType,
    List<List<Rational> >& precomputedDynkinIndicesSl2SubalgebrasSimpleTypes,
    HashedList<DynkinSimpleType>& dynkinSimpleTypesWithComputedSl2Subalgebras,
    HashedList<Rational>& outputDynkinIndices,
    MapReferences<std::string, AlgebraicClosureRationals>& algebraicClosures
  );
  void getDynkinIndicesSl2Subalgebras(
    List<List<Rational> >& precomputedDynkinIndicesSl2SubalgebrasSimpleTypes,
    HashedList<DynkinSimpleType>& dynkinSimpleTypesWithComputedSl2Subalgebras,
    HashedList<Rational>& outputDynkinIndices,
    MapReferences<std::string, AlgebraicClosureRationals>& algebraicClosures
  );
  bool hasExceptionalComponent() const;
  bool operator>(const DynkinType& other) const;
  void operator=(const LinearCombination<DynkinSimpleType, Rational>& other);
  DynkinType operator-(
    const LinearCombination<DynkinSimpleType, Rational>& other
  );
  // These functions are used to plot dynkin diagrams and Vogan diagrams.
  void plot(Plot& output);
  static void plotInitialize(Plot& output);
  // Makes a dynkin type from stirng
  void fromString(const std::string& input);
};

class DynkinDiagramRootSubalgebra {
public:
  Vectors<Rational> ambientRootSystem;
  Matrix<Rational> ambientBilinearForm;
  List<Vectors<Rational> > simpleBasesConnectedComponents;
  // to each connected component of the simple bases corresponds
  // its dynkin string with the same index
  List<DynkinSimpleType> simpleComponentTypes;
  List<int> indicesThreeNodes;
  List<List<int> > indicesEnds;
  List<List<int> > sameTypeComponents;
  List<int> indexUniComponent;
  List<int> indexInUniComponent;
  bool checkInitialization() const;
  std::string toMathMLFinal(FormatExpressions* format = nullptr) const;
  std::string toMathML(FormatExpressions* format = nullptr) const;
  std::string toString(FormatExpressions* format = nullptr) const;
  int rankTotal();
  int numberRootsGeneratedByDiagram();
  void sort();
  void getDynkinType(DynkinType& output) const;
  void swapDynkinStrings(int i, int j);
  // Gets the index of the first simple component in the simpleComponentTypes
  // that has a simple root whose scalar product with the given root is
  // non-zero.
  int indexFirstComponentLinkedTo(const Vector<Rational>& inputVector) const;
  // Gets the first Dynkin simple type that has a root with
  // that is not perpendicular to the given vector.
  DynkinSimpleType typeFirstComponentLinkedTo(
    const Vector<Rational>& inputVector
  ) const;
  Rational getSquareLengthLongestRootLinkedTo(
    const Vector<Rational>& inputVector
  );
  Rational getSquareLengthShortestRootLinkedTo(
    const Vector<Rational>& inputVector
  );
  bool letterIsDynkinGreaterThanLetter(char letter1, char letter2);
  // The function below takes as an input a set of roots and
  // computes the corredponding Dynkin diagram of the
  // root subsystem. Note: the simleBasisInput
  // is required to be a set of simple basis roots,
  // not to be confused with a basis of a simple Dynkin diagram.
  // The procedure calls a
  // transformation to simple basis on the simpleBasisInput,
  // so your input will get changed if it wasn't
  // simple as required.
  void computeDiagramTypeModifyInput(
    Vectors<Rational>& inputRoots, WeylGroupData& weylGroup
  );
  // The following function is just as the above but doesn't modify
  // simpleBasisInput
  void computeDiagramInputIsSimpleBasis(
    const Vectors<Rational>& simpleBasisInput
  );
  void computeDiagramTypeModifyInputRelative(
    Vectors<Rational>& inputOutputSimpleWeightSystem,
    const HashedList<Vector<Rational> >& weightSystem,
    const Matrix<Rational>& bilinearForm
  );
  void computeDynkinStrings();
  void computeDynkinString(int indexComponent);
  int numberOfThreeValencyNodes(int indexComponent);
  bool operator==(const DynkinDiagramRootSubalgebra& right) const;
  bool isGreaterThan(DynkinDiagramRootSubalgebra& right);
  Rational getSizeCorrespondingWeylGroupByFormula();
  void getAutomorphism(List<List<int> >& output, int index);
  void getAutomorphisms(List<List<List<int> > >& output);
  void getMapFromPermutation(
    Vectors<Rational>& domain,
    Vectors<Rational>& range,
    List<int>& permutation,
    List<List<List<int> > >& automorphisms,
    SelectionWithDifferentMaxMultiplicities& autosPermutations,
    DynkinDiagramRootSubalgebra& right
  );
};

class LittelmannPath {
public:
  friend std::ostream& operator<<(
    std::ostream& output, const LittelmannPath& unused
  ) {
    (void) unused;
    output << "A Littelmann Path.";
    return output;
  }
  WeylGroupData* owner;
  Vectors<Rational> waypoints;
  void makeFromweightInSimpleCoordinates(
    const Vector<Rational>& weightInSimpleCoordinates,
    WeylGroupData& inputOwner
  );
  void makeFromWaypoints(
    Vectors<Rational>& weightsinSimpleCoordinates, WeylGroupData& inputOwner
  ) {
    this->owner = &inputOwner;
    this->waypoints = weightsinSimpleCoordinates;
    this->simplify();
  }
  void actByFAlpha(int indexAlpha);
  void actByEAlpha(int indexAlpha);
  void actByEFDisplayIndex(int displayIndex);
  void operator+=(const LittelmannPath& other) {
    this->waypoints.reserve(this->waypoints.size + other.waypoints.size);
    Vector<Rational> endPoint = *this->waypoints.lastObject();
    for (int i = 0; i < other.waypoints.size; i ++) {
      this->waypoints.addOnTop(other.waypoints[i] + endPoint);
    }
  }
  bool isAdaptedString(
    MonomialTensor<int, HashFunctions::hashFunction>& inputString
  );
  std::string toStringIndicesToCalculatorOutput(
    LittelmannPath& inputStartingPath, List<int>& input
  );
  std::string toStringOperatorSequenceStartingOnMe(List<int>& input);
  bool generateOrbit(
    List<LittelmannPath>& output,
    List<List<int> >& outputOperators,
    int upperBoundNumberOfElements,
    Selection* parabolicNonSelectedAreInLeviPart = nullptr
  );
  std::string generateOrbitAndAnimate();
  bool minimaAreIntegral();
  std::string toString(
    bool useSimpleCoordinates = true,
    bool useArrows = true,
    bool includeDominance = false
  ) const;
  std::string toMathML(
    bool useSimpleCoordinates = true,
    bool useArrows = true,
    bool includeDominance = false
  ) const;
  void simplify();
  unsigned int hashFunction() const {
    return this->waypoints.hashFunction();
  }
  static unsigned int hashFunction(const LittelmannPath& input) {
    return input.hashFunction();
  }
  bool isEqualToZero() const {
    return this->waypoints.size == 0;
  }
  LittelmannPath();
  LittelmannPath(const LittelmannPath& other);
  LittelmannPath& operator=(const LittelmannPath& other) {
    this->waypoints = other.waypoints;
    this->owner = other.owner;
    return *this;
  }
  bool operator==(const LittelmannPath& other) const {
    return this->waypoints == other.waypoints;
  }
};

class PolynomialOverModule;

class SlTwoInSlN {
  int getModuleIndexFromHighestWeightVector(const Matrix<Rational>& input) {
    Rational scale;
    for (int i = 0; i < this->highestWeightVectors.size; i ++) {
      if (
        this->highestWeightVectors.objects[i].isProportionalTo(input, scale)
      ) {
        return i;
      }
    }
    return - 1;
  }
public:
  int dimension;
  Matrix<Rational> hElement;
  Matrix<Rational> eElement;
  Matrix<Rational> fElement;
  List<int> partition;
  List<Matrix<Rational> > projectors;
  List<Matrix<Rational> > highestWeightVectors;
  List<List<Matrix<Rational> > > gModKModules;
  List<List<List<int> > > pairingTable;
  void getIsPlusKIndexingFrom(int input, int& s, int& k);
  std::string elementMatrixToTensorString(
    const Matrix<Rational>& input, bool useHtml
  );
  std::string initFromModuleDecomposition(
    List<int>& decompositionDimensions, bool useHtml, bool computePairingTable
  );
  std::string initPairingTable(bool useHtml);
  std::string elementModuleIndexToString(int input, bool useHtml);
  std::string getNotationString(bool useHtml);
  bool computeInvariantsOfDegree(
    List<int>& decompositionDimensions,
    int degree,
    List<Polynomial<Rational> >& output,
    std::string& outputError
  );
  std::string pairTwoIndices(
    List<int>& output, int leftIndex, int rightIndex, bool useHtml
  );
  void extractHighestWeightVectorsFromVector(
    Matrix<Rational>& input,
    List<Matrix<Rational> >& outputDecompositionOfInput,
    List<Matrix<Rational> >& outputTheHWVectors
  );
  void climbDownFromHighestWeightAlongSl2String(
    Matrix<Rational>& input,
    Matrix<Rational>& output,
    Rational& outputCoefficients,
    int generatorPower
  );
  void climbUpFromVector(
    Matrix<Rational>& input,
    Matrix<Rational>& outputLastNonZero,
    int& largestPowerNotKillingInput
  );
};

class KazhdanLusztigPolynomials: public HashedList<Vector<Rational> > {
public:
  WeylGroupData* weylGroup;
  List<int> multiplicities;
  List<bool> explored;
  int nextToExplore;
  int lowestNonExplored;
  List<List<int> > bruhatOrder;
  List<List<int> > simpleReflectionsActionList;
  List<List<int> > inverseBruhatOrder;
  List<List<Polynomial<Rational> > > kazhdanLuzstigPolynomials;
  List<List<Polynomial<Rational> > > rPolynomials;
  List<List<Rational> > kazhdanLuzstigCoefficients;
  void kazhdanLuzstigCoefficientsToString(
    List<int>& kazhdanLuzstigCoefficients, std::string& output
  );
  void findNextToExplore();
  int findMinimalBruhatNonExplored(List<bool>& explored);
  int findMaximalBruhatNonExplored(List<bool>& explored);
  void initTheMults();
  void compute(int x);
  void check();
  // returns true if computation succeeded, false otherwise
  bool computeRxy(int x, int y, int simpleReflectionIndex);
  void computeKLxy(int x, int y);
  bool isMaximalNonExplored(int index);
  bool indexGEQIndex(int a, int b);
  bool indexGreaterThanIndex(int a, int b);
  std::string toString(FormatExpressions* format = nullptr);
  void mergeBruhatLists(int fromList, int toList);
  std::string KLPolysToString(FormatExpressions* format = nullptr);
  void computeKLCoefficients();
  int chamberIndicatorToIndex(Vector<Rational>& chamberIndicator);
  std::string rPolysToString(FormatExpressions* format = nullptr);
  bool computeKLPolys(WeylGroupData* weylGroup);
  void computeRPolys();
  int computeProductfromSimpleReflectionsActionList(int x, int y);
  void writeKLCoeffsToFile(
    std::fstream& output, List<int>& KLCoefficient, int topIndex
  );
  // returns the topIndex of the KL coefficients
  int readKLCoeffsFromFile(std::fstream& input, List<int>& output);
  KazhdanLusztigPolynomials() {
    this->weylGroup = nullptr;
  }
  void generatePartialBruhatOrder();
  void initFromWeyl(WeylGroupData* weylGroup);
};

#endif // header_math_lie_theory_ALREADY_INCLUDED
