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

var collectionGraphicsNDimension = {};

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
  this.canvasContainer = document.getElementById(this.idCanvas);
  this.dimension = 0;
  this.theBilinearForm = [];
  this.graphicsUnit = 0;
  this.selectedBasisIndex = - 1;
  this.flagAllowMovingCoordinateSystemFromArbitraryClick = true;

  this.canvasContainer.addEventListener("DOMMouseScroll", this.mouseHandleWheel.bind(this));
  this.canvasContainer.addEventListener("wheel", this.mouseHandleWheel.bind(this));
  this.canvasContainer.addEventListener("mousedown", this.clickCanvas.bind(this));
  this.canvasContainer.addEventListener("mouseup", this.releaseMouse.bind(this));
  this.canvasContainer.addEventListener("mousemove", this.mouseMoveRedraw.bind(this));
  this.frameCount = 0;
  this.numberOfFrames = 20;
  this.screenGoal = [];
  this.screenStart = [];
  this.canvas = null;
  this.widthHtml = this.canvasContainer.width;
  this.heightHtml = this.canvasContainer.height;
  this.shiftScreenCoordinates = [this.widthHtml / 2, this.heightHtml / 2];
  this.mousePositionScreen = [0, 0];
  this.mousePositionScreenClicked = [0, 0];
  this.shiftScreenCoordinatesClicked = [0, 0];
  
  collectionGraphicsNDimension[this.idCanvas] = this;
  this.clickTolerance = 5;
  this.frameStarted = false;
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
  result += `<br>Mouse position: (${this.mousePositionScreen.join(", ")})`;
  result += `<br>Selected index: ${this.selectedBasisIndex}`;
  result += `<br>Coordinate center in screen coordinates: (${this.shiftScreenCoordinates.join(", ")})`;
  result += "<br>The projection plane (drawn on the screen) is spanned by the following two vectors <br>\n";
  result += `(${this.screenBasis[0].join(", ")})<br>`;
  result += `(${this.screenBasis[1].join(", ")})<br>`;
  for (var i = 0; i < this.basisCircles.length; i ++) {
    result += `${i}: (${this.basisCircles[i].join(", ")}): (${this.projectionsBasisCircles[i].join(", ")})<br>`;
  }
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
  result += `<span id = '${this.idPlaneInfo}'></span><br>`;
  document.getElementById(this.idInfo).innerHTML = result;
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

GraphicsNDimensions.prototype.drawStandardEiBasis = function(color) {
  for (var i = 0; i < this.dimension; i ++) {
    var eiVector = new Array(this.dimension);
    eiVector.fill(0);
    eiVector[i] = 1;
    this.drawCircle(eiVector, color, 3);
  }
}

GraphicsNDimensions.prototype.drawCircle = function(position, color, radius) {
  if (radius === undefined) {
    radius = 3;
  }
  if (color === undefined) {
    color = "black";
  }
  this.drawOperations.push(new DrawCircleAtVector(
    this.idCanvas, {
      location: position,
      colorFill: color,
      radius: radius
  }));
}

GraphicsNDimensions.prototype.drawLine = function(left, right, color) {
  if (color === undefined) {
    color = "black";
  }
  this.drawOperations.push(new DrawSegmentBetweenTwoVectors(
    this.idCanvas, {
      left: left,
      right: right,
      lineWidth: 1,
      lineColor: color    
    }
  ));
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

GraphicsNDimensions.prototype.changeProjectionPlaneUser = function() {
  this.frameCount ++;
  if (this.frameCount >= this.numberOfFrames) {
    return;
  }
  for (var i = 0; i < this.dimension; i ++) {
    this.screenBasis[0][i] = this.screenGoal[0][i] * (this.frameCount / this.numFramesUserPlane) + this.screenStart[0][i] * (1 - this.frameCount / this.numFramesUserPlane);
    this.screenBasis[1][i] = this.screenGoal[1][i] * (this.frameCount / this.numFramesUserPlane) + this.screenStart[1][i] * (1 - this.frameCount / this.numFramesUserPlane);
  }
  this.makeScreenBasisOrthonormal();
  this.drawAll();
  setTimeout(this.changeProjectionPlaneUser.bind(this), 100);
}

function DrawSegmentBetweenTwoVectors(inputOwnerId, inputData) {
  this.ownerId = inputOwnerId;
  this.left = inputData.left.slice();
  this.right = inputData.right.slice();
  this.lineWidth = inputData.lineWidth;
  this.lineColor = inputData.lineColor;
}

DrawSegmentBetweenTwoVectors.prototype.drawNoFinish = function() {
  var owner = collectionGraphicsNDimension[this.ownerId];
  var canvas = owner.canvas;
  canvas.beginPath();
  canvas.strokeStyle = this.colorLine;
  canvas.lineWidth = this.lineWidth;
  var screenCoordinatesLeft = [];
  var screenCoordinatesRight = [];
  owner.computeScreenCoordinates(this.left, screenCoordinatesLeft);
  owner.computeScreenCoordinates(this.right,screenCoordinatesRight);
  canvas.moveTo(screenCoordinatesLeft[0], screenCoordinatesLeft[1]);
  canvas.lineTo(screenCoordinatesRight[0], screenCoordinatesRight[1]);
  canvas.stroke();
}

function DrawFilledShape(inputOwnerId, inputData) {
  this.points = inputData.points;
  this.ownerId = inputOwnerId;
  this.colorFill = inputData.colorFill;
  this.lineWidth = inputData.lineWidth;
}

DrawFilledShape.prototype.drawNoFinish = function () {
  var owner = collectionGraphicsNDimension[this.ownerId];
  var canvas = owner.canvas;
  canvas.beginPath();
  canvas.fillStyle = this.colorFill;
  canvas.lineWidth = this.lineWidth;
  var currentPoint = [];
  owner.computeScreenCoordinates(this.points[0], currentPoint);
  canvas.moveTo(currentPoint[0], currentPoint[1]);
  for (var counterPoint = 1; counterPoint < this.points.length; counterPoint ++) {
    owner.computeAllScreenCoordinates(this.points[counterPoint], currentPoint);
    canvas.lineTo(currentPoint[0], currentPoint[1]);
  }
  canvas.closePath();
  canvas.fill();
  //canvas.stroke();
}

function DrawCircleAtVector(inputOwnerId, inputData) {
  this.ownerId = inputOwnerId;
  this.location = inputData.location.slice();
  this.locationScreenCoordinates = [];
  this.colorFill = inputData.colorFill;
  this.radius = inputData.radius;
}

DrawCircleAtVector.prototype.drawNoFinish = function() {
  var owner = collectionGraphicsNDimension[this.ownerId];
  var canvas = owner.canvas;
  canvas.strokeStyle = this.colorFill;
  canvas.fillStyle = this.colorFill;
  owner.computeScreenCoordinates(this.location, this.locationScreenCoordinates);
  canvas.beginPath();
  canvas.arc( 
    this.locationScreenCoordinates[0], 
    this.locationScreenCoordinates[1],
    this.radius,
    0, 2 * Math.PI
  );
  canvas.fill();
}

function DrawTextAtVector(inputOwnerId, inputData) {
  this.ownerId = inputOwnerId;
  this.text = inputData.text;
  this.location = inputData.location;
  this.locationScreen = [];
  this.colorFill = inputData.colorFill;
}

DrawTextAtVector.prototype.drawNoFinish = function() {
  var owner = collectionGraphicsNDimension[this.ownerId];
  var canvas = owner.canvas;
  canvas.strokeStyle = this.colorFill;
  this.locationScreen = owner.computeScreenCoordinates(this.location, this.locationScreen);
  canvas.strokeText(this.text, this.locationScreen[0], this.locationScreen[1]);
}


GraphicsNDimensions.prototype.drawAll = function() {
  if (this.frameStarted) {
    return;
  }
  this.frameStarted = true;
  if (this.canvas == null || this.canvas == undefined) {
    this.init();
  }
  this.computeProjectionsEiBasis();
  this.ComputeProjectionsSpecialVectors();
  this.canvas.clearRect(0, 0, this.widthHtml, this.heightHtml);
  //this.canvas.fillStyle = "#FFFFFF";
  //this.canvas.fillRect(0, 0, this.widthHtml, this.heightHtml);
  //this.canvas.stroke();
  this.writeInfo();
  for (var counterOperation = 0; counterOperation < this.drawOperations.length; counterOperation ++) { 
    this.drawOperations[counterOperation].drawNoFinish();
  }
  this.frameStarted = false;
  //this.canvas.stroke();
}

GraphicsNDimensions.prototype.makeStandardBilinearForm = function () {
  this.theBilinearForm = new Array(this.dimension);
  for (var i = 0; i < this.theBilinearForm.length; i ++) {
    this.theBilinearForm[i] = newArray(this.dimension);
    this.theBilinearForm[i].fill(0, 0, this.dimension);
    this.theBilinearForm[i][i] = 1;
  }
}

GraphicsNDimensions.prototype.makeEiBasis = function () {
  this.eiBasis = new Array(this.dimension);
  for (var i = 0; i < this.eiBasis.length; i ++) {
    this.eiBasis[i] = new Array(this.dimension);
    this.eiBasis[i].fill(0, 0, this.dimension);
    this.eiBasis[i][i] = 1;
  }
}

GraphicsNDimensions.prototype.makeBasisCircles = function () {
  this.basisCircles = new Array(this.dimension);
  this.projectionsBasisCircles = new Array(this.dimension);
  for (var i = 0; i < this.basisCircles.length; i ++) {
    this.basisCircles[i] = new Array(this.dimension);
    this.basisCircles[i].fill(0, 0, this.dimension);
    this.basisCircles[i][i] = 1;
    this.projectionsBasisCircles[i] = new Array(2);
    this.projectionsBasisCircles[i].fill(0, 0, 2);
  }
}

GraphicsNDimensions.prototype.init = function () {
  this.initInfo();
  if (this.theBilinearForm.length != this.dimension) {
    this.makeStandardBilinearForm(this.dimension);
  }
  if (this.basisCircles.length === 0) {
    this.makeBasisCircles();
  }
  this.makeEiBasis();
  this.modifyBasisToOrthonormalNoShiftSecond();
  var theDiv = document.getElementById(this.idCanvas);
  this.canvas = theDiv.getContext("2d");
}

GraphicsNDimensions.prototype.pointsAreWithinClickTolerance = function(x1, y1, x2, y2) {
  if (
    Math.abs(x1 - x2) > this.clickTolerance ||
    Math.abs(y1 - y2) > this.clickTolerance
  ) {
    return false;
  } 
  return true;
}

GraphicsNDimensions.prototype.scaleToUnitLength = function(output) {
  var scale =  1 / Math.sqrt(this.scalarProduct(output, output));
  multiplyVectorByScalar(output, scale)
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

GraphicsNDimensions.prototype.rotateOutOfPlane = function(
  input, orthoBasis1, orthoBasis2, oldTanSquared, newTanSquared, newX, newY, oldX, oldY
) {
  var projection = orthoBasis1.slice();
  var vComponent = input.slice();
  var scalarProduct1 = this.scalarProduct(orthoBasis1, input);
  var scalarProduct2 = this.scalarProduct(orthoBasis2, input);
  multiplyVectorByScalar(projection, scalarProduct1);
  addVectorTimesScalar(projection, orthoBasis2, scalarProduct2);
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
  output[0] = this.shiftScreenCoordinates[0] + this.graphicsUnit * this.scalarProduct(this.screenBasis[0], input);
  output[1] = this.shiftScreenCoordinates[1] + this.graphicsUnit * this.scalarProduct(this.screenBasis[1], input);
}

GraphicsNDimensions.prototype.computeProjectionsEiBasis = function () {
  if (this.projectionsEiVectors.length != this.dimension) {
    this.projectionsEiVectors[i] = new Array(this.dimension);
    for (var i = 0; i < this.dimension; i ++) {
      this.projectionsEiVectors[i] = new Array(this.dimension);
    }
  }
  for (var i = 0; i < this.eiBasis.length; i ++) {
    this.computeScreenCoordinates(this.eiBasis[i], this.projectionsEiVectors[i]);
  }
}

GraphicsNDimensions.prototype.ComputeProjectionsSpecialVectors = function() {
  for (var i = 0; i < this.basisCircles.length; i ++) {
    this.computeScreenCoordinates(this.basisCircles[i], this.projectionsBasisCircles[i]);
  } 
  for (var i = 0; i < this.labeledVectors.length; i ++) {
    this.computeScreenCoordinates(this.labeledVectors[i], this.projectionsLabeledVectors[i]);
  }
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
  }
  return result;
}

GraphicsNDimensions.prototype.changeBasis = function(selectedIndex) {
  var newX = this.mousePositionScreen[0];
  var newY = this.mousePositionScreen[1];
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
  return;
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

GraphicsNDimensions.prototype.computePosXPosY = function(event) {
  this.mousePositionScreen[0] = event.clientX;
  this.mousePositionScreen[1] = event.clientY;
  var thePointer = this.canvasContainer;
  while (thePointer) {
    this.mousePositionScreen[0] -= thePointer.offsetLeft; 
    if (typeof thePointer.scrollLeft === "number") {
      this.mousePositionScreen[0] += thePointer.scrollLeft;
    }
    this.mousePositionScreen[1] -= thePointer.offsetTop;
    if (typeof thePointer.scrollTop === "number") {
      this.mousePositionScreen[1] += thePointer.scrollTop;
    }
    thePointer = thePointer.offsetParent;
  }
}

GraphicsNDimensions.prototype.releaseMouse = function(event) {
  this.selectedBasisIndex = - 1;
}

GraphicsNDimensions.prototype.clickCanvas = function(event) {
  this.computePosXPosY(event);
  this.mousePositionScreenClicked[0] = this.mousePositionScreen[0];
  this.mousePositionScreenClicked[1] = this.mousePositionScreen[1];
  this.selectedBasisIndex = - 1;
  if (!this.flagAllowMovingCoordinateSystemFromArbitraryClick) {
    if (this.pointsAreWithinClickTolerance(posx, posy, 0, 0)) {
      this.selectedBasisIndex = - 2;
    } 
  } else { 
    this.selectedBasisIndex = - 2;
  }
  if (this.selectedBasisIndex === - 2) {
    this.shiftScreenCoordinatesClicked[0] = this.shiftScreenCoordinates[0];
    this.shiftScreenCoordinatesClicked[1] = this.shiftScreenCoordinates[1];
  }
  for (var i = 0; i < this.basisCircles.length; i ++) {
    if (this.pointsAreWithinClickTolerance(
      this.mousePositionScreenClicked[0], this.mousePositionScreenClicked[1], 
      this.projectionsBasisCircles[i][0], this.projectionsBasisCircles[i][1]
    )) {
      this.selectedBasisIndex = i;
    }
  }
}

GraphicsNDimensions.prototype.mouseMoveRedraw = function(event) {
  this.computePosXPosY(event);
  this.writeInfo();
  //this.processMousePosition(posx, - posy);
  if (this.selectedBasisIndex == - 1) {
    return;
  }
  var doRedraw = false;
  if (this.selectedBasisIndex == - 2){
    if (
      this.shiftScreenCoordinates[0] !== this.mousePositionScreen[0] ||
      this.shiftScreenCoordinates[1] !== this.mousePositionScreen[1]
    ) {
      doRedraw = true;
    }
    var deltaX = this.mousePositionScreen[0] - this.mousePositionScreenClicked[0];
    var deltaY = this.mousePositionScreen[1] - this.mousePositionScreenClicked[1];
    this.shiftScreenCoordinates[0] = this.shiftScreenCoordinatesClicked[0] + deltaX;
    this.shiftScreenCoordinates[1] = this.shiftScreenCoordinatesClicked[1] + deltaY;
  } else { 
    this.changeBasis(this.selectedBasisIndex);
    var doRedraw = true;
  }
  if (doRedraw) {
    this.drawAll();
  }
}

GraphicsNDimensions.prototype.mouseHandleWheel = function(event) {
  event = event ? event : window.event;
  event.preventDefault();
  event.stopPropagation();
  var theWheelDelta = event.detail ? event.detail * - 1 : event.wheelDelta / 40;
  this.computePosXPosY(event);

  var oldXDelta = (this.mousePositionScreen[0] - this.shiftScreenCoordinates[0]);
  var oldYDelta = (this.mousePositionScreen[1] - this.shiftScreenCoordinates[1]);
  var oldXDeltaScaled = oldXDelta / this.graphicsUnit;
  var oldYDeltaScaled = oldYDelta / this.graphicsUnit;
  this.graphicsUnit += theWheelDelta;
  var newXDelta = oldXDeltaScaled * this.graphicsUnit;
  var newYDelta = oldYDeltaScaled * this.graphicsUnit;
  this.shiftScreenCoordinates[0] += oldXDelta - newXDelta;
  this.shiftScreenCoordinates[1] += oldYDelta - newYDelta;
  this.drawAll();
}

function testA3(idCanvas, idSpanInformation) {
  var theA3 = new GraphicsNDimensions(idCanvas, idSpanInformation);
  theA3.dimension = 3;
  theA3.theBilinearForm = [
    [ 2, -1,  0],
    [-1,  2, -1],
    [ 0, -1,  2]
  ];
  theA3.screenBasis = [
    [0.707107, 0.707107, 0],
    [0, 0.707107, 0.707107]
  ];
  var labeledVectors = [
    [-1,-1,-1],
    [ 0,-1,-1],
    [-1,-1, 0],
    [ 0, 0,-1],
    [ 0,-1, 0],
    [-1, 0, 0],
    [ 1, 0, 0],
    [ 0, 1, 0],
    [ 0, 0, 1],
    [ 1, 1, 0],
    [ 0, 1, 1],
    [ 1, 1, 1],
  ];
  var segments =    [
    [[0 , - 1, - 1], [ 1,  0,  0]],
    [[0 , - 1, - 1], [-1, -1, -1]],
    [[0 , - 1, - 1], [ 0,  0, -1]],
    [[0 , - 1, - 1], [ 0, -1,  0]],
    [[-1,  -1,   0], [-1,  0,  0]],
    [[-1,  -1,   0], [ 0,  0,  1]],
    [[-1,  -1,   0], [-1, -1, -1]],
    [[-1,  -1,   0], [ 0, -1,  0]],
    [[1 ,   1,   0], [ 1,  0,  0]],
    [[1 ,   1,   0], [ 0,  0, -1]],
    [[1 ,   1,   0], [ 1,  1,  1]],
    [[1 ,   1,   0], [ 0,  1,  0]],
    [[0 ,   1,   1], [-1,  0,  0]],
    [[0 ,   1,   1], [ 0,  0,  1]],
    [[0 ,   1,   1], [ 1,  1,  1]],
    [[0 ,   1,   1], [ 0,  1,  0]],
    [[-1,   0,   0], [-1, -1, -1]],
    [[-1,   0,   0], [ 0,  1,  0]],
    [[1 ,   0,   0], [ 1,  1,  1]],
    [[1 ,   0,   0], [ 0, -1,  0]],
    [[0 ,   0,   1], [ 1,  1,  1]],
    [[0 ,   0,   1], [ 0, -1,  0]],
    [[-1,  -1,  -1], [ 0,  0, -1]],
    [[0 ,   0,  -1], [ 0,  1,  0]]
  ];
  theA3.drawStandardEiBasis("red");
  for (var counterLabel = 0; counterLabel < labeledVectors.length; counterLabel ++) {
    theA3.drawLine([0, 0, 0], labeledVectors[counterLabel], "green");
    //theA3.drawText(labeledVectors[counterLabel], `[${labeledVectors.join(', ')}]`);
    //theA3.drawCircle(labeledVectors[counterLabel], "red");
  }
  for (var counterSegment = 0; counterSegment < segments.length; counterSegment ++) {
    theA3.drawLine(segments[counterSegment][0], segments[counterSegment][1]);
  }
  theA3.graphicsUnit = 150;
  theA3.drawAll();
}