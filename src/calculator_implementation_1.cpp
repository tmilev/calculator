// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_interface.h"
#include "calculator_inner_typed_functions.h"
#include "math_general_implementation.h"
#include "math_extra_semisimple_Lie_algebras.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_general_polynomial_computations_advanced_implementation.h"
#include "math_extra_semisimple_Lie_algebras_implementation.h"
#include "math_extra_finite_groups_implementation.h" // undefined reference to `WeylGroupRepresentation<Rational>::reset()
#include "math_rational_function_implementation.h"

#include <cmath>
#include <cfloat>

template <class Coefficient>
bool Calculator::innerGreatestCommonDivisorOrLeastCommonMultiplePolynomialTypePartTwo(
  Calculator& calculator,
  const Polynomial<Coefficient>& left,
  const Polynomial<Coefficient>& right,
  const ExpressionContext &context,
  Expression& output,
  bool doGCD
) {
  MacroRegisterFunctionWithName("Calculator::innerGreatestCommonDivisorOrLeastCommonMultiplePolynomialTypePartTwo");
  Polynomial<Coefficient> outputPolynomial;
  if (left.isEqualToZero()) {
    return calculator << "Not allowed to take gcd/lcm of zero. ";
  }
  Coefficient one = left.coefficients[0].one();
  if (doGCD) {
    Polynomial<Coefficient>::greatestCommonDivisor(
      left, right, outputPolynomial, one, &calculator.comments
    );
  } else {
    Polynomial<Coefficient>::leastCommonMultiple(
      left, right, outputPolynomial, one, &calculator.comments
    );
  }
  return output.assignValueWithContext(outputPolynomial, context, calculator);
}

bool Calculator::innerGreatestCommonDivisorOrLeastCommonMultipleAlgebraic(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool doGCD
) {
  if (input.size() != 3) {
    return false;
  }
  Expression left = input[1];
  Expression right = input[2];
  if (!input.mergeContexts(left, right)) {
    return false;
  }
  Polynomial<AlgebraicNumber> leftPolynomial, rightPolynomial;
  if (!left.isOfType(&leftPolynomial) || !right.isOfType(&rightPolynomial)) {
    return false;
  }
  return Calculator::innerGreatestCommonDivisorOrLeastCommonMultiplePolynomialTypePartTwo(
    calculator, leftPolynomial, rightPolynomial, left.getContext(), output, doGCD
  );
}

bool Calculator::innerGreatestCommonDivisorOrLeastCommonMultipleModular(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool doGCD
) {
  if (input.size() != 3) {
    return false;
  }
  Expression left = input[1];
  Expression right = input[2];
  if (!input.mergeContexts(left, right)) {
    return false;
  }
  Polynomial<ElementZmodP> leftPolynomial, rightPolynomial;
  if (!left.isOfType(&leftPolynomial) || !right.isOfType(&rightPolynomial)) {
    return false;
  }
  if (leftPolynomial.isEqualToZero() || rightPolynomial.isEqualToZero()) {
    calculator
    << "Greatest common divisor / "
    << "least common multiple with zero not allowed. ";
    return output.makeError("Error in least common multiple / greatest common divisor.", calculator);
  }
  LargeIntegerUnsigned modulus = leftPolynomial.coefficients[0].modulus;

  if (modulus > static_cast<unsigned>(ElementZmodP::maximumModulusForUserFacingPolynomialDivision)) {
    return calculator
    << "Polynomial modulus exceeds the maximum allowed "
    << "for user-facing polynomial division: "
    << ElementZmodP::maximumModulusForUserFacingPolynomialDivision << ". ";
  }
  if (!modulus.isPossiblyPrime(0, true)) {
    return calculator << "Cannot do GCD / lcm: modulus "
    << modulus << " is not prime. ";
  }
  return Calculator::innerGreatestCommonDivisorOrLeastCommonMultiplePolynomialTypePartTwo(
    calculator, leftPolynomial, rightPolynomial, left.getContext(), output, doGCD
  );
}

bool Calculator::innerGreatestCommonDivisorOrLeastCommonMultiplePolynomial(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool doGCD
) {
  MacroRegisterFunctionWithName("Calculator::innerGreatestCommonDivisorOrLeastCommonMultiplePolynomial");
  if (input.size() != 3) {
    return false;
  }
  const Expression& left = input[1];
  if (left.isOfType<Polynomial<ElementZmodP> >()) {
    return Calculator::innerGreatestCommonDivisorOrLeastCommonMultipleModular(calculator, input, output, doGCD);
  }
  if (left.isOfType<Polynomial<AlgebraicNumber> >()) {
    return Calculator::innerGreatestCommonDivisorOrLeastCommonMultipleAlgebraic(calculator, input, output, doGCD);
  }
  Vector<Polynomial<Rational> > polynomials;
  ExpressionContext theContext(calculator);
  if (!calculator.getVectorFromFunctionArguments(
    input,
    polynomials,
    &theContext,
    2,
    CalculatorConversions::functionPolynomial<Rational>
  )) {
    return output.makeError("Failed to extract a list of 2 polynomials. ", calculator);
  }
  return Calculator::innerGreatestCommonDivisorOrLeastCommonMultiplePolynomialTypePartTwo(
    calculator, polynomials[0], polynomials[1], theContext, output, doGCD
  );
}

bool Calculator::getListPolynomialVariableLabelsLexicographic(
  const Expression& input,
  Vector<Polynomial<AlgebraicNumber> >& output,
  ExpressionContext& outputContext
) {
  MacroRegisterFunctionWithName("Calculator::getListPolynomialVariableLabelsLexicographic");
  ExpressionContext theContextStart(*this);
  if (!this->getVectorFromFunctionArguments(
    input,
    output,
    &theContextStart,
    0,
    CalculatorConversions::functionPolynomial<AlgebraicNumber>
  )) {
    return false;
  }
  if (output.size < 2) {
    return false;
  }
  int numVars = theContextStart.numberOfVariables();
  HashedList<Expression> theVars;
  theVars.setExpectedSize(numVars);
  for (int i = 0; i < numVars; i ++) {
    theVars.addOnTop(theContextStart.getVariable(i));
  }
  theVars.quickSortAscending();
  PolynomialSubstitution<AlgebraicNumber> substitution;
  substitution.setSize(numVars);
  const AlgebraicNumber& one = this->theObjectContainer.theAlgebraicClosure.one();
  for (int i = 0; i < substitution.size; i ++) {
    int currentIndex = theVars.getIndex(theContextStart.getVariable(i));
    substitution[i].makeMonomial(
      currentIndex,
      1,
      one
    );
  }
  for (int i = 0; i < output.size; i ++) {
    Polynomial<AlgebraicNumber>& currentP = output[i];
    currentP.substitution(substitution, one);
  }
  return outputContext.setVariables(theVars);
}

bool DynkinSimpleType:: hasPrecomputedSubalgebras() const {
  if (this->theLetter == 'F') {
    return true;
  }
  if (this->theLetter == 'G') {
    return true;
  }
  if (this->theLetter == 'A' && this->theRank <= 6) {
    return true;
  }
  if (this->theLetter == 'B' && this->theRank <= 4) {
    return true;
  }
  if (this->theLetter == 'D' && this->theRank <= 4) {
    return true;
  }
  if (this->theLetter == 'C' && this->theRank <= 5) {
    return true;
  }
  if (this->theLetter == 'E' && this->theRank == 6) {
    return true;
  }
  return false;
}

// WARNING
// Do not use the following web addresses to identify the server,
// EXCEPT in special circumstances described below.
// Instead, to get the web address of the
// calculator server,
// please use global.hostNoPort.
// That address is extracted from the
// incoming message headers
// and is how our server is seen from the outside world.
// HOWEVER,
// the incoming message headers may be forged.
// In most cases, this is not a problem
// as the forgery will
// affect negatively only the forgerer.
// HOWEVER,
// we CANNOT rely on the headers when sending
// activation emails, as an attacker could claim my email,
// forge his headers, and send me activation email which would
// wrongly link to the host given in his forged headers
// - that would be an evil site prompting me for password.
// So, only in this very special circumstance,
// we can only rely on a hard-coded web address.

std::string GlobalVariables::hopefullyPermanentWebAdress =
"https://calculator-algebra.org";

std::string GlobalVariables::hopefullyPermanentWebAdressOfServerExecutable =
"http://calculator-algebra.org/cgi-bin/calculator";
std::string GlobalVariables::hopefullyPermanent_HTTPS_WebAdressOfServerExecutable =
"https://calculator-algebra.org/cgi-bin/calculator";
std::string GlobalVariables::hopefullyPermanentWebAdressOfServerOutputFolder =
"http://calculator-algebra.org/";
std::string GlobalVariables::hopefullyPermanent_HTTPS_WebAdressJavascriptFolder =
"https://calculator-algebra.org/";

std::string Calculator::toStringSemismipleLieAlgebraLinksFromHD(const DynkinType& theType, FormatExpressions* theFormat) {
  (void) theFormat;
  std::stringstream out;
  SemisimpleLieAlgebra folderComputer;
  folderComputer.theWeyl.theDynkinType = theType;
  std::string prefixFolder = "calculator/";
  out << "<tr><td><a href=\""
  << folderComputer.toStringDisplayFolderName(prefixFolder)
  << folderComputer.toStringFileNameNoPathStructureConstants()
  << "\">"
  << theType[0].theLetter << theType[0].theRank << " structure constants</a></td>\n ";
  if (theType[0].hasPrecomputedSubalgebras()) {
    out << "<td><a href=\""
    << folderComputer.toStringDisplayFolderName(prefixFolder) << folderComputer.toStringFileNameNoPathSemisimpleSubalgebras()
    << "\">"
    << theType[0].theLetter << theType[0].theRank << " semisimple subalgebras</a>";
    out << "</td>\n ";
  } else {
    out << "<td>Not available</td>\n";
  }
  out << "<td><a href=\""
  << folderComputer.toStringDisplayFolderName(prefixFolder)
  << folderComputer.toStringFileNameRelativePathSlTwoSubalgebras() << "\">"
  << theType[0].theLetter << theType[0].theRank << " sl(2) triples</a></td>\n";
  out << "<td><a href=\""
  << folderComputer.toStringDisplayFolderName(prefixFolder)
  << folderComputer.toStringFileNameNoPathRootSubalgebras()
  << "\">" << theType[0].theLetter
  << theType[0].theRank << " root subalgebras</a></td>\n";
  return out.str();
}

void DynkinType::getPrecomputedDynkinTypes(List<DynkinType>& output) {
  MacroRegisterFunctionWithName("DynkinType::getPrecomputedDynkinTypes");
  output.setSize(0);
  DynkinType theType;
  theType.makeSimpleType('F', 4);
  output.addOnTop(theType);
  for (int i = 6; i <= 8; i ++) {
    theType.makeSimpleType('E', i);
    output.addOnTop(theType);
  }
  theType.makeSimpleType('G', 2);
  output.addOnTop(theType);
  for (int i = 1; i <= 8; i ++) {
    theType.makeSimpleType('A', i);
    output.addOnTop(theType);
  }
  for (int i = 4; i <= 8; i ++) {
    theType.makeSimpleType('D', i);
    output.addOnTop(theType);
  }
  for (int i = 2; i <= 8; i ++) {
    theType.makeSimpleType('B', i);
    output.addOnTop(theType);
  }
  for (int i = 3; i <= 8; i ++) {
    theType.makeSimpleType('C', i);
    output.addOnTop(theType);
  }
}

bool SemisimpleSubalgebras::computeStructureWriteFiles(
  SemisimpleLieAlgebra& newOwner,
  AlgebraicClosureRationals& ownerField,
  MapReferences<DynkinType, SemisimpleLieAlgebra>& containerSubalgebras,
  ListReferences<SltwoSubalgebras>& containerSl2Subalgebras,
  std::stringstream* outputStream,
  bool forceRecompute,
  bool doFullInit,
  bool computeNilradicals,
  bool computeModuleDecomposition,
  bool attemptToSolveSystems,
  bool computePairingTable,
  bool adjustCentralizers
) {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::computeStructureWriteFiles");
  this->ToStringExpressionString = CalculatorConversions::innerStringFromSemisimpleSubalgebras;
  this->owner = &newOwner;
  this->computeFolderNames(this->currentFormat);
  if (!FileOperations::fileExistsVirtual(this->virtualNameMainFile1) || forceRecompute) {
    if (doFullInit) {
      this->millisecondsComputationStart = global.getElapsedMilliseconds();
    }
    this->flagComputeNilradicals = computeNilradicals;
    this->flagcomputeModuleDecompositionsition = computeModuleDecomposition;
    this->flagAttemptToSolveSystems = attemptToSolveSystems;
    this->flagcomputePairingTable = computePairingTable;
    this->flagAttemptToAdjustCentralizers = adjustCentralizers;
    this->checkFileWritePermissions();
    if (doFullInit) {
      this->findTheSemisimpleSubalgebrasFromScratch(newOwner, ownerField, containerSubalgebras, containerSl2Subalgebras, nullptr);
    }
    this->writeReportToFiles();
  } else {
    if (outputStream != nullptr) {
      *outputStream << "Files precomputed, serving from HD. ";
    }
  }
  if (outputStream != nullptr) {
    *outputStream << "<br>Output file: <a href = \""
    << this->displayNameMainFile1WithPath << "\" target = \"_blank\">"
    << this->displayNameMainFile1NoPath << "</a>";
  }
  return true;
}

bool MathRoutines::isPrime(int theInt) {
  if (theInt <= 1) {
    return false;
  }
  for (int i = 2; i * i <= theInt; i += 2) {
    if (theInt % i == 0) {
      return false;
    }
  }
  return true;
}

bool Calculator::innerGroebner(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  int order,
  bool useModZp
) {
  MacroRegisterFunctionWithName("Calculator::innerGroebner");
  Vector<Polynomial<Rational> > inputVector;
  Vector<Polynomial<ElementZmodP> > inputVectorZmodP;
  ExpressionContext theContext(calculator);
  if (input.size() < 3) {
    return output.makeError("Function takes at least two arguments. ", calculator);
  }
  const Expression& numComputationsE = input[1];
  Rational upperBound = 0;
  if (!numComputationsE.isOfType(&upperBound)) {
    return output.makeError(
      "Failed to convert the first argument of "
      "the expression to rational number. ",
      calculator
    );
  }
  if (upperBound > 1000000) {
    return output.makeError(
      "Error: your upper limit of polynomial "
      "operations exceeds 1000000, which is too large. "
      "You may use negative or zero number "
      "give no computation bound, but please don't. ",
      calculator
    );
  }
  int upperBoundComputations = int(upperBound.getDoubleValue());
  output.reset(calculator);
  for (int i = 1; i < input.children.size; i ++) {
    output.children.addOnTop(input.children[i]);
  }
  int theMod = 0;
  if (useModZp) {
    if (!output[1].isSmallInteger(&theMod)) {
      return output.makeError(
        "Error: failed to extract modulo from the second argument. ",
        calculator
      );
    }
    if (!MathRoutines::isPrime(theMod)) {
      return output.makeError("Error: modulus not prime. ", calculator);
    }
  }
  if (!calculator.getVectorFromFunctionArguments<Polynomial<Rational> >(
    output,
    inputVector,
    &theContext,
    - 1,
    CalculatorConversions::functionPolynomial<Rational>
  )) {
    return output.makeError(
      "Failed to extract polynomial expressions",
      calculator
    );
  }
  for (int i = 0; i < inputVector.size; i ++) {
    inputVector[i].scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  }
  GroebnerBasisComputation<AlgebraicNumber> theGroebnerComputation;
  theContext.getFormat(theGroebnerComputation.theFormat);
  theContext.getFormat(global.theDefaultFormat.getElement());
  if (useModZp) {
    ElementZmodP tempElt;
    tempElt.makeMinusOne(static_cast<unsigned>(theMod));
    inputVectorZmodP.setSize(inputVector.size);
    for (int i = 0; i < inputVector.size; i ++) {
      inputVectorZmodP[i].makeZero();
      for (int j = 0; j < inputVector[i].size(); j ++) {
        tempElt = inputVector[i].coefficients[j];
        inputVectorZmodP[i].addMonomial(inputVector[i][j], tempElt);
      }
    }
  }
  List<Polynomial<AlgebraicNumber> > outputGroebner, outputGroebner2;
  outputGroebner = inputVector;
  outputGroebner2 = inputVector;
  if (order == MonomialP::Order::gradedLexicographic) {
    theGroebnerComputation.polynomialOrder.monomialOrder.setComparison(
      MonomialP::greaterThan_totalDegree_leftLargerWins
    );
  } else if (order == MonomialP::Order::gradedReverseLexicographic) {
    theGroebnerComputation.polynomialOrder.monomialOrder.setComparison(
      MonomialP::greaterThan_totalDegree_rightSmallerWins
    );
  } else if (order == MonomialP::Order::lexicographicOpposite) {
    theGroebnerComputation.polynomialOrder.monomialOrder.setComparison(
      MonomialP::greaterThan_rightLargerWins
    );
  } else if (order == MonomialP::Order::lexicographic){
    theGroebnerComputation.polynomialOrder.monomialOrder.setComparison(
      MonomialP::greaterThan_leftLargerWins
    );
  } else {
    global.fatal << "Unexpected order value: " << order << global.fatal;
  }
  theGroebnerComputation.theFormat.monomialOrder = theGroebnerComputation.polynomialOrder.monomialOrder;
  theGroebnerComputation.maximumPolynomialComputations = upperBoundComputations;
  bool success = theGroebnerComputation.transformToReducedGroebnerBasis(outputGroebner);
  std::stringstream out;
  out << theGroebnerComputation.toStringLetterOrder(false);
  out << "Letter/expression order: ";
  int numberOfVariables = theContext.numberOfVariables();
  for (int i = 0; i < numberOfVariables; i ++) {
    out << theContext.getVariable(i).toString();
    if (i != numberOfVariables - 1) {
      out << "&lt;";
    }
  }
  out << "<br>Starting basis (" << inputVector.size  << " elements): ";
  for (int i = 0; i < inputVector.size; i ++) {
    out << "<br>"
    << HtmlRoutines::getMathNoDisplay(
      inputVector[i].toString(&theGroebnerComputation.theFormat)
    );
  }
  if (success) {
    out << "<br>Minimal Groebner basis with "
    << outputGroebner.size
    << " elements, computed using algorithm 1, using "
    << theGroebnerComputation.numberPolynomialDivisions
    << " polynomial divisions. ";
    for (int i = 0; i < outputGroebner.size; i ++) {
      out << "<br> " << HtmlRoutines::getMathNoDisplay(
        outputGroebner[i].toString(&theGroebnerComputation.theFormat)
      );
    }
    out << "<br>Output in calculator-ready format: ";
    out << "<br>(";
    for (int i = 0; i < outputGroebner.size; i ++) {
      out << outputGroebner[i].toString(&theGroebnerComputation.theFormat);
      if (i != outputGroebner.size - 1) {
        out << ", <br>";
      }
    }
    out << ")";
  } else {
    out << "<br>Minimal Groebner basis not computed: "
    << "exceeded the user-given limit of "
    << upperBoundComputations << " polynomial operations. ";
    out << "<br>An intermediate non-Groebner basis containing total "
    << theGroebnerComputation.theBasis.size
    << " basis elements: ";
    out << "<br>GroebnerLexUpperLimit{}(10000, <br>";
    for (int i = 0; i < theGroebnerComputation.theBasis.size; i ++) {
      out << theGroebnerComputation.theBasis[i].element.toString(&theGroebnerComputation.theFormat);
      if (i != theGroebnerComputation.theBasis.size - 1) {
        out << ", <br>";
      }
    }
    out << ");";
  }
  return output.assignValue(out.str(), calculator);
}

bool Calculator::innerDeterminantPolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerDeterminantPolynomial");
  if (input.size() != 2) {
    return false;
  }
  Matrix<Polynomial<Rational> > matPol;
  ExpressionContext context(calculator);
  if (!calculator.functionGetMatrix(
    input[1],
    matPol,
    &context,
    - 1,
    CalculatorConversions::functionPolynomial<Rational>
  )) {
    return calculator << "<hr>Failed to convert the input to "
    << "matrix of polynomials. ";
  }
  if (matPol.numberOfRows != matPol.numberOfColumns) {
    return output.makeError("<hr>Failed to compute determinant: matrix is non-square. ", calculator);
  }
  if (matPol.numberOfRows > 8) {
    return calculator << "<hr>Failed to compute determinant: "
    << "matrix is larger than 8 x 8, and your matrix had "
    << matPol.numberOfRows << " rows. Note that you can compute "
    << "determinant using the \\det function which "
    << "does Gaussian elimination "
    << "and will work for large rational matrices. "
    << "This function is meant to be used with honest "
    << "polynomial entries. ";
  }
  Polynomial<Rational> outputPoly;
  outputPoly.makeDeterminantFromSquareMatrix(matPol);
  return output.assignValueWithContext(outputPoly, context, calculator);
}

bool Calculator::innerTranspose(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerTranspose");
  if (
    !input.isSequenceNElements() &&
    !input.isMatrix() &&
    !input.startsWithGivenOperation(Calculator::Atoms::transpose)
  ) {
    return false;
  }
  Matrix<Expression> theMat;
  if (input.startsWithGivenOperation(Calculator::Atoms::transpose)) {
    calculator.getMatrixExpressionsFromArguments(input, theMat);
  } else {
    calculator.getMatrixExpressions(input, theMat);
  }
  // The commented code used to be here. I don't remember why I added it, perhaps there was a solid reason?
  // If the code is uncommented, then ((1,2),(3,5))^t will not be transposed according to expectation.
  // If the commented code needs to be restored, please document why.
  // if (input.isSequenceNElements())
  //   if (theMat.numberOfRows !=1)
  //     return false;
  theMat.transpose();
  return output.assignMatrixExpressions(theMat, calculator, true, true);
}

void Plot::operator+=(const Plot& other) {
  MacroRegisterFunctionWithName("Plot::operator+=");
  if (other.priorityCanvasName > this->priorityCanvasName) {
    this->setCanvasName(other.getCanvasName());
  } else if (this->getCanvasName() == "" && this->priorityCanvasName == other.priorityCanvasName) {
    this->setCanvasName(other.getCanvasName());
  }
  if (other.priorityCanvasName > this->priorityCanvasName) {
    this->priorityCanvasName = other.priorityCanvasName;
  }
  if (this->priorityCanvasName == 0) {
    this->setCanvasName("");
  }
  if (this->dimension == - 1) {
    this->dimension = other.dimension;
  }
  if (other.priorityViewRectangle > this->priorityViewRectangle) {
    this->highBoundY = other.highBoundY;
    this->lowBoundY = other.lowBoundY;
    this->theLowerBoundAxes = other.theLowerBoundAxes;
    this->theUpperBoundAxes = other.theUpperBoundAxes;
  }
  if (other.priorityViewRectangle == this->priorityViewRectangle) {
    this->highBoundY = MathRoutines::maximum(this->highBoundY, other.highBoundY);
    this->lowBoundY = MathRoutines::minimum(this->lowBoundY,  other.lowBoundY);
    this->theUpperBoundAxes = MathRoutines::maximum(this->theUpperBoundAxes, other.theUpperBoundAxes);
    this->theLowerBoundAxes = MathRoutines::minimum(this->theLowerBoundAxes, other.theLowerBoundAxes);
  }
  this->thePlots.addListOnTop(other.thePlots);
  if (other.priorityWindow > this->priorityWindow) {
    this->desiredHtmlHeightInPixels = other.desiredHtmlHeightInPixels;
    this->desiredHtmlWidthInPixels = other.desiredHtmlWidthInPixels;
  } else if (this->priorityWindow == other.priorityWindow) {
    if (this->desiredHtmlHeightInPixels < other.desiredHtmlHeightInPixels) {
      this->desiredHtmlHeightInPixels = other.desiredHtmlHeightInPixels;
    }
    if (this->desiredHtmlWidthInPixels < other.desiredHtmlWidthInPixels) {
      this->desiredHtmlWidthInPixels = other.desiredHtmlWidthInPixels;
    }
  }
  if (!other.flagIncludeCoordinateSystem) {
    this->flagIncludeCoordinateSystem = false;
  }
  this->boxesThatUpdateMe.addOnTopNoRepetition(other.boxesThatUpdateMe);
  this->priorityWindow = MathRoutines::maximum(this->priorityWindow, other.priorityWindow);
  this->priorityViewRectangle = MathRoutines::maximum(this->priorityViewRectangle, other.priorityViewRectangle);
}

bool Plot::operator==(const Plot& other) const {
  return
  this->priorityWindow == other.priorityWindow &&
  this->priorityCanvasName == other.priorityCanvasName &&
  this->priorityViewRectangle == other.priorityViewRectangle &&
  this->desiredHtmlHeightInPixels == other.desiredHtmlHeightInPixels &&
  this->desiredHtmlWidthInPixels == other.desiredHtmlWidthInPixels &&
  ((this->highBoundY        - other.highBoundY       ) == 0.0) &&
  ((this->lowBoundY         - other.lowBoundY        ) == 0.0) &&
  ((this->theLowerBoundAxes - other.theLowerBoundAxes) == 0.0) &&
  ((this->theUpperBoundAxes - other.theUpperBoundAxes) == 0.0) &&
  this->thePlots == other.thePlots &&
  this->boxesThatUpdateMe == other.boxesThatUpdateMe &&
  this->getCanvasName() == other.getCanvasName() &&
  this->dimension == other.dimension &&
  this->flagIncludeCoordinateSystem == other.flagIncludeCoordinateSystem;
}

void Plot::operator+=(const PlotObject& other) {
  if (other.dimension != - 1 && this->dimension != - 1 && this->dimension != other.dimension) {
    global.fatal << "Attempting to add plot of dimension: "
    << this->dimension << " to a plot of dimension: "
    << other.dimension << ". " << global.fatal;
  }
  if (this->dimension == - 1) {
    this->dimension = other.dimension;
  }
  this->thePlots.addOnTop(other);
  this->setCanvasName("");
}

bool PlotObject::operator==(const PlotObject& other) const {
  return
  ((this->xLow      - other.xLow     ) == 0.0)                               &&
  ((this->xHigh     - other.xHigh    ) == 0.0)                               &&
  ((this->yLow      - other.yLow     ) == 0.0)                               &&
  ((this->yHigh     - other.yHigh    ) == 0.0)                               &&
  ((this->paramLow  - other.paramLow ) == 0.0)                               &&
  ((this->paramHigh - other.paramHigh) == 0.0)                               &&
  ((this->lineWidth - other.lineWidth) == 0.0)                               &&
  this->thePlotString                  == other.thePlotString                &&
  this->fillStyle                      == other.fillStyle                    &&
  this->thePlotStringWithHtml          == other.thePlotStringWithHtml        &&
  this->colorRGB                       == other.colorRGB                     &&
  this->colorFillRGB                   == other.colorFillRGB                 &&
  this->dimension                      == other.dimension                    &&
  this->colorUV                        == other.colorUV                      &&
  this->colorVU                        == other.colorVU                      &&
  this->colorJS                        == other.colorJS                      &&
  this->lineWidthJS                    == other.lineWidthJS                  &&
  this->numSegmenTsJS                  == other.numSegmenTsJS                &&
  this->thePointS                      == other.thePointS                    &&
  this->thePointsDouble                == other.thePointsDouble              &&
  this->thePointsJS                    == other.thePointsJS                  &&
  this->theRectangles                  == other.theRectangles                &&
  this->thePlotType                    == other.thePlotType                  &&
  this->manifoldImmersion              == other.manifoldImmersion            &&
  this->coordinateFunctionsE           == other.coordinateFunctionsE         &&
  this->coordinateFunctionsJS          == other.coordinateFunctionsJS        &&
  this->variablesInPlay                == other.variablesInPlay              &&
  this->theVarRangesJS                 == other.theVarRangesJS               &&
  this->leftPtE                        == other.leftPtE                      &&
  this->rightPtE                       == other.rightPtE                     &&
  this->paramLowE                      == other.paramLowE                    &&
  this->paramHighE                     == other.paramHighE                   &&
  this->numSegmentsE                   == other.numSegmentsE                 &&
  this->variablesInPlayJS              == other.variablesInPlayJS            &&
  this->leftBoundaryIsMinusInfinity    == other.leftBoundaryIsMinusInfinity  &&
  this->rightBoundaryIsMinusInfinity   == other.rightBoundaryIsMinusInfinity &&
  this->leftPtJS                       == other.leftPtJS                     &&
  this->rightPtJS                      == other.rightPtJS                    &&
  this->colorFillJS                    == other.colorFillJS                  &&
  this->paramLowJS                     == other.paramLowJS                   &&
  this->paramHighJS                    == other.paramHighJS                  &&
  this->defaultLengthJS                == other.defaultLengthJS
  ;
}

PlotObject::PlotObject() {
  this->xLow         = 0;
  this->xHigh        = 0;
  this->paramLow     = 0;
  this->paramHigh    = 0;
  this->yLow         = 0;
  this->yHigh        = 0;
  this->colorRGB     = 0;
  this->lineWidth    = 1;
  this->colorFillRGB = 0;
  this->dimension    = - 1;
  this->rightBoundaryIsMinusInfinity = false;
  this->leftBoundaryIsMinusInfinity = false;
}

void PlotObject::computeYBounds() {
  MacroRegisterFunctionWithName("PlotObject::computeYBounds");
  for (int i = 0; i < this->thePointsDouble.size; i ++) {
    this->yHigh = MathRoutines::maximum(this->yHigh, this->thePointsDouble[i][1]);
    this->yLow = MathRoutines::minimum(this->yLow, this->thePointsDouble[i][1]);
  }
}

std::string PlotObject::getPlotStringFromFunctionStringAndRanges(
  bool useHtml,
  const std::string& functionStringPostfixNotation,
  const std::string& functionStringCalculatorFormat,
  double inputLowerBound,
  double inputUpperBound
) {
  std::stringstream out;
  if (useHtml) {
    out << "<br>";
  }
  out << "\n\n%Function formula: " << functionStringCalculatorFormat << "\n\n";
  if (useHtml) {
    out << "<br>";
  }
  out << "\\psplot[linecolor =\\fcColorGraph, plotpoints =1000]{"
  << FloatingPoint::doubleToString(inputLowerBound) << "}{"
  << FloatingPoint::doubleToString(inputUpperBound) << "}{";
  out << functionStringPostfixNotation << "}";
  return out.str();
}

Plot::Plot() {
  this->theLowerBoundAxes = - 0.5;
  this->theUpperBoundAxes = 1;
  this->lowBoundY = - 0.5;
  this->highBoundY = 0.5;
  this->flagIncludeExtraHtmlDescriptions = true;
  this->desiredHtmlHeightInPixels = 350;
  this->desiredHtmlWidthInPixels = 350;
  this->defaultLineColor = 0;
  this->flagPlotShowJavascriptOnly = false;
  this->priorityWindow = 0;
  this->priorityViewRectangle = 0;
  this->priorityCanvasName = 0;
  this->dimension = - 1;
  this->flagDivAlreadyDisplayed = false;
  this->flagIncludeCoordinateSystem = true;
}

void Plot::computeAxesAndBoundingBox() {
  MacroRegisterFunctionWithName("Plot::computeAxesAndBoundingBox");
  this->theLowerBoundAxes = - 0.5;
  this->theUpperBoundAxes = 1.1;
  this->lowBoundY = - 0.5;
  this->highBoundY = 1.1;
  for (int k = 0; k < this->thePlots.size; k ++) {
    this->thePlots[k].computeYBounds();
    this->theLowerBoundAxes = MathRoutines::minimum(this->thePlots[k].xLow, theLowerBoundAxes);
    this->theUpperBoundAxes = MathRoutines::maximum(this->thePlots[k].xHigh, theUpperBoundAxes);
    this->lowBoundY = MathRoutines::minimum(this->thePlots[k].yLow, this->lowBoundY);
    this->highBoundY = MathRoutines::maximum(this->thePlots[k].yHigh, this->highBoundY);
/*    for (int j = 0; j < this->thePlots[k].theLines.size; j ++) {
      List<Vector<double> > currentLine = this->thePlots[k].theLines[j];
      this->theLowerBoundAxes =MathRoutines::minimum(this->theLowerBoundAxes, currentLine[0][0]);
      this->theLowerBoundAxes =MathRoutines::minimum(this->theLowerBoundAxes, currentLine[1][0]);
      this->theUpperBoundAxes =MathRoutines::maximum(this->theUpperBoundAxes, currentLine[0][0]);
      this->theUpperBoundAxes =MathRoutines::maximum(this->theUpperBoundAxes, currentLine[1][0]);
      this->lowBoundY =MathRoutines::minimum  (currentLine[0][1], this->lowBoundY);
      this->lowBoundY =MathRoutines::minimum  (currentLine[1][1], this->lowBoundY);
      this->highBoundY =MathRoutines::maximum (currentLine[0][1], this->highBoundY);
      this->highBoundY =MathRoutines::maximum (currentLine[1][1], this->highBoundY);
    }*/
    for (int j = 0; j < this->thePlots[k].thePointsDouble.size; j ++) {
      Vector<double>& currentPoint = this->thePlots[k].thePointsDouble[j];
      if (!this->isOKVector(currentPoint)) {
        continue;
      }
      this->theLowerBoundAxes = MathRoutines::minimum(this->theLowerBoundAxes, currentPoint[0]);
      this->theUpperBoundAxes = MathRoutines::maximum(this->theUpperBoundAxes, currentPoint[0]);
      this->lowBoundY = MathRoutines::minimum (currentPoint[1], this->lowBoundY);
      this->highBoundY = MathRoutines::maximum (currentPoint[1], this->highBoundY);
    }
  }
}

void Plot::computeAxesAndBoundingBox3d() {
  MacroRegisterFunctionWithName("Plot::computeAxesAndBoundingBox3d");
  this->theLowerBoundAxes = - 0.5;
  this->theUpperBoundAxes = 1.1;
  this->lowBoundY = - 0.5;
  this->highBoundY = 1.1;
  for (int k = 0; k < this->thePlots.size; k ++) {
    this->thePlots[k].computeYBounds();
    this->theLowerBoundAxes = MathRoutines::minimum(this->thePlots[k].xLow, theLowerBoundAxes);
    this->theUpperBoundAxes = MathRoutines::maximum(this->thePlots[k].xHigh, theUpperBoundAxes);
    this->lowBoundY = MathRoutines::minimum(this->thePlots[k].yLow, this->lowBoundY);
    this->highBoundY = MathRoutines::maximum(this->thePlots[k].yHigh, this->highBoundY);
/*    for (int j = 0; j < this->thePlots[k].theLines.size; j ++) {
      List<Vector<double> > currentLine = this->thePlots[k].theLines[j];
      this->theLowerBoundAxes =MathRoutines::minimum(this->theLowerBoundAxes, currentLine[0][0]);
      this->theLowerBoundAxes =MathRoutines::minimum(this->theLowerBoundAxes, currentLine[1][0]);
      this->theUpperBoundAxes =MathRoutines::maximum(this->theUpperBoundAxes, currentLine[0][0]);
      this->theUpperBoundAxes =MathRoutines::maximum(this->theUpperBoundAxes, currentLine[1][0]);
      this->lowBoundY =MathRoutines::minimum  (currentLine[0][1], this->lowBoundY);
      this->lowBoundY =MathRoutines::minimum  (currentLine[1][1], this->lowBoundY);
      this->highBoundY =MathRoutines::maximum (currentLine[0][1], this->highBoundY);
      this->highBoundY =MathRoutines::maximum (currentLine[1][1], this->highBoundY);
    }*/
    for (int j = 0; j < this->thePlots[k].thePointsDouble.size; j ++) {
      Vector<double>& currentPoint = this->thePlots[k].thePointsDouble[j];
      if (!this->isOKVector(currentPoint)) {
        continue;
      }
      this->theLowerBoundAxes = MathRoutines::minimum(this->theLowerBoundAxes, currentPoint[0]);
      this->theUpperBoundAxes = MathRoutines::maximum(this->theUpperBoundAxes, currentPoint[0]);
      this->lowBoundY = MathRoutines::minimum(currentPoint[1], this->lowBoundY);
      this->highBoundY = MathRoutines::maximum(currentPoint[1], this->highBoundY);
    }
  }
}

bool Plot::isOKVector(const Vector<double>& input) {
  for (int i = 0; i < input.size; i ++) {
    if (std::isnan(input[i])) {
      return false;
    }
  }
  return true;
}

std::string Plot::commonCanvasSetup() {
  std::stringstream out;
  out
  << "var theDrawer = window.calculator.drawing;\n"
  << "if (document.getElementById('" << this->getCanvasName() << "') === null) {\n"
  << "  return;\n"
  << "}\n"
  << "theDrawer.deleteCanvas('" << this->getCanvasName() << "');\n";
  return out.str();
}

std::string Plot::getPlotHtml3d(Calculator& owner) {
  MacroRegisterFunctionWithName("Plot::getPlotHtml3d");
  owner.flagHasGraphics = true;
  std::stringstream outContent, outScript;
  this->computeCanvasNameIfNecessary(owner.theObjectContainer.canvasPlotCounter);
  if (!owner.flagPlotShowJavascriptOnly) {
    outContent << "<canvas width =\"" << this->desiredHtmlWidthInPixels
    << "\" height =\"" << this->desiredHtmlHeightInPixels << "\" "
    << "style =\"border:solid 1px\" id =\""
    << this->getCanvasName()
    << "\" "
    << ">"
    << "Your browser does not support the HTML5 canvas tag.</canvas><br>"
    << "<span id =\"" << this->getCanvasName() << "Controls\"></span>"
    << "<span id =\"" << this->getCanvasName() << "Messages\"></span>";
  }
  outContent << "<script language =\"javascript\">\n";
  std::string canvasFunctionName = "functionMake" + this->getCanvasName();
  std::string canvasResetFunctionName = "functionReset" + this->getCanvasName();
  outScript
  << "function " << canvasResetFunctionName << "() {\n"
  << canvasFunctionName << "();\n"
  << "}\n";
  outScript << "function " << canvasFunctionName << "() {\n";
  for (int i = 0; i < this->boxesThatUpdateMe.size; i ++) {
    InputBox& currentBox = owner.theObjectContainer.
    theUserInputTextBoxesWithValues.getValueCreate(this->boxesThatUpdateMe[i]);
    outScript << " window.calculator.drawing.plotUpdaters['"
    << currentBox.getSliderName() << "'] =" << "'" << this->getCanvasName() << "'"
    << ";\n";
  }
  List<std::string> the3dObjects;
  the3dObjects.setSize(this->thePlots.size);
  int funCounter = 0;
  for (int i = 0; i < this->thePlots.size; i ++) {
    PlotObject& currentO = this->thePlots[i];
    if (currentO.thePlotType == "surface") {
      outScript << currentO.getJavascriptSurfaceImmersion(the3dObjects[i], this->getCanvasName(), funCounter) << "\n ";
    }
    if (currentO.thePlotType == "parametricCurve") {
      outScript << currentO.getJavascriptCurveImmersionIn3d(the3dObjects[i], this->getCanvasName(), funCounter) << "\n ";
    }
  }
  outScript << this->commonCanvasSetup();
  outScript << "var theCanvas = theDrawer.getCanvas('" << this->getCanvasName() << "');\n"
  << "theCanvas.initialize('" << this->getCanvasName() << "');\n";
  outScript << "theCanvas.canvasResetFunction = " << canvasResetFunctionName << ";\n";
  for (int i = 0; i < this->thePlots.size; i ++) {
    PlotObject& currentPlot = this->thePlots[i];
    if (currentPlot.thePlotType == "surface") {
      outScript
      << "theCanvas.drawSurface("
      <<  the3dObjects[i]
      << ");\n";
    }
    if (currentPlot.thePlotType == "parametricCurve") {
      outScript
      << "theCanvas.drawCurve("
      <<  the3dObjects[i]
      << ");\n";
    }
    if (currentPlot.thePlotType == "setProjectionScreen" && currentPlot.thePointsDouble.size >= 2) {
      outScript
      << "theCanvas.screenBasisUserDefault = "
      << "["
      << currentPlot.thePointsDouble[0].toStringSquareBracketsBasicType()
      << ","
      << currentPlot.thePointsDouble[1].toStringSquareBracketsBasicType()
      << "];\n";
      outScript << "theCanvas.screenBasisUser = theCanvas.screenBasisUserDefault.slice();\n";
    }
    if (currentPlot.thePlotType == "label") {
      outScript
      << "theCanvas.drawText({"
      << "location: "
      << currentPlot.thePointsDouble[0].toStringSquareBracketsBasicType()
      << ", "
      << "text:"
      << "'" << currentPlot.thePlotString << "'"
      << ", "
      << "color: "
      << "'"
      << currentPlot.colorJS
      << "'"
      << "});\n";
    }
    if (currentPlot.thePlotType == "segment" && currentPlot.thePointsDouble.size >= 2) {
      outScript
      << "theCanvas.drawLine("
      << currentPlot.thePointsDouble[0].toStringSquareBracketsBasicType()
      << ", "
      << currentPlot.thePointsDouble[1].toStringSquareBracketsBasicType()
      << ", "
      << "'"
      << currentPlot.colorJS
      << "'"
      << ", "
      << currentPlot.lineWidth
      << ");\n";
    }
  }
  if (owner.flagPlotNoControls) {
    outScript << "theCanvas.flagShowPerformance = false;\n";
  } else {
    outScript << "theCanvas.flagShowPerformance = true;\n";
  }
  outScript << "theCanvas.setBoundingBoxAsDefaultViewWindow();\n"
  << "theCanvas.redraw();\n"
  << "}\n";
  outScript << canvasFunctionName << "();\n";
  outContent << outScript.str();
  outContent << "</script>";
  owner.theObjectContainer.graphicsScripts.setKeyValue(this->getCanvasName(), outScript.str());
  return outContent.str();
}

void Plot::setCanvasName(const std::string& input) {
  this->canvasNamE = StringRoutines::convertStringToJavascriptVariable(input);
}

std::string Plot::getCanvasName() const {
  return this->canvasNamE;
}

std::string Plot::toStringDebug() {
  std::stringstream out;
  out <<  "Objects: " << this->thePlots.size << "<br>";
  for (int i = 0; i < this->thePlots.size; i ++) {
    if (this->thePlots[i].thePlotType == "surface") {
      PlotObject& theSurface = this->thePlots[i];
      out << theSurface.toStringDebug();
    }
  }
  return out.str();
}

std::string PlotObject::toStringDebug() {
  MacroRegisterFunctionWithName("PlotSurfaceIn3d::toStringDebug");
  std::stringstream out;
  out << "colorUV: " << this->colorUV << "<br>";
  out << "colorVU: " << this->colorVU << "<br>";
  out << "Coord f-ns: " << this->coordinateFunctionsE.toStringCommaDelimited() << "<br>";
  out << "Vars: " << this->variablesInPlay << "<br>";
  out << "Var ranges: " << this->theVarRangesJS << "<br>";
  return out.str();
}

std::string PlotObject::getJavascriptCurveImmersionIn3d(
  std::string& outputCurveInstantiationJS, const std::string& canvasName, int& funCounter
) {
  MacroRegisterFunctionWithName("PlotSurfaceIn3d::getJavascriptCurveImmersionIn3d");
  std::stringstream out;
  funCounter ++;
  std::stringstream fnNameStream;
  fnNameStream << "theCanvasSurfaceFn_" << canvasName << "_" << funCounter;
  std::string fnName = fnNameStream.str();

  if (this->coordinateFunctionsJS.size == 3) {
    out << "function " << fnName
    << " (" << this->variablesInPlayJS[0]
    << "){\n";
    out << "return [ " << this->coordinateFunctionsJS[0] << ", "
    << this->coordinateFunctionsJS[1]
    << ", " << this->coordinateFunctionsJS[2] << "];\n";
    out << "}\n";
  } else {
    out << "//this->theCoordinateFunctionsJS has "
    << this->coordinateFunctionsJS.size
    << " elements instead of 3 (expected).\n";
  }
  std::stringstream curveInstStream;
  curveInstStream << "new theDrawer.CurveThreeD(" << fnName
  << ", " << this->paramLowJS << ", " << this->paramHighJS;
  if (this->numSegmenTsJS.size > 0) {
    curveInstStream << ", " << this->numSegmenTsJS[0];
  } else {
    curveInstStream << ", 100";
  }
  curveInstStream << ", "
  << "\"" << this->colorJS << "\"";
  curveInstStream << ", " << this->lineWidth;
  curveInstStream << ")";
  outputCurveInstantiationJS = curveInstStream.str();
  return out.str();
}

std::string PlotObject::getJavascriptSurfaceImmersion(
  std::string& outputSurfaceInstantiationJS, const std::string& canvasName, int& funCounter
) {
  MacroRegisterFunctionWithName("PlotSurfaceIn3d::getJavascriptSurfaceImmersion");
  std::stringstream out;
  funCounter ++;
  std::stringstream fnNameStream;
  fnNameStream << "theCanvasSurfaceFn_" << canvasName << "_" << funCounter;
  std::string fnName = fnNameStream.str();

  if (this->coordinateFunctionsJS.size == 3) {
    out << "function " << fnName
    << " (" << this->variablesInPlayJS[0] << ","
    << this->variablesInPlayJS[1] << "){\n";
    out << "return [ " << this->coordinateFunctionsJS[0] << ", "
    << this->coordinateFunctionsJS[1]
    << ", " << this->coordinateFunctionsJS[2] << "];\n";
    out << "}\n";
  } else
    out << "//this->theCoordinateFunctionsJS has "
    << this->coordinateFunctionsJS.size
    << " elements instead of 3 (expected).\n";
  if (this->theVarRangesJS.size != 2) {
    out << "//this->theVarRangesJS has "
    << this->theVarRangesJS.size << " elements instead of 2 (expected).";
  } else if (this->theVarRangesJS[0].size != 2 || this->theVarRangesJS[1].size != 2) {
    out << "//this->theVarRangesJS had unexpected value: "
    << this->theVarRangesJS.size;
  } else {
    std::stringstream surfaceInstStream;
    surfaceInstStream << "new window.calculator.drawing.Surface("
    << fnName
    << ", [[" << this->theVarRangesJS[0][0] << "," << this->theVarRangesJS[1][0] << "],"
    << " ["   << this->theVarRangesJS[0][1] << ", " << this->theVarRangesJS[1][1] << "]], ";
    if (this->numSegmenTsJS.size > 1) {
      surfaceInstStream << "[" << this->numSegmenTsJS[0] << ","
      << this->numSegmenTsJS[1] << "], ";
    } else {
      surfaceInstStream << "[22, 4], ";
    }
    surfaceInstStream << "{colorContour: \"black\", ";
    if (this->colorUV != "") {
      surfaceInstStream << "colorUV: \"" << this->colorUV << "\",";
    } else {
      surfaceInstStream << "colorUV: \"red\",";
    }
    if (this->colorVU != "") {
      surfaceInstStream << "colorVU: \"" << this->colorVU << "\"";
    } else {
      surfaceInstStream << "colorVU: \"pink\"";
    }
    surfaceInstStream << "}" << ",";
    if (this->lineWidthJS != "") {
      surfaceInstStream << this->lineWidthJS;
    } else {
      surfaceInstStream << "1";
    }
    surfaceInstStream << ")";
    outputSurfaceInstantiationJS = surfaceInstStream.str();
  }
  return out.str();
}

std::string Plot::getPlotHtml(Calculator& owner) {
  MacroRegisterFunctionWithName("Plot::getPlotHtml");
  if (this->dimension == 3) {
    return this->getPlotHtml3d(owner);
  } else if (this->dimension == 2) {
    return this->getPlotHtml2d(owner);
  } else {
    std::stringstream out;
    out << "Error:dimension =" << this->dimension;
    return out.str();
  }
}

std::string PlotObject::getJavascriptParametricCurve2D(
  std::string& outputPlotInstantiationJS, const std::string& canvasName, int& funCounter
) {
  MacroRegisterFunctionWithName("PlotSurfaceIn3d::getJavascript2dPlot");
  std::stringstream out;
  List<std::string> fnNames;
  fnNames.setSize(this->coordinateFunctionsJS.size);
  for (int i = 0; i < this->coordinateFunctionsJS.size; i ++) {
    funCounter ++;
    std::stringstream fnNameStream;
    fnNameStream << "theCanvasCoordinateFn_" << canvasName << "_" << funCounter;
    fnNames[i] = fnNameStream.str();
    if (this->variablesInPlayJS.size > 0) {
      out << "function " << fnNames[i]
      << " (" << this->variablesInPlayJS[0] << "){\n";
      out << "return " << this->coordinateFunctionsJS[i] << ";\n";
      out << "}\n";
    } else {
      out << "console.log(\"Error: function with zero variables.\");";
    }
  }
  if (this->coordinateFunctionsJS.size != 2) {
    outputPlotInstantiationJS = "";
    return out.str();
  }
  std::stringstream fnInstStream;
  fnInstStream.precision(7);
  fnInstStream << "drawCurve("
  << "[" << fnNames[0] << ", " << fnNames[1] << "]"
  << ", " << this->paramLowJS << ", " << this->paramHighJS << ", ";
  if (this->numSegmenTsJS.size > 0) {
    fnInstStream << this->numSegmenTsJS[0] << ", ";
  } else {
    fnInstStream << "200, ";
  }
  fnInstStream << "'" << this->colorJS << "'";
  if (this->lineWidthJS != "") {
    fnInstStream << ", " << this->lineWidthJS;
  } else {
    fnInstStream << ", " << this->lineWidth;
  }
  fnInstStream << ");\n";
  outputPlotInstantiationJS = fnInstStream.str();
  return out.str();
}

std::string PlotObject::getJavascriptDirectionField(
  std::string& outputPlotInstantiationJS, const std::string& canvasName, int& funCounter
) {
  MacroRegisterFunctionWithName("PlotSurfaceIn3d::getJavascriptDirectionField");
  std::stringstream out;
  std::string fnName;
  funCounter ++;
  std::stringstream fnNameStream;
  fnNameStream << "theCanvasCoordinateFn_" << canvasName << "_" << funCounter;
  fnName = fnNameStream.str();
  if (this->variablesInPlayJS.size > 0) {
    out << "function " << fnName
    << " (";
    for (int i = 0; i < this->variablesInPlayJS.size; i ++) {
      out << this->variablesInPlayJS[i];
      if (i != this->variablesInPlayJS.size - 1) {
        out << ", ";
      }
    }
    out << "){\n";
    out << "return " << this->manifoldImmersionJS << ";\n";
    out << "}\n";
  } else {
    out << "console.log(\"Error: function with zero variables.\");";
  }
  std::stringstream fnInstStream;
  fnInstStream.precision(7);
  fnInstStream << "drawVectorField("
  << fnName
  << ", true"
  << ", ";
  for (int i = 0; i < 2; i ++) {
    fnInstStream << "[";
    for (int j = 0; j < this->theVarRangesJS.size; j ++) {
      if (i < this->theVarRangesJS[j].size) {
        fnInstStream << this->theVarRangesJS[j][i];
        if (j != this->theVarRangesJS.size - 1) {
          fnInstStream << ", ";
        }
      } else {
        fnInstStream << "(bad variable range)";
      }
    }
    fnInstStream << "], ";
  }
  fnInstStream << "[";
  for (int i = 0; i < this->numSegmenTsJS.size; i ++) {
    fnInstStream << this->numSegmenTsJS[i];
    if (i != this->numSegmenTsJS.size - 1) {
      fnInstStream << ", ";
    }
  }
  fnInstStream << "], ";
  fnInstStream << this->defaultLengthJS << ", ";
  fnInstStream << "'" << this->colorJS << "'";
  if (this->lineWidthJS != "") {
    fnInstStream << ", " << this->lineWidthJS;
  } else {
    fnInstStream << ", " << this->lineWidth;
  }
  fnInstStream << ");\n";
  outputPlotInstantiationJS = fnInstStream.str();
  return out.str();
}

std::string PlotObject::getJavascript2dPlot(
  std::string& outputPlotInstantiationJS, const std::string& canvasName, int& funCounter
) {
  MacroRegisterFunctionWithName("PlotSurfaceIn3d::getJavascript2dPlot");
  std::stringstream out;
  std::stringstream fnNameStream;
  funCounter ++;
  fnNameStream << "theCanvasPlotFn_" << canvasName << "_"
  << funCounter;
  std::string fnName = fnNameStream.str();
  if (this->variablesInPlayJS.size > 0) {
    out << "function " << fnName
    << " (" << HtmlRoutines::getJavascriptVariable(this->variablesInPlayJS[0]) << "){\n";
    out << "return " << this->coordinateFunctionsJS[0] << ";\n";
    out << "}\n";
  } else {
    out << "console.log(\"Error: function with zero variables.\");";
  }
  std::stringstream fnInstStream;
  fnInstStream << "drawFunction("
  << fnName
  << ", " << this->leftPtJS << ", " << this->rightPtJS << ", ";
  if (this->numSegmenTsJS.size > 0) {
    fnInstStream << this->numSegmenTsJS[0] << ", ";
  } else {
    fnInstStream << "200, ";
  }
  fnInstStream << "'" << this->colorJS << "'" << ", ";
  if (this->lineWidthJS != "") {
    fnInstStream << this->lineWidthJS;
  } else {
    fnInstStream << this->lineWidth;
  }
  fnInstStream << ");\n";
  outputPlotInstantiationJS = fnInstStream.str();
  return out.str();
}

std::string PlotObject::getJavascriptPoints(
  std::string& outputPlotInstantiationJS, const std::string& canvasName, int& funCounter
) {
  MacroRegisterFunctionWithName("PlotSurfaceIn3d::getJavascriptPoints");
  (void) (canvasName);
  (void) funCounter;
  std::stringstream fnInstStream;
  fnInstStream << "drawPoints([";
  for (int i = 0; i < this->thePointsJS.numberOfRows; i ++) {
    fnInstStream << "[";
    for (int j = 0; j < this->thePointsJS.numberOfColumns; j ++) {
      fnInstStream << this->thePointsJS(i, j);
      if (j != this->thePointsJS.numberOfColumns - 1) {
        fnInstStream << ", ";
      }
    }
    fnInstStream << "]";
    if (i != this->thePointsJS.numberOfRows - 1) {
      fnInstStream << ", ";
    }
  }
  fnInstStream << "], ";
  fnInstStream << "'" << this->colorJS << "');\n";
  outputPlotInstantiationJS = fnInstStream.str();
  return "";
}

std::string PlotObject::toStringPointsList() {
  MacroRegisterFunctionWithName("PlotObject::toStringPointsList");
  std::stringstream out;
  out << "[";
  for (int j = 0; j < this->thePointsDouble.size; j ++) {
    out << this->thePointsDouble[j].toStringSquareBracketsBasicType();
    if (j != this->thePointsDouble.size - 1) {
      out << ",";
    }
  }
  out << "]";

  return out.str();
}

void Plot::computeCanvasNameIfNecessary(int& canvasCounter) {
  if (this->getCanvasName() != "") {
    return;
  }
  canvasCounter ++;
  std::stringstream canvasNameStream;
  canvasNameStream << "theCanvas" << canvasCounter;
  this->setCanvasName(canvasNameStream.str());
}

std::string Plot::getPlotHtml2d(Calculator& owner) {
  MacroRegisterFunctionWithName("Plot::getPlotHtml2d");
  owner.flagHasGraphics = true;
  if (this->flagDivAlreadyDisplayed) {
    return "[plot alredy displayed]";
  }
  this->flagDivAlreadyDisplayed = true;
  this->computeCanvasNameIfNecessary(owner.theObjectContainer.canvasPlotCounter);
  std::stringstream out;
  if (this->priorityViewRectangle <= 0) {
    this->computeAxesAndBoundingBox();
  }
  if (!this->flagPlotShowJavascriptOnly) {
    out << "<canvas width =\"" << this->desiredHtmlWidthInPixels
    << "\" height =\"" << this->desiredHtmlHeightInPixels << "\" "
    << "style =\"border:solid 1px\" id =\""
    << this->getCanvasName()
    << "\" "
    << ">"
    << "Your browser does not support the HTML5 canvas tag.</canvas><br>"
    << "<span id =\"" << this->getCanvasName() << "Controls\"></span>";
    if (!owner.flagPlotNoControls) {
      out << "<br>";
    }
    out << "<span id =\"" << this->getCanvasName() << "Messages\"></span>";
  }
  std::stringstream outScript;
  std::string canvasFunctionName = "functionMake" + this->getCanvasName();
  std::string canvasResetFunctionName = "functionReset" + this->getCanvasName();
  outScript
  << "function " << canvasResetFunctionName << "() {\n"
  << canvasFunctionName << "();\n"
  << "}\n";

  outScript << "function " << canvasFunctionName << "() {\n";
  for (int i = 0; i < this->boxesThatUpdateMe.size; i ++) {
    InputBox& currentBox = owner.theObjectContainer.theUserInputTextBoxesWithValues.getValueCreate(this->boxesThatUpdateMe[i]);
    outScript << "  window.calculator.drawing.plotUpdaters['"
    << currentBox.getSliderName() << "'] ="
    << "'" << this->getCanvasName() << "'"
    << ";\n";
  }
  int funCounter = 0;
  List<std::string> theFnPlots;
  theFnPlots.setSize(this->thePlots.size);
  for (int i = 0; i < this->thePlots.size; i ++) {
    PlotObject& currentPlot = this->thePlots[i];
    if (currentPlot.thePlotType == "plotFunction") {
      outScript << currentPlot.getJavascript2dPlot(theFnPlots[i], this->getCanvasName(), funCounter) << "\n ";
    }
    if (currentPlot.thePlotType == "parametricCurve") {
      outScript << currentPlot.getJavascriptParametricCurve2D(theFnPlots[i], this->getCanvasName(), funCounter) << "\n ";
    }
    if (currentPlot.thePlotType == "plotDirectionField") {
      outScript << currentPlot.getJavascriptDirectionField(theFnPlots[i], this->getCanvasName(), funCounter) << "\n ";
    }
    if (currentPlot.thePlotType == "points") {
      currentPlot.getJavascriptPoints(theFnPlots[i], this->getCanvasName(), funCounter);
    }
  }
  outScript << this->commonCanvasSetup();
  outScript << "var theCanvas = theDrawer.getCanvasTwoD('" << this->getCanvasName() << "');\n"
  << "theCanvas.initialize('" << this->getCanvasName() << "');\n";
  outScript << "theCanvas.canvasResetFunction = " << canvasResetFunctionName << ";\n";
  if (owner.flagPlotNoControls) {
    outScript << "theCanvas.flagShowPerformance = false;\n";
  } else {
    outScript << "theCanvas.flagShowPerformance = true;\n";
  }
  outScript.precision(7);
  for (int i = 0; i < this->thePlots.size; i ++) {
    PlotObject& currentPlot = this->thePlots[i];
    if (currentPlot.thePlotType == "plotFunction") {
      outScript << "theCanvas." << theFnPlots[i];
      continue;
    }
    if (currentPlot.thePlotType == "parametricCurve") {
      outScript << "theCanvas." << theFnPlots[i];
      continue;
    }
    if (currentPlot.thePlotType == "plotDirectionField") {
      outScript << "theCanvas." << theFnPlots[i];
      continue;
    }
    if (currentPlot.thePlotType == "points") {
      outScript << "theCanvas." << theFnPlots[i];
      continue;
    }
    if (currentPlot.thePlotType == "label") {
      if (currentPlot.thePointsDouble.size > 0) {
        outScript << "theCanvas.drawText("
        << currentPlot.thePointsDouble[0].toStringSquareBracketsBasicType()
        << ", "
        << "'" << currentPlot.thePlotString << "'"
        << ", "
        << "'" << currentPlot.colorJS << "');\n" ;
      }
      continue;
    }
    if (currentPlot.thePlotType == "plotFillStart") {
      outScript << "theCanvas.plotFillStart('" << currentPlot.colorFillJS << "');\n";
      continue;
    }
    if (currentPlot.thePlotType == "plotFillFinish") {
      outScript << "theCanvas.plotFillFinish();\n";
      continue;
    }
    if (currentPlot.thePlotType == "axesGrid") {
      outScript << "theCanvas.drawGrid();\n";
      continue;
    }
    if (currentPlot.thePlotType == "pathFilled") {
      outScript << "theCanvas.drawPathFilled( ";
      outScript << currentPlot.toStringPointsList();
      outScript << ", "
      << "\"" << currentPlot.colorJS << "\""
      << ","
      << "\"" << currentPlot.colorFillJS << "\""
      << ");\n";
      continue;
    }
    outScript << "theCanvas.drawPath( ";
    outScript << currentPlot.toStringPointsList();
    outScript << ", " << "\"";
    if (currentPlot.colorJS == "") {
      outScript << DrawingVariables::getColorHtmlFromColorIndex (currentPlot.colorRGB);
    } else {
      outScript << currentPlot.colorJS;
    }
    outScript << "\"";
    if (currentPlot.lineWidthJS != "") {
      outScript << ", " << "\"" << currentPlot.lineWidthJS << "\"";
    } else {
      outScript << ", " << "\"" << currentPlot.lineWidth << "\"";
    }
    outScript << ");\n";
  }
  if (this->flagIncludeCoordinateSystem) {
    //outScript << "theCanvas.drawLine([" << this->theLowerBoundAxes*1.10
    //<< ",0],[" << this->theUpperBoundAxes*1.10 << ",0], 'black',1);\n";
    //outScript << "theCanvas.drawLine([0," << this->lowBoundY *1.10
    //<< "],[0," << this->highBoundY*1.10 << "], 'black',1);\n";
    //outScript << "theCanvas.drawLine([1,-0.1],[1,0.1], 'black',1);\n";
    //outScript << "theCanvas.drawText([1,-0.2],'1','black');\n";
    outScript << "theCanvas.drawCoordinateAxes();\n";
  }
  if (this->priorityViewRectangle > 0) {
    outScript << "theCanvas.setViewWindow("
    << "[" << this->theLowerBoundAxes * 1.10 << ", " << this->lowBoundY * 1.10 << "]"
    << ", "
    << "[" << this->theUpperBoundAxes * 1.10 << ", " << this->highBoundY * 1.10 << "]"
    << ");\n";
  } else {
    outScript << "theCanvas.computeViewWindow();\n";
  }
  outScript
  << "theCanvas.redraw();\n"
  << "}\n"
  << " var currentCanvas = window.calculator.drawing.getCanvasTwoDNullOnFailure('" << this->getCanvasName() << "');\n"
  << "if (currentCanvas !== null) {\n"
  << "currentCanvas.canvasResetFunction = "
  << canvasFunctionName << ";\n"
  << canvasFunctionName << "();\n"
  << "}\n";
  owner.theObjectContainer.graphicsScripts.setKeyValue(this->getCanvasName(), outScript.str());
  out << "<script language =\"javascript\">\n" << outScript.str() << "</script>";
  return out.str();
}

std::string Plot::getPlotStringAddLatexCommands(bool useHtml) {
  MacroRegisterFunctionWithName("Plot::getPlotStringAddLatexCommands");
  std::stringstream resultStream;
  this->computeAxesAndBoundingBox();
  std::string lineSeparator = useHtml ? "<br>\n" : "\n";
  resultStream << "\\documentclass{article}\\usepackage{pstricks}\\usepackage{auto-pst-pdf}"
  << "\\usepackage{pst-math}\\usepackage{pst-plot}";
  resultStream << lineSeparator << "\\newcommand{\\fcLabels}[2]{\\rput[t](#1, -0.1){$x$}\\rput[r](-0.1, #2){$y$}}" << lineSeparator;
  resultStream << "\\addtolength{\\hoffset}{-3.5cm}\\addtolength{\\textwidth}{6.8cm}"
  << "\\addtolength{\\voffset}{-3.2cm}\\addtolength{\\textheight}{6.3cm}"
  << lineSeparator;
  resultStream << "\\newcommand{\\fcColorGraph}{red}" << lineSeparator << "\\begin{document} \\pagestyle{empty}" << lineSeparator
  << "\\newcommand{\\fcColorAreaUnderGraph}{cyan}" << lineSeparator << "\\newcommand{\\fcColorNegativeAreaUnderGraph}{orange}"
  << "\\newcommand{\\fcAxesStandard}[4]{ \\psframe*[linecolor =white](! #1 #2)(! #3 0.1 add #4 01 add) "
  << "\\psaxes[ticks = none, labels = none]{<->}(0,0)(#1, #2)(#3, #4)\\fcLabels{#3}{#4}}"
  << lineSeparator << " \\psset{xunit =1cm, yunit =1cm}";
  resultStream << lineSeparator;
  resultStream << "\\begin{pspicture}(" << FloatingPoint::doubleToString(theLowerBoundAxes - 0.4) << ", "
  << FloatingPoint::doubleToString(lowBoundY - 0.4) << ")("
  << FloatingPoint::doubleToString(theUpperBoundAxes + 0.4)
  << "," << FloatingPoint::doubleToString(highBoundY + 0.5) << ")\n\n";
  resultStream << lineSeparator << "\\tiny\n" << lineSeparator;
  resultStream << " \\fcAxesStandard{" << FloatingPoint::doubleToString(theLowerBoundAxes - 0.15)
  << "}{" << FloatingPoint::doubleToString(lowBoundY - 0.15) << "}{"
  << FloatingPoint::doubleToString(theUpperBoundAxes + 0.15) << "}{"
  << FloatingPoint::doubleToString(highBoundY + 0.15) << "}" << lineSeparator;
  for (int i = 0; i < this->thePlots.size; i ++) {
    if (useHtml) {
      resultStream << this->thePlots[i].thePlotStringWithHtml << lineSeparator;
    } else {
      resultStream << this->thePlots[i].thePlotString << lineSeparator;
    }
  }
  resultStream << "\\end{pspicture}\n\n" << lineSeparator << "\\end{document}";
  return resultStream.str();
}

bool Expression::assignStringParsed(
  const std::string& theString, MapList<std::string, Expression, MathRoutines::hashString>* substitutions, Calculator& owner
) {
  MacroRegisterFunctionWithName("Expression::assignStringParsed");
  Expression commands, result;
  List<SyntacticElement> outputSyntacticSoup, outputSyntacticStack;
  std::string outputSyntacticErrors;
  if (!owner.parseAndExtractExpressions(theString, commands, outputSyntacticSoup, outputSyntacticStack, &outputSyntacticErrors)) {
    this->assignValue(outputSyntacticErrors, owner);
    return false;
  }
  if (commands.startsWith(owner.opEndStatement(), 2)) {
    result = commands[1];
  } else {
    result = commands;
  }
  if (substitutions != nullptr) {
    MapList<Expression, Expression> theSubs;
    for (int i = 0; i < substitutions->size(); i ++) {
      Expression theSubbed;
      theSubbed.makeAtom(substitutions->theKeys[i], owner);
      theSubs.setKeyValue(theSubbed, substitutions->theValues[i]);
    }
    result.substituteRecursively(theSubs);
  }
  *this = result;
  return true;
}

bool Expression::isSuitableForSubstitution() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->isBuiltInType() || this->startsWith(this->owner->opBind())) {
    return false;
  }
  return true;
}

bool Expression::isSuitableForRecursion() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->isAtom() || this->isBuiltInType() || this->startsWith(this->owner->opBind())) {
    return false;
  }
  return true;
}

void Expression::substituteRecursively(MapList<Expression, Expression>& theSubs) {
  if (theSubs.contains(*this)) {
    (*this) = theSubs.getValueCreate(*this);
    return;
  }
  this->substituteRecursivelyInChildren(theSubs);
}

void Expression::substituteRecursivelyInChildren(MapList<Expression, Expression>& theSubs) {
  if (!this->isSuitableForSubstitution()) {
    return;
  }
  Expression tempE;
  for (int i = 0; i < this->size(); i ++) {
    if (theSubs.contains((*this)[i])) {
      this->setChild(i, theSubs.getValueCreate((*this)[i]));
    } else {
      tempE = (*this)[i];
      tempE.substituteRecursivelyInChildren(theSubs);
      if (!(tempE == (*this)[i])) {
        this->setChild(i, tempE);
      }
    }
  }
}

void Expression::substituteRecursively(const Expression& toBeSubbed, const Expression& toBeSubbedWith) {
  if ((*this) == toBeSubbed) {
    (*this) = toBeSubbedWith;
    return;
  }
  this->substituteRecursivelyInChildren(toBeSubbed, toBeSubbedWith);
}

void Expression::substituteRecursivelyInChildren(const Expression& toBeSubbed, const Expression& toBeSubbedWith) {
  if (!this->isSuitableForSubstitution()) {
    return;
  }
  Expression tempE;
  for (int i = 0; i < this->size(); i ++) {
    if (toBeSubbed == (*this)[i]) {
      this->setChild(i, toBeSubbedWith);
    } else {
      tempE = (*this)[i];
      tempE.substituteRecursivelyInChildren(toBeSubbed, toBeSubbedWith);
      if (!(tempE == (*this)[i])) {
        this->setChild(i, tempE);
      }
    }
  }
}

bool Calculator::innerSuffixNotationForPostScript(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerSuffixNotationForPostScript");
  RecursionDepthCounter theCounter(&calculator.recursionDepth);
  if (*theCounter.theCounter == calculator.maximumRecursionDepth - 2) {
    return output.assignValue(std::string("..."), calculator);
  }
  std::string currentString;
  if (input.isOperation(&currentString)) {
    if (input.toString() == "e") {
      return output.assignValue<std::string>(" 2.718281828 ", calculator);
    }
    if (input.toString() == "\\pi") {
      return output.assignValue<std::string>(" 3.141592654 ", calculator);
    }
    if (input.theData >= calculator.numberOfPredefinedAtoms) {
      return output.assignValue(currentString, calculator);
    }
    if (currentString == "|") {
      return output.assignValue<std::string>("abs ", calculator);
    }
    if (currentString == "+") {
      return output.assignValue<std::string>("add ", calculator);
    }
    if (currentString == "*") {
      return output.assignValue<std::string>("mul ", calculator);
    }
    if (currentString == "-") {
      return output.assignValue<std::string>("sub ", calculator);
    }
    if (currentString == "/") {
      return output.assignValue<std::string>("div ", calculator);
    }
    if (currentString == "^") {
      return output.assignValue<std::string>("exp ", calculator);
    }
    if (currentString == "\\log") {
      return output.assignValue<std::string>("ln ", calculator);
    }
    if (currentString == "\\sin") {
      return output.assignValue<std::string>(" 57.29578 mul sin ", calculator);
    }
    if (currentString == "\\cos") {
      return output.assignValue<std::string>(" 57.29578 mul cos ", calculator);
    }
    if (currentString == "\\tan") {
      return output.assignValue<std::string>(" 57.29578 mul tan ", calculator);
    }
    if (currentString == "\\arctan") {
      return output.assignValue<std::string>("ATAN ", calculator);
    }
    if (currentString == "\\arcsin") {
      return output.assignValue<std::string>("ASIN ", calculator);
    }
    if (currentString == "\\arccos") {
      return output.assignValue<std::string>("ACOS ", calculator);
    }
    if (currentString == "\\sqrt") {
      return output.assignValue<std::string>("sqrt ", calculator);
    }
    return output.makeError("Cannot convert " + currentString + " to suffix notation.", calculator);
  }
  std::stringstream out;
  out.precision(7);
  bool hasDoubleValue = false;
  double theDoubleValue = - 1;
  Rational theRat;
  if (input.isOfType<Rational>(&theRat)) {
    if (
      theRat.getDenominator().isIntegerFittingInInt(nullptr) &&
      theRat.getNumerator().isIntegerFittingInInt(nullptr)
    ) {
      out << " " << theRat.getNumerator().toString() << " " << theRat.getDenominator() << " div ";
      return output.assignValue(out.str(), calculator);
    }
    hasDoubleValue = true;
    theDoubleValue = input.getValue<Rational>().getDoubleValue();
  }
  if (input.isOfType<AlgebraicNumber>()) {
    hasDoubleValue = input.getValue<AlgebraicNumber>().evaluatesToDouble(&theDoubleValue);
  }
  if (input.isOfType<double>()) {
    hasDoubleValue = true;
    theDoubleValue = input.getValue<double>();
  }
  if (hasDoubleValue) {
    out << " " << FloatingPoint::doubleToString(theDoubleValue);
    return output.assignValue(out.str(), calculator);
  }
  Expression currentE;
  bool useUsualOrder =
    !input[0].isOperationGiven(calculator.opDivide()) &&
    !input[0].isOperationGiven(calculator.opThePower());
  if (useUsualOrder) {
    for (int i = input.size() - 1; i >= 1; i --) {
      if (!calculator.innerSuffixNotationForPostScript(calculator, input[i], currentE)) {
        return output.makeError("Failed to convert " + input[i].toString(), calculator);
      }
      if (!currentE.isOfType(&currentString)) {
        return output.makeError("Failed to convert " + input[i].toString(), calculator);
      }
      out << currentString << " ";
    }
  } else {
    for (int i = 1; i < input.size(); i ++) {
      if (!calculator.innerSuffixNotationForPostScript(calculator, input[i], currentE)) {
        return output.makeError("Failed to convert " + input[i].toString(), calculator);
      }
      if (!currentE.isOfType(&currentString)) {
        return output.makeError("Failed to convert " + input[i].toString(), calculator);
      }
      out << currentString << " ";
    }
  }
  if (!calculator.innerSuffixNotationForPostScript(calculator, input[0], currentE)) {
    return output.makeError("Failed to convert " + input[0].toString(), calculator);
  }
  if (!currentE.isOfType(&currentString)) {
    return output.makeError("Failed to convert " + input[0].toString(), calculator);
  }
  out << currentString << " ";
  return output.assignValue(out.str(), calculator);
}


template <class Coefficient>
Coefficient ElementUniversalEnveloping<Coefficient>::getKillingFormProduct(
  const ElementUniversalEnveloping<Coefficient>& right
) const {
  MacroRegisterFunctionWithName("ElementUniversalEnveloping::getKillingFormProduct");
  if (this->isEqualToZero()) {
    return 0;
  }
  Coefficient result = 0;
  ElementUniversalEnveloping<Coefficient> adadAppliedToMon, tempElt;
  SemisimpleLieAlgebra* theOwner;
  theOwner = &this->getOwner();
  MonomialUniversalEnveloping<Coefficient> baseGen;
  for (int i = 0; i < theOwner->getNumberOfGenerators(); i ++) {
    baseGen.makeGenerator(i, *theOwner);
    adadAppliedToMon.makeZero(*theOwner);
    adadAppliedToMon.addMonomial(baseGen, 1);
    right.adjointRepresentationAction(adadAppliedToMon, tempElt);
    tempElt.simplify();
    this->adjointRepresentationAction(tempElt, adadAppliedToMon);
    adadAppliedToMon.simplify();
    result += adadAppliedToMon.getCoefficientOf(baseGen);
  }
  return result;
}

template <class Coefficient>
Coefficient SemisimpleLieAlgebra::getKillingForm(
  const ElementSemisimpleLieAlgebra<Coefficient>& left,
  const ElementSemisimpleLieAlgebra<Coefficient>& right
) {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::getKillingForm");
  Coefficient result = 0;
  ElementSemisimpleLieAlgebra<Coefficient> adadAppliedToMon, tempElt;
  ChevalleyGenerator baseGen;
  for (int i = 0; i < this->getNumberOfGenerators(); i ++) {
    baseGen.makeGenerator(*this, i);
    adadAppliedToMon.makeZero();
    adadAppliedToMon.addMonomial(baseGen, 1);
    this->lieBracket(right, adadAppliedToMon, tempElt);
    this->lieBracket(left, tempElt, adadAppliedToMon);
    result += adadAppliedToMon.getCoefficientOf(baseGen);
  }
  return result;
}

bool Calculator::innerKillingForm(Calculator& calculator, const Expression& input, Expression& output) {
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
  ElementUniversalEnveloping<RationalFunction<Rational> > left, right;
  if (
    !leftE.isOfType<ElementUniversalEnveloping<RationalFunction<Rational> > >(&left) ||
    !rightE.isOfType<ElementUniversalEnveloping<RationalFunction<Rational> > >(&right)
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



class HistorySubExpression {
public:
  friend std::ostream& operator << (
    std::ostream& output, const HistorySubExpression& history
  ) {
    if (history.current == nullptr) {
      output << "(no expression)";
    } else {
      output << history.current->toString();
    }
    output << " active: " << history.lastActiveSubexpression;
    return output;
  }
  const Expression* current;
  int lastActiveSubexpression;
  HistorySubExpression(): current(nullptr), lastActiveSubexpression(- 1) {
  }
};

class ExpressionHistoryEnumerator {
public:
  Calculator* owner;
  int recursionDepth;
  int maximumRecursionDepth;
  Expression theHistory;
  List<Expression> output;
  List<List<std::string> > rulesNames;
  // List<List<std::string> > rulesDisplayNames;
  HashedList<std::string, MathRoutines::hashString> rulesToBeIgnored;
  MapList<std::string, std::string, MathRoutines::hashString> rulesDisplayNamesMap;
  bool computeRecursively(int incomingRecursionDepth, std::stringstream* commentsOnFailure);
  bool processTransformation(const Expression& current, std::stringstream* commentsOnFailure);
  bool processChildrenTransformations(int startIndex, int numberOfChildren, std::stringstream* commentsOnFailure);
  void initializeComputation();
  ExpressionHistoryEnumerator();
  bool checkInitialization() {
    if (this->owner == nullptr) {
      global.fatal << "Expression history enumerator has zero owner. " << global.fatal;
    }
    return true;
  }
  std::string toStringExpressionHistoryMerged();
  std::string toStringDebug();
};

void ExpressionHistoryEnumerator::initializeComputation() {
  MacroRegisterFunctionWithName("ExpressionHistoryEnumerator::initializeComputation");
  this->output.setSize(0);
  // this->rulesDisplayNames.setSize(0);
  this->rulesNames.setSize(0);
  // this->rulesToBeIgnored.clear();
  // this->rulesToBeIgnored.addOnTop("CommuteIfUnivariate");
  // this->rulesDisplayNamesMap.clear();
  // this->rulesDisplayNamesMap.setKeyValue("minus", "");
  // this->rulesDisplayNamesMap.setKeyValue("DistributeMultiplication", "");
  // this->rulesDisplayNamesMap.setKeyValue("MultiplyRationals", "");
  // this->rulesDisplayNamesMap.setKeyValue("ConstantExtraction", "");
  // this->rulesDisplayNamesMap.setKeyValue("MultiplyByOne", "");
  // this->rulesDisplayNamesMap.setKeyValue("AddTerms", "");
  // this->rulesDisplayNamesMap.setKeyValue("AssociativeRule", "");
}

ExpressionHistoryEnumerator::ExpressionHistoryEnumerator() {
  this->owner = nullptr;
  this->maximumRecursionDepth = 1000;
  this->recursionDepth = 0;
}

bool ExpressionHistoryEnumerator::computeRecursively(
  int incomingRecursionDepth, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("ExpressionHistoryEnumerator::computeRecursively");
  this->recursionDepth = incomingRecursionDepth;
  if (recursionDepth > this->maximumRecursionDepth) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "While computing expression history, "
      << "exceeded maximum recursion depth of "
      << this->maximumRecursionDepth << ". ";
    }
    return false;
  }
  if (!this->theHistory.startsWith(this->owner->opExpressionHistory())) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Corrupt expression history does not start with the expected atom: "
      << this->theHistory.toString();
      return false;
    }
  }
  int childrenToAccount = 0;
  int firstNonAccountedChildIndex = - 1;
  for (int i = 1; i < this->theHistory.size(); i ++) {
    const Expression& current = this->theHistory[i];
    if (current.startsWith(this->owner->opExpressionHistorySetChild())) {
      if (childrenToAccount == 0) {
        firstNonAccountedChildIndex = i;
      }
      childrenToAccount ++;
      continue;
    }
    if (!this->processChildrenTransformations(firstNonAccountedChildIndex, childrenToAccount, commentsOnFailure)) {
      return false;
    }
    childrenToAccount = 0;
    if (!this->processTransformation(current, commentsOnFailure)) {
      return false;
    }
  }
  if (!this->processChildrenTransformations(firstNonAccountedChildIndex, childrenToAccount, commentsOnFailure)) {
    return false;
  }
  return true;
}

bool ExpressionHistoryEnumerator::processChildrenTransformations(
  int startIndex, int numberOfChildren, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("ExpressionHistoryEnumerator::processChildrenTransformations");
  if (numberOfChildren <= 0) {
    return true;
  }
  List<int> indicesInParent;
  List<ExpressionHistoryEnumerator> childrenEnumerators;
  childrenEnumerators.setSize(numberOfChildren);
  indicesInParent.setSize(numberOfChildren);
  for (int i = 0; i < numberOfChildren; i ++) {
    const Expression& current = this->theHistory[startIndex + i];
    if (current.size() < 3) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "ExpressionHistorySetChild "
        << "expression does not have enough children. "
        << current.toString() ;
      }
      return false;
    }
    if (!current[1].isSmallInteger(&indicesInParent[i])) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "ExpressionHistorySetChild: bad child index. "
        << current.toString() ;
      }
      return false;
    }
    childrenEnumerators[i].owner = this->owner;
    childrenEnumerators[i].theHistory = current[2];
    if (!childrenEnumerators[i].computeRecursively(
      this->recursionDepth + 1, commentsOnFailure
    )) {
      return false;
    }
  }
  if (this->output.size == 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Missing starting expression. History: " << this->theHistory.toString();
    }
    return false;
  }
  List<int> currentIndices;
  currentIndices.initializeFillInObject(numberOfChildren, 0);
  while (true) {
    bool found = false;
    Expression next;
    next = *this->output.lastObject();
    List<std::string> nextRules;
    for (int i = 0; i < numberOfChildren; i ++) {
      int& currentIndex = currentIndices[i];
      List<Expression>& expressionSequence = childrenEnumerators[i].output;
      if (currentIndex + 1 < expressionSequence.size) {
        currentIndex ++;
        found = true;
        int indexInParent = indicesInParent[i];
        next.setChild(indexInParent, expressionSequence[currentIndex]);
        nextRules.addListOnTop(childrenEnumerators[i].rulesNames[currentIndex]);
      }
    }
    if (!found) {
      break;
    }
    this->output.addOnTop(next);
    this->rulesNames.addOnTop(nextRules);
  }
  return true;
}

bool ExpressionHistoryEnumerator::processTransformation(
  const Expression& current, std::stringstream* commentsOnFailure
) {
  if (!current.startsWith(this->owner->opExpressionHistorySet())) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Bad first atom in history child: " << current.toString();
    }
    return false;
  }
  if (current.size() < 2) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Not enough elements in history child: " << current.toString();
    }
    return false;
  }
  this->output.addOnTop(current[1]);
  this->rulesNames.setSize(this->rulesNames.size + 1);
  this->rulesNames.lastObject()->setSize(0);
  if (current.size() >= 3) {
    std::string incoming = current[2].toString();
    if (incoming != "") {
      this->rulesNames.lastObject()->addOnTop(incoming);
    }
  }
  return true;
}

std::string ExpressionHistoryEnumerator::toStringDebug() {
  MacroRegisterFunctionWithName("ExpressionHistoryEnumerator::toStringDebug");
  std::stringstream out;
  out << "<b>History</b><br>" << this->theHistory.toStringTreeHtml(- 1) << "<hr>";
  out << "" << this->theHistory.toStringSemiFull() << "<hr>";
  // out << "Current state: " << this->currentState.toString()
  // << "<br>";
  // out << "Current subtree: " << this->currentSubTree.toString();

  return out.str();
}

std::string ExpressionHistoryEnumerator::toStringExpressionHistoryMerged() {
  std::stringstream out;
  out << "\\(\\begin{array}{ll|l}";
  std::string prevEstring = "";
  List<std::string> currentRules;
  for (int j = 0; j < this->output.size; j ++) {
    std::string currentEstring = this->output[j].toString();
    if (currentEstring == prevEstring) {
      currentRules.addListOnTop(this->rulesNames[j]);
      continue;
    }
    prevEstring = currentEstring;
    currentRules.addListOnTop(this->rulesNames[j]);
    if (j > 0) {
      if (currentRules.size > 0) {
        out << "&\\text{" << currentRules.toStringCommaDelimited() << "}";
      }
      currentRules.setSize(0);
      out << "\\\\";
      out << "=";
    }
    out << "&\n" << currentEstring;
  }
  if (currentRules.size > 0) {
    out << "&\\text{apply: " << currentRules.toStringCommaDelimited() << "}";
  }
  out << "\\end{array}\\)";
  return out.str();
}

bool Calculator::innerLogEvaluationStepsDebug(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return Calculator::innerLogEvaluationStepsHumanReadableMerged(calculator, input, output, true);
}

bool Calculator::innerLogEvaluationSteps(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return Calculator::innerLogEvaluationStepsHumanReadableMerged(calculator, input, output, false);
}

bool Calculator::innerLogEvaluationStepsHumanReadableMerged(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool doDebug
) {
  MacroRegisterFunctionWithName("Calculator::innerLogEvaluationStepsHumanReadableMerged");
  Expression argument;
  if (input.size() == 2) {
    argument = input[1];
  } else {
    argument = input;
    argument.setChildAtomValue(0, calculator.opSequence());
  }
  Expression outputTransformation;
  bool notUsed = false;
  ExpressionHistoryEnumerator history;
  calculator.evaluateExpression(
    calculator,
    argument,
    outputTransformation,
    notUsed,
    - 1,
    &history.theHistory
  );
  std::stringstream out;
  history.owner = &calculator;
  history.computeRecursively(0, &out);
  out << history.toStringExpressionHistoryMerged();
  if (doDebug) {
    out << "<hr>" << history.toStringDebug();
  }
  return output.assignValue(out.str(), calculator);
}
