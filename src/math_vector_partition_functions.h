
#ifndef header_math_vector_partition_functions_ALREADY_INCLUDED
#define header_math_vector_partition_functions_ALREADY_INCLUDED

#include "math_general.h"
#include "math_lattices.h"

class PartialFractions;

class Cone {
  void computeVerticesFromNormals();
  void computeVerticesOneDimension();
  bool eliminateFakeNormalsUsingVertices(int numberOfAddedFakeWalls);
  friend std::ostream& operator<<(std::ostream& output, const Cone& cone) {
    output << cone.toString();
    return output;
  }
  // Helper method for hasCommonIrregularWall.
  bool twoPlanesSeparateUs(
    Cone& neighborCandidate, const Vector<Rational>& normalOfWall
  );
  // Helper method for hasCommonIrregularWall.
  bool isSeparatingPlane(Cone& neighborCandidate, const Wall& wall);
  void computeRescaledVerticesForDrawing(Vectors<Rational>& output) const;
public:
  class Payload {
    QuasiPolynomial polynomial;
  public:
    QuasiPolynomial previousPolynomial;
    int lowestSlicingIndex;
    bool visited;
    Vector<Rational> internalPoint;
    List<unsigned char> hashOfContainingSimplices;
    Payload();
    void incrementHashOfContainingSimplices(char input);
    void setPolynomial(QuasiPolynomial& input);
    QuasiPolynomial& getPolynomial();
  };

  bool flagIsTheZeroCone;
  Vectors<Rational> vertices;
  List<Wall> walls;
  int id;
  Cone::Payload payload;
  void transformToWeylProjective(ConeCollection& owner);
  std::string drawMeToHtmlProjective(
    DrawingVariables& drawingVariables, FormatExpressions& format
  );
  std::string drawMeToHtmlLastCoordAffine(
    DrawingVariables& drawingVariables, FormatExpressions& format
  );
  void getLinesContainedInCone(Vectors<Rational>& output);
  void translateMeMyLastCoordinateAffinization(
    Vector<Rational>& translationVector
  );
  bool isHonest1DEdgeAffine(
    const Vector<Rational>& vertex1, const Vector<Rational>& vertex2
  ) const;
  bool isEntireSpace();
  bool isHonest1DEdgeAffine(int vertexIndex1, int vertexIndex2) const;
  static bool isRegularToBasis(
    const Vectors<Rational>& basis,
    Vector<Rational>& r,
    Vector<Rational>& outputFailingNormal,
    int dimension
  );
  static bool regularizeToBasis(
    const Vectors<Rational>& basis, Vector<Rational>& output
  );
  bool drawMeLastCoordinateAffine(
    bool initializeDrawingVariables,
    DrawingVariables& drawingVariables,
    FormatExpressions& format,
    const std::string& chamberWallColor = nullptr
  ) const;
  bool drawMeProjective(DrawingVariables& drawingVariables) const;
  bool drawMeProjectiveVertices(DrawingVariables& drawingVariables) const;
  bool drawMeProjectiveSlice(DrawingVariables& drawingVariables) const;
  bool isInCone(const Vector<Rational>& point) const;
  bool isInCone(const Vectors<Rational>& vertices) const;
  bool getLatticePointsInCone(
    Lattice& lattice,
    Vector<Rational>& shift,
    int upperBoundPointsInEachDim,
    bool lastCoordinateIsOne,
    Vectors<Rational>& outputPoints,
    Vector<Rational>* shiftAllPointsBy
  ) const;
  bool makeConvexHullOfMeAnd(const Cone& other);
  void changeBasis(Matrix<Rational>& linearMap);
  int getDimension() const;
  // Creates a cone from a set of walls.
  // When set, hasEnoughProjectiveVertices
  // indicates a mathematical promise (not checked for performance reasons)
  // that the projective cone, when made affine, is finite.
  bool createFromNormals(
    const Vectors<Rational>& inputNormals,
    const List<List<int> >& inputNeighbors,
    bool hasEnoughProjectiveVertices
  );
  // Returns false if the cone is non-proper, i.e. when either
  // 1) the cone is empty or is of smaller dimension than the dimension of the
  // ambient space
  // 2) the resulting cone is the entire space.
  bool createFromNormals(
    const Vectors<Rational>& inputNormals,
    const List<List<int> >& inputNeighbors
  );
  bool createFromWalls(
    const List<Wall>& inputWalls, bool hasEnoughProjectiveVertices
  );
  // If the chamber contains lines - i.e., does not have
  // "enough projective vertices", this function will
  // add fake walls to the chamber, so as to generate
  // extra points in the chamber.
  // These extra fake walls are removed later.
  // Returns the number of added fake walls.
  int addFakeWalls();
  void removeFakeWalls(int fakeWallCount);
  void removeNeighbor(int neighborId);
  bool hasMultipleNeighborWall() const;
  bool createFromVertices(const Vectors<Rational>& inputVertices);
  static void scaleNormalizeByPositive(Vector<Rational>& toScale);
  bool internalPoint(Vector<Rational>& output) const;
  Vector<Rational>& internalPointCached();
  Vector<Rational> internalPoint() const;
  Vector<Rational> internalPointNormal() const;
  unsigned int hashFunction() const;
  static unsigned int hashFunction(const Cone& input);
  bool haveCommonVertex(
    const Vector<Rational>& normal1, const Vector<Rational>& normal2
  );
  bool hasNormal(const Vector<Rational>& normalOfWall) const;
  bool produceNormalSeparatingWallsInDirection(
    const Vector<Rational>& slicingDirection,
    const Vector<Rational>& base,
    const Vector<Rational>& otherWall,
    Vector<Rational>& output
  ) const;
  Cone();
  Cone(int inputId);
  Vectors<Rational> getAllNormals() const;
  template <class TemplateList>
  void getNormalsOfWallsContainingPoint(
    const Vector<Rational>& point, TemplateList& output
  ) const {
    output.clear();
    for (const Wall& wall : this->walls) {
      if (wall.normal.scalarEuclidean(point).isEqualToZero()) {
        output.addOnTop(wall.normal);
      }
    }
  }
  List<List<int> > getAllNeighbors() const;
  void getAllNeighbors(HashedList<int>& output) const;
  void intersectHyperplane(
    Vector<Rational>& normal, Cone& outputConeLowerDimension
  );
  bool getRootFromLPolynomialConstantTermGoesToLastVariable(
    Polynomial<Rational>& inputLPoly, Vector<Rational>& output
  );
  bool solveLPolynomialEqualsZeroIAmProjective(
    Polynomial<Rational>& inputLPoly, Cone& outputCone
  );
  bool solveLQuasiPolyEqualsZeroIAmProjective(
    QuasiPolynomial& inputLQP,
    List<Cone>& outputConesOverEachLatticeShift
  );
  template <class ListTemplate>
  void getAllVerticesPerpendicularTo(
    const Vector<Rational>& normal, ListTemplate& output
  ) const {
    output.clear();
    for (const Vector<Rational>& vertex : this->vertices) {
      if (vertex.scalarEuclidean(normal).isEqualToZero()) {
        output.addOnTop(vertex);
      }
    }
  }
  bool checkConsistencyFull(const ConeCollection& owner) const;
  Wall& getWallWithNormalNoFail(const Vector<Rational>& desiredNormal);
  bool operator>(const Cone& other) const;
  bool operator==(const Cone& other) const;
  std::string toString(FormatExpressions* format = nullptr) const;
  std::string toStringNeighbors() const;
  std::string toHTML() const;
  // Determines whether the two Cones have a common irregular wall.
  // Assumptions made before this computation.
  // 1) Both cones have sufficiently many projective vertices.
  // 2) Both this cone and the candidate have all their vertices computed.
  // 3) This cone has a wall with the given normal, and the other neighbor has
  // a
  // wall with the opposite normal.
  bool hasCommonPossiblyIrregularWall(
    Cone& neighborCandidate, Vector<Rational>& normalOfWall
  );
  void addOneConeIfAdjacentToWall(Wall& wall, Cone& candidateNeighbor);
};

class ConeLatticeAndShift {
public:
  Cone projectivizedCone;
  Lattice lattice;
  Vector<Rational> shift;
  void findExtremaInDirectionOverLatticeOneNonParametric(
    Vector<Rational>& lpToMaximizeAffine,
    Vectors<Rational>& outputAppendLPToMaximizeAffine,
    List<ConeLatticeAndShift>& outputAppend
  );
  void findExtremaInDirectionOverLatticeOneNonParamDegenerateCase(
    Vector<Rational>& lpToMaximizeAffine,
    Vectors<Rational>& outputAppendLPToMaximizeAffine,
    List<ConeLatticeAndShift>& outputAppend,
    Matrix<Rational>& projectionLatticeLevel
  );
  bool readFromFile(std::fstream& input);
  std::string toString(FormatExpressions& format);
  void operator=(const ConeLatticeAndShift& other);
  int getDimensionProjectivized();
  int getDimensionAffine();
};

// A collection of cones associated with a given vector partition function.
class ConeCollection {
private:
  static void addCone(MapList<int, Cone>& map, const Cone& cone);
  bool checkConsistencyFullWithoutDebugMessage() const;
public:
  bool flagChambersHaveTooFewVertices;
  bool flagIsRefined;
  bool flagRecordSlicingHistory;
  // Governs whether slicing with planes that are not spanned
  // by n-1 directions is allowed.
  bool flagUseSpannedSlices;
  HashedList<Vector<Rational> > splittingNormals;
  Vectors<Rational> slicingDirections;
  Cone convexHull;
  MapList<int, Cone> nonRefinedCones;
  MapList<int, Cone> refinedCones;
  MapList<int, Cone> conesWithIrregularWalls;
  int conesCreated;
  List<std::string> historyHTML;
  class Statistics {
  public:
    int totalSplitFailuresDueToVisitOrder;
    int currentDirectionIndex;
    Statistics();
  };

  Statistics statistics;
  static const int maximumCones = 1000;
  void addHistoryPoint();
  // Returns false if the plane with the given normal
  // passes through directions that span the entire plane.
  bool isSpannedByDirections(const Vector<Rational>& planeNormal);
  void markAllConesNonRefined(int directionIndex);
  void refineByDirectionsAndSort();
  void refineByDirections(ProgressReport* report);
  void refineByOneDirection(int directionIndex, ProgressReport* report);
  void reportStats(ProgressReport* report);
  std::string toStringRefineStats();
  // Implementation of refineOneByOneDirection when only using spanned slices.
  void refineByOneDirectionSpannedSlices(int directionIndex);
  bool refineOneByOneDirectionSpannedSlices(
    Cone& toBeSliced, List<Wall>& exitWalls, int directionIndex
  );
  void mergeChambers();
  void mergeOneChamberFamily(HashedList<int>& family);
  void accountOneDirectionSimplex(
    Cone& directionSimplex,
    MapList<List<unsigned char>, HashedList<int> >& currentConeFamilies,
    MapList<List<unsigned char>, HashedList<int> >& nextConeFamilies
  );
  bool computeSplittingNormal(
    Cone& toBeSliced,
    List<Wall>& exitWalls,
    int directionIndex,
    Vector<Rational>& normal
  );
  bool isAddmissibleConeSplit(
    const Vector<Rational>& sliceNormal,
    const Vector<Rational>& wall1,
    const Vector<Rational>& wall2
  );
  // Returns false if the cone is refined relative to the splitting normals,
  // otherwise slices the cone.
  bool refineOneByNormals(Cone& toBeRefined, List<Cone>& output);
  // Returns true if a chamber is split so that
  // each chamber has unique exit wall with respect
  // to the given direction, or if it already has
  // has that property. Returns false if the chamber
  // cannot be split to have exit walls due to the
  // exit wall neighbors not being visited.
  bool refineOneByOneDirection(Cone& toBeRefined, int directionIndex);
  bool refineOneByOneDirectionArbitrarySlices(
    Cone& toBeRefined,
    List<Wall>& exitWalls,
    const Vector<Rational>& direction
  );
  void refineAllConesWithWallsWithMultipleNeighbors(ProgressReport* report);
  bool allExitWallsAreVisited(
    Cone& toBeRefined,
    const Vector<Rational>& direction,
    List<Wall>& outputExitWalls
  );
  void attachNeighbbors(
    const Cone& toBeReplaced, MapList<int, Cone>& allCandidates
  );
  void attachNeighbbors(
    const Cone& toBeReplaced, const List<Cone*>& newCones
  );
  void replaceConeAdjacentToWall(
    const Cone& toBeReplaced,
    const Wall& wall,
    const List<Cone*>& newCones
  );
  void replaceConeInNeighbor(
    Cone& neighbor,
    const Vector<Rational>& wallNormal,
    int idReplacedCone,
    const List<Cone*>& newCones
  );
  void addAllMutualNeighborsIfAdjacent(
    Cone& original, Wall& wallInOriginal, const List<Cone*>& incoming
  );
  void addMutualNeighborsIfAdjacent(
    Cone& original, Wall& wallInOriginal, Cone& incoming
  );
  void constructSubstituteChambers(
    List<Cone>& candidates, List<Cone>& output
  );
  void refineByNormals();
  void refineMakeCommonRefinement(const ConeCollection& other);
  void sort();
  void makeAffineAndTransformToProjectiveDimensionPlusOne(
    Vector<Rational>& affinePoint, ConeCollection& output
  );
  void transformToWeylProjective();
  int getDimension() const;
  void getAllWallsConesNoOrientationNoRepetitionNoSplittingNormals(
    Vectors<Rational>& output
  ) const;
  bool drawMeLastCoordinateAffine(
    bool initializeDrawingVariables,
    DrawingVariables& drawingVariables,
    FormatExpressions& format
  );
  void translateMeMyLastCoordinateAffinization(
    Vector<Rational>& translationVector
  );
  void initializeFromDirectionsAndRefine(Vectors<Rational>& inputVectors);
  void initializeFromDirections(
    const List<Vector<Rational> >& inputVectors
  );
  void initializeFromAffineDirectionsAndRefine(
    Vectors<Rational>& inputDirections,
    Vectors<Rational>& inputAffinePoints
  );
  std::string drawMeToHtmlLastCoordAffine(
    DrawingVariables& drawingVariables, FormatExpressions& format
  );
  bool containsId(int id) const;
  const Cone* getConeById(int id) const;
  Cone* getConeByIdNonConst(int id);
  Cone& getConeByIdNonConstNoFail(int id);
  bool drawMeProjectiveInitialize(DrawingVariables& drawingVariables) const;
  bool drawMeProjective(DrawingVariables& drawingVariables) const;
  bool drawProjectiveChambers(DrawingVariables& drawingVariables) const;
  std::string drawMeToHtmlProjective(
    DrawingVariables& drawingVariables, bool generateControls
  ) const;
  std::string toString() const;
  std::string toStringConeCount() const;
  std::string toStringConeIds() const;
  std::string toStringNeighborGraph() const;
  std::string toStringNeighborGraph(const MapList<int, Cone>& cones) const;
  // Returns an html canvas drawing of the cones plus detailed decription.
  std::string toHTML() const;
  // Returns the detailed cone description without the graphics.
  std::string toHTMLWithoutGraphics() const;
  // Returns the graphics of all the cones without description.
  std::string toHTMLGraphicsOnly(bool includePanels) const;
  // Returns a sequence of html canvas drawings of the various stages of
  // slicing.
  std::string toHTMLHistory() const;
  int getLowestIndexRefinedChamberContaining(const Vector<Rational>& root)
  const;
  bool findMaxLFOverConeProjective(
    const Cone& input,
    List<Polynomial<Rational> >& inputLinearPolynomials,
    List<int>& outputMaximumOverEeachSubChamber
  );
  bool findMaxLFOverConeProjective(
    const Cone& input,
    Vectors<Rational>& inputLFsLastCoordConst,
    List<int>& outputMaximumOverEeachSubChamber
  );
  void processNeighborsOfNewlyAddedCone(const Cone& cone);
  void addNonRefinedCone(const Cone& cone);
  void addRefinedCone(Cone& cone);
  // Creates a cone collection from a set of walls.
  // hasEnoughProjectiveVertices is a mathematical promise that
  // the input is of finite size when made affine.
  void initFromConeWalls(
    List<Vectors<Rational> >& normalsOfCones,
    bool hasEnoughProjectiveVertices
  );
  void initFromConeWalls(
    List<Cone>& normalsOfCones, bool hasEnoughProjectiveVertices
  );
  // Returns true if a chamber is sliced
  // in two by a given slicing plane.
  // If that is the case, slices the chamber
  // and writes the result in the output variable.
  bool splitChamber(
    const Cone& toBeSliced, const Vector<Rational>& normal
  );
  bool splitVerticesByNormal(
    const Cone& toBeSliced,
    const Vector<Rational>& normal,
    Vectors<Rational>& outputPlusCone,
    Vectors<Rational>& outputMinusCone,
    HashedList<Vector<Rational> >& outputZeroVertices
  );
  void removeIdFromNeighbors(const Cone& cone);
  void splitConeByMultipleNeighbors(Cone& input);
  void splitConeByMultipleNeighbors(Cone& input, Wall& wall);
  void getNewVerticesAppend(
    const Cone& toBeSplit,
    const Vector<Rational>& normalSlicingPlane,
    HashedList<Vector<Rational> >& outputVertices
  );
  void initialize();
  ConeCollection();
  bool checkIsRefinedOrCrash() const;
  // Checks the consistency of the cone collection.
  // Runs slow; avoid calling multiple times.
  // Will produce a global message to prevent you from
  // accidentally calling this too often.
  // Use checkConsistencyFullWithoutDebugMessage
  // to perform the check without leaving a log message
  // however with the caveat that you can blow up performance.
  bool checkConsistencyFull() const;
  bool checkConsistencyOneCollection(const MapList<int, Cone>& collection)
  const;
};

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
  // We define the n^th bernoulli sum to be
  // B_n(X) = \sum_{t=1}^X t^n.
  void getBernoulliSumStartingAtOne(int power, Polynomial<Rational>& output);
  // Same as the previous sum but starts at t=0.
  // C_n(X) = \sum_{t=0}^X t^n.
  // The two functions produce the same output, i.e., C_n(X)= B_n(X)
  // except for n=0 where
  // C_0(X) = X+1
  // but
  // B_0(X) = X.
  void getBernoulliSumStartingAtZero(int power, Polynomial<Rational>& output);
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
  bool computeStartingQuasipolynomial(Cone &cone);
  void getExitConesAfterStart(
    Cone& start, Vector<Rational>& direction, List<int>& output
  );
  void sumZeroQuasiPolynomialFromWall(
  const Vector<Rational>& direction,
  const Wall &exitWall,
  Cone& neighbor,
  QuasiPolynomial& output

  );
  void sumZeroQuasiPolynomialFromWallOnce(
      const Vector<Rational> &neighborShift,
      const Lattice &neighborLattice,
      const Polynomial<Rational> &pivotValue,
      const Vector<Rational> &representative,
      const Vector<Rational> &direction,
      const Vector<Rational> &normalRescaled,
      QuasiPolynomial &output);

  void sumQuasiPolynomialOverCone(Cone &cone,
                                  const Vector<Rational> &direction,
                                  const Wall &exitWall,
                                  QuasiPolynomial &output);
  void addSingleNeighborContribution(
    Cone& cone,
    const Vector<Rational>& direction,
    const Vector<Rational>& entranceWall,
    const Vector<Rational>& exitWall,
    QuasiPolynomial& outputAccumulator
  );
  void computeSingleNeighborSingleShiftContribution(
    const Polynomial<Rational>& toBeSubstituted,
    const Vector<Rational>& shift,
    const Lattice& ambientLattice,
    const Vector<Rational>& direction,
    const Vector<Rational>& exitWall,
    QuasiPolynomial& output
  );
  void computeOneQuasiPolynomialExitWallWithoutNeighborOneScale(
    const QuasiPolynomial& toBeIntegrated,
    int shift,
    int scale,
    QuasiPolynomial& outputAccumulator,
    const Vector<Rational>& direction,
    const Vector<Rational>& exitWall
  );
  void computeOneQuasiPolynomialExitWallWithoutNeighborOneScaleOneShift(
      const QuasiPolynomial &toBeIntegrated,
      const Rational &rationalShift,
      PolynomialSubstitution<Rational> &substitution,
      const Vector<Rational> &latticeShift,
      const Polynomial<Rational> &valueOnLatticeShift,
      QuasiPolynomial &outputAccumulator,
      const Vector<Rational> &exitWallRescaled,
      Lattice &rougherLattice);
};

class VectorPartitionFunction {
public:
  PartialFractions fractions;
  VectorPartitionFunctionElementary elementaryMethod;
  void initializeVectors(List<Vector<Rational> >& inputVectors);
  std::string toHTML() const;
};

class ConeLatticeAndShiftMaxComputation {
public:
  int numNonParaM;
  int numProcessedNonParam;
  List<ConeCollection> complexStartingPerRepresentative;
  List<ConeCollection> complexRefinedPerRepresentative;
  List<List<Vectors<Rational> > > maximaCandidates;
  List<Vectors<Rational> > startingLPtoMaximize;
  List<Vectors<Rational> > finalMaxima;
  Lattice finalRougherLattice;
  Vector<Rational> startingRepresentative;
  Vectors<Rational> finalRepresentatives;
  List<ConeLatticeAndShift> conesLargerDimension;
  List<ConeLatticeAndShift> conesSmallerDimension;
  List<List<ConeCollection> > finalMaximaChambers;
  List<List<List<int> > > finalMaximaChambersIndicesMaxFunctions;
  List<bool> isInfinity;
  Vectors<Rational> LPtoMaximizeLargerDim;
  Vectors<Rational> LPtoMaximizeSmallerDim;
  std::string toString(FormatExpressions* format = nullptr);
  void initialize(
    Vector<Rational>& inequalities,
    Cone& startingCone,
    Lattice& startingLattice,
    Vector<Rational>& startingShift
  );
  void findExtremaParametricStep1();
  void findExtremaParametricStep3();
  void findExtremaParametricStep4();
  void findExtremaParametricStep5();
};

class PiecewiseQuasipolynomial {
public:
  // Let the piecewise quasipolynomial be in n variables.
  // Then the projectivizedComplex is an n + 1-dimensional complex,
  // which is the projectivization of the n-dim affine complex representing the
  // domain of the piecewise quasipoly.
  ConeCollection projectivizedComplex;
  List<QuasiPolynomial> quasiPolynomials;
  int numberOfVariables;
  std::string toString(FormatExpressions* format = nullptr);
  void drawMe(
    DrawingVariables& drawingVariables,
    int numberOfLatticePointsPerDimension,
    Cone* restrictingChamber = nullptr,
    Vector<Rational>* distinguishedPoint = nullptr
  );
  int minimalNumberOfVariables() {
    return this->numberOfVariables;
  }
  void makeCommonRefinement(const PiecewiseQuasipolynomial& other) {
    this->makeCommonRefinement(other.projectivizedComplex);
  }
  bool checkConsistency() const {
    return true;
  }
  void makeCommonRefinement(const ConeCollection& other);
  void translateArgument(Vector<Rational>& translateToBeAddedToArgument);
  bool makeVPF(Vectors<Rational>& roots, std::string& outputstring);
  Rational evaluate(
    const Vector<Rational>& point, std::stringstream* comments
  );
  Rational evaluateInputProjectivized(
    const Vector<Rational>& point, std::stringstream* comments
  );
  void makeZero(int numberOfVariables) {
    this->numberOfVariables = numberOfVariables;
    this->projectivizedComplex.initialize();
    this->quasiPolynomials.size = 0;
  }
  void operator+=(const PiecewiseQuasipolynomial& other);
  void operator*=(const Rational& other);
};

#endif // header_math_vector_partition_functions_ALREADY_INCLUDED
