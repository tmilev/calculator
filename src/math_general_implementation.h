#ifndef math_general_implementation_H
#define math_general_implementation_H
#include "math_general.h"

template <class coefficient>
void Matrix<coefficient>::ComputeDeterminantOverwriteMatrix(
  coefficient& output, const coefficient& theRingOne, const coefficient& theRingZero
) {
  MacroRegisterFunctionWithName("Matrix::ComputeDeterminantOverwriteMatrix");
  bool doReport = this->NumCols > 10 && this->NumRows > 10 && this->NumCols * this->NumRows >= 400;
  ProgressReport theReport(1, GlobalVariables::Response::ReportType::gaussianElimination);
  ProgressReport theReport2(400, GlobalVariables::Response::ReportType::gaussianElimination);
  int tempI;
  output = theRingOne;
  coefficient tempRat;
  if (this->NumCols != this->NumRows) {
    global.fatal << "Error: determinant computation: number of columns different from number of rows. " << global.fatal;
  }
  int dim = this->NumCols;
  for (int i = 0; i < dim; i ++) {
    tempI = this->FindPivot(i, i);
    if (tempI == - 1) {
      output = theRingZero;
      return;
    }
    this->SwitchTwoRows(i, tempI);
    if (tempI != i) {
      output *= - 1;
    }
    tempRat = this->elements[i][i];
    output *= tempRat;
    tempRat.invert();
    this->RowTimesScalar(i, tempRat);
    if (doReport) {
      if (theReport.TickAndWantReport()) {
        std::stringstream reportStream;
        reportStream << "Pivot row " << i + 1 << " out of " << dim << ": ";
        for (int colCounter = 0; colCounter < this->NumCols; colCounter ++) {
          reportStream << (*this)(i, colCounter).toString();
          if (colCounter != this->NumCols - 1) {
            reportStream << ", ";
          }
        }
        theReport.Report(reportStream.str());
      }
    }
    for (int j = i + 1; j < dim; j ++) {
      if (!this->elements[j][i].IsEqualToZero()) {
        tempRat = this->elements[j][i];
        tempRat.Minus();
        this->AddTwoRows (i, j, i, tempRat);
        if (doReport) {
          if (theReport2.TickAndWantReport()) {
            std::stringstream reportStream;
            reportStream << "Computing large determinant: pivot " << i + 1 << ", row " << j << " out of "
            << dim <<  " times  " << dim << " total.";
            theReport2.Report(reportStream.str());
          }
        }
      }
    }
  }
}

template<class coefficient>
std::ostream& operator<< (std::ostream& output, const Matrix<coefficient>& theMat) {
  output << "\\left(\\begin{array}{";
  for (int j = 0; j < theMat.NumCols; j ++) {
    output << "c";
  }
  output << "}";
  int firstMatRowIndexToHide = theMat.NumRows;
  int lastMatRowIndexToHide = theMat.NumRows;
  int firstMatColIndexToHide = theMat.NumCols;
  int lastMatColIndexToHide = theMat.NumCols;
  FormatExpressions& theFormat = global.theDefaultFormat.GetElement();
  if (theFormat.flagSuppressLongMatrices) {
    if (theMat.NumRows > theFormat.MaxMatrixDisplayedRows) {
      firstMatRowIndexToHide = theFormat.MaxMatrixDisplayedRows / 2;
      lastMatRowIndexToHide = theMat.NumRows - 1 - theFormat.MaxMatrixDisplayedRows / 2;
    }
    if (theMat.NumCols > theFormat.MaxMatrixLineLength) {
      firstMatColIndexToHide = theFormat.MaxMatrixLineLength / 2;
      lastMatColIndexToHide = theMat.NumCols - 1 - theFormat.MaxMatrixLineLength / 2;
    }
  }
  for (int i = 0; i < theMat.NumRows; i ++) {
    if (firstMatRowIndexToHide <= i && i <= lastMatRowIndexToHide) {
      if (firstMatRowIndexToHide == i) {
        output << "...\\\\\n";
      }
      continue;
    }
    for (int j = 0; j < theMat.NumCols; j ++) {
      if (firstMatColIndexToHide <= j && j <= lastMatColIndexToHide) {
        if (firstMatColIndexToHide == j) {
          output << "...&";
        }
        continue;
      }
      output << theMat(i, j);
      if (j != theMat.NumCols - 1) {
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

template <class coefficient, typename theIntegerType>
void MathRoutines::RaiseToPower(
  coefficient& theElement, const theIntegerType& thePower, const coefficient& theRingUnit
) {
  MacroRegisterFunctionWithName("MathRoutines::RaiseToPower");
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
  coefficient squares;
  squares = theElement;
  if (thePowerCopy < 4) {
    for (theIntegerType i = 1; i < thePowerCopy; i ++) {
      theElement *= squares;
    }
    return;
  }
  if (reportOne.TickAndWantReport()) {
    std::stringstream reportStream;
    reportStream << "Raising " << theElement.toString()
    << " to power: " << thePowerCopy;
    reportOne.Report(reportStream.str());
  }
  theElement = theRingUnit;
  while (thePowerCopy > 0) {
    if (thePowerCopy % 2 == 1) {
      if (reportTwo.TickAndWantReport()) {
        std::stringstream reportStream2;
        reportStream2 << "Remaining exponent: " << thePowerCopy << "<br>";
        reportStream2 << "Multiplying " << theElement.toString()
        << " by " << squares.toString();
        reportTwo.Report(reportStream2.str());
      }
      theElement *= squares;
    }
    thePowerCopy /= 2;
    if (thePowerCopy > 0) {
      if (reportTwo.TickAndWantReport()) {
        std::stringstream reportStream2;
        reportStream2 << "Remaining exponent: " << thePowerCopy << "<br>";
        reportStream2 << "Squaring: " << squares.toString();
        reportTwo.Report(reportStream2.str());
      }
      squares *= squares;
    }
  }
}

template <class templateMonomial, class coefficient>
void ElementMonomialAlgebra<templateMonomial, coefficient>::MultiplyBy(
  const ElementMonomialAlgebra<templateMonomial, coefficient>& other,
  ElementMonomialAlgebra<templateMonomial, coefficient>& output,
  ElementMonomialAlgebra<templateMonomial, coefficient>& bufferPoly,
  templateMonomial& bufferMon
) const {
  MacroRegisterFunctionWithName("ElementMonomialAlgebra::MultiplyBy");
  if (other.IsEqualToZero()) {
    output.makeZero();
    return;
  }
  if (&bufferPoly == this || &bufferPoly == &other) {
    global.fatal << "Bad buffer in ElementMonomialAlgebra::MultiplyBy." << global.fatal;
  }
  this->CheckConsistency();
  other.CheckConsistency();
  int maxNumMonsFinal = this->size() * other.size();
  if (maxNumMonsFinal > 2000000) {
    maxNumMonsFinal = 2000000;
  }
  int totalMonPairs = 0;
  ProgressReport theReport1;
  ProgressReport theReport2(400, GlobalVariables::Response::ReportType::monomialAlgebraProduct);
  if (theReport1.TickAndWantReport()) {
    totalMonPairs = other.size() * this->size();
    std::stringstream reportStream;
    reportStream << "Large polynomial computation: " << this->size() << " x "
    << other.size() << "=" << totalMonPairs << " monomials:\n<br>\n"
    << this->toString() << " times " << other.toString();
    theReport1.Report(reportStream.str());
  }
  bufferPoly.makeZero();
  bufferPoly.SetExpectedSize(maxNumMonsFinal);
  bufferPoly.CheckConsistency();
  bufferMon.CheckConsistency();
  coefficient theCoeff;
  for (int i = 0; i < other.size(); i ++) {
    for (int j = 0; j < this->size(); j ++) {
      if (theReport2.TickAndWantReport()) {
        std::stringstream reportStream2;
        reportStream2 << "Multiplying monomials: "
        << i + 1 << " out of " << other.size() << " by " << j + 1
        << " out of " << this->size() << ". ";
        theReport2.Report(reportStream2.str());
      }
      bufferMon = (*this)[j];
      bufferMon *= other[i];
      theCoeff = this->coefficients[j];
      theCoeff *= other.coefficients[i];
      bufferPoly.AddMonomial(bufferMon, theCoeff);
      ParallelComputing::SafePointDontCallMeFromDestructors();
    }
  }
  output = bufferPoly;
}

template <class templateMonomial, class coefficient>
void ElementMonomialAlgebra<templateMonomial, coefficient>::MultiplyBy(
  const templateMonomial& other,
  ElementMonomialAlgebra<templateMonomial, coefficient>& output
) const {
  if (this == &output) {
    ElementMonomialAlgebra<templateMonomial, coefficient> thisCopy = *this;
    thisCopy.MultiplyBy(other, output);
    return;
  }
  output.makeZero();
  templateMonomial currentMonomial;
  for (int i = 0; i < this->size(); i ++) {
    currentMonomial = this->theMonomials[i];
    currentMonomial *= other;
    output.AddMonomial(currentMonomial, this->coefficients[i]);
  }
}

template<class coefficient>
void MatrixTensor<coefficient>::RaiseToPower(int power) {
  if (power <= 0) {
    global.fatal << "MatrixTensor::RaiseToPower is currently implemented for positive integer power only. " << global.fatal;
  }
  MatrixTensor<coefficient> id;
  id.MakeIdSpecial();
  MathRoutines::RaiseToPower(*this, power, id);
}

template <class coefficient>
void MatrixTensor<coefficient>::operator*=(
  const MatrixTensor<coefficient>& other
) {
  MacroRegisterFunctionWithName("MatrixTensor::MultiplyBy");
  if (other.IsEqualToZero()) {
    this->makeZero();
    return;
  }
  this->CheckConsistency();
  other.CheckConsistency();
  int maxNumMonsFinal = this->size() * other.size();
  if (maxNumMonsFinal > 2000000) {
    maxNumMonsFinal = 2000000;
  }
  int totalMonPairs = 0;
  ProgressReport theReport1;
  ProgressReport theReport2(400, GlobalVariables::Response::ReportType::monomialAlgebraProduct);
  if (theReport1.TickAndWantReport()) {
    totalMonPairs = other.size() * this->size();
    std::stringstream reportStream;
    reportStream << "Large matrix monomial computation: " << this->size() << " x "
    << other.size() << "=" << totalMonPairs << " monomials:\n<br>\n"
    << this->toString() << " times " << other.toString();
    theReport1.Report(reportStream.str());
  }
  MatrixTensor<coefficient> result;
  result.SetExpectedSize(maxNumMonsFinal);
  coefficient currentCoefficient;
  MonomialMatrix currentMonomial;
  for (int i = 0; i < other.size(); i ++) {
    for (int j = 0; j < this->size(); j ++) {
      if (theReport2.TickAndWantReport()) {
        std::stringstream reportStream2;
        reportStream2 << "Multiplying monomials: "
        << i + 1 << " out of " << other.size() << " by " << j + 1
        << " out of " << this->size() << ". ";
        theReport2.Report(reportStream2.str());
      }
      currentMonomial = (*this)[j];
      currentMonomial *= other[i];
      if (currentMonomial.IsZeroMonomial()) {
        continue;
      }
      currentCoefficient = this->coefficients[j];
      currentCoefficient *= other.coefficients[i];
      result.AddMonomial(currentMonomial, currentCoefficient);
    }
  }
  *this = result;
}

template <class coefficient>
void Matrix<coefficient>::GaussianEliminationEuclideanDomain(
  Matrix<coefficient>* otherMatrix,
  const coefficient& theRingMinusUnit,
  const coefficient& theRingUnit,
  bool (*comparisonGEQFunction) (const coefficient& left, const coefficient& right)
) {
  MacroRegisterFunctionWithName("Matrix_Element::GaussianEliminationEuclideanDomain");
  ProgressReport theReport(1, GlobalVariables::Response::ReportType::gaussianElimination);
  if (otherMatrix == this) {
    global.fatal << "This is a programming error: the Carbon copy in the Gaussian elimination "
    << "coincides with the matrix which we are row-reducing "
    << "(most probably this is a wrong pointer typo). " << global.fatal;
  }
  int col = 0;
  coefficient tempElt;
  int row = 0;
  while (row < this->NumRows && col < this->NumCols) {
    int foundPivotRow = - 1;
    for (int i = row; i < this->NumRows; i ++) {
      if (!this->elements[i][col].IsEqualToZero()) {
        foundPivotRow = i;
        break;
      }
    }
    if (foundPivotRow != - 1) {
      this->SwitchTwoRowsWithCarbonCopy(row, foundPivotRow, otherMatrix);
      if (this->elements[row][col].IsNegative()) {
        this->RowTimesScalarWithCarbonCopy(row, theRingMinusUnit, otherMatrix);
      }
      int ExploringRow = row + 1;
      while (ExploringRow < this->NumRows) {
        if (theReport.TickAndWantReport()) {
          std::stringstream out;
          out << "Pivotting on row of index " << row + 1 << " with exploring row of index "
          << ExploringRow + 1 << "; total rows: " << this->NumRows;
          theReport.Report(out.str());
        }
        coefficient& PivotElt = this->elements[row][col];
        coefficient& otherElt = this->elements[ExploringRow][col];
        if (otherElt.IsNegative()) {
          this->RowTimesScalarWithCarbonCopy(ExploringRow, theRingMinusUnit, otherMatrix);
        }
        bool isSmallerOrEqualTo = comparisonGEQFunction == 0 ? PivotElt <= otherElt :
        comparisonGEQFunction(otherElt, PivotElt);
        if (isSmallerOrEqualTo) {
          tempElt = otherElt;
          tempElt /= PivotElt;
          tempElt.AssignFloor();
          this->SubtractRowsWithCarbonCopy(ExploringRow, row, 0, tempElt, otherMatrix);
        }
        if (this->elements[ExploringRow][col].IsEqualToZero()) {
          ExploringRow ++;
        } else {
          this->SwitchTwoRowsWithCarbonCopy(ExploringRow, row, otherMatrix);
        }
      }
      coefficient& PivotElt = this->elements[row][col];
      for (int i = 0; i < row; i ++) {
        tempElt = this->elements[i][col];
        tempElt /= PivotElt;
        tempElt.AssignFloor();
        this->SubtractRowsWithCarbonCopy(i, row, 0, tempElt, otherMatrix);
        if (this->elements[i][col].IsNegative()) {
          this->AddTwoRowsWithCarbonCopy(row, i, 0, theRingUnit, otherMatrix);
        }
      }
      row ++;
    }
    col ++;
  }
}

template <class coefficient>
void Vectors<coefficient>::ChooseABasis() {
  Vectors<Rational> output;
  Matrix<coefficient> tempMat;
  Selection tempSel;
  for (int i = 0; i < this->size; i ++) {
    output.addOnTop(this->theObjects[i]);
    if (output.GetRankOfSpanOfElements(&tempMat, &tempSel) < output.size) {
      output.RemoveLastObject();
    }
  }
  this->operator=(output);
}

template <class coefficient>
void Vectors<coefficient>::BeefUpWithEiToLinearlyIndependentBasis(int theDim) {
  Selection BufferSel;
  Matrix<coefficient> Buffer;
  if (this->size != 0 && theDim != this->GetDim()) {
    global.fatal << "Vector dimension is incorrect. " << global.fatal;
  }
  int currentRank = this->GetRankOfSpanOfElements(Buffer, BufferSel);
  if (currentRank == theDim) {
    return;
  }
  Vector<coefficient> theVect;
  for (int i = 0; i < theDim && currentRank < theDim; i ++) {
    theVect.MakeEi(theDim, i);
    this->addOnTop(theVect);
    int candidateRank = this->GetRankOfSpanOfElements(Buffer, BufferSel);
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

template <class coefficient>
bool Vectors<coefficient>::LinSpanContainsVector(const Vector<coefficient>& input) const {
    Matrix<coefficient> buffer;
    Selection bufferSelection;
    return this->LinSpanContainsVector(input, buffer, bufferSelection);
  }

template <class coefficient>
void Vectors<coefficient>::SelectABasisInSubspace(
  const List<Vector<coefficient> >& input, List<Vector<coefficient> >& output, Selection& outputSelectedPivotColumns
) {
  if (&input == &output) {
    List<Vector<coefficient> > inputCopy = input;
    Vectors<coefficient>::SelectABasisInSubspace(inputCopy, output, outputSelectedPivotColumns);
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
  if (reportTask.TickAndWantReport()) {
    std::stringstream reportStream;
    reportStream << "Selecting a basis of a vector space with " << input.size
    << " generators in dimension " << theDim << "... " ;
    reportTask.Report(reportStream.str());
  }
  Matrix<coefficient> theMat;
  int MaxNumRows = MathRoutines::Minimum(input.size, theDim);
  theMat.init(MaxNumRows, theDim);
  int currentRow = 0;
  for (int i = 0; i < input.size; i ++) {
    for (int j = 0; j < theDim; j ++) {
      theMat(currentRow, j) = input[i][j];
    }
    currentRow ++;
    if (currentRow == MaxNumRows || i == input.size - 1) {
      theMat.GaussianEliminationByRows(0, 0, &outputSelectedPivotColumns);
      currentRow = outputSelectedPivotColumns.CardinalitySelection;
    }
    if (currentRow == MaxNumRows) {
      break;
    }
  }
  output.SetSize(outputSelectedPivotColumns.CardinalitySelection);
  for (int i = 0; i < output.size; i ++) {
    theMat.GetVectorFromRow(i, output[i]);
  }
  if (reportProgress.TickAndWantReport()) {
    std::stringstream reportStream;
    reportStream << "Selecting a basis of a vector space with " << input.size
    << " generators in dimension " << theDim << "... done. " ;
    reportProgress.Report(reportStream.str());
  }
}

template <typename coefficient>
void Matrix<coefficient>::AddTwoRows(int fromRowIndex, int ToRowIndex, int StartColIndex, const coefficient& scalar) {
  ProgressReport theReport (10, GlobalVariables::Response::ReportType::gaussianElimination);
  coefficient tempElement;
  for (int i = StartColIndex; i < this->NumCols; i ++) {
    tempElement = this->elements[fromRowIndex][i];
    tempElement *= scalar;
    if (theReport.TickAndWantReport()) {
      std::stringstream out;
      out << "Processing row, element " << i + 1 << " out of " << this->NumCols;
      theReport.Report(out.str());
    }
    this->elements[ToRowIndex][i] += tempElement;
  }
}

template <typename coefficient>
void Matrix<coefficient>::GaussianEliminationByRows(
  Matrix<coefficient>* carbonCopyMat,
  Selection* outputNonPivotColumns,
  Selection* outputPivotColumns,
  std::stringstream* humanReadableReport,
  FormatExpressions* theFormat
) {
  MacroRegisterFunctionWithName("Matrix::GaussianEliminationByRows");
  if (this->NumRows == 0) {
    global.fatal << "This is a programming error: requesting to do Gaussian elimination on a matrix with "
    << " zero rows. " << global.fatal;
  }
  if (carbonCopyMat != 0) {
    if (carbonCopyMat->NumRows != this->NumRows) {
      global.fatal << "This is a programming error: requesting to do "
      << "Gaussian elimination with carbon copy, however the matrix has "
      << this->NumRows << " rows, while the carbon copy has "
      << carbonCopyMat->NumRows << " rows. " << global.fatal;
    }
  }
  ///////////////////
  int tempI;
  int NumFoundPivots = 0;
  int MaxRankMat = MathRoutines::Minimum(this->NumRows, this->NumCols);
  coefficient tempElement;
  if (outputNonPivotColumns != nullptr) {
    outputNonPivotColumns->init(this->NumCols);
  }
  if (outputPivotColumns != nullptr) {
    outputPivotColumns->init(this->NumCols);
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
  for (int i = 0; i < this->NumCols; i ++) {
    if (NumFoundPivots == MaxRankMat) {
      if (outputNonPivotColumns != nullptr) {
        for (int j = i; j < this->NumCols; j ++) {
          outputNonPivotColumns->AddSelectionAppendNewIndex(j);
        }
      }
      break;
    }
    tempI = this->FindPivot(i, NumFoundPivots);
    if (tempI == - 1) {
      if (outputNonPivotColumns != nullptr) {
        outputNonPivotColumns->AddSelectionAppendNewIndex(i);
      }
      continue;
    }
    if (humanReadableReport != nullptr) {
      if (useHtmlInReport) {
        *humanReadableReport << "<tr><td style =\"border-bottom:1pt solid black;\">";
        if (formatAsLinearSystem) {
          *humanReadableReport << HtmlRoutines::GetMathSpanPure(this->ToStringSystemLatex(carbonCopyMat, theFormat), - 1);
        } else {
          *humanReadableReport << HtmlRoutines::GetMathSpanPure(this->ToStringLatex(theFormat), - 1);
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
      outputPivotColumns->AddSelectionAppendNewIndex(i);
    }
    this->SwitchTwoRows(NumFoundPivots, tempI);
    if (carbonCopyMat != 0) {
      carbonCopyMat->SwitchTwoRows(NumFoundPivots, tempI);
    }
    tempElement = this->elements[NumFoundPivots][i];
    tempElement.invert();
    this->RowTimesScalar(NumFoundPivots, tempElement);
    if (carbonCopyMat != 0) {
      carbonCopyMat->RowTimesScalar(NumFoundPivots, tempElement);
    }
    for (int j = 0; j < this->NumRows; j ++) {
      if (j != NumFoundPivots) {
        if (!this->elements[j][i].IsEqualToZero()) {
          tempElement = this->elements[j][i];
          tempElement.Minus();
          if (theReport.TickAndWantReport()) {
            std::stringstream reportStream;
            reportStream << "Gaussian elimination (" << this->NumRows << "x" << this->NumCols
            << "): column " << i + 1 << " out of " << this->NumCols
            << ".\n<br>Pivot row: " << NumFoundPivots + 1 << ", eliminating row " << j + 1 << " out of " << this->NumRows;
            theReport.Report(reportStream.str());
          }
          this->AddTwoRows(NumFoundPivots, j, i, tempElement);
          if (carbonCopyMat != 0) {
            carbonCopyMat->AddTwoRows(NumFoundPivots, j, 0, tempElement);
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
        *humanReadableReport << "<tr><td>" << HtmlRoutines::GetMathSpanPure(this->ToStringSystemLatex(carbonCopyMat, theFormat), - 1)
        << "</td><td> Final result.</td></tr></table>\n\n\n\n";
      } else {
        *humanReadableReport << "<tr><td>" << HtmlRoutines::GetMathSpanPure(this->ToStringLatex(theFormat))
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

