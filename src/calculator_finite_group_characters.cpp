#include "calculator.h"
#include "math_general_implementation.h"
#include "math_extra_finite_groups.h"
#include "math_extra_finite_groups_implementation.h"
#include "math_extra_symmetric_groups_and_generalizations.h"
#include "math_extra_graph.h"
#include "calculator_Weyl_group_characters.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_extra_differential_operators.h"

template<>
List<ClassFunction<WeylGroupData::WeylGroupBase, Rational> >::Comparator*
FormatExpressions::getMonomialOrder<ClassFunction<WeylGroupData::WeylGroupBase, Rational> >() {
  return nullptr;
}

template < >
WeylGroupData& Expression::getValueNonConst() const;

bool WeylGroupData::checkConsistency() const {
  if (this->flagDeallocated) {
    global.fatal << "Use after free of WeylGroup. " << global.fatal;
  }
  for (int i = 0; i < this->theGroup.generators.size; i ++) {
    this->theGroup.generators[i].checkConsistency();
  }
  this->rootsOfBorel.checkConsistency();
  return true;
}

template <typename elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::checkInitializationFiniteDimensionalRepresentationComputation() const {
  if (this->elements.size == 0) {
    global.fatal << "Request to compute character hermitian product in a group whose "
    << "conjugacy classes and/or elements have not been computed. The group reports to have "
    << this->conjugacyClassCount() << " conjugacy classes and " << this->elements.size << " elements. "
    << global.fatal;
    return false;
  }
  return this->checkInitializationConjugacyClasses();
}

template <typename somegroup, typename Coefficient>
bool GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::checkAllSimpleGeneratorsAreOK() const {
  this->checkInitialization();
  for (int i = 0; i < this->ownerGroup->generators.size; i ++) {
    if (this->generators[i].numberOfRows == 0) {
      global.fatal << "Working with a "
      << "representation in which the action of the simple generators is not computed. " << global.fatal;
      return false;
    }
  }
  return true;
}

/*template <typename somegroup, typename Coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::checkRepresentationIsMultiplicativelyClosed() {
  HashedList<Matrix<Rational> > tempList;
  tempList.addOnTop(this->theElementImages);
  Matrix<Rational> tempMat;
  ElementWeylGroup tempElt;
  for (int i = 0; i < tempList.size; i ++) {
    for (int j = 0; j < tempList.size; j ++) {
      tempMat = tempList[i];
      tempMat.multiplyOnTheLeft(tempList[j]);
      tempElt = this->ownerGroup->theElements[j];
      tempElt *= this->ownerGroup->theElements[i];
      tempElt.makeCanonical();
      int targetIndex = this->ownerGroup->theElements.getIndex(tempElt);
      if (!(tempMat == this->theElementImages[targetIndex])) {
        global.fatal << "this is a programming error: element " << i + 1 << " times element "
        << j + 1 << " is outside of the set, i.e.,  "
        << tempList[i].toString() << " * " << tempList[j].toString() << " is bad. " << global.fatal;
      }
    }
  }
}
*/

/*
template <typename somegroup, typename Coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::ComputeAllGeneratorImagesFromSimple(GlobalVariables* global) {
  MacroRegisterFunctionWithName("GroupRepresentationCarriesAllMatrices::ComputeAllGeneratorImagesFromSimple");
  this->checkInitialization();
  this->ownerGroup->checkInitializationFiniteDimensionalRepresentationComputation();
  HashedList<ElementWeylGroup> ElementsExplored;
  ElementsExplored.setExpectedSize(this->ownerGroup->theElements.size);
  this->theElementImages[0].makeIdentityMatrix(this->getDimension());
  ElementWeylGroup currentElt;
  int theRank = this->ownerGroup->getDimension();
  currentElt.makeIdentity(*this->ownerGroup);
  ElementsExplored.addOnTop(currentElt);
  List<ElementWeylGroup> theGens;
  theGens.setSize(theRank);
  for (int i = 0; i < theRank; i ++)
    theGens[i].makeSimpleReflection(i, *this->ownerGroup);
  for (int i = 0; i <ElementsExplored.size; i ++) {
    int indexParentElement = this->ownerGroup->theElements.getIndex(ElementsExplored[i]);
    for (int j = 0; j < theRank; j ++) {
      currentElt = theGens[j]* ElementsExplored[i];
      if (!ElementsExplored.contains(currentElt)) {
        int indexCurrentElt = this->ownerGroup->theElements.getIndex(currentElt);
        this->theElementIsComputed[indexCurrentElt] = true;
        this->theElementImages[indexParentElement].multiplyOnTheLeft(this->generators[j], this->theElementImages[indexCurrentElt]);
        ElementsExplored.addOnTop(currentElt);
      }
    }
  }
//  this->checkRepresentationIsMultiplicativelyClosed();
  //FormatExpressions tempFormat;
  //tempFormat.flagUseLatex = true;
  //for (int i = 0; i < this->theElementImages.size; i ++)
  //{
  //}
}
*/

// This method uses auto everywhere, and a lot of copying data in order to use auto
template <typename somegroup, typename Coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::computeAllElementImages() {
  MacroRegisterFunctionWithName("GroupRepresentationCarriesAllMatrices::ComputeAllGeneratorImagesFromSimple");
  this->checkInitialization();
  this->ownerGroup->checkInitializationFiniteDimensionalRepresentationComputation();
  auto elementsExplored = this->ownerGroup->elements;
  elementsExplored.clear();
  elementsExplored.setExpectedSize(this->ownerGroup->elements.size);
  this->theElementImages[0].makeIdentityMatrix(this->getDimension());
  auto currentElt = this->ownerGroup->generators[0];
  currentElt.makeIdentity(this->ownerGroup->generators[0]);
  int theRank = this->ownerGroup->generators.size;
  auto& theGens = this->ownerGroup->generators;

  elementsExplored.addOnTop(currentElt);
  for (int i = 0; i < elementsExplored.size; i ++) {
    int indexParentElement = this->ownerGroup->elements.getIndex(elementsExplored[i]);
    for (int j = 0; j < theRank; j ++) {
      currentElt = theGens[j] * elementsExplored[i];
      if (!elementsExplored.contains(currentElt)) {
        int indexCurrentElt = this->ownerGroup->elements.getIndex(currentElt);
        this->elementIsComputed[indexCurrentElt] = true;
        this->theElementImages[indexParentElement].multiplyOnTheLeft(this->generators[j], this->theElementImages[indexCurrentElt]);
        elementsExplored.addOnTop(currentElt);
      }
    }
  }
//  this->checkRepresentationIsMultiplicativelyClosed();
  //FormatExpressions tempFormat;
  //tempFormat.flagUseLatex = true;
  //for (int i = 0; i < this->theElementImages.size; i ++)
  //{   //}
}

template <typename somegroup, typename Coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::operator*=(
  const GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>& other
) {
  MacroRegisterFunctionWithName("WeylGroupRepresentation::operator*=");
  //lazy programmers handling://////
  if (&other == this ) {
    GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> otherCopy;
    otherCopy = other;
    *this *= otherCopy;
    return;
  }
  //////////////////////////////////
  if (this->ownerGroup != other.ownerGroup) {
    global.fatal << "Attempt to multiply representations with different owner groups. " << global.fatal;
  }
  if (!this->flagCharacterIsComputed || !other.flagCharacterIsComputed) {
    global.fatal << "Attempting to multiply Weyl group reps whose characters have not been computed. " << global.fatal;
  }
  GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> output;
  output.initialize(*this->ownerGroup);
  output.theCharacter = this->theCharacter;
  output.theCharacter *= other.theCharacter;
  for (int i = 0; i < output.generators.size; i ++) {
    output.generators[i].assignTensorProduct(this->generators[i], other.generators[i]);
  }
  for (int i = 0; i < output.theElementImages.size; i ++) {
    if (this->elementIsComputed[i] && other.elementIsComputed[i]) {
      output.theElementImages[i].assignTensorProduct(this->theElementImages[i], other.theElementImages[i]);
      output.elementIsComputed[i] = true;
    }
  }
  *this = output;
}

template <typename somegroup, typename Coefficient>
void GroupRepresentation<somegroup, Coefficient>::operator*=(const GroupRepresentation<somegroup, Coefficient>& other) {
  MacroRegisterFunctionWithName("GroupRepresentation::operator*=");
  //lazy programmers handling://////
  if (&other == this ) {
    GroupRepresentation<somegroup, Coefficient> otherCopy;
    otherCopy = other;
    *this *= otherCopy;
    return;
  }
  //////////////////////////////////
  if (this->ownerGroup != other.ownerGroup) {
    global.fatal << "Attempt to multiply representations with different owner groups. " << global.fatal;
  }
  if (!this->flagCharacterIsComputed || !other.flagCharacterIsComputed) {
    global.fatal << "Attempting to multiply Weyl group reps whose characters have not been computed. " << global.fatal;
  }
  GroupRepresentation<somegroup, Coefficient> output;
  output.ownerGroup = this->ownerGroup;
  output.theCharacter = this->theCharacter;
  output.theCharacter *= other.theCharacter;
  for (int i = 0; i < output.generators.size; i ++) {
    output.generators[i].assignTensorProduct(this->generators[i], other.generators[i]);
  }
  *this = output;
}

template <typename somegroup, typename Coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::restrictRepresentation(
  const Vectors<Coefficient>& vectorSpaceBasisSubrep,
  const ClassFunction<somegroup, Rational>& remainingCharacter,
  GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>& output
) {
  MacroRegisterFunctionWithName("WeylGroupRepresentation::restrict");
  this->checkAllSimpleGeneratorsAreOK();
  if (vectorSpaceBasisSubrep.size == 0) {
    global.fatal << "Restriction of representation to a zero subspace is not allowed. " << global.fatal;
  }
  output.initialize(*this->ownerGroup);
  output.basis = vectorSpaceBasisSubrep;
  output.basis.getGramMatrix(output.gramMatrixInverted, 0);
  output.gramMatrixInverted.invert();
  output.theCharacter = remainingCharacter;
  ProgressReport theReport;
  for (int i = 0; i < this->generators.size; i ++) {
    if (theReport.tickAndWantReport()) {
      std::stringstream reportStream;
      reportStream << "Restricting the action of generator of index " << i;
      theReport.report(reportStream.str());
    }
    Matrix<Coefficient>::matrixInBasis(this->generators[i], output.generators[i], output.basis, output.gramMatrixInverted);
  }
  output.checkAllSimpleGeneratorsAreOK();
}

template <typename somegroup, typename Coefficient>
bool GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::decomposeTodorsVersion(
  VirtualRepresentation<somegroup, Coefficient>& outputIrrepMults,
  List<GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> >* appendOnlyGRCAMSList
) {
  MacroRegisterFunctionWithName("WeylGroupRepresentation::decomposeTodorsVersion");
  this->checkInitialization();
  this->ownerGroup->checkInitializationFiniteDimensionalRepresentationComputation();
  outputIrrepMults.makeZero();
  List<GroupRepresentation<somegroup, Coefficient> > appendOnlyIrrepsList;
  for (int i = 0; i < this->ownerGroup->irreps.size; i ++) {
    appendOnlyIrrepsList.addOnTop(this->ownerGroup->irreps[i]);
  }
  return this->decomposeTodorsVersionRecursive(outputIrrepMults, appendOnlyIrrepsList, appendOnlyGRCAMSList);
}

template <class Element>
bool Matrix<Element>::getEigenspacesProvidedAllAreIntegralWithEigenValueSmallerThanDimension(List<Vectors<Element> >& output) const {
  int upperLimitComputations = 100000;
  output.setSize(0);
  int found = 0;
  Polynomial<Element> theMinPoly;
  theMinPoly.assignMinimalPolynomial(*this);
  Vector<Element> theEigenValueCandidate;
  theEigenValueCandidate.setSize(1);
  Matrix<Rational> tempMat;
  for (int ii = 0; ii < upperLimitComputations; ii ++) {
    int i = ((ii + 1) / 2) * (2 * (ii % 2) - 1); // 0, 1, - 1, 2, - 2, 3, - 3,...
    theEigenValueCandidate[0] = i;
    if (theMinPoly.evaluate(theEigenValueCandidate) == 0) {
      tempMat = *this;
      output.setSize(output.size + 1);
      tempMat.getEigenspaceModifyMe(theEigenValueCandidate[0], *output.lastObject());
      if (output.lastObject()->size == 0) {
        global.fatal << "This is a programmig error: " << theEigenValueCandidate[0].toString()
        << " is a zero of the minimal polynomial "
        << theMinPoly.toString() << " of the operator " << this->toString()
        << " but the corresponding eigenspace is empty. " << global.fatal;
      }
      found += output.lastObject()->size;
      if (found == this->numberOfColumns) {
        return true;
      }
    }
  }
  return false;
}

void WeylGroupData::computeIrreducibleRepresentationsWithFormulasImplementation(FiniteGroup<ElementWeylGroup> &G) {
  List<char> letters;
  List<int> ranks;
  WeylGroupData& WD = *(G.generators[0].owner);
  WD.dynkinType.getLettersTypesMultiplicities(&letters, &ranks, nullptr);
  // When WeylGroupRepresentation is merged with GroupRepresentation,
  // and WeylGroupData split from FiniteGroup<ElementWeylGroup<WeylGroup> >
  // theRepresentations will be this->theGroup->irreps
  // currently we have the difficulty with GroupRepresentation<WeylGroup, Coefficient> vs
  // GroupRepresentation<FiniteGroup<ElementWeylGroup<WeylGroup> >, Coefficient>
  if ((letters.size == 1) && (letters[0] == 'A')) {
    int theRank = ranks[0];
    List<Partition> thePartitions;
    Partition::GetPartitions(thePartitions,theRank + 1);
    for (int i = 0; i < thePartitions.size; i ++) {
      GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> irrep;
      thePartitions[i].spechtModuleMatricesOfTranspositionsjjplusone(irrep.generators);
      irrep.ownerGroup = &G;
      irrep.identifyingString = thePartitions[i].toString();
      irrep.computeCharacter();
      G.addIrreducibleRepresentation(irrep);
    }
  } else if ((letters.size == 1) && (letters[0] == 'B')) {
    int theRank = ranks[0];
    HyperoctahedralGroupData HOG;
    HOG.makeHyperoctahedralGroup(theRank);
    HOG.allSpechtModules();
    GroupHomomorphism<ElementWeylGroup, ElementHyperoctahedralGroupR2> phi;
    phi.preimageGroup = &G;
    phi.generatorImages.setSize(G.generators.size);
    for (int i = 0; i < phi.generatorImages.size - 1; i ++) {
      phi.generatorImages[i].h.addTransposition(i, i + 1);
    }
    for (int i = 0; i < phi.generatorImages.size - 1; i ++) {
      phi.generatorImages.lastObject()->k.toggleBit(i);
    }
    global << "Generator commutation relations of groups\n";
    global << HOG.theGroup->prettyPrintGeneratorCommutationRelations();
    FiniteGroup<ElementHyperoctahedralGroupR2> phiG;
    phiG.generators = phi.generatorImages;
    global << phiG.prettyPrintGeneratorCommutationRelations();
    global << G.prettyPrintGeneratorCommutationRelations();
    global << "pulling back irreps:\n";
    for (int i = 0; i < HOG.theGroup->irreps.size; i ++) {
      auto irrep = phi.pullbackRepresentation(HOG.theGroup->irreps[i]);
      irrep.computeCharacter();
      global << HOG.theGroup->irreps[i].theCharacter << "->" << irrep.theCharacter << '\n';
      G.addIrreducibleRepresentation(irrep);
    }
  } else if ((letters.size == 1) && (letters[0] == 'D')) {
    int theRank = ranks[0];
    HyperoctahedralGroupData HOG;
    HOG.makeHyperoctahedralGroup(theRank + 1);
    GroupHomomorphism<ElementWeylGroup, ElementHyperoctahedralGroupR2> inclusionMap;
    inclusionMap.preimageGroup = &G;
    inclusionMap.generatorImages.setSize(G.generators.size);
    for (int i = 0; i < inclusionMap.generatorImages.size - 1; i ++) {
      inclusionMap.generatorImages[i].h.addTransposition(i, i + 1);
      inclusionMap.generatorImages[i].k.toggleBit(i);
      inclusionMap.generatorImages[i].k.toggleBit(i + 1);
    }
    int oneortwo = (inclusionMap.generatorImages.size + 1) % 2;
    for (int i = 0; i < inclusionMap.generatorImages.size - 1 - oneortwo; i ++) {
      inclusionMap.generatorImages.lastObject()->k.toggleBit(i);
    }

    FiniteGroup<ElementHyperoctahedralGroupR2> imG;
    imG.generators = inclusionMap.generatorImages;
    global << HOG.theGroup->prettyPrintGeneratorCommutationRelations() << '\n';
    global << imG.prettyPrintGeneratorCommutationRelations();
    global << G.prettyPrintGeneratorCommutationRelations() << '\n';
  }
  // silently fail instead of crashing to support calling into this and if it doesn't
  // work then using brute force
  //  else
  //    global.fatal << "ComputeIrreducibleRepresentationsUsingSpechtModules: Type "
  // << this->theDynkinType << " is unsupported.  If you think it should work, edit " << __FILE__ << ":" << __LINE__ << global.fatal;
  global << G.prettyPrintCharacterTable() << '\n';
}

template <class Coefficient>
void WeylGroupData::raiseToMaximallyDominant(List<Vector<Coefficient> >& theWeights) {
  bool found;
  for (int i = 0; i < theWeights.size; i ++) {
    do {
      found = false;
      for (int j = 0; j < this->rootsOfBorel.size; j ++) {
        if (this->rootScalarCartanRoot(this->rootsOfBorel[j], theWeights[i]) < 0) {
          bool isGood = true;
          for (int k = 0; k < i; k ++) {
            if (this->rootScalarCartanRoot(this->rootsOfBorel[j], theWeights[k]) > 0) {
              isGood = false;
              break;
            }
          }
          if (!isGood) {
            continue;
          }
          for (int k = 0; k < theWeights.size; k ++) {
            this->reflectBetaWithRespectToAlpha(this->rootsOfBorel[j], theWeights[k], false, theWeights[k]);
          }
          found = true;
        }
      }
    } while (found);
  }
}

bool CalculatorFunctionsWeylGroup::weylRaiseToMaximallyDominant(
  Calculator& calculator, const Expression& input, Expression& output, bool useOuter
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::weylRaiseToMaximallyDominant");
  if (input.size() < 2) {
    return output.makeError("Raising to maximally dominant takes at least 2 arguments, type and vector", calculator);
  }
  const Expression& semisimpleLieAlgebraNode = input[1];
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebra;
  if (!CalculatorConversions::convert(
    semisimpleLieAlgebraNode,
    CalculatorConversions::functionSemisimpleLieAlgebra,
    semisimpleLieAlgebra
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  SemisimpleLieAlgebra* theSSalgebra = semisimpleLieAlgebra.content;
  Vectors<Rational> theHWs;
  theHWs.setSize(input.size() - 2);
  bool isGood = true;
  for (int i = 2; i < input.size(); i ++) {
    if (!calculator.getVector<Rational>(
      input[i], theHWs[i - 2], nullptr, theSSalgebra->getRank()
    )) {
      isGood = false;
      break;
    }
  }
  if (!isGood && input.size() == 3) {
    Matrix<Rational> theHWsMatForm;
    if (calculator.functionGetMatrix(
      input[2], theHWsMatForm, nullptr, theSSalgebra->getRank()
    )) {
      theHWsMatForm.getVectorsFromRows(theHWs);
      isGood = true;
    }
  }
  if (theHWs.size == 0 || !isGood) {
    return output.makeError("Failed to extract rational vectors from expression " + input.toString() + ". ", calculator);
  }
  std::stringstream out;
  out << "Input: " << theHWs.toString()
  << ", simultaneously raising to maximally dominant in Weyl group of type "
  << theSSalgebra->weylGroup.dynkinType.toString();
  if (!useOuter) {
    theSSalgebra->weylGroup.raiseToMaximallyDominant(theHWs);
  } else {
    WeylGroupAutomorphisms theOuterAutos;
    theOuterAutos.theWeyl = &theSSalgebra->weylGroup;
    theOuterAutos.raiseToMaximallyDominant(theHWs);
  }
  out << "<br>Maximally dominant output: " << theHWs.toString();
  return output.assignValue(out.str(), calculator);
}

template <>
bool CalculatorConversions::functionPolynomial<Rational>(Calculator& calculator, const Expression& input, Expression& output);

bool CalculatorFunctionsWeylGroup::weylGroupOrbitOuterSimple(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::weylGroupOrbitOuterSimple");
  if (!input.isListNElements(3)) {
    return output.makeError("weylOrbit takes two arguments", calculator);
  }
  const Expression& algebraExpression = input[1];
  const Expression& vectorNode = input[2];
  DynkinType dynkinType;
  if (algebraExpression.isOfType<SemisimpleLieAlgebra*>()) {
    SemisimpleLieAlgebra* algebra = algebraExpression.getValueNonConst<SemisimpleLieAlgebra*>();
    dynkinType = algebra->weylGroup.dynkinType;
  } else {
    if (!CalculatorConversions::functionDynkinType(
      calculator, algebraExpression, dynkinType
    )) {
      return false;
    }
  }
  Vector<Polynomial<Rational> > theHWfundCoords, theHWsimpleCoords;
  ExpressionContext theContext(calculator);
  if (!calculator.getVector(
    vectorNode,
    theHWfundCoords,
    &theContext,
    dynkinType.getRank(),
    CalculatorConversions::functionPolynomial<Rational>
  )) {
    return output.makeError("Failed to extract highest weight", calculator);
  }
  WeylGroupData theWeyl;
  theWeyl.makeFromDynkinType(dynkinType);
  theHWsimpleCoords = theHWfundCoords;
  theHWfundCoords = theWeyl.getFundamentalCoordinatesFromSimple(theHWsimpleCoords);
  std::stringstream out, latexReport;
  Vectors<Polynomial<Rational> > theHWs;
  FormatExpressions theFormat = theContext.getFormat();
  theHWs.addOnTop(theHWsimpleCoords);
  HashedList<Vector<Polynomial<Rational> > > outputOrbit;
  WeylGroupAutomorphisms theOuterAutos;
  theOuterAutos.theWeyl = &theWeyl;
  if (!theOuterAutos.generateOuterOrbit(
    theHWs, outputOrbit, &theOuterAutos.allElements, 1921 * 2
  )) {
    out << "Failed to generate the entire orbit "
    << "(maybe too large?), generated the first " << outputOrbit.size
    << " elements only.";
  } else {
    out << "The orbit has " << outputOrbit.size << " elements.";
  }
  latexReport << "\\begin{longtable}{p{3cm}p{4cm}p{4cm}p{4cm}}Element & Eps. coord. & Image fund. coordinates& Hw minus wt. \\\\\n<br>";
  out << "<table><tr> <td>Group element</td> <td>Image in simple coords</td> <td>Epsilon coords</td><td>Fundamental coords</td>";
  out << "</tr>";
  bool useMathTag = outputOrbit.size < 150;
  Matrix<Rational> epsCoordMat;
  Polynomial<Rational> zero;
  theWeyl.dynkinType.getEpsilonMatrix(epsCoordMat);
  for (int i = 0; i < outputOrbit.size; i ++) {
    theFormat.simpleRootLetter = "\\alpha";
    theFormat.fundamentalWeightLetter = "\\psi";
    std::string orbitEltString = outputOrbit[i].toString(&theFormat);
    Vector<Polynomial<Rational> > epsVect = outputOrbit[i];
    epsCoordMat.actOnVectorColumn(epsVect, zero);
    std::string orbitEltStringEpsilonCoords = epsVect.toStringLetterFormat("\\varepsilon", &theFormat);
    std::string weightEltString =
    theWeyl.getFundamentalCoordinatesFromSimple(outputOrbit[i]).toStringLetterFormat
    (theFormat.fundamentalWeightLetter, &theFormat);
    out << "<tr>" << "<td>"
    << (useMathTag ? HtmlRoutines::getMathNoDisplay(theOuterAutos.allElements[i].toString()) : theOuterAutos.allElements[i].toString())
    << "</td><td>" << (useMathTag ? HtmlRoutines::getMathNoDisplay(orbitEltString) : orbitEltString) << "</td><td>"
    << (useMathTag ? HtmlRoutines::getMathNoDisplay(orbitEltStringEpsilonCoords) : orbitEltStringEpsilonCoords)
    << "</td><td>" << (useMathTag ? HtmlRoutines::getMathNoDisplay(weightEltString) : weightEltString) << "</td>";
    latexReport << "$" << theOuterAutos.allElements[i].toString(&theFormat) << "$ & $" << orbitEltStringEpsilonCoords << "$ & $"
    << weightEltString << "$ & $" << (outputOrbit[0]-outputOrbit[i]).toStringLetterFormat(theFormat.simpleRootLetter, &theFormat) << "$\\\\\n<br>";
    out << "</tr>";
  }
  latexReport << "\\end{longtable}";
  out << "</table>" << "<br> " << latexReport.str();
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctionsWeylGroup::weylGroupOrbitSize(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::weylGroupOrbitSize");
  //double startTimeForDebug= global.getElapsedSeconds();
  WithContext<SemisimpleLieAlgebra*> theAlgebra;
  Vector<Rational> theWeightRat;
  if (calculator.getTypeWeight<Rational>(
    calculator, input, theWeightRat, theAlgebra, nullptr
  )) {
    Rational result = theAlgebra.content->weylGroup.getOrbitSize(theWeightRat);
    return output.assignValue(result, calculator);
  }
  SemisimpleLieAlgebra* theSSalgebra = theAlgebra.content;
  Vector<Polynomial<Rational> > theWeightPoly;
  if (calculator.getTypeWeight<Polynomial<Rational> >(
    calculator,
    input,
    theWeightPoly,
    theAlgebra,
    CalculatorConversions::functionPolynomial<Rational>
  )) {
    Rational result = theSSalgebra->weylGroup.getOrbitSize(theWeightPoly);
    return output.assignValue(result, calculator);
  }
  return false;
}

bool CalculatorFunctionsWeylGroup::weylOrbit(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool useFundCoords,
  bool useRho
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::weylOrbit");
  if (!input.isListNElements(3)) {
    return output.makeError("weylOrbit takes two arguments", calculator);
  }
  WithContext<SemisimpleLieAlgebra*> theSSalgebra;
  Vector<Polynomial<Rational> > theWeight;
  if (!calculator.getTypeWeight(
    calculator,
    input,
    theWeight,
    theSSalgebra,
    CalculatorConversions::functionPolynomial<Rational>
  )) {
    return false;
  }
  Vector<Polynomial<Rational> > theHWfundCoords, theHWsimpleCoords, currentWeight;
  Polynomial<Rational> zero;
  WeylGroupData& theWeyl = theSSalgebra.content->weylGroup;
  if (!useFundCoords) {
    theHWsimpleCoords = theWeight;
    theHWfundCoords = theWeyl.getFundamentalCoordinatesFromSimple(theWeight);
  } else {
    theHWfundCoords = theWeight;
    theHWsimpleCoords = theWeyl.getSimpleCoordinatesFromFundamental(theWeight, zero);
  }
  std::stringstream out, latexReport;
  Vectors<Polynomial<Rational> > theHWs;
  FormatExpressions theFormat;
  theSSalgebra.context.getFormat(theFormat);
//  theFormat.fundamentalWeightLetter ="\\psi";
  theHWs.addOnTop(theHWsimpleCoords);
  HashedList<Vector<Polynomial<Rational> > > outputOrbit;
  HashedList<ElementWeylGroup> orbitGeneratingSet;
  Polynomial<Rational> theExp;
  if (!theSSalgebra.content->weylGroup.generateOrbit(theHWs, useRho, outputOrbit, false, 1921, &orbitGeneratingSet, 1921)) {
    out << "Failed to generate the entire orbit (maybe too large?), generated the first " << outputOrbit.size << " elements only.";
  } else {
    out << "The orbit has " << outputOrbit.size << " elements.";
  }
  latexReport
  << "\\begin{longtable}{p{3cm}p{4cm}p{4cm}p{4cm}}Element & Eps. coord. "
  << "& Image fund. coordinates& Hw minus wt. \\\\\n<br>";
  out << "<table><tr> <td>Group element</td>"
  << "<td>Image in simple coords</td>"
  << "<td>Epsilon coords</td><td>Fundamental coords</td>";
  if (useRho) {
    out << "<td>Homomorphism generator candidate</td>";
  }
  out << "</tr>";
  MonomialUniversalEnveloping<Polynomial<Rational> > standardElt;
  LargeInteger tempInt;
  bool useMathTag = outputOrbit.size < 150;
  Matrix<Rational> epsCoordMat;
  theWeyl.dynkinType.getEpsilonMatrix(epsCoordMat);
  GraphWeightedLabeledEdges integralPositiveRootReflectionGraph;
  integralPositiveRootReflectionGraph.numNodes = outputOrbit.size;
  integralPositiveRootReflectionGraph.nodeLabels.setSize(outputOrbit.size);
  theFormat.flagUseFrac = true;
  for (int i = 0; i < outputOrbit.size; i ++) {
    integralPositiveRootReflectionGraph.nodeLabels[i] =
    "$" + theWeyl.getEpsilonCoordinates(outputOrbit[i]).toStringEpsilonFormat(&theFormat) + "$ = $" +
    theWeyl.getFundamentalCoordinatesFromSimple(outputOrbit[i]).toStringLetterFormat("\\psi") + "$";
  }
  ElementWeylGroup currentElt;
  Vector<Polynomial<Rational> > differenceVector;
  Rational currentCoordDifference;
  for (int i = 0; i <outputOrbit.size; i ++) {
    for (int j = 0; j < theWeyl.rootsOfBorel.size; j ++) {
      currentWeight = outputOrbit[i];
      currentElt.MakeRootReflection(theWeyl.rootsOfBorel[j], theWeyl);
      if (useRho) {
        currentWeight += theWeyl.rho;
      }
      theWeyl.actOn(currentElt, currentWeight);
      if (useRho) {
        currentWeight -= theWeyl.rho;
      }
      differenceVector = outputOrbit[i] - currentWeight;
      bool isGood = !differenceVector.isEqualToZero();
      for (int k = 0; k < differenceVector.size; k++) {
        if (!differenceVector[k].isConstant(&currentCoordDifference)) {
          isGood = false;
          break;
        } else if (!currentCoordDifference.isInteger() || currentCoordDifference < 0) {
          isGood = false;
          break;
        }
      }
      if (isGood) {
        std::stringstream reflectionStream;
        reflectionStream << "s_{" << i << "}";
        integralPositiveRootReflectionGraph.addEdge(i, outputOrbit.getIndex(currentWeight), reflectionStream.str());
      }
    }
  }
  integralPositiveRootReflectionGraph.checkConsistency();
  out << integralPositiveRootReflectionGraph.toStringPsTricks(nullptr);
  for (int i = 0; i < outputOrbit.size; i ++) {
    theFormat.simpleRootLetter = "\\alpha";
    theFormat.fundamentalWeightLetter = "\\psi";
    std::string orbitEltString = outputOrbit[i].toString(&theFormat);
    Vector<Polynomial<Rational> > epsVect = outputOrbit[i];
    epsCoordMat.actOnVectorColumn(epsVect, zero);
    std::string orbitEltStringEpsilonCoords = epsVect.toStringLetterFormat("\\varepsilon", &theFormat);
    std::string weightEltString=
    theWeyl.getFundamentalCoordinatesFromSimple(outputOrbit[i]).toStringLetterFormat(theFormat.fundamentalWeightLetter, &theFormat);
    out << "<tr>" << "<td>"
    << (useMathTag ? HtmlRoutines::getMathNoDisplay(orbitGeneratingSet[i].toString()) : orbitGeneratingSet[i].toString())
    << "</td><td>"
    << (useMathTag ? HtmlRoutines::getMathNoDisplay(orbitEltString) : orbitEltString) << "</td><td>"
    << (useMathTag ? HtmlRoutines::getMathNoDisplay(orbitEltStringEpsilonCoords) : orbitEltStringEpsilonCoords)
    << "</td><td>"
    << (useMathTag ? HtmlRoutines::getMathNoDisplay(weightEltString) : weightEltString)
    << "</td>";
    latexReport << "$" << orbitGeneratingSet[i].toString(&theFormat) << "$ & $" << orbitEltStringEpsilonCoords
    << "$ & $" <<  weightEltString << "$ & $"
    << (outputOrbit[0] - outputOrbit[i]).toStringLetterFormat(theFormat.simpleRootLetter, &theFormat)
    << "$\\\\\n<br>";
    if (useRho) {
      currentWeight = theHWsimpleCoords;
      standardElt.makeOne(*theSSalgebra.content);
      bool isGood = true;
      for (int j = orbitGeneratingSet[i].generatorsLastAppliedFirst.size - 1; j >= 0; j --) {
        int simpleIndex = orbitGeneratingSet[i].generatorsLastAppliedFirst[j].index;
        theExp = theWeyl.getScalarProductSimpleRoot(currentWeight, simpleIndex);
        theWeyl.reflectRhoSimple(simpleIndex, currentWeight);
        theExp *= 2;
        theExp /= theWeyl.cartanSymmetric.elements[simpleIndex][simpleIndex];
        theExp += Rational(1);
        if (theExp.isInteger(&tempInt)) {
          if (tempInt < 0) {
            isGood = false;
            break;
          }
        }
        standardElt.multiplyByGeneratorPowerOnTheLeft(theSSalgebra.content->getNumberOfPositiveRoots() - simpleIndex - 1, theExp);
      }
      out << "<td>";
      if (isGood) {
        out << HtmlRoutines::getMathNoDisplay(standardElt.toString(&theFormat));
      } else {
        out << "-";
      }
      out << "</td>";
    }
    out << "</tr>";
  }
  latexReport << "\\end{longtable}";
  out << "</table>" << "<br> " << latexReport.str();
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctionsWeylGroup::weylGroupLoadOrComputeCharTable(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::weylGroupLoadOrComputeCharTable");
  if (!CalculatorConversions::innerLoadWeylGroup(calculator, input, output)) {
    return false;
  }
  WeylGroupData& theGroup = output.getValueNonConst<WeylGroupData>();
  if (theGroup.getDimension() > 8) {
    calculator << "Computing character table disabled for rank >= 8, modify file " << __FILE__
    << " line "  << __LINE__ << " to change that. ";
    return false;
  }
  std::stringstream reportStream;
  theGroup.computeOrLoadCharacterTable(&reportStream);
  calculator << reportStream.str();
  return output.assignValue(theGroup, calculator);
}

bool CalculatorFunctionsWeylGroup::weylGroupConjugacyClasseS(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::weylGroupConjugacyClasseS");
  if (!CalculatorConversions::innerLoadWeylGroup(calculator, input, output)) {
    return false;
  }
  WeylGroupData& theGroup = output.getValueNonConst<WeylGroupData>();
  if (theGroup.getDimension() > 8) {
    calculator << "Conjugacy classes computation disabled for rank greater than 8. "
    << "Modify source code "
    << "file " << __FILE__ << " line " << __LINE__
    << " and rebuild the calculator to change that.";
    return false;
  }
  std::stringstream out;
  theGroup.computeOrLoadConjugacyClasses(&out);
  calculator << out.str();
  return output.assignValue(theGroup, calculator);
}

bool CalculatorFunctionsWeylGroup::weylGroupOuterConjugacyClassesFromAllElements(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::weylGroupOuterConjugacyClassesFromAllElements");
  if (!CalculatorFunctionsWeylGroup::weylGroupConjugacyClassesFromAllElements(calculator, input, output)) {
    return false;
  }
  WeylGroupData& theGroupData = output.getValueNonConst<WeylGroupData>();
  char theType ='X';
  int theRank = - 1;
  bool hasOuterAutosAndIsSimple = false;
  if (theGroupData.dynkinType.isSimple(&theType, &theRank)) {
    if (theType == 'D' || theType == 'A') {
      hasOuterAutosAndIsSimple = true;
    }
    if (theType == 'E' && theRank == 6) {
      hasOuterAutosAndIsSimple = true;
    }
  }
  if (!hasOuterAutosAndIsSimple) {
    return false;
  }
  FiniteGroup<Matrix<Rational> > groupNoOuterAutos;
  WeylGroupAutomorphisms theAutomorphismGroup;
  theAutomorphismGroup.theWeyl = &theGroupData;
  theAutomorphismGroup.computeOuterAutoGenerators();
  groupNoOuterAutos.generators.setSize(theGroupData.getDimension());
  Vector<Rational> simpleRoot;
  for (int i = 0; i < theGroupData.getDimension(); i ++) {
    simpleRoot.makeEi(theGroupData.getDimension(), i);
    theGroupData.getMatrixReflection(simpleRoot, groupNoOuterAutos.generators[i]);
  }
  //if (false)
  Matrix<Rational> currentAuto;
  List<Matrix<Rational> > outerAutos;
  for (int i = 0; i < theAutomorphismGroup.theOuterAutos.theGenerators.size; i ++) {
    theAutomorphismGroup.theOuterAutos.theGenerators[i].getMatrix(currentAuto, theGroupData.getDimension());
    outerAutos.addOnTop(currentAuto);
  }
  std::stringstream out;
  out << "Weyl group generators. <br>";
  for (int i = 0; i <groupNoOuterAutos.generators.size; i ++) {
    out << "Generator " << i + 1 << ": "
    << groupNoOuterAutos.generators[i].toString() << "<br>";
  }
//  out << "Outer automorphism realizations:<br>";
//  for (int i = 0; i <outerAutossize; i ++)
//    out << "Generator " << i << ": "
//    << outerAutossize[i].toString() << "<br>";
//  groupWithOuterAutos.computeAllElements(false, - 1);
  groupNoOuterAutos.computeAllElements(false, - 1);
  groupNoOuterAutos.computeConjugacyClassesFromAllElements();
  out << "Weyl group matrix realization: " << groupNoOuterAutos.toString();
  Matrix<Rational> conjugatedMat, invertedOuterAuto;
  for (int j = 0; j < outerAutos.size; j ++) {
    out << "Outer automorphism " << j << ": "
    << outerAutos[j].toString() << "<br>";
    invertedOuterAuto = outerAutos[j];
    invertedOuterAuto.invert();
    for (int i = 0; i <groupNoOuterAutos.conjugacyClasses.size; i ++) {
      conjugatedMat = outerAutos[j];
      conjugatedMat *= groupNoOuterAutos.conjugacyClasses[i].representative;
      conjugatedMat *= invertedOuterAuto;
      int found = - 1;
      for (int k = 0; k<groupNoOuterAutos.conjugacyClasses.size; k ++) {
        if (groupNoOuterAutos.conjugacyClasses[k].elements.contains(conjugatedMat)) {
          found = k;
          break;
        }
      }
      out << "Maps conj. class " << i + 1 << " -> " << found + 1 << "<br>";
    }
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctionsWeylGroup::weylGroupConjugacyClassesFromAllElements(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::weylGroupConjugacyClassesFromAllElements");
  if (!CalculatorConversions::innerLoadWeylGroup(calculator, input, output)) {
    return false;
  }
  WeylGroupData& theGroupData = output.getValueNonConst<WeylGroupData>();
  if (theGroupData.getDimension() > 7) {
    calculator << "<hr>Loaded Dynkin type "
    << theGroupData.dynkinType.toString() << " of rank "
    << theGroupData.getDimension() << " but I've been told "
    << "not to compute when the rank is larger than 7. ";
    return false;
  }
  double timeStart1 = global.getElapsedSeconds();
  theGroupData.theGroup.computeConjugacyClassesFromAllElements();
  //std::stringstream out;
  calculator << "<hr>Computed conjugacy classes of "
  << theGroupData.toString() << " in " << global.getElapsedSeconds() - timeStart1
  << " second(s). ";
  return output.assignValue(theGroupData, calculator);
}

bool CalculatorFunctionsWeylGroup::weylGroupConjugacyClassesRepresentatives(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::weylGroupConjugacyClassesRepresentatives");
  if (!CalculatorConversions::innerLoadWeylGroup(calculator, input, output)) {
    return false;
  }
  WeylGroupData& theGroupData = output.getValueNonConst<WeylGroupData>();
  theGroupData.checkConsistency();
  if (theGroupData.getDimension() > 8) {
    return calculator << "<hr>Loaded Dynkin type " << theGroupData.dynkinType.toString()
    << " of rank " << theGroupData.getDimension() << " but I've been told "
    << "not to compute when the rank is larger than 8. ";
  }
  theGroupData.checkConsistency();
  double timeStart1 = global.getElapsedSeconds();
  theGroupData.checkConsistency();
  theGroupData.theGroup.computeConjugacyClassSizesAndRepresentatives();
  calculator << "<hr> Computed conjugacy classes representatives of "
  << theGroupData.dynkinType.toString() << " in " << global.getElapsedSeconds()-timeStart1
  << " second(s). ";
  return output.assignValue(theGroupData, calculator);
}

bool CalculatorFunctionsWeylGroup::weylGroupIrrepsAndCharTableComputeFromScratch(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::weylGroupIrrepsAndCharTableComputeFromScratch");
  if (!CalculatorFunctionsWeylGroup::weylGroupConjugacyClasseS(calculator, input, output)) {
    return false;
  }
  if (!output.isOfType<WeylGroupData>()) {
    return true;
  }
  WeylGroupData& theGroupData = output.getValueNonConst<WeylGroupData>();
  theGroupData.computeInitialIrreducibleRepresentations();
  theGroupData.theGroup.computeIrreducibleRepresentationsTodorsVersion();
  FormatExpressions tempFormat;
  tempFormat.flagUseLatex = true;
  tempFormat.flagUseHTML = false;
  std::stringstream out;
  out << "Character table: ";
  out << theGroupData.theGroup.prettyPrintCharacterTable();
  out << "<br>Explicit realizations of each representation follow.";
  for (int i = 0; i < theGroupData.theGroup.irreps.size; i ++) {
    out << "<hr>" << theGroupData.theGroup.irreps[i].toString(&tempFormat);
  }
  out << theGroupData.toString(&tempFormat);
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctionsWeylGroup::weylGroupOuterAutoGeneratorsPrint(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::weylGroupOuterAutoGeneratorsPrint");
  DynkinType theType;
  if (!CalculatorConversions::innerDynkinTypE(calculator, input, theType)) {
    return output.makeError("Failed to extract Dynkin type from argument. ", calculator);
  }
  std::stringstream out, outCommand;
  FinitelyGeneratedMatrixMonoid<Rational> groupGeneratedByMatrices;
  theType.getOuterAutosGeneratorsActOnVectorColumn(groupGeneratedByMatrices.theGenerators);
  FormatExpressions tempFormat;
  tempFormat.flagUseLatex = true;
  tempFormat.flagUseHTML = false;
  for (int i = 0; i <groupGeneratedByMatrices.theGenerators.size; i ++) {
    outCommand << "<br>s_{" << i + 1
    << "}=MatrixRationals" << groupGeneratedByMatrices.theGenerators[i].toStringMatrixForm(&tempFormat) << ";";
    out << "<br>s_" << i + 1 << " = "
    << HtmlRoutines::getMathNoDisplay(groupGeneratedByMatrices.theGenerators[i].toStringMatrixForm(&tempFormat));
  }
  outCommand << "<br>GenerateFiniteMultiplicativelyClosedSet(1000, ";
  for (int i = 0; i < groupGeneratedByMatrices.theGenerators.size; i ++) {
    outCommand << "s_{" << i + 1 << "}";
    if (i != groupGeneratedByMatrices.theGenerators.size - 1) {
      outCommand << ", ";
    }
  }
  outCommand << ");";
  out << outCommand.str();
  bool success = groupGeneratedByMatrices.generateElements(10000);
  if (!success) {
    out << "<br>Did not succeed to generate all elements of the group - the group is of size larger than 10000";
  } else {
    out << "<br>The group generated by the outer automorphisms is of size " << groupGeneratedByMatrices.theElements.size;
    if (groupGeneratedByMatrices.theElements.size > 100) {
      out << "<br>As the group has more than 100 elements, I shall abstain from printing them. ";
    } else {
      out << "<table><tr><td>Element</td><td>Matrix</td></tr>";
      for (int i = 0; i < groupGeneratedByMatrices.theElements.size; i ++) {
        std::stringstream elementNameStream;
        elementNameStream << "t_" << i + 1;
        out << "<tr><td>" << HtmlRoutines::getMathNoDisplay(elementNameStream.str())<< "</td><td>"
        << HtmlRoutines::getMathNoDisplay(groupGeneratedByMatrices.theElements[i].toStringMatrixForm(&tempFormat)) << "</td></tr>";
      }
      out << "</table>";
    }
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctionsWeylGroup::weylGroupOrbitFundRho(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return CalculatorFunctionsWeylGroup::weylOrbit(calculator, input, output, true, true);
}

bool CalculatorFunctionsWeylGroup::weylGroupOrbitFund(Calculator& calculator, const Expression& input, Expression& output) {
  return CalculatorFunctionsWeylGroup::weylOrbit(calculator, input, output, true, false);
}

bool CalculatorFunctionsWeylGroup::weylGroupOrbitSimple(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return CalculatorFunctionsWeylGroup::weylOrbit(calculator, input, output, false, false);
}

bool CalculatorFunctionsWeylGroup::tensorWeylReps(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::tensorWeylReps");
  if (input.size() != 3) {
    return false;
  }
  GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> leftRep;
  GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> rightRep;
  if (!input[1].isOfType<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >(&leftRep)) {
    return false;
  }
  if (!input[2].isOfType<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> > (&rightRep)) {
    return false;
  }
  FormatExpressions theFormat;
  theFormat.flagUseLatex = true;
  theFormat.flagUseHTML = false;
  if (leftRep.ownerGroup != rightRep.ownerGroup) {
    return output.makeError("Error: attempting to tensor irreps with different owner groups. ", calculator);
  }
  leftRep *= rightRep;
  return output.assignValue(leftRep, calculator);
}

bool CalculatorFunctionsWeylGroup::tensorAndDecomposeWeylReps(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::tensorAndDecomposeWeylReps");
  Expression theTensor;
  if (input.size() != 3) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (!leftE.isOfType<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >()) {
    return false;
  }
  if (!rightE.isOfType<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >()) {
    return false;
  }
  VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> outputRep =
  leftE.getValue<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >();
  outputRep *= rightE.getValue<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >();
  return output.assignValue(outputRep, calculator);
}

std::string WeylGroupData::toStringIrreducibleRepresentationLabel(int indexIrrep) {
  if (indexIrrep < this->irrepsCarterLabels.size) {
    return this->irrepsCarterLabels[indexIrrep];
  }
  std::stringstream out;
  out << "\\phi_{" << indexIrrep + 1 << "}";
  return out.str();
}

std::string WeylGroupData::toStringSignSignatureRootSubsystem(const List<SubgroupDataRootReflections>& inputSubgroups) {
  MacroRegisterFunctionWithName("WeylGroup::toStringSignSignatureRootSubsystem");
  if (inputSubgroups.size == 0) {
    return "";
  }
  std::stringstream out;

  FormatExpressions formatSupressUpperIndexOne;
  formatSupressUpperIndexOne.flagSupressDynkinIndexOne = true;
  DynkinSimpleType simpleType;

  std::stringstream mainTableStream;
  int numParabolicClasses = 0, numNonParabolicPseudoParabolic = 0, numNonPseudoParabolic = 0;
  for (int i = 0; i < inputSubgroups.size; i ++) {
    SubgroupDataRootReflections& currentSG = inputSubgroups[i];
    if (!currentSG.flagIsParabolic && !currentSG.flagIsExtendedParabolic) {
      numNonPseudoParabolic ++;
      continue;
    }
    if (currentSG.flagIsParabolic) {
      numParabolicClasses ++;
    }
    if (!currentSG.flagIsParabolic && currentSG.flagIsExtendedParabolic) {
      numNonParabolicPseudoParabolic ++;
    }
  }

  //check for repeating signatures
  List<List<Rational> > pseudoSignSig, fullSignSig, parabolicSignSig;
  fullSignSig.setSize(this->theGroup.conjugacyClassCount());
  pseudoSignSig.setSize(this->theGroup.conjugacyClassCount());
  parabolicSignSig.setSize(this->theGroup.conjugacyClassCount());
  for (int i = 0; i < this->theGroup.conjugacyClassCount(); i ++) {
    fullSignSig[i].setSize(inputSubgroups.size);
    pseudoSignSig[i].setSize(numParabolicClasses+numNonParabolicPseudoParabolic);
    parabolicSignSig[i].setSize(numParabolicClasses);
    for (int j = 0; j < inputSubgroups.size; j ++) {
      fullSignSig[i][j] = (inputSubgroups[j].tauSignature[i] == 0 ? 0 : 1);
      if (j < numParabolicClasses+numNonParabolicPseudoParabolic) {
        pseudoSignSig[i][j] = fullSignSig[i][j];
      }
      if (j < numParabolicClasses) {
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
  for (int i = 0; i < pseudoSignSig.size && !hasRepeatingPseudoParabolicSigs; i ++) {
    for (int j = i + 1; j < pseudoSignSig.size && !hasRepeatingPseudoParabolicSigs; j ++) {
      if (pseudoSignSig[i] == pseudoSignSig[j]) {
        hasRepeatingPseudoParabolicSigs = true;
      }
    }
  }
  for (int i = 0; i < parabolicSignSig.size && !hasRepeatingParSigs; i ++) {
    for (int j = i + 1; j < parabolicSignSig.size && !hasRepeatingParSigs; j ++) {
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
    out << "<hr><b>There are repeating pseudo-parabolic sign signatures. </b><hr>";
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
      irrepsPerSignature[parSignSigsNoRepetition.getIndex(parabolicSignSig[i])].addOnTop(this->irrepsCarterLabels[i]);
    }
    mainTableStream << "\n<br>\n\n<br>\nThe following families of representations share the same sign signature. ";
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
  } //end of check for repeating signatures
  if (hasRepeatingPseudoParabolicSigs) {
    HashedList<List<Rational> > pseudoSigsNoRepetition;
    List<List<std::string> > irrepsPerSignature;
    pseudoSigsNoRepetition.addOnTopNoRepetition(pseudoSignSig);
    irrepsPerSignature.setSize(pseudoSigsNoRepetition.size);
    for (int i = 0; i < pseudoSignSig.size; i ++) {
      irrepsPerSignature[pseudoSigsNoRepetition.getIndex(pseudoSignSig[i])].addOnTop(this->irrepsCarterLabels[i]);
    }
    mainTableStream << "\n<br>\n\n<br>\nThe following families of representations share the same pseudo-sign signature. ";
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
  } //end of check for repeating signatures
  int startIndex = 0;
  int numColsPerPage = 25;
  int startIndexNextCol = 0;
  for (;;) {
    startIndex = startIndexNextCol;
    if (startIndex >= inputSubgroups.size) {
      break;
    }
    startIndexNextCol = startIndex + numColsPerPage;
    if (startIndexNextCol - inputSubgroups.size > - 3) {
      startIndexNextCol = inputSubgroups.size;
    }

    mainTableStream << "\n<br>\n\\begin{longtable}{c|";
    for (int i = startIndex; i < startIndexNextCol; i ++) {
      if (i == numParabolicClasses) {
        mainTableStream << "|";
      }
      if (i == numParabolicClasses+numNonParabolicPseudoParabolic) {
        mainTableStream << "|";
      }
      mainTableStream << "p{0.275cm}";
    }
    mainTableStream << "}\n<br>\n" << "\\caption{\\label{table:SignSignature"
    << HtmlRoutines::cleanUpForLaTeXLabelUse(this->dynkinType.toString())
    << "}Multiplicity of the sign representation over the classes of root subgroups. "
    << "There are " << numParabolicClasses << " parabolic subgroup classes, " << numNonParabolicPseudoParabolic
    << " pseudo-parabolic subgroup classes that are not parabolic, and "
    << numNonPseudoParabolic << " non-pseudo-parabolic subgroup classes. \n<br>\n"
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
        mainTableStream << "&${\\widehat{\\mathfrak{ p}}}_{" << i - numParabolicClasses + 1 << "}$";
      }
    }
    mainTableStream << "\\\\\n<br>\n";
    mainTableStream << "Irrep label";
    if (this->dynkinType.isSimple(&simpleType.letter, &simpleType.rank, &simpleType.cartanSymmetricInverseScale)) {
      for (int i = startIndex; i < startIndexNextCol; i ++) {
        mainTableStream << "&$" << inputSubgroups[i].theDynkinType.toString(&formatSupressUpperIndexOne) << "$";
      }
    } else {
      for (int i = startIndex; i < startIndexNextCol; i ++) {
        mainTableStream << "&$" << inputSubgroups[i].theDynkinType.toString() << "$";
      }
    }
    for (int i = 0; i < this->theGroup.conjugacyClassCount(); i ++) {
      mainTableStream << "\\\\";
      if (i == 0) {
        mainTableStream << "\\hline";
      }
      mainTableStream << "\n<br>\n$" << this->toStringIrreducibleRepresentationLabel(i) << "$";
      for (int j = startIndex; j < startIndexNextCol; j ++) {
        mainTableStream << "&" << inputSubgroups[j].tauSignature[i].toString();
      }
      mainTableStream << "\\\\\n<br>\n";
    }
    mainTableStream << "\\end{longtable}\n<br>\n";
  }
  for (int s = 0; s < 2; s ++) {
    out << "<table style =\"white-space: nowrap;\" border =\"1\">";
    Selection parSelrootsAreOuttaLevi;
    out << "<tr><td>Irrep Label</td><td>Irreducible representation characters</td>";
    if (inputSubgroups[0].flagIsParabolic || inputSubgroups[0].flagIsExtendedParabolic) {
      for (int i = 0; i < inputSubgroups.size; i ++) {
        parSelrootsAreOuttaLevi = inputSubgroups[i].simpleRootsInLeviParabolic;
        parSelrootsAreOuttaLevi.invertSelection();
        out << "<td>" << parSelrootsAreOuttaLevi.toString() << "</td>";
      }
      out << "</tr><tr><td></td><td></td>";
    }
    if (this->dynkinType.isSimple(&simpleType.letter, &simpleType.rank, &simpleType.cartanSymmetricInverseScale)) {
      for (int i = 0; i < inputSubgroups.size; i ++) {
        out << "<td>" << inputSubgroups[i].theDynkinType.toString(&formatSupressUpperIndexOne)
        << "</td>";
      }
    } else {
      for (int i = 0; i < inputSubgroups.size; i ++) {
        out << "<td>" << inputSubgroups[i].theDynkinType.toString() << "</td>";
      }
    }
    out << "</tr>";
    for (int i = 0; i < this->theGroup.conjugacyClassCount(); i ++) {
      out << "<tr>";
      if (i < this->irrepsCarterLabels.size) {
        out << "<td>" << this->irrepsCarterLabels[i] << "</td>";
      } else {
        out << "<td></td>";
      }
      out << "<td>" << this->theGroup.characterTable[i].toString() << "</td>";
      if (s == 0) {
        for (int j = 0; j < inputSubgroups.size; j ++) {
          out << "<td>" << inputSubgroups[j].tauSignature[i].toString() << "</td>";
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

  out << "<br>A version of the table in ready for LaTeX consumption form follows.<br>\n<br>\n\n<br>\n\n<br>\n";
  out << "\\documentclass{article}\\usepackage{amssymb}\\usepackage{longtable}\\usepackage{pdflscape}"
  << "\\addtolength{\\hoffset}{-3.5cm}\\addtolength{\\textwidth}{6.8cm}"
  << "\\addtolength{\\voffset}{-3.3cm}\\addtolength{\\textheight}{6.3cm}"
  << " \\begin{document}\\begin{landscape}\n<br>\n\n<br>\n\n<br>\n\n<br>\n";
  out << "{\\tiny \n<br>\n \\renewcommand{\\arraystretch}{0}%\n<br>\n";
  out << "\\begin{longtable}{r|";
  int numIrreps = this->theGroup.characterTable[0].data.size;
  for (int i = 0; i < numIrreps; i ++) {
    out << "r";
  }
  out << "}"
  << "\\caption{\\label{tableIrrepChars" << this->dynkinType.toString()
  << "}\\\\ Irreducible representation characters. Columns are labeled by conjugacy classes.} \\\\";
  out << "<br>\n  & \\multicolumn{"
  << this->theGroup.characterTable[0].data.size
  << "}{c}{Conjugacy class $\\#$}\\\\<br>\n";

  out << "Irrep label";
  for (int i = 0; i < numIrreps; i ++) {
    out << "&$" << i + 1 << "$";
  }
  out << "\\\\\\hline<br>\n";
  for (int i = 0; i < this->theGroup.characterTable.size; i ++) {
    out << "$" << this->toStringIrreducibleRepresentationLabel(i) << "$";
    for (int j = 0; j < this->theGroup.characterTable[i].data.size; j ++) {
      out << "&$" << this->theGroup.characterTable[i].data[j].toString() << "$";
    }
    out << "\\\\<br>\n";
  }
  out << "\\end{longtable}\n<br>\n";
  out << "\\begin{longtable}{rrcl}" << "\\caption{\\label{tableConjugacyClassTable"
  << HtmlRoutines::cleanUpForLaTeXLabelUse(this->dynkinType.toString()) << "}}\\\\ ";
  out << "$\\#$ & Representative & Class size & Root subsystem label\\\\<br>\n";
  for (int i = 0; i < this->theGroup.conjugacyClassCount(); i ++) {
    out << "$" << i + 1 << "$ & " "$" << this->theGroup.conjugacyClasses[i].representative.toString()
    << "$&$ " << this->theGroup.conjugacyClasses[i].size.toString() << "$";
    if (i < this->ccCarterLabels.size) {
      out << "&$" << this->ccCarterLabels[i] << "$";
    }
    out << "\\\\<br>\n";
  }
  out << "\\end{longtable}\n<br>\n";

  out << mainTableStream.str();
  out << "}%arraystretch renewcommand scope\n<br\n>\n<br>\n\n<br>\n\n<br>\n\n<br>\n";
  out << "\\end{landscape}\\end{document}";
  return out.str();
}

class KostkaNumber {
public:
  List<int> partition;
  List<int> tuple;
  int sumTuple;
  int sumPartition;
  int MaxNumCachedKostkaNumbers;
  LargeInteger value;
//  bool incrementReturnFalseIfPastLast();
  bool initTableaux(std::stringstream* comments = nullptr);
  bool operator==(const KostkaNumber& other) const;
  static Rational ComputeTypeBParabolicSignMultiplicity(
    const Partition& parabolicPartition,
    const Partition& leftPartition,
    const Partition& rightPartition,
    std::stringstream* comments
  );
  static std::string GetTypeBParabolicSignMultiplicityTable(int rank);
  static unsigned int hashFunction(const KostkaNumber& input);
  std::string toString();
  KostkaNumber();
  bool compute(HashedList<KostkaNumber>* KNcache, std::stringstream* comments = nullptr);
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
  return  MathRoutines::hashListInts(input.partition) + MathRoutines::hashListInts(input.tuple);
}

bool KostkaNumber::operator==(const KostkaNumber& other) const {
  return this->partition == other.partition && this->tuple == other.tuple;
}

KostkaNumber::KostkaNumber() {
  this->MaxNumCachedKostkaNumbers = 10000;
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
        *comments << "Partition is supposed to be a non-decreasing sequence of integers, instead it is: "
        << this->partition;
      }
      return false;
    }
  }
  this->sumTuple = 0;
  this->sumPartition = 0;
  for (int i = 0; i < this->tuple.size; i ++) {
    this->sumTuple += this->tuple[i];
    if (this->sumTuple > 10000000 || this->tuple[i] > 10000000 || this->tuple[i] < 0) {
      if (comments != nullptr) {
        *comments << "Failed to compute Kostka number: the tuple "
        << this->tuple << " is too large or negative. ";
      }
      return false;
    }
  }
  for (int i = 0; i < this->partition.size; i ++) {
    this->sumPartition += this->partition[i];
    if (this->sumPartition > 10000000 || this->partition[i] > 10000000 || this->partition[i] < 0) {
      if (comments != nullptr) {
        *comments << "Failed to compute Kostka number: the partition: "
        << this->partition << " is too large or negative. ";
      }
      return false;
    }
  }
  return true;
}

bool KostkaNumber::compute(HashedList<KostkaNumber>* KNcache, std::stringstream* comments) {
  MacroRegisterFunctionWithName("KostkaNumber::compute");
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
  SelectionFixedRankDifferentMaxMultiplicities theSel;
  theSel.initialize();
  theSel.capacities.setSize(this->partition.size);
  for (int i = 0; i < this->partition.size; i ++) {
    if (i != this->partition.size - 1) {
      theSel.capacities[i] = this->partition[i] - this->partition[i + 1];
    } else {
      theSel.capacities[i] = this->partition[i];
    }
  }
  theSel.rank = *this->tuple.lastObject();
  this->value = 0;
  while (theSel.incrementReturnFalseIfPastLast()) {
    KostkaNumber ancestor;
    ancestor.partition = this->partition;
    ancestor.tuple = this->tuple;
    ancestor.tuple.setSize(ancestor.tuple.size - 1);
    for (int i = 0; i < theSel.multiplicities.size; i ++) {
      ancestor.partition[i] -= theSel.multiplicities[i];
    }
    if (KNcache != nullptr) {
      int ancestorIndex = KNcache->getIndex(ancestor);
      if (ancestorIndex != - 1) {
        ancestor = KNcache->getElement(ancestorIndex);
      } else if (!ancestor.compute(KNcache, comments)) {
        return false;
      } else {
        if (KNcache->size < this->MaxNumCachedKostkaNumbers) {
          KNcache->addOnTop(ancestor);
        }
      }
    } else {
      if (!ancestor.compute(KNcache, comments)) {
        return false;
      }
    }
    this->value += ancestor.value;
  }
  return true;
}

std::string KostkaNumber::GetTypeBParabolicSignMultiplicityTable(int rank) {
  MacroRegisterFunctionWithName("KostkaNumber::GetTypeBParabolicSignMultiplicityTable");
  std::stringstream out;
  List<Pair<Partition, Partition> > partitionPairs;
  List<Partition> partitionsLeft, partitionsRight;
  List<Partition> partitionsParabolics;
  Pair<Partition, Partition> currentPartition;
  for (int i = 0; i <= rank; i ++) {
    Partition::GetPartitions(partitionsLeft, i);
    Partition::GetPartitions(partitionsRight, rank - i);
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
    out << partitionPairs[i].object1.toString()
    << "," << partitionPairs[i].object2.toString() << "<br>";
  }
  partitionPairs.quickSortAscending();
  partitionsParabolics.quickSortAscending();
  Matrix<Rational> theMultTable;
  theMultTable.initialize(partitionPairs.size, partitionsParabolics.size);
  for (int j = 0; j < partitionPairs.size; j ++) {
    out << "V_{\\lambda, \\mu}, "
    << "<br>\\lambda =" << partitionPairs[j].object1.p
    << "<br>\\mu=" << partitionPairs[j].object2.p;
    for (int i = 0; i < partitionsParabolics.size; i ++) {
      theMultTable(j, i) = KostkaNumber::ComputeTypeBParabolicSignMultiplicity(
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
    std::stringstream parStream;
    parStream << "P_{";
    parStream << partitionsParabolics[i].toStringForArticles("(", ")")
    << ", ";
    int typeBsize = rank - partitionsParabolics[i].n;
    if (typeBsize == 0) {
      parStream << "\\emptyset";
    } else {
      parStream << "(" << typeBsize << ")";
    }
    parStream << "}";
    outLaTeX << "&$" << parStream.str() << "$";
    out << "<td>" << parStream.str();
    out << "</td>";
  }
  outLaTeX << "\\\\";
  out << "</tr>";
  ///////////////////////////////////////
  FormatExpressions theFormat;
  theFormat.flagSupressDynkinIndexOne = true;
  for (int i = 0; i < partitionsParabolics.size; i ++) {
    int typeBsize = rank - partitionsParabolics[i].n;
    DynkinType theType;
    DynkinSimpleType theSimpleType;
    for (int j = 0; j < partitionsParabolics[i].p.size; j ++) {
      if (partitionsParabolics[i].p[j] <= 1) {
        continue;
      }
      theSimpleType.makeArbitrary('A', partitionsParabolics[i].p[j] - 1, 1);
      theType.addMonomial(theSimpleType, 1);
    }
    if (typeBsize == 1) {
      theSimpleType.makeArbitrary('A', 1, 2);
      theType.addMonomial(theSimpleType,1);
    } else if (typeBsize > 1) {
      theSimpleType.makeArbitrary('B', typeBsize, 1);
      theType.addMonomial(theSimpleType, 1);
    }
    outLaTeX << "&$" << theType.toString(&theFormat) << "$";
  }
  outLaTeX << "\\\\";
  ///////////////////////////////////////
  ///////////////////////////////////////
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
  ///////////////////////////////////////
  for (int i = 0; i < partitionPairs.size; i ++) {
    std::stringstream Vstream;
    Vstream << "V_{"
    << partitionPairs[i].object1.toStringForArticles("[", "]")
    << ", "
    << partitionPairs[i].object2.toStringForArticles("[", "]")
    << "}";
    out << "<tr><td>";
    out << Vstream.str();
    out << "</td>";
    outLaTeX << "$" << Vstream.str() << "$";
    for (int j = 0; j < partitionsParabolics.size; j ++) {
      out << "<td>" << theMultTable(i, j) << "</td>";
      outLaTeX << "&" << "$" << theMultTable(i, j) << "$";
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

Rational KostkaNumber::ComputeTypeBParabolicSignMultiplicity(
  const Partition& parabolicPartition,
  const Partition& leftPartition,
  const Partition& rightPartition,
  std::stringstream* comments
) {
  MacroRegisterFunctionWithName("KostkaNumber::ComputeTypeBParabolicSignMultiplicity");
  int rank = leftPartition.n + rightPartition.n;
  int BcomponentSize = rank - parabolicPartition.n;
  if (comments != nullptr) {
    *comments << "<br>dim Hom_P(Sign P_{("
    << parabolicPartition.p << "), ("
    << BcomponentSize << ")}, V_{"
    << leftPartition.p.toStringCommaDelimited()
    << ", "
    << rightPartition.p.toStringCommaDelimited()
    << "} )=";
  }
  Rational result = 0;
  SelectionWithDifferentMaxMultiplicities theSelection;
  List<int> complementSelection;
  HashedList<KostkaNumber> KNcache;
  KostkaNumber leftKN, rightKN;
  Partition tempP;
  tempP = leftPartition;
  tempP.transpose();
  leftKN.partition = tempP.p;
  tempP = rightPartition;
  tempP.transpose();
  rightKN.partition = tempP.p;
  theSelection.initFromInts(parabolicPartition.p);
  do {
    complementSelection.setSize(parabolicPartition.p.size);
    for (int k = 0; k < theSelection.capacities.size; k ++) {
      complementSelection[k] = parabolicPartition.p[k] - theSelection.multiplicities[k];
    }
    leftKN.tuple = theSelection.multiplicities;
    leftKN.compute(&KNcache, nullptr);
    rightKN.tuple = complementSelection;
    rightKN.tuple.addOnTop(BcomponentSize);
    rightKN.compute(&KNcache, nullptr);
    result += leftKN.value * rightKN.value;
  } while (theSelection.incrementReturnFalseIfPastLast());
  if (comments != nullptr) {
    *comments << result.toString();
  }
  return result;
}

std::string SelectionFixedRankDifferentMaxMultiplicities::toString() {
  std::stringstream out;
  Vector<int> theMults;
  theMults = this->multiplicities;
  out << theMults;
  return out.str();
}

std::string SelectionFixedRankDifferentMaxMultiplicities::toStringFull() {
  std::stringstream out;
  Vector<int> theMults, theMaxMults;
  theMaxMults = this->capacities;
  theMults = this->multiplicities;
  out << "multiplicities: " << theMults << "; max: " << theMaxMults
  << "; rank: " << this->rank;
  return out.str();
}

SelectionFixedRankDifferentMaxMultiplicities::SelectionFixedRankDifferentMaxMultiplicities() {
  this->flagFirstComputed = false;
}

bool SelectionFixedRankDifferentMaxMultiplicities::initialize() {
  this->flagFirstComputed = false;
  return true;
}

bool SelectionFixedRankDifferentMaxMultiplicities::firstIncrement() {
  MacroRegisterFunctionWithName("SelectionFixedRankDifferentMaxMultiplicities::firstIncrement");
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

bool SelectionFixedRankDifferentMaxMultiplicities::incrementReturnFalseIfPastLast() {
  MacroRegisterFunctionWithName("SelectionFixedRankDifferentMaxMultiplicities::incrementReturnFalseIfPastLast");
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
    if (this->multiplicities[i] < this->capacities[i] && rankToRedistribute > 0) {
      this->multiplicities[i] ++;
      rankToRedistribute --;
      for (int j = this->multiplicities.size - 1; j > i; j --) {
        if (this->capacities[j]<=rankToRedistribute) {
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

bool CalculatorFunctionsWeylGroup::kostkaNumber(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::kostkaNumber");
  if (input.size() != 3) {
    return false;
  }
  KostkaNumber theKN;
  if (
    !calculator.getVectorInt(input[1], theKN.partition) ||
    !calculator.getVectorInt(input[2], theKN.tuple)
  ) {
    return calculator << "Failed to extract partition and tuple from input: " << input.toString();
  }
  std::stringstream out;
  HashedList<KostkaNumber> theKNs;
  if (theKN.compute(&theKNs, &out)) {
    out << "<br>Final result: " << theKN.value.toString();
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctionsWeylGroup::allSelectionsFixedRank(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::kostkaNumber");
  if (input.size() != 3) {
    return false;
  }
  SelectionFixedRankDifferentMaxMultiplicities theSel;
  if (!input[1].isSmallInteger(&theSel.rank)) {
    return false;
  }
  if (!calculator.getVectorInt(input[2], theSel.capacities)) {
    return calculator << "Failed to extract list of multiplicities from "
    << input[2].toString();
  }
  if (theSel.rank < 0) {
    return output.assignValue(0, calculator);
  }
  theSel.initialize();
  std::stringstream out;
  out << "Max multiplicities: " << theSel.capacities << " rank: "
  << theSel.rank;
  while (theSel.incrementReturnFalseIfPastLast()) {
    out << "<br>" << theSel.toString();
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctionsWeylGroup::signSignatureRootSubsystemsFromKostkaNumbers(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::signSignatureRootSubsystems");
  std::stringstream out;
  if (!CalculatorConversions::innerLoadWeylGroup(calculator, input, output)) {
    return false;
  }
  if (!output.isOfType<WeylGroupData>()) {
    return false;
  }
  WeylGroupData& theWeyl = output.getValueNonConst<WeylGroupData>();
  if (theWeyl.getDimension() > 12) {
    return calculator << "<hr>Computing sign signatures restricted up to rank 12.";
  }
  char type = 'X';
  int rank = - 1;
  if (!theWeyl.dynkinType.isSimple(&type, &rank)) {
    return calculator << "This function is implemented for simple classical Weyl groups only.";
  }
  if (type != 'A' && type != 'B' && type != 'C' && type != 'D') {
    return calculator << "You requested computation for type " << type
    << " but our formulas work only for classical types: A, B-C and D. ";
  }
  if (type == 'B' || type == 'C') {
    out << KostkaNumber::GetTypeBParabolicSignMultiplicityTable(rank);
  }
  if (type == 'A') {
    if ((false)) {
      int permutationSize = rank + 1;
      List<Partition> thePartitions, partitionsTransposed;
      Partition::GetPartitions(thePartitions, permutationSize);
      partitionsTransposed.setSize(thePartitions.size);
      for (int i = 0; i < thePartitions.size; i ++) {
        partitionsTransposed[i] = thePartitions[i];
        partitionsTransposed[i].transpose();
        out << "<br>Partition: " << thePartitions[i].toString()
        << ", transposed: " << partitionsTransposed[i].toString();
      }
    }
    return calculator << "Not implemented yet.";
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctionsWeylGroup::signSignatureRootSubsystems(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::signSignatureRootSubsystems");
  if (!CalculatorConversions::innerLoadWeylGroup(calculator, input, output)) {
    return false;
  }
  if (!output.isOfType<WeylGroupData>()) {
    return false;
  }
  WeylGroupData& theWeyl = output.getValueNonConst<WeylGroupData>();
  if (theWeyl.getDimension() > 8) {
    calculator << "<hr>Computing sign signatures restricted up to rank 8.";
    return false;
  }
  std::stringstream out;
  List<SubgroupDataRootReflections> parabolicSubgroupS, extendedParabolicSubgroups,
  allRootSubgroups, finalSubGroups;
  if (!theWeyl.loadSignSignatures(finalSubGroups)) {
    theWeyl.getSignSignatureParabolics(parabolicSubgroupS);
    theWeyl.getSignSignatureExtendedParabolics(extendedParabolicSubgroups);
    theWeyl.getSignSignatureAllRootSubsystems(allRootSubgroups);
    List<Pair<std::string, List<Rational>, MathRoutines::hashString> > tauSigPairs;
    finalSubGroups.reserve(allRootSubgroups.size);
    Pair<std::string, List<Rational>, MathRoutines::hashString> currentTauSig;
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
        currentTauSig.object1 = (*currentSGs)[i].theDynkinType.toString();
        currentTauSig.object2 = (*currentSGs)[i].tauSignature;
        if (!tauSigPairs.contains(currentTauSig)) {
          tauSigPairs.addOnTop(currentTauSig);
          finalSubGroups.addOnTop((*currentSGs)[i]);
        }
      }
    }
  }
  out << theWeyl.toStringSignSignatureRootSubsystem(finalSubGroups);
  return output.assignValue(out.str(), calculator);
}

template < >
GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& Expression::getValueNonConst() const;


bool CalculatorFunctionsWeylGroup::decomposeWeylRep(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::decomposeWeylRep");
  if (!input.isOfType<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> > ()) {
    return false;
  }
  GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& inputRep =
  input.getValueNonConst<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >();
  VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> outputRep;
  inputRep.makeGRCAM().decomposeTodorsVersion(outputRep);
  return output.assignValue(outputRep, calculator);
}

bool CalculatorFunctionsWeylGroup::isOuterAutoWeylGroup(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::isOuterAutoWeylGroup");
  if (input.size() != 3) {
    return calculator << "<hr>IsOuterAuto expects 2 arguments.";
  }
  DynkinType theType;
  if (!CalculatorConversions::functionDynkinType(calculator, input[1], theType)) {
    return calculator << "<hr>Failed to get Dynkin type from argument. " << input[1].toString();
  }
  Matrix<Rational> theMat;
  if (!calculator.functionGetMatrix(input[2], theMat)) {
    return calculator << "<hr>Failed to get matrix from argument. " << input[2].toString();
  }
  if (theMat.numberOfColumns != theMat.numberOfRows || theMat.numberOfColumns != theType.getRank()) {
    calculator << "<hr>Extracted Dynkin type " << theType.toString() << " is of rank " << theType.getRank()
    << " but extracted linear operator has " << theMat.numberOfColumns << " columns and " << theMat.numberOfRows << " rows.";
    return false;
  }
  WeylGroupData theWeyl;
  theWeyl.makeFromDynkinType(theType);
  theWeyl.computeRho(true);
  WeylGroupAutomorphisms theAutomorphisms;
  theAutomorphisms.theWeyl = &theWeyl;
  MatrixTensor<Rational> theOp;
  theOp = theMat;
  if (theAutomorphisms.isElementWeylGroupOrOuterAutomorphisms(theOp)) {
    return output.assignValue(1, calculator);
  }
  return output.assignValue(0, calculator);
}

bool CalculatorFunctionsWeylGroup::weylGroupNaturalRep(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::weylGroupNaturalRep");
  if (!CalculatorFunctionsWeylGroup::weylGroupConjugacyClasseS(calculator, input, output)) {
    return false;
  }
  if (!output.isOfType<WeylGroupData>()) {
    return false;
  }
  WeylGroupData& theGroup = output.getValueNonConst<WeylGroupData>();
  theGroup.computeInitialIrreducibleRepresentations();
  theGroup.theGroup.computeIrreducibleRepresentationsTodorsVersion();
  GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup>, Rational> tempRep;
  theGroup.getStandardRepresentation(tempRep);
  return output.assignValue(tempRep.makeOtherGroupRepresentationClass(), calculator);
}

class MonomialMacdonald {
public:
  SemisimpleLieAlgebra* owner;
  Selection rootSel;
  bool flagDeallocated;
  MonomialMacdonald(): owner(nullptr), flagDeallocated(false) {
  }
  std::string toString(FormatExpressions* format = nullptr) const;
  bool checkConsistency() {
    if (this->flagDeallocated) {
      global.fatal << "Use after free of MonomialMacdonald. " << global.fatal;
      return false;
    }
    return true;
  }
  static unsigned int hashFunction(const MonomialMacdonald& input) {
    return input.rootSel.hashFunction();
  }
  void operator=(const MonomialMacdonald& other) {
    this->owner = other.owner;
    this->rootSel = other.rootSel;
    this->flagDeallocated = other.flagDeallocated;
  }
  bool operator==(const MonomialMacdonald& other) const;
  void MakeFromRootSubsystem(const Vectors<Rational>& inputRoots, SemisimpleLieAlgebra& inputOwner);
  void ActOnMeSimpleReflection(int indexSimpleReflection, Rational& outputMultiple);
  void GenerateMyOrbit(HashedList<MonomialMacdonald>& output);
  ~MonomialMacdonald();
};

std::string MonomialMacdonald::toString(FormatExpressions* format) const {
  MacroRegisterFunctionWithName("MonomialMacdonald::toString");
  (void) format;
  if (this->owner == nullptr) {
    return "(non-initialized)";
  }
  if (this->rootSel.cardinalitySelection == 0) {
    return "1";
  }
  std::stringstream out;
  for (int i = 0; i < this->rootSel.cardinalitySelection; i ++) {
    out << "a_" << this->rootSel.elements[i] << "";
  }
  return out.str();
}

bool MonomialMacdonald::operator==(const MonomialMacdonald& other) const {
  return this->owner == other.owner && this->rootSel == other.rootSel;
}

MonomialMacdonald::~MonomialMacdonald() {
  this->flagDeallocated = true;
}

void MonomialMacdonald::GenerateMyOrbit(HashedList<MonomialMacdonald>& output) {
  MacroRegisterFunctionWithName("MonomialMacdonald::GenerateMyOrbit");
  output.clear();
  output.addOnTop(*this);
  MonomialMacdonald currentMon;
  Rational tempRat;
  for (int i = 0; i < output.size; i ++) {
    for (int j = 0; j < this->owner->getRank(); j ++) {
      currentMon = output[i];
      currentMon.ActOnMeSimpleReflection(j, tempRat);
      output.addOnTopNoRepetition(currentMon);
    }
  }
}

void MonomialMacdonald::MakeFromRootSubsystem(const Vectors<Rational>& inputRoots, SemisimpleLieAlgebra& inputOwner) {
  MacroRegisterFunctionWithName("MonomialMacdonald::MakeFromRootSubsystem");
  this->owner = &inputOwner;
  this->rootSel.initialize(inputOwner.weylGroup.rootSystem.size);
  Vector<Rational> currentV;
  for (int i = 0; i < inputRoots.size; i ++) {
    currentV = inputRoots[i];
    if (currentV.isNegative()) {
      currentV *= - 1;
    }
    int indexInRoots = inputOwner.weylGroup.rootSystem.getIndex(currentV);
    if (indexInRoots < 0) {
      global.fatal << "Attempt to make a Macdonald polynomial from "
      << inputRoots.toString()
      << ": the vector " << currentV.toString()
      << " is not a root. " << global.fatal;
    }
    this->rootSel.addSelectionAppendNewIndex(indexInRoots);
  }
  this->rootSel.computeIndicesFromSelection();
}

void MonomialMacdonald::ActOnMeSimpleReflection(int indexSimpleReflection, Rational& outputMultiple) {
  Selection originalSel;
  originalSel = this->rootSel;
  this->rootSel.initialize(this->owner->weylGroup.rootSystem.size);
  Vector<Rational> currentV;
  outputMultiple = 1;
  for (int i = 0; i < originalSel.cardinalitySelection; i ++) {
    currentV = this->owner->weylGroup.rootSystem[originalSel.elements[i]];
    this->owner->weylGroup.reflectSimple(indexSimpleReflection, currentV);
    if (currentV.isNegative()) {
      currentV *= - 1;
      outputMultiple *= - 1;
    }
    this->rootSel.addSelectionAppendNewIndex(this->owner->weylGroup.rootSystem.getIndex(currentV));
  }
}

bool CalculatorFunctionsWeylGroup::macdonaldPolys(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::macdonaldPolys");
  //note that if input is list of 2 elements then input[0] is sequence atom, and your two elements are in fact
  //input[1] and input[2];
  if (input.size() != 2) {
    return calculator << "Macdonald polynomials expects as input a single argument. ";
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!CalculatorConversions::convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, algebra
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  RootSubalgebras theRootSAs;
  theRootSAs.owner = algebra.content;
  theRootSAs.computeAllReductiveRootSubalgebrasUpToIsomorphism();
  std::stringstream out;
  MonomialMacdonald theGenerator;
  HashedList<MonomialMacdonald> theOrbit;
  for (int i = 0; i < theRootSAs.subalgebras.size; i ++) {
    RootSubalgebra& currentRootSA = theRootSAs.subalgebras[i];
    theGenerator.MakeFromRootSubsystem(currentRootSA.positiveRootsReductiveSubalgebra, *algebra.content);
    theGenerator.GenerateMyOrbit(theOrbit);
    out << "<hr>Root subsystem type " << currentRootSA.dynkinDiagram.toString();
    out << ". Orbit has " << theOrbit.size << " element(s), here they are: ";
    for (int j = 0; j < theOrbit.size; j ++) {
      out << "<br>" << theOrbit[j].toString();
    }
  }
  out << "Type: " << theRootSAs.owner->weylGroup.dynkinType.toString()
  << ". Number of root subsystems: " << theRootSAs.subalgebras.size;
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctionsWeylGroup::lieAlgebraWeight(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::lieAlgebraWeight");
  Weight<Polynomial<Rational> > resultWeight;
  if (input.size() != 4) {
    return false;
  }
  Expression tempE;
  SemisimpleLieAlgebra* theSSowner = nullptr;
  if (!CalculatorConversions::functionSemisimpleLieAlgebra(
    calculator, input[1], tempE, theSSowner
  )) {
    return calculator << "<hr>Failed to load semisimple Lie algebra";
  }
  std::string theCoordsString;
  bool isGood = input[3].isOperation(&theCoordsString);
  if (isGood) {
    isGood = (theCoordsString == "epsilon") || (theCoordsString == "fundamental") || (theCoordsString == "simple");
  }
  if (!isGood) {
    return calculator << "<hr>The third argument of MakeWeight is bad: must be one of the "
    << "keywords: epsilon, fundamental, simple. ";
  }
  int theWeightIndex = - 1;
  if (!input[2].isSmallInteger(&theWeightIndex)) {
    return false;
  }
  if (theCoordsString != "epsilon") {
    if (theWeightIndex < 1 || theWeightIndex> theSSowner->getRank()) {
      std::stringstream errorStream;
      errorStream << "The second argument of the MakeWeight function "
      << "needs to be index of a weight between 1 and the Lie algebra rank. "
      << "However, the index is " << theWeightIndex << ".";
      return output.makeError(errorStream.str(), calculator);
    }
    Vector<Polynomial<Rational> > EiVector;
    EiVector.makeEi(theSSowner->getRank(), theWeightIndex - 1);
    if (theCoordsString == "fundamental") {
      resultWeight.weightFundamentalCoordinates = EiVector;
    } else if (theCoordsString == "simple") {
      resultWeight.weightFundamentalCoordinates = theSSowner->weylGroup.getFundamentalCoordinatesFromSimple(EiVector);
    }
  } else {
    Vector<Rational> EiVector;
    EiVector.makeZero(theSSowner->getRank());
    Vector<Rational> tempV = theSSowner->weylGroup.getEpsilonCoordinates(EiVector);
    if (theWeightIndex>tempV.size || theWeightIndex < 1) {
      std::stringstream errorStream;
      errorStream << "The second argument of the MakeWeight function needs to be index of a weight between 1 and " << tempV.size
      << ". However, the index is " << theWeightIndex << ".";
      return output.makeError(errorStream.str(), calculator);
    }
    EiVector.makeEi(tempV.size, theWeightIndex - 1);
    resultWeight.weightFundamentalCoordinates = theSSowner->weylGroup.getFundamentalCoordinatesFromEpsilon(EiVector);
  }
  resultWeight.owner = theSSowner;
  ExpressionContext theContext(calculator);
  theContext.setAmbientSemisimpleLieAlgebra(*theSSowner);
  return output.assignValueWithContext(resultWeight, theContext, calculator);
}

bool CalculatorFunctionsWeylGroup::lieAlgebraRhoWeight(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::lieAlgebraRhoWeight");
  if (input.size() != 2) {
    return calculator << "Lie algebra rho weight expects a single argument. ";
  }
  Weight<Polynomial<Rational> > resultWeight;
  SemisimpleLieAlgebra* theSSowner = nullptr;
  Expression tempE;
  if (!CalculatorConversions::functionSemisimpleLieAlgebra(
    calculator, input[1], tempE, theSSowner
  )) {
    return calculator << "<hr>Failed to load semisimple Lie algebra. ";
  }
  theSSowner->checkConsistency();
  ExpressionContext theContext(calculator);
  theContext.setAmbientSemisimpleLieAlgebra(*theSSowner);
  resultWeight.weightFundamentalCoordinates = theSSowner->weylGroup.getFundamentalCoordinatesFromSimple(theSSowner->weylGroup.rho);
  resultWeight.owner = theSSowner;
  return output.assignValueWithContext(resultWeight, theContext, calculator);
}

bool CalculatorFunctionsWeylGroup::testSpechtModules(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::testSpechtModules");
  int theSymmetricGroupRank = 0;
  if (!input.isSmallInteger(&theSymmetricGroupRank)) {
    return calculator << "testSpechtModules called with input that is not a small integer, not performing any tests.";
  }
  if (theSymmetricGroupRank < 1) {
    return calculator << "testSpechtModules takes as input a small positive integer, instead the input I got was "
    << theSymmetricGroupRank << ". ";
  }
  if (theSymmetricGroupRank > 6) {
    return calculator
    << "For speed/memory reasons, testSpechtModules is currently restricted to take input no larger than 6. Your input was: "
    << theSymmetricGroupRank;
  }
  std::stringstream out;
  out << "User has requested the test of Specht modules of S_"
  << theSymmetricGroupRank << ". ";
  Partition::testAllSpechtModules(theSymmetricGroupRank);
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctionsWeylGroup::representElementHyperOctahedral(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::representElementHyperOctahedral");
  if (input.size() != 3) {
    return calculator << "Representating takes 2 arguments: element and rep.";
  }
  ElementHyperoctahedralGroupR2 theElt;
  GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> theRep;
  if (!input[1].isOfType(&theElt)) {
    return calculator << "Failed to extract element of hyperoctahedral group from " << input[1].toString();
  }
  if (!input[2].isOfType(&theRep)) {
    return calculator << "Failed to extract representation from " << input[2].toString();
  }
  Matrix<Rational> result;
  if (!theRep.getMatrixOfElement(theElt, result)) {
    return calculator << "Failed to get matrix of element " << theElt.toString()
    << " from representation: " << theRep.toString();
  }
  return output.assignMatrix(result, calculator, nullptr, false);
}

bool CalculatorFunctionsWeylGroup::hyperOctahedralGetOneRepresentation(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::hyperOctahedralGetOneRepresentation");
  Vector<Rational> inputLeftRat, inputRightRat;
  if (input.size() != 3) {
    return calculator << "CalculatorFunctionsWeylGroup::hyperOctahedralGetOneRepresentation needs two arguments";
  }
  if (!calculator.getVector(input[1], inputLeftRat)|| !calculator.getVector(input[2], inputRightRat)) {
    return false;
  }
  if (inputLeftRat.size < 1 || inputRightRat.size < 1) {
    return false;
  }
  const int maxPartitionSize = 1000;
  Vector<int> inputLeft, inputRight;
  Vector<int>* currentInput = &inputLeft;
  Vector<Rational>* currentInputRat = & inputLeftRat;
  for (
    int inputCounter = 0;
    inputCounter < 2;
    inputCounter ++, currentInput = &inputRight, currentInputRat = &inputRightRat
  ) {
    currentInput->setSize(currentInputRat->size);
    for (int i = 0; i < currentInputRat->size; i ++) {
      if (!(*currentInputRat)[i].isIntegerFittingInInt(&(*currentInput)[i])) {
        return calculator << "Failed to convert input: " << input.toString() << " to a list of small integers.";
      }
    }
    for (int i = 0; i < currentInput->size; i ++) {
      if ((*currentInput)[i] < 1 || (*currentInput)[i] > maxPartitionSize ) {
        return calculator << "Entry: " << (*currentInput)[i] << " of " << (*currentInput)
        << " is outside of the allowed input range.";
      }
    }
    if (currentInput->sumCoordinates() > maxPartitionSize) {
      return calculator << "The coordinates of vector " << (*currentInput) << " have sum that is too large. ";
    }
  }
  Partition partitionLeft, partitionRight;
  partitionLeft.fromListInt(inputLeft);
  partitionRight.fromListInt(inputRight);
  //std::stringstream out;
  //out << "Left partition is: " << partitionLeft.toString() << ", created from: " << inputLeft;
  //out << "Right partition is: " << partitionRight.toString() << ", created from: " << inputRight;
  int index = 0;
  for (; index < calculator.objectContainer.hyperOctahedralGroups.size; index ++) {
    if (
      calculator.objectContainer.hyperOctahedralGroups[index].flagIsEntireHyperoctahedralGroup &&
      calculator.objectContainer.hyperOctahedralGroups[index].N == partitionLeft.n + partitionRight.n
    ) {
      break;
    }
  }
  if (index == calculator.objectContainer.hyperOctahedralGroups.size) {
    calculator.objectContainer.hyperOctahedralGroups.setSize(
      calculator.objectContainer.hyperOctahedralGroups.size + 1
    );
    calculator.objectContainer.hyperOctahedralGroups[index].makeHyperoctahedralGroup(
      partitionLeft.n + partitionRight.n
    );
  }
  //<-may be broken if copying of groups doesn't work!!!!!!!!
  HyperoctahedralGroupData& HD = calculator.objectContainer.hyperOctahedralGroups[index];
  GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> R;
  HD.spechtModuleOfPartititons(partitionLeft, partitionRight, R);
  //out << R;
  R.computeCharacter();
  return output.assignValue(R, calculator);
}

bool CalculatorFunctionsWeylGroup::spechtModule(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::spechtModule");
  Vector<Rational> inputRat;
  if (!calculator.getVectorFromFunctionArguments(input, inputRat)) {
    return false;
  }
  if (inputRat.size < 1) {
    return false;
  }
  const int maxPartitionSize = 1000;
  Vector<int> inputInt;
  inputInt.setSize(inputRat.size);
  for (int i = 0; i < inputRat.size; i ++) {
    if (!(inputRat[i].isIntegerFittingInInt(&inputInt[i]))) {
      return calculator << "Failed to convert input: " << input.toString() << " to a list of small integers.";
    }
  }
  for (int i = 0; i < inputInt.size; i ++) {
    if ((inputInt[i] < 1) || (inputInt[i] > maxPartitionSize)) {
      return calculator <<  "Entry: " << inputInt[i] << " of " << inputInt << " is outside of the allowed input range.";
    }
  }
  if (inputInt.sumCoordinates() > maxPartitionSize) {
    return calculator << "The coordinates of vector " << inputInt << " have sum that is too large. ";
  }
  Partition p;
  p.fromListInt(inputInt);
  std::stringstream out;
  out << "Partition is " << p.toString();
  List<Matrix<Rational> > gens;
  p.spechtModuleMatricesOfTranspositionsjjplusone(gens);
  for (int i = 0; i < gens.size; i ++) {
    out << i << "\n" << gens[i].toStringPlainText() << "\n";
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctionsWeylGroup::hyperOctahedralAllModulesInducedFromSpechtModules(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::hyperOctahedralAllModulesInducedFromSpechtModules");
  if (input.size() != 2) {
    return false;
  }
  int rank = 0;
  if (!input[1].isSmallInteger(&rank)) {
    return false;
  }
  if (rank < 1|| rank > 7) {
    return calculator << "Input of hyperoctahedral print function has to be between 1 and 10";
  }
  HyperoctahedralGroupData groupData;
  groupData.makeHyperoctahedralGroup(rank);
  groupData.allSpechtModules();
  return output.assignValue(groupData.theGroup->prettyPrintCharacterTable(), calculator);
}

bool CalculatorFunctionsWeylGroup::hyperOctahedralPrintGeneratorCommutationRelations(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::hyperOctahedralPrintGeneratorCommutationRelations");
  if (input.size() != 2) {
    return false;
  }
  int rank = 0;
  if (!input[1].isSmallInteger(&rank)) {
    return false;
  }
  if (rank < 1 || rank > 10) {
    return calculator << "Input of hyperoctahedral print function has to be between 1 and 10";
  }
  HyperoctahedralGroupData groupData;
  groupData.makeHyperoctahedralGroup(rank);
  return output.assignValue(groupData.theGroup->prettyPrintGeneratorCommutationRelations(), calculator);
}

bool CalculatorFunctionsWeylGroup::weylGroupElement(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::weylGroupElement");
  if (input.size() < 2) {
    return output.makeError("Function WeylElement needs to know what group the element belongs to", calculator);
  }
  WithContext<SemisimpleLieAlgebra*> thePointer;
  if (!CalculatorConversions::convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, thePointer
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  ElementWeylGroup theElt;
  theElt.generatorsLastAppliedFirst.reserve(input.size() - 2);
  for (int i = 2; i < input.size(); i ++) {
    int tmp;
    if (!input[i].isSmallInteger(& tmp)) {
      return false;
    }
    theElt.multiplyOnTheRightBySimpleReflection(tmp - 1);
  }
  theElt.owner = &thePointer.content->weylGroup;
  for (int i = 0; i < theElt.generatorsLastAppliedFirst.size; i ++) {
    if (
      theElt.generatorsLastAppliedFirst[i].index >= thePointer.content->getRank() ||
      theElt.generatorsLastAppliedFirst[i].index < 0
    ) {
      return output.makeError("Bad reflection index", calculator);
    }
  }
  theElt.makeCanonical();
  return output.assignValue(theElt, calculator);
}

template <typename somegroup, typename Coefficient>
void VirtualRepresentation<somegroup, Coefficient>::operator*=(const VirtualRepresentation<somegroup, Coefficient>& other) {
  MacroRegisterFunctionWithName("VirtualRepresentation::operator*=");
  (void) other;
  global.fatal << "Not implemented yet. " << global.fatal;
/*  WeylGroupVirtualRepresentation<Coefficient> output, currentContribution;
  output.ownerGroup = this->ownerGroup;
  output.coefficientsIrreps.makeZero(this->coefficientsIrreps.size);
  WeylGroupRepresentation<Rational> tempRep;
  for (int i = 0; i < this->coefficientsIrreps.size; i ++)
    for (int j = 0; j<other.coefficientsIrreps.size; j ++) {
      Rational theCoeff= this->coefficientsIrreps[i]*other.coefficientsIrreps[j];
      if (theCoeff== 0)
        continue;
      tempRep = this->ownerGroup->irreps[i];
      tempRep*= this->ownerGroup->irreps[j];
      tempRep.decomposeTodorsVersion(currentContribution, 0);
      output.coefficientsIrreps+= currentContribution*theCoeff;
    }
  *this = output;*/
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
void VirtualRepresentation<somegroup, Coefficient>::assignRepresentation(const GroupRepresentation<somegroup, Rational>& other) {
  VirtualRepresentation<somegroup, Coefficient> out;
  out.addMonomial(other.theCharacter, 1);
//  otherCopy.decomposeTodorsVersion(this->coefficientsIrreps, global);
}

bool CalculatorFunctionsWeylGroup::makeVirtualWeylRep(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::makeVirtualWeylRep");
  if (input.isOfType<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >()) {
    output = input;
    return true;
  }
  if (!input.isOfType<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >()) {
    return false;
  }
  GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& inputRep =
  input.getValueNonConst<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >();
  WeylGroupData* theWeylData = inputRep.ownerGroup->generators[0].owner;
  if (
    inputRep.ownerGroup->irreps.size<inputRep.ownerGroup->conjugacyClassCount() &&
    theWeylData->theGroup.ComputeIrreducibleRepresentationsWithFormulas
  ) {
    theWeylData->theGroup.ComputeIrreducibleRepresentationsWithFormulas(theWeylData->theGroup);
  }
  if (inputRep.ownerGroup->irreps.size<inputRep.ownerGroup->conjugacyClassCount()) {
    theWeylData->computeInitialIrreducibleRepresentations();
    theWeylData->theGroup.computeIrreducibleRepresentationsTodorsVersion();
  }
  VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> outputRep;
  outputRep.assignRepresentation(inputRep);
  return output.assignValue(outputRep, calculator);
}
