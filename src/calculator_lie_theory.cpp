#include "calculator_lie_theory.h"
#include "calculator_inner_typed_functions.h"
#include "math_extra_differential_operators.h"
#include "math_extra_modules_semisimple_lie_algebras_implementation.h"
#include "math_extra_semisimple_lie_algebras_implementation.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_general_implementation.h"
#include "math_extra_weyl_algebras_implementation.h"
#include "math_rational_function_implementation.h"

MapList<std::string, VoganDiagram::DiagramType, MathRoutines::hashString> VoganDiagram::mapStringToType;

bool CalculatorLieTheory::writeGenVermaModAsDiffOperatorInner(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  Vectors<Polynomial<Rational> >& highestWeights,
  ExpressionContext& hwContext,
  Selection& selInducing,
  SemisimpleLieAlgebra* owner,
  bool AllGenerators,
  std::string* xLetter,
  std::string* partialLetter,
  std::string* exponentVariableLetter,
  bool useNilWeight,
  bool ascending
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::writeGenVermaModAsDiffOperatorInner");
   /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
  if (highestWeights.size == 0) {
    return false;
  }
  SemisimpleLieAlgebra& semisimpleLieAlgebra = *owner;
  List<ElementUniversalEnveloping<Polynomial<Rational> > > elementsNegativeNilrad;
  ElementSemisimpleLieAlgebra<Rational> generator;
  ElementUniversalEnveloping<Polynomial<Rational> > genericElement, actionOnGenericElement;
  List<QuasiDifferentialOperator<Rational> > quasiDifferentialOperators;
  FormatExpressions weylFormat, universalEnvelopingFormat;
  std::stringstream out, latexReport, latexReport2;
  Polynomial<Rational> zero;
  Polynomial<Rational> one;
  one.makeConstant(Rational::one());
  weylFormat.maximumLineLength = 40;
  weylFormat.flagUseLatex = true;
  universalEnvelopingFormat.maximumLineLength = 20;
  universalEnvelopingFormat.flagUseLatex = true;
  universalEnvelopingFormat.chevalleyGGeneratorLetter = "g";
  universalEnvelopingFormat.chevalleyHGeneratorLetter = "h";
  hwContext.getFormat(universalEnvelopingFormat);
  universalEnvelopingFormat.polyDefaultLetter = exponentVariableLetter == nullptr  ? "a" : *exponentVariableLetter;
  universalEnvelopingFormat.maximumLineLength = 178;
  universalEnvelopingFormat.numberOfAmpersandsPerNewLineForLaTeX = 2;
  weylFormat.numberOfAmpersandsPerNewLineForLaTeX = 2;
  hwContext.getFormat(weylFormat);
  List<ElementSemisimpleLieAlgebra<Rational> > generatorsItry;
  if (!AllGenerators) {
    for (int j = 0; j < semisimpleLieAlgebra.getRank(); j ++) {
      Vector<Rational> ei;
      ei.makeEi(semisimpleLieAlgebra.getRank(), j);
      generator.makeGGenerator(ei, semisimpleLieAlgebra);
      generatorsItry.addOnTop(generator);
      ei.negate();
      generator.makeGGenerator(ei, semisimpleLieAlgebra);
      generatorsItry.addOnTop(generator);
    }
  } else {
    for (int j = 0; j < semisimpleLieAlgebra.getNumberOfGenerators(); j ++) {
      generator.makeGenerator(j, semisimpleLieAlgebra);
      generatorsItry.addOnTop(generator);
    }
  }
  quasiDifferentialOperators.setSize(generatorsItry.size);
  out << "<table border =\"1\">";
  latexReport << "\\begin{longtable}{rll";
  for (int i = 0; i < generatorsItry.size; i ++) {
    latexReport << "l";
  }
  latexReport << "}\\caption{\\label{tableDiffOps" << selInducing.toString()
  << "} Differential operators corresponding to actions"
  << " of simple positive generators for the " << selInducing.toString() << "-parabolic subalgebra.}\\\\<br>";
  List<ModuleSSalgebra<RationalFraction<Rational> > > theMods;
  theMods.setSize(highestWeights.size);
  Vector<RationalFraction<Rational> > tempV;
  int numStartingVars = hwContext.numberOfVariables();
  std::stringstream reportfourierTransformedCalculatorCommands, reportCalculatorCommands;
  long long totalAdditions = 0;
  long long currentAdditions = 0;
  long long totalMultiplications = 0;
  long long currentMultiplications = 0;
  double totalTime = 0, currentTime = 0;
  for (int i = 0; i < highestWeights.size; i ++) {
    ModuleSSalgebra<RationalFraction<Rational> >& theMod = theMods[i];
    tempV = highestWeights[i];
    if (!theMod.makeFromHW(
      semisimpleLieAlgebra,
      tempV,
      selInducing,
      RationalFraction<Rational>(Rational::one()),
      RationalFraction<Rational>(Rational::zero()),
      nullptr,
      true
    )) {
      return output.assignError(calculator, "Failed to create module.");
    }
    if (i == 0) {
      theMod.getElementsNilradical(elementsNegativeNilrad, true, nullptr, useNilWeight, ascending);
      Polynomial<Rational> Pone, Pzero;
      Pone.makeOne();
      Pzero.makeZero();
      theMod.getGenericUnMinusElt(true, genericElement, useNilWeight, ascending);
      weylFormat.polynomialAlphabet.setSize(numStartingVars + elementsNegativeNilrad.size);
      weylFormat.weylAlgebraLetters.setSize(numStartingVars + elementsNegativeNilrad.size);
      universalEnvelopingFormat.polynomialAlphabet.setSize(numStartingVars + elementsNegativeNilrad.size);
      for (int k = 0; k < numStartingVars; k ++) {
        weylFormat.weylAlgebraLetters[k] = "error";
      }
      std::string finalXletter = (xLetter == nullptr) ? "x": *xLetter;
      std::string finalPartialLetter = (partialLetter == nullptr) ? "\\partial" : *partialLetter;
      for (int k = numStartingVars; k < universalEnvelopingFormat.polynomialAlphabet.size; k ++) {
        std::stringstream tmpStream, tempstream2, tempstream3, tempStream4;
        tmpStream << universalEnvelopingFormat.polyDefaultLetter << "_{" << k - hwContext.numberOfVariables() + 1 << "}";
        universalEnvelopingFormat.polynomialAlphabet[k] = tmpStream.str();
        tempstream2 << finalXletter << "_{" << k-numStartingVars + 1 << "}";
        tempstream3 << finalXletter << "_" << k-numStartingVars + 1;
        tempStream4 << finalPartialLetter << "_{" << k-numStartingVars + 1 << "}";
        if (
          weylFormat.polynomialAlphabet.contains(tempstream2.str()) ||
          weylFormat.polynomialAlphabet.contains(tempstream3.str())
        ) {
          return output.assignError(
            calculator,
            "Error: the variable " +
            tempstream2.str() +
            " is reserved for me: you are not allowed to use it as a coordinate of the highest weight. "
          );
        }
        weylFormat.polynomialAlphabet[k] = tempstream2.str();
        weylFormat.weylAlgebraLetters[k] = tempStream4.str();
      }
      out << "<tr><td>General monomial in U(n_-):</td><td>"
      << HtmlRoutines::getMathNoDisplay(genericElement.toString(&universalEnvelopingFormat)) << "</td> </tr>";
      latexReport << "& \\multicolumn{" << generatorsItry.size << "}{c}{Element acting}\\\\<br>\n ";
      latexReport << "Action on ";
      out << "<tr><td></td><td colspan =\"" << generatorsItry.size << "\"> Element acting</td></td></tr>";
      out << "<tr><td>Action on</td>";
      for (int j = 0; j < generatorsItry.size; j ++) {
        out << "<td>" << generatorsItry[j].toString(&universalEnvelopingFormat) << "</td>";
        latexReport << "& $" << generatorsItry[j].toString(&universalEnvelopingFormat)  << "$";
      }
      latexReport << "\\endhead \\hline<br>";
      out << "</tr>";
      out << "<tr><td>" << HtmlRoutines::getMathNoDisplay(genericElement.toString(&universalEnvelopingFormat)) << "</td>";
      latexReport << "$" << genericElement.toString(&universalEnvelopingFormat) << "$";
      for (int j = 0; j < generatorsItry.size; j ++) {
        actionOnGenericElement.assignElementLieAlgebra(generatorsItry[j], semisimpleLieAlgebra, Pone);
        actionOnGenericElement *= genericElement;
        semisimpleLieAlgebra.orderNilradical(theMod.parabolicSelectionNonSelectedAreElementsLevi, useNilWeight, ascending);
        actionOnGenericElement.simplify(one);
        universalEnvelopingFormat.numberOfAmpersandsPerNewLineForLaTeX = 2;
        out << "<td>" << HtmlRoutines::getMathNoDisplay("\\begin{array}{rcl}&&" + actionOnGenericElement.toString(&universalEnvelopingFormat) + "\\end{array}") << "</td>";
        universalEnvelopingFormat.numberOfAmpersandsPerNewLineForLaTeX = 0;
        latexReport << "& $\\begin{array}{l} " << actionOnGenericElement.toString(&universalEnvelopingFormat) << "\\end{array}$ ";
      }
      latexReport << "\\\\ \\hline\\hline<br>";
      out << "</tr>";
    }
    out << "<tr><td>" << HtmlRoutines::getMathNoDisplay(theMod.character.toString()) << "</td>";
    latexReport2 << "\\begin{longtable}{rll}";
    latexReport2 << "$\\gog$& $n$& element of $\\mathbb W_n$ \\\\\\hline" << "\\multirow{" << generatorsItry.size
    << "}{*}{$" << semisimpleLieAlgebra.toStringLieAlgebraName() << "$}" << " &  \\multirow{"  << generatorsItry.size << "}{*}{"
    << elementsNegativeNilrad.size << "}&";

    latexReport << "$\\begin{array}{r}" << theMod.character.toString()
    << "(\\mathfrak{l}) \\\\ \\\\dim:~" << theMod.getDimension() << " \\end{array}$";
    for (int j = 0; j < generatorsItry.size; j ++) {
      generator = generatorsItry[j];
      currentTime = global.getElapsedSeconds();
      currentAdditions = Rational::totalAdditions();
      currentMultiplications = Rational::totalMultiplications();
      theMod.getActionGeneralizedVermaModuleAsDifferentialOperator(
        generator, quasiDifferentialOperators[j], useNilWeight, ascending
      );
      totalAdditions += Rational::totalAdditions() - currentAdditions;
      totalMultiplications += Rational::totalMultiplications() - currentMultiplications;
      totalTime += global.getElapsedSeconds() - currentTime;
      weylFormat.customCoefficientMonomialSeparator = "\\otimes ";
      weylFormat.numberOfAmpersandsPerNewLineForLaTeX = 2;
      out << "<td>" << HtmlRoutines::getMathNoDisplay(
        "\\begin{array}{|r|c|l|}&&" + quasiDifferentialOperators[j].toString(&weylFormat) + "\\end{array}"
      ) << "</td>";
      weylFormat.numberOfAmpersandsPerNewLineForLaTeX = 0;
      weylFormat.maximumLineLength = 300;
      latexReport << " & $\\begin{array}{l}" << quasiDifferentialOperators[j].toString(&weylFormat) << "\\end{array}$";
      if (j != 0) {
        latexReport2 << "&&";
      }
      latexReport2 << " $\\begin{array}{l}" << quasiDifferentialOperators[j].toString(&weylFormat) << "\\end{array}$\\\\ "
      << (j != generatorsItry.size - 1 ? "\\cline{3-3}" : "\\hline" ) << "\n<br>";
      weylFormat.customCoefficientMonomialSeparator = "";
    }
    latexReport2 << "\\end{longtable}";
    latexReport << "\\\\\\hline<br>";
    out << "</tr>";
    if (theMod.getDimension() == 1) {
      ElementWeylAlgebra<Rational> diffOpPart, transformedDO;
      reportfourierTransformedCalculatorCommands << "<hr>" << HtmlRoutines::getMathNoDisplay(theMod.character.toString())
      << ", differential operators Fourier transformed - formatted for calculator input. <br><br>";
      reportfourierTransformedCalculatorCommands << "x_{{i}}= ElementWeylAlgebraPoly{}(\\partial_i, x_i);\n<br>"
      << "\\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n";
      reportCalculatorCommands << "<hr>" << HtmlRoutines::getMathNoDisplay(theMod.character.toString())
      << ", differential operators - formatted for calculator input. <br><br>";
      reportCalculatorCommands << "x_{{i}}= ElementWeylAlgebraPoly{}(\\partial_i, x_i);\n<br>"
      << "\\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n";

      for (int j = 0; j < generatorsItry.size; j ++) {
        quasiDifferentialOperators[j].getElementWeylAlgebraSetMatrixPartsToId(diffOpPart);
        diffOpPart.fourierTransform(transformedDO);
        reportfourierTransformedCalculatorCommands << "<br>"
        << generatorsItry[j].toString() << "=" << transformedDO.toString() << ";";
        reportCalculatorCommands << "<br>" << generatorsItry[j].toString() << "=" << diffOpPart.toString() << ";";
      }
      reportfourierTransformedCalculatorCommands << "<br>generateVectorSpaceClosedWithRespectToLieBracket{}(248," ;
      reportCalculatorCommands << "<br>generateVectorSpaceClosedWithRespectToLieBracket{}(248," ;
      for (int j = 0; j < generatorsItry.size; j ++) {
        reportfourierTransformedCalculatorCommands << generatorsItry[j].toString();
        reportCalculatorCommands << generatorsItry[j].toString();
        if (j != generatorsItry.size - 1) {
          reportfourierTransformedCalculatorCommands << ", ";
          reportCalculatorCommands << ", ";
        }
      }
      reportfourierTransformedCalculatorCommands << ");";
      reportfourierTransformedCalculatorCommands << "<hr>";
      reportCalculatorCommands << ");";
      reportCalculatorCommands << "<hr>";
    }
  }
  latexReport << "\\end{longtable}";
  out << "</table>";
  out << "<br>Performance notes: see comments. ";
  //please keep this report in the comments section for reasons of reproducibility of the
  //string output!
  calculator << "<hr>"
  << "Multiplications needed to embed generators: " << totalMultiplications << ".<br>"
  << "Additions needed to embed generators: " << totalAdditions << ".<br>"
  << "Total time to embed generators invoked by command: " << input.toString()
  << ": <br>" << totalTime << " seconds. ";
  out << reportCalculatorCommands.str();
  out << reportfourierTransformedCalculatorCommands.str();
  out << "<br>" << latexReport.str();
  out << "<br><br>" << latexReport2.str();
  return output.assignValue<std::string>(calculator, out.str());
}

bool CalculatorLieTheory::highestWeightVectorCommon(
  Calculator& calculator,
  Expression& output,
  Vector<RationalFraction<Rational> >& highestWeightFundCoords,
  Selection& selectionParSel,
  ExpressionContext& hwContext,
  SemisimpleLieAlgebra* owner,
  bool Verbose
) {
  MacroRegisterFunctionWithName("Calculator::highestWeightVectorCommon");
  RecursionDepthCounter recursionIncrementer(&calculator.recursionDepth);
  RationalFraction<Rational> RFOne, RFZero;
  RFOne.makeOne();
  RFZero.makeZero();
  std::string report;
  ElementTensorsGeneralizedVermas<RationalFraction<Rational> > element;
  ListReferences<ModuleSSalgebra<RationalFraction<Rational> > >& allModules = calculator.objectContainer.categoryOModules;
  int indexOfModule = - 1;

  for (int i = 0; i < allModules.size; i ++) {
    ModuleSSalgebra<RationalFraction<Rational> >& currentMod = allModules[i];
    if (
      highestWeightFundCoords == currentMod.highestWeightFundamentalCoordinatesBaseField &&
      selectionParSel == currentMod.parabolicSelectionNonSelectedAreElementsLevi &&
      currentMod.owner == owner
    ) {
      indexOfModule = i;
      break;
    }
  }
  if (indexOfModule == - 1) {
    indexOfModule = allModules.size;
    allModules.setSize(allModules.size + 1);
    allModules.lastObject().reset();
  }
  ModuleSSalgebra<RationalFraction<Rational> >& currentModule = allModules[indexOfModule];
  if (!currentModule.flagIsInitialized) {
    bool isGood = currentModule.makeFromHW(*owner, highestWeightFundCoords, selectionParSel, RFOne, RFZero, &report);
    if (Verbose) {
      calculator << currentModule.toString();
    }
    if (!isGood) {
      return output.assignError(calculator, "Error while generating highest weight module. See comments for details. ");
    }
  }
  if (&currentModule.getOwner() != owner) {
    global.fatal << "Module has owner that is not what it should be. " << global.fatal;
  }
  element.makeHighestWeightVector(currentModule, RFOne);
  if (&element.getOwnerSemisimple() != owner) {
    global.fatal << "Just created an ElementTensorsGeneralizedVermas "
    << "whose owner is not what it should be. " << global.fatal;
  }
  return output.assignValueWithContext(calculator, element, hwContext);
}

bool CalculatorLieTheory::animateLittelmannPaths(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::animateLittelmannPaths");
  RecursionDepthCounter recursionCounter(&calculator.recursionDepth);
  if (!input.isListNElements(3)) {
    return output.assignError(calculator, "This function takes 2 arguments");
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!CalculatorConversions::convert(
    calculator,
    input[1],
    algebra
  )) {
    return output.assignError(calculator, "Error extracting Lie algebra.");
  }
  SemisimpleLieAlgebra* semisimpleLieAlgebraOwner = algebra.content;
  Vector<Rational> weight;
  ExpressionContext tempContext(calculator);
  if (!calculator.getVector<Rational>(
    input[2],
    weight,
    &tempContext,
    semisimpleLieAlgebraOwner->getRank()
  )) {
    return output.assignError(
      calculator,
      "Failed to convert the argument of the function to a highest weight vector"
    );
  }
  Vector<Rational> weightInSimpleCoords;
  weightInSimpleCoords = semisimpleLieAlgebraOwner->weylGroup.getSimpleCoordinatesFromFundamental(weight, Rational::zero());
  calculator << "<br>Function animateLittelmannPaths: your input in simple coords: "
  << weightInSimpleCoords.toString();
  LittelmannPath path;
  path.makeFromWeightInSimpleCoords(weightInSimpleCoords, semisimpleLieAlgebraOwner->weylGroup);
  return output.assignValue(calculator, path.generateOrbitAndAnimate());
}

bool CalculatorLieTheory::splitFDpartB3overG2inner(
  Calculator& calculator, BranchingData& g2B3Data, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::splitFDpartB3overG2inner");
  ModuleSSalgebra<RationalFraction<Rational> > theModCopy;
  theModCopy.makeFromHW(
    g2B3Data.homomorphism.coDomainAlgebra(),
    g2B3Data.weightFundamentalCoordinates,
    g2B3Data.inducing,
    Rational::one(),
    Rational::zero(),
    nullptr,
    false
  );
  g2B3Data.resetOutputData();
  g2B3Data.initializeAfterParabolicSelectionAndHomomorphism();
  g2B3Data.splittingParabolicSelection = g2B3Data.inducing;
  if (g2B3Data.splittingParabolicSelection.selected[0] != g2B3Data.splittingParabolicSelection.selected[2]) {
    g2B3Data.splittingParabolicSelection.addSelectionAppendNewIndex(0);
    g2B3Data.splittingParabolicSelection.addSelectionAppendNewIndex(2);
  }
  Vector<Rational> splittingParSel;
  splittingParSel = g2B3Data.splittingParabolicSelection;

  calculator.objectContainer.categoryOModules.addNoRepetitionOrReturnIndexFirst(theModCopy);
  int theModIndex = calculator.objectContainer.categoryOModules.getIndex(theModCopy);
  ModuleSSalgebra<RationalFraction<Rational> >& theMod = calculator.objectContainer.categoryOModules[theModIndex];
  theMod.getOwner().flagHasNilradicalOrder = true;
  std::stringstream out;
  calculator << "<hr>Time elapsed before making B3 irrep: " << global.getElapsedSeconds();
  double timeAtStart = global.getElapsedSeconds();
  theMod.splitFDpartOverFKLeviRedSubalg(
    g2B3Data.homomorphism,
    g2B3Data.smallParabolicSelection,
    &g2B3Data.outputEigenWords,
    &g2B3Data.outputWeightsFundCoordS,
    &g2B3Data.leviEigenSpace,
    nullptr
  );
  calculator << "<br>Time needed to make B3 irrep: " << global.getElapsedSeconds() - timeAtStart;
  g2B3Data.g2Weights.setSize(g2B3Data.outputWeightsFundCoordS.size);
  g2B3Data.g2DualWeights.setSize(g2B3Data.outputWeightsFundCoordS.size);
  Matrix<Rational> invertedG2cartanMat;
  invertedG2cartanMat = g2B3Data.homomorphism.domainAlgebra().weylGroup.cartanSymmetric;
  invertedG2cartanMat.invert();
  WeylGroupData& rangeWeyl = g2B3Data.homomorphism.coDomainAlgebra().weylGroup;
  RationalFraction<Rational> zero(Rational::zero());
  RationalFraction<Rational> one(Rational::one());
  g2B3Data.outputWeightsSimpleCoords = rangeWeyl.getSimpleCoordinatesFromFundamental(
    g2B3Data.outputWeightsFundCoordS, zero
  );
  Vector<RationalFraction<Rational> > weightSimpleCoordinates;
  weightSimpleCoordinates = rangeWeyl.getSimpleCoordinatesFromFundamental(
    g2B3Data.weightFundamentalCoordinates, zero
  );
  g2B3Data.ambientCharacter.makeFromWeight(weightSimpleCoordinates, &g2B3Data.homomorphism.coDomainAlgebra());
  g2B3Data.smallCharacterFiniteDimensionalPart.makeZero();
  CharacterSemisimpleLieAlgebraModule<RationalFraction<Rational> > tempMon;
  for (int i = 0; i < g2B3Data.outputWeightsSimpleCoords.size; i ++) {
    Vector<RationalFraction<Rational> >& currentWeight = g2B3Data.outputWeightsSimpleCoords[i];
    Vector<RationalFraction<Rational> >& currentG2Weight = g2B3Data.g2Weights[i];
    Vector<RationalFraction<Rational> >& currentG2DualWeight = g2B3Data.g2DualWeights[i];
    currentG2DualWeight.setSize(2);
    currentG2DualWeight[0] = g2B3Data.homomorphism.coDomainAlgebra().weylGroup.rootScalarCartanRoot(
      currentWeight, g2B3Data.homomorphism.imagesCartanDomain[0]
    );
    //<-note: implicit type conversion: the return type is the left coefficient type.
    currentG2DualWeight[1] = g2B3Data.homomorphism.coDomainAlgebra().weylGroup.rootScalarCartanRoot(
      currentWeight, g2B3Data.homomorphism.imagesCartanDomain[1]
    );
    //<-note: implicit type conversion: the return type is the left coefficient type.
    invertedG2cartanMat.actOnVectorColumn(currentG2DualWeight, currentG2Weight, zero);//<-g2weight is now computed;
    tempMon.makeFromWeight(currentG2Weight, &g2B3Data.homomorphism.domainAlgebra());
    g2B3Data.smallCharacterFiniteDimensionalPart += tempMon;
  }
  ElementUniversalEnveloping<RationalFraction<Rational> > g2Casimir, g2CasimirCopy, imageCasimirInB3, element;
  g2Casimir.makeCasimir(g2B3Data.homomorphism.domainAlgebra());

  g2B3Data.allCharacters.setSize(g2B3Data.outputWeightsFundCoordS.size);
  for (int i = 0; i < g2B3Data.outputWeightsSimpleCoords.size; i ++) {
    Vector<RationalFraction<Rational> >& currentG2DualWeight = g2B3Data.g2DualWeights[i];
    g2CasimirCopy = g2Casimir;
    g2CasimirCopy.modOutVermaRelations(&currentG2DualWeight, one, zero);
    if (g2CasimirCopy.isEqualToZero()) {
      g2B3Data.allCharacters[i] = 0;
    } else {
      g2B3Data.allCharacters[i] = g2CasimirCopy.coefficients[0];
    }
  }
  g2B3Data.eigenVectorsLevi.setSize(g2B3Data.g2Weights.size);
  g2B3Data.eigenVectors.setSize(g2B3Data.g2Weights.size);
  g2B3Data.additionalMultipliers.setSize(g2B3Data.g2Weights.size);
  g2B3Data.shapovalovProducts.setSize(g2B3Data.g2Weights.size);
  g2B3Data.elementsUniversalEnveloping.setSize(g2B3Data.g2Weights.size);
  ElementSumGeneralizedVermas<RationalFraction<Rational> >& theHWV = *g2B3Data.eigenVectorsLevi.lastObject();
  theHWV.makeHighestWeightVector(theMod, one);
  theHWV *= - 1;
  *g2B3Data.eigenVectors.lastObject() = theHWV;
  Vector<RationalFraction<Rational> > weightDifference;
  g2B3Data.homomorphism.applyHomomorphism(g2Casimir, imageCasimirInB3);
  g2Casimir.checkConsistency();
  imageCasimirInB3.checkConsistency();
  RationalFraction<Rational> charDiff;
  g2B3Data.homomorphism.coDomainAlgebra().orderNilradical(
    theMod.parabolicSelectionNonSelectedAreElementsLevi,
    g2B3Data.flagUseNilWeightGeneratorOrder,
    g2B3Data.flagAscendingGeneratorOrder
  );
  for (int k = 0; k < g2B3Data.g2Weights.size; k ++) {
    ElementSumGeneralizedVermas<RationalFraction<Rational> >& currentTensorEltLevi = g2B3Data.eigenVectorsLevi[k];
    ElementSumGeneralizedVermas<RationalFraction<Rational> >& currentTensorEltEigen = g2B3Data.eigenVectors[k];
    ElementUniversalEnveloping<RationalFraction<Rational> >& currentUEelt = g2B3Data.elementsUniversalEnveloping[k];
    currentTensorEltLevi = theHWV;
    currentTensorEltLevi.multiplyMeByUEEltOnTheLeft(g2B3Data.outputEigenWords[k]);
    currentTensorEltEigen = currentTensorEltLevi;
    if (g2B3Data.inducing.cardinalitySelection > 0) {
      for (int j = 0; j < g2B3Data.g2Weights.size; j ++) {
        weightDifference = g2B3Data.g2Weights[j] - g2B3Data.g2Weights[k];
        if (weightDifference.isPositive()) {
          g2CasimirCopy = imageCasimirInB3;
          element.makeConstant(g2B3Data.allCharacters[j], g2B3Data.homomorphism.coDomainAlgebra());
          g2CasimirCopy -= element;
          g2CasimirCopy *= Rational(12);
          currentTensorEltEigen.multiplyMeByUEEltOnTheLeft(g2CasimirCopy);
          charDiff = g2B3Data.allCharacters[j];
          charDiff -= *g2B3Data.allCharacters.lastObject();
          g2B3Data.characterDifferences.addOnTopNoRepetition(charDiff);
        }
      }
    }
    RationalFraction<Rational> scale = currentTensorEltEigen.scaleNormalizeLeadingMonomial(nullptr);
    if (!scale.isConstant(&g2B3Data.additionalMultipliers[k])) {
      global.fatal << "This is unexpected: the scale is not a constant. " << global.fatal;
    }
    currentTensorEltEigen.extractElementUniversalEnveloping(currentUEelt, *theMod.owner);
    currentUEelt.highestWeightTransposeAntiAutomorphismBilinearForm(
      currentUEelt, g2B3Data.shapovalovProducts[k], &theMod.highestWeightDualCoordinatesBaseField, one, zero, nullptr
    );
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::testMonomialBaseConjecture(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::testMonomialBaseConjecture");
  RecursionDepthCounter theRecursion(&calculator.recursionDepth);
  if (!input.isListNElements(3)) {
    return output.assignError(calculator, "testMonomialBaseConjecture takes two arguments as input");
  }
  const Expression& rankE = input[1];
  const Expression& dimE = input[2];
  int rankBound = 0;
  int dimBound = 0;
  if (!rankE.isSmallInteger(&rankBound) || !dimE.isSmallInteger(&dimBound)) {
    return output.assignError(calculator, "The rank and  dim bounds must be small integers");
  }
  if (rankBound < 2 || rankBound > 100 || dimBound < 1 || dimBound > 10000) {
    return output.assignError(
      calculator,
      "The rank bound must be an integer between 2 and 100, "
      "and the dim bound must be an integer between 1 and 10000. "
    );
  }
  std::stringstream out;
  List<int> ranks;
  List<char> weylLetters;
  for (int i = 2; i <= rankBound; i ++) {
    ranks.addOnTop(i);
    weylLetters.addOnTop('A');
    ranks.addOnTop(i);
    weylLetters.addOnTop('B');
    ranks.addOnTop(i);
    weylLetters.addOnTop('C');
    if (i >= 4) {
      ranks.addOnTop(i);
      weylLetters.addOnTop('D');
    }
    if (i >= 6 && i <= 8) {
      ranks.addOnTop(i);
      weylLetters.addOnTop('E');
    }
    if (i == 4) {
      ranks.addOnTop(i);
      weylLetters.addOnTop('F');
    }
    if (i == 2) {
      ranks.addOnTop(i);
      weylLetters.addOnTop('G');
    }
  }
  List<List<Vector<Rational> > > theHighestWeights;
  theHighestWeights.setSize(ranks.size);
  bool foundBad = false;
  Selection tempSel;
  std::stringstream latexReport;
  latexReport << "\\documentclass{article} <br>\\usepackage{longtable}\\begin{document}<br>\n\n\n\n\n";
  latexReport << " \\begin{longtable}{|lllll|} ";
  ProgressReport report;
  bool ConjectureBholds = true;
  bool ConjectureCholds = true;
  LittelmannPath hwPath;
  List<LittelmannPath> tempList;
  List<List<int> > integerStrings;
  MonomialTensor<int, HashFunctions::hashFunction> tempMon;
  DynkinType currentType;
  for (int i = 0; i < ranks.size; i ++) {
    currentType.makeSimpleType(weylLetters[i], ranks[i]);
    SemisimpleLieAlgebra& currentAlg =
    calculator.objectContainer.getLieAlgebraCreateIfNotPresent(currentType);
    currentAlg.computeChevalleyConstants();
    currentAlg.weylGroup.getHighestWeightsAllRepresentationsDimensionLessThanOrEqualTo(theHighestWeights[i], dimBound);
    latexReport << "\\hline\\multicolumn{5}{c}{" << "$" << currentAlg.toStringLieAlgebraName() << "$}\\\\\\hline\n\n"
    << "$\\lambda$ & dim &\\# pairs 1& \\# pairs total  & \\# Arithmetic op.  \\\\\\hline";
    out << "<br>" << " <table><tr><td  border =\"1\" colspan =\"3\">"
    << weylLetters[i] << "_{" << ranks[i] << "}" << "</td></tr> <tr><td>highest weight</td><td>dim</td></tr>";
    List<Vector<Rational> >& theHws = theHighestWeights[i];
    tempSel.initialize(ranks[i]);
    for (int j = 0; j < theHws.size; j ++) {
      std::stringstream reportStream;
      Vector<Rational>& currentHW = theHws[j];
      out << "<tr><td> " << currentHW.toString() << "</td><td>"
      << currentAlg.weylGroup.weylDimFormulaFundamentalCoords(currentHW) << "</td>";
      reportStream << "Processing " << currentAlg.toStringLieAlgebraName() << ", index  "
      << i + 1 << " out of " << ranks.size << ",  highest weight "
      << currentHW.toString() << ", dim: " << currentAlg.weylGroup.weylDimFormulaFundamentalCoords(currentHW)
      << ", index " << j + 1 << " out of " << theHws.size;
      report.report(reportStream.str());
      latexReport << "$" << currentHW.toStringLetterFormat("\\omega") << "$ &"
      << currentAlg.weylGroup.weylDimFormulaFundamentalCoords(currentHW) << "&";
      hwPath.makeFromWeightInSimpleCoords(
        currentAlg.weylGroup.getSimpleCoordinatesFromFundamental(currentHW, Rational::zero()), currentAlg.weylGroup
      );
      hwPath.generateOrbit(
        tempList,
        integerStrings,
        MathRoutines::minimum(1000, currentAlg.weylGroup.weylDimFormulaFundamentalCoords(currentHW).numeratorShort),
        nullptr
      );
      reportStream << "\nPath orbit size = " << integerStrings.size
      << " generated in " << global.getElapsedSeconds() << " seconds. ";
      report.report(reportStream.str());
      for (int k = 0; k < integerStrings.size; k ++) {
        LittelmannPath& currentPath = tempList[k];
        tempMon = integerStrings[k];
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
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::littelmannOperator(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::littelmannOperator");
  RecursionDepthCounter theRecursionIncrementer(&calculator.recursionDepth);
  if (input.hasBoundVariables()) {
    return false;
  }
  int index = 0;
  if (!input.isSmallInteger(&index)) {
    return output.assignError(
      calculator,
      "The argument of the Littelmann root operator is "
      "expected to be a small integer, instead you gave me " +
      input.toString()
    );
  }
  if (index == 0) {
    return output.assignError(calculator, "The index of the Littelmann root operator is expected to be non-zero.");
  }
  return output.assignValue(calculator, index);
}

bool CalculatorLieTheory::LSPath(Calculator& calculator, const Expression& input, Expression& output) {
  RecursionDepthCounter theRecutionIncrementer(&calculator.recursionDepth);
  MacroRegisterFunctionWithName("Calculator::LSPath");
  if (input.size() < 3) {
    return output.assignError(calculator, "LSPath needs at least two arguments.");
  }
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebraPointer;
  if (!CalculatorConversions::convert(
    calculator,
    input[1],
    semisimpleLieAlgebraPointer
  )) {
    return output.assignError(calculator, "Error extracting Lie algebra.");
  }
  SemisimpleLieAlgebra& semisimpleLieAlgebra = *semisimpleLieAlgebraPointer.content;
  Vectors<Rational> waypoints;
  waypoints.setSize(input.size() - 2);
  for (int i = 2; i < input.size(); i ++) {
    if (!calculator.getVector<Rational>(
      input[i],
      waypoints[i - 2],
      nullptr,
      semisimpleLieAlgebra.getRank()
    )) {
      return output.assignError(calculator, "Failed to extract waypoints");
    }
  }
  waypoints = semisimpleLieAlgebra.weylGroup.getSimpleCoordinatesFromFundamental(waypoints, Rational::zero());
  LittelmannPath theLSpath;
  theLSpath.makeFromWaypoints(waypoints, semisimpleLieAlgebra.weylGroup);
  return output.assignValue(calculator, theLSpath);
}

bool CalculatorLieTheory::kazhdanLuzstigCoeffificents(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::kazhdanLuzstigCoeffificents");
  if (input.size() != 2) {
    return calculator
    << "Kazhdan-Lusztig coefficients function expects 1 argument. ";
  }
  RecursionDepthCounter theRecursionIncrementer(&calculator.recursionDepth);
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebra;
  if (!CalculatorConversions::convert(
    calculator,
    input[1],
    semisimpleLieAlgebra
  )) {
    return output.assignError(calculator, "Error extracting Lie algebra.");
  }
  std::stringstream out;
  WeylGroupData& weylGroup = semisimpleLieAlgebra.content->weylGroup;
  if (weylGroup.group.getSize() > 192) {
    out << "I have been instructed to run only for Weyl groups that"
    << " have at most 192 elements (i.e. no larger than D_4). "
    << semisimpleLieAlgebra.content->toStringLieAlgebraName()
    << " has " << weylGroup.group.getSize().toString() << ".";
    return output.assignValue(calculator, out.str());
  }
  FormatExpressions format;
  format.polynomialAlphabet.setSize(1);
  format.polynomialAlphabet[0] = "q";
  out << "Our notation follows that of the original Kazhdan-Lusztig paper, "
  << "Representations of Coxeter Groups and Hecke Algebras.<br>";
  out << " The algebra: " << semisimpleLieAlgebra.content->toStringLieAlgebraName();
  KazhdanLusztigPolynomials theKLpolys;
  theKLpolys.computeKLPolys(&weylGroup);
  format.flagUseHTML = true;
  out << theKLpolys.toString(&format);
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::writeGenVermaModAsDiffOperators(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool AllGenerators,
  bool useNilWeight,
  bool ascending
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::writeGenVermaModAsDiffOperators");
  RecursionDepthCounter theRecursionIncrementer(&calculator.recursionDepth);
  Vectors<Polynomial<Rational> > theHWs;
  theHWs.setSize(1);
  Selection theParSel;
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebra;
  Expression truncatedInput = input;
  if (truncatedInput.size() > 4) {
    int numEltsToCut = truncatedInput.size() - 4;
    for (int i = 0; i < numEltsToCut; i ++) {
      truncatedInput.removeLastChild();
    }
  }
  if (!calculator.getTypeHighestWeightParabolic<Polynomial<Rational> >(
    calculator,
    truncatedInput,
    output,
    theHWs[0],
    theParSel,
    semisimpleLieAlgebra
  )) {
    return output.assignError(
      calculator,
      "Failed to extract type, highest weight, parabolic selection"
    );
  }
  if (output.isError()) {
    return true;
  }
  std::string letterString = "x";
  std::string partialString = "\\partial";
  std::string exponentLetterString = "a";
  if (input.size() > 4) {
    letterString = input[4].toString();
  }
  if (input.size() > 5) {
    partialString = input[5].toString();
  }
  if (input.size() > 6) {
    exponentLetterString = input[6].toString();
  }
  return CalculatorLieTheory::writeGenVermaModAsDiffOperatorInner(
    calculator,
    input,
    output,
    theHWs,
    semisimpleLieAlgebra.context,
    theParSel,
    semisimpleLieAlgebra.content,
    AllGenerators,
    &letterString,
    &partialString,
    &exponentLetterString,
    useNilWeight,
    ascending
  );
}

bool CalculatorLieTheory::printB3G2branchingIntermediate(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  Vectors<RationalFraction<Rational> >& highestWeights,
  BranchingData& g2inB3Data,
  ExpressionContext& context
) {
  MacroRegisterFunctionWithName("Calculator::printB3G2branchingIntermediate");
  (void) input;
  std::stringstream out, timeReport;
  std::stringstream latexTable, latexTable2;
  bool isFD = (g2inB3Data.inducing.cardinalitySelection == 0);
  if (isFD) {
    out << "<table border =\"1\"><tr><td>$so(7)$-highest weight</td><td>Decomposition over $G_2$</td>"
    <<  "<td>$G_2\\cap b$-eigenvectors </td></tr>";
    latexTable << "\\begin{longtable}{|ccccl|} \\caption{\\label{tableB3fdsOverG2charsAndHWV} "
    << " Decompositions of finite dimensional $so(7)$-modules over $G_2$}\\\\"
    << "\\hline so(7)& dim. &$G_2$&dim.& $\\mathfrak b \\cap G_2$-singular vectors\\\\ \\hline"
    << "\\endhead \n<br>";
  } else {
    out << "Let " << HtmlRoutines::getMathNoDisplay("p\\subset so(7)") << " be the "
    << g2inB3Data.inducing.toString() << "-parabolic subalgebra"
    << " and let " << HtmlRoutines::getMathNoDisplay("{p}'= p\\cap G_2")
    << ". Then  " << HtmlRoutines::getMathNoDisplay("{p}'") << " is the "
    << g2inB3Data.smallParabolicSelection.toString() << "- parabolic subalgebra of G_2"
    << "<br> <table border =\"1\"><tr><td>$so(7)$-highest weight</td>"
    << "<td>character difference from top</td>"
    << "<td>Decomposition of inducing module over "
    << HtmlRoutines::getMathNoDisplay("p'")
    << "</td><td>" << HtmlRoutines::getMathNoDisplay("p'\\cap b")
    << "-eigenvectors</td><td>Casimir projector</td><td>Extra multiplier</td><td>corresponding "
    << HtmlRoutines::getMathNoDisplay("G_2\\cap b")
    << "-eigenvectors</td><td>Shapovalov square</td></tr>";
    latexTable << "\\begin{longtable}{|cccclll|} \\caption{\\label{tableB3fdsOverG2charsAndHWV"
    << g2inB3Data.inducing.toString() << "} "
    << "Decomposition of inducing $" << g2inB3Data.inducing.toString() << "$-$\\mathfrak p$-module over $"
    << g2inB3Data.smallParabolicSelection.toString() << "$-$\\mathfrak p'=\\mathfrak p\\cap G_2$}\\\\"
    << "\\hline $\\mathfrak p-inducing$& dim. &$\\mathfrak p' decomp. $&dim.&"
    << " $\\mathfrak b \\cap \\mathfrak p'$-singular vectors & Casimir projector "
    << "& Corresp. $\\mathfrak b \\cap G_2$-singular vectors  \\\\ \\hline"
    << "\\endhead \n<br>";
  }
  context.getFormat(g2inB3Data.format);
  g2inB3Data.format.flagUseLatex = true;
  g2inB3Data.format.numberOfAmpersandsPerNewLineForLaTeX = 0;
  Expression tempExpression;
  RationalFraction<Rational> rfZero, rfOne;
  rfZero.makeZero();
  rfOne.makeOne();
  latexTable2 << "\\begin{longtable}{|rll|}\\caption"
  << "{Values of $x_1$ for each $v_{\\lambda,i}$}\\label{tableCriticalValuesvlambda}"
  << "\\endhead";
  for (int i = 0; i < highestWeights.size; i ++) {
    g2inB3Data.weightFundamentalCoordinates = highestWeights[i];
    CalculatorLieTheory::splitFDpartB3overG2inner(calculator, g2inB3Data, tempExpression);
    timeReport << tempExpression.getValue<std::string>();
    RationalFraction<Rational> numEigenVectors;
    numEigenVectors = rfZero;
    for (int j = 0; j < g2inB3Data.smallCharacterFiniteDimensionalPart.size(); j ++) {
      numEigenVectors += g2inB3Data.smallCharacterFiniteDimensionalPart.coefficients[j];
    }
    g2inB3Data.format.customPlusSign = "";
    int eigenIndexcounter = 0;
    if (i != 0) {
      latexTable2 << "\\hline\\multicolumn{3}{|c|}{$\\lambda ="
      << g2inB3Data.weightFundamentalCoordinates.toStringLetterFormat("\\omega", &g2inB3Data.format)
      << "$}\\\\vector& coefficient of $v_\\lambda$ in $Sh_{\\lambda,i}$ &$x_1\\notin$ \\\\\\hline";
    }
    for (int k = 0; k < g2inB3Data.smallCharacterFiniteDimensionalPart.size(); k ++) {
      CharacterSemisimpleLieAlgebraModule<RationalFraction<Rational> > tempChar;
      tempChar.addMonomial(g2inB3Data.smallCharacterFiniteDimensionalPart[k], g2inB3Data.smallCharacterFiniteDimensionalPart.coefficients[k]);
      int multiplicity = 0;
      g2inB3Data.smallCharacterFiniteDimensionalPart.coefficients[k].isSmallInteger(&multiplicity);
      for (int counter = 0; counter < multiplicity; counter ++, eigenIndexcounter ++) {
        out << "<tr>";
        if (k == 0 && counter == 0) {
          g2inB3Data.format.customPlusSign = "\\oplus ";
          g2inB3Data.format.fundamentalWeightLetter = "\\omega";
          out << "<td rowspan =\"" << numEigenVectors.toString() << "\">" << g2inB3Data.ambientCharacter.toString(&g2inB3Data.format) << "</td> ";
          if (!isFD) {
            out << "<td rowspan =\"" << numEigenVectors.toString() << "\">";
            for (int charcounter1 = 0; charcounter1 < g2inB3Data.characterDifferences.size; charcounter1 ++) {
              std::string tempS = g2inB3Data.format.customPlusSign;
              g2inB3Data.format.customPlusSign = "";
              out << "A_{" << charcounter1 << "}=" << g2inB3Data.characterDifferences[charcounter1].toString(&g2inB3Data.format) << ";";
              if (charcounter1 != g2inB3Data.characterDifferences.size - 1) {
                out << "<br>";
              }
              g2inB3Data.format.customPlusSign = tempS;
            }
            out << "</td> ";
          }
          latexTable << "\\multirow{" << g2inB3Data.eigenVectors.size  << "}{*}{$"
          << g2inB3Data.ambientCharacter.toString(&g2inB3Data.format) << "$}";
          Vector<RationalFraction<Rational> > simpleCoordinates;
          simpleCoordinates = g2inB3Data.weylGroupFiniteDimensional.ambientWeyl->getSimpleCoordinatesFromFundamental(
            g2inB3Data.ambientCharacter[0].weightFundamentalCoordinates,
            RationalFraction<Rational>::oneRational()
          );
          RationalFraction<Rational> weylSize;
          weylSize = g2inB3Data.weylGroupFiniteDimensional.weylDimensionFormulaInnerSimpleCoords(simpleCoordinates);
          latexTable << "& \\multirow{" << g2inB3Data.eigenVectors.size  << "}{*}{$"
          << weylSize.toString(&g2inB3Data.format)
          << "$}";
        } else {
          latexTable << "&";
        }
        latexTable << " & ";
        if (counter == 0) {
          g2inB3Data.format.fundamentalWeightLetter = "\\psi";
          g2inB3Data.format.customPlusSign = "\\oplus ";
          out << "<td rowspan =\"" << multiplicity << " \">" << tempChar.toString(&g2inB3Data.format) << "</td>";
          latexTable << "\\multirow{" << multiplicity  << "}{*}{$";
          latexTable << tempChar.toString(&g2inB3Data.format) << "$}";
          latexTable << "&\\multirow{" << multiplicity  << "}{*}{$";
          if (multiplicity > 1) {
            latexTable << multiplicity << "\\times";
          }
          Vector<RationalFraction<Rational> > simpleCoordinates;
          simpleCoordinates = g2inB3Data.weylGroupFiniteDimensionalSmall.ambientWeyl->getSimpleCoordinatesFromFundamental(
            tempChar[0].weightFundamentalCoordinates, RationalFraction<Rational>::zeroRational()
          );
          RationalFraction<Rational> dimension;
          dimension = g2inB3Data.weylGroupFiniteDimensionalSmall.weylDimensionFormulaInnerSimpleCoords(simpleCoordinates, rfOne);
          latexTable << dimension.toString(&g2inB3Data.format) << "$}";
        } else {
          latexTable << "&";
        }
        latexTable << "&";
        g2inB3Data.format.customPlusSign = "";
        out << "<td>" << HtmlRoutines::getMathNoDisplay(g2inB3Data.eigenVectorsLevi[eigenIndexcounter].toString(&g2inB3Data.format)) << "</td>";
        g2inB3Data.format.maximumLineLength = 20;
        latexTable << "$\\begin{array}{l}" << g2inB3Data.eigenVectorsLevi[eigenIndexcounter].toString(&g2inB3Data.format) << "\\end{array}$ \n";
        if (!isFD) {
          std::string tempS1 = g2inB3Data.getStringCasimirProjector(eigenIndexcounter, 12);
          std::string tempS2 = "("+ g2inB3Data.elementsUniversalEnveloping[eigenIndexcounter].toString(&g2inB3Data.format) + ")\\cdot v_\\lambda";
          out << "<td>" << HtmlRoutines::getMathNoDisplay(tempS1) << "</td>";
          out << "<td>" << g2inB3Data.additionalMultipliers[eigenIndexcounter].toString() << "</td>";
          out << "<td>" << HtmlRoutines::getMathNoDisplay(tempS2) << "</td>";
          out << "<td>" << g2inB3Data.shapovalovProducts[eigenIndexcounter].toString(&g2inB3Data.format);
          out << "</td>";
          int index = - 1;
          numEigenVectors.isSmallInteger(&index);
          if (index - eigenIndexcounter - 1 > 0) {
            List<Rational> tempList, tempList2;
            latexTable2 << " $v_{\\lambda," <<  index- eigenIndexcounter - 1 << "} $&";
            Polynomial<Rational> tempP;
            g2inB3Data.shapovalovProducts[eigenIndexcounter].getNumerator(tempP);
            tempP.scaleNormalizeLeadingMonomial(&MonomialPolynomial::orderDefault());
            latexTable2 << "$\\begin{array}{l}" << tempP.toString(&g2inB3Data.format) << "\\end{array}$ & ";
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
      if (k != g2inB3Data.smallCharacterFiniteDimensionalPart.size() - 1) {
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
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::printB3G2branchingTable(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::printB3G2branchingTable");
  Vectors<RationalFraction<Rational> > highestWeights;
  BranchingData g2B3Data;
  ExpressionContext context(calculator);
  if (!CalculatorLieTheory::printB3G2branchingTableCommon(
    calculator, input, output, highestWeights, g2B3Data, context
  )) {
    return false;
  }
  if (output.isError()) {
    return true;
  }
  return CalculatorLieTheory::printB3G2branchingIntermediate(
    calculator, input, output, highestWeights, g2B3Data, context
  );
}

bool CalculatorLieTheory::printB3G2branchingTableCharsOnly(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::printB3G2branchingTableCharsOnly");
  BranchingData g3InB3Data;
  ExpressionContext context(calculator);
  Vectors<RationalFraction<Rational> > theHWs;
  CalculatorLieTheory::printB3G2branchingTableCommon(
    calculator, input, output, theHWs, g3InB3Data, context
  );
  if (output.isError()) {
    return true;
  }
  CharacterSemisimpleLieAlgebraModule<RationalFraction<Rational> > theCharacter, outputChar;
  std::stringstream out;
  std::stringstream latexTable;
  bool isFD = (g3InB3Data.inducing.cardinalitySelection == 0);
  if (isFD) {
    out << "<table>"
    << "<tr><td>$so(7)$-highest weight</td>"
    << "<td>Dimension</td><td>Decomposition over $G_2$</td><td>Dimensions</td>" << "</tr>";
    latexTable << "\\begin{longtable}{|rl|} \\caption{\\label{tableB3fdsOverG2charsonly} "
    << "Decompositions of finite dimensional $so(7)$-modules over $G_2$}\\\\"
    << "\\hline$so(7)$-module & ~~~~~~ decomposition over $G_2$\\endhead \\hline\n<br>";
  } else {
    out << "Let " << HtmlRoutines::getMathNoDisplay("p\\subset so(7)") << " be the "
    << g3InB3Data.inducing.toString() << "-parabolic subalgebra "
    << "and let " << HtmlRoutines::getMathNoDisplay("{p}'= p\\cap G_2")
    << ". Then  " << HtmlRoutines::getMathNoDisplay("{p}'") << " is the "
    << g3InB3Data.smallParabolicSelection.toString() << "- parabolic subalgebra of G_2"
    << "<br> <table><tr><td>$so(7)$-highest weight</td>"
    << "<td>Dimension of inducing fin. dim. "
    << HtmlRoutines::getMathNoDisplay(" p")
    << "-module</td><td>Decomposition of inducing module over "
    << HtmlRoutines::getMathNoDisplay("p'")
    << "</td><td>Dimensions</td>"
    << " <td>Highest weight <br> is sufficiently generic <br> if none of <br>the following vanish</td>"
    << "</tr>";
    latexTable << "\\begin{longtable}{|p{2cm}l|} \\caption{\\label{tableB3fdsOverG2charsonly"
    << g3InB3Data.inducing.toString() << "} "
    << "Decompositions of inducing $\\mathfrak{p}" << "_{"
    << g3InB3Data.inducing.toString()
    << "}" << "$-modules over $\\bar{ \\mathfrak {l}}$"
    << ", where $\\mathfrak{l}$ is the reductive Levi part of "
    << "$\\mathfrak{p}" << "_{" << g3InB3Data.inducing.toString()
    <<  "}$ and $\\bar{\\mathfrak {l}}$ is the reductive Levi part of $\\bar{\\mathfrak {p}} _{"
    << g3InB3Data.smallParabolicSelection.toString()  << "}$" << " } \\\\"
    << "\\hline $V_\\lambda(\\mathfrak l)$ " << "& Decomposition over $\\bar { \\mathfrak l}$ "
    << "\\endhead \\hline\n<br>";
  }
  g3InB3Data.format.flagUseLatex = true;
  ElementUniversalEnveloping<RationalFraction<Rational> > theCasimir, theCentralCharacter, resultChar;
  RationalFraction<Rational> minusOne(Rational(- 1));
  HashedList<ElementUniversalEnveloping<RationalFraction<Rational> > > theCentralChars;
  theCasimir.makeCasimir(g3InB3Data.homomorphism.domainAlgebra());
  WeylGroupData& smallWeyl = g3InB3Data.homomorphism.domainAlgebra().weylGroup;
  for (int k = 0; k < theHWs.size; k ++) {
    theCharacter.makeFromWeight(
      g3InB3Data.homomorphism.coDomainAlgebra().weylGroup.getSimpleCoordinatesFromFundamental(
        theHWs[k], RationalFraction<Rational>::zeroRational()
      ),
      &g3InB3Data.homomorphism.coDomainAlgebra()
    );
    theCharacter.splitCharacterOverReductiveSubalgebra(nullptr, outputChar, g3InB3Data);
    g3InB3Data.format.fundamentalWeightLetter = "\\omega";
    out << "<tr><td> " << theCharacter.toString(&g3InB3Data.format) << "</td> ";
    Vector<RationalFraction<Rational> > simpleCoordinates;
    simpleCoordinates = g3InB3Data.weylGroupFiniteDimensional.ambientWeyl->getSimpleCoordinatesFromFundamental(
      theCharacter[0].weightFundamentalCoordinates,
      RationalFraction<Rational>::zeroRational()
    );
    RationalFraction<Rational> dimension;
    dimension = g3InB3Data.weylGroupFiniteDimensional.weylDimensionFormulaInnerSimpleCoords(simpleCoordinates);
    out << "<td>" << dimension.toString() << "</td>";
    latexTable << " $ " << theCharacter.toString(&g3InB3Data.format) << " $ ";
    g3InB3Data.format.fundamentalWeightLetter = "\\psi";
    out << "<td>" << outputChar.toString(&g3InB3Data.format) << "</td>";
    out << "<td>";
    g3InB3Data.format.customPlusSign = "\\oplus ";
    Vector<RationalFraction<Rational> > leftWeightSimple, leftWeightDual, rightWeightSimple, rightWeightDual;
    theCentralChars.clear();
    for (int i = 0; i < outputChar.size(); i ++) {
      if (!outputChar.coefficients[i].isEqualToOne()) {
        out << outputChar.coefficients[i].toString() << " x ";
      }
      simpleCoordinates = g3InB3Data.weylGroupFiniteDimensionalSmall.ambientWeyl->getSimpleCoordinatesFromFundamental(
        outputChar[i].weightFundamentalCoordinates, RationalFraction<Rational>::zeroRational()
      );
      dimension = g3InB3Data.weylGroupFiniteDimensionalSmall.weylDimensionFormulaInnerSimpleCoords(simpleCoordinates);
      out << dimension;
      if (i != outputChar.size() - 1) {
        out << "+";
      }
      leftWeightSimple = smallWeyl.getSimpleCoordinatesFromFundamental(
        outputChar[i].weightFundamentalCoordinates, RationalFraction<Rational>::zeroRational()
      );
      leftWeightDual = smallWeyl.getDualCoordinatesFromFundamental(outputChar[i].weightFundamentalCoordinates);
      for (int j = 0; j < outputChar.size(); j ++) {
        rightWeightSimple = smallWeyl.getSimpleCoordinatesFromFundamental(
          outputChar[j].weightFundamentalCoordinates, RationalFraction<Rational>::zeroRational()
        );
        rightWeightDual = smallWeyl.getDualCoordinatesFromFundamental(outputChar[j].weightFundamentalCoordinates);
        if ((rightWeightSimple - leftWeightSimple).isPositive()) {
          resultChar = theCasimir;
          theCentralCharacter = theCasimir;
          resultChar.modOutVermaRelations(&rightWeightDual);
          theCentralCharacter.modOutVermaRelations(&leftWeightDual);
          resultChar -= theCentralCharacter;
          resultChar.scaleNormalizeLeadingMonomial(nullptr);
          resultChar *= minusOne;
          theCentralChars.addOnTopNoRepetition(resultChar);
        }
      }
    }
    out << "</td>";
    g3InB3Data.format.maximumLineLength = 80;
    latexTable << "& $\\begin{array}{l} " << outputChar.toString(&g3InB3Data.format) << "\\end{array} $ ";
    g3InB3Data.format.customPlusSign = "";
    if (!isFD && theCentralChars.size > 0) {
      out << "<td>";
      latexTable << " \\\\\n<br> ";
      latexTable << "\\multicolumn{2}{|p{11.6cm}|}{ \\tiny Strong Condition B: ";
      for (int l = 0; l < theCentralChars.size; l ++) {
        out << theCentralChars[l].toString(&g3InB3Data.format) << "<br> ";
        latexTable << "$" << theCentralChars[l].toString(&g3InB3Data.format) << "\\neq 0$";
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
  << "<br>\\begin{document}<br>%text body<br>" << latexTable.str()
  << "<br>%end of text body <br>\\end{document}";
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::printGeneralizedVermaModule(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::printGeneralizedVermaModule");
  Selection selectionParSel;
  Vector<RationalFraction<Rational> > highestWeightFundamentalCoordinates;
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebra;
  if (!calculator.getTypeHighestWeightParabolic(
    calculator,
    input,
    output,
    highestWeightFundamentalCoordinates,
    selectionParSel,
    semisimpleLieAlgebra
  )) {
    return output.assignError(
      calculator,
      "Failed to extract highest weight vector data"
    );
  } else {
    if (output.isError()) {
      return true;
    }
  }
  if (!CalculatorLieTheory::highestWeightVectorCommon(
    calculator,
    output,
    highestWeightFundamentalCoordinates,
    selectionParSel,
    semisimpleLieAlgebra.context,
    semisimpleLieAlgebra.content,
    false
  )) {
    return output.assignError(calculator, "Failed to create Generalized Verma module");
  }
  if (output.isError()) {
    return true;
  }
  ElementTensorsGeneralizedVermas<RationalFraction<Rational> > element;
  element = output.getValue<ElementTensorsGeneralizedVermas<RationalFraction<Rational> > >();
  ModuleSSalgebra<RationalFraction<Rational> >& module = *element[0].monomials[0].owner;
  return output.assignValue(calculator, module.toString());
}

bool CalculatorLieTheory::writeGeneralizedVermaModuleAsDifferentialOperatorUpToLevel(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::writeGeneralizedVermaModuleAsDifferentialOperatorUpToLevel");
  RecursionDepthCounter theRecursionIncrementer(&calculator.recursionDepth);
  if (!input.isListNElements(4)) {
    return output.assignError(
      calculator,
      "Function splitGenericGeneralizedVermaTensorFiniteDimensional is expected "
      "to have three arguments: SS algebra type, Number, List{}. "
    );
  }
  const Expression& leftE = input[1];
  const Expression& genVemaWeightNode = input[3];
  const Expression& levelNode = input[2];
  Expression resultSSalgebraE;
  resultSSalgebraE = leftE;
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebra;
  if (!CalculatorConversions::convert(
    calculator,
    leftE,
    semisimpleLieAlgebra
  )) {
    return output.assignError(
      calculator,
      "Error extracting Lie algebra."
    );
  }
  int rank = semisimpleLieAlgebra.content->getRank();
  Vector<Polynomial<Rational> > highestWeightFundCoords;
  ExpressionContext hwContext(calculator);
  if (!calculator.getVector(
    genVemaWeightNode,
    highestWeightFundCoords,
    &hwContext,
    rank
  )) {
    return calculator
    << "Failed to convert the third argument of "
    << "splitGenericGeneralizedVermaTensorFiniteDimensional to a list of " << rank
    << " polynomials. The second argument you gave is "
    << genVemaWeightNode.toString() << ".";
  }
  int desiredHeight;
  if (!levelNode.isSmallInteger(&desiredHeight)) {
    return output.assignError(
      calculator,
      "second argument of " + input.toString() + " must be a small integer"
    );
  }
  RationalFraction<Rational> RFOne, RFZero;
  RFOne.makeOne();
  RFZero.makeZero();
  Selection selInducing;
  selInducing.makeFullSelection(rank);
  int coefficient;
  for (int i = 0; i < rank; i ++) {
    if (highestWeightFundCoords[i].isSmallInteger(&coefficient)) {
      if (coefficient == 0) {
        selInducing.removeSelection(i);
      }
    }
  }
  Vectors<Polynomial<Rational> > theHws;
  Selection invertedSelInducing = selInducing;
  invertedSelInducing.invertSelection();
  theHws.setSize(0);
  SelectionWithMaxMultiplicity theHWenumerator;
  Vector<Polynomial<Rational> > theHWrf;
  for (int j = 0; j <= desiredHeight; j ++) {
    theHWenumerator.initMaxMultiplicity(rank - selInducing.cardinalitySelection, j);
    theHWenumerator.incrementSubsetFixedCardinality(j);
    LargeInteger numCycles = theHWenumerator.numberOfCombinationsOfCardinality(j);
    for (int i = 0; i < numCycles; i ++, theHWenumerator.incrementSubsetFixedCardinality(j)) {
      theHWrf = highestWeightFundCoords;
      for (int k = 0; k < invertedSelInducing.cardinalitySelection; k ++) {
        theHWrf[invertedSelInducing.elements[k]] += Rational(theHWenumerator.multiplicities[k]);
      }
      theHws.addOnTop(theHWrf);
    }
  }
  FormatExpressions format;
  hwContext.getFormat(format);
  return CalculatorLieTheory::writeGenVermaModAsDiffOperatorInner(
    calculator,
    input,
    output,
    theHws,
    hwContext,
    selInducing,
    semisimpleLieAlgebra.content,
    false,
    nullptr,
    nullptr,
    nullptr,
    true,
    true
  );
}

bool CalculatorLieTheory::killingForm(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::killingForm");
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression leftE = input[1];
  Expression rightE = input[2];
  if (!leftE.isBuiltInType() || !rightE.isBuiltInType()) {
    return false;
  }
  if (!Expression::mergeContexts(leftE, rightE)) {
    return false;
  }
  ExpressionContext context = leftE.getContext();
  ElementUniversalEnveloping<RationalFraction<Rational> > left, right;
  if (
    !leftE.isOfType<ElementUniversalEnveloping<RationalFraction<Rational> > >(&left) ||
    !rightE.isOfType<ElementUniversalEnveloping<RationalFraction<Rational> > >(&right)
  ) {
    return false;
  }
  if (left.isEqualToZero() || right.isEqualToZero()) {
    return output.assignValue(calculator, 0);
  }
  if (&left.getOwner() != &right.getOwner()) {
    return false;
  }
  ElementSemisimpleLieAlgebra<Rational> leftElement, rightElement;
  if (left.isLieAlgebraElementRational(leftElement) && right.isLieAlgebraElementRational(rightElement)) {
    return output.assignValue(calculator, leftElement.getOwner()->getKillingForm(leftElement, rightElement));
  }
  return output.assignValueWithContext(calculator, left.getKillingFormProduct(right), context);
}

bool CalculatorLieTheory::highestWeightVector(Calculator& calculator, const Expression& input, Expression& output) {
  Selection parabolicSelection;
  Vector<RationalFraction<Rational> > highestWeightFundamentalCoordinates;
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebra;
  if (!calculator.getTypeHighestWeightParabolic(
    calculator,
    input,
    output,
    highestWeightFundamentalCoordinates,
    parabolicSelection,
    semisimpleLieAlgebra
  )) {
    return output.assignError(
      calculator,
      "Failed to extract highest weight vector data"
    );
  } else {
    if (output.isError()) {
      return true;
    }
  }
  return CalculatorLieTheory::highestWeightVectorCommon(
    calculator,
    output,
    highestWeightFundamentalCoordinates,
    parabolicSelection,
    semisimpleLieAlgebra.context,
    semisimpleLieAlgebra.content
  );
}

bool CalculatorLieTheory::splitGenericGeneralizedVermaTensorFiniteDimensional(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::splitGenericGeneralizedVermaTensorFiniteDimensional");
  RecursionDepthCounter theRecursionIncrementer(&calculator.recursionDepth);
  if (!input.isListNElements(4))
    return output.assignError(
      calculator,
      "Function splitGenericGeneralizedVermaTensorFiniteDimensional is expected to "
      "have three arguments: SS algebra type, weight, weight. "
    );
  const Expression& leftE = input[1];
  const Expression& genVemaWeightNode = input[3];
  const Expression& fdWeightNode = input[2];
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebra;
  if (!CalculatorConversions::convert(
    calculator,
    leftE,
    semisimpleLieAlgebra
  )) {
    return output.assignError(
      calculator,
      "Error extracting Lie algebra."
    );
  }
  int rank = semisimpleLieAlgebra.content->getRank();
  Vector<RationalFraction<Rational> > highestWeightFundCoords;
  ExpressionContext hwContext(calculator);
  if (!calculator.getVector<RationalFraction<Rational> >(
    genVemaWeightNode,
    highestWeightFundCoords,
    &hwContext,
    rank
    // CalculatorConversions::functionRationalFunction<Rational>
  )) {
    return calculator
    << "Failed to convert the third argument of "
    << "splitGenericGeneralizedVermaTensorFiniteDimensional to a list of "
    << rank
    << " polynomials. The second argument you gave is "
    << genVemaWeightNode.toString() << ".";
  }
  Vector<Rational> highestWeightFiniteDimensional;
  if (!calculator.getVector<Rational>(
    fdWeightNode, highestWeightFiniteDimensional, nullptr, rank
  )) {
    return calculator
    << "Failed to convert the second argument of "
    << "splitGenericGeneralizedVermaTensorFiniteDimensional to a list of "
    << rank
    << " rationals. The second argument you gave is "
    << fdWeightNode.toString() << ".";
  }
  int numberOfVariables = hwContext.numberOfVariables();
  RationalFraction<Rational> RFOne, RFZero;
  RFOne.makeOne();
  RFZero.makeZero();
  ElementTensorsGeneralizedVermas<RationalFraction<Rational> > element;
  Selection parabolicSelection1, finiteDimensionalSelection;
  Expression highestWeightVectorFiniteDimensionalExpression, highestWeightVectorGeneralizedVerma;
  parabolicSelection1.makeFullSelection(rank);
  finiteDimensionalSelection.initialize(rank);
  int coefficient;
  for (int i = 0; i < rank; i ++) {
    if (highestWeightFundCoords[i].isSmallInteger(&coefficient)) {
      if (coefficient >= 0) {
        parabolicSelection1.removeSelection(i);
      }
    }
    bool isGood = false;
    if (highestWeightFiniteDimensional[i].isSmallInteger(&coefficient)) {
      if (coefficient >= 0) {
        isGood = true;
      }
    }
    if (!isGood) {
      return output.assignError(
        calculator,
        "Error: the third argument of "
        "splitGenericGeneralizedVermaTensorFiniteDimensional "
        "must be a list of small non-negative integers."
      );
    }
  }
  semisimpleLieAlgebra.content->flagHasNilradicalOrder = true;
  if (!CalculatorLieTheory::highestWeightVectorCommon(
    calculator, highestWeightVectorGeneralizedVerma, highestWeightFundCoords, parabolicSelection1, hwContext, semisimpleLieAlgebra.content
  )) {
    return false;
  }
  if (highestWeightVectorGeneralizedVerma.isError()) {
    output = highestWeightVectorGeneralizedVerma;
    return true;
  }
  Vector<RationalFraction<Rational> > highestWeightFiniteDimensionalRationalFunction;
  highestWeightFiniteDimensionalRationalFunction = highestWeightFiniteDimensional;
  if (!CalculatorLieTheory::highestWeightVectorCommon(
    calculator,
    highestWeightVectorFiniteDimensionalExpression,
    highestWeightFiniteDimensionalRationalFunction,
    finiteDimensionalSelection,
    hwContext,
    semisimpleLieAlgebra.content
  )) {
    return false;
  }
  if (highestWeightVectorFiniteDimensionalExpression.isError()) {
    output = highestWeightVectorFiniteDimensionalExpression;
    return true;
  }
  std::stringstream out;
  out << "hwv par sel: " << highestWeightVectorGeneralizedVerma.toString() << "hwv fd: " << highestWeightVectorFiniteDimensionalExpression.toString();
  const ElementTensorsGeneralizedVermas<RationalFraction<Rational> >& theHWgenVerma =
  highestWeightVectorGeneralizedVerma.getValue<ElementTensorsGeneralizedVermas<RationalFraction<Rational> > >();
  const ElementTensorsGeneralizedVermas<RationalFraction<Rational> >& theHWfd =
  highestWeightVectorFiniteDimensionalExpression.getValue<ElementTensorsGeneralizedVermas<RationalFraction<Rational> > >();

  ModuleSSalgebra<RationalFraction<Rational> >& generalizedModule = theHWgenVerma[0].monomials[0].getOwner();
  ModuleSSalgebra<RationalFraction<Rational> >& finiteDimensionalModule = theHWfd[0].monomials[0].getOwner();
  if (
    generalizedModule.owner != finiteDimensionalModule.owner ||
    generalizedModule.getOwner().getRank() != generalizedModule.parabolicSelectionNonSelectedAreElementsLevi.numberOfElements ||
    finiteDimensionalModule.getOwner().getRank() != finiteDimensionalModule.parabolicSelectionNonSelectedAreElementsLevi.numberOfElements
  ) {
    global.fatal << "The two modules have owners, "
    << finiteDimensionalModule.getOwner().weylGroup.dynkinType.toString()
    << " and " << generalizedModule.getOwner().weylGroup.dynkinType.toString() << ", and parabolic selections of max size "
    << generalizedModule.parabolicSelectionNonSelectedAreElementsLevi.numberOfElements
    << " and " << finiteDimensionalModule.parabolicSelectionNonSelectedAreElementsLevi.numberOfElements << global.fatal;
  }
  ElementUniversalEnveloping<RationalFraction<Rational> > casimir;
  ElementUniversalEnveloping<RationalFraction<Rational> > casimirMinusChar;
  CharacterSemisimpleLieAlgebraModule<RationalFraction<Rational> > highestWeightCharacter;
  CharacterSemisimpleLieAlgebraModule<RationalFraction<Rational> > finiteDimensionalLeviSplit;
  CharacterSemisimpleLieAlgebraModule<RationalFraction<Rational> > finiteDimensionalCharacter;
  CharacterSemisimpleLieAlgebraModule<RationalFraction<Rational> > finiteDimensionalLeviSplitShifted;
  highestWeightCharacter.makeFromWeight(finiteDimensionalModule.highestWeightSimpleCoordinatesBaseField, semisimpleLieAlgebra.content);
  List<ElementUniversalEnveloping<RationalFraction<Rational> > > theLeviEigenVectors;
  Vectors<RationalFraction<Rational> > eigenVectorWeightsFund;
  if (generalizedModule.parabolicSelectionNonSelectedAreElementsLevi.numberOfElements != generalizedModule.getOwner().getRank()) {
    global.fatal << "Module has parabolic selection with max size "
    << generalizedModule.parabolicSelectionNonSelectedAreElementsLevi.numberOfElements
    << " but the ambient semisimple Lie algebra is of rank "
    << generalizedModule.getOwner().getRank() << ". " << global.fatal;
  }
  std::string report;
  finiteDimensionalModule.splitOverLevi(
    &report,
    generalizedModule.parabolicSelectionNonSelectedAreElementsLevi,
    &theLeviEigenVectors,
    &eigenVectorWeightsFund,
    nullptr,
    &finiteDimensionalLeviSplit
  );
  finiteDimensionalModule.getFDchar(finiteDimensionalCharacter);
  List<ElementUniversalEnveloping<RationalFraction<Rational> > > theCentralCharacters;
  RationalFraction<Rational> zero(Rational::zero());
  casimir.makeCasimir(*semisimpleLieAlgebra.content);
  Vector<RationalFraction<Rational> > currentHWsimplecoords, currentHWdualcoords;
  FormatExpressions tempFormat;
  tempFormat.maximumLineLength = 60;
  tempFormat.flagUseLatex = true;
  tempFormat.fundamentalWeightLetter = "\\psi";
  tempFormat.customPlusSign = "\\oplus ";
  hwContext.getFormat(tempFormat);
  out << "<br>Character of finite dimensional module:" << HtmlRoutines::getMathNoDisplay(finiteDimensionalCharacter.toString());
  if (generalizedModule.parabolicSelectionSelectedAreElementsLevi.cardinalitySelection > 0) {
    out << "<br>The finite dimensional character "
    << "split over the levi component:"
    << HtmlRoutines::getMathNoDisplay(finiteDimensionalLeviSplit.toString(&tempFormat));
  }
  std::stringstream latexReport1;
  out << "<br><table><tr><td>weight in fundamental coords</td><td>Character</td></tr>";
  latexReport1 << " \\begin{longtable}{rl}\\caption{\\label{table"
  << generalizedModule.parabolicSelectionNonSelectedAreElementsLevi.toString()
  << "GenVermatimes7DimCentralCharacters} $" << generalizedModule.parabolicSelectionNonSelectedAreElementsLevi.toString()
  << "$- parabolic $\\bar{\\mathfrak{p}}$} \\\\ $\\mu+\\gamma$ & Action of $\\bar c$\\\\\\hline";
  tempFormat.customPlusSign = "";
  tempFormat.chevalleyGGeneratorLetter = "\\bar{g}";
  tempFormat.chevalleyHGeneratorLetter = "\\bar{h}";
  finiteDimensionalLeviSplitShifted.makeZero();
  Weight<RationalFraction<Rational> > tempMon;
  tempMon.owner = semisimpleLieAlgebra.content;
  ElementUniversalEnveloping<RationalFraction<Rational> > currentChar;
  for (int i = 0; i < theLeviEigenVectors.size; i ++) {
    tempMon.weightFundamentalCoordinates = eigenVectorWeightsFund[i];
    tempMon.weightFundamentalCoordinates += generalizedModule.highestWeightFundamentalCoordinatesBaseField;
    finiteDimensionalLeviSplitShifted.addMonomial(tempMon, RFOne);
    currentHWdualcoords = semisimpleLieAlgebra.content->weylGroup.getDualCoordinatesFromFundamental(tempMon.weightFundamentalCoordinates);
    currentChar = casimir;
    currentChar.modOutVermaRelations(& currentHWdualcoords, RFOne, RFZero);
    theCentralCharacters.addOnTop(currentChar);
    out << "<tr><td>" << finiteDimensionalLeviSplitShifted[i].weightFundamentalCoordinates.toStringLetterFormat("\\psi") << "</td><td>"
    << currentChar.toString(&tempFormat) << "</td></tr>";
    latexReport1 << " $" << finiteDimensionalLeviSplitShifted[i].weightFundamentalCoordinates.toStringLetterFormat("\\psi", &tempFormat) << "$"
    << "&$p_{" << i + 1 << "}=$ $" << currentChar.toString(&tempFormat) << "$\\\\<br>";
  }
  out << "</table>";
  latexReport1 << "\\end{longtable}<br>";
  ElementTensorsGeneralizedVermas<RationalFraction<Rational> > elementGeneralizedVerma, element2;
  finiteDimensionalModule.highestWeightVectorNotation = "v";
  generalizedModule.highestWeightVectorNotation = "w";
  out << "Let w be the highest weight vector of the generalized Verma component, and let v be the highest weight vector of the finite dimensional component";
  out << "<br><table><tr><td>weight in fundamental coords</td><td>Algebraic expression</td><td>Additional multiplier</td>";
  if (numberOfVariables == 1) {
    out << "<td>gcd polynomial coeffs</td>";
  }
  out << "<td>the hwv</td>";
  if (numberOfVariables == 1) {
    out << "<td>gcd divided out</td>";
  }
  out << "</tr>";
  std::stringstream latexReport2;
  latexReport2 << "\\begin{longtable}{p{2.5cm}p{2.5cm}p{1.5cm}l}\\caption{\\label{tableDecompo"
  << generalizedModule.parabolicSelectionNonSelectedAreElementsLevi.toString() << "times7dim}Decomposition for the $"
  << generalizedModule.parabolicSelectionNonSelectedAreElementsLevi.toString()
  << "$-parabolic subalgebra $\\bar{\\mathfrak{p}}$ } \\\\ Weight & Projector applied to &"
  << " Extra multiplier & Resulting $\\bar {\\mathfrak b}$-singular vector \\endhead\\hline";
  for (int i = 0; i < theCentralCharacters.size; i ++) {
    Vector<RationalFraction<Rational> > currentWeightSimpleCoords =
    semisimpleLieAlgebra.content->weylGroup.getSimpleCoordinatesFromFundamental(
      eigenVectorWeightsFund[i], RationalFraction<Rational>::zeroRational()
    );
    element.makeHighestWeightVector(finiteDimensionalModule, RFOne);
    element.multiplyOnTheLeft(theLeviEigenVectors[i], element, *semisimpleLieAlgebra.content, RFOne);
    element.makeHighestWeightVector(generalizedModule, RFOne);
    element.tensorOnTheRight(element);
    element *= - 1;
    std::string startingEltString = element.toString(&tempFormat);
    std::stringstream tempStream, tempStream2;
    tempStream << "\\begin{array}{l}";
    bool found = false;
    for (int j = 0; j < theCentralCharacters.size; j ++) {
      Vector<RationalFraction<Rational> > otherWeightSimpleCoords =
      semisimpleLieAlgebra.content->weylGroup.getSimpleCoordinatesFromFundamental(
        eigenVectorWeightsFund[j], RationalFraction<Rational>::zeroRational()
      );
      if ((otherWeightSimpleCoords - currentWeightSimpleCoords).isPositive()) {
        casimirMinusChar = casimir;
        casimirMinusChar -= theCentralCharacters[j];
        element.multiplyOnTheLeft(casimirMinusChar, element2, *semisimpleLieAlgebra.content, RFOne);
        element = element2;
        tempStream << "(i(\\bar c)- (" << theCentralCharacters[j].toString() << ") )\\\\";
        tempStream2 << " $(\\bar c-p_" << j + 1 << ") $ ";
        found = true;
      }
    }
    if (found) {
      tempStream << "\\cdot";
      tempStream2 << " $\\cdot$ ";
    }
    tempStream << "(" << startingEltString << ")";
    tempStream << "\\end{array}";
    tempStream2 << " $(" << startingEltString << ")$ ";
    RationalFraction<Rational> scale = element.scaleNormalizeLeadingMonomial(nullptr);
    Rational scaleRational;
    if (!scale.isConstant(&scaleRational)) {
      global.fatal << "Unexpected: scale not rational" << global.fatal;
    }
    currentHWsimplecoords = generalizedModule.highestWeightSimpleCoordinatesBaseField;
    currentHWsimplecoords += finiteDimensionalModule.moduleWeightsSimpleCoordinates[i];
    out << "<tr><td>"
    << semisimpleLieAlgebra.content->weylGroup.getFundamentalCoordinatesFromSimple(currentHWsimplecoords).toStringLetterFormat("\\psi")
    << "</td><td>" << HtmlRoutines::getMathNoDisplay(tempStream.str()) << "</td><td>" << scaleRational.toString() << "</td>";
    latexReport2
    << "$" << semisimpleLieAlgebra.content->weylGroup.getFundamentalCoordinatesFromSimple(currentHWsimplecoords).toStringLetterFormat("\\psi")
    << "$ &  " << tempStream2.str() << " &" << scaleRational.toString();
    Polynomial<Rational> tmpGCD, tmpRF;
    tempFormat.maximumLineLength = 80;
    if (numberOfVariables == 1) {
      scale = element.scaleNormalizeLeadingMonomial(nullptr);
      scale.getNumerator(tmpGCD);
      tmpGCD.scaleNormalizeLeadingMonomial(&MonomialPolynomial::orderDefault());
      out << "<td>" << HtmlRoutines::getMathNoDisplay(tmpGCD.toString(&tempFormat)) << "</td>";
    }
    out << "<td>" << HtmlRoutines::getMathNoDisplay(element.toString(&tempFormat)) << "</td>";
    latexReport2 << "&$\\begin{array}{l}" << element.toString(&tempFormat) << "\\end{array}$\\\\<br>";
    if (numberOfVariables == 1) {
      tmpRF = tmpGCD;
      element /= tmpRF;
      out << "<td>" << HtmlRoutines::getMathNoDisplay("\\begin{array}{l}" + element.toString(&tempFormat) + "\\end{array}") << "</td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  latexReport2 << "\\end{longtable}";
  out << "<br>Ready LaTeX (table 1 and 2): <br><br><br>" << latexReport1.str() << "<br><br><br>" << latexReport2.str() << "<br>";
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::splitFDpartB3overG2(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::splitFDpartB3overG2");
  BranchingData g3InB3Data;
  ExpressionContext context(calculator);
  CalculatorLieTheory::splitFDpartB3overG2Init(calculator, input, output, g3InB3Data, context);
  if (output.isError()) {
    return true;
  }
  Vectors<RationalFraction<Rational> > theHWs;
  theHWs.addOnTop(g3InB3Data.weightFundamentalCoordinates);
  return CalculatorLieTheory::printB3G2branchingIntermediate(calculator, input, output, theHWs, g3InB3Data, context);
}

bool CalculatorLieTheory::printB3G2branchingTableCommon(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  Vectors<RationalFraction<Rational> >& outputHWs,
  BranchingData& g2InB3Data,
  ExpressionContext& context
) {
  MacroRegisterFunctionWithName("Calculator::printB3G2branchingTableCommon");
  Vector<RationalFraction<Rational> > theHWrf;
  SelectionWithMaxMultiplicity theHWenumerator;
  int desiredHeight = 0;
  if (!CalculatorLieTheory::printB3G2branchingTableInit(
    calculator, input, output, g2InB3Data, desiredHeight, context
  )) {
    return false;
  }
  if (output.isError()) {
    return true;
  }
  Selection invertedSelInducing = g2InB3Data.inducing;
  context.getFormat(g2InB3Data.format);
  invertedSelInducing.invertSelection();
  outputHWs.setSize(0);
  for (int j = 0; j <= desiredHeight; j ++) {
    theHWenumerator.initMaxMultiplicity(3 - g2InB3Data.inducing.cardinalitySelection, j);
    theHWenumerator.incrementSubsetFixedCardinality(j);
    LargeInteger numberOfCycles = theHWenumerator.numberOfCombinationsOfCardinality(j);
    for (int i = 0; i < numberOfCycles; i ++, theHWenumerator.incrementSubsetFixedCardinality(j)) {
      theHWrf = g2InB3Data.weightFundamentalCoordinates;
      for (int k = 0; k < invertedSelInducing.cardinalitySelection; k ++) {
        theHWrf[invertedSelInducing.elements[k]] += theHWenumerator.multiplicities[k];
      }
      outputHWs.addOnTop(theHWrf);
    }
  }
  return true;
}

bool CalculatorLieTheory::splitFDpartB3overG2old(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::splitFDpartB3overG2old");
  BranchingData g2B3Data;
  CalculatorLieTheory::splitFDpartB3overG2CharsOutput(calculator, input, output, g2B3Data);
  if (output.isError()) {
    return true;
  }
  std::stringstream out;
  CalculatorLieTheory::splitFDpartB3overG2inner(calculator, g2B3Data, output);
  out << "<br>Highest weight: " << g2B3Data.weightFundamentalCoordinates.toString() << "<br>Parabolic selection: "
  << g2B3Data.inducing.toString() << "<br>common Levi part of G_2 and B_3: "
  << g2B3Data.smallParabolicSelection.toString();
  out
  << "<table border =\"1\"><tr><td>word</td><td>B_3-weight simple coords</td><td>B_3-weight fund. coords </td>"
  << "<td>G_2 simple coordinates</td><td>G2-fund. coords</td><td>G2-dual coordinates</td><td>character</td></tr>";
  std::stringstream readyForLatexConsumptionTable1;

  readyForLatexConsumptionTable1 << "\\hline\\multicolumn{3}{|c|}{Highest weight $ "
  << g2B3Data.weightFundamentalCoordinates.toStringLetterFormat("\\omega")
  << "$}\\\\ weight fund. coord.& singular vector& weight proj. $\\bar h^*$ \\\\\\hline\n<br> ";
  for (int i = 0; i < g2B3Data.outputWeightsSimpleCoords.size; i ++) {
    Vector<RationalFraction<Rational> >& currentWeightSimpleB3coords = g2B3Data.outputWeightsSimpleCoords[i];
    Vector<RationalFraction<Rational> >& currentWeightFundB3coords = g2B3Data.outputWeightsFundCoordS[i];
    Vector<RationalFraction<Rational> >& currentG2Weight = g2B3Data.g2Weights[i];
    Vector<RationalFraction<Rational> >& currentG2DualWeight = g2B3Data.g2DualWeights[i];
    readyForLatexConsumptionTable1 << "$" << currentWeightFundB3coords.toStringLetterFormat("\\omega")
    << " $ & $" << g2B3Data.leviEigenSpace[i].toStringLetterFormat("m")
    << " $ & $ " << currentG2Weight.toStringLetterFormat("\\alpha") << " $ \\\\\n<br>";
    out << "<tr><td>" << g2B3Data.outputEigenWords[i].toString() << "</td><td> "
    << currentWeightSimpleB3coords.toString() << "</td><td> " << currentWeightFundB3coords.toString()
    << "</td><td>" << currentG2Weight.toStringLetterFormat("\\alpha") << "</td><td> "
    << g2B3Data.homomorphism.domainAlgebra().weylGroup.getFundamentalCoordinatesFromSimple(currentG2Weight).toString()
    << "</td><td> " << currentG2DualWeight.toString() << "</td>";
    out << "<td>" << HtmlRoutines::getMathNoDisplay(g2B3Data.allCharacters[i].toString()) << "</td>";
    out << "</tr>";
  }
  readyForLatexConsumptionTable1 << "\\hline \n";
  out << "</table>";
  out << "<br>Ready for LaTeX consumption: ";
  out << "<br><br>" << readyForLatexConsumptionTable1.str() << "<br><br>";
  out << "<table border =\"1\"><tr><td>weight</td><td>the elt closed form</td><td>the elt</td></tr>";
  Vector<RationalFraction<Rational> > weightDifference;
  std::stringstream formulaStream1;
  for (int k = 0; k < g2B3Data.g2Weights.size; k ++) {
    out << "<tr><td>" << g2B3Data.g2Weights[k].toString() << "</td><td>";
    for (int j = 0; j < g2B3Data.g2Weights.size; j ++) {
      weightDifference = g2B3Data.g2Weights[j] - g2B3Data.g2Weights[k];
      if (weightDifference.isPositive()) {
        formulaStream1 << "(12(i(\\bar c) - " << g2B3Data.allCharacters[j].toString() <<  "))";
      }
    }
    formulaStream1 << "v_\\lambda";
    out << HtmlRoutines::getMathNoDisplay(formulaStream1.str())
    << "</td><td>" << HtmlRoutines::getMathNoDisplay(g2B3Data.eigenVectors[k].toString()) << "</td></tr>";
  }
  out << "</table>";
  out << "<br>Time final: " << global.getElapsedSeconds();
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::printB3G2branchingTableInit(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  BranchingData& g2b3Data,
  int& desiredHeight,
  ExpressionContext& outputContext
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::printB3G2branchingTableInit");
  if (input.size() != 3) {
    return output.assignError(
      calculator,
      "I need two arguments: first is height, second is parabolic selection. "
    );
  }
  desiredHeight = 0;
  if (!input[1].isSmallInteger(&desiredHeight)) {
    return output.assignError(
      calculator,
      "the first argument must be a small integer"
    );
  }
  if (desiredHeight < 0) {
    desiredHeight = 0;
  }
  const Expression& weightNode = input[2];
  CalculatorLieTheory::splitFDpartB3overG2Init(calculator, weightNode, output, g2b3Data, outputContext);
  if (output.isError()) {
    return true;
  }
  return false;
}

bool CalculatorLieTheory::splitFDpartB3overG2CharsOutput(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  BranchingData& g2B3Data
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::splitFDpartB3overG2CharsOutput");
  ExpressionContext context(calculator);
  CalculatorLieTheory::splitFDpartB3overG2Init(
    calculator, input, output, g2B3Data, context
  );
  if (output.isError()) {
    return true;
  }
  std::stringstream out;
  out << "<br>Highest weight: " << g2B3Data.weightFundamentalCoordinates.toString() << "<br>Parabolic selection: "
  << g2B3Data.inducing.toString();
  std::string report;
  CharacterSemisimpleLieAlgebraModule<RationalFraction<Rational> > tempChar;
  CharacterSemisimpleLieAlgebraModule<RationalFraction<Rational> > startingChar;
  Vector<RationalFraction<Rational> > simpleWeight;
  simpleWeight = g2B3Data.homomorphism.coDomainAlgebra().weylGroup.getSimpleCoordinatesFromFundamental(
    g2B3Data.weightFundamentalCoordinates,
    RationalFraction<Rational>::zeroRational()
  );
  startingChar.makeFromWeight(simpleWeight, &g2B3Data.homomorphism.coDomainAlgebra());
  startingChar.splitCharacterOverReductiveSubalgebra(&report, tempChar, g2B3Data);
  out << report;
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::splitFDpartB3overG2Init(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  BranchingData& g2b3Data,
  ExpressionContext& outputContext
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::splitFDpartB3overG2Init");
  if (!input.isListNElements(4)) {
    return output.assignError(
      calculator,
      "Splitting the f.d. part of a B_3-representation "
      "over G_2 requires 3 arguments"
    );
  }
  if (!calculator.getVectorFromFunctionArguments<RationalFraction<Rational> >(
    input,
    g2b3Data.weightFundamentalCoordinates,
    &outputContext,
    3//,
    //CalculatorConversions::functionRationalFunction<Rational>
  )) {
    output.assignError(
      calculator,
      "Failed to extract highest weight in fundamental coordinates. "
    );
  }
  calculator.makeHmmG2InB3(g2b3Data.homomorphism);
  g2b3Data.inducing.initialize(3);
  for (int i = 0; i < g2b3Data.weightFundamentalCoordinates.size; i ++) {
    if (!g2b3Data.weightFundamentalCoordinates[i].isSmallInteger()) {
      g2b3Data.inducing.addSelectionAppendNewIndex(i);
    }
  }
  g2b3Data.initializePart1NoSubgroups();
  return true;
}

bool CalculatorLieTheory::splitFDpartB3overG2CharsOnly(
  Calculator& calculator, const Expression& input, Expression& output
) {
  BranchingData g2B3Data;
  return CalculatorLieTheory::splitFDpartB3overG2CharsOutput(calculator, input, output, g2B3Data);
}

bool CalculatorLieTheory::slTwoRealFormStructureForceRecompute(
  Calculator& calculator,
  const Expression& input,
  Expression& output
) {
  return CalculatorLieTheory::slTwoRealFormStructure(calculator, input, output, true);
}

bool CalculatorLieTheory::slTwoRealFormStructureComputeOnDemand(
  Calculator& calculator,
  const Expression& input,
  Expression& output
) {
  return CalculatorLieTheory::slTwoRealFormStructure(calculator, input, output, false);
}

bool CalculatorLieTheory::elementsInSameLieAlgebra(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  SemisimpleLieAlgebra*& outputOwner,
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputElements
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::getElementsInSameLieAlgebra");
  if (input.size() < 3) {
    output.assignError(
      calculator,
      "Function ad common eigenspaces needs at least 2 arguments - "
      "type and at least one element of the algebra."
    );
    return false;
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!CalculatorConversions::convert(
    calculator,
    input[1],
    algebra
  )) {
    output.assignError(
      calculator,
      "Error extracting Lie algebra."
    );
    return false;
  }
  outputOwner = algebra.content;
  outputElements.reserve(input.size() - 2);
  ElementSemisimpleLieAlgebra<AlgebraicNumber> element;
  for (int i = 2; i < input.size(); i ++) {
    if (!CalculatorConversions::loadElementSemisimpleLieAlgebraAlgebraicNumbers(
      calculator, input[i], element, *outputOwner
    )) {
      std::stringstream out;
      out
      << "Failed to extract element of semisimple Lie algebra from: "
      << input[i] << ". ";
      output.assignError(
        calculator,
        out.str()
      );
      return false;
    }
    outputElements.addOnTop(element);
  }
  return true;
}

bool CalculatorLieTheory::adjointCommonEigenSpaces(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::adjointCommonEigenSpaces");
  SemisimpleLieAlgebra* ownerSemisimple = nullptr;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > elements, outputElements;
  if (!CalculatorLieTheory::elementsInSameLieAlgebra(
    calculator, input, output, ownerSemisimple, elements
  )) {
    return true;
  }
  ownerSemisimple->getCommonCentralizer(elements, outputElements);
  std::stringstream out;
  out << "<br>Starting elements: " << elements.toStringCommaDelimited();
  out << "<br>EigenSpace basis (" << outputElements.size << " elements total):<br> (";
  for (int i = 0; i < outputElements.size; i ++) {
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& current = outputElements[i];
    out << current.toString();
    if (i != outputElements.size - 1) {
      out << ", ";
    }
  }
  out << ")";
  output.assignValue(calculator, out.str());
  return true;
}

bool CalculatorLieTheory::standardRepresentationMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::standardRepresentationMatrix");
  if (input.size() != 2) {
    return calculator << "Expected a single argument.";
  }
  ElementUniversalEnveloping<RationalFraction<Rational> > element;
  if (!input[1].isOfType(&element)) {
    return calculator << "Argument expected to be element of universal enveloping.";
  }
  if (element.isEqualToZero()) {
    return false;
  }
  ExpressionContext context = input[1].getContext();
  SemisimpleLieAlgebra& owner = element.getOwner();
  Matrix<RationalFraction<Rational> > result;
  owner.getElementStandardRepresentation(element, result, &calculator.comments);
  Matrix<Expression> resultExpressions;
  resultExpressions.initialize(result.numberOfRows, result.numberOfColumns);
  for (int i = 0; i < resultExpressions.numberOfRows; i ++) {
    for (int j = 0; j < resultExpressions.numberOfColumns; j ++) {
      Rational whichConstant;
      if (result(i, j).isConstant(&whichConstant)) {
        resultExpressions(i, j).assignValue(calculator, whichConstant);
      } else {
        resultExpressions(i, j).assignValueWithContext(calculator, result(i, j), context);
      }
    }
  }
  return output.assignMatrixExpressions(resultExpressions, calculator, false, false);
}

bool CalculatorLieTheory::adjointMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::adjointMatrix");
  if (input.size() != 2) {
    return calculator << "Expected a single argument.";
  }
  ElementUniversalEnveloping<RationalFraction<Rational> > element;
  if (!input[1].isOfType(&element)) {
    return calculator << "<hr>Argument expected to be an element of universal enveloping algebra.";
  }
  ExpressionContext context = input[1].getContext();
  SemisimpleLieAlgebra& owner = element.getOwner();
  Matrix<RationalFraction<Rational> > result;
  owner.getElementAdjointRepresentation(element, result, &calculator.comments);
  Matrix<Expression> resultExpressions;
  resultExpressions.initialize(result.numberOfRows, result.numberOfColumns);
  for (int i = 0; i < resultExpressions.numberOfRows; i ++) {
    for (int j = 0; j < resultExpressions.numberOfColumns; j ++) {
      Rational whichConstant;
      if (result(i, j).isConstant(&whichConstant)) {
        resultExpressions(i, j).assignValue(calculator, whichConstant);
      } else {
        resultExpressions(i, j).assignValueWithContext(calculator, result(i, j), context);
      }
    }
  }
  return output.assignMatrixExpressions(resultExpressions, calculator, false, false);
}

std::string VoganDiagram::toString() {
  std::stringstream out;
  switch (this->diagram) {
  case VoganDiagram::G:
    out << "G";
    break;
  case VoganDiagram::AI:
    out << "AI";
    break;
  case VoganDiagram::AII:
    out << "AII";
    break;
  case VoganDiagram::AIII:
    out << "AIII";
    break;
  case VoganDiagram::BI:
    out << "BI";
    break;
  case VoganDiagram::CI:
    out << "CI";
    break;
  case VoganDiagram::DI:
    out << "DI";
    break;
  case VoganDiagram::DII:
    out << "DII";
    break;
  case VoganDiagram::EI:
    out << "EI";
    break;
  case VoganDiagram::EII:
    out << "EII";
    break;
  case VoganDiagram::EIII:
    out << "EIII";
    break;
  case VoganDiagram::EIV:
    out << "EIV";
    break;
  default:
    out << "[unknown vogan diagram]";
  }
  out << ", " << this->rank << ", " << this->parameter;
  return out.str();
}

DynkinType VoganDiagram::dynkinTypeAmbientNoFailure() {
  DynkinType result;
  std::stringstream failure;
  if (!this->dynkinTypeAmbient(result, &failure)) {
    global.fatal
    << "Failed to get ambient Dynkin type with a no-fail functions: "
    << failure.str() << ". ";
  }
  return result;
}

bool VoganDiagram::dynkinTypeAmbient(
  DynkinType& output,
  std::stringstream* commentsOnFailure
) {
  switch (this->diagram) {
  case VoganDiagram::AI:
  case VoganDiagram::AII:
  case VoganDiagram::AIII:
    output.makeSimpleType('A', this->rank, nullptr);
    return true;
  case VoganDiagram::BI:
    output.makeSimpleType('B', this->rank, nullptr);
    return true;
  case VoganDiagram::CI:
    output.makeSimpleType('C', this->rank, nullptr);
    return true;
  case VoganDiagram::DI:
  case VoganDiagram::DII:
    output.makeSimpleType('D', this->rank, nullptr);
    return true;
  case VoganDiagram::EI:
  case VoganDiagram::EII:
  case VoganDiagram::EIII:
  case VoganDiagram::EIV:
    output.makeSimpleType('E', 6, nullptr);
    return true;
  default:
    break;
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure
    << "Vogan diagram with unknown semisimple Lie algebra type: "
    << this->toString() << ". ";
  }
  return false;
}

void VoganDiagram::computeMapStringToType() {
  if (this->mapStringToType.size() != 0) {
    return;
  }
  this->mapStringToType.setKeyValue("AI", VoganDiagram::AI);
  this->mapStringToType.setKeyValue("AII", VoganDiagram::AII);
  this->mapStringToType.setKeyValue("AIII", VoganDiagram::AIII);
  this->mapStringToType.setKeyValue("BI", VoganDiagram::BI);
  this->mapStringToType.setKeyValue("CI", VoganDiagram::CI);
  this->mapStringToType.setKeyValue("DI", VoganDiagram::DI);
  this->mapStringToType.setKeyValue("DII", VoganDiagram::DII);
  this->mapStringToType.setKeyValue("DIII", VoganDiagram::DIII);
  this->mapStringToType.setKeyValue("EI", VoganDiagram::EI);
  this->mapStringToType.setKeyValue("EII", VoganDiagram::EII);
  this->mapStringToType.setKeyValue("EIII", VoganDiagram::EIII);
  this->mapStringToType.setKeyValue("EIV", VoganDiagram::EIV);
  this->mapStringToType.setKeyValue("EV", VoganDiagram::EV);
  this->mapStringToType.setKeyValue("EVI", VoganDiagram::EVI);
  this->mapStringToType.setKeyValue("EVII", VoganDiagram::EVII);
  this->mapStringToType.setKeyValue("EVIII", VoganDiagram::EVIII);
  this->mapStringToType.setKeyValue("EIX", VoganDiagram::EIX);
  this->mapStringToType.setKeyValue("FI", VoganDiagram::FI);
  this->mapStringToType.setKeyValue("FII", VoganDiagram::FII);
  this->mapStringToType.setKeyValue("G", VoganDiagram::G);
}

bool VoganDiagram::assignParameter(
  const std::string& input,
  int inputRank,
  int inputParameter,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("VoganDiagram::assignParameter");
  this->computeMapStringToType();
  if (!this->mapStringToType.contains(input)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Unknown Vogan diagram type: " << input;
    }
    return false;
  }
  this->diagram = this->mapStringToType.getValueNoFail(input);
  this->parameter = inputParameter;
  this->rank = inputRank;
  if (this->parameter < 0) {
    this->parameter = 0;
  }
  if (this->rank < 0) {
    this->rank = 0;
  }
  if (
    this->diagram == VoganDiagram::DiagramType::AIII &&
    this->parameter > (this->rank - 1) / 2
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Parameter must be at least 1 and at most rank/2="
      << this->rank / 2
      << ", it is instead: " << this->parameter;
    }
    return false;
  }
  // Check that we have not forgotten to implement
  // the ambient Lie algebra computation.
  DynkinType unused;
  if (!this->dynkinTypeAmbient(unused, commentsOnFailure)) {
    return false;
  }
  return true;
}

VoganDiagram::VoganDiagram() {
  this->rank = 0;
  this->parameter = - 1;
}

std::string CartanInvolution::toString() {
  MacroRegisterFunctionWithName("CartanInvolution::toString");
  std::stringstream out;
  out << this->voganDiagram.toString() << ". ";
  out << this->automorphism.toString();
  return out.str();
}

DynkinType CartanInvolution::dynkinTypeAmbientNoFailure(
) {
  std::stringstream failure;
  DynkinType result;
  if (!this->dynkinTypeAmbient(result, &failure)) {
    global.fatal
    << "Failed to extract ambient Dynkin type with a no-fail function: "
    << failure.str() << ". "
    << global.fatal;
  }
  return result;
}

bool CartanInvolution::dynkinTypeAmbient(
  DynkinType& output, std::stringstream* commentsOnFailure
) {
  return this->voganDiagram.dynkinTypeAmbient(
    output, commentsOnFailure
  );
}

CartanInvolution::CartanInvolution() {
  this->owner = nullptr;
}

void CartanInvolution::setSimpleRootSwap(int indexLeft, int indexRight) {
  MacroRegisterFunctionWithName("CartanInvolution::setSimpleRootSwap");
  int rank = this->dynkinTypeAmbientNoFailure().getRank();
  Vector<Rational> simpleLeft, simpleRight;
  simpleLeft.makeEi(rank, indexLeft);
  simpleRight.makeEi(rank, indexRight);
  this->automorphism.imagesPositiveSimpleChevalleyGenerators[indexLeft].makeGGenerator(simpleRight, *this->owner);
  this->automorphism.imagesNegativeSimpleChevalleyGenerators[indexLeft].makeGGenerator(- simpleRight, *this->owner);
  this->automorphism.imagesPositiveSimpleChevalleyGenerators[indexRight].makeGGenerator(simpleLeft, *this->owner);
  this->automorphism.imagesNegativeSimpleChevalleyGenerators[indexRight].makeGGenerator(- simpleLeft, *this->owner);
}

void CartanInvolution::setFilledSimpleRoot(int index) {
  MacroRegisterFunctionWithName("CartanInvolution::setFilledSimpleRoot");
  this->setHollowSimpleRoot(index);
  this->automorphism.imagesPositiveSimpleChevalleyGenerators[index] *= - 1;
  this->automorphism.imagesNegativeSimpleChevalleyGenerators[index] *= - 1;
}

void CartanInvolution::setHollowSimpleRoot(int index) {
  Vector<Rational> simpleRoot;
  simpleRoot.makeEi(this->owner->getRank(), index);
  this->automorphism.imagesPositiveSimpleChevalleyGenerators[index].makeGGenerator(simpleRoot, *this->owner);
  this->automorphism.imagesNegativeSimpleChevalleyGenerators[index].makeGGenerator(- simpleRoot, *this->owner);
}

bool CartanInvolution::computeSimpleRootImagesTypeAI(
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("CartanInvolution::computeSimpleRootImagesTypeAI");
  if (!this->owner->weylGroup.dynkinType.isSimpleOfType('A')) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Type is not simple type A. ";
    }
    return false;
  }
  int rank = this->owner->getRank();
  this->automorphism.imagesPositiveSimpleChevalleyGenerators.setSize(rank);
  this->automorphism.imagesNegativeSimpleChevalleyGenerators.setSize(rank);
  for (int i = 0; i < rank / 2; i ++) {
    this->setSimpleRootSwap(i, rank - i - 1);
  }
  if (rank % 2 == 1) {
    this->setFilledSimpleRoot(rank / 2);
  }
  return true;
}

bool CartanInvolution::computeSimpleRootImagesTypeAII(
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("CartanInvolution::computeSimpleRootImagesTypeAII");
  if (!this->owner->weylGroup.dynkinType.isSimpleOfType('A')) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Type is not simple type A. ";
    }
    return false;
  }
  int rank = this->owner->getRank();
  if (rank % 2 != 1) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Vogan diagram of type AII requires odd rank.";
    }
    return false;
  }
  this->automorphism.imagesPositiveSimpleChevalleyGenerators.setSize(rank);
  this->automorphism.imagesNegativeSimpleChevalleyGenerators.setSize(rank);
  for (int i = 0; i < rank; i ++) {
    this->setSimpleRootSwap(i, rank - i - 1);
  }
  this->setHollowSimpleRoot(rank / 2);
  return true;
}

bool CartanInvolution::computeSimpleRootImagesTypeAIII(
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("CartanInvolution::computeSimpleRootImagesTypeAIII");
  if (!this->owner->weylGroup.dynkinType.isSimpleOfType('A')) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Type is not simple type A. ";
    }
    return false;
  }
  int rank = this->owner->getRank();
  if (this->voganDiagram.parameter < 0 || this->voganDiagram.parameter > rank) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Parameter must be between 0 and the rank "
      << rank << " instead it is: " << this->voganDiagram.rank;
    }
    return false;
  }
  if (this->voganDiagram.parameter >= rank) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Parameter " << this->voganDiagram.parameter + 1
      << " must be less than or equal to: " << rank << ". ";
    }
    return false;
  }
  this->automorphism.imagesPositiveSimpleChevalleyGenerators.setSize(rank);
  this->automorphism.imagesNegativeSimpleChevalleyGenerators.setSize(rank);
  Vector<Rational> simpleRootLeft, simpleRootRight;
  for (int i = 0; i < this->voganDiagram.rank; i ++) {
    if (i == this->voganDiagram.parameter) {
      continue;
    }
    this->setHollowSimpleRoot(i);
  }
  this->setFilledSimpleRoot(this->voganDiagram.parameter);
  return true;
}

bool CartanInvolution::computeSimpleRootImagesTypeBI(
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("CartanInvolution::computeSimpleRootImagesTypeBI");
  if (!this->owner->weylGroup.dynkinType.isSimpleOfType('B')) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Type is not simple type B. ";
    }
    return false;
  }
  int rank = this->owner->getRank();
  if (
    this->voganDiagram.parameter < 0 ||
    this->voganDiagram.parameter >= rank - 1
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Parameter must be between 0 and the rank less one. The rank is: "
      << rank << " the parameter is: " << this->voganDiagram.parameter + 1;
    }
    return false;
  }
  if (this->voganDiagram.parameter >= rank) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Parameter " << this->voganDiagram.parameter + 1
      << " must be less than or equal to: " << rank << ". ";
    }
    return false;
  }
  this->automorphism.imagesPositiveSimpleChevalleyGenerators.setSize(rank);
  this->automorphism.imagesNegativeSimpleChevalleyGenerators.setSize(rank);
  Vector<Rational> simpleRootLeft;
  Vector<Rational> simpleRootRight;
  for (int i = 0; i < this->voganDiagram.rank; i ++) {
    if (i == this->voganDiagram.parameter) {
      continue;
    }
    this->setHollowSimpleRoot(i);
  }
  this->setFilledSimpleRoot(this->voganDiagram.parameter);
  return true;
}

bool CartanInvolution::computeSimpleRootImagesTypeCI(
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("CartanInvolution::computeSimpleRootImagesTypeCI");
  if (!this->owner->weylGroup.dynkinType.isSimpleOfType('C')) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Type is not simple type C but of type: "
      << this->owner->weylGroup.dynkinType.toString();
    }
    return false;
  }
  int rank = this->owner->getRank();
  if (
    this->voganDiagram.parameter < 0 ||
    this->voganDiagram.parameter >= rank
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Parameter must be between 0 and the rank less one. The rank is: "
      << rank << " the parameter is: " << this->voganDiagram.parameter + 1;
    }
    return false;
  }
  if (this->voganDiagram.parameter >= rank) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Parameter " << this->voganDiagram.parameter + 1
      << " must be less than or equal to: " << rank << ". ";
    }
    return false;
  }
  this->automorphism.imagesPositiveSimpleChevalleyGenerators.setSize(rank);
  this->automorphism.imagesNegativeSimpleChevalleyGenerators.setSize(rank);
  Vector<Rational> simpleRootLeft;
  Vector<Rational> simpleRootRight;
  for (int i = 0; i < this->voganDiagram.rank; i ++) {
    if (i == this->voganDiagram.parameter) {
      continue;
    }
    this->setHollowSimpleRoot(i);
  }
  this->setFilledSimpleRoot(this->voganDiagram.parameter);
  return true;
}

bool CartanInvolution::computeSimpleRootImagesTypeDI(
  std::stringstream* commentsOnFailure
) {
  return this->computeSimpleRootImagesTypeD(commentsOnFailure, true);
}

bool CartanInvolution::computeSimpleRootImagesTypeDII(
  std::stringstream* commentsOnFailure
) {
  return this->computeSimpleRootImagesTypeD(commentsOnFailure, false);
}

bool CartanInvolution::computeSimpleRootImagesTypeD(
  std::stringstream* commentsOnFailure,
  bool useAutomorphism
) {
  MacroRegisterFunctionWithName("CartanInvolution::computeSimpleRootImagesTypeD");
  if (!this->owner->weylGroup.dynkinType.isSimpleOfType('D')) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Type is not simple type D but of type: "
      << this->owner->weylGroup.dynkinType.toString() << ". ";
    }
    return false;
  }
  int rank = this->owner->getRank();
  if (
    this->voganDiagram.parameter < 0 ||
    this->voganDiagram.parameter >= rank - 1
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Parameter must be between 0 and the rank less one. The rank is: "
      << rank << " the parameter is: " << this->voganDiagram.parameter + 1;
    }
    return false;
  }
  if (this->voganDiagram.parameter >= rank) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Parameter " << this->voganDiagram.parameter + 1
      << " must be less than or equal to: " << rank << ". ";
    }
    return false;
  }
  this->automorphism.imagesPositiveSimpleChevalleyGenerators.setSize(rank);
  this->automorphism.imagesNegativeSimpleChevalleyGenerators.setSize(rank);
  Vector<Rational> simpleRootLeft;
  Vector<Rational> simpleRootRight;

  for (int i = 0; i < this->voganDiagram.rank - 2; i ++) {
    if (i == this->voganDiagram.parameter) {
      continue;
    }
    this->setHollowSimpleRoot(i);
  }
  if (useAutomorphism) {
    this->setSimpleRootSwap(
      this->voganDiagram.rank - 2,
      this->voganDiagram.rank - 1
    );
  }
  if (this->voganDiagram.parameter >= 0) {
    this->setFilledSimpleRoot(this->voganDiagram.parameter);
  }
  return true;
}

bool CartanInvolution::computeSimpleRootImagesTypeEI(
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("CartanInvolution::computeSimpleRootImagesTypeEI");
  // Vogan diagram of type EI is for Lie algebra E_6.
  if (
    !this->owner->weylGroup.dynkinType.isSimpleOfType('E') ||
    this->owner->getRank() != 6
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Type is not E_6. ";
    }
    return false;
  }
  int rank = 6;
  this->automorphism.imagesPositiveSimpleChevalleyGenerators.setSize(rank);
  this->automorphism.imagesNegativeSimpleChevalleyGenerators.setSize(rank);
  // The sticky part of Dynkin diagram.
  this->setFilledSimpleRoot(1);
  // The middle of the 5-node string of the Dynkin diagram
  this->setHollowSimpleRoot(3);
  this->setSimpleRootSwap(0, 5);
  this->setSimpleRootSwap(2, 4);
  return true;
}

bool CartanInvolution::computeSimpleRootImagesTypeEII(
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("CartanInvolution::computeSimpleRootImagesTypeEII");
  // Vogan diagram of type EII is for Lie algebra E_6.
  if (
    !this->owner->weylGroup.dynkinType.isSimpleOfType('E') ||
    this->owner->getRank() != 6
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Type is not E_6. ";
    }
    return false;
  }
  int rank = 6;
  this->automorphism.imagesPositiveSimpleChevalleyGenerators.setSize(rank);
  this->automorphism.imagesNegativeSimpleChevalleyGenerators.setSize(rank);
  this->setHollowSimpleRoot(0);
  // The sticky part of Dynkin diagram.
  this->setFilledSimpleRoot(1);
  this->setHollowSimpleRoot(2);
  this->setHollowSimpleRoot(3);
  this->setHollowSimpleRoot(4);
  this->setHollowSimpleRoot(5);
  return true;
}

bool CartanInvolution::computeSimpleRootImagesTypeEIII(
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("CartanInvolution::computeSimpleRootImagesTypeEIII");
  // Vogan diagram of type EIII is for Lie algebra E_6.
  if (
    !this->owner->weylGroup.dynkinType.isSimpleOfType('E') ||
    this->owner->getRank() != 6
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Type is not E_6. ";
    }
    return false;
  }
  int rank = 6;
  this->automorphism.imagesPositiveSimpleChevalleyGenerators.setSize(rank);
  this->automorphism.imagesNegativeSimpleChevalleyGenerators.setSize(rank);
  // The sticky part of Dynkin diagram.
  this->setFilledSimpleRoot(0);
  this->setHollowSimpleRoot(1);
  this->setHollowSimpleRoot(2);
  this->setHollowSimpleRoot(3);
  this->setHollowSimpleRoot(4);
  this->setHollowSimpleRoot(5);
  return true;
}

bool CartanInvolution::computeSimpleRootImagesTypeEIV(
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("CartanInvolution::computeSimpleRootImagesTypeEIV");
  // Vogan diagram of type EIV is for Lie algebra E_6.
  if (
    !this->owner->weylGroup.dynkinType.isSimpleOfType('E') ||
    this->owner->getRank() != 6
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Type is not E_6. ";
    }
    return false;
  }
  int rank = 6;
  this->automorphism.imagesPositiveSimpleChevalleyGenerators.setSize(rank);
  this->automorphism.imagesNegativeSimpleChevalleyGenerators.setSize(rank);
  // The sticky part of Dynkin diagram.
  this->setHollowSimpleRoot(1);
  // The middle of the 5-node string of the Dynkin diagram
  this->setHollowSimpleRoot(3);
  this->setSimpleRootSwap(0, 5);
  this->setSimpleRootSwap(2, 4);
  return true;
}

bool CartanInvolution::computeSimpleRootImages(
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("CartanInvolution::computeSimpleRootImages");
  this->automorphism.domain = this->owner;
  this->automorphism.coDomain = this->owner;
  this->owner->weylGroup.computeRho(false);
  switch (this->voganDiagram.diagram) {
  case VoganDiagram::DiagramType::AI:
    return this->computeSimpleRootImagesTypeAI(commentsOnFailure);
  case VoganDiagram::DiagramType::AII:
    return this->computeSimpleRootImagesTypeAII(commentsOnFailure);
  case VoganDiagram::DiagramType::AIII:
    return this->computeSimpleRootImagesTypeAIII(commentsOnFailure);
  case VoganDiagram::DiagramType::BI:
    return this->computeSimpleRootImagesTypeBI(commentsOnFailure);
  case VoganDiagram::DiagramType::CI:
    return this->computeSimpleRootImagesTypeCI(commentsOnFailure);
  case VoganDiagram::DiagramType::DI:
    return this->computeSimpleRootImagesTypeDI(commentsOnFailure);
  case VoganDiagram::DiagramType::EI:
    return this->computeSimpleRootImagesTypeEI(commentsOnFailure);
  case VoganDiagram::DiagramType::EII:
    return this->computeSimpleRootImagesTypeEII(commentsOnFailure);
  case VoganDiagram::DiagramType::EIII:
    return this->computeSimpleRootImagesTypeEIII(commentsOnFailure);
  case VoganDiagram::DiagramType::EIV:
    return this->computeSimpleRootImagesTypeEIV(commentsOnFailure);
  default:
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Not implemented: Vogan diagram: "
      << this->voganDiagram.toString() << ". ";
    }
    return false;
  }
}

bool CartanInvolution::computeFromDiagram(
  const VoganDiagram& inputDiagram,
  SemisimpleLieAlgebra& inputOwner,
  std::stringstream* commentsOnFailure
) {
  this->owner = &inputOwner;
  this->voganDiagram = inputDiagram;
  if (!this->computeSimpleRootImages(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to compute simple root images. ";
    }
    return false;
  }
  if (!this->automorphism.computeHomomorphismFromImagesSimpleChevalleyGenerators(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to extend images of simple generators to a Cartan involution. ";
    }
    return false;
  }
  return true;
}

void DynkinType::plot(Plot& output) {
  DynkinType::plotInitialize(output);
  int verticalOffset = 0;
  for (int i = 0; i < this->monomials.size; i ++) {
    for (int j = 0; j < this->coefficients[i]; j ++) {
      this->monomials[i].plot(output, verticalOffset);
      verticalOffset += - 2 * DynkinSimpleType::distanceBetweenRootCenters;
    }
  }
}

void DynkinType::plotInitialize(Plot& output) {
  output.dimension = 2;
  output.flagIncludeCoordinateSystem = false;
  output.desiredHtmlWidthInPixels = 200;
  output.desiredHtmlHeightInPixels = 75;
}

void DynkinSimpleType::plotOneRoot(
  Plot& output, int index, bool filled, int verticalOffset
) {
  Vector<Rational> center;
  center.makeZero(2);
  center[1] = verticalOffset;
  center[0] = index * DynkinSimpleType::distanceBetweenRootCenters;
  output.drawCircle(
    center,
    DynkinSimpleType::radiusOfRootCircle,
    "black",
    filled
  );
}

void DynkinSimpleType::plotHorizontalChainOfRoots(
  Plot& output, int count, Selection* filledNodes
) {
  DynkinSimpleType::plotHorizontalChainOfRoots(
    output, count,  0, filledNodes, nullptr
  );
}

void DynkinSimpleType::plotHorizontalChainOfRoots(
  Plot& output,
  int count,
  int verticalOffset,
  Selection* filledNodes,
  List<std::string>* labels
) {
  for (int i = 0; i < count; i ++) {
    bool selected = false;
    if (filledNodes != nullptr) {
      selected = filledNodes->selected[i];
    }
    DynkinSimpleType::plotOneRoot(output, i, selected, verticalOffset);
  }
  Vector<Rational> center;
  center.makeZero(2);
  center[1] = verticalOffset;
  if (labels != nullptr) {
    center[1] += DynkinSimpleType::labelDistance;
    for (int i = 0; i < labels->size; i ++) {
      center[0] = i * DynkinSimpleType::distanceBetweenRootCenters;
      output.drawLabel(center, (*labels)[i]);
    }
  }
  Vector<Rational> left, right;
  left.makeZero(2);
  left[1] = verticalOffset;
  right = left;
  for (int i = 0; i < count - 1; i ++) {
    left[0] = i * DynkinSimpleType::distanceBetweenRootCenters + DynkinSimpleType::radiusOfRootCircle;
    right[0] = (i + 1) * DynkinSimpleType::distanceBetweenRootCenters - DynkinSimpleType::radiusOfRootCircle;
    output.drawSegment(left, right);
  }
}

void DynkinSimpleType::plot(Plot& output, int verticalOffset) const {
  switch (this->letter) {
  case 'A':
    DynkinSimpleType::plotAn(output, this->rank, nullptr, verticalOffset);
    break;
  case 'B':
    DynkinSimpleType::plotBn(output, this->rank, nullptr, verticalOffset);
    break;
  case 'C':
    DynkinSimpleType::plotCn(output, this->rank, nullptr, verticalOffset);
    break;
  case 'D':
    DynkinSimpleType::plotDn(output, this->rank, nullptr, verticalOffset);
    break;
  case 'E':
    switch (this->rank) {
    case 6:
      DynkinSimpleType::plotE6(output, verticalOffset);
      break;
    case 7:
      DynkinSimpleType::plotE7(output, verticalOffset);
      break;
    case 8:
      DynkinSimpleType::plotE8(output, verticalOffset);
      break;
    default:
      global.comments << "Plotting of type: " << this->toString() << " not implemented. ";
    }
    break;
  case 'F':
    DynkinSimpleType::plotF4(output, verticalOffset);
    break;
  case 'G':
    DynkinSimpleType::plotG2(output, verticalOffset);
    break;
  default:
    global.comments << "Plotting of type: " << this->toString() << " not implemented. ";
    break;
  }
}

void DynkinSimpleType::plotF4(Plot& output, int verticalOffset) {
  DynkinSimpleType::plotBn(output, 3, nullptr, verticalOffset);
  DynkinSimpleType::appendOneSingleConnectedRootToTheRight(output, 2, verticalOffset, "4", false);
}

void DynkinSimpleType::plotG2(Plot& output, int verticalOffset) {
  DynkinSimpleType::plotBn(output, 2, nullptr, verticalOffset);
  DynkinSimpleType::appendOneSingleConnectedRootToTheRight(output, 0, verticalOffset, "2", false);
}

void DynkinSimpleType::plotAn(
  Plot& output, int rank, Selection* filledRoots, int verticalOffset
) {
  List<std::string> labels;
  for (int i = 0; i < rank; i ++) {
    std::stringstream rootLabel;
    rootLabel << i + 1;
    labels.addOnTop(rootLabel.str());
  }
  DynkinSimpleType::plotHorizontalChainOfRoots(output, rank, verticalOffset, filledRoots, &labels);
}

void DynkinSimpleType::plotBC(
  Plot& output, int rank, Selection* filledRoots, int verticalOffset
) {
  if (rank <= 1) {
    return;
  }
  DynkinSimpleType::plotAn(output, rank - 1, filledRoots, verticalOffset);
  Vector<Rational> lastCenter, secondToLastCenter;
  secondToLastCenter.makeZero(2);
  secondToLastCenter[1] = verticalOffset;
  secondToLastCenter[0] = DynkinSimpleType::distanceBetweenRootCenters * (rank - 2);
  secondToLastCenter[1] -= DynkinSimpleType::radiusOfRootCircle;
  lastCenter = secondToLastCenter;
  lastCenter[0] += DynkinSimpleType::distanceBetweenRootCenters;
  output.drawSegment(lastCenter, secondToLastCenter);
  secondToLastCenter[1] += DynkinSimpleType::radiusOfRootCircle * 2;
  lastCenter[1] += DynkinSimpleType::radiusOfRootCircle * 2;
  output.drawSegment(lastCenter, secondToLastCenter);
  lastCenter[1] = verticalOffset;
  bool filled = false;
  if (filledRoots != nullptr) {
    filled = filledRoots->selected[rank - 1];
  }
  output.drawCircle(lastCenter, DynkinSimpleType::radiusOfRootCircle, "black", filled);
  lastCenter[1] += DynkinSimpleType::labelDistance;
  std::stringstream label;
  label << rank;
  output.drawLabel(lastCenter, label.str());
}

void DynkinSimpleType::plotCn(
  Plot& output, int rank, Selection* filledRoots, int verticalOffset
) {
  DynkinSimpleType::plotBC(output, rank, filledRoots, verticalOffset);
  if (rank < 2) {
    return;
  }
  Vector<Rational> left, right;
  left.makeZero(2);
  left[0] = DynkinSimpleType::distanceBetweenRootCenters * (rank - 2) + DynkinSimpleType::radiusOfRootCircle;
  left[1] = verticalOffset;
  right = left;
  right[0] += DynkinSimpleType::distanceBetweenRootCenters / 4;
  right[1] += DynkinSimpleType::distanceBetweenRootCenters / 4;
  output.drawSegment(left, right);
  right[1] -= DynkinSimpleType::distanceBetweenRootCenters / 2;
  output.drawSegment(left, right);
}

void DynkinSimpleType::plotBn(
  Plot& output,
  int rank,
  Selection* filledRoots,
  int verticalOffset
) {
  MacroRegisterFunctionWithName("DynkinSimpleType::plotBn");
  DynkinSimpleType::plotBC(output, rank, filledRoots, verticalOffset);
  if (rank < 2) {
    return;
  }
  Vector<Rational> left, right;
  left.makeZero(2);
  left[0] = DynkinSimpleType::distanceBetweenRootCenters * (rank - 2) + DynkinSimpleType::radiusOfRootCircle;
  left[1] = verticalOffset;
  right = left;
  left[0] += DynkinSimpleType::distanceBetweenRootCenters / 4;
  right[1] += DynkinSimpleType::distanceBetweenRootCenters / 4;
  output.drawSegment(left, right);
  right[1] -= DynkinSimpleType::distanceBetweenRootCenters / 2;
  output.drawSegment(left, right);
}

void DynkinSimpleType::plotDn(
  Plot& output,
  int rank,
  Selection* filledRoots,
  int verticalOffset
) {
  if (rank <= 2) {
    return;
  }
  DynkinSimpleType::plotAn(output, rank - 2, filledRoots, verticalOffset);
  Vector<Rational> bottomCenter, topCenter, lastAnCenter;
  lastAnCenter.makeZero(2);
  lastAnCenter[1] = verticalOffset;
  lastAnCenter[0] = DynkinSimpleType::distanceBetweenRootCenters * (rank - 3);
  // Approximate sqrt(2) by 1414/1000:
  Rational sqrt2DividedBy2 = Rational(1414, 1000) / 2;
  Rational distanceTimesSqrt2DividedByTwo = sqrt2DividedBy2 * DynkinSimpleType::distanceBetweenRootCenters;
  bottomCenter = lastAnCenter;
  bottomCenter[0] += distanceTimesSqrt2DividedByTwo;
  bottomCenter[1] -= distanceTimesSqrt2DividedByTwo;
  topCenter = bottomCenter;
  topCenter[1] += distanceTimesSqrt2DividedByTwo * 2;
  bool bottomFilled = false;
  bool topFilled = false;
  if (filledRoots != nullptr) {
    bottomFilled = filledRoots->selected[rank - 2];
    topFilled = filledRoots->selected[rank - 1];
  }
  output.drawCircle(bottomCenter, DynkinSimpleType::radiusOfRootCircle, "black", bottomFilled);
  output.drawCircle(topCenter, DynkinSimpleType::radiusOfRootCircle, "black", topFilled);
  bottomCenter[0] -= sqrt2DividedBy2 * DynkinSimpleType::radiusOfRootCircle;
  bottomCenter[1] += sqrt2DividedBy2 * DynkinSimpleType::radiusOfRootCircle;
  topCenter[0] -= sqrt2DividedBy2 * DynkinSimpleType::radiusOfRootCircle;
  topCenter[1] -= sqrt2DividedBy2 * DynkinSimpleType::radiusOfRootCircle;

  lastAnCenter[0] += sqrt2DividedBy2 * DynkinSimpleType::radiusOfRootCircle;
  lastAnCenter[1] += sqrt2DividedBy2 * DynkinSimpleType::radiusOfRootCircle;
  output.drawSegment(lastAnCenter, topCenter);
  lastAnCenter[1] -= sqrt2DividedBy2 * 2 * DynkinSimpleType::radiusOfRootCircle;
  output.drawSegment(lastAnCenter, bottomCenter);
  std::stringstream secondToLastRoot;
  secondToLastRoot << rank - 1;
  bottomCenter[0] += sqrt2DividedBy2 * DynkinSimpleType::radiusOfRootCircle + DynkinSimpleType::labelDistance;
  bottomCenter[1] -= sqrt2DividedBy2 * DynkinSimpleType::radiusOfRootCircle;
  output.drawLabel(bottomCenter, secondToLastRoot.str());
  std::stringstream lastRoot;
  lastRoot << rank;
  topCenter[0] += sqrt2DividedBy2 * DynkinSimpleType::radiusOfRootCircle + DynkinSimpleType::labelDistance;
  topCenter[1] += sqrt2DividedBy2 * DynkinSimpleType::radiusOfRootCircle;
  output.drawLabel(topCenter, lastRoot.str());
}

void DynkinSimpleType::plotE6(Plot& output, int verticalOffset) {
  List<std::string> labels = List<std::string>({
    "1", "3", "4", "5", "6"
  });
  DynkinSimpleType::plotHorizontalChainOfRoots(output, 5, verticalOffset, nullptr, &labels);
  Vector<Rational> left, right;
  left.makeZero(2);
  left[0] = DynkinSimpleType::distanceBetweenRootCenters * 2;
  left[1] = DynkinSimpleType::radiusOfRootCircle + verticalOffset;
  right = left;
  right[1] = DynkinSimpleType::distanceBetweenRootCenters - DynkinSimpleType::radiusOfRootCircle + verticalOffset;
  output.drawSegment(left, right);
  right[1] = DynkinSimpleType::distanceBetweenRootCenters + verticalOffset;
  output.drawCircle(right, DynkinSimpleType::radiusOfRootCircle, "black", false);
  right[1] += DynkinSimpleType::labelDistance;
  output.drawLabel(right, "2");
}

void DynkinSimpleType::appendOneSingleConnectedRootToTheRight(
  Plot &output,
  int segmentsSoFar,
  int verticalOffset,
  const std::string& label,
  bool filled
) {
  Vector<Rational> left, right;
  left.makeZero(2);
  left[0] = DynkinSimpleType::distanceBetweenRootCenters * segmentsSoFar;
  left[1] = verticalOffset;
  right = left;
  right[0] += DynkinSimpleType::distanceBetweenRootCenters;
  output.drawCircle(right, DynkinSimpleType::radiusOfRootCircle, "black", filled);
  right[1] += DynkinSimpleType::labelDistance;
  output.drawLabel(right, label);
  right[1] -= DynkinSimpleType::labelDistance;
  left[0] += DynkinSimpleType::radiusOfRootCircle;
  right[0] -= DynkinSimpleType::radiusOfRootCircle;
  output.drawSegment(left, right);
}

void DynkinSimpleType::plotE7(Plot& output, int verticalOffset) {
  DynkinSimpleType::plotE6(output, verticalOffset);
  DynkinSimpleType::appendOneSingleConnectedRootToTheRight(output, 4, verticalOffset, "7", false);
}

void DynkinSimpleType::plotE8(Plot& output, int verticalOffset) {
  DynkinSimpleType::plotE7(output, verticalOffset);
  DynkinSimpleType::appendOneSingleConnectedRootToTheRight(output, 5, verticalOffset, "8", false);
}

void SatakeDiagram::plot(Plot& output, int verticalOffset) {
  DynkinType::plotInitialize(output);
  switch (this->diagram) {
  case SatakeDiagram::AI:
    this->plotAI(output, verticalOffset);
    return;
  case SatakeDiagram::AII:
    this->plotAII(output, verticalOffset);
    return;
  case SatakeDiagram::AIII:
    this->plotAIII(output, verticalOffset);
    return;
  case SatakeDiagram::BI:
    this->plotBI(output, verticalOffset);
    return;
  default:
    break;
  }
}

void SatakeDiagram::plotAI(Plot& output, int verticalOffset) {
  DynkinSimpleType::plotAn(output, this->rank, nullptr, verticalOffset);
}

void SatakeDiagram::plotAII(Plot& output, int verticalOffset) {
  Selection filledRoots;
  filledRoots.initialize(this->rank);
  for (int i = 0; i < this->rank; i += 2) {
    filledRoots.addSelectionAppendNewIndex(i);
  }
  DynkinSimpleType::plotHorizontalChainOfRoots(
    output, this->rank, verticalOffset, &filledRoots, nullptr
  );
}

void SatakeDiagram::plotAIII(Plot& output, int verticalOffset) {
  Selection filledRoots;
  filledRoots.initialize(this->rank);
  int remainingRank = this->rank - (this->parameter + 1) * 2;
  for (int i = this->parameter + 1; i < this->parameter + remainingRank + 1; i ++) {
    filledRoots.addSelectionAppendNewIndex(i);
  }
  for (int i = 0 ; i < this->parameter + 1; i ++) {
    VoganDiagram::plotTwoElementOrbit(output, i, this->rank - i - 1, verticalOffset, this->rank);
  }
  DynkinSimpleType::plotHorizontalChainOfRoots(output, this->rank, &filledRoots);
}

void SatakeDiagram::plotBI(Plot& output, int verticalOffset) {
  Selection filledRoots;
  filledRoots.initialize(this->rank);
  for (int i = this->parameter + 1; i < this->rank; i ++) {
    filledRoots.addSelectionAppendNewIndex(i);
  }
  DynkinSimpleType::plotBn(output, this->rank, &filledRoots, verticalOffset);
}

void SatakeDiagram::plotCI(Plot& output, int verticalOffset) {
  (void) output;
  (void) verticalOffset;
}

SatakeDiagram VoganDiagram::computeSatakeDiagram() {
  SatakeDiagram result;
  switch (this->diagram) {
  case VoganDiagram::DiagramType::AI:
    result.diagram = SatakeDiagram::DiagramType::AI;
    break;
  case VoganDiagram::DiagramType::AII:
    result.diagram = SatakeDiagram::DiagramType::AII;
    break;
  case VoganDiagram::DiagramType::AIII:
    result.diagram = SatakeDiagram::DiagramType::AIII;
    break;
  case VoganDiagram::DiagramType::BI:
    result.diagram = SatakeDiagram::DiagramType::BI;
    break;
  case VoganDiagram::DiagramType::CI:
    result.diagram = SatakeDiagram::DiagramType::CI;
    break;
  case VoganDiagram::DiagramType::DI:
    result.diagram = SatakeDiagram::DiagramType::DI;
    break;
  default:
    global.fatal << "ComputeSatakeDiagram: not implemented yet." << global.fatal;
  }

  result.rank = this->rank;
  result.parameter = this->parameter;
  return result;
}

void VoganDiagram::plot(Plot& output) {
  DynkinType::plotInitialize(output);
  switch (this->diagram) {
  case VoganDiagram::AI:
    this->plotAI(output);
    return;
  case VoganDiagram::AII:
    this->plotAII(output);
    return;
  case VoganDiagram::AIII:
    this->plotAIII(output);
    return;
  case VoganDiagram::BI:
    this->plotBI(output);
    return;
  case VoganDiagram::CI:
    this->plotCI(output);
    return;
  case VoganDiagram::DI:
    this->plotDI(output);
    return;
  case VoganDiagram::DII:
    this->plotDII(output);
    return;
  case VoganDiagram::EI:
    this->plotEI(output);
    return;
  case VoganDiagram::EII:
    this->plotEII(output);
    return;
  case VoganDiagram::EIII:
    this->plotEIII(output);
    return;
  case VoganDiagram::EIV:
    this->plotEIV(output);
    return;
  default:
    return;
  }
}

void VoganDiagram::plotAI(Plot& output) {
  Selection filledNodes;
  filledNodes.initialize(this->rank);
  if (this->rank % 2 == 1) {
    filledNodes.addSelectionAppendNewIndex(this->rank / 2);
  }
  DynkinSimpleType::plotHorizontalChainOfRoots(output, this->rank, &filledNodes);
  for (int i = 0; i < this->rank / 2; i ++) {
    VoganDiagram::plotTwoElementOrbit(output, i, this->rank - i - 1, 0, this->rank);
  }
}

void VoganDiagram::plotAII(Plot& output) {
  DynkinSimpleType::plotHorizontalChainOfRoots(output, this->rank, nullptr);
  for (int i = 0; i < this->rank / 2; i ++) {
    VoganDiagram::plotTwoElementOrbit(output, i, this->rank - i - 1, 0, this->rank);
  }
}

void VoganDiagram::plotAIII(Plot& output) {
  Selection filledRoots;
  filledRoots.initialize(this->rank);
  filledRoots.addSelectionAppendNewIndex(this->parameter);
  DynkinSimpleType::plotHorizontalChainOfRoots(output, this->rank, &filledRoots);
}

void VoganDiagram::plotBI(Plot& output) {
  Selection filledRoots;
  filledRoots.initialize(this->rank);
  filledRoots.addSelectionAppendNewIndex(this->parameter);
  DynkinSimpleType::plotBn(output, this->rank, &filledRoots, 0);
}

void VoganDiagram::plotCI(Plot& output) {
  Selection filledRoots;
  filledRoots.initialize(this->rank);
  filledRoots.addSelectionAppendNewIndex(this->parameter);
  DynkinSimpleType::plotCn(output, this->rank, &filledRoots, 0);
}

void VoganDiagram::plotDI(Plot& output) {
  this->plotDII(output);
  Rational squareRootOfTwo = Rational(1414, 1000);
  Rational distanceBetweenCentersDividedBySquareRootOfTwo = DynkinSimpleType::distanceBetweenRootCenters;
  distanceBetweenCentersDividedBySquareRootOfTwo /= squareRootOfTwo;
  Vector<Rational> bottomEndAutomorphism;
  bottomEndAutomorphism.makeZero(2);
  bottomEndAutomorphism[0] = DynkinSimpleType::distanceBetweenRootCenters * (this->rank - 3);
  bottomEndAutomorphism[0] += distanceBetweenCentersDividedBySquareRootOfTwo;
  bottomEndAutomorphism[1] = - distanceBetweenCentersDividedBySquareRootOfTwo + 3;
  Vector<Rational> topEndAutomorphism = bottomEndAutomorphism;
  topEndAutomorphism[1] += distanceBetweenCentersDividedBySquareRootOfTwo * 2 - 6;
  output.drawSegment(bottomEndAutomorphism, topEndAutomorphism);
}

void VoganDiagram::plotDII(Plot& output) {
  Selection filledRoots;
  filledRoots.initialize(this->rank);
  filledRoots.addSelectionAppendNewIndex(this->parameter);
  DynkinSimpleType::plotDn(output, this->rank, &filledRoots, 0);
}

void VoganDiagram::plotTwoElementOrbit(
  Plot& output,
  int leftIndex,
  int rightIndex,
  int verticalOffset,
  int rootsOnThisPlotRow
) {
  Vector<Rational> left, right;
  left.makeZero(2);
  left[1] = verticalOffset;
  right = left;
  left[0] = DynkinSimpleType::distanceBetweenRootCenters * leftIndex;
  right[0] = DynkinSimpleType::distanceBetweenRootCenters * rightIndex;
  Vector<Rational> center = (left + right) / 2;
  PlotObject plotObject;
  plotObject.variableRangesJS.setSize(1);
  plotObject.variableRangesJS[0].setSize(2);
  plotObject.variableRangesJS[0][0] = "0";
  plotObject.variableRangesJS[0][1] = "3.14159";
  plotObject.paramLowJS  = "0";
  plotObject.paramHighJS = "3.14159";
  plotObject.numberOfSegmentsJS.setSize(1);
  plotObject.numberOfSegmentsJS[0] = "30";
  plotObject.coordinateFunctionsJS.setSize(2);
  plotObject.variablesInPlayJS.addOnTop("t");
  int totalWidth = (rootsOnThisPlotRow - 1) * DynkinSimpleType::distanceBetweenRootCenters;
  int width = DynkinSimpleType::distanceBetweenRootCenters * (rightIndex - leftIndex) / 2;
  std::string height = StringRoutines::toStringElement(DynkinSimpleType::distanceBetweenRootCenters * width / totalWidth);
  std::string widthString = StringRoutines::toStringElement(width);
  plotObject.coordinateFunctionsJS[0] = center[0].toString() + "+" + widthString + "*Math.cos(t)";
  plotObject.coordinateFunctionsJS[1] = center[1].toString() + "+" + height + "*Math.sin(t)";
  plotObject.colorJS = "black";
  plotObject.plotType = PlotObject::PlotTypes::parametricCurve;
  output.addPlotOnTop(plotObject);
}

void VoganDiagram::plotEI(Plot& output) {
  DynkinSimpleType::plotE6(output, 0);
  VoganDiagram::plotTwoElementOrbit(output, 0, 4, 0, - 4);
  VoganDiagram::plotTwoElementOrbit(output, 1, 3, 0, - 4);
  DynkinSimpleType::plotOneRoot(output, 2, true, DynkinSimpleType::distanceBetweenRootCenters);
}

void VoganDiagram::plotEII(Plot& output) {
  DynkinSimpleType::plotE6(output, 0);
  DynkinSimpleType::plotOneRoot(output, 2, true, DynkinSimpleType::distanceBetweenRootCenters);
}

void VoganDiagram::plotEIII(Plot& output) {
  DynkinSimpleType::plotE6(output, 0);
  DynkinSimpleType::plotOneRoot(output, 0, true, 0);
}

void VoganDiagram::plotEIV(Plot& output) {
  DynkinSimpleType::plotE6(output, 0);
  VoganDiagram::plotTwoElementOrbit(output, 0, 4, 0, - 4);
  VoganDiagram::plotTwoElementOrbit(output, 1, 3, 0, - 4);
}

void CartanInvolution::plotVoganDiagram(Plot& output) {
  return this->voganDiagram.plot(output);
}

void CartanInvolution::plotSatakeDiagram(Plot& output, int verticalOffset) {
  return this->voganDiagram.computeSatakeDiagram().plot(output, verticalOffset);
}

bool CalculatorLieTheory::cartanInvolutionInternal(
  Calculator& calculator, const Expression& input, CartanInvolution& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::cartanInvolutionInternal");
  if (input.size() != 4 && input.size() != 3 && input.size() != 2) {
    return calculator << "CartanInvolution takes as input three or two arguments.";
  }
  std::string typeString;
  if (!input[1].isOfType(&typeString)) {
    typeString = input[1].toString();
  }
  int diagramParameter = 1;
  int rank = 0;
  if (
    typeString == "EI"   ||
    typeString == "EII"  ||
    typeString == "EIII" ||
    typeString == "EIV"
  ) {
    rank = 6;
  } else {
    diagramParameter = 1;
    if (input.size() == 4) {
      if (!input[3].isSmallInteger(&diagramParameter)) {
        return calculator
        << "Failed to extract small integer from the 3rd argument: "
        << input[3].toString();
      }
    }
    rank = 0;
    if (!input[2].isSmallInteger(&rank)) {
      return calculator << "Failed to extract small integer from the 2nd argument.";
    }
  }
  if (!output.voganDiagram.assignParameter(
    typeString, rank, diagramParameter - 1, &calculator.comments
  )) {
    return false;
  }
  SemisimpleLieAlgebra& owner = calculator.objectContainer.getLieAlgebraCreateIfNotPresent(
    output.voganDiagram.dynkinTypeAmbientNoFailure()
  );
  if (!owner.hasImplementedCartanInvolution(
    output.voganDiagram, &output, &calculator.comments
  )) {
    return false;
  }
  return true;
}

bool CalculatorLieTheory::cartanInvolution(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::cartanInvolution");
  CartanInvolution involution;
  if (!CalculatorLieTheory::cartanInvolutionInternal(
    calculator, input, involution
  )) {
    return false;
  }
  Plot plotVogan;
  Plot plotSatake;
  involution.plotVoganDiagram(plotVogan);
  involution.plotSatakeDiagram(plotSatake, 0);
  std::stringstream result;
  result << "Vogan diagram:\n<br>\n"
  << plotVogan.getPlotHtml(calculator);
  if (plotSatake.getPlots().size > 0) {
    result << "\n<br>\nSatake diagram:\n<br>\n"
    << plotSatake.getPlotHtml(calculator);
  }
  result << "\n<br>\n" + involution.toString();
  return output.assignValue(
    calculator,  result.str()
  );
}

bool CalculatorLieTheory::isReductiveLieSubalgebra(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::isReductiveLieSubalgebra");
  SemisimpleLieAlgebra* ownerSemisimple = nullptr;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > elements, outputElements;
  if (!CalculatorLieTheory::elementsInSameLieAlgebra(
    calculator, input, output, ownerSemisimple, elements
  )) {
    return true;
  }
  global.comments << "Not implemented yet.";
  return false;
}

bool CalculatorLieTheory::slTwoRealFormStructure(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool forceRecompute
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::slTwoRealFormStructure");
  if (input.size() != 2) {
    return calculator << "Root subalgebra / sl(2)-subalgebras function expects 1 argument. ";
  }
  WithContext<SemisimpleLieAlgebra*> ownerLieAlgebra;
  if (!CalculatorConversions::convert(
    calculator,
    input[1],
    ownerLieAlgebra
  )) {
    return output.assignError(
      calculator,
      "Error extracting Lie algebra."
    );
  }
  FormatExpressions format;
  format.flagUseHTML = true;
  format.flagUseLatex = false;
  format.flagUsePNG = true;

  std::stringstream outRootHtmlFileName, outRootHtmlDisplayName, outSltwoMainFile, outKostantSekiguchi;

  SemisimpleLieAlgebra::FileNames& fileNames = ownerLieAlgebra.content->fileNames;
  std::string displayFolder = fileNames.displayFolderName("output/");

  outSltwoMainFile << displayFolder << fileNames.fileNameRelativePathSlTwoSubalgebras();
  outRootHtmlFileName << displayFolder << fileNames.fileNameNoPathRootSubalgebras();
  outRootHtmlDisplayName << displayFolder << fileNames.fileNameNoPathRootSubalgebras();
  outKostantSekiguchi << displayFolder << fileNames.fileNameSlTwoRealFormSubalgebraStructure();
  std::stringstream out;
  out << "<a href='"
  << outKostantSekiguchi.str()
  << "' target='_blank'>"
  << ownerLieAlgebra.content->toStringLieAlgebraName() << "</a>";
  if (FileOperations::fileExistsVirtual(outKostantSekiguchi.str()) && !forceRecompute) {
    out << "<br>The table is precomputed and served from the hard disk. <br>";
    return output.assignValue(calculator, out.str());
  }
  if (!global.userDefaultHasAdminRights()) {
    return calculator << "The real form structure is not precomputed. "
    << "Triggering a computation requires a logged-in administrator.";
  }
  SemisimpleSubalgebras& subalgebras =
  calculator.objectContainer.getSemisimpleSubalgebrasCreateIfNotPresent(
    ownerLieAlgebra.content->weylGroup.dynkinType
  );
  subalgebras.computeStructureRealFormsWriteFiles(
    *ownerLieAlgebra.content,
    calculator.objectContainer.algebraicClosure,
    calculator.objectContainer.semisimpleLieAlgebras,
    calculator.objectContainer.slTwoSubalgebras,
    &out
  );
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::rootSubalgebrasAndSlTwos(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool showSLTwos,
  bool computeRealFormSlTwos,
  bool mustRecompute
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::rootSAsAndSltwos");
  if (input.size() != 2) {
    return calculator << "Root subalgebra / sl(2)-subalgebras function expects 1 argument. ";
  }
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebra;
  if (!CalculatorConversions::convert(
    calculator,
    input[1],
    semisimpleLieAlgebra
  )) {
    return output.assignError(
      calculator,
      "Error extracting Lie algebra."
    );
  }
  FormatExpressions format;
  format.flagUseHTML = true;
  format.flagUseLatex = false;
  format.flagUsePNG = true;

  std::stringstream outRootHtmlFileName, outRootHtmlDisplayName, outSltwoMainFile;

  std::string displayFolder = semisimpleLieAlgebra.content->fileNames.displayFolderName("output/");

  outSltwoMainFile << displayFolder << semisimpleLieAlgebra.content->fileNames.fileNameRelativePathSlTwoSubalgebras();
  outRootHtmlFileName    << displayFolder << semisimpleLieAlgebra.content->fileNames.fileNameNoPathRootSubalgebras();
  outRootHtmlDisplayName << displayFolder << semisimpleLieAlgebra.content->fileNames.fileNameNoPathRootSubalgebras();
  if (
    !FileOperations::fileExistsVirtual(outSltwoMainFile.str()) ||
    !FileOperations::fileExistsVirtual(outRootHtmlFileName.str())
  ) {
    mustRecompute = true;
  }
  std::stringstream out;
  if (mustRecompute) {
    SlTwoSubalgebras slTwoSubalgebras(*semisimpleLieAlgebra.content);
    slTwoSubalgebras.rootSubalgebras.flagPrintParabolicPseudoParabolicInfo = true;
    semisimpleLieAlgebra.content->findSl2Subalgebras(
      *semisimpleLieAlgebra.content,
      slTwoSubalgebras,
      computeRealFormSlTwos,
      &calculator.objectContainer.algebraicClosure
    );
    slTwoSubalgebras.writeHTML(&format);
    Plot plot;
    semisimpleLieAlgebra.content->weylGroup.dynkinType.plot(plot);
    semisimpleLieAlgebra.content->writeHTML(true, false, plot.getPlotHtml2d(calculator));
  } else {
    out << "The table is precomputed and served from the hard disk. <br>";
  }
  out << "<a href='"
  << (showSLTwos ? outSltwoMainFile.str() : outRootHtmlDisplayName.str())
  << "' target='_blank'>"
  << semisimpleLieAlgebra.content->toStringLieAlgebraName() << " </a>";
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::decomposeFDPartGeneralizedVermaModuleOverLeviPart(
  Calculator& calculator, const Expression& input, Expression& output
) {
  RecursionDepthCounter recursionCounter(&calculator.recursionDepth);
  if (!input.isListNElements(5)) {
    return output.assignError(
      calculator,
      "Function decompose finite-dimensional part of "
      "generalized Verma over Levi expects 4 arguments."
    );
  }
  const Expression& typeNode = input[1];
  const Expression& weightNode = input[2];
  const Expression& inducingParNode = input[3];
  const Expression& splittingParNode = input[4];
  WithContext<SemisimpleLieAlgebra*> ownerSSPointer;
  if (!CalculatorConversions::convert(
    calculator,
    typeNode,
    ownerSSPointer
  )) {
    return output.assignError(
      calculator,
      "Error extracting Lie algebra."
    );
  }
  Vector<RationalFraction<Rational> > weightFundCoords;
  Vector<Rational> inducingParSel, splittingParSel;
  SemisimpleLieAlgebra& ownerSS = *ownerSSPointer.content;
  WeylGroupData& weylGroup = ownerSS.weylGroup;
  int dimension = ownerSS.getRank();
  ExpressionContext finalContext(calculator);
  if (!calculator.getVector<RationalFraction<Rational> >(
    weightNode,
    weightFundCoords,
    &finalContext,
    dimension
  )) {
    return output.assignError(
      calculator,
      "Failed to extract highest weight from the second argument."
    );
  }
  if (!calculator.getVector<Rational>(
    inducingParNode, inducingParSel, &finalContext, dimension
  )) {
    return output.assignError(
      calculator,
      "Failed to extract parabolic selection from the third argument"
    );
  }
  if (!calculator.getVector<Rational>(
    splittingParNode, splittingParSel, &finalContext, dimension
  )) {
    return output.assignError(
      calculator,
      "Failed to extract parabolic selection from the fourth argument"
    );
  }
  calculator << "Your input weight in fundamental coordinates: " << weightFundCoords.toString();
  calculator << "<br>Your input weight in simple coordinates: "
  << weylGroup.getSimpleCoordinatesFromFundamental(
    weightFundCoords, RationalFraction<Rational>::zeroRational()
  ).toString()
  << "<br>Your inducing parabolic subalgebra: " << inducingParSel.toString() << "."
  << "<br>The parabolic subalgebra I should split over: " << splittingParSel.toString() << ".";
  ModuleSSalgebra<RationalFraction<Rational> > theMod;
  Selection selInducing = inducingParSel;
  Selection selSplittingParSel = splittingParSel;
  theMod.makeFromHW(ownerSS, weightFundCoords, selInducing, 1, 0, nullptr, false);
  std::string report;
  theMod.splitOverLevi(&report, selSplittingParSel);
  return output.assignValue(calculator, report);
}

bool CalculatorLieTheory::parabolicWeylGroups(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::parabolicWeylGroups");
  if (input.size() != 2) {
    return false;
  }
  Selection selectionParSel;
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebraWithContext;
  if (!CalculatorConversions::convert(
    calculator,
    input[1],
    semisimpleLieAlgebraWithContext
  )) {
    return output.assignError(calculator, "Error extracting Lie algebra.");
  }
  SemisimpleLieAlgebra& semisimpleLieAlgebra = *semisimpleLieAlgebraWithContext.content;
  int numCycles = MathRoutines::twoToTheNth(selectionParSel.numberOfElements);
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms subgroup;
  std::stringstream out;
  for (int i = 0; i < numCycles; i ++, selectionParSel.incrementSelection()) {
    subgroup.makeParabolicFromSelectionSimpleRoots(semisimpleLieAlgebra.weylGroup, selectionParSel, 2000);
    out << "<hr>" << HtmlRoutines::getMathNoDisplay(subgroup.toString());
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::weylDimensionFormula(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::weylDimensionFormula");
  RecursionDepthCounter recursionCounter(&calculator.recursionDepth);
  if (input.size() != 3) {
    return output.assignError(calculator, "This function takes 2 arguments");
  }

  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebra;
  if (!CalculatorConversions::convert(
    calculator,
    input[1],
    semisimpleLieAlgebra
  )) {
    return output.assignError(
      calculator,
      "Error extracting Lie algebra."
    );
  }
  Vector<RationalFraction<Rational> > weight;
  if (!calculator.getVector<RationalFraction<Rational> >(
    input[2],
    weight,
    &semisimpleLieAlgebra.context,
    semisimpleLieAlgebra.content->getRank()
  )) {
    return output.assignError(
      calculator,
      "Failed to convert the argument of the "
      "function to a highest weight vector"
    );
  }
  RationalFraction<Rational> rfOne;
  rfOne.makeOne();
  Vector<RationalFraction<Rational> > weightInSimpleCoords;
  FormatExpressions format;
  semisimpleLieAlgebra.context.getFormat(format);
  weightInSimpleCoords = semisimpleLieAlgebra.content->weylGroup.getSimpleCoordinatesFromFundamental(weight);
  calculator << "<br>Weyl dim formula input: simple coords: "
  << weightInSimpleCoords.toString(&format)
  << ", fundamental coords: " << weight.toString(&format);
  RationalFraction<Rational> result = semisimpleLieAlgebra.content->weylGroup.weylDimensionFormulaSimpleCoordinates(weightInSimpleCoords);
  return output.assignValueWithContext(calculator, result, semisimpleLieAlgebra.context);
}

bool CalculatorLieTheory::parabolicWeylGroupsBruhatGraph(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::parabolicWeylGroupsBruhatGraph");
  RecursionDepthCounter theRecursion(&calculator.recursionDepth);
  Selection parabolicSel;
  Vector<RationalFraction<Rational> > highestWeightFundamentalCoordinates;
  Vector<RationalFraction<Rational> > highestWeightSimpleCoordinates;
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebraPointer;
  if (!calculator.getTypeHighestWeightParabolic(
    calculator,
    input,
    output,
    highestWeightFundamentalCoordinates,
    parabolicSel,
    semisimpleLieAlgebraPointer
  )) {
    return output.assignError(calculator, "Failed to extract highest weight vector data");
  } else {
    if (output.isError()) {
      return true;
    }
  }
  SemisimpleLieAlgebra& semisimpleLieAlgebra = *semisimpleLieAlgebraPointer.content;

  WeylGroupData& ambientWeylGroup = semisimpleLieAlgebra.weylGroup;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms subgroup;
  std::stringstream out;
  if (!subgroup.makeParabolicFromSelectionSimpleRoots(ambientWeylGroup, parabolicSel, 500)) {
    return output.assignError(calculator, "<br><br>Failed to generate Weyl subgroup, 500 elements is the limit");
  }
  subgroup.findQuotientRepresentatives(2000);
  out << "<br>Number elements of the coset: "
  << subgroup.RepresentativesQuotientAmbientOrder.size;
  out << "<br>Number of elements of the Weyl group of the Levi part: " << subgroup.allElements.size;
  out << "<br>Number of elements of the ambient Weyl: "
  << subgroup.ambientWeyl->group.elements.size;
  FormatExpressions format;
  semisimpleLieAlgebraPointer.context.getFormat(format);
  if (subgroup.allElements.size > 498) {
    if (subgroup.ambientWeyl->sizeByFormulaOrNegative1('E', 6) <= subgroup.ambientWeyl->group.getSize()) {
      out << "Weyl group is too large. <br>";
    } else {
      out << "Weyl group is too large for LaTeX. <br>";
    }
  } else {
    std::stringstream outputFileContent, outputFileContent2;
    std::string fileHasse, fileCosetGraph;
    bool useJavascript = (subgroup.allElements.size < 100);
    outputFileContent << "\\documentclass{article}\\usepackage[all,cmtip]{xy}\\begin{document}\n";
    outputFileContent << "\\[" << subgroup.toStringBruhatGraph() << "\\]";
    outputFileContent << "\n\\end{document}";
    outputFileContent2 << "\\documentclass{article}\\usepackage[all,cmtip]{xy}\\begin{document}\n";
    outputFileContent2 << "\\[" << subgroup.toStringCosetGraph() << "\\]";
    outputFileContent2 << "\n\\end{document}";
    calculator.writeDefaultLatexFileReturnHtmlLink(outputFileContent.str(), &fileHasse, true);
    calculator.writeDefaultLatexFileReturnHtmlLink(outputFileContent2.str(), &fileCosetGraph, true);
    out << "<hr>"
    << " The Hasse graph of the Weyl group of the Levi part follows. <a href=\""
    << fileHasse << ".tex\"> "
    << fileHasse << ".tex</a>";
    out << ", <iframe src =\"" << fileHasse
    << ".png\" width =\"800\" height =\"600\">"
    << fileHasse << ".png</iframe>";
    out << "<hr> The coset graph of the Weyl group of the Levi part follows. "
    << "The cosets are right, i.e. a coset "
    << " of the subgroup X is written in the form Xw, where w is one of the elements below. "
    << "<a href=\""
    << fileCosetGraph
    << ".tex\"> " << fileCosetGraph << ".tex</a>";
    out << ", <iframe src =\"" << fileCosetGraph
    << ".png\" width =\"800\" height =\"600\"> " << fileCosetGraph << ".png</iframe>";
    out << "<b>The .png file might be bad if LaTeX crashed while trying to process it; "
    << "please check whether the .tex corresponds to the .png.</b>";
    out << "<hr>Additional printout follows.<br> ";
    out << "<br>Representatives of the coset follow. Below them you can find "
    << "the elements of the subgroup. <br>";
    out << "<table><tr><td>Element</td><td>weight simple coords</td>"
    << "<td>weight fund. coords</td></tr>";
    format.fundamentalWeightLetter = "\\omega";
    for (int i = 0; i < subgroup.RepresentativesQuotientAmbientOrder.size; i ++) {
      ElementWeylGroup& current = subgroup.RepresentativesQuotientAmbientOrder[i];
      out << "<tr><td>"
      << (useJavascript ? HtmlRoutines::getMathNoDisplay(current.toString()) : current.toString())
      << "</td>";
      highestWeightSimpleCoordinates = semisimpleLieAlgebra.weylGroup.getSimpleCoordinatesFromFundamental(highestWeightFundamentalCoordinates, RationalFraction<Rational>::zeroRational());
      semisimpleLieAlgebra.weylGroup.actOnRhoModified(subgroup.RepresentativesQuotientAmbientOrder[i], highestWeightSimpleCoordinates);
      out << "<td>"
      << (useJavascript ? HtmlRoutines::getMathNoDisplay(highestWeightSimpleCoordinates.toString(&format))
      : highestWeightSimpleCoordinates.toString(&format))
      << "</td>";
      Vector<RationalFraction<Rational> > root;
      root = semisimpleLieAlgebra.weylGroup.getFundamentalCoordinatesFromSimple(highestWeightSimpleCoordinates);
      std::string fundamentalString = root.toStringLetterFormat(format.fundamentalWeightLetter, &format);
      out << "<td>" << (useJavascript ? HtmlRoutines::getMathNoDisplay(fundamentalString): fundamentalString)
      << "</td>";
      out << "</tr>";
    }
    out << "</table><hr>";
    out << subgroup.toString();
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::decomposeCharGenVerma(
  Calculator& calculator, const Expression& input, Expression& output
) {
  RecursionDepthCounter recursionIncrementer(&calculator.recursionDepth);
  MacroRegisterFunctionWithName("CalculatorLieTheory::decomposeCharGenVerma");
  Vector<RationalFraction<Rational> > highestWeightFundamentalCoordinates;
  Vector<RationalFraction<Rational> > highestWeightSimpleCoordinates;
  Vector<RationalFraction<Rational> > highestWeightFundamentalCoordinatesFiniteDimensionalPart;
  Vector<RationalFraction<Rational> > highestWeightSimpleCoordinatesFiniteDimensionalPart;
  Selection parabolicSelection;
  Selection invertedParabolicSelection;
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebra;
  output.reset(calculator);
  if (!calculator.getTypeHighestWeightParabolic<RationalFraction<Rational> >(
    calculator,
    input,
    output,
    highestWeightFundamentalCoordinates,
    parabolicSelection,
    semisimpleLieAlgebra
  )) {
   return false;
  }
  if (output.isError()) {
    return true;
  }
  std::stringstream out;
  FormatExpressions format;
  semisimpleLieAlgebra.context.getFormat(format);
  out << "<br>Highest weight: " << highestWeightFundamentalCoordinates.toString(&format)
  << "<br>Parabolic selection: " << parabolicSelection.toString();
  highestWeightFundamentalCoordinatesFiniteDimensionalPart = highestWeightFundamentalCoordinates;
  for (int i = 0; i < parabolicSelection.cardinalitySelection; i ++) {
    highestWeightFundamentalCoordinatesFiniteDimensionalPart[parabolicSelection.elements[i]] = 0;
  }
  KazhdanLusztigPolynomials theKLpolys;
  WeylGroupData& weylGroup = semisimpleLieAlgebra.content->weylGroup;
  if (!theKLpolys.computeKLPolys(&weylGroup)) {
    return output.assignError(
      calculator,
      "failed to generate Kazhdan-Lusztig polynomials (output too large?)"
    );
  }
  highestWeightSimpleCoordinatesFiniteDimensionalPart = weylGroup.getSimpleCoordinatesFromFundamental(
    highestWeightFundamentalCoordinatesFiniteDimensionalPart, RationalFraction<Rational>::zeroRational()
  );
  highestWeightSimpleCoordinatesFiniteDimensionalPart += weylGroup.rho;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms subgroup;
  if (!subgroup.makeParabolicFromSelectionSimpleRoots(weylGroup, parabolicSelection, 1000)) {
    return output.assignError(
      calculator,
      "Failed to generate Weyl subgroup of Levi part (possibly too large? element limit is 1000)."
    );
  }
  highestWeightSimpleCoordinates = weylGroup.getSimpleCoordinatesFromFundamental(
    highestWeightFundamentalCoordinates, RationalFraction<Rational>::zeroRational()
  );
  List<ElementWeylGroup> weylGroupElements;
  subgroup.getGroupElementsIndexedAsAmbientGroup(weylGroupElements);
  Vector<RationalFraction<Rational> > currentHighestWeight;
  out << "<br>Orbit modified with small rho: "
  << "<table><tr><td>Simple coords</td><td>Fund coords</td></tr>";
  for (int i = 0; i < weylGroup.group.elements.size; i ++) {
    currentHighestWeight = highestWeightSimpleCoordinates;
    currentHighestWeight += subgroup.getRho();
    weylGroup.actOn(i, currentHighestWeight);
    currentHighestWeight -= subgroup.getRho();
    out << "<tr><td>" << currentHighestWeight.toString() << "</td><td>"
    << weylGroup.getFundamentalCoordinatesFromSimple(currentHighestWeight).toString() << "</td></tr>";
  }
  out << "</table>";
  out << "<br>The rho of the Levi part is: "
  << subgroup.getRho().toString() << "<br>Weyl group of Levi part follows. "
  << "<br><table><tr><td>Weyl element</td>"
  << "<td>Image hw under small rho modified action fund coords</td>"
  << "<td>Character Verma given h.w.</td></tr>";
  invertedParabolicSelection = parabolicSelection;
  invertedParabolicSelection.invertSelection();
  CharacterSemisimpleLieAlgebraModule<RationalFraction<Rational> > theChar, currentChar;
  Weight<RationalFraction<Rational> > monomial;
  theChar.makeZero();
  FormatExpressions formatChars;
  formatChars.finiteDimensionalRepresentationLetter = "L";
  formatChars.fundamentalWeightLetter = "\\omega";
  formatChars.flagUseLatex = true;
  subgroup.ambientWeyl = theKLpolys.weylGroup;
  for (int i = 0; i < weylGroupElements.size; i ++) {
    ElementWeylGroup& currentElement = weylGroupElements[i];
    out << "<tr><td>" << currentElement.toString() << "</td>";

    int indexInWeyl = theKLpolys.weylGroup->group.elements.getIndex(currentElement);
    if (indexInWeyl == - 1) {
      global.fatal << "Something is wrong: "
      << "I am getting that an element of the Weyl group of the Levi part of "
      << "the parabolic does not lie in the ambient Weyl group, which is impossible. "
      << "There is a bug somewhere; crashing in accordance. " << global.fatal;
    }
    currentChar.makeZero();
    monomial.owner = semisimpleLieAlgebra.content;
    for (int j = 0; j < theKLpolys.kazhdanLuzstigCoefficients[indexInWeyl].size; j ++) {
      if (!theKLpolys.kazhdanLuzstigCoefficients[indexInWeyl][j].isEqualToZero()) {
        currentHighestWeight = highestWeightSimpleCoordinates;
        weylGroup.actOnRhoModified(j, currentHighestWeight);
        monomial.weightFundamentalCoordinates = weylGroup.getFundamentalCoordinatesFromSimple(currentHighestWeight);
        int sign = (currentElement.generatorsLastAppliedFirst.size - weylGroup.group.elements[j].generatorsLastAppliedFirst.size) % 2 == 0 ? 1 : - 1;
        currentChar.addMonomial(monomial, theKLpolys.kazhdanLuzstigCoefficients[indexInWeyl][j] * sign);
      }
    }
    currentHighestWeight = highestWeightSimpleCoordinates;
    currentHighestWeight += subgroup.getRho();
    weylGroup.actOn(indexInWeyl, currentHighestWeight);
    currentHighestWeight -= subgroup.getRho();
    out << "<td>" << weylGroup.getFundamentalCoordinatesFromSimple(currentHighestWeight).toStringLetterFormat("\\omega") << "</td>";
    out << "<td>" << HtmlRoutines::getMathNoDisplay(currentChar.toString(&formatChars)) << "</td>";
    if (currentElement.generatorsLastAppliedFirst.size % 2 == 1) {
      currentChar *= - 1;
    }
    theChar += currentChar;
    out << "</tr>";
  }
  out << "</table>";
  out << "Final char: " << HtmlRoutines::getMathNoDisplay(theChar.toString(&formatChars));
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::constructCartanSubalgebra(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::constructCartanSubalgebra");
  SubalgebraSemisimpleLieAlgebra subalgebra;
  WithContext<ElementSemisimpleLieAlgebra<AlgebraicNumber> > element;
  for (int i = 1; i < input.size(); i ++) {
    if (!CalculatorConversions::convertWithoutComputation(
      calculator, input[i], element
    )) {
      return calculator
      << "Failed to extract element of a semisimple Lie algebra from "
      << input[i].toString();
    }
    subalgebra.generators.addOnTop(element.content);
  }
  for (int i = 0; i < subalgebra.generators.size; i ++) {
    if (!subalgebra.generators[i].isEqualToZero()) {
      if (subalgebra.owner != nullptr) {
        if (subalgebra.owner != subalgebra.generators[i].getOwner()) {
          return calculator << "The input elements in " << input.toString()
          << " belong to different semisimple Lie algebras";
        }
      }
      subalgebra.owner = subalgebra.generators[i].getOwner();
    }
  }
  if (subalgebra.owner == nullptr) {
    return calculator << "Failed to extract input semisimple Lie algebra "
    << "elements from the inputs of " << input.toString();
  }
  subalgebra.computeBasis();
  subalgebra.computeCartanSubalgebra();
  return output.assignValue(calculator, subalgebra.toString());
}

bool CalculatorLieTheory::growDynkinType(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::growDynkinType");
  if (input.size() != 3) {
    return false;
  }
  const Expression& smallerTypeE = input[1];
  DynkinType smallDynkinType;
  if (!CalculatorConversions::functionDynkinType(
    calculator, smallerTypeE, smallDynkinType
  )) {
    return false;
  }
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebra;
  if (!CalculatorConversions::convert(
    calculator,
    input[2],
    semisimpleLieAlgebra
  )) {
    return output.assignError(calculator, "Error extracting ambient Lie algebra.");
  }
  SemisimpleSubalgebras subalgebras;
  subalgebras.initHookUpPointers(
    *semisimpleLieAlgebra.content,
    &calculator.objectContainer.algebraicClosure,
    &calculator.objectContainer.semisimpleLieAlgebras,
    &calculator.objectContainer.slTwoSubalgebras
  );
  subalgebras.slTwoSubalgebras.checkMinimalContainingRootSubalgebras();
  subalgebras.computeSl2sInitOrbitsForComputationOnDemand(false);
  if (!subalgebras.ranksAndIndicesFit(smallDynkinType)) {
    return output.assignError(
      calculator,
      "Error: type " + smallDynkinType.toString() +
      " does not fit inside " +
      semisimpleLieAlgebra.content->weylGroup.dynkinType.toString()
    );
  }
  List<DynkinType> largerTypes;
  List<List<int> > imagesSimpleRoots;
  if (!subalgebras.growDynkinType(smallDynkinType, largerTypes, &imagesSimpleRoots)) {
    return output.assignError(
      calculator,
      "Error: growing type " + smallDynkinType.toString() +
      " inside " + semisimpleLieAlgebra.content->weylGroup.dynkinType.toString() +
      " failed. "
    );
  }
  std::stringstream out;
  out << "Inside " << semisimpleLieAlgebra.content->weylGroup.dynkinType.toString()
  << ", input type " << smallDynkinType.toString();
  if (largerTypes.size == 0) {
    out << " cannot grow any further. ";
  } else {
    CandidateSemisimpleSubalgebra tempCandidate;
    out << " can grow to the following types. <br>";
    out << "<table border='1'>"
    << "<td>Larger type</td>"
    << "<td>Root injection</td>"
    << "<td>Highest weight module containing new simple generator</td></tr>";
    for (int i = 0; i < largerTypes.size; i ++) {
      out << "<tr><td>" << largerTypes[i].toString() << "</td>";
      out << "<td>";
      for (int j = 0; j < imagesSimpleRoots[i].size; j ++) {
        out << "r_" << j + 1 << " -> " << "r_" << imagesSimpleRoots[i][j] + 1;
        if (j != imagesSimpleRoots[i].size) {
          out << ", ";
        }
      }
      out << "</td><td>";
      Vector<Rational> currentHighestWeight = subalgebras.getHighestWeightFundamentalNewComponentFromImagesOldSimpleRootsAndNewRoot(
        largerTypes[i], imagesSimpleRoots[i], tempCandidate
      );
      out << HtmlRoutines::getMathNoDisplay(
        currentHighestWeight.toStringLetterFormat("\\omega")
      );
      out << "</td></tr>";
    }
    out << "</table>";
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::computeSemisimpleSubalgebras(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::computeSemisimpleSubalgebras");
  if (input.size() != 2) {
    return calculator << "Semisimple subalgebras function expects 1 argument. ";
  }
  WithContext<SemisimpleLieAlgebra*> lieAlgebraPointer;
  if (!CalculatorConversions::convert(
    calculator,
    input[1],
    lieAlgebraPointer
  )) {
    return output.assignError(calculator, "Error extracting Lie algebra.");
  }
  SemisimpleLieAlgebra& lieAlgebra = *lieAlgebraPointer.content;
  std::stringstream out;
  if (lieAlgebra.getRank() > 6) {
    out << "<b>This code is completely experimental and has been set to run up to rank 6. "
    << "As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
    return output.assignValue(calculator, out.str());
  }
  SemisimpleSubalgebras& semisimpleSubalgebras =
  calculator.objectContainer.getSemisimpleSubalgebrasCreateIfNotPresent(lieAlgebra.weylGroup.dynkinType);
  semisimpleSubalgebras.flagComputePairingTable = false;
  semisimpleSubalgebras.flagComputeNilradicals = false;
  semisimpleSubalgebras.findTheSemisimpleSubalgebrasFromScratch(
    lieAlgebra,
    calculator.objectContainer.algebraicClosure,
    calculator.objectContainer.semisimpleLieAlgebras,
    calculator.objectContainer.slTwoSubalgebras,
    nullptr
  );
  return output.assignValue(calculator, semisimpleSubalgebras);
}

template < >
SemisimpleSubalgebras& Expression::getValueNonConst() const;

bool CalculatorLieTheory::computePairingTablesAndFKFTsubalgebras(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::computePairingTablesAndFKFTsubalgebras");
  if (!global.userDefaultHasAdminRights()) {
    return calculator << "Function computePairingTablesAndFKFTsubalgebras requires administrative rights. ";
  }
  if (!input[1].isOfType<SemisimpleSubalgebras>()) {
    return calculator << "<hr>Input of ComputeFKFT must be of type semisimple subalgebras. ";
  }
  SemisimpleSubalgebras& subalgebras = input[1].getValueNonConst<SemisimpleSubalgebras>();
  subalgebras.flagComputePairingTable = true;
  subalgebras.flagComputeNilradicals = true;
  subalgebras.computePairingTablesAndFKFTtypes();
  output = input;
  std::fstream file;
  std::string fileName;
  fileName = "FKFTcomputation.html";
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML = true;
  tempFormat.flagUseLatex = true;
  tempFormat.flagUseHTML = true;
  tempFormat.flagCandidateSubalgebraShortReportOnly = false;
  FileOperations::openFileCreateIfNotPresentVirtual(file, "output/" + fileName, false, true, false);
  file << subalgebras.toString(&tempFormat, false);
  std::stringstream out;
  out << "<a href='" << global.displayPathOutputFolder << "FKFTcomputation.html'>FKFTcomputation.html</a>";
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::getCentralizerChainsSemisimpleSubalgebras(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::getCentralizerChainsSemisimpleSubalgebras");
  if (!input.isOfType<SemisimpleSubalgebras>()) {
    return calculator << "<hr>Input of getCentralizerChains must be of type semisimple subalgebras. ";
  }
  SemisimpleSubalgebras& subalgebras = input.getValueNonConst<SemisimpleSubalgebras>();
  List<List<int> > chains;
  std::stringstream out;
  subalgebras.getCentralizerChains(chains);
  Expression currentChainE;
  out << chains.size << " chains total. <br>";
  for (int i = 0; i < chains.size; i ++) {
    out << "<br>Chain " << i + 1 << ": LoadSemisimpleSubalgebras{}( "
    << subalgebras.owner->weylGroup.dynkinType.toString() << ", (";
    for (int j = 0; j < chains[i].size; j ++) {
      CalculatorConversions::storeCandidateSubalgebra(
        calculator, subalgebras.subalgebras.values[chains[i][j]], currentChainE
      );
      out << currentChainE.toString();
      if (j != chains[i].size - 1) {
        out << ", ";
      }
    }
    out << ")  )";
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::getPrincipalSl2Index(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::getPrincipalSl2Index");
  DynkinType dynkinType;
  if (!CalculatorConversions::dynkinType(calculator, input, dynkinType)) {
    return calculator << "Failed to convert "
    << input.toString() << " to DynkinType.";
  }
  return output.assignValue(calculator, dynkinType.getPrincipalSlTwoCartanSymmetricInverseScale());
}

bool CalculatorLieTheory::getDynkinIndicesSlTwoSubalgebras(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::getDynkinIndicesSlTwoSubalgebras");
  DynkinType dynkinType;
  if (!CalculatorConversions::dynkinType(calculator, input, dynkinType)) {
    return calculator << "Failed to convert "
    << input.toString() << " to DynkinType.";
  }
  if (dynkinType.getRank() > 20) {
    return calculator
    << "Getting absolute Dynkin indices of sl(2)-subalgebras "
    << "is restricted up to rank 20 "
    << "(for computational feasibility reasons). ";
  }
  List<List<Rational> > bufferIndices;
  HashedList<DynkinSimpleType> bufferTypes;
  HashedList<Rational> indices;
  dynkinType.getDynkinIndicesSl2Subalgebras(
    bufferIndices, bufferTypes, indices, &calculator.objectContainer.algebraicClosure
  );
  std::stringstream out;
  out << "There are " << indices.size << " absolute Dynkin indices. The indices are: "
  << indices.toStringCommaDelimited();
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::generateVectorSpaceClosedWithRespectToLieBracket(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::generateVectorSpaceClosedWithRespectToLieBracket");
  Vector<ElementWeylAlgebra<Rational> > theOps;
  if (input.size() <= 1) {
    return false;
  }
  int upperBound = - 1;
  if (!input[1].isSmallInteger(&upperBound)) {
    return calculator << "<hr>Failed to extract upper bound "
    << "for the vector space dimension from the first argument. ";
  }
  Expression inputModded = input;
  inputModded.removeChildShiftDown(1);
  ExpressionContext context(calculator);
  if (!calculator.getVectorFromFunctionArguments(inputModded, theOps, &context)) {
    Vector<ElementUniversalEnveloping<RationalFraction<Rational> > > lieAlgebraElements;
    context.initialize(calculator);
    if (!calculator.getVectorFromFunctionArguments(inputModded, lieAlgebraElements, &context)) {
      return calculator
      << "<hr>Failed to extract elements of Weyl algebra and "
      << "failed to extract elements of UE algebra from input "
      << input.toString();
    }
    FormatExpressions format;
    context.getFormat(format);
    std::stringstream out;
    out << "Starting elements: <br>";
    for (int i = 0; i < lieAlgebraElements.size; i ++) {
      out << HtmlRoutines::getMathNoDisplay(lieAlgebraElements[i].toString(&format)) << "<br>";
    }
    bool success = MathRoutines::generateVectorSpaceClosedWithRespectToLieBracket(lieAlgebraElements, upperBound);
    if (!success) {
      out << "<br>Did not succeed with generating vector space, "
      << "instead got a vector space with basis "
      << lieAlgebraElements.size << " exceeding the limit. "
      << "The basis generated before exceeding the limit was: "
      << lieAlgebraElements.toString();
    } else {
      out << "<br>Lie bracket generates vector space of dimension "
      << lieAlgebraElements.size << " with basis:";
      for (int i = 0; i < lieAlgebraElements.size; i ++) {
        out << "<br>";
        if (lieAlgebraElements.size > 50) {
          out << lieAlgebraElements[i].toString(&format);
        } else {
          out << HtmlRoutines::getMathNoDisplay(lieAlgebraElements[i].toString(&format));
        }
      }
    }
    return output.assignValue(calculator, out.str());
  }
  FormatExpressions format;
  context.getFormat(format);
  std::stringstream out;
  out << "Starting elements: <br>";
  for (int i = 0; i < theOps.size; i ++) {
    out << HtmlRoutines::getMathNoDisplay(theOps[i].toString(&format)) << "<br>";
  }
  bool success = MathRoutines::generateVectorSpaceClosedWithRespectToLieBracket(theOps, upperBound);
  if (!success) {
    out << "<br>Did not succeed with generating vector space, "
    << "instead got a vector space with basis "
    << theOps.size << " exceeding the limit. "
    << "The basis generated before exceeding the limit was: "
    << theOps.toString();
  } else {
    out << "<br>Lie bracket generates vector space of dimension "
    << theOps.size << " with basis:";
    for (int i = 0; i < theOps.size; i ++) {
      out << "<br>";
      if (theOps.size > 50) {
        out << theOps[i].toString(&format);
      } else {
        out << HtmlRoutines::getMathNoDisplay(theOps[i].toString(&format));
      }
    }
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::casimirWithRespectToLevi(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::casimir");
  RecursionDepthCounter recursionCounter(&calculator.recursionDepth);
  if (!input.isListNElements(3)) {
    return false;
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!CalculatorConversions::convert(
    calculator,
    input[1],
    algebra
  )) {
    return output.assignError(
      calculator,
      "Error extracting Lie algebra."
    );
  }
  SemisimpleLieAlgebra* semisimpleLieAlgebra = algebra.content;
  Vector<Rational> leviSelection;
  if (!calculator.getVector(input[2], leviSelection, nullptr, semisimpleLieAlgebra->getRank())) {
    return calculator << "<hr>Failed to extract parabolic selection. ";
  }
  Selection parabolicSelection;
  parabolicSelection = leviSelection;
  parabolicSelection.invertSelection();
  ElementUniversalEnveloping<RationalFraction<Rational> > theCasimir;
  theCasimir.makeCasimirWRTLeviParabolic(*semisimpleLieAlgebra, parabolicSelection);
  ExpressionContext contextE(calculator);
  contextE.setAmbientSemisimpleLieAlgebra(*semisimpleLieAlgebra);
  return output.assignValueWithContext(calculator, theCasimir, contextE);
}

template <class Type>
bool MathRoutines::generateVectorSpaceClosedWithRespectToOperation(
  List<Type>& inputOutputElts,
  int upperDimensionBound,
  void (*theBinaryOperation)(const Type& left, const Type& right, Type& output)
) {
  MacroRegisterFunctionWithName("MathRoutines::generateVectorSpaceClosedWithRespectToOperation");
  inputOutputElts[0].gaussianEliminationByRowsDeleteZeroRows(inputOutputElts);
  Type operationResult;
  ProgressReport report1(1, GlobalVariables::Response::ReportType::gaussianElimination);
  ProgressReport report2(20, GlobalVariables::Response::ReportType::gaussianElimination);
  if (report1.tickAndWantReport()) {
    report1.report("Extending vector space to closed with respect to binary operation. ");
  }
  List<Type> eltementsForGaussianElimination = inputOutputElts;
  for (int i = 0; i < inputOutputElts.size; i ++) {
    for (int j = i; j < inputOutputElts.size; j ++) {
      theBinaryOperation(inputOutputElts[i], inputOutputElts[j], operationResult);
      //int oldNumElts = inputOutputElts.size;
      eltementsForGaussianElimination.addOnTop(operationResult);
      eltementsForGaussianElimination[0].gaussianEliminationByRowsDeleteZeroRows(eltementsForGaussianElimination);
      if (eltementsForGaussianElimination.size > inputOutputElts.size) {
        inputOutputElts.addOnTop(operationResult);
      }
      if (upperDimensionBound > 0 && inputOutputElts.size > upperDimensionBound) {
        return false;
      }
      if (report2.tickAndWantReport()) {
        std::stringstream reportStream;
        reportStream << "Accounted operation between elements " << i + 1
        << " and " << j + 1 << " out of " << inputOutputElts.size;
        report2.report(reportStream.str());
      }
    }
  }
  inputOutputElts[0].gaussianEliminationByRowsDeleteZeroRows(inputOutputElts);
  return true;
}

bool CalculatorLieTheory::canBeExtendedParabolicallyTo(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::canBeExtendedParabolicallyTo");
  if (!input.isListNElements(3)) {
    return false;
  }
  DynkinType smallType, targetType;
  if (
    !CalculatorConversions::functionDynkinType(calculator, input[1], smallType) ||
    !CalculatorConversions::functionDynkinType(calculator, input[2], targetType)
  ) {
    return calculator << "Failed to convert arguments of " << input.toString() << " to two DynkinType's.";
  }
  return output.assignValue(calculator, static_cast<int>(smallType.canBeExtendedParabolicallyTo(targetType)));
}

bool CalculatorLieTheory::getSymmetricCartan(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::getSymmetricCartan");
  DynkinType dynkinType;
  if (!CalculatorConversions::dynkinType(calculator, input, dynkinType)) {
    return calculator << "Failed to convert " << input.toString() << " to DynkinType.";
  }
  std::stringstream out;
  Matrix<Rational> outputMat, outputCoMat;
  dynkinType.getCartanSymmetric(outputMat);
  dynkinType.getCoCartanSymmetric(outputCoMat);
  out << "Symmetric Cartan matrix: " << HtmlRoutines::getMathNoDisplay(outputMat.toStringLatex(), 10000)
  << "<br>Co-symmetric Cartan matrix: " << HtmlRoutines::getMathNoDisplay(outputCoMat.toStringLatex(), 10000);
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::drawWeightSupportWithMults(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (!input.isListNElements(3)) {
    return output.assignError(
      calculator,
      "Error: the function for drawing weight support takes two arguments (type and highest weight)"
    );
  }
  const Expression& typeNode = input[1];
  const Expression& hwNode = input[2];
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebraPointer;
  if (!CalculatorConversions::convert(
    calculator,
    typeNode,
    semisimpleLieAlgebraPointer
  )) {
    return output.assignError(
      calculator,
      "Error extracting Lie algebra."
    );
  }
  Vector<Rational> highestWeightFundCoords;
  ExpressionContext context(calculator);
  if (!calculator.getVector<Rational>(
    hwNode, highestWeightFundCoords, &context, semisimpleLieAlgebraPointer.content->getRank()
  )) {
    return output.assignError(
      calculator,
      "Failed to extract highest weight vector"
    );
  }
  Vector<Rational> highestWeightSimpleCoords;
  WeylGroupData& weylGroup = semisimpleLieAlgebraPointer.content->weylGroup;
  highestWeightSimpleCoords = weylGroup.getSimpleCoordinatesFromFundamental(highestWeightFundCoords);
  std::stringstream out;
  CharacterSemisimpleLieAlgebraModule<Rational> theChar;
  theChar.makeFromWeight(highestWeightSimpleCoords, semisimpleLieAlgebraPointer.content);
  DrawingVariables drawingVariables;
  std::string report;
  theChar.drawMeWithMultiplicities(report, drawingVariables, 10000);
  out << report << drawingVariables.getHTMLDiv(weylGroup.getDimension(), true);
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::drawRootSystem(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::drawRootSystem");
  if (input.size() < 2) {
    return calculator << "drawRootSystem expects at least 1 argument. ";
  }
  bool hasPreferredProjectionPlane = input.isListNElements(4);
  WithContext<SemisimpleLieAlgebra*> theAlgPointer;
  if (!CalculatorConversions::convert(
    calculator,
    input[1],
    theAlgPointer
  )) {
    return output.assignError(calculator, "Error extracting Lie algebra.");
  }
  SemisimpleLieAlgebra& theAlg = *theAlgPointer.content;
  WeylGroupData& weylGroup = theAlg.weylGroup;
  Vectors<Rational> preferredProjectionPlane;
  if (hasPreferredProjectionPlane) {
    preferredProjectionPlane.setSize(2);
    bool isGood = calculator.getVector(
      input[2],
      preferredProjectionPlane[0],
      nullptr,
      weylGroup.getDimension()
    ) && calculator.getVector(
      input[3],
      preferredProjectionPlane[1],
      nullptr,
      weylGroup.getDimension()
    );
    if (!isGood) {
      return output.assignError(
        calculator,
        "Failed to convert second or third argument to vector of desired dimension"
      );
    }
  }
  std::stringstream out;
  DrawingVariables drawingVariables;
  weylGroup.drawRootSystem(drawingVariables, true, false, nullptr, true, nullptr);
  if (hasPreferredProjectionPlane) {
    drawingVariables.flagFillUserDefinedProjection = true;
    drawingVariables.FillUserDefinedProjection = preferredProjectionPlane;
  }
  out << drawingVariables.getHTMLDiv(weylGroup.getDimension(), true);
  return output.assignValue(calculator, out.str());
}

template <class Coefficient>
int CharacterSemisimpleLieAlgebraModule<Coefficient>::getPositiveNStringSuchThatWeightMinusNAlphaIsWeight(
  const Weight<Coefficient>& weightInFundamentalCoordinates, const Vector<Coefficient>& alphaInFundamentalCoordinates
) {
  MacroRegisterFunctionWithName("CharacterSemisimpleLieAlgebraModule::getPositiveNStringSuchThatWeightMinusNAlphaIsWeight");
  int result = - 1;
  Weight<Coefficient> currentWeight;
  currentWeight = weightInFundamentalCoordinates;
  for (
    ;
    this->monomials.contains(currentWeight);
    result ++, currentWeight.weightFundamentalCoordinates -= alphaInFundamentalCoordinates
  ) {
  }
  return result;
}

template <class Coefficient>
std::string CharacterSemisimpleLieAlgebraModule<Coefficient>::toStringFullCharacterWeightsTable() {
  MacroRegisterFunctionWithName("CharacterSemisimpleLieAlgebraModule::toStringFullCharacterWeightsTable");
  std::stringstream out;
  CharacterSemisimpleLieAlgebraModule<Coefficient> outputChar;
  if (!this->freudenthalEvaluateMeFullCharacter(outputChar, 10000, nullptr)) {
    out << "Failed to compute the character with highest weight " << this->toString()
    << " I used Fredenthal's formula; likely the computation was too large. ";
    return out.str();
  }
  out << "<table><tr><td>Weight in fund. coords</td><td>simple coords.</td>"
  << "<td>Simple strings</td><td>Simple half-strings</td></tr>";
  Vector<Coefficient> outputSimpleStringCoords, outputSimpleHalfStringCoords;
  Vector<Coefficient> simpleRoot;
  Vector<Coefficient> simpleRootFundamentalCoordinates;
  for (int k = 0; k < outputChar.size(); k ++) {
    out << "<tr>";
    out << "<td>" << outputChar[k].weightFundamentalCoordinates.toString() << "</td>";
    Vector<Coefficient> weightSimple = this->getOwner()->weylGroup.getSimpleCoordinatesFromFundamental(
      outputChar[k].weightFundamentalCoordinates, Coefficient::zero()
    );
    out << "<td>" << weightSimple.toString() << "</td>";
    outputSimpleStringCoords.makeZero(this->getOwner()->getRank());
    outputSimpleHalfStringCoords.makeZero(this->getOwner()->getRank());
    for (int j = 0; j < this->getOwner()->getRank(); j ++) {
      simpleRoot.makeEi(this->getOwner()->getRank(), j);
      simpleRootFundamentalCoordinates =
      this->getOwner()->weylGroup.getFundamentalCoordinatesFromSimple(simpleRoot);
      outputSimpleStringCoords[j] = outputChar.getPositiveNStringSuchThatWeightMinusNAlphaIsWeight(
        outputChar[k], simpleRootFundamentalCoordinates
      ) - outputChar.getPositiveNStringSuchThatWeightMinusNAlphaIsWeight(
        outputChar[k], - simpleRootFundamentalCoordinates
      );
      outputSimpleHalfStringCoords[j] = outputChar.getPositiveNStringSuchThatWeightMinusNAlphaIsWeight(
        outputChar[k], simpleRootFundamentalCoordinates
      );
    }
    if (outputSimpleStringCoords != outputChar[k].weightFundamentalCoordinates) {
      out << "<td><b style =\"color:red\">" << outputSimpleStringCoords.toString() << "</b></td>" ;
    } else {
      out << "<td>" << outputSimpleStringCoords.toString() << "</td>";
    }
    if (outputSimpleHalfStringCoords != outputChar[k].weightFundamentalCoordinates) {
      out << "<td><b style =\"color:red\">"
      << outputSimpleHalfStringCoords.toString() << "</b></td>" ;
    } else {
      out << "<td>" << outputSimpleHalfStringCoords.toString() << "</td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

bool CalculatorLieTheory::drawWeightSupport(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::drawWeightSupport");
  if (!input.isListNElements(3)) {
    return output.assignError(
      calculator,
      "Wrong number of arguments, must be 2. "
    );
  }
  const Expression& typeNode = input[1];
  const Expression& hwNode = input[2];
  WithContext<SemisimpleLieAlgebra*> theAlgPointer;
  if (!CalculatorConversions::convert(
    calculator,
    typeNode,
    theAlgPointer
  )) {
    return output.assignError(
      calculator,
      "Error extracting Lie algebra."
    );
  }
  SemisimpleLieAlgebra& theAlg = *theAlgPointer.content;
  Vector<Rational> highestWeightFundCoords;
  if (!calculator.getVector<Rational>(
    hwNode,
    highestWeightFundCoords,
    nullptr,
    theAlg.getRank()
  )) {
    return false;
  }
  Vector<Rational> highestWeightSimpleCoords;
  WeylGroupData& weylGroup = theAlg.weylGroup;
  highestWeightSimpleCoords = weylGroup.getSimpleCoordinatesFromFundamental(highestWeightFundCoords, Rational::zero());
  // Vectors<Rational> weightsToBeDrawn;
  std::stringstream out;
  CharacterSemisimpleLieAlgebraModule<Rational> theChar;
  theChar.makeFromWeight(highestWeightSimpleCoords, theAlgPointer.content);
  DrawingVariables drawingVariables;
  std::string report;
  theChar.drawMeNoMultiplicities(report, drawingVariables, 10000);
  out << report << drawingVariables.getHTMLDiv(weylGroup.getDimension(), true);
  out << "<br>A table with the weights of the character follows. <br>";
  out << theChar.toStringFullCharacterWeightsTable();
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::getLinksToSimpleLieAlgebras(
  Calculator& calculator, const Expression& input, Expression& output
) {
  (void) input;
  std::stringstream outFromHD, outRecomputeLinks;

  outFromHD << "\n\n<p>\n\n<table>"
  << "<tr><td>Structure constants</td>"
  << "<td>Semisimple subalgebras</td> "
  << "<td>sl(2) subalgebras</td><td>root subalgebras</td> </tr>\n";
  List<DynkinType> precomputedTypes;
  DynkinType::getPrecomputedDynkinTypes(precomputedTypes);
  for (int i = 0; i < precomputedTypes.size; i ++) {
    outFromHD << calculator.toStringSemismipleLieAlgebraLinksFromHardDrive("", precomputedTypes[i]);
    if (precomputedTypes[i].hasPrecomputedSubalgebras()) {
      std::stringstream recomputeCommand;
      recomputeCommand << "PrintSemisimpleSubalgebrasRecompute{}("
      << precomputedTypes[i].toString() << ")";
      outRecomputeLinks << "<br>"
      << HtmlRoutines::getCalculatorComputationAnchorSameURL(recomputeCommand.str(), "");
    }
  }
  outFromHD << "</table></p>";
  std::string fileName = "semisimple_lie_algebra_structure.html";
  std::stringstream out;
  out
  << calculator.writeFileToOutputFolderReturnLink(outFromHD.str(), fileName, "Links file");
  out << "<br>Recompute links.";
  out << outRecomputeLinks.str();
  out << outFromHD.str();
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::printSemisimpleSubalgebrasNilradicals(Calculator& calculator, const Expression& input, Expression& output) {
  return CalculatorLieTheory::printSemisimpleSubalgebras(calculator, input, output, true, true, true, true, true, true);
}

bool CalculatorLieTheory::printSemisimpleSubalgebrasRecompute(Calculator& calculator, const Expression& input, Expression& output) {
  return CalculatorLieTheory::printSemisimpleSubalgebras(calculator, input, output, true, true, false, true, false, true);
}

bool CalculatorLieTheory::printSemisimpleSubalgebrasNoSolutions(Calculator& calculator, const Expression& input, Expression& output) {
  return CalculatorLieTheory::printSemisimpleSubalgebras(calculator, input, output, true, false, false, false, false, false);
}

bool CalculatorLieTheory::printSemisimpleSubalgebrasNoCentralizers(Calculator& calculator, const Expression& input, Expression& output) {
  return CalculatorLieTheory::printSemisimpleSubalgebras(calculator, input, output, true, true, false, true, false, false);
}

bool CalculatorLieTheory::printSemisimpleSubalgebrasRegular(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return CalculatorLieTheory::printSemisimpleSubalgebras(calculator, input, output, false, true, false, true, false, true);
}

bool CalculatorLieTheory::printSemisimpleSubalgebras(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool doForceRecompute,
  bool doAttemptToSolveSystems,
  bool docomputePairingTable,
  bool docomputeModuleDecompositionsition,
  bool doComputeNilradicals,
  bool doAdjustCentralizers
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::printSemisimpleSubalgebras");
  if (doForceRecompute) {
    if (!global.userDefaultHasAdminRights()) {
      return calculator
      << "Only logged-in admins allowed to "
      << "force-recompute semisimple subalgebras. ";
    }
  }
  if (global.response.monitoringAllowed()) {
    global.response.initiate("Triggered by printSemisimpleSubalgebras.");
  }
  if (input.size() != 2) {
    return calculator << "Semisimple Lie algebra expects a single argument. ";
  }
  std::stringstream out;
  WithContext<SemisimpleLieAlgebra*> ownerAlgebra;
  ownerAlgebra.content = nullptr;
  SemisimpleLieAlgebra* ownerAlgebraPointer = nullptr;
  bool isAlreadySubalgebrasObject = input[1].isOfType<SemisimpleSubalgebras>();
  int maximumRank = 8;
  if (global.userDefaultHasAdminRights()) {
    maximumRank = 12;
  }
  if (!isAlreadySubalgebrasObject) {
    if (!CalculatorConversions::convert(
      calculator,
      input[1],
      ownerAlgebra
    )) {
      return output.assignError(
        calculator,
        "Error extracting Lie algebra."
      );
    }
    ownerAlgebraPointer = ownerAlgebra.content;
    if (ownerAlgebraPointer == nullptr) {
      global.fatal
      << "Zero pointer to semisimple Lie algebra: this shouldn't happen. "
      << global.fatal;
    }
    if (ownerAlgebraPointer->getRank() > maximumRank) {
      out << "<b>This code is has been set to run up to rank "
      << maximumRank << ". "
      << "As soon as the algorithms are mature enough, "
      << "higher ranks will be allowed. </b>";
      return output.assignValue(calculator, out.str());
    }
  } else {
    ownerAlgebraPointer = input[1].getValue<SemisimpleSubalgebras>().owner;
  }
  if (ownerAlgebraPointer == nullptr) {
    global.fatal
    << "Zero pointer to semisimple Lie algebra: this shouldn't happen. "
    << global.fatal;
  }
  SemisimpleLieAlgebra& ownerLieAlgebra = *ownerAlgebraPointer;
  std::string dynkinString = ownerAlgebraPointer->weylGroup.dynkinType.toString();
  global.relativePhysicalNameOptionalProgressReport = "progress_subalgebras_" + dynkinString;
  global.relativePhysicalNameOptionalResult = "result_subalgebras_" + dynkinString;
  SemisimpleSubalgebras& subalgebras =
  calculator.objectContainer.getSemisimpleSubalgebrasCreateIfNotPresent(ownerLieAlgebra.weylGroup.dynkinType);
  if (subalgebras.owner == nullptr) {
    subalgebras.owner = &ownerLieAlgebra;
  }
  Plot plot;
  subalgebras.owner->weylGroup.dynkinType.plot(plot);
  subalgebras.computeStructureWriteFiles(
    ownerLieAlgebra,
    calculator.objectContainer.algebraicClosure,
    calculator.objectContainer.semisimpleLieAlgebras,
    calculator.objectContainer.slTwoSubalgebras,
    CalculatorConversions::stringFromSemisimpleSubalgebras,
    &out,
    doForceRecompute,
    !isAlreadySubalgebrasObject,
    doComputeNilradicals,
    docomputeModuleDecompositionsition,
    doAttemptToSolveSystems,
    docomputePairingTable,
    doAdjustCentralizers,
    plot.getPlotHtml(calculator)
  );
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::casimir(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::casimir");
  if (input.size() != 2) {
    return calculator << "Casimir function expects a single input. ";
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!CalculatorConversions::convert(
    calculator,
    input[1],
    algebra
  )) {
    return output.assignError(
      calculator,
      "Error extracting Lie algebra."
    );
  }
  SemisimpleLieAlgebra& algebraReference = *algebra.content;
  ElementUniversalEnveloping<RationalFraction<Rational> > theCasimir;
  theCasimir.makeCasimir(algebraReference);
  calculator << "Context Lie algebra: " << algebraReference.weylGroup.dynkinType.toString()
  << ". The coefficient: " << algebraReference.weylGroup.getKillingDividedByTraceRatio().toString()
  <<  ". The Casimir element of the ambient Lie algebra. ";
  ExpressionContext context(calculator);
  context.setAmbientSemisimpleLieAlgebra(algebraReference);
  return output.assignValueWithContext(calculator, theCasimir, context);
}

bool CalculatorLieTheory::embedG2InB3(Calculator& calculator, const Expression& input, Expression& output) {
  if (input.size() != 2) {
    return false;
  }
  output = input[1];
  if (!output.isOfType < ElementUniversalEnveloping<RationalFraction<Rational> > >()) {
    return output.assignError(
      calculator,
      "Failed to convert argument to element of the Universal enveloping algebra. "
    );
  }
  SemisimpleLieAlgebra& ownerSemisimple = *output.getAmbientSemisimpleLieAlgebraNonConstUseWithCaution();
  if (!ownerSemisimple.isOfSimpleType('G', 2)) {
    return output.assignError(
      calculator,
      "Error: embedding of G_2 in B_3 takes elements of U(G_2) as arguments."
    );
  }
  HomomorphismSemisimpleLieAlgebra homomorphism;
  calculator.makeHmmG2InB3(homomorphism);

  ElementUniversalEnveloping<RationalFraction<Rational> > argument = output.getValue<ElementUniversalEnveloping<RationalFraction<Rational> > >();
  ElementUniversalEnveloping<RationalFraction<Rational> > outputUE;
  if (!homomorphism.applyHomomorphism(argument, outputUE)) {
    return output.assignError(
      calculator,
      "Failed to apply homomorphism for unspecified reason"
    );
  }
  outputUE.simplify(RationalFraction<Rational>::oneRational());
  ExpressionContext context(calculator);
  context.setAmbientSemisimpleLieAlgebra(homomorphism.coDomainAlgebra());
  return output.assignValueWithContext(calculator, outputUE, context);
}

bool CalculatorLieTheory::characterSemisimpleLieAlgebraFiniteDimensional(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::characterSemisimpleLieAlgebraFiniteDimensional");
  Vector<Rational> highestWeight;
  Selection parSel;
  WithContext<SemisimpleLieAlgebra*> ownerSSLiealg;
  Expression tempE, tempE2;
  if (!calculator.getTypeHighestWeightParabolic(
    calculator,
    input,
    output,
    highestWeight,
    parSel,
    ownerSSLiealg
  )) {
    return false;
  }
  if (output.isError()) {
    return true;
  }
  if (parSel.cardinalitySelection != 0) {
    return output.assignError(
      calculator,
      "I know only to compute with finite "
      "dimensional characters, for the time being. "
    );
  }
  CharacterSemisimpleLieAlgebraModule<Rational> element;
  element.makeFromWeight(
    ownerSSLiealg.content->weylGroup.getSimpleCoordinatesFromFundamental(highestWeight),
    ownerSSLiealg.content
  );
  return output.assignValue(calculator, element);
}

bool CalculatorLieTheory::chevalleyGenerator(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::chevalleyGenerator");
  if (input.size() != 3) {
    return false;
  }
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebra;
  if (!CalculatorConversions::convert(
    calculator,
    input[1],
    semisimpleLieAlgebra
  )) {
    return output.assignError(calculator, "Error extracting Lie algebra.");
  }
  int generatorIndex = - 1;
  if (!input[2].isSmallInteger(&generatorIndex)) {
    return false;
  }
  if (
    generatorIndex > semisimpleLieAlgebra.content->getNumberOfPositiveRoots() ||
    generatorIndex == 0 ||
    generatorIndex < - semisimpleLieAlgebra.content->getNumberOfPositiveRoots()
  ) {
    return output.assignError(calculator, "Bad Chevalley-Weyl generator index.");
  }
  ElementSemisimpleLieAlgebra<Rational> element;
  if (generatorIndex > 0) {
    generatorIndex += semisimpleLieAlgebra.content->getRank() - 1;
  }
  generatorIndex += semisimpleLieAlgebra.content->getNumberOfPositiveRoots();
  element.makeGenerator(generatorIndex, *semisimpleLieAlgebra.content);
  ElementUniversalEnveloping<RationalFraction<Rational> > elementOverRationalFunctions;
  elementOverRationalFunctions.assignElementLieAlgebra(element, *semisimpleLieAlgebra.content);
  ExpressionContext context(calculator);
  int indexInOwner = calculator.objectContainer.semisimpleLieAlgebras.getIndex(
    semisimpleLieAlgebra.content->weylGroup.dynkinType
  );
  context.setIndexAmbientSemisimpleLieAlgebra(indexInOwner);
  return output.assignValueWithContext(calculator, elementOverRationalFunctions, context);
}

bool CalculatorLieTheory::cartanGenerator(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::cartanGenerator");
  if (input.size() != 3) {
    return false;
  }
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebra;
  if (!CalculatorConversions::convert(
    calculator,
    input[1],
    semisimpleLieAlgebra
  )) {
    return output.assignError(calculator, "Error extracting Lie algebra.");
  }
  if (semisimpleLieAlgebra.content == nullptr) {
    global.fatal << "Called conversion function successfully, "
    << "but the output is a zero pointer to a semisimple Lie algebra. "
    << global.fatal;
  }
  int index;
  if (!input[2].isSmallInteger(&index)) {
    return false;
  }
  if (
    index == 0 ||
    index > semisimpleLieAlgebra.content->getNumberOfPositiveRoots() ||
    index < - semisimpleLieAlgebra.content->getNumberOfPositiveRoots()
  ) {
    return output.assignError(calculator, "Bad Cartan subalgebra generator index.");
  }
  ElementSemisimpleLieAlgebra<Rational> element;
  Vector<Rational> theH = semisimpleLieAlgebra.content->weylGroup.rootSystem[semisimpleLieAlgebra.content->getRootIndexFromDisplayIndex(index)];
  element.makeCartanGenerator(theH, *semisimpleLieAlgebra.content);
  ElementUniversalEnveloping<RationalFraction<Rational> > theUE;
  theUE.assignElementLieAlgebra(element, *semisimpleLieAlgebra.content);
  ExpressionContext context(calculator);
  int theAlgIndex = calculator.objectContainer.semisimpleLieAlgebras.getIndex(semisimpleLieAlgebra.content->weylGroup.dynkinType);
  context.setIndexAmbientSemisimpleLieAlgebra(theAlgIndex);
  return output.assignValueWithContext(calculator, theUE, context);
}

bool CalculatorLieTheory::rootSubsystem(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::rootSubsystem");
  if (input.size() < 3) {
    return false;
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!CalculatorConversions::convert(
    calculator,
    input[1],
    algebra
  )) {
    return output.assignError(calculator, "Error extracting Lie algebra.");
  }
  SemisimpleLieAlgebra* semisimpleLieAlgebra = algebra.content;
  int rank = semisimpleLieAlgebra->getRank();
  Vector<Rational> currentRoot;
  Vectors<Rational> outputRoots;
  WeylGroupData& weylGroup = semisimpleLieAlgebra->weylGroup;
  if (!weylGroup.dynkinType.isSimple()) {
    return calculator << "<hr>Function root subsystem works for simple ambient types only. ";
  }
  for (int i = 2; i < input.size(); i ++) {
    if (!calculator.getVector(input[i], currentRoot, nullptr, rank)) {
      return false;
    }
    if (!weylGroup.rootSystem.contains(currentRoot)) {
      return output.assignError(calculator, "Input vector " + currentRoot.toString() + " is not a root. ");
    }
    outputRoots.addOnTop(currentRoot);
  }
  std::stringstream out;
  DynkinDiagramRootSubalgebra diagram;
  weylGroup.transformToSimpleBasisGenerators(outputRoots, weylGroup.rootSystem);
  diagram.ambientBilinearForm = weylGroup.cartanSymmetric;
  diagram.ambientRootSystem = weylGroup.rootSystem;
  diagram.computeDiagramInputIsSimple(outputRoots);
  out << "Diagram final: " << diagram.toString()
  << ". Simple basis: " << diagram.simpleBasesConnectedComponents.toString();
  return output.assignValue(calculator, out.str());
}

bool CalculatorLieTheory::printSemisimpleLieAlgebra(
  Calculator& calculator, const Expression& input, Expression& output, bool verbose
) {
  return CalculatorLieTheory::writeToHardDiskOrPrintSemisimpleLieAlgebra(calculator, input, output, verbose, false);
}

bool CalculatorLieTheory::writeSemisimpleLieAlgebraToHardDisk(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::writeSemisimpleLieAlgebraToHardDisk");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!global.userDefaultHasAdminRights() && !global.flagRunningConsoleTest) {
    return output.assignError(
      calculator,
      "Caching structure constants to HD available to admins only. "
    );
  }
  return CalculatorLieTheory::writeToHardDiskOrPrintSemisimpleLieAlgebra(calculator, input, output, true, true);
}

bool CalculatorLieTheory::writeToHardDiskOrPrintSemisimpleLieAlgebra(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool verbose,
  bool writeToHD
) {
  MacroRegisterFunctionWithName("Calculator::writeToHardDiskOrPrintSemisimpleLieAlgebra");
  if (input.size() != 2) {
    return calculator << "Print semisimple Lie algebras expects 1 argument. ";
  }
  return CalculatorLieTheory::functionWriteToHardDiskOrPrintSemisimpleLieAlgebra(
    calculator, input[1], output, verbose, writeToHD
  );
}

bool CalculatorLieTheory::functionWriteToHardDiskOrPrintSemisimpleLieAlgebra(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool verbose,
  bool writeToHD
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::functionWriteToHardDiskOrPrintSemisimpleLieAlgebra");
  WithContext<SemisimpleLieAlgebra*> algebraPointer;
  input.checkInitialization();
  if (!CalculatorConversions::convert(
    calculator,
    input,
    algebraPointer
  )) {
    calculator << "Failed to extract Lie algebra from: " << input.toString() << "<br>";
    return output.assignError(
      calculator,
      "Error extracting Lie algebra."
    );
  }
  algebraPointer.content->checkConsistency();
  algebraPointer.context.checkInitialization();
  SemisimpleLieAlgebra& semisimpleAlgebra = *algebraPointer.content;
  std::stringstream out;
  Plot plot;
  semisimpleAlgebra.weylGroup.dynkinType.plot(plot);
  std::string plotHTML = plot.getPlotHtml2d(calculator);
  if (writeToHD) {
    semisimpleAlgebra.writeHTML(verbose, calculator.flagWriteLatexPlots, plotHTML);
    out << "<a href='"
    << semisimpleAlgebra.fileNames.virtualFileNameWithPathStructureConstants()
    << "' target='_blank'>hard drive output</a><br>";
  }
  out << semisimpleAlgebra.toHTML(verbose, calculator.flagWriteLatexPlots, plotHTML);
  return output.assignValue(calculator, out.str());
}
