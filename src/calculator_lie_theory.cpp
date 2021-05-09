#include "calculator_lie_theory.h"
#include "calculator_inner_typed_functions.h"
#include "math_extra_differential_operators.h"
#include "math_extra_modules_semisimple_lie_algebras_implementation.h"
#include "math_extra_semisimple_lie_algebras_implementation.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_general_implementation.h"
#include "math_extra_weyl_algebras_implementation.h"
#include "math_rational_function_implementation.h"

bool CalculatorLieTheory::writeGenVermaModAsDiffOperatorInner(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  Vectors<Polynomial<Rational> >& theHws,
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
  if (theHws.size == 0) {
    return false;
  }
  SemisimpleLieAlgebra& theSSalgebra = *owner;
  List<ElementUniversalEnveloping<Polynomial<Rational> > > elementsNegativeNilrad;
  ElementSemisimpleLieAlgebra<Rational> theGenerator;
  ElementUniversalEnveloping<Polynomial<Rational> > genericElt, actionOnGenericElt;
  List<QuasiDifferentialOperator<Rational> > theQDOs;
  FormatExpressions theWeylFormat, theUEformat;
  std::stringstream out, latexReport, latexReport2;
  Polynomial<Rational> zero;
  Polynomial<Rational> one;
  one.makeConstant(Rational::one());
  theWeylFormat.maximumLineLength = 40;
  theWeylFormat.flagUseLatex = true;
  theUEformat.maximumLineLength = 20;
  theUEformat.flagUseLatex = true;
  theUEformat.chevalleyGgeneratorLetter = "g";
  theUEformat.chevalleyHgeneratorLetter = "h";
  hwContext.getFormat(theUEformat);
  theUEformat.polyDefaultLetter = exponentVariableLetter == nullptr  ? "a" : *exponentVariableLetter;
  theUEformat.maximumLineLength = 178;
  theUEformat.numberOfAmpersandsPerNewLineForLaTeX = 2;
  theWeylFormat.numberOfAmpersandsPerNewLineForLaTeX = 2;
  hwContext.getFormat(theWeylFormat);
  List<ElementSemisimpleLieAlgebra<Rational> > theGeneratorsItry;
  if (!AllGenerators) {
    for (int j = 0; j < theSSalgebra.getRank(); j ++) {
      Vector<Rational> ei;
      ei.makeEi(theSSalgebra.getRank(), j);
      theGenerator.makeGGenerator(ei, theSSalgebra);
      theGeneratorsItry.addOnTop(theGenerator);
      ei.negate();
      theGenerator.makeGGenerator(ei, theSSalgebra);
      theGeneratorsItry.addOnTop(theGenerator);
    }
  } else {
    for (int j = 0; j < theSSalgebra.getNumberOfGenerators(); j ++) {
      theGenerator.makeGenerator(j, theSSalgebra);
      theGeneratorsItry.addOnTop(theGenerator);
    }
  }
  theQDOs.setSize(theGeneratorsItry.size);
  out << "<table border =\"1\">";
  latexReport << "\\begin{longtable}{rll";
  for (int i = 0; i < theGeneratorsItry.size; i ++) {
    latexReport << "l";
  }
  latexReport << "}\\caption{\\label{tableDiffOps" << selInducing.toString()
  << "} Differential operators corresponding to actions"
  << " of simple positive generators for the " << selInducing.toString() << "-parabolic subalgebra.}\\\\<br>";
  List<ModuleSSalgebra<RationalFraction<Rational> > > theMods;
  theMods.setSize(theHws.size);
  Vector<RationalFraction<Rational> > tempV;
  int numStartingVars = hwContext.numberOfVariables();
  std::stringstream reportfourierTransformedCalculatorCommands, reportCalculatorCommands;
  long long totalAdditions = 0;
  long long currentAdditions = 0;
  long long totalMultiplications = 0;
  long long currentMultiplications = 0;
  double totalTime = 0, currentTime = 0;
  for (int i = 0; i < theHws.size; i ++) {
    ModuleSSalgebra<RationalFraction<Rational> >& theMod = theMods[i];
    tempV = theHws[i];
    if (!theMod.makeFromHW(
      theSSalgebra,
      tempV,
      selInducing,
      RationalFraction<Rational>(Rational::one()),
      RationalFraction<Rational>(Rational::zero()),
      nullptr,
      true
    )) {
      return output.makeError("Failed to create module.", calculator);
    }
    if (i == 0) {
      theMod.getElementsNilradical(elementsNegativeNilrad, true, nullptr, useNilWeight, ascending);
      Polynomial<Rational> Pone, Pzero;
      Pone.makeOne();
      Pzero.makeZero();
      theMod.getGenericUnMinusElt(true, genericElt, useNilWeight, ascending);
      theWeylFormat.polynomialAlphabet.setSize(numStartingVars + elementsNegativeNilrad.size);
      theWeylFormat.weylAlgebraLetters.setSize(numStartingVars + elementsNegativeNilrad.size);
      theUEformat.polynomialAlphabet.setSize(numStartingVars + elementsNegativeNilrad.size);
      for (int k = 0; k < numStartingVars; k ++) {
        theWeylFormat.weylAlgebraLetters[k] = "error";
      }
      std::string theFinalXletter = (xLetter == nullptr) ? "x": *xLetter;
      std::string theFinalPartialLetter = (partialLetter == nullptr) ? "\\partial" : *partialLetter;
      for (int k = numStartingVars; k < theUEformat.polynomialAlphabet.size; k ++) {
        std::stringstream tmpStream, tempstream2, tempstream3, tempStream4;
        tmpStream << theUEformat.polyDefaultLetter << "_{" << k - hwContext.numberOfVariables() + 1 << "}";
        theUEformat.polynomialAlphabet[k] = tmpStream.str();
        tempstream2 << theFinalXletter << "_{" << k-numStartingVars + 1 << "}";
        tempstream3 << theFinalXletter << "_" << k-numStartingVars + 1;
        tempStream4 << theFinalPartialLetter << "_{" << k-numStartingVars + 1 << "}";
        if (
          theWeylFormat.polynomialAlphabet.contains(tempstream2.str()) ||
          theWeylFormat.polynomialAlphabet.contains(tempstream3.str())
        ) {
          return output.makeError(
            "Error: the variable " +
            tempstream2.str() +
            " is reserved for me: you are not allowed to use it as a coordinate of the highest weight. ",
            calculator
          );
        }
        theWeylFormat.polynomialAlphabet[k] = tempstream2.str();
        theWeylFormat.weylAlgebraLetters[k] = tempStream4.str();
      }
      out << "<tr><td>General monomial in U(n_-):</td><td>"
      << HtmlRoutines::getMathNoDisplay(genericElt.toString(&theUEformat)) << "</td> </tr>";
      latexReport << "& \\multicolumn{" << theGeneratorsItry.size << "}{c}{Element acting}\\\\<br>\n ";
      latexReport << "Action on ";
      out << "<tr><td></td><td colspan =\"" << theGeneratorsItry.size << "\"> Element acting</td></td></tr>";
      out << "<tr><td>Action on</td>";
      for (int j = 0; j < theGeneratorsItry.size; j ++) {
        out << "<td>" << theGeneratorsItry[j].toString(&theUEformat) << "</td>";
        latexReport << "& $" << theGeneratorsItry[j].toString(&theUEformat)  << "$";
      }
      latexReport << "\\endhead \\hline<br>";
      out << "</tr>";
      out << "<tr><td>" << HtmlRoutines::getMathNoDisplay(genericElt.toString(&theUEformat)) << "</td>";
      latexReport << "$" << genericElt.toString(&theUEformat) << "$";
      for (int j = 0; j < theGeneratorsItry.size; j ++) {
        actionOnGenericElt.assignElementLieAlgebra(theGeneratorsItry[j], theSSalgebra, Pone);
        actionOnGenericElt *= genericElt;
        theSSalgebra.orderNilradical(theMod.parabolicSelectionNonSelectedAreElementsLevi, useNilWeight, ascending);
        actionOnGenericElt.simplify(one);
        theUEformat.numberOfAmpersandsPerNewLineForLaTeX = 2;
        out << "<td>" << HtmlRoutines::getMathNoDisplay("\\begin{array}{rcl}&&" + actionOnGenericElt.toString(&theUEformat) + "\\end{array}") << "</td>";
        theUEformat.numberOfAmpersandsPerNewLineForLaTeX = 0;
        latexReport << "& $\\begin{array}{l} " << actionOnGenericElt.toString(&theUEformat) << "\\end{array}$ ";
      }
      latexReport << "\\\\ \\hline\\hline<br>";
      out << "</tr>";
    }
    out << "<tr><td>" << HtmlRoutines::getMathNoDisplay(theMod.theChaR.toString()) << "</td>";
    latexReport2 << "\\begin{longtable}{rll}";
    latexReport2 << "$\\gog$& $n$& element of $\\mathbb W_n$ \\\\\\hline" << "\\multirow{" << theGeneratorsItry.size
    << "}{*}{$" << theSSalgebra.toStringLieAlgebraName() << "$}" << " &  \\multirow{"  << theGeneratorsItry.size << "}{*}{"
    << elementsNegativeNilrad.size << "}&";

    latexReport << "$\\begin{array}{r}" << theMod.theChaR.toString()
    << "(\\mathfrak{l}) \\\\ \\\\dim:~" << theMod.getDimension() << " \\end{array}$";
    for (int j = 0; j < theGeneratorsItry.size; j ++) {
      theGenerator = theGeneratorsItry[j];
      currentTime = global.getElapsedSeconds();
      currentAdditions = Rational::totalAdditions();
      currentMultiplications = Rational::totalMultiplications();
      theMod.getActionGeneralizedVermaModuleAsDifferentialOperator(
        theGenerator, theQDOs[j], useNilWeight, ascending
      );
      totalAdditions += Rational::totalAdditions() - currentAdditions;
      totalMultiplications += Rational::totalMultiplications() - currentMultiplications;
      totalTime += global.getElapsedSeconds() - currentTime;
      theWeylFormat.customCoefficientMonomialSeparator = "\\otimes ";
      theWeylFormat.numberOfAmpersandsPerNewLineForLaTeX = 2;
      out << "<td>" << HtmlRoutines::getMathNoDisplay(
        "\\begin{array}{|r|c|l|}&&" + theQDOs[j].toString(&theWeylFormat) + "\\end{array}"
      ) << "</td>";
      theWeylFormat.numberOfAmpersandsPerNewLineForLaTeX = 0;
      theWeylFormat.maximumLineLength = 300;
      latexReport << " & $\\begin{array}{l}" << theQDOs[j].toString(&theWeylFormat) << "\\end{array}$";
      if (j != 0) {
        latexReport2 << "&&";
      }
      latexReport2 << " $\\begin{array}{l}" << theQDOs[j].toString(&theWeylFormat) << "\\end{array}$\\\\ "
      << (j != theGeneratorsItry.size - 1 ? "\\cline{3-3}" : "\\hline" ) << "\n<br>";
      theWeylFormat.customCoefficientMonomialSeparator = "";
    }
    latexReport2 << "\\end{longtable}";
    latexReport << "\\\\\\hline<br>";
    out << "</tr>";
    if (theMod.getDimension() == 1) {
      ElementWeylAlgebra<Rational> diffOpPart, transformedDO;
      reportfourierTransformedCalculatorCommands << "<hr>" << HtmlRoutines::getMathNoDisplay(theMod.theChaR.toString())
      << ", differential operators Fourier transformed - formatted for calculator input. <br><br>";
      reportfourierTransformedCalculatorCommands << "x_{{i}}= ElementWeylAlgebraPoly{}(\\partial_i, x_i);\n<br>"
      << "\\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n";
      reportCalculatorCommands << "<hr>" << HtmlRoutines::getMathNoDisplay(theMod.theChaR.toString())
      << ", differential operators - formatted for calculator input. <br><br>";
      reportCalculatorCommands << "x_{{i}}= ElementWeylAlgebraPoly{}(\\partial_i, x_i);\n<br>"
      << "\\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n";

      for (int j = 0; j < theGeneratorsItry.size; j ++) {
        theQDOs[j].getElementWeylAlgebraSetMatrixPartsToId(diffOpPart);
        diffOpPart.fourierTransform(transformedDO);
        reportfourierTransformedCalculatorCommands << "<br>"
        << theGeneratorsItry[j].toString() << "=" << transformedDO.toString() << ";";
        reportCalculatorCommands << "<br>" << theGeneratorsItry[j].toString() << "=" << diffOpPart.toString() << ";";
      }
      reportfourierTransformedCalculatorCommands << "<br>generateVectorSpaceClosedWithRespectToLieBracket{}(248," ;
      reportCalculatorCommands << "<br>generateVectorSpaceClosedWithRespectToLieBracket{}(248," ;
      for (int j = 0; j < theGeneratorsItry.size; j ++) {
        reportfourierTransformedCalculatorCommands << theGeneratorsItry[j].toString();
        reportCalculatorCommands << theGeneratorsItry[j].toString();
        if (j != theGeneratorsItry.size - 1) {
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
  return output.assignValue<std::string>(out.str(), calculator);
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
  RecursionDepthCounter therecursionIncrementer(&calculator.recursionDepth);
  RationalFraction<Rational> RFOne, RFZero;
  RFOne.makeOne();
  RFZero.makeZero();
  std::string report;
  ElementTensorsGeneralizedVermas<RationalFraction<Rational> > theElt;
  //= theElementData.theElementTensorGenVermas.getElement();
  ListReferences<ModuleSSalgebra<RationalFraction<Rational> > >& theMods = calculator.objectContainer.theCategoryOmodules;
  int indexOfModule = - 1;

  for (int i = 0; i < theMods.size; i ++) {
    ModuleSSalgebra<RationalFraction<Rational> >& currentMod = theMods[i];
    if (
      highestWeightFundCoords == currentMod.theHWFundamentalCoordsBaseField &&
      selectionParSel == currentMod.parabolicSelectionNonSelectedAreElementsLevi &&
      currentMod.owner == owner
    ) {
      indexOfModule = i;
      break;
    }
  }
  if (indexOfModule == - 1) {
    indexOfModule = theMods.size;
    theMods.setSize(theMods.size + 1);
    theMods.lastObject().reset();
  }
  ModuleSSalgebra<RationalFraction<Rational> >& theMod = theMods[indexOfModule];
  if (!theMod.flagIsInitialized) {
    bool isGood = theMod.makeFromHW(*owner, highestWeightFundCoords, selectionParSel, RFOne, RFZero, &report);
    if (Verbose) {
      calculator << theMod.toString();
    }
    if (!isGood) {
      return output.makeError("Error while generating highest weight module. See comments for details. ", calculator);
    }
  }
  if (&theMod.getOwner() != owner) {
    global.fatal << "Module has owner that is not what it should be. " << global.fatal;
  }
  theElt.makeHWV(theMod, RFOne);
  if (&theElt.getOwnerSemisimple() != owner) {
    global.fatal << "Just created an ElementTensorsGeneralizedVermas "
    << "whose owner is not what it should be. " << global.fatal;
  }
  return output.assignValueWithContext<ElementTensorsGeneralizedVermas<RationalFraction<Rational> > >(theElt, hwContext, calculator);
}

bool CalculatorLieTheory::animateLittelmannPaths(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::animateLittelmannPaths");
  RecursionDepthCounter recursionCounter(&calculator.recursionDepth);
  if (!input.isListNElements(3)) {
    return output.makeError("This function takes 2 arguments", calculator);
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!CalculatorConversions::convert(
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
  theWeightInSimpleCoords = theSSowner->weylGroup.getSimpleCoordinatesFromFundamental(theWeight, Rational::zero());
  calculator << "<br>Function animateLittelmannPaths: your input in simple coords: "
  << theWeightInSimpleCoords.toString();
  LittelmannPath thePath;
  thePath.makeFromWeightInSimpleCoords(theWeightInSimpleCoords, theSSowner->weylGroup);
  return output.assignValue(thePath.generateOrbitAndAnimate(), calculator);
}

bool CalculatorLieTheory::splitFDpartB3overG2inner(Calculator& calculator, BranchingData& theG2B3Data, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::splitFDpartB3overG2inner");
  ModuleSSalgebra<RationalFraction<Rational> > theModCopy;
  theModCopy.makeFromHW(
    theG2B3Data.theHmm.range(),
    theG2B3Data.theWeightFundCoords,
    theG2B3Data.selInducing,
    Rational::one(),
    Rational::zero(),
    nullptr,
    false
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

  calculator.objectContainer.theCategoryOmodules.addNoRepetitionOrReturnIndexFirst(theModCopy);
  int theModIndex = calculator.objectContainer.theCategoryOmodules.getIndex(theModCopy);
  ModuleSSalgebra<RationalFraction<Rational> >& theMod = calculator.objectContainer.theCategoryOmodules[theModIndex];
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
  invertedG2cartanMat = theG2B3Data.theHmm.domain().weylGroup.cartanSymmetric;
  invertedG2cartanMat.invert();
  WeylGroupData& rangeWeyl = theG2B3Data.theHmm.range().weylGroup;
  RationalFraction<Rational> zero(Rational::zero());
  RationalFraction<Rational> one(Rational::one());
  theG2B3Data.outputWeightsSimpleCoords = rangeWeyl.getSimpleCoordinatesFromFundamental(
    theG2B3Data.outputWeightsFundCoordS, zero
  );
  Vector<RationalFraction<Rational> > weightSimpleCoordinates;
  weightSimpleCoordinates = rangeWeyl.getSimpleCoordinatesFromFundamental(
    theG2B3Data.theWeightFundCoords, zero
  );
  theG2B3Data.theAmbientChar.makeFromWeight(weightSimpleCoordinates, &theG2B3Data.theHmm.range());
  theG2B3Data.theSmallCharFDpart.makeZero();
  CharacterSemisimpleLieAlgebraModule<RationalFraction<Rational> > tempMon;
  for (int i = 0; i < theG2B3Data.outputWeightsSimpleCoords.size; i ++) {
    Vector<RationalFraction<Rational> >& currentWeight = theG2B3Data.outputWeightsSimpleCoords[i];
    Vector<RationalFraction<Rational> >& currentG2Weight = theG2B3Data.g2Weights[i];
    Vector<RationalFraction<Rational> >& currentG2DualWeight = theG2B3Data.g2DualWeights[i];
    currentG2DualWeight.setSize(2);
    currentG2DualWeight[0] = theG2B3Data.theHmm.range().weylGroup.rootScalarCartanRoot(
      currentWeight, theG2B3Data.theHmm.ImagesCartanDomain[0]
    );
    //<-note: implicit type conversion: the return type is the left coefficient type.
    currentG2DualWeight[1] = theG2B3Data.theHmm.range().weylGroup.rootScalarCartanRoot(
      currentWeight, theG2B3Data.theHmm.ImagesCartanDomain[1]
    );
    //<-note: implicit type conversion: the return type is the left coefficient type.
    invertedG2cartanMat.actOnVectorColumn(currentG2DualWeight, currentG2Weight, zero);//<-g2weight is now computed;
    tempMon.makeFromWeight(currentG2Weight, &theG2B3Data.theHmm.domain());
    theG2B3Data.theSmallCharFDpart += tempMon;
  }
  ElementUniversalEnveloping<RationalFraction<Rational> > theG2Casimir, theG2CasimirCopy, imageCasimirInB3, tempElt;
  theG2Casimir.makeCasimir(theG2B3Data.theHmm.domain());

  theG2B3Data.theChars.setSize(theG2B3Data.outputWeightsFundCoordS.size);
  for (int i = 0; i < theG2B3Data.outputWeightsSimpleCoords.size; i ++) {
    Vector<RationalFraction<Rational> >& currentG2DualWeight = theG2B3Data.g2DualWeights[i];
    theG2CasimirCopy = theG2Casimir;
    theG2CasimirCopy.modOutVermaRelations(&currentG2DualWeight, one, zero);
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
  ElementSumGeneralizedVermas<RationalFraction<Rational> >& theHWV = *theG2B3Data.theEigenVectorsLevi.lastObject();
  theHWV.makeHWV(theMod, one);
  theHWV *= - 1;
  *theG2B3Data.theEigenVectorS.lastObject() = theHWV;
  Vector<RationalFraction<Rational> > weightDifference;
  theG2B3Data.theHmm.applyHomomorphism(theG2Casimir, imageCasimirInB3);
  theG2Casimir.checkConsistency();
  imageCasimirInB3.checkConsistency();
  RationalFraction<Rational> charDiff;
  theG2B3Data.theHmm.range().orderNilradical(
    theMod.parabolicSelectionNonSelectedAreElementsLevi,
    theG2B3Data.flagUseNilWeightGeneratorOrder,
    theG2B3Data.flagAscendingGeneratorOrder
  );
  for (int k = 0; k < theG2B3Data.g2Weights.size; k ++) {
    ElementSumGeneralizedVermas<RationalFraction<Rational> >& currentTensorEltLevi = theG2B3Data.theEigenVectorsLevi[k];
    ElementSumGeneralizedVermas<RationalFraction<Rational> >& currentTensorEltEigen = theG2B3Data.theEigenVectorS[k];
    ElementUniversalEnveloping<RationalFraction<Rational> >& currentUEelt = theG2B3Data.theUEelts[k];
    currentTensorEltLevi = theHWV;
    currentTensorEltLevi.multiplyMeByUEEltOnTheLeft(theG2B3Data.outputEigenWords[k]);
    currentTensorEltEigen = currentTensorEltLevi;
    if (theG2B3Data.selInducing.cardinalitySelection > 0) {
      for (int j = 0; j < theG2B3Data.g2Weights.size; j ++) {
        weightDifference = theG2B3Data.g2Weights[j] - theG2B3Data.g2Weights[k];
        if (weightDifference.isPositive()) {
          theG2CasimirCopy = imageCasimirInB3;
          tempElt.makeConstant(theG2B3Data.theChars[j], theG2B3Data.theHmm.range());
          theG2CasimirCopy -= tempElt;
          theG2CasimirCopy *= Rational(12);
          currentTensorEltEigen.multiplyMeByUEEltOnTheLeft(theG2CasimirCopy);
          charDiff = theG2B3Data.theChars[j];
          charDiff -= *theG2B3Data.theChars.lastObject();
          theG2B3Data.theCharacterDifferences.addOnTopNoRepetition(charDiff);
        }
      }
    }
    RationalFraction<Rational> scale = currentTensorEltEigen.scaleNormalizeLeadingMonomial(nullptr);
    if (!scale.isConstant(&theG2B3Data.additionalMultipliers[k])) {
      global.fatal << "This is unexpected: the scale is not a constant. " << global.fatal;
    }
    currentTensorEltEigen.extractElementUniversalEnveloping(currentUEelt, *theMod.owner);
    currentUEelt.highestWeightTransposeAntiAutomorphismBilinearForm(
      currentUEelt, theG2B3Data.theShapovalovProducts[k], &theMod.theHWDualCoordsBaseFielD, one, zero, nullptr
    );
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorLieTheory::testMonomialBaseConjecture(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::testMonomialBaseConjecture");
  RecursionDepthCounter theRecursion(&calculator.recursionDepth);
  if (!input.isListNElements(3)) {
    return output.makeError("testMonomialBaseConjecture takes two arguments as input", calculator);
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
  MonomialTensor<int, HashFunctions::hashFunction> tempMon;
  DynkinType currentType;
  for (int i = 0; i < theRanks.size; i ++) {
    currentType.makeSimpleType(theWeylLetters[i], theRanks[i]);
    SemisimpleLieAlgebra& currentAlg =
    calculator.objectContainer.getLieAlgebraCreateIfNotPresent(currentType);
    currentAlg.computeChevalleyConstants();
    currentAlg.weylGroup.getHighestWeightsAllRepresentationsDimensionLessThanOrEqualTo(theHighestWeights[i], dimBound);
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
      << currentAlg.weylGroup.weylDimFormulaFundamentalCoords(currentHW) << "</td>";
      reportStream << "Processing " << currentAlg.toStringLieAlgebraName() << ", index  "
      << i + 1 << " out of " << theRanks.size << ",  highest weight "
      << currentHW.toString() << ", dim: " << currentAlg.weylGroup.weylDimFormulaFundamentalCoords(currentHW)
      << ", index " << j + 1 << " out of " << theHws.size;
      theReport.report(reportStream.str());
      latexReport << "$" << currentHW.toStringLetterFormat("\\omega") << "$ &"
      << currentAlg.weylGroup.weylDimFormulaFundamentalCoords(currentHW) << "&";
      hwPath.makeFromWeightInSimpleCoords(
        currentAlg.weylGroup.getSimpleCoordinatesFromFundamental(currentHW, Rational::zero()), currentAlg.weylGroup
      );
      hwPath.generateOrbit(
        tempList,
        theStrings,
        MathRoutines::minimum(1000, currentAlg.weylGroup.weylDimFormulaFundamentalCoords(currentHW).numeratorShort),
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

bool CalculatorLieTheory::littelmannOperator(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::littelmannOperator");
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

bool CalculatorLieTheory::LSPath(Calculator& calculator, const Expression& input, Expression& output) {
  RecursionDepthCounter theRecutionIncrementer(&calculator.recursionDepth);
  MacroRegisterFunctionWithName("Calculator::LSPath");
  if (input.size() < 3) {
    return output.makeError("LSPath needs at least two arguments.", calculator);
  }
  WithContext<SemisimpleLieAlgebra*> theSSowner;
  if (!CalculatorConversions::convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, theSSowner
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  SemisimpleLieAlgebra& ownerSSalgebra = *theSSowner.content;
  Vectors<Rational> waypoints;
  waypoints.setSize(input.size() - 2);
  for (int i = 2; i < input.size(); i ++) {
    if (!calculator.getVector<Rational>(
      input[i], waypoints[i - 2], nullptr, ownerSSalgebra.getRank(), nullptr
    )) {
      return output.makeError("Failed to extract waypoints", calculator);
    }
  }
  waypoints = ownerSSalgebra.weylGroup.getSimpleCoordinatesFromFundamental(waypoints, Rational::zero());
  LittelmannPath theLSpath;
  theLSpath.makeFromWaypoints(waypoints, ownerSSalgebra.weylGroup);
  return output.assignValue(theLSpath, calculator);
}

bool CalculatorLieTheory::kazhdanLuzstigCoeffificents(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::kazhdanLuzstigCoeffificents");
  if (input.size() != 2) {
    return calculator
    << "Kazhdan-Lusztig coefficients function expects 1 argument. ";
  }
  RecursionDepthCounter theRecursionIncrementer(&calculator.recursionDepth);
  WithContext<SemisimpleLieAlgebra*> theSSalgebra;
  if (!CalculatorConversions::convert(
    input[1],
    CalculatorConversions::functionSemisimpleLieAlgebra,
    theSSalgebra
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  std::stringstream out;
  WeylGroupData& theWeyl = theSSalgebra.content->weylGroup;
  if (theWeyl.group.getSize() > 192) {
    out << "I have been instructed to run only for Weyl groups that"
    << " have at most 192 elements (i.e. no larger than D_4). "
    << theSSalgebra.content->toStringLieAlgebraName()
    << " has " << theWeyl.group.getSize().toString() << ".";
    return output.assignValue(out.str(), calculator);
  }
  FormatExpressions theFormat;
  theFormat.polynomialAlphabet.setSize(1);
  theFormat.polynomialAlphabet[0] = "q";
  out << "Our notation follows that of the original Kazhdan-Lusztig paper, "
  << "Representations of Coxeter Groups and Hecke Algebras.<br>";
  out << " The algebra: " << theSSalgebra.content->toStringLieAlgebraName();
  KazhdanLusztigPolynomials theKLpolys;
  theKLpolys.computeKLPolys(&theWeyl);
  theFormat.flagUseHTML = true;
  out << theKLpolys.toString(&theFormat);
  return output.assignValue(out.str(), calculator);
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
  WithContext<SemisimpleLieAlgebra*> theSSalgebra;
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
    theSSalgebra,
    CalculatorConversions::functionPolynomial<Rational>)
  ) {
    return output.makeError("Failed to extract type, highest weight, parabolic selection", calculator);
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
    theSSalgebra.context,
    theParSel,
    theSSalgebra.content,
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
  Vectors<RationalFraction<Rational> >& theHWs,
  BranchingData& theG2B3Data,
  ExpressionContext& theContext
) {
  MacroRegisterFunctionWithName("Calculator::printB3G2branchingIntermediate");
  (void) input;
  std::stringstream out, timeReport;
  std::stringstream latexTable, latexTable2;
  bool isFD = (theG2B3Data.selInducing.cardinalitySelection == 0);
  if (isFD) {
    out << "<table border =\"1\"><tr><td>$so(7)$-highest weight</td><td>Decomposition over $G_2$</td>"
    <<  "<td>$G_2\\cap b$-eigenvectors </td></tr>";
    latexTable << "\\begin{longtable}{|ccccl|} \\caption{\\label{tableB3fdsOverG2charsAndHWV} "
    << " Decompositions of finite dimensional $so(7)$-modules over $G_2$}\\\\"
    << "\\hline so(7)& dim. &$G_2$&dim.& $\\mathfrak b \\cap G_2$-singular vectors\\\\ \\hline"
    << "\\endhead \n<br>";
  } else {
    out << "Let " << HtmlRoutines::getMathNoDisplay("p\\subset so(7)") << " be the "
    << theG2B3Data.selInducing.toString() << "-parabolic subalgebra"
    << " and let " << HtmlRoutines::getMathNoDisplay("{p}'= p\\cap G_2")
    << ". Then  " << HtmlRoutines::getMathNoDisplay("{p}'") << " is the "
    << theG2B3Data.selSmallParSel.toString() << "- parabolic subalgebra of G_2"
    << "<br> <table border =\"1\"><tr><td>$so(7)$-highest weight</td>"
    << "<td>character difference from top</td>"
    << "<td>Decomposition of inducing module over "
    << HtmlRoutines::getMathNoDisplay("p'")
    << "</td><td>" << HtmlRoutines::getMathNoDisplay("p'\\cap b")
    << "-eigenvectors</td><td>Casimir projector</td><td>Extra multiplier</td><td>corresponding "
    << HtmlRoutines::getMathNoDisplay("G_2\\cap b")
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
  theG2B3Data.theFormat.numberOfAmpersandsPerNewLineForLaTeX = 0;
  Expression tempExpression;
  RationalFraction<Rational> rfZero, rfOne;
  rfZero.makeZero();
  rfOne.makeOne();
  latexTable2 << "\\begin{longtable}{|rll|}\\caption"
  << "{Values of $x_1$ for each $v_{\\lambda,i}$}\\label{tableCriticalValuesvlambda}"
  << "\\endhead";
  for (int i = 0; i < theHWs.size; i ++) {
    theG2B3Data.theWeightFundCoords = theHWs[i];
    CalculatorLieTheory::splitFDpartB3overG2inner(calculator, theG2B3Data, tempExpression);
    timeReport << tempExpression.getValue<std::string>();
    RationalFraction<Rational> numEigenVectors;
    numEigenVectors = rfZero;
    for (int j = 0; j < theG2B3Data.theSmallCharFDpart.size(); j ++) {
      numEigenVectors += theG2B3Data.theSmallCharFDpart.coefficients[j];
    }
    theG2B3Data.theFormat.customPlusSign = "";
    int eigenIndexcounter = 0;
    if (i != 0) {
      latexTable2 << "\\hline\\multicolumn{3}{|c|}{$\\lambda ="
      << theG2B3Data.theWeightFundCoords.toStringLetterFormat("\\omega", &theG2B3Data.theFormat)
      << "$}\\\\vector& coefficient of $v_\\lambda$ in $Sh_{\\lambda,i}$ &$x_1\\notin$ \\\\\\hline";
    }
    for (int k = 0; k < theG2B3Data.theSmallCharFDpart.size(); k ++) {
      CharacterSemisimpleLieAlgebraModule<RationalFraction<Rational> > tempChar;
      tempChar.addMonomial(theG2B3Data.theSmallCharFDpart[k], theG2B3Data.theSmallCharFDpart.coefficients[k]);
      int multiplicity = 0;
      theG2B3Data.theSmallCharFDpart.coefficients[k].isSmallInteger(&multiplicity);
      for (int counter = 0; counter < multiplicity; counter ++, eigenIndexcounter ++) {
        out << "<tr>";
        if (k == 0 && counter == 0) {
          theG2B3Data.theFormat.customPlusSign = "\\oplus ";
          theG2B3Data.theFormat.fundamentalWeightLetter = "\\omega";
          out << "<td rowspan =\"" << numEigenVectors.toString() << "\">" << theG2B3Data.theAmbientChar.toString(&theG2B3Data.theFormat) << "</td> ";
          if (!isFD) {
            out << "<td rowspan =\"" << numEigenVectors.toString() << "\">";
            for (int charcounter1 = 0; charcounter1 < theG2B3Data.theCharacterDifferences.size; charcounter1 ++) {
              std::string tempS = theG2B3Data.theFormat.customPlusSign;
              theG2B3Data.theFormat.customPlusSign = "";
              out << "A_{" << charcounter1 << "}=" << theG2B3Data.theCharacterDifferences[charcounter1].toString(&theG2B3Data.theFormat) << ";";
              if (charcounter1 != theG2B3Data.theCharacterDifferences.size - 1) {
                out << "<br>";
              }
              theG2B3Data.theFormat.customPlusSign = tempS;
            }
            out << "</td> ";
          }
          latexTable << "\\multirow{" << theG2B3Data.theEigenVectorS.size  << "}{*}{$"
          << theG2B3Data.theAmbientChar.toString(&theG2B3Data.theFormat) << "$}";
          Vector<RationalFraction<Rational> > theSimpleCoordinates;
          theSimpleCoordinates = theG2B3Data.WeylFD.ambientWeyl->getSimpleCoordinatesFromFundamental(
            theG2B3Data.theAmbientChar[0].weightFundamentalCoordinates,
            RationalFraction<Rational>::oneRational()
          );
          RationalFraction<Rational> theWeylSize;
          theWeylSize = theG2B3Data.WeylFD.weylDimensionFormulaInnerSimpleCoords(theSimpleCoordinates);
          latexTable << "& \\multirow{" << theG2B3Data.theEigenVectorS.size  << "}{*}{$"
          << theWeylSize.toString(&theG2B3Data.theFormat)
          << "$}";
        } else {
          latexTable << "&";
        }
        latexTable << " & ";
        if (counter == 0) {
          theG2B3Data.theFormat.fundamentalWeightLetter = "\\psi";
          theG2B3Data.theFormat.customPlusSign = "\\oplus ";
          out << "<td rowspan =\"" << multiplicity << " \">" << tempChar.toString(&theG2B3Data.theFormat) << "</td>";
          latexTable << "\\multirow{" << multiplicity  << "}{*}{$";
          latexTable << tempChar.toString(&theG2B3Data.theFormat) << "$}";
          latexTable << "&\\multirow{" << multiplicity  << "}{*}{$";
          if (multiplicity > 1) {
            latexTable << multiplicity << "\\times";
          }
          Vector<RationalFraction<Rational> > theSimpleCoordinates;
          theSimpleCoordinates = theG2B3Data.WeylFDSmall.ambientWeyl->getSimpleCoordinatesFromFundamental(
            tempChar[0].weightFundamentalCoordinates, RationalFraction<Rational>::zeroRational()
          );
          RationalFraction<Rational> dimension;
          dimension = theG2B3Data.WeylFDSmall.weylDimensionFormulaInnerSimpleCoords(theSimpleCoordinates, rfOne);
          latexTable << dimension.toString(&theG2B3Data.theFormat) << "$}";
        } else {
          latexTable << "&";
        }
        latexTable << "&";
        theG2B3Data.theFormat.customPlusSign = "";
        out << "<td>" << HtmlRoutines::getMathNoDisplay(theG2B3Data.theEigenVectorsLevi[eigenIndexcounter].toString(&theG2B3Data.theFormat)) << "</td>";
        theG2B3Data.theFormat.maximumLineLength = 20;
        latexTable << "$\\begin{array}{l}" << theG2B3Data.theEigenVectorsLevi[eigenIndexcounter].toString(&theG2B3Data.theFormat) << "\\end{array}$ \n";
        if (!isFD) {
          std::string tempS1 = theG2B3Data.getStringCasimirProjector(eigenIndexcounter, 12);
          std::string tempS2 = "("+ theG2B3Data.theUEelts[eigenIndexcounter].toString(&theG2B3Data.theFormat) + ")\\cdot v_\\lambda";
          out << "<td>" << HtmlRoutines::getMathNoDisplay(tempS1) << "</td>";
          out << "<td>" << theG2B3Data.additionalMultipliers[eigenIndexcounter].toString() << "</td>";
          out << "<td>" << HtmlRoutines::getMathNoDisplay(tempS2) << "</td>";
          out << "<td>" << theG2B3Data.theShapovalovProducts[eigenIndexcounter].toString(&theG2B3Data.theFormat);
          out << "</td>";
          int theIndex = - 1;
          numEigenVectors.isSmallInteger(&theIndex);
          if (theIndex - eigenIndexcounter - 1 > 0) {
            List<Rational> tempList, tempList2;
            latexTable2 << " $v_{\\lambda," <<  theIndex- eigenIndexcounter - 1 << "} $&";
            Polynomial<Rational> tempP;
            theG2B3Data.theShapovalovProducts[eigenIndexcounter].getNumerator(tempP);
            tempP.scaleNormalizeLeadingMonomial(&MonomialPolynomial::orderDefault());
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
  return output.assignValue(out.str(), calculator);
}

bool CalculatorLieTheory::printB3G2branchingTable(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::printB3G2branchingTable");
  Vectors<RationalFraction<Rational> > theHWs;
  BranchingData theG2B3Data;
  ExpressionContext context(calculator);
  if (!CalculatorLieTheory::printB3G2branchingTableCommon(
    calculator, input, output, theHWs, theG2B3Data, context
  )) {
    return false;
  }
  if (output.isError()) {
    return true;
  }
  return CalculatorLieTheory::printB3G2branchingIntermediate(
    calculator, input, output, theHWs, theG2B3Data, context
  );
}

bool CalculatorLieTheory::printB3G2branchingTableCharsOnly(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::printB3G2branchingTableCharsOnly");
  BranchingData theg2b3data;
  ExpressionContext theContext(calculator);
  Vectors<RationalFraction<Rational> > theHWs;
  CalculatorLieTheory::printB3G2branchingTableCommon(
    calculator, input, output, theHWs, theg2b3data, theContext
  );
  if (output.isError()) {
    return true;
  }
  CharacterSemisimpleLieAlgebraModule<RationalFraction<Rational> > theCharacter, outputChar;
  std::stringstream out;
  std::stringstream latexTable;
  bool isFD = (theg2b3data.selInducing.cardinalitySelection == 0);
  if (isFD) {
    out << "<table>"
    << "<tr><td>$so(7)$-highest weight</td>"
    << "<td>Dimension</td><td>Decomposition over $G_2$</td><td>Dimensions</td>" << "</tr>";
    latexTable << "\\begin{longtable}{|rl|} \\caption{\\label{tableB3fdsOverG2charsonly} "
    << "Decompositions of finite dimensional $so(7)$-modules over $G_2$}\\\\"
    << "\\hline$so(7)$-module & ~~~~~~ decomposition over $G_2$\\endhead \\hline\n<br>";
  } else {
    out << "Let " << HtmlRoutines::getMathNoDisplay("p\\subset so(7)") << " be the "
    << theg2b3data.selInducing.toString() << "-parabolic subalgebra "
    << "and let " << HtmlRoutines::getMathNoDisplay("{p}'= p\\cap G_2")
    << ". Then  " << HtmlRoutines::getMathNoDisplay("{p}'") << " is the "
    << theg2b3data.selSmallParSel.toString() << "- parabolic subalgebra of G_2"
    << "<br> <table><tr><td>$so(7)$-highest weight</td>"
    << "<td>Dimension of inducing fin. dim. "
    << HtmlRoutines::getMathNoDisplay(" p")
    << "-module</td><td>Decomposition of inducing module over "
    << HtmlRoutines::getMathNoDisplay("p'")
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
  ElementUniversalEnveloping<RationalFraction<Rational> > theCasimir, theCentralCharacter, resultChar;
  RationalFraction<Rational> minusOne(Rational(- 1));
  HashedList<ElementUniversalEnveloping<RationalFraction<Rational> > > theCentralChars;
  theCasimir.makeCasimir(theg2b3data.theHmm.domain());
  WeylGroupData& smallWeyl = theg2b3data.theHmm.domain().weylGroup;
  for (int k = 0; k < theHWs.size; k ++) {
    theCharacter.makeFromWeight(
      theg2b3data.theHmm.range().weylGroup.getSimpleCoordinatesFromFundamental(
        theHWs[k], RationalFraction<Rational>::zeroRational()
      ),
      &theg2b3data.theHmm.range()
    );
    theCharacter.splitCharacterOverReductiveSubalgebra(nullptr, outputChar, theg2b3data);
    theg2b3data.theFormat.fundamentalWeightLetter = "\\omega";
    out << "<tr><td> " << theCharacter.toString(&theg2b3data.theFormat) << "</td> ";
    Vector<RationalFraction<Rational> > simpleCoordinates;
    simpleCoordinates = theg2b3data.WeylFD.ambientWeyl->getSimpleCoordinatesFromFundamental(
      theCharacter[0].weightFundamentalCoordinates,
      RationalFraction<Rational>::zeroRational()
    );
    RationalFraction<Rational> dimension;
    dimension = theg2b3data.WeylFD.weylDimensionFormulaInnerSimpleCoords(simpleCoordinates);
    out << "<td>" << dimension.toString() << "</td>";
    latexTable << " $ " << theCharacter.toString(&theg2b3data.theFormat) << " $ ";
    theg2b3data.theFormat.fundamentalWeightLetter = "\\psi";
    out << "<td>" << outputChar.toString(&theg2b3data.theFormat) << "</td>";
    out << "<td>";
    theg2b3data.theFormat.customPlusSign = "\\oplus ";
    Vector<RationalFraction<Rational> > leftWeightSimple, leftWeightDual, rightWeightSimple, rightWeightDual;
    theCentralChars.clear();
    for (int i = 0; i < outputChar.size(); i ++) {
      if (!outputChar.coefficients[i].isEqualToOne()) {
        out << outputChar.coefficients[i].toString() << " x ";
      }
      simpleCoordinates = theg2b3data.WeylFDSmall.ambientWeyl->getSimpleCoordinatesFromFundamental(
        outputChar[i].weightFundamentalCoordinates, RationalFraction<Rational>::zeroRational()
      );
      dimension = theg2b3data.WeylFDSmall.weylDimensionFormulaInnerSimpleCoords(simpleCoordinates);
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
    theg2b3data.theFormat.maximumLineLength = 80;
    latexTable << "& $\\begin{array}{l} " << outputChar.toString(&theg2b3data.theFormat) << "\\end{array} $ ";
    theg2b3data.theFormat.customPlusSign = "";
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
  << "<br>\\begin{document}<br>%text body<br>" << latexTable.str()
  << "<br>%end of text body <br>\\end{document}";
  return output.assignValue(out.str(), calculator);
}

bool CalculatorLieTheory::printGeneralizedVermaModule(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::printGeneralizedVermaModule");
  Selection selectionParSel;
  Vector<RationalFraction<Rational> > theHWfundcoords;
  WithContext<SemisimpleLieAlgebra*> theSSalgebra;
  if (!calculator.getTypeHighestWeightParabolic(
    calculator,
    input,
    output,
    theHWfundcoords,
    selectionParSel,
    theSSalgebra,
    CalculatorConversions::functionRationalFunction<Rational>
  )) {
    return output.makeError("Failed to extract highest weight vector data", calculator);
  } else {
    if (output.isError()) {
      return true;
    }
  }
  if (!CalculatorLieTheory::highestWeightVectorCommon(
    calculator,
    output,
    theHWfundcoords,
    selectionParSel,
    theSSalgebra.context,
    theSSalgebra.content,
    false
  )) {
    return output.makeError("Failed to create Generalized Verma module", calculator);
  }
  if (output.isError()) {
    return true;
  }
  ElementTensorsGeneralizedVermas<RationalFraction<Rational> > theElt;
  theElt = output.getValue<ElementTensorsGeneralizedVermas<RationalFraction<Rational> > >();
  ModuleSSalgebra<RationalFraction<Rational> >& theModule = *theElt[0].theMons[0].owner;
  return output.assignValue(theModule.toString(), calculator);
}

bool CalculatorLieTheory::writeGeneralizedVermaModuleAsDifferentialOperatorUpToLevel(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::writeGeneralizedVermaModuleAsDifferentialOperatorUpToLevel");
  RecursionDepthCounter theRecursionIncrementer(&calculator.recursionDepth);
  if (!input.isListNElements(4)) {
    return output.makeError(
      "Function splitGenericGeneralizedVermaTensorFiniteDimensional is expected "
      "to have three arguments: SS algebra type, Number, List{}. ",
      calculator
    );
  }
  const Expression& leftE = input[1];
  const Expression& genVemaWeightNode = input[3];
  const Expression& levelNode = input[2];
  Expression resultSSalgebraE;
  resultSSalgebraE = leftE;
  WithContext<SemisimpleLieAlgebra*> theSSalgebra;
  if (!CalculatorConversions::convert(
    leftE, CalculatorConversions::functionSemisimpleLieAlgebra, theSSalgebra
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  int theRank = theSSalgebra.content->getRank();
  Vector<Polynomial<Rational> > highestWeightFundCoords;
  ExpressionContext hwContext(calculator);
  if (!calculator.getVector(
    genVemaWeightNode,
    highestWeightFundCoords,
    &hwContext,
    theRank,
    CalculatorConversions::functionPolynomial<Rational>
  )) {
    return calculator
    << "Failed to convert the third argument of "
    << "splitGenericGeneralizedVermaTensorFiniteDimensional to a list of " << theRank
    << " polynomials. The second argument you gave is "
    << genVemaWeightNode.toString() << ".";
  }
  int desiredHeight;
  if (!levelNode.isSmallInteger(&desiredHeight)) {
    return output.makeError("second argument of " + input.toString() + " must be a small integer", calculator);
  }
  RationalFraction<Rational> RFOne, RFZero;
  RFOne.makeOne();
  RFZero.makeZero();
  Selection selInducing;
  selInducing.makeFullSelection(theRank);
  int theCoeff;
  for (int i = 0; i < theRank; i ++) {
    if (highestWeightFundCoords[i].isSmallInteger(&theCoeff)) {
      if (theCoeff == 0) {
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
    theHWenumerator.initMaxMultiplicity(theRank - selInducing.cardinalitySelection, j);
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
  FormatExpressions theFormat;
  hwContext.getFormat(theFormat);
  return CalculatorLieTheory::writeGenVermaModAsDiffOperatorInner(
    calculator,
    input,
    output,
    theHws,
    hwContext,
    selInducing,
    theSSalgebra.content,
    false,
    nullptr,
    nullptr,
    nullptr,
    true,
    true
  );
}

bool CalculatorLieTheory::killingForm(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerKillingForm");
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
  ExpressionContext theContext = leftE.getContext();
  ElementUniversalEnveloping<RationalFraction<Rational> > left, right;
  if (
    !leftE.isOfType<ElementUniversalEnveloping<RationalFraction<Rational> > >(&left) ||
    !rightE.isOfType<ElementUniversalEnveloping<RationalFraction<Rational> > >(&right)
  ) {
    return false;
  }
  if (left.isEqualToZero() || right.isEqualToZero()) {
    return output.assignValue(0, calculator);
  }
  if (&left.getOwner() != &right.getOwner()) {
    return false;
  }
  ElementSemisimpleLieAlgebra<Rational> leftEltSS, rightEltSS;
  if (left.getLieAlgebraElementIfPossible(leftEltSS) && right.getLieAlgebraElementIfPossible(rightEltSS)) {
    return output.assignValue(leftEltSS.getOwner()->getKillingForm(leftEltSS, rightEltSS), calculator);
  }
  return output.assignValueWithContext(left.getKillingFormProduct(right), theContext, calculator);
}

bool CalculatorLieTheory::highestWeightVector(Calculator& calculator, const Expression& input, Expression& output) {
  Selection selectionParSel;
  Vector<RationalFraction<Rational> > theHWfundcoords;
  WithContext<SemisimpleLieAlgebra*> theSSalgebra;
  if (!calculator.getTypeHighestWeightParabolic(
    calculator,
    input,
    output,
    theHWfundcoords,
    selectionParSel,
    theSSalgebra,
    CalculatorConversions::functionRationalFunction<Rational>
  )) {
    return output.makeError("Failed to extract highest weight vector data", calculator);
  } else {
    if (output.isError()) {
      return true;
    }
  }
  return CalculatorLieTheory::highestWeightVectorCommon(
    calculator,
    output,
    theHWfundcoords,
    selectionParSel,
    theSSalgebra.context,
    theSSalgebra.content
  );
}

bool CalculatorLieTheory::splitGenericGeneralizedVermaTensorFiniteDimensional(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::splitGenericGeneralizedVermaTensorFiniteDimensional");
  RecursionDepthCounter theRecursionIncrementer(&calculator.recursionDepth);
  if (!input.isListNElements(4))
    return output.makeError(
      "Function splitGenericGeneralizedVermaTensorFiniteDimensional is expected to "
      "have three arguments: SS algebra type, weight, weight. ",
      calculator
    );
  const Expression& leftE = input[1];
  const Expression& genVemaWeightNode = input[3];
  const Expression& fdWeightNode = input[2];
  WithContext<SemisimpleLieAlgebra*> theSSalgebra;
  if (!CalculatorConversions::convert(
    leftE, CalculatorConversions::functionSemisimpleLieAlgebra, theSSalgebra
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  int theRank = theSSalgebra.content->getRank();
  Vector<RationalFraction<Rational> > highestWeightFundCoords;
  ExpressionContext hwContext(calculator);
  if (!calculator.getVector<RationalFraction<Rational> >(
    genVemaWeightNode,
    highestWeightFundCoords,
    &hwContext,
    theRank,
    CalculatorConversions::functionRationalFunction<Rational>
  )) {
    return calculator
    << "Failed to convert the third argument of "
    << "splitGenericGeneralizedVermaTensorFiniteDimensional to a list of "
    << theRank
    << " polynomials. The second argument you gave is "
    << genVemaWeightNode.toString() << ".";
  }
  Vector<Rational> theFDhw;
  if (!calculator.getVector<Rational>(fdWeightNode, theFDhw, nullptr, theRank, nullptr)) {
    return calculator
    << "Failed to convert the second argument of "
    << "splitGenericGeneralizedVermaTensorFiniteDimensional to a list of "
    << theRank
    << " rationals. The second argument you gave is "
    << fdWeightNode.toString() << ".";
  }
  int theNumVars = hwContext.numberOfVariables();
  RationalFraction<Rational> RFOne, RFZero;
  RFOne.makeOne();
  RFZero.makeZero();
  ElementTensorsGeneralizedVermas<RationalFraction<Rational> > theElt;
  //= theElementData.theElementTensorGenVermas.getElement();
  Selection selParSel1, selFD;
  Expression hwvFD, hwvGenVerma;
  selParSel1.makeFullSelection(theRank);
  selFD.initialize(theRank);
  int theCoeff;
  for (int i = 0; i < theRank; i ++) {
    if (highestWeightFundCoords[i].isSmallInteger(&theCoeff)) {
      if (theCoeff >= 0) {
        selParSel1.removeSelection(i);
      }
    }
    bool isGood = false;
    if (theFDhw[i].isSmallInteger(&theCoeff)) {
      if (theCoeff >= 0) {
        isGood = true;
      }
    }
    if (!isGood) {
      return output.makeError(
        "Error: the third argument of splitGenericGeneralizedVermaTensorFiniteDimensional "
        "must be a list of small non-negative integers.",
        calculator
      );
    }
  }
  theSSalgebra.content->flagHasNilradicalOrder = true;
  if (!CalculatorLieTheory::highestWeightVectorCommon(
    calculator, hwvGenVerma, highestWeightFundCoords, selParSel1, hwContext, theSSalgebra.content
  )) {
    return false;
  }
  if (hwvGenVerma.isError()) {
    output = hwvGenVerma;
    return true;
  }
  Vector<RationalFraction<Rational> > theFDhwRF;
  theFDhwRF = theFDhw;
  if (!CalculatorLieTheory::highestWeightVectorCommon(calculator, hwvFD, theFDhwRF, selFD, hwContext, theSSalgebra.content)) {
    return false;
  }
  if (hwvFD.isError()) {
    output = hwvFD;
    return true;
  }
  std::stringstream out;
  out << "hwv par sel: " << hwvGenVerma.toString() << "hwv fd: " << hwvFD.toString();
  const ElementTensorsGeneralizedVermas<RationalFraction<Rational> >& theHWgenVerma =
  hwvGenVerma.getValue<ElementTensorsGeneralizedVermas<RationalFraction<Rational> > >();
  const ElementTensorsGeneralizedVermas<RationalFraction<Rational> >& theHWfd =
  hwvFD.getValue<ElementTensorsGeneralizedVermas<RationalFraction<Rational> > >();

  ModuleSSalgebra<RationalFraction<Rational> >& theGenMod = theHWgenVerma[0].theMons[0].getOwner();
  ModuleSSalgebra<RationalFraction<Rational> >& theFDMod = theHWfd[0].theMons[0].getOwner();
  if (
    theGenMod.owner != theFDMod.owner ||
    theGenMod.getOwner().getRank() != theGenMod.parabolicSelectionNonSelectedAreElementsLevi.numberOfElements ||
    theFDMod.getOwner().getRank() != theFDMod.parabolicSelectionNonSelectedAreElementsLevi.numberOfElements
  ) {
    global.fatal << "The two modules have owners, "
    << theFDMod.getOwner().weylGroup.dynkinType.toString()
    << " and " << theGenMod.getOwner().weylGroup.dynkinType.toString() << ", and parabolic selections of max size "
    << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.numberOfElements
    << " and " << theFDMod.parabolicSelectionNonSelectedAreElementsLevi.numberOfElements << global.fatal;
  }
  ElementUniversalEnveloping<RationalFraction<Rational> > theCasimir, theCasimirMinusChar;
  CharacterSemisimpleLieAlgebraModule<RationalFraction<Rational> > theHWchar, theFDLeviSplit, theFDChaR, theFDLeviSplitShifteD;
  theHWchar.makeFromWeight(theFDMod.theHWSimpleCoordSBaseField, theSSalgebra.content);
  List<ElementUniversalEnveloping<RationalFraction<Rational> > > theLeviEigenVectors;
  Vectors<RationalFraction<Rational> > theEigenVectorWeightsFund;
  if (theGenMod.parabolicSelectionNonSelectedAreElementsLevi.numberOfElements != theGenMod.getOwner().getRank()) {
    global.fatal << "Module has parabolic selection with max size "
    << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.numberOfElements
    << " but the ambient semisimple Lie algebra is of rank "
    << theGenMod.getOwner().getRank() << ". " << global.fatal;
  }
  std::string report;
  theFDMod.splitOverLevi(
    &report,
    theGenMod.parabolicSelectionNonSelectedAreElementsLevi,
    &theLeviEigenVectors,
    &theEigenVectorWeightsFund,
    nullptr,
    &theFDLeviSplit
  );
  theFDMod.getFDchar(theFDChaR);
  List<ElementUniversalEnveloping<RationalFraction<Rational> > > theCentralCharacters;
  RationalFraction<Rational> zero(Rational::zero());
  theCasimir.makeCasimir(*theSSalgebra.content);
  Vector<RationalFraction<Rational> > currentHWsimplecoords, currentHWdualcoords;
  FormatExpressions tempFormat;
  tempFormat.maximumLineLength = 60;
  tempFormat.flagUseLatex = true;
  tempFormat.fundamentalWeightLetter = "\\psi";
  tempFormat.customPlusSign = "\\oplus ";
  hwContext.getFormat(tempFormat);
  out << "<br>Character of finite dimensional module:" << HtmlRoutines::getMathNoDisplay(theFDChaR.toString());
  if (theGenMod.parabolicSelectionSelectedAreElementsLevi.cardinalitySelection > 0) {
    out << "<br>theFDChar split over levi:" << HtmlRoutines::getMathNoDisplay(theFDLeviSplit.toString(&tempFormat));
  }
  std::stringstream latexReport1;
  out << "<br><table><tr><td>weight in fundamental coords</td><td>Character</td></tr>";
  latexReport1 << " \\begin{longtable}{rl}\\caption{\\label{table"
  << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.toString()
  << "GenVermatimes7DimCentralCharacters} $" << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.toString()
  << "$- parabolic $\\bar{\\mathfrak{p}}$} \\\\ $\\mu+\\gamma$ & Action of $\\bar c$\\\\\\hline";
  tempFormat.customPlusSign = "";
  tempFormat.chevalleyGgeneratorLetter = "\\bar{g}";
  tempFormat.chevalleyHgeneratorLetter = "\\bar{h}";
  theFDLeviSplitShifteD.makeZero();
  Weight<RationalFraction<Rational> > tempMon;
  tempMon.owner = theSSalgebra.content;
  ElementUniversalEnveloping<RationalFraction<Rational> > currentChar;
  for (int i = 0; i < theLeviEigenVectors.size; i ++) {
    tempMon.weightFundamentalCoordinates = theEigenVectorWeightsFund[i];
    tempMon.weightFundamentalCoordinates += theGenMod.theHWFundamentalCoordsBaseField;
    theFDLeviSplitShifteD.addMonomial(tempMon, RFOne);
    currentHWdualcoords = theSSalgebra.content->weylGroup.getDualCoordinatesFromFundamental(tempMon.weightFundamentalCoordinates);
    currentChar = theCasimir;
    currentChar.modOutVermaRelations(& currentHWdualcoords, RFOne, RFZero);
    theCentralCharacters.addOnTop(currentChar);
    out << "<tr><td>" << theFDLeviSplitShifteD[i].weightFundamentalCoordinates.toStringLetterFormat("\\psi") << "</td><td>"
    << currentChar.toString(&tempFormat) << "</td></tr>";
    latexReport1 << " $" << theFDLeviSplitShifteD[i].weightFundamentalCoordinates.toStringLetterFormat("\\psi", &tempFormat) << "$"
    << "&$p_{" << i + 1 << "}=$ $" << currentChar.toString(&tempFormat) << "$\\\\<br>";
  }
  out << "</table>";
  latexReport1 << "\\end{longtable}<br>";
  ElementTensorsGeneralizedVermas<RationalFraction<Rational> > tempElt, tempElt2;
  theFDMod.highestWeightVectorNotation = "v";
  theGenMod.highestWeightVectorNotation = "w";
  out << "Let w be the highest weight vector of the generalized Verma component, and let v be the highest weight vector of the finite dimensional component";
  out << "<br><table><tr><td>weight in fundamental coords</td><td>Algebraic expression</td><td>Additional multiplier</td>";
  if (theNumVars == 1) {
    out << "<td>gcd polynomial coeffs</td>";
  }
  out << "<td>the hwv</td>";
  if (theNumVars == 1) {
    out << "<td>gcd divided out</td>";
  }
  out << "</tr>";
  std::stringstream latexReport2;
  latexReport2 << "\\begin{longtable}{p{2.5cm}p{2.5cm}p{1.5cm}l}\\caption{\\label{tableDecompo"
  << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.toString() << "times7dim}Decomposition for the $"
  << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.toString()
  << "$-parabolic subalgebra $\\bar{\\mathfrak{p}}$ } \\\\ Weight & Projector applied to &"
  << " Extra multiplier & Resulting $\\bar {\\mathfrak b}$-singular vector \\endhead\\hline";
  for (int i = 0; i < theCentralCharacters.size; i ++) {
    Vector<RationalFraction<Rational> > currentWeightSimpleCoords =
    theSSalgebra.content->weylGroup.getSimpleCoordinatesFromFundamental(
      theEigenVectorWeightsFund[i], RationalFraction<Rational>::zeroRational()
    );
    tempElt.makeHWV(theFDMod, RFOne);
    tempElt.multiplyOnTheLeft(theLeviEigenVectors[i], theElt, *theSSalgebra.content, RFOne);
    tempElt.makeHWV(theGenMod, RFOne);
    theElt.tensorOnTheRight(tempElt);
    theElt *= - 1;
    std::string startingEltString = theElt.toString(&tempFormat);
    std::stringstream tempStream, tempStream2;
    tempStream << "\\begin{array}{l}";
    bool found = false;
    for (int j = 0; j < theCentralCharacters.size; j ++) {
      Vector<RationalFraction<Rational> > otherWeightSimpleCoords =
      theSSalgebra.content->weylGroup.getSimpleCoordinatesFromFundamental(
        theEigenVectorWeightsFund[j], RationalFraction<Rational>::zeroRational()
      );
      if ((otherWeightSimpleCoords - currentWeightSimpleCoords).isPositive()) {
        theCasimirMinusChar = theCasimir;
        theCasimirMinusChar -= theCentralCharacters[j];
        theElt.multiplyOnTheLeft(theCasimirMinusChar, tempElt2, *theSSalgebra.content, RFOne);
        theElt = tempElt2;
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
    RationalFraction<Rational> scale = theElt.scaleNormalizeLeadingMonomial(nullptr);
    Rational tempRat;
    if (!scale.isConstant(&tempRat)) {
      global.fatal << "Unexpected: scale not rational" << global.fatal;
    }
    currentHWsimplecoords = theGenMod.theHWSimpleCoordSBaseField;
    currentHWsimplecoords += theFDMod.theModuleWeightsSimpleCoords[i];
    out << "<tr><td>"
    << theSSalgebra.content->weylGroup.getFundamentalCoordinatesFromSimple(currentHWsimplecoords).toStringLetterFormat("\\psi")
    << "</td><td>" << HtmlRoutines::getMathNoDisplay(tempStream.str()) << "</td><td>" << tempRat.toString() << "</td>";
    latexReport2
    << "$" << theSSalgebra.content->weylGroup.getFundamentalCoordinatesFromSimple(currentHWsimplecoords).toStringLetterFormat("\\psi")
    << "$ &  " << tempStream2.str() << " &" << tempRat.toString();
    Polynomial<Rational> tmpGCD, tmpRF;
    tempFormat.maximumLineLength = 80;
    if (theNumVars == 1) {
      scale = theElt.scaleNormalizeLeadingMonomial(nullptr);
      scale.getNumerator(tmpGCD);
      tmpGCD.scaleNormalizeLeadingMonomial(&MonomialPolynomial::orderDefault());
      out << "<td>" << HtmlRoutines::getMathNoDisplay(tmpGCD.toString(&tempFormat)) << "</td>";
    }
    out << "<td>" << HtmlRoutines::getMathNoDisplay(theElt.toString(&tempFormat)) << "</td>";
    latexReport2 << "&$\\begin{array}{l}" << theElt.toString(&tempFormat) << "\\end{array}$\\\\<br>";
    if (theNumVars == 1) {
      tmpRF = tmpGCD;
      theElt /= tmpRF;
      out << "<td>" << HtmlRoutines::getMathNoDisplay("\\begin{array}{l}" + theElt.toString(&tempFormat) + "\\end{array}") << "</td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  latexReport2 << "\\end{longtable}";
  out << "<br>Ready LaTeX (table 1 and 2): <br><br><br>" << latexReport1.str() << "<br><br><br>" << latexReport2.str() << "<br>";
  return output.assignValue(out.str(), calculator);
}

bool CalculatorLieTheory::splitFDpartB3overG2(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::splitFDpartB3overG2");
  BranchingData theG2B3Data;
  ExpressionContext context(calculator);
  CalculatorLieTheory::splitFDpartB3overG2Init(calculator, input, output, theG2B3Data, context);
  if (output.isError()) {
    return true;
  }
  Vectors<RationalFraction<Rational> > theHWs;
  theHWs.addOnTop(theG2B3Data.theWeightFundCoords);
  return CalculatorLieTheory::printB3G2branchingIntermediate(calculator, input, output, theHWs, theG2B3Data, context);
}

bool CalculatorLieTheory::printB3G2branchingTableCommon(Calculator& calculator,
  const Expression& input,
  Expression& output,
  Vectors<RationalFraction<Rational> >& outputHWs,
  BranchingData& theG2B3Data,
  ExpressionContext& theContext
) {
  MacroRegisterFunctionWithName("Calculator::printB3G2branchingTableCommon");
  Vector<RationalFraction<Rational> > theHWrf;
  SelectionWithMaxMultiplicity theHWenumerator;
  int desiredHeight = 0;
  if (!CalculatorLieTheory::printB3G2branchingTableInit(
    calculator, input, output, theG2B3Data, desiredHeight, theContext
  )) {
    return false;
  }
  if (output.isError()) {
    return true;
  }
  Selection invertedSelInducing = theG2B3Data.selInducing;
  theContext.getFormat(theG2B3Data.theFormat);
  invertedSelInducing.invertSelection();
  outputHWs.setSize(0);
  for (int j = 0; j <= desiredHeight; j ++) {
    theHWenumerator.initMaxMultiplicity(3 - theG2B3Data.selInducing.cardinalitySelection, j);
    theHWenumerator.incrementSubsetFixedCardinality(j);
    LargeInteger numberOfCycles = theHWenumerator.numberOfCombinationsOfCardinality(j);
    for (int i = 0; i < numberOfCycles; i ++, theHWenumerator.incrementSubsetFixedCardinality(j)) {
      theHWrf = theG2B3Data.theWeightFundCoords;
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
  BranchingData theG2B3Data;
  CalculatorLieTheory::splitFDpartB3overG2CharsOutput(calculator, input, output, theG2B3Data);
  if (output.isError()) {
    return true;
  }
  std::stringstream out;
  CalculatorLieTheory::splitFDpartB3overG2inner(calculator, theG2B3Data, output);
  out << "<br>Highest weight: " << theG2B3Data.theWeightFundCoords.toString() << "<br>Parabolic selection: "
  << theG2B3Data.selInducing.toString() << "<br>common Levi part of G_2 and B_3: "
  << theG2B3Data.selSmallParSel.toString();
  out
  << "<table border =\"1\"><tr><td>word</td><td>B_3-weight simple coords</td><td>B_3-weight fund. coords </td>"
  << "<td>G_2 simple coordinates</td><td>G2-fund. coords</td><td>G2-dual coordinates</td><td>character</td></tr>";
  std::stringstream readyForLatexConsumptionTable1;

  readyForLatexConsumptionTable1 << "\\hline\\multicolumn{3}{|c|}{Highest weight $ "
  << theG2B3Data.theWeightFundCoords.toStringLetterFormat("\\omega")
  << "$}\\\\ weight fund. coord.& singular vector& weight proj. $\\bar h^*$ \\\\\\hline\n<br> ";
  for (int i = 0; i < theG2B3Data.outputWeightsSimpleCoords.size; i ++) {
    Vector<RationalFraction<Rational> >& currentWeightSimpleB3coords = theG2B3Data.outputWeightsSimpleCoords[i];
    Vector<RationalFraction<Rational> >& currentWeightFundB3coords = theG2B3Data.outputWeightsFundCoordS[i];
    Vector<RationalFraction<Rational> >& currentG2Weight = theG2B3Data.g2Weights[i];
    Vector<RationalFraction<Rational> >& currentG2DualWeight = theG2B3Data.g2DualWeights[i];
    readyForLatexConsumptionTable1 << "$" << currentWeightFundB3coords.toStringLetterFormat("\\omega")
    << " $ & $" << theG2B3Data.leviEigenSpace[i].toStringLetterFormat("m")
    << " $ & $ " << currentG2Weight.toStringLetterFormat("\\alpha") << " $ \\\\\n<br>";
    out << "<tr><td>" << theG2B3Data.outputEigenWords[i].toString() << "</td><td> "
    << currentWeightSimpleB3coords.toString() << "</td><td> " << currentWeightFundB3coords.toString()
    << "</td><td>" << currentG2Weight.toStringLetterFormat("\\alpha") << "</td><td> "
    << theG2B3Data.theHmm.domain().weylGroup.getFundamentalCoordinatesFromSimple(currentG2Weight).toString()
    << "</td><td> " << currentG2DualWeight.toString() << "</td>";
    out << "<td>" << HtmlRoutines::getMathNoDisplay(theG2B3Data.theChars[i].toString()) << "</td>";
    out << "</tr>";
  }
  readyForLatexConsumptionTable1 << "\\hline \n";
  out << "</table>";
  out << "<br>Ready for LaTeX consumption: ";
  out << "<br><br>" << readyForLatexConsumptionTable1.str() << "<br><br>";
  out << "<table border =\"1\"><tr><td>weight</td><td>the elt closed form</td><td>the elt</td></tr>";
  Vector<RationalFraction<Rational> > weightDifference;
  std::stringstream formulaStream1;
  for (int k = 0; k < theG2B3Data.g2Weights.size; k ++) {
    out << "<tr><td>" << theG2B3Data.g2Weights[k].toString() << "</td><td>";
    for (int j = 0; j < theG2B3Data.g2Weights.size; j ++) {
      weightDifference = theG2B3Data.g2Weights[j] - theG2B3Data.g2Weights[k];
      if (weightDifference.isPositive()) {
        formulaStream1 << "(12(i(\\bar c) - " << theG2B3Data.theChars[j].toString() <<  "))";
      }
    }
    formulaStream1 << "v_\\lambda";
    out << HtmlRoutines::getMathNoDisplay(formulaStream1.str())
    << "</td><td>" << HtmlRoutines::getMathNoDisplay(theG2B3Data.theEigenVectorS[k].toString()) << "</td></tr>";
  }
  out << "</table>";
  out << "<br>Time final: " << global.getElapsedSeconds();
  return output.assignValue(out.str(), calculator);
}

bool CalculatorLieTheory::printB3G2branchingTableInit(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  BranchingData& theG2B3data,
  int& desiredHeight,
  ExpressionContext& outputContext
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::printB3G2branchingTableInit");
  if (input.size() != 3) {
    return output.makeError("I need two arguments: first is height, second is parabolic selection. ", calculator);
  }
  desiredHeight = 0;
  if (!input[1].isSmallInteger(&desiredHeight)) {
    return output.makeError("the first argument must be a small integer", calculator);
  }
  if (desiredHeight < 0) {
    desiredHeight = 0;
  }
  const Expression& weightNode = input[2];
  CalculatorLieTheory::splitFDpartB3overG2Init(calculator, weightNode, output, theG2B3data, outputContext);
  if (output.isError()) {
    return true;
  }
  return false;
}

bool CalculatorLieTheory::splitFDpartB3overG2CharsOutput(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  BranchingData& theG2B3Data
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::splitFDpartB3overG2CharsOutput");
  ExpressionContext context(calculator);
  CalculatorLieTheory::splitFDpartB3overG2Init(
    calculator, input, output, theG2B3Data, context
  );
  if (output.isError()) {
    return true;
  }
  std::stringstream out;
  out << "<br>Highest weight: " << theG2B3Data.theWeightFundCoords.toString() << "<br>Parabolic selection: "
  << theG2B3Data.selInducing.toString();
  std::string report;
  CharacterSemisimpleLieAlgebraModule<RationalFraction<Rational> > tempChar;
  CharacterSemisimpleLieAlgebraModule<RationalFraction<Rational> > startingChar;
  Vector<RationalFraction<Rational> > simpleWeight;
  simpleWeight = theG2B3Data.theHmm.range().weylGroup.getSimpleCoordinatesFromFundamental(
    theG2B3Data.theWeightFundCoords,
    RationalFraction<Rational>::zeroRational()
  );
  startingChar.makeFromWeight(simpleWeight, &theG2B3Data.theHmm.range());
  startingChar.splitCharacterOverReductiveSubalgebra(&report, tempChar, theG2B3Data);
  out << report;
  return output.assignValue(out.str(), calculator);
}

bool CalculatorLieTheory::splitFDpartB3overG2Init(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  BranchingData& theG2B3Data,
  ExpressionContext& outputContext
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::splitFDpartB3overG2Init");
  if (!input.isListNElements(4)) {
    return output.makeError(
      "Splitting the f.d. part of a B_3-representation "
      "over G_2 requires 3 arguments",
      calculator
    );
  }
  if (!calculator.getVectorFromFunctionArguments<RationalFraction<Rational> >(
    input,
    theG2B3Data.theWeightFundCoords,
    &outputContext,
    3,
    CalculatorConversions::functionRationalFunction<Rational>
  )) {
    output.makeError(
      "Failed to extract highest weight in fundamental coordinates. ",
      calculator
    );
  }
  calculator.makeHmmG2InB3(theG2B3Data.theHmm);
  theG2B3Data.selInducing.initialize(3);
  for (int i = 0; i < theG2B3Data.theWeightFundCoords.size; i ++) {
    if (!theG2B3Data.theWeightFundCoords[i].isSmallInteger()) {
      theG2B3Data.selInducing.addSelectionAppendNewIndex(i);
    }
  }
  theG2B3Data.initAssumingParSelAndHmmInittedPart1NoSubgroups();
  return true;
}

bool CalculatorLieTheory::splitFDpartB3overG2CharsOnly(
  Calculator& calculator, const Expression& input, Expression& output
) {
  BranchingData theG2B3Data;
  return CalculatorLieTheory::splitFDpartB3overG2CharsOutput(calculator, input, output, theG2B3Data);
}

bool CalculatorLieTheory::rootSAsAndSltwos(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool showSLtwos,
  bool mustRecompute
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::rootSAsAndSltwos");
  if (input.size() != 2) {
    return calculator << "Root subalgebra / sl(2)-subalgebras function expects 1 argument. ";
  }
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebra;
  if (!CalculatorConversions::convert(
    input[1],
    CalculatorConversions::functionSemisimpleLieAlgebra,
    semisimpleLieAlgebra
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  FormatExpressions format;
  format.flagUseHTML = true;
  format.flagUseLatex = false;
  format.flagUsePNG = true;

  std::stringstream outRootHtmlFileName, outRootHtmlDisplayName, outSltwoMainFile;

  std::string displayFolder = semisimpleLieAlgebra.content->fileNames.displayFolderName("output/");

  outSltwoMainFile << displayFolder << semisimpleLieAlgebra.content->fileNames.fileNameRelativePathSlTwoSubalgebras();
  outRootHtmlFileName    << displayFolder << semisimpleLieAlgebra.content->fileNames.toStringFileNameNoPathRootSubalgebras();
  outRootHtmlDisplayName << displayFolder << semisimpleLieAlgebra.content->fileNames.toStringFileNameNoPathRootSubalgebras();
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
      &calculator.objectContainer.algebraicClosure
    );
    slTwoSubalgebras.writeHTML(&format);
    semisimpleLieAlgebra.content->writeHTML(true, false);
  } else {
    out << "The table is precomputed and served from the hard disk. <br>";
  }
  out << "<a href='"
  << (showSLtwos ? outSltwoMainFile.str() : outRootHtmlDisplayName.str())
  << "' target='_blank'>"
  << semisimpleLieAlgebra.content->toStringLieAlgebraName() << " </a>";
  return output.assignValue(out.str(), calculator);
}

bool CalculatorLieTheory::decomposeFDPartGeneralizedVermaModuleOverLeviPart(
  Calculator& calculator, const Expression& input, Expression& output
) {
  RecursionDepthCounter recursionCounter(&calculator.recursionDepth);
  if (!input.isListNElements(5)) {
    return output.makeError(
      "Function decompose finite-dimensional part of "
      "generalized Verma over Levi expects 4 arguments.",
      calculator
    );
  }
  const Expression& typeNode = input[1];
  const Expression& weightNode = input[2];
  const Expression& inducingParNode = input[3];
  const Expression& splittingParNode = input[4];
  WithContext<SemisimpleLieAlgebra*> ownerSSPointer;
  if (!CalculatorConversions::convert(
    typeNode,
    CalculatorConversions::functionSemisimpleLieAlgebra,
    ownerSSPointer
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  Vector<RationalFraction<Rational> > theWeightFundCoords;
  Vector<Rational> inducingParSel, splittingParSel;
  SemisimpleLieAlgebra& ownerSS = *ownerSSPointer.content;
  WeylGroupData& theWeyl = ownerSS.weylGroup;
  int theDim = ownerSS.getRank();
  ExpressionContext finalContext(calculator);
  if (!calculator.getVector<RationalFraction<Rational> >(
    weightNode,
    theWeightFundCoords,
    &finalContext,
    theDim,
    CalculatorConversions::functionRationalFunction<Rational>
  )) {
    return output.makeError("Failed to extract highest weight from the second argument.", calculator);
  }
  if (!calculator.getVector<Rational>(inducingParNode, inducingParSel, &finalContext, theDim, nullptr)) {
    return output.makeError("Failed to extract parabolic selection from the third argument", calculator);
  }
  if (!calculator.getVector<Rational>(splittingParNode, splittingParSel, &finalContext, theDim, nullptr)) {
    return output.makeError("Failed to extract parabolic selection from the fourth argument", calculator);
  }
  calculator << "Your input weight in fundamental coordinates: " << theWeightFundCoords.toString();
  calculator << "<br>Your input weight in simple coordinates: "
  << theWeyl.getSimpleCoordinatesFromFundamental(
    theWeightFundCoords, RationalFraction<Rational>::zeroRational()
  ).toString()
  << "<br>Your inducing parabolic subalgebra: " << inducingParSel.toString() << "."
  << "<br>The parabolic subalgebra I should split over: " << splittingParSel.toString() << ".";
  ModuleSSalgebra<RationalFraction<Rational> > theMod;
  Selection selInducing = inducingParSel;
  Selection selSplittingParSel = splittingParSel;
  theMod.makeFromHW(ownerSS, theWeightFundCoords, selInducing, 1, 0, nullptr, false);
  std::string report;
  theMod.splitOverLevi(&report, selSplittingParSel);
  return output.assignValue(report, calculator);
}

bool CalculatorLieTheory::parabolicWeylGroups(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::parabolicWeylGroups");
  if (input.size() != 2) {
    return false;
  }
  Selection selectionParSel;
  WithContext<SemisimpleLieAlgebra*> theSSPointer;
  if (!CalculatorConversions::convert(
    input[1],
    CalculatorConversions::functionSemisimpleLieAlgebra,
    theSSPointer
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  SemisimpleLieAlgebra& theSSalgebra = *theSSPointer.content;
  int numCycles = MathRoutines::twoToTheNth(selectionParSel.numberOfElements);
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms theSubgroup;
  std::stringstream out;
  for (int i = 0; i < numCycles; i ++, selectionParSel.incrementSelection()) {
    theSubgroup.makeParabolicFromSelectionSimpleRoots(theSSalgebra.weylGroup, selectionParSel, 2000);
    out << "<hr>" << HtmlRoutines::getMathNoDisplay(theSubgroup.toString());
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorLieTheory::weylDimFormula(Calculator& calculator, const Expression& input, Expression& output) {
  RecursionDepthCounter recursionCounter(&calculator.recursionDepth);
  if (input.size() != 3) {
    return output.makeError("This function takes 2 arguments", calculator);
  }
  WithContext<SemisimpleLieAlgebra*> theSSowner;
  if (!CalculatorConversions::convert(
    input[1],
    CalculatorConversions::functionSemisimpleLieAlgebra,
    theSSowner
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  Vector<RationalFraction<Rational> > theWeight;
  if (!calculator.getVector<RationalFraction<Rational> >(
    input[2],
    theWeight,
    &theSSowner.context,
    theSSowner.content->getRank(),
    CalculatorConversions::functionRationalFunction<Rational>
  )) {
    return output.makeError(
      "Failed to convert the argument of the function to a highest weight vector",
      calculator
    );
  }
  RationalFraction<Rational> rfOne;
  rfOne.makeOne();
  Vector<RationalFraction<Rational> > theWeightInSimpleCoords;
  FormatExpressions theFormat;
  theSSowner.context.getFormat(theFormat);
  theWeightInSimpleCoords = theSSowner.content->weylGroup.getSimpleCoordinatesFromFundamental(theWeight);
  calculator << "<br>Weyl dim formula input: simple coords: "
  << theWeightInSimpleCoords.toString(&theFormat)
  << ", fundamental coords: " << theWeight.toString(&theFormat);
  RationalFraction<Rational> tempRF = theSSowner.content->weylGroup.weylDimensionFormulaSimpleCoordinates(theWeightInSimpleCoords);
  return output.assignValueWithContext(tempRF, theSSowner.context, calculator);
}

bool CalculatorLieTheory::parabolicWeylGroupsBruhatGraph(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::parabolicWeylGroupsBruhatGraph");
  RecursionDepthCounter theRecursion(&calculator.recursionDepth);
  Selection parabolicSel;
  Vector<RationalFraction<Rational> > theHWfundcoords, tempRoot, theHWsimplecoords;
  WithContext<SemisimpleLieAlgebra*> theSSalgPointer;
  if (!calculator.getTypeHighestWeightParabolic(
    calculator,
    input,
    output,
    theHWfundcoords,
    parabolicSel,
    theSSalgPointer,
    CalculatorConversions::functionRationalFunction<Rational>
  )) {
    return output.makeError("Failed to extract highest weight vector data", calculator);
  } else {
    if (output.isError()) {
      return true;
    }
  }
  SemisimpleLieAlgebra& theSSalgebra = *theSSalgPointer.content;

  WeylGroupData& theAmbientWeyl = theSSalgebra.weylGroup;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms theSubgroup;
  std::stringstream out;
  if (!theSubgroup.makeParabolicFromSelectionSimpleRoots(theAmbientWeyl, parabolicSel, 500)) {
    return output.makeError("<br><br>Failed to generate Weyl subgroup, 500 elements is the limit", calculator);
  }
  theSubgroup.findQuotientRepresentatives(2000);
  out << "<br>Number elements of the coset: "
  << theSubgroup.RepresentativesQuotientAmbientOrder.size;
  out << "<br>Number of elements of the Weyl group of the Levi part: " << theSubgroup.allElements.size;
  out << "<br>Number of elements of the ambient Weyl: "
  << theSubgroup.ambientWeyl->group.elements.size;
  FormatExpressions theFormat;
  theSSalgPointer.context.getFormat(theFormat);
  if (theSubgroup.allElements.size > 498) {
    if (theSubgroup.ambientWeyl->sizeByFormulaOrNegative1('E', 6) <= theSubgroup.ambientWeyl->group.getSize()) {
      out << "Weyl group is too large. <br>";
    } else {
      out << "Weyl group is too large for LaTeX. <br>";
    }
  } else {
    std::stringstream outputFileContent, outputFileContent2;
    std::string fileHasse, fileCosetGraph;
    bool useJavascript = (theSubgroup.allElements.size < 100);
    outputFileContent << "\\documentclass{article}\\usepackage[all,cmtip]{xy}\\begin{document}\n";
    outputFileContent << "\\[" << theSubgroup.toStringBruhatGraph() << "\\]";
    outputFileContent << "\n\\end{document}";
    outputFileContent2 << "\\documentclass{article}\\usepackage[all,cmtip]{xy}\\begin{document}\n";
    outputFileContent2 << "\\[" << theSubgroup.toStringCosetGraph() << "\\]";
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
    theFormat.fundamentalWeightLetter = "\\omega";
    for (int i = 0; i < theSubgroup.RepresentativesQuotientAmbientOrder.size; i ++) {
      ElementWeylGroup& current = theSubgroup.RepresentativesQuotientAmbientOrder[i];
      out << "<tr><td>"
      << (useJavascript ? HtmlRoutines::getMathNoDisplay(current.toString()) : current.toString())
      << "</td>";
      theHWsimplecoords = theSSalgebra.weylGroup.getSimpleCoordinatesFromFundamental(theHWfundcoords, RationalFraction<Rational>::zeroRational());
      theSSalgebra.weylGroup.actOnRhoModified(theSubgroup.RepresentativesQuotientAmbientOrder[i], theHWsimplecoords);
      out << "<td>"
      << (useJavascript ? HtmlRoutines::getMathNoDisplay(theHWsimplecoords.toString(&theFormat))
      : theHWsimplecoords.toString(&theFormat))
      << "</td>";
      tempRoot = theSSalgebra.weylGroup.getFundamentalCoordinatesFromSimple(theHWsimplecoords);
      std::string theFundString = tempRoot.toStringLetterFormat(theFormat.fundamentalWeightLetter, &theFormat);
      out << "<td>" << (useJavascript ? HtmlRoutines::getMathNoDisplay(theFundString): theFundString)
      << "</td>";
      out << "</tr>";
    }
    out << "</table><hr>";
    out << theSubgroup.toString();
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorLieTheory::decomposeCharGenVerma(
  Calculator& calculator, const Expression& input, Expression& output
) {
  RecursionDepthCounter theRecursionIncrementer(&calculator.recursionDepth);
  MacroRegisterFunctionWithName("CalculatorLieTheory::decomposeCharGenVerma");
  Vector<RationalFraction<Rational> > theHWfundcoords, theHWsimpCoords, theHWFundCoordsFDPart, theHWSimpCoordsFDPart;
  Selection parSel, invertedParSel;
  WithContext<SemisimpleLieAlgebra*> theSSlieAlg;
  output.reset(calculator);
  if (!calculator.getTypeHighestWeightParabolic<RationalFraction<Rational> >(
    calculator,
    input,
    output,
    theHWfundcoords,
    parSel,
    theSSlieAlg,
    CalculatorConversions::functionRationalFunction<Rational>
  )) {
   return false;
  }
  if (output.isError()) {
    return true;
  }
  std::stringstream out;
  FormatExpressions theFormat;
  theSSlieAlg.context.getFormat(theFormat);
  out << "<br>Highest weight: " << theHWfundcoords.toString(&theFormat)
  << "<br>Parabolic selection: " << parSel.toString();
  theHWFundCoordsFDPart = theHWfundcoords;
  for (int i = 0; i < parSel.cardinalitySelection; i ++) {
    theHWFundCoordsFDPart[parSel.elements[i]] = 0;
  }
  KazhdanLusztigPolynomials theKLpolys;
  WeylGroupData& theWeyl = theSSlieAlg.content->weylGroup;
  if (!theKLpolys.computeKLPolys(&theWeyl)) {
    return output.makeError("failed to generate Kazhdan-Lusztig polynomials (output too large?)", calculator);
  }
  theHWSimpCoordsFDPart = theWeyl.getSimpleCoordinatesFromFundamental(
    theHWFundCoordsFDPart, RationalFraction<Rational>::zeroRational()
  );
  theHWSimpCoordsFDPart += theWeyl.rho;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms theSub;
  if (!theSub.makeParabolicFromSelectionSimpleRoots(theWeyl, parSel, 1000)) {
    return output.makeError(
      "Failed to generate Weyl subgroup of Levi part (possibly too large? element limit is 1000).",
      calculator
    );
  }
  theHWsimpCoords = theWeyl.getSimpleCoordinatesFromFundamental(
    theHWfundcoords, RationalFraction<Rational>::zeroRational()
  );
  List<ElementWeylGroup> theWeylElements;
  theSub.getGroupElementsIndexedAsAmbientGroup(theWeylElements);
  Vector<RationalFraction<Rational> > currentHW;
  out << "<br>Orbit modified with small rho: "
  << "<table><tr><td>Simple coords</td><td>Fund coords</td></tr>";
  for (int i = 0; i < theWeyl.group.elements.size; i ++) {
    currentHW = theHWsimpCoords;
    currentHW += theSub.getRho();
    theWeyl.actOn(i, currentHW);
    currentHW -= theSub.getRho();
    out << "<tr><td>" << currentHW.toString() << "</td><td>"
    << theWeyl.getFundamentalCoordinatesFromSimple(currentHW).toString() << "</td></tr>";
  }
  out << "</table>";
  out << "<br>The rho of the Levi part is: "
  << theSub.getRho().toString() << "<br>Weyl group of Levi part follows. "
  << "<br><table><tr><td>Weyl element</td>"
  << "<td>Image hw under small rho modified action fund coords</td>"
  << "<td>Character Verma given h.w.</td></tr>";
  invertedParSel = parSel;
  invertedParSel.invertSelection();
  CharacterSemisimpleLieAlgebraModule<RationalFraction<Rational> > theChar, currentChar;
  Weight<RationalFraction<Rational> > theMon;
  theChar.makeZero();
  FormatExpressions formatChars;
  formatChars.FDrepLetter = "L";
  formatChars.fundamentalWeightLetter = "\\omega";
  formatChars.flagUseLatex = true;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms theSubgroup;
  theSubgroup.ambientWeyl = theKLpolys.weylGroup;
  for (int i = 0; i < theWeylElements.size; i ++) {
    ElementWeylGroup& currentElement = theWeylElements[i];
    out << "<tr><td>" << currentElement.toString() << "</td>";

    int indexInWeyl = theKLpolys.weylGroup->group.elements.getIndex(currentElement);
    if (indexInWeyl == - 1) {
      global.fatal << "Something is wrong: "
      << "I am getting that an element of the Weyl group of the Levi part of "
      << "the parabolic does not lie in the ambient Weyl group, which is impossible. "
      << "There is a bug somewhere; crashing in accordance. " << global.fatal;
    }
    currentChar.makeZero();
    theMon.owner = theSSlieAlg.content;
    for (int j = 0; j < theKLpolys.kazhdanLuzstigCoefficients[indexInWeyl].size; j ++) {
      if (!theKLpolys.kazhdanLuzstigCoefficients[indexInWeyl][j].isEqualToZero()) {
        currentHW = theHWsimpCoords;
        theWeyl.actOnRhoModified(j, currentHW);
        theMon.weightFundamentalCoordinates = theWeyl.getFundamentalCoordinatesFromSimple(currentHW);
        int sign = (currentElement.generatorsLastAppliedFirst.size - theWeyl.group.elements[j].generatorsLastAppliedFirst.size) % 2 == 0 ? 1 : - 1;
        currentChar.addMonomial(theMon, theKLpolys.kazhdanLuzstigCoefficients[indexInWeyl][j] * sign);
      }
    }
    currentHW = theHWsimpCoords;
    currentHW += theSub.getRho();
    theWeyl.actOn(indexInWeyl, currentHW);
    currentHW -= theSub.getRho();
    out << "<td>" << theWeyl.getFundamentalCoordinatesFromSimple(currentHW).toStringLetterFormat("\\omega") << "</td>";
    out << "<td>" << HtmlRoutines::getMathNoDisplay(currentChar.toString(&formatChars)) << "</td>";
    if (currentElement.generatorsLastAppliedFirst.size % 2 == 1) {
      currentChar *= - 1;
    }
    theChar += currentChar;
    out << "</tr>";
  }
  out << "</table>";
  out << "Final char: " << HtmlRoutines::getMathNoDisplay(theChar.toString(&formatChars));
  return output.assignValue<std::string>(out.str(), calculator);
}

template <>
bool Expression::convertsInternally<ElementSemisimpleLieAlgebra<AlgebraicNumber> >(
  WithContext<ElementSemisimpleLieAlgebra<AlgebraicNumber> >* whichElement
) const;

bool CalculatorLieTheory::constructCartanSubalgebra(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::constructCartanSubalgebra");
  SubalgebraSemisimpleLieAlgebra theSA;
  WithContext<ElementSemisimpleLieAlgebra<AlgebraicNumber> > element;
  if (input.convertsInternally(&element)) {
    theSA.theGenerators.addOnTop(element.content);
  } else {
    for (int i = 1; i < input.size(); i ++) {
      if (input[i].convertsInternally(&element)) {
        theSA.theGenerators.addOnTop(element.content);
      } else {
        return calculator
        << "Failed to extract element of a semisimple Lie algebra from "
        << input[i].toString();
      }
    }
  }
  for (int i = 0; i < theSA.theGenerators.size; i ++) {
    if (!theSA.theGenerators[i].isEqualToZero()) {
      if (theSA.owner != nullptr) {
        if (theSA.owner != theSA.theGenerators[i].getOwner()) {
          return calculator << "The input elements in " << input.toString()
          << " belong to different semisimple Lie algebras";
        }
      }
      theSA.owner = theSA.theGenerators[i].getOwner();
    }
  }
  if (theSA.owner == nullptr) {
    return calculator << "Failed to extract input semisimple Lie algebra "
    << "elements from the inputs of " << input.toString();
  }
  theSA.computeBasis();
  theSA.computeCartanSubalgebra();
  return output.assignValue(theSA.toString(), calculator);
}

bool CalculatorLieTheory::growDynkinType(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::growDynkinType");
  if (input.size() != 3) {
    return false;
  }
  const Expression& theSmallerTypeE = input[1];
  DynkinType theSmallDynkinType;
  if (!CalculatorConversions::functionDynkinType(
    calculator, theSmallerTypeE, theSmallDynkinType
  )) {
    return false;
  }
  WithContext<SemisimpleLieAlgebra*> theSSalg;
  if (!CalculatorConversions::convert(
    input[2], CalculatorConversions::functionSemisimpleLieAlgebra, theSSalg
  )) {
    return output.makeError("Error extracting ambient Lie algebra.", calculator);
  }
  SemisimpleSubalgebras tempSas;
  tempSas.initHookUpPointers(
    *theSSalg.content,
    &calculator.objectContainer.algebraicClosure,
    &calculator.objectContainer.semisimpleLieAlgebras,
    &calculator.objectContainer.slTwoSubalgebras
  );
  tempSas.computeSl2sInitOrbitsForComputationOnDemand();
  if (!tempSas.ranksAndIndicesFit(theSmallDynkinType)) {
    return output.makeError(
      "Error: type " + theSmallDynkinType.toString() + " does not fit inside " + theSSalg.content->weylGroup.dynkinType.toString(),
      calculator
    );
  }
  List<DynkinType> largerTypes;
  List<List<int> > imagesSimpleRoots;
  if (!tempSas.growDynkinType(theSmallDynkinType, largerTypes, &imagesSimpleRoots)) {
    return output.makeError(
      "Error: growing type " + theSmallDynkinType.toString() + " inside " + theSSalg.content->weylGroup.dynkinType.toString() + " failed. ",
      calculator
    );
  }
  std::stringstream out;
  out << "Inside " << theSSalg.content->weylGroup.dynkinType.toString()
  << ", input type " << theSmallDynkinType.toString();
  if (largerTypes.size == 0) {
    out << " cannot grow any further. ";
  } else {
    CandidateSemisimpleSubalgebra tempCandidate;
    out << " can grow to the following types. <br>";
    out << "<table border =\"1\">"
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
      Vector<Rational> currentHighestWeight = tempSas.getHighestWeightFundNewComponentFromImagesOldSimpleRootsAndNewRoot(
        largerTypes[i], imagesSimpleRoots[i], tempCandidate
      );
      out << HtmlRoutines::getMathNoDisplay(
        currentHighestWeight.toStringLetterFormat("\\omega")
      );
      out << "</td></tr>";
    }
    out << "</table>";
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorLieTheory::computeSemisimpleSubalgebras(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::computeSemisimpleSubalgebras");
  if (input.size() != 2) {
    return calculator << "Semisimple subalgebras function expects 1 argument. ";
  }
  WithContext<SemisimpleLieAlgebra*> ownerSSPointer;
  if (!CalculatorConversions::convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, ownerSSPointer
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  SemisimpleLieAlgebra& ownerSS = *ownerSSPointer.content;
  std::stringstream out;
  if (ownerSS.getRank() > 6) {
    out << "<b>This code is completely experimental and has been set to run up to rank 6. "
    << "As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
    return output.assignValue(out.str(), calculator);
  } else {
    out << "<b>This code is completely experimental. Use the following printouts on your own risk</b>";
  }
  SemisimpleSubalgebras& theSSsubalgebras =
  calculator.objectContainer.getSemisimpleSubalgebrasCreateIfNotPresent(ownerSS.weylGroup.dynkinType);
  theSSsubalgebras.flagcomputePairingTable = false;
  theSSsubalgebras.flagComputeNilradicals = false;
  theSSsubalgebras.findTheSemisimpleSubalgebrasFromScratch(
    ownerSS,
    calculator.objectContainer.algebraicClosure,
    calculator.objectContainer.semisimpleLieAlgebras,
    calculator.objectContainer.slTwoSubalgebras,
    nullptr
  );
  return output.assignValue(theSSsubalgebras, calculator);
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
  SemisimpleSubalgebras& theSAs = input[1].getValueNonConst<SemisimpleSubalgebras>();
  theSAs.flagcomputePairingTable = true;
  theSAs.flagComputeNilradicals = true;
  theSAs.computePairingTablesAndFKFTtypes();
  output = input;
  std::fstream theFile;
  std::string theFileName;
  theFileName = "FKFTcomputation.html";
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML = true;
  tempFormat.flagUseLatex = true;
  tempFormat.flagUseHTML = true;
  tempFormat.flagCandidateSubalgebraShortReportOnly = false;
  FileOperations::openFileCreateIfNotPresentVirtual(theFile, "output/" + theFileName, false, true, false);
  theFile << theSAs.toString(&tempFormat);
  std::stringstream out;
  out << "<a href=\"" << global.displayPathOutputFolder << "FKFTcomputation.html\">FKFTcomputation.html</a>";
  return output.assignValue(out.str(), calculator);
}

bool CalculatorLieTheory::getCentralizerChainsSemisimpleSubalgebras(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::getCentralizerChainsSemisimpleSubalgebras");
  if (!input.isOfType<SemisimpleSubalgebras>()) {
    return calculator << "<hr>Input of getCentralizerChains must be of type semisimple subalgebras. ";
  }
  SemisimpleSubalgebras& theSAs = input.getValueNonConst<SemisimpleSubalgebras>();
  List<List<int> > theChains;
  std::stringstream out;
  theSAs.getCentralizerChains(theChains);
  Expression currentChainE;
  out << theChains.size << " chains total. <br>";
  for (int i = 0; i < theChains.size; i ++) {
    out << "<br>Chain " << i + 1 << ": LoadSemisimpleSubalgebras{}( "
    << theSAs.owner->weylGroup.dynkinType.toString() << ", (";
    for (int j = 0; j < theChains[i].size; j ++) {
      CalculatorConversions::innerStoreCandidateSubalgebra(
        calculator, theSAs.subalgebras.values[theChains[i][j]], currentChainE
      );
      out << currentChainE.toString();
      if (j != theChains[i].size - 1) {
        out << ", ";
      }
    }
    out << ")  )";
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorLieTheory::getPrincipalSl2Index(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::getPrincipalSl2Index");
  DynkinType theType;
  if (!CalculatorConversions::innerDynkinTypE(calculator, input, theType)) {
    return calculator << "Failed to convert "
    << input.toString() << " to DynkinType.";
  }
  return output.assignValue(theType.getPrincipalSlTwoCartanSymmetricInverseScale(), calculator);
}

bool CalculatorLieTheory::getDynkinIndicesSlTwoSubalgebras(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::getDynkinIndicesSlTwoSubalgebras");
  DynkinType dynkinType;
  if (!CalculatorConversions::innerDynkinTypE(calculator, input, dynkinType)) {
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
  return output.assignValue(out.str(), calculator);
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
    Vector<ElementUniversalEnveloping<RationalFraction<Rational> > > theLieAlgElts;
    context.initialize(calculator);
    if (!calculator.getVectorFromFunctionArguments(inputModded, theLieAlgElts, &context)) {
      return calculator
      << "<hr>Failed to extract elements of Weyl algebra and "
      << "failed to extract elements of UE algebra from input "
      << input.toString();
    }
    FormatExpressions theFormat;
    context.getFormat(theFormat);
    std::stringstream out;
    out << "Starting elements: <br>";
    for (int i = 0; i < theLieAlgElts.size; i ++) {
      out << HtmlRoutines::getMathNoDisplay(theLieAlgElts[i].toString(&theFormat)) << "<br>";
    }
    bool success = MathRoutines::generateVectorSpaceClosedWithRespectToLieBracket(theLieAlgElts, upperBound);
    if (!success) {
      out << "<br>Did not succeed with generating vector space, "
      << "instead got a vector space with basis "
      << theLieAlgElts.size << " exceeding the limit. "
      << "The basis generated before exceeding the limit was: "
      << theLieAlgElts.toString();
    } else {
      out << "<br>Lie bracket generates vector space of dimension "
      << theLieAlgElts.size << " with basis:";
      for (int i = 0; i < theLieAlgElts.size; i ++) {
        out << "<br>";
        if (theLieAlgElts.size > 50) {
          out << theLieAlgElts[i].toString(&theFormat);
        } else {
          out << HtmlRoutines::getMathNoDisplay(theLieAlgElts[i].toString(&theFormat));
        }
      }
    }
    return output.assignValue(out.str(), calculator);
  }
  FormatExpressions theFormat;
  context.getFormat(theFormat);
  std::stringstream out;
  out << "Starting elements: <br>";
  for (int i = 0; i < theOps.size; i ++) {
    out << HtmlRoutines::getMathNoDisplay(theOps[i].toString(&theFormat)) << "<br>";
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
        out << theOps[i].toString(&theFormat);
      } else {
        out << HtmlRoutines::getMathNoDisplay(theOps[i].toString(&theFormat));
      }
    }
  }
  return output.assignValue(out.str(), calculator);
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
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, algebra
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  SemisimpleLieAlgebra* theSSalg = algebra.content;
  Vector<Rational> leviSelection;
  if (!calculator.getVector(input[2], leviSelection, nullptr, theSSalg->getRank())) {
    return calculator << "<hr>Failed to extract parabolic selection. ";
  }
  Selection theParSel;
  theParSel = leviSelection;
  theParSel.invertSelection();
  ElementUniversalEnveloping<RationalFraction<Rational> > theCasimir;
  theCasimir.makeCasimirWRTLeviParabolic(*theSSalg, theParSel);
  ExpressionContext contextE(calculator);
  contextE.setAmbientSemisimpleLieAlgebra(*theSSalg);
  return output.assignValueWithContext(theCasimir, contextE, calculator);
}

template <class theType>
bool MathRoutines::generateVectorSpaceClosedWRTOperation(
  List<theType>& inputOutputElts,
  int upperDimensionBound,
  void (*theBinaryOperation)(const theType& left, const theType& right, theType& output)
) {
  MacroRegisterFunctionWithName("MathRoutines::generateVectorSpaceClosedWRTOperation");
  inputOutputElts[0].gaussianEliminationByRowsDeleteZeroRows(inputOutputElts);
  theType theOpResult;
  ProgressReport theReport1(1, GlobalVariables::Response::ReportType::gaussianElimination);
  ProgressReport theReport2(20, GlobalVariables::Response::ReportType::gaussianElimination);
  if (theReport1.tickAndWantReport()) {
    theReport1.report("Extending vector space to closed with respect to binary operation. ");
  }
  List<theType> theEltsForGaussianElimination = inputOutputElts;
  for (int i = 0; i < inputOutputElts.size; i ++) {
    for (int j = i; j < inputOutputElts.size; j ++) {
      theBinaryOperation(inputOutputElts[i], inputOutputElts[j], theOpResult);
      //int oldNumElts = inputOutputElts.size;
      theEltsForGaussianElimination.addOnTop(theOpResult);
      theEltsForGaussianElimination[0].gaussianEliminationByRowsDeleteZeroRows(theEltsForGaussianElimination);
      if (theEltsForGaussianElimination.size > inputOutputElts.size) {
        inputOutputElts.addOnTop(theOpResult);
      }
      if (upperDimensionBound > 0 && inputOutputElts.size > upperDimensionBound) {
        return false;
      }
      if (theReport2.tickAndWantReport()) {
        std::stringstream reportStream;
        reportStream << "Accounted operation between elements " << i + 1
        << " and " << j + 1 << " out of " << inputOutputElts.size;
        theReport2.report(reportStream.str());
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
  return output.assignValue(static_cast<int>(smallType.canBeExtendedParabolicallyTo(targetType)), calculator);
}

bool CalculatorLieTheory::getSymmetricCartan(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::getSymmetricCartan");
  DynkinType theType;
  if (!CalculatorConversions::innerDynkinTypE(calculator, input, theType)) {
    return calculator << "Failed to convert " << input.toString() << " to DynkinType.";
  }
  std::stringstream out;
  Matrix<Rational> outputMat, outputCoMat;
  theType.getCartanSymmetric(outputMat);
  theType.getCoCartanSymmetric(outputCoMat);
  out << "Symmetric Cartan matrix: " << HtmlRoutines::getMathNoDisplay(outputMat.toStringLatex(), 10000)
  << "<br>Co-symmetric Cartan matrix: " << HtmlRoutines::getMathNoDisplay(outputCoMat.toStringLatex(), 10000);
  return output.assignValue(out.str(), calculator);
}

bool CalculatorLieTheory::drawWeightSupportWithMults(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (!input.isListNElements(3)) {
    return output.makeError(
      "Error: the function for drawing weight support takes two arguments (type and highest weight)",
      calculator
    );
  }
  const Expression& typeNode = input[1];
  const Expression& hwNode = input[2];
  WithContext<SemisimpleLieAlgebra*> theSSalgpointer;
  if (!CalculatorConversions::convert(
    typeNode,
    CalculatorConversions::functionSemisimpleLieAlgebra,
    theSSalgpointer
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  Vector<Rational> highestWeightFundCoords;
  ExpressionContext theContext(calculator);
  if (!calculator.getVector<Rational>(
    hwNode, highestWeightFundCoords, &theContext, theSSalgpointer.content->getRank(), nullptr
  )) {
    return output.makeError("Failed to extract highest weight vector", calculator);
  }
  Vector<Rational> highestWeightSimpleCoords;
  WeylGroupData& theWeyl = theSSalgpointer.content->weylGroup;
  highestWeightSimpleCoords = theWeyl.getSimpleCoordinatesFromFundamental(highestWeightFundCoords);
  std::stringstream out;
  CharacterSemisimpleLieAlgebraModule<Rational> theChar;
  theChar.makeFromWeight(highestWeightSimpleCoords, theSSalgpointer.content);
  DrawingVariables theDV;
  std::string report;
  theChar.drawMeWithMultiplicities(report, theDV, 10000);
  out << report << theDV.getHTMLDiv(theWeyl.getDimension(), true);
  return output.assignValue(out.str(), calculator);
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
    input[1],
    CalculatorConversions::functionSemisimpleLieAlgebra,
    theAlgPointer
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  SemisimpleLieAlgebra& theAlg = *theAlgPointer.content;
  WeylGroupData& theWeyl = theAlg.weylGroup;
  Vectors<Rational> preferredProjectionPlane;
  if (hasPreferredProjectionPlane) {
    preferredProjectionPlane.setSize(2);
    bool isGood = calculator.getVector(
      input[2],
      preferredProjectionPlane[0],
      nullptr,
      theWeyl.getDimension(),
      nullptr
    ) && calculator.getVector(
      input[3],
      preferredProjectionPlane[1],
      nullptr,
      theWeyl.getDimension(),
      nullptr
    );
    if (!isGood) {
      return output.makeError("Failed to convert second or third argument to vector of desired dimension", calculator);
    }
  }
  std::stringstream out;
  DrawingVariables drawingVariables;
  theWeyl.drawRootSystem(drawingVariables, true, false, nullptr, true, nullptr);
  if (hasPreferredProjectionPlane) {
    drawingVariables.flagFillUserDefinedProjection = true;
    drawingVariables.FillUserDefinedProjection = preferredProjectionPlane;
  }
  out << drawingVariables.getHTMLDiv(theWeyl.getDimension(), true);
  return output.assignValue(out.str(), calculator);
}

template <class Coefficient>
int CharacterSemisimpleLieAlgebraModule<Coefficient>::getPositiveNStringSuchThatWeightMinusNAlphaIsWeight(
  const Weight<Coefficient>& theWeightInFundCoords, const Vector<Coefficient>& theAlphaInFundCoords
) {
  MacroRegisterFunctionWithName("CharacterSemisimpleLieAlgebraModule::getPositiveNStringSuchThatWeightMinusNAlphaIsWeight");
  int result = - 1;
  Weight<Coefficient> currentWeight;
  currentWeight = theWeightInFundCoords;
  for (
    ;
    this->monomials.contains(currentWeight);
    result ++, currentWeight.weightFundamentalCoordinates -= theAlphaInFundCoords
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
  Vector<Coefficient> theSimpleRoot;
  Vector<Coefficient> theSimpleRootFundCoords;
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
      theSimpleRoot.makeEi(this->getOwner()->getRank(), j);
      theSimpleRootFundCoords =
      this->getOwner()->weylGroup.getFundamentalCoordinatesFromSimple(theSimpleRoot);
      outputSimpleStringCoords[j] = outputChar.getPositiveNStringSuchThatWeightMinusNAlphaIsWeight(
        outputChar[k], theSimpleRootFundCoords
      ) - outputChar.getPositiveNStringSuchThatWeightMinusNAlphaIsWeight(
        outputChar[k], - theSimpleRootFundCoords
      );
      outputSimpleHalfStringCoords[j] = outputChar.getPositiveNStringSuchThatWeightMinusNAlphaIsWeight(
        outputChar[k], theSimpleRootFundCoords
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
    return output.makeError(
      "Wrong number of arguments, must be 2. ",
      calculator
    );
  }
  const Expression& typeNode = input[1];
  const Expression& hwNode = input[2];
  WithContext<SemisimpleLieAlgebra*> theAlgPointer;
  if (!CalculatorConversions::convert(
    typeNode,
    CalculatorConversions::functionSemisimpleLieAlgebra,
    theAlgPointer
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  SemisimpleLieAlgebra& theAlg = *theAlgPointer.content;
  Vector<Rational> highestWeightFundCoords;
  if (!calculator.getVector<Rational>(
    hwNode,
    highestWeightFundCoords,
    nullptr,
    theAlg.getRank(),
    nullptr
  )) {
    return false;
  }
  Vector<Rational> highestWeightSimpleCoords;
  WeylGroupData& theWeyl = theAlg.weylGroup;
  highestWeightSimpleCoords = theWeyl.getSimpleCoordinatesFromFundamental(highestWeightFundCoords, Rational::zero());
  // Vectors<Rational> theWeightsToBeDrawn;
  std::stringstream out;
  CharacterSemisimpleLieAlgebraModule<Rational> theChar;
  theChar.makeFromWeight(highestWeightSimpleCoords, theAlgPointer.content);
  DrawingVariables drawingVariables;
  std::string report;
  theChar.drawMeNoMultiplicities(report, drawingVariables, 10000);
  out << report << drawingVariables.getHTMLDiv(theWeyl.getDimension(), true);
  out << "<br>A table with the weights of the character follows. <br>";
  out << theChar.toStringFullCharacterWeightsTable();
  return output.assignValue(out.str(), calculator);
}

bool CalculatorLieTheory::getLinksToSimpleLieAlgerbas(Calculator& calculator, const Expression& input, Expression& output) {
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
  return output.assignValue(out.str(), calculator);
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

bool CalculatorLieTheory::printSemisimpleSubalgebrasRegular(Calculator& calculator, const Expression& input, Expression& output) {
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
  MacroRegisterFunctionWithName("CalculatorLieTheory::innerPrintSSsubalgebras");
  if (doForceRecompute) {
    if (!global.userDefaultHasAdminRights()) {
      return calculator << "Only logged-in admins allowed to force-recompute semisimple subalgebras. ";
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
  SemisimpleLieAlgebra* ownerSSPointer = nullptr;
  bool isAlreadySubalgebrasObject = input[1].isOfType<SemisimpleSubalgebras>();
  if (!isAlreadySubalgebrasObject) {
    if (!CalculatorConversions::convert(
      input[1],
      CalculatorConversions::functionSemisimpleLieAlgebra,
      ownerAlgebra
    )) {
      return output.makeError("Error extracting Lie algebra.", calculator);
    }
    ownerSSPointer = ownerAlgebra.content;
    if (ownerSSPointer->getRank() > 8) {
      out << "<b>This code is completely experimental and has been set to run up to rank 6. "
      << "As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
      return output.assignValue(out.str(), calculator);
    } else {
      out << "<b>This code is completely experimental. "
      << "Use the following printouts on your own risk.</b><br>";
    }
  } else {
    ownerSSPointer = input[1].getValue<SemisimpleSubalgebras>().owner;
  }
  if (ownerSSPointer == nullptr) {
    global.fatal << "zero pointer to semisimple Lie algebra: this shouldn't happen. " << global.fatal;
  }
  SemisimpleLieAlgebra& ownerLieAlgebra = *ownerSSPointer;
  std::string dynkinString = ownerSSPointer->weylGroup.dynkinType.toString();
  global.relativePhysicalNameOptionalProgressReport = "progress_subalgebras_" + dynkinString;
  global.relativePhysicalNameOptionalResult = "result_subalgebras_" + dynkinString;
  SemisimpleSubalgebras& subalgebras =
  calculator.objectContainer.getSemisimpleSubalgebrasCreateIfNotPresent(ownerLieAlgebra.weylGroup.dynkinType);
  subalgebras.computeStructureWriteFiles(
    ownerLieAlgebra,
    calculator.objectContainer.algebraicClosure,
    calculator.objectContainer.semisimpleLieAlgebras,
    calculator.objectContainer.slTwoSubalgebras,
    &out,
    doForceRecompute,
    !isAlreadySubalgebrasObject,
    doComputeNilradicals,
    docomputeModuleDecompositionsition,
    doAttemptToSolveSystems,
    docomputePairingTable,
    doAdjustCentralizers
  );
  return output.assignValue(out.str(), calculator);
}

bool CalculatorLieTheory::casimir(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::casimir");
  if (input.size() != 2) {
    return calculator << "Casimir function expects a single input. ";
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!CalculatorConversions::convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, algebra
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  SemisimpleLieAlgebra& algebraReference = *algebra.content;
  ElementUniversalEnveloping<RationalFraction<Rational> > theCasimir;
  theCasimir.makeCasimir(algebraReference);
  calculator << "Context Lie algebra: " << algebraReference.weylGroup.dynkinType.toString()
  << ". The coefficient: " << algebraReference.weylGroup.getKillingDividedByTraceRatio().toString()
  <<  ". The Casimir element of the ambient Lie algebra. ";
  ExpressionContext context(calculator);
  context.setAmbientSemisimpleLieAlgebra(algebraReference);
  return output.assignValueWithContext(theCasimir, context, calculator);
}

bool CalculatorLieTheory::embedG2InB3(Calculator& calculator, const Expression& input, Expression& output) {
  if (input.size() != 2) {
    return false;
  }
  output = input[1];
  if (!output.isOfType < ElementUniversalEnveloping<RationalFraction<Rational> > >()) {
    return output.makeError("Failed to convert argument to element of the Universal enveloping algebra. ", calculator);
  }
  SemisimpleLieAlgebra& ownerSS = *output.getAmbientSemisimpleLieAlgebraNonConstUseWithCaution();
  if (!ownerSS.isOfSimpleType('G', 2)) {
    return output.makeError("Error: embedding of G_2 in B_3 takes elements of U(G_2) as arguments.", calculator);
  }
  HomomorphismSemisimpleLieAlgebra theHmm;
  calculator.makeHmmG2InB3(theHmm);

  ElementUniversalEnveloping<RationalFraction<Rational> > argument = output.getValue<ElementUniversalEnveloping<RationalFraction<Rational> > >();
  ElementUniversalEnveloping<RationalFraction<Rational> > outputUE;
  if (!theHmm.applyHomomorphism(argument, outputUE)) {
    return output.makeError("Failed to apply homomorphism for unspecified reason", calculator);
  }
  outputUE.simplify(RationalFraction<Rational>::oneRational());
  ExpressionContext context(calculator);
  context.setAmbientSemisimpleLieAlgebra(theHmm.range());
  return output.assignValueWithContext(outputUE, context, calculator);
}

bool CalculatorLieTheory::characterSemisimpleLieAlgebraFiniteDimensional(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::characterSemisimpleLieAlgebraFiniteDimensional");
  Vector<Rational> theHW;
  Selection parSel;
  WithContext<SemisimpleLieAlgebra*> ownerSSLiealg;
  Expression tempE, tempE2;
  if (!calculator.getTypeHighestWeightParabolic(
    calculator, input, output, theHW, parSel, ownerSSLiealg, nullptr
  )) {
    return false;
  }
  if (output.isError()) {
    return true;
  }
  if (parSel.cardinalitySelection != 0) {
    return output.makeError("I know only to compute with finite dimensional characters, for the time being. ", calculator);
  }
  CharacterSemisimpleLieAlgebraModule<Rational> theElt;
  theElt.makeFromWeight(ownerSSLiealg.content->weylGroup.getSimpleCoordinatesFromFundamental(theHW), ownerSSLiealg.content);
  return output.assignValue(theElt, calculator);
}

bool CalculatorLieTheory::chevalleyGenerator(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::chevalleyGenerator");
  if (input.size() != 3) {
    return false;
  }
  WithContext<SemisimpleLieAlgebra*> theSSalg;
  if (!CalculatorConversions::convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, theSSalg
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  int theIndex;
  if (!input[2].isSmallInteger(&theIndex)) {
    return false;
  }
  if (theIndex > theSSalg.content->getNumberOfPositiveRoots() || theIndex == 0 || theIndex < - theSSalg.content->getNumberOfPositiveRoots()) {
    return output.makeError("Bad Chevalley-Weyl generator index.", calculator);
  }
  ElementSemisimpleLieAlgebra<Rational> theElt;
  if (theIndex > 0) {
    theIndex += theSSalg.content->getRank() - 1;
  }
  theIndex += theSSalg.content->getNumberOfPositiveRoots();
  theElt.makeGenerator(theIndex, *theSSalg.content);
  ElementUniversalEnveloping<RationalFraction<Rational> > theUE;
  theUE.assignElementLieAlgebra(theElt, *theSSalg.content);
  ExpressionContext context(calculator);
  int indexInOwner = calculator.objectContainer.semisimpleLieAlgebras.getIndex(
    theSSalg.content->weylGroup.dynkinType
  );
  context.setIndexAmbientSemisimpleLieAlgebra(indexInOwner);
  return output.assignValueWithContext(theUE, context, calculator);
}

bool CalculatorLieTheory::cartanGenerator(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::cartanGenerator");
  if (input.size() != 3) {
    return false;
  }
  WithContext<SemisimpleLieAlgebra*> theSSalg;
  if (!CalculatorConversions::convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, theSSalg
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  if (theSSalg.content == nullptr) {
    global.fatal << "Called conversion function successfully, "
    << "but the output is a zero pointer to a semisimple Lie algebra. "
    << global.fatal;
  }
  int theIndex;
  if (!input[2].isSmallInteger(&theIndex)) {
    return false;
  }
  if (
    theIndex == 0 ||
    theIndex > theSSalg.content->getNumberOfPositiveRoots() ||
    theIndex < - theSSalg.content->getNumberOfPositiveRoots()
  ) {
    return output.makeError("Bad Cartan subalgebra generator index.", calculator);
  }
  ElementSemisimpleLieAlgebra<Rational> theElt;
  Vector<Rational> theH = theSSalg.content->weylGroup.rootSystem[theSSalg.content->getRootIndexFromDisplayIndex(theIndex)];
  theElt.makeCartanGenerator(theH, *theSSalg.content);
  ElementUniversalEnveloping<RationalFraction<Rational> > theUE;
  theUE.assignElementLieAlgebra(theElt, *theSSalg.content);
  ExpressionContext theContext(calculator);
  int theAlgIndex = calculator.objectContainer.semisimpleLieAlgebras.getIndex(theSSalg.content->weylGroup.dynkinType);
  theContext.setIndexAmbientSemisimpleLieAlgebra(theAlgIndex);
  return output.assignValueWithContext(theUE, theContext, calculator);
}

bool CalculatorLieTheory::rootSubsystem(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::rootSubsystem");
  if (input.size() < 3) {
    return false;
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!CalculatorConversions::convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, algebra
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  SemisimpleLieAlgebra* theSSlieAlg = algebra.content;
  int theRank = theSSlieAlg->getRank();
  Vector<Rational> currentRoot;
  Vectors<Rational> outputRoots;
  WeylGroupData& theWeyl = theSSlieAlg->weylGroup;
  if (!theWeyl.dynkinType.isSimple()) {
    return calculator << "<hr>Function root subsystem works for simple ambient types only. ";
  }
  for (int i = 2; i < input.size(); i ++) {
    if (!calculator.getVector(input[i], currentRoot, nullptr, theRank, nullptr)) {
      return false;
    }
    if (!theWeyl.rootSystem.contains(currentRoot)) {
      return output.makeError("Input vector " + currentRoot.toString() + " is not a root. ", calculator);
    }
    outputRoots.addOnTop(currentRoot);
  }
  std::stringstream out;
  DynkinDiagramRootSubalgebra theDiagram;
  theWeyl.transformToSimpleBasisGenerators(outputRoots, theWeyl.rootSystem);
  theDiagram.ambientBilinearForm = theWeyl.cartanSymmetric;
  theDiagram.ambientRootSystem = theWeyl.rootSystem;
  theDiagram.computeDiagramInputIsSimple(outputRoots);
  out << "Diagram final: " << theDiagram.toString()
  << ". Simple basis: " << theDiagram.simpleBasesConnectedComponents.toString();
  return output.assignValue(out.str(), calculator);
}

bool CalculatorLieTheory::printSemisimpleLieAlgebra(
  Calculator& calculator, const Expression& input, Expression& output, bool Verbose
) {
  return CalculatorLieTheory::writeToHardDiskOrPrintSemisimpleLieAlgebra(calculator, input, output, Verbose, false);
}

bool CalculatorLieTheory::writeSemisimpleLieAlgebraToHardDisk(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorLieTheory::writeSemisimpleLieAlgebraToHardDisk");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!global.userDefaultHasAdminRights() && !global.flagRunningConsoleTest) {
    return output.makeError(
      "Caching structure constants to HD available to admins only. ",
      calculator);
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
    input,
    CalculatorConversions::functionSemisimpleLieAlgebra,
    algebraPointer
  )) {
    calculator << "Failed to extract Lie algebra from: " << input.toString() << "<br>";
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  algebraPointer.content->checkConsistency();
  algebraPointer.context.checkInitialization();
  SemisimpleLieAlgebra& semisimpleAlgebra = *algebraPointer.content;
  std::stringstream out;
  if (writeToHD) {
    semisimpleAlgebra.writeHTML(verbose, calculator.flagWriteLatexPlots);
    out << "<a href='"
    << semisimpleAlgebra.fileNames.toStringVirtualFileNameWithPathStructureConstants()
    << "' target='_blank'>hard drive output</a><br>";
  }
  out << semisimpleAlgebra.toHTML(verbose, calculator.flagWriteLatexPlots);
  return output.assignValue(out.str(), calculator);
}
