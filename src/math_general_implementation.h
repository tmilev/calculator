#ifndef math_general_implementation_H
#define math_general_implementation_H
#include "math_general.h"

template <class Coefficient>
void Matrix<Coefficient>::computeDeterminantOverwriteMatrix(
  Coefficient& output,
  const Coefficient& ringOne,
  const Coefficient& ringZero
) {
  MacroRegisterFunctionWithName("Matrix::computeDeterminantOverwriteMatrix");
  bool doReport = this->numberOfColumns > 10 && this->numberOfRows > 10 && this->numberOfColumns * this->numberOfRows >= 400;
  ProgressReport report(1, GlobalVariables::Response::ReportType::gaussianElimination);
  ProgressReport report2(400, GlobalVariables::Response::ReportType::gaussianElimination);
  int pivotIndex = 0;
  output = ringOne;
  Coefficient scalar;
  if (this->numberOfColumns != this->numberOfRows) {
    global.fatal << "Error: determinant computation: number of columns different from number of rows. " << global.fatal;
  }
  int dim = this->numberOfColumns;
  for (int i = 0; i < dim; i ++) {
    pivotIndex = this->findPivot(i, i);
    if (pivotIndex == - 1) {
      output = ringZero;
      return;
    }
    this->switchRows(i, pivotIndex);
    if (pivotIndex != i) {
      output *= - 1;
    }
    scalar = this->elements[i][i];
    output *= scalar;
    scalar.invert();
    this->rowTimesScalar(i, scalar);
    if (doReport) {
      if (report.tickAndWantReport()) {
        std::stringstream reportStream;
        reportStream << "Pivot row " << i + 1 << " out of " << dim << ": ";
        for (int colCounter = 0; colCounter < this->numberOfColumns; colCounter ++) {
          reportStream << (*this)(i, colCounter).toString();
          if (colCounter != this->numberOfColumns - 1) {
            reportStream << ", ";
          }
        }
        report.report(reportStream.str());
      }
    }
    for (int j = i + 1; j < dim; j ++) {
      if (!this->elements[j][i].isEqualToZero()) {
        scalar = this->elements[j][i];
        scalar.negate();
        this->addTwoRows(i, j, i, scalar);
        if (doReport) {
          if (report2.tickAndWantReport()) {
            std::stringstream reportStream;
            reportStream << "Computing large determinant: pivot " << i + 1 << ", row " << j << " out of "
            << dim <<  " times  " << dim << " total.";
            report2.report(reportStream.str());
          }
        }
      }
    }
  }
}

template<class Coefficient>
std::ostream& operator<< (std::ostream& output, const Matrix<Coefficient>& matrix) {
  output << "\\left(\\begin{array}{";
  for (int j = 0; j < matrix.numberOfColumns; j ++) {
    output << "c";
  }
  output << "}";
  int firstMatRowIndexToHide = matrix.numberOfRows;
  int lastMatRowIndexToHide = matrix.numberOfRows;
  int firstMatColIndexToHide = matrix.numberOfColumns;
  int lastMatColIndexToHide = matrix.numberOfColumns;
  FormatExpressions& format = global.defaultFormat.getElement();
  if (format.flagSuppressLongMatrices) {
    if (matrix.numberOfRows > format.maximumMatrixDisplayedRows) {
      firstMatRowIndexToHide = format.maximumMatrixDisplayedRows / 2;
      lastMatRowIndexToHide = matrix.numberOfRows - 1 - format.maximumMatrixDisplayedRows / 2;
    }
    if (matrix.numberOfColumns > format.maximumMatrixLineLength) {
      firstMatColIndexToHide = format.maximumMatrixLineLength / 2;
      lastMatColIndexToHide = matrix.numberOfColumns - 1 - format.maximumMatrixLineLength / 2;
    }
  }
  for (int i = 0; i < matrix.numberOfRows; i ++) {
    if (firstMatRowIndexToHide <= i && i <= lastMatRowIndexToHide) {
      if (firstMatRowIndexToHide == i) {
        output << "...\\\\\n";
      }
      continue;
    }
    for (int j = 0; j < matrix.numberOfColumns; j ++) {
      if (firstMatColIndexToHide <= j && j <= lastMatColIndexToHide) {
        if (firstMatColIndexToHide == j) {
          output << "...&";
        }
        continue;
      }
      output << matrix(i, j);
      if (j != matrix.numberOfColumns - 1) {
        output << " & ";
      } else {
        output << "\\\\";
      }
    }
    output << "\n";
  }
  output << "\\end{array}\\right)";
  return output;
}

template <class Coefficient, typename IntegerType>
void MathRoutines::raiseToPower(Coefficient& element, const IntegerType& power, const Coefficient& ringUnit
) {
  MacroRegisterFunctionWithName("MathRoutines::raiseToPower");
  IntegerType powerCopy;
  powerCopy = power;
  if (powerCopy < 0) {
    return;
  }
  if (powerCopy == 1) {
    return;
  }
  if (powerCopy == 0) {
    element = ringUnit;
    return;
  }
  ProgressReport reportOne;
  ProgressReport reportTwo(32, GlobalVariables::Response::ReportType::general);
  Coefficient squares;
  squares = element;
  if (powerCopy < 4) {
    for (IntegerType i = 1; i < powerCopy; i ++) {
      element *= squares;
    }
    return;
  }
  if (reportOne.tickAndWantReport()) {
    std::stringstream reportStream;
    reportStream << "Raising " << element.toString()
    << " to power: " << powerCopy;
    reportOne.report(reportStream.str());
  }
  element = ringUnit;
  while (powerCopy > 0) {
    if (powerCopy % 2 == 1) {
      if (reportTwo.tickAndWantReport()) {
        std::stringstream reportStream2;
        reportStream2 << "Remaining exponent: " << powerCopy << "<br>";
        reportStream2 << "Multiplying " << element.toString()
        << " by " << squares.toString();
        reportTwo.report(reportStream2.str());
      }
      element *= squares;
    }
    powerCopy /= 2;
    if (powerCopy > 0) {
      if (reportTwo.tickAndWantReport()) {
        std::stringstream reportStream2;
        reportStream2 << "Remaining exponent: " << powerCopy << "<br>";
        reportStream2 << "Squaring: " << squares.toString();
        reportTwo.report(reportStream2.str());
      }
      squares *= squares;
    }
  }
}

template <class TemplateMonomial, class Coefficient>
void ElementMonomialAlgebra<TemplateMonomial, Coefficient>::multiplyBy(
  const ElementMonomialAlgebra<TemplateMonomial, Coefficient>& other,
  ElementMonomialAlgebra<TemplateMonomial, Coefficient>& output,
  ElementMonomialAlgebra<TemplateMonomial, Coefficient>& bufferPoly,
  TemplateMonomial& bufferMon
) const {
  MacroRegisterFunctionWithName("ElementMonomialAlgebra::multiplyBy");
  if (other.isEqualToZero()) {
    output.makeZero();
    return;
  }
  if (&bufferPoly == this || &bufferPoly == &other) {
    global.fatal << "Bad buffer in ElementMonomialAlgebra::multiplyBy." << global.fatal;
  }
  this->checkConsistency();
  other.checkConsistency();
  int maximumMonomials = this->size() * other.size();
  if (maximumMonomials > 2000000) {
    maximumMonomials = 2000000;
  }
  int totalMonPairs = 0;
  ProgressReport report1;
  ProgressReport report2(400, GlobalVariables::Response::ReportType::monomialAlgebraProduct);
  if (report1.tickAndWantReport()) {
    totalMonPairs = other.size() * this->size();
    std::stringstream reportStream;
    reportStream << "Large polynomial computation: " << this->size() << " x "
    << other.size() << "=" << totalMonPairs << " monomials:\n<br>\n"
    << this->toString() << " times " << other.toString();
    report1.report(reportStream.str());
  }
  bufferPoly.makeZero();
  bufferPoly.setExpectedSize(maximumMonomials);
  bufferPoly.checkConsistency();
  bufferMon.checkConsistency();
  Coefficient current;
  for (int i = 0; i < other.size(); i ++) {
    for (int j = 0; j < this->size(); j ++) {
      if (report2.tickAndWantReport()) {
        std::stringstream reportStream2;
        reportStream2 << "Multiplying monomials: "
        << i + 1 << " out of " << other.size() << " by " << j + 1
        << " out of " << this->size() << ". ";
        report2.report(reportStream2.str());
      }
      bufferMon = (*this)[j];
      bufferMon *= other[i];
      current = this->coefficients[j];
      current *= other.coefficients[i];
      bufferPoly.addMonomial(bufferMon, current);
    }
  }
  output = bufferPoly;
}

template <class TemplateMonomial, class Coefficient>
void ElementMonomialAlgebra<TemplateMonomial, Coefficient>::multiplyBy(
  const TemplateMonomial& other,
  ElementMonomialAlgebra<TemplateMonomial, Coefficient>& output
) const {
  if (this == &output) {
    ElementMonomialAlgebra<TemplateMonomial, Coefficient> thisCopy = *this;
    thisCopy.multiplyBy(other, output);
    return;
  }
  output.makeZero();
  TemplateMonomial currentMonomial;
  for (int i = 0; i < this->size(); i ++) {
    currentMonomial = this->monomials[i];
    currentMonomial *= other;
    output.addMonomial(currentMonomial, this->coefficients[i]);
  }
}

template<class Coefficient>
void MatrixTensor<Coefficient>::raiseToPower(int power) {
  if (power <= 0) {
    global.fatal << "MatrixTensor::raiseToPower is currently implemented for positive integer power only. " << global.fatal;
  }
  MatrixTensor<Coefficient> id;
  id.makeIdentitySpecial();
  MathRoutines::raiseToPower(*this, power, id);
}

template <class Coefficient>
void MatrixTensor<Coefficient>::operator*=(
  const MatrixTensor<Coefficient>& other
) {
  MacroRegisterFunctionWithName("MatrixTensor::multiplyBy");
  if (other.isEqualToZero()) {
    this->makeZero();
    return;
  }
  this->checkConsistency();
  other.checkConsistency();
  int maxNumMonsFinal = this->size() * other.size();
  if (maxNumMonsFinal > 2000000) {
    maxNumMonsFinal = 2000000;
  }
  int totalMonPairs = 0;
  ProgressReport report1;
  ProgressReport report2(400, GlobalVariables::Response::ReportType::monomialAlgebraProduct);
  if (report1.tickAndWantReport()) {
    totalMonPairs = other.size() * this->size();
    std::stringstream reportStream;
    reportStream << "Large matrix monomial computation: " << this->size() << " x "
    << other.size() << "=" << totalMonPairs << " monomials:\n<br>\n"
    << this->toString() << " times " << other.toString();
    report1.report(reportStream.str());
  }
  MatrixTensor<Coefficient> result;
  result.setExpectedSize(maxNumMonsFinal);
  Coefficient currentCoefficient;
  MonomialMatrix currentMonomial;
  for (int i = 0; i < other.size(); i ++) {
    for (int j = 0; j < this->size(); j ++) {
      if (report2.tickAndWantReport()) {
        std::stringstream reportStream2;
        reportStream2 << "Multiplying monomials: "
        << i + 1 << " out of " << other.size() << " by " << j + 1
        << " out of " << this->size() << ". ";
        report2.report(reportStream2.str());
      }
      currentMonomial = (*this)[j];
      currentMonomial *= other[i];
      if (currentMonomial.isZeroMonomial()) {
        continue;
      }
      currentCoefficient = this->coefficients[j];
      currentCoefficient *= other.coefficients[i];
      result.addMonomial(currentMonomial, currentCoefficient);
    }
  }
  *this = result;
}

template <class Coefficient>
void Matrix<Coefficient>::gaussianEliminationEuclideanDomain(
  Matrix<Coefficient>* otherMatrix,
  const Coefficient& negativeOne,
  const Coefficient& ringUnit,
  bool (*comparisonGEQFunction) (const Coefficient& left, const Coefficient& right)
) {
  MacroRegisterFunctionWithName("Matrix_Element::gaussianEliminationEuclideanDomain");
  ProgressReport report(1, GlobalVariables::Response::ReportType::gaussianElimination);
  if (otherMatrix == this) {
    global.fatal << "The Carbon copy in the Gaussian elimination "
    << "coincides with the matrix which we are row-reducing "
    << "(most probably this is a wrong pointer typo). " << global.fatal;
  }
  int col = 0;
  Coefficient element;
  int row = 0;
  while (row < this->numberOfRows && col < this->numberOfColumns) {
    int foundPivotRow = - 1;
    for (int i = row; i < this->numberOfRows; i ++) {
      if (!this->elements[i][col].isEqualToZero()) {
        foundPivotRow = i;
        break;
      }
    }
    if (foundPivotRow != - 1) {
      this->switchRowsWithCarbonCopy(row, foundPivotRow, otherMatrix);
      if (this->elements[row][col].isNegative()) {
        this->rowTimesScalarWithCarbonCopy(row, negativeOne, otherMatrix);
      }
      int ExploringRow = row + 1;
      while (ExploringRow < this->numberOfRows) {
        if (report.tickAndWantReport()) {
          std::stringstream out;
          out << "Pivotting on row of index " << row + 1 << " with exploring row of index "
          << ExploringRow + 1 << "; total rows: " << this->numberOfRows;
          report.report(out.str());
        }
        Coefficient& pivotElement = this->elements[row][col];
        Coefficient& otherElt = this->elements[ExploringRow][col];
        if (otherElt.isNegative()) {
          this->rowTimesScalarWithCarbonCopy(ExploringRow, negativeOne, otherMatrix);
        }
        bool isSmallerOrEqualTo = comparisonGEQFunction == 0 ? pivotElement <= otherElt :
        comparisonGEQFunction(otherElt, pivotElement);
        if (isSmallerOrEqualTo) {
          element = otherElt;
          element /= pivotElement;
          element.assignFloor();
          this->subtractRowsWithCarbonCopy(ExploringRow, row, 0, element, otherMatrix);
        }
        if (this->elements[ExploringRow][col].isEqualToZero()) {
          ExploringRow ++;
        } else {
          this->switchRowsWithCarbonCopy(ExploringRow, row, otherMatrix);
        }
      }
      Coefficient& pivotElement = this->elements[row][col];
      for (int i = 0; i < row; i ++) {
        element = this->elements[i][col];
        element /= pivotElement;
        element.assignFloor();
        this->subtractRowsWithCarbonCopy(i, row, 0, element, otherMatrix);
        if (this->elements[i][col].isNegative()) {
          this->addTwoRowsWithCarbonCopy(row, i, 0, ringUnit, otherMatrix);
        }
      }
      row ++;
    }
    col ++;
  }
}

template <class Coefficient>
void Vectors<Coefficient>::chooseABasis() {
  Vectors<Rational> output;
  Matrix<Coefficient> toBeEliminated;
  Selection selection;
  for (int i = 0; i < this->size; i ++) {
    output.addOnTop(this->objects[i]);
    if (output.getRankElementSpan(&toBeEliminated, &selection) < output.size) {
      output.removeLastObject();
    }
  }
  this->operator=(output);
}

template <class Coefficient>
void Vectors<Coefficient>::beefUpWithEiToLinearlyIndependentBasis(int dimension) {
  Selection selection;
  Matrix<Coefficient> buffer;
  if (this->size != 0 && dimension != this->getDimension()) {
    global.fatal << "Vector dimension is incorrect. " << global.fatal;
  }
  int currentRank = this->getRankElementSpan(buffer, selection);
  if (currentRank == dimension) {
    return;
  }
  Vector<Coefficient> vector;
  for (int i = 0; i < dimension && currentRank < dimension; i ++) {
    vector.makeEi(dimension, i);
    this->addOnTop(vector);
    int candidateRank = this->getRankElementSpan(buffer, selection);
    if (candidateRank > currentRank) {
      currentRank = candidateRank;
    } else {
      this->size --;
    }
  }
  if (currentRank != dimension) {
    global.fatal << "Rank does not match dimension. " << global.fatal;
  }
}

template <class Coefficient>
bool Vectors<Coefficient>::linearSpanContainsVector(const Vector<Coefficient>& input) const {
  Matrix<Coefficient> buffer;
  Selection bufferSelection;
  return this->linearSpanContainsVector(input, buffer, bufferSelection);
}

template <class Coefficient>
void Vectors<Coefficient>::selectBasisInSubspace(
  const List<Vector<Coefficient> >& input, List<Vector<Coefficient> >& output, Selection& outputSelectedPivotColumns
) {
  if (&input == &output) {
    List<Vector<Coefficient> > inputCopy = input;
    Vectors<Coefficient>::selectBasisInSubspace(inputCopy, output, outputSelectedPivotColumns);
    return;
  }
  if (input.size == 0) {
    output = input;
    return;
  }
  MacroRegisterFunctionWithName("Vectors::selectBasisInSubspace");
  ProgressReport reportTask(1, GlobalVariables::Response::ReportType::gaussianElimination);
  ProgressReport reportProgress(200, GlobalVariables::Response::ReportType::gaussianElimination);
  int dimension = input[0].size;
  if (reportTask.tickAndWantReport()) {
    std::stringstream reportStream;
    reportStream << "Selecting a basis of a vector space with " << input.size
    << " generators in dimension " << dimension << "... " ;
    reportTask.report(reportStream.str());
  }
  Matrix<Coefficient> matrix;
  int maximumNumberOfRows = MathRoutines::minimum(input.size, dimension);
  matrix.initialize(maximumNumberOfRows, dimension);
  int currentRow = 0;
  for (int i = 0; i < input.size; i ++) {
    for (int j = 0; j < dimension; j ++) {
      matrix(currentRow, j) = input[i][j];
    }
    currentRow ++;
    if (currentRow == maximumNumberOfRows || i == input.size - 1) {
      matrix.gaussianEliminationByRows(0, 0, &outputSelectedPivotColumns);
      currentRow = outputSelectedPivotColumns.cardinalitySelection;
    }
    if (currentRow == maximumNumberOfRows) {
      break;
    }
  }
  output.setSize(outputSelectedPivotColumns.cardinalitySelection);
  for (int i = 0; i < output.size; i ++) {
    matrix.getVectorFromRow(i, output[i]);
  }
  if (reportProgress.tickAndWantReport()) {
    std::stringstream reportStream;
    reportStream << "Selecting a basis of a vector space with " << input.size
    << " generators in dimension " << dimension << "... done. " ;
    reportProgress.report(reportStream.str());
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::addTwoRows(
  int fromRowIndex, int toRowIndex, int startColumnIndex, const Coefficient& scalar
) {
  ProgressReport report(10, GlobalVariables::Response::ReportType::gaussianElimination);
  Coefficient coefficient;
  for (int i = startColumnIndex; i < this->numberOfColumns; i ++) {
    coefficient = this->elements[fromRowIndex][i];
    coefficient *= scalar;
    if (report.tickAndWantReport()) {
      std::stringstream out;
      out << "Processing row, element " << i + 1 << " out of " << this->numberOfColumns;
      report.report(out.str());
    }
    this->elements[toRowIndex][i] += coefficient;
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::gaussianEliminationByRows(
  Matrix<Coefficient>* carbonCopyMatrix,
  Selection* outputNonPivotColumns,
  Selection* outputPivotColumns,
  std::stringstream* humanReadableReport,
  FormatExpressions* format
) {
  MacroRegisterFunctionWithName("Matrix::gaussianEliminationByRows");
  if (this->numberOfRows == 0) {
    global.fatal << "Request to do Gaussian elimination on a matrix with "
    << " zero rows. " << global.fatal;
  }
  if (carbonCopyMatrix != 0) {
    if (carbonCopyMatrix->numberOfRows != this->numberOfRows) {
      global.fatal << "Request to do "
      << "Gaussian elimination with carbon copy, however the matrix has "
      << this->numberOfRows << " rows, while the carbon copy has "
      << carbonCopyMatrix->numberOfRows << " rows. " << global.fatal;
    }
  }
  ///////////////////
  int numberOfPivots = 0;
  int maximumMatrixRank = MathRoutines::minimum(this->numberOfRows, this->numberOfColumns);
  Coefficient tempElement;
  if (outputNonPivotColumns != nullptr) {
    outputNonPivotColumns->initialize(this->numberOfColumns);
  }
  if (outputPivotColumns != nullptr) {
    outputPivotColumns->initialize(this->numberOfColumns);
  }
  bool formatAsLinearSystem = format == nullptr ? false : format->flagFormatMatrixAsLinearSystem;
  bool useHtmlInReport = format == nullptr ? true : format->flagUseHTML;
  ProgressReport report(100, GlobalVariables::Response::ReportType::gaussianElimination);
  if (humanReadableReport != nullptr) {
    if (useHtmlInReport) {
      *humanReadableReport << "\n\n\n\n<table><tr><td style='border-bottom:3pt solid black;'>System status</td>"
      << "<td style='border-bottom:3pt solid black;mid-width:100px;'>action</td></tr>";
    } else {
      *humanReadableReport << "\n\n\\begin{longtable}{cc} System status&Action \\\\\\hline\n";
    }
  }
  // Initialization done! Time to do actual work:
  for (int i = 0; i < this->numberOfColumns; i ++) {
    if (numberOfPivots == maximumMatrixRank) {
      if (outputNonPivotColumns != nullptr) {
        for (int j = i; j < this->numberOfColumns; j ++) {
          outputNonPivotColumns->addSelectionAppendNewIndex(j);
        }
      }
      break;
    }
    int pivotIndex = this->findPivot(i, numberOfPivots);
    if (pivotIndex == - 1) {
      if (outputNonPivotColumns != nullptr) {
        outputNonPivotColumns->addSelectionAppendNewIndex(i);
      }
      continue;
    }
    if (humanReadableReport != nullptr) {
      if (useHtmlInReport) {
        *humanReadableReport << "<tr><td style='border-bottom:1pt solid black;'>";
        if (formatAsLinearSystem) {
          *humanReadableReport << HtmlRoutines::getMathNoDisplay(this->toStringSystemLatex(carbonCopyMatrix, format), - 1);
        } else {
          *humanReadableReport << HtmlRoutines::getMathNoDisplay(this->toStringLatex(format), - 1);
        }
        *humanReadableReport << "</td><td style='border-bottom:1pt solid black;'><div style='display:inline-block;min-width:100px'>Selected pivot column "
        << i + 1 << ". ";
        if (numberOfPivots != pivotIndex) {
          *humanReadableReport << "Swapping rows so the pivot row is number " << numberOfPivots << ". ";
        }
      } else {
        if (formatAsLinearSystem) {
          *humanReadableReport << "\\(" << this->toStringSystemLatex(carbonCopyMatrix, format) << "\\)";
        } else {
          *humanReadableReport << "\\(" << this->toStringLatex(format) << "\\)";
        }
        *humanReadableReport << "& Selected pivot column " << i + 1 << ". ";
        if (numberOfPivots != pivotIndex) {
          *humanReadableReport << "Swapping rows so the pivot row is number "
          << numberOfPivots << ". ";
        }
      }
    }
    if (outputPivotColumns != nullptr) {
      outputPivotColumns->addSelectionAppendNewIndex(i);
    }
    this->switchRows(numberOfPivots, pivotIndex);
    if (carbonCopyMatrix != 0) {
      carbonCopyMatrix->switchRows(numberOfPivots, pivotIndex);
    }
    tempElement = this->elements[numberOfPivots][i];
    tempElement.invert();
    this->rowTimesScalar(numberOfPivots, tempElement);
    if (carbonCopyMatrix != 0) {
      carbonCopyMatrix->rowTimesScalar(numberOfPivots, tempElement);
    }
    for (int j = 0; j < this->numberOfRows; j ++) {
      if (j != numberOfPivots) {
        if (!this->elements[j][i].isEqualToZero()) {
          tempElement = this->elements[j][i];
          tempElement.negate();
          if (report.tickAndWantReport()) {
            std::stringstream reportStream;
            reportStream << "Gaussian elimination ("
            << this->numberOfRows << "x" << this->numberOfColumns
            << "): column " << i + 1 << " out of " << this->numberOfColumns
            << ".\n<br>Pivot row: " << numberOfPivots + 1
            << ", eliminating row " << j + 1 << " out of " << this->numberOfRows;
            report.report(reportStream.str());
          }
          this->addTwoRows(numberOfPivots, j, i, tempElement);
          if (carbonCopyMatrix != 0) {
            carbonCopyMatrix->addTwoRows(numberOfPivots, j, 0, tempElement);
          }
        }
      }
    }
    if (humanReadableReport != nullptr) {
      if (useHtmlInReport) {
        *humanReadableReport << "Eliminated the non-zero entries in the pivot column</div></td></tr>";
      } else {
        *humanReadableReport << "Eliminated the non-zero entries in the pivot column. \\\\\\hline\n";
      }
    }
    numberOfPivots ++;
  }
  if (humanReadableReport != nullptr) {
    if (useHtmlInReport) {
      if (formatAsLinearSystem) {
        *humanReadableReport << "<tr><td>"
        << HtmlRoutines::getMathNoDisplay(this->toStringSystemLatex(carbonCopyMatrix, format), - 1)
        << "</td><td> Final result.</td></tr></table>\n\n\n\n";
      } else {
        *humanReadableReport << "<tr><td>"
        << HtmlRoutines::getMathNoDisplay(this->toStringLatex(format))
        << "</td><td> Final result.</td></tr></table>\n\n\n\n";
      }
    } else {
      if (formatAsLinearSystem) {
        *humanReadableReport << "\\("
        << this->toStringSystemLatex(carbonCopyMatrix, format)
        << "\\)& Final result.\\\\\n";
      } else {
        *humanReadableReport << "\\("
        << this->toStringLatex(format) << "\\)& Final result.\\\\\n";
      }
      *humanReadableReport << "\\end{longtable}";
    }
  }
}

#endif // math_general_implementation_H

