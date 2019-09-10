//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader3Calculator0_Interface.h"
#include "vpfImplementationHeader2Math0_General.h"
#include "vpfImplementationHeader2Math3_WeylAlgebra.h"
#include "vpfImplementationHeader2Math051_PolynomialComputations_Basic.h"
#include "vpfImplementationHeader2Math15_UniversalEnveloping.h"
#include "vpfImplementationHeader2Math6_ModulesSSLieAlgebras.h"
#include "vpfHeader2Math5_SubsetsSelections.h"
#include "vpfImplementationHeader2Math3_FiniteGroups.h"
#include "vpfImplementationHeader2Math1_SemisimpleLieAlgebras.h"

extern ProjectInformationInstance ProjectInfoVpf5cpp;
ProjectInformationInstance ProjectInfoVpf5cpp(__FILE__, "Calculator built-in functions. ");

//If you get the C++ multiple definition error:
//- Try moving template *EXPLICIT* specializations (i.e. template <>)
//  to the *beginning* (!) of .cpp files.
//- Try moving template generics into .h files.

template <>
bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::IsDominantWRTgenerator<RationalFunctionOld>(
  const Vector<RationalFunctionOld>& theWeight, int generatorIndex
) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::IsDominantWRTgenerator");
  this->CheckInitialization();
  Vector<RationalFunctionOld> tempVect;
  RationalFunctionOld tempRF;
  tempVect = this->simpleRootsInner[generatorIndex].GetVectorRational();
  tempRF = this->AmbientWeyl->RootScalarCartanRoot(theWeight, tempVect);
  if (tempRF.expressionType != tempRF.typeRational) {
    crash << "This might or might not be a programming mistake: "
    << "I am being asked whether a weight "
    << "with rational function coefficients is dominant. "
    << "I took the scalar products with the positive simple roots "
    << "whose reflections generate the ambient group, "
    << "however one of the scalar products in question was non-constant. "
    << "More precisely, the scalar product of "
    << theWeight.ToString() << " and " << tempVect.ToString() << " "
    << "equals " << tempRF.ToString() << ". "
    << "I cannot decide (more precisely, do not want to *silently* decide for you) "
    << "whether a non-constant function is to be declared positive or not. "
    << "If this is not a programming mistake, "
    << "you might want to consider introducing a substitution "
    << "evaluating the rational function, some sort of a monomial order, "
    << "or some other method of deciding the \"sign\" of a rational function. "
    << "Whether or not this is a mistake, I am crashing. " << crash;
    return false;
  }
  return !tempRF.ratValue.IsNegative();
}

template <>
bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::IsDominantWRTgenerator<Rational>(
  const Vector<Rational>& theWeight, int generatorIndex
) {
  this->CheckInitialization();
  return !this->AmbientWeyl->RootScalarCartanRoot(theWeight, this->simpleRootsInner[generatorIndex]).IsNegative();
}

template <>
bool WeylGroupData::IsDominantWRTgenerator<RationalFunctionOld>(
  const Vector<RationalFunctionOld>& theWeight, int generatorIndex
) {
  Vector<Rational> tempVect;
  RationalFunctionOld tempRF;
  tempVect.MakeEi(this->GetDim(), generatorIndex);
  tempRF = this->RootScalarCartanRoot(theWeight, tempVect);
  if (tempRF.expressionType != tempRF.typeRational) {
    crash << "This might or might not be a programming mistake: "
    << "I am being asked whether a weight "
    << "with rational function coefficients is dominant. "
    << "I took the scalar products with the positive simple roots "
    << "whose reflections generate the ambient group, "
    << "however one of the scalar products in question was non-constant. "
    << "More precisely, the scalar product of "
    << theWeight.ToString() << " and " << tempVect.ToString() << " "
    << "equals " << tempRF.ToString() << ". "
    << "I cannot decide (more precisely, do not want to *silently* decide for you) "
    << "whether a non-constant function is positive or not. "
    << "If this is not a programming mistake, "
    << "you might want to consider introducing a substitution "
    << "evaluating the rational function, some sort of a monomial order, "
    << "or some other method of deciding the \"sign\" of a rational function. "
    << "Whether or not this is a mistake, I am crashing.  "
    << crash;
    return false;
  }
  return !tempRF.ratValue.IsNegative();
}

template <>
bool WeylGroupData::IsDominantWRTgenerator<Rational>(const Vector<Rational>& theWeight, int generatorIndex) {
  return !this->GetScalarProdSimpleRoot(theWeight, generatorIndex).IsNegative();
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
  this->CheckInitialization();
  std::stringstream out;
  Vector<Rational> highestWeightTrue = highestWeightSimpleCoords;
  Vectors<Rational> basisEi;
  int theDim= this->AmbientWeyl->GetDim();
  basisEi.MakeEiBasis(theDim);
  this->RaiseToDominantWeightInner(highestWeightTrue);
  Vector<Rational> highestWeightFundCoords = this->AmbientWeyl->GetFundamentalCoordinatesFromSimple(highestWeightTrue);
  if (!highestWeightFundCoords.SumCoords().IsSmallInteger()) {
    return false;
  }
  int theTopHeightSimpleCoords = (int) highestWeightSimpleCoords.SumCoords().GetDoubleValue() + 1;
  if (theTopHeightSimpleCoords < 0) {
    theTopHeightSimpleCoords = 0;
  }
  List<HashedList<Vector<Rational> > > outputWeightsByHeight;
  int topHeightRootSystem = this->AmbientWeyl->RootsOfBorel.LastObject()->SumCoords().NumShort;
  int topHeightRootSystemPlusOne = topHeightRootSystem + 1;
  outputWeightsByHeight.SetSize(topHeightRootSystemPlusOne);
  int finalHashSize = 100;
  for (int i = 0; i < topHeightRootSystemPlusOne; i ++) {
    outputWeightsByHeight[i].SetHashSizE(finalHashSize);
  }
  outputWeightsSimpleCoords.Clear();
  outputWeightsByHeight[0].AddOnTop(highestWeightTrue);
  int numTotalWeightsFound = 0;
  int numPosRoots = this->AmbientWeyl->RootsOfBorel.size;
  Vector<Rational> currentWeight, currentWeightRaisedToDominantWRTAmbientAlgebra;
  for (
    int lowestUnexploredHeightDiff = 0;
    lowestUnexploredHeightDiff <= theTopHeightSimpleCoords;
    lowestUnexploredHeightDiff ++
  ) {
    //double startCycleTime = theGlobalVariables.GetElapsedSeconds();
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
          if (currentWeightRaisedToDominantWRTAmbientAlgebra.IsNegativeOrZero()) {
            int currentIndexShift = this->AmbientWeyl->RootsOfBorel[i].SumCoords().NumShort;
            currentIndexShift = (currentIndexShift + bufferIndexShift) % topHeightRootSystemPlusOne;
            if (outputWeightsByHeight[currentIndexShift].AddOnTopNoRepetition(currentWeight)) {
              numTotalWeightsFound ++;
              outputWeightsByHeight[currentIndexShift].AdjustHashes();
            }
          }
        }
      }
    }
    outputWeightsSimpleCoords.AddOnTop(currentHashes);
    outputWeightsSimpleCoords.AdjustHashes();
    currentHashes.Clear();
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

bool Calculator::innerAnimateLittelmannPaths(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerAnimateLittelmannPaths");
  RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(3)) {
    return output.MakeError("This function takes 2 arguments", theCommands);
  }
  SemisimpleLieAlgebra* theSSowner = nullptr;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(
    CalculatorConversions::innerSSLieAlgebra, input[1], theSSowner
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  Vector<Rational> theWeight;
  Expression tempContext(theCommands);
  if (!theCommands.GetVectoR<Rational>(input[2], theWeight, &tempContext, theSSowner->GetRank(), 0)) {
    return output.MakeError("Failed to convert the argument of the function to a highest weight vector", theCommands);
  }
  Vector<Rational> theWeightInSimpleCoords;
  theWeightInSimpleCoords = theSSowner->theWeyl.GetSimpleCoordinatesFromFundamental(theWeight);
  theCommands << "<br>Function innerAnimateLittelmannPaths: your input in simple coords: "
  << theWeightInSimpleCoords.ToString();
  LittelmannPath thePath;
  thePath.MakeFromWeightInSimpleCoords(theWeightInSimpleCoords, theSSowner->theWeyl);
  return output.AssignValue(thePath.GenerateOrbitAndAnimate(), theCommands);
}

bool Calculator::innerCasimir(Calculator& theCommands, const Expression& input, Expression& output) {
  RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  SemisimpleLieAlgebra* theSSalg = 0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(
    CalculatorConversions::innerSSLieAlgebra, input, theSSalg
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  SemisimpleLieAlgebra& theSSowner = *theSSalg;
  ElementUniversalEnveloping<RationalFunctionOld> theCasimir;
  theCasimir.MakeCasimir(theSSowner);
  theCommands << "Context Lie algebra: " << theSSowner.theWeyl.theDynkinType.ToString()
  << ". The coefficient: " << theSSowner.theWeyl.GetKillingDivTraceRatio().ToString()
  <<  ". The Casimir element of the ambient Lie algebra. ";
  Expression contextE;
  contextE.MakeContextSSLieAlg(theCommands, theSSowner);
  return output.AssignValueWithContext(theCasimir, contextE, theCommands);
}

bool Calculator::innerEmbedG2inB3(Calculator& theCommands, const Expression& input, Expression& output) {
  output = input;
  if (!output.IsOfType < ElementUniversalEnveloping<RationalFunctionOld> >()) {
    return output.MakeError("Failed to convert argument to element of the Universal enveloping algebra. ", theCommands);
  }
  SemisimpleLieAlgebra& ownerSS = *output.GetAmbientSSAlgebraNonConstUseWithCaution();
  if (!ownerSS.IsOfSimpleType('G', 2)) {
    return output.MakeError("Error: embedding of G_2 in B_3 takes elements of U(G_2) as arguments.", theCommands);
  }
  HomomorphismSemisimpleLieAlgebra theHmm;
  theCommands.MakeHmmG2InB3(theHmm);

  ElementUniversalEnveloping<RationalFunctionOld> argument = output.GetValue<ElementUniversalEnveloping<RationalFunctionOld> >();
  ElementUniversalEnveloping<RationalFunctionOld> outputUE;
  if (!theHmm.ApplyHomomorphism(argument, outputUE)) {
    return output.MakeError("Failed to apply homomorphism for unspecified reason", theCommands);
  }
  outputUE.Simplify();
  Expression contextE;
  contextE.MakeContextSSLieAlg(theCommands, theHmm.theRange());
  return output.AssignValueWithContext(outputUE, contextE, theCommands);
}

std::string HtmlRoutines::GetSliderSpanStartsHidden(
  const std::string& content, const std::string& label, const std::string& desiredID
) {
  (void) label;//avoid unused parameter warning, portable
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
  if (!this->GenerateOrbit(theOrbit, theGens, 1000, 0)) {
    out  << "<b>Not all paths were genenerated, only the first " << theOrbit.size << "</b>";
  }
  Vectors<double> coxPlane;
  coxPlane.SetSize(2);
  this->owner->GetCoxeterPlane(coxPlane[0], coxPlane[1]);
  DrawingVariables animated, collapsed;
  this->owner->DrawRootSystem(animated, true, true);
  this->owner->DrawRootSystem(collapsed, true, true);
  for (int i = 0; i < theOrbit.size; i ++) {
    LittelmannPath& currentPath = theOrbit[i];
    animated.drawPath(currentPath.Waypoints, "black", 1, this->owner->theDynkinType.ToString(), i);
    collapsed.drawPath(currentPath.Waypoints, "black", 1);
    for (int j = 0; j < currentPath.Waypoints.size; j ++) {
      animated.drawCircleAtVector(currentPath.Waypoints[j], "purple", 3, this->owner->theDynkinType.ToString(), i);
      collapsed.drawCircleAtVector(currentPath.Waypoints[j], "purple", 3);
    }
  }
  out << "<br>Animation of the Littelmann paths follows. ";
  out << animated.GetHtmlFromDrawOperationsCreateDivWithUniqueName(this->owner->GetDim());
  out << "<br>Here are all Littelmann paths drawn simultaneously. ";
  out << collapsed.GetHtmlFromDrawOperationsCreateDivWithUniqueName(this->owner->GetDim());
  out << "Littelmann paths in simple coordinates given in the order in which they are generated ("
  << theOrbit.size << " total):<br>";
  out << "<table>";
  for (int i = 0; i < theOrbit.size; i ++) {
    LittelmannPath& currentPath = theOrbit[i];
    out << "<tr><td>" << currentPath.ToString() << "</td>"
    << "<td>"
    << this->ElementToStringOperatorSequenceStartingOnMe(theGens[i])
    << "</td></tr>";
  }
  out << "</table>";
  LittelmannPath lastPath = theOrbit[0];
  LittelmannPath tempPath;
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> tempMon;
  tempMon = *theGens.LastObject();
  tempMon.generatorsIndices.ReverseOrderElements();
  tempMon.Powers.ReverseOrderElements();
  out << "<table>";
  for (int i = tempMon.generatorsIndices.size - 1; i >= 1; i --) {
    int curInd = - tempMon.generatorsIndices[i] - 1;
    int nextInd = - tempMon.generatorsIndices[i - 1] - 1;
    for (int k = 0; k < tempMon.Powers[i]; k ++) {
      lastPath.ActByFalpha(curInd);
    }
    tempPath = lastPath;
    tempPath.ActByEalpha(nextInd);
    out << "<tr><td> e_" << nextInd + 1 << "(" << lastPath.ToString() << ") =</td>" << "<td>"
    << tempPath.ToString() << "</td>";
    for (int j = 0; j < this->owner->GetDim(); j ++) {
      tempPath = lastPath;
      tempPath.ActByEalpha(j);
      out << "<td> e_" << j + 1 << "("
      << lastPath.ToString() << ")=</td>" << "<td>" << tempPath.ToString() << "</td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  out << "<table><td>corresponding element of U(g)</td><td>is adapted</td>"
  << "<td>path</td><td>e operators with non-zero action.</td>";
  for (int i = 0; i < theGens.size; i ++) {
    tempPath = theOrbit[i];
    tempMon = theGens[i];
    tempMon.generatorsIndices.ReverseOrderElements();
    tempMon.Powers.ReverseOrderElements();
    bool isadapted = tempPath.IsAdaptedString(tempMon);
    out << "<tr><td>" << tempMon.ToString() << "</td><td>"
    << (isadapted ? "is adapted to" : "is not adapted to" ) << "</td><td>"
    << tempPath.ToString() << "</td><td>";
    for (int j = 0; j < this->owner->GetDim(); j ++) {
      tempPath = theOrbit[i];
      tempPath.ActByEFDisplayIndex(j + 1);
      if (!tempPath.IsEqualToZero()) {
        out << "e_{" << j + 1 << "}, ";
      }
      tempPath = theOrbit[i];
      tempPath.ActByEFDisplayIndex(- j - 1);
      if (!tempPath.IsEqualToZero()) {
        out << "e_{" << - j - 1 << "}, ";
      }
    }
    out << "</td></tr>";
  }
  out << "</table>";
  return out.str();
}

template<class coefficient>
void ModuleSSalgebra<coefficient>::SplitFDpartOverFKLeviRedSubalg(
  HomomorphismSemisimpleLieAlgebra& theHmm,
  Selection& LeviInSmall,
  List<ElementUniversalEnveloping<coefficient> >* outputEigenVectors,
  Vectors<coefficient>* outputWeightsFundCoords,
  Vectors<coefficient>* outputEigenSpace,
  std::stringstream* comments
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<coefficient>::SplitFDpartOverFKLeviRedSubalg");
  if (this->theChaR.size() != 1) {
    if (comments != 0) {
      std::stringstream out;
      out << "I have been instructed only to split modules that are irreducible over the ambient Lie algebra";
      out << " Instead I got the character " << this->theChaR.ToString()
      << " (" << this->theChaR.size() << " monomials)";
      *comments << out.str();
    }
    return;
  }
  std::string tempS;
  std::stringstream out;
  if (comments != 0) {
    out << tempS;
  }
  out << "<br>Parabolic selection: " << LeviInSmall.ToString();
  std::stringstream tempStream1;
  tempStream1 << "Started splitting the f.d. part of the " << theHmm.theRange().ToStringLieAlgebraName() << "-module with highest weight in fund coords "
  << this->theChaR[0].weightFundamentalCoordS.ToString();
  ProgressReport theReport;
  theReport.Report(tempStream1.str());
  List<List<Vector<coefficient> > > eigenSpacesPerSimpleGenerator;
  Selection InvertedLeviInSmall;
  InvertedLeviInSmall = LeviInSmall;
  InvertedLeviInSmall.InvertSelection();
  eigenSpacesPerSimpleGenerator.SetSize(InvertedLeviInSmall.CardinalitySelection);
  Vectors<coefficient> tempSpace1, tempSpace2;
  MemorySaving<Vectors<coefficient> > tempEigenVects;
  Vectors<coefficient>& theFinalEigenSpace = (outputEigenSpace == 0) ? tempEigenVects.GetElement() : *outputEigenSpace;
  theFinalEigenSpace.SetSize(0);
  if (InvertedLeviInSmall.CardinalitySelection == 0) {
    theFinalEigenSpace.MakeEiBasis(this->GetDim());
  }
  for (int i = 0; i < InvertedLeviInSmall.CardinalitySelection; i ++) {
    ElementSemisimpleLieAlgebra<Rational>& currentElt =
    theHmm.imagesSimpleChevalleyGenerators[InvertedLeviInSmall.elements[i]];
    MatrixTensor<coefficient> currentOp, tempMat;
    currentOp.MakeZero();
    for (int j = 0; j < currentElt.size(); j ++) {
      tempMat = this->GetActionGeneratorIndeX(currentElt[j].theGeneratorIndex);
      tempMat *= currentElt.theCoeffs[j];
      currentOp += tempMat;
    }
    std::stringstream tempStream3;
    double timeAtStart1 = theGlobalVariables.GetElapsedSeconds();
    tempStream3 << "Computing eigenspace corresponding to " << currentElt.ToString() << "...";
    theReport.Report(tempStream3.str());
    Matrix<coefficient> currentOpMat;
    currentOp.GetMatrix(currentOpMat, this->GetDim());
    currentOpMat.GetZeroEigenSpace(eigenSpacesPerSimpleGenerator[i]);
    tempStream3 << " done in " << theGlobalVariables.GetElapsedSeconds() - timeAtStart1 << " seconds. ";
    theReport.Report(tempStream3.str());
    if (i == 0) {
      theFinalEigenSpace = eigenSpacesPerSimpleGenerator[i];
    } else {
      std::stringstream tempStream4;
      double timeAtStart2 = theGlobalVariables.GetElapsedSeconds();
      tempStream4 << "Intersecting with eigenspace corresponding to " << currentElt.ToString() << "...";
      tempSpace1 = theFinalEigenSpace;
      theReport.Report(tempStream4.str());
      tempSpace2 = eigenSpacesPerSimpleGenerator[i];
      theFinalEigenSpace.IntersectTwoLinSpaces(tempSpace1, tempSpace2, theFinalEigenSpace);
      tempStream4 << " done in " << theGlobalVariables.GetElapsedSeconds() - timeAtStart2 << " seconds. ";
      theReport.Report(tempStream4.str());
    }
  }
  out << "<br>Eigenvectors: <table>";
  std::stringstream readyForLatexComsumption;
  readyForLatexComsumption << "\\begin{tabular}{|lll|}\n <br>";
  readyForLatexComsumption << "\\hline\\multicolumn{3}{|c|}{Highest weight $"
  << this->theHWFundamentalCoordsBaseField.ToStringLetterFormat("\\omega") << "$}\\\\\n<br>";
  readyForLatexComsumption << "weight fund. coord.& singular vector \\\\\\hline\n<br>";
  Vector<coefficient> currentWeight;
  Vector<coefficient> hwFundCoordsNilPart;
  hwFundCoordsNilPart = this->theHWFundamentalCoordsBaseField;
  hwFundCoordsNilPart -= this->theHWFDpartFundamentalCoordS;
  ElementUniversalEnveloping<coefficient> currentElt, tempElt;
  if (outputEigenVectors != 0) {
    outputEigenVectors->SetSize(0);
  }
  for (int j = 0; j < theFinalEigenSpace.size; j ++) {
    out << "<tr><td>";
    currentElt.MakeZero(this->GetOwner());
    Vector<coefficient>& currentVect = theFinalEigenSpace[j];
    int lastNonZeroIndex = - 1;
    for (int i = 0; i < currentVect.size; i ++) {
      if (!(currentVect[i].IsEqualToZero())) {
        tempElt.MakeZero(this->GetOwner());
        tempElt.AddMonomial(this->theGeneratingWordsNonReduced[i], 1);
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
    if (outputEigenVectors != 0) {
      outputEigenVectors->AddOnTop(currentElt);
    }
    if (outputWeightsFundCoords != 0) {
      outputWeightsFundCoords->AddOnTop(currentWeight);
    }
    out << currentElt.ToString(&theGlobalVariables.theDefaultFormat.GetElement());
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
  theReport.Report("SplitFDpartOverFKLeviRedSubalg done!");
  if (comments != 0) {
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
  output.imagesSimpleChevalleyGenerators.SetSize(4);
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
  Vectors<RationalFunctionOld>& theHWs,
  branchingData& theG2B3Data,
  Expression& theContext
) {
  MacroRegisterFunctionWithName("Calculator::innerPrintB3G2branchingIntermediate");
  (void) input;//avoid unused parameter warning, portable
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
    << theG2B3Data.selInducing.ToString() << "-parabolic subalgebra"
    << " and let " << HtmlRoutines::GetMathSpanPure("{p}'= p\\cap G_2")
    << ". Then  " << HtmlRoutines::GetMathSpanPure("{p}'") << " is the "
    << theG2B3Data.selSmallParSel.ToString() << "- parabolic subalgebra of G_2"
    << "<br> <table border =\"1\"><tr><td>$so(7)$-highest weight</td>"
    << "<td>character difference from top</td>"
    << "<td>Decomposition of inducing module over "
    << HtmlRoutines::GetMathSpanPure("p'")
    << "</td><td>" << HtmlRoutines::GetMathSpanPure("p'\\cap b")
    << "-eigenvectors</td><td>Casimir projector</td><td>Extra multiplier</td><td>corresponding "
    << HtmlRoutines::GetMathSpanPure("G_2\\cap b")
    << "-eigenvectors</td><td>Shapovalov square</td></tr>";
    latexTable << "\\begin{longtable}{|cccclll|} \\caption{\\label{tableB3fdsOverG2charsAndHWV"
    << theG2B3Data.selInducing.ToString() << "} "
    << "Decomposition of inducing $" << theG2B3Data.selInducing.ToString() << "$-$\\mathfrak p$-module over $"
    << theG2B3Data.selSmallParSel.ToString() << "$-$\\mathfrak p'=\\mathfrak p\\cap G_2$}\\\\"
    << "\\hline $\\mathfrak p-inducing$& dim. &$\\mathfrak p' decomp. $&dim.&"
    << " $\\mathfrak b \\cap \\mathfrak p'$-singular vectors & Casimir projector "
    << "& Corresp. $\\mathfrak b \\cap G_2$-singular vectors  \\\\ \\hline"
    << "\\endhead \n<br>";
  }
  theContext.ContextGetFormatExpressions(theG2B3Data.theFormat);
  theG2B3Data.theFormat.flagUseLatex = true;
  theG2B3Data.theFormat.NumAmpersandsPerNewLineForLaTeX = 0;
  Expression tempExpression;
  RationalFunctionOld rfZero, rfOne;
  rfZero.MakeZero();
  rfOne.MakeOne();
  latexTable2 << "\\begin{longtable}{|rll|}\\caption"
  << "{Values of $x_1$ for each $v_{\\lambda,i}$}\\label{tableCriticalValuesvlambda}"
  << "\\endhead";
  for (int i = 0; i < theHWs.size; i ++) {
    theG2B3Data.theWeightFundCoords = theHWs[i];
    theCommands.innerSplitFDpartB3overG2inner(theCommands, theG2B3Data, tempExpression);
    timeReport << tempExpression.GetValue<std::string>();
    RationalFunctionOld numEigenVectors;
    numEigenVectors = rfZero;
    for (int j = 0; j < theG2B3Data.theSmallCharFDpart.size(); j ++) {
      numEigenVectors += theG2B3Data.theSmallCharFDpart.theCoeffs[j];
    }
    theG2B3Data.theFormat.CustomPlusSign = "";
    int eigenIndexcounter = 0;
    if (i != 0) {
      latexTable2 << "\\hline\\multicolumn{3}{|c|}{$\\lambda ="
      << theG2B3Data.theWeightFundCoords.ToStringLetterFormat("\\omega", &theG2B3Data.theFormat)
      << "$}\\\\vector& coefficient of $v_\\lambda$ in $Sh_{\\lambda,i}$ &$x_1\\notin$ \\\\\\hline";
    }
    for (int k = 0; k < theG2B3Data.theSmallCharFDpart.size(); k ++) {
      charSSAlgMod<RationalFunctionOld> tempChar;
      tempChar.AddMonomial(theG2B3Data.theSmallCharFDpart[k], theG2B3Data.theSmallCharFDpart.theCoeffs[k]);
      int multiplicity = 0;
      theG2B3Data.theSmallCharFDpart.theCoeffs[k].IsSmallInteger(&multiplicity);
      for (int counter = 0; counter < multiplicity; counter ++, eigenIndexcounter ++) {
        out << "<tr>";
        if (k == 0 && counter == 0) {
          theG2B3Data.theFormat.CustomPlusSign = "\\oplus ";
          theG2B3Data.theFormat.fundamentalWeightLetter = "\\omega";
          out << "<td rowspan =\"" << numEigenVectors.ToString() << "\">" << theG2B3Data.theAmbientChar.ToString(&theG2B3Data.theFormat) << "</td> ";
          if (!isFD) {
            out << "<td rowspan =\"" << numEigenVectors.ToString() << "\">";
            for (int charcounter1 = 0; charcounter1 < theG2B3Data.theCharacterDifferences.size; charcounter1 ++) {
              std::string tempS = theG2B3Data.theFormat.CustomPlusSign;
              theG2B3Data.theFormat.CustomPlusSign = "";
              out << "A_{" << charcounter1 << "}=" << theG2B3Data.theCharacterDifferences[charcounter1].ToString(&theG2B3Data.theFormat) << ";";
              if (charcounter1 != theG2B3Data.theCharacterDifferences.size - 1) {
                out << "<br>";
              }
              theG2B3Data.theFormat.CustomPlusSign = tempS;
            }
            out << "</td> ";
          }
          latexTable << "\\multirow{" << theG2B3Data.theEigenVectorS.size  << "}{*}{$"
          << theG2B3Data.theAmbientChar.ToString(&theG2B3Data.theFormat) << "$}";
          Vector<RationalFunctionOld> theSimpleCoordinates;
          theSimpleCoordinates = theG2B3Data.WeylFD.AmbientWeyl->GetSimpleCoordinatesFromFundamental(
            theG2B3Data.theAmbientChar[0].weightFundamentalCoordS
          );
          RationalFunctionOld theWeylSize;
          theWeylSize = theG2B3Data.WeylFD.WeylDimFormulaInnerSimpleCoords(theSimpleCoordinates);
          latexTable << "& \\multirow{" << theG2B3Data.theEigenVectorS.size  << "}{*}{$"
          << theWeylSize.ToString(&theG2B3Data.theFormat)
          << "$}";
        } else {
          latexTable << "&";
        }
        latexTable << " & ";
        if (counter == 0) {
          theG2B3Data.theFormat.fundamentalWeightLetter = "\\psi";
          theG2B3Data.theFormat.CustomPlusSign = "\\oplus ";
          out << "<td rowspan =\"" << multiplicity << " \">" << tempChar.ToString(&theG2B3Data.theFormat) << "</td>";
          latexTable << "\\multirow{" << multiplicity  << "}{*}{$";
          latexTable << tempChar.ToString(&theG2B3Data.theFormat) << "$}";
          latexTable << "&\\multirow{" << multiplicity  << "}{*}{$";
          if (multiplicity > 1) {
            latexTable << multiplicity << "\\times";
          }
          Vector<RationalFunctionOld> theSimpleCoordinates;
          theSimpleCoordinates = theG2B3Data.WeylFDSmall.AmbientWeyl->GetSimpleCoordinatesFromFundamental(
            tempChar[0].weightFundamentalCoordS
          );
          RationalFunctionOld dimension;
          dimension = theG2B3Data.WeylFDSmall.WeylDimFormulaInnerSimpleCoords(theSimpleCoordinates, rfOne);
          latexTable << dimension.ToString(&theG2B3Data.theFormat) << "$}";
        } else {
          latexTable << "&";
        }
        latexTable << "&";
        theG2B3Data.theFormat.CustomPlusSign = "";
        out << "<td>" << HtmlRoutines::GetMathSpanPure(theG2B3Data.theEigenVectorsLevi[eigenIndexcounter].ToString(&theG2B3Data.theFormat)) << "</td>";
        theG2B3Data.theFormat.MaxLineLength = 20;
        latexTable << "$\\begin{array}{l}" << theG2B3Data.theEigenVectorsLevi[eigenIndexcounter].ToString(&theG2B3Data.theFormat) << "\\end{array}$ \n";
        if (!isFD) {
          std::string tempS1 = theG2B3Data.GetStringCasimirProjector(eigenIndexcounter, 12);
          std::string tempS2 = "("+ theG2B3Data.theUEelts[eigenIndexcounter].ToString(&theG2B3Data.theFormat) + ")\\cdot v_\\lambda";
          out << "<td>" << HtmlRoutines::GetMathSpanPure(tempS1) << "</td>";
          out << "<td>" << theG2B3Data.additionalMultipliers[eigenIndexcounter].ToString() << "</td>";
          out << "<td>" << HtmlRoutines::GetMathSpanPure(tempS2) << "</td>";
          out << "<td>" << theG2B3Data.theShapovalovProducts[eigenIndexcounter].ToString(&theG2B3Data.theFormat);
          out << "</td>";
          int theIndex = - 1;
          numEigenVectors.IsSmallInteger(&theIndex);
          if (theIndex - eigenIndexcounter - 1 > 0) {
            List<Rational> tempList, tempList2;
            latexTable2 << " $v_{\\lambda," <<  theIndex- eigenIndexcounter - 1 << "} $&";
            Polynomial<Rational> tempP;
            theG2B3Data.theShapovalovProducts[eigenIndexcounter].GetNumerator(tempP);
            tempP.ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy();
            latexTable2 << "$\\begin{array}{l}" << tempP.ToString(&theG2B3Data.theFormat) << "\\end{array}$ & ";
            if (tempP.FindOneVarRatRoots(tempList)) {
              tempList2.AddOnTopNoRepetition(tempList);
              out << "<td>Rational roots: " << tempList.ToString() << "</td>";
              latexTable2 << tempList2.ToString();
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
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerPrintB3G2branchingTable(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerPrintB3G2branchingTable");
  Vectors<RationalFunctionOld> theHWs;
  branchingData theG2B3Data;
  Expression theContext(theCommands);
  if (!theCommands.innerPrintB3G2branchingTableCommon(theCommands, input, output, theHWs, theG2B3Data, theContext)) {
    return false;
  }
  if (output.IsError()) {
    return true;
  }
  return theCommands.innerPrintB3G2branchingIntermediate(theCommands, input, output, theHWs, theG2B3Data, theContext);
}

bool Calculator::innerPrintB3G2branchingTableCharsOnly(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerPrintB3G2branchingTableCharsOnly");
  branchingData theg2b3data;
  Expression theContext(theCommands);
  Vectors<RationalFunctionOld> theHWs;
  theCommands.innerPrintB3G2branchingTableCommon(
    theCommands, input, output, theHWs, theg2b3data, theContext
  );
  if (output.IsError()) {
    return true;
  }
  charSSAlgMod<RationalFunctionOld> theCharacter, outputChar;
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
    << theg2b3data.selInducing.ToString() << "-parabolic subalgebra"
    << " and let " << HtmlRoutines::GetMathSpanPure("{p}'= p\\cap G_2")
    << ". Then  " << HtmlRoutines::GetMathSpanPure("{p}'") << " is the "
    << theg2b3data.selSmallParSel.ToString() << "- parabolic subalgebra of G_2"
    << "<br> <table><tr><td>$so(7)$-highest weight</td>"
    << "<td>Dimension of inducing fin. dim. "
    << HtmlRoutines::GetMathSpanPure(" p")
    << "-module</td><td>Decomposition of inducing module over "
    << HtmlRoutines::GetMathSpanPure("p'")
    << "</td><td>Dimensions</td>"
    << " <td>Highest weight <br> is sufficiently generic <br> if none of <br>the following vanish</td>"
    << "</tr>";
    latexTable << "\\begin{longtable}{|p{2cm}l|} \\caption{\\label{tableB3fdsOverG2charsonly"
    << theg2b3data.selInducing.ToString() << "} "
    << "Decompositions of inducing $\\mathfrak{p}" << "_{"
    << theg2b3data.selInducing.ToString()
    << "}" << "$-modules over $\\bar{ \\mathfrak {l}}$"
    << ", where $\\mathfrak{l}$ is the reductive Levi part of "
    << "$\\mathfrak{p}" << "_{" << theg2b3data.selInducing.ToString()
    <<  "}$ and $\\bar{\\mathfrak {l}}$ is the reductive Levi part of $\\bar{\\mathfrak {p}} _{"
    << theg2b3data.selSmallParSel.ToString()  << "}$" << " } \\\\"
    << "\\hline $V_\\lambda(\\mathfrak l)$ " << "& Decomposition over $\\bar { \\mathfrak l}$ "
    << "\\endhead \\hline\n<br>";
  }
  theg2b3data.theFormat.flagUseLatex = true;
  ElementUniversalEnveloping<RationalFunctionOld> theCasimir, theCentralCharacter, resultChar;
  HashedList<ElementUniversalEnveloping<RationalFunctionOld> > theCentralChars;
  theCasimir.MakeCasimir(theg2b3data.theHmm.theDomain());
  WeylGroupData& smallWeyl = theg2b3data.theHmm.theDomain().theWeyl;
  for (int k = 0; k < theHWs.size; k ++) {
    theCharacter.MakeFromWeight(
      theg2b3data.theHmm.theRange().theWeyl.GetSimpleCoordinatesFromFundamental(theHWs[k]),
      &theg2b3data.theHmm.theRange()
    );
    theCharacter.SplitCharOverRedSubalg(0, outputChar, theg2b3data);
    theg2b3data.theFormat.fundamentalWeightLetter = "\\omega";
    out << "<tr><td> " << theCharacter.ToString(&theg2b3data.theFormat) << "</td> ";
    Vector<RationalFunctionOld> simpleCoordinates;
    simpleCoordinates = theg2b3data.WeylFD.AmbientWeyl->GetSimpleCoordinatesFromFundamental(
      theCharacter[0].weightFundamentalCoordS
    );
    RationalFunctionOld dimension;
    dimension = theg2b3data.WeylFD.WeylDimFormulaInnerSimpleCoords(simpleCoordinates);
    out << "<td>" << dimension.ToString() << "</td>";
    latexTable << " $ " << theCharacter.ToString(&theg2b3data.theFormat) << " $ ";
    theg2b3data.theFormat.fundamentalWeightLetter = "\\psi";
    out << "<td>" << outputChar.ToString(&theg2b3data.theFormat) << "</td>";
    out << "<td>";
    theg2b3data.theFormat.CustomPlusSign = "\\oplus ";
    Vector<RationalFunctionOld> leftWeightSimple, leftWeightDual, rightWeightSimple, rightWeightDual;
    theCentralChars.Clear();
    for (int i = 0; i < outputChar.size(); i ++) {
      if (!outputChar.theCoeffs[i].IsEqualToOne()) {
        out << outputChar.theCoeffs[i].ToString() << " x ";
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
        if ((rightWeightSimple - leftWeightSimple).IsPositive()) {
          resultChar = theCasimir;
          theCentralCharacter = theCasimir;
          resultChar.ModOutVermaRelations(&rightWeightDual);
          theCentralCharacter.ModOutVermaRelations(&leftWeightDual);
          resultChar -= theCentralCharacter;
          resultChar.ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy();
          resultChar *= - 1;
          theCentralChars.AddOnTopNoRepetition(resultChar);
        }
      }
    }
    out << "</td>";
    theg2b3data.theFormat.MaxLineLength = 80;
    latexTable << "& $\\begin{array}{l} " << outputChar.ToString(&theg2b3data.theFormat) << "\\end{array} $ ";
    theg2b3data.theFormat.CustomPlusSign = "";
    if (!isFD && theCentralChars.size > 0) {
      out << "<td>";
      latexTable << " \\\\\n<br> ";
      latexTable << "\\multicolumn{2}{|p{11.6cm}|}{ \\tiny Strong Condition B: ";
      for (int l = 0; l < theCentralChars.size; l ++) {
        out << theCentralChars[l].ToString(&theg2b3data.theFormat) << "<br> ";
        latexTable << "$" << theCentralChars[l].ToString(&theg2b3data.theFormat) << "\\neq 0$";
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
  return output.AssignValue(out.str(), theCommands);
}

void branchingData::resetOutputData() {
  this->theEigenVectorS.SetSize(0);
  this->theEigenVectorsLevi.SetSize(0);
  this->outputEigenWords.SetSize(0);
  this->g2Weights.SetSize(0);
  this->outputWeightsFundCoordS.SetSize(0);
  this->theCharacterDifferences.Clear();
}

template<class coefficient>
bool ElementSumGeneralizedVermas<coefficient>::ExtractElementUE(
  ElementUniversalEnveloping<coefficient>& output, SemisimpleLieAlgebra& theOwner
) {
  output.MakeZero(theOwner);
  ModuleSSalgebra<coefficient>* theModPtr = 0;
  MonomialUniversalEnveloping<coefficient> tempMon;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialGeneralizedVerma<coefficient>& currentMon = (*this)[i];
    if (i == 0) {
      theModPtr = currentMon.owner;
    }
    if (currentMon.owner != theModPtr) {
      return false;
    }
    tempMon = currentMon.theMonCoeffOne;
    tempMon *= currentMon.GetOwner().theGeneratingWordsNonReduced[currentMon.indexFDVector];
    output.AddMonomial(tempMon, this->theCoeffs[i]);
  }
  return true;
}

bool Calculator::innerSplitFDpartB3overG2inner(Calculator& theCommands, branchingData& theG2B3Data, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerSplitFDpartB3overG2inner");
  ModuleSSalgebra<RationalFunctionOld> theModCopy;
  theModCopy.MakeFromHW(
    theG2B3Data.theHmm.theRange(), theG2B3Data.theWeightFundCoords, theG2B3Data.selInducing, 1, 0, 0, false
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

  theCommands.theObjectContainer.theCategoryOmodules.AddNoRepetitionOrReturnIndexFirst(theModCopy);
  int theModIndex = theCommands.theObjectContainer.theCategoryOmodules.GetIndex(theModCopy);
  ModuleSSalgebra<RationalFunctionOld>& theMod = theCommands.theObjectContainer.theCategoryOmodules[theModIndex];
  theMod.GetOwner().flagHasNilradicalOrder = true;
  std::stringstream out;
  theCommands << "<hr>Time elapsed before making B3 irrep: " << theGlobalVariables.GetElapsedSeconds();
  double timeAtStart = theGlobalVariables.GetElapsedSeconds();
  theMod.SplitFDpartOverFKLeviRedSubalg(
    theG2B3Data.theHmm,
    theG2B3Data.selSmallParSel,
    &theG2B3Data.outputEigenWords,
    &theG2B3Data.outputWeightsFundCoordS,
    &theG2B3Data.leviEigenSpace,
    0
  );
  theCommands << "<br>Time needed to make B3 irrep: " << theGlobalVariables.GetElapsedSeconds() - timeAtStart;
  theG2B3Data.g2Weights.SetSize(theG2B3Data.outputWeightsFundCoordS.size);
  theG2B3Data.g2DualWeights.SetSize(theG2B3Data.outputWeightsFundCoordS.size);
  Matrix<Rational> invertedG2cartanMat;
  invertedG2cartanMat = theG2B3Data.theHmm.theDomain().theWeyl.CartanSymmetric;
  invertedG2cartanMat.Invert();
  WeylGroupData& rangeWeyl = theG2B3Data.theHmm.theRange().theWeyl;
  theG2B3Data.outputWeightsSimpleCoords = rangeWeyl.GetSimpleCoordinatesFromFundamental(theG2B3Data.outputWeightsFundCoordS);
  Vector<RationalFunctionOld> weightSimpleCoordinates;
  weightSimpleCoordinates = rangeWeyl.GetSimpleCoordinatesFromFundamental(
    theG2B3Data.theWeightFundCoords
  );
  theG2B3Data.theAmbientChar.MakeFromWeight(weightSimpleCoordinates, &theG2B3Data.theHmm.theRange());
  theG2B3Data.theSmallCharFDpart.MakeZero();
  charSSAlgMod<RationalFunctionOld> tempMon;
  for (int i = 0; i < theG2B3Data.outputWeightsSimpleCoords.size; i ++) {
    Vector<RationalFunctionOld>& currentWeight = theG2B3Data.outputWeightsSimpleCoords[i];
    Vector<RationalFunctionOld>& currentG2Weight = theG2B3Data.g2Weights[i];
    Vector<RationalFunctionOld>& currentG2DualWeight = theG2B3Data.g2DualWeights[i];
    currentG2DualWeight.SetSize(2);
    currentG2DualWeight[0] = theG2B3Data.theHmm.theRange().theWeyl.RootScalarCartanRoot(
      currentWeight, theG2B3Data.theHmm.ImagesCartanDomain[0]
    );
    //<-note: implicit type conversion: the return type is the left coefficient type.
    currentG2DualWeight[1] = theG2B3Data.theHmm.theRange().theWeyl.RootScalarCartanRoot(
      currentWeight, theG2B3Data.theHmm.ImagesCartanDomain[1]
    );
    //<-note: implicit type conversion: the return type is the left coefficient type.
    invertedG2cartanMat.ActOnVectorColumn(currentG2DualWeight, currentG2Weight);//<-g2weight is now computed;
    tempMon.MakeFromWeight(currentG2Weight, &theG2B3Data.theHmm.theDomain());
    theG2B3Data.theSmallCharFDpart += tempMon;
  }
  ElementUniversalEnveloping<RationalFunctionOld> theG2Casimir, theG2CasimirCopy, imageCasimirInB3, tempElt;
  theG2Casimir.MakeCasimir(theG2B3Data.theHmm.theDomain());

  theG2B3Data.theChars.SetSize(theG2B3Data.outputWeightsFundCoordS.size);
  for (int i = 0; i < theG2B3Data.outputWeightsSimpleCoords.size; i ++) {
    Vector<RationalFunctionOld>& currentG2DualWeight = theG2B3Data.g2DualWeights[i];
    theG2CasimirCopy = theG2Casimir;
    theG2CasimirCopy.ModOutVermaRelations(&currentG2DualWeight, 1, 0);
    if (theG2CasimirCopy.IsEqualToZero()) {
      theG2B3Data.theChars[i] = 0;
    } else {
      theG2B3Data.theChars[i] = theG2CasimirCopy.theCoeffs[0];
    }
  }
  theG2B3Data.theEigenVectorsLevi.SetSize(theG2B3Data.g2Weights.size);
  theG2B3Data.theEigenVectorS.SetSize(theG2B3Data.g2Weights.size);
  theG2B3Data.additionalMultipliers.SetSize(theG2B3Data.g2Weights.size);
  theG2B3Data.theShapovalovProducts.SetSize(theG2B3Data.g2Weights.size);
  theG2B3Data.theUEelts.SetSize(theG2B3Data.g2Weights.size);
  ElementSumGeneralizedVermas<RationalFunctionOld>& theHWV = *theG2B3Data.theEigenVectorsLevi.LastObject();
  theHWV.MakeHWV(theMod, 1);
  theHWV *= - 1;
  *theG2B3Data.theEigenVectorS.LastObject() = theHWV;
  Vector<RationalFunctionOld> weightDifference;
  theG2B3Data.theHmm.ApplyHomomorphism(theG2Casimir, imageCasimirInB3);
  theG2Casimir.CheckConsistency();
  imageCasimirInB3.CheckConsistency();
  RationalFunctionOld charDiff;
  theG2B3Data.theHmm.theRange().OrderNilradical(
    theMod.parabolicSelectionNonSelectedAreElementsLevi,
    theG2B3Data.flagUseNilWeightGeneratorOrder,
    theG2B3Data.flagAscendingGeneratorOrder
  );
  for (int k = 0; k < theG2B3Data.g2Weights.size; k ++) {
    ElementSumGeneralizedVermas<RationalFunctionOld>& currentTensorEltLevi = theG2B3Data.theEigenVectorsLevi[k];
    ElementSumGeneralizedVermas<RationalFunctionOld>& currentTensorEltEigen = theG2B3Data.theEigenVectorS[k];
    ElementUniversalEnveloping<RationalFunctionOld>& currentUEelt = theG2B3Data.theUEelts[k];
    currentTensorEltLevi = theHWV;
    currentTensorEltLevi.MultiplyMeByUEEltOnTheLeft(theG2B3Data.outputEigenWords[k]);
    currentTensorEltEigen = currentTensorEltLevi;
    if (theG2B3Data.selInducing.CardinalitySelection > 0) {
      for (int j = 0; j < theG2B3Data.g2Weights.size; j ++) {
        weightDifference = theG2B3Data.g2Weights[j] - theG2B3Data.g2Weights[k];
        if (weightDifference.IsPositive()) {
          theG2CasimirCopy = imageCasimirInB3;
          tempElt.MakeConst(theG2B3Data.theChars[j], theG2B3Data.theHmm.theRange());
          theG2CasimirCopy -= tempElt;
          theG2CasimirCopy *= 12;
          currentTensorEltEigen.MultiplyMeByUEEltOnTheLeft(theG2CasimirCopy);
          charDiff = theG2B3Data.theChars[j];
          charDiff -= *theG2B3Data.theChars.LastObject();
          theG2B3Data.theCharacterDifferences.AddOnTopNoRepetition(charDiff);
        }
      }
    }
    theG2B3Data.additionalMultipliers[k] = currentTensorEltEigen.ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy();
    currentTensorEltEigen.ExtractElementUE(currentUEelt, *theMod.owner);
    currentUEelt.HWTAAbilinearForm(
      currentUEelt, theG2B3Data.theShapovalovProducts[k], &theMod.theHWDualCoordsBaseFielD, 1, 0, 0
    );
  }
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerJacobiSymbol(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerJacobiSymbol");
  crash << "Function not implemented yet." << crash;
  (void) theCommands;//avoid unused parameter warning, portable
  (void) output;
  return false;
  if (input.children.size != 3) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  int leftInt, rightInt;
  if (!leftE.IsSmallInteger(&leftInt) || !rightE.IsSmallInteger(&rightInt)) {
    return false;
  }
  return true;
}

bool Calculator::innerPrintAllVectorPartitions(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerPrintAllVectorPartitions");
  RecursionDepthCounter theRecursion(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(3)) {
    return output.MakeError("Function innerPrintAllPartitions expects 2 arguments.", theCommands);
  }

  SemisimpleLieAlgebra* theSSowner;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(
    CalculatorConversions::innerSSLieAlgebra, input[1], theSSowner
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  SemisimpleLieAlgebra& theSSalgebra = *theSSowner;
  Expression theContext;
  Vector<Rational> theHW;
  if (!theCommands.GetVectoR<Rational>(input[2], theHW, &theContext, theSSalgebra.GetRank())) {
    return output.MakeError("Failed to extract weight you want partitioned from " + input[2].ToString(), theCommands);
  }
  Vector<int> theHWint;
  theHWint.SetSize(theHW.size);
  for (int i = 0; i < theHW.size; i ++) {
    if (!theHW[i].IsSmallInteger(&theHWint[i]) || theHW[i] < 0) {
      return output.MakeError(
        "The input weight you gave is bad: it must consist of non-negative small integers", theCommands
      );
    }
  }
  std::stringstream out;
  out << "<br>the weight you want partitioned: " << theHWint;
  Vector<int> thePartition;
  thePartition.SetSize(theSSalgebra.GetNumPosRoots());
  for (int i = 0; i < thePartition.size; i ++) {
    thePartition[i] = 0;
  }
  Vector<Rational> theWeight, tmpWt;
  Vectors<Rational>& rootsBorel = theSSalgebra.theWeyl.RootsOfBorel;
  int counter = 0;
  int totalCycles = 0;
  theWeight.MakeZero(theSSalgebra.GetRank());
  int i = rootsBorel.size;
  while (i > 0 && counter < 10000) {
    totalCycles ++;
    if (theWeight == theHW) {
      tmpWt = thePartition;
      out << "<br>" << tmpWt.ToStringLetterFormat("\\alpha");
      counter ++;
    }
    if (!(theHW - theWeight).IsPositive() || i > rootsBorel.size) {
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
  return output.AssignValue(out.str(), theCommands);
}

void WeylGroupData::GetHighestWeightsAllRepsDimLessThanOrEqualTo(
  List<Vector<Rational> >& outputHighestWeightsFundCoords, int inputDimBound
) {
  if (inputDimBound < 1) {
    outputHighestWeightsFundCoords.SetSize(0);
    return;
  }
  HashedList<Vector<Rational> > output;
  Vector<Rational> current;
  current.MakeZero(this->GetDim());
  output.AddOnTop(current);
  Rational theDim;
  Rational dimBound = inputDimBound + 1;
  for (int i = 0; i < output.size; i ++) {
    current = output[i];
    for (int k = 0; k < this->GetDim(); k ++) {
      current[k] += 1;
      theDim = this->WeylDimFormulaFundamentalCoords(current);
      if (theDim < dimBound) {
        output.AddOnTopNoRepetition(current);
      }
      current[k] -= 1;
    }
  }
  outputHighestWeightsFundCoords = output;
}

bool Calculator::innerTestMonomialBaseConjecture(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerTestMonomialBaseConjecture");
  RecursionDepthCounter theRecursion(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(3)) {
    return output.MakeError("innerTestMonomialBaseConjecture takes two arguments as input", theCommands);
  }
  const Expression& rankE = input[1];
  const Expression& dimE = input[2];
  int rankBound = 0;
  int dimBound = 0;
  if (!rankE.IsSmallInteger(&rankBound) || !dimE.IsSmallInteger(&dimBound)) {
    return output.MakeError("The rank and  dim bounds must be small integers", theCommands);
  }
  if (rankBound < 2 || rankBound > 100 || dimBound < 1 || dimBound > 10000) {
    return output.MakeError(
      "The rank bound must be an integer between 2 and 100, "
      "and the dim bound must be an integer between 1 and 10000",
      theCommands
    );
  }
  std::stringstream out;
  List<int> theRanks;
  List<char> theWeylLetters;
  for (int i = 2; i <= rankBound; i ++) {
    theRanks.AddOnTop(i);
    theWeylLetters.AddOnTop('A');
    theRanks.AddOnTop(i);
    theWeylLetters.AddOnTop('B');
    theRanks.AddOnTop(i);
    theWeylLetters.AddOnTop('C');
    if (i >= 4) {
      theRanks.AddOnTop(i);
      theWeylLetters.AddOnTop('D');
    }
    if (i >= 6 && i <= 8) {
      theRanks.AddOnTop(i);
      theWeylLetters.AddOnTop('E');
    }
    if (i == 4) {
      theRanks.AddOnTop(i);
      theWeylLetters.AddOnTop('F');
    }
    if (i == 2) {
      theRanks.AddOnTop(i);
      theWeylLetters.AddOnTop('G');
    }
  }
  List<List<Vector<Rational> > > theHighestWeights;
  theHighestWeights.SetSize(theRanks.size);
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
    tempSel.init(theRanks[i]);
    for (int j = 0; j < theHws.size; j ++) {
      std::stringstream reportStream;
      Vector<Rational>& currentHW = theHws[j];
      out << "<tr><td> " << currentHW.ToString() << "</td><td>"
      << currentAlg.theWeyl.WeylDimFormulaFundamentalCoords(currentHW) << "</td>";
      reportStream << "Processing " << currentAlg.ToStringLieAlgebraName() << ", index  "
      << i + 1 << " out of " << theRanks.size << ",  highest weight "
      << currentHW.ToString() << ", dim: " << currentAlg.theWeyl.WeylDimFormulaFundamentalCoords(currentHW)
      << ", index " << j + 1 << " out of " << theHws.size;
      theReport.Report(reportStream.str());
      latexReport << "$" << currentHW.ToStringLetterFormat("\\omega") << "$ &"
      << currentAlg.theWeyl.WeylDimFormulaFundamentalCoords(currentHW) << "&";
      hwPath.MakeFromWeightInSimpleCoords(
        currentAlg.theWeyl.GetSimpleCoordinatesFromFundamental(currentHW), currentAlg.theWeyl
      );
      hwPath.GenerateOrbit(
        tempList,
        theStrings,
        MathRoutines::Minimum(1000, currentAlg.theWeyl.WeylDimFormulaFundamentalCoords(currentHW).NumShort), 0
      );
      reportStream << "\nPath orbit size = " << theStrings.size
      << " generated in " << theGlobalVariables.GetElapsedSeconds() << " seconds. ";
      theReport.Report(reportStream.str());
      for (int k = 0; k < theStrings.size; k ++) {
        LittelmannPath& currentPath = tempList[k];
        tempMon = theStrings[k];
        tempMon.generatorsIndices.ReverseOrderElements();
        tempMon.Powers.ReverseOrderElements();
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
        theReport.Report("BAD BAD BAD!!!");
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
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerLittelmannOperator(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerLittelmannOperator");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (input.HasBoundVariables()) {
    return false;
  }
  int theIndex = 0;
  if (!input.IsSmallInteger(&theIndex)) {
    return output.MakeError(
      "The argument of the Littelmann root operator is expected to be a small integer, instead you gave me" + input.ToString(),
      theCommands
    );
  }
  if (theIndex == 0) {
    return output.MakeError("The index of the Littelmann root operator is expected to be non-zero", theCommands);
  }
  return output.AssignValue(theIndex, theCommands);
}

bool Calculator::innerLSPath(Calculator& theCommands, const Expression& input, Expression& output) {
  RecursionDepthCounter theRecutionIncrementer(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("Calculator::innerLSPath");
  if (input.children.size < 3) {
    return output.MakeError("LSPath needs at least two arguments.", theCommands);
  }
  SemisimpleLieAlgebra* theSSowner;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(
    CalculatorConversions::innerSSLieAlgebra, input[1], theSSowner
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  SemisimpleLieAlgebra& ownerSSalgebra = *theSSowner;
  Vectors<Rational> waypoints;
  waypoints.SetSize(input.children.size - 2);
  for (int i = 2; i < input.children.size; i ++) {
    if (!theCommands.GetVectoR<Rational>(input[i], waypoints[i - 2], 0, ownerSSalgebra.GetRank(), 0)) {
      return output.MakeError("Failed to extract waypoints", theCommands);
    }
  }
  waypoints = ownerSSalgebra.theWeyl.GetSimpleCoordinatesFromFundamental(waypoints);
  LittelmannPath theLSpath;
  theLSpath.MakeFromWaypoints(waypoints, ownerSSalgebra.theWeyl);
  return output.AssignValue(theLSpath, theCommands);
}

template <class coefficient>
void Polynomial<coefficient>::Interpolate(
  const Vector<coefficient>& thePoints, const Vector<coefficient>& ValuesAtThePoints
) {
  Polynomial<coefficient> theLagrangeInterpolator, tempP;
  this->MakeZero();
  for (int i = 0; i < thePoints.size; i ++) {
    theLagrangeInterpolator.MakeConst(1, 1);
    for (int j = 0; j < thePoints.size; j ++) {
      if (i != j) {
        tempP.MakeDegreeOne(1, 0, 1, - thePoints[j]);
        tempP /= thePoints[i] - thePoints[j];
        theLagrangeInterpolator *= tempP;
      }
    }
    theLagrangeInterpolator *= ValuesAtThePoints[i];
    *this += theLagrangeInterpolator;
  }
}

template <class coefficient>
void Polynomial<coefficient>::GetValuesLagrangeInterpolandsAtConsecutivePoints(
  Vector<Rational>& inputConsecutivePointsOfInterpolation,
  Vector<Rational>& inputPointsOfEvaluation,
  Vectors<Rational>& outputValuesInterpolands
) {
  outputValuesInterpolands.SetSize(inputConsecutivePointsOfInterpolation.size);
  for (int i = 0; i < inputConsecutivePointsOfInterpolation.size; i ++) {
    Vector<Rational>& currentInterpoland = outputValuesInterpolands[i];
    currentInterpoland.SetSize(inputPointsOfEvaluation.size);
    for (int j = 0; j < inputConsecutivePointsOfInterpolation.size; j ++) {
      currentInterpoland[j] = (i == j) ? 1 : 0;
    }
    for (int j = inputConsecutivePointsOfInterpolation.size; j < inputPointsOfEvaluation.size; j ++) {
      currentInterpoland[j] = 1;
      for (int k = 0; k < inputConsecutivePointsOfInterpolation.size; k ++) {
        if (i != k) {
          currentInterpoland[j] *= inputPointsOfEvaluation[j] - inputConsecutivePointsOfInterpolation[k];
          currentInterpoland[j] /= inputConsecutivePointsOfInterpolation[i] - inputConsecutivePointsOfInterpolation[k];
        }
      }
    }
  }
}

template <class coefficient>
bool Polynomial<coefficient>::FactorMe(List<Polynomial<Rational> >& outputFactors, std::stringstream* comments) const {
  MacroRegisterFunctionWithName("Polynomial::FactorMe");
  outputFactors.SetSize(0);
  if (this->IsEqualToZero() || this->IsConstant()) {
    outputFactors.AddOnTop(*this);
    return true;
  }
  List<Polynomial<Rational> > factorsToBeProcessed;
  factorsToBeProcessed.AddOnTop(*this);
  Polynomial<Rational> currentFactor, divisor;
  while (factorsToBeProcessed.size > 0) {
    currentFactor = factorsToBeProcessed.PopLastObject();
    if (!currentFactor.FactorMeOutputIsADivisor(divisor, comments)) {
      return false;
    }
    if (currentFactor.IsEqualToOne()) {
      outputFactors.AddOnTop(divisor);
      continue;
    }
    Rational tempRat = divisor.ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
    currentFactor /= tempRat;
    factorsToBeProcessed.AddOnTop(divisor);
    factorsToBeProcessed.AddOnTop(currentFactor);
  }
  outputFactors.QuickSortAscending();
  return true;
}

template <class coefficient>
bool Polynomial<coefficient>::
FactorMeOutputIsADivisor(Polynomial<Rational>& output, std::stringstream* comments) {
  MacroRegisterFunctionWithName("Polynomial_CoefficientType::FactorMeOutputIsADivisor");
  if (this->GetMinNumVars() > 1) {
    return false;
  }
  if (this->GetMinNumVars() == 0) {
    return true;
  }
  Polynomial<Rational> thePoly = *this;
  thePoly.ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy();
  int degree = 0;
  if (!thePoly.TotalDegree().IsSmallInteger(&degree)) {
    return false;
  }
  int degreeLeft = degree / 2;
  Vector<Rational> AllPointsOfEvaluation;
  List<List<LargeInt> > thePrimeFactorsAtPoints;
  List<List<int> > thePrimeFactorsMults;
  Vector<Rational> theValuesAtPoints, theValuesAtPointsLeft;
  AllPointsOfEvaluation.SetSize(degree + 1);
  thePrimeFactorsAtPoints.SetSize(degreeLeft + 1);
  thePrimeFactorsMults.SetSize(degreeLeft + 1);
  AllPointsOfEvaluation[0] = 0;
  for (int i = 1; i < AllPointsOfEvaluation.size; i ++) {
    AllPointsOfEvaluation[i] = (i % 2 == 1) ? i / 2 + 1 : - (i / 2);
  }
  Vector<Rational> theArgument;
  theArgument.SetSize(1);
  theValuesAtPoints.SetSize(AllPointsOfEvaluation.size);
  LargeInt tempLI;
  for (int i = 0; i < AllPointsOfEvaluation.size; i ++) {
    theArgument[0] = AllPointsOfEvaluation[i];
    theValuesAtPoints[i] = thePoly.Evaluate(theArgument);
    if (theValuesAtPoints[i].IsEqualToZero()) {
      output.MakeDegreeOne(1, 0, 1, - theArgument[0]);
      *this /= output;
      return true;
    }
    if (i < degreeLeft + 1) {
      theValuesAtPoints[i].IsInteger(&tempLI);
      if (!tempLI.value.FactorReturnFalseIfFactorizationIncomplete(
        thePrimeFactorsAtPoints[i], thePrimeFactorsMults[i], 0, comments
      )) {
        if (comments != 0) {
          *comments << "<br>Aborting polynomial factorization: failed to factor the integer "
          << theValuesAtPoints[i].ToString() << " (most probably the integer is too large).";
        }
        return false;
      }
    }
  }
  Incrementable<Incrementable<SelectionOneItem> > theDivisorSel;
  Incrementable<SelectionOneItem> signSel;
  Polynomial<Rational> quotient, remainder;
  Vectors<Rational> valuesLeftInterpolands;
  Vector<Rational> PointsOfInterpolationLeft;
  PointsOfInterpolationLeft.Reserve(degreeLeft + 1);
  Rational currentPrimePowerContribution, currentPointContribution;
  for (int i = 0; i <= degreeLeft; i ++) {
    PointsOfInterpolationLeft.AddOnTop(AllPointsOfEvaluation[i]);
  }
  theDivisorSel.initFromMults(thePrimeFactorsMults);
  signSel.initFromMults(1, degreeLeft + 1);
  valuesLeftInterpolands.SetSize(degreeLeft + 1);
  this->GetValuesLagrangeInterpolandsAtConsecutivePoints(
    PointsOfInterpolationLeft, AllPointsOfEvaluation, valuesLeftInterpolands
  );
  do {
    do {
      theValuesAtPointsLeft.MakeZero(theValuesAtPoints.size);
      Rational firstValue;
      bool isGood = false;//<-we shall first check if the divisor is constant.
      for (int j = 0; j < theDivisorSel.theElements.size; j ++) {
        currentPointContribution = 1;
        for (int k = 0; k < theDivisorSel[j].theElements.size; k ++) {
          currentPrimePowerContribution = thePrimeFactorsAtPoints[j][k];
          currentPrimePowerContribution.RaiseToPower(theDivisorSel[j][k].SelectedMult);
          currentPointContribution *= currentPrimePowerContribution;
        }
        if (signSel[j].SelectedMult == 1) {
          currentPointContribution *= - 1;
        }
        if (!isGood) {
          if (j == 0) {
            firstValue = currentPointContribution;
          } else {
            if (firstValue != currentPointContribution) {
              isGood = true; //<- the divisor has takes two different values, hence is non-constant.
            }
          }
        }
        theValuesAtPointsLeft += valuesLeftInterpolands[j]*currentPointContribution;
      }
      if (!isGood) {
        continue;
      }
      for (int j = 0; j < AllPointsOfEvaluation.size; j ++) {
        if (theValuesAtPointsLeft[j].IsEqualToZero()) {
          isGood = false;
          break;
        }
        currentPointContribution = (theValuesAtPoints[j]) / theValuesAtPointsLeft[j];
        if (!currentPointContribution.IsInteger()) {
          isGood = false;
          break;
        }
      }
      if (!isGood) {
        continue;
      }
      output.Interpolate((Vector<Rational>) PointsOfInterpolationLeft, (Vector<Rational>) theValuesAtPointsLeft);
      this->DivideBy(output, quotient, remainder);
      if (!remainder.IsEqualToZero()) {
        continue;
      }
      *this = quotient;
      return true;
    } while (theDivisorSel.IncrementReturnFalseIfPastLast());
  } while (signSel.IncrementReturnFalseIfPastLast());
  output = *this;
  this->MakeOne(1);
  return true;
}

bool Calculator::innerFactorPoly(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerFactorPoly");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (!theCommands.CallCalculatorFunction(CalculatorConversions::innerPolynomial<Rational>, input, output)) {
    return false;
  }
  Expression theContext = output.GetContext();
  Polynomial<Rational> thePoly = output.GetValue<Polynomial<Rational> >();
  if (thePoly.GetMinNumVars() > 1) {
    return output.MakeError("I have been taught to factor one variable polys only. ", theCommands);
  }
  List<Polynomial<Rational> > theFactors;
  if (!thePoly.FactorMe(theFactors, &theCommands.Comments)) {
    return false;
  }
  output.reset(theCommands, theFactors.size + 1);
  Expression polyE(theCommands), expressionE(theCommands);
  output.AddChildAtomOnTop(theCommands.opSequence());
  for (int i = 0; i < theFactors.size; i ++) {
    polyE.AssignValueWithContext(theFactors[i], theContext, theCommands);
    expressionE.children.Clear();
    expressionE.AddChildAtomOnTop("MakeExpression");
    expressionE.AddChildOnTop(polyE);
    output.AddChildOnTop(expressionE);
  }
  return true;
}

bool Calculator::innerZmodP(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerZmodP");
  if (!input.IsListNElements(3)) {
    return false;
  }
  Rational left, right;
  if (!input[1].IsRational(&left) || !input[2].IsRational(&right)) {
    return false;
  }
  LargeInt base;
  if (!right.IsInteger(&base)) {
    return false;
  }
  if (base.IsEqualToZero()) {
    return false;
  }
  LargeIntUnsigned theGCD;
  LargeIntUnsigned::gcd(left.GetDenominator(), base.value, theGCD);
  if (theGCD > 1) {
    return false;
  }
  ElementZmodP outputElt;
  outputElt.theModulo = base.value;
  outputElt = left.GetNumerator();
  return output.AssignValue(outputElt, theCommands);
}

bool Calculator::innerInterpolatePoly(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerInterpolatePoly");
  Matrix<Rational> pointsOfInterpoly;
  if (!theCommands.GetMatriXFromArguments(input, pointsOfInterpoly, 0, 2)) {
    return theCommands << "<hr>Failed to extract points of interpolation from " << input.ToString();
  }
  Polynomial<Rational> interPoly;
  Vector<Rational> theArgs, theValues;
  pointsOfInterpoly.GetVectorFromColumn(0, theArgs);
  pointsOfInterpoly.GetVectorFromColumn(1, theValues);
  interPoly.Interpolate(theArgs, theValues);
  Expression theContext;
  theContext.ContextMakeContextWithOnePolyVar(theCommands, "x");
  return output.AssignValueWithContext(interPoly, theContext, theCommands);
}

bool Calculator::innerPrintZnEnumeration(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerPrintZnEnumeration");
  if (!input.IsListNElements(3)) {
    return false;
  }
  int grade, dimension;
  if (!input[2].IsSmallInteger(&grade) || !input[1].IsSmallInteger(&dimension)) {
    return false;
  }
  if (grade > 10 || dimension > 5 || grade < 0 || dimension < 0) {
    return false;
  }
  SelectionPositiveIntegers theSel;
  theSel.init(dimension);
  std::stringstream out2, out;
  LargeIntUnsigned gradeLarge = (unsigned) grade;
  int counter = 0;
  for (theSel.SetFirstInGradeLevel(gradeLarge); theSel.GetGrading() == gradeLarge; theSel.IncrementReturnFalseIfPastLast()) {
    out2 << theSel.ToString() << "<br>";
    counter ++;
  }
  out << "Total " << counter << " vectors:<br>" << out2.str();
  return output.AssignValue(out.str(), theCommands);
}

bool Expression::AssignMatrixExpressions(
  const Matrix<Expression>& input,
  Calculator& owner,
  bool reduceOneRowToSequenceAndOneByOneToNonMatrix,
  bool dontReduceTypes
) {
  MacroRegisterFunctionWithName("Expression::AssignMatrixExpressions");
  if (reduceOneRowToSequenceAndOneByOneToNonMatrix && input.NumRows == 1) {
    if (input.NumCols == 1) {
      (*this) = input(0, 0);
      return true;
    }
    this->MakeSequence(owner);
    for (int i = 0; i < input.NumCols; i ++) {
      this->AddChildOnTop(input(0, i));
    }
    return true;
  }
  this->reset(owner, input.NumRows + 1);
  Expression theMatType(owner);
  theMatType.AddChildAtomOnTop(owner.opMatriX());
  this->AddChildOnTop(theMatType);
  enum mType{typeUnknown, typeRat, typeDouble, typeAlgebraic, typePolyRat, typePolyAlg, typeRF, typeExpression};
  mType outType = typeUnknown;
  Expression currentRow;
  for (int i = 0; i < input.NumRows; i ++) {
    currentRow.reset(owner);
    currentRow.children.Reserve(input.NumCols + 1);
    currentRow.AddChildAtomOnTop(owner.opSequence());
    for (int j = 0; j < input.NumCols; j ++) {
      currentRow.AddChildOnTop(input(i, j));
      mType inType;
      if (input(i, j).IsOfType<Rational>()) {
        inType = typeRat;
      } else if (input(i, j).IsOfType<AlgebraicNumber>()) {
        inType = typeAlgebraic;
      } else if (input(i, j).IsOfType<double>()) {
        inType = typeDouble;
      } else if (input(i, j).IsOfType<Polynomial<Rational> >()) {
        inType = typePolyRat;
      } else if (input(i, j).IsOfType<Polynomial<AlgebraicNumber> >()) {
        inType = typeAlgebraic;
      } else if (input(i, j).IsOfType<RationalFunctionOld>()) {
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
    this->AddChildOnTop(currentRow);
  }
  switch(outType) {
    case typeRat:
      theMatType.AddChildAtomOnTop(owner.opRational());
      break;
    case typeDouble:
      theMatType.AddChildAtomOnTop(owner.opDouble());
      break;
    case typeAlgebraic:
      theMatType.AddChildAtomOnTop(owner.opAlgNumber());
      break;
    case typePolyRat:
      theMatType.AddChildAtomOnTop(owner.opPoly());
      break;
    case typePolyAlg:
      theMatType.AddChildAtomOnTop(owner.opPolyOverANs());
      break;
    case typeRF:
      theMatType.AddChildAtomOnTop(owner.opRationalFunction());
      break;
    default:
      break;
  }
  if (outType != typeUnknown && outType != typeExpression && !dontReduceTypes) {
    this->SetChilD(0, theMatType);
  }
  return true;
}

bool Calculator::GetMatrixExpressionsFromArguments(
  const Expression& input, Matrix<Expression>& output, int desiredNumRows, int desiredNumCols
) {
  MacroRegisterFunctionWithName("Calculator::GetMatrixExpressionsFromArguments");
  if (!input.IsLisT()) {
    return false;
  }
  Expression inputModified = input;
  inputModified.SetChildAtomValue(0, this->opSequence());
  return this->GetMatrixExpressions(inputModified, output, desiredNumRows, desiredNumCols);
}

bool Calculator::GetMatrixExpressions(
  const Expression& input, Matrix<Expression>& output, int desiredNumRows, int desiredNumCols
) {
  MacroRegisterFunctionWithName("Calculator::GetMatrixExpressions");
  if (!input.IsSequenceNElementS() && !input.IsMatrix() && !input.IsIntervalRealLine()) {
    output.init(1, 1);
    output(0, 0) = input;
    return true;
  }
  if (input.size() < 2) {
    if (input.IsMatrix()) {
      output.init(0, 0);
      return true;
    }
    return false;
  }
  if (!input[1].IsSequenceNElementS() && !input[1].StartsWith(this->opIntervalOpen())) {
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
    output.init(1, input.size() - 1);
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
  output.init(input.size() - 1, input[1].size() - 1);
  for (int i = 1; i < input.size(); i ++) {
    if (input[i].IsSequenceNElementS(output.NumCols) || input[i].StartsWith(this->opIntervalOpen(), output.NumCols + 1)) {
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
  if (!input.IsListNElements(3)) {
    return false;
  }
  Vector<Polynomial<Rational> > inputPolForm;
  Expression startContext;
  if (!theCommands.GetVectorFromFunctionArguments(
    input, inputPolForm, &startContext, 2, CalculatorConversions::innerPolynomial<Rational>
  )) {
    return theCommands << "<hr>Failed to extract polynomials from arguments of " << input.ToString();
  }
  int letterDiff = 0, letterPol = 0;
  if (
    !inputPolForm[0].IsOneLetterFirstDegree(&letterDiff) ||
    !inputPolForm[1].IsOneLetterFirstDegree(&letterPol) ||
    letterDiff == letterPol
  ) {
    return theCommands << "<hr>Failed to get different one-variable polynomials from input.  " << input.ToString();
  }
  Expression endContext;
  endContext.MakeContextWithOnePolyVarOneDiffVar(
    theCommands, startContext.ContextGetContextVariable(letterPol), startContext.ContextGetContextVariable(letterDiff)
  );
  ElementWeylAlgebra<Rational> outputEWA;
  if (assignPoly) {
    outputEWA.Makexi(0, 1);
  } else {
    outputEWA.Makedi(0, 1);
  }
  return output.AssignValueWithContext(outputEWA, endContext, theCommands);
}

bool Calculator::ReadTestStrings(
  HashedList<std::string, MathRoutines::HashString>& outputCommands, List<std::string>& outputResults
) {
  MacroRegisterFunctionWithName("Calculator::ReadTestStrings");
  XML theFileReader;
  if (!theFileReader.ReadFromFile(this->theTestFile)) {
    return false;
  }
  outputCommands.Clear();
  outputResults.SetSize(0);
  outputCommands.SetExpectedSize(this->GetNumBuiltInFunctions());
  outputResults.Reserve(this->GetNumBuiltInFunctions());
  std::string buffer;
  while (theFileReader.positionInString < theFileReader.theString.size()) {
    if (!theFileReader.GetStringEnclosedIn("input", buffer)) {
      break;
    }
    outputCommands.AddOnTop(buffer);
    if (!theFileReader.GetStringEnclosedIn("output", buffer)) {
      return *this << "<hr>Failed to read result string number " << outputResults.size + 1 << ": is the test file corrupt?";
    }
    outputResults.AddOnTop(buffer);
  }
  if (outputCommands.size != outputResults.size || outputCommands.size == 0) {
    return *this << "<hr>Corrupt test file: got " << outputCommands.size
    << " commands and " << outputResults.size << " results, which should not happen. ";
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

bool Calculator::WriteTestStrings(List<std::string>& inputCommands, List<std::string>& inputResults) {
  MacroRegisterFunctionWithName("Calculator::WriteTestStrings");
  for (int i = 0; i < inputCommands.size; i ++) {
    this->theTestFile << "Command " << i + 1 << ": <input>" << inputCommands[i] << "</input>"
    << "<output>" << inputResults[i] << "</output>\n\n";
  }
  this->theTestFile.flush();
  return true;
}

bool Calculator::innerAutomatedTestSetKnownGoodCopy(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerAutomatedTestSetKnownGoodCopy");
  if (!theGlobalVariables.UserDefaultHasAdminRights()) {
    return theCommands << "Function requires admin access. ";
  }
  (void) input;//avoid unused variable warning, portable
  theGlobalVariables.millisecondsMaxComputation = 30000000; //30k seconds: ok, we have admin access.
  List<std::string> inputStringsTest, outputStringsTestWithInit, outputStringsTestNoInit;
  std::stringstream out;
  theCommands.theTestFileName = "automatedTest.txt";
  if (!FileOperations::OpenFileVirtual(theCommands.theTestFile, "output/" + theCommands.theTestFileName, false, true, false)) {
    crash << "This is a programming error or worse: file " << theCommands.theTestFileName
    << " does not exist but cannot be created. Something is very wrong. " << crash;
  }
  double startTime = theGlobalVariables.GetElapsedSeconds();
  theCommands.AutomatedTestRun(inputStringsTest, outputStringsTestWithInit, outputStringsTestNoInit);
  theCommands.WriteTestStrings(inputStringsTest, outputStringsTestWithInit);
  out << "Test run completed in " << theGlobalVariables.GetElapsedSeconds() - startTime << " seconds.";
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerAutomatedTest(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerAutomatedTest");
  if (!theGlobalVariables.UserDefaultHasAdminRights()) {
    return theCommands << "Automated test requires admin access";
  }
  theGlobalVariables.millisecondsMaxComputation = 30000000; //30k seconds, ok as we have admin access
  int64_t startTime = theGlobalVariables.GetElapsedMilliseconds();
  theCommands.theTestFileName = "automatedTest.txt";
  if (!FileOperations::FileExistsVirtual("output/" + theCommands.theTestFileName)) {
    return theCommands.innerAutomatedTestSetKnownGoodCopy(theCommands, input, output);
  }
  if (!FileOperations::OpenFileCreateIfNotPresentVirtual(theCommands.theTestFile, "ouput/" + theCommands.theTestFileName, false, false, false)) {
    crash << "This is a programming error or worse: failed to open an existing file: "
    << theCommands.theTestFileName << ". Something is very wrong. " << crash;
  }
  List<std::string> knownResults;
  HashedList<std::string, MathRoutines::HashString> knownCommands;
  std::stringstream out;
  if (!theCommands.ReadTestStrings(knownCommands, knownResults)) {
    out << "Error: failed to load test strings: the test file " << theCommands.theTestFileName << " may be corrupt. ";
    return output.AssignValue(out.str(), theCommands);
  }
  List<std::string> commandStrings, resultStringsWithInit, resultStringsNoInit;
  theCommands.AutomatedTestRun(commandStrings, resultStringsWithInit, resultStringsNoInit);
  std::stringstream errorTableStream;
  int numInconsistencies = 0;
  List<std::string> newCommands;
  for (int i = 0; i < commandStrings.size; i ++) {
    if (!knownCommands.Contains(commandStrings[i])) {
      newCommands.AddOnTop(commandStrings[i]);
      if (knownCommands[i] == commandStrings[i]) {
        crash << "Known command result not match current. " << crash;
      }
      continue;
    }
    int theIndex = knownCommands.GetIndex(commandStrings[i]);
    if (knownResults[theIndex] != resultStringsWithInit[i]) {
      errorTableStream << "\n<tr><td>" << commandStrings[i] << "</td><td>"
      << knownResults[theIndex] << "</td><td>" << resultStringsWithInit[i]
      << "</td></tr>\n";
      numInconsistencies ++;
    }
  }
  bool allWentGreat = true;
  if (numInconsistencies > 0) {
    out << "<b style = 'color:red'>The test file results do not match the current results.</b> There were "
    << numInconsistencies << " inconsistencies out of " << knownCommands.size
    << " input strings. The inconsistent result table follows. "
    << "\n<hr>\n<table><tr><td>Input</td><td>Desired result</td><td>Computed result</td></tr>"
    << errorTableStream.str() << "</table>\n<hr>\n";
    allWentGreat = false;
  }
  if (commandStrings.size != knownCommands.size || newCommands.size > 0) {
    if (commandStrings.size != knownCommands.size) {
      out << "There were " << knownCommands.size << " known commands read from the test file but the calculator has "
      << commandStrings.size << " functions total. ";
    }
    if (newCommands.size > 0) {
      out << "There were " << newCommands.size << " commands not recorded in the test file. The new commands follow. <br>";
      for (int i = 0; i < newCommands.size; i ++) {
        out << newCommands[i] << "<br>";
      }
    }
    out << "The test file must be out of date. Please update it.<hr>";
    allWentGreat = false;
  }
  if (allWentGreat) {
    out << "<span style =\"color:#0000FF\">All " << commandStrings.size
    << " results coincide with previously recorded values.</span> ";
  }
  out << "<br>The command for updating the test file is "
  << HtmlRoutines::GetCalculatorComputationAnchor("AutomatedTestSetKnownGoodCopy 0");
  out << "<br>Total time for the test: " << theGlobalVariables.GetElapsedMilliseconds() - startTime << " ms. ";
  return output.AssignValue(out.str(), theCommands);
}

int Calculator::GetNumBuiltInFunctions() {
  int result = 0;
  for (int i = 0; i < this->FunctionHandlers.size; i ++) {
    result += this->FunctionHandlers[i].size;
  }
  for (int i = 0; i < this->operationsCompositeHandlers.size; i ++) {
    result += this->operationsCompositeHandlers[i].size;
  }
  return result;
}

