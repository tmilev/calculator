// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "math_general_implementation.h"
#include "math_extra_Weyl_algebras_implementation.h"
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_extra_modules_semisimple_Lie_algebras_implementation.h"
#include "math_extra_semisimple_Lie_algebras_implementation.h"
#include "math_extra_finite_groups_implementation.h"
#include "string_constants.h"
#include "math_rational_function_implementation.h"

#include <cmath>
#include <cfloat>

Calculator::Calculator() {
  this->numOutputFileS = 0;
  this->flagHideLHS = false;
  this->flagUseHtml = true;
  this->flagShowCalculatorExamples = true;
  this->flagWriteLatexPlots = true;
  this->flagUseLnInsteadOfLog = false;
  this->flagPlotShowJavascriptOnly = false;
  this->numberExpectedExpressionsAtInitialization = - 1;
}

MemorySaving<Calculator>& GlobalVariables::calculator() {
  static MemorySaving<Calculator> result;
  return result;
}

std::string Calculator::writeDefaultLatexFileReturnHtmlLink(
  const std::string& fileContent,
  std::string* outputFileNameNoExtension,
  bool useLatexDviPSpsToPNG
) {
  std::fstream theFile;
  std::stringstream fileName;
  std::stringstream systemCommand1, systemCommand2, systemCommand3;
  fileName << "defaultOutput" << MathRoutines::hashString(fileContent);
  if (!FileOperations::openFileCreateIfNotPresentVirtual(
    theFile, global.displayPathOutputFolder + fileName.str() + ".tex", false, true, false
  )) {
    return "failed to create file: " + fileName.str() + ".tex";
  }
  std::string baseFolder;
  std::string fileNameWithPathPhysical;
  FileOperations::getPhysicalFileNameFromVirtual("output/", baseFolder, false, false, nullptr);
  FileOperations::getPhysicalFileNameFromVirtual(
    "output/" + fileName.str(), fileNameWithPathPhysical, false, false, nullptr
  );
  theFile << fileContent;
  theFile.flush();
  theFile.close();
  systemCommand1 << " latex -output-directory=" << baseFolder << " " << fileName.str() << ".tex";
  global.externalCommandNoOutput(systemCommand1.str(), true);
  if (useLatexDviPSpsToPNG) {
    systemCommand2 << " dvips -o " << fileNameWithPathPhysical << ".ps "
    << fileNameWithPathPhysical << ".dvi";
    global.externalCommandNoOutput(systemCommand2.str(), true);
    systemCommand3 << " convert " << fileNameWithPathPhysical
    << ".ps " << fileNameWithPathPhysical << ".png";
    global.externalCommandNoOutput(systemCommand3.str(), true);
  }
  std::stringstream out;
  out << "<img src =\"" << global.displayPathOutputFolder
  << fileName.str() << ".png\"></img><a href=\""
  << global.displayPathOutputFolder
  << fileName.str() << ".tex\">" << global.displayPathOutputFolder
  << fileName.str() << ".tex</a>";
  if (outputFileNameNoExtension != nullptr) {
    *outputFileNameNoExtension = global.displayPathOutputFolder + fileName.str();
  }
  this->numOutputFileS ++;
  return out.str();
}

int IntIdentity(const int& x) {
  return x;
}

bool Calculator::getVectorExpressions(const Expression& input, List<Expression>& output, int targetDimNonMandatory) {
  MacroRegisterFunctionWithName("Calculator::getVectorExpressions");
  output.reserve(input.size());
  output.setSize(0);
  if (!input.isSequenceNElements() && !input.startsWith(this->opIntervalOpen())) {
    if (targetDimNonMandatory > 0) {
      if (targetDimNonMandatory != 1) {
        return *this << "<hr>getVector failure: target dim is "
        << targetDimNonMandatory << " but the input " << input.toString()
        << " can only be interpreted as a single element";
      }
    }
    output.addOnTop(input);
    return true;
  }
  if (targetDimNonMandatory > 0) {
    if (targetDimNonMandatory != input.size() - 1) {
      return *this << "<hr>Failed to getVector: the input is required to have "
      << targetDimNonMandatory << " columns but it has "
      << input.size() - 1 << " columns instead. <hr>";
    }
  }
  targetDimNonMandatory = input.size() - 1;
  for (int i = 0; i < targetDimNonMandatory; i ++) {
    output.addOnTop(input[i + 1]);
  }
  return true;
}

template <class Coefficient>
bool ModuleSSalgebra<Coefficient>::isNotInParabolic(int theGeneratorIndex) {
  Vector<Rational> theWeight = this->getOwner().getWeightOfGenerator(theGeneratorIndex);
  for (int j = 0; j < this->parabolicSelectionNonSelectedAreElementsLevi.cardinalitySelection; j ++) {
    if (!(theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[j]] < 0)) {
      return true;
    }
  }
  return false;
}

template <class Coefficient>
bool ModuleSSalgebra<Coefficient>::isNotInLevi(int theGeneratorIndex) {
  Vector<Rational> theWeight = this->getOwner().getWeightOfGenerator(theGeneratorIndex);
  for (int j = 0; j < this->parabolicSelectionNonSelectedAreElementsLevi.cardinalitySelection; j ++) {
    if (!theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[j]].isEqualToZero()) {
      return true;
    }
  }
  return false;
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::getGenericUnMinusElt(
  bool shiftPowersByNumVarsBaseField,
  ElementUniversalEnveloping<RationalFunction<Rational> >& output,
  bool useNilWeight,
  bool ascending
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra::getGenericUnMinusElt");
  List<ElementUniversalEnveloping<Coefficient> > eltsNilrad;
  this->getElementsNilradical(eltsNilrad, true, useNilWeight, ascending);
  RationalFunction<Rational> tempRF;
  output.makeZero(*this->theAlgebras, this->indexAlgebra);
  MonomialUniversalEnveloping<RationalFunction<Rational> > tempMon;
  tempMon.makeConstant(*this->theAlgebras, this->indexAlgebra);
  int varShift = 0;
  if (shiftPowersByNumVarsBaseField) {
    varShift = this->minimalNumberOfVariables();
  }
  for (int i = 0; i < eltsNilrad.size; i ++) {
    tempRF.makeOneLetterMonomial(i + varShift, 1);
    tempMon.multiplyByGeneratorPowerOnTheRight(eltsNilrad[i][0].generatorsIndices[0], tempRF);
  }
  tempRF.makeOne();
  output.addMonomial(tempMon, tempRF);
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::getGenericUnMinusElt(
  bool shiftPowersByNumVarsBaseField,
  ElementUniversalEnveloping<Polynomial<Rational> >& output,
  bool useNilWeight,
  bool ascending
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra::getGenericUnMinusElt");
  List<ElementUniversalEnveloping<Coefficient> > eltsNilrad;
  this->getElementsNilradical(eltsNilrad, true, nullptr, useNilWeight, ascending);
  Polynomial<Rational> tempRF;
  output.makeZero(*this->owner);
  MonomialUniversalEnveloping<Polynomial<Rational> > tempMon;
  tempMon.makeOne(*this->owner);
  int varShift = 0;
  if (shiftPowersByNumVarsBaseField) {
    varShift = this->minimalNumberOfVariables();
  }
  for (int i = 0; i < eltsNilrad.size; i ++) {
    tempRF.makeMonomial(i + varShift, 1, 1);
    tempMon.multiplyByGeneratorPowerOnTheRight(eltsNilrad[i][0].generatorsIndices[0], tempRF);
  }
  tempRF.makeOne();
  output.addMonomial(tempMon, tempRF);
}

class QuasiDifferentialMononomial {
  friend std::ostream& operator << (std::ostream& output, const QuasiDifferentialMononomial& theMon) {
    output << theMon.toString();
    return output;
  }
  public:
  MonomialWeylAlgebra theWeylMon;
  MonomialMatrix theMatMon;
  static unsigned int hashFunction(const QuasiDifferentialMononomial& input) {
    return input.theWeylMon.hashFunction() * someRandomPrimes[0] + input.theMatMon.hashFunction() * someRandomPrimes[1];
  }
  unsigned int hashFunction() const {
    return hashFunction(*this);
  }
  bool operator==(const QuasiDifferentialMononomial& other) const {
    return this->theWeylMon == other.theWeylMon && this->theMatMon == other.theMatMon;
  }
  void operator=(const QuasiDifferentialMononomial& other) {
    this->theWeylMon = other.theWeylMon;
    this->theMatMon = other.theMatMon;
  }
  bool operator>(const QuasiDifferentialMononomial& other) const {
    if (this->theMatMon == other.theMatMon) {
      return this->theWeylMon>other.theWeylMon;
    }
    return this->theMatMon>other.theMatMon;
  }
  std::string toString(FormatExpressions* theFormat = nullptr) const;
};

std::string QuasiDifferentialMononomial::toString(FormatExpressions* theFormat) const {
  std::stringstream out;
  out << this->theWeylMon.toString(theFormat) << "\\otimes ";
  out << this->theMatMon.toString(theFormat);
  return out.str();
}

template <class Coefficient>
class QuasiDifferentialOperator : public LinearCombination<QuasiDifferentialMononomial, Coefficient> {
public:
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  void generateBasisLieAlgebra(List<QuasiDifferentialOperator<Coefficient> >& theElts, FormatExpressions* theFormat = nullptr);
  void operator*=(const QuasiDifferentialOperator<Coefficient>& standsOnTheRight);
  void operator=(const LinearCombination<QuasiDifferentialMononomial, Coefficient>& other) {
    this->LinearCombination<QuasiDifferentialMononomial, Coefficient>::operator=(other);
  }
  void lieBracketMeOnTheRight(const LinearCombination<QuasiDifferentialMononomial, Coefficient>& standsOnTheRight) {
    QuasiDifferentialOperator<Coefficient> tempRight;
    tempRight = standsOnTheRight;
    MathRoutines::lieBracket(*this, tempRight, *this);
  }
  void fourierTransformDifferentialPartOnly(QuasiDifferentialOperator<Coefficient>& output) const;
  void getElementWeylAlgebraSetMatrixPartsToId(ElementWeylAlgebra<Coefficient>& output) const;
};

template <class Coefficient>
void QuasiDifferentialOperator<Coefficient>::fourierTransformDifferentialPartOnly(QuasiDifferentialOperator<Coefficient>& output) const {
  if (&output == this) {
    QuasiDifferentialOperator<Coefficient> thisCopy;
    thisCopy.fourierTransformDifferentialPartOnly(output);
    return;
  }
  output.makeZero();
  ElementWeylAlgebra<Coefficient> startDO, finalDO;
  QuasiDifferentialMononomial theMon;
  for (int i = 0; i < this->size(); i ++) {
    startDO.makeZero();
    startDO.addMonomial((*this)[i].theWeylMon, this->coefficients[i]);
    startDO.fourierTransform(finalDO);
    for (int j = 0; j < finalDO.size(); j ++) {
      theMon.theMatMon = (*this)[i].theMatMon;
      theMon.theWeylMon = finalDO[j];
      output.addMonomial(theMon, finalDO.coefficients[j]);
    }
  }
}

template <class Coefficient>
void QuasiDifferentialOperator<Coefficient>::getElementWeylAlgebraSetMatrixPartsToId(ElementWeylAlgebra<Coefficient>& output) const {
  output.makeZero();
  for (int i = 0; i < this->size(); i ++) {
    output.addMonomial((*this)[i].theWeylMon, this->coefficients[i]);
  }
}

template<class Element>
void MathRoutines::lieBracket(const Element& standsOnTheLeft, const Element& standsOnTheRight, Element& output) {
  if (&standsOnTheLeft == &output || &standsOnTheRight == &output) {
    Element standsOnTheLeftNew, standsOnTheRightNew;
    standsOnTheLeftNew = standsOnTheLeft;
    standsOnTheRightNew = standsOnTheRight;
    MathRoutines::lieBracket(standsOnTheLeftNew, standsOnTheRightNew, output);
    return;
  }
  Element tempE;
  output = standsOnTheLeft;
  output *= standsOnTheRight;
  tempE = standsOnTheRight;
  tempE *= standsOnTheLeft;
  output -= tempE;
}

template <class Coefficient>
void QuasiDifferentialOperator<Coefficient>::generateBasisLieAlgebra(
  List<QuasiDifferentialOperator<Coefficient> >& theElts, FormatExpressions* theFormat
) {
  MacroRegisterFunctionWithName("QuasiDifferentialOperator<Coefficient>::generateBasisLieAlgebra");
  ProgressReport theReport;
  HashedList<QuasiDifferentialMononomial> bufferMons;
  List< LinearCombination<QuasiDifferentialMononomial, Coefficient> > theEltsConverted;
  theEltsConverted = theElts;
  this->gaussianEliminationByRows(theEltsConverted);
  QuasiDifferentialOperator tempQDO;
  bool foundNew = true;
  int numTimesEliminationWasExecuted = 0;
  while (foundNew) {
    foundNew = false;
    for (int i = 0; i < theEltsConverted.size; i ++) {
      for (int j = i + 1; j < theEltsConverted.size; j ++) {
        tempQDO = theEltsConverted[i];
        std::stringstream report;
        report << "Lie bracketing elements " << " of indices " << i + 1
        << " and " << j + 1 << " out of " << theEltsConverted.size << "<br> "
        << tempQDO.toString(theFormat) << "<br> with element <br>"
        << theEltsConverted[j].toString(theFormat) << " to get <br>";
        tempQDO.lieBracketMeOnTheRight(theEltsConverted[j]);
        theReport.report(report.str());
        report << tempQDO.toString(theFormat);
        theReport.report(report.str());
        theEltsConverted.addOnTop(tempQDO);
        QuasiDifferentialOperator::gaussianEliminationByRows(theEltsConverted, 0, &bufferMons);
        numTimesEliminationWasExecuted ++;
        if (!theEltsConverted.lastObject()->isEqualToZero()) {
          foundNew = true;
        }
        for (int k = theEltsConverted.size - 1; k >= 0; k --) {
          if (theEltsConverted[k].isEqualToZero()) {
            theEltsConverted.removeIndexSwapWithLast(k);
          } else {
            break;
          }
        }
      }
    }
  }
  theElts.setSize(theEltsConverted.size);
  for (int i = 0; i < theEltsConverted.size; i ++) {
    theElts[i] = theEltsConverted[i];
  }

}

template <class Coefficient>
void QuasiDifferentialOperator<Coefficient>::operator*=(const QuasiDifferentialOperator<Coefficient>& standsOnTheRight) {
  QuasiDifferentialOperator<Coefficient> output;
  ElementWeylAlgebra<Rational> leftElt, rightElt;
  QuasiDifferentialMononomial outputMon;
  output.makeZero();
  for (int j = 0; j < standsOnTheRight.size(); j ++) {
    rightElt.makeZero();
    rightElt.addMonomial(standsOnTheRight[j].theWeylMon, standsOnTheRight.coefficients[j]);
    for (int i = 0; i < this->size(); i ++) {
      leftElt.makeZero();
      leftElt.addMonomial((*this)[i].theWeylMon, this->coefficients[i]);
      outputMon.theMatMon = (*this)[i].theMatMon;
      outputMon.theMatMon *= standsOnTheRight[j].theMatMon;
      leftElt *= rightElt;
      for (int k = 0; k < leftElt.size(); k ++) {
        outputMon.theWeylMon = leftElt[k];
        output.addMonomial(outputMon, leftElt.coefficients[k]);
      }
    }
  }
  *this = output;
}

template <class Coefficient>
std::string QuasiDifferentialOperator<Coefficient>::toString(FormatExpressions* theFormat) const {
  bool combineWeylPart = true;
  if (theFormat != nullptr) {
    combineWeylPart = theFormat->flagQuasiDiffOpCombineWeylPart;
  }
  if (!combineWeylPart) {
    return this->LinearCombination<QuasiDifferentialMononomial, Coefficient>::toString(theFormat);
  }
  MatrixTensor<ElementWeylAlgebra<Rational> > reordered;
  reordered.makeZero();
  ElementWeylAlgebra<Rational> tempP;
  for (int i = 0; i < this->size(); i ++) {
    const QuasiDifferentialMononomial& currentMon = (*this)[i];
    tempP.makeZero();
    tempP.addMonomial(currentMon.theWeylMon, this->coefficients[i]);
    reordered.addMonomial(currentMon.theMatMon, tempP);
  }
  std::string result = reordered.toString(theFormat);
  if (result == "0" && this->size() != 0) {
    global.fatal << "This is likely a programming error (crashing at any rate): "
    << "I have a non-zero quasidifferential operator "
    << " with non-properly formatted LaTeX string "
    << this->LinearCombination<QuasiDifferentialMononomial, Coefficient>::toString(theFormat)
    << ", however its properly formatted string is 0. "
    << "Probably there is something wrong with the initializations of the monomials of the qdo. " << global.fatal;
  }
  return result;
}

template <class Coefficient>
bool ModuleSSalgebra<Coefficient>::getActionEulerOperatorPart(
  const MonomialP& theCoeff, ElementWeylAlgebra<Rational>& outputDO
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra::getActionEulerOperatorPart");
  int powerMonCoeff = 0;
  ElementWeylAlgebra<Rational> currentMonContribution;
  outputDO.makeOne();
  for (int i = 0; i < theCoeff.minimalNumberOfVariables(); i ++) {
    if (!theCoeff(i).isSmallInteger(&powerMonCoeff)) {
      global.fatal << "This is a programming error. "
      << "Getting euler operator part of action on generalized Verma module: "
      << "I have an exponent with non-small integer entry. "
      << global.fatal;
    }
    currentMonContribution.makexidj(i, i, 0);
    currentMonContribution.raiseToPower(powerMonCoeff);
    outputDO *= currentMonContribution;
  }
  return true;
}

template <class Coefficient>
bool ModuleSSalgebra<Coefficient>::getActionGeneralizedVermaModuleAsDifferentialOperator(
  ElementSemisimpleLieAlgebra<Rational>& inputElt,
  QuasiDifferentialOperator<Rational>& output,
  bool useNilWeight,
  bool ascending
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra_CoefficientType::getActionGeneralizedVermaModuleAsDifferentialOperator");
  List<ElementUniversalEnveloping<Coefficient> > eltsNilrad;
  List<int> indicesNilrad;
  this->getElementsNilradical(eltsNilrad, true, &indicesNilrad, useNilWeight, ascending);
  ElementUniversalEnveloping<Polynomial<Rational> > theGenElt, result;
  this->getGenericUnMinusElt(true, theGenElt, useNilWeight, ascending);
  result.assignElementLieAlgebra(inputElt, *this->owner, 1);
  theGenElt.simplify();
  result *= theGenElt;
  result.simplify();
  MatrixTensor<Polynomial<Rational> > endoPart, tempMT, idMT;
  idMT.makeIdentitySpecial();
  MatrixTensor<RationalFunction<Rational> > tempMat1;

  int varShift = this->minimalNumberOfVariables();
  ElementWeylAlgebra<Rational> weylPartSummand, exponentContribution, oneIndexContribution,
  eulerOperatorContribution;
  Polynomial<Rational> tempP1, negativeExponentDenominatorContribution, theCoeff;
  QuasiDifferentialMononomial monQDO, monQDO2;
  Rational tempRat;
  output.makeZero();
  Rational currentShift;
  for (int i = 0; i < result.size(); i ++) {
    //problemCounter ++;
    const MonomialUniversalEnveloping<Polynomial<Rational> >& currentMon = result[i];
    endoPart = idMT;
    for (int j = currentMon.powers.size - 1; j >= indicesNilrad.size; j --) {
      int thePower = 0;
      if (!currentMon.powers[j].isSmallInteger(&thePower)) {
        return false;
      }
      tempMat1 = this->getActionGeneratorIndex(currentMon.generatorsIndices[j]);
      tempMT.makeZero();
      for (int k = 0; k < tempMat1.size(); k ++) {
        if (tempMat1.coefficients[k].expressionType == RationalFunction<Rational>::typeRationalFunction) {
          return false;
        }
        tempMat1.coefficients[k].getNumerator(tempP1);
        tempMT.addMonomial(tempMat1[k], tempP1);
      }
      MathRoutines::raiseToPower(tempMT, thePower, idMT);
      endoPart *= tempMT;
    }
    exponentContribution.makeOne();
    theCoeff = result.coefficients[i];
    for (int j = 0; j < indicesNilrad.size; j ++) {
      currentMon.powers[j].getConstantTerm(currentShift);
      ElementWeylAlgebra<Rational>::getStandardOrderDifferentialOperatorCorrespondingToNRaisedTo(
        currentShift, j + varShift, oneIndexContribution, negativeExponentDenominatorContribution
      );
      exponentContribution *= oneIndexContribution;
      theCoeff.divideBy(negativeExponentDenominatorContribution, theCoeff, tempP1, &MonomialP::orderDefault());
      if (!tempP1.isEqualToZero()) {
        global.fatal << "This is a mathematical error! "
        << "Something is very wrong with embedding semisimple Lie algebras in Weyl algebras. "
        << global.fatal;
      }
    }
    for (int l = 0; l < theCoeff.size(); l ++) {
      if (!this->getActionEulerOperatorPart(theCoeff[l], eulerOperatorContribution)) {
        return false;
      }
      weylPartSummand = exponentContribution;
      weylPartSummand *= eulerOperatorContribution;
      weylPartSummand *= theCoeff.coefficients[l];
      for (int j = 0; j < weylPartSummand.size(); j ++) {
        for (int k = 0; k < endoPart.size(); k ++) {
          monQDO.theMatMon = endoPart[k];
          monQDO.theWeylMon = weylPartSummand[j];
          Polynomial<Rational>& currentEndoCoeff = endoPart.coefficients[k];
          for (int m = 0; m < currentEndoCoeff.size(); m ++) {
            monQDO2 = monQDO;
            monQDO2.theWeylMon.polynomialPart *= currentEndoCoeff[m];
            tempRat = currentEndoCoeff.coefficients[m];
            tempRat *= weylPartSummand.coefficients[j];
            output.addMonomial(monQDO2, tempRat);
          }
        }
      }
    }
  }
  return true;
}

bool Calculator::innerWriteGenVermaModAsDiffOperatorInner(
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
  MacroRegisterFunctionWithName("Calculator::innerWriteGenVermaModAsDiffOperatorInner");
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
  List<ModuleSSalgebra<RationalFunction<Rational> > > theMods;
  theMods.setSize(theHws.size);
  Vector<RationalFunction<Rational> > tempV;
  int numStartingVars = hwContext.numberOfVariables();
  std::stringstream reportfourierTransformedCalculatorCommands, reportCalculatorCommands;
  long long totalAdditions = 0;
  long long currentAdditions = 0;
  long long totalMultiplications = 0;
  long long currentMultiplications = 0;
  double totalTime = 0, currentTime = 0;
  for (int i = 0; i < theHws.size; i ++) {
    ModuleSSalgebra<RationalFunction<Rational> >& theMod = theMods[i];
    tempV = theHws[i];
    if (!theMod.makeFromHW(theSSalgebra, tempV, selInducing, 1, 0, nullptr, true)) {
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
        actionOnGenericElt.simplify();
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
      theMod.getActionGeneralizedVermaModuleAsDifferentialOperator(theGenerator, theQDOs[j], useNilWeight, ascending);
      totalAdditions += Rational::totalAdditions() - currentAdditions;
      totalMultiplications += Rational::totalMultiplications() - currentMultiplications;
      totalTime += global.getElapsedSeconds() - currentTime;
      theWeylFormat.customCoefficientMonomialSeparator = "\\otimes ";
      theWeylFormat.numberOfAmpersandsPerNewLineForLaTeX = 2;
      out << "<td>" << HtmlRoutines::getMathNoDisplay("\\begin{array}{|r|c|l|}&&"+theQDOs[j].toString(&theWeylFormat) +"\\end{array}")
      << "</td>";
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

bool Calculator::innerHWVCommon(
  Calculator& calculator,
  Expression& output,
  Vector<RationalFunction<Rational> >& highestWeightFundCoords,
  Selection& selectionParSel,
  ExpressionContext& hwContext,
  SemisimpleLieAlgebra* owner,
  bool Verbose
) {
  MacroRegisterFunctionWithName("Calculator::innerHWVCommon");
  RecursionDepthCounter therecursionIncrementer(&calculator.recursionDepth);
  RationalFunction<Rational> RFOne, RFZero;
  RFOne.makeOne();
  RFZero.makeZero();
  std::string report;
  ElementTensorsGeneralizedVermas<RationalFunction<Rational> > theElt;
  //= theElementData.theElementTensorGenVermas.getElement();
  ListReferences<ModuleSSalgebra<RationalFunction<Rational> > >& theMods = calculator.theObjectContainer.theCategoryOmodules;
  int indexOfModule = - 1;

  for (int i = 0; i < theMods.size; i ++) {
    ModuleSSalgebra<RationalFunction<Rational> >& currentMod = theMods[i];
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
  ModuleSSalgebra<RationalFunction<Rational> >& theMod = theMods[indexOfModule];
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
    global.fatal << "This is a programming error: module has owner that is not what it should be. " << global.fatal;
  }
  theElt.makeHWV(theMod, RFOne);
  if (&theElt.getOwnerSemisimple() != owner) {
    global.fatal << "This is a programming error: just created an ElementTensorsGeneralizedVermas "
    << "whose owner is not what it should be. " << global.fatal;
  }
  return output.assignValueWithContext<ElementTensorsGeneralizedVermas<RationalFunction<Rational> > >(theElt, hwContext, calculator);
}

bool Calculator::recursionDepthExceededHandleRoughly(const std::string& additionalErrorInfo) {
  if (this->recursionDepth <= this->maximumRecursionDepth) {
    return false;
  }
  if (!this->flagMaxRecursionErrorEncountered) {
    *this << additionalErrorInfo
    << "<span style =\"color:#FF0000\"><b> maximum recursion depth of " << this->maximumRecursionDepth
    << " exceeded. </b></span>" << "Aborting computation ASAP. ";
  }
  this->flagAbortComputationASAP = true;
  this->flagMaxRecursionErrorEncountered = true;
  return true;
}

bool Calculator::checkOperationHandlers() {
  MacroRegisterFunctionWithName("Calculator::checkOperationHandlers");
  for (int i = 0; i < this->operations.size(); i ++) {
    MemorySaving<Calculator::OperationHandlers>& current = this->operations.theValues[i];
    if (current.isZeroPointer()) {
      continue;
    }
    current.getElement().checkConsistency();
    Calculator::OperationHandlers& allHandlers = current.getElement();
    for (int j = 0; j < allHandlers.compositeHandlers.size; j ++) {
      allHandlers.compositeHandlers[j].checkConsistency();
    }
    for (int j = 0; j < allHandlers.handlers.size; j ++) {
      allHandlers.handlers[j].checkConsistency();
    }
  }
  return true;
}

bool Calculator::checkConsistencyAfterInitialization() {
  this->allChildExpressions.grandMasterConsistencyCheck();
  this->evaluatedExpressionsStack.grandMasterConsistencyCheck();
  this->cachedExpressions.grandMasterConsistencyCheck();
  if (this->numberExpectedExpressionsAtInitialization < 0) {
    this->numberExpectedExpressionsAtInitialization = this->allChildExpressions.size;
  } else if (
    this->allChildExpressions.size != this->numberExpectedExpressionsAtInitialization
  ) {
    global.fatal << "Expression container expected to have "
    << this->numberExpectedExpressionsAtInitialization
    << " elements but instead it has " << this->allChildExpressions.size
    << ". Expression container: ";
    for (int i = 0; i < this->allChildExpressions.size; i ++) {
      global.fatal << this->allChildExpressions[i].toString() << ", ";
    }
    global.fatal << global.fatal;
  }
  if (
    this->cachedExpressions.size != 0 ||
    this->imagesCachedExpressions.size != 0 ||
    this->evaluatedExpressionsStack.size != 0
  ) {
    global.fatal << "This is a programming error: cached expressions, "
    << "images cached expressions, expression stack and "
    << "expression container are supposed to be empty, but "
    << "instead they contain respectively "
    << this->cachedExpressions.size << ", "
    << this->imagesCachedExpressions.size << ", and "
    << this->evaluatedExpressionsStack.size
    << " elements. " << global.fatal;
  }
  return this->theObjectContainer.checkConsistencyAfterReset();
}

bool Calculator::innerFunctionToMatrix(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerFunctionToMatrix");
  if (!input.isListNElements(4)) {
    return false;
  }
  const Expression& leftE   = input[1];
  const Expression& middleE = input[2];
  const Expression& rightE  = input[3];
  int numRows, numCols;
  if (!middleE.isIntegerFittingInInt(&numRows) || !rightE.isIntegerFittingInInt(&numCols)) {
    return false;
  }
  if (numRows <= 0 || numCols <= 0) {
    return false;
  }
  LargeInteger numRowsTimesCols = numRows;
  numRowsTimesCols *= numCols ;
  if (numRowsTimesCols > 10000) {
    calculator << "Max number of matrix entries is 10000. You requested " << numRows
    << " rows and " << numCols
    << " columns, total: " << numRowsTimesCols.toString() << " entries<br>";
    return false;
  }
  Matrix<Expression> resultMat;
  resultMat.initialize(numRows, numCols);
  Expression leftIE, rightIE;
  for (int i = 0; i < numRows; i ++) {
    for (int j = 0; j < numCols; j ++) {
      leftIE.assignValue(i + 1, calculator);
      rightIE.assignValue(j + 1, calculator);
      resultMat.elements[i][j].reset(calculator, 3);
      resultMat.elements[i][j].addChildOnTop(leftE);
      resultMat.elements[i][j].addChildOnTop(leftIE);
      resultMat.elements[i][j].addChildOnTop(rightIE);
    }
  }
  return output.assignMatrixExpressions(resultMat, calculator, true, true);
}


bool Calculator::innerKLcoeffs(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerKLcoeffs");
  if (input.size() != 2) {
    return calculator
    << "Kazhdan-Lusztig coefficients function expects 1 argument. ";
  }
  RecursionDepthCounter theRecursionIncrementer(&calculator.recursionDepth);
  WithContext<SemisimpleLieAlgebra*> theSSalgebra;
  if (!calculator.convert(
    input[1],
    CalculatorConversions::functionSemisimpleLieAlgebra,
    theSSalgebra
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  std::stringstream out;
  WeylGroupData& theWeyl = theSSalgebra.content->theWeyl;
  if (theWeyl.theGroup.getSize() > 192) {
    out << "I have been instructed to run only for Weyl groups that"
    << " have at most 192 elements (i.e. no larger than D_4). "
    << theSSalgebra.content->toStringLieAlgebraName()
    << " has " << theWeyl.theGroup.getSize().toString() << ".";
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

bool Expression::checkInitializationRecursively() const {
  MacroRegisterFunctionWithName("Expression::checkInitializationRecursively");
  this->checkInitialization();
  for (int i = 0; i < this->children.size; i ++) {
    (*this)[i].checkInitializationRecursively();
  }
  return true;
}

bool Expression::checkInitialization() const {
  if (this->owner == nullptr) {
    global.fatal << "This is a programming error: " << "Expression has non-initialized owner. " << global.fatal;
    return false;
  }
  return true;
}

bool Expression::hasInputBoxVariables(HashedList<std::string, MathRoutines::hashString>* boxNames) const {
  MacroRegisterFunctionWithName("Expression::hasInputBoxVariables");
  if (this->owner == nullptr) {
    return false;
  }
  RecursionDepthCounter recursionCounter(&this->owner->recursionDepth);
  if (this->owner->recursionDepth > this->owner->maximumRecursionDepth) {
    global.fatal << "This is a programming error: "
    << "function hasInputBoxVariables has exceeded "
    << "recursion depth limit. " << global.fatal;
  }
  bool result = false;
  InputBox tempBox;
  if (this->isOfType<InputBox>(&tempBox)) {
    if (boxNames == nullptr) {
      return true;
    } else {
      result = true;
      boxNames->addOnTopNoRepetition(tempBox.name);
    }
  }
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].hasInputBoxVariables(boxNames)) {
      if (boxNames == nullptr) {
        return true;
      } else {
        result = true;
      }
    }
  }
  return result;
}

bool Expression::hasBoundVariables() const {
  if (this->owner == nullptr) {
    global.fatal << "This is a programming error: calling function "
    << "hasBoundVariables on non-initialized expression. " << global.fatal;
  }
  RecursionDepthCounter recursionCounter(&this->owner->recursionDepth);
  MacroRegisterFunctionWithName("Expression::hasBoundVariables");
  if (this->owner->recursionDepth>this->owner->maximumRecursionDepth) {
    global.fatal << "This is a programming error: function hasBoundVariables has exceeded recursion depth limit. " << global.fatal;
  }
  if (this->isListOfTwoAtomsStartingWith(this->owner->opBind())) {
    return true;
  }
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].hasBoundVariables()) {
      return true;
    }
  }
  return false;
}

bool Calculator::innerIsInteger(Calculator& calculator, const Expression& input, Expression& output) {
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (argument.hasBoundVariables()) {
    return false;
  }
  if (argument.isInteger()) {
    output.assignValue(1, calculator);
  } else {
    output.assignValue(0, calculator);
  }
  return true;
}

bool Calculator::innerIsRational(Calculator& calculator, const Expression& input, Expression& output) {
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (argument.hasBoundVariables()) {
    return false;
  }
  if (argument.isRational()) {
    output.assignValue(1, calculator);
  } else {
    output.assignValue(0, calculator);
  }
  return true;
}

bool Calculator::appendOpandsReturnTrueIfOrderNonCanonical(
  const Expression& input, List<Expression>& output, int theOp
) {
  RecursionDepthCounter recursionCounter(&this->recursionDepth);
  if (this->recursionDepth > this->maximumRecursionDepth) {
    return false;
  }
  bool result = false;
  if (!input.isListStartingWithAtom(theOp)) {
    output.addOnTop(input);
  } else {
    for (int i = 1; i < input.size(); i ++) {
      if (this->appendOpandsReturnTrueIfOrderNonCanonical(input[i], output, theOp)) {
        result = true;
      }
      if (i < input.size() - 1 && input[i].isListStartingWithAtom(theOp) && input[i].size() > 2) {
        result = true;
      }
    }
  }
  return result;
}

bool CalculatorBasics::tensorProductStandard(Calculator& calculator, const Expression& input, Expression& output) {
  RecursionDepthCounter theRecursionIncrementer(&calculator.recursionDepth);
  MacroRegisterFunctionWithName("Calculator::outerTensorProductStandard");
  if (CalculatorBasics::distribute(calculator, input, output, calculator.opPlus(), calculator.opTensor())) {
    return true;
  }
  if (CalculatorBasics::associate(calculator, input, output)) {
    return true;
  }
  if (CalculatorBasics::extractBaseMultiplication(calculator, input, output)) {
    return true;
  }
  return false;
}

bool CalculatorBasics::multiplyAtoXtimesAtoYequalsAtoXplusY(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerMultiplyAtoXtimesAtoYequalsAtoXplusY");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  Expression constPower, thePower;
  const Expression* left = &input[1];
  const Expression* right = &input[2];
  if (*left == *right) {
    constPower.assignValue(2, calculator);
    output.makeXOX(calculator, calculator.opThePower(), *left, constPower);
    return true;
  }
  for (int i = 0; i < 2; i ++, MathRoutines::swap(left, right)) {
    if (right->startsWith(calculator.opThePower(), 3)) {
      if ((*right)[1] == (*left)) {
        bool isGood = true;
        if ((*right)[2].isOfType<Rational>()) {
          if (!(*right)[2].getValue<Rational>().isInteger()) {
            Rational ratBase;
            if ((*right)[1].isRational(&ratBase)) {
              if (ratBase > 0) {
                isGood = false;
              }
            }
          }
        }
        if (isGood) {
          thePower.makeXOX(calculator, calculator.opPlus(), (*right)[2], calculator.expressionOne());
          return output.makeXOX(calculator, calculator.opThePower(), *left, thePower);
        }
      }
      if (left->startsWith(calculator.opThePower(), 3)) {
        if ((*left)[1] == (*right)[1]) {
          bool isGood = (*left)[2].isInteger() || (*right)[2].isInteger();
          if (!isGood) {
            isGood = (*left)[1].isKnownToBeNonNegative();
          }
          if (!isGood) {
            continue;
          }
          Rational leftRat, rightRat;
          if (!isGood) {
            if ((*left)[2].isOfType<Rational>(&leftRat) && (*right)[2].isOfType<Rational>(&rightRat)) {
              if (leftRat.isInteger() && !rightRat.isInteger()) {
                isGood = true;
              }
              if (!leftRat.isInteger() && rightRat.isInteger()) {
                isGood = true;
              }
            }
          }
          if (!isGood) {
            continue;
          }
          thePower.makeXOX(calculator, calculator.opPlus(), (*left)[2], (*right)[2]);
          return output.makeXOX(calculator, calculator.opThePower(), (*left)[1], thePower);
        }
      }
    }
  }
  return false;
}

bool CalculatorBasics::combineFractions(Calculator& calculator, const Expression& input, Expression& output) {
  if (!input.startsWith(calculator.opPlus(), 3)) {
    return false;
  }
  const Expression* quotientE = nullptr;
  const Expression* summandE = nullptr;
  if (input[1].startsWith(calculator.opDivide(), 3)) {
    quotientE = &input[1];
    summandE = &input[2];
  } else if (input[2].startsWith(calculator.opDivide(), 3)) {
    quotientE = &input[2];
    summandE = &input[1];
  } else {
    return false;
  }
  const Expression& quotientNumeratorE = (*quotientE)[1];
  const Expression& quotientDenominatorE = (*quotientE)[2];
  Expression outputSummandE, outputNumE;
  outputSummandE.makeXOX(calculator, calculator.opTimes(), *summandE, quotientDenominatorE);
  outputNumE.makeXOX(calculator, calculator.opPlus(), quotientNumeratorE, outputSummandE);
  return output.makeXOX(calculator, calculator.opDivide(), outputNumE, quotientDenominatorE);
}

bool CalculatorBasics::checkRule(Calculator& calculator, const Expression& input, Expression& output) {
  if (!input.startsWith(calculator.opDefine(), 3)) {
    return false;
  }
  if (input[1] != input[2]) {
    return false;
  }
  std::stringstream out;
  out << "Bad rule: you are asking me to substitute " << input[1] << " by itself ("
  << input[2] << ")" << " which is an infinite substitution cycle. ";
  return output.makeError(out.str(), calculator);
}

bool CalculatorBasics::subZeroDivAnythingWithZero(Calculator& calculator, const Expression& input, Expression& output) {
  if (!input.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  if (input[1].isEqualToZero()) {
    if (!input[2].isEqualToZero()) {
      return output.assignValue(0, calculator);
    }
  }
  return false;
}

bool CalculatorBasics::cancelMultiplicativeInverse(Calculator& calculator, const Expression& input, Expression& output) {
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  if (!input[1].startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  if (input[1][2] == input[2]) {
    output = input[1][1];
    return true;
  }
  return false;
}

bool CalculatorBasics::associateTimesDivision(Calculator& calculator, const Expression& input, Expression& output) {
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  if (!input[2].startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  Expression newLeftE;
  newLeftE.makeXOX(calculator, calculator.opTimes(), input[1], input[2][1]);
  output.makeXOX(calculator, calculator.opDivide(), newLeftE, input[2][2]);
  return true;
}

bool CalculatorBasics::associate(Calculator& calculator, const Expression& input, Expression& output) {
  if (input.size() != 3) {
    return false;
  }
  int operation = input[0].theData;
  if (!input[1].startsWith(operation) && !input[2].startsWith(operation)) {
    return false;
  }
  List<Expression> multiplicands;
  calculator.collectOpands(input, operation, multiplicands);
  Expression result;
  result.makeOXdotsX(calculator, operation, multiplicands);
  if (result == input) {
    return false;
  }
  output = result;
  return true;
}

bool CalculatorBasics::standardIsDenotedBy(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::standardIsDenotedBy");
  RecursionDepthCounter theRecursionIncrementer(&calculator.recursionDepth);
  if (!input.startsWith(calculator.opIsDenotedBy(), 3)) {
    return false;
  }
  const Expression& withNotation = input[2];
  const Expression& theNotation = input[1];
  calculator << "<br>Registering notation: globally, " << withNotation.toString() << " will be denoted by "
  << theNotation.toString();
  calculator.theObjectContainer.expressionNotation.addOnTop(theNotation.toString());
  calculator.theObjectContainer.expressionWithNotation.addOnTop(withNotation);
  output = input;
  output.setChildAtomValue(0, calculator.opDefine());
  ////
  if (withNotation.isOfType<ElementTensorsGeneralizedVermas<RationalFunction<Rational> > >()) {
    if (withNotation.getValue<ElementTensorsGeneralizedVermas<RationalFunction<Rational> > >().isHWV()) {
      MonomialGeneralizedVerma<RationalFunction<Rational> >& theElt =
      withNotation.getValue<ElementTensorsGeneralizedVermas<RationalFunction<Rational> > >()[0].theMons[0];
      theElt.getOwner().highestWeightVectorNotation = theNotation.toString();
    }
  }
  return true;
}

bool CalculatorBasics::multiplyByOne(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerMultiplyByOne");
  if (!input.isListStartingWithAtom(calculator.opTimes()) || input.size() != 3) {
    return false;
  }
  if (!input[1].isEqualToOne()) {
    return false;
  }
  output = input[2];
  return true;
}

bool Calculator::getVectorLargeIntegerFromFunctionArguments(const Expression& input, List<LargeInteger>& output) {
  MacroRegisterFunctionWithName("Calculator::getVectorLargeIntegerFromFunctionArguments");
  Vector<Rational> theRats;
  if (!this->getVectorFromFunctionArguments(input, theRats)) {
    return false;
  }
  output.initializeFillInObject(theRats.size, 0);
  for (int i = 0; i < theRats.size; i ++) {
    if (!theRats[i].isInteger(&output[i])) {
      return *this << "<hr>Succeeded to convert " << input.toString() << " to the vector of rationals: "
      << theRats.toString() << " but failed to convert that to list of integers. ";
    }
  }
  return true;
}

bool Calculator::getVectorInt(const Expression& input, List<int>& output) {
  MacroRegisterFunctionWithName("Calculator::getVectorInt");
  Vector<Rational> theRats;
  if (!this->getVector(input, theRats)) {
    return false;
  }
  output.initializeFillInObject(theRats.size,0);
  for (int i = 0; i < theRats.size; i ++) {
    if (!theRats[i].isSmallInteger(&output[i])) {
      return *this << "<hr>Succeeded to convert " << input.toString() << " to the vector of rationals: "
      << theRats.toString() << " but failed to convert that to list of small integers. ";
    }
  }
  return true;
}

bool CalculatorBasics::timesToFunctionApplication(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::outerTimesToFunctionApplication");
  if (!input.startsWith(calculator.opTimes())) {
    return false;
  }
  if (input.size() < 2) {
    return false;
  }
  const Expression& firstElt = input[1];
  if (!firstElt.isBuiltInAtom()) {
    if (!firstElt.startsWith(calculator.opThePower(), 3)) {
      return false;
    }
    if (!firstElt[1].isAtomWhoseExponentsAreInterpretedAsFunction()) {
      return false;
    }
  }
  if (firstElt.isAtomNotInterpretedAsFunction()) {
    return false;
  }
  const Expression& secondElt = input[2];
  if (secondElt.isSequenceNElements() || secondElt.startsWith(calculator.opIntervalOpen())) {
    output = secondElt;
    return output.setChild(0, firstElt);
  }
  output = input;
  output.children.removeIndexShiftDown(0);
  return true;
}

bool CalculatorBasics::distribute(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  int theAdditiveOp,
  int theMultiplicativeOp,
  bool constantsOnly
) {
  if (CalculatorBasics::leftDistributeBracketIsOnTheLeft(
    calculator,
    input,
    output,
    theAdditiveOp,
    theMultiplicativeOp,
    constantsOnly
  )) {
    return true;
  }
  return CalculatorBasics::rightDistributeBracketIsOnTheRight(
    calculator,
    input,
    output,
    theAdditiveOp,
    theMultiplicativeOp,
    constantsOnly
  );
}

bool CalculatorBasics::distributeTimes(Calculator& calculator, const Expression& input, Expression& output) {
  return CalculatorBasics::distribute(
    calculator,
    input,
    output,
    calculator.opPlus(),
    calculator.opTimes(),
    false
  );
}

bool CalculatorBasics::distributeTimesConstant(Calculator& calculator, const Expression& input, Expression& output) {
  return CalculatorBasics::distribute(calculator, input, output,calculator.opPlus(), calculator.opTimes(), true);
}

bool CalculatorBasics::leftDistributeBracketIsOnTheLeft(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  int theAdditiveOp,
  int theMultiplicativeOp,
  bool constantsOnly
) {
  MacroRegisterFunctionWithName("CalculatorBasics::leftDistributeBracketIsOnTheLeft");
  if (theAdditiveOp == - 1) {
    theAdditiveOp = calculator.opPlus();
  }
  if (theMultiplicativeOp == - 1) {
    theMultiplicativeOp = calculator.opTimes();
  }
  if (!input.startsWith(theMultiplicativeOp, 3)) {
    return false;
  }
  if (!input[1].startsWith(theAdditiveOp, 3)) {
    return false;
  }
  if (constantsOnly) {
    if (!input[2].isConstantNumber()) {
      return false;
    }
  }
  Expression leftE, rightE;
  leftE.makeXOX(calculator, theMultiplicativeOp, input[1][1], input[2]);
  rightE.makeXOX(calculator, theMultiplicativeOp, input[1][2], input[2]);
  return output.makeXOX(calculator, theAdditiveOp, leftE, rightE);
}

bool CalculatorBasics::rightDistributeBracketIsOnTheRight(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  int theAdditiveOp,
  int theMultiplicativeOp,
  bool constantsOnly
) {
  MacroRegisterFunctionWithName("Calculator::outerRightDistributeBracketIsOnTheRight");
  if (theAdditiveOp == - 1) {
    theAdditiveOp = calculator.opPlus();
  }
  if (theMultiplicativeOp == - 1) {
    theMultiplicativeOp = calculator.opTimes();
  }
  if (!input.startsWith(theMultiplicativeOp, 3)) {
    return false;
  }
  if (!input[2].startsWith(theAdditiveOp, 3)) {
    return false;
  }
  if (constantsOnly) {
    if (!input[1].isConstantNumber()) {
      return false;
    }
  }
//  int theFormat = input.format;
  Expression leftE, rightE;
  leftE.makeXOX(calculator, theMultiplicativeOp, input[1], input[2][1]);
  rightE.makeXOX(calculator, theMultiplicativeOp, input[1], input[2][2]);
  return output.makeXOX(calculator, theAdditiveOp, leftE, rightE);
}

bool Calculator::collectCoefficientsPowersVariables(
  const Expression& input, const Expression& theVariable, VectorSparse<Expression>& outputPositionIiscoeffXtoIth
) {
  MacroRegisterFunctionWithName("Calculator::collectCoefficientsPowersVariables");
  List<Expression> theSummands, currentMultiplicands, remainingMultiplicands;
  Calculator& calculator = *input.owner;
  calculator.collectOpands(input, calculator.opPlus(), theSummands);
  Expression currentCoeff;
  outputPositionIiscoeffXtoIth.makeZero();
  for (int i = 0; i < theSummands.size; i ++) {
    calculator.collectOpands(theSummands[i], calculator.opTimes(), currentMultiplicands);
    bool found = false;
    for (int j = 0; j < currentMultiplicands.size; j ++) {
      const Expression& currentE = currentMultiplicands[j];
      remainingMultiplicands = currentMultiplicands;
      remainingMultiplicands.removeIndexShiftDown(j);
      if (remainingMultiplicands.size == 0) {
        currentCoeff.assignValue(1, calculator);
      } else {
        currentCoeff.makeProduct(calculator, remainingMultiplicands);
      }
      if (currentE == theVariable) {
        outputPositionIiscoeffXtoIth.addMonomial(MonomialVector(1), currentCoeff);
        found = true;
        break;
      }
      if (currentE.startsWith(calculator.opThePower(), 3)) {
        int thePower;
        if (currentE[1] == theVariable) {
          if (currentE[2].isSmallInteger(&thePower)) {
            outputPositionIiscoeffXtoIth.addMonomial(MonomialVector(thePower), currentCoeff);
            found = true;
            break;
          }
        }
      }
    }
    if (!found) {
      outputPositionIiscoeffXtoIth.addMonomial(MonomialVector(0), theSummands[i]);
    }
  }
  return true;
}

bool Calculator::collectOpands(const Expression& input, int theOp, List<Expression>& outputOpands) {
  MacroRegisterFunctionWithName("Calculator::collectOpands");
  outputOpands.setSize(0);
  return this->collectOpandsAccumulate(input, theOp, outputOpands);
}

bool Calculator::collectOpandsAccumulate(
  const Expression& input, int theOp, List<Expression>& outputOpands
) {
  MacroRegisterFunctionWithName("Calculator::collectOpandsAccumulate");
  if (!input.startsWith(theOp)) {
    outputOpands.addOnTop(input);
    return true;
  }
  for (int i = 1; i < input.size(); i ++) {
    this->collectOpandsAccumulate(input[i], theOp, outputOpands);
  }
  return true;
}

bool Calculator::functionCollectSummands(
  Calculator& calculator,
  const Expression& input,
  LinearCombination<Expression, Rational>& outputSum
) {
  MacroRegisterFunctionWithName("Calculator::functionCollectSummands");
  List<Expression> summands;
  calculator.appendSummandsReturnTrueIfOrderNonCanonical(input, summands);
  outputSum.makeZero();
  LinearCombination<Expression, AlgebraicNumber> sumOverAlgebraicNumbers;
  LinearCombination<Expression, double> sumOverDoubles;
  Rational coeffRat = 1;
  AlgebraicNumber coeffAlg = 1;
  double coeffDouble = 1;
  bool hasNAN = false;
  for (int i = 0; i < summands.size; i ++) {
    if (summands[i].isEqualToZero()) {
      continue;
    }
    if (summands[i].startsWith(calculator.opTimes(), 3)) {
      if (summands[i][1].isOfType<Rational>(&coeffRat)) {
        outputSum.addMonomial(summands[i][2], coeffRat);
        continue;
      } else if (summands[i][1].isOfType<AlgebraicNumber>(&coeffAlg)) {
        if (coeffAlg.isRational(&coeffRat)) {
          outputSum.addMonomial(summands[i][2], coeffRat);
          continue;
        }
        sumOverAlgebraicNumbers.addMonomial(summands[i][2], coeffAlg);
        continue;
      } else if (summands[i][1].isOfType<double>(&coeffDouble)) {
        sumOverDoubles.addMonomial(summands[i][2], coeffDouble);
        if (std::isnan(coeffDouble)) {
          hasNAN = true;
        }
        continue;
      }
    }
    if (summands[i].isRational(&coeffRat)) {
      outputSum.addMonomial(calculator.expressionOne(), coeffRat);
    } else {
      outputSum.addMonomial(summands[i], 1);
    }
  }
  if (!sumOverDoubles.isEqualToZero() && !hasNAN) {
    sumOverDoubles.quickSortDescending();
    Expression doubleSum;
    doubleSum.makeSum(calculator, sumOverDoubles);
    outputSum.addMonomial(doubleSum, 1);
  }
  if (!sumOverAlgebraicNumbers.isEqualToZero()) {
    sumOverAlgebraicNumbers.quickSortDescending();
    Expression algebraicSum;
    algebraicSum.makeSum(calculator, sumOverAlgebraicNumbers);
    outputSum.addMonomial(algebraicSum, 1);
  }
  outputSum.quickSortDescending();
  return !hasNAN;
}

bool CalculatorBasics::associateExponentExponent(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerAssociateExponentExponent");
  int opPower = calculator.opThePower();
  if (!input.startsWith(opPower, 3)) {
    return false;
  }
  if (!input[1].startsWith(opPower, 3)) {
    return false;
  }
  bool isGood = false;
  if (input[1][1].isKnownToBeNonNegative()) {
    isGood = true;
  }
  if (input[2].isInteger()) {
    isGood = true;
  }
  if (!isGood) {
    Rational powerInner, powerOuter;
    if (input[2].isRational(&powerOuter) && input[1][2].isRational(&powerInner)) {
      if ((powerInner * powerOuter).isEven()) {
        isGood = true;
      }
    }
  }
  if (!isGood) {
    return false;
  }
  Expression tempE;
  tempE.makeProduct(calculator, input[1][2], input[2]);
  output.makeXOX(calculator, opPower, input[1][1], tempE);
  return true;
}

bool Calculator::outerPowerRaiseToFirst(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::outerPowerRaiseToFirst");
  if (!input.startsWith(calculator.opThePower(), 3)) {
    return false;
  }
  if (
    input[1].startsWith(calculator.opIntegral(), 2) ||
    input[1].isOperationGiven(calculator.opIntegral())
  ) {
    return false;
  }
  if (input[2].isEqualToOne()) {
    output = input[1];
    return true;
  }
  return false;
}

bool Expression::makeXOXOdotsOX(Calculator& owner, int theOp, const List<Expression>& input) {
  MacroRegisterFunctionWithName("Expression::makeXOXOdotsOX");
  if (input.size == 0) {
    global.fatal
    << "This is a programming error: cannot "
    << "create operation sequence from an empty list. " << global.fatal;
  }
  if (input.size == 1) {
    *this = input[0];
    return true;
  }
  this->makeXOX(owner, theOp, input[input.size - 2], *input.lastObject());
  for (int i = input.size - 3; i >= 0; i --) {
    this->makeXOX(owner, theOp, input[i], *this);
  }
  return true;
}

bool Expression::makeAtom(int input, Calculator& newBoss) {
  this->reset(newBoss);
  this->theData = input;
  return true;
}

bool Expression::makeIdentityMatrixExpressions(int theDim, Calculator& inputBoss) {
  Matrix<Expression> theMat;
  theMat.makeIdentityMatrix(theDim, inputBoss.expressionOne(), inputBoss.expressionZero());
  return this->assignMatrixExpressions(theMat, inputBoss, false, true);
}

bool Calculator::outerPlus(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::outerPlus");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.startsWith(calculator.opPlus())) {
    return false;
  }
  LinearCombination<Expression, Rational> theSum;
  if (!calculator.functionCollectSummands(calculator, input, theSum)) {
    return false;
  }
  theSum.quickSortDescending();
  if (theSum.size() < 5) {
    for (int i = 0; i < theSum.size(); i ++) {
      for (int j = i; j < theSum.size(); j ++) {
        if (theSum[i] > theSum[j] && theSum[j] > theSum[i]) {
          global.fatal << "Faulty comparison: "
          << theSum[i].toString() << " and " << theSum[j].toString()
          << " are mutually greater than one another. " << global.fatal;
        }
      }
    }
  }
  output.makeSum(calculator, theSum);
  if (output == input) {
    return false;
  }
  return true;
}

bool CalculatorBasics::evaluateIf(Calculator& calculator, const Expression& input, Expression& output) {
  if (!input.startsWith(calculator.opDefineConditional(), 4)) {
    return output.makeError("Error: operation :if = takes three arguments.", calculator);
  }
  Rational conditionRat;
  if (!input[1].isOfType<Rational>(&conditionRat)) {
    return false;
  }
  if (conditionRat.isEqualToOne()) {
    output = input[2];
    return true;
  }
  if (conditionRat.isEqualToZero()) {
    output = input[3];
    return true;
  }
  return false;
}

bool CalculatorBasics::minus(Calculator& calculator, const Expression& input, Expression& output) {
  if (!(input.startsWith(calculator.opMinus(), 3) || input.startsWith(calculator.opMinus(), 2))) {
    return false;
  }
  Expression tempE, minusOne;
  minusOne.assignValue(- 1, calculator);
  if (input.size() == 2) {
    output.makeXOX(calculator, calculator.opTimes(), minusOne, input[1]);
  } else {
    tempE.makeXOX(calculator, calculator.opTimes(), minusOne, input[2]);
    output.makeXOX(calculator, calculator.opPlus(), input[1], tempE);
  }
  return true;
}

void Expression::operator+=(const Expression& other) {
  MacroRegisterFunctionWithName("Expression::operator+=");
  if (this->owner == nullptr && other.owner == nullptr) {
    this->theData += other.theData;
    if (this->theData != 1 && this->theData != 0) {
      global.fatal << "Attempting to add non-initialized expressions" << global.fatal;
    }
    return;
  }
  if (other.owner == nullptr) {
    Expression otherCopy;
    otherCopy.assignValue(other.theData, *this->owner);
    *this += otherCopy;
    return;
  }
  if (this->owner == nullptr) {
    this->assignValue(this->theData, *other.owner);
  }
  if (this->owner != other.owner) {
    global.fatal << "Error: adding expressions with different owners. " << global.fatal;
  }
  Expression resultE;
  resultE.makeXOX(*this->owner, this->owner->opPlus(), *this, other);
  *this = resultE;
}

void Expression::operator-=(const Expression& other) {
  MacroRegisterFunctionWithName("Expression::operator+=");
  if (this->owner == nullptr && other.owner == nullptr) {
    this->theData -= other.theData;
    if (this->theData != 1 && this->theData != 0) {
      global.fatal << "Attempt to subtract non-initialized expressions. " << global.fatal;
    }
    return;
  }
  if (other.owner == nullptr) {
    Expression otherCopy;
    otherCopy.assignValue(other.theData, *this->owner);
    (*this) -= otherCopy;
    return;
  }
  if (this->owner == nullptr) {
    this->assignValue(this->theData, *other.owner);
  }
  if (this->owner != other.owner) {
    global.fatal << "Error: attempt to add expressions with different owners. " << global.fatal;
  }
  Expression resultE;
  resultE.makeXOX(*this->owner, this->owner->opMinus(), *this, other);
  *this = resultE;
}

Expression Expression::operator*(const Expression& other) {
  MacroRegisterFunctionWithName("Expression::operator*");
  Expression result;
  result = *this;
  result *= other;
  return result;
}

Expression Expression::operator*(int other) {
  MacroRegisterFunctionWithName("Expression::operator*");
  Expression result;
  if (this->owner == nullptr) {
    // perhaps we should allow the code below for convenience: really
    // hard to judge if the convenience is worth it, or whether it will cause hard-to-detect bugs.
    // Rational resultRat = this->theData;
    // resultRat*= other;
    // if (resultRat.isSmallInteger(&result.theData))
    //  return result;
    global.fatal << "Multiplying non-initialized expression with data: "
    << this->theData << " by integer " << other << " is not allowed. "
    << global.fatal;
  }
  Expression otherE;
  otherE.assignValue(other, *this->owner);
  result = *this;
  result *= otherE;
  return result;
}

Expression Expression::operator/(int other) {
  MacroRegisterFunctionWithName("Expression::operator/");
  Expression result;
  if (this->owner == nullptr) {
    global.fatal << "Multiplying non-initialized expression with data: "
    << this->theData << " by integer " << other
    << " is not allowed. "
    << global.fatal;
  }
  Expression otherE;
  otherE.assignValue(other, *this->owner);
  result = *this;
  result /= otherE;
  return result;
}

Expression Expression::operator+(const Expression& other) {
  Expression result;
  result = *this;
  result += other;
  return result;
}

Expression Expression::operator-(const Expression& other) {
  Expression result;
  result = *this;
  result -= other;
  return result;
}

Expression Expression::operator/(const Expression& other) {
  MacroRegisterFunctionWithName("Expression::operator/");
  Expression result;
  result = *this;
  result /= other;
  return result;
}

void Expression::operator/=(const Expression& other) {
  MacroRegisterFunctionWithName("Expression::operator/=");
  if (this->owner == nullptr && other.owner == nullptr) {
    this->theData /= other.theData;
    if (this->theData != 1 && this->theData != 0) {
      global.fatal << "Attempting to divide non-initialized expressions. " << global.fatal;
    }
    return;
  }
  if (other.owner == nullptr) {
    Expression otherCopy;
    otherCopy.assignValue(other.theData, *this->owner);
    (*this) /= otherCopy;
    return;
  }
  if (this->owner == nullptr) {
    this->assignValue(this->theData, *other.owner);
  }
  if (this->owner != other.owner) {
    global.fatal << "Error: dividing expressions with different owners. " << global.fatal;
  }
  Expression resultE;
  resultE.makeXOX(*this->owner, this->owner->opDivide(), *this, other);
  *this = resultE;
}

void Expression::operator*=(const Expression& other) {
  MacroRegisterFunctionWithName("Expression::operator*=");
  if (this->owner == nullptr && other.owner == nullptr) {
    this->theData *= other.theData;
    if (this->theData != 1 && this->theData != 0) {
      global.fatal << "Attempting to add non-initialized expressions. " << global.fatal;
    }
    return;
  }
  if (other.owner == nullptr) {
    Expression otherCopy;
    otherCopy.assignValue(other.theData, *this->owner);
    (*this) *= otherCopy;
    return;
  }
  if (this->owner == nullptr) {
    this->assignValue(this->theData, *other.owner);
  }
  if (this->owner != other.owner) {
    global.fatal << "Error: adding expressions with different owners. " << global.fatal;
  }
  Expression resultE;
  resultE.makeXOX(*this->owner, this->owner->opTimes(), *this, other);
  *this = resultE;
}

bool Expression::operator==(const Expression& other) const {
  if (this->owner != other.owner) {
    return false;
  }
  return this->theData == other.theData && this->children == other.children;
}

bool Expression::isEqualToMathematically(const Expression& other) const {
  MacroRegisterFunctionWithName("Expression::isEqualToMathematically");
  if (this->owner == nullptr && other.owner == nullptr) {
    return this->theData == other.theData;
  }
  if (this->owner == nullptr) {
    return false;
  }
  if (*this == other) {
    return true;
  }
  Rational theRat, theRatTwo;
  AlgebraicNumber theAlgebraic;
  if (this->isOfType(&theRat) && other.isOfType(&theRatTwo)) {
    return theRat == theRatTwo;
  }
  if (this->isOfType(&theRat) && other.isOfType(&theAlgebraic)) {
    return theAlgebraic == theRat;
  }
  if (other.isOfType(&theRat) && this->isOfType(&theAlgebraic)) {
    return theAlgebraic == theRat;
  }
  double leftD = - 1, rightD = - 1;
  if (this->evaluatesToDouble(&leftD) && other.evaluatesToDouble(&rightD)) {
    return (leftD - rightD == 0.0);
  }
  Expression differenceE = *this;
  differenceE -= other;
  Expression differenceEsimplified;
  if (!this->owner->evaluateExpression(*this->owner, differenceE, differenceEsimplified)) {
    return false;
  }
  if (differenceEsimplified.isEqualToZero()) {
    return true;
  }
  if (differenceEsimplified.isSequenceNElements()) {
    for (int i = 1; i < differenceEsimplified.size(); i ++) {
      if (!differenceEsimplified[i].isEqualToZero()) {
        return false;
      }
    }
    return true;
  }
  if (this->size() != other.size()) {
    return false;
  }
  if (this->size() == 0) {
    return this->theData == other.theData;
  }
  for (int i = 0; i < this->size(); i ++) {
    if (!(*this)[i].isEqualToMathematically(other[i])) {
      return false;
    }
  }
  return true;
}

SemisimpleLieAlgebra* Expression::getAmbientSemisimpleLieAlgebraNonConstUseWithCaution() const {
  this->checkInitialization();
  ExpressionContext myContext = this->getContext();
  int indexSSalg = myContext.indexAmbientSemisimpleLieAlgebra;
  if (indexSSalg == - 1) {
    return nullptr;
  }
  return &this->owner->theObjectContainer.semisimpleLieAlgebras.theValues[indexSSalg];
}

Function& Calculator::getFunctionHandlerFromNamedRule(const std::string& inputNamedRule) {
  const Calculator::NamedRuleLocation& current =
  this->namedRules.getValueNoFail(inputNamedRule);
  const MemorySaving<Calculator::OperationHandlers>& currentOperation =
  this->operations.getValueNoFail(current.containerOperation);
  if (currentOperation.isZeroPointer()) {
    global.fatal << "Named rule " << inputNamedRule
    << " registered with operation " << current.containerOperation
    << " but the operation has no handlers. " << global.fatal;
  }
  if (current.isComposite) {
    return currentOperation.getElementConst().compositeHandlers[current.index];
  }
  return currentOperation.getElementConst().handlers[current.index];
}

int Calculator::addOperationNoRepetitionOrReturnIndexFirst(const std::string& theOpName) {
  int result = this->operations.getIndex(theOpName);
  if (result == - 1) {
    result = this->operations.size();
    this->operations.getValueCreate(theOpName);
  }
  return result;
}

void Calculator::addOperationBuiltInType(const std::string& theOpName) {
  this->addOperationNoRepetitionAllowed(theOpName);
  this->builtInTypes.addOnTop(theOpName);
}

void Calculator::addOperationNoRepetitionAllowed(const std::string& theOpName) {
  if (this->operations.contains(theOpName)) {
    global.fatal << "This is a programming error: operation "
    << theOpName << " already created. " << global.fatal;
  }
  this->operations.getValueCreate(theOpName);
}

Function::Function() {
  this->owner = nullptr;
  this->resetExceptOwner();
}

Function::Function(
  Calculator& inputOwner,
  int inputIndexOperation,
  const Expression::FunctionAddress& functionPointer,
  Expression* inputArgTypes,
  const std::string& description,
  const std::string& inputExample,
  const std::string& inputAdditionalIndentifier,
  const std::string& inputCalculatorIdentifier,
  const Options& inputOptions,
  int inputIndexParentThatBansHandler
) {
  this->owner = nullptr;
  if (&this->options == &inputOptions) {
    global.fatal << "Input options and target "
    << "options have the same address. " << global.fatal;
  }
  this->reset(inputOwner);
  this->indexOperation = inputIndexOperation;
  this->options = inputOptions;
  this->theFunction = functionPointer;
  this->theDescription = description;
  this->theExample = inputExample;
  this->additionalIdentifier = inputAdditionalIndentifier;
  this->calculatorIdentifier = inputCalculatorIdentifier;
  if (inputArgTypes != nullptr) {
    this->theArgumentTypes = *inputArgTypes;
  }
  this->indexOperationParentThatBansHandler = inputIndexParentThatBansHandler;
}

void Calculator::addOperationBinaryInnerHandlerWithTypes(
  const std::string& theOpName,
  Expression::FunctionAddress innerHandler,
  int leftType,
  int rightType,
  const std::string& opDescription,
  const std::string& opExample,
  const std::string& inputAdditionalIdentifier,
  const std::string& inputCalculatorIdentifier,
  const Function::Options& options
) {
  int indexOp = this->operations.getIndex(theOpName);
  if (indexOp == - 1) {
    indexOp = this->operations.size();
    this->operations.getValueCreate(theOpName);
  }
  Function innerFunction(
    *this,
    indexOp,
    innerHandler,
    nullptr,
    opDescription,
    opExample,
    inputAdditionalIdentifier,
    inputCalculatorIdentifier,
    options,
    - 1
  );
  innerFunction.theArgumentTypes.reset(*this, 2);
  innerFunction.theArgumentTypes.addChildAtomOnTop(leftType);
  innerFunction.theArgumentTypes.addChildAtomOnTop(rightType);
  this->registerCalculatorFunction(innerFunction, indexOp);
}

void Calculator::registerCalculatorFunction(Function& theFun, int indexOp) {
  MacroRegisterFunctionWithName("Calculator::registerCalculatorFunction");
  if (indexOp < 0 || indexOp >= this->operations.size()) {
    global.fatal << "Invalid index operation: " << indexOp
    << ", there are: " << this->operations.size()
    << " operations total." << global.fatal;
  }
  MemorySaving<Calculator::OperationHandlers>& handlerPointer = this->operations.theValues[indexOp];
  Calculator::OperationHandlers& handler = handlerPointer.getElement();
  handler.checkConsistency();
  if (theFun.options.flagIsCompositeHandler) {
    theFun.indexInOperationHandlers = handler.compositeHandlers.size;
    handler.compositeHandlers.addOnTop(theFun);
    if (handler.compositeHandlers.size <= 0) {
      global.fatal << "Composite handlers cannot be empty. " << global.fatal;
    }
  } else {
    theFun.indexInOperationHandlers = handler.handlers.size;
    handler.handlers.addOnTop(theFun);
    if (handler.handlers.size <= 0) {
      global.fatal << "Handlers cannot be empty. " << global.fatal;
    }
  }
  if (theFun.calculatorIdentifier == "") {
    return;
  }
  Calculator::NamedRuleLocation namedRule;
  namedRule.containerOperation = this->operations.theKeys[indexOp];
  namedRule.index = theFun.indexInOperationHandlers;
  namedRule.isComposite = theFun.options.flagIsCompositeHandler;
  this->namedRules.setKeyValue(
    theFun.calculatorIdentifier,
    namedRule
  );
}

void Calculator::addOperationHandler(
  const std::string& theOpName,
  Expression::FunctionAddress handler,
  const std::string& opArgumentListIgnoredForTheTimeBeing,
  const std::string& opDescription,
  const std::string& opExample,
  const std::string& inputAdditionalIdentifier,
  const std::string& inputCalculatorIdentifier,
  const Function::Options& options,
  const std::string& parentOpThatBansHandler
) {
  if (opArgumentListIgnoredForTheTimeBeing != "") {
    global.fatal << "This section of code is not implemented yet. Crashing to let you know. " << global.fatal;
  }
  int indexOp = this->operations.getIndex(theOpName);
  if (indexOp == - 1) {
    indexOp = this->operations.size();
    this->operations.getValueCreate(theOpName);
  }
  int indexParentOpThatBansHandler = this->operations.getIndex(parentOpThatBansHandler);
  Function theFun(
    *this,
    indexOp,
    handler,
    nullptr,
    opDescription,
    opExample,
    inputAdditionalIdentifier,
    inputCalculatorIdentifier,
    options,
    indexParentOpThatBansHandler
  );
  if (theFun.theFunction == nullptr || theFun.owner == nullptr) {
    global.fatal << "Function not initialized properly. " << global.fatal;
  }
  this->registerCalculatorFunction(theFun, indexOp);
}

void Function::Options::reset() {
  this->flagIsCompositeHandler    = false;
  this->flagIsInner               = true ;
  this->flagMayActOnBoundVars     = false;
  this->visible                   = true ;
  this->flagIsExperimental        = false;
  this->disabledByUser            = false;
  this->disabledByUserDefault     = false;
  this->dontTestAutomatically = false;
  this->adminOnly                 = false;
}

Function::Options::Options() {
  this->reset();
}

bool Function::inputFitsMyInnerType(const Expression& input) {
  if (!this->options.flagIsInner) {
    return false;
  }
  if (this->theArgumentTypes.children.size != 2) {
    return true;
  }
  if (input.children.size != 3) {
    return false;
  }
  bool argument1good = this->theArgumentTypes[0].theData == - 1 ? true : input[1].isListStartingWithAtom(this->theArgumentTypes[0].theData);
  bool argument2good = this->theArgumentTypes[1].theData == - 1 ? true : input[2].isListStartingWithAtom(this->theArgumentTypes[1].theData);
  return argument1good && argument2good;
}

std::string Function::ToStringShort() const {
  if (this->owner == nullptr) {
    return "(non-initialized)";
  }
  std::stringstream out;
  out << this->owner->operations.theKeys[this->indexOperation];
  MemorySaving<Calculator::OperationHandlers>& handlerPointer = this->owner->operations.theValues[this->indexOperation];
  Calculator::OperationHandlers& handler = handlerPointer.getElement();
  if (this->options.flagIsCompositeHandler) {
    out << " (composite) ("
    << this->indexInOperationHandlers + 1 << " out of "
    << handler.compositeHandlers.size << ") ";
  } else {
    out << "(" << this->indexInOperationHandlers + 1 << " out of "
    << handler.handlers.size << "). ";
  }
  return out.str();
}

std::string Function::toStringSummary() const {
  if (this->owner == nullptr) {
    return "(non-initialized)";
  }
  std::stringstream out;
  out << this->ToStringShort();
  if (this->calculatorIdentifier != "") {
    out << "Rule name: <span style ='color:blue'>" << this->calculatorIdentifier << "</span>. ";
  }
  if (this->additionalIdentifier != "") {
    out << "Handler: " << this->additionalIdentifier << ". ";
  }
  return out.str();
}

bool Function::shouldBeApplied(int parentOpIfAvailable) {
  if (this->options.disabledByUser) {
    return false;
  }
  bool parentIsGood = true;
  if (parentOpIfAvailable >= 0 && this->indexOperationParentThatBansHandler >= 0) {
    parentIsGood = (this->indexOperationParentThatBansHandler != parentOpIfAvailable);
  }
  if (!parentIsGood) {
    return false;
  }
  if (this->options.adminOnly) {
    if (!global.userDefaultHasAdminRights()) {
      return (*this->owner) << "Rule " << this->calculatorIdentifier
      << " requires logged-in administrator account. ";
    }
  }
  return true;
}

JSData Function::toJSON() const {
  MacroRegisterFunctionWithName("Function::toJSON");
  JSData result;
  result.theType = JSData::token::tokenObject;
  if (this->owner == nullptr) {
    result[WebAPI::result::error] = "bad_owner";
    return result;
  }
  if (this->options.visible) {
    result["visible"] = "true";
  } else {
    result["visible"] = "false";
  }
  Calculator::OperationHandlers& operationHandlers = this->owner->operations.theValues[this->indexOperation].getElement();
  result["number"] = this->indexInOperationHandlers + 1;
  if (this->options.flagIsCompositeHandler) {
    result["composite"] = "true";
    result["total"] = operationHandlers.compositeHandlers.size;
    result["atom"] = this->owner->operations.theKeys[this->indexOperation];
  } else {
    result["composite"] = "false";
    result["total"] = operationHandlers.handlers.size;
    result["atom"] = this->owner->operations.theKeys[this->indexOperation];
  }
  if (this->options.flagIsExperimental) {
    result["experimental"] = "true";
  } else {
    result["experimental"] = "false";
  }
  result["description"] = this->theDescription;
  if (this->calculatorIdentifier != "") {
    result["ruleName"] = this->calculatorIdentifier;
  }
  if (this->additionalIdentifier != "") {
    result["additionalIdentifier"] = this->additionalIdentifier;
  }
  std::stringstream functionAddress;
  functionAddress << std::hex << reinterpret_cast<unsigned long>(this->theFunction);
  result["memoryAddress"] = functionAddress.str();
  if (this->options.flagIsInner) {
    result["inner"] = "true";
  } else {
    result["inner"] = "false";
  }
  if (this->theExample != "") {
    result["example"] = this->theExample;
  }
  return result;
}

std::string Function::toStringFull() const {
  if (!this->options.visible) {
    return "";
  }
  if (this->owner == nullptr) {
    return "(non-intialized)";
  }
  std::stringstream out2;
  out2 << this->ToStringShort();
  if (!this->options.flagIsExperimental) {
    std::stringstream out;
    out << this->theDescription;
    if (this->calculatorIdentifier != "") {
      out << "Rule name: " << this->calculatorIdentifier << ". ";
    }
    if (this->additionalIdentifier != "") {
      out << "Handler: " << this->additionalIdentifier << ". ";
    }
    // use of unsigned long is correct on i386 and amd64
    // uintptr_t is only available in c++ 0x
    // Please fix if the following code is not portable:
    out << "Function memory address: " << std::hex << reinterpret_cast<unsigned long>(this->theFunction) << ". ";
    if (!this->options.flagIsInner) {
      out << "This is a <b>``law''</b> - substitution takes place only if output expression is different from input. ";
    }
    if (this->theExample != "") {
      out << " <br> " << this->theExample << "&nbsp&nbsp&nbsp";
    }
    if (this->theExample != "") {
      out2 << "<a href=\"" << global.displayNameExecutable
      << "?request=calculator&showExamples = true&mainInput="
      << HtmlRoutines::convertStringToURLString(this->theExample, false)
      << "\"> " << " Example" << "</a>" ;
    }
  } else {
    out2 << "<b>Experimental, please don't use.</b>";
  }
  return out2.str();
}

std::string ObjectContainer::toString() {
  MacroRegisterFunctionWithName("ObjectContainer::toString");
  std::stringstream out;
  if (this->semisimpleLieAlgebras.theValues.size > 0) {
    out << "Lie algebras created (" << this->semisimpleLieAlgebras.theValues.size << " total): ";
    for (int i = 0; i < this->semisimpleLieAlgebras.theValues.size; i ++) {
      out << this->semisimpleLieAlgebras.theValues[i].toStringLieAlgebraName();
      if (i != this->semisimpleLieAlgebras.theValues.size - 1) {
        out << ", ";
      }
    }
  }
  if (this->theSSSubalgebraS.theValues.size > 0) {
    out << "<br>Lie semisimple subalgebras computation data structures ("
    << this->theSSSubalgebraS.theValues.size << " total): ";
    for (int i = 0; i < this->theSSSubalgebraS.theValues.size; i ++) {
      out << " Type " << this->theSSSubalgebraS.theValues[i].owner->toStringLieAlgebraName() << " with "
      << this->theSSSubalgebraS.theValues[i].theSubalgebras.theValues.size << " candidates";
      if (i != this->theSSSubalgebraS.theValues.size - 1) {
        out << ", ";
      }
    }
  }
  return out.str();
}

JSData Calculator::toJSONOutputAndSpecials() {
  MacroRegisterFunctionWithName("Calculator::toJSONOutputAndSpecials");
  JSData result = this->outputJS;
  if (this->inputString == "") {
    return result;
  }
  result[WebAPI::result::performance] = this->toJSONPerformance();
  result[WebAPI::result::parsingLog] = this->parsingLog;
  return result;
}

std::string Calculator::toStringOutputAndSpecials() {
  MacroRegisterFunctionWithName("Calculator::toStringOutputAndSpecials");
  return this->toJSONOutputAndSpecials().toString(nullptr);
}

void Calculator::writeAutoCompleteKeyWordsToFile() {
  MacroRegisterFunctionWithName("Calculator::writeAutoCompleteKeyWordsToFile");
  std::stringstream out;
  out << "\"use strict\"; //This file is automatically generated, please do not modify.\n";
  out << "  var theAutocompleteDictionary = [\n  ";
  for (int i = 0; i < this->autoCompleteKeyWords.size; i ++) {
    if (this->autoCompleteKeyWords[i].size() > 2) {
      out << "\"" << HtmlRoutines::convertStringEscapeNewLinesQuotesBackslashes(this->autoCompleteKeyWords[i]) << "\"";
      if (i != this->autoCompleteKeyWords.size - 1) {
        out << ", ";
      }
    }
  }
  out << "];\n";
  out << "module.exports = {theAutocompleteDictionary};";
  std::fstream theFileStream;
  FileOperations::openFileCreateIfNotPresentVirtual(
    theFileStream, "/calculator-html/autocomplete_keywords.js",
    false, true, false, false
  );
  theFileStream << out.str();
}

void Calculator::computeAutoCompleteKeyWords() {
  MacroRegisterFunctionWithName("Calculator::computeAutoCompleteKeyWords");
  this->autoCompleteKeyWords.setExpectedSize(this->operations.size() * 2);
  for (int i = 0; i < this->operations.size(); i ++) {
    this->autoCompleteKeyWords.addOnTopNoRepetition(this->operations.theKeys[i]);
  }
  for (int i = 0; i < this->namedRules.size(); i ++) {
    this->autoCompleteKeyWords.addOnTopNoRepetition(this->namedRules.theKeys[i]);
  }
  for (int i = 0; i < this->controlSequences.size; i ++) {
    if (this->controlSequences[i].size() > 0) {
      if (MathRoutines::isALatinLetter(this->controlSequences[i][0])) {
        autoCompleteKeyWords.addOnTopNoRepetition(this->controlSequences[i]);
      }
    }
  }
}

JSData Calculator::toJSONPerformance() {
  MacroRegisterFunctionWithName("Calculator::toStringPerformance");
  int64_t elapsedMilliseconds = global.getElapsedMilliseconds();
  int64_t computationMilliseconds = elapsedMilliseconds - this->statistics.startTimeEvaluationMilliseconds;
  int64_t requestMilliseconds = elapsedMilliseconds - global.millisecondsComputationStart;
  int64_t waitingMilliseconds = elapsedMilliseconds - requestMilliseconds;
  JSData result;
  std::stringstream moreDetails;
  moreDetails << "<br>Max computation time soft: "
  << (static_cast<double>(global.millisecondsMaxComputation) / 2000)
  << " s [calculator error when exceeded]. ";
  moreDetails << "<br>Max computation time hard: "
  << (static_cast<double>(global.millisecondsMaxComputation) / 1000) << " s [worker crash when exceeded]. ";
  moreDetails << "<br>Total process request time: "
  << requestMilliseconds << " ms (~"
  << (static_cast<double> (requestMilliseconds) / 1000)
  << " s). ";
  moreDetails << "<br>Time waiting on open connection: "
  << waitingMilliseconds << " ms (~"
  << (static_cast<double>(waitingMilliseconds) / 1000)
  << " s).";
  moreDetails << "<br>Expressions generated: " << this->allChildExpressions.size << ". ";
  moreDetails << "<br>Expressions evaluated: " << this->statistics.expressionsEvaluated << ". ";
  moreDetails << "<br>Total number of pattern matches performed: "
  << this->totalPatternMatchesPerformed << "";
  if (this->depthRecursionReached > 0) {
    moreDetails << "<br>maximum recursion depth reached: " << this->depthRecursionReached << ".";
  }
  moreDetails << "<br>Lists created: " << "computation: "
  << (GlobalStatistics::numListsCreated - static_cast<unsigned>(this->statistics.numberOfListsStart))
  << ", total: " << GlobalStatistics::numListsCreated;
  moreDetails << "<br> # List resizes: computation: "
  << (GlobalStatistics::numListResizesTotal - static_cast<unsigned>(this->statistics.numberListResizesStart))
  << ", total: " << GlobalStatistics::numListResizesTotal
  << "<br> # hash resizing: computation: "
  << (GlobalStatistics::numHashResizes - static_cast<unsigned>(this->statistics.numberHashResizesStart))
  << ", total: " << GlobalStatistics::numHashResizes;
  if (Rational::totalSmallAdditions > 0) {
    moreDetails << "<br>Small rational additions: computation: "
    << Rational::totalSmallAdditions - static_cast<unsigned long long>(this->statistics.numberOfSmallAdditionsStart)
    << ", total: " << Rational::totalSmallAdditions;
  }
  if (Rational::totalSmallMultiplications > 0) {
    moreDetails << "<br>Small rational multiplications: computation: "
    << Rational::totalSmallMultiplications - static_cast<unsigned long long>(this->statistics.numberOfSmallMultiplicationsStart)
    << ", total: " << Rational::totalSmallMultiplications;
  }
  if (Rational::totalSmallGreatestCommonDivisors > 0) {
    moreDetails << "<br>Small gcd calls: computation: "
    << Rational::totalSmallGreatestCommonDivisors - static_cast<unsigned long long>(this->statistics.numberOfSmallGreatestCommonDivisorsStart)
    << ", total: " << Rational::totalSmallGreatestCommonDivisors;
  }
  if (Rational::totalLargeAdditions > 0) {
    moreDetails << "<br>Large integer additions: "
    << Rational::totalLargeAdditions - static_cast<unsigned long long>(this->statistics.numberOfLargeAdditionsStart)
    << ", total: "
    << Rational::totalLargeAdditions;
  }
  if (Rational::totalLargeMultiplications > 0) {
    moreDetails << "<br>Large integer multiplications: computation: "
    << Rational::totalLargeMultiplications - static_cast<unsigned long long>(this->statistics.numberOfLargeMultiplicationsStart)
    << ", total: " << Rational::totalLargeMultiplications;
  }
  if (Rational::totalLargeGreatestCommonDivisors > 0) {
    moreDetails << "<br>Large gcd calls: "
    << Rational::totalLargeGreatestCommonDivisors - static_cast<unsigned long long>(this->statistics.numberOfLargeGreatestCommonDivisorsStart)
    << ", total: " << Rational::totalLargeGreatestCommonDivisors;
  }
  std::stringstream millisecondsStream;
  millisecondsStream << computationMilliseconds << " ms";
  result[WebAPI::result::computationTime] = millisecondsStream.str();
  result[WebAPI::result::comments] = moreDetails.str();
  return result;
}

std::string Calculator::toString() {
  MacroRegisterFunctionWithName("Calculator::toString");
  std::stringstream out2;
  std::string openTag1 = "<span style =\"color:blue\">";
  std::string closeTag1 = "</span>";
  if (global.millisecondsMaxComputation > 0) {
    out2 << "Computation time limit: "
    << global.millisecondsMaxComputation
    << " ms.<hr>";
  } else {
    out2 << "No computation time limit.<hr> ";
  }
  if (this->ruleStack.children.size > 1) {
    out2 << "<b>Predefined rules.</b><br>";
    for (int i = 1; i < this->ruleStack.children.size; i ++) {
      out2 << this->ruleStack[i].toString();
      if (i != this->ruleStack.children.size - 1) {
        out2 << "<br>";
      }
    }
  }
  if (!global.userDebugFlagOn()) {
    return out2.str();
  }
  std::stringstream out;
  out2 << "<hr><b>Further calculator details.</b>";
  out
  << "<br><b>Object container information</b>. "
  << "The object container is the data "
  << "structure storing all c++ built-in data types "
  << "requested by the user<br> " << this->theObjectContainer.toString();
  out << "<hr>Control sequences (" << this->controlSequences.size
  << " total):\n<br>\n";
  for (int i = 0; i < this->controlSequences.size; i ++) {
    out << openTag1 << this->controlSequences[i] << closeTag1;
    if (i != this->controlSequences.size) {
      out << ", ";
    }
  }
  out << "<br>\n User or run-time defined atoms = " << this->operations.size() << " (= "
  << this->numberOfPredefinedAtoms << " predefined + "
  << this->operations.size() - this->numberOfPredefinedAtoms << " user-defined):<br>\n";
  for (int i = 0; i < this->operations.size(); i ++) {
    out << "\n" << i << ": " << openTag1 << this->operations.theKeys[i] << closeTag1;
    if (i != this->operations.size() - 1) {
      out << ", ";
    }
  }
  out << "<hr>";
  out << "Children expressions (" << this->allChildExpressions.size << " total): <br>";
  int numExpressionsToDisplay = this->allChildExpressions.size;
  if (this->allChildExpressions.size > 1000) {
    numExpressionsToDisplay = 1000;
    out << " <b>Displaying first " << numExpressionsToDisplay << " only </b><br>";
  }
  for (int i = 0; i < numExpressionsToDisplay; i ++) {
    out << this->allChildExpressions[i].toString() << ", ";
  }
  out << "<hr>";
  out << "\n Cached expressions (" << this->cachedExpressions.size << " total):\n<br>\n";
  numExpressionsToDisplay = this->cachedExpressions.size;
  if (numExpressionsToDisplay > 1000) {
    numExpressionsToDisplay = 1000;
    out << "<b>Displaying first " << numExpressionsToDisplay << " expressions only. </b><br>";
  }
  for (int i = 0; i < numExpressionsToDisplay; i ++) {
    out << this->cachedExpressions[i].toString() << " -> " << this->imagesCachedExpressions[i].toString();
    if (i != this->cachedExpressions.size - 1) {
      out << "<br>";
    }
  }
  out2 <<
    out.str();
  return out2.str();
}

std::string Calculator::toStringSyntacticStackHumanReadable(
  bool includeLispifiedExpressions, bool ignoreCommandEnclosures
) {
  MacroRegisterFunctionWithName("Calculator::toStringSyntacticStackHumanReadable");
  std::stringstream out;
  if ((*this->currentSyntacticStack).size< this->numEmptyTokensStart) {
    return
    "Error: this is a programming error: "
    "not enough empty tokens in the start of the syntactic stack.";
  }
  bool isBad = ((*this->currentSyntacticStack).size > this->numEmptyTokensStart + 1);
  SyntacticElement& lastSyntacticElt = *(*this->currentSyntacticStack).lastObject();
  if ((*this->currentSyntacticStack).size == this->numEmptyTokensStart + 1) {
    if (lastSyntacticElt.controlIndex != this->conExpression()) {
      isBad = true;
    }
  }
  if (!isBad) {
    out << this->currentSyntacticStack->lastObject()->toStringHumanReadable(*this, includeLispifiedExpressions);
    return out.str();
  }
  out << "<table style =\"vertical-align:top;border-spacing:0px 0px;\"><tr>";
  int counter = 0;
  for (int i = this->numEmptyTokensStart; i < (*this->currentSyntacticStack).size; i ++) {
    SyntacticElement& currentElt = (*this->currentSyntacticStack)[i];
    if (ignoreCommandEnclosures) {
      if (
        currentElt.controlIndex == this->conExpression() ||
        currentElt.controlIndex == this->conSequenceStatements()
      ) {
        if (currentElt.theData.startsWith(this->opCommandEnclosure())) {
          continue;
        }
        if (currentElt.theData.startsWith(this->opEndStatement())) {
          if (currentElt.theData.size() >= 2) {
            if (currentElt.theData[1].startsWith(this->opCommandEnclosure())) {
              continue;
            }
          }
        }
      }
    }
    out
    << "<td style =\"vertical-align:top;background-color:"
    << ((counter % 2 == 0) ? "#FAFAFA" : "#F0F0F0" ) << "\">"
    << currentElt.toStringHumanReadable(*this, includeLispifiedExpressions)
    << "</td>";
    counter ++;
  }
  out << "</tr></table>";
  return out.str();
}

std::string Calculator::toStringSyntacticStackHTMLTable(bool ignoreCommandEnclosures) {
  return this->toStringSyntacticStackHumanReadable(true, ignoreCommandEnclosures);
}

SemisimpleSubalgebras& ObjectContainer::getSemisimpleSubalgebrasCreateIfNotPresent(const DynkinType& input) {
  MacroRegisterFunctionWithName("ObjectContainer::getSemisimpleSubalgebrasCreateIfNotPresent");
  SemisimpleSubalgebras& currentSAs = this->theSSSubalgebraS.getValueCreateNoInit(input);
  return currentSAs;
}

SemisimpleLieAlgebra& ObjectContainer::getLieAlgebraCreateIfNotPresent(const DynkinType& input) {
  MacroRegisterFunctionWithName("ObjectContainer::getLieAlgebraCreateIfNotPresent");
  bool needToInit = false;
  if (!this->semisimpleLieAlgebras.contains(input)) {
    needToInit = true;
  }
  SemisimpleLieAlgebra& theLA = this->semisimpleLieAlgebras.getValueCreateNoInit(input);
  if (needToInit) {
    this->semisimpleLieAlgebraPointers.addOnTop(&theLA);
    theLA.theWeyl.makeFromDynkinType(input);
  }
  return theLA;
}

WeylGroupData& ObjectContainer::getWeylGroupDataCreateIfNotPresent(const DynkinType& input) {
  MacroRegisterFunctionWithName("ObjectContainer::getWeylGroupDataCreateIfNotPresent");
  return this->getLieAlgebraCreateIfNotPresent(input).theWeyl;
}

std::string ObjectContainer::toStringJavascriptForUserInputBoxes() {
  std::stringstream out;
  out << "<script>\n";
  out << "window.calculator.calculator.inputBoxNames = [";
  for (int i = 0; i < this->theUserInputTextBoxesWithValues.size(); i ++) {
    InputBox& currentBox = this->theUserInputTextBoxesWithValues.theValues[i];
    out << "'" << currentBox.name << "'";
    if (i != this->theUserInputTextBoxesWithValues.size() - 1) {
      out << ", ";
    }
  }
  out << "];\n";
  out << "window.calculator.calculator.inputBoxToSliderUpdaters = {};";
  for (int i = 0; i < this->theUserInputTextBoxesWithValues.size(); i ++) {
    InputBox& currentBox = this->theUserInputTextBoxesWithValues.theValues[i];
    out << "window.calculator.calculator.inputBoxToSliderUpdaters['"
    << currentBox.name << "'] ='"
    << currentBox.getSliderName() << "';\n";
  }
  //out << "console.log(window.calculator.calculator.inputBoxNames);\n ";
  out << "</script>";
  return out.str();
}

void ObjectContainer::resetPlots() {
  this->canvasPlotCounter = 0;
  for (int i = 0; i < this->thePlots.size; i ++) {
    this->thePlots[i].flagDivAlreadyDisplayed = false;
  }
}

void ObjectContainer::resetSliders() {
  this->userInputBoxSliderDisplayed.initializeFillInObject(this->theUserInputTextBoxesWithValues.size(), false);
}

bool ObjectContainer::checkConsistencyAfterReset() {
  MacroRegisterFunctionWithName("ObjectContainer::checkConsistencyAfterReset");
  if (this->theWeylGroupElements.size != 0) {
    global.fatal << "WeylGroupElements expected to be empty, got "
    << this->theWeylGroupElements.size << " elements. " << global.fatal;
  }
  if (this->semisimpleLieAlgebras.size() != 0) {
    global.fatal << "theSSLieAlgebras expected to be empty, got "
    << this->semisimpleLieAlgebras.size() << " elements. " << global.fatal;
  }
  // MapReferences<DynkinType, SemisimpleSubalgebras> theSSSubalgebraS;
  if (this->theWeylGroupReps.size != 0) {
    global.fatal << "theWeylGroupReps expected to be empty, got "
    << this->theWeylGroupReps.size << " elements. " << global.fatal;
  }
  if (this->theWeylGroupVirtualReps.size != 0) {
    global.fatal << "theWeylGroupVirtualReps expected to be empty, got "
    << this->theWeylGroupVirtualReps.size << " elements. " << global.fatal;
  }
  // ListReferences<ModuleSSalgebra<RationalFunctionOld> > theCategoryOmodules;
  // ListReferences<SltwoSubalgebras> theSltwoSAs;
  // HashedListReferences<ElementEllipticCurve<ElementZmodP> > ellipticCurveElementsZmodP;
  // HashedListReferences<ElementEllipticCurve<Rational> > ellipticCurveElementsRational;
  // HashedListReferences<ElementTensorsGeneralizedVermas<RationalFunctionOld> > theTensorElts;

  if (this->polynomialsRational.size != 0) {
    global.fatal << "The rational polynomials are expected to be empty, have: " << this->polynomialsRational.size << " elements instead. " << global.fatal;
  }
  // HashedListReferences<Polynomial<AlgebraicNumber> > polynomialsAlgebraic;
  // HashedListReferences<ElementWeylAlgebra<Rational> > theWeylAlgebraElements;
  // HashedListReferences<ElementUniversalEnveloping<RationalFunctionOld> > theUEs;
  // HashedListReferences<RationalFunctionOld> theRFs;
  if (this->theRationals.size != 0) {
    global.fatal << "Rationals expected to be empty, have: " << this->theRationals.size << " elements instead. " << global.fatal;
  }
  // HashedListReferences<CharacterSemisimpleLieAlgebraModule<Rational> > theCharsSSLieAlgFD;
  if (this->theDoubles.size != 1) {
    global.fatal << "Doubles expected to be have exactly 1 element (namely, nan), have: " << this->theDoubles.size << " elements instead. " << global.fatal;
  }
  // HashedListReferences<std::string, MathRoutines::hashString> theStrings;
  // HashedListReferences<std::string, MathRoutines::hashString> expressionNotation;
  // HashedListReferences<Expression> expressionWithNotation;
  // HashedListReferences<LittelmannPath> theLSpaths;
  // HashedListReferences<MatrixTensor<Rational> > theMatTensorRats;
  // HashedListReferences<ElementZmodP> theEltsModP;
  // HashedListReferences<Weight<Rational> > theWeights;
  // HashedListReferences<Weight<Polynomial<Rational> > > theWeightsPoly;
  // ListReferences<GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational > > theHyperoctahedralReps;
  // ListReferences<Plot> thePlots;
  // List<bool> userInputBoxSliderDisplayed;
  // MapReferences<std::string, InputBox, MathRoutines::hashString> theUserInputTextBoxesWithValues;
  // MapReferences<std::string, std::string, MathRoutines::hashString> graphicsScripts;
  // AlgebraicClosureRationals theAlgebraicClosure;
  if (this->theAlgebraicNumbers.size != 0) {
    global.fatal << "Algebraic numbers expected to be empty, have: " << this->theAlgebraicNumbers.size << " elements instead. " << global.fatal;
  }
  // HashedListReferences<ElementHyperoctahedralGroupR2> theElementsHyperOctGroup;
  // ListReferences<HyperoctahedralGroupData> theHyperOctahedralGroups;
  // HashedListReferences<MonomialTensor<int, MathRoutines::IntUnsignIdentity> > theLittelmannOperators;
  // WeylGroupData& getWeylGroupDataCreateIfNotPresent(const DynkinType& input);
  // SemisimpleLieAlgebra& getLieAlgebraCreateIfNotPresent(const DynkinType& input);
  // SemisimpleSubalgebras& getSemisimpleSubalgebrasCreateIfNotPresent(const DynkinType& input);
  return true;
}

void ObjectContainer::reset() {
  MacroRegisterFunctionWithName("ObjectContainer::reset");
  this->theWeylGroupElements.clear();
  this->theWeylGroupReps.clear();
  this->theWeylGroupVirtualReps.clear();
  this->theCategoryOmodules.setSize(0);
  this->semisimpleLieAlgebras.clear();
  this->semisimpleLieAlgebraPointers.clear();
  this->theSSSubalgebraS.clear();
  this->theTensorElts.clear();
  this->polynomialsRational.clear();
  this->polynomialsAlgebraic.clear();
  this->polynomialsModular.clear();
  this->theWeylAlgebraElements.clear();
  this->theUEs.clear();
  this->rationalFunctions.clear();
  this->rationalFunctionsModular.clear();
  this->theRationals.clear();
  this->theCharsSSLieAlgFD.clear();
  this->theDoubles.clear();
  this->theDoubles.addOnTop(std::nan(""));
  this->theStrings.clear();
  this->expressionNotation.clear();
  this->expressionWithNotation.clear();
  this->theLSpaths.clear();
  this->theMatTensorRats.clear();
  this->theEltsModP.clear();
  this->thePlots.setSize(0);
  this->theAlgebraicClosure.reset();
  this->theAlgebraicNumbers.clear();
  this->theLittelmannOperators.clear();
  this->theSltwoSAs.setSize(0);
  this->theWeights.clear();
  this->theWeightsPoly.clear();
  this->theHyperOctahedralGroups.setSize(0);
  this->theElementsHyperOctGroup.clear();
  this->pseudoRandom.setRandomSeedSmall(static_cast<uint32_t>(time(nullptr)));
  this->theUserInputTextBoxesWithValues.clear();
  this->graphicsScripts.clear();
  this->ellipticCurveElementsZmodP.clear();
  this->ellipticCurveElementsRational.clear();
  this->canvasPlotCounter = 0;
  this->resetPlots();
  this->resetSliders();
}

template <>
bool CalculatorConversions::functionPolynomial<Rational>(Calculator& calculator, const Expression& input, Expression& output);

bool Calculator::innerFreudenthalFull(Calculator& calculator, const Expression& input, Expression& output) {
  Vector<Rational> hwFundamental, hwSimple;
  Selection tempSel;
  WithContext<SemisimpleLieAlgebra*> theSSalg;
  if (!calculator.getTypeHighestWeightParabolic<Rational>(
    calculator, input, output, hwFundamental, tempSel, theSSalg, nullptr
  )) {
    return output.makeError("Failed to extract highest weight and algebra", calculator);
  }
  if (output.isError()) {
    return true;
  }
  if (tempSel.cardinalitySelection > 0) {
    return output.makeError("Failed to extract highest weight. ", calculator);
  }
  CharacterSemisimpleLieAlgebraModule<Rational> startingChar, resultChar;
  hwSimple = theSSalg.content->theWeyl.getSimpleCoordinatesFromFundamental(hwFundamental);
  startingChar.makeFromWeight(hwSimple, theSSalg.content);
  std::string reportString;
  if (!startingChar.freudenthalEvaluateMeFullCharacter(resultChar, 10000, &reportString)) {
    return output.makeError(reportString, calculator);
  }
  std::stringstream out;
  out << resultChar.toString();
  return output.assignValue(out.str(), calculator);
}

bool Calculator::innerFreudenthalFormula(Calculator& calculator, const Expression& input, Expression& output) {
  Vector<Rational> hwFundamental, hwSimple;
  Selection tempSel;
  WithContext<SemisimpleLieAlgebra*> theSSalg;
  if (!calculator.getTypeHighestWeightParabolic<Rational>(
    calculator, input, output, hwFundamental, tempSel, theSSalg, nullptr
  )) {
    return output.makeError("Failed to extract highest weight and algebra", calculator);
  }
  if (output.isError()) {
    return true;
  }
  if (tempSel.cardinalitySelection > 0) {
    return output.makeError("Failed to extract highest weight. ", calculator);
  }
  CharacterSemisimpleLieAlgebraModule<Rational> startingChar, resultChar;
  hwSimple = theSSalg.content->theWeyl.getSimpleCoordinatesFromFundamental(hwFundamental);
  startingChar.makeFromWeight(hwSimple, theSSalg.content);
  std::string reportString;
  if (!startingChar.freudenthalEvalMeDominantWeightsOnly(resultChar, 10000, &reportString)) {
    return output.makeError(reportString, calculator);
  }
  return output.assignValue(resultChar, calculator);
}

bool Expression::isMeltable(int* numResultingChildren) const {
  this->checkInitialization();
  if (!this->startsWith(this->owner->opMelt(), 2)) {
    return false;
  }
  if (numResultingChildren != nullptr) {
    if (!(*this)[1].startsWith(this->owner->opEndStatement())) {
      *numResultingChildren = 1;
    } else {
      *numResultingChildren = (*this)[1].children.size - 1;
    }
  }
  return true;
}

bool Expression::mergeContextsMyAruments(
  Expression& output, std::stringstream* commentsOnFailure
) const {
  MacroRegisterFunctionWithName("Expression::mergeContextsMyAruments");
  this->checkInitialization();
  if (this->size() < 2) {
    return false;
  }
  for (int i = 1; i < this->size(); i ++) {
    if (!(*this)[i].isBuiltInType()) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "<hr>Failed to merge the arguments of the expression "
        << this->toString() << ": the argument "
        << (*this)[i].toString() << " is not of built-in type";
      }
      return false;
    }
  }
  ExpressionContext commonContext = (*this)[1].getContext();
  bool needsMerge = false;
  for (int i = 2; i < this->size(); i ++) {
    if (!(commonContext == (*this)[i].getContext())) {
      needsMerge = true;
      break;
    }
  }
  if (!needsMerge) {
    output = *this;
    return true;
  }
  for (int i = 2; i < this->size(); i ++) {
    if (!(*this)[i].isBuiltInType()) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "<hr>Failed to merge contexts of arguments: "
        << "an argument is not of built-in type. ";
      }
      return false;
    }
    ExpressionContext currentContext = (*this)[i].getContext();
    if (!commonContext.mergeContexts(currentContext, commonContext)) {
      *this->owner << "<hr>Failed to merge context " << commonContext.toString()
      << " with " << (*this)[i].getContext().toString();
      return false;
    }

  }
  output.reset(*this->owner, this->size());
  output.addChildOnTop((*this)[0]);
  Expression convertedE;
  for (int i = 1; i < this->size(); i ++) {
    convertedE = (*this)[i];
    if (!convertedE.setContextAtLeastEqualTo(commonContext, commentsOnFailure)) {
      *this->owner << "<hr>Failed to convert "
      << convertedE.toString() << " to context " << commonContext.toString();
      return false;
    }
    output.addChildOnTop(convertedE);
  }
  return true;
}

bool Calculator::convertExpressionsToCommonContext(
  List<Expression>& inputOutputEs, ExpressionContext* inputOutputStartingContext
) {
  MacroRegisterFunctionWithName("Calculator::convertExpressionsToCommonContext");
  ExpressionContext commonContext(*this);
  if (inputOutputStartingContext != nullptr) {
    commonContext = *inputOutputStartingContext;
  }
  for (int i = 0; i < inputOutputEs.size; i ++) {
    if (!inputOutputEs[i].isBuiltInType()) {
      return
      *this << "<hr>Possible programming error: "
      << "calling convertExpressionsToCommonContext "
      << "on expressions without context. "
      << global.fatal.getStackTraceEtcErrorMessageHTML();
    }
    if (!commonContext.mergeContexts(inputOutputEs[i].getContext(), commonContext)) {
      return *this << "<hr>Failed to merge context "
      << commonContext.toString() << " with "
      << inputOutputEs[i].getContext().toString();
    }
  }
  for (int i = 0; i < inputOutputEs.size; i ++) {
    if (!inputOutputEs[i].setContextAtLeastEqualTo(commonContext, nullptr)) {
      return false;
    }
  }
  if (inputOutputStartingContext != nullptr) {
    *inputOutputStartingContext = commonContext;
  }
  return true;
}

bool CalculatorBasics::meltBrackets(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::outerMeltBrackets");
  RecursionDepthCounter theCounter(&calculator.recursionDepth);
  if (!input.startsWith(calculator.opEndStatement())) {
    return false;
  }
  int tempInt;
  int ChildIncrease = 0;
  bool found = false;
  for (int i = 0; i < input.children.size; i ++) {
    const Expression& currentChild = input[i];
    if (currentChild.isMeltable(&tempInt)) {
      found = true;
      ChildIncrease += tempInt - 1;
    }
  }
  if (!found) {
    return false;
  }
  output.reset(calculator, input.children.size + ChildIncrease);
  output.addChildAtomOnTop(calculator.opEndStatement());
  for (int i = 1; i < input.children.size; i ++) {
    const Expression& currentChild = input[i];
    if (!currentChild.isMeltable()) {
      output.addChildOnTop(input[i]);
      continue;
    }
    if (!currentChild[1].startsWith(calculator.opEndStatement())) {
      output.addChildOnTop(currentChild[1]);
      continue;
    }
    for (int j = 1; j < currentChild[1].children.size; j ++) {
      output.addChildOnTop(currentChild[1][j]);
    }
  }
  return true;
}
