#include "crypto_calculator.h"
#include "general_lists.h"
#include "html_routines.h"
#include "math_extra_drawing_variables.h"
#include "math_general.h"

void DrawingVariables::toJSON(JSData& output) {
  output.makeEmptyArray();
  for (DrawOperation& operation : this->operations) {
    JSData next;
    operation.toJSON(next, *this);
    output.listObjects.addOnTop(next);
  }
}

std::string DrawingVariables::getHTMLDiv(
  int dimension, bool generateInfoPanels
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
  data[drawObjects].elementType = JSData::Type::tokenArray;
  this->toJSON(data[drawObjects]);
  data["dimension"] = dimension;
  std::string graphicsId =
  Crypto::convertStringToHex(Crypto::computeSha256(data.toString()), 0, false);
  std::string idHighlightInformation =
  "idHighlightInfoNDimensionalGraphics" + graphicsId;
  std::string idSpanInformation =
  "idCanvasInfoNDimensionalGraphics" + graphicsId;
  if (generateInfoPanels) {
    data["idSpanInformation"] = idSpanInformation;
    data["idHighlightInformation"] = idHighlightInformation;
  }
  std::string idCanvas = "idCanvasNDimensionalGraphics" + graphicsId;
  std::string idControls = "idNDimensionalGraphicsControls" + graphicsId;
  data["idCanvas"] = idCanvas;
  data["idControls"] = idControls;
  std::stringstream out;
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
  out << "<div id='" << idControls << "'></div>";
  if (generateInfoPanels) {
    out << "<div id='" << idHighlightInformation << "'></div>\n<br>\n";
    out << "<span id='" << idSpanInformation << "'></span>\n<br>\n";
  }
  out << HtmlRoutines::jsonContainer("graphicsNDimensional", data) << "\n";
  return out.str();
}
