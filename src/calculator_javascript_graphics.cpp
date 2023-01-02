#include "general_lists.h"
#include "math_general.h"
#include "math_extra_drawing_variables.h"
#include "crypto.h"
#include "calculator_interface.h"

std::string DrawingVariables::getHTMLDiv(
  int dimension, bool useSpanTag, bool generateInfoPanels
) {
  STACK_TRACE("DrawingVariables::getHTMLDiv");
  JSData data;
  data["widthHTML"] = this->defaultHtmlWidth;
  data["heightHTML"] = this->defaultHtmlHeight;
  data["screenBasis"] = this->basisProjectionPlane;
  data["draggablePoints"] = this->basisToDrawCirclesAt;
  data["bilinearForm"] = this->bilinearForm;
  data["graphicsUnit"] = this->graphicsUnit;
  data["frameLength"] = this->frameLengthInMilliseconds;
  std::string drawObjects = "drawObjects";
  data[drawObjects].elementType = JSData::token::tokenArray;
  data[drawObjects].listObjects = this->operations;
  data["dimension"] = dimension;
  std::string graphicsId =
  Crypto::convertStringToHex(
    Crypto::computeSha256(data.toString()), 0, false
  );
  std::string idHighlightInformation =
  "idHighlightInfoNDimensionalGraphics" + graphicsId;
  std::string idSpanInformation =
  "idCanvasInfoNDimensionalGraphics" + graphicsId;
  if (generateInfoPanels) {
    data["idSpanInformation"] = idSpanInformation;
    data["idHighlightInformation"] = idHighlightInformation;
  }
  std::string idCanvas = "idCanvasNDimensionalGraphics" + graphicsId;
  data["idCanvas"] = idCanvas;
  std::stringstream out;
  std::string graphicsVar = "drawGraphics" + graphicsId;
  out
  << "\n<canvas width = '"
  << this->defaultHtmlWidth
  << "' "
  << "height = '"
  << this->defaultHtmlHeight
  << "'"
  << " id = '"
  << idCanvas
  << "'>Canvas not supported</canvas>\n<br>\n";
  if (generateInfoPanels) {
    out << "<div id='" << idHighlightInformation << "'></div>\n<br>\n";
    out << "<span id='" << idSpanInformation << "'></span>\n<br>\n";
  }
  if (useSpanTag) {
    out << HtmlRoutines::jsonContainer("graphicsNDimensional", data) << "\n";
  } else {
    out << HtmlRoutines::scriptFromJSON("graphicsNDimensional", data) << "\n";
  }
  return out.str();
}
