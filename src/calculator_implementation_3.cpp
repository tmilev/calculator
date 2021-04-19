// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_interface.h"
#include "math_general_implementation.h"
#include "math_extra_Weyl_algebras_implementation.h"
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_extra_modules_semisimple_Lie_algebras_implementation.h"
#include "math_subsets_selections.h"
#include "math_extra_finite_groups_implementation.h"
#include "math_extra_semisimple_Lie_algebras_implementation.h"
#include "string_constants.h"
#include "math_rational_function_implementation.h"

// If you get the C++ multiple definition error:
// - Try moving template *EXPLICIT* specializations (i.e. template <>)
//   to the *beginning* (!) of .cpp files.
// - Try moving template generics into .h files.

template <>
bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::isDominantWithRespectToGenerator<RationalFraction<Rational> >(
  const Vector<RationalFraction<Rational> >& theWeight, int generatorIndex
) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::isDominantWithRespectToGenerator");
  this->checkInitialization();
  Vector<RationalFraction<Rational> > tempVect;
  RationalFraction<Rational> tempRF;
  tempVect = this->simpleRootsInner[generatorIndex].getVectorRational();
  tempRF = this->ambientWeyl->rootScalarCartanRoot(theWeight, tempVect);
  if (tempRF.expressionType != tempRF.typeConstant) {
    global.fatal << "This might or might not be a programming mistake: "
    << "I am being asked whether a weight "
    << "with rational function coefficients is dominant. "
    << "I took the scalar products with the positive simple roots "
    << "whose reflections generate the ambient group, "
    << "however one of the scalar products in question was non-constant. "
    << "More precisely, the scalar product of "
    << theWeight.toString() << " and " << tempVect.toString() << " "
    << "equals " << tempRF.toString() << ". "
    << "I cannot decide (more precisely, do not want to *silently* decide for you) "
    << "whether a non-constant function is to be declared positive or not. "
    << "If this is not a programming mistake, "
    << "you might want to consider introducing a substitution "
    << "evaluating the rational function, some sort of a monomial order, "
    << "or some other method of deciding the \"sign\" of a rational function. "
    << "Whether or not this is a mistake, I am crashing. " << global.fatal;
    return false;
  }
  return !tempRF.constantValue.isNegative();
}

template <>
bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::isDominantWithRespectToGenerator<Rational>(
  const Vector<Rational>& theWeight, int generatorIndex
) {
  this->checkInitialization();
  return !this->ambientWeyl->rootScalarCartanRoot(theWeight, this->simpleRootsInner[generatorIndex]).isNegative();
}

template <>
bool WeylGroupData::isDominantWithRespectToGenerator<RationalFraction<Rational> >(
  const Vector<RationalFraction<Rational> >& theWeight, int generatorIndex
) {
  Vector<Rational> tempVect;
  RationalFraction<Rational> tempRF;
  tempVect.makeEi(this->getDimension(), generatorIndex);
  tempRF = this->rootScalarCartanRoot(theWeight, tempVect);
  if (tempRF.expressionType != tempRF.typeConstant) {
    global.fatal << "This might or might not be a programming mistake: "
    << "I am being asked whether a weight "
    << "with rational function coefficients is dominant. "
    << "I took the scalar products with the positive simple roots "
    << "whose reflections generate the ambient group, "
    << "however one of the scalar products in question was non-constant. "
    << "More precisely, the scalar product of "
    << theWeight.toString() << " and " << tempVect.toString() << " "
    << "equals " << tempRF.toString() << ". "
    << "I cannot decide (more precisely, do not want to *silently* decide for you) "
    << "whether a non-constant function is positive or not. "
    << "If this is not a programming mistake, "
    << "you might want to consider introducing a substitution "
    << "evaluating the rational function, some sort of a monomial order, "
    << "or some other method of deciding the \"sign\" of a rational function. "
    << "Whether or not this is a mistake, I am crashing.  "
    << global.fatal;
    return false;
  }
  return !tempRF.constantValue.isNegative();
}

template <>
bool WeylGroupData::isDominantWithRespectToGenerator<Rational>(const Vector<Rational>& theWeight, int generatorIndex) {
  return !this->getScalarProductSimpleRoot(theWeight, generatorIndex).isNegative();
}

void SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::makeParabolicFromSelectionSimpleRoots(
  WeylGroupData& inputWeyl, const Vector<Rational>& ZeroesMeanSimpleRootSpaceIsInParabolic, int upperLimitNumberOfElements
) {
  Selection tempSel;
  tempSel = ZeroesMeanSimpleRootSpaceIsInParabolic;
  this->makeParabolicFromSelectionSimpleRoots(inputWeyl, tempSel, upperLimitNumberOfElements);
}

bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::getAllDominantWeightsHWFDIMwithRespectToAmbientAlgebra(
  Vector<Rational>& highestWeightSimpleCoords,
  HashedList<Vector<Rational> >& outputWeightsSimpleCoords,
  int upperBoundDominantWeights,
  std::string& outputDetails
) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::getAllDominantWeightsHWFDIMwithRespectToAmbientAlgebra");
  this->checkInitialization();
  std::stringstream out;
  Vector<Rational> highestWeightTrue = highestWeightSimpleCoords;
  Vectors<Rational> basisEi;
  int theDim = this->ambientWeyl->getDimension();
  basisEi.makeEiBasis(theDim);
  this->raiseToDominantWeightInner(highestWeightTrue);
  Vector<Rational> highestWeightFundCoords = this->ambientWeyl->getFundamentalCoordinatesFromSimple(highestWeightTrue);
  if (!highestWeightFundCoords.sumCoordinates().isSmallInteger()) {
    return false;
  }
  int theTopHeightSimpleCoords = static_cast<int>(highestWeightSimpleCoords.sumCoordinates().getDoubleValue()) + 1;
  if (theTopHeightSimpleCoords < 0) {
    theTopHeightSimpleCoords = 0;
  }
  List<HashedList<Vector<Rational> > > outputWeightsByHeight;
  int topHeightRootSystem = this->ambientWeyl->rootsOfBorel.lastObject()->sumCoordinates().numeratorShort;
  int topHeightRootSystemPlusOne = topHeightRootSystem + 1;
  outputWeightsByHeight.setSize(topHeightRootSystemPlusOne);
  int finalHashSize = 100;
  for (int i = 0; i < topHeightRootSystemPlusOne; i ++) {
    outputWeightsByHeight[i].setHashSize(finalHashSize);
  }
  outputWeightsSimpleCoords.clear();
  outputWeightsByHeight[0].addOnTop(highestWeightTrue);
  int numTotalWeightsFound = 0;
  int numPosRoots = this->ambientWeyl->rootsOfBorel.size;
  Vector<Rational> currentWeight, currentWeightRaisedToDominantWRTAmbientAlgebra;
  for (
    int lowestUnexploredHeightDiff = 0;
    lowestUnexploredHeightDiff <= theTopHeightSimpleCoords;
    lowestUnexploredHeightDiff ++
  ) {
    //double startCycleTime = global.getElapsedSeconds();
    if (upperBoundDominantWeights > 0 && numTotalWeightsFound > upperBoundDominantWeights) {
      break;
    }
    int bufferIndexShift = lowestUnexploredHeightDiff % topHeightRootSystemPlusOne;
    HashedList<Vector<Rational> >& currentHashes = outputWeightsByHeight[bufferIndexShift];
    for (int lowest = 0; lowest < currentHashes.size; lowest ++) {
      for (int i = 0; i < numPosRoots; i ++) {
        currentWeight = currentHashes[lowest];
        currentWeight -= this->ambientWeyl->rootsOfBorel[i];
        if (this->isDominantWeight(currentWeight)) {
          currentWeightRaisedToDominantWRTAmbientAlgebra = currentWeight;
          this->ambientWeyl->raiseToDominantWeight(currentWeightRaisedToDominantWRTAmbientAlgebra);
          currentWeightRaisedToDominantWRTAmbientAlgebra -= highestWeightTrue;
          if (currentWeightRaisedToDominantWRTAmbientAlgebra.isNegativeOrZero()) {
            int currentIndexShift = this->ambientWeyl->rootsOfBorel[i].sumCoordinates().numeratorShort;
            currentIndexShift = (currentIndexShift + bufferIndexShift) % topHeightRootSystemPlusOne;
            if (outputWeightsByHeight[currentIndexShift].addOnTopNoRepetition(currentWeight)) {
              numTotalWeightsFound ++;
              outputWeightsByHeight[currentIndexShift].adjustHashes();
            }
          }
        }
      }
    }
    outputWeightsSimpleCoords.addListOnTop(currentHashes);
    outputWeightsSimpleCoords.adjustHashes();
    currentHashes.clear();
  }
  out << " Total number of dominant weights: " << outputWeightsSimpleCoords.size;
  if (numTotalWeightsFound >= upperBoundDominantWeights) {
    out << "<hr>This message is generated either because the number of "
    << "weights has exceeded the hard-coded RAM memory limits, or because "
    << "a priori bound for the number of weights is WRONG. "
    << "If the latter is the case, make sure to send an angry email to the author(s).";
  }
  outputDetails = out.str();
  return (numTotalWeightsFound <= upperBoundDominantWeights);
}

std::string HtmlRoutines::getSliderSpanStartsHidden(
  const std::string& content, const std::string& label, const std::string& desiredID
) {
  (void) label;
  std::stringstream out;
  HtmlRoutines::globalGeneralPurposeID ++;
  std::stringstream idStringStream;
  idStringStream << desiredID;
  if (desiredID == "") {
    idStringStream << "UnnamedSpan" << HtmlRoutines::globalGeneralPurposeID;
  }
  out << "<a href=\"javascript:;\" onmusedown =\"document.getElementById('"
  << idStringStream.str() << "').slideToggle('slow');\">Expand/collapse</a>";
  out << "<span id =\"" << idStringStream.str() << "\" style =\"display:none\">" << content << "</span>";
  return out.str();
}

std::string LittelmannPath::generateOrbitAndAnimate() {
  std::stringstream out;
  List<LittelmannPath> theOrbit;
  List<List<int> > theGens;
  if (!this->generateOrbit(theOrbit, theGens, 1000, nullptr)) {
    out  << "<b>Not all paths were genenerated, only the first " << theOrbit.size << "</b>";
  }
  Vectors<double> coxPlane;
  coxPlane.setSize(2);
  this->owner->getCoxeterPlane(coxPlane[0], coxPlane[1]);
  DrawingVariables animated, collapsed;
  this->owner->drawRootSystem(animated, true, true);
  this->owner->drawRootSystem(collapsed, true, true);
  for (int i = 0; i < theOrbit.size; i ++) {
    LittelmannPath& currentPath = theOrbit[i];
    animated.drawPath(currentPath.waypoints, "black", 1, this->owner->dynkinType.toString(), i);
    collapsed.drawPath(currentPath.waypoints, "black", 1);
    for (int j = 0; j < currentPath.waypoints.size; j ++) {
      animated.drawCircleAtVector(currentPath.waypoints[j], "purple", 3, this->owner->dynkinType.toString(), i);
      collapsed.drawCircleAtVector(currentPath.waypoints[j], "purple", 3);
    }
  }
  out << "<br>Animation of the Littelmann paths follows. ";
  out << animated.getHTMLDiv(this->owner->getDimension(), false);
  out << "<br>Here are all Littelmann paths drawn simultaneously. ";
  out << collapsed.getHTMLDiv(this->owner->getDimension(), false);
  out << "Littelmann paths in simple coordinates given in the order in which they are generated ("
  << theOrbit.size << " total):<br>";
  out << "<table>";
  for (int i = 0; i < theOrbit.size; i ++) {
    LittelmannPath& currentPath = theOrbit[i];
    out << "<tr><td>" << currentPath.toString() << "</td>"
    << "<td>"
    << this->toStringOperatorSequenceStartingOnMe(theGens[i])
    << "</td></tr>";
  }
  out << "</table>";
  LittelmannPath lastPath = theOrbit[0];
  LittelmannPath tempPath;
  MonomialTensor<int, HashFunctions::hashFunction> tempMon;
  tempMon = *theGens.lastObject();
  tempMon.generatorsIndices.reverseElements();
  tempMon.powers.reverseElements();
  out << "<table>";
  for (int i = tempMon.generatorsIndices.size - 1; i >= 1; i --) {
    int curInd = - tempMon.generatorsIndices[i] - 1;
    int nextInd = - tempMon.generatorsIndices[i - 1] - 1;
    for (int k = 0; k < tempMon.powers[i]; k ++) {
      lastPath.actByFAlpha(curInd);
    }
    tempPath = lastPath;
    tempPath.actByEAlpha(nextInd);
    out << "<tr><td> e_" << nextInd + 1 << "(" << lastPath.toString() << ") =</td>" << "<td>"
    << tempPath.toString() << "</td>";
    for (int j = 0; j < this->owner->getDimension(); j ++) {
      tempPath = lastPath;
      tempPath.actByEAlpha(j);
      out << "<td> e_" << j + 1 << "("
      << lastPath.toString() << ")=</td>" << "<td>" << tempPath.toString() << "</td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  out << "<table><td>corresponding element of U(g)</td><td>is adapted</td>"
  << "<td>path</td><td>e operators with non-zero action.</td>";
  for (int i = 0; i < theGens.size; i ++) {
    tempPath = theOrbit[i];
    tempMon = theGens[i];
    tempMon.generatorsIndices.reverseElements();
    tempMon.powers.reverseElements();
    bool isadapted = tempPath.isAdaptedString(tempMon);
    out << "<tr><td>" << tempMon.toString() << "</td><td>"
    << (isadapted ? "is adapted to" : "is not adapted to" ) << "</td><td>"
    << tempPath.toString() << "</td><td>";
    for (int j = 0; j < this->owner->getDimension(); j ++) {
      tempPath = theOrbit[i];
      tempPath.actByEFDisplayIndex(j + 1);
      if (!tempPath.isEqualToZero()) {
        out << "e_{" << j + 1 << "}, ";
      }
      tempPath = theOrbit[i];
      tempPath.actByEFDisplayIndex(- j - 1);
      if (!tempPath.isEqualToZero()) {
        out << "e_{" << - j - 1 << "}, ";
      }
    }
    out << "</td></tr>";
  }
  out << "</table>";
  return out.str();
}

void Calculator::makeHmmG2InB3(HomomorphismSemisimpleLieAlgebra& output) {
  MacroRegisterFunctionWithName("Calculator::makeHmmG2InB3");
  DynkinType b3Type, g2Type;
  b3Type.makeSimpleType('B', 3);
  g2Type.makeSimpleType('G', 2);
  output.domainAlg = &this->objectContainer.getLieAlgebraCreateIfNotPresent(g2Type);
  output.rangeAlg = &this->objectContainer.getLieAlgebraCreateIfNotPresent(b3Type);

  output.theRange().computeChevalleyConstants();
  output.theDomain().computeChevalleyConstants();
  ElementSemisimpleLieAlgebra<Rational> g_2, g_1plusg_3, g_m2, g_m1plusg_m3, tempElt;
  g_2.makeGenerator         (13, output.theRange());
  g_m2.makeGenerator        (7,  output.theRange());
  g_1plusg_3.makeGenerator  (12, output.theRange());
  tempElt.makeGenerator     (14, output.theRange());
  g_1plusg_3 += tempElt;
  g_m1plusg_m3.makeGenerator(6, output.theRange());
  tempElt.makeGenerator     (8, output.theRange());
  g_m1plusg_m3 += tempElt;
  output.imagesSimpleChevalleyGenerators.setSize(4);
  output.imagesSimpleChevalleyGenerators[0] = g_1plusg_3;
  output.imagesSimpleChevalleyGenerators[1] = g_2;
  output.imagesSimpleChevalleyGenerators[3] = g_m2;
  output.imagesSimpleChevalleyGenerators[2] = g_m1plusg_m3;
  output.computeHomomorphismFromImagesSimpleChevalleyGenerators();
  output.getRestrictionAmbientRootSystemToTheSmallercartanSubalgebra(output.RestrictedRootSystem);
}

void BranchingData::resetOutputData() {
  this->theEigenVectorS.setSize(0);
  this->theEigenVectorsLevi.setSize(0);
  this->outputEigenWords.setSize(0);
  this->g2Weights.setSize(0);
  this->outputWeightsFundCoordS.setSize(0);
  this->theCharacterDifferences.clear();
}

void WeylGroupData::getHighestWeightsAllRepresentationsDimensionLessThanOrEqualTo(
  List<Vector<Rational> >& outputHighestWeightsFundCoords, int inputDimBound
) {
  if (inputDimBound < 1) {
    outputHighestWeightsFundCoords.setSize(0);
    return;
  }
  HashedList<Vector<Rational> > output;
  Vector<Rational> current;
  current.makeZero(this->getDimension());
  output.addOnTop(current);
  Rational theDim;
  Rational dimBound = inputDimBound + 1;
  for (int i = 0; i < output.size; i ++) {
    current = output[i];
    for (int k = 0; k < this->getDimension(); k ++) {
      current[k] += 1;
      theDim = this->weylDimFormulaFundamentalCoords(current);
      if (theDim < dimBound) {
        output.addOnTopNoRepetition(current);
      }
      current[k] -= 1;
    }
  }
  outputHighestWeightsFundCoords = output;
}

bool Expression::assignMatrixExpressions(
  const Matrix<Expression>& input,
  Calculator& owner,
  bool reduceOneRowToSequenceAndOneByOneToNonMatrix,
  bool dontReduceTypes
) {
  MacroRegisterFunctionWithName("Expression::assignMatrixExpressions");
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
  Expression theMatType(owner);
  theMatType.addChildAtomOnTop(owner.opMatrix());
  this->addChildOnTop(theMatType);
  enum matrixType {typeUnknown, typeRat, typeDouble, typeAlgebraic, typePolyRat, typePolyAlg, typeRF, typeExpression};
  matrixType outType = typeUnknown;
  Expression currentRow;
  for (int i = 0; i < input.numberOfRows; i ++) {
    currentRow.reset(owner);
    currentRow.children.reserve(input.numberOfColumns + 1);
    currentRow.addChildAtomOnTop(owner.opSequence());
    for (int j = 0; j < input.numberOfColumns; j ++) {
      currentRow.addChildOnTop(input(i, j));
      matrixType inType;
      if (input(i, j).isOfType<Rational>()) {
        inType = typeRat;
      } else if (input(i, j).isOfType<AlgebraicNumber>()) {
        inType = typeAlgebraic;
      } else if (input(i, j).isOfType<double>()) {
        inType = typeDouble;
      } else if (input(i, j).isOfType<Polynomial<Rational> >()) {
        inType = typePolyRat;
      } else if (input(i, j).isOfType<Polynomial<AlgebraicNumber> >()) {
        inType = typeAlgebraic;
      } else if (input(i, j).isOfType<RationalFraction<Rational> >()) {
        inType = typeRF;
      } else {
        inType = typeExpression;
      }
      if (outType == typeUnknown) {
        outType = inType;
      }
      if (inType == typeAlgebraic) {
        if (outType == typeRat) {
          outType = inType;
          continue;
        }
        if (outType == typeDouble || outType == typeAlgebraic || outType == typePolyAlg) {
          continue;
        }
        outType = typeExpression;
        continue;
      } else if (inType == typeDouble) {
        if (outType == typeRat || outType == typeAlgebraic || outType == typeDouble) {
          outType = inType;
          continue;
        }
        outType = typeExpression;
        continue;
      } else if (inType == typePolyRat) {
        if (outType == typeRat) {
          outType = inType;
          continue;
        }
        if (outType == typeAlgebraic || outType == typePolyAlg) {
          outType = typePolyAlg;
          continue;
        }
        outType = typeExpression;
        continue;
      } else if (inType == typePolyAlg) {
        if (outType == typeRat || outType == typeAlgebraic || outType == typePolyRat || outType == typePolyAlg) {
          outType = inType;
          continue;
        }
        outType = typeExpression;
        continue;
      } else if (inType == typeRF) {
        if (outType == typeRat || outType == typePolyRat) {
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
  switch(outType) {
    case typeRat:
      theMatType.addChildAtomOnTop(owner.opRational());
      break;
    case typeDouble:
      theMatType.addChildAtomOnTop(owner.opDouble());
      break;
    case typeAlgebraic:
      theMatType.addChildAtomOnTop(owner.opAlgebraicNumber());
      break;
    case typePolyRat:
      theMatType.addChildAtomOnTop(owner.opPolynomialRational());
      break;
    case typePolyAlg:
      theMatType.addChildAtomOnTop(owner.opPolynomialAlgebraicNumbers());
      break;
    case typeRF:
      theMatType.addChildAtomOnTop(owner.opRationalFunction());
      break;
    default:
      break;
  }
  if (outType != typeUnknown && outType != typeExpression && !dontReduceTypes) {
    this->setChild(0, theMatType);
  }
  return true;
}

bool Calculator::getMatrixExpressionsFromArguments(
  const Expression& input, Matrix<Expression>& output, int desiredNumRows, int desiredNumCols
) {
  MacroRegisterFunctionWithName("Calculator::getMatrixExpressionsFromArguments");
  if (!input.isList()) {
    return false;
  }
  Expression inputModified = input;
  inputModified.setChildAtomValue(0, this->opSequence());
  return this->getMatrixExpressions(inputModified, output, desiredNumRows, desiredNumCols);
}

bool Calculator::getMatrixExpressions(
  const Expression& input, Matrix<Expression>& output, int desiredNumberOfRows, int desiredNumberOfColumns
) {
  MacroRegisterFunctionWithName("Calculator::getMatrixExpressions");
  if (!input.isSequenceNElements() && !input.isMatrix() && !input.isIntervalRealLine()) {
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
  if (!input[1].isSequenceNElements() && !input[1].startsWith(this->opIntervalOpen())) {
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

bool Calculator::Test::processOneTest(JSData& input) {
  if (input["input"].elementType != JSData::token::tokenString) {
    global << Logger::red << "Input command is missing. " << Logger::endL;
    return false;
  }
  std::string command = input["input"].stringValue;
  if (!this->commands.contains(command)) {
    std::stringstream reportStream;
    reportStream << "Command [" << command
    << "] not recognized. "
    << "If the testing commands have recently changed, this is OK, "
    << "otherwise, it isn't.";
    global << Logger::red << reportStream.str() << Logger::endL;
    return false;
  }
  if (input["output"].elementType != JSData::token::tokenString) {
    global << Logger::red << "Command: " << command
    << " is missing its expected output. " << Logger::endL;
    return false;
  }
  Calculator::Test::OneTest& currentTest = this->commands.getValueCreate(command);
  currentTest.expectedResult = input["output"].stringValue;
  return true;
}

bool Calculator::Test::loadTestStrings(
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Calculator::loadTestStrings");
  if (!FileOperations::fileExistsVirtual(
    WebAPI::calculator::testFileNameVirtual, false, false, nullptr
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Test json file does not exist. ";
    }
    this->flagTestResultsExist = false;
    return false;
  }
  std::string testStrings;
  if (!FileOperations::loadFileToStringVirtual(
    WebAPI::calculator::testFileNameVirtual,
    testStrings,
    false,
    commentsOnFailure
  )) {
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
  if (this->storedResults.elementType != JSData::token::tokenArray) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Test json is not an array. ";
    }
    return false;
  }
  for (int i = 0; i < this->storedResults.listObjects.size; i ++) {
    this->processOneTest(this->storedResults.listObjects[i]);
  }
  return true;
}

std::string Calculator::writeFileToOutputFolderReturnLink(
  const std::string& fileContent, const std::string& fileName, const std::string& linkText
) {
  MacroRegisterFunctionWithName("Calculator::writeFileToOutputFolderReturnLink");
  std::string fileNameVirtual = "output/" + fileName;
  return FileOperations::writeFileReturnHTMLLink(fileContent, fileNameVirtual, linkText);
}

bool Calculator::Test::writeTestStrings(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("Calculator::writeTestStrings");
  JSData result;
  result.elementType = JSData::token::tokenArray;
  result.listObjects.setSize(this->commands.size());
  for (int i = 0; i < this->commands.size(); i ++) {
    JSData nextEntry;
    nextEntry["input"] = this->commands.keys[i];
    Calculator::Test::OneTest& output = this->commands.values[i];
    nextEntry["output"] = output.actualResult;
    result.listObjects[i] = nextEntry;
  }
  return FileOperations::writeFileVirual(
    WebAPI::calculator::testFileNameVirtual,
    result.toString(&JSData::PrintOptions::newLine()),
    commentsOnFailure
  );
}

Calculator::Test::Test(Calculator& inputOwner) {
  this->startIndex = 0;
  this->startTime = global.getElapsedMilliseconds();
  this->inconsistencies = 0;
  this->unknown = 0;
  this->noTestSkips = 0;
  this->numberOfTests = 0;
  this->lastIndexNotTested = 0;
  this->owner = &inputOwner;
  this->flagTestResultsExist = true;
}

bool Calculator::innerAutomatedTest(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerAutomatedTest");
  if (!global.userDefaultHasAdminRights()) {
    return calculator << "Automated test requires administrator access";
  }
  if (input.size() != 3) {
    return calculator << "Automated test expects two arguments: "
    << "index of first test to run and number of tests to run. ";
  }
  global.millisecondsMaxComputation = 30000000; //30k seconds, ok as we have administrator access
  Calculator::Test test(calculator);
  if (
    !input[1].isSmallInteger(&test.startIndex) ||
    !input[2].isSmallInteger(&test.numberOfTests)
  ) {
    return calculator
    << "Automated test takes two arguments: "
    << "index of the first test to run and total "
    << "number of tests to run after that. ";
  }
  test.calculatorTestRun();
  return output.assignValue(test.reportHtml, calculator);
}

bool Calculator::Test::processResults() {
  std::stringstream commentsOnFailure, out;

  if (!this->loadTestStrings(&commentsOnFailure)) {
    global << Logger::red << "Failed to load test strings. " << Logger::endL
    << commentsOnFailure.str();
    out << "<b style='color:red'>Failed to load test strings. </b>" << commentsOnFailure.str();
  }
  if (!this->flagTestResultsExist) {
    out << "<b style='color:green'>Writing new test strings into: "
    << WebAPI::calculator::testFileNameVirtual << ". </b>";
    std::stringstream commentsOnFailure2;
    if (!this->writeTestStrings(&commentsOnFailure2)) {
      global << Logger::red << "Failed to write test strings. " << Logger::endL
      << commentsOnFailure2.str();
      out << "<b style='color:red'>Write file failed. </b>" << commentsOnFailure2.str();
    }
  }
  std::stringstream goodCommands, unknownCommands, badCommands, badCommandsConsole;
  this->inconsistencies = 0;
  if (
    this->startIndex > 0 ||
    this->lastIndexNotTested < this->commands.size()
  ) {
    out << "<b style='color:red'>Only " << this->numberOfTests
    << " out of " << this->commands.size() << " processed. </b>";
  }
  for (int i = this->startIndex; i < this->lastIndexNotTested; i ++) {
    Calculator::Test::OneTest& currentTest = this->commands.values[i];
    std::stringstream currentLine, currentLineConsole;
    currentLine << "<tr>";
    currentLine << "<td style = 'min-width:25px;'>" << i << "</td>";
    currentLineConsole << "Test " << i << "\n";
    currentLine << "<td style = 'min-width:200px;'>" << currentTest.functionAdditionalIdentifier
    << "</td>";
    currentLineConsole << "Function " << currentTest.functionAdditionalIdentifier << ", atom: " << currentTest.atom << "\n";
    currentLine << "<td style = 'min-width:45px;'>" << currentTest.atom << "</td>";
    currentLineConsole << "Ran:\n" << this->commands.keys[i] << "\n";
    currentLine << "<td style = 'min-width:200px;'>"
    << HtmlRoutines::getCalculatorComputationAnchorThisServer(
      this->commands.keys[i], this->commands.keys[i]
    ) << "</td>";
    bool isBad = false;
    bool isUknown = false;
    if (currentTest.actualResult == currentTest.expectedResult) {
      currentLine << "<td style = 'min-width:30px;'><b style='color:green;'>OK</b></td>";
    } else if (currentTest.expectedResult == "") {
      currentLine << "<td><b style='color:orange;'>expected result unknown</b></td>";
      isUknown = true;
      this->unknown ++;
    } else {
      StringRoutines::Differ theDiffer;
      theDiffer.left = HtmlRoutines::convertStringToHtmlString(currentTest.actualResult, false);
      theDiffer.right = HtmlRoutines::convertStringToHtmlString(currentTest.expectedResult, false);
      currentLine << "<td style = 'min-width:100px;'><b style='color:red'>unexpected result</b></td>"
      << "<td class = 'cellCalculatorResult'>";
      currentLine << theDiffer.differenceHTML("actual", "expected");
      currentLine << "</td>";
      currentLineConsole << "Got:\n" << currentTest.actualResult << "\n";
      currentLineConsole << "Expected:\n" << currentTest.expectedResult << "\n";
      isBad = true;
      this->inconsistencies ++;
    }
    currentLine << "</tr>";
    if (isBad) {
      badCommands << currentLine.str();
      badCommandsConsole << currentLineConsole.str();
    } else if (isUknown) {
      unknownCommands << currentLine.str();
    } else {
      goodCommands << currentLine.str();
    }
  }
  if (this->noTestSkips > 0) {
    out << this->noTestSkips << " functions were not tested. "
    << "The kinds of functions not auto-tested are "
    << "described in the comments of class Function::Options. ";
  }
  if (this->inconsistencies > 0) {
    out << "<b style = 'color:red'>"
    << "The test file results do not match the current results. </b>"
    << "There were " << this->inconsistencies << " inconsistencies. "
    << "If you think the current computations are correct, "
    << "say, the expected results have changed since the last test run, "
    << "erase file: "
    << WebAPI::calculator::testFileNameVirtual
    << " and rerun the present test to store the expected results. ";
    out << "<table class = 'tableCalculatorOutput'>" << badCommands.str() << "</table>";
    global << Logger::red << "There were "
    << this->inconsistencies << " inconsistencies. " << Logger::endL;
    global << badCommandsConsole.str() << Logger::endL;
  }
  if (this->unknown > 0) {
    out << "<b style = 'color:orange'>There were " << this->unknown
    << " commands with no previous recorded results. </b>";
    global << Logger::orange << "There were " << this->unknown
    << " commands with no previous recorded results." << Logger::endL;
    if (this->flagTestResultsExist) {
      out
      << "<b>Erase file " << WebAPI::calculator::testFileNameVirtual
      << " and rerun the present test to store the expected results.</b>";
      global << Logger::yellow << "Erase file " << WebAPI::calculator::testFileNameVirtual
      << " and rerun the present test to store the expected results. ";
    }
    out << "<table>" << unknownCommands.str() << "</table>";
  }
  if (this->unknown == 0 && this->inconsistencies == 0) {
    out << "<b style ='color:green'>No inconsistencies or uknown computations.</b> ";
  }
  out << "<table>" << goodCommands.str() << "</table>";
  out << "<br>Total run time: " << global.getElapsedMilliseconds() - this->startTime << " ms. ";
  this->reportHtml = out.str();
  return this->inconsistencies == 0;
}

int Calculator::getNumberOfBuiltInFunctions() {
  int result = 0;
  for (int i = this->numberOfPredefinedAtoms - 1; i >= 0; i --) {
    MemorySaving<Calculator::OperationHandlers>& current = this->operations.values[i];
    if (current.isZeroPointer()) {
      continue;
    }
    result += current.getElement().handlers.size + current.getElement().compositeHandlers.size;
  }
  return result;
}
