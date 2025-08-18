#include "calculator_interface.h"
#include "calculator_weyl_group_characters.h"
#include "math_extra_finite_groups.h"
#include "math_extra_finite_groups_implementation.h"
#include "math_extra_graph.h"
#include "math_extra_symmetric_groups_and_generalizations.h"
#include "math_general.h"
#include "math_general_implementation.h"
#include "math_general_polynomial_computations_basic_implementation.h" // IWYU pragma: keep: breaks g++ 'make optimize=1' build.
#include "math_linear_combination.h"

template < >
List<ClassFunction<WeylGroupData::WeylGroupBase, Rational> >::Comparator*
FormatExpressions::getMonomialOrder<
  ClassFunction<WeylGroupData::WeylGroupBase, Rational>
>() {
  return nullptr;
}

template < >
WeylGroupData& Expression::getValueNonConst() const;

bool WeylGroupData::checkConsistency() const {
  if (this->flagDeallocated) {
    global.fatal << "Use after free of WeylGroup. " << global.fatal;
  }
  for (int i = 0; i < this->group.generators.size; i ++) {
    this->group.generators[i].checkConsistency();
  }
  this->rootsOfBorel.checkConsistency();
  return true;
}

template <typename elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::
checkInitializationFiniteDimensionalRepresentationComputation() const {
  if (this->elements.size == 0) {
    global.fatal
    << "Request to compute character hermitian product in a group whose "
    << "conjugacy classes and/or elements have "
    << "not been computed. The group reports to have "
    << this->conjugacyClassCount()
    << " conjugacy classes and "
    << this->elements.size
    << " elements. "
    << global.fatal;
    return false;
  }
  return this->checkInitializationConjugacyClasses();
}

template <typename somegroup, typename Coefficient>
bool GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::
checkAllSimpleGeneratorsAreOK() const {
  this->checkInitialization();
  for (int i = 0; i < this->ownerGroup->generators.size; i ++) {
    if (this->generators[i].numberOfRows == 0) {
      global.fatal
      << "Working with a "
      << "representation in which the action "
      << "of the simple generators is not computed. "
      << global.fatal;
      return false;
    }
  }
  return true;
}

template <typename somegroup, typename Coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::
checkRepresentationIsMultiplicativelyClosed() {
  List<Matrix<Rational> > allProducts;
  allProducts.addOnTop(this->elementImages);
  Matrix<Rational> matrix;
  ElementWeylGroup element;
  for (int i = 0; i < allProducts.size; i ++) {
    for (int j = 0; j < allProducts.size; j ++) {
      matrix = allProducts[i];
      matrix.multiplyOnTheLeft(allProducts[j]);
      element = this->ownerGroup->elements[j];
      element *= this->ownerGroup->elements[i];
      element.makeCanonical();
      int targetIndex = this->ownerGroup->elements.getIndex(element);
      if (!(matrix == this->elementImages[targetIndex])) {
        global.fatal
        << "this is a programming error: element "
        << i + 1
        << " times element "
        << j + 1
        << " is outside of the set, i.e.,  "
        << allProducts[i].toString()
        << " * "
        << allProducts[j].toString()
        << " is bad. "
        << global.fatal;
      }
    }
  }
}

template <typename somegroup, typename Coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::
computeAllGeneratorImagesFromSimple() {
  STACK_TRACE(
    "GroupRepresentationCarriesAllMatrices::"
    "computeAllGeneratorImagesFromSimple"
  );
  this->checkInitialization();
  this->ownerGroup->
  checkInitializationFiniteDimensionalRepresentationComputation();
  HashedList<ElementWeylGroup> elementsExplored;
  elementsExplored.setExpectedSize(this->ownerGroup->elements.size);
  this->elementImages[0].makeIdentityMatrix(this->getDimension());
  ElementWeylGroup currentElement;
  int rank = this->ownerGroup->getDimension();
  currentElement.makeIdentity(*this->ownerGroup);
  elementsExplored.addOnTop(currentElement);
  List<ElementWeylGroup> generators;
  generators.setSize(rank);
  for (int i = 0; i < rank; i ++) {
    generators[i].makeSimpleReflection(i, *this->ownerGroup);
  }
  for (int i = 0; i < elementsExplored.size; i ++) {
    int indexParentElement =
    this->ownerGroup->elements.getIndex(elementsExplored[i]);
    for (int j = 0; j < rank; j ++) {
      currentElement = generators[j] * elementsExplored[i];
      if (!elementsExplored.contains(currentElement)) {
        int indexCurrentElement =
        this->ownerGroup->elements.getIndex(currentElement);
        this->elementIsComputed[indexCurrentElement] = true;
        this->elementImages[indexParentElement].multiplyOnTheLeft(
          this->generators[j], this->elementImages[indexCurrentElement]
        );
        elementsExplored.addOnTop(currentElement);
      }
    }
  }
}

// This method uses auto everywhere, and a lot of copying data in order to use
// auto.
template <typename somegroup, typename Coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::
computeAllElementImages() {
  STACK_TRACE(
    "GroupRepresentationCarriesAllMatrices::"
    "ComputeAllGeneratorImagesFromSimple"
  );
  this->checkInitialization();
  this->ownerGroup->
  checkInitializationFiniteDimensionalRepresentationComputation();
  auto elementsExplored = this->ownerGroup->elements;
  elementsExplored.clear();
  elementsExplored.setExpectedSize(this->ownerGroup->elements.size);
  this->elementImages[0].makeIdentityMatrix(this->getDimension());
  auto currentElement = this->ownerGroup->generators[0];
  currentElement.makeIdentity(this->ownerGroup->generators[0]);
  int rank = this->ownerGroup->generators.size;
  auto& generators = this->ownerGroup->generators;
  elementsExplored.addOnTop(currentElement);
  for (int i = 0; i < elementsExplored.size; i ++) {
    int indexParentElement =
    this->ownerGroup->elements.getIndex(elementsExplored[i]);
    for (int j = 0; j < rank; j ++) {
      currentElement = generators[j] * elementsExplored[i];
      if (!elementsExplored.contains(currentElement)) {
        int indexCurrentElement =
        this->ownerGroup->elements.getIndex(currentElement);
        this->elementIsComputed[indexCurrentElement] = true;
        this->elementImages[indexParentElement].multiplyOnTheLeft(
          this->generators[j], this->elementImages[indexCurrentElement]
        );
        elementsExplored.addOnTop(currentElement);
      }
    }
  }
}

template <typename somegroup, typename Coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::operator*=(
  const GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>& other
) {
  STACK_TRACE("GroupRepresentationCarriesAllMatrices::operator*=");
  if (&other == this) {
    GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> otherCopy;
    otherCopy = other;
    *this *= otherCopy;
    return;
  }
  if (this->ownerGroup != other.ownerGroup) {
    global.fatal
    << "Attempt to multiply representations with different owner groups. "
    << global.fatal;
  }
  if (!this->flagCharacterIsComputed || !other.flagCharacterIsComputed) {
    global.fatal
    << "Attempting to multiply Weyl group reps "
    << "whose characters have not been computed. "
    << global.fatal;
  }
  GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> output;
  output.initialize(*this->ownerGroup);
  output.character = this->character;
  output.character *= other.character;
  for (int i = 0; i < output.generators.size; i ++) {
    output.generators[i].assignTensorProduct(
      this->generators[i], other.generators[i]
    );
  }
  for (int i = 0; i < output.elementImages.size; i ++) {
    if (this->elementIsComputed[i] && other.elementIsComputed[i]) {
      output.elementImages[i].assignTensorProduct(
        this->elementImages[i], other.elementImages[i]
      );
      output.elementIsComputed[i] = true;
    }
  }
  *this = output;
}

template <typename somegroup, typename Coefficient>
void GroupRepresentation<somegroup, Coefficient>::operator*=(
  const GroupRepresentation<somegroup, Coefficient>& other
) {
  STACK_TRACE("GroupRepresentation::operator*=");
  if (&other == this) {
    GroupRepresentation<somegroup, Coefficient> otherCopy;
    otherCopy = other;
    *this *= otherCopy;
    return;
  }
  if (this->ownerGroup != other.ownerGroup) {
    global.fatal
    << "Attempt to multiply representations with different owner groups. "
    << global.fatal;
  }
  if (!this->flagCharacterIsComputed || !other.flagCharacterIsComputed) {
    global.fatal
    << "Attempting to multiply Weyl group reps "
    << "whose characters have not been computed. "
    << global.fatal;
  }
  GroupRepresentation<somegroup, Coefficient> output;
  output.ownerGroup = this->ownerGroup;
  output.character = this->character;
  output.character *= other.character;
  for (int i = 0; i < output.generators.size; i ++) {
    output.generators[i].assignTensorProduct(
      this->generators[i], other.generators[i]
    );
  }
  *this = output;
}

template <typename somegroup, typename Coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::
restrictRepresentation(
  const Vectors<Coefficient>& vectorSpaceBasisSubrep,
  const ClassFunction<somegroup, Rational>& remainingCharacter,
  GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>& output
) {
  STACK_TRACE(
    "GroupRepresentationCarriesAllMatrices::restrictRepresentation"
  );
  this->checkAllSimpleGeneratorsAreOK();
  if (vectorSpaceBasisSubrep.size == 0) {
    global.fatal
    << "Restriction of representation to a zero subspace is not allowed. "
    << global.fatal;
  }
  output.initialize(*this->ownerGroup);
  output.basis = vectorSpaceBasisSubrep;
  output.basis.getGramMatrix(output.gramMatrixInverted, 0);
  output.gramMatrixInverted.invert();
  output.character = remainingCharacter;
  ProgressReport report;
  for (int i = 0; i < this->generators.size; i ++) {
    if (report.tickAndWantReport()) {
      std::stringstream reportStream;
      reportStream << "Restricting the action of generator of index " << i;
      report.report(reportStream.str());
    }
    Matrix<Coefficient>::matrixInBasis(
      this->generators[i],
      output.generators[i],
      output.basis,
      output.gramMatrixInverted
    );
  }
  output.checkAllSimpleGeneratorsAreOK();
}

template <typename somegroup, typename Coefficient>
bool GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::
decomposeTodorsVersion(
  VirtualRepresentation<somegroup, Coefficient>& outputIrrepMults,
  List<GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> >*
  appendOnlyGRCAMSList
) {
  STACK_TRACE(
    "GroupRepresentationCarriesAllMatrices::decomposeTodorsVersion"
  );
  this->checkInitialization();
  this->ownerGroup->
  checkInitializationFiniteDimensionalRepresentationComputation();
  outputIrrepMults.makeZero();
  List<GroupRepresentation<somegroup, Coefficient> > appendOnlyIrrepsList;
  for (
    int i = 0; i < this->ownerGroup->irreducibleRepresentations.size; i ++
  ) {
    appendOnlyIrrepsList.addOnTop(
      this->ownerGroup->irreducibleRepresentations[i]
    );
  }
  return
  this->decomposeTodorsVersionRecursive(
    outputIrrepMults, appendOnlyIrrepsList, appendOnlyGRCAMSList
  );
}

void WeylGroupData::computeIrreducibleRepresentationsWithFormulasImplementation
(FiniteGroup<ElementWeylGroup>& g) {
  List<char> letters;
  List<int> ranks;
  WeylGroupData& weylData = *(g.generators[0].owner);
  weylData.dynkinType.getLettersTypesMultiplicities(&letters, &ranks, nullptr);
  // When WeylGroupRepresentation is merged with GroupRepresentation,
  // and WeylGroupData split from FiniteGroup<ElementWeylGroup<WeylGroup> >
  // representations will be this->group->irreps
  // currently we have the difficulty with GroupRepresentation<WeylGroup,
  // Coefficient> vs
  // GroupRepresentation<FiniteGroup<ElementWeylGroup<WeylGroup> >,
  // Coefficient>
  if ((letters.size == 1) && (letters[0] == 'A')) {
    int rank = ranks[0];
    List<Partition> partitions;
    Partition::getPartitions(partitions, rank + 1);
    for (int i = 0; i < partitions.size; i ++) {
      GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> irrep;
      partitions[i].spechtModuleMatricesOfTranspositionsjjplusone(
        irrep.generators
      );
      irrep.ownerGroup = &g;
      irrep.identifyingString = partitions[i].toString();
      irrep.computeCharacter();
      g.addIrreducibleRepresentation(irrep);
    }
  } else if ((letters.size == 1) && (letters[0] == 'B')) {
    int rank = ranks[0];
    HyperoctahedralGroupData HOG;
    HOG.makeHyperoctahedralGroup(rank);
    HOG.allSpechtModules();
    GroupHomomorphism<ElementWeylGroup, ElementHyperoctahedralGroupR2> phi;
    phi.preimageGroup = &g;
    phi.generatorImages.setSize(g.generators.size);
    for (int i = 0; i < phi.generatorImages.size - 1; i ++) {
      phi.generatorImages[i].h.addTransposition(i, i + 1);
    }
    for (int i = 0; i < phi.generatorImages.size - 1; i ++) {
      phi.generatorImages.lastObject()->k.toggleBit(i);
    }
    global << "Generator commutation relations of groups\n";
    global << HOG.group->prettyPrintGeneratorCommutationRelations();
    FiniteGroup<ElementHyperoctahedralGroupR2> phiG;
    phiG.generators = phi.generatorImages;
    global << phiG.prettyPrintGeneratorCommutationRelations();
    global << g.prettyPrintGeneratorCommutationRelations();
    global << "pulling back irreps:\n";
    for (int i = 0; i < HOG.group->irreducibleRepresentations.size; i ++) {
      auto irreducibleRepresentation =
      phi.pullbackRepresentation(HOG.group->irreducibleRepresentations[i]);
      irreducibleRepresentation.computeCharacter();
      global
      << HOG.group->irreducibleRepresentations[i].character
      << "->"
      << irreducibleRepresentation.character
      << '\n';
      g.addIrreducibleRepresentation(irreducibleRepresentation);
    }
  } else if ((letters.size == 1) && (letters[0] == 'D')) {
    int rank = ranks[0];
    HyperoctahedralGroupData HOG;
    HOG.makeHyperoctahedralGroup(rank + 1);
    GroupHomomorphism<ElementWeylGroup, ElementHyperoctahedralGroupR2>
    inclusionMap;
    inclusionMap.preimageGroup = &g;
    inclusionMap.generatorImages.setSize(g.generators.size);
    for (int i = 0; i < inclusionMap.generatorImages.size - 1; i ++) {
      inclusionMap.generatorImages[i].h.addTransposition(i, i + 1);
      inclusionMap.generatorImages[i].k.toggleBit(i);
      inclusionMap.generatorImages[i].k.toggleBit(i + 1);
    }
    int oneortwo = (inclusionMap.generatorImages.size + 1) % 2;
    for (
      int i = 0; i < inclusionMap.generatorImages.size - 1 - oneortwo; i ++
    ) {
      inclusionMap.generatorImages.lastObject()->k.toggleBit(i);
    }
    FiniteGroup<ElementHyperoctahedralGroupR2> imG;
    imG.generators = inclusionMap.generatorImages;
    global << HOG.group->prettyPrintGeneratorCommutationRelations() << '\n';
    global << imG.prettyPrintGeneratorCommutationRelations();
    global << g.prettyPrintGeneratorCommutationRelations() << '\n';
  }
  // silently fail instead of crashing to support calling into this and if it
  // doesn't
  // work then using brute force
  //  else
  // global.fatal << "ComputeIrreducibleRepresentationsUsingSpechtModules: Type
  // "
  // << this->dynkinType << " is unsupported. If you think it should work, edit
  // " << __FILE__ << ":" << __LINE__ << global.fatal;
  global << g.prettyPrintCharacterTable() << '\n';
}

template <class Coefficient>
void WeylGroupData::raiseToMaximallyDominant(
  List<Vector<Coefficient> >& weights
) {
  bool found = false;
  for (int i = 0; i < weights.size; i ++) {
    do {
      found = false;
      for (int j = 0; j < this->rootsOfBorel.size; j ++) {
        if (this->rootScalarCartanRoot(this->rootsOfBorel[j], weights[i]) < 0) {
          bool isGood = true;
          for (int k = 0; k < i; k ++) {
            if (
              this->rootScalarCartanRoot(this->rootsOfBorel[j], weights[k]) > 0
            ) {
              isGood = false;
              break;
            }
          }
          if (!isGood) {
            continue;
          }
          for (int k = 0; k < weights.size; k ++) {
            this->reflectBetaWithRespectToAlpha(
              this->rootsOfBorel[j], weights[k], false, weights[k]
            );
          }
          found = true;
        }
      }
    } while (found);
  }
}

bool CalculatorFunctionsWeylGroup::weylRaiseToMaximallyDominant(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool useOuter
) {
  STACK_TRACE("CalculatorFunctionsWeylGroup::weylRaiseToMaximallyDominant");
  if (input.size() < 2) {
    return
    output.assignError(
      calculator,
      "Raising to maximally dominant takes at least 2 arguments, type and vector"
    );
  }
  const Expression& semisimpleLieAlgebraNode = input[1];
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebraWithContext;
  if (
    !CalculatorConversions::convert(
      calculator, semisimpleLieAlgebraNode, semisimpleLieAlgebraWithContext
    )
  ) {
    return output.assignError(calculator, "Error extracting Lie algebra.");
  }
  SemisimpleLieAlgebra* semisimpleLieAlgebra =
  semisimpleLieAlgebraWithContext.content;
  Vectors<Rational> highestWeights;
  highestWeights.setSize(input.size() - 2);
  bool isGood = true;
  for (int i = 2; i < input.size(); i ++) {
    if (
      !calculator.getVector<Rational>(
        input[i],
        highestWeights[i - 2],
        nullptr,
        semisimpleLieAlgebra->getRank()
      )
    ) {
      isGood = false;
      break;
    }
  }
  if (!isGood && input.size() == 3) {
    Matrix<Rational> highestWeightsMatrixForm;
    if (
      CalculatorConversions::functionGetMatrix(
        calculator,
        input[2],
        highestWeightsMatrixForm,
        false,
        nullptr,
        semisimpleLieAlgebra->getRank()
      )
    ) {
      highestWeightsMatrixForm.getVectorsFromRows(highestWeights);
      isGood = true;
    }
  }
  if (highestWeights.size == 0 || !isGood) {
    return
    output.assignError(
      calculator,
      "Failed to extract rational vectors from expression " +
      input.toString() +
      ". "
    );
  }
  std::stringstream out;
  out
  << "Input: "
  << highestWeights.toString()
  << ", simultaneously raising to maximally dominant in Weyl group of type "
  << semisimpleLieAlgebra->weylGroup.dynkinType.toString();
  if (!useOuter) {
    semisimpleLieAlgebra->weylGroup.raiseToMaximallyDominant(highestWeights);
  } else {
    WeylGroupAutomorphisms outerAutomorphisms;
    outerAutomorphisms.weylGroup = &semisimpleLieAlgebra->weylGroup;
    outerAutomorphisms.raiseToMaximallyDominant(highestWeights);
  }
  out << "<br>Maximally dominant output: " << highestWeights.toString();
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsWeylGroup::weylGroupOrbitOuterSimple(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsWeylGroup::weylGroupOrbitOuterSimple");
  if (!input.isListNElements(3)) {
    return output.assignError(calculator, "weylOrbit takes two arguments");
  }
  const Expression& algebraExpression = input[1];
  const Expression& vectorNode = input[2];
  DynkinType dynkinType;
  if (algebraExpression.isOfType<SemisimpleLieAlgebra*>()) {
    SemisimpleLieAlgebra* algebra = algebraExpression.getValueNonConst<SemisimpleLieAlgebra*>();
    dynkinType = algebra->weylGroup.dynkinType;
  } else {
    if (
      !CalculatorConversions::functionDynkinType(
        calculator, algebraExpression, dynkinType
      )
    ) {
      return false;
    }
  }
  Vector<Polynomial<Rational> > highestWeightFundamentalCoordinates;
  Vector<Polynomial<Rational> > highestWeightSimpleCoordinates;
  ExpressionContext context(calculator);
  if (
    !calculator.getVector(
      vectorNode,
      highestWeightFundamentalCoordinates,
      &context,
      dynkinType.getRank() // ,
      // CalculatorConversions::functionPolynomial<Rational>
    )
  ) {
    return output.assignError(calculator, "Failed to extract highest weight");
  }
  WeylGroupData weyl;
  weyl.makeFromDynkinType(dynkinType);
  highestWeightSimpleCoordinates = highestWeightFundamentalCoordinates;
  highestWeightFundamentalCoordinates =
  weyl.getFundamentalCoordinatesFromSimple(highestWeightSimpleCoordinates);
  std::stringstream out;
  std::stringstream latexReport;
  Vectors<Polynomial<Rational> > highestWeights;
  FormatExpressions format = context.getFormat();
  highestWeights.addOnTop(highestWeightSimpleCoordinates);
  HashedList<Vector<Polynomial<Rational> > > outputOrbit;
  WeylGroupAutomorphisms outerAutomophisms;
  outerAutomophisms.weylGroup = &weyl;
  if (
    !outerAutomophisms.generateOuterOrbit(
      highestWeights, outputOrbit, &outerAutomophisms.allElements, 1921* 2
    )
  ) {
    out
    << "Failed to generate the entire orbit "
    << "(maybe too large?), generated the first "
    << outputOrbit.size
    << " elements only.";
  } else {
    out << "The orbit has " << outputOrbit.size << " elements.";
  }
  latexReport
  << "\\begin{longtable}{p{3cm}p{4cm}p{4cm}p{4cm}}"
  << "Element & Eps. coord. & "
  << "Image fund. coordinates& Hw minus wt. \\\\\n<br>";
  out
  << "<table><tr> <td>Group element</td> "
  << "<td>Image in simple coords</td> "
  << "<td>Epsilon coords</td><td>Fundamental coords</td>";
  out << "</tr>";
  bool useMathTag = outputOrbit.size < 150;
  Matrix<Rational> epsilonCoordinateMatrix;
  Polynomial<Rational> zero;
  weyl.dynkinType.getEpsilonMatrix(epsilonCoordinateMatrix);
  for (int i = 0; i < outputOrbit.size; i ++) {
    format.simpleRootLetter = "\\alpha";
    format.fundamentalWeightLetter = "\\psi";
    std::string orbitElementString = outputOrbit[i].toString(&format);
    Vector<Polynomial<Rational> > epsilonVector = outputOrbit[i];
    epsilonCoordinateMatrix.actOnVectorColumn(epsilonVector, zero);
    std::string orbitElementStringEpsilonCoordinates =
    epsilonVector.toStringLetterFormat("\\varepsilon", &format);
    std::string weightElementString =
    weyl.getFundamentalCoordinatesFromSimple(outputOrbit[i]).
    toStringLetterFormat(format.fundamentalWeightLetter, &format);
    out
    << "<tr>"
    << "<td>"
    << (
      useMathTag ?
      HtmlRoutines::getMathNoDisplay(
        outerAutomophisms.allElements[i].toString()
      ) :
      outerAutomophisms.allElements[i].toString()
    )
    << "</td><td>"
    << (
      useMathTag ?
      HtmlRoutines::getMathNoDisplay(orbitElementString) :
      orbitElementString
    )
    << "</td><td>"
    << (
      useMathTag ?
      HtmlRoutines::getMathNoDisplay(orbitElementStringEpsilonCoordinates) :
      orbitElementStringEpsilonCoordinates
    )
    << "</td><td>"
    << (
      useMathTag ?
      HtmlRoutines::getMathNoDisplay(weightElementString) :
      weightElementString
    )
    << "</td>";
    latexReport
    << "$"
    << outerAutomophisms.allElements[i].toString(&format)
    << "$ & $"
    << orbitElementStringEpsilonCoordinates
    << "$ & $"
    << weightElementString
    << "$ & $"
    << (outputOrbit[0] - outputOrbit[i]).toStringLetterFormat(
      format.simpleRootLetter, &format
    )
    << "$\\\\\n<br>";
    out << "</tr>";
  }
  latexReport << "\\end{longtable}";
  out << "</table>" << "<br> " << latexReport.str();
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsWeylGroup::weylGroupOrbitSize(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsWeylGroup::weylGroupOrbitSize");
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebra;
  semisimpleLieAlgebra.content = nullptr;
  Vector<Rational> weightRational;
  if (
    calculator.getTypeWeight<Rational>(
      calculator, input, weightRational, semisimpleLieAlgebra
    )
  ) {
    Rational result =
    semisimpleLieAlgebra.content->weylGroup.getOrbitSize(weightRational);
    return output.assignValue(calculator, result);
  }
  Vector<Polynomial<Rational> > weightPolynomial;
  if (
    calculator.getTypeWeight<Polynomial<Rational> >(
      calculator, input, weightPolynomial, semisimpleLieAlgebra
    )
  ) {
    Rational result =
    semisimpleLieAlgebra.content->weylGroup.getOrbitSize(weightPolynomial);
    return output.assignValue(calculator, result);
  }
  return false;
}

bool CalculatorFunctionsWeylGroup::weylOrbit(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool useFundamentalCoordinates,
  bool useRho
) {
  STACK_TRACE("CalculatorFunctionsWeylGroup::weylOrbit");
  if (!input.isListNElements(3)) {
    return output.assignError(calculator, "weylOrbit takes two arguments");
  }
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebra;
  Vector<Polynomial<Rational> > weight;
  if (
    !calculator.getTypeWeight(calculator, input, weight, semisimpleLieAlgebra)
  ) {
    return false;
  }
  Vector<Polynomial<Rational> > highestWeightFundamentalCoordinates;
  Vector<Polynomial<Rational> > highestWeightSimpleCoordinates;
  Vector<Polynomial<Rational> > currentWeight;
  Polynomial<Rational> zero;
  WeylGroupData& weyl = semisimpleLieAlgebra.content->weylGroup;
  if (!useFundamentalCoordinates) {
    highestWeightSimpleCoordinates = weight;
    highestWeightFundamentalCoordinates =
    weyl.getFundamentalCoordinatesFromSimple(weight);
  } else {
    highestWeightFundamentalCoordinates = weight;
    highestWeightSimpleCoordinates =
    weyl.getSimpleCoordinatesFromFundamental(weight, zero);
  }
  std::stringstream out, latexReport;
  Vectors<Polynomial<Rational> > highestWeights;
  FormatExpressions format;
  semisimpleLieAlgebra.context.getFormat(format);
  //  format.fundamentalWeightLetter ="\\psi";
  highestWeights.addOnTop(highestWeightSimpleCoordinates);
  HashedList<Vector<Polynomial<Rational> > > outputOrbit;
  HashedList<ElementWeylGroup> orbitGeneratingSet;
  Polynomial<Rational> exponent;
  if (
    !semisimpleLieAlgebra.content->weylGroup.generateOrbit(
      highestWeights,
      useRho,
      outputOrbit,
      false,
      1921,
      &orbitGeneratingSet,
      1921
    )
  ) {
    out
    << "Failed to generate the entire orbit "
    << "(maybe too large?), generated the first "
    << outputOrbit.size
    << " elements only.";
  } else {
    out << "The orbit has " << outputOrbit.size << " elements.";
  }
  latexReport
  << "\\begin{longtable}{p{3cm}p{4cm}p{4cm}p{4cm}}Element & Eps. coord. "
  << "& Image fund. coordinates& Hw minus wt. \\\\\n<br>";
  out
  << "<table><tr> <td>Group element</td>"
  << "<td>Image in simple coords</td>"
  << "<td>Epsilon coords</td><td>Fundamental coords</td>";
  if (useRho) {
    out << "<td>Homomorphism generator candidate</td>";
  }
  out << "</tr>";
  MonomialUniversalEnveloping<Polynomial<Rational> > standardElement;
  LargeInteger largeInteger;
  bool useMathTag = outputOrbit.size < 150;
  Matrix<Rational> epsilonCoordinatesMatrix;
  weyl.dynkinType.getEpsilonMatrix(epsilonCoordinatesMatrix);
  GraphWeightedLabeledEdges integralPositiveRootReflectionGraph;
  integralPositiveRootReflectionGraph.numberOfNodes = outputOrbit.size;
  integralPositiveRootReflectionGraph.nodeLabels.setSize(outputOrbit.size);
  format.flagUseFrac = true;
  for (int i = 0; i < outputOrbit.size; i ++) {
    integralPositiveRootReflectionGraph.nodeLabels[i] =
    "$" +
    weyl.getEpsilonCoordinates(outputOrbit[i]).toStringEpsilonFormat(&format) +
    "$ = $" +
    weyl.getFundamentalCoordinatesFromSimple(outputOrbit[i]).
    toStringLetterFormat("\\psi") +
    "$";
  }
  ElementWeylGroup currentElement;
  Vector<Polynomial<Rational> > differenceVector;
  Rational currentCoordDifference;
  for (int i = 0; i < outputOrbit.size; i ++) {
    for (int j = 0; j < weyl.rootsOfBorel.size; j ++) {
      currentWeight = outputOrbit[i];
      currentElement.makeRootReflection(weyl.rootsOfBorel[j], weyl);
      if (useRho) {
        currentWeight += weyl.rho;
      }
      weyl.actOn(currentElement, currentWeight);
      if (useRho) {
        currentWeight -= weyl.rho;
      }
      differenceVector = outputOrbit[i] - currentWeight;
      bool isGood = !differenceVector.isEqualToZero();
      for (int k = 0; k < differenceVector.size; k ++) {
        if (!differenceVector[k].isConstant(&currentCoordDifference)) {
          isGood = false;
          break;
        } else if (
          !currentCoordDifference.isInteger() || currentCoordDifference < 0
        ) {
          isGood = false;
          break;
        }
      }
      if (isGood) {
        std::stringstream reflectionStream;
        reflectionStream << "s_{" << i << "}";
        integralPositiveRootReflectionGraph.addEdge(
          i, outputOrbit.getIndex(currentWeight), reflectionStream.str()
        );
      }
    }
  }
  integralPositiveRootReflectionGraph.checkConsistency();
  out << integralPositiveRootReflectionGraph.toStringPsTricks(nullptr);
  for (int i = 0; i < outputOrbit.size; i ++) {
    format.simpleRootLetter = "\\alpha";
    format.fundamentalWeightLetter = "\\psi";
    std::string orbitElementString = outputOrbit[i].toString(&format);
    Vector<Polynomial<Rational> > epsilonVector = outputOrbit[i];
    epsilonCoordinatesMatrix.actOnVectorColumn(epsilonVector, zero);
    std::string orbitElementStringEpsilonCoordinates =
    epsilonVector.toStringLetterFormat("\\varepsilon", &format);
    std::string weightElementString =
    weyl.getFundamentalCoordinatesFromSimple(outputOrbit[i]).
    toStringLetterFormat(format.fundamentalWeightLetter, &format);
    out
    << "<tr>"
    << "<td>"
    << (
      useMathTag ?
      HtmlRoutines::getMathNoDisplay(orbitGeneratingSet[i].toString()) :
      orbitGeneratingSet[i].toString()
    )
    << "</td><td>"
    << (
      useMathTag ?
      HtmlRoutines::getMathNoDisplay(orbitElementString) :
      orbitElementString
    )
    << "</td><td>"
    << (
      useMathTag ?
      HtmlRoutines::getMathNoDisplay(orbitElementStringEpsilonCoordinates) :
      orbitElementStringEpsilonCoordinates
    )
    << "</td><td>"
    << (
      useMathTag ?
      HtmlRoutines::getMathNoDisplay(weightElementString) :
      weightElementString
    )
    << "</td>";
    latexReport
    << "$"
    << orbitGeneratingSet[i].toString(&format)
    << "$ & $"
    << orbitElementStringEpsilonCoordinates
    << "$ & $"
    << weightElementString
    << "$ & $"
    << (outputOrbit[0] - outputOrbit[i]).toStringLetterFormat(
      format.simpleRootLetter, &format
    )
    << "$\\\\\n<br>";
    if (useRho) {
      currentWeight = highestWeightSimpleCoordinates;
      standardElement.makeOne(*semisimpleLieAlgebra.content);
      bool isGood = true;
      for (
        int j = orbitGeneratingSet[i].generatorsLastAppliedFirst.size - 1; j
        >=
        0; j --
      ) {
        int simpleIndex =
        orbitGeneratingSet[i].generatorsLastAppliedFirst[j].index;
        exponent = weyl.getScalarProductSimpleRoot(currentWeight, simpleIndex);
        weyl.reflectRhoSimple(simpleIndex, currentWeight);
        exponent *= 2;
        exponent /= weyl.cartanSymmetric.elements[simpleIndex][simpleIndex];
        exponent += Rational(1);
        if (exponent.isInteger(&largeInteger)) {
          if (largeInteger < 0) {
            isGood = false;
            break;
          }
        }
        standardElement.multiplyByGeneratorPowerOnTheLeft(
          semisimpleLieAlgebra.content->getNumberOfPositiveRoots() -
          simpleIndex -
          1,
          exponent
        );
      }
      out << "<td>";
      if (isGood) {
        out
        << HtmlRoutines::getMathNoDisplay(standardElement.toString(&format));
      } else {
        out << "-";
      }
      out << "</td>";
    }
    out << "</tr>";
  }
  latexReport << "\\end{longtable}";
  out << "</table>" << "<br> " << latexReport.str();
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsWeylGroup::weylGroupLoadOrComputeCharTable(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsWeylGroup::weylGroupLoadOrComputeCharTable"
  );
  if (!CalculatorConversions::loadWeylGroup(calculator, input, output)) {
    return false;
  }
  WeylGroupData& weylGroup = output.getValueNonConst<WeylGroupData>();
  if (weylGroup.getDimension() > 8) {
    calculator
    << "Computing character table disabled for rank >= 8, modify file "
    << __FILE__
    << " line "
    << __LINE__
    << " to change that. ";
    return false;
  }
  std::stringstream reportStream;
  weylGroup.computeOrLoadCharacterTable(&reportStream);
  calculator << reportStream.str();
  return output.assignValue(calculator, weylGroup);
}

bool CalculatorFunctionsWeylGroup::weylGroupConjugacyClasseS(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsWeylGroup::weylGroupConjugacyClasseS");
  if (!CalculatorConversions::loadWeylGroup(calculator, input, output)) {
    return false;
  }
  WeylGroupData& weylGroup = output.getValueNonConst<WeylGroupData>();
  if (weylGroup.getDimension() > 8) {
    calculator
    << "Conjugacy classes computation disabled for rank greater than 8. "
    << "Modify source code "
    << "file "
    << __FILE__
    << " line "
    << __LINE__
    << " and rebuild the calculator to change that.";
    return false;
  }
  std::stringstream out;
  weylGroup.computeOrLoadConjugacyClasses(&out);
  calculator << out.str();
  return output.assignValue(calculator, weylGroup);
}

bool CalculatorFunctionsWeylGroup::
weylGroupOuterConjugacyClassesFromAllElements(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsWeylGroup::"
    "weylGroupOuterConjugacyClassesFromAllElements"
  );
  if (
    !CalculatorFunctionsWeylGroup::weylGroupConjugacyClassesFromAllElements(
      calculator, input, output
    )
  ) {
    return false;
  }
  WeylGroupData& groupData = output.getValueNonConst<WeylGroupData>();
  char dynkinType = 'X';
  int rank = - 1;
  bool hasOuterAutosAndIsSimple = false;
  if (groupData.dynkinType.isSimple(&dynkinType, &rank)) {
    if (dynkinType == 'D' || dynkinType == 'A') {
      hasOuterAutosAndIsSimple = true;
    }
    if (dynkinType == 'E' && rank == 6) {
      hasOuterAutosAndIsSimple = true;
    }
  }
  if (!hasOuterAutosAndIsSimple) {
    return false;
  }
  FiniteGroup<Matrix<Rational> > groupNoOuterAutos;
  WeylGroupAutomorphisms automorphismGroup;
  automorphismGroup.weylGroup = &groupData;
  automorphismGroup.computeOuterAutoGenerators();
  groupNoOuterAutos.generators.setSize(groupData.getDimension());
  Vector<Rational> simpleRoot;
  for (int i = 0; i < groupData.getDimension(); i ++) {
    simpleRoot.makeEi(groupData.getDimension(), i);
    groupData.getMatrixReflection(simpleRoot, groupNoOuterAutos.generators[i]);
  }
  // if (false)
  Matrix<Rational> currentAutomorphism;
  List<Matrix<Rational> > outerAutomorphisms;
  for (
    int i = 0; i < automorphismGroup.outerAutomorphisms.generators.size; i ++
  ) {
    automorphismGroup.outerAutomorphisms.generators[i].getMatrix(
      currentAutomorphism, groupData.getDimension()
    );
    outerAutomorphisms.addOnTop(currentAutomorphism);
  }
  std::stringstream out;
  out << "Weyl group generators. <br>";
  for (int i = 0; i < groupNoOuterAutos.generators.size; i ++) {
    out
    << "Generator "
    << i + 1
    << ": "
    << groupNoOuterAutos.generators[i].toString()
    << "<br>";
  }
  //  out << "Outer automorphism realizations:<br>";
  //  for (int i = 0; i <outerAutossize; i ++)
  //    out << "Generator " << i << ": "
  //    << outerAutossize[i].toString() << "<br>";
  //  groupWithOuterAutos.computeAllElements(false, - 1);
  groupNoOuterAutos.computeAllElements(false, - 1);
  groupNoOuterAutos.computeConjugacyClassesFromAllElements();
  out << "Weyl group matrix realization: " << groupNoOuterAutos.toString();
  Matrix<Rational> conjugatedMatrix;
  Matrix<Rational> invertedOuterAutomorphism;
  for (int j = 0; j < outerAutomorphisms.size; j ++) {
    out
    << "Outer automorphism "
    << j
    << ": "
    << outerAutomorphisms[j].toString()
    << "<br>";
    invertedOuterAutomorphism = outerAutomorphisms[j];
    invertedOuterAutomorphism.invert();
    for (int i = 0; i < groupNoOuterAutos.conjugacyClasses.size; i ++) {
      conjugatedMatrix = outerAutomorphisms[j];
      conjugatedMatrix *= groupNoOuterAutos.conjugacyClasses[i].representative;
      conjugatedMatrix *= invertedOuterAutomorphism;
      int found = - 1;
      for (int k = 0; k < groupNoOuterAutos.conjugacyClasses.size; k ++) {
        if (
          groupNoOuterAutos.conjugacyClasses[k].elements.contains(
            conjugatedMatrix
          )
        ) {
          found = k;
          break;
        }
      }
      out << "Maps conj. class " << i + 1 << " -> " << found + 1 << "<br>";
    }
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsWeylGroup::weylGroupConjugacyClassesFromAllElements(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsWeylGroup::weylGroupConjugacyClassesFromAllElements"
  );
  if (!CalculatorConversions::loadWeylGroup(calculator, input, output)) {
    return false;
  }
  WeylGroupData& groupData = output.getValueNonConst<WeylGroupData>();
  if (groupData.getDimension() > 7) {
    calculator
    << "<hr>Loaded Dynkin type "
    << groupData.dynkinType.toString()
    << " of rank "
    << groupData.getDimension()
    << " but I've been told "
    << "not to compute when the rank is larger than 7. ";
    return false;
  }
  double timeStart1 = global.getElapsedSeconds();
  groupData.group.computeConjugacyClassesFromAllElements();
  // std::stringstream out;
  calculator
  << "<hr>Computed conjugacy classes of "
  << groupData.toString()
  << " in "
  << global.getElapsedSeconds() - timeStart1
  << " second(s). ";
  return output.assignValue(calculator, groupData);
}

bool CalculatorFunctionsWeylGroup::weylGroupConjugacyClassesRepresentatives(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsWeylGroup::weylGroupConjugacyClassesRepresentatives"
  );
  if (!CalculatorConversions::loadWeylGroup(calculator, input, output)) {
    return false;
  }
  WeylGroupData& groupData = output.getValueNonConst<WeylGroupData>();
  groupData.checkConsistency();
  if (groupData.getDimension() > 8) {
    return
    calculator
    << "<hr>Loaded Dynkin type "
    << groupData.dynkinType.toString()
    << " of rank "
    << groupData.getDimension()
    << " but I've been told "
    << "not to compute when the rank is larger than 8. ";
  }
  groupData.checkConsistency();
  double timeStart1 = global.getElapsedSeconds();
  groupData.checkConsistency();
  groupData.group.computeConjugacyClassSizesAndRepresentatives();
  calculator
  << "<hr> Computed conjugacy classes representatives of "
  << groupData.dynkinType.toString()
  << " in "
  << global.getElapsedSeconds() - timeStart1
  << " second(s). ";
  return output.assignValue(calculator, groupData);
}

bool CalculatorFunctionsWeylGroup::
weylGroupIrrepsAndCharTableComputeFromScratch(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsWeylGroup::"
    "weylGroupIrrepsAndCharTableComputeFromScratch"
  );
  if (
    !CalculatorFunctionsWeylGroup::weylGroupConjugacyClasseS(
      calculator, input, output
    )
  ) {
    return false;
  }
  if (!output.isOfType<WeylGroupData>()) {
    return true;
  }
  WeylGroupData& groupData = output.getValueNonConst<WeylGroupData>();
  groupData.computeInitialIrreducibleRepresentations();
  groupData.group.computeIrreducibleRepresentationsTodorsVersion();
  FormatExpressions currentFormat;
  currentFormat.flagUseLatex = true;
  currentFormat.flagUseHTML = false;
  std::stringstream out;
  out << "Character table: ";
  out << groupData.group.prettyPrintCharacterTable();
  out << "<br>Explicit realizations of each representation follow.";
  for (int i = 0; i < groupData.group.irreducibleRepresentations.size; i ++) {
    out
    << "<hr>"
    << groupData.group.irreducibleRepresentations[i].toString(&currentFormat);
  }
  out << groupData.toString(&currentFormat);
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsWeylGroup::weylGroupOuterAutoGeneratorsPrint(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsWeylGroup::weylGroupOuterAutoGeneratorsPrint"
  );
  DynkinType dynkinType;
  if (!CalculatorConversions::dynkinType(calculator, input, dynkinType)) {
    return
    output.assignError(
      calculator, "Failed to extract Dynkin type from argument. "
    );
  }
  std::stringstream out;
  std::stringstream outCommand;
  FinitelyGeneratedMatrixMonoid<Rational> groupGeneratedByMatrices;
  dynkinType.getOuterAutosGeneratorsActOnVectorColumn(
    groupGeneratedByMatrices.generators
  );
  FormatExpressions currentFormat;
  currentFormat.flagUseLatex = true;
  currentFormat.flagUseHTML = false;
  for (int i = 0; i < groupGeneratedByMatrices.generators.size; i ++) {
    outCommand
    << "<br>s_{"
    << i + 1
    << "}=MatrixRationals"
    << groupGeneratedByMatrices.generators[i].toStringMatrixForm(
      &currentFormat
    )
    << ";";
    out
    << "<br>s_"
    << i + 1
    << " = "
    << HtmlRoutines::getMathNoDisplay(
      groupGeneratedByMatrices.generators[i].toStringMatrixForm(
        &currentFormat
      )
    );
  }
  outCommand << "<br>GenerateFiniteMultiplicativelyClosedSet(1000, ";
  for (int i = 0; i < groupGeneratedByMatrices.generators.size; i ++) {
    outCommand << "s_{" << i + 1 << "}";
    if (i != groupGeneratedByMatrices.generators.size - 1) {
      outCommand << ", ";
    }
  }
  outCommand << ");";
  out << outCommand.str();
  bool success = groupGeneratedByMatrices.generateElements(10000);
  if (!success) {
    out
    << "<br>Did not succeed to generate all elements of "
    << "the group - the group is of size larger than 10000";
  } else {
    out
    << "<br>The group generated by the outer automorphisms is of size "
    << groupGeneratedByMatrices.elements.size;
    if (groupGeneratedByMatrices.elements.size > 100) {
      out
      << "<br>As the group has more than 100 elements, "
      << "I shall abstain from printing them. ";
    } else {
      out << "<table><tr><td>Element</td><td>Matrix</td></tr>";
      for (int i = 0; i < groupGeneratedByMatrices.elements.size; i ++) {
        std::stringstream elementNameStream;
        elementNameStream << "t_" << i + 1;
        out
        << "<tr><td>"
        << HtmlRoutines::getMathNoDisplay(elementNameStream.str())
        << "</td><td>"
        << HtmlRoutines::getMathNoDisplay(
          groupGeneratedByMatrices.elements[i].toStringMatrixForm(
            &currentFormat
          )
        )
        << "</td></tr>";
      }
      out << "</table>";
    }
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsWeylGroup::weylGroupOrbitFundRho(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return
  CalculatorFunctionsWeylGroup::weylOrbit(
    calculator, input, output, true, true
  );
}

bool CalculatorFunctionsWeylGroup::weylGroupOrbitFund(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return
  CalculatorFunctionsWeylGroup::weylOrbit(
    calculator, input, output, true, false
  );
}

bool CalculatorFunctionsWeylGroup::weylGroupOrbitSimple(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return
  CalculatorFunctionsWeylGroup::weylOrbit(
    calculator, input, output, false, false
  );
}

bool CalculatorFunctionsWeylGroup::tensorWeylReps(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsWeylGroup::tensorWeylReps");
  if (input.size() != 3) {
    return false;
  }
  GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
  leftRepresentation;
  GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
  rightRepresentation;
  if (
    !input[1].isOfType<
      GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
    >(&leftRepresentation)
  ) {
    return false;
  }
  if (
    !input[2].isOfType<
      GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
    >(&rightRepresentation)
  ) {
    return false;
  }
  FormatExpressions format;
  format.flagUseLatex = true;
  format.flagUseHTML = false;
  if (leftRepresentation.ownerGroup != rightRepresentation.ownerGroup) {
    return
    output.assignError(
      calculator,
      "Error: attempting to tensor irreps with different owner groups. "
    );
  }
  leftRepresentation *= rightRepresentation;
  return output.assignValue(calculator, leftRepresentation);
}

bool CalculatorFunctionsWeylGroup::tensorAndDecomposeWeylRepresentations(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsWeylGroup::tensorAndDecomposeWeylRepresentations"
  );
  Expression tensorExpression;
  if (input.size() != 3) {
    return false;
  }
  const Expression& leftExpression = input[1];
  const Expression& rightExpression = input[2];
  if (
    !leftExpression.isOfType<
      VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
    >()
  ) {
    return false;
  }
  if (
    !rightExpression.isOfType<
      VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
    >()
  ) {
    return false;
  }
  VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
  outputRepresentation =
  leftExpression.getValue<
    VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
  >();
  outputRepresentation *=
  rightExpression.getValue<
    VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
  >();
  return output.assignValue(calculator, outputRepresentation);
}

std::string WeylGroupData::toStringIrreducibleRepresentationLabel(
  int indexIrrep
) {
  if (indexIrrep < this->irrepsCarterLabels.size) {
    return this->irrepsCarterLabels[indexIrrep];
  }
  std::stringstream out;
  out << "\\phi_{" << indexIrrep + 1 << "}";
  return out.str();
}

std::string WeylGroupData::toStringSignSignatureRootSubsystem(
  const List<SubgroupDataRootReflections>& inputSubgroups
) {
  STACK_TRACE("WeylGroupData::toStringSignSignatureRootSubsystem");
  if (inputSubgroups.size == 0) {
    return "";
  }
  std::stringstream out;
  FormatExpressions formatSupressUpperIndexOne;
  formatSupressUpperIndexOne.flagSupressDynkinIndexOne = true;
  DynkinSimpleType simpleType;
  std::stringstream mainTableStream;
  int numberOfParabolicClasses = 0;
  int numberOfNonParabolicPseudoParabolic = 0;
  int numberOfNonPseudoParabolic = 0;
  for (int i = 0; i < inputSubgroups.size; i ++) {
    SubgroupDataRootReflections& currentSG = inputSubgroups[i];
    if (!currentSG.flagIsParabolic && !currentSG.flagIsExtendedParabolic) {
      numberOfNonPseudoParabolic ++;
      continue;
    }
    if (currentSG.flagIsParabolic) {
      numberOfParabolicClasses ++;
    }
    if (!currentSG.flagIsParabolic && currentSG.flagIsExtendedParabolic) {
      numberOfNonParabolicPseudoParabolic ++;
    }
  }
  // check for repeating signatures
  List<List<Rational> > pseudoSignSig;
  List<List<Rational> > fullSignSig;
  List<List<Rational> > parabolicSignSig;
  fullSignSig.setSize(this->group.conjugacyClassCount());
  pseudoSignSig.setSize(this->group.conjugacyClassCount());
  parabolicSignSig.setSize(this->group.conjugacyClassCount());
  for (int i = 0; i < this->group.conjugacyClassCount(); i ++) {
    fullSignSig[i].setSize(inputSubgroups.size);
    pseudoSignSig[i].setSize(
      numberOfParabolicClasses + numberOfNonParabolicPseudoParabolic
    );
    parabolicSignSig[i].setSize(numberOfParabolicClasses);
    for (int j = 0; j < inputSubgroups.size; j ++) {
      fullSignSig[i][j] = (inputSubgroups[j].tauSignature[i] == 0 ? 0 : 1);
      if (j < numberOfParabolicClasses + numberOfNonParabolicPseudoParabolic) {
        pseudoSignSig[i][j] = fullSignSig[i][j];
      }
      if (j < numberOfParabolicClasses) {
        parabolicSignSig[i][j] = fullSignSig[i][j];
      }
    }
  }
  bool hasRepeatingFullSigs = false;
  bool hasRepeatingPseudoParabolicSigs = false;
  bool hasRepeatingParSigs = false;
  for (int i = 0; i < fullSignSig.size && !hasRepeatingFullSigs; i ++) {
    for (int j = i + 1; j < fullSignSig.size && !hasRepeatingFullSigs; j ++) {
      if (fullSignSig[i] == fullSignSig[j]) {
        hasRepeatingFullSigs = true;
      }
    }
  }
  for (
    int i = 0; i < pseudoSignSig.size && !hasRepeatingPseudoParabolicSigs; i
    ++
  ) {
    for (
      int j = i + 1; j < pseudoSignSig.size &&
      !hasRepeatingPseudoParabolicSigs; j ++
    ) {
      if (pseudoSignSig[i] == pseudoSignSig[j]) {
        hasRepeatingPseudoParabolicSigs = true;
      }
    }
  }
  for (int i = 0; i < parabolicSignSig.size && !hasRepeatingParSigs; i ++) {
    for (
      int j = i + 1; j < parabolicSignSig.size && !hasRepeatingParSigs; j ++
    ) {
      if (parabolicSignSig[i] == parabolicSignSig[j]) {
        hasRepeatingParSigs = true;
      }
    }
  }
  if (hasRepeatingParSigs) {
    out << "<hr><b>There are repeating parabolic sign signatures. </b><hr>";
  } else {
    out << "<hr>No repeating parabolic sign signatures. <hr>";
  }
  if (hasRepeatingPseudoParabolicSigs) {
    out
    << "<hr><b>There are repeating pseudo-parabolic sign signatures. </b><hr>";
  } else {
    out << "<hr>No repeating pseudo-parabolic sign signatures. <hr>";
  }
  if (hasRepeatingFullSigs) {
    out << "<hr><b>There are repeating extended sign signatures.</b> <hr>";
  } else {
    out << "<hr>No repeating extended sign signatures. <hr>";
  }
  if (hasRepeatingParSigs) {
    HashedList<List<Rational> > parSignSigsNoRepetition;
    List<List<std::string> > irrepsPerSignature;
    parSignSigsNoRepetition.addOnTopNoRepetition(parabolicSignSig);
    irrepsPerSignature.setSize(parSignSigsNoRepetition.size);
    for (int i = 0; i < parabolicSignSig.size; i ++) {
      irrepsPerSignature[
        parSignSigsNoRepetition.getIndex(parabolicSignSig[i])
      ].addOnTop(this->irrepsCarterLabels[i]);
    }
    mainTableStream
    << "\n<br>\n\n<br>\nThe following families of "
    << "representations share the same sign signature. ";
    for (int i = 0; i < irrepsPerSignature.size; i ++) {
      if (irrepsPerSignature[i].size > 1) {
        mainTableStream << "$(";
        for (int j = 0; j < irrepsPerSignature[i].size; j ++) {
          mainTableStream << irrepsPerSignature[i][j];
          if (j != irrepsPerSignature[i].size - 1) {
            mainTableStream << ", ";
          }
        }
        mainTableStream << ")$ ";
      }
    }
    mainTableStream << "\n<br>\n";
  }
  // end of check for repeating signatures
  if (hasRepeatingPseudoParabolicSigs) {
    HashedList<List<Rational> > pseudoSigsNoRepetition;
    List<List<std::string> > irrepsPerSignature;
    pseudoSigsNoRepetition.addOnTopNoRepetition(pseudoSignSig);
    irrepsPerSignature.setSize(pseudoSigsNoRepetition.size);
    for (int i = 0; i < pseudoSignSig.size; i ++) {
      irrepsPerSignature[pseudoSigsNoRepetition.getIndex(pseudoSignSig[i])].
      addOnTop(this->irrepsCarterLabels[i]);
    }
    mainTableStream
    << "\n<br>\n\n<br>\nThe following families of "
    << "representations share the same pseudo-sign signature. ";
    for (int i = 0; i < irrepsPerSignature.size; i ++) {
      if (irrepsPerSignature[i].size > 1) {
        mainTableStream << "$(";
        for (int j = 0; j < irrepsPerSignature[i].size; j ++) {
          mainTableStream << irrepsPerSignature[i][j];
          if (j != irrepsPerSignature[i].size - 1) {
            mainTableStream << ", ";
          }
        }
        mainTableStream << ")$ ";
      }
    }
    mainTableStream << "\n<br>\n";
  }
  // end of check for repeating signatures
  int startIndex = 0;
  int numberOfColumnsPerPage = 25;
  int startIndexNextCol = 0;
  for (;;) {
    startIndex = startIndexNextCol;
    if (startIndex >= inputSubgroups.size) {
      break;
    }
    startIndexNextCol = startIndex + numberOfColumnsPerPage;
    if (startIndexNextCol - inputSubgroups.size > - 3) {
      startIndexNextCol = inputSubgroups.size;
    }
    mainTableStream << "\n<br>\n\\begin{longtable}{c|";
    for (int i = startIndex; i < startIndexNextCol; i ++) {
      if (i == numberOfParabolicClasses) {
        mainTableStream << "|";
      }
      if (i == numberOfParabolicClasses + numberOfNonParabolicPseudoParabolic) {
        mainTableStream << "|";
      }
      mainTableStream << "p{0.275cm}";
    }
    mainTableStream
    << "}\n<br>\n"
    << "\\caption{\\label{table:SignSignature"
    << HtmlRoutines::cleanUpForLaTeXLabelUse(this->dynkinType.toString())
    << "}Multiplicity of the sign representation "
    << "over the classes of root subgroups. "
    << "There are "
    << numberOfParabolicClasses
    << " parabolic subgroup classes, "
    << numberOfNonParabolicPseudoParabolic
    << " pseudo-parabolic subgroup classes that are not parabolic, and "
    << numberOfNonPseudoParabolic
    << " non-pseudo-parabolic subgroup classes. \n<br>\n"
    << "}\\\\ ";
    for (int i = startIndex; i < startIndexNextCol; i ++) {
      SubgroupDataRootReflections& currentSG = inputSubgroups[i];
      if (!currentSG.flagIsParabolic && !currentSG.flagIsExtendedParabolic) {
        mainTableStream << "&-";
        continue;
      }
      if (currentSG.flagIsParabolic) {
        mainTableStream << "&$\\mathfrak{p}_{" << i + 1 << "}$";
      }
      if (!currentSG.flagIsParabolic && currentSG.flagIsExtendedParabolic) {
        mainTableStream
        << "&${\\widehat{\\mathfrak{ p}}}_{"
        << i - numberOfParabolicClasses + 1
        << "}$";
      }
    }
    mainTableStream << "\\\\\n<br>\n";
    mainTableStream << "Irrep label";
    if (
      this->dynkinType.isSimple(
        &simpleType.letter,
        &simpleType.rank,
        &simpleType.cartanSymmetricInverseScale
      )
    ) {
      for (int i = startIndex; i < startIndexNextCol; i ++) {
        mainTableStream
        << "&$"
        << inputSubgroups[i].dynkinType.toString(&formatSupressUpperIndexOne)
        << "$";
      }
    } else {
      for (int i = startIndex; i < startIndexNextCol; i ++) {
        mainTableStream
        << "&$"
        << inputSubgroups[i].dynkinType.toString()
        << "$";
      }
    }
    for (int i = 0; i < this->group.conjugacyClassCount(); i ++) {
      mainTableStream << "\\\\";
      if (i == 0) {
        mainTableStream << "\\hline";
      }
      mainTableStream
      << "\n<br>\n$"
      << this->toStringIrreducibleRepresentationLabel(i)
      << "$";
      for (int j = startIndex; j < startIndexNextCol; j ++) {
        mainTableStream << "&" << inputSubgroups[j].tauSignature[i].toString();
      }
      mainTableStream << "\\\\\n<br>\n";
    }
    mainTableStream << "\\end{longtable}\n<br>\n";
  }
  for (int s = 0; s < 2; s ++) {
    out << "<table style =\"white-space: nowrap;\" border =\"1\">";
    Selection parabolicSelectionRootsAreOutOfLevi;
    out
    << "<tr><td>Irrep Label</td>"
    << "<td>Irreducible representation characters</td>";
    if (
      inputSubgroups[0].flagIsParabolic ||
      inputSubgroups[0].flagIsExtendedParabolic
    ) {
      for (int i = 0; i < inputSubgroups.size; i ++) {
        parabolicSelectionRootsAreOutOfLevi =
        inputSubgroups[i].simpleRootsInLeviParabolic;
        parabolicSelectionRootsAreOutOfLevi.invertSelection();
        out
        << "<td>"
        << parabolicSelectionRootsAreOutOfLevi.toString()
        << "</td>";
      }
      out << "</tr><tr><td></td><td></td>";
    }
    if (
      this->dynkinType.isSimple(
        &simpleType.letter,
        &simpleType.rank,
        &simpleType.cartanSymmetricInverseScale
      )
    ) {
      for (int i = 0; i < inputSubgroups.size; i ++) {
        out
        << "<td>"
        << inputSubgroups[i].dynkinType.toString(&formatSupressUpperIndexOne)
        << "</td>";
      }
    } else {
      for (int i = 0; i < inputSubgroups.size; i ++) {
        out << "<td>" << inputSubgroups[i].dynkinType.toString() << "</td>";
      }
    }
    out << "</tr>";
    for (int i = 0; i < this->group.conjugacyClassCount(); i ++) {
      out << "<tr>";
      if (i < this->irrepsCarterLabels.size) {
        out << "<td>" << this->irrepsCarterLabels[i] << "</td>";
      } else {
        out << "<td></td>";
      }
      out << "<td>" << this->group.characterTable[i].toString() << "</td>";
      if (s == 0) {
        for (int j = 0; j < inputSubgroups.size; j ++) {
          out
          << "<td>"
          << inputSubgroups[j].tauSignature[i].toString()
          << "</td>";
        }
      }
      if (s == 1) {
        for (int j = 0; j < pseudoSignSig[i].size; j ++) {
          out << "<td>" << pseudoSignSig[i][j].toString() << "</td>";
        }
      }
      out << "</tr>";
    }
    out << "</table>";
    out << "<br>";
    if (s == 0) {
      out << "Clipped tau signature follows.<br>";
    }
  }
  out
  << "<br>A version of the table in ready for "
  << "LaTeX consumption form follows.<br>\n<br>\n\n<br>\n\n<br>\n";
  out
  << "\\documentclass{article}"
  << "\\usepackage{amssymb}"
  << "\\usepackage{longtable}"
  << "\\usepackage{pdflscape}"
  << "\\addtolength{\\hoffset}{-3.5cm}\\addtolength{\\textwidth}{6.8cm}"
  << "\\addtolength{\\voffset}{-3.3cm}\\addtolength{\\textheight}{6.3cm}"
  << " \\begin{document}\\begin{landscape}\n<br>\n\n<br>\n\n<br>\n\n<br>\n";
  out << "{\\tiny \n<br>\n \\renewcommand{\\arraystretch}{0}%\n<br>\n";
  out << "\\begin{longtable}{r|";
  int numberOfIrreducibleRepresentations =
  this->group.characterTable[0].data.size;
  for (int i = 0; i < numberOfIrreducibleRepresentations; i ++) {
    out << "r";
  }
  out
  << "}"
  << "\\caption{\\label{tableIrrepChars"
  << this->dynkinType.toString()
  << "}\\\\ Irreducible representation characters. "
  << "Columns are labeled by conjugacy classes.} \\\\";
  out
  << "<br>\n  & \\multicolumn{"
  << this->group.characterTable[0].data.size
  << "}{c}{Conjugacy class $\\#$}\\\\<br>\n";
  out << "Irrep label";
  for (int i = 0; i < numberOfIrreducibleRepresentations; i ++) {
    out << "&$" << i + 1 << "$";
  }
  out << "\\\\\\hline<br>\n";
  for (int i = 0; i < this->group.characterTable.size; i ++) {
    out << "$" << this->toStringIrreducibleRepresentationLabel(i) << "$";
    for (int j = 0; j < this->group.characterTable[i].data.size; j ++) {
      out << "&$" << this->group.characterTable[i].data[j].toString() << "$";
    }
    out << "\\\\<br>\n";
  }
  out << "\\end{longtable}\n<br>\n";
  out
  << "\\begin{longtable}{rrcl}"
  << "\\caption{\\label{tableConjugacyClassTable"
  << HtmlRoutines::cleanUpForLaTeXLabelUse(this->dynkinType.toString())
  << "}}\\\\ ";
  out
  << "$\\#$ & Representative & Class size & Root subsystem label\\\\<br>\n";
  for (int i = 0; i < this->group.conjugacyClassCount(); i ++) {
    out
    << "$"
    << i + 1
    << "$ & "
    "$"
    << this->group.conjugacyClasses[i].representative.toString()
    << "$&$ "
    << this->group.conjugacyClasses[i].size.toString()
    << "$";
    if (i < this->ccCarterLabels.size) {
      out << "&$" << this->ccCarterLabels[i] << "$";
    }
    out << "\\\\<br>\n";
  }
  out << "\\end{longtable}\n<br>\n";
  out << mainTableStream.str();
  out
  << "}%arraystretch renewcommand scope\n"
  << "<br\n>\n<br>\n\n<br>\n\n<br>\n\n<br>\n";
  out << "\\end{landscape}\\end{document}";
  return out.str();
}

class KostkaNumber {
public:
  List<int> partition;
  List<int> tuple;
  int sumTuple;
  int sumPartition;
  int maximumCachedKostkaNumbers;
  LargeInteger value;
  bool initTableaux(std::stringstream* comments = nullptr);
  bool operator==(const KostkaNumber& other) const;
  static Rational computeTypeBParabolicSignMultiplicity(
    const Partition& parabolicPartition,
    const Partition& leftPartition,
    const Partition& rightPartition,
    std::stringstream* comments
  );
  static std::string GetTypeBParabolicSignMultiplicityTable(int rank);
  static unsigned int hashFunction(const KostkaNumber& input);
  std::string toString();
  KostkaNumber();
  bool compute(
    HashedList<KostkaNumber>* kostkaNumberCache,
    std::stringstream* comments = nullptr
  );
};

class SelectionFixedRankDifferentMaxMultiplicities {
public:
  List<int> multiplicities;
  List<int> capacities;
  int rank;
  bool flagFirstComputed;
  bool initialize();
  bool firstIncrement();
  bool incrementReturnFalseIfPastLast();
  SelectionFixedRankDifferentMaxMultiplicities();
  std::string toString();
  std::string toStringFull();
};

unsigned int KostkaNumber::hashFunction(const KostkaNumber& input) {
  return
  MathRoutines::hashListInts(input.partition) +
  MathRoutines::hashListInts(input.tuple);
}

bool KostkaNumber::operator==(const KostkaNumber& other) const {
  return this->partition == other.partition && this->tuple == other.tuple;
}

KostkaNumber::KostkaNumber() {
  this->maximumCachedKostkaNumbers = 10000;
}

std::string KostkaNumber::toString() {
  std::stringstream out;
  out << "KostkaNumber" << "((";
  for (int i = 0; i < this->partition.size; i ++) {
    out << this->partition[i];
    if (i != this->partition.size - 1) {
      out << ", ";
    }
  }
  out << "), (";
  for (int i = 0; i < this->tuple.size; i ++) {
    out << this->tuple[i];
    if (i != this->tuple.size - 1) {
      out << ", ";
    }
  }
  out << "))";
  return out.str();
}

bool KostkaNumber::initTableaux(std::stringstream* comments) {
  for (int i = 0; i < this->partition.size - 1; i ++) {
    if (this->partition[i] < this->partition[i + 1]) {
      if (comments != nullptr) {
        *comments
        << "Partition is supposed to be a "
        << "non-decreasing sequence of integers, instead it is: "
        << this->partition;
      }
      return false;
    }
  }
  this->sumTuple = 0;
  this->sumPartition = 0;
  for (int i = 0; i < this->tuple.size; i ++) {
    this->sumTuple += this->tuple[i];
    if (
      this->sumTuple > 10000000 ||
      this->tuple[i] > 10000000 ||
      this->tuple[i] < 0
    ) {
      if (comments != nullptr) {
        *comments
        << "Failed to compute Kostka number: the tuple "
        << this->tuple
        << " is too large or negative. ";
      }
      return false;
    }
  }
  for (int i = 0; i < this->partition.size; i ++) {
    this->sumPartition += this->partition[i];
    if (
      this->sumPartition > 10000000 ||
      this->partition[i] > 10000000 ||
      this->partition[i] < 0
    ) {
      if (comments != nullptr) {
        *comments
        << "Failed to compute Kostka number: the partition: "
        << this->partition
        << " is too large or negative. ";
      }
      return false;
    }
  }
  return true;
}

bool KostkaNumber::compute(
  HashedList<KostkaNumber>* kostkaNumberCache, std::stringstream* comments
) {
  STACK_TRACE("KostkaNumber::compute");
  this->value = - 1;
  if (!this->initTableaux(comments)) {
    return false;
  }
  if (this->sumTuple != this->sumPartition) {
    this->value = 0;
    return true;
  }
  if (this->sumTuple == 0) {
    if (this->sumPartition == 0) {
      this->value = 1;
    } else {
      this->value = 0;
    }
    return true;
  }
  SelectionFixedRankDifferentMaxMultiplicities selection;
  selection.initialize();
  selection.capacities.setSize(this->partition.size);
  for (int i = 0; i < this->partition.size; i ++) {
    if (i != this->partition.size - 1) {
      selection.capacities[i] = this->partition[i] - this->partition[i + 1];
    } else {
      selection.capacities[i] = this->partition[i];
    }
  }
  selection.rank = *this->tuple.lastObject();
  this->value = 0;
  while (selection.incrementReturnFalseIfPastLast()) {
    KostkaNumber ancestor;
    ancestor.partition = this->partition;
    ancestor.tuple = this->tuple;
    ancestor.tuple.setSize(ancestor.tuple.size - 1);
    for (int i = 0; i < selection.multiplicities.size; i ++) {
      ancestor.partition[i] -= selection.multiplicities[i];
    }
    if (kostkaNumberCache != nullptr) {
      int ancestorIndex = kostkaNumberCache->getIndex(ancestor);
      if (ancestorIndex != - 1) {
        ancestor = kostkaNumberCache->getElement(ancestorIndex);
      } else if (!ancestor.compute(kostkaNumberCache, comments)) {
        return false;
      } else {
        if (kostkaNumberCache->size < this->maximumCachedKostkaNumbers) {
          kostkaNumberCache->addOnTop(ancestor);
        }
      }
    } else {
      if (!ancestor.compute(kostkaNumberCache, comments)) {
        return false;
      }
    }
    this->value += ancestor.value;
  }
  return true;
}

std::string KostkaNumber::GetTypeBParabolicSignMultiplicityTable(int rank) {
  STACK_TRACE("KostkaNumber::GetTypeBParabolicSignMultiplicityTable");
  std::stringstream out;
  List<Pair<Partition, Partition> > partitionPairs;
  List<Partition> partitionsLeft;
  List<Partition> partitionsRight;
  List<Partition> partitionsParabolics;
  Pair<Partition, Partition> currentPartition;
  for (int i = 0; i <= rank; i ++) {
    Partition::getPartitions(partitionsLeft, i);
    Partition::getPartitions(partitionsRight, rank - i);
    partitionsParabolics.addListOnTop(partitionsRight);
    for (int j = 0; j < partitionsLeft.size; j ++) {
      for (int k = 0; k < partitionsRight.size; k ++) {
        currentPartition.object1 = partitionsLeft[j];
        currentPartition.object2 = partitionsRight[k];
        partitionPairs.addOnTop(currentPartition);
      }
    }
  }
  out << partitionPairs.size << " partition pairs. <br>";
  for (int i = 0; i < partitionPairs.size; i ++) {
    out
    << partitionPairs[i].object1.toString()
    << ","
    << partitionPairs[i].object2.toString()
    << "<br>";
  }
  partitionPairs.quickSortAscending();
  partitionsParabolics.quickSortAscending();
  Matrix<Rational> multiplicitiesTable;
  multiplicitiesTable.initialize(
    partitionPairs.size, partitionsParabolics.size
  );
  for (int j = 0; j < partitionPairs.size; j ++) {
    out
    << "V_{\\lambda, \\mu}, "
    << "<br>\\lambda ="
    << partitionPairs[j].object1.p
    << "<br>\\mu="
    << partitionPairs[j].object2.p;
    for (int i = 0; i < partitionsParabolics.size; i ++) {
      multiplicitiesTable(j, i) =
      KostkaNumber::computeTypeBParabolicSignMultiplicity(
        partitionsParabolics[i],
        partitionPairs[j].object1,
        partitionPairs[j].object2,
        &out
      );
    }
  }
  std::stringstream outLaTeX;
  out << "<table><tr><td></td>";
  outLaTeX << "\\begin{tabular}{c|";
  for (int i = 0; i < partitionsParabolics.size; i ++) {
    outLaTeX << "\n>{\\centering\\arraybackslash} p{1cm}";
  }
  outLaTeX << "}";
  for (int i = 0; i < partitionsParabolics.size; i ++) {
    std::stringstream parabolicStream;
    parabolicStream << "P_{";
    parabolicStream
    << partitionsParabolics[i].toStringForArticles("(", ")")
    << ", ";
    int typeBsize = rank - partitionsParabolics[i].n;
    if (typeBsize == 0) {
      parabolicStream << "\\emptyset";
    } else {
      parabolicStream << "(" << typeBsize << ")";
    }
    parabolicStream << "}";
    outLaTeX << "&$" << parabolicStream.str() << "$";
    out << "<td>" << parabolicStream.str();
    out << "</td>";
  }
  outLaTeX << "\\\\";
  out << "</tr>";
  // /////////////////////////////////////
  FormatExpressions format;
  format.flagSupressDynkinIndexOne = true;
  for (int i = 0; i < partitionsParabolics.size; i ++) {
    int typeBsize = rank - partitionsParabolics[i].n;
    DynkinType dynkinType;
    DynkinSimpleType simpleType;
    for (int j = 0; j < partitionsParabolics[i].p.size; j ++) {
      if (partitionsParabolics[i].p[j] <= 1) {
        continue;
      }
      simpleType.makeArbitrary('A', partitionsParabolics[i].p[j] - 1, 1);
      dynkinType.addMonomial(simpleType, 1);
    }
    if (typeBsize == 1) {
      simpleType.makeArbitrary('A', 1, 2);
      dynkinType.addMonomial(simpleType, 1);
    } else if (typeBsize > 1) {
      simpleType.makeArbitrary('B', typeBsize, 1);
      dynkinType.addMonomial(simpleType, 1);
    }
    outLaTeX << "&$" << dynkinType.toString(&format) << "$";
  }
  outLaTeX << "\\\\";
  // /////////////////////////////////////
  // /////////////////////////////////////
  for (int i = 0; i < partitionsParabolics.size; i ++) {
    int typeBsize = rank - partitionsParabolics[i].n;
    outLaTeX << "&$";
    for (int j = 0; j < partitionsParabolics[i].p.size; j ++) {
      if (j > 0) {
        outLaTeX << "\\times ";
      }
      outLaTeX << "S_{" << partitionsParabolics[i].p[j] << "}";
    }
    if (typeBsize > 0) {
      if (partitionsParabolics[i].n > 0) {
        outLaTeX << "\\times ";
      }
      outLaTeX << "B_{" << typeBsize << "}";
    }
    outLaTeX << "$";
  }
  outLaTeX << "\\\\";
  // /////////////////////////////////////
  for (int i = 0; i < partitionPairs.size; i ++) {
    std::stringstream Vstream;
    Vstream
    << "V_{"
    << partitionPairs[i].object1.toStringForArticles("[", "]")
    << ", "
    << partitionPairs[i].object2.toStringForArticles("[", "]")
    << "}";
    out << "<tr><td>";
    out << Vstream.str();
    out << "</td>";
    outLaTeX << "$" << Vstream.str() << "$";
    for (int j = 0; j < partitionsParabolics.size; j ++) {
      out << "<td>" << multiplicitiesTable(i, j) << "</td>";
      outLaTeX << "&" << "$" << multiplicitiesTable(i, j) << "$";
    }
    outLaTeX << "\\\\";
    out << "</tr>";
  }
  out << "</table>";
  outLaTeX << "\\end{tabular}";
  out << "<br>LaTeX'ed table follows. <br>";
  out << outLaTeX.str();
  return out.str();
}

Rational KostkaNumber::computeTypeBParabolicSignMultiplicity(
  const Partition& parabolicPartition,
  const Partition& leftPartition,
  const Partition& rightPartition,
  std::stringstream* comments
) {
  STACK_TRACE("KostkaNumber::computeTypeBParabolicSignMultiplicity");
  int rank = leftPartition.n + rightPartition.n;
  int bComponentSize = rank - parabolicPartition.n;
  if (comments != nullptr) {
    *comments
    << "<br>dim Hom_P(Sign P_{("
    << parabolicPartition.p
    << "), ("
    << bComponentSize
    << ")}, V_{"
    << leftPartition.p.toStringCommaDelimited()
    << ", "
    << rightPartition.p.toStringCommaDelimited()
    << "} )=";
  }
  Rational result = 0;
  SelectionWithDifferentMaxMultiplicities selection;
  List<int> complementSelection;
  HashedList<KostkaNumber> KNcache;
  KostkaNumber leftKostkaNumber;
  KostkaNumber rightKostkaNumber;
  Partition bufferPartition;
  bufferPartition = leftPartition;
  bufferPartition.transpose();
  leftKostkaNumber.partition = bufferPartition.p;
  bufferPartition = rightPartition;
  bufferPartition.transpose();
  rightKostkaNumber.partition = bufferPartition.p;
  selection.initializeFromIntegers(parabolicPartition.p);
  do {
    complementSelection.setSize(parabolicPartition.p.size);
    for (int k = 0; k < selection.capacities.size; k ++) {
      complementSelection[k] =
      parabolicPartition.p[k] - selection.multiplicities[k];
    }
    leftKostkaNumber.tuple = selection.multiplicities;
    leftKostkaNumber.compute(&KNcache, nullptr);
    rightKostkaNumber.tuple = complementSelection;
    rightKostkaNumber.tuple.addOnTop(bComponentSize);
    rightKostkaNumber.compute(&KNcache, nullptr);
    result += leftKostkaNumber.value * rightKostkaNumber.value;
  } while (selection.incrementReturnFalseIfPastLast());
  if (comments != nullptr) {
    *comments << result.toString();
  }
  return result;
}

std::string SelectionFixedRankDifferentMaxMultiplicities::toString() {
  std::stringstream out;
  Vector<int> multiplicitiesVector;
  multiplicitiesVector = this->multiplicities;
  out << multiplicitiesVector;
  return out.str();
}

std::string SelectionFixedRankDifferentMaxMultiplicities::toStringFull() {
  std::stringstream out;
  Vector<int> multiplicitiesVector;
  Vector<int> maximulMultiplicities;
  maximulMultiplicities = this->capacities;
  multiplicitiesVector = this->multiplicities;
  out
  << "multiplicities: "
  << multiplicitiesVector
  << "; max: "
  << maximulMultiplicities
  << "; rank: "
  << this->rank;
  return out.str();
}

SelectionFixedRankDifferentMaxMultiplicities::
SelectionFixedRankDifferentMaxMultiplicities() {
  this->flagFirstComputed = false;
}

bool SelectionFixedRankDifferentMaxMultiplicities::initialize() {
  this->flagFirstComputed = false;
  return true;
}

bool SelectionFixedRankDifferentMaxMultiplicities::firstIncrement() {
  STACK_TRACE("SelectionFixedRankDifferentMaxMultiplicities::firstIncrement");
  this->flagFirstComputed = true;
  this->multiplicities.setSize(this->capacities.size);
  int remainingRank = this->rank;
  for (int i = this->capacities.size - 1; i >= 0; i --) {
    if (this->capacities[i] < remainingRank) {
      this->multiplicities[i] = this->capacities[i];
    } else {
      this->multiplicities[i] = remainingRank;
    }
    remainingRank -= this->multiplicities[i];
  }
  if (remainingRank > 0) {
    return false;
  }
  return true;
}

bool SelectionFixedRankDifferentMaxMultiplicities::
incrementReturnFalseIfPastLast() {
  STACK_TRACE(
    "SelectionFixedRankDifferentMaxMultiplicities::"
    "incrementReturnFalseIfPastLast"
  );
  if (this->rank < 0) {
    return false;
  }
  if (!this->flagFirstComputed) {
    return this->firstIncrement();
  }
  int rankToRedistribute = 0;
  for (int i = this->multiplicities.size - 2; i >= 0; i --) {
    rankToRedistribute += this->multiplicities[i + 1];
    this->multiplicities[i + 1] = 0;
    if (
      this->multiplicities[i] < this->capacities[i] && rankToRedistribute > 0
    ) {
      this->multiplicities[i] ++;
      rankToRedistribute --;
      for (int j = this->multiplicities.size - 1; j > i; j --) {
        if (this->capacities[j] <= rankToRedistribute) {
          this->multiplicities[j] = this->capacities[j];
        } else {
          this->multiplicities[j] = rankToRedistribute;
        }
        rankToRedistribute -= this->multiplicities[j];
        if (rankToRedistribute == 0) {
          return true;
        }
      }
      return true;
    }
  }
  return false;
}

bool CalculatorFunctionsWeylGroup::kostkaNumber(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::kostkaNumber");
  if (input.size() != 3) {
    return false;
  }
  KostkaNumber computer;
  if (
    !calculator.getVectorInt(input[1], computer.partition) ||
    !calculator.getVectorInt(input[2], computer.tuple)
  ) {
    return
    calculator
    << "Failed to extract partition and tuple from input: "
    << input.toString();
  }
  std::stringstream out;
  HashedList<KostkaNumber> allKostkaNumbers;
  if (computer.compute(&allKostkaNumbers, &out)) {
    out << "<br>Final result: " << computer.value.toString();
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsWeylGroup::allSelectionsFixedRank(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsWeylGroup::allSelectionsFixedRank");
  if (input.size() != 3) {
    return false;
  }
  SelectionFixedRankDifferentMaxMultiplicities selection;
  if (!input[1].isSmallInteger(&selection.rank)) {
    return false;
  }
  if (!calculator.getVectorInt(input[2], selection.capacities)) {
    return
    calculator
    << "Failed to extract list of multiplicities from "
    << input[2].toString();
  }
  if (selection.rank < 0) {
    return output.assignValue(calculator, 0);
  }
  selection.initialize();
  std::stringstream out;
  out
  << "Max multiplicities: "
  << selection.capacities
  << " rank: "
  << selection.rank;
  while (selection.incrementReturnFalseIfPastLast()) {
    out << "<br>" << selection.toString();
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsWeylGroup::signSignatureRootSubsystemsFromKostkaNumbers
(Calculator& calculator, const Expression& input, Expression& output) {
  STACK_TRACE(
    "CalculatorFunctionsWeylGroup::"
    "signSignatureRootSubsystemsFromKostkaNumbers"
  );
  std::stringstream out;
  if (!CalculatorConversions::loadWeylGroup(calculator, input, output)) {
    return false;
  }
  if (!output.isOfType<WeylGroupData>()) {
    return false;
  }
  WeylGroupData& weyl = output.getValueNonConst<WeylGroupData>();
  if (weyl.getDimension() > 12) {
    return
    calculator
    << "<hr>Computing sign signatures restricted up to rank 12.";
  }
  char type = 'X';
  int rank = - 1;
  if (!weyl.dynkinType.isSimple(&type, &rank)) {
    return
    calculator
    << "This function is implemented for simple classical Weyl groups only.";
  }
  if (type != 'A' && type != 'B' && type != 'C' && type != 'D') {
    return
    calculator
    << "You requested computation for type "
    << type
    << " but our formulas work only for classical types: A, B-C and D. ";
  }
  if (type == 'B' || type == 'C') {
    out << KostkaNumber::GetTypeBParabolicSignMultiplicityTable(rank);
  }
  if (type == 'A') {
    if ((false)) {
      int permutationSize = rank + 1;
      List<Partition> partitions, partitionsTransposed;
      Partition::getPartitions(partitions, permutationSize);
      partitionsTransposed.setSize(partitions.size);
      for (int i = 0; i < partitions.size; i ++) {
        partitionsTransposed[i] = partitions[i];
        partitionsTransposed[i].transpose();
        out
        << "<br>Partition: "
        << partitions[i].toString()
        << ", transposed: "
        << partitionsTransposed[i].toString();
      }
    }
    return calculator << "Not implemented yet.";
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsWeylGroup::signSignatureRootSubsystems(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsWeylGroup::signSignatureRootSubsystems");
  if (!CalculatorConversions::loadWeylGroup(calculator, input, output)) {
    return false;
  }
  if (!output.isOfType<WeylGroupData>()) {
    return false;
  }
  WeylGroupData& weyl = output.getValueNonConst<WeylGroupData>();
  if (weyl.getDimension() > 8) {
    calculator << "<hr>Computing sign signatures restricted up to rank 8.";
    return false;
  }
  std::stringstream out;
  List<SubgroupDataRootReflections> parabolicSubgroupS;
  List<SubgroupDataRootReflections> extendedParabolicSubgroups;
  List<SubgroupDataRootReflections> allRootSubgroups;
  List<SubgroupDataRootReflections> finalSubGroups;
  if (!weyl.loadSignSignatures(finalSubGroups)) {
    weyl.getSignSignatureParabolics(parabolicSubgroupS);
    weyl.getSignSignatureExtendedParabolics(extendedParabolicSubgroups);
    weyl.getSignSignatureAllRootSubsystems(allRootSubgroups);
    List<Pair<std::string, List<Rational> > > tauSigPairs;
    finalSubGroups.reserve(allRootSubgroups.size);
    Pair<std::string, List<Rational> > currentTauSig;
    for (int j = 0; j < 3; j ++) {
      List<SubgroupDataRootReflections>* currentSGs = nullptr;
      if (j == 0) {
        currentSGs = &parabolicSubgroupS;
      }
      if (j == 1) {
        currentSGs = &extendedParabolicSubgroups;
      }
      if (j == 2) {
        currentSGs = &allRootSubgroups;
      }
      for (int i = 0; i < currentSGs->size; i ++) {
        currentTauSig.object1 = (*currentSGs)[i].dynkinType.toString();
        currentTauSig.object2 = (*currentSGs)[i].tauSignature;
        if (!tauSigPairs.contains(currentTauSig)) {
          tauSigPairs.addOnTop(currentTauSig);
          finalSubGroups.addOnTop((*currentSGs)[i]);
        }
      }
    }
  }
  out << weyl.toStringSignSignatureRootSubsystem(finalSubGroups);
  return output.assignValue(calculator, out.str());
}

template < >
GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& Expression::
getValueNonConst() const;

bool CalculatorFunctionsWeylGroup::decomposeWeylRep(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsWeylGroup::decomposeWeylRep");
  if (
    !input.isOfType<
      GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
    >()
  ) {
    return false;
  }
  GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>&
  inputRepresentation =
  input.getValueNonConst<
    GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
  >();
  VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
  outputRepresentation;
  inputRepresentation.makeGRCAM().decomposeTodorsVersion(
    outputRepresentation
  );
  return output.assignValue(calculator, outputRepresentation);
}

bool CalculatorFunctionsWeylGroup::isOuterAutoWeylGroup(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsWeylGroup::isOuterAutoWeylGroup");
  if (input.size() != 3) {
    return calculator << "<hr>IsOuterAuto expects 2 arguments.";
  }
  DynkinType dynkinType;
  if (
    !CalculatorConversions::functionDynkinType(
      calculator, input[1], dynkinType
    )
  ) {
    return
    calculator
    << "<hr>Failed to get Dynkin type from argument. "
    << input[1].toString();
  }
  Matrix<Rational> matrix;
  if (
    !CalculatorConversions::functionGetMatrixNoComputation(
      calculator, input[2], matrix
    )
  ) {
    return
    calculator
    << "<hr>Failed to get matrix from argument. "
    << input[2].toString();
  }
  if (
    matrix.numberOfColumns != matrix.numberOfRows ||
    matrix.numberOfColumns != dynkinType.getRank()
  ) {
    calculator
    << "<hr>Extracted Dynkin type "
    << dynkinType.toString()
    << " is of rank "
    << dynkinType.getRank()
    << " but extracted linear operator has "
    << matrix.numberOfColumns
    << " columns and "
    << matrix.numberOfRows
    << " rows.";
    return false;
  }
  WeylGroupData weyl;
  weyl.makeFromDynkinType(dynkinType);
  weyl.computeRho(true);
  WeylGroupAutomorphisms automorphisms;
  automorphisms.weylGroup = &weyl;
  MatrixTensor<Rational> operatorTensorForm;
  operatorTensorForm = matrix;
  if (
    automorphisms.isElementWeylGroupOrOuterAutomorphisms(operatorTensorForm)
  ) {
    return output.assignValue(calculator, 1);
  }
  return output.assignValue(calculator, 0);
}

bool CalculatorFunctionsWeylGroup::weylGroupNaturalRep(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsWeylGroup::weylGroupNaturalRep");
  if (
    !CalculatorFunctionsWeylGroup::weylGroupConjugacyClasseS(
      calculator, input, output
    )
  ) {
    return false;
  }
  if (!output.isOfType<WeylGroupData>()) {
    return false;
  }
  WeylGroupData& weylGroup = output.getValueNonConst<WeylGroupData>();
  weylGroup.computeInitialIrreducibleRepresentations();
  weylGroup.group.computeIrreducibleRepresentationsTodorsVersion();
  GroupRepresentationCarriesAllMatrices<
    FiniteGroup<ElementWeylGroup>, Rational
  > representation;
  weylGroup.getStandardRepresentation(representation);
  return
  output.assignValue(
    calculator, representation.makeOtherGroupRepresentationClass()
  );
}

class MonomialMacdonald {
public:
  SemisimpleLieAlgebra* owner;
  Selection rootSelection;
  bool flagDeallocated;
  MonomialMacdonald(): owner(nullptr), flagDeallocated(false) {}
  std::string toString(FormatExpressions* format = nullptr) const;
  bool checkConsistency() {
    if (this->flagDeallocated) {
      global.fatal << "Use after free of MonomialMacdonald. " << global.fatal;
      return false;
    }
    return true;
  }
  static unsigned int hashFunction(const MonomialMacdonald& input) {
    return input.rootSelection.hashFunction();
  }
  void operator=(const MonomialMacdonald& other) {
    this->owner = other.owner;
    this->rootSelection = other.rootSelection;
    this->flagDeallocated = other.flagDeallocated;
  }
  bool operator==(const MonomialMacdonald& other) const;
  void makeFromRootSubsystem(
    const Vectors<Rational>& inputRoots, SemisimpleLieAlgebra& inputOwner
  );
  void actOnMeSimpleReflection(
    int indexSimpleReflection, Rational& outputMultiple
  );
  void generateMyOrbit(HashedList<MonomialMacdonald>& output);
  ~MonomialMacdonald();
};

std::string MonomialMacdonald::toString(FormatExpressions* format) const {
  STACK_TRACE("MonomialMacdonald::toString");
  (void) format;
  if (this->owner == nullptr) {
    return "(non-initialized)";
  }
  if (this->rootSelection.cardinalitySelection == 0) {
    return "1";
  }
  std::stringstream out;
  for (int i = 0; i < this->rootSelection.cardinalitySelection; i ++) {
    out << "a_" << this->rootSelection.elements[i] << "";
  }
  return out.str();
}

bool MonomialMacdonald::operator==(const MonomialMacdonald& other) const {
  return
  this->owner == other.owner &&
  this->rootSelection == other.rootSelection;
}

MonomialMacdonald::~MonomialMacdonald() {
  this->flagDeallocated = true;
}

void MonomialMacdonald::generateMyOrbit(
  HashedList<MonomialMacdonald>& output
) {
  STACK_TRACE("MonomialMacdonald::generateMyOrbit");
  output.clear();
  output.addOnTop(*this);
  MonomialMacdonald monomialMacdonald;
  Rational coefficient;
  for (int i = 0; i < output.size; i ++) {
    for (int j = 0; j < this->owner->getRank(); j ++) {
      monomialMacdonald = output[i];
      monomialMacdonald.actOnMeSimpleReflection(j, coefficient);
      output.addOnTopNoRepetition(monomialMacdonald);
    }
  }
}

void MonomialMacdonald::makeFromRootSubsystem(
  const Vectors<Rational>& inputRoots, SemisimpleLieAlgebra& inputOwner
) {
  STACK_TRACE("MonomialMacdonald::makeFromRootSubsystem");
  this->owner = &inputOwner;
  this->rootSelection.initialize(inputOwner.weylGroup.rootSystem.size);
  Vector<Rational> currentVector;
  for (int i = 0; i < inputRoots.size; i ++) {
    currentVector = inputRoots[i];
    if (currentVector.isNegative()) {
      currentVector *= - 1;
    }
    int indexInRoots =
    inputOwner.weylGroup.rootSystem.getIndex(currentVector);
    if (indexInRoots < 0) {
      global.fatal
      << "Attempt to make a Macdonald polynomial from "
      << inputRoots.toString()
      << ": the vector "
      << currentVector.toString()
      << " is not a root. "
      << global.fatal;
    }
    this->rootSelection.addSelectionAppendNewIndex(indexInRoots);
  }
  this->rootSelection.computeIndicesFromSelection();
}

void MonomialMacdonald::actOnMeSimpleReflection(
  int indexSimpleReflection, Rational& outputMultiple
) {
  Selection originalSel;
  originalSel = this->rootSelection;
  this->rootSelection.initialize(this->owner->weylGroup.rootSystem.size);
  Vector<Rational> currentVector;
  outputMultiple = 1;
  for (int i = 0; i < originalSel.cardinalitySelection; i ++) {
    currentVector =
    this->owner->weylGroup.rootSystem[originalSel.elements[i]];
    this->owner->weylGroup.reflectSimple(indexSimpleReflection, currentVector);
    if (currentVector.isNegative()) {
      currentVector *= - 1;
      outputMultiple *= - 1;
    }
    this->rootSelection.addSelectionAppendNewIndex(
      this->owner->weylGroup.rootSystem.getIndex(currentVector)
    );
  }
}

bool CalculatorFunctionsWeylGroup::macdonaldPolynomials(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsWeylGroup::macdonaldPolynomials");
  // note that if input is list of 2 elements then input[0] is sequence atom,
  // and your two elements are in fact
  // input[1] and input[2];
  if (input.size() != 2) {
    return
    calculator
    << "Macdonald polynomials expects as input a single argument. ";
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!CalculatorConversions::convert(calculator, input[1], algebra)) {
    return output.assignError(calculator, "Error extracting Lie algebra.");
  }
  RootSubalgebras rootSubalgebras;
  rootSubalgebras.owner = algebra.content;
  rootSubalgebras.computeAllReductiveRootSubalgebrasUpToIsomorphism();
  std::stringstream out;
  MonomialMacdonald generator;
  HashedList<MonomialMacdonald> orbit;
  for (int i = 0; i < rootSubalgebras.subalgebras.size; i ++) {
    RootSubalgebra& currentRootSubalgebra = rootSubalgebras.subalgebras[i];
    generator.makeFromRootSubsystem(
      currentRootSubalgebra.positiveRootsReductiveSubalgebra, *algebra.content
    );
    generator.generateMyOrbit(orbit);
    out
    << "<hr>Root subsystem type "
    << currentRootSubalgebra.dynkinDiagram.toString();
    out << ". Orbit has " << orbit.size << " element(s), here they are: ";
    for (int j = 0; j < orbit.size; j ++) {
      out << "<br>" << orbit[j].toString();
    }
  }
  out
  << "Type: "
  << rootSubalgebras.owner->weylGroup.dynkinType.toString()
  << ". Number of root subsystems: "
  << rootSubalgebras.subalgebras.size;
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsWeylGroup::lieAlgebraWeight(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsWeylGroup::lieAlgebraWeight");
  Weight<Polynomial<Rational> > resultWeight;
  if (input.size() != 4) {
    return false;
  }
  Expression expression;
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebraWithContext;
  semisimpleLieAlgebraWithContext.content = nullptr;
  if (
    !CalculatorConversions::functionSemisimpleLieAlgebraFromDynkinType(
      calculator, input[1], expression, semisimpleLieAlgebraWithContext
    )
  ) {
    return calculator << "<hr>Failed to load semisimple Lie algebra";
  }
  SemisimpleLieAlgebra* semisimpleLieAlgebra =
  semisimpleLieAlgebraWithContext.content;
  std::string coordinatesString;
  bool isGood = input[3].isOperation(&coordinatesString);
  if (isGood) {
    isGood = (coordinatesString == "epsilon") || (
      coordinatesString == "fundamental"
    ) || (coordinatesString == "simple");
  }
  if (!isGood) {
    return
    calculator
    << "<hr>The third argument of MakeWeight is bad: must be one of the "
    << "keywords: epsilon, fundamental, simple. ";
  }
  int weightIndex = - 1;
  if (!input[2].isSmallInteger(&weightIndex)) {
    return false;
  }
  if (coordinatesString != "epsilon") {
    if (weightIndex < 1 || weightIndex > semisimpleLieAlgebra->getRank()) {
      std::stringstream errorStream;
      errorStream
      << "The second argument of the MakeWeight function "
      << "needs to be index of a weight between 1 and the Lie algebra rank. "
      << "However, the index is "
      << weightIndex
      << ".";
      return output.assignError(calculator, errorStream.str());
    }
    Vector<Polynomial<Rational> > eiVector;
    eiVector.makeEi(semisimpleLieAlgebra->getRank(), weightIndex - 1);
    if (coordinatesString == "fundamental") {
      resultWeight.weightFundamentalCoordinates = eiVector;
    } else if (coordinatesString == "simple") {
      resultWeight.weightFundamentalCoordinates =
      semisimpleLieAlgebra->weylGroup.getFundamentalCoordinatesFromSimple(
        eiVector
      );
    }
  } else {
    Vector<Rational> eiVector;
    eiVector.makeZero(semisimpleLieAlgebra->getRank());
    Vector<Rational> currentVector =
    semisimpleLieAlgebra->weylGroup.getEpsilonCoordinates(eiVector);
    if (weightIndex > currentVector.size || weightIndex < 1) {
      std::stringstream errorStream;
      errorStream
      << "The second argument of the MakeWeight "
      << "function needs to be index of a weight between 1 and "
      << currentVector.size
      << ". However, the index is "
      << weightIndex
      << ".";
      return output.assignError(calculator, errorStream.str());
    }
    eiVector.makeEi(currentVector.size, weightIndex - 1);
    resultWeight.weightFundamentalCoordinates =
    semisimpleLieAlgebra->weylGroup.getFundamentalCoordinatesFromEpsilon(
      eiVector
    );
  }
  resultWeight.owner = semisimpleLieAlgebra;
  ExpressionContext context(calculator);
  context.setAmbientSemisimpleLieAlgebra(*semisimpleLieAlgebra);
  return output.assignValueWithContext(calculator, resultWeight, context);
}

bool CalculatorFunctionsWeylGroup::lieAlgebraRhoWeight(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsWeylGroup::lieAlgebraRhoWeight");
  if (input.size() != 2) {
    return calculator << "Lie algebra rho weight expects a single argument. ";
  }
  Weight<Polynomial<Rational> > resultWeight;
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebraWithContext;
  semisimpleLieAlgebraWithContext.content = nullptr;
  Expression expression;
  if (
    !CalculatorConversions::functionSemisimpleLieAlgebraFromDynkinType(
      calculator, input[1], expression, semisimpleLieAlgebraWithContext
    )
  ) {
    return calculator << "<hr>Failed to load semisimple Lie algebra. ";
  }
  SemisimpleLieAlgebra* semisimpleLieAlgebra =
  semisimpleLieAlgebraWithContext.content;
  semisimpleLieAlgebra->checkConsistency();
  ExpressionContext context(calculator);
  context.setAmbientSemisimpleLieAlgebra(*semisimpleLieAlgebra);
  resultWeight.weightFundamentalCoordinates =
  semisimpleLieAlgebra->weylGroup.getFundamentalCoordinatesFromSimple(
    semisimpleLieAlgebra->weylGroup.rho
  );
  resultWeight.owner = semisimpleLieAlgebra;
  return output.assignValueWithContext(calculator, resultWeight, context);
}

bool CalculatorFunctionsWeylGroup::testSpechtModules(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsWeylGroup::testSpechtModules");
  int symmetricGroupRank = 0;
  if (!input.isSmallInteger(&symmetricGroupRank)) {
    return
    calculator
    << "testSpechtModules called with input "
    << "that is not a small integer, not performing any tests.";
  }
  if (symmetricGroupRank < 1) {
    return
    calculator
    << "testSpechtModules takes as input a small "
    << "positive integer, instead the input I got was "
    << symmetricGroupRank
    << ". ";
  }
  if (symmetricGroupRank > 6) {
    return
    calculator
    << "For speed/memory reasons, testSpechtModules is "
    << "currently restricted to take input no larger than 6. "
    << "Your input was: "
    << symmetricGroupRank;
  }
  std::stringstream out;
  out
  << "User has requested the test of Specht modules of S_"
  << symmetricGroupRank
  << ". ";
  Partition::testAllSpechtModules(symmetricGroupRank);
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsWeylGroup::representElementHyperOctahedral(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsWeylGroup::representElementHyperOctahedral"
  );
  if (input.size() != 3) {
    return calculator << "Representating takes 2 arguments: element and rep.";
  }
  ElementHyperoctahedralGroupR2 element;
  GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational>
  representation;
  if (!input[1].isOfType(&element)) {
    return
    calculator
    << "Failed to extract element of hyperoctahedral group from "
    << input[1].toString();
  }
  if (!input[2].isOfType(&representation)) {
    return
    calculator
    << "Failed to extract representation from "
    << input[2].toString();
  }
  Matrix<Rational> result;
  if (!representation.getMatrixOfElement(element, result)) {
    return
    calculator
    << "Failed to get matrix of element "
    << element.toString()
    << " from representation: "
    << representation.toString();
  }
  return output.makeMatrix(calculator, result, nullptr, false);
}

bool CalculatorFunctionsWeylGroup::hyperOctahedralGetOneRepresentation(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsWeylGroup::hyperOctahedralGetOneRepresentation"
  );
  Vector<Rational> inputLeftRational;
  Vector<Rational> inputRightRational;
  if (input.size() != 3) {
    return
    calculator
    << "CalculatorFunctionsWeylGroup::hyperOctahedralGetOneRepresentation "
    << "needs two arguments";
  }
  if (
    !calculator.getVector(input[1], inputLeftRational) ||
    !calculator.getVector(input[2], inputRightRational)
  ) {
    return false;
  }
  if (inputLeftRational.size < 1 || inputRightRational.size < 1) {
    return false;
  }
  const int maxPartitionSize = 1000;
  Vector<int> inputLeft;
  Vector<int> inputRight;
  Vector<int>* currentInput = &inputLeft;
  Vector<Rational>* currentInputRat = &inputLeftRational;
  for (
    int inputCounter = 0; inputCounter < 2;
    inputCounter ++,
    currentInput = &inputRight,
    currentInputRat = &inputRightRational
  ) {
    currentInput->setSize(currentInputRat->size);
    for (int i = 0; i < currentInputRat->size; i ++) {
      if (!(*currentInputRat)[i].isIntegerFittingInInt(&(*currentInput)[i])) {
        return
        calculator
        << "Failed to convert input: "
        << input.toString()
        << " to a list of small integers.";
      }
    }
    for (int i = 0; i < currentInput->size; i ++) {
      if ((*currentInput)[i] < 1 || (*currentInput)[i] > maxPartitionSize) {
        return
        calculator
        << "Entry: "
        << (*currentInput)[i]
        << " of "
        << (*currentInput)
        << " is outside of the allowed input range.";
      }
    }
    if (currentInput->sumCoordinates() > maxPartitionSize) {
      return
      calculator
      << "The coordinates of vector "
      << (*currentInput)
      << " have sum that is too large. ";
    }
  }
  Partition partitionLeft;
  Partition partitionRight;
  partitionLeft.fromListInt(inputLeft);
  partitionRight.fromListInt(inputRight);
  int index = 0;
  for (
    ; index < calculator.objectContainer.hyperOctahedralGroups.size; index ++
  ) {
    if (
      calculator.objectContainer.hyperOctahedralGroups[index].
      flagIsEntireHyperoctahedralGroup &&
      calculator.objectContainer.hyperOctahedralGroups[index].dimension ==
      partitionLeft.n + partitionRight.n
    ) {
      break;
    }
  }
  if (index == calculator.objectContainer.hyperOctahedralGroups.size) {
    calculator.objectContainer.hyperOctahedralGroups.setSize(
      calculator.objectContainer.hyperOctahedralGroups.size + 1
    );
    calculator.objectContainer.hyperOctahedralGroups[index].
    makeHyperoctahedralGroup(partitionLeft.n + partitionRight.n);
  }
  // <-May be broken if copying of groups doesn't work as advertised.
  HyperoctahedralGroupData& HD =
  calculator.objectContainer.hyperOctahedralGroups[index];
  GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> R;
  HD.spechtModuleOfPartititons(partitionLeft, partitionRight, R);
  // out << R;
  R.computeCharacter();
  return output.assignValue(calculator, R);
}

bool CalculatorFunctionsWeylGroup::spechtModule(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsWeylGroup::spechtModule");
  Vector<Rational> inputRational;
  if (!calculator.getVectorFromFunctionArguments(input, inputRational)) {
    return false;
  }
  if (inputRational.size < 1) {
    return false;
  }
  const int maxPartitionSize = 1000;
  Vector<int> inputInt;
  inputInt.setSize(inputRational.size);
  for (int i = 0; i < inputRational.size; i ++) {
    if (!(inputRational[i].isIntegerFittingInInt(&inputInt[i]))) {
      return
      calculator
      << "Failed to convert input: "
      << input.toString()
      << " to a list of small integers.";
    }
  }
  for (int i = 0; i < inputInt.size; i ++) {
    if ((inputInt[i] < 1) || (inputInt[i] > maxPartitionSize)) {
      return
      calculator
      << "Entry: "
      << inputInt[i]
      << " of "
      << inputInt
      << " is outside of the allowed input range.";
    }
  }
  if (inputInt.sumCoordinates() > maxPartitionSize) {
    return
    calculator
    << "The coordinates of vector "
    << inputInt
    << " have sum that is too large. ";
  }
  Partition p;
  p.fromListInt(inputInt);
  std::stringstream out;
  out << "Partition is " << p.toString();
  List<Matrix<Rational> > generators;
  p.spechtModuleMatricesOfTranspositionsjjplusone(generators);
  for (int i = 0; i < generators.size; i ++) {
    out << i << "\n" << generators[i].toStringPlainText() << "\n";
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsWeylGroup::
hyperOctahedralAllModulesInducedFromSpechtModules(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsWeylGroup::"
    "hyperOctahedralAllModulesInducedFromSpechtModules"
  );
  if (input.size() != 2) {
    return false;
  }
  int rank = 0;
  if (!input[1].isSmallInteger(&rank)) {
    return false;
  }
  if (rank < 1 || rank > 7) {
    return
    calculator
    << "Input of hyperoctahedral print function has to be between 1 and 10";
  }
  HyperoctahedralGroupData groupData;
  groupData.makeHyperoctahedralGroup(rank);
  groupData.allSpechtModules();
  return
  output.assignValue(calculator, groupData.group->prettyPrintCharacterTable());
}

bool CalculatorFunctionsWeylGroup::
hyperOctahedralPrintGeneratorCommutationRelations(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsWeylGroup::"
    "hyperOctahedralPrintGeneratorCommutationRelations"
  );
  if (input.size() != 2) {
    return false;
  }
  int rank = 0;
  if (!input[1].isSmallInteger(&rank)) {
    return false;
  }
  if (rank < 1 || rank > 10) {
    return
    calculator
    << "Input of hyperoctahedral print function has to be between 1 and 10";
  }
  HyperoctahedralGroupData groupData;
  groupData.makeHyperoctahedralGroup(rank);
  return
  output.assignValue(
    calculator, groupData.group->prettyPrintGeneratorCommutationRelations()
  );
}

bool CalculatorFunctionsWeylGroup::weylGroupElement(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsWeylGroup::weylGroupElement");
  if (input.size() < 2) {
    return
    output.assignError(
      calculator,
      "Function WeylElement needs to know what group the element belongs to"
    );
  }
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebraPointer;
  if (
    !CalculatorConversions::convert(
      calculator, input[1], semisimpleLieAlgebraPointer
    )
  ) {
    return output.assignError(calculator, "Error extracting Lie algebra.");
  }
  ElementWeylGroup element;
  element.generatorsLastAppliedFirst.reserve(input.size() - 2);
  for (int i = 2; i < input.size(); i ++) {
    int smallInt;
    if (!input[i].isSmallInteger(&smallInt)) {
      return false;
    }
    element.multiplyOnTheRightBySimpleReflection(smallInt - 1);
  }
  element.owner = &semisimpleLieAlgebraPointer.content->weylGroup;
  for (int i = 0; i < element.generatorsLastAppliedFirst.size; i ++) {
    if (
      element.generatorsLastAppliedFirst[i].index >=
      semisimpleLieAlgebraPointer.content->getRank() ||
      element.generatorsLastAppliedFirst[i].index < 0
    ) {
      return output.assignError(calculator, "Bad reflection index");
    }
  }
  element.makeCanonical();
  return output.assignValue(calculator, element);
}

template <typename somegroup, typename Coefficient>
void VirtualRepresentation<somegroup, Coefficient>::operator*=(
  const VirtualRepresentation<somegroup, Coefficient>& other
) {
  STACK_TRACE("VirtualRepresentation::operator*=");
  (void) other;
  VirtualRepresentation<somegroup, Coefficient> output, currentContribution;
  output.makeZero();
  for (int i = 0; i < this->size(); i ++) {
    for (int j = 0; j < other.size(); j ++) {
      Rational coefficient = this->coefficients[i] * other.coefficients[j];
      if (coefficient == 0) {
        continue;
      }
      ClassFunction<somegroup, Coefficient> productNonDecomposed;
      productNonDecomposed = this->monomials[i] * other.monomials[j];
      global.fatal << "Must decompose representation" << global.fatal;
      // TODO(tmilev): make this work again.
      // productNonDecomposed.decomposeTodorsVersion(currentContribution, 0);
      output += currentContribution * coefficient;
    }
  }
  *this = output;
}

template <typename somegroup, typename Coefficient>
void VirtualRepresentation<somegroup, Coefficient>::assignRepresentation(
  const GroupRepresentationCarriesAllMatrices<somegroup, Rational>& other
) {
  global.fatal << " not implemented " << global.fatal;
  GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> otherCopy;
  otherCopy = other;
  //  otherCopy.decomposeTodorsVersion(this->coefficientsIrreps, global);
}

template <typename somegroup, typename Coefficient>
void VirtualRepresentation<somegroup, Coefficient>::assignRepresentation(
  const GroupRepresentation<somegroup, Rational>& other
) {
  VirtualRepresentation<somegroup, Coefficient> out;
  out.addMonomial(other.character, 1);
  //  otherCopy.decomposeTodorsVersion(this->coefficientsIrreps, global);
}

bool CalculatorFunctionsWeylGroup::makeVirtualWeylRepresentation(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsWeylGroup::makeVirtualWeylRepresentation");
  if (
    input.isOfType<
      VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
    >()
  ) {
    output = input;
    return true;
  }
  if (
    !input.isOfType<
      GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
    >()
  ) {
    return false;
  }
  GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>&
  inputRepresentation =
  input.getValueNonConst<
    GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
  >();
  WeylGroupData* weylData =
  inputRepresentation.ownerGroup->generators[0].owner;
  if (
    inputRepresentation.ownerGroup->irreducibleRepresentations.size <
    inputRepresentation.ownerGroup->conjugacyClassCount() &&
    weylData->group.ComputeIrreducibleRepresentationsWithFormulas
  ) {
    weylData->group.ComputeIrreducibleRepresentationsWithFormulas(
      weylData->group
    );
  }
  if (
    inputRepresentation.ownerGroup->irreducibleRepresentations.size <
    inputRepresentation.ownerGroup->conjugacyClassCount()
  ) {
    weylData->computeInitialIrreducibleRepresentations();
    weylData->group.computeIrreducibleRepresentationsTodorsVersion();
  }
  VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
  outputRepresentation;
  outputRepresentation.assignRepresentation(inputRepresentation);
  return output.assignValue(calculator, outputRepresentation);
}
