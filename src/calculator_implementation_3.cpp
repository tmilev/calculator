#include "calculator_interface.h"
#include "general_file_operations_encodings.h"
#include "math_extra_drawing_variables.h"
#include "math_extra_finite_groups_implementation.h"
#include "math_extra_lie_theory_extras.h"
#include "math_rational_function.h"
#include "math_subsets_selections.h"
#include "string_constants.h"

// If you get the C++ multiple definition error:
// - Try moving template *EXPLICIT* specializations (i.e. template <>)
//   to the *beginning* (!) of .cpp files.
// - Try moving template generics into .h files.
template < >
bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::
isDominantWithRespectToGenerator<RationalFraction<Rational> >(
  const Vector<RationalFraction<Rational> >& weight, int generatorIndex
) {
  STACK_TRACE(
    "SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms"
    "::isDominantWithRespectToGenerator"
  );
  this->checkInitialization();
  Vector<RationalFraction<Rational> > currentVector;
  RationalFraction<Rational> rationalFraction;
  currentVector = this->simpleRootsInner[generatorIndex].getVectorRational();
  rationalFraction =
  this->ambientWeyl->rootScalarCartanRoot(weight, currentVector);
  if (
    rationalFraction.expressionType !=
    RationalFraction<Rational>::TypeExpression::typeConstant
  ) {
    global.fatal
    << "This might or might not be a programming mistake: "
    << "I am being asked whether a weight "
    << "with rational function coefficients is dominant. "
    << "I took the scalar products with the positive simple roots "
    << "whose reflections generate the ambient group, "
    << "however one of the scalar products in question was non-constant. "
    << "More precisely, the scalar product of "
    << weight.toString()
    << " and "
    << currentVector.toString()
    << " "
    << "equals "
    << rationalFraction.toString()
    << ". "
    << "I cannot decide (more precisely, "
    << "do not want to *silently* decide for you) "
    << "whether a non-constant function is to be declared positive or not. "
    << "If this is not a programming mistake, "
    << "you might want to consider introducing a substitution "
    << "evaluating the rational function, some sort of a monomial order, "
    << "or some other method of deciding the "
    << "\"sign\" of a rational function. "
    << "Whether or not this is a mistake, I am crashing. "
    << global.fatal;
    return false;
  }
  return !rationalFraction.constantValue.isNegative();
}

template < >
bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::
isDominantWithRespectToGenerator<Rational>(
  const Vector<Rational>& weight, int generatorIndex
) {
  this->checkInitialization();
  return
  !this->ambientWeyl->rootScalarCartanRoot(
    weight, this->simpleRootsInner[generatorIndex]
  ).isNegative();
}

template < >
bool WeylGroupData::isDominantWithRespectToGenerator<
  RationalFraction<Rational>
>(const Vector<RationalFraction<Rational> >& weight, int generatorIndex) {
  Vector<Rational> unitVector;
  RationalFraction<Rational> coefficient;
  unitVector.makeEi(this->getDimension(), generatorIndex);
  coefficient = this->rootScalarCartanRoot(weight, unitVector);
  if (
    coefficient.expressionType !=
    RationalFraction<Rational>::TypeExpression::typeConstant
  ) {
    global.fatal
    << "This might or might not be a programming mistake: "
    << "I am being asked whether a weight "
    << "with rational function coefficients is dominant. "
    << "I took the scalar products with the positive simple roots "
    << "whose reflections generate the ambient group, "
    << "however one of the scalar products in question was non-constant. "
    << "More precisely, the scalar product of "
    << weight.toString()
    << " and "
    << unitVector.toString()
    << " "
    << "equals "
    << coefficient.toString()
    << ". "
    << "I cannot decide (more precisely, "
    << "do not want to *silently* decide for you) "
    << "whether a non-constant function is positive or not. "
    << "If this is not a programming mistake, "
    << "you might want to consider introducing a substitution "
    << "evaluating the rational function, some sort of a monomial order, "
    << "or some other method of deciding the "
    << "'sign' of a rational function. "
    << "Whether or not this is a mistake, I am crashing.  "
    << global.fatal;
    return false;
  }
  return !coefficient.constantValue.isNegative();
}

template < >
bool WeylGroupData::isDominantWithRespectToGenerator<Rational>(
  const Vector<Rational>& weight, int generatorIndex
) {
  return
  !this->getScalarProductSimpleRoot(weight, generatorIndex).isNegative();
}

void SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::
makeParabolicFromSelectionSimpleRoots(
  WeylGroupData& inputWeyl,
  const Vector<Rational>& zeroesMeanSimpleRootSpaceIsInParabolic,
  int upperLimitNumberOfElements
) {
  Selection selection;
  selection = zeroesMeanSimpleRootSpaceIsInParabolic;
  this->makeParabolicFromSelectionSimpleRoots(
    inputWeyl, selection, upperLimitNumberOfElements
  );
}

bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::
getAllDominantWeightsHWFDIMwithRespectToAmbientAlgebra(
  Vector<Rational>& highestWeightSimpleCoordinates,
  HashedList<Vector<Rational> >& outputWeightsSimpleCoordinates,
  int upperBoundDominantWeights,
  std::string& outputDetails
) {
  STACK_TRACE(
    "SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms"
    "::getAllDominantWeightsHWFDIMwithRespectToAmbientAlgebra"
  );
  this->checkInitialization();
  std::stringstream out;
  Vector<Rational> highestWeightTrue = highestWeightSimpleCoordinates;
  Vectors<Rational> basisEi;
  int dimension = this->ambientWeyl->getDimension();
  basisEi.makeEiBasis(dimension);
  this->raiseToDominantWeightInner(highestWeightTrue);
  Vector<Rational> highestWeightFundamentalCoordinates =
  this->ambientWeyl->getFundamentalCoordinatesFromSimple(highestWeightTrue);
  if (!highestWeightFundamentalCoordinates.sumCoordinates().isSmallInteger()) {
    return false;
  }
  int topHeightSimpleCoordinates = static_cast<int>(
    highestWeightSimpleCoordinates.sumCoordinates().getDoubleValue()
  ) +
  1;
  if (topHeightSimpleCoordinates < 0) {
    topHeightSimpleCoordinates = 0;
  }
  List<HashedList<Vector<Rational> > > outputWeightsByHeight;
  int topHeightRootSystem =
  this->ambientWeyl->rootsOfBorel.lastObject()->sumCoordinates().
  numeratorShort;
  int topHeightRootSystemPlusOne = topHeightRootSystem + 1;
  outputWeightsByHeight.setSize(topHeightRootSystemPlusOne);
  int finalHashSize = 100;
  for (int i = 0; i < topHeightRootSystemPlusOne; i ++) {
    outputWeightsByHeight[i].setHashSize(finalHashSize);
  }
  outputWeightsSimpleCoordinates.clear();
  outputWeightsByHeight[0].addOnTop(highestWeightTrue);
  int totalWeightsFound = 0;
  int positiveRootCount = this->ambientWeyl->rootsOfBorel.size;
  Vector<Rational> currentWeight;
  Vector<Rational> currentWeightRaisedToDominantWRTAmbientAlgebra;
  for (
    int lowestUnexploredHeightDiff = 0; lowestUnexploredHeightDiff <=
    topHeightSimpleCoordinates; lowestUnexploredHeightDiff ++
  ) {
    if (
      upperBoundDominantWeights > 0 &&
      totalWeightsFound > upperBoundDominantWeights
    ) {
      break;
    }
    int bufferIndexShift = lowestUnexploredHeightDiff %
    topHeightRootSystemPlusOne;
    HashedList<Vector<Rational> >& currentHashes =
    outputWeightsByHeight[bufferIndexShift];
    for (int lowest = 0; lowest < currentHashes.size; lowest ++) {
      for (int i = 0; i < positiveRootCount; i ++) {
        currentWeight = currentHashes[lowest];
        currentWeight -= this->ambientWeyl->rootsOfBorel[i];
        if (this->isDominantWeight(currentWeight)) {
          currentWeightRaisedToDominantWRTAmbientAlgebra = currentWeight;
          this->ambientWeyl->raiseToDominantWeight(
            currentWeightRaisedToDominantWRTAmbientAlgebra
          );
          currentWeightRaisedToDominantWRTAmbientAlgebra -= highestWeightTrue;
          if (
            currentWeightRaisedToDominantWRTAmbientAlgebra.isNegativeOrZero()
          ) {
            int currentIndexShift =
            this->ambientWeyl->rootsOfBorel[i].sumCoordinates().numeratorShort;
            currentIndexShift = (currentIndexShift + bufferIndexShift) %
            topHeightRootSystemPlusOne;
            if (
              outputWeightsByHeight[currentIndexShift].addOnTopNoRepetition(
                currentWeight
              )
            ) {
              totalWeightsFound ++;
              outputWeightsByHeight[currentIndexShift].adjustHashes();
            }
          }
        }
      }
    }
    outputWeightsSimpleCoordinates.addListOnTop(currentHashes);
    outputWeightsSimpleCoordinates.adjustHashes();
    currentHashes.clear();
  }
  out
  << " Total number of dominant weights: "
  << outputWeightsSimpleCoordinates.size;
  if (totalWeightsFound >= upperBoundDominantWeights) {
    out
    << "<hr>This message is generated either because the number of "
    << "weights has exceeded the hard-coded RAM memory limits, or because "
    << "a priori bound for the number of weights is WRONG. "
    << "If the latter is the case, "
    << "make sure to send an angry email to the author(s).";
  }
  outputDetails = out.str();
  return (totalWeightsFound <= upperBoundDominantWeights);
}

std::string LittelmannPath::generateOrbitAndAnimate() {
  std::stringstream out;
  List<LittelmannPath> orbit;
  List<List<int> > generators;
  if (!this->generateOrbit(orbit, generators, 1000, nullptr)) {
    out
    << "<b>Not all paths were genenerated, only the first "
    << orbit.size
    << "</b>";
  }
  Vectors<double> coxeterPlane;
  coxeterPlane.setSize(2);
  this->owner->getCoxeterPlane(coxeterPlane[0], coxeterPlane[1]);
  DrawingVariables animated, collapsed;
  this->owner->drawRootSystem(animated, true, false);
  this->owner->drawRootSystem(collapsed, true, false);
  for (int i = 0; i < orbit.size; i ++) {
    LittelmannPath& currentPath = orbit[i];
    animated.drawPath(
      currentPath.waypoints, "black", 1, this->owner->dynkinType.toString(), i
    );
    collapsed.drawPath(currentPath.waypoints, "black", 1);
    for (int j = 0; j < currentPath.waypoints.size; j ++) {
      animated.drawCircleAtVectorBufferRational(
        currentPath.waypoints[j],
        "purple",
        3,
        this->owner->dynkinType.toString(),
        i
      );
      collapsed.drawCircleAtVectorBufferRational(
        currentPath.waypoints[j], "purple", 3
      );
    }
  }
  out << "<br>Animation of the Littelmann paths follows. ";
  out << animated.getHTMLDiv(this->owner->getDimension(), true);
  out << "<br>Here are all Littelmann paths drawn simultaneously. ";
  out << collapsed.getHTMLDiv(this->owner->getDimension(), true);
  out
  << "Littelmann paths in simple coordinates "
  << "given in the order in which they are generated ("
  << orbit.size
  << " total):<br>";
  out << "<table>";
  for (int i = 0; i < orbit.size; i ++) {
    LittelmannPath& currentPath = orbit[i];
    out
    << "<tr><td>"
    << currentPath.toString()
    << "</td>"
    << "<td>"
    << this->toStringOperatorSequenceStartingOnMe(generators[i])
    << "</td></tr>";
  }
  out << "</table>";
  LittelmannPath lastPath = orbit[0];
  LittelmannPath path;
  MonomialTensor<int, HashFunctions::hashFunction> monomial;
  monomial = *generators.lastObject();
  monomial.generatorsIndices.reverseElements();
  monomial.powers.reverseElements();
  out << "<table>";
  for (int i = monomial.generatorsIndices.size - 1; i >= 1; i --) {
    int currentIndex = - monomial.generatorsIndices[i] - 1;
    int nextIndex = - monomial.generatorsIndices[i - 1] - 1;
    for (int k = 0; k < monomial.powers[i]; k ++) {
      lastPath.actByFAlpha(currentIndex);
    }
    path = lastPath;
    path.actByEAlpha(nextIndex);
    out
    << "<tr><td> e_"
    << nextIndex + 1
    << "("
    << lastPath.toString()
    << ") =</td>"
    << "<td>"
    << path.toString()
    << "</td>";
    for (int j = 0; j < this->owner->getDimension(); j ++) {
      path = lastPath;
      path.actByEAlpha(j);
      out
      << "<td> e_"
      << j + 1
      << "("
      << lastPath.toString()
      << ")=</td>"
      << "<td>"
      << path.toString()
      << "</td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  out
  << "<table><td>corresponding element of U(g)</td>"
  << "<td>is adapted</td>"
  << "<td>path</td>"
  << "<td>e operators with non-zero action.</td>";
  for (int i = 0; i < generators.size; i ++) {
    path = orbit[i];
    monomial = generators[i];
    monomial.generatorsIndices.reverseElements();
    monomial.powers.reverseElements();
    bool isadapted = path.isAdaptedString(monomial);
    out
    << "<tr><td>"
    << monomial.toString()
    << "</td><td>"
    << (isadapted ? "is adapted to" : "is not adapted to")
    << "</td><td>"
    << path.toString()
    << "</td><td>";
    for (int j = 0; j < this->owner->getDimension(); j ++) {
      path = orbit[i];
      path.actByEFDisplayIndex(j + 1);
      if (!path.isEqualToZero()) {
        out << "e_{" << j + 1 << "}, ";
      }
      path = orbit[i];
      path.actByEFDisplayIndex(- j - 1);
      if (!path.isEqualToZero()) {
        out << "e_{" << - j - 1 << "}, ";
      }
    }
    out << "</td></tr>";
  }
  out << "</table>";
  return out.str();
}

void Calculator::makeHmmG2InB3(HomomorphismSemisimpleLieAlgebra& output) {
  STACK_TRACE("Calculator::makeHmmG2InB3");
  DynkinType b3Type;
  DynkinType g2Type;
  b3Type.makeSimpleType('B', 3);
  g2Type.makeSimpleType('G', 2);
  output.domain =
  &this->objectContainer.getLieAlgebraCreateIfNotPresent(g2Type);
  output.coDomain =
  &this->objectContainer.getLieAlgebraCreateIfNotPresent(b3Type);
  output.coDomainAlgebra().computeChevalleyConstants();
  output.domainAlgebra().computeChevalleyConstants();
  ElementSemisimpleLieAlgebra<Rational> g_2;
  ElementSemisimpleLieAlgebra<Rational> g_1plusg_3;
  ElementSemisimpleLieAlgebra<Rational> g_m2;
  ElementSemisimpleLieAlgebra<Rational> g_m1plusg_m3;
  ElementSemisimpleLieAlgebra<Rational> element;
  g_2.makeGenerator(13, output.coDomainAlgebra());
  g_m2.makeGenerator(7, output.coDomainAlgebra());
  g_1plusg_3.makeGenerator(12, output.coDomainAlgebra());
  element.makeGenerator(14, output.coDomainAlgebra());
  g_1plusg_3 += element;
  g_m1plusg_m3.makeGenerator(6, output.coDomainAlgebra());
  element.makeGenerator(8, output.coDomainAlgebra());
  g_m1plusg_m3 += element;
  output.imagesPositiveSimpleChevalleyGenerators.setSize(2);
  output.imagesPositiveSimpleChevalleyGenerators[0] = g_1plusg_3;
  output.imagesPositiveSimpleChevalleyGenerators[1] = g_2;
  output.imagesNegativeSimpleChevalleyGenerators.setSize(2);
  output.imagesNegativeSimpleChevalleyGenerators[0] = g_m1plusg_m3;
  output.imagesNegativeSimpleChevalleyGenerators[1] = g_m2;
  output.computeHomomorphismFromImagesSimpleChevalleyGenerators(nullptr);
  output.getRestrictionAmbientRootSystemToTheSmallerCartanSubalgebra(
    output.restrictedRootSystem
  );
}

void BranchingData::resetOutputData() {
  this->eigenVectors.setSize(0);
  this->eigenVectorsLevi.setSize(0);
  this->outputEigenWords.setSize(0);
  this->g2Weights.setSize(0);
  this->outputWeightsFundamentalCoordinates.setSize(0);
  this->characterDifferences.clear();
}

void WeylGroupData::
getHighestWeightsAllRepresentationsDimensionLessThanOrEqualTo(
  List<Vector<Rational> >& outputHighestWeightsFundamentalCoordinates,
  int inputDimensionBound
) {
  if (inputDimensionBound < 1) {
    outputHighestWeightsFundamentalCoordinates.setSize(0);
    return;
  }
  HashedList<Vector<Rational> > output;
  Vector<Rational> current;
  current.makeZero(this->getDimension());
  output.addOnTop(current);
  Rational dimension;
  Rational dimBound = inputDimensionBound + 1;
  for (int i = 0; i < output.size; i ++) {
    current = output[i];
    for (int k = 0; k < this->getDimension(); k ++) {
      current[k] += 1;
      dimension = this->weylDimFormulaFundamentalCoordinates(current);
      if (dimension < dimBound) {
        output.addOnTopNoRepetition(current);
      }
      current[k] -= 1;
    }
  }
  outputHighestWeightsFundamentalCoordinates = output;
}

bool Expression::assignMatrixExpressions(
  const Matrix<Expression>& input,
  Calculator& owner,
  bool reduceOneRowToSequenceAndOneByOneToNonMatrix,
  bool dontReduceTypes
) {
  STACK_TRACE("Expression::assignMatrixExpressions");
  if (reduceOneRowToSequenceAndOneByOneToNonMatrix && input.numberOfRows == 1) {
    if (input.numberOfColumns == 1) {
      (*this) = input(0, 0);
      return true;
    }
    this->makeSequence(owner);
    for (int i = 0; i < input.numberOfColumns; i ++) {
      this->addChildOnTop(input(0, i));
    }
    return true;
  }
  this->reset(owner, input.numberOfRows + 1);
  Expression matrixType(owner);
  matrixType.addChildAtomOnTop(owner.opMatrix());
  this->addChildOnTop(matrixType);
  enum MatrixType {
    typeUnknown,
    typeRational,
    typeDouble,
    typeAlgebraic,
    typePolynomialRational,
    typePolynomialAlgebraic,
    typeRationalFraction,
    typeExpression
  };
  MatrixType outType = typeUnknown;
  Expression currentRow;
  for (int i = 0; i < input.numberOfRows; i ++) {
    currentRow.reset(owner);
    currentRow.children.reserve(input.numberOfColumns + 1);
    currentRow.addChildAtomOnTop(owner.opSequence());
    for (int j = 0; j < input.numberOfColumns; j ++) {
      currentRow.addChildOnTop(input(i, j));
      MatrixType inType;
      if (input(i, j).isOfType<Rational>()) {
        inType = typeRational;
      } else if (input(i, j).isOfType<AlgebraicNumber>()) {
        inType = typeAlgebraic;
      } else if (input(i, j).isOfType<double>()) {
        inType = typeDouble;
      } else if (input(i, j).isOfType<Polynomial<Rational> >()) {
        inType = typePolynomialRational;
      } else if (input(i, j).isOfType<Polynomial<AlgebraicNumber> >()) {
        inType = typeAlgebraic;
      } else if (input(i, j).isOfType<RationalFraction<Rational> >()) {
        inType = typeRationalFraction;
      } else {
        inType = typeExpression;
      }
      if (outType == typeUnknown) {
        outType = inType;
      }
      if (inType == typeAlgebraic) {
        if (outType == typeRational) {
          outType = inType;
          continue;
        }
        if (
          outType == typeDouble ||
          outType == typeAlgebraic ||
          outType == typePolynomialAlgebraic
        ) {
          continue;
        }
        outType = typeExpression;
        continue;
      } else if (inType == typeDouble) {
        if (
          outType == typeRational ||
          outType == typeAlgebraic ||
          outType == typeDouble
        ) {
          outType = inType;
          continue;
        }
        outType = typeExpression;
        continue;
      } else if (inType == typePolynomialRational) {
        if (outType == typeRational) {
          outType = inType;
          continue;
        }
        if (outType == typeAlgebraic || outType == typePolynomialAlgebraic) {
          outType = typePolynomialAlgebraic;
          continue;
        }
        outType = typeExpression;
        continue;
      } else if (inType == typePolynomialAlgebraic) {
        if (
          outType == typeRational ||
          outType == typeAlgebraic ||
          outType == typePolynomialRational ||
          outType == typePolynomialAlgebraic
        ) {
          outType = inType;
          continue;
        }
        outType = typeExpression;
        continue;
      } else if (inType == typeRationalFraction) {
        if (outType == typeRational || outType == typePolynomialRational) {
          outType = inType;
          continue;
        }
        outType = typeExpression;
        continue;
      } else if (inType == typeExpression) {
        outType = inType;
      }
    }
    this->addChildOnTop(currentRow);
  }
  switch (outType) {
  case typeRational:
    matrixType.addChildAtomOnTop(owner.opRational());
    break;
  case typeDouble:
    matrixType.addChildAtomOnTop(owner.opDouble());
    break;
  case typeAlgebraic:
    matrixType.addChildAtomOnTop(owner.builtInCode<AlgebraicNumber>());
    break;
  case typePolynomialRational:
    matrixType.addChildAtomOnTop(owner.opPolynomialRational());
    break;
  case typePolynomialAlgebraic:
    matrixType.addChildAtomOnTop(owner.opPolynomialAlgebraicNumbers());
    break;
  case typeRationalFraction:
    matrixType.addChildAtomOnTop(owner.opRationalFraction());
    break;
  default:
    break;
  }
  if (outType != typeUnknown && outType != typeExpression && !dontReduceTypes) {
    this->setChild(0, matrixType);
  }
  return true;
}

bool Calculator::getMatrixExpressionsFromArguments(
  const Expression& input,
  Matrix<Expression>& output,
  int desiredRowCount,
  int desiredColumnCount
) {
  STACK_TRACE("Calculator::getMatrixExpressionsFromArguments");
  if (!input.isList()) {
    return false;
  }
  Expression inputModified = input;
  inputModified.setChildAtomValue(0, this->opSequence());
  return
  this->getMatrixExpressions(
    inputModified, output, desiredRowCount, desiredColumnCount
  );
}

bool Calculator::getMatrixExpressions(
  const Expression& input,
  Matrix<Expression>& output,
  int desiredNumberOfRows,
  int desiredNumberOfColumns
) {
  STACK_TRACE("Calculator::getMatrixExpressions");
  if (
    !input.isSequenceNElements() &&
    !input.isMatrix() &&
    !input.isIntervalRealLine()
  ) {
    output.initialize(1, 1);
    output(0, 0) = input;
    return true;
  }
  if (input.size() < 2) {
    if (input.isMatrix()) {
      output.initialize(0, 0);
      return true;
    }
    return false;
  }
  if (
    !input[1].isSequenceNElements() &&
    !input[1].startsWith(this->opIntervalOpen())
  ) {
    if (desiredNumberOfRows > 0) {
      if (desiredNumberOfRows != 1) {
        return false;
      }
    }
    if (desiredNumberOfColumns > 0) {
      if (desiredNumberOfColumns != input.size() - 1) {
        return false;
      }
    }
    output.initialize(1, input.size() - 1);
    for (int i = 1; i < input.size(); i ++) {
      output(0, i - 1) = input[i];
    }
    return true;
  }
  if (desiredNumberOfRows > 0) {
    if (desiredNumberOfRows != input.size() - 1) {
      return false;
    }
  }
  if (desiredNumberOfColumns > 0) {
    if (desiredNumberOfColumns != input[1].size() - 1) {
      return false;
    }
  }
  output.initialize(input.size() - 1, input[1].size() - 1);
  for (int i = 1; i < input.size(); i ++) {
    if (
      input[i].isSequenceNElements(output.numberOfColumns) ||
      input[i].startsWith(this->opIntervalOpen(), output.numberOfColumns + 1)
    ) {
      for (int j = 1; j < input[i].size(); j ++) {
        output(i - 1, j - 1) = input[i][j];
      }
    } else {
      return false;
    }
  }
  return true;
}

bool Calculator::Test::loadOneTest(JSData& input) {
  STACK_TRACE("Calculator::Test::loadOneTest");
  Calculator::Test::OneTest readerTest;
  if (!readerTest.fromJSON(input)) {
    return false;
  }
  std::string command = readerTest.command;
  if (!this->commands.contains(command)) {
    std::stringstream reportStream;
    reportStream
    << "Command ["
    << command
    << "] not recognized. "
    << "If the testing commands have recently changed, this is OK, "
    << "otherwise, it isn't.";
    global << Logger::red << reportStream.str() << Logger::endL;
    return false;
  }
  Calculator::Test::OneTest& output =
  this->commands.getValueNoFailNonConst(command);
  output.expectedResult = readerTest.expectedResult;
  output.expectedResultMathML = readerTest.expectedResultMathML;
  return true;
}

bool Calculator::Test::OneTest::fromJSON(JSData& input) {
  if (input["input"].elementType != JSData::Type::tokenString) {
    global << Logger::red << "Input command is missing. " << Logger::endL;
    return false;
  }
  this->command = input["input"].stringValue;
  if (input["output"].elementType != JSData::Type::tokenString) {
    global
    << Logger::red
    << "Command: "
    << this->command
    << " is missing its expected output. "
    << Logger::endL;
    return false;
  }
  if (input["outputMathML"].elementType != JSData::Type::tokenString) {
    input["outputMathML"] = "";
  }
  this->expectedResult = input["output"].stringValue;
  this->expectedResultMathML = input["outputMathML"].stringValue;
  return true;
}

bool Calculator::Test::loadTestStrings(std::stringstream* commentsOnFailure) {
  STACK_TRACE("Calculator::Test::loadTestStrings");
  if (
    !FileOperations::fileExistsVirtual(
      WebAPI::Calculator::testFileNameVirtual, false, false, nullptr
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Test json file does not exist. ";
    }
    this->flagTestResultsExist = false;
    return false;
  }
  std::string testStrings;
  if (
    !FileOperations::loadFileToStringVirtual(
      WebAPI::Calculator::testFileNameVirtual,
      testStrings,
      false,
      commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load test json. ";
    }
    return false;
  }
  if (!this->storedResults.parse(testStrings, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to parse test json. ";
    }
    return false;
  }
  if (this->storedResults.elementType != JSData::Type::tokenArray) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Test json is not an array. ";
    }
    return false;
  }
  for (int i = 0; i < this->storedResults.listObjects.size; i ++) {
    this->loadOneTest(this->storedResults.listObjects[i]);
  }
  return true;
}

std::string Calculator::writeFileToOutputFolderReturnLink(
  const std::string& fileContent,
  const std::string& fileName,
  const std::string& linkText
) {
  STACK_TRACE("Calculator::writeFileToOutputFolderReturnLink");
  std::string fileNameVirtual = "output/" + fileName;
  return
  FileOperations::writeFileReturnHTMLLink(
    fileContent, fileNameVirtual, linkText
  );
}

JSData Calculator::Test::OneTest::toJSON() const {
  JSData result;
  result["input"] = this->command;
  result["output"] = this->actualResult;
  if (this->actualResultMathML != "") {
    result["outputMathML"] = this->actualResultMathML;
  }
  return result;
}

bool Calculator::Test::writeTestStrings(std::stringstream* commentsOnFailure) {
  STACK_TRACE("Calculator::Test::writeTestStrings");
  JSData result;
  result.elementType = JSData::Type::tokenArray;
  result.listObjects.setExpectedSize(this->commands.size());
  result.listObjects.clear();
  for (Calculator::Test::OneTest& output : this->commands.values) {
    result.listObjects.addOnTop(output.toJSON());
  }
  return
  FileOperations::writeFileVirtual(
    WebAPI::Calculator::testFileNameVirtual,
    result.toString(&JSData::PrintOptions::newLine()),
    commentsOnFailure
  );
}

Calculator::Test::Test(Calculator& inputOwner) {
  this->startIndex = 0;
  this->startTime = global.getElapsedMilliseconds();
  this->inconsistencies = 0;
  this->inconsistenciesMathML = 0;
  this->unknown = 0;
  this->unknownMathML = 0;
  this->noTestSkips = 0;
  this->numberOfTests = 0;
  this->lastIndexNotTested = 0;
  this->owner = &inputOwner;
  this->flagTestResultsExist = true;
}

bool Calculator::automatedTest(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("Calculator::automatedTest");
  if (!global.userDefaultHasAdminRights()) {
    return calculator << "Automated test requires administrator access";
  }
  if (input.size() != 3) {
    return
    calculator
    << "Automated test expects two arguments: "
    << "index of first test to run and number of tests to run. ";
  }
  global.millisecondsMaxComputation = 30000000;
  // 30k seconds, ok as we have administrator access
  Calculator::Test test(calculator);
  if (
    !input[1].isSmallInteger(&test.startIndex) ||
    !input[2].isSmallInteger(&test.numberOfTests)
  ) {
    return
    calculator
    << "Automated test takes two arguments: "
    << "index of the first test to run and total "
    << "number of tests to run after that. ";
  }
  test.calculatorTestRun();
  return output.assignValue(calculator, test.reportHtml);
}

bool Calculator::Test::processResults() {
  std::stringstream commentsOnFailure;
  std::stringstream out;
  if (!this->loadTestStrings(&commentsOnFailure)) {
    global
    << Logger::red
    << "Failed to load test strings. "
    << Logger::endL
    << commentsOnFailure.str()
    << Logger::endL;
    out
    << "<b style='color:red'>Failed to load test strings. </b>"
    << commentsOnFailure.str();
  }
  if (!this->flagTestResultsExist) {
    out
    << "<b style='color:green'>Writing new test strings into: "
    << WebAPI::Calculator::testFileNameVirtual
    << ". </b>";
    std::stringstream commentsOnFailure2;
    if (!this->writeTestStrings(&commentsOnFailure2)) {
      global
      << Logger::red
      << "Failed to write test strings. "
      << Logger::endL
      << commentsOnFailure2.str();
      out
      << "<b style='color:red'>Write file failed. </b>"
      << commentsOnFailure2.str();
    }
  }
  std::stringstream goodCommands;
  std::stringstream unknownCommands;
  std::stringstream badCommands;
  std::stringstream badCommandsConsole;
  this->inconsistencies = 0;
  if (
    this->startIndex > 0 || this->lastIndexNotTested < this->commands.size()
  ) {
    out
    << "<b style='color:red'>Only "
    << this->numberOfTests
    << " out of "
    << this->commands.size()
    << " processed. </b>";
  }
  for (int i = this->startIndex; i < this->lastIndexNotTested; i ++) {
    Calculator::Test::OneTest& currentTest = this->commands.values[i];
    std::stringstream currentLine;
    std::stringstream currentLineConsole;
    currentLine << "<tr>";
    currentLine << "<td style='min-width:25px;'>" << i << "</td>";
    currentLineConsole << "Test " << i << "\n";
    currentLine
    << "<td style='min-width:200px;'>"
    << currentTest.functionAdditionalIdentifier
    << "</td>";
    currentLineConsole
    << "Function "
    << currentTest.functionAdditionalIdentifier
    << ", atom: "
    << currentTest.atom
    << "\n";
    currentLine
    << "<td style='min-width:45px;'>"
    << currentTest.atom
    << "</td>";
    currentLineConsole << "Ran:\n" << this->commands.keys[i] << "\n";
    currentLine
    << "<td style='min-width:200px;'>"
    << HtmlRoutines::getCalculatorComputationAnchorThisServer(
      this->commands.keys[i], this->commands.keys[i], true
    )
    << "</td>"
    << "<td>"
    << HtmlRoutines::getCalculatorComputationAnchorThisServer(
      "%UseMathML" + this->commands.keys[i], "mathML link", true
    )
    << "</td>";
    bool isBad = false;
    bool isUnknown = false;
    if (currentTest.actualResult == currentTest.expectedResult) {
      currentLine
      << "<td style='min-width:30px;'><b style='color:green;'>OK</b></td>";
    } else if (currentTest.expectedResult == "") {
      currentLine
      << "<td><b style='color:orange;'>expected result unknown</b></td>";
      isUnknown = true;
      this->unknown ++;
    } else {
      StringRoutines::Differ differ;
      differ.left =
      HtmlRoutines::convertStringToHtmlString(currentTest.actualResult, false);
      differ.right =
      HtmlRoutines::convertStringToHtmlString(
        currentTest.expectedResult, false
      );
      currentLine
      << "<td style='min-width:100px;'>"
      << "<b style='color:red'>unexpected result</b></td>"
      << "<td class='cellCalculatorResult'>";
      currentLine << differ.differenceHTML("actual", "expected");
      currentLine << "</td>";
      currentLineConsole << "Got:\n" << currentTest.actualResult << "\n";
      currentLineConsole
      << "Expected:\n"
      << currentTest.expectedResult
      << "\n";
      isBad = true;
      this->inconsistencies ++;
    }
    if (currentTest.actualResultMathML == currentTest.expectedResultMathML) {
      currentLine
      << "<td style='min-width:30px;'>"
      << "<b style='color:green;'>mathml OK</b>"
      << "</td>";
    } else if (currentTest.expectedResultMathML == "") {
      currentLine
      << "<td><b style='color:orange;'>expected mathml unknown</b></td>";
      isUnknown = true;
      this->unknownMathML ++;
    } else {
      StringRoutines::Differ differ;
      differ.left =
      HtmlRoutines::convertStringToHtmlString(
        currentTest.actualResultMathML, false
      );
      differ.right =
      HtmlRoutines::convertStringToHtmlString(
        currentTest.expectedResultMathML, false
      );
      currentLine
      << "<td style='min-width:100px;'>"
      << "<b style='color:red'>unexpected mathml</b></td>"
      << "<td class='cellCalculatorResult'>";
      currentLine << differ.differenceHTML("actual", "expected");
      currentLine << "</td>";
      currentLineConsole << "Got:\n" << currentTest.actualResultMathML << "\n";
      currentLineConsole
      << "Expected:\n"
      << currentTest.expectedResultMathML
      << "\n";
      isBad = true;
      this->inconsistenciesMathML ++;
    }
    currentLine << "</tr>";
    if (isBad) {
      badCommands << currentLine.str();
      badCommandsConsole << currentLineConsole.str();
    } else if (isUnknown) {
      unknownCommands << currentLine.str();
    } else {
      goodCommands << currentLine.str();
    }
  }
  if (this->noTestSkips > 0) {
    out
    << this->noTestSkips
    << " functions were not tested. "
    << "The kinds of functions not auto-tested are "
    << "described in the comments of class Function::Options. ";
  }
  if (this->inconsistencies > 0 || this->inconsistenciesMathML > 0) {
    out
    << "<b style='color:red'>"
    << "The test file results do not match the current results. </b>"
    << "There were "
    << this->inconsistencies
    << " inconsistencies. "
    << "If you think the current computations are correct, "
    << "say, the expected results have changed since the last test run, "
    << "erase file: "
    << WebAPI::Calculator::testFileNameVirtual
    << " and rerun the present test to store the expected results. "
    << "Alternatively, run: ./calculator test update";
    out
    << "<table class='tableCalculatorOutput'>"
    << badCommands.str()
    << "</table>";
    global
    << Logger::red
    << "There were "
    << this->inconsistencies
    << " latex inconsistencies and "
    << this->inconsistenciesMathML
    << " mathML inconsistencies."
    << Logger::endL;
    global << badCommandsConsole.str() << Logger::endL;
  }
  if (this->unknown > 0) {
    out
    << "<b style = 'color:orange'>There were "
    << this->unknown
    << " commands with no previously recorded regular outputs. </b>";
  }
  if (this->unknownMathML > 0) {
    out
    << "<b style = 'color:orange'>There were "
    << this->unknownMathML
    << " commands with no previously recorded MathML outputs. </b>";
  }
  if (this->unknownMathML > 0 || this->unknown > 0) {
    global
    << Logger::orange
    << "There were "
    << this->unknown
    << " commands with missing expected results and "
    << this->unknownMathML
    << " commands with missing mathML expected results. "
    << Logger::endL;
    if (this->flagTestResultsExist) {
      out
      << "<b>Erase file "
      << WebAPI::Calculator::testFileNameVirtual
      << " and rerun the present test to store the expected results.</b>";
      global
      << Logger::yellow
      << "Erase file "
      << WebAPI::Calculator::testFileNameVirtual
      << " and rerun the present test to store the expected results. ";
    }
    out << "<table>" << unknownCommands.str() << "</table>";
  }
  if (this->unknown == 0 && this->inconsistencies == 0) {
    out
    << "<b style='color:green'>"
    << "No inconsistencies or unknown computations.</b> ";
  }
  out << "<table>" << goodCommands.str() << "</table>";
  out
  << "<br>Total run time: "
  << global.getElapsedMilliseconds() - this->startTime
  << " ms. ";
  this->reportHtml = out.str();
  return this->inconsistencies == 0 && this->inconsistenciesMathML == 0;
}

int Calculator::getNumberOfBuiltInFunctions() {
  int result = 0;
  for (int i = this->numberOfPredefinedAtoms - 1; i >= 0; i --) {
    MemorySaving<Calculator::OperationHandlers>& current =
    this->operations.values[i];
    if (current.isZeroPointer()) {
      continue;
    }
    result +=
    current.getElement().handlers.size +
    current.getElement().compositeHandlers.size;
  }
  return result;
}
