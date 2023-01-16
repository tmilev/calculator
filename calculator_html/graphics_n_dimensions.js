/**
 * File needs to function as stand-alone javascript
 * as well be used as a commonJS module included via
 * require('graphics_n_dimension.js')).
 * Please don't require(...) any modules from this file.
 **/
"use strict";

function toStringVector(vector) {
  let result = "";
  result += "(";
  for (let i = 0; i < vector.length; i++) {
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

function getAngleScreenChange(
  /** @type{number} */
  newX,
  /** @type{number} */
  newY,
  /** @type{number} */
  oldX,
  /** @type{number} */
  oldY,
) {
  let result = getAngleFromXandY(newX, newY) - getAngleFromXandY(oldX, oldY);
  let topBound = Math.PI;
  let bottomBound = - Math.PI;
  while (result > topBound || result <= bottomBound) {
    if (result > topBound) {
      result -= 2 * Math.PI;
    } else {
      result += 2 * Math.PI;
    }
  }
  return result;
}

const stringCorrections = {
  "lightblue": "blue",
};

/** @return{string} */
function correctColor(
  /** @type {string} */
  input,
) {
  if (input in stringCorrections) {
    return stringCorrections[input];
  }
  return input;
}

class Layer {
  constructor(
    /** @type{string} */
    name,
  ) {
    this.name = name;
    /** @type{number[]} */
    this.indices = [];
    this.visible = true;
  }

  addIndex(
    /** @type{number} */
    index,
  ) {
    this.indices.push(index);    
  }
}

class GraphicsNDimensions {
  constructor(
    /** @type{HTMLCanvasElement|string} */
    inputContainer,
    /** @type{HTMLCanvasElement|string} */
    inputContainerControls,
    /** @type{HTMLElement|string} */
    inputInfoContainer,
    inputIdHighlightInfo,
  ) {
    this.vectors = [];
    this.vectorProjections = [];
    this.drawOperations = [];
    /** @type{Array.<Array.<HTMLTextAreaElement>>} */
    this.screenBasisInputs = [];
    if (typeof inputInfoContainer === "string") {
      inputInfoContainer = document.getElementById(inputInfoContainer);
    }
    /** @type{HTMLElement} */
    this.informationContainer = inputInfoContainer;
    /** @type{HTMLElement} */
    this.planeInfo = null;
    /** @type{HTMLElement} */
    this.panelInfo = null;
    if (
      inputIdHighlightInfo !== null &&
      inputIdHighlightInfo !== undefined &&
      inputIdHighlightInfo !== ""
    ) {
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
    if (typeof inputContainer === "string") {
      inputContainer = document.getElementById(inputContainer);
    }
    /** @type{HTMLCanvasElement} */
    this.canvasContainer = inputContainer;
    if (typeof inputContainerControls === "string") {
      inputContainerControls = document.getElementById(inputContainerControls);
    }
    /** @type{HTMLElement} */
    this.containerControls = inputContainerControls;
    this.dimension = 0;
    this.bilinearForm = [];
    this.graphicsUnit = 0;
    this.selectedBasisIndex = - 1;
    this.selectedHighlightIndex = - 1;

    /**
     * @type {Object<string, Object<string, boolean> >}
     * Keys are indices of groups currently highlighted.
     * Values are arrays with indices
     * within each group over which the mouse is located. 
     */
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
    /** @type{HTMLCanvasElement|null} */
    this.canvas = null;
    this.widthHTML = this.canvasContainer.width;
    this.heightHTML = this.canvasContainer.height;
    this.shiftScreenCoordinates = [this.widthHTML / 2, this.heightHTML / 2];
    this.mousePositionScreen = [0, 0];
    this.mousePositionScreenClicked = [0, 0];
    this.shiftScreenCoordinatesClicked = [0, 0];

    this.clickTolerance = 5;
    this.textShift = [- 3, - 4];

    /** @type{Object.<string, Layer>} */
    this.layers = {};
    /** @type{Layer[]} */
    this.layerOfEachDrawOperation = [];
    /** @type{HTMLElement|null} */
    this.containerLayers = null;
    this.boundingBoxScreen = [[0, 0], [0, 0]];
    this.boundingBoxLatex = [[0, 0], [0, 0]];
  }

  initVectors(inputVectors) {
    this.vectors = inputVectors;
    this.vectorProjections = [];
    for (let counterVector = 0; counterVector < this.vectors.length; counterVector++) {
      this.vectorProjections.push([0, 0]);
    }
  }

  /** @returns {string} */
  getInfoHTML() {
    let result = "";
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
    for (let i = 0; i < this.basisCircles.length; i++) {
      result += `<br>${i}: ${toStringVector(this.basisCircles[i])}: ${toStringVector(this.projectionsBasisCircles[i])}`;
    }
    return result;
  }

  writeInfo() {
    let info = this.getInfoHTML();
    if (this.panelInfo === null) {
      return;
    }
    this.panelInfo.innerHTML = info;
  }

  initializeControls() {
    if (this.containerControls === null) {
      return;
    }

    this.containerControls.style.transition = "1s all";
    this.containerControls.textContent = "";
    let button = document.createElement("button");
    button.textContent = "\u21BB";
    button.title = "redraw";
    this.containerControls.appendChild(button);
    let snapShotLaTeX = document.createElement("button");
    snapShotLaTeX.innerHTML = "<tiny>L&#x1F4CB;</tiny>";
    snapShotLaTeX.title = "Copy LaTeX+pstricks self-contained document.";
    snapShotLaTeX.addEventListener("click", () => {
      this.snapShotLaTeX();
    });
    this.containerControls.appendChild(snapShotLaTeX);
    if (Object.keys(this.layers).length > 1) {
      this.initializeLayers();
    }

  }

  initInfo() {
    let info = document.createElement("div");
    for (let i = 0; i < 2; i++) {
      this.screenBasisInputs.push([]);
      for (let j = 0; j < this.dimension; j++) {
        let textArea = document.createElement("textarea");
        textArea.rows = 1;
        textArea.cols = 2;
        this.screenBasisInputs[i].push(textArea);
        info.appendChild(textArea);
      }
      info.appendChild(document.createElement("br"));
    }
    let button = document.createElement("button");
    button.textContent = "Change to basis";
    button.addEventListener("click", () => {
      this.startProjectionPlaneUser();
    });
    info.appendChild(button);
    info.appendChild(document.createElement("br"));
    this.planeInfo = document.createElement("div");
    this.planeInfo.className = "panel";
    info.appendChild(this.planeInfo);
    if (this.informationContainer !== null) {
      this.informationContainer.textContent = "";
      this.informationContainer.appendChild(info);
    }
  }

  snapShotLaTeX() {
    let result = [];
    this.containerControls.style.backgroundColor = "lightgreen";
    setTimeout(() => {
      this.containerControls.style.backgroundColor = "";
    }, 1000);
    result.push("\\documentclass{article}");
    result.push("\\usepackage{auto-pst-pdf}");
    result.push("\\usepackage{pst-plot}")
    result.push("\\begin{document}");
    result.push("\\psset{xunit = 0.01cm, yunit = 0.01cm}");
    result.push("\\begin{pspicture}(-100,-100)(100,100)");
    this.computeProjectionsEiBasis();
    this.computeBoundingBox();
    for (
      let i = 0;
      i < this.drawOperations.length;
      i++
    ) {
      if (!this.isVisible(i)) {
        continue;
      }
      const drawOperation = this.drawOperations[i];
      result.push(drawOperation.getLaTeXOperation());
    }
    result.push("\\end{pspicture}");
    result.push("\\end{document}");
    
    navigator.clipboard.writeText(result.join("\n"));
  }

  computeBoundingBox() {
    this.boundingBoxLatex = [[0, 0], [0, 0]];
    this.boundingBoxScreen = [[0, 0], [0, 0]];
    for (
      let i = 0;
      i < this.drawOperations.length;
      i++
    ) {
      if (!this.isVisible(i)) {
        continue;
      }
      const drawOperation = this.drawOperations[i];
      drawOperation.accountBoundingBox();
    }    
  }

  accountVectorInBoundingBox(
    /** @type{number[]} */
    vector,
  ) {
    const screenCoordinates = [0, 0];
    const latexCoordinates = [0, 0];
    this.computeScreenCoordinates(vector, screenCoordinates);
    this.computeLatexCoordinates(vector, latexCoordinates);
    this.accountVectorInBoundingBoxOfType(
      screenCoordinates,
      this.boundingBoxScreen,
    );
    this.accountVectorInBoundingBoxOfType(
      latexCoordinates,
      this.boundingBoxLatex,
    );
  }

  accountVectorInBoundingBoxOfType(
    /** @type{number[]} */
    vector,
    /** @type{number[][]} */
    output,
  ) {
    output[0][0] = Math.min(vector[0], output[0][0]);
    output[0][1] = Math.min(vector[1], output[0][1]);
    output[1][0] = Math.max(vector[0], output[1][0]);
    output[1][1] = Math.max(vector[1], output[1][1]); 
  }

  isVisible(index) {
    return this.layerOfEachDrawOperation[index].visible;
  }

  drawStandardEiBasis(color) {
    for (let i = 0; i < this.dimension; i++) {
      let eiVector = new Array(this.dimension);
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
      this, {
      location: position,
      colorFill: color,
      radius: radius,
      frameId: frameId,
      frameIndex: frameIndex
    }));
  }

  drawLine(left, right, color, extraOptions) {
    if (color === undefined) {
      color = "black";
    }
    let segment = {
      left: left,
      right: right,
      lineWidth: 1,
      colorLine: color,
    };
    // Copy all unforeseen options into the segment object.
    segment = Object.assign(extraOptions, segment);
    this.drawOperations.push(new DrawSegmentBetweenTwoVectors(
      this, segment
    ));
  }

  drawPath(points, color, lineWidth, frameId, frameIndex) {
    if (color === undefined) {
      color = "black";
    }
    this.drawOperations.push(new DrawPath(
      this, {
      points: points,
      lineWidth: lineWidth,
      colorLine: color,
      frameId: frameId,
      frameIndex: frameIndex,
    }));
  }

  drawHighlightGroup(points, labels, color, radius) {
    if (color === undefined) {
      color = "black";
    }
    if (radius === undefined) {
      radius = 3;
    }
    this.drawOperations.push(new DrawHighlights(
      this, {
      vectors: points,
      indexInOperations: this.drawOperations.length,
      labels: labels,
      color: color,
      radius: radius
    }));
  }

  drawText(location, text, color) {
    if (color === undefined) {
      color = "black";
    }
    this.drawOperations.push(new DrawTextAtVector(
      this, {
      location: location,
      text: text,
      colorFill: color
    }
    ));
  }

  startProjectionPlaneUser() {
    this.animationBasisChange.frameCount = 0;
    this.animationBasisChange.screenStart[0] = this.screenBasis[0].slice();
    this.animationBasisChange.screenStart[1] = this.screenBasis[1].slice();
    for (let i = 0; i < 2; i++) {
      this.animationBasisChange.screenGoal[i] = [];
      for (let j = 0; j < this.dimension; j++) {
        let coordinate =  this.screenBasisInputs[i][j].value;
        this.animationBasisChange.screenGoal[i][j] = Number(coordinate);
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
    let frameCount = this.animationBasisChange.frameCount;
    let maxFrameCount = this.animationBasisChange.numberOfFrames;
    if (frameCount > maxFrameCount) {
      return;
    }
    let screenStart = this.animationBasisChange.screenStart;
    let screenGoal = this.animationBasisChange.screenGoal;
    let ratio = frameCount / maxFrameCount;
    for (let i = 0; i < this.dimension; i++) {
      this.screenBasis[0][i] = screenGoal[0][i] * ratio + screenStart[0][i] * (1 - ratio);
      this.screenBasis[1][i] = screenGoal[1][i] * ratio + screenStart[1][i] * (1 - ratio);
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
    if (this.dimension === -1) {
      console.log("Not drawing: received negative dimension.");
      return;
    }
    this.computeProjectionsEiBasis();
    this.computeProjectionsSpecialVectors();
    this.canvas.clearRect(0, 0, this.widthHTML, this.heightHTML);
    this.writeInfo();
    this.highlightInfoContent = [];
    for (let i = 0; i < this.drawOperations.length; i++) {
      let currentOperation = this.drawOperations[i];
      if (
        currentOperation.frameId !== undefined &&
        currentOperation.frameId !== null
      ) {
        if (currentOperation.frameIndex !== this.animation.currentFrameIndex) {
          continue;
        }
      }
      if (!this.isVisible(i)) {
        continue;
      }
      currentOperation.drawNoFinish();
    }
    if (this.highlightInfoContent.length > 0) {
      if (this.idHighlightInfo !== null) {
        let highlightComponent = document.getElementById(this.idHighlightInfo);
        highlightComponent.innerHTML = this.highlightInfoContent.join("<br>");
      }
    }
    this.animationBasisChange.frameStarted = false;
  }

  makeStandardBilinearForm() {
    this.bilinearForm = new Array(this.dimension);
    for (let i = 0; i < this.bilinearForm.length; i++) {
      this.bilinearForm[i] = new Array(this.dimension);
      this.bilinearForm[i].fill(0, 0, this.dimension);
      this.bilinearForm[i][i] = 1;
    }
  }

  makeEiBasis() {
    this.eiBasis = new Array(this.dimension);
    for (let i = 0; i < this.eiBasis.length; i++) {
      this.eiBasis[i] = new Array(this.dimension);
      this.eiBasis[i].fill(0, 0, this.dimension);
      this.eiBasis[i][i] = 1;
    }
  }

  makeBasisCircles() {
    this.basisCircles = new Array(this.dimension);
    this.projectionsBasisCircles = new Array(this.dimension);
    for (let i = 0; i < this.basisCircles.length; i++) {
      this.basisCircles[i] = new Array(this.dimension);
      this.basisCircles[i].fill(0, 0, this.dimension);
      this.basisCircles[i][i] = 1;
      this.projectionsBasisCircles[i] = new Array(2);
      this.projectionsBasisCircles[i].fill(0, 0, 2);
    }
  }

  init() {
    this.initializeControls();
    this.initInfo();
    if (this.dimension === -1) {
      console.log("Negative dimension received, not plotting.");
      return;
    }
    if (this.bilinearForm.length !== this.dimension ) {
      this.makeStandardBilinearForm(this.dimension);
    }
    if (this.basisCircles.length === 0) {
      this.makeBasisCircles();
    } else {
      this.projectionsBasisCircles = new Array(this.basisCircles.length);
      for (let i = 0; i < this.basisCircles.length; i++) {
        this.projectionsBasisCircles[i] = new Array(2);
        this.projectionsBasisCircles[i].fill(0, 0, 2);
      }
    }

    this.makeEiBasis();
    this.canvasContainer.width = this.widthHTML;
    this.canvasContainer.height = this.heightHTML;
    /** @type {CanvasRenderingContext2D} */
    this.canvas = this.canvasContainer.getContext("2d");
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
    let scale = 1 / Math.sqrt(this.scalarProduct(output, output));
    multiplyVectorByScalar(output, scale)
  }

  scalarProduct(vector1, vector2) {
    let result = 0;
    for (let i = 0; i < this.dimension; i++) {
      for (let j = 0; j < this.dimension; j++) {
        result += vector1[i] * vector2[j] * this.bilinearForm[i][j];
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

  computeScreenCoordinates(
    /** @type{number[]} */
    input,
    /** @type{number[]} */
    output,
  ) {
    output[0] = this.shiftScreenCoordinates[0];
    output[0] += this.graphicsUnit * this.scalarProduct(
      this.screenBasis[0], input,
    );
    output[1] = this.shiftScreenCoordinates[1];
    output[1] += this.graphicsUnit * this.scalarProduct(
      this.screenBasis[1], input,
    );
  }

  computeLatexCoordinates(
    /** @type{number[]} */
    input,
    /** @type{number[]} */
    output,
  ) {
    const intermediate = [0, 0];
    this.computeScreenCoordinates(input, intermediate);
    output[0] = intermediate[0] - this.shiftScreenCoordinates[0];
    output[1] = intermediate[1] - this.shiftScreenCoordinates[1];
    output[1] *= -1;
  }

  computeProjectionsEiBasis() {
    if (this.projectionsEiVectors.length != this.dimension) {
      for (let i = 0; i < this.dimension; i++) {
        this.projectionsEiVectors[i] = [0, 0];
      }
    }
    for (let i = 0; i < this.eiBasis.length; i++) {
      this.computeScreenCoordinates(this.eiBasis[i], this.projectionsEiVectors[i]);
    }
  }

  computeProjectionsSpecialVectors() {
    for (let i = 0; i < this.basisCircles.length; i++) {
      if (
        this.projectionsBasisCircles[i] === null ||
        this.projectionsBasisCircles[i] === undefined
      ) {
        this.projectionsBasisCircles[i] = [];
      }
      this.computeScreenCoordinates(this.basisCircles[i], this.projectionsBasisCircles[i]);
    }
  }

  rotateInPlane(
    inputOutput, orthonormalBasis1, orthonormalBasis2, angle
  ) {
    let result = inputOutput.slice();
    let scalarProductWithBasis1 = this.scalarProduct(result, orthonormalBasis1);
    let scalarProductWithBasis2 = this.scalarProduct(result, orthonormalBasis2);
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
    for (let i = 0; i < inputOutput.length; i++) {
      inputOutput[i] = result[i];
    }
  }

  changeBasis() {
    let newX = this.mousePositionScreen[0] - this.shiftScreenCoordinates[0];
    let newY = this.mousePositionScreen[1] - this.shiftScreenCoordinates[1];
    if (newX == 0 && newY == 0) {
      return;
    }
    let oldX = this.projectionSelectedAtSelection[0] - this.shiftScreenCoordinates[0];
    let oldY = this.projectionSelectedAtSelection[1] - this.shiftScreenCoordinates[1];
    newX /= this.graphicsUnit;
    newY /= this.graphicsUnit;
    oldX /= this.graphicsUnit;
    oldY /= this.graphicsUnit;
    if (newX * newX + newY * newY <= 0.03) {
      return;
    }
    this.angleScreenChange = - getAngleScreenChange(newX, newY, oldX, oldY);
    if (
      this.angleScreenChange > Math.PI / 2 ||
      this.angleScreenChange < - Math.PI / 2
    ) {
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
    let selectedVector = this.basisCircles[this.selectedBasisIndex];
    let xCuttingOld = this.scalarProduct(
      selectedVector,
      this.vProjectionNormalizedSelected,
    );
    let yCuttingOld = this.scalarProduct(selectedVector, this.vOrthogonalSelected);
    let hypothenuseSquared = xCuttingOld * xCuttingOld + yCuttingOld * yCuttingOld;

    let xCuttingNew = Math.sqrt(newX * newX + newY * newY);
    if (xCuttingNew * xCuttingNew > hypothenuseSquared) {
      xCuttingNew = Math.sqrt(hypothenuseSquared) - 0.01;
    }
    let yCuttingNew = Math.sqrt(hypothenuseSquared - xCuttingNew * xCuttingNew);
    //if (newX * oldX + newY * oldY < 0) {
    //  xCuttingNew *= - 1;
    //}
    this.angleCuttingChange = - Math.atan2(
      yCuttingNew, xCuttingNew
    ) + Math.atan2(
      yCuttingOld, xCuttingOld
    );
    for (let i = 0; i < 5; i++) {
      if (this.angleCuttingChange <= - Math.PI) {
        this.angleCuttingChange += 2 * Math.PI;
      }
      if (this.angleCuttingChange > Math.PI) {
        this.angleCuttingChange -= 2 * Math.PI;
      }
      if (
        this.angleCuttingChange >= 0 &&
        this.angleCuttingChange <= Math.PI
      ) {
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
    let rect = this.canvasContainer.getBoundingClientRect();
    this.mousePositionScreen[0] -= rect.left;
    this.mousePositionScreen[1] -= rect.top;
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

    let epsilon = 0.0000001;
    for (let round = 0; round < this.dimension + 1; round++) {
      let selectedVector = this.basisCircles[this.selectedBasisIndex].slice();
      if (round > 0) {
        selectedVector[(round + 1) % this.dimension] += 0.01;
      }
      let vectorTimesE1 = this.scalarProduct(
        selectedVector,
        this.screenBasisAtSelection[0],
      );
      let vectorTimesE2 = this.scalarProduct(
        selectedVector,
        this.screenBasisAtSelection[1],
      );
      this.vProjectionNormalizedSelected = this.screenBasisAtSelection[0].slice();
      multiplyVectorByScalar(this.vProjectionNormalizedSelected, vectorTimesE1);
      addVectorTimesScalar(
        this.vProjectionNormalizedSelected,
        this.screenBasisAtSelection[1],
        vectorTimesE2,
      );
      this.vOrthogonalSelected = selectedVector.slice();
      addVectorTimesScalar(this.vOrthogonalSelected, this.vProjectionNormalizedSelected, - 1);
      let vOrthogonalSquareLength = this.scalarProduct(
        this.vOrthogonalSelected,
        this.vOrthogonalSelected,
      );
      let vProjectionSquareLength = this.scalarProduct(
        this.vProjectionNormalizedSelected,
        this.vProjectionNormalizedSelected,
      );
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
    for (let i = 0; i < this.basisCircles.length; i++) {
      if (this.pointsAreWithinClickTolerance(
        this.mousePositionScreenClicked[0], this.mousePositionScreenClicked[1],
        this.projectionsBasisCircles[i][0], this.projectionsBasisCircles[i][1]
      )) {
        this.selectIndex(i);
      }
    }
  }

  computeHighlightedIndex() {
    let oldSelected = this.selectedHighlightIndex;
    this.selectedHighlightIndex = - 1;
    this.currentHighlightIndices = {};
    for (let i = 0; i < this.indicesHighlightOperations.length; i++) {
      let currentIndex = this.indicesHighlightOperations[i];
      let currentHighlight = this.drawOperations[currentIndex];
      currentHighlight.computeProjections();
      let currentVectors = currentHighlight.vectorProjections;
      for (let j = 0; j < currentVectors.length; j++) {
        if (this.pointsAreWithinClickTolerance(
          this.mousePositionScreen[0],
          this.mousePositionScreen[1],
          currentVectors[j][0],
          currentVectors[j][1],
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
    let doRedraw = this.computeHighlightedIndex();
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
      let deltaX = this.mousePositionScreen[0] - this.mousePositionScreenClicked[0];
      let deltaY = this.mousePositionScreen[1] - this.mousePositionScreenClicked[1];
      this.shiftScreenCoordinates[0] = this.shiftScreenCoordinatesClicked[0] + deltaX;
      this.shiftScreenCoordinates[1] = this.shiftScreenCoordinatesClicked[1] + deltaY;
    } else if (this.selectedBasisIndex >= 0) {
      this.changeBasis();
      doRedraw = true;
    }
    if (doRedraw) {
      this.drawAll();
    }
  }

  mouseHandleWheel(event) {
    event = event ? event : window.event;
    event.preventDefault();
    event.stopPropagation();
    let wheelDelta = event.detail ? event.detail * - 1 : event.wheelDelta / 40;
    this.computePosXPosY(event);

    let oldXDelta = (this.mousePositionScreen[0] - this.shiftScreenCoordinates[0]);
    let oldYDelta = (this.mousePositionScreen[1] - this.shiftScreenCoordinates[1]);
    let oldXDeltaScaled = oldXDelta / this.graphicsUnit;
    let oldYDeltaScaled = oldYDelta / this.graphicsUnit;
    this.graphicsUnit += wheelDelta;
    if (this.graphicsUnit < 3) {
      this.graphicsUnit = 3;
    }
    let newXDelta = oldXDeltaScaled * this.graphicsUnit;
    let newYDelta = oldYDeltaScaled * this.graphicsUnit;
    this.shiftScreenCoordinates[0] += oldXDelta - newXDelta;
    this.shiftScreenCoordinates[1] += oldYDelta - newYDelta;
    this.drawAll();
  }

  initializeFromObject(input) {
    this.dimension = input.dimension;
    this.bilinearForm = input.bilinearForm;
    this.screenBasis = input.screenBasis;
    this.basisCircles = input.draggablePoints;
    if (input.frameLength !== undefined && input.frameLength !== null) {
      this.animation.frameLength = input.frameLength;
    }
    this.layers = {};
    this.layerOfEachDrawOperation = [];
    for (let i = 0; i < input.drawObjects.length; i++) {
      let currentOperation = input.drawObjects[i];
      this.drawOneOperation(currentOperation);
      this.accountLayer(currentOperation, i);
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

  initializeLayers() {
    if (this.containerLayers === null) {
      this.containerLayers = document.createElement("div");
      this.containerControls.appendChild(this.containerLayers);
    }
    this.containerLayers.textContent = "";
    for (const label in this.layers) {
      if (label === "") {
        continue;
      }
      const layer = this.layers[label];
      const child = document.createElement("input");
      child.type = "checkbox";
      child.checked = true;
      child.addEventListener(
        "change", () => {
          layer.visible = child.checked;
          this.drawAll();
        });
      this.containerLayers.appendChild(child);
      this.containerLayers.appendChild(document.createTextNode(layer.name));
    }
  }

  accountLayer(
    input,
    /** @type{number} */
    index,
  ) {
    let layerLabel = input["layer"];
    if (layerLabel === undefined ) {
      layerLabel = "";
    }
    if (!(layerLabel in this.layers)) {
      this.layers[layerLabel] = new Layer(layerLabel);
    }
    const layer = this.layers[layerLabel];
    layer.addIndex(index);
    this.layerOfEachDrawOperation.push(layer);
  }

  drawOneOperation(input) {
    switch (input.operation) {
      case "circleAtVector":
        this.drawCircle(
          input.location,
          input.color,
          input.radius,
          input.frameId,
          input.frameIndex
        );
        return;
      case "segment":
        this.drawLine(
          input.points[0],
          input.points[1],
          input.color,
          input,
        );
        return;
      case "highlightGroup":
        this.drawHighlightGroup(
          input.points, input.labels,
          input.color, input.radius
        );
        return;
      case "text":
        this.drawText(
          input.location,
          input.text,
          input.color,
        );
        return;
      case "path":
        this.drawPath(
          input.points,
          input.color,
          input.lineWidth,
          input.frameId,
          input.frameIndex
        );
        return;
      default:
        throw new Error(`Unknown object type: ${input.operation}`);
    }
  }

  animate() {
    clearTimeout(this.animation.timeoutHandle);
    this.animation.currentFrameIndex++;
    if (
      this.animation.currentFrameIndex < 0 ||
      this.animation.currentFrameIndex > this.animation.numberOfFrames
    ) {
      this.animation.currentFrameIndex = 0;
    }
    this.drawAll();
    this.animation.timeoutHandle = setTimeout(
      this.animate.bind(this),
      this.animation.frameLength,
    );
  }
}

class DrawHighlights {
  constructor(
    /** @type {GraphicsNDimensions} */
    inputOwner,
    inputData,
  ) {
    this.owner = inputOwner;
    this.indexInOperations = inputData.indexInOperations;
    this.owner.indicesHighlightOperations.push(this.indexInOperations);
    this.vectors = inputData.vectors;
    this.vectorProjections = [];
    this.color = inputData.color;
    this.labels = inputData.labels;
    this.radius = inputData.radius;
  }

  computeProjections() {
    if (this.vectorProjections.length !== this.vectors.length) {
      this.vectorProjecions = new Array(this.vectors.length);
    }
    for (let i = 0; i < this.vectors.length; i++) {
      if (
        this.vectorProjections[i] === undefined ||
        this.vectorProjections[i] === null
      ) {
        this.vectorProjections[i] = [];
      }
      this.owner.computeScreenCoordinates(
        this.vectors[i],
        this.vectorProjections[i],
      );
    }
  }

  drawNoFinish() {
    /** @type{GraphicsNDimensions} */
    if (!(this.indexInOperations in this.owner.currentHighlightIndices)) {
      return;
    }
    let currentIndices = this.owner.currentHighlightIndices[
      this.indexInOperations
    ];
    let nextLine = [];
    for (let i = 0; i < this.labels.length; i++) {
      if (i in currentIndices) {
        nextLine.push(`<b>${this.labels[i]}</b>`);
      } else {
        nextLine.push(this.labels[i]);
      }
    }
    this.owner.highlightInfoContent.push(nextLine.join(", "));

    this.computeProjections();
    let canvas = this.owner.canvas;
    canvas.strokeStyle = this.color;
    for (let i = 0; i < this.vectors.length; i++) {
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

  getLaTeXOperation() {
    return '%javascript highlighting not suitable for latex';
  }

  accountBoundingBox() {  
    // Nothing to do.
  }
}

class DrawPath {
  constructor(
    /** @type{GraphicsNDimensions} */
    inputOwner, 
    inputData,
  ) {
    /** @type{GraphicsNDimensions} */
    this.owner = inputOwner;
    this.points = inputData.points;
    this.lineWidth = inputData.lineWidth;
    this.colorLine = inputData.colorLine;
    if (
      inputData.frameId !== "" &&
      inputData.frameId !== null &&
      inputData.frameId !== undefined
    ) {
      this.frameId = inputData.frameId;
      this.frameIndex = inputData.frameIndex;
      this.owner.registerFrameIndex(this.frameIndex);
    }
  }

  drawNoFinish() {
    let canvas = this.owner.canvas;
    canvas.beginPath();
    canvas.strokeStyle = this.colorLine;
    canvas.lineWidth = this.lineWidth;
    let vector = [];
    this.owner.computeScreenCoordinates(this.points[0], vector);
    canvas.moveTo(vector[0], vector[1]);
    for (let i = 1; i < this.points.length; i++) {
      this.owner.computeScreenCoordinates(this.points[i], vector);
      canvas.lineTo(vector[0], vector[1]);
    }
    canvas.stroke();
  }

  getLaTeXOperation() {
    return '%not implemented';
  }

  accountBoundingBox() {
    for (let i = 0; i < this.points.length; i++){
      this.owner.accountVectorInBoundingBox(this.points[i]);
    }
  }
}

class DrawSegmentBetweenTwoVectors {
  constructor(
    /** @type{GraphicsNDimensions} */
    inputOwner, 
    inputData,
  ) {
    /** @type{GraphicsNDimensions} */
    this.owner = inputOwner;
    this.left = inputData.left.slice();
    this.right = inputData.right.slice();
    this.lineWidth = inputData.lineWidth;
    this.colorLine = inputData.colorLine;
    if (this.colorLine === undefined) {
      this.colorLine = "";
    }
    this.penStyle = inputData.penStyle;
    if (this.penStyle === undefined) {
      this.penStyle = "";
    }
  }

  drawNoFinish() {
    let canvas = this.owner.canvas;
    canvas.beginPath();
    canvas.strokeStyle = this.colorLine;
    canvas.lineWidth = this.lineWidth;
    let screenCoordinatesLeft = [];
    let screenCoordinatesRight = [];
    this.owner.computeScreenCoordinates(this.left, screenCoordinatesLeft);
    this.owner.computeScreenCoordinates(this.right, screenCoordinatesRight);
    if (this.penStyle === "dashed") {
      canvas.setLineDash([3, 4]);
    }
    canvas.moveTo(screenCoordinatesLeft[0], screenCoordinatesLeft[1]);
    canvas.lineTo(screenCoordinatesRight[0], screenCoordinatesRight[1]);
    canvas.stroke();
    if (this.penStyle !== "") {
      canvas.setLineDash([]);
    }
  }

  getLaTeXOptions() {
    if (this.colorLine === "") {
      return "";
    }
    let options = `linecolor=${correctColor(this.colorLine)}`;
    if (this.penStyle !== "") {
      options += `, linestyle=${this.penStyle}`;
    }
    if (options !== "") {
      options = `[${options}]`;
    } 
    return options;
  }

  getLaTeXOperation() {
    let leftCoordinates = [0, 0];
    let rightCoordinates = [0, 0];
    this.owner.computeLatexCoordinates(this.left, leftCoordinates);
    this.owner.computeLatexCoordinates(this.right, rightCoordinates);
    return `\\psline${this.getLaTeXOptions()}(${leftCoordinates[0]},${leftCoordinates[1]})` +
      `(${rightCoordinates[0]},${rightCoordinates[1]})`;
  }

  accountBoundingBox() {
    this.owner.accountVectorInBoundingBox(this.left);
    this.owner.accountVectorInBoundingBox(this.right);
  }
}

class DrawFilledShape {
  constructor(
    /** @type{GraphicsNDimensions} */
    inputOwner, inputData,
  ) {
    this.points = inputData.points;
    this.owner = inputOwner;
    this.colorFill = inputData.colorFill;
    this.lineWidth = inputData.lineWidth;
  }

  drawNoFinish() {
    let canvas = this.owner.canvas;
    canvas.beginPath();
    canvas.fillStyle = this.colorFill;
    canvas.lineWidth = this.lineWidth;
    let currentPoint = [];
    this.owner.computeScreenCoordinates(this.points[0], currentPoint);
    canvas.moveTo(currentPoint[0], currentPoint[1]);
    for (
      let counterPoint = 1;
      counterPoint < this.points.length;
      counterPoint++
    ) {
      this.owner.computeAllScreenCoordinates(
        this.points[counterPoint],
        currentPoint
      );
      canvas.lineTo(currentPoint[0], currentPoint[1]);
    }
    canvas.closePath();
    canvas.fill();
  }
}

class DrawCircleAtVector {
  constructor(
    /** @type{GraphicsNDimensions} */
    inputOwner, 
    inputData,
  ) {
    /** @type{GraphicsNDimensions} */
    this.owner = inputOwner;
    this.location = inputData.location.slice();
    this.locationScreenCoordinates = [];
    this.colorFill = inputData.colorFill;
    this.radius = inputData.radius;
    if (inputData.frameId !== undefined && inputData.frameId !== null) {
      this.frameId = inputData.frameId;
      this.frameIndex = inputData.frameIndex;
      this.owner.registerFrameIndex(this.frameIndex);
    }
  }

  drawNoFinish() {
    let canvas = this.owner.canvas;
    canvas.strokeStyle = this.colorFill;
    canvas.fillStyle = this.colorFill;
    this.owner.computeScreenCoordinates(
      this.location,
      this.locationScreenCoordinates,
    );
    canvas.beginPath();
    canvas.arc(
      this.locationScreenCoordinates[0],
      this.locationScreenCoordinates[1],
      this.radius,
      0, 2 * Math.PI
    );
    canvas.fill();
  }

  getLaTeXOperation() {
    let coordinates = [0, 0];
    this.owner.computeLatexCoordinates(this.location, coordinates);
    let rescaledRadius = 0.03 * this.radius;
    return `\\pscircle*(${coordinates[0]},${coordinates[1]}){${rescaledRadius}}`;
  }

  accountBoundingBox() {
    this.owner.accountVectorInBoundingBox(this.location);
  }
}

class DrawTextAtVector {
  /** @type{GraphicsNDimensions} */
  constructor(
    /** @type{GraphicsNDimensions} */
    inputOwner, 
    inputData,
  ) {
    this.owner = inputOwner;
    /** @type{string} */
    this.text = inputData.text;
    this.location = inputData.location;
    this.locationScreen = [];
    this.colorFill = inputData.colorFill;
  }

  drawNoFinish() {
    let canvas = this.owner.canvas;
    canvas.strokeStyle = this.colorFill;
    this.owner.computeScreenCoordinates(this.location, this.locationScreen);
    canvas.strokeText(
      this.text, 
      this.locationScreen[0] + this.owner.textShift[0], 
      this.locationScreen[1] + this.owner.textShift[1],
    );
  }

  getLaTeXOperation() {
    let coordinates = [0, 0];
    this.owner.computeLatexCoordinates(this.location, coordinates);
    let correctedText = this.text;
    if (correctedText.includes("_")) {
      // The input is latex.
      correctedText = `$${correctedText}$`;
    }
    let width = this.owner.boundingBoxLatex[1][0] - this.owner.boundingBoxLatex[0][0];
    let height = this.owner.boundingBoxLatex[1][1] - this.owner.boundingBoxLatex[0][1];
    let xOffset = coordinates[0] - this.owner.boundingBoxLatex[0][0];
    let yOffset = coordinates[1] - this.owner.boundingBoxLatex[0][1];
    let options = "";
    let widthRatio = xOffset / width;
    let heightRatio = yOffset / height;
    if (widthRatio < 0.33) {
      options += "r";
    } else if (widthRatio > 0.67) {
      options += "l";
    }
    if (heightRatio < 0.33) {
      options += "t";
    } else if (heightRatio > 0.67) {
      options += "b";
    }
    if (options !== "") {
      options = `[${options}]`;
    }
    return `\\rput${options}(${coordinates[0]},${coordinates[1]}){${correctedText}}`;
  }

  accountBoundingBox() {
    this.owner.accountVectorInBoundingBox(this.location);
  }
}

function multiplyVectorByScalar(vector, scalar) {
  for (let i = 0; i < vector.length; i++) {
    vector[i] *= scalar;
  }
}

function addVectorTimesScalar(vector, other, scalar) {
  for (let i = 0; i < vector.length; i++) {
    vector[i] += other[i] * scalar;
  }
}

function createGraphicsFromObject(input) {
  if (input.idCanvas === undefined || input.idCanvas === null) {
    throw ("idCanvas missing.");
  }
  let spanInformationId = input.idSpanInformation;
  if (spanInformationId == undefined) {
    spanInformationId = null;
  }
  let object = new GraphicsNDimensions(
    input.idCanvas,
    input.idControls,
    spanInformationId,
    input.idHighlightInformation,
  );
  object.initializeFromObject(input);
}

function testA3(idCanvas, idSpanInformation) {
  let a3 = new GraphicsNDimensions(idCanvas, idSpanInformation);
  a3.dimension = 3;
  a3.bilinearForm = [
    [2, -1, 0],
    [-1, 2, -1],
    [0, -1, 2],
  ];
  a3.screenBasis = [
    [0.707107, 0.707107, 0],
    [0, 0.707107, 0.707107],
  ];
  let labeledVectors = [
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
  let segments = [
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
    [[0, 0, -1], [0, 1, 0]],
  ];
  a3.drawStandardEiBasis("red");
  a3.drawText([1, 0, 0], "(1, 0, 0)");
  for (let i = 0; i < labeledVectors.length; i++) {
    a3.drawLine([0, 0, 0], labeledVectors[i], "green");
  }
  for (let i = 0; i < segments.length; i++) {
    a3.drawLine(segments[i][0], segments[i][1], "blue");
  }
  a3.graphicsUnit = 150;
  a3.drawAll();
}

function testA4(idCanvas, idSpanInformation) {
  let a4 = new GraphicsNDimensions(idCanvas, idSpanInformation);
  a4.dimension = 4;
  a4.bilinearForm = [
    [2, -1, 0, 0],
    [-1, 2, -1, 0],
    [0, -1, 2, -1],
    [0, 0, -1, 2],
  ];
  a4.screenBasis = [
    [-0.195, 0.316, 0.828, 0.632],
    [0.602, 0.973, 0.602, 0],
  ];
  let labeledVectors = [
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
  let numberOfPositiveVectors = labeledVectors.length;
  for (let i = 0; i < numberOfPositiveVectors; i++) {
    let vector = labeledVectors[i].slice();
    multiplyVectorByScalar(vector, - 1);
    labeledVectors.push(vector);
  }
  let segments = [];
  for (let i = 0; i < labeledVectors.length; i++) {
    let minDistance = 10;
    for (let j = i + 1; j < labeledVectors.length; j++) {
      let newDistance = a4.scalarProduct(labeledVectors[i], labeledVectors[j]);
      if (newDistance < minDistance && newDistance > 0) {
        minDistance = newDistance;
      }
    }
    for (let j = i + 1; j < labeledVectors.length; j++) {
      let distance = a4.scalarProduct(labeledVectors[i], labeledVectors[j]);
      if (distance != minDistance) {
        continue;
      }
      segments.push([labeledVectors[i].slice(), labeledVectors[j].slice()]);
    }
  }

  a4.drawStandardEiBasis("red");
  for (let i = 0; i < labeledVectors.length; i++) {
    a4.drawLine([0, 0, 0, 0], labeledVectors[i], "green");
  }
  for (let i = 0; i < segments.length; i++) {
    a4.drawLine(segments[i][0], segments[i][1], "blue");
  }
  a4.graphicsUnit = 150;
  a4.drawAll();
}

var module;
if (module === undefined) {
  module = {};
}

if (window.calculator === undefined) {
  window.calculator = {};
}

if (window.calculator.graphicsNDimensions === undefined) {
  window.calculator.graphicsNDimensions = {
    createGraphicsFromObject: createGraphicsFromObject,
    GraphicsNDimensions: GraphicsNDimensions,
  };
}

module.exports = {
  createGraphicsFromObject,
  GraphicsNDimensions,
  testA3,
  testA4,
};
