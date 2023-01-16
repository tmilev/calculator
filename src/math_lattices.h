
#ifndef header_math_lattices_ALREADY_INCLUDED
#define header_math_lattices_ALREADY_INCLUDED

#include "math_general.h"

class Lattice {
  void testGaussianEliminationEuclideanDomainRationals(
    Matrix<Rational>& output
  );
  void drawOnePointWithSigns(
    const Vector<Rational>& toDraw,
    const List<Vector<Rational> >& basis,
    DrawingVariables& output,
    int& outputTotalPointsDrawn,
    List<Vector<Rational> >* restrictingCone
  ) const;
  void drawOnePoint(
    const Vector<Rational>& inBasisCoordinates,
    const List<Vector<Rational> >& basis,
    const Selection& signs,
    DrawingVariables& output,
    int& outputTotalPointsDrawn,
    List<Vector<Rational> >* restrictingCone
  ) const;
  static void drawOnePointBase(
    const Vector<Rational>& toDraw,
    DrawingVariables& output,
    int& outputTotalPointsDrawn
  );
  static const int maximumPointsToDraw;
public:
  Matrix<Rational> basisRationalForm;
  Matrix<LargeInteger> basis;
  LargeIntegerUnsigned denominator;
  int getDimension() const;
  int getRank() const;
  int getMinimalIntegerScalarSendingVectorIntoLattice(
    const Vector<Rational>& input
  ) const;
  // Intersects a lattice with another and writes the result
  // in the caller lattice.
  void intersectWith(const Lattice& other);
  bool findOnePreimageInLatticeOf(
    const Matrix<Rational>& linearMap,
    const Vectors<Rational>& input,
    Vectors<Rational>& output
  );
  void intersectWithPreimageOfLattice(
    const Matrix<Rational>& linearMap, const Lattice& other
  );
  void intersectWithLineGivenBy(
    Vector<Rational>& inputLine, Vector<Rational>& outputGenerator
  );
  static bool
  getClosestPointInDirectionOfNormalToAffineWallMovingIntegralStepsInDirection(
    Vector<Rational>& startingPoint,
    Vector<Rational>& affineHyperplane,
    Vector<Rational>& direction,
    Vector<Rational>& outputPoint
  );
  void getDefaultFundamentalDomainInternalPoint(Vector<Rational>& output);
  bool getInternalPointInConeForSomeFundamentalDomain(
    Vector<Rational>& output, Cone& coneContainingOutputPoint
  );
  void getRootOnLatticeSmallestPositiveProportionalTo(
    Vector<Rational>& input, Vector<Rational>& output
  );
  void getRougherLatticeFromAffineHyperplaneDirectionAndLattice(
    const Vector<Rational>& direction,
    Vector<Rational>& outputDirectionMultipleOnLattice,
    Vector<Rational>& shift,
    Vector<Rational>& affineHyperplane,
    Vectors<Rational>& outputRepresentatives,
    Vectors<Rational>& movementInDirectionPerRepresentative,
    Lattice& outputRougherLattice
  );
  void applyLinearMap(Matrix<Rational>& linearMap, Lattice& output);
  void intersectWithBothOfMaximalRank(const Lattice& other);
  bool getDualLattice(Lattice& output) const;
  bool containsLattice(Lattice& other) const;
  bool isInLattice(const Vector<Rational>& vector) const {
    STACK_TRACE("Lattice::isInLattice");
    Vector<Rational> tempVect = vector;
    if (!this->reduceVector(tempVect)) {
      return false;
    }
    return tempVect.isEqualToZero();
  }
  // returns false if the vector is not in the vector space spanned by the
  // lattice
  bool reduceVector(Vector<Rational>& vector) const;
  // In the following two functions, the format of the matrix of the
  // substitution is as follows.
  // Let the ambient dimension be n, and the coordinates be x_1,..., x_n.
  // Let the new vector space be of dimension m, with coordinates y_1,..., y_m.
  // Then the substitution matrix is an n by m matrix,
  // where the i^th row of the matrix gives the expression of x_i via the
  // y_j's.
  // In addition, we require that n>=m (otherwise, in general, we do not expect
  // to get a lattice).
  // For example, if we want to carry out the substitution
  // x_1=y_1+y_2, x_2=y_1-y_2, x_3=y_1, then
  // the substitution matrix should be initialized as:
  // 1  1
  // 1 - 1
  // 1  0
  bool substitutionHomogeneous(const Matrix<Rational>& substitution);
  bool substitutionHomogeneous(
    const PolynomialSubstitution<Rational>& substitution
  );
  void subLatticeWithIntegralScalarProducts(
    const Vector<Rational>& mustHaveIntegerScalarProductWith,
    Lattice& output
  ) const;
  // Returns the sub-lattice of vectors:
  // { v \in L | <b,v>d \in M}
  // where
  // v is the bound variable that enumerates the set,
  // L -> this,
  // b -> scalarProductWith,
  // d -> direction
  // M -> target
  void subLatticeScalarProductTimesDirectionInLattice(
    const Vector<Rational>& scalarProductWith,
    const Vector<Rational>& direction,
    const Lattice& target,
    Lattice& output
  ) const;
  void reduce();
  void intersectWithLinearSubspaceSpannedBy(
    const Vectors<Rational>& subspaceBasis
  );
  void intersectWithLinearSubspaceGivenByNormals(
    const Vectors<Rational>& subspaceNormals
  );
  void intersectWithLinearSubspaceGivenByNormal(
    const Vector<Rational>& normal
  );
  static bool
  getHomogeneousSubstitutionMatrixFromSubstitutionIgnoreConstantTerms(
    const PolynomialSubstitution<Rational>& substitution,
    Matrix<Rational>& output
  );
  // Returning false means that the lattice given as rougher
  // is not actually rougher than the current one
  // or that there are too many representatives.
  bool getAllRepresentatives(
    const Lattice& rougherLattice, Vectors<Rational>& output
  ) const;
  bool getAllRepresentativesProjectingDownTo(
    const Lattice& rougherLattice,
    Vectors<Rational>& startingShifts,
    Vectors<Rational>& output
  ) const;
  std::string toString() const;
  std::string toStringParentheses() const;
  static unsigned int hashFunction(const Lattice& input);
  bool operator!=(const Lattice& other) const;
  bool operator==(const Lattice& other) const;
  void writeToFile(std::fstream& output);
  bool readFromFile(std::fstream& input);
  void makeZn(int dimension);
  void refineByOtherLattice(const Lattice& other);
  void refineByVector(const Vector<Rational>& other);
  void makeFromRoots(const List<Vector<Rational> >& input);
  void makeFromMatrix(const Matrix<Rational>& input);
  // Draws the lattice. If a restricting cone is given,
  // all points outside of if will be omitted.
  // The restricting cone is specified by giving the normals of its walls.
  void draw(
    DrawingVariables& output, List<Vector<Rational> >* restrictingCone
  ) const;
  static void scaleNormalizeByPositive(Vector<Rational>& toScale);
};

class QuasiPolynomial {
public:
  int minimalNumberOfVariables() const {
    return this->ambientLatticeReduced.basis.numberOfRows;
  }
  Lattice ambientLatticeReduced;
  Vectors<Rational> latticeShifts;
  List<Polynomial<Rational> > valueOnEachLatticeShift;
  std::string toString(FormatExpressions* format = nullptr) const;
  std::string toHTML(FormatExpressions* format = nullptr) const;
  std::string toLatex(FormatExpressions* format = nullptr) const;
  void sortLatticeShifts();
  void combineLatticeShifts(
    MapList<Polynomial<Rational>, List<Vector<Rational> > >& output
  ) const;
  Rational evaluate(
    const Vector<Rational>& input, std::stringstream* comments
  ) const;
  void addLatticeShift(
    const Polynomial<Rational>& input,
    const Vector<Rational>& inputShift
  );
  void setLatticeShift(
    const Polynomial<Rational>& input,
    const Vector<Rational>& inputShift
  );
  void makeRougherLattice(const Lattice& latticeToRoughenBy);
  void makeFromPolynomialShiftAndLattice(
    const Polynomial<Rational>& inputPoly,
    const MonomialPolynomial& shift,
    const Lattice& lattice
  );
  void makeZeroLatticeZn(int dimension);
  void makeZeroOverLattice(const Lattice& lattice);
  bool isEqualToZero() const {
    return this->valueOnEachLatticeShift.size == 0;
  }
  void substitute(
    const Matrix<Rational>& mapFromNewSpaceToOldSpace,
    const Vector<Rational>& inputTranslationSubtractedFromArgument,
    const Lattice& ambientLatticeNewSpace,
    QuasiPolynomial& output
  );
  void substitute(
    const Matrix<Rational>& mapFromNewSpaceToOldSpace,
    const Lattice& ambientLatticeNewSpace,
    QuasiPolynomial& output
  );
  void substitute(
    const Vector<Rational>& inputTranslationSubtractedFromArgument,
    QuasiPolynomial& output
  );
  // The transformation f(t)  -> i_{delta + Lambda} (v) f(Floor(<b,v>+c))
  // Here, v are the variables of the quasipolynomial.
  // f -> univariateInput, must be univariate.
  // v -> scalarProductBy.
  // c -> constantTerm.
  // Lambda -> lattice.
  // delta -> latticeShift
  // this will hold the result of the computation.
  void assignPolynomialOfFloorOfLinearFunction(
    const Polynomial<Rational>& univariateInput,
    const Vector<Rational>& scalarProductBy,
    const Rational& constantTerm,
    const Lattice& lattice,
    const Vector<Rational>& latticeShift
  );
  // The transformation Q(v) -> Q(v- Floor(<b,v>+c)d).
  // Here, v are the variables of the quasipolynomial.
  // b -> scalarProductBy.
  // c -> shift.
  // d -> direction.
  // this -> the original input.
  // This kind of substitution is the main tool for elementary
  // computation with vector partition functions.
  void substituteShiftByFloorOfLinearFunction(
    const Vector<Rational>& scalarProductBy,
    const Rational& shift,
    const Vector<Rational>& direction,
    QuasiPolynomial& output,
    List<std::string>* comments
  ) const;
  void substituteShiftByFloorOfLinearFunctionOnce(
    const Vector<Rational>& scalarProductBy,
    const Rational& shift,
    const Vector<Rational>& direction,
    const Polynomial<Rational>& startingValueOnLattice,
    const Vector<Rational>& startingLatticeShift,
    const Vector<Rational>& representativeScalarProductInZ,
    const Lattice& scalarProductInZ,
    const Vector<Rational>& representativeScalarProductTimesDirectionInLambda,
    const Lattice& scalarProductTimesDirectionInLambda,
    QuasiPolynomial& outputAccumulator,
    List<std::string>* comments
  ) const;
  bool substitutionFewerVariables(
    const PolynomialSubstitution<Rational>& substitution,
    QuasiPolynomial& output
  ) const;
  bool compress();
  bool compressOnce();
  void getValueOnShift(
    const Vector<Rational>& shift, Polynomial<Rational>& output
  ) const;
  bool compressWithRespectToPeriod(const Vector<Rational>& period);
  void operator+=(const QuasiPolynomial& other);
  void operator-=(const QuasiPolynomial& other);
  void operator*=(const Rational& scalar);
  void operator*=(const Polynomial<Rational>& other);
  void operator*=(const QuasiPolynomial& other);
  // Does basic correctness checks. This is an expensive function
  // and will generate a global comment so you don't call this function too
  // much.
  bool checkConsistency() const;
  // Same as checkConsistency but does not make a debug message.
  bool checkConsistencyWithoutDebugMessage() const;
};

#endif // header_math_lattices_ALREADY_INCLUDED
