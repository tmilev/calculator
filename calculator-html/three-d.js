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

var module;
var require;
if (require === undefined) {
  require = function(){};
}

if (module === undefined) {
  module = {};
}

function Drawing() {
  this.canvases = {};
  this.firstCriticalRunTimeError = "";
  this.firstCanvas = null;
  this.numberOfControlsConstructed = 0;
  this.plotUpdaters = {};
  this.Surface = Surface;
}

function calculatorError(x) { 
  console.log(x);
  if (firstCriticalRunTimeError !== "") {
    return;  
  }
  firstCriticalRunTimeError = x;
  if (firstCriticalRunTimeError !== "" && firstCanvas !== undefined && firstCanvas !== null) { 
    firstCanvas.textErrors = firstCriticalRunTimeError + " All further error messages are suppressed.";
    firstCanvas.showMessages();
  }
}

function vectorScalarVector(s, t) { 
  var result = 0;
  if (s.length !== t.length) {
    calculatorError("Scalar product of vectors of different length: " + s + " and " + t + ".");
  }
  for (var i = 0; i < s.length; i ++) {
    result += s[i] * t[i];
  }
  return result;
}

function vectorTimesScalar(vector, s) { 
  for (var i = 0; i < vector.length; i ++) {
    vector[i] *= s;
  }
}

function vectorPlusVector(left, right) { 
  var output = new Array(left.length);
  for (var i = 0; i < left.length; i ++) {
    output[i] = left[i] + right[i];
  }
  return output;
}

function vectorCrossVector(left, right) { 
  return [
    left[1] * right[2] - left[2] * right[1], 
    -left[0] * right[2] + left[2] * right[0], 
    left[0] * right[1] - left[1] * right[0] 
  ];
}

function vectorMinusVector(left, right) { 
  var output = new Array(left.length);
  for (var i = 0; i < left.length; i ++) {
    output[i] = left[i] - right[i];
  }
  return output;
}

function vectorAddVectorTimesScalar(output, otherVector, scalar) { 
  for (var i = 0; i < output.length; i ++) {
    output[i] += otherVector[i] * scalar;
  }
}

function vectorLength(vector) { 
  return Math.sqrt(vectorScalarVector(vector, vector));
}

function vectorRound(vector) { 
  for (var i = 0; i < vector.length; i ++) {
    vector[i] = Math.round(vector[i]);
  }
}

function vectorNormalize(vector) { 
  vectorTimesScalar(vector, 1 / vectorLength(vector));
}

function getPosXPosYObject(theObject, cx, cy) { 
  var rectangle = theObject.getBoundingClientRect();
  return [cx - rectangle.left, cy - rectangle.top];
}

function getAngleChangeMathScreen(newX, newY, oldX, oldY) { 
  var result = Math.atan2(newY, newX) - Math.atan2(oldY, oldX);
  if (result > Math.PI) {
    result -= Math.PI;
  } else if (result < - Math.PI) {
    result += Math.PI;
  }
  return result;
}

function testFunctionPlot(v) { 
  return Math.sin(v);
}

function testFunctionPlot2(v) { 
  return Math.sin(2 * v) + 2;
}

function testMoebiusStripEmbedding(u, v) { 
  var z = v * Math.sin(u / 2);
  var x = (2 + (v) * Math.cos(u / 2)) * Math.cos(u);
  var y = (2 + (v) * Math.cos(u / 2)) * Math.sin(u);
  //var z=v/2;//*Math.sin(u);
  //var x = (u)/10+v/2;// (2+(v)*Math.cos(u))*Math.cos(u);
  //var y = (u/11);// (2+(v)*Math.cos(u))*Math.sin(u);
  return [x, y, z];
}

function testMoebiusStripEmbedding2(u, v) { 
  var first = testMoebiusStripEmbedding(u, v);
  var x = first[0];
  var y = first[1];
  var z = first[2];
  return [x + 2, z, y];
}

function testGetMoebiusSurface() { 
  var colors = {
    colorContour: "black", 
    colorUV: "blue", 
    colorVU: "cyan"
  };
  var result = new Surface(testMoebiusStripEmbedding, [[0, - 0.6], [Math.PI * 2, 0.6]], [22, 4], colors, 2);
  return result;
}

function testGetMoebiusSurface2() { 
  var colors = {
    colorContour: "black", 
    colorUV: "red", 
    colorVU: "pink"
  };
  var result = new Surface(testMoebiusStripEmbedding2, [[0, - 0.6], [Math.PI * 2, 0.6]], [22, 4], colors, 0.5);
  return result;
}

function testVectorField2d(x, y) { 
  return [- y, x];
}

function testGetTestPlane() { 
  var colors = {
    colorContour: "black", 
    colorUV: "blue", 
    colorVU: "cyan"
  };
  var result = new Surface(function(u, v) {
    return [u, 0.9 * v, 1 + u + v]; 
  }, [[- 1.2, - 0.7], [1, 1]], [5, 5], colors);
  return result;
}

function CurveThreeD(
  inputCoordinateFunctions, inputLeftPt, inputRightPt,
  inputNumSegments, inputColor, inputLineWidth
) { 
  this.coordinateFunctions = inputCoordinateFunctions;
  this.leftPt = inputLeftPt;
  this.rightPt = inputRightPt;
  this.color = colorToRGB(inputColor);
  this.numSegments = inputNumSegments;
  this.lineWidth = inputLineWidth;
}

CurveThreeD.prototype.accountBoundingBox = function(inputOutputBox) { 
  var theT = this.leftPt;
  var theX = this.coordinateFunctions[0](theT);
  var theY = this.coordinateFunctions[1](theT);
  accountBoundingBox([theX, theY], inputOutputBox);
  for (var i = 0; i < this.numSegments; i ++) { 
    var theRatio = i / (this.numSegments - 1);
    theT = this.leftPt * (1 - theRatio) +  this.rightPt * theRatio;
    theX = this.coordinateFunctions[0](theT);
    theY = this.coordinateFunctions[1](theT);
    accountBoundingBox([theX, theY], inputOutputBox);
  }
}

CurveThreeD.prototype.drawNoFinish = function(theCanvas, startByMoving) { 
  var theSurface = theCanvas.surface;
  theSurface.strokeStyle = colorRGBToString(this.color);
  theSurface.fillStyle = colorRGBToString(this.color);
  var theT = this.leftPt;
  var theX = this.coordinateFunctions[0](theT);
  var theY = this.coordinateFunctions[1](theT);
  var theCoords = theCanvas.coordsMathToScreen([theX, theY]);
  if (startByMoving) {
    theSurface.moveTo(theCoords[0], theCoords[1]);
  } else {
    theSurface.lineTo(theCoords[0], theCoords[1]);
  }
  var skippedValues = false;
  for (var i = 0; i < this.numSegments; i ++)
  { var theRatio = i / (this.numSegments - 1);
    theT = this.leftPt * (1 - theRatio) +  this.rightPt * theRatio; //<- this way of
    //computing x introduces smaller numerical errors.
    //For example, suppose you plot sqrt(1-x^2) from - 1 to 1.
    //If not careful with rounding errors,
    //you may end up evaluating sqrt(1-x^2) for x =1.00000000000004
    //resulting in serious visual glitches.
    //Note: the remarks above were discovered the painful way (trial and error).
    theX = this.coordinateFunctions[0](theT);
    theY = this.coordinateFunctions[1](theT);
    if (!isFinite(theY) || !isFinite(theX))
      console.log('Failed to evaluate: ' + this.theFunction + ' at x = ' + theX);
    if (Math.abs(theY)>100000 || Math.abs(theX)>100000)
    { if (!skippedValues)
        console.log('Curve point: ' + [theX, theY] + " is too large, skipping. Further errors suppressed.");
      skippedValues = true;
      continue;
    }
    theCoords = theCanvas.coordsMathToScreen([theX, theY]);
    theSurface.lineTo(theCoords[0], theCoords[1]);
  }
}

CurveThreeD.prototype.draw = function(theCanvas)
{ var theSurface = theCanvas.surface;
  theSurface.beginPath();
  this.drawNoFinish(theCanvas, true);
  theSurface.stroke();
}

function Surface(inputxyzFun, inputUVBox, inputPatchDimensions, inputColors, inputContourWidth)
{ this.xyzFun = inputxyzFun;
  this.uvBox = inputUVBox;
  this.patchDimensions = inputPatchDimensions;
  this.colors = inputColors;
  this.contourWidth = inputContourWidth;
  this.colors.colorUV= colorToHex(this.colors.colorUV);
  this.colors.colorVU= colorToHex(this.colors.colorVU);
  this.deltaU= (inputUVBox[1][0]-inputUVBox[0][0])/this.patchDimensions[0];
  this.deltaV= (inputUVBox[1][1]-inputUVBox[0][1])/this.patchDimensions[1];
  this.numSamplesUSegment =10;
  this.numSamplesVSegment =10;
}

function Point(inputLocation, inputColor)
{ this.location = inputLocation;
  this.color = colorToRGB(inputColor);
}

function Patch(inputBase, inputEdge1, inputEdge2, inputColorUV, inputColorVU) { 
  this.base = inputBase.slice();
  this.edge1 = inputEdge1.slice();
  this.edge2 = inputEdge2.slice();
  this.colorUV= colorToRGB(inputColorUV);
  this.colorVU= colorToRGB(inputColorVU);
  this.v1=vectorPlusVector(this.base, this.edge1);
  this.v2=vectorPlusVector(this.base, this.edge2);
  this.vEnd =vectorPlusVector(this.v1, this.edge2);
  this.internalPoint = this.base.slice();
  vectorAddVectorTimesScalar(this.internalPoint, this.edge1, 0.5);
  vectorAddVectorTimesScalar(this.internalPoint, this.edge2, 0.5);
  this.normalVector =vectorCrossVector(inputEdge1, inputEdge2);
  this.adjacentContours =[];
  this.traversalOrder =[];
  this.patchesBelowMe =[];
  this.patchesAboveMe =[];
  this.index = - 1;
}

function Contour(inputPoints, inputColor, inputLineWidth)
{ this.thePoints = inputPoints.slice();
  this.thePointsMathScreen =[];
  this.color = colorToRGB(inputColor);
  this.adjacentPatches =[];
  this.index = - 1;
  this.lineWidth = inputLineWidth;
}

function colorRGBToHex(r, g, b){ 
  return "#" + ((1 << 24) + (r << 16) + (g << 8) + b).toString(16).slice(1);
}

function colorRGBToString(input) { 
  return `rgb(${input[0].toString()},${input[1].toString()},${input[2].toString()})`;
}

function colorScale(inputRGB, theScale){ 
  var result = [0, 0, 0];
  for (var i = 0; i < inputRGB.length; i ++) { 
    result[i] = Math.round(inputRGB[i] * theScale);
    if (result[i] < 0) {
      result[i] = 0;
    }
    if (result[i] > 255) {
      result[i] = 255;
    }
  }
  return result;
}

function colorToRGB(input)
{ if (input.length ===3 && !(typeof input === 'string'))
    return input;
  var hex = colorToHex(input);
  return [parseInt(hex.slice(1,3), 16), parseInt(hex.slice(3,5), 16), parseInt(hex.slice(5,7), 16)];
}

function colorToHex(color) { 
  var colors = {
    "aliceblue":"#f0f8ff","antiquewhite":"#faebd7","aqua":"#00ffff","aquamarine":"#7fffd4","azure":"#f0ffff",
    "beige":"#f5f5dc","bisque":"#ffe4c4","black":"#000000","blanchedalmond":"#ffebcd","blue":"#0000ff","blueviolet":"#8a2be2","brown":"#a52a2a","burlywood":"#deb887",
    "cadetblue":"#5f9ea0","chartreuse":"#7fff00","chocolate":"#d2691e","coral":"#ff7f50","cornflowerblue":"#6495ed","cornsilk":"#fff8dc","crimson":"#dc143c","cyan":"#00ffff",
    "darkblue":"#00008b","darkcyan":"#008b8b","darkgoldenrod":"#b8860b","darkgray":"#a9a9a9","darkgreen":"#006400","darkkhaki":"#bdb76b","darkmagenta":"#8b008b","darkolivegreen":"#556b2f",
    "darkorange":"#ff8c00","darkorchid":"#9932cc","darkred":"#8b0000","darksalmon":"#e9967a","darkseagreen":"#8fbc8f","darkslateblue":"#483d8b","darkslategray":"#2f4f4f","darkturquoise":"#00ced1",
    "darkviolet":"#9400d3","deeppink":"#ff1493","deepskyblue":"#00bfff","dimgray":"#696969","dodgerblue":"#1e90ff",
    "firebrick":"#b22222","floralwhite":"#fffaf0","forestgreen":"#228b22","fuchsia":"#ff00ff",
    "gainsboro":"#dcdcdc","ghostwhite":"#f8f8ff","gold":"#ffd700","goldenrod":"#daa520","gray":"#808080","green":"#008000","greenyellow":"#adff2f",
    "honeydew":"#f0fff0","hotpink":"#ff69b4",
    "indianred ":"#cd5c5c","indigo":"#4b0082","ivory":"#fffff0","khaki":"#f0e68c",
    "lavender":"#e6e6fa","lavenderblush":"#fff0f5","lawngreen":"#7cfc00","lemonchiffon":"#fffacd","lightblue":"#add8e6","lightcoral":"#f08080","lightcyan":"#e0ffff","lightgoldenrodyellow":"#fafad2",
    "lightgrey":"#d3d3d3","lightgreen":"#90ee90","lightpink":"#ffb6c1","lightsalmon":"#ffa07a","lightseagreen":"#20b2aa","lightskyblue":"#87cefa","lightslategray":"#778899","lightsteelblue":"#b0c4de",
    "lightyellow":"#ffffe0","lime":"#00ff00","limegreen":"#32cd32","linen":"#faf0e6",
    "magenta":"#ff00ff","maroon":"#800000","mediumaquamarine":"#66cdaa","mediumblue":"#0000cd","mediumorchid":"#ba55d3","mediumpurple":"#9370d8","mediumseagreen":"#3cb371","mediumslateblue":"#7b68ee",
    "mediumspringgreen":"#00fa9a","mediumturquoise":"#48d1cc","mediumvioletred":"#c71585","midnightblue":"#191970","mintcream":"#f5fffa","mistyrose":"#ffe4e1","moccasin":"#ffe4b5",
    "navajowhite":"#ffdead","navy":"#000080",
    "oldlace":"#fdf5e6","olive":"#808000","olivedrab":"#6b8e23","orange":"#ffa500","orangered":"#ff4500","orchid":"#da70d6",
    "palegoldenrod":"#eee8aa","palegreen":"#98fb98","paleturquoise":"#afeeee","palevioletred":"#d87093","papayawhip":"#ffefd5","peachpuff":"#ffdab9","peru":"#cd853f","pink":"#ffc0cb","plum":"#dda0dd","powderblue":"#b0e0e6","purple":"#800080",
    "red":"#ff0000","rosybrown":"#bc8f8f","royalblue":"#4169e1",
    "saddlebrown":"#8b4513","salmon":"#fa8072","sandybrown":"#f4a460","seagreen":"#2e8b57","seashell":"#fff5ee","sienna":"#a0522d","silver":"#c0c0c0","skyblue":"#87ceeb","slateblue":"#6a5acd","slategray":"#708090","snow":"#fffafa","springgreen":"#00ff7f","steelblue":"#4682b4",
    "tan":"#d2b48c","teal":"#008080","thistle":"#d8bfd8","tomato":"#ff6347","turquoise":"#40e0d0",
    "violet":"#ee82ee",
    "wheat":"#f5deb3","white":"#ffffff","whitesmoke":"#f5f5f5",
    "yellow":"#ffff00","yellowgreen":"#9acd32"
  };
  if (typeof colors[color.toLowerCase()] != 'undefined') {
    return colors[color.toLowerCase()];
  }
  if (color.length === 3) {
    return colorRGBToHex(color[0], color[1], color[2]);
  }
  return color;
}

Drawing.prototype.deleteCanvas = function(canvasId) { 
  if (typeof canvasId !== "string") {
    canvasId = canvasId.id;
  }
  if (this.canvases[canvasId] !== undefined && this.canvases[canvasId] !== null) { 
    delete this.canvases[canvasId];
  }
}

//Drawing.prototype.resetCanvas = function (inputCanvas) { 
//  if (this.canvases[inputCanvas.id] !== undefined && this.canvases[inputCanvas.id] !== null) { 
//    this.canvases[inputCanvas.id].init(inputCanvas.id);
//  }
//}

function PointsTwoD(inputLocation, inputColor)
{ this.location = inputLocation.slice();
  this.color = colorToRGB(inputColor);
  this.type = "points";

}

PointsTwoD.prototype.accountBoundingBox = function(inputOutputBox)
{ for (var i = 0; i < this.location.length; i ++)
    accountBoundingBox(this.location[i], inputOutputBox);
}

PointsTwoD.prototype.draw = function(theCanvas)
{ var theSurface = theCanvas.surface;
  for (var i = 0; i < this.location.length; i ++)
  { theSurface.beginPath();
    theSurface.strokeStyle = colorRGBToString(this.color);
    theSurface.fillStyle = colorRGBToString(this.color );
    var theCoords = theCanvas.coordsMathToScreen(this.location[i]);
    theSurface.arc(theCoords[0], theCoords[1],3, 0, Math.PI*2);
    theSurface.fill();
  }
}

function accountBoundingBox(inputPoint, outputBox)
{ for (var i = 0; i < inputPoint.length; i ++)
  { if (inputPoint[i]<outputBox[0][i])
      outputBox[0][i] = inputPoint[i];
  }
  for (i = 0; i < inputPoint.length; i ++)
  { if (inputPoint[i] >outputBox[1][i])
      outputBox[1][i] = inputPoint[i];
  }
}

function CurveTwoD(
  inputCoordinateFunctions, inputLeftPt, inputRightPt,
  inputNumSegments, inputColor, inputLineWidth
) { 
  this.coordinateFunctions = inputCoordinateFunctions;
  this.leftPt = inputLeftPt;
  this.rightPt = inputRightPt;
  this.color = colorToRGB(inputColor);
  this.numSegments = inputNumSegments;
  this.lineWidth = inputLineWidth;
}

CurveTwoD.prototype.accountBoundingBox = function(inputOutputBox) { 
  var theT= this.leftPt;
  var theX = this.coordinateFunctions[0](theT);
  var theY = this.coordinateFunctions[1](theT);
  accountBoundingBox([theX, theY], inputOutputBox);
  for (var i = 0; i < this.numSegments; i ++) { 
    var theRatio = i/(this.numSegments- 1);
    theT= this.leftPt *(1-theRatio) +  this.rightPt*theRatio;
    theX = this.coordinateFunctions[0](theT);
    theY = this.coordinateFunctions[1](theT);
    accountBoundingBox([theX, theY], inputOutputBox);
  }
}

CurveTwoD.prototype.draw = function(theCanvas) { 
  var theSurface = theCanvas.surface;
  theSurface.beginPath();
  this.drawNoFinish(theCanvas, true);
  theSurface.stroke();
}

CurveTwoD.prototype.drawNoFinish = function(theCanvas, startByMoving) { 
  var theSurface = theCanvas.surface;
  theSurface.strokeStyle = colorRGBToString(this.color);
  theSurface.fillStyle = colorRGBToString(this.color);
  var theT= this.leftPt;
  var theX = this.coordinateFunctions[0](theT);
  var theY = this.coordinateFunctions[1](theT);
  var theCoords = theCanvas.coordsMathToScreen([theX, theY]);
  var alreadyMoved = false;
  if (startByMoving) {
    alreadyMoved = true;
  }
  theSurface.lineWidth = this.lineWidth;
  var skippedValues = false;
  for (var i = 0; i < this.numSegments; i ++) { 
    var theRatio = i / (this.numSegments - 1);
    theT = this.leftPt * (1 - theRatio) + this.rightPt * theRatio; //<- this way of
    //computing x introduces smaller numerical errors.
    //For example, suppose you plot sqrt(1-x^2) from - 1 to 1.
    //If not careful with rounding errors,
    //you may end up evaluating sqrt(1-x^2) for x =1.00000000000004
    //resulting in serious visual glitches.
    //Note: the remarks above were discovered the painful way (trial and error).
    theX = this.coordinateFunctions[0](theT);
    theY = this.coordinateFunctions[1](theT);
    if (!isFinite(theY) || !isFinite(theX)) {
      console.log(`Failed to evaluate: ${this.theFunction} at x = ${theX}`);
    }
    if (Math.abs(theY) > 100000 || Math.abs(theX) > 100000) { 
      if (!skippedValues) {
        console.log(`Curve point: [${theX}, ${theY}] is too large, skipping. Further errors suppressed.`);
      }
      skippedValues = true;
      continue;
    }
    theCoords = theCanvas.coordsMathToScreen([theX, theY]);
    if (!alreadyMoved) { 
      alreadyMoved = true;
      theSurface.moveTo(theCoords[0], theCoords[1]);
    } else {
      theSurface.lineTo(theCoords[0], theCoords[1]);
    }
  }
}

function PathTwoD(inputPath, inputColor, inputFillColor, inputLineWidth)
{ this.path = inputPath;
  this.color = colorToRGB(inputColor);
  this.colorFill = colorToRGB(inputFillColor);
  this.isFilled = false;
  this.type = "path";
  this.lineWidth = inputLineWidth;
}

PathTwoD.prototype.accountBoundingBox = function(inputOutputBox) { 
  for (var i = 0; i < this.path.length; i ++) {
    accountBoundingBox(this.path[i], inputOutputBox);
  }
}

PathTwoD.prototype.drawNoFinish = function(theCanvas, startByMoving) { 
  if (this.path.length < 1)
    return;
  var theSurface = theCanvas.surface;
  var theCoords = theCanvas.coordsMathToScreen(this.path[0]);
  if (startByMoving) {
    theSurface.moveTo(theCoords[0], theCoords[1]);
  } else {
    theSurface.lineTo(theCoords[0], theCoords[1]);
  }
  theSurface.lineWidth = this.lineWidth;
  for (var i =1; i < this.path.length; i ++) { 
    theCoords = theCanvas.coordsMathToScreen(this.path[i]);
    theSurface.lineTo(theCoords[0], theCoords[1]);
  }
  theSurface.strokeStyle = colorRGBToString(this.color);
  theSurface.fillStyle = colorRGBToString(this.colorFill);
  if (this.isFilled) {
    theSurface.fill();
  }
}

PathTwoD.prototype.draw = function(theCanvas) { 
  if (this.path.length < 1) {
    return;
  }
  var theSurface = theCanvas.surface;
  theSurface.beginPath();
  this.drawNoFinish(theCanvas, true);
  theSurface.stroke();
}

function drawCoordinateAxesTwoD() { 
}

drawCoordinateAxesTwoD.prototype.draw = function(theCanvas) { 
  this.drawNoFinish(theCanvas, true);
}

drawCoordinateAxesTwoD.prototype.accountBoundingBox = function(inputOutputBox)
{ accountBoundingBox([0,1], inputOutputBox);
  accountBoundingBox([1,0], inputOutputBox);
}

drawCoordinateAxesTwoD.prototype.drawNoFinish = function(theCanvas, startByMoving) { 
  var lowLeft = theCanvas.coordsScreenToMathScreen([0, theCanvas.height]);
  var topRight = theCanvas.coordsScreenToMathScreen([theCanvas.width, 0]);
  var theSurface = theCanvas.surface;
  theSurface.strokeStyle = colorRGBToString([0, 0, 0]);
  theSurface.fillStyle = colorRGBToString([0, 0, 0] );
  theSurface.lineWidth =1;
  var left = lowLeft[0];
  var right = topRight[0];
  var bottom= lowLeft[1];
  var top = topRight[1];
  theSurface.beginPath();
  var theCoords = theCanvas.coordsMathScreenToScreen([0, bottom]);
  theSurface.moveTo(theCoords[0], theCoords[1]);
  theCoords = theCanvas.coordsMathScreenToScreen([0, top]);
  theSurface.lineTo(theCoords[0], theCoords[1]);
  theSurface.stroke();
  theCoords = theCanvas.coordsMathScreenToScreen([left, 0]);
  theSurface.moveTo(theCoords[0], theCoords[1]);
  theCoords = theCanvas.coordsMathScreenToScreen([right, 0]);
  theSurface.lineTo(theCoords[0], theCoords[1]);
  theSurface.stroke();
  theCoords = theCanvas.coordsMathScreenToScreen([1, -0.1]);
  theSurface.moveTo(theCoords[0], theCoords[1]);
  theCoords = theCanvas.coordsMathScreenToScreen([1, 0.1]);
  theSurface.lineTo(theCoords[0], theCoords[1]);
  theSurface.stroke();
  theCoords = theCanvas.coordsMathScreenToScreen([1, 0]);
  theSurface.fillText("1", theCoords[0], theCoords[1] + 10);
}

function AxesGrid() { 
}

AxesGrid.prototype.draw = function(theCanvas) { 
  this.drawNoFinish(theCanvas, true);
}

AxesGrid.prototype.drawNoFinish = function(theCanvas, startByMoving) { 
  var lowLeft = theCanvas.coordsScreenToMathScreen([0, theCanvas.height]);
  var topRight = theCanvas.coordsScreenToMathScreen([theCanvas.width, 0]);
  var theSurface = theCanvas.surface;
  theSurface.strokeStyle = colorRGBToString([160, 160, 160]);
  theSurface.fillStyle = colorRGBToString([160, 160, 160] );
  theSurface.lineWidth = 0.1;
  var left = lowLeft[0];
  var right = topRight[0];
  var bottom = lowLeft[1];
  var top = topRight[1];
  var floorLeft = Math.floor(left);
  var ceilRight = Math.ceil(right);
  var floorBottom = Math.floor(bottom);
  var ceilTop = Math.ceil(top);
  var distanceBetweenMarks = 30;
  var DeltaHorizontal = Math.max(1, Math.floor ((ceilRight - floorLeft) / (theCanvas.width/distanceBetweenMarks)));
  var DeltaVertical = Math.max(1, Math.floor ((ceilTop - floorBottom) / (theCanvas.height/distanceBetweenMarks)));
  var Delta = Math.max(DeltaHorizontal, DeltaVertical);
  theSurface.beginPath();
  for (var i = floorLeft; i <= ceilRight; i += Delta) { 
    var theCoords = theCanvas.coordsMathScreenToScreen([i, bottom]);
    theSurface.moveTo(theCoords[0], theCoords[1]);
    theCoords = theCanvas.coordsMathScreenToScreen([i, top]);
    theSurface.lineTo(theCoords[0], theCoords[1]);
    theSurface.stroke();
  }
  for (i = floorBottom; i <= ceilTop; i += Delta)
  { theCoords = theCanvas.coordsMathScreenToScreen([left, i]);
    theSurface.moveTo(theCoords[0], theCoords[1]);
    theCoords = theCanvas.coordsMathScreenToScreen([right, i]);
    theSurface.lineTo(theCoords[0], theCoords[1]);
    theSurface.stroke();
  }
  theSurface.strokeStyle = colorRGBToString([0, 0, 0]);
  theSurface.fillStyle = colorRGBToString([0, 0, 0] );
  var counter = 0;
  for (var i = floorLeft; i <= ceilRight; i += Delta)
  { counter ++;
    //if (counter%2=== 0 && i !==1){
      theCoords = theCanvas.coordsMathScreenToScreen([i, 0]);
      theSurface.fillText(i, theCoords[0], theCoords[1] + 10);
    //}
  }
  counter = 0;
  for (i = floorBottom; i <= ceilTop; i += Delta)
  { counter ++;
    //if (counter%2=== 0 && i !==1){
      theCoords = theCanvas.coordsMathScreenToScreen([ 0,i]);
      theSurface.fillText(i, theCoords[0]- 10, theCoords[1]);
    //}
  }
};


function PlotTwoD(inputTheFn, inputLeftPt, inputRightPt, inputNumSegments, inputColor, inputLineWidth) { 
  this.theFunction = inputTheFn;
  this.leftPt = inputLeftPt;
  this.rightPt = inputRightPt;
  this.color = colorToRGB(inputColor);
  if (inputLineWidth === undefined) {
    this.lineWidth = 1;
  } else {
    this.lineWidth = inputLineWidth;
  }
  this.type = "plotFunction";
  this.numSegments = inputNumSegments;
  if (inputRightPt !== "infinity" && inputLeftPt !== "minusInfinity") {
    this.Delta = (inputRightPt - inputLeftPt) / inputNumSegments;
  }
}

PlotTwoD.prototype.accountBoundingBox = function(inputOutputBox) { 
  for (var i = 0; i < this.numSegments; i ++) { 
    var theRatio = i / (this.numSegments - 1);
    var theX = this.leftPt * (1 - theRatio) +  this.rightPt * theRatio;
    var theY = this.theFunction(theX);
    if (!isFinite(theY)) {
      continue;
    }
    if (Math.abs(theY) > 100000) {
      continue;
    }
    accountBoundingBox([theX, theY], inputOutputBox);
  }
}

PlotTwoD.prototype.drawNoFinish = function(theCanvas, startByMoving)
{ var theSurface = theCanvas.surface;
  theSurface.strokeStyle = colorRGBToString(this.color);
  theSurface.fillStyle = colorRGBToString(this.color);
  var realLeft = this.leftPt;
  var realRight = this.rightPt;
  if (realLeft === "minusInfinity")
    realLeft = theCanvas.leftMostPlotPoint;
  if (this.rightPt === "infinity" || this.leftPt === "minusInfinity")
  { if (realRight === "infinity")
      realRight = theCanvas.rightMostPlotPoint;
    this.Delta = (realRight-realLeft)/this.numSegments;
  }
  var theX =realLeft;
  var theY = this.theFunction(theX);
  var theCoords = theCanvas.coordsMathToScreen([theX, theY]);
  var alreadyMoved = false;
  if (startByMoving)
    alreadyMoved = true;
  theSurface.lineWidth = this.lineWidth;
  var skippedValues = false;
  for (var i = 0; i < this.numSegments; i ++)
  { var theRatio = i/(this.numSegments- 1);
    theX = realLeft *(1-theRatio) +  realRight*theRatio; //<- this way of
    //computing x this way introduces smaller numerical errors.
    //For example, suppose you plot sqrt(1-x^2) from - 1 to 1.
    //If not careful with rounding errors,
    //you may end up evaluating sqrt(1-x^2) for x =1.00000000000004
    //resulting in serious visual glitches.
    //Note: the remarks above were discovered the painful way (trial and error).
    theY = this.theFunction(theX);
    if (!isFinite(theY) )
      console.log('Failed to evaluate: ' + this.theFunction + ' at x = ' + theX);
    if (Math.abs(theY)>10000)
    { if (!skippedValues)
        console.log('Function result: ' + theY + " is too large, skipping. Further errors suppressed.");
      skippedValues = true;
      continue;
    }
    theCoords = theCanvas.coordsMathToScreen([theX, theY]);
    if (!alreadyMoved)
    { alreadyMoved = true;
      theSurface.moveTo(theCoords[0], theCoords[1]);
    } else
      theSurface.lineTo(theCoords[0], theCoords[1]);
  }
}

PlotTwoD.prototype.draw = function(theCanvas)
{ var theSurface = theCanvas.surface;
  theSurface.beginPath();
  this.drawNoFinish(theCanvas,true);
  theSurface.stroke();
}

function TextPlotTwoD(inputLocation, inputText, inputColor)
{ this.location = inputLocation;
  this.text = inputText;
  this.color = colorToRGB(inputColor);
  this.type ="plotText";
}

TextPlotTwoD.prototype.accountBoundingBox = function(inputOutputBox)
{ accountBoundingBox(this.location, inputOutputBox);
}

TextPlotTwoD.prototype.drawNoFinish = function(theCanvas)
{ var theSurface = theCanvas.surface;
  theSurface.strokeStyle = colorRGBToString(this.color);
  theSurface.fillStyle = colorRGBToString(this.color);
  var theCoords = theCanvas.coordsMathToScreen(this.location);
  theSurface.fillText(this.text, theCoords[0], theCoords[1]);
}

TextPlotTwoD.prototype.draw = function(theCanvas)
{ var theSurface = theCanvas.surface;
  theSurface.beginPath();
  this.drawNoFinish(theCanvas);
  theSurface.stroke();
}

function VectorFieldTwoD(
  inputField, inputIsDirectionField,
  inputLowLeft, inputHighRight,
  inputNumSegmentsXY,
  inputDesiredLengthDirectionVectors,
  inputColor, inputLineWidth
) { 
  this.theField = inputField;
  this.isDirectionField = inputIsDirectionField;
  this.lowLeft = inputLowLeft;
  this.highRight = inputHighRight;
  this.numSegmentsXY = inputNumSegmentsXY;
  this.desiredLengthDirectionVectors = inputDesiredLengthDirectionVectors;
  this.color = colorToRGB(inputColor);
  this.lineWidth = inputLineWidth;
}

VectorFieldTwoD.prototype.accountBoundingBox = function(inputOutputBox)
{ accountBoundingBox(this.lowLeft, inputOutputBox);
  accountBoundingBox(this.highRight, inputOutputBox);
}

VectorFieldTwoD.prototype.draw = function(theCanvas) { 
  var theSurface = theCanvas.surface;
  theSurface.beginPath();
  theSurface.strokeStyle = colorRGBToString(this.color);
  theSurface.fillStyle = colorRGBToString(this.color);
  theSurface.lineWidth = this.lineWidth;
  for (var i = 0; i < this.numSegmentsXY[0]; i ++)
  { var theRatioX = i/(this.numSegmentsXY[0]- 1);
    var theX = this.lowLeft[0] *(1-theRatioX) +
      this.highRight[0]*theRatioX;
    for (var j = 0; j < this.numSegmentsXY[1]; j ++)
    { var theRatioY = j/(this.numSegmentsXY[1]- 1);
      var theY = this.lowLeft[1] *(1-theRatioY) +
        this.highRight[1]*theRatioY;
      var theV= this.theField(theX, theY);
      if (this.isDirectionField) {
        if (vectorLength(theV) !== 0) {
          vectorTimesScalar(theV,this.desiredLengthDirectionVectors* 1/vectorLength(theV));
        }
      }
      var headMath = [theX+theV[0]/2, theY+theV[1]/2 ];
      var tailMath = [theX-theV[0]/2, theY-theV[1]/2 ];
      var headScreen = theCanvas.coordsMathToScreen(headMath);
      var tailScreen = theCanvas.coordsMathToScreen(tailMath);
      var baseScreen = theCanvas.coordsMathToScreen([theX, theY]);
      theSurface.moveTo(tailScreen[0], tailScreen[1]);
      theSurface.lineTo(headScreen[0], headScreen[1]);
      theSurface.stroke();
      //theSurface.moveTo(baseScreen[0], baseScreen[1]);
      //theSurface.arc(baseScreen[0], baseScreen[1],0.5, 0, Math.PI*2);
      //theSurface.fill();
    }
  }
}

function SegmentTwoD(inputLeftPt, inputRightPt, inputColor, inputLineWidth)
{ this.leftPt = inputLeftPt;
  this.rightPt = inputRightPt;
  this.color = colorToRGB(inputColor);
  this.type ="segment";
  this.lineWidth = inputLineWidth;
}

SegmentTwoD.prototype.accountBoundingBox = function(inputOutputBox) { 
  accountBoundingBox(this.leftPt , inputOutputBox);
  accountBoundingBox(this.rightPt, inputOutputBox);
}

SegmentTwoD.prototype.drawNoFinish = function (theCanvas, startByMoving)
{ var theSurface = theCanvas.surface;
  var theCoords = theCanvas.coordsMathToScreen(this.leftPt);
  theSurface.lineWidth = this.lineWidth;
  if (startByMoving)
    theSurface.moveTo(theCoords[0], theCoords[1]);
  else
    theSurface.lineTo(theCoords[0], theCoords[1]);
  theCoords = theCanvas.coordsMathToScreen(this.rightPt);
  theSurface.lineTo(theCoords[0], theCoords[1]);
}

SegmentTwoD.prototype.draw = function(theCanvas)
{ var theSurface = theCanvas.surface;
  theSurface.beginPath();
  theSurface.strokeStyle = colorRGBToString(this.color);
  theSurface.fillStyle = colorRGBToString(this.color);
  this.drawNoFinish(theCanvas, true);
  theSurface.stroke();
}

function PlotFillTwoD(inputCanvas, inputColor)
{ this.indexFillStart = inputCanvas.theObjects.length;
  this.color = colorToRGB(inputColor);
  this.accountBoundingBox = function(inputOutputBox)
  {
  };
  this.draw = function(inputCanvas)
  { var theSurface = inputCanvas.surface;
    var theObs = inputCanvas.theObjects;
    var tempCounter = inputCanvas.numDrawnObjects;
    theSurface.beginPath();
    var firstRun = true;
    for (tempCounter ++; tempCounter<theObs.length; tempCounter ++)
    { var currentO = theObs[tempCounter];
      if (currentO.type === "plotFillFinish")
        break;
      currentO.drawNoFinish(inputCanvas, firstRun);
      firstRun = false;
    }
    theSurface.fillStyle = colorRGBToString(this.color);
    theSurface.fill();
    for (inputCanvas.numDrawnObjects++; inputCanvas.numDrawnObjects<theObs.length; inputCanvas.numDrawnObjects++)
    { currentO = theObs[inputCanvas.numDrawnObjects];
      if (currentO.type === "plotFillFinish")
        break;
      currentO.draw(inputCanvas);
    }
  }
}

function CanvasTwoD(inputCanvas) { 
  this.canvasResetFunction = null;
  this.theObjects =[];
  this.surface = null;
  this.canvasContainer = null;
  this.canvasId = null;
  this.screenBasisOrthonormal = [];
  this.spanMessages = null;
  this.spanCriticalErrors = null;
  this.spanControls = null;
  this.numDrawnObjects = 0;
  this.boundingBoxMath = [[-0.1,-0.1],[0.1,0.1]];
  this.leftMostPlotPoint ="none";
  this.rightMostPlotPoint ="none";
  this.width = inputCanvas.width;
  this.height = inputCanvas.height;
  this.centerCanvasX = inputCanvas.width/2;
  this.centerCanvasY = inputCanvas.height/2;
  this.centerX = this.centerCanvasX;
  this.centerY = this.centerCanvasY;
  this.viewWindowDefault =[[-5,-5],[5,5]];
  this.scale = 50;
  this.mousePosition = [];
  this.clickedPosition = [];
  this.positionDelta = [];
  this.spanMessages = undefined;
  this.textMouseInfo = "";
  this.textProjectionInfo = "";
  this.textPatchInfo = "";
  this.textErrors = "";
  this.textPerformance = "";
  this.selectedElement = "";
  this.redrawStart = 0;
  this.redrawFinish = 0;
  this.redrawTime = 0;
  this.defaultNumSegmentsPerContour = 10;
  this.flagShowPerformance = true;
  this.flagShowAxesTicks = false;
  this.flagShowGrid = false;
}

CanvasTwoD.prototype.drawPoints = function (inputPoints, inputColor) { 
  this.theObjects.push(new PointsTwoD(inputPoints, inputColor));
}

CanvasTwoD.prototype.drawLine = function (inputLeftPt, inputRightPt, inputColor, inputLineWidth) { 
  var newLine = new SegmentTwoD(inputLeftPt, inputRightPt, inputColor, inputLineWidth);
  this.theObjects.push(newLine);
}

CanvasTwoD.prototype.drawGrid = function () { 
  this.flagShowAxesTicks = true;
  this.flagShowGrid = true;
  this.theObjects.push(new AxesGrid());
}

CanvasTwoD.prototype.drawCoordinateAxes = function () { 
  this.theObjects.push(new drawCoordinateAxesTwoD());
}

CanvasTwoD.prototype.drawVectorField = function (
  inputField, inputIsDirectionField, inputLowLeft,
  inputHighRight, inputNumSegmentsXY,
  inputDesiredLengthDirectionVectors,
  inputColor, inputLineWidth
) { 
  var newLine = new VectorFieldTwoD(
    inputField, inputIsDirectionField, inputLowLeft, inputHighRight,
    inputNumSegmentsXY,
    inputDesiredLengthDirectionVectors,
    inputColor, inputLineWidth
  );
  this.theObjects.push(newLine);
}

CanvasTwoD.prototype.drawPath = function (inputPath, inputColor, inputLineWidth) { 
  var newPath = new PathTwoD(inputPath, inputColor, inputColor, inputLineWidth);
  this.theObjects.push(newPath);
}

CanvasTwoD.prototype.drawPathFilled = function (inputPath, inputContourColor, inputFillColor) { 
  var newPath = new PathTwoD(inputPath, inputContourColor, inputFillColor);
  newPath.isFilled = true;
  this.theObjects.push(newPath);
}

CanvasTwoD.prototype.drawFunction = function (inputFun, inputLeftPt, inputRightPt, inputNumSegments, inputColor, inputLineWidth) { 
  var newPlot = new PlotTwoD(inputFun, inputLeftPt, inputRightPt, inputNumSegments, inputColor, inputLineWidth);
  if (inputLeftPt !== "minusInfinity" && inputRightPt !== "infinity") { 
    if (this.leftMostPlotPoint === "none") {
      this.leftMostPlotPoint = inputLeftPt;
    } else {
      this.leftMostPlotPoint = Math.min(inputLeftPt, this.leftMostPlotPoint);
    }
    if (this.rightMostPlotPoint === "none") {
      this.rightMostPlotPoint = inputRightPt;
    } else {
      this.rightMostPlotPoint = Math.max(inputRightPt, this.rightMostPlotPoint);
    }
  }
  this.theObjects.push(newPlot);
}

CanvasTwoD.prototype.drawCurve = function (inputCoordinateFuns, inputLeftPt, inputRightPt, inputNumSegments, inputColor, inputLineWidth) { 
  var newPlot = new CurveTwoD(inputCoordinateFuns, inputLeftPt, inputRightPt, inputNumSegments, inputColor, inputLineWidth);
  this.theObjects.push(newPlot);
}

CanvasTwoD.prototype.drawText = function (inputLocation, inputText, inputColor) { 
  var newPlot = new TextPlotTwoD(inputLocation, inputText, inputColor);
  this.theObjects.push(newPlot);
}

CanvasTwoD.prototype.plotFillStart = function (inputColor) { 
  this.theObjects.push(new PlotFillTwoD(this, inputColor));
}

CanvasTwoD.prototype.plotFillFinish = function () { 
  this.theObjects.push({type:"plotFillFinish"});
}

CanvasTwoD.prototype.computeViewWindow = function() { 
  this.boundingBoxMath = [[- 0.1, - 0.1], [0.1, 0.1]];
  for (var i = 0; i < this.theObjects.length; i ++) { 
    if (this.theObjects[i].accountBoundingBox === undefined) {
      continue;
    }
    this.theObjects[i].accountBoundingBox(this.boundingBoxMath);
  }
  vectorTimesScalar(this.boundingBoxMath[0], 1.05);
  vectorTimesScalar(this.boundingBoxMath[1], 1.05);
  this.setViewWindow(this.boundingBoxMath[0], this.boundingBoxMath[1]);
};

CanvasTwoD.prototype.setViewWindow = function(leftLowPt, rightUpPt) { 
  this.viewWindowDefault =[leftLowPt,rightUpPt];
  var leftLowScreen = this.coordsMathToScreen(leftLowPt);
  var rightUpScreen = this.coordsMathToScreen(rightUpPt);
  var desiredHeight = Math.abs(rightUpScreen[1] - leftLowScreen[1]);
  var desiredWidth = Math.abs(rightUpScreen[0] - leftLowScreen[0]);
  var candidateScaleHeight = this.scale * this.height / desiredHeight;
  var candidateScaleWidth = this.scale * this.width / desiredWidth;
  this.scale = Math.min(candidateScaleHeight, candidateScaleWidth);
  //console.log("new scale: "+ this.scale);
  var centerViewWindowMath = vectorPlusVector(leftLowPt,rightUpPt);
  vectorTimesScalar(centerViewWindowMath, 0.5);
  var centerViewWindowScreen = this.coordsMathToScreen(centerViewWindowMath);
  this.centerX += this.centerCanvasX - centerViewWindowScreen[0];
  this.centerY += this.centerCanvasY - centerViewWindowScreen[1];
}

CanvasTwoD.prototype.redraw = function() { 
  this.textPerformance ="";
  this.redrawStart = new Date().getTime();
  var theSurface = this.surface;
  theSurface.clearRect(0, 0, this.width, this.height);
  for (this.numDrawnObjects = 0; this.numDrawnObjects < this.theObjects.length; this.numDrawnObjects ++) {
    this.theObjects[this.numDrawnObjects].draw(this);
  }
  var redrawTime = new Date().getTime();
  if (this.flagShowPerformance) { 
    this.textPerformance = "Redraw time (ms): " + (redrawTime - this.redrawStart);
    this.showMessages();
  }
}

CanvasTwoD.prototype.computeBasis = function () { 
  this.screenBasisOrthonormal[0] = [1, 0];
  this.screenBasisOrthonormal[1] = [0, 1];
  this.textProjectionInfo = "";
  this.textProjectionInfo += `<br>e1: ${this.screenBasisOrthonormal[0]} <br>e2: ${this.screenBasisOrthonormal[1]}`;
}

CanvasTwoD.prototype.init = function(inputCanvasId) { 
  this.canvasId = inputCanvasId;
  this.canvasContainer = document.getElementById(inputCanvasId);
  this.surface = this.canvasContainer.getContext("2d");
  this.canvasContainer.addEventListener("DOMMouseScroll", drawing.mouseWheel.bind(drawing), false);
  this.canvasContainer.addEventListener("mousewheel", drawing.mouseWheel.bind(drawing), false);
//    this.canvasContainer.addEventListener("wheel", drawing.mouseWheel.bind(drawing), true);
  this.theObjects = [];
  this.spanMessages = document.getElementById(this.canvasId + "Messages");
  this.spanCriticalErrors = document.getElementById(this.canvasId + "CriticalErrors");
  this.spanControls = document.getElementById(this.canvasId + "Controls");
  this.constructControls();
  this.computeBasis();
}

CanvasTwoD.prototype.resetView = function() { 
  this.setViewWindow(this.viewWindowDefault[0], this.viewWindowDefault[1]);
  this.redraw();
}

CanvasTwoD.prototype.constructControls = function() { 
  drawing.numberOfControlsConstructed ++;
  var controlButtonId = `buttonControlCanvas${drawing.numberOfControlsConstructed}`;
  this.spanControls.innerHTML = `<button id = "${controlButtonId}" style = "border:none; background:none; color:blue; padding:0; text-decoration: underline; cursor:pointer" >reset view</button> `;
  document.getElementById(controlButtonId).addEventListener('click', this.resetView.bind(this));
}

CanvasTwoD.prototype.coordsMathScreenToMath = function(theCoords) { 
  var output = this.screenBasisOrthonormal[0].slice();
  vectorTimesScalar(output, theCoords[0]);
  vectorAddVectorTimesScalar(output, this.screenBasisOrthonormal[1], theCoords[1]);
  return output;
}

CanvasTwoD.prototype.coordsMathToMathScreen = function(vector) { 
  return [
    vectorScalarVector(vector, this.screenBasisOrthonormal[0]),
    vectorScalarVector(vector, this.screenBasisOrthonormal[1])
  ];
}

CanvasTwoD.prototype.coordsMathToScreen = function(vector) { 
  return [
    this.scale * vectorScalarVector(vector, this.screenBasisOrthonormal[0]) + this.centerX,
    (- 1) * this.scale * vectorScalarVector(vector, this.screenBasisOrthonormal[1]) + this.centerY
  ];
}

CanvasTwoD.prototype.coordsScreenAbsoluteToScreen = function(screenX, screenY) { 
  return getPosXPosYObject(this.canvasContainer, screenX, screenY);
}

CanvasTwoD.prototype.coordsScreenToMathScreen = function(screenPos) { 
  return [ 
    (screenPos[0] - this.centerX) / this.scale, 
    (this.centerY - screenPos[1]) / this.scale
  ];
}

CanvasTwoD.prototype.coordsScreenAbsoluteToMathScreen = function(screenX, screenY) { 
  return this.coordsScreenToMathScreen(this.coordsScreenAbsoluteToScreen(screenX, screenY));
}

CanvasTwoD.prototype.coordsMathScreenToScreen = function(theCoords) { 
  return [this.scale * theCoords[0] + this.centerX, this.centerY - this.scale * theCoords[1]];
}

CanvasTwoD.prototype.mouseWheel = function(wheelDelta, screenX, screenY) { 
  var screenPos = this.coordsScreenAbsoluteToScreen(screenX, screenY);
  var mathScreenPos = this.coordsScreenToMathScreen(screenPos);
  if (wheelDelta / this.scale > 0.1) {
    wheelDelta = this.scale * 0.1;
  }
  if (wheelDelta / this.scale < - 0.1) {
    wheelDelta = - this.scale * 0.1;
  }
  this.scale += wheelDelta;
  if (this.scale <= 0) {
    this.scale = 1;
  }
  var intermediateScreenPos = this.coordsMathScreenToScreen(mathScreenPos);
  //console.log("start screen: "+[screenX, screenY]);
  //console.log("intermed. screen: "+ intermediateScreenPos);
  this.centerX = this.centerX + screenPos[0] - intermediateScreenPos[0];
  this.centerY = this.centerY + screenPos[1] - intermediateScreenPos[1];
  this.redraw();
}

CanvasTwoD.prototype.mouseMove = function(screenX, screenY) { 
  if (this.selectedElement === "") {
    return;
  }
  this.mousePosition = this.coordsScreenAbsoluteToMathScreen(screenX, screenY);
  if (this.selectedElement === "origin") {
    this.panAfterCursor();
  }
  this.redrawFinish = new Date().getTime();
  this.redrawTime = this.redrawFinish - this.redrawStart;
}

CanvasTwoD.prototype.panAfterCursor = function() { 
  var difference = vectorMinusVector(this.mousePosition, this.clickedPosition);
  this.centerX += difference[0] * this.scale;
  this.centerY -= difference[1] * this.scale;
  this.redraw();
}

CanvasTwoD.prototype.pointsWithinClickTolerance = function (leftXY, rightXY) { 
  var squaredDistance =
  ((leftXY[0] - rightXY[0]) * (leftXY[0] - rightXY[0]) +
   (leftXY[1] - rightXY[1]) * (leftXY[1] - rightXY[1])) * this.scale;
  return squaredDistance < 1000;
}

CanvasTwoD.prototype.canvasClick = function (screenX, screenY) {
  this.clickedPosition = this.coordsScreenAbsoluteToMathScreen(screenX, screenY);
  this.mousePosition = [];
  //if (this.pointsWithinClickTolerance(this.clickedPosition,[0,0]))
  this.selectedElement = "origin";
  //else
  //  this.selectedElement ="";
  //console.log("DEBUG: flag show performance hardcoded. ");
  if (this.flagShowPerformance) {
    this.logStatus();
  }
}

CanvasTwoD.prototype.selectEmpty = function() { 
  this.selectedElement = "";
}

CanvasTwoD.prototype.showMessages = function() { 
  if (this.spanMessages === null || this.spanMessages === undefined) {
    return;
  }
  var theHTML = "";
  if (this.textPerformance != "") {
    theHTML += this.textPerformance + "<hr>";
  }
  if (this.textErrors !== "") {
    theHTML += `<span style =\"red\"><b>${this.textErrors}</b></span><hr>`;
  }
  theHTML += `<span>${this.textMouseInfo}</span><hr>`;
  theHTML += `<span>${this.textProjectionInfo}</span>`;
  if (this.textPatchInfo !== "") {
    theHTML += `<hr><span>${this.textPatchInfo}</span>`;
  }
  this.spanMessages.innerHTML= theHTML;
}

CanvasTwoD.prototype.logStatus = function() { 
  this.textMouseInfo = "";
  this.textMouseInfo += `time last redraw: ${this.redrawTime}" ms `;
  this.textMouseInfo += `(~ ${(1000 / this.redrawTime).toFixed(1)} f.p.s.)`;
  this.textMouseInfo += `<br>mouse coordinates: ${this.mousePosition}`;
  this.textMouseInfo += `<br>clicked coordinates: ${this.clickedPosition}`;
  this.textMouseInfo += `<br>delta of position: ${this.positionDelta}`;
  this.showMessages();
}

function Canvas(inputCanvas) { 
  this.canvasResetFunction = null;
  this.theIIIdObjects = { 
    thePatches: [],
    theContours: [],
    thePoints: [],
    theLabels: []
  };
  this.screenXY = [0, 0];
  this.flagShowPerformance = true;
  this.thePatchOrder = [];
  this.numAccountedPatches = 0;
  this.numCyclicallyOverlappingPatchTieBreaks = 0;
  this.numContourPoints = 0;
  this.numContourPaths = 0;
  this.patchIsAccounted = [];
  this.surface = null;
  this.canvasContainer = null;
  this.canvasId = null;
  this.screenBasisUserDefault = [[2, 1, 0], [0, 1, 1]];
  this.screenBasisUser = this.screenBasisUserDefault.slice();
  this.screenNormal = [];
  this.screenBasisOrthonormal = [];
  this.zBufferColCount = 20;
  this.zBufferRowCount = 20;
  this.zBuffer = [];
  this.zBufferIndexStrip =[];
  this.bufferDeltaX = 0;
  this.bufferDeltaY = 0;
  this.colorDepthFactor = 0.4;
  this.boundingBoxMathScreen = [[- 0.01, - 0.01], [0.01, 0.01]];
  this.boundingBoxMath = [[- 0.01, - 0.01, - 0.01],[0.01, 0.01, 0.01]];
  this.boundingSegmentZ = [ -0.01, 0.01];
  this.lastCenterScreen = [0, 0];
  this.width = inputCanvas.width;
  this.height = inputCanvas.height;
  this.defaultCenterX = this.width / 2;
  this.defaultCenterY = this.height / 2;
  this.centerX = this.defaultCenterX;
  this.centerY = this.defaultCenterY;
  this.scaleDefault = 50;
  this.scale = this.scaleDefault;
  this.mousePosition = [];
  this.clickedPosition = [];
  this.unitRay = [];
  this.rayComponent = [];
  this.positionDelta = [];
  this.spanMessages = undefined;
  this.textMouseInfo = "";
  this.textProjectionInfo = "";
  this.textPatchInfo = "";
  this.textErrors = "";
  this.textPerformance = "";
  this.angleNormal = 0;
  this.oldAngleNormal = 0;
  this.newAngleNormal = 0;
  this.anglePolar = 0;
  this.selectedElement = "";
  this.selectedVector = [];
  this.selectedScreenBasis = [];
  this.selectedScreenProjectionNormalized = [];
  this.selectedScreenNormal = [];
  this.selectedPolarAngleChange = 0;
  this.redrawStart = 0;
  this.redrawFinish = 0;
  this.redrawTime = 0;
  this.defaultNumSegmentsPerContour = 10;
  this.flagRoundContours = false;
  this.flagRoundPatches = true;
}

Canvas.prototype.drawText = function(theText) { 
  this.theIIIdObjects.theLabels.push(theText);
}

Canvas.prototype.drawCurve = function(theCurve) { 
  var contourPoints = new Array(theCurve.numSegments + 1);
  for (var i = 0; i < theCurve.numSegments + 1; i ++) { 
    var theRatio = i / theCurve.numSegments;
    var currentParam = theCurve.leftPt * (1 - theRatio) + theCurve.rightPt * theRatio;
    contourPoints[i] = theCurve.coordinateFunctions(currentParam);
  }
  this.theIIIdObjects.theContours.push(new Contour(contourPoints, theCurve.color, theCurve.lineWidth));
}

Canvas.prototype.drawPatchStraight = function(base, edge1, edge2, color) { 
  this.theIIIdObjects.thePatches.push(new Patch(base, edge1, edge2, color));
  var patchIndex = this.theIIIdObjects.thePatches.length- 1;
  var thePatch = this.theIIIdObjects.thePatches[patchIndex];
  thePatch.index = patchIndex;
  var theContours = this.theIIIdObjects.theContours;
  thePatch.adjacentContours.push(this.drawLine(thePatch.base, thePatch.v1));
  thePatch.traversalOrder.push(1);

  theContours[theContours.length- 1].adjacentPatches.push(patchIndex);
  thePatch.adjacentContours.push(this.drawLine(thePatch.v1, thePatch.vEnd));
  thePatch.traversalOrder.push(1);

  theContours[theContours.length- 1].adjacentPatches.push(patchIndex);
  thePatch.adjacentContours.push(this.drawLine(thePatch.vEnd, thePatch.v2));
  thePatch.traversalOrder.push(1);

  theContours[theContours.length- 1].adjacentPatches.push(patchIndex);
  thePatch.adjacentContours.push(this.drawLine(thePatch.v2, thePatch.base));
  thePatch.traversalOrder.push(1);

  theContours[theContours.length- 1].adjacentPatches.push(patchIndex);
}

Canvas.prototype.drawPoints = function (inputPoints, inputColor) { 
  for (var i = 0; i < inputPoints.length; i ++) {
    this.theIIIdObjects.thePoints.push(new Point(inputPoints[i], inputColor));
  }
}

Canvas.prototype.drawLine = function (leftPt, rightPt, inputColor, inputLineWidth) { 
  var newContour = new Contour([], inputColor, inputLineWidth);
  var numSegments = this.defaultNumSegmentsPerContour;
  newContour.index = this.theIIIdObjects.theContours.length;
  var incrementVector =vectorMinusVector(rightPt, leftPt);
  if (vectorLength(incrementVector) * 10 > numSegments) {
    numSegments = Math.ceil(vectorLength(incrementVector) * 10);
  }
  newContour.thePoints = new Array(numSegments);
  var incrementScalar = 1 / numSegments;
  vectorTimesScalar(incrementVector, incrementScalar);
  var currentPoint = leftPt.slice();
  for (var i = 0; i < numSegments + 1; i ++) { 
    newContour.thePoints[i] = currentPoint;
    currentPoint = vectorPlusVector(currentPoint, incrementVector);
  }
  this.theIIIdObjects.theContours.push(newContour);
  return this.theIIIdObjects.theContours.length - 1;
}

Canvas.prototype.computePatch = function(thePatch) { 
  thePatch.normalScreen1= vectorCrossVector(this.screenNormal, thePatch.edge1);
  thePatch.normalScreen2= vectorCrossVector(this.screenNormal, thePatch.edge2);
  thePatch.normal = vectorCrossVector(thePatch.edge1, thePatch.edge2);
  thePatch.internalPoint = thePatch.base.slice();
  vectorAddVectorTimesScalar(thePatch.internalPoint, thePatch.edge1, 0.5);
  vectorAddVectorTimesScalar(thePatch.internalPoint, thePatch.edge2, 0.5);
}

Canvas.prototype.computeContour = function(theContour) { 
  if (theContour.thePointsMathScreen.length!== theContour.thePoints.length) {
    theContour.thePointsMathScreen = new Array(theContour.thePoints.length);
  }
  for (var i = 0; i < theContour.thePoints.length; i ++) {
    theContour.thePointsMathScreen[i] = this.coordsMathToMathScreen(theContour.thePoints[i]);
  }
}

Canvas.prototype.pointRelativeToPatch = function(thePoint, thePatch) { 
  if (
    vectorLength(thePatch.normalScreen1) < 0.00001 ||
    vectorLength(thePatch.normalScreen2) < 0.00001 ||
    vectorLength(thePatch.normal) < 0.00001
  ) {
    return 0;
  }
  if (
    vectorScalarVector(vectorMinusVector(thePoint, thePatch.base), thePatch.normalScreen1) *
    vectorScalarVector(vectorMinusVector(thePatch.internalPoint, thePatch.base), thePatch.normalScreen1) > 0
  ) {
    if (
      vectorScalarVector(vectorMinusVector(thePoint, thePatch.vEnd), thePatch.normalScreen1) *
      vectorScalarVector(vectorMinusVector(thePatch.internalPoint, thePatch.vEnd), thePatch.normalScreen1) > 0
    ) {
      if (
        vectorScalarVector(vectorMinusVector(thePoint, thePatch.base), thePatch.normalScreen2) *
        vectorScalarVector(vectorMinusVector(thePatch.internalPoint, thePatch.base), thePatch.normalScreen2) > 0
      ) {
        if (
          vectorScalarVector(vectorMinusVector(thePoint, thePatch.vEnd), thePatch.normalScreen2) *
          vectorScalarVector(vectorMinusVector(thePatch.internalPoint, thePatch.vEnd), thePatch.normalScreen2) > 0
        ) { 
          if (
            vectorScalarVector(vectorMinusVector(thePoint, thePatch.base), thePatch.normal) *
            vectorScalarVector(this.screenNormal, thePatch.normal) <= 0
          ) {
            return - 1;
          }
          if (
            vectorScalarVector(vectorMinusVector(thePoint, thePatch.base), thePatch.normal) *
            vectorScalarVector(this.screenNormal, thePatch.normal) >= 0
          ) {
            return 1;
          }
        }
      }
    }
  }
  return 0;
}

Canvas.prototype.pointIsBehindPatch = function(thePoint, thePatch) { 
  return this.pointRelativeToPatch(thePoint, thePatch) === - 1;
}

Canvas.prototype.pointIsInFrontOfPatch = function(thePoint, thePatch) { 
  return this.pointRelativeToPatch(thePoint, thePatch) === 1;
}

Canvas.prototype.pointIsInForeGround = function(thePoint, containerPatches) { 
  var thePatches = this.theIIIdObjects.thePatches;
  for (var i = 0; i < thePatches.length; i ++) { 
    if (containerPatches.indexOf(i) !== - 1) {
      continue;
    }
    if (this.pointIsBehindPatch(thePoint, thePatches[i])) {
      return false;
    }
  }
  return true;
}

Canvas.prototype.paintMouseInfo = function() { 
  if (
    this.selectedElement !== "default" || this.selectedElement === undefined || 
    this.selectedVector === [] || this.selectedVector === undefined
  ) {
    return;
  }
  if (this.selectedVector.length === 0 || this.selectedVector.length === undefined) {
    return;
  }
//    if (true)
//      return;
  var currentPt;
  this.surface.beginPath();
  this.surface.setLineDash([4, 4]);
  this.surface.lineWidth = 2;
  this.surface.strokeStyle = "green";
  currentPt = this.coordsMathToSelectedScreen(this.selectedVector);
  this.surface.moveTo(currentPt[0], currentPt[1]);
  currentPt = this.coordsMathToSelectedScreen([0, 0, 0]);
  this.surface.lineTo(currentPt[0], currentPt[1]);
  this.surface.stroke();
  this.surface.beginPath();
  this.surface.setLineDash([]);
  this.surface.lineWidth = 2;
  this.surface.strokeStyle = "green";
  currentPt = this.coordsMathToScreen(this.selectedVector);
  this.surface.moveTo(currentPt[0], currentPt[1]);
  currentPt = this.coordsMathToScreen([0, 0, 0]);
  this.surface.lineTo(currentPt[0], currentPt[1]);
  this.surface.stroke();
}

Canvas.prototype.paintOneContour = function(theContour) { 
  if (theContour.thePoints.length < 2) {
    return;
  }
  var theSurface = this.surface;
  var thePts = theContour.thePoints;
  //console.log("line start\n");
  var currentPt = this.coordsMathToScreen(thePts[0]);
  if (this.flagRoundContours) {
    vectorRound(currentPt);
  }
  var oldIsInForeGround = this.pointIsInForeGround(theContour.thePoints[0], theContour.adjacentPatches);
  var newIsInForeground = true;
  var dashIsOn = false;
  theSurface.beginPath();
  theSurface.setLineDash([]);
  this.numContourPaths ++;
//    theSurface.setLineDash([]);
  theSurface.strokeStyle = colorRGBToString(theContour.color);
  if (theContour.lineWidth !== undefined) {
    theSurface.lineWidth = theContour.lineWidth;
  } else {
    theSurface.lineWidth = 1;
  }
  theSurface.moveTo(currentPt[0], currentPt[1]);
  this.numContourPoints += thePts.length;
  for (var i = 1; i < thePts.length; i ++) { 
    newIsInForeground = this.pointIsInForeGround(theContour.thePoints[i],theContour.adjacentPatches);
    if (!newIsInForeground && !oldIsInForeGround && !dashIsOn) { 
      if (i > 1) { 
        theSurface.stroke();
        theSurface.setLineDash([4, 4]);
        theSurface.beginPath();
        this.numContourPaths ++;
        theSurface.moveTo(currentPt[0], currentPt[1]);
      }
      theSurface.setLineDash([4,4]);
      dashIsOn = true;
    } else if (dashIsOn && newIsInForeground) { 
      theSurface.stroke();
      theSurface.setLineDash([]);
      theSurface.beginPath();
      this.numContourPaths ++;
      //theSurface.setLineDash([]);
      //console.log("removed dash\n");
      theSurface.moveTo(currentPt[0], currentPt[1]);
      dashIsOn = false;
    }
    currentPt = this.coordsMathToScreen(thePts[i]);
    if (this.flagRoundContours) {
      vectorRound(currentPt);
    }
    theSurface.lineTo(currentPt[0], currentPt[1]);
    oldIsInForeGround = newIsInForeground;
  }
  theSurface.stroke();
  /////////////////
  if (0) { 
    currentPt = this.coordsMathToScreen(thePts[4]);
    if (this.flagRoundContours) {
      vectorRound(currentPt);
    }
    theSurface.font = "9pt sans-serif";
    theSurface.fillStyle = "black";
    theSurface.fillText(theContour.index, currentPt[0], currentPt[1]);
  }
  //console.log("line end\n");
}

Canvas.prototype.paintOnePatch = function(thePatch) { 
  var theSurface = this.surface;
  var visibilityScalarProd = vectorScalarVector(this.screenNormal, thePatch.normal);
  var depthScalarProd = vectorScalarVector(this.screenNormal, thePatch.internalPoint);
  var depthRatio = (depthScalarProd - this.boundingSegmentZ[0]) / (this.boundingSegmentZ[1] - this.boundingSegmentZ[0]) - 0.5;
  //depthRatio is a number between -0.5 and 0.5 measuring how deep is the patch
  //relative to the center of the picture
  var colorFactor = 1 + depthRatio * this.colorDepthFactor;
  if (visibilityScalarProd >= 0) {
    theSurface.fillStyle = colorRGBToString(colorScale(thePatch.colorUV, colorFactor));
  } else {
    theSurface.fillStyle = colorRGBToString(colorScale(thePatch.colorVU, colorFactor));
  }
  theSurface.beginPath();
/*    if (0)
  { var
    theCoords = this.coordsMathToScreen(thePatch.base);
    theSurface.moveTo(Math.round(theCoords[0]), Math.round(theCoords[1]));
    theCoords = this.coordsMathToScreen(thePatch.v1);
    theSurface.lineTo(Math.round(theCoords[0]), Math.round(theCoords[1]));
    theCoords = this.coordsMathToScreen(thePatch.vEnd);
    theSurface.lineTo(Math.round(theCoords[0]), Math.round(theCoords[1]));
    theCoords = this.coordsMathToScreen(thePatch.v2);
    theSurface.lineTo(Math.round(theCoords[0]), Math.round(theCoords[1]));
    theCoords = this.coordsMathToScreen(thePatch.base);
    theSurface.lineTo(Math.round(theCoords[0]), Math.round(theCoords[1]));
    theSurface.fill();
    return;
  }*/
  var first = true;
  for (var i = 0; i < thePatch.adjacentContours.length; i ++) { 
    var currentContour = this.theIIIdObjects.theContours[thePatch.adjacentContours[i]];
    for (var j = 0; j<currentContour.thePoints.length; j ++) { 
      var theIndex = (thePatch.traversalOrder[i] === - 1) ? j : currentContour.thePoints.length-j- 1;
      var theCoords = this.coordsMathToScreen(currentContour.thePoints[theIndex]);
      if (this.flagRoundPatches) {
        vectorRound(theCoords);
      }
      if (first) {
        theSurface.moveTo(theCoords[0], theCoords[1]);
      } else {
        theSurface.lineTo(theCoords[0], theCoords[1]);
      }
      first = false;
    }
  }
  theSurface.closePath();
//    theSurface.clip();
  theSurface.fill();
  if (0) { 
    theCoords = this.coordsMathToScreen(thePatch.internalPoint);
    theSurface.fillStyle = "black";
    theSurface.font = "20pt sans-serif";
    theSurface.fillStyle = "cyan";
    //if (thePatch.index ===90)
    theSurface.fillText(thePatch.index, theCoords[0], theCoords[1]);
  }
//    theSurface.stroke();
}

Canvas.prototype.paintText = function(theText) { 
  var theSurface = this.surface;
  var isInForeGround = this.pointIsInForeGround(theText.location, []);
  theSurface.beginPath();
  theSurface.strokeStyle = theText.color;
  var theCoords = this.coordsMathToScreen(theText.location);
  theSurface.font = "15pt sans-serif";
  theSurface.lineWidth = 1;
  if (isInForeGround) { 
    theSurface.fillStyle = theText.color;
    theSurface.fillText(theText.text, theCoords[0], theCoords[1]);
  } else { 
    theSurface.strokeStyle = theText.color;
    theSurface.strokeText(theText.text, theCoords[0], theCoords[1]);
  }
}

Canvas.prototype.paintOnePoint = function(thePoint) { 
  var theSurface = this.surface;
  var isInForeGround = this.pointIsInForeGround(thePoint.location, []);
  theSurface.beginPath();
  theSurface.strokeStyle = colorRGBToString(thePoint.color);
  theSurface.fillStyle = colorRGBToString(thePoint.color );
  var theCoords = this.coordsMathToScreen(thePoint.location);
  theSurface.arc(theCoords[0], theCoords[1], 3, 0, Math.PI * 2);
  if (isInForeGround) {
    theSurface.fill();
  } else {
    theSurface.stroke();
  }
}

Canvas.prototype.getExtremePoint = function(indexToCompareBy, getLarger, pt1, pt2, pt3, pt4)
{ var result = pt1[indexToCompareBy];
  if (getLarger === 1) { 
    if (result < pt2[indexToCompareBy]) {
      result = pt2[indexToCompareBy];
    }
    if (result < pt3[indexToCompareBy]) {
      result = pt3[indexToCompareBy];
    }
    if (result < pt4[indexToCompareBy]) {
      result = pt4[indexToCompareBy];
    }
  } else { 
    if (result > pt2[indexToCompareBy]) {
      result = pt2[indexToCompareBy];
    }
    if (result > pt3[indexToCompareBy]) {
      result = pt3[indexToCompareBy];
    }
    if (result > pt4[indexToCompareBy]) {
      result = pt4[indexToCompareBy];
    }
  }
  return result;
}

Canvas.prototype.coordsMathScreenToBufferIndicesROWSFloat = function (input) { 
  return this.zBufferRowCount * (input - this.boundingBoxMathScreen[0][1]) / (this.boundingBoxMathScreen[1][1] - this.boundingBoxMathScreen[0][1]);
}

Canvas.prototype.coordsMathScreenToBufferIndicesROWS = function (input) { 
  var result = Math.floor(this.coordsMathScreenToBufferIndicesROWSFloat(input));
  if (result >= this.zBufferRowCount) {
    result --;
  }
  return result;
}

Canvas.prototype.coordsMathScreenToBufferIndicesCOLSFloat = function (input) { 
  return (this.zBufferColCount) * (input-this.boundingBoxMathScreen[0][0]) / (this.boundingBoxMathScreen[1][0] - this.boundingBoxMathScreen[0][0]);
}

Canvas.prototype.coordsMathScreenToBufferIndicesCOLS = function (input) { 
  var result = Math.floor(this.coordsMathScreenToBufferIndicesCOLSFloat(input));
  if (result >= this.zBufferColCount) {
    result --;
  }
  return result;
}

Canvas.prototype.coordsMathScreenToBufferIndices = function (input) { 
  var row = this.coordsMathScreenToBufferIndicesROWS(input[1]);
  var col = this.coordsMathScreenToBufferIndicesCOLS(input[0]);
  if (row < 0 || row >= this.zBufferRowCount || col < 0 || col >= this.zBufferColCount) {
    console.log("point with math-screen coords: " + input + " is out of the bounding box");
  }
  return [row, col];
}

Canvas.prototype.accountOnePointMathCoordsInBufferStrip = function(row, thePoint, patchIndex) { 
  if (row < 0 || row >= this.zBufferIndexStrip.length) {
    return;
  }
  var bufferCoords = this.coordsMathScreenToBufferIndices(this.coordsMathToMathScreen(thePoint));
  //If there were no rounding errors, row would be equal
  //to bufferCoords[0]. However since there will be rounding errors,
  //the row is passed instead as an argument.
  if (this.zBufferIndexStrip[row][0] === - 1) { 
    this.zBufferIndexStrip[row][1] = bufferCoords[1];
    this.zBufferIndexStrip[row][0] = bufferCoords[1];
    return;
  }
  if (bufferCoords[1] < this.zBufferIndexStrip[row][0]) {
    this.zBufferIndexStrip[row][0] = bufferCoords[1];
  }
  if (bufferCoords[1] > this.zBufferIndexStrip[row][1]) {
    this.zBufferIndexStrip[row][1] = bufferCoords[1];
  }
}

Canvas.prototype.accountEdgeInBufferStrip = function(base, edgeVector, patchIndex) { 
  var end = vectorPlusVector(base, edgeVector);
  var lowFloat = this.coordsMathScreenToBufferIndicesROWSFloat(this.coordsMathToMathScreen(base)[1]);
  var highFloat = this.coordsMathScreenToBufferIndicesROWSFloat(this.coordsMathToMathScreen(end)[1]);
  if (lowFloat > highFloat) { 
    var minusEdge = edgeVector.slice();
    vectorTimesScalar(minusEdge, - 1);
    this.accountEdgeInBufferStrip(end, minusEdge, patchIndex);
    return;
  }
  this.accountOnePointMathCoordsInBufferStrip(Math.floor(lowFloat), base, patchIndex);
  this.accountOnePointMathCoordsInBufferStrip(Math.floor(highFloat), end, patchIndex);
  if (lowFloat === highFloat) {
    return;
  }
  var currentPoint;
  for (var i = Math.ceil(lowFloat); i < highFloat; i ++) { 
    currentPoint = base.slice();
    vectorAddVectorTimesScalar(currentPoint, edgeVector, (i - lowFloat) / (highFloat - lowFloat));
    this.accountOnePointMathCoordsInBufferStrip(i, currentPoint, patchIndex);
    this.accountOnePointMathCoordsInBufferStrip(i - 1, currentPoint, patchIndex);
  }
}

Canvas.prototype.accountOnePatch = function(patchIndex) { 
  var thePatch = this.theIIIdObjects.thePatches[patchIndex];
  var pt1 = this.coordsMathToMathScreen(thePatch.base);
  var pt2 = this.coordsMathToMathScreen(thePatch.v1);
  var pt3 = this.coordsMathToMathScreen(thePatch.v2);
  var pt4 = this.coordsMathToMathScreen(thePatch.vEnd);
  var lowFloat = this.getExtremePoint(1, 0, pt1, pt2, pt3, pt4);
  var highFloat = this.getExtremePoint(1, 1, pt1, pt2, pt3, pt4);
  var low = this.coordsMathScreenToBufferIndicesROWS(lowFloat);
  var high = this.coordsMathScreenToBufferIndicesROWS(highFloat);
  for (var i = low; i <= high; i ++) { 
    this.zBufferIndexStrip[i][0] = - 1;
    this.zBufferIndexStrip[i][1] = - 1;
  }
  this.accountEdgeInBufferStrip(thePatch.base, thePatch.edge1, patchIndex);
  this.accountEdgeInBufferStrip(thePatch.base, thePatch.edge2, patchIndex);
  this.accountEdgeInBufferStrip(thePatch.v1, thePatch.edge2,   patchIndex);
  this.accountEdgeInBufferStrip(thePatch.v2, thePatch.edge1,   patchIndex);
  for (var i = low; i <= high; i ++) {
    for (var j = this.zBufferIndexStrip[i][0]; j<= this.zBufferIndexStrip[i][1]; j ++) { 
      if (i === - 1 || j === - 1) {
        continue;
      }
      this.zBuffer[i][j].push(patchIndex);
    }
  }
}

Canvas.prototype.computeBoundingBoxAccountPoint = function(input) { 
  var theV = this.coordsMathToMathScreen(input);
  for (var i = 0; i < 2; i ++) {
    if (theV[i] < this.boundingBoxMathScreen[0][i]) {
      this.boundingBoxMathScreen[0][i] = theV[i];
    }
  }
  for (i = 0; i < 2; i ++) {
    if (theV[i] > this.boundingBoxMathScreen[1][i]) {
      this.boundingBoxMathScreen[1][i] = theV[i];
    }
  }
  for (i = 0; i < 3; i ++) {
    if (input[i] < this.boundingBoxMath[0][i]) {
      this.boundingBoxMath[0][i] = input[i];
    }
  }
  for (i = 0; i < 3; i ++) {
    if (input[i] > this.boundingBoxMath[1][i]) {
      this.boundingBoxMath[1][i] = input[i];
    }
  }
  var theScalarProd = vectorScalarVector(this.screenNormal, input);
  if (theScalarProd < this.boundingSegmentZ[0]) {
    this.boundingSegmentZ[0] = theScalarProd;
  }
  if (theScalarProd > this.boundingSegmentZ[1]) {
    this.boundingSegmentZ[1] = theScalarProd;
  }
}

Canvas.prototype.computeBoundingBox = function() { 
  var thePatches = this.theIIIdObjects.thePatches;
  var theContours = this.theIIIdObjects.theContours;
  var thePoints = this.theIIIdObjects.thePoints;
  for (var i = 0; i < thePatches.length; i ++) { 
    this.computeBoundingBoxAccountPoint(thePatches[i].base);
    this.computeBoundingBoxAccountPoint(thePatches[i].v1);
    this.computeBoundingBoxAccountPoint(thePatches[i].v2);
    this.computeBoundingBoxAccountPoint(thePatches[i].vEnd);
  }
  for (i = 0; i < theContours.length; i ++) {
    for (var j = 0; j < theContours[i].thePoints.length; j ++) {
      this.computeBoundingBoxAccountPoint(theContours[i].thePoints[j]);
    }
  }
  for (i = 0; i < thePoints.length; i ++) {
    this.computeBoundingBoxAccountPoint(thePoints[i].location);
  }
}

Canvas.prototype.computeBuffers = function() { 
  var thePatches = this.theIIIdObjects.thePatches;
  for (var i = 0; i < this.zBuffer.length; i ++) {
    for (var j = 0; j < this.zBuffer[i].length; j ++) {
      this.zBuffer[i][j].length = 0;
    }
  }
  this.computeBoundingBox();
  this.bufferDeltaX = (this.boundingBoxMathScreen[1][0] - this.boundingBoxMathScreen[0][0]) / this.zBufferColCount;
  this.bufferDeltaY = (this.boundingBoxMathScreen[1][1] - this.boundingBoxMathScreen[0][1]) / this.zBufferRowCount;
  for (i = 0; i < thePatches.length; i ++) {
    this.accountOnePatch(i);
  }
  this.computePatchOrder();
}

Canvas.prototype.computePatchOrderOneContourPoint = function(thePatch, theContour, ptIndex) { 
  var thePointMathScreen = theContour.thePointsMathScreen[ptIndex];
  var thePoint = theContour.thePoints[ptIndex];
  var theIndices = this.coordsMathScreenToBufferIndices(thePointMathScreen);
  var currentBuffer = this.zBuffer[theIndices[0]][theIndices[1]];
  var thePatches = this.theIIIdObjects.thePatches;
  for (var i = 0; i <currentBuffer.length; i ++) { 
    if (thePatch.index === currentBuffer[i]) {
      continue;
    }
    if (
      thePatch.patchesAboveMe.indexOf(currentBuffer[i]) >= 0 ||
      thePatch.patchesBelowMe.indexOf(currentBuffer[i]) >= 0
    ) {
      continue;
    }
    var otherPatch = thePatches[currentBuffer[i]];
    var relativePosition = this.pointRelativeToPatch(thePoint, otherPatch);
    if (relativePosition === - 1) { 
      otherPatch.patchesBelowMe.push(thePatch.index);
      thePatch.patchesAboveMe.push(currentBuffer[i]);
    } else if (relativePosition === 1) { 
      otherPatch.patchesAboveMe.push(thePatch.index);
      thePatch.patchesBelowMe.push(currentBuffer[i]);
    }
  }
}

Canvas.prototype.computePatchPartialOrderOnePatch = function(thePatch) { 
  var theContours = this.theIIIdObjects.theContours;
  for (var i = 0; i < thePatch.adjacentContours.length; i ++) {
    for (var j = 0; j < theContours[thePatch.adjacentContours[i]].thePoints.length; j ++) {
      this.computePatchOrderOneContourPoint(thePatch, theContours[thePatch.adjacentContours[i]], j);
    }
  }
}

Canvas.prototype.computePatchPartialOrder = function() { 
  var thePatches = this.theIIIdObjects.thePatches;
  for (var i = 0; i < thePatches.length; i ++) { 
    thePatches[i].patchesBelowMe = [];
    thePatches[i].patchesAboveMe = [];
  }
  for (var i = 0; i < thePatches.length; i ++) {
    this.computePatchPartialOrderOnePatch(thePatches[i]);
  }
}

Canvas.prototype.computePatchOrder = function() { 
  this.computePatchPartialOrder();
  var thePatches = this.theIIIdObjects.thePatches;
  if (this.thePatchOrder.length!== thePatches.length) { 
    this.thePatchOrder = new Array(thePatches.length);
    this.patchIsAccounted = new Array(thePatches.length);
  }
  for (var i = 0; i < thePatches.length; i ++) { 
    this.thePatchOrder[i] = - 1;
    this.patchIsAccounted[i] = 0;
  }
  this.numAccountedPatches = 0;
  for (i = 0; i < thePatches.length; i ++) {
    if (thePatches[i].patchesBelowMe.length === 0) { 
      this.thePatchOrder[this.numAccountedPatches] = i;
      this.numAccountedPatches ++;
      this.patchIsAccounted[i] = 1;
    }
  }
  this.numCyclicallyOverlappingPatchTieBreaks = 0;
  while (this.numAccountedPatches < thePatches.length) { 
    var currentIndex = 0;
    while (currentIndex < this.numAccountedPatches) { 
      var currentPatch = thePatches[this.thePatchOrder[currentIndex]];
      for (i = 0; i < currentPatch.patchesAboveMe.length; i ++) { 
        var nextIndex = currentPatch.patchesAboveMe[i];
        if (this.patchIsAccounted[nextIndex] === 1) {
          continue;
        }
        var nextPatch = thePatches[nextIndex];
        var isGood = 1;
        for (var j = 0; j< nextPatch.patchesBelowMe.length; j ++) {
          if (this.patchIsAccounted[nextPatch.patchesBelowMe[j]] !== 1) { 
            isGood = 0;
            break;
          }
        }
        if (isGood === 1) { 
          this.thePatchOrder[this.numAccountedPatches] = nextIndex;
          this.numAccountedPatches ++;
          this.patchIsAccounted[nextIndex] = 1;
        }
      }
      currentIndex ++;
    }
    if (this.numAccountedPatches < thePatches.length) {
      this.patchOverlapTieBreak();
    }
  }
}

Canvas.prototype.patchOverlapTieBreak = function() { 
  //if we have cyclically overlapping patches we break ties
  //by selecting/painting first the patches whose internal point
  //has the highest (screen) depth.
  var thePatches = this.theIIIdObjects.thePatches;
  var deepestNonAccountedIndex = - 1;
  var minDepth = 0;
  for (var i = 0; i < this.patchIsAccounted.length; i ++) { 
    if (this.patchIsAccounted[i] === 1) {
      continue;
    }
    if (deepestNonAccountedIndex === - 1) { 
      deepestNonAccountedIndex = i;
      minDepth = vectorScalarVector(this.screenNormal, thePatches[i].internalPoint);
    }
    var currentDepth = vectorScalarVector(this.screenNormal, thePatches[i].internalPoint);
    if (currentDepth < minDepth) { 
      minDepth = currentDepth;
      deepestNonAccountedIndex = i;
    }
  }
  this.numCyclicallyOverlappingPatchTieBreaks ++;
  this.thePatchOrder[this.numAccountedPatches] = deepestNonAccountedIndex;
  this.numAccountedPatches ++;
  this.patchIsAccounted[deepestNonAccountedIndex] = 1;
}

Canvas.prototype.allocateZbuffer = function() { 
  if (this.zBufferRowCount < 1) {
    this.zBufferRowCount = 1;
  }
  if (this.zBufferColCount < 1) {
    this.zBufferColCount = 1;
  }
  if (this.zBuffer.length < this.zBufferRowCount) { 
    this.zBuffer = new Array(this.zBufferRowCount);
    this.zBufferIndexStrip = new Array(this.zBufferRowCount);
    for (var i = 0; i < this.zBufferRowCount; i ++) { 
      this.zBuffer[i] = new Array(this.zBufferColCount);
      for (var j = 0; j < this.zBufferColCount; j ++) {
        this.zBuffer[i][j] = [];
      }
      this.zBufferIndexStrip[i] = [- 1, - 1];
    }
  }
}

Canvas.prototype.getBufferBox = function(row, col) { 
  return [
    [this.boundingBoxMathScreen[0][0] + this.bufferDeltaX * col, this.boundingBoxMathScreen[0][1] + this.bufferDeltaY * row],
    [this.boundingBoxMathScreen[0][0] + this.bufferDeltaX * (col + 1), this.boundingBoxMathScreen[0][1] + this.bufferDeltaY * (row + 1)]
  ];
}

Canvas.prototype.paintZbuffer = function() { 
  var theSurface = this.surface;
  theSurface.strokeStyle = "gray";
  theSurface.fillStyle = "black";
  theSurface.setLineDash([]);
  for (var i = 0; i < this.zBuffer.length; i ++) {
    for (var j = 0; j < this.zBuffer[i].length; j ++) { 
      var bufferBox = this.getBufferBox(i, j);
      var bufferBoxLowLeft = this.coordsMathScreenToScreen(bufferBox[0]);
      var bufferBoxTopRight = this.coordsMathScreenToScreen(bufferBox[1]);
      theSurface.beginPath();
      theSurface.moveTo(bufferBoxLowLeft[0],  bufferBoxLowLeft [1]);
      theSurface.lineTo(bufferBoxTopRight[0], bufferBoxLowLeft [1]);
      theSurface.lineTo(bufferBoxTopRight[0], bufferBoxTopRight[1]);
      theSurface.lineTo(bufferBoxLowLeft[0],  bufferBoxTopRight[1]);
      theSurface.lineTo(bufferBoxLowLeft[0],  bufferBoxLowLeft [1]);
      theSurface.stroke();
      if (this.zBuffer[i][j].length !== 0) {
        theSurface.fill();
      }
    }
  }
}

Canvas.prototype.computeBasisFromNormal = function(inputNormal) { 
  if (inputNormal[0] !== 0) { 
    this.screenBasisUser[0] = [- inputNormal[1], inputNormal[0], 0];
    this.screenBasisUser[1] = [- inputNormal[2], 0, inputNormal[0]];
  } else if (inputNormal[1] !== 0) { 
    this.screenBasisUser[0] = [1, 0, 0];
    this.screenBasisUser[1] = [0, inputNormal[2], - inputNormal[1]];
  } else { 
    this.screenBasisUser[0] = [1, 0, 0];
    this.screenBasisUser[1] = [0, 1, 0];
  }
}

Canvas.prototype.infoProjectionCompute = function () {
  if (!this.flagShowPerformance) {
    return;
  }
  this.textProjectionInfo = "";
  this.textProjectionInfo += `<br>screen normal: ${this.screenNormal}`;
  this.textProjectionInfo += `<br>e1: ${this.screenBasisOrthonormal[0]}`;
  this.textProjectionInfo += `<br>e2: ${this.screenBasisOrthonormal[1]}`;
  this.textProjectionInfo += `<br>selected e1: ${this.selectedScreenBasis[0]}`;
  this.textProjectionInfo += `<br>selected e2: ${this.selectedScreenBasis[1]}`;
}

Canvas.prototype.computeBasis = function () { 
  //if (this.screenBasisOrthonormal.length<2)
  //  this.screenBasisOrthonormal.length =2;
  this.screenBasisOrthonormal[0] = this.screenBasisUser[0].slice();
  this.screenBasisOrthonormal[1] = this.screenBasisUser[1].slice();
  var e1 = this.screenBasisOrthonormal[0];
  var e2 = this.screenBasisOrthonormal[1];
  vectorNormalize(e1);
  vectorAddVectorTimesScalar(e2, e1, - vectorScalarVector(e1, e2));
  vectorNormalize(e2);
  this.screenNormal = vectorCrossVector(e1, e2);
}

Canvas.prototype.setBoundingBoxAsDefaultViewWindow = function() { 
  this.resetViewNoRedraw();
  this.computeBoundingBox();
  var leftLowScreen = this.coordsMathScreenToScreen(this.boundingBoxMathScreen[0]);
  var rightUpScreen = this.coordsMathScreenToScreen(this.boundingBoxMathScreen[1]);
  var desiredHeight = Math.abs(rightUpScreen[1] - leftLowScreen[1]) * 1.05;
  var desiredWidth = Math.abs(rightUpScreen[0] - leftLowScreen[0]) * 1.05;
  var candidateScaleHeight = this.scale * this.height / desiredHeight;
  var candidateScaleWidth = this.scale * this.width / desiredWidth;
  //console.log("leftLowScreen: "+ leftLowScreen +" rightUpScreen: "+rightUpScreen);
  //console.log(centerScreen);
  //console.log("desiredHeight: "+desiredHeight);
  //console.log("candidateScaleHeight: "+candidateScaleHeight);
  //console.log("candidateScaleWidth: "+candidateScaleWidth);
  //console.log("old scale: "+ this.scale);
  this.scale = Math.min(candidateScaleHeight, candidateScaleWidth);
  //console.log("new scale: "+ this.scale);
  var newViewWindowCenterMath = vectorPlusVector(
    this.boundingBoxMathScreen[0],
    this.boundingBoxMathScreen[1]
  );
  vectorTimesScalar(newViewWindowCenterMath, 0.5);
  var newCenterViewWindowScreenRescaled = this.coordsMathScreenToScreen(newViewWindowCenterMath);
  var oldCenterViewWindowMathScreenRescaled = this.coordsMathScreenToScreen(this.lastCenterScreen);
  this.centerX = this.centerX + oldCenterViewWindowMathScreenRescaled[0] - newCenterViewWindowScreenRescaled[0];
  this.centerY = this.centerY + oldCenterViewWindowMathScreenRescaled[1] - newCenterViewWindowScreenRescaled[1];
  this.defaultCenterX = this.centerX;
  this.defaultCenterY = this.centerY;
  this.scaleDefault = this.scale;
  this.lastCenterScreen = newViewWindowCenterMath.slice();
}

Canvas.prototype.resetView = function() { 
  this.resetViewNoRedraw();
  this.redraw();
}

Canvas.prototype.resetViewNoRedraw = function() { 
  this.boundingBoxMathScreen = [[- 0.01, - 0.01], [0.01, 0.01]];
  this.boundingBoxMath = [[- 0.01, - 0.01, - 0.01], [0.01, 0.01, 0.01]];
  this.boundingSegmentZ = [- 0.01, 0.01];
  this.screenBasisUser = this.screenBasisUserDefault.slice();
  this.centerX = this.defaultCenterX;
  this.centerY = this.defaultCenterY;
  this.scale = this.scaleDefault;
  this.computeBasis();
}

Canvas.prototype.constructControls = function() { 
  var incomingButton = "";
  incomingButton += `<button style ="border:none; background:none; color:blue; padding:0; text-decoration: underline; cursor:pointer"`;
  incomingButton += `onclick = "window.calculator.drawing.getCanvas('${this.canvasId}').resetView();">reset view</button>`;
  incomingButton += `<small> shift + drag to rotate</small>`;
  this.spanControls.innerHTML = incomingButton;
}

Canvas.prototype.init = function(inputCanvasId) { 
  this.canvasId = inputCanvasId;
  this.canvasContainer = document.getElementById(inputCanvasId);
  this.surface = this.canvasContainer.getContext("2d");
  this.canvasContainer.addEventListener("DOMMouseScroll", drawing.mouseWheel.bind(drawing), true);
  this.canvasContainer.addEventListener("mousewheel", drawing.mouseWheel.bind(drawing), true);
  this.spanMessages = document.getElementById(`${this.canvasId}Messages`);
  this.spanCriticalErrors = document.getElementById(`${this.canvasId}CriticalErrors`);
  this.spanControls = document.getElementById(`${this.canvasId}Controls`);
  this.theIIIdObjects.thePatches = [];
  this.theIIIdObjects.theContours = [];
  this.theIIIdObjects.thePoints = [];
  this.theIIIdObjects.theLabels = [];
  this.constructControls();
  this.computeBasis();
  if (this.zBuffer.length === 0) {
    this.allocateZbuffer();
  }
}

Canvas.prototype.coordsMathScreenToMath = function(theCoords) { 
  var output = this.screenBasisOrthonormal[0].slice();
  vectorTimesScalar(output, theCoords[0]);
  vectorAddVectorTimesScalar(output, this.screenBasisOrthonormal[1], theCoords[1]);
  return output;
}

Canvas.prototype.coordsMathToMathScreen = function(vector) { 
  return [
    vectorScalarVector(vector, this.screenBasisOrthonormal[0]),
    vectorScalarVector(vector, this.screenBasisOrthonormal[1])
  ];
}

Canvas.prototype.projectToMathScreen = function(vector) { 
  var output = this.screenBasisOrthonormal[0].slice();
  vectorTimesScalar(output, vectorScalarVector(vector, this.screenBasisOrthonormal[0]));
  vectorAddVectorTimesScalar(output, this.screenBasisOrthonormal[1], vectorScalarVector(vector, this.screenBasisOrthonormal[1]));
  return output;
}

Canvas.prototype.projectToSelectedMathScreen = function(vector) { 
  var output = this.selectedScreenBasis[0].slice();
  vectorTimesScalar(output, vectorScalarVector(vector, this.selectedScreenBasis[0]));
  vectorAddVectorTimesScalar(output, this.selectedScreenBasis[1], vectorScalarVector(vector, this.selectedScreenBasis[1]));
  return output;
}

Canvas.prototype.coordsMathToScreen = function(vector) { 
  return [
    this.scale * vectorScalarVector(vector, this.screenBasisOrthonormal[0]) + this.centerX,
    (- 1) * this.scale * vectorScalarVector(vector, this.screenBasisOrthonormal[1]) + this.centerY
  ];
}

Canvas.prototype.coordsMathToSelectedScreen = function(vector) { 
  return [
    this.scale * vectorScalarVector(vector, this.selectedScreenBasis[0]) + this.centerX,
    (- 1) * this.scale * vectorScalarVector(vector, this.selectedScreenBasis[1]) + this.centerY
  ];
}
Canvas.prototype.coordsScreenToMathScreen = function(screenPos) { 
  return [(screenPos[0] - this.centerX) / this.scale, (this.centerY - screenPos[1]) / this.scale];
}

Canvas.prototype.coordsScreenAbsoluteToScreen = function (cx, cy) { 
  return getPosXPosYObject(this.canvasContainer, cx, cy);
}

Canvas.prototype.coordsScreenAbsoluteToMathScreen = function(screenX, screenY) { 
  return this.coordsScreenToMathScreen(this.coordsScreenAbsoluteToScreen(screenX, screenY));
}

Canvas.prototype.coordsMathScreenToScreen = function(theCoords) { 
  return [this.scale * theCoords[0] + this.centerX, this.centerY - this.scale * theCoords[1]];
}

Canvas.prototype.rotateOutOfPlane = function (input, orthoBasis1, orthoBasis2, theAngle) { 
  var vComponent = input.slice();
  var scal1 = vectorScalarVector(orthoBasis1, input);
  var scal2 = vectorScalarVector(orthoBasis2, input);
  var projection = orthoBasis1.slice();
  vectorTimesScalar(projection, scal1);
  vectorAddVectorTimesScalar(projection, orthoBasis2, scal2);
  vectorAddVectorTimesScalar(vComponent, projection, - 1);
  projection = orthoBasis1.slice();
  vectorTimesScalar(projection, Math.cos(theAngle) * scal1 - Math.sin(theAngle) * scal2);
  vectorAddVectorTimesScalar(projection, orthoBasis2, Math.sin(theAngle) * scal1 + Math.sin(theAngle) * scal2);
  return vectorPlusVector(vComponent, projection);
}

//please use only with doRotateRadially = false
//the option creates confusing user interface, needs to be fixed/reworked
Canvas.prototype.rotateAfterCursorDefaultGreatNormalCircle = function(doRotateRadially) { 
  if (this.mousePosition.length == 0) {
    return;
  }
  if (doRotateRadially) {
    /* this.selectedPolarAngleChange = -getAngleChangeMathScreen(newX, newY, oldX, oldY);
    vectorTimesScalar(newE1, Math.cos(this.selectedPolarAngleChange));
    vectorAddVectorTimesScalar(newE1, oldE2, Math.sin(this.selectedPolarAngleChange));
    vectorTimesScalar(newE2, Math.cos(this.selectedPolarAngleChange));
    vectorAddVectorTimesScalar(newE2, oldE1, -Math.sin(this.selectedPolarAngleChange));
    this.screenBasisOrthonormal[0] = newE1;
    this.screenBasisOrthonormal[1] = newE2;*/
  }
  var oldX = this.clickedPosition[0];
  var oldY = this.clickedPosition[1];
  var newX = this.mousePosition[0];
  var newY = this.mousePosition[1];
  this.positionDelta = [newX - oldX, newY - oldY];
  this.unitRay = this.clickedPosition.slice();
  vectorNormalize(this.unitRay);
  this.rayComponent = this.unitRay.slice();
  vectorTimesScalar(this.rayComponent, vectorScalarVector(this.mousePosition, this.unitRay));
  var osculatingOldX = vectorLength(this.clickedPosition);
  var osculatingOldCos = osculatingOldX/vectorLength(this.selectedVector);
//    var osculatingOldSin =1-osculatingOldCos*osculatingOldCos;
  var osculatingNewX = vectorScalarVector(this.mousePosition, this.unitRay);
  var osculatingNewCos = osculatingNewX / vectorLength(this.selectedVector);
//    var osculatingNewSin =1-osculatingNewCos*osculatingNewCos;
//    var osculatingOldY =vectorScalarVector(this.selectedVector, selectedOrthogonal);
  this.newAngleNormal = Math.acos(osculatingNewCos);
  this.oldAngleNormal = Math.acos(osculatingOldCos);
  this.angleNormal = this.oldAngleNormal - this.newAngleNormal;
  if (isNaN(this.angleNormal)) {
    return;
  }
  var newE1 = this.selectedScreenBasis[0].slice();
  var newE2 = this.selectedScreenBasis[1].slice();
  newE1 = this.rotateOutOfPlane(newE1, this.selectedScreenProjectionNormalized, this.selectedScreenNormal, this.angleNormal);
  newE2 = this.rotateOutOfPlane(newE2, this.selectedScreenProjectionNormalized, this.selectedScreenNormal, this.angleNormal);
  this.screenBasisUser[0] = newE1.slice();
  this.screenBasisUser[1] = newE2.slice();
  this.computeBasis();
  this.redraw();
}

Canvas.prototype.rotateAfterCursorDefaultCartesian = function() { 
  if (this.mousePosition.length === 0) {
    return;
  }
  this.positionDelta = [this.mousePosition[0] - this.clickedPosition[0], this.mousePosition[1] - this.clickedPosition[1]];
  var xyThreshhold = 5;
  if (this.positionDelta[0] > xyThreshhold) {
    this.positionDelta[0] = xyThreshhold;
  }
  if (this.positionDelta[1] > xyThreshhold) {
    this.positionDelta[1] = xyThreshhold;
  }
  if (this.positionDelta[0] < - xyThreshhold) {
    this.positionDelta[0] = - xyThreshhold;
  }
  if (this.positionDelta[1] < - xyThreshhold) {
    this.positionDelta[1] = - xyThreshhold;
  }
  this.computeBasis();
  this.redraw();
}

Canvas.prototype.rotateAfterCursorDefault = function() { 
  if (this.mousePosition.length === 0) {
    return;
  }
  this.rotateAfterCursorDefaultGreatNormalCircle(false);
}

Canvas.prototype.mouseWheel = function(wheelDelta, screenX, screenY)
{ var screenPos = this.coordsScreenAbsoluteToScreen(screenX, screenY);
  var mathScreenPos = this.coordsScreenToMathScreen(screenPos);
  this.scale += wheelDelta;
  if (this.scale <= 0) {
    this.scale = 1;
  }
  var intermediateScreenPos = this.coordsMathScreenToScreen(mathScreenPos);
  //console.log("start screen: "+[screenX, screenY]);
  //console.log("intermed. screen: "+ intermediateScreenPos);
  this.centerX = this.centerX + screenPos[0] - intermediateScreenPos[0];
  this.centerY = this.centerY + screenPos[1] - intermediateScreenPos[1];
  this.redraw();
}

Canvas.prototype.mouseMove = function(screenX, screenY) { 
  this.screenXY[0] = screenX; 
  this.screenXY[1] = screenY;
  if (this.selectedElement == "") {
    return;
  }
  this.mousePosition = this.coordsScreenAbsoluteToMathScreen(this.screenXY[0], this.screenXY[1]);
  this.infoMouseCompute();
  //this.infoPatchesCompute();
  this.infoProjectionCompute();
  this.showMessages();
  if (this.selectedElement === "default") { 
    this.rotateAfterCursorDefault();
  }
  if (this.selectedElement === "origin") { 
    this.panAfterCursor();
  }
  this.redrawFinish = new Date().getTime();
  this.redrawTime = this.redrawFinish - this.redrawStart;
}

Canvas.prototype.panAfterCursor = function() { 
  var difference = vectorMinusVector(this.mousePosition, this.clickedPosition);
  this.centerX += difference[0] * this.scale;
  this.centerY -= difference[1] * this.scale;
  this.redraw();
}

Canvas.prototype.pointsWithinClickTolerance = function (leftXY, rightXY) { 
  var squaredDistance = (
    (leftXY[0] - rightXY[0]) * (leftXY[0] - rightXY[0]) + 
    (leftXY[1] - rightXY[1]) * (leftXY[1] - rightXY[1])
  ) * this.scale;
  return squaredDistance < 7;
}

Canvas.prototype.canvasClick = function (screenX, screenY, event) { 
  this.clickedPosition = this.coordsScreenAbsoluteToMathScreen(screenX, screenY);
  this.mousePosition = [];
  var mustSelectOrigin = true;
  if (event !== undefined) {
    if (event.shiftKey) {
      mustSelectOrigin = false;
    }
  }
  if (window.event !== undefined) {
    if (window.event.shiftKey) {
      mustSelectOrigin = false;
    }
  }
  if (this.pointsWithinClickTolerance(this.clickedPosition, [0, 0]) || mustSelectOrigin) {
    this.selectedElement = "origin";
  } else { 
    this.selectedElement = "default";
    this.computeSelectedVector();
  }
  this.infoMouseCompute();
  this.showMessages();
}

Canvas.prototype.selectEmpty = function() { 
  this.selectedElement = "";
  this.selectedScreenBasis = [];
  this.selectedVector = [];
  this.selectedPolarAngleChange = 0;
  this.angleNormal = 0;
}

Canvas.prototype.computeSelectedVector = function() { 
  this.selectedScreenProjectionNormalized = this.coordsMathScreenToMath(this.clickedPosition);
  this.selectedVector = this.selectedScreenProjectionNormalized.slice();
  vectorNormalize(this.selectedScreenProjectionNormalized);
  this.selectedScreenNormal = this.screenNormal;
//    vectorAddVectorTimesScalar(this.selectedVector, this.screenNormal, 0.1);
  var lengthSelectedVector = vectorScalarVector(this.selectedVector, this.selectedVector);
  if (lengthSelectedVector < 0.5) {
    vectorTimesScalar(this.selectedVector, 1 / Math.sqrt(lengthSelectedVector));
  }
  this.selectedScreenBasis = [this.screenBasisOrthonormal[0].slice(), this.screenBasisOrthonormal[1].slice()];
}

Canvas.prototype.showMessages = function() { 
  if (!this.flagShowPerformance) {
    return;
  }
  if (this.spanMessages === null || this.spanMessages === undefined) {
    return;
  }
  var theHTML = "";
  if (this.textPerformance !== "") {
    theHTML += this.textPerformance + "<hr>";
  }
  if (this.textErrors !== "") {
    theHTML += `<span style =\"red\"><b>${this.textErrors}</b></span><hr>`;
  }
  theHTML += `<span>${this.textMouseInfo}</span><hr><span>${this.textProjectionInfo}</span>`;
  if (this.textPatchInfo !== "") {
    theHTML += `<hr><span>${this.textPatchInfo}</span>`;
  }
  this.spanMessages.innerHTML = theHTML;
}

Canvas.prototype.infoPatchesCompute = function() { 
  this.textPatchInfo = "";
  this.textPatchInfo += "Z-depth: " + this.boundingSegmentZ + "<br>";
  var thePatches = this.theIIIdObjects.thePatches;
  for (var i = 0; i < thePatches.length; i ++) { 
    var currentPatch = thePatches[i];
    for (var j = 0; j < currentPatch.patchesAboveMe.length; j ++) { 
      this.textPatchInfo += currentPatch.patchesAboveMe[j];
      if (j !== currentPatch.patchesAboveMe.length - 1) {
        this.textPatchInfo += ", ";
      } else {
        this.textPatchInfo += "->";
      }
    }
    this.textPatchInfo += `<b>${i}</b>`;
    if (currentPatch.patchesBelowMe.length > 0) {
      this.textPatchInfo += "->";
    }
    for (j = 0; j < currentPatch.patchesBelowMe.length; j ++) { 
      this.textPatchInfo += currentPatch.patchesBelowMe[j];
      if (j !== currentPatch.patchesBelowMe.length) {
        this.textPatchInfo += ", ";
      }
    }
    this.textPatchInfo += "; contours: ";
    for (j = 0; j < currentPatch.adjacentContours.length; j ++) { 
      this.textPatchInfo += currentPatch.adjacentContours[j];
      if (j !== currentPatch.adjacentContours.length - 1) {
        this.textPatchInfo += ", ";
      }
    }
    if (i != thePatches.length - 1) {
      this.textPatchInfo += "<br>";
    }
  }
  this.textPatchInfo += "<style>#patchInfo{ border: 1px solid black;}</style>";
  this.textPatchInfo += "<table id =\"patchInfo\">";
  for (var i = this.zBuffer.length - 1; i >= 0; i --) { 
    this.textPatchInfo += "<tr id =\"patchInfo\">";
    for (var j = 0; j < this.zBuffer[i].length; j ++) { 
      this.textPatchInfo += "<td id =\"patchInfo\">";
      for (var k = 0; k < this.zBuffer[i][j].length; k ++) { 
        this.textPatchInfo += this.zBuffer[i][j][k];
        if (k !== this.zBuffer[i][j].length - 1) {
          this.textPatchInfo += ", ";
        }
      }
      this.textPatchInfo += "</td>";
    }
    this.textPatchInfo += "</tr>";
  }
  this.textPatchInfo += "</table>";
}

function vectorToString(vector) {
  var result = "[";
  for (var i = 0; i < vector.length; i ++) {
    result += vector[i].toFixed(2);
    if (i != vector.length - 1) {
      result += ", ";
    }
  }
  result += "]";
  return result;
}

Canvas.prototype.infoMouseCompute = function() {
  //console.log('DEBUG: show log performance set to true');
  if (!this.flagShowPerformance) {
    return;
  }
  this.textMouseInfo = "";
  var thePatches = this.theIIIdObjects.thePatches;
  if (this.numAccountedPatches < thePatches.length) {
    this.textMouseInfo += `<span style ='color:red'><b>Error: only ${this.numAccountedPatches} out of `;
    this.textMouseInfo += `${this.theIIIdObjects.thePatches.length} patches accounted. </b></span><br>`;
  }
  console.log("HEre be i ... mouse position?");
  this.textMouseInfo += `time last redraw: ${this.redrawTime} ms `; 
  this.textMouseInfo += `(~${(1000 / this.redrawTime).toFixed(1)} f.p.s.)`;
  this.textMouseInfo += `<br>selected element: ${this.selectedElement}`;
  this.textMouseInfo += `<br>mouse coordinates: ${vectorToString(this.screenXY)}`;
  this.textMouseInfo += `<br>2d coordinates: ${vectorToString(this.mousePosition)}`;
  this.textMouseInfo += `<br>clicked coordinates: ${this.clickedPosition}`;
  this.textMouseInfo += `<br>delta of position: ${this.positionDelta}`;
  this.textMouseInfo += `<br>ray component of mouse: ${this.rayComponent}`;
  this.textMouseInfo += `<br>selected vector: ${this.selectedVector}`;
  this.textMouseInfo += `<br>normal angle change: ${this.angleNormal.toFixed(3)}`;
  this.textMouseInfo += ` (${(this.angleNormal * 180 / Math.PI).toFixed(1)} deg)`;
  this.textMouseInfo += ` = ${this.oldAngleNormal.toFixed(3)}`;
  this.textMouseInfo += ` (${(this.oldAngleNormal * 180 / Math.PI).toFixed(1)} deg)`;
  this.textMouseInfo += `- ${this.newAngleNormal.toFixed(3)} `;
  this.textMouseInfo += `(${(this.newAngleNormal * 180 / Math.PI).toFixed(1)} deg)`;
}

Canvas.prototype.redraw = function() { 
  this.redrawStart = new Date().getTime();
  var theContours = this.theIIIdObjects.theContours;
  var thePatches = this.theIIIdObjects.thePatches;
  var thePoints = this.theIIIdObjects.thePoints;
  var theLabels = this.theIIIdObjects.theLabels;
  var theSurface = this.surface;
  theSurface.clearRect(0, 0, this.width, this.height);
  for (var i = 0; i < thePatches.length; i ++) {
    this.computePatch(thePatches[i]);
  }
  var computePatchesTime = new Date().getTime();
  for (i = 0; i < theContours.length; i ++) {
    this.computeContour(theContours[i]);
  }
  var computeContoursTime = new Date().getTime();
  this.computeBuffers();
  var computeBuffersTime = new Date().getTime();
  //this.paintZbuffer();
  var paintBuffersTime = new Date().getTime();
  var numPainted = 0;
  for (i = 0; i < this.thePatchOrder.length; i ++) {
    if (this.thePatchOrder[i] !== - 1) { 
      this.paintOnePatch(thePatches[this.thePatchOrder[i]]);
      numPainted ++;
    }
  }
  for (i = 0; i < this.patchIsAccounted.length; i ++) {
    if (this.patchIsAccounted[i] === 0) { 
      this.paintOnePatch(thePatches[i]);
      numPainted ++;
    }
  }
  var paintPatchTime = new Date().getTime();
  this.numContourPoints = 0;
  this.numContourPaths = 0;
  for (var i = 0; i < theContours.length; i ++) {
    this.paintOneContour(theContours[i]);
  }
  var paintContourTime = new Date().getTime();
  for (var i = 0; i < thePoints.length; i ++) {
    this.paintOnePoint(thePoints[i]);
  }
  for (var i = 0; i < theLabels.length; i ++) {
    this.paintText(theLabels[i]);
  }
  this.paintMouseInfo();
  var redrawTime = new Date().getTime();
  if (this.flagShowPerformance) {
    this.textPerformance = "";

    this.textPerformance += `Redraw time (ms): ${(redrawTime - this.redrawStart)}=<br>`;
    this.textPerformance += `${computePatchesTime - this.redrawStart} (compute patches) + `;
    this.textPerformance += (computeContoursTime - computePatchesTime) + " (compute contours) + ";
    this.textPerformance += (computeBuffersTime - computeContoursTime) + " (compute buffers) + ";
    this.textPerformance += (paintBuffersTime - computeBuffersTime) + "<br>(paint buffers) + ";
    this.textPerformance += (paintPatchTime - paintBuffersTime) + "<br> (paint " + numPainted + " patche(s)) + <br>";
    this.textPerformance += (paintContourTime - paintPatchTime) + " (paint " + theContours.length + " contour(s) with ";
    this.textPerformance += this.numContourPoints + " points in " + this.numContourPaths  + " sub-paths) + " ;
    this.textPerformance += (redrawTime-paintContourTime) + " (paint all else). ";
  }
  this.showMessages();
}

Canvas.prototype.drawSurface = function(theSurface) { 
  var numUsegments = theSurface.patchDimensions[0];
  var numVsegments = theSurface.patchDimensions[1];
  var thePatches = this.theIIIdObjects.thePatches;
  var theContours = this.theIIIdObjects.theContours;
  //var incomingPatches = new Array(numUsegments);
  var deltaU = theSurface.deltaU;
  var deltaV = theSurface.deltaV;
  var firstPatchIndex = thePatches.length;
  for (var i = 0; i < numUsegments; i ++) { 
    //incomingPatches[i] = new Array(numVsegments);
    for (var j = 0; j<numVsegments; j ++) { 
      //var incomingPatch = incomingPatches[i][j];
      var currentU = theSurface.uvBox[0][0] + i * deltaU;
      var currentV = theSurface.uvBox[0][1] + j * deltaV;
      var base = theSurface.xyzFun(currentU, currentV);
      var v1= theSurface.xyzFun(currentU + deltaU, currentV);
      var v2= theSurface.xyzFun(currentU, currentV + deltaV);
      var edge1=vectorMinusVector(v1, base);
      var edge2=vectorMinusVector(v2, base);
      var incomingPatch = new Patch(base, edge1, edge2, theSurface.colors.colorUV, theSurface.colors.colorVU);
      incomingPatch.adjacentContours = new Array(4);
      incomingPatch.traversalOrder = [1, 1, 1, 1];
      incomingPatch.index = thePatches.length;
      incomingPatch.internalPoint = theSurface.xyzFun(currentU + deltaU / 2, currentV + deltaV / 2);
      thePatches.push(incomingPatch);
    }
  }
  var numSegmentsPerContour = this.defaultNumSegmentsPerContour;
  var contourPoints = new Array(numSegmentsPerContour + 1);
  for (i = 0; i < numUsegments + 1; i ++) {
    for (j = 0; j < numVsegments; j ++) { 
      currentU = theSurface.uvBox[0][0] + i * deltaU;
      for (var k = 0; k < numSegmentsPerContour + 1; k ++) { 
        currentV = theSurface.uvBox[0][1] + (j + k / numSegmentsPerContour) * deltaV;
        contourPoints[k] = theSurface.xyzFun(currentU,currentV);
      }
      var incomingContour = new Contour(contourPoints, theSurface.colors.colorContour, theSurface.contourWidth);
      incomingContour.index = theContours.length;
      if (i > 0) { 
        incomingContour.adjacentPatches.push(firstPatchIndex + numVsegments * (i - 1) + j);
        thePatches[firstPatchIndex + numVsegments * (i - 1) + j].adjacentContours[2] = theContours.length;
        thePatches[firstPatchIndex + numVsegments * (i - 1) + j].traversalOrder[2] = - 1;
      }
      if (i < numUsegments) { 
        incomingContour.adjacentPatches.push(firstPatchIndex + numVsegments * i + j);
        thePatches[firstPatchIndex + numVsegments * i + j].adjacentContours[0] = theContours.length;
      }
      theContours.push(incomingContour);
    }
  }
  for (i = 0; i < numUsegments; i ++) {
    for (j = 0; j < numVsegments + 1; j ++) { 
      currentV = theSurface.uvBox[0][1] + j * deltaV;
      for (k = 0; k < numSegmentsPerContour + 1; k ++) { 
        currentU = theSurface.uvBox[0][0] + (i + k / numSegmentsPerContour) * deltaU;
        contourPoints[k] = theSurface.xyzFun(currentU, currentV);
      }
      incomingContour = new Contour(contourPoints, theSurface.colors.colorContour, theSurface.contourWidth);
      incomingContour.index = theContours.length;
      if (j > 0) { 
        incomingContour.adjacentPatches.push(firstPatchIndex + numVsegments * i + j - 1);
        thePatches[firstPatchIndex + numVsegments * i + j - 1].adjacentContours[1] = theContours.length;
      }
      if (j < numVsegments) { 
        incomingContour.adjacentPatches.push(firstPatchIndex + numVsegments * i + j);
        thePatches[firstPatchIndex + numVsegments * i + j].adjacentContours[3] = theContours.length;
        thePatches[firstPatchIndex + numVsegments * i + j].traversalOrder[3] = - 1;
      }
      theContours.push(incomingContour);
    }
  }
}

Drawing.prototype.getCanvas = function(/**@type {string}}*/ canvasId) { 
  var theCanvas = null;
  if (typeof canvasId === "string") {
    theCanvas = document.getElementById(canvasId);
  } else {
    theCanvas = canvasId;
    canvasId = theCanvas.id;
  }
  if (this.canvases[canvasId] === undefined) { 
    this.canvases[canvasId] = new Canvas(theCanvas);
    if (this.firstCanvas == null) {
      this.firstCanvas = this.canvases[canvasId];
    }
  }
  return this.canvases[canvasId];
}

Drawing.prototype.getCanvasTwoD = function(canvasId) { 
  var theCanvas = null;
  if (typeof canvasId === "string") {
    theCanvas = document.getElementById(canvasId);
  } else {
    theCanvas = canvasId;
    canvasId = theCanvas.id;
  }
  if (this.canvases[canvasId] === undefined) { 
    this.canvases[canvasId] = new CanvasTwoD(theCanvas);
    if (this.firstCanvas == null) {
      this.firstCanvas = this.canvases[canvasId];
    }
  }
  return this.canvases[canvasId];
}

Drawing.prototype.testPicture =  function(inputCanvas) { 
  var theCanvas = this.getCanvas(document.getElementById(inputCanvas));
  theCanvas.screenBasisUserDefault = [[0.59, 0.78, 0.18], [0.46, - 0.15, - 0.87]];
  theCanvas.screenBasisUser = theCanvas.screenBasisUserDefault.slice();
  theCanvas.init(inputCanvas, false);
  theCanvas.drawLine([- 1, 0, 0], [1, 0, 0], 'black', 2);
  theCanvas.drawLine([0, - 1, 0], [0, 1, 0], 'black', 2);
  theCanvas.drawLine([0, 0, - 1], [0, 0, 1], 'black', 2);
  theCanvas.drawLine([0, 0, 0], [1, 0.5, 0.5], 'red', 2);
  theCanvas.scale = 100;
  theCanvas.drawSurface(testGetMoebiusSurface());
  theCanvas.drawSurface(testGetMoebiusSurface2());
  theCanvas.drawPoints([[1, 0, 0]], 'red');
  theCanvas.drawPoints([[0, 1, 0]], 'green');
  theCanvas.drawPoints([[0, 0, 1]], 'blue');
  theCanvas.drawText({location:[1, 0, 0], text: "x", color: "green"});
  theCanvas.drawText({location:[0, 1, 0], text: "y", color: "green"});
  theCanvas.drawText({location:[0, 0, 1], text: "z", color: "green"});
  theCanvas.setBoundingBoxAsDefaultViewWindow();
  //console.log(theCanvas.theIIIdObjects.thePatches);
  theCanvas.redraw();
}

Drawing.prototype.testPictureTwoD = function(inputCanvas1, inputCanvas2, inputCanvas3) { 
  var theCanvas = this.getCanvasTwoD(document.getElementById(inputCanvas1));
  theCanvas.init(inputCanvas1);
  theCanvas.drawLine([- 10, 0], [19, 0], 'green');
  theCanvas.drawLine([0, - 1], [0, 1], 'purple');
  theCanvas.drawText([- 1,- 1], '(- 1,- 1)', 'orange');
  theCanvas.drawFunction(testFunctionPlot, - 10, 10, 100, 'red', 4);
  theCanvas.setViewWindow([- 10, - 1], [19, 1]);
  theCanvas.plotFillStart('orange');
  theCanvas.drawLine([0,- 5],[1, - 4], 'green');
  theCanvas.drawLine([1,- 4],[2, - 5], 'black');
  theCanvas.drawLine([2,- 5],[0, - 5], 'black');
  theCanvas.plotFillFinish();
  theCanvas.plotFillStart('pink');
  theCanvas.drawCurve([testFunctionPlot, testFunctionPlot2], - 4, 4, 300, 'blue', 1);
  theCanvas.plotFillFinish();
  theCanvas.drawVectorField(testVectorField2d, true, [- 6,- 6], [6, 6], [20, 20], 0.5, "red",2);
  theCanvas.redraw();
  var theCanvas2 = this.getCanvasTwoD(document.getElementById(inputCanvas2));
  theCanvas2.init(inputCanvas2);
  theCanvas2.drawLine([- 10, - 1], [10, 1], 'green');
  theCanvas2.drawLine([0, - 19], [0, 1], 'purple');
  theCanvas2.drawText([- 1,- 1], '(- 1,- 1)', 'orange');
  theCanvas2.drawPath([[2, 2], [3, 3], [1, 4]], 'cyan');
  theCanvas2.drawPathFilled([[- 2, - 2], [- 7, - 3],[- 1, - 4], [- 2, - 2]], 'red', 'green');
  theCanvas2.plotFillStart('pink');
  theCanvas2.drawFunction(testFunctionPlot, - 10,10, 100, 'red', 2);
  theCanvas2.drawLine([10, 0], [- 10,0], 'black');
  theCanvas2.plotFillFinish();
  theCanvas2.drawFunction(testFunctionPlot, - 10, 10, 100, 'red', 0.5);
  theCanvas2.setViewWindow([- 1, - 19], [1, 5]);
  theCanvas2.drawVectorField(testVectorField2d, false, [- 6, - 6], [6, 6], [20, 20], 0.5, "red", 2);
  theCanvas2.redraw();
  var theCanvas3 = this.getCanvasTwoD(document.getElementById(inputCanvas3));
  theCanvas3.init(inputCanvas3);
  theCanvas3.drawFunction(testFunctionPlot, - 10, 10, 100, 'red', 2);
  theCanvas3.drawGrid();
  theCanvas3.drawCoordinateAxes();
  theCanvas3.setViewWindow([- 1,- 19], [1, 5]);
  theCanvas3.drawFunction(testFunctionPlot2, "minusInfinity", "infinity", 100, 'red',4);
  theCanvas3.redraw();
}


Drawing.prototype.mouseMoveRedraw = function(inputCanvas, x, y) { 
  var theCanvas = this.getCanvas(inputCanvas);
  if (theCanvas !== null && theCanvas !== undefined) {   
    theCanvas.mouseMove(x, y);
  }
}

Drawing.prototype.mouseWheel = function(theEvent) { 
  if (theEvent === undefined) {
    theEvent = window.event;
  }
  if (theEvent.target === undefined) {
    return;
  }
  if (theEvent.preventDefault !== undefined) {
    theEvent.preventDefault();
  }
  if (theEvent.stopPropagation !== undefined) {
    theEvent.stopPropagation();
  }
  var theWheelDelta = theEvent.detail ? theEvent.detail * - 1 : theEvent.wheelDelta / 40;
  var theCanvas = this.canvases[theEvent.target.id];
  if (theCanvas === undefined || theCanvas === null) {
    return;
  }
  var theIncrement = 0.6;
  theCanvas.mouseWheel(theWheelDelta * theIncrement, theEvent.clientX, theEvent.clientY);
}

Drawing.prototype.mouseUp = function(inputCanvas) { 
  var theCanvas = this.getCanvas(inputCanvas);
  theCanvas.selectEmpty();
  theCanvas.redraw();
}

Drawing.prototype.canvasClick = function(theCanvasContainer, theEvent) { 
  this.canvases[theCanvasContainer.id].canvasClick(theEvent.clientX, theEvent.clientY, theEvent);
}

var drawing = new Drawing();
if (window.calculator === undefined) {
  window.calculator = {};
}
window.calculator.drawing = drawing;

module.exports = {
  drawing, 
};