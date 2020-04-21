#include "calculator.h"
#include "math_general_implementation.h"
#include "math_extra_finite_groups.h"
#include "math_extra_finite_groups_implementation.h"
#include "math_extra_symmetric_groups_and_generalizations.h"
#include "math_extra_graph.h"
#include "calculator_Weyl_group_characters.h"
#include "math_extra_universal_enveloping_implementation.h"

template<>
List<ClassFunction<WeylGroupData::WeylGroupBase, Rational> >::Comparator*
FormatExpressions::GetMonOrder<ClassFunction<WeylGroupData::WeylGroupBase, Rational> >() {
  return nullptr;
}

template < >
WeylGroupData& Expression::GetValueNonConst() const;

bool WeylGroupData::CheckConsistency() const {
  //if (this == 0)
  //  global.fatal << "The this pointer of a Weyl group is zero. " << global.fatal;
  if (this->flagDeallocated) {
    global.fatal << "This is a programming error: use after free of WeylGroup. " << global.fatal;
  }
  for (int i = 0; i < this->theGroup.generators.size; i ++) {
    this->theGroup.generators[i].CheckConsistency();
  }
  this->RootsOfBorel.CheckConsistency();
  return true;
}

template <typename elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::CheckInitializationFDrepComputation() const {
  if (this->theElements.size == 0) {
    global.fatal << "This is a programming error: requesting to compute character hermitian product in a group whose "
    << "conjugacy classes and/or elements have not been computed. The group reports to have "
    << this->ConjugacyClassCount() << " conjugacy classes and " << this->theElements.size << " elements. "
    << global.fatal;
    return false;
  }
  return this->CheckInitializationConjugacyClasses();
}

template <typename somegroup, typename coefficient>
bool GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::CheckAllSimpleGensAreOK() const {
  this->CheckInitialization();
  for (int i = 0; i < this->ownerGroup->generators.size; i ++) {
    if (this->generatorS[i].NumRows == 0) {
      global.fatal << "This is a programming error: working with a "
      << "representation in which the action of the simple generators is not computed. " << global.fatal;
      return false;
    }
  }
  return true;
}

/*template <typename somegroup, typename coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::CheckRepIsMultiplicativelyClosed() {
  HashedList<Matrix<Rational> > tempList;
  tempList.addOnTop(this->theElementImages);
  Matrix<Rational> tempMat;
  ElementWeylGroup tempElt;
  for (int i = 0; i < tempList.size; i ++) {
    for (int j = 0; j < tempList.size; j ++) {
      tempMat = tempList[i];
      tempMat.MultiplyOnTheLeft(tempList[j]);
      tempElt = this->ownerGroup->theElements[j];
      tempElt *= this->ownerGroup->theElements[i];
      tempElt.MakeCanonical();
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
template <typename somegroup, typename coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::ComputeAllGeneratorImagesFromSimple(GlobalVariables* global) {
  MacroRegisterFunctionWithName("GroupRepresentationCarriesAllMatrices::ComputeAllGeneratorImagesFromSimple");
  this->CheckInitialization();
  this->ownerGroup->CheckInitializationFDrepComputation();
  HashedList<ElementWeylGroup> ElementsExplored;
  ElementsExplored.setExpectedSize(this->ownerGroup->theElements.size);
  this->theElementImageS[0].MakeIdMatrix(this->getDimension());
  ElementWeylGroup currentElt;
  int theRank = this->ownerGroup->getDimension();
  currentElt.makeIdentity(*this->ownerGroup);
  ElementsExplored.addOnTop(currentElt);
  List<ElementWeylGroup> theGens;
  theGens.setSize(theRank);
  for (int i = 0; i < theRank; i ++)
    theGens[i].MakeSimpleReflection(i, *this->ownerGroup);
  for (int i = 0; i <ElementsExplored.size; i ++) {
    int indexParentElement = this->ownerGroup->theElements.getIndex(ElementsExplored[i]);
    for (int j = 0; j < theRank; j ++) {
      currentElt = theGens[j]* ElementsExplored[i];
      if (!ElementsExplored.contains(currentElt)) {
        int indexCurrentElt = this->ownerGroup->theElements.getIndex(currentElt);
        this->theElementIsComputed[indexCurrentElt] = true;
        this->theElementImageS[indexParentElement].MultiplyOnTheLeft(this->generatorS[j], this->theElementImageS[indexCurrentElt]);
        ElementsExplored.addOnTop(currentElt);
      }
    }
  }
//  this->CheckRepIsMultiplicativelyClosed();
  //FormatExpressions tempFormat;
  //tempFormat.flagUseLatex = true;
  //for (int i = 0; i < this->theElementImages.size; i ++)
  //{
  //}
}
*/

// This method uses auto everywhere, and a lot of copying data in order to use auto
template <typename somegroup, typename coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::ComputeAllElementImages() {
  MacroRegisterFunctionWithName("GroupRepresentationCarriesAllMatrices::ComputeAllGeneratorImagesFromSimple");
  this->CheckInitialization();
  this->ownerGroup->CheckInitializationFDrepComputation();
  auto ElementsExplored = this->ownerGroup->theElements;
  ElementsExplored.clear();
  ElementsExplored.setExpectedSize(this->ownerGroup->theElements.size);
  this->theElementImageS[0].MakeIdMatrix(this->getDimension());
  auto currentElt = this->ownerGroup->generators[0];
  currentElt.makeIdentity(this->ownerGroup->generators[0]);
  int theRank = this->ownerGroup->generators.size;
  auto& theGens = this->ownerGroup->generators;

  ElementsExplored.addOnTop(currentElt);
  for (int i = 0; i < ElementsExplored.size; i ++) {
    int indexParentElement = this->ownerGroup->theElements.getIndex(ElementsExplored[i]);
    for (int j = 0; j < theRank; j ++) {
      currentElt = theGens[j] * ElementsExplored[i];
      if (!ElementsExplored.contains(currentElt)) {
        int indexCurrentElt = this->ownerGroup->theElements.getIndex(currentElt);
        this->theElementIsComputed[indexCurrentElt] = true;
        this->theElementImageS[indexParentElement].MultiplyOnTheLeft(this->generatorS[j], this->theElementImageS[indexCurrentElt]);
        ElementsExplored.addOnTop(currentElt);
      }
    }
  }
//  this->CheckRepIsMultiplicativelyClosed();
  //FormatExpressions tempFormat;
  //tempFormat.flagUseLatex = true;
  //for (int i = 0; i < this->theElementImages.size; i ++)
  //{   //}
}

template <typename somegroup, typename coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::operator*=(
  const GroupRepresentationCarriesAllMatrices<somegroup, coefficient>& other
) {
  MacroRegisterFunctionWithName("WeylGroupRepresentation::operator*=");
  //lazy programmers handling://////
  if (&other == this ) {
    GroupRepresentationCarriesAllMatrices<somegroup, coefficient> otherCopy;
    otherCopy = other;
    *this *= otherCopy;
    return;
  }
  //////////////////////////////////
  if (this->ownerGroup != other.ownerGroup) {
    global.fatal << "This is a programming error: attempting to multiply representations with different owner groups. " << global.fatal;
  }
  if (!this->flagCharacterIsComputed || !other.flagCharacterIsComputed) {
    global.fatal << "Attempting to multiply weyl group reps whose characters have not been computed. " << global.fatal;
  }
  GroupRepresentationCarriesAllMatrices<somegroup, coefficient> output;
  output.init(*this->ownerGroup);
  output.theCharacteR = this->theCharacteR;
  output.theCharacteR *= other.theCharacteR;
  for (int i = 0; i < output.generatorS.size; i ++) {
    output.generatorS[i].AssignTensorProduct(this->generatorS[i], other.generatorS[i]);
  }
  for (int i = 0; i < output.theElementImageS.size; i ++) {
    if (this->theElementIsComputed[i] && other.theElementIsComputed[i]) {
      output.theElementImageS[i].AssignTensorProduct(this->theElementImageS[i], other.theElementImageS[i]);
      output.theElementIsComputed[i] = true;
    }
  }
  *this = output;
}

template <typename somegroup, typename coefficient>
void GroupRepresentation<somegroup, coefficient>::operator*=(const GroupRepresentation<somegroup, coefficient>& other) {
  MacroRegisterFunctionWithName("GroupRepresentation::operator*=");
  //lazy programmers handling://////
  if (&other == this ) {
    GroupRepresentation<somegroup, coefficient> otherCopy;
    otherCopy = other;
    *this *= otherCopy;
    return;
  }
  //////////////////////////////////
  if (this->ownerGroup != other.ownerGroup) {
    global.fatal << "This is a programming error: attempting to multiply representations with different owner groups. " << global.fatal;
  }
  if (!this->flagCharacterIsComputed || !other.flagCharacterIsComputed) {
    global.fatal << "Attempting to multiply weyl group reps whose characters have not been computed. " << global.fatal;
  }
  GroupRepresentation<somegroup, coefficient> output;
  output.ownerGroup = this->ownerGroup;
  output.theCharacteR = this->theCharacteR;
  output.theCharacteR *= other.theCharacteR;
  for (int i = 0; i < output.generatorS.size; i ++) {
    output.generatorS[i].AssignTensorProduct(this->generatorS[i], other.generatorS[i]);
  }
  *this = output;
}

template <typename somegroup, typename coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::Restrict(
  const Vectors<coefficient>& VectorSpaceBasisSubrep,
  const ClassFunction<somegroup, Rational>& remainingCharacter,
  GroupRepresentationCarriesAllMatrices<somegroup, coefficient>& output
) {
  MacroRegisterFunctionWithName("WeylGroupRepresentation::Restrict");
  this->CheckAllSimpleGensAreOK();
  if (VectorSpaceBasisSubrep.size == 0) {
    global.fatal << "This is a programming error: restriction of "
    << "representation to a zero subspace is not allowed. " << global.fatal;
  }
  output.init(*this->ownerGroup);
  output.basis = VectorSpaceBasisSubrep;
  output.basis.GetGramMatrix(output.gramMatrixInverted, 0);
  output.gramMatrixInverted.Invert();
  output.theCharacteR = remainingCharacter;
  ProgressReport theReport;
  for (int i = 0; i < this->generatorS.size; i ++) {
    if (theReport.TickAndWantReport()) {
      std::stringstream reportStream;
      reportStream << "Restricting the action of generator of index " << i;
      theReport.report(reportStream.str());
    }
    Matrix<coefficient>::MatrixInBasis(this->generatorS[i], output.generatorS[i], output.basis, output.gramMatrixInverted);
  }
  /*
  for (int i = 0; i < this->classFunctionMatrices.size; i ++)
    if (this->classFunctionMatricesComputed[i]) {
      output.classFunctionMatricesComputed[i] = true;
      if (global != 0) {
        std::stringstream reportStream;
        reportStream << "Restricting class function matrix " << i + 1 << " out of "
        << this->classFunctionMatrices.size;
        theReport.report(reportStream.str());
      }
      Matrix<coefficient>::MatrixInBasis
      (this->classFunctionMatrices[i], output.classFunctionMatrices[i], output.vectorSpaceBasis,
       output.gramMatrixInverted);
    }
  */
  output.CheckAllSimpleGensAreOK();
}

template <typename somegroup, typename coefficient>
bool GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::DecomposeTodorsVersion(
  VirtualRepresentation<somegroup, coefficient>& outputIrrepMults,
  List<GroupRepresentationCarriesAllMatrices<somegroup, coefficient> >* appendOnlyGRCAMSList
) {
  MacroRegisterFunctionWithName("WeylGroupRepresentation::DecomposeTodorsVersion");
  this->CheckInitialization();
  this->ownerGroup->CheckInitializationFDrepComputation();
  outputIrrepMults.makeZero();
  List<GroupRepresentation<somegroup, coefficient> > appendOnlyIrrepsList;
  for (int i = 0; i < this->ownerGroup->irreps.size; i ++) {
    appendOnlyIrrepsList.addOnTop(this->ownerGroup->irreps[i]);
  }
  return this->DecomposeTodorsVersionRecursive(outputIrrepMults, appendOnlyIrrepsList, appendOnlyGRCAMSList);
}

template <class Element>
bool Matrix<Element>::GetEigenspacesProvidedAllAreIntegralWithEigenValueSmallerThanDim(List<Vectors<Element> >& output) const {
  int upperLimitComputations = 100000;
  output.setSize(0);
  int found = 0;
  Polynomial<Element> theMinPoly;
  theMinPoly.AssignMinPoly(*this);
  Vector<Element> theEigenValueCandidate;
  theEigenValueCandidate.setSize(1);
  Matrix<Rational> tempMat;
  for (int ii = 0; ii < upperLimitComputations; ii ++) {
    int i = ((ii + 1) / 2) * (2 * (ii % 2) - 1); // 0, 1, - 1, 2, - 2, 3, - 3,...
    theEigenValueCandidate[0] = i;
    if (theMinPoly.Evaluate(theEigenValueCandidate) == 0) {
      tempMat = *this;
      output.setSize(output.size + 1);
      tempMat.GetEigenspaceModifyMe(theEigenValueCandidate[0], *output.LastObject());
      if (output.LastObject()->size == 0) {
        global.fatal << "This is a programmig error: " << theEigenValueCandidate[0].toString()
        << " is a zero of the minimal polynomial "
        << theMinPoly.toString() << " of the operator " << this->toString()
        << " but the corresponding eigenspace is empty. " << global.fatal;
      }
      found += output.LastObject()->size;
      if (found == this->NumCols) {
        return true;
      }
    }
  }
  return false;
}

void WeylGroupData::ComputeIrreducibleRepresentationsWithFormulasImplementation(FiniteGroup<ElementWeylGroup> &G) {
  List<char> letters;
  List<int> ranks;
  WeylGroupData& WD = *(G.generators[0].owner);
  WD.theDynkinType.GetLettersTypesMults(&letters, &ranks, nullptr);
  // When WeylGroupRepresentation is merged with GroupRepresentation,
  // and WeylGroupData split from FiniteGroup<ElementWeylGroup<WeylGroup> >
  // theRepresentations will be this->theGroup->irreps
  // currently we have the difficulty with GroupRepresentation<WeylGroup, coefficient> vs
  // GroupRepresentation<FiniteGroup<ElementWeylGroup<WeylGroup> >, coefficient>
  if ((letters.size == 1) && (letters[0] == 'A')) {
    int theRank = ranks[0];
    List<Partition> thePartitions;
    Partition::GetPartitions(thePartitions,theRank + 1);
    for (int i = 0; i < thePartitions.size; i ++) {
      GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> irrep;
      thePartitions[i].SpechtModuleMatricesOfTranspositionsjjplusone(irrep.generatorS);
      irrep.ownerGroup = &G;
      irrep.identifyingString = thePartitions[i].toString();
      irrep.ComputeCharacter();
      G.AddIrreducibleRepresentation(irrep);
    }
  } else if ((letters.size == 1) && (letters[0] == 'B')) {
    int theRank = ranks[0];
    HyperoctahedralGroupData HOG;
    HOG.MakeHyperoctahedralGroup(theRank);
    HOG.AllSpechtModules();
    GroupHomomorphism<ElementWeylGroup, ElementHyperoctahedralGroupR2> phi;
    phi.preimageGroup = &G;
    phi.generatorImages.setSize(G.generators.size);
    for (int i = 0; i < phi.generatorImages.size - 1; i ++) {
      phi.generatorImages[i].h.AddTransposition(i, i + 1);
    }
    for (int i = 0; i < phi.generatorImages.size - 1; i ++) {
      phi.generatorImages.LastObject()->k.ToggleBit(i);
    }
    global << "Generator commutation relations of groups\n";
    global << HOG.theGroup->PrettyPrintGeneratorCommutationRelations();
    FiniteGroup<ElementHyperoctahedralGroupR2> phiG;
    phiG.generators = phi.generatorImages;
    global << phiG.PrettyPrintGeneratorCommutationRelations();
    global << G.PrettyPrintGeneratorCommutationRelations();
    global << "pulling back irreps:\n";
    for (int i = 0; i < HOG.theGroup->irreps.size; i ++) {
      auto irrep = phi.PullbackRepresentation(HOG.theGroup->irreps[i]);
      irrep.ComputeCharacter();
      global << HOG.theGroup->irreps[i].theCharacteR << "->" << irrep.theCharacteR << '\n';
      G.AddIrreducibleRepresentation(irrep);
    }
  } else if ((letters.size == 1) && (letters[0] == 'D')) {
    int theRank = ranks[0];
    HyperoctahedralGroupData HOG;
    HOG.MakeHyperoctahedralGroup(theRank + 1);
    GroupHomomorphism<ElementWeylGroup, ElementHyperoctahedralGroupR2> inclusionMap;
    inclusionMap.preimageGroup = &G;
    inclusionMap.generatorImages.setSize(G.generators.size);
    for (int i = 0; i < inclusionMap.generatorImages.size - 1; i ++) {
      inclusionMap.generatorImages[i].h.AddTransposition(i, i + 1);
      inclusionMap.generatorImages[i].k.ToggleBit(i);
      inclusionMap.generatorImages[i].k.ToggleBit(i + 1);
    }
    int oneortwo = (inclusionMap.generatorImages.size + 1) % 2;
    for (int i = 0; i < inclusionMap.generatorImages.size - 1 - oneortwo; i ++) {
      inclusionMap.generatorImages.LastObject()->k.ToggleBit(i);
    }

    FiniteGroup<ElementHyperoctahedralGroupR2> imG;
    imG.generators = inclusionMap.generatorImages;
    global << HOG.theGroup->PrettyPrintGeneratorCommutationRelations() << '\n';
    global << imG.PrettyPrintGeneratorCommutationRelations();
    global << G.PrettyPrintGeneratorCommutationRelations() << '\n';
  }
  // silently fail instead of crashing to support calling into this and if it doesn't
  // work then using brute force
  //  else
  //    global.fatal << "ComputeIrreducibleRepresentationsUsingSpechtModules: Type "
  // << this->theDynkinType << " is unsupported.  If you think it should work, edit " << __FILE__ << ":" << __LINE__ << global.fatal;
  global << G.PrettyPrintCharacterTable() << '\n';
}

template <class coefficient>
void WeylGroupData::RaiseToMaximallyDominanT(List<Vector<coefficient> >& theWeights) {
  bool found;
  for (int i = 0; i < theWeights.size; i ++) {
    do {
      found = false;
      for (int j = 0; j < this->RootsOfBorel.size; j ++) {
        if (this->RootScalarCartanRoot(this->RootsOfBorel[j], theWeights[i]) < 0) {
          bool isGood = true;
          for (int k = 0; k < i; k ++) {
            if (this->RootScalarCartanRoot(this->RootsOfBorel[j], theWeights[k]) > 0) {
              isGood = false;
              break;
            }
          }
          if (!isGood) {
            continue;
          }
          for (int k = 0; k < theWeights.size; k ++) {
            this->ReflectBetaWRTAlpha(this->RootsOfBorel[j], theWeights[k], false, theWeights[k]);
          }
          found = true;
        }
      }
    } while (found);
  }
}

bool CalculatorFunctionsWeylGroup::innerWeylRaiseToMaximallyDominant(
  Calculator& theCommands, const Expression& input, Expression& output, bool useOuter
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylRaiseToMaximallyDominant");
  if (input.children.size < 2) {
    return output.MakeError("Raising to maximally dominant takes at least 2 arguments, type and vector", theCommands);
  }
  const Expression& semisimpleLieAlgebraNode = input[1];
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebra;
  if (!theCommands.Convert(
    semisimpleLieAlgebraNode,
    CalculatorConversions::functionSemisimpleLieAlgebra,
    semisimpleLieAlgebra
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  SemisimpleLieAlgebra* theSSalgebra = semisimpleLieAlgebra.content;
  Vectors<Rational> theHWs;
  theHWs.setSize(input.children.size - 2);
  bool isGood = true;
  for (int i = 2; i < input.size(); i ++) {
    if (!theCommands.GetVector<Rational>(
      input[i], theHWs[i - 2], nullptr, theSSalgebra->GetRank()
    )) {
      isGood = false;
      break;
    }
  }
  if (!isGood && input.children.size == 3) {
    Matrix<Rational> theHWsMatForm;
    if (theCommands.functionGetMatrix(
      input[2], theHWsMatForm, nullptr, theSSalgebra->GetRank()
    )) {
      theHWsMatForm.GetVectorsFromRows(theHWs);
      isGood = true;
    }
  }
  if (theHWs.size == 0 || !isGood) {
    return output.MakeError("Failed to extract rational vectors from expression " + input.toString() + ". ", theCommands);
  }
  std::stringstream out;
  out << "Input: " << theHWs.toString()
  << ", simultaneously raising to maximally dominant in Weyl group of type "
  << theSSalgebra->theWeyl.theDynkinType.toString();
  if (!useOuter) {
    theSSalgebra->theWeyl.RaiseToMaximallyDominanT(theHWs);
  } else {
    WeylGroupAutomorphisms theOuterAutos;
    theOuterAutos.theWeyl = &theSSalgebra->theWeyl;
    theOuterAutos.RaiseToMaximallyDominant(theHWs);
  }
  out << "<br>Maximally dominant output: " << theHWs.toString();
  return output.AssignValue(out.str(), theCommands);
}


template <>
bool CalculatorConversions::functionPolynomial<Rational>(Calculator& theCommands, const Expression& input, Expression& output);

bool CalculatorFunctionsWeylGroup::innerWeylGroupOrbitOuterSimple(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupOrbitOuterSimple");
  if (!input.IsListNElements(3)) {
    return output.MakeError("innerWeylOrbit takes two arguments", theCommands);
  }
  const Expression& theSSalgebraNode = input[1];
  const Expression& vectorNode = input[2];
  DynkinType theType;
  if (theSSalgebraNode.IsOfType<SemisimpleLieAlgebra*>()) {
    SemisimpleLieAlgebra* theAlgebra = theSSalgebraNode.getValue<SemisimpleLieAlgebra*>();
    theType = theAlgebra->theWeyl.theDynkinType;
  } else {
    if (!CalculatorConversions::functionDynkinType(
      theCommands, theSSalgebraNode, theType
    )) {
      return false;
    }
  }
  Vector<Polynomial<Rational> > theHWfundCoords, theHWsimpleCoords;
  ExpressionContext theContext(theCommands);
  if (!theCommands.GetVector(
    vectorNode,
    theHWfundCoords,
    &theContext,
    theType.GetRank(),
    CalculatorConversions::functionPolynomial<Rational>
  )) {
    return output.MakeError("Failed to extract highest weight", theCommands);
  }
  WeylGroupData theWeyl;
  theWeyl.MakeFromDynkinType(theType);
  theHWsimpleCoords = theHWfundCoords;
  theHWfundCoords = theWeyl.GetFundamentalCoordinatesFromSimple(theHWsimpleCoords);
  std::stringstream out, latexReport;
  Vectors<Polynomial<Rational> > theHWs;
  FormatExpressions theFormat = theContext.getFormat();
  theHWs.addOnTop(theHWsimpleCoords);
  HashedList<Vector<Polynomial<Rational> > > outputOrbit;
  WeylGroupAutomorphisms theOuterAutos;
  theOuterAutos.theWeyl = &theWeyl;
  if (!theOuterAutos.GenerateOuterOrbit(
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
  theWeyl.theDynkinType.GetEpsilonMatrix(epsCoordMat);
  for (int i = 0; i < outputOrbit.size; i ++) {
    theFormat.simpleRootLetter = "\\alpha";
    theFormat.fundamentalWeightLetter = "\\psi";
    std::string orbitEltString = outputOrbit[i].toString(&theFormat);
    Vector<Polynomial<Rational> > epsVect = outputOrbit[i];
    epsCoordMat.ActOnVectorColumn(epsVect);
    std::string orbitEltStringEpsilonCoords = epsVect.ToStringLetterFormat("\\varepsilon", &theFormat);
    std::string weightEltString =
    theWeyl.GetFundamentalCoordinatesFromSimple(outputOrbit[i]).ToStringLetterFormat
    (theFormat.fundamentalWeightLetter, &theFormat);
    out << "<tr>" << "<td>"
    << (useMathTag ? HtmlRoutines::GetMathSpanPure(theOuterAutos.allElements[i].toString()) : theOuterAutos.allElements[i].toString())
    << "</td><td>" << (useMathTag ? HtmlRoutines::GetMathSpanPure(orbitEltString) : orbitEltString) << "</td><td>"
    << (useMathTag ? HtmlRoutines::GetMathSpanPure(orbitEltStringEpsilonCoords) : orbitEltStringEpsilonCoords)
    << "</td><td>" << (useMathTag ? HtmlRoutines::GetMathSpanPure(weightEltString) : weightEltString) << "</td>";
    latexReport << "$" << theOuterAutos.allElements[i].toString(&theFormat) << "$ & $" << orbitEltStringEpsilonCoords << "$ & $"
    << weightEltString << "$ & $" << (outputOrbit[0]-outputOrbit[i]).ToStringLetterFormat(theFormat.simpleRootLetter, &theFormat) << "$\\\\\n<br>";
    out << "</tr>";
  }
  latexReport << "\\end{longtable}";
  out << "</table>" << "<br> " << latexReport.str();
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupOrbitSize(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupOrbitSize");
  //double startTimeForDebug= global.GetElapsedSeconds();
  WithContext<SemisimpleLieAlgebra*> theAlgebra;
  Vector<Rational> theWeightRat;
  if (theCommands.GetTypeWeight<Rational>(
    theCommands, input, theWeightRat, theAlgebra, nullptr
  )) {
    Rational result = theAlgebra.content->theWeyl.GetOrbitSize(theWeightRat);
    return output.AssignValue(result, theCommands);
  }
  SemisimpleLieAlgebra* theSSalgebra = theAlgebra.content;
  Vector<Polynomial<Rational> > theWeightPoly;
  if (theCommands.GetTypeWeight<Polynomial<Rational> >(
    theCommands,
    input,
    theWeightPoly,
    theAlgebra,
    CalculatorConversions::functionPolynomial<Rational>
  )) {
    Rational result = theSSalgebra->theWeyl.GetOrbitSize(theWeightPoly);
    return output.AssignValue(result, theCommands);
  }
  return false;
}

bool CalculatorFunctionsWeylGroup::innerWeylOrbit(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  bool useFundCoords,
  bool useRho
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylOrbit");
  if (!input.IsListNElements(3)) {
    return output.MakeError("innerWeylOrbit takes two arguments", theCommands);
  }
  WithContext<SemisimpleLieAlgebra*> theSSalgebra;
  Vector<Polynomial<Rational> > theWeight;
  if (!theCommands.GetTypeWeight(
    theCommands,
    input,
    theWeight,
    theSSalgebra,
    CalculatorConversions::functionPolynomial<Rational>
  )) {
    return false;
  }
  Vector<Polynomial<Rational> > theHWfundCoords, theHWsimpleCoords, currentWeight;
  WeylGroupData& theWeyl = theSSalgebra.content->theWeyl;
  if (!useFundCoords) {
    theHWsimpleCoords = theWeight;
    theHWfundCoords = theWeyl.GetFundamentalCoordinatesFromSimple(theWeight);
  } else {
    theHWfundCoords = theWeight;
    theHWsimpleCoords = theWeyl.GetSimpleCoordinatesFromFundamental(theWeight);
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
  if (!theSSalgebra.content->theWeyl.GenerateOrbit(theHWs, useRho, outputOrbit, false, 1921, &orbitGeneratingSet, 1921)) {
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
  theWeyl.theDynkinType.GetEpsilonMatrix(epsCoordMat);
  GraphWeightedLabeledEdges integralPositiveRootReflectionGraph;
  integralPositiveRootReflectionGraph.numNodes = outputOrbit.size;
  integralPositiveRootReflectionGraph.nodeLabels.setSize(outputOrbit.size);
  theFormat.flagUseFrac = true;
  for (int i = 0; i < outputOrbit.size; i ++) {
    integralPositiveRootReflectionGraph.nodeLabels[i] =
    "$" + theWeyl.GetEpsilonCoords(outputOrbit[i]).ToStringEpsilonFormat(&theFormat) + "$ = $" +
    theWeyl.GetFundamentalCoordinatesFromSimple(outputOrbit[i]).ToStringLetterFormat("\\psi") + "$";
  }
  ElementWeylGroup currentElt;
  Vector<Polynomial<Rational> > differenceVector;
  Rational currentCoordDifference;
  for (int i = 0; i <outputOrbit.size; i ++) {
    for (int j = 0; j < theWeyl.RootsOfBorel.size; j ++) {
      currentWeight = outputOrbit[i];
      currentElt.MakeRootReflection(theWeyl.RootsOfBorel[j], theWeyl);
      if (useRho) {
        currentWeight += theWeyl.rho;
      }
      theWeyl.ActOn(currentElt, currentWeight);
      if (useRho) {
        currentWeight -= theWeyl.rho;
      }
      differenceVector = outputOrbit[i] - currentWeight;
      bool isGood = !differenceVector.isEqualToZero();
      for (int k = 0; k < differenceVector.size; k++) {
        if (!differenceVector[k].IsConstant(&currentCoordDifference)) {
          isGood = false;
          break;
        } else if (!currentCoordDifference.IsInteger() || currentCoordDifference < 0) {
          isGood = false;
          break;
        }
      }
      if (isGood) {
        std::stringstream reflectionStream;
        reflectionStream << "s_{" << i << "}";
        integralPositiveRootReflectionGraph.AddEdge(i, outputOrbit.getIndex(currentWeight), reflectionStream.str());
      }
    }
  }
  integralPositiveRootReflectionGraph.CheckConsistency();
  out << integralPositiveRootReflectionGraph.ToStringPsTricks(nullptr);
  for (int i = 0; i < outputOrbit.size; i ++) {
    theFormat.simpleRootLetter = "\\alpha";
    theFormat.fundamentalWeightLetter = "\\psi";
    std::string orbitEltString = outputOrbit[i].toString(&theFormat);
    Vector<Polynomial<Rational> > epsVect = outputOrbit[i];
    epsCoordMat.ActOnVectorColumn(epsVect);
    std::string orbitEltStringEpsilonCoords = epsVect.ToStringLetterFormat("\\varepsilon", &theFormat);
    std::string weightEltString=
    theWeyl.GetFundamentalCoordinatesFromSimple(outputOrbit[i]).ToStringLetterFormat(theFormat.fundamentalWeightLetter, &theFormat);
    out << "<tr>" << "<td>"
    << (useMathTag ? HtmlRoutines::GetMathMouseHover(orbitGeneratingSet[i].toString()) : orbitGeneratingSet[i].toString())
    << "</td><td>"
    << (useMathTag ? HtmlRoutines::GetMathMouseHover(orbitEltString) : orbitEltString) << "</td><td>"
    << (useMathTag ? HtmlRoutines::GetMathMouseHover(orbitEltStringEpsilonCoords) : orbitEltStringEpsilonCoords)
    << "</td><td>"
    << (useMathTag ? HtmlRoutines::GetMathMouseHover(weightEltString) : weightEltString)
    << "</td>";
    latexReport << "$" << orbitGeneratingSet[i].toString(&theFormat) << "$ & $" << orbitEltStringEpsilonCoords
    << "$ & $" <<  weightEltString << "$ & $"
    << (outputOrbit[0] - outputOrbit[i]).ToStringLetterFormat(theFormat.simpleRootLetter, &theFormat)
    << "$\\\\\n<br>";
    if (useRho) {
      currentWeight = theHWsimpleCoords;
      standardElt.makeOne(*theSSalgebra.content);
      bool isGood = true;
      for (int j = orbitGeneratingSet[i].generatorsLastAppliedFirst.size - 1; j >= 0; j --) {
        int simpleIndex = orbitGeneratingSet[i].generatorsLastAppliedFirst[j].index;
        theExp = theWeyl.GetScalarProdSimpleRoot(currentWeight, simpleIndex);
        theWeyl.SimpleReflectionRhoModified(simpleIndex, currentWeight);
        theExp *= 2;
        theExp /= theWeyl.CartanSymmetric.elements[simpleIndex][simpleIndex];
        theExp += Rational(1);
        if (theExp.IsInteger(&tempInt)) {
          if (tempInt < 0) {
            isGood = false;
            break;
          }
        }
        standardElt.MultiplyByGeneratorPowerOnTheLeft(theSSalgebra.content->GetNumPosRoots() - simpleIndex - 1, theExp);
      }
      out << "<td>";
      if (isGood) {
        out << HtmlRoutines::GetMathMouseHover(standardElt.toString(&theFormat));
      } else {
        out << "-";
      }
      out << "</td>";
    }
    out << "</tr>";
  }
  latexReport << "\\end{longtable}";
  out << "</table>" << "<br> " << latexReport.str();
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupLoadOrComputeCharTable(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupLoadOrComputeCharTable");
  if (!CalculatorConversions::innerLoadWeylGroup(theCommands, input, output)) {
    return false;
  }
  WeylGroupData& theGroup = output.GetValueNonConst<WeylGroupData>();
  if (theGroup.getDimension() > 8) {
    theCommands << "Computing character table disabled for rank >= 8, modify file " << __FILE__
    << " line "  << __LINE__ << " to change that. ";
    return false;
  }
  std::stringstream reportStream;
  theGroup.ComputeOrLoadCharacterTable(&reportStream);
  theCommands << reportStream.str();
  return output.AssignValue(theGroup, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClasseS(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClasseS");
  if (!CalculatorConversions::innerLoadWeylGroup(theCommands, input, output)) {
    return false;
  }
  WeylGroupData& theGroup = output.GetValueNonConst<WeylGroupData>();
  if (theGroup.getDimension() > 8) {
    theCommands << "Conjugacy classes computation disabled for rank greater than 8. "
    << "Modify source code "
    << "file " << __FILE__ << " line " << __LINE__
    << " and rebuild the calculator to change that.";
    return false;
  }
  std::stringstream out;
  theGroup.ComputeOrLoadConjugacyClasses(&out);
  theCommands << out.str();
  return output.AssignValue(theGroup, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupOuterConjugacyClassesFromAllElements(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupOuterConjugacyClassesFromAllElements");
  if (!CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClassesFromAllElements(theCommands, input, output)) {
    return false;
  }
  WeylGroupData& theGroupData = output.GetValueNonConst<WeylGroupData>();
  char theType ='X';
  int theRank = - 1;
  bool hasOuterAutosAndIsSimple = false;
  if (theGroupData.theDynkinType.IsSimple(&theType, &theRank)) {
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
  theAutomorphismGroup.ComputeOuterAutoGenerators();
  groupNoOuterAutos.generators.setSize(theGroupData.getDimension());
  Vector<Rational> simpleRoot;
  for (int i = 0; i < theGroupData.getDimension(); i ++) {
    simpleRoot.MakeEi(theGroupData.getDimension(), i);
    theGroupData.GetMatrixReflection(simpleRoot, groupNoOuterAutos.generators[i]);
  }
  //if (false)
  Matrix<Rational> currentAuto;
  List<Matrix<Rational> > outerAutos;
  for (int i = 0; i < theAutomorphismGroup.theOuterAutos.theGenerators.size; i ++) {
    theAutomorphismGroup.theOuterAutos.theGenerators[i].GetMatrix(currentAuto, theGroupData.getDimension());
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
//  groupWithOuterAutos.ComputeAllElements(false, - 1);
  groupNoOuterAutos.ComputeAllElements(false, - 1);
  groupNoOuterAutos.ComputeCCfromAllElements();
  out << "Weyl group matrix realization: " << groupNoOuterAutos.toString();
  Matrix<Rational> conjugatedMat, invertedOuterAuto;
  for (int j = 0; j < outerAutos.size; j ++) {
    out << "Outer automorphism " << j << ": "
    << outerAutos[j].toString() << "<br>";
    invertedOuterAuto = outerAutos[j];
    invertedOuterAuto.Invert();
    for (int i = 0; i <groupNoOuterAutos.conjugacyClasseS.size; i ++) {
      conjugatedMat = outerAutos[j];
      conjugatedMat *= groupNoOuterAutos.conjugacyClasseS[i].representative;
      conjugatedMat *= invertedOuterAuto;
      int found = - 1;
      for (int k = 0; k<groupNoOuterAutos.conjugacyClasseS.size; k ++) {
        if (groupNoOuterAutos.conjugacyClasseS[k].theElements.contains(conjugatedMat)) {
          found = k;
          break;
        }
      }
      out << "Maps conj. class " << i + 1 << " -> " << found + 1 << "<br>";
    }
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClassesFromAllElements(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClassesFromAllElements");
  if (!CalculatorConversions::innerLoadWeylGroup(theCommands, input, output)) {
    return false;
  }
  WeylGroupData& theGroupData = output.GetValueNonConst<WeylGroupData>();
  if (theGroupData.getDimension() > 7) {
    theCommands << "<hr>Loaded Dynkin type "
    << theGroupData.theDynkinType.toString() << " of rank "
    << theGroupData.getDimension() << " but I've been told "
    << "not to compute when the rank is larger than 7. ";
    return false;
  }
  double timeStart1 = global.GetElapsedSeconds();
  theGroupData.theGroup.ComputeCCfromAllElements();
  //std::stringstream out;
  theCommands << "<hr> Computed conjugacy classes of "
  << theGroupData.toString() << " in " << global.GetElapsedSeconds() - timeStart1
  << " second(s). ";
  return output.AssignValue(theGroupData, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClassesRepresentatives(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClassesRepresentatives");
  if (!CalculatorConversions::innerLoadWeylGroup(theCommands, input, output)) {
    return false;
  }
  WeylGroupData& theGroupData = output.GetValueNonConst<WeylGroupData>();
  theGroupData.CheckConsistency();
  if (theGroupData.getDimension() > 8) {
    return theCommands << "<hr>Loaded Dynkin type " << theGroupData.theDynkinType.toString()
    << " of rank " << theGroupData.getDimension() << " but I've been told "
    << "not to compute when the rank is larger than 8. ";
  }
  theGroupData.CheckConsistency();
  double timeStart1 = global.GetElapsedSeconds();
  theGroupData.CheckConsistency();
  theGroupData.theGroup.ComputeCCSizesAndRepresentatives();
  theCommands << "<hr> Computed conjugacy classes representatives of "
  << theGroupData.theDynkinType.toString() << " in " << global.GetElapsedSeconds()-timeStart1
  << " second(s). ";
  return output.AssignValue(theGroupData, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupIrrepsAndCharTableComputeFromScratch(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupIrrepsAndCharTableComputeFromScratch");
  if (!CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClasseS(theCommands, input, output)) {
    return false;
  }
  if (!output.IsOfType<WeylGroupData>()) {
    return true;
  }
  WeylGroupData& theGroupData = output.GetValueNonConst<WeylGroupData>();
  theGroupData.ComputeInitialIrreps();
  theGroupData.theGroup.ComputeIrreducibleRepresentationsTodorsVersion();
  FormatExpressions tempFormat;
  tempFormat.flagUseLatex = true;
  tempFormat.flagUseHTML = false;
  std::stringstream out;
  out << "Character table: ";
  out << theGroupData.theGroup.PrettyPrintCharacterTable();
  //Matrix<Rational> charMat;
  //charMat.init(theGroupData.theGroup.ConjugacyClassCount(), theGroupData.theGroup.ConjugacyClassCount());
  //for (int i = 0; i < theGroupData.theGroup.irreps.size; i ++)
  //{ //out << "<br>" << theGroup.irreps[i].theCharacteR.toString();
  //  charMat.AssignVectorToRowKeepOtherRowsIntactNoInit(i, theGroupData.irreps[i].GetCharacter().data);
  //}
  //out << HtmlRoutines::GetMathSpanPure(charMat.toString(&tempFormat));
  out << "<br>Explicit realizations of each representation follow.";
  for (int i = 0; i < theGroupData.theGroup.irreps.size; i ++) {
    out << "<hr>" << theGroupData.theGroup.irreps[i].toString(&tempFormat);
  }
  out << theGroupData.toString(&tempFormat);
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupOuterAutoGeneratorsPrint(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupOuterAutoGeneratorsPrint");
  DynkinType theType;
  if (!CalculatorConversions::innerDynkinTypE(theCommands, input, theType)) {
    return output.MakeError("Failed to extract Dynkin type from argument. ", theCommands);
  }
  std::stringstream out, outCommand;
  FinitelyGeneratedMatrixMonoid<Rational> groupGeneratedByMatrices;
  theType.GetOuterAutosGeneratorsActOnVectorColumn(groupGeneratedByMatrices.theGenerators);
  FormatExpressions tempFormat;
  tempFormat.flagUseLatex = true;
  tempFormat.flagUseHTML = false;
  for (int i = 0; i <groupGeneratedByMatrices.theGenerators.size; i ++) {
    outCommand << "<br>s_{" << i + 1
    << "}=MatrixRationals" << groupGeneratedByMatrices.theGenerators[i].toStringMatrixForm(&tempFormat) << ";";
    out << "<br>s_" << i + 1 << " = "
    << HtmlRoutines::GetMathSpanPure(groupGeneratedByMatrices.theGenerators[i].toStringMatrixForm(&tempFormat));
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
  bool success = groupGeneratedByMatrices.GenerateElements(10000);
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
        out << "<tr><td>" << HtmlRoutines::GetMathMouseHover(elementNameStream.str())<< "</td><td>"
        << HtmlRoutines::GetMathMouseHover(groupGeneratedByMatrices.theElements[i].toStringMatrixForm(&tempFormat)) << "</td></tr>";
      }
      out << "</table>";
    }
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupOrbitFundRho(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  return CalculatorFunctionsWeylGroup::innerWeylOrbit(theCommands, input, output, true, true);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupOrbitFund(Calculator& theCommands, const Expression& input, Expression& output) {
  return CalculatorFunctionsWeylGroup::innerWeylOrbit(theCommands, input, output, true, false);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupOrbitSimple(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  return CalculatorFunctionsWeylGroup::innerWeylOrbit(theCommands, input, output, false, false);
}

bool CalculatorFunctionsWeylGroup::innerTensorWeylReps(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerTensorWeylReps");
  if (input.size() != 3) {
    return false;
  }
  GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> leftRep;
  GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> rightRep;
  if (!input[1].IsOfType<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >(&leftRep)) {
    return false;
  }
  if (!input[2].IsOfType<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> > (&rightRep)) {
    return false;
  }
  FormatExpressions theFormat;
  theFormat.flagUseLatex = true;
  theFormat.flagUseHTML = false;
  if (leftRep.ownerGroup != rightRep.ownerGroup) {
    return output.MakeError("Error: attempting to tensor irreps with different owner groups. ", theCommands);
  }
  leftRep *= rightRep;
  return output.AssignValue(leftRep, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerTensorAndDecomposeWeylReps(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerTensorAndDecomposeWeylReps");
  Expression theTensor;
  if (!(input.children.size == 3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (!leftE.IsOfType<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >()) {
    return false;
  }
  if (!rightE.IsOfType<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >()) {
    return false;
  }
  VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> outputRep =
  leftE.getValue<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >();
  outputRep *= rightE.getValue<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >();
  return output.AssignValue(outputRep, theCommands);
}

std::string WeylGroupData::ToStringIrrepLabel(int indexIrrep) {
  if (indexIrrep < this->irrepsCarterLabels.size) {
    return this->irrepsCarterLabels[indexIrrep];
  }
  std::stringstream out;
  out << "\\phi_{" << indexIrrep + 1 << "}";
  return out.str();
}

std::string WeylGroupData::ToStringSignSignatureRootSubsystem(const List<SubgroupDataRootReflections>& inputSubgroups) {
  MacroRegisterFunctionWithName("WeylGroup::ToStringSignSignatureRootSubsystem");
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
  fullSignSig.setSize(this->theGroup.ConjugacyClassCount());
  pseudoSignSig.setSize(this->theGroup.ConjugacyClassCount());
  parabolicSignSig.setSize(this->theGroup.ConjugacyClassCount());
  for (int i = 0; i < this->theGroup.ConjugacyClassCount(); i ++) {
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
    << HtmlRoutines::CleanUpForLaTeXLabelUse(this->theDynkinType.toString())
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
    if (this->theDynkinType.IsSimple(&simpleType.theLetter, &simpleType.theRank, &simpleType.CartanSymmetricInverseScale)) {
      for (int i = startIndex; i < startIndexNextCol; i ++) {
        mainTableStream << "&$" << inputSubgroups[i].theDynkinType.toString(&formatSupressUpperIndexOne) << "$";
      }
    } else {
      for (int i = startIndex; i < startIndexNextCol; i ++) {
        mainTableStream << "&$" << inputSubgroups[i].theDynkinType.toString() << "$";
      }
    }
    for (int i = 0; i < this->theGroup.ConjugacyClassCount(); i ++) {
      mainTableStream << "\\\\";
      if (i == 0) {
        mainTableStream << "\\hline";
      }
      mainTableStream << "\n<br>\n$" << this->ToStringIrrepLabel(i) << "$";
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
        parSelrootsAreOuttaLevi.InvertSelection();
        out << "<td>" << parSelrootsAreOuttaLevi.toString() << "</td>";
      }
      out << "</tr><tr><td></td><td></td>";
    }
    if (this->theDynkinType.IsSimple(&simpleType.theLetter, &simpleType.theRank, &simpleType.CartanSymmetricInverseScale)) {
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
    for (int i = 0; i < this->theGroup.ConjugacyClassCount(); i ++) {
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
  << "\\caption{\\label{tableIrrepChars" << this->theDynkinType.toString()
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
    out << "$" << this->ToStringIrrepLabel(i) << "$";
    for (int j = 0; j < this->theGroup.characterTable[i].data.size; j ++) {
      out << "&$" << this->theGroup.characterTable[i].data[j].toString() << "$";
    }
    out << "\\\\<br>\n";
  }
  out << "\\end{longtable}\n<br>\n";
  out << "\\begin{longtable}{rrcl}" << "\\caption{\\label{tableConjugacyClassTable"
  << HtmlRoutines::CleanUpForLaTeXLabelUse(this->theDynkinType.toString()) << "}}\\\\ ";
  out << "$\\#$ & Representative & Class size & Root subsystem label\\\\<br>\n";
  for (int i = 0; i < this->theGroup.ConjugacyClassCount(); i ++) {
    out << "$" << i + 1 << "$ & " "$" << this->theGroup.conjugacyClasseS[i].representative.toString()
    << "$&$ " << this->theGroup.conjugacyClasseS[i].size.toString() << "$";
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
//  bool IncrementReturnFalseIfPastLast();
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
  bool Compute(HashedList<KostkaNumber>* KNcache, std::stringstream* comments = nullptr);
};

class SelectionFixedRankDifferentMaxMultiplicities {
public:
  List<int> Multiplicities;
  List<int> MaxMultiplicities;
  int rank;
  bool flagFirstComputed;
  bool init();
  bool firstIncrement();
  bool IncrementReturnFalseIfPastLast();
  SelectionFixedRankDifferentMaxMultiplicities();
  std::string toString();
  std::string toStringFull();
};

unsigned int KostkaNumber::hashFunction(const KostkaNumber& input) {
  return  MathRoutines::HashListInts(input.partition) + MathRoutines::HashListInts(input.tuple);
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

bool KostkaNumber::Compute(HashedList<KostkaNumber>* KNcache, std::stringstream* comments) {
  MacroRegisterFunctionWithName("KostkaNumber::Compute");
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
  theSel.init();
  theSel.MaxMultiplicities.setSize(this->partition.size);
  for (int i = 0; i < this->partition.size; i ++) {
    if (i != this->partition.size - 1) {
      theSel.MaxMultiplicities[i] = this->partition[i] - this->partition[i + 1];
    } else {
      theSel.MaxMultiplicities[i] = this->partition[i];
    }
  }
  theSel.rank = *this->tuple.LastObject();
  this->value = 0;
  while (theSel.IncrementReturnFalseIfPastLast()) {
    KostkaNumber ancestor;
    ancestor.partition = this->partition;
    ancestor.tuple = this->tuple;
    ancestor.tuple.setSize(ancestor.tuple.size - 1);
    for (int i = 0; i < theSel.Multiplicities.size; i ++) {
      ancestor.partition[i] -= theSel.Multiplicities[i];
    }
    if (KNcache != nullptr) {
      int ancestorIndex = KNcache->getIndex(ancestor);
      if (ancestorIndex != - 1) {
        ancestor = KNcache->getElement(ancestorIndex);
      } else if (!ancestor.Compute(KNcache, comments)) {
        return false;
      } else {
        if (KNcache->size < this->MaxNumCachedKostkaNumbers) {
          KNcache->addOnTop(ancestor);
        }
      }
    } else {
      if (!ancestor.Compute(KNcache, comments)) {
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
        currentPartition.Object1 = partitionsLeft[j];
        currentPartition.Object2 = partitionsRight[k];
        partitionPairs.addOnTop(currentPartition);
      }
    }
  }
  out << partitionPairs.size << " partition pairs. <br>";
  for (int i = 0; i < partitionPairs.size; i ++) {
    out << partitionPairs[i].Object1.toString()
    << "," << partitionPairs[i].Object2.toString() << "<br>";
  }
  partitionPairs.quickSortAscending();
  partitionsParabolics.quickSortAscending();
  Matrix<Rational> theMultTable;
  theMultTable.init(partitionPairs.size, partitionsParabolics.size);
  for (int j = 0; j < partitionPairs.size; j ++) {
    out << "V_{\\lambda, \\mu}, "
    << "<br>\\lambda =" << partitionPairs[j].Object1.p
    << "<br>\\mu=" << partitionPairs[j].Object2.p;
    for (int i = 0; i < partitionsParabolics.size; i ++) {
      theMultTable(j, i) = KostkaNumber::ComputeTypeBParabolicSignMultiplicity(
        partitionsParabolics[i],
        partitionPairs[j].Object1,
        partitionPairs[j].Object2,
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
    parStream << partitionsParabolics[i].ToStringForArticles("(", ")")
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
      theSimpleType.MakeArbitrary('A', partitionsParabolics[i].p[j] - 1, 1);
      theType.AddMonomial(theSimpleType, 1);
    }
    if (typeBsize == 1) {
      theSimpleType.MakeArbitrary('A', 1, 2);
      theType.AddMonomial(theSimpleType,1);
    } else if (typeBsize > 1) {
      theSimpleType.MakeArbitrary('B', typeBsize, 1);
      theType.AddMonomial(theSimpleType, 1);
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
    << partitionPairs[i].Object1.ToStringForArticles("[", "]")
    << ", "
    << partitionPairs[i].Object2.ToStringForArticles("[", "]")
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
    << leftPartition.p.ToStringCommaDelimited()
    << ", "
    << rightPartition.p.ToStringCommaDelimited()
    << "} )=";
  }
  Rational result = 0;
  SelectionWithDifferentMaxMultiplicities theSelection;
  List<int> complementSelection;
  HashedList<KostkaNumber> KNcache;
  KostkaNumber leftKN, rightKN;
  Partition tempP;
  tempP = leftPartition;
  tempP.Transpose();
  leftKN.partition = tempP.p;
  tempP = rightPartition;
  tempP.Transpose();
  rightKN.partition = tempP.p;
  theSelection.initFromInts(parabolicPartition.p);
  do {
    complementSelection.setSize(parabolicPartition.p.size);
    for (int k = 0; k < theSelection.MaxMultiplicities.size; k ++) {
      complementSelection[k] = parabolicPartition.p[k] - theSelection.Multiplicities[k];
    }
    leftKN.tuple = theSelection.Multiplicities;
    leftKN.Compute(&KNcache, nullptr);
    rightKN.tuple = complementSelection;
    rightKN.tuple.addOnTop(BcomponentSize);
    rightKN.Compute(&KNcache, nullptr);
    result += leftKN.value * rightKN.value;
  } while (theSelection.IncrementReturnFalseIfPastLast());
  if (comments != nullptr) {
    *comments << result.toString();
  }
  return result;
}

std::string SelectionFixedRankDifferentMaxMultiplicities::toString() {
  std::stringstream out;
  Vector<int> theMults;
  theMults = this->Multiplicities;
  out << theMults;
  return out.str();
}

std::string SelectionFixedRankDifferentMaxMultiplicities::toStringFull() {
  std::stringstream out;
  Vector<int> theMults, theMaxMults;
  theMaxMults = this->MaxMultiplicities;
  theMults = this->Multiplicities;
  out << "Multiplicities: " << theMults << "; max: " << theMaxMults
  << "; rank: " << this->rank;
  return out.str();
}

SelectionFixedRankDifferentMaxMultiplicities::SelectionFixedRankDifferentMaxMultiplicities() {
  this->flagFirstComputed = false;
}

bool SelectionFixedRankDifferentMaxMultiplicities::init() {
  this->flagFirstComputed = false;
  return true;
}

bool SelectionFixedRankDifferentMaxMultiplicities::firstIncrement() {
  MacroRegisterFunctionWithName("SelectionFixedRankDifferentMaxMultiplicities::firstIncrement");
  this->flagFirstComputed = true;
  this->Multiplicities.setSize(this->MaxMultiplicities.size);
  int remainingRank = this->rank;
  for (int i = this->MaxMultiplicities.size - 1; i >= 0; i --) {
    if (this->MaxMultiplicities[i] < remainingRank) {
      this->Multiplicities[i] = this->MaxMultiplicities[i];
    } else {
      this->Multiplicities[i] = remainingRank;
    }
    remainingRank -= this->Multiplicities[i];
  }
  if (remainingRank > 0) {
    return false;
  }
  return true;
}

bool SelectionFixedRankDifferentMaxMultiplicities::IncrementReturnFalseIfPastLast() {
  MacroRegisterFunctionWithName("SelectionFixedRankDifferentMaxMultiplicities::IncrementReturnFalseIfPastLast");
  if (this->rank < 0) {
    return false;
  }
  if (!this->flagFirstComputed) {
    return this->firstIncrement();
  }
  int rankToRedistribute = 0;
  for (int i = this->Multiplicities.size - 2; i >= 0; i --) {
    rankToRedistribute += this->Multiplicities[i + 1];
    this->Multiplicities[i + 1] = 0;
    if (this->Multiplicities[i] < this->MaxMultiplicities[i] && rankToRedistribute > 0) {
      this->Multiplicities[i] ++;
      rankToRedistribute --;
      for (int j = this->Multiplicities.size - 1; j > i; j --) {
        if (this->MaxMultiplicities[j]<=rankToRedistribute) {
          this->Multiplicities[j] = this->MaxMultiplicities[j];
        } else {
          this->Multiplicities[j] = rankToRedistribute;
        }
        rankToRedistribute -= this->Multiplicities[j];
        if (rankToRedistribute == 0) {
          return true;
        }
      }
      return true;
    }
  }
  return false;
}

bool CalculatorFunctionsWeylGroup::innerKostkaNumber(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerKostkaNumber");
  if (input.size() != 3) {
    return false;
  }
  KostkaNumber theKN;
  if (
    !theCommands.GetVectoRInt(input[1], theKN.partition) ||
    !theCommands.GetVectoRInt(input[2], theKN.tuple)
  ) {
    return theCommands << "Failed to extract partition and tuple from input: " << input.toString();
  }
  std::stringstream out;
  HashedList<KostkaNumber> theKNs;
  if (theKN.Compute(&theKNs, &out)) {
    out << "<br>Final result: " << theKN.value.toString();
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerAllSelectionsFixedRank(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerKostkaNumber");
  if (input.size() != 3) {
    return false;
  }
  SelectionFixedRankDifferentMaxMultiplicities theSel;
  if (!input[1].IsSmallInteger(&theSel.rank)) {
    return false;
  }
  if (!theCommands.GetVectoRInt(input[2], theSel.MaxMultiplicities)) {
    return theCommands << "Failed to extract list of multiplicities from "
    << input[2].toString();
  }
  if (theSel.rank < 0) {
    return output.AssignValue(0, theCommands);
  }
  theSel.init();
  std::stringstream out;
  out << "Max multiplicities: " << theSel.MaxMultiplicities << " rank: "
  << theSel.rank;
  while (theSel.IncrementReturnFalseIfPastLast()) {
    out << "<br>" << theSel.toString();
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerSignSignatureRootSubsystemsFromKostkaNumbers(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerSignSignatureRootSubsystems");
  std::stringstream out;
  if (!CalculatorConversions::innerLoadWeylGroup(theCommands, input, output)) {
    return false;
  }
  if (!output.IsOfType<WeylGroupData>()) {
    return false;
  }
  WeylGroupData& theWeyl = output.GetValueNonConst<WeylGroupData>();
  if (theWeyl.getDimension() > 12) {
    return theCommands << "<hr>Computing sign signatures restricted up to rank 12.";
  }
  char type = 'X';
  int rank = - 1;
  if (!theWeyl.theDynkinType.IsSimple(&type, &rank)) {
    return theCommands << "This function is implemented for simple classical Weyl groups only.";
  }
  if (type != 'A' && type != 'B' && type != 'C' && type != 'D') {
    return theCommands << "You requested computation for type " << type
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
        partitionsTransposed[i].Transpose();
        out << "<br>Partition: " << thePartitions[i].toString()
        << ", transposed: " << partitionsTransposed[i].toString();
      }
    }
    return theCommands << "Not implemented yet.";
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerSignSignatureRootSubsystems(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerSignSignatureRootSubsystems");
  if (!CalculatorConversions::innerLoadWeylGroup(theCommands, input, output)) {
    return false;
  }
  if (!output.IsOfType<WeylGroupData>()) {
    return false;
  }
  WeylGroupData& theWeyl = output.GetValueNonConst<WeylGroupData>();
  if (theWeyl.getDimension() > 8) {
    theCommands << "<hr>Computing sign signatures restricted up to rank 8.";
    return false;
  }
  std::stringstream out;
  List<SubgroupDataRootReflections> parabolicSubgroupS, extendedParabolicSubgroups,
  allRootSubgroups, finalSubGroups;
  if (!theWeyl.LoadSignSignatures(finalSubGroups)) {
    theWeyl.GetSignSignatureParabolics(parabolicSubgroupS);
    theWeyl.GetSignSignatureExtendedParabolics(extendedParabolicSubgroups);
    theWeyl.GetSignSignatureAllRootSubsystems(allRootSubgroups);
    List<Pair<std::string, List<Rational>, MathRoutines::HashString> > tauSigPairs;
    finalSubGroups.Reserve(allRootSubgroups.size);
    Pair<std::string, List<Rational>, MathRoutines::HashString> currentTauSig;
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
        currentTauSig.Object1 = (*currentSGs)[i].theDynkinType.toString();
        currentTauSig.Object2 = (*currentSGs)[i].tauSignature;
        if (!tauSigPairs.contains(currentTauSig)) {
          tauSigPairs.addOnTop(currentTauSig);
          finalSubGroups.addOnTop((*currentSGs)[i]);
        }
      }
    }
  }
  out << theWeyl.ToStringSignSignatureRootSubsystem(finalSubGroups);
  return output.AssignValue(out.str(), theCommands);
}

template < >
GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& Expression::GetValueNonConst() const;


bool CalculatorFunctionsWeylGroup::innerDecomposeWeylRep(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerDecomposeWeylRep");
  if (!input.IsOfType<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> > ()) {
    return false;
  }
  GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& inputRep =
  input.GetValueNonConst<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >();
  VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> outputRep;
  inputRep.MakeGRCAM().DecomposeTodorsVersion(outputRep);
  return output.AssignValue(outputRep, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerIsOuterAutoWeylGroup(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerIsOuterAutoWeylGroup");
  if (input.size() != 3) {
    return theCommands << "<hr>IsOuterAuto expects 2 arguments.";
  }
  DynkinType theType;
  if (!CalculatorConversions::functionDynkinType(theCommands, input[1], theType)) {
    return theCommands << "<hr>Failed to get Dynkin type from argument. " << input[1].toString();
  }
  Matrix<Rational> theMat;
  if (!theCommands.functionGetMatrix(input[2], theMat)) {
    return theCommands << "<hr>Failed to get matrix from argument. " << input[2].toString();
  }
  if (theMat.NumCols != theMat.NumRows || theMat.NumCols != theType.GetRank()) {
    theCommands << "<hr>Extracted Dynkin type " << theType.toString() << " is of rank " << theType.GetRank()
    << " but extracted linear operator has " << theMat.NumCols << " columns and " << theMat.NumRows << " rows.";
    return false;
  }
  WeylGroupData theWeyl;
  theWeyl.MakeFromDynkinType(theType);
  theWeyl.ComputeRho(true);
  WeylGroupAutomorphisms theAutomorphisms;
  theAutomorphisms.theWeyl = &theWeyl;
  MatrixTensor<Rational> theOp;
  theOp = theMat;
  if (theAutomorphisms.IsElementWeylGroupOrOuterAuto(theOp)) {
    return output.AssignValue(1, theCommands);
  }
  return output.AssignValue(0, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupNaturalRep(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupNaturalRep");
  if (!CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClasseS(theCommands, input, output)) {
    return false;
  }
  if (!output.IsOfType<WeylGroupData>()) {
    return false;
  }
  WeylGroupData& theGroup = output.GetValueNonConst<WeylGroupData>();
  theGroup.ComputeInitialIrreps();
  theGroup.theGroup.ComputeIrreducibleRepresentationsTodorsVersion();
  GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup>, Rational> tempRep;
  theGroup.GetStandardRepresentation(tempRep);
  return output.AssignValue(tempRep.MakeOtherGroupRepresentationClass(), theCommands);
}

class MonomialMacdonald {
public:
  SemisimpleLieAlgebra* owner;
  Selection rootSel;
  bool flagDeallocated;
  MonomialMacdonald(): owner(nullptr), flagDeallocated(false) {
  }
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  bool CheckConsistency() {
    if (this->flagDeallocated) {
      global.fatal << "This is a programming error: use after free of MonomialMacdonald. " << global.fatal;
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

std::string MonomialMacdonald::toString(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("MonomialMacdonald::toString");
  (void) theFormat; //prevent unused parameter warning, portable
  if (this->owner == nullptr) {
    return "(non-initialized)";
  }
  if (this->rootSel.CardinalitySelection == 0) {
    return "1";
  }
  std::stringstream out;
  for (int i = 0; i < this->rootSel.CardinalitySelection; i ++) {
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
    for (int j = 0; j < this->owner->GetRank(); j ++) {
      currentMon = output[i];
      currentMon.ActOnMeSimpleReflection(j, tempRat);
      output.addOnTopNoRepetition(currentMon);
    }
  }
}

void MonomialMacdonald::MakeFromRootSubsystem(const Vectors<Rational>& inputRoots, SemisimpleLieAlgebra& inputOwner) {
  MacroRegisterFunctionWithName("MonomialMacdonald::MakeFromRootSubsystem");
  this->owner = &inputOwner;
  this->rootSel.init(inputOwner.theWeyl.RootSystem.size);
  Vector<Rational> currentV;
  for (int i = 0; i < inputRoots.size; i ++) {
    currentV = inputRoots[i];
    if (currentV.isNegative()) {
      currentV *= - 1;
    }
    int indexInRoots = inputOwner.theWeyl.RootSystem.getIndex(currentV);
    if (indexInRoots < 0) {
      global.fatal << "This is a programming error: attempting to make a Macdonald polynomial from " << inputRoots.toString()
      << ": the vector " << currentV.toString()
      << " is not a root. " << global.fatal;
    }
    this->rootSel.AddSelectionAppendNewIndex(indexInRoots);
  }
  this->rootSel.ComputeIndicesFromSelection();
}

void MonomialMacdonald::ActOnMeSimpleReflection(int indexSimpleReflection, Rational& outputMultiple) {
  Selection originalSel;
  originalSel = this->rootSel;
  this->rootSel.init(this->owner->theWeyl.RootSystem.size);
  Vector<Rational> currentV;
  outputMultiple = 1;
  for (int i = 0; i <originalSel.CardinalitySelection; i ++) {
    currentV = this->owner->theWeyl.RootSystem[originalSel.elements[i]];
    this->owner->theWeyl.SimpleReflection(indexSimpleReflection, currentV);
    if (currentV.isNegative()) {
      currentV *= - 1;
      outputMultiple *= - 1;
    }
    this->rootSel.AddSelectionAppendNewIndex(this->owner->theWeyl.RootSystem.getIndex(currentV));
  }
}

bool CalculatorFunctionsWeylGroup::innerMacdonaldPolys(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerMacdonaldPolys");
  //note that if input is list of 2 elements then input[0] is sequence atom, and your two elements are in fact
  //input[1] and input[2];
  if (input.size() != 2) {
    return theCommands << "Macdonald polynomials expects as input a single argument. ";
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!theCommands.Convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, algebra
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  rootSubalgebras theRootSAs;
  theRootSAs.owner = algebra.content;
  theRootSAs.ComputeAllReductiveRootSubalgebrasUpToIsomorphism();
  std::stringstream out;
  MonomialMacdonald theGenerator;
  HashedList<MonomialMacdonald> theOrbit;
  for (int i = 0; i < theRootSAs.theSubalgebras.size; i ++) {
    rootSubalgebra& currentRootSA = theRootSAs.theSubalgebras[i];
    theGenerator.MakeFromRootSubsystem(currentRootSA.PositiveRootsK, *algebra.content);
    theGenerator.GenerateMyOrbit(theOrbit);
    out << "<hr>Root subsystem type " << currentRootSA.theDynkinDiagram.toString();
    out << ". Orbit has " << theOrbit.size << " element(s), here they are: ";
    for (int j = 0; j < theOrbit.size; j ++) {
      out << "<br>" << theOrbit[j].toString();
    }
  }
  out << "Type: " << theRootSAs.owner->theWeyl.theDynkinType.toString()
  << ". Number of root subsystems: " << theRootSAs.theSubalgebras.size;
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerLieAlgebraWeight(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerLieAlgebraWeight");
  Weight<Polynomial<Rational> > resultWeight;
  if (input.size() != 4) {
    return false;
  }
  Expression tempE;
  SemisimpleLieAlgebra* theSSowner = nullptr;
  if (!CalculatorConversions::functionSemisimpleLieAlgebra(
    theCommands, input[1], tempE, theSSowner
  )) {
    return theCommands << "<hr>Failed to load semisimple Lie algebra";
  }
  std::string theCoordsString;
  bool isGood = input[3].IsOperation(&theCoordsString);
  if (isGood) {
    isGood = (theCoordsString == "epsilon") || (theCoordsString == "fundamental") || (theCoordsString == "simple");
  }
  if (!isGood) {
    return theCommands << "<hr>The third argument of MakeWeight is bad: must be one of the "
    << "keywords: epsilon, fundamental, simple. ";
  }
  int theWeightIndex = - 1;
  if (!input[2].IsSmallInteger(&theWeightIndex)) {
    return false;
  }
  if (theCoordsString != "epsilon") {
    if (theWeightIndex < 1 || theWeightIndex> theSSowner->GetRank()) {
      std::stringstream errorStream;
      errorStream << "The second argument of the MakeWeight function "
      << "needs to be index of a weight between 1 and the Lie algebra rank. "
      << "However, the index is " << theWeightIndex << ".";
      return output.MakeError(errorStream.str(), theCommands);
    }
    Vector<Polynomial<Rational> > EiVector;
    EiVector.MakeEi(theSSowner->GetRank(), theWeightIndex - 1);
    if (theCoordsString == "fundamental") {
      resultWeight.weightFundamentalCoordS = EiVector;
    } else if (theCoordsString == "simple") {
      resultWeight.weightFundamentalCoordS = theSSowner->theWeyl.GetFundamentalCoordinatesFromSimple(EiVector);
    }
  } else {
    Vector<Rational> EiVector;
    EiVector.makeZero(theSSowner->GetRank());
    Vector<Rational> tempV = theSSowner->theWeyl.GetEpsilonCoords(EiVector);
    if (theWeightIndex>tempV.size || theWeightIndex < 1) {
      std::stringstream errorStream;
      errorStream << "The second argument of the MakeWeight function needs to be index of a weight between 1 and " << tempV.size
      << ". However, the index is " << theWeightIndex << ".";
      return output.MakeError(errorStream.str(), theCommands);
    }
    EiVector.MakeEi(tempV.size, theWeightIndex - 1);
    resultWeight.weightFundamentalCoordS = theSSowner->theWeyl.GetFundamentalCoordinatesFromEpsilon(EiVector);
  }
  resultWeight.owner = theSSowner;
  ExpressionContext theContext(theCommands);
  theContext.setAmbientSemisimpleLieAlgebra(*theSSowner);
  return output.AssignValueWithContext(resultWeight, theContext, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerLieAlgebraRhoWeight(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerLieAlgebraRhoWeight");
  if (input.size() != 2) {
    return theCommands << "Lie algebra rho weight expects a single argument. ";
  }
  Weight<Polynomial<Rational> > resultWeight;
  SemisimpleLieAlgebra* theSSowner = nullptr;
  Expression tempE;
  if (!CalculatorConversions::functionSemisimpleLieAlgebra(
    theCommands, input[1], tempE, theSSowner
  )) {
    return theCommands << "<hr>Failed to load semisimple Lie algebra. ";
  }
  theSSowner->CheckConsistency();
  ExpressionContext theContext(theCommands);
  theContext.setAmbientSemisimpleLieAlgebra(*theSSowner);
  resultWeight.weightFundamentalCoordS = theSSowner->theWeyl.GetFundamentalCoordinatesFromSimple(theSSowner->theWeyl.rho);
  resultWeight.owner = theSSowner;
  return output.AssignValueWithContext(resultWeight, theContext, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerTestSpechtModules(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerTestSpechtModules");
  int theSymmetricGroupRank = 0;
  if (!input.IsSmallInteger(&theSymmetricGroupRank)) {
    return theCommands << "innerTestSpechtModules called with input that is not a small integer, not performing any tests.";
  }
  if (theSymmetricGroupRank < 1) {
    return theCommands << "innerTestSpechtModules takes as input a small positive integer, instead the input I got was "
    << theSymmetricGroupRank << ". ";
  }
  if (theSymmetricGroupRank > 6) {
    return theCommands
    << "For speed/memory reasons, innerTestSpechtModules is currently restricted to take input no larger than 6. Your input was: "
    << theSymmetricGroupRank;
  }
  std::stringstream out;
  out << "User has requested the test of Specht modules of S_"
  << theSymmetricGroupRank << ". ";
  Partition::TestAllSpechtModules(theSymmetricGroupRank);
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerRepresentElementHyperOctahedral(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerRepresentElementHyperOctahedral");
  if (input.size() != 3) {
    return theCommands << "Representating takes 2 arguments: element and rep.";
  }
  ElementHyperoctahedralGroupR2 theElt;
  GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> theRep;
  if (!input[1].IsOfType(&theElt)) {
    return theCommands << "Failed to extract element of hyperoctahedral group from " << input[1].toString();
  }
  if (!input[2].IsOfType(&theRep)) {
    return theCommands << "Failed to extract representation from " << input[2].toString();
  }
  Matrix<Rational> result;
  if (!theRep.GetMatrixOfElement(theElt, result)) {
    return theCommands << "Failed to get matrix of element " << theElt.toString()
    << " from representation: " << theRep.toString();
  }
  return output.AssignMatrix(result, theCommands, nullptr, false);
}

bool CalculatorFunctionsWeylGroup::innerHyperOctahedralGetOneRepresentation(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerHyperOctahedralGetOneRepresentation");
  Vector<Rational> inputLeftRat, inputRightRat;
  if (input.size() != 3) {
    return theCommands << "CalculatorFunctionsWeylGroup::innerHyperOctahedralGetOneRepresentation needs two arguments";
  }
  if (!theCommands.GetVector(input[1], inputLeftRat)|| !theCommands.GetVector(input[2], inputRightRat)) {
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
      if (!(*currentInputRat)[i].IsIntegerFittingInInt(&(*currentInput)[i])) {
        return theCommands << "Failed to convert input: " << input.toString() << " to a list of small integers.";
      }
    }
    for (int i = 0; i < currentInput->size; i ++) {
      if ((*currentInput)[i] < 1 || (*currentInput)[i] > maxPartitionSize ) {
        return theCommands << "Entry: " << (*currentInput)[i] << " of " << (*currentInput)
        << " is outside of the allowed input range.";
      }
    }
    if (currentInput->SumCoords() > maxPartitionSize) {
      return theCommands << "The coordinates of vector " << (*currentInput) << " have sum that is too large. ";
    }
  }
  Partition partitionLeft, partitionRight;
  partitionLeft.FromListInt(inputLeft);
  partitionRight.FromListInt(inputRight);
  //std::stringstream out;
  //out << "Left partition is: " << partitionLeft.toString() << ", created from: " << inputLeft;
  //out << "Right partition is: " << partitionRight.toString() << ", created from: " << inputRight;
  int index = 0;
  for (; index < theCommands.theObjectContainer.theHyperOctahedralGroups.size; index ++) {
    if (
      theCommands.theObjectContainer.theHyperOctahedralGroups[index].flagIsEntireHyperoctahedralGroup &&
      theCommands.theObjectContainer.theHyperOctahedralGroups[index].N == partitionLeft.n + partitionRight.n
    ) {
      break;
    }
  }
  if (index == theCommands.theObjectContainer.theHyperOctahedralGroups.size) {
    theCommands.theObjectContainer.theHyperOctahedralGroups.setSize(
      theCommands.theObjectContainer.theHyperOctahedralGroups.size + 1
    );
    theCommands.theObjectContainer.theHyperOctahedralGroups[index].MakeHyperoctahedralGroup(
      partitionLeft.n + partitionRight.n
    );
  }
  //<-may be broken if copying of groups doesn't work!!!!!!!!
  HyperoctahedralGroupData& HD = theCommands.theObjectContainer.theHyperOctahedralGroups[index];
  GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> R;
  HD.SpechtModuleOfPartititons(partitionLeft, partitionRight, R);
  //out << R;
  R.ComputeCharacter();
  return output.AssignValue(R, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerSpechtModule(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerSpechtModule");
  Vector<Rational> inputRat;
  if (!theCommands.GetVectorFromFunctionArguments(input, inputRat)) {
    return false;
  }
  if (inputRat.size < 1) {
    return false;
  }
  const int maxPartitionSize = 1000;
  Vector<int> inputInt;
  inputInt.setSize(inputRat.size);
  for (int i = 0; i < inputRat.size; i ++) {
    if (!(inputRat[i].IsIntegerFittingInInt(&inputInt[i]))) {
      return theCommands << "Failed to convert input: " << input.toString() << " to a list of small integers.";
    }
  }
  for (int i = 0; i < inputInt.size; i ++) {
    if ((inputInt[i] < 1) || (inputInt[i] > maxPartitionSize)) {
      return theCommands <<  "Entry: " << inputInt[i] << " of " << inputInt << " is outside of the allowed input range.";
    }
  }
  if (inputInt.SumCoords() > maxPartitionSize) {
    return theCommands << "The coordinates of vector " << inputInt << " have sum that is too large. ";
  }
  Partition p;
  p.FromListInt(inputInt);
  std::stringstream out;
  out << "Partition is " << p.toString();
  List<Matrix<Rational> > gens;
  p.SpechtModuleMatricesOfTranspositionsjjplusone(gens);
  for (int i = 0; i < gens.size; i ++) {
    out << i << "\n" << gens[i].ToStringPlainText() << "\n";
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerHyperOctahedralAllModulesInducedFromSpechtModules(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerHyperOctahedralAllModulesInducedFromSpechtModules");
  int theRank = 0;
  if (!input.IsSmallInteger(&theRank)) {
    return false;
  }
  if (theRank < 1|| theRank > 7) {
    return theCommands << "Input of hyperoctahedral print function has to be between 1 and 10";
  }
  HyperoctahedralGroupData G;
  G.MakeHyperoctahedralGroup(theRank);
  G.AllSpechtModules();
  return output.AssignValue(G.theGroup->PrettyPrintCharacterTable(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerHyperOctahedralPrintGeneratorCommutationRelations(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerHyperOctahedralPrintGeneratorCommutationRelations");
  int theRank = 0;
  if (!input.IsSmallInteger(&theRank)) {
    return false;
  }
  if (theRank < 1 || theRank > 10) {
    return theCommands << "Input of hyperoctahedral print function has to be between 1 and 10";
  }
  HyperoctahedralGroupData G;
  G.MakeHyperoctahedralGroup(theRank);
  return output.AssignValue(G.theGroup->PrettyPrintGeneratorCommutationRelations(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupElement(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupElement");
  if (input.size() < 2) {
    return output.MakeError("Function WeylElement needs to know what group the element belongs to", theCommands);
  }
  WithContext<SemisimpleLieAlgebra*> thePointer;
  if (!theCommands.Convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, thePointer
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  ElementWeylGroup theElt;
  theElt.generatorsLastAppliedFirst.Reserve(input.size() - 2);
  for (int i = 2; i < input.size(); i ++) {
    int tmp;
    if (!input[i].IsSmallInteger(& tmp)) {
      return false;
    }
    theElt.MultiplyOnTheRightBySimpleReflection(tmp - 1);
  }
  theElt.owner = &thePointer.content->theWeyl;
  for (int i = 0; i < theElt.generatorsLastAppliedFirst.size; i ++) {
    if (
      theElt.generatorsLastAppliedFirst[i].index >= thePointer.content->GetRank() ||
      theElt.generatorsLastAppliedFirst[i].index < 0
    ) {
      return output.MakeError("Bad reflection index", theCommands);
    }
  }
  theElt.MakeCanonical();
  return output.AssignValue(theElt, theCommands);
}

bool Calculator::innerGenerateMultiplicativelyClosedSet(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerGenerateMultiplicativelyClosedSet");
  if (input.size() <= 2) {
    return output.MakeError("I need at least two arguments - upper bound and at least one element to multiply. ", theCommands);
  }
  int upperLimit;
  if (!input[1].IsSmallInteger(&upperLimit)) {
    return output.MakeError("First argument must be a small integer, serving as upper bound for the set. ", theCommands);
  }
  if (upperLimit <= 0) {
    upperLimit = 10000;
    theCommands << "The upper computation limit I got was 0 or less; I replaced it with the default value "
    << upperLimit << ".";
  }
  HashedList<Expression> theSet;
  theSet.setExpectedSize(input.size() - 2);
  for (int i = 2; i < input.size(); i ++) {
    theSet.addOnTop(input[i]);
  }
  int numGenerators = theSet.size;
  Expression theProduct, evaluatedProduct;
  ProgressReport theReport;
  for (int i = 0; i < theSet.size; i ++) {
    for (int j = 0; j < numGenerators; j ++) {
      theProduct.MakeProducT(theCommands, theSet[j], theSet[i]);
      std::stringstream reportStream;
      reportStream << "found " << theSet.size << "elements so far, exploring element " << i + 1;
      reportStream << "<br>Evaluating: " << theProduct.toString();
      theReport.report(reportStream.str());
      theCommands.EvaluateExpression(theCommands, theProduct, evaluatedProduct);
      //if (evaluatedProduct == theSet[0])
      //{
      //}
      theSet.addOnTopNoRepetition(evaluatedProduct);
      if (theSet.size >upperLimit) {
        std::stringstream out;
        out << "<hr>While generating multiplicatively closed set, I went above the upper limit of "
        << upperLimit << " elements.";
        evaluatedProduct.MakeError(out.str(), theCommands);
        theSet.addOnTop(evaluatedProduct);
        i = theSet.size; break;
      }
    }
  }
  theCommands << "<hr>Generated a list of " << theSet.size << " elements";
  output.reset(theCommands, theSet.size + 1);
  output.AddChildAtomOnTop(theCommands.opSequence());
  for (int i = 0; i < theSet.size; i ++) {
    output.addChildOnTop(theSet[i]);
  }
  return true;
}

template <typename somegroup, typename coefficient>
void VirtualRepresentation<somegroup, coefficient>::operator*=(const VirtualRepresentation<somegroup, coefficient>& other) {
  MacroRegisterFunctionWithName("VirtualRepresentation::operator*=");
  (void) other;
  global.fatal << "Not implemented yet. " << global.fatal;
/*  WeylGroupVirtualRepresentation<coefficient> output, currentContribution;
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
      tempRep.DecomposeTodorsVersion(currentContribution, 0);
      output.coefficientsIrreps+= currentContribution*theCoeff;
    }
  *this = output;*/
}

template <typename somegroup, typename coefficient>
void VirtualRepresentation<somegroup, coefficient>::AssignRep(
  const GroupRepresentationCarriesAllMatrices<somegroup, Rational>& other
) {
  global.fatal << " not implemented " << global.fatal;
  GroupRepresentationCarriesAllMatrices<somegroup, coefficient> otherCopy;
  otherCopy = other;
//  otherCopy.DecomposeTodorsVersion(this->coefficientsIrreps, global);
}

template <typename somegroup, typename coefficient>
void VirtualRepresentation<somegroup, coefficient>::AssignRep(const GroupRepresentation<somegroup, Rational>& other) {
  VirtualRepresentation<somegroup, coefficient> out;
  out.AddMonomial(other.theCharacteR, 1);
//  otherCopy.DecomposeTodorsVersion(this->coefficientsIrreps, global);
}

bool CalculatorFunctionsWeylGroup::innerMakeVirtualWeylRep(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerMakeVirtualWeylRep");
  if (input.IsOfType<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >()) {
    output = input;
    return true;
  }
  if (!input.IsOfType<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >()) {
    return false;
  }
  GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& inputRep =
  input.GetValueNonConst<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >();
  WeylGroupData* theWeylData = inputRep.ownerGroup->generators[0].owner;
  if (
    inputRep.ownerGroup->irreps.size<inputRep.ownerGroup->ConjugacyClassCount() &&
    theWeylData->theGroup.ComputeIrreducibleRepresentationsWithFormulas
  ) {
    theWeylData->theGroup.ComputeIrreducibleRepresentationsWithFormulas(theWeylData->theGroup);
  }
  if (inputRep.ownerGroup->irreps.size<inputRep.ownerGroup->ConjugacyClassCount()) {
    theWeylData->ComputeInitialIrreps();
    theWeylData->theGroup.ComputeIrreducibleRepresentationsTodorsVersion();
  }
  VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> outputRep;
  outputRep.AssignRep(inputRep);
  return output.AssignValue(outputRep, theCommands);
}

