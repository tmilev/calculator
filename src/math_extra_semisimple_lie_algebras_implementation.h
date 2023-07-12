
#ifndef header_math_extra_semisimple_lie_algebras_implementation_ALREADY_INCLUDED
#define header_math_extra_semisimple_lie_algebras_implementation_ALREADY_INCLUDED

#include "math_extra_semisimple_lie_algebras.h"
#include "math_extra_lie_theory_extras.h" // error: invalid use of incomplete type ‘struct BranchingData’
#include "math_extra_drawing_variables.h"

template <class Coefficient>
std::string Weight<Coefficient>::toString(FormatExpressions* format) const {
  std::stringstream out;
  bool formatWeightAsIndexVectorSpace = format ==
  nullptr ? true : format->flagFormatWeightAsVectorSpaceIndex;
  if (!formatWeightAsIndexVectorSpace) {
    if (this->owner == nullptr) {
      return
      this->weightFundamentalCoordinates.toStringLetterFormat("\\psi", format);
    }
    Vector<Coefficient> weightEpsilonCoordinates;
    Vector<Coefficient> weightSimpleCoordinates;
    weightSimpleCoordinates =
    this->owner->weylGroup.getSimpleCoordinatesFromFundamental(
      this->weightFundamentalCoordinates, Coefficient::zero()
    );
    this->owner->weylGroup.getEpsilonCoordinates(
      weightSimpleCoordinates, weightEpsilonCoordinates
    );
    return
    weightEpsilonCoordinates.toStringLetterFormat("\\varepsilon", format);
  }
  bool useOmega = true;
  std::string oldCustomPlus;
  std::string VectorSpaceLetter = "V";
  if (format != nullptr) {
    useOmega = (format->fundamentalWeightLetter == "");
    oldCustomPlus = format->customPlusSign;
    format->customPlusSign = "";
    VectorSpaceLetter = format->finiteDimensionalRepresentationLetter;
  }
  if (useOmega) {
    out
    << VectorSpaceLetter
    << "_{"
    << this->weightFundamentalCoordinates.toStringLetterFormat(
      "\\omega", format
    )
    << "}";
  } else {
    out
    << VectorSpaceLetter
    << "_{"
    << this->weightFundamentalCoordinates.toStringLetterFormat(
      format->fundamentalWeightLetter, format
    )
    << "}";
  }
  if (format != nullptr) {
    format->customPlusSign = oldCustomPlus;
  }
  return out.str();
}

template <class Coefficient>
void Weight<Coefficient>::accountSingleWeight(
  const Vector<Rational>& currentWeightSimpleCoordinates,
  const Vector<Rational>& otherhighestWeightSimpleCoordinates,
  Rational& multiplicity,
  CharacterSemisimpleLieAlgebraModule<Coefficient>& outputAccum
) const {
  // This is the Brauer-Klimyk formula. Reference:
  // Humphreys J., Introduction to Lie algebras and representation theory
  // page 142, exercise 9.
  STACK_TRACE("Weight_CoefficientType::accountSingleWeight");
  this->checkNonZeroOwner();
  Vector<Rational> dominant = currentWeightSimpleCoordinates;
  dominant += otherhighestWeightSimpleCoordinates;
  WeylGroupData& weylGroup = this->owner->weylGroup;
  dominant += weylGroup.rho;
  int sign;
  // a weight has no stabilizer if and only if it is not stabilized by all root
  // reflections.
  for (int i = 0; i < weylGroup.rootsOfBorel.size; i ++) {
    if (
      weylGroup.rootScalarCartanRoot(dominant, weylGroup.rootsOfBorel[i]).
      isEqualToZero()
    ) {
      return;
    }
  }
  weylGroup.raiseToDominantWeight(dominant, &sign);
  dominant -= weylGroup.rho;
  if (!weylGroup.isDominantWeight(dominant)) {
    return;
  }
  Weight<Rational> monomial;
  monomial.owner = this->owner;
  monomial.weightFundamentalCoordinates =
  weylGroup.getFundamentalCoordinatesFromSimple(dominant);
  Coefficient coeffChange;
  coeffChange = multiplicity;
  coeffChange *= sign;
  outputAccum.addMonomial(monomial, coeffChange);
}

template <class Coefficient>
std::string Weight<Coefficient>::tensorAndDecompose(
  const Weight<Coefficient>& other,
  CharacterSemisimpleLieAlgebraModule<Coefficient>& output,
  const Coefficient& zero
) const {
  // This is the Brauer-Klimyk formula. Reference:
  // Humphreys J. Introduction to Lie algebras and representation theory
  // page 142, exercise 9.
  STACK_TRACE("Weight_CoefficientType::tensorAndDecompose");
  this->checkNonZeroOwner();
  std::stringstream errorLog;
  std::string tempS;
  output.makeZero();
  WeylGroupData& weylGrouop = this->owner->weylGroup;
  Vector<Rational> lefthighestWeightFundamentalCoordinates;
  lefthighestWeightFundamentalCoordinates = this->weightFundamentalCoordinates;
  Vector<Rational> righthighestWeightFundamentalCoordinates;
  righthighestWeightFundamentalCoordinates =
  other.weightFundamentalCoordinates;
  Rational leftTotalDim =
  weylGrouop.weylDimFormulaFundamentalCoordinates(
    lefthighestWeightFundamentalCoordinates
  );
  Rational rightTotalDim =
  weylGrouop.weylDimFormulaFundamentalCoordinates(
    righthighestWeightFundamentalCoordinates
  );
  if (leftTotalDim > rightTotalDim) {
    MathRoutines::swap(leftTotalDim, rightTotalDim);
    MathRoutines::swap(
      lefthighestWeightFundamentalCoordinates,
      righthighestWeightFundamentalCoordinates
    );
  }
  HashedList<Vector<Coefficient> > weightsLeftSimpleCoordinates;
  List<Rational> multiplicitiesLeft;
  if (
    !weylGrouop.freudenthalFormula(
      lefthighestWeightFundamentalCoordinates,
      weightsLeftSimpleCoordinates,
      multiplicitiesLeft,
      &tempS,
      1000000
    )
  ) {
    errorLog << "Freudenthal formula generated error: " << tempS;
    return errorLog.str();
  }
  HashedList<Vector<Coefficient> > currentOrbit;
  const int orbitSizeHardLimit = 10000000;
  Vector<Rational> righthighestWeightSimpleCoordinates =
  weylGrouop.getSimpleCoordinatesFromFundamental(
    righthighestWeightFundamentalCoordinates, zero
  );
  Vectors<Rational> roots;
  roots.setSize(1);
  for (int i = 0; i < weightsLeftSimpleCoordinates.size; i ++) {
    roots[0] = weightsLeftSimpleCoordinates[i];
    weylGrouop.generateOrbit(
      roots, false, currentOrbit, false, 0, 0, orbitSizeHardLimit
    );
    if (currentOrbit.size >= orbitSizeHardLimit) {
      errorLog
      << "Error: orbit layer size exceeded hard-coded limit of "
      << orbitSizeHardLimit
      << ".";
      return errorLog.str();
    }
    for (int j = 0; j < currentOrbit.size; j ++) {
      this->accountSingleWeight(
        currentOrbit[j],
        righthighestWeightSimpleCoordinates,
        multiplicitiesLeft[i],
        output
      );
    }
  }
  return errorLog.str();
}

template <class Coefficient>
bool CharacterSemisimpleLieAlgebraModule<Coefficient>::
freudenthalEvaluateMeFullCharacter(
  CharacterSemisimpleLieAlgebraModule<Coefficient>& outputCharOwnerSetToZero,
  int upperBoundTotalDominantWeights,
  std::string* outputDetails
) {
  STACK_TRACE(
    "charSSAlgMod_CoefficientType::freudenthalEvaluateMeFullCharacter"
  );
  this->checkNonZeroOwner();
  CharacterSemisimpleLieAlgebraModule<Coefficient> domChar;
  if (
    !this->freudenthalEvalMeDominantWeightsOnly(
      domChar, upperBoundTotalDominantWeights, outputDetails
    )
  ) {
    return false;
  }
  outputCharOwnerSetToZero.makeZero();
  Vectors<Rational> vector;
  HashedList<Vector<Coefficient> > orbit;
  vector.setSize(1);
  Weight<Coefficient> monomial;
  monomial.owner = nullptr;
  for (int i = 0; i < domChar.size(); i ++) {
    vector[0] =
    this->getOwner()->weylGroup.getSimpleCoordinatesFromFundamental(
      domChar[i].weightFundamentalCoordinates, Rational::zero()
    );
    if (
      !(
        this->getOwner()->weylGroup.generateOrbit(
          vector,
          false,
          orbit,
          false,
          - 1,
          0,
          upperBoundTotalDominantWeights
        )
      )
    ) {
      if (outputDetails != nullptr) {
        *outputDetails = "failed to generate orbit (possibly too large?)";
      }
      return false;
    }
    int orbitSize = orbit.size;
    for (int j = 0; j < orbitSize; j ++) {
      monomial.weightFundamentalCoordinates =
      this->getOwner()->weylGroup.getFundamentalCoordinatesFromSimple(
        orbit[j]
      );
      outputCharOwnerSetToZero.addMonomial(
        monomial, domChar.coefficients[i]
      );
    }
  }
  return true;
}

template <class Coefficient>
void CharacterSemisimpleLieAlgebraModule<Coefficient>::getDual(
  CharacterSemisimpleLieAlgebraModule<Coefficient>& output
) const {
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
  const Vector<Coefficient>& inputweightSimpleCoordinates,
  SemisimpleLieAlgebra* inputOwner
) {
  this->makeZero();
  if (inputweightSimpleCoordinates.size != inputOwner->getRank()) {
    global.fatal
    << "Attempting to create a character from highest weight in simple coords "
    << inputweightSimpleCoordinates.toString()
    << "("
    << inputweightSimpleCoordinates.size
    << " coordinates) while the owner semisimple "
    << " Lie algebra is of rank "
    << (inputOwner->getRank())
    << global.fatal;
  }
  Weight<Coefficient> monomial;
  monomial.owner = inputOwner;
  monomial.weightFundamentalCoordinates =
  inputOwner->weylGroup.getFundamentalCoordinatesFromSimple(
    inputweightSimpleCoordinates
  );
  this->addMonomial(monomial, 1);
}

template <class Coefficient>
bool CharacterSemisimpleLieAlgebraModule<Coefficient>::
freudenthalEvalMeDominantWeightsOnly(
  CharacterSemisimpleLieAlgebraModule<Coefficient>& outputCharOwnerSetToZero,
  int upperBoundTotalDominantWeights,
  std::string* outputDetails
) {
  STACK_TRACE(
    "charSSAlgMod_CoefficientType::freudenthalEvalMeDominantWeightsOnly"
  );
  if (&outputCharOwnerSetToZero == this) {
    CharacterSemisimpleLieAlgebraModule<Coefficient> thisCopy = *this;
    return
    thisCopy.freudenthalEvalMeDominantWeightsOnly(
      outputCharOwnerSetToZero, upperBoundTotalDominantWeights, outputDetails
    );
  }
  this->checkNonZeroOwner();
  outputCharOwnerSetToZero.makeZero();
  Vector<Coefficient> currentweightFundamentalCoordinates;
  List<Coefficient> currentMults;
  HashedList<Vector<Coefficient> > currentWeights;
  std::stringstream localErrors, localDetails;
  std::string localDetail;
  Weight<Coefficient> monomial;
  monomial.owner = nullptr;
  Coefficient bufferCoefficient;
  for (int i = 0; i < this->size(); i ++) {
    currentweightFundamentalCoordinates = (*this)[i].
    weightFundamentalCoordinates;
    if (
      !this->getOwner()->weylGroup.freudenthalFormula(
        currentweightFundamentalCoordinates,
        currentWeights,
        currentMults,
        &localDetail,
        upperBoundTotalDominantWeights
      )
    ) {
      if (outputDetails != nullptr) {
        localErrors
        << "Encountered error while evaluating "
        << "Freudenthal formula. Error details: "
        << localDetail
        << "<br> Further computation detail: "
        << localDetails.str();
        *outputDetails = localErrors.str();
      }
      return false;
    }
    if (localDetail != "") {
      localDetails
      << "<br>MonomialPolynomial "
      << i + 1
      << " computation details: "
      << localDetail;
    }
    for (int j = 0; j < currentWeights.size; j ++) {
      monomial.weightFundamentalCoordinates =
      this->getOwner()->weylGroup.getFundamentalCoordinatesFromSimple(
        currentWeights[j]
      );
      bufferCoefficient = this->coefficients[i];
      bufferCoefficient *= currentMults[j];
      outputCharOwnerSetToZero.addMonomial(monomial, bufferCoefficient);
    }
  }
  if (outputDetails != nullptr) {
    *outputDetails = localDetails.str();
  }
  return true;
}

template <class Coefficient>
void SemisimpleLieAlgebra::getAdjoint(
  MatrixTensor<Coefficient>& output,
  ElementSemisimpleLieAlgebra<Coefficient>& e
) {
  Matrix<Coefficient> matForm;
  this->getAdjoint(matForm, e);
  output = matForm;
}

template <class Coefficient>
void SemisimpleLieAlgebra::getAdjoint(
  Matrix<Coefficient>& output,
  ElementSemisimpleLieAlgebra<Coefficient>& e
) {
  int numberOfGenerators = this->getNumberOfGenerators();
  output.initialize(numberOfGenerators, numberOfGenerators);
  output.makeZero();
  ElementSemisimpleLieAlgebra<Coefficient> generator, result;
  for (int i = 0; i < numberOfGenerators; i ++) {
    generator.makeGenerator(i, *this);
    this->lieBracket(e, generator, result);
    for (int j = 0; j < result.size(); j ++) {
      output(result[j].generatorIndex, i) = result.coefficients[j];
    }
  }
}

template <class Coefficient>
void SemisimpleLieAlgebra::getCommonCentralizer(
  const List<ElementSemisimpleLieAlgebra<Coefficient> >&
  inputElementsToCentralize,
  List<ElementSemisimpleLieAlgebra<Coefficient> >& outputCentralizingElements
) {
  STACK_TRACE("SemisimpleLieAlgebra::getCommonCentralizer");
  Matrix<Coefficient> currentAdjointOperator, commonAdjointOperator;
  for (int i = 0; i < inputElementsToCentralize.size; i ++) {
    this->getAdjoint(currentAdjointOperator, inputElementsToCentralize[i]);
    commonAdjointOperator.appendMatrixToTheBottom(currentAdjointOperator);
  }
  Vectors<Coefficient> outputV;
  commonAdjointOperator.getZeroEigenSpace(outputV);
  outputCentralizingElements.setSize(outputV.size);
  for (int i = 0; i < outputV.size; i ++) {
    ElementSemisimpleLieAlgebra<Coefficient>& currentElement =
    outputCentralizingElements[i];
    currentElement.assignVectorNegativeRootSpacesCartanPosistiveRootSpaces(
      outputV[i], *this
    );
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
  int maximumMonomialsFinal = g1.size() * g2.size();
  output.setExpectedSize(maximumMonomialsFinal);
  Coefficient coefficient;
  ElementSemisimpleLieAlgebra<Coefficient> buffer;
  for (int i = 0; i < g1.size(); i ++) {
    for (int j = 0; j < g2.size(); j ++) {
      buffer =
      this->lieBrackets.elements[g1[i].generatorIndex][
        g2[j].generatorIndex
      ];
      coefficient = g1.coefficients[i];
      coefficient *= g2.coefficients[j];
      buffer *= coefficient;
      output += buffer;
    }
  }
}

template <class Coefficient>
void ElementSemisimpleLieAlgebra<Coefficient>::actOnMe(
  const ElementSemisimpleLieAlgebra<Coefficient>& element,
  ElementSemisimpleLieAlgebra<Coefficient>& output,
  SemisimpleLieAlgebra& owner
) {
  owner.lieBracket(element, *this, output);
}

template <class Coefficient>
Vector<Coefficient> ElementSemisimpleLieAlgebra<Coefficient>::getCartanPart()
const {
  Vector<Coefficient> result;
  if (this->isEqualToZero()) {
    result.makeZero(0);
    return result;
  }
  ChevalleyGenerator tempGen;
  SemisimpleLieAlgebra* owner = this->getOwner();
  int rank = owner->getRank();
  int numberOfPositiveRoots = owner->getNumberOfPositiveRoots();
  result.makeZero(rank);
  if (rank <= 0 || owner == nullptr) {
    global.fatal
    << "The owner of "
    << "a semisimple Lie algebra element is non-present or corrupted. "
    << global.fatal;
  }
  for (int i = 0; i < rank; i ++) {
    tempGen.makeGenerator(*owner, i + numberOfPositiveRoots);
    int currentIndex = this->monomials.getIndex(tempGen);
    if (currentIndex != - 1) {
      result[i] += this->coefficients[currentIndex];
    }
  }
  return result;
}

template <class Coefficient>
void ElementSemisimpleLieAlgebra<Coefficient>::toVectorNegativeRootSpacesFirst(
  Vector<Coefficient>& output, SemisimpleLieAlgebra& owner
) const {
  if (this->isEqualToZero()) {
    output.makeZero(owner.getNumberOfGenerators());
    return;
  }
  return this->toVectorNegativeRootSpacesFirst(output);
}

template <class Coefficient>
void ElementSemisimpleLieAlgebra<Coefficient>::toVectorNegativeRootSpacesFirst(
  Vector<Coefficient>& output
) const {
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
  int generatorIndex = inputOwner.getGeneratorIndexFromRoot(root);
  if (generatorIndex < 0) {
    global.fatal
    << "Function makeGGenerator does not allow non-root input: "
    << root.toString()
    << global.fatal;
  }
  this->makeGenerator(generatorIndex, inputOwner);
}

template <class Coefficient>
void ElementSemisimpleLieAlgebra<Coefficient>::
assignVectorNegativeRootSpacesCartanPosistiveRootSpaces(
  const Vector<Coefficient>& input, SemisimpleLieAlgebra& owner
) {
  // Changing RootSystem order invalidates this function!
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

template <class Coefficient>
bool ElementSemisimpleLieAlgebra<Coefficient>::isElementCartan() const {
  if (this->size() == 0) {
    return true;
  }
  SemisimpleLieAlgebra * owner = (*this)[0].owner;
  for (int i = 0; i < this->size(); i ++) {
    if (!owner->isGeneratorFromCartan((*this)[i].generatorIndex)) {
      return false;
    }
  }
  return true;
}

template <class Coefficient>
void ElementSemisimpleLieAlgebra<Coefficient>::makeCartanGenerator(
  const Vector<Coefficient>& elementH, SemisimpleLieAlgebra& inputOwner
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

template <class Coefficient>
bool CharacterSemisimpleLieAlgebraModule<Coefficient>::drawMe(
  std::string& outputDetails,
  DrawingVariables& drawingVariables,
  int upperBoundWeights,
  bool useMultiplicities
) {
  STACK_TRACE("CharacterSemisimpleLieAlgebraModule::drawMe");
  this->checkNonZeroOwner();
  CharacterSemisimpleLieAlgebraModule<Coefficient> characteristicCartan;
  bool result =
  this->freudenthalEvalMeDominantWeightsOnly(
    characteristicCartan, upperBoundWeights, &outputDetails
  );
  std::stringstream out;
  WeylGroupData& weylGroup = this->getOwner()->weylGroup;
  weylGroup.drawRootSystem(drawingVariables, false, false);
  int totalWeights = 0;
  Vectors<Coefficient> dominantWeightsNonHashed;
  HashedList<Vector<Coefficient> > finalWeights;
  Vector<Rational> convertor;
  for (int i = 0; i < characteristicCartan.size(); i ++) {
    const Weight<Coefficient>& monomial = characteristicCartan[i];
    dominantWeightsNonHashed.size = 0;
    dominantWeightsNonHashed.addOnTop(
      weylGroup.getSimpleCoordinatesFromFundamental(
        monomial.weightFundamentalCoordinates
      )
    );
    bool isTrimmed =
    !weylGroup.generateOrbit(
      dominantWeightsNonHashed,
      false,
      finalWeights,
      false,
      0,
      0,
      upperBoundWeights
    );
    totalWeights += finalWeights.size;
    if (isTrimmed || totalWeights > upperBoundWeights) {
      out << "Did not generate all weights of the module due to RAM limits. ";
      result = false;
      break;
    }
    for (int j = 0; j < finalWeights.size; j ++) {
      convertor = finalWeights[j].getVectorRational();
      drawingVariables.drawCircleAtVectorBufferRational(
        convertor, "black", 3
      );
      if (useMultiplicities) {
        drawingVariables.drawTextAtVector(
          convertor,
          characteristicCartan.coefficients[i].toString(),
          "black",
          drawingVariables.fontSizeNormal
        );
      }
    }
  }
  out << "<br>Number of computed weights: " << totalWeights << ". ";
  if (result && totalWeights < upperBoundWeights) {
    out << "<br>All weights were computed and are drawn. <br>";
  } else {
    out << "<br><b> Not all weights were computed. </b>";
  }
  outputDetails = out.str();
  return result;
}

template <class Coefficient>
void CharacterSemisimpleLieAlgebraModule<Coefficient>::
drawMeAssumeCharIsOverCartan(
  WeylGroupData& actualAmbientWeyl, DrawingVariables& drawingVariables
) const {
  if (actualAmbientWeyl.getDimension() < 2) {
    return;
  }
  Vector<Coefficient> actualWeight;
  Vector<Rational> actualWeightRationalPart;
  actualAmbientWeyl.drawRootSystem(
    drawingVariables, true, false, nullptr, false
  );
  for (int j = 0; j < this->size(); j ++) {
    actualWeight =
    actualAmbientWeyl.getSimpleCoordinatesFromFundamental((*this)[j].
      weightFundamentalCoordinates
    );
    actualWeightRationalPart = actualWeight.getVectorRational();
    // <-type conversion here!
    drawingVariables.drawCircleAtVectorBufferRational(
      actualWeightRationalPart, "black", 5
    );
    drawingVariables.drawTextAtVector(
      actualWeightRationalPart,
      this->coefficients[j].toString(),
      "black"
    );
  }
}

template <class Coefficient>
void CharacterSemisimpleLieAlgebraModule<Coefficient>::makeTrivial(
  SemisimpleLieAlgebra& inputOwner
) {
  this->makeZero();
  Weight<Rational> monomial;
  monomial.owner = &inputOwner;
  monomial.weightFundamentalCoordinates.makeZero(inputOwner.getRank());
  this->addMonomial(monomial, 1);
}

template <class Coefficient>
std::string CharacterSemisimpleLieAlgebraModule<Coefficient>::operator*=(
  const CharacterSemisimpleLieAlgebraModule& other
) {
  return this->multiplyBy(other);
}

template <class Coefficient>
std::string CharacterSemisimpleLieAlgebraModule<Coefficient>::multiplyBy(
  const CharacterSemisimpleLieAlgebraModule& other
) {
  if (this->getOwner() != other.getOwner() || this->getOwner() == 0) {
    global.fatal
    << "Attempt to multiply characters of "
    << "different or non-initialized semisimple Lie algebras."
    << global.fatal;
  }
  this->setExpectedSize(other.size() + this->size());
  CharacterSemisimpleLieAlgebraModule result, summand;
  result.makeZero();
  std::string potentialError;
  Coefficient coefficient;
  for (int i = 0; i < this->size(); i ++) {
    for (int j = 0; j < other.size(); j ++) {
      const Weight<Rational>& left = (*this)[i];
      const Weight<Rational>& right = other[j];
      potentialError = left.tensorAndDecompose(right, summand);
      if (potentialError != "") {
        return potentialError;
      }
      coefficient = this->coefficients[i];
      coefficient *= other.coefficients[j];
      summand *= coefficient;
      result += summand;
    }
  }
  this->operator=(result);
  return "";
}

template <class Coefficient>
bool CharacterSemisimpleLieAlgebraModule<Coefficient>::
splitCharacterOverReductiveSubalgebra(
  std::string* report,
  CharacterSemisimpleLieAlgebraModule& output,
  BranchingData& inputData
) {
  if (this->isEqualToZero()) {
    return false;
  }
  this->checkNonZeroOwner();
  WeylGroupData& weylGroup = this->getOwner()->weylGroup;
  std::stringstream out;
  std::string tempS;
  inputData.initializeAfterParabolicSelectionAndHomomorphism();
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms&
  weylGroupFiniteDimensionalSmallAsSubgroupInLarge =
  inputData.weylGroupFiniteDimensionalSmallAsSubgroupInLarge;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms&
  weylGroupFiniteDimensionalSmall =
  inputData.weylGroupFiniteDimensionalSmall;
  SemisimpleLieAlgebra& smallAlgebra = inputData.homomorphism.domainAlgebra();
  Vectors<Rational>& embeddingsSimpleEiGoesTo =
  inputData.homomorphism.imagesCartanDomain;
  CharacterSemisimpleLieAlgebraModule charAmbientFDWeyl;
  CharacterSemisimpleLieAlgebraModule remainingCharProjected;
  CharacterSemisimpleLieAlgebraModule remainingCharDominantLevI;
  Weight<Coefficient> workingMonomial;
  Weight<Coefficient> localHighest;
  List<Coefficient> tempMults;
  HashedList<Vector<Coefficient> > tempHashedRoots;
  Coefficient bufferCoefficient;
  Coefficient highestCoeff;
  for (int i = 0; i < this->size(); i ++) {
    const Weight<Coefficient>& monomial = (*this)[i];
    if (
      !inputData.weylGroupFiniteDimensional.
      freudenthalFormulaIrrepIsWRTLeviPart(
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
      workingMonomial.weightFundamentalCoordinates =
      weylGroup.getFundamentalCoordinatesFromSimple(tempHashedRoots[j]);
      workingMonomial.owner = this->getOwner();
      bufferCoefficient *= tempMults[j];
      charAmbientFDWeyl.addMonomial(workingMonomial, bufferCoefficient);
    }
  }
  Vectors<Coefficient> orbitDom;
  for (int i = 0; i < charAmbientFDWeyl.size(); i ++) {
    orbitDom.setSize(0);
    if (
      !inputData.weylGroupFiniteDimensional.generateOrbitReturnFalseIfTruncated
      (
        weylGroup.getSimpleCoordinatesFromFundamental(
          charAmbientFDWeyl[i].weightFundamentalCoordinates
        ),
        orbitDom,
        true,
        10000
      )
    ) {
      out
      << "Failed to generate the complement-sub-Weyl-orbit of weight "
      << weylGroup.getSimpleCoordinatesFromFundamental(
        charAmbientFDWeyl[i].weightFundamentalCoordinates
      ).toString();
      if (report != nullptr) {
        *report = out.str();
      }
      return false;
    }
    workingMonomial.owner = this->getOwner();
    for (int k = 0; k < orbitDom.size; k ++) {
      if (
        weylGroupFiniteDimensionalSmallAsSubgroupInLarge.isDominantWeight(
          orbitDom[k]
        )
      ) {
        workingMonomial.weightFundamentalCoordinates =
        weylGroup.getFundamentalCoordinatesFromSimple(orbitDom[k]);
        remainingCharDominantLevI.addMonomial(
          workingMonomial, charAmbientFDWeyl.coefficients[i]
        );
      }
    }
  }
  FormatExpressions format;
  format.flagUseLatex = true;
  format.customPlusSign = "\\oplus ";
  format.fundamentalWeightLetter = "\\omega";
  out
  << "<br>Character w.r.t Levi part of the parabolic of the larger algebra: "
  << HtmlRoutines::getMathNoDisplay(
    remainingCharDominantLevI.toString(&format)
  );
  remainingCharProjected.makeZero();
  Vector<Coefficient> fundamentalCoordinatesSmaller;
  Vector<Coefficient> inSimpleCoordinates;
  fundamentalCoordinatesSmaller.setSize(
    weylGroupFiniteDimensionalSmall.ambientWeyl->getDimension()
  );
  for (int i = 0; i < remainingCharDominantLevI.size(); i ++) {
    inSimpleCoordinates =
    weylGroup.getSimpleCoordinatesFromFundamental(
      remainingCharDominantLevI[i].weightFundamentalCoordinates
    );
    for (
      int j = 0; j < weylGroupFiniteDimensionalSmall.ambientWeyl->getDimension(
      ); j ++
    ) {
      fundamentalCoordinatesSmaller[j] =
      weylGroup.rootScalarCartanRoot(
        inSimpleCoordinates, embeddingsSimpleEiGoesTo[j]
      );
      fundamentalCoordinatesSmaller[j] /=
      weylGroupFiniteDimensionalSmall.ambientWeyl->cartanSymmetric(j, j) /
      2;
    }
    workingMonomial.owner = &smallAlgebra;
    workingMonomial.weightFundamentalCoordinates =
    fundamentalCoordinatesSmaller;
    remainingCharProjected.addMonomial(
      workingMonomial, remainingCharDominantLevI.coefficients[i]
    );
  }
  Vector<Coefficient> simpleGeneratorBaseField;
  output.makeZero();
  while (!remainingCharProjected.isEqualToZero()) {
    localHighest = *remainingCharProjected.monomials.lastObject();
    for (bool Found = true; Found;) {
      Found = false;
      for (
        int i = 0; i < weylGroupFiniteDimensionalSmall.rootsOfBorel.size; i ++
      ) {
        workingMonomial = localHighest;
        simpleGeneratorBaseField =
        weylGroupFiniteDimensionalSmall.rootsOfBorel[i];
        // <- implicit type conversion here!
        workingMonomial.weightFundamentalCoordinates +=
        weylGroupFiniteDimensionalSmall.ambientWeyl->
        getFundamentalCoordinatesFromSimple(simpleGeneratorBaseField);
        if (remainingCharProjected.monomials.contains(workingMonomial)) {
          localHighest = workingMonomial;
          Found = true;
        }
      }
    }
    highestCoeff =
    remainingCharProjected.coefficients[
      remainingCharProjected.monomials.getIndex(localHighest)
    ];
    output.addMonomial(localHighest, highestCoeff);
    if (
      !weylGroupFiniteDimensionalSmall.freudenthalFormulaIrrepIsWRTLeviPart(
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
      workingMonomial.owner = &smallAlgebra;
      workingMonomial.weightFundamentalCoordinates =
      weylGroupFiniteDimensionalSmall.ambientWeyl->
      getFundamentalCoordinatesFromSimple(tempHashedRoots[i]);
      bufferCoefficient = tempMults[i];
      bufferCoefficient *= highestCoeff;
      remainingCharProjected.subtractMonomial(
        workingMonomial, bufferCoefficient
      );
    }
  }
  format.fundamentalWeightLetter = "\\psi";
  out
  << "<br>Character w.r.t the Levi part "
  << "of the parabolic of the small algebra: "
  << HtmlRoutines::getMathNoDisplay(output.toString(&format));
  if (report != nullptr) {
    DrawingVariables drawingVariables1;
    std::string tempS;
    output.drawMeNoMultiplicities(tempS, drawingVariables1, 10000);
    Vector<Rational> leftRoot;
    Vector<Rational> rightRoot;
    weylGroupFiniteDimensionalSmall.ambientWeyl->group.computeAllElements(
      false, 20
    );
    out << "<hr>";
    for (int i = 0; i < output.size(); i ++) {
      leftRoot =
      weylGroupFiniteDimensionalSmall.ambientWeyl->
      getSimpleCoordinatesFromFundamental(
        output[i].weightFundamentalCoordinates
      ).getVectorRational();
      std::stringstream currentStream;
      currentStream << output.coefficients[i].toString();
      drawingVariables1.drawTextAtVector(
        leftRoot, currentStream.str(), "black"
      );
      for (
        int j = 1; j < weylGroupFiniteDimensionalSmall.ambientWeyl->group.
        elements.size; j ++
      ) {
        rightRoot = leftRoot;
        weylGroupFiniteDimensionalSmall.ambientWeyl->actOnRhoModified(
          j, rightRoot
        );
        drawingVariables1.drawCircleAtVectorBufferRational(
          rightRoot, "#a00000", 5
        );
      }
    }
    out
    << "<hr>"
    << drawingVariables1.getHTMLDiv(
      weylGroupFiniteDimensionalSmall.ambientWeyl->getDimension(), true
    );
    *report = out.str();
  }
  return true;
}

template <class Coefficient>
void SemisimpleLieAlgebra::getKillingFormMatrix(
  Matrix<Coefficient>& output
) {
  output.makeZeroMatrix(this->getNumberOfGenerators(), 0);
  ElementSemisimpleLieAlgebra<Coefficient> left, right;
  for (int i = 0; i < this->getNumberOfGenerators(); i ++) {
    left.makeGenerator(i, *this);
    for (int j = 0; j < this->getNumberOfGenerators(); j ++) {
      right.makeGenerator(j, *this);
      output(i, j) = this->getKillingForm(left, right);
    }
  }
}

template <class Coefficient>
Coefficient SemisimpleLieAlgebra::getKillingForm(
  const ElementSemisimpleLieAlgebra<Coefficient>& left,
  const ElementSemisimpleLieAlgebra<Coefficient>& right
) {
  STACK_TRACE("SemisimpleLieAlgebra::getKillingForm");
  Coefficient result = 0;
  ElementSemisimpleLieAlgebra<Coefficient> adadAppliedToMonomial, element;
  ChevalleyGenerator baseGenerator;
  for (int i = 0; i < this->getNumberOfGenerators(); i ++) {
    baseGenerator.makeGenerator(*this, i);
    adadAppliedToMonomial.makeZero();
    adadAppliedToMonomial.addMonomial(baseGenerator, 1);
    this->lieBracket(right, adadAppliedToMonomial, element);
    this->lieBracket(left, element, adadAppliedToMonomial);
    result += adadAppliedToMonomial.getCoefficientOf(baseGenerator);
  }
  return result;
}

template <typename Coefficient>
bool SemisimpleLieAlgebra::getElementStandardRepresentation(
  const ElementSemisimpleLieAlgebra<Coefficient>& element,
  Matrix<Coefficient>& output
) {
  STACK_TRACE("SemisimpleLieAlgebra::getElementStandardRepresentation");
  char dynkinType = 0;
  if (!this->weylGroup.dynkinType.isSimple(&dynkinType, nullptr)) {
    return false;
  }
  if (dynkinType != 'A') {
    // Only type A is supported as of writing.
    return false;
  }
  output.makeZeroMatrix(this->getRank() + 1, 0);
  for (int i = 0; i < element.size(); i ++) {
    this->accumulateChevalleyGeneratorStandardRepresentation(
      element[i], element.coefficients[i], output
    );
  }
  return true;
}

template <typename Coefficient>
bool SemisimpleLieAlgebra::getElementStandardRepresentation(
  const ElementUniversalEnveloping<Coefficient>& element,
  Matrix<Coefficient>& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("SemisimpleLieAlgebra::getElementStandardRepresentation");
  char dynkinType = 0;
  if (!this->weylGroup.dynkinType.isSimple(&dynkinType, nullptr)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "getElementStandardRepresentation "
      << "implemented only for simple Lie algebras.";
    }
    return false;
  }
  if (dynkinType != 'A') {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "getElementStandardRepresentation implemented only for type A.";
    }
    // Only type A is supported as of writing.
    return false;
  }
  int dimension = this->getRank() + 1;
  output.makeZeroMatrix(dimension, 0);
  Matrix<Coefficient> generatorMatrix, monomialMatrix, powerMatrix, identity;
  identity.makeIdentity(output, 0, 1);
  Coefficient one, zero;
  one = 1;
  zero = 0;
  for (int i = 0; i < element.size(); i ++) {
    const MonomialUniversalEnveloping<Coefficient>& monomial = element[i];
    monomialMatrix.makeIdentity(output, 0, 1);
    for (int j = 0; j < monomial.generatorsIndices.size; j ++) {
      ChevalleyGenerator generator;
      generator.makeGenerator(*this, monomial.generatorsIndices[j]);
      powerMatrix.initialize(dimension, dimension);
      powerMatrix.makeZero(zero);
      this->accumulateChevalleyGeneratorStandardRepresentation(
        generator, one, powerMatrix
      );
      int power = 0;
      if (!monomial.powers[j].isSmallInteger(&power)) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "Failed to extract powers. ";
        }
        return false;
      }
      MathRoutines::raiseToPower(powerMatrix, power, identity);
      monomialMatrix.multiplyOnTheRight(powerMatrix);
    }
    monomialMatrix *= element.coefficients[i];
    output += monomialMatrix;
  }
  return true;
}

template <typename Coefficient>
bool SemisimpleLieAlgebra::accumulateChevalleyGeneratorStandardRepresentation(
  const ChevalleyGenerator& element,
  const Coefficient& coefficient,
  Matrix<Coefficient>& output
) {
  char dynkinType = 0;
  int rank = 0;
  if (!this->weylGroup.dynkinType.isSimple(&dynkinType, &rank)) {
    return false;
  }
  if (dynkinType != 'A') {
    // Only type A is supported as of writing.
    return false;
  }
  if (element.owner != this) {
    global.fatal
    << "Generator does not belong to this semisimple Lie algebra."
    << global.fatal;
  }
  return
  accumulateChevalleyGeneratorStandardRepresentationInTypeA(
    element, coefficient, output
  );
}

template <typename Coefficient>
bool SemisimpleLieAlgebra::
accumulateChevalleyGeneratorStandardRepresentationInTypeA(
  const ChevalleyGenerator& element,
  const Coefficient& coefficient,
  Matrix<Coefficient>& output
) {
  STACK_TRACE(
    "SemisimpleLieAlgebra::"
    "accumulateChevalleyGeneratorStandardRepresentationInTypeA"
  );
  if (element.isInCartan(nullptr)) {
    int index =
    this->getCartanCoordinateIndexFromCartanGeneratorIndex(
      element.generatorIndex
    );
    output(index, index) += coefficient;
    output(index + 1, index + 1) -= coefficient;
    return true;
  }
  Vector<Rational> vector;
  if (!element.isInRootSpace(&vector)) {
    global.fatal
    << "Chevalley generator is neither in a "
    << "root space nor in the Cartan subalgebra. "
    << global.fatal;
  }
  int firstNonZeroIndex = 0;
  for (; firstNonZeroIndex < vector.size; firstNonZeroIndex ++) {
    if (vector[firstNonZeroIndex] != 0) {
      break;
    }
  }
  int lastNonZeroIndex = firstNonZeroIndex;
  for (; lastNonZeroIndex < vector.size; lastNonZeroIndex ++) {
    if (vector[lastNonZeroIndex] == 0) {
      break;
    }
  }
  int rowIndex = 0;
  int columnIndex = 0;
  if (vector[firstNonZeroIndex] > 0) {
    rowIndex = firstNonZeroIndex;
    columnIndex = lastNonZeroIndex;
  } else {
    rowIndex = lastNonZeroIndex;
    columnIndex = firstNonZeroIndex;
  }
  output(rowIndex, columnIndex) += coefficient;
  return true;
}

template <typename Coefficient>
bool SemisimpleLieAlgebra::hasImplementedCartanInvolutionMaximallyCompactCase(
  LinearMapSemisimpleLieAlgebra<Coefficient>* whichInvolution
) {
  char dynkinType = 0;
  if (!this->weylGroup.dynkinType.isSimple(&dynkinType)) {
    return false;
  }
  if (dynkinType != 'A') {
    return false;
  }
  if (whichInvolution == nullptr) {
    return true;
  }
  whichInvolution->imagesChevalleyGenerators.setSize(
    this->getNumberOfGenerators()
  );
  int numberOfPositiveRoots = this->getNumberOfPositiveRoots();
  int rank = this->getRank();
  for (int i = 0; i < numberOfPositiveRoots; i ++) {
    const Vector<Rational>& root = this->weylGroup.rootsOfBorel[i];
    ChevalleyGenerator positive;
    positive.makeGeneratorRootSpace(*this, root);
    ChevalleyGenerator negative;
    negative.makeGeneratorRootSpace(*this, - root);
    whichInvolution->imagesChevalleyGenerators[positive.generatorIndex].
    makeGGenerator(- root, *this);
    whichInvolution->imagesChevalleyGenerators[positive.generatorIndex] *= - 1;
    whichInvolution->imagesChevalleyGenerators[negative.generatorIndex].
    makeGGenerator(root, *this);
    whichInvolution->imagesChevalleyGenerators[negative.generatorIndex] *= - 1;
  }
  for (int i = 0; i < rank; i ++) {
    ElementSemisimpleLieAlgebra<Coefficient>& current =
    whichInvolution->imagesChevalleyGenerators[numberOfPositiveRoots + i];
    Vector<Rational> root;
    root.makeEi(rank, i);
    current.makeCartanGenerator(root, *this);
    current *= - 1;
  }
  return true;
}

template <class Coefficient>
void LinearMapSemisimpleLieAlgebra<Coefficient>::applyTo(
  const ElementSemisimpleLieAlgebra<Coefficient>& input,
  ElementSemisimpleLieAlgebra<Coefficient>& output
) {
  STACK_TRACE("LinearMapSemisimpleLieAlgebra::applyTo");
  if (&input == &output) {
    ElementSemisimpleLieAlgebra<Coefficient> copy = input;
    this->applyTo(copy, output);
    return;
  }
  output.makeZero();
  ElementSemisimpleLieAlgebra<Coefficient> contribution;
  for (int i = 0; i < input.size(); i ++) {
    const ChevalleyGenerator& current = input.monomials[i];
    ElementSemisimpleLieAlgebra<Coefficient> contribution =
    this->imagesChevalleyGenerators[current.generatorIndex];
    contribution *= input.coefficients[i];
    output += contribution;
  }
}

template <class Coefficient>
SemisimpleLieAlgebra&LinearMapSemisimpleLieAlgebra<Coefficient>::owner() {
  for (int i = 0; i < this->imagesChevalleyGenerators.size; i ++) {
    ElementSemisimpleLieAlgebra<Coefficient>& current =
    this->imagesChevalleyGenerators[i];
    if (current.size() == 0) {
      continue;
    }
    return *current.getOwner();
  }
  global.fatal
  << "owner() function called on zero or empty "
  << "linear map of semisimple lie algebras. "
  << global.fatal;
  SemisimpleLieAlgebra* empty = nullptr;
  return *empty;
}

template <class Coefficient>
void LinearMapSemisimpleLieAlgebra<Coefficient>::findEigenSpace(
  const Coefficient& eigenValue,
  List<ElementSemisimpleLieAlgebra<Coefficient> >& outputBasis
) {
  STACK_TRACE("LinearMapSemisimpleLieAlgebra::findEigenSpace");
  outputBasis.clear();
  Matrix<Coefficient> matrix;
  this->getMatrix(matrix);
  List<Vector<Coefficient> > eigenSpace;
  matrix.getEigenspaceModifyMe(eigenValue, eigenSpace);
  ChevalleyGenerator generator;
  SemisimpleLieAlgebra& ownerAlgebra = this->owner();
  for (int i = 0; i < eigenSpace.size; i ++) {
    ElementSemisimpleLieAlgebra<Coefficient> next;
    Vector<Coefficient>& current = eigenSpace[i];
    for (int j = 0; j < current.size; j ++) {
      generator.makeGenerator(ownerAlgebra, j);
      next.addMonomial(generator, current[j]);
    }
    outputBasis.addOnTop(next);
  }
}

template <class Coefficient>
void LinearMapSemisimpleLieAlgebra<Coefficient>::getMatrix(
  Matrix<Coefficient>& output
) {
  STACK_TRACE("LinearMapSemisimpleLieAlgebra::getMatrix");
  if (this->imagesChevalleyGenerators.size == 0) {
    output.makeZero();
    return;
  }
  output.makeZeroMatrix(this->imagesChevalleyGenerators.size, 0);
  for (int i = 0; i < this->imagesChevalleyGenerators.size; i ++) {
    ElementSemisimpleLieAlgebra<Coefficient>& current =
    this->imagesChevalleyGenerators[i];
    for (int j = 0; j < current.size(); j ++) {
      const ChevalleyGenerator& monomial = current.monomials[j];
      int rowIndex = monomial.generatorIndex;
      output(rowIndex, i) = current.coefficients[j];
    }
  }
}

#endif // header_math_extra_semisimple_lie_algebras_implementation_ALREADY_INCLUDED
