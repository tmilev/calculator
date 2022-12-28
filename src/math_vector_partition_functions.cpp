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
    << this->fractions.toHTML();
    out
    << "<br>Chambers:<br>"
    << this->fractions.chambers.toHTML()
    << "</div>";
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
  out << "<br>Total " << this->comments.totalSteps << " computational steps. ";
  if (
    this->comments.flagGenerateComments &&
    this->comments.totalSteps > this->comments.maximumStepsToRecord
  ) {
    out
    << "<br>Only the first"
    << this->comments.maximumStepsToRecord
    << " are shown. ";
  }
  out << StringRoutines::join(this->comments.comments, "<br>");
  if (
    !this->comments.flagGenerateComments ||
    this->comments.totalSteps > this->comments.maximumStepsToRecord
  ) {
    out << this->collection.toHTMLGraphicsOnly(false);
  }
  for (Cone& cone : this->collection.refinedCones.values) {
    out << "<hr>";
    out << "Cone: " << cone.id << ".<br>";
    out << "Quasipolynomial:<br>" << cone.payload.getPolynomial().toHTML();
  }
  return out.str();
}

void VectorPartitionFunctionElementary::compute() {
  STACK_TRACE("VectorPartitionFunctionElementary::compute");
  ProgressReport report;
  this->collection.initializeFromDirections(this->originalVectors);
  this->comments.initialize(this);
  for (int i = 0; i < this->originalVectors.size; i ++) {
    this->collection.markAllConesNonRefined(i);
    this->collection.refineByOneDirection(i, &report);
    if (i >= this->collection.getDimension() - 1) {
      this->comments.addGraphicsIfAllowed();
    }
    this->computeQuasiPolynomials(i);
    this->comments.comments.addOnTop("<hr><hr>");
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
  STACK_TRACE(
    "VectorPartitionFunctionElementary::computeFirstQuasiPolynomial"
  );
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

bool VectorPartitionFunctionElementary::computeStartingQuasipolynomial(
  Cone& cone
) {
  STACK_TRACE(
    "VectorPartitionFunctionElementary::"
    "computeStartingQuasipolynomial"
  );
  Cone baseCone;
  List<Vector<Rational> > firstDirections;
  this->originalVectors.slice(
    0, this->collection.getDimension(), firstDirections
  );
  baseCone.createFromVertices(firstDirections);
  QuasiPolynomial output;
  if (!baseCone.isInCone(cone.internalPoint())) {
    output.ambientLatticeReduced.makeZn(this->collection.getDimension());
    cone.payload.setPolynomial(output);
    if (this->comments.shouldComment()) {
      this->comments.comments.addOnTop(((std::string) "Zero quasipolynomial: "
        ) +
        "the cone is outside of the span of " +
        firstDirections.toString()
      );
    }
    return true;
  }
  output.ambientLatticeReduced.makeFromRoots(firstDirections);
  Polynomial<Rational> one;
  one.makeOne();
  Vector<Rational> zeroVector;
  zeroVector.makeZero(this->collection.getDimension());
  output.addLatticeShift(one, zeroVector);
  cone.payload.setPolynomial(output);
  if (this->comments.shouldComment()) {
    std::stringstream report;
    report << "Initial quasipolynomial value equals 1. " << output.toHTML();
    this->comments.comments.addOnTop(report.str());
  }
  return true;
}

bool VectorPartitionFunctionElementary::computeOneQuasiPolynomial(
  Cone& cone, int directionIndex
) {
  STACK_TRACE("VectorPartitionFunctionElementary::computeOneQuasiPolynomial");
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
  this->comments.totalSteps ++;
  if (this->comments.shouldComment()) {
    std::stringstream commentStream;
    commentStream << "Cone id: " << cone.id << ".";
    this->comments.comments.addOnTop(commentStream.str());
  }
  if (directionIndex == this->collection.getDimension() - 1) {
    return this->computeStartingQuasipolynomial(cone);
  }
  QuasiPolynomial output;
  this->sumQuasiPolynomialOverCone(cone, direction, exitWalls[0], output);
  List<int> exitCones;
  this->getExitConesAfterStart(cone, direction, exitCones);
  List<Wall> exitWallsNeighbor;
  for (int i : exitCones) {
    Cone& next = this->collection.getConeByIdNonConstNoFail(i);
    this->collection.allExitWallsAreVisited(
      next, direction, exitWallsNeighbor
    );
    if (exitWallsNeighbor.size != 1) {
      global.fatal << "Single expected wall expected here. " << global.fatal;
    }
    Vector<Rational> exitWallNeighbor = exitWallsNeighbor[0].normal;
    this->addSingleNeighborContribution(
      next, direction, exitWalls[0].normal, exitWallNeighbor, output
    );
  }
  cone.payload.setPolynomial(output);
  return true;
}

void VectorPartitionFunctionElementary::getExitConesAfterStart(
  Cone& start, Vector<Rational>& direction, List<int>& output
) {
  output.clear();
  Cone* next = &start;
  List<Wall> exitWalls;
  while (next != nullptr) {
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
    if (next != nullptr) {
      output.addOnTop(next->id);
    }
  }
}

void VectorPartitionFunctionElementary::addSingleNeighborContribution(
  Cone& cone,
  const Vector<Rational>& direction,
  const Vector<Rational>& entranceWall,
  const Vector<Rational>& exitWall,
  QuasiPolynomial& outputAccumulator
) {
  STACK_TRACE(
    "VectorPartitionFunctionElementary::"
    "addSingleNeighborContribution"
  );
  const QuasiPolynomial& toBeSubstituted = cone.payload.getPolynomial();
  Vector<Rational> exitWallRescaled = exitWall;
  exitWallRescaled /= direction.scalarEuclidean(exitWall);
  Vector<Rational> entranceWallRescaled = entranceWall;
  entranceWallRescaled /= direction.scalarEuclidean(entranceWall);
  QuasiPolynomial summand;
  QuasiPolynomial subtracand;
  toBeSubstituted.substituteShiftByFloorOfLinearFunction(
    entranceWallRescaled,
    1,
    direction,
    summand,
    this->comments.commentsPointer()
  );
  toBeSubstituted.substituteShiftByFloorOfLinearFunction(
    exitWallRescaled,
    1,
    direction,
    subtracand,
    this->comments.commentsPointer()
  );
  outputAccumulator += summand;
  outputAccumulator -= subtracand;
}

void VectorPartitionFunctionElementary::
computeSingleNeighborSingleShiftContribution(
  const Polynomial<Rational>& toBeSubstituted,
  const Vector<Rational>& shift,
  const Lattice& ambientLattice,
  const Vector<Rational>& direction,
  const Vector<Rational>& exitWallRescaled,
  QuasiPolynomial& output
) {
  STACK_TRACE(
    "VectorPartitionFunctionElementary::"
    "computeSingleNeighborSingleShiftContribution"
  );
  Lattice rougherLattice;
  ambientLattice.subLatticeWithIntegralScalarProducts(
    exitWallRescaled, rougherLattice
  );
  Vectors<Rational> representatives;
  ambientLattice.getAllRepresentatives(rougherLattice, representatives);
  int dimension = this->collection.getDimension();
  Polynomial<Rational> linearFunction;
  PolynomialSubstitution<Rational> substitution;
  substitution.setSize(dimension);
  Polynomial<Rational> substitutedIn;
  output.makeZeroOverLattice(rougherLattice);
  for (Vector<Rational>& representative : representatives) {
    if (!rougherLattice.isInLattice(representative - shift)) {
      continue;
    }
    linearFunction.makeLinearNoConstant(exitWallRescaled);
    linearFunction -=
    exitWallRescaled.scalarEuclidean(representative).fractionalValue();
    linearFunction += 1;
    for (int i = 0; i < dimension; i ++) {
      substitution[i].makeMonomial(i, 1, 1);
      substitution[i] -= linearFunction * direction[i];
    }
    substitutedIn = toBeSubstituted;
    substitutedIn.substitute(substitution, 1);
    output.addLatticeShift(substitutedIn, representative);
  }
}

void VectorPartitionFunctionElementary::induceQuasiPolynomialFromWall(
  const Vector<Rational>& direction,
  const Wall& exitWall,
  Cone& neighbor,
  QuasiPolynomial& output
) {
  STACK_TRACE(
    "VectorPartitionFunctionElementary::induceQuasiPolynomialFromWall"
  );
  QuasiPolynomial pivotValue = neighbor.payload.previousPolynomial;
  Vector<Rational> normalRescaled = exitWall.normal;
  normalRescaled /= direction.scalarEuclidean(normalRescaled);
  pivotValue.substituteShiftByFloorOfLinearFunction(
    normalRescaled,
    0,
    direction,
    output,
    this->comments.commentsPointer()
  );
  int dimension = this->collection.getDimension();
  QuasiPolynomial indicatorIntegral;
  Lattice zN;
  zN.makeZn(dimension);
  Lattice integral;
  zN.subLatticeWithIntegralScalarProducts(normalRescaled, integral);
  indicatorIntegral.makeZeroOverLattice(integral);
  Polynomial<Rational> one;
  one.makeOne();
  Vector<Rational> zeroVector;
  zeroVector.makeZero(dimension);
  indicatorIntegral.addLatticeShift(one, zeroVector);
  if (this->comments.shouldComment()) {
    this->comments.comments.addOnTop("About to multiply: ");
    this->comments.comments.addOnTop(output.toHTML());
    this->comments.comments.addOnTop("by the indicator: ");
    this->comments.comments.addOnTop(indicatorIntegral.toHTML());
  }
  output *= indicatorIntegral;
  if (this->comments.shouldComment()) {
    this->comments.comments.addOnTop("Induced starting value: ");
    this->comments.comments.addOnTop(output.toHTML());
  }
}

void VectorPartitionFunctionElementary::sumQuasiPolynomialOverCone(
  Cone& cone,
  const Vector<Rational>& direction,
  const Wall& exitWall,
  QuasiPolynomial& output
) {
  STACK_TRACE(
    "VectorPartitionFunctionElementary::"
    "sumQuasiPolynomialOverCone"
  );
  QuasiPolynomial toBeIntegrated = cone.payload.getPolynomial();
  if (toBeIntegrated.isEqualToZero()) {
    if (exitWall.neighbors.size == 0) {
      global.fatal
      << "Not expected: no neighbors along wall. "
      << global.fatal;
    }
    Cone & neighbor =
    this->collection.getConeByIdNonConstNoFail(exitWall.neighbors[0]);
    this->induceQuasiPolynomialFromWall(
      direction, exitWall, neighbor, output
    );
    return;
  }
  output.makeZeroOverLattice(toBeIntegrated.ambientLatticeReduced);
  int scale =
  toBeIntegrated.ambientLatticeReduced.
  getMinimalIntegerScalarSendingVectorIntoLattice(direction);
  for (int i = 0; i < scale; i ++) {
    this->computeOneQuasiPolynomialExitWallWithoutNeighborOneScale(
      toBeIntegrated, i, scale, output, direction, exitWall.normal
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
  Vector<Rational> exitWallRescaled = exitWall;
  exitWallRescaled /= direction.scalarEuclidean(exitWall);
  exitWallRescaled /= scale;
  Rational rationalShift = - shift;
  rationalShift /= scale;
  QuasiPolynomial contribution;
  for (int i = 0; i < toBeIntegrated.latticeShifts.size; i ++) {
    Vector<Rational> latticeShift = toBeIntegrated.latticeShifts[i];
    latticeShift += direction * shift;
    this->computeOneQuasiPolynomialExitWallWithoutNeighborOneScaleOneShift(
      toBeIntegrated.valueOnEachLatticeShift[i],
      substitution,
      exitWallRescaled,
      rationalShift,
      toBeIntegrated.ambientLatticeReduced,
      latticeShift,
      contribution
    );
  }
  outputAccumulator += contribution;
}

void VectorPartitionFunctionElementary::
computeOneQuasiPolynomialExitWallWithoutNeighborOneScaleOneShift(
  const Polynomial<Rational>& toBeIntegrated,
  const PolynomialSubstitution<Rational>& substitution,
  const Vector<Rational>& scalarProductBy,
  const Rational& additionalConstantTerm,
  const Lattice& startingLattice,
  const Vector<Rational>& startingShift,
  QuasiPolynomial& output
) {
  STACK_TRACE(
    "VectorPartitionFunctionElementary::"
    "computeOneQuasiPolynomialExitWallWithoutNeighborOneScaleOneShift"
  );
  Polynomial<Rational> substituted;
  substituted = toBeIntegrated;
  substituted.substitute(substitution, 1);
  int startingDegree = toBeIntegrated.totalDegreeInt();
  int dimension = scalarProductBy.size;
  Polynomial<Rational> coefficientInFrontOfPower;
  Polynomial<Rational> bernoulliSum;
  QuasiPolynomial bernoulliSumSubstituted;
  output.makeZeroOverLattice(startingLattice);
  for (int i = 0; i <= startingDegree; i ++) {
    substituted.getCoefficientPolynomialOfXPowerK(
      dimension, i, coefficientInFrontOfPower
    );
    this->bernoulliSumComputer.getBernoulliSumStartingAtZero(i, bernoulliSum);
    bernoulliSumSubstituted.assignPolynomialOfFloorOfLinearFunction(
      bernoulliSum,
      scalarProductBy,
      additionalConstantTerm,
      startingLattice,
      startingShift
    );
    output += bernoulliSumSubstituted;
  }
}

void VectorPartitionFunctionElementary::Comments::initialize(
  VectorPartitionFunctionElementary* inputOwner
) {
  this->owner = inputOwner;
}

void VectorPartitionFunctionElementary::Comments::addGraphicsIfAllowed() {
  int largestId = this->owner->collection.largestConeId();
  if (largestId == this->largestIdWithGeneratedGraphics) {
    return;
  }
  this->largestIdWithGeneratedGraphics = largestId;
  this->comments.addOnTop(
    this->owner->collection.toHTMLGraphicsOnly(false)
  );
}

List<std::string>* VectorPartitionFunctionElementary::Comments::commentsPointer
() {
  if (!this->shouldComment()) {
    return nullptr;
  }
  return &this->comments;
}

bool VectorPartitionFunctionElementary::Comments::shouldComment() const {
  if (!this->flagGenerateComments) {
    return false;
  }
  if (this->totalSteps >= this->maximumStepsToRecord) {
    return false;
  }
  return true;
}

VectorPartitionFunctionElementary::Comments::Comments() {
  this->owner = nullptr;
  this->maximumStepsToRecord = 10;
  this->totalSteps = 0;
  this->flagGenerateComments = false;
  this->largestIdWithGeneratedGraphics = - 1;
}

void BernoulliSumComputer::getBernoulliSumStartingAtOne(
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

void BernoulliSumComputer::getBernoulliSumStartingAtZero(
  int power, Polynomial<Rational>& output
) {
  this->getBernoulliSumStartingAtOne(power, output);
  if (power == 0) {
    output += 1;
  }
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
