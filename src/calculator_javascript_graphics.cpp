//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#include "general_1_lists.h"
#include "vpfHeader2Math0_General.h"
#include "vpfHeader2Math9DrawingVariables.h"
ProjectInformationInstance ProjectInfoVpf5_2cpp(__FILE__, "Calculator javascript/html/graphics. ");

std::string CreateJavaScriptVectors(Vectors<double>& inputVectors, const std::string& arrayName, bool useVar) {
  MacroRegisterFunctionWithName("CreateStaticJavaScriptVectorsArrayWithProjection");
  if (inputVectors.size == 0) {
    return "";
  }
  std::stringstream out;
  out << "\n";
  if (useVar) {
    out << "var ";
  }
  out << arrayName << "= new Array(" << inputVectors.size << ");\n";
  int theDimension = inputVectors[0].size;
  for (int i = 0; i < inputVectors.size; i ++) {
    out << arrayName << "[" << i << "] =[";
    for (int j = 0; j < theDimension; j ++) {
      out << inputVectors[i][j];
      if (j != theDimension - 1) {
        out << ",";
      }
    }
    out <<  "];\n";
  }
  return out.str();
}

std::string CreateStaticJavaScriptVectorsArrayWithProjection(
  Vectors<double>& inputVectors, const std::string& arrayName, const std::string& projectionsName
) {
  MacroRegisterFunctionWithName("CreateStaticJavaScriptVectorsArrayWithProjection");
  if (inputVectors.size == 0)
    return "";
  std::stringstream out;
  out << "\nvar " << arrayName << "= new Array(" << inputVectors.size << ");\n";
  out << "var " << projectionsName << "= new Array(" << inputVectors.size << ");\n";

  int theDimension = inputVectors[0].size;
  for (int i = 0; i < inputVectors.size; i ++) {
    out << arrayName << "[" << i << "] =[";
    for (int j = 0; j < theDimension; j ++) {
      out << inputVectors[i][j];
      if (j != theDimension - 1) {
        out << ",";
      }
     }
    out <<  "];\n";
  }
  for (int i = 0; i < inputVectors.size; i ++) {
    ////////////////////
    out << projectionsName << "[" << i << "] =[0,0];\n";
  }
  return out.str();
}

std::string CreateStaticJavaScriptListVectorsWithProjection(
  List<Vectors<double> >& inputVectors, const std::string& arrayName, const std::string& projectionsName
) {
  MacroRegisterFunctionWithName("CreateStaticJavaScriptListVectorsWithProjection");
  if (inputVectors.size == 0) {
    return "";
  }
  std::stringstream out;
  out << "\nvar " << arrayName << "= new Array(" << inputVectors.size << ");\n";
  out << "var " << projectionsName << "= new Array(" << inputVectors.size << ");\n";
  for (int i = 0; i < inputVectors.size; i ++) {
    std::stringstream projNamesWithIndex, arrayNameWithIndex;
    arrayNameWithIndex << arrayName << "[" << i << "]";
    projNamesWithIndex << projectionsName << "[" << i << "]";
    out << CreateStaticJavaScriptVectorsArrayWithProjection(inputVectors[i], arrayNameWithIndex.str(), projNamesWithIndex.str());
  }
  return out.str();
}

std::string CreateJavaScriptListVectors(List<Vectors<double> >& inputVectors, const std::string& arrayName) {
  MacroRegisterFunctionWithName("CreateStaticJavaScriptListVectors");
  if (inputVectors.size == 0) {
    return "";
  }
  std::stringstream out;
  out << "\nvar " << arrayName << "= new Array(" << inputVectors.size << ");\n";
  for (int i = 0; i < inputVectors.size; i ++) {
    std::stringstream arrayNameWithIndex;
    arrayNameWithIndex << arrayName << "[" << i << "]";
    out << CreateJavaScriptVectors(inputVectors[i], arrayNameWithIndex.str(), false);
  }
  return out.str();
}

std::string CreateStaticJavaScriptTextArray(List<std::string>& theLabels, const std::string& arrayName) {
  MacroRegisterFunctionWithName("CreateStaticJavaScriptVectorsArrayWithProjection");
  if (theLabels.size == 0) {
    return "";
  }
  std::stringstream out;
  out << "var " << arrayName << "= new Array(" << theLabels.size << ");\n";
  for (int i = 0; i < theLabels.size; i ++) {
    out << arrayName << "[" << i << "] =\"" << theLabels[i] << "\";\n";
  }
  return out.str();
}

std::string DrawingVariables::GetHtmlDiv(int theDimension) {
  std::stringstream out;
  this->NumHtmlGraphics ++;
  std::string idCanvas = "idCanvasNDimensionalGraphics" + std::to_string(this->NumHtmlGraphics);
  std::string idHighlightInformation = "idHighlightInfoNDimensionalGraphics" + std::to_string(this->NumHtmlGraphics);
  std::string idSpanInformation = "idCanvasInfoNDimensionalGraphics" + std::to_string(this->NumHtmlGraphics);
  std::string graphicsVar = "drawGraphics" + std::to_string(this->NumHtmlGraphics);
  out << "<canvas width = '" << this->DefaultHtmlWidth << "' "
  << "height = '" << this->DefaultHtmlHeight << "'"
  << " id = '" << idCanvas << "'>Canvas not supported</canvas><br>";
  out << "<div id = '" << idHighlightInformation << "'></div><br>";
  out << "<span id = '" << idSpanInformation << "'></span><br>";
  JSData theData;
  theData["idCanvas"] = idCanvas;
  theData["idSpanInformation"] = idSpanInformation;
  theData["idHighlightInformation"] = idHighlightInformation;
  theData["widthHTML"] = this->DefaultHtmlWidth;
  theData["heightHTML"] = this->DefaultHtmlHeight;
  theData["screenBasis"] = this->theBuffer.BasisProjectionPlane;
  theData["draggablePoints"] = this->theBuffer.BasisToDrawCirclesAt;
  theData["bilinearForm"] = this->theBuffer.theBilinearForm;
  theData["graphicsUnit"] = this->theBuffer.GraphicsUnit;
  theData["frameLength"] = this->theBuffer.frameLengthInMilliseconds;
  std::string drawObjects = "drawObjects";
  theData[drawObjects].theType = JSData::token::tokenArray;
  theData[drawObjects].theList = this->theBuffer.theOperations;
  theData["dimension"] = theDimension;
  out << "<script>\n";
  out << "var " << graphicsVar << " = " << theData.ToString(false, false) << ";\n";
  out << "window.calculator.graphicsNDimensions.createGraphicsFromObject(" << graphicsVar << ");\n";
  out << "</script>";
  return out.str();
}

std::string DrawingVariables::GetHtmlFromDrawOperationsCreateDivWithUniqueName(int theDimension) {
  return this->GetHtmlDiv(theDimension);
}
