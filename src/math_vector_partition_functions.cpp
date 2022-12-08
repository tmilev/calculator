#include "math_vector_partition_functions.h"
#include "progress_report.h"
#include "math_general_polynomial_computations_basic_implementation.h"

void VectorPartitionFunction::initializeVectors(
  List<Vector<Rational> >& inputVectors
) {
  this->elementaryMethod.originalVectors = inputVectors;
  this->fractions.originalVectors = inputVectors;
}

std::string VectorPartitionFunction::toHTML() const {
  std::stringstream out;
  if (this->fractions.flagInitialized) {
    out
    << "<div style='max-width: 500px; max-height:500px; overflow:scroll'>"
    << fractions.toHTML();
    out << "<br>Chambers:<br>" << fractions.chambers.toHTML() << "</div>";
  }
  if (this->elementaryMethod.flagInitialized) {
    out << this->elementaryMethod.toHTML();
  }
  return out.str();
}

VectorPartitionFunctionElementary::VectorPartitionFunctionElementary() {
  this->flagInitialized = false;
}

std::string VectorPartitionFunctionElementary::toHTML() const {
  if (!this->flagInitialized) {
    return "";
  }
  std::stringstream out;
  out << "Vector partition function of " << this->originalVectors;
  out << this->collection.toHTMLGraphicsOnly(false);
  for (Cone& cone : this->collection.refinedCones.values) {
    out << "<hr>";
    out << "Cone: " << cone.id << ".<br>";
    out << "Quasipolynomial: " << cone.payload.polynomial.toHTML();
  }
  return out.str();
}

void VectorPartitionFunctionElementary::compute() {
  STACK_TRACE("VectorPartitionFunctionElementary::compute");
  ProgressReport report;
  this->collection.initializeFromDirections(this->originalVectors);
  for (int i = 0; i < this->originalVectors.size; i ++) {
    this->collection.markAllConesNonRefined(i);
    this->collection.refineByOneDirection(i, &report);
    this->computeQuasiPolynomials(i);
  }
}

void VectorPartitionFunctionElementary::computeQuasiPolynomials(
  int directionIndex
) {
  STACK_TRACE("VectorPartitionFunctionElementary::computeQuasiPolynomials");
  this->collection.markAllConesNonRefined(directionIndex);
  while (this->collection.nonRefinedCones.size() > 0) {
    this->computeFirstQuasiPolynomial(directionIndex);
  }
}

void VectorPartitionFunctionElementary::computeFirstQuasiPolynomial(
  int directionIndex
) {
  for (int i = 0; i < this->collection.nonRefinedCones.size(); i ++) {
    Cone& cone = this->collection.nonRefinedCones.values[i];
    if (this->computeOneQuasiPolynomial(cone, directionIndex)) {
      this->collection.refinedCones.setKeyValue(cone.id, cone);
      this->collection.nonRefinedCones.removeIndex(i);
      return;
    }
  }
  if (this->collection.nonRefinedCones.size() != 0) {
    global.fatal
    << "Cone collection is expected to have "
    << "a cone with all exit walls visited."
    << global.fatal;
  }
}

bool VectorPartitionFunctionElementary::computeOneQuasiPolynomial(
  Cone& cone, int directionIndex
) {
  Vector<Rational> direction = this->originalVectors[directionIndex];
  List<Wall> exitWalls;
  if (
    !this->collection.allExitWallsAreVisited(cone, direction, exitWalls)
  ) {
    return false;
  }
  if (exitWalls.size > 1) {
    global.fatal
    << "At this point of code, a maximum of 1 exit walls is expected."
    << global.fatal;
  }
  cone.payload.visited = true;
  if (directionIndex == this->collection.getDimension() - 1) {
    Cone baseCone;
    List<Vector<Rational> > firstDirections;
    this->originalVectors.slice(
      0, this->collection.getDimension(), firstDirections
    );
    baseCone.createFromVertices(firstDirections);
    if (!baseCone.isInCone(cone.internalPoint())) {
      return true;
    }
    Lattice lattice;
    lattice.makeFromRoots(firstDirections);
    cone.payload.polynomial.makeZeroOverLattice(lattice);
    Polynomial<Rational> one;
    one.makeOne();
    Vector<Rational> zeroVector;
    zeroVector.makeZero(this->collection.getDimension());
    cone.payload.polynomial.addLatticeShift(one, zeroVector);
    return true;
  }
  QuasiPolynomial discreteIntegrand = cone.payload.polynomial;
  global.comments
  << "DEBUG: mathematically incorrect workign version. Please fix. <br>";
  this->computeOneQuasiPolynomialExitWallWithoutNeighbor(
    cone, direction, exitWalls[0].normal
  );
  return true;
}

void VectorPartitionFunctionElementary::
computeOneQuasiPolynomialExitWallWithoutNeighbor(
  Cone& cone,
  const Vector<Rational>& direction,
  const Vector<Rational>& exitWall
) {
  STACK_TRACE(
    "VectorPartitionFunctionElementary::"
    "computeOneQuasiPolynomialExitWallWithoutNeighbor"
  );
  QuasiPolynomial toBeIntegrated = cone.payload.polynomial;
  int scale = 1;
  while (true) {
    Vector<Rational> rescaled = direction;
    rescaled *= scale;
    if (toBeIntegrated.ambientLatticeReduced.isInLattice(rescaled)) {
      break;
    }
    scale ++;
  }
  QuasiPolynomial output;
  for (int i = 0; i < scale; i ++) {
    this->computeOneQuasiPolynomialExitWallWithoutNeighborOneScale(
      toBeIntegrated, i, scale, output, direction, exitWall
    );
  }
}

void VectorPartitionFunctionElementary::
computeOneQuasiPolynomialExitWallWithoutNeighborOneScale(
  const QuasiPolynomial& toBeIntegrated,
  int shift,
  int scale,
  QuasiPolynomial& outputAccumulator,
  const Vector<Rational>& direction,
  const Vector<Rational>& exitWall
) {
  STACK_TRACE(
    "VectorPartitionFunctionElementary::"
    "computeOneQuasiPolynomialExitWallWithoutNeighborOneScale"
  );
  int dimension = this->collection.getDimension();
  PolynomialSubstitution<Rational> substitution;
  substitution.setSize(dimension);
  for (int i = 0; i < dimension; i ++) {
    substitution[i].makeMonomial(i, 1, 1);
    substitution[i].subtractMonomial(
      MonomialPolynomial(dimension + 1), direction[i] * scale
    );
    substitution[i] -= direction[i] * shift;
  }
  Lattice rougherLattice;
  Vector<Rational> exitWallRescaled = exitWall;
  exitWallRescaled /= direction.scalarEuclidean(exitWall);
  toBeIntegrated.ambientLatticeReduced.subLatticeWithIntegralScalarProducts(
    exitWallRescaled, rougherLattice
  );
  for (int i = 0; i < toBeIntegrated.latticeShifts.size; i ++) {
    this->computeOneQuasiPolynomialExitWallWithoutNeighborOneScaleOneShift(
      toBeIntegrated,
      shift,
      scale,
      substitution,
      i,
      outputAccumulator,
      direction,
      exitWall,
      rougherLattice
    );
  }
}

void VectorPartitionFunctionElementary::
computeOneQuasiPolynomialExitWallWithoutNeighborOneScaleOneShift(
  const QuasiPolynomial& toBeIntegrated,
  int shift,
  int scale,
  PolynomialSubstitution<Rational>& substitution,
  int latticeShiftIndex,
  QuasiPolynomial& outputAccumulator,
  const Vector<Rational>& direction,
  const Vector<Rational>& exitWall,
  Lattice& rougherLattice
) {
  STACK_TRACE(
    "VectorPartitionFunctionElementary::"
    "computeOneQuasiPolynomialExitWallWithoutNeighborOneScaleOneShift"
  );
  Polynomial<Rational> value =
  toBeIntegrated.valueOnEachLatticeShift[latticeShiftIndex];
  int startingDegree = value.totalDegreeInt();
  value.substitute(substitution, 1);
  int dimension = this->collection.getDimension();
  Polynomial<Rational> coefficientInFrontOfPower;
  Polynomial<Rational> bernoulliSum;
  for (int i = 0; i <= startingDegree; i ++) {
    value.getCoefficientPolynomialOfXPowerK(
      dimension, i, coefficientInFrontOfPower
    );
    this->bernoulliSumComputer.getBernoulliSum(i, bernoulliSum);
  }
  global << "DEBUG: continue work here. ";
}

void BernoulliSumComputer::getBernoulliSum(
  int power, Polynomial<Rational>& output
) {
  output.makeZero();
  ProgressReport report;
  std::stringstream out;
  out << "Computing Bernoulli sums of powers: " << power << ".";
  report.report(out.str());
  for (int i = 0; i <= power; i ++) {
    Rational coefficient;
    this->getNthBernoulliPlusNumber(i, coefficient);
    coefficient *= Rational::nChooseK(power + 1, i);
    output.addMonomial(MonomialPolynomial(0, power + 1 - i), coefficient);
  }
  output /= power + 1;
}

void BernoulliSumComputer::getNthBernoulliPlusNumber(
  int index, Rational& output
) {
  if (index < this->bernoulliPlusNumbers.size) {
    // The number already computed.
    output = this->bernoulliPlusNumbers[index];
    return;
  }
  // Compute the numbers from small to large to ensure we don't get a deep
  // recursion.
  ProgressReport report;
  std::stringstream out;
  out << "Computing Bernoulli-plus number index: " << index << ".";
  report.report(out.str());
  output = 1;
  for (int i = 0; i < index; i ++) {
    Rational summand;
    this->getNthBernoulliPlusNumber(i, summand);
    summand *= Rational::nChooseK(index, i);
    summand /= (index - i + 1);
    output -= summand;
  }
  this->bernoulliPlusNumbers.addOnTop(output);
}
