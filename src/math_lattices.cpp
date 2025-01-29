#include "math_extra_drawing_variables.h"
#include "math_general.h"
#include "math_general_implementation.h" // IWYU pragma: keep: breaks the build.
#include "math_general_polynomial_computations_basic_implementation.h" // IWYU pragma: keep: breaks the build.
#include "math_lattices.h"

const int Lattice::maximumPointsToDraw = 1024;

void QuasiPolynomial::addLatticeShift(
  const Polynomial<Rational>& input, const Vector<Rational>& inputShift
) {
  STACK_TRACE("QuasiPolynomial::addLatticeShift");
  Vector<Rational> shift = inputShift;
  this->ambientLatticeReduced.reduceVector(shift);
  int index = this->latticeShifts.getIndex(shift);
  if (index == - 1) {
    index = this->latticeShifts.size;
    Polynomial<Rational> zero;
    this->latticeShifts.addOnTop(shift);
    this->valueOnEachLatticeShift.addOnTop(zero);
  }
  this->valueOnEachLatticeShift[index] += input;
  if (this->valueOnEachLatticeShift[index].isEqualToZero()) {
    this->latticeShifts.removeIndexSwapWithLast(index);
    this->valueOnEachLatticeShift.removeIndexSwapWithLast(index);
  }
}

void QuasiPolynomial::setLatticeShift(
  const Polynomial<Rational>& input, const Vector<Rational>& inputShift
) {
  STACK_TRACE("QuasiPolynomial::setLatticeShift");
  Vector<Rational> shift = inputShift;
  this->ambientLatticeReduced.reduceVector(shift);
  int index = this->latticeShifts.getIndex(shift);
  if (index == - 1) {
    this->addLatticeShift(input, inputShift);
    return;
  }
  this->valueOnEachLatticeShift[index] = input;
}

void QuasiPolynomial::substitute(
  const Matrix<Rational>& mapFromNewSpaceToOldSpace,
  const Lattice& ambientLatticeNewSpace,
  QuasiPolynomial& output
) {
  STACK_TRACE("QuasiPolynomial::substitution");
  // Format of the substitution.
  // If we want to carry out a substitution in P(y_1, ..., y_m),
  // of the form
  // y_1 = a_11 x_1+...+a_1n x_n
  // ...
  // y_m = a_m1 x_1+...+a_mn x_n
  // then the mapFromNewSpaceToOldSpace should be the matrix
  // a_11 ... a_1n
  // ...
  // a_m1 ... a_mn
  if (
    this == &output ||
    mapFromNewSpaceToOldSpace.numberOfRows != this->minimalNumberOfVariables()
    ||
    ambientLatticeNewSpace.getDimension() !=
    mapFromNewSpaceToOldSpace.numberOfColumns
  ) {
    global.fatal << "Bad lattice dimensions. " << global.fatal;
  }
  output.ambientLatticeReduced = ambientLatticeNewSpace;
  output.ambientLatticeReduced.intersectWithPreimageOfLattice(
    mapFromNewSpaceToOldSpace, this->ambientLatticeReduced
  );
  Vectors<Rational> allRepresentatives;
  Vectors<Rational> imagesAllRepresentatives;
  bool tempBool =
  ambientLatticeNewSpace.getAllRepresentatives(
    output.ambientLatticeReduced, allRepresentatives
  );
  if (!tempBool) {
    global.fatal << "Failed to get all representatives. " << global.fatal;
  }
  mapFromNewSpaceToOldSpace.actOnVectorsColumn(
    allRepresentatives, imagesAllRepresentatives
  );
  PolynomialSubstitution<Rational> substitution;
  substitution.setSize(this->minimalNumberOfVariables());
  Vector<Rational> root;
  for (int i = 0; i < substitution.size; i ++) {
    Polynomial<Rational>& currentPolynomial = substitution[i];
    mapFromNewSpaceToOldSpace.getVectorFromRow(i, root);
    currentPolynomial.makeLinearNoConstant(root);
  }
  Polynomial<Rational> polynomial;
  for (int i = 0; i < this->valueOnEachLatticeShift.size; i ++) {
    polynomial = this->valueOnEachLatticeShift[i];
    bool mustBeTrue = polynomial.substitute(substitution, Rational::one());
    if (!mustBeTrue) {
      global.fatal
      << "Substitution "
      << substitution.toString()
      << " into polynomial "
      << polynomial.toString()
      << " failed but the current function does not handle this properly. "
      << global.fatal;
    }
    for (int j = 0; j < allRepresentatives.size; j ++) {
      if (imagesAllRepresentatives[j] == this->latticeShifts[i]) {
        output.addLatticeShift(polynomial, allRepresentatives[j]);
      }
    }
  }
}

void QuasiPolynomial::substitute(
  const Vector<Rational>& inputTranslationSubtractedFromArgument,
  QuasiPolynomial& output
) {
  STACK_TRACE("QuasiPolynomial::substitute");
  // format of the translation. If the starting quasipolynomial was P(y_1, ...,
  // y_n),
  // and the translation has coordinates (t_1, ..., t_n),
  // then the resulting quasipolynomial will be P(x_1-t_1, ..., x_n-t_n)
  PolynomialSubstitution<Rational> substitution;
  substitution.makeIdentitySubstitution(
    this->minimalNumberOfVariables(), Rational(1)
  );
  for (int i = 0; i < substitution.size; i ++) {
    substitution[i].addConstant(- inputTranslationSubtractedFromArgument[i]);
  }
  Polynomial<Rational> polynomial;
  output.makeZeroLatticeZn(this->minimalNumberOfVariables());
  output.ambientLatticeReduced = this->ambientLatticeReduced;
  for (int i = 0; i < this->valueOnEachLatticeShift.size; i ++) {
    polynomial = this->valueOnEachLatticeShift[i];
    bool mustBeTrue = polynomial.substitute(substitution, Rational::one());
    if (!mustBeTrue) {
      global.fatal
      << "Substitution "
      << substitution.toString()
      << " into polynomial "
      << polynomial.toString()
      << " failed but the current function does not handle this properly. "
      << global.fatal;
    }
    output.addLatticeShift(
      polynomial,
      this->latticeShifts[i] + inputTranslationSubtractedFromArgument
    );
  }
}

void QuasiPolynomial::substitute(
  const Matrix<Rational>& mapFromNewSpaceToOldSpace,
  const Vector<Rational>& inputTranslationSubtractedFromArgument,
  const Lattice& ambientLatticeNewSpace,
  QuasiPolynomial& output
) {
  QuasiPolynomial quasiPolynomial;
  this->substitute(inputTranslationSubtractedFromArgument, quasiPolynomial);
  quasiPolynomial.substitute(
    mapFromNewSpaceToOldSpace, ambientLatticeNewSpace, output
  );
}

void QuasiPolynomial::assignPolynomialOfFloorOfLinearFunction(
  const Polynomial<Rational>& univariateInput,
  const Vector<Rational>& scalarProductBy,
  const Rational& constantTerm,
  const Lattice& lattice,
  const Vector<Rational>& latticeShift
) {
  STACK_TRACE("QuasiPolynomial::assignPolynomialOfFloorOfLinearFunction");
  int dimension = scalarProductBy.size;
  if (dimension != lattice.getDimension()) {
    global.fatal
    << "The lattice must live in space of "
    << "dimension equal to the number of variables. "
    << global.fatal;
  }
  lattice.subLatticeWithIntegralScalarProducts(
    scalarProductBy, this->ambientLatticeReduced
  );
  this->makeZeroOverLattice(this->ambientLatticeReduced);
  Lattice zN;
  zN.makeZn(dimension);
  Vectors<Rational> representatives;
  zN.getAllRepresentatives(this->ambientLatticeReduced, representatives);
  PolynomialSubstitution<Rational> substitution;
  substitution.setSize(1);
  Polynomial<Rational> summand;
  for (Vector<Rational>& representative : representatives) {
    if (!lattice.isInLattice(representative - latticeShift)) {
      continue;
    }
    Rational scalarProduct = representative.scalarEuclidean(scalarProductBy);
    Rational subtracand =
    scalarProduct -(scalarProduct + constantTerm).floor();
    substitution[0].makeLinearNoConstant(scalarProductBy);
    substitution[0] -= subtracand;
    summand = univariateInput;
    summand.substitute(substitution, 1);
    this->addLatticeShift(summand, representative);
  }
}

void QuasiPolynomial::substituteShiftByFloorOfLinearFunction(
  const Vector<Rational>& scalarProductBy,
  const Rational& shift,
  const Vector<Rational>& direction,
  QuasiPolynomial& output,
  List<std::string>* comments
) const {
  STACK_TRACE("QuasiPolynomial::substituteShiftByFloorOfLinearFunction");
  if (this->ambientLatticeReduced.getDimension() != scalarProductBy.size) {
    global.fatal
    << "The lattice of the quasipolynomial must be of full rank. "
    << global.fatal;
  }
  if (!this->ambientLatticeReduced.denominator.isEqualToOne()) {
    global.fatal
    << "The starting lattice is not integral. "
    << "Mathematically, this function requires a natural fine lattice "
    << "that contains the ambient lattice of the quasipolynomial. "
    << "To make the inputs of the function simpler, "
    << "we restrict it to the case "
    << "of integral lattice."
    << global.fatal;
  }
  Lattice zN;
  Lattice scalarProductInZ;
  Lattice scalarProductTimesDirectionInLambda;
  zN.makeZn(scalarProductBy.size);
  this->ambientLatticeReduced.subLatticeWithIntegralScalarProducts(
    scalarProductBy, scalarProductInZ
  );
  this->ambientLatticeReduced.subLatticeScalarProductTimesDirectionInLattice(
    scalarProductBy,
    direction,
    this->ambientLatticeReduced,
    scalarProductTimesDirectionInLambda
  );
  Vectors<Rational> representativesScalarProductInZ;
  Vectors<Rational> representativesScalarProductTimesDirectionInLambda;
  zN.getAllRepresentatives(
    scalarProductTimesDirectionInLambda,
    representativesScalarProductTimesDirectionInLambda
  );
  zN.getAllRepresentatives(scalarProductInZ, representativesScalarProductInZ);
  output.makeZeroLatticeZn(scalarProductBy.size);
  if (comments != nullptr) {
    std::stringstream commentStream1;
    std::stringstream commentStream2;
    commentStream1
    << "Computing \\(Q(\\alpha-\\lfloor\\langle"
    << scalarProductBy
    << ", \\alpha \\rangle +"
    << shift
    << "\\rfloor"
    << direction
    << ")\\)";
    commentStream2 << "where \\(Q(\\alpha)=\\)";
    comments->addOnTop(commentStream1.str());
    comments->addOnTop(commentStream2.str());
    comments->addOnTop(this->toHTML());
  }
  for (int i = 0; i < this->latticeShifts.size; i ++) {
    for (
      Vector<Rational>& representativeScalarProductInZ :
      representativesScalarProductInZ
    ) {
      for (
        Vector<Rational>& representativeScalarProductTimesDirectionInLambda :
        representativesScalarProductTimesDirectionInLambda
      ) {
        this->substituteShiftByFloorOfLinearFunctionOnce(
          scalarProductBy,
          shift,
          direction,
          this->valueOnEachLatticeShift[i],
          this->latticeShifts[i],
          representativeScalarProductInZ,
          scalarProductInZ,
          representativeScalarProductTimesDirectionInLambda,
          scalarProductTimesDirectionInLambda,
          output,
          comments
        );
      }
    }
  }
}

void QuasiPolynomial::substituteShiftByFloorOfLinearFunctionOnce(
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
) const {
  STACK_TRACE("QuasiPolynomial::substituteShiftByFloorOfLinearFunctionOnce");
  Vector<Rational> shiftVector = startingLatticeShift;
  Rational scalarProductIntermediate =
  scalarProductBy.scalarEuclidean(representativeScalarProductInZ);
  Rational floorPart = (scalarProductIntermediate + shift);
  floorPart.assignFloor();
  Rational shiftContribution = scalarProductIntermediate - floorPart;
  shiftVector +=
  direction *(
    scalarProductBy.scalarEuclidean(
      representativeScalarProductTimesDirectionInLambda
    ) -
    shiftContribution
  );
  QuasiPolynomial indicator1;
  QuasiPolynomial indicator2;
  QuasiPolynomial indicator3;
  indicator1.makeZeroOverLattice(scalarProductInZ);
  indicator2.makeZeroOverLattice(scalarProductTimesDirectionInLambda);
  indicator3.makeZeroOverLattice(this->ambientLatticeReduced);
  Polynomial<Rational> one;
  one.makeOne();
  indicator1.addLatticeShift(one, representativeScalarProductInZ);
  indicator2.addLatticeShift(
    one, representativeScalarProductTimesDirectionInLambda
  );
  indicator3.addLatticeShift(one, shiftVector);
  QuasiPolynomial summand = indicator1;
  summand *= indicator2;
  summand *= indicator3;
  if (summand.isEqualToZero()) {
    return;
  }
  PolynomialSubstitution<Rational> substitution;
  int dimension = scalarProductBy.size;
  substitution.setSize(dimension);
  Polynomial<Rational> linearFunction;
  for (int i = 0; i < dimension; i ++) {
    Polynomial<Rational>& current = substitution[i];
    current.makeMonomial(i, 1, 1);
    linearFunction.makeLinearNoConstant(scalarProductBy);
    linearFunction -= shiftContribution;
    linearFunction *= direction[i];
    current -= linearFunction;
  }
  Polynomial<Rational> substituted = startingValueOnLattice;
  substituted.substitute(substitution, 1);
  summand *= substituted;
  if (comments != nullptr) {
    std::stringstream substitutionStream;
    substitutionStream
    << "over: \\("
    << representativeScalarProductTimesDirectionInLambda
    << "+"
    << outputAccumulator.ambientLatticeReduced.toString()
    << "\\)"
    << " with lattice shift: \\("
    << startingLatticeShift.toString()
    << "\\)"
    << "<br>";
    substitutionStream
    << "\\("
    << substitution.toString()
    << startingValueOnLattice.toString()
    << "\\mapsto "
    << substituted.toString()
    << "\\)";
    comments->addOnTop(substitutionStream.str());
  }
  outputAccumulator += summand;
}

void QuasiPolynomial::sortLatticeShifts() {
  this->latticeShifts.quickSortAscending(
    nullptr, &this->valueOnEachLatticeShift
  );
}

void QuasiPolynomial::combineLatticeShifts(
  MapList<Polynomial<Rational>, List<Vector<Rational> > >& output
) const {
  STACK_TRACE("QuasiPolynomial::combineLatticeShifts");
  output.clear();
  List<Vector<Rational> > empty;
  for (int i = 0; i < this->latticeShifts.size; i ++) {
    const Polynomial<Rational>& current = this->valueOnEachLatticeShift[i];
    if (!output.contains(current)) {
      output.setKeyValue(current, empty);
    }
    output.getValueNoFailNonConst(current).addOnTop(this->latticeShifts[i]);
  }
}

bool QuasiPolynomial::compress() {
  STACK_TRACE("QuasiPolynomial::compress");
  if (!this->ambientLatticeReduced.basisRationalForm.isSquare()) {
    return false;
  }
  bool result = false;
  while (this->compressOnce()) {
    result = true;
  }
  return result;
}

bool QuasiPolynomial::compressOnce() {
  STACK_TRACE("QuasiPolynomial::compressOnce");
  for (int i = 0; i < this->latticeShifts.size; i ++) {
    for (int j = i + 1; j < this->latticeShifts.size; j ++) {
      if (this->valueOnEachLatticeShift[i] != this->valueOnEachLatticeShift[j])
      {
        continue;
      }
      if (
        this->compressWithRespectToPeriod(
          this->latticeShifts[i] - this->latticeShifts[j]
        )
      ) {
        return true;
      }
    }
  }
  return false;
}

void QuasiPolynomial::getValueOnShift(
  const Vector<Rational>& shift, Polynomial<Rational>& output
) const {
  Vector<Rational> reduced = shift;
  this->ambientLatticeReduced.reduceVector(reduced);
  for (int i = 0; i < this->latticeShifts.size; i ++) {
    if (reduced == this->latticeShifts[i]) {
      output = this->valueOnEachLatticeShift[i];
      return;
    }
  }
  output.makeZero();
}

bool QuasiPolynomial::compressWithRespectToPeriod(
  const Vector<Rational>& period
) {
  QuasiPolynomial result;
  result.ambientLatticeReduced = this->ambientLatticeReduced;
  result.ambientLatticeReduced.refineByVector(period);
  Polynomial<Rational> valueShifted;
  for (int i = 0; i < this->latticeShifts.size; i ++) {
    Vector<Rational> shift = this->latticeShifts[i];
    result.ambientLatticeReduced.reduceVector(shift);
    this->getValueOnShift(shift, valueShifted);
    if (!this->valueOnEachLatticeShift[i].isEqualTo(valueShifted)) {
      return false;
    }
    result.setLatticeShift(valueShifted, shift);
  }
  *this = result;
  return true;
}

bool QuasiPolynomial::substitutionFewerVariables(
  const PolynomialSubstitution<Rational>& substitution, QuasiPolynomial& output
) const {
  Matrix<Rational> latticeSubstitution;
  if (
    !this->ambientLatticeReduced.
    getHomogeneousSubstitutionMatrixFromSubstitutionIgnoreConstantTerms(
      substitution, latticeSubstitution
    )
  ) {
    return false;
  }
  Matrix<Rational> subLatticeShift;
  output.ambientLatticeReduced = this->ambientLatticeReduced;
  if (
    !output.ambientLatticeReduced.substitutionHomogeneous(
      latticeSubstitution
    )
  ) {
    return false;
  }
  subLatticeShift.initialize(latticeSubstitution.numberOfRows, 1);
  for (int i = 0; i < subLatticeShift.numberOfRows; i ++) {
    substitution[i].constantTerm(subLatticeShift.elements[i][0], Rational(0));
  }
  Matrix<Rational> shiftImage;
  Matrix<Rational> shiftMatrixForm;
  output.latticeShifts.size = 0;
  output.valueOnEachLatticeShift.size = 0;
  output.valueOnEachLatticeShift.reserve(this->latticeShifts.size);
  output.latticeShifts.reserve(this->latticeShifts.size);
  Vector<Rational> root;
  Polynomial<Rational> valueOnShift;
  for (int i = 0; i < this->latticeShifts.size; i ++) {
    shiftMatrixForm.assignVectorColumn(this->latticeShifts[i]);
    shiftMatrixForm -= subLatticeShift;
    if (
      latticeSubstitution.
      solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(
        latticeSubstitution, shiftMatrixForm, shiftImage
      )
    ) {
      root.assignMatrixDetectRowOrColumn(shiftImage);
      valueOnShift = this->valueOnEachLatticeShift[i];
      bool shouldBeTrue =
      valueOnShift.substitute(substitution, Rational::one());
      if (!shouldBeTrue) {
        global.fatal
        << "Substitution "
        << substitution.toString()
        << " into polynomial "
        << valueOnShift.toString()
        << " failed but the current function does not handle this properly. "
        << global.fatal;
      }
      output.addLatticeShift(valueOnShift, root);
    }
  }
  return true;
}

void QuasiPolynomial::operator*=(const Rational& scalar) {
  if (scalar.isEqualToZero()) {
    this->valueOnEachLatticeShift.size = 0;
    this->latticeShifts.size = 0;
    return;
  }
  for (int i = 0; i < this->valueOnEachLatticeShift.size; i ++) {
    this->valueOnEachLatticeShift[i] *= scalar;
  }
}

void QuasiPolynomial::operator*=(const Polynomial<Rational>& scalar) {
  if (scalar.isEqualToZero()) {
    *this *= (Rational) 0;
    return;
  }
  for (int i = 0; i < this->valueOnEachLatticeShift.size; i ++) {
    this->valueOnEachLatticeShift[i] *= scalar;
  }
}

void QuasiPolynomial::operator*=(const QuasiPolynomial& other) {
  if (other.isEqualToZero()) {
    this->valueOnEachLatticeShift.size = 0;
    this->latticeShifts.size = 0;
    return;
  }
  QuasiPolynomial left = *this;
  QuasiPolynomial right = other;
  left.makeRougherLattice(other.ambientLatticeReduced);
  right.makeRougherLattice(this->ambientLatticeReduced);
  if (!(left.ambientLatticeReduced == right.ambientLatticeReduced)) {
    global.fatal << "Ambient lattices must be equal. " << global.fatal;
  }
  this->makeZeroOverLattice(left.ambientLatticeReduced);
  Polynomial<Rational> summand;
  for (int i = 0; i < left.latticeShifts.size; i ++) {
    for (int j = 0; j < right.latticeShifts.size; j ++) {
      if (left.latticeShifts[i] != right.latticeShifts[j]) {
        continue;
      }
      summand =
      left.valueOnEachLatticeShift[i] * right.valueOnEachLatticeShift[j];
      this->addLatticeShift(summand, left.latticeShifts[i]);
    }
  }
}

bool QuasiPolynomial::checkConsistency() const {
  STACK_TRACE("QuasiPolynomial::checkConsistency");
  global.comments << "<br>Expensive consistency check. ";
  return this->checkConsistencyWithoutDebugMessage();
}

bool QuasiPolynomial::checkConsistencyWithoutDebugMessage() const {
  STACK_TRACE("QuasiPolynomial::checkConsistencyWithoutDebugMessage");
  for (int i = 0; i < this->latticeShifts.size; i ++) {
    Vector<Rational> shift1 = this->latticeShifts[i];
    this->ambientLatticeReduced.reduceVector(shift1);
    if (shift1 != this->latticeShifts[i]) {
      global.fatal
      << "Non-reduced lattice shift: "
      << this->latticeShifts[i]
      << " not reduced over: "
      << this->ambientLatticeReduced.toStringParentheses()
      << global.fatal;
    }
    for (int j = i + 1; j < this->latticeShifts.size; j ++) {
      const Vector<Rational>& shift2 = this->latticeShifts[j];
      if (!this->ambientLatticeReduced.isInLattice(shift1 - shift2)) {
        continue;
      }
      global.fatal
      << "Difference of two shifts is in the lattice: "
      << shift1
      << " - "
      << shift2
      << ". Lattice: "
      << this->ambientLatticeReduced.toString()
      << global.fatal;
    }
  }
  return true;
}

void QuasiPolynomial::makeRougherLattice(const Lattice& latticeToRoughenBy) {
  STACK_TRACE("QuasiPolynomial::makeRougherLattice");
  if (this->ambientLatticeReduced == latticeToRoughenBy) {
    return;
  }
  Lattice oldLattice;
  oldLattice = this->ambientLatticeReduced;
  this->ambientLatticeReduced.intersectWith(latticeToRoughenBy);
  Vectors<Rational> representativesQuotientLattice;
  oldLattice.getAllRepresentatives(
    this->ambientLatticeReduced, representativesQuotientLattice
  );
  Vectors<Rational> oldLatticeShifts = this->latticeShifts;
  List<Polynomial<Rational> > oldValues;
  oldValues = this->valueOnEachLatticeShift;
  this->latticeShifts.setSize(
    oldLatticeShifts.size * representativesQuotientLattice.size
  );
  this->valueOnEachLatticeShift.setSize(this->latticeShifts.size);
  for (int i = 0; i < oldLatticeShifts.size; i ++) {
    for (int j = 0; j < representativesQuotientLattice.size; j ++) {
      int index = i * representativesQuotientLattice.size + j;
      this->latticeShifts[index] =
      oldLatticeShifts[i] + representativesQuotientLattice[j];
      this->valueOnEachLatticeShift[index] = oldValues[i];
      this->ambientLatticeReduced.reduceVector(this->latticeShifts[index]);
    }
  }
}

void QuasiPolynomial::operator+=(const QuasiPolynomial& other) {
  STACK_TRACE("QuasiPolynomial::operator+=");
  this->makeRougherLattice(other.ambientLatticeReduced);
  QuasiPolynomial summand = other;
  summand.makeRougherLattice(this->ambientLatticeReduced);
  if (summand.ambientLatticeReduced != this->ambientLatticeReduced) {
    global.fatal << "Lattices must be equal." << global.fatal;
  }
  for (int i = 0; i < summand.latticeShifts.size; i ++) {
    this->addLatticeShift(
      summand.valueOnEachLatticeShift[i], summand.latticeShifts[i]
    );
  }
}

void QuasiPolynomial::operator-=(const QuasiPolynomial& other) {
  this->makeRougherLattice(other.ambientLatticeReduced);
  QuasiPolynomial summand = other;
  summand.makeRougherLattice(this->ambientLatticeReduced);
  for (int i = 0; i < summand.latticeShifts.size; i ++) {
    this->addLatticeShift(
      summand.valueOnEachLatticeShift[i] *(Rational)(- 1),
      summand.latticeShifts[i]
    );
  }
}

std::string QuasiPolynomial::toString(FormatExpressions* format) const {
  std::stringstream out;
  if (this->latticeShifts.size == 0) {
    return "0";
  }
  out << "\\begin{array}{l}\n";
  for (int i = 0; i < this->latticeShifts.size; i ++) {
    out << this->valueOnEachLatticeShift[i].toString(format);
    out << "\\\\\n \\text{~over~}";
    if (!this->latticeShifts[i].isEqualToZero()) {
      out << this->latticeShifts[i].toString() << " + ";
    }
    out << "\\Lambda, \\\\\\hline\n ";
    if (i != this->latticeShifts.size - 1) {
      out << "\\\\";
    }
  }
  if (!this->ambientLatticeReduced.basisRationalForm.isIdentity()) {
    out << ", \\mathrm{~where~} \\Lambda = \\langle ";
    Vectors<Rational> roots;
    roots.assignMatrixRows(this->ambientLatticeReduced.basisRationalForm);
    for (int i = 0; i < roots.size; i ++) {
      out << roots[i].toString();
      if (i != roots.size - 1) {
        out << ", ";
      }
    }
    out << "\\rangle";
  } else {
    out
    << "\\Lambda =\\mathbb{Z}^{"
    << this->minimalNumberOfVariables()
    << "}";
  }
  out << "\\end{array}";
  return out.str();
}

std::string QuasiPolynomial::toHTML(FormatExpressions* format) const {
  std::stringstream out;
  if (this->latticeShifts.size == 0) {
    return "0";
  }
  for (int i = 0; i < this->latticeShifts.size; i ++) {
    out << "\\(" << this->valueOnEachLatticeShift[i].toString(format) << "\\)";
    out << " over ";
    if (!this->latticeShifts[i].isEqualToZero()) {
      out << this->latticeShifts[i].toString() << " + ";
    }
    out << "&Lambda;";
    if (i != this->latticeShifts.size - 1) {
      out << "<br>";
    }
  }
  if (!this->ambientLatticeReduced.basisRationalForm.isIdentity()) {
    out << "<br>where " << "\\(" << "\\Lambda = \\langle";
    Vectors<Rational> roots;
    roots.assignMatrixRows(this->ambientLatticeReduced.basisRationalForm);
    for (int i = 0; i < roots.size; i ++) {
      out << roots[i].toString();
      if (i != roots.size - 1) {
        out << ", ";
      }
    }
    out << "\\rangle";
  } else {
    out
    << "<br>where \\(\\Lambda =\\mathbb{Z}^{"
    << this->minimalNumberOfVariables()
    << "}";
  }
  out << "\\)";
  return out.str();
}

std::string QuasiPolynomial::toLatex(FormatExpressions* format) const {
  std::stringstream out;
  if (this->latticeShifts.size == 0) {
    return "0";
  }
  for (int i = 0; i < this->latticeShifts.size; i ++) {
    out << "\\(" << this->valueOnEachLatticeShift[i].toString(format) << "\\)";
    out << " over ";
    if (!this->latticeShifts[i].isEqualToZero()) {
      out
      << "\\("
      << this->latticeShifts[i].toString()
      << " + "
      << "\\Lambda\\)";
    } else {
      out << "\\(\\Lambda\\)";
    }
    out << "\n";
  }
  if (!this->ambientLatticeReduced.basisRationalForm.isIdentity()) {
    out << "\nwhere " << "\\(" << "\\Lambda = \\langle";
    Vectors<Rational> roots;
    roots.assignMatrixRows(this->ambientLatticeReduced.basisRationalForm);
    for (int i = 0; i < roots.size; i ++) {
      out << roots[i].toString();
      if (i != roots.size - 1) {
        out << ", ";
      }
    }
    out << "\\rangle";
  } else {
    out
    << "\nwhere \\(\\Lambda =\\mathbb{Z}^{"
    << this->minimalNumberOfVariables()
    << "}";
  }
  out << "\\)";
  return out.str();
}

void QuasiPolynomial::makeFromPolynomialShiftAndLattice(
  const Polynomial<Rational>& inputPoly,
  const MonomialPolynomial& shift,
  const Lattice& lattice
) {
  STACK_TRACE("QuasiPolynomial::makeFromPolynomialShiftAndLattice");
  this->ambientLatticeReduced = lattice;
  this->latticeShifts.setSize(1);
  Vector<Rational>& firstShift = this->latticeShifts[0];
  firstShift.setSize(lattice.getDimension());
  for (int i = 0; i < lattice.getDimension(); i ++) {
    firstShift[i] = shift(i);
  }
  this->ambientLatticeReduced.reduceVector(this->latticeShifts[0]);
  this->valueOnEachLatticeShift.setSize(1);
  this->valueOnEachLatticeShift[0] = inputPoly;
}

void QuasiPolynomial::makeZeroOverLattice(const Lattice& lattice) {
  this->ambientLatticeReduced = lattice;
  this->latticeShifts.size = 0;
  this->valueOnEachLatticeShift.size = 0;
}

void QuasiPolynomial::makeZeroLatticeZn(int dimension) {
  if (dimension <= 0) {
    global.fatal << "Negative dimension not allowed. " << global.fatal;
  }
  this->ambientLatticeReduced.makeZn(dimension);
  this->latticeShifts.size = 0;
  this->valueOnEachLatticeShift.size = 0;
}

Rational QuasiPolynomial::evaluate(
  const Vector<Rational>& input, std::stringstream* comments
) const {
  Vector<Rational> testLatticeBelonging;
  for (int i = 0; i < this->latticeShifts.size; i ++) {
    testLatticeBelonging = this->latticeShifts[i] - input;
    if (!this->ambientLatticeReduced.isInLattice(testLatticeBelonging)) {
      continue;
    }
    if (comments != nullptr) {
      *comments
      << "Lattice shift: \\("
      << this->latticeShifts[i]
      << "\\) contains: "
      << input.toString()
      << ", ambient lattice: \\("
      << this->ambientLatticeReduced.toString()
      << "\\)<br>Quasipolynomial on shift: "
      << "\\("
      << this->valueOnEachLatticeShift[i].toString()
      << "\\)";
    }
    return this->valueOnEachLatticeShift[i].evaluate(input);
  }
  return 0;
}

bool Lattice::containsLattice(Lattice& other) const {
  Vectors<Rational> generators;
  other.basisRationalForm.getVectorsFromRows(generators);
  for (const Vector<Rational>& generator : generators) {
    if (!this->isInLattice(generator)) {
      return false;
    }
  }
  return true;
}

bool Lattice::getDualLattice(Lattice& output) const {
  if (this->getRank() != this->getDimension()) {
    return false;
  }
  Matrix<Rational> dualMatrix;
  dualMatrix = this->basisRationalForm;
  dualMatrix.invert();
  dualMatrix.transpose();
  dualMatrix.getMatrixIntegerWithDenominator(output.basis, output.denominator);
  output.reduce();
  return true;
}

bool Lattice::findOnePreimageInLatticeOf(
  const Matrix<Rational>& linearMap,
  const Vectors<Rational>& input,
  Vectors<Rational>& output
) {
  Vectors<Rational> thisBasis;
  thisBasis.assignMatrixRows(this->basisRationalForm);
  linearMap.actOnVectorsColumn(thisBasis, Rational(0));
  bool result =
  input.getIntegralCoordinatesInBasisIfTheyExist(thisBasis, output, 1, - 1, 0);
  Matrix<Rational> matrix;
  matrix = this->basisRationalForm;
  matrix.transpose();
  matrix.actOnVectorsColumn(output);
  return result;
}

void Lattice::intersectWithPreimageOfLattice(
  const Matrix<Rational>& linearMap, const Lattice& other
) {
  Vectors<Rational> startingBasis;
  Vectors<Rational> imageStartingBasis;
  Vectors<Rational> basisImageIntersection;
  Vectors<Rational> imageBasisInImageStartingBasisCoordinates;
  Vectors<Rational> resultNonKernelPart;
  Vectors<Rational> result;
  Vectors<Rational> roots;
  startingBasis.assignMatrixRows(this->basisRationalForm);
  linearMap.actOnVectorsColumn(startingBasis, imageStartingBasis);
  Lattice imageLattice;
  imageLattice.makeFromRoots(imageStartingBasis);
  imageLattice.intersectWith(other);
  basisImageIntersection.assignMatrixRows(imageLattice.basisRationalForm);
  Vectors<Rational> tempBasisImageIntersection;
  Vectors<Rational> tempImageStartingBasis;
  Vectors<Rational> tempimageBasisInImageStartingBasisCoordinates;
  basisImageIntersection = tempBasisImageIntersection;
  imageStartingBasis = tempImageStartingBasis;
  bool tempBool =
  tempBasisImageIntersection.getIntegralCoordinatesInBasisIfTheyExist(
    tempImageStartingBasis,
    tempimageBasisInImageStartingBasisCoordinates,
    Rational(1),
    Rational(- 1),
    Rational(0)
  );
  imageBasisInImageStartingBasisCoordinates = (
    tempimageBasisInImageStartingBasisCoordinates
  );
  if (!tempBool) {
    global.fatal
    << "Coordinates not integral when they should be. "
    << global.fatal;
  }
  resultNonKernelPart.setSize(imageBasisInImageStartingBasisCoordinates.size);
  for (int i = 0; i < resultNonKernelPart.size; i ++) {
    Vector<Rational>& currentRoot = resultNonKernelPart[i];
    currentRoot.makeZero(this->getDimension());
    for (int j = 0; j < startingBasis.size; j ++) {
      currentRoot +=
      startingBasis[j] * imageBasisInImageStartingBasisCoordinates[i][j];
    }
  }
  Lattice kernelPart;
  kernelPart = *this;
  roots.assignMatrixRows(linearMap);
  kernelPart.intersectWithLinearSubspaceGivenByNormals(roots);
  result.assignMatrixRows(kernelPart.basisRationalForm);
  result.addListOnTop(resultNonKernelPart);
  this->makeFromRoots(result);
}

int Lattice::getDimension() const {
  return this->basis.numberOfColumns;
}

int Lattice::getRank() const {
  return this->basis.numberOfRows;
}

int Lattice::getMinimalIntegerScalarSendingVectorIntoLattice(
  const Vector<Rational>& input
) const {
  int scale = 1;
  while (true) {
    Vector<Rational> rescaled = input;
    rescaled *= scale;
    if (this->isInLattice(rescaled)) {
      return scale;
    }
    scale ++;
  }
  global.fatal
  << "The scale does not fit in integer. "
  << "Please improve the implementation to take care of this. "
  << global.fatal;
  return - 1;
}

void Lattice::intersectWith(const Lattice& other) {
  STACK_TRACE("Lattice::intersectWith");
  Vectors<Rational> commonBasis;
  Vectors<Rational> otherBasis;
  Vectors<Rational> startBasis;
  startBasis.assignMatrixRows(this->basisRationalForm);
  otherBasis.assignMatrixRows(other.basisRationalForm);
  startBasis.intersectTwoLinearSpaces(startBasis, otherBasis, commonBasis);
  Lattice thisLatticeIntersected;
  Lattice otherLatticeIntersected;
  thisLatticeIntersected = *this;
  otherLatticeIntersected = other;
  thisLatticeIntersected.intersectWithLinearSubspaceSpannedBy(commonBasis);
  otherLatticeIntersected.intersectWithLinearSubspaceSpannedBy(commonBasis);
  Vectors<Rational> thisCommonBasis;
  Vectors<Rational> otherCommonBasis;
  Vectors<Rational> thisCommonCoordinates;
  Vectors<Rational> otherCommonCoordinates;
  thisCommonBasis.assignMatrixRows(thisLatticeIntersected.basisRationalForm);
  otherCommonBasis.assignMatrixRows(
    otherLatticeIntersected.basisRationalForm
  );
  thisCommonBasis.getCoordinatesInBasis(commonBasis, thisCommonCoordinates);
  otherCommonBasis.getCoordinatesInBasis(commonBasis, otherCommonCoordinates);
  Lattice thisCommonCoordinatesLattice;
  Lattice otherCommonCoordinatesLattice;
  thisCommonCoordinatesLattice.makeFromRoots(thisCommonCoordinates);
  otherCommonCoordinatesLattice.makeFromRoots(otherCommonCoordinates);
  thisCommonCoordinatesLattice.intersectWithBothOfMaximalRank(
    otherCommonCoordinatesLattice
  );
  Vectors<Rational> resultBasis;
  resultBasis.setSize(
    thisCommonCoordinatesLattice.basisRationalForm.numberOfRows
  );
  for (int i = 0; i < resultBasis.size; i ++) {
    Vector<Rational>& currentRoot = resultBasis[i];
    currentRoot.makeZero(this->getDimension());
    for (
      int j = 0; j < thisCommonCoordinatesLattice.basisRationalForm.
      numberOfColumns; j ++
    ) {
      currentRoot +=
      commonBasis[j] *
      thisCommonCoordinatesLattice.basisRationalForm.elements[i][j];
    }
  }
  this->makeFromRoots(resultBasis);
}

void Lattice::intersectWithBothOfMaximalRank(const Lattice& other) {
  Lattice dualLatticeThis;
  Lattice dualLatticeOther;
  if (
    this->basis.numberOfRows != this->getDimension() ||
    this->getDimension() != other.getDimension() ||
    other.basis.numberOfRows != this->getDimension()
  ) {
    global.fatal << "Bad dimensions. " << global.fatal;
  }
  this->getDualLattice(dualLatticeThis);
  other.getDualLattice(dualLatticeOther);
  dualLatticeThis.refineByOtherLattice(dualLatticeOther);
  dualLatticeThis.getDualLattice(*this);
}

// Returning false means that the lattice given as rougher is not actually
// rougher than the current lattice
// or that there are too many representatives.
bool Lattice::getAllRepresentatives(
  const Lattice& rougherLattice, Vectors<Rational>& output
) const {
  output.size = 0;
  if (this->basis.numberOfRows != rougherLattice.basis.numberOfRows) {
    return false;
  }
  List<int> periods;
  Vectors<Rational> periodVectors;
  periods.setSize(this->basis.numberOfRows);
  periodVectors.setSize(this->basis.numberOfRows);
  Vector<Rational> root;
  Vector<Rational> root2;
  int column = 0;
  int dimension = this->getDimension();
  Rational currentPeriod;
  LargeInteger currentPeriodInteger;
  for (int i = 0; i < this->basis.numberOfRows; i ++) {
    while (this->basisRationalForm.elements[i][column].isEqualToZero()) {
      column ++;
    }
    currentPeriod = rougherLattice.basisRationalForm.elements[i][column] /
    this->basisRationalForm.elements[i][column];
    if (!currentPeriod.isIntegerFittingInInt(&periods[i])) {
      return false;
    }
    this->basisRationalForm.getVectorFromRow(i, periodVectors[i]);
    rougherLattice.basisRationalForm.getVectorFromRow(i, root2);
    root = periodVectors[i];
    root *= periods[i];
  }
  for (int i = 0; i < periods.size; i ++) {
    periods[i] --;
  }
  SelectionWithDifferentMaxMultiplicities coefficientSelection;
  coefficientSelection.initializeFromIntegers(periods);
  LargeInteger resultSize = coefficientSelection.totalNumberOfSubsets();
  int numberOfCycles = 0;
  if (!resultSize.isIntegerFittingInInt(&numberOfCycles)) {
    return false;
  }
  output.setSize(numberOfCycles);
  for (
    int i = 0; i < numberOfCycles;
    i ++,
    coefficientSelection.incrementReturnFalseIfPastLast()
  ) {
    output[i].makeZero(dimension);
    for (int j = 0; j < coefficientSelection.multiplicities.size; j ++) {
      output[i] += periodVectors[j] * coefficientSelection.multiplicities[j];
    }
  }
  return true;
}

bool Lattice::
getClosestPointInDirectionOfNormalToAffineWallMovingIntegralStepsInDirection(
  Vector<Rational>& startingPoint,
  Vector<Rational>& affineHyperplane,
  Vector<Rational>& direction,
  Vector<Rational>& outputPoint
) {
  Vector<Rational> normal = affineHyperplane;
  normal.setSize(affineHyperplane.size - 1);
  Rational shift = -(*affineHyperplane.lastObject());
  if (normal.scalarEuclidean(startingPoint) == shift) {
    outputPoint = startingPoint;
    return true;
  }
  if (direction.scalarEuclidean(normal).isEqualToZero()) {
    return false;
  }
  Rational movement = (shift - startingPoint.scalarEuclidean(normal)) /
  direction.scalarEuclidean(normal);
  global.comments
  << "<br>the movement: "
  << movement.toString()
  << ", ("
  << shift.toString()
  << " - "
  << startingPoint.scalarEuclidean(normal).toString()
  << ")/ "
  << direction.scalarEuclidean(normal).toString()
  << ", ";
  if (!movement.isInteger()) {
    global.comments << "the movement is not integral; ";
    movement.assignFloor();
    if (direction.scalarEuclidean(normal).isPositive()) {
      movement += 1;
    }
  }
  global.comments
  << "the normal: "
  << normal.toString()
  << ", the direction: "
  << direction.toString()
  << ", the shift: "
  << shift.toString()
  << ", the movement: "
  << movement.toString()
  << ", startingPoint: "
  << startingPoint.toString();
  outputPoint = startingPoint;
  outputPoint += direction * movement;
  global.comments << ", finalPoint: " << outputPoint.toString();
  return true;
}

bool Lattice::getAllRepresentativesProjectingDownTo(
  const Lattice& rougherLattice,
  Vectors<Rational>& startingShifts,
  Vectors<Rational>& output
) const {
  Vectors<Rational> tempRepresentatives;
  if (!this->getAllRepresentatives(rougherLattice, tempRepresentatives)) {
    return false;
  }
  output.setExpectedSize(startingShifts.size * tempRepresentatives.size);
  output.size = 0;
  for (int i = 0; i < startingShifts.size; i ++) {
    for (int j = 0; j < tempRepresentatives.size; j ++) {
      output.addOnTop(startingShifts[i] + tempRepresentatives[j]);
    }
  }
  return true;
}

bool Lattice::reduceVector(Vector<Rational>& vector) const {
  STACK_TRACE("Lattice::reduceVector");
  Vector<Rational> output;
  if (vector.size != this->getDimension()) {
    global.fatal
    << "Expected dimension: "
    << this->getDimension()
    << ", got: "
    << vector.size
    << ", vector: "
    << vector.toString()
    << ". "
    << global.fatal;
  }
  Vectors<Rational> basisRoots;
  basisRoots.assignMatrixRows(this->basisRationalForm);
  if (!vector.getCoordinatesInBasis(basisRoots, output)) {
    global.fatal
    << "Failed to get coordinates of: "
    << vector.toString()
    << " in basis: "
    << basisRoots.toString()
    << global.fatal;
    return false;
  }
  for (int i = 0; i < output.size; i ++) {
    output[i].assignFractionalValue();
  }
  vector.makeZero(vector.size);
  for (int i = 0; i < basisRoots.size; i ++) {
    vector += basisRoots[i] * output[i];
  }
  return true;
}

void Lattice::makeZn(int dimension) {
  this->basisRationalForm.makeIdentityMatrix(
    dimension, Rational::one(), Rational::zero()
  );
  this->basis.makeIdentityMatrix(dimension, LargeInteger(1), LargeInteger(0));
  this->denominator.makeOne();
}

bool Lattice::substitutionHomogeneous(
  const PolynomialSubstitution<Rational>& substitution
) {
  Matrix<Rational> matrixSubstitution;
  if (
    !this->getHomogeneousSubstitutionMatrixFromSubstitutionIgnoreConstantTerms(
      substitution, matrixSubstitution
    )
  ) {
    return false;
  }
  return this->substitutionHomogeneous(matrixSubstitution);
}

bool Lattice::
getHomogeneousSubstitutionMatrixFromSubstitutionIgnoreConstantTerms(
  const PolynomialSubstitution<Rational>& substitution,
  Matrix<Rational>& output
) {
  if (substitution.size < 1) {
    return false;
  }
  int targetDimension = 0;
  for (int i = 0; i < substitution.size; i ++) {
    targetDimension =
    MathRoutines::maximum(
      targetDimension, substitution[i].minimalNumberOfVariables()
    );
  }
  output.initialize(substitution.size, targetDimension);
  for (int i = 0; i < substitution.size; i ++) {
    Polynomial<Rational>& currentPoly = substitution[i];
    if (!currentPoly.isLinear()) {
      return false;
    }
    for (int j = 0; j < targetDimension; j ++) {
      currentPoly.coefficientInFrontOfLinearTermVariableIndex(
        j, output.elements[i][j]
      );
    }
  }
  return true;
}

void Lattice::intersectWithLinearSubspaceGivenByNormal(
  const Vector<Rational>& normal
) {
  Vectors<Rational> startingBasis;
  Vectors<Rational> resultBasis;
  startingBasis.assignMatrixRows(this->basisRationalForm);
  Vector<Rational> scalarProducts;
  scalarProducts.setSize(startingBasis.size);
  for (int i = 0; i < this->basisRationalForm.numberOfRows; i ++) {
    scalarProducts[i] = normal.scalarEuclidean(startingBasis[i]);
  }
  if (scalarProducts.isEqualToZero()) {
    return;
  }
  int pivotColumnIndex = scalarProducts.getIndexLastNonZeroCoordinate();
  Rational pivotCoeff = scalarProducts[pivotColumnIndex];
  scalarProducts /= - pivotCoeff;
  Vectors<Rational> eigenSpacePlusOrthogonalComponent;
  eigenSpacePlusOrthogonalComponent.setSize(scalarProducts.size);
  for (int i = 0; i < scalarProducts.size; i ++) {
    if (i != pivotColumnIndex) {
      Vector<Rational>& currentRoot = eigenSpacePlusOrthogonalComponent[i];
      currentRoot.makeZero(scalarProducts.size);
      currentRoot[i] = 1;
      currentRoot[pivotColumnIndex] = scalarProducts[i];
    }
  }
  scalarProducts.scaleNormalizeFirstNonZero();
  eigenSpacePlusOrthogonalComponent[pivotColumnIndex] = scalarProducts;
  Lattice eigenLattice;
  Lattice znLattice;
  eigenLattice.makeFromRoots(eigenSpacePlusOrthogonalComponent);
  znLattice.makeZn(scalarProducts.size);
  znLattice.intersectWithBothOfMaximalRank(eigenLattice);
  resultBasis.reserve(scalarProducts.size - 1);
  Vector<Rational> root;
  Vector<Rational> resultRoot;
  Rational orthogonalComponent;
  for (int i = 0; i < znLattice.basisRationalForm.numberOfRows; i ++) {
    znLattice.basisRationalForm.getVectorFromRow(i, root);
    orthogonalComponent = root.scalarEuclidean(scalarProducts) /
    scalarProducts.scalarEuclidean(scalarProducts);
    root -= scalarProducts * orthogonalComponent;
    if (!orthogonalComponent.isInteger()) {
      global.fatal
      << "Orthogonal component is supposed to be an integer. "
      << global.fatal;
    }
    if (!root.isEqualToZero()) {
      resultRoot.makeZero(this->getDimension());
      for (int j = 0; j < startingBasis.size; j ++) {
        resultRoot += startingBasis[j] * root[j];
      }
      resultBasis.addOnTop(resultRoot);
    }
  }
  this->makeFromRoots(resultBasis);
}

void Lattice::intersectWithLinearSubspaceSpannedBy(
  const Vectors<Rational>& subspaceBasis
) {
  Vectors<Rational> normals;
  Matrix<Rational> matrix;
  subspaceBasis.getMatrixRootsToRows(matrix);
  matrix.getZeroEigenSpace(normals);
  this->intersectWithLinearSubspaceGivenByNormals(normals);
}

void Lattice::intersectWithLinearSubspaceGivenByNormals(
  const Vectors<Rational>& subspaceNormals
) {
  for (int i = 0; i < subspaceNormals.size; i ++) {
    this->intersectWithLinearSubspaceGivenByNormal(subspaceNormals[i]);
  }
}

bool Lattice::operator!=(const Lattice& other) const {
  return !(*this == other);
}

bool Lattice::operator==(const Lattice& other) const {
  return this->basisRationalForm == other.basisRationalForm;
}

unsigned int Lattice::hashFunction(const Lattice& input) {
  return input.basisRationalForm.hashFunction();
}

void Lattice::scaleNormalizeByPositive(Vector<Rational>& toScale) {
  int firstIndex = toScale.getIndexFirstNonZeroCoordinate();
  if (firstIndex < 0) {
    return;
  }
  Rational scale = Rational::scaleNormalizeIndex(toScale, firstIndex);
  if (scale < 0) {
    toScale *= - 1;
  }
}

bool Lattice::substitutionHomogeneous(const Matrix<Rational>& substitution) {
  int targetDimension = substitution.numberOfColumns;
  if (substitution.numberOfRows != this->getDimension()) {
    return false;
  }
  int startingDimension = this->getDimension();
  Matrix<Rational> matrix;
  Matrix<Rational> oldBasisTransformed;
  Matrix<Rational> matRelationBetweenStartingVariables;
  matrix = substitution;
  oldBasisTransformed = this->basisRationalForm;
  oldBasisTransformed.transpose();
  Selection nonPivotPoints;
  matrix.gaussianEliminationByRows(&oldBasisTransformed, &nonPivotPoints);
  if (nonPivotPoints.cardinalitySelection != 0) {
    return false;
  }
  int numberOfNonZeroRows = nonPivotPoints.numberOfElements;
  int numberOfZeroRows = matrix.numberOfRows - numberOfNonZeroRows;
  matRelationBetweenStartingVariables.initialize(
    numberOfZeroRows, startingDimension
  );
  for (int i = 0; i < numberOfZeroRows; i ++) {
    for (int j = 0; j < startingDimension; j ++) {
      matRelationBetweenStartingVariables.elements[i][j] =
      oldBasisTransformed.elements[i + numberOfNonZeroRows][j];
    }
  }
  Vectors<Rational> eigenSpace;
  matRelationBetweenStartingVariables.getZeroEigenSpaceModifyMe(eigenSpace);
  for (int i = 0; i < eigenSpace.size; i ++) {
    Lattice::scaleNormalizeByPositive(eigenSpace[i]);
  }
  oldBasisTransformed.actOnVectorsColumn(eigenSpace);
  this->basisRationalForm.initialize(targetDimension, targetDimension);
  for (int i = 0; i < targetDimension; i ++) {
    for (int j = 0; j < targetDimension; j ++) {
      this->basisRationalForm.elements[i][j] = eigenSpace[i][j];
    }
  }
  this->basisRationalForm.getMatrixIntegerWithDenominator(
    this->basis, this->denominator
  );
  this->reduce();
  return true;
}

void Lattice::getRougherLatticeFromAffineHyperplaneDirectionAndLattice(
  const Vector<Rational>& direction,
  Vector<Rational>& outputDirectionMultipleOnLattice,
  Vector<Rational>& shift,
  Vector<Rational>& affineHyperplane,
  Vectors<Rational>& outputRepresentatives,
  Vectors<Rational>& movementInDirectionPerRepresentative,
  Lattice& outputRougherLattice
) {
  Vector<Rational> normal = affineHyperplane;
  normal.setSize(normal.size - 1);
  if (direction.scalarEuclidean(normal).isEqualToZero()) {
    return;
  }
  Rational constOnRightHandSide = - *affineHyperplane.lastObject();
  Vectors<Rational> basis;
  basis.assignMatrixRows(this->basisRationalForm);
  Lattice hyperplaneLatticeNoShift;
  Lattice directionLattice;
  // , normalProjectionLattice, trueProjectionLattice;
  Vectors<Rational> roots;
  // Vector<Rational> root;
  roots.addOnTop(direction);
  directionLattice = *this;
  directionLattice.intersectWithLinearSubspaceSpannedBy(roots);
  directionLattice.basisRationalForm.getVectorFromRow(
    0, outputDirectionMultipleOnLattice
  );
  hyperplaneLatticeNoShift = *this;
  hyperplaneLatticeNoShift.intersectWithLinearSubspaceGivenByNormal(normal);
  roots.assignMatrixRows(hyperplaneLatticeNoShift.basisRationalForm);
  roots.addOnTop(outputDirectionMultipleOnLattice);
  outputRougherLattice.makeFromRoots(roots);
  this->getAllRepresentatives(outputRougherLattice, outputRepresentatives);
  for (int i = 0; i < outputRepresentatives.size; i ++) {
    outputRepresentatives[i] += shift;
    outputRougherLattice.reduceVector(outputRepresentatives[i]);
  }
  Rational shiftedConstant;
  Rational unitMovement;
  Rational scalarProduct;
  unitMovement = normal.scalarEuclidean(outputDirectionMultipleOnLattice);
  movementInDirectionPerRepresentative.setSize(outputRepresentatives.size);
  for (int i = 0; i < outputRepresentatives.size; i ++) {
    scalarProduct = (
      normal.scalarEuclidean(outputRepresentatives[i]) - constOnRightHandSide
    ) /
    unitMovement;
    scalarProduct.assignFractionalValue();
    shiftedConstant = constOnRightHandSide + scalarProduct;
    Vector<Rational>& currentMovement =
    movementInDirectionPerRepresentative[i];
    currentMovement = affineHyperplane;
    *currentMovement.lastObject() = shiftedConstant;
  }
}

void Lattice::subLatticeScalarProductTimesDirectionInLattice(
  const Vector<Rational>& scalarProductWith,
  const Vector<Rational>& direction,
  const Lattice& target,
  Lattice& output
) const {
  Vectors<Rational> basis;
  basis.assignMatrixRows(target.basisRationalForm);
  Vector<Rational> coordinates;
  direction.getCoordinatesInBasis(basis, coordinates);
  Rational scale;
  scale = Rational::scaleNoSignChange(coordinates);
  this->subLatticeWithIntegralScalarProducts(scalarProductWith, output);
  output.basisRationalForm *= scale;
  output.makeFromMatrix(output.basisRationalForm);
}

void Lattice::subLatticeWithIntegralScalarProducts(
  const Vector<Rational>& mustHaveIntegerScalarProductWith, Lattice& output
) const {
  this->getDualLattice(output);
  Matrix<Rational> lastRow;
  lastRow.assignVectorRow(mustHaveIntegerScalarProductWith);
  output.basisRationalForm.appendMatrixToTheBottom(lastRow);
  output.makeFromMatrix(output.basisRationalForm);
  output.getDualLattice(output);
}

void Lattice::reduce() {
  STACK_TRACE("Lattice::reduce");
  this->basis.gaussianEliminationEuclideanDomain();
  int rowsToTrim = 0;
  for (int i = this->basis.numberOfRows - 1; i >= 0; i --) {
    bool foundNonZeroRow = false;
    for (int j = 0; j < this->basis.numberOfColumns; j ++) {
      if (!this->basis.elements[i][j].isEqualToZero()) {
        foundNonZeroRow = true;
        break;
      }
    }
    if (foundNonZeroRow) {
      break;
    }
    rowsToTrim ++;
  }
  this->basis.resize(
    this->basis.numberOfRows - rowsToTrim, this->basis.numberOfColumns, true
  );
  this->basisRationalForm.assignMatrixIntegerWithDenominator(
    this->basis, this->denominator
  );
}

void Lattice::testGaussianEliminationEuclideanDomainRationals(
  Matrix<Rational>& output
) {
  output.assignMatrixIntegerWithDenominator(this->basis, this->denominator);
  std::stringstream out;
  global.comments << "Test output: " << output.toString();
  out << "Test output: " << output.toString();
  output.gaussianEliminationEuclideanDomain();
  global.comments << "<br>After gaussian elimination:" << output.toString();
  out << "<br>After gaussian elimination:" << output.toString();
}

void Lattice::refineByVector(const Vector<Rational>& other) {
  Vectors<Rational> generators;
  this->basisRationalForm.getVectorsFromRows(generators);
  generators.addOnTop(other);
  this->makeFromRoots(generators);
}

void Lattice::refineByOtherLattice(const Lattice& other) {
  if (other.basis.numberOfColumns == 0) {
    return;
  }
  if (other.basis == this->basis && this->denominator == other.denominator) {
    return;
  }
  if (other.getDimension() != this->getDimension()) {
    global.fatal << "Dimension mismatch. " << global.fatal;
  }
  int dimension = this->getDimension();
  LargeIntegerUnsigned oldDenominator = this->denominator;
  LargeIntegerUnsigned::leastCommonMultiple(
    other.denominator, oldDenominator, this->denominator
  );
  LargeIntegerUnsigned scaleThis;
  LargeIntegerUnsigned scaleOther;
  scaleThis = this->denominator / oldDenominator;
  scaleOther = this->denominator / other.denominator;
  int oldNumberOfRows = this->basis.numberOfRows;
  LargeInteger scale;
  scale = scaleThis;
  this->basis *= scale;
  this->basis.resize(
    this->basis.numberOfRows + other.basis.numberOfRows, dimension, true
  );
  for (int i = oldNumberOfRows; i < this->basis.numberOfRows; i ++) {
    for (int j = 0; j < this->basis.numberOfColumns; j ++) {
      this->basis.elements[i][j] =
      other.basis.elements[i - oldNumberOfRows][j] * scaleOther;
    }
  }
  this->reduce();
}

void Lattice::makeFromMatrix(const Matrix<Rational>& input) {
  this->basisRationalForm = input;
  this->basisRationalForm.getMatrixIntegerWithDenominator(
    this->basis, this->denominator
  );
  this->reduce();
}

void Lattice::makeFromRoots(const List<Vector<Rational> >& input) {
  Matrix<Rational> rescaled;
  rescaled.assignVectorsToRows(input);
  rescaled.getMatrixIntegerWithDenominator(this->basis, this->denominator);
  this->reduce();
}

std::string Lattice::toString() const {
  std::stringstream out;
  out << "\\langle" << this->toStringParentheses() << "\\rangle";
  return out.str();
}

std::string Lattice::toStringParentheses() const {
  std::stringstream out;
  out << "(";
  Vectors<Rational> roots;
  roots.assignMatrixRows(this->basisRationalForm);
  for (int i = 0; i < this->basisRationalForm.numberOfRows; i ++) {
    out << roots[i].toString();
    if (i != this->basisRationalForm.numberOfRows - 1) {
      out << ",";
    }
  }
  out << ")";
  return out.str();
}

void Lattice::drawOnePointWithSigns(
  const Vector<Rational>& toDraw,
  const List<Vector<Rational> >& basis,
  DrawingVariables& output,
  int& outputTotalPointsDrawn,
  List<Vector<Rational> >* restrictingCone
) const {
  STACK_TRACE("Lattice::drawOnePointWithSigns");
  Selection signs;
  signs.initialize(toDraw.size);
  this->drawOnePoint(
    toDraw, basis, signs, output, outputTotalPointsDrawn, restrictingCone
  );
  while (signs.incrementReturnFalseIfPastLast()) {
    this->drawOnePoint(
      toDraw, basis, signs, output, outputTotalPointsDrawn, restrictingCone
    );
  }
}

void Lattice::drawOnePointBase(
  const Vector<Rational>& toDraw,
  DrawingVariables& output,
  int& outputTotalPointsDrawn
) {
  outputTotalPointsDrawn ++;
  output.drawCircleAtVectorBufferRational(toDraw, "blue", 3);
}

void Lattice::drawOnePoint(
  const Vector<Rational>& inBasisCoordinates,
  const List<Vector<Rational> >& basis,
  const Selection& signs,
  DrawingVariables& output,
  int& outputTotalPointsDrawn,
  List<Vector<Rational> >* restrictingCone
) const {
  STACK_TRACE("Lattice::drawOnePoint");
  if (outputTotalPointsDrawn >= this->maximumPointsToDraw) {
    return;
  }
  Vector<Rational> toDraw;
  toDraw.makeZero(this->getDimension());
  for (int i = 0; i < inBasisCoordinates.size; i ++) {
    Rational coefficient = inBasisCoordinates[i];
    if (signs.selected[i]) {
      coefficient *= - 1;
    }
    toDraw += basis[i] * coefficient;
  }
  for (const Vector<Rational>& wallNormal : *restrictingCone) {
    if (wallNormal.scalarEuclidean(toDraw).isNegative()) {
      return;
    }
  }
  this->drawOnePointBase(toDraw, output, outputTotalPointsDrawn);
}

void Lattice::draw(
  DrawingVariables& output, List<Vector<Rational> >* restrictingCone
) const {
  STACK_TRACE("Lattice::draw");
  if (this->basisRationalForm.numberOfRows == 0) {
    return;
  }
  Vector<Rational> currentDot;
  int dimension = this->getDimension();
  output.initializeDimensions(dimension);
  output.makeMeAStandardBasis(dimension);
  output.drawCoordinateSystemBuffer(dimension);
  int totalPointsDrawn = 0;
  this->drawOnePointBase(Vector<Rational>({0, 0}), output, totalPointsDrawn);
  Vectors<Rational> generators;
  this->basisRationalForm.getVectorsFromRows(generators);
  SelectionWithMaximumMultiplicity selection;
  for (int i = 1; i < 9; i ++) {
    selection.initializeMaximumMultiplicity(generators.size, i);
    while (selection.incrementSubsetFixedCardinality(i)) {
      currentDot = selection;
      this->drawOnePointWithSigns(
        currentDot, generators, output, totalPointsDrawn, restrictingCone
      );
      if (totalPointsDrawn > this->maximumPointsToDraw) {
        return;
      }
    }
  }
}
