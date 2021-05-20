// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "general_lists.h"
#include "math_general.h"
#include "math_extra_drawing_variables.h"
#include "crypto.h"

std::string DrawingVariables::getHTMLDiv(
  int dimension, bool useSpanTag
) {
  JSData data;
  data["widthHTML"] = this->defaultHtmlWidth;
  data["heightHTML"] = this->defaultHtmlHeight;
  data["screenBasis"] = this->operations.basisProjectionPlane;
  data["draggablePoints"] = this->operations.basisToDrawCirclesAt;
  data["bilinearForm"] = this->operations.bilinearForm;
  data["graphicsUnit"] = this->operations.graphicsUnit;
  data["frameLength"] = this->operations.frameLengthInMilliseconds;
  std::string drawObjects = "drawObjects";
  data[drawObjects].elementType = JSData::token::tokenArray;
  data[drawObjects].listObjects = this->operations.theOperations;
  data["dimension"] = dimension;

  std::string graphicsId = Crypto::convertStringToHex(Crypto::computeSha256(data.toString()), 0, false);
  std::string idCanvas = "idCanvasNDimensionalGraphics" + graphicsId;
  std::string idHighlightInformation = "idHighlightInfoNDimensionalGraphics" + graphicsId;
  std::string idSpanInformation = "idCanvasInfoNDimensionalGraphics" + graphicsId;
  data["idCanvas"] = idCanvas;
  data["idSpanInformation"] = idSpanInformation;
  data["idHighlightInformation"] = idHighlightInformation;

  std::stringstream out;
  std::string graphicsVar = "drawGraphics" + graphicsId;
  out << "\n<canvas width = '" << this->defaultHtmlWidth << "' "
  << "height = '" << this->defaultHtmlHeight << "'"
  << " id = '" << idCanvas << "'>Canvas not supported</canvas>\n<br>\n";
  out << "<div id = '" << idHighlightInformation << "'></div>\n<br>\n";
  out << "<span id = '" << idSpanInformation << "'></span>\n<br>\n";
  if (useSpanTag) {
    out << HtmlRoutines::jsonContainer("graphicsNDimensional", data) << "\n";
  } else {
    out << HtmlRoutines::scriptFromJSON("graphicsNDimensional", data) << "\n";
  }
  return out.str();
}
