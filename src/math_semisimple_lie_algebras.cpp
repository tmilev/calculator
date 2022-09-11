#include "math_extra_semisimple_lie_algebras_root_subalgebras.h"
#include "math_extra_semisimple_lie_algebras_implementation.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_rational_function_implementation.h"
#include "math_extra_drawing_variables.h"
#include "string_constants.h"
#include "math_extra_algebraic_numbers.h"

std::string SemisimpleLieAlgebra::toString(FormatExpressions* format) {
  STACK_TRACE("SemisimpleLieAlgebra::toString");
  std::stringstream out;
  std::string tempS;
  Vector<Rational> root;
  Vector<Rational> root2;
  int numberOfRoots = this->weylGroup.rootSystem.size;
  int dimension = this->weylGroup.cartanSymmetric.numberOfRows;
  ElementSemisimpleLieAlgebra<Rational> element1, element2, element3;
  std::string hLetter = "h";
  std::string gLetter = "g";
  if (format != nullptr) {
    hLetter = format->chevalleyHGeneratorLetter;
    gLetter = format->chevalleyGGeneratorLetter;
  }
  out << "Type " << this->weylGroup.dynkinType.toString() << ".";
  out
  << "The letter "
  << HtmlRoutines::getMathNoDisplay(hLetter)
  << " stands for elements of the Cartan subalgebra, <br>"
  << "the letter "
  << HtmlRoutines::getMathNoDisplay(gLetter)
  << " stands for the Chevalley (root space) "
  << "generators of non-zero weight. <br>"
  << "The generator "
  << HtmlRoutines::getMathNoDisplay(hLetter + "_i")
  << " is the element of the Cartan subalgebra dual to the <br>"
  << "i^th simple root, that is, "
  << HtmlRoutines::getMathNoDisplay(
    "[" + hLetter + "_i, g] =\\langle \\alpha_i , \\gamma\\rangle g"
  )
  << ", <br> where g is a Chevalley generator, "
  << HtmlRoutines::getMathNoDisplay("\\gamma")
  << " is its weight, and <br>"
  << HtmlRoutines::getMathNoDisplay("\\alpha_i")
  << " is the i^th simple root. ";
  std::stringstream tableLateXStream, htmlStream;
  htmlStream
  << "<table><tr><td> roots simple coords </td><td>epsilon coordinates</td>"
  << "<td>[,]</td>";
  tableLateXStream << "\\begin{array}{cc|";
  for (int i = 0; i < this->getNumberOfGenerators() + 1; i ++) {
    tableLateXStream << "c";
  }
  tableLateXStream << "}\n";
  tableLateXStream
  << "\\mathrm{roots~simple~coords}&\\varepsilon-\\mathrm{root~notation}&"
  << "[\\bullet, \\bullet]\n";
  for (int i = 0; i < numberOfRoots + dimension; i ++) {
    element1.makeGenerator(i, *this);
    tempS = element1.toString(format);
    htmlStream << "<td>" << tempS << "</td>";
    tableLateXStream << " & ";
    tableLateXStream << tempS;
  }
  tableLateXStream << "\\\\\n";
  htmlStream << "</tr>";
  // int lineCounter = 0;
  for (int i = 0; i < dimension + numberOfRoots; i ++) {
    root = this->getWeightOfGenerator(i);
    tableLateXStream << root.toString() << "&";
    htmlStream << "<tr><td>" << root.toString() << "</td>";
    this->weylGroup.getEpsilonCoordinates(root, root2);
    tableLateXStream << root2.toStringLetterFormat("\\varepsilon") << "&";
    htmlStream << "<td>" << root2.toStringLetterFormat("e") << "</td>";
    element1.makeGenerator(i, *this);
    tempS = element1.toString(format);
    tableLateXStream << tempS;
    htmlStream << "<td>" << tempS << "</td>";
    for (int j = 0; j < numberOfRoots + dimension; j ++) {
      element2.makeGenerator(j, *this);
      this->lieBracket(element1, element2, element3);
      tempS = element3.toString(format);
      tableLateXStream << "& ";
      tableLateXStream << tempS;
      htmlStream << "<td>" << tempS << "</td>";
    }
    htmlStream << "</tr>";
    tableLateXStream << "\\\\\n";
  }
  htmlStream << "</table>";
  tableLateXStream << "\\end{array}";
  if (this->getNumberOfGenerators() < 22) {
    out
    << "<div class='mathcalculator'>"
    << tableLateXStream.str()
    << "</div>";
    return out.str();
  }
  out
  << "<br><b> The Lie bracket table is too "
  << "large to be rendered in LaTeX, displaying in "
  << "html format instead.</b> ";
  if (format != nullptr) {
    if (format->flagLatexDetailsInHtml) {
      out
      << "Below you can find the same table in pure LaTeX, "
      << "which you can render in a separate LaTeX session, should "
      << "wish to do so on your own. ";
    }
  }
  out << htmlStream.str();
  if (format != nullptr) {
    if (format->flagLatexDetailsInHtml) {
      out
      << "The above table in LaTex format follows. <hr>"
      << tableLateXStream.str()
      << "<hr>";
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
  out
  << this->toStringMenuStructurePages(
    relativeTo,
    includeStructureConstants,
    includeRootSubalgebras,
    includeSl2Subalgebras,
    includeSemisimpleSubalgebras
  );
  out
  << "<hr>Page generated by the <a href = '"
  << HtmlRoutines::gitRepository
  << "'>calculator project</a>. ";
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
    << "<a href = '"
    << relativeTo
    << this->fileNames.fileNameNoPathStructureConstants()
    << "'>Structure constants and notation</a>.<br>";
  } else {
    out << "<b>Structure constants and notation</b>.<br>";
  }
  if (includeRootSubalgebras) {
    out
    << "<a href = '"
    << relativeTo
    << this->fileNames.fileNameNoPathRootSubalgebras()
    << "'>Root subalgebras / root subsystems</a>.<br>";
  } else {
    out << "<b>Root subalgebras / root subsystems</b>.<br>";
  }
  if (includeSl2Subalgebras) {
    out
    << "<a href = '"
    << relativeTo
    << this->fileNames.fileNameRelativePathSlTwoSubalgebras()
    << "'>sl(2)-subalgebras</a>.<br>";
  } else {
    out << "<b>sl(2)-subalgebras</b>.<br>";
  }
  if (this->weylGroup.dynkinType.hasPrecomputedSubalgebras()) {
    if (includeSemisimpleSubalgebras) {
      out
      << "<a href = '"
      << relativeTo
      << this->fileNames.fileNameNoPathSemisimpleSubalgebras()
      << "'>Semisimple subalgebras</a>.<br>";
    } else {
      out << "<b>Semisimple subalgebras</b>.<br>";
    }
  }
  return out.str();
}

std::string SemisimpleLieAlgebra::toHTMLCalculatorHeadElements(
  const std::string& relativeTo
) {
  std::stringstream out;
  out
  << "\n<link rel='stylesheet' href='"
  << relativeTo
  << WebAPI::request::calculatorCSS
  << "'>";
  out
  << "\n<link rel='stylesheet' href='"
  << relativeTo
  << WebAPI::request::lieAlgebrasCSS
  << "'>";
  out
  << "\n<script src='"
  << relativeTo
  << WebAPI::request::onePageJS
  << "'></script>\n";
  return out.str();
}

std::string SemisimpleLieAlgebra::toHTMLCalculatorBodyOnload() {
  std::stringstream out;
  out
  << "<body onload='" // <<
  // "window.calculator.equationEditor.typeset(document.body, null); "
  << "window.calculator.dynamicJavascript."
  << "dynamicJavascript.bootstrapAllScripts(document.body);"
  << "window.calculator.lieAlgebras.bootstrap();"
  << "'>";
  return out.str();
}

std::string SemisimpleLieAlgebra::toHTMLCalculatorMainDiv() {
  return "<div class='divSemisimpleLieAlgebraStructureMainPage'>";
}

std::string SemisimpleLieAlgebra::toHTML(
  bool verbose,
  bool flagWriteLatexPlots,
  const std::string& extraDynkinDiagramPlot
) {
  STACK_TRACE("SemisimpleLieAlgebra::toHTMLCalculator");
  std::stringstream out;
  FormatExpressions format, latexFormat;
  latexFormat.flagUseLatex = true;
  latexFormat.flagUseHTML = false;
  out
  << "<h1>Lie algebra "
  << this->toStringLieAlgebraNameFullHTML()
  << "</h1>";
  out
  << "<br>Weyl group size: "
  << this->weylGroup.group.getSize().toString()
  << ".<br>";
  if (!verbose) {
    out << "<br>To get extra details: ";
    std::stringstream tempStream;
    tempStream
    << "PrintSemisimpleLieAlgebra{}("
    << this->weylGroup.dynkinType
    << ")";
    out
    << HtmlRoutines::getCalculatorComputationAnchorSameURL(
      tempStream.str(), ""
    )
    << "<br>";
  } else {
    DrawingVariables drawingVariables;
    this->weylGroup.drawRootSystem(
      drawingVariables, true, true, nullptr, true, nullptr
    );
    out
    << "A drawing of the root system in its corresponding Coxeter plane. "
    << "A basis of the plane was computed "
    << "as explained by the website of John Stembridge. "
    << "<br>The darker red dots can be "
    << "dragged with the mouse to rotate the picture."
    << "<br>The grey lines are the edges of the Weyl chamber.<br>"
    << drawingVariables.getHTMLDiv(this->weylGroup.getDimension(), true);
    out << this->weylGroup.toStringRootsAndRootReflections();
    out
    << " The resulting Lie bracket pairing table follows. <hr> "
    << this->toString(&global.defaultFormat.getElement());
    if (flagWriteLatexPlots) {
      out
      << "Ready for LaTeX consumption version of the first three columns: ";
      out
      << "<br>%Add to preamble: <br>"
      << "\\usepackage{longtable} <br>%Add to body: <br>"
      << "\\begin{longtable}{ccc}generator & root simple coord. "
      << "& root $\\varepsilon$-notation \\\\\\hline<br>\n";
      Vector<Rational> root, root2;
      ElementSemisimpleLieAlgebra<Rational> element1;
      for (int i = 0; i < this->getNumberOfGenerators(); i ++) {
        element1.makeGenerator(i, *this);
        root = this->getWeightOfGenerator(i);
        this->weylGroup.getEpsilonCoordinates(root, root2);
        out
        << "$"
        << element1.toString(&format)
        << "$&$"
        << root.toString()
        << "$";
        out << "&$" << root2.toStringLetterFormat("\\varepsilon") << "$";
        out << "\\\\\n";
      }
      out << "\\end{longtable}" << "<hr>";
    }
  }
  out
  << "We define the symmetric Cartan matrix"
  << "<br>by requesting that the entry in the i-th row and j-th column<br>"
  << "be the scalar product of the i^th and j^th roots. "
  << "The symmetric Cartan matrix is:<br>"
  << HtmlRoutines::getMathNoDisplay(
    this->weylGroup.cartanSymmetric.toString(&latexFormat)
  );
  if (extraDynkinDiagramPlot != "") {
    out << "<br>" << extraDynkinDiagramPlot;
  }
  out
  << "<br>Let the (i, j)^{th} entry of the symmetric Cartan matrix be a_{ij}. "
  << "<br> Then we define the co-symmetric Cartan matrix as "
  << " the matrix whose (i, j)^{th} entry equals 4*a_{ij}/(a_{ii}*a_{jj}). "
  << "In other words, the co-symmetric Cartan matrix is the "
  << "symmetric Cartan matrix of the dual root system. "
  << "The co-symmetric Cartan matrix equals:<br>"
  << HtmlRoutines::getMathNoDisplay(
    this->weylGroup.coCartanSymmetric.toStringLatex()
  );
  out
  << "<br>The determinant of the symmetric Cartan matrix is: "
  << this->weylGroup.cartanSymmetric.getDeterminant().toString();
  Vectors<Rational> fundamentalWeights, fundamentalWeightsEpsForm;
  this->weylGroup.getFundamentalWeightsInSimpleCoordinates(fundamentalWeights);
  Vectors<Rational> simpleBasis, simplebasisEpsCoords;
  out << "<hr> Half sum of positive roots: " << this->weylGroup.rho.toString();
  Vector<Rational> root;
  this->weylGroup.getEpsilonCoordinates(this->weylGroup.rho, root);
  out
  << "= "
  << HtmlRoutines::getMathNoDisplay(
    root.toStringLetterFormat("\\varepsilon")
  );
  out
  << "<hr>The fundamental weights "
  << "(the j^th fundamental weight has scalar product 1 "
  << "<br>with the j^th simple root times "
  << "2 divided by the root length squared,<br> "
  << " and 0 with the remaining simple roots): ";
  this->weylGroup.getEpsilonCoordinates(
    fundamentalWeights, fundamentalWeightsEpsForm
  );
  std::stringstream fundamentalWeightsStream;
  FormatExpressions latexFormatWithFractions;
  latexFormatWithFractions.flagUseHTML = false;
  latexFormatWithFractions.flagUseFrac = true;
  latexFormatWithFractions.flagUseLatex = true;
  fundamentalWeightsStream << "\\begin{array}{rcl}";
  for (int i = 0; i < fundamentalWeights.size; i ++) {
    fundamentalWeightsStream
    << fundamentalWeights[i].toString(&latexFormatWithFractions)
    << "&=&"
    << fundamentalWeightsEpsForm[i].toStringEpsilonFormat(
      &latexFormatWithFractions
    )
    << "\\\\\n";
  }
  fundamentalWeightsStream << "\\end{array}";
  out
  << "<br>"
  << HtmlRoutines::getMathNoDisplay(fundamentalWeightsStream.str());
  out
  << "<hr>Below is the simple basis realized in epsilon coordinates. "
  << "Please note that the epsilon coordinate realizations "
  << "do not have long roots of length of 2 in types G and C. "
  << "This means that gramm matrix (w.r.t. the standard scalar product) "
  << "of the epsilon coordinate realizations in types G and C "
  << "does not equal the  corresponding symmetric Cartan matrix. "
  << "<table>";
  simpleBasis.makeEiBasis(this->weylGroup.getDimension());
  this->weylGroup.getEpsilonCoordinates(simpleBasis, simplebasisEpsCoords);
  std::stringstream simpleBasisStream;
  simpleBasisStream << "\\begin{array}{rcl}";
  for (int i = 0; i < simplebasisEpsCoords.size; i ++) {
    simpleBasisStream
    << simpleBasis[i].toString(&latexFormatWithFractions)
    << "&=&"
    << simplebasisEpsCoords[i].toStringEpsilonFormat(
      &latexFormatWithFractions
    )
    << "\\\\";
  }
  simpleBasisStream << "\\end{array}";
  out << "<br>" << HtmlRoutines::getMathNoDisplay(simpleBasisStream.str());
  DynkinSimpleType tempSimpleType;
  if (
    this->weylGroup.dynkinType.isSimple(
      &tempSimpleType.letter,
      &tempSimpleType.rank,
      &tempSimpleType.cartanSymmetricInverseScale
    )
  ) {
    if (tempSimpleType.cartanSymmetricInverseScale == 1) {
      Matrix<Rational> tempM, tempM2;
      this->weylGroup.dynkinType.getEpsilonMatrix(tempM);
      tempM2 = tempM;
      tempM2.transpose();
      tempM2.multiplyOnTheRight(tempM);
      tempM2 *= 2 /
      tempSimpleType.getEpsilonRealizationLongRootLengthSquared();
      if (!(tempM2 == this->weylGroup.cartanSymmetric)) {
        global.fatal
        << "This is a (non-critical) programming error: "
        << "the epsilon coordinates of the vectors are incorrect. "
        << "Please fix function DynkinType::getEpsilonMatrix. "
        << "The matrix of the epsilon coordinates is "
        << tempM.toString()
        << ", the Symmetric Cartan matrix is "
        << this->weylGroup.cartanSymmetric.toString()
        << ", and the "
        << "transpose of the epsilon matrix times the epsilon matrix: "
        << tempM2.toString()
        << ". "
        << global.fatal;
      }
    }
  }
  return out.str();
}

void SemisimpleLieAlgebra::writeHTML(
  bool verbose,
  bool flagWriteLatexPlots,
  const std::string& extraDynkinDiagramPlot
) {
  std::stringstream outWithLinks;
  std::stringstream outFile;
  outFile << "<html>";
  outFile << this->toHTMLCalculatorHeadElements();
  outFile << this->toHTMLCalculatorBodyOnload();
  outFile
  << this->toStringHTMLMenuStructureSummary("", false, true, true, true);
  outFile << this->toHTMLCalculatorMainDiv();
  outFile
  << this->toHTML(verbose, flagWriteLatexPlots, extraDynkinDiagramPlot);
  outFile << "</div>";
  outFile << "</body>";
  outFile << "</html>";
  FileOperations::writeFileVirual(
    this->fileNames.virtualFileNameWithPathStructureConstants(),
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
  out
  << "/output/semisimple_lie_algebras/"
  << FileOperations::cleanUpForFileNameUse(this->toString())
  << "/";
  return out.str();
}

std::string SemisimpleLieAlgebra::FileNames::
fileNameRelativePathSlTwoSubalgebras() const {
  std::stringstream out;
  out
  << "sl2s/sl2s_"
  << FileOperations::cleanUpForFileNameUse(
    this->owner->weylGroup.dynkinType.toString()
  )
  << ".html";
  return out.str();
}

std::string SemisimpleLieAlgebra::FileNames::
fileNameSlTwoRealFormSubalgebraStructure() const {
  std::stringstream out;
  out << "real_form_" << this->fileNameNoPathSemisimpleSubalgebras();
  return out.str();
}

std::string SemisimpleLieAlgebra::FileNames::
fileNameNoPathSemisimpleSubalgebras() const {
  std::stringstream out;
  out
  << "semisimple_subalgebras_"
  << FileOperations::cleanUpForFileNameUse(
    this->owner->weylGroup.dynkinType.toString()
  )
  << ".html";
  return out.str();
}

std::string SemisimpleLieAlgebra::FileNames::fileNameNoPathRootSubalgebras()
const {
  std::stringstream fileName;
  fileName
  << "rootSubalgebras_"
  << FileOperations::cleanUpForFileNameUse(
    this->owner->weylGroup.dynkinType.toString()
  )
  << ".html";
  return fileName.str();
}

std::string SemisimpleLieAlgebra::FileNames::virtualFolderName() const {
  STACK_TRACE("SemisimpleLieAlgebra::FileNames::virtualFolderName");
  this->owner->checkConsistency();
  return this->owner->weylGroup.dynkinType.toStringVirtualNameFolder();
}

std::string SemisimpleLieAlgebra::FileNames::
virtualFileNameWithPathStructureConstants() const {
  std::stringstream fileName;
  fileName
  << this->virtualFolderName()
  << this->fileNameNoPathStructureConstants();
  return fileName.str();
}

std::string SemisimpleLieAlgebra::FileNames::displayFolderName(
  const std::string& baseFolder
) const {
  std::stringstream out;
  out
  << baseFolder
  << "semisimple_lie_algebras/"
  << FileOperations::cleanUpForFileNameUse(
    this->owner->weylGroup.dynkinType.toString()
  )
  << "/";
  return out.str();
}

std::string SemisimpleLieAlgebra::FileNames::fileNameNoPathStructureConstants()
const {
  std::stringstream fileName;
  fileName
  << "structure_constants_notation_"
  << FileOperations::cleanUpForFileNameUse(
    this->owner->weylGroup.dynkinType.toString()
  )
  << ".html";
  return fileName.str();
}

std::string SemisimpleLieAlgebra::FileNames::fileNameWithPathStructureConstants
(const std::string& baseFolder) const {
  std::stringstream fileName;
  fileName
  << this->displayFolderName(baseFolder)
  << this->fileNameNoPathStructureConstants();
  return fileName.str();
}

void SemisimpleLieAlgebra::computeChevalleyConstants() {
  STACK_TRACE("SemisimpleLieAlgebra::computeChevalleyConstants");
  this->weylGroup.computeRho(true);
  this->chevalleyConstants.initialize(
    this->weylGroup.rootSystem.size,
    this->weylGroup.rootSystem.size
  );
  this->computedChevalleyConstants.initialize(
    this->weylGroup.rootSystem.size,
    this->weylGroup.rootSystem.size
  );
  this->computedChevalleyConstants.makeZero(false);
  Selection nonExploredRoots;
  this->flagAnErrorHasOccurredTimeToPanic = false;
  Vectors<Rational>& positiveRoots = this->weylGroup.rootsOfBorel;
  nonExploredRoots.makeFullSelection(positiveRoots.size);
  Vector<Rational> root;
  std::stringstream out;
  ProgressReport report;
  double startTimer = - 1;
  if (report.tickAndWantReport()) {
    out
    << "Initializing matrix for structure constant computation of "
    << this->toStringLieAlgebraName()
    << "... ";
    startTimer = global.getElapsedSeconds();
    report.report(out.str());
  }
  for (int i = 0; i < this->weylGroup.rootSystem.size; i ++) {
    for (int j = i; j < this->weylGroup.rootSystem.size; j ++) {
      root = this->weylGroup.rootSystem[i] + this->weylGroup.rootSystem[j];
      if (!root.isEqualToZero()) {
        if (!this->weylGroup.isARoot(root)) {
          this->computedChevalleyConstants.elements[i][j] = true;
          this->chevalleyConstants.elements[i][j].makeZero();
          this->computedChevalleyConstants.elements[j][i] = true;
          this->chevalleyConstants.elements[j][i].makeZero();
        }
      }
    }
  }
  double startStructureConstantComputation = - 1;
  if (report.tickAndWantReport()) {
    out
    << "done in "
    << global.getElapsedSeconds() - startTimer
    << " seconds.<br> "
    << "Computing structure constants...";
    report.report(out.str());
    startStructureConstantComputation = global.getElapsedSeconds();
  }
  Rational coordinateSum;
  while (nonExploredRoots.cardinalitySelection > 0) {
    int borelIndex = nonExploredRoots.elements[0];
    Rational height = positiveRoots[borelIndex].sumCoordinates();
    for (int i = 1; i < nonExploredRoots.cardinalitySelection; i ++) {
      coordinateSum =
      positiveRoots[nonExploredRoots.elements[i]].sumCoordinates();
      if (height.isGreaterThan(coordinateSum)) {
        height = coordinateSum;
        borelIndex = nonExploredRoots.elements[i];
      }
    }
    Vector<Rational>& root = positiveRoots[borelIndex];
    int index = this->weylGroup.rootSystem.getIndex(root);
    Vector<Rational> smallRoot2;
    int firstIndexFirstPositiveChoice = - 1;
    int secondIndexFirstPositiveChoice = - 1;
    Rational currentHeight;
    for (int i = 0; i < this->weylGroup.rootsOfBorel.size; i ++) {
      Vector<Rational>& smallRoot1 = this->weylGroup.rootsOfBorel[i];
      currentHeight = smallRoot1.sumCoordinates();
      int firstPositiveIndex = this->weylGroup.rootSystem.getIndex(smallRoot1);
      int firstNegativeIndex =
      this->weylGroup.rootSystem.getIndex(- smallRoot1);
      if (height.isGreaterThan(currentHeight)) {
        smallRoot2 = root - smallRoot1;
        int SecondPosIndex = this->weylGroup.rootSystem.getIndex(smallRoot2);
        if (firstPositiveIndex < SecondPosIndex) {
          int secondNegativeIndex =
          this->weylGroup.rootSystem.getIndex(- smallRoot2);
          if (firstIndexFirstPositiveChoice == - 1) {
            firstIndexFirstPositiveChoice = firstPositiveIndex;
            secondIndexFirstPositiveChoice = SecondPosIndex;
            int power;
            this->getMaxQForWhichBetaMinusQAlphaisARoot(
              smallRoot1, smallRoot2, power
            );
            this->chevalleyConstants.elements[firstNegativeIndex][
              secondNegativeIndex
            ] =
            - 1 - power;
            this->computedChevalleyConstants.elements[firstNegativeIndex][
              secondNegativeIndex
            ] =
            true;
          } else {
            this->computeOneChevalleyConstant(
              firstIndexFirstPositiveChoice,
              secondIndexFirstPositiveChoice,
              firstNegativeIndex,
              secondNegativeIndex,
              index
            );
          }
          this->exploitSymmetryAndCyclicityChevalleyConstants(
            firstNegativeIndex, secondNegativeIndex
          );
        }
      }
    }
    nonExploredRoots.selected[borelIndex] = false;
    nonExploredRoots.computeIndicesFromSelection();
  }
  double startMultTable = - 1;
  if (report.tickAndWantReport()) {
    out
    << " done in "
    << global.getElapsedSeconds() - startStructureConstantComputation
    << " seconds.<br> Computing Lie bracket "
    << "pairing (``multiplication'') table...";
    report.report(out.str());
    startMultTable = global.getElapsedSeconds();
  }
  this->computeLieBracketTable();
  if (report.tickAndWantReport()) {
    out
    << " done in "
    << global.getElapsedSeconds() - startMultTable
    << " seconds. Total structure constant computation time: "
    << global.getElapsedSeconds() - startTimer
    << " seconds. ";
    report.report(out.str());
  }
  if (this->getNumberOfPositiveRoots() <= 0) {
    global.fatal
    << "Number of positive roots of a "
    << "semisimple Lie algebra is reported to be zero. "
    << global.fatal;
  }
}

void SemisimpleLieAlgebra::computeLieBracketTable() {
  int numberOfPositiveRoots = this->weylGroup.rootsOfBorel.size;
  int rank = this->weylGroup.cartanSymmetric.numberOfRows;
  int numberOfRoots = numberOfPositiveRoots * 2;
  int numberOfGenerators = numberOfRoots + rank;
  this->lieBrackets.initialize(numberOfGenerators, numberOfGenerators);
  this->universalEnvelopingGeneratorOrder.initializeFillInObject(
    numberOfGenerators, - 1
  );
  Vector<Rational> leftWeight, rightWeight, hRoot;
  for (int i = 0; i < numberOfGenerators; i ++) {
    leftWeight = this->getWeightOfGenerator(i);
    for (int j = i; j < numberOfGenerators; j ++) {
      rightWeight = this->getWeightOfGenerator(j);
      if (leftWeight.isEqualToZero() && rightWeight.isEqualToZero()) {
        this->lieBrackets.elements[i][j].makeZero();
        continue;
      }
      if (leftWeight.isEqualToZero() && !rightWeight.isEqualToZero()) {
        this->lieBrackets.elements[i][j].makeGenerator(j, *this);
        hRoot.makeEi(rank, i - numberOfPositiveRoots);
        this->lieBrackets.elements[i][j] *= Vector<Rational>::scalarProduct(
          hRoot, rightWeight, this->weylGroup.cartanSymmetric
        );
        continue;
      }
      if (!leftWeight.isEqualToZero() && rightWeight.isEqualToZero()) {
        this->lieBrackets.elements[i][j].makeGenerator(i, *this);
        hRoot.makeEi(rank, j - numberOfPositiveRoots);
        this->lieBrackets.elements[i][j] *=
        - Vector<Rational>::scalarProduct(
          hRoot, leftWeight, this->weylGroup.cartanSymmetric
        );
        continue;
      }
      if (!leftWeight.isEqualToZero() && !rightWeight.isEqualToZero()) {
        int newIndex =
        this->getGeneratorIndexFromRoot(leftWeight + rightWeight);
        if (newIndex != - 1) {
          this->lieBrackets.elements[i][j].makeGenerator(newIndex, *this);
          int leftIndex = this->getRootIndexFromGenerator(i);
          int rightIndex = this->getRootIndexFromGenerator(j);
          this->lieBrackets.elements[i][j] *=
          this->chevalleyConstants.elements[leftIndex][rightIndex];
        } else {
          if (!(leftWeight + rightWeight).isEqualToZero()) {
            this->lieBrackets.elements[i][j].makeZero();
          } else {
            ElementSemisimpleLieAlgebra<Rational>& current =
            this->lieBrackets.elements[i][j];
            current.makeCartanGenerator(
              leftWeight * 2 / (
                this->weylGroup.rootScalarCartanRoot(leftWeight, leftWeight)
              ),
              *this
            );
          }
        }
        continue;
      }
    }
  }
  for (int i = 0; i < numberOfGenerators; i ++) {
    for (int j = i + 1; j < numberOfGenerators; j ++) {
      this->lieBrackets.elements[j][i] = this->lieBrackets.elements[i][j];
      this->lieBrackets.elements[j][i] *= - 1;
    }
  }
  for (int i = 0; i < numberOfGenerators; i ++) {
    this->universalEnvelopingGeneratorOrder[i] = i;
  }
}

void SemisimpleLieAlgebra::exploitSymmetryAndCyclicityChevalleyConstants(
  int indexI, int indexJ
) {
  const Vector<Rational>& rootI = this->weylGroup.rootSystem[indexI];
  const Vector<Rational>& rootJ = this->weylGroup.rootSystem[indexJ];
  if ((rootI + rootJ).isEqualToZero()) {
    global.fatal
    << "Sum or roots not allowed to be zero here. "
    << global.fatal;
  }
  this->exploitSymmetryChevalleyConstants(indexI, indexJ);
  int indexMinusIMinusJ = this->weylGroup.rootSystem.getIndex(- rootI - rootJ);
  this->exploitTheCyclicTrick(indexI, indexJ, indexMinusIMinusJ);
}

void SemisimpleLieAlgebra::exploitSymmetryChevalleyConstants(
  int indexI, int indexJ
) {
  const Vector<Rational>& rootI = this->weylGroup.rootSystem[indexI];
  const Vector<Rational>& rootJ = this->weylGroup.rootSystem[indexJ];
  if (!this->computedChevalleyConstants.elements[indexI][indexJ]) {
    global.fatal
    << "Structure constants computed in a wrong order. "
    << global.fatal;
  }
  int indexMinusI = this->weylGroup.rootSystem.getIndex(- rootI);
  int indexMinusJ = this->weylGroup.rootSystem.getIndex(- rootJ);
  this->chevalleyConstants.elements[indexJ][indexI] = (
    this->chevalleyConstants.elements[indexI][indexJ] *(- 1)
  );
  this->computedChevalleyConstants.elements[indexJ][indexI] = true;
  if ((rootI + rootJ).isEqualToZero()) {
    global.fatal << "Bad root sum. " << global.fatal;
  }
  int power;
  this->getMaxQForWhichBetaMinusQAlphaisARoot(
    this->weylGroup.rootSystem[indexMinusI],
    this->weylGroup.rootSystem[indexMinusJ],
    power
  );
  int i = 1 + power;
  this->chevalleyConstants.elements[indexMinusI][indexMinusJ] = - i * i;
  this->chevalleyConstants.elements[indexMinusI][indexMinusJ] /=
  this->chevalleyConstants.elements[indexI][indexJ];
  this->computedChevalleyConstants.elements[indexMinusI][indexMinusJ] = true;
  this->chevalleyConstants.elements[indexMinusJ][indexMinusI] =
  this->chevalleyConstants.elements[indexMinusI][indexMinusJ] *(- 1);
  this->computedChevalleyConstants.elements[indexMinusJ][indexMinusI] = true;
}

void SemisimpleLieAlgebra::exploitTheCyclicTrick(int i, int j, int k) {
  const Vector<Rational>& rootI = this->weylGroup.rootSystem[i];
  const Vector<Rational>& rootK = this->weylGroup.rootSystem[k];
  const Vector<Rational>& rootJ = this->weylGroup.rootSystem[j];
  // the following three checks can be commented out to increase speed. They
  // have never failed so far.
  if (!(rootI + rootK + rootJ).isEqualToZero()) {
    global.fatal
    << "Three roots do not sum to zero as required. "
    << global.fatal;
  }
  if (!this->computedChevalleyConstants.elements[i][j]) {
    global.fatal
    << "Bad structure constant computation order."
    << global.fatal;
  }
  Rational& chevalleyConstant = this->chevalleyConstants.elements[i][j];
  Rational scalarProduct = this->weylGroup.rootScalarCartanRoot(rootK, rootK);
  this->chevalleyConstants.elements[j][k] = (
    chevalleyConstant * this->weylGroup.rootScalarCartanRoot(rootI, rootI)
  ) /
  scalarProduct;
  this->computedChevalleyConstants.elements[j][k] = true;
  this->chevalleyConstants.elements[k][i] = (
    chevalleyConstant * this->weylGroup.rootScalarCartanRoot(rootJ, rootJ)
  ) /
  scalarProduct;
  this->computedChevalleyConstants.elements[k][i] = true;
  this->exploitSymmetryChevalleyConstants(j, k);
  this->exploitSymmetryChevalleyConstants(k, i);
}

bool SemisimpleLieAlgebra::getMaxQForWhichBetaMinusQAlphaisARoot(
  const Vector<Rational>& alpha,
  const Vector<Rational>& beta,
  int& output
) const {
  output = - 1;
  Vector<Rational> root = beta;
  if (alpha.isEqualToZero()) {
    global.fatal
    << "Calling function "
    << "getMaxQForWhichBetaMinusQAlphaisARoot "
    << "with zero value for alpha is not allowed. "
    << global.fatal;
  }
  bool foundRoot = false;
  while (this->weylGroup.isARoot(root)) {
    output ++;
    root -= alpha;
    foundRoot = true;
  }
  return foundRoot;
}

void SemisimpleLieAlgebra::computeOneChevalleyConstant(
  int indexGamma,
  int indexDelta,
  int indexMinusEpsilon,
  int indexMinusZeta,
  int indexEta
) {
  // using formula (**), 2.9, page 49, Samelson, Notes on Lie algebras, 1989
  const Vector<Rational>& gamma =
  this->weylGroup.rootSystem[indexGamma];
  const Vector<Rational>& delta = this->weylGroup.rootSystem[indexDelta];
  const Vector<Rational>& minusEpsilon =
  this->weylGroup.rootSystem[indexMinusEpsilon];
  const Vector<Rational>& eta = this->weylGroup.rootSystem[indexEta];
  const Vector<Rational>& minusZeta =
  this->weylGroup.rootSystem[indexMinusZeta];
  if (eta != gamma + delta) {
    global.fatal << "Eta must equal gamma plus delta. " << global.fatal;
  }
  if (!this->weylGroup.isARoot(eta + minusEpsilon)) {
    global.fatal << "Eta minus epsilon must be a root. " << global.fatal;
  }
  if (
    !this->computedChevalleyConstants.elements[indexDelta][indexMinusEpsilon]
    ||
    !this->computedChevalleyConstants.elements[indexMinusEpsilon][indexGamma]
    ||
    !this->computedChevalleyConstants.elements[indexGamma][indexDelta]
  ) {
    global.fatal
    << "Bad structure constant computation order. "
    << global.fatal;
  }
  if (
    this->chevalleyConstants.elements[indexGamma][indexDelta].isEqualToZero()
  ) {
    global.fatal << "Unexpected zero structure constants" << global.fatal;
  }
  int indexDeltaMinusEpsilon =
  this->weylGroup.rootSystem.getIndex(delta + minusEpsilon);
  int indexGammaMinusEpsilon =
  this->weylGroup.rootSystem.getIndex(gamma + minusEpsilon);
  Rational firstSummand, secondSummand;
  if (indexDeltaMinusEpsilon != - 1) {
    if (
      !this->computedChevalleyConstants.elements[indexGamma][
        indexDeltaMinusEpsilon
      ] ||
      !this->computedChevalleyConstants.elements[indexDelta][indexMinusEpsilon]
    ) {
      global.fatal
      << "Structure constants must be computed at this point. "
      << global.fatal;
    }
    firstSummand =
    this->chevalleyConstants.elements[indexGamma][indexDeltaMinusEpsilon] *
    this->chevalleyConstants.elements[indexDelta][indexMinusEpsilon];
  } else {
    firstSummand.makeZero();
  }
  if (indexGammaMinusEpsilon != - 1) {
    if (
      !this->computedChevalleyConstants.elements[indexDelta][
        indexGammaMinusEpsilon
      ] ||
      !this->computedChevalleyConstants.elements[indexMinusEpsilon][indexGamma]
    ) {
      global.fatal
      << "Structure constants must be computed at this point. "
      << global.fatal;
    }
    secondSummand =
    this->chevalleyConstants.elements[indexDelta][indexGammaMinusEpsilon] *
    this->chevalleyConstants.elements[indexMinusEpsilon][indexGamma];
  } else {
    secondSummand.makeZero();
  }
  this->chevalleyConstants.elements[indexMinusEpsilon][indexMinusZeta] = (
    this->weylGroup.rootScalarCartanRoot(eta, eta) /
    this->weylGroup.rootScalarCartanRoot(minusZeta, minusZeta)
  ) *(firstSummand + secondSummand) /
  this->chevalleyConstants.elements[indexGamma][indexDelta];
  this->computedChevalleyConstants.elements[indexMinusEpsilon][indexMinusZeta]
  =
  true;
}

bool SemisimpleLieAlgebra::testForConsistency() {
  FormatExpressions& format = global.defaultFormat.getElement();
  ElementSemisimpleLieAlgebra<Rational>
  g1,
  g2,
  g3,
  g23,
  g31,
  g12,
  g123,
  g231,
  g312,
  temp;
  // this->ComputeDebugString(false, false, global);
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
          << "The computed structure constants "
          << "are wrong: the Jacobi identity fails. "
          << "More precisely, I get that "
          << "<br>["
          << g1.toString(&format)
          << ", "
          << g2.toString(&format)
          << "] ="
          << g12.toString(&format)
          << "<br>["
          << g2.toString(&format)
          << ", "
          << g3.toString(&format)
          << "] ="
          << g23.toString(&format)
          << "<br>["
          << g3.toString(&format)
          << ", "
          << g1.toString(&format)
          << "] ="
          << g31.toString(&format)
          << "<br>g123= "
          << g123.toString(&format)
          << "<br>g231="
          << g231.toString(&format)
          << "<br>g312="
          << g312.toString(&format)
          << "<br>"
          << global.fatal;
          return false;
        }
        this->makeChevalleyTestReport(
          i, j, k, this->getNumberOfGenerators()
        );
      }
    }
  }
  return true;
}

Vector<Rational> SemisimpleLieAlgebra::getWeightOfGenerator(int index) {
  if (index < this->getNumberOfPositiveRoots()) {
    return this->weylGroup.rootSystem[index];
  }
  if (index >= this->getRank() + this->getNumberOfPositiveRoots()) {
    return this->weylGroup.rootSystem[index - this->getRank()];
  }
  Vector<Rational> result;
  result.makeZero(this->getRank());
  return result;
}

Rational SemisimpleLieAlgebra::getConstant(
  const Vector<Rational>& root1, const Vector<Rational>& root2
) {
  int index1 = this->weylGroup.rootSystem.getIndex(root1);
  int index2 = this->weylGroup.rootSystem.getIndex(root2);
  if (index1 == - 1 || index2 == - 1) {
    return 0;
  }
  return this->chevalleyConstants(index1, index2);
}

bool SemisimpleLieAlgebra::getConstantOrHElement(
  const Vector<Rational>& root1,
  const Vector<Rational>& root2,
  Rational& outputRat,
  Vector<Rational>& outputH
) {
  if (!(root1 + root2).isEqualToZero()) {
    outputRat = this->getConstant(root1, root2);
    return true;
  }
  if (
    this->weylGroup.rootSystem.getIndex(root1) == - 1 ||
    this->weylGroup.rootSystem.getIndex(root2) == - 1
  ) {
    outputRat.makeZero();
    return true;
  }
  outputH = (root1* 2) / Vector<Rational>::scalarProduct(
    root1, root1, this->weylGroup.cartanSymmetric
  );
  return false;
}

void SemisimpleLieAlgebra::makeChevalleyTestReport(
  int i, int j, int k, int total
) {
  if (!global.response.reportDesired()) {
    return;
  }
  std::stringstream out2, out3;
  int x = i * total * total + j * total + k + 1;
  out2
  << "i: "
  << i + 1
  << " of "
  << total
  << " j: "
  << j + 1
  << " of "
  << total
  << " k: "
  << k + 1
  << " of "
  << total;
  out3 << "Total progress: " << x << " out of " << (total * total * total);
  ProgressReport report;
  report.report(out2.str() + out3.str());
}

void SemisimpleLieAlgebra::computeOneAutomorphism(
  Matrix<Rational>& outputAutomorphism, bool useNegativeRootsFirst
) {
  global.fatal << "Not implemented yet!!!!!" << global.fatal;
  RootSubalgebra rootSubalgebra;
  //  rootSubalgebra.initialize(*this);
  int dimension = this->weylGroup.cartanSymmetric.numberOfRows;
  rootSubalgebra.genK.makeEiBasis(dimension);
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms
  automorphisms;
  rootSubalgebra.generateAutomorphismsPreservingBorel(automorphisms);
  Matrix<Rational> mapOnRootSpaces;
  int automorphismIndex = automorphisms.externalAutomorphisms.size > 1 ? 1 : 0;
  mapOnRootSpaces.assignVectorsToRows(
    automorphisms.externalAutomorphisms[automorphismIndex]
  );
  mapOnRootSpaces.transpose();
  Selection nonExplored;
  int numberOfRoots = this->weylGroup.rootSystem.size;
  nonExplored.initialize(numberOfRoots);
  nonExplored.makeFullSelection();
  Vector<Rational> domainRoot, rangeRoot;
  this->computeChevalleyConstants();
  List<ElementSemisimpleLieAlgebra<Rational> > domain, range;
  range.setSize(numberOfRoots + dimension);
  domain.setSize(numberOfRoots + dimension);
  ElementSemisimpleLieAlgebra<Rational> element;
  for (int i = 0; i < dimension; i ++) {
    domainRoot.makeEi(dimension, i);
    mapOnRootSpaces.actOnVectorColumn(domainRoot, rangeRoot);
    element.makeCartanGenerator(domainRoot, *this);
    domain[numberOfRoots + i] = element;
    element.makeCartanGenerator(rangeRoot, *this);
    range[numberOfRoots + i] = element;
    for (
      int i = 0; i < 2; i ++, domainRoot.negate(), rangeRoot.negate()
    ) {
      int index = this->weylGroup.rootSystem.getIndex(rangeRoot);
      element.makeGGenerator(rangeRoot, *this);
      range[index] = element;
      element.makeGGenerator(domainRoot, *this);
      domain[index] = element;
      nonExplored.removeSelection(index);
    }
  }
  Vector<Rational> left;
  Vector<Rational> right;
  while (nonExplored.cardinalitySelection > 0) {
    for (int i = 0; i < nonExplored.cardinalitySelection; i ++) {
      int index = nonExplored.elements[i];
      const Vector<Rational>& current = this->weylGroup.rootSystem[index];
      for (int j = 0; j < dimension; j ++) {
        left.makeEi(dimension, j);
        for (int k = 0; k < 2; k ++, left.negate()) {
          right = current - left;
          if (this->weylGroup.isARoot(right)) {
            int leftIndex = this->weylGroup.rootSystem.getIndex(left);
            int rightIndex = this->weylGroup.rootSystem.getIndex(right);
            if (!nonExplored.selected[rightIndex]) {
              ElementSemisimpleLieAlgebra<Rational>& leftDomainElt =
              domain[leftIndex];
              ElementSemisimpleLieAlgebra<Rational>& rightDomainElt =
              domain[rightIndex];
              this->lieBracket(
                leftDomainElt, rightDomainElt, domain[index]
              );
              ElementSemisimpleLieAlgebra<Rational>& leftRangeElt =
              range[leftIndex];
              ElementSemisimpleLieAlgebra<Rational>& rightRangeElt =
              range[rightIndex];
              this->lieBracket(leftRangeElt, rightRangeElt, range[index]);
              nonExplored.removeSelection(index);
            }
          }
        }
      }
    }
  }
  Vectors<Rational> vectorsLeft, vectorsRight;
  vectorsLeft.setSize(range.size);
  vectorsRight.setSize(range.size);
  if (!useNegativeRootsFirst) {
    for (int i = 0; i < range.size; i ++) {
      range[i].toVectorNegativeRootSpacesFirst(vectorsRight[i]);
      domain[i].toVectorNegativeRootSpacesFirst(vectorsLeft[i]);
    }
  } else {
    for (int i = 0; i < range.size; i ++) {
      range[i].toVectorNegativeRootSpacesFirst(vectorsRight[i]);
      domain[i].toVectorNegativeRootSpacesFirst(vectorsLeft[i]);
    }
  }
  outputAutomorphism.makeInvertibleHomomorphismFromDomainAndRange(
    vectorsLeft, vectorsRight
  );
}

bool SemisimpleLieAlgebra::hasImplementedCartanInvolution(
  const VoganDiagram& voganDiagram,
  CartanInvolution* whichInvolution,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("SemisimpleLieAlgebra::hasImplementedCartanInvolution");
  CartanInvolution outputBuffer;
  if (whichInvolution == nullptr) {
    whichInvolution = &outputBuffer;
  }
  return
  whichInvolution->computeFromDiagram(
    voganDiagram, *this, commentsOnFailure
  );
}

bool SemisimpleLieAlgebra::isInTheWeightSupport(
  Vector<Rational>& weight, Vector<Rational>& highestWeight
) {
  Vector<Rational> correspondingDominant = weight;
  this->weylGroup.raiseToDominantWeight(correspondingDominant);
  Vector<Rational> difference = highestWeight - correspondingDominant;
  if (!difference.isPositiveOrZero()) {
    return false;
  }
  return true;
}

void SemisimpleLieAlgebra::createEmbeddingFromFDModuleHaving1dimWeightSpaces(
  Vector<Rational>& highestWeight
) {
  (void) highestWeight;
  /*Vectors<Rational> weightSupport;
  this->GenerateWeightSupport(highestWeight, weightSupport);
  int highestWeight, distanceToHW;
  this->EmbeddingsRootSpaces.setSize(this->weylGroup.RootSystem.size);
  int dimension = this->weylGroup.cartanSymmetric.numberOfRows;
  List<bool> Explored;
  Explored.initializeFillInObject(this->weylGroup.RootSystem.size, false);
  int numExplored = 0;
  for (int i = 0; i < this->weylGroup.RootSystem.size; i ++) {
    Vector<Rational>& current = this->weylGroup.RootSystem.objects[i];
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
  simpleBasis.makeEiBasis(dimension);
  while (numExplored< this->weylGroup.RootSystem.size) {
    for (int i = 0; i < this->weylGroup.RootSystem.size; i ++)
      if (Explored.objects[i])
        for (int j = 0; j < this->weylGroup.RootSystem.size; j ++)
          if (Explored.objects[j]) {
            Vector<Rational> root = this->weylGroup.RootSystem.objects[i] + this->weylGroup.RootSystem.objects[j];
            if (this->weylGroup.isARoot(root)) {
              int index = this->weylGroup.RootSystem.getIndex(root);
              if (!Explored.objects[index]) {
                Explored.objects[index] = true;
                numExplored++;
                this->EmbeddingsRootSpaces.objects[index] = this->EmbeddingsRootSpaces.objects[i];
                this->EmbeddingsRootSpaces.objects[index].lieBracketWith(this->EmbeddingsRootSpaces.objects[j]);
              }
            }
          }
  }
  this->EmbeddingsCartan.setSize(dimension);
  for (int i = 0; i < dimension; i ++) {
    Matrix<Rational> & current = this->EmbeddingsCartan.objects[i];
    current.initialize(weightSupport.size, weightSupport.size);
    current.makeZero();
    Vector<Rational> root;
    root.makeEi(dimension, i);
    for (int j = 0; j<weightSupport.size; j ++)
      current.elements[j][j] = this->weylGroup.rootScalarCartanRoot(root, weightSupport.objects[j]);
  }*/
}

int SemisimpleLieAlgebra::getLengthStringAlongAlphaThroughBeta(
  Vector<Rational>& alpha,
  Vector<Rational>& beta,
  int& distanceToHighestWeight,
  Vectors<Rational>& weightSupport
) {
  Vector<Rational> root = beta;
  for (int i = 0;; i ++) {
    root += alpha;
    if (!weightSupport.contains(root)) {
      distanceToHighestWeight = i;
      break;
    }
  }
  for (int i = 0;; i ++) {
    root -= alpha;
    if (!weightSupport.contains(root)) {
      return i;
    }
  }
  //  global.fatal << global.fatal;
  //  return - 1;
}

bool HomomorphismSemisimpleLieAlgebra::
computeHomomorphismFromImagesSimpleChevalleyGenerators(
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE(
    "HomomorphismSemisimpleLieAlgebra::"
    "computeHomomorphismFromImagesSimpleChevalleyGenerators"
  );
  (void) commentsOnFailure;
  this->domainAlgebra().computeChevalleyConstants();
  this->coDomainAlgebra().computeChevalleyConstants();
  int domainRank = this->domainAlgebra().getRank();
  Selection nonComputed;
  int numberofGenerators = this->domainAlgebra().getNumberOfGenerators();
  nonComputed.initialize(numberofGenerators);
  nonComputed.makeFullSelection();
  List<ElementSemisimpleLieAlgebra<Rational> > tempDomain, tempRange;
  tempDomain.setSize(numberofGenerators);
  tempRange.setSize(numberofGenerators);
  Vector<Rational> simpleRoot;
  for (int i = 0; i < domainRank; i ++) {
    simpleRoot.makeEi(domainRank, i);
    int index = this->domainAlgebra().getGeneratorIndexFromRoot(simpleRoot);
    int indexNegative =
    this->domainAlgebra().getGeneratorIndexFromRoot(- simpleRoot);
    tempDomain[index].makeGGenerator(simpleRoot, this->domainAlgebra());
    tempDomain[indexNegative].makeGGenerator(
      - simpleRoot, this->domainAlgebra()
    );
    tempRange[index] = this->imagesPositiveSimpleChevalleyGenerators[i];
    tempRange[indexNegative] =
    this->imagesNegativeSimpleChevalleyGenerators[i];
    nonComputed.removeSelection(index);
    nonComputed.removeSelection(indexNegative);
  }
  Vector<Rational> rightRoot;
  this->checkInitialization();
  while (nonComputed.cardinalitySelection > 0) {
    for (int i = 0; i < nonComputed.cardinalitySelection; i ++) {
      int index = nonComputed.elements[i];
      Vector<Rational> current =
      this->domainAlgebra().getWeightOfGenerator(index);
      // Looking for elements left, right such that [left, right] = current.
      for (
        int leftIndex = 0; leftIndex < nonComputed.numberOfElements; leftIndex
        ++
      ) {
        if (nonComputed.selected[leftIndex]) {
          // The candidate for left is not computed yet.
          continue;
        }
        Vector<Rational> leftRoot =
        this->domainAlgebra().getWeightOfGenerator(leftIndex);
        rightRoot = current - leftRoot;
        if (!this->domainAlgebra().weylGroup.isARoot(rightRoot)) {
          // Not a candidate for right.
          continue;
        }
        int rightIndex =
        this->domainAlgebra().getGeneratorIndexFromRoot(rightRoot);
        if (nonComputed.selected[rightIndex]) {
          // I have not computed the right generator yet.
          continue;
        }
        ElementSemisimpleLieAlgebra<Rational>& leftDomainElement =
        tempDomain[leftIndex];
        ElementSemisimpleLieAlgebra<Rational>& rightDomainElement =
        tempDomain[rightIndex];
        this->domainAlgebra().lieBracket(
          leftDomainElement, rightDomainElement, tempDomain[index]
        );
        ElementSemisimpleLieAlgebra<Rational>& leftRangeElement =
        tempRange[leftIndex];
        ElementSemisimpleLieAlgebra<Rational>& rightRangeElement =
        tempRange[rightIndex];
        this->coDomainAlgebra().lieBracket(
          leftRangeElement, rightRangeElement, tempRange[index]
        );
        nonComputed.removeSelection(index);
        break;
      }
    }
  }
  int numberOfPositiveRoots = this->domainAlgebra().getNumberOfPositiveRoots();
  for (int i = 0; i < domainRank; i ++) {
    simpleRoot.makeEi(domainRank, i);
    int leftIndex =
    this->domainAlgebra().getGeneratorIndexFromRoot(simpleRoot);
    int rightIndex =
    this->domainAlgebra().getGeneratorIndexFromRoot(- simpleRoot);
    this->domainAlgebra().lieBracket(
      tempDomain[leftIndex],
      tempDomain[rightIndex],
      tempDomain[numberOfPositiveRoots + i]
    );
    this->coDomainAlgebra().lieBracket(
      tempRange[leftIndex],
      tempRange[rightIndex],
      tempRange[numberOfPositiveRoots + i]
    );
  }
  Vectors<Rational> vectorsLeft, vectorsRight;
  vectorsLeft.setSize(tempDomain.size);
  vectorsRight.setSize(tempDomain.size);
  for (int i = 0; i < tempRange.size; i ++) {
    tempDomain[i].toVectorNegativeRootSpacesFirst(vectorsLeft[i]);
    tempRange[i].toVectorNegativeRootSpacesFirst(
      vectorsRight[i], this->coDomainAlgebra()
    );
  }
  Matrix<Rational> linearSpaceMap;
  linearSpaceMap.makeInvertibleHomomorphismFromDomainAndRange(
    vectorsLeft, vectorsRight
  );
  Vector<Rational> imageRoot;
  this->domainAllChevalleyGenerators.setSize(tempDomain.size);
  this->imagesAllChevalleyGenerators.setSize(tempDomain.size);
  for (
    int i = 0; i < this->domainAlgebra().getNumberOfGenerators(); i ++
  ) {
    this->domainAllChevalleyGenerators[i].makeGenerator(
      i, this->domainAlgebra()
    );
  }
  Vector<Rational> root;
  for (int i = 0; i < this->imagesAllChevalleyGenerators.size; i ++) {
    this->domainAllChevalleyGenerators[i].toVectorNegativeRootSpacesFirst(
      root
    );
    linearSpaceMap.actOnVectorColumn(root, imageRoot);
    this->imagesAllChevalleyGenerators[i].
    assignVectorNegativeRootSpacesCartanPosistiveRootSpaces(
      imageRoot, this->coDomainAlgebra()
    );
  }
  return true;
}

void HomomorphismSemisimpleLieAlgebra::projectOntoSmallCartan(
  Vectors<Rational>& input, Vectors<Rational>& output
) {
  output.setSize(input.size);
  for (int i = 0; i < input.size; i ++) {
    this->projectOntoSmallCartan(input[i], output[i]);
  }
}

void HomomorphismSemisimpleLieAlgebra::projectOntoSmallCartan(
  Vector<Rational>& input, Vector<Rational>& output
) {
  Matrix<Rational> invertedSmallCartan;
  invertedSmallCartan = this->domainAlgebra().weylGroup.cartanSymmetric;
  invertedSmallCartan.invert();
  int smallDimension =
  this->domainAlgebra().weylGroup.cartanSymmetric.numberOfRows;
  output.makeZero(smallDimension);
  for (int i = 0; i < smallDimension; i ++) {
    output[i] =
    this->coDomainAlgebra().weylGroup.rootScalarCartanRoot(
      this->imagesAllChevalleyGenerators[
        this->domainAlgebra().weylGroup.rootsOfBorel.size + i
      ].getCartanPart(),
      input
    );
  }
  invertedSmallCartan.actOnVectorColumn(output, output);
}

bool HomomorphismSemisimpleLieAlgebra::applyHomomorphism(
  const MonomialUniversalEnveloping<RationalFraction<Rational> >& input,
  const RationalFraction<Rational>& coefficient,
  ElementUniversalEnveloping<RationalFraction<Rational> >& output
) {
  ElementUniversalEnveloping<RationalFraction<Rational> > element;
  output.makeZero(this->coDomainAlgebra());
  RationalFraction<Rational> polyOne;
  polyOne.makeOne();
  output.makeConstant(coefficient, this->coDomainAlgebra());
  for (int i = 0; i < input.generatorsIndices.size; i ++) {
    if (
      input.generatorsIndices[i] >= this->imagesAllChevalleyGenerators.size
    ) {
      return false;
    }
    element.assignElementLieAlgebra(
      this->imagesAllChevalleyGenerators[input.generatorsIndices[i]],
      this->coDomainAlgebra(),
      polyOne
    );
    RationalFraction<Rational>& power = input.powers[i];
    int integralPower;
    if (!power.isSmallInteger(&integralPower)) {
      return false;
    }
    for (int j = 0; j < integralPower; j ++) {
      output *= element;
    }
  }
  return true;
}

void HomomorphismSemisimpleLieAlgebra::applyHomomorphism(
  const ElementSemisimpleLieAlgebra<Rational>& input,
  ElementSemisimpleLieAlgebra<Rational>& output
) {
  if (&output == &input) {
    global.fatal
    << "Output required to be different from input. "
    << global.fatal;
  }
  output.makeZero();
  for (int i = 0; i < input.size(); i ++) {
    int currentIndex = input[i].generatorIndex;
    output +=
    this->imagesAllChevalleyGenerators[currentIndex] * input.coefficients[i];
  }
}

void HomomorphismSemisimpleLieAlgebra::getMapSmallCartanDualToLargeCartanDual(
  Matrix<Rational>& output
) {
  output.initialize(
    this->coDomainAlgebra().getRank(),
    this->domainAlgebra().getRank()
  );
  ElementSemisimpleLieAlgebra<Rational> domainElt, imageElt;
  for (int i = 0; i < this->domainAlgebra().getRank(); i ++) {
    domainElt.makeCartanGenerator(
      Vector<Rational>::getEi(this->domainAlgebra().getRank(), i),
      this->domainAlgebra()
    );
    this->applyHomomorphism(domainElt, imageElt);
    output.assignVectorToColumnKeepOtherColsIntactNoInit(
      i, imageElt.getCartanPart()
    );
  }
}

bool HomomorphismSemisimpleLieAlgebra::applyHomomorphism(
  const ElementUniversalEnveloping<RationalFraction<Rational> >& input,
  ElementUniversalEnveloping<RationalFraction<Rational> >& output
) {
  if (&output == &input) {
    global.fatal << "Output must be different from input. " << global.fatal;
  }
  output.makeZero(this->coDomainAlgebra());
  ElementUniversalEnveloping<RationalFraction<Rational> > element;
  for (int i = 0; i < input.size(); i ++) {
    if (
      !this->applyHomomorphism(
        input[i], input.coefficients[i], element
      )
    ) {
      return false;
    }
    output += element;
  }
  return true;
}

void HomomorphismSemisimpleLieAlgebra::makeGInGWithIdentity(
  SemisimpleLieAlgebra& owner
) {
  STACK_TRACE("HomomorphismSemisimpleLieAlgebra::makeGInGWithIdentity");
  DynkinType dynkinType;
  this->domain = &owner;
  this->coDomain = this->domain;
  this->domainAlgebra().computeChevalleyConstants();
  int numberOfGenerators = owner.getNumberOfGenerators();
  this->imagesAllChevalleyGenerators.setSize(numberOfGenerators);
  this->domainAllChevalleyGenerators.setSize(numberOfGenerators);
  int rank = owner.getRank();
  this->imagesPositiveSimpleChevalleyGenerators.setSize(rank);
  this->imagesNegativeSimpleChevalleyGenerators.setSize(rank);
  for (int i = 0; i < numberOfGenerators; i ++) {
    ElementSemisimpleLieAlgebra<Rational>& element1 =
    this->imagesAllChevalleyGenerators[i];
    ElementSemisimpleLieAlgebra<Rational>& element2 =
    this->domainAllChevalleyGenerators[i];
    element2.makeGenerator(i, this->domainAlgebra());
    element1.makeGenerator(i, this->coDomainAlgebra());
  }
  Vector<Rational> simpleVector;
  for (int i = 0; i < rank; i ++) {
    simpleVector.makeEi(rank, i);
    this->imagesPositiveSimpleChevalleyGenerators[i].makeGGenerator(
      simpleVector, owner
    );
    this->imagesNegativeSimpleChevalleyGenerators[i].makeGGenerator(
      - simpleVector, owner
    );
  }
}

std::string HomomorphismSemisimpleLieAlgebra::toString(bool useHtml) {
  STACK_TRACE("HomomorphismSemisimpleLieAlgebra::toString");
  this->checkInitialization();
  std::stringstream out;
  if (this->checkClosednessLieBracket()) {
    out << "Checked that the Lie bracket closes. ";
  } else {
    out << "The Lie bracket is not closed. ";
  }
  if (this->checkIsHomomorphism()) {
    out
    << "Checked that applying the homomorphism first, "
    << "then the Lie bracket "
    << "matches applying Lie bracket first, then homomorphism. ";
  }
  if (useHtml) {
    out << "<br>";
  }
  out << "Images simple negative Chevalley generators:\n\n";
  if (useHtml) {
    out << "<br>";
  }
  FormatExpressions* format = &global.defaultFormat.getElement();
  for (
    int i = 0; i < this->imagesNegativeSimpleChevalleyGenerators.size; i ++
  ) {
    ChevalleyGenerator preimage;
    Vector<Rational> simpleRoot;
    simpleRoot.makeEi(this->domain->getRank(), i);
    preimage.makeGeneratorRootSpace(this->domainAlgebra(), - simpleRoot);
    const ElementSemisimpleLieAlgebra<Rational>& image =
    this->imagesNegativeSimpleChevalleyGenerators[i];
    out
    << "\\("
    << preimage.toString(format)
    << "\\mapsto "
    << image.toString(format)
    << "\\)"
    << "\n\n";
    if (useHtml) {
      out << "<br>";
    }
  }
  out << "Images simple positive Chevalley generators:\n\n";
  if (useHtml) {
    out << "<br>";
  }
  for (
    int i = 0; i < this->imagesPositiveSimpleChevalleyGenerators.size; i ++
  ) {
    ChevalleyGenerator preimage;
    Vector<Rational> simpleRoot;
    simpleRoot.makeEi(this->domain->getRank(), i);
    preimage.makeGeneratorRootSpace(this->domainAlgebra(), simpleRoot);
    const ElementSemisimpleLieAlgebra<Rational>& image =
    this->imagesPositiveSimpleChevalleyGenerators[i];
    out
    << "\\("
    << preimage.toString(format)
    << "\\mapsto "
    << image.toString(format)
    << "\\)"
    << "\n\n";
    if (useHtml) {
      out << "<br>";
    }
  }
  out << "Maps of Chevalley generators:<br>\n";
  out << "\\(\\begin{array}{rcl}\n";
  for (int i = 0; i < this->domainAllChevalleyGenerators.size; i ++) {
    const ElementSemisimpleLieAlgebra<Rational>& preimage =
    this->domainAllChevalleyGenerators[i];
    const ElementSemisimpleLieAlgebra<Rational>& image =
    this->imagesAllChevalleyGenerators[i];
    out
    << preimage.toString(format)
    << " &\\mapsto& "
    << image.toString(format)
    << "\\\\\n";
  }
  out << "\\end{array}\\)";
  return out.str();
}

class SlTwoInSlN;

void HomomorphismSemisimpleLieAlgebra::
getRestrictionAmbientRootSystemToTheSmallerCartanSubalgebra(
  Vectors<Rational>& output
) {
  List<Vector<Rational> >& rootSystem =
  this->coDomainAlgebra().weylGroup.rootSystem;
  int rankSubalgebra = this->domainAlgebra().weylGroup.getDimension();
  Matrix<Rational> invertedCartan;
  invertedCartan = this->domainAlgebra().weylGroup.cartanSymmetric;
  invertedCartan.invert();
  int numberOfPositiveRootsDomain =
  this->domainAlgebra().weylGroup.rootsOfBorel.size;
  output.setSize(rootSystem.size);
  Vector<Rational> scalarProducts;
  scalarProducts.setSize(rankSubalgebra);
  for (int i = 0; i < rootSystem.size; i ++) {
    for (int j = 0; j < rankSubalgebra; j ++) {
      ElementSemisimpleLieAlgebra<Rational>& currentH =
      this->imagesAllChevalleyGenerators[j + numberOfPositiveRootsDomain];
      scalarProducts[j] =
      this->coDomainAlgebra().weylGroup.rootScalarCartanRoot(
        currentH.getCartanPart(), rootSystem[i]
      );
    }
    invertedCartan.actOnVectorColumn(scalarProducts, output[i]);
  }
  this->imagesCartanDomain.setSize(rankSubalgebra);
  for (int i = 0; i < rankSubalgebra; i ++) {
    this->imagesCartanDomain[i] =
    this->imagesAllChevalleyGenerators[i + numberOfPositiveRootsDomain].
    getCartanPart();
  }
}

bool HomomorphismSemisimpleLieAlgebra::checkInitialization() {
  if (this->domain == nullptr || this->coDomain == nullptr) {
    global.fatal
    << "Uninitialized homomorphism of semisimple Lie algebras"
    << global.fatal;
  }
  return true;
}

bool HomomorphismSemisimpleLieAlgebra::checkIsHomomorphism() {
  STACK_TRACE("HomomorphismSemisimpleLieAlgebra::checkIsHomomorphism");
  ElementSemisimpleLieAlgebra<Rational> coDomainElement;
  ElementSemisimpleLieAlgebra<Rational> domainElement;
  ElementSemisimpleLieAlgebra<Rational> imageOfDomainElement;
  for (int i = 0; i < this->imagesAllChevalleyGenerators.size; i ++) {
    for (int j = 0; j < this->imagesAllChevalleyGenerators.size; j ++) {
      this->coDomainAlgebra().lieBracket(
        this->imagesAllChevalleyGenerators[i],
        this->imagesAllChevalleyGenerators[j],
        coDomainElement
      );
      this->coDomainAlgebra().lieBracket(
        this->domainAllChevalleyGenerators[i],
        this->domainAllChevalleyGenerators[j],
        domainElement
      );
      this->applyHomomorphism(domainElement, imageOfDomainElement);
      if (imageOfDomainElement != coDomainElement) {
        global.fatal
        << "Applying the homomorphism first, then Lie bracket "
        << "does not match doing Lie bracket first, then homomorphism."
        << global.fatal;
        return false;
      }
    }
  }
  return true;
}

bool HomomorphismSemisimpleLieAlgebra::checkClosednessLieBracket() {
  STACK_TRACE("HomomorphismSemisimpleLieAlgebra::checkClosednessLieBracket");
  ElementSemisimpleLieAlgebra<Rational> element;
  Vectors<Rational> roots;
  Vector<Rational> root;
  roots.setSize(this->imagesAllChevalleyGenerators.size);
  for (int i = 0; i < roots.size; i ++) {
    this->imagesAllChevalleyGenerators[i].toVectorNegativeRootSpacesFirst(
      roots[i], this->coDomainAlgebra()
    );
  }
  for (int i = 0; i < this->imagesAllChevalleyGenerators.size; i ++) {
    for (int j = 0; j < this->imagesAllChevalleyGenerators.size; j ++) {
      this->coDomainAlgebra().lieBracket(
        this->imagesAllChevalleyGenerators[i],
        this->imagesAllChevalleyGenerators[j],
        element
      );
      if (element.isEqualToZero()) {
        continue;
      }
      element.toVectorNegativeRootSpacesFirst(root);
      if (!roots.linearSpanContainsVector(root)) {
        return false;
      }
    }
  }
  return true;
}

void ChevalleyGenerator::makeGeneratorRootSpace(
  SemisimpleLieAlgebra& inputOwner, const Vector<Rational>& root
) {
  this->makeGenerator(
    inputOwner, inputOwner.getGeneratorIndexFromRoot(root)
  );
}

void ChevalleyGenerator::makeGenerator(
  SemisimpleLieAlgebra& inputOwner, int inputGeneratorIndex
) {
  this->owner = &inputOwner;
  this->generatorIndex = inputGeneratorIndex;
}

void ChevalleyGenerator::checkConsistencyWithOther(
  const ChevalleyGenerator& other
) const {
  if (this->owner != other.owner) {
    global.fatal
    << "Attempt to compare Chevalley generators of different Lie algebras. "
    << global.fatal;
  }
}

bool ChevalleyGenerator::checkInitialization() const {
  if (this->owner == nullptr) {
    global.fatal
    << "Attempt to use a non-initialized Chevalley generator. "
    << global.fatal;
    return false;
  }
  return true;
}

bool ChevalleyGenerator::isInCartan(Vector<Rational>* whichElement) const {
  this->checkInitialization();
  bool result = this->owner->isGeneratorFromCartan(this->generatorIndex);
  if (result && whichElement != nullptr) {
    whichElement->makeEi(
      this->owner->getRank(),
      this->owner->getCartanCoordinateIndexFromCartanGeneratorIndex(
        this->generatorIndex
      )
    );
  }
  return result;
}

bool ChevalleyGenerator::isInRootSpace(Vector<Rational>* whichRootSpace) const {
  this->checkInitialization();
  bool result = !this->owner->isGeneratorFromCartan(this->generatorIndex);
  if (result && whichRootSpace != nullptr) {
    *whichRootSpace = this->owner->getWeightOfGenerator(this->generatorIndex);
  }
  return result;
}

std::string ChevalleyGenerator::toString(FormatExpressions* inputFormat) const {
  this->checkInitialization();
  return
  this->owner->getStringFromChevalleyGenerator(
    this->generatorIndex, inputFormat
  );
}

bool ChevalleyGenerator::operator>(const ChevalleyGenerator& other) const {
  return this->generatorIndex > other.generatorIndex;
}

std::string SemisimpleLieAlgebra::getStringFromChevalleyGenerator(
  int index, FormatExpressions* polynomialFormat
) const {
  std::stringstream out;
  MemorySaving<FormatExpressions> tempFormat;
  if (polynomialFormat == nullptr) {
    polynomialFormat = &tempFormat.getElement();
  }
  if (this->isGeneratorFromCartan(index)) {
    out
    << polynomialFormat->chevalleyHGeneratorLetter
    << "_{"
    << index - this->getNumberOfPositiveRoots() + 1
    << "}";
  } else {
    out << polynomialFormat->chevalleyGGeneratorLetter << "_{";
    if (index >= this->getNumberOfPositiveRoots()) {
      out << index - this->getNumberOfPositiveRoots() - this->getRank() + 1;
    } else {
      out << index - this->getNumberOfPositiveRoots();
    }
    out << "}";
  }
  return out.str();
}

void SemisimpleLieAlgebra::orderNilradicalFirstTotalWeightDescending(
  const Selection& parabolicSelectionZeroMeansLeviPart
) {
  STACK_TRACE(
    "SemisimpleLieAlgebra::orderNilradicalFirstTotalWeightDescending"
  );
  Vector<Rational> selection;
  selection = parabolicSelectionZeroMeansLeviPart;
  for (int i = 0; i < this->getNumberOfGenerators(); i ++) {
    if (this->getWeightOfGenerator(i).scalarEuclidean(selection) < 0) {
      this->universalEnvelopingGeneratorOrder[i] =
      - i - this->getNumberOfGenerators() * 5;
    }
  }
}

void SemisimpleLieAlgebra::orderNilradicalFirstTotalWeightAscending(
  const Selection& parabolicSelectionZeroMeansLeviPart
) {
  STACK_TRACE(
    "SemisimpleLieAlgebra::orderNilradicalFirstTotalWeightDescending"
  );
  Vector<Rational> tempVect;
  tempVect = parabolicSelectionZeroMeansLeviPart;
  for (int i = 0; i < this->getNumberOfGenerators(); i ++) {
    if (this->getWeightOfGenerator(i).scalarEuclidean(tempVect) < 0) {
      this->universalEnvelopingGeneratorOrder[i] =
      i - this->getNumberOfGenerators() * 5;
    }
  }
}

void SemisimpleLieAlgebra::orderNilradicalNilWeightAscending(
  const Selection& parSelZeroMeansLeviPart
) {
  Vector<Rational> tempVect;
  tempVect = parSelZeroMeansLeviPart;
  for (int i = 0; i < this->getNumberOfGenerators(); i ++) {
    Rational translationCoeff =
    this->getWeightOfGenerator(i).scalarEuclidean(tempVect) *
    this->getNumberOfPositiveRoots();
    if (translationCoeff < 0) {
      this->universalEnvelopingGeneratorOrder[i] =
      i + translationCoeff.numeratorShort * this->getNumberOfGenerators() * 5;
    }
  }
}

void SemisimpleLieAlgebra::orderNilradicalNilWeightDescending(
  const Selection& parSelZeroMeansLeviPart
) {
  Vector<Rational> tempVect;
  tempVect = parSelZeroMeansLeviPart;
  for (int i = 0; i < this->getNumberOfGenerators(); i ++) {
    Rational translationCoeff =
    this->getWeightOfGenerator(i).scalarEuclidean(tempVect) *
    this->getNumberOfPositiveRoots();
    if (translationCoeff < 0) {
      this->universalEnvelopingGeneratorOrder[i] =
      - i +
      translationCoeff.numeratorShort * this->getNumberOfGenerators() * 5;
    }
  }
}

void SemisimpleLieAlgebra::orderNilradical(
  const Selection& parabolicSelectionZeroMeansLeviPart,
  bool useNilWeight,
  bool ascending
) {
  STACK_TRACE("SemisimpleLieAlgebra::orderNilradical");
  if (useNilWeight) {
    if (ascending) {
      this->orderNilradicalNilWeightAscending(
        parabolicSelectionZeroMeansLeviPart
      );
    } else {
      this->orderNilradicalNilWeightDescending(
        parabolicSelectionZeroMeansLeviPart
      );
    }
  } else {
    if (ascending) {
      this->orderNilradicalFirstTotalWeightAscending(
        parabolicSelectionZeroMeansLeviPart
      );
    } else {
      this->orderNilradicalFirstTotalWeightDescending(
        parabolicSelectionZeroMeansLeviPart
      );
    }
  }
}

bool SemisimpleLieAlgebra::hasComputedStructureConstants() {
  return
  this->chevalleyConstants.numberOfRows ==
  this->weylGroup.dynkinType.getRootSystemSize();
}

void SemisimpleLieAlgebra::orderSSalgebraForHWbfComputation() {
  int numberOfPositiveRoots = this->getNumberOfPositiveRoots();
  for (int i = 0; i < numberOfPositiveRoots; i ++) {
    this->universalEnvelopingGeneratorOrder[i] = - 1;
  }
}

void SemisimpleLieAlgebra::orderStandardAscending() {
  int numGens = this->getNumberOfGenerators();
  for (int i = 0; i < numGens; i ++) {
    this->universalEnvelopingGeneratorOrder[i] = i;
  }
}

void SemisimpleLieAlgebra::orderStandardDescending() {
  int numGens = this->getNumberOfGenerators();
  for (int i = 0; i < numGens; i ++) {
    this->universalEnvelopingGeneratorOrder[i] = numGens - i - 1;
  }
}

bool SemisimpleLieAlgebra::areOrderedProperly(int leftIndex, int rightIndex) {
  return
  this->universalEnvelopingGeneratorOrder[leftIndex] <=
  this->universalEnvelopingGeneratorOrder[rightIndex];
}

bool SemisimpleLieAlgebra::isGeneratorFromCartan(int index) const {
  return
  index >= this->getNumberOfPositiveRoots() &&
  index < this->getNumberOfPositiveRoots() + this->getRank();
}

int SemisimpleLieAlgebra::getRootIndexFromDisplayIndex(int index) {
  int numberOfPositiveRoots = this->weylGroup.rootsOfBorel.size;
  if (index < 0) {
    return index + numberOfPositiveRoots;
  }
  if (index > 0) {
    return index + numberOfPositiveRoots - 1;
  }
  return - 1;
}

int SemisimpleLieAlgebra::getDisplayIndexFromRootIndex(int index) const {
  int numberOfPositiveRoots = this->weylGroup.rootsOfBorel.size;
  if (index >= numberOfPositiveRoots) {
    return index - numberOfPositiveRoots + 1;
  }
  if (index < numberOfPositiveRoots) {
    return index - numberOfPositiveRoots;
  }
  return - 10000000;
}

int SemisimpleLieAlgebra::getGeneratorFromRootIndex(int index) const {
  if (index < 0 || index >= this->weylGroup.rootSystem.size) {
    return - 1;
  }
  int dimension = this->weylGroup.cartanSymmetric.numberOfRows;
  int numberOfPositiveRoots = this->weylGroup.rootsOfBorel.size;
  if (index >= numberOfPositiveRoots) {
    return index + dimension;
  }
  return index;
}

int SemisimpleLieAlgebra::getDisplayIndexFromGenerator(int index) const {
  if (index < this->getNumberOfPositiveRoots()) {
    return index - this->getNumberOfPositiveRoots();
  }
  if (index >= this->getNumberOfPositiveRoots() + this->getRank()) {
    return index + 1 - this->getNumberOfPositiveRoots() - this->getRank();
  }
  return index - this->getNumberOfPositiveRoots();
}

int SemisimpleLieAlgebra::getGeneratorIndexFromNonZeroCoefficientIndexInCartan(
  int simpleRootIndex
) const {
  return simpleRootIndex + this->weylGroup.rootsOfBorel.size;
}

int SemisimpleLieAlgebra::getCartanCoordinateIndexFromCartanGeneratorIndex(
  int generatorIndex
) const {
  return generatorIndex - this->weylGroup.rootsOfBorel.size;
}

int SemisimpleLieAlgebra::getRootIndexFromGenerator(int index) const {
  int numberOfPositiveRoots = this->weylGroup.rootsOfBorel.size;
  int dimension = this->weylGroup.cartanSymmetric.numberOfRows;
  if (index < numberOfPositiveRoots) {
    return index;
  }
  if (index >= numberOfPositiveRoots + dimension) {
    return index - dimension;
  }
  return - 1;
}
