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
bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::isDominantWithRespectToGenerator<RationalFunction<Rational> >(
  const Vector<RationalFunction<Rational> >& theWeight, int generatorIndex
) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::isDominantWithRespectToGenerator");
  this->checkInitialization();
  Vector<RationalFunction<Rational> > tempVect;
  RationalFunction<Rational> tempRF;
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
bool WeylGroupData::isDominantWithRespectToGenerator<RationalFunction<Rational> >(
  const Vector<RationalFunction<Rational> >& theWeight, int generatorIndex
) {
  Vector<Rational> tempVect;
  RationalFunction<Rational> tempRF;
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

bool Calculator::innerAnimateLittelmannPaths(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerAnimateLittelmannPaths");
  RecursionDepthCounter recursionCounter(&calculator.recursionDepth);
  if (!input.isListNElements(3)) {
    return output.makeError("This function takes 2 arguments", calculator);
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!calculator.convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, algebra
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  SemisimpleLieAlgebra* theSSowner = algebra.content;
  Vector<Rational> theWeight;
  ExpressionContext tempContext(calculator);
  if (!calculator.getVector<Rational>(
    input[2],
    theWeight,
    &tempContext,
    theSSowner->getRank(),
    nullptr
  )) {
    return output.makeError(
      "Failed to convert the argument of the function to a highest weight vector",
      calculator
    );
  }
  Vector<Rational> theWeightInSimpleCoords;
  theWeightInSimpleCoords = theSSowner->theWeyl.getSimpleCoordinatesFromFundamental(theWeight);
  calculator << "<br>Function innerAnimateLittelmannPaths: your input in simple coords: "
  << theWeightInSimpleCoords.toString();
  LittelmannPath thePath;
  thePath.makeFromWeightInSimpleCoords(theWeightInSimpleCoords, theSSowner->theWeyl);
  return output.assignValue(thePath.generateOrbitAndAnimate(), calculator);
}

std::string HtmlRoutines::getSliderSpanStartsHidden(
  const std::string& content, const std::string& label, const std::string& desiredID
) {
  (void) label;
  std::stringstream out;
  HtmlRoutines::GlobalGeneralPurposeID ++;
  std::stringstream idStringStream;
  idStringStream << desiredID;
  if (desiredID == "") {
    idStringStream << "UnnamedSpan" << HtmlRoutines::GlobalGeneralPurposeID;
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
    animated.drawPath(currentPath.Waypoints, "black", 1, this->owner->theDynkinType.toString(), i);
    collapsed.drawPath(currentPath.Waypoints, "black", 1);
    for (int j = 0; j < currentPath.Waypoints.size; j ++) {
      animated.drawCircleAtVector(currentPath.Waypoints[j], "purple", 3, this->owner->theDynkinType.toString(), i);
      collapsed.drawCircleAtVector(currentPath.Waypoints[j], "purple", 3);
    }
  }
  out << "<br>Animation of the Littelmann paths follows. ";
  out << animated.getHTMLDiv(this->owner->getDimension());
  out << "<br>Here are all Littelmann paths drawn simultaneously. ";
  out << collapsed.getHTMLDiv(this->owner->getDimension());
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
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> tempMon;
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

template<class Coefficient>
void ModuleSSalgebra<Coefficient>::splitFDpartOverFKLeviRedSubalg(
  HomomorphismSemisimpleLieAlgebra& theHmm,
  Selection& LeviInSmall,
  List<ElementUniversalEnveloping<Coefficient> >* outputEigenVectors,
  Vectors<Coefficient>* outputWeightsFundCoords,
  Vectors<Coefficient>* outputEigenSpace,
  std::stringstream* comments
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<Coefficient>::splitFDpartOverFKLeviRedSubalg");
  if (this->theChaR.size() != 1) {
    if (comments != nullptr) {
      std::stringstream out;
      out << "I have been instructed only to split modules that are irreducible over the ambient Lie algebra";
      out << " Instead I got the character " << this->theChaR.toString()
      << " (" << this->theChaR.size() << " monomials)";
      *comments << out.str();
    }
    return;
  }
  std::string tempS;
  std::stringstream out;
  if (comments != nullptr) {
    out << tempS;
  }
  out << "<br>Parabolic selection: " << LeviInSmall.toString();
  std::stringstream tempStream1;
  tempStream1 << "Started splitting the f.d. part of the " << theHmm.theRange().toStringLieAlgebraName() << "-module with highest weight in fund coords "
  << this->theChaR[0].weightFundamentalCoordS.toString();
  ProgressReport theReport;
  theReport.report(tempStream1.str());
  List<List<Vector<Coefficient> > > eigenSpacesPerSimpleGenerator;
  Selection InvertedLeviInSmall;
  InvertedLeviInSmall = LeviInSmall;
  InvertedLeviInSmall.invertSelection();
  eigenSpacesPerSimpleGenerator.setSize(InvertedLeviInSmall.cardinalitySelection);
  Vectors<Coefficient> tempSpace1, tempSpace2;
  MemorySaving<Vectors<Coefficient> > tempEigenVects;
  Vectors<Coefficient>& theFinalEigenSpace = (outputEigenSpace == nullptr) ? tempEigenVects.getElement() : *outputEigenSpace;
  theFinalEigenSpace.setSize(0);
  if (InvertedLeviInSmall.cardinalitySelection == 0) {
    theFinalEigenSpace.makeEiBasis(this->getDimension());
  }
  for (int i = 0; i < InvertedLeviInSmall.cardinalitySelection; i ++) {
    ElementSemisimpleLieAlgebra<Rational>& currentElt =
    theHmm.imagesSimpleChevalleyGenerators[InvertedLeviInSmall.elements[i]];
    MatrixTensor<Coefficient> currentOp, tempMat;
    currentOp.makeZero();
    for (int j = 0; j < currentElt.size(); j ++) {
      tempMat = this->getActionGeneratorIndex(currentElt[j].generatorIndex);
      tempMat *= currentElt.coefficients[j];
      currentOp += tempMat;
    }
    std::stringstream tempStream3;
    double timeAtStart1 = global.getElapsedSeconds();
    tempStream3 << "Computing eigenspace corresponding to " << currentElt.toString() << "...";
    theReport.report(tempStream3.str());
    Matrix<Coefficient> currentOpMat;
    currentOp.getMatrix(currentOpMat, this->getDimension());
    currentOpMat.getZeroEigenSpace(eigenSpacesPerSimpleGenerator[i]);
    tempStream3 << " done in " << global.getElapsedSeconds() - timeAtStart1 << " seconds. ";
    theReport.report(tempStream3.str());
    if (i == 0) {
      theFinalEigenSpace = eigenSpacesPerSimpleGenerator[i];
    } else {
      std::stringstream tempStream4;
      double timeAtStart2 = global.getElapsedSeconds();
      tempStream4 << "Intersecting with eigenspace corresponding to " << currentElt.toString() << "...";
      tempSpace1 = theFinalEigenSpace;
      theReport.report(tempStream4.str());
      tempSpace2 = eigenSpacesPerSimpleGenerator[i];
      theFinalEigenSpace.intersectTwoLinearSpaces(tempSpace1, tempSpace2, theFinalEigenSpace);
      tempStream4 << " done in " << global.getElapsedSeconds() - timeAtStart2 << " seconds. ";
      theReport.report(tempStream4.str());
    }
  }
  out << "<br>Eigenvectors: <table>";
  std::stringstream readyForLatexComsumption;
  readyForLatexComsumption << "\\begin{tabular}{|lll|}\n <br>";
  readyForLatexComsumption << "\\hline\\multicolumn{3}{|c|}{Highest weight $"
  << this->theHWFundamentalCoordsBaseField.toStringLetterFormat("\\omega") << "$}\\\\\n<br>";
  readyForLatexComsumption << "weight fund. coord.& singular vector \\\\\\hline\n<br>";
  Vector<Coefficient> currentWeight;
  Vector<Coefficient> hwFundCoordsNilPart;
  hwFundCoordsNilPart = this->theHWFundamentalCoordsBaseField;
  hwFundCoordsNilPart -= this->theHWFDpartFundamentalCoordS;
  ElementUniversalEnveloping<Coefficient> currentElt, tempElt;
  if (outputEigenVectors != nullptr) {
    outputEigenVectors->setSize(0);
  }
  for (int j = 0; j < theFinalEigenSpace.size; j ++) {
    out << "<tr><td>";
    currentElt.makeZero(this->getOwner());
    Vector<Coefficient>& currentVect = theFinalEigenSpace[j];
    int lastNonZeroIndex = - 1;
    for (int i = 0; i < currentVect.size; i ++) {
      if (!(currentVect[i].isEqualToZero())) {
        tempElt.makeZero(this->getOwner());
        tempElt.addMonomial(this->theGeneratingWordsNonReduced[i], 1);
        tempElt *= currentVect[i];
        currentElt += tempElt;
        lastNonZeroIndex = i;
      }
    }
    currentWeight = theHmm.theRange().theWeyl.getFundamentalCoordinatesFromSimple(
      this->theGeneratingWordsWeightsPlusWeightFDpart[lastNonZeroIndex]
    );//<-implicit type conversion here
    currentWeight += hwFundCoordsNilPart;
    readyForLatexComsumption <<  "$" << currentWeight.toStringLetterFormat("\\omega") << "$&$" << currentVect.toStringLetterFormat("m") << "$";
    if (currentElt.size() > 1) {
      out << "(";
    }
    if (outputEigenVectors != nullptr) {
      outputEigenVectors->addOnTop(currentElt);
    }
    if (outputWeightsFundCoords != nullptr) {
      outputWeightsFundCoords->addOnTop(currentWeight);
    }
    out << currentElt.toString(&global.theDefaultFormat.getElement());
    if (currentElt.size() > 1) {
      out << ")";
    }
    out << " v_\\lambda";
    out << "</td><td>(weight: " << currentWeight.toStringLetterFormat("\\omega") << ")</td></tr>";
    readyForLatexComsumption << "\\\\\n<br>";
  }
  out << "</table>";
  readyForLatexComsumption << "\\hline \n<br> \\end{tabular}";
  out << "<br>Your ready for LaTeX consumption text follows.<br>";
  out << readyForLatexComsumption.str();
  theReport.report("splitFDpartOverFKLeviRedSubalg done!");
  if (comments != nullptr) {
    *comments << out.str();
  }
}

void Calculator::makeHmmG2InB3(HomomorphismSemisimpleLieAlgebra& output) {
  MacroRegisterFunctionWithName("Calculator::makeHmmG2InB3");
  DynkinType b3Type, g2Type;
  b3Type.makeSimpleType('B', 3);
  g2Type.makeSimpleType('G', 2);
  output.domainAlg = &this->theObjectContainer.getLieAlgebraCreateIfNotPresent(g2Type);
  output.rangeAlg = &this->theObjectContainer.getLieAlgebraCreateIfNotPresent(b3Type);

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

template<class Coefficient>
bool ElementSumGeneralizedVermas<Coefficient>::extractElementUniversalEnveloping(
  ElementUniversalEnveloping<Coefficient>& output, SemisimpleLieAlgebra& theOwner
) {
  output.makeZero(theOwner);
  ModuleSSalgebra<Coefficient>* theModPtr = nullptr;
  MonomialUniversalEnveloping<Coefficient> tempMon;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialGeneralizedVerma<Coefficient>& currentMon = (*this)[i];
    if (i == 0) {
      theModPtr = currentMon.owner;
    }
    if (currentMon.owner != theModPtr) {
      return false;
    }
    tempMon = currentMon.theMonCoeffOne;
    tempMon *= currentMon.getOwner().theGeneratingWordsNonReduced[currentMon.indexFDVector];
    output.addMonomial(tempMon, this->coefficients[i]);
  }
  return true;
}

bool Calculator::innerSplitFDpartB3overG2inner(Calculator& calculator, BranchingData& theG2B3Data, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerSplitFDpartB3overG2inner");
  ModuleSSalgebra<RationalFunction<Rational> > theModCopy;
  theModCopy.makeFromHW(
    theG2B3Data.theHmm.theRange(), theG2B3Data.theWeightFundCoords, theG2B3Data.selInducing, 1, 0, nullptr, false
  );
  theG2B3Data.resetOutputData();
  theG2B3Data.initAssumingParSelAndHmmInitted();
  theG2B3Data.SelSplittingParSel = theG2B3Data.selInducing;
  if (theG2B3Data.SelSplittingParSel.selected[0] != theG2B3Data.SelSplittingParSel.selected[2]) {
    theG2B3Data.SelSplittingParSel.addSelectionAppendNewIndex(0);
    theG2B3Data.SelSplittingParSel.addSelectionAppendNewIndex(2);
  }
  Vector<Rational> splittingParSel;
  splittingParSel = theG2B3Data.SelSplittingParSel;

  calculator.theObjectContainer.theCategoryOmodules.addNoRepetitionOrReturnIndexFirst(theModCopy);
  int theModIndex = calculator.theObjectContainer.theCategoryOmodules.getIndex(theModCopy);
  ModuleSSalgebra<RationalFunction<Rational> >& theMod = calculator.theObjectContainer.theCategoryOmodules[theModIndex];
  theMod.getOwner().flagHasNilradicalOrder = true;
  std::stringstream out;
  calculator << "<hr>Time elapsed before making B3 irrep: " << global.getElapsedSeconds();
  double timeAtStart = global.getElapsedSeconds();
  theMod.splitFDpartOverFKLeviRedSubalg(
    theG2B3Data.theHmm,
    theG2B3Data.selSmallParSel,
    &theG2B3Data.outputEigenWords,
    &theG2B3Data.outputWeightsFundCoordS,
    &theG2B3Data.leviEigenSpace,
    nullptr
  );
  calculator << "<br>Time needed to make B3 irrep: " << global.getElapsedSeconds() - timeAtStart;
  theG2B3Data.g2Weights.setSize(theG2B3Data.outputWeightsFundCoordS.size);
  theG2B3Data.g2DualWeights.setSize(theG2B3Data.outputWeightsFundCoordS.size);
  Matrix<Rational> invertedG2cartanMat;
  invertedG2cartanMat = theG2B3Data.theHmm.theDomain().theWeyl.cartanSymmetric;
  invertedG2cartanMat.invert();
  WeylGroupData& rangeWeyl = theG2B3Data.theHmm.theRange().theWeyl;
  theG2B3Data.outputWeightsSimpleCoords = rangeWeyl.getSimpleCoordinatesFromFundamental(theG2B3Data.outputWeightsFundCoordS);
  Vector<RationalFunction<Rational> > weightSimpleCoordinates;
  weightSimpleCoordinates = rangeWeyl.getSimpleCoordinatesFromFundamental(
    theG2B3Data.theWeightFundCoords
  );
  theG2B3Data.theAmbientChar.makeFromWeight(weightSimpleCoordinates, &theG2B3Data.theHmm.theRange());
  theG2B3Data.theSmallCharFDpart.makeZero();
  CharacterSemisimpleLieAlgebraModule<RationalFunction<Rational> > tempMon;
  for (int i = 0; i < theG2B3Data.outputWeightsSimpleCoords.size; i ++) {
    Vector<RationalFunction<Rational> >& currentWeight = theG2B3Data.outputWeightsSimpleCoords[i];
    Vector<RationalFunction<Rational> >& currentG2Weight = theG2B3Data.g2Weights[i];
    Vector<RationalFunction<Rational> >& currentG2DualWeight = theG2B3Data.g2DualWeights[i];
    currentG2DualWeight.setSize(2);
    currentG2DualWeight[0] = theG2B3Data.theHmm.theRange().theWeyl.rootScalarCartanRoot(
      currentWeight, theG2B3Data.theHmm.ImagesCartanDomain[0]
    );
    //<-note: implicit type conversion: the return type is the left coefficient type.
    currentG2DualWeight[1] = theG2B3Data.theHmm.theRange().theWeyl.rootScalarCartanRoot(
      currentWeight, theG2B3Data.theHmm.ImagesCartanDomain[1]
    );
    //<-note: implicit type conversion: the return type is the left coefficient type.
    invertedG2cartanMat.actOnVectorColumn(currentG2DualWeight, currentG2Weight);//<-g2weight is now computed;
    tempMon.makeFromWeight(currentG2Weight, &theG2B3Data.theHmm.theDomain());
    theG2B3Data.theSmallCharFDpart += tempMon;
  }
  ElementUniversalEnveloping<RationalFunction<Rational> > theG2Casimir, theG2CasimirCopy, imageCasimirInB3, tempElt;
  theG2Casimir.makeCasimir(theG2B3Data.theHmm.theDomain());

  theG2B3Data.theChars.setSize(theG2B3Data.outputWeightsFundCoordS.size);
  for (int i = 0; i < theG2B3Data.outputWeightsSimpleCoords.size; i ++) {
    Vector<RationalFunction<Rational> >& currentG2DualWeight = theG2B3Data.g2DualWeights[i];
    theG2CasimirCopy = theG2Casimir;
    theG2CasimirCopy.modOutVermaRelations(&currentG2DualWeight, 1, 0);
    if (theG2CasimirCopy.isEqualToZero()) {
      theG2B3Data.theChars[i] = 0;
    } else {
      theG2B3Data.theChars[i] = theG2CasimirCopy.coefficients[0];
    }
  }
  theG2B3Data.theEigenVectorsLevi.setSize(theG2B3Data.g2Weights.size);
  theG2B3Data.theEigenVectorS.setSize(theG2B3Data.g2Weights.size);
  theG2B3Data.additionalMultipliers.setSize(theG2B3Data.g2Weights.size);
  theG2B3Data.theShapovalovProducts.setSize(theG2B3Data.g2Weights.size);
  theG2B3Data.theUEelts.setSize(theG2B3Data.g2Weights.size);
  ElementSumGeneralizedVermas<RationalFunction<Rational> >& theHWV = *theG2B3Data.theEigenVectorsLevi.lastObject();
  theHWV.makeHWV(theMod, 1);
  theHWV *= - 1;
  *theG2B3Data.theEigenVectorS.lastObject() = theHWV;
  Vector<RationalFunction<Rational> > weightDifference;
  theG2B3Data.theHmm.applyHomomorphism(theG2Casimir, imageCasimirInB3);
  theG2Casimir.checkConsistency();
  imageCasimirInB3.checkConsistency();
  RationalFunction<Rational> charDiff;
  theG2B3Data.theHmm.theRange().orderNilradical(
    theMod.parabolicSelectionNonSelectedAreElementsLevi,
    theG2B3Data.flagUseNilWeightGeneratorOrder,
    theG2B3Data.flagAscendingGeneratorOrder
  );
  for (int k = 0; k < theG2B3Data.g2Weights.size; k ++) {
    ElementSumGeneralizedVermas<RationalFunction<Rational> >& currentTensorEltLevi = theG2B3Data.theEigenVectorsLevi[k];
    ElementSumGeneralizedVermas<RationalFunction<Rational> >& currentTensorEltEigen = theG2B3Data.theEigenVectorS[k];
    ElementUniversalEnveloping<RationalFunction<Rational> >& currentUEelt = theG2B3Data.theUEelts[k];
    currentTensorEltLevi = theHWV;
    currentTensorEltLevi.multiplyMeByUEEltOnTheLeft(theG2B3Data.outputEigenWords[k]);
    currentTensorEltEigen = currentTensorEltLevi;
    if (theG2B3Data.selInducing.cardinalitySelection > 0) {
      for (int j = 0; j < theG2B3Data.g2Weights.size; j ++) {
        weightDifference = theG2B3Data.g2Weights[j] - theG2B3Data.g2Weights[k];
        if (weightDifference.isPositive()) {
          theG2CasimirCopy = imageCasimirInB3;
          tempElt.makeConstant(theG2B3Data.theChars[j], theG2B3Data.theHmm.theRange());
          theG2CasimirCopy -= tempElt;
          theG2CasimirCopy *= 12;
          currentTensorEltEigen.multiplyMeByUEEltOnTheLeft(theG2CasimirCopy);
          charDiff = theG2B3Data.theChars[j];
          charDiff -= *theG2B3Data.theChars.lastObject();
          theG2B3Data.theCharacterDifferences.addOnTopNoRepetition(charDiff);
        }
      }
    }
    RationalFunction<Rational> scale = currentTensorEltEigen.scaleNormalizeLeadingMonomial(nullptr);
    if (!scale.isConstant(&theG2B3Data.additionalMultipliers[k])) {
      global.fatal << "This is unexpected: the scale is not a constant. " << global.fatal;
    }
    currentTensorEltEigen.extractElementUniversalEnveloping(currentUEelt, *theMod.owner);
    currentUEelt.highestWeightTransposeAntiAutomorphismBilinearForm(
      currentUEelt, theG2B3Data.theShapovalovProducts[k], &theMod.theHWDualCoordsBaseFielD, 1, 0, nullptr
    );
  }
  return output.assignValue(out.str(), calculator);
}

bool Calculator::innerJacobiSymbol(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerJacobiSymbol");
  global.fatal << "Function not implemented yet." << global.fatal;
  (void) calculator;
  (void) output;
  if (input.children.size != 3) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  int leftInt, rightInt;
  if (!leftE.isSmallInteger(&leftInt) || !rightE.isSmallInteger(&rightInt)) {
    return false;
  }
  return true;
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

bool Calculator::innerTestMonomialBaseConjecture(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerTestMonomialBaseConjecture");
  RecursionDepthCounter theRecursion(&calculator.recursionDepth);
  if (!input.isListNElements(3)) {
    return output.makeError("innerTestMonomialBaseConjecture takes two arguments as input", calculator);
  }
  const Expression& rankE = input[1];
  const Expression& dimE = input[2];
  int rankBound = 0;
  int dimBound = 0;
  if (!rankE.isSmallInteger(&rankBound) || !dimE.isSmallInteger(&dimBound)) {
    return output.makeError("The rank and  dim bounds must be small integers", calculator);
  }
  if (rankBound < 2 || rankBound > 100 || dimBound < 1 || dimBound > 10000) {
    return output.makeError(
      "The rank bound must be an integer between 2 and 100, "
      "and the dim bound must be an integer between 1 and 10000. ",
      calculator
    );
  }
  std::stringstream out;
  List<int> theRanks;
  List<char> theWeylLetters;
  for (int i = 2; i <= rankBound; i ++) {
    theRanks.addOnTop(i);
    theWeylLetters.addOnTop('A');
    theRanks.addOnTop(i);
    theWeylLetters.addOnTop('B');
    theRanks.addOnTop(i);
    theWeylLetters.addOnTop('C');
    if (i >= 4) {
      theRanks.addOnTop(i);
      theWeylLetters.addOnTop('D');
    }
    if (i >= 6 && i <= 8) {
      theRanks.addOnTop(i);
      theWeylLetters.addOnTop('E');
    }
    if (i == 4) {
      theRanks.addOnTop(i);
      theWeylLetters.addOnTop('F');
    }
    if (i == 2) {
      theRanks.addOnTop(i);
      theWeylLetters.addOnTop('G');
    }
  }
  List<List<Vector<Rational> > > theHighestWeights;
  theHighestWeights.setSize(theRanks.size);
  bool foundBad = false;
  Selection tempSel;
  std::stringstream latexReport;
  latexReport << "\\documentclass{article} <br>\\usepackage{longtable}\\begin{document}<br>\n\n\n\n\n";
  latexReport << " \\begin{longtable}{|lllll|} ";
  ProgressReport theReport;
  bool ConjectureBholds = true;
  bool ConjectureCholds = true;
  LittelmannPath hwPath;
  List<LittelmannPath> tempList;
  List<List<int> > theStrings;
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> tempMon;
  DynkinType currentType;
  for (int i = 0; i < theRanks.size; i ++) {
    currentType.makeSimpleType(theWeylLetters[i], theRanks[i]);
    SemisimpleLieAlgebra& currentAlg =
    calculator.theObjectContainer.getLieAlgebraCreateIfNotPresent(currentType);
    currentAlg.computeChevalleyConstants();
    currentAlg.theWeyl.getHighestWeightsAllRepresentationsDimensionLessThanOrEqualTo(theHighestWeights[i], dimBound);
    latexReport << "\\hline\\multicolumn{5}{c}{" << "$" << currentAlg.toStringLieAlgebraName() << "$}\\\\\\hline\n\n"
    << "$\\lambda$ & dim &\\# pairs 1& \\# pairs total  & \\# Arithmetic op.  \\\\\\hline";
    out << "<br>" << " <table><tr><td  border =\"1\" colspan =\"3\">"
    << theWeylLetters[i] << "_{" << theRanks[i] << "}" << "</td></tr> <tr><td>highest weight</td><td>dim</td></tr>";
    List<Vector<Rational> >& theHws = theHighestWeights[i];
    tempSel.initialize(theRanks[i]);
    for (int j = 0; j < theHws.size; j ++) {
      std::stringstream reportStream;
      Vector<Rational>& currentHW = theHws[j];
      out << "<tr><td> " << currentHW.toString() << "</td><td>"
      << currentAlg.theWeyl.weylDimFormulaFundamentalCoords(currentHW) << "</td>";
      reportStream << "Processing " << currentAlg.toStringLieAlgebraName() << ", index  "
      << i + 1 << " out of " << theRanks.size << ",  highest weight "
      << currentHW.toString() << ", dim: " << currentAlg.theWeyl.weylDimFormulaFundamentalCoords(currentHW)
      << ", index " << j + 1 << " out of " << theHws.size;
      theReport.report(reportStream.str());
      latexReport << "$" << currentHW.toStringLetterFormat("\\omega") << "$ &"
      << currentAlg.theWeyl.weylDimFormulaFundamentalCoords(currentHW) << "&";
      hwPath.makeFromWeightInSimpleCoords(
        currentAlg.theWeyl.getSimpleCoordinatesFromFundamental(currentHW), currentAlg.theWeyl
      );
      hwPath.generateOrbit(
        tempList,
        theStrings,
        MathRoutines::minimum(1000, currentAlg.theWeyl.weylDimFormulaFundamentalCoords(currentHW).numeratorShort),
        nullptr
      );
      reportStream << "\nPath orbit size = " << theStrings.size
      << " generated in " << global.getElapsedSeconds() << " seconds. ";
      theReport.report(reportStream.str());
      for (int k = 0; k < theStrings.size; k ++) {
        LittelmannPath& currentPath = tempList[k];
        tempMon = theStrings[k];
        tempMon.generatorsIndices.reverseElements();
        tempMon.powers.reverseElements();
        if (!currentPath.isAdaptedString(tempMon)) {
          foundBad = true;
          break;
        }
      }
      if (!foundBad) {
        out << "<td>all strings adapted</td>";
      } else {
        out << "<td>has non-adapted string</td>";
      }
/*      if (theMod.makeFromHW
          (calculator.theObjectContainer.theLieAlgebras, i,
           currentHW, tempSel, 1, 0, 0, true)) {
        out << "<td>is good</td>";
        if (!theMod.flagConjectureBholds) {
          out << "<td><b>conjecture B fails!</b></td>";
          ConjectureBholds = false;
        }
        if (!theMod.flagConjectureCholds) {
          out << "<td><b>conjecture C holds</b></td>";
          ConjectureCholds = false;
        }
        if (theMod.NumCachedPairsBeforeSimpleGen>= theMod.MaxNumCachedPairs)
          latexReport << "$ \\geq$ " << theMod.MaxNumCachedPairs;
        else
          latexReport << theMod.NumCachedPairsBeforeSimpleGen;
        latexReport
        << "&" << theMod.cachedPairs.size << " & "
        << Rational::totalLargeAdditions+Rational::totalSmallAdditions
        +Rational::totalLargeMultiplications+Rational::totalSmallMultiplications -
        startRatOps;
      }
      else {
        latexReport << " & \\textbf{BAD}";
        out << "<td><b>Is bad!!!!</b></td>";
        theReport.report("BAD BAD BAD!!!");
        foundBad = true;
        break;
      }*/
      out << "</tr>";
      if (foundBad) {
        break;
      }
      latexReport << "\\\\";
    }
    out << "</table>";
    if (foundBad) {
      break;
    }
  }
  latexReport << "\\end{longtable} \n\n\n\n";
  if (ConjectureBholds) {
    latexReport << " Conjecture B holds for all computed entries.";
  } else {
    latexReport << "Conjecture B fails.";
  }
  if (ConjectureCholds) {
    latexReport << "Conjecture C holds for all computed entries.";
  } else {
    latexReport << "Conjecture C fails.";
  }
  latexReport << "\\end{document}";
  out << "<br><br>\n\n\n\n\n" << latexReport.str();
  return output.assignValue(out.str(), calculator);
}

bool Calculator::innerLittelmannOperator(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerLittelmannOperator");
  RecursionDepthCounter theRecursionIncrementer(&calculator.recursionDepth);
  if (input.hasBoundVariables()) {
    return false;
  }
  int theIndex = 0;
  if (!input.isSmallInteger(&theIndex)) {
    return output.makeError(
      "The argument of the Littelmann root operator is "
      "expected to be a small integer, instead you gave me " +
      input.toString(),
      calculator
    );
  }
  if (theIndex == 0) {
    return output.makeError("The index of the Littelmann root operator is expected to be non-zero", calculator);
  }
  return output.assignValue(theIndex, calculator);
}

bool Calculator::innerLSPath(Calculator& calculator, const Expression& input, Expression& output) {
  RecursionDepthCounter theRecutionIncrementer(&calculator.recursionDepth);
  MacroRegisterFunctionWithName("Calculator::innerLSPath");
  if (input.size() < 3) {
    return output.makeError("LSPath needs at least two arguments.", calculator);
  }
  WithContext<SemisimpleLieAlgebra*> theSSowner;
  if (!calculator.convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, theSSowner
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  SemisimpleLieAlgebra& ownerSSalgebra = *theSSowner.content;
  Vectors<Rational> waypoints;
  waypoints.setSize(input.children.size - 2);
  for (int i = 2; i < input.children.size; i ++) {
    if (!calculator.getVector<Rational>(
      input[i], waypoints[i - 2], nullptr, ownerSSalgebra.getRank(), nullptr
    )) {
      return output.makeError("Failed to extract waypoints", calculator);
    }
  }
  waypoints = ownerSSalgebra.theWeyl.getSimpleCoordinatesFromFundamental(waypoints);
  LittelmannPath theLSpath;
  theLSpath.makeFromWaypoints(waypoints, ownerSSalgebra.theWeyl);
  return output.assignValue(theLSpath, calculator);
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
  theMatType.addChildAtomOnTop(owner.opMatriX());
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
      } else if (input(i, j).isOfType<RationalFunction<Rational> >()) {
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
      theMatType.addChildAtomOnTop(owner.opAlgNumber());
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
  const Expression& input, Matrix<Expression>& output, int desiredNumRows, int desiredNumCols
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
    if (desiredNumRows > 0) {
      if (desiredNumRows != 1) {
        return false;
      }
    }
    if (desiredNumCols > 0) {
      if (desiredNumCols != input.size() - 1) {
        return false;
      }
    }
    output.initialize(1, input.size() - 1);
    for (int i = 1; i < input.size(); i ++) {
      output(0, i - 1) = input[i];
    }
    return true;
  }
  if (desiredNumRows > 0) {
    if (desiredNumRows != input.size() - 1) {
      return false;
    }
  }
  if (desiredNumCols > 0) {
    if (desiredNumCols != input[1].size() - 1) {
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
  if (input["input"].theType != JSData::token::tokenString) {
    global << Logger::red << "Input command is missing. " << Logger::endL;
    return false;
  }
  std::string command = input["input"].theString;
  if (!this->commands.contains(command)) {
    std::stringstream reportStream;
    reportStream << "Command [" << command
    << "] not recognized. "
    << "If the testing commands have recently changed, this is OK, "
    << "otherwise, it isn't.";
    global << Logger::red << reportStream.str() << Logger::endL;
    return false;
  }
  if (input["output"].theType != JSData::token::tokenString) {
    global << Logger::red << "Command: " << command
    << " is missing its expected output. " << Logger::endL;
    return false;
  }
  Calculator::Test::OneTest& currentTest = this->commands.getValueCreate(command);
  currentTest.expectedResult = input["output"].theString;
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
  if (!FileOperations::loadFiletoStringVirtual(
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
  if (!this->storedResults.readstring(testStrings, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to parse test json. ";
    }
    return false;
  }
  if (this->storedResults.theType != JSData::token::tokenArray) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Test json is not an array. ";
    }
    return false;
  }
  for (int i = 0; i < this->storedResults.theList.size; i ++) {
    this->processOneTest(this->storedResults.theList[i]);
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
  result.theType = JSData::token::tokenArray;
  result.theList.setSize(this->commands.size());
  for (int i = 0; i < this->commands.size(); i ++) {
    JSData nextEntry;
    nextEntry["input"] = this->commands.theKeys[i];
    Calculator::Test::OneTest& output = this->commands.theValues[i];
    nextEntry["output"] = output.actualResult;
    result.theList[i] = nextEntry;
  }
  return FileOperations::writeFileVirual(
    WebAPI::calculator::testFileNameVirtual,
    result.toString(&JSData::PrintOptions::NewLine()),
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
    Calculator::Test::OneTest& currentTest = this->commands.theValues[i];
    std::stringstream currentLine, currentLineConsole;
    currentLine << "<tr>";
    currentLine << "<td style = 'min-width:25px;'>" << i << "</td>";
    currentLineConsole << "Test " << i << "\n";
    currentLine << "<td style = 'min-width:200px;'>" << currentTest.functionAdditionalIdentifier
    << "</td>";
    currentLineConsole << "Function " << currentTest.functionAdditionalIdentifier << ", atom: " << currentTest.atom << "\n";
    currentLine << "<td style = 'min-width:45px;'>" << currentTest.atom << "</td>";
    currentLineConsole << "Ran:\n" << this->commands.theKeys[i] << "\n";
    currentLine << "<td style = 'min-width:200px;'>"
    << HtmlRoutines::getCalculatorComputationNewPage(
      this->commands.theKeys[i], this->commands.theKeys[i]
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
    MemorySaving<Calculator::OperationHandlers>& current = this->operations.theValues[i];
    if (current.isZeroPointer()) {
      continue;
    }
    result += current.getElement().handlers.size + current.getElement().compositeHandlers.size;
  }
  return result;
}
