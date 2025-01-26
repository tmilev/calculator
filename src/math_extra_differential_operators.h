#ifndef header_math_extra_differential_operators_ALREADY_INCLUDED
#define header_math_extra_differential_operators_ALREADY_INCLUDED

#include "math_general.h"
#include "math_weyl_algebras.h"
#include "progress_report.h"

class QuasiDifferentialMononomial {
  friend std::ostream& operator<<(
    std::ostream& output, const QuasiDifferentialMononomial& monomial
  ) {
    output << monomial.toString();
    return output;
  }
public:
  MonomialWeylAlgebra weylMonomial;
  MonomialMatrix matrixMonomial;
  static unsigned int hashFunction(const QuasiDifferentialMononomial& input) {
    return
    input.weylMonomial.hashFunction() * HashConstants::constant0 +
    input.matrixMonomial.hashFunction() * HashConstants::constant1;
  }
  unsigned int hashFunction() const {
    return hashFunction(*this);
  }
  bool operator==(const QuasiDifferentialMononomial& other) const {
    return
    this->weylMonomial == other.weylMonomial &&
    this->matrixMonomial == other.matrixMonomial;
  }
  void operator=(const QuasiDifferentialMononomial& other) {
    this->weylMonomial = other.weylMonomial;
    this->matrixMonomial = other.matrixMonomial;
  }
  bool operator>(const QuasiDifferentialMononomial& other) const {
    if (this->matrixMonomial == other.matrixMonomial) {
      return this->weylMonomial > other.weylMonomial;
    }
    return this->matrixMonomial > other.matrixMonomial;
  }
  std::string toString(FormatExpressions* format = nullptr) const;
};

template <class Coefficient>
class QuasiDifferentialOperator: public LinearCombination<
  QuasiDifferentialMononomial, Coefficient
> {
public:
  std::string toString(FormatExpressions* format = nullptr) const;
  void generateBasisLieAlgebra(
    List<QuasiDifferentialOperator<Coefficient> >& inputElements,
    FormatExpressions* format =
    nullptr
  );
  void operator*=(
    const QuasiDifferentialOperator<Coefficient>& standsOnTheRight
  );
  void operator=(
    const LinearCombination<QuasiDifferentialMononomial, Coefficient>& other
  ) {
    this->LinearCombination<QuasiDifferentialMononomial, Coefficient>::operator
    =(other);
  }
  void lieBracketMeOnTheRight(
    const LinearCombination<QuasiDifferentialMononomial, Coefficient>&
    standsOnTheRight
  ) {
    QuasiDifferentialOperator<Coefficient> tempRight;
    tempRight = standsOnTheRight;
    MathRoutines::lieBracket(*this, tempRight, *this);
  }
  void fourierTransformDifferentialPartOnly(
    QuasiDifferentialOperator<Coefficient>& output
  ) const;
  void getElementWeylAlgebraSetMatrixPartsToId(
    ElementWeylAlgebra<Coefficient>& output
  ) const;
};

template <class Coefficient>
void QuasiDifferentialOperator<Coefficient>::
fourierTransformDifferentialPartOnly(
  QuasiDifferentialOperator<Coefficient>& output
) const {
  if (&output == this) {
    QuasiDifferentialOperator<Coefficient> thisCopy;
    thisCopy.fourierTransformDifferentialPartOnly(output);
    return;
  }
  output.makeZero();
  ElementWeylAlgebra<Coefficient>
  startDifferentialOperator,
  finalDifferentialOperator;
  QuasiDifferentialMononomial monomial;
  for (int i = 0; i < this->size(); i ++) {
    startDifferentialOperator.makeZero();
    startDifferentialOperator.addMonomial((*this)[i].weylMonomial, this->
      coefficients[i]
    );
    startDifferentialOperator.fourierTransform(finalDifferentialOperator);
    for (int j = 0; j < finalDifferentialOperator.size(); j ++) {
      monomial.matrixMonomial = (*this)[i].matrixMonomial;
      monomial.weylMonomial = finalDifferentialOperator[j];
      output.addMonomial(monomial, finalDifferentialOperator.coefficients[j]);
    }
  }
}

template <class Coefficient>
void QuasiDifferentialOperator<Coefficient>::
getElementWeylAlgebraSetMatrixPartsToId(
  ElementWeylAlgebra<Coefficient>& output
) const {
  output.makeZero();
  for (int i = 0; i < this->size(); i ++) {
    output.addMonomial((*this)[i].weylMonomial, this->coefficients[i]);
  }
}

template <class Coefficient>
void QuasiDifferentialOperator<Coefficient>::generateBasisLieAlgebra(
  List<QuasiDifferentialOperator<Coefficient> >& inputElements,
  FormatExpressions* format
) {
  STACK_TRACE("QuasiDifferentialOperator::generateBasisLieAlgebra");
  ProgressReport report;
  HashedList<QuasiDifferentialMononomial> bufferMons;
  List<LinearCombination<QuasiDifferentialMononomial, Coefficient> >
  elementsConverted;
  elementsConverted = inputElements;
  this->gaussianEliminationByRows(elementsConverted);
  QuasiDifferentialOperator tempQDO;
  bool foundNew = true;
  while (foundNew) {
    foundNew = false;
    for (int i = 0; i < elementsConverted.size; i ++) {
      for (int j = i + 1; j < elementsConverted.size; j ++) {
        tempQDO = elementsConverted[i];
        std::stringstream reportStream;
        reportStream
        << "Lie bracketing elements "
        << " of indices "
        << i + 1
        << " and "
        << j + 1
        << " out of "
        << elementsConverted.size
        << "<br> "
        << tempQDO.toString(format)
        << "<br> with element <br>"
        << elementsConverted[j].toString(format)
        << " to get <br>";
        tempQDO.lieBracketMeOnTheRight(elementsConverted[j]);
        report.report(reportStream.str());
        report << tempQDO.toString(format);
        report.report(reportStream.str());
        elementsConverted.addOnTop(tempQDO);
        QuasiDifferentialOperator::gaussianEliminationByRows(
          elementsConverted, 0, &bufferMons
        );
        if (!elementsConverted.lastObject()->isEqualToZero()) {
          foundNew = true;
        }
        for (int k = elementsConverted.size - 1; k >= 0; k --) {
          if (elementsConverted[k].isEqualToZero()) {
            elementsConverted.removeIndexSwapWithLast(k);
          } else {
            break;
          }
        }
      }
    }
  }
  inputElements.setSize(elementsConverted.size);
  for (int i = 0; i < elementsConverted.size; i ++) {
    inputElements[i] = elementsConverted[i];
  }
}

template <class Coefficient>
void QuasiDifferentialOperator<Coefficient>::operator*=(
  const QuasiDifferentialOperator<Coefficient>& standsOnTheRight
) {
  QuasiDifferentialOperator<Coefficient> output;
  ElementWeylAlgebra<Rational> leftElement, rightElement;
  QuasiDifferentialMononomial outputMon;
  output.makeZero();
  for (int j = 0; j < standsOnTheRight.size(); j ++) {
    rightElement.makeZero();
    rightElement.addMonomial(
      standsOnTheRight[j].weylMonomial, standsOnTheRight.coefficients[j]
    );
    for (int i = 0; i < this->size(); i ++) {
      leftElement.makeZero();
      leftElement.addMonomial((*this)[i].weylMonomial, this->coefficients[i]);
      outputMon.matrixMonomial = (*this)[i].matrixMon;
      outputMon.matrixMonomial *= standsOnTheRight[j].matrixMon;
      leftElement *= rightElement;
      for (int k = 0; k < leftElement.size(); k ++) {
        outputMon.weylMonomial = leftElement[k];
        output.addMonomial(outputMon, leftElement.coefficients[k]);
      }
    }
  }
  *this = output;
}

template <class Coefficient>
std::string QuasiDifferentialOperator<Coefficient>::toString(
  FormatExpressions* format
) const {
  bool combineWeylPart = true;
  if (format != nullptr) {
    combineWeylPart = format->flagQuasiDiffOpCombineWeylPart;
  }
  if (!combineWeylPart) {
    return
    this->LinearCombination<QuasiDifferentialMononomial, Coefficient>::toString
    (format);
  }
  MatrixTensor<ElementWeylAlgebra<Rational> > reordered;
  reordered.makeZero();
  ElementWeylAlgebra<Rational> polynomial;
  for (int i = 0; i < this->size(); i ++) {
    const QuasiDifferentialMononomial& monomial = (*this)[i];
    polynomial.makeZero();
    polynomial.addMonomial(monomial.weylMonomial, this->coefficients[i]);
    reordered.addMonomial(monomial.matrixMonomial, polynomial);
  }
  std::string result = reordered.toString(format);
  if (result == "0" && this->size() != 0) {
    global.fatal
    << "This is likely a programming error (crashing at any rate): "
    << "I have a non-zero quasidifferential operator "
    << "with non-properly formatted LaTeX string "
    << this->LinearCombination<QuasiDifferentialMononomial, Coefficient>::
    toString(format)
    << ", however its properly formatted string is 0. "
    << "Probably there is something wrong with "
    << "the initializations of the monomials of the qdo. "
    << global.fatal;
  }
  return result;
}

#endif // header_math_extra_differential_operators_ALREADY_INCLUDED
