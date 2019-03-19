//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader2Math1_SemisimpleLieAlgebras.h"
#include "vpfImplementationHeader2Math051_PolynomialComputations_Basic.h"
ProjectInformationInstance ProjectInfoVpf9_4cpp(__FILE__, "Semisimple Lie algebras. ");

std::string SemisimpleLieAlgebra::ToString(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::ToString");
  std::stringstream out;
  std::string tempS;
  Vector<Rational> tempRoot, tempRoot2;
  int numRoots = this->theWeyl.RootSystem.size;
  int theDimension = this->theWeyl.CartanSymmetric.NumRows;
  ElementSemisimpleLieAlgebra<Rational> tempElt1, tempElt2, tempElt3;
//  out << beginMath << "\\begin{array}{ccc}a& a&a\\\\a&a&a\\end{array}";
  std::string hLetter = "h";
  std::string gLetter = "g";
  if (theFormat != 0) {
    hLetter = theFormat->chevalleyHgeneratorLetter;
    gLetter = theFormat->chevalleyGgeneratorLetter;
  }
  out << "Type " << this->theWeyl.theDynkinType.ToString() << ".";
  out << "The letter " << HtmlRoutines::GetMathSpanPure(hLetter) << " stands for elements of the Cartan subalgebra, <br>"
  << " the letter " << HtmlRoutines::GetMathSpanPure(gLetter) << " stands for the Chevalley (root space) generators of non-zero weight. <br>"
  << " The generator " << HtmlRoutines::GetMathSpanPure(hLetter + "_i") << " is the element of the Cartan subalgebra dual to the <br>"
  << "i^th simple root, that is, " << HtmlRoutines::GetMathSpanPure("[" + hLetter + "_i, g] =\\langle \\alpha_i , \\gamma\\rangle g")
  << ", <br> where g is a Chevalley generator, " << HtmlRoutines::GetMathSpanPure("\\gamma") << " is its weight, and <br>"
  << HtmlRoutines::GetMathSpanPure("\\alpha_i") << " is the i^th simple root. ";
  std::stringstream theTableLateXStream, theHtmlStream;
  theHtmlStream << "<table><tr><td> roots simple coords </td><td>epsilon coordinates</td>"
  << "<td>[,]</td>";
  theTableLateXStream << "\\begin{array}{cc|";
  for (int i = 0; i < this->GetNumGenerators() + 1; i ++)
    theTableLateXStream << "c";
  theTableLateXStream << "}\n";
  theTableLateXStream << "\\mathrm{roots~simple~coords}&\\varepsilon-\\mathrm{root~notation}&" << "[\\bullet, \\bullet]\n";
  for (int i = 0; i < numRoots + theDimension; i ++) {
    tempElt1.MakeGenerator(i, *this);
    tempS = tempElt1.ToString(theFormat);
    theHtmlStream << "<td>" << tempS << "</td>";
    theTableLateXStream << " & ";
    theTableLateXStream << tempS;
  }
  theTableLateXStream << "\\\\\n";
  theHtmlStream << "</tr>";
  //int lineCounter = 0;
  for (int i = 0; i < theDimension + numRoots; i ++) {
    tempRoot = this->GetWeightOfGenerator(i);
    theTableLateXStream << tempRoot.ToString() << "&";
    theHtmlStream << "<tr><td>" << tempRoot.ToString() << "</td>";
    this->theWeyl.GetEpsilonCoords(tempRoot, tempRoot2);
    theTableLateXStream << tempRoot2.ToStringLetterFormat("\\varepsilon") << "&";
    theHtmlStream << "<td>" << tempRoot2.ToStringLetterFormat("e") << "</td>";
    tempElt1.MakeGenerator(i, *this);
    tempS = tempElt1.ToString(theFormat);
    theTableLateXStream << tempS;
    theHtmlStream << "<td>" << tempS << "</td>";
    for (int j = 0; j < numRoots + theDimension; j ++) {
      tempElt2.MakeGenerator(j, *this);
      this->LieBracket(tempElt1, tempElt2, tempElt3);
      tempS = tempElt3.ToString(theFormat);
      theTableLateXStream << "& ";
      theTableLateXStream << tempS;
      theHtmlStream << "<td>" << tempS << "</td>";
    }
    theHtmlStream << "</tr>";
    theTableLateXStream << "\\\\\n";
  }
  theHtmlStream << "</table>";
  theTableLateXStream << "\\end{array}";
  if (this->GetNumGenerators() < 22) {
    out << "<div class =\"math\">" << theTableLateXStream.str() << "</div>";
    return out.str();
  }
  out << "<br><b> The Lie bracket table is too large to be rendered in LaTeX, displaying in"
  << " html format instead.</b> "
  << " Below you can find the same table in pure LaTeX, "
  << " which you can render in a separate LaTeX session, should "
  << " wish to do so on your own. " <<  theHtmlStream.str();
  if (theFormat != 0) {
    if (theFormat->flagLatexDetailsInHtml) {
      out << "The above table in LaTex format follows. <hr>"
      << theTableLateXStream.str() << "<hr>";
    }
  }
  return out.str();
}

void SemisimpleLieAlgebra::ComputeFolderNames() {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::ComputeFolderNames");
  std::stringstream outMainDisplayPath;
  this->CheckConsistency();
  DynkinType& theType = this->theWeyl.theDynkinType;
  this->VirtualNameSSAlgOutputFolder = "output/" + HtmlRoutines::CleanUpForFileNameUse(theType.ToString()) + "/";
  outMainDisplayPath << theGlobalVariables.DisplayPathOutputFolder
  << HtmlRoutines::CleanUpForFileNameUse(theType.ToString()) << "/";
  this->DisplayNameSSalgOutputFolder = outMainDisplayPath.str();
}

void SemisimpleLieAlgebra::ComputeChevalleyConstants() {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::ComputeChevalleyConstants");
  this->theWeyl.ComputeRho(true);
  this->ChevalleyConstants.init(this->theWeyl.RootSystem.size, this->theWeyl.RootSystem.size);
  this->Computed.init(this->theWeyl.RootSystem.size, this->theWeyl.RootSystem.size);
  this->Computed.MakeZero(false);
  Selection nonExploredRoots;
  this->flagAnErrorHasOccurredTimeToPanic = false;
  Vectors<Rational>& posRoots = this->theWeyl.RootsOfBorel;
  nonExploredRoots.MakeFullSelection(posRoots.size);
  Vector<Rational> tempRoot;
  std::stringstream out;
  ProgressReport theReport;
  double startTimer = - 1;
  if (theGlobalVariables.flagReportEverything) {
    out << "Initializing matrix for structure constant computation of " << this->GetLieAlgebraName() << "... ";
    startTimer = theGlobalVariables.GetElapsedSeconds();
    theReport.Report(out.str());
  }
  for (int i = 0; i < this->theWeyl.RootSystem.size; i ++) {
    for (int j = i; j < this->theWeyl.RootSystem.size; j ++) {
      tempRoot = this->theWeyl.RootSystem[i] + this->theWeyl.RootSystem[j];
      if (!tempRoot.IsEqualToZero()) {
        if (!this->theWeyl.IsARoot(tempRoot)) {
          this->Computed.elements[i][j] = true;
          this->ChevalleyConstants.elements[i][j].MakeZero();
          this->Computed.elements[j][i] = true;
          this->ChevalleyConstants.elements[j][i].MakeZero();
        }
      }
    }
  }
  double startStructureConstantComputation = - 1;
  if (theGlobalVariables.flagReportEverything) {
    out << "done in " << theGlobalVariables.GetElapsedSeconds() - startTimer
    << " seconds.<br> " << "Computing structure constants...";
    theReport.Report(out.str());
    startStructureConstantComputation = theGlobalVariables.GetElapsedSeconds();
  }
  Rational tempRat;
  while (nonExploredRoots.CardinalitySelection > 0) {
    //this->ComputeDebugString();
    //nonExploredRoots.ComputeDebugString();
    int theBorelIndex = nonExploredRoots.elements[0];
    Rational theHeight = posRoots[theBorelIndex].SumCoords();
    for (int i = 1; i < nonExploredRoots.CardinalitySelection; i ++) {
      tempRat = posRoots[nonExploredRoots.elements[i]].SumCoords();
      if (theHeight.IsGreaterThan(tempRat)) {
        theHeight = tempRat;
        theBorelIndex = nonExploredRoots.elements[i];
      }
    }
    Vector<Rational>& theRoot = posRoots[theBorelIndex];
    int theIndex = this->theWeyl.RootSystem.GetIndex(theRoot);
    Vector<Rational> smallRoot2;
    int FirstIndexFirstPosChoice = - 1;
    int SecondIndexFirstPosChoice = - 1;
    Rational CurrentHeight;
    for (int i = 0; i < this->theWeyl.RootsOfBorel.size; i ++) {
      Vector<Rational>& smallRoot1 = this->theWeyl.RootsOfBorel[i];
      CurrentHeight = smallRoot1.SumCoords();
      int FirstPosIndex = this->theWeyl.RootSystem.GetIndex(smallRoot1);
      int FirstNegIndex = this->theWeyl.RootSystem.GetIndex(- smallRoot1);
      if (theHeight.IsGreaterThan(CurrentHeight)) {
        smallRoot2 = theRoot - smallRoot1;
        int SecondPosIndex = this->theWeyl.RootSystem.GetIndex(smallRoot2);
        if (FirstPosIndex<SecondPosIndex) {
          int SecondNegIndex = this->theWeyl.RootSystem.GetIndex(- smallRoot2);
          if (FirstIndexFirstPosChoice == - 1) {
            FirstIndexFirstPosChoice = FirstPosIndex;
            SecondIndexFirstPosChoice = SecondPosIndex;
            int thePower;
            this->GetMaxQForWhichBetaMinusQAlphaIsARoot(smallRoot1, smallRoot2, thePower);
            this->ChevalleyConstants.elements[FirstNegIndex][SecondNegIndex] = - 1 - thePower;
            this->Computed.elements[FirstNegIndex][SecondNegIndex] = true;
          } else {
            this->ComputeOneChevalleyConstant(
              FirstIndexFirstPosChoice, SecondIndexFirstPosChoice, FirstNegIndex, SecondNegIndex, theIndex
            );
          }
          this->ExploitSymmetryAndCyclicityChevalleyConstants(FirstNegIndex, SecondNegIndex);
        }
      }
    }
    nonExploredRoots.selected[theBorelIndex] = false;
    nonExploredRoots.ComputeIndicesFromSelection();
  }
  double startMultTable = - 1;
  if (theGlobalVariables.flagReportEverything) {
    out << "done in " << theGlobalVariables.GetElapsedSeconds() - startStructureConstantComputation
    << " seconds.<br> Computing Lie bracket pairing (``multiplication'') table...";
    theReport.Report(out.str());
    startMultTable = theGlobalVariables.GetElapsedSeconds();
  }
  this->ComputeMultTable();
  if (theGlobalVariables.flagReportEverything) {
    out << " done in " << theGlobalVariables.GetElapsedSeconds() - startMultTable
    << " seconds. Total structure constant computation time: "
    << theGlobalVariables.GetElapsedSeconds() - startTimer << " seconds. ";
    theReport.Report(out.str());
  }
  if (this->GetNumPosRoots() <= 0) {
    crash << "This is a programming error: number of positive roots of a "
    << "semisimple Lie algebra is reported to be zero. " << crash;
  }
}

void SemisimpleLieAlgebra::ComputeMultTable() {
  int numPosRoots = this->theWeyl.RootsOfBorel.size;
  int theRank = this->theWeyl.CartanSymmetric.NumRows;
  int numRoots = numPosRoots * 2;
  int numGenerators = numRoots + theRank;
  this->theLiebrackets.init(numGenerators, numGenerators);
//  this->theLiebracketPairingIndices.init(numGenerators, numGenerators);
  this->UEGeneratorOrderIncludingCartanElts.initializeFillInObject(numGenerators, - 1);
//  this->theLiebracketPairingIndices.MakeZero(- 1);
//  this->OppositeRootSpaces.initializeFillInObject(numRoots+theDimension, - 1);
  Vector<Rational> leftWeight, rightWeight, hRoot;
  for (int i = 0; i < numGenerators; i ++) {
    leftWeight = this->GetWeightOfGenerator(i);
    for (int j = i; j < numGenerators; j ++) {
      rightWeight = this->GetWeightOfGenerator(j);
      if (leftWeight.IsEqualToZero() && rightWeight.IsEqualToZero()) {
        this->theLiebrackets.elements[i][j].MakeZero();
        continue;
      }
      if (leftWeight.IsEqualToZero() && !rightWeight.IsEqualToZero()) {
        this->theLiebrackets.elements[i][j].MakeGenerator(j, *this);
        hRoot.MakeEi(theRank, i - numPosRoots);
        this->theLiebrackets.elements[i][j] *= Vector<Rational>::ScalarProduct(
          hRoot, rightWeight, this->theWeyl.CartanSymmetric
        );
        continue;
      }
      if (!leftWeight.IsEqualToZero() && rightWeight.IsEqualToZero()) {
        this->theLiebrackets.elements[i][j].MakeGenerator(i, *this);
        hRoot.MakeEi(theRank, j - numPosRoots);
        this->theLiebrackets.elements[i][j] *= - Vector<Rational>::ScalarProduct(
          hRoot, leftWeight, this->theWeyl.CartanSymmetric
        );
        continue;
      }
      if (!leftWeight.IsEqualToZero() && !rightWeight.IsEqualToZero()) {
        int newIndex = this->GetGeneratorFromRoot(leftWeight + rightWeight);
        if (newIndex != - 1) {
          this->theLiebrackets.elements[i][j].MakeGenerator(newIndex, *this);
          int leftIndex = this->GetRootIndexFromGenerator(i);
          int rightIndex = this->GetRootIndexFromGenerator(j);
          this->theLiebrackets.elements[i][j] *= this->ChevalleyConstants.elements[leftIndex][rightIndex];
        } else {
          if (!(leftWeight +rightWeight).IsEqualToZero()) {
            this->theLiebrackets.elements[i][j].MakeZero();
          } else {
            ElementSemisimpleLieAlgebra<Rational>& current = this->theLiebrackets.elements[i][j];
            current.MakeHgenerator(leftWeight * 2 / (this->theWeyl.RootScalarCartanRoot(leftWeight, leftWeight)), *this);
          }
        }
        continue;
      }
    }
  }
  for (int i = 0; i < numGenerators; i ++) {
    for (int j = i + 1; j < numGenerators; j ++) {
      this->theLiebrackets.elements[j][i] = this->theLiebrackets.elements[i][j];
      this->theLiebrackets.elements[j][i] *= - 1;
    }
  }
  for (int i = 0; i < numGenerators; i ++) {
    this->UEGeneratorOrderIncludingCartanElts[i] = i;
  }
}

void SemisimpleLieAlgebra::ExploitSymmetryAndCyclicityChevalleyConstants(int indexI, int indexJ) {
  const Vector<Rational>& rootI = this->theWeyl.RootSystem[indexI];
  const Vector<Rational>& rootJ = this->theWeyl.RootSystem[indexJ];
  if ((rootI + rootJ).IsEqualToZero())
    crash << crash;
  //int indexMinusI = this->theWeyl.RootSystem.GetIndex(-rootI);
  //int indexMinusJ = this->theWeyl.RootSystem.GetIndex(-rootJ);
  //this->ComputeDebugString();
  this->ExploitSymmetryChevalleyConstants(indexI, indexJ);
  //this->ComputeDebugString();
  //int indexRootIPlusRootJ = this->theWeyl.RootSystem.GetIndex(rootI + rootJ);
  int indexMinusIMinusJ = this->theWeyl.RootSystem.GetIndex(- rootI - rootJ);
  this->ExploitTheCyclicTrick(indexI, indexJ, indexMinusIMinusJ);
  //this->ComputeDebugString();
  //this->ExploitTheCyclicTrick(indexMinusI, indexMinusJ, indexRootIPlusRootJ);
  //this->ComputeDebugString();
}

void SemisimpleLieAlgebra::ExploitSymmetryChevalleyConstants(int indexI, int indexJ) {
  const Vector<Rational>& rootI = this->theWeyl.RootSystem[indexI];
  const Vector<Rational>& rootJ = this->theWeyl.RootSystem[indexJ];
  if (!this->Computed.elements[indexI][indexJ]) {
    crash << "Structure constants computed in a wrong order. " << crash;
  }
  int indexMinusI = this->theWeyl.RootSystem.GetIndex(- rootI);
  int indexMinusJ = this->theWeyl.RootSystem.GetIndex(- rootJ);
  this->ChevalleyConstants.elements[indexJ][indexI] = (this->ChevalleyConstants.elements[indexI][indexJ] * (- 1));
  this->Computed.elements[indexJ][indexI] = true;
  if ((rootI + rootJ).IsEqualToZero())
    crash << "Bad root sum. " << crash;
  int thePower;
  this->GetMaxQForWhichBetaMinusQAlphaIsARoot(
    this->theWeyl.RootSystem[indexMinusI], this->theWeyl.RootSystem[indexMinusJ], thePower
  );
  int i = 1 + thePower;
  this->ChevalleyConstants.elements[indexMinusI][indexMinusJ] = - i * i;
  this->ChevalleyConstants.elements[indexMinusI][indexMinusJ] /= this->ChevalleyConstants.elements[indexI][indexJ];
  this->Computed.elements[indexMinusI][indexMinusJ] = true;
  this->ChevalleyConstants.elements[indexMinusJ][indexMinusI] = this->ChevalleyConstants.elements[indexMinusI][indexMinusJ] * (- 1);
  this->Computed.elements[indexMinusJ][indexMinusI] = true;
}

void SemisimpleLieAlgebra::ExploitTheCyclicTrick(int i, int j, int k) {
  const Vector<Rational>& rootI = this->theWeyl.RootSystem[i];
  const Vector<Rational>& rootK = this->theWeyl.RootSystem[k];
  const Vector<Rational>& rootJ = this->theWeyl.RootSystem[j];
  //the following three checks can be commented out to increase speed. They have never failed so far.
  if (!(rootI + rootK + rootJ).IsEqualToZero()) {
    crash << "Three roots do not sum to zero as required. " << crash;
  }
  if (!this->Computed.elements[i][j]) {
    crash << "Bad structure constant computation order." << crash;
  }
  /////////////////////////////////////////////////////////////////
  Rational& tempRat = this->ChevalleyConstants.elements[i][j];
  Rational tempRat2 = this->theWeyl.RootScalarCartanRoot(rootK, rootK);
  this->ChevalleyConstants.elements[j][k] = (tempRat * this->theWeyl.RootScalarCartanRoot(rootI, rootI)) / tempRat2;
  this->Computed.elements[j][k] = true;
  this->ChevalleyConstants.elements[k][i] = (tempRat * this->theWeyl.RootScalarCartanRoot(rootJ, rootJ)) / tempRat2;
  this->Computed.elements[k][i] = true;
  this->ExploitSymmetryChevalleyConstants(j, k);
  this->ExploitSymmetryChevalleyConstants(k, i);
}

bool SemisimpleLieAlgebra::GetMaxQForWhichBetaMinusQAlphaIsARoot(
  const Vector<Rational>& alpha, const Vector<Rational>& beta, int& output
) const {
  output = - 1;
  Vector<Rational> tempRoot = beta;
  if (alpha.IsEqualToZero()) {
    crash << "This is a programming error: calling function "
    << "GetMaxQForWhichBetaMinusQAlphaIsARoot with zero value for alpha is not allowed. " << crash;
  }
  bool foundRoot = false;
  while (this->theWeyl.IsARoot(tempRoot)) {
    output ++;
    tempRoot -= alpha;
    foundRoot = true;
  }
  return foundRoot;
}

void SemisimpleLieAlgebra::ComputeOneChevalleyConstant(
  int indexGamma, int indexDelta, int indexMinusEpsilon, int indexMinusZeta, int indexEta
) {
 //using formula (**), 2.9, page 49, Samelson, Notes on Lie algebras, 1989
  const Vector<Rational>& gamma = this->theWeyl.RootSystem[indexGamma];
  const Vector<Rational>& delta = this->theWeyl.RootSystem[indexDelta];
  const Vector<Rational>& minusEpsilon = this->theWeyl.RootSystem[indexMinusEpsilon];
  const Vector<Rational>& eta = this->theWeyl.RootSystem[indexEta];
  const Vector<Rational>& minusZeta = this->theWeyl.RootSystem[indexMinusZeta];
  if (eta != gamma + delta) {
    crash << "Eta must equal gamma plus delta. " << crash;
  }
  if (!this->theWeyl.IsARoot(eta + minusEpsilon)) {
    crash << "Eta minus epsilon must be a root. " << crash;
  }
  if (
    !this->Computed.elements[indexDelta][indexMinusEpsilon] ||
    !this->Computed.elements[indexMinusEpsilon][indexGamma] ||
    !this->Computed.elements[indexGamma][indexDelta]
  ) {
    crash << "Bad structure constant computation order. " << crash;
  }
  if (this->ChevalleyConstants.elements[indexGamma][indexDelta].IsEqualToZero())
    crash << "Unexpected zero structure constants" << crash;
  int indexDeltaMinusEpsilon = this->theWeyl.RootSystem.GetIndex(delta + minusEpsilon);
  int indexGammaMinusEpsilon = this->theWeyl.RootSystem.GetIndex(gamma + minusEpsilon);
  Rational FirstSummand, SecondSummand;
  if (indexDeltaMinusEpsilon != - 1) {
    if (
      !this->Computed.elements[indexGamma][indexDeltaMinusEpsilon] ||
      !this->Computed.elements[indexDelta][indexMinusEpsilon]
    ) {
      crash << "Structure constants must be computed at this point. " << crash;
    }
    FirstSummand = this->ChevalleyConstants.elements[indexGamma][indexDeltaMinusEpsilon] * this->ChevalleyConstants.elements[indexDelta][indexMinusEpsilon];
  } else {
    FirstSummand.MakeZero();
  }
  if (indexGammaMinusEpsilon != - 1) {
    if (
      !this->Computed.elements[indexDelta][indexGammaMinusEpsilon] ||
      !this->Computed.elements[indexMinusEpsilon][indexGamma]
    ) {
      crash << "Structure constants must be computed at this point. " << crash;
    }
    SecondSummand = this->ChevalleyConstants.elements[indexDelta][indexGammaMinusEpsilon]*this->ChevalleyConstants.elements[indexMinusEpsilon][indexGamma];
  } else {
    SecondSummand.MakeZero();
  }
  this->ChevalleyConstants.elements[indexMinusEpsilon][indexMinusZeta] = (
    this->theWeyl.RootScalarCartanRoot(eta, eta) /
    this->theWeyl.RootScalarCartanRoot(minusZeta, minusZeta)
  ) * (FirstSummand + SecondSummand) / this->ChevalleyConstants.elements[indexGamma][indexDelta];
  this->Computed.elements[indexMinusEpsilon][indexMinusZeta] = true;
}

bool SemisimpleLieAlgebra::TestForConsistency() {
  //HashedList<Vector<Rational> >& theRoots = this->theWeyl.RootSystem;
  FormatExpressions& theFormat = theGlobalVariables.theDefaultFormat.GetElement();
  ElementSemisimpleLieAlgebra<Rational> g1, g2, g3, g23, g31, g12, g123, g231, g312, temp;
  //this->ComputeDebugString(false, false, theGlobalVariables);
  for (int i = 0; i < this->GetNumGenerators(); i ++) {
    g1.MakeGenerator(i, *this);
    for (int j = 0; j < this->GetNumGenerators(); j ++) {
      g2.MakeGenerator(j, *this);
      for (int k = 0; k < this->GetNumGenerators(); k ++) {
        g3.MakeGenerator(k, *this);
        this->LieBracket(g2, g3, g23);
        this->LieBracket(g1, g23, g123);
        this->LieBracket(g3, g1, g31);
        this->LieBracket(g2, g31, g231);
        this->LieBracket(g1, g2, g12);
        this->LieBracket(g3, g12, g312);
        temp = g123;
        temp += g231;
        temp += g312;
        if (!temp.IsEqualToZero()) {
          crash << "This is a programming error. "
          << "The computed structure constants are wrong: the Jacobi identity fails. "
          << "More precisely, I get that "
          << "<br>[" << g1.ToString(&theFormat) << ", " << g2.ToString(&theFormat) << "] =" << g12.ToString(&theFormat)
          << "<br>[" << g2.ToString(&theFormat) << ", " << g3.ToString(&theFormat) << "] =" << g23.ToString(&theFormat)
          << "<br>[" << g3.ToString(&theFormat) << ", " << g1.ToString(&theFormat) << "] =" << g31.ToString(&theFormat)
          << "<br>g123= " << g123.ToString(&theFormat)
          << "<br>g231=" << g231.ToString(&theFormat)
          << "<br>g312=" << g312.ToString(&theFormat) << "<br>"
          << crash;
          return false;
        }
        this->MakeChevalleyTestReport(i, j, k, this->GetNumGenerators());
      }
    }
  }
  return true;
}

Rational SemisimpleLieAlgebra::GetConstant(const Vector<Rational>& root1, const Vector<Rational>& root2) {
  int index1 = this->theWeyl.RootSystem.GetIndex(root1);
  int index2 = this->theWeyl.RootSystem.GetIndex(root2);
  if (index1 == - 1 || index2 == - 1) {
    return 0;
  }
  return this->ChevalleyConstants(index1, index2);
}

bool SemisimpleLieAlgebra::GetConstantOrHElement(
  const Vector<Rational>& root1, const Vector<Rational>& root2, Rational& outputRat, Vector<Rational>& outputH
) {
  if (!(root1 + root2).IsEqualToZero()) {
    outputRat = this->GetConstant(root1, root2);
    return true;
  }
  if (this->theWeyl.RootSystem.GetIndex(root1) == - 1 || this->theWeyl.RootSystem.GetIndex(root2) == - 1) {
    outputRat.MakeZero();
    return true;
  }
  outputH = (root1 * 2) / Vector<Rational>::ScalarProduct(root1, root1, this->theWeyl.CartanSymmetric);
  return false;
}

void SemisimpleLieAlgebra::MakeChevalleyTestReport(int i, int j, int k, int Total) {
  if (theGlobalVariables.IndicatorStringOutputFunction == 0) {
    return;
  }
  std::stringstream out2, out3;
  int x = i * Total * Total + j * Total + k + 1;
  out2 << "i: " << i + 1 << " of " << Total << " j: " << j + 1 << " of "
  << Total << " k: " << k + 1 << " of " << Total;
  out3 << "Total progress: " << x << " out of " << (Total * Total * Total);
  ProgressReport theReport;
  theReport.Report(out2.str() + out3.str());
}
