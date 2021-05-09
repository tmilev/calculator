// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfImplementationHeaderSemisimpleLieAlgebrasIncluded
#define vpfImplementationHeaderSemisimpleLieAlgebrasIncluded

#include "math_extra_semisimple_lie_algebras.h"
#include "math_extra_lie_theory_extras.h" // error: invalid use of incomplete type ‘struct BranchingData’
#include "math_extra_drawing_variables.h"

template <class Coefficient>
std::string Weight<Coefficient>::toString(FormatExpressions* format) const {
  std::stringstream out;
  bool formatWeightAsIndexVectorSpace = format == nullptr ? true : format->flagFormatWeightAsVectorSpaceIndex;
  if (!formatWeightAsIndexVectorSpace) {
    if (this->owner == nullptr) {
      return this->weightFundamentalCoordinates.toStringLetterFormat("\\psi", format);
    }
    Vector<Coefficient> weightEpsCoords, weightSimpleCoords;
    weightSimpleCoords = this->owner->weylGroup.getSimpleCoordinatesFromFundamental(
      this->weightFundamentalCoordinates, Coefficient::zero()
    );
    this->owner->weylGroup.getEpsilonCoordinates(weightSimpleCoords, weightEpsCoords);
    return weightEpsCoords.toStringLetterFormat("\\varepsilon", format);
  }
  bool useOmega = true;
  std::string oldCustomPlus;
  std::string VectorSpaceLetter = "V";
  if (format != nullptr) {
    useOmega = (format->fundamentalWeightLetter == "");
    oldCustomPlus = format->customPlusSign;
    format->customPlusSign = "";
    VectorSpaceLetter = format->FDrepLetter;
  }
  if (useOmega) {
    out << VectorSpaceLetter << "_{" << this->weightFundamentalCoordinates.toStringLetterFormat("\\omega", format) << "}";
  } else {
    out << VectorSpaceLetter << "_{"
    << this->weightFundamentalCoordinates.toStringLetterFormat(format->fundamentalWeightLetter, format) << "}";
  }
  if (format != nullptr) {
    format->customPlusSign = oldCustomPlus;
  }
  return out.str();
}

template <class Coefficient>
void Weight<Coefficient>::accountSingleWeight(
  const Vector<Rational>& currentWeightSimpleCoords,
  const Vector<Rational>& otherHighestWeightSimpleCoords,
  Rational& theMult,
  CharacterSemisimpleLieAlgebraModule<Coefficient>& outputAccum
) const {
  //This is the Brauer-Klimyk formula. Reference:
  //Humphreys J., Introduction to Lie algebras and representation theory
  //page 142, exercise 9.
  MacroRegisterFunctionWithName("Weight_CoefficientType::accountSingleWeight");
  this->checkNonZeroOwner();
  Vector<Rational> dominant = currentWeightSimpleCoords;
  dominant += otherHighestWeightSimpleCoords;
  WeylGroupData& theWeyl = this->owner->weylGroup;
  dominant += theWeyl.rho;
  int sign;
  // a weight has no stabilizer if and only if it is not stabilized by all root reflections.
  for (int i = 0; i < theWeyl.rootsOfBorel.size; i ++) {
    if (theWeyl.rootScalarCartanRoot(dominant, theWeyl.rootsOfBorel[i]).isEqualToZero()) {
      return;
    }
  }
  theWeyl.raiseToDominantWeight(dominant, &sign);
  dominant -= theWeyl.rho;
  if (!theWeyl.isDominantWeight(dominant)) {
    return;
  }
  Weight<Rational> tempMon;
  tempMon.owner = this->owner;
  tempMon.weightFundamentalCoordinates = theWeyl.getFundamentalCoordinatesFromSimple(dominant);
  Coefficient coeffChange;
  coeffChange = theMult;
  coeffChange *= sign;
  outputAccum.addMonomial(tempMon, coeffChange);
}

template <class Coefficient>
std::string Weight<Coefficient>::tensorAndDecompose(
  const Weight<Coefficient>& other,
  CharacterSemisimpleLieAlgebraModule<Coefficient>& output,
  const Coefficient& zero
) const {
  //This is the Brauer-Klimyk formula. Reference:
  //Humphreys J. Introduction to Lie algebras and representation theory
  //page 142, exercise 9.
  MacroRegisterFunctionWithName("Weight_CoefficientType::tensorAndDecompose");
  this->checkNonZeroOwner();
  std::stringstream errorLog;
  std::string tempS;
  output.makeZero();
  WeylGroupData& theWeyl = this->owner->weylGroup;
  Vector<Rational> leftHWFundCoords;
  leftHWFundCoords = this->weightFundamentalCoordinates;
  Vector<Rational> rightHWFundCoords;
  rightHWFundCoords = other.weightFundamentalCoordinates;

  Rational leftTotalDim = theWeyl.weylDimFormulaFundamentalCoords(leftHWFundCoords);
  Rational rightTotalDim = theWeyl.weylDimFormulaFundamentalCoords(rightHWFundCoords);
  if (leftTotalDim > rightTotalDim) {
    MathRoutines::swap(leftTotalDim, rightTotalDim);
    MathRoutines::swap(leftHWFundCoords, rightHWFundCoords);
  }
  HashedList<Vector<Coefficient> > weightsLeftSimpleCoords;
  List<Rational> multsLeft;
  if (!theWeyl.freudenthalFormula(leftHWFundCoords, weightsLeftSimpleCoords, multsLeft, &tempS, 1000000)) {
    errorLog << "Freudenthal formula generated error: " << tempS;
    return errorLog.str();
  }
  HashedList<Vector<Coefficient> > currentOrbit;
  const int OrbitSizeHardLimit = 10000000;
  Vector<Rational> rightHWSimpleCoords = theWeyl.getSimpleCoordinatesFromFundamental(rightHWFundCoords, zero);
  Vectors<Rational> tempRoots;
  tempRoots.setSize(1);
  for (int i = 0; i < weightsLeftSimpleCoords.size; i ++) {
    tempRoots[0] = weightsLeftSimpleCoords[i];
    theWeyl.generateOrbit(tempRoots, false, currentOrbit, false, 0, 0, OrbitSizeHardLimit);
    if (currentOrbit.size >= OrbitSizeHardLimit) {
      errorLog << "Error: orbit layer size exceeded hard-coded limit of " << OrbitSizeHardLimit << ".";
      return errorLog.str();
    }
    for (int j = 0; j < currentOrbit.size; j ++) {
      this->accountSingleWeight(currentOrbit[j], rightHWSimpleCoords, multsLeft[i], output);
    }
  }
  return errorLog.str();
}

template <class Coefficient>
bool CharacterSemisimpleLieAlgebraModule<Coefficient>::freudenthalEvaluateMeFullCharacter(
  CharacterSemisimpleLieAlgebraModule<Coefficient>& outputCharOwnerSetToZero, int upperBoundNumDominantWeights, std::string* outputDetails
) {
  MacroRegisterFunctionWithName("charSSAlgMod_CoefficientType::freudenthalEvaluateMeFullCharacter");
  this->checkNonZeroOwner();
  CharacterSemisimpleLieAlgebraModule<Coefficient> domChar;
  if (!this->freudenthalEvalMeDominantWeightsOnly(domChar, upperBoundNumDominantWeights, outputDetails)) {
    return false;
  }
  outputCharOwnerSetToZero.makeZero();
  Vectors<Rational> theVect;
  HashedList<Vector<Coefficient> > theOrbit;
  theVect.setSize(1);
  Weight<Coefficient> tempMon;
  tempMon.owner = nullptr;
  for (int i = 0; i < domChar.size(); i ++) {
    theVect[0] = this->getOwner()->weylGroup.getSimpleCoordinatesFromFundamental(domChar[i].weightFundamentalCoordinates, Rational::zero());
    if (!(this->getOwner()->weylGroup.generateOrbit(theVect, false, theOrbit, false, - 1, 0, upperBoundNumDominantWeights))) {
      if (outputDetails != nullptr) {
        *outputDetails = "failed to generate orbit (possibly too large?)";
      }
      return false;
    }
    int orbitSize = theOrbit.size;
    for (int j = 0; j < orbitSize; j ++) {
      tempMon.weightFundamentalCoordinates = this->getOwner()->weylGroup.getFundamentalCoordinatesFromSimple(theOrbit[j]);
      outputCharOwnerSetToZero.addMonomial(tempMon, domChar.coefficients[i]);
    }
  }
  return true;
}

template <class Coefficient>
void CharacterSemisimpleLieAlgebraModule<Coefficient>::getDual(CharacterSemisimpleLieAlgebraModule<Coefficient>& output) const {
  if (&output == this) {
    CharacterSemisimpleLieAlgebraModule<Coefficient> thisCopy = *this;
    thisCopy.getDual(output);
    return;
  }
  Weight<Coefficient> tempM;
  output.makeZero();
  for (int i = 0; i < this->size(); i ++) {
    tempM = (*this)[i];
    tempM.weightFundamentalCoordinates.negate();
    output.addMonomial(tempM, this->coefficients[i]);
  }
}

template <class Coefficient>
void CharacterSemisimpleLieAlgebraModule<Coefficient>::makeFromWeight(
  const Vector<Coefficient>& inputWeightSimpleCoords, SemisimpleLieAlgebra* inputOwner
) {
  this->makeZero();
  if (inputWeightSimpleCoords.size != inputOwner->getRank()) {
    global.fatal << "Attempting to create a character from highest weight in simple coords "
    << inputWeightSimpleCoords.toString() << "("
    << inputWeightSimpleCoords.size << " coordinates) while the owner semisimple "
    << " Lie algebra is of rank " << (inputOwner->getRank()) << global.fatal;
  }
  Weight<Coefficient> theMon;
  theMon.owner = inputOwner;
  theMon.weightFundamentalCoordinates =
  inputOwner->weylGroup.getFundamentalCoordinatesFromSimple(inputWeightSimpleCoords);
  this->addMonomial(theMon, 1);
}

template <class Coefficient>
bool CharacterSemisimpleLieAlgebraModule<Coefficient>::freudenthalEvalMeDominantWeightsOnly(
  CharacterSemisimpleLieAlgebraModule<Coefficient>& outputCharOwnerSetToZero, int upperBoundNumDominantWeights, std::string* outputDetails
) {
  MacroRegisterFunctionWithName("charSSAlgMod_CoefficientType::freudenthalEvalMeDominantWeightsOnly");
  if (&outputCharOwnerSetToZero == this) {
    CharacterSemisimpleLieAlgebraModule<Coefficient> thisCopy = *this;
    return thisCopy.freudenthalEvalMeDominantWeightsOnly(outputCharOwnerSetToZero, upperBoundNumDominantWeights, outputDetails);
  }
  this->checkNonZeroOwner();
  outputCharOwnerSetToZero.makeZero();
  Vector<Coefficient> currentWeightFundCoords;
  List<Coefficient> currentMults;
  HashedList<Vector<Coefficient> > currentWeights;
  std::stringstream localErrors, localDetails;
  std::string localDetail;
  Weight<Coefficient> tempMon;
  tempMon.owner = nullptr;
  Coefficient bufferCoeff;
  for (int i = 0; i < this->size(); i ++) {
    currentWeightFundCoords = (*this)[i].weightFundamentalCoordinates;
    if (!this->getOwner()->weylGroup.freudenthalFormula(
      currentWeightFundCoords, currentWeights, currentMults, &localDetail, upperBoundNumDominantWeights
    )) {
      if (outputDetails != nullptr) {
        localErrors << "Encountered error while evaluating freudenthal formula. Error details: " << localDetail
        << "<br> Further computation detail: " << localDetails.str();
        *outputDetails = localErrors.str();
      }
      return false;
    }
    if (localDetail != "") {
      localDetails << "<br>MonomialPolynomial " << i + 1 << " computation details: " << localDetail;
    }
    for (int j = 0; j < currentWeights.size; j ++) {
      tempMon.weightFundamentalCoordinates = this->getOwner()->weylGroup.getFundamentalCoordinatesFromSimple(currentWeights[j]);
      bufferCoeff = this->coefficients[i];
      bufferCoeff *= currentMults[j];
      outputCharOwnerSetToZero.addMonomial(tempMon, bufferCoeff);
    }
  }
  if (outputDetails != nullptr) {
    *outputDetails = localDetails.str();
  }
  return true;
}

template <class Coefficient>
void SemisimpleLieAlgebra::getAdjoint(MatrixTensor<Coefficient>& output, ElementSemisimpleLieAlgebra<Coefficient>& e) {
  Matrix<Coefficient> matForm;
  this->getAdjoint(matForm, e);
  output = matForm;
}

template <class Coefficient>
void SemisimpleLieAlgebra::getAdjoint(Matrix<Coefficient>& output, ElementSemisimpleLieAlgebra<Coefficient>& e) {
  int NumGenerators = this->getNumberOfGenerators();
  output.initialize(NumGenerators, NumGenerators);
  output.makeZero();
  ElementSemisimpleLieAlgebra<Coefficient> theGen, theResult;
  for (int i = 0; i < NumGenerators; i ++) {
    theGen.makeGenerator(i, *this);
    this->lieBracket(e, theGen, theResult);
    for (int j = 0; j < theResult.size(); j ++) {
      output(theResult[j].generatorIndex, i) = theResult.coefficients[j];
    }
  }
}

template <class Coefficient>
void SemisimpleLieAlgebra::getCommonCentralizer(
  const List<ElementSemisimpleLieAlgebra<Coefficient> >& inputElementsToCentralize,
  List<ElementSemisimpleLieAlgebra<Coefficient> >& outputCentralizingElements
) {
  Matrix<Coefficient> tempAd, commonAd;
  for (int i = 0; i < inputElementsToCentralize.size; i ++) {
    this->getAdjoint(tempAd, inputElementsToCentralize[i]);
    commonAd.appendMatrixToTheBottom(tempAd);
  }
  Vectors<Coefficient> outputV;
  commonAd.getZeroEigenSpace(outputV);
  outputCentralizingElements.setSize(outputV.size);
  for (int i = 0; i < outputV.size; i ++) {
    ElementSemisimpleLieAlgebra<Coefficient>& currentElt = outputCentralizingElements[i];
    currentElt.assignVectorNegRootSpacesCartanPosRootSpaces(outputV[i], *this);
  }
}

template <class Coefficient>
void SemisimpleLieAlgebra::lieBracket(
  const ElementSemisimpleLieAlgebra<Coefficient>& g1,
  const ElementSemisimpleLieAlgebra<Coefficient>& g2,
  ElementSemisimpleLieAlgebra<Coefficient>& output
) {
  if (&output == &g1 || &output == &g2) {
    ElementSemisimpleLieAlgebra<Coefficient> outputNew;
    this->lieBracket(g1, g2, outputNew);
    output = outputNew;
    return;
  }
  output.makeZero();
  if (g1.isEqualToZero() || g2.isEqualToZero()) {
    return;
  }
  int maxNumMonsFinal = g1.size() * g2.size();
  output.setExpectedSize(maxNumMonsFinal);
  Coefficient theCoeff;
  ElementSemisimpleLieAlgebra<Coefficient> buffer;
  for (int i = 0; i < g1.size(); i ++) {
    for (int j = 0; j < g2.size(); j ++) {
      buffer = this->lieBrackets.elements[g1[i].generatorIndex][g2[j].generatorIndex];
      theCoeff = g1.coefficients[i];
      theCoeff *= g2.coefficients[j];
      buffer *= theCoeff;
      output += buffer;
    }
  }
}

template <class Coefficient>
void ElementSemisimpleLieAlgebra<Coefficient>::actOnMe(
  const ElementSemisimpleLieAlgebra<Coefficient>& theElt,
  ElementSemisimpleLieAlgebra<Coefficient>& output,
  SemisimpleLieAlgebra& owner
) {
  owner.lieBracket(theElt, *this, output);
}

template <class Coefficient>
Vector<Coefficient> ElementSemisimpleLieAlgebra<Coefficient>::getCartanPart() const {
  Vector<Coefficient> result;
  if (this->isEqualToZero()) {
    result.makeZero(0);
    return result;
  }
  ChevalleyGenerator tempGen;
  SemisimpleLieAlgebra* owner = this->getOwner();
  int theRank = owner->getRank();
  int numPosRoots = owner->getNumberOfPositiveRoots();
  result.makeZero(theRank);
  if (theRank <= 0 || owner == nullptr) {
    global.fatal << "The owner of "
    << "a semisimple Lie algebra element is non-present or corrupted. " << global.fatal;
  }
  for (int i = 0; i < theRank; i ++) {
    tempGen.makeGenerator(*owner, i + numPosRoots);
    int currentIndex = this->monomials.getIndex(tempGen);
    if (currentIndex != - 1) {
      result[i] += this->coefficients[currentIndex];
    }
  }
  return result;
}

template <class Coefficient>
void ElementSemisimpleLieAlgebra<Coefficient>::elementToVectorNegativeRootSpacesFirst(Vector<Coefficient>& output) const {
  if (this->isEqualToZero()) {
    output.makeZero(0);
    return;
  }
  output.makeZero(this->getOwner()->getNumberOfGenerators());
  for (int i = 0; i < this->size(); i ++) {
    output[(*this)[i].generatorIndex] = this->coefficients[i];
  }
}

template <class Coefficient>
void ElementSemisimpleLieAlgebra<Coefficient>::makeGGenerator(
  const Vector<Rational>& root, SemisimpleLieAlgebra& inputOwner
) {
  this->makeGenerator(inputOwner.getGeneratorIndexFromRoot(root), inputOwner);
}

template <class Coefficient>
void ElementSemisimpleLieAlgebra<Coefficient>::assignVectorNegRootSpacesCartanPosRootSpaces(
  const Vector<Coefficient>& input, SemisimpleLieAlgebra& owner
) {
  //Changing RootSystem order invalidates this function!
  this->makeZero();
  ChevalleyGenerator generator;
  for (int i = 0; i < input.size; i ++) {
    if (input[i] == 0) {
      continue;
    }
    generator.makeGenerator(owner, i);
    this->addMonomial(generator, input[i]);
  }
}

template<class Coefficient>
bool ElementSemisimpleLieAlgebra<Coefficient>::isElementCartan() const {
  if (this->size() == 0) {
    return true;
  }
  SemisimpleLieAlgebra* owner = (*this)[0].owner;
  for (int i = 0; i < this->size(); i ++) {
    if (!owner->isGeneratorFromCartan((*this)[i].generatorIndex)) {
      return false;
    }
  }
  return true;
}

template <class Coefficient>
void ElementSemisimpleLieAlgebra<Coefficient>::makeCartanGenerator(
  const Vector<Coefficient>& elementH,
  SemisimpleLieAlgebra& inputOwner
) {
  ChevalleyGenerator generator;
  this->makeZero();
  for (int i = 0; i < elementH.size; i ++) {
    generator.makeGenerator(
      inputOwner,
      inputOwner.getGeneratorIndexFromNonZeroCoefficientIndexInCartan(i)
    );
    this->addMonomial(generator, elementH[i]);
  }
}

template<class Coefficient>
bool CharacterSemisimpleLieAlgebraModule<Coefficient>::drawMe(
  std::string& outputDetails, DrawingVariables& theDrawingVars, int upperBoundWeights, bool useMults
) {
  MacroRegisterFunctionWithName("CharacterSemisimpleLieAlgebraModule::drawMe");
  this->checkNonZeroOwner();
  CharacterSemisimpleLieAlgebraModule<Coefficient> CharCartan;
  bool result = this->freudenthalEvalMeDominantWeightsOnly(CharCartan, upperBoundWeights, &outputDetails);
  std::stringstream out;
  WeylGroupData& theWeyl = this->getOwner()->weylGroup;
  theWeyl.drawRootSystem(theDrawingVars, false, true);
  int totalNumWeights = 0;
  Vectors<Coefficient> dominantWeightsNonHashed;
  HashedList<Vector<Coefficient> > finalWeights;
  Vector<Rational> convertor;
  for (int i = 0; i < CharCartan.size(); i ++) {
    const Weight<Coefficient>& currentMon = CharCartan[i];
    dominantWeightsNonHashed.size = 0;
    dominantWeightsNonHashed.addOnTop(theWeyl.getSimpleCoordinatesFromFundamental(currentMon.weightFundamentalCoordinates));
    bool isTrimmed = !theWeyl.generateOrbit(dominantWeightsNonHashed, false, finalWeights, false, 0,  0, upperBoundWeights);
    totalNumWeights += finalWeights.size;
    if (isTrimmed || totalNumWeights>upperBoundWeights) {
      out << "Did not generate all weights of the module due to RAM limits. ";
      result = false;
      break;
    }
    for (int j = 0; j < finalWeights.size; j ++) {
      convertor = finalWeights[j].getVectorRational();
      theDrawingVars.drawCircleAtVectorBufferRational(convertor, "black", 3);
      if (useMults) {
        theDrawingVars.drawTextAtVectorBufferRational(convertor, CharCartan.coefficients[i].toString(), "black");
      }
    }
  }
  out << "<br>Number of computed weights: " << totalNumWeights << ". ";
  if (result && totalNumWeights<upperBoundWeights) {
    out << "<br>All weights were computed and are drawn. <br>";
  } else {
    out << "<br><b> Not all weights were computed. </b>";
  }
  outputDetails = out.str();
  return result;
}

template <class Coefficient>
void CharacterSemisimpleLieAlgebraModule<Coefficient>::drawMeAssumeCharIsOverCartan(
  WeylGroupData& actualAmbientWeyl, DrawingVariables& theDrawingVars
) const {
  if (actualAmbientWeyl.getDimension() < 2) {
    return;
  }
  Vector<Coefficient> actualWeight;
  Vector<Rational> actualWeightRationalPart;

  actualAmbientWeyl.drawRootSystem(theDrawingVars, true, false, nullptr, false);
  for (int j = 0; j < this->size(); j ++) {
    actualWeight = actualAmbientWeyl.getSimpleCoordinatesFromFundamental((*this)[j].weightFundamentalCoordinates);
    actualWeightRationalPart = actualWeight.getVectorRational(); // <-type conversion here!
    theDrawingVars.drawCircleAtVectorBufferRational(actualWeightRationalPart, "black", 5);
    theDrawingVars.drawTextAtVectorBufferRational(actualWeightRationalPart, this->coefficients[j].toString(), "black");
  }
}

template <class Coefficient>
void CharacterSemisimpleLieAlgebraModule<Coefficient>::makeTrivial(SemisimpleLieAlgebra& inputOwner) {
  this->makeZero();
  Weight<Rational> tempMon;
  tempMon.owner = &inputOwner;
  tempMon.weightFundamentalCoordinates.makeZero(inputOwner.getRank());
  this->addMonomial(tempMon, 1);
}

template <class Coefficient>
std::string CharacterSemisimpleLieAlgebraModule<Coefficient>::operator*=(const CharacterSemisimpleLieAlgebraModule& other) {
  return this->multiplyBy(other);
}

template <class Coefficient>
std::string CharacterSemisimpleLieAlgebraModule<Coefficient>::multiplyBy(const CharacterSemisimpleLieAlgebraModule& other) {
  if (this->getOwner() != other.getOwner() || this->getOwner() == 0) {
    global.fatal << "Attempt to multiply characters of "
    << "different or non-initialized semisimple Lie algebras." << global.fatal;
  }
  this->setExpectedSize(other.size() + this->size());
  CharacterSemisimpleLieAlgebraModule result, summand;
  result.makeZero();
  std::string potentialError;
  Coefficient theCF;
  for (int i = 0; i < this->size(); i ++) {
    for (int j = 0; j < other.size(); j ++) {
      const Weight<Rational>& left = (*this)[i];
      const Weight<Rational>& right = other[j];
      potentialError = left.tensorAndDecompose(right, summand);
      if (potentialError != "") {
        return potentialError;
      }
      theCF = this->coefficients[i];
      theCF *= other.coefficients[j];
      summand *= theCF;
      result += summand;
    }
  }
  this->operator=(result);
  return "";
}

template <class Coefficient>
bool CharacterSemisimpleLieAlgebraModule<Coefficient>::splitCharacterOverReductiveSubalgebra(
  std::string* report, CharacterSemisimpleLieAlgebraModule& output, BranchingData& inputData
) {
  if (this->isEqualToZero()) {
    return false;
  }
  this->checkNonZeroOwner();
  WeylGroupData& theWeyL = this->getOwner()->weylGroup;
  std::stringstream out;
  std::string tempS;
  inputData.initAssumingParSelAndHmmInitted();
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& WeylFDSmallAsSubInLarge = inputData.WeylFDSmallAsSubInLarge;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& WeylFDSmall = inputData.WeylFDSmall;
  SemisimpleLieAlgebra& theSmallAlgebra = inputData.theHmm.domain();
  Vectors<Rational>& embeddingsSimpleEiGoesTo = inputData.theHmm.ImagesCartanDomain;

  CharacterSemisimpleLieAlgebraModule charAmbientFDWeyl, remainingCharProjected, remainingCharDominantLevI;

  Weight<Coefficient> tempMon, localHighest;
  List<Coefficient> tempMults;
  HashedList<Vector<Coefficient> > tempHashedRoots;
  Coefficient bufferCoeff, highestCoeff;
  for (int i = 0; i < this->size(); i ++) {
    const Weight<Coefficient>& currentMon = (*this)[i];
    if (!inputData.WeylFD.freudenthalFormulaIrrepIsWRTLeviPart(
      currentMon.weightFundamentalCoordinates,
      tempHashedRoots,
      tempMults,
      tempS,
      10000
    )) {
      if (report != nullptr) {
        *report = tempS;
      }
      return false;
    }
    for (int j = 0; j < tempHashedRoots.size; j ++) {
      bufferCoeff = this->coefficients[i];
      tempMon.weightFundamentalCoordinates = theWeyL.getFundamentalCoordinatesFromSimple(tempHashedRoots[j]);
      tempMon.owner = this->getOwner();
      bufferCoeff *= tempMults[j];
      charAmbientFDWeyl.addMonomial(tempMon, bufferCoeff);
    }
  }
  Vectors<Coefficient> orbitDom;
  for (int i = 0; i < charAmbientFDWeyl.size(); i ++) {
    orbitDom.setSize(0);
    if (!inputData.WeylFD.generateOrbitReturnFalseIfTruncated(
      theWeyL.getSimpleCoordinatesFromFundamental(
        charAmbientFDWeyl[i].weightFundamentalCoordinates
      ),
      orbitDom,
      true,
      10000
    )) {
      out << "Failed to generate the complement-sub-Weyl-orbit of weight "
      << theWeyL.getSimpleCoordinatesFromFundamental(charAmbientFDWeyl[i].weightFundamentalCoordinates).toString();
      if (report != nullptr) {
        *report = out.str();
      }
      return false;
    }
    tempMon.owner = this->getOwner();
    for (int k = 0; k < orbitDom.size; k ++) {
      if (WeylFDSmallAsSubInLarge.isDominantWeight(orbitDom[k])) {
        tempMon.weightFundamentalCoordinates = theWeyL.getFundamentalCoordinatesFromSimple(orbitDom[k]);
        remainingCharDominantLevI.addMonomial(tempMon, charAmbientFDWeyl.coefficients[i]);
      }
    }
  }
  FormatExpressions theFormat;
  theFormat.flagUseLatex = true;
  theFormat.customPlusSign = "\\oplus ";
  theFormat.fundamentalWeightLetter = "\\omega";
  out << "<br>Character w.r.t Levi part of the parabolic of the larger algebra: "
  << HtmlRoutines::getMathNoDisplay(remainingCharDominantLevI.toString(&theFormat));
  remainingCharProjected.makeZero();
  Vector<Coefficient> fundCoordsSmaller, inSimpleCoords;
  fundCoordsSmaller.setSize(WeylFDSmall.ambientWeyl->getDimension());
  for (int i = 0; i < remainingCharDominantLevI.size(); i ++) {
    inSimpleCoords = theWeyL.getSimpleCoordinatesFromFundamental(remainingCharDominantLevI[i].weightFundamentalCoordinates);
    for (int j = 0; j < WeylFDSmall.ambientWeyl->getDimension(); j ++) {
      fundCoordsSmaller[j] = theWeyL.rootScalarCartanRoot(inSimpleCoords, embeddingsSimpleEiGoesTo[j]);
      fundCoordsSmaller[j] /= WeylFDSmall.ambientWeyl->cartanSymmetric(j, j) / 2;
    }
    tempMon.owner = &theSmallAlgebra;
    tempMon.weightFundamentalCoordinates = fundCoordsSmaller;
    remainingCharProjected.addMonomial(tempMon, remainingCharDominantLevI.coefficients[i]);
  }
  Vector<Coefficient> simpleGeneratorBaseField;
  output.makeZero();
  while (!remainingCharProjected.isEqualToZero()) {
    localHighest = *remainingCharProjected.monomials.lastObject();
    for (bool Found = true; Found;) {
      Found = false;
      for (int i = 0; i < WeylFDSmall.RootsOfBorel.size; i ++) {
        tempMon = localHighest;
        simpleGeneratorBaseField = WeylFDSmall.RootsOfBorel[i]; // <- implicit type conversion here!
        tempMon.weightFundamentalCoordinates += WeylFDSmall.ambientWeyl->getFundamentalCoordinatesFromSimple(
          simpleGeneratorBaseField
        );
        if (remainingCharProjected.monomials.contains(tempMon)) {
          localHighest = tempMon;
          Found = true;
        }
      }
    }
    highestCoeff = remainingCharProjected.coefficients[remainingCharProjected.monomials.getIndex(localHighest)];
    output.addMonomial(localHighest, highestCoeff);
    if (!WeylFDSmall.freudenthalFormulaIrrepIsWRTLeviPart(
      localHighest.weightFundamentalCoordinates, tempHashedRoots, tempMults, tempS, 10000
    )) {
      if (report != nullptr) {
        *report = tempS;
      }
      return false;
    }
    for (int i = 0; i < tempHashedRoots.size; i ++) {
      tempMon.owner = &theSmallAlgebra;
      tempMon.weightFundamentalCoordinates =
      WeylFDSmall.ambientWeyl->getFundamentalCoordinatesFromSimple(tempHashedRoots[i]);
      bufferCoeff = tempMults[i];
      bufferCoeff *= highestCoeff;
      remainingCharProjected.subtractMonomial(tempMon, bufferCoeff);
    }
  }
  theFormat.fundamentalWeightLetter = "\\psi";
  out << "<br>Character w.r.t the Levi part of the parabolic of the small algebra: "
  << HtmlRoutines::getMathNoDisplay(output.toString(&theFormat));
  if (report != nullptr) {
    DrawingVariables theDV1;
    std::string tempS;
    output.drawMeNoMultiplicities(tempS, theDV1, 10000);
    Vector<Rational> tempRoot, tempRoot2;
    WeylFDSmall.ambientWeyl->group.computeAllElements(false, 20);
    out << "<hr>";
    for (int i = 0; i < output.size(); i ++) {
      tempRoot = WeylFDSmall.ambientWeyl->getSimpleCoordinatesFromFundamental(
        output[i].weightFundamentalCoordinates
      ).getVectorRational();
      std::stringstream tempStream;
      tempStream << output.coefficients[i].toString();
      theDV1.drawTextAtVectorBufferRational(tempRoot, tempStream.str(), "black");
      for (int j = 1; j < WeylFDSmall.ambientWeyl->group.elements.size; j ++) {
        tempRoot2 = tempRoot;
        WeylFDSmall.ambientWeyl->actOnRhoModified(j, tempRoot2);
        theDV1.drawCircleAtVectorBufferRational(tempRoot2, "#a00000", 5);
      }
    }
    out << "<hr>" << theDV1.getHTMLDiv(WeylFDSmall.ambientWeyl->getDimension(), false);
    *report = out.str();
  }
  return true;
}

template <class Coefficient>
Coefficient SemisimpleLieAlgebra::getKillingForm(
  const ElementSemisimpleLieAlgebra<Coefficient>& left,
  const ElementSemisimpleLieAlgebra<Coefficient>& right
) {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::getKillingForm");
  Coefficient result = 0;
  ElementSemisimpleLieAlgebra<Coefficient> adadAppliedToMon, tempElt;
  ChevalleyGenerator baseGen;
  for (int i = 0; i < this->getNumberOfGenerators(); i ++) {
    baseGen.makeGenerator(*this, i);
    adadAppliedToMon.makeZero();
    adadAppliedToMon.addMonomial(baseGen, 1);
    this->lieBracket(right, adadAppliedToMon, tempElt);
    this->lieBracket(left, tempElt, adadAppliedToMon);
    result += adadAppliedToMon.getCoefficientOf(baseGen);
  }
  return result;
}
#endif
