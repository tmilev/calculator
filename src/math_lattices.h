
#ifndef header_math_lattices_ALREADY_INCLUDED
#define header_math_lattices_ALREADY_INCLUDED

#include "math_general.h"

class Lattice {
  void testGaussianEliminationEuclideanDomainRationals(
    Matrix<Rational>& output
  );
public:
  Matrix<Rational> basisRationalForm;
  Matrix<LargeInteger> basis;
  LargeIntegerUnsigned denominator;
  int getDimension() const {
    return this->basis.numberOfColumns;
  }
  int getRank() const {
    return this->basis.numberOfRows;
  }
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
  // The following function follows the same convention
  // as the preceding except that we allow n < m. However,
  // in order to assure that the preimage of the lattice is a lattice,
  // we provide as input an ambient lattice
  // in the new vector space of dimension m.
  bool substitutionHomogeneous(
    const Matrix<Rational>& substitution, Lattice& resultIsSubsetOf
  );
  void subLatticeWithIntegralScalarProducts(
    const Vector<Rational>& mustHaveIntegerScalarProductWith,
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
  bool operator==(const Lattice& other) const;
  void operator=(const Lattice& other) {
    this->basis = other.basis;
    this->denominator = other.denominator;
    this->basisRationalForm = other.basisRationalForm;
  }
  void writeToFile(std::fstream& output);
  bool readFromFile(std::fstream& input);
  void makeZn(int dimension);
  void refineByOtherLattice(const Lattice& other);
  void makeFromRoots(const List<Vector<Rational> >& input);
  Lattice() {}
  Lattice(const Lattice& other) {
    this->operator=(other);
  }
  void makeFromMatrix(const Matrix<Rational>& input);
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
  std::string toString(FormatExpressions* format = nullptr);
  std::string toHTML(FormatExpressions* format = nullptr);
  Rational evaluate(
    const Vector<Rational>& input, std::stringstream* comments
  );
  void addLatticeShift(
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
  void makeZeroOverLattice(Lattice& lattice);
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
  void substituteFloorOfLinearFunction(
    const Vector<Rational>& scalarProductBy,
    const Rational& shift,
    QuasiPolynomial& output
  );
  bool substitutionFewerVariables(
    const PolynomialSubstitution<Rational>& substitution,
    QuasiPolynomial& output
  ) const;
  void operator+=(const QuasiPolynomial& other);
  QuasiPolynomial() {}
  QuasiPolynomial(const QuasiPolynomial& other) {
    this->operator=(other);
  }
  void operator*=(const Rational& scalar);
  void operator=(const QuasiPolynomial& other) {
    this->ambientLatticeReduced = other.ambientLatticeReduced;
    this->latticeShifts = other.latticeShifts;
    this->valueOnEachLatticeShift = other.valueOnEachLatticeShift;
  }
};

#endif // header_math_lattices_ALREADY_INCLUDED
