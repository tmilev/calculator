// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_extra_semisimple_Lie_algebras_implementation.h"
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_extra_drawing_variables.h"

std::string SemisimpleLieAlgebra::toString(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::toString");
  std::stringstream out;
  std::string tempS;
  Vector<Rational> tempRoot, tempRoot2;
  int numRoots = this->theWeyl.RootSystem.size;
  int theDimension = this->theWeyl.cartanSymmetric.numberOfRows;
  ElementSemisimpleLieAlgebra<Rational> tempElt1, tempElt2, tempElt3;
//  out << beginMath << "\\begin{array}{ccc}a& a&a\\\\a&a&a\\end{array}";
  std::string hLetter = "h";
  std::string gLetter = "g";
  if (theFormat != nullptr) {
    hLetter = theFormat->chevalleyHgeneratorLetter;
    gLetter = theFormat->chevalleyGgeneratorLetter;
  }
  out << "Type " << this->theWeyl.theDynkinType.toString() << ".";
  out << "The letter " << HtmlRoutines::getMathSpanPure(hLetter)
  << " stands for elements of the Cartan subalgebra, <br>"
  << " the letter " << HtmlRoutines::getMathSpanPure(gLetter)
  << " stands for the Chevalley (root space) generators of non-zero weight. <br>"
  << " The generator " << HtmlRoutines::getMathSpanPure(hLetter + "_i")
  << " is the element of the Cartan subalgebra dual to the <br>"
  << "i^th simple root, that is, "
  << HtmlRoutines::getMathSpanPure("[" + hLetter + "_i, g] =\\langle \\alpha_i , \\gamma\\rangle g")
  << ", <br> where g is a Chevalley generator, " << HtmlRoutines::getMathSpanPure("\\gamma")
  << " is its weight, and <br>"
  << HtmlRoutines::getMathSpanPure("\\alpha_i") << " is the i^th simple root. ";
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
    tempS = tempElt1.toString(theFormat);
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
    this->theWeyl.getEpsilonCoordinates(tempRoot, tempRoot2);
    theTableLateXStream << tempRoot2.ToStringLetterFormat("\\varepsilon") << "&";
    theHtmlStream << "<td>" << tempRoot2.ToStringLetterFormat("e") << "</td>";
    tempElt1.makeGenerator(i, *this);
    tempS = tempElt1.toString(theFormat);
    theTableLateXStream << tempS;
    theHtmlStream << "<td>" << tempS << "</td>";
    for (int j = 0; j < numRoots + theDimension; j ++) {
      tempElt2.makeGenerator(j, *this);
      this->lieBracket(tempElt1, tempElt2, tempElt3);
      tempS = tempElt3.toString(theFormat);
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
    out << "<div class =\"math\">" << theTableLateXStream.str() << "</div>";
    return out.str();
  }
  out << "<br><b> The Lie bracket table is too large to be rendered in LaTeX, displaying in "
  << "html format instead.</b> ";
  if (theFormat != nullptr) {
    if (theFormat->flagLatexDetailsInHtml) {
      out << "Below you can find the same table in pure LaTeX, "
      << "which you can render in a separate LaTeX session, should "
      << "wish to do so on your own. ";
    }
  }
  out << theHtmlStream.str();
  if (theFormat != nullptr) {
    if (theFormat->flagLatexDetailsInHtml) {
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
  if (this->theWeyl.theDynkinType.HasPrecomputedSubalgebras()) {
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

std::string SemisimpleLieAlgebra::toHTMLCalculator(
  bool Verbose, bool writeToHD, bool flagWriteLatexPlots
) {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::toHTMLCalculator");
  WeylGroupData& theWeyl = this->theWeyl;
  std::stringstream out;
  FormatExpressions theFormat, latexFormat;
  latexFormat.flagUseLatex = true;
  latexFormat.flagUseHTML = false;
  out << "<h1>Lie algebra " << this->toStringLieAlgebraNameFullHTML() << "</h1>";
  out << "<br>Weyl group size: " << theWeyl.theGroup.getSize().toString() << ".<br>";
  if (!Verbose) {
    out << "<br>To get extra details: ";
    std::stringstream tempStream;
    tempStream << "PrintSemisimpleLieAlgebra{}(" << theWeyl.theDynkinType << ")";
    out << HtmlRoutines::getCalculatorComputationAnchor(tempStream.str()) << "<br>";
  } else {
    DrawingVariables theDV;
    theWeyl.drawRootSystem(theDV, true, true, nullptr, true, nullptr);
    out << "A drawing of the root system in its corresponding Coxeter plane. "
    << "A basis of the plane was computed as explained by the website of John Stembridge. "
    << "<br>The darker red dots can be dragged with the mouse to rotate the picture."
    << "<br>The grey lines are the edges of the Weyl chamber.<br>"
    << theDV.getHTMLDiv(theWeyl.getDimension());
    out << theWeyl.ToStringRootsAndRootReflections();
    out << " The resulting Lie bracket pairing table follows. <hr> "
    << this->toString(&global.theDefaultFormat.getElement());
    if (flagWriteLatexPlots) {
      out << "Ready for LaTeX consumption version of the first three columns: ";
      out << "<br>%Add to preamble: <br>\\usepackage{longtable} <br>%Add to body: <br>"
      << " \\begin{longtable}{ccc}generator & root simple coord. & root $\\varepsilon$-notation \\\\\\hline<br>\n";
      Vector<Rational> tempRoot, tempRoot2;
      ElementSemisimpleLieAlgebra<Rational> tempElt1;
      for (int i = 0; i < this->getNumberOfGenerators(); i ++) {
        tempElt1.makeGenerator(i, *this);
        tempRoot = this->getWeightOfGenerator(i);
        theWeyl.getEpsilonCoordinates(tempRoot, tempRoot2);
        out << "$" << tempElt1.toString(&theFormat) << "$&$" << tempRoot.toString() << "$";
        out << "&$" << tempRoot2.ToStringLetterFormat("\\varepsilon") << "$";
        out << "\\\\\n";
      }
      out << "\\end{longtable}" << "<hr>";
    }
  }
  out << "We define the symmetric Cartan matrix <br>by requesting that the entry in the i-th row and j-th column<br> "
  << " be the scalar product of the i^th and j^th roots. The symmetric Cartan matrix is:<br>"
  << HtmlRoutines::getMathSpanPure(theWeyl.cartanSymmetric.toString(&latexFormat));
  out << "<br>Let the (i, j)^{th} entry of the symmetric Cartan matrix be a_{ij}. "
  << "<br> Then we define the co-symmetric Cartan matrix as "
  << " the matrix whose (i, j)^{th} entry equals 4*a_{ij}/(a_{ii}*a_{jj}). "
  << "In other words, the co-symmetric Cartan matrix is the "
  << "symmetric Cartan matrix of the dual root system. The co-symmetric Cartan matrix equals:<br>"
  << HtmlRoutines::getMathSpanPure(theWeyl.coCartanSymmetric.ToStringLatex());
  out << "<br>The determinant of the symmetric Cartan matrix is: " << theWeyl.cartanSymmetric.getDeterminant().toString();
  /*  Rational theRatio;
    for (int j = 0; j < theWeyl.getDimension(); j ++) {
      theRatio = 0;
      for (int i = 0; i < theWeyl.RootSystem.size; i ++) {
        Rational tempRat = theWeyl.RootScalarCartanRoot(theWeyl.RootSystem[i], theWeyl.RootSystem[j]);
        theRatio+= tempRat*tempRat;
      }
      theRatio.invert();
      theRatio*= theWeyl.RootScalarCartanRoot(theWeyl.RootSystem[j], theWeyl.RootSystem[j]);
      Rational tempRat = theWeyl.GetKillingDivTraceRatio();
      tempRat.invert();
    }*/
  //Lattice tempLattice;
  //theWeyl.GetIntegralLatticeInSimpleCoordinates(tempLattice);
  //out << "<br>The integral lattice in simple coordinates is (generated by): " << tempLattice.toString(true, false);
  //Vectors<Rational> integralRoots, integralRootsEpsForm;
  Vectors<Rational> fundamentalWeights, fundamentalWeightsEpsForm;
  //theWeyl.getEpsilonCoordinates(integralRoots, integralRootsEpsForm);
  //out << "<br>The integral lattice generators in epsilon format: " << integralRootsEpsForm.ElementToStringEpsilonForm();
  theWeyl.GetFundamentalWeightsInSimpleCoordinates(fundamentalWeights);
  Vectors<Rational> simpleBasis, simplebasisEpsCoords;
  out << "<hr> Half sum of positive roots: " << theWeyl.rho.toString();
  Vector<Rational> tempRoot;
  theWeyl.getEpsilonCoordinates(theWeyl.rho, tempRoot);
  out << "= " << HtmlRoutines::getMathSpanPure(tempRoot.ToStringLetterFormat("\\varepsilon"));
  out << "<hr>The fundamental weights (the j^th fundamental weight has scalar product 1 "
  << "<br>with the j^th simple root times 2 divided by the root length squared,<br> "
  << " and 0 with the remaining simple roots): ";
  theWeyl.getEpsilonCoordinates(fundamentalWeights, fundamentalWeightsEpsForm);
  out << "<table>";
  for (int i = 0; i < fundamentalWeights.size; i ++) {
    out << "<tr><td style =\"white-space: nowrap\">" << fundamentalWeights[i].toString()
    << "</td><td> =</td><td style =\"white-space: nowrap\"> "
    << HtmlRoutines::getMathSpanPure(fundamentalWeightsEpsForm[i].ToStringEpsilonFormat())
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
    << HtmlRoutines::getMathSpanPure(simplebasisEpsCoords[i].ToStringEpsilonFormat())
    << "</td></tr>";
  }
  out << "</table>";
  DynkinSimpleType tempSimpleType;
  if (theWeyl.theDynkinType.IsSimple(
    &tempSimpleType.theLetter, &tempSimpleType.theRank, &tempSimpleType.CartanSymmetricInverseScale
  )) {
    if (tempSimpleType.CartanSymmetricInverseScale == 1) {
      Matrix<Rational> tempM, tempM2;
      theWeyl.theDynkinType.getEpsilonMatrix(tempM);
      tempM2 = tempM;
      tempM2.transpose();
      tempM2.multiplyOnTheRight(tempM);
      tempM2 *= 2 / tempSimpleType.GetEpsilonRealizationLongRootLengthSquared();
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
  if (writeToHD) {
    std::stringstream outWithLinks;
    std::stringstream outFile;
    outFile << "<html>";
    outFile << HtmlRoutines::getCSSLinkLieAlgebrasAndCalculator("../../../");
    outFile << HtmlRoutines::getJavascriptMathjax("../../../");
    outFile << HtmlRoutines::getJavascriptLinkGraphicsNDimensionsWithPanels("../../../");
    outFile << "<body>";
    outFile << this->toStringHTMLMenuStructureSummary("", false, true, true, true);
    outFile << out.str();
    outFile << "</body>";
    outFile << "</html>";
    outWithLinks
    << FileOperations::writeFileReturnHTMLLink(
      outFile.str(), this->toStringVirtualFileNameWithPathStructureConstants(), "hard drive output"
    )
    << "<br>" << out.str();
    return outWithLinks.str();
  }
  return out.str();
}

bool DynkinType::HasPrecomputedSubalgebras() const {
  if (!this->IsSimple()) {
    return false;
  }
  return this->GetSmallestSimpleType().HasPrecomputedSubalgebras();
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
  << FileOperations::cleanUpForFileNameUse(this->theWeyl.theDynkinType.toString())
  << ".html";
  return out.str();
}

std::string SemisimpleLieAlgebra::toStringFileNameNoPathSemisimpleSubalgebras() const {
  std::stringstream theFileName;
  theFileName << "semisimple_subalgebras_"
  << FileOperations::cleanUpForFileNameUse(this->theWeyl.theDynkinType.toString())
  << ".html";
  return theFileName.str();
}

std::string SemisimpleLieAlgebra::toStringFileNameNoPathRootSubalgebras() const {
  std::stringstream theFileName;
  theFileName << "rootSubalgebras_"
  << FileOperations::cleanUpForFileNameUse(this->theWeyl.theDynkinType.toString())
  << ".html";
  return theFileName.str();
}

std::string SemisimpleLieAlgebra::toStringVirtualFolderName() const {
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::toStringVirtualFolderName");
  this->checkConsistency();
  return this->theWeyl.theDynkinType.toStringVirtualNameFolder();
}

std::string SemisimpleLieAlgebra::toStringVirtualFileNameWithPathStructureConstants() const {
  std::stringstream theFileName;
  theFileName
  << this->toStringVirtualFolderName()
  << this->toStringFileNameNoPathStructureConstants();
  return theFileName.str();
}

std::string SemisimpleLieAlgebra::toStringDisplayFolderName(const std::string& baseFolder) const {
  std::stringstream out;
  out << baseFolder << "output/semisimple_lie_algebras/"
  << FileOperations::cleanUpForFileNameUse(this->theWeyl.theDynkinType.toString())
  << "/";
  return out.str();
}

std::string SemisimpleLieAlgebra::toStringFileNameNoPathStructureConstants() const {
  std::stringstream theFileName;
  theFileName
  << "structure_constants_notation_"
  << FileOperations::cleanUpForFileNameUse(this->theWeyl.theDynkinType.toString())
  << ".html";
  return theFileName.str();
}

std::string SemisimpleLieAlgebra::toStringDisplayFileNameWithPathStructureConstants(const std::string& baseFolder) const {
  std::stringstream theFileName;
  theFileName
  << this->toStringDisplayFolderName(baseFolder)
  << this->toStringFileNameNoPathStructureConstants();
  return theFileName.str();
}

void SemisimpleLieAlgebra::computeChevalleyConstants() {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::computeChevalleyConstants");
  this->theWeyl.ComputeRho(true);
  this->ChevalleyConstants.initialize(this->theWeyl.RootSystem.size, this->theWeyl.RootSystem.size);
  this->Computed.initialize(this->theWeyl.RootSystem.size, this->theWeyl.RootSystem.size);
  this->Computed.makeZero(false);
  Selection nonExploredRoots;
  this->flagAnErrorHasOccurredTimeToPanic = false;
  Vectors<Rational>& posRoots = this->theWeyl.RootsOfBorel;
  nonExploredRoots.MakeFullSelection(posRoots.size);
  Vector<Rational> tempRoot;
  std::stringstream out;
  ProgressReport theReport;
  double startTimer = - 1;
  if (theReport.tickAndWantReport()) {
    out << "Initializing matrix for structure constant computation of " << this->toStringLieAlgebraName() << "... ";
    startTimer = global.getElapsedSeconds();
    theReport.report(out.str());
  }
  for (int i = 0; i < this->theWeyl.RootSystem.size; i ++) {
    for (int j = i; j < this->theWeyl.RootSystem.size; j ++) {
      tempRoot = this->theWeyl.RootSystem[i] + this->theWeyl.RootSystem[j];
      if (!tempRoot.isEqualToZero()) {
        if (!this->theWeyl.isARoot(tempRoot)) {
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
    Rational theHeight = posRoots[theBorelIndex].SumCoords();
    for (int i = 1; i < nonExploredRoots.cardinalitySelection; i ++) {
      tempRat = posRoots[nonExploredRoots.elements[i]].SumCoords();
      if (theHeight.IsGreaterThan(tempRat)) {
        theHeight = tempRat;
        theBorelIndex = nonExploredRoots.elements[i];
      }
    }
    Vector<Rational>& theRoot = posRoots[theBorelIndex];
    int theIndex = this->theWeyl.RootSystem.getIndex(theRoot);
    Vector<Rational> smallRoot2;
    int FirstIndexFirstPosChoice = - 1;
    int SecondIndexFirstPosChoice = - 1;
    Rational CurrentHeight;
    for (int i = 0; i < this->theWeyl.RootsOfBorel.size; i ++) {
      Vector<Rational>& smallRoot1 = this->theWeyl.RootsOfBorel[i];
      CurrentHeight = smallRoot1.SumCoords();
      int FirstPosIndex = this->theWeyl.RootSystem.getIndex(smallRoot1);
      int FirstNegIndex = this->theWeyl.RootSystem.getIndex(- smallRoot1);
      if (theHeight.IsGreaterThan(CurrentHeight)) {
        smallRoot2 = theRoot - smallRoot1;
        int SecondPosIndex = this->theWeyl.RootSystem.getIndex(smallRoot2);
        if (FirstPosIndex<SecondPosIndex) {
          int SecondNegIndex = this->theWeyl.RootSystem.getIndex(- smallRoot2);
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
    global.fatal << "This is a programming error: number of positive roots of a "
    << "semisimple Lie algebra is reported to be zero. " << global.fatal;
  }
}

void SemisimpleLieAlgebra::computeLieBracketTable() {
  int numPosRoots = this->theWeyl.RootsOfBorel.size;
  int theRank = this->theWeyl.cartanSymmetric.numberOfRows;
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
          hRoot, rightWeight, this->theWeyl.cartanSymmetric
        );
        continue;
      }
      if (!leftWeight.isEqualToZero() && rightWeight.isEqualToZero()) {
        this->theLiebrackets.elements[i][j].makeGenerator(i, *this);
        hRoot.makeEi(theRank, j - numPosRoots);
        this->theLiebrackets.elements[i][j] *= - Vector<Rational>::scalarProduct(
          hRoot, leftWeight, this->theWeyl.cartanSymmetric
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
            current.makeCartanGenerator(leftWeight * 2 / (this->theWeyl.RootScalarCartanRoot(leftWeight, leftWeight)), *this);
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
  const Vector<Rational>& rootI = this->theWeyl.RootSystem[indexI];
  const Vector<Rational>& rootJ = this->theWeyl.RootSystem[indexJ];
  if ((rootI + rootJ).isEqualToZero())
    global.fatal << "Sum or roots not allowed to be zero here. " << global.fatal;
  //int indexMinusI = this->theWeyl.RootSystem.getIndex(-rootI);
  //int indexMinusJ = this->theWeyl.RootSystem.getIndex(-rootJ);
  //this->ComputeDebugString();
  this->exploitSymmetryChevalleyConstants(indexI, indexJ);
  //this->ComputeDebugString();
  //int indexRootIPlusRootJ = this->theWeyl.RootSystem.getIndex(rootI + rootJ);
  int indexMinusIMinusJ = this->theWeyl.RootSystem.getIndex(- rootI - rootJ);
  this->exploitTheCyclicTrick(indexI, indexJ, indexMinusIMinusJ);
  //this->ComputeDebugString();
  //this->exploitTheCyclicTrick(indexMinusI, indexMinusJ, indexRootIPlusRootJ);
  //this->ComputeDebugString();
}

void SemisimpleLieAlgebra::exploitSymmetryChevalleyConstants(int indexI, int indexJ) {
  const Vector<Rational>& rootI = this->theWeyl.RootSystem[indexI];
  const Vector<Rational>& rootJ = this->theWeyl.RootSystem[indexJ];
  if (!this->Computed.elements[indexI][indexJ]) {
    global.fatal << "Structure constants computed in a wrong order. " << global.fatal;
  }
  int indexMinusI = this->theWeyl.RootSystem.getIndex(- rootI);
  int indexMinusJ = this->theWeyl.RootSystem.getIndex(- rootJ);
  this->ChevalleyConstants.elements[indexJ][indexI] = (this->ChevalleyConstants.elements[indexI][indexJ] * (- 1));
  this->Computed.elements[indexJ][indexI] = true;
  if ((rootI + rootJ).isEqualToZero()) {
    global.fatal << "Bad root sum. " << global.fatal;
  }
  int thePower;
  this->getMaxQForWhichBetaMinusQAlphaisARoot(
    this->theWeyl.RootSystem[indexMinusI], this->theWeyl.RootSystem[indexMinusJ], thePower
  );
  int i = 1 + thePower;
  this->ChevalleyConstants.elements[indexMinusI][indexMinusJ] = - i * i;
  this->ChevalleyConstants.elements[indexMinusI][indexMinusJ] /= this->ChevalleyConstants.elements[indexI][indexJ];
  this->Computed.elements[indexMinusI][indexMinusJ] = true;
  this->ChevalleyConstants.elements[indexMinusJ][indexMinusI] = this->ChevalleyConstants.elements[indexMinusI][indexMinusJ] * (- 1);
  this->Computed.elements[indexMinusJ][indexMinusI] = true;
}

void SemisimpleLieAlgebra::exploitTheCyclicTrick(int i, int j, int k) {
  const Vector<Rational>& rootI = this->theWeyl.RootSystem[i];
  const Vector<Rational>& rootK = this->theWeyl.RootSystem[k];
  const Vector<Rational>& rootJ = this->theWeyl.RootSystem[j];
  //the following three checks can be commented out to increase speed. They have never failed so far.
  if (!(rootI + rootK + rootJ).isEqualToZero()) {
    global.fatal << "Three roots do not sum to zero as required. " << global.fatal;
  }
  if (!this->Computed.elements[i][j]) {
    global.fatal << "Bad structure constant computation order." << global.fatal;
  }
  /////////////////////////////////////////////////////////////////
  Rational& tempRat = this->ChevalleyConstants.elements[i][j];
  Rational tempRat2 = this->theWeyl.RootScalarCartanRoot(rootK, rootK);
  this->ChevalleyConstants.elements[j][k] = (tempRat * this->theWeyl.RootScalarCartanRoot(rootI, rootI)) / tempRat2;
  this->Computed.elements[j][k] = true;
  this->ChevalleyConstants.elements[k][i] = (tempRat * this->theWeyl.RootScalarCartanRoot(rootJ, rootJ)) / tempRat2;
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
    global.fatal << "This is a programming error: calling function "
    << "getMaxQForWhichBetaMinusQAlphaisARoot with zero value for alpha is not allowed. " << global.fatal;
  }
  bool foundRoot = false;
  while (this->theWeyl.isARoot(tempRoot)) {
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
  const Vector<Rational>& gamma = this->theWeyl.RootSystem[indexGamma];
  const Vector<Rational>& delta = this->theWeyl.RootSystem[indexDelta];
  const Vector<Rational>& minusEpsilon = this->theWeyl.RootSystem[indexMinusEpsilon];
  const Vector<Rational>& eta = this->theWeyl.RootSystem[indexEta];
  const Vector<Rational>& minusZeta = this->theWeyl.RootSystem[indexMinusZeta];
  if (eta != gamma + delta) {
    global.fatal << "Eta must equal gamma plus delta. " << global.fatal;
  }
  if (!this->theWeyl.isARoot(eta + minusEpsilon)) {
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
  int indexDeltaMinusEpsilon = this->theWeyl.RootSystem.getIndex(delta + minusEpsilon);
  int indexGammaMinusEpsilon = this->theWeyl.RootSystem.getIndex(gamma + minusEpsilon);
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
    this->theWeyl.RootScalarCartanRoot(eta, eta) /
    this->theWeyl.RootScalarCartanRoot(minusZeta, minusZeta)
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
          global.fatal << "This is a programming error. "
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
  int index1 = this->theWeyl.RootSystem.getIndex(root1);
  int index2 = this->theWeyl.RootSystem.getIndex(root2);
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
  if (this->theWeyl.RootSystem.getIndex(root1) == - 1 || this->theWeyl.RootSystem.getIndex(root2) == - 1) {
    outputRat.makeZero();
    return true;
  }
  outputH = (root1 * 2) / Vector<Rational>::scalarProduct(root1, root1, this->theWeyl.cartanSymmetric);
  return false;
}

void SemisimpleLieAlgebra::makeChevalleyTestReport(int i, int j, int k, int Total) {
  if (!global.theResponse.reportDesired()) {
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
