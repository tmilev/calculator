
#ifndef header_math_vector_partition_functions_ALREADY_INCLUDED
#define header_math_vector_partition_functions_ALREADY_INCLUDED

#include "math_general.h"

class PartialFractions;

// This class holds a partial fraction denominator
// of the form (1-x^{e_1 v})^m_1 ... (1-x^{e_k v})^m_k.
// Here, v is a vector whose coefficients have no
// common divisor, e_k ("elongation") is a small integer
// and m_k ("multiplicity") is another small integer.
// The normalizedVectorIndex is the index (position)
// of the vector v in the normalized list of starting vectors.
class OnePartialFractionDenominatorComponent {
  friend std::ostream& operator<<(
    std::ostream& output,
    const OnePartialFractionDenominatorComponent& input
  ) {
    output << input.toString(nullptr);
    return output;
  }
public:
  List<int> multiplicities;
  List<int> elongations;
  int normalizedVectorIndex;
  PartialFractions* owner;
  void addMultiplicity(int multiplicityIncrement, int elongation);
  int indexLargestElongation() const;
  int getLargestElongation() const;
  void getElongatedVector(int index, Vector<Rational>& output) const;
  void getLongestVector(Vector<Rational>& output) const;
  const Vector<Rational>& getNormalizedVector() const;
  void getMonomial(int elongationIndex, MonomialPolynomial& output);
  void getPolynomialDenominator(
    Polynomial<LargeInteger>& output,
    int multiplicityIndex,
    Vector<Rational>& exponent
  );
  int getMultiplicityLargestElongation();
  int getLeastCommonMultipleElongations() const;
  int getTotalMultiplicity() const;
  void getDenominatorExponents(List<Vector<Rational> >& output) const;
  void getDenominatorExponentsWithoutMultiplicities(
    List<Vector<Rational> >& output
  ) const;
  void invert();
  void initialize(
    PartialFractions& inputOwner, int inputNormalizedVectorIndex
  );
  static Vector<Rational> getCheckSumRoot(int numberOfVariables);
  unsigned int hashFunction() const;
  static unsigned int hashFunction(
    const OnePartialFractionDenominatorComponent& input
  ) {
    return input.hashFunction();
  }
  OnePartialFractionDenominatorComponent();
  void computeOneCheckSum(
    Rational& output, const Vector<Rational>& variableValues
  );
  void operator=(const OnePartialFractionDenominatorComponent& right);
  bool operator==(const OnePartialFractionDenominatorComponent& right) const;
  bool operator>(const OnePartialFractionDenominatorComponent& other) const;
  std::string toStringInternal() const;
  std::string toString(FormatExpressions* format) const;
  std::string toStringOneDenominator(
    int elongation, int multiplicity, FormatExpressions* format
  ) const;
  bool checkInitialization() const;
};

// Suppose the initial vectors are v_1, ..., v_n.
// For every vector v=(a_1, ..., a_k), define
// x^{v} as the monomial x_1^{a_1} ... x_n^{a_k}.
//
// Then the denominator recorded here has the form:
// (1-x^{v_1})^m_1 ... (1-x^{v_n})^m_n,
// where we call
// e_1, ..., e_n
// the vector "elongations" and we call
// m_1, ..., m_n the vector "multiplicities".
class OnePartialFractionDenominator {
private:
  void findPivot();
  void findInitialPivot();
  bool rootIsInFractionCone(Vector<Rational>* root) const;
  friend class PartialFractions;
public:
  // The keys are the exponents vectors, rescaled so that the
  // coefficients have no greatest common divisor.
  // Given a key v, the values are all
  // denominators of the form (1-x^{a_1 v})^{m_1} ... (1-x^{a_k v})^{m_k}.
  MapList<Vector<Rational>, OnePartialFractionDenominatorComponent>
  denominatorsNoScale;
  PartialFractions* owner;
  friend std::ostream& operator<<(
    std::ostream& output, const OnePartialFractionDenominator& input
  ) {
    output << input.toString();
    return output;
  }
  void computePolynomialCorrespondingToOneMonomial(
    QuasiPolynomial& outputQuasipolynomial,
    const MonomialPolynomial& monomial,
    Vectors<Rational>& coneGenerators,
    Vectors<Rational>& normals,
    Lattice& lattice
  ) const;
  static void evaluateIntegerPolynomial(
    const Polynomial<LargeInteger>& input,
    const Vector<Rational>& values,
    Rational& output
  );
  static void makePolynomialFromOneNormal(
    Vector<Rational>& normal,
    const MonomialPolynomial& shiftRational,
    int multiplicities,
    Polynomial<Rational>& output
  );
  void computeNormals(
    PartialFractions& owner,
    Vectors<Rational>& output,
    int dimension,
    Matrix<Rational>& buffer
  );
  int computeGainingMultiplicityIndexInLinearRelation(
    Vector<Rational>& linearRelation
  );
  void getNormalizedSortedDenominatorExponents(Vectors<Rational>& output)
  const;
  void getDenominatorsSorted(
    MapList<Vector<Rational>, OnePartialFractionDenominatorComponent>& output
  ) const;
  // Returns the denominator exponents in the order implied by
  // the order of the normalized vectors.
  void getDenominatorExponents(Vectors<Rational>& output) const;
  void getDenominatorExponentsWithoutMultiplicities(
    Vectors<Rational>& output
  ) const;
  void getVectorPartitionFunction(
    PartialFractions& owner,
    Polynomial<LargeInteger>& coefficient,
    QuasiPolynomial& output
  ) const;
  void decomposeAMinusNB(
    int indexA,
    int indexB,
    int n,
    int indexAMinusNB,
    LinearCombination<
      OnePartialFractionDenominator, Polynomial<LargeInteger>
    >& output,
    PartialFractions& owner
  );
  bool decomposeFromNormalizedLinearRelation(
    Vector<Rational>& linearRelationBetweenNormalizedVectors,
    Vectors<Rational>& normalizedVectors,
    LinearCombination<
      OnePartialFractionDenominator, Polynomial<LargeInteger>
    >& output
  );
  void getLinearRelationFromNormalized(
    int gainingMultiplicityIndex,
    List<Vector<Rational> >& normalizedVectors,
    Vector<Rational>& linearRelationBetweenNormalizedVectors,
    Vector<Rational>& output
  );
  void computeOneCheckSum(
    Vector<Rational>& variableValues, Rational& output
  ) const;
  void applyGeneralizedSzenesVergneFormula(
    MapList<Vector<Rational>, OnePartialFractionDenominatorComponent>&
    toBeReduced,
    const Vector<Rational>& linearRelation,
    const Vector<Rational>& gainingNormalizedExponent,
    int elongationGainingMultiplicityIndex,
    LinearCombination<
      OnePartialFractionDenominator, Polynomial<LargeInteger>
    >& output
  ) const;
  bool reduceOnceTotalOrderMethod(
    LinearCombination<
      OnePartialFractionDenominator, Polynomial<LargeInteger>
    >& output
  );
  bool reduceOnce(
    LinearCombination<
      OnePartialFractionDenominator, Polynomial<LargeInteger>
    >& output
  );
  unsigned int hashFunction() const;
  static unsigned int hashFunction(const OnePartialFractionDenominator& input)
  {
    return input.hashFunction();
  }
  bool decreasePowerOneFraction(int index, int increment);
  void prepareFraction(
    int indexA,
    int indexB,
    int aMinusNBIndex,
    bool indexAIsNullified,
    OnePartialFractionDenominator& output,
    Polynomial<LargeInteger>& aMinusNBetaPolynomial,
    Polynomial<LargeInteger>& outputCommonCoefficient
  );
  int controlLineSizeFracs(
    std::string& output, FormatExpressions& polynomialFormatLocal
  );
  int controlLineSizeStringPolys(
    std::string& output, FormatExpressions& polynomialFormatLocal
  );
  OnePartialFractionDenominator();
  OnePartialFractionDenominator(const OnePartialFractionDenominator& other);
  ~OnePartialFractionDenominator();
  void getPolyReduceMonomialByMonomial(
    PartialFractions& owner,
    Vector<Rational>& exponent,
    int startMonomialPower,
    int denominatorPowerReduction,
    int startDenominatorPower,
    Polynomial<LargeInteger>& output
  );
  void getAlphaMinusNBetaPoly(
    PartialFractions& owner,
    int indexA,
    int indexB,
    int n,
    Polynomial<LargeInteger>& output
  );
  static void getNElongationPolynomial(
    int lengthOfGeometricSeries,
    const Vector<Rational>& exponent,
    Polynomial<LargeInteger>& output
  );
  int getNumberProportionalVectorsClassicalRootSystems(
    PartialFractions& owner
  );
  bool operator==(const OnePartialFractionDenominator& right) const;
  void operator=(const OnePartialFractionDenominator& right);
  bool initializeFromPartialFractions(PartialFractions& owner);
  void addMultiplicity(
    const Vector<Rational>& normalizedVector,
    int multiplicity,
    int elongation
  );
  std::string toString(FormatExpressions* format = nullptr) const;
  std::string toStringDenominatorOnly(FormatExpressions* format = nullptr)
  const;
  std::string toLatex(
    const Polynomial<LargeInteger>& numerator,
    FormatExpressions* format = nullptr
  ) const;
  bool checkInitialization() const;
  bool operator>(const OnePartialFractionDenominator& other) const;
};

class PartialFractions {
  bool splitPartial();
  void initializeCommon();
  void initializeInput(const List<Vector<Rational> >& input);
  void initializeDimension();
public:
  LinearCombination<
    OnePartialFractionDenominator, Polynomial<LargeInteger>
  > nonReduced;
  LinearCombination<
    OnePartialFractionDenominator, Polynomial<LargeInteger>
  > reducedWithElongationRedundancies;
  LinearCombination<
    OnePartialFractionDenominator, Polynomial<LargeInteger>
  > reduced;
  int ambientDimension;
  int indexCurrentlyProcessed;
  int highestIndex;
  int numberOfIrrelevantFractions;
  int numberOfRelevantReducedFractions;
  int numberOfMonomialsInNumerators;
  int numberOfGeneratorsInNumerators;
  int numberOfRelevantNonReducedFractions;
  int numberOfMonomialsInNumeratorsRelevantFractions;
  int totalGeneratorsRelevantFractions;
  int numberOfMonomialsInNumeratorsIrrelevantFractions;
  int numberOfGeneratorsIrrelevantFractions;
  int totalReduced;
  int totalFractionsWithAccountedVectorPartitionFunction;
  int numberOfRunsReduceMonomialByMonomial;
  int numberOfProcessedForVPFMonomialsTotal;
  Rational checkSumStart;
  Rational checkSumLast;
  bool flagDiscardingFractions;
  bool flagInitialized;
  int limitSplittingSteps;
  int splitStepsCounter;
  // The list of polyhedral cones over which our
  // vector partition function is a
  // quasipolynomial.
  ConeCollection chambers;
  // The quasipolynomials over each chamber,
  // in the same order.
  List<QuasiPolynomial> allQuasiPolynomials;
  bool flagSplitTestModeNoNumerators;
  bool flagMakingProgressReport;
  bool flagUsingCheckSum;
  int flagMaxNumStringOutputLines;
  // The original vectors whose vector partition function
  // we are computing.
  List<Vector<Rational> > originalVectors;
  // Rescaled, deduped and sorted version of the original vectors
  // so their coefficients have no greatest common
  // divisor and sorted in graded lexicographic order.
  HashedList<Vector<Rational> > normalizedVectors;
  OnePartialFractionDenominator initialPartialFraction;
  Matrix<int> tableAllowedAminusB;
  Matrix<int> tableAllowedAminus2B;
  Selection indicesRedundantShortRoots;
  List<int> indicesDoublesOfRedundantShortRoots;
  int numberOfNonRedundantShortRoots;
  Vector<Rational> weights;
  int getIndex(const Vector<Rational>& root);
  int getIndexDoubleOfARoot(const Vector<Rational>& root);
  void computeTable(int dimension);
  void prepareCheckSums();
  bool argumentsAllowed(
    Vectors<Rational>& arguments, std::string& outputWhatWentWrong
  );
  bool assureIndicatorRegularity(Vector<Rational>& indicator);
  void compareCheckSums();
  bool initializeFromVectors(
    Vectors<Rational>& input, std::stringstream* commentsOnFailure
  );
  // Returns whether the input vectors are
  // valid for vector partition function computations.
  bool inputIsValid(std::stringstream* commentsOnFailure) const;
  void initializeAndSplit(
    Vectors<Rational>& input, std::stringstream* commentsOnFailure
  );
  void run(Vectors<Rational>& input);
  void removeRedundantShortRoots(Vector<Rational>* indicator);
  // Reduces redundant short roots if present.
  // Returns true if redundant short roots were found, false otherwise.
  bool reduceOnceRedundantShortRoots(
    const OnePartialFractionDenominator& toBeReduced,
    OnePartialFractionDenominator& outputFraction,
    Polynomial<LargeInteger>& outputCoefficient,
    Vector<Rational>* indicator
  );
  bool split(Vector<Rational>* indicator);
  void computeKostantFunctionFromWeylGroup(
    char weylGroupLetter,
    int weylGroupNumber,
    QuasiPolynomial& output,
    Vector<Rational>* chamberIndicator
  );
  bool isHigherThanWithRespectToWeight(
    const Vector<Rational>& left,
    const Vector<Rational>& r,
    const Vector<Rational>& weights
  );
  void computeCheckSum(Rational& output);
  Rational computeCheckSumFromLinearCombination(
    const LinearCombination<
      OnePartialFractionDenominator, Polynomial<LargeInteger>
    >& input
  );
  void accumulateCheckSum(
    const LinearCombination<
      OnePartialFractionDenominator, Polynomial<LargeInteger>
    >& input,
    Rational& output
  );
  void evaluateVectorPartitionFunction(
    const Vector<Rational>& input,
    Rational& output,
    std::stringstream* comments
  );
  void prepareIndicatorVariables();
  bool computeOneVectorPartitionFunction(
    QuasiPolynomial& output, Vector<Rational>& newIndicator
  );
  void computeAllVectorPartitionFunctions();
  void computeQuasipolynomials();
  PartialFractions();
  bool checkForMinimalityDecompositionWithRespectToRoot(
    Vector<Rational>* root
  );
  void makeProgressReportSplittingMainPart();
  void makeProgressVPFcomputation();
  std::string toString(FormatExpressions* format = nullptr) const;
  std::string toHTML(FormatExpressions* format = nullptr) const;
  std::string toLatex(FormatExpressions* format = nullptr) const;
  class Test {
  public:
    static bool all();
    static bool splitTwoDimensional();
    class SplitTestCase {
    public:
      std::string expected;
      List<std::string> vectors;
      bool test();
    };
  };
};

class BernoulliSumComputer {
public:
  List<Rational> bernoulliPlusNumbers;
  // Reference: https://en.wikipedia.org/wiki/Bernoulli_number
  // We the m^th bernoulli plus number B^+_m, m>= 0 to be:
  // B^+_m = 1-\sum_{k=0}^{m-1} m \choose k B^+_k/(m-k+1)
  void getNthBernoulliPlusNumber(int index, Rational& output);
  void getBernoulliSum(int power, Polynomial<Rational>& output);
};

class VectorPartitionFunctionElementary {
public:
  bool flagInitialized;
  List<Vector<Rational> > originalVectors;
  ConeCollection collection;
  BernoulliSumComputer bernoulliSumComputer;
  VectorPartitionFunctionElementary();
  std::string toHTML() const;
  void compute();
  void computeQuasiPolynomials(int directionIndex);
  void computeFirstQuasiPolynomial(int directionIndex);
  bool computeOneQuasiPolynomial(Cone& cone, int directionIndex);
};

class VectorPartitionFunction {
public:
  PartialFractions fractions;
  VectorPartitionFunctionElementary elementaryMethod;
  void initializeVectors(List<Vector<Rational> >& inputVectors);
  std::string toHTML() const;
};

#endif // header_math_vector_partition_functions_ALREADY_INCLUDED
