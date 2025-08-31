#ifndef header_math_general_implementation_ALREADY_INCLUDED
#define header_math_general_implementation_ALREADY_INCLUDED

#include "html_routines.h"
#include "math_general.h"
#include "progress_report.h"

template <class Coefficient>
bool Matrix<Coefficient>::changeBasis(
  const List<Vector<Coefficient> >& newBasis,
  Matrix<Coefficient>& output,
  std::stringstream* commentsOnFailure
) const {
  STACK_TRACE("Matrix::changeBasis");
  return
  Matrix<Coefficient>::changeBasis(*this, newBasis, output, commentsOnFailure);
}

template <class Coefficient>
bool Matrix<Coefficient>::changeBasis(
  const Matrix<Coefficient>& linearOperator,
  const List<Vector<Coefficient> >& newBasis,
  Matrix<Coefficient>& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("Matrix::changeBasis");
  if (&linearOperator == &output) {
    Matrix<Coefficient> inputCopy = linearOperator;
    return
    Matrix<Coefficient>::changeBasis(
      inputCopy, newBasis, output, commentsOnFailure
    );
  }
  Vectors<Coefficient> vectors = newBasis;
  Vectors<Coefficient> transformed;
  Vectors<Coefficient> outputCoordinates;
  linearOperator.actOnVectorsColumn(vectors, transformed);
  if (
    !transformed.coordinatesInBasis(
      newBasis, outputCoordinates, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "The matrix does not preserve the input vector space: it maps: "
      << newBasis.toStringCommaDelimited()
      << " to: "
      << transformed.toStringCommaDelimited();
    }
    return false;
  }
  output.assignVectorsToColumns(transformed);
  return true;
}

template <class Coefficient>
void Matrix<Coefficient>::computeDeterminantOverwriteMatrix(
  Coefficient& output, const Coefficient& ringOne, const Coefficient& ringZero
) {
  STACK_TRACE("Matrix::computeDeterminantOverwriteMatrix");
  bool doReport = this->numberOfColumns > 10 &&
  this->numberOfRows > 10 &&
  this->numberOfColumns * this->numberOfRows >= 400;
  ProgressReport report(1);
  ProgressReport report2(400);
  int pivotIndex = 0;
  output = ringOne;
  Coefficient scalar;
  if (this->numberOfColumns != this->numberOfRows) {
    global.fatal
    << "Error: determinant computation: "
    << "number of columns different from number of rows. "
    << global.fatal;
  }
  int dimension = this->numberOfColumns;
  for (int i = 0; i < dimension; i ++) {
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
    if (doReport && report.tickAndWantReport()) {
      std::stringstream reportStream;
      reportStream << "Pivot row " << i + 1 << " out of " << dimension << ": ";
      for (int column = 0; column < this->numberOfColumns; column ++) {
        reportStream << (*this)(i, column).toString();
        if (column != this->numberOfColumns - 1) {
          reportStream << ", ";
        }
      }
      report.report(reportStream.str());
    }
    for (int j = i + 1; j < dimension; j ++) {
      if (this->elements[j][i].isEqualToZero()) {
        continue;
      }
      scalar = this->elements[j][i];
      scalar.negate();
      this->addTwoRows(i, j, i, scalar);
      if (doReport) {
        if (report2.tickAndWantReport()) {
          std::stringstream reportStream;
          reportStream
          << "Computing large determinant: pivot "
          << i + 1
          << ", row "
          << j
          << " out of "
          << dimension
          << " times  "
          << dimension
          << " total.";
          report2.report(reportStream.str());
        }
      }
    }
  }
}

template <class Coefficient>
std::ostream& operator<<(
  std::ostream& output, const Matrix<Coefficient>& matrix
) {
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
      lastMatRowIndexToHide =
      matrix.numberOfRows - 1 - format.maximumMatrixDisplayedRows /
      2;
    }
    if (matrix.numberOfColumns > format.maximumMatrixLineLength) {
      firstMatColIndexToHide = format.maximumMatrixLineLength / 2;
      lastMatColIndexToHide =
      matrix.numberOfColumns - 1 - format.maximumMatrixLineLength /
      2;
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
void MathRoutines::raiseToPower(
  Coefficient& element, const IntegerType& power, const Coefficient& ringUnit
) {
  STACK_TRACE("MathRoutines::raiseToPower");
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
  ProgressReport reportTwo(32);
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
    reportStream
    << "Raising "
    << element.toString()
    << " to power: "
    << powerCopy;
    reportOne.report(reportStream.str());
  }
  element = ringUnit;
  while (powerCopy > 0) {
    if (powerCopy % 2 == 1) {
      if (reportTwo.tickAndWantReport()) {
        std::stringstream reportStream2;
        reportStream2 << "Remaining exponent: " << powerCopy << "<br>";
        reportStream2
        << "Multiplying "
        << element.toString()
        << " by "
        << squares.toString();
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

template <class Coefficient>
void Matrix<Coefficient>::gaussianEliminationEuclideanDomain(
  Matrix<Coefficient>* otherMatrix,
  const Coefficient& negativeOne,
  const Coefficient& ringUnit,
  bool(*comparisonGEQFunction)(
    const Coefficient& left, const Coefficient& right
  )
) {
  STACK_TRACE("Matrix::gaussianEliminationEuclideanDomain");
  ProgressReport report(1);
  if (otherMatrix == this) {
    global.fatal
    << "The carbon copy in the Gaussian elimination "
    << "coincides with the matrix which we are row-reducing "
    << "(most probably this is a wrong pointer typo). "
    << global.fatal;
  }
  Coefficient element;
  int column = 0;
  int row = 0;
  while (row < this->numberOfRows && column < this->numberOfColumns) {
    int foundPivotRow = - 1;
    for (int i = row; i < this->numberOfRows; i ++) {
      if (!this->elements[i][column].isEqualToZero()) {
        foundPivotRow = i;
        break;
      }
    }
    if (foundPivotRow != - 1) {
      this->switchRowsWithCarbonCopy(row, foundPivotRow, otherMatrix);
      if (this->elements[row][column].isNegative()) {
        this->rowTimesScalarWithCarbonCopy(row, negativeOne, otherMatrix);
      }
      int exploringRow = row + 1;
      while (exploringRow < this->numberOfRows) {
        if (report.tickAndWantReport()) {
          std::stringstream out;
          out
          << "Pivotting on row of index "
          << row + 1
          << " with exploring row of index "
          << exploringRow + 1
          << "; total rows: "
          << this->numberOfRows;
          report.report(out.str());
        }
        Coefficient & pivotElement = this->elements[row][column];
        Coefficient& otherElement = this->elements[exploringRow][column];
        if (otherElement.isNegative()) {
          this->rowTimesScalarWithCarbonCopy(
            exploringRow, negativeOne, otherMatrix
          );
        }
        bool isSmallerOrEqualTo = comparisonGEQFunction ==
        0 ?
        pivotElement <= otherElement :
        comparisonGEQFunction(otherElement, pivotElement);
        if (isSmallerOrEqualTo) {
          element = otherElement;
          element /= pivotElement;
          element.assignFloor();
          this->subtractRowsWithCarbonCopy(
            exploringRow, row, 0, element, otherMatrix
          );
        }
        if (this->elements[exploringRow][column].isEqualToZero()) {
          exploringRow ++;
        } else {
          this->switchRowsWithCarbonCopy(exploringRow, row, otherMatrix);
        }
      }
      Coefficient& pivotElement = this->elements[row][column];
      for (int i = 0; i < row; i ++) {
        element = this->elements[i][column];
        element /= pivotElement;
        element.assignFloor();
        this->subtractRowsWithCarbonCopy(i, row, 0, element, otherMatrix);
        if (this->elements[i][column].isNegative()) {
          this->addTwoRowsWithCarbonCopy(row, i, 0, ringUnit, otherMatrix);
        }
      }
      row ++;
    }
    column ++;
  }
}

template <class Coefficient>
void Vectors<Coefficient>::chooseABasis() {
  Vectors<Rational> output;
  Matrix<Coefficient> toBeEliminated;
  Selection selection;
  for (int i = 0; i < this->size; i ++) {
    output.addOnTop(this->objects[i]);
    if (output.getRankLinearSpan(&toBeEliminated, &selection) < output.size) {
      output.removeLastObject();
    }
  }
  this->operator=(output);
}

template <class Coefficient>
void Vectors<Coefficient>::beefUpWithEiToLinearlyIndependentBasis(
  int dimension
) {
  Selection selection;
  Matrix<Coefficient> buffer;
  if (this->size != 0 && dimension != this->getDimension()) {
    global.fatal << "Vector dimension is incorrect. " << global.fatal;
  }
  int currentRank = this->getRankLinearSpan(buffer, selection);
  if (currentRank == dimension) {
    return;
  }
  Vector<Coefficient> vector;
  for (int i = 0; i < dimension && currentRank < dimension; i ++) {
    vector.makeEi(dimension, i);
    this->addOnTop(vector);
    int candidateRank = this->getRankLinearSpan(buffer, selection);
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
bool Vectors<Coefficient>::linearSpanContainsVector(
  const Vector<Coefficient>& input
) const {
  Matrix<Coefficient> buffer;
  Selection bufferSelection;
  return this->linearSpanContainsVector(input, buffer, bufferSelection);
}

template <class Coefficient>
void Vectors<Coefficient>::selectBasisInSubspace(
  const List<Vector<Coefficient> >& input,
  List<Vector<Coefficient> >& output,
  Selection& outputSelectedPivotColumns
) {
  if (&input == &output) {
    List<Vector<Coefficient> > inputCopy = input;
    Vectors<Coefficient>::selectBasisInSubspace(
      inputCopy, output, outputSelectedPivotColumns
    );
    return;
  }
  if (input.size == 0) {
    output = input;
    return;
  }
  STACK_TRACE("Vectors::selectBasisInSubspace");
  ProgressReport reportTask(1);
  ProgressReport reportProgress(200);
  int dimension = input[0].size;
  if (reportTask.tickAndWantReport()) {
    std::stringstream reportStream;
    reportStream
    << "Selecting a basis of a vector space with "
    << input.size
    << " generators in dimension "
    << dimension
    << "... ";
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
    reportStream
    << "Selecting a basis of a vector space with "
    << input.size
    << " generators in dimension "
    << dimension
    << "... done. ";
    reportProgress.report(reportStream.str());
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::addTwoRows(
  int fromRowIndex,
  int toRowIndex,
  int startColumnIndex,
  const Coefficient& scalar
) {
  ProgressReport report(10);
  Coefficient coefficient;
  for (int i = startColumnIndex; i < this->numberOfColumns; i ++) {
    coefficient = this->elements[fromRowIndex][i];
    coefficient *= scalar;
    if (report.tickAndWantReport()) {
      std::stringstream out;
      out
      << "Processing row, element "
      << i + 1
      << " out of "
      << this->numberOfColumns;
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
  STACK_TRACE("Matrix::gaussianEliminationByRows");
  if (this->numberOfRows == 0) {
    global.fatal
    << "Request to do Gaussian elimination on a matrix with "
    << "zero rows. "
    << global.fatal;
  }
  if (carbonCopyMatrix != 0) {
    if (carbonCopyMatrix->numberOfRows != this->numberOfRows) {
      global.fatal
      << "Request to do "
      << "Gaussian elimination with carbon copy, however the matrix has "
      << this->numberOfRows
      << " rows, while the carbon copy has "
      << carbonCopyMatrix->numberOfRows
      << " rows. "
      << global.fatal;
    }
  }
  // /////////////////
  int numberOfPivots = 0;
  int maximumMatrixRank =
  MathRoutines::minimum(this->numberOfRows, this->numberOfColumns);
  if (outputNonPivotColumns != nullptr) {
    outputNonPivotColumns->initialize(this->numberOfColumns);
  }
  if (outputPivotColumns != nullptr) {
    outputPivotColumns->initialize(this->numberOfColumns);
  }
  bool formatAsLinearSystem = format ==
  nullptr ? false : format->flagFormatMatrixAsLinearSystem;
  bool useHtmlInReport = format == nullptr ? true : format->flagUseHTML;
  ProgressReport report(100);
  if (humanReadableReport != nullptr) {
    if (useHtmlInReport) {
      *humanReadableReport
      << "\n\n\n\n<table><tr>"
      << "<td style='border-bottom:3pt solid black;'>"
      << "System status</td>"
      << "<td style='border-bottom:3pt solid black;mid-width:100px;'>"
      << "action</td></tr>";
    } else {
      *humanReadableReport
      << "\n\n\\begin{longtable}{cc} "
      << "System status&Action \\\\\\hline\n";
    }
  }
  // Initialization done! Time to do actual work:
  Coefficient coefficient;
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
        *humanReadableReport
        << "<tr><td style='border-bottom:1pt solid black;'>";
        if (formatAsLinearSystem) {
          *humanReadableReport
          << HtmlRoutines::getMathNoDisplay(
            this->toStringSystemLatex(carbonCopyMatrix, format)
          );
        } else {
          *humanReadableReport
          << HtmlRoutines::getMathNoDisplay(this->toStringLatex(format));
        }
        *humanReadableReport
        << "</td><td style='border-bottom:1pt solid black;'>"
        << "<div style='display:inline-block;min-width:100px'>"
        << "Selected pivot column "
        << i + 1
        << ". ";
        if (numberOfPivots != pivotIndex) {
          *humanReadableReport
          << "Swapping rows so the pivot row is number "
          << numberOfPivots
          << ". ";
        }
      } else {
        if (formatAsLinearSystem) {
          *humanReadableReport
          << "\\("
          << this->toStringSystemLatex(carbonCopyMatrix, format)
          << "\\)";
        } else {
          *humanReadableReport
          << "\\("
          << this->toStringLatex(format)
          << "\\)";
        }
        *humanReadableReport << "& Selected pivot column " << i + 1 << ". ";
        if (numberOfPivots != pivotIndex) {
          *humanReadableReport
          << "Swapping rows so the pivot row is number "
          << numberOfPivots
          << ". ";
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
    coefficient = this->elements[numberOfPivots][i];
    coefficient.invert();
    this->rowTimesScalar(numberOfPivots, coefficient);
    if (carbonCopyMatrix != 0) {
      carbonCopyMatrix->rowTimesScalar(numberOfPivots, coefficient);
    }
    for (int j = 0; j < this->numberOfRows; j ++) {
      if (j != numberOfPivots) {
        if (!this->elements[j][i].isEqualToZero()) {
          coefficient = this->elements[j][i];
          coefficient.negate();
          if (report.tickAndWantReport()) {
            std::stringstream reportStream;
            reportStream
            << "Gaussian elimination ("
            << this->numberOfRows
            << "x"
            << this->numberOfColumns
            << "): column "
            << i + 1
            << " out of "
            << this->numberOfColumns
            << ".\n<br>Pivot row: "
            << numberOfPivots + 1
            << ", eliminating row "
            << j + 1
            << " out of "
            << this->numberOfRows;
            report.report(reportStream.str());
          }
          this->addTwoRows(numberOfPivots, j, i, coefficient);
          if (carbonCopyMatrix != 0) {
            carbonCopyMatrix->addTwoRows(numberOfPivots, j, 0, coefficient);
          }
        }
      }
    }
    if (humanReadableReport != nullptr) {
      if (useHtmlInReport) {
        *humanReadableReport
        << "Eliminated the non-zero entries "
        << "in the pivot column</div></td></tr>";
      } else {
        *humanReadableReport
        << "Eliminated the non-zero entries "
        << "in the pivot column. \\\\\\hline\n";
      }
    }
    numberOfPivots ++;
  }
  if (humanReadableReport != nullptr) {
    if (useHtmlInReport) {
      if (formatAsLinearSystem) {
        *humanReadableReport
        << "<tr><td>"
        << HtmlRoutines::getMathNoDisplay(
          this->toStringSystemLatex(carbonCopyMatrix, format)
        )
        << "</td><td> Final result.</td></tr></table>\n\n\n\n";
      } else {
        *humanReadableReport
        << "<tr><td>"
        << HtmlRoutines::getMathNoDisplay(this->toStringLatex(format))
        << "</td><td> Final result.</td></tr></table>\n\n\n\n";
      }
    } else {
      if (formatAsLinearSystem) {
        *humanReadableReport
        << "\\("
        << this->toStringSystemLatex(carbonCopyMatrix, format)
        << "\\)& Final result.\\\\\n";
      } else {
        *humanReadableReport
        << "\\("
        << this->toStringLatex(format)
        << "\\)& Final result.\\\\\n";
      }
      *humanReadableReport << "\\end{longtable}";
    }
  }
}

#endif // header_math_general_implementation_ALREADY_INCLUDED
