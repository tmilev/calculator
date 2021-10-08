// Licensed to you under the Apache 2.0 license.
//
// Documentation of the graphics can be found in:
//
// https://calculator-algebra.org:8166/calculator_html/test_graphics.html
//
// The source code can be found in:
//
// https://github.com/tmilev/calculator
//
// The formatting in this file follows
// an auto-code formatting associated with the closure compiler.
//
// We are not using 'use strict' as that creates
// a "uselessCode" compiler error with
// the closure compiler.

/**
 * Converts a vector to string.
 * @param {Array.<number>} vector to be converted to string
 * @returns{string}
 */
function vectorToString(vector) {
  let result = '[';
  for (let i = 0; i < vector.length; i++) {
    result += vector[i].toFixed(2);
    if (i != vector.length - 1) {
      result += ', ';
    }
  }
  result += ']';
  return result;
}

/**
 * Scalar product of two vectors.
 * @param {Array.<number>} s the first vector
 * @param {Array.<number>} t the second vector
 * @returns{number}
 */
function vectorScalarVector(s, t) {
  let result = 0;
  if (s.length !== t.length) {
    drawing.calculatorError(
      `Scalar product of vectors of different length: ${s} and ${t}.`);
  }
  for (let i = 0; i < s.length; i++) {
    result += s[i] * t[i];
  }
  return result;
}

function vectorTimesScalar(vector, s) {
  for (let i = 0; i < vector.length; i++) {
    vector[i] *= s;
  }
}

/** @returns{Array.<number>}*/
function vectorPlusVector(left, right) {
  let output = new Array(left.length);
  for (let i = 0; i < left.length; i++) {
    output[i] = left[i] + right[i];
  }
  return output;
}

/** @returns {Array.<number>} */
function vectorCrossVector(left, right) {
  return [
    left[1] * right[2] - left[2] * right[1],
    -left[0] * right[2] + left[2] * right[0],
    left[0] * right[1] - left[1] * right[0]
  ];
}

/** @returns {Array.<number>} */
function vectorMinusVector(left, right) {
  let output = new Array(left.length);
  for (let i = 0; i < left.length; i++) {
    output[i] = left[i] - right[i];
  }
  return output;
}

function vectorAddVectorTimesScalar(output, otherVector, scalar) {
  for (let i = 0; i < output.length; i++) {
    output[i] += otherVector[i] * scalar;
  }
}

function vectorLength(vector) {
  return Math.sqrt(vectorScalarVector(vector, vector));
}

function vectorRound(vector) {
  for (let i = 0; i < vector.length; i++) {
    vector[i] = Math.round(vector[i]);
  }
}

function vectorNormalize(vector) {
  vectorTimesScalar(vector, 1 / vectorLength(vector));
}

/**
 * Returns true if the vector is the zero vector,
 * false otherwise.
 * @returns {boolean}
 * */
function vectorIsZero(
  /**@type{Array.<number>} */
  vector,
) {
  for (let i = 0; i < vector.length; i++) {
    if (vector[i] !== 0) {
      return false;
    }
  }
  return true;
}

function getPosXPosYObject(theObject, cx, cy) {
  let rectangle = theObject.getBoundingClientRect();
  return [cx - rectangle.left, cy - rectangle.top];
}

class CurveThreeD {
  constructor(
      /** @type{Array.<Function>}*/ inputCoordinateFunctions, inputLeftPt,
    inputRightPt, inputNumSegments, inputColor, inputLineWidth) {
    /** @type{Array.<Function>}*/
    this.coordinateFunctions = inputCoordinateFunctions;
    /** @type{number} */
    this.leftPt = inputLeftPt;
    /** @type{number} */
    this.rightPt = inputRightPt;
    /** @type{Array.<number>} */
    this.color = colorToRGB(inputColor);
    /** @type{number} */
    this.numSegments = inputNumSegments;
    this.lineWidth = inputLineWidth;
  }

  accountBoundingBox(inputOutputBox) {
    let argumentT = this.leftPt;
    let x = this.coordinateFunctions[0](argumentT);
    let y = this.coordinateFunctions[1](argumentT);
    accountBoundingBox([x, y], inputOutputBox);
    for (let i = 0; i < this.numSegments; i++) {
      let ratio = i / (this.numSegments - 1);
      argumentT = this.leftPt * (1 - ratio) + this.rightPt * ratio;
      x = this.coordinateFunctions[0](argumentT);
      y = this.coordinateFunctions[1](argumentT);
      accountBoundingBox([x, y], inputOutputBox);
    }
  }

  drawNoFinish(theCanvas, startByMoving) {
    let theSurface = theCanvas.surface;
    theSurface.strokeStyle = colorRGBToString(this.color);
    theSurface.fillStyle = colorRGBToString(this.color);
    let argumentT = this.leftPt;
    let x = this.coordinateFunctions[0](argumentT);
    let y = this.coordinateFunctions[1](argumentT);
    let theCoords = theCanvas.coordinatesMathToScreen([x, y]);
    if (startByMoving) {
      theSurface.moveTo(theCoords[0], theCoords[1]);
    } else {
      theSurface.lineTo(theCoords[0], theCoords[1]);
    }
    let skippedValues = false;
    for (let i = 0; i < this.numSegments; i++) {
      let ratio = i / (this.numSegments - 1);
      argumentT =
        this.leftPt * (1 - ratio) + this.rightPt * ratio;  //<- this way of
      // computing x introduces smaller numerical errors.
      // For example, suppose you plot sqrt(1-x^2) from - 1 to 1.
      // If not careful with rounding errors,
      // you may end up evaluating sqrt(1-x^2) for x =1.00000000000004
      // resulting in serious visual glitches.
      // Note: the remarks above were discovered the painful way (trial and
      // error).
      x = this.coordinateFunctions[0](argumentT);
      y = this.coordinateFunctions[1](argumentT);
      if (!isFinite(y) || !isFinite(x)) {
        console.log(
          `Failed to evaluate: ${this.coordinateFunctions} at x = ${x}`);
      }
      if (Math.abs(y) > 100000 || Math.abs(x) > 100000) {
        if (!skippedValues) {
          console.log('Curve point: ' + [
            x, y
          ] + ' is too large, skipping. Further errors suppressed.');
        }
        skippedValues = true;
        continue;
      }
      theCoords = theCanvas.coordinatesMathToScreen([x, y]);
      theSurface.lineTo(theCoords[0], theCoords[1]);
    }
  }

  draw(theCanvas) {
    let theSurface = theCanvas.surface;
    theSurface.beginPath();
    this.drawNoFinish(theCanvas, true);
    theSurface.stroke();
  }
}

class Surface {
  constructor(
    /**@type{Function} */
    inputxyzFun,
    /**@type{Array.<Array.<number>>} Variable ranges, in format [[uMin, uMax],
       [vMin, vMax]]*/
    inputUVBox,
    /**@type{Array.<number>} Number of patches in the format [uPatchCount,
       vPathCount]*/
    inputPatchDimensions,
    /**@type{string} */
    colorFront,
    /**@type{string} */
    colorBack,
    /**@type{string} */
    colorContour,
    /**@type{number} */
    inputContourWidth,
  ) {
    this.xyzFun = inputxyzFun;
    this.uvBox = inputUVBox;
    this.patchDimensions = inputPatchDimensions;
    this.colors = {
      colorContour: colorContour,
      colorUV: colorToHex(colorBack),
      colorVU: colorToHex(colorFront),
    };
    this.contourWidth = inputContourWidth;
    this.deltaU =
      (this.uvBox[0][1] - this.uvBox[0][0]) / this.patchDimensions[0];
    this.deltaV =
      (this.uvBox[1][1] - this.uvBox[1][0]) / this.patchDimensions[1];
    this.numSamplesUSegment = 10;
    this.numSamplesVSegment = 10;
  }
}

class Point {
  constructor(inputLocation, inputColor) {
    this.location = inputLocation;
    /** @type{Array.<number>} */
    this.color = colorToRGB(inputColor);
  }
}

class Patch {
  constructor(
      /** @type{Array.<number>}*/ inputBase,
      /** @type{Array.<number>}*/ inputEdge1,
      /** @type{Array.<number>}*/ inputEdge2, inputColorUV, inputColorVU) {
    /** @type{Array.<number>}*/
    this.base = inputBase.slice();
    /** @type{Array.<number>}*/
    this.edge1 = inputEdge1.slice();
    /** @type{Array.<number>}*/
    this.edge2 = inputEdge2.slice();
    this.colorUV = colorToRGB(inputColorUV);
    this.colorVU = colorToRGB(inputColorVU);
    /** @type{Array.<number>}*/
    this.v1 = vectorPlusVector(this.base, this.edge1);
    this.v2 = vectorPlusVector(this.base, this.edge2);
    this.vEnd = vectorPlusVector(this.v1, this.edge2);
    this.internalPoint = this.base.slice();
    vectorAddVectorTimesScalar(this.internalPoint, this.edge1, 0.5);
    vectorAddVectorTimesScalar(this.internalPoint, this.edge2, 0.5);
    this.normalVector = vectorCrossVector(inputEdge1, inputEdge2);
    this.adjacentContours = [];
    this.traversalOrder = [];
    this.patchesBelowMe = [];
    this.patchesAboveMe = [];
    this.index = -1;
  }
}

class Contour {
  constructor(inputPoints, /**@type{string}*/ inputColor, inputLineWidth) {
    this.thePoints = inputPoints.slice();
    this.thePointsMathScreen = [];
    /** @type{Array.<number>} */
    this.color = colorToRGB(inputColor);
    this.adjacentPatches = [];
    this.index = -1;
    this.lineWidth = inputLineWidth;
  }
}

function colorRGBToHex(r, g, b) {
  return '#' + ((1 << 24) + (r << 16) + (g << 8) + b).toString(16).slice(1);
}

function colorRGBToString(input) {
  return `rgb(${input[0].toString()},${input[1].toString()},${input[2].toString()})`;
}

function colorScale(inputRGB, theScale) {
  let result = [0, 0, 0];
  for (let i = 0; i < inputRGB.length; i++) {
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

/** @returns{Array.<number>} */
function colorToRGB(
  /** @type{string|Array.<number>} */
  input,
) {
  if (typeof input !== 'string') {
    return input;
  }
  let hex = colorToHex(input);
  return [
    parseInt(hex.slice(1, 3), 16), parseInt(hex.slice(3, 5), 16),
    parseInt(hex.slice(5, 7), 16)
  ];
}

/** @returns{string} */
function colorToHex(
  /** @type{string} */
  color,
) {
  const colors = {
    'aliceblue': '#f0f8ff',
    'antiquewhite': '#faebd7',
    'aqua': '#00ffff',
    'aquamarine': '#7fffd4',
    'azure': '#f0ffff',
    'beige': '#f5f5dc',
    'bisque': '#ffe4c4',
    'black': '#000000',
    'blanchedalmond': '#ffebcd',
    'blue': '#0000ff',
    'blueviolet': '#8a2be2',
    'brown': '#a52a2a',
    'burlywood': '#deb887',
    'cadetblue': '#5f9ea0',
    'chartreuse': '#7fff00',
    'chocolate': '#d2691e',
    'coral': '#ff7f50',
    'cornflowerblue': '#6495ed',
    'cornsilk': '#fff8dc',
    'crimson': '#dc143c',
    'cyan': '#00ffff',
    'darkblue': '#00008b',
    'darkcyan': '#008b8b',
    'darkgoldenrod': '#b8860b',
    'darkgray': '#a9a9a9',
    'darkgreen': '#006400',
    'darkkhaki': '#bdb76b',
    'darkmagenta': '#8b008b',
    'darkolivegreen': '#556b2f',
    'darkorange': '#ff8c00',
    'darkorchid': '#9932cc',
    'darkred': '#8b0000',
    'darksalmon': '#e9967a',
    'darkseagreen': '#8fbc8f',
    'darkslateblue': '#483d8b',
    'darkslategray': '#2f4f4f',
    'darkturquoise': '#00ced1',
    'darkviolet': '#9400d3',
    'deeppink': '#ff1493',
    'deepskyblue': '#00bfff',
    'dimgray': '#696969',
    'dodgerblue': '#1e90ff',
    'firebrick': '#b22222',
    'floralwhite': '#fffaf0',
    'forestgreen': '#228b22',
    'fuchsia': '#ff00ff',
    'gainsboro': '#dcdcdc',
    'ghostwhite': '#f8f8ff',
    'gold': '#ffd700',
    'goldenrod': '#daa520',
    'gray': '#808080',
    'green': '#008000',
    'greenyellow': '#adff2f',
    'honeydew': '#f0fff0',
    'hotpink': '#ff69b4',
    'indianred ': '#cd5c5c',
    'indigo': '#4b0082',
    'ivory': '#fffff0',
    'khaki': '#f0e68c',
    'lavender': '#e6e6fa',
    'lavenderblush': '#fff0f5',
    'lawngreen': '#7cfc00',
    'lemonchiffon': '#fffacd',
    'lightblue': '#add8e6',
    'lightcoral': '#f08080',
    'lightcyan': '#e0ffff',
    'lightgoldenrodyellow': '#fafad2',
    'lightgrey': '#d3d3d3',
    'lightgreen': '#90ee90',
    'lightpink': '#ffb6c1',
    'lightsalmon': '#ffa07a',
    'lightseagreen': '#20b2aa',
    'lightskyblue': '#87cefa',
    'lightslategray': '#778899',
    'lightsteelblue': '#b0c4de',
    'lightyellow': '#ffffe0',
    'lime': '#00ff00',
    'limegreen': '#32cd32',
    'linen': '#faf0e6',
    'magenta': '#ff00ff',
    'maroon': '#800000',
    'mediumaquamarine': '#66cdaa',
    'mediumblue': '#0000cd',
    'mediumorchid': '#ba55d3',
    'mediumpurple': '#9370d8',
    'mediumseagreen': '#3cb371',
    'mediumslateblue': '#7b68ee',
    'mediumspringgreen': '#00fa9a',
    'mediumturquoise': '#48d1cc',
    'mediumvioletred': '#c71585',
    'midnightblue': '#191970',
    'mintcream': '#f5fffa',
    'mistyrose': '#ffe4e1',
    'moccasin': '#ffe4b5',
    'navajowhite': '#ffdead',
    'navy': '#000080',
    'oldlace': '#fdf5e6',
    'olive': '#808000',
    'olivedrab': '#6b8e23',
    'orange': '#ffa500',
    'orangered': '#ff4500',
    'orchid': '#da70d6',
    'palegoldenrod': '#eee8aa',
    'palegreen': '#98fb98',
    'paleturquoise': '#afeeee',
    'palevioletred': '#d87093',
    'papayawhip': '#ffefd5',
    'peachpuff': '#ffdab9',
    'peru': '#cd853f',
    'pink': '#ffc0cb',
    'plum': '#dda0dd',
    'powderblue': '#b0e0e6',
    'purple': '#800080',
    'red': '#ff0000',
    'rosybrown': '#bc8f8f',
    'royalblue': '#4169e1',
    'saddlebrown': '#8b4513',
    'salmon': '#fa8072',
    'sandybrown': '#f4a460',
    'seagreen': '#2e8b57',
    'seashell': '#fff5ee',
    'sienna': '#a0522d',
    'silver': '#c0c0c0',
    'skyblue': '#87ceeb',
    'slateblue': '#6a5acd',
    'slategray': '#708090',
    'snow': '#fffafa',
    'springgreen': '#00ff7f',
    'steelblue': '#4682b4',
    'tan': '#d2b48c',
    'teal': '#008080',
    'thistle': '#d8bfd8',
    'tomato': '#ff6347',
    'turquoise': '#40e0d0',
    'violet': '#ee82ee',
    'wheat': '#f5deb3',
    'white': '#ffffff',
    'whitesmoke': '#f5f5f5',
    'yellow': '#ffff00',
    'yellowgreen': '#9acd32',
  };
  if (typeof colors[color.toLowerCase()] != 'undefined') {
    return colors[color.toLowerCase()];
  }
  if (color.length === 3) {
    return colorRGBToHex(color[0], color[1], color[2]);
  }
  return color;
}

class PointsTwoD {
  constructor(
    /**@type{Array.<Array.<number>>} */
    inputPoints,
    inputColor,
  ) {
    this.location = inputPoints.slice();
    /** @type{Array.<number>} */
    this.color = colorToRGB(inputColor);
    this.type = 'points';
  }

  accountBoundingBox(inputOutputBox) {
    for (let i = 0; i < this.location.length; i++) {
      accountBoundingBox(this.location[i], inputOutputBox);
    }
  }

  draw(theCanvas) {
    let theSurface = theCanvas.surface;
    for (let i = 0; i < this.location.length; i++) {
      theSurface.beginPath();
      theSurface.strokeStyle = colorRGBToString(this.color);
      theSurface.fillStyle = colorRGBToString(this.color);
      let theCoords = theCanvas.coordinatesMathToScreen(this.location[i]);
      theSurface.arc(theCoords[0], theCoords[1], 3, 0, Math.PI * 2);
      theSurface.fill();
    }
  }
}

function accountBoundingBox(inputPoint, outputBox) {
  for (let i = 0; i < inputPoint.length; i++) {
    if (inputPoint[i] < outputBox[0][i]) {
      outputBox[0][i] = inputPoint[i];
    }
  }
  for (let i = 0; i < inputPoint.length; i++) {
    if (inputPoint[i] > outputBox[1][i]) {
      outputBox[1][i] = inputPoint[i];
    }
  }
}

class CurveTwoD {
  constructor(
    /** @type{Array.<Function>}*/
    inputCoordinateFunctions, inputLeftPt, inputRightPt, inputNumSegments,
    inputColor, inputLineWidth) {
    /** @type{Array.<Function>}*/
    this.coordinateFunctions = inputCoordinateFunctions;
    /** @type{number} */
    this.leftPt = inputLeftPt;
    /** @type{number} */
    this.rightPt = inputRightPt;
    /** @type{Array.<number>} */
    this.color = colorToRGB(inputColor);
    /** @type{number} */
    this.numSegments = inputNumSegments;
    this.lineWidth = inputLineWidth;
  }

  accountBoundingBox(inputOutputBox) {
    let argumentT = this.leftPt;
    let x = this.coordinateFunctions[0](argumentT);
    let y = this.coordinateFunctions[1](argumentT);
    accountBoundingBox([x, y], inputOutputBox);
    for (let i = 0; i < this.numSegments; i++) {
      let ratio = i / (this.numSegments - 1);
      argumentT = this.leftPt * (1 - ratio) + this.rightPt * ratio;
      x = this.coordinateFunctions[0](argumentT);
      y = this.coordinateFunctions[1](argumentT);
      accountBoundingBox([x, y], inputOutputBox);
    }
  }

  draw(theCanvas) {
    let theSurface = theCanvas.surface;
    theSurface.beginPath();
    this.drawNoFinish(theCanvas, true);
    theSurface.stroke();
  }

  drawNoFinish(theCanvas, startByMoving) {
    let theSurface = theCanvas.surface;
    theSurface.strokeStyle = colorRGBToString(this.color);
    theSurface.fillStyle = colorRGBToString(this.color);
    let argumentT = this.leftPt;
    let x = this.coordinateFunctions[0](argumentT);
    let y = this.coordinateFunctions[1](argumentT);
    let theCoords = theCanvas.coordinatesMathToScreen([x, y]);
    let alreadyMoved = false;
    if (startByMoving) {
      alreadyMoved = true;
    }
    theSurface.lineWidth = this.lineWidth;
    let skippedValues = false;
    for (let i = 0; i < this.numSegments; i++) {
      let ratio = i / (this.numSegments - 1);
      argumentT =
        this.leftPt * (1 - ratio) + this.rightPt * ratio;  //<- this way of
      // computing x introduces smaller numerical errors.
      // For example, suppose you plot sqrt(1-x^2) from - 1 to 1.
      // If not careful with rounding errors,
      // you may end up evaluating sqrt(1-x^2) for x =1.00000000000004
      // resulting in serious visual glitches.
      // Note: the remarks above were discovered the painful way (trial and
      // error).
      x = this.coordinateFunctions[0](argumentT);
      y = this.coordinateFunctions[1](argumentT);
      if (!isFinite(y) || !isFinite(x)) {
        console.log(
          `Failed to evaluate: ${this.coordinateFunctions} at x = ${x}`);
      }
      if (Math.abs(y) > 100000 || Math.abs(x) > 100000) {
        if (!skippedValues) {
          console.log(`Curve point: [${x}, ${y}] is too large, skipping. Further errors suppressed.`);
        }
        skippedValues = true;
        continue;
      }
      theCoords = theCanvas.coordinatesMathToScreen([x, y]);
      if (!alreadyMoved) {
        alreadyMoved = true;
        theSurface.moveTo(theCoords[0], theCoords[1]);
      } else {
        theSurface.lineTo(theCoords[0], theCoords[1]);
      }
    }
  }
}

class PathTwoD {
  constructor(inputPath, inputColor, inputFillColor, inputLineWidth) {
    /** @type{Array.<Array.<number>>} */
    this.path = inputPath;
    /** @type{Array.<number>} */
    this.color = colorToRGB(inputColor);
    this.colorFill = colorToRGB(inputFillColor);
    this.isFilled = false;
    this.type = 'path';
    this.lineWidth = inputLineWidth;
  }

  accountBoundingBox(inputOutputBox) {
    for (let i = 0; i < this.path.length; i++) {
      accountBoundingBox(this.path[i], inputOutputBox);
    }
  }

  drawNoFinish(theCanvas, startByMoving) {
    if (this.path.length < 1) {
      return;
    }
    let theSurface = theCanvas.surface;
    let theCoords = theCanvas.coordinatesMathToScreen(this.path[0]);
    if (startByMoving) {
      theSurface.moveTo(theCoords[0], theCoords[1]);
    } else {
      theSurface.lineTo(theCoords[0], theCoords[1]);
    }
    theSurface.lineWidth = this.lineWidth;
    for (let i = 1; i < this.path.length; i++) {
      theCoords = theCanvas.coordinatesMathToScreen(this.path[i]);
      theSurface.lineTo(theCoords[0], theCoords[1]);
    }
    theSurface.strokeStyle = colorRGBToString(this.color);
    theSurface.fillStyle = colorRGBToString(this.colorFill);
    if (this.isFilled) {
      theSurface.fill();
    }
  }

  draw(theCanvas) {
    if (this.path.length < 1) {
      return;
    }
    let theSurface = theCanvas.surface;
    theSurface.beginPath();
    this.drawNoFinish(theCanvas, true);
    theSurface.stroke();
  }
}

class DrawCoordinateAxesTwoD {
  constructor(
      /** @type{string} */ inputXAxisLabel,
      /** @type{string} */ inputYAxisLabel) {
    this.xAxisLabel = inputXAxisLabel;
    this.yAxisLabel = inputYAxisLabel;
  }

  draw(theCanvas) {
    this.drawNoFinish(theCanvas, true);
  }

  accountBoundingBox(inputOutputBox) {
    accountBoundingBox([0, 1], inputOutputBox);
    accountBoundingBox([1, 0], inputOutputBox);
  }

  drawNoFinish(theCanvas, startByMoving) {
    let lowLeft = theCanvas.coordsScreenToMathScreen([0, theCanvas.height]);
    let topRight = theCanvas.coordsScreenToMathScreen([theCanvas.width, 0]);
    let surface = theCanvas.surface;
    surface.strokeStyle = colorRGBToString([0, 0, 0]);
    surface.fillStyle = colorRGBToString([0, 0, 0]);
    surface.lineWidth = 1;
    let left = lowLeft[0];
    let right = topRight[0];
    let bottom = lowLeft[1];
    let top = topRight[1];
    surface.beginPath();
    let coordinates = theCanvas.coordsMathScreenToScreen([0, bottom]);
    surface.moveTo(coordinates[0], coordinates[1]);
    coordinates = theCanvas.coordsMathScreenToScreen([0, top]);
    surface.lineTo(coordinates[0], coordinates[1]);
    surface.stroke();
    surface.fillText(this.yAxisLabel, coordinates[0] - 10, coordinates[1] + 10);

    coordinates = theCanvas.coordsMathScreenToScreen([left, 0]);
    surface.moveTo(coordinates[0], coordinates[1]);
    coordinates = theCanvas.coordsMathScreenToScreen([right, 0]);
    surface.lineTo(coordinates[0], coordinates[1]);
    surface.stroke();
    surface.fillText(this.xAxisLabel, coordinates[0] - 10, coordinates[1] + 10);
    coordinates = theCanvas.coordsMathScreenToScreen([1, -0.1]);
    surface.moveTo(coordinates[0], coordinates[1]);
    coordinates = theCanvas.coordsMathScreenToScreen([1, 0.1]);
    surface.lineTo(coordinates[0], coordinates[1]);
    surface.stroke();
    coordinates = theCanvas.coordsMathScreenToScreen([1, 0]);
    surface.fillText('1', coordinates[0], coordinates[1] + 10);
  }
}

class AxesGrid {
  constructor() { }

  draw(theCanvas) {
    this.drawNoFinish(theCanvas, true);
  }

  drawNoFinish(theCanvas, startByMoving) {
    let lowLeft = theCanvas.coordsScreenToMathScreen([0, theCanvas.height]);
    let topRight = theCanvas.coordsScreenToMathScreen([theCanvas.width, 0]);
    let surface = theCanvas.surface;
    surface.strokeStyle = colorRGBToString([160, 160, 160]);
    surface.fillStyle = colorRGBToString([160, 160, 160]);
    surface.lineWidth = 0.1;
    let left = lowLeft[0];
    let right = topRight[0];
    let bottom = lowLeft[1];
    let top = topRight[1];
    let floorLeft = Math.floor(left);
    let ceilRight = Math.ceil(right);
    let floorBottom = Math.floor(bottom);
    let ceilTop = Math.ceil(top);
    let distanceBetweenMarks = 30;
    let DeltaHorizontal = Math.max(
      1,
      Math.floor(
        (ceilRight - floorLeft) /
        (theCanvas.width / distanceBetweenMarks)));
    let DeltaVertical = Math.max(
      1,
      Math.floor(
        (ceilTop - floorBottom) /
        (theCanvas.height / distanceBetweenMarks)));
    let Delta = Math.max(DeltaHorizontal, DeltaVertical);
    surface.beginPath();
    for (let i = floorLeft; i <= ceilRight; i += Delta) {
      let theCoords = theCanvas.coordsMathScreenToScreen([i, bottom]);
      surface.moveTo(theCoords[0], theCoords[1]);
      theCoords = theCanvas.coordsMathScreenToScreen([i, top]);
      surface.lineTo(theCoords[0], theCoords[1]);
      surface.stroke();
    }
    for (let i = floorBottom; i <= ceilTop; i += Delta) {
      let theCoords = theCanvas.coordsMathScreenToScreen([left, i]);
      surface.moveTo(theCoords[0], theCoords[1]);
      theCoords = theCanvas.coordsMathScreenToScreen([right, i]);
      surface.lineTo(theCoords[0], theCoords[1]);
      surface.stroke();
    }
    surface.strokeStyle = colorRGBToString([0, 0, 0]);
    surface.fillStyle = colorRGBToString([0, 0, 0]);
    let counter = 0;
    for (let i = floorLeft; i <= ceilRight; i += Delta) {
      counter++;
      // if (counter%2=== 0 && i !==1){
      let theCoords = theCanvas.coordsMathScreenToScreen([i, 0]);
      surface.fillText(i, theCoords[0], theCoords[1] + 10);
      //}
    }
    counter = 0;
    for (let i = floorBottom; i <= ceilTop; i += Delta) {
      counter++;
      // if (counter%2=== 0 && i !==1){
      let theCoords = theCanvas.coordsMathScreenToScreen([0, i]);
      surface.fillText(i, theCoords[0] - 10, theCoords[1]);
      //}
    }
  }
}

/** @returns{number} */
function numberFromNumberOfInfinity(/**@type{number|string} */ input) {
  if (typeof input === 'string') {
    let inputString = /**@type{string}*/ (input);
    if (inputString.toLowerCase() === 'infinity' ||
      inputString.toLowerCase() === 'minusInfinity') {
      return 0;
    }
    return parseInt(input, 10);
  }
  return /** @type{number} */ (input);
}

/**
 * @returns{string} If the input is a string that indicates infinity, returns
 * a normalized string representation. Otherwise returns the empty string.
 */
function infinityType(/** @type{number|string} */ input) {
  if (typeof input !== 'string') {
    return '';
  }
  /**@type{string} */
  let inputString = /**@type{string} */ (input);
  if (inputString.toLowerCase() === 'infinity') {
    return 'infinity';
  }
  if (inputString.toLowerCase() === 'minusinfinity') {
    return 'minusInfinity';
  }
  return '';
}

class PlotTwoD {
  constructor(
    /**@type{Function} */
    inputFunction,
    /**@type{number|string} */
    leftPoint,
    /**@type{number|string} */
    rightPoint,
    /**@type{number} */
    numberOfSegments,
    /**@type{string} */
    inputColor,
    /**@type{number} */
    inputLineWidth,
  ) {
    this.functionPlotted = inputFunction;
    /** @type{number} */
    this.leftPoint = numberFromNumberOfInfinity(leftPoint);
    /** @type{string} */
    this.leftInfinityType = infinityType(leftPoint);
    /** @type{number} */
    this.rightPoint = numberFromNumberOfInfinity(rightPoint);
    /** @type{string} */
    this.rightInfinityType = infinityType(rightPoint);
    /** @type{Array.<number>} */
    this.color = colorToRGB(inputColor);
    if (inputLineWidth === undefined) {
      this.lineWidth = 1;
    } else {
      this.lineWidth = inputLineWidth;
    }
    this.type = 'plotFunction';
    this.numberOfSegments = numberOfSegments;
  }

  accountBoundingBox(inputOutputBox) {
    for (let i = 0; i < this.numberOfSegments; i++) {
      let ratio = i / (this.numberOfSegments - 1);
      let x = this.leftPoint * (1 - ratio) + this.rightPoint * ratio;
      let y = this.functionPlotted(x);
      if (!isFinite(y)) {
        continue;
      }
      if (Math.abs(y) > 100000) {
        continue;
      }
      accountBoundingBox([x, y], inputOutputBox);
    }
  }

  drawNoFinish(theCanvas, startByMoving) {
    let theSurface = theCanvas.surface;
    theSurface.strokeStyle = colorRGBToString(this.color);
    theSurface.fillStyle = colorRGBToString(this.color);
    let realLeft = this.leftPoint;
    let realRight = this.rightPoint;
    if (this.leftInfinityType === 'minusInfinity') {
      let lowLeft = theCanvas.coordsScreenToMathScreen([0, theCanvas.height]);
      realLeft = lowLeft[0];
    }
    if (this.rightInfinityType === 'infinity') {
      let topRight = theCanvas.coordsScreenToMathScreen([theCanvas.width, 0]);
      realRight = topRight[0];
    }
    let x = realLeft;
    let y = this.functionPlotted(x);
    let theCoords = theCanvas.coordinatesMathToScreen([x, y]);
    let alreadyMoved = false;
    if (startByMoving) {
      alreadyMoved = true;
    }
    theSurface.lineWidth = this.lineWidth;
    let skippedValues = false;
    for (let i = 0; i < this.numberOfSegments; i++) {
      let ratio = i / (this.numberOfSegments - 1);
      x = realLeft * (1 - ratio) + realRight * ratio;  //<- this way of
      // computing x this way introduces smaller numerical errors.
      // For example, suppose you plot sqrt(1-x^2) from - 1 to 1.
      // If not careful with rounding errors,
      // you may end up evaluating sqrt(1-x^2) for x =1.00000000000004
      // resulting in serious visual glitches.
      // Note: the remarks above were discovered the painful way (trial and
      // error).
      y = this.functionPlotted(x);
      if (!isFinite(y)) {
        console.log(
          'Failed to evaluate: ' + this.functionPlotted + ' at x = ' + x);
      }
      if (Math.abs(y) > 10000) {
        if (!skippedValues) {
          console.log(
            'Function result: ' + y +
            ' is too large, skipping. Further errors suppressed.');
        }
        skippedValues = true;
        continue;
      }
      theCoords = theCanvas.coordinatesMathToScreen([x, y]);
      if (!alreadyMoved) {
        alreadyMoved = true;
        theSurface.moveTo(theCoords[0], theCoords[1]);
      } else {
        theSurface.lineTo(theCoords[0], theCoords[1]);
      }
    }
  }

  draw(theCanvas) {
    let theSurface = theCanvas.surface;
    theSurface.beginPath();
    this.drawNoFinish(theCanvas, true);
    theSurface.stroke();
  }
}

class TextPlotTwoD {
  constructor(inputLocation, inputText, inputColor) {
    /**@type{Array.<number>} */
    this.location = inputLocation;
    /**@type{string} */
    this.text = inputText;
    /**@type{Array.<number>} */
    this.color = colorToRGB(inputColor);
    this.type = 'plotText';
  }

  accountBoundingBox(inputOutputBox) {
    accountBoundingBox(this.location, inputOutputBox);
  }

  drawNoFinish(theCanvas) {
    let theSurface = theCanvas.surface;
    theSurface.strokeStyle = colorRGBToString(this.color);
    theSurface.fillStyle = colorRGBToString(this.color);
    let theCoords = theCanvas.coordinatesMathToScreen(this.location);
    theSurface.fillText(this.text, theCoords[0], theCoords[1]);
  }

  draw(theCanvas) {
    let theSurface = theCanvas.surface;
    theSurface.beginPath();
    this.drawNoFinish(theCanvas);
    theSurface.stroke();
  }
}

class VectorFieldTwoD {
  constructor(
    inputField,
    inputIsDirectionField,
    inputLowLeft,
    inputHighRight,
    /**@type{Array.<number>} */
    inputNumSegmentsXY,
    inputDesiredLengthDirectionVectors,
    inputColor,
    inputLineWidth,
  ) {
    /** @type{Function} */
    this.theField = inputField;
    /** @type{boolean} */
    this.isDirectionField = inputIsDirectionField;
    /** @type{number} */
    this.lowLeft = inputLowLeft;
    /** @type{number} */
    this.highRight = inputHighRight;
    this.numSegmentsXY = inputNumSegmentsXY;
    /** @type{number} */
    this.desiredLengthDirectionVectors = inputDesiredLengthDirectionVectors;
    /** @type{Array.<number>} */
    this.color = colorToRGB(inputColor);
    this.lineWidth = inputLineWidth;
  }

  accountBoundingBox(inputOutputBox) {
    accountBoundingBox(this.lowLeft, inputOutputBox);
    accountBoundingBox(this.highRight, inputOutputBox);
  }

  draw(theCanvas) {
    let theSurface = theCanvas.surface;
    theSurface.beginPath();
    theSurface.strokeStyle = colorRGBToString(this.color);
    theSurface.fillStyle = colorRGBToString(this.color);
    theSurface.lineWidth = this.lineWidth;
    for (let i = 0; i < this.numSegmentsXY[0]; i++) {
      let theRatioX = i / (this.numSegmentsXY[0] - 1);
      let x = this.lowLeft[0] * (1 - theRatioX) + this.highRight[0] * theRatioX;
      for (let j = 0; j < this.numSegmentsXY[1]; j++) {
        let theRatioY = j / (this.numSegmentsXY[1] - 1);
        let y =
          this.lowLeft[1] * (1 - theRatioY) + this.highRight[1] * theRatioY;
        let theV = this.theField(x, y);
        if (this.isDirectionField) {
          if (vectorLength(theV) !== 0) {
            vectorTimesScalar(
              theV,
              this.desiredLengthDirectionVectors * 1 / vectorLength(theV));
          }
        }
        let headMath = [x + theV[0] / 2, y + theV[1] / 2];
        let tailMath = [x - theV[0] / 2, y - theV[1] / 2];
        let headScreen = theCanvas.coordinatesMathToScreen(headMath);
        let tailScreen = theCanvas.coordinatesMathToScreen(tailMath);
        let baseScreen = theCanvas.coordinatesMathToScreen([x, y]);
        theSurface.moveTo(tailScreen[0], tailScreen[1]);
        theSurface.lineTo(headScreen[0], headScreen[1]);
        theSurface.stroke();
        // theSurface.moveTo(baseScreen[0], baseScreen[1]);
        // theSurface.arc(baseScreen[0], baseScreen[1],0.5, 0, Math.PI*2);
        // theSurface.fill();
      }
    }
  }
}

class SegmentTwoD {
  constructor(inputLeftPt, inputRightPt, inputColor, inputLineWidth) {
    /** @type{number} */
    this.leftPt = inputLeftPt;
    /** @type{number} */
    this.rightPt = inputRightPt;
    /** @type{Array.<number>} */
    this.color = colorToRGB(inputColor);
    this.type = 'segment';
    this.lineWidth = inputLineWidth;
  }

  accountBoundingBox(inputOutputBox) {
    accountBoundingBox(this.leftPt, inputOutputBox);
    accountBoundingBox(this.rightPt, inputOutputBox);
  }

  drawNoFinish(theCanvas, startByMoving) {
    let theSurface = theCanvas.surface;
    let theCoords = theCanvas.coordinatesMathToScreen(this.leftPt);
    theSurface.lineWidth = this.lineWidth;
    if (startByMoving) {
      theSurface.moveTo(theCoords[0], theCoords[1]);
    } else {
      theSurface.lineTo(theCoords[0], theCoords[1]);
    }
    theCoords = theCanvas.coordinatesMathToScreen(this.rightPt);
    theSurface.lineTo(theCoords[0], theCoords[1]);
  }

  draw(theCanvas) {
    let theSurface = theCanvas.surface;
    theSurface.beginPath();
    theSurface.strokeStyle = colorRGBToString(this.color);
    theSurface.fillStyle = colorRGBToString(this.color);
    this.drawNoFinish(theCanvas, true);
    theSurface.stroke();
  }
}

class PlotFillTwoD {
  constructor(inputCanvas, inputColor) {
    this.indexFillStart = inputCanvas.theObjects.length;
    /** @type{Array.<number>} */
    this.color = colorToRGB(inputColor);
  }

  accountBoundingBox(inputOutputBox) { }

  draw(inputCanvas) {
    let theSurface = inputCanvas.surface;
    let theObs = inputCanvas.theObjects;
    let tempCounter = inputCanvas.numDrawnObjects;
    theSurface.beginPath();
    let firstRun = true;
    for (tempCounter++; tempCounter < theObs.length; tempCounter++) {
      let currentO = theObs[tempCounter];
      if (currentO.type === 'plotFillFinish') {
        break;
      }
      currentO.drawNoFinish(inputCanvas, firstRun);
      firstRun = false;
    }
    theSurface.fillStyle = colorRGBToString(this.color);
    theSurface.fill();
    for (inputCanvas.numDrawnObjects++;
      inputCanvas.numDrawnObjects < theObs.length;
      inputCanvas.numDrawnObjects++) {
      let currentO = theObs[inputCanvas.numDrawnObjects];
      if (currentO.type === 'plotFillFinish') {
        break;
      }
      currentO.draw(inputCanvas);
    }
  }
}

class CanvasTwoD {
  constructor(
    /**@type{HTMLCanvasElement} */
    inputCanvas,
    /**@type{HTMLElement} */
    controls,
    /**@type{HTMLElement} */
    messages,
  ) {
    this.theObjects = [];
    this.surface = null;
    this.canvasContainer = inputCanvas;

    this.canvasId = null;
    this.screenBasisOrthonormal = [];
    /**@type{HTMLElement|null} */
    this.spanMessages = messages;
    if (this.spanMessages === undefined) {
      this.spanMessages = null;
    }
    /**@type{HTMLElement|null} */
    this.spanControls = controls;
    if (this.spanControls === undefined) {
      this.spanControls = null;
    }
    this.numDrawnObjects = 0;
    this.boundingBoxMath = [[-0.1, -0.1], [0.1, 0.1]];
    this.width = inputCanvas.width;
    this.height = inputCanvas.height;
    this.centerCanvasX = inputCanvas.width / 2;
    this.centerCanvasY = inputCanvas.height / 2;
    this.centerX = this.centerCanvasX;
    this.centerY = this.centerCanvasY;
    this.viewWindowDefault = [[-5, -5], [5, 5]];
    /** @type{number} */
    this.scale = 50;
    /** @type{Array.<number>} */
    this.mousePosition = [];
    /** @type{Array.<number>} */
    this.clickedPosition = [];
    this.positionDelta = [];
    this.textMouseInfo = '';
    this.textProjectionInfo = '';
    this.textPatchInfo = '';
    this.textErrors = '';
    this.textPerformance = '';
    this.selectedElement = '';
    this.redrawStart = 0;
    this.redrawFinish = 0;
    this.redrawTime = 0;
    this.defaultNumSegmentsPerContour = 10;
    this.flagShowPerformance = true;
    this.flagShowAxesTicks = false;
    this.flagShowGrid = false;
  }

  drawPoints(
    /**@type{Array.<Array.<number>>} */
    inputPoints,
    inputColor,
  ) {
    this.theObjects.push(new PointsTwoD(inputPoints, inputColor));
  }

  drawLine(inputLeftPt, inputRightPt, inputColor, inputLineWidth) {
    let newLine =
      new SegmentTwoD(inputLeftPt, inputRightPt, inputColor, inputLineWidth);
    this.theObjects.push(newLine);
  }

  drawGrid() {
    this.flagShowAxesTicks = true;
    this.flagShowGrid = true;
    this.theObjects.push(new AxesGrid());
  }

  drawCoordinateAxes(
      /** @type{string|undefined} */ xAxisLabel,
      /** @type{string|undefined} */ yAxisLabel) {
    if (xAxisLabel === undefined) {
      xAxisLabel = '';
    }
    if (yAxisLabel === undefined) {
      yAxisLabel = '';
    }
    this.theObjects.push(new DrawCoordinateAxesTwoD(xAxisLabel, yAxisLabel));
  }

  drawVectorField(
    inputField,
    /**@type{boolean} */
    inputIsDirectionField,
    inputLowLeft,
    inputHighRight,
    /**@type{Array.<number>} */
    inputNumSegmentsXY,
    inputDesiredLengthDirectionVectors,
    inputColor,
    inputLineWidth,
  ) {
    let newLine = new VectorFieldTwoD(
      inputField,
      inputIsDirectionField,
      inputLowLeft,
      inputHighRight,
      inputNumSegmentsXY,
      inputDesiredLengthDirectionVectors,
      inputColor,
      inputLineWidth,
    );
    this.theObjects.push(newLine);
  }

  drawPath(inputPath, inputColor, inputLineWidth) {
    let newPath =
      new PathTwoD(inputPath, inputColor, inputColor, inputLineWidth);
    this.theObjects.push(newPath);
  }

  drawPathFilled(inputPath, inputContourColor, inputFillColor) {
    let newPath = new PathTwoD(inputPath, inputContourColor, inputFillColor, 1);
    newPath.isFilled = true;
    this.theObjects.push(newPath);
  }

  /**
   * Plots a function.
   *
   * @param{function(number):number} toBePlotted the function to plot
   * @param{number|string} leftPoint the x-coordinate of the left-most point of
   * the plot or the string 'minusInfinity'
   * @param{number|string} rightPoint the x-coordinate of the right-most point
   * of the plot or the string 'infinity'
   * @param{number} inputNumSegments number of straight segments that make up
   * our plot
   * @param{string} inputColor the color of the plot
   * @param{number} inputLineWidth the width of the line to plot in pixels.
   *
   */
  drawFunction(
    toBePlotted, leftPoint, rightPoint, inputNumSegments, inputColor,
    inputLineWidth) {
    let newPlot = new PlotTwoD(
      toBePlotted, leftPoint, rightPoint, inputNumSegments, inputColor,
      inputLineWidth);
    this.theObjects.push(newPlot);
  }

  drawCurve(
    inputCoordinateFuns, inputLeftPt, inputRightPt, inputNumSegments,
    inputColor, inputLineWidth) {
    let newPlot = new CurveTwoD(
      inputCoordinateFuns, inputLeftPt, inputRightPt, inputNumSegments,
      inputColor, inputLineWidth);
    this.theObjects.push(newPlot);
  }

  drawText(inputLocation, inputText, inputColor) {
    let newPlot = new TextPlotTwoD(inputLocation, inputText, inputColor);
    this.theObjects.push(newPlot);
  }

  plotFillStart(inputColor) {
    this.theObjects.push(new PlotFillTwoD(this, inputColor));
  }

  plotFillFinish() {
    this.theObjects.push({ type: 'plotFillFinish' });
  }

  computeViewWindow() {
    this.boundingBoxMath = [[-0.1, -0.1], [0.1, 0.1]];
    for (let i = 0; i < this.theObjects.length; i++) {
      if (this.theObjects[i].accountBoundingBox === undefined) {
        continue;
      }
      this.theObjects[i].accountBoundingBox(this.boundingBoxMath);
    }
    vectorTimesScalar(this.boundingBoxMath[0], 1.05);
    vectorTimesScalar(this.boundingBoxMath[1], 1.05);
    this.setViewWindow(this.boundingBoxMath[0], this.boundingBoxMath[1]);
  };

  setViewWindow(leftLowPt, rightUpPt) {
    this.viewWindowDefault = [leftLowPt, rightUpPt];
    let leftLowScreen = this.coordinatesMathToScreen(leftLowPt);
    let rightUpScreen = this.coordinatesMathToScreen(rightUpPt);
    let desiredHeight = Math.abs(rightUpScreen[1] - leftLowScreen[1]);
    let desiredWidth = Math.abs(rightUpScreen[0] - leftLowScreen[0]);
    let candidateScaleHeight = this.scale * this.height / desiredHeight;
    let candidateScaleWidth = this.scale * this.width / desiredWidth;
    this.scale = Math.min(candidateScaleHeight, candidateScaleWidth);
    // console.log("new scale: "+ this.scale);
    let centerViewWindowMath = vectorPlusVector(leftLowPt, rightUpPt);
    vectorTimesScalar(centerViewWindowMath, 0.5);
    let centerViewWindowScreen =
      this.coordinatesMathToScreen(centerViewWindowMath);
    this.centerX += this.centerCanvasX - centerViewWindowScreen[0];
    this.centerY += this.centerCanvasY - centerViewWindowScreen[1];
  }

  redraw() {
    this.textPerformance = '';
    this.redrawStart = new Date().getTime();
    let theSurface = this.surface;
    theSurface.clearRect(0, 0, this.width, this.height);
    for (this.numDrawnObjects = 0;
      this.numDrawnObjects < this.theObjects.length;
      this.numDrawnObjects++) {
      this.theObjects[this.numDrawnObjects].draw(this);
    }
    let redrawTime = new Date().getTime();
    if (this.flagShowPerformance) {
      this.textPerformance =
        'Redraw time (ms): ' + (redrawTime - this.redrawStart);
      this.showMessages();
    }
  }

  computeBasis() {
    this.screenBasisOrthonormal[0] = [1, 0];
    this.screenBasisOrthonormal[1] = [0, 1];
    this.textProjectionInfo = '';
    this.textProjectionInfo +=
      `<br>e1: ${this.screenBasisOrthonormal[0]} <br>e2: ${this.screenBasisOrthonormal[1]}`;
  }

  mouseWheelHandler(e) {
    let inputs = drawing.mouseWheelCommon(e);
    this.mouseWheel(inputs.delta, inputs.x, inputs.y);
  }

  mouseUp() {
    this.selectEmpty();
    this.redraw();
  }

  clickHandler(e) {
    this.canvasClick(e.clientX, e.clientY, e);
  }

  initialize() {
    this.surface = this.canvasContainer.getContext('2d');
    this.canvasContainer.addEventListener('DOMMouseScroll', (e) => {
      this.mouseWheelHandler(e);
    }, { passive: false });
    this.canvasContainer.addEventListener('mousewheel', (e) => {
      this.mouseWheelHandler(e);
    }, { passive: false });
    this.canvasContainer.addEventListener('mousedown', (e) => {
      this.clickHandler(e);
    }, true);
    this.canvasContainer.addEventListener('mouseup', () => {
      this.mouseUp();
    }, true);
    this.canvasContainer.addEventListener('mousemove', (e) => {
      let x = e.clientX;
      let y = e.clientY;
      this.mouseMove(x, y);
    }, true);

    this.theObjects = [];
    this.constructControls();
    this.computeBasis();
  }

  resetView() {
    if (this.surface === null) {
      // Graphics not intialized.
      return;
    }
    this.setViewWindow(this.viewWindowDefault[0], this.viewWindowDefault[1]);
    this.redraw();
  }

  constructControls() {
    if (this.spanControls === null) {
      return;
    }
    let button = document.createElement('button');
    button.textContent = '\u21BB';
    button.addEventListener('click', () => {
      this.resetView();
    });
    this.spanControls.textContent = '';
    this.spanControls.appendChild(button);
  }

  coordinatesMathScreenToMath(theCoords) {
    let output = this.screenBasisOrthonormal[0].slice();
    vectorTimesScalar(output, theCoords[0]);
    vectorAddVectorTimesScalar(
      output, this.screenBasisOrthonormal[1], theCoords[1]);
    return output;
  }

  coordinatesMathToMathScreen(vector) {
    return [
      vectorScalarVector(vector, this.screenBasisOrthonormal[0]),
      vectorScalarVector(vector, this.screenBasisOrthonormal[1])
    ];
  }

  coordinatesMathToScreen(vector) {
    return [
      this.scale * vectorScalarVector(vector, this.screenBasisOrthonormal[0]) +
      this.centerX,
      (-1) * this.scale *
      vectorScalarVector(vector, this.screenBasisOrthonormal[1]) +
      this.centerY
    ];
  }

  coordsScreenAbsoluteToScreen(screenX, screenY) {
    return getPosXPosYObject(this.canvasContainer, screenX, screenY);
  }

  coordsScreenToMathScreen(screenPos) {
    return [
      (screenPos[0] - this.centerX) / this.scale,
      (this.centerY - screenPos[1]) / this.scale
    ];
  }

  coordsScreenAbsoluteToMathScreen(screenX, screenY) {
    return this.coordsScreenToMathScreen(
      this.coordsScreenAbsoluteToScreen(screenX, screenY));
  }

  coordsMathScreenToScreen(theCoords) {
    return [
      this.scale * theCoords[0] + this.centerX,
      this.centerY - this.scale * theCoords[1]
    ];
  }

  mouseWheel(wheelDelta, screenX, screenY) {
    let screenPos = this.coordsScreenAbsoluteToScreen(screenX, screenY);
    let mathScreenPos = this.coordsScreenToMathScreen(screenPos);
    if (wheelDelta / this.scale > 0.1) {
      wheelDelta = this.scale * 0.1;
    }
    if (wheelDelta / this.scale < -0.1) {
      wheelDelta = -this.scale * 0.1;
    }
    this.scale += wheelDelta;
    if (this.scale <= 0) {
      this.scale = 1;
    }
    let intermediateScreenPos = this.coordsMathScreenToScreen(mathScreenPos);
    // console.log("start screen: "+[screenX, screenY]);
    // console.log("intermed. screen: "+ intermediateScreenPos);
    this.centerX = this.centerX + screenPos[0] - intermediateScreenPos[0];
    this.centerY = this.centerY + screenPos[1] - intermediateScreenPos[1];
    this.redraw();
  }

  mouseMove(screenX, screenY) {
    if (this.selectedElement === '') {
      return;
    }
    this.mousePosition =
      this.coordsScreenAbsoluteToMathScreen(screenX, screenY);
    if (this.selectedElement === 'origin') {
      this.panAfterCursor();
    }
    this.redrawFinish = new Date().getTime();
    this.redrawTime = this.redrawFinish - this.redrawStart;
  }

  panAfterCursor() {
    let difference =
      vectorMinusVector(this.mousePosition, this.clickedPosition);
    this.centerX += difference[0] * this.scale;
    this.centerY -= difference[1] * this.scale;
    this.redraw();
  }

  pointsWithinClickTolerance(leftXY, rightXY) {
    let squaredDistance =
      ((leftXY[0] - rightXY[0]) * (leftXY[0] - rightXY[0]) +
        (leftXY[1] - rightXY[1]) * (leftXY[1] - rightXY[1])) *
      this.scale;
    return squaredDistance < 1000;
  }

  canvasClick(screenX, screenY, e) {
    this.clickedPosition =
      this.coordsScreenAbsoluteToMathScreen(screenX, screenY);
    this.mousePosition = [];
    // if (this.pointsWithinClickTolerance(this.clickedPosition,[0,0]))
    this.selectedElement = 'origin';
    // else
    //   this.selectedElement ="";
    if (this.flagShowPerformance) {
      this.logStatus();
    }
  }

  selectEmpty() {
    this.selectedElement = '';
  }

  showMessages() {
    if (this.spanMessages === null || this.spanMessages === undefined) {
      return;
    }
    this.spanMessages.textContent = '';
    if (this.textPerformance != '') {
      this.spanMessages.appendChild(
        document.createTextNode(this.textPerformance));
      this.spanMessages.appendChild(document.createElement('br'));
    }
    if (this.textErrors !== '') {
      this.spanMessages.appendChild(document.createTextNode(this.textErrors));
    }
    this.spanMessages.appendChild(document.createTextNode(this.textMouseInfo));
    this.spanMessages.appendChild(
      document.createTextNode(this.textProjectionInfo));
    if (this.textPatchInfo !== '') {
      this.spanMessages.appendChild(
        document.createTextNode(this.textPatchInfo));
    }
  }

  logStatus() {
    this.textMouseInfo = '';
    this.textMouseInfo += `time last redraw: ${this.redrawTime}" ms `;
    this.textMouseInfo += `(~ ${(1000 / this.redrawTime).toFixed(1)} f.p.s.)`;
    this.textMouseInfo += `<br>mouse coordinates: ${this.mousePosition}`;
    this.textMouseInfo += `<br>clicked coordinates: ${this.clickedPosition}`;
    this.textMouseInfo += `<br>delta of position: ${this.positionDelta}`;
    this.showMessages();
  }
}

class TextInThreeD {
  constructor(location, text, color) {
    this.location = location;
    this.text = text;
    this.color = color;
  }
}

class Canvas {
  constructor(
    /**@type{HTMLCanvasElement} */
    inputCanvas,
    /**@type{HTMLElement} */
    controls,
    /**@type{HTMLElement} */
    messages,
  ) {
    this.canvasContainer = inputCanvas;
    this.spanControls = controls;
    this.spanMessages = messages;
    this.theIIIdObjects = {
      thePatches: [],
      theContours: [],
      thePoints: [],
      /**@type{Array.<TextInThreeD>} */
      theLabels: [],
    };
    this.screenXY = [0, 0];
    this.flagShowPerformance = true;
    this.rotationModesAvailable = {
      'rotateAfterCursorDefaultGreatNormalCircle':
        this.rotateAfterCursorDefaultGreatNormalCircle,
    };
    this.rotationMode = 'rotateAfterCursorDefaultGreatNormalCircle';

    this.thePatchOrder = [];
    this.numAccountedPatches = 0;
    this.numCyclicallyOverlappingPatchTieBreaks = 0;
    this.numContourPoints = 0;
    this.numContourPaths = 0;
    this.patchIsAccounted = [];
    /** @type{CanvasRenderingContext2D|null} */
    this.surface = null;
    this.canvasId = null;
    this.screenBasisUserDefault = [[2, 1, 0], [0, 1, 1]];
    this.screenBasisUser = this.screenBasisUserDefault.slice();
    this.screenNormal = [];
    this.screenBasisOrthonormal = [];
    this.zBufferColCount = 20;
    this.zBufferRowCount = 20;
    this.zBuffer = [];
    this.zBufferIndexStrip = [];
    this.bufferDeltaX = 0;
    this.bufferDeltaY = 0;
    this.colorDepthFactor = 0.4;
    this.boundingBoxMathScreen = [[-0.01, -0.01], [0.01, 0.01]];
    this.boundingBoxMath = [[-0.01, -0.01, -0.01], [0.01, 0.01, 0.01]];
    this.boundingSegmentZ = [-0.01, 0.01];
    this.lastCenterScreen = [0, 0];
    this.width = inputCanvas.width;
    this.height = inputCanvas.height;
    this.defaultCenterX = this.width / 2;
    this.defaultCenterY = this.height / 2;
    this.centerX = this.defaultCenterX;
    this.centerY = this.defaultCenterY;
    this.scaleDefault = 50;
    /** @type{number} */
    this.scale = this.scaleDefault;
    /** @type{Array.<number>} */
    this.mousePosition = [];
    /** @type{Array.<number>} Position in math coordinates, 3d. */
    this.clickedPosition = [];
    this.unitRay = [];
    this.zUnit = [];
    this.rayComponent = [];
    this.positionDelta = [];
    this.spanMessages = undefined;
    this.textMouseInfo = '';
    this.textProjectionInfo = '';
    this.textPatchInfo = '';
    this.textErrors = '';
    this.textPerformance = '';
    this.angleNormal = 0;
    this.oldAngleNormal = 0;
    this.newAngleNormal = 0;
    this.anglePolar = 0;
    this.selectedElement = '';
    this.selectedVector = [];
    /** @type{Array.<Array.<number>>} */
    this.selectedScreenBasisOrthonormal = [];
    /** @type{Array.<number>} */
    this.selectedScreenProjectionNormalized = [];
    this.selectedScreenNormal = [];
    this.selectedPolarAngleChange = 0;
    this.redrawStart = 0;
    this.redrawFinish = 0;
    this.redrawTime = 0;
    this.defaultNumSegmentsPerContour = 10;
    this.flagRoundContours = false;
    this.flagRoundPatches = true;
    this.flagDebugLabelPatches = false;
  }

  mouseWheelHandler(e) {
    let inputs = drawing.mouseWheelCommon(e);
    this.mouseWheel(inputs.delta, inputs.x, inputs.y);
  }

  mouseUp() {
    this.selectEmpty();
    this.redraw();
  }

  clickHandler(e) {
    this.canvasClick(e.clientX, e.clientY, e);
  }

  initialize() {
    this.surface = /** @type{CanvasRenderingContext2D} */ (
      this.canvasContainer.getContext('2d'));
    this.canvasContainer.addEventListener('DOMMouseScroll', (e) => {
      this.mouseWheelHandler(e);
    }, { passive: false });
    this.canvasContainer.addEventListener('mousewheel', (e) => {
      this.mouseWheelHandler(e);
    }, { passive: false });
    this.canvasContainer.addEventListener('mousedown', (e) => {
      this.clickHandler(e);
    }, true);
    this.canvasContainer.addEventListener('mouseup', () => {
      this.mouseUp();
    }, true);
    this.canvasContainer.addEventListener('mousemove', (e) => {
      let x = e.clientX;
      let y = e.clientY;
      this.mouseMove(x, y);
    }, true);
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

  drawText(location, text, color) {
    this.theIIIdObjects.theLabels.push(new TextInThreeD(location, text, color));
  }

  drawCurve(theCurve) {
    let contourPoints = new Array(theCurve.numSegments + 1);
    for (let i = 0; i < theCurve.numSegments + 1; i++) {
      let ratio = i / theCurve.numSegments;
      let currentParam =
        theCurve.leftPt * (1 - ratio) + theCurve.rightPt * ratio;
      contourPoints[i] = theCurve.coordinateFunctions(currentParam);
    }
    this.theIIIdObjects.theContours.push(
      new Contour(contourPoints, theCurve.color, theCurve.lineWidth));
  }

  drawPatchStraight(base, edge1, edge2, color) {
    this.theIIIdObjects.thePatches.push(
      new Patch(base, edge1, edge2, color, color));
    let patchIndex = this.theIIIdObjects.thePatches.length - 1;
    let thePatch = this.theIIIdObjects.thePatches[patchIndex];
    thePatch.index = patchIndex;
    let theContours = this.theIIIdObjects.theContours;
    thePatch.adjacentContours.push(
      this.drawLine(thePatch.base, thePatch.v1, color, 1));
    thePatch.traversalOrder.push(1);

    theContours[theContours.length - 1].adjacentPatches.push(patchIndex);
    thePatch.adjacentContours.push(
      this.drawLine(thePatch.v1, thePatch.vEnd, color, 1));
    thePatch.traversalOrder.push(1);

    theContours[theContours.length - 1].adjacentPatches.push(patchIndex);
    thePatch.adjacentContours.push(
      this.drawLine(thePatch.vEnd, thePatch.v2, color, 1));
    thePatch.traversalOrder.push(1);

    theContours[theContours.length - 1].adjacentPatches.push(patchIndex);
    thePatch.adjacentContours.push(
      this.drawLine(thePatch.v2, thePatch.base, color, 1));
    thePatch.traversalOrder.push(1);

    theContours[theContours.length - 1].adjacentPatches.push(patchIndex);
  }

  drawPoints(inputPoints, inputColor) {
    for (let i = 0; i < inputPoints.length; i++) {
      this.theIIIdObjects.thePoints.push(new Point(inputPoints[i], inputColor));
    }
  }

  drawLine(leftPt, rightPt, inputColor, inputLineWidth) {
    let newContour = new Contour([], inputColor, inputLineWidth);
    let numSegments = this.defaultNumSegmentsPerContour;
    newContour.index = this.theIIIdObjects.theContours.length;
    let incrementVector = vectorMinusVector(rightPt, leftPt);
    if (vectorLength(incrementVector) * 10 > numSegments) {
      numSegments = Math.ceil(vectorLength(incrementVector) * 10);
    }
    newContour.thePoints = new Array(numSegments);
    let incrementScalar = 1 / numSegments;
    vectorTimesScalar(incrementVector, incrementScalar);
    let currentPoint = leftPt.slice();
    for (let i = 0; i < numSegments + 1; i++) {
      newContour.thePoints[i] = currentPoint;
      currentPoint = vectorPlusVector(currentPoint, incrementVector);
    }
    this.theIIIdObjects.theContours.push(newContour);
    return this.theIIIdObjects.theContours.length - 1;
  }

  computePatch(thePatch) {
    thePatch.normalScreen1 =
      vectorCrossVector(this.screenNormal, thePatch.edge1);
    thePatch.normalScreen2 =
      vectorCrossVector(this.screenNormal, thePatch.edge2);
    thePatch.normal = vectorCrossVector(thePatch.edge1, thePatch.edge2);
    thePatch.internalPoint = thePatch.base.slice();
    vectorAddVectorTimesScalar(thePatch.internalPoint, thePatch.edge1, 0.5);
    vectorAddVectorTimesScalar(thePatch.internalPoint, thePatch.edge2, 0.5);
  }

  computeContour(theContour) {
    if (theContour.thePointsMathScreen.length !== theContour.thePoints.length) {
      theContour.thePointsMathScreen = new Array(theContour.thePoints.length);
    }
    for (let i = 0; i < theContour.thePoints.length; i++) {
      theContour.thePointsMathScreen[i] =
        this.coordsProjectMathToMathScreen2d(theContour.thePoints[i]);
    }
  }

  pointRelativeToPatch(thePoint, thePatch) {
    if (vectorLength(thePatch.normalScreen1) < 0.00001 ||
      vectorLength(thePatch.normalScreen2) < 0.00001 ||
      vectorLength(thePatch.normal) < 0.00001) {
      return 0;
    }
    if (vectorScalarVector(
      vectorMinusVector(thePoint, thePatch.base),
      thePatch.normalScreen1) *
      vectorScalarVector(
        vectorMinusVector(thePatch.internalPoint, thePatch.base),
        thePatch.normalScreen1) >
      0) {
      if (vectorScalarVector(
        vectorMinusVector(thePoint, thePatch.vEnd),
        thePatch.normalScreen1) *
        vectorScalarVector(
          vectorMinusVector(thePatch.internalPoint, thePatch.vEnd),
          thePatch.normalScreen1) >
        0) {
        if (vectorScalarVector(
          vectorMinusVector(thePoint, thePatch.base),
          thePatch.normalScreen2) *
          vectorScalarVector(
            vectorMinusVector(thePatch.internalPoint, thePatch.base),
            thePatch.normalScreen2) >
          0) {
          if (vectorScalarVector(
            vectorMinusVector(thePoint, thePatch.vEnd),
            thePatch.normalScreen2) *
            vectorScalarVector(
              vectorMinusVector(thePatch.internalPoint, thePatch.vEnd),
              thePatch.normalScreen2) >
            0) {
            if (vectorScalarVector(
              vectorMinusVector(thePoint, thePatch.base),
              thePatch.normal) *
              vectorScalarVector(this.screenNormal, thePatch.normal) <=
              0) {
              return -1;
            }
            if (vectorScalarVector(
              vectorMinusVector(thePoint, thePatch.base),
              thePatch.normal) *
              vectorScalarVector(this.screenNormal, thePatch.normal) >=
              0) {
              return 1;
            }
          }
        }
      }
    }
    return 0;
  }

  pointIsBehindPatch(thePoint, thePatch) {
    return this.pointRelativeToPatch(thePoint, thePatch) === -1;
  }

  pointIsInFrontOfPatch(thePoint, thePatch) {
    return this.pointRelativeToPatch(thePoint, thePatch) === 1;
  }

  pointIsInForeGround(thePoint, containerPatches) {
    let thePatches = this.theIIIdObjects.thePatches;
    for (let i = 0; i < thePatches.length; i++) {
      if (containerPatches.indexOf(i) !== -1) {
        continue;
      }
      if (this.pointIsBehindPatch(thePoint, thePatches[i])) {
        return false;
      }
    }
    return true;
  }

  paintMouseInfo() {
    if (this.selectedElement !== 'default' ||
      this.selectedElement === undefined || this.selectedVector === [] ||
      this.selectedVector === undefined) {
      return;
    }
    if (this.selectedVector.length === 0 ||
      this.selectedVector.length === undefined) {
      return;
    }
    let currentPt;
    this.surface.beginPath();
    this.surface.setLineDash([4, 4]);
    this.surface.lineWidth = 2;
    this.surface.strokeStyle = 'green';
    currentPt = this.coordsMathToSelectedScreen(this.selectedVector);
    this.surface.moveTo(currentPt[0], currentPt[1]);
    currentPt = this.coordsMathToSelectedScreen([0, 0, 0]);
    this.surface.lineTo(currentPt[0], currentPt[1]);
    this.surface.stroke();
    this.surface.beginPath();
    this.surface.setLineDash([]);
    this.surface.lineWidth = 2;
    this.surface.strokeStyle = 'green';
    currentPt = this.coordinatesMathToScreen(this.selectedVector);
    this.surface.moveTo(currentPt[0], currentPt[1]);
    currentPt = this.coordinatesMathToScreen([0, 0, 0]);
    this.surface.lineTo(currentPt[0], currentPt[1]);
    this.surface.stroke();
  }

  paintOneContour(theContour) {
    if (theContour.thePoints.length < 2) {
      return;
    }
    let theSurface = this.surface;
    let thePts = theContour.thePoints;
    // console.log("line start\n");
    let currentPt = this.coordinatesMathToScreen(thePts[0]);
    if (this.flagRoundContours) {
      vectorRound(currentPt);
    }
    let oldIsInForeGround = this.pointIsInForeGround(
      theContour.thePoints[0], theContour.adjacentPatches);
    let newIsInForeground = true;
    let dashIsOn = false;
    theSurface.beginPath();
    theSurface.setLineDash([]);
    this.numContourPaths++;
    //    theSurface.setLineDash([]);
    theSurface.strokeStyle = colorRGBToString(theContour.color);
    if (theContour.lineWidth !== undefined) {
      theSurface.lineWidth = theContour.lineWidth;
    } else {
      theSurface.lineWidth = 1;
    }
    theSurface.moveTo(currentPt[0], currentPt[1]);
    this.numContourPoints += thePts.length;
    for (let i = 1; i < thePts.length; i++) {
      newIsInForeground = this.pointIsInForeGround(
        theContour.thePoints[i], theContour.adjacentPatches);
      if (!newIsInForeground && !oldIsInForeGround && !dashIsOn) {
        if (i > 1) {
          theSurface.stroke();
          theSurface.setLineDash([4, 4]);
          theSurface.beginPath();
          this.numContourPaths++;
          theSurface.moveTo(currentPt[0], currentPt[1]);
        }
        theSurface.setLineDash([4, 4]);
        dashIsOn = true;
      } else if (dashIsOn && newIsInForeground) {
        theSurface.stroke();
        theSurface.setLineDash([]);
        theSurface.beginPath();
        this.numContourPaths++;
        // theSurface.setLineDash([]);
        // console.log("removed dash\n");
        theSurface.moveTo(currentPt[0], currentPt[1]);
        dashIsOn = false;
      }
      currentPt = this.coordinatesMathToScreen(thePts[i]);
      if (this.flagRoundContours) {
        vectorRound(currentPt);
      }
      theSurface.lineTo(currentPt[0], currentPt[1]);
      oldIsInForeGround = newIsInForeground;
    }
    theSurface.stroke();
    /////////////////
    if (this.flagDebugLabelPatches) {
      currentPt = this.coordinatesMathToScreen(thePts[4]);
      if (this.flagRoundContours) {
        vectorRound(currentPt);
      }
      theSurface.font = '9pt sans-serif';
      theSurface.fillStyle = 'black';
      theSurface.fillText(theContour.index, currentPt[0], currentPt[1]);
    }
    // console.log("line end\n");
  }

  paintOnePatch(thePatch) {
    let theSurface = this.surface;
    let visibilityScalarProd =
      vectorScalarVector(this.screenNormal, thePatch.normal);
    let depthScalarProd =
      vectorScalarVector(this.screenNormal, thePatch.internalPoint);
    let depthRatio = (depthScalarProd - this.boundingSegmentZ[0]) /
      (this.boundingSegmentZ[1] - this.boundingSegmentZ[0]) -
      0.5;
    // depthRatio is a number between -0.5 and 0.5 measuring how deep is the
    // patch relative to the center of the picture
    let colorFactor = 1 + depthRatio * this.colorDepthFactor;
    if (visibilityScalarProd >= 0) {
      theSurface.fillStyle =
        colorRGBToString(colorScale(thePatch.colorUV, colorFactor));
    } else {
      theSurface.fillStyle =
        colorRGBToString(colorScale(thePatch.colorVU, colorFactor));
    }
    theSurface.beginPath();
    let first = true;
    for (let i = 0; i < thePatch.adjacentContours.length; i++) {
      let currentContour =
        this.theIIIdObjects.theContours[thePatch.adjacentContours[i]];
      for (let j = 0; j < currentContour.thePoints.length; j++) {
        let theIndex = (thePatch.traversalOrder[i] === -1) ?
          j :
          currentContour.thePoints.length - j - 1;
        let theCoords =
          this.coordinatesMathToScreen(currentContour.thePoints[theIndex]);
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
  }

  paintText(theText) {
    let theSurface = this.surface;
    let isInForeGround = this.pointIsInForeGround(theText.location, []);
    theSurface.beginPath();
    theSurface.strokeStyle = theText.color;
    let theCoords = this.coordinatesMathToScreen(theText.location);
    theSurface.font = '15pt sans-serif';
    theSurface.lineWidth = 1;
    if (isInForeGround) {
      theSurface.fillStyle = theText.color;
      theSurface.fillText(theText.text, theCoords[0], theCoords[1]);
    } else {
      theSurface.strokeStyle = theText.color;
      theSurface.strokeText(theText.text, theCoords[0], theCoords[1]);
    }
  }

  paintOnePoint(thePoint) {
    let theSurface = this.surface;
    let isInForeGround = this.pointIsInForeGround(thePoint.location, []);
    theSurface.beginPath();
    theSurface.strokeStyle = colorRGBToString(thePoint.color);
    theSurface.fillStyle = colorRGBToString(thePoint.color);
    let theCoords = this.coordinatesMathToScreen(thePoint.location);
    theSurface.arc(theCoords[0], theCoords[1], 3, 0, Math.PI * 2);
    if (isInForeGround) {
      theSurface.fill();
    } else {
      theSurface.stroke();
    }
  }

  getExtremePoint(indexToCompareBy, getLarger, pt1, pt2, pt3, pt4) {
    let result = pt1[indexToCompareBy];
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

  coordsMathScreenToBufferIndicesROWSFloat(input) {
    return this.zBufferRowCount * (input - this.boundingBoxMathScreen[0][1]) /
      (this.boundingBoxMathScreen[1][1] - this.boundingBoxMathScreen[0][1]);
  }

  coordsMathScreenToBufferIndicesROWS(input) {
    let result =
      Math.floor(this.coordsMathScreenToBufferIndicesROWSFloat(input));
    if (result >= this.zBufferRowCount) {
      result--;
    }
    return result;
  }

  coordsMathScreenToBufferIndicesCOLSFloat(input) {
    return (this.zBufferColCount) * (input - this.boundingBoxMathScreen[0][0]) /
      (this.boundingBoxMathScreen[1][0] - this.boundingBoxMathScreen[0][0]);
  }

  coordsMathScreenToBufferIndicesCOLS(input) {
    let result =
      Math.floor(this.coordsMathScreenToBufferIndicesCOLSFloat(input));
    if (result >= this.zBufferColCount) {
      result--;
    }
    return result;
  }

  coordsMathScreenToBufferIndices(input) {
    let row = this.coordsMathScreenToBufferIndicesROWS(input[1]);
    let col = this.coordsMathScreenToBufferIndicesCOLS(input[0]);
    if (row < 0 || row >= this.zBufferRowCount || col < 0 ||
      col >= this.zBufferColCount) {
      console.log(
        'point with math-screen coords: ' + input +
        ' is out of the bounding box');
    }
    return [row, col];
  }

  accountOnePointMathCoordsInBufferStrip(row, thePoint, patchIndex) {
    if (row < 0 || row >= this.zBufferIndexStrip.length) {
      return;
    }
    let bufferCoords = this.coordsMathScreenToBufferIndices(
      this.coordsProjectMathToMathScreen2d(thePoint));
    // If there were no rounding errors, row would be equal
    // to bufferCoords[0]. However since there will be rounding errors,
    // the row is passed instead as an argument.
    if (this.zBufferIndexStrip[row][0] === -1) {
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

  accountEdgeInBufferStrip(base, edgeVector, patchIndex) {
    let end = vectorPlusVector(base, edgeVector);
    let lowFloat = this.coordsMathScreenToBufferIndicesROWSFloat(
      this.coordsProjectMathToMathScreen2d(base)[1]);
    let highFloat = this.coordsMathScreenToBufferIndicesROWSFloat(
      this.coordsProjectMathToMathScreen2d(end)[1]);
    if (lowFloat > highFloat) {
      let minusEdge = edgeVector.slice();
      vectorTimesScalar(minusEdge, -1);
      this.accountEdgeInBufferStrip(end, minusEdge, patchIndex);
      return;
    }
    this.accountOnePointMathCoordsInBufferStrip(
      Math.floor(lowFloat), base, patchIndex);
    this.accountOnePointMathCoordsInBufferStrip(
      Math.floor(highFloat), end, patchIndex);
    if (lowFloat === highFloat) {
      return;
    }
    let currentPoint;
    for (let i = Math.ceil(lowFloat); i < highFloat; i++) {
      currentPoint = base.slice();
      vectorAddVectorTimesScalar(
        currentPoint, edgeVector, (i - lowFloat) / (highFloat - lowFloat));
      this.accountOnePointMathCoordsInBufferStrip(i, currentPoint, patchIndex);
      this.accountOnePointMathCoordsInBufferStrip(
        i - 1, currentPoint, patchIndex);
    }
  }

  accountOnePatch(patchIndex) {
    let thePatch = this.theIIIdObjects.thePatches[patchIndex];
    let pt1 = this.coordsProjectMathToMathScreen2d(thePatch.base);
    let pt2 = this.coordsProjectMathToMathScreen2d(thePatch.v1);
    let pt3 = this.coordsProjectMathToMathScreen2d(thePatch.v2);
    let pt4 = this.coordsProjectMathToMathScreen2d(thePatch.vEnd);
    let lowFloat = this.getExtremePoint(1, 0, pt1, pt2, pt3, pt4);
    let highFloat = this.getExtremePoint(1, 1, pt1, pt2, pt3, pt4);
    let low = this.coordsMathScreenToBufferIndicesROWS(lowFloat);
    let high = this.coordsMathScreenToBufferIndicesROWS(highFloat);
    for (let i = low; i <= high; i++) {
      this.zBufferIndexStrip[i][0] = -1;
      this.zBufferIndexStrip[i][1] = -1;
    }
    this.accountEdgeInBufferStrip(thePatch.base, thePatch.edge1, patchIndex);
    this.accountEdgeInBufferStrip(thePatch.base, thePatch.edge2, patchIndex);
    this.accountEdgeInBufferStrip(thePatch.v1, thePatch.edge2, patchIndex);
    this.accountEdgeInBufferStrip(thePatch.v2, thePatch.edge1, patchIndex);
    for (let i = low; i <= high; i++) {
      for (let j = this.zBufferIndexStrip[i][0];
        j <= this.zBufferIndexStrip[i][1]; j++) {
        if (i === -1 || j === -1) {
          continue;
        }
        this.zBuffer[i][j].push(patchIndex);
      }
    }
  }

  computeBoundingBoxAccountPoint(input) {
    let theV = this.coordsProjectMathToMathScreen2d(input);
    for (let i = 0; i < 2; i++) {
      if (theV[i] < this.boundingBoxMathScreen[0][i]) {
        this.boundingBoxMathScreen[0][i] = theV[i];
      }
    }
    for (let i = 0; i < 2; i++) {
      if (theV[i] > this.boundingBoxMathScreen[1][i]) {
        this.boundingBoxMathScreen[1][i] = theV[i];
      }
    }
    for (let i = 0; i < 3; i++) {
      if (input[i] < this.boundingBoxMath[0][i]) {
        this.boundingBoxMath[0][i] = input[i];
      }
    }
    for (let i = 0; i < 3; i++) {
      if (input[i] > this.boundingBoxMath[1][i]) {
        this.boundingBoxMath[1][i] = input[i];
      }
    }
    let theScalarProd = vectorScalarVector(this.screenNormal, input);
    if (theScalarProd < this.boundingSegmentZ[0]) {
      this.boundingSegmentZ[0] = theScalarProd;
    }
    if (theScalarProd > this.boundingSegmentZ[1]) {
      this.boundingSegmentZ[1] = theScalarProd;
    }
  }

  computeBoundingBox() {
    let thePatches = this.theIIIdObjects.thePatches;
    let theContours = this.theIIIdObjects.theContours;
    let thePoints = this.theIIIdObjects.thePoints;
    for (let i = 0; i < thePatches.length; i++) {
      this.computeBoundingBoxAccountPoint(thePatches[i].base);
      this.computeBoundingBoxAccountPoint(thePatches[i].v1);
      this.computeBoundingBoxAccountPoint(thePatches[i].v2);
      this.computeBoundingBoxAccountPoint(thePatches[i].vEnd);
    }
    for (let i = 0; i < theContours.length; i++) {
      for (let j = 0; j < theContours[i].thePoints.length; j++) {
        this.computeBoundingBoxAccountPoint(theContours[i].thePoints[j]);
      }
    }
    for (let i = 0; i < thePoints.length; i++) {
      this.computeBoundingBoxAccountPoint(thePoints[i].location);
    }
  }

  computeBuffers() {
    let thePatches = this.theIIIdObjects.thePatches;
    for (let i = 0; i < this.zBuffer.length; i++) {
      for (let j = 0; j < this.zBuffer[i].length; j++) {
        this.zBuffer[i][j].length = 0;
      }
    }
    this.computeBoundingBox();
    this.bufferDeltaX =
      (this.boundingBoxMathScreen[1][0] - this.boundingBoxMathScreen[0][0]) /
      this.zBufferColCount;
    this.bufferDeltaY =
      (this.boundingBoxMathScreen[1][1] - this.boundingBoxMathScreen[0][1]) /
      this.zBufferRowCount;
    for (let i = 0; i < thePatches.length; i++) {
      this.accountOnePatch(i);
    }
    this.computePatchOrder();
  }

  computePatchOrderOneContourPoint(thePatch, theContour, ptIndex) {
    let thePointMathScreen = theContour.thePointsMathScreen[ptIndex];
    let thePoint = theContour.thePoints[ptIndex];
    let theIndices = this.coordsMathScreenToBufferIndices(thePointMathScreen);
    let currentBuffer = this.zBuffer[theIndices[0]][theIndices[1]];
    let thePatches = this.theIIIdObjects.thePatches;
    for (let i = 0; i < currentBuffer.length; i++) {
      if (thePatch.index === currentBuffer[i]) {
        continue;
      }
      if (thePatch.patchesAboveMe.indexOf(currentBuffer[i]) >= 0 ||
        thePatch.patchesBelowMe.indexOf(currentBuffer[i]) >= 0) {
        continue;
      }
      let otherPatch = thePatches[currentBuffer[i]];
      let relativePosition = this.pointRelativeToPatch(thePoint, otherPatch);
      if (relativePosition === -1) {
        otherPatch.patchesBelowMe.push(thePatch.index);
        thePatch.patchesAboveMe.push(currentBuffer[i]);
      } else if (relativePosition === 1) {
        otherPatch.patchesAboveMe.push(thePatch.index);
        thePatch.patchesBelowMe.push(currentBuffer[i]);
      }
    }
  }

  computePatchPartialOrderOnePatch(thePatch) {
    let theContours = this.theIIIdObjects.theContours;
    for (let i = 0; i < thePatch.adjacentContours.length; i++) {
      for (let j = 0;
        j < theContours[thePatch.adjacentContours[i]].thePoints.length;
        j++) {
        this.computePatchOrderOneContourPoint(
          thePatch, theContours[thePatch.adjacentContours[i]], j);
      }
    }
  }

  computePatchPartialOrder() {
    let thePatches = this.theIIIdObjects.thePatches;
    for (let i = 0; i < thePatches.length; i++) {
      thePatches[i].patchesBelowMe = [];
      thePatches[i].patchesAboveMe = [];
    }
    for (let i = 0; i < thePatches.length; i++) {
      this.computePatchPartialOrderOnePatch(thePatches[i]);
    }
  }

  computePatchOrder() {
    this.computePatchPartialOrder();
    let thePatches = this.theIIIdObjects.thePatches;
    if (this.thePatchOrder.length !== thePatches.length) {
      this.thePatchOrder = new Array(thePatches.length);
      this.patchIsAccounted = new Array(thePatches.length);
    }
    for (let i = 0; i < thePatches.length; i++) {
      this.thePatchOrder[i] = -1;
      this.patchIsAccounted[i] = 0;
    }
    this.numAccountedPatches = 0;
    for (let i = 0; i < thePatches.length; i++) {
      if (thePatches[i].patchesBelowMe.length === 0) {
        this.thePatchOrder[this.numAccountedPatches] = i;
        this.numAccountedPatches++;
        this.patchIsAccounted[i] = 1;
      }
    }
    this.numCyclicallyOverlappingPatchTieBreaks = 0;
    while (this.numAccountedPatches < thePatches.length) {
      let currentIndex = 0;
      while (currentIndex < this.numAccountedPatches) {
        let currentPatch = thePatches[this.thePatchOrder[currentIndex]];
        for (let i = 0; i < currentPatch.patchesAboveMe.length; i++) {
          let nextIndex = currentPatch.patchesAboveMe[i];
          if (this.patchIsAccounted[nextIndex] === 1) {
            continue;
          }
          let nextPatch = thePatches[nextIndex];
          let isGood = 1;
          for (let j = 0; j < nextPatch.patchesBelowMe.length; j++) {
            if (this.patchIsAccounted[nextPatch.patchesBelowMe[j]] !== 1) {
              isGood = 0;
              break;
            }
          }
          if (isGood === 1) {
            this.thePatchOrder[this.numAccountedPatches] = nextIndex;
            this.numAccountedPatches++;
            this.patchIsAccounted[nextIndex] = 1;
          }
        }
        currentIndex++;
      }
      if (this.numAccountedPatches < thePatches.length) {
        this.patchOverlapTieBreak();
      }
    }
  }

  patchOverlapTieBreak() {
    // If we have cyclically overlapping patches we break ties
    // by selecting/painting first the patches whose internal point
    // has the highest (screen) depth.
    let thePatches = this.theIIIdObjects.thePatches;
    let deepestNonAccountedIndex = -1;
    let minDepth = 0;
    for (let i = 0; i < this.patchIsAccounted.length; i++) {
      if (this.patchIsAccounted[i] === 1) {
        continue;
      }
      if (deepestNonAccountedIndex === -1) {
        deepestNonAccountedIndex = i;
        minDepth =
          vectorScalarVector(this.screenNormal, thePatches[i].internalPoint);
      }
      let currentDepth =
        vectorScalarVector(this.screenNormal, thePatches[i].internalPoint);
      if (currentDepth < minDepth) {
        minDepth = currentDepth;
        deepestNonAccountedIndex = i;
      }
    }
    this.numCyclicallyOverlappingPatchTieBreaks++;
    this.thePatchOrder[this.numAccountedPatches] = deepestNonAccountedIndex;
    this.numAccountedPatches++;
    this.patchIsAccounted[deepestNonAccountedIndex] = 1;
  }

  allocateZbuffer() {
    if (this.zBufferRowCount < 1) {
      this.zBufferRowCount = 1;
    }
    if (this.zBufferColCount < 1) {
      this.zBufferColCount = 1;
    }
    if (this.zBuffer.length < this.zBufferRowCount) {
      this.zBuffer = new Array(this.zBufferRowCount);
      this.zBufferIndexStrip = new Array(this.zBufferRowCount);
      for (let i = 0; i < this.zBufferRowCount; i++) {
        this.zBuffer[i] = new Array(this.zBufferColCount);
        for (let j = 0; j < this.zBufferColCount; j++) {
          this.zBuffer[i][j] = [];
        }
        this.zBufferIndexStrip[i] = [-1, -1];
      }
    }
  }

  getBufferBox(row, col) {
    return [
      [
        this.boundingBoxMathScreen[0][0] + this.bufferDeltaX * col,
        this.boundingBoxMathScreen[0][1] + this.bufferDeltaY * row
      ],
      [
        this.boundingBoxMathScreen[0][0] + this.bufferDeltaX * (col + 1),
        this.boundingBoxMathScreen[0][1] + this.bufferDeltaY * (row + 1)
      ]
    ];
  }

  paintZbuffer() {
    let theSurface = this.surface;
    theSurface.strokeStyle = 'gray';
    theSurface.fillStyle = 'black';
    theSurface.setLineDash([]);
    for (let i = 0; i < this.zBuffer.length; i++) {
      for (let j = 0; j < this.zBuffer[i].length; j++) {
        let bufferBox = this.getBufferBox(i, j);
        let bufferBoxLowLeft = this.coordsMathScreenToScreen(bufferBox[0]);
        let bufferBoxTopRight = this.coordsMathScreenToScreen(bufferBox[1]);
        theSurface.beginPath();
        theSurface.moveTo(bufferBoxLowLeft[0], bufferBoxLowLeft[1]);
        theSurface.lineTo(bufferBoxTopRight[0], bufferBoxLowLeft[1]);
        theSurface.lineTo(bufferBoxTopRight[0], bufferBoxTopRight[1]);
        theSurface.lineTo(bufferBoxLowLeft[0], bufferBoxTopRight[1]);
        theSurface.lineTo(bufferBoxLowLeft[0], bufferBoxLowLeft[1]);
        theSurface.stroke();
        if (this.zBuffer[i][j].length !== 0) {
          theSurface.fill();
        }
      }
    }
  }

  computeBasisFromNormal(inputNormal) {
    if (inputNormal[0] !== 0) {
      this.screenBasisUser[0] = [-inputNormal[1], inputNormal[0], 0];
      this.screenBasisUser[1] = [-inputNormal[2], 0, inputNormal[0]];
    } else if (inputNormal[1] !== 0) {
      this.screenBasisUser[0] = [1, 0, 0];
      this.screenBasisUser[1] = [0, inputNormal[2], -inputNormal[1]];
    } else {
      this.screenBasisUser[0] = [1, 0, 0];
      this.screenBasisUser[1] = [0, 1, 0];
    }
  }

  infoProjectionCompute() {
    if (!this.flagShowPerformance) {
      return;
    }
    this.textProjectionInfo = '';
    this.textProjectionInfo += `<br>screen normal: ${this.screenNormal}`;
    this.textProjectionInfo += `<br>e1: ${this.screenBasisOrthonormal[0]}`;
    this.textProjectionInfo += `<br>e2: ${this.screenBasisOrthonormal[1]}`;
    this.textProjectionInfo +=
      `<br>selected e1: ${this.selectedScreenBasisOrthonormal[0]}`;
    this.textProjectionInfo +=
      `<br>selected e2: ${this.selectedScreenBasisOrthonormal[1]}`;
  }

  computeBasis() {
    // if (this.screenBasisOrthonormal.length<2)
    //   this.screenBasisOrthonormal.length =2;
    this.screenBasisOrthonormal[0] = this.screenBasisUser[0].slice();
    this.screenBasisOrthonormal[1] = this.screenBasisUser[1].slice();
    let e1 = this.screenBasisOrthonormal[0];
    let e2 = this.screenBasisOrthonormal[1];
    vectorNormalize(e1);
    vectorAddVectorTimesScalar(e2, e1, -vectorScalarVector(e1, e2));
    vectorNormalize(e2);
    this.screenNormal = vectorCrossVector(e1, e2);
  }

  setBoundingBoxAsDefaultViewWindow() {
    this.resetViewNoRedraw();
    this.computeBoundingBox();
    let leftLowScreen =
      this.coordsMathScreenToScreen(this.boundingBoxMathScreen[0]);
    let rightUpScreen =
      this.coordsMathScreenToScreen(this.boundingBoxMathScreen[1]);
    let desiredHeight = Math.abs(rightUpScreen[1] - leftLowScreen[1]) * 1.05;
    let desiredWidth = Math.abs(rightUpScreen[0] - leftLowScreen[0]) * 1.05;
    let candidateScaleHeight = this.scale * this.height / desiredHeight;
    let candidateScaleWidth = this.scale * this.width / desiredWidth;
    // console.log("leftLowScreen: "+ leftLowScreen +" rightUpScreen:
    // "+rightUpScreen); console.log(centerScreen); console.log("desiredHeight:
    // "+desiredHeight); console.log("candidateScaleHeight:
    // "+candidateScaleHeight); console.log("candidateScaleWidth:
    // "+candidateScaleWidth); console.log("old scale: "+ this.scale);
    this.scale = Math.min(candidateScaleHeight, candidateScaleWidth);
    // console.log("new scale: "+ this.scale);
    let newViewWindowCenterMath = vectorPlusVector(
      this.boundingBoxMathScreen[0], this.boundingBoxMathScreen[1]);
    vectorTimesScalar(newViewWindowCenterMath, 0.5);
    let newCenterViewWindowScreenRescaled =
      this.coordsMathScreenToScreen(newViewWindowCenterMath);
    let oldCenterViewWindowMathScreenRescaled =
      this.coordsMathScreenToScreen(this.lastCenterScreen);
    this.centerX = this.centerX + oldCenterViewWindowMathScreenRescaled[0] -
      newCenterViewWindowScreenRescaled[0];
    this.centerY = this.centerY + oldCenterViewWindowMathScreenRescaled[1] -
      newCenterViewWindowScreenRescaled[1];
    this.defaultCenterX = this.centerX;
    this.defaultCenterY = this.centerY;
    this.scaleDefault = this.scale;
    this.lastCenterScreen = newViewWindowCenterMath.slice();
  }

  resetView() {
    this.resetViewNoRedraw();
    this.redraw();
  }

  resetViewNoRedraw() {
    this.boundingBoxMathScreen = [[-0.01, -0.01], [0.01, 0.01]];
    this.boundingBoxMath = [[-0.01, -0.01, -0.01], [0.01, 0.01, 0.01]];
    this.boundingSegmentZ = [-0.01, 0.01];
    this.screenBasisUser = this.screenBasisUserDefault.slice();
    this.centerX = this.defaultCenterX;
    this.centerY = this.defaultCenterY;
    this.scale = this.scaleDefault;
    this.computeBasis();
  }

  constructControls() {
    if (this.spanControls === null) {
      return;
    }
    this.spanControls.textContent = '';
    let buttonElement = document.createElement('BUTTON');
    buttonElement.style.border = 'none';
    buttonElement.style.background = 'none';
    buttonElement.style.cursor = 'pointer';
    buttonElement.textContent = '\u21BB';
    buttonElement.addEventListener('click', this.resetView.bind(this));
    let hintElement = document.createElement('SMALL');
    hintElement.textContent = ` \u21E7+\u279A=\u21BB`;
    this.spanControls.appendChild(buttonElement);
    this.spanControls.appendChild(hintElement);
  }

  coordsInjectMathScreen2dToMath3d(theCoords) {
    let output = this.screenBasisOrthonormal[0].slice();
    vectorTimesScalar(output, theCoords[0]);
    vectorAddVectorTimesScalar(
      output, this.screenBasisOrthonormal[1], theCoords[1]);
    return output;
  }

  coordsProjectMathToMathSelectedScreen2d(vector) {
    return [
      vectorScalarVector(vector, this.selectedScreenBasisOrthonormal[0]),
      vectorScalarVector(vector, this.selectedScreenBasisOrthonormal[1]),
    ];
  }

  coordsProjectMathToMathScreen2d(vector) {
    return [
      vectorScalarVector(vector, this.screenBasisOrthonormal[0]),
      vectorScalarVector(vector, this.screenBasisOrthonormal[1]),
    ];
  }

  /**
   * Returns a projection in 3d of
   * of the input vector
   * to the current screen.`
   * @returns{Array.<number>}
   */
  coordsProjectToMathScreen3d(vector) {
    let output = this.screenBasisOrthonormal[0].slice();
    vectorTimesScalar(
      output, vectorScalarVector(vector, this.screenBasisOrthonormal[0]));
    vectorAddVectorTimesScalar(
      output,
      this.screenBasisOrthonormal[1],
      vectorScalarVector(vector, this.screenBasisOrthonormal[1]),
    );
    return output;
  }

  /**
   * Returns a projection in 3d
   * of the input vector
   * to the selected screen.`
   * @returns{Array.<number>}
   */
  coordsProjectToSelectedMathScreen3d(
    /**@type{Array.<number>} */
    vector,
  ) {
    let output = this.selectedScreenBasisOrthonormal[0].slice();
    vectorTimesScalar(
      output,
      vectorScalarVector(vector, this.selectedScreenBasisOrthonormal[0]));
    vectorAddVectorTimesScalar(
      output,
      this.selectedScreenBasisOrthonormal[1],
      vectorScalarVector(vector, this.selectedScreenBasisOrthonormal[1]),
    );
    return output;
  }

  coordinatesMathToScreen(vector) {
    return [
      this.scale * vectorScalarVector(vector, this.screenBasisOrthonormal[0]) +
      this.centerX,
      (-1) * this.scale *
      vectorScalarVector(vector, this.screenBasisOrthonormal[1]) +
      this.centerY
    ];
  }

  coordsMathToSelectedScreen(vector) {
    return [
      this.scale *
      vectorScalarVector(
        vector, this.selectedScreenBasisOrthonormal[0]) +
      this.centerX,
      (-1) * this.scale *
      vectorScalarVector(
        vector, this.selectedScreenBasisOrthonormal[1]) +
      this.centerY
    ];
  }

  coordsScreenToMathScreen(screenPos) {
    return [
      (screenPos[0] - this.centerX) / this.scale,
      (this.centerY - screenPos[1]) / this.scale
    ];
  }

  coordsScreenAbsoluteToScreen(cx, cy) {
    return getPosXPosYObject(this.canvasContainer, cx, cy);
  }

  coordsScreenAbsoluteToMathScreen(screenX, screenY) {
    return this.coordsScreenToMathScreen(
      this.coordsScreenAbsoluteToScreen(screenX, screenY));
  }

  coordsMathScreenToScreen(theCoords) {
    return [
      this.scale * theCoords[0] + this.centerX,
      this.centerY - this.scale * theCoords[1]
    ];
  }

  rotateOutOfPlane(input, orthoBasis1, orthoBasis2, theAngle) {
    let vComponent = input.slice();
    let scal1 = vectorScalarVector(orthoBasis1, input);
    let scal2 = vectorScalarVector(orthoBasis2, input);
    let projection = orthoBasis1.slice();
    vectorTimesScalar(projection, scal1);
    vectorAddVectorTimesScalar(projection, orthoBasis2, scal2);
    vectorAddVectorTimesScalar(vComponent, projection, -1);
    projection = orthoBasis1.slice();
    vectorTimesScalar(
      projection, Math.cos(theAngle) * scal1 - Math.sin(theAngle) * scal2);
    vectorAddVectorTimesScalar(
      projection, orthoBasis2,
      Math.sin(theAngle) * scal1 + Math.sin(theAngle) * scal2);
    return vectorPlusVector(vComponent, projection);
  }

  computePositionDelta() {
    let oldX = this.clickedPosition[0];
    let oldY = this.clickedPosition[1];
    let newX = this.mousePosition[0];
    let newY = this.mousePosition[1];
    this.positionDelta = [newX - oldX, newY - oldY];
  }

  rotateAfterCursorDefaultGreatNormalCircle() {
    if (this.mousePosition.length == 0) {
      return;
    }
    this.computePositionDelta();
    this.unitRay = this.clickedPosition.slice();
    vectorNormalize(this.unitRay);
    this.rayComponent = this.unitRay.slice();
    vectorTimesScalar(
      this.rayComponent,
      vectorScalarVector(this.mousePosition, this.unitRay));
    let osculatingOldX = vectorLength(this.clickedPosition);
    let osculatingOldCos = osculatingOldX / vectorLength(this.selectedVector);
    let osculatingNewX = vectorScalarVector(this.mousePosition, this.unitRay);
    let osculatingNewCos = osculatingNewX / vectorLength(this.selectedVector);
    this.newAngleNormal = Math.acos(osculatingNewCos);
    this.oldAngleNormal = Math.acos(osculatingOldCos);
    this.angleNormal = this.oldAngleNormal - this.newAngleNormal;
    if (isNaN(this.angleNormal)) {
      return;
    }
    let newE1 = this.selectedScreenBasisOrthonormal[0].slice();
    let newE2 = this.selectedScreenBasisOrthonormal[1].slice();
    newE1 = this.rotateOutOfPlane(
      newE1, this.selectedScreenProjectionNormalized,
      this.selectedScreenNormal, this.angleNormal);
    newE2 = this.rotateOutOfPlane(
      newE2, this.selectedScreenProjectionNormalized,
      this.selectedScreenNormal, this.angleNormal);
    this.screenBasisUser[0] = newE1.slice();
    this.screenBasisUser[1] = newE2.slice();
    this.computeBasis();
    this.redraw();
  }

  makeValidCosine(
    /** @type{number}*/
    input,
  ) {
    if (input > 1) {
      return 1;
    }
    if (input < -1) {
      return -1;
    }
    return input;
  }

  rotateAfterCursorDefault() {
    if (this.mousePosition.length === 0) {
      return;
    }
    this.rotationModesAvailable[this.rotationMode].bind(this)();
  }

  mouseWheel(wheelDelta, screenX, screenY) {
    let screenPos = this.coordsScreenAbsoluteToScreen(screenX, screenY);
    let mathScreenPos = this.coordsScreenToMathScreen(screenPos);
    this.scale += wheelDelta;
    if (this.scale <= 0) {
      this.scale = 1;
    }
    let intermediateScreenPos = this.coordsMathScreenToScreen(mathScreenPos);
    // console.log("start screen: "+[screenX, screenY]);
    // console.log("intermed. screen: "+ intermediateScreenPos);
    this.centerX = this.centerX + screenPos[0] - intermediateScreenPos[0];
    this.centerY = this.centerY + screenPos[1] - intermediateScreenPos[1];
    this.redraw();
  }

  mouseMove(screenX, screenY) {
    this.screenXY[0] = screenX;
    this.screenXY[1] = screenY;
    if (this.selectedElement == '') {
      return;
    }
    this.mousePosition = this.coordsScreenAbsoluteToMathScreen(
      this.screenXY[0], this.screenXY[1]);
    this.infoMouseCompute();
    // this.infoPatchesCompute();
    this.infoProjectionCompute();
    this.showMessages();
    if (this.selectedElement === 'default') {
      this.rotateAfterCursorDefault();
    }
    if (this.selectedElement === 'origin') {
      this.panAfterCursor();
    }
    this.redrawFinish = new Date().getTime();
    this.redrawTime = this.redrawFinish - this.redrawStart;
  }

  panAfterCursor() {
    let difference =
      vectorMinusVector(this.mousePosition, this.clickedPosition);
    this.centerX += difference[0] * this.scale;
    this.centerY -= difference[1] * this.scale;
    this.redraw();
  }

  pointsWithinClickTolerance(leftXY, rightXY) {
    let squaredDistance =
      ((leftXY[0] - rightXY[0]) * (leftXY[0] - rightXY[0]) +
        (leftXY[1] - rightXY[1]) * (leftXY[1] - rightXY[1])) *
      this.scale;
    return squaredDistance < 7;
  }

  canvasClick(screenX, screenY, event) {
    this.clickedPosition =
      this.coordsScreenAbsoluteToMathScreen(screenX, screenY);
    this.mousePosition = [];
    let mustSelectOrigin = true;
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
    if (this.pointsWithinClickTolerance(this.clickedPosition, [0, 0]) ||
      mustSelectOrigin) {
      this.selectedElement = 'origin';
    } else {
      this.selectedElement = 'default';
      this.computeSelectedVector();
    }
    this.infoMouseCompute();
    this.showMessages();
  }

  selectEmpty() {
    this.selectedElement = '';
    this.selectedScreenBasisOrthonormal = [];
    this.selectedVector = [];
    this.selectedPolarAngleChange = 0;
    this.angleNormal = 0;
  }

  computeSelectedVector() {
    this.selectedScreenProjectionNormalized =
      this.coordsInjectMathScreen2dToMath3d(this.clickedPosition);
    this.selectedVector = this.selectedScreenProjectionNormalized.slice();
    vectorNormalize(this.selectedScreenProjectionNormalized);
    this.selectedScreenNormal = this.screenNormal;
    //    vectorAddVectorTimesScalar(this.selectedVector, this.screenNormal,
    //    0.1);
    let lengthSelectedVector =
      vectorScalarVector(this.selectedVector, this.selectedVector);
    if (lengthSelectedVector < 0.5) {
      vectorTimesScalar(
        this.selectedVector, 1 / Math.sqrt(lengthSelectedVector));
    }
    this.selectedScreenBasisOrthonormal = [
      this.screenBasisOrthonormal[0].slice(),
      this.screenBasisOrthonormal[1].slice()
    ];
  }

  showMessages() {
    if (!this.flagShowPerformance) {
      return;
    }
    if (this.spanMessages === null || this.spanMessages === undefined) {
      return;
    }
    this.spanMessages.textContent = '';
    if (this.textPerformance !== '') {
      this.spanMessages.appendChild(
        document.createTextNode(this.textPerformance));
    }
    if (this.textErrors !== '') {
      this.spanMessages.appendChild(document.createTextNode(this.textErrors));
    }
    this.spanMessages.appendChild(document.createTextNode(this.textMouseInfo));
    this.spanMessages.appendChild(
      document.createTextNode(this.textProjectionInfo));

    if (this.textPatchInfo !== '') {
      this.spanMessages.appendChild(
        document.createTextNode(this.textPatchInfo));
    }
  }

  infoPatchesCompute() {
    this.textPatchInfo = '';
    this.textPatchInfo += 'Z-depth: ' + this.boundingSegmentZ + '<br>';
    let thePatches = this.theIIIdObjects.thePatches;
    for (let i = 0; i < thePatches.length; i++) {
      let currentPatch = thePatches[i];
      for (let j = 0; j < currentPatch.patchesAboveMe.length; j++) {
        this.textPatchInfo += currentPatch.patchesAboveMe[j];
        if (j !== currentPatch.patchesAboveMe.length - 1) {
          this.textPatchInfo += ', ';
        } else {
          this.textPatchInfo += '->';
        }
      }
      this.textPatchInfo += `<b>${i}</b>`;
      if (currentPatch.patchesBelowMe.length > 0) {
        this.textPatchInfo += '->';
      }
      for (let j = 0; j < currentPatch.patchesBelowMe.length; j++) {
        this.textPatchInfo += currentPatch.patchesBelowMe[j];
        if (j !== currentPatch.patchesBelowMe.length) {
          this.textPatchInfo += ', ';
        }
      }
      this.textPatchInfo += '; contours: ';
      for (let j = 0; j < currentPatch.adjacentContours.length; j++) {
        this.textPatchInfo += currentPatch.adjacentContours[j];
        if (j !== currentPatch.adjacentContours.length - 1) {
          this.textPatchInfo += ', ';
        }
      }
      if (i != thePatches.length - 1) {
        this.textPatchInfo += '<br>';
      }
    }
    this.textPatchInfo +=
      '<style>#patchInfo{ border: 1px solid black;}</style>';
    this.textPatchInfo += '<table id ="patchInfo">';
    for (let i = this.zBuffer.length - 1; i >= 0; i--) {
      this.textPatchInfo += '<tr id ="patchInfo">';
      for (let j = 0; j < this.zBuffer[i].length; j++) {
        this.textPatchInfo += '<td id ="patchInfo">';
        for (let k = 0; k < this.zBuffer[i][j].length; k++) {
          this.textPatchInfo += this.zBuffer[i][j][k];
          if (k !== this.zBuffer[i][j].length - 1) {
            this.textPatchInfo += ', ';
          }
        }
        this.textPatchInfo += '</td>';
      }
      this.textPatchInfo += '</tr>';
    }
    this.textPatchInfo += '</table>';
  }

  infoMouseCompute() {
    if (!this.flagShowPerformance) {
      return;
    }
    this.textMouseInfo = '';
    let thePatches = this.theIIIdObjects.thePatches;
    if (this.numAccountedPatches < thePatches.length) {
      this.textMouseInfo += `<span style ='color:red'><b>Error: only ${this.numAccountedPatches} out of `;
      this.textMouseInfo += `${this.theIIIdObjects.thePatches
          .length} patches accounted. </b></span><br>`;
    }
    this.textMouseInfo += `time last redraw: ${this.redrawTime} ms `;
    this.textMouseInfo += `(~${(1000 / this.redrawTime).toFixed(1)} f.p.s.)`;
    this.textMouseInfo += `<br>selected element: ${this.selectedElement}`;
    this.textMouseInfo +=
      `<br>mouse coordinates: ${vectorToString(this.screenXY)}`;
    this.textMouseInfo +=
      `<br>2d coordinates: ${vectorToString(this.mousePosition)}`;
    this.textMouseInfo += `<br>clicked coordinates: ${this.clickedPosition}`;
    this.textMouseInfo += `<br>delta of position: ${this.positionDelta}`;
    this.textMouseInfo += `<br>ray component of mouse: ${this.rayComponent}`;
    this.textMouseInfo += `<br>selected vector: ${this.selectedVector}`;
    this.textMouseInfo +=
      `<br>normal angle change: ${this.angleNormal.toFixed(3)}`;
    this.textMouseInfo +=
      ` (${(this.angleNormal * 180 / Math.PI).toFixed(1)} deg)`;
    this.textMouseInfo += ` = ${this.oldAngleNormal.toFixed(3)}`;
    this.textMouseInfo +=
      ` (${(this.oldAngleNormal * 180 / Math.PI).toFixed(1)} deg)`;
    this.textMouseInfo += `- ${this.newAngleNormal.toFixed(3)} `;
    this.textMouseInfo +=
      `(${(this.newAngleNormal * 180 / Math.PI).toFixed(1)} deg)`;
  }

  redraw() {
    this.redrawStart = new Date().getTime();
    let theContours = this.theIIIdObjects.theContours;
    let thePatches = this.theIIIdObjects.thePatches;
    let thePoints = this.theIIIdObjects.thePoints;
    let theLabels = this.theIIIdObjects.theLabels;
    let theSurface = this.surface;
    theSurface.clearRect(0, 0, this.width, this.height);
    for (let i = 0; i < thePatches.length; i++) {
      this.computePatch(thePatches[i]);
    }
    let computePatchesTime = new Date().getTime();
    for (let i = 0; i < theContours.length; i++) {
      this.computeContour(theContours[i]);
    }
    let computeContoursTime = new Date().getTime();
    this.computeBuffers();
    let computeBuffersTime = new Date().getTime();
    // this.paintZbuffer();
    let paintBuffersTime = new Date().getTime();
    let numPainted = 0;
    for (let i = 0; i < this.thePatchOrder.length; i++) {
      if (this.thePatchOrder[i] !== -1) {
        this.paintOnePatch(thePatches[this.thePatchOrder[i]]);
        numPainted++;
      }
    }
    for (let i = 0; i < this.patchIsAccounted.length; i++) {
      if (this.patchIsAccounted[i] === 0) {
        this.paintOnePatch(thePatches[i]);
        numPainted++;
      }
    }
    let paintPatchTime = new Date().getTime();
    this.numContourPoints = 0;
    this.numContourPaths = 0;
    for (let i = 0; i < theContours.length; i++) {
      this.paintOneContour(theContours[i]);
    }
    let paintContourTime = new Date().getTime();
    for (let i = 0; i < thePoints.length; i++) {
      this.paintOnePoint(thePoints[i]);
    }
    for (let i = 0; i < theLabels.length; i++) {
      this.paintText(theLabels[i]);
    }
    this.paintMouseInfo();
    let redrawTime = new Date().getTime();
    if (this.flagShowPerformance) {
      this.textPerformance = '';

      this.textPerformance +=
        `Redraw time (ms): ${(redrawTime - this.redrawStart)}=<br>`;
      this.textPerformance +=
        `${computePatchesTime - this.redrawStart} (compute patches) + `;
      this.textPerformance +=
        (computeContoursTime - computePatchesTime) + ' (compute contours) + ';
      this.textPerformance +=
        (computeBuffersTime - computeContoursTime) + ' (compute buffers) + ';
      this.textPerformance +=
        (paintBuffersTime - computeBuffersTime) + '<br>(paint buffers) + ';
      this.textPerformance += (paintPatchTime - paintBuffersTime) +
        '<br> (paint ' + numPainted + ' patche(s)) + <br>';
      this.textPerformance += (paintContourTime - paintPatchTime) + ' (paint ' +
        theContours.length + ' contour(s) with ';
      this.textPerformance += this.numContourPoints + ' points in ' +
        this.numContourPaths + ' sub-paths) + ';
      this.textPerformance +=
        (redrawTime - paintContourTime) + ' (paint all else). ';
    }
    this.showMessages();
  }

  drawSurfaceCreate(
    inputxyzFun,
    inputUVBox,
    inputPatchDimensions,
    colorFront,
    colorBack,
    colorContour,
    inputContourWidth,
  ) {
    this.drawSurface(new Surface(
      inputxyzFun,
      inputUVBox,
      inputPatchDimensions,
      colorFront,
      colorBack,
      colorContour,
      inputContourWidth,
    ));
  }

  drawSurface(
    /**@type{Surface} */
    surface,
  ) {
    let numUsegments = surface.patchDimensions[0];
    let numVsegments = surface.patchDimensions[1];
    let thePatches = this.theIIIdObjects.thePatches;
    let theContours = this.theIIIdObjects.theContours;
    // let incomingPatches = new Array(numUsegments);
    let deltaU = surface.deltaU;
    let deltaV = surface.deltaV;
    let firstPatchIndex = thePatches.length;
    for (let i = 0; i < numUsegments; i++) {
      // incomingPatches[i] = new Array(numVsegments);
      for (let j = 0; j < numVsegments; j++) {
        // let incomingPatch = incomingPatches[i][j];
        let currentU = surface.uvBox[0][0] + i * deltaU;
        let currentV = surface.uvBox[1][0] + j * deltaV;
        let base = surface.xyzFun(currentU, currentV);
        let v1 = surface.xyzFun(currentU + deltaU, currentV);
        let v2 = surface.xyzFun(currentU, currentV + deltaV);
        let edge1 = vectorMinusVector(v1, base);
        let edge2 = vectorMinusVector(v2, base);
        let incomingPatch = new Patch(
          base, edge1, edge2, surface.colors.colorUV, surface.colors.colorVU);
        incomingPatch.adjacentContours = new Array(4);
        incomingPatch.traversalOrder = [1, 1, 1, 1];
        incomingPatch.index = thePatches.length;
        incomingPatch.internalPoint =
          surface.xyzFun(currentU + deltaU / 2, currentV + deltaV / 2);
        thePatches.push(incomingPatch);
      }
    }
    let numSegmentsPerContour = this.defaultNumSegmentsPerContour;
    let contourPoints = new Array(numSegmentsPerContour + 1);
    for (let i = 0; i < numUsegments + 1; i++) {
      for (let j = 0; j < numVsegments; j++) {
        let currentU = surface.uvBox[0][0] + i * deltaU;
        for (let k = 0; k < numSegmentsPerContour + 1; k++) {
          let currentV =
            surface.uvBox[1][0] + (j + k / numSegmentsPerContour) * deltaV;
          contourPoints[k] = surface.xyzFun(currentU, currentV);
        }
        let incomingContour = new Contour(
          contourPoints, surface.colors.colorContour, surface.contourWidth);
        incomingContour.index = theContours.length;
        if (i > 0) {
          incomingContour.adjacentPatches.push(
            firstPatchIndex + numVsegments * (i - 1) + j);
          thePatches[firstPatchIndex + numVsegments * (i - 1) + j]
            .adjacentContours[2] = theContours.length;
          thePatches[firstPatchIndex + numVsegments * (i - 1) + j]
            .traversalOrder[2] = -1;
        }
        if (i < numUsegments) {
          incomingContour.adjacentPatches.push(
            firstPatchIndex + numVsegments * i + j);
          thePatches[firstPatchIndex + numVsegments * i + j]
            .adjacentContours[0] = theContours.length;
        }
        theContours.push(incomingContour);
      }
    }
    for (let i = 0; i < numUsegments; i++) {
      for (let j = 0; j < numVsegments + 1; j++) {
        let currentV = surface.uvBox[1][0] + j * deltaV;
        for (let k = 0; k < numSegmentsPerContour + 1; k++) {
          let currentU =
            surface.uvBox[0][0] + (i + k / numSegmentsPerContour) * deltaU;
          contourPoints[k] = surface.xyzFun(currentU, currentV);
        }
        let incomingContour = new Contour(
          contourPoints, surface.colors.colorContour, surface.contourWidth);
        incomingContour.index = theContours.length;
        if (j > 0) {
          incomingContour.adjacentPatches.push(
            firstPatchIndex + numVsegments * i + j - 1);
          thePatches[firstPatchIndex + numVsegments * i + j - 1]
            .adjacentContours[1] = theContours.length;
        }
        if (j < numVsegments) {
          incomingContour.adjacentPatches.push(
            firstPatchIndex + numVsegments * i + j);
          thePatches[firstPatchIndex + numVsegments * i + j]
            .adjacentContours[3] = theContours.length;
          thePatches[firstPatchIndex + numVsegments * i + j].traversalOrder[3] =
            -1;
        }
        theContours.push(incomingContour);
      }
    }
  }
}

class Drawing {
  constructor() {
    this.firstCriticalRunTimeError = '';
    this.firstCanvas = null;
    this.numberOfControlsConstructed = 0;
    this.plotUpdaters = {};
    this.Surface = Surface;
    this.CurveThreeD = CurveThreeD;
  }

  calculatorError(
    /**@type{string} */
    x,
  ) {
    console.log(x);
  }

  /**@returns{{delta:number, x:number, y:number}} */
  mouseWheelCommon(e) {
    e.preventDefault();
    e.stopPropagation();
    let theWheelDelta = e.detail ? e.detail * -1 : e.wheelDelta / 40;
    let theIncrement = 0.6;
    return {
      delta: theWheelDelta * theIncrement,
      x: e.clientX,
      y: e.clientY,
    };
  }
}

let drawing = new Drawing();

/**
 * When using as a commonJS module inside the calculator project, the content of
 * this file is inserted into a function body, which receives as an argument a
 * variable called module. Modifications to module.exports are then visible to
 * the caller of this functions, i.e., to external javascript files.
 */
var module;
if (module === undefined) {
  module = {
    exports: null,
  };
}

module.exports = {
  drawing,
  Canvas,
  CanvasTwoD,
};
