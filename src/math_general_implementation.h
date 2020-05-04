#ifndef math_general_implementation_H
#define math_general_implementation_H
#include "math_general.h"

template <class Coefficient>
void Matrix<Coefficient>::computeDeterminantOverwriteMatrix(
  Coefficient& output, const Coefficient& theRingOne, const Coefficient& theRingZero
) {
  MacroRegisterFunctionWithName("Matrix::computeDeterminantOverwriteMatrix");
  bool doReport = this->numberOfColumns > 10 && this->numberOfRows > 10 && this->numberOfColumns * this->numberOfRows >= 400;
  ProgressReport theReport(1, GlobalVariables::Response::ReportType::gaussianElimination);
  ProgressReport theReport2(400, GlobalVariables::Response::ReportType::gaussianElimination);
  int tempI;
  output = theRingOne;
  Coefficient tempRat;
  if (this->numberOfColumns != this->numberOfRows) {
    global.fatal << "Error: determinant computation: number of columns different from number of rows. " << global.fatal;
  }
  int dim = this->numberOfColumns;
  for (int i = 0; i < dim; i ++) {
    tempI = this->findPivot(i, i);
    if (tempI == - 1) {
      output = theRingZero;
      return;
    }
    this->switchRows(i, tempI);
    if (tempI != i) {
      output *= - 1;
    }
    tempRat = this->elements[i][i];
    output *= tempRat;
    tempRat.invert();
    this->rowTimesScalar(i, tempRat);
    if (doReport) {
      if (theReport.tickAndWantReport()) {
        std::stringstream reportStream;
        reportStream << "Pivot row " << i + 1 << " out of " << dim << ": ";
        for (int colCounter = 0; colCounter < this->numberOfColumns; colCounter ++) {
          reportStream << (*this)(i, colCounter).toString();
          if (colCounter != this->numberOfColumns - 1) {
            reportStream << ", ";
          }
        }
        theReport.report(reportStream.str());
      }
    }
    for (int j = i + 1; j < dim; j ++) {
      if (!this->elements[j][i].isEqualToZero()) {
        tempRat = this->elements[j][i];
        tempRat.minus();
        this->addTwoRows (i, j, i, tempRat);
        if (doReport) {
          if (theReport2.tickAndWantReport()) {
            std::stringstream reportStream;
            reportStream << "Computing large determinant: pivot " << i + 1 << ", row " << j << " out of "
            << dim <<  " times  " << dim << " total.";
            theReport2.report(reportStream.str());
          }
        }
      }
    }
  }
}

template<class Coefficient>
std::ostream& operator<< (std::ostream& output, const Matrix<Coefficient>& theMat) {
  output << "\\left(\\begin{array}{";
  for (int j = 0; j < theMat.numberOfColumns; j ++) {
    output << "c";
  }
  output << "}";
  int firstMatRowIndexToHide = theMat.numberOfRows;
  int lastMatRowIndexToHide = theMat.numberOfRows;
  int firstMatColIndexToHide = theMat.numberOfColumns;
  int lastMatColIndexToHide = theMat.numberOfColumns;
  FormatExpressions& theFormat = global.theDefaultFormat.getElement();
  if (theFormat.flagSuppressLongMatrices) {
    if (theMat.numberOfRows > theFormat.MaxMatrixDisplayedRows) {
      firstMatRowIndexToHide = theFormat.MaxMatrixDisplayedRows / 2;
      lastMatRowIndexToHide = theMat.numberOfRows - 1 - theFormat.MaxMatrixDisplayedRows / 2;
    }
    if (theMat.numberOfColumns > theFormat.MaxMatrixLineLength) {
      firstMatColIndexToHide = theFormat.MaxMatrixLineLength / 2;
      lastMatColIndexToHide = theMat.numberOfColumns - 1 - theFormat.MaxMatrixLineLength / 2;
    }
  }
  for (int i = 0; i < theMat.numberOfRows; i ++) {
    if (firstMatRowIndexToHide <= i && i <= lastMatRowIndexToHide) {
      if (firstMatRowIndexToHide == i) {
        output << "...\\\\\n";
      }
      continue;
    }
    for (int j = 0; j < theMat.numberOfColumns; j ++) {
      if (firstMatColIndexToHide <= j && j <= lastMatColIndexToHide) {
        if (firstMatColIndexToHide == j) {
          output << "...&";
        }
        continue;
      }
      output << theMat(i, j);
      if (j != theMat.numberOfColumns - 1) {
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

template <class Coefficient, typename theIntegerType>
void MathRoutines::raiseToPower(
  Coefficient& theElement, const theIntegerType& thePower, const Coefficient& theRingUnit
) {
  MacroRegisterFunctionWithName("MathRoutines::raiseToPower");
  theIntegerType thePowerCopy;
  thePowerCopy = thePower;
  if (thePowerCopy < 0) {
    return;
  }
  if (thePowerCopy == 1) {
    return;
  }
  if (thePowerCopy == 0) {
    theElement = theRingUnit;
    return;
  }
  ProgressReport reportOne;
  ProgressReport reportTwo(32, GlobalVariables::Response::ReportType::general);
  Coefficient squares;
  squares = theElement;
  if (thePowerCopy < 4) {
    for (theIntegerType i = 1; i < thePowerCopy; i ++) {
      theElement *= squares;
    }
    return;
  }
  if (reportOne.tickAndWantReport()) {
    std::stringstream reportStream;
    reportStream << "Raising " << theElement.toString()
    << " to power: " << thePowerCopy;
    reportOne.report(reportStream.str());
  }
  theElement = theRingUnit;
  while (thePowerCopy > 0) {
    if (thePowerCopy % 2 == 1) {
      if (reportTwo.tickAndWantReport()) {
        std::stringstream reportStream2;
        reportStream2 << "Remaining exponent: " << thePowerCopy << "<br>";
        reportStream2 << "Multiplying " << theElement.toString()
        << " by " << squares.toString();
        reportTwo.report(reportStream2.str());
      }
      theElement *= squares;
    }
    thePowerCopy /= 2;
    if (thePowerCopy > 0) {
      if (reportTwo.tickAndWantReport()) {
        std::stringstream reportStream2;
        reportStream2 << "Remaining exponent: " << thePowerCopy << "<br>";
        reportStream2 << "Squaring: " << squares.toString();
        reportTwo.report(reportStream2.str());
      }
      squares *= squares;
    }
  }
}

template <class templateMonomial, class Coefficient>
void ElementMonomialAlgebra<templateMonomial, Coefficient>::multiplyBy(
  const ElementMonomialAlgebra<templateMonomial, Coefficient>& other,
  ElementMonomialAlgebra<templateMonomial, Coefficient>& output,
  ElementMonomialAlgebra<templateMonomial, Coefficient>& bufferPoly,
  templateMonomial& bufferMon
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
  ProgressReport theReport1;
  ProgressReport theReport2(400, GlobalVariables::Response::ReportType::monomialAlgebraProduct);
  if (theReport1.tickAndWantReport()) {
    totalMonPairs = other.size() * this->size();
    std::stringstream reportStream;
    reportStream << "Large polynomial computation: " << this->size() << " x "
    << other.size() << "=" << totalMonPairs << " monomials:\n<br>\n"
    << this->toString() << " times " << other.toString();
    theReport1.report(reportStream.str());
  }
  bufferPoly.makeZero();
  bufferPoly.setExpectedSize(maximumMonomials);
  bufferPoly.checkConsistency();
  bufferMon.checkConsistency();
  Coefficient current;
  for (int i = 0; i < other.size(); i ++) {
    for (int j = 0; j < this->size(); j ++) {
      if (theReport2.tickAndWantReport()) {
        std::stringstream reportStream2;
        reportStream2 << "Multiplying monomials: "
        << i + 1 << " out of " << other.size() << " by " << j + 1
        << " out of " << this->size() << ". ";
        theReport2.report(reportStream2.str());
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

template <class templateMonomial, class Coefficient>
void ElementMonomialAlgebra<templateMonomial, Coefficient>::multiplyBy(
  const templateMonomial& other,
  ElementMonomialAlgebra<templateMonomial, Coefficient>& output
) const {
  if (this == &output) {
    ElementMonomialAlgebra<templateMonomial, Coefficient> thisCopy = *this;
    thisCopy.multiplyBy(other, output);
    return;
  }
  output.makeZero();
  templateMonomial currentMonomial;
  for (int i = 0; i < this->size(); i ++) {
    currentMonomial = this->theMonomials[i];
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
  id.MakeIdSpecial();
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
  ProgressReport theReport1;
  ProgressReport theReport2(400, GlobalVariables::Response::ReportType::monomialAlgebraProduct);
  if (theReport1.tickAndWantReport()) {
    totalMonPairs = other.size() * this->size();
    std::stringstream reportStream;
    reportStream << "Large matrix monomial computation: " << this->size() << " x "
    << other.size() << "=" << totalMonPairs << " monomials:\n<br>\n"
    << this->toString() << " times " << other.toString();
    theReport1.report(reportStream.str());
  }
  MatrixTensor<Coefficient> result;
  result.setExpectedSize(maxNumMonsFinal);
  Coefficient currentCoefficient;
  MonomialMatrix currentMonomial;
  for (int i = 0; i < other.size(); i ++) {
    for (int j = 0; j < this->size(); j ++) {
      if (theReport2.tickAndWantReport()) {
        std::stringstream reportStream2;
        reportStream2 << "Multiplying monomials: "
        << i + 1 << " out of " << other.size() << " by " << j + 1
        << " out of " << this->size() << ". ";
        theReport2.report(reportStream2.str());
      }
      currentMonomial = (*this)[j];
      currentMonomial *= other[i];
      if (currentMonomial.IsZeroMonomial()) {
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
  const Coefficient& theRingMinusUnit,
  const Coefficient& theRingUnit,
  bool (*comparisonGEQFunction) (const Coefficient& left, const Coefficient& right)
) {
  MacroRegisterFunctionWithName("Matrix_Element::gaussianEliminationEuclideanDomain");
  ProgressReport theReport(1, GlobalVariables::Response::ReportType::gaussianElimination);
  if (otherMatrix == this) {
    global.fatal << "This is a programming error: the Carbon copy in the Gaussian elimination "
    << "coincides with the matrix which we are row-reducing "
    << "(most probably this is a wrong pointer typo). " << global.fatal;
  }
  int col = 0;
  Coefficient tempElt;
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
        this->rowTimesScalarWithCarbonCopy(row, theRingMinusUnit, otherMatrix);
      }
      int ExploringRow = row + 1;
      while (ExploringRow < this->numberOfRows) {
        if (theReport.tickAndWantReport()) {
          std::stringstream out;
          out << "Pivotting on row of index " << row + 1 << " with exploring row of index "
          << ExploringRow + 1 << "; total rows: " << this->numberOfRows;
          theReport.report(out.str());
        }
        Coefficient& PivotElt = this->elements[row][col];
        Coefficient& otherElt = this->elements[ExploringRow][col];
        if (otherElt.isNegative()) {
          this->rowTimesScalarWithCarbonCopy(ExploringRow, theRingMinusUnit, otherMatrix);
        }
        bool isSmallerOrEqualTo = comparisonGEQFunction == 0 ? PivotElt <= otherElt :
        comparisonGEQFunction(otherElt, PivotElt);
        if (isSmallerOrEqualTo) {
          tempElt = otherElt;
          tempElt /= PivotElt;
          tempElt.assignFloor();
          this->subtractRowsWithCarbonCopy(ExploringRow, row, 0, tempElt, otherMatrix);
        }
        if (this->elements[ExploringRow][col].isEqualToZero()) {
          ExploringRow ++;
        } else {
          this->switchRowsWithCarbonCopy(ExploringRow, row, otherMatrix);
        }
      }
      Coefficient& PivotElt = this->elements[row][col];
      for (int i = 0; i < row; i ++) {
        tempElt = this->elements[i][col];
        tempElt /= PivotElt;
        tempElt.assignFloor();
        this->subtractRowsWithCarbonCopy(i, row, 0, tempElt, otherMatrix);
        if (this->elements[i][col].isNegative()) {
          this->addTwoRowsWithCarbonCopy(row, i, 0, theRingUnit, otherMatrix);
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
  Matrix<Coefficient> tempMat;
  Selection tempSel;
  for (int i = 0; i < this->size; i ++) {
    output.addOnTop(this->theObjects[i]);
    if (output.getRankOfSpanOfElements(&tempMat, &tempSel) < output.size) {
      output.removeLastObject();
    }
  }
  this->operator=(output);
}

template <class Coefficient>
void Vectors<Coefficient>::beefUpWithEiToLinearlyIndependentBasis(int theDim) {
  Selection BufferSel;
  Matrix<Coefficient> Buffer;
  if (this->size != 0 && theDim != this->getDimension()) {
    global.fatal << "Vector dimension is incorrect. " << global.fatal;
  }
  int currentRank = this->getRankOfSpanOfElements(Buffer, BufferSel);
  if (currentRank == theDim) {
    return;
  }
  Vector<Coefficient> theVect;
  for (int i = 0; i < theDim && currentRank < theDim; i ++) {
    theVect.makeEi(theDim, i);
    this->addOnTop(theVect);
    int candidateRank = this->getRankOfSpanOfElements(Buffer, BufferSel);
    if (candidateRank > currentRank) {
      currentRank = candidateRank;
    } else {
      this->size --;
    }
  }
  if (currentRank != theDim) {
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
void Vectors<Coefficient>::SelectABasisInSubspace(
  const List<Vector<Coefficient> >& input, List<Vector<Coefficient> >& output, Selection& outputSelectedPivotColumns
) {
  if (&input == &output) {
    List<Vector<Coefficient> > inputCopy = input;
    Vectors<Coefficient>::SelectABasisInSubspace(inputCopy, output, outputSelectedPivotColumns);
    return;
  }
  if (input.size == 0) {
    output = input;
    return;
  }
  MacroRegisterFunctionWithName("Vectors::SelectABasisInSubspace");
  ProgressReport reportTask(1, GlobalVariables::Response::ReportType::gaussianElimination);
  ProgressReport reportProgress(200, GlobalVariables::Response::ReportType::gaussianElimination);
  int theDim = input[0].size;
  if (reportTask.tickAndWantReport()) {
    std::stringstream reportStream;
    reportStream << "Selecting a basis of a vector space with " << input.size
    << " generators in dimension " << theDim << "... " ;
    reportTask.report(reportStream.str());
  }
  Matrix<Coefficient> theMat;
  int MaxNumRows = MathRoutines::minimum(input.size, theDim);
  theMat.initialize(MaxNumRows, theDim);
  int currentRow = 0;
  for (int i = 0; i < input.size; i ++) {
    for (int j = 0; j < theDim; j ++) {
      theMat(currentRow, j) = input[i][j];
    }
    currentRow ++;
    if (currentRow == MaxNumRows || i == input.size - 1) {
      theMat.gaussianEliminationByRows(0, 0, &outputSelectedPivotColumns);
      currentRow = outputSelectedPivotColumns.cardinalitySelection;
    }
    if (currentRow == MaxNumRows) {
      break;
    }
  }
  output.setSize(outputSelectedPivotColumns.cardinalitySelection);
  for (int i = 0; i < output.size; i ++) {
    theMat.getVectorFromRow(i, output[i]);
  }
  if (reportProgress.tickAndWantReport()) {
    std::stringstream reportStream;
    reportStream << "Selecting a basis of a vector space with " << input.size
    << " generators in dimension " << theDim << "... done. " ;
    reportProgress.report(reportStream.str());
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::addTwoRows(int fromRowIndex, int ToRowIndex, int StartColIndex, const Coefficient& scalar) {
  ProgressReport theReport (10, GlobalVariables::Response::ReportType::gaussianElimination);
  Coefficient tempElement;
  for (int i = StartColIndex; i < this->numberOfColumns; i ++) {
    tempElement = this->elements[fromRowIndex][i];
    tempElement *= scalar;
    if (theReport.tickAndWantReport()) {
      std::stringstream out;
      out << "Processing row, element " << i + 1 << " out of " << this->numberOfColumns;
      theReport.report(out.str());
    }
    this->elements[ToRowIndex][i] += tempElement;
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::gaussianEliminationByRows(
  Matrix<Coefficient>* carbonCopyMat,
  Selection* outputNonPivotColumns,
  Selection* outputPivotColumns,
  std::stringstream* humanReadableReport,
  FormatExpressions* theFormat
) {
  MacroRegisterFunctionWithName("Matrix::gaussianEliminationByRows");
  if (this->numberOfRows == 0) {
    global.fatal << "This is a programming error: requesting to do Gaussian elimination on a matrix with "
    << " zero rows. " << global.fatal;
  }
  if (carbonCopyMat != 0) {
    if (carbonCopyMat->numberOfRows != this->numberOfRows) {
      global.fatal << "This is a programming error: requesting to do "
      << "Gaussian elimination with carbon copy, however the matrix has "
      << this->numberOfRows << " rows, while the carbon copy has "
      << carbonCopyMat->numberOfRows << " rows. " << global.fatal;
    }
  }
  ///////////////////
  int tempI;
  int NumFoundPivots = 0;
  int MaxRankMat = MathRoutines::minimum(this->numberOfRows, this->numberOfColumns);
  Coefficient tempElement;
  if (outputNonPivotColumns != nullptr) {
    outputNonPivotColumns->initialize(this->numberOfColumns);
  }
  if (outputPivotColumns != nullptr) {
    outputPivotColumns->initialize(this->numberOfColumns);
  }
  bool formatAsLinearSystem = theFormat == nullptr ? false : theFormat->flagFormatMatrixAsLinearSystem;
  bool useHtmlInReport = theFormat == nullptr ? true : theFormat->flagUseHTML;
  ProgressReport theReport(100, GlobalVariables::Response::ReportType::gaussianElimination);
  if (humanReadableReport != nullptr) {
    if (useHtmlInReport) {
      *humanReadableReport << "\n\n\n\n<table><tr><td style =\"border-bottom:3pt solid black;\">System status</td>"
      << "<td style =\"border-bottom:3pt solid black;\">action</td></tr>";
    } else {
      *humanReadableReport << "\n\n\\begin{longtable}{cc} System status&Action \\\\\\hline\n";
    }
  }
  //Initialization done! Time to do actual work:
  for (int i = 0; i < this->numberOfColumns; i ++) {
    if (NumFoundPivots == MaxRankMat) {
      if (outputNonPivotColumns != nullptr) {
        for (int j = i; j < this->numberOfColumns; j ++) {
          outputNonPivotColumns->addSelectionAppendNewIndex(j);
        }
      }
      break;
    }
    tempI = this->findPivot(i, NumFoundPivots);
    if (tempI == - 1) {
      if (outputNonPivotColumns != nullptr) {
        outputNonPivotColumns->addSelectionAppendNewIndex(i);
      }
      continue;
    }
    if (humanReadableReport != nullptr) {
      if (useHtmlInReport) {
        *humanReadableReport << "<tr><td style =\"border-bottom:1pt solid black;\">";
        if (formatAsLinearSystem) {
          *humanReadableReport << HtmlRoutines::getMathSpanPure(this->ToStringSystemLatex(carbonCopyMat, theFormat), - 1);
        } else {
          *humanReadableReport << HtmlRoutines::getMathSpanPure(this->ToStringLatex(theFormat), - 1);
        }
        *humanReadableReport << "</td><td style =\"border-bottom:1pt solid black;\">Selected pivot column "
        << i + 1 << ". ";
        if (NumFoundPivots != tempI) {
          *humanReadableReport << "Swapping rows so the pivot row is number " << NumFoundPivots << ". ";
        }
      } else {
        if (formatAsLinearSystem) {
          *humanReadableReport << "$" << this->ToStringSystemLatex(carbonCopyMat, theFormat) << "$";
        } else {
          *humanReadableReport << "$" << this->ToStringLatex(theFormat) << "$";
        }
        *humanReadableReport << "& Selected pivot column " << i + 1 << ". ";
        if (NumFoundPivots != tempI) {
          *humanReadableReport << "Swapping rows so the pivot row is number " << NumFoundPivots << ". ";
        }
      }
    }
    if (outputPivotColumns != nullptr) {
      outputPivotColumns->addSelectionAppendNewIndex(i);
    }
    this->switchRows(NumFoundPivots, tempI);
    if (carbonCopyMat != 0) {
      carbonCopyMat->switchRows(NumFoundPivots, tempI);
    }
    tempElement = this->elements[NumFoundPivots][i];
    tempElement.invert();
    this->rowTimesScalar(NumFoundPivots, tempElement);
    if (carbonCopyMat != 0) {
      carbonCopyMat->rowTimesScalar(NumFoundPivots, tempElement);
    }
    for (int j = 0; j < this->numberOfRows; j ++) {
      if (j != NumFoundPivots) {
        if (!this->elements[j][i].isEqualToZero()) {
          tempElement = this->elements[j][i];
          tempElement.minus();
          if (theReport.tickAndWantReport()) {
            std::stringstream reportStream;
            reportStream << "Gaussian elimination (" << this->numberOfRows << "x" << this->numberOfColumns
            << "): column " << i + 1 << " out of " << this->numberOfColumns
            << ".\n<br>Pivot row: " << NumFoundPivots + 1 << ", eliminating row " << j + 1 << " out of " << this->numberOfRows;
            theReport.report(reportStream.str());
          }
          this->addTwoRows(NumFoundPivots, j, i, tempElement);
          if (carbonCopyMat != 0) {
            carbonCopyMat->addTwoRows(NumFoundPivots, j, 0, tempElement);
          }
        }
      }
    }
    if (humanReadableReport != nullptr) {
      if (useHtmlInReport) {
        *humanReadableReport << "Eliminated the non-zero entries in the pivot column</td></tr>";
      } else {
        *humanReadableReport << "Eliminated the non-zero entries in the pivot column. \\\\\\hline\n";
      }
    }
    NumFoundPivots ++;
  }
  if (humanReadableReport != nullptr) {
    if (useHtmlInReport) {
      if (formatAsLinearSystem) {
        *humanReadableReport << "<tr><td>" << HtmlRoutines::getMathSpanPure(this->ToStringSystemLatex(carbonCopyMat, theFormat), - 1)
        << "</td><td> Final result.</td></tr></table>\n\n\n\n";
      } else {
        *humanReadableReport << "<tr><td>" << HtmlRoutines::getMathSpanPure(this->ToStringLatex(theFormat))
        << "</td><td> Final result.</td></tr></table>\n\n\n\n";
      }
    } else {
      if (formatAsLinearSystem) {
        *humanReadableReport << "$" << this->ToStringSystemLatex(carbonCopyMat, theFormat)
        << "$& Final result.\\\\\n";
      } else {
        *humanReadableReport << "$" << this->ToStringLatex(theFormat) << "$& Final result.\\\\\n";
      }
      *humanReadableReport << "\\end{longtable}";
    }
  }
}

#endif // math_general_implementation_H

