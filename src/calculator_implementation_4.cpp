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

#include <cmath>
#include <cfloat>

static ProjectInformationInstance projectInfoCalculatorParserCPP(__FILE__, "Calculator parser, implementation. ");

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

std::string Calculator::WriteDefaultLatexFileReturnHtmlLink(
  const std::string& fileContent,
  std::string* outputFileNameNoExtension,
  bool useLatexDviPSpsToPNG
) {
  std::fstream theFile;
  std::stringstream fileName;
  std::stringstream systemCommand1, systemCommand2, systemCommand3;
  fileName << "defaultOutput" << MathRoutines::HashString(fileContent);
  if (!FileOperations::OpenFileCreateIfNotPresentVirtual(
    theFile, global.DisplayPathOutputFolder + fileName.str() + ".tex", false, true, false
  )) {
    return "failed to create file: " + fileName.str() + ".tex";
  }
  std::string baseFolder;
  std::string fileNameWithPathPhysical;
  FileOperations::GetPhysicalFileNameFromVirtual("output/", baseFolder, false, false, nullptr);
  FileOperations::GetPhysicalFileNameFromVirtual(
    "output/" + fileName.str(), fileNameWithPathPhysical, false, false, nullptr
  );
  theFile << fileContent;
  theFile.flush();
  theFile.close();
  systemCommand1 << " latex -output-directory=" << baseFolder << " " << fileName.str() << ".tex";
  global.CallSystemNoOutput(systemCommand1.str(), true);
  if (useLatexDviPSpsToPNG) {
    systemCommand2 << " dvips -o " << fileNameWithPathPhysical << ".ps "
    << fileNameWithPathPhysical << ".dvi";
    global.CallSystemNoOutput(systemCommand2.str(), true);
    systemCommand3 << " convert " << fileNameWithPathPhysical
    << ".ps " << fileNameWithPathPhysical << ".png";
    global.CallSystemNoOutput(systemCommand3.str(), true);
  }
  std::stringstream out;
  out << "<img src =\"" << global.DisplayPathOutputFolder
  << fileName.str() << ".png\"></img><a href=\""
  << global.DisplayPathOutputFolder
  << fileName.str() << ".tex\">" << global.DisplayPathOutputFolder
  << fileName.str() << ".tex</a>";
  if (outputFileNameNoExtension != nullptr) {
    *outputFileNameNoExtension = global.DisplayPathOutputFolder + fileName.str();
  }
  this->numOutputFileS ++;
  return out.str();
}

int IntIdentity(const int& x) {
  return x;
}

bool Calculator::GetVectorExpressions(const Expression& input, List<Expression>& output, int targetDimNonMandatory) {
  MacroRegisterFunctionWithName("Calculator::GetVectorExpressions");
  output.Reserve(input.size());
  output.SetSize(0);
  if (!input.IsSequenceNElementS() && !input.StartsWith(this->opIntervalOpen())) {
    if (targetDimNonMandatory > 0) {
      if (targetDimNonMandatory != 1) {
        return *this << "<hr>GetVector failure: target dim is "
        << targetDimNonMandatory << " but the input " << input.ToString()
        << " can only be interpreted as a single element";
      }
    }
    output.AddOnTop(input);
    return true;
  }
  if (targetDimNonMandatory > 0) {
    if (targetDimNonMandatory != input.size() - 1) {
      return *this << "<hr>Failed to GetVector: the input is required to have "
      << targetDimNonMandatory << " columns but it has "
      << input.size() - 1 << " columns instead. <hr>";
    }
  }
  targetDimNonMandatory = input.size() - 1;
  for (int i = 0; i < targetDimNonMandatory; i ++) {
    output.AddOnTop(input[i + 1]);
  }
  return true;
}

template <class coefficient>
bool ModuleSSalgebra<coefficient>::IsNotInParabolic(int theGeneratorIndex) {
  Vector<Rational> theWeight = this->GetOwner().GetWeightOfGenerator(theGeneratorIndex);
  for (int j = 0; j < this->parabolicSelectionNonSelectedAreElementsLevi.CardinalitySelection; j ++) {
    if (!(theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[j]] < 0)) {
      return true;
    }
  }
  return false;
}

template <class coefficient>
bool ModuleSSalgebra<coefficient>::IsNotInLevi(int theGeneratorIndex) {
  Vector<Rational> theWeight = this->GetOwner().GetWeightOfGenerator(theGeneratorIndex);
  for (int j = 0; j < this->parabolicSelectionNonSelectedAreElementsLevi.CardinalitySelection; j ++) {
    if (!theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[j]].IsEqualToZero()) {
      return true;
    }
  }
  return false;
}

template <class coefficient>
void ModuleSSalgebra<coefficient>::GetGenericUnMinusElt(
  bool shiftPowersByNumVarsBaseField,
  ElementUniversalEnveloping<RationalFunctionOld>& output,
  bool useNilWeight,
  bool ascending
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra::GetGenericUnMinusElt");
  List<ElementUniversalEnveloping<coefficient> > eltsNilrad;
  this->GetElementsNilradical(eltsNilrad, true, useNilWeight, ascending);
  RationalFunctionOld tempRF;
  output.MakeZero(*this->theAlgebras, this->indexAlgebra);
  MonomialUniversalEnveloping<RationalFunctionOld> tempMon;
  tempMon.MakeConst(*this->theAlgebras, this->indexAlgebra);
  int varShift = 0;
  if (shiftPowersByNumVarsBaseField) {
    varShift = this->GetMinNumVars();
  }
  for (int i = 0; i < eltsNilrad.size; i ++) {
    tempRF.MakeOneLetterMoN(i + varShift, 1);
    tempMon.MultiplyByGeneratorPowerOnTheRight(eltsNilrad[i][0].generatorsIndices[0], tempRF);
  }
  tempRF.MakeOne();
  output.AddMonomial(tempMon, tempRF);
}

template <class coefficient>
void ModuleSSalgebra<coefficient>::GetGenericUnMinusElt(
  bool shiftPowersByNumVarsBaseField,
  ElementUniversalEnveloping<Polynomial<Rational> >& output,
  bool useNilWeight,
  bool ascending
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra::GetGenericUnMinusElt");
  List<ElementUniversalEnveloping<coefficient> > eltsNilrad;
  this->GetElementsNilradical(eltsNilrad, true, nullptr, useNilWeight, ascending);
  Polynomial<Rational> tempRF;
  output.MakeZero(*this->owner);
  MonomialUniversalEnveloping<Polynomial<Rational> > tempMon;
  tempMon.MakeOne(*this->owner);
  int varShift = 0;
  if (shiftPowersByNumVarsBaseField) {
    varShift = this->GetMinNumVars();
  }
  int numVars = varShift + eltsNilrad.size;
  for (int i = 0; i < eltsNilrad.size; i ++) {
    tempRF.MakeMonomiaL(i + varShift, 1, 1, numVars);
    tempMon.MultiplyByGeneratorPowerOnTheRight(eltsNilrad[i][0].generatorsIndices[0], tempRF);
  }
  tempRF.MakeOne(numVars);
  output.AddMonomial(tempMon, tempRF);
}

class quasiDiffMon {
  friend std::ostream& operator << (std::ostream& output, const quasiDiffMon& theMon) {
    output << theMon.ToString();
    return output;
  }
  public:
  MonomialWeylAlgebra theWeylMon;
  MonomialMatrix theMatMon;
  static unsigned int HashFunction(const quasiDiffMon& input) {
    return input.theWeylMon.HashFunction() * SomeRandomPrimes[0] + input.theMatMon.HashFunction() * SomeRandomPrimes[1];
  }
  unsigned int HashFunction() const {
    return HashFunction(*this);
  }
  bool IsMonEqualToZero() const {
    return this->theMatMon.IsMonEqualToZero();
  }
  bool operator==(const quasiDiffMon& other) const {
    return this->theWeylMon == other.theWeylMon && this->theMatMon == other.theMatMon;
  }
  void operator=(const quasiDiffMon& other) {
    this->theWeylMon = other.theWeylMon;
    this->theMatMon = other.theMatMon;
  }
  bool operator>(const quasiDiffMon& other) const {
    if (this->theMatMon == other.theMatMon) {
      return this->theWeylMon>other.theWeylMon;
    }
    return this->theMatMon>other.theMatMon;
  }
  std::string ToString(FormatExpressions* theFormat = nullptr) const;
};

std::string quasiDiffMon::ToString(FormatExpressions* theFormat) const {
  std::stringstream out;
  out << this->theWeylMon.ToString(theFormat) << "\\otimes ";
  out << this->theMatMon.ToString(theFormat);
  return out.str();
}

template <class coefficient>
class quasiDiffOp : public MonomialCollection<quasiDiffMon, coefficient> {
public:
  std::string ToString(FormatExpressions* theFormat = nullptr) const;
  void GenerateBasisLieAlgebra(List<quasiDiffOp<coefficient> >& theElts, FormatExpressions* theFormat = nullptr);
  void operator*=(const quasiDiffOp<coefficient>& standsOnTheRight);
  void operator=(const  MonomialCollection<quasiDiffMon, coefficient>& other) {
    this->MonomialCollection<quasiDiffMon, coefficient>::operator=(other);
  }
  void LieBracketMeOnTheRight(const MonomialCollection<quasiDiffMon, coefficient>& standsOnTheRight) {
    quasiDiffOp<coefficient> tempRight;
    tempRight = standsOnTheRight;
    MathRoutines::LieBracket(*this, tempRight, *this);
  }
  void FourierTransformDiffPartOnly(quasiDiffOp<coefficient>& output) const;
  void GetEWAsetMatrixPartsToId(ElementWeylAlgebra<coefficient>& output) const;
};

template <class coefficient>
void quasiDiffOp<coefficient>::FourierTransformDiffPartOnly(quasiDiffOp<coefficient>& output) const {
  if (&output == this) {
    quasiDiffOp<coefficient> thisCopy;
    thisCopy.FourierTransformDiffPartOnly(output);
    return;
  }
  output.MakeZero();
  ElementWeylAlgebra<coefficient> startDO, finalDO;
  quasiDiffMon theMon;
  for (int i = 0; i < this->size(); i ++) {
    startDO.MakeZero();
    startDO.AddMonomial((*this)[i].theWeylMon, this->theCoeffs[i]);
    startDO.FourierTransform(finalDO);
    for (int j = 0; j < finalDO.size(); j ++) {
      theMon.theMatMon = (*this)[i].theMatMon;
      theMon.theWeylMon = finalDO[j];
      output.AddMonomial(theMon, finalDO.theCoeffs[j]);
    }
  }
}

template <class coefficient>
void quasiDiffOp<coefficient>::GetEWAsetMatrixPartsToId(ElementWeylAlgebra<coefficient>& output) const {
  output.MakeZero();
  for (int i = 0; i < this->size(); i ++) {
    output.AddMonomial((*this)[i].theWeylMon, this->theCoeffs[i]);
  }
}

template<class Element>
void MathRoutines::LieBracket(const Element& standsOnTheLeft, const Element& standsOnTheRight, Element& output) {
  if (&standsOnTheLeft == &output || &standsOnTheRight == &output) {
    Element standsOnTheLeftNew, standsOnTheRightNew;
    standsOnTheLeftNew = standsOnTheLeft;
    standsOnTheRightNew = standsOnTheRight;
    MathRoutines::LieBracket(standsOnTheLeftNew, standsOnTheRightNew, output);
    return;
  }
  Element tempE;
  output = standsOnTheLeft;
  output *= standsOnTheRight;
  tempE = standsOnTheRight;
  tempE *= standsOnTheLeft;
  output -= tempE;
}

template <class coefficient>
void quasiDiffOp<coefficient>::GenerateBasisLieAlgebra(
  List<quasiDiffOp<coefficient> >& theElts, FormatExpressions* theFormat
) {
  MacroRegisterFunctionWithName("quasiDiffOp<coefficient>::GenerateBasisLieAlgebra");
  ProgressReport theReport;
  HashedList<quasiDiffMon> bufferMons;
  List< MonomialCollection<quasiDiffMon, coefficient> > theEltsConverted;
  theEltsConverted = theElts;
  this->GaussianEliminationByRows(theEltsConverted);
  quasiDiffOp tempQDO;
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
        << tempQDO.ToString(theFormat) << "<br> with element <br>"
        << theEltsConverted[j].ToString(theFormat) << " to get <br>";
        tempQDO.LieBracketMeOnTheRight(theEltsConverted[j]);
        theReport.Report(report.str());
        report << tempQDO.ToString(theFormat);
        theReport.Report(report.str());
        theEltsConverted.AddOnTop(tempQDO);
        quasiDiffOp::GaussianEliminationByRows(theEltsConverted, 0, &bufferMons);
        numTimesEliminationWasExecuted ++;
        if (!theEltsConverted.LastObject()->IsEqualToZero()) {
          foundNew = true;
        }
        for (int k = theEltsConverted.size - 1; k >= 0; k --) {
          if (theEltsConverted[k].IsEqualToZero()) {
            theEltsConverted.RemoveIndexSwapWithLast(k);
          } else {
            break;
          }
        }
      }
    }
  }
  theElts.SetSize(theEltsConverted.size);
  for (int i = 0; i < theEltsConverted.size; i ++) {
    theElts[i] = theEltsConverted[i];
  }

}

template <class coefficient>
void quasiDiffOp<coefficient>::operator*=(const quasiDiffOp<coefficient>& standsOnTheRight) {
  quasiDiffOp<coefficient> output;
  ElementWeylAlgebra<Rational> leftElt, rightElt;
  quasiDiffMon outputMon;
  output.MakeZero();
  for (int j = 0; j < standsOnTheRight.size(); j ++) {
    rightElt.MakeZero();
    rightElt.AddMonomial(standsOnTheRight[j].theWeylMon, standsOnTheRight.theCoeffs[j]);
    for (int i = 0; i < this->size(); i ++) {
      leftElt.MakeZero();
      leftElt.AddMonomial((*this)[i].theWeylMon, this->theCoeffs[i]);
      outputMon.theMatMon = (*this)[i].theMatMon;
      outputMon.theMatMon *= standsOnTheRight[j].theMatMon;
      leftElt *= rightElt;
      for (int k = 0; k < leftElt.size(); k ++) {
        outputMon.theWeylMon = leftElt[k];
        output.AddMonomial(outputMon, leftElt.theCoeffs[k]);
      }
    }
  }
  *this = output;
}

template <class coefficient>
std::string quasiDiffOp<coefficient>::ToString(FormatExpressions* theFormat) const {
  bool combineWeylPart = true;
  if (theFormat != nullptr) {
    combineWeylPart = theFormat->flagQuasiDiffOpCombineWeylPart;
  }
  if (!combineWeylPart) {
    return this->MonomialCollection<quasiDiffMon, coefficient>::ToString(theFormat);
  }
  MatrixTensor<ElementWeylAlgebra<Rational> > reordered;
  reordered.MakeZero();
  ElementWeylAlgebra<Rational> tempP;
  for (int i = 0; i < this->size(); i ++) {
    const quasiDiffMon& currentMon = (*this)[i];
    tempP.MakeZero();
    tempP.AddMonomial(currentMon.theWeylMon, this->theCoeffs[i]);
    reordered.AddMonomial(currentMon.theMatMon, tempP);
  }
  std::string result = reordered.ToString(theFormat);
  if (result == "0" && this->size() != 0) {
    global.fatal << "This is likely a programming error (crashing at any rate): "
    << "I have a non-zero quasidifferential operator "
    << " with non-properly formatted LaTeX string "
    << this->MonomialCollection<quasiDiffMon, coefficient>::ToString(theFormat)
    << ", however its properly formatted string is 0. "
    << "Probably there is something wrong with the initializations of the monomials of the qdo. " << global.fatal;
  }
  return result;
}

template <class coefficient>
bool ModuleSSalgebra<coefficient>::GetActionEulerOperatorPart(
  const MonomialP& theCoeff, ElementWeylAlgebra<Rational>& outputDO
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra::GetActionEulerOperatorPart");
  int powerMonCoeff = 0;
  ElementWeylAlgebra<Rational> currentMonContribution;
  outputDO.MakeOne();
  for (int i = 0; i < theCoeff.GetMinNumVars(); i ++) {
    if (!theCoeff(i).IsSmallInteger(&powerMonCoeff)) {
      global.fatal << "This is a programming error. "
      << "Getting euler operator part of action on generalized Verma module: "
      << "I have an exponent with non-small integer entry. "
      << global.fatal;
    }
    currentMonContribution.Makexidj(i, i, 0);
    currentMonContribution.RaiseToPower(powerMonCoeff);
    outputDO *= currentMonContribution;
  }
  return true;
}

template <class coefficient>
bool ModuleSSalgebra<coefficient>::GetActionGenVermaModuleAsDiffOperator(
  ElementSemisimpleLieAlgebra<Rational>& inputElt,
  quasiDiffOp<Rational>& output,
  bool useNilWeight,
  bool ascending
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra_CoefficientType::GetActionGenVermaModuleAsDiffOperator");
  List<ElementUniversalEnveloping<coefficient> > eltsNilrad;
  List<int> indicesNilrad;
  this->GetElementsNilradical(eltsNilrad, true, &indicesNilrad, useNilWeight, ascending);
  ElementUniversalEnveloping<Polynomial<Rational> > theGenElt, result;
  this->GetGenericUnMinusElt(true, theGenElt, useNilWeight, ascending);
  result.AssignElementLieAlgebra(inputElt, *this->owner, 1);
  theGenElt.Simplify();
  result *= theGenElt;
  result.Simplify();
  MatrixTensor<Polynomial<Rational> > endoPart, tempMT, idMT;
  idMT.MakeIdSpecial();
  MatrixTensor<RationalFunctionOld> tempMat1;

  int varShift = this->GetMinNumVars();
  ElementWeylAlgebra<Rational> weylPartSummand, exponentContribution, oneIndexContribution,
  eulerOperatorContribution;
  Polynomial<Rational> tempP1, negativeExponentDenominatorContribution, theCoeff;
  quasiDiffMon monQDO, monQDO2;
  Rational tempRat;
  output.MakeZero();
  Rational currentShift;
  for (int i = 0; i < result.size(); i ++) {
    //problemCounter ++;
    const MonomialUniversalEnveloping<Polynomial<Rational> >& currentMon = result[i];
    endoPart = idMT;
    for (int j = currentMon.Powers.size - 1; j >= indicesNilrad.size; j --) {
      int thePower = 0;
      if (!currentMon.Powers[j].IsSmallInteger(&thePower)) {
        return false;
      }
      tempMat1 = this->GetActionGeneratorIndeX(currentMon.generatorsIndices[j]);
      tempMT.MakeZero();
      for (int k = 0; k < tempMat1.size(); k ++) {
        if (tempMat1.theCoeffs[k].expressionType == RationalFunctionOld::typeRationalFunction) {
          return false;
        }
        tempMat1.theCoeffs[k].GetNumerator(tempP1);
        tempMT.AddMonomial(tempMat1[k], tempP1);
      }
      MathRoutines::RaiseToPower(tempMT, thePower, idMT);
      endoPart *= tempMT;
    }
    exponentContribution.MakeOne();
    theCoeff = result.theCoeffs[i];
    for (int j = 0; j < indicesNilrad.size; j ++) {
      currentMon.Powers[j].GetConstantTerm(currentShift);
      ElementWeylAlgebra<Rational>::GetStandardOrderDiffOperatorCorrespondingToNraisedTo(
        currentShift, j + varShift, oneIndexContribution, negativeExponentDenominatorContribution
      );
      exponentContribution *= oneIndexContribution;
      theCoeff.DivideBy(negativeExponentDenominatorContribution, theCoeff, tempP1);
      if (!tempP1.IsEqualToZero()) {
        global.fatal << "This is a mathematical error! "
        << "Something is very wrong with embedding semisimple Lie algebras in Weyl algebras. "
        << global.fatal;
      }
    }
    for (int l = 0; l < theCoeff.size(); l ++) {
      if (!this->GetActionEulerOperatorPart(theCoeff[l], eulerOperatorContribution)) {
        return false;
      }
      weylPartSummand = exponentContribution;
      weylPartSummand *= eulerOperatorContribution;
      weylPartSummand *= theCoeff.theCoeffs[l];
      for (int j = 0; j < weylPartSummand.size(); j ++) {
        for (int k = 0; k < endoPart.size(); k ++) {
          monQDO.theMatMon = endoPart[k];
          monQDO.theWeylMon = weylPartSummand[j];
          Polynomial<Rational>& currentEndoCoeff = endoPart.theCoeffs[k];
          for (int m = 0; m < currentEndoCoeff.size(); m ++) {
            monQDO2 = monQDO;
            monQDO2.theWeylMon.polynomialPart *= currentEndoCoeff[m];
            tempRat = currentEndoCoeff.theCoeffs[m];
            tempRat *= weylPartSummand.theCoeffs[j];
            output.AddMonomial(monQDO2, tempRat);
          }
        }
      }
    }
  }
  return true;
}

bool Calculator::innerWriteGenVermaModAsDiffOperatorInner(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  Vectors<Polynomial<Rational> >& theHws,
  Expression& hwContext,
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
  List<quasiDiffOp<Rational> > theQDOs;
  FormatExpressions theWeylFormat, theUEformat;
  std::stringstream out, latexReport, latexReport2;
  theWeylFormat.MaxLineLength = 40;
  theWeylFormat.flagUseLatex = true;
  theUEformat.MaxLineLength = 20;
  theUEformat.flagUseLatex = true;
  theUEformat.chevalleyGgeneratorLetter = "g";
  theUEformat.chevalleyHgeneratorLetter = "h";
  hwContext.ContextGetFormatExpressions(theUEformat);
  theUEformat.polyDefaultLetter = exponentVariableLetter == nullptr  ? "a" : *exponentVariableLetter;
  theUEformat.MaxLineLength = 178;
  theUEformat.NumAmpersandsPerNewLineForLaTeX = 2;
  theWeylFormat.NumAmpersandsPerNewLineForLaTeX = 2;
  hwContext.ContextGetFormatExpressions(theWeylFormat);
  List<ElementSemisimpleLieAlgebra<Rational> > theGeneratorsItry;
  if (!AllGenerators) {
    for (int j = 0; j < theSSalgebra.GetRank(); j ++) {
      Vector<Rational> ei;
      ei.MakeEi(theSSalgebra.GetRank(), j);
      theGenerator.MakeGGenerator(ei, theSSalgebra);
      theGeneratorsItry.AddOnTop(theGenerator);
      ei.Minus();
      theGenerator.MakeGGenerator(ei, theSSalgebra);
      theGeneratorsItry.AddOnTop(theGenerator);
    }
  } else {
    for (int j = 0; j < theSSalgebra.GetNumGenerators(); j ++) {
      theGenerator.MakeGenerator(j, theSSalgebra);
      theGeneratorsItry.AddOnTop(theGenerator);
    }
  }
  theQDOs.SetSize(theGeneratorsItry.size);
  out << "<table border =\"1\">";
  latexReport << "\\begin{longtable}{rll";
  for (int i = 0; i < theGeneratorsItry.size; i ++) {
    latexReport << "l";
  }
  latexReport << "}\\caption{\\label{tableDiffOps" << selInducing.ToString()
  << "} Differential operators corresponding to actions"
  << " of simple positive generators for the " << selInducing.ToString() << "-parabolic subalgebra.}\\\\<br>";
  List<ModuleSSalgebra<RationalFunctionOld > > theMods;
  theMods.SetSize(theHws.size);
  Vector<RationalFunctionOld> tempV;
  int numStartingVars = hwContext.ContextGetNumContextVariables();
  std::stringstream reportFourierTransformedCalculatorCommands, reportCalculatorCommands;
  long long totalAdditions = 0;
  long long currentAdditions = 0;
  long long totalMultiplications = 0;
  long long currentMultiplications = 0;
  double totalTime = 0, currentTime = 0;
  for (int i = 0; i < theHws.size; i ++) {
    ModuleSSalgebra<RationalFunctionOld>& theMod = theMods[i];
    tempV = theHws[i];
    if (!theMod.MakeFromHW(theSSalgebra, tempV, selInducing, 1, 0, nullptr, true)) {
      return output.MakeError("Failed to create module.", theCommands);
    }
    if (i == 0) {
      theMod.GetElementsNilradical(elementsNegativeNilrad, true, nullptr, useNilWeight, ascending);
      Polynomial<Rational> Pone, Pzero;
      Pone.MakeOne(elementsNegativeNilrad.size + theMod.GetMinNumVars());
      Pzero.MakeZero();
      theMod.GetGenericUnMinusElt(true, genericElt, useNilWeight, ascending);
      theWeylFormat.polyAlphabeT.SetSize(numStartingVars + elementsNegativeNilrad.size);
      theWeylFormat.weylAlgebraLetters.SetSize(numStartingVars + elementsNegativeNilrad.size);
      theUEformat.polyAlphabeT.SetSize(numStartingVars + elementsNegativeNilrad.size);
      for (int k = 0; k < numStartingVars; k ++) {
        theWeylFormat.weylAlgebraLetters[k] = "error";
      }
      std::string theFinalXletter = (xLetter == nullptr) ? "x": *xLetter;
      std::string theFinalPartialLetter = (partialLetter == nullptr) ? "\\partial" : *partialLetter;
      for (int k = numStartingVars; k < theUEformat.polyAlphabeT.size; k ++) {
        std::stringstream tmpStream, tempstream2, tempstream3, tempStream4;
        tmpStream << theUEformat.polyDefaultLetter << "_{" << k-hwContext.ContextGetNumContextVariables() + 1 << "}";
        theUEformat.polyAlphabeT[k] = tmpStream.str();
        tempstream2 << theFinalXletter << "_{" << k-numStartingVars + 1 << "}";
        tempstream3 << theFinalXletter << "_" << k-numStartingVars + 1;
        tempStream4 << theFinalPartialLetter << "_{" << k-numStartingVars + 1 << "}";
        if (
          theWeylFormat.polyAlphabeT.Contains(tempstream2.str()) ||
          theWeylFormat.polyAlphabeT.Contains(tempstream3.str())
        ) {
          return output.MakeError(
            "Error: the variable " +
            tempstream2.str() +
            " is reserved for me: you are not allowed to use it as a coordinate of the highest weight. ",
            theCommands
          );
        }
        theWeylFormat.polyAlphabeT[k] = tempstream2.str();
        theWeylFormat.weylAlgebraLetters[k] = tempStream4.str();
      }
      out << "<tr><td>General monomial in U(n_-):</td><td>"
      << HtmlRoutines::GetMathMouseHover(genericElt.ToString(&theUEformat)) << "</td> </tr>";
      latexReport << "& \\multicolumn{" << theGeneratorsItry.size << "}{c}{Element acting}\\\\<br>\n ";
      latexReport << "Action on ";
      out << "<tr><td></td><td colspan =\"" << theGeneratorsItry.size << "\"> Element acting</td></td></tr>";
      out << "<tr><td>Action on</td>";
      for (int j = 0; j < theGeneratorsItry.size; j ++) {
        out << "<td>" << theGeneratorsItry[j].ToString(&theUEformat) << "</td>";
        latexReport << "& $" << theGeneratorsItry[j].ToString(&theUEformat)  << "$";
      }
      latexReport << "\\endhead \\hline<br>";
      out << "</tr>";
      out << "<tr><td>" << HtmlRoutines::GetMathMouseHover(genericElt.ToString(&theUEformat)) << "</td>";
      latexReport << "$" << genericElt.ToString(&theUEformat) << "$";
      for (int j = 0; j < theGeneratorsItry.size; j ++) {
        actionOnGenericElt.AssignElementLieAlgebra(theGeneratorsItry[j], theSSalgebra, Pone);
        actionOnGenericElt *= genericElt;
        theSSalgebra.OrderNilradical(theMod.parabolicSelectionNonSelectedAreElementsLevi, useNilWeight, ascending);
        actionOnGenericElt.Simplify();
        theUEformat.NumAmpersandsPerNewLineForLaTeX = 2;
        out << "<td>" << HtmlRoutines::GetMathMouseHover("\\begin{array}{rcl}&&" + actionOnGenericElt.ToString(&theUEformat) + "\\end{array}") << "</td>";
        theUEformat.NumAmpersandsPerNewLineForLaTeX = 0;
        latexReport << "& $\\begin{array}{l} " << actionOnGenericElt.ToString(&theUEformat) << "\\end{array}$ ";
      }
      latexReport << "\\\\ \\hline\\hline<br>";
      out << "</tr>";
    }
    out << "<tr><td>" << HtmlRoutines::GetMathMouseHover(theMod.theChaR.ToString()) << "</td>";
    latexReport2 << "\\begin{longtable}{rll}";
    latexReport2 << "$\\gog$& $n$& element of $\\mathbb W_n$ \\\\\\hline" << "\\multirow{" << theGeneratorsItry.size
    << "}{*}{$" << theSSalgebra.ToStringLieAlgebraName() << "$}" << " &  \\multirow{"  << theGeneratorsItry.size << "}{*}{"
    << elementsNegativeNilrad.size << "}&";

    latexReport << "$\\begin{array}{r}" << theMod.theChaR.ToString() << "(\\mathfrak{l}) \\\\ \\\\dim:~" << theMod.GetDim() << " \\end{array}$";
    for (int j = 0; j < theGeneratorsItry.size; j ++) {
      theGenerator = theGeneratorsItry[j];
      currentTime = global.GetElapsedSeconds();
      currentAdditions = Rational::TotalAdditions();
      currentMultiplications = Rational::TotalMultiplications();
      theMod.GetActionGenVermaModuleAsDiffOperator(theGenerator, theQDOs[j], useNilWeight, ascending);
      totalAdditions += Rational::TotalAdditions() - currentAdditions;
      totalMultiplications += Rational::TotalMultiplications() - currentMultiplications;
      totalTime += global.GetElapsedSeconds() - currentTime;
      theWeylFormat.CustomCoeffMonSeparator = "\\otimes ";
      theWeylFormat.NumAmpersandsPerNewLineForLaTeX = 2;
      out << "<td>" << HtmlRoutines::GetMathMouseHover("\\begin{array}{|r|c|l|}&&"+theQDOs[j].ToString(&theWeylFormat) +"\\end{array}")
      << "</td>";
      theWeylFormat.NumAmpersandsPerNewLineForLaTeX = 0;
      theWeylFormat.MaxLineLength = 300;
      latexReport << " & $\\begin{array}{l}" << theQDOs[j].ToString(&theWeylFormat) << "\\end{array}$";
      if (j != 0) {
        latexReport2 << "&&";
      }
      latexReport2 << " $\\begin{array}{l}" << theQDOs[j].ToString(&theWeylFormat) << "\\end{array}$\\\\ "
      << (j != theGeneratorsItry.size - 1 ? "\\cline{3-3}" : "\\hline" ) << "\n<br>";
      theWeylFormat.CustomCoeffMonSeparator = "";
    }
    latexReport2 << "\\end{longtable}";
    latexReport << "\\\\\\hline<br>";
    out << "</tr>";
    if (theMod.GetDim() == 1) {
      ElementWeylAlgebra<Rational> diffOpPart, transformedDO;
      reportFourierTransformedCalculatorCommands << "<hr>" << HtmlRoutines::GetMathMouseHover(theMod.theChaR.ToString())
      << ", differential operators Fourier transformed - formatted for calculator input. <br><br>";
      reportFourierTransformedCalculatorCommands << "x_{{i}}= ElementWeylAlgebraPoly{}(\\partial_i, x_i);\n<br>"
      << "\\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n";
      reportCalculatorCommands << "<hr>" << HtmlRoutines::GetMathMouseHover(theMod.theChaR.ToString())
      << ", differential operators - formatted for calculator input. <br><br>";
      reportCalculatorCommands << "x_{{i}}= ElementWeylAlgebraPoly{}(\\partial_i, x_i);\n<br>"
      << "\\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n";

      for (int j = 0; j < theGeneratorsItry.size; j ++) {
        theQDOs[j].GetEWAsetMatrixPartsToId(diffOpPart);
        diffOpPart.FourierTransform(transformedDO);
        reportFourierTransformedCalculatorCommands << "<br>"
        << theGeneratorsItry[j].ToString() << "=" << transformedDO.ToString() << ";";
        reportCalculatorCommands << "<br>" << theGeneratorsItry[j].ToString() << "=" << diffOpPart.ToString() << ";";
      }
      reportFourierTransformedCalculatorCommands << "<br>GenerateVectorSpaceClosedWRTLieBracket{}(248," ;
      reportCalculatorCommands << "<br>GenerateVectorSpaceClosedWRTLieBracket{}(248," ;
      for (int j = 0; j < theGeneratorsItry.size; j ++) {
        reportFourierTransformedCalculatorCommands << theGeneratorsItry[j].ToString();
        reportCalculatorCommands << theGeneratorsItry[j].ToString();
        if (j != theGeneratorsItry.size - 1) {
          reportFourierTransformedCalculatorCommands << ", ";
          reportCalculatorCommands << ", ";
        }
      }
      reportFourierTransformedCalculatorCommands << ");";
      reportFourierTransformedCalculatorCommands << "<hr>";
      reportCalculatorCommands << ");";
      reportCalculatorCommands << "<hr>";
    }
  }
  latexReport << "\\end{longtable}";
  out << "</table>";
  out << "<br>Multiplications needed to embed generators: " << totalMultiplications << ". ";
  out << "<br>Additions needed to embed generators: " << totalAdditions << ". ";
  out << "<br>Total time to embed generators: see comments. ";
  //please keep this report in the comments section for reasons of reproducibility of the
  //string output!
  theCommands << "<hr>Total time to embed generators invoked by command: " << input.ToString()
  << ": <br>" << totalTime << " seconds. ";
  out << reportCalculatorCommands.str();
  out << reportFourierTransformedCalculatorCommands.str();
  out << "<br>" << latexReport.str();
  out << "<br><br>" << latexReport2.str();
  return output.AssignValue<std::string>(out.str(), theCommands);
}

bool Calculator::innerHWVCommon(
  Calculator& theCommands,
  Expression& output,
  Vector<RationalFunctionOld>& highestWeightFundCoords,
  Selection& selectionParSel,
  Expression& hwContext,
  SemisimpleLieAlgebra* owner,
  bool Verbose
) {
  MacroRegisterFunctionWithName("Calculator::innerHWVCommon");
  RecursionDepthCounter therecursionIncrementer(&theCommands.RecursionDeptH);
  RationalFunctionOld RFOne, RFZero;
  RFOne.MakeOne();
  RFZero.MakeZero();
  std::string report;
  ElementTensorsGeneralizedVermas<RationalFunctionOld> theElt;
  //= theElementData.theElementTensorGenVermas.GetElement();
  ListReferences<ModuleSSalgebra<RationalFunctionOld> >& theMods = theCommands.theObjectContainer.theCategoryOmodules;
  int indexOfModule = - 1;

  for (int i = 0; i < theMods.size; i ++) {
    ModuleSSalgebra<RationalFunctionOld>& currentMod = theMods[i];
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
    theMods.SetSize(theMods.size + 1);
    theMods.LastObject().reset();
  }
  ModuleSSalgebra<RationalFunctionOld>& theMod = theMods[indexOfModule];
  if (!theMod.flagIsInitialized) {
    bool isGood = theMod.MakeFromHW(*owner, highestWeightFundCoords, selectionParSel, RFOne, RFZero, &report);
    if (Verbose) {
      theCommands << theMod.ToString();
    }
    if (!isGood) {
      return output.MakeError("Error while generating highest weight module. See comments for details. ", theCommands);
    }
  }
  if (&theMod.GetOwner() != owner) {
    global.fatal << "This is a programming error: module has owner that is not what it should be. " << global.fatal;
  }
  theElt.MakeHWV(theMod, RFOne);
  if (&theElt.GetOwnerSS() != owner) {
    global.fatal << "This is a programming error: just created an ElementTensorsGeneralizedVermas "
    << "whose owner is not what it should be. " << global.fatal;
  }
  return output.AssignValueWithContext<ElementTensorsGeneralizedVermas<RationalFunctionOld> >(theElt, hwContext, theCommands);
}

bool Calculator::RecursionDepthExceededHandleRoughly(const std::string& additionalErrorInfo) {
  if (this->RecursionDeptH <= this->MaxRecursionDeptH) {
    return false;
  }
  if (!this->flagMaxRecursionErrorEncountered) {
    *this << additionalErrorInfo
    << "<span style =\"color:#FF0000\"><b> Maximum recursion depth of " << this->MaxRecursionDeptH
    << " exceeded. </b></span>" << "Aborting computation ASAP. ";
  }
  this->flagAbortComputationASAP = true;
  this->flagMaxRecursionErrorEncountered = true;
  return true;
}

bool Calculator::CheckOperationHandlers() {
  MacroRegisterFunctionWithName("Calculator::CheckOperationHandlers");
  for (int i = 0; i < this->operations.size(); i ++) {
    MemorySaving<Calculator::OperationHandlers>& current = this->operations.theValues[i];
    if (current.IsZeroPointer()) {
      continue;
    }
    current.GetElement().CheckConsisitency();
    Calculator::OperationHandlers& allHandlers = current.GetElement();
    for (int j = 0; j < allHandlers.compositeHandlers.size; j ++) {
      allHandlers.compositeHandlers[j].CheckConsistency();
    }
    for (int j = 0; j < allHandlers.handlers.size; j ++) {
      allHandlers.handlers[j].CheckConsistency();
    }
  }
  return true;
}

bool Calculator::CheckConsistencyAfterInitialization() {
  this->theExpressionContainer.GrandMasterConsistencyCheck();
  this->EvaluatedExpressionsStack.GrandMasterConsistencyCheck();
  this->cachedExpressions.GrandMasterConsistencyCheck();
  if (this->numberExpectedExpressionsAtInitialization < 0) {
    this->numberExpectedExpressionsAtInitialization = this->theExpressionContainer.size;
  } else if (
    this->theExpressionContainer.size != this->numberExpectedExpressionsAtInitialization
  ) {
    global.fatal << "Expression container expected to have "
    << this->numberExpectedExpressionsAtInitialization
    << " elements but instead it has " << this->theExpressionContainer.size
    << ". Expression container: ";
    for (int i = 0; i < this->theExpressionContainer.size; i ++) {
      global.fatal << this->theExpressionContainer[i].ToString() << ", ";
    }
    global.fatal << global.fatal;
  }
  if (
    this->cachedExpressions.size != 0 ||
    this->imagesCachedExpressions.size != 0 ||
    this->EvaluatedExpressionsStack.size != 0
  ) {
    global.fatal << "This is a programming error: cached expressions, "
    << "images cached expressions, expression stack and "
    << "expression container are supposed to be empty, but "
    << "instead they contain respectively "
    << this->cachedExpressions.size << ", "
    << this->imagesCachedExpressions.size << ", and "
    << this->EvaluatedExpressionsStack.size
    << " elements. " << global.fatal;
  }
  return this->theObjectContainer.CheckConsistencyAfterReset();
}

bool Calculator::innerFunctionToMatrix(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerFunctionToMatrix");
  if (!input.IsListNElements(4)) {
    return false;
  }
  const Expression& leftE   = input[1];
  const Expression& middleE = input[2];
  const Expression& rightE  = input[3];
  int numRows, numCols;
  if (!middleE.IsIntegerFittingInInt(&numRows) || !rightE.IsIntegerFittingInInt(&numCols)) {
    return false;
  }
  if (numRows <= 0 || numCols <= 0) {
    return false;
  }
  LargeInteger numRowsTimesCols = numRows;
  numRowsTimesCols *= numCols ;
  if (numRowsTimesCols > 10000) {
    theCommands << "Max number of matrix entries is 10000. You requested " << numRows
    << " rows and " << numCols
    << " columns, total: " << numRowsTimesCols.ToString() << " entries<br>";
    return false;
  }
  Matrix<Expression> resultMat;
  resultMat.init(numRows, numCols);
  Expression leftIE, rightIE;
  for (int i = 0; i < numRows; i ++) {
    for (int j = 0; j < numCols; j ++) {
      leftIE.AssignValue(i + 1, theCommands);
      rightIE.AssignValue(j + 1, theCommands);
      resultMat.elements[i][j].reset(theCommands, 3);
      resultMat.elements[i][j].AddChildOnTop(leftE);
      resultMat.elements[i][j].AddChildOnTop(leftIE);
      resultMat.elements[i][j].AddChildOnTop(rightIE);
    }
  }
  return output.AssignMatrixExpressions(resultMat, theCommands, true, true);
}

bool Calculator::innerGetChevGen(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerGetChevGen");
  if (!input.IsListNElements(3)) {
    return false;
  }
  SemisimpleLieAlgebra* theSSalg;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(
    CalculatorConversions::innerSSLieAlgebra, input[1], theSSalg
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  int theIndex;
  if (!input[2].IsSmallInteger(&theIndex)) {
    return false;
  }
  if (theIndex>theSSalg->GetNumPosRoots() || theIndex == 0 || theIndex < - theSSalg->GetNumPosRoots()) {
    return output.MakeError("Bad Chevalley-Weyl generator index.", theCommands);
  }
  ElementSemisimpleLieAlgebra<Rational> theElt;
  if (theIndex > 0) {
    theIndex += theSSalg->GetRank() - 1;
  }
  theIndex += theSSalg->GetNumPosRoots();
  theElt.MakeGenerator(theIndex, *theSSalg);
  ElementUniversalEnveloping<RationalFunctionOld> theUE;
  theUE.AssignElementLieAlgebra(theElt, *theSSalg);
  Expression theContext;
  int indexInOwner = theCommands.theObjectContainer.theSSLieAlgebras.GetIndex(theSSalg->theWeyl.theDynkinType);
  theContext.ContextMakeContextSSLieAlgebrA(indexInOwner, theCommands);
  output.AssignValueWithContext(theUE, theContext, theCommands);
  return true;
}

bool Calculator::innerGetCartanGen(Calculator& theCommands, const Expression& input, Expression& output) {
  if (!input.IsListNElements(3)) {
    return false;
  }
  SemisimpleLieAlgebra* theSSalg = nullptr;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(
    CalculatorConversions::innerSSLieAlgebra, input[1], theSSalg
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  if (theSSalg == nullptr) {
    global.fatal << "This is a programming error: called conversion function successfully, "
    << "but the output is a zero pointer to a semisimple Lie algebra. "
    << global.fatal;
  }
  int theIndex;
  if (!input[2].IsSmallInteger(&theIndex)) {
    return false;
  }
  if (theIndex == 0 || theIndex > theSSalg->GetNumPosRoots() || theIndex < - theSSalg->GetNumPosRoots()) {
    return output.MakeError("Bad Cartan subalgebra generator index.", theCommands);
  }
  ElementSemisimpleLieAlgebra<Rational> theElt;
  Vector<Rational> theH = theSSalg->theWeyl.RootSystem[theSSalg->GetRootIndexFromDisplayIndex(theIndex)];
  theElt.MakeHgenerator(theH, *theSSalg);
  ElementUniversalEnveloping<RationalFunctionOld> theUE;
  theUE.AssignElementLieAlgebra(theElt, *theSSalg);
  Expression theContext;
  int theAlgIndex = theCommands.theObjectContainer.theSSLieAlgebras.GetIndex(theSSalg->theWeyl.theDynkinType);
  theContext.ContextMakeContextSSLieAlgebrA(theAlgIndex, theCommands);
  return output.AssignValueWithContext(theUE, theContext, theCommands);
}

bool Calculator::innerKLcoeffs(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerKLcoeffs");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  SemisimpleLieAlgebra* theSSalgebra = nullptr;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(
    CalculatorConversions::innerSSLieAlgebra, input, theSSalgebra
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  std::stringstream out;
  WeylGroupData& theWeyl = theSSalgebra->theWeyl;
  if (theWeyl.theGroup.GetSize() > 192) {
    out << "I have been instructed to run only for Weyl groups that"
    << " have at most 192 elements (i.e. no larger than D_4). "
    << theSSalgebra->ToStringLieAlgebraName() << " has " << theWeyl.theGroup.GetSize().ToString() << ".";
    return output.AssignValue(out.str(), theCommands);
  }
  FormatExpressions theFormat;
  theFormat.polyAlphabeT.SetSize(1);
  theFormat.polyAlphabeT[0] = "q";
  out << "Our notation follows that of the original Kazhdan-Lusztig paper, "
  << "Representations of Coxeter Groups and Hecke Algebras.<br>";
  out << " The algebra: " << theSSalgebra->ToStringLieAlgebraName();
  KLpolys theKLpolys;
  theKLpolys.ComputeKLPolys(&theWeyl);
  theFormat.flagUseHTML = true;
  out << theKLpolys.ToString(&theFormat);
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerPrintSSLieAlgebra(Calculator& theCommands, const Expression& input, Expression& output, bool Verbose) {
  return Calculator::innerWriteToHDOrPrintSSLieAlgebra(theCommands, input, output, Verbose, false);
}

bool Calculator::innerWriteSSLieAlgebraToHD(Calculator& theCommands, const Expression& input, Expression& output) {
  if (!global.UserDefaultHasAdminRights()) {
    return output.MakeError(std::string("Caching structure constants to HD available to logged-in admins only. "), theCommands);
  }
  return Calculator::innerWriteToHDOrPrintSSLieAlgebra(theCommands, input, output, true, true);
}

bool Calculator::innerWriteToHDOrPrintSSLieAlgebra(
  Calculator& theCommands, const Expression& input, Expression& output, bool Verbose, bool writeToHD
) {
  MacroRegisterFunctionWithName("Calculator::innerPrintSSLieAlgebra");
//  double startTimeDebug= global.GetElapsedSeconds();
  SemisimpleLieAlgebra *tempSSpointer = nullptr;
  input.CheckInitialization();
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(
    CalculatorConversions::innerSSLieAlgebra, input, tempSSpointer
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  SemisimpleLieAlgebra& theSSalgebra = *tempSSpointer;

  std::string result = theSSalgebra.ToHTMLCalculator(Verbose, writeToHD, theCommands.flagWriteLatexPlots);
  return output.AssignValue(result, theCommands);
}

bool Expression::CheckInitializationRecursively() const {
  MacroRegisterFunctionWithName("Expression::CheckInitializationRecursively");
  this->CheckInitialization();
  for (int i = 0; i < this->children.size; i ++) {
    (*this)[i].CheckInitializationRecursively();
  }
  return true;
}

bool Expression::CheckInitialization() const {
  if (this->owner == nullptr) {
    global.fatal << "This is a programming error: " << "Expression has non-initialized owner. " << global.fatal;
    return false;
  }
  return true;
}

bool Expression::HasInputBoxVariables(HashedList<std::string, MathRoutines::HashString>* boxNames) const {
  MacroRegisterFunctionWithName("Expression::HasInputBoxVariables");
  if (this->owner == nullptr) {
    return false;
  }
  RecursionDepthCounter recursionCounter(&this->owner->RecursionDeptH);
  if (this->owner->RecursionDeptH > this->owner->MaxRecursionDeptH) {
    global.fatal << "This is a programming error: "
    << "function HasInputBoxVariables has exceeded "
    << "recursion depth limit. " << global.fatal;
  }
  bool result = false;
  InputBox tempBox;
  if (this->IsOfType<InputBox>(&tempBox)) {
    if (boxNames == nullptr) {
      return true;
    } else {
      result = true;
      boxNames->AddOnTopNoRepetition(tempBox.name);
    }
  }
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].HasInputBoxVariables(boxNames)) {
      if (boxNames == nullptr) {
        return true;
      } else {
        result = true;
      }
    }
  }
  return result;
}

bool Expression::HasBoundVariables() const {
  if (this->owner == nullptr) {
    global.fatal << "This is a programming error: calling function "
    << "HasBoundVariables on non-initialized expression. " << global.fatal;
  }
  RecursionDepthCounter recursionCounter(&this->owner->RecursionDeptH);
  MacroRegisterFunctionWithName("Expression::HasBoundVariables");
  if (this->owner->RecursionDeptH>this->owner->MaxRecursionDeptH) {
    global.fatal << "This is a programming error: function HasBoundVariables has exceeded recursion depth limit. " << global.fatal;
  }
  if (this->IsListOfTwoAtomsStartingWith(this->owner->opBind())) {
    return true;
  }
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].HasBoundVariables()) {
      return true;
    }
  }
  return false;
}

bool Calculator::innerNot(Calculator& theCommands, const Expression& input, Expression& output) {
  int theInt;
  if (!input.IsSmallInteger(&theInt)) {
    return false;
  }
  if (theInt == 0) {
    return output.AssignValue(1, theCommands);
  }
  return output.AssignValue(0, theCommands);
}

bool Calculator::innerIsInteger(Calculator& theCommands, const Expression& input, Expression& output) {
  if (input.HasBoundVariables()) {
    return false;
  }
  if (input.IsInteger()) {
    output.AssignValue(1, theCommands);
  } else {
    output.AssignValue(0, theCommands);
  }
  return true;
}

bool Calculator::innerIsRational(Calculator& theCommands, const Expression& input, Expression& output) {
  if (input.HasBoundVariables()) {
    return false;
  }
  if (input.IsRational()) {
    output.AssignValue(1, theCommands);
  } else {
    output.AssignValue(0, theCommands);
  }
  return true;
}

bool Calculator::AppendOpandsReturnTrueIfOrderNonCanonical(const Expression& input, List<Expression>& output, int theOp) {
  RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  if (this->RecursionDeptH > this->MaxRecursionDeptH) {
    return false;
  }
  bool result = false;
  if (!input.IsListStartingWithAtom(theOp)) {
    output.AddOnTop(input);
  } else {
    for (int i = 1; i < input.size(); i ++) {
      if (this->AppendOpandsReturnTrueIfOrderNonCanonical(input[i], output, theOp)) {
        result = true;
      }
      if (i < input.size() - 1 && input[i].IsListStartingWithAtom(theOp) && input[i].size() > 2) {
        result = true;
      }
    }
  }
  return result;
}

bool Calculator::outerTensor(Calculator& theCommands, const Expression& input, Expression& output) {
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("Calculator::StandardTensor");
  if (theCommands.outerDistribute(theCommands, input, output, theCommands.opPlus(), theCommands.opTensor())) {
    return true;
  }
  if (theCommands.outerAssociate(theCommands, input, output)) {
    return true;
  }
  if (theCommands.outerExtractBaseMultiplication(theCommands, input, output)) {
    return true;
  }
  return false;
}

bool Calculator::innerMultiplyAtoXtimesAtoYequalsAtoXplusY(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerMultiplyAtoXtimesAtoYequalsAtoXplusY");
  if (!input.StartsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  Expression constPower, thePower;
  const Expression* left = &input[1];
  const Expression* right = &input[2];
  if (*left == *right) {
    constPower.AssignValue(2, theCommands);
    output.MakeXOX(theCommands, theCommands.opThePower(), *left, constPower);
    return true;
  }
  for (int i = 0; i < 2; i ++, MathRoutines::swap(left, right)) {
    if (right->StartsWith(theCommands.opThePower(), 3)) {
      if ((*right)[1] == (*left)) {
        bool isGood = true;
        if ((*right)[2].IsOfType<Rational>()) {
          if (!(*right)[2].GetValue<Rational>().IsInteger()) {
            Rational ratBase;
            if ((*right)[1].IsRational(&ratBase)) {
              if (ratBase > 0) {
                isGood = false;
              }
            }
          }
        }
        if (isGood) {
          thePower.MakeXOX(theCommands, theCommands.opPlus(), (*right)[2], theCommands.EOne());
          return output.MakeXOX(theCommands, theCommands.opThePower(), *left, thePower);
        }
      }
      if (left->StartsWith(theCommands.opThePower(), 3)) {
        if ((*left)[1] == (*right)[1]) {
          bool isGood = (*left)[2].IsInteger() || (*right)[2].IsInteger();
          if (!isGood) {
            isGood = (*left)[1].IsKnownToBeNonNegative();
          }
          if (!isGood) {
            continue;
          }
          Rational leftRat, rightRat;
          if (!isGood) {
            if ((*left)[2].IsOfType<Rational>(&leftRat) && (*right)[2].IsOfType<Rational>(&rightRat)) {
              if (leftRat.IsInteger() && !rightRat.IsInteger()) {
                isGood = true;
              }
              if (!leftRat.IsInteger() && rightRat.IsInteger()) {
                isGood = true;
              }
            }
          }
          if (!isGood) {
            continue;
          }
          thePower.MakeXOX(theCommands, theCommands.opPlus(), (*left)[2], (*right)[2]);
          return output.MakeXOX(theCommands, theCommands.opThePower(), (*left)[1], thePower);
        }
      }
    }
  }
  return false;
}

bool Calculator::outerCombineFractions(Calculator& theCommands, const Expression& input, Expression& output) {
  if (!input.StartsWith(theCommands.opPlus(), 3)) {
    return false;
  }
  const Expression* quotientE = nullptr;
  const Expression* summandE = nullptr;
  if (input[1].StartsWith(theCommands.opDivide(), 3)) {
    quotientE = &input[1];
    summandE = &input[2];
  } else if (input[2].StartsWith(theCommands.opDivide(), 3)) {
    quotientE = &input[2];
    summandE = &input[1];
  } else {
    return false;
  }
  const Expression& quotientNumeratorE = (*quotientE)[1];
  const Expression& quotientDenominatorE = (*quotientE)[2];
  Expression outputSummandE, outputNumE;
  outputSummandE.MakeXOX(theCommands, theCommands.opTimes(), *summandE, quotientDenominatorE);
  outputNumE.MakeXOX(theCommands, theCommands.opPlus(), quotientNumeratorE, outputSummandE);
  return output.MakeXOX(theCommands, theCommands.opDivide(), outputNumE, quotientDenominatorE);
}

bool Calculator::outerCheckRule(Calculator& theCommands, const Expression& input, Expression& output) {
  if (!input.StartsWith(theCommands.opDefine(), 3)) {
    return false;
  }
  if (input[1] != input[2]) {
    return false;
  }
  std::stringstream out;
  out << "Bad rule: you are asking me to substitute " << input[1] << " by itself ("
  << input[2] << ")" << " which is an infinite substitution cycle. ";
  return output.MakeError(out.str(), theCommands);
}

bool Calculator::innerSubZeroDivAnythingWithZero(Calculator& theCommands, const Expression& input, Expression& output) {
  if (!input.StartsWith(theCommands.opDivide(), 3)) {
    return false;
  }
  if (input[1].IsEqualToZero()) {
    if (!input[2].IsEqualToZero()) {
      return output.AssignValue(0, theCommands);
    }
  }
  return false;
}

bool Calculator::innerCancelMultiplicativeInverse(Calculator& theCommands, const Expression& input, Expression& output) {
  if (!input.StartsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  if (!input[1].StartsWith(theCommands.opDivide(), 3)) {
    return false;
  }
  if (input[1][2] == input[2]) {
    output = input[1][1];
    return true;
  }
  return false;
}

bool Calculator::outerAssociateTimesDivision(Calculator& theCommands, const Expression& input, Expression& output) {
  if (!input.StartsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  if (!input[2].StartsWith(theCommands.opDivide(), 3)) {
    return false;
  }
  Expression newLeftE;
  newLeftE.MakeXOX(theCommands, theCommands.opTimes(), input[1], input[2][1]);
  output.MakeXOX(theCommands, theCommands.opDivide(), newLeftE, input[2][2]);
  return true;
}

bool Calculator::outerAssociate(Calculator& theCommands, const Expression& input, Expression& output) {
  if (!input.StartsWith(- 1, 3)) {
    return false;
  }
  int theOperation = input[0].theData;
  if (!input[1].StartsWith(theOperation, 3)) {
    return false;
  }
  Expression newRight;
  newRight.MakeXOX(theCommands, theOperation, input[1][2], input[2]);
  output.MakeXOX(theCommands, theOperation, input[1][1], newRight);
  return true;
}

bool Calculator::StandardIsDenotedBy(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::StandardIsDenotedBy");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (!input.StartsWith(theCommands.opIsDenotedBy(), 3)) {
    return false;
  }
  const Expression& withNotation = input[2];
  const Expression& theNotation = input[1];
  theCommands << "<br>Registering notation: globally, " << withNotation.ToString() << " will be denoted by "
  << theNotation.ToString();
  theCommands.theObjectContainer.ExpressionNotation.AddOnTop(theNotation.ToString());
  theCommands.theObjectContainer.ExpressionWithNotation.AddOnTop(withNotation);
  output = input;
  output.SetChildAtomValue(0, theCommands.opDefine());
  ////
  if (withNotation.IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >()) {
    if (withNotation.GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >().IsHWV()) {
      MonomialGeneralizedVerma<RationalFunctionOld>& theElt =
      withNotation.GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >()[0].theMons[0];
      theElt.GetOwner().highestWeightVectorNotation = theNotation.ToString();
    }
  }
  return true;
}

bool Calculator::innerMultiplyByOne(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerMultiplyByOne");
  if (!input.IsListStartingWithAtom(theCommands.opTimes()) || input.size() != 3) {
    return false;
  }
  if (!input[1].IsEqualToOne()) {
    return false;
  }
  output = input[2];
  return true;
}

bool Calculator::GetVectorLargeIntFromFunctionArguments(const Expression& input, List<LargeInteger>& output) {
  MacroRegisterFunctionWithName("Calculator::GetVectorLargeIntFromFunctionArguments");
  Vector<Rational> theRats;
  if (!this->GetVectorFromFunctionArguments(input, theRats)) {
    return false;
  }
  output.initializeFillInObject(theRats.size, 0);
  for (int i = 0; i < theRats.size; i ++) {
    if (!theRats[i].IsInteger(&output[i])) {
      return *this << "<hr>Succeeded to convert " << input.ToString() << " to the vector of rationals: "
      << theRats.ToString() << " but failed to convert that to list of integers. ";
    }
  }
  return true;
}

bool Calculator::GetVectoRInt(const Expression& input, List<int>& output) {
  MacroRegisterFunctionWithName("Calculator::GetVectoRInt");
  Vector<Rational> theRats;
  if (!this->GetVectoR(input, theRats)) {
    return false;
  }
  output.initializeFillInObject(theRats.size,0);
  for (int i = 0; i < theRats.size; i ++) {
    if (!theRats[i].IsSmallInteger(&output[i])) {
      return *this << "<hr>Succeeded to convert " << input.ToString() << " to the vector of rationals: "
      << theRats.ToString() << " but failed to convert that to list of small integers. ";
    }
  }
  return true;
}

bool Calculator::outerTimesToFunctionApplication(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::outerTimesToFunctionApplication");
  if (!input.StartsWith(theCommands.opTimes())) {
    return false;
  }
  if (input.size() < 2) {
    return false;
  }
  const Expression& firstElt = input[1];
  if (!firstElt.IsBuiltInAtom()) {
    if (!firstElt.StartsWith(theCommands.opThePower(), 3)) {
      return false;
    }
    if (!firstElt[1].IsAtomWhoseExponentsAreInterpretedAsFunction()) {
      return false;
    }
  }
  if (firstElt.IsAtomNotInterpretedAsFunction()) {
    return false;
  }
  const Expression& secondElt = input[2];
  if (secondElt.IsSequenceNElementS() || secondElt.StartsWith(theCommands.opIntervalOpen())) {
    output = secondElt;
    return output.SetChilD(0, firstElt);
  }
  output = input;
  output.children.RemoveIndexShiftDown(0);
  return true;
}

bool Calculator::outerDistribute(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  int theAdditiveOp,
  int theMultiplicativeOp,
  bool constantsOnly
) {
  if (theCommands.outerLeftDistributeBracketIsOnTheLeft(
    theCommands, input, output, theAdditiveOp, theMultiplicativeOp, constantsOnly
  )) {
    return true;
  }
  return theCommands.outerRightDistributeBracketIsOnTheRight(
    theCommands, input, output, theAdditiveOp, theMultiplicativeOp, constantsOnly
  );
}

bool Calculator::outerDistributeTimes(Calculator& theCommands, const Expression& input, Expression& output) {
  return Calculator::outerDistribute(theCommands, input, output,theCommands.opPlus(), theCommands.opTimes(), false);
}

bool Calculator::outerDistributeTimesConstant(Calculator& theCommands, const Expression& input, Expression& output) {
  return Calculator::outerDistribute(theCommands, input, output,theCommands.opPlus(), theCommands.opTimes(), true);
}

bool Calculator::outerLeftDistributeBracketIsOnTheLeft(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  int theAdditiveOp,
  int theMultiplicativeOp,
  bool constantsOnly
) {
  MacroRegisterFunctionWithName("Calculator::outerLeftDistributeBracketIsOnTheLeft");
  if (theAdditiveOp == - 1) {
    theAdditiveOp = theCommands.opPlus();
  }
  if (theMultiplicativeOp == - 1) {
    theMultiplicativeOp = theCommands.opTimes();
  }
  if (!input.StartsWith(theMultiplicativeOp, 3)) {
    return false;
  }
  if (!input[1].StartsWith(theAdditiveOp, 3)) {
    return false;
  }
  if (constantsOnly) {
    if (!input[2].IsConstantNumber()) {
      return false;
    }
  }
  Expression leftE, rightE;
  leftE.MakeXOX(theCommands, theMultiplicativeOp, input[1][1], input[2]);
  rightE.MakeXOX(theCommands, theMultiplicativeOp, input[1][2], input[2]);
  return output.MakeXOX(theCommands, theAdditiveOp, leftE, rightE);
}

bool Calculator::outerRightDistributeBracketIsOnTheRight(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  int theAdditiveOp,
  int theMultiplicativeOp,
  bool constantsOnly
) {
  MacroRegisterFunctionWithName("Calculator::outerRightDistributeBracketIsOnTheRight");
  if (theAdditiveOp == - 1) {
    theAdditiveOp = theCommands.opPlus();
  }
  if (theMultiplicativeOp == - 1) {
    theMultiplicativeOp = theCommands.opTimes();
  }
  if (!input.StartsWith(theMultiplicativeOp, 3)) {
    return false;
  }
  if (!input[2].StartsWith(theAdditiveOp, 3)) {
    return false;
  }
  if (constantsOnly) {
    if (!input[1].IsConstantNumber()) {
      return false;
    }
  }
//  int theFormat = input.format;
  Expression leftE, rightE;
  leftE.MakeXOX(theCommands, theMultiplicativeOp, input[1], input[2][1]);
  rightE.MakeXOX(theCommands, theMultiplicativeOp, input[1], input[2][2]);
  return output.MakeXOX(theCommands, theAdditiveOp, leftE, rightE);
}

bool Calculator::CollectCoefficientsPowersVar(
  const Expression& input, const Expression& theVariable, VectorSparse<Expression>& outputPositionIiscoeffXtoIth
) {
  MacroRegisterFunctionWithName("Calculator::CollectCoefficientsPowersVar");
  List<Expression> theSummands, currentMultiplicands, remainingMultiplicands;
  Calculator& theCommands = *input.owner;
  theCommands.CollectOpands(input, theCommands.opPlus(), theSummands);
  Expression currentCoeff;
  outputPositionIiscoeffXtoIth.MakeZero();
  for (int i = 0; i < theSummands.size; i ++) {
    theCommands.CollectOpands(theSummands[i], theCommands.opTimes(), currentMultiplicands);
    bool found = false;
    for (int j = 0; j < currentMultiplicands.size; j ++) {
      const Expression& currentE = currentMultiplicands[j];
      remainingMultiplicands = currentMultiplicands;
      remainingMultiplicands.RemoveIndexShiftDown(j);
      if (remainingMultiplicands.size == 0) {
        currentCoeff.AssignValue(1, theCommands);
      } else {
        currentCoeff.MakeProducT(theCommands, remainingMultiplicands);
      }
      if (currentE == theVariable) {
        outputPositionIiscoeffXtoIth.AddMonomial(MonomialVector(1), currentCoeff);
        found = true;
        break;
      }
      if (currentE.StartsWith(theCommands.opThePower(), 3)) {
        int thePower;
        if (currentE[1] == theVariable) {
          if (currentE[2].IsSmallInteger(&thePower)) {
            outputPositionIiscoeffXtoIth.AddMonomial(MonomialVector(thePower), currentCoeff);
            found = true;
            break;
          }
        }
      }
    }
    if (!found) {
      outputPositionIiscoeffXtoIth.AddMonomial(MonomialVector(0), theSummands[i]);
    }
  }
  return true;
}

bool Calculator::CollectOpands(const Expression& input, int theOp, List<Expression>& outputOpands) {
  MacroRegisterFunctionWithName("Calculator::CollectOpands");
  outputOpands.SetSize(0);
  return this->CollectOpandsAccumulate(input, theOp, outputOpands);
}

bool Calculator::CollectOpandsAccumulate(
  const Expression& input, int theOp, List<Expression>& outputOpands
) {
  MacroRegisterFunctionWithName("Calculator::CollectOpandsAccumulate");
  if (!input.StartsWith(theOp)) {
    outputOpands.AddOnTop(input);
    return true;
  }
  for (int i = 1; i < input.size(); i ++) {
    this->CollectOpandsAccumulate(input[i], theOp, outputOpands);
  }
  return true;
}

bool Calculator::CollectSummands(
  Calculator& theCommands, const Expression& input, MonomialCollection<Expression, Rational>& outputSum
) {
  MacroRegisterFunctionWithName("Calculator::CollectSummands");
  List<Expression> summands;
  theCommands.AppendSummandsReturnTrueIfOrderNonCanonical(input, summands);
  outputSum.MakeZero();
  MonomialCollection<Expression, AlgebraicNumber> sumOverAlgNums;
  MonomialCollection<Expression, double> sumOverDoubles;
  Rational coeffRat = 1;
  AlgebraicNumber coeffAlg = 1;
  double coeffDouble = 1;
  bool hasNAN = false;
  for (int i = 0; i < summands.size; i ++) {
    if (summands[i].IsEqualToZero()) {
      continue;
    }
    if (summands[i].StartsWith(theCommands.opTimes(), 3)) {
      if (summands[i][1].IsOfType<Rational>(&coeffRat)) {
        outputSum.AddMonomial(summands[i][2], coeffRat);
        continue;
      } else if (summands[i][1].IsOfType<AlgebraicNumber>(&coeffAlg)) {
        if (coeffAlg.IsRational(&coeffRat)) {
          outputSum.AddMonomial(summands[i][2], coeffRat);
          continue;
        }
        sumOverAlgNums.AddMonomial(summands[i][2], coeffAlg);
        continue;
      } else if (summands[i][1].IsOfType<double>(&coeffDouble)) {
        sumOverDoubles.AddMonomial(summands[i][2], coeffDouble);
        if (std::isnan(coeffDouble)) {
          hasNAN = true;
        }
        continue;
      }
    }
    if (summands[i].IsRational(&coeffRat)) {
      outputSum.AddMonomial(theCommands.EOne(), coeffRat);
    } else {
      outputSum.AddMonomial(summands[i], 1);
    }
  }
  if (!sumOverDoubles.IsEqualToZero() && !hasNAN) {
    sumOverDoubles.QuickSortDescending();
    Expression doubleSum;
    doubleSum.MakeSum(theCommands, sumOverDoubles);
    outputSum.AddMonomial(doubleSum, 1);
  }
  if (!sumOverAlgNums.IsEqualToZero()) {
    sumOverAlgNums.QuickSortDescending();
    Expression algSum;
    algSum.MakeSum(theCommands, sumOverAlgNums);
    outputSum.AddMonomial(algSum, 1);
  }
  outputSum.QuickSortDescending();
  return !hasNAN;
}

bool Calculator::innerAssociateExponentExponent(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerAssociateExponentExponent");
  int opPower = theCommands.opThePower();
  if (!input.StartsWith(opPower, 3)) {
    return false;
  }
  if (!input[1].StartsWith(opPower, 3)) {
    return false;
  }
  bool isGood = false;
  if (input[1][1].IsKnownToBeNonNegative()) {
    isGood = true;
  }
  if (input[2].IsInteger()) {
    isGood = true;
  }
  if (!isGood) {
    Rational powerInner, powerOuter;
    if (input[2].IsRational(&powerOuter) && input[1][2].IsRational(&powerInner)) {
      if ((powerInner * powerOuter).IsEven()) {
        isGood = true;
      }
    }
  }
  if (!isGood) {
    return false;
  }
  Expression tempE;
  tempE.MakeProducT(theCommands, input[1][2], input[2]);
  output.MakeXOX(theCommands, opPower, input[1][1], tempE);
  return true;
}

bool Calculator::outerPowerRaiseToFirst(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::outerPowerRaiseToFirst");
  if (!input.StartsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  if (
    input[1].StartsWith(theCommands.opIntegral(), 2) ||
    input[1].IsOperationGiven(theCommands.opIntegral())
  ) {
    return false;
  }
  if (input[2].IsEqualToOne()) {
    output = input[1];
    return true;
  }
  return false;
}

bool Expression::MakeXOXOdotsOX(Calculator& owner, int theOp, const List<Expression>& input) {
  MacroRegisterFunctionWithName("Expression::MakeXOXOdotsOX");
  if (input.size == 0) {
    global.fatal << "This is a programming error: cannot create operation sequence from an empty list. " << global.fatal;
  }
  if (input.size == 1) {
    *this = input[0];
    return true;
  }
  this->MakeXOX(owner, theOp, input[input.size - 2], *input.LastObject());
  Expression result;
  for (int i = input.size - 3; i >= 0; i --) {
    result.reset(owner, 3);
    result.AddChildAtomOnTop(theOp);
    result.AddChildOnTop(input[i]);
    result.AddChildOnTop(*this);
    *this = result;
  }
  return true;
}

bool Expression::MakeIdMatrixExpressions(int theDim, Calculator& inputBoss) {
  Matrix<Expression> theMat;
  theMat.MakeIdMatrix(theDim, inputBoss.EOne(), inputBoss.EZero());
  return this->AssignMatrixExpressions(theMat, inputBoss, false, true);
}

bool Calculator::outerPlus(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::outerPlus");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.StartsWith(theCommands.opPlus())) {
    return false;
  }
  MonomialCollection<Expression, Rational> theSum;
  if (!theCommands.CollectSummands(theCommands, input, theSum)) {
    return false;
  }
  theSum.QuickSortDescending();
  if (theSum.size() < 5) {
    for (int i = 0; i < theSum.size(); i ++) {
      for (int j = i; j < theSum.size(); j ++) {
        if (theSum[i] > theSum[j] && theSum[j] > theSum[i]) {
          global.fatal << "Faulty comparison: " << theSum[i].ToString() << " and " << theSum[j].ToString()
          << " are mutually greater than one another. " << global.fatal;
        }
      }
    }
  }
  output.MakeSum(theCommands, theSum);
  if (output == input) {
    return false;
  }
  return true;
}

bool Calculator::EvaluateIf(Calculator& theCommands, const Expression& input, Expression& output) {
  if (!input.StartsWith(theCommands.opDefineConditional(), 4)) {
    return output.MakeError("Error: operation :if = takes three arguments.", theCommands);
  }
  Rational conditionRat;
  if (!input[1].IsOfType<Rational>(&conditionRat)) {
    return false;
  }
  if (conditionRat.IsEqualToOne()) {
    output = input[2];
    return true;
  }
  if (conditionRat.IsEqualToZero()) {
    output = input[3];
    return true;
  }
  return false;
}

bool Calculator::outerMinus(Calculator& theCommands, const Expression& input, Expression& output) {
  if (!(input.StartsWith(theCommands.opMinus(), 3) || input.StartsWith(theCommands.opMinus(), 2))) {
    return false;
  }
  Expression tempE, minusOne;
  minusOne.AssignValue(- 1, theCommands);
  if (input.size() == 2) {
    output.MakeXOX(theCommands, theCommands.opTimes(), minusOne, input[1]);
  } else {
    tempE.MakeXOX(theCommands, theCommands.opTimes(), minusOne, input[2]);
    output.MakeXOX(theCommands, theCommands.opPlus(), input[1], tempE);
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
    otherCopy.AssignValue(other.theData, *this->owner);
    *this += otherCopy;
    return;
  }
  if (this->owner == nullptr) {
    this->AssignValue(this->theData, *other.owner);
  }
  if (this->owner != other.owner) {
    global.fatal << "Error: adding expressions with different owners. " << global.fatal;
  }
  Expression resultE;
  resultE.MakeXOX(*this->owner, this->owner->opPlus(), *this, other);
  *this = resultE;
}

void Expression::operator-=(const Expression& other) {
  MacroRegisterFunctionWithName("Expression::operator+=");
  if (this->owner == nullptr && other.owner == nullptr) {
    this->theData -= other.theData;
    if (this->theData != 1 && this->theData != 0) {
      global.fatal << "Attempting to subtract non-initialized expressions" << global.fatal;
    }
    return;
  }
  if (other.owner == nullptr) {
    Expression otherCopy;
    otherCopy.AssignValue(other.theData, *this->owner);
    (*this) -= otherCopy;
    return;
  }
  if (this->owner == nullptr) {
    this->AssignValue(this->theData, *other.owner);
  }
  if (this->owner != other.owner) {
    global.fatal << "Error: adding expressions with different owners. " << global.fatal;
  }
  Expression resultE;
  resultE.MakeXOX(*this->owner, this->owner->opMinus(), *this, other);
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
    //perhaps we should allow the code below for convenience: really
    //hard to judge if the convenience is worth it, or whether it will cause hard-to-detect bugs.
    //Rational resultRat = this->theData;
    //resultRat*= other;
    //if (resultRat.IsSmallInteger(&result.theData))
    //  return result;
    global.fatal << "Multiplying non-initialized expression with data: "
    << this->theData << " by integer " << other << " is not allowed. "
    << global.fatal;
  }
  Expression otherE;
  otherE.AssignValue(other, *this->owner);
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
  otherE.AssignValue(other, *this->owner);
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
      global.fatal << "Attempting to divide non-initialized expressions" << global.fatal;
    }
    return;
  }
  if (other.owner == nullptr) {
    Expression otherCopy;
    otherCopy.AssignValue(other.theData, *this->owner);
    (*this) /= otherCopy;
    return;
  }
  if (this->owner == nullptr) {
    this->AssignValue(this->theData, *other.owner);
  }
  if (this->owner != other.owner) {
    global.fatal << "Error: dividing expressions with different owners. " << global.fatal;
  }
  Expression resultE;
  resultE.MakeXOX(*this->owner, this->owner->opDivide(), *this, other);
  *this = resultE;
}

void Expression::operator*=(const Expression& other) {
  MacroRegisterFunctionWithName("Expression::operator*=");
  if (this->owner == nullptr && other.owner == nullptr) {
    this->theData *= other.theData;
    if (this->theData != 1 && this->theData != 0) {
      global.fatal << "Attempting to add non-initialized expressions" << global.fatal;
    }
    return;
  }
  if (other.owner == nullptr) {
    Expression otherCopy;
    otherCopy.AssignValue(other.theData, *this->owner);
    (*this) *= otherCopy;
    return;
  }
  if (this->owner == nullptr) {
    this->AssignValue(this->theData, *other.owner);
  }
  if (this->owner != other.owner) {
    global.fatal << "Error: adding expressions with different owners. " << global.fatal;
  }
  Expression resultE;
  resultE.MakeXOX(*this->owner, this->owner->opTimes(), *this, other);
  *this = resultE;
}

bool Expression::operator==(const Expression& other) const {
  if (this->owner != other.owner) {
    return false;
  }
  return this->theData == other.theData && this->children == other.children;
}

bool Expression::IsEqualToMathematically(const Expression& other) const {
  MacroRegisterFunctionWithName("Expression::IsEqualToMathematically");
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
  if (this->IsOfType(&theRat) && other.IsOfType(&theRatTwo)) {
    return theRat == theRatTwo;
  }
  if (this->IsOfType(&theRat) && other.IsOfType(&theAlgebraic)) {
    return theAlgebraic == theRat;
  }
  if (other.IsOfType(&theRat) && this->IsOfType(&theAlgebraic)) {
    return theAlgebraic == theRat;
  }
  double leftD = - 1, rightD = - 1;
  if (this->EvaluatesToDouble(&leftD) && other.EvaluatesToDouble(&rightD)) {
    return (leftD - rightD == 0.0);
  }
  Expression differenceE = *this;
  differenceE -= other;
  Expression differenceEsimplified;
  if (!this->owner->EvaluateExpression(*this->owner, differenceE, differenceEsimplified)) {
    return false;
  }
  if (differenceEsimplified.IsEqualToZero()) {
    return true;
  }
  if (differenceEsimplified.IsSequenceNElementS()) {
    for (int i = 1; i < differenceEsimplified.size(); i ++) {
      if (!differenceEsimplified[i].IsEqualToZero()) {
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
    if (!(*this)[i].IsEqualToMathematically(other[i])) {
      return false;
    }
  }
  return true;
}

SemisimpleLieAlgebra* Expression::GetAmbientSSAlgebraNonConstUseWithCaution() const {
  this->CheckInitialization();
  Expression myContext = this->GetContext();
  int indexSSalg = myContext.ContextGetIndexAmbientSSalg();
  if (indexSSalg == - 1) {
    return nullptr;
  }
  return &this->owner->theObjectContainer.theSSLieAlgebras.theValues[indexSSalg];
}

Function& Calculator::GetFunctionHandlerFromNamedRule(const std::string& inputNamedRule) {
  const Calculator::NamedRuleLocation& current =
  this->namedRules.GetValueConstCrashIfNotPresent(inputNamedRule);
  const MemorySaving<Calculator::OperationHandlers>& currentOperation =
  this->operations.GetValueConstCrashIfNotPresent(current.containerOperation);
  if (currentOperation.IsZeroPointer()) {
    global.fatal << "Named rule " << inputNamedRule
    << " registered with operation " << current.containerOperation
    << " but the operation has no handlers. " << global.fatal;
  }
  if (current.isComposite) {
    return currentOperation.GetElementConst().compositeHandlers[current.index];
  }
  return currentOperation.GetElementConst().handlers[current.index];
}

int Calculator::AddOperationNoRepetitionOrReturnIndexFirst(const std::string& theOpName) {
  int result = this->operations.GetIndex(theOpName);
  if (result == - 1) {
    result = this->operations.size();
    this->operations.GetValueCreate(theOpName);
  }
  return result;
}

void Calculator::AddOperationBuiltInType(const std::string& theOpName) {
  this->AddOperationNoRepetitionAllowed(theOpName);
  this->builtInTypes.AddOnTop(theOpName);
}

void Calculator::AddOperationNoRepetitionAllowed(const std::string& theOpName) {
  if (this->operations.Contains(theOpName)) {
    global.fatal << "This is a programming error: operation " << theOpName << " already created. " << global.fatal;
  }
  this->operations.GetValueCreate(theOpName);
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
    global.fatal << "Input options and target options have the same address. " << global.fatal;
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

void Calculator::AddOperationBinaryInnerHandlerWithTypes(
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
  int indexOp = this->operations.GetIndex(theOpName);
  if (indexOp == - 1) {
    indexOp = this->operations.size();
    this->operations.GetValueCreate(theOpName);
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
  innerFunction.theArgumentTypes.AddChildAtomOnTop(leftType);
  innerFunction.theArgumentTypes.AddChildAtomOnTop(rightType);
  this->RegisterCalculatorFunction(innerFunction, indexOp);
}

void Calculator::RegisterCalculatorFunction(Function& theFun, int indexOp) {
  MacroRegisterFunctionWithName("Calculator::RegisterCalculatorFunction");
  if (indexOp < 0 || indexOp >= this->operations.size()) {
    global.fatal << "Invalid index operation: " << indexOp
    << ", there are: " << this->operations.size()
    << " operations total." << global.fatal;
  }
  MemorySaving<Calculator::OperationHandlers>& handlerPointer = this->operations.theValues[indexOp];
  Calculator::OperationHandlers& handler = handlerPointer.GetElement();
  handler.CheckConsisitency();
  if (theFun.options.flagIsCompositeHandler) {
    theFun.indexInOperationHandlers = handler.compositeHandlers.size;
    handler.compositeHandlers.AddOnTop(theFun);
    if (handler.compositeHandlers.size <= 0) {
      global.fatal << "Composite handlers cannot be empty. " << global.fatal;
    }
  } else {
    theFun.indexInOperationHandlers = handler.handlers.size;
    handler.handlers.AddOnTop(theFun);
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
  this->namedRules.SetKeyValue(namedRule.containerOperation, namedRule);
}

void Calculator::AddOperationHandler(
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
  int indexOp = this->operations.GetIndex(theOpName);
  if (indexOp == - 1) {
    indexOp = this->operations.size();
    this->operations.GetValueCreate(theOpName);
  }
  int indexParentOpThatBansHandler = this->operations.GetIndex(parentOpThatBansHandler);
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
  this->RegisterCalculatorFunction(theFun, indexOp);
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
  bool argument1good = this->theArgumentTypes[0].theData == - 1 ? true : input[1].IsListStartingWithAtom(this->theArgumentTypes[0].theData);
  bool argument2good = this->theArgumentTypes[1].theData == - 1 ? true : input[2].IsListStartingWithAtom(this->theArgumentTypes[1].theData);
  return argument1good && argument2good;
}

std::string Function::ToStringShort() const {
  if (this->owner == nullptr) {
    return "(non-initialized)";
  }
  std::stringstream out;
  out << this->owner->operations.theKeys[this->indexOperation];
  MemorySaving<Calculator::OperationHandlers>& handlerPointer = this->owner->operations.theValues[this->indexOperation];
  Calculator::OperationHandlers& handler = handlerPointer.GetElement();
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

std::string Function::ToStringSummary() const {
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

bool Function::ShouldBeApplied(int parentOpIfAvailable) {
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
    if (!global.UserDefaultHasAdminRights()) {
      return (*this->owner) << "Rule " << this->calculatorIdentifier << " requires logged-in administrator account. ";
    }
  }
  return true;
}

JSData Function::ToJSON() const {
  MacroRegisterFunctionWithName("Function::ToJSON");
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
  Calculator::OperationHandlers& operationHandlers = this->owner->operations.theValues[this->indexOperation].GetElement();
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

std::string Function::ToStringFull() const {
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
    out2 << HtmlRoutines::GetHtmlSpanHidableStartsHiddeN(out.str());
    if (this->theExample != "") {
      out2 << "<a href=\"" << global.DisplayNameExecutable
      << "?request=calculator&showExamples = true&mainInput="
      << HtmlRoutines::ConvertStringToURLString(this->theExample, false)
      << "\"> " << " Example" << "</a>" ;
    }
  } else {
    out2 << "<b>Experimental, please don't use.</b>";
  }
  return out2.str();
}

std::string ObjectContainer::ToString() {
  MacroRegisterFunctionWithName("ObjectContainer::ToString");
  std::stringstream out;
  if (this->theSSLieAlgebras.theValues.size > 0) {
    out << "Lie algebras created (" << this->theSSLieAlgebras.theValues.size << " total): ";
    for (int i = 0; i < this->theSSLieAlgebras.theValues.size; i ++) {
      out << this->theSSLieAlgebras.theValues[i].ToStringLieAlgebraName();
      if (i != this->theSSLieAlgebras.theValues.size - 1) {
        out << ", ";
      }
    }
  }
  if (this->theSSSubalgebraS.theValues.size > 0) {
    out << "<br>Lie semisimple subalgebras computation data structures ("
    << this->theSSSubalgebraS.theValues.size << " total): ";
    for (int i = 0; i < this->theSSSubalgebraS.theValues.size; i ++) {
      out << " Type " << this->theSSSubalgebraS.theValues[i].owner->ToStringLieAlgebraName() << " with "
      << this->theSSSubalgebraS.theValues[i].theSubalgebras.theValues.size << " candidates";
      if (i != this->theSSSubalgebraS.theValues.size - 1) {
        out << ", ";
      }
    }
  }
  return out.str();
}

JSData Calculator::ToJSONOutputAndSpecials() {
  MacroRegisterFunctionWithName("Calculator::ToJSONOutputAndSpecials");
  JSData result = this->outputJS;
  if (this->inputString == "") {
    return result;
  }
  result["performance"] = this->ToStringPerformance();
  result["parsingLog"] = this->parsingLog;
  return result;
}

std::string Calculator::ToStringOutputAndSpecials() {
  MacroRegisterFunctionWithName("Calculator::ToStringOutputAndSpecials");
  return this->ToJSONOutputAndSpecials().ToString(false);
}

void Calculator::WriteAutoCompleteKeyWordsToFile() {
  MacroRegisterFunctionWithName("Calculator::WriteAutoCompleteKeyWordsToFile");
  std::stringstream out;
  out << "\"use strict\"; //This file is automatically generated, please do not modify.\n";
  out << "  var theAutocompleteDictionary = [\n  ";
  for (int i = 0; i < this->autoCompleteKeyWords.size; i ++) {
    if (this->autoCompleteKeyWords[i].size() > 2) {
      out << "\"" << HtmlRoutines::ConvertStringEscapeNewLinesQuotesBackslashes(this->autoCompleteKeyWords[i]) << "\"";
      if (i != this->autoCompleteKeyWords.size - 1) {
        out << ", ";
      }
    }
  }
  out << "];\n";
  out << "module.exports = {theAutocompleteDictionary};";
  std::fstream theFileStream;
  FileOperations::OpenFileCreateIfNotPresentVirtual(
    theFileStream, "/calculator-html/autocomplete_keywords.js",
    false, true, false, false
  );
  theFileStream << out.str();
}

void Calculator::ComputeAutoCompleteKeyWords() {
  MacroRegisterFunctionWithName("Calculator::ComputeAutoCompleteKeyWords");
  this->autoCompleteKeyWords.SetExpectedSize(this->operations.size() * 2);
  for (int i = 0; i < this->operations.size(); i ++) {
    this->autoCompleteKeyWords.AddOnTopNoRepetition(this->operations.theKeys[i]);
  }
  for (int i = 0; i < this->namedRules.size(); i ++) {
    this->autoCompleteKeyWords.AddOnTopNoRepetition(this->namedRules.theKeys[i]);
  }
  for (int i = 0; i < this->controlSequences.size; i ++) {
    if (this->controlSequences[i].size() > 0) {
      if (MathRoutines::isALatinLetter(this->controlSequences[i][0])) {
        autoCompleteKeyWords.AddOnTopNoRepetition(this->controlSequences[i]);
      }
    }
  }
}

std::string Calculator::ToStringPerformance() {
  MacroRegisterFunctionWithName("Calculator::ToStringPerformance");
  std::stringstream out;
  int64_t elapsedMilliseconds = global.GetElapsedMilliseconds();
  int64_t computationMilliseconds = elapsedMilliseconds - this->startTimeEvaluationMilliseconds;
  int64_t requestMilliseconds = elapsedMilliseconds - global.millisecondsComputationStart;
  int64_t waitingMilliseconds = elapsedMilliseconds - requestMilliseconds;
  out << "<b>Double-click formulas to get their LaTeX.</b>"
  << "<br>Double-click back to hide the LaTeX. ";
  out << "<br>Computation time: "
  << computationMilliseconds
  << " ms (~"
  << ((static_cast<double>(computationMilliseconds)) / 1000)
  << " s). ";
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
  << ( static_cast<double>(waitingMilliseconds) / 1000)
  << " s).";
  moreDetails << "<br>Expressions generated: " << this->theExpressionContainer.size << ". ";
  moreDetails << "<br>Expressions evaluated: " << this->stats.expressionEvaluated << ". ";
  moreDetails << "<br>Total number of pattern matches performed: "
  << this->TotalNumPatternMatchedPerformed << "";
  if (this->DepthRecursionReached > 0) {
    moreDetails << "<br>Maximum recursion depth reached: " << this->DepthRecursionReached << ".";
  }
  #ifdef MacroIncrementCounter
  moreDetails << "<br>Lists created: " << "computation: "
  << (ParallelComputing::NumListsCreated - static_cast<unsigned>(this->NumListsStart))
  << ", total: " << ParallelComputing::NumListsCreated;
  moreDetails << "<br> # List resizes: computation: "
  << (ParallelComputing::NumListResizesTotal - static_cast<unsigned>(this->NumListResizesStart))
  << ", total: " << ParallelComputing::NumListResizesTotal
  << "<br> # hash resizing: computation: " << (ParallelComputing::NumHashResizes - static_cast<unsigned>(this->NumHashResizesStart))
  << ", total: " << ParallelComputing::NumHashResizes;
  if (Rational::TotalSmallAdditions > 0) {
    moreDetails << "<br>Small rational additions: computation: "
    << Rational::TotalSmallAdditions - static_cast<unsigned long long>(this->NumSmallAdditionsStart)
    << ", total: " << Rational::TotalSmallAdditions;
  }
  if (Rational::TotalSmallMultiplications > 0) {
    moreDetails << "<br>Small rational multiplications: computation: "
    << Rational::TotalSmallMultiplications - static_cast<unsigned long long>(this->NumSmallMultiplicationsStart)
    << ", total: " << Rational::TotalSmallMultiplications;
  }
  if (Rational::TotalSmallGCDcalls > 0) {
    moreDetails << "<br>Small gcd calls: computation: "
    << Rational::TotalSmallGCDcalls - static_cast<unsigned long long>(this->NumSmallGCDcallsStart)
    << ", total: " << Rational::TotalSmallGCDcalls;
  }
  if (Rational::TotalLargeAdditions > 0) {
    moreDetails << "<br>Large integer additions: "
    << Rational::TotalLargeAdditions - static_cast<unsigned long long>(this->NumLargeAdditionsStart)
    << ", total: "
    << Rational::TotalLargeAdditions;
  }
  if (Rational::TotalLargeMultiplications > 0) {
    moreDetails << "<br>Large integer multiplications: computation: "
    << Rational::TotalLargeMultiplications - static_cast<unsigned long long>(this->NumLargeMultiplicationsStart)
    << ", total: " << Rational::TotalLargeMultiplications;
  }
  if (Rational::TotalLargeGCDcalls > 0) {
    moreDetails << "<br>Large gcd calls: "
    << Rational::TotalLargeGCDcalls - static_cast<unsigned long long>(this->NumLargeGCDcallsStart)
    << ", total: " << Rational::TotalLargeGCDcalls;
  }
  out << "<br>" << HtmlRoutines::GetHtmlSpanHidableStartsHiddeN(moreDetails.str(), "More details");
  #endif
  return out.str();
}

std::string Calculator::ToString() {
  MacroRegisterFunctionWithName("Calculator::ToString");
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
  if (this->RuleStack.children.size > 1) {
    out2 << "<b>Predefined rules.</b><br>";
    for (int i = 1; i < this->RuleStack.children.size; i ++) {
      out2 << this->RuleStack[i].ToString();
      if (i != this->RuleStack.children.size - 1) {
        out2 << "<br>";
      }
    }
  }

  if (!this->flagShowCalculatorExamples) {
    std::stringstream theExampleInjector;
    theExampleInjector
    << "if (document.getElementById('calculatorExamples').innerHTML.length < 300) {\n"
    << "  submitStringAsMainInput('', 'calculatorExamples', 'calculatorExamples', null);\n"
    << "} else {\n"
    << "  window.calculator.miscellaneousFrontend.switchMenu('calculatorExamples');\n"
    << "}\n";
    out2 << HtmlRoutines::GetHtmlButton("ShowCalculatorExamplesButton", theExampleInjector.str(), "Examples.");
    out2 << "<span id =\"calculatorExamples\"></span>";
  }
  if (!global.UserDebugFlagOn()) {
    return out2.str();
  }
  std::stringstream out;
  out2 << "<hr><b>Further calculator details.</b>";
  out << "<br><b>Object container information</b>. "
  << "The object container is the data structure storing all c++ built-in data types "
  << " requested by the user<br> " << this->theObjectContainer.ToString();
  out << "<hr>Control sequences (" << this->controlSequences.size << " total):\n<br>\n";
  for (int i = 0; i < this->controlSequences.size; i ++) {
    out << openTag1 << this->controlSequences[i] << closeTag1;
    if (i != this->controlSequences.size) {
      out << ", ";
    }
  }
  out << "<br>\n User or run-time defined atoms = " << this->operations.size() << " (= "
  << this->NumPredefinedAtoms << " predefined + "
  << this->operations.size() - this->NumPredefinedAtoms << " user-defined):<br>\n";
  for (int i = 0; i < this->operations.size(); i ++) {
    out << "\n" << i << ": " << openTag1 << this->operations.theKeys[i] << closeTag1;
    if (i != this->operations.size() - 1) {
      out << ", ";
    }
  }
  out << "<hr>";
  out << "Children expressions (" << this->theExpressionContainer.size << " total): <br>";
  int numExpressionsToDisplay = this->theExpressionContainer.size;
  if (this->theExpressionContainer.size > 1000) {
    numExpressionsToDisplay = 1000;
    out << " <b>Displaying first " << numExpressionsToDisplay << " only </b><br>";
  }
  for (int i = 0; i < numExpressionsToDisplay; i ++) {
    out << this->theExpressionContainer[i].ToString() << ", ";
  }
  out << "<hr>";
  out << "\n Cached expressions (" << this->cachedExpressions.size << " total):\n<br>\n";
  numExpressionsToDisplay = this->cachedExpressions.size;
  if (numExpressionsToDisplay > 1000) {
    numExpressionsToDisplay = 1000;
    out << "<b>Displaying first " << numExpressionsToDisplay << " expressions only. </b><br>";
  }
  for (int i = 0; i < numExpressionsToDisplay; i ++) {
    out << this->cachedExpressions[i].ToString() << " -> " << this->imagesCachedExpressions[i].ToString();
    if (i != this->cachedExpressions.size - 1) {
      out << "<br>";
    }
  }
  out2 << HtmlRoutines::GetHtmlSpanHidableStartsHiddeN(out.str(), "info expand/collapse", "calculatorInternalDetails");
  return out2.str();
}

std::string Calculator::ToStringSyntacticStackHumanReadable(
  bool includeLispifiedExpressions, bool ignoreCommandEnclosures
) {
  MacroRegisterFunctionWithName("Calculator::ToStringSyntacticStackHumanReadable");
  std::stringstream out;
  if ((*this->CurrentSyntacticStacK).size< this->numEmptyTokensStart) {
    return "Error: this is a programming error: not enough empty tokens in the start of the syntactic stack.";
  }
  bool isBad = ((*this->CurrentSyntacticStacK).size > this->numEmptyTokensStart + 1);
  SyntacticElement& lastSyntacticElt = *(*this->CurrentSyntacticStacK).LastObject();
  if ((*this->CurrentSyntacticStacK).size == this->numEmptyTokensStart + 1) {
    if (lastSyntacticElt.controlIndex != this->conExpression()) {
      isBad = true;
    }
  }
  if (!isBad) {
    out << this->CurrentSyntacticStacK->LastObject()->ToStringHumanReadable(*this, includeLispifiedExpressions);
    return out.str();
  }
  out << "<table style =\"vertical-align:top;border-spacing:0px 0px;\"><tr>";
  int counter = 0;
  for (int i = this->numEmptyTokensStart; i < (*this->CurrentSyntacticStacK).size; i ++) {
    SyntacticElement& currentElt = (*this->CurrentSyntacticStacK)[i];
    if (ignoreCommandEnclosures) {
      if (
        currentElt.controlIndex == this->conExpression() ||
        currentElt.controlIndex == this->conSequenceStatements()
      ) {
        if (currentElt.theData.StartsWith(this->opCommandEnclosure())) {
          continue;
        }
        if (currentElt.theData.StartsWith(this->opEndStatement())) {
          if (currentElt.theData.size() >= 2) {
            if (currentElt.theData[1].StartsWith(this->opCommandEnclosure())) {
              continue;
            }
          }
        }
      }
    }
    out
    << "<td style =\"vertical-align:top;background-color:"
    << ((counter % 2 == 0) ? "#FAFAFA" : "#F0F0F0" ) << "\">"
    << currentElt.ToStringHumanReadable(*this, includeLispifiedExpressions)
    << "</td>";
    counter ++;
  }
  out << "</tr></table>";
  return out.str();
}

std::string Calculator::ToStringSyntacticStackHTMLTable(bool ignoreCommandEnclosures) {
  return this->ToStringSyntacticStackHumanReadable(true, ignoreCommandEnclosures);
}

SemisimpleSubalgebras& ObjectContainer::GetSemisimpleSubalgebrasCreateIfNotPresent(const DynkinType& input) {
  MacroRegisterFunctionWithName("ObjectContainer::GetSemisimpleSubalgebrasCreateIfNotPresent");
  SemisimpleSubalgebras& currentSAs = this->theSSSubalgebraS.GetValueCreateNoInit(input);
  return currentSAs;
}

SemisimpleLieAlgebra& ObjectContainer::GetLieAlgebraCreateIfNotPresent(const DynkinType& input) {
  MacroRegisterFunctionWithName("ObjectContainer::GetLieAlgebraCreateIfNotPresent");
  bool needToInit = false;
  if (!this->theSSLieAlgebras.Contains(input)) {
    needToInit = true;
  }
  SemisimpleLieAlgebra& theLA = this->theSSLieAlgebras.GetValueCreateNoInit(input);
  if (needToInit) {
    theLA.theWeyl.MakeFromDynkinType(input);
  }
  return theLA;
}

WeylGroupData& ObjectContainer::GetWeylGroupDataCreateIfNotPresent(const DynkinType& input) {
  MacroRegisterFunctionWithName("ObjectContainer::GetWeylGroupDataCreateIfNotPresent");
  return this->GetLieAlgebraCreateIfNotPresent(input).theWeyl;
}

std::string ObjectContainer::ToStringJavascriptForUserInputBoxes() {
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
    << currentBox.GetSliderName() << "';\n";
  }
  //out << "console.log(window.calculator.calculator.inputBoxNames);\n ";
  out << "</script>";
  return out.str();
}

void ObjectContainer::resetPlots() {
  for (int i = 0; i < this->thePlots.size; i ++) {
    this->thePlots[i].flagDivAlreadyDisplayed = false;
  }
}

void ObjectContainer::resetSliders() {
  this->userInputBoxSliderDisplayed.initializeFillInObject(this->theUserInputTextBoxesWithValues.size(), false);
}

bool ObjectContainer::CheckConsistencyAfterReset() {
  MacroRegisterFunctionWithName("ObjectContainer::CheckConsistencyAfterReset");
  if (this->theWeylGroupElements.size != 0) {
    global.fatal << "WeylGroupElements expected to be empty, got " << this->theWeylGroupElements.size << " elements. " << global.fatal;
  }
  if (this->theSSLieAlgebras.size() != 0) {
    global.fatal << "theSSLieAlgebras expected to be empty, got " << this->theSSLieAlgebras.size() << " elements. " << global.fatal;
  }
  // MapReferences<DynkinType, SemisimpleSubalgebras> theSSSubalgebraS;
  if (this->theWeylGroupReps.size != 0) {
    global.fatal << "theWeylGroupReps expected to be empty, got " << this->theWeylGroupReps.size << " elements. " << global.fatal;
  }
  if (this->theWeylGroupVirtualReps.size != 0) {
    global.fatal << "theWeylGroupVirtualReps expected to be empty, got " << this->theWeylGroupVirtualReps.size << " elements. " << global.fatal;
  }
  // ListReferences<ModuleSSalgebra<RationalFunctionOld> > theCategoryOmodules;
  // ListReferences<SltwoSubalgebras> theSltwoSAs;
  // HashedListReferences<ElementEllipticCurve<ElementZmodP> > EllipticCurveElementsZmodP;
  // HashedListReferences<ElementEllipticCurve<Rational> > EllipticCurveElementsRational;
  // HashedListReferences<ElementTensorsGeneralizedVermas<RationalFunctionOld> > theTensorElts;

  if (this->thePolys.size != 0) {
    global.fatal << "The rational polynomials are expected to be empty, have: " << this->thePolys.size << " elements instead. " << global.fatal;
  }
  // HashedListReferences<Polynomial<AlgebraicNumber> > thePolysOverANs;
  // HashedListReferences<ElementWeylAlgebra<Rational> > theWeylAlgebraElements;
  // HashedListReferences<ElementUniversalEnveloping<RationalFunctionOld> > theUEs;
  // HashedListReferences<RationalFunctionOld> theRFs;
  if (this->theRationals.size != 0) {
    global.fatal << "Rationals expected to be empty, have: " << this->theRationals.size << " elements instead. " << global.fatal;
  }
  // HashedListReferences<charSSAlgMod<Rational> > theCharsSSLieAlgFD;
  if (this->theDoubles.size != 1) {
    global.fatal << "Doubles expected to be have exactly 1 element (namely, nan), have: " << this->theDoubles.size << " elements instead. " << global.fatal;
  }
  // HashedListReferences<std::string, MathRoutines::HashString> theStrings;
  // HashedListReferences<std::string, MathRoutines::HashString> ExpressionNotation;
  // HashedListReferences<Expression> ExpressionWithNotation;
  // HashedListReferences<LittelmannPath> theLSpaths;
  // HashedListReferences<MatrixTensor<Rational> > theMatTensorRats;
  // HashedListReferences<ElementZmodP> theEltsModP;
  // HashedListReferences<Weight<Rational> > theWeights;
  // HashedListReferences<Weight<Polynomial<Rational> > > theWeightsPoly;
  // ListReferences<GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational > > theHyperoctahedralReps;
  // ListReferences<Plot> thePlots;
  // List<bool> userInputBoxSliderDisplayed;
  // MapReferences<std::string, InputBox, MathRoutines::HashString> theUserInputTextBoxesWithValues;
  // MapReferences<std::string, std::string, MathRoutines::HashString> graphicsScripts;
  // AlgebraicClosureRationals theAlgebraicClosure;
  if (this->theAlgebraicNumbers.size != 0) {
    global.fatal << "Algebraic numbers expected to be empty, have: " << this->theAlgebraicNumbers.size << " elements instead. " << global.fatal;
  }
  // HashedListReferences<ElementHyperoctahedralGroupR2> theElementsHyperOctGroup;
  // ListReferences<HyperoctahedralGroupData> theHyperOctahedralGroups;
  // HashedListReferences<MonomialTensor<int, MathRoutines::IntUnsignIdentity> > theLittelmannOperators;
  // WeylGroupData& GetWeylGroupDataCreateIfNotPresent(const DynkinType& input);
  // SemisimpleLieAlgebra& GetLieAlgebraCreateIfNotPresent(const DynkinType& input);
  // SemisimpleSubalgebras& GetSemisimpleSubalgebrasCreateIfNotPresent(const DynkinType& input);
  return true;
}

void ObjectContainer::reset() {
  MacroRegisterFunctionWithName("ObjectContainer::reset");
  this->theWeylGroupElements.Clear();
  this->theWeylGroupReps.Clear();
  this->theWeylGroupVirtualReps.Clear();
  this->theCategoryOmodules.SetSize(0);
  this->theSSLieAlgebras.Clear();
  this->theSSSubalgebraS.Clear();
  this->theTensorElts.Clear();
  this->thePolys.Clear();
  this->thePolysOverANs.Clear();
  this->theWeylAlgebraElements.Clear();
  this->theUEs.Clear();
  this->theRFs.Clear();
  this->theRationals.Clear();
  this->theCharsSSLieAlgFD.Clear();
  this->theDoubles.Clear();
  this->theDoubles.AddOnTop(std::nan(""));
  this->theStrings.Clear();
  this->ExpressionNotation.Clear();
  this->ExpressionWithNotation.Clear();
  this->theLSpaths.Clear();
  //this->theMatRats.Clear();
  this->theMatTensorRats.Clear();
  //this->theMatRFs.Clear();
  this->theEltsModP.Clear();
  this->thePlots.SetSize(0);
  this->theAlgebraicClosure.reset();
  this->theAlgebraicNumbers.Clear();
  this->theLittelmannOperators.Clear();
  this->theSltwoSAs.SetSize(0);
  //this->theMatDoubles.SetSize(0);
  //this->theMatsAlgebraic.Clear();
  //this->theMatPolyRational.Clear();
  this->theWeights.Clear();
  this->theWeightsPoly.Clear();
  this->theHyperOctahedralGroups.SetSize(0);
  this->theElementsHyperOctGroup.Clear();
  this->CurrentRandomSeed = static_cast<int>(time(nullptr));
  this->theUserInputTextBoxesWithValues.Clear();
  this->graphicsScripts.Clear();
  this->EllipticCurveElementsZmodP.Clear();
  this->EllipticCurveElementsRational.Clear();
   //Setting up a random seed.
  srand (static_cast<unsigned>(this->CurrentRandomSeed));
}

template <>
bool CalculatorConversions::innerPolynomial<Rational>(Calculator& theCommands, const Expression& input, Expression& output);

bool Calculator::innerWriteGenVermaModAsDiffOperators(
  Calculator& theCommands, const Expression& input, Expression& output, bool AllGenerators,
  bool useNilWeight, bool ascending
) {
  MacroRegisterFunctionWithName("Calculator::innerWriteGenVermaModAsDiffOperators");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  Vectors<Polynomial<Rational> > theHWs;
  theHWs.SetSize(1);
  Expression theContext;
  Selection theParSel;
  SemisimpleLieAlgebra* theSSalgebra;
  Expression truncatedInput = input;
  if (truncatedInput.children.size > 4) {
    int numEltsToCut = truncatedInput.children.size - 4;
    for (int i = 0; i < numEltsToCut; i ++) {
      truncatedInput.children.RemoveLastObject();
    }
  }
  if (!theCommands.GetTypeHighestWeightParabolic<Polynomial<Rational> >(
    theCommands,
    truncatedInput,
    output,
    theHWs[0],
    theParSel,
    theContext,
    theSSalgebra,
    CalculatorConversions::innerPolynomial<Rational>)
  ) {
    return output.MakeError("Failed to extract type, highest weight, parabolic selection", theCommands);
  }
  if (output.IsError()) {
    return true;
  }
  std::string letterString = "x";
  std::string partialString = "\\partial";
  std::string exponentLetterString = "a";
  if (input.children.size > 4) {
    letterString = input[4].ToString();
  }
  if (input.children.size > 5) {
    partialString = input[5].ToString();
  }
  if (input.children.size > 6) {
    exponentLetterString = input[6].ToString();
  }
  return theCommands.innerWriteGenVermaModAsDiffOperatorInner(
    theCommands,
    input,
    output,
    theHWs,
    theContext,
    theParSel,
    theSSalgebra,
    AllGenerators,
    &letterString,
    &partialString,
    &exponentLetterString,
    useNilWeight,
    ascending
  );
}

bool Calculator::innerFreudenthalFull(Calculator& theCommands, const Expression& input, Expression& output) {
  Vector<Rational> hwFundamental, hwSimple;
  Selection tempSel;
  SemisimpleLieAlgebra* theSSalg;
  Expression context;
  if (!theCommands.GetTypeHighestWeightParabolic<Rational>(
    theCommands, input, output, hwFundamental, tempSel, context, theSSalg, nullptr
  )) {
    return output.MakeError("Failed to extract highest weight and algebra", theCommands);
  }
  if (output.IsError()) {
    return true;
  }
  if (tempSel.CardinalitySelection > 0) {
    return output.MakeError("Failed to extract highest weight. ", theCommands);
  }
  charSSAlgMod<Rational> startingChar, resultChar;
  hwSimple = theSSalg->theWeyl.GetSimpleCoordinatesFromFundamental(hwFundamental);
  startingChar.MakeFromWeight(hwSimple, theSSalg);
  std::string reportString;
  if (!startingChar.FreudenthalEvalMeFullCharacter(resultChar, 10000, &reportString)) {
    return output.MakeError(reportString, theCommands);
  }
  std::stringstream out;
  out << resultChar.ToString();
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerFreudenthalEval(Calculator& theCommands, const Expression& input, Expression& output) {
  Vector<Rational> hwFundamental, hwSimple;
  Selection tempSel;
  SemisimpleLieAlgebra* theSSalg = nullptr;
  Expression context;
  if (!theCommands.GetTypeHighestWeightParabolic<Rational>(
    theCommands, input, output, hwFundamental, tempSel, context, theSSalg, nullptr
  )) {
    return output.MakeError("Failed to extract highest weight and algebra", theCommands);
  }
  if (output.IsError()) {
    return true;
  }
  if (tempSel.CardinalitySelection > 0) {
    return output.MakeError("Failed to extract highest weight. ", theCommands);
  }
  charSSAlgMod<Rational> startingChar, resultChar;
  hwSimple = theSSalg->theWeyl.GetSimpleCoordinatesFromFundamental(hwFundamental);
  startingChar.MakeFromWeight(hwSimple, theSSalg);
  std::string reportString;
  if (!startingChar.FreudenthalEvalMeDominantWeightsOnly(resultChar, 10000, &reportString)) {
    return output.MakeError(reportString, theCommands);
  }
  return output.AssignValue(resultChar, theCommands);
}

bool Expression::IsMeltable(int* numResultingChildren) const {
  this->CheckInitialization();
  if (!this->StartsWith(this->owner->opMelt(), 2)) {
    return false;
  }
  if (numResultingChildren != nullptr) {
    if (!(*this)[1].StartsWith(this->owner->opEndStatement())) {
      *numResultingChildren = 1;
    } else {
      *numResultingChildren = (*this)[1].children.size - 1;
    }
  }
  return true;
}

bool Expression::MergeContextsMyAruments(Expression& output, std::stringstream* commentsOnFailure) const {
  MacroRegisterFunctionWithName("Expression::MergeContextsMyAruments");
  this->CheckInitialization();
  if (this->size() < 2) {
    return false;
  }
  for (int i = 1; i < this->size(); i ++) {
    if (!(*this)[i].IsBuiltInTypE()) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "<hr>Failed to merge the arguments of the expression" << this->ToString() << ": the argument "
        << (*this)[i].ToString() << "is not of built-in type";
      }
      return false;
    }
  }
  Expression commonContext = (*this)[1].GetContext();
  bool needsMerge = false;
  for (int i = 2; i < this->size(); i ++) {
    if (!(commonContext == (*this)[i].GetContext())) {
      needsMerge = true;
      break;
    }
  }
  if (!needsMerge) {
    output = *this;
    return true;
  }
  for (int i = 2; i < this->size(); i ++) {
    if (!(*this)[i].IsBuiltInTypE()) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "<hr>Failed to merge contexts of arguments: an argument is not of built-in type";
      }
      return false;
    }
    if (!commonContext.ContextMergeContexts(commonContext, (*this)[i].GetContext(), commonContext)) {
      *this->owner << "<hr>Failed to merge context " << commonContext.ToString()
      << " with " << (*this)[i].GetContext().ToString();
      return false;
    }
  }
  output.reset(*this->owner, this->size());
  output.AddChildOnTop((*this)[0]);
  Expression convertedE;
  for (int i = 1; i < this->size(); i ++) {
    convertedE = (*this)[i];
    if (!convertedE.SetContextAtLeastEqualTo(commonContext)) {
      *this->owner << "<hr>Failed to convert " << convertedE.ToString() << " to context " << commonContext.ToString();
      return false;
    }
    output.AddChildOnTop(convertedE);
  }
  return true;
}

bool Calculator::ConvertExpressionsToCommonContext(List<Expression>& inputOutputEs, Expression* inputOutputStartingContext) {
  MacroRegisterFunctionWithName("Calculator::ConvertExpressionsToCommonContext");
  Expression commonContext;
  commonContext.MakeEmptyContext(*this);
  if (inputOutputStartingContext != nullptr) {
    if (inputOutputStartingContext->IsContext()) {
      commonContext = *inputOutputStartingContext;
    }
  }
  for (int i = 0; i < inputOutputEs.size; i ++) {
    if (!inputOutputEs[i].IsBuiltInTypE()) {
      return *this << "<hr>Possible programming error: calling ConvertExpressionsToCommonContext "
      << "on expressions without context. "
      << global.fatal.GetStackTraceEtcErrorMessageHTML();
    }
    if (!commonContext.ContextMergeContexts(commonContext, inputOutputEs[i].GetContext(), commonContext)) {
      return *this << "<hr>Failed to merge context "
      << commonContext.ToString() << " with " << inputOutputEs[i].GetContext().ToString();
    }
  }
  for (int i = 0; i < inputOutputEs.size; i ++) {
    if (!inputOutputEs[i].::Expression::SetContextAtLeastEqualTo(commonContext)) {
      return false;
    }
  }
  if (inputOutputStartingContext != nullptr) {
    *inputOutputStartingContext = commonContext;
  }
  return true;
}

bool Calculator::outerMeltBrackets(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::outerMeltBrackets");
  RecursionDepthCounter theCounter(&theCommands.RecursionDeptH);
  if (!input.StartsWith(theCommands.opEndStatement())) {
    return false;
  }
  int tempInt;
  int ChildIncrease = 0;
  bool found = false;
  for (int i = 0; i < input.children.size; i ++) {
    const Expression& currentChild = input[i];
    if (currentChild.IsMeltable(&tempInt)) {
      found = true;
      ChildIncrease += tempInt - 1;
    }
  }
  if (!found) {
    return false;
  }
  output.reset(theCommands, input.children.size + ChildIncrease);
  output.AddChildAtomOnTop(theCommands.opEndStatement());
  for (int i = 1; i < input.children.size; i ++) {
    const Expression& currentChild = input[i];
    if (!currentChild.IsMeltable()) {
      output.AddChildOnTop(input[i]);
      continue;
    }
    if (!currentChild[1].StartsWith(theCommands.opEndStatement())) {
      output.AddChildOnTop(currentChild[1]);
      continue;
    }
    for (int j = 1; j < currentChild[1].children.size; j ++) {
      output.AddChildOnTop(currentChild[1][j]);
    }
  }
  return true;
}
