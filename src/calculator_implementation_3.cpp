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

// If you get the C++ multiple definition error:
// - Try moving template *EXPLICIT* specializations (i.e. template <>)
//   to the *beginning* (!) of .cpp files.
// - Try moving template generics into .h files.

template <>
bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::IsDominantWRTgenerator<RationalFunction>(
  const Vector<RationalFunction>& theWeight, int generatorIndex
) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::IsDominantWRTgenerator");
  this->checkInitialization();
  Vector<RationalFunction> tempVect;
  RationalFunction tempRF;
  tempVect = this->simpleRootsInner[generatorIndex].GetVectorRational();
  tempRF = this->AmbientWeyl->RootScalarCartanRoot(theWeight, tempVect);
  if (tempRF.expressionType != tempRF.typeRational) {
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
  return !tempRF.ratValue.isNegative();
}

template <>
bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::IsDominantWRTgenerator<Rational>(
  const Vector<Rational>& theWeight, int generatorIndex
) {
  this->checkInitialization();
  return !this->AmbientWeyl->RootScalarCartanRoot(theWeight, this->simpleRootsInner[generatorIndex]).isNegative();
}

template <>
bool WeylGroupData::IsDominantWRTgenerator<RationalFunction>(
  const Vector<RationalFunction>& theWeight, int generatorIndex
) {
  Vector<Rational> tempVect;
  RationalFunction tempRF;
  tempVect.makeEi(this->getDimension(), generatorIndex);
  tempRF = this->RootScalarCartanRoot(theWeight, tempVect);
  if (tempRF.expressionType != tempRF.typeRational) {
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
  return !tempRF.ratValue.isNegative();
}

template <>
bool WeylGroupData::IsDominantWRTgenerator<Rational>(const Vector<Rational>& theWeight, int generatorIndex) {
  return !this->GetScalarProdSimpleRoot(theWeight, generatorIndex).isNegative();
}

void SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::MakeParabolicFromSelectionSimpleRoots(
  WeylGroupData& inputWeyl, const Vector<Rational>& ZeroesMeanSimpleRootSpaceIsInParabolic, int UpperLimitNumElements
) {
  Selection tempSel;
  tempSel = ZeroesMeanSimpleRootSpaceIsInParabolic;
  this->MakeParabolicFromSelectionSimpleRoots(inputWeyl, tempSel, UpperLimitNumElements);
}

bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::GetAlLDominantWeightsHWFDIMwithRespectToAmbientAlgebra(
  Vector<Rational>& highestWeightSimpleCoords,
  HashedList<Vector<Rational> >& outputWeightsSimpleCoords,
  int upperBoundDominantWeights,
  std::string& outputDetails
) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::GetAlLDominantWeightsHWFDIMwithRespectToAmbientAlgebra");
  this->checkInitialization();
  std::stringstream out;
  Vector<Rational> highestWeightTrue = highestWeightSimpleCoords;
  Vectors<Rational> basisEi;
  int theDim= this->AmbientWeyl->getDimension();
  basisEi.MakeEiBasis(theDim);
  this->RaiseToDominantWeightInner(highestWeightTrue);
  Vector<Rational> highestWeightFundCoords = this->AmbientWeyl->GetFundamentalCoordinatesFromSimple(highestWeightTrue);
  if (!highestWeightFundCoords.SumCoords().isSmallInteger()) {
    return false;
  }
  int theTopHeightSimpleCoords = static_cast<int>(highestWeightSimpleCoords.SumCoords().GetDoubleValue()) + 1;
  if (theTopHeightSimpleCoords < 0) {
    theTopHeightSimpleCoords = 0;
  }
  List<HashedList<Vector<Rational> > > outputWeightsByHeight;
  int topHeightRootSystem = this->AmbientWeyl->RootsOfBorel.lastObject()->SumCoords().numeratorShort;
  int topHeightRootSystemPlusOne = topHeightRootSystem + 1;
  outputWeightsByHeight.setSize(topHeightRootSystemPlusOne);
  int finalHashSize = 100;
  for (int i = 0; i < topHeightRootSystemPlusOne; i ++) {
    outputWeightsByHeight[i].setHashSize(finalHashSize);
  }
  outputWeightsSimpleCoords.clear();
  outputWeightsByHeight[0].addOnTop(highestWeightTrue);
  int numTotalWeightsFound = 0;
  int numPosRoots = this->AmbientWeyl->RootsOfBorel.size;
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
        currentWeight -= this->AmbientWeyl->RootsOfBorel[i];
        if (this->IsDominantWeight(currentWeight)) {
          currentWeightRaisedToDominantWRTAmbientAlgebra = currentWeight;
          this->AmbientWeyl->RaiseToDominantWeight(currentWeightRaisedToDominantWRTAmbientAlgebra);
          currentWeightRaisedToDominantWRTAmbientAlgebra -= highestWeightTrue;
          if (currentWeightRaisedToDominantWRTAmbientAlgebra.isNegativeOrZero()) {
            int currentIndexShift = this->AmbientWeyl->RootsOfBorel[i].SumCoords().numeratorShort;
            currentIndexShift = (currentIndexShift + bufferIndexShift) % topHeightRootSystemPlusOne;
            if (outputWeightsByHeight[currentIndexShift].addOnTopNoRepetition(currentWeight)) {
              numTotalWeightsFound ++;
              outputWeightsByHeight[currentIndexShift].adjustHashes();
            }
          }
        }
      }
    }
    outputWeightsSimpleCoords.addOnTop(currentHashes);
    outputWeightsSimpleCoords.adjustHashes();
    currentHashes.clear();
  }
  out << " Total number of dominant weights: " << outputWeightsSimpleCoords.size;
  if (numTotalWeightsFound >= upperBoundDominantWeights) {
    out << "<hr>This message is generated either because the number of "
    << "weights has exceeded the hard-coded RAM memory limits, or because "
    << " a priori bound for the number of weights is WRONG. "
    << "If the latter is the case, make sure to send an angry email to the author(s).";
  }
  outputDetails = out.str();
  return (numTotalWeightsFound <= upperBoundDominantWeights);
}

bool Calculator::innerAnimateLittelmannPaths(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerAnimateLittelmannPaths");
  RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  if (!input.isListNElements(3)) {
    return output.makeError("This function takes 2 arguments", theCommands);
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!theCommands.Convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, algebra
  )) {
    return output.makeError("Error extracting Lie algebra.", theCommands);
  }
  SemisimpleLieAlgebra* theSSowner = algebra.content;
  Vector<Rational> theWeight;
  ExpressionContext tempContext(theCommands);
  if (!theCommands.getVector<Rational>(
    input[2],
    theWeight,
    &tempContext,
    theSSowner->GetRank(),
    nullptr
  )) {
    return output.makeError(
      "Failed to convert the argument of the function to a highest weight vector",
      theCommands
    );
  }
  Vector<Rational> theWeightInSimpleCoords;
  theWeightInSimpleCoords = theSSowner->theWeyl.GetSimpleCoordinatesFromFundamental(theWeight);
  theCommands << "<br>Function innerAnimateLittelmannPaths: your input in simple coords: "
  << theWeightInSimpleCoords.toString();
  LittelmannPath thePath;
  thePath.MakeFromWeightInSimpleCoords(theWeightInSimpleCoords, theSSowner->theWeyl);
  return output.assignValue(thePath.GenerateOrbitAndAnimate(), theCommands);
}

bool Calculator::innerCasimir(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerCasimir");
  if (input.size() != 2) {
    return theCommands << "Casimir function expects a single input. ";
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!theCommands.Convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, algebra
  )) {
    return output.makeError("Error extracting Lie algebra.", theCommands);
  }
  SemisimpleLieAlgebra& algebraReference = *algebra.content;
  ElementUniversalEnveloping<RationalFunction> theCasimir;
  theCasimir.MakeCasimir(algebraReference);
  theCommands << "Context Lie algebra: " << algebraReference.theWeyl.theDynkinType.toString()
  << ". The coefficient: " << algebraReference.theWeyl.GetKillingDivTraceRatio().toString()
  <<  ". The Casimir element of the ambient Lie algebra. ";
  ExpressionContext context(theCommands);
  context.setAmbientSemisimpleLieAlgebra(algebraReference);
  return output.assignValueWithContext(theCasimir, context, theCommands);
}

bool Calculator::innerEmbedG2inB3(Calculator& theCommands, const Expression& input, Expression& output) {
  if (input.size() != 2) {
    return false;
  }

  output = input[1];
  if (!output.isOfType < ElementUniversalEnveloping<RationalFunction> >()) {
    return output.makeError("Failed to convert argument to element of the Universal enveloping algebra. ", theCommands);
  }
  SemisimpleLieAlgebra& ownerSS = *output.GetAmbientSSAlgebraNonConstUseWithCaution();
  if (!ownerSS.IsOfSimpleType('G', 2)) {
    return output.makeError("Error: embedding of G_2 in B_3 takes elements of U(G_2) as arguments.", theCommands);
  }
  HomomorphismSemisimpleLieAlgebra theHmm;
  theCommands.MakeHmmG2InB3(theHmm);

  ElementUniversalEnveloping<RationalFunction> argument = output.getValue<ElementUniversalEnveloping<RationalFunction> >();
  ElementUniversalEnveloping<RationalFunction> outputUE;
  if (!theHmm.applyHomomorphism(argument, outputUE)) {
    return output.makeError("Failed to apply homomorphism for unspecified reason", theCommands);
  }
  outputUE.simplify();
  ExpressionContext context(theCommands);
  context.setAmbientSemisimpleLieAlgebra(theHmm.theRange());
  return output.assignValueWithContext(outputUE, context, theCommands);
}

std::string HtmlRoutines::GetSliderSpanStartsHidden(
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

std::string LittelmannPath::GenerateOrbitAndAnimate() {
  std::stringstream out;
  List<LittelmannPath> theOrbit;
  List<List<int> > theGens;
  if (!this->generateOrbit(theOrbit, theGens, 1000, nullptr)) {
    out  << "<b>Not all paths were genenerated, only the first " << theOrbit.size << "</b>";
  }
  Vectors<double> coxPlane;
  coxPlane.setSize(2);
  this->owner->GetCoxeterPlane(coxPlane[0], coxPlane[1]);
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
  out << animated.GetHtmlDiv(this->owner->getDimension());
  out << "<br>Here are all Littelmann paths drawn simultaneously. ";
  out << collapsed.GetHtmlDiv(this->owner->getDimension());
  out << "Littelmann paths in simple coordinates given in the order in which they are generated ("
  << theOrbit.size << " total):<br>";
  out << "<table>";
  for (int i = 0; i < theOrbit.size; i ++) {
    LittelmannPath& currentPath = theOrbit[i];
    out << "<tr><td>" << currentPath.toString() << "</td>"
    << "<td>"
    << this->ElementToStringOperatorSequenceStartingOnMe(theGens[i])
    << "</td></tr>";
  }
  out << "</table>";
  LittelmannPath lastPath = theOrbit[0];
  LittelmannPath tempPath;
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> tempMon;
  tempMon = *theGens.lastObject();
  tempMon.generatorsIndices.reverseElements();
  tempMon.Powers.reverseElements();
  out << "<table>";
  for (int i = tempMon.generatorsIndices.size - 1; i >= 1; i --) {
    int curInd = - tempMon.generatorsIndices[i] - 1;
    int nextInd = - tempMon.generatorsIndices[i - 1] - 1;
    for (int k = 0; k < tempMon.Powers[i]; k ++) {
      lastPath.ActByFalpha(curInd);
    }
    tempPath = lastPath;
    tempPath.ActByEalpha(nextInd);
    out << "<tr><td> e_" << nextInd + 1 << "(" << lastPath.toString() << ") =</td>" << "<td>"
    << tempPath.toString() << "</td>";
    for (int j = 0; j < this->owner->getDimension(); j ++) {
      tempPath = lastPath;
      tempPath.ActByEalpha(j);
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
    tempMon.Powers.reverseElements();
    bool isadapted = tempPath.IsAdaptedString(tempMon);
    out << "<tr><td>" << tempMon.toString() << "</td><td>"
    << (isadapted ? "is adapted to" : "is not adapted to" ) << "</td><td>"
    << tempPath.toString() << "</td><td>";
    for (int j = 0; j < this->owner->getDimension(); j ++) {
      tempPath = theOrbit[i];
      tempPath.ActByEFDisplayIndex(j + 1);
      if (!tempPath.isEqualToZero()) {
        out << "e_{" << j + 1 << "}, ";
      }
      tempPath = theOrbit[i];
      tempPath.ActByEFDisplayIndex(- j - 1);
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
void ModuleSSalgebra<Coefficient>::SplitFDpartOverFKLeviRedSubalg(
  HomomorphismSemisimpleLieAlgebra& theHmm,
  Selection& LeviInSmall,
  List<ElementUniversalEnveloping<Coefficient> >* outputEigenVectors,
  Vectors<Coefficient>* outputWeightsFundCoords,
  Vectors<Coefficient>* outputEigenSpace,
  std::stringstream* comments
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<Coefficient>::SplitFDpartOverFKLeviRedSubalg");
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
  tempStream1 << "Started splitting the f.d. part of the " << theHmm.theRange().ToStringLieAlgebraName() << "-module with highest weight in fund coords "
  << this->theChaR[0].weightFundamentalCoordS.toString();
  ProgressReport theReport;
  theReport.report(tempStream1.str());
  List<List<Vector<Coefficient> > > eigenSpacesPerSimpleGenerator;
  Selection InvertedLeviInSmall;
  InvertedLeviInSmall = LeviInSmall;
  InvertedLeviInSmall.InvertSelection();
  eigenSpacesPerSimpleGenerator.setSize(InvertedLeviInSmall.CardinalitySelection);
  Vectors<Coefficient> tempSpace1, tempSpace2;
  MemorySaving<Vectors<Coefficient> > tempEigenVects;
  Vectors<Coefficient>& theFinalEigenSpace = (outputEigenSpace == nullptr) ? tempEigenVects.getElement() : *outputEigenSpace;
  theFinalEigenSpace.setSize(0);
  if (InvertedLeviInSmall.CardinalitySelection == 0) {
    theFinalEigenSpace.MakeEiBasis(this->getDimension());
  }
  for (int i = 0; i < InvertedLeviInSmall.CardinalitySelection; i ++) {
    ElementSemisimpleLieAlgebra<Rational>& currentElt =
    theHmm.imagesSimpleChevalleyGenerators[InvertedLeviInSmall.elements[i]];
    MatrixTensor<Coefficient> currentOp, tempMat;
    currentOp.makeZero();
    for (int j = 0; j < currentElt.size(); j ++) {
      tempMat = this->GetActionGeneratorIndeX(currentElt[j].theGeneratorIndex);
      tempMat *= currentElt.coefficients[j];
      currentOp += tempMat;
    }
    std::stringstream tempStream3;
    double timeAtStart1 = global.getElapsedSeconds();
    tempStream3 << "Computing eigenspace corresponding to " << currentElt.toString() << "...";
    theReport.report(tempStream3.str());
    Matrix<Coefficient> currentOpMat;
    currentOp.GetMatrix(currentOpMat, this->getDimension());
    currentOpMat.GetZeroEigenSpace(eigenSpacesPerSimpleGenerator[i]);
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
      theFinalEigenSpace.IntersectTwoLinSpaces(tempSpace1, tempSpace2, theFinalEigenSpace);
      tempStream4 << " done in " << global.getElapsedSeconds() - timeAtStart2 << " seconds. ";
      theReport.report(tempStream4.str());
    }
  }
  out << "<br>Eigenvectors: <table>";
  std::stringstream readyForLatexComsumption;
  readyForLatexComsumption << "\\begin{tabular}{|lll|}\n <br>";
  readyForLatexComsumption << "\\hline\\multicolumn{3}{|c|}{Highest weight $"
  << this->theHWFundamentalCoordsBaseField.ToStringLetterFormat("\\omega") << "$}\\\\\n<br>";
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
    currentElt.makeZero(this->GetOwner());
    Vector<Coefficient>& currentVect = theFinalEigenSpace[j];
    int lastNonZeroIndex = - 1;
    for (int i = 0; i < currentVect.size; i ++) {
      if (!(currentVect[i].isEqualToZero())) {
        tempElt.makeZero(this->GetOwner());
        tempElt.addMonomial(this->theGeneratingWordsNonReduced[i], 1);
        tempElt *= currentVect[i];
        currentElt += tempElt;
        lastNonZeroIndex = i;
      }
    }
    currentWeight = theHmm.theRange().theWeyl.GetFundamentalCoordinatesFromSimple(
      this->theGeneratingWordsWeightsPlusWeightFDpart[lastNonZeroIndex]
    );//<-implicit type conversion here
    currentWeight += hwFundCoordsNilPart;
    readyForLatexComsumption <<  "$" << currentWeight.ToStringLetterFormat("\\omega") << "$&$" << currentVect.ToStringLetterFormat("m") << "$";
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
    out << "</td><td>(weight: " << currentWeight.ToStringLetterFormat("\\omega") << ")</td></tr>";
    readyForLatexComsumption << "\\\\\n<br>";
  }
  out << "</table>";
  readyForLatexComsumption << "\\hline \n<br> \\end{tabular}";
  out << "<br>Your ready for LaTeX consumption text follows.<br>";
  out << readyForLatexComsumption.str();
  theReport.report("SplitFDpartOverFKLeviRedSubalg done!");
  if (comments != nullptr) {
    *comments << out.str();
  }
}

void Calculator::MakeHmmG2InB3(HomomorphismSemisimpleLieAlgebra& output) {
  MacroRegisterFunctionWithName("Calculator::MakeHmmG2InB3");
  DynkinType b3Type, g2Type;
  b3Type.MakeSimpleType('B', 3);
  g2Type.MakeSimpleType('G', 2);
  output.domainAlg = &this->theObjectContainer.GetLieAlgebraCreateIfNotPresent(g2Type);
  output.rangeAlg = &this->theObjectContainer.GetLieAlgebraCreateIfNotPresent(b3Type);

  output.theRange().ComputeChevalleyConstants();
  output.theDomain().ComputeChevalleyConstants();
  ElementSemisimpleLieAlgebra<Rational> g_2, g_1plusg_3, g_m2, g_m1plusg_m3, tempElt;
  g_2.MakeGenerator         (13, output.theRange());
  g_m2.MakeGenerator        (7,  output.theRange());
  g_1plusg_3.MakeGenerator  (12, output.theRange());
  tempElt.MakeGenerator     (14, output.theRange());
  g_1plusg_3 += tempElt;
  g_m1plusg_m3.MakeGenerator(6, output.theRange());
  tempElt.MakeGenerator     (8, output.theRange());
  g_m1plusg_m3 += tempElt;
  output.imagesSimpleChevalleyGenerators.setSize(4);
  output.imagesSimpleChevalleyGenerators[0] = g_1plusg_3;
  output.imagesSimpleChevalleyGenerators[1] = g_2;
  output.imagesSimpleChevalleyGenerators[3] = g_m2;
  output.imagesSimpleChevalleyGenerators[2] = g_m1plusg_m3;
  output.ComputeHomomorphismFromImagesSimpleChevalleyGenerators();
  output.GetRestrictionAmbientRootSystemToTheSmallerCartanSA(output.RestrictedRootSystem);
}

bool Calculator::innerPrintB3G2branchingIntermediate(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  Vectors<RationalFunction>& theHWs,
  BranchingData& theG2B3Data,
  ExpressionContext& theContext
) {
  MacroRegisterFunctionWithName("Calculator::innerPrintB3G2branchingIntermediate");
  (void) input;
  std::stringstream out, timeReport;
  std::stringstream latexTable, latexTable2;
  bool isFD = (theG2B3Data.selInducing.CardinalitySelection == 0);
  if (isFD) {
    out << "<table border =\"1\"><tr><td>$so(7)$-highest weight</td><td>Decomposition over $G_2$</td>"
    <<  "<td>$G_2\\cap b$-eigenvectors </td></tr>";
    latexTable << "\\begin{longtable}{|ccccl|} \\caption{\\label{tableB3fdsOverG2charsAndHWV} "
    << " Decompositions of finite dimensional $so(7)$-modules over $G_2$}\\\\"
    << "\\hline so(7)& dim. &$G_2$&dim.& $\\mathfrak b \\cap G_2$-singular vectors\\\\ \\hline"
    << "\\endhead \n<br>";
  } else {
    out << "Let " << HtmlRoutines::GetMathSpanPure("p\\subset so(7)") << " be the "
    << theG2B3Data.selInducing.toString() << "-parabolic subalgebra"
    << " and let " << HtmlRoutines::GetMathSpanPure("{p}'= p\\cap G_2")
    << ". Then  " << HtmlRoutines::GetMathSpanPure("{p}'") << " is the "
    << theG2B3Data.selSmallParSel.toString() << "- parabolic subalgebra of G_2"
    << "<br> <table border =\"1\"><tr><td>$so(7)$-highest weight</td>"
    << "<td>character difference from top</td>"
    << "<td>Decomposition of inducing module over "
    << HtmlRoutines::GetMathSpanPure("p'")
    << "</td><td>" << HtmlRoutines::GetMathSpanPure("p'\\cap b")
    << "-eigenvectors</td><td>Casimir projector</td><td>Extra multiplier</td><td>corresponding "
    << HtmlRoutines::GetMathSpanPure("G_2\\cap b")
    << "-eigenvectors</td><td>Shapovalov square</td></tr>";
    latexTable << "\\begin{longtable}{|cccclll|} \\caption{\\label{tableB3fdsOverG2charsAndHWV"
    << theG2B3Data.selInducing.toString() << "} "
    << "Decomposition of inducing $" << theG2B3Data.selInducing.toString() << "$-$\\mathfrak p$-module over $"
    << theG2B3Data.selSmallParSel.toString() << "$-$\\mathfrak p'=\\mathfrak p\\cap G_2$}\\\\"
    << "\\hline $\\mathfrak p-inducing$& dim. &$\\mathfrak p' decomp. $&dim.&"
    << " $\\mathfrak b \\cap \\mathfrak p'$-singular vectors & Casimir projector "
    << "& Corresp. $\\mathfrak b \\cap G_2$-singular vectors  \\\\ \\hline"
    << "\\endhead \n<br>";
  }
  theContext.getFormat(theG2B3Data.theFormat);
  theG2B3Data.theFormat.flagUseLatex = true;
  theG2B3Data.theFormat.NumAmpersandsPerNewLineForLaTeX = 0;
  Expression tempExpression;
  RationalFunction rfZero, rfOne;
  rfZero.makeZero();
  rfOne.makeOne();
  latexTable2 << "\\begin{longtable}{|rll|}\\caption"
  << "{Values of $x_1$ for each $v_{\\lambda,i}$}\\label{tableCriticalValuesvlambda}"
  << "\\endhead";
  for (int i = 0; i < theHWs.size; i ++) {
    theG2B3Data.theWeightFundCoords = theHWs[i];
    theCommands.innerSplitFDpartB3overG2inner(theCommands, theG2B3Data, tempExpression);
    timeReport << tempExpression.getValue<std::string>();
    RationalFunction numEigenVectors;
    numEigenVectors = rfZero;
    for (int j = 0; j < theG2B3Data.theSmallCharFDpart.size(); j ++) {
      numEigenVectors += theG2B3Data.theSmallCharFDpart.coefficients[j];
    }
    theG2B3Data.theFormat.CustomPlusSign = "";
    int eigenIndexcounter = 0;
    if (i != 0) {
      latexTable2 << "\\hline\\multicolumn{3}{|c|}{$\\lambda ="
      << theG2B3Data.theWeightFundCoords.ToStringLetterFormat("\\omega", &theG2B3Data.theFormat)
      << "$}\\\\vector& coefficient of $v_\\lambda$ in $Sh_{\\lambda,i}$ &$x_1\\notin$ \\\\\\hline";
    }
    for (int k = 0; k < theG2B3Data.theSmallCharFDpart.size(); k ++) {
      CharacterSemisimpleLieAlgebraModule<RationalFunction> tempChar;
      tempChar.addMonomial(theG2B3Data.theSmallCharFDpart[k], theG2B3Data.theSmallCharFDpart.coefficients[k]);
      int multiplicity = 0;
      theG2B3Data.theSmallCharFDpart.coefficients[k].isSmallInteger(&multiplicity);
      for (int counter = 0; counter < multiplicity; counter ++, eigenIndexcounter ++) {
        out << "<tr>";
        if (k == 0 && counter == 0) {
          theG2B3Data.theFormat.CustomPlusSign = "\\oplus ";
          theG2B3Data.theFormat.fundamentalWeightLetter = "\\omega";
          out << "<td rowspan =\"" << numEigenVectors.toString() << "\">" << theG2B3Data.theAmbientChar.toString(&theG2B3Data.theFormat) << "</td> ";
          if (!isFD) {
            out << "<td rowspan =\"" << numEigenVectors.toString() << "\">";
            for (int charcounter1 = 0; charcounter1 < theG2B3Data.theCharacterDifferences.size; charcounter1 ++) {
              std::string tempS = theG2B3Data.theFormat.CustomPlusSign;
              theG2B3Data.theFormat.CustomPlusSign = "";
              out << "A_{" << charcounter1 << "}=" << theG2B3Data.theCharacterDifferences[charcounter1].toString(&theG2B3Data.theFormat) << ";";
              if (charcounter1 != theG2B3Data.theCharacterDifferences.size - 1) {
                out << "<br>";
              }
              theG2B3Data.theFormat.CustomPlusSign = tempS;
            }
            out << "</td> ";
          }
          latexTable << "\\multirow{" << theG2B3Data.theEigenVectorS.size  << "}{*}{$"
          << theG2B3Data.theAmbientChar.toString(&theG2B3Data.theFormat) << "$}";
          Vector<RationalFunction> theSimpleCoordinates;
          theSimpleCoordinates = theG2B3Data.WeylFD.AmbientWeyl->GetSimpleCoordinatesFromFundamental(
            theG2B3Data.theAmbientChar[0].weightFundamentalCoordS
          );
          RationalFunction theWeylSize;
          theWeylSize = theG2B3Data.WeylFD.WeylDimFormulaInnerSimpleCoords(theSimpleCoordinates);
          latexTable << "& \\multirow{" << theG2B3Data.theEigenVectorS.size  << "}{*}{$"
          << theWeylSize.toString(&theG2B3Data.theFormat)
          << "$}";
        } else {
          latexTable << "&";
        }
        latexTable << " & ";
        if (counter == 0) {
          theG2B3Data.theFormat.fundamentalWeightLetter = "\\psi";
          theG2B3Data.theFormat.CustomPlusSign = "\\oplus ";
          out << "<td rowspan =\"" << multiplicity << " \">" << tempChar.toString(&theG2B3Data.theFormat) << "</td>";
          latexTable << "\\multirow{" << multiplicity  << "}{*}{$";
          latexTable << tempChar.toString(&theG2B3Data.theFormat) << "$}";
          latexTable << "&\\multirow{" << multiplicity  << "}{*}{$";
          if (multiplicity > 1) {
            latexTable << multiplicity << "\\times";
          }
          Vector<RationalFunction> theSimpleCoordinates;
          theSimpleCoordinates = theG2B3Data.WeylFDSmall.AmbientWeyl->GetSimpleCoordinatesFromFundamental(
            tempChar[0].weightFundamentalCoordS
          );
          RationalFunction dimension;
          dimension = theG2B3Data.WeylFDSmall.WeylDimFormulaInnerSimpleCoords(theSimpleCoordinates, rfOne);
          latexTable << dimension.toString(&theG2B3Data.theFormat) << "$}";
        } else {
          latexTable << "&";
        }
        latexTable << "&";
        theG2B3Data.theFormat.CustomPlusSign = "";
        out << "<td>" << HtmlRoutines::GetMathSpanPure(theG2B3Data.theEigenVectorsLevi[eigenIndexcounter].toString(&theG2B3Data.theFormat)) << "</td>";
        theG2B3Data.theFormat.MaxLineLength = 20;
        latexTable << "$\\begin{array}{l}" << theG2B3Data.theEigenVectorsLevi[eigenIndexcounter].toString(&theG2B3Data.theFormat) << "\\end{array}$ \n";
        if (!isFD) {
          std::string tempS1 = theG2B3Data.GetStringCasimirProjector(eigenIndexcounter, 12);
          std::string tempS2 = "("+ theG2B3Data.theUEelts[eigenIndexcounter].toString(&theG2B3Data.theFormat) + ")\\cdot v_\\lambda";
          out << "<td>" << HtmlRoutines::GetMathSpanPure(tempS1) << "</td>";
          out << "<td>" << theG2B3Data.additionalMultipliers[eigenIndexcounter].toString() << "</td>";
          out << "<td>" << HtmlRoutines::GetMathSpanPure(tempS2) << "</td>";
          out << "<td>" << theG2B3Data.theShapovalovProducts[eigenIndexcounter].toString(&theG2B3Data.theFormat);
          out << "</td>";
          int theIndex = - 1;
          numEigenVectors.isSmallInteger(&theIndex);
          if (theIndex - eigenIndexcounter - 1 > 0) {
            List<Rational> tempList, tempList2;
            latexTable2 << " $v_{\\lambda," <<  theIndex- eigenIndexcounter - 1 << "} $&";
            Polynomial<Rational> tempP;
            theG2B3Data.theShapovalovProducts[eigenIndexcounter].getNumerator(tempP);
            tempP.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
            latexTable2 << "$\\begin{array}{l}" << tempP.toString(&theG2B3Data.theFormat) << "\\end{array}$ & ";
            if (tempP.findOneVariableRationalRoots(tempList)) {
              tempList2.addOnTopNoRepetition(tempList);
              out << "<td>Rational roots: " << tempList.toString() << "</td>";
              latexTable2 << tempList2.toString();
            }
            latexTable2 << "\\\\";
          }
          latexTable << "& $\\begin{array}{l}" << tempS1 << "\\end{array}$"
          << "&" << "$\\begin{array}{l}" << tempS2 << "\\end{array}$";
        }
        latexTable << "\\\\ \n <br>\n";
        if (counter != multiplicity - 1) {
          if (isFD) {
            latexTable << "\\cline{5-5}";
          } else {
            latexTable << "\\cline{5-7}";
          }
        }
        out << "</tr>";
      }
      if (k != theG2B3Data.theSmallCharFDpart.size() - 1) {
        if (isFD) {
          latexTable << "\\cline{3-5}";
        } else {
          latexTable << "\\cline{3-7}";
        }
      }
    }
    latexTable << "\\hline";
  }
  latexTable << "\\hline";
  out << "</table>";
  latexTable << "\\end{longtable}";
  latexTable2 << "\\end{longtable}";
  out << "<br>" << timeReport.str() << "<br><br><br><b>Ready for LaTeX consumption:</b><br  >";
  out << latexTable2.str();
  out << "<br><br><br>";
  out << latexTable.str();
  out << "<br>";
  return output.assignValue(out.str(), theCommands);
}

bool Calculator::innerPrintB3G2branchingTable(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerPrintB3G2branchingTable");
  Vectors<RationalFunction> theHWs;
  BranchingData theG2B3Data;
  ExpressionContext context(theCommands);
  if (!theCommands.innerPrintB3G2branchingTableCommon(
    theCommands, input, output, theHWs, theG2B3Data, context
  )) {
    return false;
  }
  if (output.IsError()) {
    return true;
  }
  return theCommands.innerPrintB3G2branchingIntermediate(
    theCommands, input, output, theHWs, theG2B3Data, context
  );
}

bool Calculator::innerPrintB3G2branchingTableCharsOnly(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerPrintB3G2branchingTableCharsOnly");
  BranchingData theg2b3data;
  ExpressionContext theContext(theCommands);
  Vectors<RationalFunction> theHWs;
  theCommands.innerPrintB3G2branchingTableCommon(
    theCommands, input, output, theHWs, theg2b3data, theContext
  );
  if (output.IsError()) {
    return true;
  }
  CharacterSemisimpleLieAlgebraModule<RationalFunction> theCharacter, outputChar;
  Vector<Rational> theHW;
  std::stringstream out;
  std::stringstream latexTable;
  bool isFD = (theg2b3data.selInducing.CardinalitySelection == 0);
  if (isFD) {
    out << "<table>"
    << "<tr><td>$so(7)$-highest weight</td>"
    << "<td>Dimension</td><td>Decomposition over $G_2$</td><td>Dimensions</td>" << "</tr>";
    latexTable << "\\begin{longtable}{|rl|} \\caption{\\label{tableB3fdsOverG2charsonly} "
    << "Decompositions of finite dimensional $so(7)$-modules over $G_2$}\\\\"
    << "\\hline$so(7)$-module & ~~~~~~ decomposition over $G_2$\\endhead \\hline\n<br>";
  } else {
    out << "Let " << HtmlRoutines::GetMathSpanPure("p\\subset so(7)") << " be the "
    << theg2b3data.selInducing.toString() << "-parabolic subalgebra "
    << "and let " << HtmlRoutines::GetMathSpanPure("{p}'= p\\cap G_2")
    << ". Then  " << HtmlRoutines::GetMathSpanPure("{p}'") << " is the "
    << theg2b3data.selSmallParSel.toString() << "- parabolic subalgebra of G_2"
    << "<br> <table><tr><td>$so(7)$-highest weight</td>"
    << "<td>Dimension of inducing fin. dim. "
    << HtmlRoutines::GetMathSpanPure(" p")
    << "-module</td><td>Decomposition of inducing module over "
    << HtmlRoutines::GetMathSpanPure("p'")
    << "</td><td>Dimensions</td>"
    << " <td>Highest weight <br> is sufficiently generic <br> if none of <br>the following vanish</td>"
    << "</tr>";
    latexTable << "\\begin{longtable}{|p{2cm}l|} \\caption{\\label{tableB3fdsOverG2charsonly"
    << theg2b3data.selInducing.toString() << "} "
    << "Decompositions of inducing $\\mathfrak{p}" << "_{"
    << theg2b3data.selInducing.toString()
    << "}" << "$-modules over $\\bar{ \\mathfrak {l}}$"
    << ", where $\\mathfrak{l}$ is the reductive Levi part of "
    << "$\\mathfrak{p}" << "_{" << theg2b3data.selInducing.toString()
    <<  "}$ and $\\bar{\\mathfrak {l}}$ is the reductive Levi part of $\\bar{\\mathfrak {p}} _{"
    << theg2b3data.selSmallParSel.toString()  << "}$" << " } \\\\"
    << "\\hline $V_\\lambda(\\mathfrak l)$ " << "& Decomposition over $\\bar { \\mathfrak l}$ "
    << "\\endhead \\hline\n<br>";
  }
  theg2b3data.theFormat.flagUseLatex = true;
  ElementUniversalEnveloping<RationalFunction> theCasimir, theCentralCharacter, resultChar;
  HashedList<ElementUniversalEnveloping<RationalFunction> > theCentralChars;
  theCasimir.MakeCasimir(theg2b3data.theHmm.theDomain());
  WeylGroupData& smallWeyl = theg2b3data.theHmm.theDomain().theWeyl;
  for (int k = 0; k < theHWs.size; k ++) {
    theCharacter.MakeFromWeight(
      theg2b3data.theHmm.theRange().theWeyl.GetSimpleCoordinatesFromFundamental(theHWs[k]),
      &theg2b3data.theHmm.theRange()
    );
    theCharacter.SplitCharOverRedSubalg(nullptr, outputChar, theg2b3data);
    theg2b3data.theFormat.fundamentalWeightLetter = "\\omega";
    out << "<tr><td> " << theCharacter.toString(&theg2b3data.theFormat) << "</td> ";
    Vector<RationalFunction> simpleCoordinates;
    simpleCoordinates = theg2b3data.WeylFD.AmbientWeyl->GetSimpleCoordinatesFromFundamental(
      theCharacter[0].weightFundamentalCoordS
    );
    RationalFunction dimension;
    dimension = theg2b3data.WeylFD.WeylDimFormulaInnerSimpleCoords(simpleCoordinates);
    out << "<td>" << dimension.toString() << "</td>";
    latexTable << " $ " << theCharacter.toString(&theg2b3data.theFormat) << " $ ";
    theg2b3data.theFormat.fundamentalWeightLetter = "\\psi";
    out << "<td>" << outputChar.toString(&theg2b3data.theFormat) << "</td>";
    out << "<td>";
    theg2b3data.theFormat.CustomPlusSign = "\\oplus ";
    Vector<RationalFunction> leftWeightSimple, leftWeightDual, rightWeightSimple, rightWeightDual;
    theCentralChars.clear();
    for (int i = 0; i < outputChar.size(); i ++) {
      if (!outputChar.coefficients[i].isEqualToOne()) {
        out << outputChar.coefficients[i].toString() << " x ";
      }
      simpleCoordinates = theg2b3data.WeylFDSmall.AmbientWeyl->GetSimpleCoordinatesFromFundamental(
        outputChar[i].weightFundamentalCoordS
      );
      dimension = theg2b3data.WeylFDSmall.WeylDimFormulaInnerSimpleCoords(simpleCoordinates);
      out << dimension;
      if (i != outputChar.size() - 1) {
        out << "+";
      }
      leftWeightSimple = smallWeyl.GetSimpleCoordinatesFromFundamental(outputChar[i].weightFundamentalCoordS);
      leftWeightDual = smallWeyl.GetDualCoordinatesFromFundamental(outputChar[i].weightFundamentalCoordS);
      for (int j = 0; j < outputChar.size(); j ++) {
        rightWeightSimple = smallWeyl.GetSimpleCoordinatesFromFundamental(outputChar[j].weightFundamentalCoordS);
        rightWeightDual = smallWeyl.GetDualCoordinatesFromFundamental(outputChar[j].weightFundamentalCoordS);
        if ((rightWeightSimple - leftWeightSimple).isPositive()) {
          resultChar = theCasimir;
          theCentralCharacter = theCasimir;
          resultChar.ModOutVermaRelations(&rightWeightDual);
          theCentralCharacter.ModOutVermaRelations(&leftWeightDual);
          resultChar -= theCentralCharacter;
          resultChar.scaleNormalizeLeadingMonomial(nullptr);
          resultChar *= - 1;
          theCentralChars.addOnTopNoRepetition(resultChar);
        }
      }
    }
    out << "</td>";
    theg2b3data.theFormat.MaxLineLength = 80;
    latexTable << "& $\\begin{array}{l} " << outputChar.toString(&theg2b3data.theFormat) << "\\end{array} $ ";
    theg2b3data.theFormat.CustomPlusSign = "";
    if (!isFD && theCentralChars.size > 0) {
      out << "<td>";
      latexTable << " \\\\\n<br> ";
      latexTable << "\\multicolumn{2}{|p{11.6cm}|}{ \\tiny Strong Condition B: ";
      for (int l = 0; l < theCentralChars.size; l ++) {
        out << theCentralChars[l].toString(&theg2b3data.theFormat) << "<br> ";
        latexTable << "$" << theCentralChars[l].toString(&theg2b3data.theFormat) << "\\neq 0$";
        if (l != theCentralChars.size - 1) {
          latexTable << ", ";
        }
      }
      latexTable << "\\normalsize}";
      out << " </td>";
    }
    out << "</tr>";
    latexTable << "\\\\\n <br>\n";
    latexTable << "\\hline";
  }
  out << "</table>";
  latexTable << "\\end{longtable}";
  out << "<br><b>Ready for LaTeX consumption:</b><br>%preamble: "
  << "<br>\\documentclass{article}<br>\\usepackage{longtable, amssymb}"
  << "<br>\\begin{document}<br>%text body<br>" << latexTable.str() << "<br>%end of text body <br>\\end{document}";
  return output.assignValue(out.str(), theCommands);
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
bool ElementSumGeneralizedVermas<Coefficient>::ExtractElementUE(
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
    tempMon *= currentMon.GetOwner().theGeneratingWordsNonReduced[currentMon.indexFDVector];
    output.addMonomial(tempMon, this->coefficients[i]);
  }
  return true;
}

bool Calculator::innerSplitFDpartB3overG2inner(Calculator& theCommands, BranchingData& theG2B3Data, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerSplitFDpartB3overG2inner");
  ModuleSSalgebra<RationalFunction> theModCopy;
  theModCopy.MakeFromHW(
    theG2B3Data.theHmm.theRange(), theG2B3Data.theWeightFundCoords, theG2B3Data.selInducing, 1, 0, nullptr, false
  );
  theG2B3Data.resetOutputData();
  theG2B3Data.initAssumingParSelAndHmmInitted();
  theG2B3Data.SelSplittingParSel = theG2B3Data.selInducing;
  if (theG2B3Data.SelSplittingParSel.selected[0] != theG2B3Data.SelSplittingParSel.selected[2]) {
    theG2B3Data.SelSplittingParSel.AddSelectionAppendNewIndex(0);
    theG2B3Data.SelSplittingParSel.AddSelectionAppendNewIndex(2);
  }
  Vector<Rational> splittingParSel;
  splittingParSel = theG2B3Data.SelSplittingParSel;

  theCommands.theObjectContainer.theCategoryOmodules.addNoRepetitionOrReturnIndexFirst(theModCopy);
  int theModIndex = theCommands.theObjectContainer.theCategoryOmodules.getIndex(theModCopy);
  ModuleSSalgebra<RationalFunction>& theMod = theCommands.theObjectContainer.theCategoryOmodules[theModIndex];
  theMod.GetOwner().flagHasNilradicalOrder = true;
  std::stringstream out;
  theCommands << "<hr>Time elapsed before making B3 irrep: " << global.getElapsedSeconds();
  double timeAtStart = global.getElapsedSeconds();
  theMod.SplitFDpartOverFKLeviRedSubalg(
    theG2B3Data.theHmm,
    theG2B3Data.selSmallParSel,
    &theG2B3Data.outputEigenWords,
    &theG2B3Data.outputWeightsFundCoordS,
    &theG2B3Data.leviEigenSpace,
    nullptr
  );
  theCommands << "<br>Time needed to make B3 irrep: " << global.getElapsedSeconds() - timeAtStart;
  theG2B3Data.g2Weights.setSize(theG2B3Data.outputWeightsFundCoordS.size);
  theG2B3Data.g2DualWeights.setSize(theG2B3Data.outputWeightsFundCoordS.size);
  Matrix<Rational> invertedG2cartanMat;
  invertedG2cartanMat = theG2B3Data.theHmm.theDomain().theWeyl.cartanSymmetric;
  invertedG2cartanMat.invert();
  WeylGroupData& rangeWeyl = theG2B3Data.theHmm.theRange().theWeyl;
  theG2B3Data.outputWeightsSimpleCoords = rangeWeyl.GetSimpleCoordinatesFromFundamental(theG2B3Data.outputWeightsFundCoordS);
  Vector<RationalFunction> weightSimpleCoordinates;
  weightSimpleCoordinates = rangeWeyl.GetSimpleCoordinatesFromFundamental(
    theG2B3Data.theWeightFundCoords
  );
  theG2B3Data.theAmbientChar.MakeFromWeight(weightSimpleCoordinates, &theG2B3Data.theHmm.theRange());
  theG2B3Data.theSmallCharFDpart.makeZero();
  CharacterSemisimpleLieAlgebraModule<RationalFunction> tempMon;
  for (int i = 0; i < theG2B3Data.outputWeightsSimpleCoords.size; i ++) {
    Vector<RationalFunction>& currentWeight = theG2B3Data.outputWeightsSimpleCoords[i];
    Vector<RationalFunction>& currentG2Weight = theG2B3Data.g2Weights[i];
    Vector<RationalFunction>& currentG2DualWeight = theG2B3Data.g2DualWeights[i];
    currentG2DualWeight.setSize(2);
    currentG2DualWeight[0] = theG2B3Data.theHmm.theRange().theWeyl.RootScalarCartanRoot(
      currentWeight, theG2B3Data.theHmm.ImagesCartanDomain[0]
    );
    //<-note: implicit type conversion: the return type is the left coefficient type.
    currentG2DualWeight[1] = theG2B3Data.theHmm.theRange().theWeyl.RootScalarCartanRoot(
      currentWeight, theG2B3Data.theHmm.ImagesCartanDomain[1]
    );
    //<-note: implicit type conversion: the return type is the left coefficient type.
    invertedG2cartanMat.actOnVectorColumn(currentG2DualWeight, currentG2Weight);//<-g2weight is now computed;
    tempMon.MakeFromWeight(currentG2Weight, &theG2B3Data.theHmm.theDomain());
    theG2B3Data.theSmallCharFDpart += tempMon;
  }
  ElementUniversalEnveloping<RationalFunction> theG2Casimir, theG2CasimirCopy, imageCasimirInB3, tempElt;
  theG2Casimir.MakeCasimir(theG2B3Data.theHmm.theDomain());

  theG2B3Data.theChars.setSize(theG2B3Data.outputWeightsFundCoordS.size);
  for (int i = 0; i < theG2B3Data.outputWeightsSimpleCoords.size; i ++) {
    Vector<RationalFunction>& currentG2DualWeight = theG2B3Data.g2DualWeights[i];
    theG2CasimirCopy = theG2Casimir;
    theG2CasimirCopy.ModOutVermaRelations(&currentG2DualWeight, 1, 0);
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
  ElementSumGeneralizedVermas<RationalFunction>& theHWV = *theG2B3Data.theEigenVectorsLevi.lastObject();
  theHWV.MakeHWV(theMod, 1);
  theHWV *= - 1;
  *theG2B3Data.theEigenVectorS.lastObject() = theHWV;
  Vector<RationalFunction> weightDifference;
  theG2B3Data.theHmm.applyHomomorphism(theG2Casimir, imageCasimirInB3);
  theG2Casimir.checkConsistency();
  imageCasimirInB3.checkConsistency();
  RationalFunction charDiff;
  theG2B3Data.theHmm.theRange().OrderNilradical(
    theMod.parabolicSelectionNonSelectedAreElementsLevi,
    theG2B3Data.flagUseNilWeightGeneratorOrder,
    theG2B3Data.flagAscendingGeneratorOrder
  );
  for (int k = 0; k < theG2B3Data.g2Weights.size; k ++) {
    ElementSumGeneralizedVermas<RationalFunction>& currentTensorEltLevi = theG2B3Data.theEigenVectorsLevi[k];
    ElementSumGeneralizedVermas<RationalFunction>& currentTensorEltEigen = theG2B3Data.theEigenVectorS[k];
    ElementUniversalEnveloping<RationalFunction>& currentUEelt = theG2B3Data.theUEelts[k];
    currentTensorEltLevi = theHWV;
    currentTensorEltLevi.MultiplyMeByUEEltOnTheLeft(theG2B3Data.outputEigenWords[k]);
    currentTensorEltEigen = currentTensorEltLevi;
    if (theG2B3Data.selInducing.CardinalitySelection > 0) {
      for (int j = 0; j < theG2B3Data.g2Weights.size; j ++) {
        weightDifference = theG2B3Data.g2Weights[j] - theG2B3Data.g2Weights[k];
        if (weightDifference.isPositive()) {
          theG2CasimirCopy = imageCasimirInB3;
          tempElt.makeConstant(theG2B3Data.theChars[j], theG2B3Data.theHmm.theRange());
          theG2CasimirCopy -= tempElt;
          theG2CasimirCopy *= 12;
          currentTensorEltEigen.MultiplyMeByUEEltOnTheLeft(theG2CasimirCopy);
          charDiff = theG2B3Data.theChars[j];
          charDiff -= *theG2B3Data.theChars.lastObject();
          theG2B3Data.theCharacterDifferences.addOnTopNoRepetition(charDiff);
        }
      }
    }
    RationalFunction scale = currentTensorEltEigen.scaleNormalizeLeadingMonomial(nullptr);
    if (!scale.isConstant(&theG2B3Data.additionalMultipliers[k])) {
      global.fatal << "This is unexpected: the scale is not a constant. " << global.fatal;
    }
    currentTensorEltEigen.ExtractElementUE(currentUEelt, *theMod.owner);
    currentUEelt.HWTAAbilinearForm(
      currentUEelt, theG2B3Data.theShapovalovProducts[k], &theMod.theHWDualCoordsBaseFielD, 1, 0, nullptr
    );
  }
  return output.assignValue(out.str(), theCommands);
}

bool Calculator::innerJacobiSymbol(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerJacobiSymbol");
  global.fatal << "Function not implemented yet." << global.fatal;
  (void) theCommands;
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

bool Calculator::innerPrintAllVectorPartitions(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerPrintAllVectorPartitions");
  RecursionDepthCounter theRecursion(&theCommands.RecursionDeptH);
  if (input.size() != 3) {
    return output.makeError("Function innerPrintAllPartitions expects 2 arguments.", theCommands);
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!theCommands.Convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, algebra
  )) {
    return output.makeError("Error extracting Lie algebra.", theCommands);
  }
  SemisimpleLieAlgebra* theSSowner = algebra.content;

  SemisimpleLieAlgebra& theSSalgebra = *theSSowner;
  ExpressionContext theContext(theCommands);
  Vector<Rational> theHW;
  if (!theCommands.getVector<Rational>(input[2], theHW, &theContext, theSSalgebra.GetRank())) {
    return output.makeError("Failed to extract weight you want partitioned from " + input[2].toString(), theCommands);
  }
  Vector<int> theHWint;
  theHWint.setSize(theHW.size);
  for (int i = 0; i < theHW.size; i ++) {
    if (!theHW[i].isSmallInteger(&theHWint[i]) || theHW[i] < 0) {
      return output.makeError(
        "The input weight you gave is bad: "
        "it must consist of non-negative small integers",
        theCommands
      );
    }
  }
  std::stringstream out;
  out << "<br>the weight you want partitioned: " << theHWint;
  Vector<int> thePartition;
  thePartition.setSize(theSSalgebra.GetNumPosRoots());
  for (int i = 0; i < thePartition.size; i ++) {
    thePartition[i] = 0;
  }
  Vector<Rational> theWeight, tmpWt;
  Vectors<Rational>& rootsBorel = theSSalgebra.theWeyl.RootsOfBorel;
  int counter = 0;
  int totalCycles = 0;
  theWeight.makeZero(theSSalgebra.GetRank());
  int i = rootsBorel.size;
  while (i > 0 && counter < 10000) {
    totalCycles ++;
    if (theWeight == theHW) {
      tmpWt = thePartition;
      out << "<br>" << tmpWt.ToStringLetterFormat("\\alpha");
      counter ++;
    }
    if (!(theHW - theWeight).isPositive() || i > rootsBorel.size) {
      if (i <= rootsBorel.size) {
        theWeight -= rootsBorel[i - 1] * thePartition[i - 1];
        thePartition[i - 1] = 0;
      }
      i --;
      if (i > 0) {
        theWeight += rootsBorel[i - 1];
        thePartition[i - 1] ++;
      }
    } else {
      i ++;
    }
  }
  out << "<br>Done in " << totalCycles << " cycles.";
  out << "<br>" << counter << " total partitions ";
  return output.assignValue(out.str(), theCommands);
}

void WeylGroupData::GetHighestWeightsAllRepsDimLessThanOrEqualTo(
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
      theDim = this->WeylDimFormulaFundamentalCoords(current);
      if (theDim < dimBound) {
        output.addOnTopNoRepetition(current);
      }
      current[k] -= 1;
    }
  }
  outputHighestWeightsFundCoords = output;
}

bool Calculator::innerTestMonomialBaseConjecture(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerTestMonomialBaseConjecture");
  RecursionDepthCounter theRecursion(&theCommands.RecursionDeptH);
  if (!input.isListNElements(3)) {
    return output.makeError("innerTestMonomialBaseConjecture takes two arguments as input", theCommands);
  }
  const Expression& rankE = input[1];
  const Expression& dimE = input[2];
  int rankBound = 0;
  int dimBound = 0;
  if (!rankE.isSmallInteger(&rankBound) || !dimE.isSmallInteger(&dimBound)) {
    return output.makeError("The rank and  dim bounds must be small integers", theCommands);
  }
  if (rankBound < 2 || rankBound > 100 || dimBound < 1 || dimBound > 10000) {
    return output.makeError(
      "The rank bound must be an integer between 2 and 100, "
      "and the dim bound must be an integer between 1 and 10000. ",
      theCommands
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
    currentType.MakeSimpleType(theWeylLetters[i], theRanks[i]);
    SemisimpleLieAlgebra& currentAlg =
    theCommands.theObjectContainer.GetLieAlgebraCreateIfNotPresent(currentType);
    currentAlg.ComputeChevalleyConstants();
    currentAlg.theWeyl.GetHighestWeightsAllRepsDimLessThanOrEqualTo(theHighestWeights[i], dimBound);
    latexReport << "\\hline\\multicolumn{5}{c}{" << "$" << currentAlg.ToStringLieAlgebraName() << "$}\\\\\\hline\n\n"
    << "$\\lambda$ & dim &\\# pairs 1& \\# pairs total  & \\# Arithmetic op.  \\\\\\hline";
    out << "<br>" << " <table><tr><td  border =\"1\" colspan =\"3\">"
    << theWeylLetters[i] << "_{" << theRanks[i] << "}" << "</td></tr> <tr><td>highest weight</td><td>dim</td></tr>";
    List<Vector<Rational> >& theHws = theHighestWeights[i];
    tempSel.initialize(theRanks[i]);
    for (int j = 0; j < theHws.size; j ++) {
      std::stringstream reportStream;
      Vector<Rational>& currentHW = theHws[j];
      out << "<tr><td> " << currentHW.toString() << "</td><td>"
      << currentAlg.theWeyl.WeylDimFormulaFundamentalCoords(currentHW) << "</td>";
      reportStream << "Processing " << currentAlg.ToStringLieAlgebraName() << ", index  "
      << i + 1 << " out of " << theRanks.size << ",  highest weight "
      << currentHW.toString() << ", dim: " << currentAlg.theWeyl.WeylDimFormulaFundamentalCoords(currentHW)
      << ", index " << j + 1 << " out of " << theHws.size;
      theReport.report(reportStream.str());
      latexReport << "$" << currentHW.ToStringLetterFormat("\\omega") << "$ &"
      << currentAlg.theWeyl.WeylDimFormulaFundamentalCoords(currentHW) << "&";
      hwPath.MakeFromWeightInSimpleCoords(
        currentAlg.theWeyl.GetSimpleCoordinatesFromFundamental(currentHW), currentAlg.theWeyl
      );
      hwPath.generateOrbit(
        tempList,
        theStrings,
        MathRoutines::Minimum(1000, currentAlg.theWeyl.WeylDimFormulaFundamentalCoords(currentHW).numeratorShort),
        nullptr
      );
      reportStream << "\nPath orbit size = " << theStrings.size
      << " generated in " << global.getElapsedSeconds() << " seconds. ";
      theReport.report(reportStream.str());
      for (int k = 0; k < theStrings.size; k ++) {
        LittelmannPath& currentPath = tempList[k];
        tempMon = theStrings[k];
        tempMon.generatorsIndices.reverseElements();
        tempMon.Powers.reverseElements();
        if (!currentPath.IsAdaptedString(tempMon)) {
          foundBad = true;
          break;
        }
      }
      if (!foundBad) {
        out << "<td>all strings adapted</td>";
      } else {
        out << "<td>has non-adapted string</td>";
      }
/*      if (theMod.MakeFromHW
          (theCommands.theObjectContainer.theLieAlgebras, i,
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
        << Rational::TotalLargeAdditions+Rational::TotalSmallAdditions
        +Rational::TotalLargeMultiplications+Rational::TotalSmallMultiplications -
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
  return output.assignValue(out.str(), theCommands);
}

bool Calculator::innerLittelmannOperator(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerLittelmannOperator");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (input.hasBoundVariables()) {
    return false;
  }
  int theIndex = 0;
  if (!input.isSmallInteger(&theIndex)) {
    return output.makeError(
      "The argument of the Littelmann root operator is "
      "expected to be a small integer, instead you gave me " +
      input.toString(),
      theCommands
    );
  }
  if (theIndex == 0) {
    return output.makeError("The index of the Littelmann root operator is expected to be non-zero", theCommands);
  }
  return output.assignValue(theIndex, theCommands);
}

bool Calculator::innerLSPath(Calculator& theCommands, const Expression& input, Expression& output) {
  RecursionDepthCounter theRecutionIncrementer(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("Calculator::innerLSPath");
  if (input.size() < 3) {
    return output.makeError("LSPath needs at least two arguments.", theCommands);
  }
  WithContext<SemisimpleLieAlgebra*> theSSowner;
  if (!theCommands.Convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, theSSowner
  )) {
    return output.makeError("Error extracting Lie algebra.", theCommands);
  }
  SemisimpleLieAlgebra& ownerSSalgebra = *theSSowner.content;
  Vectors<Rational> waypoints;
  waypoints.setSize(input.children.size - 2);
  for (int i = 2; i < input.children.size; i ++) {
    if (!theCommands.getVector<Rational>(
      input[i], waypoints[i - 2], nullptr, ownerSSalgebra.GetRank(), nullptr
    )) {
      return output.makeError("Failed to extract waypoints", theCommands);
    }
  }
  waypoints = ownerSSalgebra.theWeyl.GetSimpleCoordinatesFromFundamental(waypoints);
  LittelmannPath theLSpath;
  theLSpath.MakeFromWaypoints(waypoints, ownerSSalgebra.theWeyl);
  return output.assignValue(theLSpath, theCommands);
}

template <>
bool CalculatorConversions::innerPolynomial<Rational>(Calculator& theCommands, const Expression& input, Expression& output);
template <>
bool CalculatorConversions::functionPolynomial<Rational>(Calculator& theCommands, const Expression& input, Expression& output);

bool Calculator::innerFactorPolynomial(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerFactorPolynomial");
  WithContext<Polynomial<Rational> > polynomial;
  if (!theCommands.Convert(
    input, CalculatorConversions::innerPolynomial<Rational>, polynomial
  )) {
    return false;
  }
  if (polynomial.content.minimalNumberOfVariables() > 1) {
    return output.makeError(
      "I have been taught to factor one variable polynomials only. ",
      theCommands
    );
  }
  PolynomialFactorization<Rational, PolynomialFactorizationKronecker> factorization;
  if (!factorization.factor(
    polynomial.content,
    &theCommands.comments
  )) {
    return false;
  }
  List<Expression> resultSequence;
  Expression constantFactor;
  constantFactor.assignValue(factorization.constantFactor, theCommands);
  resultSequence.addOnTop(constantFactor);
  Expression polynomialE, expressionE(theCommands);

  for (int i = 0; i < factorization.reduced.size; i ++) {
    polynomialE.assignValueWithContext(
      factorization.reduced[i], polynomial.context, theCommands
    );
    expressionE.children.clear();
    expressionE.addChildAtomOnTop("MakeExpression");
    expressionE.addChildOnTop(polynomialE);
    resultSequence.addOnTop(expressionE);
  }
  return output.makeSequence(theCommands, &resultSequence);
}

bool Calculator::innerZmodP(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerZmodP");
  if (!input.isListNElements(3)) {
    return false;
  }
  Rational left, right;
  if (!input[1].isRational(&left) || !input[2].isRational(&right)) {
    return false;
  }
  LargeInteger base;
  if (!right.isInteger(&base)) {
    return false;
  }
  if (base.isEqualToZero()) {
    return false;
  }
  LargeIntegerUnsigned theGCD;
  LargeIntegerUnsigned::gcd(left.getDenominator(), base.value, theGCD);
  if (theGCD > 1) {
    return false;
  }
  ElementZmodP outputElt;
  outputElt.theModulus = base.value;
  outputElt = left.getNumerator();
  return output.assignValue(outputElt, theCommands);
}

bool Calculator::innerInterpolatePoly(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerInterpolatePoly");
  if (input.size() < 2) {
    return false;
  }
  Expression convertedE;
  if (!CalculatorConversions::innerMakeMatrix(theCommands, input, convertedE)) {
    return false;
  }
  Matrix<Rational> pointsOfInterpoly;
  if (!theCommands.functionGetMatrix(
    convertedE, pointsOfInterpoly, nullptr, 2
  )) {
    return theCommands
    << "<hr>Failed to extract points of interpolation from "
    << convertedE.toString();
  }
  Polynomial<Rational> interPoly;
  Vector<Rational> theArgs, theValues;
  pointsOfInterpoly.GetVectorFromColumn(0, theArgs);
  pointsOfInterpoly.GetVectorFromColumn(1, theValues);
  interPoly.Interpolate(theArgs, theValues);
  ExpressionContext theContext(theCommands);
  theContext.makeOneVariableFromString("x");
  return output.assignValueWithContext(interPoly, theContext, theCommands);
}

bool Calculator::innerPrintZnEnumeration(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerPrintZnEnumeration");
  if (!input.isListNElements(3)) {
    return false;
  }
  int grade, dimension;
  if (!input[2].isSmallInteger(&grade) || !input[1].isSmallInteger(&dimension)) {
    return false;
  }
  if (grade > 10 || dimension > 5 || grade < 0 || dimension < 0) {
    return false;
  }
  SelectionPositiveIntegers theSel;
  theSel.initialize(dimension);
  std::stringstream out2, out;
  LargeIntegerUnsigned gradeLarge = static_cast<unsigned>(grade);
  int counter = 0;
  for (
    theSel.SetFirstInGradeLevel(gradeLarge);
    theSel.GetGrading() == gradeLarge;
    theSel.IncrementReturnFalseIfPastLast()
  ) {
    out2 << theSel.toString() << "<br>";
    counter ++;
  }
  out << "Total " << counter << " vectors:<br>" << out2.str();
  return output.assignValue(out.str(), theCommands);
}

bool Expression::AssignMatrixExpressions(
  const Matrix<Expression>& input,
  Calculator& owner,
  bool reduceOneRowToSequenceAndOneByOneToNonMatrix,
  bool dontReduceTypes
) {
  MacroRegisterFunctionWithName("Expression::AssignMatrixExpressions");
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
  enum mType{typeUnknown, typeRat, typeDouble, typeAlgebraic, typePolyRat, typePolyAlg, typeRF, typeExpression};
  mType outType = typeUnknown;
  Expression currentRow;
  for (int i = 0; i < input.numberOfRows; i ++) {
    currentRow.reset(owner);
    currentRow.children.reserve(input.numberOfColumns + 1);
    currentRow.addChildAtomOnTop(owner.opSequence());
    for (int j = 0; j < input.numberOfColumns; j ++) {
      currentRow.addChildOnTop(input(i, j));
      mType inType;
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
      } else if (input(i, j).isOfType<RationalFunction>()) {
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

bool Calculator::GetMatrixExpressionsFromArguments(
  const Expression& input, Matrix<Expression>& output, int desiredNumRows, int desiredNumCols
) {
  MacroRegisterFunctionWithName("Calculator::GetMatrixExpressionsFromArguments");
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
  if (!input.IsSequenceNElementS() && !input.isMatrix() && !input.IsIntervalRealLine()) {
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
  if (!input[1].IsSequenceNElementS() && !input[1].startsWith(this->opIntervalOpen())) {
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
      input[i].IsSequenceNElementS(output.numberOfColumns) ||
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

bool Calculator::innerEWAorPoly(Calculator& theCommands, const Expression& input, Expression& output, bool assignPoly) {
  MacroRegisterFunctionWithName("Calculator::innerEWAorPoly");
  if (!input.isListNElements(3)) {
    return false;
  }
  Vector<Polynomial<Rational> > inputPolForm;
  ExpressionContext startContext(theCommands);
  if (!theCommands.getVectorFromFunctionArguments(
    input,
    inputPolForm,
    &startContext,
    2,
    CalculatorConversions::functionPolynomial<Rational>
  )) {
    return theCommands
    << "<hr>Failed to extract polynomials from arguments of "
    << input.toString();
  }
  int letterDiff = 0, letterPol = 0;
  if (
    !inputPolForm[0].IsOneLetterFirstDegree(&letterDiff) ||
    !inputPolForm[1].IsOneLetterFirstDegree(&letterPol) ||
    letterDiff == letterPol
  ) {
    return theCommands
    << "<hr>Failed to get different one-variable polynomials from input. "
    << input.toString();
  }
  ExpressionContext endContext(theCommands);
  endContext.makeOneVariableOneDifferentialOperator(
    startContext.getVariable(letterPol),
    startContext.getVariable(letterDiff)
  );
  ElementWeylAlgebra<Rational> outputEWA;
  if (assignPoly) {
    outputEWA.Makexi(0, 1);
  } else {
    outputEWA.Makedi(0, 1);
  }
  return output.assignValueWithContext(outputEWA, endContext, theCommands);
}

bool Calculator::Test::ProcessOneTest(JSData& input) {
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
  Calculator::Test::OneTest& currentTest = this->commands.GetValueCreate(command);
  currentTest.expectedResult = input["output"].theString;
  return true;
}

bool Calculator::Test::LoadTestStrings(
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Calculator::LoadTestStrings");
  if (!FileOperations::FileExistsVirtual(
    WebAPI::calculator::testFileNameVirtual, false, false, nullptr
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Test json file does not exist. ";
    }
    this->flagTestResultsExist = false;
    return false;
  }
  std::string testStrings;
  if (!FileOperations::LoadFileToStringVirtual(
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
    this->ProcessOneTest(this->storedResults.theList[i]);
  }
  return true;
}

std::string Calculator::WriteFileToOutputFolderReturnLink(
  const std::string& fileContent, const std::string& fileName, const std::string& linkText
) {
  MacroRegisterFunctionWithName("Calculator::WriteFileToOutputFolderReturnLink");
  std::string fileNameVirtual = "output/" + fileName;
  return FileOperations::WriteFileReturnHTMLLink(fileContent, fileNameVirtual, linkText);
}

bool Calculator::Test::WriteTestStrings(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("Calculator::WriteTestStrings");
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
  return FileOperations::WriteFileVirual(
    WebAPI::calculator::testFileNameVirtual,
    result.toString(&JSData::PrintOptions::NewLine()),
    commentsOnFailure
  );
}

Calculator::Test::Test(Calculator& inputOwner) {
  this->startIndex = 0;
  this->startTime = global.GetElapsedMilliseconds();
  this->inconsistencies = 0;
  this->unknown = 0;
  this->noTestSkips = 0;
  this->numberOfTests = 0;
  this->lastIndexNotTested = 0;
  this->owner = &inputOwner;
  this->flagTestResultsExist = true;
}

bool Calculator::innerAutomatedTest(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerAutomatedTest");
  if (!global.UserDefaultHasAdminRights()) {
    return theCommands << "Automated test requires administrator access";
  }
  if (input.size() != 3) {
    return theCommands << "Automated test expects two arguments: "
    << "index of first test to run and number of tests to run. ";
  }
  global.millisecondsMaxComputation = 30000000; //30k seconds, ok as we have administrator access
  Calculator::Test test(theCommands);
  if (
    !input[1].isSmallInteger(&test.startIndex) ||
    !input[2].isSmallInteger(&test.numberOfTests)
  ) {
    return theCommands
    << "Automated test takes two arguments: "
    << "index of the first test to run and total "
    << "number of tests to run after that. ";
  }
  test.CalculatorTestRun();
  return output.assignValue(test.reportHtml, theCommands);
}

bool Calculator::Test::ProcessResults() {
  std::stringstream commentsOnFailure, out;

  if (!this->LoadTestStrings(&commentsOnFailure)) {
    global << Logger::red << "Failed to load test strings. " << Logger::endL
    << commentsOnFailure.str();
    out << "<b style='color:red'>Failed to load test strings. </b>" << commentsOnFailure.str();
  }
  if (!this->flagTestResultsExist) {
    out << "<b style='color:green'>Writing new test strings into: "
    << WebAPI::calculator::testFileNameVirtual << ". </b>";
    std::stringstream commentsOnFailure2;
    if (!this->WriteTestStrings(&commentsOnFailure2)) {
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
    << global.ToStringCalculatorComputation(
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
      theDiffer.left = HtmlRoutines::ConvertStringToHtmlString(currentTest.actualResult, false);
      theDiffer.right = HtmlRoutines::ConvertStringToHtmlString(currentTest.expectedResult, false);
      currentLine << "<td style = 'min-width:100px;'><b style='color:red'>unexpected result</b></td>"
      << "<td class = 'cellCalculatorResult'>";
      currentLine << theDiffer.DifferenceHTML("actual", "expected");
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
    << " and rerun the present test to store the expected results. "
    ;
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
    out << "<b style =\"color:green\">No inconsistencies or uknown computations.</b> ";
  }
  out << "<table>" << goodCommands.str() << "</table>";
  out << "<br>Total run time: " << global.GetElapsedMilliseconds() - this->startTime << " ms. ";
  this->reportHtml = out.str();
  return this->inconsistencies == 0;
}

int Calculator::GetNumBuiltInFunctions() {
  int result = 0;
  for (int i = this->NumPredefinedAtoms - 1; i >= 0; i --) {
    MemorySaving<Calculator::OperationHandlers>& current = this->operations.theValues[i];
    if (current.IsZeroPointer()) {
      continue;
    }
    result += current.getElement().handlers.size + current.getElement().compositeHandlers.size;
  }
  return result;
}

