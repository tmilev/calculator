// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "general_lists.h"
#include "math_general.h"
#include "math_extra_drawing_variables.h"
#include "crypto.h"

std::string DrawingVariables::getHTMLDiv(int theDimension) {
  JSData theData;
  theData["widthHTML"] = this->DefaultHtmlWidth;
  theData["heightHTML"] = this->DefaultHtmlHeight;
  theData["screenBasis"] = this->theBuffer.BasisProjectionPlane;
  theData["draggablePoints"] = this->theBuffer.basisToDrawCirclesAt;
  theData["bilinearForm"] = this->theBuffer.theBilinearForm;
  theData["graphicsUnit"] = this->theBuffer.graphicsUnit;
  theData["frameLength"] = this->theBuffer.frameLengthInMilliseconds;
  std::string drawObjects = "drawObjects";
  theData[drawObjects].theType = JSData::token::tokenArray;
  theData[drawObjects].theList = this->theBuffer.theOperations;
  theData["dimension"] = theDimension;

  std::string graphicsId = Crypto::convertStringToHex(Crypto::computeSha256(theData.toString()), 0, false);
  std::string idCanvas = "idCanvasNDimensionalGraphics" + graphicsId;
  std::string idHighlightInformation = "idHighlightInfoNDimensionalGraphics" + graphicsId;
  std::string idSpanInformation = "idCanvasInfoNDimensionalGraphics" + graphicsId;
  theData["idCanvas"] = idCanvas;
  theData["idSpanInformation"] = idSpanInformation;
  theData["idHighlightInformation"] = idHighlightInformation;

  std::stringstream out;
  std::string graphicsVar = "drawGraphics" + graphicsId;
  out << "<canvas width = '" << this->DefaultHtmlWidth << "' "
  << "height = '" << this->DefaultHtmlHeight << "'"
  << " id = '" << idCanvas << "'>Canvas not supported</canvas><br>";
  out << "<div id = '" << idHighlightInformation << "'></div><br>";
  out << "<span id = '" << idSpanInformation << "'></span><br>";
  out << "<script>\n";
  out << "var " << graphicsVar << " = " << theData.toString(nullptr) << ";\n";
  out << "window.calculator.graphicsNDimensions.createGraphicsFromObject(" << graphicsVar << ");\n";
  out << "</script>";
  return out.str();
}
