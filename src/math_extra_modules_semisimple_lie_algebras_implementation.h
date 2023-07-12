
#ifndef header_math_extra_modules_semisimple_lie_algebras_implementation_ALREADY_INCLUDED
#define header_math_extra_modules_semisimple_lie_algebras_implementation_ALREADY_INCLUDED

#include "math_extra_modules_semisimple_lie_algebras.h"
#include "math_extra_drawing_variables.h"
#include "math_extra_differential_operators.h"
#include "math_rational_function.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "progress_report.h"

template <class Coefficient>
Rational ModuleSSalgebra<Coefficient>::highestWeightTrace(
  const Pair<
    MonomialTensor<int, HashFunctions::hashFunction>,
    MonomialTensor<int, HashFunctions::hashFunction>
  >& pair,
  ProgressReport* progressReport
) {
  STACK_TRACE("ModuleSSalgebra::highestWeightTrace");
  int indexInCache = this->cachedPairs.getIndex(pair);
  if (indexInCache != - 1) {
    return this->cachedTraces[indexInCache];
  }
  if (pair.object1.generatorsIndices.size == 0) {
    return 1;
  }
  Pair<
    MonomialTensor<int, HashFunctions::hashFunction>,
    MonomialTensor<int, HashFunctions::hashFunction>
  > newPair;
  MonomialTensor<int, HashFunctions::hashFunction>& newLeft = newPair.object1;
  MonomialTensor<int, HashFunctions::hashFunction>& newRight = newPair.object2;
  const MonomialTensor<int, HashFunctions::hashFunction>& oldRight =
  pair.object2;
  newLeft = pair.object1;
  (*newLeft.powers.lastObject()) -= 1;
  int index = *newLeft.generatorsIndices.lastObject();
  if (*newLeft.powers.lastObject() == 0) {
    newLeft.generatorsIndices.size --;
    newLeft.powers.size --;
  }
  int indexMinus =
  2 * this->getOwner().getNumberOfPositiveRoots() +
  this->getOwner().getRank() - index - 1;
  int simpleIndex =
  index -
  this->getOwner().getNumberOfPositiveRoots() -
  this->getOwner().getRank();
  MonomialTensor<int, HashFunctions::hashFunction> accumulator;
  accumulator.powers.reserve(oldRight.powers.size);
  accumulator.generatorsIndices.reserve(oldRight.generatorsIndices.size);
  Vector<Rational> remainingWeight;
  Rational result = 0;
  Rational summand;
  WeylGroupData& weylGroup = this->getOwner().weylGroup;
  for (int i = 0; i < oldRight.generatorsIndices.size; i ++) {
    if (oldRight.generatorsIndices[i] == indexMinus) {
      summand = 0;
      newRight = accumulator;
      newRight.multiplyByGeneratorPowerOnTheRight(
        oldRight.generatorsIndices[i], oldRight.powers[i] - 1
      );
      remainingWeight.makeZero(weylGroup.getDimension());
      for (int j = i + 1; j < oldRight.generatorsIndices.size; j ++) {
        newRight.multiplyByGeneratorPowerOnTheRight(
          oldRight.generatorsIndices[j], oldRight.powers[j]
        );
        remainingWeight +=
        weylGroup.rootSystem[oldRight.generatorsIndices[j]] *
        oldRight.powers[j];
      }
      remainingWeight +=
      this->highestWeightFiniteDimensionalPartSimpleCoordinates;
      summand +=
      weylGroup.getScalarProductSimpleRoot(remainingWeight, simpleIndex);
      summand *= 2;
      summand /= weylGroup.cartanSymmetric.elements[simpleIndex][simpleIndex];
      summand += 1;
      summand -= oldRight.powers[i];
      if (!summand.isEqualToZero()) {
        summand *= this->highestWeightTrace(newPair, progressReport);
      }
      summand *= oldRight.powers[i];
      result += summand;
    }
    accumulator.generatorsIndices.addOnTop(oldRight.generatorsIndices[i]);
    accumulator.powers.addOnTop(oldRight.powers[i]);
  }
  if (this->cachedPairs.size < this->maximumNumberOfCachedPairs) {
    this->cachedPairs.addOnTop(pair);
    this->cachedTraces.addOnTop(result);
  }
  if (progressReport != nullptr && this->cachedPairs.size < 500000) {
    std::stringstream currentStream;
    currentStream
    << "Number of cached pairs: "
    << this->cachedPairs.size
    << " at recursion depth "
    << global.customStackTrace.size;
    progressReport->report(currentStream.str());
  }
  return result;
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::applyTransposeAntiAutomorphism(
  MonomialTensor<int, HashFunctions::hashFunction>& monomial
) {
  for (int i = 0; i < monomial.generatorsIndices.size; i ++) {
    monomial.generatorsIndices[i] =
    this->getOwner().getNumberOfPositiveRoots() * 2 +
    this->getOwner().getRank() - monomial.generatorsIndices[i] - 1;
  }
  monomial.powers.reverseElements();
  monomial.generatorsIndices.reverseElements();
}

template <class Coefficient>
Rational ModuleSSalgebra<Coefficient>::
highestWeightTransposeAntiAutomorphismBilinearFormSimpleGeneratorsOnly(
  const MonomialTensor<int, HashFunctions::hashFunction>& leftMonomial,
  const MonomialTensor<int, HashFunctions::hashFunction>& rightMonomial,
  ProgressReport* progressReport
) {
  STACK_TRACE("ModuleSSalgebra<Coefficient>::hwtaabfSimpleGensOnly");
  const MonomialTensor<int, HashFunctions::hashFunction>* left = &leftMonomial;
  const MonomialTensor<int, HashFunctions::hashFunction>* right =
  &rightMonomial;
  if (leftMonomial > rightMonomial) {
    MathRoutines::swap(left, right);
  }
  Pair<
    MonomialTensor<int, HashFunctions::hashFunction>,
    MonomialTensor<int, HashFunctions::hashFunction>
  > pair;
  pair.object1 = *left;
  pair.object2 = *right;
  this->applyTransposeAntiAutomorphism(pair.object1);
  Rational result = this->highestWeightTrace(pair, progressReport);
  if (progressReport != nullptr) {
    std::stringstream currentStream;
    currentStream << this->cachedPairs.size << " total cached pairs";
    progressReport->report(currentStream.str());
  }
  return result;
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::substitute(
  const PolynomialSubstitution<Rational>& variableImages
) {
  for (int i = 0; i < this->actionsGeneratorsMatrix.size; i ++) {
    this->actionsGeneratorsMatrix[i].substitute(variableImages);
  }
  for (int i = 0; i < this->actionsGenerators.size; i ++) {
    for (int j = 0; j < this->actionsGenerators[i].size; j ++) {
      for (int k = 0; k < this->actionsGenerators[i][j].size; k ++) {
        this->actionsGenerators[i][j][k].substitute(variableImages);
      }
    }
  }
  List<MonomialUniversalEnveloping<Coefficient> > oldGeneratingWordsNonReduced;
  oldGeneratingWordsNonReduced = this->generatingWordsNonReduced;
  this->generatingWordsNonReduced.clear();
  for (int i = 0; i < oldGeneratingWordsNonReduced.size; i ++) {
    oldGeneratingWordsNonReduced[i].substitute(variableImages);
    this->generatingWordsNonReduced.addOnTop(
      oldGeneratingWordsNonReduced[i]
    );
  }
  for (int i = 0; i < this->generatingWordsGrouppedByWeight.size; i ++) {
    for (
      int j = 0; j < this->generatingWordsGrouppedByWeight[i].size; j ++
    ) {
      this->generatingWordsGrouppedByWeight[i][j].substitute(variableImages);
    }
  }
  for (int i = 0; i < this->bilinearFormsAtEachWeightLevel.size; i ++) {
    this->bilinearFormsAtEachWeightLevel[i].substitute(variableImages);
    this->bilinearFormsInverted[i].substitute(variableImages);
  }
  for (
    int i = 0; i < this->highestWeightDualCoordinatesBaseField.size; i ++
  ) {
    this->highestWeightDualCoordinatesBaseField[i].substitute(
      variableImages, Rational::one(), nullptr
    );
    this->highestWeightFundamentalCoordinatesBaseField[i].substitute(
      variableImages, Rational::one(), nullptr
    );
    this->highestWeightSimpleCoordinatesBaseField[i].substitute(
      variableImages, Rational::one(), nullptr
    );
  }
}

template <class Coefficient>
MatrixTensor<Coefficient>& ModuleSSalgebra<Coefficient>::
getActionGeneratorIndex(int generatorIndex) {
  STACK_TRACE("ModuleSSalgebra::getActionGeneratorIndex");
  int numberOfGenerators = this->getOwner().getNumberOfGenerators();
  if (generatorIndex < 0 || generatorIndex >= numberOfGenerators) {
    global.fatal
    << "Bad generator index: "
    << generatorIndex
    << ". "
    << global.fatal;
  }
  if (this->computedGeneratorActions.selected[generatorIndex]) {
    return this->actionsGeneratorsMatrix[generatorIndex];
  }
  this->computedGeneratorActions.addSelectionAppendNewIndex(generatorIndex);
  if (this->hasFreeAction(generatorIndex)) {
    global.fatal
    << "Due to a change in "
    << "implementation of the generalized Verma module class. "
    << global.fatal;
    this->actionsGeneratorsMatrix[generatorIndex].makeZero();
    return this->actionsGeneratorsMatrix[generatorIndex];
  }
  if (this->hasZeroActionFDpart(generatorIndex)) {
    this->actionsGeneratorsMatrix[generatorIndex].makeZero();
    return this->actionsGeneratorsMatrix[generatorIndex];
  }
  if (this->getOwner().isSimpleGenerator(generatorIndex)) {
    return this->getActionSimpleGeneratorIndex(generatorIndex);
  }
  MatrixTensor<Coefficient>& output =
  this->actionsGeneratorsMatrix[generatorIndex];
  if (this->getOwner().isGeneratorFromCartan(generatorIndex)) {
    output.makeZero();
    MonomialMatrix monomial;
    Vector<Coefficient> weightH;
    Coefficient coefficient, highestWeightCoefficientShift;
    weightH.makeEi(
      this->getOwner().getRank(),
      generatorIndex - this->getOwner().getNumberOfPositiveRoots()
    );
    highestWeightCoefficientShift =
    this->getOwner().weylGroup.rootScalarCartanRoot(
      weightH, this->highestWeightSimpleCoordinatesBaseField
    );
    highestWeightCoefficientShift -=
    this->getOwner().weylGroup.rootScalarCartanRoot(
      weightH, this->highestWeightFiniteDimensionalPartSimpleCoordinates
    );
    for (int i = 0; i < this->generatingWordsNonReduced.size; i ++) {
      Vector<Rational>& weight =
      this->generatingWordsWeightsPlusWeightFiniteDimensionalPart[i];
      coefficient =
      this->getOwner().weylGroup.rootScalarCartanRoot(weightH, weight);
      coefficient += highestWeightCoefficientShift;
      monomial.isIdentity = false;
      monomial.vIndex = i;
      monomial.dualIndex = i;
      output.addMonomial(monomial, coefficient);
    }
    return output;
  }
  List<int> adActions;
  Rational coefficient;
  this->getOwner().getChevalleyGeneratorAsLieBracketsSimpleGenerators(
    generatorIndex, adActions, coefficient
  );
  MatrixTensor<Coefficient> tempO;
  output = this->getActionGeneratorIndex(*adActions.lastObject());
  for (int i = adActions.size - 2; i >= 0; i --) {
    tempO = this->getActionGeneratorIndex(adActions[i]);
    output.lieBracketOnTheLeft(tempO);
  }
  Coefficient tempCF;
  tempCF = coefficient;
  output *= tempCF;
  return output;
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::getMatrixHomogenousElement(
  ElementUniversalEnveloping<Coefficient>& inputHomogeneous,
  List<List<ElementUniversalEnveloping<Coefficient> > >&
  outputSortedByArgumentWeight,
  Vector<Rational>& weightUEElementSimpleCoordinates,
  MatrixTensor<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  STACK_TRACE("ModuleSSalgebra::getMatrixHomogenousElement");
  this->getAdActionHomogenousElement(
    inputHomogeneous,
    weightUEElementSimpleCoordinates,
    outputSortedByArgumentWeight,
    ringUnit,
    ringZero
  );
  output.makeZero();
  for (int j = 0; j < outputSortedByArgumentWeight.size; j ++) {
    for (int k = 0; k < outputSortedByArgumentWeight[j].size; k ++) {
      MonomialUniversalEnveloping<Coefficient>& monomial =
      this->generatingWordsGrouppedByWeight[j][k];
      ElementUniversalEnveloping<Coefficient>& imageMonomial =
      outputSortedByArgumentWeight[j][k];
      int indexColumn = this->generatingWordsNonReduced.getIndex(monomial);
      if (indexColumn == - 1) {
        global.fatal << "Column index equals -1. " << global.fatal;
      }
      for (int l = 0; l < imageMonomial.size; l ++) {
        int indexRow =
        this->generatingWordsNonReduced.getIndex(imageMonomial[l]);
        if (indexRow == - 1) {
          global.fatal << "Negative row index not allowed. " << global.fatal;
        }
        output.addMonomial(
          MonomialMatrix(indexRow, indexColumn),
          imageMonomial.coefficients[l]
        );
      }
    }
  }
  if (!this->flagIsInitialized) {
    return;
  }
}

template <class Coefficient>
bool CharacterSemisimpleLieAlgebraModule<Coefficient>::
splitOverLeviMonomialsEncodeHighestWeight(
  std::string* report,
  CharacterSemisimpleLieAlgebraModule& output,
  const Selection& splittingParabolicSelection,
  const Selection& parabolicSelectionFiniteDimensionalInducingPart,
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms&
  outputWeylSubgroup
) {
  STACK_TRACE(
    "CharacterSemisimpleLieAlgebraModule::splitOverLeviMonomialsEncodeHighestWeight"
  );
  this->checkNonZeroOwner();
  std::stringstream out;
  std::string tempS;
  if (
    this->getOwner()->getRank() != splittingParabolicSelection.numberOfElements
  ) {
    global.fatal
    << "Parabolic selection selects out of "
    << splittingParabolicSelection.numberOfElements
    << " elements while the weyl group is of rank "
    << this->getOwner()->getRank()
    << ". "
    << global.fatal;
  }
  outputWeylSubgroup.makeParabolicFromSelectionSimpleRoots(
    this->getOwner()->weylGroup, splittingParabolicSelection, 1
  );
  outputWeylSubgroup.computeRootSubsystem();
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms
  complementGroup;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms
  finiteDimensionalWeyl;
  complementGroup.ambientWeyl = outputWeylSubgroup.ambientWeyl;
  finiteDimensionalWeyl.ambientWeyl = outputWeylSubgroup.ambientWeyl;
  Selection invertedSelection;
  invertedSelection = splittingParabolicSelection;
  invertedSelection.invertSelection();
  complementGroup.makeParabolicFromSelectionSimpleRoots(
    this->getOwner()->weylGroup, invertedSelection, 1
  );
  complementGroup.computeRootSubsystem();
  out << outputWeylSubgroup.toString(false);
  CharacterSemisimpleLieAlgebraModule charAmbientFDWeyl;
  CharacterSemisimpleLieAlgebraModule remainingCharDominantLevi;
  finiteDimensionalWeyl.makeParabolicFromSelectionSimpleRoots(
    this->getOwner()->weylGroup,
    parabolicSelectionFiniteDimensionalInducingPart,
    1
  );
  Weight<Coefficient> currentMonomial;
  Weight<Coefficient> localHighest;
  List<Coefficient> tempMults;
  HashedList<Vector<Coefficient> > tempHashedRoots;
  WeylGroupData& weylGroup = this->getOwner()->weylGroup;
  charAmbientFDWeyl.makeZero();
  Coefficient bufferCoefficient;
  Coefficient highestCoeff;
  out << "Starting character: " << this->toString();
  currentMonomial.owner = this->getOwner();
  for (int i = 0; i < this->size(); i ++) {
    const Weight<Coefficient>& monomial = (*this)[i];
    if (
      !finiteDimensionalWeyl.freudenthalFormulaIrrepIsWRTLeviPart(
        monomial.weightFundamentalCoordinates,
        tempHashedRoots,
        tempMults,
        tempS,
        10000
      )
    ) {
      if (report != nullptr) {
        *report = tempS;
      }
      return false;
    }
    for (int j = 0; j < tempHashedRoots.size; j ++) {
      bufferCoefficient = this->coefficients[i];
      currentMonomial.weightFundamentalCoordinates =
      weylGroup.getFundamentalCoordinatesFromSimple(tempHashedRoots[j]);
      bufferCoefficient *= tempMults[j];
      charAmbientFDWeyl.addMonomial(currentMonomial, bufferCoefficient);
    }
  }
  remainingCharDominantLevi.makeZero();
  Vectors<Coefficient> orbitDom;
  currentMonomial.owner = this->getOwner();
  for (int i = 0; i < charAmbientFDWeyl.size(); i ++) {
    orbitDom.setSize(0);
    if (
      !finiteDimensionalWeyl.generateOrbitReturnFalseIfTruncated(
        weylGroup.getSimpleCoordinatesFromFundamental(
          charAmbientFDWeyl[i].weightFundamentalCoordinates
        ),
        orbitDom,
        false,
        10000
      )
    ) {
      out
      << "failed to generate the complement-sub-Weyl-orbit of weight "
      << this->getOwner()->weylGroup.getSimpleCoordinatesFromFundamental(
        charAmbientFDWeyl[i].weightFundamentalCoordinates
      ).toString();
      if (report != nullptr) {
        *report = out.str();
      }
      return false;
    }
    for (int k = 0; k < orbitDom.size; k ++) {
      if (outputWeylSubgroup.isDominantWeight(orbitDom[k])) {
        currentMonomial.weightFundamentalCoordinates =
        weylGroup.getFundamentalCoordinatesFromSimple(orbitDom[k]);
        remainingCharDominantLevi.addMonomial(
          currentMonomial, charAmbientFDWeyl.coefficients[i]
        );
      }
    }
  }
  output.makeZero();
  out
  << "<br>Character with respect to Levi part: "
  << HtmlRoutines::getMathNoDisplay(remainingCharDominantLevi.toString());
  Vector<Coefficient> simpleGeneratorBaseField;
  while (!remainingCharDominantLevi.isEqualToZero()) {
    localHighest = *remainingCharDominantLevi.monomials.lastObject();
    for (bool found = true; found;) {
      found = false;
      for (int i = 0; i < outputWeylSubgroup.simpleRootsInner.size; i ++) {
        currentMonomial = localHighest;
        simpleGeneratorBaseField = outputWeylSubgroup.simpleRootsInner[i];
        // <- implicit type conversion here!
        currentMonomial.weightFundamentalCoordinates +=
        this->getOwner()->weylGroup.getFundamentalCoordinatesFromSimple(
          simpleGeneratorBaseField
        );
        if (
          remainingCharDominantLevi.monomials.contains(currentMonomial)
        ) {
          localHighest = currentMonomial;
          found = true;
        }
      }
    }
    highestCoeff =
    remainingCharDominantLevi.coefficients[
      remainingCharDominantLevi.monomials.getIndex(localHighest)
    ];
    output.addMonomial(localHighest, highestCoeff);
    if (
      !outputWeylSubgroup.freudenthalFormulaIrrepIsWRTLeviPart(
        localHighest.weightFundamentalCoordinates,
        tempHashedRoots,
        tempMults,
        tempS,
        10000
      )
    ) {
      if (report != nullptr) {
        *report = tempS;
      }
      return false;
    }
    for (int i = 0; i < tempHashedRoots.size; i ++) {
      currentMonomial.owner = this->getOwner();
      currentMonomial.weightFundamentalCoordinates =
      weylGroup.getFundamentalCoordinatesFromSimple(tempHashedRoots[i]);
      bufferCoefficient = tempMults[i];
      bufferCoefficient *= highestCoeff;
      remainingCharDominantLevi.subtractMonomial(
        currentMonomial, bufferCoefficient
      );
    }
  }
  out
  << "<br>Character with respect to Levi part: "
  << HtmlRoutines::getMathNoDisplay(output.toString());
  if (report != nullptr) {
    DrawingVariables drawingVariables;
    std::string tempS;
    this->drawMeNoMultiplicities(tempS, drawingVariables, 10000);
    Vector<Rational> root;
    out
    << "<hr>In the following weight visualization, "
    << "a yellow line is drawn if the corresponding weights are "
    << "simple reflections of one another, "
    << "with respect to a simple root of "
    << "the Levi part of the parabolic subalgebra. ";
    for (int i = 0; i < output.size(); i ++) {
      root =
      weylGroup.getSimpleCoordinatesFromFundamental(
        output[i].weightFundamentalCoordinates
      ).getVectorRational();
      outputWeylSubgroup.drawContour(
        root, drawingVariables, "#a0a000", 1000
      );
      std::stringstream currentStream;
      currentStream << output.coefficients[i].toString();
      drawingVariables.drawTextAtVector(
        root, currentStream.str(), "black"
      );
    }
    out
    << "<hr>"
    << drawingVariables.getHTMLDiv(weylGroup.getDimension(), true);
    *report = out.str();
  }
  return true;
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::splitOverLevi(
  std::string* report,
  Selection& splittingParSel,
  List<ElementUniversalEnveloping<Coefficient> >* outputEigenVectors,
  Vectors<Coefficient>* outputWeightsFundamentalCoordinates,
  Vectors<Coefficient>* outputEigenSpace,
  CharacterSemisimpleLieAlgebraModule<Coefficient>* outputChar
) {
  STACK_TRACE("ModuleSSalgebra::splitOverLevi");
  this->checkInitialization();
  if (this->character.size() != 1) {
    if (report != nullptr) {
      std::stringstream out;
      out
      << "I have been instructed only to split modules "
      << "that are irreducible over the ambient Lie algebra";
      out
      << " Instead I got the character "
      << this->character.toString()
      << " ("
      << this->character.size()
      << " monomials)";
      *report = out.str();
    }
    return;
  }
  if (this->getOwner().getRank() != splittingParSel.numberOfElements) {
    global.fatal
    << "Semisimple rank is "
    << this->getOwner().getRank()
    << " but splitting parabolic selects "
    << " out of "
    << splittingParSel.numberOfElements
    << " simple roots. "
    << global.fatal;
  }
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms
  subWeyl;
  subWeyl.ambientWeyl = &this->owner->weylGroup;
  MemorySaving<CharacterSemisimpleLieAlgebraModule<Coefficient> > buffer;
  CharacterSemisimpleLieAlgebraModule<Coefficient>& charWRTsubalgebra = (
    outputChar == 0
  ) ?
  buffer.getElement() :
  *outputChar;
  this->character.splitOverLeviMonomialsEncodeHighestWeight(
    report,
    charWRTsubalgebra,
    splittingParSel,
    this->parabolicSelectionNonSelectedAreElementsLevi,
    subWeyl
  );
  std::stringstream out;
  if (report != nullptr) {
    out << *report;
  }
  Selection splittingParSelectedInLevi;
  splittingParSelectedInLevi = splittingParSel;
  splittingParSelectedInLevi.invertSelection();
  if (
    !splittingParSelectedInLevi.isSubset(
      this->parabolicSelectionSelectedAreElementsLevi
    )
  ) {
    out
    << "The parabolic subalgebra you selected is "
    << "not a subalgebra of the ambient parabolic subalgebra. "
    << "The parabolic has root of Levi given by "
    << splittingParSel.toString()
    << " while the ambient parabolic subalgebra has root of Levi given by "
    << this->parabolicSelectionNonSelectedAreElementsLevi.toString();
    if (report != nullptr) {
      *report = out.str();
    }
    return;
  }
  out << "<br>Parabolic selection: " << splittingParSel.toString();
  List<List<Vector<Coefficient> > > eigenSpacesPerSimpleGenerator;
  // if (false)
  eigenSpacesPerSimpleGenerator.setSize(
    splittingParSelectedInLevi.cardinalitySelection
  );
  Vectors<Coefficient> tempSpace1, tempSpace2;
  MemorySaving<Vectors<Coefficient> > tempEigenVects;
  Vectors<Coefficient>& finalEigenSpace = (outputEigenSpace == 0) ?
  tempEigenVects.getElement() :
  *outputEigenSpace;
  finalEigenSpace.makeEiBasis(this->getDimension());
  for (
    int i = 0; i < splittingParSelectedInLevi.cardinalitySelection; i ++
  ) {
    int generatorIndex =
    splittingParSelectedInLevi.elements[i] +
    this->getOwner().getRank() +
    this->getOwner().getNumberOfPositiveRoots();
    MatrixTensor<Coefficient>& currentOp =
    this->getActionGeneratorIndex(generatorIndex);
    Matrix<Coefficient> currentOpMat;
    currentOp.getMatrix(currentOpMat, this->getDimension());
    currentOpMat.getZeroEigenSpaceModifyMe(eigenSpacesPerSimpleGenerator[i]);
    tempSpace1 = finalEigenSpace;
    tempSpace2 = eigenSpacesPerSimpleGenerator[i];
    finalEigenSpace.intersectTwoLinearSpaces(
      tempSpace1, tempSpace2, finalEigenSpace
    );
  }
  out << "<br>Eigenvectors:<table> ";
  std::stringstream readyForLatexComsumption;
  readyForLatexComsumption << "\\begin{tabular}{|lll|}\n <br>";
  readyForLatexComsumption
  << "\\hline\\multicolumn{3}{|c|}{Highest weight $"
  << this->highestWeightFundamentalCoordinatesBaseField.toStringLetterFormat(
    "\\omega"
  )
  << "$}\\\\\n<br>";
  readyForLatexComsumption
  << "weight fund. coord.& singular vector \\\\\\hline\n<br>";
  Vector<Coefficient> currentWeight;
  Vector<Coefficient> highestWeightFundamentalCoordinatesNilPart;
  highestWeightFundamentalCoordinatesNilPart =
  this->highestWeightFundamentalCoordinatesBaseField;
  highestWeightFundamentalCoordinatesNilPart -=
  this->highestWeightFiniteDimensionalPartFundamentalCoordinates;
  ElementUniversalEnveloping<Coefficient> currentElement, element;
  for (int j = 0; j < finalEigenSpace.size; j ++) {
    out << "<tr><td>";
    currentElement.makeZero(this->getOwner());
    Vector<Coefficient>& currentVect = finalEigenSpace[j];
    int lastNonZeroIndex = - 1;
    for (int i = 0; i < currentVect.size; i ++) {
      if (!(currentVect[i].isEqualToZero())) {
        element.makeZero(this->getOwner());
        element.addMonomial(this->generatingWordsNonReduced[i], 1);
        element *= currentVect[i];
        currentElement += element;
        lastNonZeroIndex = i;
      }
    }
    currentWeight =
    subWeyl.ambientWeyl->getFundamentalCoordinatesFromSimple(
      this->generatingWordsWeightsPlusWeightFiniteDimensionalPart[
        lastNonZeroIndex
      ]
    );
    // <-implicitTypeConversionHere
    currentWeight += highestWeightFundamentalCoordinatesNilPart;
    readyForLatexComsumption
    << "$"
    << currentWeight.toStringLetterFormat("\\omega")
    << "$&$"
    << currentVect.toStringLetterFormat("m")
    << "$";
    if (currentElement.size() > 1) {
      out << "(";
    }
    if (outputEigenVectors != 0) {
      outputEigenVectors->addOnTop(currentElement);
    }
    if (outputWeightsFundamentalCoordinates != 0) {
      outputWeightsFundamentalCoordinates->addOnTop(currentWeight);
    }
    out << currentElement.toString(&global.defaultFormat.getElement());
    if (currentElement.size() > 1) {
      out << ")";
    }
    out << " v_\\lambda";
    out
    << "</td><td>(weight: "
    << currentWeight.toStringLetterFormat("\\omega")
    << ")</td></tr>";
    readyForLatexComsumption << "\\\\\n<br>";
  }
  out << "</table>";
  readyForLatexComsumption << "\\hline \n<br> \\end{tabular}";
  out << "<br>Your ready for LaTeX consumption text follows.<br>";
  out << readyForLatexComsumption.str();
  if (report != nullptr) {
    *report = out.str();
  }
}

template <class Coefficient>
MatrixTensor<Coefficient>& ModuleSSalgebra<Coefficient>::
getActionSimpleGeneratorIndex(int generatorIndex) {
  Vector<Rational> genWeight =
  this->getOwner().getWeightOfGenerator(generatorIndex);
  Vector<Rational> targetWeight;
  Pair<
    MonomialTensor<int, HashFunctions::hashFunction>,
    MonomialTensor<int, HashFunctions::hashFunction>
  > currentPair;
  MatrixTensor<Coefficient>& outputMat =
  this->actionsGeneratorsMatrix[generatorIndex];
  Vector<Coefficient> scalarProducts;
  outputMat.makeZero();
  for (int i = 0; i < this->generatingWordsGrouppedByWeight.size; i ++) {
    List<MonomialTensor<int, HashFunctions::hashFunction> >& currentWordList =
    this->generatingWordsIntegersGrouppedByWeight[i];
    const Vector<Rational>& currentWeight =
    this->moduleWeightsSimpleCoordinates[i];
    targetWeight = currentWeight + genWeight;
    int weightLevelIndex =
    this->moduleWeightsSimpleCoordinates.getIndex(targetWeight);
    if (weightLevelIndex != - 1) {
      int columnOffset = this->getOffsetFromWeightIndex(i);
      int rowOffset = this->getOffsetFromWeightIndex(weightLevelIndex);
      List<MonomialTensor<int, HashFunctions::hashFunction> >& otherWordList =
      this->generatingWordsIntegersGrouppedByWeight[weightLevelIndex];
      for (int j = 0; j < currentWordList.size; j ++) {
        scalarProducts.setSize(otherWordList.size);
        for (int k = 0; k < otherWordList.size; k ++) {
          if (
            generatorIndex > this->getOwner().getNumberOfPositiveRoots()
          ) {
            currentPair.object1 = currentWordList[j];
            this->applyTransposeAntiAutomorphism(currentPair.object1);
            currentPair.object2 = otherWordList[k];
            currentPair.object2.multiplyByGeneratorPowerOnTheLeft(
              this->getOwner().getOppositeGeneratorIndex(generatorIndex),
              1
            );
          } else {
            currentPair.object1 = currentWordList[j];
            currentPair.object1.multiplyByGeneratorPowerOnTheLeft(
              generatorIndex, 1
            );
            this->applyTransposeAntiAutomorphism(currentPair.object1);
            currentPair.object2 = otherWordList[k];
          }
          ProgressReport report;
          scalarProducts[k] = this->highestWeightTrace(currentPair, &report);
        }
        this->bilinearFormsInverted[weightLevelIndex].actOnVectorColumn(
          scalarProducts
        );
        for (int k = 0; k < scalarProducts.size; k ++) {
          outputMat.addMonomial(
            MonomialMatrix(rowOffset + k, columnOffset + j),
            scalarProducts[k]
          );
        }
      }
    }
  }
  return outputMat;
}

template <class Coefficient>
bool ModuleSSalgebra<Coefficient>::makeFromHW(
  SemisimpleLieAlgebra& inputAlgebra,
  Vector<Coefficient>& highestWeightFundamentalCoordinates,
  const Selection& selNonSelectedAreElementsLevi,
  const Coefficient& ringUnit,
  const Coefficient& ringZero,
  std::string* outputReport,
  bool computeSimpleGens
) {
  STACK_TRACE("ModuleSSalgebra<Coefficient>::makeFromHW");
  ProgressReport report;
  this->owner = &inputAlgebra;
  SemisimpleLieAlgebra& algebra = inputAlgebra;
  int rank = algebra.getRank();
  if (
    highestWeightFundamentalCoordinates.size != rank ||
    selNonSelectedAreElementsLevi.numberOfElements != rank
  ) {
    global.fatal
    << "I am asked to create a "
    << "generalized Verma module with a semisimple Lie algebra of rank "
    << rank
    << " but the input highest weight, "
    << highestWeightFundamentalCoordinates.toString()
    << ", has "
    << highestWeightFundamentalCoordinates.size
    << " coordinates and "
    << " the parabolic section indicates rank of "
    << selNonSelectedAreElementsLevi.numberOfElements
    << ". "
    << global.fatal;
  }
  WeylGroupData& weylGroup = algebra.weylGroup;
  this->cachedPairs.clear();
  this->cachedTraces.setSize(0);
  this->parabolicSelectionNonSelectedAreElementsLevi =
  selNonSelectedAreElementsLevi;
  this->parabolicSelectionSelectedAreElementsLevi =
  this->parabolicSelectionNonSelectedAreElementsLevi;
  this->parabolicSelectionSelectedAreElementsLevi.invertSelection();
  this->highestWeightFundamentalCoordinatesBaseField =
  highestWeightFundamentalCoordinates;
  this->highestWeightDualCoordinatesBaseField.setSize(rank);
  this->highestWeightFiniteDimensionalPartFundamentalCoordinates.setSize(rank);
  for (int i = 0; i < rank; i ++) {
    this->highestWeightFiniteDimensionalPartFundamentalCoordinates[i] = 0;
    if (this->parabolicSelectionSelectedAreElementsLevi.selected[i]) {
      int inputCoordinate;
      if (
        !this->highestWeightFundamentalCoordinatesBaseField[i].isSmallInteger(
          &inputCoordinate
        )
      ) {
        if (outputReport != nullptr) {
          *outputReport =
          "The given module over the Levi part is not finite dimensional";
        }
        return false;
      }
      this->highestWeightFiniteDimensionalPartFundamentalCoordinates[i] =
      inputCoordinate;
    }
    this->highestWeightDualCoordinatesBaseField[i] =
    this->highestWeightFundamentalCoordinatesBaseField[i];
    this->highestWeightDualCoordinatesBaseField[i] *=
    weylGroup.cartanSymmetric.elements[i][i] /
    2;
  }
  this->highestWeightFiniteDimensionalPartSimpleCoordinates =
  weylGroup.getSimpleCoordinatesFromFundamental(
    this->highestWeightFiniteDimensionalPartFundamentalCoordinates,
    Rational::zero()
  );
  this->highestWeightFiniteDimensionalPartDualCoordinates =
  weylGroup.getDualCoordinatesFromFundamental(
    this->highestWeightFiniteDimensionalPartFundamentalCoordinates
  );
  this->highestWeightSimpleCoordinatesBaseField =
  weylGroup.getSimpleCoordinatesFromFundamental(
    this->highestWeightFundamentalCoordinatesBaseField, ringZero
  );
  this->character.makeFromWeight(
    this->highestWeightSimpleCoordinatesBaseField, this->owner
  );
  unsigned long long startingRationalOperations =
  Rational::totalLargeAdditions +
  Rational::totalSmallAdditions +
  Rational::totalLargeMultiplications +
  Rational::totalSmallMultiplications;
  LittelmannPath startingPath;
  startingPath.makeFromweightInSimpleCoordinates(
    this->highestWeightFiniteDimensionalPartSimpleCoordinates, weylGroup
  );
  List<List<int> > generatorsIndices;
  if (
    !startingPath.generateOrbit(
      this->allPaths,
      generatorsIndices,
      1000,
      &this->parabolicSelectionNonSelectedAreElementsLevi
    )
  ) {
    if (outputReport != nullptr) {
      *outputReport =
      "Error: number of Littelmann paths exceeded allowed limit of 1000.";
    }
    return false;
  }
  this->moduleWeightsSimpleCoordinates.clear();
  Weight<Coefficient> tempCharMon;
  tempCharMon.owner = nullptr;
  this->characterOverH.makeZero();
  for (int i = 0; i < this->allPaths.size; i ++) {
    this->moduleWeightsSimpleCoordinates.addOnTopNoRepetition(
      *this->allPaths[i].waypoints.lastObject()
    );
    tempCharMon.weightFundamentalCoordinates =
    weylGroup.getFundamentalCoordinatesFromSimple(
      *this->allPaths[i].waypoints.lastObject()
    );
    this->characterOverH.addMonomial(tempCharMon, 1);
  }
  this->moduleWeightsSimpleCoordinates.quickSortAscending();
  std::stringstream out2;
  this->generatingWordsGrouppedByWeight.setSize(
    this->moduleWeightsSimpleCoordinates.size
  );
  this->generatingWordsIntegersGrouppedByWeight.setSize(
    this->moduleWeightsSimpleCoordinates.size
  );
  for (int i = 0; i < this->generatingWordsGrouppedByWeight.size; i ++) {
    this->generatingWordsGrouppedByWeight[i].size = 0;
    this->generatingWordsIntegersGrouppedByWeight[i].size = 0;
  }
  MonomialUniversalEnveloping<Coefficient> currentNonReducedElement;
  MonomialTensor<int, HashFunctions::hashFunction> monomialInt;
  for (int i = 0; i < this->allPaths.size; i ++) {
    List<int>& currentPath = generatorsIndices[i];
    currentNonReducedElement.makeOne(this->getOwner());
    monomialInt.makeConstant();
    for (int j = currentPath.size - 1; j >= 0; j --) {
      int index = currentPath[j];
      if (index > 0) {
        index ++;
      }
      currentNonReducedElement.multiplyByGeneratorPowerOnTheRight(
        this->getOwner().getGeneratorFromDisplayIndex(index),
        ringUnit
      );
      monomialInt.multiplyByGeneratorPowerOnTheRight(
        this->getOwner().getGeneratorFromDisplayIndex(index), 1
      );
    }
    Vector<Rational>& hwCurrent = *this->allPaths[i].waypoints.lastObject();
    int index = this->moduleWeightsSimpleCoordinates.getIndex(hwCurrent);
    if (index == - 1) {
      out2
      << "Error: could not generate all weights "
      << "in the weight support. Maybe they are too many? Allowed "
      << "# of weights is 10000";
      if (outputReport != nullptr) {
        *outputReport = out2.str();
      }
      return false;
    }
    this->generatingWordsGrouppedByWeight[index].addOnTop(
      currentNonReducedElement
    );
    this->generatingWordsIntegersGrouppedByWeight[index].addOnTop(monomialInt);
  }
  this->generatingWordsNonReduced.clear();
  this->generatingWordsNonReduced.setExpectedSize(this->allPaths.size);
  this->generatingWordsNonReducedInt.clear();
  this->generatingWordsNonReducedInt.setExpectedSize(this->allPaths.size);
  this->generatingWordsNonReduced.size = 0;
  this->generatingWordsWeightsPlusWeightFiniteDimensionalPart.setSize(0);
  this->generatingWordsWeightsPlusWeightFiniteDimensionalPart.reserve(
    this->allPaths.size
  );
  for (int i = 0; i < this->generatingWordsGrouppedByWeight.size; i ++) {
    List<MonomialUniversalEnveloping<Coefficient> >& currentList =
    this->generatingWordsGrouppedByWeight[i];
    List<MonomialTensor<int, HashFunctions::hashFunction> >& currentListInt =
    this->generatingWordsIntegersGrouppedByWeight[i];
    currentList.quickSortDescending();
    currentListInt.quickSortDescending();
    for (int j = 0; j < currentList.size; j ++) {
      // wordCounter ++;
      this->generatingWordsNonReduced.addOnTop(currentList[j]);
      this->generatingWordsNonReducedInt.addOnTop(currentListInt[j]);
      this->generatingWordsWeightsPlusWeightFiniteDimensionalPart.addOnTop(
        this->moduleWeightsSimpleCoordinates[i]
      );
    }
  }
  this->intermediateStepForMakeFromHW(ringUnit, ringZero);
  this->numberOfCachedPairsBeforeSimpleGenerators = this->cachedPairs.size;
  this->numberRationalMultiplicationsAndAdditionsBeforeSimpleGenerators =
  Rational::totalLargeAdditions +
  Rational::totalSmallAdditions +
  Rational::totalLargeMultiplications +
  Rational::totalSmallMultiplications - startingRationalOperations;
  bool isBad = false;
  for (int k = 0; k < this->bilinearFormsAtEachWeightLevel.size; k ++) {
    Matrix<Coefficient>& bilinearForm =
    this->bilinearFormsAtEachWeightLevel[k];
    Matrix<Coefficient>& bilinearFormInverted = this->bilinearFormsInverted[k];
    if (!bilinearForm.isNonNegativeAllEntries()) {
      this->flagConjectureBholds = false;
    }
    if (bilinearFormInverted.numberOfRows <= 0) {
      isBad = true;
    }
  }
  if (isBad) {
    if (outputReport != nullptr) {
      out2
      << "<br>Error: the Littelmann-path induced "
      << "monomials do not give a monomial basis. ";
      *outputReport = out2.str();
    }
    return false;
  }
  ElementSemisimpleLieAlgebra<Rational> currentElement;
  if (computeSimpleGens) {
    for (int k = 0; k < 2; k ++) {
      for (int j = 0; j < this->getOwner().getRank(); j ++) {
        if (this->parabolicSelectionSelectedAreElementsLevi.selected[j]) {
          int index = this->getOwner().getNumberOfPositiveRoots() - j - 1;
          if (k == 1) {
            index =
            this->getOwner().getNumberOfPositiveRoots() +
            this->getOwner().getRank() +
            j;
          }
          currentElement.makeGenerator(index, this->getOwner());
          if (outputReport != nullptr) {
            out2
            << "<hr>Simple generator: "
            << currentElement.toString(
              &global.defaultFormat.getElement()
            );
          }
          MatrixTensor<Coefficient>& matrix =
          this->getActionGeneratorIndex(index);
          std::stringstream currentStream;
          currentStream
          << "computing action simple generator index "
          << (2 * k - 1) *(j + 1)
          << " ... ";
          report.report(currentStream.str());
          currentStream << " done!";
          report.report(currentStream.str());
          if (outputReport != nullptr) {
            if (this->getDimension() < 50) {
              out2
              << "<br>Matrix of elemenent in the m_i basis:<br>"
              << HtmlRoutines::getMathNoDisplay(matrix.toString(), 5000);
            } else {
              out2
              << "<br>Matrix of elemenent in the m_i basis:<br>"
              << matrix.toString();
            }
          }
          if (k == 1) {
            this->getActionGeneratorIndex(
              this->getOwner().getNumberOfPositiveRoots() + j
            );
          }
        }
      }
    }
  }
  if (outputReport != nullptr) {
    *outputReport = out2.str();
  }
  this->flagIsInitialized = true;
  report.report("Done with module generation");
  return true;
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::intermediateStepForMakeFromHW(
  const Coefficient& ringUnit, const Coefficient& ringZero
) {
  STACK_TRACE("ModuleSSalgebra<Coefficient>::intermediateStepForMakeFromHW");
  ProgressReport report;
  ProgressReport report2;
  this->bilinearFormsAtEachWeightLevel.setSize(
    this->generatingWordsGrouppedByWeight.size
  );
  this->bilinearFormsInverted.setSize(
    this->generatingWordsGrouppedByWeight.size
  );
  this->computedGeneratorActions.initialize(
    this->getOwner().getNumberOfGenerators()
  );
  this->actionsGenerators.setSize(
    this->getOwner().getNumberOfGenerators()
  );
  this->actionsGeneratorsMatrix.setSize(
    this->getOwner().getNumberOfGenerators()
  );
  this->flagConjectureBholds = true;
  this->flagConjectureCholds = true;
  for (int l = 0; l < this->generatingWordsGrouppedByWeight.size; l ++) {
    Matrix<Coefficient>& currentBF = this->bilinearFormsAtEachWeightLevel[l];
    List<MonomialUniversalEnveloping<Coefficient> >& currentWordList =
    this->generatingWordsGrouppedByWeight[l];
    List<MonomialTensor<int, HashFunctions::hashFunction> >& currentWordListInt
    =
    this->generatingWordsIntegersGrouppedByWeight[l];
    currentBF.initialize(currentWordList.size, currentWordList.size);
    for (int i = 0; i < currentWordList.size; i ++) {
      for (int j = i; j < currentWordList.size; j ++) {
        std::stringstream currentStream;
        currentStream
        << " Computing Shapovalov form layer "
        << l
        << " out of "
        << this->generatingWordsGrouppedByWeight.size
        << " between indices "
        << i + 1
        << " and "
        << j + 1
        << " out of "
        << currentWordList.size;
        currentBF.elements[i][j] =
        this->
        highestWeightTransposeAntiAutomorphismBilinearFormSimpleGeneratorsOnly(
          currentWordListInt[i], currentWordListInt[j], &report2
        );
        report.report(currentStream.str());
        if (i != j) {
          currentBF.elements[j][i] = currentBF.elements[i][j];
        }
      }
    }
    Matrix<Coefficient> determinantComputer;
    determinantComputer = currentBF;
    Coefficient determinant;
    determinantComputer.computeDeterminantOverwriteMatrix(
      determinant, ringUnit, ringZero
    );
    if (!determinant.isEqualToZero()) {
      this->bilinearFormsInverted[l] = currentBF;
      this->bilinearFormsInverted[l].invert();
      if (!currentBF.isNonNegativeAllEntries()) {
        this->flagConjectureBholds = false;
      }
    } else {
      this->bilinearFormsInverted[l].initialize(0, 0);
    }
  }
}

template <class Coefficient>
template <class ResultType>
void ModuleSSalgebra<Coefficient>::getElementsNilradical(
  List<ElementUniversalEnveloping<ResultType> >& output,
  bool useNegativeNilradical,
  List<int>* outputListOfGenerators,
  bool useNilWeight,
  bool ascending
) {
  SemisimpleLieAlgebra& ownerSS = this->getOwner();
  ownerSS.orderNilradical(
    this->parabolicSelectionNonSelectedAreElementsLevi,
    useNilWeight,
    ascending
  );
  ElementUniversalEnveloping<ResultType> element;
  output.setSize(0);
  output.reserve(ownerSS.getNumberOfPositiveRoots());
  int beginning =
  useNegativeNilradical ?
  0 :
  ownerSS.getNumberOfPositiveRoots() + ownerSS.getRank();
  MemorySaving<List<int> > tempList;
  if (outputListOfGenerators == nullptr) {
    outputListOfGenerators = &tempList.getElement();
  }
  outputListOfGenerators->setSize(0);
  outputListOfGenerators->reserve(ownerSS.getNumberOfPositiveRoots());
  for (
    int i = beginning; i < beginning + ownerSS.getNumberOfPositiveRoots(); i ++
  ) {
    if (this->isNotInLevi(i)) {
      outputListOfGenerators->addOnTop(i);
    }
  }
  // bubble sort:
  for (int i = 0; i < outputListOfGenerators->size; i ++) {
    for (int j = i + 1; j < outputListOfGenerators->size; j ++) {
      if (
        ownerSS.universalEnvelopingGeneratorOrder[(*outputListOfGenerators)[i]
        ] >
        ownerSS.universalEnvelopingGeneratorOrder[(*outputListOfGenerators)[j]
        ]
      ) {
        outputListOfGenerators->swapTwoIndices(i, j);
      }
    }
  }
  for (int i = 0; i < outputListOfGenerators->size; i ++) {
    element.makeOneGeneratorCoefficientOne(
      outputListOfGenerators->objects[i], *this->owner
    );
    output.addOnTop(element);
  }
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::checkConsistency() {
  STACK_TRACE("ModuleSSalgebra::checkConsistency");
  ProgressReport report;
  MatrixTensor<Coefficient>
  left,
  right,
  output,
  otherOutput,
  matrix,
  diffMat;
  for (int i = 0; i < this->getOwner().getNumberOfGenerators(); i ++) {
    for (
      int j = 0; j < this->getOwner().getNumberOfGenerators(); j ++
    ) {
      left = this->getActionGeneratorIndex(i);
      right = this->getActionGeneratorIndex(j);
      output = right;
      output.lieBracketOnTheLeft(left);
      ElementSemisimpleLieAlgebra<Rational> leftGen, rightGen, outputGen;
      leftGen.makeGenerator(i, *this->owner);
      rightGen.makeGenerator(j, *this->owner);
      this->getOwner().lieBracket(leftGen, rightGen, outputGen);
      otherOutput.makeZero();
      for (int k = 0; k < outputGen.size(); k ++) {
        matrix =
        this->getActionGeneratorIndex(outputGen[k].generatorIndex);
        matrix *= outputGen.coefficients[k];
        otherOutput += matrix;
      }
      diffMat = otherOutput;
      diffMat -= output;
      if (!diffMat.isEqualToZero()) {
        global.fatal
        << "<br>["
        << left.toString()
        << ", "
        << right.toString()
        << "] = "
        << output.toString()
        << "<br> ["
        << leftGen.toString()
        << ", "
        << rightGen.toString()
        << "] = "
        << outputGen.toString()
        << "<br>"
        << outputGen.toString()
        << "->"
        << otherOutput.toString()
        << "<br>Something is wrong with the algorithm, a check fails! "
        << global.fatal;
      } else {
        std::stringstream currentStream;
        currentStream
        << "tested index "
        << i + 1
        << " out of "
        << this->getOwner().getNumberOfGenerators()
        << ", "
        << j + 1
        << " out of "
        << this->getOwner().getNumberOfGenerators();
        report.report(currentStream.str());
      }
    }
  }
  for (
    int i = 0; i < this->getOwner().getNumberOfPositiveRoots(); i ++
  ) {
    left = this->getActionGeneratorIndex(i);
    right =
    this->getActionGeneratorIndex(
      this->getOwner().getNumberOfGenerators() - 1 - i
    );
    left.transpose();
    left -= right;
    left = this->getActionGeneratorIndex(i);
    right.lieBracketOnTheLeft(left);
  }
  global.comments << "Consistency check passed successfully!";
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::getFDchar(
  CharacterSemisimpleLieAlgebraModule<Coefficient>& output
) {
  output.makeZero();
  if (this->highestWeightFundamentalCoordinatesBaseField.size <= 0) {
    return;
  }
  Weight<Coefficient> monomial;
  monomial.owner = &this->getOwner();
  WeylGroupData& weylGroup = this->getOwner().weylGroup;
  for (int i = 0; i < this->moduleWeightsSimpleCoordinates.size; i ++) {
    monomial.weightFundamentalCoordinates =
    weylGroup.getFundamentalCoordinatesFromSimple(
      this->moduleWeightsSimpleCoordinates[i]
    );
    output.addMonomial(
      monomial, this->generatingWordsGrouppedByWeight[i].size
    );
  }
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::expressAsLinearCombinationHomogenousElement(
  ElementUniversalEnveloping<Coefficient>& inputHomogeneous,
  ElementUniversalEnveloping<Coefficient>& outputHomogeneous,
  int indexInputBasis,
  const Vector<Coefficient>& substitutionHiGoesToIthElement,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  Vector<Coefficient> scalarProducts;
  List<MonomialUniversalEnveloping<Coefficient> >& inputBasis =
  this->generatingWordsGrouppedByWeight[indexInputBasis];
  scalarProducts.setSize(inputBasis.size);
  for (int i = 0; i < inputBasis.size; i ++) {
    std::stringstream currentStream;
    inputHomogeneous.highestWeightTransposeAntiAutomorphismBilinearForm(
      inputBasis[i],
      scalarProducts[i],
      &substitutionHiGoesToIthElement,
      ringUnit,
      ringZero,
      &currentStream
    );
  }
  this->bilinearFormsInverted[indexInputBasis].actOnVectorColumn(
    scalarProducts, ringZero
  );
  outputHomogeneous.makeZero(*this->owner);
  ElementUniversalEnveloping<Coefficient> element;
  for (int i = 0; i < scalarProducts.size; i ++) {
    element.makeZero(*this->owner);
    element.addMonomial(
      this->generatingWordsGrouppedByWeight[indexInputBasis][i],
      ringUnit
    );
    element *= scalarProducts[i];
    outputHomogeneous += element;
  }
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::reset() {
  this->actionsGeneratorsMatrix.setSize(0);
  this->actionsGenerators.setSize(0);
  this->computedGeneratorActions.initialize(0);
  this->owner = nullptr;
  this->generatingWordsNonReduced.clear();
  // Note: for some reason, the linker fails to resolve without the explicit
  // template
  // specialization below.
  // [Update:] made a bug report on this in the gcc bug tracker.
  // This issue has officially been recognized as a gcc bug. Hope to get a fix
  // soon.
  this->generatingWordsNonReducedInt.clear();
  this->generatingWordsWeightsPlusWeightFiniteDimensionalPart.setSize(0);
  this->allPaths.setSize(0);
  this->generatingWordsGrouppedByWeight.setSize(0);
  this->generatingWordsIntegersGrouppedByWeight.setSize(0);
  this->bilinearFormsAtEachWeightLevel.setSize(0);
  this->bilinearFormsInverted.setSize(0);
  // Vectors<Rational> weightsSimpleGens;
  this->highestWeightDualCoordinatesBaseField.setSize(0);
  this->highestWeightSimpleCoordinatesBaseField.setSize(0);
  this->highestWeightFundamentalCoordinatesBaseField.setSize(0);
  this->highestWeightFiniteDimensionalPartDualCoordinates.setSize(0);
  this->highestWeightFiniteDimensionalPartSimpleCoordinates.setSize(0);
  this->highestWeightFiniteDimensionalPartFundamentalCoordinates.setSize(0);
  // List<List<Matrix<Coefficient> > >
  this->moduleWeightsSimpleCoordinates.clear();
  this->characterOverH.makeZero();
  this->character.makeZero();
  this->parabolicSelectionNonSelectedAreElementsLevi.initialize(0);
  this->parabolicSelectionSelectedAreElementsLevi.initialize(0);
  this->highestWeightVectorNotation = "";
  // Note: for some reason, the linker fails to resolve without the
  // explicit template specialization below.
  // [Update:] This is now a recognized gcc bug.
  this->cachedPairs.clear();
  this->cachedTraces.setSize(0);
  this->flagIsInitialized = false;
  this->flagConjectureBholds = true;
  this->flagConjectureCholds = true;
  this->numberOfCachedPairsBeforeSimpleGenerators = 0;
  this->numberRationalMultiplicationsAndAdditionsBeforeSimpleGenerators = 0;
  this->maximumNumberOfCachedPairs = 1000000;
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::getAdActionHomogenousElement(
  ElementUniversalEnveloping<Coefficient>& inputHomogeneous,
  Vector<Rational>& weightUEElementSimpleCoordinates,
  List<List<ElementUniversalEnveloping<Coefficient> > >&
  outputSortedByArgumentWeight,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  Vector<Rational> targetWeight;
  outputSortedByArgumentWeight.setSize(
    this->generatingWordsGrouppedByWeight.size
  );
  ElementUniversalEnveloping<Coefficient> element;
  std::string generatorString = inputHomogeneous.toString();
  ProgressReport report;
  for (int i = 0; i < this->generatingWordsGrouppedByWeight.size; i ++) {
    List<MonomialUniversalEnveloping<Coefficient> >& currentWordList =
    this->generatingWordsGrouppedByWeight[i];
    List<ElementUniversalEnveloping<Coefficient> >& outputCurrentList =
    outputSortedByArgumentWeight[i];
    outputCurrentList.setSize(currentWordList.size);
    Vector<Rational>& currentWeight = this->moduleWeightsSimpleCoordinates[i];
    targetWeight = currentWeight + weightUEElementSimpleCoordinates;
    int index = this->moduleWeightsSimpleCoordinates.getIndex(targetWeight);
    for (int j = 0; j < currentWordList.size; j ++) {
      std::stringstream progressStream;
      progressStream
      << "Computing action of "
      << generatorString
      << " on weight layer "
      << i + 1
      << " out of "
      << this->generatingWordsGrouppedByWeight.size
      << ", word "
      << j + 1
      << " out of "
      << currentWordList.size
      << "...";
      report.report(progressStream.str());
      ElementUniversalEnveloping<Coefficient>& currentOutputWord =
      outputCurrentList[j];
      if (index == - 1) {
        currentOutputWord.makeZero(*this->owner);
      } else {
        element = inputHomogeneous;
        element.multiplyBy(currentWordList[j], ringUnit);
        this->expressAsLinearCombinationHomogenousElement(
          element,
          currentOutputWord,
          index,
          this->highestWeightDualCoordinatesBaseField,
          ringUnit,
          ringZero
        );
      }
      progressStream << " done!";
      report.report(progressStream.str());
    }
  }
}

template <class Coefficient>
void ElementTensorsGeneralizedVermas<Coefficient>::substitute(
  const PolynomialSubstitution<Rational>& substitution,
  ListReferences<ModuleSSalgebra<Coefficient> >& modules
) {
  ElementTensorsGeneralizedVermas<Coefficient> output;
  MonomialTensorGeneralizedVermas<Coefficient> monomial;
  output.makeZero();
  Coefficient coefficient;
  for (int i = 0; i < this->size(); i ++) {
    monomial = (*this)[i];
    monomial.substitute(substitution, modules);
    coefficient = this->coefficients[i];
    coefficient.substitute(substitution, 1, nullptr);
    output.addMonomial(monomial, coefficient);
  }
  *this = output;
}

template <class Coefficient>
ElementTensorsGeneralizedVermas<Coefficient>& ElementTensorsGeneralizedVermas<
  Coefficient
>::operator=(const ElementSumGeneralizedVermas<Coefficient>& other) {
  this->makeZero();
  MonomialTensorGeneralizedVermas<Coefficient> monomial;
  for (int i = 0; i < other.size(); i ++) {
    monomial = other[i];
    this->addMonomial(monomial, other.coefficients[i]);
  }
  return *this;
}

template <class Coefficient>
bool ElementTensorsGeneralizedVermas<Coefficient>::multiplyOnTheLeft(
  const ElementUniversalEnveloping<Coefficient>& element,
  ElementTensorsGeneralizedVermas<Coefficient>& output,
  SemisimpleLieAlgebra& ownerAlgebra,
  const Coefficient& ringUnit
) const {
  ElementTensorsGeneralizedVermas<Coefficient> buffer;
  output.makeZero();
  for (int i = 0; i < element.size(); i ++) {
    if (
      !this->multiplyOnTheLeft(
        element[i], buffer, ownerAlgebra, ringUnit
      )
    ) {
      ElementSumGeneralizedVermas<Coefficient> tempOutput;
      global.comments << "<hr>emergency mode!";
      for (int j = 0; j < this->size(); j ++) {
        const MonomialTensorGeneralizedVermas<Coefficient>& monomial = (*this)[
          j
        ];
        if (monomial.monomials.size != 1) {
          return false;
        }
        MonomialGeneralizedVerma<Coefficient>& singleMonomial =
        monomial.monomials[0];
        if (j == 0) {
          tempOutput.makeZero();
        }
        tempOutput.addMonomial(singleMonomial, this->coefficients[j]);
      }
      tempOutput.multiplyMeByUEElementOnTheLeft(element);
      output = tempOutput;
      return true;
    }
    buffer *= element.coefficients[i];
    output += buffer;
  }
  return true;
}

template <class Coefficient>
void ElementTensorsGeneralizedVermas<Coefficient>::makeHighestWeightVector(
  ModuleSSalgebra<Coefficient>& inputOwner, const Coefficient& ringUnit
) {
  MonomialTensorGeneralizedVermas<Coefficient> tensorMon;
  Coefficient currentCoefficient;
  currentCoefficient = ringUnit;
  tensorMon.monomials.setSize(1);
  MonomialGeneralizedVerma<Coefficient>& monomial = tensorMon.monomials[0];
  monomial.indexFDVector = inputOwner.generatingWordsNonReduced.size - 1;
  monomial.makeConstant(inputOwner);
  this->makeZero();
  this->addMonomial(tensorMon, ringUnit);
}

template <class Coefficient>
bool ElementTensorsGeneralizedVermas<Coefficient>::multiplyOnTheLeft(
  const MonomialUniversalEnveloping<Coefficient>& element,
  ElementTensorsGeneralizedVermas<Coefficient>& output,
  SemisimpleLieAlgebra& ownerAlgebra,
  const Coefficient& ringUnit
) const {
  STACK_TRACE("ElementTensorsGeneralizedVermas::multiplyOnTheLeft");
  if (&output == this) {
    global.fatal
    << "Output equals input, this is not supposed to happen. "
    << global.fatal;
  }
  output = *this;
  ElementTensorsGeneralizedVermas<Coefficient> buffer;
  for (int i = element.powers.size - 1; i >= 0; i --) {
    int power;
    if (!element.powers[i].isSmallInteger(&power)) {
      return false;
    }
    int index = element.generatorsIndices[i];
    for (int j = 0; j < power; j ++) {
      output.multiplyByElementLieAlg(buffer, ownerAlgebra, index, ringUnit);
      output = buffer;
    }
  }
  return true;
}

template <class Coefficient>
void ElementTensorsGeneralizedVermas<Coefficient>::multiplyByElementLieAlg(
  ElementTensorsGeneralizedVermas<Coefficient>& output,
  SemisimpleLieAlgebra& ownerAlgebra,
  int indexGenerator,
  const Coefficient& ringUnit
) const {
  output.makeZero();
  MonomialTensorGeneralizedVermas<Coefficient> accumulator;
  MonomialTensorGeneralizedVermas<Coefficient> monomialActedOn;
  ElementSumGeneralizedVermas<Coefficient> element;
  ElementUniversalEnveloping<Coefficient> generator;
  generator.makeOneGenerator(indexGenerator, ownerAlgebra, ringUnit);
  Coefficient coefficient;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialTensorGeneralizedVermas<Coefficient>& monomial = (*this)[i];
    accumulator.monomials.setSize(0);
    for (int j = 0; j < monomial.monomials.size; j ++) {
      element.makeZero();
      element.addMonomial(monomial.monomials[j], ringUnit);
      element.multiplyMeByUEElementOnTheLeft(generator);
      for (int k = 0; k < element.size(); k ++) {
        coefficient = this->coefficients[i];
        coefficient *= element.coefficients[k];
        monomialActedOn = accumulator;
        monomialActedOn *= element[k];
        for (int l = j + 1; l < monomial.monomials.size; l ++) {
          monomialActedOn *= monomial.monomials[l];
        }
        output.addMonomial(monomialActedOn, coefficient);
      }
      accumulator *= monomial.monomials[j];
    }
  }
}

template <class Coefficient>
void ElementTensorsGeneralizedVermas<Coefficient>::tensorOnTheRight(
  const ElementTensorsGeneralizedVermas<Coefficient>& right
) {
  STACK_TRACE("ElementTensorsGeneralizedVermas::tensorOnTheRight");
  if (right.isEqualToZero()) {
    this->makeZero();
    return;
  }
  int maximumMonomialsFinal = this->size() * right.size();
  ElementTensorsGeneralizedVermas<Coefficient> output;
  MonomialTensorGeneralizedVermas<Coefficient> monomial;
  output.makeZero();
  output.setExpectedSize(maximumMonomialsFinal);
  Coefficient coefficient;
  for (int i = 0; i < right.size(); i ++) {
    for (int j = 0; j < this->size(); j ++) {
      monomial = (*this)[j];
      monomial *= right[i];
      coefficient = this->coefficients[j];
      coefficient *= right.coefficients[i];
      output.addMonomial(monomial, coefficient);
    }
  }
  *this = output;
}

template <class Coefficient>
std::string ModuleSSalgebra<Coefficient>::toString(FormatExpressions* format)
const {
  STACK_TRACE("ModuleSSalgebra::toString");
  if (this->owner == nullptr) {
    return "(Error: module not initialized)";
  }
  SemisimpleLieAlgebra& algebra = *this->owner;
  WeylGroupData& weylGroup = algebra.weylGroup;
  std::stringstream out;
  out
  << "<br>Semisimple Lie algebra acting on generalized Verma module: "
  << algebra.toStringLieAlgebraName()
  << ".";
  out
  << "<br>Parabolic selection: "
  << this->parabolicSelectionNonSelectedAreElementsLevi.toString();
  out
  << "<br>Highest weight of Generalized "
  << "Verma module in fundamental coordinates: "
  << this->highestWeightFundamentalCoordinatesBaseField.toString();
  out
  << "<br>In simple coordinates: "
  << this->highestWeightSimpleCoordinatesBaseField.toString();
  out
  << "<br>Finite dimensional part h. w. fundamental coordinates: "
  << this->highestWeightFiniteDimensionalPartFundamentalCoordinates.toString();
  out
  << "<br>Finite dimensinoal part h. w. simple coords: "
  << this->highestWeightFiniteDimensionalPartSimpleCoordinates.toString();
  out << "<br>Inducing module character (over the Cartan subalgebra): ";
  FormatExpressions latexFormat;
  latexFormat.flagUseLatex = true;
  latexFormat.flagUseHTML = false;
  latexFormat.maximumLineLength = 0;
  if (this->characterOverH.size() < 100) {
    out
    << HtmlRoutines::getMathNoDisplay(
      this->characterOverH.toString(&latexFormat)
    );
  } else {
    out << this->characterOverH.toString();
  }
  out
  << "<br>Dimensionn of the finite dimensional part of the module: "
  << this->getDimension();
  out << "<br>A module basis follows.";
  out
  << "<table><tr><td>Monomial label</td>"
  << "<td>Definition</td><td>Littelmann path string</td></tr>";
  ElementWeylGroup tempWElement;
  int wordCounter = 0;
  SimpleReflection aGen;
  for (int i = 0; i < this->generatingWordsGrouppedByWeight.size; i ++) {
    List<MonomialUniversalEnveloping<Coefficient> >& currentList =
    this->generatingWordsGrouppedByWeight[i];
    List<MonomialTensor<int, HashFunctions::hashFunction> >& currentListInt =
    this->generatingWordsIntegersGrouppedByWeight[i];
    for (int j = 0; j < currentList.size; j ++) {
      wordCounter ++;
      tempWElement.generatorsLastAppliedFirst.setSize(
        currentListInt[j].generatorsIndices.size
      );
      for (
        int k = 0; k < currentListInt[j].generatorsIndices.size; k ++
      ) {
        aGen.makeSimpleReflection(
          weylGroup.rootsOfBorel.size -
          1 -
          currentListInt[j].generatorsIndices[k]
        );
        tempWElement.generatorsLastAppliedFirst[k] = aGen;
      }
      out
      << "<tr><td>m_{ "
      << wordCounter
      << "} </td><td>"
      << currentList[j].toString(&global.defaultFormat.getElement())
      << " v_\\lambda</td><td>"
      << tempWElement.toString()
      << "</td> </tr>";
    }
  }
  out << "</table>";
  out << "<br>Character: " << this->character.toString();
  out
  << "<br>Computed generator actions ("
  << this->computedGeneratorActions.cardinalitySelection
  << " out of "
  << this->actionsGeneratorsMatrix.size
  << " computed actions) follow. "
  << "Note that generator actions are computed on demand, only the simple "
  << "Chevalley generators are computed by default. ";
  out << "<table><tr><td>Generator</td><td>Action</td></tr>";
  ElementSemisimpleLieAlgebra<Rational> semisimpleLieAlgebraElement;
  for (int i = 0; i < this->actionsGeneratorsMatrix.size; i ++) {
    if (this->computedGeneratorActions.selected[i]) {
      semisimpleLieAlgebraElement.makeGenerator(i, algebra);
      out << "<tr>";
      out
      << "<td>"
      << HtmlRoutines::getMathNoDisplay(
        semisimpleLieAlgebraElement.toString(format)
      )
      << "</td>";
      out << "<td>";
      if (this->getDimension() < 28) {
        Matrix<Coefficient> outputMat;
        this->actionsGeneratorsMatrix[i].getMatrix(
          outputMat, this->getDimension()
        );
        out
        << HtmlRoutines::getMathNoDisplay(
          outputMat.toString(&latexFormat), 5000
        )
        << " = ";
        out << this->actionsGeneratorsMatrix[i].toString();
      } else {
        out << this->actionsGeneratorsMatrix[i].toString();
      }
      out << "</td></tr>";
    }
  }
  out << "</table>";
  DrawingVariables drawingVariables;
  this->characterOverH.drawMeAssumeCharIsOverCartan(
    weylGroup, drawingVariables
  );
  out
  << " A picture of the weight support follows. "
  << drawingVariables.getHTMLDiv(weylGroup.getDimension(), true);
  bool isBad = false;
  for (int k = 0; k < this->bilinearFormsAtEachWeightLevel.size; k ++) {
    Matrix<Coefficient>& bilinearForm =
    this->bilinearFormsAtEachWeightLevel[k];
    Matrix<Coefficient>& bilinearFormInverted = this->bilinearFormsInverted[k];
    out
    << "<hr>weight in simple coords: "
    << this->moduleWeightsSimpleCoordinates[k].toString();
    List<MonomialUniversalEnveloping<Coefficient> >& currentList =
    this->generatingWordsGrouppedByWeight[k];
    for (int i = 0; i < currentList.size; i ++) {
      MonomialUniversalEnveloping<Coefficient>& currentElement =
      currentList[i];
      out
      << "<br>monomial "
      << i + 1
      << ": "
      << currentElement.toString(&global.defaultFormat.getElement());
    }
    out
    << "; Matrix of Shapovalov form associated to current weight level: <br> "
    << bilinearForm.toString(&global.defaultFormat.getElement());
    /*    if (!bilinearForm.isPositiveDefinite()) {
      monomialDetailStream << "<b>Is not positive definite!</b>";
      this->flagConjectureCholds = false;
    }
    else
      monomialDetailStream << " (positive definite)";*/
    if (!bilinearForm.isNonNegativeAllEntries()) {
      out << "<b>Has negative entries</b>";
    } else {
      out << " (positive entries only )";
    }
    out << " corresonding inverted matrix:<br>";
    if (bilinearFormInverted.numberOfRows > 0) {
      out
      << bilinearFormInverted.toString(
        &global.defaultFormat.getElement()
      );
    } else {
      out << "<b>The matrix of the bilinear form is not invertible!</b>";
      isBad = true;
    }
  }
  if (isBad) {
    out
    << "<br>Error: the Littelmann-path induced "
    << "monomials do not give a monomial basis. ";
  }
  out
  << "<br>Cached Shapovalov products before generator action computation: "
  << this->cachedPairs.size
  << ". Dimension : "
  << this->getDimension();
  out
  << "<br>Cached Shapovalov products final: "
  << this->cachedPairs.size
  << "; dimension : "
  << this->getDimension();
  return out.str();
}

template <class Coefficient>
bool ModuleSSalgebra<Coefficient>::isNotInParabolic(int generatorIndex) {
  Vector<Rational> weight =
  this->getOwner().getWeightOfGenerator(generatorIndex);
  for (
    int j = 0; j < this->parabolicSelectionNonSelectedAreElementsLevi.
    cardinalitySelection; j ++
  ) {
    if (
      !(
        weight[
          this->parabolicSelectionNonSelectedAreElementsLevi.elements[j]
        ] <
        0
      )
    ) {
      return true;
    }
  }
  return false;
}

template <class Coefficient>
bool ModuleSSalgebra<Coefficient>::isNotInLevi(int generatorIndex) {
  Vector<Rational> weight =
  this->getOwner().getWeightOfGenerator(generatorIndex);
  for (
    int j = 0; j < this->parabolicSelectionNonSelectedAreElementsLevi.
    cardinalitySelection; j ++
  ) {
    if (
      !weight[
        this->parabolicSelectionNonSelectedAreElementsLevi.elements[j]
      ].isEqualToZero()
    ) {
      return true;
    }
  }
  return false;
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::getGenericUnMinusElement(
  bool shiftPowersByNumberOfVariablesBaseField,
  ElementUniversalEnveloping<RationalFraction<Rational> >& output,
  bool useNilWeight,
  bool ascending
) {
  STACK_TRACE("ModuleSSalgebra::getGenericUnMinusElement");
  List<ElementUniversalEnveloping<Coefficient> > elementsNilradical;
  this->getElementsNilradical(
    elementsNilradical, true, useNilWeight, ascending
  );
  RationalFraction<Rational> rationalFunction;
  output.makeZero(*this->owner);
  MonomialUniversalEnveloping<RationalFraction<Rational> > monomial;
  monomial.makeConstant();
  int variableShift = 0;
  if (shiftPowersByNumberOfVariablesBaseField) {
    variableShift = this->minimalNumberOfVariables();
  }
  for (int i = 0; i < elementsNilradical.size; i ++) {
    rationalFunction.makeOneLetterMonomial(i + variableShift, 1);
    monomial.multiplyByGeneratorPowerOnTheRight(
      elementsNilradical[i][0].generatorsIndices[0],
      rationalFunction
    );
  }
  rationalFunction.makeOne();
  output.addMonomial(monomial, rationalFunction);
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::getGenericUnMinusElement(
  bool shiftPowersByNumberOfVariablesBaseField,
  ElementUniversalEnveloping<Polynomial<Rational> >& output,
  bool useNilWeight,
  bool ascending
) {
  STACK_TRACE("ModuleSSalgebra::getGenericUnMinusElement");
  List<ElementUniversalEnveloping<Coefficient> > elementsNilradical;
  this->getElementsNilradical(
    elementsNilradical, true, nullptr, useNilWeight, ascending
  );
  Polynomial<Rational> unknownExponent;
  output.makeZero(*this->owner);
  MonomialUniversalEnveloping<Polynomial<Rational> > monomial;
  monomial.makeOne(*this->owner);
  int variableShift = 0;
  if (shiftPowersByNumberOfVariablesBaseField) {
    variableShift = this->minimalNumberOfVariables();
  }
  for (int i = 0; i < elementsNilradical.size; i ++) {
    unknownExponent.makeMonomial(i + variableShift, 1, 1);
    monomial.multiplyByGeneratorPowerOnTheRight(
      elementsNilradical[i][0].generatorsIndices[0],
      unknownExponent
    );
  }
  unknownExponent.makeOne();
  output.addMonomial(monomial, unknownExponent);
}

template <class Coefficient>
bool ModuleSSalgebra<Coefficient>::
getActionGeneralizedVermaModuleAsDifferentialOperator(
  ElementSemisimpleLieAlgebra<Rational>& inputElement,
  QuasiDifferentialOperator<Rational>& output,
  bool useNilWeight,
  bool ascending
) {
  STACK_TRACE(
    "ModuleSSalgebra_CoefficientType::"
    "getActionGeneralizedVermaModuleAsDifferentialOperator"
  );
  List<ElementUniversalEnveloping<Coefficient> > elementsNilradical;
  List<int> indicesNilrad;
  this->getElementsNilradical(
    elementsNilradical, true, &indicesNilrad, useNilWeight, ascending
  );
  ElementUniversalEnveloping<Polynomial<Rational> > genericElement;
  ElementUniversalEnveloping<Polynomial<Rational> > result;
  this->getGenericUnMinusElement(
    true, genericElement, useNilWeight, ascending
  );
  result.assignElementLieAlgebra(inputElement, *this->owner, 1);
  Polynomial<Rational> onePolynomial;
  onePolynomial.makeConstant(1);
  genericElement.simplify(onePolynomial);
  result *= genericElement;
  result.simplify(onePolynomial);
  MatrixTensor<Polynomial<Rational> > endoPart;
  MatrixTensor<Polynomial<Rational> > tempMT;
  MatrixTensor<Polynomial<Rational> > idMT;
  idMT.makeIdentitySpecial();
  MatrixTensor<RationalFraction<Rational> > matrix;
  int varShift = this->minimalNumberOfVariables();
  ElementWeylAlgebra<Rational> weylPartSummand;
  ElementWeylAlgebra<Rational> exponentContribution;
  ElementWeylAlgebra<Rational> oneIndexContribution;
  ElementWeylAlgebra<Rational> eulerOperatorContribution;
  Polynomial<Rational> tempP1;
  Polynomial<Rational> negativeExponentDenominatorContribution;
  Polynomial<Rational> coefficient;
  QuasiDifferentialMononomial monQDO;
  QuasiDifferentialMononomial monQDO2;
  output.makeZero();
  Rational currentShift;
  for (int i = 0; i < result.size(); i ++) {
    // problemCounter ++;
    const MonomialUniversalEnveloping<Polynomial<Rational> >& monomial =
    result[i];
    endoPart = idMT;
    for (
      int j = monomial.powers.size - 1; j >= indicesNilrad.size; j --
    ) {
      int power = 0;
      if (!monomial.powers[j].isSmallInteger(&power)) {
        return false;
      }
      matrix =
      this->getActionGeneratorIndex(monomial.generatorsIndices[j]);
      tempMT.makeZero();
      for (int k = 0; k < matrix.size(); k ++) {
        if (
          matrix.coefficients[k].expressionType ==
          RationalFraction<Rational>::TypeExpression::typeRationalFraction
        ) {
          return false;
        }
        matrix.coefficients[k].getNumerator(tempP1);
        tempMT.addMonomial(matrix[k], tempP1);
      }
      MathRoutines::raiseToPower(tempMT, power, idMT);
      endoPart *= tempMT;
    }
    exponentContribution.makeOne();
    coefficient = result.coefficients[i];
    for (int j = 0; j < indicesNilrad.size; j ++) {
      monomial.powers[j].constantTerm(currentShift);
      ElementWeylAlgebra<Rational>::
      getStandardOrderDifferentialOperatorCorrespondingToNRaisedTo(
        currentShift,
        j + varShift,
        oneIndexContribution,
        negativeExponentDenominatorContribution
      );
      exponentContribution *= oneIndexContribution;
      coefficient.divideBy(
        negativeExponentDenominatorContribution,
        coefficient,
        tempP1,
        &MonomialPolynomial::orderDefault()
      );
      if (!tempP1.isEqualToZero()) {
        global.fatal
        << "This is a mathematical error! "
        << "Something is very wrong with "
        << "embedding semisimple Lie algebras in Weyl algebras. "
        << global.fatal;
      }
    }
    Rational rationalCoefficient;
    for (int l = 0; l < coefficient.size(); l ++) {
      if (
        !this->getActionEulerOperatorPart(
          coefficient[l], eulerOperatorContribution
        )
      ) {
        return false;
      }
      weylPartSummand = exponentContribution;
      weylPartSummand *= eulerOperatorContribution;
      weylPartSummand *= coefficient.coefficients[l];
      for (int j = 0; j < weylPartSummand.size(); j ++) {
        for (int k = 0; k < endoPart.size(); k ++) {
          monQDO.matrixMonomial = endoPart[k];
          monQDO.weylMonomial = weylPartSummand[j];
          Polynomial<Rational>& currentEndoCoeff = endoPart.coefficients[k];
          for (int m = 0; m < currentEndoCoeff.size(); m ++) {
            monQDO2 = monQDO;
            monQDO2.weylMonomial.polynomialPart *= currentEndoCoeff[m];
            rationalCoefficient = currentEndoCoeff.coefficients[m];
            rationalCoefficient *= weylPartSummand.coefficients[j];
            output.addMonomial(monQDO2, rationalCoefficient);
          }
        }
      }
    }
  }
  return true;
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::splitFDpartOverFKLeviRedSubalg(
  HomomorphismSemisimpleLieAlgebra& homomorphism,
  Selection& LeviInSmall,
  List<ElementUniversalEnveloping<Coefficient> >* outputEigenVectors,
  Vectors<Coefficient>* outputWeightsFundamentalCoordinates,
  Vectors<Coefficient>* outputEigenSpace,
  std::stringstream* comments
) {
  STACK_TRACE("ModuleSSalgebra::splitFDpartOverFKLeviRedSubalg");
  if (this->character.size() != 1) {
    if (comments != nullptr) {
      std::stringstream out;
      out
      << "I have been instructed only to split "
      << "modules that are irreducible over the ambient Lie algebra. ";
      out
      << "Instead I got the character "
      << this->character.toString()
      << " ("
      << this->character.size()
      << " monomials)";
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
  std::stringstream currentStream1;
  currentStream1
  << "Started splitting the f.d. part of the "
  << homomorphism.coDomainAlgebra().toStringLieAlgebraName()
  << "-module with highest weight in fund coords "
  << this->character[0].weightFundamentalCoordinates.toString();
  ProgressReport report;
  report.report(currentStream1.str());
  List<List<Vector<Coefficient> > > eigenSpacesPerSimpleGenerator;
  Selection invertedLeviInSmall;
  invertedLeviInSmall = LeviInSmall;
  invertedLeviInSmall.invertSelection();
  eigenSpacesPerSimpleGenerator.setSize(
    invertedLeviInSmall.cardinalitySelection
  );
  Vectors<Coefficient> tempSpace1, tempSpace2;
  MemorySaving<Vectors<Coefficient> > tempEigenVects;
  Vectors<Coefficient>& finalEigenSpace = (outputEigenSpace == nullptr) ?
  tempEigenVects.getElement() :
  *outputEigenSpace;
  finalEigenSpace.setSize(0);
  if (invertedLeviInSmall.cardinalitySelection == 0) {
    finalEigenSpace.makeEiBasis(this->getDimension());
  }
  for (int i = 0; i < invertedLeviInSmall.cardinalitySelection; i ++) {
    ElementSemisimpleLieAlgebra<Rational>& currentElement =
    homomorphism.imagesPositiveSimpleChevalleyGenerators[
      invertedLeviInSmall.elements[i]
    ];
    MatrixTensor<Coefficient> currentOp, matrix;
    currentOp.makeZero();
    for (int j = 0; j < currentElement.size(); j ++) {
      matrix =
      this->getActionGeneratorIndex(currentElement[j].generatorIndex);
      matrix *= currentElement.coefficients[j];
      currentOp += matrix;
    }
    std::stringstream currentStream3;
    double timeAtStart1 = global.getElapsedSeconds();
    currentStream3
    << "Computing eigenspace corresponding to "
    << currentElement.toString()
    << "...";
    report.report(currentStream3.str());
    Matrix<Coefficient> currentOpMat;
    currentOp.getMatrix(currentOpMat, this->getDimension());
    currentOpMat.getZeroEigenSpace(eigenSpacesPerSimpleGenerator[i]);
    currentStream3
    << " done in "
    << global.getElapsedSeconds() - timeAtStart1
    << " seconds. ";
    report.report(currentStream3.str());
    if (i == 0) {
      finalEigenSpace = eigenSpacesPerSimpleGenerator[i];
    } else {
      std::stringstream currentStream4;
      double timeAtStart2 = global.getElapsedSeconds();
      currentStream4
      << "Intersecting with eigenspace corresponding to "
      << currentElement.toString()
      << "...";
      tempSpace1 = finalEigenSpace;
      report.report(currentStream4.str());
      tempSpace2 = eigenSpacesPerSimpleGenerator[i];
      finalEigenSpace.intersectTwoLinearSpaces(
        tempSpace1, tempSpace2, finalEigenSpace
      );
      currentStream4
      << " done in "
      << global.getElapsedSeconds() - timeAtStart2
      << " seconds. ";
      report.report(currentStream4.str());
    }
  }
  out << "<br>Eigenvectors: <table>";
  std::stringstream readyForLatexConsumption;
  readyForLatexConsumption << "\\begin{tabular}{|lll|}\n <br>";
  readyForLatexConsumption
  << "\\hline\\multicolumn{3}{|c|}{Highest weight $"
  << this->highestWeightFundamentalCoordinatesBaseField.toStringLetterFormat(
    "\\omega"
  )
  << "$}\\\\\n<br>";
  readyForLatexConsumption
  << "weight fund. coord.& singular vector \\\\\\hline\n<br>";
  Vector<Coefficient> currentWeight;
  Vector<Coefficient> highestWeightFundamentalCoordinatesNilradicalPart;
  highestWeightFundamentalCoordinatesNilradicalPart =
  this->highestWeightFundamentalCoordinatesBaseField;
  highestWeightFundamentalCoordinatesNilradicalPart -=
  this->highestWeightFiniteDimensionalPartFundamentalCoordinates;
  ElementUniversalEnveloping<Coefficient> currentElement, element;
  if (outputEigenVectors != nullptr) {
    outputEigenVectors->setSize(0);
  }
  for (int j = 0; j < finalEigenSpace.size; j ++) {
    out << "<tr><td>";
    currentElement.makeZero(this->getOwner());
    Vector<Coefficient>& currentVect = finalEigenSpace[j];
    int lastNonZeroIndex = - 1;
    for (int i = 0; i < currentVect.size; i ++) {
      if (!(currentVect[i].isEqualToZero())) {
        element.makeZero(this->getOwner());
        element.addMonomial(this->generatingWordsNonReduced[i], 1);
        element *= currentVect[i];
        currentElement += element;
        lastNonZeroIndex = i;
      }
    }
    currentWeight =
    homomorphism.coDomainAlgebra().weylGroup.
    getFundamentalCoordinatesFromSimple(
      this->generatingWordsWeightsPlusWeightFiniteDimensionalPart[
        lastNonZeroIndex
      ]
    );
    // <-implicit type conversion here
    currentWeight += highestWeightFundamentalCoordinatesNilradicalPart;
    readyForLatexConsumption
    << "$"
    << currentWeight.toStringLetterFormat("\\omega")
    << "$&$"
    << currentVect.toStringLetterFormat("m")
    << "$";
    if (currentElement.size() > 1) {
      out << "(";
    }
    if (outputEigenVectors != nullptr) {
      outputEigenVectors->addOnTop(currentElement);
    }
    if (outputWeightsFundamentalCoordinates != nullptr) {
      outputWeightsFundamentalCoordinates->addOnTop(currentWeight);
    }
    out << currentElement.toString(&global.defaultFormat.getElement());
    if (currentElement.size() > 1) {
      out << ")";
    }
    out << " v_\\lambda";
    out
    << "</td><td>(weight: "
    << currentWeight.toStringLetterFormat("\\omega")
    << ")</td></tr>";
    readyForLatexConsumption << "\\\\\n<br>";
  }
  out << "</table>";
  readyForLatexConsumption << "\\hline \n<br> \\end{tabular}";
  out << "<br>Your ready for LaTeX consumption text follows.<br>";
  out << readyForLatexConsumption.str();
  report.report("splitFDpartOverFKLeviRedSubalg done!");
  if (comments != nullptr) {
    *comments << out.str();
  }
}

template <class Coefficient>
bool ModuleSSalgebra<Coefficient>::getActionEulerOperatorPart(
  const MonomialPolynomial& coefficient,
  ElementWeylAlgebra<Rational>& output
) {
  STACK_TRACE("ModuleSSalgebra::getActionEulerOperatorPart");
  int powerMonomialCoefficient = 0;
  ElementWeylAlgebra<Rational> monomialContribution;
  output.makeOne();
  for (int i = 0; i < coefficient.minimalNumberOfVariables(); i ++) {
    if (!coefficient(i).isSmallInteger(&powerMonomialCoefficient)) {
      global.fatal
      << "Getting Euler operator part of action on generalized Verma module: "
      << "I have an exponent with non-small integer entry. "
      << global.fatal;
    }
    monomialContribution.makexidj(i, i);
    monomialContribution.raiseToPower(powerMonomialCoefficient);
    output *= monomialContribution;
  }
  return true;
}

template <class Coefficient>
std::string MonomialTensorGeneralizedVermas<Coefficient>::toString(
  FormatExpressions* format, bool includeV
) const {
  std::stringstream out;
  if (this->monomials.size > 1) {
    for (int i = 0; i < this->monomials.size; i ++) {
      bool ishwv = this->monomials[i].isHighestWeightVector();
      if (!ishwv) {
        out << "(";
      }
      out << this->monomials[i].toString(format, includeV);
      if (!ishwv) {
        out << ")";
      }
      if (i != this->monomials.size - 1) {
        out << "\\otimes ";
      }
    }
  } else {
    out << this->monomials[0].toString(format, includeV);
  }
  return out.str();
}

template <class Coefficient>
std::string MonomialGeneralizedVerma<Coefficient>::toString(
  FormatExpressions* format, bool includeV
) const {
  if (this->owner == nullptr) {
    global.fatal
    << "Non-initialized generalized Verma monomial (owner is 0)."
    << global.fatal;
  }
  ModuleSSalgebra<Coefficient>& module = *this->owner;
  std::string coefficientString;
  if (coefficientString == "1") {
    coefficientString = "";
  }
  if (coefficientString == "- 1" || coefficientString == "-1") {
    coefficientString = "-";
  }
  coefficientString += this->monomialCoefficientOne.toString(format);
  if (coefficientString == "1") {
    coefficientString = "";
  }
  if (coefficientString == "- 1" || coefficientString == "-1") {
    coefficientString = "-";
  }
  bool needsCdot = (
    coefficientString != "1" &&
    coefficientString != "-" &&
    coefficientString != ""
  );
  std::stringstream out;
  out << coefficientString;
  coefficientString =
  module.generatingWordsNonReduced[this->indexFDVector].toString(format);
  if (coefficientString != "1") {
    out << coefficientString;
  }
  needsCdot = needsCdot || (
    coefficientString != "1" && coefficientString != "-"
  );
  if (needsCdot) {
    out << "\\cdot ";
  }
  if (includeV) {
    out << module.elementToStringHighestWeightVector(format);
  }
  return out.str();
}

template <class Coefficient>
void MonomialGeneralizedVerma<Coefficient>::substitute(
  const PolynomialSubstitution<Rational>& substitution,
  ListReferences<ModuleSSalgebra<Coefficient> >& modules
) {
  this->monomialCoefficientOne.substitute(substitution);
  ModuleSSalgebra<Coefficient> newOwner;
  newOwner = *this->owner;
  newOwner.substitute(substitution);
  int newModIndex = modules.addNoRepetitionOrReturnIndexFirst(newOwner);
  this->owner = &modules[newModIndex];
}

template <class Coefficient>
void MonomialTensorGeneralizedVermas<Coefficient>::operator=(
  const MonomialGeneralizedVerma<Coefficient>& other
) {
  this->monomials.setSize(1);
  this->monomials[0] = other;
}

template <class Coefficient>
void MonomialGeneralizedVerma<Coefficient>::multiplyMeByUEElementOnTheLeft(
  const ElementUniversalEnveloping<Coefficient>& elementUniversalEnveloping,
  ElementSumGeneralizedVermas<Coefficient>& output
) const {
  STACK_TRACE(
    "MonomialGeneralizedVerma::"
    "multiplyMeByUEElementOnTheLeft"
  );
  MonomialGeneralizedVerma<Coefficient> monomial;
  output.makeZero();
  ElementSumGeneralizedVermas<Coefficient> buffer;
  ProgressReport report;
  if (!this->getOwner().owner->flagHasNilradicalOrder) {
    global.fatal
    << "Calling generalized verma module "
    << "simplification requires nilradical order on the generators. "
    << global.fatal;
  }
  for (int j = 0; j < elementUniversalEnveloping.size(); j ++) {
    monomial.monomialCoefficientOne = elementUniversalEnveloping[j];
    monomial.monomialCoefficientOne *= this->monomialCoefficientOne;
    monomial.owner = this->owner;
    monomial.indexFDVector = this->indexFDVector;
    monomial.owner = this->owner;
    std::stringstream reportStream;
    reportStream
    << "reducing mon: "
    << monomial.toString()
    << ", index"
    << j + 1
    << " out of "
    << elementUniversalEnveloping.size()
    << "...";
    report.report(reportStream.str());
    monomial.reduceMe(buffer);
    reportStream << " done.";
    report.report(reportStream.str());
    buffer *= elementUniversalEnveloping.coefficients[j];
    output += buffer;
  }
}

template <class Coefficient>
void ElementSumGeneralizedVermas<Coefficient>::multiplyMeByUEElementOnTheLeft(
  const ElementUniversalEnveloping<Coefficient>& elementUniversalEnveloping
) {
  STACK_TRACE(
    "ElementSumGeneralizedVermas<Coefficient>::multiplyMeByUEElementOnTheLeft"
  );
  ElementSumGeneralizedVermas<Coefficient> buffer;
  ElementSumGeneralizedVermas<Coefficient> accumulator;
  accumulator.makeZero();
  for (int i = 0; i < this->size(); i ++) {
    (*this)[i].multiplyMeByUEElementOnTheLeft(
      elementUniversalEnveloping, buffer
    );
    buffer *= this->coefficients[i];
    accumulator += buffer;
  }
  *this = accumulator;
}

template <class Coefficient>
void MonomialGeneralizedVerma<Coefficient>::reduceMe(
  ElementSumGeneralizedVermas<Coefficient>& output
) const {
  STACK_TRACE("MonomialGeneralizedVerma::reduceMe");
  ModuleSSalgebra<Coefficient>& module = *this->owner;
  output.makeZero();
  MonomialUniversalEnveloping<Coefficient> monomial;
  monomial = this->monomialCoefficientOne;
  monomial *= module.generatingWordsNonReduced[this->indexFDVector];
  int indexCheck = module.generatingWordsNonReduced.getIndex(monomial);
  if (!this->owner->owner->flagHasNilradicalOrder) {
    global.fatal << "Owner needs nilradical order!!!" << global.fatal;
  }
  if (indexCheck != - 1) {
    MonomialGeneralizedVerma<Coefficient> basisMonomial;
    basisMonomial.makeConstant(*this->owner);
    basisMonomial.indexFDVector = indexCheck;
    output.addMonomial(basisMonomial, 1);
    return;
  }
  ElementUniversalEnveloping<Coefficient> elementUniversalEnveloping;
  elementUniversalEnveloping.makeZero(*this->getOwner().owner);
  elementUniversalEnveloping.addMonomial(this->monomialCoefficientOne, 1);
  elementUniversalEnveloping.simplify();
  MonomialGeneralizedVerma<Coefficient> newMonomial;
  MatrixTensor<Coefficient> matrix1;
  MatrixTensor<Coefficient> matrix2;
  ProgressReport report;
  Coefficient coefficient;
  for (int l = 0; l < elementUniversalEnveloping.size(); l ++) {
    monomial = elementUniversalEnveloping[l];
    matrix1.makeIdentitySpecial();
    for (int k = monomial.powers.size - 1; k >= 0; k --) {
      std::stringstream reportStream;
      reportStream
      << "accounting monomial "
      << monomial.toString()
      << " of index "
      << l + 1
      << " out of "
      << elementUniversalEnveloping.size()
      << " and letter index "
      << monomial.powers.size - k
      << " out of "
      << monomial.powers.size
      << "...";
      report.report(reportStream.str());
      int power = - 1;
      if (!monomial.powers[k].isSmallInteger(&power)) {
        break;
      }
      int index = monomial.generatorsIndices[k];
      if (module.hasFreeAction(index)) {
        break;
      }
      matrix2 = matrix1;
      matrix1 = module.getActionGeneratorIndex(index);
      matrix1.raiseToPower(power);
      matrix1 *= matrix2;
      monomial.powers.size --;
      monomial.generatorsIndices.size --;
      reportStream << "done!";
      report.report(reportStream.str());
    }
    newMonomial.owner = this->owner;
    for (int i = 0; i < matrix1.size(); i ++) {
      int otherIndex = - 1;
      if (matrix1[i].dualIndex == this->indexFDVector) {
        otherIndex = matrix1[i].vIndex;
      }
      if (matrix1[i].isIdentity) {
        otherIndex = this->indexFDVector;
      }
      if (otherIndex != - 1) {
        newMonomial.monomialCoefficientOne = monomial;
        newMonomial.indexFDVector = otherIndex;
        coefficient = elementUniversalEnveloping.coefficients[l];
        coefficient *= matrix1.coefficients[i];
        output.addMonomial(newMonomial, coefficient);
      }
    }
  }
  module.getOwner().orderStandardAscending();
}

template <class Coefficient>
void ElementSumGeneralizedVermas<Coefficient>::makeHighestWeightVector(
  ModuleSSalgebra<Coefficient>& inputOwner, const Coefficient& ringUnit
) {
  this->makeZero();
  MonomialGeneralizedVerma<Coefficient> monomial;
  monomial.indexFDVector = inputOwner.generatingWordsNonReduced.size - 1;
  monomial.monomialCoefficientOne.makeOne(inputOwner.getOwner());
  monomial.owner = &inputOwner;
  this->addMonomial(monomial, ringUnit);
}

template <class Coefficient>
bool ElementSumGeneralizedVermas<Coefficient>::
extractElementUniversalEnveloping(
  ElementUniversalEnveloping<Coefficient>& output,
  SemisimpleLieAlgebra& owner
) {
  output.makeZero(owner);
  ModuleSSalgebra<Coefficient>* modulePointer = nullptr;
  MonomialUniversalEnveloping<Coefficient> intermediateMonomial;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialGeneralizedVerma<Coefficient>& monomial = (*this)[i];
    if (i == 0) {
      modulePointer = monomial.owner;
    }
    if (monomial.owner != modulePointer) {
      return false;
    }
    intermediateMonomial = monomial.monomialCoefficientOne;
    intermediateMonomial *=
    monomial.getOwner().generatingWordsNonReduced[monomial.indexFDVector];
    output.addMonomial(intermediateMonomial, this->coefficients[i]);
  }
  return true;
}

#endif // header_math_extra_modules_semisimple_lie_algebras_implementation_ALREADY_INCLUDED
