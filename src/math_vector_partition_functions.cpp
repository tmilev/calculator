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
  if (directionIndex < this->collection.getDimension() - 1) {
    return true;
  }
  if (directionIndex == this->collection.getDimension() - 1) {
    Cone baseCone;
    List<Vector<Rational> > firstDirections;
    this->originalVectors.slice(
      0, this->collection.getDimension(), firstDirections
    );
    baseCone.createFromVertices(firstDirections);
    Lattice lattice;
    lattice.makeFromRoots(firstDirections);
    cone.payload.polynomial.makeZeroOverLattice(lattice);
    if (!baseCone.isInCone(cone.internalPoint())) {
      return true;
    }
    Polynomial<Rational> one;
    one.makeOne();
    Vector<Rational> zeroVector;
    zeroVector.makeZero(this->collection.getDimension());
    cone.payload.polynomial.addLatticeShift(one, zeroVector);
    return true;
  }
  QuasiPolynomial discreteIntegrand = cone.payload.polynomial;
  List<int> exitCones;
  this->getExitCones(cone, direction, exitCones);
  QuasiPolynomial output;
  output.ambientLatticeReduced = cone.payload.polynomial.ambientLatticeReduced;
  for (int i : exitCones) {
    Cone& next = this->collection.getConeByIdNonConstNoFail(i);
    this->collection.allExitWallsAreVisited(next, direction, exitWalls);
    if (exitWalls.size != 1) {
      global.fatal << "Single expected wall expected here. " << global.fatal;
    }
    this->accumulateQuasiPolynomialExitWall(
      next, direction, exitWalls[0].normal, output
    );
  }
  global.comments
  << "DEBUG: mathematically incorrect workign version. Please fix. <br>";
  return true;
}

void VectorPartitionFunctionElementary::getExitCones(
  Cone& start, Vector<Rational>& direction, List<int>& output
) {
  output.clear();
  Cone* next = &start;
  List<Wall> exitWalls;
  while (next != nullptr) {
    output.addOnTop(next->id);
    if (
      !this->collection.allExitWallsAreVisited(*next, direction, exitWalls)
    ) {
      global.fatal
      << "Expected all exit walls to be visited. "
      << global.fatal;
    }
    if (exitWalls.size > 1) {
      global.fatal << "Expected single exit wall. " << global.fatal;
    }
    const Wall& wall = exitWalls[0];
    if (wall.neighbors.size > 1) {
      global.fatal << "Multiple neighbors along exit wall. " << global.fatal;
    }
    if (wall.neighbors.size == 0) {
      return;
    }
    next = &this->collection.getConeByIdNonConstNoFail(wall.neighbors[0]);
  }
}

void VectorPartitionFunctionElementary::accumulateQuasiPolynomialExitWall(
  Cone& cone,
  const Vector<Rational>& direction,
  const Vector<Rational>& exitWall,
  QuasiPolynomial& output
) {
  STACK_TRACE(
    "VectorPartitionFunctionElementary::"
    "accumulateQuasiPolynomialExitWallWithoutNeighbor"
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
  for (int i = 0; i < scale; i ++) {
    this->computeOneQuasiPolynomialExitWallWithoutNeighborOneScale(
      toBeIntegrated, i, scale, output, direction, exitWall
    );
  }
  cone.payload.polynomial = output;
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
  Lattice latticeFromWallDistance;
  Vector<Rational> exitWallRescaled = exitWall;
  exitWallRescaled /= direction.scalarEuclidean(exitWall);
  exitWallRescaled /= scale;
  Rational rationalShift = shift;
  rationalShift /= scale;
  toBeIntegrated.ambientLatticeReduced.subLatticeWithIntegralScalarProducts(
    exitWallRescaled, latticeFromWallDistance
  );
  for (int i = 0; i < toBeIntegrated.latticeShifts.size; i ++) {
    Vector<Rational> latticeShift = toBeIntegrated.latticeShifts[i];
    latticeShift += direction * shift;
    this->computeOneQuasiPolynomialExitWallWithoutNeighborOneScaleOneShift(
      toBeIntegrated,
      rationalShift,
      substitution,
      latticeShift,
      toBeIntegrated.valueOnEachLatticeShift[i],
      outputAccumulator,
      exitWall,
      latticeFromWallDistance
    );
  }
}

void VectorPartitionFunctionElementary::
computeOneQuasiPolynomialExitWallWithoutNeighborOneScaleOneShift(
  const QuasiPolynomial& toBeIntegrated,
  const Rational& rationalShift,
  PolynomialSubstitution<Rational>& substitution,
  const Vector<Rational>& latticeShift,
  const Polynomial<Rational>& valueOnLatticeShift,
  QuasiPolynomial& outputAccumulator,
  const Vector<Rational>& exitWall,
  Lattice& rougherLattice
) {
  STACK_TRACE(
    "VectorPartitionFunctionElementary::"
    "computeOneQuasiPolynomialExitWallWithoutNeighborOneScaleOneShift"
  );
  Polynomial<Rational> value = valueOnLatticeShift;
  int startingDegree = value.totalDegreeInt();
  value.substitute(substitution, 1);
  Vectors<Rational> representatives;
  toBeIntegrated.ambientLatticeReduced.getAllRepresentatives(
    rougherLattice, representatives
  );
  Polynomial<Rational> linearFunction;
  Polynomial<Rational> coefficientInFrontOfPower;
  Polynomial<Rational> summand;
  PolynomialSubstitution<Rational> bernoulliSubstitution;
  bernoulliSubstitution.setSize(1);
  for (Vector<Rational>& representative : representatives) {
    if (!rougherLattice.isInLattice(representative - latticeShift)) {
      continue;
    }
    Vector<Rational> vector = exitWall;
    vector.addOnTop((
        exitWall.scalarEuclidean(representative) + rationalShift
      ).fractionalValue()
    );
    bernoulliSubstitution[0].makeLinearWithConstantTerm(vector);
    int dimension = this->collection.getDimension();
    for (int i = 0; i <= startingDegree; i ++) {
      value.getCoefficientPolynomialOfXPowerK(
        dimension, i, coefficientInFrontOfPower
      );
      this->bernoulliSumComputer.getBernoulliSum(i, summand);
      summand.substitute(bernoulliSubstitution, 1);
      summand *= coefficientInFrontOfPower;
      outputAccumulator.addLatticeShift(summand, representative);
    }
  }
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
