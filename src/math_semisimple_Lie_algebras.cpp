// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_extra_semisimple_Lie_algebras_root_subalgebras.h"
#include "math_extra_semisimple_Lie_algebras_implementation.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_extra_drawing_variables.h"
#include "string_constants.h"


std::string SemisimpleLieAlgebra::toString(FormatExpressions* format) {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::toString");
  std::stringstream out;
  std::string tempS;
  Vector<Rational> tempRoot, tempRoot2;
  int numRoots = this->weylGroup.rootSystem.size;
  int theDimension = this->weylGroup.cartanSymmetric.numberOfRows;
  ElementSemisimpleLieAlgebra<Rational> tempElt1, tempElt2, tempElt3;
//  out << beginMath << "\\begin{array}{ccc}a& a&a\\\\a&a&a\\end{array}";
  std::string hLetter = "h";
  std::string gLetter = "g";
  if (format != nullptr) {
    hLetter = format->chevalleyHgeneratorLetter;
    gLetter = format->chevalleyGgeneratorLetter;
  }
  out << "Type " << this->weylGroup.dynkinType.toString() << ".";
  out << "The letter " << HtmlRoutines::getMathNoDisplay(hLetter)
  << " stands for elements of the Cartan subalgebra, <br>"
  << " the letter " << HtmlRoutines::getMathNoDisplay(gLetter)
  << " stands for the Chevalley (root space) generators of non-zero weight. <br>"
  << " The generator " << HtmlRoutines::getMathNoDisplay(hLetter + "_i")
  << " is the element of the Cartan subalgebra dual to the <br>"
  << "i^th simple root, that is, "
  << HtmlRoutines::getMathNoDisplay("[" + hLetter + "_i, g] =\\langle \\alpha_i , \\gamma\\rangle g")
  << ", <br> where g is a Chevalley generator, " << HtmlRoutines::getMathNoDisplay("\\gamma")
  << " is its weight, and <br>"
  << HtmlRoutines::getMathNoDisplay("\\alpha_i") << " is the i^th simple root. ";
  std::stringstream theTableLateXStream, theHtmlStream;
  theHtmlStream << "<table><tr><td> roots simple coords </td><td>epsilon coordinates</td>"
  << "<td>[,]</td>";
  theTableLateXStream << "\\begin{array}{cc|";
  for (int i = 0; i < this->getNumberOfGenerators() + 1; i ++) {
    theTableLateXStream << "c";
  }
  theTableLateXStream << "}\n";
  theTableLateXStream << "\\mathrm{roots~simple~coords}&\\varepsilon-\\mathrm{root~notation}&" << "[\\bullet, \\bullet]\n";
  for (int i = 0; i < numRoots + theDimension; i ++) {
    tempElt1.makeGenerator(i, *this);
    tempS = tempElt1.toString(format);
    theHtmlStream << "<td>" << tempS << "</td>";
    theTableLateXStream << " & ";
    theTableLateXStream << tempS;
  }
  theTableLateXStream << "\\\\\n";
  theHtmlStream << "</tr>";
  //int lineCounter = 0;
  for (int i = 0; i < theDimension + numRoots; i ++) {
    tempRoot = this->getWeightOfGenerator(i);
    theTableLateXStream << tempRoot.toString() << "&";
    theHtmlStream << "<tr><td>" << tempRoot.toString() << "</td>";
    this->weylGroup.getEpsilonCoordinates(tempRoot, tempRoot2);
    theTableLateXStream << tempRoot2.toStringLetterFormat("\\varepsilon") << "&";
    theHtmlStream << "<td>" << tempRoot2.toStringLetterFormat("e") << "</td>";
    tempElt1.makeGenerator(i, *this);
    tempS = tempElt1.toString(format);
    theTableLateXStream << tempS;
    theHtmlStream << "<td>" << tempS << "</td>";
    for (int j = 0; j < numRoots + theDimension; j ++) {
      tempElt2.makeGenerator(j, *this);
      this->lieBracket(tempElt1, tempElt2, tempElt3);
      tempS = tempElt3.toString(format);
      theTableLateXStream << "& ";
      theTableLateXStream << tempS;
      theHtmlStream << "<td>" << tempS << "</td>";
    }
    theHtmlStream << "</tr>";
    theTableLateXStream << "\\\\\n";
  }
  theHtmlStream << "</table>";
  theTableLateXStream << "\\end{array}";
  if (this->getNumberOfGenerators() < 22) {
    out << "<div class='mathcalculator'>" << theTableLateXStream.str() << "</div>";
    return out.str();
  }
  out << "<br><b> The Lie bracket table is too large to be rendered in LaTeX, displaying in "
  << "html format instead.</b> ";
  if (format != nullptr) {
    if (format->flagLatexDetailsInHtml) {
      out << "Below you can find the same table in pure LaTeX, "
      << "which you can render in a separate LaTeX session, should "
      << "wish to do so on your own. ";
    }
  }
  out << theHtmlStream.str();
  if (format != nullptr) {
    if (format->flagLatexDetailsInHtml) {
      out << "The above table in LaTex format follows. <hr>"
      << theTableLateXStream.str() << "<hr>";
    }
  }
  return out.str();
}

std::string SemisimpleLieAlgebra::toStringHTMLMenuStructureSummary(
  const std::string& relativeTo,
  bool includeStructureConstants,
  bool includeRootSubalgebras,
  bool includeSl2Subalgebras,
  bool includeSemisimpleSubalgebras
) {
  std::stringstream out;
  out << "<div class = 'divPanelSemisimpleLieAlgebraStructure'>";
  out << this->toStringLieAlgebraNameFullHTML() << "<br>";
  out << this->toStringMenuStructurePages(
    relativeTo, includeStructureConstants, includeRootSubalgebras, includeSl2Subalgebras, includeSemisimpleSubalgebras
  );
  out << "<hr>Page generated by the <a href = '" << HtmlRoutines::gitRepository << "'>calculator project</a>. ";
  out << "</div>";
  return out.str();
}

std::string SemisimpleLieAlgebra::toStringMenuStructurePages(
  const std::string& relativeTo,
  bool includeStructureConstants,
  bool includeRootSubalgebras,
  bool includeSl2Subalgebras,
  bool includeSemisimpleSubalgebras
) const {
  std::stringstream out;
  if (includeStructureConstants) {
    out
    << "<a href = '" << relativeTo << this->toStringFileNameNoPathStructureConstants()
    << "'>Structure constants and notation</a>.<br>";
  } else {
    out << "<b>Structure constants and notation</b>.<br>";
  }
  if (includeRootSubalgebras) {
    out
    << "<a href = '" << relativeTo << this->toStringFileNameNoPathRootSubalgebras()
    << "'>Root subalgebras / root subsystems</a>.<br>";
  } else {
    out << "<b>Root subalgebras / root subsystems</b>.<br>";
  }
  if (includeSl2Subalgebras) {
    out
    << "<a href = '" << relativeTo
    << this->toStringFileNameRelativePathSlTwoSubalgebras()
    << "'>sl(2)-subalgebras</a>.<br>";
  } else {
    out << "<b>sl(2)-subalgebras</b>.<br>";
  }
  if (this->weylGroup.dynkinType.hasPrecomputedSubalgebras()) {
    if (includeSemisimpleSubalgebras) {
      out
      << "<a href = '" << relativeTo
      << this->toStringFileNameNoPathSemisimpleSubalgebras()
      << "'>Semisimple subalgebras</a>.<br>";
    } else {
      out
      << "<b>Semisimple subalgebras</b>.<br>";
    }
  }
  return out.str();

}

std::string SemisimpleLieAlgebra::toHTMLCalculatorHeadElements(const std::string& relativeTo) {
  std::stringstream out;
  out << "\n<link rel='stylesheet' href='" << relativeTo << WebAPI::request::calculatorCSS << "'>";
  out << "\n<link rel='stylesheet' href='" << relativeTo << WebAPI::request::lieAlgebrasCSS << "'>";
  out << "\n<script src='" << relativeTo << WebAPI::request::onePageJS << "'></script>\n";
  return out.str();
}

std::string SemisimpleLieAlgebra::toHTMLCalculatorBodyOnload() {
  std::stringstream out;
  out << "<body onload='"
  << "window.calculator.equationEditor.typeset(document.body, null); "
  << "window.calculator.dynamicJavascript.dynamicJavascript.bootstrapAllScripts(document.body);"
  << "'>";
  return out.str();
}

std::string SemisimpleLieAlgebra::toHTMLCalculatorMainDiv() {
  return "<div class='divSemisimpleLieAlgebraStructureMainPage'>";
}

std::string SemisimpleLieAlgebra::toHTML(
  bool verbose, bool flagWriteLatexPlots
) {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::toHTMLCalculator");
  WeylGroupData& theWeyl = this->weylGroup;
  std::stringstream out;
  FormatExpressions theFormat, latexFormat;
  latexFormat.flagUseLatex = true;
  latexFormat.flagUseHTML = false;
  out << "<h1>Lie algebra " << this->toStringLieAlgebraNameFullHTML() << "</h1>";
  out << "<br>Weyl group size: " << theWeyl.theGroup.getSize().toString() << ".<br>";
  if (!verbose) {
    out << "<br>To get extra details: ";
    std::stringstream tempStream;
    tempStream << "PrintSemisimpleLieAlgebra{}(" << theWeyl.dynkinType << ")";
    out << HtmlRoutines::getCalculatorComputationAnchorNewPage(tempStream.str(), "")
    << "<br>";
  } else {
    DrawingVariables drawingVariables;
    theWeyl.drawRootSystem(drawingVariables, true, true, nullptr, true, nullptr);
    out << "A drawing of the root system in its corresponding Coxeter plane. "
    << "A basis of the plane was computed as explained by the website of John Stembridge. "
    << "<br>The darker red dots can be dragged with the mouse to rotate the picture."
    << "<br>The grey lines are the edges of the Weyl chamber.<br>"
    << drawingVariables.getHTMLDiv(theWeyl.getDimension(), true);
    out << theWeyl.toStringRootsAndRootReflections();
    out << " The resulting Lie bracket pairing table follows. <hr> "
    << this->toString(&global.theDefaultFormat.getElement());
    if (flagWriteLatexPlots) {
      out << "Ready for LaTeX consumption version of the first three columns: ";
      out << "<br>%Add to preamble: <br>\\usepackage{longtable} <br>%Add to body: <br>"
      << "\\begin{longtable}{ccc}generator & root simple coord. & root $\\varepsilon$-notation \\\\\\hline<br>\n";
      Vector<Rational> tempRoot, tempRoot2;
      ElementSemisimpleLieAlgebra<Rational> tempElt1;
      for (int i = 0; i < this->getNumberOfGenerators(); i ++) {
        tempElt1.makeGenerator(i, *this);
        tempRoot = this->getWeightOfGenerator(i);
        theWeyl.getEpsilonCoordinates(tempRoot, tempRoot2);
        out << "$" << tempElt1.toString(&theFormat) << "$&$" << tempRoot.toString() << "$";
        out << "&$" << tempRoot2.toStringLetterFormat("\\varepsilon") << "$";
        out << "\\\\\n";
      }
      out << "\\end{longtable}" << "<hr>";
    }
  }
  out << "We define the symmetric Cartan matrix <br>by requesting that the entry in the i-th row and j-th column<br> "
  << " be the scalar product of the i^th and j^th roots. The symmetric Cartan matrix is:<br>"
  << HtmlRoutines::getMathNoDisplay(theWeyl.cartanSymmetric.toString(&latexFormat));
  out << "<br>Let the (i, j)^{th} entry of the symmetric Cartan matrix be a_{ij}. "
  << "<br> Then we define the co-symmetric Cartan matrix as "
  << " the matrix whose (i, j)^{th} entry equals 4*a_{ij}/(a_{ii}*a_{jj}). "
  << "In other words, the co-symmetric Cartan matrix is the "
  << "symmetric Cartan matrix of the dual root system. The co-symmetric Cartan matrix equals:<br>"
  << HtmlRoutines::getMathNoDisplay(theWeyl.coCartanSymmetric.toStringLatex());
  out << "<br>The determinant of the symmetric Cartan matrix is: " << theWeyl.cartanSymmetric.getDeterminant().toString();
  Vectors<Rational> fundamentalWeights, fundamentalWeightsEpsForm;
  theWeyl.getFundamentalWeightsInSimpleCoordinates(fundamentalWeights);
  Vectors<Rational> simpleBasis, simplebasisEpsCoords;
  out << "<hr> Half sum of positive roots: " << theWeyl.rho.toString();
  Vector<Rational> tempRoot;
  theWeyl.getEpsilonCoordinates(theWeyl.rho, tempRoot);
  out << "= " << HtmlRoutines::getMathNoDisplay(tempRoot.toStringLetterFormat("\\varepsilon"));
  out << "<hr>The fundamental weights (the j^th fundamental weight has scalar product 1 "
  << "<br>with the j^th simple root times 2 divided by the root length squared,<br> "
  << " and 0 with the remaining simple roots): ";
  theWeyl.getEpsilonCoordinates(fundamentalWeights, fundamentalWeightsEpsForm);
  out << "<table>";
  for (int i = 0; i < fundamentalWeights.size; i ++) {
    out << "<tr><td style =\"white-space: nowrap\">" << fundamentalWeights[i].toString()
    << "</td><td> =</td><td style =\"white-space: nowrap\"> "
    << HtmlRoutines::getMathNoDisplay(fundamentalWeightsEpsForm[i].toStringEpsilonFormat())
    << "</td></tr>";
  }
  out << "</table>";
  out << "<hr>Below is the simple basis realized in epsilon coordinates. "
  << "Please note that the epsilon coordinate realizations "
  << "do not have long roots of length of 2 in types G and C. "
  << "This means that gramm matrix (w.r.t. the standard scalar product) "
  << "of the epsilon coordinate realizations in types G and C "
  << "does not equal the  corresponding symmetric Cartan matrix. "
  << "<table>";
  simpleBasis.makeEiBasis(theWeyl.getDimension());
  theWeyl.getEpsilonCoordinates(simpleBasis, simplebasisEpsCoords);
  for (int i = 0; i < simplebasisEpsCoords.size; i ++) {
    out << "<tr><td style =\"white-space: nowrap\">" << simpleBasis[i].toString()
    << " </td><td>=</td> <td style =\"white-space: nowrap\">"
    << HtmlRoutines::getMathNoDisplay(simplebasisEpsCoords[i].toStringEpsilonFormat())
    << "</td></tr>";
  }
  out << "</table>";
  DynkinSimpleType tempSimpleType;
  if (theWeyl.dynkinType.isSimple(
    &tempSimpleType.letter, &tempSimpleType.rank, &tempSimpleType.cartanSymmetricInverseScale
  )) {
    if (tempSimpleType.cartanSymmetricInverseScale == 1) {
      Matrix<Rational> tempM, tempM2;
      theWeyl.dynkinType.getEpsilonMatrix(tempM);
      tempM2 = tempM;
      tempM2.transpose();
      tempM2.multiplyOnTheRight(tempM);
      tempM2 *= 2 / tempSimpleType.getEpsilonRealizationLongRootLengthSquared();
      if (!(tempM2 == theWeyl.cartanSymmetric)) {
        global.fatal << "This is a (non-critical) programming error: "
        << "the epsilon coordinates of the vectors are incorrect. "
        << "Please fix function DynkinType::getEpsilonMatrix. "
        << "The matrix of the epsilon coordinates is " << tempM.toString()
        << ", the Symmetric Cartan matrix is "
        << theWeyl.cartanSymmetric.toString() << ", and the "
        << "transpose of the epsilon matrix times the epsilon matrix: "
        << tempM2.toString() << ". " << global.fatal;
      }
    }
  }
  return out.str();
}

void SemisimpleLieAlgebra::writeHTML(bool verbose, bool flagWriteLatexPlots) {
  std::stringstream outWithLinks;
  std::stringstream outFile;
  outFile << "<html>";
  outFile << this->toHTMLCalculatorHeadElements();
  outFile << this->toHTMLCalculatorBodyOnload();
  outFile << this->toStringHTMLMenuStructureSummary("", false, true, true, true);
  outFile << this->toHTMLCalculatorMainDiv();
  outFile << this->toHTML(verbose, flagWriteLatexPlots);
  outFile << "</div>";
  outFile << "</body>";
  outFile << "</html>";
  FileOperations::writeFileVirual(
    this->toStringVirtualFileNameWithPathStructureConstants(),
    outFile.str(),
    &outFile
  );
}

bool DynkinType::hasPrecomputedSubalgebras() const {
  if (!this->isSimple()) {
    return false;
  }
  return this->getSmallestSimpleType().hasPrecomputedSubalgebras();
}

std::string DynkinType::toStringVirtualNameFolder() const {
  std::stringstream out;
  out << "/output/semisimple_lie_algebras/"
  << FileOperations::cleanUpForFileNameUse(this->toString()) << "/";
  return out.str();
}

std::string SemisimpleLieAlgebra::toStringFileNameRelativePathSlTwoSubalgebras() const {
  std::stringstream out;
  out << "sl2s/sl2s_"
  << FileOperations::cleanUpForFileNameUse(this->weylGroup.dynkinType.toString())
  << ".html";
  return out.str();
}

std::string SemisimpleLieAlgebra::toStringFileNameNoPathSemisimpleSubalgebras() const {
  std::stringstream theFileName;
  theFileName << "semisimple_subalgebras_"
  << FileOperations::cleanUpForFileNameUse(this->weylGroup.dynkinType.toString())
  << ".html";
  return theFileName.str();
}

std::string SemisimpleLieAlgebra::toStringFileNameNoPathRootSubalgebras() const {
  std::stringstream theFileName;
  theFileName << "rootSubalgebras_"
  << FileOperations::cleanUpForFileNameUse(this->weylGroup.dynkinType.toString())
  << ".html";
  return theFileName.str();
}

std::string SemisimpleLieAlgebra::toStringVirtualFolderName() const {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::toStringVirtualFolderName");
  this->checkConsistency();
  return this->weylGroup.dynkinType.toStringVirtualNameFolder();
}

std::string SemisimpleLieAlgebra::toStringVirtualFileNameWithPathStructureConstants() const {
  std::stringstream fileName;
  fileName
  << this->toStringVirtualFolderName()
  << this->toStringFileNameNoPathStructureConstants();
  return fileName.str();
}

std::string SemisimpleLieAlgebra::toStringDisplayFolderNamE(const std::string& baseFolder) const {
  std::stringstream out;
  out << baseFolder << "semisimple_lie_algebras/"
  << FileOperations::cleanUpForFileNameUse(this->weylGroup.dynkinType.toString())
  << "/";
  return out.str();
}

std::string SemisimpleLieAlgebra::toStringFileNameNoPathStructureConstants() const {
  std::stringstream fileName;
  fileName
  << "structure_constants_notation_"
  << FileOperations::cleanUpForFileNameUse(this->weylGroup.dynkinType.toString())
  << ".html";
  return fileName.str();
}

std::string SemisimpleLieAlgebra::toStringDisplayFileNameWithPathStructureConstants(const std::string& baseFolder) const {
  std::stringstream fileName;
  fileName
  << this->toStringDisplayFolderNamE(baseFolder)
  << this->toStringFileNameNoPathStructureConstants();
  return fileName.str();
}

void SemisimpleLieAlgebra::computeChevalleyConstants() {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::computeChevalleyConstants");
  this->weylGroup.computeRho(true);
  this->ChevalleyConstants.initialize(this->weylGroup.rootSystem.size, this->weylGroup.rootSystem.size);
  this->Computed.initialize(this->weylGroup.rootSystem.size, this->weylGroup.rootSystem.size);
  this->Computed.makeZero(false);
  Selection nonExploredRoots;
  this->flagAnErrorHasOccurredTimeToPanic = false;
  Vectors<Rational>& posRoots = this->weylGroup.rootsOfBorel;
  nonExploredRoots.makeFullSelection(posRoots.size);
  Vector<Rational> tempRoot;
  std::stringstream out;
  ProgressReport theReport;
  double startTimer = - 1;
  if (theReport.tickAndWantReport()) {
    out << "Initializing matrix for structure constant computation of " << this->toStringLieAlgebraName() << "... ";
    startTimer = global.getElapsedSeconds();
    theReport.report(out.str());
  }
  for (int i = 0; i < this->weylGroup.rootSystem.size; i ++) {
    for (int j = i; j < this->weylGroup.rootSystem.size; j ++) {
      tempRoot = this->weylGroup.rootSystem[i] + this->weylGroup.rootSystem[j];
      if (!tempRoot.isEqualToZero()) {
        if (!this->weylGroup.isARoot(tempRoot)) {
          this->Computed.elements[i][j] = true;
          this->ChevalleyConstants.elements[i][j].makeZero();
          this->Computed.elements[j][i] = true;
          this->ChevalleyConstants.elements[j][i].makeZero();
        }
      }
    }
  }
  double startStructureConstantComputation = - 1;
  if (theReport.tickAndWantReport()) {
    out << "done in " << global.getElapsedSeconds() - startTimer
    << " seconds.<br> " << "Computing structure constants...";
    theReport.report(out.str());
    startStructureConstantComputation = global.getElapsedSeconds();
  }
  Rational tempRat;
  while (nonExploredRoots.cardinalitySelection > 0) {
    int theBorelIndex = nonExploredRoots.elements[0];
    Rational theHeight = posRoots[theBorelIndex].sumCoordinates();
    for (int i = 1; i < nonExploredRoots.cardinalitySelection; i ++) {
      tempRat = posRoots[nonExploredRoots.elements[i]].sumCoordinates();
      if (theHeight.isGreaterThan(tempRat)) {
        theHeight = tempRat;
        theBorelIndex = nonExploredRoots.elements[i];
      }
    }
    Vector<Rational>& theRoot = posRoots[theBorelIndex];
    int theIndex = this->weylGroup.rootSystem.getIndex(theRoot);
    Vector<Rational> smallRoot2;
    int FirstIndexFirstPosChoice = - 1;
    int SecondIndexFirstPosChoice = - 1;
    Rational CurrentHeight;
    for (int i = 0; i < this->weylGroup.rootsOfBorel.size; i ++) {
      Vector<Rational>& smallRoot1 = this->weylGroup.rootsOfBorel[i];
      CurrentHeight = smallRoot1.sumCoordinates();
      int FirstPosIndex = this->weylGroup.rootSystem.getIndex(smallRoot1);
      int FirstNegIndex = this->weylGroup.rootSystem.getIndex(- smallRoot1);
      if (theHeight.isGreaterThan(CurrentHeight)) {
        smallRoot2 = theRoot - smallRoot1;
        int SecondPosIndex = this->weylGroup.rootSystem.getIndex(smallRoot2);
        if (FirstPosIndex<SecondPosIndex) {
          int SecondNegIndex = this->weylGroup.rootSystem.getIndex(- smallRoot2);
          if (FirstIndexFirstPosChoice == - 1) {
            FirstIndexFirstPosChoice = FirstPosIndex;
            SecondIndexFirstPosChoice = SecondPosIndex;
            int thePower;
            this->getMaxQForWhichBetaMinusQAlphaisARoot(smallRoot1, smallRoot2, thePower);
            this->ChevalleyConstants.elements[FirstNegIndex][SecondNegIndex] = - 1 - thePower;
            this->Computed.elements[FirstNegIndex][SecondNegIndex] = true;
          } else {
            this->computeOneChevalleyConstant(
              FirstIndexFirstPosChoice, SecondIndexFirstPosChoice, FirstNegIndex, SecondNegIndex, theIndex
            );
          }
          this->exploitSymmetryAndCyclicityChevalleyConstants(FirstNegIndex, SecondNegIndex);
        }
      }
    }
    nonExploredRoots.selected[theBorelIndex] = false;
    nonExploredRoots.computeIndicesFromSelection();
  }
  double startMultTable = - 1;
  if (theReport.tickAndWantReport()) {
    out << " done in " << global.getElapsedSeconds() - startStructureConstantComputation
    << " seconds.<br> Computing Lie bracket pairing (``multiplication'') table...";
    theReport.report(out.str());
    startMultTable = global.getElapsedSeconds();
  }
  this->computeLieBracketTable();
  if (theReport.tickAndWantReport()) {
    out << " done in " << global.getElapsedSeconds() - startMultTable
    << " seconds. Total structure constant computation time: "
    << global.getElapsedSeconds() - startTimer << " seconds. ";
    theReport.report(out.str());
  }
  if (this->getNumberOfPositiveRoots() <= 0) {
    global.fatal << "Number of positive roots of a "
    << "semisimple Lie algebra is reported to be zero. " << global.fatal;
  }
}

void SemisimpleLieAlgebra::computeLieBracketTable() {
  int numPosRoots = this->weylGroup.rootsOfBorel.size;
  int theRank = this->weylGroup.cartanSymmetric.numberOfRows;
  int numRoots = numPosRoots * 2;
  int numGenerators = numRoots + theRank;
  this->theLiebrackets.initialize(numGenerators, numGenerators);
  //  this->theLiebracketPairingIndices.initialize(numGenerators, numGenerators);
  this->UEGeneratorOrderIncludingCartanElts.initializeFillInObject(numGenerators, - 1);
  //  this->theLiebracketPairingIndices.makeZero(- 1);
  //  this->OppositeRootSpaces.initializeFillInObject(numRoots+theDimension, - 1);
  Vector<Rational> leftWeight, rightWeight, hRoot;
  for (int i = 0; i < numGenerators; i ++) {
    leftWeight = this->getWeightOfGenerator(i);
    for (int j = i; j < numGenerators; j ++) {
      rightWeight = this->getWeightOfGenerator(j);
      if (leftWeight.isEqualToZero() && rightWeight.isEqualToZero()) {
        this->theLiebrackets.elements[i][j].makeZero();
        continue;
      }
      if (leftWeight.isEqualToZero() && !rightWeight.isEqualToZero()) {
        this->theLiebrackets.elements[i][j].makeGenerator(j, *this);
        hRoot.makeEi(theRank, i - numPosRoots);
        this->theLiebrackets.elements[i][j] *= Vector<Rational>::scalarProduct(
          hRoot, rightWeight, this->weylGroup.cartanSymmetric
        );
        continue;
      }
      if (!leftWeight.isEqualToZero() && rightWeight.isEqualToZero()) {
        this->theLiebrackets.elements[i][j].makeGenerator(i, *this);
        hRoot.makeEi(theRank, j - numPosRoots);
        this->theLiebrackets.elements[i][j] *= - Vector<Rational>::scalarProduct(
          hRoot, leftWeight, this->weylGroup.cartanSymmetric
        );
        continue;
      }
      if (!leftWeight.isEqualToZero() && !rightWeight.isEqualToZero()) {
        int newIndex = this->getGeneratorFromRoot(leftWeight + rightWeight);
        if (newIndex != - 1) {
          this->theLiebrackets.elements[i][j].makeGenerator(newIndex, *this);
          int leftIndex = this->getRootIndexFromGenerator(i);
          int rightIndex = this->getRootIndexFromGenerator(j);
          this->theLiebrackets.elements[i][j] *= this->ChevalleyConstants.elements[leftIndex][rightIndex];
        } else {
          if (!(leftWeight +rightWeight).isEqualToZero()) {
            this->theLiebrackets.elements[i][j].makeZero();
          } else {
            ElementSemisimpleLieAlgebra<Rational>& current = this->theLiebrackets.elements[i][j];
            current.makeCartanGenerator(leftWeight * 2 / (this->weylGroup.rootScalarCartanRoot(leftWeight, leftWeight)), *this);
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

void SemisimpleLieAlgebra::exploitSymmetryAndCyclicityChevalleyConstants(int indexI, int indexJ) {
  const Vector<Rational>& rootI = this->weylGroup.rootSystem[indexI];
  const Vector<Rational>& rootJ = this->weylGroup.rootSystem[indexJ];
  if ((rootI + rootJ).isEqualToZero())
    global.fatal << "Sum or roots not allowed to be zero here. " << global.fatal;
  //int indexMinusI = this->theWeyl.RootSystem.getIndex(-rootI);
  //int indexMinusJ = this->theWeyl.RootSystem.getIndex(-rootJ);
  //this->ComputeDebugString();
  this->exploitSymmetryChevalleyConstants(indexI, indexJ);
  //this->ComputeDebugString();
  //int indexRootIPlusRootJ = this->theWeyl.RootSystem.getIndex(rootI + rootJ);
  int indexMinusIMinusJ = this->weylGroup.rootSystem.getIndex(- rootI - rootJ);
  this->exploitTheCyclicTrick(indexI, indexJ, indexMinusIMinusJ);
  //this->ComputeDebugString();
  //this->exploitTheCyclicTrick(indexMinusI, indexMinusJ, indexRootIPlusRootJ);
  //this->ComputeDebugString();
}

void SemisimpleLieAlgebra::exploitSymmetryChevalleyConstants(int indexI, int indexJ) {
  const Vector<Rational>& rootI = this->weylGroup.rootSystem[indexI];
  const Vector<Rational>& rootJ = this->weylGroup.rootSystem[indexJ];
  if (!this->Computed.elements[indexI][indexJ]) {
    global.fatal << "Structure constants computed in a wrong order. " << global.fatal;
  }
  int indexMinusI = this->weylGroup.rootSystem.getIndex(- rootI);
  int indexMinusJ = this->weylGroup.rootSystem.getIndex(- rootJ);
  this->ChevalleyConstants.elements[indexJ][indexI] = (this->ChevalleyConstants.elements[indexI][indexJ] * (- 1));
  this->Computed.elements[indexJ][indexI] = true;
  if ((rootI + rootJ).isEqualToZero()) {
    global.fatal << "Bad root sum. " << global.fatal;
  }
  int thePower;
  this->getMaxQForWhichBetaMinusQAlphaisARoot(
    this->weylGroup.rootSystem[indexMinusI], this->weylGroup.rootSystem[indexMinusJ], thePower
  );
  int i = 1 + thePower;
  this->ChevalleyConstants.elements[indexMinusI][indexMinusJ] = - i * i;
  this->ChevalleyConstants.elements[indexMinusI][indexMinusJ] /= this->ChevalleyConstants.elements[indexI][indexJ];
  this->Computed.elements[indexMinusI][indexMinusJ] = true;
  this->ChevalleyConstants.elements[indexMinusJ][indexMinusI] = this->ChevalleyConstants.elements[indexMinusI][indexMinusJ] * (- 1);
  this->Computed.elements[indexMinusJ][indexMinusI] = true;
}

void SemisimpleLieAlgebra::exploitTheCyclicTrick(int i, int j, int k) {
  const Vector<Rational>& rootI = this->weylGroup.rootSystem[i];
  const Vector<Rational>& rootK = this->weylGroup.rootSystem[k];
  const Vector<Rational>& rootJ = this->weylGroup.rootSystem[j];
  //the following three checks can be commented out to increase speed. They have never failed so far.
  if (!(rootI + rootK + rootJ).isEqualToZero()) {
    global.fatal << "Three roots do not sum to zero as required. " << global.fatal;
  }
  if (!this->Computed.elements[i][j]) {
    global.fatal << "Bad structure constant computation order." << global.fatal;
  }
  /////////////////////////////////////////////////////////////////
  Rational& tempRat = this->ChevalleyConstants.elements[i][j];
  Rational tempRat2 = this->weylGroup.rootScalarCartanRoot(rootK, rootK);
  this->ChevalleyConstants.elements[j][k] = (tempRat * this->weylGroup.rootScalarCartanRoot(rootI, rootI)) / tempRat2;
  this->Computed.elements[j][k] = true;
  this->ChevalleyConstants.elements[k][i] = (tempRat * this->weylGroup.rootScalarCartanRoot(rootJ, rootJ)) / tempRat2;
  this->Computed.elements[k][i] = true;
  this->exploitSymmetryChevalleyConstants(j, k);
  this->exploitSymmetryChevalleyConstants(k, i);
}

bool SemisimpleLieAlgebra::getMaxQForWhichBetaMinusQAlphaisARoot(
  const Vector<Rational>& alpha, const Vector<Rational>& beta, int& output
) const {
  output = - 1;
  Vector<Rational> tempRoot = beta;
  if (alpha.isEqualToZero()) {
    global.fatal << "Calling function "
    << "getMaxQForWhichBetaMinusQAlphaisARoot with zero value for alpha is not allowed. " << global.fatal;
  }
  bool foundRoot = false;
  while (this->weylGroup.isARoot(tempRoot)) {
    output ++;
    tempRoot -= alpha;
    foundRoot = true;
  }
  return foundRoot;
}

void SemisimpleLieAlgebra::computeOneChevalleyConstant(
  int indexGamma, int indexDelta, int indexMinusEpsilon, int indexMinusZeta, int indexEta
) {
 //using formula (**), 2.9, page 49, Samelson, Notes on Lie algebras, 1989
  const Vector<Rational>& gamma = this->weylGroup.rootSystem[indexGamma];
  const Vector<Rational>& delta = this->weylGroup.rootSystem[indexDelta];
  const Vector<Rational>& minusEpsilon = this->weylGroup.rootSystem[indexMinusEpsilon];
  const Vector<Rational>& eta = this->weylGroup.rootSystem[indexEta];
  const Vector<Rational>& minusZeta = this->weylGroup.rootSystem[indexMinusZeta];
  if (eta != gamma + delta) {
    global.fatal << "Eta must equal gamma plus delta. " << global.fatal;
  }
  if (!this->weylGroup.isARoot(eta + minusEpsilon)) {
    global.fatal << "Eta minus epsilon must be a root. " << global.fatal;
  }
  if (
    !this->Computed.elements[indexDelta][indexMinusEpsilon] ||
    !this->Computed.elements[indexMinusEpsilon][indexGamma] ||
    !this->Computed.elements[indexGamma][indexDelta]
  ) {
    global.fatal << "Bad structure constant computation order. " << global.fatal;
  }
  if (this->ChevalleyConstants.elements[indexGamma][indexDelta].isEqualToZero()) {
    global.fatal << "Unexpected zero structure constants" << global.fatal;
  }
  int indexDeltaMinusEpsilon = this->weylGroup.rootSystem.getIndex(delta + minusEpsilon);
  int indexGammaMinusEpsilon = this->weylGroup.rootSystem.getIndex(gamma + minusEpsilon);
  Rational FirstSummand, SecondSummand;
  if (indexDeltaMinusEpsilon != - 1) {
    if (
      !this->Computed.elements[indexGamma][indexDeltaMinusEpsilon] ||
      !this->Computed.elements[indexDelta][indexMinusEpsilon]
    ) {
      global.fatal << "Structure constants must be computed at this point. " << global.fatal;
    }
    FirstSummand =
    this->ChevalleyConstants.elements[indexGamma][indexDeltaMinusEpsilon] *
    this->ChevalleyConstants.elements[indexDelta][indexMinusEpsilon];
  } else {
    FirstSummand.makeZero();
  }
  if (indexGammaMinusEpsilon != - 1) {
    if (
      !this->Computed.elements[indexDelta][indexGammaMinusEpsilon] ||
      !this->Computed.elements[indexMinusEpsilon][indexGamma]
    ) {
      global.fatal << "Structure constants must be computed at this point. " << global.fatal;
    }
    SecondSummand = this->ChevalleyConstants.elements[indexDelta][indexGammaMinusEpsilon] *
    this->ChevalleyConstants.elements[indexMinusEpsilon][indexGamma];
  } else {
    SecondSummand.makeZero();
  }
  this->ChevalleyConstants.elements[indexMinusEpsilon][indexMinusZeta] = (
    this->weylGroup.rootScalarCartanRoot(eta, eta) /
    this->weylGroup.rootScalarCartanRoot(minusZeta, minusZeta)
  ) * (FirstSummand + SecondSummand) / this->ChevalleyConstants.elements[indexGamma][indexDelta];
  this->Computed.elements[indexMinusEpsilon][indexMinusZeta] = true;
}

bool SemisimpleLieAlgebra::testForConsistency() {
  //HashedList<Vector<Rational> >& theRoots = this->theWeyl.RootSystem;
  FormatExpressions& theFormat = global.theDefaultFormat.getElement();
  ElementSemisimpleLieAlgebra<Rational> g1, g2, g3, g23, g31, g12, g123, g231, g312, temp;
  //this->ComputeDebugString(false, false, global);
  for (int i = 0; i < this->getNumberOfGenerators(); i ++) {
    g1.makeGenerator(i, *this);
    for (int j = 0; j < this->getNumberOfGenerators(); j ++) {
      g2.makeGenerator(j, *this);
      for (int k = 0; k < this->getNumberOfGenerators(); k ++) {
        g3.makeGenerator(k, *this);
        this->lieBracket(g2, g3, g23);
        this->lieBracket(g1, g23, g123);
        this->lieBracket(g3, g1, g31);
        this->lieBracket(g2, g31, g231);
        this->lieBracket(g1, g2, g12);
        this->lieBracket(g3, g12, g312);
        temp = g123;
        temp += g231;
        temp += g312;
        if (!temp.isEqualToZero()) {
          global.fatal
          << "The computed structure constants are wrong: the Jacobi identity fails. "
          << "More precisely, I get that "
          << "<br>[" << g1.toString(&theFormat) << ", " << g2.toString(&theFormat) << "] =" << g12.toString(&theFormat)
          << "<br>[" << g2.toString(&theFormat) << ", " << g3.toString(&theFormat) << "] =" << g23.toString(&theFormat)
          << "<br>[" << g3.toString(&theFormat) << ", " << g1.toString(&theFormat) << "] =" << g31.toString(&theFormat)
          << "<br>g123= " << g123.toString(&theFormat)
          << "<br>g231=" << g231.toString(&theFormat)
          << "<br>g312=" << g312.toString(&theFormat) << "<br>"
          << global.fatal;
          return false;
        }
        this->makeChevalleyTestReport(i, j, k, this->getNumberOfGenerators());
      }
    }
  }
  return true;
}

Rational SemisimpleLieAlgebra::getConstant(const Vector<Rational>& root1, const Vector<Rational>& root2) {
  int index1 = this->weylGroup.rootSystem.getIndex(root1);
  int index2 = this->weylGroup.rootSystem.getIndex(root2);
  if (index1 == - 1 || index2 == - 1) {
    return 0;
  }
  return this->ChevalleyConstants(index1, index2);
}

bool SemisimpleLieAlgebra::getConstantOrHElement(
  const Vector<Rational>& root1, const Vector<Rational>& root2, Rational& outputRat, Vector<Rational>& outputH
) {
  if (!(root1 + root2).isEqualToZero()) {
    outputRat = this->getConstant(root1, root2);
    return true;
  }
  if (this->weylGroup.rootSystem.getIndex(root1) == - 1 || this->weylGroup.rootSystem.getIndex(root2) == - 1) {
    outputRat.makeZero();
    return true;
  }
  outputH = (root1 * 2) / Vector<Rational>::scalarProduct(root1, root1, this->weylGroup.cartanSymmetric);
  return false;
}

void SemisimpleLieAlgebra::makeChevalleyTestReport(int i, int j, int k, int Total) {
  if (!global.response.reportDesired()) {
    return;
  }
  std::stringstream out2, out3;
  int x = i * Total * Total + j * Total + k + 1;
  out2 << "i: " << i + 1 << " of " << Total << " j: " << j + 1 << " of "
  << Total << " k: " << k + 1 << " of " << Total;
  out3 << "Total progress: " << x << " out of " << (Total * Total * Total);
  ProgressReport theReport;
  theReport.report(out2.str() + out3.str());
}

void SemisimpleLieAlgebra::computeOneAutomorphism(Matrix<Rational>& outputAuto, bool useNegativeRootsFirst) {
  global.fatal << "Not implemented yet!!!!!" << global.fatal;
  RootSubalgebra theRootSA;
//  theRootSA.initialize(*this);
  int theDimension = this->weylGroup.cartanSymmetric.numberOfRows;
  theRootSA.genK.makeEiBasis(theDimension);
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms theAutos;
  theRootSA.generateAutomorphismsPreservingBorel(theAutos);
  Matrix<Rational> mapOnRootSpaces;
  int theAutoIndex = theAutos.ExternalAutomorphisms.size > 1 ? 1 : 0;
  /*if (this->theWeyl.WeylLetter == 'D' && theDimension ==4)
    theAutoIndex =2;
*/
  mapOnRootSpaces.assignVectorsToRows(theAutos.ExternalAutomorphisms[theAutoIndex]);
  mapOnRootSpaces.transpose();
//  mapOnRootSpaces.ComputeDebugString();
//  Matrix<Rational>  theDet =mapOnRootSpaces;
//  Rational tempRat;
//  theDet.computeDeterminantOverwriteMatrix(tempRat);
  Selection NonExplored;
  int numRoots = this->weylGroup.rootSystem.size;
  NonExplored.initialize(numRoots);
  NonExplored.makeFullSelection();
  Vector<Rational> domainRoot, rangeRoot;

  this->computeChevalleyConstants();
  List<ElementSemisimpleLieAlgebra<Rational> > Domain, Range;
  Range.setSize(numRoots + theDimension);
  Domain.setSize(numRoots + theDimension);
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  for (int i = 0; i < theDimension; i ++) {
    domainRoot.makeEi(theDimension, i);
    mapOnRootSpaces.actOnVectorColumn(domainRoot, rangeRoot);
    tempElt.makeCartanGenerator(domainRoot, *this);
    Domain[numRoots + i] = tempElt;
    tempElt.makeCartanGenerator(rangeRoot, *this);
    Range[numRoots + i] = tempElt;
    for (int i = 0; i < 2; i ++, domainRoot.negate(), rangeRoot.negate()) {
      int theIndex = this->weylGroup.rootSystem.getIndex(rangeRoot);
      tempElt.makeGGenerator(rangeRoot, *this);
      Range[theIndex] = tempElt;
      tempElt.makeGGenerator(domainRoot, *this);
      Domain[theIndex] = tempElt;
      NonExplored.removeSelection(theIndex);
    }
  }
  Vector<Rational> left, right;
  while (NonExplored.cardinalitySelection > 0) {
    for (int i = 0; i < NonExplored.cardinalitySelection; i ++) {
      int theIndex = NonExplored.elements[i];
      const Vector<Rational>& current = this->weylGroup.rootSystem[theIndex];
      for (int j = 0; j < theDimension; j ++) {
        left.makeEi(theDimension, j);
        for (int k = 0; k < 2; k ++, left.negate()) {
          right = current - left;
          if (this->weylGroup.isARoot(right)) {
            int leftIndex = this->weylGroup.rootSystem.getIndex(left);
            int rightIndex = this->weylGroup.rootSystem.getIndex(right);
            if (!NonExplored.selected[rightIndex]) {
              ElementSemisimpleLieAlgebra<Rational>& leftDomainElt = Domain[leftIndex];
              ElementSemisimpleLieAlgebra<Rational>& rightDomainElt = Domain[rightIndex];
              this->lieBracket(leftDomainElt, rightDomainElt, Domain[theIndex]);
              ElementSemisimpleLieAlgebra<Rational>& leftRangeElt = Range[leftIndex];
              ElementSemisimpleLieAlgebra<Rational>& rightRangeElt = Range[rightIndex];
              this->lieBracket(leftRangeElt, rightRangeElt, Range[theIndex]);
              NonExplored.removeSelection(theIndex);
            }
          }
        }
      }
    }
  }
  Vectors<Rational> vectorsLeft, vectorsRight;
  vectorsLeft.setSize(Range.size);
  vectorsRight.setSize(Range.size);
  if (!useNegativeRootsFirst) {
    for (int i = 0; i < Range.size; i ++) {
      Range[i].elementToVectorNegativeRootSpacesFirst(vectorsRight[i]);
      Domain[i].elementToVectorNegativeRootSpacesFirst(vectorsLeft[i]);
    }
  } else {
    for (int i = 0; i < Range.size; i ++) {
      Range[i].elementToVectorNegativeRootSpacesFirst(vectorsRight[i]);
      Domain[i].elementToVectorNegativeRootSpacesFirst(vectorsLeft[i]);
    }
  }
  outputAuto.makeLinearOperatorFromDomainAndRange(vectorsLeft, vectorsRight);
}

bool SemisimpleLieAlgebra::isInTheWeightSupport(Vector<Rational>& theWeight, Vector<Rational>& highestWeight) {
  Vector<Rational> correspondingDominant = theWeight;
  this->weylGroup.raiseToDominantWeight(correspondingDominant);
  Vector<Rational> theDiff = highestWeight - correspondingDominant;
  if (!theDiff.isPositiveOrZero())
    return false;
  return true;
}

void SemisimpleLieAlgebra::createEmbeddingFromFDModuleHaving1dimWeightSpaces(Vector<Rational>& theHighestWeight) {
  (void) theHighestWeight;
  /*Vectors<Rational> weightSupport;
  this->GenerateWeightSupport(theHighestWeight, weightSupport);
  int highestWeight, distanceToHW;
  this->EmbeddingsRootSpaces.setSize(this->theWeyl.RootSystem.size);
  int theDimension = this->theWeyl.cartanSymmetric.numberOfRows;
  List<bool> Explored;
  Explored.initializeFillInObject(this->theWeyl.RootSystem.size, false);
  int numExplored = 0;
  for (int i = 0; i < this->theWeyl.RootSystem.size; i ++) {
    Vector<Rational>& current = this->theWeyl.RootSystem.objects[i];
    if (current.SumCoordinates() ==1 || current.SumCoordinates() == - 1) {
      numExplored++;
      Explored.objects[i] = true;
      Matrix<Rational> & currentMat = this->EmbeddingsRootSpaces.objects[i];
      currentMat.initialize(weightSupport.size, weightSupport.size);
      currentMat.makeZero();
      for (int j = 0; j<weightSupport.size; j ++) {
        int indexTarget = weightSupport.getIndex(current +weightSupport.objects[j]);
        if (indexTarget != - 1) {
          highestWeight = - 1+ this->getLengthStringAlongAlphaThroughBeta(current, weightSupport.objects[j], distanceToHW, weightSupport);
          if (current.isNegativeOrZero())
            currentMat.elements[indexTarget][j] =1;
          else
            currentMat.elements[indexTarget][j] =(highestWeight-distanceToHW+ 1)*distanceToHW;
        }
      }
    }
  }
  Vectors<Rational> simpleBasis;
  simpleBasis.makeEiBasis(theDimension);
  while (numExplored< this->theWeyl.RootSystem.size) {
    for (int i = 0; i < this->theWeyl.RootSystem.size; i ++)
      if (Explored.objects[i])
        for (int j = 0; j < this->theWeyl.RootSystem.size; j ++)
          if (Explored.objects[j]) {
            Vector<Rational> tempRoot = this->theWeyl.RootSystem.objects[i] + this->theWeyl.RootSystem.objects[j];
            if (this->theWeyl.isARoot(tempRoot)) {
              int index = this->theWeyl.RootSystem.getIndex(tempRoot);
              if (!Explored.objects[index]) {
                Explored.objects[index] = true;
                numExplored++;
                this->EmbeddingsRootSpaces.objects[index] = this->EmbeddingsRootSpaces.objects[i];
                this->EmbeddingsRootSpaces.objects[index].lieBracketWith(this->EmbeddingsRootSpaces.objects[j]);
              }
            }
          }
  }
  this->EmbeddingsCartan.setSize(theDimension);
  for (int i = 0; i < theDimension; i ++) {
    Matrix<Rational> & current = this->EmbeddingsCartan.objects[i];
    current.initialize(weightSupport.size, weightSupport.size);
    current.makeZero();
    Vector<Rational> tempRoot;
    tempRoot.makeEi(theDimension, i);
    for (int j = 0; j<weightSupport.size; j ++)
      current.elements[j][j] = this->theWeyl.rootScalarCartanRoot(tempRoot, weightSupport.objects[j]);
  }*/
}

int SemisimpleLieAlgebra::getLengthStringAlongAlphaThroughBeta(Vector<Rational>& alpha, Vector<Rational>& beta, int& distanceToHighestWeight, Vectors<Rational>& weightSupport) {
  Vector<Rational> tempRoot = beta;
  for (int i = 0; ; i ++) {
    tempRoot += alpha;
    if (!weightSupport.contains(tempRoot)) {
      distanceToHighestWeight = i;
      break;
    }
  }
  for (int i = 0; ; i ++) {
    tempRoot -= alpha;
    if (!weightSupport.contains(tempRoot)) {
      return i;
    }
  }
//  global.fatal << global.fatal;
//  return - 1;
}

bool HomomorphismSemisimpleLieAlgebra::computeHomomorphismFromImagesSimpleChevalleyGenerators() {
  MacroRegisterFunctionWithName("HomomorphismSemisimpleLieAlgebra::computeHomomorphismFromImagesSimpleChevalleyGenerators");
  this->theDomain().computeChevalleyConstants();
  this->theRange().computeChevalleyConstants();
  int theDomainDimension = this->theDomain().weylGroup.cartanSymmetric.numberOfRows;
  Selection NonExplored;
  int numRoots = this->theDomain().weylGroup.rootSystem.size;
  NonExplored.initialize(numRoots);
  NonExplored.makeFullSelection();
  List<ElementSemisimpleLieAlgebra<Rational> > tempDomain, tempRange;
  tempDomain.setSize(numRoots+theDomainDimension);
  tempRange.setSize(numRoots+theDomainDimension);
  Vector<Rational> tempRoot;
  for (int i = 0; i < theDomainDimension; i ++) {
    tempRoot.makeEi(theDomainDimension, i);
    for (int j = 0; j < 2; j ++, tempRoot.negate()) {
      int index = this->theDomain().weylGroup.rootSystem.getIndex(tempRoot);
      tempDomain[index].makeZero();
      ChevalleyGenerator tempGen;
      tempGen.makeGenerator(this->theDomain(), this->theDomain().getGeneratorFromRoot(tempRoot));
      tempDomain[index].addMonomial(tempGen, 1);
      tempRange[index] = this->imagesSimpleChevalleyGenerators[i +j*theDomainDimension];
      NonExplored.removeSelection(index);
    }
  }
  Vector<Rational> right;
  while (NonExplored.cardinalitySelection > 0) {
    for (int i = 0; i < NonExplored.cardinalitySelection; i ++) {
      int theIndex = NonExplored.elements[i];
      const Vector<Rational>& current = this->theDomain().weylGroup.rootSystem[theIndex];
      for (int j = 0; j < NonExplored.numberOfElements; j ++) {
        if (!NonExplored.selected[j]) {
          const Vector<Rational>& left = this->theDomain().weylGroup.rootSystem[j];
          right = current - left;
          if (this->theDomain().weylGroup.isARoot(right)) {
            int leftIndex = this->theDomain().weylGroup.rootSystem.getIndex(left);
            int rightIndex = this->theDomain().weylGroup.rootSystem.getIndex(right);
            if (!NonExplored.selected[rightIndex]) {
              ElementSemisimpleLieAlgebra<Rational>& leftDomainElt = tempDomain[leftIndex];
              ElementSemisimpleLieAlgebra<Rational>& rightDomainElt = tempDomain[rightIndex];
              this->theDomain().lieBracket(leftDomainElt, rightDomainElt, tempDomain[theIndex]);
              ElementSemisimpleLieAlgebra<Rational>& leftRangeElt = tempRange[leftIndex];
              ElementSemisimpleLieAlgebra<Rational>& rightRangeElt = tempRange[rightIndex];
              this->theRange().lieBracket(leftRangeElt, rightRangeElt, tempRange[theIndex]);
              NonExplored.removeSelection(theIndex);
              break;
            }
          }
        }
      }
    }
  }
  for (int i = 0; i < theDomainDimension; i ++) {
    tempRoot.makeEi(theDomainDimension, i);
    int leftIndex = this->theDomain().weylGroup.rootSystem.getIndex(tempRoot);
    int rightIndex = this->theDomain().weylGroup.rootSystem.getIndex(- tempRoot);
    this->theDomain().lieBracket(tempDomain[leftIndex], tempDomain[rightIndex], tempDomain[numRoots + i]);
    this->theRange().lieBracket(tempRange[leftIndex], tempRange[rightIndex], tempRange[numRoots + i]);
  }
  Vectors<Rational> vectorsLeft, vectorsRight;
  vectorsLeft.setSize(tempDomain.size);
  vectorsRight.setSize(tempDomain.size);
  for (int i = 0; i < tempRange.size; i ++) {
    tempDomain[i].elementToVectorNegativeRootSpacesFirst(vectorsLeft[i]);
    tempRange[i].elementToVectorNegativeRootSpacesFirst(vectorsRight[i]);
  }
  Matrix<Rational> tempMat;
  tempMat.makeLinearOperatorFromDomainAndRange(vectorsLeft, vectorsRight);
  Vector<Rational> imageRoot;
  this->domainAllChevalleyGenerators.setSize(tempDomain.size);
  this->imagesAllChevalleyGenerators.setSize(tempDomain.size);
  for (int i = 0; i < this->theDomain().getNumberOfGenerators(); i ++) {
    this->domainAllChevalleyGenerators[i].makeGenerator(i, this->theDomain());
  }
  for (int i = 0; i < this->imagesAllChevalleyGenerators.size; i ++) {
    this->domainAllChevalleyGenerators[i].elementToVectorNegativeRootSpacesFirst(tempRoot);
    tempMat.actOnVectorColumn(tempRoot, imageRoot);
    this->imagesAllChevalleyGenerators[i].assignVectorNegRootSpacesCartanPosRootSpaces(imageRoot, this->theRange());
  }
  return true;
}

void HomomorphismSemisimpleLieAlgebra::projectOntoSmallCartan(Vectors<Rational>& input, Vectors<Rational>& output) {
  output.setSize(input.size);
  for (int i = 0; i < input.size; i ++) {
    this->projectOntoSmallCartan(input[i], output[i]);
  }
}

void HomomorphismSemisimpleLieAlgebra::projectOntoSmallCartan(Vector<Rational>& input, Vector<Rational>& output) {
  Matrix<Rational> invertedSmallCartan;
  invertedSmallCartan = this->theDomain().weylGroup.cartanSymmetric;
  invertedSmallCartan.invert();
  int theSmallDimension = this->theDomain().weylGroup.cartanSymmetric.numberOfRows;
  output.makeZero(theSmallDimension);
  for (int i = 0; i < theSmallDimension; i ++) {
    output[i] = this->theRange().weylGroup.rootScalarCartanRoot(
      this->imagesAllChevalleyGenerators[this->theDomain().weylGroup.rootsOfBorel.size + i].getCartanPart(), input
    );
  }
  invertedSmallCartan.actOnVectorColumn(output, output);
}

bool HomomorphismSemisimpleLieAlgebra::applyHomomorphism(
  const MonomialUniversalEnveloping<RationalFunction<Rational> >& input,
  const RationalFunction<Rational>& theCoeff,
  ElementUniversalEnveloping<RationalFunction<Rational> >& output
) {
  ElementUniversalEnveloping<RationalFunction<Rational> > tempElt;
  output.makeZero(this->theRange());
  RationalFunction<Rational> polyOne;
  polyOne.makeOne();
  output.makeConstant(theCoeff, this->theRange());
  for (int i = 0; i < input.generatorsIndices.size; i ++) {
    if (input.generatorsIndices[i] >= this->imagesAllChevalleyGenerators.size) {
      return false;
    }
    tempElt.assignElementLieAlgebra(
      this->imagesAllChevalleyGenerators[input.generatorsIndices[i]],
      this->theRange(),
      polyOne
    );
    RationalFunction<Rational>& thePower = input.powers[i];
    int theIntegralPower;
    if (!thePower.isSmallInteger(&theIntegralPower)) {
      return false;
    }
    for (int j = 0; j < theIntegralPower; j ++) {
      output *= tempElt;
    }
  }
  return true;
}

void HomomorphismSemisimpleLieAlgebra::applyHomomorphism(
  const ElementSemisimpleLieAlgebra<Rational>& input,
  ElementSemisimpleLieAlgebra<Rational>& output
) {
  if (&output == &input) {
    global.fatal << "Output required to be different from input. " << global.fatal;
  }
  output.makeZero();
  for (int i = 0; i < input.size(); i ++) {
    int currentIndex = input[i].generatorIndex;
    output += this->imagesAllChevalleyGenerators[currentIndex] * input.coefficients[i];
  }
}

void HomomorphismSemisimpleLieAlgebra::getMapSmallCartanDualToLargeCartanDual(Matrix<Rational>& output) {
  output.initialize(this->theRange().getRank(), this->theDomain().getRank());
  ElementSemisimpleLieAlgebra<Rational> domainElt, imageElt;
  for (int i = 0; i < this->theDomain().getRank(); i ++) {
    domainElt.makeCartanGenerator(Vector<Rational>::getEi(this->theDomain().getRank(), i), this->theDomain());
    this->applyHomomorphism(domainElt, imageElt);
    output.assignVectorToColumnKeepOtherColsIntactNoInit(i, imageElt.getCartanPart());
  }
}

bool HomomorphismSemisimpleLieAlgebra::applyHomomorphism(
  const ElementUniversalEnveloping<RationalFunction<Rational> >& input,
  ElementUniversalEnveloping<RationalFunction<Rational> >& output
) {
  if (&output == &input) {
    global.fatal << "Output must be different from input. " << global.fatal;
  }
  output.makeZero(this->theRange());
  ElementUniversalEnveloping<RationalFunction<Rational> > tempElt;
  for (int i = 0; i < input.size(); i ++) {
    if (!this->applyHomomorphism(input[i], input.coefficients[i], tempElt)) {
      return false;
    }
    output += tempElt;
  }
  return true;
}

void HomomorphismSemisimpleLieAlgebra::makeGinGWithIdentity(
  char theWeylLetter, int theWeylDim, MapReferences<DynkinType, SemisimpleLieAlgebra>& ownerOfAlgebras
) {
  MacroRegisterFunctionWithName("HomomorphismSemisimpleLieAlgebra::makeGinGWithIdentity");
  DynkinType theType;
  theType.makeSimpleType(theWeylLetter, theWeylDim);
  this->domainAlg = &ownerOfAlgebras.getValueCreateNoInit(theType);
  this->rangeAlg = this->domainAlg;
  this->domainAlg->weylGroup.makeArbitrarySimple(theWeylLetter, theWeylDim);
  this->theDomain().computeChevalleyConstants();
  int numPosRoots = this->theDomain().weylGroup.rootsOfBorel.size;
  this->imagesAllChevalleyGenerators.setSize(numPosRoots * 2 + theWeylDim);
  this->domainAllChevalleyGenerators.setSize(numPosRoots * 2 + theWeylDim);
  this->imagesSimpleChevalleyGenerators.setSize(theWeylDim * 2);
  for (int i = 0; i < 2 * numPosRoots + theWeylDim; i ++) {
    ElementSemisimpleLieAlgebra<Rational>& tempElt1 = this->imagesAllChevalleyGenerators[i];
    ElementSemisimpleLieAlgebra<Rational>& tempElt2 = this->domainAllChevalleyGenerators[i];
    tempElt2.makeGenerator(i, this->theDomain());
    tempElt1.makeGenerator(i, this->theRange());
  }
  for (int i = 0; i < theWeylDim; i ++) {
    ElementSemisimpleLieAlgebra<Rational>& tempElt1 = this->imagesSimpleChevalleyGenerators[i];
    tempElt1.makeGenerator(i, this->theRange());
    ElementSemisimpleLieAlgebra<Rational>& tempElt2 = this->imagesSimpleChevalleyGenerators[theWeylDim + i];
    tempElt2.makeGenerator(i + numPosRoots, this->theRange());
  }
}

void HomomorphismSemisimpleLieAlgebra::toString(std::string& output, bool useHtml) {
  std::stringstream out;
  if (this->checkClosednessLieBracket()) {
    out << "Lie bracket closes, everything is good!";
  } else {
    out << "The Lie bracket is incorrect!";
  }
  if (useHtml) {
    out << "<br>";
  }
  out << "Images simple Chevalley generators:\n\n";
  if (useHtml) {
    out << "<br>";
  }
  for (int i = 0; i < this->imagesSimpleChevalleyGenerators.size; i ++) {
    out << this->imagesSimpleChevalleyGenerators[i].toString(&global.theDefaultFormat.getElement()) << "\n\n";
    if (useHtml) {
      out << "<br>";
    }
  }
  out << "Maps of Chevalley generators:\n\n";
  for (int i = 0; i < this->domainAllChevalleyGenerators.size; i ++) {
    out << "<br>" << this->domainAllChevalleyGenerators[i].toString(&global.theDefaultFormat.getElement())
    << " \\mapsto " << this->imagesAllChevalleyGenerators[i].toString(&global.theDefaultFormat.getElement());
  }
  output = out.str();
}

class SlTwoInSlN;

void HomomorphismSemisimpleLieAlgebra::getRestrictionAmbientRootSystemToTheSmallercartanSubalgebra(Vectors<Rational>& output) {
  List<Vector<Rational> >& theRootSystem= this->theRange().weylGroup.rootSystem;
  int rankSA = this->theDomain().weylGroup.getDimension();
  Matrix<Rational> tempMat;
  tempMat = this->theDomain().weylGroup.cartanSymmetric;
  tempMat.invert();
  int numPosRootsDomain = this->theDomain().weylGroup.rootsOfBorel.size;
  output.setSize(theRootSystem.size);
  Vector<Rational> theScalarProducts;
  theScalarProducts.setSize(rankSA);
  for (int i = 0; i < theRootSystem.size; i ++) {
    for (int j = 0; j < rankSA; j ++) {
      ElementSemisimpleLieAlgebra<Rational>& currentH = this->imagesAllChevalleyGenerators[j + numPosRootsDomain];
      theScalarProducts[j] = this->theRange().weylGroup.rootScalarCartanRoot(currentH.getCartanPart(), theRootSystem[i]);
    }
    tempMat.actOnVectorColumn(theScalarProducts, output[i]);
  }
  this->ImagesCartanDomain.setSize(rankSA);
  for (int i = 0; i < rankSA; i ++) {
    this->ImagesCartanDomain[i] = this->imagesAllChevalleyGenerators[i +numPosRootsDomain].getCartanPart();
  }
}

bool HomomorphismSemisimpleLieAlgebra::checkClosednessLieBracket() {
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  Vectors<Rational> tempRoots;
  Vector<Rational> tempRoot;
  tempRoots.setSize(this->imagesAllChevalleyGenerators.size);
  for (int i = 0; i < tempRoots.size; i ++) {
    this->imagesAllChevalleyGenerators[i].elementToVectorNegativeRootSpacesFirst(tempRoots[i]);
  }
  for (int i = 0; i < this->imagesAllChevalleyGenerators.size; i ++) {
    for (int j = 0; j < this->imagesAllChevalleyGenerators.size; j ++) {
      this->theRange().lieBracket(this->imagesAllChevalleyGenerators[i], this->imagesAllChevalleyGenerators[j], tempElt);
      tempElt.elementToVectorNegativeRootSpacesFirst(tempRoot);
      if (!tempRoots.linearSpanContainsVector(tempRoot)) {
        return false;
      }
    }
  }
  return true;
}

void ChevalleyGenerator::checkConsistencyWithOther(const ChevalleyGenerator& other) const {
  if (this->owner != other.owner) {
    global.fatal << "Attempt to compare Chevalley generators of different Lie algebras. " << global.fatal;
  }
}

bool ChevalleyGenerator::checkInitialization() const {
  if (this->owner == nullptr) {
    global.fatal << "Attempt to use a non-initialized Chevalley generator. " << global.fatal;
    return false;
  }
  return true;
}

std::string ChevalleyGenerator::toString(FormatExpressions* inputFormat) const {
  this->checkInitialization();
  return this->owner->getStringFromChevalleyGenerator(this->generatorIndex, inputFormat);
}

bool ChevalleyGenerator::operator>(const ChevalleyGenerator& other) const {
  return this->generatorIndex>other.generatorIndex;
}

std::string SemisimpleLieAlgebra::getStringFromChevalleyGenerator(
  int theIndex, FormatExpressions* thePolynomialFormat
) const {
  std::stringstream out;
  MemorySaving<FormatExpressions> tempFormat;
  if (thePolynomialFormat == nullptr) {
    thePolynomialFormat = &tempFormat.getElement();
  }
  if (this->isGeneratorFromCartan(theIndex)) {
    out << thePolynomialFormat->chevalleyHgeneratorLetter << "_{" << theIndex - this->getNumberOfPositiveRoots() + 1 << "}";
  } else {
    out << thePolynomialFormat->chevalleyGgeneratorLetter << "_{";
    if (theIndex >= this->getNumberOfPositiveRoots()) {
      out << theIndex - this->getNumberOfPositiveRoots() - this->getRank() + 1;
    } else {
      out << theIndex - this->getNumberOfPositiveRoots();
    }
    out << "}";
  }
  return out.str();
}

void SemisimpleLieAlgebra::orderNilradicalFirstTotalWeightDescending(const Selection& parSelZeroMeansLeviPart) {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::orderNilradicalFirstTotalWeightDescending");
  Vector<Rational> tempVect;
  tempVect = parSelZeroMeansLeviPart;
  for (int i = 0; i < this->getNumberOfGenerators(); i ++) {
    if (this->getWeightOfGenerator(i).scalarEuclidean(tempVect) < 0) {
      this->UEGeneratorOrderIncludingCartanElts[i] = - i - this->getNumberOfGenerators() * 5;
    }
  }
}

void SemisimpleLieAlgebra::orderNilradicalFirstTotalWeightAscending(const Selection& parSelZeroMeansLeviPart) {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::orderNilradicalFirstTotalWeightDescending");
  Vector<Rational> tempVect;
  tempVect = parSelZeroMeansLeviPart;
  for (int i = 0; i < this->getNumberOfGenerators(); i ++) {
    if (this->getWeightOfGenerator(i).scalarEuclidean(tempVect) < 0) {
      this->UEGeneratorOrderIncludingCartanElts[i] = i - this->getNumberOfGenerators() * 5;
    }
  }
}

void SemisimpleLieAlgebra::orderNilradicalNilWeightAscending(const Selection& parSelZeroMeansLeviPart) {
  Vector<Rational> tempVect;
  tempVect = parSelZeroMeansLeviPart;
  for (int i = 0; i < this->getNumberOfGenerators(); i ++) {
    Rational translationCoeff = this->getWeightOfGenerator(i).scalarEuclidean(tempVect) * this->getNumberOfPositiveRoots();
    if (translationCoeff < 0) {
      this->UEGeneratorOrderIncludingCartanElts[i] = i + translationCoeff.numeratorShort * this->getNumberOfGenerators() * 5;
    }
  }
}

void SemisimpleLieAlgebra::orderNilradicalNilWeightDescending(const Selection& parSelZeroMeansLeviPart) {
  Vector<Rational> tempVect;
  tempVect = parSelZeroMeansLeviPart;
  for (int i = 0; i < this->getNumberOfGenerators(); i ++) {
    Rational translationCoeff = this->getWeightOfGenerator(i).scalarEuclidean(tempVect) * this->getNumberOfPositiveRoots();
    if (translationCoeff < 0) {
      this->UEGeneratorOrderIncludingCartanElts[i] = - i + translationCoeff.numeratorShort * this->getNumberOfGenerators() * 5;
    }
  }
}

void SemisimpleLieAlgebra::orderNilradical(const Selection& parSelZeroMeansLeviPart, bool useNilWeight, bool ascending) {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::orderNilradical");
  if (useNilWeight) {
    if (ascending) {
      this->orderNilradicalNilWeightAscending(parSelZeroMeansLeviPart);
    } else {
      this->orderNilradicalNilWeightDescending(parSelZeroMeansLeviPart);
    }
  } else {
    if (ascending) {
      this->orderNilradicalFirstTotalWeightAscending(parSelZeroMeansLeviPart);
    } else {
      this->orderNilradicalFirstTotalWeightDescending(parSelZeroMeansLeviPart);
    }
  }
}

bool SemisimpleLieAlgebra::hasComputedStructureConstants() {
  return this->ChevalleyConstants.numberOfRows == this->weylGroup.dynkinType.getRootSystemSize();
}

void SemisimpleLieAlgebra::orderSSalgebraForHWbfComputation() {
  int numPosRoots = this->getNumberOfPositiveRoots();
  for (int i = 0; i < numPosRoots; i ++) {
    this->UEGeneratorOrderIncludingCartanElts[i] = - 1;
  }
}

void SemisimpleLieAlgebra::orderStandardAscending() {
  int numGens = this->getNumberOfGenerators();
  for (int i = 0; i < numGens; i ++) {
    this->UEGeneratorOrderIncludingCartanElts[i] = i;
  }
}

void SemisimpleLieAlgebra::orderStandardDescending() {
  int numGens = this->getNumberOfGenerators();
  for (int i = 0; i < numGens; i ++) {
    this->UEGeneratorOrderIncludingCartanElts[i] = numGens - i - 1;
  }
}

bool SemisimpleLieAlgebra::areOrderedProperly(int leftIndex, int rightIndex) {
  return this->UEGeneratorOrderIncludingCartanElts[leftIndex] <= this->UEGeneratorOrderIncludingCartanElts[rightIndex];
}

int SemisimpleLieAlgebra::getRootIndexFromDisplayIndex(int theIndex) {
  int numPosRoots = this->weylGroup.rootsOfBorel.size;
  if (theIndex < 0) {
    return theIndex + numPosRoots;
  }
  if (theIndex > 0) {
    return theIndex + numPosRoots - 1;
  }
  return - 1;
}

int SemisimpleLieAlgebra::GetDisplayIndexFromRootIndex(int theIndex) const {
  int numPosRoots = this->weylGroup.rootsOfBorel.size;
  if (theIndex >= numPosRoots) {
    return theIndex - numPosRoots + 1;
  }
  if (theIndex < numPosRoots) {
    return theIndex - numPosRoots;
  }
  return - 10000000;
}

int SemisimpleLieAlgebra::getGeneratorFromRootIndex(int theIndex) const {
  if (theIndex < 0 || theIndex >= this->weylGroup.rootSystem.size) {
    return - 1;
  }
  int theDimension = this->weylGroup.cartanSymmetric.numberOfRows;
  int numPosRoots = this->weylGroup.rootsOfBorel.size;
  if (theIndex >= numPosRoots) {
    return theIndex + theDimension;
  }
  return theIndex;
}

int SemisimpleLieAlgebra::getRootIndexFromGenerator(int theIndex) const {
  int numPosRoots = this->weylGroup.rootsOfBorel.size;
  int theDimension = this->weylGroup.cartanSymmetric.numberOfRows;
  if (theIndex < numPosRoots) {
    return theIndex;
  }
  if (theIndex >= numPosRoots + theDimension) {
    return theIndex - theDimension;
  }
  return - 1;
}

bool SemisimpleLieAlgebra::getElementStandardRepresentation(
  const ElementSemisimpleLieAlgebra<Rational>& element,
  Matrix<Rational>& output
) {
  return false;

}
