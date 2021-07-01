"use strict";
/**
 * File needs to function as stand-alone javascript
 * as well be used as a commonJS module included via
 * require('graphics_n_dimension.js')).
 * Please don't require(...) any modules from this file.
 **/

var collectionGraphicsNDimensions = {};

function startProjectionPlaneUser(canvasId) {
  collectionGraphicsNDimensions[canvasId].startProjectionPlaneUser();
}

function toStringVector(vector) {
  var result = "";
  result += "(";
  for (var i = 0; i < vector.length; i++) {
    result += vector[i].toFixed(2);
    if (i !== vector.length - 1) {
      result += ", ";
    }
  }
  result += ")";
  return result;
}

function getAngleFromXandY(x, y) {
  return Math.atan2(y, x);
}

function getAngleScreenChange(newX, newY, oldX, oldY) {
  var result = getAngleFromXandY(newX, newY) - getAngleFromXandY(oldX, oldY);
  var topBound = Math.PI;
  var bottomBound = - Math.PI;
  while (result > topBound || result <= bottomBound) {
    if (result > topBound) {
      result -= 2 * Math.PI;
    } else {
      result += 2 * Math.PI;
    }
  }
  return result;
}

class GraphicsNDimensions {
  constructor(inputIdCanvas, inputIdInfo, inputIdHighlightInfo, PanelExpandable) {
    this.PanelExpandable = PanelExpandable;
    if (this.PanelExpandable === undefined) {
      this.PanelExpandable = null;
    }
    this.vectors = [];
    this.vectorProjections = [];
    this.drawOperations = [];
    this.idCanvas = inputIdCanvas;
    this.idsBasis = [];
    this.idInfo = inputIdInfo;
    this.idPlaneInfo = `${inputIdInfo}projectionPlane`;
    this.panelInfo = null;
    if (inputIdHighlightInfo !== null && inputIdHighlightInfo !== undefined && inputIdHighlightInfo !== "") {
      this.idHighlightInfo = inputIdHighlightInfo;
    } else {
      this.idHighlightInfo = null;
    }
    this.highlightInfoContent = [];
    this.basisCircles = [];
    this.projectionsBasisCircles = [];
    this.eiBasis = [];
    this.projectionsEiVectors = [];
    this.indicesHighlightOperations = [];
    this.projectionsLabeledVectors = [];
    this.projectionScreenBasis = [];
    this.projectionSelectedAtSelection = [];
    this.screenBasis = [];
    this.screenBasisAtSelection = [];
    this.canvasContainer = document.getElementById(this.idCanvas);
    this.dimension = 0;
    this.theBilinearForm = [];
    this.graphicsUnit = 0;
    this.selectedBasisIndex = - 1;
    this.selectedHighlightIndex = - 1;
    /**
     * @type {Object<string, Object<string, boolean> >}
     * Keys are indices of groups currently highlighted.
     * Values are arrays with indices
     * within each group over which the mouse is located. */

    this.currentHighlightIndices = {};
    this.flagAllowMovingCoordinateSystemFromArbitraryClick = true;

    this.vOrthogonalSelected = [];
    this.vProjectionNormalizedSelected = [];

    this.angleScreenChange = 0;
    this.angleCuttingChange = 0;

    this.canvasContainer.addEventListener("DOMMouseScroll", this.mouseHandleWheel.bind(this));
    this.canvasContainer.addEventListener("wheel", this.mouseHandleWheel.bind(this));
    this.canvasContainer.addEventListener("mousedown", this.clickCanvas.bind(this));
    this.canvasContainer.addEventListener("mouseup", this.releaseMouse.bind(this));
    this.canvasContainer.addEventListener("mousemove", this.mouseMoveRedraw.bind(this));
    this.animationBasisChange = {
      frameCount: 0,
      numberOfFrames: 20,
      screenGoal: [],
      screenStart: [],
      frameStarted: false
    };
    this.animation = {
      currentFrameIndex: - 1,
      numberOfFrames: - 1,
      timeoutHandle: null,
      frameLength: 300
    };
    this.canvas = null;
    this.widthHTML = this.canvasContainer.width;
    this.heightHTML = this.canvasContainer.height;
    this.shiftScreenCoordinates = [this.widthHTML / 2, this.heightHTML / 2];
    this.mousePositionScreen = [0, 0];
    this.mousePositionScreenClicked = [0, 0];
    this.shiftScreenCoordinatesClicked = [0, 0];

    collectionGraphicsNDimensions[this.idCanvas] = this;
    this.clickTolerance = 5;
    this.textShift = [- 3, - 4];
  }

  initVectors(inputVectors) {
    this.vectors = inputVectors;
    this.vectorProjections = [];
    for (var counterVector = 0; counterVector < this.vectors.length; counterVector++) {
      this.vectorProjections.push([0, 0]);
    }
  }

  /** @returns {string} */
  getInfoHTML() {
    var result = "";
    result += `<br>Mouse position: ${toStringVector(this.mousePositionScreen)}`;
    result += `<br>Selected index: ${this.selectedBasisIndex}`;
    if (this.selectedBasisIndex >= 0) {
      result += `: ${toStringVector(this.basisCircles[this.selectedBasisIndex])}`;
      result += `<br>Screen basis at selection:`;
      result += `<br>${toStringVector(this.screenBasisAtSelection[0])}`;
      result += `<br>${toStringVector(this.screenBasisAtSelection[1])}`;
      result += `<br>Angle change screen: ${this.angleScreenChange.toFixed(2)}`;
      result += `<br>Angle change cutting: ${this.angleCuttingChange.toFixed(2)}`;
      result += `<br>Projection selected: ${toStringVector(this.vProjectionNormalizedSelected)}`;
      result += `<br>Orthogonal component selected: ${toStringVector(this.vOrthogonalSelected)}`;
    }
    result += `<br>Coordinate center in screen coordinates: <br>${toStringVector(this.shiftScreenCoordinates)}`;
    result += "<br>The projection plane (drawn on the screen) is spanned by the following two vectors.\n";
    result += `<br>${toStringVector(this.screenBasis[0])}`;
    result += `<br>${toStringVector(this.screenBasis[1])}`;
    for (var i = 0; i < this.basisCircles.length; i++) {
      result += `<br>${i}: ${toStringVector(this.basisCircles[i])}: ${toStringVector(this.projectionsBasisCircles[i])}`;
    }
    return result;
  }

  writeInfo() {
    let info = this.getInfoHTML();
    let panelInfoElement = document.getElementById(this.idPlaneInfo);
    if (panelInfoElement === null) {
      return;
    }
    panelInfoElement.innerHTML = info;
  }

  initInfo() {
    var infoHTML = "";
    infoHTML += "<div style = 'height:300px; overflow-y: scroll;'>";
    for (var i = 0; i < 2; i++) {
      this.idsBasis[i] = [];
      for (var j = 0; j < this.dimension; j++) {
        this.idsBasis[i][j] = `${this.idCanvas}textEbasis_${i}_${j}`;
        infoHTML += `<textarea rows = "1" cols = "2" id = "${this.idsBasis[i][j]}"></textarea>`;
      }
      infoHTML += "<br>";
    }
    infoHTML += `<button onclick = "window.calculator.graphicsNDimensions.startProjectionPlaneUser('${this.idCanvas}')">`;
    infoHTML += `Change to basis</button><br>`;
    //infoHTML += `<button onclick = "snapShotLaTeX('${this.idCanvas}')>LaTeX snapshot</button>`;
    infoHTML += `<span id = '${this.idCanvas}snapShotLateXspan'></span>\n`;
    infoHTML += `<div id = '${this.idPlaneInfo}' class = 'panel'></div>`;
    infoHTML += `</div>`;
    if (this.PanelExpandable === null) {
      document.getElementById(this.idInfo).innerHTML = infoHTML;
    } else {
      this.panelInfo = new this.PanelExpandable(this.idInfo);
      this.panelInfo.initialize(true);
      this.panelInfo.setPanelContent(infoHTML);
    }
  }

  snapShotLaTeX() {
    var textComponent = document.getElementById(`${this.idCanvas}snapShotLateXspan`);
    var result = "";
    result += `\\documentclass{article} \\usepackage{auto-pst-pdf}<br>`;
    result += `\n%\\usepackage{pst-plot}<br>\n\\begin{document}<br>\n`;
    result += `\\psset{xunit = 0.01cm, yunit = 0.01cm} <br>\n\\begin{pspicture}(0,0)(1,1)\n`;
    ComputeProjections();
    for (var counterDrawOperation = 0; counterDrawOperation < this.drawOperations.length; counterDrawOperation++) {
      result += this.drawOperations[counterDrawOperation].getLaTeXOperation();
    }
    result += "\\end{pspicture}<br>";
    if (nonImplementedFound) {
      result += `Not all elements in the html picture were drawn in LaTex. To do: fix this.`;
    }
    result += "\\end{document}";
    textComponent.innerHTML = result;
  }

  drawStandardEiBasis(color) {
    for (var i = 0; i < this.dimension; i++) {
      var eiVector = new Array(this.dimension);
      eiVector.fill(0);
      eiVector[i] = 1;
      this.drawCircle(eiVector, color, 3);
    }
  }

  drawCircle(position, color, radius, frameId, frameIndex) {
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
      radius: radius,
      frameId: frameId,
      frameIndex: frameIndex
    }));
  }

  drawLine(left, right, color) {
    if (color === undefined) {
      color = "black";
    }
    this.drawOperations.push(new DrawSegmentBetweenTwoVectors(
      this.idCanvas, {
      left: left,
      right: right,
      lineWidth: 1,
      colorLine: color
    }
    ));
  }

  drawPath(points, color, lineWidth, frameId, frameIndex) {
    if (color === undefined) {
      color = "black";
    }
    this.drawOperations.push(new DrawPath(
      this.idCanvas, {
      points: points,
      lineWidth: lineWidth,
      colorLine: color,
      frameId: frameId,
      frameIndex: frameIndex
    }
    ));
  }

  drawHighlightGroup(points, labels, color, radius) {
    if (color === undefined) {
      color = "black";
    }
    if (radius === undefined) {
      radius = 3;
    }
    this.drawOperations.push(new DrawHighlights(
      this.idCanvas, {
      vectors: points,
      indexInOperations: this.drawOperations.length,
      labels: labels,
      color: color,
      radius: radius
    }
    ));
  }

  drawText(location, text, color) {
    if (color === undefined) {
      color = "black";
    }
    this.drawOperations.push(new DrawTextAtVector(
      this.idCanvas, {
      location: location,
      text: text,
      colorFill: color
    }
    ));
  }

  getBilinearFormInput() {
    var result = "";
    result += "The bilinear form of the vector space follows. The ij^th element ";
    result += "gives the scalar product of e_i and e_j. If you enter a degenerate or non-positive definite ";
    result += "symmetric bilinear form the javascript might crash. You are expected to enter ";
    result += "a symmetric strictly positive definite matrix. <br> \n";
    for (var counterRow = 0; counterRow < this.theBilinearForm.length; counterRow++) {
      for (var counterColumn = 0; counterColumn < this.theBilinearForm[counterRow].NumCols; counterColumn++) {
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
    result += "Zooming is tested to work on Firefox and google Chrome browsers on Ubuntu. ";
    result += `<br>${this.drawOperations.length} elements drawn. `;
    var textComponent = document.getElementById(`${this.idCanvas}snapShotLateXspan`);
    textComponent.innerHTML = result;
  }

  startProjectionPlaneUser() {
    this.animationBasisChange.frameCount = 0;
    this.animationBasisChange.screenStart[0] = this.screenBasis[0].slice();
    this.animationBasisChange.screenStart[1] = this.screenBasis[1].slice();
    for (var i = 0; i < 2; i++) {
      this.animationBasisChange.screenGoal[i] = [];
      for (var counterDimension = 0; counterDimension < this.dimension; counterDimension++) {
        var coordinate = document.getElementById(this.idsBasis[i][counterDimension]).value;
        this.animationBasisChange.screenGoal[i][counterDimension] = Number(coordinate);
      }
    }
    this.animateChangeProjectionPlaneUser();
  }

  registerFrameIndex(frameIndex) {
    if (typeof frameIndex !== "number") {
      return;
    }
    if (this.animation.numberOfFrames < frameIndex) {
      this.animation.numberOfFrames = frameIndex;
    }
  }

  animateChangeProjectionPlaneUser() {
    this.animationBasisChange.frameCount++;
    var frameCount = this.animationBasisChange.frameCount;
    var maxFrameCount = this.animationBasisChange.numberOfFrames;
    if (frameCount > maxFrameCount) {
      return;
    }
    var screenStart = this.animationBasisChange.screenStart;
    var screenGoal = this.animationBasisChange.screenGoal;
    for (var i = 0; i < this.dimension; i++) {
      this.screenBasis[0][i] = screenGoal[0][i] * (frameCount / maxFrameCount) + screenStart[0][i] * (1 - frameCount / maxFrameCount);
      this.screenBasis[1][i] = screenGoal[1][i] * (frameCount / maxFrameCount) + screenStart[1][i] * (1 - frameCount / maxFrameCount);
    }
    this.makeScreenBasisOrthonormal();
    this.drawAll();
    setTimeout(this.animateChangeProjectionPlaneUser.bind(this), 100);
  }


  drawAll() {
    if (this.animationBasisChange.frameStarted) {
      return;
    }
    this.animationBasisChange.frameStarted = true;
    if (this.canvas == null || this.canvas == undefined) {
      this.init();
    }
    this.computeProjectionsEiBasis();
    this.computeProjectionsSpecialVectors();
    this.canvas.clearRect(0, 0, this.widthHTML, this.heightHTML);
    this.writeInfo();
    this.highlightInfoContent = [];
    for (var counterOperation = 0; counterOperation < this.drawOperations.length; counterOperation++) {
      var currentOperation = this.drawOperations[counterOperation];
      if (currentOperation.frameId !== undefined && currentOperation.frameId !== null) {
        if (currentOperation.frameIndex !== this.animation.currentFrameIndex) {
          continue;
        }
      }
      currentOperation.drawNoFinish();
    }
    if (this.highlightInfoContent.length > 0) {
      if (this.idHighlightInfo !== null) {
        var highlightComponent = document.getElementById(this.idHighlightInfo);
        highlightComponent.innerHTML = this.highlightInfoContent.join("<br>");
      }
    }
    this.animationBasisChange.frameStarted = false;
  }

  makeStandardBilinearForm() {
    this.theBilinearForm = new Array(this.dimension);
    for (var i = 0; i < this.theBilinearForm.length; i++) {
      this.theBilinearForm[i] = newArray(this.dimension);
      this.theBilinearForm[i].fill(0, 0, this.dimension);
      this.theBilinearForm[i][i] = 1;
    }
  }

  makeEiBasis() {
    this.eiBasis = new Array(this.dimension);
    for (var i = 0; i < this.eiBasis.length; i++) {
      this.eiBasis[i] = new Array(this.dimension);
      this.eiBasis[i].fill(0, 0, this.dimension);
      this.eiBasis[i][i] = 1;
    }
  }

  makeBasisCircles() {
    this.basisCircles = new Array(this.dimension);
    this.projectionsBasisCircles = new Array(this.dimension);
    for (var i = 0; i < this.basisCircles.length; i++) {
      this.basisCircles[i] = new Array(this.dimension);
      this.basisCircles[i].fill(0, 0, this.dimension);
      this.basisCircles[i][i] = 1;
      this.projectionsBasisCircles[i] = new Array(2);
      this.projectionsBasisCircles[i].fill(0, 0, 2);
    }
  }

  init() {
    this.initInfo();
    if (this.theBilinearForm.length != this.dimension) {
      this.makeStandardBilinearForm(this.dimension);
    }
    if (this.basisCircles.length === 0) {
      this.makeBasisCircles();
    } else {
      this.projectionsBasisCircles = new Array(this.basisCircles.length);
      for (var i = 0; i < this.basisCircles.length; i++) {
        this.projectionsBasisCircles[i] = new Array(2);
        this.projectionsBasisCircles[i].fill(0, 0, 2);
      }
    }

    this.makeEiBasis();
    var theDiv = document.getElementById(this.idCanvas);
    theDiv.width = this.widthHTML;
    theDiv.height = this.heightHTML;
    this.canvas = theDiv.getContext("2d");
  }

  pointsAreWithinClickTolerance(x1, y1, x2, y2) {
    if (
      Math.abs(x1 - x2) > this.clickTolerance ||
      Math.abs(y1 - y2) > this.clickTolerance
    ) {
      return false;
    }
    return true;
  }

  scaleToUnitLength(output) {
    var scale = 1 / Math.sqrt(this.scalarProduct(output, output));
    multiplyVectorByScalar(output, scale)
  }

  scalarProduct(vector1, vector2) {
    var result = 0;
    for (var i = 0; i < this.dimension; i++) {
      for (var j = 0; j < this.dimension; j++) {
        result += vector1[i] * vector2[j] * this.theBilinearForm[i][j];
      }
    }
    return result;
  }

  makeScreenBasisOrthonormal() {
    addVectorTimesScalar(
      this.screenBasis[1],
      this.screenBasis[0],
      - this.scalarProduct(this.screenBasis[0], this.screenBasis[1]) /
      this.scalarProduct(this.screenBasis[0], this.screenBasis[0])
    );
    this.scaleToUnitLength(this.screenBasis[0]);
    this.scaleToUnitLength(this.screenBasis[1]);
  }

  computeScreenCoordinates(input, output) {
    output[0] = this.shiftScreenCoordinates[0] + this.graphicsUnit * this.scalarProduct(this.screenBasis[0], input);
    output[1] = this.shiftScreenCoordinates[1] + this.graphicsUnit * this.scalarProduct(this.screenBasis[1], input);
  }

  computeProjectionsEiBasis() {
    if (this.projectionsEiVectors.length != this.dimension) {
      this.projectionsEiVectors[i] = new Array(this.dimension);
      for (var i = 0; i < this.dimension; i++) {
        this.projectionsEiVectors[i] = new Array(this.dimension);
      }
    }
    for (var i = 0; i < this.eiBasis.length; i++) {
      this.computeScreenCoordinates(this.eiBasis[i], this.projectionsEiVectors[i]);
    }
  }

  computeProjectionsSpecialVectors() {
    for (var i = 0; i < this.basisCircles.length; i++) {
      if (this.projectionsBasisCircles[i] === null || this.projectionsBasisCircles[i] === undefined) {
        this.projectionsBasisCircles[i] = [];
      }
      this.computeScreenCoordinates(this.basisCircles[i], this.projectionsBasisCircles[i]);
    }
  }

  rotateInPlane(
    inputOutput, orthonormalBasis1, orthonormalBasis2, angle
  ) {
    var result = inputOutput.slice();
    var scalarProductWithBasis1 = this.scalarProduct(result, orthonormalBasis1);
    var scalarProductWithBasis2 = this.scalarProduct(result, orthonormalBasis2);
    addVectorTimesScalar(result, orthonormalBasis1, - scalarProductWithBasis1);
    addVectorTimesScalar(result, orthonormalBasis2, - scalarProductWithBasis2);
    addVectorTimesScalar(
      result,
      orthonormalBasis1,
      scalarProductWithBasis1 * Math.cos(angle) -
      scalarProductWithBasis2 * Math.sin(angle)
    );
    addVectorTimesScalar(
      result,
      orthonormalBasis2,
      scalarProductWithBasis1 * Math.sin(angle) +
      scalarProductWithBasis2 * Math.cos(angle)
    );
    for (var i = 0; i < inputOutput.length; i++) {
      inputOutput[i] = result[i];
    }
  }

  changeBasis() {
    var newX = this.mousePositionScreen[0] - this.shiftScreenCoordinates[0];
    var newY = this.mousePositionScreen[1] - this.shiftScreenCoordinates[1];
    if (newX == 0 && newY == 0) {
      return;
    }
    var oldX = this.projectionSelectedAtSelection[0] - this.shiftScreenCoordinates[0];
    var oldY = this.projectionSelectedAtSelection[1] - this.shiftScreenCoordinates[1];
    newX /= this.graphicsUnit;
    newY /= this.graphicsUnit;
    oldX /= this.graphicsUnit;
    oldY /= this.graphicsUnit;
    if (newX * newX + newY * newY <= 0.03) {
      return;
    }
    this.angleScreenChange = - getAngleScreenChange(newX, newY, oldX, oldY);
    if (this.angleScreenChange > Math.PI / 2 || this.angleScreenChange < - Math.PI / 2) {
      return;
    }
    this.screenBasis[0] = this.screenBasisAtSelection[0].slice();
    this.screenBasis[1] = this.screenBasisAtSelection[1].slice();
    this.rotateInPlane(
      this.screenBasis[0],
      this.screenBasisAtSelection[0], this.screenBasisAtSelection[1],
      this.angleScreenChange
    );
    this.rotateInPlane(
      this.screenBasis[1],
      this.screenBasisAtSelection[0], this.screenBasisAtSelection[1],
      this.angleScreenChange
    );
    if (this.dimension <= 2) {
      return;
    }
    var selectedVector = this.basisCircles[this.selectedBasisIndex];
    var xCuttingOld = this.scalarProduct(selectedVector, this.vProjectionNormalizedSelected);
    var yCuttingOld = this.scalarProduct(selectedVector, this.vOrthogonalSelected);
    var hypothenuseSquared = xCuttingOld * xCuttingOld + yCuttingOld * yCuttingOld;

    var xCuttingNew = Math.sqrt(newX * newX + newY * newY);
    if (xCuttingNew * xCuttingNew > hypothenuseSquared) {
      xCuttingNew = Math.sqrt(hypothenuseSquared) - 0.01;
    }
    var yCuttingNew = Math.sqrt(hypothenuseSquared - xCuttingNew * xCuttingNew);
    //if (newX * oldX + newY * oldY < 0) {
    //  xCuttingNew *= - 1;
    //}
    this.angleCuttingChange = - Math.atan2(yCuttingNew, xCuttingNew) + Math.atan2(yCuttingOld, xCuttingOld);
    for (var i = 0; i < 5; i++) {
      if (this.angleCuttingChange <= - Math.PI) {
        this.angleCuttingChange += 2 * Math.PI;
      }
      if (this.angleCuttingChange > Math.PI) {
        this.angleCuttingChange -= 2 * Math.PI;
      }
      if (this.angleCuttingChange >= 0 && this.angleCuttingChange <= Math.PI) {
        break;
      }
    }

    this.rotateInPlane(
      this.screenBasis[0],
      this.vProjectionNormalizedSelected, this.vOrthogonalSelected,
      this.angleCuttingChange
    );
    this.rotateInPlane(
      this.screenBasis[1],
      this.vProjectionNormalizedSelected, this.vOrthogonalSelected,
      this.angleCuttingChange
    );
    this.makeScreenBasisOrthonormal();
  }

  computePosXPosY(event) {
    this.mousePositionScreen[0] = event.clientX;
    this.mousePositionScreen[1] = event.clientY;
    var rect = this.canvasContainer.getBoundingClientRect();
    this.mousePositionScreen[0] -= rect.left;
    this.mousePositionScreen[1] -= rect.top;
    //  while (thePointer) {
    //    this.mousePositionScreen[0] -= thePointer.offsetLeft;
    //    if (typeof thePointer.scrollLeft === "number") {
    //      this.mousePositionScreen[0] += thePointer.scrollLeft;
    //    }
    //    this.mousePositionScreen[1] -= thePointer.offsetTop;
    //    if (typeof thePointer.scrollTop === "number") {
    //      this.mousePositionScreen[1] += thePointer.scrollTop;
    //    }
    //    //thePointer = thePointer.offsetParent;
    //    thePointer = thePointer.parentElement;
    //  }
  }

  releaseMouse(event) {
    this.selectedBasisIndex = - 1;
  }

  selectIndex(index) {
    this.selectedBasisIndex = index;
    this.projectionSelectedAtSelection[0] = this.projectionsBasisCircles[index][0];
    this.projectionSelectedAtSelection[1] = this.projectionsBasisCircles[index][1];
    if (this.screenBasisAtSelection.length === 0) {
      this.screenBasisAtSelection = new Array(2);
    }
    this.screenBasisAtSelection[0] = this.screenBasis[0].slice();
    this.screenBasisAtSelection[1] = this.screenBasis[1].slice();

    var epsilon = 0.0000001;
    for (var round = 0; round < this.dimension + 1; round++) {
      var selectedVector = this.basisCircles[this.selectedBasisIndex].slice();
      if (round > 0) {
        selectedVector[(round + 1) % this.dimension] += 0.01;
      }
      var vectorTimesE1 = this.scalarProduct(selectedVector, this.screenBasisAtSelection[0]);
      var vectorTimesE2 = this.scalarProduct(selectedVector, this.screenBasisAtSelection[1]);
      this.vProjectionNormalizedSelected = this.screenBasisAtSelection[0].slice();
      multiplyVectorByScalar(this.vProjectionNormalizedSelected, vectorTimesE1);
      addVectorTimesScalar(this.vProjectionNormalizedSelected, this.screenBasisAtSelection[1], vectorTimesE2);
      this.vOrthogonalSelected = selectedVector.slice();
      addVectorTimesScalar(this.vOrthogonalSelected, this.vProjectionNormalizedSelected, - 1);
      var vOrthogonalSquareLength = this.scalarProduct(this.vOrthogonalSelected, this.vOrthogonalSelected);
      var vProjectionSquareLength = this.scalarProduct(this.vProjectionNormalizedSelected, this.vProjectionNormalizedSelected);
      if (vOrthogonalSquareLength > epsilon && vProjectionSquareLength > epsilon) {
        break;
      }
      if (this.dimension <= 2 && vProjectionSquareLength > epsilon) {
        break;
      }
    }
    this.scaleToUnitLength(this.vProjectionNormalizedSelected);
    if (this.dimension > 2) {
      this.scaleToUnitLength(this.vOrthogonalSelected);
    }
  }

  clickCanvas(event) {
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
    for (var i = 0; i < this.basisCircles.length; i++) {
      if (this.pointsAreWithinClickTolerance(
        this.mousePositionScreenClicked[0], this.mousePositionScreenClicked[1],
        this.projectionsBasisCircles[i][0], this.projectionsBasisCircles[i][1]
      )) {
        this.selectIndex(i);
      }
    }
  }

  computeHighlightedIndex() {
    var oldSelected = this.selectedHighlightIndex;
    this.selectedHighlightIndex = - 1;
    this.currentHighlightIndices = {};
    for (var i = 0; i < this.indicesHighlightOperations.length; i++) {
      var currentIndex = this.indicesHighlightOperations[i];
      var currentHighlight = this.drawOperations[currentIndex];
      currentHighlight.computeProjections();
      var currentVectors = currentHighlight.vectorProjections;
      for (var j = 0; j < currentVectors.length; j++) {
        if (this.pointsAreWithinClickTolerance(
          this.mousePositionScreen[0], this.mousePositionScreen[1], currentVectors[j][0], currentVectors[j][1]
        )) {
          this.selectedHighlightIndex = currentIndex;
          if (this.currentHighlightIndices[currentIndex] === undefined) {
            this.currentHighlightIndices[currentIndex] = {};
          }
          this.currentHighlightIndices[currentIndex][j] = true;
        }
      }
    }
    return oldSelected !== this.selectedHighlightIndex;
  }

  mouseMoveRedraw(event) {
    this.computePosXPosY(event);
    this.writeInfo();
    //this.processMousePosition(posx, - posy);
    var doRedraw = this.computeHighlightedIndex();
    if (this.selectedBasisIndex == - 1 && !doRedraw) {
      return;
    }
    if (this.selectedBasisIndex == - 2) {
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
    } else if (this.selectedBasisIndex >= 0) {
      this.changeBasis();
      var doRedraw = true;
    }
    if (doRedraw) {
      this.drawAll();
    }
  }

  mouseHandleWheel(event) {
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
    if (this.graphicsUnit < 3) {
      this.graphicsUnit = 3;
    }
    var newXDelta = oldXDeltaScaled * this.graphicsUnit;
    var newYDelta = oldYDeltaScaled * this.graphicsUnit;
    this.shiftScreenCoordinates[0] += oldXDelta - newXDelta;
    this.shiftScreenCoordinates[1] += oldYDelta - newYDelta;
    this.drawAll();
  }

  initFromObject(input) {
    this.dimension = input.dimension;
    this.theBilinearForm = input.bilinearForm;
    this.screenBasis = input.screenBasis;
    this.basisCircles = input.draggablePoints;
    if (input.frameLength !== undefined && input.frameLength !== null) {
      this.animation.frameLength = input.frameLength;
    }
    for (var i = 0; i < input.drawObjects.length; i++) {
      var currentOperation = input.drawObjects[i];
      if (currentOperation.operation === "circleAtVector") {
        this.drawCircle(
          currentOperation.location,
          currentOperation.color,
          currentOperation.radius,
          currentOperation.frameId,
          currentOperation.frameIndex
        );
      } else if (currentOperation.operation === "segment") {
        this.drawLine(currentOperation.points[0], currentOperation.points[1], currentOperation.color);
      } else if (currentOperation.operation === "highlightGroup") {
        this.drawHighlightGroup(
          currentOperation.points, currentOperation.labels,
          currentOperation.color, currentOperation.radius
        );
      } else if (currentOperation.operation === "text") {
        this.drawText(currentOperation.location, currentOperation.text, currentOperation.color);
      } else if (currentOperation.operation === "path") {
        this.drawPath(
          currentOperation.points,
          currentOperation.color,
          currentOperation.lineWidth,
          currentOperation.frameId,
          currentOperation.frameIndex
        );
      }
    }
    this.graphicsUnit = input.graphicsUnit;
    if (this.graphicsUnit === undefined || this.graphicsUnit === null) {
      this.graphicsUnit = 150;
    }
    if (input.widthHTML !== undefined && input.widthHTML !== null) {
      this.widthHTML = input.widthHTML;
    }
    if (input.heightHTML !== undefined && input.heightHTML !== null) {
      this.heightHTML = input.heightHTML;
    }
    if (this.animation.numberOfFrames > 0) {
      this.animation.timeoutHandle = setTimeout(this.animate.bind(this), 0);
    } else {
      this.drawAll();
    }
  }

  animate() {
    clearTimeout(this.animation.timeoutHandle);
    this.animation.currentFrameIndex++;
    if (this.animation.currentFrameIndex < 0 || this.animation.currentFrameIndex > this.animation.numberOfFrames) {
      this.animation.currentFrameIndex = 0;
    }
    this.drawAll();
    this.animation.timeoutHandle = setTimeout(this.animate.bind(this), this.animation.frameLength);
  }

}

class DrawHighlights {
  constructor(inputOwnerId, inputData) {
    this.ownerId = inputOwnerId;
    this.indexInOperations = inputData.indexInOperations;
    var owner = collectionGraphicsNDimensions[this.ownerId];
    owner.indicesHighlightOperations.push(this.indexInOperations);
    this.vectors = inputData.vectors;
    this.vectorProjections = [];
    this.color = inputData.color;
    this.labels = inputData.labels;
    this.radius = inputData.radius;
  }

  computeProjections() {
    var owner = collectionGraphicsNDimensions[this.ownerId];
    if (this.vectorProjections.length !== this.vectors.length) {
      this.vectorProjecions = new Array(this.vectors.length);
    }
    for (var i = 0; i < this.vectors.length; i++) {
      if (this.vectorProjections[i] === undefined || this.vectorProjections[i] === null) {
        this.vectorProjections[i] = [];
      }
      owner.computeScreenCoordinates(this.vectors[i], this.vectorProjections[i]);
    }
  }

  drawNoFinish() {
    /**@type {GraphicsNDimensions} */
    var owner = collectionGraphicsNDimensions[this.ownerId];
    if (!(this.indexInOperations in owner.currentHighlightIndices)) {
      return;
    }
    var currentIndices = owner.currentHighlightIndices[this.indexInOperations];
    var nextLine = [];
    for (var i = 0; i < this.labels.length; i++) {
      if (i in currentIndices) {
        nextLine.push(`<b>${this.labels[i]}</b>`);
      } else {
        nextLine.push(this.labels[i]);
      }
    }
    owner.highlightInfoContent.push(nextLine.join(", "));

    this.computeProjections();
    var canvas = owner.canvas;
    canvas.strokeStyle = this.color;
    for (var i = 0; i < this.vectors.length; i++) {
      canvas.beginPath();
      canvas.arc(
        this.vectorProjections[i][0],
        this.vectorProjections[i][1],
        this.radius,
        0, 2 * Math.PI
      );
      canvas.stroke();
    }
  }
}

class DrawPath {
  constructor(inputOwnerId, inputData) {
    this.ownerId = inputOwnerId;
    this.points = inputData.points;
    this.lineWidth = inputData.lineWidth;
    this.colorLine = inputData.colorLine;
    if (inputData.frameId !== "" && inputData.frameId !== null && inputData.frameId !== undefined) {
      this.frameId = inputData.frameId;
      this.frameIndex = inputData.frameIndex;
      collectionGraphicsNDimensions[this.ownerId].registerFrameIndex(this.frameIndex);
    }
  }

  drawNoFinish() {
    var owner = collectionGraphicsNDimensions[this.ownerId];
    var canvas = owner.canvas;
    canvas.beginPath();
    canvas.strokeStyle = this.colorLine;
    canvas.lineWidth = this.lineWidth;
    var vector = [];
    owner.computeScreenCoordinates(this.points[0], vector);
    canvas.moveTo(vector[0], vector[1]);
    for (var counterPoints = 1; counterPoints < this.points.length; counterPoints++) {
      owner.computeScreenCoordinates(this.points[counterPoints], vector);
      canvas.lineTo(vector[0], vector[1]);
    }
    canvas.stroke();
  }
}

class DrawSegmentBetweenTwoVectors {
  constructor(inputOwnerId, inputData) {
    this.ownerId = inputOwnerId;
    this.left = inputData.left.slice();
    this.right = inputData.right.slice();
    this.lineWidth = inputData.lineWidth;
    this.colorLine = inputData.colorLine;
  }

  drawNoFinish() {
    var owner = collectionGraphicsNDimensions[this.ownerId];
    var canvas = owner.canvas;
    canvas.beginPath();
    canvas.strokeStyle = this.colorLine;
    canvas.lineWidth = this.lineWidth;
    var screenCoordinatesLeft = [];
    var screenCoordinatesRight = [];
    owner.computeScreenCoordinates(this.left, screenCoordinatesLeft);
    owner.computeScreenCoordinates(this.right, screenCoordinatesRight);
    canvas.moveTo(screenCoordinatesLeft[0], screenCoordinatesLeft[1]);
    canvas.lineTo(screenCoordinatesRight[0], screenCoordinatesRight[1]);
    canvas.stroke();
  }
}

class DrawFilledShape {
  constructor(inputOwnerId, inputData) {
    this.points = inputData.points;
    this.ownerId = inputOwnerId;
    this.colorFill = inputData.colorFill;
    this.lineWidth = inputData.lineWidth;
  }

  drawNoFinish() {
    var owner = collectionGraphicsNDimensions[this.ownerId];
    var canvas = owner.canvas;
    canvas.beginPath();
    canvas.fillStyle = this.colorFill;
    canvas.lineWidth = this.lineWidth;
    var currentPoint = [];
    owner.computeScreenCoordinates(this.points[0], currentPoint);
    canvas.moveTo(currentPoint[0], currentPoint[1]);
    for (var counterPoint = 1; counterPoint < this.points.length; counterPoint++) {
      owner.computeAllScreenCoordinates(this.points[counterPoint], currentPoint);
      canvas.lineTo(currentPoint[0], currentPoint[1]);
    }
    canvas.closePath();
    canvas.fill();
    //canvas.stroke();
  }
}

class DrawCircleAtVector {
  constructor(inputOwnerId, inputData) {
    this.ownerId = inputOwnerId;
    this.location = inputData.location.slice();
    this.locationScreenCoordinates = [];
    this.colorFill = inputData.colorFill;
    this.radius = inputData.radius;
    if (inputData.frameId !== undefined && inputData.frameId !== null) {
      this.frameId = inputData.frameId;
      this.frameIndex = inputData.frameIndex;
      collectionGraphicsNDimensions[this.ownerId].registerFrameIndex(this.frameIndex);
    }
  }

  drawNoFinish() {
    var owner = collectionGraphicsNDimensions[this.ownerId];
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
}

class DrawTextAtVector {
  constructor(inputOwnerId, inputData) {
    this.ownerId = inputOwnerId;
    this.text = inputData.text;
    this.location = inputData.location;
    this.locationScreen = [];
    this.colorFill = inputData.colorFill;
  }

  drawNoFinish() {
    var owner = collectionGraphicsNDimensions[this.ownerId];
    var canvas = owner.canvas;
    canvas.strokeStyle = this.colorFill;
    owner.computeScreenCoordinates(this.location, this.locationScreen);
    canvas.strokeText(this.text, this.locationScreen[0] + owner.textShift[0], this.locationScreen[1] + owner.textShift[1]);
  }
}

function multiplyVectorByScalar(vector, scalar) {
  for (var counterDimension = 0; counterDimension < vector.length; counterDimension++) {
    vector[counterDimension] *= scalar;
  }
}

function addVectorTimesScalar(vector, other, scalar) {
  for (var counterDimension = 0; counterDimension < vector.length; counterDimension++) {
    vector[counterDimension] += other[counterDimension] * scalar;
  }
}

function testA3(idCanvas, idSpanInformation) {
  var theA3 = new GraphicsNDimensions(idCanvas, idSpanInformation);
  theA3.dimension = 3;
  theA3.theBilinearForm = [
    [2, -1, 0],
    [-1, 2, -1],
    [0, -1, 2]
  ];
  theA3.screenBasis = [
    [0.707107, 0.707107, 0],
    [0, 0.707107, 0.707107]
  ];
  var labeledVectors = [
    [-1, -1, -1],
    [0, -1, -1],
    [-1, -1, 0],
    [0, 0, -1],
    [0, -1, 0],
    [-1, 0, 0],
    [1, 0, 0],
    [0, 1, 0],
    [0, 0, 1],
    [1, 1, 0],
    [0, 1, 1],
    [1, 1, 1],
  ];
  var segments = [
    [[0, - 1, - 1], [1, 0, 0]],
    [[0, - 1, - 1], [-1, -1, -1]],
    [[0, - 1, - 1], [0, 0, -1]],
    [[0, - 1, - 1], [0, -1, 0]],
    [[-1, -1, 0], [-1, 0, 0]],
    [[-1, -1, 0], [0, 0, 1]],
    [[-1, -1, 0], [-1, -1, -1]],
    [[-1, -1, 0], [0, -1, 0]],
    [[1, 1, 0], [1, 0, 0]],
    [[1, 1, 0], [0, 0, -1]],
    [[1, 1, 0], [1, 1, 1]],
    [[1, 1, 0], [0, 1, 0]],
    [[0, 1, 1], [-1, 0, 0]],
    [[0, 1, 1], [0, 0, 1]],
    [[0, 1, 1], [1, 1, 1]],
    [[0, 1, 1], [0, 1, 0]],
    [[-1, 0, 0], [-1, -1, -1]],
    [[-1, 0, 0], [0, 1, 0]],
    [[1, 0, 0], [1, 1, 1]],
    [[1, 0, 0], [0, -1, 0]],
    [[0, 0, 1], [1, 1, 1]],
    [[0, 0, 1], [0, -1, 0]],
    [[-1, -1, -1], [0, 0, -1]],
    [[0, 0, -1], [0, 1, 0]]
  ];
  theA3.drawStandardEiBasis("red");
  theA3.drawText([1, 0, 0], "(1, 0, 0)");
  for (var counterLabel = 0; counterLabel < labeledVectors.length; counterLabel++) {
    theA3.drawLine([0, 0, 0], labeledVectors[counterLabel], "green");
    //theA3.drawText(labeledVectors[counterLabel], `[${labeledVectors.join(', ')}]`);
    //theA3.drawCircle(labeledVectors[counterLabel], "red");
  }
  for (var counterSegment = 0; counterSegment < segments.length; counterSegment++) {
    theA3.drawLine(segments[counterSegment][0], segments[counterSegment][1], "blue");
  }
  theA3.graphicsUnit = 150;
  theA3.drawAll();
}

function createGraphicsFromObject(input) {
  if (input.idCanvas === undefined || input.idCanvas === null) {
    throw ("idCanvas missing.");
  }
  if (input.idSpanInformation === undefined || input.idSpanInformation === null) {
    throw ("idSpanInformation missing.");
  }
  var theObject = new GraphicsNDimensions(input.idCanvas, input.idSpanInformation, input.idHighlightInformation);
  theObject.initFromObject(input);
}

function testA4(idCanvas, idSpanInformation) {
  var theA4 = new GraphicsNDimensions(idCanvas, idSpanInformation);
  theA4.dimension = 4;
  theA4.theBilinearForm = [
    [2, -1, 0, 0],
    [-1, 2, -1, 0],
    [0, -1, 2, -1],
    [0, 0, -1, 2]
  ];
  theA4.screenBasis = [
    [-0.195, 0.316, 0.828, 0.632],
    [0.602, 0.973, 0.602, 0]
  ];
  var labeledVectors = [
    [1, 0, 0, 0],
    [0, 1, 0, 0],
    [0, 0, 1, 0],
    [0, 0, 0, 1],
    [1, 1, 0, 0],
    [0, 1, 1, 0],
    [0, 0, 1, 1],
    [1, 1, 1, 0],
    [0, 1, 1, 1],
    [1, 1, 1, 1],
  ];
  var numberOfPositiveVectors = labeledVectors.length;
  for (var counter = 0; counter < numberOfPositiveVectors; counter++) {
    var theVector = labeledVectors[counter].slice();
    multiplyVectorByScalar(theVector, - 1);
    labeledVectors.push(theVector);
  }
  var segments = [];
  for (var counter = 0; counter < labeledVectors.length; counter++) {
    var minDistance = 10;
    for (var secondCounter = counter + 1; secondCounter < labeledVectors.length; secondCounter++) {
      var newDistance = theA4.scalarProduct(labeledVectors[counter], labeledVectors[secondCounter]);
      if (newDistance < minDistance && newDistance > 0) {
        minDistance = newDistance;
      }
    }
    for (var secondCounter = counter + 1; secondCounter < labeledVectors.length; secondCounter++) {
      var theDistance = theA4.scalarProduct(labeledVectors[counter], labeledVectors[secondCounter]);
      if (theDistance != minDistance) {
        continue;
      }
      segments.push([labeledVectors[counter].slice(), labeledVectors[secondCounter].slice()]);
    }
  }

  theA4.drawStandardEiBasis("red");
  for (var counterLabel = 0; counterLabel < labeledVectors.length; counterLabel++) {
    theA4.drawLine([0, 0, 0, 0], labeledVectors[counterLabel], "green");
    //theA3.drawText(labeledVectors[counterLabel], `[${labeledVectors.join(', ')}]`);
    //theA3.drawCircle(labeledVectors[counterLabel], "red");
  }
  for (var counterSegment = 0; counterSegment < segments.length; counterSegment++) {
    theA4.drawLine(segments[counterSegment][0], segments[counterSegment][1], "blue");
  }
  theA4.graphicsUnit = 150;
  theA4.drawAll();
}

var module;
if (window.calculator === undefined) {
  window.calculator = {};
}

if (window.calculator.graphicsNDimensions === undefined) {
  window.calculator.graphicsNDimensions = {
    startProjectionPlaneUser: startProjectionPlaneUser,
    createGraphicsFromObject: createGraphicsFromObject,
    GraphicsNDimensions: GraphicsNDimensions,
  };
}

if (module === undefined) {
  module = {};
}

module.exports = {
  createGraphicsFromObject,
  GraphicsNDimensions,
  startProjectionPlaneUser,
  testA3,
  testA4,
};
