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
#include "math_extra_tree.h"

#include <cmath>
#include <cfloat>

template<class Element>
bool Matrix<Element>::SystemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution(
  Matrix<Element>& matA, Matrix<Element>& matb, Vector<Element>* outputSolution
) {
  // this function return true if Ax = b >= 0 has a solution with x >= 0 and records a solution x at outputPoint
  // else returns false, where b is a given nonnegative column vector, A is an n by m matrix
  // and x is a column vector with m entries
  Matrix<Rational> tempMatA;
  Vector<Rational> matX;
  Selection BaseVariables;
  Rational GlobalGoal;
  GlobalGoal.MakeZero();
  if (matA.NumRows != matb.NumRows) {
    global.fatal << "The number of inequalities: " << matA.NumRows << " does not match the number of "
    << "constaints: " << matb.NumRows << ". " << global.fatal;
  }
  for (int j = 0; j < matb.NumRows; j ++) {
    GlobalGoal += matb.elements[j][0];
    if (matb.elements[j][0].IsNegative()) {
      global.fatal << "Constraint index " << j << " is negative: "
      << matb.elements[j][0] << " which is not allowed. " << global.fatal;
    }
  }
  if (GlobalGoal.IsEqualToZero()) {
    return false;
  }
  int NumTrueVariables = matA.NumCols;
  //tempMatb.Assign(matb);
  tempMatA.init(matA.NumRows, NumTrueVariables + matA.NumRows);
  HashedList<Selection> VisitedVertices;
  VisitedVertices.Clear();
  BaseVariables.init(tempMatA.NumCols);
  tempMatA.MakeZero();
  matX.MakeZero(tempMatA.NumCols);
  for (int j = 0; j < matA.NumCols; j ++) {
    for (int i = 0; i < matA.NumRows; i ++) {
      tempMatA.elements[i][j].Assign(matA.elements[i][j]);
    }
  }
  for (int j = 0; j < matA.NumRows; j ++) {
    tempMatA.elements[j][j + NumTrueVariables].MakeOne();
    matX[j + NumTrueVariables] = (matb.elements[j][0]);
    BaseVariables.AddSelectionAppendNewIndex(j + NumTrueVariables);
  }
  Rational ChangeGradient; //Change, PotentialChange;
  int EnteringVariable = 0;
  bool WeHaveNotEnteredACycle = true;
  while (EnteringVariable != - 1 && WeHaveNotEnteredACycle && GlobalGoal.IsPositive()) {
    EnteringVariable = - 1; ChangeGradient.MakeZero();
    for (int i = 0; i < tempMatA.NumCols; i ++) {
      if (!BaseVariables.selected[i]) {
        Rational PotentialChangeGradient; bool hasAPotentialLeavingVariable;
        Matrix<Rational>::ComputePotentialChangeGradient(
          tempMatA, BaseVariables, NumTrueVariables, i, PotentialChangeGradient, hasAPotentialLeavingVariable
        );
        if (PotentialChangeGradient.IsGreaterThanOrEqualTo(ChangeGradient) && hasAPotentialLeavingVariable) {
          EnteringVariable = i;
          ChangeGradient.Assign(PotentialChangeGradient);
        }
      }
    }
    if (EnteringVariable != - 1) {
      int LeavingVariableRow;
      Rational MaxMovement;
      Matrix<Rational>::GetMaxMovementAndLeavingVariableRow(
        MaxMovement, LeavingVariableRow, EnteringVariable, tempMatA, matX, BaseVariables
      );
      Rational tempRat, tempTotalChange;
      if (tempMatA.elements[LeavingVariableRow][EnteringVariable].IsEqualToZero()) {
        global.fatal << "The leaving-entering coefficient is not allowed to be zero. " << global.fatal;
      }
      tempRat.Assign(tempMatA.elements[LeavingVariableRow][EnteringVariable]);
      tempRat.Invert();
      for (int i = 0; i < tempMatA.NumRows; i ++) {
        if (!tempMatA.elements[i][BaseVariables.elements[i]].IsEqualTo(1)) {
          global.fatal << "The base variable coefficient is required to be 1 at this point of code. "
          << global.fatal;
        }
      }
      tempMatA.RowTimesScalar(LeavingVariableRow, tempRat);
      tempTotalChange.Assign(MaxMovement);
      tempTotalChange.MultiplyBy(ChangeGradient);
      matX[EnteringVariable] += MaxMovement;
      if (!tempTotalChange.IsEqualToZero()) {
        VisitedVertices.Clear();
        GlobalGoal.Subtract(tempTotalChange);
      } else {
        int tempI = VisitedVertices.GetIndex(BaseVariables);
        if (tempI == - 1) {
          VisitedVertices.AddOnTop(BaseVariables);
        } else {
          WeHaveNotEnteredACycle = false;
        }
      }
      for (int i = 0; i < tempMatA.NumRows; i ++) {
        if (!tempMatA.elements[i][EnteringVariable].IsEqualToZero()&& i != LeavingVariableRow) {
          tempRat.Assign(tempMatA.elements[i][EnteringVariable]);
          tempRat.MultiplyBy(MaxMovement);
          matX[BaseVariables.elements[i]] -= tempRat;
          tempRat.Assign(tempMatA.elements[i][EnteringVariable]);
          tempRat.Minus();
          tempMatA.AddTwoRows(LeavingVariableRow, i, 0, tempRat);
        }
        if (i == LeavingVariableRow) {
          matX[BaseVariables.elements[i]] = 0;
        }
      }
      if (!matX[BaseVariables.elements[LeavingVariableRow]].IsEqualToZero()) {
        global.fatal << "Leaving variable coefficient not allowed to be zero. " << global.fatal;
      }
      BaseVariables.selected[BaseVariables.elements[LeavingVariableRow]] = false;
      BaseVariables.elements[LeavingVariableRow] = EnteringVariable;
      BaseVariables.selected[EnteringVariable] = true;
      //BaseVariables.ComputeDebugString();
      for (int i = 0; i < tempMatA.NumRows; i ++) {
        if (!tempMatA.elements[i][BaseVariables.elements[i]].IsEqualTo(1)) {
          global.fatal << "New base variable expected to be equal to 1. " << global.fatal;
        }
      }
    }
  }
  for (int i = NumTrueVariables; i < matX.size; i ++) {
    if (matX[i].IsPositive()) {
      return false;
    }
  }
  if (outputSolution != nullptr) {
    outputSolution->SetSize(NumTrueVariables);
    for (int i = 0; i < NumTrueVariables; i ++) {
      (*outputSolution)[i] = matX[i];
    }
  }
  return true;
}

template <>
bool CalculatorConversions::functionPolynomial<Rational>(Calculator& theCommands, const Expression& input, Expression& output);

bool Calculator::innerGCDOrLCMPoly(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  bool doGCD
) {
  MacroRegisterFunctionWithName("Calculator::innerGCDOrLCMPoly");
  Vector<Polynomial<Rational> > thePolys;
  Expression theContext(theCommands);
  if (!theCommands.GetVectorFromFunctionArguments(
    input,
    thePolys,
    &theContext,
    2,
    CalculatorConversions::functionPolynomial<Rational>
  )) {
    return output.MakeError("Failed to extract a list of 2 polynomials. ", theCommands);
  }
  Polynomial<Rational> outputPolynomial;
  if (doGCD) {
    RationalFunction::gcd(thePolys[0], thePolys[1], outputPolynomial);
  } else {
    RationalFunction::lcm(thePolys[0], thePolys[1], outputPolynomial);
  }
  return output.AssignValueWithContext(outputPolynomial, theContext, theCommands);
}

bool Calculator::GetListPolynomialVariableLabelsLexicographic(
  const Expression& input,
  Vector<Polynomial<AlgebraicNumber> >& output,
  Expression& outputContext
) {
  MacroRegisterFunctionWithName("Calculator::GetListPolynomialVariableLabelsLexicographic");
  Expression theContextStart(*this);
  if (!this->GetVectorFromFunctionArguments(
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
  int numVars = theContextStart.ContextGetNumContextVariables();
  HashedList<Expression> theVars;
  theVars.SetExpectedSize(numVars);
  for (int i = 0; i < numVars; i ++) {
    theVars.AddOnTop(theContextStart.ContextGetContextVariable(i));
  }
  theVars.QuickSortAscending();
  PolynomialSubstitution<AlgebraicNumber> theSub;
  theSub.SetSize(numVars);
  for (int i = 0; i < theSub.size; i ++) {
    int currentIndex = theVars.GetIndex(theContextStart.ContextGetContextVariable(i));
    theSub[i].MakeMonomiaL(
      currentIndex,
      1,
      this->theObjectContainer.theAlgebraicClosure.One(),
      numVars
    );
  }
  outputContext.MakeEmptyContext(*this);
  Expression PolyVarsE, tempE;
  PolyVarsE.reset(*this);
  PolyVarsE.children.Reserve(numVars + 1);
  tempE.MakeAtom(this->opPolynomialVariables(), *this);
  PolyVarsE.AddChildOnTop(tempE);
  for (int i = 0; i < theVars.size; i ++) {
    PolyVarsE.AddChildOnTop(theVars[i]);
  }
  for (int i = 0; i < output.size; i ++) {
    Polynomial<AlgebraicNumber>& currentP = output[i];
    currentP.Substitution(theSub);
  }
  return outputContext.AddChildOnTop(PolyVarsE);
}

bool DynkinSimpleType:: HasPrecomputedSubalgebras() const {
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

std::string Calculator::ToStringSemismipleLieAlgebraLinksFromHD(const DynkinType& theType, FormatExpressions* theFormat) {
  (void) theFormat;
  std::stringstream out;
  SemisimpleLieAlgebra folderComputer;
  folderComputer.theWeyl.theDynkinType = theType;
  std::string prefixFolder = "calculator/";
  out << "<tr><td><a href=\""
  << folderComputer.ToStringDisplayFolderName(prefixFolder)
  << folderComputer.ToStringFileNameNoPathStructureConstants()
  << "\">"
  << theType[0].theLetter << theType[0].theRank << " structure constants</a></td>\n ";
  if (theType[0].HasPrecomputedSubalgebras()) {
    out << "<td><a href=\""
    << folderComputer.ToStringDisplayFolderName(prefixFolder) << folderComputer.ToStringFileNameNoPathSemisimpleSubalgebras()
    << "\">"
    << theType[0].theLetter << theType[0].theRank << " semisimple subalgebras</a>";
    out << "</td>\n ";
  } else {
    out << "<td>Not available</td>\n";
  }
  out << "<td><a href=\""
  << folderComputer.ToStringDisplayFolderName(prefixFolder)
  << folderComputer.ToStringFileNameRelativePathSlTwoSubalgebras() << "\">"
  << theType[0].theLetter << theType[0].theRank << " sl(2) triples</a></td>\n";
  out << "<td><a href=\""
  << folderComputer.ToStringDisplayFolderName(prefixFolder)
  << folderComputer.ToStringFileNameNoPathRootSubalgebras()
  << "\">" << theType[0].theLetter
  << theType[0].theRank << " root subalgebras</a></td>\n";
  return out.str();
}

void DynkinType::GetPrecomputedDynkinTypes(List<DynkinType>& output) {
  MacroRegisterFunctionWithName("DynkinType::GetPrecomputedDynkinTypes");
  output.SetSize(0);
  DynkinType theType;
  theType.MakeSimpleType('F', 4);
  output.AddOnTop(theType);
  for (int i = 6; i <= 8; i ++) {
    theType.MakeSimpleType('E', i);
    output.AddOnTop(theType);
  }
  theType.MakeSimpleType('G', 2);
  output.AddOnTop(theType);
  for (int i = 1; i <= 8; i ++) {
    theType.MakeSimpleType('A', i);
    output.AddOnTop(theType);
  }
  for (int i = 4; i <= 8; i ++) {
    theType.MakeSimpleType('D', i);
    output.AddOnTop(theType);
  }
  for (int i = 2; i <= 8; i ++) {
    theType.MakeSimpleType('B', i);
    output.AddOnTop(theType);
  }
  for (int i = 3; i <= 8; i ++) {
    theType.MakeSimpleType('C', i);
    output.AddOnTop(theType);
  }
}

bool Calculator::innerGetLinksToSimpleLieAlgerbas(Calculator& theCommands, const Expression& input, Expression& output) {
  (void) input;//avoid unused parameter warning, portable
  std::stringstream outFromHD, outRecomputeLinks;

  outFromHD << "\n\n<p>\n\n<table>"
  << "<tr><td>Structure constants</td>"
  << "<td>Semisimple subalgebras</td> "
  << "<td>sl(2) subalgebras</td><td>root subalgebras</td> </tr>\n";
  List<DynkinType> precomputedTypes;
  DynkinType::GetPrecomputedDynkinTypes(precomputedTypes);
  for (int i = 0; i < precomputedTypes.size; i ++) {
    outFromHD << theCommands.ToStringSemismipleLieAlgebraLinksFromHD(precomputedTypes[i]);
    if (precomputedTypes[i].HasPrecomputedSubalgebras()) {
      std::stringstream recomputeCommand;
      recomputeCommand << "PrintSemisimpleSubalgebrasRecompute{}(" << precomputedTypes[i].ToString() << ")";
      outRecomputeLinks << "<br>" << HtmlRoutines::GetCalculatorComputationAnchor(recomputeCommand.str());
    }
  }
  outFromHD << "</table></p>";
  std::string fileName = "semisimple_lie_algebra_structure.html";
  std::stringstream out;
  out
  << theCommands.WriteFileToOutputFolderReturnLink(outFromHD.str(), fileName, "Links file");
  out << "<br>Recompute links.";
  out << outRecomputeLinks.str();
  out << outFromHD.str();
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerPrintSSsubalgebrasNilradicals(Calculator& theCommands, const Expression& input, Expression& output) {
  return theCommands.innerPrintSSSubalgebras(theCommands, input, output, true, true, true, true, true, true);
}

bool Calculator::innerPrintSSsubalgebrasRecompute(Calculator& theCommands, const Expression& input, Expression& output) {
  return theCommands.innerPrintSSSubalgebras(theCommands, input, output, true, true, false, true, false, true);
}

bool Calculator::innerPrintSSsubalgebrasNoSolutions(Calculator& theCommands, const Expression& input, Expression& output) {
  return theCommands.innerPrintSSSubalgebras(theCommands, input, output, true, false, false, false, false, false);
}

bool Calculator::innerPrintSSsubalgebrasNoCentralizers(Calculator& theCommands, const Expression& input, Expression& output) {
  return theCommands.innerPrintSSSubalgebras(theCommands, input, output, true, true, false, true, false, false);
}

bool Calculator::innerPrintSSsubalgebrasRegular(Calculator& theCommands, const Expression& input, Expression& output) {
  return theCommands.innerPrintSSSubalgebras(theCommands, input, output, false, true, false, true, false, true);
}

bool Calculator::innerPrintSSSubalgebras(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  bool doForceRecompute,
  bool doAttemptToSolveSystems,
  bool doComputePairingTable,
  bool doComputeModuleDecomposition,
  bool doComputeNilradicals,
  bool doAdjustCentralizers
) {
  MacroRegisterFunctionWithName("Calculator::innerPrintSSsubalgebras");
  if (doForceRecompute) {
    if (!global.UserDefaultHasAdminRights()) {
      return theCommands << "Only logged-in admins allowed to force-recompute semisimple subalgebras. ";
    }
  }
  if (global.theResponse.MonitoringAllowed()) {
    global.theResponse.Initiate("Triggered by printSemisimpleSubalgebras.");
  }
  if (input.size() != 2) {
    return theCommands << "Semisimple Lie algebra expects a single argument. ";
  }
  std::stringstream out;
  WithContext<SemisimpleLieAlgebra*> ownerAlgebra;
  SemisimpleLieAlgebra* ownerSSPointer = nullptr;
  bool isAlreadySubalgebrasObject = input[1].IsOfType<SemisimpleSubalgebras>();
  if (!isAlreadySubalgebrasObject) {
    if (!theCommands.Convert(
      input[1],
      CalculatorConversions::functionSemisimpleLieAlgebra,
      ownerAlgebra
    )) {
      return output.MakeError("Error extracting Lie algebra.", theCommands);
    }
    ownerSSPointer = ownerAlgebra.content;
    if (ownerSSPointer->GetRank() > 8) {
      out << "<b>This code is completely experimental and has been set to run up to rank 6. "
      << "As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
      return output.AssignValue(out.str(), theCommands);
    } else {
      out << "<b>This code is completely experimental. "
      << "Use the following printouts on your own risk.</b><br>";
    }
  } else {
    ownerSSPointer = input[1].GetValue<SemisimpleSubalgebras>().owner;
  }
  if (ownerSSPointer == nullptr) {
    global.fatal << "Zero pointer to semisimple Lie algebra: this shouldn't happen. " << global.fatal;
  }
  SemisimpleLieAlgebra& ownerLieAlgebra = *ownerSSPointer;
  std::string dynkinString = ownerSSPointer->theWeyl.theDynkinType.ToString();
  global.RelativePhysicalNameOptionalProgressReport = "progress_subalgebras_" + dynkinString;
  global.RelativePhysicalNameOptionalResult = "result_subalgebras_" + dynkinString;
  SemisimpleSubalgebras& theSubalgebras =
  theCommands.theObjectContainer.GetSemisimpleSubalgebrasCreateIfNotPresent(ownerLieAlgebra.theWeyl.theDynkinType);
  theSubalgebras.ComputeStructureWriteFiles(
    ownerLieAlgebra,
    theCommands.theObjectContainer.theAlgebraicClosure,
    theCommands.theObjectContainer.semisimpleLieAlgebras,
    theCommands.theObjectContainer.theSltwoSAs,
    &out,
    doForceRecompute,
    !isAlreadySubalgebrasObject,
    doComputeNilradicals,
    doComputeModuleDecomposition,
    doAttemptToSolveSystems,
    doComputePairingTable,
    doAdjustCentralizers
  );
  return output.AssignValue(out.str(), theCommands);
}

bool SemisimpleSubalgebras::ComputeStructureWriteFiles(
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
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::ComputeStructureWriteFiles");
  this->ToStringExpressionString = CalculatorConversions::innerStringFromSemisimpleSubalgebras;
  this->owner = &newOwner;
  this->ComputeFolderNames(this->currentFormat);
  if (!FileOperations::FileExistsVirtual(this->VirtualNameMainFile1) || forceRecompute) {
    if (doFullInit) {
      this->millisecondsComputationStart = global.GetElapsedMilliseconds();
    }
    this->flagComputeNilradicals = computeNilradicals;
    this->flagComputeModuleDecomposition = computeModuleDecomposition;
    this->flagAttemptToSolveSystems = attemptToSolveSystems;
    this->flagComputePairingTable = computePairingTable;
    this->flagAttemptToAdjustCentralizers = adjustCentralizers;
    this->CheckFileWritePermissions();
    if (doFullInit) {
      this->FindTheSSSubalgebrasFromScratch(newOwner, ownerField, containerSubalgebras, containerSl2Subalgebras, nullptr);
    }
    this->WriteReportToFiles();
  } else {
    if (outputStream != nullptr) {
      *outputStream << "Files precomputed, serving from HD. ";
    }
  }
  if (outputStream != nullptr) {
    *outputStream << "<br>Output file: <a href = \""
    << this->DisplayNameMainFile1WithPath << "\" target = \"_blank\">"
    << this->DisplayNameMainFile1NoPath << "</a>";
  }
  return true;
}

bool MathRoutines::IsPrime(int theInt) {
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

bool Calculator::innerAttemptExtendingEtoHEFwithHinCartan(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerAttemptExtendingEtoHEFwithHinCartan");
  if (input.size() != 3) {
    return output.MakeError("Function takes 2 arguments - type and an element of the Lie algebra.", theCommands);
  }
  WithContext<SemisimpleLieAlgebra*> ownerAlgebra;
  if (!theCommands.Convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, ownerAlgebra
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  SemisimpleLieAlgebra* ownerSS = ownerAlgebra.content;
  ElementSemisimpleLieAlgebra<Rational> theErational;
  if (!CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs(theCommands, input[2], theErational, *ownerSS)) {
    return output.MakeError("Failed to extract element of semisimple Lie algebra. ", theCommands);
  }
  ElementSemisimpleLieAlgebra<AlgebraicNumber> theF, theH, theE;
  theE = theErational;
  std::stringstream out, logStream;
  bool success = ownerSS->AttemptExtendingEtoHEFwithHinCartan(theE, theH, theF, &logStream);
  if (success) {
    out << HtmlRoutines::GetMathSpanPure("F=" + theF.ToString() + ";")
    << "<br>" << HtmlRoutines::GetMathSpanPure("H=" + theH.ToString() + ";") << "<br>"
    << HtmlRoutines::GetMathSpanPure("E=" + theE.ToString() + ";")
    << "<br><br>The log stream of the computation follows. " << logStream.str();
  } else {
    out << "<br>Couldn't extend E to sl(2)-triple. The log stream follows. " << logStream.str();
  }
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerAdCommonEigenSpaces(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerAdCommonEigenSpaces");
  if (input.size() < 3) {
    return output.MakeError(
      "Function ad common eigenspaces needs at least 2 arguments - type and at least one element of the algebra.",
      theCommands
    );
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!theCommands.Convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, algebra
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  SemisimpleLieAlgebra* ownerSS = algebra.content;
  List<ElementSemisimpleLieAlgebra<Rational> > theOperators, outputElts;
  theOperators.Reserve(input.size() - 2);
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  for (int i = 2; i < input.size(); i ++) {
    if (!CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs(theCommands, input[i], tempElt, *ownerSS)) {
      return output.MakeError("Failed to extract element of semisimple Lie algebra. ", theCommands);
    }
    theOperators.AddOnTop(tempElt);
  }
  ownerSS->GetCommonCentralizer(theOperators, outputElts);
  std::stringstream out;
  out << "<br>EigenSpace basis (" << outputElts.size << " elements total):<br> (";
  for (int i = 0; i < outputElts.size; i ++) {
    ElementSemisimpleLieAlgebra<Rational>& currentElt = outputElts[i];
    out << currentElt.ToString();
    if (i != outputElts.size - 1) {
      out << ", ";
    }
  }
  out << ")";
  output.AssignValue(out.str(), theCommands);
  return true;
}

bool Calculator::innerGroebner(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  int order,
  bool useModZp
) {
  MacroRegisterFunctionWithName("Calculator::innerGroebner");
  Vector<Polynomial<Rational> > inputVector;
  Vector<Polynomial<ElementZmodP> > inputVectorZmodP;
  Expression theContext;
  if (input.size() < 3) {
    return output.MakeError("Function takes at least two arguments. ", theCommands);
  }
  const Expression& numComputationsE = input[1];
  Rational upperBound = 0;
  if (!numComputationsE.IsOfType(&upperBound)) {
    return output.MakeError("Failed to convert the first argument of the expression to rational number.", theCommands);
  }
  if (upperBound > 1000000) {
    return output.MakeError(
      "Error: your upper limit of polynomial operations exceeds 1000000, which is too large. "
      "You may use negative or zero number give no computation bound, but please don't. ",
      theCommands
    );
  }
  int upperBoundComputations = int(upperBound.GetDoubleValue());
  output.reset(theCommands);
  for (int i = 1; i < input.children.size; i ++) {
    output.children.AddOnTop(input.children[i]);
  }
  int theMod = 0;
  if (useModZp) {
    if (!output[1].IsSmallInteger(&theMod)) {
      return output.MakeError("Error: failed to extract modulo from the second argument. ", theCommands);
    }
    if (!MathRoutines::IsPrime(theMod)) {
      return output.MakeError("Error: modulus not prime. ", theCommands);
    }
  }
  if (!theCommands.GetVectorFromFunctionArguments<Polynomial<Rational> >(
    output,
    inputVector,
    &theContext,
    - 1,
    CalculatorConversions::functionPolynomial<Rational>
  )) {
    return output.MakeError("Failed to extract polynomial expressions", theCommands);
  }
  for (int i = 0; i < inputVector.size; i ++) {
    inputVector[i].ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
  }
  GroebnerBasisComputation<AlgebraicNumber> theGroebnerComputation;
  theContext.ContextGetFormatExpressions(theGroebnerComputation.theFormat);
  theContext.ContextGetFormatExpressions(global.theDefaultFormat.GetElement());
  if (useModZp) {
    ElementZmodP tempElt;
    tempElt.MakeMOne(static_cast<unsigned>(theMod));
    inputVectorZmodP.SetSize(inputVector.size);
    for (int i = 0; i < inputVector.size; i ++) {
      inputVectorZmodP[i].MakeZero();
      for (int j = 0; j < inputVector[i].size(); j ++) {
        tempElt = inputVector[i].coefficients[j];
        inputVectorZmodP[i].AddMonomial(inputVector[i][j], tempElt);
      }
    }
    GroebnerBasisComputation<ElementZmodP> theGroebnerComputationZmodP;
  }
  List<Polynomial<AlgebraicNumber> > outputGroebner, outputGroebner2;
  outputGroebner = inputVector;
  outputGroebner2 = inputVector;
  if (order == MonomialP::Order::gradedLexicographic) {
    theGroebnerComputation.thePolynomialOrder.theMonOrder.setComparison(
      MonomialP::greaterThan_totalDegree_leftLargerWins
    );
  } else if (order == MonomialP::Order::gradedReverseLexicographic) {
    theGroebnerComputation.thePolynomialOrder.theMonOrder.setComparison(
      MonomialP::greaterThan_totalDegree_rightSmallerWins
    );
  } else if (order == MonomialP::Order::lexicographicOpposite) {
    theGroebnerComputation.thePolynomialOrder.theMonOrder.setComparison(
      MonomialP::greaterThan_rightLargerWins
    );
  } else if (order == MonomialP::Order::lexicographic){
    theGroebnerComputation.thePolynomialOrder.theMonOrder.setComparison(
      MonomialP::greaterThan_leftLargerWins
    );
  } else {
    global.fatal << "Unexpected order value: " << order << global.fatal;
  }
  theGroebnerComputation.theFormat.monomialOrder = theGroebnerComputation.thePolynomialOrder.theMonOrder;
  theGroebnerComputation.MaxNumGBComputations = upperBoundComputations;
  bool success = theGroebnerComputation.TransformToReducedGroebnerBasis(outputGroebner);
  std::stringstream out;
  out << theGroebnerComputation.ToStringLetterOrder(false);
  out << "Letter/expression order: ";
  for (int i = 0; i < theContext.ContextGetNumContextVariables(); i ++) {
    out << theContext.ContextGetContextVariable(i).ToString();
    if (i != theContext.ContextGetNumContextVariables() - 1) {
      out << "&lt;";
    }
  }
  out << "<br>Starting basis (" << inputVector.size  << " elements): ";
  for (int i = 0; i < inputVector.size; i ++) {
    out << "<br>" << HtmlRoutines::GetMathSpanPure(inputVector[i].ToString(&theGroebnerComputation.theFormat));
  }
  if (success) {
    out << "<br>Minimal Groebner basis with " << outputGroebner.size << " elements, computed using algorithm 1, using "
    << theGroebnerComputation.NumberGBComputations << " polynomial operations. ";
    for (int i = 0; i < outputGroebner.size; i ++) {
      out << "<br> " << HtmlRoutines::GetMathSpanPure(outputGroebner[i].ToString(&theGroebnerComputation.theFormat));
    }
    out << "<br>Output in calculator-ready format: ";
    out << "<br>(";
    for (int i = 0; i < outputGroebner.size; i ++) {
      out << outputGroebner[i].ToString(&theGroebnerComputation.theFormat);
      if (i != outputGroebner.size - 1) {
        out << ", <br>";
      }
    }
    out << ")";
  } else {
    out << "<br>Minimal Groebner basis not computed: exceeded the user-given limit of "
    << upperBoundComputations << " polynomial operations. ";
    out << "<br>An intermediate non-Groebner basis containing total " << theGroebnerComputation.theBasiS.size
    << " basis elements: ";
    out << "<br>GroebnerLexUpperLimit{}(10000, <br>";
    for (int i = 0; i < theGroebnerComputation.theBasiS.size; i ++) {
      out << theGroebnerComputation.theBasiS[i].ToString(&theGroebnerComputation.theFormat);
      if (i != theGroebnerComputation.theBasiS.size - 1) {
        out << ", <br>";
      }
    }
    out << ");";
  }
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerDeterminantPolynomial(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerDeterminantPolynomial");
  if (input.size() != 2) {
    return false;
  }
  Matrix<Polynomial<Rational> > matPol;
  Expression theContext;
  if (!theCommands.functionGetMatrix(
    input[1],
    matPol,
    &theContext,
    - 1,
    CalculatorConversions::functionPolynomial<Rational>
  )) {
    return theCommands << "<hr>Failed to convert the input to "
    << "matrix of polynomials. ";
  }
  if (matPol.NumRows != matPol.NumCols) {
    return output.MakeError("<hr>Failed to compute determinant: matrix is non-square. ", theCommands);
  }
  if (matPol.NumRows > 8) {
    return theCommands << "<hr>Failed to compute determinant: "
    << "matrix is larger than 8 x 8, and your matrix had "
    << matPol.NumRows << " rows. Note that you can compute "
    << "determinant using the \\det function which "
    << "does Gaussian elimination "
    << "and will work for large rational matrices. "
    << "This function is meant to be used with honest "
    << "polynomial entries. ";
  }
  Polynomial<Rational> outputPoly;
  outputPoly.MakeDeterminantFromSquareMatrix(matPol);
  return output.AssignValueWithContext(outputPoly, theContext, theCommands);
}

bool Calculator::innerTranspose(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerTranspose");
  if (
    !input.IsSequenceNElementS() &&
    !input.IsMatrix() &&
    !input.StartsWithGivenOperation("Transpose")
  ) {
    return false;
  }
  Matrix<Expression> theMat;
  if (input.StartsWithGivenOperation("Transpose")) {
    theCommands.GetMatrixExpressionsFromArguments(input, theMat);
  } else {
    theCommands.GetMatrixExpressions(input, theMat);
  }
  // The commented code used to be here. I don't remember why I added it, perhaps there was a solid reason?
  // If the code is uncommented, then ((1,2),(3,5))^t will not be transposed according to expectation.
  // If the commented code needs to be restored, please document why.
  // if (input.IsSequenceNElementS())
  //   if (theMat.NumRows !=1)
  //     return false;
  theMat.Transpose();
  return output.AssignMatrixExpressions(theMat, theCommands, true, true);
}

void Plot::operator+=(const Plot& other) {
  MacroRegisterFunctionWithName("Plot::operator+=");
  if (other.priorityCanvasName > this->priorityCanvasName) {
    this->SetCanvasName(other.GetCanvasName());
  } else if (this->GetCanvasName() == "" && this->priorityCanvasName == other.priorityCanvasName) {
    this->SetCanvasName(other.GetCanvasName());
  }
  if (other.priorityCanvasName > this->priorityCanvasName) {
    this->priorityCanvasName = other.priorityCanvasName;
  }
  if (this->priorityCanvasName == 0) {
    this->SetCanvasName("");
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
    this->highBoundY = MathRoutines::Maximum(this->highBoundY, other.highBoundY);
    this->lowBoundY = MathRoutines::Minimum(this->lowBoundY,  other.lowBoundY);
    this->theUpperBoundAxes = MathRoutines::Maximum(this->theUpperBoundAxes, other.theUpperBoundAxes);
    this->theLowerBoundAxes = MathRoutines::Minimum(this->theLowerBoundAxes, other.theLowerBoundAxes);
  }
  this->thePlots.AddListOnTop(other.thePlots);
  if (other.priorityWindow > this->priorityWindow) {
    this->DesiredHtmlHeightInPixels = other.DesiredHtmlHeightInPixels;
    this->DesiredHtmlWidthInPixels = other.DesiredHtmlWidthInPixels;
  } else if (this->priorityWindow == other.priorityWindow) {
    if (this->DesiredHtmlHeightInPixels < other.DesiredHtmlHeightInPixels) {
      this->DesiredHtmlHeightInPixels = other.DesiredHtmlHeightInPixels;
    }
    if (this->DesiredHtmlWidthInPixels < other.DesiredHtmlWidthInPixels) {
      this->DesiredHtmlWidthInPixels = other.DesiredHtmlWidthInPixels;
    }
  }
  if (!other.flagIncludeCoordinateSystem) {
    this->flagIncludeCoordinateSystem = false;
  }
  this->boxesThatUpdateMe.AddOnTopNoRepetition(other.boxesThatUpdateMe);
  this->priorityWindow = MathRoutines::Maximum(this->priorityWindow, other.priorityWindow);
  this->priorityViewRectangle = MathRoutines::Maximum(this->priorityViewRectangle, other.priorityViewRectangle);
}

bool Plot::operator==(const Plot& other) const {
  return
  this->priorityWindow == other.priorityWindow &&
  this->priorityCanvasName == other.priorityCanvasName &&
  this->priorityViewRectangle == other.priorityViewRectangle &&
  this->DesiredHtmlHeightInPixels == other.DesiredHtmlHeightInPixels &&
  this->DesiredHtmlWidthInPixels == other.DesiredHtmlWidthInPixels &&
  ((this->highBoundY        - other.highBoundY       ) == 0.0) &&
  ((this->lowBoundY         - other.lowBoundY        ) == 0.0) &&
  ((this->theLowerBoundAxes - other.theLowerBoundAxes) == 0.0) &&
  ((this->theUpperBoundAxes - other.theUpperBoundAxes) == 0.0) &&
  this->thePlots == other.thePlots &&
  this->boxesThatUpdateMe == other.boxesThatUpdateMe &&
  this->GetCanvasName() == other.GetCanvasName() &&
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
  this->thePlots.AddOnTop(other);
  this->SetCanvasName("");
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

void PlotObject::ComputeYbounds() {
  MacroRegisterFunctionWithName("PlotObject::ComputeYbounds");
  for (int i = 0; i < this->thePointsDouble.size; i ++) {
    this->yHigh = MathRoutines::Maximum(this->yHigh, this->thePointsDouble[i][1]);
    this->yLow = MathRoutines::Minimum(this->yLow, this->thePointsDouble[i][1]);
  }
}

std::string PlotObject::GetPlotStringFromFunctionStringAndRanges(
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
  << FloatingPoint::DoubleToString(inputLowerBound) << "}{" << FloatingPoint::DoubleToString(inputUpperBound) << "}{";
  out << functionStringPostfixNotation << "}";
  return out.str();
}

Plot::Plot() {
  this->theLowerBoundAxes = - 0.5;
  this->theUpperBoundAxes = 1;
  this->lowBoundY = - 0.5;
  this->highBoundY = 0.5;
  this->flagIncludeExtraHtmlDescriptions = true;
  this->DesiredHtmlHeightInPixels = 350;
  this->DesiredHtmlWidthInPixels = 350;
  this->defaultLineColor = 0;
  this->flagPlotShowJavascriptOnly = false;
  this->priorityWindow = 0;
  this->priorityViewRectangle = 0;
  this->priorityCanvasName = 0;
  this->dimension = - 1;
  this->flagDivAlreadyDisplayed = false;
  this->flagIncludeCoordinateSystem = true;
}

void Plot::ComputeAxesAndBoundingBox() {
  MacroRegisterFunctionWithName("Plot::ComputeAxesAndBoundingBox");
  this->theLowerBoundAxes = - 0.5;
  this->theUpperBoundAxes = 1.1;
  this->lowBoundY = - 0.5;
  this->highBoundY = 1.1;
  for (int k = 0; k < this->thePlots.size; k ++) {
    this->thePlots[k].ComputeYbounds();
    this->theLowerBoundAxes = MathRoutines::Minimum(this->thePlots[k].xLow, theLowerBoundAxes);
    this->theUpperBoundAxes = MathRoutines::Maximum(this->thePlots[k].xHigh, theUpperBoundAxes);
    this->lowBoundY = MathRoutines::Minimum(this->thePlots[k].yLow, this->lowBoundY);
    this->highBoundY = MathRoutines::Maximum(this->thePlots[k].yHigh, this->highBoundY);
/*    for (int j = 0; j < this->thePlots[k].theLines.size; j ++) {
      List<Vector<double> > currentLine = this->thePlots[k].theLines[j];
      this->theLowerBoundAxes =MathRoutines::Minimum(this->theLowerBoundAxes, currentLine[0][0]);
      this->theLowerBoundAxes =MathRoutines::Minimum(this->theLowerBoundAxes, currentLine[1][0]);
      this->theUpperBoundAxes =MathRoutines::Maximum(this->theUpperBoundAxes, currentLine[0][0]);
      this->theUpperBoundAxes =MathRoutines::Maximum(this->theUpperBoundAxes, currentLine[1][0]);
      this->lowBoundY =MathRoutines::Minimum  (currentLine[0][1], this->lowBoundY);
      this->lowBoundY =MathRoutines::Minimum  (currentLine[1][1], this->lowBoundY);
      this->highBoundY =MathRoutines::Maximum (currentLine[0][1], this->highBoundY);
      this->highBoundY =MathRoutines::Maximum (currentLine[1][1], this->highBoundY);
    }*/
    for (int j = 0; j < this->thePlots[k].thePointsDouble.size; j ++) {
      Vector<double>& currentPoint = this->thePlots[k].thePointsDouble[j];
      if (!this->IsOKVector(currentPoint)) {
        continue;
      }
      this->theLowerBoundAxes = MathRoutines::Minimum(this->theLowerBoundAxes, currentPoint[0]);
      this->theUpperBoundAxes = MathRoutines::Maximum(this->theUpperBoundAxes, currentPoint[0]);
      this->lowBoundY = MathRoutines::Minimum (currentPoint[1], this->lowBoundY);
      this->highBoundY = MathRoutines::Maximum (currentPoint[1], this->highBoundY);
    }
  }
}

void Plot::ComputeAxesAndBoundingBox3d() {
  MacroRegisterFunctionWithName("Plot::ComputeAxesAndBoundingBox3d");
  this->theLowerBoundAxes = - 0.5;
  this->theUpperBoundAxes = 1.1;
  this->lowBoundY = - 0.5;
  this->highBoundY = 1.1;
  for (int k = 0; k < this->thePlots.size; k ++) {
    this->thePlots[k].ComputeYbounds();
    this->theLowerBoundAxes = MathRoutines::Minimum(this->thePlots[k].xLow, theLowerBoundAxes);
    this->theUpperBoundAxes = MathRoutines::Maximum(this->thePlots[k].xHigh, theUpperBoundAxes);
    this->lowBoundY = MathRoutines::Minimum(this->thePlots[k].yLow, this->lowBoundY);
    this->highBoundY = MathRoutines::Maximum(this->thePlots[k].yHigh, this->highBoundY);
/*    for (int j = 0; j < this->thePlots[k].theLines.size; j ++) {
      List<Vector<double> > currentLine = this->thePlots[k].theLines[j];
      this->theLowerBoundAxes =MathRoutines::Minimum(this->theLowerBoundAxes, currentLine[0][0]);
      this->theLowerBoundAxes =MathRoutines::Minimum(this->theLowerBoundAxes, currentLine[1][0]);
      this->theUpperBoundAxes =MathRoutines::Maximum(this->theUpperBoundAxes, currentLine[0][0]);
      this->theUpperBoundAxes =MathRoutines::Maximum(this->theUpperBoundAxes, currentLine[1][0]);
      this->lowBoundY =MathRoutines::Minimum  (currentLine[0][1], this->lowBoundY);
      this->lowBoundY =MathRoutines::Minimum  (currentLine[1][1], this->lowBoundY);
      this->highBoundY =MathRoutines::Maximum (currentLine[0][1], this->highBoundY);
      this->highBoundY =MathRoutines::Maximum (currentLine[1][1], this->highBoundY);
    }*/
    for (int j = 0; j < this->thePlots[k].thePointsDouble.size; j ++) {
      Vector<double>& currentPoint = this->thePlots[k].thePointsDouble[j];
      if (!this->IsOKVector(currentPoint)) {
        continue;
      }
      this->theLowerBoundAxes = MathRoutines::Minimum(this->theLowerBoundAxes, currentPoint[0]);
      this->theUpperBoundAxes = MathRoutines::Maximum(this->theUpperBoundAxes, currentPoint[0]);
      this->lowBoundY = MathRoutines::Minimum(currentPoint[1], this->lowBoundY);
      this->highBoundY = MathRoutines::Maximum(currentPoint[1], this->highBoundY);
    }
  }

}

bool Plot::IsOKVector(const Vector<double>& input) {
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
  << "if (document.getElementById('" << this->GetCanvasName() << "') === null) {\n"
  << "  return;\n"
  << "}\n"
  << "theDrawer.deleteCanvas('" << this->GetCanvasName() << "');\n";
  return out.str();
}

std::string Plot::GetPlotHtml3d_New(Calculator& owner) {
  MacroRegisterFunctionWithName("Plot::GetPlotHtml3d_New");
  owner.flagHasGraphics = true;
  std::stringstream outContent, outScript;
  this->ComputeCanvasNameIfNecessary(owner.theObjectContainer.canvasPlotCounter);
  if (!owner.flagPlotShowJavascriptOnly) {
    outContent << "<canvas width =\"" << this->DesiredHtmlWidthInPixels
    << "\" height =\"" << this->DesiredHtmlHeightInPixels << "\" "
    << "style =\"border:solid 1px\" id =\""
    << this->GetCanvasName()
    << "\" "
    << ">"
    << "Your browser does not support the HTML5 canvas tag.</canvas><br>"
    << "<span id =\"" << this->GetCanvasName() << "Controls\"></span>"
    << "<span id =\"" << this->GetCanvasName() << "Messages\"></span>";
  }
  outContent << "<script language =\"javascript\">\n";
  std::string canvasFunctionName = "functionMake" + this->GetCanvasName();
  std::string canvasResetFunctionName = "functionReset" + this->GetCanvasName();
  outScript
  << "function " << canvasResetFunctionName << "() {\n"
  << canvasFunctionName << "();\n"
  << "}\n";
  outScript << "function " << canvasFunctionName << "() {\n";
  for (int i = 0; i < this->boxesThatUpdateMe.size; i ++) {
    InputBox& currentBox = owner.theObjectContainer.
    theUserInputTextBoxesWithValues.GetValueCreate(this->boxesThatUpdateMe[i]);
    outScript << " window.calculator.drawing.plotUpdaters['"
    << currentBox.GetSliderName() << "'] =" << "'" << this->GetCanvasName() << "'"
    << ";\n";
  }
  List<std::string> the3dObjects;
  the3dObjects.SetSize(this->thePlots.size);
  int funCounter = 0;
  for (int i = 0; i < this->thePlots.size; i ++) {
    PlotObject& currentO = this->thePlots[i];
    if (currentO.thePlotType == "surface") {
      outScript << currentO.GetJavascriptSurfaceImmersion(the3dObjects[i], this->GetCanvasName(), funCounter) << "\n ";
    }
    if (currentO.thePlotType == "parametricCurve") {
      outScript << currentO.GetJavascriptCurveImmersionIn3d(the3dObjects[i], this->GetCanvasName(), funCounter) << "\n ";
    }
  }
  outScript << this->commonCanvasSetup();
  outScript << "var theCanvas = theDrawer.getCanvas('" << this->GetCanvasName() << "');\n"
  << "theCanvas.initialize('" << this->GetCanvasName() << "');\n";
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
      << currentPlot.thePointsDouble[0].ToStringSquareBracketsBasicType()
      << ","
      << currentPlot.thePointsDouble[1].ToStringSquareBracketsBasicType()
      << "];\n";
      outScript << "theCanvas.screenBasisUser = theCanvas.screenBasisUserDefault.slice();\n";
    }
    if (currentPlot.thePlotType == "label") {
      outScript
      << "theCanvas.drawText({"
      << "location: "
      << currentPlot.thePointsDouble[0].ToStringSquareBracketsBasicType()
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
      << currentPlot.thePointsDouble[0].ToStringSquareBracketsBasicType()
      << ", "
      << currentPlot.thePointsDouble[1].ToStringSquareBracketsBasicType()
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
  owner.theObjectContainer.graphicsScripts.SetKeyValue(this->GetCanvasName(), outScript.str());
  return outContent.str();
}

void Plot::SetCanvasName(const std::string& input) {
  this->canvasNamE = StringRoutines::ConvertStringToJavascriptVariable(input);
}

std::string Plot::GetCanvasName() const {
  return this->canvasNamE;
}

std::string Plot::ToStringDebug() {
  std::stringstream out;
  out <<  "Objects: " << this->thePlots.size << "<br>";
  for (int i = 0; i < this->thePlots.size; i ++) {
    if (this->thePlots[i].thePlotType == "surface") {
      PlotObject& theSurface = this->thePlots[i];
      out << theSurface.ToStringDebug();
    }
  }
  return out.str();
}

std::string PlotObject::ToStringDebug() {
  MacroRegisterFunctionWithName("PlotSurfaceIn3d::ToStringDebug");
  std::stringstream out;
  out << "colorUV: " << this->colorUV << "<br>";
  out << "colorVU: " << this->colorVU << "<br>";
  out << "Coord f-ns: " << this->coordinateFunctionsE.ToStringCommaDelimited() << "<br>";
  out << "Vars: " << this->variablesInPlay << "<br>";
  out << "Var ranges: " << this->theVarRangesJS << "<br>";
  return out.str();
}

std::string PlotObject::GetJavascriptCurveImmersionIn3d(
  std::string& outputCurveInstantiationJS, const std::string& canvasName, int& funCounter
) {
  MacroRegisterFunctionWithName("PlotSurfaceIn3d::GetJavascriptCurveImmersionIn3d");
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

std::string PlotObject::GetJavascriptSurfaceImmersion(
  std::string& outputSurfaceInstantiationJS, const std::string& canvasName, int& funCounter
) {
  MacroRegisterFunctionWithName("PlotSurfaceIn3d::GetJavascriptSurfaceImmersion");
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
    out << "//this->theVarRangesJS has " << this->theVarRangesJS.size << " elements instead of 2 (expected).";
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

std::string Plot::GetPlotHtml(Calculator& owner) {
  MacroRegisterFunctionWithName("Plot::GetPlotHtml");
  if (this->dimension == 3) {
    return this->GetPlotHtml3d_New(owner);
  } else if (this->dimension == 2) {
    return this->GetPlotHtml2d_New(owner);
  } else {
    std::stringstream out;
    out << "Error:dimension =" << this->dimension;
    return out.str();
  }
}

std::string PlotObject::GetJavascriptParametricCurve2D(
  std::string& outputPlotInstantiationJS, const std::string& canvasName, int& funCounter
) {
  MacroRegisterFunctionWithName("PlotSurfaceIn3d::GetJavascript2dPlot");
  std::stringstream out;
  List<std::string> fnNames;
  fnNames.SetSize(this->coordinateFunctionsJS.size);
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

std::string PlotObject::GetJavascriptDirectionField(
  std::string& outputPlotInstantiationJS, const std::string& canvasName, int& funCounter
) {
  MacroRegisterFunctionWithName("PlotSurfaceIn3d::GetJavascriptDirectionField");
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

std::string PlotObject::GetJavascript2dPlot(
  std::string& outputPlotInstantiationJS, const std::string& canvasName, int& funCounter
) {
  MacroRegisterFunctionWithName("PlotSurfaceIn3d::GetJavascript2dPlot");
  std::stringstream out;
  std::stringstream fnNameStream;
  funCounter ++;
  fnNameStream << "theCanvasPlotFn_" << canvasName << "_"
  << funCounter;
  std::string fnName = fnNameStream.str();
  if (this->variablesInPlayJS.size > 0) {
    out << "function " << fnName
    << " (" << HtmlRoutines::GetJavascriptVariable(this->variablesInPlayJS[0]) << "){\n";
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

std::string PlotObject::GetJavascriptPoints(
  std::string& outputPlotInstantiationJS, const std::string& canvasName, int& funCounter
) {
  MacroRegisterFunctionWithName("PlotSurfaceIn3d::GetJavascriptPoints");
  (void) (canvasName);
  (void) funCounter;
  std::stringstream fnInstStream;
  fnInstStream << "drawPoints([";
  for (int i = 0; i < this->thePointsJS.NumRows; i ++) {
    fnInstStream << "[";
    for (int j = 0; j < this->thePointsJS.NumCols; j ++) {
      fnInstStream << this->thePointsJS(i, j);
      if (j != this->thePointsJS.NumCols - 1) {
        fnInstStream << ", ";
      }
    }
    fnInstStream << "]";
    if (i != this->thePointsJS.NumRows - 1) {
      fnInstStream << ", ";
    }
  }
  fnInstStream << "], ";
  fnInstStream << "'" << this->colorJS << "');\n";
  outputPlotInstantiationJS = fnInstStream.str();
  return "";
}

std::string PlotObject::ToStringPointsList() {
  MacroRegisterFunctionWithName("PlotObject::ToStringPointsList");
  std::stringstream out;
  out << "[";
  for (int j = 0; j < this->thePointsDouble.size; j ++) {
    out << this->thePointsDouble[j].ToStringSquareBracketsBasicType();
    if (j != this->thePointsDouble.size - 1) {
      out << ",";
    }
  }
  out << "]";

  return out.str();
}

void Plot::ComputeCanvasNameIfNecessary(int& canvasCounter) {
  if (this->GetCanvasName() != "") {
    return;
  }
  canvasCounter ++;
  std::stringstream canvasNameStream;
  canvasNameStream << "theCanvas" << canvasCounter;
  this->SetCanvasName(canvasNameStream.str());
}

std::string Plot::GetPlotHtml2d_New(Calculator& owner) {
  MacroRegisterFunctionWithName("Plot::GetPlotHtml2d_New");
  owner.flagHasGraphics = true;
  if (this->flagDivAlreadyDisplayed) {
    return "[plot alredy displayed]";
  }
  this->flagDivAlreadyDisplayed = true;
  this->ComputeCanvasNameIfNecessary(owner.theObjectContainer.canvasPlotCounter);
  std::stringstream out;
  if (this->priorityViewRectangle <= 0) {
    this->ComputeAxesAndBoundingBox();
  }
  if (!this->flagPlotShowJavascriptOnly) {
    out << "<canvas width =\"" << this->DesiredHtmlWidthInPixels
    << "\" height =\"" << this->DesiredHtmlHeightInPixels << "\" "
    << "style =\"border:solid 1px\" id =\""
    << this->GetCanvasName()
    << "\" "
    << ">"
    << "Your browser does not support the HTML5 canvas tag.</canvas><br>"
    << "<span id =\"" << this->GetCanvasName() << "Controls\"></span>";
    if (!owner.flagPlotNoControls) {
      out << "<br>";
    }
    out << "<span id =\"" << this->GetCanvasName() << "Messages\"></span>";
  }
  std::stringstream outScript;
  std::string canvasFunctionName = "functionMake" + this->GetCanvasName();
  std::string canvasResetFunctionName = "functionReset" + this->GetCanvasName();
  outScript
  << "function " << canvasResetFunctionName << "() {\n"
  << canvasFunctionName << "();\n"
  << "}\n";

  outScript << "function " << canvasFunctionName << "() {\n";
  for (int i = 0; i < this->boxesThatUpdateMe.size; i ++) {
    InputBox& currentBox = owner.theObjectContainer.theUserInputTextBoxesWithValues.GetValueCreate(this->boxesThatUpdateMe[i]);
    outScript << "  window.calculator.drawing.plotUpdaters['"
    << currentBox.GetSliderName() << "'] ="
    << "'" << this->GetCanvasName() << "'"
    << ";\n";
  }
  int funCounter = 0;
  List<std::string> theFnPlots;
  theFnPlots.SetSize(this->thePlots.size);
  for (int i = 0; i < this->thePlots.size; i ++) {
    PlotObject& currentPlot = this->thePlots[i];
    if (currentPlot.thePlotType == "plotFunction") {
      outScript << currentPlot.GetJavascript2dPlot(theFnPlots[i], this->GetCanvasName(), funCounter) << "\n ";
    }
    if (currentPlot.thePlotType == "parametricCurve") {
      outScript << currentPlot.GetJavascriptParametricCurve2D(theFnPlots[i], this->GetCanvasName(), funCounter) << "\n ";
    }
    if (currentPlot.thePlotType == "plotDirectionField") {
      outScript << currentPlot.GetJavascriptDirectionField(theFnPlots[i], this->GetCanvasName(), funCounter) << "\n ";
    }
    if (currentPlot.thePlotType == "points") {
      currentPlot.GetJavascriptPoints(theFnPlots[i], this->GetCanvasName(), funCounter);
    }
  }
  outScript << this->commonCanvasSetup();
  outScript << "var theCanvas = theDrawer.getCanvasTwoD('" << this->GetCanvasName() << "');\n"
  << "theCanvas.initialize('" << this->GetCanvasName() << "');\n";
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
        << currentPlot.thePointsDouble[0].ToStringSquareBracketsBasicType()
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
      outScript << currentPlot.ToStringPointsList();
      outScript << ", "
      << "\"" << currentPlot.colorJS << "\""
      << ","
      << "\"" << currentPlot.colorFillJS << "\""
      << ");\n";
      continue;
    }
    outScript << "theCanvas.drawPath( ";
    outScript << currentPlot.ToStringPointsList();
    outScript << ", " << "\"";
    if (currentPlot.colorJS == "") {
      outScript << DrawingVariables::GetColorHtmlFromColorIndex (currentPlot.colorRGB);
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
  << " var currentCanvas = window.calculator.drawing.getCanvasTwoDNullOnFailure('" << this->GetCanvasName() << "');\n"
  << "if (currentCanvas !== null) {\n"
  << "currentCanvas.canvasResetFunction = "
  << canvasFunctionName << ";\n"
  << canvasFunctionName << "();\n"
  << "}\n";
  owner.theObjectContainer.graphicsScripts.SetKeyValue(this->GetCanvasName(), outScript.str());
  out << "<script language =\"javascript\">\n" << outScript.str() << "</script>";
  return out.str();
}

std::string Plot::GetPlotStringAddLatexCommands(bool useHtml) {
  MacroRegisterFunctionWithName("Plot::GetPlotStringAddLatexCommands");
  std::stringstream resultStream;
  this->ComputeAxesAndBoundingBox();
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
  resultStream << "\\begin{pspicture}(" << FloatingPoint::DoubleToString(theLowerBoundAxes - 0.4) << ", "
  << FloatingPoint::DoubleToString(lowBoundY - 0.4) << ")("
  << FloatingPoint::DoubleToString(theUpperBoundAxes + 0.4)
  << "," << FloatingPoint::DoubleToString(highBoundY + 0.5) << ")\n\n";
  resultStream << lineSeparator << "\\tiny\n" << lineSeparator;
  resultStream << " \\fcAxesStandard{" << FloatingPoint::DoubleToString(theLowerBoundAxes - 0.15)
  << "}{" << FloatingPoint::DoubleToString(lowBoundY - 0.15) << "}{"
  << FloatingPoint::DoubleToString(theUpperBoundAxes + 0.15) << "}{"
  << FloatingPoint::DoubleToString(highBoundY + 0.15) << "}" << lineSeparator;
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

bool Expression::AssignStringParsed(
  const std::string& theString, MapList<std::string, Expression, MathRoutines::HashString>* substitutions, Calculator& owner
) {
  MacroRegisterFunctionWithName("Expression::AssignStringParsed");
  Expression commands, result;
  List<SyntacticElement> outputSyntacticSoup, outputSyntacticStack;
  std::string outputSyntacticErrors;
  if (!owner.ParseAndExtractExpressions(theString, commands, outputSyntacticSoup, outputSyntacticStack, &outputSyntacticErrors)) {
    this->AssignValue(outputSyntacticErrors, owner);
    return false;
  }
  if (commands.StartsWith(owner.opEndStatement(), 2)) {
    result = commands[1];
  } else {
    result = commands;
  }
  if (substitutions != nullptr) {
    MapList<Expression, Expression> theSubs;
    for (int i = 0; i < substitutions->size(); i ++) {
      Expression theSubbed;
      theSubbed.MakeAtom(substitutions->theKeys[i], owner);
      theSubs.SetKeyValue(theSubbed, substitutions->theValues[i]);
    }
    result.SubstituteRecursively(theSubs);
  }
  *this = result;
  return true;
}

bool Expression::IsSuitableForSubstitution() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->IsBuiltInTypE() || this->StartsWith(this->owner->opBind())) {
    return false;
  }
  return true;
}

bool Expression::IsSuitableForRecursion() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->IsAtom() || this->IsBuiltInTypE() || this->StartsWith(this->owner->opBind())) {
    return false;
  }
  return true;
}

void Expression::SubstituteRecursively(MapList<Expression, Expression>& theSubs) {
  if (theSubs.Contains(*this)) {
    (*this) = theSubs.GetValueCreate(*this);
    return;
  }
  this->SubstituteRecursivelyInChildren(theSubs);
}

void Expression::SubstituteRecursivelyInChildren(MapList<Expression, Expression>& theSubs) {
  if (!this->IsSuitableForSubstitution()) {
    return;
  }
  Expression tempE;
  for (int i = 0; i < this->size(); i ++) {
    if (theSubs.Contains((*this)[i])) {
      this->SetChilD(i, theSubs.GetValueCreate((*this)[i]));
    } else {
      tempE = (*this)[i];
      tempE.SubstituteRecursivelyInChildren(theSubs);
      if (!(tempE == (*this)[i])) {
        this->SetChilD(i, tempE);
      }
    }
  }
}

void Expression::SubstituteRecursively(const Expression& toBeSubbed, const Expression& toBeSubbedWith) {
  if ((*this) == toBeSubbed) {
    (*this) = toBeSubbedWith;
    return;
  }
  this->SubstituteRecursivelyInChildren(toBeSubbed, toBeSubbedWith);
}

void Expression::SubstituteRecursivelyInChildren(const Expression& toBeSubbed, const Expression& toBeSubbedWith) {
  if (!this->IsSuitableForSubstitution()) {
    return;
  }
  Expression tempE;
  for (int i = 0; i < this->size(); i ++) {
    if (toBeSubbed == (*this)[i]) {
      this->SetChilD(i, toBeSubbedWith);
    } else {
      tempE = (*this)[i];
      tempE.SubstituteRecursivelyInChildren(toBeSubbed, toBeSubbedWith);
      if (!(tempE == (*this)[i])) {
        this->SetChilD(i, tempE);
      }
    }
  }
}

bool Calculator::innerSuffixNotationForPostScript(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerSuffixNotationForPostScript");
  RecursionDepthCounter theCounter(&theCommands.RecursionDeptH);
  if (*theCounter.theCounter == theCommands.MaxRecursionDeptH - 2) {
    return output.AssignValue(std::string("..."), theCommands);
  }
  std::string currentString;
  if (input.IsOperation(&currentString)) {
    if (input.ToString() == "e") {
      return output.AssignValue<std::string>(" 2.718281828 ", theCommands);
    }
    if (input.ToString() == "\\pi") {
      return output.AssignValue<std::string>(" 3.141592654 ", theCommands);
    }
    if (input.theData >= theCommands.NumPredefinedAtoms) {
      return output.AssignValue(currentString, theCommands);
    }
    if (currentString == "|") {
      return output.AssignValue<std::string>("abs ", theCommands);
    }
    if (currentString == "+") {
      return output.AssignValue<std::string>("add ", theCommands);
    }
    if (currentString == "*") {
      return output.AssignValue<std::string>("mul ", theCommands);
    }
    if (currentString == "-") {
      return output.AssignValue<std::string>("sub ", theCommands);
    }
    if (currentString == "/") {
      return output.AssignValue<std::string>("div ", theCommands);
    }
    if (currentString == "^") {
      return output.AssignValue<std::string>("exp ", theCommands);
    }
    if (currentString == "\\log") {
      return output.AssignValue<std::string>("ln ", theCommands);
    }
    if (currentString == "\\sin") {
      return output.AssignValue<std::string>(" 57.29578 mul sin ", theCommands);
    }
    if (currentString == "\\cos") {
      return output.AssignValue<std::string>(" 57.29578 mul cos ", theCommands);
    }
    if (currentString == "\\tan") {
      return output.AssignValue<std::string>(" 57.29578 mul tan ", theCommands);
    }
    if (currentString == "\\arctan") {
      return output.AssignValue<std::string>("ATAN ", theCommands);
    }
    if (currentString == "\\arcsin") {
      return output.AssignValue<std::string>("ASIN ", theCommands);
    }
    if (currentString == "\\arccos") {
      return output.AssignValue<std::string>("ACOS ", theCommands);
    }
    if (currentString == "\\sqrt") {
      return output.AssignValue<std::string>("sqrt ", theCommands);
    }
    return output.MakeError("Cannot convert " + currentString + " to suffix notation.", theCommands);
  }
  std::stringstream out;
  out.precision(7);
  bool hasDoubleValue = false;
  double theDoubleValue = - 1;
  Rational theRat;
  if (input.IsOfType<Rational>(&theRat)) {
    if (
      theRat.GetDenominator().IsIntegerFittingInInt(nullptr) &&
      theRat.GetNumerator().IsIntegerFittingInInt(nullptr)
    ) {
      out << " " << theRat.GetNumerator().ToString() << " " << theRat.GetDenominator() << " div ";
      return output.AssignValue(out.str(), theCommands);
    }
    hasDoubleValue = true;
    theDoubleValue = input.GetValue<Rational>().GetDoubleValue();
  }
  if (input.IsOfType<AlgebraicNumber>()) {
    hasDoubleValue = input.GetValue<AlgebraicNumber>().EvaluatesToDouble(&theDoubleValue);
  }
  if (input.IsOfType<double>()) {
    hasDoubleValue = true;
    theDoubleValue = input.GetValue<double>();
  }
  if (hasDoubleValue) {
    out << " " << FloatingPoint::DoubleToString(theDoubleValue);
    return output.AssignValue(out.str(), theCommands);
  }
  Expression currentE;
  bool useUsualOrder =
    !input[0].IsOperationGiven(theCommands.opDivide()) &&
    !input[0].IsOperationGiven(theCommands.opThePower());
  if (useUsualOrder) {
    for (int i = input.size() - 1; i >= 1; i --) {
      if (!theCommands.innerSuffixNotationForPostScript(theCommands, input[i], currentE)) {
        return output.MakeError("Failed to convert " + input[i].ToString(), theCommands);
      }
      if (!currentE.IsOfType(&currentString)) {
        return output.MakeError("Failed to convert " + input[i].ToString(), theCommands);
      }
      out << currentString << " ";
    }
  } else {
    for (int i = 1; i < input.size(); i ++) {
      if (!theCommands.innerSuffixNotationForPostScript(theCommands, input[i], currentE)) {
        return output.MakeError("Failed to convert " + input[i].ToString(), theCommands);
      }
      if (!currentE.IsOfType(&currentString)) {
        return output.MakeError("Failed to convert " + input[i].ToString(), theCommands);
      }
      out << currentString << " ";
    }
  }
  if (!theCommands.innerSuffixNotationForPostScript(theCommands, input[0], currentE)) {
    return output.MakeError("Failed to convert " + input[0].ToString(), theCommands);
  }
  if (!currentE.IsOfType(&currentString)) {
    return output.MakeError("Failed to convert " + input[0].ToString(), theCommands);
  }
  out << currentString << " ";
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerCharacterSSLieAlgFD(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerCharacterSSLieAlgFD");
  Vector<Rational> theHW;
  Selection parSel;
  WithContext<SemisimpleLieAlgebra*> ownerSSLiealg;
  Expression tempE, tempE2;
  if (!theCommands.GetTypeHighestWeightParabolic(
    theCommands, input, output, theHW, parSel, ownerSSLiealg, nullptr
  )) {
    return false;
  }
  if (output.IsError()) {
    return true;
  }
  if (parSel.CardinalitySelection != 0) {
    return output.MakeError("I know only to compute with finite dimensional characters, for the time being.", theCommands);
  }
  charSSAlgMod<Rational> theElt;
  theElt.MakeFromWeight(ownerSSLiealg.content->theWeyl.GetSimpleCoordinatesFromFundamental(theHW), ownerSSLiealg.content);
  return output.AssignValue(theElt, theCommands);
}

bool Calculator::innerConesIntersect(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerConesIntersect");
  if (!input.IsListNElements(3)) {
    return theCommands << "Function ConesIntersection expects 2 arguments, got " << input.children.size - 1 << " instead. ";
  }
  Matrix<Rational> coneNonStrictMatForm;
  Matrix<Rational> coneStrictMatForm;
  Vectors<Rational> coneNonStrictGens;
  Vectors<Rational> coneStrictGens;
  if (!theCommands.functionGetMatrix(input[1], coneStrictMatForm)) {
    return theCommands << "Failed to extract matrix from the first argument, " << input[1].ToString();
  }
  if (!theCommands.functionGetMatrix(input[2], coneNonStrictMatForm)) {
    return theCommands << "Failed to extract matrix from the second argument, " << input[2].ToString();
  }
  std::stringstream out;
  if (coneNonStrictMatForm.NumCols != coneStrictMatForm.NumCols) {
    out << "I got as input vectors of different dimensions, first groups had vectors of dimension " << coneNonStrictMatForm.NumCols
    << " and second of dimension " << coneStrictMatForm.NumCols << " which is not allowed. ";
    return output.MakeError(out.str(), theCommands);
  }
  coneNonStrictMatForm.GetVectorsFromRows(coneNonStrictGens);
  coneStrictMatForm.GetVectorsFromRows(coneStrictGens);
  out << "<br>Input non-strict (i.e., over Z_{&gt;= 0}) cone: ";
  for (int i = 0; i < coneStrictGens.size; i ++) {
    out << "<br>v_{" << i + 1 << "}=" << coneStrictGens[i].ToString() << ";";
  }
  out << "<br>Input strict (i.e., over Z_{&gt;0}) cone: ";
  for (int i = 0; i < coneNonStrictGens.size; i ++) {
    out << "<br>v_{" << coneStrictGens.size + i + 1 << "}=" << coneNonStrictGens[i].ToString() << ";";
  }
  Vector<Rational> outputIntersection, outputSeparatingNormal;
  bool conesDoIntersect = coneNonStrictGens.ConesIntersect(coneStrictGens, coneNonStrictGens, &outputIntersection, &outputSeparatingNormal);
  if (conesDoIntersect) {
    Vector<Rational> checkVector;
    checkVector.MakeZero(coneStrictMatForm.NumCols);
    for (int i = 0; i < coneStrictGens.size; i ++) {
      checkVector += coneStrictGens[i] * outputIntersection[i];
    }
    for (int i = 0; i < coneNonStrictGens.size; i ++) {
      checkVector += coneNonStrictGens[i] * outputIntersection[coneStrictGens.size + i];
    }
    if (!checkVector.IsEqualToZero()) {
      global.fatal << "<br>This is a programming error: the output linear combination " << outputIntersection.ToString()
      << " corresponds to the cone intersection " << checkVector.ToString()
      << " and is not equal to zero! Here is the cone output so far: "
      << out.str() << global.fatal;
    }
    out << "<br>Cones intersect, here is one intersection: 0= " << outputIntersection.ToStringLetterFormat("v");
  } else {
    out << "<br>Cones have empty intersection.";
    out << "<br> A normal separating the cones is: n =" << outputSeparatingNormal.ToString() << ". Indeed, ";
    for (int i = 0; i < coneStrictGens.size; i ++) {
      out << "<br>\\langle v_{" << i + 1 << "}, n\\rangle = "
      << outputSeparatingNormal.ScalarEuclidean(coneStrictGens[i]).ToString();
    }
    for (int i = 0; i < coneNonStrictGens.size; i ++) {
      out << "<br>\\langle v_{" << i + 1 + coneStrictGens.size << "}, n\\rangle = "
      << outputSeparatingNormal.ScalarEuclidean(coneNonStrictGens[i]).ToString();
    }
  }
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerReverseOrderRecursivelY(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerReverseOrderRecursively");
  if (input.size() < 2) {
    return false;
  }
  Expression toReverse;
  if (input.size() == 2) {
    toReverse = input[1];
  } else {
    toReverse = input;
    toReverse.SetChildAtomValue(0, theCommands.opSequence());
  }
  return theCommands.functionReverseOrderRecursively(theCommands, toReverse, output);
}

bool Calculator::functionReverseOrderRecursively(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  if (!input.IsSuitableForRecursion()) {
    output = input;
    return true;
  }
  output.reset(theCommands, input.size());
  output.AddChildOnTop(input[0]);
  for (int i = input.size() - 1; i >= 1; i --) {
    Expression currentE = input[i];
    Expression reversedCurrentE;
    if (!theCommands.functionReverseOrderRecursively(theCommands,  currentE, reversedCurrentE)) {
      return false;
    }
    output.AddChildOnTop(reversedCurrentE);
  }
  return true;
}

bool Calculator::innerReverseOrder(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerReverseOrder");
  if (!input.IsSuitableForRecursion()) {
    output = input;
    return true;
  }
  if (input.size() < 2) {
    return false;
  }
  Expression toReverse;
  if (input.size() == 2) {
    toReverse = input[1];
  } else {
    toReverse = input;
    toReverse.SetChildAtomValue(0, theCommands.opSequence());
  }
  output.reset(theCommands, toReverse.size());
  output.AddChildOnTop(toReverse[0]);
  for (int i = toReverse.size() - 1; i >= 1; i --) {
    output.AddChildOnTop(toReverse[i]);
  }
  return true;
}

template <class coefficient>
coefficient ElementUniversalEnveloping<coefficient>::GetKillingFormProduct(
  const ElementUniversalEnveloping<coefficient>& right
) const {
  MacroRegisterFunctionWithName("ElementUniversalEnveloping::GetKillingFormProduct");
  if (this->IsEqualToZero()) {
    return 0;
  }
  coefficient result = 0;
  ElementUniversalEnveloping<coefficient> adadAppliedToMon, tempElt;
  SemisimpleLieAlgebra* theOwner;
  theOwner = &this->GetOwner();
  MonomialUniversalEnveloping<coefficient> baseGen;
  for (int i = 0; i < theOwner->GetNumGenerators(); i ++) {
    baseGen.MakeGenerator(i, *theOwner);
    adadAppliedToMon.MakeZero(*theOwner);
    adadAppliedToMon.AddMonomial(baseGen, 1);
    right.AdjointRepresentationAction(adadAppliedToMon, tempElt);
    tempElt.Simplify();
    this->AdjointRepresentationAction(tempElt, adadAppliedToMon);
    adadAppliedToMon.Simplify();
    result += adadAppliedToMon.GetMonomialCoefficient(baseGen);
  }
  return result;
}

template <class coefficient>
coefficient SemisimpleLieAlgebra::GetKillingForm(
  const ElementSemisimpleLieAlgebra<coefficient>& left, const ElementSemisimpleLieAlgebra<coefficient>& right
) {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::GetKillingForm");
  coefficient result = 0;
  ElementSemisimpleLieAlgebra<coefficient> adadAppliedToMon, tempElt;
  ChevalleyGenerator baseGen;
  for (int i = 0; i < this->GetNumGenerators(); i ++) {
    baseGen.MakeGenerator(*this, i);
    adadAppliedToMon.MakeZero();
    adadAppliedToMon.AddMonomial(baseGen, 1);
    this->LieBracket(right, adadAppliedToMon, tempElt);
    this->LieBracket(left, tempElt, adadAppliedToMon);
    result += adadAppliedToMon.GetMonomialCoefficient(baseGen);
  }
  return result;
}

bool Calculator::innerKillingForm(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerKillingForm");
  if (!input.IsListNElements(3)) {
    return false;
  }
  Expression leftE = input[1];
  Expression rightE = input[2];
  if (!leftE.IsBuiltInTypE() || !rightE.IsBuiltInTypE()) {
    return false;
  }
  if (!Expression::MergeContexts(leftE, rightE)) {
    return false;
  }
  Expression theContext = leftE.GetContext();
  ElementUniversalEnveloping<RationalFunction> left, right;
  if (
    !leftE.IsOfType<ElementUniversalEnveloping<RationalFunction> >(&left) ||
    !rightE.IsOfType<ElementUniversalEnveloping<RationalFunction> >(&right)
  ) {
    return false;
  }
  if (left.IsEqualToZero() || right.IsEqualToZero()) {
    return output.AssignValue(0, theCommands);
  }
  if (&left.GetOwner() != &right.GetOwner()) {
    return false;
  }
  ElementSemisimpleLieAlgebra<Rational> leftEltSS, rightEltSS;
  if (left.GetLieAlgebraElementIfPossible(leftEltSS) && right.GetLieAlgebraElementIfPossible(rightEltSS)) {
    return output.AssignValue(leftEltSS.GetOwner()->GetKillingForm(leftEltSS, rightEltSS), theCommands);
  }
  return output.AssignValueWithContext(left.GetKillingFormProduct(right), theContext, theCommands);
}

bool Calculator::innerRootSubsystem(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerRootSubsystem");
  if (input.size() < 3) {
    return false;
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!theCommands.Convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, algebra
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  SemisimpleLieAlgebra* theSSlieAlg = algebra.content;
  int theRank = theSSlieAlg->GetRank();
  Vector<Rational> currentRoot;
  Vectors<Rational> outputRoots;
  WeylGroupData& theWeyl = theSSlieAlg->theWeyl;
  if (!theWeyl.theDynkinType.IsSimple()) {
    return theCommands << "<hr>Function root subsystem works for simple ambient types only.";
  }
  for (int i = 2; i < input.size(); i ++) {
    if (!theCommands.GetVectoR(input[i], currentRoot, nullptr, theRank, nullptr)) {
      return false;
    }
    if (!theWeyl.RootSystem.Contains(currentRoot)) {
      return output.MakeError("Input vector " + currentRoot.ToString() + " is not a root. ", theCommands);
    }
    outputRoots.AddOnTop(currentRoot);
  }
  std::stringstream out;
  DynkinDiagramRootSubalgebra theDiagram;
  theWeyl.TransformToSimpleBasisGenerators(outputRoots, theWeyl.RootSystem);
  theDiagram.AmbientBilinearForm = theWeyl.CartanSymmetric;
  theDiagram.AmbientRootSystem = theWeyl.RootSystem;
  theDiagram.ComputeDiagramInputIsSimple(outputRoots);
  out << "Diagram final: " << theDiagram.ToString()
  << ". Simple basis: " << theDiagram.SimpleBasesConnectedComponents.ToString();
  return output.AssignValue(out.str(), theCommands);
}

template <>
void Vector<Rational>::PerturbNormalRelativeToVectorsInGeneralPosition(
  const Vectors<Rational>& NonStrictConeNonPositiveScalar,
  const List<Vector<Rational> >& VectorsToBeInGeneralPosition
);

bool Calculator::innerPerturbSplittingNormal(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerPerturbSplittingNormal");
  std::stringstream out;
  if (input.size() != 4) {
    out << "Perturbing splitting normal takes 3 arguments: normal, "
    << "positive vectors, and vectors relative to which to perturb. "
    << "Instead I got " << input.size() - 1 << ". ";
    return output.MakeError(out.str(), theCommands);
  }
  Vector<Rational> splittingNormal;
  if (!theCommands.GetVectoR(input[1], splittingNormal, nullptr)) {
    return output.MakeError("Failed to extract normal from first argument. ", theCommands);
  }
  Matrix<Rational> theMat;
  Vectors<Rational> NonStrictCone, VectorsToPerturbRelativeTo;
  if (!theCommands.functionGetMatrix(
    input[2], theMat, nullptr, splittingNormal.size, nullptr
  )) {
    return output.MakeError("Failed to extract matrix from second argument. ", theCommands);
  }
  NonStrictCone.AssignMatrixRows(theMat);
  if (!theCommands.functionGetMatrix(
    input[3], theMat, nullptr, splittingNormal.size, nullptr
  )) {
    return output.MakeError("Failed to extract matrix from third argument. ", theCommands);
  }
  VectorsToPerturbRelativeTo.AssignMatrixRows(theMat);
  for (int i = 0; i < NonStrictCone.size; i ++) {
    if (splittingNormal.ScalarEuclidean(NonStrictCone[i]) < 0) {
      std::stringstream out;
      out << "The normal vector " << splittingNormal.ToString()
      << " is has negative scalar product with " << NonStrictCone[i].ToString();
      return output.MakeError(out.str(), theCommands);
    }
  }
  out << "Perturbing " << splittingNormal.ToString() << " relative to cone "
  << NonStrictCone.ToString() << " and vectors " << VectorsToPerturbRelativeTo.ToString();
  splittingNormal.PerturbNormalRelativeToVectorsInGeneralPosition(NonStrictCone, VectorsToPerturbRelativeTo);
  out << "<br>End result: " << splittingNormal.ToString();
  return output.AssignValue(out.str(), theCommands);
}

class HistorySubExpression {
public:
  friend std::ostream& operator << (
    std::ostream& output, const HistorySubExpression& theH
  ) {
    if (theH.currentE == nullptr) {
      output << "(no expression)";
    } else {
      output << theH.currentE->ToString();
    }
    output << " active: " << theH.lastActiveSubexpression;
    return output;
  }
  const Expression* currentE;
  int lastActiveSubexpression;
  HistorySubExpression(): currentE(nullptr), lastActiveSubexpression(- 1) {
  }
};

class ExpressionHistoryEnumerator{
public:
  Calculator* owner;
  int recursionDepth;
  int maximumRecursionDepth;
  Expression theHistory;
  List<Expression> output;
  List<List<std::string> > rulesNames;
  // List<List<std::string> > rulesDisplayNames;
  HashedList<std::string, MathRoutines::HashString> rulesToBeIgnored;
  MapList<std::string, std::string, MathRoutines::HashString> rulesDisplayNamesMap;
  bool ComputeRecursively(int incomingRecursionDepth, std::stringstream* commentsOnFailure);
  bool ProcessTransformation(const Expression& current, std::stringstream* commentsOnFailure);
  bool ProcessChildrenTransformations(int startIndex, int numberOfChildren, std::stringstream* commentsOnFailure);
  void initializeComputation();
  const Expression* getCurrentE(const List<Expression>& input);
  ExpressionHistoryEnumerator();
  bool IncrementRecursivelyReturnFalseIfPastLast(TreeNode<HistorySubExpression>& currentNode);
  bool CheckInitialization() {
    if (this->owner == nullptr) {
      global.fatal << "Expression history enumerator has zero owner. " << global.fatal;
    }
    return true;
  }
  std::string ToStringExpressionHistoryMerged();
  std::string ToStringDebug();
  Expression GetExpression(
    const TreeNode<HistorySubExpression>& currentNode,
    List<std::string>& outputRuleNames
  );
};

void ExpressionHistoryEnumerator::initializeComputation() {
  MacroRegisterFunctionWithName("ExpressionHistoryEnumerator::initializeComputation");
  this->output.SetSize(0);
  // this->rulesDisplayNames.SetSize(0);
  this->rulesNames.SetSize(0);
  // this->rulesToBeIgnored.Clear();
  // this->rulesToBeIgnored.AddOnTop("CommuteIfUnivariate");
  // this->rulesDisplayNamesMap.Clear();
  // this->rulesDisplayNamesMap.SetKeyValue("Minus", "");
  // this->rulesDisplayNamesMap.SetKeyValue("DistributeMultiplication", "");
  // this->rulesDisplayNamesMap.SetKeyValue("MultiplyRationals", "");
  // this->rulesDisplayNamesMap.SetKeyValue("ConstantExtraction", "");
  // this->rulesDisplayNamesMap.SetKeyValue("MultiplyByOne", "");
  // this->rulesDisplayNamesMap.SetKeyValue("AddTerms", "");
  // this->rulesDisplayNamesMap.SetKeyValue("AssociativeRule", "");
}

Expression ExpressionHistoryEnumerator::GetExpression(
  const TreeNode<HistorySubExpression>& currentNode, List<std::string>& outputRuleNames
) {
  MacroRegisterFunctionWithName("ExpressionHistoryEnumerator::GetExpression");
  Expression result;
  if (
    currentNode.theData.lastActiveSubexpression< currentNode.theData.currentE->size() &&
    currentNode.theData.lastActiveSubexpression >= 0
  ) {
    const Expression& currentRuleSequence = (*currentNode.theData.currentE)[
      currentNode.theData.lastActiveSubexpression
    ];
    if (currentRuleSequence.size() > 3) {
      std::string ruleName = currentRuleSequence[3].ToString();
      if (ruleName != "" && ruleName != "Sub-expression simplification") {
        outputRuleNames.AddOnTop(ruleName);
      }
    }
  }

  if (currentNode.children.size == 0) {
    result = *currentNode.theData.currentE;
    return result;
  }
  if (currentNode.children.size == 1) {
    result = this->GetExpression(currentNode.GetChild(0), outputRuleNames);
    return result;
  }
  result.reset(*this->owner);
  for (int i = 0; i < currentNode.children.size; i ++) {
    Expression currentE = this->GetExpression(currentNode.GetChild(i), outputRuleNames);
    result.AddChildOnTop(currentE);
  }
  return result;
}

ExpressionHistoryEnumerator::ExpressionHistoryEnumerator() {
  this->owner = nullptr;
  this->maximumRecursionDepth = 1000;
  this->recursionDepth = 0;
}

bool ExpressionHistoryEnumerator::ComputeRecursively(
  int incomingRecursionDepth, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("ExpressionHistoryEnumerator::ComputeRecursively");
  this->recursionDepth = incomingRecursionDepth;
  if (recursionDepth > this->maximumRecursionDepth) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "While computing expression history, "
      << "exceeded maximum recursion depth of "
      << this->maximumRecursionDepth << ". ";
    }
    return false;
  }
  if (!this->theHistory.StartsWith(this->owner->opExpressionHistory())) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Corrupt expression history does not start with the expected atom: "
      << this->theHistory.ToString();
      return false;
    }
  }
  int childrenToAccount = 0;
  int firstNonAccountedChildIndex = - 1;
  for (int i = 1; i < this->theHistory.size(); i ++) {
    const Expression& current = this->theHistory[i];
    if (current.StartsWith(this->owner->opExpressionHistorySetChild())) {
      if (childrenToAccount == 0) {
        firstNonAccountedChildIndex = i;
      }
      childrenToAccount ++;
      continue;
    }
    if (!this->ProcessChildrenTransformations(firstNonAccountedChildIndex, childrenToAccount, commentsOnFailure)) {
      return false;
    }
    childrenToAccount = 0;
    if (!this->ProcessTransformation(current, commentsOnFailure)) {
      return false;
    }
  }
  if (!this->ProcessChildrenTransformations(firstNonAccountedChildIndex, childrenToAccount, commentsOnFailure)) {
    return false;
  }
  return true;
}

bool ExpressionHistoryEnumerator::ProcessChildrenTransformations(
  int startIndex, int numberOfChildren, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("ExpressionHistoryEnumerator::ProcessChildrenTransformations");
  if (numberOfChildren <= 0) {
    return true;
  }
  List<int> indicesInParent;
  List<ExpressionHistoryEnumerator> childrenEnumerators;
  childrenEnumerators.SetSize(numberOfChildren);
  indicesInParent.SetSize(numberOfChildren);
  for (int i = 0; i < numberOfChildren; i ++) {
    const Expression& current = this->theHistory[startIndex + i];
    if (current.size() < 3) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "ExpressionHistorySetChild "
        << "expression does not have enough children. "
        << current.ToString() ;
      }
      return false;
    }
    if (!current[1].IsSmallInteger(&indicesInParent[i])) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "ExpressionHistorySetChild: bad child index. "
        << current.ToString() ;
      }
      return false;
    }
    childrenEnumerators[i].owner = this->owner;
    childrenEnumerators[i].theHistory = current[2];
    if (!childrenEnumerators[i].ComputeRecursively(
      this->recursionDepth + 1, commentsOnFailure
    )) {
      return false;
    }
  }
  if (this->output.size == 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Missing starting expression. History: " << this->theHistory.ToString();
    }
    return false;
  }
  List<int> currentIndices;
  currentIndices.initializeFillInObject(numberOfChildren, 0);
  while (true) {
    bool found = false;
    Expression next;
    next = *this->output.LastObject();
    List<std::string> nextRules;
    for (int i = 0; i < numberOfChildren; i ++) {
      int& currentIndex = currentIndices[i];
      List<Expression>& expressionSequence = childrenEnumerators[i].output;
      if (currentIndex + 1 < expressionSequence.size) {
        currentIndex ++;
        found = true;
        int indexInParent = indicesInParent[i];
        next.SetChilD(indexInParent, expressionSequence[currentIndex]);
        nextRules.AddListOnTop(childrenEnumerators[i].rulesNames[currentIndex]);
      }
    }
    if (!found) {
      break;
    }
    this->output.AddOnTop(next);
    this->rulesNames.AddOnTop(nextRules);
  }
  return true;
}

bool ExpressionHistoryEnumerator::ProcessTransformation(
  const Expression& current, std::stringstream* commentsOnFailure
) {
  if (!current.StartsWith(this->owner->opExpressionHistorySet())) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Bad first atom in history child : " << current.ToString();
    }
    return false;
  }
  if (current.size() < 2) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Not enough elements in history child: " << current.ToString();
    }
    return false;
  }
  this->output.AddOnTop(current[1]);
  this->rulesNames.SetSize(this->rulesNames.size + 1);
  this->rulesNames.LastObject()->SetSize(0);
  if (current.size() >= 3) {
    std::string incoming = current[2].ToString();
    if (incoming != "") {
      this->rulesNames.LastObject()->AddOnTop(incoming);
    }
  }
  return true;
}

std::string ExpressionHistoryEnumerator::ToStringDebug() {
  MacroRegisterFunctionWithName("ExpressionHistoryEnumerator::ToStringDebug");
  std::stringstream out;
  out << "<b>History</b><br>" << this->theHistory.ToStringTreeHtml(- 1) << "<hr>";
  out << "" << this->theHistory.ToStringSemiFull() << "<hr>";
  // out << "Current state: " << this->currentState.ToString()
  // << "<br>";
  // out << "Current subtree: " << this->currentSubTree.ToString();

  return out.str();
}

std::string ExpressionHistoryEnumerator::ToStringExpressionHistoryMerged() {
  std::stringstream out;
  out << "\\(\\begin{array}{ll|l}";
  std::string prevEstring = "";
  List<std::string> currentRules;
  for (int j = 0; j < this->output.size; j ++) {
    std::string currentEstring = this->output[j].ToString();
    if (currentEstring == prevEstring) {
      currentRules.AddListOnTop(this->rulesNames[j]);
      continue;
    }
    prevEstring = currentEstring;
    currentRules.AddListOnTop(this->rulesNames[j]);
    if (j > 0) {
      if (currentRules.size > 0) {
        out << "&\\text{" << currentRules.ToStringCommaDelimited() << "}";
      }
      currentRules.SetSize(0);
      out << "\\\\";
      out << "=";
    }
    out << "&\n" << currentEstring;
  }
  if (currentRules.size > 0) {
    out << "&\\text{apply: " << currentRules.ToStringCommaDelimited() << "}";
  }
  out << "\\end{array}\\)";
  return out.str();
}

bool Calculator::innerLogEvaluationStepsDebug(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  return Calculator::innerLogEvaluationStepsHumanReadableMerged(theCommands, input, output, true);
}

bool Calculator::innerLogEvaluationSteps(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  return Calculator::innerLogEvaluationStepsHumanReadableMerged(theCommands, input, output, false);
}

bool Calculator::innerLogEvaluationStepsHumanReadableMerged(
  Calculator& theCommands,
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
    argument.SetChildAtomValue(0, theCommands.opSequence());
  }
  Expression outputTransformation;
  bool notUsed = false;
  ExpressionHistoryEnumerator history;
  theCommands.EvaluateExpression(
    theCommands,
    argument,
    outputTransformation,
    notUsed,
    - 1,
    &history.theHistory
  );
  std::stringstream out;
  history.owner = &theCommands;
  history.ComputeRecursively(0, &out);
  out << history.ToStringExpressionHistoryMerged();
  if (doDebug) {
    out << "<hr>" << history.ToStringDebug();
  }
  return output.AssignValue(out.str(), theCommands);
}
