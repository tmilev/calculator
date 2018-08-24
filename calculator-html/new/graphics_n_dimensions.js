"use strict";

//Copyright (c) 2016, Todor Emilov Milev
//This file is licensed to you under the permissive MIT license.
//The license text is included below for your convenience (copied from Wikipedia).

//Permission is hereby granted, free of charge,
//to any person obtaining a copy of this software and associated documentation files
//(the "Software"), to deal in the Software without restriction,
//including without limitation the rights to use,
//copy, modify, merge, publish, distribute, sublicense,
//and/or sell copies of the Software, and to permit persons to whom
//the Software is furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included
//in all copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
//DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

function GraphicsNDimensions(inputIdCanvas, inputIdInfo) {
  this.vectors = [];
  this.vectorProjections = [];
  this.drawOperations = [];
  this.idCanvas = inputIdCanvas;
  this.idInfo = inputIdInfo;
  this.idPlaneInfo = `${inputIdInfo}projectionPlane`;
  this.basisCircles = [];
  this.projectionsBasisCircles = [];
  this.eiBasis = [];
  this.projectionsEiVectors = [];
  this.labeledVectors = [];
  this.projectionsLabeledVectors = [];
  this.projectionScreenBasis = [];
  this.screenBasis = [];
  this.shift = [0 , 0];
  this.dimension = 0;
  this.theBilinearForm = [];
  this.graphicsUnit = 0;
  this.selectedBasisIndex = - 1;
  this.flagAllowMovingCoordinateSystemFromArbitraryClick = true;

  var theDiv = document.getElementById(this.idCanvas);
  theDiv.addEventListener("DOMMouseScroll", this.mouseHandleWheel.bind(this));
}

GraphicsNDimensions.prototype.drawAll = function () {

}

GraphicsNDimensions.prototype.initVectors = function(inputVectors) {
  this.vectors = inputVectors;
  this.vectorProjections = [];
  for (var counterVector = 0; counterVector < this.vectors.length; counterVector ++) {
    this.vectorProjections.push([0, 0]);
  }
}

GraphicsNDimensions.prototype.writeInfo = function() {
  var result = "";
  result += "<br>The projection plane (drawn on the screen) is spanned by the following two vectors <br>\n";
  result += `(${this.eiBasis[0].join(", ")})<br>`;
  result += `(${this.eiBasis[1].join(", ")})<br>`;
  document.getElementById(this.idPlaneInfo).innerHTML = result;
}

GraphicsNDimensions.prototype.initInfo = function () {

  var result = "";
  for (var i = 0; i < 2; i ++) { 
    for (var j = 0; j < this.dimension; j ++) { 
      result += `<textarea rows = "1" cols = "2" id = "${this.idCanvas}textEbasis_${i}_${j}"></textarea>`;
      result += "</textarea>\n";
    }
    result += "<br>";
  }
  result += `<button onclick = "startProjectionPlaneUser('${this.idCanvas}')>`;
  result += `Change to basis</button><br>`;
  result += `<button onclick = "snapShotLaTeX('${this.idCanvas}')>LaTeX snapshot</button>`;
  result += `<span id = "${this.idCanvas}snapShotLateXspan"> </span>\n`;
  document.getElementById(this.idInfo).innerHTML = result
}
  
GraphicsNDimensions.prototype.snapShotLaTeX = function() {
  var textComponent = document.getElementById(`${this.idCanvas}snapShotLateXspan`);
  var result = "";
  result += `\\documentclass{article} \\usepackage{auto-pst-pdf}<br>`;
  result += `\n%\\usepackage{pst-plot}<br>\n\\begin{document}<br>\n`;
  result += `\\psset{xunit = 0.01cm, yunit = 0.01cm} <br>\n\\begin{pspicture}(0,0)(1,1)\n`;
  ComputeProjections();
  for (var counterDrawOperation = 0; counterDrawOperation < this.drawOperations.length; counterDrawOperation ++) { 
    result += this.drawOperations[counterDrawOperation].getLaTeXOperation(); 
  //  case DrawOperations::typeDrawLineBetweenTwoVectors:
  //  out << "theText.innerHTML+=\""
  //  << this->GetColorPsTricksFromColorIndex(this->theBuffer.theDrawLineBetweenTwoRootsOperations[currentIndex].ColorIndex)
  //  << "<br>\";\n";
  //  out << "theText.innerHTML+=\"\\\\psline[linecolor = currentColor](\"+ "
  //  << functionConvertToXYName << "( " << Points1ArrayName << "[" << currentIndex<< "])[0]"
  //  << "+\",\"+"
  //  << functionConvertToXYName << "( " << Points1ArrayName << "["
  //  << currentIndex << "])[1]*- 1 +\")\";\n ";
  //  out << "theText.innerHTML+=\"(\"+"
  //  << functionConvertToXYName << "( " << Points2ArrayName << "["
  //  << currentIndex << "])[0] +\",\"+"
  //  << " " << functionConvertToXYName << "( " << Points2ArrayName << "["
  //  << currentIndex << "])[1]*- 1+\")<br>\"; \n";
  //  break;
  //case DrawOperations::typeDrawCircleAtVector:
  //  out << "theText.innerHTML+=\""
  //  << this->GetColorPsTricksFromColorIndex(this->theBuffer.theDrawCircleAtVectorOperations[currentIndex].ColorIndex)
  //  << "<br>\";\n";
  //  out << "theText.innerHTML+=\"\\\\pscircle[linecolor = currentColor](\"+ "
  //  << functionConvertToXYName << "( " << circArrayName << "["
  //  << currentIndex << "])[0]" << "+\",\"+"
  //  << functionConvertToXYName << "( " << circArrayName << "["
  //  << currentIndex << "])[1]*- 1" << "+ \"){\"+"
  //  << (((double)this->theBuffer.theDrawCircleAtVectorOperations[currentIndex].radius) / 40) << "+\"}<br>\";\n";
  //  break;
  //case DrawOperations::typeFilledShape:
/*//        out << "theText.innerHTML+=\""
  //  << this->GetColorPsTricksFromColorIndex
  //  (this->theBuffer.theParallelograms[currentIndex].ColorIndex)
  //  << "<br>\";\n";
  //  out << "theText.innerHTML+=\"\\\\pscustom[linecolor = currentColor](\"+ "
  //  << functionConvertToXYName << "( " << circArrayName << "["
  //  << currentIndex << "])[0]" << "+\",\"+"
  //  << functionConvertToXYName << "( " << circArrayName << "["
  //  << currentIndex << "])[1]*- 1" << "+ \"){\"+"
  //  << (((double)this->theBuffer.theDrawCircleAtVectorOperations[currentIndex].radius)/40) << "+\"}<br>\";\n";*/
  //  nonImplementedFound = true;
  //  break;
  //case DrawOperations::typeDrawTextAtVector:
  //  out << "theText.innerHTML+=\""
  //  << this->GetColorPsTricksFromColorIndex(this->theBuffer.theDrawTextAtVectorOperations[currentIndex].ColorIndex)
  //  << "<br>\";\n";
  //  out << "theText.innerHTML+=\"\\\\rput[b](\"+ "
  //  << functionConvertToXYName << "( " << txtArrayName << "["
  //  << currentIndex << "])[0]" << "+\",\"+"
  //  << functionConvertToXYName << "( " << txtArrayName << "["
  //  << currentIndex << "])[1]*- 1" << "+ \"){\\\\color{currentColor}"
  //  << this->theBuffer.theDrawTextAtVectorOperations[currentIndex].theText << "}<br>\";\n";
  //  break;
  //default: break;
  }
//  stOutput << "<hr>DEBUG: got to here pt 2";
  //  out << theSurfaceName << ".stroke();\n";
  result += "\\end{pspicture}<br>";
  if (nonImplementedFound) {
    result += `Not all elements in the html picture were drawn in LaTex. To do: fix this.`;
  }
  result += "\\end{document}";
  textComponent.innerHTML = result;
}

function setBilinearForm(idCanvas, row, column) {

}
  
GraphicsNDimensions.prototype.getBilinearFormInput = function () {
  var result = "";
  result += "The bilinear form of the vector space follows. The ij^th element ";
  result += " gives the scalar product of e_i and e_j. If you enter a degenerate or non-positive definite ";
  result += " symmetric bilinear form the javascript might crash. You are expected to enter";
  result += " a symmetric strictly positive definite matrix. <br> \n";
  for (var counterRow = 0; counterRow < this.theBilinearForm.length; counterRow ++) { 
    for (var counterColumn = 0; counterColumn < this.theBilinearForm[counterRow].NumCols; counterColumn ++) { 
      var idBilinearForm = `${this.idCanvas}textBilinearForm_${i}_${j}`;
      result += `<textarea rows = "1" cols = "2" id = "${idBilinearForm}" `;
      result += `onChange = "setBilinearForm('this.idCanvas', ${counterRow}, ${counterColumn})">`;
      result += this.theBilinearForm[i][j];
      result += "</textarea>";
    }
    result += "<br>";
  }
  result += `<button id = "canvasRedraw('${theCanvasId}');">redraw</button>`;
  result += "<br> Left click + hold+ move the mouse on a special vector = rotates the special vector. ";
  result += "<br>Moving a vector rotates ``infinitesimally'' the projection plane of your screen ";
  result += "<br>1) inside the projection plane ";
  result += "<br>2) in the plane spanned by the selected vector and its orthogonal complement relative to the projection plane. ";
  result += "<br>The angle change matches the motion of your mouse pointer.  ";
  result += " Special care must be taken if the selected vector lies ";
  result += "inside the projection plane or the selected vector is orthogonal to the projection plane. ";
  result += "If one of these cases happens, the picture might jump around a bit.";
  result += "<br>The mouse wheel zooms in/out. ";
  result += " Zooming is tested to work on Firefox and google Chrome browsers on Ubuntu.";
  result += `<br>${this.drawOperations.length} elements drawn. `;
  var textComponent = document.getElementById(`${this.idCanvas}snapShotLateXspan`);
  textComponent.innerHTML = result;
}

GraphicsNDimensions.prototype.modifyBasisToOrthonormalNoShiftSecond = function() {

} 

GraphicsNDimensions.prototype.init = function() {
  if (this.ProjectionsEiVectors.length != this.dimension || this.theBilinearForm.length != this.dimension) {
    this.theBuffer.MakeMeAStandardBasis(theDimension);
  }
  this.theBilinearForm = new Array(this.dimension);
  for (var counterDimension = 0; counterDimension < this.dimension; counterDimension ++) { 
    this.theBilinearForm[counterDimension] = Array.fill(0, 0, this.dimension);
  }
  this.modifyBasisToOrthonormalNoShiftSecond();

  out << "function changeProjectionPlaneUser" << timesCalled << "(){\n"
  << "  frameCount" << timesCalled << "++;\n"
  << "  if (frameCount" << timesCalled << "> " << numFramesUserPlane << ")\n"
  << "    return;\n"
  << "  for (i = 0; i <" << theDimension << "; i ++)\n"
  << "  { "
  << "VectorE1Cone" << timesCalled << "[i] =VectorE1ConeGoal" << timesCalled << "[i]*"
  << "(frameCount" << timesCalled << "/" << numFramesUserPlane << ")"
  << "+ VectorE1ConeStart" << timesCalled << "[i]*"
  << "(1-frameCount" << timesCalled << "/" << numFramesUserPlane << ");\n"
  << "    VectorE2Cone" << timesCalled << "[i] =VectorE2ConeGoal" << timesCalled << "[i]*"
  << "(frameCount" << timesCalled << "/" << numFramesUserPlane << ")"
  << "+ VectorE2ConeStart" << timesCalled << "[i]*"
  << "(1-frameCount" << timesCalled << "/" << numFramesUserPlane << ");\n"
  << "  }\n"
  << "  MakeVectorE1AndE2orthonormal" << timesCalled << "();\n"
  << "  ComputeProjections" << timesCalled << "();\n"
  << "  " << theDrawFunctionName << "();\n"
  << "  window.setTimeout(\"changeProjectionPlaneUser" << timesCalled << "()\",100);\n"
  << "}";
//  stOutput << "<hr>DEBUG: got to here pt 4";
  if (this->theBuffer.BasisProjectionPlane.size > 2)
  { out << "BasisProjectionPlane" << timesCalled << "= new Array(" << this->theBuffer.BasisProjectionPlane.size << ");\n";
    for (int j = 0; j < this->theBuffer.BasisProjectionPlane.size; j ++)
    { out << "BasisProjectionPlane" << timesCalled << "[" << j << "] = new Array(2);\n";
      for (int k = 0; k < 2; k ++)
      { out << "BasisProjectionPlane" << timesCalled << "[" << j << "][" << k << "] = new Array(" << theDimension << ");\n";
        for (int l = 0; l < theDimension; l ++)
          out << "BasisProjectionPlane" << timesCalled << "[" << j << "][" << k << "][" << l << "] ="
          << this->theBuffer.BasisProjectionPlane[j][k][l] << ";\t";
        out << "\n";
      }
      out << "\n";
    }
    out << "function changeProjectionPlaneOnTimer" << timesCalled << "(){\n"
    << "if (frameCountGoesUp" << timesCalled << ")\n"
    << "  frameCount" << timesCalled << "++;\n"
    << "else\n"
    << "  frameCount" << timesCalled << "--;\n"
    << "if (frameCount" << timesCalled << "==" << this->theBuffer.BasisProjectionPlane.size - 1 << " || "
    << "frameCount" << timesCalled << "== 0)\n"
    << "{ \n"
    << "  frameCountGoesUp" << timesCalled << "=! frameCountGoesUp" << timesCalled << ";\n"
    << "}\n"
    << "if (frameCount" << timesCalled << ">= " << this->theBuffer.BasisProjectionPlane.size << ")\n"
    << "  return;"
    << "for (i = 0; i <" << theDimension << "; i ++)\n"
    << "{ VectorE1Cone" << timesCalled << "[i] =BasisProjectionPlane" << timesCalled
    << "[frameCount" << timesCalled << "][0][i];\n"
    << "  VectorE2Cone" << timesCalled << "[i] =BasisProjectionPlane" << timesCalled
    << "[frameCount" << timesCalled << "][1][i];\n"
    << "\n}\n";
    out << theDrawFunctionName << "();\n";
    out << "window.setTimeout(\"changeProjectionPlaneOnTimer" << timesCalled << "()\", 100);\n" << "}\n";
  }
//  stOutput << "<hr>DEBUG: got to here pt 5";

  out << CreateStaticJavaScriptVectorsArrayWithProjection
  (this->theBuffer.labeledVectors, labeledVectorsVarName, "proj" + labeledVectorsVarName);
  out << CreateStaticJavaScriptTextArray
  (this->theBuffer.labelsOfLabeledVectors, "labels" + labeledVectorsVarName);
  out << CreateJavaScriptListVectors(this->theBuffer.toBeHighlightedWhenLabeledVectorHovered, "highlight" + labeledVectorsVarName);
//  stOutput << "<hr>DEBUG: got to here pt 6";
  out << "var selectedLabels" << timesCalled << "= new Array(" << this->theBuffer.labeledVectors.size << ");\n";
  out << "var " << projName << "= new Array(" << theDimension << ");\n";
  out << "var " << eiBasis << "= new Array(" << theDimension << ");\n";
  for (int i = 0; i < theDimension; i ++)
    out << projName << "[" << i << "] = new Array(2);\n";
  out << "var " << basisCircles << "= new Array(" << this->theBuffer.BasisToDrawCirclesAt.size << ");\n";
  out << "var " << projBasisCircles << "= new Array(" << this->theBuffer.BasisToDrawCirclesAt.size << ");\n";
//  stOutput << "<hr>DEBUG: got to here pt 6.5";
  for (int i = 0; i < this->theBuffer.BasisToDrawCirclesAt.size; i ++)
  { out << basisCircles << "[" << i << "] =[";
    for (int j = 0; j < theDimension; j ++)
    { out << this->theBuffer.BasisToDrawCirclesAt[i][j];
      if (j != theDimension - 1)
        out << ",";
     }
    out <<  "];\n";
  }
//  stOutput << "<hr>DEBUG: got to here pt 7";
  for (int i = 0; i < theDimension; i ++)
  { ////////////////////
    out << eiBasis << "[" << i << "] =[";
    for (int j = 0; j < theDimension; j ++)
    { out << ((i == j) ? 1 : 0);
      if (j != theDimension - 1)
        out << ",";
     }
    out <<  "];\n";
    //////////////////
    out << projBasisCircles << "[" << i << "] = new Array(2);\n";
  }
  out << "var " << Points2ArrayName << "= new Array(" << this->theBuffer.theDrawLineBetweenTwoRootsOperations.size << ");\n";
  out << "var " << circArrayName << "= new Array(" << this->theBuffer.theDrawCircleAtVectorOperations.size << ");\n";
  out << "var " << txtArrayName << "= new Array(" << this->theBuffer.theDrawTextAtVectorOperations.size << ");\n";
  out << "var " << filledShapes << "= new Array(" << this->theBuffer.theShapes.size << ");\n";
  for (int i = 0; i < this->theBuffer.theShapes.size; i ++)
  { out << filledShapes << "[" << i << "] =[";
    DrawFilledShapeOperation& currentShape = this->theBuffer.theShapes[i];
    for (int j = 0; j < currentShape.theCorners.size; j ++)
    { out << currentShape.theCorners[j].ToStringSquareBracketsBasicType();
      if (j != currentShape.theCorners.size - 1)
        out << ", ";
    }
    out << "];\n";
  }
  out << "var " << Points1ArrayName << "=[ ";
  for (int i = 0; i < this->theBuffer.theDrawLineBetweenTwoRootsOperations.size; i ++)
  { Vector<double>& current1 = theBuffer.theDrawLineBetweenTwoRootsOperations[i].v1;
    out << current1.ToStringSquareBracketsBasicType();
    if (i < this->theBuffer.theDrawLineBetweenTwoRootsOperations.size - 1)
      out << ", ";
  }
  out << "];\n";
  out << "var " << Points2ArrayName << "=[";
  for (int i = 0; i < this->theBuffer.theDrawLineBetweenTwoRootsOperations.size; i ++)
  { Vector<double>& current2 = theBuffer.theDrawLineBetweenTwoRootsOperations[i].v2;
    out << current2.ToStringSquareBracketsBasicType();
    if (i < this->theBuffer.theDrawLineBetweenTwoRootsOperations.size - 1)
      out << ", ";
  }
  out << "];\n";
  for (int i = 0; i < this->theBuffer.theDrawCircleAtVectorOperations.size; i ++)
  { Vector<double>& current1 = theBuffer.theDrawCircleAtVectorOperations[i].theVector;
    out << circArrayName << "[" << i << "] =[";
    for (int j = 0; j < theDimension; j ++)
    { out << current1[j];
      if (j != theDimension - 1)
        out << ",";
    }
    out << "];\n";
  }
  for (int i = 0; i < this->theBuffer.theDrawTextAtVectorOperations.size; i ++)
  { Vector<double>& current1= theBuffer.theDrawTextAtVectorOperations[i].theVector;
    out << txtArrayName << "[" << i << "] =[";
    for (int j = 0; j < theDimension; j ++)
    { out << current1[j];
      if (j != theDimension - 1)
        out << ",";
    }
    out << "];\n";
  }
  //stOutput << "<hr>DEBUG: got to here pt 8";
  out << "var " << shiftX << "=" << this->theBuffer.centerX[0] << ";\n";
  out << "var " << shiftY << "=" << this->theBuffer.centerY[0] << ";\n";
  out << "var GraphicsUnitCone" << timesCalled << "=" << this->theBuffer.GraphicsUnit[0] << ";\n";
  out << "function " << functionConvertToXYName << "(vector){\n"
  << "  resultX =" << shiftX << ";\n"
  << "  resultY =" << shiftY << ";\n"
  << "  for (i = 0; i <" << theDimension << "; i ++){\n"
  << "    resultX+=vector[i]*" << projName << "[i][0];\n"
  << "    resultY+=vector[i]*" << projName << "[i][1];\n"
  << "  }\n"
  << "  result =[resultX, resultY];\n"
  << "  return result;\n"
  << "}\n";
  out << "var " << theSurfaceName << "= 0;\n";
  out << "function drawHighlights" << timesCalled << "() {\n"
  << "for (var i = 0; i <" << this->theBuffer.labeledVectors.size << "; i ++)\n"
  << "  if (selectedLabels" << timesCalled << "[i])\n"
  << "    for (var j = 0; j<highlight" << labeledVectorsVarName << "[i].length; j ++)\n"
  << "    { " << theSurfaceName << ".strokeStyle =\"#555555\";\n "
  << "      " << theSurfaceName << ".beginPath();\n "
  << "      " << theSurfaceName << ".arc("
  << functionConvertToXYName << "(highlight" << labeledVectorsVarName << "[i][j])[0],"
  << functionConvertToXYName << "(highlight" << labeledVectorsVarName << "[i][j])[1], 7"
  << ", 0, 2*Math.PI);\n "
  << "      " << theSurfaceName << ".stroke();\n"
  << "    }\n"
  << "}\n";
  out << "function " << theDrawFunctionName << "(){\n"
  << "  if (" << theSurfaceName << "== 0)\n"
  << "    " << theInitFunctionName << "();\n"
  << "ComputeProjections" << timesCalled << "();\n";
  out << theSurfaceName << ".fillStyle =\"#FFFFFF\";\n";
  out << theSurfaceName << ".fillRect(0,0," << this->DefaultHtmlWidth  << " ," << this->DefaultHtmlHeight << ");\n";
  //stOutput << "<hr>DEBUG: got to here pt 9";
  for (int i = 0; i < this->theBuffer.IndexNthDrawOperation.size; i ++)
  { int currentIndex = this->theBuffer.IndexNthDrawOperation[i];
    switch (theBuffer.TypeNthDrawOperation[i])
    { case DrawOperations::typeDrawLineBetweenTwoVectors:
        out << theSurfaceName << ".beginPath();\n ";
        out << theSurfaceName << ".strokeStyle =\""
        << this->GetColorHtmlFromColorIndex(this->theBuffer.theDrawLineBetweenTwoRootsOperations[currentIndex].ColorIndex)
        << "\"; ";
        out << theSurfaceName << ".lineWidth ="
        << FloatingPoint::DoubleToString
        (this->theBuffer.theDrawLineBetweenTwoRootsOperations[currentIndex].lineWidth)
        << "; ";
        out << theSurfaceName << ".moveTo("
        << functionConvertToXYName << "( " << Points1ArrayName << "[" << currentIndex << "])[0],"
        << functionConvertToXYName << "( " << Points1ArrayName << "["
        << currentIndex << "])[1]); ";
        out << theSurfaceName << ".lineTo("
        << functionConvertToXYName << "( " << Points2ArrayName << "["
        << currentIndex << "])[0],"
        << " " << functionConvertToXYName << "( " << Points2ArrayName << "["
        << currentIndex << "])[1]); ";
        out << theSurfaceName << ".stroke();\n";
        break;
      case DrawOperations::typeFilledShape:
        if (theDimension != 2)
          break;
        out << theSurfaceName << ".beginPath();\n ";
        out << theSurfaceName << ".fillStyle =\""
        << this->GetColorHtmlFromColorIndex(this->theBuffer.theShapes[currentIndex].ColorFillIndex)
        << "\";\n";
        out << theSurfaceName << ".lineWidth ="
        << FloatingPoint::DoubleToString(this->theBuffer.theShapes[currentIndex].lineWidth)
        << ";\n ";
        out << theSurfaceName << ".moveTo("
        << functionConvertToXYName << "( " << filledShapes << "[" << currentIndex << "][0])[0],"
        << functionConvertToXYName << "( " << filledShapes << "[" << currentIndex << "][0])[1]);\n ";
        out << "for (var i =1; i < " << filledShapes << "[" << currentIndex << "].length; i ++)\n";
        out << "  " << theSurfaceName << ".lineTo("
        << functionConvertToXYName << "( " << filledShapes << "[" << currentIndex << "][i])[0],"
        << functionConvertToXYName << "( " << filledShapes << "[" << currentIndex << "][i])[1]);\n ";
//        out << theSurfaceName << ".lineTo("
//        << functionConvertToXYName << "( " << filledShapes << "[" << currentIndex << "][0])[0],"
//        << functionConvertToXYName << "( " << filledShapes << "[" << currentIndex << "][0])[1]);\n ";
        out << theSurfaceName << ".closePath();\n";
        out << theSurfaceName << ".fill();\n";
        out << theSurfaceName << ".strokeStyle =\""
        << this->GetColorHtmlFromColorIndex(this->theBuffer.theShapes[currentIndex].ColorIndex)
        << "\";\n";
        out << theSurfaceName << ".stroke();\n";
        break;
      case DrawOperations::typeDrawCircleAtVector:
        out << theSurfaceName << ".strokeStyle =\""
        << this->GetColorHtmlFromColorIndex(this->theBuffer.theDrawCircleAtVectorOperations[currentIndex].ColorIndex)
        << "\"; ";
        out << theSurfaceName << ".beginPath(); ";
        out << theSurfaceName << ".arc("
        << functionConvertToXYName << "( " << circArrayName << "["
        << currentIndex << "])[0],"
        << functionConvertToXYName << "( " << circArrayName << "["
        << currentIndex << "])[1],"
        <<  this->theBuffer.theDrawCircleAtVectorOperations[currentIndex].radius << ", 0, 2*Math.PI); ";
        out << theSurfaceName << ".stroke();\n";
        break;
      case DrawOperations::typeDrawTextAtVector:
        out << theSurfaceName << ".strokeStyle =\""
        << this->GetColorHtmlFromColorIndex(this->theBuffer.theDrawTextAtVectorOperations[currentIndex].ColorIndex)
        << "\"; ";
        out << theSurfaceName << ".strokeText(\"" << this->theBuffer.theDrawTextAtVectorOperations[currentIndex].theText << "\", "
        << functionConvertToXYName << "( " << txtArrayName << "[" << currentIndex<< "])[0],"
        << functionConvertToXYName << "( " << txtArrayName << "[" << currentIndex << "])[1]);\n";
      default: break;
    }
  }
//    stOutput << " got to here pt 10C";

  out << "drawHighlights" << timesCalled << "();\n";
  out << "}\n";
  out << "function " << theInitFunctionName << "(){\n"
  << theSurfaceName << " = document.getElementById(\"" << theCanvasId << "\").getContext(\"2d\");\n"
  << theDrawFunctionName << "();\n";
  if (this->theBuffer.BasisProjectionPlane.size > 2)
    out << "window.setTimeout(\"changeProjectionPlaneOnTimer" << timesCalled << "()\",100);\n";
  out << " }\n";
  out << "var selectedBasisIndexCone" << timesCalled << "= - 1;\n";
  out << "var xShiftPointer" << timesCalled << "= 0;\n";
  out << "var yShiftPointer" << timesCalled << "= 0;\n";
  out << "var clickTolerance =5;\n"
  << "function ptsWithinClickToleranceCone" << timesCalled << "(x1, y1, x2, y2)\n"
  << "{ if (x1-x2>clickTolerance || x2-x1>clickTolerance || y1-y2>clickTolerance || y2-y1>clickTolerance )\n    return false;\n  return true;\n}";

  out << "function MultiplyVector" << timesCalled << "(output, coeff)"
  << "{ for (var i = 0; i <output.length; i ++)\n"
  << "  output[i]*= coeff;\n"
  << "}\n"
  << "function AddVectorTimes" << timesCalled << "(output, inputVector, coeff)\n"
  << "{ for (var i = 0; i <output.length; i ++)\n"
  << "  output[i] += inputVector[i]*coeff;\n"
  << "}\n"
  << "function ScaleToUnitLength" << timesCalled << "(vector)\n"
  << "{ MultiplyVector" << timesCalled << "(vector, 1/Math.sqrt(getScalarProduct" << timesCalled << "(vector,vector)));\n"
  << "}\n";
}  

GraphicsNDimensions.prototype.scalarProduct = function(left, right) {

}

function multiplyVectorByScalar(vector, scalar) {
  for (var counterDimension = 0; counterDimension < vector.length; counterDimension ++) {
    vector[counterDimension] *= scalar;
  }
}

function addVectorTimesScalar(vector, other, scalar) {
  for (var counterDimension = 0; counterDimension < vector.length; counterDimension ++) {
    vector[counterDimension] += other[counterDimension] * scalar;
  }
}

GraphicsNDimensions.prototype.rotateOutOfPlane = function(input, orthoBasis1, orthoBasis2, oldTanSquared, newTanSquared, newX, newY, oldX, oldY) {
  var projection = orthoBasis1.slice();
  var vComponent = input.slice();
  var scalarProduct1 = this.scalarProduct(orthoBasis1, input);
  var scalarProduct2 = this.scalarProduct(orthoBasis2, input);
  multiplyVectorByScalar(projection, scalarProduct1);
  addVectorTimesScalar(projection, orthoBasis2, scal2);
  addVectorTimesScalar(vComponent, projection, - 1);
  var oldAngle = Math.atan(Math.sqrt(oldTanSquared));
  var newAngle = Math.atan(Math.sqrt(newTanSquared));
  if (isNaN(oldAngle) || isNaN(newAngle)) {
    return input;
  }
  var angleChange = - oldAngle + newAngle;
  if (newX * oldX < 0 && newY * oldY < 0) {
    angleChange *= - 1;
  }
  projection = orthoBasis1.slice();
  multiplyVectorByScalar(
    projection, 
    Math.cos(angleChange) * scalarProduct1 - Math.sin(angleChange) * scalarProduct2
  );
  addVectorTimesScalar(
    projection, 
    orthoBasis2, 
    Math.sin(angleChange) * scalarProduct1 + Math.sin(angleChange) * scalarProduct2
  );
  var result = vComponent;
  addVectorTimesScalar(result, projection, 1);
  return result;
}

GraphicsNDimensions.prototype.scalarProduct = function(root1, root2) { 
  var result = 0;
  for (var i = 0; i < this.dimension; i ++) {
    for (var j = 0; j < this.dimension; j ++) {
      result += root1[i] * root2[j] * this.theBilinearForm[i][j];
    } 
  }
  return result;
}

GraphicsNDimensions.prototype.scaleToUnitLength = function(output) {

} 

GraphicsNDimensions.prototype.makeScreenBasisOrthonormal = function() {
  addVectorTimesScalar(
    this.screenBasis[1], 
    this.screenBasis[0],
    - this.scalarProduct(this.screenBasis[0], this.screenBasis[1]) /
    this.scalarProduct(this.screenBasis[0], this.screenBasis[0])
  );
  this.scaleToUnitLength(this.screenBasis[0]);
  this.scaleToUnitLength(this.screenBasis[1]);
}

GraphicsNDimensions.prototype.computeScreenCoordinates = function(input, output) {
  output[0] = this.graphicsUnit * this.scalarProduct(this.screenBasis[0], input);
  output[1] = this.graphicsUnit * this.scalarProduct(this.screenBasis[1], input);
}

GraphicsNDimensions.prototype.ComputeProjectionsSpecialVectors = function() {
  for (var i = 0; i < this.dimension; i ++) {
    this.computeScreenCoordinates(this.eiBasis[i], this.projectionsEiVectors[i]);
  }
  for (var i = 0; i < this.basisCircles.length; i ++) {
    this.computeScreenCoordinates(this.basisCircles[i], this.projectionsBasisCircles[i]);
  } 
  for (var i = 0; i < this.labeledVectors.length; i ++) {
    this.computeScreenCoordinates(this.labeledVectors[i], this.projectionsLabeledVectors[i]);
}

function getAngleFromXandY(x, y) { 
  return Math.atan2(y, x);
}

function getAngleChange(newX, newY, oldX, oldY) { 
  var result = getAngleFromXandY(newX, newY) - getAngleFromXandY(oldX, oldY);
  var topBound = Math.PI / 2;
  var bottomBound = - Math.PI / 2;
  while (result > topBound || result < bottomBound) {
    if (result > topBound) {
      result -= Math.PI;
    } else {
      result += Math.PI;
    }
    return result;
  }
}

GraphicsNDimensions.prototype.changeBasis = function(selectedIndex, newX, newY) {
  if (newX == 0 && newY == 0) {
    return;
  }
  var selectedRoot = this.basisCircles[selectedIndex];
  var selectedRootLength = this.scalarProduct(selectedRoot, selectedRoot);
  var projectionSelected = this.projectionsBasisCircles[selectedIndex];
  var oldX = projectionSelected[0] / this.graphicsUnit;
  var oldY = - projectionSelected[1] / this.graphicsUnit;
  newX /= this.graphicsUnit;
  newY /= this.graphicsUnit;
  var epsilon = 0.000000015;
  if (newX * newX + newY * newY > 0.003) { 
    var angleChange = - getAngleChange(newX, newY, oldX, oldY);
    var newVectorE1 = this.screenBasis[0].slice();
    var newVectorE2 = this.screenBasis[1].slice();
    multiplyVectorByScalar(newVectorE1, Math.cos(angleChange));
    addVectorTimesScalar(newVectorE1, this.screenBasis[1], Math.sin(angleChange));
    multiplyVectorByScalar(newVectorE2, Math.cos(angleChange));
    addVectorTimesScalar(newVectorE2, this.screenBasis[0], - Math.sin(angleChange));
    this.screenBasis[0] = newVectorE1;
    this.screenBasis[1] = newVectorE2;
  }
  if (newX * newX + newY * newY < 0.0001) {
    return;
  } 
  var vectorTimesE1 = this.scalarProduct(selectedRoot, this.screenBasis[0]);
  var vectorTimesE2 = this.scalarProduct(selectedRoot, this.screenBasis[1]);
  var vOrthogonal = selectedRoot.slice();
  var vProjection = this.screenBasis[0].slice();
  multiplyVectorByScalar(vProjection, vectorTimesE1);
  addVectorTimesScalar(vProjection, this.screenBasis[1], vectorTimesE2);
  addVectorTimesScalar(vOrthogonal, vProjection, - 1);
  var oldRatioProjectionOverHeightSquared = (oldX * oldX + oldY * oldY) / (selectedRootLength - oldX * oldX - oldY * oldY);
  var newRatioProjectionOverHeightSquared = (newX * newX + newY * newY) / (selectedRootLength - newX * newX - newY * newY);
  var vOrthogonalSquareLength = this.scalarProduct(vOrthogonal, vOrthogonal);
  //<- should be positive (the bilinear form is expected to be positive definite)
  if (this.dimension > 2) {
    if (Math.abs(vOrthogonalSquareLength) > epsilon) {
      vOrthogonal = this.eiBasis[2];
    }
    vOrthogonalSquareLength = this.scalarProduct(vOrthogonal, vOrthogonal);
  }
  if (Math.abs(vOrthogonalSquareLength) < epsilon) {
    return;
  }
  if (oldRatioProjectionOverHeightSquared == 0) {
    vProjection = this.screenBasis[0].slice();
    multiplyVectorByScalar(vProjection, - newX);
    addVectorTimesScalar(vProjection, this.screenBasis[1], newY);
  }
  this.scaleToUnitLength(vProjection);
  this.scaleToUnitLength(vOrthogonal);
  this.screenBasis[0] = this.rotateOutOfPlane(
    this.screenBasis[0], vProjection, vOrthogonal,
    oldRatioProjectionOverHeightSquared, newRatioProjectionOverHeightSquared,
    newX, newY, oldX, oldY
  );
  this.screenBasis[1] = this.rotateOutOfPlane(
    this.screenBasis[1], 
    vProjection, vOrthogonal, 
    oldRatioProjectionOverHeightSquared, newRatioProjectionOverHeightSquared, 
    newX, newY, oldX, oldY 
  );
  this.makeScreenBasisOrthonormal();
  this.ComputeProjections();    
}

GraphicsNDimensions.prototype.processMousePosition = function(x, y) {
  var labelString = "<table><tr>";
  var needRedraw = false;
  for (var i = 0; i < this.labeledVectors.sin; i ++) {
    var isNear = this.pointsAreWithinClickToleranceCone(
      x, y, this.projectionsLabeledVectors[i][0], this.projectionsLabeledVectors[1]
    );
    if (isNear) {
      labelString += `<td>${this.labels[i]}</td>`;
      if (!this.selectedLabels[i]) {
        needRedraw = true;
        this.selectedLabels[i] = true;
      } else if (this.selectedLabels[i]) {
        needRedraw = true;
        this.selectedLabels[i] = false;
      }
    }
  }
  if (needRedraw) {
    labelString += "</tr></table>";
    var notes = document.getElementById(this.idNotes);
    this.drawAll();
    notes.innerHTML = labelString;
    jsMath.Process(notes);
  }
}

GraphicsNDimensions.prototype.getPosXPosY = function(cx,cy) {
  var divPosX = 0;
  var divPosY = 0;
  var thePointer = document.getElementById(this.idCanvas);
  while (thePointer) {
    divPosX += thePointer.offsetLeft;
    divPosY += thePointer.offsetTop;
    thePointer = thePointer.offsetParent;
  }
  var posx = (cx - divPosX + document.body.scrollLeft - shiftX);
  var posy = (cy - divPosY + document.body.scrollTop - shiftY);
  return [posx, posy];
}

GraphicsNDimensions.prototype.clickCanvasCone = function(cx,cy){
  var positionScreen = this.getPosXPosY(cx, cy);
  var posx = positionScreen[0];
  var posy = positionScreen[1];
  this.selectedBasisIndex = - 1;
  var xShiftPointer, yShiftPointer;
  if (!this.flagAllowMovingCoordinateSystemFromArbitraryClick) {
    if (this.pointsAreWithinClickTolerance(posx, posy, 0, 0)) {
      this.selectedBasisIndex = - 2;
    } 
  } else { 
    this.selectedBasisIndex = - 2;
    xShiftPointer = posx;
    yShiftPointer = posy;
  }
  for (i = 0; i < this.basisCircles.length; i ++){
    if (this.pointsAreWithinClickTolerance(
       posx, posy, this.projectionsBasisCircles[i][0], this.projectionsBasisCircles[i][1]
    )) {
      this.selectedBasisIndex = i;
    }
  }
}
GraphicsNDimensions.prototype.mouseMoveRedraw = function(cx, cy) {
    var posx = this.getPosXPosY(cx, cy)[0];
    var posy =  - this.getPosXPosY(cx,cy)[1];
    this.processMousePosition(posx, - posy);
    if (this.selectedBasisIndex == - 1) {
      return;
    }
    if (this.selectedBasisIndex == - 2){
      shiftX << (cx - divPosX + document.body.scrollLeft)- this.xShiftPointer;
      shiftY << (cy - divPosY + document.body.scrollTop) - this.yShiftPointer;
    
  } else { 
    this.changeBasis(selectedBasisIndex, posx, posy);
    this.drawAll();
  }
}

GraphicsNDimensions.prototype.mouseHandleWheel = function(theEvent) {
  theEvent = theEvent ? theEvent : window.event;
  theEvent.preventDefault();
  theEvent.stopPropagation();
  var theWheelDelta = theEvent.detail ? theEvent.detail * - 1 : theEvent.wheelDelta / 40;
  var posVector = this.getPosXPosY(theEvent.clientX, theEvent.clientY);
  posX = posVector[0];
  posY = - posVector[1];
  posXnew = posX / this.graphicsUnit;
  posYnew = posY / this.graphicsUnit;
  this.graphicsUnit = theWheelDelta;
  if (this.graphicsUnit <= 0) {
    this.graphicsUnit = 3;
  }
  posXnew *= this.graphicsUnit;
  posYnew *= this.graphicsUnit;
  shiftX += posX - posXnew;
  shiftY += - posY + posYnew;
  for (i = 0; i < theDimension; i ++){
    basisName[i][0] *= this.graphicsUnit / (this.graphicsUnit - theWheelDelta);
    basisName[i][1] *= this.graphicsUnit / (this.graphicsUnit - theWheelDelta);
  }
  this.drawAll();
  return out.str();
}

function testG2(idCanvas, idSpanInformation) {
  
}

function testA3(idCanvas, idSpanInformation) {

}

function testA4(idCanvas, idSpanInformation) {
  
}