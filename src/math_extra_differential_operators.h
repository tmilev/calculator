// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef MATH_EXTRA_DIFFERENTIAL_OPERATORS_INCLUDED
#define MATH_EXTRA_DIFFERENTIAL_OPERATORS_INCLUDED
#include "math_general.h"

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
  std::string toString(FormatExpressions* format = nullptr) const;
};

template <class Coefficient>
class QuasiDifferentialOperator : public LinearCombination<QuasiDifferentialMononomial, Coefficient> {
public:
  std::string toString(FormatExpressions* format = nullptr) const;
  void generateBasisLieAlgebra(List<QuasiDifferentialOperator<Coefficient> >& theElts, FormatExpressions* format = nullptr);
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

template <class Coefficient>
void QuasiDifferentialOperator<Coefficient>::generateBasisLieAlgebra(
  List<QuasiDifferentialOperator<Coefficient> >& theElts, FormatExpressions* format
) {
  MacroRegisterFunctionWithName("QuasiDifferentialOperator::generateBasisLieAlgebra");
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
        << tempQDO.toString(format) << "<br> with element <br>"
        << theEltsConverted[j].toString(format) << " to get <br>";
        tempQDO.lieBracketMeOnTheRight(theEltsConverted[j]);
        theReport.report(report.str());
        report << tempQDO.toString(format);
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
std::string QuasiDifferentialOperator<Coefficient>::toString(FormatExpressions* format) const {
  bool combineWeylPart = true;
  if (format != nullptr) {
    combineWeylPart = format->flagQuasiDiffOpCombineWeylPart;
  }
  if (!combineWeylPart) {
    return this->LinearCombination<QuasiDifferentialMononomial, Coefficient>::toString(format);
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
  std::string result = reordered.toString(format);
  if (result == "0" && this->size() != 0) {
    global.fatal << "This is likely a programming error (crashing at any rate): "
    << "I have a non-zero quasidifferential operator "
    << " with non-properly formatted LaTeX string "
    << this->LinearCombination<QuasiDifferentialMononomial, Coefficient>::toString(format)
    << ", however its properly formatted string is 0. "
    << "Probably there is something wrong with the initializations of the monomials of the qdo. " << global.fatal;
  }
  return result;
}

#endif
