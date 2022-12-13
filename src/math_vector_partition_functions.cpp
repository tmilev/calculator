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
    out << "Quasipolynomial: " << cone.payload.getPolynomial().toHTML();
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

bool VectorPartitionFunctionElementary::computeStartingQuasipolynomial(Cone &cone)
{
  STACK_TRACE("VectorPartitionFunctionElementary::"
  "computeStartingQuasipolynomial");
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
    return true;
  }
output.ambientLatticeReduced.makeFromRoots(firstDirections);

  Polynomial<Rational> one;
  one.makeOne();
  Vector<Rational> zeroVector;
  zeroVector.makeZero(this->collection.getDimension());
  output.addLatticeShift(one, zeroVector);
  cone.payload.setPolynomial(output);
  global.comments << "<br>DEBUG: computed " << cone.id << " vpf as: " << cone.payload.getPolynomial().toHTML();
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
  if (directionIndex == this->collection.getDimension() - 1) {
    return this->computeStartingQuasipolynomial(cone);
  }
  QuasiPolynomial output;
  this->sumQuasiPolynomialOverCone(
    cone, direction, exitWalls[0], output
  );
  global.comments << "<br>DEBUG: output before neighbis: "
  << output.toHTML();
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
    global.comments << "<br>DEBUG: handling neighbor: " << i;
    this->addSingleNeighborContribution(
      next, direction, exitWalls[0].normal, exitWallNeighbor, output
    );
  }
  cone.payload.setPolynomial( output);
  global.comments << "<br>DEBUG: computed " << cone.id << " vpf as: " << cone.payload.getPolynomial().toHTML();

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
  for (int i = 0; i < toBeSubstituted.latticeShifts.size; i ++) {
    Vector<Rational> shift = toBeSubstituted.latticeShifts[i];
    this->computeSingleNeighborSingleShiftContribution(
      toBeSubstituted.valueOnEachLatticeShift[i],
      toBeSubstituted.latticeShifts[i],
      toBeSubstituted.ambientLatticeReduced,
      direction,
      entranceWallRescaled,
      summand
    );
    global.comments << "<br>DEBUG: entrance wall: " << entranceWallRescaled
    << " contribution: " << summand.toHTML();
    outputAccumulator += summand;
    this->computeSingleNeighborSingleShiftContribution(
      toBeSubstituted.valueOnEachLatticeShift[i],
      toBeSubstituted.latticeShifts[i],
      toBeSubstituted.ambientLatticeReduced,
      direction,
      exitWallRescaled,
      summand
    );
    global.comments << "<br>DEBUG: exit wall: " << exitWall
    << " negative contribution: " << summand.toHTML();
    outputAccumulator -= summand;
  }
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
    "addSingleNeighborSingleShiftContribution"
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

void VectorPartitionFunctionElementary::sumZeroQuasiPolynomialFromWall(
const Vector<Rational> &direction, const Wall &exitWall, Cone &neighbor, QuasiPolynomial &output){
  STACK_TRACE("VectorPartitionFunctionElementary::sumZeroQuasiPolynomialFromWall");
  global.comments << "<br>DEBUG: insdie sumZeroQuasiPolynomialFromWall";
  QuasiPolynomial pivotValue = neighbor.payload.previousPolynomial;
  global.comments << "<br>DEBUG: neighbi payload: " << pivotValue.toHTML();
  int modulus = pivotValue.ambientLatticeReduced.getMinimalIntegerScalarSendingVectorIntoLattice(direction);

  Lattice zN;
  zN.makeZn(this->collection.getDimension());
  Lattice rougherLattice;
  Lattice intermediateLattice;
  Vector<Rational> normalRescaled = exitWall.normal;
  normalRescaled /= direction.scalarEuclidean(normalRescaled);
  pivotValue.ambientLatticeReduced.subLatticeWithIntegralScalarProducts(normalRescaled, intermediateLattice);
  pivotValue.ambientLatticeReduced.subLatticeWithIntegralScalarProducts(normalRescaled / modulus, rougherLattice);
  Vectors<Rational> representatives;

  zN.getAllRepresentatives(rougherLattice, representatives);
  output.makeZeroOverLattice(rougherLattice);
  for (Vector<Rational> &representative : representatives)
  {
    if (!intermediateLattice.isInLattice(representative))
    {
      continue;
    }
    for (int i = 0; i < pivotValue.latticeShifts.size; i++)
    {
      this->sumZeroQuasiPolynomialFromWallOnce(pivotValue.latticeShifts[i], pivotValue.ambientLatticeReduced, pivotValue.valueOnEachLatticeShift[i], representative, direction, normalRescaled, output);
    }
  }
}

void VectorPartitionFunctionElementary::sumZeroQuasiPolynomialFromWallOnce(
    const Vector<Rational> &neighborShift,
    const Lattice &neighborLattice,
    const Polynomial<Rational> &pivotValue,
    const Vector<Rational> &representative,
    const Vector<Rational> &direction,
    const Vector<Rational> &normalRescaled,
    QuasiPolynomial &output

)
{
  Vector<Rational> sum = neighborShift;
  sum += direction * representative.scalarEuclidean(normalRescaled);
  if (!neighborLattice.isInLattice(sum))
  {
    return;
  }
  PolynomialSubstitution<Rational> substitution;
  int dimension = this->collection.getDimension();
  substitution.setSize(dimension);
  Polynomial<Rational> linearFunction;
  linearFunction.makeLinearNoConstant(normalRescaled);
  for (int i = 0; i < dimension; i++)
  {
    substitution[i].makeMonomial(i, 1, 1);
    substitution[i] -= linearFunction * direction[i];
  }

  Polynomial<Rational> substituted = pivotValue;
  substituted.substitute(substitution, 1);
  output.addLatticeShift(substituted, representative);
}

void VectorPartitionFunctionElementary::sumQuasiPolynomialOverCone(
  Cone& cone,
  const Vector<Rational>& direction,
  const Wall& exitWall,
  QuasiPolynomial& output
) {
  STACK_TRACE(
    "VectorPartitionFunctionElementary::"
    "accumulateQuasiPolynomialExitWallWithoutNeighbor"
  );
  QuasiPolynomial toBeIntegrated = cone.payload.getPolynomial();
  if (toBeIntegrated.isEqualToZero()) {
    if (exitWall.neighbors.size == 0)
    {
      global.comments << "<br>Cone of id: " << cone.id << " has no neighbors along: " << exitWall.toString();
      global.comments << "<br>All walls: " << cone.walls.toStringCommaDelimited();
      global.comments << "<br>Direction: " << direction.toString();

      global.comments << "<br>To be integrated: " << cone.payload.getPolynomial().toHTML() << " over " << cone.payload.getPolynomial().ambientLatticeReduced.toString();
      global.comments << "<br>" << this->collection.toStringNeighborGraph() << "<br>";
      global.comments << this->collection.toHTMLGraphicsOnly(false);
      global.fatal << "Not expected: no neighbors along wall. " << global.fatal;
    }
    Cone& neighbor = this->collection.getConeByIdNonConstNoFail(exitWall.neighbors[0]);
  this->sumZeroQuasiPolynomialFromWall( direction,exitWall, neighbor, output);
return;
  }
  output.makeZeroOverLattice( toBeIntegrated.ambientLatticeReduced);
  int scale = toBeIntegrated.ambientLatticeReduced.getMinimalIntegerScalarSendingVectorIntoLattice(direction);
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
  global.comments << "<hr>DEBUG: integrating " << toBeIntegrated.toHTML()
                  << " with shfit: " << shift << ", scale: " << scale << ", direction: " << direction.toString() << ", exit wall: " << exitWall.toString();
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
        exitWallRescaled,
        latticeFromWallDistance);
  }
}

void VectorPartitionFunctionElementary::
    computeOneQuasiPolynomialExitWallWithoutNeighborOneScaleOneShift(
        const QuasiPolynomial &toBeIntegrated,
        const Rational &rationalShift,
        PolynomialSubstitution<Rational> &substitution,
        const Vector<Rational> &latticeShift,
        const Polynomial<Rational> &valueOnLatticeShift,
        QuasiPolynomial &outputAccumulator,
        const Vector<Rational> &exitWallRescaled,
        Lattice &rougherLattice)
{
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
    bernoulliSubstitution[0].makeLinearNoConstant(exitWallRescaled);
    bernoulliSubstitution[0] -= (exitWallRescaled.scalarEuclidean(representative) + rationalShift).fractionalValue();
    global.comments << "<br>DEBUG: Bernoulli substitution: " << bernoulliSubstitution.toString()
                    << " from exit wall: " << exitWallRescaled;
    int dimension = this->collection.getDimension();
    for (int i = 0; i <= startingDegree; i ++) {
      value.getCoefficientPolynomialOfXPowerK(
        dimension, i, coefficientInFrontOfPower
      );
      this->bernoulliSumComputer.getBernoulliSumStartingAtZero(i, summand);
      summand.substitute(bernoulliSubstitution, 1);
      summand *= coefficientInFrontOfPower;
      outputAccumulator.addLatticeShift(summand, representative);
    }
  }
}

void BernoulliSumComputer::getBernoulliSumStartingAtOne(int power, Polynomial<Rational> &output)
 {
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

void BernoulliSumComputer::getBernoulliSumStartingAtZero(int power, Polynomial<Rational> &output){
  this->getBernoulliSumStartingAtOne(power, output);
  if (power ==0){
    output+=1;
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
